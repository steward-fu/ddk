//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//////////////////////////////////////////////////////////////////////
// nmne2k.cpp - source file generated by NetDrvWizard. Contains
//				  KNdisMiniDriver-derived implementation
//				  

#include "NMNE2KDrv.h"
#include "NMNE2KAdap.h"

///////////////////////////////////////////////////////////////////
// Begin INIT time code
//
// Code in section INIT is discarded after the driver initializes
//#pragma code_seg("INIT")

// The wizard inserts the following macro (declares the driver class)

DECLARE_MINIDRIVER_CLASS(NMNE2KDriver)

//////////////////////////////////////////////////////////////////////
// NMNE2KDriver	Implementation

/////////////////////////////////////////////////////////////////////
// NMNE2KDriver::DriverEntry
//
// This method registers adapter characteristics (a handler pointer table)
// with NDIS. Usually that's all this function has to do.
//
// Returns:
//	Status code of registration. Usually, it's NDIS_STATUS_SUCCESS.
//  Returning any other code prevents the driver from being loaded.
NTSTATUS NMNE2KDriver::DriverEntry(IN PVOID)
{
	TRACE("NMNE2KDriver::DriverEntry\n");

	KNDIS_MINIPORT_CHARACTERISTICS<NMNE2KAdapter> Chars;
	return	Chars.Register(*this);	
}
 