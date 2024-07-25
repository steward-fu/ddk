// vgloblv86.cpp - implementation of class VGlobalV86Area
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

// Function
//	VGlobalV86Area::operator new
//
// Syntax
//	void* _cdecl VGlobalV86Area::operator new(size_t n, DWORD flags)
//
// Parameters
//	n	number of bytes to allocate, should be a multiple of 4096
//	flags	bit flag options:  see _Allocate_Global_V86_Data_Area.
//
// Remarks
//	Allocates storage in the V86 address space of all virtual machines.
//	This function will only succeed during initialization.
//
// Return Value
//	Returns the VxD level pointer to the allocated memory.
//

#include INIT_CODE_SEGMENT
#include INIT_DATA_SEGMENT

void* _cdecl VGlobalV86Area::operator new(size_t n, DWORD flags)
{
	return 	_Allocate_Global_V86_Data_Area(n, flags);
}

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VGlobalV86Area::operator delete
//
// Syntax
//	void _cdecl VGlobalV86Area::operator delete(void* p)
//
// Parameters
//	p	pointer to memory to delete
//
// Remarks
//	The call has no effect.  There is no service to deallocate
//	the storage.
//
void _cdecl VGlobalV86Area::operator delete(void* p)
{
	DEBUGEVENT(DBG_WARNING, "VGlobalV86Area be cannot deleted");
}
