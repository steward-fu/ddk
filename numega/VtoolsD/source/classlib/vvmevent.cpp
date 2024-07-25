// vvmevent.cpp - implementation of class VVMEvent
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

extern "C" PVOID __cdecl createEventThunk(VVMEvent*, BYTE*, VOID (VVMEvent::*)(VMHANDLE,CLIENT_STRUCT*,PVOID));

// Function
//	VVMEvent::VVMEvent
//
// Syntax
//	VVMEvent::VVMEvent(VMHANDLE hVM, PVOID refData)
//
// Parameters
//	hVM		handle of virtual machine in which event is to run
//	refData		value passed to event handler
//
// Remarks
//	Constructs an instance of class VVMEvent.  Use class VVMEvent to
//	cause the VMM to call your event handler at a point where it is
//	safe to call non-async VMM services.  It is safe to allocate
//	VVMEvent objects with operator new at interrupt level.
//
VVMEvent::VVMEvent(VMHANDLE hVM, PVOID refData)
{
	m_VM = hVM;
	m_refData = refData;
}

// Function
//	VVMEvent::schedule
//
// Syntax
//	VOID VVMEvent::schedule()
//
// Remarks
//	Schedules a single invocation of member function handler. The 
//	handler will be called in the context of the virtual machine whose
//	handle was specified in the call to the constructor. This may 
//	require a context switch.
//
//	This member function is usually called during interrupt processing,
//	or whenever it is unsafe to use non-async VMM services.
//
VOID VVMEvent::schedule()
{
	PVOID pFunc = createEventThunk(this, m_thunk, &VVMEvent::handler);

	m_handle = Schedule_VM_Event(m_VM, (PEvent_HANDLER) pFunc, m_refData, NULL);
}

// Function
//	VVMEvent::call
//
// Syntax
//	BOOL VVMEvent::call()
//
// Remarks
//	Schedules invocation of, or calls, member function handler. The 
//	handler will be called in the context of the virtual machine whose
//	handle was specified in the call to the constructor. This may 
//	require a context switch.
//
//	If a hardware interrupt that interrupted the VMM is being processed,
//	or virtual machine specified in the call to the constructor is not
//	the current virtual machine, then the event will be scheduled.
//	Otherwise, member function handler is called immediately.
//
//	This member function is usually called during interrupt processing,
//	or whenever it is unsafe to use non-async VMM services.
//
// Return Value
//	Returns non-zero (TRUE) if the event is scheduled, as opposed
//	to being called immediately.
//
BOOL VVMEvent::call()
{
	PVOID pFunc = createEventThunk(this, m_thunk, &VVMEvent::handler);
	m_handle = Call_VM_Event(m_VM, (PEvent_HANDLER) pFunc, m_refData, NULL);
	return m_handle;
}


// Function
//	VVMEvent::cancel
//
// Syntax
//	VOID VVMEvent::cancel()
//
// Remarks
//	Cancels invocation of member function handler previously scheduled
//	by a call to either member function call or member function schedule.
//
//	Do not attempt to cancel events that have already occurred, as 
//	doing so may corrupt the VMM.
//
VOID VVMEvent::cancel()
{
	Cancel_VM_Event(m_VM, m_handle);
}

// Function
//	VVMEvent::handler
//
// Syntax
//	VOID VVMEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, 
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
VOID VVMEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VVMEvent.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VVMEvent class is not defined correctly");
}

