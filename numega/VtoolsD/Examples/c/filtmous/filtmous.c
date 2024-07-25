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

// FILTMOUS.C - demonstration of mouse mini-driver filter calls
// Copyright (c) 1996, Compuware Corporation

#define   DEVICE_MAIN
#include  "filtmous.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(FILTMOUS)

DeviceService_THUNK MPM_Thunk;

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);

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
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);

	END_CONTROL_DISPATCH

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// Mouse Filter 
//
//
// Input:
//	regs->
//		RECX	Delta Wheel (IntelliMouse only)
//		RESI	DeltaX (mickeys)
//		REDI	DeltaY
//		REAX	Button status (low byte)
//		REDX	Pointer to "Mouse instance" structure
//
// Output:
//	regs->
//		RESI	Possibly modified DeltaX
//		REDI	Possibly modified DeltaY
//		REAX	Possibly modified button status
//		RECX	Possibly modified wheel value
//
// Button status bits:
//
//	bit 2	== button 4
//	bit 3	== button 3
//	bit 4	== button 2
//	bit 5	== button 1
//
//	bit 7   Filter routine sets this bit if it maps the buttons.
//
VOID __stdcall MPM_Hook(PDSFRAME regs)
{
	DWORD temp;

// As a demonstration, we reverse the X and Y deltas. As a result, the mouse behaves
// differently when this VxD is loaded.
	temp = regs->RESI;	
	regs->RESI = regs->REDI;
	regs->REDI = temp;

	regs->REAX &= 0x7f;
}

BOOL OnSysDynamicDeviceInit()
{
	Hook_Device_Service(__VMD_Manipulate_Pointer_Message, MPM_Hook, &MPM_Thunk);
	return TRUE;
}

BOOL OnSysDynamicDeviceExit()
{
	Unhook_Device_Service(__VMD_Manipulate_Pointer_Message, MPM_Hook, &MPM_Thunk);
	return TRUE;
}
