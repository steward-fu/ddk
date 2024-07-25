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

// CALLEEP.CPP - main module for VxD CALLEEP
// Copyright (c) 1994 Compuware Corporation

// This example VxD provides services to the CALLERP VxD.

#define DEVICE_MAIN
#include "calleep.h"
Declare_Virtual_Device(CALLEEP)
#undef DEVICE_MAIN

CalleepVM::CalleepVM(VMHANDLE hVM) : VVirtualMachine(hVM) {}

BOOL CalleepDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail)
{
	return TRUE;
}

DWORD _cdecl CalleepDevice::CALLEEP_Get_Version()
{
	return 0x10;
}
DWORD _cdecl CalleepDevice::CALLEEP_Sum(DWORD arg1, DWORD arg2)
{
	return arg1+arg2;
}
PCHAR _cdecl CalleepDevice::CALLEEP_ToUpper(PCHAR s)
{
	return _strupr(s);
}
