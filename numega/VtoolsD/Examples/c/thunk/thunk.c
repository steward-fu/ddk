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

//	thunk.c - Main program module for THUNK example VxD.
//
//	Copyright (c) 1994 by Compuware Corporation
//
//	This example demonstrates how to hook some of the services
//	that can be thunked, and how to unhook services which are
//	unhookable.  This example does NOT exercise the thunks;
//	some will be called in the course of normal system operation
//	(such as V86 interrupt hooks) while others will never be called
//	(such as V86 or PM callbacks).
//


#define   DEVICE_MAIN
#include  "thunk.h"
#undef	  DEVICE_MAIN

Declare_Virtual_Device(THUNK)

#define REFTEST  ( (PVOID) 0x12341234)

#define LOG(string) {DebugBreak(); Out_Debug_String(string);}

// Type safety for control handlers
//
DefineControlHandler(DEVICE_INIT, OnDeviceInit);


// These are the thunks.  The template ASM code gets
// copied here.

V86Int_THUNK		thunkV86Int;
IPF_THUNK		thunkIPF;
V86Call_THUNK		thunkV86Call;
PMCall_THUNK		thunkPMCall;
VMMFault_THUNK		thunkVMMFault;
PMFault_THUNK		thunkPMFault;
V86Fault_THUNK		thunkV86Fault;
PMApi_THUNK		thunkPMApi;
V86Api_THUNK		thunkV86Api;
Event_THUNK		thunkNotCritical;
Idle_THUNK		thunkIdle;
V86Break_THUNK		thunkV86Break;
DeviceService_THUNK	thunkDeviceService;


// Previous handlers for chaining

PVOID	PreviousVMMFaultHandler;
PVOID	PreviousPMFaultHandler;
PVOID	PreviousV86FaultHandler;
PVOID	PreviousPMApiHandler;
PVOID	PreviousV86ApiHandler;



// These are not required, but provide type safety
//
DefineControlHandler(DEVICE_INIT, OnDeviceInit);


//	Invalid Page Fault handler
//
BOOL __stdcall MyIPFHandler(VMHANDLE hVM, PIPF_DATA ipf)
{
	LOG("Ipf handler");
	return	TRUE;
}


//	V86 Interrupt handler
//
BOOL __stdcall MyV86IntHandler(DWORD Intnum, VMHANDLE hVM, PCLIENT_STRUCT pcrs)
{
	LOG("V86 Int handler\n");
	return	FALSE;
}


//	V86 Callback handler
//
BOOL __stdcall MyV86CallHandler(VMHANDLE hVM, PVOID RefData, PCLIENT_STRUCT pcrs)
{
	LOG("V86 callback handler");
	Assert(RefData == REFTEST);
	Simulate_Far_Ret();
	return	FALSE;
}


//	PM Callback handler
//
BOOL __stdcall MyPMCallHandler(VMHANDLE hVM, PVOID RefData, PCLIENT_STRUCT pcrs)
{
	LOG("PM callback handler");
	Assert(RefData == REFTEST);
	Simulate_Far_Ret();
	return	FALSE;
}


//	VMM Fault handler
//
PVOID __stdcall MyVMMFaultHandler(VMHANDLE hVM, PVOID Stackframe)
{
	LOG("VMM fault handler");

// Mark as handled by returning NULL,
// Chain by returning previous handler
	return	PreviousVMMFaultHandler;
}


//	PM Fault handler
//
PVOID __stdcall MyPMFaultHandler(VMHANDLE hVM, PVOID Stackframe)
{
	LOG("PM Fault handler");

// Mark as handled by returning NULL,
// Chain by returning previous handler
	return	PreviousPMFaultHandler;
}


//	V86 Fault handler
//
PVOID __stdcall MyV86FaultHandler(VMHANDLE hVM, PVOID Stackframe)
{
	LOG("V86 Fault handler");

// Mark as handled by returning NULL,
// Chain by returning previous handler
	return	PreviousV86FaultHandler;
}


//	V86 Device API
//
PVOID __stdcall MyV86ApiHandler(VMHANDLE hVM, PCLIENT_STRUCT pcrs)
{
	LOG("V86 API handler");
	return	PreviousV86ApiHandler;
}


