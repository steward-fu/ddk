// usbfilt.cpp - USB filter driver example
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
#include <kusb.h>

#include "usbfdev.h"
#include "usbfilt.h"

KUstring us(L"UsbFilter");

// Global driver trace object
KDebugOnlyTrace T("UsbFilter", TRACE_MONITOR | TRACE_DEBUGGER, TraceAlways, BreakNever, us);

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(UsbFilterDriver, NULL)

/////////////////////////////////////////////////////////////////////
// Driver Entry
//
NTSTATUS UsbFilterDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	T << "UsbFilterDriver::DriverEntry\n";

	m_Unit = 0;
	return STATUS_SUCCESS;

	// The following macro simply allows compilation at Warning Level 4
	// If you reference this parameter in the function simply remove the macro.
	UNREFERENCED_PARAMETER(RegistryPath);
}

// End INIT section
/////////////////////////////////////////////////////////////////////
#pragma code_seg()

/////////////////////////////////////////////////////////////////////
// Add Device
//
NTSTATUS UsbFilterDriver::AddDevice(PDEVICE_OBJECT Pdo)
{
	T << "UsbFilterDriver::AddDevice\n";

	//Create an instance of our UsbFilterDevice class.

	// NOTE: The following parameters to placement new  
	// may have to be changed for a particular USB filter driver.
	// For an upper filter, it is important to match the DeviceType, 
	// Characteristics, and DeviceFlags of the function 
	// driver DEVICE_OBJECT. 
	UsbFilterDevice * pFilterDevice = new (
			static_cast<PCWSTR>(NULL),
			FILE_DEVICE_UNKNOWN,
			static_cast<PCWSTR>(NULL),
			0,
			DO_DIRECT_IO
			)
		UsbFilterDevice(Pdo, m_Unit);

	if (pFilterDevice)
	{
		NTSTATUS status = pFilterDevice->ConstructorStatus();

		if ( !NT_SUCCESS(status) )
		{
			T << "Failed to construct UsbFilterDevice"
			  << (ULONG) m_Unit 
			  << " status = "
			  << status
			  << "\n";

			delete pFilterDevice;
		}
		else
		{
			m_Unit++;
		}

		return status;
	}
	else
	{
		T << "Failed to allocate UsbFilterDevice"
		  << (ULONG) m_Unit 
		  << "\n";

		return STATUS_INSUFFICIENT_RESOURCES;
	}
}
