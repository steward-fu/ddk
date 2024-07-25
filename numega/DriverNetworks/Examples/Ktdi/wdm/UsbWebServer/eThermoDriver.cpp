// eThermoDriver.cpp - implementation of class eThermoDriver 
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#define VDW_MAIN
#include <vdw.h>		  // DriverWorks
#include <kusb.h>		  // DriverWorks
#include <tdiSclient.h>   // KTDI

#include "KHttpServerSession.h"
#include "eUsbTherm.h"
#ifdef TDI_WINDOWS_NT
#include "tdipnp.h"
#endif
#include "eThermoDriver.h"

KUstring us(L"eThermoDriver");

// Global driver trace object
KDebugOnlyTrace T("eThermoDriver", TRACE_MONITOR | TRACE_DEBUGGER, TraceAlways, BreakNever, us);

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(eThermoDriver, NULL)

/////////////////////////////////////////////////////////////////////
// Driver Entry
//
NTSTATUS eThermoDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	T << "eThermoDriver::DriverEntry\n";

	m_Unit = 0;
	KNdisString temp(RegistryPath);

	m_RegistryPath.Initialize(temp.Length()+1);
	m_RegistryPath.Assign(temp);
	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(RegistryPath);
}

// End INIT section
/////////////////////////////////////////////////////////////////////
#pragma code_seg()

/////////////////////////////////////////////////////////////////////
// Add Device
//
NTSTATUS eThermoDriver::AddDevice(PDEVICE_OBJECT Pdo)
{
	T << "eThermoDriver::AddDevice\n";

	//Create an instance of our eUsbThermometer class.

	eUsbThermometer * pFilterDevice = new (
			static_cast<PCWSTR>(NULL),
			FILE_DEVICE_UNKNOWN,
			static_cast<PCWSTR>(NULL),
			0,
			DO_DIRECT_IO
			)
		eUsbThermometer(Pdo, m_Unit);

	if (pFilterDevice)
	{
		NTSTATUS status = pFilterDevice->ConstructorStatus();

		if ( !NT_SUCCESS(status) )
		{
			T << "Failed to construct eUsbThermometer"
			  << (ULONG) m_Unit << " status = " << status << "\n";

			delete pFilterDevice;
		}
		else
		{
			m_Unit++;
#ifdef TDI_WINDOWS_NT
			m_TdiCallbacks.SetDeviceObject(pFilterDevice);
			m_TdiCallbacks.Initialize(m_RegistryPath, TDI_PNP_HOOK_BINDINGS);
#endif
		}

		return status;
	}
	else
	{
		T << "Failed to allocate eUsbThermometer"
		  << (ULONG) m_Unit << "\n";

		return STATUS_INSUFFICIENT_RESOURCES;
	}
}

