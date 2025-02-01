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
 *  IRQ.C - PCI IRQ Miniport functions
 *
 *  Notes:
 *
 *      This dispatches the PCI IRQ services to the chipset-specific function.
 */

#include "pch.h"

LOCAL_DATA      ULONG   ulIRQInstance=(ULONG)-1;

GLOBAL_DATA     UCHAR   bBusPIC=0;
GLOBAL_DATA     UCHAR   bDevFuncPIC=0;

/****************************************************************************
 *
 *      PCIMP_IRQInitialize - Initialize miniport
 *
 *      Exported.
 *
 *      ENTRY:  ulInstance is the index of chipset services to use.
 *
 *              bBus is the bus of the interrupt controller.
 *
 *              bDevFunc is the devfunc of the interrupt controller.
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_IRQInitialize(ULONG ulInstance, UCHAR bBus, UCHAR bDevFunc)
{
	//
	// Make sure we aren't already running.
	//
	if (ulIRQInstance!=(ULONG)-1) {

		return(PCIMP_ALREADY_INITIALIZED);
	}

	//
	// If instance is too big, we don't support it yet.
	//
	if (ulInstance>=NUM_IRQ_INSTANCE) {

		return(PCIMP_INVALID_INSTANCE);
	}

	//
	// Set persistent IRQ data.
	//
	ulIRQInstance=ulInstance;
	bBusPIC=bBus;
	bDevFuncPIC=bDevFunc;

	return(PCIMP_SUCCESS);
}

/****************************************************************************
 *
 *      PCIMP_IRQExit - Exit miniport
 *
 *      Exported.
 *
 *      ENTRY:  None.
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_IRQExit(VOID)
{
	//
	// Make sure we are already running.
	//
	if (ulIRQInstance==(ULONG)-1) {

		return(PCIMP_NOT_INITIALIZED);
	}

	//
	// Clear persistent IRQ data.
	//
	ulIRQInstance=(ULONG)-1;
	bBusPIC=(UCHAR)-1;
	bDevFuncPIC=(UCHAR)-1;

	return(PCIMP_SUCCESS);
}

/****************************************************************************
 *
 *      PCIMP_SetIRQ - Set a PCI link to a specific IRQ
 *
 *      Exported.
 *
 *      ENTRY:  bIRQNumber is the new IRQ to be used.
 *
 *              bLink is the Link to be set.
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_SetIRQ(UCHAR bIRQNumber, UCHAR bLink)
{
	//
	// Need to initialize the Instance first.
	//
	if (ulIRQInstance == -1) {

		return(PCIMP_NO_INSTANCE);
	}

	//
	// Call the Instance handler and return appropriate value.
	//
	return(PCIMP_SUCCESS);
}

/****************************************************************************
 *
 *      PCIMP_GetIRQ - Get the IRQ of a PCI link.
 *
 *      Exported.
 *
 *      ENTRY:  pbIRQNumber is filled with the current IRQ.
 *
 *              bLink is the Link to be read.
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_GetIRQ(PUCHAR pbIRQNumber, UCHAR bLink)
{
	//
	// Need to initialize the Instance first.
	//
	if (ulIRQInstance == -1) {

		return(PCIMP_NO_INSTANCE);
	}

	//
	// Validate pbIRQNumber.
	//
	if (!pbIRQNumber) {

		return(PCIMP_INVALID_PARAMETER);
	}

	*pbIRQNumber=0;
	
	//
	// Call the Instance handler and return appropriate value..
	//
	return(PCIMP_SUCCESS);
}

/****************************************************************************
 *
 *      PCIMP_SetTrigger - Set the IRQ triggering values.
 *
 *      Exported.
 *
 *      ENTRY:  ulTrigger has bits set for Level triggered IRQs.
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_SetTrigger(ULONG ulTrigger)
{
	//
	// Need to initialize the Instance first.
	//
	if (ulIRQInstance == -1) {

		return(PCIMP_NO_INSTANCE);
	}

	//
	// Can't manipulate IRQ > 15!
	//
	if (ulTrigger & 0xFFFF0000) {

		return(PCIMP_INVALID_PARAMETER);
	}

	//
	// Call the Instance handler and return appropriate value.
	//
	return(PCIMP_SUCCESS);
}

/****************************************************************************
 *
 *      PCIMP_GetTrigger - Get the IRQ triggering values.
 *
 *      Exported.
 *
 *      ENTRY:  pulTrigger will contain trigger settings.
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_GetTrigger(PULONG pulTrigger)
{
	//
	// Need to initialize the Instance first.
	//
	if (ulIRQInstance == -1) {

		return(PCIMP_NO_INSTANCE);
	}

	//
	// Call the Instance handler and return appropriate value.
	//
	return(PCIMP_SUCCESS);
}

/****************************************************************************
 *
 *      PCIMP_ValidateTable - Validate an IRQ table
 *
 *      Exported.
 *
 *      ENTRY:  piihIRQInfoHeader points to an IRQInfoHeader followed
 *              by an IRQ Routing Table.
 *
 *              ulFlags are PCIMP_VALIDATE flags.
 *
 *      EXIT:   Standard PCIMP return value.
 *
 ***************************************************************************/
PCIMPRET CDECL
PCIMP_ValidateTable(PIRQINFOHEADER piihIRQInfoHeader, ULONG ulFlags)
{
	//
	// Need to initialize the Instance first.
	//
	if (ulIRQInstance == -1) {

		return(PCIMP_NO_INSTANCE);
	}

	//
	// Validate piihIRQInfoHeader.
	//
	if (!piihIRQInfoHeader) {

		return(PCIMP_INVALID_PARAMETER);
	}

	//
	// Call the Instance handler and return appropriate value.
	//
	return(PCIMP_SUCCESS);
}

