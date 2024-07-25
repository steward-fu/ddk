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

// REGFSD.C - main module for VxD REGFSD
// Copyright (c) 1996, Compuware Corporation

// This is the main module for the Registry File System Driver (RFSD). 
// RFSD presents the system Registry as file volume mounted on a hard
// drive under Windows 95. It does this by first creating a virtual 
// drive, and then installing a file system on that virtual drive. The
// file system maps file i/o calls to the system Registry. Specifically,
// directories are mapped to keys, files to values, and file contents
// to value data.
//
// RFSD is both a file system driver and a layered block device driver.
// As a file system driver, it translates file i/o requests into 
// calls to services that access the Registry. As a port driver, it
// simulates the existence of a hard drive, and provides routines to
// handle i/o requests to that drive.

// The driver may be built with either Microsoft Visual C++ 2.0 (or 
// later) or Borland C++ 4.02 (or later). 

#define   DEVICE_MAIN
#include  "regfsd.h"
#undef    DEVICE_MAIN

// Here we declare key data structures for the driver. This macro
// invocation defines the Device Data Block for the VxD, the Device
// Registration Packet (DRP), and the IOS Linkage Block (ILB). 
// The macro is defined in vtoolsc.h. 

Declare_Port_Driver(REGFSD,DRP_MISC_PD,REGFSD_NAME,\
	REGFSD_REV,REGFSD_FEATURE,REGFSD_IFR,DRP_BT_ESDI,0)

// Set prototype for control message handler
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	ControlDispatcher
//
// Purpose
//	Handles control messages sent by the system
//
// Parameters
//	dwControlMessage	message identifier
//	register parameters
//
// Return Value
//	The meaning of the return value varies depending on the
//	message, but in general, TRUE means success.
//
// Remarks
//	Like most all layered block device drivers, this driver
//	processes only SYS_DYNAMIC_DEVICE_INIT.
//
BOOL __cdecl ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
	END_CONTROL_DISPATCH

	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	OnSysDynamicDeviceInit
//
// Purpose
//	First level initialization of the RFSD
//
// Parameters
//	None
//
// Return Value
//	Returns TRUE if successful.
//
// Remarks
//	There are two tasks to accomplish. First, the driver must
//	register with the IFSMgr. If this is successful, the driver
//	registers with the I/O Supervisor.
//
BOOL OnSysDynamicDeviceInit()
{
// First register with IFSMgr. Parameter RegMountVolume is the address
// of a function that the IFSMgr will call when a volume is requested
// by an application.

	if (IFSMgr_RegisterMount(RegMountVolume,IFSMGRVERSION,0) == -1)
	{
		DEBUGERROR("call to IFSMgr_RegisterMount failed");
		return FALSE;
	}	  

// Now register with the IOS, passing a pointer to the Device 
// Registration Packet that is declared above using macro
// Declare_Port_Driver. No error returns are defined for this call.

	IOS_Register(&REGFSD_Drp);

// Return TRUE to indicate a successful initialization.

	return TRUE;
}






