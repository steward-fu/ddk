// kndis.h
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

#ifndef __KNDIS_VERSION__
#define __KNDIS_VERSION__	320

// unreferenced inline/local function has been removed
#pragma warning (disable:4514)

// nonstandard extension used : nameless struct/union
#pragma warning (disable:4201)

// conditional expression is constant
#pragma warning (disable:4127)

// function 'some function()' not inlined
#pragma warning (disable:4710)

// unreferenced label
#pragma warning (disable:4102)

// function selected for automatic inline expansion
#pragma warning (disable:4711)

//Unreferenced local function has been removed
#pragma warning (disable:4505)

#if !defined(_WIN64)
typedef long LONG_PTR;
typedef unsigned long ULONG_PTR;
typedef ULONG_PTR* PULONG_PTR;
#endif


extern "C"
{
// supress a typo (!) in NT 4 DDK (NDIS.H, line 5628)
#if BINARY_COMPATIBLE
#define BINARY_COMPATIBILE 1
#endif

#include <ndis.h>
#ifndef VTOOLSD
#include <minmax.h>	// MSVC defs for min and max
#include <string.h>	// why should I do that???
#else // VTOOLSD
//Put in a missing definition for the VxD builds
#define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))

#endif


}; // "C"

// Heuristics to check what DDK we build against
#if !defined(KNDIS_DDK_VERSION)
 #ifdef NDIS_WDM
  #ifndef NDIS_FLAGS_SENT_AT_DPC
   #define	KNDIS_DDK_VERSION	5			// W2K
  #else
   #ifndef NDIS_FLAGS_UIO
    #define	KNDIS_DDK_VERSION	0x0501		// XP
   #else
    #define	KNDIS_DDK_VERSION	0x0600		// Longhorn
   #endif
  #endif
 #else
  #define	KNDIS_DDK_VERSION	4			// NT4
 #endif
#endif


// Make sure the DDK environment and target build are consistent
#if KNDIS_DDK_VERSION < 5
 #if (defined(NDIS50) || defined(NDIS50_MINIPORT))
  #error Attempt to build an NDIS 5.0 driver under an older DDK. Use DDK that support Win2000 build environment.
 #endif
#endif
#if KNDIS_DDK_VERSION < 0x0501
 #if (defined(NDIS51) || defined(NDIS51_MINIPORT))
  #error Attempt to build an NDIS 5.1 driver under an older DDK. Use DDK that support WinXP build environment.
 #endif
#endif
#if KNDIS_DDK_VERSION < 0x600
 #if (defined(NDIS6) || defined(NDIS6_MINIPORT))
  #error Attempt to build an NDIS 6 driver under an older DDK. Use DDK that support WinLH build environment.
 #endif
#endif

// Heuristic to define whether to include PnP and PM framework stuff
#if (KNDIS_DDK_VERSION > 4) && !defined(KNDIS_PNP_AWARE)
#define KNDIS_PNP_AWARE	1
#else
#define KNDIS_PNP_AWARE	0
#endif

// Define what subset of NDIS exports to use.
// DriverNetworks defaults to generate binary compatible ("loadable") miniports.
// For W2K only drivers, set KNDIS_W9X_COMPATIBLE to 0 for optimal W2K implementation.
#ifndef KNDIS_W9X_COMPATIBLE
# if BINARY_COMPATIBLE && defined(NDIS_MINIPORT_DRIVER)
#  define KNDIS_W9X_COMPATIBLE	1
# else
#  define KNDIS_W9X_COMPATIBLE	0
# endif
#endif

// Set define for W9X platform-only targets. In particular, VxD targets must have this on.
#ifndef KNDIS_W9X
#if defined(VTOOLSD) || defined(TDI_WINDOWS_95)
#define  KNDIS_W9X
#endif
#endif

// NT uses DBG=0/1, W9X uses DEBUG. Take care of that discrepancy.
#if !defined(DBG)
#if defined(DEBUG)
#define DBG 1
#else
#define DBG 0
#endif
#endif // DBG


