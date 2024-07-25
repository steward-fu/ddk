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
// vappy.cpp - implementation of class VAppyTimeEvent

#ifdef WIN40
#include <vtoolscp.h>

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

extern "C" APPY_CALLBACK __cdecl createAppyTimeEventThunk(VAppyTimeEvent*, BYTE* m_thunk, VOID (VAppyTimeEvent::*pHandler)(PVOID,DWORD));

// Function
//	VAppyTimeEvent::VAppyTimeEvent - constructor
//
// Syntax
//	VAppyTimeEvent::VAppyTimeEvent(DWORD refData)
//
// Parameters
//	refData		passed to appy time event handler when invoked
//
// Remarks
//	Constructs an instance of VAppyTimeEvent. As with classes derived
//	from VEvent, the instance is automatically destroyed when the
//	handler exits.
//
VAppyTimeEvent::VAppyTimeEvent(PVOID refData, DWORD flags, DWORD msecTimeout)
{
	m_refData = refData;
	m_flags = flags;
	m_timeout = msecTimeout;
	m_handle = 0;
}

// Function
//	VAppyTimeEvent::schedule - schedule application time event
//
// Syntax
//	BOOL VAppyTimeEvent::schedule()
//
// Parameters
//	refData		passed to appy time event handler when invoked
//
// Remarks
//	Schedules the VAppyTimeEvent. When the system is in the application
//	state, member function handler will be called in the context of the
//	system VM.
//
// Returns
//	Returns TRUE if the event is successfully scheduled.
//
BOOL VAppyTimeEvent::schedule()
{
	APPY_CALLBACK phand = createAppyTimeEventThunk(this, m_thunk, &VAppyTimeEvent::handler);
	m_handle = (EVENTHANDLE)_SHELL_CallAtAppyTime(phand, m_refData, m_flags, m_timeout);

	return (m_handle != 0);
}

// Function
//	VAppyTimeEvent::cancel - cancel a previously scheduled application 
//				 time event
//
// Syntax
//	VOID VAppyTimeEvent::cancel()
//
// Remarks
//	Cancels a previously scheduled application time event.
//
VOID VAppyTimeEvent::cancel()
{
	EVENTHANDLE h;

	h = m_handle;
	m_handle = 0;
	if (h != 0)
		_SHELL_CancelAppyTimeEvent(h);
}


// Function
//	VAppyTimeEvent::IsAppyTimeAvailable - check if appy services active
//
// Syntax
//	BOOL VAppyTimeEvent::IsAppyTimeAvailable(void)
//
// Remarks
//	Static member returns TRUE if AppyTime services are available.
//	
//
BOOL VAppyTimeEvent::IsAppyTimeAvailable(void)
{
	return _SHELL_QueryAppyTimeAvailable();
}

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VAppyTimeEvent::handler - callback for application time event
//
// Syntax
//	VOID VAppyTimeEvent::handler(DWORD refdata)
//
// Parameters
//	refData		reference data value passed to constructor
//
// Remarks
//	Static member functions of class VAppyTimeEvent may be called from
//	inside the handler.
//
VOID VAppyTimeEvent::handler(PVOID refData, DWORD dwFlags)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VAppyTimeEvent.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VAppyTimeEvent class is not defined correctly");
}


// Function
//	VAppyTimeEvent::LocalAlloc - allocate ring 3 memory for application
//				time usage
//
// Syntax
//	SHELLMEMHANDLE LocalAlloc(DWORD flags, DWORD cb, PVOID& linearAddress,
//				  PVOID pInitData = NULL);
//	
// Parameters
//	flags		Bit options:
//			LMEM_ZEROINIT	
//			LMEM_FIXED
//			LPTR (equivalent to LMEM_ZEROINIT+LMEM_FIXED)
//			LMEM_STRING
//	cb		size of region to allocate (use zero if 
//			flags == LMEM_STRING)
//	linearAddress	receives linear address of allocated block
//	pInitData	pointer to data to be copied to ring 3 buffer after
//			allocation (size must be at least cb bytes). If
//			flags == LMEM_STRING, pInitData must point to a 
//			null terminated string.
//
// Remarks
//	This is a static member function. It allocates memory from the 
//	local heap of the message server. VxDs must be careful to limit
//	the amount of memory allocated to a minimum.
//
//	To free memory allocated with this function, call member function
//	LocalFree, passing to it the 16:16 pointer returned by this function.
//	
// Returns
//	Returns the 16:16 pointer to the allocated block. This can be placed
//	in the argument list when using member function CallDLL.
//
//	Both the return value and linearAddress will be zero if the 
//	allocation fails.
//
//SEGOFFSET VAppyTimeEvent::LocalAlloc(DWORD flags, DWORD cb, PVOID& linearAddress,
//					PVOID pInitData)
// {
// Implemented inline in vappy.h
// }
//{
//	return _SHELL_LocalAllocEx(flags, cb, pInitData, &linearAddress);
//}

// Function
//	VAppyTimeEvent::LocalFree - free memory allocated by LocalAlloc
//
// Syntax
//	VOID LocalFree(SEGOFFSET handle)
//				     
// Parameters
//	ptr1616		The DWORD value returned by member function
//			LocalAlloc
//
// Remarks
//	Releases memory allocated by member function LocalAlloc. This
//	is a static member function. Call this only at application time.
//
// {
// Implemented inline in vappy.h
// }
#endif

