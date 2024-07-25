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

// HOOKVIRQ.C - main module for VxD HOOKVIRQ
// Copyright (c) 1996, Compuware Corporation

// This VxD shows you how to intercept virtualization of an IRQ by another
// VxD. By hooking device service VPICD_Virtualize_IRQ, a VxD such as this
// one can gain control on an interrupt both before and after the VxD which
// normally supplies the ISR.

#define   DEVICE_MAIN
#include  "hookvirq.h"
#undef    DEVICE_MAIN

#define THE_IRQ 1

// Declare the DDB, etc.
Declare_Virtual_Device(HOOKVIRQ)

// Thunks
DeviceService_THUNK		VIRQthunk;
VPICD_HWInt_Ex_THUNK	HwIntThunk;

PVOID ChainHwIntAddress;			// actual ISR address
DWORD (*TheRealVirtualizeIRQ)(void);// actual VPICD_Virtualize_IRQ address

// declare control message handler
DefineControlHandler(SYS_CRITICAL_INIT, OnSysCriticalInit);

// ----------------------------------------------------------------------
// ControlDispatcher
//
// Get control as early as possible by supplying handler for 
// SYS_CRITICAL_INIT.

BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_SYS_CRITICAL_INIT(OnSysCriticalInit);

	END_CONTROL_DISPATCH

	return TRUE;
}

// ----------------------------------------------------------------------
// MyIRQHandler
//
// This is the ISR. It gets control when the interrupt occurs and
// VPICD dispatches it.
//

BOOL __stdcall MyIRQHandler(VMHANDLE hVM, IRQHANDLE hIRQ, DWORD Refdata)
{
	// do whatever

	_asm mov edx, Refdata
	_asm mov eax, hIRQ
	_asm mov ebx, hVM
	_asm call DWORD PTR [ChainHwIntAddress]
	_asm jc retfalse

	return TRUE;
retfalse:
	return FALSE;
}		

// ----------------------------------------------------------------------
// MyVirtualizeIRQ
//
// Hook for VPICD_Virtualize_IRQ. This routine gets control when other
// VxDs call VPICD_Virtualize_IRQ. It substitutes the local ISR for 
// the one supplied by the caller.
//
VOID __stdcall MyVirtualizeIRQ(PDSFRAME pDS)
{
	VID* pVID;
	DWORD ResultFlags;

	pVID = (VID*)(pDS->REDI);

	dprintf("IRQ %04x is being virtualized, options=%04x\n",
		pVID->VID_IRQ_Number, pVID->VID_Options);

	if (pVID->VID_IRQ_Number == THE_IRQ)
	{
		ChainHwIntAddress = (PVOID)pVID->VID_Hw_Int_Proc;
		pVID->VID_Hw_Int_Proc = 
			(DWORD)VPICD_Thunk_HWInt_Ex(MyIRQHandler, &HwIntThunk);
	}

	_asm mov edi, pVID
	pDS->REAX = TheRealVirtualizeIRQ();	
	_asm pushfd
	_asm pop ResultFlags

	pDS->RFLAGS = ResultFlags;
}
// ----------------------------------------------------------------------
// OnSysCriticalInit
//
// Control message handler, hooks VPICD_Virtualize_IRQ.
//
BOOL OnSysCriticalInit(VMHANDLE hVM, PCHAR CommandTail, DWORD refData)
{
	TheRealVirtualizeIRQ = Hook_Device_Service(__VPICD_Virtualize_IRQ, 
						MyVirtualizeIRQ, &VIRQthunk);

	return TRUE;
}

