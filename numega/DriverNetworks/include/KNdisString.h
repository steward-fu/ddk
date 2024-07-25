// KNdisString.h: interface for the KNdisString class.
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

#if !defined(AFX_KNDISSTRING_H__D47AFC66_2FC7_11D3_8F9F_00C04F7445B7__INCLUDED_)
#define AFX_KNDISSTRING_H__D47AFC66_2FC7_11D3_8F9F_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////
// KNdisStringConst
//
// Wraps NDIS_STRING. Used as a base for more generic KNdisString.
// Use KNdisStringConst to describe non-modifiable (const) unicode strings.
// Use KNdisString for all other cases.
//
class KNdisStringConst
{
public:
// Constructors
	inline KNdisStringConst(PCWSTR pStr, USHORT maxlen, USHORT len)
	{
		m_str.Length = len;
		m_str.MaximumLength = maxlen;
		m_str.Buffer = (PWSTR) pStr;
	}

	inline KNdisStringConst(PCWSTR pStr) { NdisInitUnicodeString(&m_str, pStr);}

// Attributes

	// Check if the string is empty
	inline BOOLEAN IsEmpty() const { return m_str.Length==0; }
	//Check whether the string's buffer has been allocated
	inline BOOLEAN IsValid(void) const {return (m_str.Buffer != NULL);}

	//Get the number of bytes in the string
	inline USHORT Size(void) const { return m_str.Length; }
	inline USHORT MaximumSize(void) const { return m_str.MaximumLength; }
	//Get the number of characters in the string
	inline USHORT Length(void) const { return static_cast<USHORT>(m_str.Length/sizeof(WCHAR)); }
	inline USHORT MaximumLength(void) const {return static_cast<USHORT>(m_str.MaximumLength/sizeof(WCHAR)); }

// Conversions
	inline operator const PNDIS_STRING () { return &m_str;}
	inline operator const PWSTR () const { return m_str.Buffer; }

protected:
	NDIS_STRING	m_str;
	inline KNdisStringConst() {} // for derived only
};

//////////////////////////////////////////////////////////////////
// KNDIS_STRING_CONST
//
// Use KNDIS_STRING_CONST defined below as a short hand for func
// parameters needing constant NDIS_STRINGs as arguments. E.g.
//
//	KNdisConfig		Config(...);
//	UINT MaxPoolSize = Config.Read(KNDIS_STRING_CONST("MaxPoolSize"));
//
#define	KNDIS_STRING_CONST(x)	KNdisStringConst(L##x, sizeof(L##x), sizeof(L##x)-2)

/////////////////////////////////////////////////////////////////////
// KNdisString
//
// Encapsulates NDIS_STRING and provides modify access to it.
//
class KNdisString : public KNdisStringConst
{
	SAFE_DESTRUCTORS
public:
	// Constructor (empty string)
	inline KNdisString();
	// Constructor (from a PCWSTR)
	inline KNdisString(PCWSTR s);
	//Constructor (from an NDIS_STRING)
	inline KNdisString(PNDIS_STRING NdisString, BOOLEAN FreeBufferOnDestroy = FALSE);
	//Constructor (From another KNdisString)
	inline KNdisString(KNdisString& theString);
	//Constructor (from an ansi string)
	inline KNdisString(PSTR ansiString);
	//Constructor (just allocates storage)
	inline KNdisString(USHORT maxSize);
	// Destructor
	inline ~KNdisString();

	// Assignment from PNDIS_STRING
	inline KNdisString& operator =(PNDIS_STRING Source);
	//Assignment for PCWSTR
	inline KNdisString& operator=(PCWSTR s);
	//Assignment from an ansi string
	inline KNdisString& operator=(PSTR Source);
	//Assignment from another KNdisString
	inline KNdisString& operator=(KNdisString& theString);
	//Allocate a specific amount of memory for the new string. MaxSize is
	//the number of characters the buffer will be able to hold
	inline VOID Initialize(USHORT maxSize);

	// Dump the buffer to a user's WCHAR string or ascii string
	inline NDIS_STATUS ToWstr(PWSTR destination, USHORT SizeOfBuffer);
	inline NDIS_STATUS ToStr(PSTR destination, USHORT SizeOfBuffer);

