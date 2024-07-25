// KNdisRxAreaNoAsync.h: interface for the KNdisRxAreaNoAsync class.
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

#if !defined(AFX_KNDISRXAREANOASYNC_H__C033C983_0EE7_11D4_AFF1_00C04F6A5DA9__INCLUDED_)
#define AFX_KNDISRXAREANOASYNC_H__C033C983_0EE7_11D4_AFF1_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////
// Use this as the Async class for KNdisSharedReceiveArea for
// miniports that do not require any Asynchronous shared
// memory allocations using NdisMAllocateSharedMemoryAsync.
// The methods will essentially become NOP's in a Free build.
//
// This class is used as the 2nd template parameter to
// KNdisSharedReceiveArea.  To declare a KNdisSharedReceiveArea
// object using this class, use the following:
//
// KNdisSharedReceiveArea< MyAdapterClass, KNdisRxAreaNoAsync > MyRxArea
//
// where MyAdapterClass is your KNdisMiniAdapter derived class.
//
class KNdisRxAreaNoAsync {
public:
	static BOOLEAN	Debit(ULONG)					{ return FALSE; }
	static BOOLEAN	Credit(ULONG)					{ return FALSE; }
	static PVOID	AllocateComplete(PVOID, UINT)	{ return NULL;  }
	static BOOLEAN	Remove(PNDIS_PACKET)			{ return FALSE; }
	static BOOLEAN	FreeMemoryChunk()				{ return FALSE; }

	KNdisRxAreaNoAsync(
		KNdisMiniAdapter&,
		ULONG,
		ULONG,
		ULONG,
		ULONG
		){}
};

#endif // !defined(AFX_KNDISRXAREANOASYNC_H__C033C983_0EE7_11D4_AFF1_00C04F6A5DA9__INCLUDED_)
