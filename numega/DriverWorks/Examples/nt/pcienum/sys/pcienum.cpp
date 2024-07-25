// pcienum.cpp - PCI enumerator driver
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

// This example enables an application to access the configuration
// space of the PCI bus(es) in the computer. The application opens
// the device (PCIENUM) and makes successive calls to ReadFile. Each
// call returns the configuration header of an installed PCI device,
// until there are no more, in which case EOF is returned.

#include "enumdev.h"
#include "pcienum.h"

////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(PciEnumDriver, NULL)

////////////////////////////////////////////////////////////////////
// DriverEntry
//
NTSTATUS PciEnumDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	PciEnumDevice* pDevice;

	pDevice = new (NonPagedPool) PciEnumDevice();

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
#pragma code_seg()
//
// End INIT section code
////////////////////////////////////////////////////////////////////
