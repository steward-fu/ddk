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

// tstshmem.cpp - test application for sharemem sample
// Copyright (c) 1998 Compuware Corporation

#include <windows.h>
#include <stdio.h>
#include <winioctl.h>
#include "..\smioctl.h"

#define APP_BUFFER_SIZE 8192

// Main.
// 
void main(int ac, char* av[])
{
	DWORD	nReturned;
	PCHAR 	AppBuffer;
	MAP_BUFFER_ARGS mbargs;
	BOOL	DevIoStatus;
	PVOID 	MappedAddress;
	HANDLE  hDevice;

// Load the auxilliary VxD

	hDevice = CreateFile("\\\\.\\SHAREMEM.VXD", 0,0,0,
                        CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);

// Verify successful load

	if (hDevice == INVALID_HANDLE_VALUE)
	{
        	fprintf(stderr, "Cannot load VxD, error=%08lx\n",
				GetLastError());
	 	exit(1);
	}


// Allocate some memory

	AppBuffer = (PCHAR)malloc(APP_BUFFER_SIZE);	

	if ( AppBuffer == NULL )
	{
		fprintf(stderr, "Failed to allocate buffer\n");
		exit(1);
	}

// copy a verification string to the buffer

	strcpy(AppBuffer, "AbCdEfGhIjKlMnOpQrStUvWxYz");

// Map the allocated buffer in the vxd

	mbargs.m_address = AppBuffer;
	mbargs.m_size = APP_BUFFER_SIZE;

	DevIoStatus = 
		DeviceIoControl(
			hDevice, 
			SHAREMEM_IOCTL_MAP_BUFFER, 
			&mbargs, 
			sizeof(mbargs),	
			&MappedAddress,
			sizeof(MappedAddress),
			&nReturned,
			NULL
			);


	if ( ! DevIoStatus )
	{
	    	fprintf(stderr, "Failed to map buffer\n");
			exit(1);
	}


// unmap it

	DevIoStatus = DeviceIoControl(hDevice, SHAREMEM_IOCTL_UNMAP_BUFFER, 0, 0, 0, 0, &nReturned, NULL);

	if ( ! DevIoStatus )
	{
	    	fprintf(stderr, "Failed to unmap buffer\n");
			exit(1);
	}
	else
		free(AppBuffer);	// never free while still mapped

	CloseHandle(hDevice);
}
















