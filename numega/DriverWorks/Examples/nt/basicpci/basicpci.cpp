// bascipci.cpp - a basic PCI driver example
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

#include "function.h"
#include "pcidev.h"
#include "basicpci.h"

KDebugOnlyTrace	T("BasicPci");	// Global driver trace object

////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(BasicPci, NULL)

////////////////////////////////////////////////////////////////////
// Driver Entry 
//		This routine is called when the driver is loaded.
//
NTSTATUS BasicPci::DriverEntry(PUNICODE_STRING RegistryPath)
{

////////////////////////// begin debug code /////////////////////////////////////////

	ULONG shouldBreak = FALSE;

	// Open the driver "Parameters" key
	KRegistryKey Params(RegistryPath, L"Parameters");
	if ( NT_SUCCESS(Params.LastError()) )
	{
		Params.QueryValue(L"BreakOnEntry", &shouldBreak);
	}

	T << "In DriverEntry\n";

	if (shouldBreak) DbgBreakPoint();

////////////////////////// end debug code /////////////////////////////////////////

	NTSTATUS status;			// Status of device creation

	// Create an instance of class BasicPciDevice
	BasicPciDevice* pDevice;
	pDevice = new (NonPagedPool) BasicPciDevice(NULL, 0);
	if (pDevice == NULL)
	{
		T << TraceError << "Error creating device BasicPciDevice\n";
		status = STATUS_INSUFFICIENT_RESOURCES;
	}
	else
	{
		status = pDevice->ConstructorStatus();
		if (!NT_SUCCESS(status))
			delete pDevice;
	}

	T << TraceInfo << "DriverEntry status is " << ULONG(status) << "\n";

	return status;
}

#pragma code_seg()
//
// End INIT section code
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
// Unload
//
// This is called when the driver is unloaded.
//
VOID BasicPci::Unload(VOID)
{
	T << "Unload called\n";

	// This will delete all devices
	KDriver::Unload();
}
