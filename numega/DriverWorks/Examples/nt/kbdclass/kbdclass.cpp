// kbdclass.cpp - Keyboard Class Driver
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

#include "ntddkbd.h"
#include "kbdlog.h"
#include "kbclsdev.h"
#include "kbdclass.h"

/////////////////////////////////////////////////////////////////////////////
// This is the keyboard class driver, based on the DDK source. It can be
// substituted for the system version of kbdclass.sys. Its purpose is to 
// provide the user subsystem with a generic keyboard interface. To do this,
// it connects to one or more keyboard port drivers.
//
/////////////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(KbdclassDriver, NULL)

/////////////////////////////////////////////////////////////////////////////
// DriverEntry
//
// This is the entry point for the driver. It fetches the parameters and
// creates the device object.
//
NTSTATUS KbdclassDriver::DriverEntry(IN PUNICODE_STRING RegistryPath)
{
	KbdclassDevice* pDevice;
	NTSTATUS status;
	ULONG nPortsConnected, i;
	WCHAR Scratch[NAME_MAX];

	// Set default values for parameters
	m_InputQueueSize = DATA_QUEUE_SIZE;
	m_MaxPortsServiced = 1;
	m_ConnectMultiplePorts = 0;

	// Get parameter values from Registry
	if ( ! NT_SUCCESS( (status=GetParameters()) ) )
		return status;

	// Construct the stem of the name of the port drivers by replacing the last 5 
	// characters ("class") with "Port".
	KUstring PortBase(m_DeviceNameBase->Size(), PagedPool);
	PortBase.Assign( *m_DeviceNameBase);
	PortBase.ShortenBy(5); // 'Class'
	PortBase.Append(L"Port");	

	// Scan the registry's Device Map to determine how many keyboard ports
	// are present
	DeterminePortsServiced((PUNICODE_STRING)PortBase, &m_nPortsToService);

	// Create a string for the base name of the port object by replacing 
	// "Class" at the tail of name of the class object with "Port0"
	PortBase.GrowMaxBy(8); // add space for "\Device\" at front
	PortBase.Assign(L"\\Device\\");
	PortBase.Append(*m_DeviceNameBase);
	PortBase.ShortenBy(5); // 'Class'
	PortBase.Append(L"Port");	
	KUnitizedName PortName(PortBase, 0);

	// Now build the Unit name, which is passed to the device constructor
	KUnitizedName UnitName(*m_DeviceNameBase, 0);

	// Now build the full class name, which is used to set up the device map 
	// in the registry
	KUstring FullClassName( 
		(USHORT)sizeof(L"\\Device\\")+m_DeviceNameBase->Size(), 
		PagedPool);
	FullClassName.Assign(L"\\Device\\");
	FullClassName.Append(UnitName);

	// Open the registry key for the device map
	KRegistryKey DeviceMapKey(
		REGISTRY_DEVICEMAP, 
		*m_DeviceNameBase,
		TRUE,
		REG_OPTION_VOLATILE,
		KEY_SET_VALUE,
		KEY_DEFAULT_ATTRIBUTES
		);

	// Now create the device object(s). If we are not connecting multiple ports
	// to a single device, then a device object is created for each port.
	// Otherwise, only a single device is created. The loop connects the newly
	// created device object to a port, and connects all ports to either the
	// single device object or each to its respective device object.
	for (i = 0, nPortsConnected = 0; i < m_nPortsToService; i++)
	{
		if ( (i == 0) || !m_ConnectMultiplePorts )
		{
			pDevice = 
				new(NonPagedPool) KbdclassDevice( 
					UnitName, 
					m_InputQueueSize,
					m_ConnectMultiplePorts ? m_nPortsToService : 1 
					);
		}
		status = pDevice->ConnectToPort(PortName);
		PortName.NextUnit();

		// Add the device map entry
		if (NT_SUCCESS(status) )
		{

			// If this is the first time through the loop or if we are not
			// connecting multiple ports to a single device object, then
			// proceed with the devicemap set up
			if ( (++nPortsConnected == 1) || !m_ConnectMultiplePorts)
			{
				if ( (i > 0) && !m_ConnectMultiplePorts )
				{
					UNICODE_STRING Root;
					DeviceMapKey.Reconstruct(
						DeviceMapKey.GetRootPath(&Root,REGISTRY_DEVICEMAP),
						FullClassName,
						KEY_SET_VALUE,
						KEY_DEFAULT_ATTRIBUTES,
						TRUE,
						REG_OPTION_VOLATILE
					);
				}
				if ( NT_SUCCESS( DeviceMapKey.LastError()) )
				{
					RtlMoveMemory(
						Scratch, 
						RegistryPath->Buffer, 
						RegistryPath->Length);
					Scratch[RegistryPath->Length/sizeof(WCHAR)]=0;

					DeviceMapKey.WriteValue(
						(PUNICODE_STRING)FullClassName, 
						Scratch);
				}

				// Log an error if writing the devicemap failed
				if ( ! NT_SUCCESS( DeviceMapKey.LastError() ) )
				{
					KErrorLogEntry::SimpleError(
						DeviceMapKey.LastError(),
						KBDCLASS_NO_DEVICEMAP_CREATED,
						KEYBOARD_ERROR_VALUE_BASE + 14,
						NULL);
				}
			}
		}
		else if	(m_ConnectMultiplePorts)
			break;

		if ( (i == 0) || !m_ConnectMultiplePorts )
		{
			UnitName.NextUnit();
			FullClassName.Last()++;
		}
	}

	// If no keyboard port devices were connected, then the initialization
	// has failed. Log a message and return an error status.
	if (nPortsConnected == 0)
	{
		KErrorLogEntry::SimpleError(
			STATUS_DEVICE_NOT_CONNECTED,
			KBDCLASS_NO_PORT_DEVICE_OBJECT,
			KEYBOARD_ERROR_VALUE_BASE + 18,
			NULL);

		return STATUS_DEVICE_NOT_CONNECTED;
	}
	// Otherwise, at least one keyboard port was successfully connected - success
	else
		return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// GetParameters
//
// This function is called during initialization to read parameters from the
// registry.
//
NTSTATUS KbdclassDriver::GetParameters(void)
{
	ULONG NameBaseLength = 0;
	PCWSTR DefaultBase = L"KeyboardClass";	
	PWSTR pNameBase = (PWSTR)DefaultBase;

	KRegistryKey Params(RegistryPath(), L"Parameters");

	if (NT_SUCCESS(Params.LastError()))
	{
		Params.QueryValue(L"KeyboardDataQueueSize", &m_InputQueueSize);
		Params.QueryValue(L"MaximumPortsServiced", &m_MaxPortsServiced);
		Params.QueryValue(L"ConnectMultiplePorts", &m_ConnectMultiplePorts);
		Params.QueryValue(L"KeyboardDeviceBaseName", pNameBase, NameBaseLength, PagedPool);
	}

	m_DeviceNameBase = new(PagedPool) KUstring(pNameBase, PagedPool);
	if (pNameBase != DefaultBase)
		delete pNameBase;

	if (m_DeviceNameBase == NULL) 
	{
		KErrorLogEntry::SimpleError(
			STATUS_INSUFFICIENT_RESOURCES,
			KBDCLASS_INSUFFICIENT_RESOURCES,
			KEYBOARD_ERROR_VALUE_BASE + 2,
			NULL);

		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// DeterminePortsServiced
//
// This function enumerates the entries under devicemap key for the keyboard
// port. Each value corresponds to a port. This is done to determine how many
// ports the class driver must provide for.
//
NTSTATUS KbdclassDriver::DeterminePortsServiced(
	PUNICODE_STRING BasePortName, 
	PULONG pnServiced
	)
{
	KUstring val(80, PagedPool);
	struct {
		KEY_VALUE_BASIC_INFORMATION Info;
		WCHAR NameSpill[127];
	} BasicInfo;

	ULONG InfoSize = sizeof(BasicInfo);

	*pnServiced = 0;

	KRegistryKey KeyboardDeviceMap(REGISTRY_DEVICEMAP, BasePortName);

	if ( NT_SUCCESS (KeyboardDeviceMap.LastError()) )
	{
		KeyboardDeviceMap.InitializeValueEnumeration();
		while (KeyboardDeviceMap.EnumerateValue(val) != STATUS_NO_MORE_ENTRIES) 
			++(*pnServiced);

		return STATUS_SUCCESS;	
	}
	else
		return KeyboardDeviceMap.LastError();
}

/////////////////////////////////////////////////////////////////////////////
#pragma code_seg()
// End INIT section code
/////////////////////////////////////////////////////////////////////////////

