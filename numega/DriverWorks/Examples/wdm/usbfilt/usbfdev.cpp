// usbfdev.cpp - device class implementation for USB filter driver
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
#include <Kwdmfltr.cpp>
#include "usbfdev.h"

extern KDebugOnlyTrace T;

////////////////////////////////////////////////////////////////////////////////
// Constructor
//
UsbFilterDevice::UsbFilterDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KWdmFilterDevice(Pdo, NULL)
{
	T << "UsbFilterDevice::UsbFilterDevice\n";

	// Check constructor status
    if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
	    return;
	}

		// Remember our unit number
	m_Unit = Unit;

	// initialize the USB lower device
	m_Usb.Initialize(this, Pdo);

	NTSTATUS status = AttachFilter(&m_Usb); //Attach the filter
	if(!NT_SUCCESS(status))
		{
		m_ConstructorStatus = status;
		return;
		}

	SetFilterPowerPolicy();
	SetFilterPnpPolicy();
}

////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_SYSTEM_CONTROL 
//
// This routine just passes the IRP through to the next device since this driver
// is not a WMI provider.  
//
NTSTATUS UsbFilterDevice::SystemControl(KIrp I) 
{
	T << "UsbFilterDevice::SystemControl\n";

	return PassThrough(I);
}

////////////////////////////////////////////////////////////////////////////////
// Dispatch Handlers
//
// For a lower filter, this just passes the IRP through to USBD (via PassThru).
// For an upper filter, this just passes the IRP through to the USB 
// function driver (via PassThru).
//
// NOTE: An upper filter driver must handle all of the IRPs that the function
// driver does.  Depending on a particular USB function driver that is being
// filtered, the driver writer may have to implement additional IRP handlers
// to pass a particular IRP through to the function driver.  To do this, add
// an appropriate preprocessor definition to function.h for the particular IRP
// to handle and add a new method with an implementation that calls PassThru().
//
NTSTATUS UsbFilterDevice::Create(KIrp I)
{ 
	T << "UsbFilterDevice::Create\n";

	return PassThrough(I, LinkTo(DeviceControlComplete), this);
}

NTSTATUS UsbFilterDevice::Close(KIrp I)			
{ 
	T << "UsbFilterDevice::Close\n";

	return PassThrough(I);
}

NTSTATUS UsbFilterDevice::DeviceControl(KIrp I)	
{ 
	T << "UsbFilterDevice::DeviceControl\n";

	return PassThrough(I);
}

NTSTATUS UsbFilterDevice::Read(KIrp I)
{ 
	T << "UsbFilterDevice::Read\n";

	return PassThrough(I);
}

NTSTATUS UsbFilterDevice::Write(KIrp I)
{ 
	T << "UsbFilterDevice::Write\n";

	return PassThrough(I);
}

