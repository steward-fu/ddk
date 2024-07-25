// kustring.cpp - implementation of class KUstring
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
/////////////////////////////////////////////////////////////
// Constructor 1
//
// Allocates storage in the specified pool, copies data from
// supplied Unicode null terminated string
//
KUstring::KUstring(PWSTR ws, POOL_TYPE pool)
{
	m_s.Length = (USHORT)ustrsiz(ws);
	m_s.MaximumLength = m_s.Length + sizeof(WCHAR);
	m_s.Buffer = new(pool) WCHAR[m_s.MaximumLength/sizeof(WCHAR)];

	if (m_s.Buffer != NULL)
	{
		m_bBufferAllocatedByConstructor = TRUE;
		RtlMoveMemory(m_s.Buffer, ws, m_s.MaximumLength);
	}
	else
	{
		m_bBufferAllocatedByConstructor = FALSE;
		m_s.MaximumLength = m_s.Length = 0;
	}
}

VOID KUstring::Initialize(PWSTR ws, POOL_TYPE pool)
{
	if (IsValid()) Invalidate();

	m_s.Length = (USHORT)ustrsiz(ws);
	m_s.MaximumLength = m_s.Length + sizeof(WCHAR);
	m_s.Buffer = new(pool) WCHAR[m_s.MaximumLength/sizeof(WCHAR)];

	if (m_s.Buffer != NULL)
	{
		m_bBufferAllocatedByConstructor = TRUE;
		RtlMoveMemory(m_s.Buffer, ws, m_s.MaximumLength);
	}
	else
	{
		m_bBufferAllocatedByConstructor = FALSE;
		m_s.MaximumLength = m_s.Length = 0;
	}
}

/////////////////////////////////////////////////////////////
// Constructor 2
//
// Allocates storage in the specified pool, copies data from
// supplied ANSI null terminated string
//
KUstring::KUstring(PSZ bcs, POOL_TYPE pool)
{
	ANSI_STRING a;

	a.Length = a.MaximumLength = static_cast<USHORT>(strlen(bcs));
	a.Buffer = bcs;

	m_s.MaximumLength = static_cast<USHORT>(strlen(bcs)+1);
	m_s.Buffer = new(pool) WCHAR[m_s.MaximumLength];
	m_s.MaximumLength *= sizeof(WCHAR);

	if (m_s.Buffer != NULL)
	{
		m_bBufferAllocatedByConstructor = TRUE;
		RtlAnsiStringToUnicodeString(&m_s, &a, FALSE);
	}
	else
	{
		m_bBufferAllocatedByConstructor = FALSE;
		m_s.MaximumLength = m_s.Length = 0;
	}
}

VOID KUstring::Initialize(PSZ bcs, POOL_TYPE pool)
{
	if (IsValid()) Invalidate();

	ANSI_STRING a;

	a.Length = a.MaximumLength = static_cast<USHORT>(strlen(bcs));
	a.Buffer = bcs;

	m_s.MaximumLength = static_cast<USHORT>(strlen(bcs)+1);
	m_s.Buffer = new(pool) WCHAR[m_s.MaximumLength];
	m_s.MaximumLength *= sizeof(WCHAR);

	if (m_s.Buffer != NULL)
	{
		m_bBufferAllocatedByConstructor = TRUE;
		RtlAnsiStringToUnicodeString(&m_s, &a, FALSE);
	}
	else
	{
		m_bBufferAllocatedByConstructor = FALSE;
		m_s.MaximumLength = m_s.Length = 0;
	}
}

/////////////////////////////////////////////////////////////
// Constructor 3
//
// Allocates storage in the specified pool with indicated size,
// does not initialize the string storage. (Use with Assign)
//
KUstring::KUstring(USHORT maxSize, POOL_TYPE pool)
{
	m_s.Length = 0;
	m_s.Buffer = new(pool) WCHAR[maxSize/sizeof(WCHAR)];

	if (m_s.Buffer != NULL)
	{
		m_bBufferAllocatedByConstructor = TRUE;
		m_s.MaximumLength = maxSize;
	}
	else
	{
		m_bBufferAllocatedByConstructor = FALSE;
		m_s.MaximumLength = 0;
	}
}

VOID KUstring::Initialize(USHORT maxSize, POOL_TYPE pool)
{
	if (IsValid()) Invalidate();

	m_s.Length = 0;
	m_s.Buffer = new(pool) WCHAR[maxSize/sizeof(WCHAR)];

	if (m_s.Buffer != NULL)
	{
		m_bBufferAllocatedByConstructor = TRUE;
		m_s.MaximumLength = maxSize;
	}
	else
	{
		m_bBufferAllocatedByConstructor = FALSE;
		m_s.MaximumLength = 0;
	}
}

/////////////////////////////////////////////////////////////
// Assign
//
// Basically, a copy constructor, but the storage must be
// already allocated
//
NTSTATUS KUstring::Assign(KUstring& source)
{
	if (source.m_s.Length <= m_s.MaximumLength)
	{
		m_s.Length = source.m_s.Length;
		RtlMoveMemory(m_s.Buffer, source.m_s.Buffer, m_s.Length);
		return STATUS_SUCCESS;
	}
	else
		return STATUS_BUFFER_TOO_SMALL;
}

