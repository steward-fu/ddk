// 1394Isoc.cpp
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998-2003 Compuware Corporation. All Rights Reserved.
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
// GUID defined in "..\Isoc1394Deviceinterface.h"

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include "..\Isoc1394types.h"
#include "..\Isoc1394ioctl.h"
#include "..\Isoc1394Deviceinterface.h"	// Has class GUID definition

// This function is found in module OpenByIntf.cpp
HANDLE OpenByInterface(GUID* pClassGuid, DWORD instance, PDWORD pError);

typedef void VOIDFUNC();

// Prototypes
void Usage(void);
void CancelLoopback(void);
void DoLoopback(void);
void CloseIfOpen(void);
void doRead(int i);
void doWrite(int i);

// Handle to device opened in driver.
//
HANDLE	hDevice = INVALID_HANDLE_VALUE;

// Class GUID used to open device
//
GUID	ClassGuid = Isoc1394Device_CLASS_GUID;


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
	int		nArgIndex;				// Walk through command line arguments
	int		val;
	DWORD	Error;

	printf("Test application 1394Isoc starting...\n");

	hDevice = OpenByInterface( &ClassGuid, 0, &Error);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("ERROR opening device: (%0x) returned from CreateFile\n", GetLastError());
		Exit(1);
	}
	else
	{
		printf("Device found, handle open.\n");
	}

	// Parse the command line

	if (argc < 2) Usage();

	nArgIndex = 1;
	while (nArgIndex < argc)
	{
		switch (argv[nArgIndex][0])
		{

			case 'r':
			case 'R':
				if (nArgIndex+1 >= argc) Usage();
				if (!isdigit(argv[nArgIndex+1][0])) Usage();
				val = atoi(argv[nArgIndex+1]);
				
				doRead(val);
				break;

			case 'w':
			case 'W':
				if (nArgIndex+1 >= argc) Usage();
				if (!isdigit(argv[nArgIndex+1][0])) Usage();
				val = atoi(argv[nArgIndex+1]);

				doWrite(val);
				break;

			case 'l':
			case 'L':
				DoLoopback();
				break;

			case 'c':
			case 'C':
				CancelLoopback();
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
}


////////////////////////////////////////////////////////////////////////
// doRead
//
//		Read 'n' bytes of data from the device
//
void doRead(int n)
{
	ULONG	nRead = 0;
	char*	buf = (char *) malloc(n);
	if (buf == NULL)
	{
		printf("Failed to allocate buffer for read");
		Exit(1);
	}

	// Read data from driver
	printf("Reading from device - ");
	ReadFile(hDevice, buf, n, &nRead, NULL);
	printf("%d bytes read from device (%d requested).\n", nRead, n);

	for(ULONG next = 0; next < nRead; next++)
		printf("%c, ", buf[next]);

	printf("\n");
	
	free(buf);
}


////////////////////////////////////////////////////////////////////////
// DoWrite
//
//		Write 'n' bytes of data to the device
//
void doWrite(int n)
{
	ULONG	nWritten = 0;
	char*	buf = (char *) malloc(n);;
	if (buf == NULL)
	{
		printf("Failed to allocate buffer for read");
		Exit(1);
	}

	// load buffer with dummy data (abcdefg...)
	for (int i=0; i<n; i++)
	{
		buf[i] = 'a' + (i % 26);
		printf("%c, ", buf[i]);
	}

	printf("\n");

	// Write data to driver
	printf("Writing to device - ");
	WriteFile(hDevice, buf, n, &nWritten, NULL);
	printf("%d bytes written to device (%d attempted).\n", nWritten, n);

	free(buf);
}


////////////////////////////////////////////////////////////////////////
// Usage
//
//		Print a usage message describing arguments to this program
//
void Usage(void)
{
	printf("Usage: 1394Isoc [r n] [w n] [l] [c]\n");
	printf("       r initiates a read of specified number of bytes\n");
	printf("       w initiates a write of specified number of bytes\n");
	printf("       l initiates a loopback test\n");
	printf("       c cancels a loopback test\n");

	printf("Example:\n");
	printf("    Isocapp w 32\n");
	printf("        Write 32 bytes\n");

	printf("    Isocapp l\n");
	printf("        Start a loopback test\n");

	printf("    Isocapp c\n");
	printf("        Cancel a loopback test in progress\n");


	Exit(1);
}


////////////////////////////////////////////////////////////////////////
// CancelLoopback
//
//	Cancels a loopback in progress
//

void CancelLoopback(void)
{
	ULONG	nOutput;						// Count written to bufOutput
	// Call device IO Control interface (ISOC1394_IOCTL_800_SETUP) in driver
	printf("Issuing Ioctl to device - ");
	if (!DeviceIoControl(hDevice,
						 ISOC1394_IOCTL_800_SETUP,
						 NULL,
						 0,
						 NULL,
						 0,
						 &nOutput,
						 NULL)
	   )
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		Exit(1);
	}
}


////////////////////////////////////////////////////////////////////////
// DoLoopback
//
//		Perform a loopback test between two PC's running the isoc1394 driver.
//
//		The driver uses asynchronous commands to start and stop the loopback
//      listener on the other machine.
//
//      1000 packets of 512 bytes each are sent to the driver on the other side,
//       which echos the packets back to our driver.  After all the packets 
//       have completed, statistics are displayed as to the status of each packet sent.
//
//      Possible status' are: 
//        succeeded:  The packet was sent and echoed back successfully
//		  timed out:  The packet was sent but wasn't returned in time
//		  data error: The packet returned did not match the packet sent
//

void DoLoopback(void)
{
	LOOPBACK_STATISTICS Statistics;
	ULONG	nOutput = 0;	// Count written to bufOutput

	// Call device IO Control interface (ISOC1394_IOCTL_802) in driver
	printf("Performing loopback test, please wait - \n");
	if (!DeviceIoControl(hDevice,
						 ISOC1394_IOCTL_802,
						 NULL,				//Input buffer (to device)
						 0,					//Input buffer size
						 &Statistics,		//Output buffer (For collecting packet statistics)
						 sizeof(Statistics),//Output buffer size
						 &nOutput,			//Number of bytes returned
						 NULL)
	   )
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		Exit(1);
	}
	else
	{
		printf("Statistics for the loopback test : \n");
		printf("------------------------------------\n");
		printf("Successful packets: %d\n", Statistics.SuccessfulPackets);
		printf("Timed out packets: %d\n", Statistics.TimeOuts);
		printf("Data error packets: %d\n", Statistics.DataErrors);
	}
}


