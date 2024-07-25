// testfltr.cpp - test application for keyboard filter driver
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

KEYBOARD_INPUT_DATA keybuf[FIFOSIZE];

HANDLE h, hEvent;

DWORD WINAPI ServiceThread(PVOID hEvent)
{
	ULONG nItems, nBytesRead;
	KEYBOARD_INPUT_DATA* pkey;
	BOOLEAN Terminate = FALSE;

	while(!Terminate)
	{
		WaitForSingleObject(hEvent, INFINITE);

		if (!ReadFile(h, keybuf, sizeof(keybuf), &nBytesRead, NULL))
		{
			printf("read failed %x\n", GetLastError());
			exit(1);
		}

		nItems = nBytesRead/sizeof(KEYBOARD_INPUT_DATA);

		for (pkey = keybuf; nItems; nItems--, pkey++)
		{
			printf("Key: %04x %04x\n", pkey->MakeCode,	pkey->Flags);
			if(pkey->MakeCode == 28) //28 is enter
				Terminate = TRUE;
		}
	}

	return 0;	
}

// -------------------------------------------------------------------------
//
// Main
//
void __cdecl main(int ac, char* av[])
{
	ULONG nReturned;
	ULONG Tid;
	HANDLE ThreadHandle;

	h = CreateFile("\\\\.\\FilterTest",
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);

	if (h == (HANDLE)INVALID_HANDLE_VALUE)
	{
        	fprintf(stderr, "Cannot load open driver, error=%08lx\n",
				GetLastError());
	 	exit(1);
	}

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (! DeviceIoControl(h, IOCTL_HOOK, &hEvent, sizeof(hEvent), NULL, 0, &nReturned, NULL) )
	{
		printf("ERROR: hook call failed, error=%x\n", GetLastError());
		exit(1);
	}

	printf("now filtering . . .  press return to exit\n");

	ThreadHandle = CreateThread(0, 0x1000, ServiceThread, (PVOID)hEvent, 0, &Tid);

	while (	_getch() != 0xd) ;

	if (! DeviceIoControl(h, IOCTL_UNHOOK, NULL, 0, NULL, 0, &nReturned, NULL) )
	{
		printf("ERROR: unhook call failed, error=%x\n", GetLastError());
		exit(1);
	}

	Sleep(1); //Make sure our thread has a chance to exit

	CloseHandle(ThreadHandle); 
	CloseHandle(hEvent);
	CloseHandle(h);

	printf("Keyboard Filter App exiting\n");
}
