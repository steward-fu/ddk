// vthrdevt.cpp - implementation of class VThreadEvent
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

#ifdef WIN40

#include <vtoolscp.h>

extern "C" PThreadEvent_HANDLER __cdecl createThreadEventThunk(VThreadEvent*, BYTE*, VOID (VThreadEvent::*)(THREADHANDLE,VMHANDLE,CLIENT_STRUCT*,PVOID));

// Function
//	VThreadEvent::VThreadEvent
//
// Syntax
//	VThreadEvent::VThreadEvent(THREADHANDLE hThread, PVOID refData)
//
// Parameters
//	hThread		handle of virtual machine in which event is to run
//	refData		value passed to event handler
//
// Remarks
//	Constructs an instance of class VThreadEvent.  Use class VThreadEvent to
//	cause the VMM to call your event handler in the context of a 
//	particular thread.  It is safe to allocate VThreadEvent objects with 
//	operator new at interrupt level.
//
VThreadEvent::VThreadEvent(THREADHANDLE hThread, PVOID refData)
{
	m_Thread = hThread;
	m_refData = refData;
}

// Function
//	VThreadEvent::schedule
//
// Syntax
//	VOID VThreadEvent::schedule()
//
// Remarks
//	Schedules a single invocation of member function handler. The 
//	handler will be called in the context of the thread whose
//	handle was specified in the call to the constructor. 
//
VOID VThreadEvent::schedule()
{
	PThreadEvent_HANDLER pFunc = createThreadEventThunk(this, m_thunk, &VThreadEvent::handler);
	m_handle = Schedule_Thread_Event(m_Thread,  pFunc,  m_refData, NULL);
}

// Function
//	VThreadEvent::cancel
//
// Syntax
//	VOID VThreadEvent::cancel()
//
// Remarks
//	Cancels invocation of member function handler previously scheduled
//	by a call to either member function call or member function schedule.
//
//	Do not attempt to cancel events that have already occurred, as 
//	doing so may corrupt the VMM.
//
VOID VThreadEvent::cancel()
{
	Cancel_Thread_Event(m_Thread, m_handle);
}

// Function
//	VThreadEvent::handler
//
// Syntax
//	VOID VThreadEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, 
//		PVOID refData)
//
// Parameters
//	hVM	handle of current virtual machine
//	pRegs	pointer to client register structure
//	refData	reference value passed to constructor
//
// Remarks
//	The handler runs in respone to a call to either member function
//	schedule or member function call.  The handler is not restricted
//	from using non-async VMM services.
//
//	It is recommended to have the handler set data member m_handle to
//	zero.  This avoids accidental attempts to cancel the event after
//	the handler has been invoked.
//
VOID VThreadEvent::handler(THREADHANDLE hThread, VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VThreadEvent.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VThreadEvent class is not defined correctly");
}

#endif

