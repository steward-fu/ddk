// vv86cb.cpp - implementation of class VV86Callback 
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
//
#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

extern "C" PVOID __cdecl createCallbackThunk(VV86Callback*,BYTE*,BOOL(VV86Callback::*)(VMHANDLE,CLIENT_STRUCT*,PVOID));

LISTHANDLE VV86Callback::m_list = 0;


// Function
//	VV86Callback::VV86Callback
//
// Syntax
//	VV86Callback::VV86Callback(PVOID refData)
//
// Parameters
//	refData		A PVOID value that will be passed to the handler when 
//			it is called.
// Remarks
//	If the VMM cannot allocate a callback, the member function getAddr 
//	returns zero.
//
//	Unlike most event objects, the thunk for a Callback is not in the
//	object, but in separate storage. Thunks are kept in a list so that
//	callbacks can be reused after the Callback object is deleted. When
//	a callback object is deleted, the thunk is moved to a list whose 
//	head is a static member of the callback class. At allocation time,
//	the list is searched for a thunk that targets the same address, and
//	if found, reuses it. In this way, callback objects can be created
//	and freed without consuming the system's callbacks, provided that
//	the same handler address is reused.
//
VV86Callback::VV86Callback(PVOID refData)
{
	PVOID pFunc;

// First try to find an unused callback 

	m_Node = AllocFromList(m_list);

// If found, use it

	if (m_Node != 0)
	{
		m_dwCBAddr = m_Node->m_CBaddr;
		createCallbackThunk(this, m_Node->m_thunk,
					&VV86Callback::handler);
	}

// If there is none, allocate a new one.

	else 
	{
		m_Node = NewNode(&m_list);
		if (m_Node == 0)       	// if no node could be allocated
		{
			m_dwCBAddr = 0;
			return;
		}

		pFunc = createCallbackThunk(this, m_Node->m_thunk,
					&VV86Callback::handler);

		m_dwCBAddr = Allocate_V86_Call_Back(refData, (PPMCall_HANDLER) pFunc, NULL);

		if (m_dwCBAddr)
			m_Node->m_CBaddr = m_dwCBAddr;

		else
			List_Deallocate(m_list, m_Node);
	}
}


// Function
//	VV86Callback::~VV86Callback
//
// Syntax
//	VV86Callback::~VV86Callback(VOID)
//
// Remarks
//	When a callback is freed, move the node to the "used" list for
//	later re-use.
//	
VV86Callback::~VV86Callback(VOID)
{
	List_Attach(m_list, m_Node);	
}



#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Unlike most classes, this dummy entry point has to be in the locked data
// segment.  The reason is that when the constructor takes the address
// of the handler, it gets the address of a compiler-generated thunk that  
// dispatches through the vtab.  At the time the constructor is called, 
// the vtab pointer is to a base class (VV86Callback) vtab, not that of the
// derived class.  The vtab pointer is not overwritten until constructors
// of the derived classes are called.  The compiler generates the thunk in
// the segment where it finds the function.  To be safe, we make it the
// locked segment.  This is not an issue for most classes because in most
// all other classes, the handler address is not passed to the VMM by the
// constructor. Rather, it is passed in a member function hook, which is
// called after the vtab pointer is in its final state.  This makes it 
// possible to examine the thunk and resolve the "un-thunked" address of
// the handler before passing it to the VMM.

BOOL VV86Callback::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VV86Callback.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VV86Callback class is not defined correctly");
#ifdef DEBUG
	setReturn();
#endif
	return FALSE;
}

#ifdef __BORLANDC__
VOID VV86Callback::setReturn()
{
        Simulate_Far_Ret();
}
#endif
