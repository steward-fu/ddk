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

// MDAPP32.C - example app for use with MAPDEV example
// Copyright (c) 1995, Compuware Corporation
//

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

#define NotVxD
#include "mapdev.h"

#ifndef __BORLANDC__
#define kbhit _kbhit
#endif

HANDLE		hDevice;


// -------------------------------------------------------------------------
//
// Main
//
void main(int ac, char* av[])
{
#ifdef __BORLANDC__
#pragma warn -par
#endif

	PVOID	inBuf[1];		// buffer for struct pointer to VxD
	DWORD	RetInfo[2];		// buffer to receive data from VxD
	DWORD	cbBytesReturned;	// count of bytes returned from VxD
	MAPDEVREQUEST req;		// map device request structure

// CreateFile recognizes the string "\\.\" as designating a dynamically
// loadable VxD.

	const PCHAR VxDName = "\\\\.\\MAPDEV.VXD";
	const PCHAR VxDNameAlreadyLoaded = "\\\\.\\MAPDEV";

// Load the VxD by calling CreateFile. The VxD file must reside in the
// the Windows system directory.

	hDevice = CreateFile(VxDName, 0,0,0,
                        CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);

	if (hDevice == INVALID_HANDLE_VALUE)
		hDevice = CreateFile(VxDNameAlreadyLoaded, 0,0,0,
                        CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
        	fprintf(stderr, "Cannot open driver, error=%08lx\n",
				GetLastError());
	 	exit(1);
	}

	req.mdr_ServiceID = MDR_SERVICE_MAP;
	req.mdr_PhysicalAddress = (PVOID)0xb8000;
	req.mdr_SizeInBytes = 0x1000;

	inBuf[0] = &req;
	if ( ! DeviceIoControl(hDevice, MDR_SERVICE_MAP,
               inBuf, sizeof(PVOID), NULL, 0,
               &cbBytesReturned, NULL)
	   )
	    	fprintf(stderr, "Failed to map device\n");
	else
	{
		printf("mapped device to address %08lx\nPress any key to continue...", 
			req.mdr_LinearAddress);

		while (!kbhit())
			;
		getchar();

		req.mdr_ServiceID = MDR_SERVICE_UNMAP;
		req.mdr_PhysicalAddress = (PVOID)0xb8000;
		req.mdr_SizeInBytes = 0x1000;

		inBuf[0] = &req;
		DeviceIoControl(
			hDevice,
			MDR_SERVICE_UNMAP,
			inBuf,
			sizeof(PVOID),
			NULL,
			0,
			&cbBytesReturned,
			NULL
			);
	}
}
