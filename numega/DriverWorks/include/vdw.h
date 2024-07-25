// vdw.h - main include file for Compuware DriverWorks Class Library
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

#ifdef _IA64_
extern "C" void __cdecl __mf(void);
#pragma intrinsic(__mf)
#endif

#ifndef __VDW_VERSION__
#define __VDW_VERSION__ 320

#ifndef NTVERSION
#error NTVERSION symbol must be defined to one of the following: 400, 'WDM'
#endif

#define _WDM_   (NTVERSION == 'WDM')
#define _NT400_ (NTVERSION == 400)

// unreferenced inline/local function has been removed
#pragma warning (disable:4514)

// nonstandard extension used : nameless struct/union
#pragma warning (disable:4201)

// conditional expression is constant
#pragma warning (disable:4127)

// function 'some function()' not inlined
#pragma warning (disable:4710)

//Unreferenced local function has been removed
#pragma warning (disable:4505)

// The DriverWorks library requires that type ULONG_PTR be defined, regardless of
// which DDK is in use. The code below declares it if _WIN64 has not been
// defined. The assumption is that if _WIN64 is defined, then basetsd.h
// will provide the correct definition for 64-bit drivers. For the 32-bit case,
// the declaration may be redundant, but should be consistent with basetsd.h if
// it eventually gets #include'd.
#if !defined(_WIN64)
typedef long LONG_PTR;
typedef unsigned long ULONG_PTR;
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef VXD_COMPATLIB
/////////////////////////// Begin NT/WDM Version /////////////////////////
extern "C" {

#if _WDM_
///////////// BEGIN WDM ONLY /////////////

#if defined(_DW_INCLUDE_NTDDK_)
#include <ntddk.h>
#else
#include <wdm.h>
#endif

NTKERNELAPI
NTSTATUS
IoRegisterShutdownNotification(
    IN PDEVICE_OBJECT DeviceObject
    );

#if !defined(IRP_MN_SURPRISE_REMOVAL)
#define IRP_MN_SURPRISE_REMOVAL 0x17
#endif

#if !defined(IRP_MN_BUS_RESET)
#define IRP_MN_BUS_RESET  0x87
#endif

///////////// END WDM ONLY /////////////

#else
#include <ntddk.h>
#endif
}

/////////////////////////// Begin NDIS_WDM Version /////////////////////////
#if KNDIS_WDM_LIBBLD
// Alternate KNDIS_WDM_LIBBLD file set

#include <kndisvdw.h>

#else	// KNDIS_WDM_LIBBLD
// Normal file set

#include <cpprt.h>
#include <util.h>

#include "function.h"

#include <kchecker.h>

#include <kspin.h>
#include <kirp.h>
#include <kmemory.h>
#include <kdevque.h>
//#include <kdmqueue.h>	// deprecated
#include <kustring.h>
#include <kdispobj.h>
#include <kevent.h>
#include <ksemaphr.h>
#include <kdpc.h>
#include <kmutex.h>
#include <kgenlock.h>
#include <klower.h>
#include <kcontrol.h>
#include <kadapter.h>
#include <kunitnam.h>
#include <kintrupt.h>
#include <klist.h>
#include <karray.h>
#include <kresreq.h>
#include <kfifo.h>
#include <ktimer.h>
#include <kregkey.h>
#include <kdriver.h>
#include <kerrlog.h>
#include <kfilter.h>
#include <kaddress.h>
#include <kimgsect.h>
#include <kdmaxfer.h>
#include <kpcicfg.h>
#include <ksysthrd.h>
#include <kfile.h>
#include <kquery.h>
#include <kheap.h>
#include <ktrace.h>
#include <kiocparm.h>
#include <ksfifo.h>
#include <kicount.h>
#include <kworkitm.h>

#if _WDM_
#include <kpnplow.h>
#include <kstdwmi.h>
#include <kwmiblock.h>
#include <kwmistr.h>
#include <kpnpdev.h>
#include <Kwdmfltr.h>
#include <kvxdintf.h>
#include <k1394.h>
#include <k1394async.h>
#include <k1394isoc.h>
#include <kbus.h>
#include <kphysdev.h>
#endif

#include <kdmqex.h>
#include <kcsq.h>
#include <kdmqcsq.h>

#if _WDM_
#include <kvirtualcom.h>
#endif

#endif			// KNDIS_WDM_LIBBLD
#else ///////////////// End NT/WDM Version //////////////////////////


//////////////////  Begin VxD Compatible Library Version ////////////////////

#define LinkTo(x)  x##LINK

#include <vtoolscp.h>
#include "vxdntlib.h"
#define NTHALAPI NTAPI
#include <util.h>
#include <kchecker.h>
#include <kustring.h>
#include <kunitnam.h>
#include <kregkey.h>
#include <kintrupt.h>
#include <klist.h>
#include <kspin.h>
#include <kfifo.h>
#include <kaddress.h>
#include <kdpc.h>
#include <kdispobj.h>
#include <ktimer.h>
#include <kevent.h>
#include <ktrace.h>
#include <kresreq.h>
#include <kmemory.h>
#include <kiocparm.h>
#include <ksfifo.h>
#include <kfile.h>

#endif          // VXD_COMPATLIB
#endif          // __VDW_VERSION__
