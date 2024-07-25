// PciDev.cpp - implementation of BasicPciDevice device class
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#include <vdw.h>

#include "basicpci.h"
#include "pcidev.h"

extern KDebugOnlyTrace T;

////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

////////////////////////////////////////////////////////////////////
// Constructor
//
// Input:
//
//	previous   Points to the last constructed instance, or NULL if
//	           this is the first instance. This enables the PCI
//	           enumeration process to detect multiple instances of
//	           the same card.
//
//	Unit       Unit number of this instance
//
//
BasicPciDevice::BasicPciDevice(BasicPciDevice* previous, ULONG Unit) :

	// Base class
	KDevice(
		KUnitizedName(L"BasicPci", Unit),
		FILE_DEVICE_UNKNOWN,
		KUnitizedName(L"BasicPci", Unit),
		0,
		DO_DIRECT_IO
		),

	// KPciConfiguation	
	m_Cfg(VENDOR_ID, DEVICE_ID, previous ? &previous->m_Cfg : NULL),

	// KInterrupt
	m_Interrupt(),

	// Unit number
	m_Unit(Unit)
{

	// Make sure the base class was constructed successfully
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		T << TraceError 
		  << "Failed to create unit " 
		  << Unit 
		  <<  "status= " << ULONG(m_ConstructorStatus) << "\n";

		return;
	}

	// Check to see if the specified device was found on a PCI bus
	if (m_Cfg.IsValid())
		T << "PCI config construction succeeded\n";
	else
	{
		T << "PCI config construction FAILED\n";
		m_ConstructorStatus = STATUS_DEVICE_NOT_CONNECTED;
		return;
	}

	// Generate the registry path of the registry key for this unit
	KUnitizedName* RegistryPath = 
		CreateRegistryPath(L"BasicPciDevice", Unit);

	// Get the resources. We are expecting an I/O range and an interrupt.
	// The system queries the device to detect the resources automatically 
	// based on the bus number and slot. It is not necessary to specify
	// the resources we are requesting.
	KResourceRequest ResReq(PCIBus, m_Cfg.Bus(), m_Cfg.Slot());

	// Submit the request
	m_ConstructorStatus = ResReq.Submit(this, *RegistryPath);

	// Registry path no longer needed.
	delete RegistryPath;

	// Determine if the resource request succeeded.
	if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
		T << "Resource request failed: " << ULONG(m_ConstructorStatus) << "\n";
		return;	// request failed
	}

	// Create an instance of KResourceAssignment for
	// each resource expected. First the I/O range:
	KResourceAssignment IoResource(
		&ResReq, 
		CmResourceTypePort, 
		0
		);

	if (IoResource.Status())
	{
		T << TraceInfo
		  << "Found I/O range starting at " 
		  << IoResource.Start().LowPart 
		  << ", length=" 
		  << IoResource.Length()
		  << "\n";

		// Set up the KIoRange member using the values determined by
		// the resource assignment
		m_ConstructorStatus = m_IoRange.Initialize(
			PCIBus,
			m_Cfg.Bus(),
			IoResource.Start().QuadPart,
			IoResource.Length(),
			TRUE		
			);

		if ( !NT_SUCCESS(m_ConstructorStatus) )
		{
			T << TraceError << 	"Failed to initialize I/O range: status="
			  << ULONG(m_ConstructorStatus) << "\n";

			return;
		}
	}
	else
	{
		T << "Failed to find I/O range\n";
		m_ConstructorStatus = STATUS_UNSUCCESSFUL;
		return;
	}

	// Now set up the interrupt
	KResourceAssignment InterruptResource(
		&ResReq,
		CmResourceTypeInterrupt,
		0
		);
	
	if (InterruptResource.Status())
	{
		ULONG IrqLevel = InterruptResource.Level();
		ULONG IrqVector = InterruptResource.Vector();

		T << "Assigned interrupt vector=" << IrqVector << "\n";

		// Create the interrupt object
		m_Interrupt.Initialize( 
				PCIBus,
				m_Cfg.Bus(),
				IrqLevel,
				IrqVector,
				LevelSensitive,
				TRUE,
				FALSE
				);

		// If the interrupt object is successfully created, connect it
		m_ConstructorStatus = m_Interrupt.Connect(LinkTo(Isr), this);
		if ( !NT_SUCCESS(m_ConstructorStatus) )
		{
			T << "Could not connect interrupt: " << ULONG(m_ConstructorStatus) << "\n";
		}
		else // connected, initialize embedded DPC object
		{
			T << "Interrupt connected, initializing DPC.\n";
			m_Dpc.Setup(LinkTo(Dpc), this);
		}
	}
	else // resource assignment failed
	{
		T << "Failed to assign IRQ resource\n";
		m_ConstructorStatus = STATUS_UNSUCCESSFUL;
	}
}

