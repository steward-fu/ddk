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

// INITFIO.CPP - example/test for C Run Time Library File I/O calls
// Copyright (c) 1998 Compuware Corporation

// This sample illustrates the usae of the initialization time file i/o
// routines. These i_xxxx functions are not available after the INIT_COMPLETE
// control message has been sent. In other circumstances, use the R0_xxx
// functions for file i/o.


#define DEVICE_MAIN
#include "initfio.h"
Declare_Virtual_Device(INITFIO)
#undef DEVICE_MAIN

#include INIT_CODE_SEGMENT

BOOL InitfioDevice::OnDeviceInit(VMHANDLE hVM, PCHAR sz)
{
	int h;
	int status;
	int n;
	char* text = 	"This is line 1 of the text file\n"
			"This is line 2 of the text file\n"
			"This is line 3 of the text file\n"
			"This is line 4 of the text file\n"
			"This is line 5 of the text file\n"
			"This is line 6 of the text file\n";

	const char* pathname="fiotest.txt";
	char buf[128];

	dout << "Entering InitfioDevice::OnDeviceInit" << endl;

// Open a text file for create

	h = i_open(pathname, _O_TEXT+_O_CREAT+_O_TRUNC, _S_IWRITE);
	if (h == -1)
	{
		dout << "failed to open for write" << endl;
		return TRUE;
	}
	
// Write the file

	dout << "write returns " << (DWORD)i_write(h, text, strlen(text)) 
		<< endl;

// Close the file

	if (i_close(h) == -1)
	{
		dout << "failed to close after create" << endl;
		return TRUE;
	}

// Open the same file for read, again as a text file

	h = i_open(pathname, _O_TEXT+_O_RDONLY, 0);
	if (h == -1)
	{
		dout << "open for read failed" << endl; 
		return TRUE;
	}

// Read the file. _read reads count or up to CRLF in text mode.

	while ( (n=i_read(h, buf, 10)) != 0)
	{
		if (n == -1) 
		{
			dout << "read failed" << endl; 
			return TRUE;
		}

// Echo the file data to the debug console

		buf[n] = 0;
		dout << buf;

		if (buf[n-1] == '\n')
			dout << "\r";  // dout does not translate \n to CRLF

	}

// Close the file

	if (i_close(h) == -1)
	{
		dout << "failed to close after read" << endl;
		return TRUE;
	}

// Delete the file

	i_remove(pathname);
	return TRUE;
}

