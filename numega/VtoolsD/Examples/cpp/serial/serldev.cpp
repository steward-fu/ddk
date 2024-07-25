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

/**********************************************************************

	File:					serldev.cpp
	Original Author:		John A. Kowalonek
	Created:				May 1, 1998

	Description:

	VSerialPort class sample program.  Derived from Walter Oney
	Serial MiniDriver example and ported to conform with Compuware
	VToolsD class library.

	Version History:

    5/1/98	jak		Initial Definition... used CPort.h, CSerialPort.h
    				from Walter Oney code.  used Comx.h from VToolsD.
	6/6/98	jak		Added SerialDevice pointer to constructor of 
					SerialPort in CreatePort method for timer management.

	Notes:

	override the Register and DriverControl methods in the base class
	VCommPortDriver (even though they AREN'T intended to be overridden)
	
	This allows us to dynamically create the devices in a Win95
	environment. 

	(c) Copyright 1998, Compuware Corporation  All Rights Reserved.
		Portions (c) Copyright 1995 by Walter Oney, Licensed Material.  

*********************************************************************** */

//*******************************************************************
// Device preliminaries
//
#define DEVICE_MAIN
#include "serldev.h"
Declare_Virtual_Device(SERIAL)
#undef DEVICE_MAIN

#ifndef __BORLANDC__
extern "C" PDRIVERCTRLFUNC __cdecl createDCThunk(VCommPortDriver*, BYTE* pThunk, VOID (VCommPortDriver::*pFunc)(DWORD,DWORD,DEVNODE,PVOID,PVOID,...));
extern "C" PCOMMPORTOPENFUNC __cdecl createOpenThunk(VCommPortDriver*, BYTE* pThunk, PPORTDATA (__cdecl VCommPortDriver::*pFunc)(DWORD,PCHAR,VMHANDLE,PDWORD));
extern "C" BOOL (* __cdecl createCloseThunk(VCommPortDriver*, BYTE* pThunk, BOOL  (__cdecl VCommPortDriver::*pFunc)(DWORD,PPORTDATA)))();
extern "C" BOOL (* __cdecl createPortFunctionThunk(VCommPort*, VCommPort*, PBYTE, BOOL, VOID (VCommPort::*pFunc)()))();
#else
extern "C" PDRIVERCTRLFUNC __cdecl createDCThunk(VCommPortDriver*, BYTE* pThunk, VOID (VCommPortDriver::*pFunc)(DWORD,DWORD,DEVNODE,PVOID,PVOID,...));
extern "C" PCOMMPORTOPENFUNC __cdecl createOpenThunk(VCommPortDriver*, BYTE* pThunk, PPORTDATA _cdecl (VCommPortDriver::*pFunc)(DWORD,PCHAR,VMHANDLE,PDWORD));
extern "C" BOOL __cdecl (* __cdecl createCloseThunk(VCommPortDriver*, BYTE* pThunk, BOOL _cdecl (VCommPortDriver::*pFunc)(DWORD,PPORTDATA)))();
extern "C" BOOL __cdecl (* __cdecl createPortFunctionThunk(VCommPort*, VCommPort*, PBYTE, BOOL, VOID (VCommPort::*pFunc)()))();
#endif
				
// This include file defines the port class
//
#include "serlport.h"

BOOL SerialDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR szCmdtail)
{
	return OnSysDynamicDeviceInit();	
}

// Handle control message SYS_DYNAMIC_DEVICE_INIT
//
BOOL SerialDevice::OnSysDynamicDeviceInit()
{

	// -- in Win3.11, we need to construct ALL instances of the ports
	//		that we support here.  In Win95, this happens in the
	//		DriverControl method when called with the DC_Initialize
	//		command.  DC_Initialize is called for EACH port requested --

	//	  For Win3.11, create instances of the port class for all ports
	//	  that the driver supports. The constructors automatically connect
	//	  with the port driver class, SerialDevice. See serldev.H and
	//	  serldev.CPP.

#ifndef WIN40

	new SerialPort("COM1");
	new SerialPort("COM2");

#endif

	// Now call VCommPortDriver::Register, which interacts with VCOMM to start
	// the port initialization process.  It registers the DriverControl method
	// which VCOMM then calls back before returning control to the Register
	// method.  In Win95, the port actually gets created and in both 3.11 and
	// Win95, Member function Initialize() of the port class will be called
	// for each port instance before this call returns.  Linkage is actually
	// handled through DriverControl method.

	Register();

	// Return Success

	return TRUE;
}

// -- creates a new port when needed by DriverControl
//		in Win95.  Default implementation doesn't do anything.
//		Your method should construct one of your VCommPort
//		derived objects	and return a pointer to it! --
VCommPort* SerialDevice::CreatePort(PCHAR szPortName)
{
	return (VCommPort*) new SerialPort(szPortName, this);
}
