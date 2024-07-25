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

// CALLERP.H - include file for VxD CALLERP
// Copyright (c) 1994 Compuware Corporation

#include <vtoolscp.h>

#define DEVICE_CLASS		CallerpDevice
#define CALLERP_DeviceID	UNDEFINED_DEVICE_ID
#define CALLERP_Init_Order	0x7f000001
#define CALLERP_Major		1
#define CALLERP_Minor		0

class CallerpDevice : public VDevice
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
	virtual BOOL OnSysDynamicDeviceInit(void);
	BOOL CallerTest(void);
};

