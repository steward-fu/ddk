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

// APPYTIME.C - main module for VxD APPYTIME
// Copyright (c) 1994, Compuware Corporation

#define   DEVICE_MAIN
#include  "appytime.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(APPYTIME)

DefineControlHandler(CREATE_VM, OnCreateVm);
DefineControlHandler(DESTROY_VM, OnDestroyVm);

#define _CREATE_	1
#define _DESTROY_	0

BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_CREATE_VM(OnCreateVm);
		ON_DESTROY_VM(OnDestroyVm);

	END_CONTROL_DISPATCH

	return TRUE;
}


VOID _cdecl Appy_Handler(PVOID RefData, DWORD flags)
{
	struct {
		WORD style;
		SEGOFFSET szTitle;
		SEGOFFSET szText;
		WORD hWnd;
	} msgboxargs;

	PVOID la;

	char* msg = ((DWORD)RefData==_CREATE_) ? "New VM Created" : "VM destroyed";


	msgboxargs.hWnd = 0;
	msgboxargs.szText = _SHELL_LocalAllocEx(LMEM_STRING, 0, msg, &la);
	msgboxargs.szTitle = _SHELL_LocalAllocEx(LMEM_STRING, 0, "Appy Time Test", &la);
	msgboxargs.style = MB_OK;

	_SHELL_CallDll("USER", "MESSAGEBOX", sizeof(msgboxargs), &msgboxargs);

	_SHELL_LocalFree(msgboxargs.szText);
	_SHELL_LocalFree(msgboxargs.szTitle);
}

BOOL OnCreateVm(VMHANDLE hVM)
{
	_SHELL_CallAtAppyTime(Appy_Handler, (PVOID)_CREATE_, 0, 0) ;
	return TRUE;
}

VOID OnDestroyVm(VMHANDLE hVM)
{
	_SHELL_CallAtAppyTime(Appy_Handler, (PVOID)_DESTROY_, 0, 0) ;
}
