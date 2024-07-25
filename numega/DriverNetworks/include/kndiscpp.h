// kndiscpp.h	--	Stuff needed for C++ run-time support for NDIS drivers
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

#ifndef __KNDISCPP_H__
#define __KNDISCPP_H__

//Include common C++ runtime definitions
#include <..\..\common\include\stcinit.h>

/////////////////////////////////////////////////////////////////////////
// Forms of new() used in DriverWorks/NDIS WDM builds.
// They simply delegate to the DriverNetworks non-paged allocator.
#if NDIS_WDM
inline void * __cdecl operator new(size_t nSize, POOL_TYPE iType)
{
	return operator new(nSize);
	UNREFERENCED_PARAMETER(iType);
}

inline void * __cdecl operator new(size_t nSize, POOL_TYPE iType, ULONG tag)
{
	return operator new(nSize);
	UNREFERENCED_PARAMETER(iType);
	UNREFERENCED_PARAMETER(tag);
}
#endif //NDIS_WDM

#endif // __KNDISCPP_H__
