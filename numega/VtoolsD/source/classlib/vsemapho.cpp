// vsemapho.cpp - implementation of VSemaphore class
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
//	VSemaphore::VSemaphore
//
// Syntax
//	VSemaphore::VSemaphore(int tokenCount)
//
// Parameters
//	tokenCount	specifies the initial token count for the 
//			semaphore
//
// Remarks
//	Constructs an instance of class VSemaphore.  If not successful,
//	the data member m_handle is zero.
// 
VSemaphore::VSemaphore(int tokenCount)
{
	m_handle = Create_Semaphore((DWORD)tokenCount);
#ifdef DEBUG
	if (m_handle == 0)
		DEBUGEVENT(DBG_WARNING, "failed to create semaphore");
#endif
}

// Function
//	VSemaphore::~VSemaphore
//
// Syntax
//	VSemaphore::~VSemaphore()
//
// Remarks
//	Destroys a VSemaphore object.  
//
VSemaphore::~VSemaphore()
{
	Destroy_Semaphore(m_handle);
}

// Function
//	VSemaphore::wait
//
// Syntax
//	VOID VSemaphore::wait(DWORD flags)
//
// Parameters
//	flags:	
//
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
//	Block_Poll		Do not switch away from the blocked virtual 
//				machine unless another virtual machine has 
//				higher priority.
//
// Remarks
//	This function claims a semaphore token, thereby reducing the
//	token count by one.  If the token count is zero when wait is
//	called, the caller is blocked until the semaphore is signalled.
//	Otherwise, the call returns immediately.  
//
VOID VSemaphore::wait(DWORD flags)
{
	Wait_Semaphore(m_handle, flags);
}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Function
//	VSemaphore::signal
// Syntax
//	VOID VSemaphore::signal()
//
// Remarks
//	Increments the token count of the semaphore.  If virtual machines
//	or threads are waiting on the semaphore, one of them will be 
//	unblocked. 
//
VOID VSemaphore::signal()
{
	Signal_Semaphore(m_handle);
}

#ifdef WIN40
// Function
//	VSemaphore::signal_noSwitch
// Syntax
//	VOID VSemaphore::signal_noSwitch()
//
// Remarks
//	Increments the token count of the semaphore.  If virtual machines
//	or threads are waiting on the semaphore, one of them will be 
//	unblocked. The VMM does not immediately reschedule threads and
//	virtual machines.
//
VOID VSemaphore::signal_noSwitch()
{
	Signal_Semaphore_No_Switch(m_handle);
}
#endif
