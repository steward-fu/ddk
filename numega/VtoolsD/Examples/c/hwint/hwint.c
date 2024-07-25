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

// HWINT.C - demonstration of periodic interrupt handling / performance test
// Copyright (c) 1995, Compuware Corporation
// 
// This simple VxD hooks the interrupt of the Real-Time Clock, programs
// the RTC to generate a periodic interrupt, and then reports the number
// of interrupts generated per second on the debug console.
//
// To set the frequency of the interrupt, add the following lines to 
// system.ini:
//
//		[HWINT]
//		FreqIndex=n
//
// Where 'n' is set to determine the interrupt frequency as follows:
//
//		 n	 Hz
//		 -	----
//		 1	 256
//		 2	 128
//		 3	8192
//		 4	4096
//		 5	2048
//		 6	1024
//		 7	 512
//		 8	 256
//		 9	 128
//		10	  64
//		11	  32
//		12	  16
//		13	   8
//		14	   4
//		15	   2
//
// Use Soft-ICE/W or WDEB386 to view the output of this VxD.
//
// ***********************************************************************
//
// Device preliminaries for main module

#define   DEVICE_MAIN
#include  "HWINT.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(HWINT)		// Declare Device Data Block

// ***********************************************************************
//
// Static data

VPICD_HWInt_THUNK RTCInt_Thunk;		// Thunk for interrupt handler
Event_THUNK SecondEventThunk;		// Thunk for hour event handler
EVENTHANDLE hEvent;			// Handle of timer event

IRQHANDLE RTCIRQHandle;			// Handle for virtual IRQ
BYTE SavedStatusRegisterA;		// Saved RTC configuration register
BYTE SavedStatusRegisterB;		// Saved RTC configuration register
DWORD TickCounter;			// Interrupt counter
BYTE prevSecond;			// RTC value for second

int nSeconds = 0;
int totTicks = 0;

// ***********************************************************************
//
// Handlers for Control Messages
//
// This VxD handles DEVICE_INIT (to hook the interrupt and configure the
// RTC), and SYSTEM_EXIT (to restore the RTC to its original state).

DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(SYSTEM_EXIT, OnSystemExit);
DefineControlHandler(DEBUG_QUERY, OnDebugQuery);
#ifdef WIN40
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
#endif

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

// Set up the structure to pass to VPICD_Virtualize_IRQ

	IRQdesc.VID_IRQ_Number = RTC_IRQ;	// IRQ to virtualize
	IRQdesc.VID_Options = 0;		// reserved

// To set the address of the our handler in the structure that will be
// passed to VPICD_Virtualize_IRQ, we pass the address of the handler's
// thunk to VPICD_Thunk_HWInt, which initializes the thunk and returns
// its address.

	IRQdesc.VID_Hw_Int_Proc =		// set address of handler
		(DWORD)VPICD_Thunk_HWInt(RTCInt_Handler, &RTCInt_Thunk);

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


// Get Frequency Index from system.ini, and set in RTC


	if (hVM == 0) // hVM is zero on a dynamic call - can't call Get_Profile_xxx
		RTClockFreqIndex = 6;
	else	
		RTClockFreqIndex = Get_Profile_Decimal_Int(6, "HWint", "FreqIndex",
					&status);

	statreg = (SavedStatusRegisterA & ~0xF) | (RTClockFreqIndex & 0xF);
	WriteCMOS(STATREG_A, statreg);

// Set RTC status register flags to enable it to assert its IRQ

	statreg = ReadCMOS(STATREG_B);
	statreg |= ENABLE_INTERRUPT;
	WriteCMOS(STATREG_B, statreg);
	ReadCMOS(STATREG_C);

// Initialize the Tick Counter
	TickCounter = 0;

	prevSecond = ReadCMOS(SECONDS);

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
	Cancel_Global_Event(hEvent);

	WriteCMOS(STATREG_A, SavedStatusRegisterA);
	WriteCMOS(STATREG_B, SavedStatusRegisterB);

	VPICD_Physically_Mask(RTCIRQHandle);
	VPICD_Force_Default_Behavior(RTCIRQHandle);
}

#ifdef WIN40

