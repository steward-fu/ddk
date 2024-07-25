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

// hello.h - include file for simplest VxD using VToolsD class library
//
// Copyright (c) 1994, Compuware Corporation
//

#include <vtoolscp.h>		// always include this file 

// There are five symbols that your include file must define:

// (1) Define DEVICE_CLASS to be the name of the class of which your device
//     is an instance.  All VxDs must have a device class, and it must be
//     derived from the base class VDevice.  You override member functions of
//     VDevice in your device class in order to process control messages and
//     to provide callable entry points for applications.

#define DEVICE_CLASS		XHelloDevice

// (2) Define xxxxx_DeviceID (where xxxxx is the device name you pass to
//     macro Define_Virtual_Device in your main module) to be the unique
//     16-bit value that identifies the device type.  Standard device types
//     have identifiers that are predefined in VMM.H (or VDEFS.H).  All
//     other device type ID's must be obtained from Microsoft.  Your device
//     may use UNDEFINED_DEVICE_ID if it does not export VxD services or
//     require an API entry point.

#define HELLO_DeviceID		UNDEFINED_DEVICE_ID

// (3) Define xxxxx_Init_Order (where xxxxx is the device name you pass to
//     macro Define_Virtual_Device in your main module) to a value that
//     determines where in the sequence of VxDs the VMM initializes your
//     device.   If no other VxDs require your VxD to be initialized prior
//     to them being initialized, use UNDEFINED_INIT_ORDER.

#define HELLO_Init_Order	UNDEFINED_INIT_ORDER

// (4) and (5) Define xxxxx_Major and xxxxxx_Minor (where xxxxx is 
//     the device name you pass to macro Define_Virtual_Device in your 
//     main module) to be the major and minor version numbers for your
//     VxD.

#define HELLO_Major	1
#define HELLO_Minor		0

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
// Class Definitions

// The simple XHelloDevice only processes a single control message, namely
// DEVICE_INIT.  Notice that XHelloDevice is derived from VDevice.  It
// overrides member function OnDeviceInit.  When overriding member functions,
// take care to use exactly the same return value and parameter list.

class XHelloDevice : public VDevice
{
	virtual BOOL OnSysDynamicDeviceInit();
};
