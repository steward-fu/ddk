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

// APPYTEST.cpp - main module for VxD APPYTEST
// Copyright (c) 1994, Compuware Corporation

// This is a simple demonstration of class VAppyTimeEvent. When the VxD
// receives control message CREATE_VM, it schedules an appy time event.
// The event handler uses member function CallDLL to call the ring 3 API
// MessageBox.

#define DEVICE_MAIN
#include "appytest.h"
Declare_Virtual_Device(APPYTEST)
#undef DEVICE_MAIN


BOOL AppytestDevice::OnSysDynamicDeviceInit(void)
{
	return VEvent::initEvents();
}

BOOL AppytestDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail)
{
	return VEvent::initEvents();
}

BOOL AppytestDevice::OnCreateVM(VMHANDLE hVM)
{
	return (new MyAppyEvent())->schedule();
}

MyAppyEvent::MyAppyEvent() : VAppyTimeEvent(0) {}

#include PAGEABLE_CODE_SEGMENT

VOID MyAppyEvent::handler(PVOID ref, DWORD flags)
{
	struct {
		WORD style;
		DWORD szTitle;
		DWORD szText;
		WORD hWnd;
	} msgboxargs;

	PVOID la;

	msgboxargs.hWnd = NULL;
	msgboxargs.szText = LocalAlloc(LMEM_STRING, 0, la, "New VM Created");
	msgboxargs.szTitle = LocalAlloc(LMEM_STRING, 0, la, "Appy Time Test");
	msgboxargs.style = MB_OK;

	CallDLL("USER", "MESSAGEBOX", sizeof(msgboxargs), &msgboxargs);

	LocalFree(msgboxargs.szText);
	LocalFree(msgboxargs.szTitle);
}
