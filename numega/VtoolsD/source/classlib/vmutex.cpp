// vmutex.cpp - implementation of VMutex class
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

#ifdef WIN40
// Function
//	VMutex::VMutex
//
// Syntax
//	VMutex::VMutex(DWORD boost, DWORD flags);
//
// Parameters
//	boost	priority boost to be granted to the thread when the mutex
//		is acquired
//	flags	flags parameter:
//		MUTEX_MUST_COMPLETE	thread cannot be suspended or 
//					terminated while mutex is owned
//
// Remarks
//	Constructs an instance of class VMutex.  If the constructor is
//	not successful, member function getHandle will return zero.
//
VMutex::VMutex(DWORD boost, DWORD flags)
{
	m_handle = _CreateMutex(boost, flags);
}

// Function
//	VMutex::~VMutex
//
// Syntax
//	VMutex::~VMutex();
//
// Remarks
//	Do not attempt to destroy a mutex while it is in use.
//
VMutex::~VMutex()
{
	_DestroyMutex(m_handle);
}

// Function
//	VMutex::enter
//
// Syntax
//	VOID VMutex::enter(DWORD flags)
//
// Parameters
//	flags:
//	Block_Svc_Ints		Service interrupts in the virtual machine
//				even if the virtual machine is blocked.
//	Block_Svc_If_Ints_Locked Service interrupts in the virtual machine
//				even if the virtual machine is blocked
//				and the VMStat_V86IntsLocked flag is set.
//	Block_Enable_Ints	Service interrupts in the virtual machine
//				even if the virtual machine does not
//				currently have interrupts enabled. This
//				forces interrupts to be enabled. This
//				value is only relevant if either
//				Block_Svc_Ints or Block_Svc_If_Ints_Locked is 
//				set.
//	Block_Thread_Idle	Consider the thread to be idle if it blocks
//				for the mutex.
//				
// Remarks
//	If the mutex is unowned, the calling thread becomes the owner, and
//	the calling thread's execution priority is boosted by the amount
//	specified when the mutex was created.  If a thread other than the
//	calling thread owns the mutex, the calling thread is blocked until
//	the mutex becomes unowned.  If the calling thread already owns the
//	mutex, the thread continues execution without a priority change.
//
//	Calls to enter() and leave() accumulate, i.e., for each call to
//	enter() there must be a corresponding call to leave(). 
//	
VOID VMutex::enter(DWORD flags)
{
	_EnterMutex(m_handle, flags);
}

// Function
//	VMutex::leave
//
// Syntax
//	VOID VMutex::leave()
//
// Remarks
//	A mutex is initially unowned, and the ownership count is zero.  For 
//	each call to enter, there mustbe a corresponding call to leave.  Each 
//	time enter is called, the ownership count increments, and each time 
//	leave is called, the ownership count decrements.  Only the thread that 
//	owns a mutex may leave it.  
//
//	When the ownership count falls to zero, the mutex becomes unowned.
//	At this point, the VMM unblocks the highest priority thread 
//	attempting to enter the mutex.
//
VOID VMutex::leave()
{
	_LeaveMutex(m_handle);
}

// Function
//	VMutex::owner
//
// Syntax
//	THREADHANDLE VMutex::owner()
//
// Remarks
//	Fetches the owner of a mutex.
//
// Return Value
//	Returns the THREADHANDLE of the thread that owns the mutex.  If 
//	the mutex is unowned, returns zero.
//
THREADHANDLE VMutex::owner()
{
	return _GetMutexOwner(m_handle);
}
#endif

// Function
//	VMutex::enterGlobal
//
// Syntax
//	VOID VMutex::enterGlobal(DWORD flags)
//
// Parameters
//	flags
//
// Remarks
//	Enters the global critical section.  Each call to enterGlobal must
//	be paired with a call to leaveGlobal.  
//
//	If the global critical section is owned by another thread (or
//	virtual machine) the calling thread will block.  If the calling
//	thread already owns the global critical section, it continues
//	execution without a change in priority.  If the global critical
//	section is unowned, the calling thread becomes the owner and its
//	execution priority is automatically boosted.
//
VOID VMutex::enterGlobal(DWORD flags)
{
	Begin_Critical_Section(flags);
}

// Function
//	VMutex::leaveGlobal
//
// Syntax
//	VOID VMutex::leaveGlobal()
//
// Remarks
//	Leaves the global critical section.
//
VOID VMutex::leaveGlobal()
{
	End_Critical_Section();
}

