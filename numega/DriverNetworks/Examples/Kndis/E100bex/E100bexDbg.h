// E100bexDbg.h: definitions of Debug flags and trace macros.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//
// Portions copied from Microsoft Windows 2000 DDK sample driver containing the
// following copyright
//
/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/


#if !defined(AFX_E100BEXDBG_H__0E5098AB_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
#define AFX_E100BEXDBG_H__0E5098AB_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//-------------------------------------------------------------------------
// DEBUG enable bit definitions
//-------------------------------------------------------------------------
#define DBG_LEVEL1  0x0010      // Display TRACE1 messages
#define DBG_LEVEL2  0x0020      // Display TRACE2 messages
#define DBG_LEVEL3  0x0040      // Display TRACE3 messages
#define DBG_LEVELS  (DBG_LEVEL1|DBG_LEVEL2|DBG_LEVEL3)
#define DBG_NORMAL  (DBG_LEVEL1)
#define DBG_MEDIUM  (DBG_LEVEL1|DBG_LEVEL2)
#define DBG_VERBOSE (DBG_LEVEL1|DBG_LEVEL2|DBG_LEVEL3)

//-------------------------------------------------------------------------
// Definitions for all of the Debug macros.  If we're in a debug (DBG) mode,
// these macros will print information to the debug terminal.  If the
// driver is compiled in a free (non-debug) environment the macros become
// NOPs.  The debug trace mask is stored in the high word of 
// KNdisOidDebug::sm_uTraceMask accessed via KNdisOidDebug::TraceMask().
//-------------------------------------------------------------------------

#if DBG

#define TRACE1(S) {if ( (KNdisOidDebug::TraceMask() >> 16) & DBG_LEVEL1) {DbgPrint S;}}
#define TRACE2(S) {if ( (KNdisOidDebug::TraceMask() >> 16) & DBG_LEVEL2) {DbgPrint S;}}
#define TRACE3(S) {if ( (KNdisOidDebug::TraceMask() >> 16) & DBG_LEVEL3) {DbgPrint S;}}

#else // DBG

#define TRACE1(S)      ;
#define TRACE2(S)      ;
#define TRACE3(S)      ;

#endif // DBG


#endif // !defined(AFX_E100BEXDBG_H__0E5098AB_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
