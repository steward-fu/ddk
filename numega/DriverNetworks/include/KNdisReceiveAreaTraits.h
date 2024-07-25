// KNdisReceiveAreaTraits.h: interface for the KNdisReceiveAreaTraits class.
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

#if !defined(AFX_KNDISRECEIVEAREATRAITS_H__3A4EA9D8_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_)
#define AFX_KNDISRECEIVEAREATRAITS_H__3A4EA9D8_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// KNdisReceiveAreaTraits
//
// Static struct to define the Shared Receive Area Memory Allocation Traits.
//
// KNdisSharedReceiveArea Asnyc Allocator classes use this object to provide
// number of allocations, allocation sizes, and whether the memory is cached.
// The Asnyc Allocator classes require the following signatures from this struct:
//		BOOLEAN Cached(void)
//			returns whether the memory should be cached or not
//		UINT GetAllocSize()
//			returns an allocation size in the form of number of bytes with
//			KNdisHeap lookaside list allocator.
//
// The 1st template parameter is the adapter class.
// The 2nd template parameter is the Hardware RFD structure (client defined).
//
// To specialize behavior, the user can specialize the GetAllocSize(UINT) and
// Cached() methods and the UnitSize data member.
template <class A, typename D>
struct KNdisReceiveAreaTraits {
	static BOOLEAN Cached() { return FALSE; }

	static UINT GetAllocSize();
};

template <class A, typename D>
UINT KNdisReceiveAreaTraits<A, D>::GetAllocSize()
{
	return sizeof(D);
}

#endif // !defined(AFX_KNDISRECEIVEAREATRAITS_H__3A4EA9D8_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_)
