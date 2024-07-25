// kbfilter.cpp - Keyboard Filter Driver
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
// This example implements a simple keyboard filter, demonstrating the
// usage of class KFilterDevice.

// The driver creates two device objects. The first is a control device. It provides
// the interface to the test application. When requested by the test app, the
// control device creates an instance of the keyboard filter device. The keyboard
// filter device intercepts keyboard data being passed from the keyboard class
// driver to the Win32 subsystem. It copies this data to the output FIFO of the
// control device. When the test app reads from the control device, the control
// device copies the keyboard data from its FIFO to the IRP.

// This #define instruct vdw.h to include source for KDriver and KDevice
#define VDW_MAIN

// This #define is required so that the constructor for KDevice is not put in
// the INIT section. This is necessary because the control device creates instances
// of the filter device after initialization.

#define DEVICES_CONSTRUCTED_AFTER_INIT

// Driver::Works include files
#include <vdw.h>

// Include files for this driver
#include "common.h"
#include "keyfifo.h"
#include "kbfltdev.h"
#include "control.h"
#include "kbfilter.h"

/////////////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

/////////////////////////////////////////////////////////////////////////////
// The following macro enables Driver::Works to instance the driver class

DECLARE_DRIVER_CLASS(KeyboardFilterDriver, NULL)

/////////////////////////////////////////////////////////////////////////////
// DriverEntry
//
// Entry point for driver initialization. Just create an instance of the
// control device.
//
NTSTATUS KeyboardFilterDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	PUNICODE_STRING DriverKeyPath = KDriver::DriverInstance()->RegistryPath();
	KRegistryKey RegKey(DriverKeyPath);
	ULONG bHookOnLoad=FALSE;
	RegKey.QueryValue(L"Start", &bHookOnLoad);
	if(bHookOnLoad >= 2)
		bHookOnLoad = FALSE;
	else 
		bHookOnLoad = TRUE;

	m_ControlDevice = new (NonPagedPool) FilterControlDevice(BOOLEAN(bHookOnLoad));

	if (m_ControlDevice) 
	{
		NTSTATUS status = m_ControlDevice->ConstructorStatus();
		if ( !NT_SUCCESS(status) )
			delete m_ControlDevice;
		return status;
	}
	else
		return STATUS_INSUFFICIENT_RESOURCES;

}

//
// End INIT section code
/////////////////////////////////////////////////////////////////////////////
#pragma code_seg()

////////////////////////////////////////////////////////////////////////
// Unload
VOID KeyboardFilterDriver::Unload(void)
{
	m_ControlDevice->_self_delete();
	KDriver::Unload();
}


