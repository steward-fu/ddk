// Common definitions for exe/sys filter driver test
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

#define IOCTL_HOOK \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UNHOOK \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MODIFY \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define FIFOSIZE 100

struct HOOK_PARMS {
    ULONG	Comm;
    HANDLE	hEvent;
};

#ifndef _IA64_
typedef ULONG ULONG_PTR;
#endif

typedef struct s_RECORD {
    ULONG_PTR	sequence;
    ULONG	function;
    ULONG	status;
    ULONG	information;
} RECORD;
