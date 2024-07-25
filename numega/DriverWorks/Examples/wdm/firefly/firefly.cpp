// Firefly.cpp
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2003 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#define VDW_MAIN

#include <vdw.h>
#include "Firefly.h"
#include "FireflyDevice.h"

#pragma hdrstop("Firefly.pch")

// Set a default 32-bit tag value to be stored with each heap block
// allocated by operator new. Use BoundsChecker to view the memory pool.
// This value can be overridden using the global function SetPoolTag().
POOLTAG DefaultPoolTag('eriF');

// Create the global driver trace object
KDebugOnlyTrace t("Firefly");

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(Firefly, NULL)


/////////////////////////////////////////////////////////////////////
//  Firefly::DriverEntry
//
//	Routine Description:
//		This is the first entry point called by the system when the
//		driver is loaded.
// 
//	Parameters:
//		RegistryPath - String used to find driver parameters in the
//			registry.  To locate Firefly look for:
//			HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Firefly
//
//	Return Value:
//		NTSTATUS - Return STATUS_SUCCESS if no errors are encountered.
//			Any other indicates to the system that an error has occured.
//
//	Comments:
//
NTSTATUS Firefly::DriverEntry(PUNICODE_STRING RegistryPath)
{
	t << "In DriverEntry Compiled at " __TIME__ " on " __DATE__ "\n";

	// The following macro simply allows compilation at Warning Level 4
	// If you reference this parameter in the function simply remove the macro.
	UNREFERENCED_PARAMETER(RegistryPath);

	m_Unit = 0;
	return STATUS_SUCCESS;
}

// End INIT section
/////////////////////////////////////////////////////////////////////
#pragma code_seg()

/////////////////////////////////////////////////////////////////////
//  Firefly::AddDevice
//
//	Routine Description:
//		Called when the system detects a device for which this
//		driver is responsible.
//
//	Parameters:
//		Pdo - Physical Device Object. This is a pointer to a system device
//			object that represents the physical device.
//
//	Return Value:
//		NTSTATUS - Success or failure code.
//
//	Comments:
//		This function creates the Functional Device Object, or FDO. The FDO
//		enables this driver to handle requests for the physical device. 
//
NTSTATUS Firefly::AddDevice(PDEVICE_OBJECT Pdo)
{
	t << "AddDevice called\n";

    // Create the device object. Note that we used a form of "placement" new,
	// that is a member operator of KDevice.  This form will use storage
	// allocated by the system in the device object's device to store our
	// class instance.
	FireflyDevice * pDevice = new (
			NULL,
			FILE_DEVICE_UNKNOWN,
			NULL,
			0,
			DO_DIRECT_IO
				| DO_POWER_PAGABLE
			)
		FireflyDevice(Pdo, m_Unit);

	if (pDevice == NULL)
	{
		t << "Error creating device FireflyDevice"
			   << (ULONG) m_Unit << EOL;
	    return STATUS_INSUFFICIENT_RESOURCES;
	}

	NTSTATUS status = pDevice->ConstructorStatus();

	if ( !NT_SUCCESS(status) )
	{
		t << "Error constructing device FireflyDevice"
		  << (ULONG) m_Unit << " status " << (ULONG) status << EOL;
		delete pDevice;
	}
	else
	{
		m_Unit++;

		pDevice->ReportNewDevicePowerState(PowerDeviceD0);
	}

	return status;
}
