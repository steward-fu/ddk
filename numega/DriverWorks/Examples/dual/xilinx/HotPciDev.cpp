// HotPciDev.cpp - implementation of class HotIIDevice
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
// Copyright (c) 1998 Virtual Computer Corporation

#include <vdw.h>
#include "HotPciDev.h"

extern KDebugOnlyTrace T;

#if ! _WDM_
//////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

// Constructor for non-WDM systems
//
// Input
//	Slot		PCI slot in which the board resides
//	Unit		Unit number (used to form device/link name)
//
// NOTE: The use of the default base class constructor is required to 
//       allocate object's memory in the device extension.

HotPciDevice::HotPciDevice(KPciSlot& Slot, ULONG Unit) :
	KDevice(),
		m_Pci(Slot.Bus(), Slot.Device(), Slot.Function())
{
// Store the unit information
	m_Unit = Unit;

// Initialize the pointer to the shared event object to NULL
	m_SharedEvent = NULL;

// Get the registry key for this device
	KUnitizedName* pRegistryPath;
	pRegistryPath = CreateRegistryPath(HOTPCI_NAME_STEM, Unit);
	if ( pRegistryPath == NULL )
	{
		T << "Failed to create registry path\n";
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}

// Get the resources for this device
	KResourceRequest ResourceReq( PCIBus, Slot.Bus(), Slot.Slot() );
	m_ConstructorStatus = ResourceReq.Submit(this, *pRegistryPath);
	delete pRegistryPath;
	if ( !NT_SUCCESS(m_ConstructorStatus) )
		return;

// Now get the IRQ, I/O Port, and Memory region for this device	
	KResourceAssignment PortAsg( &ResourceReq, CmResourceTypePort );
	KResourceAssignment MemAsg( &ResourceReq, CmResourceTypeMemory );
	KResourceAssignment IrqAsg( &ResourceReq, CmResourceTypeInterrupt );

	if ( !(PortAsg.Status() && MemAsg.Status() && IrqAsg.Status()) )
	{
		m_ConstructorStatus = STATUS_CONFLICTING_ADDRESSES;
		T << "Unable to allocate hardware resources. ";
		if ( !PortAsg.Status() )
			T << "Port range assignment failed. ";
		if ( !MemAsg.Status() )
			T << "Mem range assignment failed. ";
		if ( !IrqAsg.Status() )
			T << "IRQ assignment failed. ";
		T << "\n";
	}
					
// Set up the I/O Port Range
	if (NT_SUCCESS(m_ConstructorStatus))
		m_ConstructorStatus = m_Port.Initialize(
			PCIBus,
			Slot.Bus(),
			PortAsg.Start().QuadPart,
			PortAsg.Length()
			);

	T << "Unit " << m_Unit << " I/O Base=" << ULONG(m_Port.Base()) 
	  << " Length=" << ULONG(m_Port.Count()) << " status=" << ULONG(m_ConstructorStatus) << "\n";

// Set up the Memory Range
	if (NT_SUCCESS(m_ConstructorStatus))
		m_ConstructorStatus = m_Memory.Initialize(
			PCIBus,
			Slot.Bus(),
			MemAsg.Start().QuadPart,
			MemAsg.Length(),
			TRUE
			);

	T << "Unit " << m_Unit << " Mem physical=" << m_Memory.CpuPhysicalAddress().LowPart
	  << " Mem virtual=" << ULONG(m_Memory.Base()) 
	  << " Length=" << ULONG(m_Memory.Count()) << " status=" << ULONG(m_ConstructorStatus) << "\n";

// Initialize and connect the interrupt object	
	if (NT_SUCCESS(m_ConstructorStatus))
	{
		m_Irq.Initialize(
			PCIBus,				// interface type
			Slot.Bus(),			// bus number
			IrqAsg.Level(),		// interrupt level
			IrqAsg.Vector(),	// interrupt vector
			LevelSensitive,		// mode
			TRUE,				// share
			FALSE				// save float
			);

		m_ConstructorStatus = m_Irq.Connect(LinkTo(TheIsr), this);

		T << "Unit " << m_Unit << " IRQ=" << IrqAsg.Vector() << " status="
		  << ULONG(m_ConstructorStatus) << "\n";

// Initialize the DPC
		m_InterruptDpc.Setup(LinkTo(InterruptDpc), this);

// Save the BARs

		m_OriginalBAR0 = m_Pci.ReadBaseAddress(0);
		m_OriginalBAR1 = m_Pci.ReadBaseAddress(1);
		m_OriginalInterrupt = m_Pci.ReadInterrupt();
	}

// Initialize the queue
	m_DeviceQueue.Initialize(*this);
}
#pragma code_seg()
//
// End INIT Section code
//////////////////////////////////////////////////////////////////////

