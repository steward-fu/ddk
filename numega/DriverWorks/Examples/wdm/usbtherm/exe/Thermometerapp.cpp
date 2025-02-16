// Thermometerapp.cpp
//
// Generated by DriverWizard version 2.02
//
// This console application demonstrates how to open a handle
// to a device in your driver, and communicate with the driver
// using Read, Write, and DeviceIoControl calls, as appropriate.
//
// This test program attempts to open the device with link name
// $ $linkname$ $, which corresponds to Unit 0 of the 
// ThermometerDevice device class in the driver.

//
// You can build this application using the command line compiler with
// the following command:
//
//		cl Thermometerapp.cpp
//


#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>

#include <winioctl.h>
#include "Thermometerioctl.h"


typedef void VOIDFUNC();

// Prototypes
void Usage(void);
void ShowIoctlValues(void);
void	Test_THERMOMETER_READ_DATA(void);

void CloseIfOpen(void);
void doRead(int i);
void doWrite(int i);

// Global data


#define N_IOCODES	1

// Names of IOCTL codes
//
char *IOnames[N_IOCODES+1] =
{
	"THERMOMETER_READ_DATA",
	""
};

// IOCTL codes
//
int IOcodes[N_IOCODES+1] =
{
	THERMOMETER_READ_DATA,
	0
};


// IOCTL test routines
//
VOIDFUNC *IOtesters[N_IOCODES+1] =
{
	Test_THERMOMETER_READ_DATA,
	NULL
};
							

// Handle to device opened in driver.
//
HANDLE	hDevice = INVALID_HANDLE_VALUE;

// Name used to open device
//
char *sLinkName = "\\\\.\\Thermometer_0";

////////////////////////////////////////////////////////////////////////
// OpenByName
//
//		Open a handle to the requested device
//
HANDLE OpenByName(void)
{
	// Create a handle to the driver
	return CreateFile(sLinkName,
					  GENERIC_READ | GENERIC_WRITE,
					  FILE_SHARE_READ,
					  NULL,
					  OPEN_EXISTING,
					  0,
					  NULL);
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

	printf("Test application Thermometerapp starting...\n");

	hDevice = OpenByName();
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
		// Parse ahead to determine numeric value of argument

		if (nArgIndex+1 >= argc) Usage();
		if (!isdigit(argv[nArgIndex+1][0])) Usage();
		val = atoi(argv[nArgIndex+1]);

		switch (argv[nArgIndex][0])
		{

			case 'i':
			case 'I':
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
}


////////////////////////////////////////////////////////////////////////
// Usage
//
//		Print a usage message describing arguments to this program
//
void Usage(void)
{
	printf("Usage: Thermometerapp [i n]\n");
	printf("       i initiates an IO Control Code message with specified index value\n");
	ShowIoctlValues();

	Exit(1);
}



#define	IOCTL_INBUF_SIZE	512
#define	IOCTL_OUTBUF_SIZE	512

////////////////////////////////////////////////////////////////////////
// Test_THERMOMETER_READ_DATA
//
//		Test one Io Control Code
//
// TODO:
//		Pass appropriate arguments to your device and check
//		the return value
//
void Test_THERMOMETER_READ_DATA(void)
{
// Note that Input and Output are named from the point of view
// of the DEVICE:
//		bufInput  supplies data to the device
//		bufOutput is written by the device to return data to this application

	ULONG	bufInput = 0xB;		// Input to device
	ULONG	bufOutput;			// Output from device
	ULONG	nOutput;			// Count written to bufOutput
	ULONG	nTemp;

	printf("Get temp every second, hit any key to stop.\n");

	while( !_kbhit() )
	{
		// Call device IO Control interface (THERMOMETER_READ_DATA) in driver
		printf("Issuing Ioctl to device - ");
		if (!DeviceIoControl(hDevice,
							 THERMOMETER_READ_DATA,
							 &bufInput,
							 sizeof(bufInput),
							 &bufOutput,
							 sizeof(bufOutput),
							 &nOutput,
							 NULL)
		   )
		{
			printf("ERROR: DeviceIoControl returns %0x.\n", GetLastError());
			Exit(1);
		}

		// A quick cursory analysis of the data yeilded this equation for
		// Celcius temperature. It might be incorrect.
		nTemp = ( (bufOutput & 0xff00) >> 8 ) - 27;
		printf("The temperature is %d degrees Celcius.\n", nTemp);
		
		// Sleep for a second
		Sleep(1000);
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
