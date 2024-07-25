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

//	thunk.h - Include file for THUNK VxD example
//
//	Copyright (c) 1994 by Compuware Corporation
//
//	Notes:
//		To use this as a template for your VxD,
//		globally replace "THUNK" with the name of your device.
//
//		This .H file may be included in other VxDs that use
//		VxD services defined by the THUNK VxD.  The symbol
//		DEVICE_MAIN is defined only when included in the
//		THUNK VxD.  Various macros expand differently, causing
//		service tables to be defined in the THUNK VxD, but
//		service call definitions elsewhere.
//
//		Prototypes for VxD and Win32 services are in the DEVICE_MAIN
//

#include <vtoolsc.h>		// Required macros and definitions

#define THUNK_Major	1		      // Major version number
#define THUNK_Minor	0		      // Minor version number		
#define THUNK_DeviceID UNDEFINED_DEVICE_ID    // Service provider must have ID
#define THUNK_Init_Order UNDEFINED_INIT_ORDER // Default initialization order
