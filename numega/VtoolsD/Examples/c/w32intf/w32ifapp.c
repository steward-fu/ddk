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

// W32IFAPP.C - example Win32 app for interfacing with VxD
// Copyright (c) 1995, Compuware Corporation
//
// -------------------------------------------------------------------------
//
// This application demonstrates how a Win32 application can communicate
// with a VxD. This is a Win32 console app, and, as such, is generally
// started from a DOS prompt, but has full access to the relevant Win32 APIs.
// All Win32 APIs used in this app may be used in the same manner from
// a non-console Win32 app.
// 
// This application prints a message whenever a Virtual Machine is created
// or destroyed. Here is the basic theory of operation:
//
//  -The application creates a Win32 synchronization Event.
//  -The application calls OpenVxDHandle to get a ring 0 event handle
//     that corresponds to the ring 3 event handle.
//  -The application calls CreateFile to dynamically load the VxD.
//  -The application calls the VxD (via DeviceIOControl), passing to it the 
//     ring 0 event handle.
//  -The VxD stores the event handle for later use, and returns the
//     addresses of the locations where it stores (1) the VM handle of the
//     last created or destroyed Virtual Machine, and (2) a boolean 
//     indicating if the last operation was creation or destruction.
//  -The application creates a separate thread for servicing the VxD.
//  -The application waits for a key stroke to signal that it should exit.
//  -The service thread waits for the event to be signalled by the VxD.
//  -When a VM is created or destroyed, the VxD stores the data and signals
//     the event.
//  -The service thread wakes up, retrieves the information from the 
//     locations obtained by the initial call to the VxD, and prints out a
//     a message.
//  -When the user presses a key, the program exits.
//
// -------------------------------------------------------------------------
//
// Note on shared memory:
//
// Memory allocated in a VxD is globally accessible, and can be shared with
// applications. Memory that an application allocates may be in a process
// specific region of the memory map. In this case, the VxD cannot access
// that memory unless the current memory context is that of the process
// in question. 
//
// -------------------------------------------------------------------------
// 
// Important:
//
// Build the VxD and copy it to \windows\system before running this app.
//
// -------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>
#include "w32if.h"

#ifndef __BORLANDC__
#define kbhit _kbhit
#endif

HANDLE		hDevice;
DWORD*		phVMCreatedOrDestroyed;
BOOL*		pbVMCreate;

// -------------------------------------------------------------------------
// GetAddressOfOpenVxDHandle
//
// This function returns the address of OpenVxDHandle. OpenVxDHandle is a 
// KERNEL32 function that returns a ring 0 event handle that corresponds to a 
// given ring 3 event handle. The ring 0 handle can be used by VxDs to 
// synchronize with the Win32 app.
//
HANDLE (WINAPI *GetAddressOfOpenVxDHandle(void))(HANDLE)
{
	CHAR K32Path[MAX_PATH];
	HINSTANCE hK32;

	GetSystemDirectory(K32Path, MAX_PATH);
	strcat(K32Path, "\\kernel32.dll");
	if ((hK32 = LoadLibrary(K32Path)) == 0)
		return NULL;

	return (HANDLE(WINAPI *)(HANDLE))GetProcAddress(hK32, "OpenVxDHandle");
}

// -------------------------------------------------------------------------
// CreateCommonEvent
//
// This function creates a synchronization event, and obtains the 
// corresponding ring 0 event handle. 
//
// Parameters:
//	pr3Evt		location to receive ring 3 event handle
//	pr0Evt		location to receive ring 0 event handle
//	bManualReset	input TRUE if event should be reset only manually 
//	bInitialState	input TRUE if event should be initially signalled,
//
// Returns:
//	Returns TRUE if successful.
//
BOOL CreateCommonEvent(HANDLE* pr3Evt, HANDLE* pr0Evt, BOOL bManualReset,
	BOOL bInitialState)
{
	static HANDLE (WINAPI *pOpenVxDHandle)(HANDLE)=0;

	*pr3Evt = CreateEvent(0, bManualReset, bInitialState, NULL);

	if (pOpenVxDHandle == 0)
		pOpenVxDHandle = GetAddressOfOpenVxDHandle();

	if (pOpenVxDHandle && *pr3Evt)
		*pr0Evt = pOpenVxDHandle(*pr3Evt);
	else
		*pr0Evt = 0;

	return ( (*pr3Evt != 0) && (*pr0Evt != 0) );
}

