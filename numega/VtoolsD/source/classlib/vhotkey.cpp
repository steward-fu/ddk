// vhotkey.cpp - implementation of VHotKey class
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

extern "C" PVOID  __cdecl createHotKeyThunk(VHotKey*, BYTE*, VOID (VHotKey::*)(BYTE,keyAction_t,DWORD,PVOID,DWORD));

// Function
//	VHotKey::VHotKey - constructor for hot key class
//
// Syntax
//	VHotKey(BYTE scan, scanType_t st, DWORD shift, DWORD flags, 
//		PVOID refData, DWORD delay)
//
// Parameters
//
//	scan	the scan code identifying the hot key
//	st	scan type: SCAN_NORMAL, SCAN_EXTENDED, SCAN_EITHER
//	shift	the shift state to scan for (see vkd.h).  The high
//		half of the DWORD is a mask which identifies which
//		bits of the global shift state are relevant.  The
//		global shift state is and'ed with this mask, and the
//		result is compared with the low half of the DWORD.
//		If the result is non-zero, the handler function may be invoked.
//	flags	flags that specify other conditions:
//
//		CallOnPress	Call call-back when key press is detected
//		CallOnRelease	Call call-back when key release is detected
//		CallOnRepeat	Call call-back when repeated press is detected
//		CallOnComplete	Call call-back when the hot key state is
//				ended (all shift modifier keys are released)
//				or when a different hot key is entered)
//
//		CallOnUpDwn	Call call-back on both press and release
//		CallOnAll	Call call-back on press, release, auto-repeat
//				and complete
//		PriorityNotify	notification calls to the call-back can only
//				happen when interrupts are enabled and
//				the critical section is un-owned
//		Local_Key	key can be locally enabled/disabled
//
//	refData	a reference value that is passed to the handler when the hot 
//		key event occurs
//	delay	if the PriorityNotify bit is set in the flags, this parameter
//		specifies the maximum delay (in milliseconds) that may
//		elapse before the handler is invoked.
//
//
// Remarks
//	
VHotKey::VHotKey(BYTE scan, scanType_t st, DWORD shift, DWORD flags, 
		PVOID refData, DWORD delay)
{
	m_scanCode = scan;
	m_scanType = st;
	m_shiftState = shift;
	m_flags = flags;
	m_refData = refData;
	m_timeout = delay;
}


// Function
//	VHotKey::hook - hook the hot key
//
// Syntax
//	BOOL VHotKey::hook()
//
// Remarks
//
//	Perform the following steps to hook a hot key:
//
//	1) Derive a class for your hot key from the base class VHotKey.
//	   The class must define a constructor and override the handler
//	   member function of VHotKey.
//
//	   example:
//
//	   	class CtrlCEvent : public VHotKey
//		{
//		public:
//			CtrlCEvent();
//			virtual VOID handler(BYTE, keyAction_t, DWORD,
//						DWORD, DWORD);
//		};
//
//	2) Create a constructor for your class that invokes the base
//	   class constructor to define your hot key.
//
//	   example:
//
//	   	CtrlCEvent::CtrlCEvent() :
//			VHotKey(VK_C, SCAN_NORMAL, HKSS_Ctrl, CallOnPress)
//		{
//		}
//
//	3) Create a handler by overriding the member function handler of
//	   the VHotKey class.
//
//	   example:
//	   	
//	   	CtrlCEvent::handler(BYTE scan, keyAction_t ka, DWORD shift,
//				PVOID refData, DWORD elapased)
//		{
//			// whatever your handler does
//		}
//
//
//	4) Create an instance of your class with operator new  
//
//	   example:
//
//	   	CtrlCEvent* pCC = new CtrlCEvent();
//
//
//	5) Hook the hot key using the member function hook.
//
//	   example:
//
//	   	pCC->hook();
//	
// Return Value
//	Returns TRUE if the hot key is successfully hooked.
//
BOOL VHotKey::hook()
{
	PVOID	pFunc = createHotKeyThunk(this, m_thunk, &VHotKey::handler);

	m_handle = (DWORD) VKD_Define_Hot_Key(m_scanCode,
					      m_scanType,
					      m_shiftState,
					      m_flags,
					      (PVKDHotkey_HANDLER) pFunc,
					      m_refData,
					      m_timeout,
					      NULL);

	if (m_handle == 0)
	{
		DEBUGEVENT(DBG_WARNING, "Failed to hook hot key");
	}

	return (m_handle != 0);
}


// Function
//	VHotKey::unhook - unhook the hot key
//
// Syntax
//	BOOL VHotKey::unhook()
//
// Remarks
//	Unhooks the hot key.
//	
// Return Value
//	Returns TRUE if the hot key is successfully unhooked.
//
BOOL VHotKey::unhook()
{
	VKD_Remove_Hot_Key((HOTKEYHANDLE) m_handle);
	return TRUE;
}


// Function
//	VHotKey::localEnable - enable or disable a hot key for a particular VM
//
// Syntax
//	VOID VHotKey::localEnable(BOOL enable, VMHANDLE hVM)
//
// Parameters
//	enable	TRUE to enable the hot key; FALSE to disable it
//	hVM	handle of the Virtual Machine for which the hot key is to
//		be enabled or disabled
//
VOID VHotKey::localEnable(BOOL enable, VMHANDLE hVM)
{
	DWORD h = m_handle;


	if (enable)
	{
		VKD_Local_Enable_Hot_Key(hVM, (HOTKEYHANDLE) h);
	}
	else
	{
		VKD_Local_Disable_Hot_Key(hVM, (HOTKEYHANDLE) h);
	}
}

// Dummy entry point for member function handler is located in the pageable
// code segment to ensure that the thunk generated by taking its address is
// accessible at the time it is hooked.  The actual handler (of the derived
// class) may be in the locked segment if desired.  The routine that creates
// the thunk for the handler invocation "un-thunks" the compiler generated
// thunk before hooking.

VOID VHotKey::handler(BYTE scan, keyAction_t ka, DWORD shift, PVOID refData, DWORD elapased)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VHotKey.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VHotKey class is not defined correctly");
}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Function
//	VHotKey::cancelState()
//
// Syntax
//	VOID VHotKey::cancelState()
//
// Remarks
//	Cancels the hot key state.
//		
//
VOID VHotKey::cancelState()
{
	VKD_Cancel_Hot_Key_State();
}

// Function
//	reflectToVM - reflect a trapped hot key into a particular VM
//
// Syntax
//	reflectToVM(VMHANDLE hVM, DWORD shift)
//
// Parameters
//	hVM	handle of the VM into which the hot key is to be reflected
//	shift	the shift state to effect in the VM prior to reflecting the
//		hot key
//	
// Remarks
//	This function is normally invoked only from inside the handler
//	member function.
//
VOID VHotKey::reflectToVM(VMHANDLE hVM, DWORD shift)
{
	VKD_Reflect_Hot_Key(hVM, (HOTKEYHANDLE) m_handle, (WORD) shift);
}
