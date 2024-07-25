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

// TESTDMA.C - Win32 console application for use with SLAVEDMA.VXD
// Copyright (c) 1996, Compuware Corporation

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

#define NotVxD
#include "slavedma.h"

HANDLE	hDevice;

//
// -------------------------------------------------------------------------
//
// Main
//
void main(int ac, char* av[])
{
	int i;
	const PCHAR DynVxDName = "\\\\.\\SLAVEDMA.VXD";
	const PCHAR StaticVxDName = "\\\\.\\SLAVEDMA";

	PVOID ABigBuffer;
	DWORD nRet;
	BOOL status;

#define NTRIALS 4
	static ULONG sizes[NTRIALS] = {
		0x100,
		0x1000,
		0x10000,
		0x20000
	};

// Load the VxD

	hDevice = CreateFile(StaticVxDName, 0,0,0,
                        CREATE_NEW, 
                        FILE_FLAG_DELETE_ON_CLOSE | FILE_FLAG_OVERLAPPED,
                        0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		hDevice = CreateFile(DynVxDName, 0,0,0,
                        CREATE_NEW, 
                        FILE_FLAG_DELETE_ON_CLOSE | FILE_FLAG_OVERLAPPED,
                        0);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
        	fprintf(stderr, "Cannot load VxD, error=%08lx\n",
				GetLastError());
	 		exit(1);
		}
	}


	for (i = 0; i < NTRIALS; i++)
	{
		printf("Trying DMA with size %08lx (%dKB)\n", sizes[i], sizes[i] >> 10);
		printf("Press a key to continue...\n"); _getch();

		ABigBuffer = malloc(sizes[i]);

		if (! ABigBuffer )
		{
			printf("Couldn't get the memory, exiting ...\n");
			continue;
		}

		status =  DeviceIoControl(hDevice, DIOC_DMA_READ_FROM_DEVICE, ABigBuffer, 
					sizes[i], NULL, 0, &nRet, NULL);


		printf("DeviceIoControl returned %s\n", status ? "TRUE":"FALSE");

		free(ABigBuffer);
	}

}






