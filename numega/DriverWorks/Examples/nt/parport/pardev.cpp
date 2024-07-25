// pardev.cpp - implementation for class ParallelDevice
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

// This class implements most of the parport driver. The driver
// serves as a simple contention arbitrator. Each device instance
// maintains data about a physical port. Client drivers may 
// allocate the port via an internal IOCTL interface or via an
// entry point supplied by an earlier IOCTL call. Each device
// may connect to the parallel port's interrupt, and provide
// interrupt dispatching to clients.

#include <vdw.h>
#include <parallel.h>

#include "parport.h"
#include "parlog.h"
#include "pardev.h"

LONG ParallelDevice::m_PortInfoReferenceCount = 0;

// Forward declarations
BOOLEAN TryAllocate(PVOID Context);
VOID FreePort(PVOID Context);
VOID FreePortAtDIRQL(PVOID Context);
BOOLEAN TryAllocateAtDIRQL(PVOID Context);

#pragma code_seg("INIT")
//////////////////////////////////////////////////////////////////////////////
// ParallelDevice Constructor
//
ParallelDevice::ParallelDevice(CONFIG_DATA* pCfgData) :
	KDevice(
		*pCfgData->DeviceName,
		FILE_DEVICE_PARALLEL_PORT),

	m_Interrupt(
		pCfgData->InterfaceType,
		pCfgData->BusNumber,
		pCfgData->InterruptLevel,
		pCfgData->InterruptVector,
		pCfgData->InterruptMode,
		TRUE,
		FALSE
		),

	m_Ports(
		pCfgData->InterfaceType,
		pCfgData->BusNumber,
		pCfgData->Controller.QuadPart,
		pCfgData->SpanOfController,
		TRUE
		)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
#if DBG
		DbgPrint("ParallelDriverDevice: Failed to create device: %x\n", m_ConstructorStatus);
#endif
		m_Unit = 0xffffffff;
		return;
	}

	m_WaitListCount = -1;
	m_IntrConnectCount = 0;
	m_PhysicalPortBase = pCfgData->Controller;
	m_BusType = pCfgData->InterfaceType;
	m_BusNumber = pCfgData->BusNumber;
	m_FreePortDpc.Setup(LinkTo(FreePortDpc), this);
	m_IsrList = NULL;
    m_Unit = IoGetConfigurationInformation()->ParallelCount++;	
	m_ConstructorStatus = ClaimResources(TRUE, TRUE);
}
#pragma code_seg()

#pragma code_seg("PAGE")
//////////////////////////////////////////////////////////////////////////////
// Destructor
//
ParallelDevice::~ParallelDevice(void)
{
	if (m_IntrConnectCount != 0)
		m_Interrupt.Disconnect();

	ClaimResources(FALSE, FALSE);

	if (m_IsrList)
	{
		while ( !m_IsrList->IsEmpty() )
		{
			ISR_LIST_ENTRY* pEntry = m_IsrList->RemoveHead();
			delete pEntry;
		}
		delete m_IsrList;
	}

	if (m_Unit != 0xffffffff)
	    IoGetConfigurationInformation()->ParallelCount--;
}

//////////////////////////////////////////////////////////////////////////////
// CreateClose
//
NTSTATUS ParallelDevice::CreateClose(KIrp I)
{
	return I.Complete(STATUS_SUCCESS);
}
#pragma code_seg()

