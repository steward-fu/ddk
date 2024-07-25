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

// COMMHOOK.h - include file for VxD COMMHOOK
// Copyright (c) 1996, Compuware Corporation

#include <vtoolsc.h>

#define COMMHOOK_Major		1
#define COMMHOOK_Minor		0
#define COMMHOOK_DeviceID	UNDEFINED_DEVICE_ID
#define COMMHOOK_Init_Order	(PORT_INIT_ORDER - 1)

typedef struct _OpenInfo
{
	PCHAR	oi_portname;
	PCOMMPORTOPENFUNC oi_openfunc;
	PPORTDATA oi_portdata;
	PortFunctions* oi_originalfuncs;
	PortFunctions oi_localfuncs;

} OPENINFO, *POPENINFO;

typedef BOOL (*ADDPORTADDR)(PVOID RefData, PCOMMPORTOPENFUNC pPortOpen, PCHAR szPortName);
typedef	BOOL (__cdecl *PCLOSEFUNC)(PPORTDATA);
typedef BOOL (__cdecl *PREADFUNC)(PPORTDATA, PCHAR, DWORD, PDWORD);
typedef BOOL (__cdecl *PWRITEFUNC)(PPORTDATA, PCHAR, DWORD, PDWORD);
