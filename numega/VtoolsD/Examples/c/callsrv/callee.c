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

// CALLEE.c - main module for VxD CALLEE
// Copyright (c) 1994 Compuware Corporation

// This example VxD provides services to the CALLER VxD.

#define   DEVICE_MAIN
#include  "callee.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(CALLEE)

DWORD _cdecl CALLEE_Get_Version()
{
	return 0x10;
}

DWORD _cdecl CALLEE_Sum(DWORD arg1, DWORD arg2)
{
	return arg1+arg2;
}

PCHAR _cdecl CALLEE_ToUpper(PCHAR s)
{
	return _strupr(s);
}

