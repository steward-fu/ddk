// kwmistr.cpp - WMI strings
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

#if _WDM_	// not supported for non-WDM drivers

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::operator new
//		description.
//
//	Parameters:
//		size
//			object size (unused)
//
//		Buffer
//			Buffer of Unicode characters comprising the string to be
//			copied to the new object.
//
//		stringSize
//			Size in bytes of string in Buffer
//
//	Return Value:
//		Returns pointer to the allocated (and initialized!) object
//
//	Comments:
//		You must use the default constructor with this operator new.
//
#if defined(_IA64_) || defined(_AMD64_)
void* KWmiString::operator new (size_t size, PWCHAR Buffer, USHORT stringSize)
#else
void* KWmiString::operator new (unsigned int size, PWCHAR Buffer, USHORT stringSize)
#endif
{
	USHORT sizeToAlloc = USHORT(sizeof(KWmiString) + stringSize);
	KWmiString* pNew = (KWmiString*) new (PagedPool) UCHAR[sizeToAlloc];

	if (pNew != NULL)
	{
		pNew->m_MaxSize = stringSize;
		pNew->Set(Buffer, stringSize);
	}

	return pNew;

	UNREFERENCED_PARAMETER(size);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::operator new
//		Overloaded new operator for class KWmiString
//
//	Parameters:
//		size
//			object size (unused)
//
//		pNullTerminatedString
//			Null terminated unicode string to be copied to object
//
//	Return Value:
//		Returns pointer to allocated (and initialized!) object
//
//	Comments:
//		You must use the default constructor with this operator new.
//
#if defined(_IA64_) || defined(_AMD64_)
void* KWmiString::operator new (size_t size, PCWSTR pNullTerminatedString)
#else
void* KWmiString::operator new (unsigned int size, PCWSTR pNullTerminatedString)
#endif
{
	USHORT stringSize = USHORT(ustrsiz(pNullTerminatedString));

	return new(const_cast<PWCHAR>(pNullTerminatedString), stringSize) KWmiString;

	UNREFERENCED_PARAMETER(size);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::operator new
//		Overloaded new operator for class KWmiString
//
//	Parameters:
//		size
//			object size (unused)
//
//		pUs
//			Pointer to UNICODE_STRING to be copied to object
//
//	Return Value:
//		Returns a pointer to the new object.
//
//	Comments:
//		You must use the default constructor with this operator new.
//
#if defined(_IA64_) || defined(_AMD64_)
void* KWmiString::operator new (size_t size, PUNICODE_STRING pUs)
#else
void* KWmiString::operator new (unsigned int size, PUNICODE_STRING pUs)
#endif
{
	return new( pUs->Buffer, pUs->Length) KWmiString;

	UNREFERENCED_PARAMETER(size);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::Set
//		Set the value of the string (i.e. store new characters in the string
//		object).
//
//	Parameters:
//		nullTerminatedString
//			Null terminated unicode string to be copied to object
//
//	Return Value:
//		Returns STATUS_SUCCESS, or STATUS_BUFFER_OVERFLOW if the object's
//		allocated buffer is not large enough to accommodate the string.
//
//	Comments:
//		The size of the null terminated string (not counting the terminator),
//		must be less than or equal to the size of the string used to initialize
//		the object.
//
NTSTATUS KWmiString::Set(PCWSTR nullTerminatedString)
{
	USHORT size = USHORT(ustrsiz(nullTerminatedString));

	if (size <= m_MaxSize)
	{
		memcpy(m_Data, nullTerminatedString, size);
		m_Size = size;
		return STATUS_SUCCESS;
	}
	else
	{
		return STATUS_BUFFER_OVERFLOW;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::Set
//		Set the value of the string (i.e. store new characters in the string
//		object).
//
//	Parameters:
//		Buffer
//			Buffer of Unicode characters comprising the string to be
//			copied to the new object.
//
//		Size
//			Size, in bytes, of the string at Buffer (do not count a
//			null terminator).
//
//	Return Value:
//		Returns STATUS_SUCCESS, or STATUS_BUFFER_OVERFLOW if the object's
//		allocated buffer is not large enough to accommodate the string.
//
//	Comments:
//		The size of the string as indicated by parameter Size
//		must be less than or equal to the size of the string used to initialize
//		the object.
//
NTSTATUS KWmiString::Set(PWCHAR Buffer, USHORT Size)
{
	if ( Size <= m_MaxSize )
	{
		memcpy(m_Data, Buffer, Size);
		m_Size = Size;
		return STATUS_SUCCESS;
	}
	else
	{
		return STATUS_BUFFER_OVERFLOW;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::Get
//		Set the value of the string (i.e. store new characters in the string
//		object).
//
//	Parameters:
//		buffer
//			Reference to a pointer to a buffer that receives the string data.
//			This function updates the pointer on output to point to the first
//			location past the last character written into the buffer.
//
//		MaxSizeToPut
//			On input, the USHORT value referenced by this parameter is the size
//			of the buffer. If the buffer is large enough to accommodate the
//			string, the location is unchanged on output. If not, then this
//			function sets the location to the number of bytes required to
//			accommodate the string.
//
//	Return Value:
//		Returns STATUS_SUCCESS, or STATUS_BUFFER_TOO_SMALL if the provided
//		buffer is not large enough to accommodate the string.
//
NTSTATUS KWmiString::Get(PWCHAR& buffer, USHORT& MaxSizeToPut)
{
	if ( Size() <= MaxSizeToPut )
	{
		*buffer++ = m_Size;
		memcpy(buffer, m_Data, m_Size);
		buffer += m_Size/sizeof(WCHAR);
		return STATUS_SUCCESS;
	}
	else
	{
		MaxSizeToPut = Size();
		return STATUS_BUFFER_TOO_SMALL;
	}
}

#endif //_WDM_
