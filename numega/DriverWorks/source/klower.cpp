// klower.cpp - implementation of class KLowerDevice
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

////////////////////////////////////////////////////////////////
// static data

KEVENT KLowerDevice::m_DummyEvent;

////////////////////////////////////////////////////////////////
// Constructors

// used by KDevice::Attach
KLowerDevice::KLowerDevice(void) :
	m_ConstructorStatus(STATUS_SUCCESS),
	m_pDeviceObject(NULL),
	m_pFileObject(NULL)
{
}

KLowerDevice::KLowerDevice(IN PDEVICE_OBJECT pDeviceObject) :
	m_ConstructorStatus(STATUS_SUCCESS),
	m_pDeviceObject(pDeviceObject),
	m_pFileObject(NULL)
{
	// Only report construction if device is valid (not a NULL device pointer),
	// otherwise this is really a default construction, and the construction
	// will be reported in Initialize()
	if (m_pDeviceObject != NULL)
	{
		BOUNDS_CHECKER(CONSTRUCTED_LOWER_DEVICE, (this, m_pDeviceObject, STATUS_SUCCESS));
	}
}

NTSTATUS KLowerDevice::Initialize(IN PDEVICE_OBJECT pDeviceObject)
{
	m_pDeviceObject = pDeviceObject;
	BOUNDS_CHECKER(CONSTRUCTED_LOWER_DEVICE, (this, m_pDeviceObject, STATUS_SUCCESS));
	return STATUS_SUCCESS;
}

KLowerDevice::KLowerDevice(IN PUNICODE_STRING name, IN ACCESS_MASK access) :
	m_pDeviceObject(NULL),
	m_pFileObject(NULL)
{
	m_ConstructorStatus =
		IoGetDeviceObjectPointer(name, access, &m_pFileObject, &m_pDeviceObject);
	BOUNDS_CHECKER(CONSTRUCTED_LOWER_DEVICE, (this, m_pDeviceObject, m_ConstructorStatus));
}

NTSTATUS KLowerDevice::Initialize(IN PUNICODE_STRING name, IN ACCESS_MASK access)
{
	m_ConstructorStatus =
		IoGetDeviceObjectPointer(name, access, &m_pFileObject, &m_pDeviceObject);
	BOUNDS_CHECKER(CONSTRUCTED_LOWER_DEVICE, (this, m_pDeviceObject, m_ConstructorStatus));
	return m_ConstructorStatus;
}

KLowerDevice::KLowerDevice(IN PCWSTR name, IN ACCESS_MASK access) :
	m_pDeviceObject(NULL),
	m_pFileObject(NULL)
{
	UNICODE_STRING u;
	RtlInitUnicodeString(&u, name);

	m_ConstructorStatus =
		IoGetDeviceObjectPointer(&u, access, &m_pFileObject, &m_pDeviceObject);
	BOUNDS_CHECKER(CONSTRUCTED_LOWER_DEVICE, (this, m_pDeviceObject, m_ConstructorStatus));
}

NTSTATUS KLowerDevice::Initialize(IN PCWSTR name, IN ACCESS_MASK access)
{
	UNICODE_STRING u;
	RtlInitUnicodeString(&u, name);

	m_ConstructorStatus =
		IoGetDeviceObjectPointer(&u, access, &m_pFileObject, &m_pDeviceObject);
	BOUNDS_CHECKER(CONSTRUCTED_LOWER_DEVICE, (this, m_pDeviceObject, m_ConstructorStatus));
	return m_ConstructorStatus;
}

////////////////////////////////////////////////////////////////
// Call device, wait for completion

NTSTATUS KLowerDevice::CallWaitComplete(KIrp I, BOOLEAN bKeepIrp, ULONG_PTR* pInformation)
{
	ASSERT (KeGetCurrentIrql() < DISPATCH_LEVEL);

	SynchCallCompletionContext_t Ctx;
	KEvent e(NotificationEvent);
	PIO_STACK_LOCATION nextStack = IoGetNextIrpStackLocation(I.m_Irp);

	// Initialize context structure

	Ctx.m_pEvent = &e;
	Ctx.m_Completion = nextStack->CompletionRoutine;
	Ctx.m_CompletionContext = nextStack->Context;
	Ctx.m_KeepIrp = bKeepIrp;

	// Set completion handler, context is addr of context struct

	I.SetCompletionRoutine(
		SynchCallCompletionRoutine,
		&Ctx,
		TRUE,
		TRUE,
		TRUE
		);

	// Call the lower device

	BOUNDS_CHECKER(CALL_LOWER_DEVICE, (this, PIRP(I)));

	IoCallDriver(m_pDeviceObject, I.m_Irp);

	// Wait for completion

	e.Wait();

	// extract information from context structure

	if (pInformation)
		*pInformation = Ctx.m_Information;

	BOUNDS_CHECKER(COMPLETED_CALLDOWN, (this, Ctx.m_Status, Ctx.m_Information));

	return Ctx.m_Status;
}

