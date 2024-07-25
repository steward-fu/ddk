// vlist.cpp - implementation of list class
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
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
//
#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VList::VList
//
// Syntax
//	VList::VList(DWORD nodesize, DWORD flags)
//
// Parameters
//	nodesize	Specifies the size of a node.  All nodes must be
//			a fixed size.
//	flags		Bit flags defined in vmm.h:
//	
//			LF_ASYNC	create a list usable at interrupt time
//			LF_USE_HEAP	newNode allocates from system heap
#ifdef WIN40
//			LF_SWAP		newNode allocates swappable memory
#endif
//			LF_ALLOC_ERROR	newNode returns null on failure
// Remarks
//	The VMM linked list services are optimized for speed.  Note that
//	all nodes for a particular list must be the same size.  The VMM
//	uses a speed-optimized pool for allocation if neither the LF_USE_HEAP 
//	nor the LF_SWAP options are used.
//
//	If LF_ALLOC_ERROR is not used, a failure to allocate a node results
//	in a fatal error.
//
//	The attach member function is most likely faster than the 
//	attachTail member function.
//
//	Storage for any item inserted into a list must be allocated by
//	the newNode member function, and deleted by the deleteNode member
//	function.  The most convenient way to do this is to define a class
//	for the node objects, and override the new and delete operators.
//	See vlist.h for example.
//
VList::VList(DWORD nodesize, DWORD flags)
{
	m_handle = List_Create(flags, nodesize);
}	

// Function
//	VList::~VList
//
// Syntax
//	VList::~VList()
//
// Remarks
//	Deallocates any existing list elements and frees the list.
//
//	If you have overridden the delete operator for the node class of
//	this list, and you require that your implementation of operator
//	delete() be called for each node as it is deleted, you must explicitly
//	delete the nodes to empty the list prior to calling this function.
//	If your implementation of operator delete() simply calls the
//	deleteNode() member function of the list, you can call this
//	function without emptying it first.
//
VList::~VList()
{
	if (m_handle != 0)
		List_Destroy(m_handle);
}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT


// Function
//	VList::newNode
//
// Syntax
//	PVOID VList::newNode()
//
// Remarks
//	Allocates storage for a new list node.  The size of the node 
//	is fixed by a parameter to the list constructor.
//
//	If the LF_USE_HEAP option was passed to the constructor, this
//	function will allocate memory for the node from the system heap.
//	If the LF_SWAP option was passed (not available in version 3.1),
//	the node is allocated in swappable memory.  If neither option
//	is used, allocation is from a special pool reserved for the list.
//	This provides the best performance.
//
//	It may be convenient to override the new operator for the class
//	that uses the list in order to ensure that memory is correctly
//	allocated. 
//
// Return Value
//	Returns a pointer to the allocated node.
//
//	If the LF_ALLOC_ERROR option was passed to the constructor, this
//	function returns NULL if the allocation fails.  Otherwise, an
//	allocation failure causes a fatal memory error.
//
PVOID VList::newNode()
{
	PVOID pNode;

	if (m_handle && ((pNode=List_Allocate(m_handle)) != NULL))
		return pNode;
	else
	{
		DEBUGEVENT(DBG_WARNING, "Failed to allocate list node");
		return NULL;
	}
}	

// Function
//	VList::deleteNode
//
// Syntax
//	VOID VList::deleteNode(PVOID pNode)
//
// Parameters
//	pNode	pointer to the node to delete
//
// Remarks
//	The node must have been allocated by the newNode member function.
//
VOID VList::deleteNode(PVOID pNode)
{
	if (m_handle)
		List_Deallocate(m_handle, pNode);
}	

// Function
//	VList::attach
//
// Syntax
//	VOID VList::attach(PVOID pNode)
//
// Parameters
//	pNode	pointer to node to attach to front of list
//
// Remarks
//	Attaches the given node to the front of the list, i.e., the
//	head of the list becomes pNode.  The memory for pointed to by 
//	pNode must have been allocated by newNode().
//
//	This function is likely to be slightly faster than attachTail().
//
VOID VList::attach(PVOID pNode)
{
	if (m_handle)
		List_Attach(m_handle, pNode);
}	

// Function
//	VList::attachTail
//
// Syntax
//	VOID VList::attachTail(PVOID pNode)
//
// Parameters
//	pNode	pointer to node to attach to tail of list
//
// Remarks
//	Attaches the given node to the tail of the list.  For long lists,
//	this function may be slightly slower than attach(). Memory for the
//	node must have been allocated by the newNode member function.
//
VOID VList::attachTail(PVOID pNode)
{
	if (m_handle)
		List_Attach_Tail(m_handle, pNode);
}	

// Function
//	VList::first
//
// Syntax
//	PVOID VList::first()
//
// Return Value
//	Returns a pointer to the node at the head of the list. If the
//	list is empty returns NULL.
//
PVOID VList::first()
{
	PVOID p;

	if (m_handle)
	{
		return List_Get_First(m_handle);
	}
	else
		return NULL;
}	

// Function
//	VList::next()
//
// Syntax
//	PVOID VList::next(PVOID pNode)
//
// Parameters
//	pNode	pointer to a list node for which the next node should be
//		be returned.
//
// Return Value
//	Returns a pointer to the node that follows pNode in the list.  If
//	pNode is at the tail of the list, returns NULL.
//
PVOID VList::next(PVOID pNode)
{
	PVOID pNext;

	if (m_handle)
	{
		return List_Get_Next(m_handle, pNode);
	}
	else
		return NULL;
}	

// Function
//	VList::insert
//
// Syntax
//	VOID VList::insert(PVOID pNodeInsert, PVOID pNodeAfter)
//
// Parameters
//	pNodeInsert	pointer to the node to be inserted
//	pNodeAfter	pointer to node after which to insert pNodeInsert
//
// Remarks
//	Inserts the node pointed to by pNodeInsert into the list following
//	the node pointed to by pNodeAfter. If pNodeAfter is NULL, pNodeInsert
//	is placed at the head of the list.
//
VOID VList::insert(PVOID pNodeInsert, PVOID pNodeAfter)
{
	List_Insert(m_handle, pNodeInsert, pNodeAfter);
}	

// Function
//	VList::remove
//
// Syntax
//	BOOL VList::remove(PVOID pNode)
//
// Parameters
//	pNode	pointer to the list node to be removed
//
// Remarks
//	Removes the specified node from the list. This function does
//	NOT deallocate storage for the node.
//
// Return Value
//	Returns TRUE if the node is successfully removed; otherwise
//	returns FALSE.
//
BOOL VList::remove(PVOID pNode)
{

	if (m_handle && List_Remove(m_handle, pNode))
		return TRUE;
	else
	{
		DEBUGEVENT(DBG_HINT, "Attempt to remove item not in list");
		return FALSE;
	}
}	

// Function
//	VList::removeFirst
//
// Syntax
//	PVOID VList::removeFirst()
//
// Remarks
//	Removes the node at the head of the list and returns its
//	address.  It does NOT deallocate storage for the node.
//
// Return Value
//	Returns the address of the node at the head of the list. If
//	the list is empty, returns NULL.
//
PVOID VList::removeFirst()
{
	PVOID pHead;

	if (m_handle)
	{
		pHead = List_Remove_First(m_handle);
		return pHead;
	}
	else
		return FALSE;
}	
