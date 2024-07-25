// Port I/O driver
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
#include <piodev.h>
#include <portio.h>

#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(PortIoDriver, NULL)

NTSTATUS PortIoDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	PortIoDevice* pDevice;

	pDevice = new (NonPagedPool) PortIoDevice();

	if (pDevice)
	{
		NTSTATUS status = pDevice->ConstructorStatus();

		if ( !NT_SUCCESS(status))
			delete pDevice;

		return status;
	}
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}

#pragma code_seg()

