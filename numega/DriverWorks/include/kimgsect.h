// kimgsect.h - include file for class KImageSection
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

#if !_WDM_
#ifndef __KIMGSECTION__
#define __KIMGSECTION__

typedef enum {CodeSection, DataSection} SECTION_TYPE;

class KImageSection
{
	SAFE_DESTRUCTORS
public:
	KImageSection(SECTION_TYPE type, PVOID AddressInSection);
	static VOID UnlockAll(void);
	static VOID ResetAll(void);
	VOID Lock(void);
	VOID Unlock(void);

	operator PVOID(void) { return m_handle; }
protected:
	PVOID m_handle;
	SECTION_TYPE m_type;
	PVOID m_address;
};

#define SECTION_MARK(Name) extern "C" VOID Name(void); VOID Name(void) {}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

inline KImageSection::KImageSection(SECTION_TYPE type, PVOID AddressInSection)
{
	m_type = type;
	m_address = AddressInSection;
	m_handle = NULL;
}

inline VOID KImageSection::UnlockAll(void)
{
	MmPageEntireDriver(&KImageSection::UnlockAll);
}

inline VOID KImageSection::ResetAll(void)
{
	MmResetDriverPaging(&KImageSection::ResetAll);
}

inline VOID KImageSection::Lock(void)
{
	if (m_handle != NULL)
		MmLockPagableSectionByHandle(m_address);
	else if (m_type == CodeSection)
		m_handle = MmLockPagableCodeSection(m_address);
	else
		m_handle = MmLockPagableDataSection(m_address);
}

inline VOID KImageSection::Unlock(void)
{
	if (m_handle != NULL)
		MmUnlockPagableImageSection(m_handle);
}

#endif
#endif