#else

// Constructor for WDM systems
//
// Input
//	Pdo			Physical Device Object
///	Unit		Unit number (used to form device/link name)

HotPciDevice::HotPciDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KPnpDevice(Pdo),
		m_Pci(Pdo)
{
// Store the unit information
	m_Unit = Unit;

// Initialize the pointer to the shared event object to NULL
	m_SharedEvent = NULL;

// Initialize the connection to the PDO

	m_Pdo.Initialize(this, Pdo);

// Establish the m_Pdo as the lower device for the object

	SetLowerDevice(&m_Pdo);

// Adopt the standard Plug and Play policy

	SetPnpPolicy();

// Save the BARs

	m_OriginalBAR0 = m_Pci.ReadBaseAddress(0);
	m_OriginalBAR1 = m_Pci.ReadBaseAddress(1);

// Initialize the queue
	m_DeviceQueue.Initialize(Pdo);
}

//////////////////////////////////////////////////////////////////////
//
// OnStartDevice - Plug and Play handler for IRP_MN_START_DEVICE
//
NTSTATUS HotPciDevice::OnStartDevice(KIrp I)
{
	NTSTATUS status;
	PCM_RESOURCE_LIST pTranslatedList = I.TranslatedResources();
	PCM_RESOURCE_LIST pRawList = I.AllocatedResources();
// base class has already handled PDO call

	status = m_Port.Initialize(pTranslatedList, pRawList, 0, TRUE);
	if ( !NT_SUCCESS(status) )
		T << "Failed to obtain expected I/O range resource.\n";
	else
		T << "I/O range address " << ULONG_PTR(m_Port.Base()) << " Length " << m_Port.Count() << "\n";

	status = m_Memory.Initialize(pTranslatedList, (ULONG)0, TRUE);
	if ( !NT_SUCCESS(status) )
		T << "Failed to obtain expected memory range resource.\n";	
	else
		T << "Mem range address " << ULONG_PTR(m_Memory.Base()) << " Length " << m_Memory.Count() << "\n";

	status = m_Irq.InitializeAndConnect(pTranslatedList, LinkTo(TheIsr), this,	0);
	if ( !NT_SUCCESS(status) )
		T << "Failed to obtain expected IRQ resource.\n";
	else
		T << "Interrupt vector is " << m_Irq.Vector() << "\n";

	return status;
	// base class will handle completion
}

//////////////////////////////////////////////////////////////////////
//
// OnStopDevice - Plug and Play handler for IRP_MN_STOP_DEVICE
//
NTSTATUS HotPciDevice::OnStopDevice(KIrp I)
{
	m_Port.~KIoRange();
	m_Memory.~KMemoryRange();
	m_Irq.~KInterrupt();

	return STATUS_SUCCESS;
	// base class will handle completion and PDO call
}

//////////////////////////////////////////////////////////////////////
//
// OnRemoveDevice - Plug and Play handler for IRP_MN_REMOVE_DEVICE
//
NTSTATUS HotPciDevice::OnRemoveDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	// nothing specific to do, all PnP operations handled by base class

	return status;
}

NTSTATUS HotPciDevice::DefaultPnp(KIrp I)
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}

NTSTATUS HotPciDevice::DefaultPower(KIrp I)
{
	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Pdo.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
// SystemControl
//
// This routine just passes the IRP through to the next device since this driver
// is not a WMI provider.
//
NTSTATUS HotPciDevice::SystemControl(KIrp I) 
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}


#endif // _WDM_

