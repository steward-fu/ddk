// KsObject.h: interface for the KsObject class.
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

#if !defined(AFX_KSOBJECT_H__CFF0948C_885F_4C43_AA3D_D6E430780CD6__INCLUDED_)
#define AFX_KSOBJECT_H__CFF0948C_885F_4C43_AA3D_D6E430780CD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KsTypeHelper
//
// Helper with KS object type mappings. Used in KsObject below.
// Parent types reflect AVStream object hierarcy.
//
template <typename T> struct KsTypeHelper
{
	typedef PVOID Descriptor;	// mapper to KS descriptor from KS object (see specializations below)
	typedef PVOID Parent;		// mapper to KS parent object from KS object (see specializations below)
};

// Specializations:
template<> struct KsTypeHelper<KSDEVICE>
{
	typedef KSDEVICE_DESCRIPTOR Descriptor;
	typedef PVOID Parent;// no parent
};
template<> struct KsTypeHelper<KSFILTERFACTORY>
{
	typedef KSFILTER_DESCRIPTOR Descriptor;
	typedef KSDEVICE			Parent;
};
template<> struct KsTypeHelper<KSFILTER>
{
	typedef KSFILTER_DESCRIPTOR Descriptor;
	typedef KSFILTERFACTORY		Parent;
};
template<> struct KsTypeHelper<KSPIN>
{
	typedef KSPIN_DESCRIPTOR Descriptor;
	typedef KSFILTER		 Parent;
};

// Generic structure "templatizer" helper

template <class S> struct KStructure
{
	S	m_desc;
	inline operator S* () { return &m_desc; }
	inline S* operator->() { return &m_desc; }
};



//////////////////////////////////////////////////////////////////////
// KsObject
//
// Base class for all system objects instantiated by KS factories.
// Template parameter T can be
// KSDEVICE, KSFILTER, KSPIN, KSFILTERFACTORY or any other
// object exposing .Context and .Bag
//
// Class provides an operator new() implementation, which
// automatically adds derived object to the underlying KS object's
// ObjectBag supplied as extra parameter. After successful construction
// the RefCount is 1.
//
// KsObject derived objects can't be delete()-d. Instead, AddRef() and Release()
// serve as lifetime management interface. The object gets deleted when
// Release() removes the last reference to the object from the underlying
// object bag. This results in a call to class' free() method, which
// provides destruction via the virtual destructor.
//
// KsObject makes use of AVStream object hierarchy to save parents context,
// which can be later accessed with Parent() accessor. This allows to gain
// access to the wrapping C++ parent object, e.g.
//
// MyPin* pin; // KsPin-derived
// . . .
// MyFilter* filter = reinterpret_cast<MyFilter*> (pin->Parent()->Context);
//
// NOTE: KsObject can be used ONLY as a root base class.
//
template <class T>
class KsObject
{
public:
	KsObject() {}
	virtual ~KsObject() {}

// type helpers
	typedef T	Type;
	typedef typename KsTypeHelper<T>::Descriptor Descriptor_t; // KSDESCRIPTOR_
	typedef typename KsTypeHelper<T>::Parent Parent_t;		  // parent descriptor type


// new() override.
// Usually called from the Wrapper to set up the contexts and
// add to the object bag of the Object created by AVStream

	void * __cdecl operator new(size_t nSize, T* Object, ULONG Tag='AOsK')
	{
		void* p = ::ExAllocatePoolWithTag(NonPagedPool, nSize, Tag);
		if (p) {
			NTSTATUS status = ::KsAddItemToObjectBag(Object->Bag, p, free);
			if (!NT_SUCCESS(status)) {
				::ExFreePool(p);
				return NULL;
			}
			// save object contexts
			reinterpret_cast<KsObject<T>*>(p)->m_ksobject = Object;
			// reinterpret_cast<KsObject<T>*>(p)->m_parent = (Parent_t*) Object->Context;
			Object->Context = p;
		}
		return p;
	}

// AddRef(): adds object reference to this (default) or specified bag

	void AddRef(KSOBJECT_BAG Bag=NULL)
	{
		::KsAddItemToObjectBag((Bag)?Bag:m_ksobject->Bag, this, free);
	}

// Release(): usually called from the Wrapper to decrement ref count.
// If no references were added, this call would result in a call
// to free().

	void Release()
	{
		::KsRemoveItemFromObjectBag(m_ksobject->Bag, this, TRUE);
	}

// accessors and convertors

	KSOBJECT_BAG Bag() const   { return m_ksobject->Bag; }
	Descriptor_t& Descriptor() const { return *m_ksobject->Descriptor; }
	Parent_t* Parent() const { return reinterpret_cast<Parent_t*>(KsGetParent(m_ksobject)); }

	operator T* () const { return m_ksobject; }
	operator KSOBJECT_BAG() const { return Bag(); }
	operator Descriptor_t* () const { return Descriptor(); }

// Restore C++ object from its KS context (should be casted by the caller)
	static inline PVOID Object(const T* pKSObject) { return pKSObject->Context; }

protected:
// default free from "Bag"
	static void free(void* p) { delete reinterpret_cast<T*> (p); }

	T*			m_ksobject;	// AVStream Object (e.g. PKSPIN)

//	Parent_t*	m_parent;	// Parent object in KS hierarchy (e.g. PKFILTER)

// delete() can be called from free() only. Use Release() to manage the life time

	void _cdecl operator delete(void * p)
	{
		if (p) ::ExFreePool(p);
	}
};


// Specializations

#endif // !defined(AFX_KSOBJECT_H__CFF0948C_885F_4C43_AA3D_D6E430780CD6__INCLUDED_)
