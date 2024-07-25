// KListBrowser.h: Safe Lookup/Removal helper for KList classes
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

#if !defined(AFX_KNDISLISTBROWSER_H__5551B5C7_4457_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISLISTBROWSER_H__5551B5C7_4457_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Safe Lookup and Removal Helper for KList classes
//
// KListBrowser is a helper class which encapsulates common pattern
// of the thread-safe list browsing when using DW or DNW KList templates.
// Specifically, it provides an atomic way to perform a "lookup-and-remove"
// transaction. In addition, KListBrowser provides 2 extra utilities:
//
// (1) a safe keyed lookup (see LookupByKey)
// (2) a safe "for each" iteration over the list (see ForEach)
//
// LookupByKey() requires the node class T expose a '==' operator with
// the K type of argument. ForEach() allows to execute a unary operation
// for each node of the list in atomic manner.
//
// KListBrowser objects are instantiated on stack to facilitate
// member list processing.
//
// NOTE that all the Lookup/Remove methods in this class could've been added
// directly to KList classes as members. However, this would require a copy-and-paste
// implementation for all flavors of KList-s. The 2nd template parameter, L, allows to
// use single KListBrowser implementation for any flavor of KList.
//
template <class T, class L=KInterlockedList<T> >
class KListBrowser
{
	KList<T> m_BlackList;

	void Dispose(); // Purging the internal list

public:
	inline KListBrowser() {}
	inline ~KListBrowser() {}

// Lookup by node
	static bool Lookup(L& List, const T* Node);
	static bool Remove(L& List, const T* Node);

// Total removal with optional deletion of nodes
	KList<T>& RemoveAll(L& List, bool bDelete=false);

// Find at index from the head
	static T* At(L& List, int i);


// Node lookup by key
	template <typename K> static T* LookupByKey(L& List, K Key)
	{
		List.Lock();							// protect the loop
		T* node = List.HeadNoLock();
		while (node && !(*node == Key))
			node = List.NextNoLock(node);
		List.Unlock();
		return node;
	}

// Node lookup by function
	template <class F, typename K> static T* LookupByFunc(L& List, F Func, K Key)
	{
		List.Lock();							// protect the loop
		T* node = List.HeadNoLock();
		while (node && !(Func(node, Key)))
			node = List.NextNoLock(node);
		List.Unlock();
		return node;
	}

// Node removal by key
	template <typename K> static T* RemoveByKey(L& List, K Key)
	{
		List.Lock();							// protect the loop
		T* node = List.HeadNoLock();
		while (node && !(*node == Key))
			node = List.NextNoLock(node);
		if (node)
			List.RemoveNoLock(node);
		List.Unlock();
		return node;
	}

// Removal of all with given key with optional deleting the node objects
	template <typename K>  KList<T>& RemoveByKeyAll(L& List, K Key, bool bDelete=false)
	{
		List.Lock();							// protect the loop
		T* node = List.HeadNoLock();
		while (node)
		{
			T* thenode = node;
			node = List.NextNoLock(node);
			if (*thenode == Key)
			{
				List.RemoveNoLock(thenode);
				m_BlackList.InsertTailNoLock(thenode);
			}
		}
		List.Unlock();
		if (bDelete)
			Dispose();
		return m_BlackList;
	}

// Generic iteration over List with executing unary functor Func(node) for each
	template <class F> static void ForEach(L& List, F Func)
	{
		List.Lock();							// protect the loop
		T* node = List.HeadNoLock();
		while (node)
		{
			Func(node);							// NOTE: Func() might be executed on a raised IRQL
			node = List.NextNoLock(node);
		}
		List.Unlock();
	}

};

//////////////////////////////////////////////////////////////////
// KListEntry
// Helper class. Deriving from KListEntry makes the derived class
// "listable" by KList<> templates
struct KListEntry { LIST_ENTRY m_ListEntry; };

//////////////////////////////////////////////////////////////////
// KListBrowser implementations

template <class T, class L> inline
bool KListBrowser<T, L>::Lookup(L& List, const T* Node)
{
	List.Lock();							// protect the loop
	T* node = List.HeadNoLock();
	while (node && node!=Node)
		node = List.NextNoLock(node);
	List.Unlock();
	return node!=NULL;
}

template <class T, class L> inline
bool KListBrowser<T, L>::Remove(L& List, const T* Node)
{
	List.Lock();							// protect the loop
	T* node = List.HeadNoLock();
	while (node && node!=Node)
		node = List.NextNoLock(node);
	if (node)
		List.RemoveNoLock(node);
	List.Unlock();
	return node!=NULL;
}

template <class T, class L> inline
KList<T>& KListBrowser<T, L>::RemoveAll(L& List, bool bDelete)
{
	List.Lock();							// protect the loop
	T* thenode;
	T* node = List.HeadNoLock();
	while (node)
	{
		thenode = node;
		node = List.NextNoLock(node);
		List.RemoveNoLock(thenode);
		m_BlackList.InsertTailNoLock(thenode);
	}
	List.Unlock();
	if (bDelete)
		Dispose();
	return m_BlackList;
}

template <class T, class L> inline
void KListBrowser<T, L>::Dispose()
{
	T* node = m_BlackList.HeadNoLock();
	while (node)
	{
		T* thenode = node;
		node = m_BlackList.NextNoLock(node);
		delete thenode;
	}
}

template <class T, class L> inline
T* KListBrowser<T, L>::At(L& List, int i)
{
	List.Lock();							// protect the loop
	T* node = List.HeadNoLock();
	while (node && (i-- > 0))
		node = List.NextNoLock(node);
	List.Unlock();
	return node;
}


#endif // !defined(AFX_KNDISLISTBROWSER_H__5551B5C7_4457_11D3_8FA3_00C04F7445B7__INCLUDED_)
