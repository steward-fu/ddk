// The most simple driver - HELLO
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

// This example is intended to show the most basic aspects of a 
// kernel mode driver written with Driver::Works. 

// VDW_MAIN
//
// This symbol controls the inclusion of certain files by vdw.h. It must
// appear in exactly one source module of a driver, and must precede the
// #include <vdw.h> line. 

#define VDW_MAIN	

// vdw.h
//
// Vdw.h is the main include file for Driver::Works. It includes all the 
// other include files for the class library. It also includes function.h,
// which is specific to the driver being built.  When VDW_MAIN is defined, 
// vdw.h pulls in source files for the libraries device and driver classes.

#include <vdw.h>

// The next lines include the definitions for this driver's driver class
// and device class.

#include "hellodev.h"
#include "hello.h"

// Declare a trace object, used for debugging
KDebugOnlyTrace Tracer("Hello");

///////////////////////////////////////////////////////////////////
// Begin INIT time code
//
// Code in section INIT is discarded after the driver initializes
#pragma code_seg("INIT")

// Macro DECLARE_DRIVER_CLASS tells the framework which class serves as 
// the driver class for this driver. The macro declares a function that
// the framework calls when the driver is loaded. The function is responsible
// for creating an instance of the driver class.

DECLARE_DRIVER_CLASS(MostSimpleDriver, NULL)

// DriverEntry
//
// This is the initialization routine for this driver. All it does is create
// a single device object. More sophisticated drivers need to check parameters
// in the registry or query the system for installed devices in order to
// control the creation of device objects. Drivers that use hardware resources
// such as I/O ports, memory ranges, DMA channels, or IRQs must report usage
// to the system during initialization.

NTSTATUS MostSimpleDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	MostSimpleDevice* pDevice;

	Tracer << TraceInfo << "This is the most simple driver\n";
	Tracer << "The Registry Path is " << RegistryPath->Buffer << "\n";

	// create a device
	pDevice = new (NonPagedPool) MostSimpleDevice();

	// return the status
	if (pDevice)
	{
		NTSTATUS status = pDevice->ConstructorStatus();
		if (!NT_SUCCESS(status))
			delete pDevice;
		return status;
	}
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}
// End INIT time code
#pragma code_seg()
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
// Unload
//
// The system calls this routine when the driver is unloaded. It must
// clean up anything that the driver created, including devices.
//
VOID MostSimpleDriver::Unload(void)
{
	Tracer << TraceInfo << "Unloading HELLO driver\n";
	DeleteDevices();
}