////////////////////////////////////////////////////////////////////////////////
// OnStartDevice - handler for IRP_MJ_PNP/IRP_MN_START_DEVICE
//
// There is no required functionality here, other than to pass the IRP
// through to USBD. However, the handler exercises some of the other 
// functions of KPnpLowerDevice and KUsbLowerDevice, and displays information
// about the device to the debug console.
//
NTSTATUS UsbFilterDevice::OnStartDevice(KIrp I)
{
	T << "UsbFilterDevice::OnStartDevice\n";


	USB_DEVICE_DESCRIPTOR DeviceDesc;
	USHORT buf[256]; ULONG PropLen;
	NTSTATUS status = STATUS_SUCCESS;

	m_Usb.GetDeviceDescription( buf, sizeof(buf), &PropLen );
	T << "Device Description: |"
	  << buf
	  << "|\n";

	IoGetDeviceProperty(
			PDO(),
			DevicePropertyDeviceDescription,
			sizeof(buf),
			buf,
			&PropLen
			);

	T << "Device Description: |"
	  << buf
	  << "|\n";

	m_Usb.GetPhysicalObjectName( buf, sizeof(buf), &PropLen );
	T << "PDO Name:	|"
	  << buf
	  << "|\n";

	m_Usb.GetHardwareID( buf, sizeof(buf), &PropLen );
	T << "Hardware ID:	|"
	  << buf
	  << "|\n";

	m_Usb.GetClassName( buf, sizeof(buf), &PropLen );
	T << "Class Name:	|"
	  << buf
	  << "|\n";

	m_Usb.GetDriverKeyName( buf, sizeof(buf), &PropLen );
	T << "Driver Key Name:	|"
	  << buf
	  << "|\n";

	m_Usb.GetClassGuid( buf, sizeof(buf), &PropLen );
	T << "Class Guid:	|"
	  << buf
	  << "|\n";

	m_Usb.GetManufacturer( buf, sizeof(buf), &PropLen );
	T << "Manufacturer:	|"
	  << buf
	  << "|\n";

	m_Usb.GetCompatibleIDs( buf, sizeof(buf), &PropLen );
	T << "Compat IDs:	|"
	  << buf
	  << "|\n";

	PCM_RESOURCE_LIST pcrl;
	m_Usb.GetBootConfiguration( FALSE, &pcrl, &PropLen ); 

	if( NT_SUCCESS( m_Usb.GetDeviceDescriptor(&DeviceDesc) ) )
	{
		T << DeviceDesc;
	}

	if( NT_SUCCESS( m_Usb.Preconfigure() ) )
	{
		T << *m_Usb.m_Config;

		PVOID Start=NULL;
		PUSB_ENDPOINT_DESCRIPTOR pEndpoints;
		PUSB_INTERFACE_DESCRIPTOR pIntf;

		for( int i = 0; (pIntf = m_Usb.LocateInterface(&Start, &pEndpoints)) != NULL; i++ )
		{
			T << *pIntf;

			for( int j = 0; j < pIntf->bNumEndpoints; j++)
			{
				T << pEndpoints[j];
			}
		}
	}
	return PassThrough(I);
	
}

////////////////////////////////////////////////////////////////////////////////
// InternalDeviceControl
//
// This routine is implemented for lower filters of a USB function driver.
//
// This is the handler for IRP_MJ_INTERNAL_DEVICE_CONTROL, which is used to
// pass URBs to USBD. This handler examines the URB and sets up a completion
// routine for the IRP. It then submits the URB on the same IRP, using the
// next IRP stack location.  
//
// NOTE: This routine can be called at DISPATCH_LEVEL since the the function
// driver's completion routine may have called IoCallDriver with this IRP.
//
NTSTATUS UsbFilterDevice::InternalDeviceControl(KIrp I)
{ 
	T << "UsbFilterDevice::InternalDeviceControl\n";

// Pass through IOCTLs that are not submitting an URB

	if (I.IoctlCode() != IOCTL_INTERNAL_USB_SUBMIT_URB)
		return DefaultPnp(I);

	PURB p = I.Urb(CURRENT);	// get URB pointer from IRP

	if(p)
	{
		T << *p;	// display URB contents using KDebugOnlyTrace
	}

// Submit the URB on the next stack location of the same IRP. Member
// function DeviceControlComplete will be called when USBD completes
// the IRP.
	return PassThrough(I, LinkTo(DeviceControlComplete), this);
}

////////////////////////////////////////////////////////////////////////////////
// InternalDeviceControl completion handler
//
// This routine is implemented for lower filters of a USB function driver.
//
// This member function examines the completed URB 
//
NTSTATUS UsbFilterDevice::DeviceControlComplete(KIrp I)
{
	T << "UsbFilterDevice::DeviceControlComplete\n";

	PURB p = I.Urb(CURRENT);

	if(p)
	{
		T << *p;	// display URB contents using KDebugOnlyTrace
	}

	return I.Status();
}

////////////////////////////////////////////////////////////////////////////////
// OnRemoveDevice
//
// Called when device is removed from the system
//
NTSTATUS UsbFilterDevice::OnRemoveDevice(KIrp I)
{
	T << "UsbFilterDevice::OnRemoveDevice\n";
	PassThrough(I);
	return STATUS_SUCCESS;
}
