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

// BEEPER.H - include file for VxD BEEPER
// Copyright (c) 1995 Compuware Corporation

#include <vtoolscp.h>

#define DEVICE_CLASS		BeeperDevice
#define BEEPER_DeviceID		UNDEFINED_DEVICE_ID
#define BEEPER_Init_Order	UNDEFINED_INIT_ORDER
#define BEEPER_Major		1
#define BEEPER_Minor		0

class BeeperDevice : public VDevice
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
	virtual BOOL OnSysDynamicDeviceInit(void);
	virtual BOOL OnSysDynamicDeviceExit(void);
};

class BeepTimeOut : public VGlobalTimeOut
{
public:
	BeepTimeOut(DWORD msec);
#ifdef WIN40
	VOID handler(VMHANDLE hVM, THREADHANDLE th, PCLIENT_STRUCT pRegs,
		DWORD lag);
#else
	VOID handler(VMHANDLE hVM, PVOID reserved, PCLIENT_STRUCT pRegs,
		DWORD lag);
#endif
};