/////////////////////////////////////////////////////////
// Define types excluded by BINARY_COMPATIBLE option.
// This is the default NDIS #define, which allows only
// an intersection of NT & Win9X exports and typedefs.
// KNDIS defines those it guarantees to perform correctly
// at both platforms.
#if BINARY_COMPATIBLE
typedef BOOLEAN (*PKSYNCHRONIZE_ROUTINE) (
    IN PVOID SynchronizeContext);
#endif //  BINARY_COMPATIBLE

#if (KNDIS_DDK_VERSION < 5)
typedef ULONG ULONG_PTR;		// for 64-bit in NDIS 4
#endif

/////////////////////////////////////////////////////////
// Controls the use of Ndis-only exports to meet NdisTest
// 3.91 NDIS_WDM requirements.
#ifndef KNDIS_WDM_STRICT
#define KNDIS_WDM_STRICT 0
#endif


/////////////////////////////////////////////////////////////////////////////
// Empty
struct KNDIS_EMPTY_t {};

// Naming convention used for static "delegators" for timers and synchronize dispatchers
#ifndef KNDIS_MEMBER_CALLBACK
#define	KNDIS_MEMBER_CALLBACK(member) SysCallback2##member
#endif

#ifndef LinkTo
#define LinkTo(x) x##LINK
#endif

/////////////////////////////////////////////////////////
// Various useful macro

// MACRO to disable copy constructors for the class
#define	PREVENT_COPY_CONSTRUCTORS(Class)	\
private:									\
	Class(const Class&);					\
	Class& operator=(const Class&)

// Checking validity of a pointer
#define KNDIS_GOOD_PTR(p)	(PVOID(p)!=NULL)	/* for now */

// unreferenced shorthand
#define UNREFERENCED_PARAMETERS2(a,b)			UNREFERENCED_PARAMETER(a); UNREFERENCED_PARAMETER(b)
#define UNREFERENCED_PARAMETERS3(a,b,c)			UNREFERENCED_PARAMETERS2(a,b); UNREFERENCED_PARAMETER(c)
#define UNREFERENCED_PARAMETERS4(a,b,c,d)		UNREFERENCED_PARAMETERS3(a,b,c); UNREFERENCED_PARAMETER(d)
#define UNREFERENCED_PARAMETERS5(a,b,c,d,e)		UNREFERENCED_PARAMETERS4(a,b,c,d); UNREFERENCED_PARAMETER(e)
#define UNREFERENCED_PARAMETERS6(a,b,c,d,e,f)	UNREFERENCED_PARAMETERS5(a,b,c,d,e); UNREFERENCED_PARAMETER(f)

// compile-time asserts
#ifndef STATIC_ASSERT
template <bool b> struct static_assert;
template<> struct static_assert<true> {};
#define STATIC_ASSERT(x) static_assert<x>()

template <bool c1, bool c2> struct static_xor; // "fix" for the lack of logical xor in C++
template <> struct static_xor<true, false> {};
template <> struct static_xor<false, true> {};
#define STATIC_ASSERT_XOR(x,y) static_xor<x,y>()
#endif

// Define a BOOLEAN const for driver type: KNDIS_PROTOCOL_DRIVER.
// It defines default parameters for some template classes.
#if defined(NDIS_MINIPORT_DRIVER)
#define KNDIS_PROTOCOL_DRIVER	FALSE
#else
#define KNDIS_PROTOCOL_DRIVER	TRUE
#endif

//////////////////////////////////////////////////////////
// Includes
#ifndef __VDW_VERSION__	// for DriverWorks builds, use their env.
 #include <kndiscpp.h>
#endif
#include <KObject.h>

#include <KNdisTimer.h>
#include <KNdisEvent.h>

#if !defined(VTOOLSD)
 #include <KNdisString.h>
#endif