///////////// Completion routine for CallSynch /////////////////////

NTSTATUS KLowerDevice::SynchCallCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
{
	NTSTATUS status;

	SynchCallCompletionContext_t* Ctx = (SynchCallCompletionContext_t*)Context;

	// Call the original completion routine, if any

	if (Ctx->m_Completion)
		status = Ctx->m_Completion(DeviceObject, Irp, Ctx->m_CompletionContext);

	// otherwise, normal pend bit propagation

	else
	{
		if (Ctx->m_KeepIrp)
			status = STATUS_MORE_PROCESSING_REQUIRED;
		else
		{
			if (Irp->PendingReturned)
				IoMarkIrpPending(Irp);

			status = STATUS_SUCCESS;
		}
	}

	// load up the context structure

	Ctx->m_Information = Irp->IoStatus.Information;
	Ctx->m_Status = Irp->IoStatus.Status;

	// signal the event to release waiting thread

	KeSetEvent(*Ctx->m_pEvent, IO_NO_INCREMENT, FALSE);

	return status;
}


////////////////////////////////////////////////////////////////
// DeviceIoControl

NTSTATUS KLowerDevice::DeviceIoControl(
	ULONG IoControlCode,
	PVOID InputBuffer,
	ULONG InputBufferSize,
	PVOID OutputBuffer,
	ULONG OutputBufferLength,
	BOOLEAN Internal,
	ULONG_PTR* pInformation
	)
{
	PIRP pIrp;
	IO_STATUS_BLOCK StatusBlock;
	KEvent event(NotificationEvent, FALSE);
	NTSTATUS status;

	pIrp = IoBuildDeviceIoControlRequest(
		IoControlCode,
		m_pDeviceObject,
		InputBuffer,
		InputBufferSize,
		OutputBuffer,
		OutputBufferLength,
		Internal,
		(PKEVENT)event.Object(),
		&StatusBlock);

	if (pIrp == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	BOUNDS_CHECKER(CALL_LOWER_DEVICE, (this, pIrp));

	status = IoCallDriver(m_pDeviceObject, pIrp);

	if (status == STATUS_PENDING)
	{
		event.Wait();
		status = StatusBlock.Status;
	}

	*pInformation = StatusBlock.Information;

	BOUNDS_CHECKER(COMPLETED_CALLDOWN, (this, status, *pInformation));

	return status;
}

NTSTATUS KLowerDevice::DeviceIoControl(
	ULONG IoControlCode,
	PVOID InputBuffer,
	ULONG InputBufferSize,
	PVOID OutputBuffer,
	ULONG OutputBufferLength,
	BOOLEAN Internal,
	PIO_COMPLETION_ROUTINE CompletionHandler,
	PVOID CompletionContext,
	PIO_STATUS_BLOCK pIoStatus
	)
{
	PIRP pIrp;

	KeInitializeEvent(&m_DummyEvent, NotificationEvent, FALSE);

	pIrp = IoBuildDeviceIoControlRequest(
		IoControlCode,
		m_pDeviceObject,
		InputBuffer,
		InputBufferSize,
		OutputBuffer,
		OutputBufferLength,
		Internal,
		&m_DummyEvent,
		pIoStatus
		);

	if (pIrp == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	IoSetCompletionRoutine(
		pIrp,
		CompletionHandler,
		CompletionContext,
		TRUE, TRUE, TRUE
		);

	BOUNDS_CHECKER(CALL_LOWER_DEVICE, (this, pIrp));

	return IoCallDriver(m_pDeviceObject, pIrp);
}


/////////////////////////////////////////////////////////////////////
// Protected routines for internal use by Read/Write/DeviceIoControl

NTSTATUS KLowerDevice::SynchronousCalldown(
	ULONG MajorFunction,
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER StartingOffset,
	ULONG_PTR * pInformation
	)
{
	PIRP pIrp;
	KEvent Event(NotificationEvent, FALSE);
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS status;
	LARGE_INTEGER loffset, *pOff;

	if ( StartingOffset == NULL )
	{
		loffset.HighPart = loffset.LowPart = 0;
		pOff = &loffset;
	}
	else
		pOff = StartingOffset;

	pIrp = IoBuildSynchronousFsdRequest(
		MajorFunction,
		m_pDeviceObject,
		Buffer,
		Length,
		pOff,
		(PKEVENT)Event.Object(),
		&IoStatus
		);

	if (pIrp == NULL)
		return STATUS_UNSUCCESSFUL;

	BOUNDS_CHECKER(CALL_LOWER_DEVICE, (this, pIrp));

	status = IoCallDriver(m_pDeviceObject, pIrp);

	if (status == STATUS_PENDING)
	{
		Event.Wait();
		status = IoStatus.Status;
	}

	if(pInformation != NULL)
		*pInformation = IoStatus.Information;

	BOUNDS_CHECKER(COMPLETED_CALLDOWN, (this, status, IoStatus.Information));

	return status;
}

NTSTATUS KLowerDevice::AsynchronousCalldown(
	ULONG MajorFunction,
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER StartingOffset,
	PIO_STATUS_BLOCK pIoStatus,
	PIO_COMPLETION_ROUTINE CompletionHandler,
	PVOID CompletionContext)
{
	PIRP pIrp;
	LARGE_INTEGER loffset, *pOff;

	if ( StartingOffset == NULL )
	{
		loffset.HighPart = loffset.LowPart = 0;
		pOff = &loffset;
	}
	else
		pOff = StartingOffset;

	pIrp = IoBuildAsynchronousFsdRequest(
		MajorFunction,
		m_pDeviceObject,
		Buffer,
		Length,
		pOff,
		pIoStatus);

	if (pIrp == NULL)
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR, (this, "KLowerDevice::AsynchronousCalldown", sizeof(_IRP)));
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		if (!CompletionHandler)
		{
			CompletionHandler = DefaultCompletionHandler;
			CompletionContext = this;
		}

		IoSetCompletionRoutine(
			pIrp,
			CompletionHandler,
			CompletionContext,
			TRUE, TRUE, TRUE);

		BOUNDS_CHECKER(CALL_LOWER_DEVICE, (this, pIrp));

		return IoCallDriver(m_pDeviceObject, pIrp);
	}
}

