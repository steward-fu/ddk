// ramdisk.cpp -  RAMDISK example driver
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
//////////////////////////////////////////////////////////////////////////
//
// This is the main module for the RAMDISK example. The
// driver implements two classes of device. The first is 
// a control device, used to interface to a Win32 that
// directs creation and destruction of RAM disks. The
// second class is the RAMDISK device itself. The driver
// is structured to allow only one RAMDISK device at a time.
//
//////////////////////////////////////////////////////////////////////////
//
#define VDW_MAIN

// The following definition is required because the RAMDISK device
// is created dynamically, at the request of the app. Normally, the
// constructor for class KDevice is the INIT section, but this #define
// prevents it from being put there.

#define DEVICES_CONSTRUCTED_AFTER_INIT

#include <vdw.h>

#include "../common.h"
#include "rddevice.h"
#include "rdcntrl.h"
#include "ramdisk.h"

#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(RamDiskDriver, NULL)

//////////////////////////////////////////////////////////////////////////
// DriverEntry
//
NTSTATUS RamDiskDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

	RamdiskControlDevice* ControlDevice;

	// Create an instance of the control device
	ControlDevice = new (NonPagedPool) RamdiskControlDevice();

	// Check initialization
	if ( !ControlDevice )
		status = STATUS_INSUFFICIENT_RESOURCES;
	else
	{
		status = ControlDevice->ConstructorStatus();
		if ( !NT_SUCCESS(status) )
			DeleteDevices();
	}			

	return status;
}

#pragma code_seg()