// -------------------------------------------------------------------------
// 
// ServiceThread
//
// This function is the thread that services the VxD. It waits on the
// event, and is signalled whenever a Virtual Machine is created or
// destroyed. When signalled, it accesses the addresses provided by
// the VxD during initialization to obtain the VM handle and creation
// flag for the last operation. The VM handle is printed, and the wait 
// is reentered.
//
DWORD WINAPI ServiceThread(PVOID hEventR3)
{
	while (TRUE)
	{

// Wait for notification from VxD

		WaitForSingleObject((HANDLE)hEventR3, INFINITE);

// Print out the information, accessing the data in the VxD directly.

		printf("\nVM %08lx %s", *phVMCreatedOrDestroyed,
				*pbVMCreate ? "Created" : "Destroyed");

// Loop back to wait again.
	}

#ifdef __BORLANDC__
#pragma warn -rch	// unreachable code
#endif
	return 0;	// never returns
}

// -------------------------------------------------------------------------
//
// Main
//
void main(int ac, char* av[])
{
#ifdef __BORLANDC__
#pragma warn -par
#endif

	HANDLE 	hEventR0, hEventR3;	// event handles
	PVOID	inBuf[1];		// buffer for passing handle to VxD
	DWORD	RetInfo[2];		// buffer to receive data from VxD
	DWORD	cbBytesReturned;	// count of bytes returned from VxD
	DWORD	Tid;			// id of created thread

// CreateFile recognizes the string "\\.\" as designating a dynamically
// loadable VxD.

	const PCHAR VxDName = "\\\\.\\W32INTF.VXD";

// The first task is create an event that the service thread can wait on.
// We simultaneously create a ring 0 handle for the event that can be
// signalled by the VxD.	

	if (!CreateCommonEvent(&hEventR3, &hEventR0, FALSE, FALSE))
	{
		fprintf(stderr, "Cannot create event\n");
	 	exit(1);
	}

// Load the VxD by calling CreateFile. The VxD file must reside in the
// the Windows system directory.

	hDevice = CreateFile(VxDName, 0,0,0,
                        CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
        	fprintf(stderr, "Cannot load VxD, error=%08lx\n",
				GetLastError());
	 	exit(1);
	}

// Now pass the ring 0 event handle to the VxD so that it can store it for
// later use (when a VM is created or destroyed). 

	inBuf[0] = hEventR0;
	if ( ! DeviceIoControl(hDevice, W32IF_PASS_EVENT,
               inBuf, sizeof(PVOID), (LPVOID)RetInfo, sizeof(RetInfo),
               &cbBytesReturned, NULL)
	   )
	    	fprintf(stderr, "Failed to register event\n");
	else
	{

// When the call is successful, the VxD returns two addresses in the
// RetInfo array. The first is the address where the VxD stores the
// VM handle of the last created or destroyed Virtual Machine. The second
// is the address of boolean indicating if the last operation was the
// creation of a VM.  The service thread will later access these locations
// in the VxD directly. 

		phVMCreatedOrDestroyed = (DWORD*)RetInfo[0];
		pbVMCreate = (BOOL*)RetInfo[1];
	}

// Create a thread that will wait for notification by the VxD.

	CreateThread(0, 0x1000, ServiceThread, hEventR3, 0, &Tid);

// Wait for the user to press a key to signal that we should exit.

	printf("Press any key to exit . . .");
	while (!kbhit())
		;
}
