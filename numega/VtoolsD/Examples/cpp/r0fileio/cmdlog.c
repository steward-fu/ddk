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

// CMDLOG.C - Win32 console application for use with VCMDLOG.VXD
// Copyright (c) 1996, Compuware Corporation

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

#define NotVxD
#include "vcmdlog.h"

// from vxdldr.h
#define	VXDLDR_APIFUNC_GETVERSION	0
#define	VXDLDR_APIFUNC_LOADDEVICE	1
#define	VXDLDR_APIFUNC_UNLOADDEVICE	2

HANDLE LoadAndOpenVxD(PCHAR VxDFile, PCHAR VxDName);
VOID UnloadVxD(PCHAR VxDName);

HANDLE	hDevice;

VOID usage(void)
{
	fprintf(stderr, "usage: cmdlog open [<log file name>]\n");
	fprintf(stderr, "       cmdlog close\n");
	fprintf(stderr, "       cmdlog unload\n");
	exit(1);
}

//
// -------------------------------------------------------------------------
//
// Main
//
void main(int ac, char* av[])
{
	int i;
	const PCHAR VxDFile = "VCMDLOG.VXD";
	const PCHAR VxDName = "\\\\.\\VCMDLOG";
	const PCHAR DevName = "VCMDLOG";

	DWORD ioCode;
	BOOL status;
	DWORD nRet;

	PCHAR filename;

// process command line

	if (ac < 2 || ac > 3)
		usage();

	if ( stricmp(av[1],"open")==0 ) 
	{
		ioCode = IOCTL_VCMDLOG_OPEN;
		if (ac == 3)
			filename = av[2];
		else
			filename = "command.log"; 
	}
	else if (stricmp(av[1],"close")==0)
	{
		if (ac == 3)
			usage();

		ioCode = IOCTL_VCMDLOG_CLOSE;
		filename = "";
	}
	else if (stricmp(av[1], "unload") == 0)
	{
		UnloadVxD(DevName);
		exit(0);
	}	

	else
		usage();


// Open the VxD

	hDevice = CreateFile(VxDName, 0,0,0, CREATE_NEW, 0, 0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		hDevice = LoadAndOpenVxD(VxDFile, VxDName);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
        	fprintf(stderr, "Cannot load VxD, error=%08lx\n",
				GetLastError());
	 		exit(1);
		}
	}

	status = DeviceIoControl(
		hDevice,
		ioCode,
		filename,
		strlen(filename)+1,
		NULL,
		0,
		&nRet,
		NULL
		);

	printf("Operation %s\n", status ? "succeeded" : "failed");

	CloseHandle(hDevice);
}

HANDLE LoadAndOpenVxD(PCHAR VxDFile, PCHAR VxDName)
{
	HANDLE hldr;
	DWORD IoctlCode;
	DWORD nRet;
	HANDLE hVxD;

	hVxD = INVALID_HANDLE_VALUE;

	hldr = CreateFile(
			"\\\\.\\VXDLDR", 
			GENERIC_READ, 
			FILE_SHARE_READ, 
			0, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	if (hldr == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "unable to open VXDLDR: %x\n", GetLastError());
		return INVALID_HANDLE_VALUE;
	}
	
	IoctlCode = VXDLDR_APIFUNC_LOADDEVICE;

	if (!DeviceIoControl(hldr, IoctlCode, VxDFile, strlen(VxDFile)+1, NULL, 0, &nRet, NULL) )
		fprintf(stderr, "ERROR: load failed.\n");
	else
		hVxD = CreateFile(VxDName, 0,0,0, CREATE_NEW, 0, 0);

	CloseHandle(hldr);
	return hVxD;
}


VOID UnloadVxD(PCHAR VxDName)
{
	HANDLE hldr;
	DWORD IoctlCode;
	DWORD nRet;
	CHAR UnloadName[256];
	BOOL status;

	*(DWORD*)UnloadName = 0xffffffff;
	strcpy(&UnloadName[4], VxDName);

	hldr = CreateFile(
			"\\\\.\\VXDLDR", 
			GENERIC_READ, 
			FILE_SHARE_READ, 
			0, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	if (hldr == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "unable to open VXDLDR: %x\n", GetLastError());
		return;
	}
	
	IoctlCode = VXDLDR_APIFUNC_UNLOADDEVICE;

	status = DeviceIoControl(
			hldr,
			IoctlCode,
			UnloadName,
			strlen(UnloadName)+1,
			NULL, 
			0, 
			&nRet,
			NULL);

	if ( !status )
		fprintf(stderr, "ERROR: unload failed.\n");

	CloseHandle(hldr);
}

