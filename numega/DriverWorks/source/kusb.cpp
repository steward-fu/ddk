// kusb.cpp - functions for USB classes
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#include <vdw.h>

#if _WDM_

#include <kusb.h>

////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::SubmitUrb - this form builds a new IRP
//
// Parameters
//	pUrb					Pointer to URB to submit
//	CompletionRoutine		IRP completion routine, or NULL
//	Context					IRP completion context
//	mSecTimeOut				Timeout period for synchronous requests
//
// If the caller provides a completion routine, that completion routine
// must NOT deallocate the IRP that this function allocates to submit
// the URB. The return value of the completion routine is ignored.
//
NTSTATUS KUsbLowerDevice::SubmitUrb(
	PURB pUrb,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context,
	ULONG mSecTimeOut
	)
{
	NTSTATUS status;

	// Allocate and initialize the IRP for IOCTL request
	KIrp I = KIrp::Allocate(StackRequirement());

	if (I.IsNull())
	{
		if ( CompletionRoutine )
		{
			IRP irp;
			memset(&irp, 0, sizeof(IRP));
			irp.IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
			CompletionRoutine(PDO(), &irp, Context);
		}

		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Setup Irp
	I.MajorFunction(NEXT) = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	I.IoctlCode(NEXT) = IOCTL_INTERNAL_USB_SUBMIT_URB;

	// Put the address of the URB in the IRP
	I.Urb(NEXT) = pUrb;

	// If completion routine is not NULL we will be calling asynchronously,
	// otherwise we will be calling synchronously.
	if (CompletionRoutine != NULL)
	{
		// Allocate context structure, which will hold the user's completion routine
		AsyncCompleteSubmitUrbContext_t* pCtx =
			new (NonPagedPool) AsyncCompleteSubmitUrbContext_t;
		if (pCtx == NULL)
		{
			I.Status()=STATUS_INSUFFICIENT_RESOURCES;
			CompletionRoutine(PDO(), I, Context);
			I.Deallocate(I);
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "SubmitUrb", sizeof(AsyncCompleteSubmitUrbContext_t)));
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		// Initialize context structure with user's completion routine and context
		pCtx->CompletionRoutine = CompletionRoutine;
		pCtx->Context = Context;
#if DBG
		pCtx->pLD = this;
		pCtx->pUrb = pUrb;
#endif

		I.SetCompletionRoutine(AsyncCompleteSubmitUrb, pCtx);

		// Pass the IRP/URB to USBD
		BOUNDS_CHECKER(SUBMIT_URB, (this, pUrb, PIRP(I)));
		status = Call(I);
	}
	else // synchronous case
	{
		// We will wait on an event, so we must be at PASSIVE_LEVEL
		ASSERT (KeGetCurrentIrql() == PASSIVE_LEVEL);

		// Careful not call this ctor if CompletionRoutine is not NULL -
		// because caller may be at DISPATCH_LEVEL, and this ctor is in
		// pageable code
		KEvent Synch(NotificationEvent, FALSE);

		// Allocate and initialize context structure on stack
		SyncCompleteSubmitUrbContext_t Ctx;
		Ctx.pEvent = &Synch;
#if DBG
		Ctx.pLD = this;
		Ctx.pUrb = pUrb;
#endif
		// Set up completion routine
		I.SetCompletionRoutine(SyncCompleteSubmitUrb, &Ctx);


		// Pass the IRP/URB to USBD
		BOUNDS_CHECKER(SUBMIT_URB, (this, pUrb, PIRP(I)));
		status = Call(I);

		if ( status == STATUS_PENDING )
		{
			// calculate the time out interval
			ULONGLONG TimeOut100nSec;
			PLARGE_INTEGER pTimeOut;

			if ( mSecTimeOut != 0 )
			{
				// the input value may be negative or positive. Make sure
				// it's positive, and then convert to negative afterwards.
				LONG mSecTimeOutSigned = LONG(mSecTimeOut);
				if (mSecTimeOutSigned < 0)
					mSecTimeOutSigned = -mSecTimeOutSigned;
				mSecTimeOut = ULONG(mSecTimeOutSigned);

				// Calculate wait as 100ns intervals.  Negative is relative
				TimeOut100nSec = mSecTimeOut;
				TimeOut100nSec *= -10000;	// convert units
				pTimeOut = PLARGE_INTEGER(&TimeOut100nSec);
			}
			else
				pTimeOut = NULL;

			// Wait for the event to be signaled, or timeout if specified
			status = Synch.Wait(KernelMode, FALSE, pTimeOut);
			if (status != STATUS_SUCCESS)
			{
				BOUNDS_CHECKER(URB_TIMEOUT, (this, pUrb));

				IoCancelIrp(I);
				// Cancelling the IRP should result in a speedy completion. We have
				// to wait because we don't want the completion routine to run
				// after the event (Synch) has gone out of scope.
				Synch.Wait(KernelMode, FALSE, NULL);
			}
			else
				status = Ctx.Status;
		}	// end STATUS_PENDING returned clause

		BOUNDS_CHECKER(COMPLETED_URB, (this, pUrb, PIRP(I), status));

		// Deallocate Irp
		I.Deallocate(I);

	} // End synchronous case

	return status;
}

////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::SyncCompleteSubmitUrb
//
// This completion routine signals an event to allow execution to
// resume in the case of a synchronous SubmitUrb.
//
// Parameters
//	DeviceObject		pointer to device object
//	pIrp				Irp that just completed
//	Context				Context structure for Irp to be completed
//
NTSTATUS KUsbLowerDevice::SyncCompleteSubmitUrb(
	PDEVICE_OBJECT	DeviceObject,
	PIRP			pIrp,
	PVOID			Context
	)
{
	SyncCompleteSubmitUrbContext_t* pCtx =
		(SyncCompleteSubmitUrbContext_t*)Context;

	BOUNDS_CHECKER(COMPLETED_URB, (pCtx->pLD, pCtx->pUrb, pIrp, pIrp->IoStatus.Status));

	// load up the context structure
	pCtx->Status = pIrp->IoStatus.Status;

	// signal the event to release waiting thread
	(pCtx->pEvent)->Set();

	return STATUS_MORE_PROCESSING_REQUIRED;
	UNREFERENCED_PARAMETER(DeviceObject);
}

////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::AsyncCompleteSubmitUrb
//
// This completion routine calls the user's completion routine,
// which is stored in the context structure.  It then deallocates
// the context structure and the Irp that was allocated by the
// SubmitUrb() method.
//
// Parameters
//	DeviceObject		pointer to device object
//	Irp					Irp that just completed
//	Context				Context structure for Irp to be completed
//
NTSTATUS
KUsbLowerDevice::AsyncCompleteSubmitUrb(
	PDEVICE_OBJECT	DeviceObject,
	PIRP			Irp,
	PVOID			Context
	)
{
	AsyncCompleteSubmitUrbContext_t* pCtx =
		(AsyncCompleteSubmitUrbContext_t*)Context;

	BOUNDS_CHECKER(COMPLETED_URB, (pCtx->pLD, pCtx->pUrb, Irp, Irp->IoStatus.Status));

	// Call the completion routine stored in the context structure.
	// In this case, the user originally supplied a completion routine and
	// context but no Irp.  The status returned by the completion routine is
	// useless since the Irp was allocated by SubmitUrb() and is
	// about to be deallocated.  The completion routine must propagate
	// any result of the Urb in some (user defined) manner other than
	// its return value.
	pCtx->CompletionRoutine( DeviceObject, Irp, pCtx->Context );

	// Deallocate Irp
	KIrp::Deallocate(KIrp(Irp));

	// Deallocate the context structure
	delete pCtx;

	// Return STATUS_MORE_PROCESSING_REQUIRED so the IO subsystem doesn't
	// try to deal with this Irp.
	return STATUS_MORE_PROCESSING_REQUIRED;
}

////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::SubmitUrb - this form uses a provided IRP to pass the URB
//
// Parameters
//	I					IRP to attach URB to
//	pUrb				URB to submit
//	CompletionRoutine	IRP Completion routine (NULL for synchronous call)
//	Context				Completion context
//	mSecTimeOut			Timeout period for synchronous requests
//
NTSTATUS KUsbLowerDevice::SubmitUrb(
	KIrp I,
	PURB pUrb,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context,
	ULONG mSecTimeOut
	)
{
	NTSTATUS status;

	// If no completion routine is specified, call synchronously
	if (CompletionRoutine == NULL)	// synchronous case
	{
		// Careful not call this ctor if CompletionRoutine is not NULL -
		// because caller may be at DISPATCH_LEVEL, and this ctor is in
		// pageable code
		KEvent Synch(NotificationEvent, FALSE);

		// Since we will block if there is no completion routine,
		// we must be at PASSIVE_LEVEL
		ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

		// Allocate and initialize context structure on stack
		SyncCompleteSubmitUrbContext_t Ctx;
		Ctx.pEvent = &Synch;
#if DBG
		Ctx.pLD = this;
		Ctx.pUrb = pUrb;
#endif
		// Set up completion routine
		I.SetCompletionRoutine(SyncCompleteSubmitUrb, &Ctx);

		I.MajorFunction(NEXT) = IRP_MJ_INTERNAL_DEVICE_CONTROL;
		I.IoctlCode(NEXT) = IOCTL_INTERNAL_USB_SUBMIT_URB;
		I.Urb(NEXT) = pUrb;

		// Pass the IRP/URB to USBD
		BOUNDS_CHECKER(SUBMIT_URB, (this, pUrb, PIRP(I)));
		status = Call(I);

		if ( status == STATUS_PENDING )
		{
			ULONGLONG TimeOut100nSec;
			PLARGE_INTEGER pTimeOut;

			if ( mSecTimeOut != 0 )
			{
				// the input value may be negative or positive. Make sure
				// it's positive, and then convert to negative afterwards.
				LONG mSecTimeOutSigned = LONG(mSecTimeOut);
				if (mSecTimeOutSigned < 0)
					mSecTimeOutSigned = -mSecTimeOutSigned;
				mSecTimeOut = ULONG(mSecTimeOutSigned);

				// Calculate wait as 100ns intervals.  Negative is relative
				TimeOut100nSec = mSecTimeOut;
				TimeOut100nSec *= -10000;	// convert units
				pTimeOut = PLARGE_INTEGER(&TimeOut100nSec);
			}
			else
				pTimeOut = NULL;

			// If the IRP times out, then cancel it
			status = Synch.Wait(KernelMode, FALSE, pTimeOut);
			if (status != STATUS_SUCCESS)
			{
				BOUNDS_CHECKER(URB_TIMEOUT, (this, pUrb));

				IoCancelIrp(I);
				// Cancelling the IRP should cause it to be completed quickly. We must wait
				// because the completion context and the event are allocated on the stack.
				// If USBD does not complete it, the thread is hung.
				Synch.Wait(KernelMode, FALSE, NULL);
			}
			else
				status = Ctx.Status;
		}

		BOUNDS_CHECKER(COMPLETED_URB, (this, pUrb, PIRP(I), status));

		// THe completion routine suspended completion. Now proceed with the remaining
		// stack locations.
		I.Complete(status);
	}
	else // asynch case
	{
		I.SetCompletionRoutine(CompletionRoutine, Context, TRUE, TRUE, TRUE);

		// Set up the next stack location
		I.MajorFunction(NEXT) = IRP_MJ_INTERNAL_DEVICE_CONTROL;
		I.IoctlCode(NEXT) = IOCTL_INTERNAL_USB_SUBMIT_URB;

		I.Urb(NEXT) = pUrb;

		// Pass the IRP/URB to USBD
		BOUNDS_CHECKER(SUBMIT_URB, (this, pUrb, PIRP(I)));
		status = Call(I);
	}

	return status;
}

//////////////////////////////////////////////////////////////////////////////
// _get_status
//
// This is function used by members GetStatus of KUsbPipe and KUsbInterface
//
NTSTATUS KUsbLowerDevice::_get_status(
	UCHAR RequestType,
	USHORT Index,
	PUSHORT pDeviceStatus,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID CompletionContext
	)
{
	ASSERT(
		(RequestType == URB_FUNCTION_GET_STATUS_FROM_DEVICE)    ||
		(RequestType == URB_FUNCTION_GET_STATUS_FROM_INTERFACE) ||
		(RequestType == URB_FUNCTION_GET_STATUS_FROM_ENDPOINT)  ||
		(RequestType == URB_FUNCTION_GET_STATUS_FROM_OTHER)
	);

	ASSERT (KeGetCurrentIrql() < DISPATCH_LEVEL);

	NTSTATUS status;
	PURB pUrb = new (NonPagedPool) URB;

	if ( !pUrb )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbLowerDevice::_get_status", sizeof(URB)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	UsbBuildGetStatusRequest(
		pUrb,
		RequestType,
		Index,
		pDeviceStatus,
		NULL,
		NULL
		);

	status = SubmitUrb(
		pUrb,
		CompletionRoutine,
		CompletionContext
		);

	if ( CompletionRoutine == NULL )
		delete pUrb;

	return status;
}

