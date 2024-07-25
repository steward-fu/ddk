// kpnplow.cpp - functions for class KUsbObject and related classes
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

///////////////////////////////////////////////////////////////////////////
// Constructors for KPnpLowerDevice

#ifndef KNDIS_WDM_LIBBLD
KPnpLowerDevice::KPnpLowerDevice(
	KDevice* AttachingDevice,
	PDEVICE_OBJECT PhysicalDeviceObject,
	NTSTATUS* pStatus
	) :
	KLowerDevice(
		IoAttachDeviceToDeviceStack(
			*AttachingDevice,
			PhysicalDeviceObject
			)
		)
{
	if (m_pDeviceObject != NULL)
	{
		m_PhysicalDeviceObject = PhysicalDeviceObject;
	}
	else
	{
		m_PhysicalDeviceObject = NULL;
		m_ConstructorStatus = STATUS_NO_SUCH_DEVICE;
	}

	*pStatus = ConstructorStatus();

	BOUNDS_CHECKER(CONSTRUCTED_PNP_LOWER_DEVICE, (this, *pStatus));
}

NTSTATUS KPnpLowerDevice::Initialize(
	KDevice* AttachingDevice,
	PDEVICE_OBJECT PhysicalDeviceObject
	)
{
	// The device should not already be initialized
	ASSERT(m_pDeviceObject == NULL);

	m_pDeviceObject =
		IoAttachDeviceToDeviceStack(
			*AttachingDevice,
			PhysicalDeviceObject
			);
	m_pFileObject = NULL;
	if (m_pDeviceObject != NULL)
	{
		m_PhysicalDeviceObject = PhysicalDeviceObject;
	}
	else
	{
		m_PhysicalDeviceObject = NULL;
		m_ConstructorStatus = STATUS_NO_SUCH_DEVICE;
	}

	BOUNDS_CHECKER(CONSTRUCTED_PNP_LOWER_DEVICE, (this, m_ConstructorStatus));

	return m_ConstructorStatus;
}
#endif // KNDIS_WDM_LIBBLD

KPnpLowerDevice::KPnpLowerDevice(
	PDEVICE_OBJECT TopOfStackDeviceObject,
	PDEVICE_OBJECT PhysicalDeviceObject,
	NTSTATUS* pStatus
	) :
	KLowerDevice(TopOfStackDeviceObject)
{
	if (m_pDeviceObject != NULL)
	{
		m_PhysicalDeviceObject = PhysicalDeviceObject;
	}
	else
	{
		m_PhysicalDeviceObject = NULL;
		m_ConstructorStatus = STATUS_NO_SUCH_DEVICE;
	}

	*pStatus = ConstructorStatus();

	BOUNDS_CHECKER(CONSTRUCTED_PNP_LOWER_DEVICE, (this, *pStatus));
}

///////////////////////////////////////////////////////////////////////////

KPnpLowerDevice::KPnpLowerDevice(void) :
	KLowerDevice(),
	m_PhysicalDeviceObject(NULL)
{
}

///////////////////////////////////////////////////////////////////////////
// Initialization (when degenerate constructor used to create object)

NTSTATUS KPnpLowerDevice::Initialize(
	PDEVICE_OBJECT TopOfStackDeviceObject,
	PDEVICE_OBJECT PhysicalDeviceObject
	)
{
	// The device should not already be initialized
	ASSERT (m_pDeviceObject == NULL);

	m_pDeviceObject = TopOfStackDeviceObject;
	m_pFileObject = NULL;
	if (m_pDeviceObject != NULL)
	{
		m_PhysicalDeviceObject = PhysicalDeviceObject;
	}
	else
	{
		m_PhysicalDeviceObject = NULL;
		m_ConstructorStatus = STATUS_NO_SUCH_DEVICE;
	}

	BOUNDS_CHECKER(CONSTRUCTED_PNP_LOWER_DEVICE, (this, m_ConstructorStatus));

	return m_ConstructorStatus;
}

