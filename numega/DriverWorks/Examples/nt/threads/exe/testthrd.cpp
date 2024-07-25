// testthrd.cpp - application for thread class test
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

// This is a simple test app to exercise the thread demonstration driver.
// It accepts strings as input and passes them to the driver, then
// displays the result.

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

#include "common.h"

//////////////////////////////////////////////////////////////////////////
// Main
//
void __cdecl main(int ac, char* av[])
{
	ULONG nReturned;
	HANDLE h;
	CHAR buf[256];

	h = CreateFile("\\\\.\\Threads0",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
			);

	if (h == (HANDLE)INVALID_HANDLE_VALUE)
	{
       	fprintf(stderr, "Cannot open driver, error=%08lx\n",
			GetLastError());
	 	exit(1);
	}


	while (TRUE)
	{
		printf("Enter a string:");
		gets(buf);
		
		if (! DeviceIoControl(
				h,
				IOCTL_TEST_THREAD,
				buf,
				(DWORD)strlen(buf)+1,
				buf,
				sizeof(buf),
				&nReturned,
				NULL) )
		{
			printf("ERROR: DeviceIoControl call failed, error=%x\n", GetLastError());
			exit(1);
		}
		else
		{	printf("return count is %d\n", nReturned);
			printf("To lower, reversed=|%s|\n", buf);
		}

	}
}
