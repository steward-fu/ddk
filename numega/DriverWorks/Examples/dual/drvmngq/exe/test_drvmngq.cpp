// Test_drvmngq.cpp
//
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
//
// This test program attempts to open the device with link name
// $ $linkname$ $, which corresponds to Unit 0 of the 
// DrvmngqDevice device class in the driver.

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>

#include <winioctl.h>
#include "..\drvmngqioctl.h"


typedef void VOIDFUNC();

// Prototypes
void Usage(void);
void ShowIoctlValues(void);
void	Test_DRVMNGQ_IOCTL_800_IMMEDIATE(void);
void	Test_DRVMNGQ_IOCTL_801_QUEUE(void);
void	Test_DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE(void);

void CloseIfOpen(void);
void doKeyboard(void);
void doRandom(void);

// Global data


#define N_IOCODES	3

// Names of IOCTL codes
//
char *IOnames[N_IOCODES+1] =
{
	"DRVMNGQ_IOCTL_800_IMMEDIATE",
	"DRVMNGQ_IOCTL_801_QUEUE",
	"DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE",
	""
};

// IOCTL codes
//
int IOcodes[N_IOCODES+1] =
{
	DRVMNGQ_IOCTL_800_IMMEDIATE,
	DRVMNGQ_IOCTL_801_QUEUE,
	DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE,
	0
};


// IOCTL test routines
//
VOIDFUNC *IOtesters[N_IOCODES+1] =
{
	Test_DRVMNGQ_IOCTL_800_IMMEDIATE,
	Test_DRVMNGQ_IOCTL_801_QUEUE,
	Test_DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE,
	NULL
};
							
char *IrpHandlingNames[DIH_MAX] =
{
	"CANCELED_IN_QUEUE_CANCEL",
	"CANCELED_IN_START_IO",
	"CANCELED_IN_INPROGRESS_CANCEL",
	"CANCELED_IN_CALLBACK",
	"CANCELED_IN_CALLBACK_WORKQUEUE",
	"COMPLETED_IN_DISPATCH",
	"COMPLETED_IN_CALLBACK"
};

LONG IrpHandlingCount[DIH_MAX] =
{
	0,
	0,
	0,
	0,
	0,
	0,
	0
};


// Handle to device opened in driver.
//
HANDLE	hDevice = INVALID_HANDLE_VALUE;

// Handle to device opened in driver for overlapped IO.
//
HANDLE	hDeviceOverlapped = INVALID_HANDLE_VALUE;

// Name used to open device
//
char *sLinkName = "\\\\.\\DrvmngqDevice0";

////////////////////////////////////////////////////////////////////////
// OpenByName
//
//		Open a handle to the requested device
//
HANDLE OpenByName(BOOL bOverlapped)
{
	// Create a handle to the driver
	return CreateFile(
		sLinkName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | ((bOverlapped) ? FILE_FLAG_OVERLAPPED : 0),
		NULL
		);

}



////////////////////////////////////////////////////////////////////////
// Exit
//
//		Print a message and exit   
//
   void Exit(int res)
{
	printf("Exiting...\n\n");
	CloseIfOpen();
	exit(res);
}


