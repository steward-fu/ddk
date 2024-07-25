// testport.cpp - test application for port i/o example
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
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <winioctl.h>

#include "common.h"

// -------------------------------------------------------------------------
//	Main Program.
// -------------------------------------------------------------------------

void __cdecl main(int ac, char* av[])
{
	CHAR op;
	ULONG addr;
	ULONG size;
	ULONG data;
	HANDLE hPortIo;
	CHAR buffer[128];
	BOOL status;
	ULONG nBytesReturned;
	ULONG PortHandle;

	PORTOPENPARAMS OpenPort;
	PORTREADPARAMS ReadPort;
	PORTWRITEPARAMS WritePort;
	PORTCLOSEPARAMS ClosePort;

	// -----------------------------------------------------------------
	//	Display a banner. 
	// -----------------------------------------------------------------

	printf("DriverWorks PortIo Demo Test Program\n\n");

	// -----------------------------------------------------------------
	//	Connect to the PortIo Driver.
	// -----------------------------------------------------------------


	hPortIo = CreateFile("\\\\.\\PortIo",
	             GENERIC_READ | GENERIC_WRITE,
                 FILE_SHARE_READ,
                 NULL,
                 OPEN_EXISTING,
                 0,
                 NULL);

	if (hPortIo == INVALID_HANDLE_VALUE)
	{
		printf("failed to open PortIo device: error=%x\n", 
				GetLastError());
		exit(1);
	}

	// -----------------------------------------------------------------
	//	Loop entering commands, exit when Control-C is pressed. 
	// -----------------------------------------------------------------

	printf("Enter command. Syntax: {R|W} hex-address size [hex-data]\n");

	while (TRUE)
	{
		// ---------------------------------------------------------
		//	Read the user input.
		// ---------------------------------------------------------

		printf("* ");
		gets(buffer);
		if (sscanf(buffer, "%c %x %d %x", &op, &addr, &size, &data) < 2)
			break;

		// ---------------------------------------------------------
		//	Display the answer header. 
		// ---------------------------------------------------------

		printf("(");
		if (op == 'R' || op == 'r')
		{
			op = 'R';
			printf("read ");
		}
		else if (op == 'W' || op == 'w')
		{
			op = 'W';
			printf("write ");
		}
		else if (op == 'X' || op == 'x')
			break;		
		else
		{
			printf("bad op)\n"); 
			continue;
		}

		printf(" port=%08x size=%08x", addr, size);
		if (op == 'W')
			printf(" data=%08x)\n", data);
		else
			printf(")\n");

		// ---------------------------------------------------------
		//	Make sure we have a valid i/o data size. 
		// ---------------------------------------------------------

		if (size!=1 && size!=2 && size!=4)
		{
			printf(" size must be 1, 2, or 4\n");
			continue;
		}

		// ---------------------------------------------------------
		//	Ask the driver to read or write the io port
		// ---------------------------------------------------------

		OpenPort.open_Address = addr;
		OpenPort.open_PortSize = size;
		OpenPort.open_PortCount = 1;

		status = DeviceIoControl(
			hPortIo, 
			IOCTL_PORTIO_OPENPORT, 
			&OpenPort, 
			sizeof(OpenPort),
			&PortHandle,
			sizeof(PortHandle),
			&nBytesReturned,
			NULL
			);

		if ( !status || (PortHandle == NULL) )
		{
			printf("Port open failed\n");
			continue;
		}

		// ---------------------------------------------------------
		//	Display the result from the driver. 
		// ---------------------------------------------------------

		if (op == 'R')
		{
			ReadPort.rd_Handle = PortHandle;
			ReadPort.rd_Index = 0;	

			status = DeviceIoControl(
				hPortIo, 
				IOCTL_PORTIO_READPORT, 
				&ReadPort, 
				sizeof(ReadPort),
				&data,
				sizeof(data),
				&nBytesReturned,
				NULL
				);

			if ( !status || (nBytesReturned==0) )
				printf("read failed: %x\n", GetLastError());
			else
				printf("result = %x\n", data);
		}
		else
		{
			WritePort.wr_Handle = PortHandle;
			WritePort.wr_Index = 0;
			WritePort.wr_Data = data;

			status = DeviceIoControl(
				hPortIo, 
				IOCTL_PORTIO_WRITEPORT, 
				&WritePort, 
				sizeof(WritePort),
				NULL,
				0,
				&nBytesReturned,
				NULL
				);

			if ( !status )
				printf("write failed: %x\n", GetLastError());
			else
				printf("write OK\n", data);
		}

		// ---------------------------------------------------------
		//	Close the handle to the PortIo driver. 
		// ---------------------------------------------------------

		ClosePort.close_Handle = PortHandle;				
		status = DeviceIoControl(
			hPortIo, 
			IOCTL_PORTIO_CLOSEPORT, 
			&ClosePort, 
			sizeof(ClosePort),
			NULL,
			0,
			&nBytesReturned,
			NULL
			);

		if ( !status )
			printf("close failed: %x\n", GetLastError());

	} 

	CloseHandle(hPortIo);
}