//////////////////////////////////////////////////////////////////////////////
// Set or Clear Feature
//
// This is a base class member used by members SetFeature, ClearFeature of
// KUsbPipe and KUsbInterface
//
NTSTATUS KUsbLowerDevice::_set_or_clear_feature(
	UCHAR RequestType,
	USHORT Index,
	USHORT Feature,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	ASSERT(
		(RequestType == URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE)    ||
		(RequestType == URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE) ||
		(RequestType == URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT)  ||
		(RequestType == URB_FUNCTION_CLEAR_FEATURE_TO_OTHER)     ||
		(RequestType == URB_FUNCTION_SET_FEATURE_TO_DEVICE)      ||
		(RequestType == URB_FUNCTION_SET_FEATURE_TO_INTERFACE)   ||
		(RequestType == URB_FUNCTION_SET_FEATURE_TO_ENDPOINT)    ||
		(RequestType == URB_FUNCTION_SET_FEATURE_TO_OTHER)
	);

	NTSTATUS status;
	PURB pUrb = new (NonPagedPool) URB;

	if ( !pUrb )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbLowerDevice::_set_or_clear_feature", sizeof(URB)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	UsbBuildFeatureRequest(
		pUrb,
		RequestType,
		Feature,
		Index,
		NULL
		);

	status = SubmitUrb(
		pUrb,
		CompletionRoutine,
		Context
		);

	if ( CompletionRoutine == NULL )
		delete pUrb;

	return status;
}

//////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::~KUsbLowerDevice
//
// Destructor for KUsbLowerdevice
//
KUsbLowerDevice::~KUsbLowerDevice(void)
{
	ReleaseResources();
}

//////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::ReleaseResources
//
void KUsbLowerDevice::ReleaseResources(void)
{
	m_IntfArray.Invalidate();
	m_PipeArray.Invalidate();

	if( m_Config )
	{
		delete m_Config;
		m_Config = NULL;
	}

	if( m_ConfiguredInterfaces )
	{
		delete m_ConfiguredInterfaces;
		m_ConfiguredInterfaces = NULL;
	}

	PreconfiguredInterfaceListEntry* pLE;
	PreconfiguredInterfaceListEntry* pNext;

	for( pLE = m_InterfaceList.Head(); pLE != NULL; pLE = pNext)
	{
		pNext = m_InterfaceList.Next(pLE);
		delete pLE;
	}

	m_InterfaceList.Reset();
}

//////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::GetDeviceDescriptor
//
// This member retrieves the device descriptor
//
// Synchronous call - must be called at IRQL==PASSIVE_LEVEL
//
NTSTATUS KUsbLowerDevice::GetDeviceDescriptor(PUSB_DEVICE_DESCRIPTOR pDevDesc)
{
	URB u;

	ASSERT (KeGetCurrentIrql() < DISPATCH_LEVEL);

	UsbBuildGetDescriptorRequest(
		&u,
		sizeof(_URB_CONTROL_DESCRIPTOR_REQUEST),
		USB_DEVICE_DESCRIPTOR_TYPE,
		0,
		0,
		pDevDesc,
		PMDL(0),
		sizeof(USB_DEVICE_DESCRIPTOR),
		NULL
		);

	return SubmitUrb(&u);
}


//////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::GetStringDescriptor
//
// This member retrieves a string descriptor.
//
// Parameters
//	Index		The index of the string to retrieve
//	pStr		Pointer to a buffer to receive the string
//  MaxLen		Size of the buffer that pStr points to.
//  LangId		Optional language ID.  Defaults to English/American.
//
// Synchronous call - must be called at IRQL==PASSIVE_LEVEL
//
NTSTATUS KUsbLowerDevice::GetStringDescriptor(
	UCHAR	Index,
	PWCHAR	pStr,
	UCHAR	MaxLen,
	SHORT	LangId
	)
{
	ASSERT (KeGetCurrentIrql() < DISPATCH_LEVEL);

	URB u;
	USB_STRING_DESCRIPTOR StringDescriptor;
    PUCHAR StringDescReqBuf = NULL;
 	NTSTATUS status = STATUS_SUCCESS;
	ULONG nBytes = sizeof(StringDescriptor);

	memset(&StringDescriptor, 0, nBytes);

	UsbBuildGetDescriptorRequest(
		&u,
		sizeof(_URB_CONTROL_DESCRIPTOR_REQUEST),
		USB_STRING_DESCRIPTOR_TYPE,
		Index,
		LangId,
		&StringDescriptor,
		PMDL(0),
		nBytes,
		NULL
		);

	status = SubmitUrb(&u);

	if (NT_SUCCESS(status))
	{
		StringDescReqBuf = new(NonPagedPool) UCHAR[StringDescriptor.bLength];

		if (StringDescReqBuf == NULL)
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "GetStringDescriptor", StringDescriptor.bLength));
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		UsbBuildGetDescriptorRequest(
			&u,
			sizeof(_URB_CONTROL_DESCRIPTOR_REQUEST),
			USB_STRING_DESCRIPTOR_TYPE,
			Index,
			LangId,
			StringDescReqBuf,
			PMDL(0),
			StringDescriptor.bLength,
			NULL
			);

		status = SubmitUrb(&u);
	}
	else
	{
		return status;
	}

	PUSB_STRING_DESCRIPTOR pStringDescriptor = (PUSB_STRING_DESCRIPTOR) StringDescReqBuf;

	if (NT_SUCCESS(status))
	{
		UCHAR StringLength = UCHAR(pStringDescriptor->bLength - 2);
		UCHAR nBytesToCopy = StringLength;

		if ( StringLength >= MaxLen - sizeof(WCHAR) )
		{
			nBytesToCopy = UCHAR(MaxLen - sizeof(WCHAR));
			status = STATUS_BUFFER_TOO_SMALL;
		}

		RtlCopyBytes(pStr, pStringDescriptor->bString, nBytesToCopy);
		pStr[nBytesToCopy / 2] = USHORT(0x00);
	}

	delete[] StringDescReqBuf;

	return status ;

}


//////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::Preconfigure
//
// This member obtains a particular configuration descriptor from the device.
//
// Synchronous call - must be called at IRQL==PASSIVE_LEVEL
//
NTSTATUS KUsbLowerDevice::Preconfigure(
	UCHAR ConfigIndex,
	ULONG MaxConfigSize
	)
{
	URB u;

	if (m_Config)
	{
		delete m_Config;
		m_Config = NULL;
	}

	m_Config = (PUSB_CONFIGURATION_DESCRIPTOR) new (NonPagedPool) UCHAR[MaxConfigSize];

	if ( !m_Config )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbLowerDevice::Preconfigure", MaxConfigSize));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	UsbBuildGetDescriptorRequest(
		&u,
		(USHORT) sizeof (_URB_CONTROL_DESCRIPTOR_REQUEST),
		USB_CONFIGURATION_DESCRIPTOR_TYPE,
		ConfigIndex,
		0,		// language
		m_Config,
		NULL, 	// mdl
		MaxConfigSize,
		NULL	// link
		);

	return SubmitUrb(&u);	// synchronous
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::LocateInterface
//
// This member function locates a particular interface in the configuration.
// Preconfigure must be called prior to calling this function.
//
PUSB_INTERFACE_DESCRIPTOR KUsbLowerDevice::LocateInterface(
	PVOID* pStart,
	PUSB_ENDPOINT_DESCRIPTOR* ppEndpoints,
	LONG InterfaceNumber,
	LONG AlternateSetting,
	LONG InterfaceClass,
	LONG InterfaceSubClass,
	LONG InterfaceProtocol
	)
{
	PUSB_INTERFACE_DESCRIPTOR pDesc;

	ASSERT (m_Config);

	if ( *pStart == NULL )
		*pStart = m_Config;

	pDesc = USBD_ParseConfigurationDescriptorEx(
		m_Config,
		*pStart,
		InterfaceNumber,
		AlternateSetting,
		InterfaceClass,
		InterfaceSubClass,
		InterfaceProtocol
		);

	*pStart = &pDesc[1];
	PUSB_ENDPOINT_DESCRIPTOR ped = NULL;

	if (pDesc && pDesc->bNumEndpoints)
	{
		ped = (PUSB_ENDPOINT_DESCRIPTOR)&pDesc[1];

		while ( ped->bDescriptorType != 5 )
		{
			if (0 == ped->bLength)
				break;
			ped = (PUSB_ENDPOINT_DESCRIPTOR) (ped->bLength + (PCHAR)ped);
		}
		if (0 == ped->bLength)
			ped = NULL;
	}

	*ppEndpoints = ped;
	return pDesc;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::PreconfigureInterface
//
// After calling Preconfigure, a driver calls this member for each Interface
// that the driver will support. A call to member Configure then sets the
// configuration.
//
NTSTATUS KUsbLowerDevice::PreconfigureInterface(
	PUSB_INTERFACE_DESCRIPTOR pInterfaceDesc
	)
{
	PreconfiguredInterfaceListEntry* pLE;

	//calculate size of PreconfiguredInterfaceListEntry
	ULONG size = GET_INTF_LIST_ENTRY_SIZE(pInterfaceDesc->bNumEndpoints);

	pLE = (PreconfiguredInterfaceListEntry*) new (NonPagedPool) UCHAR[size];

	if ( !pLE )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "PreconfigureInterface", size));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	InitializePreConfiguredInterface( pInterfaceDesc, pLE);

	m_InterfaceList.InsertTail(pLE);

	return STATUS_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::Configure
