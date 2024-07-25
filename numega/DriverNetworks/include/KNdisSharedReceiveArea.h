// KNdisSharedReceiveArea.h: interface for the KNdisSharedReceiveArea class.
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

#if !defined(AFX_KNDISSHAREDRECEIVEAREA_H__3A4EA9D3_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_)
#define AFX_KNDISSHAREDRECEIVEAREA_H__3A4EA9D3_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>


/////////////////////////////////////////////////////////////////////////////
// KNdisSharedReceiveArea
//
// This derivation is for drivers that interact with the NIC via shared memory.
// The class preallocates buffers that the NIC will use to DMA receive frames.
// The client of this class is required to provide 3 template parameters.
//
// The 1st template parameter is the client's KNdisMiniAdapter derived class.
// This class must provide method signatures that KNdisSharedReceiveArea will
// call to manipulate the h/w descriptors at appropriate times - setup,
// complete, and reclaim.  Whenever the NIC "completes" h/w descriptor(s),
// Complete() is to be called to advance the engine and return the packet, which
// becomes ready. This packet can then be NdisMReceiveIndicate-d.
// When NDIS is done with the packet (e.g. on MiniportReturnPacket), the driver
// calls Reclaim() to return the packet (and appropriate descriptor) to the "free list".
// The following are the required signatures:
//
//		void DescriptorInitialize(D* pHwDesc, PNDIS_PHYSICAL_ADDRESS pPhysAddr);
//			This is called so that the client can initialize a h/w desc.
//			Usually called multiple times to initialize an entire group of descriptors
//
//		void DescriptorInvalidate(D* pHwDesc);
//			This is called so that the client can perform any acctions associated with
//			de-initializing a h/w desc.	Usually called multiple times to de-initialize
//			an entire group of descriptors
//
//		void DescriptorReclaim(D* pHwDesc, PNDIS_PHYSICAL_ADDRESS pPhysAddr);
//			This is called during reclaim processing so the client can update
//			the h/w descriptor.  pLastDesc is a pointer to the last descriptor on the free
//			descriptor list.
//
//		void DescriptorComplete(D* pHwDesc, PNDIS_PHYSICAL_ADDRESS pPhysAddr);
//			This is called during complete processing so the client can update
//			the h/w descriptor
//
// The 2nd template parameter is the Hardware RFD structure (client defined).
// This structure must define the following static methods:
//
//		ULONG GetBufferLength(void);
//			BufferLength for the RFD used by NdisAllocateBuffer which could
//			be different than sizeof(D)
//
//		ULONG GetBufferOffset(void);
//			Offset from beginning of D (the h/w RFD) to the beginning of the
//			Buffer.  This is used so that the receive area can calculate the address
//			of the buffer.
//
// It is assumed that the buffer itself is contained within D.
//
// The 3rd template parameter is an allocator class for optional asynchronous memory support.
// The framework provides default classes to specialize the behavior of the Receive
// area.  Class KNdisReceiveAreaNoMemAlloc provides required method signatures for
// no asynchronous allocation support.  This allows for maximum efficiency for those
// drivers that do not have dynamic memory support by providing implementations of
// required signatures which compile to NOP's.   Class KNdisSharedReceiveAreaMemAllocator<S>
// provides for asynchronous allocations based on strategy template class S.  A default
// struct KNdisMemAllocatorStrategy is provided.  The client can override methods to
// further specialize the behavior of the asynchronous support or provide an entire class
// for complete specicalization of the asynchronous support.
//
// This derivation requires its client to provide an NDIS shared memory object,
// which is used to build the h/w descriptors in.
//
// For deserialized drivers, the client must hold an adapter wide spin lock at
// appropriate times to protect the internals of the receive area.
//
template<class A, typename D, class Async = _KNDIS::_RxAreaAsyncMgr< KNdisReceiveAreaTraits< A, D > > >
class KNdisSharedReceiveArea : public KNdisReceiveArea {

	friend class Async;

// Construction
public:
	SAFE_DESTRUCTORS;

	KNdisSharedReceiveArea(
		A& Adapter,
		KNdisSharedMemory& ShMem,
		UINT InitialNumOfDescriptors,
		UINT MaximumNumOfDescriptors,
		UINT MinFreeDescriptors
		);

	KNdisSharedReceiveArea(
		A& Adapter,
		KNdisSharedMemory& ShMem,
		UINT InitialNumOfDescriptors,
		UINT MaximumNumOfDescriptors,
		UINT MinFreeDescriptors,
		UINT HighWaterMark,
		UINT LowWaterMark
		);

