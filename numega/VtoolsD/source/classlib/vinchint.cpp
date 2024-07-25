// vinchint.cpp - implementation of class VInChainInt
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


#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VInChainInt::VInChainInt
//
// Syntax
//	VInChainInt::VInChainInt(DWORD iInt)
//
// Parameters
//	iInt	specifies the interrrupt number
//
// Remarks
//	Constructs an instance of class VInChainInt.  This is a base
//	class for VInChainV86Int and VInChainPMInt.
//
VInChainInt::VInChainInt(DWORD iInt)
{
	m_iInt = iInt;
	m_flags = 0;
}

// Function
//	VInChainInt::hook
//
// Syntax
//	BOOL VInChainInt::hook()
//
// Remarks
//	Hooks an interrupt by setting the interrupt vector to a
//	callback address. The callback is  automatically targeted to 
//	member function handler. See (VCallback::handler)
//	
// Return Value
//	Returns TRUE if successful.  The only failure case is when the
//	callback could not be allocated by the class constructor.
//
BOOL VInChainInt::hook()
{
	DWORD vector = _getCallback();
	if (vector == 0)
		return FALSE;

	_getVector(&m_prevSegment, &m_prevOffset);
	_setVector(vector >> 16, vector & 0xffffL);

	return TRUE;
}

// Function
//	VInChainInt::unhook
//
// Syntax
//	BOOL VInChainInt::unhook()
//
// Remarks
//	Attempts to unhook a previously hooked interrupt.  If the 
//	current vector is not equal to the callback address, the
//	interrupt is not unhooked, as doing so could cause a crash.
//
// Return Value
//	Returns TRUE if the interrupt is successfully unhooked.
//
BOOL VInChainInt::unhook()
{

	DWORD dwOffset;
	WORD wSegment;
	DWORD vector = _getCallback();

	_getVector(&wSegment, &dwOffset);
	if ( (wSegment == (vector >> 16)) &&
	     (dwOffset == (vector & 0xffff)))
	{
		_setVector(m_prevSegment, m_prevOffset);
		return TRUE;
	}
	else
		return FALSE;
}

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

// Function
//	VInChainInt::chainToPrevious
//
// Syntax
//	VOID VInChainInt::chainToPrevious()
//
// Remarks
//	Modifies the CS:eIP of the current virtual machine such that
//	when the handler returns, it will run the interrupt handler
//	found in the interrupt vector table at the time the interrupt
//	was hooked.
//
//	This function may ONLY be called from inside the member function
//	handler.
//
VOID VInChainInt::chainToPrevious()
{
	Simulate_Far_Jmp(m_prevSegment, m_prevOffset);
}


#ifdef __BORLANDC__
VOID VInChainInt::setReturn() 
{
        Simulate_Iret()
}
#endif

