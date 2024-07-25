// kstlnphs.cpp - STL: default data object for non-paged heap size initializer ( data only )
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
//
// This data object is normally declared and instantiated in the driver, but if the driver
// writer elects to use the default value, this will be taken from the library.

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

#endif	// NDIS_MINIPORT_DRIVER

#include <kstl.h>

size_t stl_traits::non_paged_heap_size = 32768;

#endif //(_MSC_VER >= 1200)
