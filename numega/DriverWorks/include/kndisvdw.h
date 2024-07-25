// kndisvdw.h - alternate main include file for Compuware DriverWorks Class Library
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

#ifndef KNDISVDW_H
#define KNDISVDW_H

#ifndef NTVERSION
#error NTVERSION symbol must be defined to one of the following: 400, 'WDM'
#endif

#ifndef _WDM_
# define _WDM_   (NTVERSION == 'WDM')
#endif
#ifndef _NT400_
# define _NT400_ (NTVERSION == 400)
#endif

#include <kndiscpp.h>

// Bounds Checker events are currently not supported in KNDIS
#ifdef KNDIS_BOUNDSCHECKER
#include <kchecker.h>		// DriverWorks -- fix it!
#else
# ifndef BOUNDS_CHECKER
#  define BOUNDS_CHECKER(id, arglist)
# endif
#endif

#define LinkTo(x)  x##LINK

#include <util.h>
#include <kspin.h>
#include <kirp.h>
#include <kmemory.h>
#include <kdispobj.h>
#include <kustring.h>
#include <kevent.h>
#include <ksemaphr.h>
#include <kdpc.h>
#include <kmutex.h>
#include <kgenlock.h>
#include <klower.h>

// The following two files required only due to inclusion of <KNdisList.h> below, which
// is in turn necessary for compilation of kusb.cpp
#include <ndis.h>
#include <KNdisSpinLock.h>

#include <KNdisList.h>

#include <karray.h>
#include <ktimer.h>
#include <ksysthrd.h>

//#include <ktrace.h>	// TODO: Port a version to KNdisTrace

#include <kiocparm.h>
#include <kworkitm.h>

#if _WDM_
#include <kpnplow.h>
#include <kheap.h>
#include <k1394.h>
#include <k1394async.h>
#include <k1394Isoc.h>
#endif

#endif          // KNDISVDW_H
