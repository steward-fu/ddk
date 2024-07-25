// KNdisReceiveArea.h: interface for the KNdisReceiveArea class.
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

#if !defined(AFX_KNDISRECEIVEAREA_H__971CD447_4E64_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISRECEIVEAREA_H__971CD447_4E64_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

/////////////////////////////////////////////////////////////////////////////
// KNdisReceiveArea
//
// Abstracts the mechanism commonly used by NIC miniports to arrange receive
// frame processing.
//
// The class is designed on the assumption that the NIC has a specific descriptor
// format for a receive frame. The frame descriptors are arranged in an array that
// NIC's hardware uses to stream received frames from the network into. It is also assumed
// that the h/w processes the descriptors in a sequential fashion.
// Usually, the descriptors are placed in the shared memory accessed by the NIC.
// Each receive interrupt results in one or more descriptors getting "ready" for
// indicating the frame(s) to NDIS.
//
// KNdisReceiveArea class bonds together standard NDIS structures (packet, buffer) and h/w
// specific descriptors and maintains a list of "free descriptors".
//
// The user is required to provide template methods to initialize the h/w specific descriptors
// in his "adapter" class and call Complete()/Reclaim() in the appropriate time. Specifically:
// Whenever the h/w "completes" a h/w descriptor(s), Complete() is to be called to advance the
// engine and return the packet, which become ready. This packet can then be NdisMReceiveIndicate-d.
// When NDIS is done with the packet (e.g. on MiniportReturnPacket), the driver calls Reclaim() to
// return the packet (and appropriate descriptor) to the "free list".
//
// Use this class via its 2 derivations: KNdisSystemReceiveArea and KNdisSharedReceiveArea
// templatized with a descriptor structure.
//
//
class KNdisReceiveArea {

// Constructors/Destructors
public:
	SAFE_DESTRUCTORS;

	KNdisReceiveArea(
		KNdisMiniAdapter* pAdapter,
		UINT InitialNumOfDescriptors,
		UINT MaximumNumOfDescriptors,
		UINT MinFreeDescriptors
		);

	~KNdisReceiveArea();

// Methods
public:

	// Checking Status
	inline BOOLEAN	IsValid() const;

	// Describe and chain the supplied chunk of buffer memory.
	// Usually called by the derived class during initialization
	PNDIS_PACKET	Chain(PVOID pChunk, UINT ChunkSize);

	// Undoes the effect of Chain() for the chunk of memory
	// Usually called by the derived class during de-initialization
	PVOID			Unchain(PVOID pChunk=NULL);

	// Unchains one packet
	PVOID			Unchain(PNDIS_PACKET pkt);

	// Next receive is completed. Called on receive interrupt.
	// Returns next packet completed:
	PNDIS_PACKET	Complete(PVOID pHwDesc=NULL);

	// Packet previously removed by Complete() gets back
	void			Reclaim(PNDIS_PACKET pkt);

// Accessors
public:

	// Accessing current packet. This is the one at the head of free list
	PNDIS_PACKET GetPacket(PVOID pHwDesc=NULL);

	// Access number of descriptors
	inline UINT			GetNumberDescriptors() const;

	// Access maximum number of descriptors
	inline UINT			GetMaxNumberDescriptors() const;

	// Access number of 'FREE' descriptors
	inline UINT			GetNumberFreeDescriptors();

	// Getting h/w descriptor for the packet
	inline PVOID		GetDescriptor(PNDIS_PACKET pkt) const;

	// Accessing HW descriptor at the tail of the free list
	inline PVOID		GetTailDescriptor();

	// Accessing HW descriptor at the head of the free list
	inline PVOID		GetHeadDescriptor();

	// Accessing physical address. This one is associated with a particular packet
	inline PNDIS_PHYSICAL_ADDRESS GetPhysicalAddress(PNDIS_PACKET pkt) const;

	// Accessing physical address. This is the one at the head of free list
	inline PNDIS_PHYSICAL_ADDRESS GetHeadPhysicalAddress();

	// Accessing physical address. This is the one at the tail of free list
	inline PNDIS_PHYSICAL_ADDRESS GetTailPhysicalAddress();

public:
	// Internal s/w descriptor for each chunk
	typedef struct _Desc_t {
		LIST_ENTRY				m_ListEntry;
		PNDIS_PACKET			pPacket;		// Packet; contains back ptr to the Desc_t
		PNDIS_BUFFER			pBuffer;		// Buffer; chained to the pPacket
		UINT					BufLength;      // orig (max) size of the buffer
		volatile PVOID			pHwDesc;		// ptr to hardware descriptor
		NDIS_PHYSICAL_ADDRESS   HwDescPhys;     // physical address of HwDesc
		USHORT					Status;         // receive status (quick retrieval)
		KNdisSharedMemory*		pHwDescMem;		// used by KNdisRxAreaAsyncMgr to detete KNdisSharedMemory
	} Desc_t, *PDesc_t;

// Data
protected:

	KNdisPacketPool		m_PktPool;				// Packet Pool used for the object
	KNdisBufferPool		m_BufPool;				// Buffer Pool for chaining to packets

