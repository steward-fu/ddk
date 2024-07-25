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

// CALLWRPP.C - wrapper functions for C++ CALLSERV example
// Copyright (c) 1994 Compuware Corporation

#include "calleep.h"

// Link this module into the VxD that calls the services in CALLEEP.
// The return lines in these functions are just to satisfy the compiler;
// they are never actually executed, because control jumps to the CALLEEP
// VxD.

// Using Microsoft C++, no stack frame is generated when optimization 
// is turned on. This is the desired case, because it enables a direct
// jump (as opposed to a call) to the service. 

// Using Borland C++, a stack frame is generated when there are parameters, 
// and it must be popped off before jumping. See SetFrame macro below.

// The Microsoft compiler will also generate a stack frame when 
// optimizations are disabled.


#ifdef __BORLANDC__
#define SetFrame	__emit__(0x5d) // POP EBP
#define NAKED 
#else
#define	SetFrame	// do nothing for Microsoft (assumes optimizations)
#define NAKED  __declspec(naked)
#endif


DWORD _cdecl CalleepDevice::CALLEEP_Get_Version()
{
	DWORD ver;

// We must use a VxDCall, not a VxDJmp for the Get_Version call. This allows a 
// graceful recovery if the called VxD is not present. Since there are no 
// parameters to pass, no special action is required.

	VxDCall (CALLEEP_Get_Version);
	_asm mov ver, eax
	return ver;
}

NAKED DWORD _cdecl CalleepDevice::CALLEEP_Sum(DWORD arg1, DWORD arg2)
{
	SetFrame;
	VxDJmp (CALLEEP_Sum);

#ifdef __BORLANDC__
	return 0;
#endif
}

NAKED PCHAR _cdecl CalleepDevice::CALLEEP_ToUpper(PCHAR s)
{
	SetFrame;
	VxDJmp (CALLEEP_ToUpper);
#ifdef __BORLANDC__
	return 0;
#endif
}
