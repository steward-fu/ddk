// kwmistr.h - string support for WMI
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

#if _WDM_

#ifndef __KWMISTR_H__
#define __KWMISTR_H__

//////////////////////////////////////////////////////////////////////////////
// Overview
//
// Class KWmiString simplifies string handling for WMI operations. The class
// aims to handle variably sized strings with minimal heap fragmentation, and,
// as a result, the usage is slightly unconventional. Specifically, the class
// has several forms of operator new, which enable the string to be stored
// contiguously with control information. Instances of this class are thus
// variably sized.
//
// The class is typically used for WMI data blocks that reference string
// information. Because strings vary in length, methods of template KWmiDataBlock
// must be specialized to handle these types. The driver writer will find
// it convenient to store pointers to KWmiStrings (allocated with operator new)
// in the structures that underlie the WMI data blocks containing strings.
//
// For example, suppose there is a struct corresponding to a MOF class whose
// only member is a 'string'. That struct might would look like this:
//
//	struct StringBlockData
//	{
//		KWmiString* m_String;
//	};
//
// The device object would include a data member of type KWmiDataBlock<StringBlockData>.
//
// The Query and Set members of KWmiDataBlock<StringBlockData> would have to
// be specialized by the driver writer. The member functions of KWmiString make it
// simple to move the string to and from WMI.
//

#pragma warning ( push )
#pragma warning (disable: 4200 )

class KWmiString
{
public:
// Constructors
	KWmiString(void);
	KWmiString(USHORT MaxSize);
	VOID Initialize(USHORT MaxSize);

// Operator new
#if defined(_IA64_) || defined(_AMD64_)
	void* operator new (size_t size, PCWSTR pNullTerminatedString);
	void* operator new (size_t size, PWCHAR Buffer, USHORT stringSize);
	void* operator new (size_t size, PUNICODE_STRING pUs);
#else
	void* operator new (unsigned int size, PCWSTR pNullTerminatedString);
	void* operator new (unsigned int size, PWCHAR Buffer, USHORT stringSize);
	void* operator new (unsigned int size, PUNICODE_STRING pUs);
#endif

// Methods
	NTSTATUS Set(PCWSTR nullTerminatedString);
	NTSTATUS Set(PWCHAR Buffer, USHORT Size);
	NTSTATUS Get(PWCHAR& buffer, USHORT& MaxSizeToGet);
	USHORT Size(void);

protected:
// data
	USHORT m_Size;		// current size in bytes of associated string
	USHORT m_MaxSize;	// allocated size for associated string
	USHORT m_Data[];	// variable string data, must be LAST!
};

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::KWmiString
//		Ctor for class KWmiString
//
//	Comments:
//		This is the default ctor, and is the only one that should be
//		used when the object is constructed using operator new.
//
inline KWmiString::KWmiString(void)
{
	// assume initialized by operator new
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::KWmiString
//		Alternate ctor for class KWmiString
//
//	Parameters:
//		MaxSize
//			Value used to initialize data member m_MaxSize
//
//	Comments:
//		The value given for m_MaxSize does not cause the ctor to allocate
//		a corresponding amount of storage. This ctor is provided for callers
//		who embed a KWmiString inside a struct, followed by additional structure
//		members that serve as storage for the variably sized data.
//
inline KWmiString::KWmiString(USHORT MaxSize)
{
	m_MaxSize = MaxSize;
	m_Size = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::Initialize
//		Initialize a WMI string object
//
//	Parameters:
//		MaxSize
//			Value used to initialize data member m_MaxSize
//
//	Comments:
//		Use this function to change the max size setting for the string. Note
//		that this would only be called when the alternate ctor was used to
//		construct the object. The recommended way to create KWmiString objects
//		is with the default constructor and one of the overloaded forms of operator
//		new.
//
inline VOID KWmiString::Initialize(USHORT MaxSize)
{
	m_MaxSize = MaxSize;
	m_Size = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiString::Size
//		Return the size required to store the string and its size for
//		a WMI query.
//
//	Return Value:
//		Returns the size of the Unicode string (in bytes), plus the size of
//		the count word that precedes the string data. The count is stored
//		in a USHORT.
//
inline USHORT KWmiString::Size(void)
{
	return USHORT(m_Size + sizeof(WCHAR));
}

#pragma warning ( pop )

#endif // __KWMISTR_H__
#endif // _WDM_
