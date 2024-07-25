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

// CLASSTUT.H - include file for VxD CLASSTUT
//
// Copyright (c) 1994, Compuware Corporation

#include <vtoolscp.h>

#define CLASSTUT_DeviceID	UNDEFINED_DEVICE_ID
#define CLASSTUT_Init_Order	UNDEFINED_INIT_ORDER
#define CLASSTUT_Major		1
#define CLASSTUT_Minor		0

#define DEVICE_CLASS		ClasstutDevice

class ClasstutDevice : public VDevice
{
public:
	static DWORD CLASSTUT_GetVersion();
	virtual BOOL OnDeviceInit(VMHANDLE hVM, PCHAR szCmdTail);
	virtual BOOL OnCreateVM(VMHANDLE hVM);
};

class ClasstutVM : public VVirtualMachine
{
public:
	ClasstutVM(VMHANDLE hVM);
	VOID OnDestroyVM();
};

Begin_VxD_Service_Table(CLASSTUT)
	VxD_Service(CLASSTUT_GetVersion)
End_VxD_Service_Table