//
// This function takes the preconfigured configuration and interfaces, builds a
// configuration request and submits it to USBDI.
//
NTSTATUS KUsbLowerDevice::Configure(
	ULONG nEndpoints,
	USB_ENDPOINT* Endpoints,
	PUSB_ENDPOINT_DESCRIPTOR pFirstDescriptor
	)
{
	PreconfiguredInterfaceListEntry* pLE;
	ULONG InterfaceCount;
	ULONG i,j,k;
	NTSTATUS status;
	PURB pConfigUrb;

	ASSERT ( m_Config != NULL );

	InterfaceCount = m_InterfaceList.Count();

	ASSERT (InterfaceCount != 0);

// allocate an array of USBD_INTERFACE_LIST_ENTRY to be used to constuct the
// configuration request URB.

	if( m_ConfiguredInterfaces )
	{
		ASSERT( !m_ConfiguredInterfaces );
		delete m_ConfiguredInterfaces;
		m_ConfiguredInterfaces = NULL;
	}

	m_ConfiguredInterfaces =
		new (NonPagedPool) USBD_INTERFACE_LIST_ENTRY[InterfaceCount+1]; // null terminated

	if ( m_ConfiguredInterfaces == NULL )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbLowerDevice::Configure", sizeof(USBD_INTERFACE_LIST_ENTRY)*(InterfaceCount+1)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

// now set up each USBD_INTERFACE_LIST_ENTRY

	for (i=0, pLE=m_InterfaceList.Head(); i < InterfaceCount; i++)
	{
		m_ConfiguredInterfaces[i].InterfaceDescriptor = pLE->m_Descriptor;
		m_ConfiguredInterfaces[i].Interface = &pLE->m_InterfaceInformation;
		pLE = m_InterfaceList.Next(pLE);
	}

// mark the end of the array

	m_ConfiguredInterfaces[InterfaceCount].InterfaceDescriptor = NULL;
	m_ConfiguredInterfaces[InterfaceCount].Interface = NULL;

// now build the configuration request URB

	pConfigUrb = USBD_CreateConfigurationRequestEx(m_Config, m_ConfiguredInterfaces);
	if ( pConfigUrb == NULL )
	{
		delete m_ConfiguredInterfaces;
		m_ConfiguredInterfaces = NULL;
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "alloc config URB", sizeof(URB)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

// Scan the endpoints array to see if any of the pipes have a preset Maximum Transfer Size


	for (i=0; i < nEndpoints; i++)
	{
// if this endpoint does not match the current config, skip it

		if ( (Endpoints[i].m_ConfigurationValue != 0) &&
			 (Endpoints[i].m_ConfigurationValue != m_Config->bConfigurationValue))
			continue;

// If there is no pipe object associated with the endpoint, or if the pipe object's
// max transfer size has not been preconfigured, then skip it.

		if ( (Endpoints[i].m_pPipeObject == NULL) ||
		     (Endpoints[i].m_pPipeObject->m_SetMaxTransferSize == 0) )
			 continue;

// find the interface for this endpoint
	if(pFirstDescriptor)
	{
		for (j=0; j < InterfaceCount; j++)
		{
			if ( m_ConfiguredInterfaces[j].Interface->InterfaceNumber == Endpoints[i].m_InterfaceNumber )
			{

// found the interface, now find the pipe by endpoint address

				PUSBD_PIPE_INFORMATION pPipeInfo = m_ConfiguredInterfaces[j].Interface->Pipes;
				ULONG nPipes = m_ConfiguredInterfaces[j].Interface->NumberOfPipes;

				PUSB_ENDPOINT_DESCRIPTOR pEndptDesc = pFirstDescriptor;
				for (k = 0; k < nPipes; k++, pPipeInfo++, pEndptDesc++)
				{
// if we find the matching endpoint address, and the there is a pipe object associated with this endpoint,
// and there has been a previous call to SetMaximumTransferSize (size != 0) then set the MaximumTransferSize
// in the USBD_PIPE_INFORMATION struct before we submit the URB for configuration.

					if (pEndptDesc->bEndpointAddress == Endpoints[i].m_EndpointAddress)
					{
						pPipeInfo->MaximumTransferSize = Endpoints[i].m_pPipeObject->m_SetMaxTransferSize;
						break;
					}
				}
			}
		}
	}//end if(pFirstDescriptor)
	}

// Now submit the URB

	status = SubmitUrb(pConfigUrb);

	BOUNDS_CHECKER(CONFIGURED_USB_DEVICE, (this, status));

// Copy the configuration memory from the URB back into the object's data structures

	if (NT_SUCCESS(status))
	{
		m_ConfigurationHandle =
			((_URB_SELECT_CONFIGURATION*)pConfigUrb)->ConfigurationHandle;

		for (i=0, pLE=m_InterfaceList.Head(); i < InterfaceCount; i++)
		{
			RtlCopyMemory(
				&pLE->m_InterfaceInformation,
				m_ConfiguredInterfaces[i].Interface,
				m_ConfiguredInterfaces[i].Interface->Length
				);

			m_ConfiguredInterfaces[i].Interface = &pLE->m_InterfaceInformation;
			pLE = m_InterfaceList.Next(pLE);
		}
	}

// release the URB

	ExFreePool(pConfigUrb);

	return status;
}
/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::ActivateConfiguration
//
// This function automates the configuration process. The input is
//
//	ConfigurationValue		Specifies the configuration value of the
//							configuration to activate. The value should
//							match the ConfigurationValue field
//							in the device's configuration descriptor (the USB
//							spec defines this field as starting with 1 - it
//							is not zero based).  If 0 is passed as this parameter
//							the first configuration is activated (config index 0)
//
//  nEndpointsDescribed		Count of entries in the Endpoints array (next param)
//
//  Endpoints				Array of endpoint specifiers. Each entry specifies
//							(see struct USB_ENDPOINT in kusb.h)
//							1. The configuration value indicating which
//							   configuration the endpoint appears in
//							2. The interface number, which is compared to the
//							   interface numbers in the device's interface
//							   descriptors for the selected configuration
//							3. The interface alternate setting value, which
//							   is compared to the alternate setting value
//							   in the device's interface descriptor for the
//							   selected configuration.
//							4. The endpoint address. Bit 7 (0x80) must be
//							   set to one if the endpoint is IN, and must
//							   be zero for an OUT endpoint.
//							5. A pointer to the corresponding KUsbInterface object
//							6. A pointer to the corresponding KUsbPipe object
//
//	ppProblem				Optional pointer to pointer to USB_ENDPOINT.  If an error
//							occurs during configuration, the offending USB_ENDPOINT
//							can be identified to the caller
//
//	MaxConfigSize			Optional parameter for the call to Preconfigure. Represents
//							the size of the configuration descriptor to retrieve.
//							Defaults to 1024
//
//	NOTE: To indicate a configuration that has an interface with no endpoints
//        (except endpoint 0, the control), put NULL in field m_pPipeObject of
//        the USB_ENDPOINT structure.
//
AC_STATUS KUsbLowerDevice::ActivateConfiguration(
	UCHAR ConfigurationValue,
	ULONG nEndpointsDescribed,
	USB_ENDPOINT* Endpoints,
	USB_ENDPOINT** ppProblem,
	ULONG MaxConfigSize
	)
{
	NTSTATUS status;
	AC_STATUS acstatus=AC_SUCCESS;
	UCHAR ThisInterface;
	UCHAR ThisAlternate;
	UCHAR ConfigIndex;
	PUSB_ENDPOINT_DESCRIPTOR pEndpoints = NULL;
	int i, j;

	// If the ConfigurationValue is zero, we simply take the device's first
	// configuration (index 0).  We "patch" the USB_ENDPOINT array with the
	// ConfigurationValue for the device's index 0 configuration.
	if ( ConfigurationValue == 0 )
	{
		ConfigIndex = 0;
		status = Preconfigure(ConfigIndex, MaxConfigSize);
		if ( !NT_SUCCESS(status) )
		{
			if (ppProblem) *ppProblem = NULL;
			acstatus = AC_NO_SUCH_CONFIGURATION;
			goto Exit;
		}

		ConfigurationValue = m_Config->bConfigurationValue;

		for (i = 0; i < (int)nEndpointsDescribed; i++)
		{
			if ( Endpoints[i].m_ConfigurationValue == 0 )
				Endpoints[i].m_ConfigurationValue = ConfigurationValue;
		}
	}
	else
	{
		USB_DEVICE_DESCRIPTOR desc;
		status = GetDeviceDescriptor( &desc );
		if ( !NT_SUCCESS(status) )
		{
			acstatus = AC_FAILED_TO_GET_DESCRIPTOR;
			goto Exit;
		}

		for ( ConfigIndex = 0; ConfigIndex < desc.bNumConfigurations; ConfigIndex++ )
		{
			status = Preconfigure(ConfigIndex, MaxConfigSize);
			if ( !NT_SUCCESS(status) )
			{
				if (ppProblem) *ppProblem = NULL;
				acstatus = AC_NO_SUCH_CONFIGURATION;
				goto Exit;
			}

			if ( m_Config->bConfigurationValue == ConfigurationValue )
				break;
		}

		if ( ConfigIndex == desc.bNumConfigurations )
		{
			acstatus = AC_NO_SUCH_CONFIGURATION;
			goto Exit;
		}
	}

	for (i = 0; i < (int)nEndpointsDescribed; i++)
	{
	// ignore interfaces not specified on this configuration

		if ( Endpoints[i].m_ConfigurationValue != ConfigurationValue )
			continue;

	// grab the interface number for the current endpoint

		ThisInterface = Endpoints[i].m_InterfaceNumber;
		ThisAlternate = Endpoints[i].m_InterfaceAlternate;

	// Make sure this is the first time we have seen this interface

		for (j = 0; j < i; j++)
		{
			if ( Endpoints[j].m_ConfigurationValue != ConfigurationValue )
				continue;

			if (Endpoints[j].m_InterfaceNumber == ThisInterface)
				break;
		}

	// if loop did not go to completion, the interface has been processed

		if (j < i)
			continue;

	// Look for a matching interface in the config descriptor

		PVOID Start=NULL;
		PUSB_INTERFACE_DESCRIPTOR pIntfDesc;

		pIntfDesc = LocateInterface(
			&Start,
			&pEndpoints,
			ThisInterface,
			ThisAlternate
			);

		if ( pIntfDesc == NULL )
		{
			if (ppProblem) *ppProblem = &Endpoints[i];
			status = STATUS_UNSUCCESSFUL;
			acstatus = AC_COULD_NOT_LOCATE_INTERFACE;
			break;
		}

	// The described interface was found. Now add it to the set of interfaces
	// that the driver will support in this configuration.

		status = PreconfigureInterface(pIntfDesc);
		if ( !NT_SUCCESS(status) )
		{
			if (ppProblem) *ppProblem = &Endpoints[i];
			acstatus = AC_COULD_NOT_PRECONFIGURE_INTERFACE;
			break;
		}
	}

	// Test for normal completion of the above loop

	if ( !NT_SUCCESS(status) )
		goto Exit;

	// Now set the configuration - this sends "SET CONFIG" URB to USBD

	status = Configure(nEndpointsDescribed, Endpoints, pEndpoints);
	if ( !NT_SUCCESS(status) )
	{
		if (ppProblem) *ppProblem = NULL;
		acstatus = AC_CONFIGURATION_REQUEST_FAILED;
		goto Exit;
	}

	// Now initialize the interface objects and pipe objects

	for (i = 0; i < (int)nEndpointsDescribed; i++)
	{
		KUsbInterface* pInterface;

	// ignore interfaces not specified on this configuration

		if ( Endpoints[i].m_ConfigurationValue != ConfigurationValue )
			continue;

	// grab the interface object pointer for the current endpoint

		pInterface = Endpoints[i].m_pInterfaceObject;

	// Make sure that this is the first time we've seen this intf object

		for (j = 0; j < i; j++)
		{
			// ignore other configs
			if ( Endpoints[j].m_ConfigurationValue != ConfigurationValue )
				continue;

			if (Endpoints[j].m_pInterfaceObject == pInterface)
				break;
		}

		if ( j < i )
			continue;

	// initialize the interface object

		ASSERT (pInterface != NULL);

		status = pInterface->Initialize(*this, Endpoints[i].m_InterfaceNumber);
		if ( !NT_SUCCESS(status) )
		{
			if (ppProblem) *ppProblem = &Endpoints[i];
			acstatus = AC_FAILED_TO_INITIALIZE_INTERFACE_OBJECT;
			break;
		}

	// now initialize all the pipes on this interface

		for (j = 0; j < (int)nEndpointsDescribed; j++)
		{
			// ignore other configs

			if ( Endpoints[j].m_ConfigurationValue != ConfigurationValue )
				continue;

			if (Endpoints[j].m_pInterfaceObject == pInterface)
			{
				KUsbPipe* pPipe = Endpoints[j].m_pPipeObject;

	// it is legal for the pipe pointer to be NULL. This indicates an
	// interface with no endpoints (except control).

				if (pPipe == NULL)
					continue;

				int k;
				PUSBD_PIPE_INFORMATION pPipeInfo = pInterface->Pipes();

	// scan for the index (k) for which the endpoint addresses match

				for (status=STATUS_UNSUCCESSFUL, k=0; k < (int)pInterface->NumberOfPipes(); k++)
				{
					if (pPipeInfo[k].EndpointAddress == Endpoints[j].m_EndpointAddress)
					{
						pPipe->Initialize(*pInterface, k);
						status = STATUS_SUCCESS;
						break;
					}
				}

				if ( !NT_SUCCESS(status) )
				{
					if (ppProblem) *ppProblem = &Endpoints[j];
					acstatus = AC_FAILED_TO_LOCATE_ENDPOINT_ADDRESS;
					break;
				}
			}
		} // end for j

		if ( !NT_SUCCESS(status) )
			break;

	} // end for i

Exit:
	BOUNDS_CHECKER(USB_ACTIVATE_CONFIG, (this, ConfigurationValue, ULONG(acstatus)));
	return acstatus;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::ActivateConfiguration
//
// This function automates the configuration process. The input is
//
//	ConfigurationValue		Specifies the configuration value of the
//							configuration to activate. The value should
//							match the ConfigurationValue field
//							in the device's configuration descriptor (the USB
//							spec defines this field as starting with 1 - it
//							is not zero based).
//
//	MaxConfigSize			Optional parameter for the call to Preconfigure. Represents
//							the size of the configuration descriptor to retrieve.
//							Defaults to 1024
//
AC_STATUS KUsbLowerDevice::ActivateConfiguration(
	UCHAR ConfigurationValue,
	ULONG MaxConfigSize
	)
{
	ASSERT( ConfigurationValue > 0 );
	ASSERT( m_PipeArray.IsValid() );
	ASSERT( m_IntfArray.IsValid() );

	NTSTATUS status;
	AC_STATUS acstatus=AC_SUCCESS;
	PVOID Start=NULL;
	PUSB_ENDPOINT_DESCRIPTOR pEndpoints = NULL;
	PUSB_INTERFACE_DESCRIPTOR pIntfDesc;
	UCHAR i;
	USB_DEVICE_DESCRIPTOR desc;

	status = GetDeviceDescriptor( &desc );
	if ( !NT_SUCCESS(status) )
	{
		acstatus = AC_FAILED_TO_GET_DESCRIPTOR;
		goto Exit;
	}

	UCHAR ConfigIndex;

	for ( ConfigIndex = 0; ConfigIndex < desc.bNumConfigurations; ConfigIndex++ )
	{
		status = Preconfigure(ConfigIndex, MaxConfigSize);
		if ( !NT_SUCCESS(status) )
		{
			acstatus = AC_NO_SUCH_CONFIGURATION;
			goto Exit;
		}

		if ( m_Config->bConfigurationValue == ConfigurationValue )
			break;
	}

	if ( ConfigIndex == desc.bNumConfigurations )
	{
		acstatus = AC_NO_SUCH_CONFIGURATION;
		goto Exit;
	}

	// Look for a matching interface in the config descriptor and the list of KUsbInterface's
	// add all found to the list of supported interfaces in KUsbLowerDevice

	//loop thru all interfaces in the list of KUsbInterface's
	for(i = 0; i < m_IntfArray.MaxInserted() + 1; i++)
	{

		if( (NULL == m_IntfArray[i]) || (m_IntfArray[i]->m_ConfigurationValue != ConfigurationValue) )
			continue;

		UCHAR ThisInterface = m_IntfArray[i]->m_InterfaceNumber;
		UCHAR ThisAlternate = m_IntfArray[i]->m_CurrentAlternateSetting;

		//locate this interface in the configuration descriptor
		pIntfDesc = LocateInterface(
			&Start,
			&pEndpoints,
			ThisInterface,
			ThisAlternate
			);

		if ( pIntfDesc == NULL )
		{
			status = STATUS_UNSUCCESSFUL;
			acstatus = AC_COULD_NOT_LOCATE_INTERFACE;
			break;
		}

	// The described interface was found. Now add it to the set of interfaces
	// that the driver will support in this configuration.

		status = PreconfigureInterface(pIntfDesc);
		if ( !NT_SUCCESS(status) )
		{
			acstatus = AC_COULD_NOT_PRECONFIGURE_INTERFACE;
			break;
		}
	}

	// Test for normal completion of the above loop

	if ( !NT_SUCCESS(status) )
		goto Exit;

	// Now set the configuration - this sends "SET CONFIG" URB to USBD

	status = Configure(m_PipeArray, pEndpoints);

	if ( !NT_SUCCESS(status) )
	{
		acstatus = AC_CONFIGURATION_REQUEST_FAILED;
		goto Exit;
	}

	// Now initialize the interface objects and pipe objects

	//First close all the pipes in case any were open from an older configuration
	UCHAR k;
	for(k = 0; k < m_PipeArray.MaxInserted() + 1; k++)
		m_PipeArray[k]->Close();


	//loop thru all interfaces in the list of KUsbInterface's
	for(i = 0; i < m_IntfArray.MaxInserted() + 1; i++)
	{

		if( (NULL == m_IntfArray[i]) || (m_IntfArray[i]->m_ConfigurationValue != ConfigurationValue) )
			continue;

		status = m_IntfArray[i]->Open();

		if(STATUS_NO_SUCH_DEVICE == status)
		{
			m_IntfArray[i]->Close();
			acstatus = AC_FAILED_TO_INITIALIZE_INTERFACE_OBJECT;
			break;
		}


		//loop thru all pipes in the list of KUsbPipe's
		//Try to open any that are not already open
		//They will be opened if they belong to the current interface
		for(UCHAR j = 0, OpenPipes = 0; j < m_PipeArray.MaxInserted() + 1; j++)
		{
			if(!m_PipeArray[j]->IsOpen())
			{
				status = m_PipeArray[j]->Open(*m_IntfArray[i]);

				if(STATUS_UNSUCCESSFUL == status)
				{
					m_PipeArray[j]->Close();
				}
				else
				{
					OpenPipes++;
				}
			}
		}

		if( OpenPipes != m_IntfArray[i]->NumberOfPipes() )
			acstatus = AC_FAILED_TO_OPEN_PIPE_OBJECT;
	}

Exit:
	BOUNDS_CHECKER(USB_ACTIVATE_CONFIG, (this, ConfigurationValue, ULONG(acstatus)));
	return acstatus;

}


NTSTATUS KUsbLowerDevice::Configure(
	KArray<KUsbPipe*>& PipeArray,
	PUSB_ENDPOINT_DESCRIPTOR pFirstEndptDesc
	)
{
	PreconfiguredInterfaceListEntry* pLE;
	ULONG InterfaceCount;
	ULONG i,j,k;
	LONG x;
	NTSTATUS status;
	PURB pConfigUrb;

	ASSERT ( m_Config );

	InterfaceCount = m_InterfaceList.Count();

	ASSERT ( InterfaceCount );

// allocate an array of USBD_INTERFACE_LIST_ENTRY to be used to constuct the
// configuration request URB.

	if( m_ConfiguredInterfaces )
	{
		ASSERT( !m_ConfiguredInterfaces );
		delete m_ConfiguredInterfaces;
		m_ConfiguredInterfaces = NULL;
	}

	m_ConfiguredInterfaces =
		new (NonPagedPool) USBD_INTERFACE_LIST_ENTRY[InterfaceCount+1]; // null terminated

	if( m_ConfiguredInterfaces == NULL )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbLowerDevice::Configure", sizeof(USBD_INTERFACE_LIST_ENTRY)*(InterfaceCount+1)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

// now set up each USBD_INTERFACE_LIST_ENTRY

	for( i = 0, pLE = m_InterfaceList.Head(); i < InterfaceCount; i++)
	{
		m_ConfiguredInterfaces[i].InterfaceDescriptor = pLE->m_Descriptor;
		m_ConfiguredInterfaces[i].Interface = &pLE->m_InterfaceInformation;
		pLE = m_InterfaceList.Next(pLE);
	}

// mark the end of the array

	m_ConfiguredInterfaces[InterfaceCount].InterfaceDescriptor = NULL;
	m_ConfiguredInterfaces[InterfaceCount].Interface = NULL;

// now build the configuration request URB

	pConfigUrb = USBD_CreateConfigurationRequestEx(m_Config, m_ConfiguredInterfaces);
	if( NULL == pConfigUrb )
	{
		delete m_ConfiguredInterfaces;
		m_ConfiguredInterfaces = NULL;
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "alloc config URB", sizeof(URB)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

// Scan the endpoints to see if any of the pipes have a preset Maximum Transfer Size

// if we find the matching endpoint address, and the there is a pipe object associated with this endpoint,
// and there has been a previous call to SetMaximumTransferSize (size != 0) then set the MaximumTransferSize
// in the USBD_PIPE_INFORMATION struct before we submit the URB for configuration.

	if( pFirstEndptDesc && PipeArray.IsValid() )
	{
		//scan all preconfigured interfaces
		for( j = 0; j < InterfaceCount; j++)
		{
			PUSBD_PIPE_INFORMATION pPipeInfo = m_ConfiguredInterfaces[j].Interface->Pipes;
			ULONG nPipes = m_ConfiguredInterfaces[j].Interface->NumberOfPipes;

			PUSB_ENDPOINT_DESCRIPTOR pEndptDesc = pFirstEndptDesc;
			//scan all pipe info struct's in the interface descriptor
			for (k = 0; k < nPipes; k++, pPipeInfo++, pEndptDesc++)
			{
				// Scan the array of pipes to see if any have a preset Maximum Transfer Size
				for (x = 0; x < PipeArray.MaxInserted() + 1; x++)
				{
					// If there is no pipe object, then skip it.
					if( (NULL == PipeArray[x]) )
						 continue;

					// find the pipe by endpoint address
					if (pEndptDesc->bEndpointAddress == PipeArray[x]->m_EndpointAddress)
					{
						// if the pipe object's
						// max transfer size has not been preconfigured, then skip it.
						if( (0 != PipeArray[x]->m_SetMaxTransferSize) )
						{
							pPipeInfo->MaximumTransferSize = PipeArray[x]->m_SetMaxTransferSize;
						}
						break;
					}
				}
			}
		}
	}

// Now submit the URB

	status = SubmitUrb(pConfigUrb);

	BOUNDS_CHECKER(CONFIGURED_USB_DEVICE, (this, status));

// Copy the configuration memory from the URB back into the object's data structures

	if( NT_SUCCESS(status) )
	{
		m_ConfigurationHandle =
			((_URB_SELECT_CONFIGURATION*)pConfigUrb)->ConfigurationHandle;

		for( i = 0, pLE = m_InterfaceList.Head(); i < InterfaceCount; i++)
		{
			RtlCopyMemory(
				&pLE->m_InterfaceInformation,
				m_ConfiguredInterfaces[i].Interface,
				m_ConfiguredInterfaces[i].Interface->Length
				);

			m_ConfiguredInterfaces[i].Interface = &pLE->m_InterfaceInformation;
			pLE = m_InterfaceList.Next(pLE);
		}
	}

// release the URB

	ExFreePool(pConfigUrb);

	return status;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::InitializePreConfiguredInterface
//
// This method initializes a PreconfiguredInterfaceListEntry with data from an
// interface descriptor
//
void KUsbLowerDevice::InitializePreConfiguredInterface(
	PUSB_INTERFACE_DESCRIPTOR pInterfaceDesc,
	PreconfiguredInterfaceListEntry* pLE
	)
{
	ASSERT( pInterfaceDesc );
	ASSERT( pLE );

	//calculate size of PreconfiguredInterfaceListEntry
	ULONG size = GET_INTF_LIST_ENTRY_SIZE(pInterfaceDesc->bNumEndpoints);

	//initialize the object
	RtlZeroMemory(pLE, size);

	pLE->m_Descriptor = pInterfaceDesc;

	pLE->m_InterfaceInformation.Length = static_cast<USHORT>(
		GET_USBD_INTERFACE_SIZE(pInterfaceDesc->bNumEndpoints)
		);
	pLE->m_InterfaceInformation.InterfaceNumber = pInterfaceDesc->bInterfaceNumber;
	pLE->m_InterfaceInformation.AlternateSetting = pInterfaceDesc->bAlternateSetting;
	pLE->m_InterfaceInformation.NumberOfPipes = pInterfaceDesc->bNumEndpoints;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::UpdateConfiguredInterface
//
// A driver calls this member after a successful SELECT_INTERFACE request to change
// an interface's alternate setting.  This function will update the PUSBD_INTERFACE_LIST_ENTRY
// & PreconfiguredInterfaceListEntry for the interface so that the USBD_INTERFACE_INFORMATION
// is correct for this alternate setting. The framework calls this function in
// KUsbInterface::SelectAlternate
//
NTSTATUS KUsbLowerDevice::UpdateConfiguredInterface(
	PUSB_INTERFACE_DESCRIPTOR pInterfaceDesc
	)
{
// Update the PreconfiguredInterfaceListEntry that has the same InterfaceNumber as pInterfaceDesc
	PreconfiguredInterfaceListEntry* pLE = m_InterfaceList.Head();

	if( NULL == pLE )
		return STATUS_UNSUCCESSFUL;

	do{
		// check for a matching InterfaceNumber
		if( pLE->m_InterfaceInformation.InterfaceNumber == pInterfaceDesc->bInterfaceNumber )
		{
			//calculate size required for interface info
			ULONG dwIntfInfoSize = GET_USBD_INTERFACE_SIZE(pInterfaceDesc->bNumEndpoints);

			//remove the entry from the list
			m_InterfaceList.Remove(pLE);

			//compare the size of the "old" INTERFACE_INFORMATION with the updated interface
			if(pLE->m_InterfaceInformation.Length != dwIntfInfoSize)
			{
				//the size does not match so need to allocate a new PreconfiguredInterfaceListEntry

				//deallocate the object
				delete pLE;
				pLE = NULL;

				//calculate the new size of the PreconfiguredInterfaceListEntry
				ULONG size = GET_INTF_LIST_ENTRY_SIZE(pInterfaceDesc->bNumEndpoints);

				//allocate a new object of the correct size
				pLE = (PreconfiguredInterfaceListEntry*) new (NonPagedPool) UCHAR[size];

				if ( !pLE )
				{
					BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "UpdateConfiguredInterface", size));
					return STATUS_INSUFFICIENT_RESOURCES;
				}
			}

			InitializePreConfiguredInterface(
				pInterfaceDesc,
				pLE
				);

			//add the new object to the list
			m_InterfaceList.InsertTail(pLE);

			//exit the loop since the PreconfiguredInterfaceListEntry was found and updated
			break;
		}

		pLE = m_InterfaceList.Next(pLE);

	} while(pLE);

	if(NULL == pLE)
		//above loop terminated normally, this is an error!
		return STATUS_UNSUCCESSFUL;


// Update PUSBD_INTERFACE_LIST_ENTRY m_ConfiguredInterfaces that has the same InterfaceNumber as pInterfaceDesc
	//m_ConfiguredInterfaces is a NULL terminated array
	for( int i = 0; NULL != m_ConfiguredInterfaces[i].InterfaceDescriptor; i++)
	{
		if( m_ConfiguredInterfaces[i].Interface &&
			( m_ConfiguredInterfaces[i].Interface->InterfaceNumber == pInterfaceDesc->bInterfaceNumber )
		  )
		{
			//update the PUSBD_INTERFACE_LIST_ENTRY
			m_ConfiguredInterfaces[i].InterfaceDescriptor = pLE->m_Descriptor;
			m_ConfiguredInterfaces[i].Interface = &pLE->m_InterfaceInformation;

			//exit the loop since the PUSBD_INTERFACE_LIST_ENTRY was found and updated
			break;
		}
	}

	if(NULL == m_ConfiguredInterfaces[i].InterfaceDescriptor)
		//above loop terminated normally, this is an error!
		return STATUS_UNSUCCESSFUL;

	return STATUS_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::DeActivateConfiguration
//
// This function puts the device in the unconfigured state and closes pipe
// and interface objects.
//
NTSTATUS KUsbLowerDevice::DeActivateConfiguration(
	PIO_COMPLETION_ROUTINE pfnCompletionRoutine,
	PVOID pContext
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	UCHAR byConfigValue = m_Config->bConfigurationValue;

	status = Unconfigure(pfnCompletionRoutine, pContext);

	if(STATUS_SUCCESS != status)
		return status;

	// Now close the interface objects and pipe objects

	//loop thru all interfaces in the list of KUsbInterface's
	for(UCHAR i = 0; i < m_IntfArray.MaxInserted() + 1; i++)
	{
		ASSERT( m_IntfArray[i] );
		if( (NULL == m_IntfArray[i]) || (m_IntfArray[i]->m_ConfigurationValue != byConfigValue) )
			continue;

		m_IntfArray[i]->Close();

		//loop thru all pipes in the array of KUsbPipe's
		for(UCHAR j = 0; j < m_PipeArray.MaxInserted() + 1; j++)
		{
			ASSERT( m_PipeArray[j] );
			m_PipeArray[j]->Close();
		}
	}
	return status;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::Unconfigure
//
// This function puts the device in the unconfigured state.
//
NTSTATUS KUsbLowerDevice::Unconfigure(
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	NTSTATUS status;
	PURB p = new (NonPagedPool) URB;
	if ( p == NULL )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbLowerDevice::Unconfigure", sizeof(URB)));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlZeroMemory(p, sizeof(URB));

	UsbBuildSelectConfigurationRequest(
		p,
		sizeof(_URB_SELECT_CONFIGURATION),
		NULL
		);

	status = SubmitUrb(p, CompletionRoutine, Context);

	if ( CompletionRoutine == NULL )
		delete p;

	// Remove all entries from the interface list and delete them
	PreconfiguredInterfaceListEntry* pLE;
	PreconfiguredInterfaceListEntry* pNext;
	pNext = pLE = m_InterfaceList.Head();
	while (pNext != NULL)
	{
		pNext = m_InterfaceList.Next(pLE);
		delete pLE;
		pLE = pNext;
	}
	m_InterfaceList.Reset();

	BOUNDS_CHECKER(UNCONFIGURED_USB_DEVICE, (this, status));

	return status;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbLowerDevice::_build_class_or_vendor_request
//
// Allocate a class or vendor request URB and initialize the fields. This is a
// protected static member.
//
PURB KUsbLowerDevice::_build_class_or_vendor_request(
	USHORT Function,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	ULONG TransferBufferLength,
	PVOID TransferBuffer,
	PMDL TransferBufferMDL,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	USHORT Index,
	PURB Link,
	PURB pExistingUrb
	)
{

	_URB_CONTROL_VENDOR_OR_CLASS_REQUEST* pUrb = NULL;

	if( NULL == pExistingUrb )
	{
		pUrb = new (NonPagedPool) _URB_CONTROL_VENDOR_OR_CLASS_REQUEST;
		if ( pUrb == NULL)
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "_build_class_or_vendor_request",  sizeof(_URB_CONTROL_VENDOR_OR_CLASS_REQUEST)));
			return NULL;
		}
	}
	else
	{
		pUrb = (_URB_CONTROL_VENDOR_OR_CLASS_REQUEST *)pExistingUrb;
	}

	_URB_HEADER* pHdr = (_URB_HEADER*)pUrb;

	pHdr->Length = sizeof _URB_CONTROL_VENDOR_OR_CLASS_REQUEST;
	pHdr->Function = Function;
	pHdr->Status = 0;

	pUrb->Reserved = 0;
	pUrb->TransferFlags = 0;
	if (bIn)
	{
		pUrb->TransferFlags |= USBD_TRANSFER_DIRECTION_IN;
		if (bShortOk)
			pUrb->TransferFlags |= USBD_SHORT_TRANSFER_OK;
	}
	else
		pUrb->TransferFlags |= USBD_TRANSFER_DIRECTION_OUT;


	pUrb->TransferBufferLength = TransferBufferLength;
	pUrb->TransferBuffer = TransferBuffer;
	pUrb->TransferBufferMDL = TransferBufferMDL;
	pUrb->UrbLink = Link;
	pUrb->RequestTypeReservedBits = RequestTypeReservedBits;
	pUrb->Request = Request;
	pUrb->Value = Value;
	pUrb->Index = Index;
	pUrb->Reserved1 = 0;

	return (PURB)pUrb;
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// KUsbPipe::Initialize
//
// Parameters
//	dev					Related KUsbLowerDevice
//	ea					Endpoint Address on the device that this object abstracts
//  MaxTransferSize		Maximum transfer size that this endpoint can handle
//
NTSTATUS KUsbPipe::Initialize(
	KUsbLowerDevice& dev,
	UCHAR ea,
	ULONG MaxTransferSize
	)
{
	NTSTATUS status = dev.RegisterPipe(this);

	ASSERT( STATUS_SUCCESS == status );

	if( STATUS_SUCCESS == status )
	{
		m_EndpointAddress = ea;
		m_SetMaxTransferSize = MaxTransferSize;
	}
	else
	{
		m_EndpointAddress = 0;
		m_SetMaxTransferSize = 0;
	}

	return status;
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Constructors for class KUsbInterface
//
// Parameters
//	Device				Related device object
//	InterfaceNumber		selects interface from configuration descriptor
//
KUsbInterface::KUsbInterface(
	KUsbLowerDevice& Device,
	UCHAR InterfaceNumber,
	NTSTATUS* pStatus
	)
{
	for (int i = 0; i < Device.m_Config->bNumInterfaces; i++)
		if (Device.m_ConfiguredInterfaces[i].Interface->InterfaceNumber == InterfaceNumber)
		{
			m_Descriptor = Device.m_ConfiguredInterfaces[i].InterfaceDescriptor;
			m_Information = Device.m_ConfiguredInterfaces[i].Interface;
			m_ConfigurationHandle = Device.m_ConfigurationHandle;
			m_pLowerDevice = &Device;
			*pStatus = STATUS_SUCCESS;
			return;
		}

	m_Descriptor = NULL;
	m_Information = NULL;
	m_ConfigurationHandle = NULL;
	m_pLowerDevice = NULL;

	*pStatus = STATUS_NO_SUCH_DEVICE;

	BOUNDS_CHECKER(CONSTRUCTED_USB_INTERFACE, (this, *pStatus));
}


NTSTATUS KUsbInterface::Initialize(
	KUsbLowerDevice& Device,
	UCHAR InterfaceNumber
	)
{
	for (int i = 0; i < Device.m_Config->bNumInterfaces; i++)
		if (Device.m_ConfiguredInterfaces[i].Interface->InterfaceNumber == InterfaceNumber)
		{
			m_Descriptor = Device.m_ConfiguredInterfaces[i].InterfaceDescriptor;
			m_Information = Device.m_ConfiguredInterfaces[i].Interface;
			m_ConfigurationHandle = Device.m_ConfigurationHandle;
			m_pLowerDevice = &Device;

			BOUNDS_CHECKER(CONSTRUCTED_USB_INTERFACE, (this, STATUS_SUCCESS));

			return STATUS_SUCCESS;
		}

	m_Descriptor = NULL;
	m_Information = NULL;
	m_ConfigurationHandle = NULL;
	m_pLowerDevice = NULL;

	BOUNDS_CHECKER(CONSTRUCTED_USB_INTERFACE, (this, STATUS_NO_SUCH_DEVICE));

	return STATUS_NO_SUCH_DEVICE;
}


/////////////////////////////////////////////////////////////////////////////////////
// Build select alternate request for class KUsbInterface
//
PURB KUsbInterface::BuildSelectAlternateRequest(
	UCHAR AlternateSetting
	)
{
	ASSERT(m_Information);

	// The URB will return the interface information at the end of this URB!
	ULONG ulSiz = sizeof(struct _URB_SELECT_INTERFACE) +
				  m_Information->Length;

	PURB p = (PURB) new (NonPagedPool) UCHAR[ulSiz];
	if ( p == NULL )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbInterface::BuildSelectAlternateRequest", ulSiz));
		return NULL;
	}

	RtlZeroMemory(p, ulSiz);

	// Put that interface information into the right place!
	PUSBD_INTERFACE_INFORMATION pInfo = &((p)->UrbSelectInterface.Interface);
	memcpy(pInfo, m_Information, m_Information->Length);

	UsbBuildSelectInterfaceRequest(
		p,
		(USHORT) ulSiz,
		m_ConfigurationHandle,
		m_Information->InterfaceNumber,
		AlternateSetting
		);

	return p;
}


/////////////////////////////////////////////////////////////////////////////////////
// Build select alternate request for class KUsbInterface
//
PURB KUsbInterface::BuildSelectAlternateRequest(
	UCHAR AlternateSetting,
	PUSB_INTERFACE_DESCRIPTOR pIntfDesc
	)
{
	ASSERT(m_Information);
	ASSERT(pIntfDesc);

	// Determine the size of the URB required for this interface alternate setting
	USHORT usUrbSize = static_cast<USHORT>(GET_SELECT_INTERFACE_REQUEST_SIZE(pIntfDesc->bNumEndpoints));

	// The URB will return the interface information at the end of this URB!

	PURB p = (PURB) new (NonPagedPool) UCHAR[usUrbSize];
	if ( p == NULL )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbInterface::BuildSelectAlternateRequest", usUrbSize));
		return NULL;
	}

	RtlZeroMemory(p, usUrbSize);

//	memcpy(pInfo, m_Information, m_Information->Length);

	UsbBuildSelectInterfaceRequest(
		p,
		usUrbSize,
		m_ConfigurationHandle,
		m_InterfaceNumber,
		AlternateSetting
		);

	// Put that interface information into the right place!
	PUSBD_INTERFACE_INFORMATION pInfo = &((p)->UrbSelectInterface.Interface);

	pInfo->Length = static_cast<USHORT>(GET_USBD_INTERFACE_SIZE(pIntfDesc->bNumEndpoints));

	return p;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbInterface::SelectAlternate
//
// This function automates the set interface process. The input is
//
//	AlternateSetting		Specifies the Alternate Setting of the
//							interface to activate.
//
SA_STATUS KUsbInterface::SelectAlternate(
	UCHAR AlternateSetting
	)
{
	ASSERT( m_pLowerDevice );

	KArray<KUsbPipe*>& PipeArray = m_pLowerDevice->PipeArray();

	if(AlternateSetting == m_CurrentAlternateSetting)
		return SA_SUCCESS;

	PVOID Start = NULL;
	PUSB_ENDPOINT_DESCRIPTOR pEndpoints = NULL;

	PUSB_INTERFACE_DESCRIPTOR pIntfDesc = m_pLowerDevice->LocateInterface(
								&Start,
								&pEndpoints,
								InterfaceNumber(),
								AlternateSetting
							    );

	if( NULL == pIntfDesc )
	{
		return SA_COULD_NOT_LOCATE_INTERFACE;
	}

	// Create the Select Interface URB
	PURB pUrb = BuildSelectAlternateRequest(AlternateSetting, pIntfDesc);
	ASSERT(pUrb);
	if(!pUrb)
		return SA_INSUFFICIENT_RESOURCES;

	PUSBD_INTERFACE_INFORMATION pInfo = &((pUrb)->UrbSelectInterface.Interface);

	PUSBD_PIPE_INFORMATION pPipeInfo = pInfo->Pipes;
	ULONG nPipes = pIntfDesc->bNumEndpoints;
	PUSB_ENDPOINT_DESCRIPTOR pEndptDesc = pEndpoints;

	//scan all pipe info struct's in the interface descriptor
	//set Pipes[i].MaximumTransferSize
	for( UCHAR k = 0; k < nPipes; k++, pPipeInfo++, pEndptDesc++)
	{
		// Scan the array of pipes to see if any have a preset Maximum Transfer Size
		for( UCHAR x = 0; x < PipeArray.MaxInserted() + 1; x++)
		{
			// If there is no pipe object, then skip it.
			if( (NULL == PipeArray[x]) )
				 continue;

			// find the pipe by endpoint address
			if(pEndptDesc->bEndpointAddress == PipeArray[x]->m_EndpointAddress)
			{
				// if the pipe object's
				// max transfer size has not been preconfigured, then skip it.
				if( (0 != PipeArray[x]->m_SetMaxTransferSize) )
					pPipeInfo->MaximumTransferSize = PipeArray[x]->m_SetMaxTransferSize;
				break;
			}
		}
	}

	// Submit the Select Interface URB
	NTSTATUS status = SubmitUrb(pUrb);

	if ( !NT_SUCCESS(status) )
	{
		delete pUrb;
		return SA_SELECT_INTERFACE_REQUEST_FAILED;
	}

	// Our interface has changed.  Fixup our internal data structures

	// Update KUsbLowerDevice data structures - the Lists of interfaces to
	// contain correct USBD_INTERFACE_INFORMATION
	status = m_pLowerDevice->UpdateConfiguredInterface(pIntfDesc);
	if ( !NT_SUCCESS(status) )
	{
		delete pUrb;
		return SA_FAILED_TO_UPDATE_INTF_LIST;
	}

	// Update KUsbInterface data structures
	for( int i = 0; i < m_pLowerDevice->m_Config->bNumInterfaces; i++)
		if( m_pLowerDevice->m_ConfiguredInterfaces[i].Interface->InterfaceNumber == m_InterfaceNumber )
		{
			m_Descriptor = m_pLowerDevice->m_ConfiguredInterfaces[i].InterfaceDescriptor;
			m_Information = m_pLowerDevice->m_ConfiguredInterfaces[i].Interface;
			m_CurrentAlternateSetting = AlternateSetting;
		}

	ASSERT(pInfo->Length == m_Information->Length);
	memcpy(m_Information, pInfo, pInfo->Length);

	delete pUrb;

	//open and close KUsbPipe objects based on the new alternate setting
	//loop thru all pipes in the list of KUsbPipe pointers
	for(UCHAR j = 0; j < PipeArray.MaxInserted() + 1; j++)
	{
		if(PipeArray[j]->Interface() == this)
		{
			//if the current pipe is already associated
			//with this interface, try to open it.
			status = PipeArray[j]->Open(*this);

			//if the open failed, then close the pipe
			//this indicates that the pipe is not active in this
			//interface with this alternate setting.
			if(STATUS_UNSUCCESSFUL == status)
				PipeArray[j]->Close();
		}
		else
		{
			//if the current pipe is NOT already associated
			//with this interface, try to open it.
			// we don't care if we fail since this just means that
			// the pipe is not active with this alternate setting
			PipeArray[j]->Open(*this);
		}
	}

	return SA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////
// KUsbInterface::SelectAlternate
//
// This function automates the set interface process. The input is
//
//	alternate				Specifies the Alternate Setting of the
//							interface to activate.
//
//  nEndpointsDescribed		Count of entries in the Endpoints array (next param)
//
//  Endpoints				Array of endpoint specifiers. Each entry specifies
//							(see struct USB_ENDPOINT in kusb.h)
//							1. The configuration value indicating which
//							   configuration the endpoint appears in
//							2. The interface number, which is compared to the
//							   interface numbers in the device's interface
//							   descriptors for the selected configuration
//							3. The interface alternate setting value, which
//							   is compared to the alternate setting value
//							   in the device's interface descriptor for the
//							   selected configuration.
//							4. The endpoint address. Bit 7 (0x80) must be
//							   set to one if the endpoint is IN, and must
//							   be zero for an OUT endpoint.
//							5. A pointer to the corresponding KUsbInterface object
//							6. A pointer to the corresponding KUsbPipe object
//
//	ppProblem				Optional pointer to pointer to USB_ENDPOINT.  If an error
//							occurs during configuration, the offending USB_ENDPOINT
//							can be identified to the caller
//
SA_STATUS KUsbInterface::SelectAlternate(
	UCHAR alternate,
	ULONG nEndpointsDescribed,
	USB_ENDPOINT* Endpoints,
	USB_ENDPOINT** ppProblem
	)
{
	// Do some bounds checking!

	// Submit the Select Interface URB
	PURB pUrb = BuildSelectAlternateRequest(alternate);
	ASSERT(pUrb);
	if (!pUrb)
		return SA_INSUFFICIENT_RESOURCES;

	NTSTATUS status = SubmitUrb(pUrb);

	if ( !NT_SUCCESS(status) )
	{
		delete pUrb;
		return SA_SELECT_INTERFACE_REQUEST_FAILED;
	}

	// Put that interface information into the right place!
	PUSBD_INTERFACE_INFORMATION pInfo = &((pUrb)->UrbSelectInterface.Interface);
	ASSERT(pInfo->Length == m_Information->Length);
	memcpy(m_Information, pInfo, pInfo->Length);

	delete pUrb;

	// Our interface has changed.  Fixup our internal data structures
	PVOID Start=NULL;
	PUSB_ENDPOINT_DESCRIPTOR pEndpoints;
	PUSB_INTERFACE_DESCRIPTOR pIntfDesc;

	pIntfDesc = m_pLowerDevice->LocateInterface(
								&Start,
								&pEndpoints,
								InterfaceNumber(),
								alternate
							   );

	if ( pIntfDesc == NULL )
	{
		if (ppProblem) *ppProblem = NULL;
		return SA_COULD_NOT_LOCATE_INTERFACE;
	}

	// For each endpoint in the endpoint descriptor list, find
	//    the appropriate Endpoint array object.
	for (ULONG i=0; i<NumberOfPipes(); i++)
	{
		for (ULONG j=0; j<nEndpointsDescribed; j++)
		{
			if (pEndpoints[i].bEndpointAddress==Endpoints[j].m_EndpointAddress)
			{
				// With the found Endpoint array object:
				//     Reset the Endpoint's alternate interface member.
				Endpoints[j].m_InterfaceAlternate = alternate;

				//     Then, initialize the Endpoint's pipe object
				if (Endpoints[j].m_pPipeObject)
					Endpoints[j].m_pPipeObject->Initialize(*this, i);

				break;
			}
		}
		if ( j == nEndpointsDescribed )
			return SA_FAILED_TO_LOCATE_ENDPOINT_ADDRESS;
	}

	return SA_SUCCESS;
}


NTSTATUS KUsbLowerDevice::RegisterPipe(KUsbPipe* pipe)
{
	ASSERT( m_PipeArray.IsValid() );
	if(! m_PipeArray.IsValid() )
		return STATUS_UNSUCCESSFUL;

	ULONG index;
	return m_PipeArray.InsertNext(pipe, &index);
}

/////////////////////////////////////////////////////////////////////////////////////
// Transfer requests for class KUsbPipe
//
PURB KUsbPipe::BuildInterruptTransfer(
	KMemory& Mdl,
	ULONG Length,
	BOOLEAN bShortOk,
	PURB Link,
	PURB pUrb,
	BOOLEAN bIn
	)
{
	ASSERT (m_Information->PipeType == UsbdPipeTypeInterrupt);

	PURB p = NULL;

	if( NULL == pUrb )
	{
		p = new (NonPagedPool) URB;
		if ( p == NULL)
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbPipe::BuildInterruptTransfer",  sizeof(URB)));
			return NULL;
		}
	}
	else
	{
		p = pUrb;
	}

	ULONG Flags;
	if (bIn)
		Flags = USBD_TRANSFER_DIRECTION_IN;
	else
		Flags = USBD_TRANSFER_DIRECTION_OUT;

	if (bShortOk)
		Flags |= USBD_SHORT_TRANSFER_OK;

	UsbBuildInterruptOrBulkTransferRequest(
		p,
		(USHORT) sizeof( _URB_BULK_OR_INTERRUPT_TRANSFER ),
		m_Information->PipeHandle,
		NULL,
		Mdl,
		Length,
		Flags,
		Link
		);

	p->UrbHeader.Status = 0;

	return p;
}

///////////////////////////////////////////////////////////////////////////////

PURB KUsbPipe::BuildInterruptTransfer(
	PVOID Buffer,
	ULONG Length,
	BOOLEAN bShortOk,
	PURB Link,
	PURB pUrb,
	BOOLEAN bIn
	)
{
	ASSERT (m_Information->PipeType == UsbdPipeTypeInterrupt);

	PURB p = NULL;

	if( NULL == pUrb )
	{
		p = new (NonPagedPool) URB;
		if ( p == NULL)
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbPipe::BuildInterruptTransfer",  sizeof(URB)));
			return NULL;
		}
	}
	else
	{
		p = pUrb;
	}

	ULONG Flags;
	if (bIn)
		Flags = USBD_TRANSFER_DIRECTION_IN;
	else
		Flags = USBD_TRANSFER_DIRECTION_OUT;

	if (bShortOk)
		Flags |= USBD_SHORT_TRANSFER_OK;

	UsbBuildInterruptOrBulkTransferRequest(
		p,
		(USHORT) sizeof( _URB_BULK_OR_INTERRUPT_TRANSFER ),
		m_Information->PipeHandle,
		Buffer,
		NULL,
		Length,
		Flags,
		Link
		);

	p->UrbHeader.Status = 0;

	return p;
}

///////////////////////////////////////////////////////////////////////////////

PURB KUsbPipe::__build_iso_xfer(
	ULONG NumberOfPackets,
	ULONG PacketSize,
	BOOLEAN bIn,
	BOOLEAN bASAP,
	ULONG StartFrame,
	ULONG Length,
	PURB pUrb
	)
{
	ASSERT (m_Information->PipeType == UsbdPipeTypeIsochronous);

	ULONG size = sizeof(URB)+(NumberOfPackets*sizeof(USBD_ISO_PACKET_DESCRIPTOR));

	PURB p = NULL;

	if( NULL == pUrb )
	{
		p = (PURB) new (NonPagedPool) char[size];
		if ( p == NULL)
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbPipe::__build_iso_xfer", size));
			return NULL;
		}
	}
	else
	{
		p = pUrb;
	}

	RtlZeroMemory(p, size);

	p->UrbHeader.Length = (USHORT)size;
	p->UrbHeader.Function = URB_FUNCTION_ISOCH_TRANSFER;
	p->UrbIsochronousTransfer.PipeHandle = m_Information->PipeHandle;
	p->UrbIsochronousTransfer.TransferFlags =
		bIn ? USBD_TRANSFER_DIRECTION_IN : 0;
	if (bASAP)
		p->UrbIsochronousTransfer.TransferFlags |= USBD_START_ISO_TRANSFER_ASAP;
	else
		p->UrbIsochronousTransfer.StartFrame = StartFrame;
    p->UrbIsochronousTransfer.TransferBufferLength = Length;
    p->UrbIsochronousTransfer.NumberOfPackets = NumberOfPackets;

	for (ULONG i=0; i< NumberOfPackets; i++)
	{
    	p->UrbIsochronousTransfer.IsoPacket[i].Offset = i * PacketSize;
		if(!bIn)
	   		p->UrbIsochronousTransfer.IsoPacket[i].Length = PacketSize;
	}

	return p;
}


///////////////////////////////////////////////////////////////////////////////

PURB KUsbPipe::BuildControlTransfer(
	PVOID Buffer,
	ULONG Length,
	BOOLEAN bIn,
	PURB Link,
	PURB pUrb
	)
{
	ASSERT (m_Information->PipeType == UsbdPipeTypeControl);

	ULONG Flags;

	PURB p = NULL;

	if( NULL == pUrb )
	{
		p = new (NonPagedPool) URB;
		if ( p == NULL)
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbPipe::BuildControlTransfer",  sizeof(URB)));
			return NULL;
		}
	}
	else
	{
		p = pUrb;
	}

	if (bIn)
		Flags = USBD_TRANSFER_DIRECTION_IN;
	else
		Flags = 0;


	p->UrbHeader.Length = sizeof(_URB_CONTROL_TRANSFER);
	p->UrbHeader.Function = URB_FUNCTION_CONTROL_TRANSFER;

	p->UrbControlTransfer.PipeHandle = m_Information->PipeHandle;
	p->UrbControlTransfer.TransferFlags = Flags;
	p->UrbControlTransfer.TransferBufferLength = Length;
	p->UrbControlTransfer.TransferBuffer = Buffer;
	p->UrbControlTransfer.TransferBufferMDL = NULL;
	p->UrbControlTransfer.UrbLink = Link;
	memset(p->UrbControlTransfer.SetupPacket, 0, sizeof(p->UrbControlTransfer.SetupPacket));

	return p;
}

///////////////////////////////////////////////////////////////////////////////

PURB KUsbPipe::BuildControlTransfer(
	KMemory& Mdl,
	ULONG Length,
	BOOLEAN bIn,
	PURB Link,
	PURB pUrb
	)
{
	ASSERT (m_Information->PipeType == UsbdPipeTypeControl);

	ULONG Flags;

	PURB p = NULL;

	if( NULL == pUrb )
	{
		p = new (NonPagedPool) URB;
		if ( p == NULL)
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbPipe::BuildControlTransfer",  sizeof(URB)));
			return NULL;
		}
	}
	else
	{
		p = pUrb;
	}

	if (bIn)
		Flags = USBD_TRANSFER_DIRECTION_IN;
	else
		Flags = 0;

	p->UrbHeader.Length = sizeof(_URB_CONTROL_TRANSFER);
	p->UrbHeader.Function = URB_FUNCTION_CONTROL_TRANSFER;

	p->UrbControlTransfer.PipeHandle = m_Information->PipeHandle;
	p->UrbControlTransfer.TransferFlags = Flags;
	p->UrbControlTransfer.TransferBufferLength = Length;
	p->UrbControlTransfer.TransferBuffer = NULL;
	p->UrbControlTransfer.TransferBufferMDL = Mdl;
	p->UrbControlTransfer.UrbLink = Link;
	memset(p->UrbControlTransfer.SetupPacket, 0, sizeof(p->UrbControlTransfer.SetupPacket));

	return p;
}

