// Test code for class KBusAddress
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
#include <busaddev.h>
#include <busaddr.h>

#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(BusAddressTestDriver, NULL)

NTSTATUS BusAddressTestDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	BusAddressTestDevice* pDevice;

	pDevice = new (NonPagedPool) BusAddressTestDevice();

	if (pDevice)
		return pDevice->ConstructorStatus();
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}

#pragma code_seg()
