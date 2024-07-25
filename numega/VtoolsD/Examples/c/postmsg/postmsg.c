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

//
// POSTMSG.C - demonstration of VxD communication w/ 16-bit Windows app
// Copyright (c) 1995, Compuware Corporation
// 
// This simple VxD hooks the interrupt of the Real-Time Clock, programs
// the RTC to generate a periodic interrupt, and then sends a message
// to a Windows application every 1024 ticks (about once per second).
//
// ***********************************************************************
//
// Device preliminaries for main module

#define   DEVICE_MAIN
#include  "POSTMSG.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(POSTMSG)		// Declare Device Data Block

// ***********************************************************************
//
// Static data

VPICD_HWInt_THUNK POSTMSG_Thunk;	// Thunk for interrupt handler
PriorityVMEvent_THUNK EventThunk;	// Thunk for "1024 ticks" event handler

IRQHANDLE RTCIRQHandle;			// Handle for virtual IRQ
BYTE SavedStatusRegisterA;		// Saved RTC configuration register
BYTE SavedStatusRegisterB;		// Saved RTC configuration register
DWORD TickCounter;			// Interrupt counter
BOOL bClientRegistered;			// True when PM API called to register
VMHANDLE SystemVMHandle;
WORD PostMsgOffset;
WORD PostMsgSelector;
WORD PostMsghWnd;

// ***********************************************************************
//
// Handlers for Control Messages
//
// This VxD handles DEVICE_INIT (to hook the interrupt and configure the
// RTC), and SYSTEM_EXIT (to restore the RTC to its original state).

DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(SYSTEM_EXIT, OnSystemExit);

// Function
// 	OnDeviceInit - Handler for DEVICE_INIT control message
//
// Input
//	hSysVM		Handle of system virtual machine
//	CommandTail	Pointer to Windows command line
//
// Remarks
//	This routine is responsible for hooking the RTC
//	interrupt and configuring it.
//
// Returns
//	Returns TRUE if successfully initialized
//
BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	BYTE statreg;
	DWORD status;
	DWORD RTClockFreqIndex;

// Hooking a hardware interrupt requires calling the Virtual Programmable
// Interrupt Controlloer Device (VPICD), to inform that this VxD will
// be responsible for the IRQ. VPICD provides five different notifications
// of events related to the IRQ, but this VxD uses only one of them, namely,
// that of the actual hardware interrupt event. The VPICD_Virtualize_IRQ
// call takes one parameter which points to a structure containing the
// address of this VxD's hardware interrupt handler, along with additional
// information related to the IRQ.

	struct VPICD_IRQ_Descriptor IRQdesc;	// struct to pass to
				    		// VPICD_Virtualize_IRQ

	SystemVMHandle = hVM;			// save for later usage

// Set up the structure to pass to VPICD_Virtualize_IRQ

	IRQdesc.VID_IRQ_Number = RTC_IRQ;	// IRQ to virtualize
	IRQdesc.VID_Options = 0;		// reserved

// To set the address of the our handler in the structure that will be
// passed to VPICD_Virtualize_IRQ, we pass the address of the handler's
// thunk to VPICD_Thunk_HWInt, which initializes the thunk and returns
// its address.

	IRQdesc.VID_Hw_Int_Proc =		// set address of handler
		(DWORD)VPICD_Thunk_HWInt(POSTMSG_Int_Handler, &POSTMSG_Thunk);

// The other callbacks are not used.

	IRQdesc.VID_EOI_Proc = 0;
	IRQdesc.VID_Virt_Int_Proc = 0;
	IRQdesc.VID_Mask_Change_Proc = 0;
	IRQdesc.VID_IRET_Proc = 0;

	IRQdesc.VID_IRET_Time_Out = 500;

// Now pass the structure to VPICD. VPICD returns the IRQ handle.

	RTCIRQHandle = VPICD_Virtualize_IRQ(&IRQdesc);
	if (RTCIRQHandle == 0)
		return FALSE;			// failed to virtualize

// Save the initial RTC status registers for restoration at exit.

	SavedStatusRegisterA = ReadCMOS(STATREG_A);
	SavedStatusRegisterB = ReadCMOS(STATREG_B);

// Set frequency to 1024
	RTClockFreqIndex = 6;
	statreg = (SavedStatusRegisterA & ~0xF) | (RTClockFreqIndex & 0xF);
	WriteCMOS(STATREG_A, statreg);