///////////////////////////////////////////////////////////////////////////////

PURB KUsbPipe::BuildBulkTransfer(
	PVOID Buffer,
	ULONG Length,
	BOOLEAN bIn,
	PURB Link,
	BOOLEAN bShortOk,
	PURB pUrb
	)
{
	ASSERT (m_Information->PipeType == UsbdPipeTypeBulk);

	ULONG Flags;

	PURB p = NULL;

	if( NULL == pUrb )
	{
		p = new (NonPagedPool) URB;
		if ( p == NULL)
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbPipe::BuildBulkTransfer",  sizeof(URB)));
			return NULL;
		}
	}
	else
	{
		p = pUrb;
	}

	if (bIn)
	{
		Flags = USBD_TRANSFER_DIRECTION_IN;
		if (bShortOk)
			Flags |= USBD_SHORT_TRANSFER_OK;
	}

	else
		Flags = 0;

	UsbBuildInterruptOrBulkTransferRequest(
		p,
		(USHORT) sizeof( _URB_BULK_OR_INTERRUPT_TRANSFER ),
		m_Information->PipeHandle,
		Buffer,
		NULL,
		Length,
		Flags,
		Link
		);

	return p;
}

///////////////////////////////////////////////////////////////////////////////

PURB KUsbPipe::BuildBulkTransfer(
	KMemory& Mdl,
	ULONG Length,
	BOOLEAN bIn,
	PURB Link,
	BOOLEAN bShortOk,
	PURB pUrb
	)
{
	ASSERT (m_Information->PipeType == UsbdPipeTypeBulk);

	ULONG Flags;

	PURB p = NULL;

	if( NULL == pUrb )
	{
		p = new (NonPagedPool) URB;
		if ( p == NULL)
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KUsbPipe::BuildBulkTransfer",  sizeof(URB)));
			return NULL;
		}
	}
	else
	{
		p = pUrb;
	}

	if(bIn)
	{
		Flags = USBD_TRANSFER_DIRECTION_IN;
		if (bShortOk)
			Flags |= USBD_SHORT_TRANSFER_OK;
	}
	else
	{
		Flags = 0;
	}

	UsbBuildInterruptOrBulkTransferRequest(
		p,
		(USHORT) sizeof( _URB_BULK_OR_INTERRUPT_TRANSFER ),
		m_Information->PipeHandle,
		NULL,
		Mdl,
		Length,
		Flags,
		Link
		);

	return p;
}


