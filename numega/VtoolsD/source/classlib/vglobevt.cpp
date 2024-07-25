// VGlobalEvent.cpp - implementation of class VGlobalEvent
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

#include <vtoolscp.h>

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

extern "C" PVOID __cdecl createEventThunk(VGlobalEvent*, BYTE*, VOID (VGlobalEvent::*)(VMHANDLE,CLIENT_STRUCT*,PVOID));

// Function
//	VGlobalEvent::VGlobalEvent
//
// Syntax
//	VGlobalEvent::VGlobalEvent(PVOID refData)
//
// Parameters
//	refData		value passed to event handler
//
// Remarks
//	Constructs an instance of class VGlobalEvent.  Use class VGlobalEvent to
//	cause the VMM to call your event handler at a point where it is
//	safe to call non-async VMM services.  It is safe to allocate
//	VGlobalEvent objects with operator new at interrupt level.
//
VGlobalEvent::VGlobalEvent(PVOID refData)
{
	m_refData = refData;
}

// Function
//	VGlobalEvent::schedule
//
// Syntax
//	VOID VGlobalEvent::schedule()
//
// Remarks
//	Schedules a single invocation of member function handler. 
//
//	This member function is usually called during interrupt processing,
//	or whenever it is unsafe to use non-async VMM services.
//
VOID VGlobalEvent::schedule()
{
	PVOID pFunc;

	pFunc = createEventThunk(this, m_thunk, &VGlobalEvent::handler);
	m_handle = Schedule_Global_Event((PEvent_HANDLER) pFunc, m_refData, NULL);
}


// Function
//	VGlobalEvent::call
//
// Syntax
//	BOOL VGlobalEvent::call()
//
// Remarks
//	Schedules invocation of, or calls, member function handler. 
//
//	If a hardware interrupt that interrupted the VMM is being processed,
//	then the event will be scheduled.  Otherwise, member function handler 
//	is called immediately.
//
//	This member function is usually called during interrupt processing,
//	or whenever it is unsafe to use non-async VMM services.
//
// Return Value
//	Returns non-zero (TRUE) if the event is scheduled, as opposed
//	to being called immediately.
//
BOOL VGlobalEvent::call()
{
	PVOID pFunc = createEventThunk(this, m_thunk, &VGlobalEvent::handler);
	m_handle = Call_Global_Event((PEvent_HANDLER) pFunc, m_refData, NULL);

	return m_handle;
}

// Function
//	VGlobalEvent::cancel
//
// Syntax
//	VOID VGlobalEvent::cancel()
//
// Remarks
//	Cancels invocation of member function handler previously scheduled
//	by a call to either member function call or member function schedule.
//
//	Do not attempt to cancel events that have already occurred, as 
//	doing so may corrupt the VMM.
//
VOID VGlobalEvent::cancel()
{
	Cancel_Global_Event(m_handle);
}

// Function
//	VGlobalEvent::handler
//
// Syntax
//	VOID VGlobalEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, 
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
VOID VGlobalEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VGlobalEvent.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VGlobalEvent class is not defined correctly");
}
