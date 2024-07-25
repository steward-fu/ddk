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

// COMMAPP.C - example application for Comm Client VxD
// Copyright (c) 1996, Compuware Corporation
//
// This Win32 console app works with COMMCLNT.VXD to demonstrate how to
// load and call a VCOMM client VxD.
//
// This app loads the auxilliary VxD by calling CreateFile. It then passes the name
// of the communication port to open (e.g. "COM1") through a DeviceIOControl call.
//
// This test app then terminates.  
// each time a file is opened. The APC function receives as a paramter a 
// pointer to the name of the file being opened, which is printed to stdout.
//
// The user can terminate the program by typing Ctrl-C.
//

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>
#include "client.h"

// Static Data

HANDLE	hDevice;     // File handle of the VxD


// Main.
// 
void main(int ac, char* av[])
{
	char	inBuf[32] = "COM2";
	DWORD	RetInfo;		// Req'd for DeviceIOControl call
	DWORD	nBytesReturned;	//  "

// Load the auxilliary VxD

	hDevice = CreateFile("\\\\.\\COMMCLNT.VXD", 0,0,0,
                        CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
        	fprintf(stderr, "Cannot load VxD, error=%08lx\n",
				GetLastError());
	 	exit(1);
	}

	if ( !DeviceIoControl(hDevice, COMMCLNT_openport,
               &inBuf, sizeof(PVOID),
               &RetInfo, sizeof(RetInfo),
               &nBytesReturned, NULL) )
	{
	    	fprintf(stderr, "Failed to initiate COMMCLNT\n");
			exit(1);
	}

	printf("Press ctrl-C to exit . . .\n");

	while (TRUE)
		SleepEx(-1, TRUE);

//	Close and unload the VxD
//	CloseHandle(hDevice);

}
