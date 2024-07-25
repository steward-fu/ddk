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

// HOOKCSVC.c - main module for VxD HOOKCSVC

// This is the main module of a dynamically loadable VxD that demonstrates
// the usage of run-time library functions Hook_Device_Service_C and
// Unhook_Device_Service_C. These functions are used in place of the
// normal Hook_Device_Service wrapper when you are hooking a C callable 
// device service. This example hooks _HeapAllocate to demonstrate this
// capability.

#define   DEVICE_MAIN
#include  "hookcsvc.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(HOOKCSVC)

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);

BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);

	END_CONTROL_DISPATCH

	return TRUE;
}

// declare a pointer to the real _HeapAllocate
PVOID (*TheRealHeapAllocate)(DWORD, DWORD);

// declare the thunk used to hook the service
HDSC_Thunk aThunk;

// --------------------------------------------------------------------
// After hooking, the following function is invoked whenever any VxD
// calls _HeapAllocate. It dumps the arguments to the debug device and
// passes the call through to the real _HeapAllocate.

PVOID MyHeapAllocate(DWORD nBytes, DWORD flags)
{
	dprintf("Call to _HeapAllocate(%08lx, %08lx)\n\r", nBytes, flags);
	return TheRealHeapAllocate(nBytes, flags);
}


// --------------------------------------------------------------------
// This function is called when the VxD receives the control message
// SYS_DYNAMIC_DEVICE_INIT.

BOOL OnSysDynamicDeviceInit()
{

// Here we hook _HeapAllocate by calling Hook_Device_Service_C. Note
// that the service name two additional underscores (__) prefixed to its
// name. This identifier is automatically generated from the VxD service 
// table for the VMM in vmm.h. When you hook a service, you must follow
// this convention, i.e., simply place to extra underscores in front of
// the service name to form the correct identifier. The VxD service table
// for the VxD owning the service must appear in an include.

	TheRealHeapAllocate =
		Hook_Device_Service_C(___HeapAllocate, MyHeapAllocate, &aThunk);

	return TRUE;
}

// --------------------------------------------------------------------
// This function is called when the VxD receives the control message
// SYS_DYNAMIC_DEVICE_EXIT.

BOOL OnSysDynamicDeviceExit()
{
	Unhook_Device_Service_C(___HeapAllocate, &aThunk);
	return TRUE;
}

