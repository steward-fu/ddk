// vecnew.cpp - _vector_new_ function for BC4.5x
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

typedef unsigned int _uint;

void* __cdecl _vector_new_(
	void* BaseOfStore,
	_uint SizeOfElement,
	_uint NumberOfElements,
	_uint _Flags,
	void* Constructor)
{
	int i, TotalSize;
	void* b;

	if (BaseOfStore == NULL)
	{
		TotalSize = SizeOfElement*NumberOfElements;

		if (_Flags & 0x10)
			TotalSize += 4;

		BaseOfStore = (void*)(new char[TotalSize]);
		if (BaseOfStore == NULL)
			return NULL;
	}

	if (_Flags & 0x10)
	{
		*(int*)BaseOfStore = NumberOfElements;
		BaseOfStore = (void*)((char*)BaseOfStore + 4);
	}

	if (Constructor == NULL)
		return BaseOfStore;

	int CtorType = _Flags & 0x7;

	for (b = BaseOfStore; NumberOfElements-- > 0; b = (void*)((char*)b + SizeOfElement))
	{
		switch (CtorType)
		{
		case 1:  
			((void (__cdecl *)(void*))Constructor)(b);
			break;

		case 2:
			((void (__stdcall *)(void*))Constructor)(b);
			break;

		case 3:
			_asm mov eax, b
			((void (__cdecl *)(void))Constructor)();
			break;

		case 5:
			((void (__stdcall *)(void*))Constructor)(b);
			break;

		default:
#ifdef DEBUG
			dout << "vector_new: bad constructor type\n\r";
			DebugBreak();
#endif
			break;

		}
	}

	return BaseOfStore;
}