	// Equality to another NDIS_STRING (case insensitive)
	inline BOOLEAN operator == (PNDIS_STRING s);

	// Equality to PWSTR (case insensitive)
	inline BOOLEAN operator == (PWSTR s);
	inline BOOLEAN operator == (PCWSTR s);
	//Equality and lack of equality to a KNdisString
	inline BOOLEAN operator == (KNdisString& s);
	inline BOOLEAN operator != (KNdisString& s);

	//Append without reallocating
	inline NDIS_STATUS Append(KNdisString& source);
	inline NDIS_STATUS Append(KNdisStringConst& source);
	inline NDIS_STATUS Append(PNDIS_STRING source);
	inline NDIS_STATUS Append(PCWSTR source);

	//Assign copies a string to the existing buffer and sets the length appropriately
	inline NDIS_STATUS Assign(PCWSTR source);
	inline NDIS_STATUS Assign(KNdisString& source);

	//Increase the number of characters the string can hold
	inline NDIS_STATUS GrowMaxBy(USHORT NumberOfWideChars);
	//Reduce the number of characters in the string.  Has no effect on
	//the maximum characters that can be stored in the string.
	inline VOID ShortenBy(USHORT NumberOfWideChars);

	inline VOID Invalidate();

protected:
	BOOLEAN m_FreeBufferOnDestroy; //should we free the buffer?

	//Free and reallocate the buffer
	inline void Reallocate(USHORT Length);
	// Get string length
	static inline ULONG ustrlen(PCWSTR ustr);
};


////////////////////////////////////////////////////////////////////////
//Implementation of KNdisString
////////////////////////////////////////////////////////////////////////

KNdisString::KNdisString():
m_FreeBufferOnDestroy(FALSE)
{
	NdisInitUnicodeString(&m_str, NULL);
}

////////////////////////////////////////////////////////////////////////
KNdisString::KNdisString(PCWSTR s):
m_FreeBufferOnDestroy(TRUE)
{
	NdisInitUnicodeString(&m_str, NULL);
	USHORT len = (USHORT) ustrlen(s);
	Reallocate(len);
	len *= sizeof(WCHAR);
	if (m_str.Buffer) {
		NdisMoveMemory(m_str.Buffer, s, len);
		m_str.Length = len;
	}

}
////////////////////////////////////////////////////////////////////////////
KNdisString::KNdisString(PNDIS_STRING NdisString, BOOLEAN FreeBufferOnDestroy):
m_FreeBufferOnDestroy(FreeBufferOnDestroy)
{
	if(NdisString)
	{
		m_str.Buffer = NdisString->Buffer;
		m_str.Length = NdisString->Length;
		m_str.MaximumLength = NdisString->MaximumLength;
	}
	else
		NdisInitUnicodeString(&m_str, NULL);
}

KNdisString::KNdisString(KNdisString& theString):
m_FreeBufferOnDestroy(FALSE)
{
	m_str = theString.m_str;
}
/////////////////////////////////////////////////////////////////////////////
KNdisString::KNdisString(PSTR Source):
m_FreeBufferOnDestroy(TRUE)
{
	NdisInitUnicodeString(&m_str, NULL);

	ANSI_STRING ansiString;

	ansiString.Length = ansiString.MaximumLength = static_cast<USHORT>(strlen(Source));
	ansiString.Buffer = Source;

	Reallocate(static_cast<USHORT>(strlen(Source)+1));

	if (m_str.Buffer != NULL)
		NdisAnsiStringToUnicodeString(&m_str, &ansiString);
}

/////////////////////////////////////////////////////////////////////////////

KNdisString::KNdisString(USHORT maxSize):
m_FreeBufferOnDestroy(TRUE)
{
	NdisInitUnicodeString(&m_str, NULL);
	Reallocate(maxSize);
}

/////////////////////////////////////////////////////////////////////////////

KNdisString& KNdisString::operator=(KNdisString& theString)
{
	if(m_str.Buffer && m_FreeBufferOnDestroy)
		delete m_str.Buffer;

	m_FreeBufferOnDestroy = FALSE;
	m_str = theString.m_str;
	return *this;
}
/////////////////////////////////////////////////////////////////////////////

