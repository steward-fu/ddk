// kstl.cpp
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

#if (_MSC_VER >= 1200)	// STL support available in MSVC 6.xx and higher

#ifndef NDIS_MINIPORT_DRIVER

extern "C" {
#if ( NTVERSION=='WDM' )
#include <wdm.h>
#else
#include <ntddk.h>
#endif
}

#else

#include <kndis.h>

#endif	// __KNDISCPP_H__

#include <kstl.h>

static void init_stl_heaps(void)
{
	if ( (stl_traits::non_paged_heap_size == 0) ||
		!non_paged_alloc::init(stl_traits::non_paged_heap_size, stl_traits::low_memory_callback) )
	{
		stl_traits::non_paged_heap_size = 0;
	}
#ifndef __KNDISCPP_H__
	if ( (stl_traits::paged_heap_size == 0) ||
		  !paged_alloc::init(stl_traits::paged_heap_size, stl_traits::low_memory_callback) )
	{
		stl_traits::paged_heap_size = 0;
	}
#endif	// __KNDISCPP_H__
}

static void terminate_stl_heaps(void)
{
	if ( stl_traits::non_paged_heap_size != 0)
	{
		non_paged_alloc::destroy();
	}
#ifndef __KNDISCPP_H__
	if ( stl_traits::paged_heap_size != 0)
	{
		paged_alloc::destroy();
	}
#endif	// __KNDISCPP_H__
}

#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
void (* __malloc_alloc_oom_handler)() = 0;
#endif

// __stl_init__ must be extern "C" because the symbol is forced in by
// a pragma in kstl.h. The idea is that the run time init code in cpprt.cpp
// should only init the STL if kstl.h is used in the driver.
#pragma data_seg(".STL$I")
extern "C" void (*__stl_init__)(void)= init_stl_heaps;
#pragma data_seg()

#pragma data_seg(".STL$U")
void (*__stl_terminate__)(void)= terminate_stl_heaps;
#pragma data_seg()

#endif //(_MSC_VER >= 1200)
