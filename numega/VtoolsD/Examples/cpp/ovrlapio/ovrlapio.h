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

// OVRLAPIO.h - include file for VxD OVRLAPIO
// Copyright (c) 1996, Compuware Corporation

#include <vtoolscp.h>
#include "ovlp.h"

#define DEVICE_CLASS		OvrlapioDevice
#define OVRLAPIO_DeviceID	UNDEFINED_DEVICE_ID
#define OVRLAPIO_Init_Order	UNDEFINED_INIT_ORDER
#define OVRLAPIO_Major		1
#define OVRLAPIO_Minor		0

class OvrlapioDevice : public VDevice
{
public:
	virtual DWORD OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams);
};

class SimulateIOTimeOut : public VGlobalTimeOut
{
public:
	SimulateIOTimeOut();
	virtual VOID handler(VMHANDLE hVM, THREADHANDLE hThread, 
				PCLIENT_STRUCT pRegs, DWORD lagTime);

	OVERLAPPED* m_pOvp;
};

#define DELAY_MSEC  3000
#define PAGENUM(p) (((ULONG)(p)) >> 12)
#define _NPAGES_(p, k)  ((PAGENUM((char*)p+(k-1)) - PAGENUM(p)) + 1)
