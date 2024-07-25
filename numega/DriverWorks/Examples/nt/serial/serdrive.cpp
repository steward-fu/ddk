// serdrive.cpp - main module for serial driver example
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

// SERDRIVE is a serial port driver. It supports multiple
// 8250/16550 style UARTs.

// The device objects created by SERDRIVE are type Uart, which is
// derived from SerialDevice. SerialDevice implements the upper
// edge of the NT serial interface. This includes IRP dispatching,
// IRP queuing, and IOCTL handling. Class Uart overrides some
// members of SerialDevice to implement UART functionality. Class
// SerialDevice is intended as a reusable class to implement
// any kind of serial device. Class Uart is an example of its 
// usage for a particular kind of device.

// The driver class, SerialDriver, is very simple. It queries the
// system for UARTs, and for each one attempts to claim the required
// resources. Note that some ports may be in use, as is the case
// when there is a serial mouse. If the driver is able to claim
// the resources for at least of the detected ports, then it remains
// loaded.

// SERDRIVE can be loaded and unloaded with Driver::Monitor. It will
// be necessary to disable the system serial driver in order to
// successfully install SERDRIVE. 

// There is built-in tracing which can be enabled by modifying 
// file glbtrace.h.

#define VDW_MAIN
#include <vdw.h>
#include "uart.h"
#include "serdrive.h"
#include "glbtrace.h"

////////////////////////////////////////////////////////////////////////////
// Global data

KTrace* Gt=0;
KList<SharedInterrupt>* SharedInterrupt::m_List = NULL;

/////////////////////////////////////////////////////////////////////////////
// Begin INIT code section
#pragma code_seg("INIT")

/////////////////////////////////////////////////////////////////////////////
DECLARE_DRIVER_CLASS(SerialDriver, L"LOADED SERIAL DRIVER RESOURCES")

/////////////////////////////////////////////////////////////////////////////
// DriverEntry
//
// Initialize the driver.
//
// DriverEntry creates a configuration query object to probe the 
// system for Uart devices. The configuration callback invokes the
// constructor of class Uart for each port detected. If at least
// one constructor succeeds, the function returns success.
//
NTSTATUS SerialDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	BOOLEAN DeviceConstructed = FALSE;	

// Initialize tracing
	SetupGlobalTrace(KUstring(L"SerDrive"));
	GTRACE( (TLEVEL,"DriverEntry\n") );

// Initialize the interrupt list required by class Uart. See shareint.h
// for more information.
	SharedInterrupt::m_List = &m_InterruptList;

// Create a configuration query. This accesses the system's device 
// inventory. For each port found, the query calls 
// SerialDriver::OnConfigure.
	KConfigurationQuery Q(
		LinkTo(OnConfigure), 
		&DeviceConstructed,
		SerialController
		);

// If at least one device was found, return success.
	if (DeviceConstructed)
		return STATUS_SUCCESS;
	else
		return STATUS_DEVICE_NOT_CONNECTED;
}

/////////////////////////////////////////////////////////////////////////////
// OnConfigure
//
// Configuration callback. The configuration query object invokes this
// routine for each serial port found in the system device inventory.
// This function invokes the constructor of class Uart, passing to
// it the resources associated with the indicated port.
//
// The context parameter points to BOOLEAN DeviceConstructed in 
// DriverEntry. This is set to TRUE is the constructor

NTSTATUS SerialDriver::OnConfigure(
	QUERY_RESULT* bus,
	QUERY_RESULT* controller,
	QUERY_RESULT* peripheral,
	PVOID Context
	)
{
	GTRACE((TLEVEL,"OnConfigure, id=%S\n", controller->m_DeviceId));

// Examine the resources required by this port. The assumption is
// that each port has an interrupt and a port range.

	BOOLEAN PortFound=FALSE;
	ULONGLONG PortBase=0;
	ULONG PortLength=0;

	BOOLEAN IrqFound=FALSE;
	ULONG IrqLevel=0;
	ULONG IrqVector=0;

	for (ULONG i=0; i < controller->m_ResourceCount; i++)
	{
		PCM_PARTIAL_RESOURCE_DESCRIPTOR pRes = 
			&controller->m_Resources[i];

	// check for port range
		if ( !PortFound && (pRes->Type == CmResourceTypePort))
		{
			PortFound = TRUE;
			PortBase = pRes->u.Port.Start.QuadPart;
			PortLength = pRes->u.Port.Length;
		}

	// check for interrupt
		else if ( !IrqFound && (pRes->Type == CmResourceTypeInterrupt))
		{
			IrqFound = TRUE;
			IrqLevel = pRes->u.Interrupt.Level;
			IrqVector = pRes->u.Interrupt.Vector;
		}
	}

	GTRACE((TLEVEL,"PortBase=%x Length=%x Vector=%x Level=%x\n",\
		(ULONG)PortBase, PortLength, IrqLevel, IrqVector)) ;

	// If both an interrupt and port range were found, we can
	// try to construct an instance of class Uart

	if (PortFound && IrqFound)
	{

	// construct device object

		Uart* pPort = 
			new (NonPagedPool) Uart(
				controller->m_number,
				bus->m_type.bus,
				bus->m_number,
				PortBase,
				PortLength,
				IrqLevel,
				IrqVector
				);

	// if the constructor succeeded, set the success flag

		if (pPort &&  NT_SUCCESS(pPort->ConstructorStatus()) )
			*(BOOLEAN*)Context = TRUE;

	// otherwise, delete the device object

		else if ( pPort != NULL)
			delete pPort;
	}

	return STATUS_SUCCESS;
}

#pragma code_seg()
// End INIT code section
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Unload
//
// This is called when 
//
VOID SerialDriver::Unload(void)
{
	GTRACE((TLEVEL,"Unloading...\n"));
	KDriver::Unload();
	ShutdownGlobalTrace();
}
