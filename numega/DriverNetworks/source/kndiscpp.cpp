// kndiscpp.cpp	--	C++ run-time support for KNdis
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

/////////////////////////////////////////////////////////////////////////
// Global new() and delete()
//
// As per recent MS recommendation
// (http://support.microsoft.com/support/kb/articles/Q266/4/03.ASP)
// NDIS drivers for XP should not use NdisAllocateMemory, but
// use NdisAllocateMemoryWithTag instead.
//
// The new() uses default tag MemTag ('dnw_') defined on namespace _KNDIS.
// The user can override it by direct assignment in its DriverEntry().
//
// For Win64 builds memory is always allocated at 16-byte boundary to
// avoid allignment problems with things like SLISTS, etc.
//
//
namespace _KNDIS {
	typedef struct {
		size_t	length;
#if defined(_WIN64)
		// gap followed by ptr to the beginning of the struct
		PVOID	pblock;
#endif
	} _memblock, *p_memblock;
	static const NDIS_PHYSICAL_ADDRESS
		HighestAcceptableAddress = NDIS_PHYSICAL_ADDRESS_CONST(-1,-1);

	ULONG MemTag = '_wnd';		// default memtag for all allocations
#if defined(_WIN64)
	size_t MemAlignment = 16;	// default alignment for IA64 allocations. Must be power of 2 >= sizeof(ULONG_PTR)
#endif
};

void * _cdecl operator new(size_t length)
{
	_KNDIS::p_memblock p;
#if !defined(_WIN64)
	length += sizeof(_KNDIS::_memblock);
#else
	size_t align = _KNDIS::MemAlignment;
	length += (sizeof(_KNDIS::_memblock) + align);
	align--;
#endif
	NDIS_STATUS Status =
#if (KNDIS_DDK_VERSION < 5) || defined(KNDIS_ALLOCMEM_NOTAGS)
		// Use for NT4 DDK/W95 miniports
		NdisAllocateMemory((PVOID*)&p, (UINT)length, 0, _KNDIS::HighestAcceptableAddress);
#else
		// WDM and higher
		NdisAllocateMemoryWithTag((PVOID*)&p, (UINT)length, _KNDIS::MemTag);
#endif
	if (Status != NDIS_STATUS_SUCCESS)
    {
    	// ASSERT(!"new"); // let the Verifier inject errors
    	return NULL;
    }

#if !defined(_WIN64)
    p->length = length;
	return (PVOID) (p+1);
#else
	_KNDIS::_memblock* pa = (_KNDIS::_memblock*) (( ULONG_PTR(p + 1) + align) & ~align);
	_KNDIS::_memblock* pb = pa-1;
	pb->length = length;
	pb->pblock = p;
	return (PVOID) pa;
#endif
}

void _cdecl operator delete(void * p)
{
	if (p) {
   		p = _KNDIS::p_memblock(p)-1;
#if !defined(_WIN64)
   		NdisFreeMemory(p, (UINT)_KNDIS::p_memblock(p)->length, 0);
#else
   		NdisFreeMemory(_KNDIS::p_memblock(p)->pblock, (UINT)_KNDIS::p_memblock(p)->length, 0);
#endif
	}
}