//////////////////////////////////////////////////////////////////////
//
// Destructor
//
HotPciDevice::~HotPciDevice(void)
{
	T << "device deconstructor\n";

#if ! _WDM_
	KUnitizedName* pRegistryPath;
	pRegistryPath = CreateRegistryPath(HOTPCI_NAME_STEM, m_Unit);

	if (pRegistryPath != NULL)
		ReleaseResources(*pRegistryPath);
	delete pRegistryPath;
#endif 

}

//////////////////////////////////////////////////////////////////////
//
// Handler for IRP_MJ_CREATE
//
NTSTATUS HotPciDevice::Create(KIrp I)
{
	T << "Opening HotII (Create)\n";

	I.Information() = 0;
	return COMPLETE_IRP(I,STATUS_SUCCESS);
}

//////////////////////////////////////////////////////////////////////
//
// Handler for IRP_MJ_CLOSE
//
NTSTATUS HotPciDevice::Close(KIrp I)
{
	T << "Closing HotII (Close)\n";

	return COMPLETE_IRP(I, STATUS_SUCCESS);
}

//////////////////////////////////////////////////////////////////////
//
// Handler for IRP_MJ_CLEANUP
//
NTSTATUS HotPciDevice::CleanUp(KIrp I)
{
	KIrp J(CurrentIrp());
	if ( !J.IsNull() )
	{
		CancelSpinLock::Acquire();
		J.SetCancelRoutine(NULL);
		J.Information() = 0;
		J.Status() = STATUS_CANCELLED;
		CancelSpinLock::Release();
		COMPLETE_IRP(J, STATUS_CANCELLED);
	}

	// remove pending IRPs
	m_DeviceQueue.CleanUp(NULL);

	I.Information() = 0;
	return COMPLETE_IRP(I, STATUS_SUCCESS);
}

