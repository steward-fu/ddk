// tdidef9x.h	-	Win9X specific definitions and includes
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
// NOTE:	Use the latest (July 1999) Windows 98 DDK. The earlier
//			Win95 DDK does not have some of the required tdixxx.h files.
//
// RE Win9X TDI Client Build Environment
// ======================================
// TDI Clients who are also NDIS miniports (.SYS files) should NOT use VtoolsD
// classes. They use DriverNetworks only. VTOOLSD symbol must NOT be defined.
// Higher layer TDI Clients (.VXD files) can use VtoolsD.
//
// NT uses DBG=0/1, W9X uses DEBUG. Take care of that discrepancy.

#if !defined(DBG)
#if defined(DEBUG)
#define DBG 1
#else
#define DBG 0
#endif
#endif // DBG

// VtoolsD environment: used for .VxD targets
#ifdef VTOOLSD

 #define USE_NDIS
 #include <vtoolscp.h>

 #ifndef TDI_TRACE
 #ifdef DEBUG
 # define TDI_TRACE dprintf
 #else
 # define TDI_TRACE
 #endif
 #endif
 #ifndef FAR
 #define FAR			// yak
 #endif

#else	                 // VTOOLSD
// Non-VtoolsD environment: used for .SYS targets (e.g. virtual miniport)
// NOTE: we include vtoolsd.h only to avoid having DDK98 installed on the build machine
extern "C" {

 #include <ndis.h>        // NT DDK
 #include "w9xNdisExp.h"  // #undef non compatibles

// Include VtoolsD without VTOOLSD for all the DDK 9X defs
#include <vtoolsd.h>

 #ifndef TDI_TRACE
 #ifdef DEBUG
 # define TDI_TRACE DbgPrint
 #else
 # define TDI_TRACE
 #endif
 #endif

} // extern "C"
#endif                  // VTOOLSD

// asserts
#ifndef TDI_ASSERT
# define TDI_ASSERT(x)	ASSERT(x)
#endif



#ifdef __cplusplus
extern "C" {
#endif

#include "tdi.h"									// DDK 98: TDI object structures

typedef												// Define the  missing callback type
void  (__cdecl *CTEReqCmpltRtn)(void* , UINT, UINT);
#include "tdivxd.h"									// DDK 98: TDI callback prototypes

#include "netvxd.h"									// DDK 98: VxD service IDs, including TDI.VXD

#include "vtdi.h"									// TDI VxD Service Table

#ifdef __cplusplus
} // extern "C"
#endif

#ifndef _WDMDDK_
#define PFILE_OBJECT    PVOID   /* to avoid #ifdefs in 9X */
#endif
