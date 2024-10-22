// cpprt.h - C++ definitions for kernel mode
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __CPPRT__
#define __CPPRT__

#include <..\..\common\include\stcinit.h>

// The checked build of NT supports pool tagging. Pool tagging associates
// a 32-bit tag with each allocated block. By default, operator new does
// tagged allocations for debug builds. The following line declares the
// default pool tag, which the driver can override by calling SetPoolTag.

typedef ULONG POOLTAG;

extern POOLTAG DefaultPoolTag;

// Prototype for function that sets the pool tag

inline void SetPoolTag(POOLTAG tag)
{
	DefaultPoolTag = tag;
}

////////////////////////////////////////////////////////////////////////
// New and delete operator overrides
//
// NT/WDM allows specification of a particular pool for memory
// allocation. The following declarations enable a driver to
// use conventional 'new' syntax, defaulting the allocation to
// the NonPagedPool. Alternatively, the driver can provide a
// parameter that specifies the pool in the invocation of new, e.g.
//
//		CHAR* p = new (PagedPool) CHAR[4096];

inline void * __cdecl operator new(size_t nSize)
{
#if DBG
	return nSize ? ExAllocatePoolWithTag(NonPagedPool, nSize, DefaultPoolTag) : NULL;
#else
	return nSize ? ExAllocatePool(NonPagedPool, nSize) : NULL;
#endif
}

inline void * __cdecl operator new(size_t nSize, POOL_TYPE iType)
{
#if DBG
	return nSize ? ExAllocatePoolWithTag(iType, nSize, DefaultPoolTag) : NULL;
#else
	return nSize ? ExAllocatePool(iType, nSize) : NULL;
#endif
}

////////////////////////////////////////////////////////////
//Added for C++ exception handling support
inline void __cdecl operator delete(void* p, POOL_TYPE iType)
{
	if (p) ExFreePool(p);
	UNREFERENCED_PARAMETER(iType);
}
//////////////////////////////////////////////////////////////

inline void * __cdecl operator new(size_t nSize, POOL_TYPE iType, ULONG tag)
{
#if DBG
	return nSize ? ExAllocatePoolWithTag(iType, nSize, tag) : NULL;
#else
	UNREFERENCED_PARAMETER(tag);
	return nSize ? ExAllocatePool(iType, nSize) : NULL;
#endif
}

inline void __cdecl operator delete(void* p)
{
	if (p) ExFreePool(p);
}

inline void __cdecl operator delete [] (void* p)
{
	if (p) ExFreePool(p);
}

#endif	// __CPPRT__
