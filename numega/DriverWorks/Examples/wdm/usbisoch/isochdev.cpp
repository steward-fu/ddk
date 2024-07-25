// isochdev.cpp - device class implementation for USB Isoc Sample Driver
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

#include "isochdev.h"
#include "isocioct.h"

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

UsbIsocDevice::UsbIsocDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :

    KPnpDevice(
        Pdo,
        KUnitizedName(L"UsbIsoc", Unit), 
        FILE_DEVICE_UNKNOWN,
        KUnitizedName(L"UsbIsoc", Unit)
        )
{
    Tracer << "UsbIsocDevice::UsbIsocDevice() Entry\n";

    m_pReadFifo     = NULL;
    m_pReadContexts = NULL;

    m_readOverflow  = 0;
    
    m_unitNum = Unit;

    m_Usb.Initialize(this, Pdo);    // initialize the USB interface

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
				0x88	//Endpoint Address
				);

    // initialize description of data out pipe (Out Bulk)
	m_Pipe1.Initialize(
				m_Usb,	//KUsbLowerDevice
				0x8		//Endpoint Address
				);

    SetLowerDevice(&m_Usb); // inform PnP code of our Top-of-stack device

    SetPnpPolicy();         // use the default Pnp policy
}


////////////////////////////////////////////////////////////////////////////////
// Destructor
//
// This is the destructor for the Functional Device Object, or FDO.

