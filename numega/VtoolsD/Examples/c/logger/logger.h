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

//	logger.h - Include file for LOGGER VxD example
//
//	Copyright (c) 1994 by Compuware Corporation
//
//		This .H file may be included in other VxDs that use
//		VxD services defined by the LOGGER VxD.  The symbol
//		DEVICE_MAIN is defined only when included in the
//		LOGGER VxD.  Various macros expand differently, causing
//		service tables to be defined in the THUNK VxD, but
//		service call definitions elsewhere.
//
//		Prototypes for VxD and Win32 services are in the DEVICE_MAIN
//

#include <vtoolsc.h>		// Required macros and definitions

#define LOGGER_Major	1			// Major version number
#define LOGGER_Minor	0			// Minor version number		
#define LOGGER_DeviceID	 NUMEGA_TEST_ID		// numega test id
#define LOGGER_Init_Order UNDEFINED_INIT_ORDER	// Default initialization order
#define LOGGER_Refdata	0			// Non-NULL if VxD requires reference data

//----------------------------------------------------------------
//		LOGGER VxD services
//
//	Prototypes

DWORD	LOGGER_GetVersion(void) ;
BOOL	LOGGER_AddEvent(PCHAR pNew, DWORD nBytes) ;
BOOL	LOGGER_AddString(PCHAR String) ;


//	Service Table

Begin_VxD_Service_Table(LOGGER)
	VxD_Service(LOGGER_GetVersion)
	VxD_Service(LOGGER_AddEvent)
	VxD_Service(LOGGER_AddString)
End_VxD_Service_Table
