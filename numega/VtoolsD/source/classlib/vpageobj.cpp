// vpageobj.cpp - implementation of class VPageObject
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
//	VPageObject::operator new
//
// Syntax
//	void* _cdecl VPageObject::operator new(size_t n)
//
// Parameters
//	n	number of bytes to allocate
//
// Remarks
//	Allocates unlocked pages.  The number of bytes allocated is the 
//	smallest multiple of 4096 that is greater or equal to n.  The
//	pages are accessible from all virtual machines.
//
// Return Value
//	Returns a pointer to the allocated memory, or NULL if the allocation
//	fails.
//
void* _cdecl VPageObject::operator new(size_t n)
{
	PVOID p;
	DWORD handle;
	DWORD nPages = (n+0xFFF) >> 12;

	if (_PageAllocate(nPages, PG_SYS, 0, 0, 0, 0, 0, 0, &handle, &p))
		((VPageObject*)p)->m_memhandle = handle;

	if (p == 0)
		DEBUGEVENT(DBG_WARNING, "Failed to allocate VPageObject");

	return p;
}

// Function
//	VPageObject::operator delete
//
// Syntax
//	void _cdecl VPageObject::operator delete(void* p)
//
// Parameters
//	p	pointer to memory to be deleted
//
// Remarks
//	Frees the pages allocated by operator new.
//
void _cdecl VPageObject::operator delete(void* p)
{
	if (p == 0)
		return;
	
	_PageFree(((VPageObject*)p)->m_memhandle, 0);
}