// Set RTC status register flags to enable it to assert its IRQ

	statreg = ReadCMOS(STATREG_B);
	statreg |= ENABLE_INTERRUPT;
	WriteCMOS(STATREG_B, statreg);
	ReadCMOS(STATREG_C);

// Initialize the Tick Counter
	TickCounter = 0;

// Make sure the IRQ is unmasked on the PIC. Otherwise, the interrupt
// will never occur.

	VPICD_Physically_Unmask(RTCIRQHandle);

	return TRUE;				// initialized successfully
}

// Function
// 	OnSystemExit - Handler for SYSTEM_EXIT control message
//
// Input
//	hSysVM		Handle of system virtual machine
//
// Remarks
//	This routine is responsible for restoring the original state
//	of the RTC
//
VOID OnSystemExit(VMHANDLE hVM)
{
	WriteCMOS(STATREG_A, SavedStatusRegisterA);
	WriteCMOS(STATREG_B, SavedStatusRegisterB);

	VPICD_Physically_Mask(RTCIRQHandle);
}

// Function
// 	ControlDispatcher - dispatch control messages
//
// Input
//	dwControlMessage	specifies control message sent
//	registers
//
// Remarks
//	This routine is responsible for dispatching to control message
//	handlers. 
//
BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_DEVICE_INIT(OnDeviceInit);
		ON_SYSTEM_EXIT(OnSystemExit);

	END_CONTROL_DISPATCH

	return TRUE;
}

// Function
// 	POSTMSG_Int_Handler - handler for VID_Hw_Int callback
//
// Input
//	hVM		handle of current virtual machine
//	hIRQ		handle of this virtualized IRQ
//
// Remarks
//	VPICD invokes this routine (via the thunk) when the RTC
//	interrupt (IRQ 8) occurrs.
//
BOOL __stdcall POSTMSG_Int_Handler(VMHANDLE hVM, IRQHANDLE hIRQ)
{
	BYTE thisSecond;
	
	TickCounter++;			// bump the tick counter

	// check if time to send the message to app
	if (bClientRegistered && (TickCounter & (1024-1)) == 0)
	{
		Call_Priority_VM_Event(LOW_PRI_DEVICE_BOOST, SystemVMHandle, 
		PEF_WAIT_FOR_STI+PEF_WAIT_NOT_CRIT, 
		(PVOID)TickCounter, EventService, 0, &EventThunk);
	}

	ReadCMOS(STATREG_C);		// clear RTC status flags

	VPICD_Phys_EOI(hIRQ);		// tell VPICD to clear the interrupt

	return TRUE;	    		// thunk will clear carry
}

// Function
// 	EventService - handler for "1024-ticks" event
//
// Input
//	hVM		handle of current VM
//	Refdata		tick counter value passed from ISR
//	pRegs		pointer to client register structure
//
// Remarks
//	This routine is the handler for the event scheduled by the interrupt
//	handler. It is called once per second.
//
VOID __stdcall EventService(VMHANDLE hVM, PVOID Refdata, PCLIENT_STRUCT pRegs, DWORD flags)
{
	CLIENT_STRUCT saveRegs;

	Save_Client_State(&saveRegs);

	Begin_Nest_Exec();

	Simulate_Push(PostMsghWnd);			// hwnd
	Simulate_Push(WM_COMMAND);			// message
	Simulate_Push(PMVXD_EVENT);	     		// wParam
	Simulate_Push((DWORD)Refdata >> 16);		// lParam
	Simulate_Push((DWORD)Refdata & 0xffff);
	Simulate_Far_Call(PostMsgSelector, PostMsgOffset);
	Resume_Exec();

	End_Nest_Exec();

	Restore_Client_State(&saveRegs);
}


// Function
// 	PM_Api_Handler - handle direct calls from app
//
// Input
//	hVM		handle of calling VM
//	pRegs		pointer to client register structure
//
// Remarks
//	This routine handles calls into the VxD from the app. The app
//	obtains the far pointer to invoke this routine by issuing INT 2Fh
//	with AX=1684h and BX=device id of this VxD.
//
VOID PM_Api_Handler(VMHANDLE hVM, PCLIENT_STRUCT pRegs)
{
	switch (_clientAX)
	{
	case PMVXD_REGISTER:
		PostMsghWnd = _clientBX;
		PostMsgSelector = _clientCX;
		PostMsgOffset = _clientDX;

		bClientRegistered = TRUE;
		_clientAX = 0;
		break;

	case PMVXD_DEREGISTER:
		bClientRegistered = FALSE;
		_clientAX = 0;
		break;

	default:
		_clientAX = 0xffff;
	}
}