//////////////////////////////////////////////////////////////////////////////
// CleanUp
//
NTSTATUS ParallelDevice::CleanUp(KIrp I)
{
	KImageSection::ResetAll();
	KIRQL irql = CancelSpinLock::Acquire();
	
	while ( !m_WaitList.IsEmpty() )
	{
		KIrp J = m_WaitList.Head();

		J->Cancel = TRUE;
		J.CancelIrql() = irql;
		J.SetCancelRoutine(NULL);
		Cancel(J);
		CancelSpinLock::Acquire();
	}

	CancelSpinLock::Release();

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////////
// InternalDeviceControl
//
// Parport supports these IOCTL's:
//
//	IOCTL_INTERNAL_PARALLEL_PORT_ALLOCATE
//		Allocate the port. Clients use this as a contention resolution
//		mechanism. The driver pends the request until the port is freed.
//		Freeing the port is via an entry point supplied by the info request.
//
//	IOCTL_INTERNAL_GET_PARALLEL_PORT_INFO
//		Retrieve information about the port.
//
//	IOCTL_INTERNAL_GET_MORE_PARALLEL_PORT_INFO
//		Retrieve additional information about the port.
//
//	IOCTL_INTERNAL_RELEASE_PARALLEL_PORT_INFO
//		Release port information. When the reference count goes to zero,
//		the driver makes itself pageable.
//
//	IOCTL_INTERNAL_PARALLEL_CONNECT_INTERRUPT
//		Connect the interrupt. The caller supplies the address of an
//		ISR to be called.
//
//	IOCTL_INTERNAL_PARALLEL_DISCONNECT_INTERRUPT
//		Disconnect the interrupt.
//

NTSTATUS ParallelDevice::InternalDeviceControl(KIrp I)
{
	NTSTATUS status;

	I.Information() = 0;

	switch (I.IoctlCode())
	{
	case IOCTL_INTERNAL_PARALLEL_PORT_ALLOCATE:

		CancelSpinLock::Acquire();
		if ( I.WasCanceled() )
			status = STATUS_CANCELLED;

		else
		{
			LONG WaitCount;

			if (m_IntrConnectCount != 0)
				SynchronizeInterrupt(
					&m_Interrupt, 
					LinkTo(IncrQueueCount), 
					&WaitCount);
			else
				IncrQueueCount(&WaitCount);

// If the port was already owned (WaitCount > 0), then queue the
// request on the wait list and return status pending. Otherwise,
// return success

			if ( WaitCount != 0 )
			{
				I.SetCancelRoutine( LinkTo(Cancel) );
				I.MarkPending();
				m_WaitList.InsertTail(I);
				CancelSpinLock::Release();
				return STATUS_PENDING;
			}
			else
				status = STATUS_SUCCESS;
		}			

		CancelSpinLock::Release();
		break;

	case IOCTL_INTERNAL_GET_PARALLEL_PORT_INFO:

// Validate the buffer and load the data. The output struct contains
// three entry points into this driver, in addition to information 
// about the physical port. This call also bumps up the global port 
// reference count, which control pageability of the driver.

		if (I.IoctlOutputBufferSize() < sizeof(PARALLEL_PORT_INFORMATION))
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			I.Information() = sizeof(PARALLEL_PORT_INFORMATION);
			PARALLEL_PORT_INFORMATION* pPortInfo = 
				(PARALLEL_PORT_INFORMATION*)I.IoctlBuffer();

			pPortInfo->OriginalController = m_PhysicalPortBase;
			pPortInfo->Controller = (PUCHAR)m_Ports.Base();
			pPortInfo->SpanOfController = m_Ports.Count()*sizeof(UCHAR);
			pPortInfo->TryAllocatePort = TryAllocate;
			pPortInfo->FreePort = FreePort;
			pPortInfo->QueryNumWaiters = QueryWaiters;
			pPortInfo->Context = this;
					
			if (InterlockedIncrement(&m_PortInfoReferenceCount) == 1)
				KImageSection::ResetAll();

			status = STATUS_SUCCESS;

		}
		break;

	case IOCTL_INTERNAL_RELEASE_PARALLEL_PORT_INFO:

// Decrements the port reference count. If no clients are using the port,
// allow the driver to be paged out.

		if (InterlockedDecrement(&m_PortInfoReferenceCount) == 0)
			KImageSection::UnlockAll();

		status = STATUS_SUCCESS;
		break;

	case IOCTL_INTERNAL_GET_MORE_PARALLEL_PORT_INFO:

// Get additonal port information.

		if (I.IoctlOutputBufferSize() < sizeof(MORE_PARALLEL_PORT_INFORMATION))
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			I.Information() = sizeof(MORE_PARALLEL_PORT_INFORMATION);
			MORE_PARALLEL_PORT_INFORMATION* pMoreInfo = 
				(MORE_PARALLEL_PORT_INFORMATION*)I.IoctlBuffer();

			pMoreInfo->InterfaceType = m_BusType;
			pMoreInfo->BusNumber = m_BusNumber;
			pMoreInfo->InterruptLevel = m_Interrupt.BusLevel();
		  	pMoreInfo->InterruptVector = m_Interrupt.BusVector();
			pMoreInfo->InterruptAffinity = m_Interrupt.Affinity();
			pMoreInfo->InterruptMode = m_Interrupt.Mode();

			status = STATUS_SUCCESS;
		}

		break;

	case IOCTL_INTERNAL_PARALLEL_CONNECT_INTERRUPT:

// Connect the interrupt. The caller provides an ISR address to be called	
// when the port issues an interrupt. The driver puts the ISR address on
// its ISR list.

	{
// Validate parameters
		if (
			(I.IoctlInputBufferSize() < sizeof(PARALLEL_INTERRUPT_SERVICE_ROUTINE)) ||
			(I.IoctlOutputBufferSize() < sizeof(PARALLEL_INTERRUPT_INFORMATION)) ) 
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

// The interrupt may be already connected

		CancelSpinLock::Acquire();
		if (m_IntrConnectCount != 0)
		{
			m_IntrConnectCount++;
			CancelSpinLock::Release();
			status = STATUS_SUCCESS;
		}
		else
		{
			CancelSpinLock::Release();

// If not already connected, reclaim the resources

			if ( NT_SUCCESS(ClaimResources(TRUE, TRUE)) )
			{
				status = m_Interrupt.Connect( LinkTo(PortIsr), this);
				if ( !NT_SUCCESS(status) )
				{
					KErrorLogEntry E;
					PIO_ERROR_LOG_PACKET p = E;
					p->ErrorCode = PAR_INTERRUPT_CONFLICT;
					p->UniqueErrorValue = 14,
					p->FinalStatus = status;
					E.SetDumpData(&m_PhysicalPortBase, sizeof(PHYSICAL_ADDRESS));
					E.Post();
					break;
				}
			    m_IntrConnectCount++; //flag that the interrupt is connected
			}
			else	// resource claim failed
			{
				status = STATUS_NO_SUCH_DEVICE;
				break;
			}

// Create the ISR list

			m_IsrList = new (NonPagedPool) 
				KInterruptSafeList<ISR_LIST_ENTRY>(&m_Interrupt);

			if ( !m_IsrList )
			{
				status = STATUS_INSUFFICIENT_RESOURCES;
				break;
			}
		}

// Allocate a struct to put on the list

		ISR_LIST_ENTRY* pIsrEntry = new (NonPagedPool) ISR_LIST_ENTRY;
		if ( !pIsrEntry )
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		PPARALLEL_INTERRUPT_SERVICE_ROUTINE pIsrInput = 
			(PPARALLEL_INTERRUPT_SERVICE_ROUTINE)I.IoctlBuffer();

// Set up the list struct with caller supplied data
				
		pIsrEntry->m_ServiceRoutine = pIsrInput->InterruptServiceRoutine;
		pIsrEntry->m_ServiceContext = pIsrInput->InterruptServiceContext;
		pIsrEntry->m_DeferredPortCheckRoutine = pIsrInput->DeferredPortCheckRoutine;
		pIsrEntry->m_CheckContext = pIsrInput->DeferredPortCheckContext;

// Tack it on to the list

		m_IsrList->InsertTail(pIsrEntry);

		PPARALLEL_INTERRUPT_INFORMATION  pIntrInfo =
			(PPARALLEL_INTERRUPT_INFORMATION)I.IoctlBuffer();

		pIntrInfo->InterruptObject = (PKINTERRUPT)m_Interrupt;
		pIntrInfo->TryAllocatePortAtInterruptLevel = TryAllocateAtDIRQL;
		pIntrInfo->FreePortFromInterruptLevel = FreePortAtDIRQL;
		pIntrInfo->Context = this;

		I.Information() = sizeof(PARALLEL_INTERRUPT_INFORMATION);
		status = STATUS_SUCCESS;
		break;
	}

	case IOCTL_INTERNAL_PARALLEL_DISCONNECT_INTERRUPT:

// Remove the caller's ISR from the list. If the list becomes empty,
// destroy the list and disconnect the interrupt.

	{
		BOOLEAN bNeedToDisconnect=FALSE;

		if (I.IoctlInputBufferSize() < sizeof(PARALLEL_INTERRUPT_SERVICE_ROUTINE))
		{
                status = STATUS_BUFFER_TOO_SMALL;
				break;
		}

		PPARALLEL_INTERRUPT_SERVICE_ROUTINE pIsrInput = 
			(PPARALLEL_INTERRUPT_SERVICE_ROUTINE)I.IoctlBuffer();

		CancelSpinLock::Acquire();
		if (m_IntrConnectCount != 0)
		{
			CancelSpinLock::Release();

			if (SynchronizeInterrupt(&m_Interrupt, LinkTo(RemoveFromIsrList), pIsrInput))
			{
				CancelSpinLock::Acquire();
				m_IntrConnectCount--;
				bNeedToDisconnect = ( m_IntrConnectCount == 0 );
				CancelSpinLock::Release();
				status = STATUS_SUCCESS;
			}
			else
				status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			CancelSpinLock::Release();
			status = STATUS_INVALID_PARAMETER;
		}

		if ( bNeedToDisconnect )
		{
			m_Interrupt.Disconnect();
			ClaimResources(TRUE, FALSE);
			delete m_IsrList;
			m_IsrList = NULL;
		}
		break;
	}
	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	return I.Complete(status);
}