	KList<Desc_t>		m_AllocatedList;		// list of all allocated descriptors
	KList<Desc_t>		m_FreeList;				// list of chained descriptors (taken off m_AllocatedList)

	KNdisHeap<Desc_t>	m_SwDesc;				// descriptor area allocated in system memory
	UINT				m_nDescriptors;			// Current # of descriptors
	const UINT			m_nMinFreeDescriptors;  // Mininum # of descriptors that must be 'free'
	const UINT			m_nMaxDescriptors;		// Maximum # of descriptors

	KNdisMiniAdapter*	m_pAdapter;				// adapter

private:
	PREVENT_COPY_CONSTRUCTORS(KNdisReceiveArea);
};


///////////////////////////////////////////////////////////////////////
// Checking Status
//
// Parameters:
// 	none
// Returns:
// 	Returns the status of the object, TRUE = valid, FALSE = invalid
// Comments:
// 	none
BOOLEAN KNdisReceiveArea::IsValid() const
{
	return
		m_SwDesc.IsValid() &&
		m_PktPool.IsValid() &&
		m_BufPool.IsValid();
}


///////////////////////////////////////////////////////////////////////
// Access number of descriptors
//
// Parameters:
// 	none
// Returns:
// 	Current number of descriptors managed by the receive area
// Comments:
// 	none
UINT KNdisReceiveArea::GetNumberDescriptors() const
{
	return m_nDescriptors;
}


///////////////////////////////////////////////////////////////////////
// Access maximum number of descriptors
//
// Parameters:
// 	none
// Returns:
// 	Maxiumum amount of descriptors managed by the receive area
// Comments:
// 	none
UINT KNdisReceiveArea::GetMaxNumberDescriptors() const
{
	return m_nMaxDescriptors;
}


///////////////////////////////////////////////////////////////////////
// Access number of 'FREE' descriptors
//
// Parameters:
// 	none
// Returns:
// 	number of descriptors on the free list which the NIC can use
// Comments:
// 	none
UINT KNdisReceiveArea::GetNumberFreeDescriptors()
{
	return m_FreeList.Count();
}


///////////////////////////////////////////////////////////////////////
// Access h/w descriptor for the packet
//
// Parameters:
// 	pkt		- PNDIS_PACKET used to retrieve its h/w desc
// Returns:
// 	h/w descriptor
// Comments:
// 	The s/w descriptor is stored in the MiniportReserved field of the
//	packet.  The h/w descriptor is stored in the s/w descriptor
PVOID KNdisReceiveArea::GetDescriptor(PNDIS_PACKET pkt) const
{
	return (*(Desc_t**)pkt->MiniportReserved)->pHwDesc;
}


///////////////////////////////////////////////////////////////////////
// Access address of physical address associated with a particular packet
//
// Parameters:
// 	none
// Returns:
// 	PNDIS_PHYSICAL_ADDRESS for a particular packet
// Comments:
// 	none
PNDIS_PHYSICAL_ADDRESS KNdisReceiveArea::GetPhysicalAddress(PNDIS_PACKET pkt) const
{
	return &(*(Desc_t**)pkt->MiniportReserved)->HwDescPhys;
}


///////////////////////////////////////////////////////////////////////
// Access h/w descriptor at the tail of the free list
//
// Parameters:
// 	none
// Returns:
// 	h/w descriptor at the tail of the free list
// Comments:
// 	none
PVOID KNdisReceiveArea::GetTailDescriptor()
{
	Desc_t* d = m_FreeList.Tail();
	return (d) ? d->pHwDesc : NULL;
}


///////////////////////////////////////////////////////////////////////
// Access address of physical address at the tail of the free list
//
// Parameters:
// 	none
// Returns:
// 	PNDIS_PHYSICAL_ADDRESS at the tail of the free list
// Comments:
// 	none
PNDIS_PHYSICAL_ADDRESS KNdisReceiveArea::GetTailPhysicalAddress()
{
	Desc_t* d = m_FreeList.Tail();
	return (d) ? &d->HwDescPhys : NULL;
}


///////////////////////////////////////////////////////////////////////
// Access h/w descriptor at the head of the free list
//
// Parameters:
// 	none
// Returns:
// 	h/w descriptor at the head of the free list
// Comments:
// 	none
PVOID KNdisReceiveArea::GetHeadDescriptor()
{
	Desc_t* d = m_FreeList.Head();
	return (d) ? d->pHwDesc : NULL;
}


///////////////////////////////////////////////////////////////////////
// Access address of physical address at the head of the free list
//
// Parameters:
// 	none
// Returns:
// 	PNDIS_PHYSICAL_ADDRESS at the head of the free list
// Comments:
// 	none
PNDIS_PHYSICAL_ADDRESS KNdisReceiveArea::GetHeadPhysicalAddress()
{
	Desc_t* d = m_FreeList.Head();
	return (d) ? &d->HwDescPhys : NULL;
}

#endif // !defined(AFX_KNDISRECEIVEAREA_H__971CD447_4E64_11D3_8FA3_00C04F7445B7__INCLUDED_)
