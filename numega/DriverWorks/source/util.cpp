// util.cpp - utility routines
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

#include <vdw.h>

ULONG ustrlen(PCWSTR ustr)
{
	ULONG i;

	for (i=0; *ustr; ustr++, i++);
	return i;
};

ULONG ustrsiz(PCWSTR ustr)
{
	return ustrlen(ustr)*sizeof(WCHAR);
}

VOID ustrcpy(PWSTR dest, PCWSTR src)
{
	while (*src) *dest++ = *src++;
	*dest = 0;
}


VOID ustrncpy(PWSTR dest, PCWSTR src, unsigned int n)
{
	WCHAR w;

	while (n--)
	{
		w = *src;
		if (*src)
			src++;

		*dest++ = w;
	}
}

PCWSTR ustrcat(PWSTR dest, PCWSTR src)
{
	ustrcpy(&dest[ustrlen(dest)], src);
	return dest;
}


PHYSICAL_ADDRESS& UlongToPhysicalAddress(PHYSICAL_ADDRESS& p, ULONG u)
{
	p.u.HighPart = 0;
	p.u.LowPart = u;
	return p;
}

struct OBJHEAD_NAMEINFO
{
	PVOID			m_ObjDir;
	UNICODE_STRING	m_ObjName;
};

struct OBJHEAD
{
	LIST_ENTRY 		m_ListEntry;
	PVOID		  	m_type;
	UCHAR			m_NameOffset;
	UCHAR			m_HandleOffset;
	UCHAR			m_QuotaOffset;
	UCHAR			m_flags;
	PVOID 			m_CreateInfo;
	PVOID			m_Security;
};


PUNICODE_STRING GetObjectName(PVOID pObj)
{
	OBJHEAD* pHead = reinterpret_cast<OBJHEAD*>(ULONG_PTR(pObj)-sizeof(OBJHEAD));

	if ( pHead->m_NameOffset != 0 )
	{
		OBJHEAD_NAMEINFO* pNameInfo =
			reinterpret_cast<OBJHEAD_NAMEINFO*>(ULONG_PTR(pHead)-pHead->m_NameOffset);

		return &pNameInfo->m_ObjName;
	}
	else
		return NULL;
}
