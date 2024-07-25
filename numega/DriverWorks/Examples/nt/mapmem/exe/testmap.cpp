// testmap.cpp - test application for MAPMEM example
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

// -------------------------------------------------------------------------
//	This test program uses the mapmem.sys DriverWorks sample driver to
//	map an area of an ISA physical memory (typically between 0x00000000
//	and 0x000FFFFF) onto user linear space. The program first connects
//	to the driver via a CreateFile call, then it loops mapping user-entered
//	ISA bus addresses into Ring 3 linear space and displaying the contents
//	of the buffer. After each display, another IoControl is issued to
//	unmap the ISA physical space. 
//
//	Mapping is done by the KMemoryToProcessMap class, which takes the 
//	physical address to be mapped and the caller's process handle. This
//	test program uses a buffer to pass this information to the driver and
//	also to receive the linear address resulting from the map operation. 
//
//	As of the creation of this sample, KMemoryToProcessMap uses a call to
//	the system routine ZwMapViewOfSection to map the required physical
//	memory to a user-side linear address. 
// -------------------------------------------------------------------------

#include <iostream>

using namespace std;

#include <windows.h>
#include <winioctl.h>
#include "mmioctl.h"

// -------------------------------------------------------------------------
//	Function prototypes. 
// -------------------------------------------------------------------------

void display(char *buffer, int address, int length);

// -------------------------------------------------------------------------
// Main Program. 
// -------------------------------------------------------------------------

void __cdecl main(int ac, char* av[])
{

	//--------------------------------------------------------------------
	//	Declare a few handles we'll need. 
	//--------------------------------------------------------------------

	HANDLE h;								//	handle to the mapmem device
	HANDLE ph = OpenProcess (				//	our process handle
					PROCESS_ALL_ACCESS, 
					TRUE, 
					GetCurrentProcessId()); 

	//--------------------------------------------------------------------
	//	Connect to the mapmem driver. 
	//--------------------------------------------------------------------

	h = CreateFile("\\\\.\\MAPMEM0",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

	//--------------------------------------------------------------------
	//	Error exit if couldn't open the MAPMEM device. 
	//--------------------------------------------------------------------

	if (h == INVALID_HANDLE_VALUE)
	{
		cout << "Failed to open MAPMEM device: error code = " 
			 << hex << GetLastError() 
			 << endl;

		exit(1);
	}

	//--------------------------------------------------------------------
	//	Declare a few variables we'll need in the loop. 
	//--------------------------------------------------------------------

	const int Length = 128;					//	how many bytes to display
	const int BufferSize=2;					//	size of our buffer
	const int Sentinel = 0xffffffff;		//	loop control

	const int BufferLength = BufferSize * sizeof(ULONG);

	BOOL b;									//	to hold status returns
	int address= 0;							//	physical address to be read
	ULONG nBytes = 8;						//	size to be transferrred out 

	
	ULONG buffer[BufferSize];				//	our buffer

	//--------------------------------------------------------------------
	// Loop until we type a non-number. 
	//--------------------------------------------------------------------

	while (address != Sentinel)				//	loop while we have a good address
	{

		//-------------------------------------------------------------
		//	Read the address
		//-------------------------------------------------------------

		address = Sentinel;					// cin >> hex changes this 

		cout << "Enter physical address or 'q' to quit: ";	

		cin >> hex >> address;				//	read the address value

		if (address != Sentinel)			//	if non-numeric we exit
		{
			//-----------------------------------------------------
			//	Put the address and the length in the user buffer.
			//-----------------------------------------------------

			buffer[0] = address;			//	address 
			buffer[1] = (int) ph;			//	process handle 

			//-----------------------------------------------------
			//	Ask MAPMEM to map the address. 
			//-----------------------------------------------------

			b = DeviceIoControl(				
					h,
					IOCTL_MAPMEM_MAP_DEVICE,
					NULL,
					0,
					buffer,			
					BufferLength,
					&nBytes,
					NULL
					);

			//-----------------------------------------------------
			//	Error out if we couldn't map the address range. 
			//-----------------------------------------------------

			if (!b)
			{
				cout << "Failed to map device: error = " 
					 << hex << GetLastError() 
					 << endl;
			}

			//-----------------------------------------------------
			//	Otherwise display the buffer. 
			//-----------------------------------------------------

			else
			{
				//-------------------------------------------------
				// Display the device's memory
				//-------------------------------------------------

				display((char *) buffer[0], address, Length);
	
				//-------------------------------------------------
				// Unmap the device
				//-------------------------------------------------
		
				b = DeviceIoControl(
						h,
						IOCTL_MAPMEM_UNMAP_DEVICE,
						NULL,
						0,
						NULL,
						0,
						&nBytes,
						NULL
						);
	
				if (!b)
				{
					cout << "Failed to unmap device: error = "
						 << hex << GetLastError()
						 << endl;
				}

			}
		}
	}
	
	//------------------------------------------------------------------
	//	We finished looping, so, close the handle and exit. 
	//------------------------------------------------------------------
	
	CloseHandle(ph); 
	CloseHandle(h);
	exit(0);
}

// -------------------------------------------------------------------------
//	Function to display a buffer worth of data. 
// -------------------------------------------------------------------------

void display(char *buffer, int address, int length)
{

	//------------------------------------------------------------------
	//	Variable 'i' counts the bytes displayed so far. 
	//------------------------------------------------------------------

	int i = 0;				

	cout.fill(0x30);
	cout << endl; 

	//------------------------------------------------------------------
	//	Loop for all bytes
	//------------------------------------------------------------------

	while (i < length)
	{

		//----------------------------------------------------------
		//	Display the address and increment it. 
		//----------------------------------------------------------
	
		cout.width(8);
		cout << hex << address << ": ";

		address += 16;

		//----------------------------------------------------------
		//	Display the hex bytes for this line. 
		//----------------------------------------------------------

		for (int k=0; k<16; k++) 
		{ 
			unsigned int d = buffer[i+k] & 0xff;
			cout.width(2);
			cout << hex << d << " "; 
		}

		//----------------------------------------------------------
		//	Give it a few blank columns. 
		//----------------------------------------------------------

		printf("   ");

		//----------------------------------------------------------
		//	Display the Ascii bytes for this line. 
		//----------------------------------------------------------

		for (int j=0; j<16; j++) 
		{ 
			char c = buffer[i++];
			if (c < 0x20 || c > 'z') c = '.'; 
			cout << c ; 
		}
		cout << endl;
	}
	cout << endl;
}