////////////////////////////////////////////////////////////////////////
// Main entry point
//
//
int __cdecl main(int argc, char *argv[])
{
	int		nArgIndex;				// Walk through command line arguments
	int		val;

	printf("Test application Test_drvmngq starting...\n");

	// Parse the command line

	if (argc < 2) Usage();

	nArgIndex = 1;
	while (nArgIndex < argc)
	{

		switch (argv[nArgIndex][0])
		{

			case 'k':
			case 'K':
			    printf("Starting keyboard mode.\n");
				doKeyboard();
				break;

			case 'r':
			case 'R':
			    printf("Starting random mode.\n");
				doRandom();
				break;

			case 'i':
			case 'I':
				// Parse ahead to determine numeric value of argument

				if (nArgIndex+1 >= argc) Usage();
				if (!isdigit(argv[nArgIndex+1][0])) Usage();
				val = atoi(argv[nArgIndex+1]);

				if (val >= N_IOCODES)
				{
					printf("IO control code index must be less than %d\n", N_IOCODES);
					ShowIoctlValues();
					Exit(1);
				}
				IOtesters[val]();
				break;

			case '?':
			case 'h':
			default:
				Usage();
		}
		nArgIndex += 2;
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////
// CloseIfOpen
//
//		Close the device if we previously opened a handle to it.
//
void CloseIfOpen(void)
{
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		// Close the handle to the driver
		if (!CloseHandle(hDevice))
		{
			printf("ERROR: CloseHandle returns %0x.\n", GetLastError());
		}
		hDevice = INVALID_HANDLE_VALUE;
	}
	if (hDeviceOverlapped != INVALID_HANDLE_VALUE)
	{
		// Close the handle to the driver
		if (!CloseHandle(hDeviceOverlapped))
		{
			printf("ERROR: Closing device overlapped CloseHandle returns %0x.\n", GetLastError());
		}
		hDeviceOverlapped = INVALID_HANDLE_VALUE;
	}
}


////////////////////////////////////////////////////////////////////////
// doKeyboard
//
//		Loop while accepting keyboard input to send or cancel IOCTLs to the
//			device.
//
//
void doKeyboard(void)
{
	HANDLE hDeviceIoctlEvent;
	OVERLAPPED DeviceOverlapped;
	DWORD dwWaitResult;
	CHAR chStdin;
	ULONG nTransferred;
	BOOL bRequestPending = FALSE;
	BOOL bError = FALSE;
	ULONG nID = 0;
	IOCTL_DATA_INPUT Input;
	IOCTL_DATA_OUTPUT Output;
	ULONG i;

	hDevice = OpenByName(FALSE);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("ERROR opening device: (%0x) returned from CreateFile\n", GetLastError());
		Exit(1);
	}
	printf("Device found, handle open.\n");

	Output.nID = 0;
	Output.handling = DIH_CANCELED_IN_QUEUE_CANCEL;

	printf("Type i for %s\n", IOnames[0]);
	printf("Type q for %s\n", IOnames[1]);
	printf("Type c for %s\n", IOnames[2]);
	printf("Type x to cancel\n\n");

	// Create overlapped event for device read
	hDeviceIoctlEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hDeviceIoctlEvent == NULL)
	{
		printf("Failed to allocate device read event chat\n");

		// Leave and declare an error
		Exit(1);
	}

	// Perform the first device read
	DeviceOverlapped.Offset     = 0; 
	DeviceOverlapped.OffsetHigh = 0; 
	DeviceOverlapped.hEvent     = hDeviceIoctlEvent;

	while (!bError)
	{ 
		// Wait with timeout for device read to complete
		dwWaitResult = WaitForSingleObject(hDeviceIoctlEvent, 5);

		if (dwWaitResult == WAIT_FAILED)
		{
			printf("ERROR: Wait failed.\n");
			bError = TRUE;
			break;
		}
		if (dwWaitResult == WAIT_OBJECT_0)
		{
			if (!ResetEvent( hDeviceIoctlEvent ))
			{
				printf("ERROR: Resetting event.  Error returned %0x.\n", GetLastError());
				bError = TRUE;
				break;
			}

			if (!GetOverlappedResult(hDeviceOverlapped, &DeviceOverlapped, &nTransferred, FALSE))
			{
				if (GetLastError() == ERROR_OPERATION_ABORTED)
				{
					if (nTransferred)
					{
						printf("IOCTL ID %9u canceled.  Driver handling was %s\n", Output.nID,
							IrpHandlingNames[Output.handling]);
						IrpHandlingCount[Output.handling]++;
					}
					else
					{
						printf("IOCTL ID %9u canceled.  Driver handling was %s\n", nID,
							IrpHandlingNames[DIH_CANCELED_IN_QUEUE_CANCEL]);
						IrpHandlingCount[DIH_CANCELED_IN_QUEUE_CANCEL]++;
					}
				}
				else
				{
					printf("ERROR: Getting overlapped result from device.  Error returned %0x.\n", GetLastError());
					bError = TRUE;
					break;
				}
			}
			else
			{
				printf("IOCTL ID %9u completed.  Driver handling was %s\n", Output.nID,
					IrpHandlingNames[Output.handling]);
				IrpHandlingCount[Output.handling]++;
			}

			// Clear the state of output for next time
			Output.nID = 0;
			Output.handling = DIH_CANCELED_IN_QUEUE_CANCEL;

			nID++;

			bRequestPending = FALSE;
		}

		if (_kbhit())
		{
			// Get the character without echo
			chStdin = (CHAR)_getch();

			// The ESC character makes us quit
			if (chStdin == 27)
			{
				break;
			}
			if (chStdin == 'i' && !bRequestPending)
			{
				Input.nID = nID;
				printf( "IOCTL ID %9u issued - type %s\n", nID, IOnames[0]);
				DeviceIoControl(hDeviceOverlapped,
					DRVMNGQ_IOCTL_800_IMMEDIATE,
					&Input,
					sizeof(IOCTL_DATA_INPUT),
					&Output,
					sizeof(IOCTL_DATA_OUTPUT),
					NULL,
					&DeviceOverlapped);
				bRequestPending = TRUE;
			}
			if (chStdin == 'q' && !bRequestPending)
			{
				Input.nID = nID;
				printf( "IOCTL ID %9u issued - type %s\n", nID, IOnames[1]);
				DeviceIoControl(hDeviceOverlapped,
					DRVMNGQ_IOCTL_801_QUEUE,
					&Input,
					sizeof(IOCTL_DATA_INPUT),
					&Output,
					sizeof(IOCTL_DATA_OUTPUT),
					NULL,
					&DeviceOverlapped);
				bRequestPending = TRUE;
			}
			if (chStdin == 'c' && !bRequestPending)
			{
				Input.nID = nID;
				printf( "IOCTL ID %9u issued - type %s\n", nID, IOnames[2]);
				DeviceIoControl(hDeviceOverlapped,
					DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE,
					&Input,
					sizeof(IOCTL_DATA_INPUT),
					&Output,
					sizeof(IOCTL_DATA_OUTPUT),
					NULL,
					&DeviceOverlapped);
				bRequestPending = TRUE;
			}
			if (chStdin == 'x')
			{
				if (!CancelIo( hDeviceOverlapped ))
				{
					printf("ERROR: Canceling overlapped I/O on device.  Error returned %0x.\n", GetLastError());
					bError = TRUE;
					break;
				}
			}
		}
	}
	printf("\n\n");
	for (i = 0; i < DIH_MAX; i++)
		printf("Driver handling %s occured %u times\n", IrpHandlingNames[i],
			IrpHandlingCount[i]);
}

