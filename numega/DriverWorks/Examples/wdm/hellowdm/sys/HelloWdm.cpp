// HelloWdm.cpp - main module for simple WDM driver
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
// WDM kernel mode driver written with Driver::Works. 

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

#include "HelloDev.h"	// our device class
#include "HelloWdm.h"	// our driver class

// Declare a trace object, used for debugging
KDebugOnlyTrace T("HelloWdm");

///////////////////////////////////////////////////////////////////
// Begin INIT time code
//
// Code in section INIT is discarded after the driver initializes
#pragma code_seg("INIT")

// Macro DECLARE_DRIVER_CLASS tells the framework which class serves as 
// the driver class for this driver. The macro declares a function that
// the framework calls when the driver is loaded. The function is responsible
// for creating an instance of the driver class.

DECLARE_DRIVER_CLASS(SimpleWdmDriver, NULL)

///////////////////////////////////////////////////////////////////
// DriverEntry
//
// This is the initialization routine for this driver. All it does is create
// a single device object. More sophisticated drivers need to check parameters
// in the registry or query the system for installed devices in order to
// control the creation of device objects. Drivers that use hardware resources
// such as I/O ports, memory ranges, DMA channels, or IRQs must report usage
// to the system during initialization.

NTSTATUS SimpleWdmDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	T << TraceInfo << "Entered DriverEntry for simple WDM driver\n";
	T << "The Registry Path is " << RegistryPath->Buffer << "\n";

	m_Instance = 0;

	// TODO: Implement driver specific initialization here

	return STATUS_SUCCESS;
}
// End INIT time code
#pragma code_seg()
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// AddDevice
//
// The system calls this when it detects each instance of the
// device that this driver supports.
//
// The parameter (PDO) to this routine identifies the Physical
// Device Object created by the system for the instance of
// device.
//
NTSTATUS SimpleWdmDriver::AddDevice(PDEVICE_OBJECT PDO)
{
	NTSTATUS status;

	// Create a new instance of the device

	SimpleWdmDevice* pD = new (NonPagedPool) SimpleWdmDevice(PDO, m_Instance);

	if (pD != NULL)
	{
		status = pD->ConstructorStatus();
		if ( !NT_SUCCESS(status) )
			delete pD;
	}
	else
		status = STATUS_INSUFFICIENT_RESOURCES;

	if ( NT_SUCCESS(status) )
	{
		m_Instance++;
		pD->ReportNewDevicePowerState(PowerDeviceD0);
	}

	return status;
}

///////////////////////////////////////////////////////////////////
// Unload
//
//
VOID SimpleWdmDriver::Unload(void)
{
	T << TraceInfo << "Unloading simple WDM driver\n";

	// TODO: implmenent driver specific unload operations here

	// call the base class handler
	KDriver::Unload();

}
