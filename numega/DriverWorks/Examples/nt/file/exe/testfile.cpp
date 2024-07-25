// testfile.cpp - application for class KFile test
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998-2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#include <iostream>

using namespace std;

#include <windows.h>
#include <winioctl.h>

#include "..\common.h"

// -------------------------------------------------------------------------
//	Function prototypes. 
// -------------------------------------------------------------------------

void Display		(char *buffer, int address, int length);
void ReportError	(char *s, int d=GetLastError());

// -------------------------------------------------------------------------
//	One-liners. 
// -------------------------------------------------------------------------

#define alpha(c)	(((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define special(c)	((c) == '\\' || (c) == '.' || (c) == '$')
#define allowed(c)	(alpha(c) || special(c))

// -------------------------------------------------------------------------
// The test app opens the driver, and then uses DeviceIoControl calls to 
// instruct the driver to open and read a file. The contents of the text 
// file are echoed to stdout.
// -------------------------------------------------------------------------

void main(int ac, char* av[])
{

	//--------------------------------------------------------------------
	//	Declare a few variables we'll need. 
	//--------------------------------------------------------------------

	HANDLE h;

	//--------------------------------------------------------------------
	// Open the FileTest driver device.
	//--------------------------------------------------------------------

	h = CreateFile("\\\\.\\FileTest",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

	//--------------------------------------------------------------------
	//	Error exit if couldn't open the FileTest device.  
	//--------------------------------------------------------------------


	if (h == INVALID_HANDLE_VALUE)
	{
		ReportError("Failed to open the FileTest device: error code = " );  
		exit(1);
	}

	//--------------------------------------------------------------------
	//	Declare some constants we'll need. 
	//--------------------------------------------------------------------

	const int BufferSize=256;				//	size of our buffer
	const int PathSize=128;					//	size of our pathname
	const char Escape = '.';				//	our escape character

	//--------------------------------------------------------------------
	//	Declare a few variables we'll need in the loop. 
	//--------------------------------------------------------------------

	int  n = 0;								//	work integer
	char c = 'a';							//	loop control
	ULONG nReturned;						//	bytes returned by driver
	BOOL status;							//	status variables
	ULONG OpenStatus;
	ULONG CloseStatus;

	char buffer[BufferSize];				//	file contents
	short pathname[PathSize];				//	ascii pathname
	char p[PathSize];						//	auxiliary pathname

	//--------------------------------------------------------------------
	// Loop until we type a non-number. 
	//--------------------------------------------------------------------

	while (allowed(c))						//	loop while we have a good address
	{

		//-------------------------------------------------------------
		//	Read the filename and convert it to Unicode. 
		//-------------------------------------------------------------

		cout << "Enter the pathname for your input file: ";	

		gets(p);							//	read the file's pathname

		n=0;								//	convert to unicode
		while (allowed(p[n])) 
		{
			pathname[n] = (int) p[n];
			pathname[n] = pathname[n] & 0xff; 
			n++;
		}

		pathname[n] = 0;

		c = *pathname;						//	pick up the first character

		if (allowed(c))						//	if dot we exit
		{
	
			//--------------------------------------------------------
			// Tell the device to open the file
			//--------------------------------------------------------

			status = DeviceIoControl(
						h,
						FILETEST_OPEN,
						(LPVOID) pathname,
						sizeof(pathname),
						&OpenStatus,		
						sizeof(OpenStatus),
						&nReturned,
						NULL);

			//--------------------------------------------------------
			//	Skip if couldn't open. 
			//--------------------------------------------------------

			if (!status )
			{
				ReportError("DeviceIoControl call to open failed with error code ");
			}
			else if (OpenStatus != 0 )
			{
				ReportError("Failed to open file, error code: ", OpenStatus);
			}

			//--------------------------------------------------------
			// Read the first few bytes of the file
			//--------------------------------------------------------

			else
			{
				cout << "Opened file ok " << endl;

				memset(buffer, 0, sizeof(buffer));

				status = DeviceIoControl(
								h,
								FILETEST_READ,
								NULL,
								0,
								buffer,
								sizeof(buffer)-1,
								&nReturned,
								NULL);

				if (! status )	
				{
					ReportError(" Failed read call with error code: ");
				}

				//-----------------------------------------------------
				// Display the device's memory and close the file. 
				//-----------------------------------------------------

				else
				{
					Display(buffer, 0, BufferSize);
	
					status = DeviceIoControl(
								h,
								FILETEST_CLOSE,
								NULL,
								0,
								&CloseStatus,		
								sizeof(CloseStatus),
								&nReturned,
								NULL);
				}
			}
		}
	}

	//------------------------------------------------------------------
	//	We finished looping, so, close the handle and exit. 
	//------------------------------------------------------------------
	
	CloseHandle(h);
	exit(0);
}

// -------------------------------------------------------------------------
//	Function to display a buffer worth of data. 
// -------------------------------------------------------------------------

void Display(char *buffer, int address, int length)
{
	//------------------------------------------------------------------
	//	Variable 'i' counts the bytes displayed so far. 
	//------------------------------------------------------------------

	int i = 0;		
	char b[16];

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
		itoa(address, b, 16);
		cout << b << ": ";
		address += 16;

		//----------------------------------------------------------
		//	Display the hex bytes for this line. 
		//----------------------------------------------------------

		for (int k=0; k<16; k++) 
		{ 
			unsigned int d = buffer[i+k] & 0xff;
			cout.width(2);
			itoa (d, b, 16);
			cout << b << " "; 
		}

		//----------------------------------------------------------
		//	Give it a few blank columns. 
		//----------------------------------------------------------

		cout << "   ";

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

// -------------------------------------------------------------------------
//	Report an error. Inputs are an error number and an error message. 
// -------------------------------------------------------------------------

void ReportError(char *s, int n)
{
	char b[16];
	itoa(n, b, 16);
	cout << s << b << endl;
}