//////////////////////////////////////////////////////////////////////
//
// Handler for IRP_MJ_DEVICE_CONTROL
//
NTSTATUS HotPciDevice::DeviceControl(KIrp I)
{
	NTSTATUS status=STATUS_SUCCESS;
	ULONG Operation = I.IoctlCode();

	// initialize the Information field of the IRP to zero
	I.Information() = 0;

	T << "Device Control: operation=" << Operation << "\n";

	switch (Operation)
	{
	////////////////////////////////////////////////////////////////////

	case DIOC_GET_VERSION:
		break;

	////////////////////////////////////////////////////////////////////

	case DIOC_READ:			// METHOD_OUT_DIRECT
	case DIOC_WRITE:		// METHOD_IN_DIRECT
	{		
		HOTII_READ_WRITE_MEMORY_ARGS* pArgs = 
			(HOTII_READ_WRITE_MEMORY_ARGS*)I.IoctlBuffer();

		if (((pArgs->DwordCount*4)+pArgs->ByteOffset) >= MEMORY_SIZE)
		{
			T << "Invalid r/w args: Count=0x" << pArgs->DwordCount
			  << ", Offset=0x" << pArgs->ByteOffset << "\n";

			status = STATUS_ACCESS_DENIED;
			break;
		}

		if ( pArgs->DwordCount == 0 )
		{
			T << "Warning: r/w operation has zero count\n";
			break;
		}

		return QUEUE_IRP(I);
	}

	////////////////////////////////////////////////////////////////////

	case DIOC_IO_READ:		// METHOD_BUFFERED
	case DIOC_IO_WRITE:		// METHOD_BUFFERED
	{		
		HOTII_READ_WRITE_IO_ARGS* pArgs = 
			(HOTII_READ_WRITE_IO_ARGS*)I.IoctlBuffer();

		if ( (pArgs->PortOffset + 4) >= m_Port.Count())
		{
			T << "Invalid i/o args:  Offset=0x" 
			  << pArgs->PortOffset
			  << " count=" << m_Port.Count() << "\n";

			status = STATUS_ACCESS_DENIED;
			break;
		}

		return QUEUE_IRP(I);
	}

	////////////////////////////////////////////////////////////////////

	case DIOC_INT_EN:		// METHOD_NEITHER
	{
		HOTII_INTERRUPT_ENABLE_ARGS* pArgs = 
			(HOTII_INTERRUPT_ENABLE_ARGS*)I.IoctlType3InputBuffer();

		KEvent* pOldEvent;
		KEvent* pNewEvent;
		HANDLE  hEvent;

	// The user buffer was not probed by the system (METHOD_NEITHER)
		_try
		{
			hEvent = pArgs->EventHandle;
		}
		_except ( EXCEPTION_EXECUTE_HANDLER )
		{
			status = STATUS_ACCESS_DENIED;
			break;
		}


	// Validate paraemter
		if ( hEvent == NULL )
		{
			T << "Event handle is NULL\n";

			status = STATUS_INVALID_PARAMETER;
			break;
		}

	// Save the prior event object pointer; create a new event object
	// with the new handle
		pOldEvent = m_SharedEvent;
		pNewEvent = new (NonPagedPool) KEvent(hEvent);
		
		if ( pNewEvent == NULL )
		{
			T << "Unable to construct event object\n";
			
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

	// prevent changing the event while a DPC is running
		m_Lock.Lock();		
		m_SharedEvent = pNewEvent;
		m_Lock.Unlock();

	// discard the old event object
		if ( pOldEvent != NULL )
			delete pOldEvent;		

		break;
	}

	////////////////////////////////////////////////////////////////////

	case DIOC_INT_DI:	// METHOD_NEITHER
	{
		KEvent* pOldEvent;

		pOldEvent = m_SharedEvent;

	// prevent changing the event while a DPC is running
		m_Lock.Lock();
		m_SharedEvent = NULL;
		m_Lock.Unlock();

	// discard the old event object
		if ( pOldEvent != NULL )
			delete pOldEvent;		
	}


	////////////////////////////////////////////////////////////////////


	case DIOC_GETDEVINFO:	// METHOD_BUFFERED

		T << "Get DevInfo\n";

		
	// Validate the buffer size
		if ( I.IoctlOutputBufferSize() < GETDEVINFO_SIZE )
		{
			T << "DIOC_GETDEVINFO buffer is too small\n";

			status = STATUS_BUFFER_TOO_SMALL;
		}
		else	// buffer ok
		{
			m_Pci.ReadHeader(I.IoctlBuffer(), 0, GETDEVINFO_SIZE);
			I.Information() = GETDEVINFO_SIZE;
		}

		break;

	////////////////////////////////////////////////////////////////////

	case DIOC_BASE_ADDRESSES:		// METHOD_NEITHER
	{
		HOTII_GET_BASE_ARGS* pArgs;

		_try
		{
			pArgs = (HOTII_GET_BASE_ARGS*)I.UserBuffer();
			pArgs->IoAddress = m_Port.Base();
			pArgs->MemAddress = PVOID((ULONG_PTR)m_Memory.CpuPhysicalAddress().LowPart);
			I.Information() = sizeof PVOID;
		}
		_except ( EXCEPTION_EXECUTE_HANDLER )
		{
			status = STATUS_ACCESS_DENIED;
		}

		break;
	}	

	case CTL_CODE(0,1,METHOD_IN_DIRECT,0): // =5
	{
		T << "Bogus Get Base Addresses\n";
#pragma warning(disable:4509)
		KMemory M(I.Mdl());

		m_Pci.ReadHeader(M.VirtualAddress(), 0, GETDEVINFO_SIZE);
		I.Information() = GETDEVINFO_SIZE;

		break;
	}


	////////////////////////////////////////////////////////////////////

	case DIOC_INT_ARG:		// METHOD_NEITHER
	{
		HOTII_GET_INTERRUPT_REGISTER* pArgs;

		_try
		{
			pArgs = (HOTII_GET_INTERRUPT_REGISTER*)I.UserBuffer();
			pArgs->reg = m_IntReg;
			I.Information() = sizeof ULONG;
		}
		_except ( EXCEPTION_EXECUTE_HANDLER )
		{
			status = STATUS_ACCESS_DENIED;
		}

		break;
	} 

	////////////////////////////////////////////////////////////////////

	case DIOC_REWRITE_BARS:		// METHOD_NEITHER
	{
		T << "Rewrite BARs\n";

		ReloadOriginalConfig();

		break;
	}
  
   	////////////////////////////////////////////////////////////////////

	default:
		ASSERT(FALSE);
		I.Information() = 0;
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	return COMPLETE_IRP(I, status);
}

//////////////////////////////////////////////////////////////////////
// ReloadOriginalConfig
//
// After the board is reset, some config settings must be reloaded.
//
VOID HotPciDevice::ReloadOriginalConfig(void)
{
		ASSERT( m_OriginalBAR0 != 0 );
		ASSERT( m_OriginalBAR1 != 0 );

		m_Pci.WriteBaseAddress(0, m_OriginalBAR0);
		m_Pci.WriteBaseAddress(1, m_OriginalBAR1);
		m_Pci.WriteInterrupt(m_OriginalInterrupt);
		m_Pci.WriteCommandRegister(3);
}


//////////////////////////////////////////////////////////////////////
// StartIo Routine
//
// This processes queued IRPs for this unit serially
//
// NOTE: This method is called from the driver managed queue m_DeviceQueue
//       Unlike the system-managed queue, the IRP is submitted in a 
//		 non-cancelable state.
//
VOID HotPciDevice::StartIo(KIrp I)
{
	ASSERT (I.MajorFunction() == IRP_MJ_DEVICE_CONTROL);

	I.Status() = STATUS_SUCCESS;
	ULONG op = I.IoctlCode();

	switch (op)
	{
	case DIOC_WRITE:
	case DIOC_READ:
	{
		HOTII_READ_WRITE_MEMORY_ARGS* pArgs =
			(HOTII_READ_WRITE_MEMORY_ARGS*)I.IoctlBuffer();

		KMemory M(I.Mdl());
		PULONG pBuffer = (PULONG)M.MapToSystemSpace();

		if (op == DIOC_READ)
		{

			T << "Read ram: offset=" << pArgs->ByteOffset
			  << " Count=" << pArgs->DwordCount 
			  << "\n";

			m_Memory.ind(
				pArgs->ByteOffset,
				pBuffer,
				pArgs->DwordCount
				);
		}
		else // DIOC_WRITE
		{
			T << "Write ram: offset=" << pArgs->ByteOffset
			  << " Count=" << pArgs->DwordCount 
			  << "\n";

			m_Memory.outd(
				pArgs->ByteOffset,
				pBuffer,
				pArgs->DwordCount
				);
		}

		I.Information() = 0;
		break;
	}

	case DIOC_IO_WRITE:
	case DIOC_IO_READ:
	{
		HOTII_READ_WRITE_IO_ARGS* pArgs = 
			(HOTII_READ_WRITE_IO_ARGS*)I.IoctlBuffer();

		if (op == DIOC_IO_READ)	
		{
			pArgs->Data = m_Port.ind(pArgs->PortOffset);
			I.Information() = sizeof ULONG;

			T << "I/O Read: Port=" << pArgs->PortOffset
			  << " data=" << pArgs->Data
			  << "\n";


		}
		else // DIOC_IO_WRITE
		{
			m_Port.outd( pArgs->PortOffset, pArgs->Data );
			I.Information() = 0;

			T << "I/O Write: Port=" << pArgs->PortOffset
			  << " data=" << pArgs->Data
			  << "\n";

		}
		break;
	}

	default:
		ASSERT(FALSE);
		I.Information() = 0;
		I.Status() = STATUS_INVALID_PARAMETER;
	}
 
	NEXT_IRP(I);
}


//////////////////////////////////////////////////////////////////////
//
// Interrupt Service Routine
//
BOOLEAN HotPciDevice::TheIsr(void)
{
	ReloadOriginalConfig();
	
	ULONG CcmZero = m_Memory.ind(CCM_OFFSET+0);
	if ( (CcmZero & 1) == 0 ) // bit 0 == 0 means interrupt is active
	{
		m_Memory.outd(CCM_OFFSET+CLR_INT0_OFFSET, 0); // clear the interrupt
		m_InterruptDpc.Request( 0 );
		return TRUE;
	}
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////
//
// DPC for Interrupt
//
VOID HotPciDevice::InterruptDpc( PVOID arg1, PVOID arg2)
{
	T << "dpc\n";

	m_Lock.Lock(TRUE);
	if (m_SharedEvent)
		m_SharedEvent->Set();
	m_Lock.Unlock();
}


