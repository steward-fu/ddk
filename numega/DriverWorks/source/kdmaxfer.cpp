// kdmaxfer.cpp - implementation of class KDmaTransfer
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

//////////////////////////////////////////////////////////////////////////////
// Constructor for KDmaTransfer
//
// The class supports both slaved and bus master transfers. Most of the
// abstraction is done by the system. The class is responsible for making
// the appropriate system calls, depending on the characteristics of the
// adapter.
//
// Scatter/gather is supported for bus masters. If the s/g flag is true in
// the adapter description, the class supplies the client with an array of
// Transfer Descriptors. Each element of the array contains a physical
// address the bus master can transfer to or from, along with a size (byte
// count). The same Transfer Descriptor is supplied to non-s/g bus masters,
// but there is only one such TD per segment.
//
// The class also supports Common Buffers. If the address of a common buffer
// is supplied to the constructor, the class copies data between the common
// buffer and the memory described by the MDL passed to member Initiate.
//
// The constructor saves the input parameters and sets up the object based on
// the characteristics of the adapter.
//
KDmaTransfer::KDmaTransfer(
	KDevice* 			pDevice,
	KDmaAdapter* 		pAdapter,
	KCommonDmaBuffer* 	pBuffer
	) :
	m_pAdapter(pAdapter),
	m_pCommonBuffer(pBuffer),
	m_pDevice(pDevice),
	m_Mdl(NULL),
	m_dir(FromDeviceToMemory),
	m_Callback(NULL),
	m_pContext(NULL),
	m_pCurrentVa(NULL),
	m_State(Idle),
	m_TerminateOnStart(FALSE),
	m_nBytesLeft(0),
	m_pTDs(NULL),
	m_nMaxTDs(0),
	m_nCurrentTDcount(0),
	m_nSequentialTDcount(0),
	m_pMapRegisterBase(NULL),
	m_nRegistersRequested(0),
	m_CurrentTransferSize(0),
	m_BusMaster(FALSE),
	m_AdapterClaimed(FALSE),
	m_RegistersClaimed(FALSE),
	m_DoScatterGather(FALSE),
#if _WDM_ && !defined VXD_COMPATLIB
	m_pScatterGather(NULL),
