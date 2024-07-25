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

// OVLAPP.C - Win32 console application for use with OVRLAPIO.VXD
// Copyright (c) 1996, Compuware Corporation

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>
#include "ovlp.h"

HANDLE	hDevice;
OVERLAPPED o;

//
// -------------------------------------------------------------------------
//
// Main
//
void main(int ac, char* av[])
{
	const PCHAR VxDName = "\\\\.\\OVRLAPIO.VXD";
	DWORD status;

// Set the event field of the overlapped structure to the handle of a 
// ring 3 synchronization event.

	o.hEvent = CreateEvent(0, TRUE, FALSE, NULL);

	if (!o.hEvent)
	{
		fprintf(stderr, "Cannot create event\n");
	 	exit(1);
	}

// Load the VxD

	hDevice = CreateFile(VxDName, 0,0,0,
                        CREATE_NEW, 
                        FILE_FLAG_DELETE_ON_CLOSE | FILE_FLAG_OVERLAPPED,
                        0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
        	fprintf(stderr, "Cannot load VxD, error=%08lx\n",
				GetLastError());
	 	exit(1);
	}

// Send the command to begin the I/O operation

	status =  DeviceIoControl(hDevice,SIMULATE_IO, NULL, 0, 
			NULL, 0, NULL, &o);

// The expected return is FALSE, last error = ERROR_IO_PENDING

	printf("DeviceIoControl returns %x, last error = %x\n", 		
		status, GetLastError());

// The program now waits. When the global timeout handler runs at VxD level,
// it should wake up the program.

	printf("Waiting . . .")	;

	status = WaitForSingleObject(o.hEvent, 10000);	// ten second timeout

	if (status == WAIT_TIMEOUT)
		printf("timed out\n");	// VxD failed to wake us up

	else if (status == 0)		// expected case
		printf("event was signalled, return status=%x\n", 
			o.InternalHigh);
	else				// some strange error occurs
		printf("wait status=%x, error=%x\n", status, GetLastError());

}
