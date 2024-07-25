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

// HWINT.H
// Copyright (c) 1995, Compuware Corporation

#include <vtoolsc.h>		// primary include for VtoolsD	
#include "cmosio.h"

// Device Parameters

#define HWINT_Major		1
#define HWINT_Minor		0
#define HWINT_DeviceID		UNDEFINED_DEVICE_ID
#define HWINT_Init_Order	UNDEFINED_INIT_ORDER


#define RTC_IRQ 8		// IRQ (not vector number) used by real 
				// time clock

// Forward declarations for functions
BOOL __stdcall RTCInt_Handler(VMHANDLE hVM, IRQHANDLE hIRQ);
VOID __stdcall SecondEventService(VMHANDLE hVM, PVOID Refdata, PCLIENT_STRUCT pRegs);

// Macro to convert a byte of binary coded decimal (BCD) to int

#define BCDtoInt(x) (((x) & 0xf) + ((((x) & 0xf0) >> 4) * 10))