//////////////////////////////////////////////////////////////////////////////
// ClaimResources
//
// Claim the i/o port and interrupt resources for the parallel port.
//
NTSTATUS ParallelDevice::ClaimResources(BOOLEAN bClaimPorts, BOOLEAN bClaimInterrupt)
{
	PUNICODE_STRING pDriverPath=KDriver::DriverInstance()->RegistryPath();

	ULONG pathsize = pDriverPath->Length + 2*sizeof(WCHAR);

	// Since resources are claimed per device and not for the whole driver,
	// we need a unique registry path for each device. The unit number is 
	// used for this purpose. The system creates the key (under the
	// driver key) if it does not already exist.

	KUstring RegPath((USHORT)pathsize, PagedPool);
	KUnitizedName Suffix(L"\\", m_Unit);
	RegPath.Assign(KUstring(pDriverPath, FALSE));
	RegPath.Append(Suffix);

	KResourceRequest ResReq(m_BusType, m_BusNumber, 0);

	// If nothing is claimed, this means we are releasing the resources
	// for this device.

	if ( !bClaimPorts && !bClaimInterrupt)
		return ReleaseResources(RegPath);


	// claim the port
	if (bClaimPorts)
	{
		LARGE_INTEGER addend;
		addend.QuadPart = m_Ports.Count()*sizeof(UCHAR) - 1;
		LARGE_INTEGER MaxAddr = RtlLargeIntegerAdd(addend, m_PhysicalPortBase);

		ResReq.AddPort(
			m_PhysicalPortBase,
			MaxAddr,
			m_Ports.Count()*sizeof(UCHAR),
			1,
			0,
			CmResourceShareDeviceExclusive);
	}

	// claim the interrupt
	if (bClaimInterrupt)
		ResReq.AddIrq(
			m_Interrupt.BusVector(),
			m_Interrupt.BusVector(),
			0,
			CmResourceShareShared,
			m_Interrupt.Mode()
			);	

	// Submit the request. Since there are no options, we don't need to
	// to use a KResourceAssignment object to determine what was assigned.
	// The status return is sufficicent.

	return ResReq.Submit(this, RegPath);
}