BOOL OnSysDynamicDeviceInit()
{
	return OnDeviceInit(0, 0);
}

BOOL OnSysDynamicDeviceExit()
{
	OnSystemExit(0);
	return TRUE;
}

#endif

// Function
// 	ControlDispatcher - dispatch control messages
//
// Input
//	dwControlMessage	specifies control message sent
//	registers
//
// Remarks
//	This routine is responsible for dispatching to control message
//	handlers. It was generated by QuickVxD.
//
BOOL __cdecl ControlDispatcher(
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
		ON_DEBUG_QUERY(OnDebugQuery);
#ifdef WIN40
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
#endif
	END_CONTROL_DISPATCH

	return TRUE;
}

// Function
// 	RTCInt_Handler - handler for VID_Hw_Int callback
//
// Input
//	hVM		handle of current virtual machine
//	hIRQ		handle of this virtualized IRQ
//
// Remarks
//	VPICD invokes this routine (via the thunk) when the RTC
//	interrupt (IRQ 8) occurrs.
//
BOOL __stdcall RTCInt_Handler(VMHANDLE hVM, IRQHANDLE hIRQ)
{
	static BOOL bIsFirst = TRUE;
	BYTE thisSecond;
	
	TickCounter++;			// bump the tick counter

	if ( (ReadCMOS(STATREG_A)&0x80) == 0) // if update not in progress
	{

// Read the current value for seconds. Although it is read in BCD format,
// we are only using to compare with prevSecond, so it doesn't matter. If
// the second value has advanced, schedule the event.

		thisSecond = ReadCMOS(SECONDS);	ReadCMOS(STATREG_C);

		if (thisSecond != prevSecond)	// if new second
		{
			prevSecond = thisSecond;
			if (!bIsFirst)
				hEvent = Call_Global_Event(SecondEventService,
				      (PVOID)TickCounter, &SecondEventThunk);

			bIsFirst = FALSE;
			TickCounter = 0;
		}
	}


	ReadCMOS(STATREG_C);		// clear RTC status flags

	VPICD_Phys_EOI(hIRQ);		// tell VPICD to clear the interrupt

	return TRUE;	    		// thunk will clear carry
}

// Function
// 	SecondEventService - handler for hour event
//
// Input
//	hVM		handle of current VM
//	Refdata		not used
//	pRegs		pointer to client register structure
//
// Remarks
//	This routine is the handler for the event scheduled by the interrupt
//	handler. It is called once per second.
//
VOID __stdcall SecondEventService(VMHANDLE hVM, PVOID Refdata, PCLIENT_STRUCT pRegs)
{
	hEvent = 0;

	totTicks += (DWORD)Refdata;
	nSeconds++;

	dprintf("Ticks/second=%ld Avg=%ld\n", Refdata, totTicks/nSeconds);
}

// Function
// 	dgets - get a string from the debug console
//
// Input
//	buf		buffer to receive string
//	maxchar		number of characters not including terminating nul that
//			buf can accommodate
//
// Returns
//	Returns the number of characters read
//
// Remarks
//	Input is terminated by a CR	
//
int dgets(char* buf, int maxchar)
{
	WORD ch;
	int  i;

	for (i=0; i < maxchar; i++, buf++)
	{
		ch  = In_Debug_Chr() & 0xff;
		if ( ((ch & 0xff) == 13) || ((ch & 0xff00) == 0xff00) )
			break;
		else	
		{
			*buf = ch;
			Out_Debug_Chr((BYTE)(ch & 0xff));
		}
	}

	*buf = 0;
	return i;
}


// Function
// 	OnDebugQuery 
//
// Remarks
//	Allows interactive control of frequency.
//
//	Invoke this by typing .hwint at the debugger propmt
//
VOID OnDebugQuery()
{
	CHAR buf[80];
	INT index;
	BYTE statreg;

	dprintf("Enter divisor index (1..15): ");
	dgets(buf, sizeof(buf)-1);

	sscanf(buf, "%d", &index);
	statreg = (SavedStatusRegisterA & ~0xF) | (index & 0xF);
	WriteCMOS(STATREG_A, statreg);

	totTicks = 0;
	nSeconds = 0;

}