NTSTATUS KLowerDevice::DefaultCompletionHandler(
   	PDEVICE_OBJECT DeviceObject,
   	PIRP Irp,
   	PVOID Context)
{
	BOUNDS_CHECKER(COMPLETED_CALLDOWN, ((KLowerDevice*)Context, Irp->IoStatus.Status, Irp->IoStatus.Information));

	ASSERT(Irp);

	KIrp I(Irp);
	ULONG flags = I->Flags;

	// check for buffered I/O
	if ( IRP_BUFFERED_IO & flags )
	{
		// Check if this was an input operation.  If so, the
		// system buffer needs to be copied to the user buffer
		if ( IRP_INPUT_OPERATION & flags )
		{
			if ( ! NT_ERROR(I.Status()) )
			{
				ULONG_PTR size = I.Information();
				PVOID pSource = I.BufferedReadDest();
				PVOID pDest = I.UserBuffer();

				ASSERT(pSource);
				ASSERT(pDest);

				if ( pSource && pDest && size )
				{
					// copy the system buffer to the user buffer
					RtlCopyMemory(pDest,pSource,size);
				}
			}
		}

		// Check if the system buffer needs to be deallocated
		if ( IRP_DEALLOCATE_BUFFER & flags )
		{
			ExFreePool(I.BufferedReadDest());
		}
	}

	// Free all MDL's associated with this IRP
	PMDL pMdl = I->MdlAddress;
	while ( pMdl )
	{
		MmUnlockPages(pMdl);
		IoFreeMdl(pMdl);
		pMdl = pMdl->Next;
	}

	// Free the IRP
	IoFreeIrp(Irp);

	return STATUS_MORE_PROCESSING_REQUIRED;

	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(DeviceObject);
}