///////////////////////////////////////////////////////////////////////////////
// Global helper functions for trace output
///////////////////////////////////////////////////////////////////////////////
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

KTrace& operator << (KTrace& t, const USB_DEVICE_DESCRIPTOR& d)
{
 	t.Trace( t.m_StreamTraceLevel, "USB_DEVICE_DESCRIPTOR 0x%08lX\n", ULONG_PTR(&d));
	t.Trace( t.m_StreamTraceLevel, "bLength = 0x%02lX\n",			d.bLength);
	t.Trace( t.m_StreamTraceLevel, "bDescriptorType = 0x%02lX\n",	d.bDescriptorType);
	t.Trace( t.m_StreamTraceLevel, "bcdUSB = 0x%04lX\n",			d.bcdUSB);
	t.Trace( t.m_StreamTraceLevel, "bDeviceClass = 0x%02lX\n",		d.bDeviceClass);
	t.Trace( t.m_StreamTraceLevel, "bDeviceSubClass = 0x%02lX\n",	d.bDeviceSubClass);
	t.Trace( t.m_StreamTraceLevel, "bDeviceProtocol = 0x%02lX\n",	d.bDeviceProtocol);
	t.Trace( t.m_StreamTraceLevel, "bMaxPacketSize0 = 0x%02lX\n",	d.bMaxPacketSize0);
	t.Trace( t.m_StreamTraceLevel, "idVendor = 0x%04lX\n",			d.idVendor);
	t.Trace( t.m_StreamTraceLevel, "idProduct = 0x%04lX\n",			d.idProduct);
	t.Trace( t.m_StreamTraceLevel, "bcdDevice = 0x%04lX\n",			d.bcdDevice);
	t.Trace( t.m_StreamTraceLevel, "iManufacturer = 0x%02lX\n",		d.iManufacturer);
	t.Trace( t.m_StreamTraceLevel, "iProduct = 0x%02lX\n",			d.iProduct);
	t.Trace( t.m_StreamTraceLevel, "iSerialNumber = 0x%02lX\n",		d.iSerialNumber);
	t.Trace( t.m_StreamTraceLevel, "bNumConfigurations = 0x%02lX\n",d.bNumConfigurations);
	return t;
}

