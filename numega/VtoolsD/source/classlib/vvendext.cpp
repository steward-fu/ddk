// vvendext.cpp - implementation of class VVendorExtension
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
//	VVendorExtension::VVendorExtension
//
// Syntax
//	VVendorExtension::VVendorExtension(PCHAR idString)
//
// Parameters
//	idString	string to be used as unique identifier
//
// Remarks
//	Constructs an instance of class VVendorExtension.  This is used
//	only as a base class for VV86VendorExtension and VPMVendorExtension.
//
//	The purpose of the class is to implement INT 2Fh/AX=168Ah, namely
//	the DPMI "Get Vendor-Specific API Entry Point" function.
//	VVendorExtensions and classes derived from it trap INT 2Fh and
//	check for AX=168Ah and DS:eSI pointing to the identifier string.
//	When found, a callback address is returned in ES:eDI and AL is
//	set to zero.
//	
//	The constructor makes a copy of the string to be stored with the
//	object for comparison by member function test.
//
VVendorExtension::VVendorExtension(PCHAR idString)
{
	m_pIdString = new char[strlen(idString)+1];
	if (m_pIdString)
		strcpy(m_pIdString, idString);
}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Function
//	VVendorExtension::test
//
// Syntax
//	BOOL VVendorExtension::test(CLIENT_STRUCT* pRegs)
//
// Parameters
//	pRegs	pointer to client register structure
//
// Remarks
//	This function is called on INT 2Fh by member function handler of 
//	VV86VendorExtension and VPMVendorExtension.  It first checks
//	to determine if the client AX value is 168Ah. If so, it compares
//	the string pointed to by the client's DS:eSI against the string
//	provided to the constructor.  If they match, the function sets
//	the client's ES:eDI to the callback address provided by member
//	function setCbAddr(), and sets client AL to zero to signal that
//	a match was found.  
//
// Return Value
//	Returns TRUE if client AX=168Ah and the strings match.  Otherwise,
//	returns FALSE.
//
BOOL VVendorExtension::test(CLIENT_STRUCT* pRegs)
{
	PCHAR pString;

	if (pRegs->CWRS.Client_AX != 0x168A)
		return FALSE;

	pString = (PCHAR)Map_Flat(CLIENT_DS, CLIENT_SI);

	if (strcmp(pString, m_pIdString) == 0)
	{
		_clientES = m_CbAddr >> 16;
		_clientEDI = m_CbAddr & 0xFFFF;
		_clientAL = 0;

		return TRUE;
	}

	return FALSE;
}



