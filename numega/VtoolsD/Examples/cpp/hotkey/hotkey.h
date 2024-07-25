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

// hotkey.h - include file for the hot key example
// Copyright (c) 1995 Compuware Corporation

#define DEVICE_CLASS 	   	HotKeyDevice
#define HOTKEY_Major    	1
#define HOTKEY_Minor	 	0
#define HOTKEY_DeviceID   	UNDEFINED_DEVICE_ID
#define HOTKEY_Init_Order  	UNDEFINED_INIT_ORDER

class HotKeyDevice: public VDevice
{	
public:
	virtual BOOL OnDeviceInit(VMHANDLE hVM, PCHAR pszCmdTail);
	virtual BOOL OnSysDynamicDeviceInit(void);
	virtual BOOL OnSysDynamicDeviceExit(void);
};

class XCtrlCEvent : public VHotKey
{
public:
	XCtrlCEvent();
	virtual VOID handler(BYTE scan, keyAction_t ka, DWORD shift,
					PVOID refData, DWORD elapased);
};
