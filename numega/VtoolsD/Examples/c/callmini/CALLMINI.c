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

// CALLMINI.c - main module for VxD CALLMINI
// Copyright (c) 1997 Compuware Corporation

// This example illustrates how a VxD can call an entry point in an
// miniport driver or WDM (.sys) driver.

#define   DEVICE_MAIN
#include  "callmini.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(CALLMINI)

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);

BOOL __cdecl ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);

	END_CONTROL_DISPATCH

	return TRUE;
}

// This typedef defines the prototype of the function in the minidriver

typedef DWORD (__stdcall *IEXP_FUNCPTR)(DWORD,DWORD);


// Handler for SysDynamicDeviceInit
//
// The handler calls entry point "ExportedFunction" in module IEXPORT.SYS
//
BOOL OnSysDynamicDeviceInit()
{
	HPEMODULE h;

// First get a handle to the module
		
	h = PELDR_GetModuleHandle("IEXPORT.SYS");
	
	if (h == NULL)
		dprintf("Module IEXPORT.SYS is not loaded\n");

	else
	{

// The module was found. Now get the address of the entry point

		IEXP_FUNCPTR pFunc = (IEXP_FUNCPTR) PELDR_GetProcAddress(
			h, 
			"ExportedFunction",
			NULL
			);

		if ( pFunc  ==  NULL)
			dprintf("Could not locate procedure \"ExportedFunction\"\n");

// Got the procedure address. Now call it.

		else
		{
			DWORD result = pFunc(6, 8);
			dprintf("pFunc(6,8)=%d\n", result);
		}

	}

	return TRUE;
}
