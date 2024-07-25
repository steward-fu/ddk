// KNdisBufferPool.h: interface for the KNdisBufferPool class.
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

#if !defined(AFX_KNDISBUFFERPOOL_H__A31A8705_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISBUFFERPOOL_H__A31A8705_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

/////////////////////////////////////////////////////////////////////
// KNdisBufferPool
//
// Abstracts NDIS buffer pool object. KNdisBufferPool serves as a
// factory for "friendly" KNdisBuffer objects. Based on NDIS "pool"
// object (NDIS_HANDLE).
//
// Underlying NDIS services: NdisAllocateBufferPool, NdisAllocateBuffer.
//
class KNdisBufferPool : public KObject<NDIS_HANDLE, 'knbp'>
{
	SAFE_DESTRUCTORS
public:
	inline KNdisBufferPool();
	inline KNdisBufferPool(const UINT NumberOfDescriptors);
    inline ~KNdisBufferPool();

	inline NDIS_STATUS Initialize(const UINT NumberOfDescriptors);
	inline void Invalidate();

// Allocating
	inline KNdisBuffer Allocate(PVOID  VirtualAddress, UINT  Length);

// Freeing. Note the underlying NDIS service doesn't need 'this'
#if !DBG
	static
#endif
	inline void Free(PNDIS_BUFFER Buffer);

// Duplicating from the pool (based on NdisCopyBuffer)
	inline KNdisBuffer Copy(KNdisBuffer& buf, UINT Length, UINT Offset=0);
	inline KNdisBuffer Copy(KNdisBuffer& buf);

// Usage count. NOTE: for debug builds only
	inline UINT Usage();

#if DBG
	protected:  KNdisCounter m_nTaken;  // simulate usage count for DDK 4 checked
#endif
};

///////////////////////////////////////////////////////////////////////////
// Implementations




///////////////////////////////////////////////////////////////////////
// Constructs a buffer pool
//
// Parameters:
// 	NumberOfDescriptors  - The number of buffer descriptors to allocate for the pool.
// Returns:
// 	none
// Comments:
//	The construction (initialization) usually takes place during driver initialization.
// 	After construction, the driver should call IsValid() to check the result of
//	construction. Then the driver calls KNdisBufferPool::Allocate() as needed to
//	get the descriptors.
KNdisBufferPool::KNdisBufferPool(const UINT NumberOfDescriptors)
	: KObject<NDIS_HANDLE, 'knbp'>()
{
#if DBG
	m_nTaken=0;
#endif
	NDIS_STATUS Status = Initialize(NumberOfDescriptors);
	if (!NT_SUCCESS(Status)) {
		ASSERT(!"KNdisBufferPool::KNdisBufferPool");
		Invalidate();
	}
}


///////////////////////////////////////////////////////////////////////
// Constructs an unitialized buffer pool object
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
//	The construction usually takes place during driver initialization.
// 	This form of constructor requires a subsequent call to KNdisBufferPool::Initialize()
//	to actually allocate a NDIS buffer bool.
//	Then the driver calls KNdisBufferPool::Allocate() as needed to get the descriptors.
KNdisBufferPool::KNdisBufferPool()
	: KObject<NDIS_HANDLE, 'knbp'>()
{
#if DBG
	m_nTaken=0;
#endif

}

///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Before calling the destructor, the driver must call KNdisBufferPool::Free()
//  as many times as necessary to release all buffer descriptors allocated with
//  KNdisBufferPool::Allocate() but not yet released.
KNdisBufferPool::~KNdisBufferPool()
{
	ASSERT(Usage()==0 && "Not all buffers were freed");
	Invalidate();
}


///////////////////////////////////////////////////////////////////////
// Allocate NDIS buffer pool
//
// Parameters:
// 	NumberOfDescriptors  - The number of buffer descriptors to allocate for the pool.
// Returns:
// 	NDIS_STATUS_SUCCESS if the allocation succeeded; error code otherwise.
// Comments:
// 	Calls NdisAllocateBufferPool.
NDIS_STATUS
KNdisBufferPool::Initialize(const UINT NumberOfDescriptors)
{
	NDIS_STATUS Status;
	ASSERT(!IsValid() && "Initializing twice");
	NdisAllocateBufferPool(&Status, &m_Handle, NumberOfDescriptors);
#if DBG
	m_nTaken = 0;
#endif
	return Status;
}


