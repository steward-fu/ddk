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

//  logger.c - Main program module for LOGGER example VxD.
//
//  Copyright (c) 1994 by Compuware Corporation
//
//


#include  "logger.h"

#include    INIT_CODE_SEGMENT
#include    INIT_DATA_SEGMENT


BOOL LOG_Allocate(DWORD nBytes);
VOID __stdcall OnTimeout(VMHANDLE hVM, PCLIENT_STRUCT pcrs, VOID * refdata, DWORD Extra);
BOOL __stdcall OnV86Int21(DWORD Intunm, VMHANDLE hVM, PCLIENT_STRUCT pcrs);
PVOID __stdcall OnHwInt(VMHANDLE hVM, IRQHANDLE hIRQ);

extern HWInt_THUNK	pHWInt_thunk;
extern V86Int_THUNK	pV86Int_thunk;
extern TIMEOUT_THUNK	pTimeout_thunk;

VOID LOGGER_InitEvents(void)
{
// For demonstration purposes, force an assertion failure
	Assert(FALSE);

// Install a handler for all hardware interrupts (EXPENSIVE!)
//	PrevHWInt = VPICD_Call_When_Hw_Int(OnHwInt, &pHWInt_thunk);

// Install a handler for DOS int-21 events (EXPENSIVE)
	Assert(
	       Hook_V86_Int_Chain(0x21, OnV86Int21, &pV86Int_thunk)
	       );

// Install a handler that triggers every 20 seconds
	Assert(
	       Set_Global_Time_Out(20 * 1000, 0, OnTimeout, &pTimeout_thunk)
	       );
}


BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
    CHAR	    buffer[100];
    DWORD	    theSize;
    DWORD	    theStatus;

// Break into debugger for test purposes

    dprintf("Logger: init %d%% complete\n", 50);		// Test DPRINTF

    DEBUGERROR("Device Init");

// Get default log size from profile

    theSize = Get_Profile_Decimal_Int(4096, "logger",  "logsize", &theStatus);

// Create the log

    if (!LOG_Allocate(theSize))
    {
	DEBUGERROR("failed to allocate log");
    }

// Post initial event

    LOGGER_AddString("Logger Init");

// Initialize for any other events that we want to monitor

    LOGGER_InitEvents();

    return TRUE;
}


BOOL OnInitComplete(VMHANDLE hVM, PCHAR CommandTail)
{
    LOGGER_AddString("Logger init complete");
    return TRUE;
}

