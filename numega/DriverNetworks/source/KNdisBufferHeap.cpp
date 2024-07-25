// KNdisBufferHeap.cpp: implementation of the KNdisBufferHeap class.
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

#include <kndis.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KNdisBufferHeap::KNdisBufferHeap() : KNdisBufferPool()
{
}

KNdisBufferHeap::KNdisBufferHeap(UINT Units, size_t UnitSize, UINT Tag)
{
	Initialize(USHORT(Units), UnitSize, Tag);
}

KNdisBufferHeap::~KNdisBufferHeap()
{
	Invalidate();
}


NTSTATUS
KNdisBufferHeap::Initialize(UINT Units, size_t UnitSize, UINT Tag)
{
	NDIS_STATUS Status;
	if ( (Status = KNdisBufferPool::Initialize(Units)) == NDIS_STATUS_SUCCESS)
		m_Heap.Initialize(USHORT(Units), UnitSize, Tag);
	return Status;
}

void KNdisBufferHeap::Invalidate()
{
	ASSERT(KNdisBufferPool::Usage()==0);	// make sure the buffers are back
	if (KNdisBufferPool::IsValid()) {
		KNdisBufferPool::Invalidate();
		m_Heap.Invalidate();
	}
}

PNDIS_BUFFER KNdisBufferHeap::Allocate()
{
	PNDIS_BUFFER buf;
	PVOID addr = m_Heap.alloc();
	if (addr) {
		buf = KNdisBufferPool::Allocate(addr, (UINT)m_Heap.ItemSize());
		if (buf)
			return buf;
		else
			m_Heap.free(addr);
	}
	// resources
	return NULL;
}

void KNdisBufferHeap::Free(PNDIS_BUFFER Buffer)
{
	PVOID addr; UINT len;
	KNdisBuffer buf(Buffer);
	buf.Query(&len, &addr);
	ASSERT(addr && len == m_Heap.ItemSize()); // make sure we return the correct buffer
	m_Heap.free(addr);
	KNdisBufferPool::Free(Buffer);		// Now recycle the buffer descriptor
}
