// KNdisRxAreaAsyncMgr.h: interface for the _RxAreaAsyncMgr class.
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
//=============================================================================

#if !defined(AFX_KNDISRXAREAASYNCMGR_H__3A4EA9D5_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_)
#define AFX_KNDISRXAREAASYNCMGR_H__3A4EA9D5_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

namespace _KNDIS {


///////////////////////////////////////////////////////////////
// This class is used as the Async class for KNdisSharedReceiveArea
// for miniports that require Asynchronous shared memory
// allocations using NdisMAllocateSharedMemoryAsync.
//
// This class is used internally by KNdisSharedReceiveArea.
// The class has template parameter Tr which provides Traits
// for memory allocations.  The client can specialize the
// default traits class KNdisReceiveAreaTraits to adjust
// allocation size, number of allocations, and cached/uncached
// allocations.
//
// This class is used as the 2nd template parameter to
// KNdisSharedReceiveArea.  This class is the default parameter
// to KNdisSharedReceiveArea.  To declare a KNdisSharedReceiveArea
// object using this class, use the following:
//
// KNdisSharedReceiveArea< MyAdapterClass > MyRxArea
//
// where MyAdapterClass is your KNdisMiniAdapter derived class.
//
template<class Tr>
class _RxAreaAsyncMgr
{
// Construction
public:
	inline _RxAreaAsyncMgr(
		KNdisMiniAdapter& Adapter,
		ULONG InitialLength,
		ULONG MaximumLength,
		ULONG HighWaterMark,
		ULONG LowWaterMark,
		KNdisHeap<KNdisReceiveArea::Desc_t> *pSwDesc
		);

	inline ~_RxAreaAsyncMgr();

// Methods
public:
	// Reporting use of memory, usually called from RxDPC()
	BOOLEAN			Debit(ULONG amount);

	// Reporting that memory is no longer in use, usually called from ReturnPacket
	inline BOOLEAN	Credit(ULONG amount);

	// Add allocated memory, usually called from AllocateComplete handler
	PVOID			AllocateComplete(KNdisSharedMemory* pHwDescMem, UINT NumOfDescriptors);

	// Report packet is no longer in use
	BOOLEAN			Remove(PNDIS_PACKET pkt);

	// Call by AddDescriptors when it failed to chain to KNdisReceiveArea
	inline BOOLEAN	RemoveFailedDescriptor();

protected:

	// Request allocation, called from Debit()
	NDIS_STATUS		AllocateMemoryChunk();

// Data
protected:

	typedef KNdisReceiveArea::Desc_t Desc_t, * PSwDesc_t;

	//policies
	ULONG	m_HighWaterMark;	// Threshold of available memory where any more makes de-allocation possible
	ULONG	m_LowWaterMark;		// Threshold of available memory where any less requires allocation (WATER= available memory)
	ULONG	m_MaxLength;		// Maximum allocation length

	//state
	ULONG	m_AvailableMem;			// Memory currently being used by the client
	BOOLEAN	m_bAllocationPending;	// Flag indicating that an allocation is pending
	BOOLEAN m_bDeAllocationPossible;// Flag indicating that based on current memory usage, a de-allocation is possible.
	UINT	m_nAllocIndex;

