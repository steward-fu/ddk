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

// CLASSTUT.CPP - main module for VxD CLASSTUT
//
// Copyright (c) 1994, Compuware Corporation  All rights reserved

#define DEVICE_MAIN
#include "classtut.h"
#include "dbwinstr.h"

Declare_Virtual_Device(CLASSTUT)

Vdbwinstream* pDwin;

DWORD ClasstutDevice::CLASSTUT_GetVersion()
{
	return (CLASSTUT_Major << 8) | CLASSTUT_Minor;
}


BOOL ClasstutDevice::OnDeviceInit(VMHANDLE hVM, PCHAR szCmdTail)
{
	VEvent::initEvents();
	pDwin = new Vdbwinstream;
	return TRUE;
}


BOOL ClasstutDevice::OnCreateVM(VMHANDLE hVM)
{
	*pDwin << "VM created, handle = " << (DWORD)hVM << endl;
	new ClasstutVM(hVM);
	return TRUE;
}

ClasstutVM::ClasstutVM(VMHANDLE hVM) : VVirtualMachine(hVM) {}

VOID ClasstutVM::OnDestroyVM()
{
	*pDwin << "VM destroyed, handle = " << (DWORD)m_handle << endl;
	delete this;
}


