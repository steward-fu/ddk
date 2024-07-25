// debug.cpp - implementation of NT debug functions for VtoolsD
// Copyright (c) 1997 Compuware Corporation

#include <vdw.h>

VOID __cdecl DbgPrint(PCHAR format, ...)
{
	char	temp[256];
	va_list	Next;

	va_start(Next, format);
	_vsnprintf(temp, 128, format, Next);
	 Out_Debug_String(temp);
}
