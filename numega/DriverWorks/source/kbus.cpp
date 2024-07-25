// kbus.cpp - functions for class KBus
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
//  KBus::ForwardToParent
//
//	Routine Description:
//		Routine to reroute IRP from one device stack to a parent
//		device stack
//
//	Parameters:
//		[in] I
//			irp to reroute
//
//		[in] bMustHandle
//			true if device stack must handle this IRP
//
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		This method can be used to forward some of the IRPs to
//		grandparent bus driver if the parent bus driver does not
//		handle them.
//
NTSTATUS KBus::ForwardToParent(KIrp I, BOOLEAN bMustHandle)
{
	NTSTATUS status = STATUS_PENDING;

	PDEVICE_OBJECT pDevice = IoGetAttachedDeviceReference(*m_Parent);
	KIrp NewIrp = KIrp::Allocate(pDevice->StackSize + 1);
	if ( !NewIrp.IsNull() )
	{
		NewIrp.NextStackLocation()->DeviceObject = pDevice;
		NewIrp.NextStackLocation()->Parameters.Others.Argument1 = I.m_Irp;

		NewIrp.SetNextStackLocation();

		RtlCopyMemory(NewIrp.NextStackLocation(), I.CurrentStackLocation(), FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine));
		NewIrp.Control(NEXT) = 0;

		NewIrp.SetCompletionRoutine(OnForwardComplete, (PVOID)bMustHandle);
		NewIrp.Status() = STATUS_NOT_SUPPORTED;
		I.MarkPending();
		IoCallDriver(pDevice, NewIrp);
	}
	else
	{
        ObDereferenceObject(pDevice);
		status = STATUS_INSUFFICIENT_RESOURCES;
		I.PnpComplete(status);
	}

	return status;
}

////////////////////////////////////////////////////////////////////////
//  KBus::OnForwardComplete
//
//	Routine Description:
//		Completion routine for ForwardToParent call
//
//	Parameters:
//		[in] pDevice
//			pointer to device object
//
//		[in] Irp
//			pointer to completed IRP
//
//		[in] Context
//			pointer to our context (bMustHandle)
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		This routine just releases the allocated resources and
//		completes the original IRP.
//
NTSTATUS KBus::OnForwardComplete(PDEVICE_OBJECT pDevice, PIRP Irp, PVOID Context)
{
	ObDereferenceObject(pDevice);

	KIrp NewIrp(Irp);
	KIrp I = (PIRP)NewIrp.CurrentStackLocation()->Parameters.Others.Argument1;

	if ( NewIrp.Status() == STATUS_NOT_SUPPORTED )
	{
		if ( Context )
			I.Status() = STATUS_UNSUCCESSFUL;
	}
	else
	{
		I->IoStatus = NewIrp->IoStatus;
	}

	KIrp::Deallocate(NewIrp);

	NTSTATUS status = I.Status();
	I.PnpComplete(status);

	return STATUS_MORE_PROCESSING_REQUIRED;
}

////////////////////////////////////////////////////////////////////////
//  KBus::QueryDeviceRelations
//
//	Routine Description:
//		IRP_MN_QUERY_DEVICE_RELATIONS handler
//
//	Parameters:
//		[in] I
//			IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		This method returns information about its children.
//
NTSTATUS KBus::QueryDeviceRelations(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( I.DeviceRelationType() == BusRelations )
	{
		ULONG prevcount = 0;
		ULONG count = 0;

		PDEVICE_RELATIONS oldRelations = (PDEVICE_RELATIONS)I.Information();

		m_Lock.Acquire();

		KPhysicalDevice* pDevice;

		if ( oldRelations )
		{
			prevcount = oldRelations->Count;
		}

		if ( !oldRelations || (m_NumberOfChildren != 0) )
		{
			pDevice = m_Children.HeadNoLock();
			while ( pDevice )
			{
				if ( pDevice->m_bExist )
					count++;

				pDevice = m_Children.NextNoLock(pDevice);
			}

			ULONG length = sizeof(DEVICE_RELATIONS) + ((count + prevcount)*sizeof(PDEVICE_OBJECT)) - 1;
			PDEVICE_RELATIONS relations = (PDEVICE_RELATIONS)ExAllocatePoolWithTag(PagedPool, length, DefaultPoolTag);
			if ( relations )
			{
				if ( prevcount )
				{
					RtlCopyMemory(relations->Objects, oldRelations->Objects, prevcount*sizeof(PDEVICE_OBJECT));
				}

				relations->Count = prevcount + count;

				pDevice = m_Children.HeadNoLock();
				while ( pDevice )
				{
					if ( pDevice->m_bExist )
					{
						relations->Objects[prevcount] = *pDevice;
						ObReferenceObject(*pDevice);
						prevcount++;
					}
					else
					{
						pDevice->m_bDeleteOnRemove = true;
					}

					pDevice = m_Children.NextNoLock(pDevice);
				}

				if ( oldRelations )
					delete oldRelations;

				I.Information() = (ULONG_PTR)relations;
				I.Status() = STATUS_SUCCESS;
			}
			else
			{
				m_Lock.Release();
				status = STATUS_INSUFFICIENT_RESOURCES;
				return status;
			}

		}

		m_Lock.Release();
	}

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return status;
}