	KNdisMiniAdapter&	m_Adapter;
	KNdisHeap<Desc_t>*	m_pSwDesc;	// descriptor area allocated in system memory
};


////////////////////////////////////////////////////////////////////
// Implementations


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
//	Adapter			- KNdisMiniAdapter&
//	InitialLength	- Initial amount of allocated memory
//	MaximumLength	- Maximum amount of allocated memory
//	HighWaterMark	- Resource condition to start 'Free'ing memory
//	LowWaterMark	- Low resource condition
// Returns:
// 	None
// Comments:
// 	None
template <class Tr>
_RxAreaAsyncMgr<Tr>::_RxAreaAsyncMgr(
	KNdisMiniAdapter& Adapter,
	ULONG InitialLength,
	ULONG MaximumLength,
	ULONG HighWaterMark,
	ULONG LowWaterMark,
	KNdisHeap<KNdisReceiveArea::Desc_t> *pSwDesc
	):
		m_Adapter(Adapter),
		m_AvailableMem(InitialLength),
		m_MaxLength(MaximumLength),
		m_HighWaterMark(HighWaterMark),
		m_LowWaterMark(LowWaterMark),
		m_pSwDesc(pSwDesc),
		m_bAllocationPending(FALSE),
		m_bDeAllocationPossible(FALSE),
		m_nAllocIndex(0)
{}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	None
// Returns:
// 	None
// Comments:
// 	None
template <class Tr>
_RxAreaAsyncMgr<Tr>::~_RxAreaAsyncMgr()
{
}


///////////////////////////////////////////////////////////////////////
// Report use of memory
//
// Parameters:
// 	dwDebit		- Amount of memory to report in use
// Returns:
//	TRUE = an allocation has been requested, FALSE = no allocation required
// Comments:
// Usually called from RxDPC() prior to indicating packets to NDIS
template <class Tr>
BOOLEAN
_RxAreaAsyncMgr<Tr>::Debit(ULONG dwDebit)
{
	BOOLEAN bRetVal = FALSE;

	if( dwDebit <= m_AvailableMem )
	{
		m_AvailableMem -= dwDebit;
	}

	if( dwDebit > m_AvailableMem || m_AvailableMem < m_LowWaterMark )
	{
		if( TRUE == m_bAllocationPending )
		{
			return TRUE;
		}

		// Need to allocate
		if( NDIS_STATUS_PENDING == AllocateMemoryChunk())
		{
			bRetVal = TRUE;
		}
		else
		{
			bRetVal = FALSE;
		}
	}

	return bRetVal;
}


///////////////////////////////////////////////////////////////////////
// Report memory no longer in use
//
// Parameters:
// 	dwCredit	- Amount of memory to report no longer in use
// Returns:
//	TRUE = a de-allocation is possible, FALSE = de-allocation not possible
// Comments:
// Usually called from ReturnPackets()
template <class Tr>
BOOLEAN
_RxAreaAsyncMgr<Tr>::Credit(ULONG dwCredit)
{
	if( (m_AvailableMem += dwCredit) >= m_HighWaterMark )
	{
		// The high water mark has been reached
		// Check if there are any async memory
		if(m_nAllocIndex)
		{
			m_bDeAllocationPossible = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////
// Request allocation
//
// Parameters:
// 	dwCredit	- Amount of memory to report no longer in use
// Returns:
//	NDIS_STATUS_PENDING = an allocation was requested and is pending.
//	NDIS_STATUS_XXXX = allocation failed...
// Comments:
//	Called from Debit() - requests an allocation using KNdisSharedMemory
//	CTOR which wraps NdisMAllocateSharedMemoryAsync.  This will result
//  in Adapter class AllocateComplete() handler being called with a
//	pointer to the KNdisSharedMemory object.  The requested allocation
//	size and cached parameter are obtained from the Tr (Traits) class.
template <class Tr>
NDIS_STATUS
_RxAreaAsyncMgr<Tr>::AllocateMemoryChunk()
{
	NDIS_STATUS AllocationStatus = NDIS_STATUS_RESOURCES;

	// check if our strategy calls for continued allocations
	UINT nAllocSize = Tr::GetAllocSize();
	if(nAllocSize)
	{
		// Request an allocation based on strategy
		// This ctor calls NdisMAllocateSharedMemoryAsync
		KNdisSharedMemory* p = new KNdisSharedMemory(
			&m_Adapter,
			nAllocSize,
			Tr::Cached()
			);

		if( p )
		{
			AllocationStatus = p->AllocationStatus();

			if( NDIS_STATUS_PENDING != AllocationStatus )
			{
				delete p;
			}
			else
			{
				m_bAllocationPending = TRUE;
			}
		}
		else
		{
			AllocationStatus = NDIS_STATUS_RESOURCES;
		}
	}

	return AllocationStatus;
}

///////////////////////////////////////////////////////////////////////
// Report that an asynchronous allocation is complete
//
// Parameters:
// 	pShMem	- KNdisSharedMemory object describing the shared memory
//				This was allocated in AllocateNextChunk()
//	NumOfDescriptors - Number of h/w descriptors described by pShMem
// Returns:
//	Pointer to s/w descriptor buffer.
// Comments:
//	Called by clients from AllocateComplete() handler as a result of
//	an allocation request in AllocateNextChunk().  This method
//	allocates lookaside list memory for s/w descriptors.
template <class Tr>
PVOID
_RxAreaAsyncMgr<Tr>::AllocateComplete(KNdisSharedMemory* pShMem, UINT NumOfDescriptors)
{
	if( NULL == pShMem || !pShMem->IsValid() )
		return NULL;

	//allocate a memory chunk for an array of software descriptors
	KNdisReceiveArea::Desc_t* pSwDesc;
	pSwDesc = (KNdisReceiveArea::Desc_t*)m_pSwDesc->alloc();
	NdisZeroMemory(pSwDesc, sizeof(Desc_t));
	pSwDesc->pHwDescMem = pShMem;
	if( NULL == pSwDesc )
	{
		return NULL;
	}

	// update state of this object
	m_AvailableMem += pShMem->Length();
	m_bAllocationPending = FALSE;
	m_nAllocIndex++;

	return pSwDesc;
}

///////////////////////////////////////////////////////////////////////
// Try to remove the use of a packet.
//
// Parameters:
//	pkt		- Packet to attempt to remove
// Returns:
//	TRUE	- The packet was removed
//	FALSE	- The packet was not removed
// Comments:
//	This method will remove 'use' of the packet if its in the shared
//	memory object to be deallocated.  If there are no de-allocations to
//	perform, the method returns FALSE.
template <class Tr>
BOOLEAN
_RxAreaAsyncMgr<Tr>::Remove(PNDIS_PACKET pkt)
{
	// Get pointer to s/w descriptor
	Desc_t* pSwDesc = *reinterpret_cast<Desc_t**>(pkt->MiniportReserved);
	if( NULL == pSwDesc ) 
		return FALSE;

	// Get pointer to h/w descriptor (virtual address)
	PUCHAR pDesc = reinterpret_cast<PUCHAR>(pSwDesc->pHwDesc);
	if( NULL == pDesc ) 
		return FALSE;

	if(pSwDesc->pHwDescMem)
		delete pSwDesc->pHwDescMem;

	m_pSwDesc->free(pSwDesc);

	UINT nAllocSize = Tr::GetAllocSize();
	if( nAllocSize <= m_AvailableMem )
	{
		m_AvailableMem -= nAllocSize;
		m_nAllocIndex--;
		return TRUE;
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////
// Request that shared memory be removed (free'd).
//
// Parameters:
//	None
// Returns:
//	TRUE	- Memory can be removed
//	FALSE	- no available memory to free
// Comments:
//  Call by AddDescriptors when it failed to chain to KNdisReceiveArea
template<class Tr>
BOOLEAN
_RxAreaAsyncMgr<Tr>::RemoveFailedDescriptor()
{
	BOOLEAN bRetVal = FALSE;

	UINT nAllocSize = Tr::GetAllocSize();
	if( nAllocSize <= m_AvailableMem )
	{
		m_AvailableMem -= nAllocSize;
		m_nAllocIndex--;
		bRetVal = TRUE;
	}
	return bRetVal;
}

};

#endif // !defined(AFX_KNDISRXAREAASYNCMGR_H__3A4EA9D5_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_)
