// MouseFilter.cpp
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
#include <kwdmfltr.cpp>
#include "MouseFilter.h"
#include "MouseFilterDevice.h"

#pragma hdrstop("MouseFilter.pch")

// Create the global driver trace object
KDebugOnlyTrace t("MouseFilter");

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(MouseFilter, NULL)

/////////////////////////////////////////////////////////////////////
// MouseFilter::Driver Entry
//
//		This is the first entry point called by the system when the
//		driver is loaded.
//
// Input
//		RegistryPath	String used to find driver parameters in the
//						registry.  To locate MouseFilter look for:
//						HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\MouFilt
//
// Output
//		NTSTATUS		Return STATUS_SUCCESS if no errors are
//						encountered.  Any other indicates to
//						the system that an error has occured.
//
// Notes
//

NTSTATUS MouseFilter::DriverEntry(PUNICODE_STRING RegistryPath)
{
	t << "In DriverEntry\n";

	m_Unit = 0;

	// Set default pool tag for all 'new' allocations to "New ".
	// Under the checked build, use the DDK POOLMON utility
	// to view the memory pool
	SetPoolTag(' dfM');

	// The following macro simply allows compilation at Warning Level 4
	// If you reference this parameter in the function simply remove the macro.
	UNREFERENCED_PARAMETER(RegistryPath);

	return STATUS_SUCCESS;
}

// End INIT section
/////////////////////////////////////////////////////////////////////
#pragma code_seg()

/////////////////////////////////////////////////////////////////////
// AddDevice (member of MouseFilter)
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

NTSTATUS MouseFilter::AddDevice(PDEVICE_OBJECT Pdo)
{
	t << "AddDevice called\n";

	// Create the device object. Note that we used a form of "placement" new,
	// that is a member operator of KDevice.  This form will use storage
	// allocated by the system in the device object's device to store our
	// class instance.
	MouseFilterDevice * pDevice = new (
			static_cast<PCWSTR>(KUnitizedName(L"mfDevice", m_Unit)),
			FILE_DEVICE_MOUSE,
			NULL,
			0,
			0  //Don't specify the flags now.  They will be set
			   //when the filter is attached.
			)
		MouseFilterDevice(Pdo, m_Unit);

	if (pDevice == NULL)
	{
		t << "Error creating device MouseFilterDevice"
			   << (ULONG) m_Unit << EOL;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	NTSTATUS status = pDevice->ConstructorStatus();

	if ( !NT_SUCCESS(status) )
	{
		t << "Error constructing device MouseFilterDevice"
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

