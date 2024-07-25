// kerrlog.h - include file for template class KErrorLogEntry
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

#ifndef __KERRLOG__
#define __KERRLOG__

class KErrorLogEntry
{
	SAFE_DESTRUCTORS

public:
	struct StringItem
	{
		StringItem(PWSTR s) {m_String=s;}
		LIST_ENTRY m_ListEntry;
		PWSTR m_String;
	};

// Construction
public:
	KErrorLogEntry( KDevice* pDevice=NULL );
	~KErrorLogEntry(void);

// Methods
public:
	VOID SetDumpData(PVOID pDump, ULONG size);
	NTSTATUS Post(void);
	NTSTATUS InsertString(PWSTR string);
	operator PIO_ERROR_LOG_PACKET ();

	static VOID SimpleError(
		NTSTATUS FinalStatus,
		NTSTATUS ErrorCode,
		ULONG Unique,
		KDevice* pDevice=NULL
		);

// Data
protected:
	KDevice* m_object;
	KList<StringItem> m_strings;
	IO_ERROR_LOG_PACKET m_packet;
	PVOID m_dumpdata;
	ULONG m_dumplength;
};

#endif
