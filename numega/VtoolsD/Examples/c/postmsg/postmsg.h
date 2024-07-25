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

// POSTMSG.H
// Copyright (c) 1995, Compuware Corporation

#ifndef NotVxD
#include <vtoolsc.h>		// primary include for VtoolsD	
#include "cmosio.h"

// Device Parameters

#define POSTMSG_Major		1
#define POSTMSG_Minor		0
#ifdef WIN40
#define POSTMSG_DeviceID	UNDEFINED_DEVICE_ID  
#else
#define POSTMSG_DeviceID	NUMEGA_TEST_ID
#endif

// Note: Each vendor is responsible for obtaining device IDs
//       from Microsoft. See you VtoolsD Manual for details.
#define POSTMSG_Init_Order	UNDEFINED_INIT_ORDER

#define RTC_IRQ 8		// IRQ (not vector number) used by real 
				// time clock

// Forward declarations for functions
BOOL __stdcall POSTMSG_Int_Handler(VMHANDLE hVM, IRQHANDLE hIRQ);
VOID __stdcall EventService(VMHANDLE hVM, PVOID Refdata, PCLIENT_STRUCT pRegs, DWORD flags);

#define WM_COMMAND		0x111
#endif

// Definitions to be shared with app go here

#define PMVXD_REGISTER		0x1000
#define PMVXD_DEREGISTER	0x1001
#define PMVXD_EVENT		0x0800

#ifndef NUMEGA_TEST_ID
#define NUMEGA_TEST_ID 		0x3180
#endif