KNdisString& KNdisString::operator=(PCWSTR s)
{
	USHORT len = (USHORT) ustrlen(s);
	Reallocate(len);
	len *= sizeof(WCHAR);
	if (m_str.Buffer) {
		NdisMoveMemory(m_str.Buffer, s, len);
		m_str.Length = len;
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////

KNdisString& KNdisString::operator=(PSTR Source)
{
	ANSI_STRING ansiString;

	ansiString.Length = ansiString.MaximumLength = static_cast<USHORT>(strlen(Source));
	ansiString.Buffer = Source;

	Reallocate(static_cast<USHORT>(strlen(Source)+1));

	if (m_str.Buffer != NULL)
		NdisAnsiStringToUnicodeString(&m_str, &ansiString);

	return *this;
}
/////////////////////////////////////////////////////////////////////////////

VOID KNdisString::Initialize(USHORT maxSize)
{
	Reallocate(maxSize);
}
/////////////////////////////////////////////////////////////////////////////

KNdisString::~KNdisString()
{
	if (m_FreeBufferOnDestroy) delete m_str.Buffer;
}
/////////////////////////////////////////////////////////////////////////////

KNdisString& KNdisString::operator=(PNDIS_STRING Source)
{
	Reallocate(Source->MaximumLength);
	if (m_str.Buffer) {
		NdisMoveMemory(m_str.Buffer, Source->Buffer, Source->MaximumLength);
		m_str.Length = Source->Length;
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////

void KNdisString::Reallocate(USHORT Length)
{
	if (m_str.Buffer && m_FreeBufferOnDestroy)
		delete m_str.Buffer;
	m_str.Length = 0;
	m_str.Buffer = NULL;
	m_str.Buffer = new WCHAR [Length];
	if(m_str.Buffer)
		m_str.MaximumLength = static_cast<USHORT>(Length*sizeof(WCHAR));
	else
	{
		m_str.MaximumLength = 0;
		ASSERT(m_str.Buffer);
	}
	m_FreeBufferOnDestroy = TRUE;

}
/////////////////////////////////////////////////////////////////////////////

ULONG KNdisString::ustrlen(PCWSTR ustr)
{
	for (ULONG i=0; *ustr; ustr++, i++);
	return i;
}

/////////////////////////////////////////////////////////////////////////////

// Define stuff missing in DDK 4
#if (KNDIS_DDK_VERSION) < 5
#if !defined(NT_INCLUDED)
NTSYSAPI BOOLEAN NTAPI RtlEqualUnicodeString(PUNICODE_STRING,PUNICODE_STRING,BOOLEAN);
#endif
#define NdisEqualUnicodeString		RtlEqualUnicodeString
#endif

BOOLEAN KNdisString::operator == (PNDIS_STRING s)
{
	// NOTE: PASSIVE_LEVEL only!!
	return NdisEqualUnicodeString(&m_str, s, TRUE);
}
/////////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisString::operator == (PWSTR s)
{
	// NOTE: PASSIVE_LEVEL only!!
	NDIS_STRING tmp;
	NdisInitUnicodeString(&tmp, s);
	return NdisEqualUnicodeString(&m_str, &tmp, TRUE);
}
BOOLEAN KNdisString::operator == (PCWSTR s)
{
	// NOTE: PASSIVE_LEVEL only!!
	NDIS_STRING tmp;
	NdisInitUnicodeString(&tmp, s);
	return NdisEqualUnicodeString(&m_str, &tmp, TRUE);
}
/////////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisString::operator == (KNdisString& s)
{
	return NdisEqualUnicodeString(&m_str, PNDIS_STRING(s), TRUE);
}
/////////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisString::operator != (KNdisString& s)
{
	return !(NdisEqualUnicodeString(&m_str, PNDIS_STRING(s), TRUE));
}
/////////////////////////////////////////////////////////////////////////////

NDIS_STATUS KNdisString::ToWstr(PWSTR destination, USHORT SizeOfBuffer)
{
	if (m_str.Length < SizeOfBuffer)
	{
		NdisMoveMemory(destination, m_str.Buffer, m_str.Length);
		destination[m_str.Length/sizeof(WCHAR)] = 0;
		return NDIS_STATUS_SUCCESS;
	}
	else
		return NDIS_STATUS_BUFFER_OVERFLOW;
}
/////////////////////////////////////////////////////////////////////////////

NDIS_STATUS KNdisString::ToStr(PSTR destination, USHORT SizeOfBuffer)
{
	ANSI_STRING ansiString;

	ansiString.Buffer = destination;
	ansiString.Length = 0;
	ansiString.MaximumLength = SizeOfBuffer;

	USHORT nchars = static_cast<USHORT>(m_str.Length/sizeof(WCHAR));

	if (nchars < SizeOfBuffer)
	{
		destination[nchars] = 0;
		return NdisUnicodeStringToAnsiString(&ansiString, &m_str);
	}
	else
		return NDIS_STATUS_BUFFER_OVERFLOW;
}

/////////////////////////////////////////////////////////////////////////////

NDIS_STATUS KNdisString::Append(KNdisString& source)
{
	return Append(&source.m_str);
}

NDIS_STATUS KNdisString::Append(KNdisStringConst& source)
{
	return Append(PNDIS_STRING(source));
}

/////////////////////////////////////////////////////////////////////////////

NDIS_STATUS KNdisString::Append(PNDIS_STRING source)
{
	if(m_str.MaximumLength < m_str.Length + source->Length)
		return NDIS_STATUS_BUFFER_OVERFLOW;
	NdisMoveMemory(m_str.Buffer + Length(), source->Buffer, source->Length);
	m_str.Length = static_cast<USHORT>(m_str.Length + source->Length);
	return NDIS_STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

NDIS_STATUS KNdisString::Append(PCWSTR source)
{
	KNdisString temp(source);
	if(temp.IsValid())
		return Append(temp);
	else
		return NDIS_STATUS_RESOURCES;
}
/////////////////////////////////////////////////////////////////////////////

NDIS_STATUS KNdisString::Assign(PCWSTR source)
{
	KNdisString temp(source);
	if(temp.IsValid())
		return Assign(temp);
	else
		return NDIS_STATUS_RESOURCES;
}
/////////////////////////////////////////////////////////////////////////////

NDIS_STATUS KNdisString::Assign(KNdisString& source)
{
	if(MaximumSize() < source.Size())
		return NDIS_STATUS_BUFFER_OVERFLOW;
	NdisMoveMemory(m_str.Buffer, (PWSTR)source, source.Size());
	m_str.Length = source.Size();
	return NDIS_STATUS_SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////

NDIS_STATUS KNdisString::GrowMaxBy(USHORT NumberOfWideChars)
{
	USHORT DesiredLength = static_cast<USHORT>(m_str.MaximumLength + NumberOfWideChars*sizeof(WCHAR));

	// create a temporary string with uninitialized storage
	KNdisString temp(DesiredLength);
	if (!temp.IsValid())
		return NDIS_STATUS_RESOURCES;

	// copy the data from this string to the temp string
	temp.Assign(*this);

	// now copy the temp string to this
	*this = temp;

	// prevent the destructor of temp from deallocating buffer
	temp.m_FreeBufferOnDestroy = FALSE;

	// set this so that destructor will deallocate
	m_FreeBufferOnDestroy = TRUE;

	return STATUS_SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////

VOID KNdisString::ShortenBy(USHORT NumberOfWideChars)
{
	USHORT Reduction = static_cast<USHORT>(NumberOfWideChars*sizeof(WCHAR));

	if (Reduction > m_str.Length)
		m_str.Length = 0;
	else
		m_str.Length = static_cast<USHORT>(m_str.Length - Reduction);

}

/////////////////////////////////////////////////////////////////////////////

VOID KNdisString::Invalidate()
{
	if (m_FreeBufferOnDestroy)
	{
		delete m_str.Buffer;
		m_FreeBufferOnDestroy = FALSE;
	}
	m_str.Length = m_str.MaximumLength = 0;
	m_str.Buffer = NULL;
}

#endif // !defined(AFX_KNDISSTRING_H__D47AFC65_2FC7_11D3_8F9F_00C04F7445B7__INCLUDED_)