////////////////////////////////////////////////////////////////////////
// doRandom
//
//		Loop while generating random IOCTLs, and randomly canceling them
//			to the device.
//
//

BOOL bLogging = TRUE;					// Global
DWORD WINAPI RandomThread(PVOID tID);	// Forward

void doRandom(void)
{
	ULONG	i;
	DWORD	Tid;			// id of created thread
	CHAR chStdin;
	int nThreads;

	hDeviceOverlapped = OpenByName(TRUE);
	if (hDeviceOverlapped == INVALID_HANDLE_VALUE)
	{
		printf("ERROR opening device overlapped: (%0x) returned from CreateFile\n", GetLastError());
		Exit(1);
	}
	printf("Device found, handle open.\n\n");

	printf("Once running, type esc to exit\n");
	printf("Once running, type l to toggle logging\n");
	printf("Once running, type r to get a report of IOCTL handling\n");

	printf("Number of threads to create? (1-9)\n");
	while (TRUE)
	{
		// Get the character without echo
		chStdin = (CHAR)_getch();
		if (chStdin >= '1' && chStdin <= '9')
		{
			nThreads = atoi(&chStdin);
			break;
		}
	}

	for (i = 0; i < (ULONG)nThreads; i++)
		CreateThread(0, 0x1000, RandomThread, (PVOID)i, 0, &Tid);

	while (TRUE)
	{
		// Get the character without echo
		chStdin = (CHAR)_getch();

		// The ESC character makes us quit
		if (chStdin == 27)
		{
			break;
		}

		// l toggels on and off logging
		if (chStdin == 'l')
		{
			bLogging = (bLogging == TRUE) ? FALSE : TRUE;
		}

		// r prints a report of IOCTL handling
		if (chStdin == 'r')
		{
			printf("\n\n");
			for (i = 0; i < DIH_MAX; i++)
				printf("Driver handling %s occured %d times\n", IrpHandlingNames[i],
					IrpHandlingCount[i]);
		}
	}
	Exit(0);
}

