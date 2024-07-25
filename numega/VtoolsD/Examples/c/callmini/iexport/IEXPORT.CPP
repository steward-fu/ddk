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

// iexport.cpp - NT/WDM driver that exports a function
// (c) Copyright 1996-1997, Compuware Corporation

// This driver is based on the Compuware Driver::Works tool kit.

#define VDW_MAIN	
#include <vdw.h>

#include "iexpdev.h"
#include "iexport.h"

// The exported function. See also: iexport.def

extern "C" ULONG __stdcall ExportedFunction(ULONG arg1, ULONG arg2)
{
	DbgPrint("The exported function was called: %x, %x\n", arg1, arg2);
	return arg1+arg2;
}

///////////////////////////////////////////////////////////////////
// Begin INIT time code
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(IexportDriver, NULL)

// DriverEntry

NTSTATUS IexportDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	IexportDevice* pDevice;

	// create a device
	pDevice = new (NonPagedPool) IexportDevice();

	// return the status
	if (pDevice)
	{
		NTSTATUS status = pDevice->ConstructorStatus();
		if (!NT_SUCCESS(status))
			delete pDevice;
		return status;
	}
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}

IexportDevice::IexportDevice(void) :
	KDevice(L"IEXPORT0", FILE_DEVICE_UNKNOWN, L"IEXPORT0")
{
}


// End INIT time code
#pragma code_seg()

// CreateClose handler
// 
NTSTATUS IexportDevice::CreateClose(KIrp I)
{
	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////
