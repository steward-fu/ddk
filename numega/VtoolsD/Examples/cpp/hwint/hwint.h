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

// hwint.h - include file for VxD HWINT, see hwint.cpp for details

#include <vtoolscp.h>		// Primary include for VtoolsD class library
#include "realclk.h"		// Include for our Real Time Clock class
#include "bellevt.h"		// Include for our bell event class

// Device parameters

#define DEVICE_CLASS	   	HwintDevice
#define HWINT_DeviceID		UNDEFINED_DEVICE_ID
#define HWINT_Init_Order	UNDEFINED_INIT_ORDER
#define HWINT_Major		1
#define HWINT_Minor		0

// The Device Class
//
// The device only needs two control message handlers: DEVICE_INIT and
// SYSTEM_EXIT.

class HwintDevice : public VDevice
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
	virtual VOID OnSystemExit(VMHANDLE hSysVM);
};


// Macro to convert a byte of binary coded decimal (BCD) to int

#define BCDtoInt(x) (((x) & 0xf) + ((((x) & 0xf0) >> 4) * 10))
