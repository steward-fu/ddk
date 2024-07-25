// Example code for class KController
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
#include "ctrldev.h"
#include "controlr.h"
#include "common.h"

// This is a demonstration of class KController. The driver creates 
// a set of device objects of the same class. The idea is that some
// types of requests to these devices require serialization through
// a controller. Multiple instances of the test app randomly generate 
// IRPs to the devices. See the device class implementation for
// more information.

//////////////////////////////////////////////////////////////////////
// Begin INIT time code
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(ControllerTestDriver, NULL)

//////////////////////////////////////////////////////////////////////
// DriverEntry
//
NTSTATUS ControllerTestDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG i;

	// create the controller object

	m_Controller = new (NonPagedPool) KController;
	if ( !m_Controller )
		return STATUS_INSUFFICIENT_RESOURCES;

	if ( (PCONTROLLER_OBJECT)*m_Controller == NULL )
	{
		// failed to initialize controller
		return status;
	}

	DbgPrint("CTRLTEST: Creating %d units\n", NUNITS);

	// create the device objects

	for (i = 0; i < NUNITS; i++)
	{
		if ( ! new (NonPagedPool) ControllerTestDevice(i, *m_Controller) )
		{
			delete m_Controller;
			DeleteDevices();
			return status;
		}
	}

	return STATUS_SUCCESS;
}

#pragma code_seg()
// End INIT time code
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Unload
//
VOID ControllerTestDriver::Unload(void)
{
	delete m_Controller;
	DeleteDevices();
}