	~KNdisSharedReceiveArea();

	PREVENT_COPY_CONSTRUCTORS(KNdisSharedReceiveArea);

// Methods
public:

	// Next receive is completed.
	PNDIS_PACKET	Complete(D* pHwDesc=NULL);

	// Packet previously removed by Complete() gets back
	void			Reclaim(PNDIS_PACKET pkt, BOOLEAN bFreeAsyncMem=FALSE);

	// Packet previously removed by Complete() gets back
	void			ReclaimFailedPacket(PNDIS_PACKET pkt);

	// For asynchronous memory support.  Called from A::AllocateComplete() handler.
	BOOLEAN			OnAllocated(KNdisSharedMemory& ShMem);

	// Add descriptors for the shared memory - must be called after the CTOR
	NDIS_STATUS		AddDescriptors(KNdisSharedMemory& ShMem, UINT NumOfDescriptors);

protected:
	// Packet previously removed by Complete() gets back - Used internally
	void			_Reclaim(PNDIS_PACKET pkt);

//Data
protected:

	// Pointer to client's Adapter derived class
	A&					m_Adapter;

	// Shared memory for HW RFD (initial)  Optional Asynchronous support is stored in Async class
	KNdisSharedMemory&	m_ShdMem;

	// Asyncronous memory manager (optional)
	Async				m_AsyncMgr;

