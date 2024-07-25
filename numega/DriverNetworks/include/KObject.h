// KObject.h: interface for the KObject class.
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

#if !defined(AFX_KOBJECT_H__8534F9A4_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_KOBJECT_H__8534F9A4_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////
// KObject
//
// KObject is a base class for all objects having a representation inside
// a system component (such as NDIS).
//
// KObject represents an object within the system component, which is accessed via a handle.
// System components (such as NDIS) expose their internal objects (such as
// Adapter) via handles. The component's client usually create its own
// internal "description block" bound to this object. The address of the
// description block is reffered to as "context". The "handle" and the "context"
// thus form a pair that allow the system and client component identify
// their objects when passing requests or callbacks to each other.
//
// The KObject template abstracts this commonly used interaction pattern
// between a client and system components.
//
// Paremeters:
//		Handle_t	-	Handle type (usually, NDIS_HANDLE)
//		Signature	-	4-byte signature used in checked builds only
//
// NOTE: The signature could be used to (1) check the validity of the object (IsValid)
// and (2) help in debugging. If 'this' points to a KObject-derived object, usually, the
// signature is found at offset +4 from 'this' (following the vftable pointer). For
// DriverNetworks classes signatures start with "kn"; e.g. it would be 'knpp' for
// KNdisPacketPool, which is easily seen on a debugger such as NuMega SoftIce. When
// a KObject-derived object gets destroyed, the memory location with the signature gets
// value of "knxx".
//
template<typename Handle_t, const ULONG Signature='kn..'>
class KObject
{
	SAFE_DESTRUCTORS
public:
// Constructor
	KObject();
// Destructor
	~KObject();

// Assign a system handle
	inline void Assign(Handle_t h)  { m_Handle=h; }

// Convert to system handle
	inline operator Handle_t() const { return m_Handle;}

// Retrieve "context" (the object pointer itself)
	inline PVOID Context() const { return this; }

// Check signature and construction status
	BOOLEAN IsValid() const;

// Retrieve pointer to the handle (for friendly classes which might assign indirectly)
	inline Handle_t* HandlePTR() { return &m_Handle; }

protected:
	// To be called by derived class when construction failed
	void Invalidate();

#if DBG
	ULONG		m_Signature;
	// convenience macro to allow 'abcd' form of use at little endian machines
	static inline ULONG _Signature() 	{ return
		 ((Signature>>24) & 0xFF) | ((Signature>>8) & 0xFF00) | ((Signature<<8) & 0xFF0000) | ((Signature<<24) & 0xFF000000);
	}

#endif

	// Underlying system handle
	Handle_t	m_Handle;
};

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


template<typename Handle_t, const ULONG Signature>
inline
KObject<Handle_t,Signature>::KObject()
{
	Invalidate();
#if DBG
	m_Signature = _Signature();
#endif
}

template<typename Handle_t, const ULONG Signature>
inline
KObject<Handle_t,Signature>::~KObject()
{
#if DBG
	m_Signature = 'XXXX';
#endif
}

template<typename Handle_t, const ULONG Signature>
inline BOOLEAN
KObject<Handle_t,Signature>::IsValid() const
{
	return	m_Handle != Handle_t(-1)
#if DBG
	&& m_Signature == _Signature()
#endif
		;
}

template<typename Handle_t, const ULONG Signature>
inline void
KObject<Handle_t,Signature>::Invalidate()
{
	m_Handle = Handle_t(-1);
}

//////////////////////////////////////////////////
// Generic object checks
//
namespace _KNDIS {
	// Check the validity of pointer to KObject-drived type T
	template<class T> inline BOOLEAN
	IsValid(const T* t) { return (t)?t->IsValid():false; }

};

#endif // !defined(AFX_KOBJECT_H__8534F9A4_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
