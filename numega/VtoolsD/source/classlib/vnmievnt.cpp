// vnmievnt.cpp - implementation of VNMIEvent class
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
#include <vtoolscp.h>

// Function
//	VNMIEvent::VNMIEvent
//
// Syntax
//	VNMIEvent::VNMIEvent()
//
// Remarks
//	Use this class as a base class when you want to hook an NMI
//	event. See vfault.h for example.
//
VNMIEvent::VNMIEvent() : VFault(2)
{
}

// These are private functions that may not be overridden

BOOL VNMIEvent::_hookProc(int i, PVOID hook, PVOID* prev)
{
	*prev = 0;
	Hook_NMI_Event((PNMIEvent_HANDLER) hook, NULL);

	return TRUE;
}


BOOL VNMIEvent::_unhookProc(int i, PVOID hook)
{
#ifdef WIN40
	Unhook_NMI_Event((PNMIEvent_HANDLER) hook, NULL);
	return TRUE;
#else
	DEBUGEVENT(DBG_WARNING, "Cannot unhook NMI Event");
	return FALSE;
#endif

}

