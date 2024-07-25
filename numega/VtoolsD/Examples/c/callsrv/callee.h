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

// CALLEE.H - include file for VxD CALLEE
// Copyright (c) 1994 Compuware Corporation

#include <vtoolsc.h>

#define CALLEE_Major		1
#define CALLEE_Minor		0
#define CALLEE_DeviceID		NUMEGA_TEST_ID
#define CALLEE_Init_Order	0x7f000000

DWORD _cdecl CALLEE_Get_Version(void);
DWORD _cdecl CALLEE_Sum(DWORD arg1, DWORD arg2);
PCHAR _cdecl CALLEE_ToUpper(PCHAR s);

Begin_VxD_Service_Table(CALLEE)
	VxD_Service(CALLEE_Get_Version)
	VxD_Service(CALLEE_Sum)
	VxD_Service(CALLEE_ToUpper)
End_VxD_Service_Table

