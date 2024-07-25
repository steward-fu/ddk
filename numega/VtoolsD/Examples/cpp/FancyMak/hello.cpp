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

// hello.cpp - main module for simplest VxD
//
// Copyright (c) 1994, Compuware Corporation

// This is the main module for the simplest VxD using the VToolsD class
// library.  Its only function is to emit a debug message when it receives
// control message DEVICE_INIT from the VMM.

#define DEVICE_MAIN		// Define this symbol in exactly one module
				// of your VxD, namely the one in which you
				// invoke the Declare_Virtual_Device macro.

#include "hello.h"		// The include file for your device.  Refer
				// to that file for guidelines.
#include "althello.h"

#include LOCKED_CODE_SEGMENT	// These lines switch to the locked code
#include LOCKED_DATA_SEGMENT	// and data segments.

Declare_Virtual_Device(HELLO)	// This macro defines the Device Data Block
				// (DDB) for your device, and defines a
				// function called _CreateDevice that is
				// automatically invoked at initialization
				// time.  The macro requires symbols that
				// you define in your include file.  The
				// parameter is the device name, and must 
				// correspond to names used in the include
				// file.

#include INIT_CODE_SEGMENT	// These lines switch to the init code and
#include INIT_DATA_SEGMENT	// and data segments.

// This function handles control message DEVICE_INIT.  To handle additional
// control messages, simply provide a funtion that overrides the  
// corresponding member function of class XHelloDevice

BOOL XHelloDevice::OnSysDynamicDeviceInit()
{

// dout is the default debug output stream.  You must include vdebug.h
// to use it.  You can use the conventional stream operator ( << ) to 
// format strings and unsigned integers on the debug console.

#ifdef DEBUG
	dout << "Hello, VxD World!" << endl;
	dout << "Here is a number " << GetTheNumber() << endl;
#endif

	return TRUE;
}
