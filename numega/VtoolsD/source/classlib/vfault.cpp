// vfault.cpp - implementation of fault classes
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

extern "C" PVOID __cdecl createFaultThunk(VFault*,BYTE*,int,BOOL(VFault::*)(VMHANDLE,CLIENT_STRUCT*,int));
extern "C" PVOID __cdecl getFaultUnhookAddress(BYTE*);
extern "C" VOID  __cdecl initFaultThunk(BYTE*, PVOID);

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VFault::VFault - constructor for fault class
//
// Syntax
//	VFault::VFault(int iFault)
//
// Parameters
//	iFault		specifies the fault number
//
// Remarks
//	The fault number must be greater or equal to zero, and less
//	than or equal to 0x4F.  
//
//	Normally this constructor is only called as a base constructor
//	for classes you derived from VV86ModeFault, VProtModeFault,
//	VVMMFault, and VNMIEvent. See VFault::hook for more details.
//	
VFault::VFault(int i)
{
	if ( (i >= 0) && (i <= 0x4f) )
	{
		m_iFault = i;
		m_flags = FAULT_VALID;
	}
	else
	{
		DEBUGEVENT(DBG_WARNING, "Attempt to create bad VFault object");
		m_flags = 0;
	}
}

// Function
//	VFault::hook - hook a fault
//
// Syntax
//	BOOL VFault::hook(VOID)
//
// Remarks
//	Follow these steps to hook a fault:
//
//	1) Depending on the kind of fault you wish to hook, derive a class 
//	   from either VProtModeFault, VV86ModeFault, VVMMFault, or VNMIEvent.
//	2) Define a constructor for your class that calls the base class
//	   constructor, passing the fault number. The constructor for
//	   VNMIEvent does not take an argument.
//	3) Override VFault::handler with the handler for your fault.
//	4) Create an instance of your fault class using the new operator.
//	5) Call the hook member function of the fault class instance.
//
//	This function is normally invoked only with an introducing class
//	derived from VV86ModeFault, VProtModeFault, VVMMFault, VNMIEvent.
//
//	See VFault::handler for a description of how the fault handler is
//	called.
//
// Return Value
//	Returns TRUE if the fault is successfully hooked.
//	
// Example
//	1) Derive your class:
//
//		class My_V86_GPFault : public VV86ModeFault
//		{
//		public:
//			My_V86_GPFault();	// constructor
//			virtual BOOL handler(VMHANDLE, CLIENT_STRUCT*, int);
//		}
//
//	2) Write the constructor:
//	
//		My_V86_GPFault::My_V86_GPFault() : VV86ModeFault(0xD)
//		{
//		}
//
//	3) Write the handler:
//
//		BOOL My_V86_GPFault::handler(VMHANDLE h, CLIENT_STRUCT* pRegs, int i)
//		{
//			// whatever your handler does
//		}
//
//	4) At the appropriate point (usually at Device_Init time), create
//	   an instance with new and call the hook function:
//
//		My_V86_GPFault* pGP = new My_V86_GPFault;
//
//		pGP->hook();
//
BOOL VFault::hook()
{
	PVOID prev;
	
	if ( !(m_flags & FAULT_VALID) 	|| 
	      (m_iFault == BYTE(0xff))		|| 
	      (m_flags & FAULT_HOOKED))
		return FALSE;

	PVOID p = createFaultThunk(this, m_thunk, m_iFault, &VFault::handler);

	if (_hookProc(m_iFault, p, &prev))
	{
		initFaultThunk(m_thunk, prev);
		m_flags |= FAULT_HOOKED;
		return TRUE;
	}
	else
		return FALSE;
}

// Function
//	VFault::unhook - unhook a fault
//
// Syntax
//	BOOL VFault::unhook(VOID)
//
// Remarks
//	Use this function to unhook a previously hooked fault. You must
//	the same instance of the class derived from VFault to hook and
//	unhook the fault.
//
//	This function is normally invoked only with an introducing class
//	derived from VV86ModeFault, VProtModeFault, VVMMFault, or VNMIEvent.
//
// Return Value
//	Returns TRUE if the fault is successfully unhooked.
//	
// Example
//	At initialization time:
//
//		VMyFault* pMyFault = new VMyFault;
//		pMyFault->hook();
//
//	At some later time, when you want to unhook:
//	
//		pMyFault->unhook();
//		delete pMyFault;
//
//
BOOL VFault::unhook()
{
	if ( (m_flags & FAULT_HOOKED) && (m_flags && FAULT_VALID))
	{
		PVOID p = getFaultUnhookAddress(m_thunk);

		if (_unhookProc(m_iFault, p))
		{
			m_flags &= ~FAULT_HOOKED;
			return TRUE;
		}
		else 
			return FALSE;
	}

	return TRUE;
}

// Dummy entry point for member function handler is located in the pageable
// code segment to ensure that the thunk generated by taking its address is
// accessible at the time it is hooked.  The actual handler (of the derived
// class) may be in the locked segment if desired.  The routine that creates
// the thunk for the handler invocation "un-thunks" the compiler generated
// thunk before hooking.

// Function
//	VFault::handler - handle a fault
//
// Syntax
//	BOOL VFault::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, int iFault)
//
// Parameters
//	hVM		handle of the virtual machine in which the fault
//			occurred (the current VM)
//	pRegs		Pointer to the client register struct (see vmm.h).
//			Any changes to register values are effected when
//			the VM resumes execution.
//	iFault		The number of the fault being handled.
//
// Remarks
//	Implement a fault handler by overriding this member function in
//	the class you derive from VV86ModeFault, VProtModeFault, VVMMFault,
//	or VNMIEvent.
//
//	Changing the values in the CLIENT_STRUCT pointed to by the pRegs
//	parameter causes the corresponding change in the VM's register
//	state after the fault handler returns.
//
//	The iFault parameter allows you to use the same handler for multiple
//	instances of your fault class.  This is useful if your fault class
//	constructor takes an argument that specifies the fault number.  For
//	example, you could use the same handler for stack faults and
//	general protection faults by passing either 0xC or 0xD to your
//	constructor, and then, in your handler, branching on the value of
//	the iFault parameter.
//
// Return Value
//	Return TRUE to indicate that your handler handled the fault, and that
//	no further handlers should be called.  Otherwise return FALSE.
//
BOOL VFault::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, int intno)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VFault

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VFault class is not defined correctly");

	return FALSE;
}