//////////////////////////////////////////////////////////////////////////////
// PortIsr
//
// This is the ISR. All it does is invoke each ISR on the port's ISR list
// until one of them returns TRUE.
//
BOOLEAN ParallelDevice::PortIsr(void)
{
	ISR_LIST_ENTRY* pEntry;

// We can safely pass AtDirql==TRUE to member functions of m_IsrList,
// because we know we are at DIRQL. This generates faster code.

	pEntry = m_IsrList->Head(TRUE);
	while ( pEntry )
	{
		if (pEntry->m_ServiceRoutine(m_Interrupt, pEntry->m_ServiceContext))
			return TRUE;

		pEntry = m_IsrList->Next(pEntry, TRUE);
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// ReadQueueCount
//
// This is a DIRQL routine. Member m_WaitListCount may only be accessed
// at Device IRQL if the interrupt is connected. This routine just reads
// the count.
//
BOOLEAN ParallelDevice::ReadQueueCount(PVOID Context)
{
	*(LONG*)Context = m_WaitListCount;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// IncrQueueCount
// 
// This is a DIRQL routine. Member m_WaitListCount may only be accessed
// at Device IRQL if the interrupt is connected. This routine increments
// the count.
//
BOOLEAN ParallelDevice::IncrQueueCount(PVOID Context)
{
	*(LONG*)Context = ++m_WaitListCount;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// DecrQueueCount
// 
// This is a DIRQL routine. Member m_WaitListCount may only be accessed
// at Device IRQL if the interrupt is connected. This routine decrements
// the count.
//
BOOLEAN ParallelDevice::DecrQueueCount(PVOID Context)
{
	*(LONG*)Context = --m_WaitListCount;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Cancel
//
// This is the cancel routine for IRPs that pend attempting to allocate
// the port. The IRPs are queued in the wait list.
//
VOID ParallelDevice::Cancel(KIrp I)
{
	LONG QueueCount;

	if (m_IntrConnectCount != 0)
		SynchronizeInterrupt(
			&m_Interrupt, 
			LinkTo(DecrQueueCount), 
			&QueueCount);
	else
		DecrQueueCount(&QueueCount);

	m_WaitList.Remove(I);

	CancelSpinLock::Release(I->CancelIrql);
	
	I.Information() = 0;	
	I.Complete(STATUS_CANCELLED);
}

//////////////////////////////////////////////////////////////////////////////
// TryAllocate
//
// This is an alternate entry point for allocating the port, which is
// called directly by a client. 
//
BOOLEAN ParallelDevice::TryAllocate(PVOID Context)
{
	BOOLEAN bAllocated;
	ParallelDevice* pDevice = (ParallelDevice*)Context;

	if (pDevice->m_IntrConnectCount > 0)
		bAllocated = 
			pDevice->SynchronizeInterrupt(
				&pDevice->m_Interrupt, 
				(PKSYNCHRONIZE_ROUTINE)TryAllocateAtDIRQL,
				pDevice);
	else
	{
		CancelSpinLock::Acquire();
		bAllocated = TryAllocateAtDIRQL(pDevice);
		CancelSpinLock::Release();
	}

	return bAllocated;
}

//////////////////////////////////////////////////////////////////////////////
// TryAllocateAtDIRQL
//
// This is an alternate entry point for allocating the port, which is
// called directly by a client. It may only be called at DIRQL.
//
BOOLEAN ParallelDevice::TryAllocateAtDIRQL(PVOID Context)
{
	ParallelDevice* pDevice = (ParallelDevice*)Context;

	if (pDevice->m_WaitListCount == -1)
	{
		pDevice->m_WaitListCount = 0;
		return TRUE;
	}
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// FreePort
//
// Clients that allocate the port call this entry point to free it. If
// there is an IRP pending in the wait list, it is granted ownership
// by completing the IRP.
//
VOID ParallelDevice::FreePort(PVOID Context)
{
	LONG QueueCount;
	ParallelDevice* pDevice = (ParallelDevice*)Context;

	CancelSpinLock::Acquire();
	if (pDevice->m_IntrConnectCount != 0)
		pDevice->SynchronizeInterrupt(
			&pDevice->m_Interrupt,
			LinkTo(DecrQueueCount),
			&QueueCount);
	else
		pDevice->DecrQueueCount(&QueueCount);

	if (QueueCount >= 0)
	{
		KIrp I = pDevice->m_WaitList.RemoveHead();
		I.SetCancelRoutine(NULL);
		CancelSpinLock::Release();		
		I.Complete(STATUS_SUCCESS, IO_PARALLEL_INCREMENT);
	}
	else
	{
		ULONG iCount = pDevice->m_IntrConnectCount;		
		CancelSpinLock::Release();		

		if (iCount != 0)
			pDevice->SynchronizeInterrupt(
				&pDevice->m_Interrupt,
				LinkTo(TraversePortCheckList),
				NULL);
	}
}

//////////////////////////////////////////////////////////////////////////////
// FreePortAtDIRQL
//
// Called by a client at DIRQL to free the port. Queues a DPC that calls FreePort.
// 
VOID ParallelDevice::FreePortAtDIRQL(PVOID Context)
{
	ParallelDevice* pDevice = (ParallelDevice*)Context;

	if (pDevice->m_WaitListCount == 0)
		pDevice->m_WaitListCount = -1;
	else
		pDevice->m_FreePortDpc.Request();
}

VOID ParallelDevice::FreePortDpc(PVOID arg1, PVOID arg2)
{
	FreePort(this);
}


//////////////////////////////////////////////////////////////////////////////
// QueryWaiters
//
// Enables clients to determine the number of clients currently waiting
// for the portp
//
ULONG ParallelDevice::QueryWaiters(PVOID Context)
{
	LONG WaitCount;
	ParallelDevice* pDevice = (ParallelDevice*)Context;

	if (pDevice->m_IntrConnectCount != 0)
		pDevice->SynchronizeInterrupt(
			&pDevice->m_Interrupt,
			LinkTo(ReadQueueCount),
			&WaitCount);
	else
	{
		CancelSpinLock::Acquire();
		pDevice->ReadQueueCount(&WaitCount);
		CancelSpinLock::Release();
	}

	return (WaitCount >= 0) ? (ULONG)WaitCount : 0;
}

//////////////////////////////////////////////////////////////////////////////
// RemoveFromIsrList
//
// Locate the entry in the ISR list indicated by the input pointed
// to by Context. Remove the entry from the list. This is a DIRQL
// routine called when a client disconnects the interrupt.
//
BOOLEAN ParallelDevice::RemoveFromIsrList(PVOID Context)
{

// We can safely pass AtDirql==TRUE to member functions of m_IsrList,
// because we know we are at DIRQL. This generates faster code.

	PPARALLEL_INTERRUPT_SERVICE_ROUTINE pRemove = 
		(PPARALLEL_INTERRUPT_SERVICE_ROUTINE)Context;
	ISR_LIST_ENTRY* p = m_IsrList->Head(TRUE);

	while (p != NULL)
	{							   
		if ( (p->m_ServiceRoutine == pRemove->InterruptServiceRoutine) &&
		     (p->m_ServiceContext == pRemove->InterruptServiceContext) )
		{
			m_IsrList->Remove(p, TRUE);
			return TRUE;
		}

		p = m_IsrList->Next(p, TRUE);
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// TraversePortCheckList
//
// Walks the ISR list and calls the port check routine for each entry.
//
BOOLEAN ParallelDevice::TraversePortCheckList(PVOID Context)
{

// We can safely pass AtDirql==TRUE to member functions of m_IsrList,
// because we know we are at DIRQL. This generates faster code.

	if (m_WaitListCount >= 0)
		return FALSE;

	ISR_LIST_ENTRY* pEntry = m_IsrList->Head(TRUE);

	while (pEntry != NULL)
	{
		if (pEntry->m_DeferredPortCheckRoutine != NULL)
			pEntry->m_DeferredPortCheckRoutine(pEntry->m_CheckContext);

		pEntry = m_IsrList->Next(pEntry, TRUE);
	}

	return TRUE;
}
 

