// vv86dpmi.cpp - implementation of class VV86DPMIEntry
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
//	VV86DPMIEntry::VV86DPMIEntry
//
// Syntax
//	VV86DPMIEntry::VV86DPMIEntry(PCHAR idString, PVOID refData)
//
// Parameters
//	idString	string to identify the vendor extension
//	refData		(optional) passed to handler function
//
// Remarks
//	Constructs an instance of class VV86DPMIEntry.  This is an
//	alternative to using VDevice::V86_API_Entry.  The advantage
//	is that it does not require a VxD ID assigned by Microsoft.
//	Instead, you use a string that you choose to identify
//	your VxD.  The disadvantage is that it requires an INT 2Fh
//	hook which may have a very slight impact on system performance.
//
//	You must derive a class from VV86DPMIEntry that defines a constructor
//	and overrides member function handler.  Create a constructor that
//	invokes the base class constructor (VV86DPMIEntry), passing to it
//	the ID string to identify your VxD.  V86 mode applications
//	can then use INT 2Fh/AX=168Ah to get the address of a callback
//	will invoke member function handler of the class you derive from
//	VV86DPMIEntry.
//	
VV86DPMIEntry::VV86DPMIEntry(PCHAR idString, PVOID refData)
	: VV86Callback(refData), VEtrap(idString)
{
	VEtrap.setCbAddr(getAddr());
	m_bStatus = VEtrap.hook();
}

// Function
//	VV86DPMIEntry::handler
//
// Syntax
//	BOOL VV86DPMIEntry::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, 
//		PVOID refData)
//	
// Parameters
//	hVM	handle of calling virtual machine
//	pRegs	pointer to client register structure
//	refData	reference data value passed to constructor
//
// Remarks
//	Override this member function in the class you derive from
//	VV86DPMIEntry.  When a V86 mode application makes a call to
//	the callback address returned by INT 2Fh/AX=168Ah, this handler
//	will be invoked.
//
//	Note that the handler is descended from VV86Callback::handler, 
//	and therefore requires that you call member setReturn in order to
//	modify the client stack for proper return.
//
// Return Value
//	The function is required to return a BOOLEAN value due to the
//	inheritance from VCallback.  In this case, it is ignored.
//
BOOL VV86DPMIEntry::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	// if this function is called, it means you have not correctly
	// overridden member function handler in the class you derived
	// from VV86DPMIEntry.

	DEBUGEVENT(DBG_ERROR,"\"handler\" for VV86DPMIEntry class is not defined correctly");
#ifdef DEBUG
	setReturn();
#endif
	return FALSE; 
}


