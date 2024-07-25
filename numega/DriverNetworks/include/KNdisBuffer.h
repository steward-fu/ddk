// KNdisBuffer.h: interface for the KNdisBuffer class.
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

#if !defined(AFX_KNDISBUFFER_H__A31A8705_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISBUFFER_H__A31A8705_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

///////////////////////////////////////////////////////////////////////
// KNdisBuffer
//
// Abstracts NDIS buffer object. KNdisBuffer wraps PNDIS_BUFFER type
// and acts simply as a "smart" pointer to NDIS_BUFFER.
//
// In NDIS, NDIS buffers are the fundamendal objects that describe
// chunks of mapped memory. In NT, NDIS_BUFFER is an alias to MDL.
// NDIS buffers are always allocated from an NDIS pool. In KNDIS, one
// uses KNdisBufferPool::Allocate(), which works as an assignment to
// an (uninitialized) KNdisBuffer object.
//
// The supplied conversions allow KNdisBuffer objects be used anywhere
// a PNDIS_BUFFER pointer can.
//
class KNdisBuffer
{
	SAFE_DESTRUCTORS
public:
	// Construction
	inline KNdisBuffer();
	inline KNdisBuffer(PNDIS_BUFFER pBuf);

	// getting to next
	inline KNdisBuffer GetNext() const;

	// Querying address and length
	inline VOID Query(OUT PUINT Length, OUT PVOID *VirtualAddress = NULL, UINT Pri=0);

	// Querying page offset and length:
    inline VOID QueryOffset(OUT PUINT Offset, OUT PUINT Length);

	// Querying length in pages:
    inline ULONG QuerySpanPages();

	// Querying length in discontinouos page chunks
	inline UINT QueryPhysicalArraySize();

	// Adjusting the length
	inline VOID AdjustLength(IN UINT Length);

	inline void Flush(BOOLEAN WriteToDevice=TRUE);

	// validity
	inline BOOLEAN IsValid() const;

	//** TO DO (where this belongs to)
	//** NdisMStartBufferPhysicalMapping() -> DMA support

	// Helpers
	inline PVOID Address() ;
	inline UINT Length() ;

	// Perform Assignment from another KNdisBuffer
	inline KNdisBuffer& operator =(const KNdisBuffer& buf) {
		m_pBuf = PNDIS_BUFFER(buf); return *this;
	}

	// Convert to PNDIS_BUFFER.
	// Allows to use KNdisBuffer type wherever PNDIS_BUFFER can be used
	inline operator PNDIS_BUFFER () const {return m_pBuf;}

	// Assign from buffer pointer
	inline KNdisBuffer& operator =(const PNDIS_BUFFER buf) { m_pBuf = buf; return *this;}
protected:

	// let those guys to access guts
	friend class KNdisPacket;
	friend class KNdisBufferPool;

	PNDIS_BUFFER	m_pBuf;		// wrapped ptr
};

///////////////////////////////////////////////////////////////////////
// Implementations


///////////////////////////////////////////////////////////////////////
// Constructs an uninitialized KNdisBuffer object
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisBuffer::KNdisBuffer()
{ m_pBuf=NULL; }


///////////////////////////////////////////////////////////////////////
// Constructs a KNdisBuffer from PNDIS_BUFFER
//
// Parameters:
// 	pBuf                 - NDIS buffer pointer to construct from
// Returns:
// 	none
// Comments:
// 	none
KNdisBuffer::KNdisBuffer(PNDIS_BUFFER pBuf)
{ m_pBuf=pBuf; }


///////////////////////////////////////////////////////////////////////
// Get next buffer.
//
// Parameters:
// 	none
// Returns::
// 	KNdisBuffer following this buffer in the buffer chain (if any)
// Comments:
// 	Always use IsValid() to check the result since this could be the last
//  buffer in the chain.
KNdisBuffer KNdisBuffer::GetNext() const
{
   KNdisBuffer  buf;
   if (IsValid())
	  NdisGetNextBuffer(m_pBuf, &buf.m_pBuf);
  return buf;
}