#pragma code_seg()
//
// End INIT section code
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Destructor
//
BasicPciDevice::~BasicPciDevice(void)
{
	T << "In destructor\n";

	KResourceRequest ResReq(PCIBus, m_Cfg.Bus(), m_Cfg.Slot());
	KUnitizedName* RegistryPath = 
		CreateRegistryPath(L"BasicPciDevice", m_Unit);

	ReleaseResources(*RegistryPath);
	delete RegistryPath;
}

// IRP handlers

///////////////////////////////////////////////////////////////////////
// Create
//
NTSTATUS BasicPciDevice::Create(KIrp I)
{
	T << "Entering Create: " << I << "\n";

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////////
// Close
//
NTSTATUS BasicPciDevice::Close(KIrp I)
{
	T << "Entering Close: " << I << "\n";

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////////
// DeviceControl
//
NTSTATUS BasicPciDevice::DeviceControl(KIrp I)
{
	T << "Entering DeviceControl: " << I << "\n";

// TODO: Validate parameters here

	return QueueIrp(I, LinkTo(CancelQueuedIrp));
}

///////////////////////////////////////////////////////////////////////
// Read
//
NTSTATUS BasicPciDevice::Read(KIrp I)
{
	T << "Entering Read: " << I << "\n";

// TODO: Validate parameters here

	return QueueIrp(I, LinkTo(CancelQueuedIrp));
}

///////////////////////////////////////////////////////////////////////
// Write
//
NTSTATUS BasicPciDevice::Write(KIrp I)
{
	T << "Entering Write: " << I << "\n";

// TODO: Validate parameters here

	return QueueIrp(I, LinkTo(CancelQueuedIrp));
}

////////////////////////////////////////////////////////////////////
//	StartIo
// 
VOID BasicPciDevice::StartIo(KIrp I)
{
	T << "Entering StartIo: " << I << "\n";

	// Before processing the Irp, we need to check to see if it has been
	// cancelled and set it to a non-cancellable state.  When performing
	// these operations, it is necessary to hold the global cancel spin
	// lock and take special precautions to ensure the Irp is still valid.
	// This is accomplished using the routine KIrp::TestAndSetCancelRoutine().
	if ( !I.TestAndSetCancelRoutine(
		LinkTo(CancelQueuedIrp),
		NULL,
		CurrentIrp()) )
	{
		
		// The Irp has been cancelled we stop processing and exit.  Since
		// it was in a cancellable state previously, it will be completed by
		// the cancel routine that had been set on it.
		return;
	}

	// Start processing request. Switch on the IRP's function:
	switch (I.MajorFunction())
	{
		case IRP_MJ_DEVICE_CONTROL:

			break;

		case IRP_MJ_READ:

			break;

		case IRP_MJ_WRITE:

			break;

		default:
			// Error - unexpected IRP received
			break;
	}
}

////////////////////////////////////////////////////////////////////
//	CancelQueuedIrp
//		This routine is called when a queued IRP is cancelled. The
//		global cancel spin lock is held, and must be released upon
//		exit.
//
VOID BasicPciDevice::CancelQueuedIrp(KIrp I)
{
	T << "Entering CancelQueuedIrp: " << I << "\n";

	KDeviceQueue dq(DeviceQueue());

	if ( (PIRP)I == CurrentIrp() )
	{
		CurrentIrp() = NULL;
		CancelSpinLock::Release(I.CancelIrql());
		I.Information() = 0;
		I.Status() = STATUS_CANCELLED;
		NextIrp(I, IO_NO_INCREMENT, TRUE);
	}
	else if (dq.RemoveSpecificEntry(I))
	{
		CancelSpinLock::Release(I.CancelIrql());
		I.Information() = 0;
		I.Complete(STATUS_CANCELLED);
	}
	else
	{
		CancelSpinLock::Release(I.CancelIrql());
	}
}

////////////////////////////////////////////////////////////////////
// Isr
//
BOOLEAN BasicPciDevice::Isr(void)
{
	
	// Determine if the controlled device caused the interrupt
	if ( 1==1 ) // replace with actual test code
	{
		// acknowledge interrupt and stabilize device

		// queue a dpc
		m_Dpc.Request();		
		return TRUE;		// interrupt was handled
	}
	else
		return FALSE;		// interrupt was not handled
}

////////////////////////////////////////////////////////////////////
// Dpc
//
VOID BasicPciDevice::Dpc(PVOID arg1, PVOID arg2)
{

	// In many cases this is where the current IRP is completed.
	// The actual implementation will be more complicated.
	KIrp I = CurrentIrp();

	I.Information() = 0; 	// set to number of bytes transferred
	I.Status() = STATUS_SUCCESS;
	NextIrp(I);
}