KTrace& operator << (KTrace& t, const USB_CONFIGURATION_DESCRIPTOR& d)
{
 	t.Trace( t.m_StreamTraceLevel, "USB_CONFIGURATION_DESCRIPTOR 0x%08lX\n", ULONG_PTR(&d));
	t.Trace( t.m_StreamTraceLevel, "bLength = 0x%02lX\n",			 d.bLength);
	t.Trace( t.m_StreamTraceLevel, "bDescriptorType = 0x%02lX\n",	 d.bDescriptorType);
	t.Trace( t.m_StreamTraceLevel, "wTotalLength = 0x%04lX\n",		 d.wTotalLength);
	t.Trace( t.m_StreamTraceLevel, "bNumInterfaces = 0x%02lX\n",	 d.bNumInterfaces);
	t.Trace( t.m_StreamTraceLevel, "bConfigurationValue = 0x%02lX\n",d.bConfigurationValue);
	t.Trace( t.m_StreamTraceLevel, "iConfiguration = 0x%02lX\n",	 d.iConfiguration);
	t.Trace( t.m_StreamTraceLevel, "bmAttributes = 0x%02lX\n",		 d.bmAttributes);
	t.Trace( t.m_StreamTraceLevel, "MaxPower = 0x%02lX\n",			 d.MaxPower);
	return t;
}

