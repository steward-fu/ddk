// busbdev.cpp - device class implementation for basic USB sample driver
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
#include <kusb.h>
#include "busbdev.h"
#include "busbioct.h"

////////////////////////////////////////////////////////////////////////////////
// Constructor
//
// This is the constructor for the Functional Device Object, or FDO. It
// is derived from KPnpDevice, which builds in automatic dispatching of
// subfunctions of IRP_MJ_POWER and IRP_MJ_PNP to virtual member functions.
//
// Input
//   Pdo       Physical Device Object - this is a pointer to a system device
//             object that represents the physical device.
//
// The object being constructed contains a data member (m_Usb) of type
// KPnpLowerDevice. By initializing it, the driver binds the FDO to the
// PDO and creates an interface to the upper edge of the system USB
// class driver.

BasicUsbDevice::BasicUsbDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KPnpDevice(
		Pdo,
		KUnitizedName(L"BasicUsb", Unit), 
		FILE_DEVICE_UNKNOWN,
		KUnitizedName(L"BasicUsb", Unit)
		)
{
	m_Usb.Initialize(this, Pdo);	// initialize the USB interface
	SetLowerDevice(&m_Usb);

	SetPnpPolicy();					// setup a standarad PnP policy
}

////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_PNP
//
// This routine just passes the IRP through to USBD. It is 
// the default handler for IRP_MJ_PNP. IRPs that correspond to
// any virtual members of KpnpDevice that handle minor functions of
// IRP_MJ_PNP and that are not overridden get passed to this routine.
//
// For diagnostic purposes, this routine is set up to emit the function
// name to the debugger.
//
NTSTATUS BasicUsbDevice::DefaultPnp(KIrp I) 
{
	static char* minors[] = {
		"IRP_MN_START_DEVICE",	
		"IRP_MN_QUERY_REMOVE_DEVICE",	
		"IRP_MN_REMOVE_DEVICE",	
		"IRP_MN_CANCEL_REMOVE_DEVICE",	
		"IRP_MN_STOP_DEVICE",	
		"IRP_MN_QUERY_STOP_DEVICE",	
		"IRP_MN_CANCEL_STOP_DEVICE",	
		"IRP_MN_QUERY_DEVICE_RELATIONS",	
		"IRP_MN_QUERY_INTERFACE",	
		"IRP_MN_QUERY_CAPABILITIES",	
		"IRP_MN_QUERY_RESOURCES",	
		"IRP_MN_QUERY_RESOURCE_REQUIREMENTS",	
		"IRP_MN_QUERY_DEVICE_TEXT",	
		"IRP_MN_FILTER_RESOURCE_REQUIREMENTS",	
		"IRP_MN_undefined",	
		"IRP_MN_READ_CONFIG",	
		"IRP_MN_WRITE_CONFIG",	
		"IRP_MN_EJECT",	
		"IRP_MN_SET_LOCK",	
		"IRP_MN_QUERY_ID",	
		"IRP_MN_QUERY_PNP_DEVICE_STATE",	
		"IRP_MN_QUERY_BUS_INFORMATION",	
		"IRP_MN_DEVICE_USAGE_NOTIFICATION",
		"IRP_MN_SURPRISE_REMOVAL"
	};

	ULONG Minor = I.MinorFunction();
	CHAR* IrpName;

	if ( Minor < IRP_MN_SURPRISE_REMOVAL )
		IrpName = minors[Minor];
	else
		IrpName = "<unknown>";

	DbgPrint("Pnp IRP minor function=%s\n", IrpName);

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Usb.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_POWER 
//
// This routine just passes the IRP through to USBD. It is 
// the default handler for IRP_MJ_POWER.
//
NTSTATUS BasicUsbDevice::DefaultPower(KIrp I) 
{
	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Usb.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
// Handler for IRP_MJ_SYSTEM_CONTROL 
//
// This routine just passes the IRP through to the next device since this driver
// is not a WMI provider.
//
NTSTATUS BasicUsbDevice::SystemControl(KIrp I) 
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Usb.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
// OnStartDevice - handler for IRP_MJ_PNP/IRP_MN_START_DEVICE
//
// This is the routine where the driver initializes the physical device and
// sets up its USB related data members.
//
// This device has one interface and one pipe. Other USB devices
// are more complex, but it simply a matter of adding more data
// members to the device class in order to support more interfaces
// or pipes.

NTSTATUS BasicUsbDevice::OnStartDevice(KIrp I)
{
	NTSTATUS status;

// The framework does a synchronous call to the lower device when IRP_MN_START_DEVICE
// is received. This is part of the 'standard' PnP policy:  m_ForwardToPdoPolicy.m_CallBeforeStart
// == TRUE. If the lower device returns success, then the framework calls this routine.

// Now we begin to initialize the device object. MOST OF THIS CODE CAN
// BE AVOIDED IF YOU USE ActivateConfiguration. See example USBBULK and
// USBISOCH to see how to do it the easier way.

	_try
	{
		// Fetch a configuration descriptor

		status = m_Usb.Preconfigure(0, 1024);
		if ( !NT_SUCCESS(status) )
			_leave;

		// Configure an interface. First locate an interface.

		PVOID Start=NULL;
		PUSB_ENDPOINT_DESCRIPTOR pEndpoints;
		PUSB_INTERFACE_DESCRIPTOR pIntfDesc;

		pIntfDesc = m_Usb.LocateInterface(
						&Start, 
						&pEndpoints
						);
		if (pIntfDesc == NULL)
		{
			status = STATUS_UNSUCCESSFUL;
			_leave;
		}

		// We found an interface. Now add it to the set of interfaces
		// that the driver will support. 

		status = m_Usb.PreconfigureInterface(pIntfDesc);
		if ( !NT_SUCCESS(status) )
			_leave;

		// Now set the configuration
			
		status = m_Usb.Configure();
		if ( !NT_SUCCESS(status) )
			_leave;

		// Initialize an Interface object

		status = m_Interface.Initialize(
			m_Usb, 
			pIntfDesc->bInterfaceNumber
			);

		if ( !NT_SUCCESS(status) )
			_leave;

		// Initialize a pipe object

		m_Pipe.Initialize(m_Interface, 0);

		// Send a class specific URB to set up the device

		PURB pSetupUrb = m_Pipe.BuildClassRequest(
			(PUCHAR)NULL,	// TransferBuffer
			0,				// TransferBufferLength
			0x22, 			// RequestTypeReservedBits
			0xa,			// Request
			0,				// Value
			FALSE,			// bIn
			FALSE,			// bShortOk
			NULL			// Urb Link
			);

		if (pSetupUrb != NULL)
		{
			status = m_Pipe.SubmitUrb( pSetupUrb );
			delete pSetupUrb;
		}

	} 
	_finally {}

	return status;
}

///////////////////////////////////////////////////////////////////////////////////
// OnStopDevice
//
// The system calls this when the device is stopped. Completion of the Irp is handled
// by the framework.

NTSTATUS BasicUsbDevice::OnStopDevice(KIrp I)
{
	return m_Usb.Unconfigure();
}

///////////////////////////////////////////////////////////////////////////////
// Dispatchers
//
// Nothing to do on Create and Close. A real driver would add additional
// dispatchers for data operations such as Read and Write. The compiler
// will combine identically coded functions.

NTSTATUS BasicUsbDevice::Create(KIrp I)
{ 
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Usb.PnpCall(this, I);
}

NTSTATUS BasicUsbDevice::Close(KIrp I)
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Usb.PnpCall(this, I);
}

NTSTATUS BasicUsbDevice::InternalDeviceControl(KIrp I)
{ 
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Usb.PnpCall(this, I);
}

///////////////////////////////////////////////////////////////////////////////////
// DeviceControl
//
// The system calls this when an application issues DeviceIoControl
//
NTSTATUS BasicUsbDevice::DeviceControl(KIrp I)
{ 
	NTSTATUS status;

	I.Information() = 0;

	switch (I.IoctlCode())
	{
	case IOCTL_TEST_XFER:
	{

	// Build a transfer request

		PURB pUrb = m_Pipe.BuildInterruptTransfer(
			I.IoctlBuffer(), 
			I.IoctlOutputBufferSize()
			);

		if( NULL == pUrb )
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

	// We are about to submit a request to the USB driver, so
	// bump the outstanding request count. Note that the counter
	// is also by the base class to count requests coming into 
	// our driver. We will decrement the count in the completion
	// routine. This enables us to handle removals cleanly.

		IncrementOutstandingRequestCount();

	// Pass the request down on the same IRP. This is the
	// asynchronous form -- note use of completion routine

		I.MarkPending();
		status = m_Pipe.SubmitUrb(
			I,
			pUrb,
			LinkTo(TransferComplete), 	
			this
			);

		return STATUS_PENDING;
	}

	case IOCTL_TEST_STATUS:
	{
		USHORT DeviceStatus;
		USHORT InterfaceStatus;
		USHORT PipeStatus;

		m_Usb.GetStatus(&DeviceStatus);
		m_Interface.GetStatus(&InterfaceStatus);
		m_Pipe.GetStatus(&PipeStatus);

		((USHORT*)I.IoctlBuffer())[0] = DeviceStatus;
		((USHORT*)I.IoctlBuffer())[1] = InterfaceStatus;
		((USHORT*)I.IoctlBuffer())[2] = PipeStatus;

		I.Information() = 3*sizeof USHORT;
		status = STATUS_SUCCESS;
		break;
	}

	case IOCTL_TEST_RESET:
	{
		status = m_Pipe.Reset();

		((ULONG*)I.IoctlBuffer())[0] = status;

		I.Information() = sizeof ULONG;
		break;
	}

	default:
		status = STATUS_INVALID_PARAMETER;
	}

	return I.PnpComplete(this, status);
}

///////////////////////////////////////////////////////////////////////////////////
// TransferComplete
//
// Completion routine for transfer test
//
NTSTATUS BasicUsbDevice::TransferComplete(
	KIrp I, 
	_URB_BULK_OR_INTERRUPT_TRANSFER* pUrb
	)
{
	if (I->PendingReturned)
		I.MarkPending();

	// Set number of bytes transferred from URB to IRP

	I.Information() = pUrb->TransferBufferLength;

	// Release the URB

	delete pUrb;

	DecrementOutstandingRequestCount();

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
// Linkage code for completion routine
//
// This is the first code called on completion of the xfer IRP. It
// just invokes the non-static function. For this simple example,
// it's not really necessary to find the device object, but this 
// shows how it can be done for more complex drivers.
//
NTSTATUS BasicUsbDevice::LinkTo(TransferComplete)(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	PVOID Context
    )
{

// get pointer to instance of BasicUsbDevice

	BasicUsbDevice* pDevice = (BasicUsbDevice*)KDevicePTR(DeviceObject);

// call non-static member function

	return pDevice->TransferComplete(
		KIrp(Irp),
		(_URB_BULK_OR_INTERRUPT_TRANSFER*)Context
		);
}
