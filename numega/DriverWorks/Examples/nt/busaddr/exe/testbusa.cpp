// testbusa.cpp - application for bus address test
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

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

#include "common.h"

// -------------------------------------------------------------------------
//
// Main
//
void __cdecl main(int ac, char* av[])
{
	ULONG i;
	ULONG nReturned;
	HANDLE h;

	printf("This is the test for class KBusAddress\n");
	printf("It is designed to run on a full screen console\n");
	printf("Press any key to continue ...\n");
	_getche();

	h = CreateFile("\\\\.\\BusAddr0",
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);

	if (h == (HANDLE)INVALID_HANDLE_VALUE)
	{
        	fprintf(stderr, "Cannot open driver, error=%08lx\n",
				GetLastError());
	 	exit(1);
	}

	// first we'll test port i/o by getting the date from the CMOS ram (x86 only)

	DATEINFO date;

	if (! DeviceIoControl(h, IOCTL_BUSADDR_GETDATE, NULL, 0, &date, sizeof(date),
		&nReturned, NULL) )
	{
		printf("ERROR: Date info call failed, error=%x\n", GetLastError());
		_getche();
	}
	else
		printf("Today's date is %x/%x/%x\n", date.month, date.day, date.year);

	// now we'll test memory mapping

	USHORT* textbuf = new USHORT[80*50];

	// reading the BusAddr device read the VGA text memory (at b8000).

	if (! ReadFile(h, textbuf, 80*50*sizeof(USHORT), &nReturned, NULL) )
	{
		printf("ERROR: read call failed, error=%x\n", GetLastError());
		_getche();
	}

	// now we'll change the screen color by modifying the odd bytes in the
	// buffer and writing it back to the device

	UCHAR* p = (UCHAR*)textbuf;

	for (i = 1; i < 80*50*sizeof(USHORT); i+=2)
		p[i] = 0x24;

	SetFilePointer(h, 0, 0, FILE_BEGIN);

	if (! WriteFile(h, textbuf, 80*50*sizeof(USHORT), &nReturned, NULL) )
	{
		printf("ERROR: write call failed, error=%x\n", GetLastError());
		_getche();
	}
	else
	{
		printf("Should have new screen color now\n");
		_getche();
	}

	// now we'll get a direct map of the vgatext for the user memory, and
	// change it directly.

	HANDLE hProcess = GetCurrentProcess();
	PUSHORT VgaMap;

	if (! DeviceIoControl(h, IOCTL_BUSADDR_MAP_VGATEXT, &hProcess, sizeof(HANDLE), 
		&VgaMap, sizeof(PUSHORT), &nReturned, NULL) )
	{
		printf("ERROR: map call failed, error=%x\n", GetLastError());
		_getche();
	}
	else
	{
		printf("Vgatext mapped to %p in user space\n", VgaMap);
		if (_getche() == 3) exit(0);
	}
	
	p = (UCHAR*)VgaMap;

	for (i = 1; i < 80*50*sizeof(USHORT); i+=2)
		p[i] = 0x42;


	printf("BusAddr test exiting\n");

	CloseHandle(h);
}
