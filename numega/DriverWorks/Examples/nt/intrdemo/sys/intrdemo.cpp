// intrdemo.cpp - implementation of interrupt class demo
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

// This is a very simple driver that demonstrates the basic aspects
// of handling interrupts in kernel mode drivers. This driver shares
// IRQ 6, which is the floppy interrupt on Intel platforms.
//
// There is a simple test application that works in conjunction with
// this driver. See ..\exe.
//

#define VDW_MAIN
#include <vdw.h>
#include "common.h"
#include "intrdev.h"
#include "intrdemo.h"

/////////////////////////////////////////////////////////////////
// Begin INIT section code

#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(InterruptDemoDriver, NULL)

/////////////////////////////////////////////////////////////////
// DriverEntry
// 

NTSTATUS InterruptDemoDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{

// Just create the device object

	InterruptDemoDevice* pDevice;

	pDevice = new (NonPagedPool) InterruptDemoDevice();

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

// End INIT section code
/////////////////////////////////////////////////////////////////
