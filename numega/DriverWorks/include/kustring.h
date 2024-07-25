// kustring.h - include file for class KUstring
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

#ifndef __KUSTRING__
#define __KUSTRING__

class KUstring
{
public:
	enum SpecialCtorParameter
	{
		DoNotInitialize
	};

// Construction
public:
	SAFE_DESTRUCTORS
	KUstring(void);
	VOID Initialize(void);

	// constructors that allocate a new buffer
	KUstring(PWSTR ws, POOL_TYPE pool);
	KUstring(PSZ bcs, POOL_TYPE pool);
	KUstring(USHORT maxSize, POOL_TYPE pool);
	VOID Initialize(PWSTR ws, POOL_TYPE pool);
	VOID Initialize(PSZ bcs, POOL_TYPE pool);
	VOID Initialize(USHORT maxSize, POOL_TYPE pool);

	// constructors that inherit a buffer
	KUstring(KUstring& s);
	KUstring(PUNICODE_STRING pUs, BOOLEAN bFreeBufferOnDestroy);
	KUstring(PCWSTR ws);
	VOID Initialize(KUstring& s);
	VOID Initialize(PUNICODE_STRING pUs, BOOLEAN bFreeBufferOnDestroy);
	VOID Initialize(PCWSTR ws);

	// constructor for use with KDevice construction using placement new
	KUstring(SpecialCtorParameter) {}

	~KUstring(){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_s.Buffer != NULL); }

// Methods
public:
	UNICODE_STRING& UnicodeString(void);
	operator PUNICODE_STRING();

	WCHAR& First(void);
	WCHAR& Last(void);
	USHORT Size(void);
	USHORT MaximumSize(void);
	USHORT Length(void);
	USHORT MaximumLength(void);

	NTSTATUS Assign(KUstring& source);
	NTSTATUS Assign(PCWSTR source);

	NTSTATUS Append(KUstring& source);
	NTSTATUS Append(PCWSTR source);
	NTSTATUS AppendHex(ULONG number, ULONG width);

	BOOLEAN Match(KUstring& s);
	operator == (KUstring&);
	operator != (KUstring&);

	NTSTATUS ToWstr(PWSTR dest, USHORT Max);
	NTSTATUS ToPsz(PSZ dest, USHORT Max);
	NTSTATUS GrowMaxBy(USHORT NumberOfWideChars, POOL_TYPE pool=PagedPool);
	VOID ShortenBy(USHORT NumberOfWideChars);

// Data
protected:
	UNICODE_STRING m_s;
	BOOLEAN m_bBufferAllocatedByConstructor;
};

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

inline KUstring::KUstring(void) :
	m_bBufferAllocatedByConstructor(FALSE)
{
	m_s.Buffer = NULL;
	m_s.Length = 0;
	m_s.MaximumLength = 0;
}

inline VOID KUstring::Initialize()
{
	if (IsValid()) Invalidate();
	m_s.Buffer = NULL;
	m_s.Length = 0;
	m_s.MaximumLength = 0;
	m_bBufferAllocatedByConstructor = FALSE;
}

inline KUstring::KUstring(PCWSTR ws) :
	m_bBufferAllocatedByConstructor(FALSE)
{
	RtlInitUnicodeString(&m_s, ws);
}

inline VOID KUstring::Initialize(PCWSTR ws)
{
	if (IsValid()) Invalidate();
	RtlInitUnicodeString(&m_s, ws);
	m_bBufferAllocatedByConstructor = FALSE;
}

inline KUstring::KUstring(PUNICODE_STRING pUs, BOOLEAN bFreeBufferOnDestroy) :
	m_bBufferAllocatedByConstructor(bFreeBufferOnDestroy)
{
	m_s = *pUs;
}

inline VOID KUstring::Initialize(PUNICODE_STRING pUs, BOOLEAN bFreeBufferOnDestroy)
{
	if (IsValid()) Invalidate();
	m_s = *pUs;
	m_bBufferAllocatedByConstructor = bFreeBufferOnDestroy;
}

inline KUstring::KUstring(KUstring& s) :
	m_bBufferAllocatedByConstructor(FALSE)
{
	m_s = s.m_s;
}

inline VOID KUstring::Initialize(KUstring& s)
{
	if (IsValid()) Invalidate();
	m_s = s.m_s;
	m_bBufferAllocatedByConstructor = FALSE;
}

inline VOID KUstring::Invalidate()
{
	if (m_bBufferAllocatedByConstructor)
	{
		delete m_s.Buffer;
		m_bBufferAllocatedByConstructor = FALSE;
	}
	m_s.Length = 0;
	m_s.Buffer = NULL;
}

inline UNICODE_STRING& KUstring::UnicodeString(void)
{
	return m_s;
}

inline KUstring::operator PUNICODE_STRING()
{
	return &m_s;
}

inline WCHAR& KUstring::First(void)
{
	return m_s.Buffer[0];
}

inline WCHAR& KUstring::Last(void)
{
	return m_s.Buffer[m_s.Length/sizeof(WCHAR)-1];
}

inline USHORT KUstring::Size(void)
{
	return m_s.Length;
}

inline USHORT KUstring::MaximumSize(void)
{
	return m_s.MaximumLength;
}

inline USHORT KUstring::Length(void)
{
	return static_cast<USHORT>(m_s.Length/sizeof(WCHAR));
}

inline USHORT KUstring::MaximumLength(void)
{
	return static_cast<USHORT>(m_s.MaximumLength/sizeof(WCHAR));
}

inline NTSTATUS KUstring::Append(KUstring& source)
{
	return RtlAppendUnicodeStringToString(&m_s, &source.m_s);
}

inline NTSTATUS KUstring::Append(PCWSTR source)
{
	KUstring temp(source);
	return Append(temp);
}

inline KUstring::operator == (KUstring& compareTo)
{
	return Match(compareTo) ? TRUE : FALSE;
}

inline KUstring::operator != (KUstring& compareTo)
{
	return Match(compareTo) ? FALSE : TRUE;
}
#endif