UsbIsocDevice::~UsbIsocDevice()
{
    Tracer << "UsbIsocDevice::~UsbIsocDevice() Entry\n";

	// If we have gotten here, it is because ALL our
	//   outstanding Isoc Read IRPs have returned,
	//   and have not been re-queued.
	//
	// NOTE: This is a duplication of code located in
	//       OnStopDevice(), put here just in case the
	//       IRP_MJ_PNP/IRP_MN_STOP_DEVICE IRP never
	//       gets received.
	//
	DeleteAllReadContexts();

    if ( m_pReadFifo ) {
        delete m_pReadFifo;
        m_pReadFifo = NULL;
    }
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
NTSTATUS UsbIsocDevice::DefaultPnp(KIrp I) 
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
NTSTATUS UsbIsocDevice::DefaultPower(KIrp I) 
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
NTSTATUS UsbIsocDevice::SystemControl(KIrp I) 
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

NTSTATUS UsbIsocDevice::OnStartDevice(KIrp I)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    AC_STATUS acStatus;

    Tracer << "UsbIsocDevice::OnStartDevice() Entry\n";
    
// The default Pnp policy has already cleared the IRP with the lower device
// Now we initialize the device object

    acStatus = m_Usb.ActivateConfiguration(1);

    switch (acStatus)
    {
    case AC_SUCCESS:
        Tracer << "Configuration OK\n";
        status = STATUS_SUCCESS;
        break;

    case AC_COULD_NOT_LOCATE_INTERFACE:
        Tracer << "Could not locate interface, endpoint address ";
        break;

    case AC_COULD_NOT_PRECONFIGURE_INTERFACE:
        Tracer << "Could not get configuration descriptor\n";
        break;

    case AC_CONFIGURATION_REQUEST_FAILED:
        Tracer << "Board did not accept configuration URB\n";
        break;

    case AC_FAILED_TO_INITIALIZE_INTERFACE_OBJECT:
        Tracer << "Failed to initialize interface for pipe[" ;
        break;

    case AC_FAILED_TO_LOCATE_ENDPOINT_ADDRESS:
        Tracer << "Failed to locate endpoint address for pipe[" ;
        break;
    }

    if ( !m_pReadFifo ) {
        m_pReadFifo =
          new (NonPagedPool) KLockableFifo<UCHAR>(ISOC_READ_FIFO_SIZE, NonPagedPool);

        if ( !m_pReadFifo ) {
          Tracer << "Allocation of Read FIFO failed!\n";
            
            status = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    ASSERT( m_pReadFifo );

	if ( NT_SUCCESS(status) )
		status = StartIsocReads();

    return status;  // base class completes the IRP
}


/////////////////////////////////////////////////////////////////////////
// OnStopDevice
//
// The system calls this when the device is stopped

NTSTATUS UsbIsocDevice::OnStopDevice(KIrp I)
{
    Tracer << "UsbIsocDevice::OnStopDevice() Entry\n";

	// If we have gotten here, it is because ALL our
	//   outstanding Isoc Read IRPs have returned,
	//   and have NOT been re-queued.
	//
	// NOTE: This code is duplicated in the destructor
	//       for this class, just in case the IRP
	//       IRP_MJ_PNP/IRP_MN_STOP_DEVICE does not
	//       get received.
	//
	DeleteAllReadContexts();

    if ( m_pReadFifo ) {
        delete m_pReadFifo;
        m_pReadFifo = NULL;
    }

    return m_Usb.Unconfigure();

    // base class passes to lower device
}

/////////////////////////////////////////////////////////////////////////
// OnRemoveDevice
//
// The system calls this when the device is removed.

NTSTATUS UsbIsocDevice::OnRemoveDevice(KIrp I)
{
    Tracer << "UsbIsocDevice::OnRemoveDevice() Entry\n";

    // Our PnP policy will take care of 
    // (1) giving the IRP to USBD
    // (2) detaching the PDO
    // (3) deleting the device object

    return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Dispatchers
//
// Nothing to do on Create and Close. A real driver would add additional
// dispatchers for data operations such as Read and Write. The compiler
// will combine identically coded functions.

NTSTATUS UsbIsocDevice::Create(KIrp I)
{ 
    Tracer << "UsbIsocDevice::Create() Entry\n";

    I.ForceReuseOfCurrentStackLocationInCalldown();
    NTSTATUS status = m_Usb.PnpCall(this, I);

    Tracer << "UsbIsocDevice:Create Status " << (ULONG)status << "\n";

    return status;
}


NTSTATUS UsbIsocDevice::Close(KIrp I)
{
    Tracer << "UsbIsocDevice::Close() Entry\n";

    I.ForceReuseOfCurrentStackLocationInCalldown();
    NTSTATUS status = m_Usb.PnpCall(this, I);

    Tracer << "UsbIsocDevice:Close Status " << (ULONG)status << "\n";

    return status;
}


NTSTATUS UsbIsocDevice::InternalDeviceControl(KIrp I)
{ 
    Tracer << "UsbIsocDevice::InternalDeviceControl() Entry\n";

    I.ForceReuseOfCurrentStackLocationInCalldown();
    NTSTATUS status = m_Usb.PnpCall(this, I);

    Tracer << "UsbIsocDevice::InternalDeviceControl Status "
           << (ULONG)status << "\n";

    return status;
}


/////////////////////////////////////////////////////////////////////////
// DeviceControl
//
// The system calls this when an application issues DeviceIoControl
//
NTSTATUS UsbIsocDevice::DeviceControl(KIrp I)
{ 
    Tracer << "UsbIsocDevice::DeviceControl() Entry\n";

    NTSTATUS status = STATUS_SUCCESS;

    switch (I.IoctlCode())
    {
        // Placeholder for real IOCTL calls
        case ISOC_FIRST_IOCTL:
            return I.PnpComplete(this, STATUS_SUCCESS);

        default:
            Tracer << "UNKNOWN [ID=" << I.IoctlCode() << "] );\n";
            I.Information() = 0;
            return I.PnpComplete(this, STATUS_INVALID_PARAMETER);
    }
}


/////////////////////////////////////////////////////////////////////////
// DeleteAllReadContexts
//
// Clean up routine
//
VOID UsbIsocDevice::DeleteAllReadContexts()
{
	while ( m_pReadContexts ) 
	{
        PISOC_CONTEXT pContext = m_pReadContexts;

		m_pReadContexts = pContext->pNext;
		
		ASSERT( !pContext->bUsed );

        if ( !pContext->bUsed ) 
        {
			if ( pContext->pIrp )
				delete pContext->pIrp;
			
			if ( pContext->pUrb )
				delete pContext->pUrb;
			
			if ( pContext->pData )
				delete pContext->pData;
			
			delete pContext;
		}
    }
}

