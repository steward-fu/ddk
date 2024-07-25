// vpmvend.cpp - implementation of class VPMVendorExtension
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
//	VPMVendorExtension::VPMVendorExtension
//
// Syntax
//	VPMVendorExtension::VPMVendorExtension(PCHAR idString) :
//
// Parameters
//	idString	string to uniquely identify vendor extension
//
// Remarks
//	Constructs an instance of class VPMVendorExtension.  This provides
//	a trap of INT 2Fh/AX=168Ah for protected mode applications in the
//	current virtual machine.
//
//	After invoking the constructor, supply the callback address to be
//	returned by the trap on a successful string match by calling
//	member function setCbAddr.
//
VPMVendorExtension::VPMVendorExtension(PCHAR idString) :
	VInChainPMInt(0x2f), VVendorExtension(idString) {}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Function
//	VPMVendorExtension::handler
//
// Syntax
//	BOOL VPMVendorExtension::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
//
// Parameters
//	hVM	handle of current virtual machine
//	pRegs	pointer to client register structure
//	iInt	number of interrupt (will always be 0x2F)
//
// Remarks
//	This function gains control on an INT 2Fh issued by a protected
//	mode application running in a virtual machine in which an instance
//	of VPMVendorExtension was successfully created.  It calls member
//	function test (shared by PM and V86 classes) to implement the
//	DPMI function "Get Vendor-Specific API Entry Point".
//
//	Unlike most other member functions named handler in the VToolsD
//	class library, this function is not necessarily intended to be 
//	overriden.
//
// Return Value
//	Returns FALSE, which is ignored.
//
BOOL VPMVendorExtension::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	if (test(pRegs))
		setReturn();
	else
		chainToPrevious();

	return FALSE;
}

#ifdef __BORLANDC__
VOID VPMVendorExtension::setReturn() 
{
	VxDCall(Simulate_Iret);	
}
#endif
