/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/
/*
 *  IDE.C - PCI IDE Miniport functions
 *
 *  Notes:
 *
 *      This dispatches the PCI IDE services to the chipset-specific function.
 */

#include "pch.h"

ULONG   ulIDEInstance=(ULONG)-1;

UCHAR   bBusIDE=(UCHAR)-1;
UCHAR   bDevFuncIDE=(UCHAR)-1;

/****************************************************************************
 *
 *      PCIMP_IDEInitialize - Initialize miniport
 *
 *      Exported.
 *
 *      ENTRY:  ulInstance is the index of chipset services to use.
 *
 *              bBus is the bus of the ide controller.
 *
 *              bDevFunc is the devfunc of the ide controller.
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_IDEInitialize(ULONG ulInstance, UCHAR bBus, UCHAR bDevFunc)
{
	//
	// Make sure we aren't already running.
	//
	if (ulIDEInstance!=(ULONG)-1) {

		return(PCIMP_ALREADY_INITIALIZED);
	}

	//
	// If instance is too big, we don't support it yet.
	//
	if (ulInstance>=NUM_IDE_INSTANCE) {

		return(PCIMP_INVALID_INSTANCE);
	}

	//
	// Set persistent IDE data.
	//
	ulIDEInstance=ulInstance;
	bBusIDE=bBus;
	bDevFuncIDE=bDevFunc;

	return(PCIMP_SUCCESS);
}

/****************************************************************************
 *
 *      PCIMP_IDEExit - Exit miniport
 *
 *      Exported.
 *
 *      ENTRY:  None.
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_IDEExit(VOID)
{
	//
	// Make sure we are already running.
	//
	if (ulIDEInstance==(ULONG)-1) {

		return(PCIMP_NOT_INITIALIZED);
	}

	//
	// Clear persistent IDE data.
	//
	ulIDEInstance=(ULONG)-1;
	bBusIDE=(UCHAR)-1;
	bDevFuncIDE=(UCHAR)-1;

	return(PCIMP_SUCCESS);
}

/****************************************************************************
 *
 *      PCIMP_GetChannelStatus - Get the enable status of the IDE channels
 *
 *      Exported.
 *
 *      ENTRY:  pulEnabled points to the location to get the enabled
 *              status (PCIMP_IDE flags).
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_GetChannelStatus(PULONG pulStatus)
{
	//
	// Need to initialize the Instance first.
	//
	if (ulIDEInstance == -1) {

		return(PCIMP_NO_INSTANCE);
	}

	*pulStatus=0;
	
	//
	// Call the Instance handler and return appropriate value.
	//      
	return(PCIMP_SUCCESS);
}

