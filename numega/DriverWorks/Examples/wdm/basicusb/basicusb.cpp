// basicusb.cpp - a simple USB driver example
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
#include <vdw.h>		// Base Driver::Works includes
#include <kusb.h>		// Additional USB definitions

#include "busbdev.h"
#include "basicusb.h"

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(BasicUsbDriver, NULL)

/////////////////////////////////////////////////////////////////////
// Driver Entry
// 
// This is the first entry point called by the system when the driver
// is loaded.

NTSTATUS BasicUsbDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	m_Unit = 0;
	return STATUS_SUCCESS;
}

#pragma code_seg()
// End INIT section
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// Add Device
//
// The system calls this entry point when it detects a device for which
// this driver is responsible.
//
// Input
//   Pdo      Physical Device Object. This is a pointer to a system device
//            object that represents the physical device.
//
// This function creates the Functional Device Object, or FDO. The FDO
// enables this driver to handle requests for the physical device. 

NTSTATUS BasicUsbDriver::AddDevice(PDEVICE_OBJECT Pdo)
{
	BasicUsbDevice* pDevice = 
		new (NonPagedPool) BasicUsbDevice(Pdo, m_Unit);

	m_Unit++;

	if (pDevice)
	{
		NTSTATUS status = pDevice->ConstructorStatus();

		if ( !NT_SUCCESS(status) )
			delete pDevice;

		return status;
	}
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}
