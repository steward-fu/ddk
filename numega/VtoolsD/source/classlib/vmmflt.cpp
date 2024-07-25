// vmmflt.cpp - implementation of VVMMFault class
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

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VVMMFault::VVMMFault(int)
//
// Syntax
//	VVMMFault::VVMMFault(int i)
//	
// Parameters
//	i	Specifies the fault number
//
// Remarks
//	Use this class as a base class when you want to hook a fault that
//	occurs at ring 0.  See VFault::VFault for more details.
//
VVMMFault::VVMMFault(int i) : VFault(i)
{
}

// These are private functions that may not be overridden

BOOL VVMMFault::_hookProc(int i, PVOID hook, PVOID* prev)
{
	*prev = Hook_VMM_Fault(i, (PVMMFault_HANDLER) hook, NULL);
	return  (*prev != (PVOID)-1);
}


BOOL VVMMFault::_unhookProc(int i, PVOID hook)
{
#ifndef WIN40
	DEBUGEVENT(DBG_WARNING, "Cannot unhook VMM faults");
	return FALSE;
#else
	return (Unhook_VMM_Fault(i, (PVMMFault_HANDLER) hook, NULL) != 0);
#endif
}

