// KNdisHeap.h - Heap classes based on NDIS lookaside lists.
//				 Implementation is replicated from DriverWorks' KHeap
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

#ifndef __KNDISHEAP__
#define __KNDISHEAP__

#if (KNDIS_DDK_VERSION < 5) && !defined(KNDIS_W9X) && !defined(NT_INCLUDED)
#include <KNdisLookaside.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// KNdisHeap
//
// Heap allocator based on non-paged look-aside lists.
//
// NOTE on NT 4 Implementation
// ===========================
//
// Lookaside lists are not exposed in NDIS 4 but present in NT.
// File KNdisLookaside.h defines missing prototypes and structs for NT 4 DDK builds.
//
// NOTE on W9X Implementation
// ==========================
//
// Starting from W98SE WDM has support for lookaside lists (ExXxxNPagedLookasideList).
// Older versions does not have it. Furthermore, VxD targets can't use it at all.
// For these cases file KNdisHeap9x.h implements the class for W9X targets.
// The inclusion of the WDM (ExX...) vs 9X-specific implementaion is governed by
// the following preprocessor symbols:
//
// Target				KNDIS_W9X_COMPATIBLE	KNDIS_9X		NDIS_WDM
// ------				--------------------	--------		---------
// WDM miniport				1					not defined			1
// VxD or W95 miniport		1					defined				0
//
// Setting NDIS_WDM to 1 defaults to using the native lookaside lists.
//
template <class T> class KNdisHeap
{
public:
	struct HeapBlock
	{
		KNdisHeap<T>* m_pList;
		UCHAR m_t[sizeof(T)];
	};

// Construction
public:
	SAFE_DESTRUCTORS
	inline KNdisHeap(void);
	inline KNdisHeap(USHORT MaxItems, ULONG Tag=0);
	inline VOID Initialize(USHORT MaxItems, ULONG Tag=0);
	inline ~KNdisHeap(void);
	inline VOID Invalidate(void);
	inline BOOLEAN IsValid(void) const;

// Methods
public:
	inline PVOID alloc(void);
	inline VOID free(PVOID p);

// Implementation Data
protected:
#if !defined(KNDIS_W9X)
	NPAGED_LOOKASIDE_LIST m_Lal;
#else
	size_t	m_Size;
#endif
};

///////////////////////////////////////////////////////////////////////////////
// KNdisHeapClient
//
template <class T> class KNdisHeapClient
{
public:
	struct HeapBlock
	{
		KNdisHeap<T>* m_pList;
		UCHAR m_t[1];
	};

// Construction
public:
	SAFE_DESTRUCTORS;
	inline void * __cdecl operator new(size_t nSize, KNdisHeap<T>* pList);
	inline void __cdecl operator delete(void* p);
};

///////////////////////////////////////////////////////////
// KNdisHeapFast
//
// Simple non-templatized heap for run-time defined sizes.
// Should use alloc() and free() only.
//
// NOTE: It should've really been the base of the "safe" one...
//
class KNdisHeapFast : public KNdisHeap<char>
{
public:
	inline KNdisHeapFast() : KNdisHeap<char>() {}
	inline KNdisHeapFast(USHORT MaxItems, size_t ItemSize, ULONG Tag=0)
		{ Initialize(MaxItems, ItemSize, Tag); }
	inline VOID Initialize(USHORT MaxItems, size_t ItemSize, ULONG Tag=0);

	inline size_t ItemSize() const;
};


//////////////////////////////////////////////////////////////////////
// Implementations


///////////////////////////////////////////////////////////////////////
// KNdisHeapClient implementation: platform independent