#endif
	m_BusMasterKeepAdapter(FALSE)
{
	m_BusMaster = pAdapter->Description()->Master;

	// Scatter/Gather is ignored if the adapter is not a bus master.
	m_DoScatterGather = pAdapter->Description()->ScatterGather && m_BusMaster;

	// Since the common buffer is contiguous, ignore the scatter/gather flag if a
	// common buffer is in use.
	if ( m_pCommonBuffer )
	{
		m_DoScatterGather = FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Constructor - requires extended form of Initiate
KDmaTransfer::KDmaTransfer(void) :
	m_pAdapter(NULL),
	m_pCommonBuffer(NULL),
	m_pDevice(NULL),
	m_Mdl(NULL),
	m_dir(FromDeviceToMemory),
	m_Callback(NULL),
	m_pContext(NULL),
	m_pCurrentVa(NULL),
	m_State(Unitialized),
	m_TerminateOnStart(FALSE),
	m_nBytesLeft(0),
	m_pTDs(NULL),
	m_nMaxTDs(0),
	m_nCurrentTDcount(0),
	m_nSequentialTDcount(0),
	m_pMapRegisterBase(NULL),
	m_nRegistersRequested(0),
	m_CurrentTransferSize(0),
	m_BusMaster(FALSE),
	m_AdapterClaimed(FALSE),
	m_RegistersClaimed(FALSE),
	m_DoScatterGather(FALSE),
#if _WDM_ && !defined VXD_COMPATLIB
	m_pScatterGather(NULL),
#endif
	m_BusMasterKeepAdapter(FALSE)
{
}

KDmaTransfer::~KDmaTransfer(void)
{
	if ( m_pTDs )
	{
		delete m_pTDs;
		m_pTDs = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Initiate
//
// The client calls this public member to start a transfer. This routine saves
// the input parameters and allocates the adapter.
//
// Input:
//	pMdl	address of MDL that describes the client buffer to receive data or
//			to supply data.
//	dir		direction of transfer (FromDeviceToMemory or FromMemoryToDevice)
//
NTSTATUS KDmaTransfer::Initiate(
	PMDL pMdl,
	DMA_DIRECTION dir,
	DMAREADY_CALLBACK OnAdapterReady,
	PVOID pContext,
	BOOLEAN BusMasterKeepAdapter
	)
{
	ASSERT (m_State == Idle);

	// There should be no allocated space for transfer descriptors until
	// GetTransferDescriptors() is called for the first time.
	ASSERT (m_pTDs == NULL);

	m_pContext = pContext;
	m_BusMasterKeepAdapter = BusMasterKeepAdapter;

	m_Mdl = KMemory(pMdl);
	m_dir = dir;
	m_nBytesLeft = m_Mdl.Size();
	m_Callback = OnAdapterReady;
	m_nMaxTDs = 1;
	m_nSequentialTDcount = 0;
	m_CurrentTransferSize = 0;

	// Flush the client buffers, since we're about to initiate a DMA operation.
	// This ensures the user's data buffer is flushed from the cache back into
	// memory.  It is only a problem on processors that are non-DMA cache coherent.
    KeFlushIoBuffers(m_Mdl, m_dir == FromDeviceToMemory, TRUE) ;

	if ( m_pCommonBuffer )
	{
		// If a common buffer is in use for this transfer, we need a system
		// virtual address to copy the data to or from.
		m_pCurrentVa = m_Mdl.MapToSystemSpace();

		m_nRegistersRequested = 1;
	}
	else
	{
		// For transfers that do not use a common buffer, m_pCurrentVa is the
		// offset in the MDL for the current address being transferred.
		m_pCurrentVa = m_Mdl.VirtualAddress();
		m_nRegistersRequested = ADDRESS_AND_SIZE_TO_SPAN_PAGES(
			m_pCurrentVa,
			m_nBytesLeft
			);

		// Limit the number of map registers to the max number available in
		// the Adapter
		if ( m_pAdapter->MaxRegisters() < m_nRegistersRequested )
			m_nRegistersRequested = m_pAdapter->MaxRegisters();
	}

	BOUNDS_CHECKER(INITIATED_DMA_TRANSFER, (this, PDEVICE_OBJECT(*m_pDevice), pMdl, m_nRegistersRequested));

	// Allocate the adapter.
	m_State	= AdapterWait;

#if _WDM_ && !defined VXD_COMPATLIB
	if ( m_DoScatterGather )
	{
		return m_pAdapter->QueueSynchRequest(
			*m_pDevice,
			m_Mdl,
			m_pCurrentVa,
			m_nBytesLeft,
			AdapterListControl,
			this,
			m_dir == FromMemoryToDevice);
	}
#endif

	return m_pAdapter->QueueSynchRequest(
			*m_pDevice,
			m_nRegistersRequested,
			AdapterAllocated,
			this);
}

//////////////////////////////////////////////////////////////////////////////
// Initiate - alternate form used with default constructor

NTSTATUS KDmaTransfer::Initiate(
	KDevice* pDevice,
	KDmaAdapter* pAdapter,
	PMDL pmdl,
	DMA_DIRECTION dir,
	DMAREADY_CALLBACK OnAdapterReady,
	KCommonDmaBuffer* pBuffer,
	PVOID pContext,
	BOOLEAN BusMasterKeepAdapter
	)
{
	ASSERT ( (m_State == Idle) || (m_State == Unitialized) );

	m_State = Idle;
	m_pDevice = pDevice;
	m_pAdapter = pAdapter;
	m_pCommonBuffer = pBuffer;
	m_BusMaster = pAdapter->Description()->Master;

	// Scatter/Gather is ignored if the adapter is not a bus master.
	m_DoScatterGather = pAdapter->Description()->ScatterGather && m_BusMaster;

	// Since the common buffer is contiguous, ignore the scatter/gather flag if a
	// common buffer is in use.
	if ( m_pCommonBuffer )
		m_DoScatterGather = FALSE;

	return Initiate(pmdl, dir, OnAdapterReady, pContext, BusMasterKeepAdapter);
}

//////////////////////////////////////////////////////////////////////////////
// Continue
//
// The client calls this public member to start the next segment of the transfer.
// Typically, this means that the DPC that follows the terminal count interrupt
// calls this routine.
//
// Input:
//	XferCountType	informs the transfer object how to determine the number of bytes
// 					that were transferred in the last segment.
//	count			If the value of XferCountType is UseSuppliedValue, this parameter
//					containes the supplied value. Otherwise, it is ignored.
//
VOID KDmaTransfer::Continue(
	TRANSFER_COUNT_TYPE XferCountType,
	ULONG count
	)
{
	// Complete the last transfer.
	CompleteLastTransfer(XferCountType, count);

	// SetupNextTransfer() is used for all transfer types except scatter/gather
	// transfers (which are also BusMaster by default) where the user elects to get
	// their transfer descriptors sequentially.  If this is not a Scatter/Gather
	// transfer, call SetupNextTransfer() now to setup the next transfer,
	// otherwise wait until the user select either GetTransferDescriptors() or
	// SequenceTransferDescriptors() as a method to obtain the transfer descriptors.
	if ( !m_DoScatterGather )
		SetupNextTransfer();

	// The next segment of the transfer is ready to go. Notify the client:
	BOUNDS_CHECKER(DMA_TRANSFER_CALLBACK, (this, *m_pDevice, m_nBytesLeft));

	m_Callback(m_pDevice, m_pDevice->CurrentIrp(), this);
}

//////////////////////////////////////////////////////////////////////////////
// GetTransferDescriptors
//
// Clients using bus master devices call this public member to get the
// physical addresses for the current transfer. For slave devices, this
// function may be called, but the client rarely has any use for the
// information that it returns.
//
// The return value is the count of TDs for the current transfer. For
// devices that do not support scatter/gather, this is always one. For
// scatter/gather devices, each TD supplies a physical address and a
// size. Note that the transfer size is not limited to a single page. If
// the MDL has contiguous physical pages, the system increases the size
// of the transfer accordingly.
//
// This routine writes the address of the TD array to the address passed
// in parameter ppTD.
ULONG KDmaTransfer::GetTransferDescriptors(PTRANSFER_DESCRIPTOR* ppTD)
{
	// If the adapter supports scatter/gather, allocate an array of transfer
	// descriptors the first time this function is called. For a single page,
	// avoid the allocation and use the single TD embedded in this transfer
	// object.
	if ( m_DoScatterGather && (m_nRegistersRequested > 1) && (m_pTDs == NULL) )
	{
		m_pTDs = new (NonPagedPool) TRANSFER_DESCRIPTOR[m_nRegistersRequested];

	// If the allocation was successful set the new max number of TDs,
	// otherwise we will use the single embedded TD to provide graceful
	// failure.
		if ( m_pTDs != NULL )
			m_nMaxTDs = m_nRegistersRequested;
	}

	// We deferred setting up the next transfer for scatter/gather devices.
	// Set it up now.
	if ( m_DoScatterGather )
		SetupNextTransfer();

	if ( m_nMaxTDs > 1 )
		*ppTD = m_pTDs;
	else
		*ppTD = &m_SingleTD;

	return m_nCurrentTDcount;
}

//////////////////////////////////////////////////////////////////////////////
// SequenceTransferDescriptors
//
// Clients using bus master devices call this public member itteratively
// to get the physical addresses for the current transfer.
// For slave devices, this function may be called, but the client rarely
// has any use for the information that it returns.
//
// The return value indicates if a valid TD was returned.  If FALSE,
// all of the TDs have been accessed.
// Note that the transfer size is not limited to a single page. If
// the MDL has contiguous physical pages, the system increases the size
// of the transfer accordingly.
//
// This routine writes the address of the TD array to the address passed
// in parameter ppTD.
BOOLEAN KDmaTransfer::SequenceTransferDescriptors(PTRANSFER_DESCRIPTOR* ppTD)
{
	BOOLEAN bReturn = FALSE;

	if ( !m_DoScatterGather )
	{
		if ( m_nSequentialTDcount < 1 )
		{
			m_nSequentialTDcount++;
			bReturn = TRUE;

	// Return our embedded TD
			*ppTD = &m_SingleTD;
		}
	}
	else
	{
		if ( ((m_nBytesLeft - m_CurrentTransferSize) > 0) &&
			(m_nSequentialTDcount < m_nRegistersRequested) )
		{
#if _WDM_ && !defined VXD_COMPATLIB
			if ( m_pScatterGather )
			{
				m_SingleTD.td_Length = m_pScatterGather->Elements[m_nSequentialTDcount].Length;
				m_SingleTD.td_PhysAddr = m_pScatterGather->Elements[m_nSequentialTDcount].Address;
			}
			else
#endif
			{
				m_SingleTD.td_Length = m_nBytesLeft - m_CurrentTransferSize;

				// Do the mapping for this transfer descriptor.
				m_SingleTD.td_PhysAddr = m_pAdapter->MapTransfer(
					m_Mdl,
					m_pMapRegisterBase,
					(PUCHAR)m_pCurrentVa + m_CurrentTransferSize,
					&m_SingleTD.td_Length,
					(m_dir == FromMemoryToDevice)
					);

				BOUNDS_CHECKER(MAP_DMA_TRANSFER, (this, m_pMapRegisterBase, m_pCurrentVa, m_SingleTD.td_Length, m_SingleTD.td_PhysAddr));
			}

			m_CurrentTransferSize += m_SingleTD.td_Length;

			m_nSequentialTDcount++;
			bReturn = TRUE;

			// Return our embedded TD
			*ppTD = &m_SingleTD;
		}
	}

	return bReturn;
}

//////////////////////////////////////////////////////////////////////////////
// Terminate
//
// Called by the client when the transfer is complete. Normally, the client
// calls this from the callback routine if there are no bytes left to transfer.
// It also may be called from the callback routine at other stages of the
// transfer to prematurely terminate. The client should not delete the object
// until the state is Idle.
//
// NOTE: This routine should only be called from the DMAREADY_CALLBACK routine.
//
VOID KDmaTransfer::Terminate(void)
{
	// For slaved DMA that terminates while transfer incomplete, call CompleteLastTransfer
	// to flush the buffers
	if ( !m_BusMaster && (m_nBytesLeft > 0) )
		CompleteLastTransfer(UseSuppliedValue, m_CurrentTransferSize);

	// Release the adapter's resources
	if ( m_AdapterClaimed )
		m_pAdapter->Release();
	else if ( m_RegistersClaimed )
		m_pAdapter->ReleaseRegisters(m_pMapRegisterBase, m_nRegistersRequested);

	// Delete any heap allocation we may have for transfer descriptors.
	if ( m_pTDs != NULL )
	{
		delete m_pTDs;
		m_pTDs = NULL;
	}

	// If we are just starting this transfer, set the flag m_TerminateOnStart.
	// This will signal the OnAdapterAllocated() routine to deallocate
	// the adapter object.
	if ( m_State == DeviceStart )
		m_TerminateOnStart = TRUE;

	// Most of the state that changes from transfer to transfer will get
	// initialized in Initiate().  Clear some important ones.
	m_AdapterClaimed = FALSE;
	m_RegistersClaimed = FALSE;
	m_State = Idle;

	BOUNDS_CHECKER(TERMINATED_DMA_TRANSFER, (this));
}

//////////////////////////////////////////////////////////////////////////////
// AdapterAllocated
//
// This is a protected static member, called by the system when the adapter
// is allocated. It invokes the non-static member that actually starts the
// transfer.
//
IO_ALLOCATION_ACTION KDmaTransfer::AdapterAllocated(
    PDEVICE_OBJECT DeviceObject,
    PIRP pIrp,
	PVOID MapRegisterBase,
    PVOID Context
    )
{
	BOUNDS_CHECKER(ALLOCATED_DMA_ADAPTER, ((KDmaTransfer*)Context, MapRegisterBase));

	return ((KDmaTransfer*)Context)->OnAdapterAllocated(MapRegisterBase);
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(pIrp);
}

//////////////////////////////////////////////////////////////////////////////
// OnAdapterAllocated
//
// This routine is called by AdapterAllocated. It sets up the first segment of
// the transfer and invokes the client's callback via dpc.
//
IO_ALLOCATION_ACTION KDmaTransfer::OnAdapterAllocated(PVOID MapRegisterBase)
{
	m_State = DeviceStart;
	m_pMapRegisterBase = MapRegisterBase;

	if ( m_BusMaster && !m_BusMasterKeepAdapter )
	{
		m_AdapterClaimed = FALSE;
	}

	// SetupNextTransfer() is used for all transfer types except scatter/gather
	// transfers (which are also BusMaster by default) where the user elects to get
	// their transfer descriptors sequentially.  If this is not a Scatter/Gather
	// transfer, call SetupNextTransfer() now to setup the next transfer,
	// otherwise wait until the user select either GetTransferDescriptors() or
	// SequenceTransferDescriptors() as a method to obtain the transfer descriptors.
	if ( !m_DoScatterGather )
		SetupNextTransfer();

	// Call the user's callback routine to start the transfer.
	BOUNDS_CHECKER(DMA_TRANSFER_CALLBACK, (this, *m_pDevice, m_nBytesLeft));

	m_Callback(m_pDevice, m_pDevice->CurrentIrp(), this);

	if ( m_TerminateOnStart )
	{
		// If the transfer was terminated in the callback, it should have
		// never been started on the device.  Deallocate both the registers
		// and the object.
		m_TerminateOnStart = FALSE;
		return DeallocateObject;
	}
	else if ( m_BusMaster && !m_BusMasterKeepAdapter )
	{

		// If the device is a busmaster, and the user didn't explicitly ask for
		// the adapter object to be kept, release the adapter and keep the registers.
		m_RegistersClaimed = TRUE;
		m_State = DeviceWait;
		return DeallocateObjectKeepRegisters;
	}
	else
	{

		// Keep both the adapter and the registers.
		m_AdapterClaimed = TRUE;
		m_RegistersClaimed = TRUE;
		m_State = DeviceWait;
		return KeepObject;
	}
}

#if _WDM_ && !defined VXD_COMPATLIB
//////////////////////////////////////////////////////////////////////////////
// AdapterListControl
//
// This is a protected static member, called by the system when the scatter
// gather list is allocated. It invokes the non-static member that actually starts the
// transfer.
//
VOID KDmaTransfer::AdapterListControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP pIrp,
	PSCATTER_GATHER_LIST ScatterGather,
    PVOID Context
    )
{
	((KDmaTransfer*)Context)->OnAdapterListControl(ScatterGather);
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(pIrp);
}

//////////////////////////////////////////////////////////////////////////////
// OnAdapterListControl
//
// This routine is called by AdapterListControl.
//
VOID KDmaTransfer::OnAdapterListControl(PSCATTER_GATHER_LIST ScatterGather)
{
	m_State = DeviceStart;
	m_pScatterGather = ScatterGather;

	// Call the user's callback routine to start the transfer.
	BOUNDS_CHECKER(DMA_TRANSFER_CALLBACK, (this, *m_pDevice, m_nBytesLeft));

	m_Callback(m_pDevice, m_pDevice->CurrentIrp(), this);

	if ( m_TerminateOnStart )
	{
		if ( m_pScatterGather )
			m_pAdapter->PutScatterGatherList(m_pScatterGather, m_dir == FromMemoryToDevice);

		m_pScatterGather = NULL;
		m_TerminateOnStart = FALSE;
	}
	else
	{
		m_State = DeviceWait;
	}

	return;
}
#endif

//////////////////////////////////////////////////////////////////////////////
// SetupNextTransfer
//
// This routine determines the physical address(es) for the next segment of
// the transfer.
VOID KDmaTransfer::SetupNextTransfer(void)
{
	PTRANSFER_DESCRIPTOR pCurTD;

	// Set up the pointer to the TD array.
	if ( m_nMaxTDs == 1 )
		pCurTD = &m_SingleTD;
	else
		pCurTD = m_pTDs;

	m_CurrentTransferSize = 0;

	if ( m_pCommonBuffer )
	{
		// Handle the common buffer case
		pCurTD->td_Length = m_pCommonBuffer->Size();
		if ( pCurTD->td_Length > (ULONG)m_nBytesLeft )
			pCurTD->td_Length = m_nBytesLeft;

		pCurTD->td_PhysAddr = m_pCommonBuffer->LogicalAddress();

		if ( m_dir == FromMemoryToDevice )
			RtlCopyMemory(m_pCommonBuffer->VirtualAddress(), m_pCurrentVa,
			pCurTD->td_Length);

		// There is no need to call IoMapTransfer for bus master transfer to or
		// from a common buffer. The physical address of the CB is already known,
		// and there is no system DMA controller to be set up. For slave transfers
		// to or from a common buffer, the DMA controller must be programmed.
		if ( !m_BusMaster && (pCurTD->td_Length > 0) )
			m_pAdapter->MapTransfer(
				m_pCommonBuffer->Mdl(),
				m_pMapRegisterBase,
				m_pCommonBuffer->Mdl().VirtualAddress(),
				&pCurTD->td_Length,
				(m_dir == FromMemoryToDevice)
				);

		m_CurrentTransferSize = pCurTD->td_Length;
		m_nCurrentTDcount = 1;

	    KeFlushIoBuffers(m_pCommonBuffer->Mdl(), m_dir==FromDeviceToMemory, TRUE);
	}
	else
	{

		// Handle the case where there is no common buffer in use
		ULONG XferSize = m_nBytesLeft;
		m_nCurrentTDcount = 0;
		PVOID va = m_pCurrentVa;

		// While we have bytes to transfer, and have map registers and transfer
		// descriptors available to map the data into, initialize the transfer
		// descriptors and map the map registers.  Note that the max number of
		// transfer descriptors (m_nMaxTDs) is limited first by whether or not
		// this is a scatter/gather transfer (if not m_nMaxTDs = 1), next by the
		// number of available registers in the adapter, and finally by how many
		// transfer descriptors we were able to allocate (may be 1 if NonPagedPool
		// allocation failed in Initiate()).
		while ( (XferSize > 0) && (m_nCurrentTDcount < m_nMaxTDs) )
		{
			// Determine the size of the next segment of the transfer.
			if ( m_DoScatterGather )
			{
				pCurTD->td_Length = XferSize;
			}
			else
			{
				struct {
					MDL mdl;
					ULONGLONG page;
				} startmem;

				KMemory Start(m_pCurrentVa, 1, &startmem.mdl);

				ULONG nPages = ADDRESS_AND_SIZE_TO_SPAN_PAGES(va, XferSize);

				if ( nPages <= m_pAdapter->MaxRegisters() )
					pCurTD->td_Length = XferSize;
				else
				{
					nPages = m_pAdapter->MaxRegisters();
					pCurTD->td_Length = (nPages*PAGE_SIZE) - Start.OffsetFromPageBase();
				}
			}

#if _WDM_ && !defined VXD_COMPATLIB
			if ( m_pScatterGather )
			{
				pCurTD->td_PhysAddr = m_pScatterGather->Elements[m_nCurrentTDcount].Address;
				pCurTD->td_Length = m_pScatterGather->Elements[m_nCurrentTDcount].Length;
			}
			else
#endif
			{
				// Do the mapping. This is required for both slaved and bus master transfers.
				pCurTD->td_PhysAddr = m_pAdapter->MapTransfer(
					m_Mdl,
					m_pMapRegisterBase,
					va,
					&pCurTD->td_Length,
					(m_dir == FromMemoryToDevice)
					);

				BOUNDS_CHECKER(MAP_DMA_TRANSFER, (this, m_pMapRegisterBase, va, pCurTD->td_Length, pCurTD->td_PhysAddr));
			}


			m_nCurrentTDcount++;

			m_CurrentTransferSize += pCurTD->td_Length;
			XferSize -= pCurTD->td_Length;
			va = (PVOID) ((PCHAR)va + pCurTD->td_Length);
			pCurTD++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// CompleteLastTransfer
//
// This is called by Continue.

VOID KDmaTransfer::CompleteLastTransfer(
	TRANSFER_COUNT_TYPE XferCountType,
	ULONG count
	)
{
	ULONG nTransferred;

	ASSERT ( !(m_BusMaster && (XferCountType == UseHardwareCounter)) );

	switch (XferCountType)
	{
	case UseTransferSize:
		nTransferred = m_CurrentTransferSize;
		break;

	case UseHardwareCounter:
		nTransferred = m_CurrentTransferSize - m_pAdapter->ReadCounter();
		break;

	case UseSuppliedValue:
		nTransferred = count;
		break;

	// This is an error!
	default:
		ASSERT (FALSE);
		nTransferred = 0;
		break;
	}

#if _WDM_ && !defined VXD_COMPATLIB
	if ( m_pScatterGather )
	{
		m_pAdapter->PutScatterGatherList(m_pScatterGather, m_dir == FromMemoryToDevice);
		m_pScatterGather = NULL;
	}
	else
#endif

	if ( !(m_pCommonBuffer && m_BusMaster) && nTransferred )
	{
		m_pAdapter->FlushBuffers(
			m_Mdl,
			m_pMapRegisterBase,
			m_pCurrentVa,
			nTransferred,
			m_dir == FromMemoryToDevice
			);
	 }

	// For common buffer transfers to memory,  copy the data from the device out of the
	// common buffer into client memory.
	if ( m_pCommonBuffer )
	{
		if ( m_dir == FromDeviceToMemory )
			RtlCopyMemory(m_pCurrentVa, m_pCommonBuffer->VirtualAddress(), nTransferred);
	}

	m_pCurrentVa = (PVOID) ((PCHAR)m_pCurrentVa + nTransferred);
	m_nBytesLeft -= nTransferred;

	BOUNDS_CHECKER(COMPLETED_TRANSFER_SEGMENT, (this, nTransferred, m_pCurrentVa));
}
