// E100bexRegVals.h: Default, min and max values for registry variables.
//
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


#if !defined(AFX_E100BEXREGVALS_H__0E5098A7_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
#define AFX_E100BEXREGVALS_H__0E5098A7_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if DBG
const ULONG	E100BEX_Debug_DEFAULT =			DBG_NORMAL;
const ULONG	E100BEX_Debug_MIN =				0;
const ULONG	E100BEX_Debug_MAX =				0xffffffff;
#endif

const ULONG	E100BEX_BusNumber_DEFAULT =		0;
const ULONG	E100BEX_BusNumber_MIN = 		0;
const ULONG	E100BEX_BusNumber_MAX = 		16;

const ULONG	E100BEX_NumRfd_DEFAULT =		NIC_DEF_RFDS;
const ULONG	E100BEX_NumRfd_MIN =			NIC_MIN_RFDS;
const ULONG	E100BEX_NumRfd_MAX =			NIC_MAX_RFDS;

const ULONG	E100BEX_RegNumTcb_DEFAULT =		16;
const ULONG	E100BEX_RegNumTcb_MIN =			1;
const ULONG	E100BEX_RegNumTcb_MAX =			64;

const ULONG	E100BEX_NumTbdPerTcb_DEFAULT =	8;
const ULONG	E100BEX_NumTbdPerTcb_MIN =		1;
const ULONG	E100BEX_NumTbdPerTcb_MAX =		NIC_MAX_PHYS_BUF_COUNT;

const ULONG	E100BEX_NumBuffer_DEFAULT =		8;
const ULONG	E100BEX_NumBuffer_MIN =			1;
const ULONG	E100BEX_NumBuffer_MAX =			32;

const ULONG	E100BEX_NumMapRegisters_DEFAULT = 64;
const ULONG	E100BEX_NumMapRegisters_MIN =	0;
const ULONG	E100BEX_NumMapRegisters_MAX =	0xffff;

const ULONG	E100BEX_PhyAddress_DEFAULT =	0xff;
const ULONG	E100BEX_PhyAddress_MIN =		0;
const ULONG	E100BEX_PhyAddress_MAX =		0xff;

const ULONG	E100BEX_Connector_DEFAULT =		0;
const ULONG	E100BEX_Connector_MIN =			0;
const ULONG	E100BEX_Connector_MAX =			0x2;

// NDIS BusType values
const ULONG ISABUS =						1;
const ULONG EISABUS =						2;
const ULONG PCIBUS =						5;

const ULONG	E100BEX_AiBusType_DEFAULT =		PCIBUS;
const ULONG	E100BEX_AiBusType_MIN =			EISABUS;
const ULONG	E100BEX_AiBusType_MAX =			PCIBUS;

const ULONG	E100BEX_AiTxFifo_DEFAULT =		DEFAULT_TX_FIFO_LIMIT;
const ULONG	E100BEX_AiTxFifo_MIN =			0;
const ULONG	E100BEX_AiTxFifo_MAX =			15;

const ULONG	E100BEX_AiRxFifo_DEFAULT =		DEFAULT_RX_FIFO_LIMIT;
const ULONG	E100BEX_AiRxFifo_MIN =			0;
const ULONG	E100BEX_AiRxFifo_MAX =			15;

const ULONG	E100BEX_AiTxDmaCount_DEFAULT =	0;
const ULONG	E100BEX_AiTxDmaCount_MIN =		0;
const ULONG	E100BEX_AiTxDmaCount_MAX =		63;

const ULONG	E100BEX_AiRxDmaCount_DEFAULT =	0;
const ULONG	E100BEX_AiRxDmaCount_MIN =		0;
const ULONG	E100BEX_AiRxDmaCount_MAX =		63;

const ULONG	E100BEX_AiUnderrunRetry_DEFAULT = DEFAULT_UNDERRUN_RETRY;
const ULONG	E100BEX_AiUnderrunRetry_MIN =	0;
const ULONG	E100BEX_AiUnderrunRetry_MAX =	3;

const ULONG	E100BEX_AiForceDpx_DEFAULT =	0;
const ULONG	E100BEX_AiForceDpx_MIN =		0;
const ULONG	E100BEX_AiForceDpx_MAX =		2;

const ULONG	E100BEX_AiTempSpeed_DEFAULT =	0;
const ULONG	E100BEX_AiTempSpeed_MIN =		0;
const ULONG	E100BEX_AiTempSpeed_MAX =		100;

const ULONG	E100BEX_AiThreshold_DEFAULT =	200;
const ULONG	E100BEX_AiThreshold_MIN =		0;
const ULONG	E100BEX_AiThreshold_MAX =		200;

const ULONG	E100BEX_McTimeoutFlag_DEFAULT =	2;
const ULONG	E100BEX_McTimeoutFlag_MIN =		0;
const ULONG	E100BEX_McTimeoutFlag_MAX =		2;

const ULONG	E100BEX_MWIEnable_DEFAULT =		1;
const ULONG	E100BEX_MWIEnable_MIN =			0;
const ULONG	E100BEX_MWIEnable_MAX =			1;

const ULONG	E100BEX_Congest_DEFAULT =		0;
const ULONG	E100BEX_Congest_MIN =			0;
const ULONG	E100BEX_Congest_MAX =			1;


#endif // !defined(AFX_E100BEXREGVALS_H__0E5098A7_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
