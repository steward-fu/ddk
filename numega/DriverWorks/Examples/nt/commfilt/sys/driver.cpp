// Comm Filter Driver
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

/*
  Overview

	This is the main module of the comm filter driver. The driver
	demonstrates the usage of class KFilterDevice, a subclass of KDevice.
	An instance of a subclass of KFilterDevice attaches a named device,
	causing the system to run all I/O requests through the attaching
	driver.

	The subclass of KFilterDevice is named CommFilterDevice, and is
	contained in device.h and device.cpp.

	This driver creates a second type of device, named FilterControlDevice.
	Its purpose is to interface to a test application.

 Operation

	DriverEntry creates an instance of FilterControlDevice, which can be
	opened by an application. The instance has a data member which is a
	fifo. 

	The test application (..\exe\commfilt.c) opens the control device and
	sends an IOCTL instructing it to hook one or more comm ports. In 
	response, the control device creates an instance of the filter 
	device for each port.

	The read function of the filter device traps read requests to the comm.
	It passes IRPs through, requesting notification upon completion. When
	the IRP completes, the filter device writes to the fifo of the control
	device.

	When data appears in the control device, the control device notifies 
	the application via a shared event. The application issues a read 
	request to the control device, and the control device reads from 
	the fifo into the client buffer. This allows the application to 
	receive notification for all reads, writes, or device control
	messages sent to the comm port.

	When the application exits, it instructs the control device to unhook 
	the comm ports. To do so, the control device destroys the instance of 
	the filter device.
*/

// This #define brings in the driver dependent source modules for 
// KDriver and KDevice 
#define VDW_MAIN

// The following #define is required for this driver because it invokes the
// constructor for KDevice after initialization time. This #define prevents
// that constructor from being compiled into the INIT section.
#define DEVICES_CONSTRUCTED_AFTER_INIT

// The Driver::Works includes
#include <vdw.h>

// Includes for this driver
#include "common.h"
#include "commfifo.h"
#include "device.h"
#include "control.h"
#include "driver.h"

// Begin code for INIT section

#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(CommFilterDriver, NULL)

//////////////////////////////////////////////////////////////////////////
// DriverEntry
//
NTSTATUS CommFilterDriver::DriverEntry(PUNICODE_STRING RegistryPath)
// 
// Driver initialization entry point
//
{
	// Create an instance of the control device
	m_ControlDevice = new (NonPagedPool) FilterControlDevice();

	if ( m_ControlDevice )
	{
		NTSTATUS status = m_ControlDevice->ConstructorStatus();
		if ( !NT_SUCCESS(status) )
			delete m_ControlDevice;
		return status;
	}
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}
#pragma code_seg()

VOID CommFilterDriver::Unload(void)
{
	KPRIORITY OrigPriority;

	// Lower priority temporarily, in order to ensure that all
	// other threads in driver have completed before unloading.
	// (TBD: find a better way to handle this problem)
	OrigPriority = KeSetPriorityThread( KeGetCurrentThread(), 1 );
	KeSetPriorityThread( KeGetCurrentThread(), OrigPriority );

	// Delete any devices not already deleted
	DeleteDevices();
}

