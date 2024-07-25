// vhidmou.cpp - Driver::Works HID minidriver sample
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

// The Virtual HID Mouse is a HID minidriver that supplies 
// mouse input reports to HIDCLASS, without requiring any
// actual hardware. Instead. the driver accepts input from
// other drivers that call two exported entry points:
//
// 	VOID __stdcall MoveVirtualMouse(CHAR deltaX, CHAR deltaY)
//  VOID __stdcall ClickVirtualMouse(ULONG LeftOrRight, ULONG DownOrUp)
//

// This module implements the driver class. The device class in
// in module vmoudev.cpp.

#define VDW_MAIN
#include <khid.h> // Note: HID minidrivers include this rather than vdw.h

#include "vhidmou.h"	// the device class
#include "vmoudev.h"	// the driver class

//////////////////////////////////////////////////////////////////////
// Begin INIT section code
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(VirtualHidMouseDriver, NULL)

// Note: DriverEntry is inherited from KHidDriver

// End INIT section code
//////////////////////////////////////////////////////////////////////

#pragma code_seg()

// AddDevice
// Called by hidclass, allows us to initialize our device extensions.
//
// Creates an instance of the device class
// Fdo: Pointer to a functional device object created by hidclass.
//
NTSTATUS VirtualHidMouseDriver::AddDevice(PDEVICE_OBJECT Fdo)
{
	NTSTATUS status;

	VirtualHidMouse* p = new (NonPagedPool) VirtualHidMouse(Fdo);

	if (p == NULL)
		status = STATUS_INSUFFICIENT_RESOURCES;

	else 
	{
		status = p->ConstructorStatus();
		if ( !NT_SUCCESS(status) )
			delete p;
	}

	return status;
}

