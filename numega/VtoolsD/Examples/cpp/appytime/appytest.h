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

// APPYTEST.h - include file for VxD APPYTEST
// Copyright (c) 1994, Compuware Corporation

#include <vtoolscp.h>

#define DEVICE_CLASS		AppytestDevice
#define APPYTEST_DeviceID		UNDEFINED_DEVICE_ID
#define APPYTEST_Init_Order	UNDEFINED_INIT_ORDER
#define APPYTEST_Major		1
#define APPYTEST_Minor		0

class AppytestDevice : public VDevice
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
	virtual BOOL OnSysDynamicDeviceInit(void);
	virtual BOOL OnCreateVM(VMHANDLE hVM);
};

class MyAppyEvent : public VAppyTimeEvent
{
public:
	MyAppyEvent();
	virtual VOID handler(PVOID ref, DWORD flags);
};

