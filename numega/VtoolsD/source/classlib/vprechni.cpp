// vprechni.cpp - Implementation of class VPreChainV86Int 
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

extern "C" PVOID __cdecl createPreChainV86IntThunk(VPreChainV86Int*, BYTE*, BOOL (VPreChainV86Int::*)(VMHANDLE,CLIENT_STRUCT*,DWORD));
extern "C" VOID	__cdecl createPostChainThunk(VPreChainV86Int*, BYTE*, VOID (VPreChainV86Int::*)(VMHANDLE,CLIENT_STRUCT*,PVOID));
extern "C" PVOID __cdecl getPreChainV86IntHookAddress(BYTE*);
extern "C" PVOID __cdecl getPostChainHookAddress(BYTE*);

// Function
//	VPreChainV86Int::VPreChainV86Int
//
// Syntax
//	VPreChainV86Int::VPreChainV86Int(int iInterrupt);
//
// Parameters
//	iIinterrrupt	specifies the interrupt number
//
// Remarks
//	Constructs an instance of class VPreChainV86Int.  The 
//	interrupt is not hooked until the member function hook
//	is called.
//
//	This class corresponds to interrupts occurring in V86
//	mode applications.  The class is designed to provide the VxD
//	with notification of the interrupt (by calling member function
//	handler) prior to the invocation of any handlers installed
//	at application level.
//
//	This class is used for software interrupts. For hardware
//	interrupts that require specialized processing at the IRQ level, 
//	use class VIrq.
//
// See Also
//	vintrs.h
//
VPreChainV86Int::VPreChainV86Int(int iInterrupt)
{
	m_iInt = iInterrupt;
	m_flags = 0;
}

// Function
//	VPreChainV86Int::~VPreChainV86Int
//
// Syntax
//	VPreChainV86Int::~VPreChainV86Int();
//
// Remarks
//	Destructor for class VPreChainV86Int.  Calls the member function
//	unhook to ensure that the interrupt does not remain hooked after
//	the object is destroyed.
//
VPreChainV86Int::~VPreChainV86Int()
{
	unhook();
}

// Function
//	VPreChainV86Int::hook
//
// Syntax
//	BOOL VPreChainV86Int::hook();
//
// Remarks
//	Hooks an interrupt occurring in V86 mode for all VMs. The interrupt 
//	number is specified when the class instance is constructed. Call
//	during device initialization. 
//
// Return Value
//	Returns TRUE if the interrupt is successfully hooked.
//
BOOL VPreChainV86Int::hook()
{
	PVOID pHandler;

	pHandler = createPreChainV86IntThunk(this, m_thunk, &VPreChainV86Int::handler);
	createPostChainThunk(this, m_postThunk, &VPreChainV86Int::postChainHandler);

	if (Hook_V86_Int_Chain(m_iInt, (PV86Int_HANDLER) pHandler, NULL))
	{
		m_flags |= INT_HOOKED;
		return TRUE;
	}
	else
		return FALSE;
}

// Function
//	VPreChainV86Int::unhook
//
// Syntax
//	BOOL VPreChainV86Int::unhook()
//
// Remarks
//	Unhooks a previously hooked V86 interrupt.
//
// Return Value
//	Returns TRUE if the interrupt is successfully unhooked.
//
BOOL VPreChainV86Int::unhook()
{
	PV86Int_HANDLER pHandler;

	if ( ! (m_flags & INT_HOOKED) )
		return TRUE;

	pHandler = (PV86Int_HANDLER)getPreChainV86IntHookAddress(m_thunk);

#ifdef WIN40
	return Unhook_V86_Int_Chain(m_iInt, pHandler, NULL);
#else
	DEBUGEVENT(DBG_WARNING, "Cannot unhook VPreChainV86Int in this version of Windows");
	return FALSE;
#endif
}

// Function
//	VPreChainV86Int::postChainHandler
//
// Syntax
//	VOID VPreChainV86Int::postChainHandler(VMHANDLE hVM,CLIENT_STRUCT* pRegs,PVOID refdata)
//
// Parameters
//	hVM	handle of the current Virtual Machine
//	pRegs	pointer to client register structure
//	refData	reference data passed to enablePostChainHandler
//
// Remarks
//	Override this member function in the class you derive from 
//	VPreChainV86Int.  You must also call enablePostChainHandler inside 
//	the member function handler of your interrupt class.
//
//	The post-chain handler is invoked after all application level 
//	handlers have executed.
//
VOID VPreChainV86Int::postChainHandler(VMHANDLE hVM,CLIENT_STRUCT* pRegs,PVOID refdata)
{
}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Function
//	VPreChainV86Int::enablePostChainHandler
//
// Syntax
//	VOID VPreChainV86Int::enablePostChainHandler(LONG timeout, PVOID refData)
//
// Parameters
//	timeout		the effect of this parameter depends on its value
//			as follows:
//			== 0 no effect
//			 > 0 specifies the number of milliseconds that may
//			     before invoking the post-chain handler
//			 < 0 specifies the negative of the number of msec
//			     that may elapse prior to invoking the post-chain
//			     handler; the post-chain handler will be called
//			     both after the timeout and when the application
//			     handler chain has been executed.
//	refData		Reference data value passed on to post-chain handler
//
// Remarks
//	This function should only be called from inside the member  function
//	handler of your derived interrupt classes.  
//
//	This function enables you to examine the state of a virtual machine
//	after all application level handlers have executed, and before the
//	VM returns to the instruction following the interrupt.
//
//	To make use of this function, the class that you derive from 
//	VPreChainV86Int must override the member functions handler and 
//	postChainHandler  (see below).
//
VOID VPreChainV86Int::enablePostChainHandler(LONG timeout, PVOID refData)
{
	PVOID pFunc = getPostChainHookAddress(m_postThunk);

	Call_When_VM_Returns((PVMReturns_HANDLER) pFunc, refData, timeout, NULL);
}

