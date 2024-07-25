// kunitnam.cpp - implementation of class KUnitizedName
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

#pragma code_seg("PAGE")

KUnitizedName::KUnitizedName() :
	m_name(NULL),
	m_unit(0),
	m_baselen(0),
	m_radix(0),
	m_FreeOnDestroy(TRUE)
{
}

KUnitizedName::KUnitizedName(PCWSTR base, ULONG unit, ULONG radix) :
	m_name(NULL),
	m_unit(unit),
	m_radix(radix),
	m_FreeOnDestroy(TRUE)
{
	UNICODE_STRING s;

	RtlInitUnicodeString(&s, base);
	__construct(&s);
}

VOID KUnitizedName::Initialize(PCWSTR base, ULONG unit, ULONG radix)
{
	if (IsValid()) Invalidate();

	m_unit = unit;
	m_radix = radix;
	m_FreeOnDestroy = TRUE;

	UNICODE_STRING s;

	RtlInitUnicodeString(&s, base);
	__construct(&s);
}

KUnitizedName::KUnitizedName(PUNICODE_STRING base, ULONG unit, ULONG radix) :
	m_name(NULL),
	m_unit(unit),
	m_radix(radix),
	m_FreeOnDestroy(TRUE)
{
	__construct(base);
}

VOID KUnitizedName::Initialize(PUNICODE_STRING base, ULONG unit, ULONG radix)
{
	if (IsValid()) Invalidate();

	m_unit = unit;
	m_radix = radix;
	m_FreeOnDestroy = TRUE;

	__construct(base);
}

KUnitizedName::KUnitizedName(KUnitizedName& src) :
	m_name(src.m_name),
	m_unit(src.m_unit),
	m_baselen(src.m_baselen),
	m_radix(src.m_radix),
	m_ustring(src.m_ustring),
	m_FreeOnDestroy(FALSE)
{
}

VOID KUnitizedName::Initialize(KUnitizedName& src)
{
	if (IsValid()) Invalidate();

	m_name = src.m_name;
	m_unit = src.m_unit;
	m_baselen = src.m_baselen;
	m_radix = src.m_radix;
	m_ustring = src.m_ustring;
	m_FreeOnDestroy = FALSE;
}

VOID KUnitizedName::__construct(PUNICODE_STRING base)
{
	ASSERT( (m_radix == 10) || (m_radix == 16) );

	if (m_radix == 10)
	{
		ASSERT( m_unit < 1000 );
	}
	else if (m_radix == 16)
	{
		ASSERT( m_unit < 0x1000 );
	}
	else
	{
		return;
	}

	m_baselen = base->Length / sizeof(WCHAR);

	// Allow three chars and terminator
	m_name = new (PagedPool) WCHAR[m_baselen+3+1];
	m_ustring.MaximumLength = static_cast<USHORT>((m_baselen+3+1)*sizeof(WCHAR));

	if (m_name != NULL)
	{
		RtlMoveMemory(m_name, base->Buffer, m_baselen*sizeof(WCHAR));
		m_unit--;
		NextUnit();
	}

	RtlInitUnicodeString(&m_ustring, m_name);
}

VOID KUnitizedName::Invalidate(void)
{
	if (m_FreeOnDestroy && (m_name != NULL))
	{
		delete m_name;
		m_name = NULL;
	}
}

VOID KUnitizedName::NextUnit(void)
{
	if (m_name != NULL)
	{
		m_unit++;
		UNICODE_STRING s;
		s.Buffer = &m_name[m_baselen];
		s.Length = 0;
		s.MaximumLength = 4*sizeof(WCHAR);
		RtlIntegerToUnicodeString(m_unit, m_radix, &s);
		s.Buffer[ s.Length/sizeof(WCHAR) ] = 0;
		m_ustring.Length = static_cast<USHORT>(m_baselen + s.Length);
	}
}

#pragma code_seg()
