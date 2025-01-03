// USBNIC.cpp - source file generated by NetDrvWizard. Contains
//				  KNdisMiniDriver-derived implementation
//====================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2002 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//====================================================================

#include "USBNICDrv.h"
#include "USBNICAdap.h"

///////////////////////////////////////////////////////////////////
// Begin INIT time code
//
// Code in section INIT is discarded after the driver initializes
//#pragma code_seg("INIT")


// The following macro declares the driver class:

DECLARE_MINIDRIVER_CLASS(USBNICDriver)

//////////////////////////////////////////////////////////////////////
// USBNICDriver	Implementation

/////////////////////////////////////////////////////////////////////
// USBNICDriver::DriverEntry
//
// This method registers adapter characteristics (a handler pointer table)
// with NDIS. Usually that's all this function has to do.
//
// Returns:
//	Status code of registration. Usually, it's NDIS_STATUS_SUCCESS.
//  Returning any other code prevents the driver from being loaded.
NTSTATUS USBNICDriver::DriverEntry(IN PVOID)
{
	TRACE("USBNICDriver::DriverEntry\n");

	KNDIS_MINIPORT_CHARACTERISTICS<USBNICAdapter> Chars;
	return	Chars.Register(*this);	
}

 