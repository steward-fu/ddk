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
//=============================================================================

// CALLASM.C - demonstration of calling assembler functions from C
// Copyright (c) 1995 Compuware Corporation

// This example demonstrates how to call functions written in assembly
// language, and how functions written in assembler call C/C++ code.
// The DEVICE_INIT handler of this VxD calls three functions, each of
// which is written in assembler. 

// The first function called is __cdecl. Name decoration is a leading
// underscore, and the caller is responsible for cleaning up the stack.

// The second function called is __stdcall. Name decoration varies between
// compilers. The called function is responsible for cleaning up the
// stack.

// The third function accepts arguments in registers. The caller uses
// in-line assembler to load the registers. It also makes two calls
// back into this code, one of which is __cdecl and one of which is 
// __stdcall.

#define   DEVICE_MAIN
#include  "callasm.h"
#undef    DEVICE_MAIN

// Declare external demonstration entry points defined in FUNCS.ASM

// example __cdecl function
extern DWORD __cdecl   Sum_ThisIsACdecl(DWORD a1, DWORD a2);

// example __stdcall function
extern DWORD __stdcall Diff_ThisIsAStdcall(DWORD a1, DWORD a2);

// example register interface
extern DWORD Sum_ThisAddsECXandEDX();

// Device parameters
Declare_Virtual_Device(CALLASM)

// Declare prototypes for control message handlers
DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);

DWORD Sum_ThisAddsECXandEDX(void);
VOID CallAsmTest(void);

// This is the control message dispatcher
BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH
		ON_DEVICE_INIT(OnDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
	END_CONTROL_DISPATCH

	return TRUE;
}

// This is the handler for control message DEVICE_INIT
BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	CallAsmTest();
	return TRUE;
}


// This is the handler for control message SYS_DYNAMIC_DEVICE_INIT
BOOL OnSysDynamicDeviceInit(void)
{
	CallAsmTest();
	return TRUE;
}


VOID CallAsmTest(void)
{
	DWORD result;
	DWORD a1, a2;

	a1 = 8;
	a2 = 5;
	result = Sum_ThisIsACdecl(a1, a2);
	dprintf("sum of %d and %d is %d\n", a1, a2, result);

	result = Diff_ThisIsAStdcall(a1, a2);
	dprintf("difference of %d and %d is %d\n", a1, a2, result);

	_asm mov ecx, a1
	_asm mov edx, a2
	result = Sum_ThisAddsECXandEDX();
	dprintf("sum of %d and %d is %d\n", a1, a2, result);
}

// These two functions are invoked from inside the above call to 
// Sum_ThisAddsECXandEDX

VOID __cdecl CallThisCdeclFromAssembler(PCHAR s)
{
	dprintf("Cdecl called: %s\n", s);
}


VOID __stdcall CallThisStdcallFromAssembler(PCHAR s)
{
	dprintf("Stdcall called: %s\n", s);
}