////////////////////////////////////////////////////////////////////////
//  KBus::HighestChildrenPowerState
//
//	Routine Description:
//		Returns the highest power state of its children
//		devices
//
//	Parameters:
//		None
//
//	Return Value:
//		DEVICE_POWER_STATE - the highest device power state
//
//	Comments:
//		None
//
DEVICE_POWER_STATE KBus::HighestChildrenPowerState()
{
	DEVICE_POWER_STATE HighestChildrenPowerState = PowerDeviceD3;

	m_Lock.Acquire();

	KPhysicalDevice* pDevice = m_Children.HeadNoLock();
	while ( pDevice )
	{
		if ( pDevice->CurrentPowerState() < HighestChildrenPowerState )
			HighestChildrenPowerState = pDevice->CurrentPowerState();

		pDevice = m_Children.NextNoLock(pDevice);
	}

	m_Lock.Release();

	return HighestChildrenPowerState;
}

////////////////////////////////////////////////////////////////////////
//  KBus::PowerChild
//
//	Routine Description:
//		this routine makes sure that parent power state is in synch with
//		it's children
//
//	Parameters:
//		[in] I
//			power IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		This is a virtual method which can be overriden if different than default
//		behavior is required
//
NTSTATUS KBus::PowerChild(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	DEVICE_POWER_STATE PowerState = HighestChildrenPowerState();

	if ( (PowerState == PowerDeviceD0) && (m_PowerState != PowerDeviceD0) )
	{
		// request device power up irp
		POWER_STATE ps;
		ps.DeviceState = PowerDeviceD0;

		I.MarkPending();
		status = PoRequestPowerIrp(*m_Parent, IRP_MN_SET_POWER,	ps, OnPowerBusComplete, (PVOID)I.m_Irp, NULL);
        if (NT_SUCCESS(status))
        {
            return status;
        }
	}
	else if ( (PowerState != PowerDeviceD0) && (m_PowerState == PowerDeviceD0) )
	{
		// request device power down irp
		m_Parent->RequestDevicePowerIrp(IRP_MN_SET_POWER, PowerState);
	}


	I.IndicatePowerIrpProcessed();

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  KBus::OnPowerBusComplete
//
//	Routine Description:
//		Completion routine for powering up the parent FDO
//
//	Parameters:
//		[in] pDevice
//			pointer to parent FDO
//
//		[in] mn
//			IRP_MJ_POWER minor code
//
//		[in] ps
//			new power state
//
//		[in] Context
//			user context
//
//		[in] IoStatus
//			status of IRP
//
//	Return Value:
//		None
//
//	Comments:
//		We need to complete the original IRP
//
VOID KBus::OnPowerBusComplete(PDEVICE_OBJECT pDevice, UCHAR mn, POWER_STATE ps, PVOID Context, PIO_STATUS_BLOCK IoStatus)
{
	KIrp I = (PIRP)Context;

	NTSTATUS status = IoStatus->Status;

	I.IndicatePowerIrpProcessed();

	I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  KBus::RequestWaitWake
//
//	Routine Description:
//		Request to arm WaitWake feature
//
//	Parameters:
//		[in] bEnable
//			if true, then enable
//			if false, then disable
//
//	Return Value:
//		None
//
//	Comments:
//		This routine creates or cancels an IRP_MN_WAIT_WAKE irp for
//		parent FDO stack.
//
void KBus::RequestWaitWake(bool bEnable)
{
	if ( bEnable )
	{
		KeEnterCriticalRegion();
		m_WaitWakeLock.Wait();

		if ( ++m_WaitWakeCount == 1 )
		{
			m_Parent->RequestDevicePowerIrp(IRP_MN_WAIT_WAKE, PowerDeviceMaximum, (PREQUEST_POWER_COMPLETE)OnWaitWakeComplete, this);
		}

		m_WaitWakeLock.Release();
		KeLeaveCriticalRegion();
	}
	else
	{
		PWAIT_WAKE_CONTEXT pContext = new WAIT_WAKE_CONTEXT;
		if ( pContext )
		{
			pContext->m_Item = IoAllocateWorkItem(*m_Parent);
			if ( pContext->m_Item )
			{
				IoQueueWorkItem(pContext->m_Item, PassiveCancelWaitWake, DelayedWorkQueue, pContext);
				return;
			}
			delete pContext;
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////////
//  KBus::PassiveCancelWaitWake
//
//	Routine Description:
//		IRP_MN_WAIT_WAKE cancel handler
//
//	Parameters:
//		[in] pDevice
//			pointer to parent FDO
//
//		[in] Context
//			WAIT_WAKE_CONTEXT
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
VOID KBus::PassiveCancelWaitWake(PDEVICE_OBJECT DeviceObject, PVOID Context)
{
	PWAIT_WAKE_CONTEXT pContext = (PWAIT_WAKE_CONTEXT)Context;
	KBus* pBus = pContext->m_Bus;

	IoFreeWorkItem(pContext->m_Item);
	delete pContext;

	KeEnterCriticalRegion();
	pBus->m_WaitWakeLock.Wait();

	if ( --pBus->m_WaitWakeCount == 0 )
	{
		pBus->m_Parent->CancelWaitWakeIrp();
	}

	pBus->m_WaitWakeLock.Release();
	KeLeaveCriticalRegion();
}

////////////////////////////////////////////////////////////////////////
//  KBus::OnWaitWakeComplete
//
//	Routine Description:
//		IRP_MN_WAIT_WAKE completion handler
//
//	Parameters:
//		[in] pDevice
//			pointer to parent FDO
//
//		[in] mn
//			IRP_MN_WAIT_WAKE
//
//		[in] ps
//			new power state
//
//		[in] Context
//			PowerCompletionContext
//
//		[in] IoStatus
//			status of IRP
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
VOID KBus::OnWaitWakeComplete(PDEVICE_OBJECT DeviceObject, UCHAR mn, POWER_STATE ps, PowerCompletionContext* Context, PIO_STATUS_BLOCK IoStatus)
{
	KBus* pBus = (KBus*)Context->m_UserContext;

	PWAIT_WAKE_CONTEXT pContext = new WAIT_WAKE_CONTEXT;
	if ( pContext )
	{
		pContext->m_Item = IoAllocateWorkItem(*pBus->m_Parent);
		if ( pContext->m_Item )
		{
			IoQueueWorkItem(pContext->m_Item, PassiveWaitWakeComplete, DelayedWorkQueue, pContext);
			return;
		}

		delete pContext;
	}
}

////////////////////////////////////////////////////////////////////////
//  KBus::PassiveWaitWakeComplete
//
//	Routine Description:
//		IRP_MN_WAIT_WAKE completion handler
//
//	Parameters:
//		[in] pDevice
//			pointer to parent FDO
//
//		[in] Context
//			WAIT_WAKE_CONTEXT
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
VOID KBus::PassiveWaitWakeComplete(PDEVICE_OBJECT DeviceObject, PVOID Context)
{
	PWAIT_WAKE_CONTEXT pContext = (PWAIT_WAKE_CONTEXT)Context;
	KBus* pBus = pContext->m_Bus;

	IoFreeWorkItem(pContext->m_Item);
	delete pContext;

	pBus->FindAwakenDevice();

	KeEnterCriticalRegion();
	pBus->m_WaitWakeLock.Wait();

	if ( --pBus->m_WaitWakeCount != 0 )
	{
		pBus->m_Parent->RequestDevicePowerIrp(IRP_MN_WAIT_WAKE, PowerDeviceMaximum, (PREQUEST_POWER_COMPLETE)OnWaitWakeComplete, pBus);
	}

	pBus->m_WaitWakeLock.Release();
	KeLeaveCriticalRegion();
}
////////////////////////////////////////////////////////////////////////
//  KBus::FindAwakenDevice
//
//	Routine Description:
//		Finds the device which is just got awaken
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		This is not very useful implementation. It just finds the first
//		non-NULL irp and completes it.
//
void KBus::FindAwakenDevice()
{
	bool bFound = false;
	KIrp I;

	m_Lock.Acquire();
	KPhysicalDevice* pDevice = m_Children.HeadNoLock();
	while ( pDevice )
	{
		pDevice->m_WaitWakeLock.Lock();
		if ( !pDevice->m_WaitWakeIrp.IsNull() && IoSetCancelRoutine(pDevice->m_WaitWakeIrp, NULL) )
		{
			bFound = true;
			I = pDevice->m_WaitWakeIrp;
			pDevice->m_WaitWakeIrp = NULL;
		}
		pDevice->m_WaitWakeLock.Unlock();

		if ( !bFound )
			pDevice = m_Children.NextNoLock(pDevice);
		else
			break;
	}

	m_Lock.Release();

	if ( bFound )
		I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  KBus::RemoveChildren
//
//	Routine Description:
//		removes all the children from a collection
//
//	Parameters:
//		None
//
//	Return Value:
//		true if found
//
//	Comments:
//		This routine should be called from IRP_MN_REMOVE_DEVICE parent FDO
//		handler. It removes all the children devices from a collection and
//		destroys them.
//
void KBus::RemoveChildren()
{
	while ( true )
	{
		m_Lock.Acquire();
		KPhysicalDevice* pDevice = m_Children.RemoveHeadNoLock();
		m_Lock.Release();

		if ( pDevice )
			pDevice->DestroySelf();
		else
			break;
	}
}

////////////////////////////////////////////////////////////////////////
//  KBus::RemoveParent
//
//	Routine Description:
//		removes a parent FDO from all children objects
//
//	Parameters:
//		None
//
//	Return Value:
//		true if found
//
//	Comments:
//		This routine should be called from IRP_MN_SURPRISE_REMOVAL parent FDO
//		handler. It removes all the children devices from a collection and marks
//		them as missing, thus the children PDO objects will be destroyed in
//		IRP_MN_REMOVE_DEVICE
//
void KBus::RemoveParent()
{
	m_Lock.Acquire();

	KPhysicalDevice* pDevice = m_Children.RemoveHeadNoLock();
	while ( pDevice )
	{
		pDevice->m_ParentBus = NULL;
		pDevice->m_bDeleteOnRemove = true;
		pDevice = m_Children.RemoveHeadNoLock();
	}

	m_Lock.Release();
}


////////////////////////////////////////////////////////////////////////
//  KBus::AddChild
//
//	Routine Description:
//		Add a new child to a collection
//
//	Parameters:
//		[in] pDevice
//			pointer to a new child object
//
//	Return Value:
//		true if found
//
//	Comments:
//		None
//
void KBus::AddChild(KPhysicalDevice* pDevice)
{
	m_Lock.Acquire();
	m_Children.InsertTailNoLock(pDevice);
	m_NumberOfChildren++;
	m_Lock.Release();
}

////////////////////////////////////////////////////////////////////////
//  KBus::DeleteChild
//
//	Routine Description:
//		Deletes child from a collections
//
//	Parameters:
//		[in] id
//			id to check
//
//	Return Value:
//		true if found
//
//	Comments:
//		None
//
void KBus::DeleteChild(KPhysicalDevice* pDevice)
{
	m_Lock.Acquire();
	m_Children.RemoveNoLock(pDevice);
	m_NumberOfChildren--;
	m_Lock.Release();
}

#endif // _WDM_
