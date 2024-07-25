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

// BEEPER.CPP - example using class VGlobalTimeOut
// Copyright (c) 1995 Compuware Corporation

#define DEVICE_MAIN
#include "beeper.h"
Declare_Virtual_Device(BEEPER)
#undef DEVICE_MAIN

#include <vsd.h>


BeepTimeOut* pBeeper;

BOOL BeeperDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail)
{
	pBeeper = new BeepTimeOut(10000); // every 10 secs
	pBeeper->Set();

	return TRUE;
}


BOOL BeeperDevice::OnSysDynamicDeviceInit(void)
{
	pBeeper = new BeepTimeOut(10000); // every 10 secs
	pBeeper->Set();

	return TRUE;
}


BOOL BeeperDevice::OnSysDynamicDeviceExit(void)
{
	pBeeper->Cancel();
	delete pBeeper;

	return TRUE;
}


// Constructor for time out class
//
BeepTimeOut::BeepTimeOut(DWORD msec) : VGlobalTimeOut(msec) {}

// Handler for time out event
// (Note handler declaration is different for Windows 3.x and Windows 95)
//
#ifdef WIN40
VOID BeepTimeOut:: handler(VMHANDLE hVM, THREADHANDLE th, PCLIENT_STRUCT pRegs,
		DWORD lag)
#else
VOID BeepTimeOut:: handler(VMHANDLE hVM, PVOID reserved, PCLIENT_STRUCT pRegs,
		DWORD lag)
#endif
{
	VSD_Bell();	// sound a tone
	Set();		// reset the timer
}
