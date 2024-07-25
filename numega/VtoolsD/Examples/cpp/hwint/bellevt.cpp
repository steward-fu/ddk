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

// BELLEVT.CPP - implementation of class BellEvent
// Copyright (c) 1995, Compuware Corporation
//
// Class BellEvent provides a simple illustration of how to use class
// VGlobalEvent from the VtoolsD class library. Its purpose is to allow
// code that runs at interrupt level to sound a signal tone. An event
// is necessary to do this, because VSD_Bell cannot be called at interrupt
// level. 
//
// To use this class, allocate an instance and call member function "call"
// or member function "schedule". Like all classes derived from VEvent, it
// is safe to use new at interrupt level, and the object is automatically
// deleted after the handler runs.

#include <vtoolscp.h>		// Primary include file for VtoolsD classlib
#include <vsd.h>		// Include file for Virtual Sound Device
#include "bellevt.h"		// Include file for this class

// Function
// 	BellEvent::BellEvent - constructor
//
// Remarks
//	Creates an instance of class BellEvent, which is derived from
//	VGlobalEvent.
//
BellEvent::BellEvent(VOID) : VGlobalEvent(0) {}


// Function
// 	BellEvent::handler - handler for BellEvent
//
// Input
//	hVM		Handle of current virtual machine
//	pRegs		Pointer to client register structure
//	refData		not used
//
// Remarks
//	Calls VSD_Bell to sound a half-second tone.
//	
//
VOID BellEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	VSD_Bell(); 		// returns immediately
}
