// threads.cpp - demonstration of class KSystemThread
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

// This driver is a simple demonstration of system threads using
// class KSystemThread. It creates a device object that starts 
// two system threads. The device object accepts an IOCTL IRP
// which passes an ascii string to the device. Each thread operates
// on the string in succession. See thrddev.cpp for most of the
// implementation.

#define VDW_MAIN
#include <vdw.h>

#include "thrddev.h"
#include "threads.h"

///////////////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(ThreadDriver, NULL)

///////////////////////////////////////////////////////////////////////////////
// DriverEntry
//
// Create the device object and return status.
//
NTSTATUS ThreadDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	ThreadDevice* pDevice;

	bUnloading = FALSE;
	
	pDevice = new (NonPagedPool) ThreadDevice();

	if (pDevice != NULL)
	{
		NTSTATUS status = pDevice->ConstructorStatus();
		if ( !NT_SUCCESS(status) )
			delete pDevice;

		return status;
	}
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}

#pragma code_seg()

// End INIT section code
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Unload
//
// This can only be called when the device is not open 

VOID ThreadDriver::Unload(void)
{
	bUnloading = TRUE;
	KDriver::Unload();
}

