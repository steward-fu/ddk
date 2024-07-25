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

// SERIAL.H - include file for VxD SERIAL
// Copyright (c) 1994, Compuware Corporation

#include <vtoolscp.h>

#define DEVICE_CLASS		SerialDevice
#define SERIAL_DeviceID		UNDEFINED_DEVICE_ID
#ifdef WIN40
#define SERIAL_Init_Order	PORT_INIT_ORDER
#else
#define SERIAL_Init_Order	UNDEFINED_INIT_ORDER
#endif
#define SERIAL_Major		1
#define SERIAL_Minor		0

// Notice that the device class is derived from VCommPortDriver, not
// from VDevice.

class SerialDevice : public VCommPortDriver
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR szCmdtail);
	virtual BOOL OnSysDynamicDeviceInit();
};

