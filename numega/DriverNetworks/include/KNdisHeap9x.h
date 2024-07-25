// KNdisHeap.h - Heap classes based on W9X targets.
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

#ifndef __KNDISHEAP9X__
#define __KNDISHEAP9X__

///////////////////////////////////////////////////////////////////////
// KNdisHeap implementation: Platform Dependent
//
// NOTE: Implementation is stubbed to ordinal C++ heap for now.
//
// TODO: Optimize!
//

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
	Initialize(MaxItems, Tag);
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
	m_Size = sizeof(HeapBlock);
	UNREFERENCED_PARAMETERS2(MaxItems, Tag);
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
	m_Size=0;
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
template <class T> BOOLEAN KNdisHeap<T>::IsValid(void)
{
	return m_Size!=0;
}


///////////////////////////////////////////////////////////////////////
// Allocate block
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	In this implementation, user block T must be preceded by the pointer
//  to the container itself. KNdisHeapClient<T>::new/delete assume that.
template <class T> PVOID KNdisHeap<T>::alloc(void)
{
	return (PVOID) new char [m_Size];
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
	delete ((char*) (p));
}

VOID KNdisHeapFast::Initialize(USHORT MaxItems, size_t ItemSize, ULONG Tag)
{
	m_Size = ItemSize;
	UNREFERENCED_PARAMETERS2(MaxItems, Tag);
}

size_t KNdisHeapFast::ItemSize() const
{
	return m_Size;
}

#endif // __KNDISHEAP9X__
