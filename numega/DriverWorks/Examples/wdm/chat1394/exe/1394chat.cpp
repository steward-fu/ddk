// 1394Chat.cpp
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
// This console application demonstrates how to open a handle
// to a device in your driver, and communicate with the driver
// using Read, Write, and DeviceIoControl calls, as appropriate.
//
// This test program attempts to open the device using the
// GUID defined in "..\Chat1394Deviceinterface.h"

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "console.h"

#include <winioctl.h>
#include "..\Chat1394ioctl.h"

#include "..\Chat1394Deviceinterface.h"	// Has class GUID definition
// This function is found in module OpenByIntf.cpp
HANDLE OpenByInterface(GUID* pClassGuid, DWORD instance, PDWORD pError,
	BOOL bOverlapped);

typedef void VOIDFUNC();

// Prototypes
void CloseIfOpen();
void doConsoleChat();

// Global data
							
// Handle to device opened in driver.
//
HANDLE	hDevice = INVALID_HANDLE_VALUE;

// Handle to device opened in driver for overlapped IO.
//
HANDLE	hDeviceOverlapped = INVALID_HANDLE_VALUE;

// Class GUID used to open device
//
GUID ClassGuid = Chat1394Device_CLASS_GUID;


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
int main(int argc, char *argv[])
{
	DWORD	Error;

	printf("Test application 1394Chat starting...\n");

	//Open the handle to the device
	hDevice = OpenByInterface( &ClassGuid, 0, &Error, FALSE);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("ERROR opening device: (%0x) returned from CreateFile\n", GetLastError());
		Exit(1);
	}
	//The overlapped handle allows the I/O to complete asychronously
	// It's used for read operations
	hDeviceOverlapped = OpenByInterface( &ClassGuid, 0, &Error, TRUE);
	if (hDeviceOverlapped == INVALID_HANDLE_VALUE)
	{
		printf("ERROR opening device overlapped: (%0x) returned from CreateFile\n", GetLastError());
		Exit(1);
	}

	printf("Device found, handles open.\n");


	//Start the chat screen
	doConsoleChat();

	CloseIfOpen();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//CloseIfOpen
//
//  Closes the handles to the device
//
void CloseIfOpen()
{
	//Close the handle to the driver
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		// Close the handle to the driver
		if (!CloseHandle(hDevice))
		{
			printf("ERROR: Closing device CloseHandle returns %0x.\n", GetLastError());
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
// DoConsoleChat
//
//  This function does the following:
//		1. Attempt to read data from the device.  If data was available
//         write it to the console
//		2. Save keystrokes to a buffer.  When enter is pressed, write the
//         buffer to the device
//
//   Pressing escape quits.
//
void doConsoleChat()
{
	//Initialize the console screen
	Console TheConsole;
	if(!TheConsole.Initialize())
	{
		printf("Unable to initialize Console screen\n");
		return;
	}

	//Our read and write buffers
	CHAR ReadBuffer[32], WriteBuffer[32];

	//An event to be signaled when data is ready to be read from the device
	HANDLE ReadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(ReadEvent == INVALID_HANDLE_VALUE)
		return;

	OVERLAPPED DeviceOverlapped;
	ULONG nReadDevice;

	DeviceOverlapped.Offset     = 0; 
	DeviceOverlapped.OffsetHigh = 0; 
	DeviceOverlapped.hEvent     = ReadEvent;
	
	// Perform the first device read
	ReadFile(hDeviceOverlapped, &ReadBuffer, 32, &nReadDevice, &DeviceOverlapped);

	ULONG WriteBufferIndex = 0;
	while(TRUE)
	{ 
		// Wait with timeout for device read to complete
		DWORD WaitResult = WaitForSingleObject(ReadEvent, 5);
		if (WaitResult == WAIT_FAILED)
		{
			printf("ERROR: Wait failed for device read.\n");
			break;
		}

		if (WaitResult == WAIT_OBJECT_0)
		{
			// Find out how much data has been read from the device
			if (!GetOverlappedResult(hDeviceOverlapped, &DeviceOverlapped, &nReadDevice, FALSE))
			{
				printf("ERROR: Getting overlapped result from device.  Error returned %0x.\n", GetLastError());
				break;
			}
			// Write the device's data to the log window
			TheConsole.LogOutput(FALSE, (PCHAR)&ReadBuffer, nReadDevice);

			// Perform a new read on the device
			ReadFile(hDeviceOverlapped, (PCHAR)&ReadBuffer, 32, &nReadDevice, &DeviceOverlapped);
		}

		if (_kbhit())
		{
			// Get the character without echo
			CHAR InputCharacter = (CHAR)_getch();

			// The ESC character makes us quit
			if (InputCharacter == 27)
				break;

			//Display the input to the screen.  The function will return TRUE if
			 // there was a carriage return
			if(TheConsole.ProcessInput(InputCharacter, (PCHAR)&WriteBuffer, &WriteBufferIndex, 32))
			{
				// Display to log section of console output window
				TheConsole.LogOutput(TRUE, (PCHAR)&WriteBuffer, WriteBufferIndex);

				// Write buffer to device
				ULONG nWrittenDevice;
				if (!WriteFile(hDevice, (PCHAR)&WriteBuffer, WriteBufferIndex, &nWrittenDevice, NULL))
				{
					printf("ERROR: Writing to device.  Error returned %0x.\n", GetLastError());
					break;
				}

				// Reset the buffer
				WriteBufferIndex = 0;
			}
		}
	}
}