DWORD WINAPI RandomThread(PVOID tID)
{	
	
	HANDLE hDeviceIoctlEvent;
	OVERLAPPED DeviceOverlapped;
	DWORD dwWaitResult;
	ULONG nTransferred;
	BOOL bError = FALSE;
	DWORD ioctl;
	int IoctlIndex;
	int CancelCondition;
	int TimeoutsToCancel = -1;
	ULONG nID = 0;
	IOCTL_DATA_INPUT Input;
	IOCTL_DATA_OUTPUT Output;
	//ULONG i;

	// Create overlapped event for device read
	hDeviceIoctlEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hDeviceIoctlEvent == NULL)
	{
		printf("Failed to allocate overlapped event\n");

		// Leave and declare an error
		Exit(1);
	}

	// Perform the first device read
	DeviceOverlapped.Offset     = 0; 
	DeviceOverlapped.OffsetHigh = 0; 
	DeviceOverlapped.hEvent     = hDeviceIoctlEvent;

	// Setup input and clear the state of output.
	Input.nID = nID;
	Output.nID = 0;
	Output.handling = DIH_CANCELED_IN_QUEUE_CANCEL;

	IoctlIndex = rand() % 3;
	ioctl = IOcodes[IoctlIndex];
	printf( "TID %p IRPID %6u issued - type %s\n", tID, nID, IOnames[IoctlIndex]);
	DeviceIoControl(hDeviceOverlapped,
		ioctl,
		&Input,
		sizeof(IOCTL_DATA_INPUT),
		&Output,
		sizeof(IOCTL_DATA_OUTPUT),
		NULL,
		&DeviceOverlapped);

	while (!bError)
	{ 
		// Wait with 5ms timeout for device read to complete
		dwWaitResult = WaitForSingleObject(hDeviceIoctlEvent, 5);

		if (dwWaitResult == WAIT_FAILED)
		{
			printf("ERROR: Wait failed.\n");
			bError = TRUE;
			break;
		}
		if (dwWaitResult == WAIT_OBJECT_0)
		{
			if (!ResetEvent( hDeviceIoctlEvent ))
			{
				printf("ERROR: Resetting event.  Error returned %0x.\n", GetLastError());
				bError = TRUE;
				break;
			}

			if (!GetOverlappedResult(hDeviceOverlapped, &DeviceOverlapped, &nTransferred, FALSE))
			{
				if (GetLastError() == ERROR_OPERATION_ABORTED)
				{
					if (nTransferred)
					{
						if (bLogging)
							printf("TID %p IRPID %6u canceled.  Driver handling was %s\n",
								tID, Output.nID, IrpHandlingNames[Output.handling]);
						IrpHandlingCount[Output.handling]++;
					}
					else
					{
						if (bLogging)
							printf("TID %p IRPID %6u canceled.  Driver handling was %s\n",
								tID, nID, IrpHandlingNames[DIH_CANCELED_IN_QUEUE_CANCEL]);
						InterlockedIncrement(&IrpHandlingCount[DIH_CANCELED_IN_QUEUE_CANCEL]);
					}
				}
				else
				{
					printf("ERROR: Getting overlapped result from device.  Error returned %0x.\n", GetLastError());
					bError = TRUE;
					break;
				}
			}
			else
			{
				if (bLogging)
					printf("TID %p IRPID %6u completed.  Driver handling was %s\n",
						tID, Output.nID, IrpHandlingNames[Output.handling]);
				InterlockedIncrement(&IrpHandlingCount[Output.handling]);
			}

			// Clear the state of output for next time and setup input
			Output.nID = 0;
			Output.handling = DIH_CANCELED_IN_QUEUE_CANCEL;
			nID++;
			Input.nID = nID;

			IoctlIndex = rand() % 3;
			ioctl = IOcodes[IoctlIndex];
			if (bLogging)
				printf( "TID %p IRPID %6u issued - type %s\n", tID, nID, IOnames[IoctlIndex]);
			DeviceIoControl(hDeviceOverlapped,
				ioctl,
				&Input,
				sizeof(IOCTL_DATA_INPUT),
				&Output,
				sizeof(IOCTL_DATA_OUTPUT),
				NULL,
				&DeviceOverlapped);

			CancelCondition = rand() % 3;
			switch (CancelCondition)
			{
			case 0:
				// No cancel
				TimeoutsToCancel = -1;
				break;
			case 1:
				// Long cancel
				// Calculate how many 5ms timeouts in the next 5 seconds
				// to wait before canceling.  1000*5=5000 ms.
				TimeoutsToCancel = rand() % 1000;
				break;
			case 2:
				// Short cancel
				// Calculate how many 5ms timeouts in the next tenth second
				// to wait before canceling.  40*5=200 ms.
				TimeoutsToCancel = rand() % 40;
				break;
			}
		}

		if (TimeoutsToCancel > 0)
		{
			TimeoutsToCancel--;
		}
		else if (TimeoutsToCancel == 0)
		{
			// Make timeouts negative so we only do this once.
			TimeoutsToCancel--;

			if (!CancelIo( hDeviceOverlapped ))
			{
				printf("ERROR: Canceling overlapped I/O on device.  Error returned %0x.\n", GetLastError());
				bError = TRUE;
				break;
			}
			else
			{
				if (bLogging)
					printf("TID %p IRPID %6u cancel request issued\n", tID, nID);
			}
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Usage
//
//		Print a usage message describing arguments to this program
//
void Usage(void)
{
	printf("Usage: Test_drvmngq [i n] [k] [r]\n");
	printf("       i initiates an IO Control Code message with specified index value\n");
	ShowIoctlValues();
	printf("       k initiates keyboard mode, in which key hits send/cancel IOCTLs.\n");
	printf("       r initiates random mode, in which IOCTLs are randomly sent/canceled.\n");
	printf("Example:\n");
	printf("    Test_drvmngq i 0\n");
	printf("        IOCTL index 0 is sent to the device.\n");

	Exit(1);
}



#define	IOCTL_INBUF_SIZE	512
#define	IOCTL_OUTBUF_SIZE	512

////////////////////////////////////////////////////////////////////////
// Test_DRVMNGQ_IOCTL_800_IMMEDIATE
//
//		Test one Io Control Code
//
// TODO:
//		Pass appropriate arguments to your device and check
//		the return value
//
void Test_DRVMNGQ_IOCTL_800_IMMEDIATE(void)
{
// Note that Input and Output are named from the point of view
// of the DEVICE:
//		Input  supplies data to the device
//		Output is written by the device to return data to this application

	IOCTL_DATA_INPUT	Input;
	IOCTL_DATA_OUTPUT	Output;
	ULONG	nOutput;

	hDevice = OpenByName(FALSE);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("ERROR opening device: (%0x) returned from CreateFile\n", GetLastError());
		Exit(1);
	}
	printf("Device found, handle open.\n");

	Input.nID = 0;

	// Call device IO Control interface (DRVMNGQ_IOCTL_800_IMMEDIATE) in driver
	printf("Issuing Ioctl to device - ");
	if (!DeviceIoControl(hDevice,
		DRVMNGQ_IOCTL_800_IMMEDIATE,
		&Input,
		sizeof(IOCTL_DATA_INPUT),
		&Output,
		sizeof(IOCTL_DATA_OUTPUT),
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		Exit(1);
	}
}

////////////////////////////////////////////////////////////////////////
// Test_DRVMNGQ_IOCTL_801_QUEUE
//
//		Test one Io Control Code
//
// TODO:
//		Pass appropriate arguments to your device and check
//		the return value
//
void Test_DRVMNGQ_IOCTL_801_QUEUE(void)
{
// Note that Input and Output are named from the point of view
// of the DEVICE:
//		Input  supplies data to the device
//		Output is written by the device to return data to this application

	IOCTL_DATA_INPUT	Input;
	IOCTL_DATA_OUTPUT	Output;
	ULONG	nOutput;

	hDevice = OpenByName(FALSE);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("ERROR opening device: (%0x) returned from CreateFile\n", GetLastError());
		Exit(1);
	}
	printf("Device found, handle open.\n");

	Input.nID = 0;

	// Call device IO Control interface (DRVMNGQ_IOCTL_801_QUEUE) in driver
	printf("Issuing Ioctl to device - ");
	if (!DeviceIoControl(hDevice,
		DRVMNGQ_IOCTL_801_QUEUE,
		&Input,
		sizeof(IOCTL_DATA_INPUT),
		&Output,
		sizeof(IOCTL_DATA_OUTPUT),
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		Exit(1);
	}
}

////////////////////////////////////////////////////////////////////////
// Test_DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE
//
//		Test one Io Control Code
//
// TODO:
//		Pass appropriate arguments to your device and check
//		the return value
//
void Test_DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE(void)
{
// Note that Input and Output are named from the point of view
// of the DEVICE:
//		Input  supplies data to the device
//		Output is written by the device to return data to this application

	IOCTL_DATA_INPUT	Input;
	IOCTL_DATA_OUTPUT	Output;
	ULONG	nOutput;

	hDevice = OpenByName(FALSE);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("ERROR opening device: (%0x) returned from CreateFile\n", GetLastError());
		Exit(1);
	}
	printf("Device found, handle open.\n");

	Input.nID = 0;

	// Call device IO Control interface (DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE) in driver
	printf("Issuing Ioctl to device - ");
	if (!DeviceIoControl(hDevice,
		DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE,
		&Input,
		sizeof(IOCTL_DATA_INPUT),
		&Output,
		sizeof(IOCTL_DATA_OUTPUT),
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		Exit(1);
	}
}

////////////////////////////////////////////////////////////////////////
// ShowIoctlValues
//
//		Print list of IO Control Code values for usage display
//
void ShowIoctlValues(void)
{
	int i;

	for (i=0; i<N_IOCODES; i++)
	{
		if (i==0)
			printf( "         IO control code index %d is code %s [%x]\n", i, IOnames[i], IOcodes[i]);
		else
			printf( "                               %d is code %s [%x]\n", i, IOnames[i], IOcodes[i]);
	}
}
