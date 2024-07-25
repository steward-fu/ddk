// pciwdmdv.cpp - KDevice subclass for PCIWDM example
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
#include <vdw.h>
#include "pciwdmdv.h"

/////////////////////////////////////////////////////////////
// Constructor

PciwdmDevice::PciwdmDevice(PDEVICE_OBJECT PDO, ULONG instance) :
	KPnpDevice(
		PDO, 
		KUnitizedName(L"Pciwdm", instance),
		FILE_DEVICE_UNKNOWN, 
		KUnitizedName(L"Pciwdm", instance),
		0,
		DO_DIRECT_IO
		),

	m_Pci(NULL)
{
// Initialize the connection to the Physical Device Object
// This enables us to pass IRPs down to the PDO for default
// processing.

	if ( NT_SUCCESS(m_ConstructorStatus) )
		m_ConstructorStatus = m_Pdo.Initialize(this, PDO);

	if ( ! NT_SUCCESS(m_ConstructorStatus) )
		return;
		
// Initialize the KPciConfiguration object. Although the exapmle
// includes this object, it is not strictly necessary. You can use
// to read and write the configuration space, but in a PnP environment,
// this is rarely required.

	m_Pci.Initialize(m_Pdo.DeviceObject());

// Inform the base class of our lower edge object

	SetLowerDevice(&m_Pdo);

// Set the standard policy for PnP handling

	SetPnpPolicy();
}


///////////////////////////////////////////////////////////////////
// Create handler
// 
NTSTATUS PciwdmDevice::Create(KIrp I)
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}

///////////////////////////////////////////////////////////////////
// Close handler
// 
NTSTATUS PciwdmDevice::Close(KIrp I)
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}

///////////////////////////////////////////////////////////////////
// DeviceControl handler
// 
NTSTATUS PciwdmDevice::DeviceControl(KIrp I)
{
	switch (I.IoctlCode())
	{
#pragma warning(disable:4060)
	// add case stmts here
	}

	I.Information() = 0;
	return I.PnpComplete(this, STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////
// The Interrupt Service Routine
//
#define IsOurInterrupt() TRUE	// replace with actual test

BOOLEAN PciwdmDevice::TheIsr(void)
{
	if (IsOurInterrupt())
	{
	// acknowledge interrupt to deassert interrupt request signal

	// Queue a DPC
		m_Dpc.Request();

	// Return TRUE to indicate that the interrupt was handled
		return TRUE;
	}

	else
	// Return FALSE to indicate that the interrupt was not for this device
		return FALSE;
}

///////////////////////////////////////////////////////////////////
// The DPC
//
VOID PciwdmDevice::TheDpc(PVOID arg1, PVOID arg2)
{
	// do DISPATCH_LEVEL processing for interrupt
}


///////////////////////////////////////////////////////////////////
// OnStartDevice
//
// This call instructs the device to initialize itself
//
NTSTATUS PciwdmDevice::OnStartDevice(KIrp I)
{
	NTSTATUS status;

	I.Information() = 0;

// The base class handles passing the IRP to the PDO, and will
// not call this routine if the PDO indicates a failure occurred.


// First get the list of translated resources from the IRP

	PCM_RESOURCE_LIST pResList = I.TranslatedResources();
	PCM_RESOURCE_LIST pRawList = I.AllocatedResources();

// Use the list of resources to set up the device's memory
// mapped region. If a device has multiple memory regions,
// the device class embed multiple instances of KMemoryRange.
// The same applies to I/O ranges, in which case KIoRange
// is the relevant class. 

	status = m_MemRange.Initialize(pResList, pRawList, 0);
	if ( !NT_SUCCESS(status) )
		return status;		

// The memory range is initialized. Its base virtual address
// in system space is obtained by calling member Base. Its
// physical address in CPU space is obtained by calling
// CpuPhysicalAddress(). Member functions inb, inw, ind, outb,
// outw, and outd access the memory mapped region.
 
// Now set up the interrupt

	status = m_Interrupt.InitializeAndConnect(
		pResList, 
		LinkTo(TheIsr), 
		this
		);

// The base class will handle completion

	return status;
}


///////////////////////////////////////////////////////////////////
// OnStopDevice
//
// This call instructs the device to uninitialize itself
//
NTSTATUS PciwdmDevice::OnStopDevice(KIrp I)
{
	m_Interrupt.Disconnect();
	m_MemRange.~KMemoryRange();		

// The base class handles forwarding to the PDO
	
	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// DefaultPnp
// 
NTSTATUS PciwdmDevice::DefaultPnp(KIrp I)
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}

///////////////////////////////////////////////////////////////////
// DefaultPower
// 
NTSTATUS PciwdmDevice::DefaultPower(KIrp I)
{
	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Pdo.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
// SystemControl
//
// This routine just passes the IRP through to the next device since this driver
// is not a WMI provider.
//
NTSTATUS PciwdmDevice::SystemControl(KIrp I) 
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}


