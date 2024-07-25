// eUsbTherm.cpp - eUsbThermometer class implementation for USB thermometer
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

#include <vdw.h>		  // DriverWorks
#include <kusb.h>		  // DriverWorks
#include <tdiSclient.h>   // KTDI
#include "KHttpServerSession.h"
#include "eUsbTherm.h"

extern KDebugOnlyTrace T;

////////////////////////////////////////////////////////////////////////////////
// Constructor
//
eUsbThermometer::eUsbThermometer(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KPnpDevice(Pdo, NULL),
		m_dwCelsiusTemp(0xFFFFFFFF),
			m_pszHostIpAddr(NULL)
{
	T << "eUsbThermometer::eUsbThermometer\n";

	// Check constructor status
    if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
	    return;
	}

	// Remember our unit number
	m_Unit = Unit;

	// initialize the USB lower device
	m_Usb.Initialize(this, Pdo);

	//Set our device flags to be equal to the lower device
	PDEVICE_OBJECT LowerSystemDeviceObject =	m_Usb.DeviceObject();
	m_pDeviceObject->DeviceType = 	LowerSystemDeviceObject->DeviceType;
	m_pDeviceObject->Characteristics =	LowerSystemDeviceObject->Characteristics;
	m_pDeviceObject->Flags |= LowerSystemDeviceObject->Flags &
				(DO_DIRECT_IO | DO_BUFFERED_IO | DO_POWER_PAGABLE
				   | DO_POWER_INRUSH);

    // Inform the base class of the lower edge device object
	SetLowerDevice(&m_Usb);

	// Initialize the PnP Policy settings to the "standard" policy
	SetPnpPolicy();
	
}

////////////////////////////////////////////////////////////////////////////////
// PassThru
//
// This routine just passes the IRP through to the function driver.
// It is used as a generic pass through mechanism for Dispatch Handlers.
//
NTSTATUS eUsbThermometer::PassThru(KIrp I) 
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Usb.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_PNP
//
// This routine just passes the IRP through to the function driver.
//
NTSTATUS eUsbThermometer::DefaultPnp(KIrp I) 
{
	return PassThru(I);
}

////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_POWER
//
// This routine just passes the IRP through to the function driver.
//
NTSTATUS eUsbThermometer::DefaultPower(KIrp I) 
{
	I.IndicatePowerIrpProcessed();
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Usb.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_SYSTEM_CONTROL 
//
// This routine just passes the IRP through to the next device since this driver
// is not a WMI provider.  
//
NTSTATUS eUsbThermometer::SystemControl(KIrp I) 
{
	return PassThru(I);
}

////////////////////////////////////////////////////////////////////////////////
// Dispatch Handlers
//
// This routine just passes the IRP through to the function driver.
//
NTSTATUS eUsbThermometer::Create(KIrp I)
{ 
	return PassThru(I);
}

NTSTATUS eUsbThermometer::Close(KIrp I)			
{ 
	return PassThru(I);
}

NTSTATUS eUsbThermometer::DeviceControl(KIrp I)	
{ 
	return PassThru(I);
}

NTSTATUS eUsbThermometer::Read(KIrp I)
{ 
	return PassThru(I);
}

NTSTATUS eUsbThermometer::Write(KIrp I)
{ 
	return PassThru(I);
}

////////////////////////////////////////////////////////////////////////////////
// OnStartDevice - handler for IRP_MJ_PNP/IRP_MN_START_DEVICE
//
// There is no required functionality here, other than to pass the IRP
// through to USBD. 
//
NTSTATUS eUsbThermometer::OnStartDevice(KIrp I)
{
	T << "eUsbThermometer::OnStartDevice\n";

	NTSTATUS status = STATUS_SUCCESS;

#ifdef TDI_WINDOWS_95	
	//Setup up the TDI 
	status = InitializeTDI();
#endif
	
	return status;
	
	UNREFERENCED_PARAMETER(I);
}


////////////////////////////////////////////////////////////////////////////////
// OnRemoveDevice
//
// Called when device is removed from the system
//
NTSTATUS eUsbThermometer::OnRemoveDevice(KIrp I)
{
	T << "eUsbThermometer::OnRemoveDevice\n";

    if (m_pListener) 
	{
		// Disable network event notifications
		m_pListener->SetEvents(FALSE);

		// Iterate through the list of active sessions
		// and forcefully disconnect all active sessions
		KHttpServerSession* p;
		TDI_STATUS Status;

		while ( p = m_ActiveSessionList.RemoveHead() )
		{
			// By default, this method will perform an
			// abortive disconnect (RST)
			Status = p->disconnect();

			ASSERT(TDI_PENDING == Status || TDI_SUCCESS == Status);
		}

		// Wait for all outstanding requests to complete
		// By issuing a disconnect for all sessions, any
		// pending requests should be completed by the transport
		m_pListener->Wait();

		// destroy the socket
		delete m_pListener; 
		m_pListener = NULL;

		T << "eUsbThermometer: Listener stopped\n";
	}


	if (m_pszHostIpAddr)
	{
		delete m_pszHostIpAddr;
		m_pszHostIpAddr = NULL;
	}

	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(I);
}



////////////////////////////////////////////////////////////////////////////////
// InitializeTDI
//
// Called to create the TDI objects
//
NTSTATUS eUsbThermometer::InitializeTDI()
{
	// Initialize the TDIClient framework first

	if( !KTDInterface::Initialize() )
	{
		return STATUS_NOT_FOUND; // something wrong with TDI
	}

	// Create our Http server, port 80

	CIPTRANSPORT_ADDRESS http_port(80);
	m_pListener = new KStreamServer<KHttpServerSession> (http_port);

    // If succeeded - enable network events

    if( m_pListener && m_pListener->IsCreated() && m_pListener->IsPresent() ) 
	{
        m_pListener->SetEvents(TRUE);
        T << "KHttpServerSession: Listener started\n";
    }
    else 
	{
        T << "KHttpServerSession: Failed to start (port conflict?)\n";
        return STATUS_INSUFFICIENT_RESOURCES;
    }

	return STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// GetTemperature
//
// Called when an HTTP get request is received.  
//
NTSTATUS eUsbThermometer::GetTemperature()
{
	ULONG		bufInput	= 0xB;	// Input to device
	ULONG		bufOutput	= 0;	// Output from device
	ULONG_PTR	nOutput		= 0;	// Count written to bufOutput

	// Issue a I/O control request to the USB thermometer driver
	// to get the temperature

	NTSTATUS status = m_Usb.DeviceIoControl(
		CTL_CODE(0,1,METHOD_BUFFERED, 0),
		&bufInput,
		sizeof(bufInput),
		&bufOutput,
		sizeof(bufOutput),
		FALSE,
		&nOutput
		);

	if( STATUS_SUCCESS != status )
	{
		return status;
	}

	ASSERT( nOutput );

	// A quick cursory analysis of the data yielded this equation for
	// Celcius temperature. It might be incorrect.
	m_dwCelsiusTemp = ( (bufOutput & 0xff00) >> 8 ) - 27;
	T << "The temperature is " << m_dwCelsiusTemp << " degrees Celcius.\n";

	return status;
}