///////////////////////////////////////////////////////////////////////
// Free the NDIS buffer pool
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Calls NdisFreeBufferPool and invalidates the underlying pool handle.
//	The object can be reused to Initialize() another pool.
void KNdisBufferPool::Invalidate()
{
	if (IsValid()) {
		NdisFreeBufferPool(m_Handle);
		KObject<NDIS_HANDLE, 'knbp'>::Invalidate();
	}
}

///////////////////////////////////////////////////////////////////////
// Allocate and map buffer descriptor
//
// Parameters:
// 	VirtualAddress       - the base virtual address of previously allocated
//						   system-space memory to be mapped in the returned buffer descriptor.
// 	Length               - the number of bytes to be mapped.
// Returns:
// 	New KNdisBuffer object mapped to the buffer area specified
// Comments:
// 	Use KNdisBuffer::IsValid() to check the result of allocation. Returning FALSE
//	indicates the "ran-out-of-buffer-descriptors" condition.
KNdisBuffer
KNdisBufferPool::Allocate(PVOID  VirtualAddress, UINT  Length)
{
	NDIS_STATUS Status;
	KNdisBuffer Buf;
   	NdisAllocateBuffer(&Status, &Buf.m_pBuf, m_Handle, VirtualAddress, Length);
    if (Status == NDIS_STATUS_SUCCESS) {
#if DBG
		++m_nTaken;
#endif
	}
    return Buf;
}

///////////////////////////////////////////////////////////////////////
// Free the buffer descriptor
//
// Parameters:
// 	Buffer               - Buffer previusly allocated with KNdisBufferPool::Allocate
// Returns:
// 	none
// Comments:
// 	none
void
KNdisBufferPool::Free(PNDIS_BUFFER  Buffer)
{
	NdisFreeBuffer(Buffer);
#if DBG
	--m_nTaken;
#endif
}

///////////////////////////////////////////////////////////////////////
// Duplicate (map a subrange of) buffer
//
// Parameters:
// 	FromBuf              - Source buffer descriptor
// 	Length               - The length of the block in bytes
// 	Offset               - The byte offset within the memory block at which to start the copy
// Returns:
// 	none
// Comments:
//  KNdisBufferPool::Copy creates a KNdisBuffer mapping a (sub)range within a
//  block of memory, described by the source FromBuf descriptor.
//	The method could be used to map inner protocol fields within a memory
//	buffer containing a packet payload.
// 	Based on NdisCopyBuffer.
KNdisBuffer
KNdisBufferPool::Copy(KNdisBuffer& FromBuf, UINT Length, UINT Offset)
{
	NDIS_STATUS Status;
	KNdisBuffer Buf;
	NdisCopyBuffer(
       &Status,
       &Buf.m_pBuf,
       m_Handle,
       PNDIS_BUFFER(FromBuf),	// src buffer
       Offset,
       Length
       );
    if (Status == NDIS_STATUS_SUCCESS) {
#if DBG
		++m_nTaken;
#endif
	}
    return Buf;
}

///////////////////////////////////////////////////////////////////////
// Duplicate (map) the entire buffer
//
// Parameters:
// 	FromBuf              - Source buffer descriptor
// Returns:
// 	none
// Comments:
//  KNdisBufferPool::Copy creates a KNdisBuffer mapping a (sub)range within a
//  block of memory, described by the source FromBuf descriptor.
// 	Based on NdisCopyBuffer.
KNdisBuffer
KNdisBufferPool::Copy(KNdisBuffer& buf)
{
	return Copy(buf, buf.Length(), 0);
}

///////////////////////////////////////////////////////////////////////
// Get usage count (debug)
//
// Parameters:
// 	none
// Returns:
// 	Number of descriptors used
// Comments:
// 	Supported in debug non-SMP builds only. In free builds, always returs 0.
UINT
KNdisBufferPool::Usage()
{
#if DBG
	return (UINT) ULONG(m_nTaken);
#else
//#pragma message("WARNING: KNdisBufferPool::Usage() not supported for Free builds")
	return 0;
#endif // DBG
}

#endif // !defined(AFX_KNDISBUFFERPOOL_H__A31A8705_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_)
