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

// CALLER.c - main module for VxD CALLER
// Copyright (c) 1994 Compuware Corporation

// This example VxD makes use of services exported by its companion VxD,
// CALLEE. In the DEVICE_INIT handler, it calls three of CALLEE's services.
// This shows that you can pass arguments, including pointers, and get
// return values from the other VxD.

// This VxD includes the module CALLWRAP.C, which provides linkage to
// CALLEE's services via the VxDJmp macro. VxDJmp is defined in CALLEE.H.
// In future releases, VxDJmp will be incorporated into VtoolsD.

#define   DEVICE_MAIN
#include  "caller.h"
#undef    DEVICE_MAIN

#include  "callee.h"  // NOTE: DEVICE_MAIN must be undef'ed before this line

Declare_Virtual_Device(CALLER)

DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);

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
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);

	END_CONTROL_DISPATCH

	return TRUE;
}

#include INIT_CODE_SEGMENT
#include INIT_DATA_SEGMENT


BOOL CallerTest(void)
{
// Call services of CALLEE
	
	if (CALLEE_Get_Version() == 0)
		dprintf("CALLEE not found\n\r");
	else
	{
		dprintf("The sum of 5+8=%d\n\r", CALLEE_Sum(5,8));

		dprintf("\"abc\" converted to uppercase is %s\n",
			CALLEE_ToUpper("abc"));
	}

	return TRUE;
}


BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	return CallerTest();
}


BOOL OnSysDynamicDeviceInit(void)
{
	return CallerTest();
}

