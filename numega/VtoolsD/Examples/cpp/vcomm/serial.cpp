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

// SERIAL.CPP - main module for VxD SERIAL.VXD
// Copyright (c) 1994, Compuware Corporation

// This example shows the framework for a VCOMM port driver. Although
// it does not actually interact with any hardware, it does demonstrate
// how to set up the basic structure of a port driver.

// If you build this driver and substitute it for SERIAL.VXD (make a 
// backup copy), you will be able to see the trace messages emitted
// from the member functions of the port class, although serial ports
// will not actually work.

//*******************************************************************
// Device preliminaries
//
#define DEVICE_MAIN
#include "serial.h"
Declare_Virtual_Device(SERIAL)
#undef DEVICE_MAIN

// This include file defines the port class
//
#include "comx.h"

BOOL SerialDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR szCmdtail)
{
	return OnSysDynamicDeviceInit();	
}

// Handle control message SYS_DYNAMIC_DEVICE_INIT
//
BOOL SerialDevice::OnSysDynamicDeviceInit()
{
// First create instances of the port class for all ports that the
// driver supports. The constructors automatically connect with the
// port driver class, SerialDevice. See COMX.H and COMX.CPP.

	new Comx("COM1");
	new Comx("COM2");

// Now call SerialDevice::Register, which invokes with VCOMM to start
// the port initialization process. Member function Initialize() of the
// port class will be called for each port instance before this call
// returns.

	Register();

// Return Success

	return TRUE;
}
