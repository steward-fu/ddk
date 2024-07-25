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
// vcallbak.cpp - implementation of class VCallback 
//
//
#include <vtoolscp.h>

// Function
//	AllocFromList - get a callback node from the used list
//
// Syntax
//	CALLBACKNODE* VCallback::AllocFromList(LISTHANDLE l)
//
// Parameters
//	l	Handle of list from which to allocate
//
// Remarks
//	Instead of allocating a new callback from the system every time,
//	reuse those that have been free. The destructor stores these in
//	the used list.
//
CALLBACKNODE* VCallback::AllocFromList(LISTHANDLE l)
{
	if (l != 0)
		return (CALLBACKNODE*)List_Remove_First(l);
	else
		return 0;
}

// Function
//	NewNode - create a new callback list node
//
// Syntax
//	CALLBACKNODE* VCallback::NewNode(LISTHANDLE* pl)
//
// Parameters
//	pl	Pointer to handle of list for which to create a node
//
// Remarks
//	Instead of allocating a new callback from the system every time,
//	reuse those that have been free. The destructor stores these in
//	the used list. This routine creates a new node in the specified 
//	list. If the list is NULL, it creates the list.
//
//	Note that it does not attach the node to the list -- this is not
//	done until the callback is deleted.
//
CALLBACKNODE* VCallback::NewNode(LISTHANDLE* pl)
{
	if (*pl == 0)
		*pl = List_Create(LF_ALLOC_ERROR|LF_USE_HEAP, sizeof(CALLBACKNODE));

	if (*pl == 0)
		return NULL;

	return (CALLBACKNODE*)List_Allocate(*pl);
}

#ifdef __BORLANDC__

VOID VCallback::setReturn()
{
        Simulate_Far_Ret();
}

#endif

// The function in the module is provide a check for misdefined handler 
// overrides.  Since this is effectively a dummy entry point, it is located 
// in the init segment where it will be flushed after initialization.

#include INIT_CODE_SEGMENT
#include INIT_DATA_SEGMENT

// Function
//	VCallback::handler
//
// Syntax
//	BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
//
// Parameters
//	hVM	Handle of the current virtual machine
//	pRegs	Pointer the client's register structure
//	refData	Reference data passed to constructor
//
// Remarks
//	Override this function in the class you derive from a VCallback
//	descendant. The callback handler is invoked when an application calls 
//	or jumps to the callback vector.
//
//	When the handler is invoked, the client CS:eIP is at the callback
//	address, and the return information is on the stack.  If you simply
//	return from the handler without modifying the client stack, 
//	execution will resume at the callback address, and the handler will
//	be immediately invoked again.  The normal action to take in the 
//	handler is to call member function setReturn, which will effect
//	a return from the call.
//
// Return Value
//	The return value is not defined.  Derived classes may apply whatever 
//	semantics are desired.
//
BOOL VCallback::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VCallback

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VCallback class is not defined correctly");
#ifdef DEBUG
	setReturn();
#endif
	return FALSE;
}
