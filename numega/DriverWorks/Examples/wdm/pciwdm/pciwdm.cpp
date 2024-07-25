// pciwdm.cpp - Sample PCI driver for WDM platform
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

#include "pciwdmdv.h"		// include file for device class
#include "pciwdm.h"			// include file for driver class

///////////////////////////////////////////////////////////////////
// Begin INIT time code
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(PciwdmDriver, NULL)

////////////////////////////////////////////////////////////////
// DriverEntry
// 
// This is the first routine in the driver that the system calls.
// FOr this driver, there is nothing to do.
//
NTSTATUS PciwdmDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	m_instance = 0;			// init count of device units created
	return STATUS_SUCCESS;
}
// End INIT time code
#pragma code_seg()

////////////////////////////////////////////////////////////////
// AddDevice
// 
// The system calls this when it detects each instance of the
// device that this driver supports.
//
// The parameter (PDO) to this routine identifies the Physical
// Device Object created by the system for the instance of
// device.
//
NTSTATUS PciwdmDriver::AddDevice(PDEVICE_OBJECT PDO)
{
	PciwdmDevice* pD = new (NonPagedPool) PciwdmDevice(PDO, m_instance);
	if (pD != NULL)
	{
		NTSTATUS status = pD->ConstructorStatus();
		if ( !NT_SUCCESS(status) )
			delete pD;
		else
			m_instance++;

		return status;
	}
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}