/////////////////////////////////////////////////////////////
// Assign
//
// Copies data from supplied Unicode string, sets count
//
NTSTATUS KUstring::Assign(PCWSTR source)
{
	USHORT sourceSize = (USHORT)ustrsiz(source);

	if (sourceSize <= m_s.MaximumLength)
	{
		m_s.Length = sourceSize;
		RtlMoveMemory(m_s.Buffer, source, m_s.Length);
		return STATUS_SUCCESS;
	}
	else
		return STATUS_BUFFER_TOO_SMALL;
}

/////////////////////////////////////////////////////////////
// ToWstr
//
// Dumps string data to supplied buffer
//
NTSTATUS KUstring::ToWstr(PWSTR dest, USHORT Max)
{
	if (m_s.Length < Max)
	{
		RtlMoveMemory(dest, m_s.Buffer, m_s.Length);
		dest[m_s.Length/sizeof(WCHAR)] = 0;
		return STATUS_SUCCESS;
	}
	else
		return STATUS_BUFFER_TOO_SMALL;
}

/////////////////////////////////////////////////////////////
// ToPsz
//
// Dumps string data to supplied buffer
//
NTSTATUS KUstring::ToPsz(PSZ dest, USHORT sizeOfBuffer)
{
	ANSI_STRING as;

	as.Buffer = dest;
	as.Length = 0;
	as.MaximumLength = sizeOfBuffer;

	USHORT nchars = static_cast<USHORT>(m_s.Length/sizeof(WCHAR));

	if (nchars < sizeOfBuffer)
	{
		dest[nchars] = 0;
		return RtlUnicodeStringToAnsiString(&as, &m_s, FALSE);
	}
	else
		return STATUS_BUFFER_TOO_SMALL;
}

/////////////////////////////////////////////////////////////
// ShortenBy
//
// Truncates the string by the indicated number of characters,
// does not reduce maximum capacity.
//
VOID KUstring::ShortenBy(USHORT NumberOfWideChars)
{
	USHORT Reduction = static_cast<USHORT>(NumberOfWideChars*sizeof(WCHAR));

	if (Reduction > m_s.Length)
		m_s.Length = 0;
	else
		m_s.Length = static_cast<USHORT>(m_s.Length - Reduction);
}

/////////////////////////////////////////////////////////////
// GrowMaxBy
//
// Increase storage capacity of the string by indicated number
// of characters
//
NTSTATUS KUstring::GrowMaxBy(USHORT nWChars, POOL_TYPE pool)
{
	USHORT DesiredLength = static_cast<USHORT>(m_s.MaximumLength + nWChars*sizeof(WCHAR));

	// create a temporary string with uninitialized storage
	KUstring temp(DesiredLength, pool);
	if (temp.m_s.Buffer == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	// copy the data from this string to the temp string
	temp.Assign(*this);

	// free the old buffer if the destructor would have
	if (m_bBufferAllocatedByConstructor)
		delete m_s.Buffer;

	// now copy the temp string to this
	*this = temp;

	// prevent the dtor of temp from deallocating buffer
	temp.m_bBufferAllocatedByConstructor = FALSE;

	// set this so that dtor will deallocate
	m_bBufferAllocatedByConstructor = TRUE;

	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////
// Match
//
// Determine if two strings are the same
//
BOOLEAN KUstring::Match(KUstring& s)
{
	USHORT len = m_s.Length;

	if (len != s.m_s.Length)
		return FALSE;

	PUSHORT p1 = m_s.Buffer;
	PUSHORT p2 = s.m_s.Buffer;

	len /= sizeof(USHORT);

	while (len--)
		if (*p1++ != *p2++)
			return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// AppendHex
//
// Append a number, encoded in hex format, to the string with specified width
//
NTSTATUS KUstring::AppendHex(ULONG number, ULONG width)
{
	ULONG reqWidth, N;
	WCHAR numbuf[9];

	// validate parameters
	if (( width == 0 ) || (width > 8) )
		return STATUS_INVALID_PARAMETER;

	// initialize the array of WCHAR representing the number
	numbuf[8] = 0;
	for (int i=0; i < 8; i++)
		numbuf[i] = L'0';

	// set up a unicode string for the conversion routine
	UNICODE_STRING us;
	us.Length = 0;
	us.MaximumLength = sizeof(numbuf); // null-terminated!

	// determine the width of the number
	if ( number == 0 )
		reqWidth = 1;
	else
		for (N=number, reqWidth=0; N != 0; reqWidth++, N /= 16)
			;

	// set the buffer so that the number is right aligned
	us.Buffer = &numbuf[8-reqWidth];

	// do the conversion
	RtlIntegerToUnicodeString(number, 16, &us);

	// append the string
	return Append( PCWSTR(&numbuf[8-width]) );
}