///////////////////////////////////////////////////////////////////////
// Get length and virtual address of the buffer range
//
// Parameters:
// 	Length			-	Pointer to UINT to receive the length of the range
// 	VirtualAddress	-	Pointer to PVOID to receive the base virtual address
// 	Pri				-	Priority of the query (NDIS 5 only).
// Returns:
// 	none
// Comments:
// 	The 'Pri' parameter defines the behaviour of the function under low resource
//  conditions. In Windows 2000, the function might fail (return 0s) if the system
//  resources are exhausted; in NT, it will bugcheck.
//  NOTE NdisQueryBufferSafe() is not avail under W9X.
VOID KNdisBuffer::Query(OUT PUINT Length, OUT PVOID *VirtualAddress, UINT Pri)
{
#if	(KNDIS_DDK_VERSION >= 5) &&	(!KNDIS_W9X_COMPATIBLE)
 #if ((BINARY_COMPATIBLE) && (KNDIS_DDK_VERSION == 5))
		NdisQueryBufferSafe(m_pBuf, VirtualAddress, Length, Pri);
 #else
		NdisQueryBufferSafe(m_pBuf, VirtualAddress, Length, (MM_PAGE_PRIORITY)Pri);	// workaround type mismatch in ddk
 #endif
#else
		NdisQueryBuffer(m_pBuf, VirtualAddress, Length); // obsolete in w2k
		UNREFERENCED_PARAMETER(Pri);
#endif
}

///////////////////////////////////////////////////////////////////////
// Get offset and length
//
// Parameters:
// 	Offset			-	Pointer to UINT to receive the offset value
// 	Length			-	Pointer to UINT to receive the length value
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisBuffer::QueryOffset(OUT PUINT Offset, OUT PUINT Length)
{
	NdisQueryBufferOffset(m_pBuf, Offset, Length);
}

///////////////////////////////////////////////////////////////////////
// Get number of physical pages
//
// Parameters:
// 	none
// Returns:
// 	Number of physical pages of memory used to back the buffer
// Comments:
// 	none
#if !defined(VTOOLSD)
ULONG KNdisBuffer::QuerySpanPages() { return NDIS_BUFFER_TO_SPAN_PAGES(m_pBuf); }
#endif

///////////////////////////////////////////////////////////////////////
// Get number of physical blocks
//
// Parameters:
// 	none
// Returns:
// 	Number of discontiguous physical blocks to back the buffer
// Comments:
// 	none
UINT KNdisBuffer::QueryPhysicalArraySize()
{ UINT u;
  NdisGetBufferPhysicalArraySize(m_pBuf, &u); return u;
}

///////////////////////////////////////////////////////////////////////
// Modify length
//
// Parameters:
// 	Length               - new length in bytes
// Returns:
// 	none
// Comments:
// 	Modifies the length of the buffer specified in the buffer descriptor.
//  The length cannot be larger than the mapped length specified when the
//	buffer was first allocated from a (KNdisBufferPool) pool.
#if !defined(VTOOLSD)
VOID KNdisBuffer::AdjustLength(IN UINT Length)
{
   NdisAdjustBufferLength(m_pBuf,Length);
}
#endif

#pragma warning (disable:4100)	// non-ref formals
///////////////////////////////////////////////////////////////////////
// Flush the memory region described by a given buffer descriptor from
// all processor caches.
//
// Parameters:
// 	WriteToDevice        - TRUE if the NIC driver calls this function to flush a buffer before
//						   making a transfer from host memory to the NIC (default)
// Returns:
// 	none
// Comments:
//	If the NIC driver specifies FALSE, it must ensure the buffer begins and ends on
//	a cache-line boundary.
void KNdisBuffer::Flush(BOOLEAN WriteToDevice)
{
	NdisFlushBuffer(m_pBuf, WriteToDevice); // NOTE: this is NOP under _X86_
}
#pragma warning (default:4100)	// non-ref formals


///////////////////////////////////////////////////////////////////////
// Test if the object was properly initialized
//
// Parameters:
// 	none
// Returns:
// 	TRUE if the object wraps an actual underlying NDIS buffer
// Comments:
// 	Returning FALSE indicates that no buffer is associated with the object.
BOOLEAN KNdisBuffer::IsValid() const
{ return KNDIS_GOOD_PTR(m_pBuf);
}

///////////////////////////////////////////////////////////////////////
// Get virtual address
//
// Parameters:
// 	none
// Returns:
// 	Virtual address of the first byte of the buffer, or NULL if and error
//  has occured.
// Comments:
// 	none
PVOID KNdisBuffer::Address()
{  PVOID va; UINT len; Query(&len, &va); return va; }

///////////////////////////////////////////////////////////////////////
// Get length
//
// Parameters:
// 	none
// Returns:
// 	The length of the underlying buffer in bytes
// Comments:
//
UINT KNdisBuffer::Length()
{  UINT len; Query(&len); return len; }

#endif // !defined(AFX_KNDISBUFFER_H__A31A8705_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_)
