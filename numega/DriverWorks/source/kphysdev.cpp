// kphysdev.cpp - functions for class KPhysicalDevice
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

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::KPhysicalDevice
//
//	Routine Description:
//		this is a contstructor for KPhysicalDevice class
//
//	Parameters:
//		[in] pParentBus
//			pointer to a parent bus class
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
KPhysicalDevice::KPhysicalDevice(KBus* pParentBus) :
	KPnpDevice(NULL),
	m_ParentBus(pParentBus),
	m_bExist(true),
	m_bDeleteOnRemove(false),
	m_WaitWakeIrp(NULL)
{
}


////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::~KPhysicalDevice
//
//	Routine Description:
//		This is the destructor for KPhysicalDevice.
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
KPhysicalDevice::~KPhysicalDevice()
{
	ASSERT(m_bDeleteOnRemove);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::DestroySelf
//
//	Routine Description:
//		This routine calls the destructor and deletes the object
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		If the device object received IRP_MN_SURPRISE_REMOVAL,
//		then we will delete it in IRP_MN_REMOVE_DEVICE
//
void KPhysicalDevice::DestroySelf()
{
	m_bDeleteOnRemove = true;

	if ( m_State.m_SurpriseRemoval )
	{
		m_ParentBus = NULL;
	}
	else
	{
		_self_delete();
	}
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::SetPnpPolicy
//
//	Routine Description:
//		Sets default pnp policy
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		The major difference between pnp policies of PDO and FDO
//		is that in the case of PDO we need to complete all the irps
//		and in the case of FDO we need to forward them.
//
VOID KPhysicalDevice::SetPnpPolicy()
{
	m_Policies.m_GeneralPolicy.m_FailRequestsIfRemoved = TRUE;
	m_Policies.m_GeneralPolicy.m_FailRequestsIfNotStarted = FALSE;
	m_Policies.m_GeneralPolicy.m_DeleteOnRemoveIfNotOpen = FALSE;
	m_Policies.m_GeneralPolicy.m_WaitForSafeRemove = TRUE;
	m_Policies.m_GeneralPolicy.m_DetachPdoOnRemove = FALSE;
	m_Policies.m_GeneralPolicy.m_DeletePdoOnRemove = FALSE;
	m_Policies.m_GeneralPolicy.m_EnableDisableInterfaces = FALSE;

	m_Policies.m_QueryStopPolicy.m_FailIfOpen = FALSE;
	m_Policies.m_QueryStopPolicy.m_FailIfOutstandingIo = FALSE;
	m_Policies.m_QueryStopPolicy.m_CancelDeviceQueue = FALSE;
	m_Policies.m_QueryStopPolicy.m_CancelCurrentIrp = FALSE;
	m_Policies.m_QueryStopPolicy.m_WaitForOutstandingIo = FALSE;
	m_Policies.m_QueryStopPolicy.m_UseTimeoutOnWait = FALSE;
	m_Policies.m_QueryStopPolicy.m_CancelHoldQueueIfTimeout = FALSE;

	m_Policies.m_QueryRemovePolicy.m_FailIfOpen = FALSE;
	m_Policies.m_QueryRemovePolicy.m_FailIfOutstandingIo = FALSE;
	m_Policies.m_QueryRemovePolicy.m_CancelDeviceQueue = FALSE;
	m_Policies.m_QueryRemovePolicy.m_CancelCurrentIrp = FALSE;
	m_Policies.m_QueryRemovePolicy.m_WaitForOutstandingIo = FALSE;
	m_Policies.m_QueryRemovePolicy.m_UseTimeoutOnWait = FALSE;
	m_Policies.m_QueryRemovePolicy.m_CancelHoldQueueIfTimeout = FALSE;

	m_Policies.m_HoldPolicy.m_HoldIfRemovePending = TRUE;
	m_Policies.m_HoldPolicy.m_HoldIfStopPending = TRUE;
	m_Policies.m_HoldPolicy.m_HoldIfStopped = TRUE;
	m_Policies.m_HoldPolicy.m_CancelAllOnCleanUp = TRUE;
	m_Policies.m_HoldPolicy.m_CancelAllOnRemove = TRUE;
	m_Policies.m_HoldPolicy.m_CancelAllOnStop = FALSE;
	m_Policies.m_HoldPolicy.m_ProcessOnCancelStop = TRUE;
	m_Policies.m_HoldPolicy.m_ProcessOnCancelRemove = TRUE;

	m_Policies.m_ForwardToPdoPolicy.m_CallBeforeStart = FALSE;
	m_Policies.m_ForwardToPdoPolicy.m_CallBeforeCancelStop = FALSE;
	m_Policies.m_ForwardToPdoPolicy.m_CallBeforeCancelRemove = FALSE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryStop = FALSE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterStop = FALSE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryRemove = FALSE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterRemove = FALSE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryPnpState = FALSE;
	m_Policies.m_ForwardToPdoPolicy.m_CallAfterSurpriseRemoval = FALSE;

	m_Policies.m_CompletedByDriverWorks.m_StartDevice = FALSE;
	m_Policies.m_CompletedByDriverWorks.m_StopDevice = FALSE;
	m_Policies.m_CompletedByDriverWorks.m_QueryRemove = FALSE;
	m_Policies.m_CompletedByDriverWorks.m_QueryStop = FALSE;
	m_Policies.m_CompletedByDriverWorks.m_CancelRemove = FALSE;
	m_Policies.m_CompletedByDriverWorks.m_CancelStop = FALSE;
	m_Policies.m_CompletedByDriverWorks.m_Remove = FALSE;
	m_Policies.m_CompletedByDriverWorks.m_QueryPnpState = FALSE;
	m_Policies.m_CompletedByDriverWorks.m_SurpriseRemoval = FALSE;
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::Pnp
//
//	Routine Description:
//		IRP_MJ_PNP handler
//
//	Parameters:
//		[in] I
//			IRP_MJ_PNP
//
//	Return Value:
//		status
//
//	Comments:
//		We need to filter IRP_MN_REMOVE_DEVICE Irp so
//		KPnpDevice::Pnp does not delete it
//
NTSTATUS KPhysicalDevice::Pnp(KIrp I)
{
	NTSTATUS status;
	UCHAR Minor = I.MinorFunction();

	if ( (Minor == IRP_MN_REMOVE_DEVICE) && !m_bDeleteOnRemove )
	{
		status = OnRemoveDevice(I);

		m_State.m_Started = FALSE;
		m_State.m_RemovePending = FALSE;
		m_State.m_StopPending = FALSE;
		m_State.m_RemoveInProgress = FALSE;
	}
	else
		status = KPnpDevice::Pnp(I);

	return status;
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::DefaultPnp
//
//	Routine Description:
//		Default IRP_MJ_PNP handler
//
//	Parameters:
//		[in] I
//			IRP_MJ_PNP
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP without changing its status
//
NTSTATUS KPhysicalDevice::DefaultPnp(KIrp I)
{
	NTSTATUS status = I.Status();

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnSurpriseRemoval
//
//	Routine Description:
//		IRP_MN_SURPRISE_REMOVAL handler
//
//	Parameters:
//		[in] I
//			IRP_MN_SURPRISE_REMOVAL
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP with STATUS_SUCCESS.
//
NTSTATUS KPhysicalDevice::OnSurpriseRemoval(KIrp I)
{
	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnRemoveDevice
//
//	Routine Description:
//		IRP_MN_REMOVE_DEVICE handler
//
//	Parameters:
//		[in] I
//			IRP_MN_REMOVE_DEVICE
//
//	Return Value:
//		status
//
//	Comments:
//		There are two different situations when we can get
//		IRP_MN_REMOVE_DEVICE request. The first one is to remove
//		FDO and the second one is to remove PDO
//
NTSTATUS KPhysicalDevice::OnRemoveDevice(KIrp I)
{
	if ( m_bDeleteOnRemove )
	{
		if ( m_ParentBus )
			m_ParentBus->DeleteChild(this);

		m_Policies.m_GeneralPolicy.m_DeleteOnRemoveIfNotOpen = TRUE;
		m_Policies.m_GeneralPolicy.m_DeletePdoOnRemove = TRUE;
	}
	else  if ( m_bExist )
	{
		// FDO is removed
	}

	I.Status() = STATUS_SUCCESS;

	return DefaultPnp(I);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnCancelRemoveDevice
//
//	Routine Description:
//		IRP_MN_CANCEL_REMOVE_DEVICE handler
//
//	Parameters:
//		[in] I
//			IRP_MN_CANCEL_REMOVE_DEVICE
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP with STATUS_SUCCESS.
//
NTSTATUS KPhysicalDevice::OnCancelRemoveDevice(KIrp I)
{
	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryRemoveDevice
//
//	Routine Description:
//		IRP_MN_QUERY_REMOVE_DEVICE handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_REMOVE_DEVICE
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP with STATUS_SUCCESS.
//
NTSTATUS KPhysicalDevice::OnQueryRemoveDevice(KIrp I)
{
	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnStartDevice
//
//	Routine Description:
//		IRP_MN_START_DEVICE handler
//
//	Parameters:
//		[in] I
//			IRP_MN_START_DEVICE
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP with STATUS_SUCCESS.
//
NTSTATUS KPhysicalDevice::OnStartDevice(KIrp I)
{
	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnStopDevice
//
//	Routine Description:
//		IRP_MN_STOP_DEVICE handler
//
//	Parameters:
//		[in] I
//			IRP_MN_STOP_DEVICE
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP with STATUS_SUCCESS.
//
NTSTATUS KPhysicalDevice::OnStopDevice(KIrp I)
{
	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnCancelStopDevice
//
//	Routine Description:
//		IRP_MN_CANCEL_STOP_DEVICE handler
//
//	Parameters:
//		[in] I
//			IRP_MN_CANCEL_STOP_DEVICE
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP with STATUS_SUCCESS.
//
NTSTATUS KPhysicalDevice::OnCancelStopDevice(KIrp I)
{
	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryStopDevice
//
//	Routine Description:
//		IRP_MN_QUERY_STOP_DEVICE handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_STOP_DEVICE
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP with STATUS_SUCCESS.
//
NTSTATUS KPhysicalDevice::OnQueryStopDevice(KIrp I)
{
	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryCapabilities
//
//	Routine Description:
//		IRP_MN_QUERY_DEVICE_CAPABILITIES handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_DEVICE_CAPABILITIES
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is use the same device capabilities as the parent
//		device.
//
NTSTATUS KPhysicalDevice::OnQueryCapabilities(KIrp I)
{
	return m_ParentBus->ForwardToParent(I, FALSE);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryDeviceText
//
//	Routine Description:
//		IRP_MN_QUERY_DEVICE_TEXT handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_DEVICE_TEXT
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP without changing the status.
//
NTSTATUS KPhysicalDevice::OnQueryDeviceText(KIrp I)
{
	NTSTATUS status = I.Status();

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryResources
//
//	Routine Description:
//		IRP_MN_QUERY_RESOURCES handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_RESOURCES
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP without changing the status.
//
NTSTATUS KPhysicalDevice::OnQueryResources(KIrp I)
{
	NTSTATUS status = I.Status();

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryResourceRequirements
//
//	Routine Description:
//		IRP_MN_QUERY_RESOURCE_REQUIREMENTS handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_RESOURCE_REQUIREMENTS
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP without changing the status.
//
NTSTATUS KPhysicalDevice::OnQueryResourceRequirements(KIrp I)
{
	NTSTATUS status = I.Status();

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnFilterResourceRequirements
//
//	Routine Description:
//		IRP_MN_FILTER_RESOURCE_REQUIREMENTS handler
//
//	Parameters:
//		[in] I
//			IRP_MN_FILTER_RESOURCE_REQUIREMENTS
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP without changing the status.
//
NTSTATUS KPhysicalDevice::OnFilterResourceRequirements(KIrp I)
{
	NTSTATUS status = I.Status();

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryDeviceState
//
//	Routine Description:
//		IRP_MN_QUERY_PNP_DEVICE_STATE handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_PNP_DEVICE_STATE
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to use the same PNP_DEVICE_STATE as the parent
//		device.
//
NTSTATUS KPhysicalDevice::OnQueryDeviceState(KIrp I)
{
	return m_ParentBus->ForwardToParent(I, FALSE);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryDeviceRelations
//
//	Routine Description:
//		IRP_MN_QUERY_DEVICE_RELATIONS handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_DEVICE_RELATIONS
//
//	Return Value:
//		status
//
//	Comments:
//		We need to AddRef the PDO and return a pointer to it.
//
NTSTATUS KPhysicalDevice::OnQueryDeviceRelations(KIrp I)
{
   NTSTATUS status;

    switch ( I.DeviceRelationType() )
	{
    case TargetDeviceRelation:
		{
			PDEVICE_RELATIONS rel = (PDEVICE_RELATIONS)ExAllocatePoolWithTag(PagedPool, sizeof(DEVICE_RELATIONS), DefaultPoolTag);
			if ( rel )
			{
				rel->Count = 1;
				rel->Objects[0] = *this;
				ObReferenceObject(*this);

				I.Information() = (ULONG_PTR)rel;
				status = STATUS_SUCCESS;
			}
			else
			{
				status = STATUS_INSUFFICIENT_RESOURCES;
			}
		}
        break;

    case BusRelations:
    case EjectionRelations:
    case RemovalRelations:
    default:
        status = I.Status();
    }

	I.PnpComplete(status);

    return status;
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryBusInformation
//
//	Routine Description:
//		IRP_MN_QUERY_BUS_INFORMATION handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_BUS_INFORMATION
//
//	Return Value:
//		status
//
//	Comments:
//		Use the same bus information as the parent FDO bus.
//
NTSTATUS KPhysicalDevice::OnQueryBusInformation(KIrp I)
{
	return m_ParentBus->ForwardToParent(I, TRUE);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnDeviceUsageNotification
//
//	Routine Description:
//		IRP_MN_DEVICE_USAGE_NOTIFICATION handler
//
//	Parameters:
//		[in] I
//			IRP_MN_DEVICE_USAGE_NOTIFICATION
//
//	Return Value:
//		status
//
//	Comments:
//		Use the same device usage as the parent FDO.
//
NTSTATUS KPhysicalDevice::OnDeviceUsageNotification(KIrp I)
{
	return m_ParentBus->ForwardToParent(I, TRUE);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryInterface
//
//	Routine Description:
//		IRP_MN_QUERY_INTERFACE handler
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_INTERFACE
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP without changing the status.
//
NTSTATUS KPhysicalDevice::OnQueryInterface(KIrp I)
{
	NTSTATUS status = I.Status();

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnReadConfig
//
//	Routine Description:
//		IRP_MN_READ_CONFIG handler
//
//	Parameters:
//		[in] I
//			IRP_MN_READ_CONFIG
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
NTSTATUS KPhysicalDevice::OnReadConfig(KIrp I)
{
	return m_ParentBus->ForwardToParent(I, TRUE);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnWriteConfig
//
//	Routine Description:
//		IRP_MN_WRITE_CONFIG handler
//
//	Parameters:
//		[in] I
//			IRP_MN_WRITE_CONFIG
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
NTSTATUS KPhysicalDevice::OnWriteConfig(KIrp I)
{
	return m_ParentBus->ForwardToParent(I, TRUE);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnEject
//
//	Routine Description:
//		IRP_MN_EJECT handler
//
//	Parameters:
//		[in] I
//			IRP_MN_EJECT
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
NTSTATUS KPhysicalDevice::OnEject(KIrp I)
{
	return m_ParentBus->ForwardToParent(I, TRUE);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnSetLock
//
//	Routine Description:
//		IRP_MN_SET_LOCK handler
//
//	Parameters:
//		[in] I
//			IRP_MN_SET_LOCK
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
NTSTATUS KPhysicalDevice::OnSetLock(KIrp I)
{
	return m_ParentBus->ForwardToParent(I, TRUE);
}

////////////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::SystemControl
//
//	Routine Description:
//		Default handler for IRP_MJ_SYSTEM_CONTROL
//
//	Parameters:
//		[in] I
//			IRP_MJ_SYSTEM_CONTROL
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP without changing the status.
//
NTSTATUS KPhysicalDevice::SystemControl(KIrp I)
{
	NTSTATUS status = I.Status();

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::SetPowerPolicy
//
//	Routine Description:
//		Sets default power policy
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		PDO is not a power policy owner
//
void KPhysicalDevice::SetPowerPolicy(BOOLEAN supportWaitWake)
{
	m_PowerPolicies.m_GeneralPolicy.m_PowerPolicyOwner = FALSE;
	m_PowerPolicies.m_GeneralPolicy.m_UsePowerSequence = FALSE;
	m_PowerPolicies.m_GeneralPolicy.m_WaitWakeEnabled = FALSE;
	m_PowerPolicies.m_GeneralPolicy.m_PowerUpForIO = FALSE;
	m_PowerPolicies.m_GeneralPolicy.m_PowerUpOnS0 = FALSE;
    m_PowerPolicies.m_GeneralPolicy.m_PowerDnOnClose = FALSE;
    m_PowerPolicies.m_GeneralPolicy.m_PowerUpOnCreate = FALSE;
    m_PowerPolicies.m_GeneralPolicy.m_GetDeviceCapabilities = FALSE;
	m_PowerPolicies.m_GeneralPolicy.m_PowerDownLevel = PowerDeviceD3;

	m_PowerPolicies.m_QueryPowerPolicy.m_FailIfOpen = FALSE;
	m_PowerPolicies.m_QueryPowerPolicy.m_FailIfOutstandingIo = FALSE;
	m_PowerPolicies.m_QueryPowerPolicy.m_CancelDeviceQueue = FALSE;
	m_PowerPolicies.m_QueryPowerPolicy.m_CancelCurrentIrp = FALSE;
	m_PowerPolicies.m_QueryPowerPolicy.m_WaitForOutstandingIo = FALSE;
	m_PowerPolicies.m_QueryPowerPolicy.m_UseTimeoutOnWait = FALSE;
	m_PowerPolicies.m_QueryPowerPolicy.m_CancelHoldQueueIfTimeout = FALSE;

	m_PowerPolicies.m_HoldPolicy.m_HoldIfPowerOffPending = FALSE;
	m_PowerPolicies.m_HoldPolicy.m_HoldIfSleepPending = FALSE;
	m_PowerPolicies.m_HoldPolicy.m_HoldIfSleeping = FALSE;
	m_PowerPolicies.m_HoldPolicy.m_HoldIfOff = FALSE;
	m_PowerPolicies.m_HoldPolicy.m_CancelAllOnPowerOff = FALSE;
	m_PowerPolicies.m_HoldPolicy.m_ProcessOnPowerOn = FALSE;
	m_PowerPolicies.m_HoldPolicy.m_WaitForOutstandingIO = FALSE;

	m_PowerPolicies.m_PowerCallByDriverWorks.m_SetPower = FALSE;
	m_PowerPolicies.m_PowerCallByDriverWorks.m_QueryPower = FALSE;
	m_PowerPolicies.m_PowerCallByDriverWorks.m_WaitWake = FALSE;
	m_PowerPolicies.m_PowerCallByDriverWorks.m_PowerSequence = FALSE;

	m_PowerPolicies.m_DeviceSleepPolicy.m_FailIfOpen = FALSE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_FailIfOutstandingIo = FALSE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_CancelDeviceQueue = FALSE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_CancelCurrentIrp = FALSE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_WaitForOutstandingIo = FALSE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_UseTimeoutOnWait = FALSE;
	m_PowerPolicies.m_DeviceSleepPolicy.m_CancelHoldQueueIfTimeout = FALSE;

	m_PowerPolicies.m_WaitWakePolicy.m_SendDeviceSleep = FALSE;
	m_PowerPolicies.m_WaitWakePolicy.m_SendSystemSleep = FALSE;
	m_PowerPolicies.m_WaitWakePolicy.m_SendStartDevice = FALSE;
	m_PowerPolicies.m_WaitWakePolicy.m_CancelStopDevice = FALSE;
	m_PowerPolicies.m_WaitWakePolicy.m_CancelRemoveDevice = FALSE;
	m_PowerPolicies.m_WaitWakePolicy.m_CancelDeviceWake = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::DefaultPower
//
//	Routine Description:
//		Default handler for IRP_MJ_POWER
//
//	Parameters:
//		[in] I
//			IRP_MJ_POWER
//
//	Return Value:
//		status
//
//	Comments:
//		Default action is to complete the IRP without changing the status.
//
NTSTATUS KPhysicalDevice::DefaultPower(KIrp I)
{
	NTSTATUS status = I.Status();

	I.IndicatePowerIrpProcessed();
	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnSetPower
//
//	Routine Description:
//		Default handler for IRP_MN_SET_POWER
//
//	Parameters:
//		[in] I
//			IRP_MN_SET_POWER
//
//	Return Value:
//		status
//
//	Comments:
//		Informs the parent about child power state change.
//
NTSTATUS KPhysicalDevice::OnSetPower(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( I.PowerStateType() == DevicePowerState )
	{
		m_CurrentDevicePowerState = I.PowerStateSetting().DeviceState;

		status = m_ParentBus->PowerChild(I);
		return status;
	}

	I.IndicatePowerIrpProcessed();

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnQueryPower
//
//	Routine Description:
//		Default handler for IRP_MN_QUERY_POWER
//
//	Parameters:
//		[in] I
//			IRP_MN_QUERY_POWER
//
//	Return Value:
//		status
//
//	Comments:
//		Completes the irp with STATUS_SUCCESS.
//
NTSTATUS KPhysicalDevice::OnQueryPower(KIrp I)
{
	I.IndicatePowerIrpProcessed();
	return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::OnWaitWake
//
//	Routine Description:
//		Default handler for IRP_MN_WAIT_WAKE
//
//	Parameters:
//		[in] I
//			IRP_MN_WAIT_WAKE
//
//	Return Value:
//		status
//
//	Comments:
//		Saves the irp, marks it pending and ask the parent object to arm WaitWake
//		feature for a child device.
//
NTSTATUS KPhysicalDevice::OnWaitWake(KIrp I)
{
	NTSTATUS status = STATUS_PENDING;

	I.MarkPending();

	m_WaitWakeLock.Lock();

	m_WaitWakeIrp = I;
	IoSetCancelRoutine(I, WaitWakeIrpCancel);
    if (I->Cancel && (IoSetCancelRoutine(I, NULL) != NULL))
    {
        m_WaitWakeIrp = NULL;
	    m_WaitWakeLock.Unlock();

        status = STATUS_CANCELLED;

	    I.IndicatePowerIrpProcessed();
	    return I.PnpComplete(status);
    }
    else
    {
	    m_WaitWakeLock.Unlock();
    }

	m_ParentBus->RequestWaitWake(true);

	return status;
}

////////////////////////////////////////////////////////////////////////
//  KPhysicalDevice::WaitWakeIrpCancel
//
//	Routine Description:
//		cancel routine for WaitWakeIrp
//
//	Parameters:
//		[in] DeviceObject
//			device object pointer
//
//		[in] I
//			IRP_MN_WAIT_WAKE
//
//	Return Value:
//		None
//
//	Comments:
//		Ask the parent object to disarm WaitWake feature and complete the irp.
//
VOID KPhysicalDevice::WaitWakeIrpCancel(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	KPhysicalDevice* pDevice = (KPhysicalDevice*)KDevicePTR(DeviceObject);

    IoReleaseCancelSpinLock(Irp->CancelIrql);

	pDevice->m_WaitWakeLock.Lock();
	pDevice->m_WaitWakeIrp = NULL;
	pDevice->m_WaitWakeLock.Unlock();

	pDevice->m_ParentBus->RequestWaitWake(false);

	KIrp I(Irp);
	I.PnpComplete(pDevice, STATUS_CANCELLED);
}

#endif // _WDM_
