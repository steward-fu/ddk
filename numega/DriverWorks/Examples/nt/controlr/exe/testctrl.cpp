// testctrl.cpp - application for controller test
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

#include "..\common.h"

//-----------------------------------------------------------------------------
//	Some required constants.
//-----------------------------------------------------------------------------

const int N_THREADS=10;		//	total threads to be opened. 

//-----------------------------------------------------------------------------
//	We need one handle per unit. 
//-----------------------------------------------------------------------------

HANDLE h[NUNITS];

//-----------------------------------------------------------------------------
//	These are the three IoControls that the driver supports. 
//-----------------------------------------------------------------------------

ULONG Ioctls[3] = 
{
	IOCTL_CTRLTEST_EASY_REQUEST,
	IOCTL_CTRLTEST_NEEDS_DEVICE_SERIALIZATION,
	IOCTL_CTRLTEST_NEEDS_CONTROLLER 
};	

//-----------------------------------------------------------------------------
//	Each thread keeps looping through this code: just pump packets. 
//	The packets are sent to one of the units at random. 
//-----------------------------------------------------------------------------

DWORD WINAPI ServiceThread(PVOID id)
{
	DWORD returned;
	BOOL b;
	DWORD n = 0;
	int unit;
	DWORD ioctl;
	CHAR input[NUNITS];	

	while (TRUE)
	{
		unit = rand() % NUNITS;
		ioctl = Ioctls[rand() % 3];

		printf("thread %p, unit %d, ioctl %x\n", id, unit, ioctl);

		b = DeviceIoControl(
			h[unit],
			ioctl,
			input,
			unit,
			NULL,
			0,
			&returned,						
			NULL);

		if (!b) 
		{
			printf("DeviceIoControl call failed, thread %p: %x\n", id, GetLastError());
			exit(1);
		}
	}

	return 0;	
}


//-----------------------------------------------------------------------------
//	The main program opens a file handle per unit and then lets the threads
//	loose. each thread wil then issue Irps at random to one of the units. 
//-----------------------------------------------------------------------------

void __cdecl main(int ac, char* av[])
{
	//-------------------------------------------------------------
	//	Some variables. 
	//-------------------------------------------------------------
	ULONG	i;				// unit counter
	DWORD	Tid;			// id of created thread

	//-------------------------------------------------------------
	//	Open one handle per unit. 
	//-------------------------------------------------------------

	CHAR devName[] = {"\\\\.\\CTRLTST0"};

	for (i=0; i < NUNITS; i++)
	{
		h[i] = CreateFile(devName,
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);

		if (h[i] == INVALID_HANDLE_VALUE)
		{
        	fprintf(stderr, "Cannot load open unit %d, error=%08lx\n",
				i, GetLastError());
		 	exit(1);
		}

		devName[11]++;
	}

	//-------------------------------------------------------------
	//	Fork the threads. 
	//-------------------------------------------------------------

	PCHAR p=0;

	for (i = 0; i < N_THREADS; i++)
	{
		p++;
		CreateThread(0, 0x1000, ServiceThread, p, 0, &Tid);
		printf("Created Thread %d\n", Tid);
	}

	//-------------------------------------------------------------
	//	Finally, wait for an enter keystroke to exit. 
	//-------------------------------------------------------------

	printf("press return to exit . . .\n");

	while (TRUE)
		if (_getche() == 0xd)
			break;
}
