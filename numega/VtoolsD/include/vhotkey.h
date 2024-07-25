// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vhotkey.h - include file for VHotKey class
//

#ifndef __vhotkey_h_
#define __vhotkey_h_

#include <vmachine.h>
#include <vkd.h>

/*
	class VHotKey
	-------------

	The class library provides the VHotKey class to facilitate hooking
	hot keys.  

	By deriving a class from VHotKey, you implicitly associate a handler 
	function with methods for defining, hooking, and unhooking your hot 
	key.

	Perform the following steps to hook a hot key:

	1) Derive a class for your hot key from the base class VHotKey.
	   The class must define a constructor and override the handler
	   member function of VHotKey.

	   example:

	   	class CtrlCEvent : public VHotKey
		{
		public:
			CtrlCEvent();
			virtual void handler(BYTE, keyAction_t, DWORD,
						DWORD, DWORD);
		};

	2) Create a constructor for your class that invokes the base
	   class constructor to define your hot key.

	   example:

	   	CtrlCEvent::CtrlCEvent() :
			VHotKey(VK_C, SCAN_NORMAL, HKSS_Ctrl, CallOnPress)
		{
		}

	3) Create a handler by overriding the handler member function of
	   the VHotKey class.

	   example:
	   	
	   	CtrlCEvent::handler(BYTE scan, keyAction_t ka, DWORD shift,
				PVOID refData, DWORD elapased)
		{
			// whatever your handler does
		}


	4) Create an instance of your class with the new operator 
	   (usually at Device_Init time).

	   example:

	   	CtrlCEvent* pCC = new CtrlCEvent();


	5) Hook the hot key using the hook() member function.

	   example:

	   	bStatus = pCC->hook();

*/

#define HKTHUNKSIZE 0x1C

class VHotKey
{
public:
	VHotKey(BYTE scan, scanType_t st, DWORD shift, DWORD flags, 
			PVOID refData=0, DWORD delay=0);
	~VHotKey() {unhook();};
	virtual VOID	handler(BYTE scan, keyAction_t ka, DWORD shift,
					PVOID refData, DWORD elapased);
	BOOL	hook();
	BOOL	unhook();
	VOID	localEnable(BOOL enable, VMHANDLE hVM);
	VOID 	reflectToVM(VMHANDLE hVM, DWORD shift);
	static	VOID 	cancelState();
protected:
	DWORD	m_handle;
	BYTE	m_scanCode;
	scanType_t m_scanType;
	DWORD	m_shiftState;
	BYTE	m_flags;
	PVOID	m_refData;
	DWORD	m_timeout;
	BYTE	m_thunk[HKTHUNKSIZE];
};

#endif
