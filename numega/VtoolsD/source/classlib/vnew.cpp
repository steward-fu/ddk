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
// vnew.cpp - 	implementation of ::operator new and ::operator delete
//		for VxD environment
//

#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

int _cdecl __default_nh(size_t)
{
#ifdef DEBUG
	DEBUGEVENT(DBG_WARNING, "out of memory in operator new");
#endif
	return 0;
};

_PNH __newhandler = __default_nh;

_PNH _cdecl _set_new_handler(_PNH pHandler)
{
	_PNH old = __newhandler;
	__newhandler = pHandler;

	return old;
}

// Function
//	::operator new( size_t n )
// Syntax
//	void* _cdecl ::operator new( size_t n )
//
// Parameters
//	n	size in bytes of memory chunk to allocate
//
// Remarks
//	This uses the VxD _HeapAllocate function.
//
// Return Value
//	Returns a pointer to the allocated memory, or NULL if the
//	allocation fails.
//
#ifdef __BORLANDC__
void* _cdecl operator new( size_t n )
#else
void* _cdecl ::operator new( size_t n )
#endif
{
	void *p;
	do
	{
		p = _HeapAllocate(n, 0);
	} while ( (p == 0) && __newhandler(n) );

	return p;
}

// Function
//	::operator delete(void* p)
//
// Syntax
//	void _cdecl ::operator delete(void* p)
//
// Parameters
//	p	pointer to memory to be freed
//
#ifdef __BORLANDC__
void _cdecl operator delete(void* p)
#else
void _cdecl ::operator delete(void* p)
#endif
{
	if (p) _HeapFree(p, 0);
}



#ifdef __BORLANDC__
// Function
//	::operator new[]( size_t n )
// Syntax
//	void* _cdecl ::operator new( size_t n )
//
// Parameters
//	n	size in bytes of memory chunk to allocate
//
// Remarks
//	This uses the VxD _HeapAllocate function.
//
// Return Value
//	Returns a pointer to the allocated memory, or NULL if the
//	allocation fails.
//
void* _cdecl operator new[]( size_t n )
{
	void *p;
	do
	{
		p = _HeapAllocate(n, 0);
	} while ( (p == 0) && __newhandler(n) );

	return p;
}

void _cdecl operator delete[](void* ptr)
{
	if (ptr) _HeapFree(ptr,0);
}

#endif

#if defined _MSC_VER && _MSC_VER >= 1310
// Function
//	::operator new[]( size_t n )
// Syntax
//	void* _cdecl ::operator new( size_t n )
//
// Parameters
//	n	size in bytes of memory chunk to allocate
//
// Remarks
//	This uses the VxD _HeapAllocate function.
//
// Return Value
//	Returns a pointer to the allocated memory, or NULL if the
//	allocation fails.
//
void* _cdecl ::operator new[]( size_t n )
{
	void *p;
	do
	{
		p = _HeapAllocate(n, 0);
	} while ( (p == 0) && __newhandler(n) );

	return p;
}

void _cdecl ::operator delete[](void* ptr)
{
	if (ptr) _HeapFree(ptr,0);
}

#endif



