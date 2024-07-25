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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in 
// Compuware Corporation license agreement and as provided in 
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995), 
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995), 
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.  
// Compuware Corporation.
// 
// This product contains confidential information and trade secrets 
// of Compuware Corporation. Use, disclosure, or reproduction is 
// prohibited without the prior express written permission of Compuware 
// Corporation.
//
//=============================================================================
//
// vcontrol.cpp - API dispatchers for VxD framework
//
#include <vtoolscp.h>

extern "C" VDevice* TheDevice;
extern "C" VOID _stdcall _V86_API(VMHANDLE hVM, CLIENT_STRUCT *pRegs);
extern "C" VOID _stdcall _PM_API(VMHANDLE hVM, CLIENT_STRUCT *pRegs);

VDevice* TheDevice=0;

// Function
//	_V86_API - dispatch to device's V86 API entry
//
// Syntax
//	VOID _V86_API(VMHANDLE hVM, CLIENT_STRUCT *pRegs)
//
// Parameters
//	hVM		VMHANDLE of calling virtual machine
//	pRegs		pointer to calling VM's register structure
//
VOID _stdcall _V86_API(VMHANDLE hVM, CLIENT_STRUCT *pRegs)
{
	TheDevice->V86_API_Entry(hVM, pRegs);
}

// Function
//	_PM_API - dispatch to device's V86 API entry
//
// Syntax
//	VOID _PM_API(VMHANDLE hVM, CLIENT_STRUCT *pRegs)
//
// Parameters
//	hVM		VMHANDLE of calling virtual machine
//	pRegs		pointer to calling VM's register structure
//
VOID _stdcall _PM_API(VMHANDLE hVM, CLIENT_STRUCT *pRegs)
{
	TheDevice->PM_API_Entry(hVM, pRegs);
}



