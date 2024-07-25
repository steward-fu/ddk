// filetest.cpp - implemetation of driver class for KFile example
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

// This is a very simple driver intended to show the usage of class
// KFile. The driver class simply creates an instance of the device
// class.

#define VDW_MAIN
#include <vdw.h>
#include <filedev.h>
#include <filetest.h>

#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(FileTestDriver, NULL)

NTSTATUS FileTestDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	FileTestDevice* pDevice;

	pDevice = new (NonPagedPool) FileTestDevice();

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