KTrace& operator << (KTrace& t, const USB_INTERFACE_DESCRIPTOR& d)
{
 	t.Trace( t.m_StreamTraceLevel, "USB_INTERFACE_DESCRIPTOR 0x%08lX\n", ULONG_PTR(&d));
	t.Trace( t.m_StreamTraceLevel, "bLength = 0x%02lX\n",			d.bLength);
	t.Trace( t.m_StreamTraceLevel, "bDescriptorType = 0x%02lX\n",	d.bDescriptorType);
	t.Trace( t.m_StreamTraceLevel, "bInterfaceNumber = 0x%02lX\n",	d.bInterfaceNumber);
	t.Trace( t.m_StreamTraceLevel, "bAlternateSetting = 0x%02lX\n",	d.bAlternateSetting);
	t.Trace( t.m_StreamTraceLevel, "bNumEndpoints = 0x%02lX\n",		d.bNumEndpoints);
	t.Trace( t.m_StreamTraceLevel, "bInterfaceClass = 0x%02lX\n",	d.bInterfaceClass);
	t.Trace( t.m_StreamTraceLevel, "bInterfaceSubClass = 0x%02lX\n",d.bInterfaceSubClass);
	t.Trace( t.m_StreamTraceLevel, "bInterfaceProtocol = 0x%02lX\n",d.bInterfaceProtocol);
	t.Trace( t.m_StreamTraceLevel, "iInterface = 0x%02lX\n",		d.iInterface);
	return t;
}

KTrace& operator << (KTrace& t, const USB_ENDPOINT_DESCRIPTOR& d)
{
 	t.Trace( t.m_StreamTraceLevel, "USB_ENDPOINT_DESCRIPTOR 0x%08lX\n", ULONG_PTR(&d));
	t.Trace( t.m_StreamTraceLevel, "bLength = 0x%02lX\n",			d.bLength);
	t.Trace( t.m_StreamTraceLevel, "bDescriptorType = 0x%02lX\n",	d.bDescriptorType);
	t.Trace( t.m_StreamTraceLevel, "bEndpointAddress = 0x%02lX\n",	d.bEndpointAddress);
	t.Trace( t.m_StreamTraceLevel, "bmAttributes = 0x%02lX\n",		d.bmAttributes);
	t.Trace( t.m_StreamTraceLevel, "wMaxPacketSize = 0x%04lX\n",	d.wMaxPacketSize);
	t.Trace( t.m_StreamTraceLevel, "bInterval = 0x%02lX\n",			d.bInterval);
	return t;
}

KTrace& operator << (KTrace& t, const USB_STRING_DESCRIPTOR& d)
{
 	t.Trace( t.m_StreamTraceLevel, "USB_STRING_DESCRIPTOR 0x%08lX\n", ULONG_PTR(&d));
	t.Trace( t.m_StreamTraceLevel, "bLength = 0x%02lX\n",		  d.bLength);
	t.Trace( t.m_StreamTraceLevel, "bDescriptorType = 0x%02lX\n", d.bDescriptorType);
	t.Trace( t.m_StreamTraceLevel, "bString = %S\n",			  d.bString);
	return t;
}

KTrace& operator << (KTrace& t, const USBD_PIPE_INFORMATION& pipe)
{
 	t.Trace( t.m_StreamTraceLevel, "USBD_PIPE_INFORMATION 0x%08lX\n",   ULONG_PTR(&pipe));
	t.Trace( t.m_StreamTraceLevel, "MaximumPacketSize = 0x%04lX\n",		pipe.MaximumPacketSize);
	t.Trace( t.m_StreamTraceLevel, "EndpointAddress = 0x%02lX\n",		pipe.EndpointAddress);
	t.Trace( t.m_StreamTraceLevel, "Interval = 0x%02lX\n",				pipe.Interval);
	t.Trace( t.m_StreamTraceLevel, "PipeType = 0x%08lX\n",				pipe.PipeType);
	t.Trace( t.m_StreamTraceLevel, "PipeHandle = 0x%08lX\n",			pipe.PipeHandle);
	t.Trace( t.m_StreamTraceLevel, "MaximumTransferSize = 0x%08lX\n",	pipe.MaximumTransferSize);
	t.Trace( t.m_StreamTraceLevel, "PipeFlags = 0x%08lX\n",				pipe.PipeFlags);
	return t;
}

KTrace& operator << (KTrace& t, const USBD_INTERFACE_INFORMATION& intf)
{
 	t.Trace( t.m_StreamTraceLevel, "USBD_INTERFACE_INFORMATION 0x%08lX\n",   ULONG_PTR(&intf));
	t.Trace( t.m_StreamTraceLevel, "Length = 0x%04lX\n",			intf.Length);
	t.Trace( t.m_StreamTraceLevel, "InterfaceNumber = 0x%02lX\n",	intf.InterfaceNumber);
	t.Trace( t.m_StreamTraceLevel, "AlternateSetting = 0x%02lX\n",	intf.AlternateSetting);
	t.Trace( t.m_StreamTraceLevel, "Class = 0x%02lX\n",				intf.Class);
	t.Trace( t.m_StreamTraceLevel, "SubClass = 0x%02lX\n",			intf.SubClass);
	t.Trace( t.m_StreamTraceLevel, "Protocol = 0x%02lX\n",			intf.Protocol);
	t.Trace( t.m_StreamTraceLevel, "Reserved = 0x%02lX\n",			intf.Reserved);
	t.Trace( t.m_StreamTraceLevel, "InterfaceHandle = 0x%08lX\n",	intf.InterfaceHandle);
	t.Trace( t.m_StreamTraceLevel, "NumberOfPipes = 0x%08lX\n",		intf.NumberOfPipes);
	t.Trace( t.m_StreamTraceLevel, "Pipes[0] = 0x%08lX\n",			ULONG_PTR(intf.Pipes));
	return t;
}

KTrace& operator << (KTrace& t, const _URB_HEADER& u)
{
	t.Trace( t.m_StreamTraceLevel, "Length = 0x%04lX\n",			u.Length);
	t.Trace( t.m_StreamTraceLevel, "Function = 0x%04lX\n",			u.Function);
	t.Trace( t.m_StreamTraceLevel, "Status = 0x%08lX\n",			u.Status);
	t.Trace( t.m_StreamTraceLevel, "UsbdDeviceHandle = 0x%08lX\n",	u.UsbdDeviceHandle);
	t.Trace( t.m_StreamTraceLevel, "UsbdFlags = 0x%08lX\n",			u.UsbdFlags);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_CONTROL_DESCRIPTOR_REQUEST& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_CONTROL_DESCRIPTOR_REQUEST 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
 	t.Trace( t.m_StreamTraceLevel, "Reserved = 0x%08lX\n",				u.Reserved);
	t.Trace( t.m_StreamTraceLevel, "Reserved0 = 0x%08lX\n",				u.Reserved0);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferLength = 0x%08lX\n",	u.TransferBufferLength);
	t.Trace( t.m_StreamTraceLevel, "TransferBuffer = 0x%08lX\n",		u.TransferBuffer);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferMDL = 0x%08lX\n",		u.TransferBufferMDL);
	t.Trace( t.m_StreamTraceLevel, "UrbLink = 0x%08lX\n",				u.UrbLink);
	t << u.hca;
	t.Trace( t.m_StreamTraceLevel, "Reserved1 = 0x%08lX\n",		u.Reserved1);
	t.Trace( t.m_StreamTraceLevel, "Index = 0x%02lX\n",			u.Index);
	t.Trace( t.m_StreamTraceLevel, "DescriptorType = 0x%02lX\n",u.DescriptorType);
	t.Trace( t.m_StreamTraceLevel, "LanguageId = 0x%04lX\n",	u.LanguageId);
	t.Trace( t.m_StreamTraceLevel, "Reserved2 = 0x%04lX\n",		u.Reserved2);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_CONTROL_GET_INTERFACE_REQUEST& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_CONTROL_GET_INTERFACE_REQUEST 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "Reserved = 0x%08lX\n",				u.Reserved);
	t.Trace( t.m_StreamTraceLevel, "Reserved0 = 0x%08lX\n",				u.Reserved0);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferLength = 0x%08lX\n",	u.TransferBufferLength);
	t.Trace( t.m_StreamTraceLevel, "TransferBuffer = 0x%08lX\n",		u.TransferBuffer);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferMDL = 0x%08lX\n",		u.TransferBufferMDL);
	t.Trace( t.m_StreamTraceLevel, "UrbLink = 0x%08lX\n",				u.UrbLink);
	t << u.hca;
	for( UCHAR i = 0; i < 4; i++)
		t.Trace( t.m_StreamTraceLevel, "Reserved1[%d] = 0x%08lX\n",	i, u.Reserved1[i]);
	t.Trace( t.m_StreamTraceLevel, "Interface = 0x%08lX\n", u.Interface);
	t.Trace( t.m_StreamTraceLevel, "Reserved2 = 0x%08lX\n", u.Reserved2);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_CONTROL_VENDOR_OR_CLASS_REQUEST& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_CONTROL_VENDOR_OR_CLASS_REQUEST 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "TransferFlags = 0x%08lX\n",			u.TransferFlags);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferLength = 0x%08lX\n",	u.TransferBufferLength);
	t.Trace( t.m_StreamTraceLevel, "TransferBuffer = 0x%08lX\n",		u.TransferBuffer);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferMDL = 0x%08lX\n",		u.TransferBufferMDL);
	t.Trace( t.m_StreamTraceLevel, "UrbLink = 0x%08lX\n",				u.UrbLink);
	t << u.hca;
	t.Trace( t.m_StreamTraceLevel, "RequestTypeReservedBits = 0x%02lX\n", u.RequestTypeReservedBits);
	t.Trace( t.m_StreamTraceLevel, "Request = 0x%02lX\n",	u.Request);
	t.Trace( t.m_StreamTraceLevel, "Value = 0x%04lX\n",		u.Value);
	t.Trace( t.m_StreamTraceLevel, "Index = 0x%04lX\n",		u.Index);
	return t;
}

