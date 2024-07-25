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

// CHIME.CPP - main module for "hour chime" example
// Copyright (c) 1995 Compuware Corporation

#include "hwint.h"
#include INIT_CODE_SEGMENT

extern RTCInterrupt* pRTCInterrupt; 	// Pointer to our RTCInterrupt object
extern VOID TickHandler(VOID);		// callback for tick event
extern BYTE TargetHour;			// Next hour to ring bell
extern BYTE HourMode;			// 12 or 24 hour setting of RTC


// Function
// 	HwintDevice::OnDeviceInit - Handler for DEVICE_INIT control message
//
// Input
//	hSysVM		Handle of system virtual machine
//	pszCmdTail	Pointer to Windows command line
//
// Remarks
//	This routine is responsible for allocating an instance
//	of the RTCInterrupt class, and enabling it.
//
// Returns
//	Returns TRUE if successfully initialized
//
BOOL HwintDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail)
{
// First, create an instance of RTCInterrupt and call its member function,
// hook(). We have defined the constructor for class RTCInterrupt 
// to accept a parameter which is the address of a callback function that
// will be invoked when the RTC interrupt occurs. We pass the address of
// our function TickHandler (see below) to the constructor. If the
// initialization of the RTCInterrupt fails, we return FALSE.

	pRTCInterrupt = new RTCInterrupt(TickHandler);

	if ((pRTCInterrupt==NULL) || !pRTCInterrupt->hook())
	{
		pRTCInterrupt = 0;
		return FALSE;
	}
	else
		pRTCInterrupt->Enable();	// Enable and unmask IRQ

// Our usage of class BellEvent (see TickHandler) requires that we call
// VEvent::InitEvents during initialization. This sets up a special heap
// that allows event objects to be allocated at interrupt time.
	
	VEvent::initEvents();

// Now we just need to compute the value for the next hour when the
// chime should sound. This depends on which mode (24hr or 12hr) the
// RTC is currently in.

	if (RTCInterrupt::ReadRegister(STATREG_B) & HOURMODE24)
		HourMode = 24;
	else
		HourMode = 12;

	BYTE curHour = BCDtoInt(RTCInterrupt::ReadRegister(HOUR));

	TargetHour = (curHour + 1) % HourMode;	// set for next hour

	return TRUE;
}				


