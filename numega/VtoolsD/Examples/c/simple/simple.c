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

// SIMPLE.C - main module for VxD SIMPLE

#define   DEVICE_MAIN
#include  "simple.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(SIMPLE)

DefineControlHandler(DEVICE_INIT, OnDeviceInit);
#ifdef WIN40
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
#endif

BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_DEVICE_INIT(OnDeviceInit);
#ifdef WIN40
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
#endif

	END_CONTROL_DISPATCH

	return TRUE;
}


BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	// Specify a target for debug output
	// DBGMON will trap output to DBG_SERIAL or DBG_MONO
	SetDebugOutput(DBG_SERIAL);

	// Note: dprintf works with debug driver build only
	//		 nprintf works with debug and retail builds
	nprintf("SIMPLE: Hello at init time\n");
	return TRUE;
}

#ifdef WIN40
BOOL OnSysDynamicDeviceInit()
{
	// Specify a target for debug output
	// DBGMON will trap output to DBG_SERIAL or DBG_MONO
	SetDebugOutput(DBG_SERIAL);

	// Note: dprintf works with debug driver build only
	//		 nprintf works with debug and retail builds
	nprintf("SIMPLE: Hello at dynamic init time\n");
	return TRUE;
}

BOOL OnSysDynamicDeviceExit()
{
	nprintf("SIMPLE: Goodbye, unloading\n");
	return TRUE;
}
#endif
