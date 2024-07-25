// vinchv86.cpp - implementation of class VInChainV86Int
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
//	VInChainV86Int::VInChainV86Int
// Syntax
//	VInChainV86Int::VInChainV86Int(DWORD intNumber)
//
// Remarks
//	Constructs an instance of class VInChainV86Int.
//
VInChainV86Int::VInChainV86Int(DWORD intNumber) :
	VInChainInt(intNumber), VV86Callback((PVOID) intNumber)
{
}

// Private members

VOID VInChainV86Int::_getVector(WORD* pSegment, DWORD* pOffset)
{
	Get_V86_Int_Vector(m_iInt, pSegment, pOffset);
}

VOID VInChainV86Int::_setVector(WORD wSegment, DWORD dwOffset)
{
	Set_V86_Int_Vector(m_iInt, wSegment, dwOffset);
}
