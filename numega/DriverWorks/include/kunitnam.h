// kunitnam.h - include file for class KUnitizedName
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

#ifndef __KUNITNAM__
#define __KUNITNAM__

class KUnitizedName
{

// Construction
public:
	SAFE_DESTRUCTORS
	KUnitizedName();
	KUnitizedName(PCWSTR base, ULONG unit, ULONG radix=10);
	KUnitizedName(PUNICODE_STRING base, ULONG unit, ULONG radix=10);
	KUnitizedName(KUnitizedName& src);
	VOID Initialize(PCWSTR base, ULONG unit, ULONG radix=10);
	VOID Initialize(PUNICODE_STRING base, ULONG unit, ULONG radix=10);
	VOID Initialize(KUnitizedName& src);
	~KUnitizedName(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_name != NULL); }
protected:
	VOID __construct(PUNICODE_STRING base);
// Methods
public:
	VOID NextUnit(void);
	operator PCWSTR() { return m_name; }
	operator PWSTR()  { return m_name; }
	operator PUNICODE_STRING() { return &m_ustring; }
	ULONG Unit(void)  { return m_unit; }
// Data
protected:
	PWSTR m_name;
	ULONG m_unit;
	ULONG m_baselen;
	ULONG m_radix;
	UNICODE_STRING m_ustring;
	BOOLEAN m_FreeOnDestroy;
};

#endif
