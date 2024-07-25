// EzUsbDriver.cpp - Implementation of EzUsbDriver driver class
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1999 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================


#define VDW_MAIN

#include <vdw.h>
#pragma warning (default:4710)
#include <kusb.h>
#include <KUsbBusIntf.h>

#include "EzUsbDriver.h"
#include "EzUsbFirmware.h"
#include "EzUsbDeviceIoctl.h"
#include "EzUsbIsoTransfer.h"
#include "EzUsbIntTransfer.h"
#include "EzUsbDevice.h"

#pragma hdrstop("EzUsbDriver.pch")

KUstring us(L"EzUsb");

// Global driver trace object
KDebugOnlyTrace t("EzUsb", TRACE_MONITOR | TRACE_DEBUGGER, TraceAlways, BreakNever, us);

//=============================================================================
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(EzUsbDriver, NULL)

//=============================================================================
// EzUsbDriver::DriverEntry
//
//		This is the first entry point called by the system when the
//		driver is loaded.
// 
NTSTATUS EzUsbDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	t << "In DriverEntry\n";

    // Set default pool tag for all 'new' allocations to "New ".
    // Use NuMega DriverWorkbench to view the memory pool
    SetPoolTag(' weN');

	m_Unit = 0;

	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(RegistryPath);
}

// End INIT section
//=============================================================================
#pragma code_seg()

//=============================================================================
// EzUsbDriver::AddDevice
//
//		Called when the system detects a device for which this
//		driver is responsible.  This function creates the Functional Device 
//		Object, or FDO. The FDO enables this driver to handle requests for 
//		the physical device. 
//
NTSTATUS EzUsbDriver::AddDevice(PDEVICE_OBJECT Pdo)
{
	t << "AddDevice called\n";

    // Create the device object
	EzUsbDevice * pDevice = new (NonPagedPool) EzUsbDevice(Pdo, m_Unit);

	if( NULL == pDevice )
	{
		t << "Error creating device EzUsbDevice" << (ULONG) m_Unit << "\n";
	    return STATUS_INSUFFICIENT_RESOURCES;
	}

	NTSTATUS status = pDevice->ConstructorStatus();

	if( !NT_SUCCESS(status) )
	{
		t << "Error constructing device EzUsbDevice" << (ULONG) m_Unit 
		  << " status " << (ULONG) status << "\n";
		delete pDevice;
	}
	else
	{
		pDevice->ReportNewDevicePowerState( PowerDeviceD0 );
		m_Unit++;
	}

	return status;
}

//=============================================================================
// EzUsbDriver::Unload
//
//		Called when the driver is unloaded.
//
VOID EzUsbDriver::Unload(void)
{
	t << "Unload called\n";
}
