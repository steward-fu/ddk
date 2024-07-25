// mapmem.cpp -  example driver
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

// This driver maps a range of physical memory on the ISA bus.
// The address and size of the range have default values, but
// can be taken from the Parameters subkey of the driver's
// registry key at initialization. 
//
// The driver does not map the memory to kernel space. Rather,
// it waits until a process sends a DeviceIoControl call, and
// then it maps the address into that process. Later, the process
// calls back to undo the mapping.

#define VDW_MAIN
#include <vdw.h>

#include "mmdevice.h"
#include "mapmem.h"

#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(MapMemDriver, NULL)

//////////////////////////////////////////////////////////////////////////
// DriverEntry
//
// Retrieve parameters and create device object
//
NTSTATUS MapMemDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	MapMemDevice* pDevice;

	ULONG DeviceMemoryAddress = DEFAULT_DEVMEMORY_ADDRESS;
	ULONG DeviceMemorySize = DEFAULT_DEVMEMORY_SIZE;

	KRegistryKey Params(RegistryPath, L"Parameters");
	if ( NT_SUCCESS (Params.LastError()) )
	{
		Params.QueryValue(L"DEVICE_ADDRESS", &DeviceMemoryAddress);
		Params.QueryValue(L"DEVICE_SIZE", &DeviceMemorySize);
	}

	pDevice = new (NonPagedPool) MapMemDevice(DeviceMemoryAddress, DeviceMemorySize);

	if ( pDevice == NULL )
		return STATUS_INSUFFICIENT_RESOURCES;
	else
	{
		NTSTATUS status = pDevice->ConstructorStatus();
		if ( !NT_SUCCESS(status) )
			delete pDevice;
		return status;
	}
}

#pragma code_seg()