//	PM Device API
//
PVOID __stdcall MyPMApiHandler(VMHANDLE hVM, PCLIENT_STRUCT pcrs)
{
	LOG("PM API handler");
	return	PreviousPMApiHandler;
}


//	V86 Break Point
//
VOID __stdcall MyV86BreakHandler(VMHANDLE hVM,
				 PCLIENT_STRUCT pcrs,
				 PVOID Refdata,
				 SEGOFFSET BreakAddress,
				 PVOID BreakLinearAddress)
{
	LOG("V86 Break handler");
	Assert(Refdata == REFTEST);
	return;
}


//	Idle
//
BOOL __stdcall MyIdleHandler(VMHANDLE hVM, PCLIENT_STRUCT pcrs)
{
	LOG("Idle handler");
	return FALSE;		// TRUE if we take a long time to run
}


//	Not Critical
//
VOID __stdcall MyNotCriticalHandler(VMHANDLE hVM, PVOID Refdata, PCLIENT_STRUCT pcrs)
{
	LOG("Not critical handler");
	Assert(Refdata == REFTEST);
	return;
}

VOID __stdcall MyDeviceServiceHandler(PDSFRAME pDS)
{
	LOG("Device Service handler called (_GetSysPageCount called)");
	return;
}


//	Control message dispatcher
//
//
BOOL ControlDispatcher(
    DWORD dwControlMessage,	// Control message number from VMM (eax)
    DWORD EBX,			// Usually current or system VM handle
    DWORD EDX,			// Additional arguments for some messages
    DWORD ESI,			// appear in these
    DWORD EDI,			// registers
    DWORD ECX)
{

    START_CONTROL_DISPATCH

	ON_DEVICE_INIT (OnDeviceInit);

    END_CONTROL_DISPATCH

    return TRUE;
}


//	Handler for Device Init system control message
//
//
BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
    Out_Debug_String("Thunk Device Init Handler\n");

    Assert(
	   Hook_V86_Int_Chain(0x21, MyV86IntHandler, &thunkV86Int));
    Assert(
	   Hook_Invalid_Page_Fault(MyIPFHandler, &thunkIPF));
    Assert(
	   Unhook_Invalid_Page_Fault(MyIPFHandler, &thunkIPF));
    Assert(
	   Allocate_V86_Call_Back(NULL, MyV86CallHandler, &thunkV86Call));
    Assert(
	   PreviousPMApiHandler = Hook_Device_PM_API(VTD_DEVICE_ID,
						     MyPMApiHandler,
						     &thunkPMApi));
    Assert(
	   PreviousV86ApiHandler = Hook_Device_V86_API(VTD_DEVICE_ID,
						       MyV86ApiHandler,
						       &thunkV86Api));
    Assert(
	   Allocate_PM_Call_Back(NULL, MyPMCallHandler, &thunkPMCall));
		    
    Call_When_Not_Critical(REFTEST, MyNotCriticalHandler, &thunkNotCritical);

    Assert(
	   Call_When_Idle(MyIdleHandler, &thunkIdle));

    Assert(
	   (PreviousVMMFaultHandler = Hook_VMM_Fault(13,
						     MyVMMFaultHandler,
						     &thunkVMMFault))
	   != (PVOID) -1);
    Assert(
	   (PreviousV86FaultHandler = Hook_V86_Fault(9,
						     MyV86FaultHandler,
						     &thunkV86Fault))
	   != (PVOID) -1);
    Assert(
	   (PreviousPMFaultHandler = Hook_PM_Fault(9,
						   MyPMFaultHandler,
						   &thunkPMFault))
	   != (PVOID) -1);
    Assert(
	   Install_V86_Break_Point(MAKE_SEGOFFSET(0xf000, 0),
				   REFTEST,
				   MyV86BreakHandler,
				   &thunkV86Break));
    Assert(
	   Remove_V86_Break_Point(MAKE_SEGOFFSET(0xf000, 0)));
    Assert(
	   Hook_Device_Service( (VMM_DEVICE_ID << 16) | (___GetSysPageCount),
				MyDeviceServiceHandler,
				&thunkDeviceService));
    _GetSysPageCount(0);


    return TRUE;
}
