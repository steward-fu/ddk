// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vlist.h - include file for list class

#ifndef __vlist_h_
#define __vlist_h_

#include <vmachine.h>

/*
	List Class
	----------

	The class library provides a simple linked list class that
	encapsulates the Linked List Services provided by the VMM.
	The VMM's List Services are optimized for speed.  Accordingly,
	there are limitations on how they can be used. 

	First, all nodes in the list must be of exactly the same size.
	The size of a node is specified in the call to the list constructor.

	Second, memory allocation for nodes may only be performed by
	the list member function newNode().  This allows the VMM to add
	the necessary overhead fields for linkage and to more efficiently
	manage memory.

	Flags passed to the constructor specify how the newNode member
	function is to allocate memory.  There are two options:

	LF_USE_HEAP	Instructs the VMM to use system heap for allocation 
			of node storage.  If this option is NOT used, then
			the VMM uses a special pool of memory which is
			optimized for speed.

	LF_SWAP		Instructs the VMM to allocate nodes from swappable 
			memory.  The special speed optimized pool is NOT 
			used.  This option is not available for version 3.1


	Other constructor option flags are:

	LF_ASYNC	Instructs the VMM to construct a list that can be
			used during interrupt processing.  This flag is
			not compatible with LF_USE_HEAP or LF_SWAP.

	LF_ALLOC_ERROR	Instructs the newNode member function to return a 
			NULL pointer if a node cannot be allocated.  If
			this option is not used, failure to allocate a
			node is a fatal error.  This option is strongly 
			recommended.

	The option flags are defined in vmm.h.

	Allocation of nodes
	-------------------

	Storage for objects in lists must be allocated by the newNode()
	member function and deallocated by the deleteNode() member function
	of the list in question.  To ensure this, you should override the 
	new and delete operators of the node class. 

	example:

	#include <windows.h>
	#include <vmm.h>
	#include <vlist.h>

	// first declare the node class

	class VMyNode
	{
	public:
		VMyNode();
		void* operator new()
			{return m_pList->newNode();};
		void  operator delete();
			{m_pList->deleteNode(this);};

		int myNodeDataA;
		int myNodeDataB;	// whatever you store in a node
		int myNodeDataC;

		static VList* m_pList;	// a base list for this node type
	};

	static VList* VMyNode::m_pList; // declare outside of function { }

	// You must create a list for a node type before you instantiate a 
	// node. When you create the list, store a pointer to it in the
	// static m_pList member of the VMyNode class.  This allows
	// the new operator for the node class to call the newNode()
	// member function of the list.

	VMyNode::m_pList = new VList(sizeof(VMyNode));

	// Create a node using the new operator:

	VMyNode pNode = new VMyNode();

	// To delete a node:

	delete pNode;	// this will invoke VMyNode::m_pList->deleteNode().

	Note: it is permitted to have multiple lists of the same type
	of node object, as long as the static m_pList member of the node 
	class points to an existing list that was created for that class.

	Manipulating Lists
	------------------

	To add nodes to a list, use one of the following member functions:

	attach		attach a node to the head of the list
	attachTail	attach a node to the tail of the list
	insert		insert a node after a specified node

	(Note: attach is faster than attachTail.)


	To remove nodes from a list, use one of the following member 
	functions:

	remove		remove a specific node
	removeFirst	remove the head of the list (and return its address)

	(Note: Neither of the above function deallocates storage for the
	 node.)

	To iterate over a list, use the member function first() to get the
	list head, then use the member function next() to get successively 
	a pointer to each list node.
*/

// The list class

class VList 
{
public:
	VList(DWORD nodesize, DWORD flags=LF_ALLOC_ERROR);
	~VList();
	PNODE 	newNode();
	VOID 	deleteNode(PNODE);
	VOID	attach(PNODE);
	VOID 	attachTail(PNODE);
	PNODE	first();
	PNODE	next(PNODE);
	VOID	insert(PNODE, PNODE);
	BOOL	remove(PNODE);
	PNODE	removeFirst();

protected:
	DWORD m_handle; //TBD change to list handle
};

#endif

