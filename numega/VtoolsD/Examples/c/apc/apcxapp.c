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

// APCXAPP.C - example application for Asynchronous Procedure Calls
// Copyright (c) 1995, Compuware Corporation
//
// This Win32 console app works with APCEXMPL.VXD to illustrate the usage
// of Asynchronous Procedure Calls. The VxD code is in APCEXMPL.C. 
//
// This app loads the auxilliary VxD by calling CreateFile. It then passes
// the address of the APC function to the VxD through a DeviceIOControl call.
// The app then goes to sleep by calling SleepEx. 
//
// The VxD, which has hooked all file system calls, invokes the APC function
// each time a file is opened. The APC function receives as a paramter a 
// pointer to the name of the file being opened, which is printed to stdout.
//
// The user can terminate the program by typing Ctrl-C.
//

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

#include "apcx.h"

// Static Data

HANDLE	hDevice;     // File handle of the VxD


// The APC function. The VxD queues a request to have this function invoked
// by the system whenever the VxD detects that a file is being opened. The
// parameter is a pointer to the name of the file being opened.
//
// After printing the file name to stdout, this function passes the address
// of the file name back to the VxD for disposal.
//
DWORD WINAPI OpenFileAPC(PVOID param)
{
	printf("Opening: %s\n", param);

	DeviceIoControl(hDevice, APCX_RELEASEMEM, &param, sizeof(PVOID),0,0,0,0);

	return 0;
}


// Main.
// 
void main(int ac, char* av[])
{
	PVOID	inBuf;		// Used to pass data in DeviceIOControl
	DWORD	RetInfo;	// Req'd for DeviceIOControl call
	DWORD	nBytesReturned;	//  "

// Load the auxilliary VxD

	hDevice = CreateFile("\\\\.\\APCEXMPL.VXD", 0,0,0,
                        CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
        	fprintf(stderr, "Cannot load VxD, error=%08lx\n",
				GetLastError());
	 	exit(1);
	}

// Pass the address of the APC to the VxD

	inBuf = OpenFileAPC;
	if ( !DeviceIoControl(hDevice, APCX_REGISTER,
               &inBuf, sizeof(PVOID),
               &RetInfo, sizeof(RetInfo),
               &nBytesReturned, NULL) )
	{
	    	fprintf(stderr, "Failed to register APC\n");
			exit(1);
	}

	printf("press ctrl-C to exit . . .\n");

// Wait for VxD to call APC. The thread that runs the APC must be in an
// "alertable" state. This requires calling WaitSingleObjectEx, 
// WaitMultipleObjectsEx, or SleepEx with the fAlertable flag set to TRUE.

	while (TRUE)
		SleepEx(-1, TRUE);

}
