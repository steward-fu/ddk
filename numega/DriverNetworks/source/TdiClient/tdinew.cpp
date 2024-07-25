// tdinew.cpp   Implementation of global new().
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
//
// This file could've been included into the library and used
// whenever the global parameterless new(size_t) is NOT defined
// in the client's framework.
//
// The policy, however, is to borrow the global, non-paged pool
// allocator from the framework the TDI library is used with. Namely,
//
// Framework	Platform	new()/delete() provider		Typical Driver
// =========	=========	=======================		==============
// KNDIS		NT/w2k/9X	KNDIS/NdisAllocate			Virtual NIC Miniport (e.g., VPN)
// VtoolsD		9X			VtoolsD/HeapAllocate		IP-enabled .VxD
// DriverWorks	NT/w2k		DW/ExAllocatePool			IP-enabled .SYS
// DDK only		NT/w2k/9X	Must be provided by client
//

#include    "tdiclient.h"

#if 0 // that's how they should've been implemented in the "DDK only" case
void * __cdecl operator new(size_t nSize)
{
    return ExAllocatePool(NonPagedPool, nSize);
}

void __cdecl operator delete(void* p)
{
	if (p) ExFreePool(p);
}

#endif // 0
