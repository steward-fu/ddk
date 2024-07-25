// Drvmngq.cpp
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

#define VDW_MAIN
#include <vdw.h>

#include "function.h"
#include "Drvmngq.h"
#include "DrvmngqDevice.h"


#pragma hdrstop("Drvmngq.pch")

// Create the global driver trace object
KTrace t("Drvmngq");

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(Drvmngq, NULL)

/////////////////////////////////////////////////////////////////////
// Driver Entry (member of Drvmngq)
// 
// This routine is called when the driver is loaded.
//
// Input
//		RegistryPath Registry path for the driver
//
// Output
//		NTSTATUS	Success or failure code.
//			
// Notes
//
//
NTSTATUS Drvmngq::DriverEntry(PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;			// Status of device creation

	t << "In DriverEntry\n";

	// Set default pool tag for all 'new' allocations to "New ".
	// Under the checked build, use the DDK POOLMON utility
	// to view the memory pool
	SetPoolTag(' weN');

	// Open the "Parameters" key under the driver
	KRegistryKey Params(RegistryPath, L"Parameters");
	if ( NT_SUCCESS(Params.LastError()) )
	{
#if DBG
		ULONG bBreakOnEntry = FALSE;
			// Read "BreakOnEntry" value from registry
		Params.QueryValue(L"BreakOnEntry", &bBreakOnEntry);
			// If requested, break into debugger
		if (bBreakOnEntry) DbgBreakPoint();

#endif
		// Load driver data members from the registry
		LoadRegistryParameters(Params);
	}

#if !_WDM_ // THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS AN NT4 DRIVER

	// Create DrvmngqDevice
	DrvmngqDevice* pDrvmngqDevice;
	pDrvmngqDevice = new (
			static_cast<PCWSTR>(KUnitizedName(L"DrvmngqDevice", 0)), 
			FILE_DEVICE_UNKNOWN,
			static_cast<PCWSTR>(KUnitizedName(L"DrvmngqDevice", 0)),
			0,
			DO_DIRECT_IO
			)
		DrvmngqDevice();
	if (pDrvmngqDevice == NULL)
	{
		t << "Error constructing device DrvmngqDevice" << EOL;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	status = pDrvmngqDevice->ConstructorStatus();
	if (!NT_SUCCESS(status))
	{
		// Error returned from a constructor
		t << "Error creating device DrvmngqDevice, status " << (ULONG) status << EOL;
		delete pDrvmngqDevice;
		return status;
	}

#else // !_WDM_ THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS A WDM DRIVER

	m_Unit = 0;

#endif // !_WDM_

	return status;
}

/////////////////////////////////////////////////////////////////////
// Load Registry Parameters (member of Drvmngq)
//
// Load driver data members from the registry.
// The parameters are found as values under the "Parameters" key,	
// HKLM\SYSTEM\CurrentControlSet\Services\Drvmngq\Parameters\...
// 
// Input
//		Params		Open registry key pointing to "Parameters"
//
// Output
//		none		(Data members are loaded)
//			
// Notes
//
void Drvmngq::LoadRegistryParameters(KRegistryKey &Params)
{

	m_bBreakOnEntry = FALSE;
	Params.QueryValue(L"BreakOnEntry", &m_bBreakOnEntry);
	t << "m_bBreakOnEntry loaded from registry, resulting value: [" << m_bBreakOnEntry << "]\n";

}

#pragma code_seg()

#if _WDM_ // THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS A WDM DRIVER

/////////////////////////////////////////////////////////////////////
// AddDevice (member of Drvmngq)
//
//		Called when the system detects a device for which this
//		driver is responsible.
//
// Input
//		Pdo			Physical Device Object. This is a pointer to a system device
//					object that represents the physical device.
// Output
//		NTSTATUS	Success or failure code.
//
// Notes
//		This function creates the Functional Device Object, or FDO. The FDO
//		enables this driver to handle requests for the physical device. 
//
NTSTATUS Drvmngq::AddDevice(PDEVICE_OBJECT Pdo)
{
	t << "AddDevice called\n";

    // Create the device object
	DrvmngqDevice * pDevice = new (
			static_cast<PCWSTR>(KUnitizedName(L"DrvmngqDevice", m_Unit)), 
			FILE_DEVICE_UNKNOWN,
			static_cast<PCWSTR>(KUnitizedName(L"DrvmngqDevice", m_Unit))
			)
		DrvmngqDevice(Pdo, m_Unit);

	if (pDevice == NULL)
	{
		t << "Error creating device DrvmngqDevice"
			   << (ULONG) m_Unit << EOL;
	    return STATUS_INSUFFICIENT_RESOURCES;
	}

	NTSTATUS status = pDevice->ConstructorStatus();

	if ( !NT_SUCCESS(status) )
	{
		t << "Error constructing device DrvmngqDevice"
		  << (ULONG) m_Unit << " status " << (ULONG) status << EOL;
		delete pDevice;
	}
	else
	{
		m_Unit++;
	}

	return status;
}

#else // _WDM_ THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS AN NT4 DRIVER

/////////////////////////////////////////////////////////////////////
// Unload (member of Drvmngq)
//
//	Unload is responsible for releasing any system objects that
//	the driver has allocated. 
//
//	In general, this function must comprehensively ensure that
//	the driver is not unloaded while holding system objects,
//	including memory, or while there are pending events that
//	would cause the system to call the driver. This is best done
//	by deconstructing top level objects, which in turn release
//	objects for which they are responsible.
//
// Input
//		none
//
// Output
//		none
//
// Notes
//		This function is called at PASSIVE_LEVEL.
//
VOID Drvmngq::Unload(VOID)
{
	t << "Unload called\n";

    // Call base class to delete all devices.
	KDriver::Unload();
}

#endif // _WDM_ 
