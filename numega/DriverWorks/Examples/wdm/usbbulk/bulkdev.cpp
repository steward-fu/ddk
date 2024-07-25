// bulkdev.cpp - device class implementation for USB Bulk sample driver
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

#include <vdw.h>
#include <kusb.h>
#include "bulkdev.h"
#include "bulkioct.h"

extern KTrace Tracer;          // Global driver trace object

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
//   Unit      Unit number - this is a number to append to the device's
//             base device name to form the Logical Device Object's name
//
// The object being constructed contains a data member (m_Usb) of type
// KPnpLowerDevice. By initializing it, the driver binds the FDO to the
// PDO and creates an interface to the upper edge of the system USB
// class driver.

UsbBulkDevice::UsbBulkDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :

    KPnpDevice(
        Pdo,
        KUnitizedName(L"UsbBulk", Unit), 
        FILE_DEVICE_UNKNOWN,
        KUnitizedName(L"UsbBulk", Unit),
		0,
		DO_DIRECT_IO
        )
{
    Tracer << "UsbBulkDevice::UsbBulkDevice() Entry\n";

	// initialize the USB lower device
    m_Usb.Initialize(this, Pdo);    

	// initialize the USB interface
    m_Interface.Initialize(
					m_Usb,	//KUsbLowerDevice
					0,		//Interface Number
					1,		//Configuration Value
					0		//Initial Alternate Setting for the Interface
					);    

    // initialize description of data in pipe (In Bulk)
	m_Pipe0.Initialize(
				m_Usb,	//KUsbLowerDevice
				0x81	//Endpoint Address
				);

    // initialize description of data out pipe (Out Bulk)
	m_Pipe1.Initialize(
				m_Usb,	//KUsbLowerDevice
				0x2		//Endpoint Address
				);

    // inform PnP code of our Top-of-stack device
	SetLowerDevice(&m_Usb);         

    // use the default Pnp policy
	SetPnpPolicy();                 

    // use the default Power policy
	SetPowerPolicy();               
}


////////////////////////////////////////////////////////////////////////////////
// Destructor
//
// This is the destructor for the Functional Device Object, or FDO.

