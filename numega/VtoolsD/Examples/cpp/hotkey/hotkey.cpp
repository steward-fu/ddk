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
//=============================================================================

// Copyright (c) 1994 Compuware Corporation
//
// VToolsD Hot Key example
//
#define DEVICE_MAIN
#include <vtoolscp.h>
#include "hotkey.h"

Declare_Virtual_Device(HOTKEY);

BOOL Init(void);

#define C_SCAN 46

XCtrlCEvent* pCC;

BOOL HotKeyDevice::OnDeviceInit(VMHANDLE hVM, PCHAR pszCmdTail)
{
	return Init();
}


BOOL HotKeyDevice::OnSysDynamicDeviceInit(void)
{
	return Init();
}

BOOL HotKeyDevice::OnSysDynamicDeviceExit(void)
{
	if (pCC)
		delete pCC;

	return TRUE;
}

BOOL Init(void)
{
	pCC = new XCtrlCEvent();
	if (pCC)
		return pCC->hook();
	else
		return FALSE;
}

XCtrlCEvent::XCtrlCEvent() :
		VHotKey(C_SCAN, SCAN_NORMAL, HKSS_Ctrl, CallOnPress) {}

VOID XCtrlCEvent::handler(BYTE scan, keyAction_t ka, DWORD shift,
					PVOID refData, DWORD elapased)
{
	dout << "Ctrl-C detected" << endl;

	reflectToVM(Get_Cur_VM_Handle(), shift);
}

