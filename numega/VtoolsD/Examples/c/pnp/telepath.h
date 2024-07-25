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

// TELEPATH.h - include file for VxD TELEPATH
// Copyright (c) 1996, Compuware Corporation 
// Adapted from code written by Walter Oney for
// Microsoft Systems Journal January 1996.
// Reprinted with permission.

#include <vtoolsc.h>

#define TELEPATH_Major			1
#define TELEPATH_Minor			0
#define TELEPATH_DeviceID		UNDEFINED_DEVICE_ID
#define TELEPATH_Init_Order		UNDEFINED_INIT_ORDER

#define ResType_Telepath ((0x10 << 5) | 5)

typedef struct
{							// telepath resource description
	ULONG allocated;		// mask for allocated channel
	ULONG requested;		// mask for requested channels
} TELEPATH_RESOURCE;		// telepath resource description

