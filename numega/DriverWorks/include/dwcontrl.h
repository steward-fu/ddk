// dwcontrl.h - DriverWorks controller for WDM
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
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

#if _WDM_

#ifndef __DWCONTROL__
#define __DWCONTROL__

#if !defined(_DW_INCLUDE_NTDDK_)

struct CONTROLLER_OBJECT
{
	KDEVICE_QUEUE	m_queue;
	KSPIN_LOCK		m_lock;
};

typedef CONTROLLER_OBJECT* PCONTROLLER_OBJECT;

PCONTROLLER_OBJECT DwCreateController(ULONG extsize);
VOID DwDeleteController(PCONTROLLER_OBJECT pController);
VOID DwAllocateController(
	PCONTROLLER_OBJECT  ControllerObject,
	PDEVICE_OBJECT  DeviceObject,
	PDRIVER_CONTROL  ExecutionRoutine,
	PVOID  Context
	);
VOID DwFreeController(PCONTROLLER_OBJECT pController);

#define IoCreateController		DwCreateController
#define IoDeleteController		DwDeleteController
#define IoAllocateController 	DwAllocateController
#define	IoFreeController		DwFreeController

#endif
#endif // __DWCONTROL__

#endif // WDM
