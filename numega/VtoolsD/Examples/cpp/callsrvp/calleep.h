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

// CALLEEP.H - include file for VxD CALLEEP
// Copyright (c) 1994 Compuware Corporation

#include <vtoolscp.h>

#undef  DEVICE_CLASS
#define DEVICE_CLASS		CalleepDevice
#define CALLEEP_DeviceID	NUMEGA_TEST_ID
#define CALLEEP_Init_Order	0x7f000000
#define CALLEEP_Major		1
#define CALLEEP_Minor		0

class CalleepDevice : public VDevice
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
	static DWORD _cdecl CALLEEP_Get_Version();
	static DWORD _cdecl CALLEEP_Sum(DWORD arg1, DWORD arg2);
	static PCHAR _cdecl CALLEEP_ToUpper(PCHAR s);
};

class CalleepVM : public VVirtualMachine
{
public:
	CalleepVM(VMHANDLE hVM);
};

Begin_VxD_Service_Table(CALLEEP)
	VxD_Service(CALLEEP_Get_Version)
	VxD_Service(CALLEEP_Sum)
	VxD_Service(CALLEEP_ToUpper)
End_VxD_Service_Table
