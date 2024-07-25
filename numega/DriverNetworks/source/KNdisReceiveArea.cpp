// KNdisReceiveArea.cpp: implementation of the KNdisReceiveArea class.
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

#include "kndis.h"


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pAdapter				- Adapter object pointer (usually, 'this')
//	InitialNumOfDescriptors - Initial number of Receive Frame Descriptors(RFD)
//	MaximumNumOfDescriptors - Maximum number of Receive Frame Descriptors(RFD)
//	MinFreeDescriptors		- Min number of Receive Frame Descriptors(RFD) that 'must' be free
// Returns:
// 	none
// Comments:
//	Allocates Packet and Buffer Pools for MaximumNumOfDescriptors.
//	Allocates s/w descriptors (RFD) for InitialNumOfDescriptors.
//	Derived classes allocate h/w RFD's.
// 	Always use IsValid() to test for successful allocation
KNdisReceiveArea::KNdisReceiveArea(
	KNdisMiniAdapter* pAdapter,
	UINT InitialNumOfDescriptors,
	UINT MaximumNumOfDescriptors,
	UINT MinFreeDescriptors
	) :	m_pAdapter(pAdapter),
		m_nDescriptors(InitialNumOfDescriptors),
		m_nMaxDescriptors(MaximumNumOfDescriptors),
		m_nMinFreeDescriptors(MinFreeDescriptors),
		m_PktPool(MaximumNumOfDescriptors),
		m_BufPool(MaximumNumOfDescriptors)
{
	KNDIS_TRACE("KNdisReceiveArea::KNdisReceiveArea: %u descriptors\n", InitialNumOfDescriptors);
	KNDIS_TRACE("KNdisReceiveArea::KNdisReceiveArea: %u Max descriptors\n", MaximumNumOfDescriptors);
	Desc_t* pDesc;

	m_SwDesc.Initialize(0, ULONG('RfdP'));
	if(m_SwDesc.IsValid())
	{
		for (UINT i=0; i<InitialNumOfDescriptors; i++)
		{
			pDesc = (Desc_t*)m_SwDesc.alloc();
			NdisZeroMemory(pDesc, sizeof(Desc_t));
			m_AllocatedList.InsertTail(pDesc);
		}
	}

	if (!IsValid())
	{
		ASSERT(!"KNdisReceiveArea::KNdisReceiveArea");
		return;
	}
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Unchains the descriptors, frees the packets and buffers, and
//	frees memory for the s/w descriptors
KNdisReceiveArea::~KNdisReceiveArea()
{
	KNDIS_TRACE("KNdisReceiveArea::~KNdisReceiveArea: %u descriptors\n");

	// Unchain the descriptors, release packet/buffers
	while (Unchain()) {};

	// Make sure it's a graceful cleanup. This will assert when
	// NDIS failed to reclaim the passed buffer back to the driver
	ASSERT(m_AllocatedList.Count() == m_nDescriptors);

	Desc_t* pDesc;
	while(pDesc = m_AllocatedList.RemoveHead())
	{
		if(pDesc->pHwDescMem)
			delete pDesc->pHwDescMem;
		m_SwDesc.free(pDesc);
	}

	m_SwDesc.Invalidate();
}


///////////////////////////////////////////////////////////////////////
// Describe and chain the supplied chunk of buffer memory.
//
// Parameters:
// 	pChunk		- pointer to chunk of memory
//	ChunkSize	- size of memory pointed to by pChunk
// Returns:
// 	PNDIS_PACKET
// Comments:
// 	Allocates a packet from the packet pool.  Allocates a buffer from
//	the buffer pool.  Chains the buffer to the packet.  Initializes
//	the s/w descriptor with the packet and buffer.  Inserts the
//	s/w descriptor into the free list.
PNDIS_PACKET KNdisReceiveArea::Chain(PVOID pChunk, UINT ChunkSize)
{
	Desc_t* pDesc = m_AllocatedList.RemoveHead();
	ASSERT(pDesc);
	if (pDesc == NULL)
		return NULL;				// too many allocations?

	KNdisPacket pkt = m_PktPool.Allocate();
	if (!pkt.IsValid())
	{
		// most likely due to MaximumNumOfDescriptors
		KNDIS_TRACE("KNdisReceiveArea::m_PktPool Exceeded!\n");
		m_AllocatedList.InsertTail(pDesc);
		return NULL;
	}

	KNdisBuffer buf = m_BufPool.Allocate(pChunk, ChunkSize);
	if (!buf.IsValid())
	{
		ASSERT(!"buf");
		m_AllocatedList.InsertTail(pDesc);
		m_PktPool.Free(pkt);
		return NULL;
	}

	pkt.ChainAtFront(buf);

	// Init the descriptor and put it into free list
	pDesc->pPacket = pkt;
	pDesc->pBuffer = buf;
	pDesc->BufLength = ChunkSize;	// remember that as we'll need to adjust the buffer on Reclaim()
	pDesc->pHwDesc = NULL;			// derived class MUST take care of that...

	// and important: remember the back pointer
	* pkt.MiniportReserved() = (ULONG_PTR) pDesc;

	m_FreeList.InsertTail(pDesc);

	return PNDIS_PACKET(pkt);
}


///////////////////////////////////////////////////////////////////////
// Undoes Chain()
//
// Parameters:
// 	pChunk	- specific memory chunk to unchain
// Returns:
// 	h/w descriptor for packet that was unchained or NULL if no
//  packet unchained
// Comments:
// 	Can't be called while holding a spin lock
PVOID KNdisReceiveArea::Unchain(PVOID pChunk)
{
	// If pChunk is specified find and unchain that specific chunk.
	// Otherwise (pChunk==NULL) unchain the first in the free list.

	PVOID pHwDesc = NULL;
	Desc_t* pDesc = m_FreeList.Head();

	if (pChunk != NULL)
	{
		while (pDesc)
		{	// find the specific chunk
			KNdisBuffer buf(pDesc->pBuffer);
			if (buf.Address() == pChunk)
				break;
			else
				pDesc = m_FreeList.Next(pDesc);
		}
	}

	if (!pDesc)
		return pHwDesc;		// no descriptor found

	// release resources bound to pDesc
	ASSERT(pDesc->pPacket && pDesc->pBuffer);
	m_FreeList.Remove(pDesc);

	pHwDesc = Unchain(pDesc->pPacket);
	ASSERT(pHwDesc);

	m_AllocatedList.InsertTail(pDesc);

	return pHwDesc;
}


///////////////////////////////////////////////////////////////////////
// Unchains a single packet
//
// Parameters:
// 	pkt		- PNDIS_PACKET to unchain
// Returns:
// 	h/w descriptor for packet that was unchained or NULL if no
//  packet unchained
// Comments:
// 	Can't be called while holding a spin lock
PVOID KNdisReceiveArea::Unchain(PNDIS_PACKET pkt)
{
	PVOID pHwDesc = NULL;
	if (pkt)
	{
		pHwDesc = GetDescriptor(pkt);
		KNdisBuffer buf = KNdisPacket(pkt).UnChainAtFront();

		NdisReinitializePacket(pkt);
		m_PktPool.Free(pkt);
		m_BufPool.Free(buf);
	}
	return pHwDesc;
}


///////////////////////////////////////////////////////////////////////
// Next receive is completed.
//
// Parameters:
// 	pHwDesc		- Hardware descriptor being completed
// Returns:
// 	PNDIS_PACKET taken off the head of the free list, NULL if empty list
// Comments:
// 	If pHwDesc is NULL, the firts packet from m_FreeList is returned. Otherwise,
//  the search using pHwDesc as a key is used.
PNDIS_PACKET KNdisReceiveArea::Complete(PVOID pHwDesc)
{
	Desc_t* pDesc = m_FreeList.Head();
	if (pHwDesc)
	{
		while (pDesc)
		{
			if (pDesc->pHwDesc == pHwDesc)
			{
				m_FreeList.Remove(pDesc);
				break;
			}
			pDesc = m_FreeList.Next(pDesc);
		}
	}
	return (pDesc) ? pDesc->pPacket : NULL;
}

///////////////////////////////////////////////////////////////////////
// Get packet by h/w descriptor
//
// Parameters:
// 	pHwDesc		- Hardware descriptor associated with the packet
// Returns:
// 	Returns the packet associated with descriptor
// Comments:
// 	If pHwDesc is NULL, the FIFO order from m_FreeList is used. Otherwise,
//  the search using pHwDesc as a key is used.
PNDIS_PACKET KNdisReceiveArea::GetPacket(PVOID pHwDesc)
{
	Desc_t* pDesc;
	if (pHwDesc)
	{	// search
		pDesc = m_FreeList.Head();
		while (pDesc)
		{
			if (pDesc->pHwDesc == pHwDesc)
			{
				break;
			}
			pDesc = m_FreeList.Next(pDesc);
		}
	}
	else
		pDesc = m_FreeList.Head();

	return (pDesc) ? pDesc->pPacket : NULL;
}


///////////////////////////////////////////////////////////////////////
// Packet previously removed by Complete() gets back
//
// Parameters:
// 	pkt		- PNDIS_PACKET to put back on the free list
// Returns:
// 	none
// Comments:
// 	none
void KNdisReceiveArea::Reclaim(PNDIS_PACKET pkt)
{
	Desc_t* pDesc = * ((Desc_t**) pkt->MiniportReserved);
	// make sure it's that packet; programming eror otherwise
	ASSERT(pDesc && pDesc->pPacket==pkt);

	// Restore the buffer length
	KNdisBuffer buf = KNdisPacket(pkt).QueryFirstBuffer();
	buf.AdjustLength(pDesc->BufLength);

	m_FreeList.InsertTail(pDesc);
}
