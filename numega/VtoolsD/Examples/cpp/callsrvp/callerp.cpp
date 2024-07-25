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

// CALLERP.CPP - main module for VxD CALLERP
// Copyright (c) 1994 Compuware Corporation

// This example VxD makes use of services exported by its companion VxD,
// CALLEEP. In the DEVICE_INIT handler, it calls three of CALLEEP's services.
// This shows that you can pass arguments, including pointers, and get
// return values from the other VxD.

// This VxD includes the module CALLWRPP.C, which provides linkage to
// CALLEEP's services via the VxDJmp macro. VxDJmp is defined in CALLEEP.H.
// In future releases, VxDJmp will be incorporated into VtoolsD.

#define DEVICE_MAIN
#include "callerp.h"
Declare_Virtual_Device(CALLERP)
#undef DEVICE_MAIN

#include "calleep.h" // NOTE: DEVICE_MAIN must be undef'ed before this line

BOOL CallerpDevice::CallerTest(void)
{
	if (CalleepDevice::CALLEEP_Get_Version() == 0)
		dout << "CALLEEP not found" << endl;

	else
	{
		dout << "The sum of 5+8=" 
			<< CalleepDevice::CALLEEP_Sum(5,8) << endl;

		dout << "\"abc\" in uppercase is " 
			<< CalleepDevice::CALLEEP_ToUpper("abc") << endl;
	}

	return TRUE;
}


BOOL CallerpDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail)
{
	return CallerTest();
}

BOOL CallerpDevice::OnSysDynamicDeviceInit(void)
{
	return CallerTest();
}


