// helloapp.cpp - test app for simple WDM example
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

#include <stdlib.h>
#include <windows.h>
#include <winioctl.h>
#include <iostream.h>

#include "..\HelloDI.h"	// Has class GUID definition 

GUID HelloClassGuid = HELLO_CLASS_GUID;

HANDLE OpenByInterface(
		GUID* pClassGuid, 
		DWORD instance, 
		PDWORD pError
		);

void __cdecl main(int ac, char* av[])
{
	DWORD Error;
	HANDLE hHello;

	hHello = OpenByInterface( &HelloClassGuid, 0, &Error);

	if (hHello == INVALID_HANDLE_VALUE)
	{
		cout << "Failed to open device, code = " << Error << endl;
		exit(1);
	}
	else
		cout << "Device opened ok\n";

	DWORD nRet;
	BOOL status = 
		DeviceIoControl(hHello, IOCTL_HELLO_TEST, NULL, 0, NULL, 0, &nRet, NULL);

	if ( ! status )
	{
		cout << "DeviceIoControl call failed, code=" << GetLastError() << endl;
		exit(1);
	}
	else
		cout << "DeviceIoControl ok\n";

	CloseHandle(hHello);		

	exit(0);
}
