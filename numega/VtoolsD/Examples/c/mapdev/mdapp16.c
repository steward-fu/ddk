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

//***********************************************************************
// mapdev16.c - Application for MAPDEV VxD example
//
// Copyright (c) 1994, Compuware Corporation All rights resreved
//***********************************************************************

#define CTL_CODE( DeviceType, Function, Method, Access ) (   \
    ((DWORD)(DeviceType) << 16) | \
    ((DWORD)(Access) << 14)     | \
    ((DWORD)(Function) << 2)    | \
    (DWORD)(Method)               \
)

#define METHOD_NEITHER                  3
#define FILE_ANY_ACCESS                 0
#define FILE_DEVICE_UNKNOWN             0x00000022


#include <windows.h>
#include <string.h> 
#define NotVxD
#include "mapdev.h"

HANDLE 	hInst;
FARPROC	VxDAddr;

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long CALLBACK __export MainWndProc(HWND, UINT, WPARAM, LPARAM);


//***********************************************************************
// GetVxDEntryPoint
//
// This function locates the entry point of a VxD using the PM API entry point
// convention.
//
// Input:
//	none
//
// Returns
//	Returns the address of the VxD entry, or NULL if none is found
//
FARPROC GetVxDEntryPoint(void)
{
	FARPROC p;
	const char far* VxDName = "MAPDEV  ";
  	_asm {
		push	bx
		push	di

  		mov	ax, 1684h
		xor	bx, bx
		les	di, VxDName
  		int	2Fh		
//
// if successful, es:di contains a far pointer to call the VxD
//
		mov	word ptr [p], di
		mov word ptr [p+2], es

		pop	di
		pop	bx			
	}

	return p;
}

BOOL MapDevice(DWORD PhysAddress, WORD size, PWORD pSelector)
{
	MAPDEVREQUEST req;

	req.mdr_ServiceID = MDR_SERVICE_MAP;	
	*(DWORD*)&req.mdr_PhysicalAddress = PhysAddress;
	req.mdr_SizeInBytes = size;	

	_asm lea	di, [req]
	_asm mov	ax, ss
	_asm mov	es, ax

	VxDAddr();

	*pSelector = req.mdr_Selector;

	return (req.mdr_Status == MDR_STATUS_SUCCESS);
}


BOOL UnmapDevice(WORD Selector, WORD Size)
{
	MAPDEVREQUEST req;

	req.mdr_ServiceID = MDR_SERVICE_UNMAP;	
	req.mdr_Selector = Selector;
	req.mdr_SizeInBytes = Size;

	_asm lea	di, [req]
	_asm mov	ax, ss
	_asm mov	es, ax

	VxDAddr();

	return (req.mdr_Status == MDR_STATUS_SUCCESS);
}



//*********************************************************************
// WinMain
//
// Windows calls this function to initialize the program.
//
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, 
				int nCmdShow)
{
	WORD Selector;
	char buffer[80];

	VxDAddr = GetVxDEntryPoint();
	if (VxDAddr == 0)
	{
		MessageBox(NULL, "No VxD found!", "ERROR", MB_OK);
	  	return FALSE;
	}  

	if (MapDevice(0xb8000L, 0x1000, &Selector) )
	{
		wsprintf(buffer, "Mapped device to selector %04x\n", Selector);
		MessageBox(NULL, buffer, "Success", MB_OK);
		UnmapDevice(Selector, 0x1000);
	} 
	else
		MessageBox(NULL, "Failed to create mapping", "ERROR", MB_OK);


	return FALSE;	// not really going to start an app 
}