///////////////////////////////////////////////////////////////////////
// Override of operator new
//
// Parameters:
// 	nSize                - number of bytes to allocate
// 	pList                - pointer to heap object from which to allocate
// Returns:
// 	Returns a pointer to the allocated storage
// Comments:
// 	none
template <class T>
void * __cdecl KNdisHeapClient<T>::operator new(
	size_t nSize,
	KNdisHeap<T>* pList)
{
	UNREFERENCED_PARAMETER(nSize); // yak!
	HeapBlock* pBlock = (HeapBlock*)pList->alloc();
	if (pBlock)
	{
		pBlock->m_pList = pList;
		return pBlock->m_t;
	}
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////
// Override of operator delete
//
// Parameters:
// 	p                    - pointer to block to be freed
// Returns:
// 	none
// Comments:
// 	none
template <class T> void __cdecl KNdisHeapClient<T>::operator delete(void* p)
{
	HeapBlock* q = (HeapBlock*)((PUCHAR)p - sizeof(KNdisHeap<T>*));
	ASSERT ( (void*)q->m_t == p);

	q->m_pList->free(q);
}


///////////////////////////////////////////////////////////////////////
// KNdisHeap implementation: Platform Dependent
//
// NOTE: NT/2K implementation is based on kernel lookaside lists.
//		 See KNdisHeap9x.h for W9X.
//

#if !defined(KNDIS_W9X)
///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
template <class T> KNdisHeap<T>::KNdisHeap(void)
{
	NdisZeroMemory(&m_Lal, sizeof(m_Lal));
}


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	MaxItems             -
// 	Tag=0                -
// Returns:
// 	none
// Comments:
// 	none
template <class T> KNdisHeap<T>::KNdisHeap(USHORT MaxItems, ULONG Tag)
{
	NdisZeroMemory(&m_Lal, sizeof(m_Lal));
	NdisInitializeNPagedLookasideList(
		&m_Lal,
		NULL,
		NULL,
		0,
		sizeof(HeapBlock),
		Tag,
		MaxItems
		);
}

///////////////////////////////////////////////////////////////////////
// Initializes the object
//
// Parameters:
// 	MaxItems             -
// 	Tag=0                -
// Returns:
// 	none
// Comments:
// 	Call if default constructor was used
template <class T> VOID KNdisHeap<T>::Initialize(USHORT MaxItems, ULONG Tag)
{
	if (IsValid())
	{
		ASSERT(m_Lal.L.Size == 0);
		Invalidate();
	}
	NdisInitializeNPagedLookasideList(
		&m_Lal,
		NULL,
		NULL,
		0,
		sizeof(HeapBlock),
		Tag,
		MaxItems
		);
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
template <class T> KNdisHeap<T>::~KNdisHeap(void)
{
	if (IsValid()) Invalidate();
}


///////////////////////////////////////////////////////////////////////
// Invalidate the object
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	Inverse of Initialize
template <class T> VOID KNdisHeap<T>::Invalidate(void)
{
	if (m_Lal.L.Size != 0)
	{
		NdisDeleteNPagedLookasideList(&m_Lal);
		NdisZeroMemory(&m_Lal, sizeof(m_Lal));
	}
}


///////////////////////////////////////////////////////////////////////
// Test object validity
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
template <class T> BOOLEAN KNdisHeap<T>::IsValid(void) const
{
	return (m_Lal.L.Size != 0);
}


///////////////////////////////////////////////////////////////////////
// Allocate block from lookaside list
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
template <class T> PVOID KNdisHeap<T>::alloc(void)
{
	return NdisAllocateFromNPagedLookasideList(&m_Lal);
}


///////////////////////////////////////////////////////////////////////
// Free block
//
// Parameters:
// 	p                    - block to be freed
// Returns:
// 	none
// Comments:
// 	none
template <class T> VOID KNdisHeap<T>::free(PVOID p)
{
	NdisFreeToNPagedLookasideList(&m_Lal, p);
}

////////////////////////////////////////////////////////////////////
// KNdisHeapFast::Initialize
//
VOID KNdisHeapFast::Initialize(USHORT MaxItems, size_t ItemSize, ULONG Tag)
{
	NdisZeroMemory(&m_Lal, sizeof(m_Lal));
	NdisInitializeNPagedLookasideList(
		&m_Lal,
		NULL,
		NULL,
		0,
		ItemSize,
		Tag,
		MaxItems
		);
	ASSERT(m_Lal.L.Size == ItemSize);
}

size_t KNdisHeapFast::ItemSize() const
{
	return (size_t) m_Lal.L.Size;
}


#else
#include <KNdisHeap9x.h>
#endif // !KNDIS_W9X

#endif // __KNDISHEAP__
