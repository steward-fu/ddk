// KTA_ADDRESS.h
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

#if !defined(_KTA_ADDRESS_H__C61431B2_2CA9_4661_977E_FB7FE2BAC022__INCLUDED_)
#define _KTA_ADDRESS_H__C61431B2_2CA9_4661_977E_FB7FE2BAC022__INCLUDED_
#pragma once

////////////////////////////////////////////////////////////////////
// KTA_ADDRESS
//
// KTA_ADDRESS is a type-safe wrapper for TA_ADDRESS.
//
// Template parameter T is one of TDI_ADDRESS_XXX types defined in tdi.h.
//
// Use operator -> to access fields of type T
//
template <class T>
class KTA_ADDRESS
{
protected:
// TA_ADDRESS layout
	USHORT  AddressLength;
	USHORT  AddressType;
	T		Type;
public:
// Comparison with PTRANSPORT_ADDRESS. This can be used with KListBrowser
	int operator==(const PTRANSPORT_ADDRESS pTA) const;
	int operator==(const KTA_ADDRESS& ta) const;

// Assignment. NULL parameter zeroes the address
	KTA_ADDRESS& operator=(PTA_ADDRESS pTA);

// conversion
	operator PTA_ADDRESS() {return reinterpret_cast<PTA_ADDRESS> (this); }

// Attributes
	static ULONG Size() { return sizeof(TA_ADDRESS)-1+sizeof(T); }
	static ULONG Length() { return sizeof(T); }

// Verify type
	static bool IsKindOf(const PTA_ADDRESS pTA);

// Accessors
	T* operator->() { return &Type; }

// ctor/dtor
	KTA_ADDRESS();
//	~KTA_ADDRESS();

protected:
	// Must be specialized for each TDI_ADDRESS_
	static inline USHORT TypeId();
};


/////////////////////////////////////////////////////////////////////////////
// Implementations

template <class T>
KTA_ADDRESS<T>::KTA_ADDRESS()
{
	memset(&Type, 0, AddressLength=sizeof(T));
	AddressType = TypeId();
}

template <class T>
bool KTA_ADDRESS<T>::IsKindOf(const PTA_ADDRESS pTA)
{ return pTA && pTA->AddressType==TypeId() &&  pTA->AddressLength==sizeof(T); }

template <class T>
KTA_ADDRESS<T>& KTA_ADDRESS<T>::operator=(PTA_ADDRESS pTA)
{
  if (pTA) {
	  ASSERT(IsKindOf(pTA));
	  memcpy(&Type, pTA->Address, sizeof(Type));
  }
  else memset(&Type, 0, sizeof(T));
  return *this;
}

template <class T>
int KTA_ADDRESS<T>::operator==(const PTRANSPORT_ADDRESS pTA) const
{ return (IsKindOf(pTA)) ? memcmp(&Type, pTA->Address, sizeof(Type))==0 : 0; }

template <class T>
int KTA_ADDRESS<T>::operator==(const KTA_ADDRESS& ta) const
{   return memcmp(&Type, &ta.Type, sizeof(Type))==0;  }


///////////////////////////////////////////////////////////////////////////
// TypeId() Specializations. They map address structure to address id

USHORT KTA_ADDRESS<TDI_ADDRESS_IP>::TypeId() { return TDI_ADDRESS_TYPE_IP; }
USHORT KTA_ADDRESS<TDI_ADDRESS_APPLETALK>::TypeId() { return TDI_ADDRESS_TYPE_APPLETALK; }
USHORT KTA_ADDRESS<TDI_ADDRESS_NETBIOS>::TypeId() { return TDI_ADDRESS_TYPE_NETBIOS; }
USHORT KTA_ADDRESS<TDI_ADDRESS_NETBIOS_EX>::TypeId() { return TDI_ADDRESS_TYPE_NETBIOS_EX ; }
USHORT KTA_ADDRESS<TDI_ADDRESS_IPX>::TypeId() { return TDI_ADDRESS_TYPE_IPX; }
USHORT KTA_ADDRESS<TDI_ADDRESS_VNS>::TypeId() { return TDI_ADDRESS_TYPE_VNS; }
USHORT KTA_ADDRESS<TDI_ADDRESS_8022>::TypeId() { return TDI_ADDRESS_TYPE_8022; }
USHORT KTA_ADDRESS<TDI_ADDRESS_NETONE>::TypeId() { return TDI_ADDRESS_TYPE_NETONE; }
USHORT KTA_ADDRESS<TDI_ADDRESS_NS>::TypeId() { return TDI_ADDRESS_TYPE_NS; }
USHORT KTA_ADDRESS<TDI_ADDRESS_OSI_TSAP>::TypeId() { return TDI_ADDRESS_TYPE_OSI_TSAP; }


#endif  //(_KTA_ADDRESS_H__C61431B2_2CA9_4661_977E_FB7FE2BAC022__INCLUDED_)
