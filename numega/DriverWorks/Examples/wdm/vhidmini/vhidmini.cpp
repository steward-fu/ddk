// VHidMini.cpp - Driver::Works HID minidriver sample
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

// This module implements the driver class.

#define VDW_MAIN
#include <khid.h> // Note: HID minidrivers include this rather than vdw.h

#include "VHidMiniDevice.h"
#include "VHidMini.h"


//////////////////////////////////////////////////////////////////////
// Begin INIT section code
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(VHidMiniDriver, NULL)

// Note: DriverEntry is inherited from KHidDriver and does not need to be over-ridden

// End INIT section code
//////////////////////////////////////////////////////////////////////

#pragma code_seg()

///////////////////////////////////////////////////
// VHidMiniDriver::AddDevice
// Called by hidclass, allows us to initialize our Device extensions.
//
// Creates an instance of the Device class
// Fdo: Pointer to a functional Device object created by hidclass.
//
NTSTATUS VHidMiniDriver::AddDevice(PDEVICE_OBJECT Fdo)
{
	NTSTATUS status;

	VHidMiniDevice* pVDD = new (NonPagedPool) VHidMiniDevice(Fdo);

	if (pVDD == NULL)
		status = STATUS_INSUFFICIENT_RESOURCES;

	else 
	{
		status = pVDD->ConstructorStatus();
		if ( !NT_SUCCESS(status) )
			delete pVDD;
	}

	return status;
}

//////////////////////////////////////////////////
// VHidMiniDriver::Register
// 
// Override of KHidMiniDriver::Register to create a non-polled
// HID minidriver
//
NTSTATUS VHidMiniDriver::Register(BOOLEAN Polled)
{
	return KHidMiniDriver::Register(FALSE);
}