////////////////////////////////////////////////////////////////
// Call device, wait for completion
NTSTATUS KPnpLowerDevice::PowerCallWaitComplete(KIrp I, BOOLEAN bKeepIrp, ULONG_PTR* pInformation)
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
		SynchCallCompletionRoutine, // static member of KPnpLowerDevice
		&Ctx,
		TRUE,
		TRUE,
		TRUE
		);

	// Call the lower device

	BOUNDS_CHECKER(POWER_CALL_LOWER_DEVICE, (this, I.m_Irp));

	PoCallDriver(m_pDeviceObject, I.m_Irp);

	// Wait for completion

	e.Wait();

	// extract information from context structure

	if (pInformation)
		*pInformation = Ctx.m_Information;

	BOUNDS_CHECKER(COMPLETED_CALLDOWN, (this, Ctx.m_Status, Ctx.m_Information));

	return Ctx.m_Status;
}

#if (WDM_MAJORVERSION == 1) && (WDM_MINORVERSION < 0x10) // WIN9X
#pragma message ( "KPnpLowerDevice::PowerCall function is not safe on Win9X" )

VOID KPnpLowerDevice::__po_call_driver(PIRP Irp)
{
	PoCallDriver(IoGetNextIrpStackLocation(Irp)->DeviceObject, Irp);
}

NTSTATUS KPnpLowerDevice::__delay_power_call(KIrp I)
{
	I.MarkPending();
	I.NextStackLocation()->DeviceObject = m_pDeviceObject;

	PWORK_QUEUE_ITEM pWorkItem = (PWORK_QUEUE_ITEM)I->Tail.Overlay.DriverContext;
	ExInitializeWorkItem(pWorkItem, (PWORKER_THREAD_ROUTINE)__po_call_driver, I.m_Irp);
	ExQueueWorkItem(pWorkItem, DelayedWorkQueue);

	return STATUS_PENDING;
}

#if (WDM_MINORVERSION < 0x05)	// WIN98
#pragma message ( "KPnpLowerDevice::PnpPowerCall function is not safe on Win9X" )
VOID KPnpLowerDevice::__pnp_po_call_driver(__PNP_PO_CALL_CONTEXT* p)
{
	PoCallDriver(IoGetNextIrpStackLocation(p->m_Irp)->DeviceObject, p->m_Irp);

	KPnpDevice* pDevice = p->m_pDevice;
	delete p;

	pDevice->DecrementOutstandingRequestCount();
}

NTSTATUS KPnpLowerDevice::__delay_pnp_power_call(KPnpDevice* pDevice, KIrp I)
{
	__PNP_PO_CALL_CONTEXT* p = new __PNP_PO_CALL_CONTEXT;

	if ( p )
	{
		p->m_pDevice = pDevice;
		p->m_Irp = I;

		I.MarkPending();
		I.NextStackLocation()->DeviceObject = m_pDeviceObject;

		PWORK_QUEUE_ITEM pWorkItem = (PWORK_QUEUE_ITEM)I->Tail.Overlay.DriverContext;
		ExInitializeWorkItem(pWorkItem, (PWORKER_THREAD_ROUTINE)__pnp_po_call_driver, p);
		ExQueueWorkItem(pWorkItem, DelayedWorkQueue);

		return STATUS_PENDING;
	}
	else
	{
		return I.PnpComplete(pDevice, STATUS_INSUFFICIENT_RESOURCES);
	}
}

#else // WINME

VOID KPnpLowerDevice::__pnp_po_call_driver(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	IoFreeWorkItem((PIO_WORKITEM)Irp->Tail.Overlay.DriverContext[0]);
	PoCallDriver(IoGetNextIrpStackLocation(Irp)->DeviceObject, Irp);
}

NTSTATUS KPnpLowerDevice::__delay_pnp_power_call(KPnpDevice* pDevice, KIrp I)
{
	I.MarkPending();
	I.NextStackLocation()->DeviceObject = m_pDeviceObject;

	PIO_WORKITEM pWorkItem = IoAllocateWorkItem(*pDevice);
	if ( pWorkItem )
	{
		I->Tail.Overlay.DriverContext[0] = pWorkItem;
		IoQueueWorkItem(pWorkItem, (PIO_WORKITEM_ROUTINE)__pnp_po_call_driver, DelayedWorkQueue, I.m_Irp);

		// IO_WORKITEM has an extra reference to pDevice
		pDevice->DecrementOutstandingRequestCount();

		return STATUS_PENDING;
	}
	else
	{
		return I.PnpComplete(pDevice, STATUS_INSUFFICIENT_RESOURCES);
	}
}

#endif // WIN98 or WINME
#endif // WIN9X

#endif
