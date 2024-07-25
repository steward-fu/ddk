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

// APCEXMPL.C - main module for Asynchronouse Procedure Call example VxD
// Copyright (c) 1995 Compuware Corporation.
//
// This example VxD illustrates usage of Asynchronous Procedure Calls (APCs)
// under Windows 95. The APC facility enables a VxD to queue an invocation
// of a function running in a Win32 thread (at ring 3). 
//
// In this example, the VxD queues an APC each time a file is opened. The
// parameter that it passes to the ring 3 function is a pointer to a
// null terminated string containing the name of the file being opened. 
//
// This application code is in module APCXAPP.C. The application causes the
// VxD to be dynamically loaded by calling CreateFile. The VxD must be in
// the current directory or on the standard path in order for it to be 
// successfully loaded.

// ------------------------------------------------------------------------
// Device preliminaries

#define   	DEVICE_MAIN
#include  	"apcexmpl.h"
#undef    	DEVICE_MAIN
#include	"apcx.h"		// definitions common to app

Declare_Virtual_Device(APCEXMPL)	// declare the DDB

// -----------------------------------------------------------------------
// Static data

PVOID	    	OpenFileApc = 0;	// ring 3 address to call
THREADHANDLE 	TheThread = 0;		// thread in which ring 3 call runs
ppIFSFileHookFunc PrevHook;			// previous IFS hook

// ------------------------------------------------------------------------
// Declare prototypes for control message handlers

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
DefineControlHandler(W32_DEVICEIOCONTROL, OnW32Deviceiocontrol);

// ------------------------------------------------------------------------
// Routine to dispatch control messages to handlers
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

		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
		ON_W32_DEVICEIOCONTROL(OnW32Deviceiocontrol);

	END_CONTROL_DISPATCH

	return TRUE;
}

// ------------------------------------------------------------------------
// File System API Hook
//
// This function is installed to hook all file system calls. Each time a
// file is opened, it allocates memory to store the name of the file 
// being opened, forms the file name string, and queues an APC, passing
// the address of the file name string.
//
// When the APC runs, it passes the address of the file name back to the VxD
// through DeviceIOControl in order to allow the VxD to deallocate the
// memory used to store the file name.
//
int _cdecl MyIfsHook(pIFSFunc pfn, int fn, int Drive, int ResType,
		int CodePage, pioreq pir)
{
	char* path;
	_QWORD Result;

	if (TheThread != 0)	// Make sure fully initialized
	{
		switch (fn)	// Branch on file system operation
		{

// In case of file open, execute the call to the APC.

		case IFSFN_OPEN:

// First allocate the memory. The required length is in the io request.

			path = (char*)malloc(pir->ir_ppath->pp_totalLength+3);

// If the memory is successfully allocated, form the file name string.

			if (path != NULL)
			{

// The first two characters of the file name string are the drive letter & ':'

				path[0] = Drive + 'A' - 1;
				path[1] = ':';

// Convert the remainder of the path from Unicode to ascii.

				UniToBCSPath(path+2, pir->ir_ppath->pp_elements,
				  pir->ir_ppath->pp_totalLength, BCS_OEM, &Result);

// Now queue up the APC, passing the address of the file name string as 
// a parameter.

				_VWIN32_QueueUserApc(OpenFileApc, (DWORD)path, TheThread);
			}
			else

// Emit message to debug console if memory allocation failed.

				dprintf("Failed to allocate memory for string\n");
			break;

		default:
			break;
		}
	}

// Pass the call down the IFS chain.

	return (*PrevHook)(pfn, fn, Drive, ResType, CodePage, pir);
}

// ------------------------------------------------------------------------
// Handler for SYS_DYNAMIC_DEVICE_INIT
//
BOOL OnSysDynamicDeviceInit(void)
{
	dprintf("APCEXMPL: Loading\n");
	OpenFileApc = 0;

// Hook file system calls to local procedure

	PrevHook = IFSMgr_InstallFileSystemApiHook(MyIfsHook);

	return TRUE;
}

// ------------------------------------------------------------------------
// Handler for SYS_DYNAMIC_DEVICE_EXIT
//
BOOL OnSysDynamicDeviceExit(void)
{

	dprintf("APCEXMPL: Unloading\n");
	OpenFileApc = 0;

// Unhook file system calls

	IFSMgr_RemoveFileSystemApiHook(MyIfsHook);

	return TRUE;
}


// ------------------------------------------------------------------------
// Handler for W32_DEVICEIOCONTROL
//
// This function is called when
//
//  (1) The app calls CreateFile
//  (2) The app calls DeviceIOControl
//  (3) The app exits (or calls CloseHandle)
//
DWORD OnW32Deviceiocontrol(PIOCTLPARAMS p)
{
	DWORD status;

// Structure member dioc_IOCtlCode determines function.

	switch (p->dioc_IOCtlCode)
	{
	case DIOC_OPEN:		// CreateFile
	case DIOC_CLOSEHANDLE: 	// file closed
		status = 0;	// return OK
		break;

	case APCX_REGISTER:

// When the app registers, grab the APC function address from the input
// input buffer. Store the current ring 0 thread handle.

		OpenFileApc = *(PVOID*)p->dioc_InBuf;
		TheThread = Get_Cur_Thread_Handle();

		status = 0;	// return OK
		break;

	case APCX_RELEASEMEM:

// The APC function calls DeviceIOControl when it is done with the file
// that was passed to it. The VxD frees the memory that was earlier 
// allocated.	

		free(*(PVOID*)p->dioc_InBuf);
		status = 0;
		break;

	default:

// Fail any other calls.

		status = 0xffffffff;
	}

	return status;
}