UsbBulkDevice::~UsbBulkDevice()
{
    Tracer << "UsbBulkDevice::~UsbBulkDevice() Entry\n";
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
NTSTATUS UsbBulkDevice::DefaultPnp(KIrp I) 
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
NTSTATUS UsbBulkDevice::DefaultPower(KIrp I) 
{
    I.IndicatePowerIrpProcessed();
    I.CopyParametersDown();
    return m_Usb.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
// SystemControl
//
// This routine just passes the IRP through to the next device since this driver
// is not a WMI provider.
//
NTSTATUS UsbBulkDevice::SystemControl(KIrp I) 
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
// This device has one interface and two pipes. Other USB devices
// are more complex, but it simply a matter of adding more data
// members to the device class in order to support more interfaces
// or pipes.

NTSTATUS UsbBulkDevice::OnStartDevice(KIrp I)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    AC_STATUS acStatus;

    Tracer << "UsbBulkDevice::OnStartDevice() Entry\n";
    
// The default Pnp policy has already cleared the IRP with the lower device

    acStatus = m_Usb.ActivateConfiguration(
        1   // configuration value 1
        );

    switch (acStatus)
    {
    case AC_SUCCESS:
        Tracer << "Configuration OK\n";
        status = STATUS_SUCCESS;
        break;

    case AC_COULD_NOT_LOCATE_INTERFACE:
        Tracer << "Could not locate interface\n";
        break;

    case AC_COULD_NOT_PRECONFIGURE_INTERFACE:
        Tracer << "Could not get configuration descriptor\n";
        break;

    case AC_CONFIGURATION_REQUEST_FAILED:
        Tracer << "Board did not accept configuration URB\n";
        break;

    case AC_FAILED_TO_INITIALIZE_INTERFACE_OBJECT:
        Tracer << "Failed to initialize interface\n";
        break;

	case AC_FAILED_TO_OPEN_PIPE_OBJECT:
		//NOTE: this may or may not be fatal.  It could mean that
		//the device has an endpoint for which a KUsbPipe object has
		//not been instanced.  If the intention is to not use this pipe,
		//then it's ok.  Otherwise, there is a failure.  Clients can
		//iterate through the pipe array in KUsbLowerDevice to check 
		//which pipes are open/closed.
		Tracer << "Failed to open pipe object \n";
		break;

	default:
		Tracer << "Unexpected error activating USB configuration\n";
		break;
    }

    return status;  // base class completes the IRP
}


/////////////////////////////////////////////////////////////////////////
// OnStopDevice
//
// The system calls this when the device is stopped

NTSTATUS UsbBulkDevice::OnStopDevice(KIrp I)
{
    Tracer << "UsbBulkDevice::OnStopDevice() Entry\n";

    return m_Usb.DeActivateConfiguration();

    // base class passes to lower device
}

/////////////////////////////////////////////////////////////////////////
// OnRemoveDevice
//
// The system calls this when the device is removed.

NTSTATUS UsbBulkDevice::OnRemoveDevice(KIrp I)
{
    Tracer << "UsbBulkDevice::OnRemoveDevice() Entry\n";

    // Our PnP policy will take care of 
    // (1) giving the IRP to USBD
    // (2) detaching the PDO
    // (3) deleting the device object

	m_Usb.ReleaseResources();

    return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Dispatchers
//
// Nothing to do on Create and Close. A real driver would add additional
// dispatchers for data operations such as Read and Write. The compiler
// will combine identically coded functions.

NTSTATUS UsbBulkDevice::Create(KIrp I)
{ 
    Tracer << "UsbBulkDevice::Create() Entry\n";

    I.ForceReuseOfCurrentStackLocationInCalldown();
    NTSTATUS status = m_Usb.PnpCall(this, I);

    Tracer << "UsbBulkDevice:Create Status " << (ULONG)status << "\n";

    return status;
}


NTSTATUS UsbBulkDevice::Close(KIrp I)
{
    Tracer << "UsbBulkDevice::Close() Entry\n";

    I.ForceReuseOfCurrentStackLocationInCalldown();
    NTSTATUS status = m_Usb.PnpCall(this, I);

    Tracer << "UsbBulkDevice:Close Status " << (ULONG)status << "\n";

    return status;
}


NTSTATUS UsbBulkDevice::InternalDeviceControl(KIrp I)
{ 
    Tracer << "UsbBulkDevice::InternalDeviceControl() Entry\n";

    I.ForceReuseOfCurrentStackLocationInCalldown();
    NTSTATUS status = m_Usb.PnpCall(this, I);

    Tracer << "UsbBulkDevice::InternalDeviceControl Status "
            << (ULONG)status << "\n";

    return status;
}


/////////////////////////////////////////////////////////////////////////
// DeviceControl
//
// The system calls this when an application issues DeviceIoControl
//
NTSTATUS UsbBulkDevice::DeviceControl(KIrp I)
{ 
    Tracer << "UsbBulkDevice::DeviceControl() Entry\n";

#pragma warning(disable:4065) // switch with no cases

    switch (I.IoctlCode())
    {
    default:
        Tracer << "UNKNOWN [ID=" << I.IoctlCode() << "] );\n",
        I.Information() = 0;
        return I.PnpComplete(this, STATUS_INVALID_PARAMETER);
    }
}


////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_READ
//
// This routine is the default handler for IRP_MJ_READ.
// Incoming IRPs are passed on to the USB device, synchronously.
//
// The KPnpDevice class handles restricting IRP flow
// if the device is stopping or being removed.
//
NTSTATUS UsbBulkDevice::Read(KIrp I) 
{
    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

    Tracer << "UsbBulkDevice::Read() Entry\n";

	// Declare a memory object
	KMemory Mem(I.Mdl());

    ULONG dwTotalSize = I.ReadSize(CURRENT);
	ULONG dwMaxSize = m_Pipe0.MaximumTransferSize();

	// If the total requested read size is greater than the Maximum Transfer
	// Size for the Pipe, request to read only the Maximum Transfer Size since
	// the bus driver will fail an URB with a TransferBufferLength of greater
	// than the Maximum Transfer Size. 
	if ( dwTotalSize > dwMaxSize )
	{
		ASSERT(dwMaxSize);
		dwTotalSize = dwMaxSize;
	}

    ULONG dwBytesRead = 0;

    // Create an URB to do actual Bulk read from Pipe0
    PURB pUrb = m_Pipe0.BuildBulkTransfer(
							Mem,		  // Where is data coming from?
							dwTotalSize,  // How much data to read?
							TRUE,         // direction (TRUE = IN)
							NULL	      // Link to next URB
							);

	if ( pUrb != NULL)
	{
	    // Submit the URB to our USB device, synchronously - say less is OK
		pUrb->UrbBulkOrInterruptTransfer.TransferFlags =
				(USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK);

        status = m_Pipe0.SubmitUrb(pUrb, NULL, NULL);

        if ( NT_SUCCESS(status) ) 
        {
	            dwBytesRead = pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength;

				if (dwBytesRead > 0) 
					Tracer << "Read() got " << dwTotalSize	<< " bytes from USB\n";
	    }

		delete pUrb;
	}

	
    I.Information() = dwBytesRead;
    return I.PnpComplete(this, status, IO_NO_INCREMENT);
}


////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_WRITE
//
// This routine is the default handler for IRP_MJ_WRITE.
// Incoming IRPs just send data down to the USB device, synchronously.
//
// The KPnpDevice class handles restricting IRP flow
// if the device is stopping or being removed.
//
NTSTATUS UsbBulkDevice::Write(KIrp I) 
{
    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

    Tracer << "UsbBulkDevice::Write() Entry\n";

	// Declare a memory object
	KMemory Mem(I.Mdl());

    ULONG dwTotalSize = I.WriteSize(CURRENT);
	ULONG dwMaxSize = m_Pipe1.MaximumTransferSize();

	// If the total requested write size is greater than the Maximum Transfer
	// Size for the Pipe, request to write only the Maximum Transfer Size since
	// the bus driver will fail an URB with a TransferBufferLength of greater
	// than the Maximum Transfer Size. 
	if ( dwTotalSize > dwMaxSize )
	{
		ASSERT(dwMaxSize);
		dwTotalSize = dwMaxSize;
	}

    ULONG dwBytesSent = 0;

    PURB pUrb = m_Pipe1.BuildBulkTransfer(
							Mem,          // Where is data coming from?
							dwTotalSize,  // How much data to write?
							FALSE,        // direction (FALSE = OUT)
							NULL          // Link to next URB
							);

    // Submit the URB to our USB device, synchronously
    if (pUrb != NULL) 
    {
        status = m_Pipe1.SubmitUrb(pUrb, NULL, NULL);

        if ( NT_SUCCESS(status) ) 
        {
            dwBytesSent = pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength;

            Tracer << "Write() posted "  << dwTotalSize << " bytes to USB\n";
        }

		delete pUrb;
    }

    I.Information() = dwBytesSent;
    return I.PnpComplete(this, status, IO_NO_INCREMENT);
}
