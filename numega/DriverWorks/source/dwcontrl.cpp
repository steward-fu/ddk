// dwcontrl.cpp - DriverWorks controller for WDM
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
#if !defined(_DW_INCLUDE_NTIFS_) && !defined(_DW_INCLUDE_NTDDK_)

struct DwControllerEntry
{
	KDEVICE_QUEUE_ENTRY m_kqe;	// must be first
	PDRIVER_CONTROL	m_function;
	PDEVICE_OBJECT m_device;
	PVOID m_context;
};

PCONTROLLER_OBJECT DwCreateController(ULONG extsize)
{
	PCONTROLLER_OBJECT p;

	p = new (NonPagedPool) CONTROLLER_OBJECT;
	if (p)
	{
		KeInitializeDeviceQueue(&p->m_queue);
		KeInitializeSpinLock(&p->m_lock);
	}
	return p;
	UNREFERENCED_PARAMETER(extsize);
}

VOID DwDeleteController(PCONTROLLER_OBJECT pController)
{
	delete pController;
}

VOID DwAllocateController(
	PCONTROLLER_OBJECT  ControllerObject,
	PDEVICE_OBJECT  DeviceObject,
	PDRIVER_CONTROL  ExecutionRoutine,
	PVOID  Context
	)
{
	ASSERT (KeGetCurrentIrql() == DISPATCH_LEVEL);
	DwControllerEntry* pEntry = new (NonPagedPool) DwControllerEntry;

	KeAcquireSpinLockAtDpcLevel(&ControllerObject->m_lock);

	if (pEntry)
	{
		pEntry->m_function = ExecutionRoutine;
		pEntry->m_device = DeviceObject;
		pEntry->m_context = Context;

		if (!KeInsertDeviceQueue(&ControllerObject->m_queue, &pEntry->m_kqe))
		{
			IO_ALLOCATION_ACTION action;
			PIRP pIrp;

			delete pEntry;

			if (DeviceObject)
				pIrp = DeviceObject->CurrentIrp;
			else
				pIrp = NULL;

			action = ExecutionRoutine(DeviceObject, pIrp, NULL, Context);

			switch (action)
			{
			case KeepObject:
				break;

			case DeallocateObject:
				KeReleaseSpinLockFromDpcLevel(&ControllerObject->m_lock);
				DwFreeController(ControllerObject);
				KeAcquireSpinLockAtDpcLevel(&ControllerObject->m_lock);
				break;

			default:
				ASSERT(FALSE);
			}
		}
	}
	KeReleaseSpinLockFromDpcLevel(&ControllerObject->m_lock);
}

VOID DwFreeController(PCONTROLLER_OBJECT pController)
{
	ASSERT (KeGetCurrentIrql() == DISPATCH_LEVEL);

	KeAcquireSpinLockAtDpcLevel(&pController->m_lock);

	DwControllerEntry* pEntry;
	PKDEVICE_QUEUE pQ = &pController->m_queue;

	while (pQ->Busy)
	{
		pEntry = (DwControllerEntry*)KeRemoveDeviceQueue(pQ);

		if (pEntry)
		{
			PDEVICE_OBJECT  DeviceObject;
			PDRIVER_CONTROL  ExecutionRoutine;
			PVOID  Context;
			IO_ALLOCATION_ACTION action;
			PIRP pIrp;

			DeviceObject = pEntry->m_device;
			ExecutionRoutine = pEntry->m_function;
			Context = pEntry->m_context;

			delete pEntry;

			if (DeviceObject)
				pIrp = DeviceObject->CurrentIrp;
			else
				pIrp = NULL;

			action = ExecutionRoutine(DeviceObject, pIrp, NULL, Context);

			if (action == KeepObject)
				break;
			else if (action == DeallocateObject)
				continue;
			else
			{
				ASSERT(FALSE);
				break;
			}

		}	// if (pEntry)

	} // while (pQ->Busy)

	KeReleaseSpinLockFromDpcLevel(&pController->m_lock);
}

#endif
#endif
