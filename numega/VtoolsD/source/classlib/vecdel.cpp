// vecdel.cpp - _vector_delete_ function for BC4.5x
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

void* __cdecl _vector_delete_(
	void* BaseOfStore,
	_uint SizeOfElement,
	_uint NumberOfElements,
	_uint _Flags,
	void* Destructor)
{
	int i, TotalSize;
	BOOL hmmm = _Flags & 0x8; 
	BOOL ummm = _Flags & 0x10;
	void* b;

	if (BaseOfStore == NULL)
		return NULL;

	if (ummm)
		NumberOfElements = *((DWORD*)BaseOfStore-1);

	b = (void*)( (char*)BaseOfStore + (NumberOfElements-1)*SizeOfElement );

	int DtorType = _Flags & 0x7;

	for (; NumberOfElements-- > 0; b = (void*)((char*)b - SizeOfElement))
	{
		switch (DtorType)
		{
		case 1:  
			((void (__cdecl *)(void*, int))Destructor)(b, 2);
			break;

		case 2:
			((void (__stdcall *)(void*, int))Destructor)(b, 2);
			break;

		case 3:
			_asm mov eax, 2
			_asm mov edx, b
			((void (__cdecl *)(void))Destructor)();
			break;

		case 5:
			((void (__stdcall *)(void*, int))Destructor)(b, 2);
			break;

		default:
#ifdef DEBUG
			dout << "vector_delete: bad destructor type\n\r";
			DebugBreak();
#endif
			break;

		}
	}

	if (ummm)
		BaseOfStore = (void*)((char*)BaseOfStore - 4);

	if (hmmm)
	{
		delete BaseOfStore;
		return NULL;
	}
	else
		return BaseOfStore;
}
