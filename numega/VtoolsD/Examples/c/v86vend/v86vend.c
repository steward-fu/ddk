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

// V86VEND.C - main module for VxD V86VEND

#define   DEVICE_MAIN
#include  "v86vend.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(V86VEND)

CHAR VendorString[] = "Compuware unique vendor string";


VOID __stdcall V86_Vendor_Api_Handler(VMHANDLE hVM, PVOID pRefdata, PCLIENT_STRUCT pcrs)
{
	Out_Debug_String("V86 Vendor Entry called");
	Simulate_Far_Ret();
}

DefineControlHandler(DEVICE_INIT, OnDeviceInit);

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

	END_CONTROL_DISPATCH

	return TRUE;
}


BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	DebugBreak();
	
	Set_V86_Vendor_Entry(VendorString, V86_Vendor_Api_Handler);

	return TRUE;
}
