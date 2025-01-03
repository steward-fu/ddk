// Ping.cpp
//
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
// Generated by DriverWizard version DriverStudio 2.0.0 (Build 552)
// Requires Compuware's DriverWorks classes
//

#define VDW_MAIN
#include <vdw.h>
#include <tdiclient.h>

#include "PingDevice.h"

#include "tdipnp.h"

#include "Ping.h"


#pragma hdrstop("Ping.pch")

// Generated by DriverWizard version DriverStudio 2.0.0 (Build 552)

// Set a default 32-bit tag value to be stored with each heap block
// allocated by operator new. Use BoundsChecker to view the memory pool.
// This value can be overridden using the global function SetPoolTag().
POOLTAG DefaultPoolTag('gniP');

// Create the global driver trace object
// TODO:	Use KDebugOnlyTrace if you want trace messages
//			to appear only in debug builds.  Use KTrace if
//			you want trace messages to always appear.	
KTrace t("Ping");

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(Ping, NULL)

/////////////////////////////////////////////////////////////////////
//  Ping::DriverEntry
//
//	Routine Description:
//		This is the first entry point called by the system when the
//		driver is loaded.
// 
//	Parameters:
//		RegistryPath - String used to find driver parameters in the
//			registry.  To locate Ping look for:
//			HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Ping
//
//	Return Value:
//		NTSTATUS - Return STATUS_SUCCESS if no errors are encountered.
//			Any other indicates to the system that an error has occured.
//
//	Comments:
//

NTSTATUS Ping::DriverEntry(PUNICODE_STRING RegistryPath)
{
	t << "In DriverEntry\n";


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
	m_Unit = 0;

#ifdef TDI_WINDOWS_NT
	m_pRegistryPath = RegistryPath;
#endif

	return STATUS_SUCCESS;
}


/////////////////////////////////////////////////////////////////////
//  Ping::LoadRegistryParameters
//
//	Routine Description:
//		Load driver data members from the registry.
// 
//	Parameters:
//		Params - Open registry key pointing to "Parameters"
//
//	Return Value:
//		None
//			
//	Comments:
//		Member variables are updated with values read from registry.
//
//		The parameters are found as values under the "Parameters" key,	
//		HKLM\SYSTEM\CurrentControlSet\Services\Ping\Parameters\...
//

void Ping::LoadRegistryParameters(KRegistryKey &Params)
{

	m_bBreakOnEntry = FALSE;
	Params.QueryValue(L"BreakOnEntry", &m_bBreakOnEntry);
	t << "m_bBreakOnEntry loaded from registry, resulting value: [" << m_bBreakOnEntry << "]\n";

}
// End INIT section
/////////////////////////////////////////////////////////////////////
#pragma code_seg()

/////////////////////////////////////////////////////////////////////
//  Ping::AddDevice
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

NTSTATUS Ping::AddDevice(PDEVICE_OBJECT Pdo)
{
	t << "AddDevice called\n";

    // Create the device object. Note that we used a form of "placement" new,
	// that is a member operator of KDevice.  This form will use storage
	// allocated by the system in the device object's device to store our
	// class instance.
	PingDevice * pDevice = new (
			static_cast<PCWSTR>(KUnitizedName(L"PingDevice", m_Unit)),
			FILE_DEVICE_UNKNOWN,
			NULL,
			0,
			DO_DIRECT_IO
				| DO_POWER_PAGABLE
			)
		PingDevice(Pdo, m_Unit);

	if (pDevice == NULL)
	{
		t << "Error creating device PingDevice"
			   << (ULONG) m_Unit << EOL;
	    return STATUS_INSUFFICIENT_RESOURCES;
	}

	NTSTATUS status = pDevice->ConstructorStatus();

	if ( !NT_SUCCESS(status) )
	{
		t << "Error constructing device PingDevice"
		  << (ULONG) m_Unit << " status " << (ULONG) status << EOL;
		delete pDevice;
	}
	else
	{
		m_Unit++;

		pDevice->ReportNewDevicePowerState(PowerDeviceD0);
	
#ifdef TDI_WINDOWS_NT
			m_TdiCallbacks.SetDeviceObject(pDevice);
			m_TdiCallbacks.Initialize(m_pRegistryPath, TDI_PNP_HOOK_BINDINGS);
#endif

	}

	return status;
}
