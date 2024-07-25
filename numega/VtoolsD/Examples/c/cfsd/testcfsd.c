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

// TESTCFSD.C - main module for VxD TESTCFSD


// TESTCFSD is the skeleton of a Character File System Driver. At
// DEVICE_INIT time, it registers as a set of character devices with 
// IFSMgr.  
//
// The devices registered have names of the form "xxxxxn", where xxxxx
// is the base of the name, and is defined by the constant 
// CHAR_DEVICE_NAME_BASE (see testcfsd.h). The "n" portion of the device
// name is a hex digit from 0 to (NUNITS-1). In this example, the
// base is CHARDEV and NUNITS is 3, so device CHARDEV0, CHARDEV1, CHARDEV2,
// and CHARDEV3 are registered.
//
// When a CFSD registers, it passes the address of its Mount function to
// IFSMgr. When Mount is called, it must set data member ir_vfunc of the
// iorequest structure. This member points to a table of volume function
// addresses.	For this example, volume level functions are all in 
// volfuncs.c.
//
// When volume function FS_Open is called, it must supply the table of
// handle (file) level functions. In this example, all the handle level
// functions are in hndlfunc.c.
//
// This example doesn't do much. It is simply intended to show the basic
// operation of a character FSD. Local (block device) and remote (network)
// FSD have similar structure, but use different calls to register with
// IFSMgr.

#define   DEVICE_MAIN
#include  "testcfsd.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(TESTCFSD)

DefineControlHandler(DEVICE_INIT, OnDeviceInit);

WORD uDeviceName[NUNITS][CHAR_DEVICE_NAME_LENGTH+2]; 
string_t DeviceNames[NUNITS+1];
BOOL bMounted;

extern volfunc VolumeFunctions;
extern hndlmisc HandleFunctions;

// ------------------------------------------------------------------------
// ControlDispatcher - dispatcher for system control messages
//
BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_DEVICE_INIT(OnDeviceInit);

	END_CONTROL_DISPATCH

	return TRUE;
}

// ------------------------------------------------------------------------
// Mount
//
// Input:
//	ir_flags is the mount type:
//
//	IR_FSD_MOUNT
//		Called on first access to media.
//		  ir_volh is the system volume handle
//		  ir_mntdrv is drive identifier
//		  ir_fh is the IFS resource handle for the volume
//
//	IR_FSD_VERIFY
//		Verify that the mounted volume has not changed.
//	IR_FSD_UNLOAD
//		Unload a currently mounted volume unconditionally
//	IR_FSD_MOUNT_CHILD
//		Mount child volume of a currently mounted volume
//	IR_FSD_MAP_DRIVE
//		Change drive letter for a volume
//	IR_FSD_UNMAP_DRIVE
//		Restore mapped drive to original letter
//
// Output:
//	ir_error is the operation status (and the return value)
//
//	IR_FSD_MOUNT
//		ir_vfunc is set to point to the VolumeFunctions structure
//		ir_rh is set to a value that IFS passes in for future
//			references to this volume.
//
//	IR_FSD_VERIFY
//	IR_FSD_UNLOAD
//	IR_FSD_MOUNT_CHILD
//	IR_FSD_MAP_DRIVE
//	IR_FSD_UNMAP_DRIVE

int _cdecl Mount(pioreq pir)
{
	switch (pir->ir_flags)
	{
	case IR_FSD_MOUNT:
		fsdtrace("Mount", pir);
		pir->ir_vfunc = &VolumeFunctions;	
		pir->ir_error = 0;
		pir->ir_rh = &The_DDB;
		break;

	default:
		pir->ir_error = -1;
	}

	return pir->ir_error;
}

// ------------------------------------------------------------------------
// OnDeviceInit - handler for control message DEVICE_INIT
//
BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	int i;
	unsigned char* asciiName = CHAR_DEVICE_NAME_BASE;
	PathElement* pDevice;
	_QWORD result;

	bMounted = FALSE;

// This routine is responsible for setting up the array of device name
// pointers to pass to IFSMgr_RegisterCFSD. Each element of the array
// DeviceNames is a pointer to a PathElement, which is a counted unicode
// string. Each unicode string is also null terminated. The two dimensional
// array uDeviceName stores the unicode strings. The end of the array
// DeviceNames is marked by a null pointer. 

// This loop initializes the set of PathElements that hold the device names
// in uDeviceName. 

	for (i=0; i < NUNITS; i++)
	{

// Construct the ascii form of the current device name by setting the last 
// character of the ascii name to the unit number. 
		asciiName[strlen(asciiName)-1] = "0123456789abcdef"[i];

// Set the current element of DeviceNames to point to the current PathElement.
		DeviceNames[i] = uDeviceName[i];

// Alias pDevice to the storage for the current PathElement
		pDevice = (PathElement*)DeviceNames[i];

// Set the length of the current PathElement (length is stored in bytes)
		pDevice->pe_length = (strlen(asciiName)+1) * sizeof(WORD);

// Convert the current ascii name to unicode into the current PathElement
		BCSToUni (pDevice->pe_unichars, asciiName, pDevice->pe_length/sizeof(WORD), 
			BCS_WANSI, &result);
	}

// The DeviceNames array is now initialized. Terminate with a null.
	DeviceNames[NUNITS] = 0;

// Now we register this Character FSD with IFSMgr, passing in the address of 
// the routine to be called when any of the named devices is first accessed.

	return (IFSMgr_RegisterCFSD(Mount, IFSMGRVERSION, DeviceNames) != -1);
}

