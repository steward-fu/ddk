// kwdmfltr.cpp - implementation of WDM filter device
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

#ifndef __KWDM_FILTER__
#define __KWDM_FILTER__

#include <vdw.h>

/////////////////////////////////////////////////////////
//KWdmFilterDevice::AttachFilter
//
// This function should be called during the filter driver's
// AddDevice function. (Device constructor) It initializes the
// lower device, attaching our device, and sets some flags so that
// our device appears the same to the system.
//
// Parameter
//   PhysicalDeviceObject  - The physical device object pointer
//                            passed to AddDevice
//
NTSTATUS KWdmFilterDevice::AttachFilter(PDEVICE_OBJECT PhysicalDeviceObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	m_pLowerDevice = new (NonPagedPool) KPnpLowerDevice(
		this,
		PhysicalDeviceObject,
		&status
		);

	if(m_pLowerDevice == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	m_AllocatedFilterDevice = TRUE;

	status = m_pLowerDevice->ConstructorStatus();
	if (NT_SUCCESS(status))
	{
		SetupLowerDevice();
	}

	return status;
}

/////////////////////////////////////////////////////////
//KWdmFilterDevice::AttachFilter
//
// This function should be called during the filter driver's
// AddDevice function. (Device constructor) This form allows the user
// to specify his own lower device object so that the class can be used
// with a class derived from KPnpLowerDevice.
// We take care of informing the system of the lower device and setting the
// flags so that our device appears the same to the system.
//
// Parameter
//   LowerDeviceObject  - The physical device object pointer
//                            passed to AddDevice
//
NTSTATUS KWdmFilterDevice::AttachFilter(KPnpLowerDevice* LowerDeviceObject)
{
	NTSTATUS status = STATUS_SUCCESS;

	if(LowerDeviceObject == NULL)
		return STATUS_INVALID_PARAMETER;

	m_pLowerDevice = LowerDeviceObject;

	status = m_pLowerDevice->ConstructorStatus();
	if (NT_SUCCESS(status))
	{
		SetupLowerDevice();
	}

	return status;
}
/////////////////////////////////////////////////////////
//KWdmFilterDevice::SetupLowerDevice
//
// This function is an internal routine called from within AttachFilter.
// It sets up the device object with the proper flags, copied from the
// lower device object.
//
// It is necesary to set our device type, characteristics, and
// flags to those of the lower device so that our device object
// looks the same to the system.  If it is not the same, we are
// changing the behavior of the system toward the device object.
//

VOID KWdmFilterDevice::SetupLowerDevice()
{

	PDEVICE_OBJECT LowerSystemDeviceObject = m_pLowerDevice->DeviceObject();
	//Set our device type to the lower device's type
	m_pDeviceObject->DeviceType =  LowerSystemDeviceObject->DeviceType;
	//Set our device characteristics to the lower device's charectoristics
	m_pDeviceObject->Characteristics = LowerSystemDeviceObject->Characteristics;
	//Set our device's flags to the lower device's flags
	m_pDeviceObject->Flags |= LowerSystemDeviceObject->Flags &
		(DO_DIRECT_IO | DO_BUFFERED_IO | DO_POWER_PAGABLE | DO_POWER_INRUSH);
	//Set our device's alignment to the lower device's alignment
	m_pDeviceObject->AlignmentRequirement = LowerSystemDeviceObject->AlignmentRequirement;

	//Inform the base class of our lower device.
	SetLowerDevice(m_pLowerDevice);

	m_FilterIsInitialized = TRUE;

}

////////////////////////////////////////////////////////////////////
// PassThrough
//
// Parameters:
//
// TheIrp - The IRP to pass through to the lower device
//
// pCompletionRoutine - An optional user completion routine
//
// pContext - The context to pass to the user completion
//			routine if one was specified.  This parameter is
//			igmored if the completion routine is NULL.
//
NTSTATUS KWdmFilterDevice::PassThrough(KIrp TheIrp,
									   PIO_COMPLETION_ROUTINE pCompletionRoutine,
									   PVOID pContext)
{
	if(!m_FilterIsInitialized)
		return STATUS_DEVICE_NOT_CONNECTED;


	if(pCompletionRoutine)
	{
		PPASS_THROUGH_CONTEXT theContext = new (NonPagedPool) PASS_THROUGH_CONTEXT;

		if(theContext == NULL)
			return STATUS_INSUFFICIENT_RESOURCES;
		//Save the user's completion routine and context to call from our
		// completion routine
		theContext->UserCompletionRoutine = pCompletionRoutine;
		theContext->pUserContext = pContext;
		theContext->pSelf = this;

		//Set up the lower device's stack
		TheIrp.CopyParametersDown();
		//Set the IRP's completion routine.
		TheIrp.SetCompletionRoutine(PassThroughComplete, theContext);

	}
	else
	{
		DecrementOutstandingRequestCount();
		TheIrp.ForceReuseOfCurrentStackLocationInCalldown();
	}
	//Call the lower device
	return m_pLowerDevice->Call(TheIrp);

}

///////////////////////////////////////////////////////////////////
NTSTATUS KWdmFilterDevice::PassThroughComplete(
											   PDEVICE_OBJECT DeviceObject,
											   PIRP TheIrp,
											   PVOID pContext
											   )
{
	KIrp I;
	NTSTATUS status;
	PPASS_THROUGH_CONTEXT theContext = (PPASS_THROUGH_CONTEXT)pContext;

	I.m_Irp = TheIrp;

	if (I->PendingReturned)
		I.MarkPending();

	//Call the user's completion routine if they set one
	// otherwise just return success.
	if(theContext->UserCompletionRoutine)
		status = (theContext->UserCompletionRoutine)(DeviceObject,
		TheIrp,
		theContext->pUserContext);
	else
		status = STATUS_SUCCESS;

	//Decrement the outstanding request count since our processing of the
	// IRP is done.
	(theContext->pSelf)->DecrementOutstandingRequestCount();

	//delete the context structure we allocated in PassThrough
	delete theContext;

	return status;
}

////////////////////////////////////////////////////////////////////
// PassThroughWait
//
// Parameters:
//
// TheIrp - The IRP to pass through to the lower device
//
// This routine should be used for all IRP's which do not have
//   IRP_MJ_POWER
NTSTATUS KWdmFilterDevice::PassThroughWait(KIrp TheIrp)
{
	//We're not connected to a lower device.  Just complete the IRP
	if(!m_FilterIsInitialized)
	{
		TheIrp.PnpComplete(STATUS_DEVICE_NOT_CONNECTED);
		return STATUS_DEVICE_NOT_CONNECTED;
	}

	TheIrp.ForceReuseOfCurrentStackLocationInCalldown();
	return m_pLowerDevice->CallWaitComplete(TheIrp, FALSE, NULL);
}


////////////////////////////////////////////////////////////////////
// PowerPassThrough
//
// Parameters:
//
// TheIrp - The power IRP to pass through to the lower device
//
// This routine should only be used for IRP's that have
//   IRP_MJ_POWER
NTSTATUS KWdmFilterDevice::PowerPassThrough(KIrp I)
{
	NTSTATUS status;

	//Don't use the power calls if this is not a power IRP.
	if(I.MajorFunction() != IRP_MJ_POWER)
		return PassThrough(I);

	I.IndicatePowerIrpProcessed();

	//We're not connected to a lower device.  Just complete the IRP
	if(!m_FilterIsInitialized)
	{
		I.PnpComplete(STATUS_DEVICE_NOT_CONNECTED);
		return STATUS_DEVICE_NOT_CONNECTED;
	}


	I.ForceReuseOfCurrentStackLocationInCalldown();
	status = m_pLowerDevice->PnpPowerCall(this,I);

	return status;
}

//////////////////////////////////////////////////////////////////
// SetFilterPnpPolicy
//

VOID KWdmFilterDevice::SetFilterPnpPolicy()
{
	//Set all the policies to FALSE.  Most of the policies
	//do not apply to filter devices as the lower device
	//should do most of the work.
	memset(&m_Policies, 0, sizeof(m_Policies));

	//Let DriverWorks detach our filter device
	m_Policies.m_GeneralPolicy.m_DetachPdoOnRemove = TRUE;
	//Wait for all IRPs to complete
	m_Policies.m_GeneralPolicy.m_WaitForSafeRemove = TRUE;
	//Make sure the system deletes our device object to avoid a memory leak
	m_Policies.m_GeneralPolicy.m_DeleteOnRemoveIfNotOpen = TRUE;
	m_Policies.m_GeneralPolicy.m_DeletePdoOnRemove = TRUE;

	//*********************************************************
	//The following policies can optionally be true or false.
	//All policies not listed here must remain false.
	m_Policies.m_GeneralPolicy.m_EnableDisableInterfaces = TRUE;

	m_Policies.m_QueryStopPolicy.m_CancelDeviceQueue = FALSE;
	m_Policies.m_QueryStopPolicy.m_CancelCurrentIrp = FALSE;

	m_Policies.m_QueryRemovePolicy.m_CancelDeviceQueue = TRUE;
	m_Policies.m_QueryRemovePolicy.m_CancelCurrentIrp = FALSE;

}

//////////////////////////////////////////////////////////////////
// SetFilterPowerPolicy
//

VOID KWdmFilterDevice::SetFilterPowerPolicy()
{
	//Most of the time, the filter device should not own the
	//power policy for the device stack.  By setting all the
	//policies to false, we pass down all power IRP's
	memset(&m_PowerPolicies, 0, sizeof(m_PowerPolicies));

}

///Override DefaultPnp with a PassThrough function.
NTSTATUS KWdmFilterDevice::DefaultPnp(KIrp I){return PassThrough(I);}
//Override DefaultPower with a PowerPassThrough function.
NTSTATUS KWdmFilterDevice::DefaultPower(KIrp I){return PowerPassThrough(I);}
NTSTATUS KWdmFilterDevice::SystemControl(KIrp I){return PassThrough(I);}

////////////////////////////////////////////////////////////////
// Replace all the driver dispatch routines with a pass through
// routine.

#define KFilterDispatch(Name) NTSTATUS KWdmFilterDevice::Name(KIrp I) { return PassThrough(I); }


#ifdef DRIVER_FUNCTION_CREATE
KFilterDispatch(Create)
#endif
#ifdef DRIVER_FUNCTION_CREATE_NAMED_PIPE
KFilterDispatch(CreateNamedPipe)
#endif
#ifdef DRIVER_FUNCTION_CLOSE
KFilterDispatch(Close)
#endif
#ifdef DRIVER_FUNCTION_CREATECLOSE
KFilterDispatch(CreateClose)
#endif
#ifdef DRIVER_FUNCTION_READ
KFilterDispatch(Read)
#endif
#ifdef DRIVER_FUNCTION_WRITE
KFilterDispatch(Write)
#endif
#ifdef DRIVER_FUNCTION_READWRITE
KFilterDispatch(ReadWrite)
#endif
#ifdef DRIVER_FUNCTION_QUERY_INFORMATION
KFilterDispatch(QueryInformation)
#endif
#ifdef DRIVER_FUNCTION_SET_INFORMATION
KFilterDispatch(SetInformation)
#endif
#ifdef DRIVER_FUNCTION_QUERY_EA
KFilterDispatch(QueryEa)
#endif
#ifdef DRIVER_FUNCTION_SET_EA
KFilterDispatch(SetEa)
#endif
#ifdef DRIVER_FUNCTION_FLUSH_BUFFERS
KFilterDispatch(FlushBuffers)
#endif
#ifdef DRIVER_FUNCTION_QUERY_VOLUME_INFORMATION
KFilterDispatch(QueryVolumeInformation)
#endif
#ifdef DRIVER_FUNCTION_SET_VOLUME_INFORMATION
KFilterDispatch(SetVolumeInformation)
#endif
#ifdef DRIVER_FUNCTION_DIRECTORY_CONTROL
KFilterDispatch(DirectoryControl)
#endif
#ifdef DRIVER_FUNCTION_FILE_SYSTEM_CONTROL
KFilterDispatch(FileSystemControl)
#endif
#ifdef DRIVER_FUNCTION_DEVICE_CONTROL
KFilterDispatch(DeviceControl)
#endif
#ifdef DRIVER_FUNCTION_INTERNAL_DEVICE_CONTROL
KFilterDispatch(InternalDeviceControl)
#endif
#ifdef DRIVER_FUNCTION_SHUTDOWN
KFilterDispatch(Shutdown)
#endif
#ifdef DRIVER_FUNCTION_LOCK_CONTROL
KFilterDispatch(LockControl)
#endif
#ifdef DRIVER_FUNCTION_CLEANUP
KFilterDispatch(CleanUp)
#endif
#ifdef DRIVER_FUNCTION_QUERY_SECURITY
KFilterDispatch(QuerySecurity)
#endif
#ifdef DRIVER_FUNCTION_SET_SECURITY
KFilterDispatch(SetSecurity)
#endif
#ifdef DRIVER_FUNCTION_CREATE_MAILSLOT
KFilterDispatch(CreateMailSlot)
#endif
#ifdef DRIVER_FUNCTION_DEVICE_CHANGE
KFilterDispatch(DeviceChange)
#endif
#ifdef DRIVER_FUNCTION_QUERY_QUOTA
KFilterDispatch(QueryQuota)
#endif
#ifdef DRIVER_FUNCTION_SET_QUOTA
KFilterDispatch(SetQuota)
#endif

#ifdef DRIVER_FUNCTION_STARTIO
VOID KWdmFilterDevice::StartIo(KIrp I)
{UNREFERENCED_PARAMETER(I);} // not called
#endif

#endif
