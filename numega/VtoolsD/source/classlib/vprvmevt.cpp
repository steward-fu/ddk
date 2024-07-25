// VPriorityVMEvent.cpp - implementation of class VPriorityVMEvent
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

extern "C" PVOID __cdecl createPriorityEventThunk(VPriorityVMEvent*, BYTE*, VOID (VPriorityVMEvent::*)(VMHANDLE,CLIENT_STRUCT*,PVOID,BOOL));

// Function
//	VPriorityVMEvent::VPriorityVMEvent
//
// Syntax
//	VPriorityVMEvent::VPriorityVMEvent(VMHANDLE hVM,  DWORD boost, PVOID refData,
//		DWORD timeout, DWORD flags)
//
// Parameters
//	hVM		handle of virtual machine in which event is to run
//	boost		priority boost to apply to VM
//	refData		value passed to event handler
//	timeout		timeout value in msec
//	flags		option bits
//
// Remarks
//	Constructs an instance of class VPriorityVMEvent.  Use class VPriorityVMEvent to
//	cause the VMM to call your event handler at a point where it is
//	safe to call non-async VMM services.  It is safe to allocate
//	VPriorityVMEvent objects with operator new at interrupt level.
//
VPriorityVMEvent::VPriorityVMEvent(VMHANDLE hVM,  DWORD boost, PVOID refData,
		DWORD timeout, DWORD flags)
{
	m_VM = hVM;
	m_refData = refData;
	m_timeout = timeout;
	m_flags = flags;
	m_boost = boost;
}

// Function
//	VPriorityVMEvent::call
//
// Syntax
//	BOOL VPriorityVMEvent::call()
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
BOOL VPriorityVMEvent::call()
{
	PVOID pFunc = createPriorityEventThunk(this, m_thunk, &VPriorityVMEvent::handler);

	m_handle = Call_Priority_VM_Event(m_boost, m_VM, m_flags,  m_refData, 
				(PPriorityVMEvent_HANDLER) pFunc, m_timeout,
					  NULL);
	return m_handle;
}


// Function
//	VPriorityVMEvent::cancel
//
// Syntax
//	VOID VPriorityVMEvent::cancel()
//
// Remarks
//	Cancels invocation of member function handler previously scheduled
//	by a call to either member function call or member function handler.
//
//	Do not attempt to cancel events that have already occurred, as 
//	doing so may corrupt the VMM.
//
VOID VPriorityVMEvent::cancel()
{
	Cancel_Priority_VM_Event(m_handle);
}

// Function
//	VPriorityVMEvent::handler
//
// Syntax
//	VOID VPriorityVMEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, 
//		PVOID refData)
//
// Parameters
//	hVM		handle of current virtual machine
//	pRegs		pointer to client register structure
//	refData		reference value passed to constructor
//	bTimeOut	TRUE if event was not delivered in time
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
//	If bTimeOut is non-zero, the VMM could not schedule the event in
//	the specified virtual machine within the timeout limit specified
//	in the call to the constructor.  In this case, the parameter hVM
//	may contain an invalid virtual machine handle.  Use the VMM service
//	Get_Cur_VM_Handle to obtain the current virtual machine handle.
//
VOID VPriorityVMEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs,
	PVOID refData, BOOL bTimeOut)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VPriorityVMEvent.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VPriorityVMEvent class is not defined correctly");
}


