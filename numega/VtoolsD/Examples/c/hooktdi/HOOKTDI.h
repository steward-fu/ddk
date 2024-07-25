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

// HOOKTDI.h - include file for VxD HOOKTDI

#define   USE_NDIS
#include <vtoolsc.h>

#define HOOKTDI_Major		1
#define HOOKTDI_Minor		0
#define HOOKTDI_DeviceID		UNDEFINED_DEVICE_ID
#define HOOKTDI_Init_Order	VTDI_Init_Order+1


