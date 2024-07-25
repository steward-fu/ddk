// testintr.cpp - application for interrupt demo driver
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
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

#include "common.h"

// This is a simple console app that exercises the interrupt demo
// driver.
// 
// The test app waits for the driver to notify it that there is
// interrupt data to read. The driver notifies the app by setting
// an event whose handle the app passes to the driver at startup.
// The app simply emits the interrupt data (a count and a timestamp)
// to the console.

TIMESTAMP tsbuf[FIFOSIZE];

HANDLE h, hEvent;

/////////////////////////////////////////////////////////////////
// ServiceThread
// 
// This thread is awakened by the driver to tell the app to come
// and get the interrupt records
//
DWORD WINAPI ServiceThread(PVOID hEvent)
{
	ULONG nItems, nBytesRead;
	PTIMESTAMP pts;

	while (TRUE)
	{
		WaitForSingleObject(hEvent, INFINITE);
		printf("New event:\n");

		if (!DeviceIoControl(
			h, 
			IOCTL_GET_TIMESTAMP_DATA, 
			NULL, 
			0, 
			tsbuf, 
			sizeof(tsbuf),
			&nBytesRead,
			NULL) )
		{
			printf("DeviceIoControl failed %x\n", GetLastError());
			exit(1);
		}

		nItems = nBytesRead/sizeof(TIMESTAMP);

		for (pts = tsbuf; nItems; nItems--, pts++)
			printf("Interrupt time %08x (High=%08x) count %x\n", 
					pts->ts_time, 
					pts->ts_interrupt_count);

	}

	return 0;	
}

/////////////////////////////////////////////////////////////////
//
// Main
//
void __cdecl main(int ac, char* av[])
{
	DWORD Tid;		  
	ULONG error;
	ULONG nRet;

// Open the device

	h = CreateFile("\\\\.\\IntrDemo0",
			GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		error=GetLastError();
       	fprintf(stderr, "Cannot open device, error %d (0x%x)\n", error, error);
	 	exit(1);
	}

// Create an event for notification

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (! DeviceIoControl(
			h, 
			IOCTL_SET_NOTIFICATION_EVENT, 
			&hEvent, 
			sizeof(hEvent),
			NULL,
			0,
			&nRet,
			NULL
			)	)
	{
		printf("Ioctl failed: %x\n", GetLastError());
		exit(1);
	}

// Create the thread that waits for the event

	CreateThread(0, 0x1000, ServiceThread, (PVOID)hEvent, 0, &Tid);

	printf("press return to exit . . .\n");

// Look for a key to terminate

	while (TRUE)
		if (_getche() == 0xd)
			break;

	CloseHandle(hEvent);
	CloseHandle(h);
}
