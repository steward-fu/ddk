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

// MAPDEV.h - include file for VxD MAPDEV
// Copyright (c) 1996 Compuware Corporation

#ifndef NotVxD

#include <vtoolsc.h>

#define MAPDEV_Major		1
#define MAPDEV_Minor		0
#define MAPDEV_DeviceID		UNDEFINED_DEVICE_ID
#define MAPDEV_Init_Order	UNDEFINED_INIT_ORDER

#define LPVOID PVOID

#endif

// This is the request structure that applications use
// to request services from the MAPDEV VxD.

typedef struct _MapDevRequest
{
	DWORD	mdr_ServiceID;			// supplied by caller
	LPVOID	mdr_PhysicalAddress;	// supplied by caller
	DWORD	mdr_SizeInBytes;		// supplied by caller
	LPVOID	mdr_LinearAddress;		// returned by VxD
	WORD	mdr_Selector;			// returned if 16-bit caller
	WORD	mdr_Status;				// MDR_xxxx code below
} MAPDEVREQUEST, *PMAPDEVREQUEST;

#define MDR_SERVICE_MAP		CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_NEITHER, FILE_ANY_ACCESS)
#define MDR_SERVICE_UNMAP	CTL_CODE(FILE_DEVICE_UNKNOWN, 2, METHOD_NEITHER, FILE_ANY_ACCESS)

#define MDR_STATUS_SUCCESS	1
#define MDR_STATUS_ERROR	0