#include <KNdisStats.h>
#include <KNdisConfig.h>
#include <KNdisMedium.h>

#include <KNdisAdapterTraits.h>
#include <KNdisProtocolTraits.h>

#include <KNdisOidDebug.h>

#ifdef NDIS_MINIPORT_DRIVER

 #if KNDIS_PNP_AWARE
  #include <KNdisPnpMode.h>
 #endif

 #include <KNdisResource.h>
 #include <KNdisInterrupt.h>

#endif // NDIS_MINIPORT_DRIVER

#include <KNdisSpinLock.h>
#include <KNdisInterlocked.h>

#include <KNdisList.h>
#include <KListBrowser.h>
#include <KNdisPacketList.h>
#include <KNdisHeap.h>

#ifdef NDIS_MINIPORT_DRIVER

 #include <KNdisChars.h>
 #include <KNdisWrapper.h>
 #include <KNdisOid.h>
 #include <KNdisMiniAdapter.h>


 #include <KNdisPciInformation.h>
 #include <KNdisPhysAddr.h>
 #include <KNdisPeripheralAddress.h>

#endif // NDIS_MINIPORT_DRIVER

#include <KNdisBuffer.h>
#include <KNdisBufferPool.h>
#include <KNdisPacket.h>
#include <KNdisPacketPool.h>
#include <KNdisPartialPacket.h>
#if (KNDIS_DDK_VERSION > 5)
 #include <KNdisPacketCancelId.h>
#endif

#include <KNdisBufferHeap.h>

#include <KNdisTrace.h>

#ifdef NDIS_MINIPORT_DRIVER

 #include <KNdisSharedMemory.h>
 #include <KNdisReceiveArea.h>

 #include <KNdisReceiveAreaTraits.h>
 #include <KNdisRxAreaAsyncMgr.h>
 #include <KNdisRxAreaNoAsync.h>
 #include <KNdisSharedReceiveArea.h>
 #include <KNdisSystemReceiveArea.h>


 #include <KNdisMiniDriver.h>
 //#include <KNdisPartialTransfer.h>
 #include <KNdisLookahead.h>

#endif // NDIS_MINIPORT_DRIVER

#ifndef VTOOLSD
 #include <KNdisProtocolBinding.h>
 #include <KNdisRequest.h>
#endif

#ifdef NDIS_MINIPORT_DRIVER
 #include <KNdisBundle.h>
 #include <KNdisFilterBinding.h>
 #include <KNdisFilterAdapter.h>
 #if (KNDIS_DDK_VERSION >= 0x0501)
  #include <KNdisFilterAdapter51.h>
 #endif
 #include <KNdisFilterPacketPool.h>
#endif // NDIS_MINIPORT_DRIVER

#ifndef VTOOLSD
 #include <KNdisProtocolWrapper.h>
#endif

#ifdef KNDIS_BOUNDSCHECKER
// #include <kchecker.h>
#else
 #ifndef BOUNDS_CHECKER
  #define BOUNDS_CHECKER(id, arglist)
 #endif
#endif

// Exposed Globals
namespace _KNDIS {
	extern ULONG MemTag;	// Tag for NDIS allocations. Can be set to override default 'dnw_'
	// extern bool bW98;
};

#if (defined(NDIS50) || defined(NDIS50_MINIPORT) || defined(NDIS51_MINIPORT))
 #ifdef NDIS_MINIPORT_DRIVER
  #include <KNdisMiniportVc.h>
  #include <KNdisCoMiniportChars.h>
  #include <KNdisMiniportVcTraits.h>
 #endif // NDIS_MINIPORT_DRIVER
 #include <KNdisCallManagerVc.h>
 #include <KNdisCallManagerVcTraits.h>
 #include <KNdisCoAddressFamily.h>
 #include <KNdisCallManagerChars.h>
 #include <KndisCallManagerWrapper.h>

#endif // KNDIS_DDK_VERSION >= 5

#endif // #define __KNDIS_VERSION__