KTrace& operator << (KTrace& t, const _USBD_ISO_PACKET_DESCRIPTOR& d)
{
 	t.Trace( t.m_StreamTraceLevel, "_USBD_ISO_PACKET_DESCRIPTOR 0x%08lX\n", ULONG_PTR(&d));
	t.Trace( t.m_StreamTraceLevel, "Offset = 0x%08lX\n", d.Offset);
	t.Trace( t.m_StreamTraceLevel, "Length = 0x%08lX\n", d.Length);
	t.Trace( t.m_StreamTraceLevel, "Status = 0x%08lX\n", d.Status);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_ISOCH_TRANSFER& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_ISOCH_TRANSFER 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "PipeHandle = 0x%08lX\n",			u.PipeHandle);
	t.Trace( t.m_StreamTraceLevel, "TransferFlags = 0x%08lX\n",			u.TransferFlags);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferLength = 0x%08lX\n",	u.TransferBufferLength);
	t.Trace( t.m_StreamTraceLevel, "TransferBuffer = 0x%08lX\n",		u.TransferBuffer);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferMDL = 0x%08lX\n",		u.TransferBufferMDL);
	t.Trace( t.m_StreamTraceLevel, "UrbLink = 0x%08lX\n",				u.UrbLink);
	t << u.hca;
	t.Trace( t.m_StreamTraceLevel, "StartFrame = 0x%08lX\n", u.StartFrame);
	t.Trace( t.m_StreamTraceLevel, "NumberOfPackets = 0x%08lX\n",	u.NumberOfPackets);
	t.Trace( t.m_StreamTraceLevel, "ErrorCount = 0x%08lX\n",		u.ErrorCount);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_SELECT_CONFIGURATION& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_SELECT_CONFIGURATION 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "ConfigurationDescriptor = 0x%08lX\n", u.ConfigurationDescriptor);
	t.Trace( t.m_StreamTraceLevel, "ConfigurationHandle = 0x%08lX\n",	  u.ConfigurationHandle);
	t.Trace( t.m_StreamTraceLevel, "Interface = 0x%08lX\n",				  u.Interface);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_SELECT_INTERFACE& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_SELECT_INTERFACE 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "ConfigurationHandle = 0x%08lX\n",	  u.ConfigurationHandle);
	t.Trace( t.m_StreamTraceLevel, "Interface = 0x%08lX\n",				  u.Interface);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_PIPE_REQUEST& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_PIPE_REQUEST 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "PipeHandle = 0x%08lX\n", u.PipeHandle);
	t.Trace( t.m_StreamTraceLevel, "Reserved = 0x%08lX\n",	 u.Reserved);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_GET_FRAME_LENGTH& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_GET_FRAME_LENGTH 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "FrameLength = 0x%08lX\n", u.FrameLength);
	t.Trace( t.m_StreamTraceLevel, "FrameNumber = 0x%08lX\n", u.FrameNumber);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_SET_FRAME_LENGTH& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_SET_FRAME_LENGTH 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "FrameLengthDelta = 0x%08lX\n", u.FrameLengthDelta);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_GET_CURRENT_FRAME_NUMBER& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_GET_CURRENT_FRAME_NUMBER 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "FrameNumber = 0x%08lX\n", u.FrameNumber);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_CONTROL_TRANSFER& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_CONTROL_TRANSFER 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "PipeHandle = 0x%08lX\n",			u.PipeHandle);
	t.Trace( t.m_StreamTraceLevel, "TransferFlags = 0x%08lX\n",			u.TransferFlags);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferLength = 0x%08lX\n",	u.TransferBufferLength);
	t.Trace( t.m_StreamTraceLevel, "TransferBuffer = 0x%08lX\n",		u.TransferBuffer);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferMDL = 0x%08lX\n",		u.TransferBufferMDL);
	t.Trace( t.m_StreamTraceLevel, "UrbLink = 0x%08lX\n",				u.UrbLink);
	t << u.hca;
    for(UCHAR i = 0; i < 8; i++)
		t.Trace( t.m_StreamTraceLevel, "SetupPacket[%d] = 0x%02lX\n", i, u.SetupPacket[i]);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_CONTROL_GET_CONFIGURATION_REQUEST& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_CONTROL_GET_CONFIGURATION_REQUEST 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "Reserved = 0x%08lX\n",				u.Reserved);
	t.Trace( t.m_StreamTraceLevel, "Reserved0 = 0x%08lX\n",				u.Reserved0);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferLength = 0x%08lX\n",	u.TransferBufferLength);
	t.Trace( t.m_StreamTraceLevel, "TransferBuffer = 0x%08lX\n",		u.TransferBuffer);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferMDL = 0x%08lX\n",		u.TransferBufferMDL);
	t.Trace( t.m_StreamTraceLevel, "UrbLink = 0x%08lX\n",				u.UrbLink);
	t << u.hca;
    for(UCHAR i = 0; i < 8; i++)
		t.Trace( t.m_StreamTraceLevel, "Reserved1[%d] = 0x%02lX\n", i, u.Reserved1[i]);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_CONTROL_GET_STATUS_REQUEST& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_CONTROL_GET_STATUS_REQUEST 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "Reserved = 0x%08lX\n",				u.Reserved);
	t.Trace( t.m_StreamTraceLevel, "Reserved0 = 0x%08lX\n",				u.Reserved0);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferLength = 0x%08lX\n",	u.TransferBufferLength);
	t.Trace( t.m_StreamTraceLevel, "TransferBuffer = 0x%08lX\n",		u.TransferBuffer);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferMDL = 0x%08lX\n",		u.TransferBufferMDL);
	t.Trace( t.m_StreamTraceLevel, "UrbLink = 0x%08lX\n",				u.UrbLink);
	t << u.hca;
    for(UCHAR i = 0; i < 4; i++)
		t.Trace( t.m_StreamTraceLevel, "Reserved1[%d] = 0x%02lX\n", i, u.Reserved1[i]);
	t.Trace( t.m_StreamTraceLevel, "Index = 0x%04lX\n",				u.Index);
	t.Trace( t.m_StreamTraceLevel, "Reserved2 = 0x%04lX\n",			u.Reserved2);
	return t;
}

KTrace& operator << (KTrace& t, const _URB_BULK_OR_INTERRUPT_TRANSFER& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_BULK_OR_INTERRUPT_TRANSFER 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;
	t.Trace( t.m_StreamTraceLevel, "PipeHandle = 0x%08lX\n",			u.PipeHandle);
	t.Trace( t.m_StreamTraceLevel, "TransferFlags = 0x%08lX\n",			u.TransferFlags);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferLength = 0x%08lX\n",	u.TransferBufferLength);
	t.Trace( t.m_StreamTraceLevel, "TransferBuffer = 0x%08lX\n",		u.TransferBuffer);
	t.Trace( t.m_StreamTraceLevel, "TransferBufferMDL = 0x%08lX\n",		u.TransferBufferMDL);
	t.Trace( t.m_StreamTraceLevel, "UrbLink = 0x%08lX\n",				u.UrbLink);
	t << u.hca;
	return t;
}

KTrace& operator << (KTrace& t, const _URB_HCD_AREA& hcd)
{
#if ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION <= 0x10))
	t.Trace( t.m_StreamTraceLevel, "HcdEndpoint = 0x%08lX\n", hcd.HcdEndpoint);
	t.Trace( t.m_StreamTraceLevel, "HcdIrp = 0x%08lX\n", hcd.HcdIrp);
	t.Trace( t.m_StreamTraceLevel, "HcdListEntry.Flink = 0x%08lX\n", hcd.HcdListEntry.Flink);
	t.Trace( t.m_StreamTraceLevel, "HcdListEntry.Blink = 0x%08lX\n", hcd.HcdListEntry.Blink);
	t.Trace( t.m_StreamTraceLevel, "HcdListEntry2.Flink = 0x%08lX\n", hcd.HcdListEntry2.Flink);
	t.Trace( t.m_StreamTraceLevel, "HcdListEntry2.Blink = 0x%08lX\n", hcd.HcdListEntry2.Blink);
	t.Trace( t.m_StreamTraceLevel, "HcdCurrentIoFlushPointer = 0x%08lX\n", hcd.HcdCurrentIoFlushPointer);
	t.Trace( t.m_StreamTraceLevel, "HcdExtension = 0x%08lX\n", hcd.HcdExtension);
#else
	t.Trace( t.m_StreamTraceLevel, "Reserved8[0] = 0x%08lX\n", hcd.Reserved8[0]);
	t.Trace( t.m_StreamTraceLevel, "Reserved8[1] = 0x%08lX\n", hcd.Reserved8[1]);
	t.Trace( t.m_StreamTraceLevel, "Reserved8[2] = 0x%08lX\n", hcd.Reserved8[2]);
	t.Trace( t.m_StreamTraceLevel, "Reserved8[3] = 0x%08lX\n", hcd.Reserved8[3]);
	t.Trace( t.m_StreamTraceLevel, "Reserved8[4] = 0x%08lX\n", hcd.Reserved8[4]);
	t.Trace( t.m_StreamTraceLevel, "Reserved8[5] = 0x%08lX\n", hcd.Reserved8[5]);
	t.Trace( t.m_StreamTraceLevel, "Reserved8[6] = 0x%08lX\n", hcd.Reserved8[6]);
	t.Trace( t.m_StreamTraceLevel, "Reserved8[7] = 0x%08lX\n", hcd.Reserved8[7]);
#endif
	return t;
}

KTrace& operator << (KTrace& t, const _URB_CONTROL_FEATURE_REQUEST& u)
{
 	t.Trace( t.m_StreamTraceLevel, "_URB_CONTROL_FEATURE_REQUEST 0x%08lX\n", ULONG_PTR(&u));
	t << u.Hdr;

//2000 DDK or 98 DDK
#if defined(USBD_PF_FAST_ISO) || ((WDM_MAJORVERSION ==1) && (WDM_MINORVERSION == 0))
	for(UCHAR i = 0; i < 20; i++)
		t.Trace( t.m_StreamTraceLevel, "Reserved[%d] = 0x%02lX\n", i, u.Reserved[i]);
#else  //Whistler DDK
		t.Trace( t.m_StreamTraceLevel, "Reserved = 0x%p\n", u.Reserved);
		t.Trace( t.m_StreamTraceLevel, "Reserved2 = 0x%021X\n", u.Reserved2);
		t.Trace( t.m_StreamTraceLevel, "Reserved3 = 0x%021X\n", u.Reserved3);
		t.Trace( t.m_StreamTraceLevel, "Reserved4 = 0x%p\n", u.Reserved4);
		t.Trace( t.m_StreamTraceLevel, "Reserved5 = 0x%p\n", u.Reserved5);
#endif
	t.Trace( t.m_StreamTraceLevel, "UrbLink = 0x%08lX\n", u.UrbLink);
	t << u.hca;
	t.Trace( t.m_StreamTraceLevel, "Reserved0 = 0x%04lX\n",		  u.Reserved0);
	t.Trace( t.m_StreamTraceLevel, "FeatureSelector = 0x%04lX\n", u.FeatureSelector);
	t.Trace( t.m_StreamTraceLevel, "Index = 0x%04lX\n",			  u.Index);
	t.Trace( t.m_StreamTraceLevel, "Reserved1 = 0x%04lX\n",		  u.Reserved1);
	return t;
}

KTrace& operator << (KTrace& t, const URB& urb)
{
	switch(urb.UrbHeader.Function)
	{
	case URB_FUNCTION_SELECT_CONFIGURATION:
		t << urb.UrbSelectConfiguration;
		break;

	case URB_FUNCTION_SELECT_INTERFACE:
		t << urb.UrbSelectInterface;
		break;

	case URB_FUNCTION_RESET_PIPE:
	case URB_FUNCTION_ABORT_PIPE:
		t << urb.UrbPipeRequest;
		break;

	case URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL:
	case URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL:
 		t.Trace( t.m_StreamTraceLevel, "_URB_FRAME_LENGTH_CONTROL 0x%08lX\n", ULONG_PTR(&urb));
		t << urb.UrbHeader;
		break;

	case URB_FUNCTION_GET_FRAME_LENGTH:
		t << urb.UrbGetFrameLength;
		break;

	case URB_FUNCTION_SET_FRAME_LENGTH:
		t << urb.UrbSetFrameLength;
		break;

	case URB_FUNCTION_GET_CURRENT_FRAME_NUMBER:
		t << urb.UrbGetCurrentFrameNumber;
		break;

	case URB_FUNCTION_CONTROL_TRANSFER:
        t << urb.UrbControlTransfer;
		break;

	case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
		t << urb.UrbBulkOrInterruptTransfer;
		break;

	case URB_FUNCTION_ISOCH_TRANSFER:
		t << urb.UrbIsochronousTransfer;
		break;

	case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
		t << urb.UrbControlDescriptorRequest;
		break;

	case URB_FUNCTION_SET_FEATURE_TO_DEVICE:
	case URB_FUNCTION_SET_FEATURE_TO_INTERFACE:
	case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT:
	case URB_FUNCTION_SET_FEATURE_TO_OTHER:
	case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE:
	case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE:
	case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT:
	case URB_FUNCTION_CLEAR_FEATURE_TO_OTHER:
		t << urb.UrbControlFeatureRequest;
		break;


	case URB_FUNCTION_GET_STATUS_FROM_DEVICE:
	case URB_FUNCTION_GET_STATUS_FROM_INTERFACE:
	case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT:
	case URB_FUNCTION_GET_STATUS_FROM_OTHER:
		t << urb.UrbControlGetStatusRequest;
		break;

	case URB_FUNCTION_VENDOR_DEVICE:
	case URB_FUNCTION_VENDOR_INTERFACE:
	case URB_FUNCTION_VENDOR_ENDPOINT:
	case URB_FUNCTION_VENDOR_OTHER:
	case URB_FUNCTION_CLASS_DEVICE:
	case URB_FUNCTION_CLASS_INTERFACE:
	case URB_FUNCTION_CLASS_ENDPOINT:
	case URB_FUNCTION_CLASS_OTHER:
		t << urb.UrbControlVendorClassRequest;
		break;

	case URB_FUNCTION_GET_CONFIGURATION:
		t << urb.UrbControlGetConfigurationRequest;
		break;

	case URB_FUNCTION_GET_INTERFACE:
		t << urb.UrbControlGetInterfaceRequest;
		break;

	default:
		break;
	}

	return t;
}

#endif // !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

#endif // _WDM_
