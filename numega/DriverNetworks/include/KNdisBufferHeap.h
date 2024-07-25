// KNdisBufferHeap.h: interface for the KNdisBufferHeap class.
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

#if !defined(AFX_KNDISBUFFERHEAP_H__198D9DDA_3CE8_49BB_88B9_9608E927DE17__INCLUDED_)
#define AFX_KNDISBUFFERHEAP_H__198D9DDA_3CE8_49BB_88B9_9608E927DE17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////
// KNdisBufferHeap
//
// Provides a pool of fixed size data buffers described as NDIS_BUFFERs.
// The buffers are typically used as (part of) TPDUs used by transport
// drivers.
//
// Can be used by NDIS protocol drivers to conveniently allocate
// transport protocol data units for (TPDU) transfer over bindings.
//
// Implemented using lookaside lists.
//
class KNdisBufferHeap : public KNdisBufferPool {
public:
	KNdisBufferHeap();
	KNdisBufferHeap(UINT Units, size_t UnitSize, UINT Tag='hbnk');
	~KNdisBufferHeap();

	NTSTATUS Initialize(UINT Units, size_t UnitSize, UINT Tag='hbnk');
	void Invalidate();

	PNDIS_BUFFER Allocate();
	void Free(PNDIS_BUFFER Buffer);

	BOOLEAN IsValid() { return KNdisBufferPool::IsValid(); }

protected:
	KNdisHeapFast m_Heap;
};

#endif // !defined(AFX_KNDISBUFFERHEAP_H__198D9DDA_3CE8_49BB_88B9_9608E927DE17__INCLUDED_)