	KNdisSpinLock		m_Lock;
};


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pAdapter				- Adapter object pointer (usually, 'this')
//	ShMem					- KNdisSharedMemory object which descibes
//								the initial chunk of shared memory for the h/w RFD's
//	InitialNumOfDescriptors - Initial number of Receive Frame Descriptors(RFD)
//	MaximumNumOfDescriptors - Maximum number of Receive Frame Descriptors(RFD)
//	MinFreeDescriptors		- Number of Receive Frame Descriptors(RFD) that 'must' be free
// Returns:
// 	none
// Comments:
// 	none
template<class A, typename D, class Async>
KNdisSharedReceiveArea<A, D, Async>::KNdisSharedReceiveArea(
	A& Adapter,
	KNdisSharedMemory& ShMem,
	UINT InitialNumOfDescriptors,
	UINT MaximumNumOfDescriptors,
	UINT MinFreeDescriptors
	) : KNdisReceiveArea(
			&Adapter,
			InitialNumOfDescriptors,
			MaximumNumOfDescriptors,
			MinFreeDescriptors
			),
		m_Adapter(Adapter),
		m_ShdMem(ShMem),
		m_AsyncMgr()
{
}


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pAdapter				- Adapter object pointer (usually, 'this')
//	ShMem					- KNdisSharedMemory object which descibes
//								the initial chunk of shared memory for the h/w RFD's
//	InitialNumOfDescriptors - Initial number of Receive Frame Descriptors(RFD)
//	MaximumNumOfDescriptors - Maximum number of Receive Frame Descriptors(RFD)
//	MinFreeDescriptors		- Number of Receive Frame Descriptors(RFD) that 'must' be free
//	HighWaterMark			-
//	LowWaterMark			-
// Returns:
// 	none
// Comments:
// 	none
template<class A, typename D, class Async>
KNdisSharedReceiveArea<A, D, Async>::KNdisSharedReceiveArea(
	A& Adapter,
	KNdisSharedMemory& ShMem,
	UINT InitialNumOfDescriptors,
	UINT MaximumNumOfDescriptors,
	UINT MinFreeDescriptors,
	UINT HighWaterMark,
	UINT LowWaterMark
	) : KNdisReceiveArea(
			&Adapter,
			InitialNumOfDescriptors,
			MaximumNumOfDescriptors,
			MinFreeDescriptors
			),
		m_Adapter(Adapter),
		m_ShdMem(ShMem),
		m_AsyncMgr(
			Adapter,
			InitialNumOfDescriptors*sizeof(D),
			MaximumNumOfDescriptors*sizeof(D),
			HighWaterMark,
			LowWaterMark,
			&m_SwDesc
			)
{
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
//	none
// Comments:
//	none
template<class A, typename D, class Async>
KNdisSharedReceiveArea<A, D, Async>::~KNdisSharedReceiveArea()
{
	// Unchain all of the packets and call the "Manipulator" to further
	// handle the invalidation of the Hw descriptors
	// Can't call this while holding a spin lock
	D* pHwDesc;
	while (pHwDesc = reinterpret_cast<D*>(Unchain()))
	{
		m_Adapter.DescriptorInvalidate(pHwDesc);
	}
}


///////////////////////////////////////////////////////////////////////
// Add descriptors for the shared memory
//
// Parameters:
//	ShMem				- KNdisSharedMemory object which descibes
//							the chunk of shared memory for the h/w RFD's
//	NumOfDescriptors	- number of Receive Frame Descriptors(RFD)
// Returns:
// 	NDIS_STATUS
// Comments:
// 	Initializes s/w descriptors that were allocated by this object.
//	Calls client's adapter methods for initializing h/w descriptors.
template<class A, typename D, class Async>
NDIS_STATUS
KNdisSharedReceiveArea<A, D, Async>::AddDescriptors(KNdisSharedMemory& ShMem, UINT NumOfDescriptors)
{
	//loop thru the number of descriptors
		//chain the buffers - puts the descriptors on the Free list
		//initialize the descriptors
		//call DescriptorInitialize() for each
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;
	ASSERT( ShMem.Length() / sizeof(D) >= NumOfDescriptors );

	ULONG PhysAddrHigh = NdisGetPhysicalAddressHigh(*ShMem.PhysicalAddress());
	ULONG PhysAddrLow = NdisGetPhysicalAddressLow(*ShMem.PhysicalAddress());
	ULONG dwBufLen = D::GetBufferLength();
	ULONG dwBufOffset = D::GetBufferOffset();
	D* pHwDesc = reinterpret_cast<D*>(ShMem.VirtualAddress());

	m_Lock.Lock();

	// chain the buffers
	for (UINT i=0; i<NumOfDescriptors; i++, PhysAddrLow += sizeof(D))
	{
		PNDIS_PACKET p = KNdisReceiveArea::Chain( PUCHAR(pHwDesc+i) + dwBufOffset, dwBufLen);
		if(!p)
		{
			Desc_t* pDesc = m_AllocatedList.RemoveTail();
			if (pDesc)
			{
				m_AsyncMgr.RemoveFailedDescriptor();
				m_SwDesc.free(pDesc);
				m_nDescriptors--;
			}
			Status = STATUS_INSUFFICIENT_RESOURCES;
			continue;
		}
		Desc_t* pSwDesc = *reinterpret_cast<Desc_t**>(p->MiniportReserved);
	   	pSwDesc->pHwDesc = pHwDesc+i;
		pSwDesc->HwDescPhys.HighPart = PhysAddrHigh;
		pSwDesc->HwDescPhys.LowPart = PhysAddrLow;

        NDIS_SET_PACKET_HEADER_SIZE(p, KNdisAdapterTraits<A>::HeaderSize());

		// use the "Manipulator" to further initialize the Hw descriptors
		m_Adapter.DescriptorInitialize(pHwDesc+i, &pSwDesc->HwDescPhys);
	}

	m_Lock.Unlock();
	return Status;
}


///////////////////////////////////////////////////////////////////////
// Report allocated shared memory to the receive area
//
// Parameters:
// 	ShMem	- KNdisSharedMemory object which descibes the shared memory
// Returns:
//	TRUE if the Receive Area accepted the memory and initialized s/w
//		descriptors, FALSE if there was an error initializing descriptors
// Comments:
//	This method is called by drivers supporting asynchronous shared
//	memory allocations.  Called from the adapter class' AllocateComplete()
//	handler.  The Receive Area allocator previously called
//	NdisAllocateSharedMemoryAsync.
//  Client AllocateComplete() should continue call KNdisSharedReceiveArea::AddDescriptors
//  on TRUE return of this handler. Otherwise it should delete this KNdisSharedMemory.
template<class A, typename D, class Async>
BOOLEAN
KNdisSharedReceiveArea<A, D, Async>::OnAllocated(KNdisSharedMemory& ShMem)
{
	UINT NumOfDescriptors = ShMem.Length()/sizeof(D);
	Desc_t* p = (Desc_t*) m_AsyncMgr.AllocateComplete(&ShMem, NumOfDescriptors);

	if( p )
	{
		// link just allocated available descriptors into descriptor 'allocated' list (in base class)
		for (UINT i=0; i<NumOfDescriptors; i++)
		{
			m_AllocatedList.InsertTail((Desc_t*)p+i);
		}
		m_nDescriptors += NumOfDescriptors;
		return TRUE;
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////
// Next receive is completed.
//
// Parameters:
// 	pHwDesc		- Hardware descriptor being completed
// Returns:
// 	PNDIS_PACKET taken off the head of the free list, NULL if empty list
// Comments:
// 	Sets the status of the packet to NDIS_STATUS_RESOURCES if a low
//	resource condition has been reached.  If async memory support is
//	required, reports use of the memory to the Allocator class.
//	Usually called by the client from the RxDPC.
template<class A, typename D, class Async>
PNDIS_PACKET
KNdisSharedReceiveArea<A, D, Async>::Complete(D* pHwDesc)
{
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

	// Report use of this memory, check return status for low resource condition
	if( TRUE == m_AsyncMgr.Debit(sizeof(D)))
	{
		// Debit returned TRUE so an allocation is pending.
		Status = NDIS_STATUS_RESOURCES;
	}

	m_Lock.Lock();
	KNdisPacket Pkt = KNdisReceiveArea::Complete(pHwDesc);
	m_Lock.Unlock();

	// use the "Manipulator" to further adjust the Hw descriptors as necessary for complete operation
	m_Adapter.DescriptorComplete(reinterpret_cast<D*>(GetDescriptor(Pkt)), GetPhysicalAddress(Pkt));

	// Check for minimum available RFD's
	m_Lock.Lock();
	if( m_FreeList.Count() < m_nMinFreeDescriptors )
	{
		// Reached the minimum RFD level, must report low resource condition
		Status = NDIS_STATUS_RESOURCES;
	}
	m_Lock.Unlock();

	Pkt.STATUS(Status);
	return Pkt;
}


///////////////////////////////////////////////////////////////////////
// Packet previously removed by Complete() gets back
//
// Parameters:
// 	pkt				- PNDIS_PACKET to put back on the free list
//  bFreeAsyncMem	- Free async memory even if it's under HighWaterMark
// Returns:
// 	none
// Comments:
//	If async memory support is required, reports use of the memory
//	to the Allocator class.  Usually called by the client from the
//	ReturnPackets handler.
//	Can't be called while holding a spin lock.
template<class A, typename D, class Async>
void
KNdisSharedReceiveArea<A, D, Async>::Reclaim(PNDIS_PACKET pkt, BOOLEAN bFreeAsyncMem=FALSE)
{
	Desc_t* pSwDesc = *reinterpret_cast<Desc_t**>(pkt->MiniportReserved);
	BOOLEAN bAyncMgr = m_AsyncMgr.Credit(sizeof(D));
	if( pSwDesc->pHwDescMem && (bAyncMgr || bFreeAsyncMem) )
	{
		//A deallocation is possible, do something!

		// Unchain the packet and call the "Manipulator" to further
		// handle the invalidation of this Hw descriptor
		// Can't call this while holding a spin lock
		D* pHwDesc = reinterpret_cast<D*>(Unchain(pkt));
		m_Adapter.DescriptorInvalidate(pHwDesc);

		m_AsyncMgr.Remove(pkt);
		m_nDescriptors--;
		return;
	}

	_Reclaim(pkt);
}


///////////////////////////////////////////////////////////////////////
// Packet previously removed by Complete() gets back
//
// Parameters:
// 	pkt		- PNDIS_PACKET to put back on the free list
// Returns:
// 	none
// Comments:
//	Internal method used by Reclaim and ReclaimFailedPacket methods.
template<class A, typename D, class Async>
void
KNdisSharedReceiveArea<A, D, Async>::_Reclaim(PNDIS_PACKET pkt)
{
	m_Lock.Lock();

	// No deallocation possible || This packet is not in the shared memory that is to be freed,
	//	so reclaim this packet back into our receive area.
	// use the "Manipulator" to further adjust the Hw descriptors as necessary for reclaim operation
	m_Adapter.DescriptorReclaim(reinterpret_cast<D*>(GetDescriptor(pkt)), GetPhysicalAddress(pkt));

	KNdisReceiveArea::Reclaim(pkt);
	m_Lock.Unlock();
}


///////////////////////////////////////////////////////////////////////
// Packet previously removed by Complete() gets back
//
// Parameters:
// 	pkt		- PNDIS_PACKET to put back on the free list
// Returns:
// 	none
// Comments:
//	Usually called by the client from the RxDPC to reclaim
//	failed packets.
template<class A, typename D, class Async>
void
KNdisSharedReceiveArea<A, D, Async>::ReclaimFailedPacket(PNDIS_PACKET pkt)
{
	m_AsyncMgr.Credit(sizeof(D));

	_Reclaim(pkt);
}

#endif // !defined(AFX_KNDISSHAREDRECEIVEAREA_H__3A4EA9D3_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_)
