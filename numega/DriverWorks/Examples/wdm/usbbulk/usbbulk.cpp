// UsbBulk.h - a simple USB bulk driver
//
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
#include <vdw.h>        // Base Driver::Works includes
#include <kusb.h>

#include "bulkdev.h"
#include "usbbulk.h"

KTrace Tracer("UsbBulk");

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(UsbBulkDriver, NULL)


/////////////////////////////////////////////////////////////////////
// Driver Entry
// 
// This is the first entry point called by the system when the driver
// is loaded.

NTSTATUS UsbBulkDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	Tracer << "In Driver Entry";

    m_Unit = 0;

    return STATUS_SUCCESS;
}

#pragma code_seg()
// End INIT section
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
// Add Device
//
// The system calls this entry point when it detects a device for which
// this driver is responsible.
//
// Input
//   Pdo      Physical Device Object. This is a pointer to a system device
//            object that represents the physical device.
//
// This function creates the Functional Device Object, or FDO. The FDO
// enables this driver to handle requests for the physical device. 

NTSTATUS UsbBulkDriver::AddDevice(PDEVICE_OBJECT Pdo)
{
	Tracer << "Into AddDevice\n";

    UsbBulkDevice* pDevice = 
        new (NonPagedPool) UsbBulkDevice(Pdo, m_Unit);

    if (pDevice)
    {
        NTSTATUS status = pDevice->ConstructorStatus();

        if ( !NT_SUCCESS(status) ) {
			Tracer << "Error constructing device UsbBulk"
					<< (ULONG)m_Unit << "\n";

            delete pDevice;

        } else {
            m_Unit++;
        }

        return status;
    }
    else {
		Tracer << "Error creating device UsbBulk"
				<< (ULONG)m_Unit << "\n";

        return STATUS_INSUFFICIENT_RESOURCES;
    }
}

