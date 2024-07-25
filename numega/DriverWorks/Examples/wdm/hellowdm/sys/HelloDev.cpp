// Code for device class of most basic driver
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
#include "HelloWdm.h"	
#include "HelloDev.h"
#include "HelloDI.h"

GUID HelloDevGuid = HELLO_CLASS_GUID;

///////////////////////////////////////////////////////////////////////////////
// Constructor for subclass of KPnpDevice
//
SimpleWdmDevice::SimpleWdmDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KPnpDevice(
		Pdo,
		KUnitizedName(L"HelloWdm", Unit),
		FILE_DEVICE_UNKNOWN, 
		&HelloDevGuid,
		0,
		DO_POWER_PAGABLE
		)
{
    T << "entered SimpleWdmDevice constructor\n";

	// check for succes of base class construction

	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		T	<< TraceError 
			<< "Failed to create device, status=" 
			<< ULONG(m_ConstructorStatus)
			<< "\n";

		return;
	}

	// Attach the PDO as a lower device

	m_Pdo.Initialize(this, Pdo);

	// Establish m_Pdo as the lower device object

	SetLowerDevice(&m_Pdo);

	// Take the standard PnP Policy

	SetPnpPolicy();

	// Take the standard Power Policy

	SetPowerPolicy();

	// Adjust the standard Power Policy.  The standard policy
	// requests the device to power ON only when a new IO
	// request is received.  For this sample device driver,
	// the policy will be adjusted to power up the device
	// when the system is powered ON.
	m_PowerPolicies.m_GeneralPolicy.m_PowerUpOnS0 = TRUE;

}

///////////////////////////////////////////////////////////////////////////////

// 
NTSTATUS SimpleWdmDevice::Create(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	T << "entered the Create handler\n";

// An application (or other driver) is opening the device. The driver can
// store handle specific data at I.FileObject()->FsContext if required.

// TODO: Add driver specific create handling code here

// Complete the request. By using PnpComplete rather than simply Complete,
// we keep track of outstanding requests. If we get a remove notification,
// we can defer deletion of the device object until all requests complete.

	return I.PnpComplete(this, status, IO_NO_INCREMENT);
}


///////////////////////////////////////////////////////////////////
// Close handler
// 
NTSTATUS SimpleWdmDevice::Close(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	T << "entered the Close handler\n";

// Complete the request

	return I.PnpComplete(this, status, IO_NO_INCREMENT);
}


///////////////////////////////////////////////////////////////////
// DeviceControl handler
// 
NTSTATUS SimpleWdmDevice::DeviceControl(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	T << "entered DeviceControl(), code = " << I.IoctlCode() << "\n";

	switch (I.IoctlCode())
	{
#pragma warning(disable:4060)
	// add case stmts here
	}

	I.Information() = 0;

// It is important to complete the IRP with PnpComplete rather than
// Complete, because doing so enables the base class to track outstanding
// IRPs. This is required for safe removal and clean up.

	return I.PnpComplete(this, status);
}

///////////////////////////////////////////////////////////////////
// OnStartDevice
//
// This call instructs the device to initialize itself
//
NTSTATUS SimpleWdmDevice::OnStartDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	T << "entered OnStartDevice()\n";

	I.Information() = 0;

// The base class handles passing the IRP to the PDO, and will
// not call this routine if the PDO indicates a failure occurred.
// In addition, our PnP policy is set up to automatically enable
// the device interface when the device is started.

// If the device has hardware resources, make the following call

//	PCM_RESOURCE_LIST pResList = I.TranslatedResources();
//
// Then use pResList to initialize the resources. Refer to the PCI/WDM
// example to see how this is done.

// The base class will handle completion

	return status;
}


///////////////////////////////////////////////////////////////////
// OnStopDevice
//
// This call instructs the device to uninitialize itself. The system
// stops a device for the purpose of reconfiguration. It is likely
// that the device will subsequently receive an IRP_MN_START.
//
NTSTATUS SimpleWdmDevice::OnStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	T << "entered OnStopDevice()\n";

// TODO: Add code to disconnect interrupts, destroy memory or I/O
//       ranges, or dismantle anything else initialized in OnStartDevice.


// The base class handles disabling of the device interface and forwarding 
// to the PDO
	
	return status;
}


///////////////////////////////////////////////////////////////////
// OnRemoveDevice
//
// This call notifies the driver that the device has been removed.
// It is not necessarily preceeded by QueryRemove or Stop
//
NTSTATUS SimpleWdmDevice::OnRemoveDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

    T << "entered OnRemoveDevice()\n";

	// TODO: add device specific removal code

    // Our PnP policy will take care of 
    // (1) giving the IRP to the PDO
    // (2) detaching the PDO
    // (3) deleting the device object when it is safe to do so
	
	return status;
}

///////////////////////////////////////////////////////////////////
// OnQueryRemoveDevice
//
// This call notifies the driver that the device has been removed.
//
NTSTATUS SimpleWdmDevice::OnQueryRemoveDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

    T << "entered OnQueryRemoveDevice()\n";

// The default Pnp policy fails the request before even calling this
// handler if the device is open. 


// TODO: prepare to allow removal if ok to do so

// If this handler returns STATUS_SUCCESS, the default Pnp policy causes
// any subsequent I/O requests to be put into the hold queue (deferred
// request queue). 

// The PnP policy can be configured to cancel the current request
// or outstanding requests in the device queue (if any) upon successful return 
// from this handler.

// The default policy causes the IRP to be forwarded to (and completed by)
// the lower device (PDO).
	
	return status;
}

///////////////////////////////////////////////////////////////////
// DetermineNewDevicePowerState
//
// This call is made by the framework to obtain the DEVICE_POWER_STATE
// that this device should transition to in response to POWER IRPs.  When
// a SYSTEM POWER IRP is received to change system power, this call is made 
// by the framework with the system's requested SYSTEM_POWER_STATE.  The 
// default base class implementation in KPnpDevice will use the DEVICE_CAPABILITIES
// structure reported from the bus driver for the hardware.  For most real
// devices, this structure will contain the correct DEVICE_POWER_STATE's.
// However, since this sample device driver does not control hardware, the
// system reports all DEVICE_POWER_STATE's to be PowerDeviceD1.  In order to
// demonstrate the framework's behavior for different power states, this
// virtual function has been overrided so that many power state transitions
// for the device can be supported.
//
// Most device drivers will not need to implement this function.
//
DEVICE_POWER_STATE SimpleWdmDevice::DetermineNewDevicePowerState(SYSTEM_POWER_STATE SystemPowerState)
{
	DEVICE_POWER_STATE DevicePowerState;

// The system is transitioning power to SystemPowerState.  We return
// the matching device power state for the system power state.  The
// base class KPnpDevice::Power member will handle requesting device 
// POWER IRP's (if required) to change device power based on these states.
	switch(SystemPowerState)
	{
	case PowerSystemWorking:
	// This is the fully ON power state.  If the device is not already in the
	// fully ON state, the base class will handle requesting a DEVICE
	// POWER IRP to put the device in the fully ON state.
		DevicePowerState = PowerDeviceD0;
		break;

	case PowerSystemSleeping1:
		DevicePowerState = PowerDeviceD1;
		break;

	case PowerSystemSleeping2:
		DevicePowerState = PowerDeviceD2;
		break;

	case PowerSystemSleeping3:
		DevicePowerState = PowerDeviceD2;
		break;

	case PowerSystemHibernate:
		DevicePowerState = PowerDeviceD3;
		break;

	case PowerSystemShutdown:
		DevicePowerState = PowerDeviceD3;
		break;

	default:
		break;
	}

	return DevicePowerState;
}

///////////////////////////////////////////////////////////////////
// DefaultPnp
// 
NTSTATUS SimpleWdmDevice::DefaultPnp(KIrp I)
{
    T << "entered DefaultPnp() \n";

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}

///////////////////////////////////////////////////////////////////
// DefaultPower
// 
NTSTATUS SimpleWdmDevice::DefaultPower(KIrp I)
{
    T << "entered DefaultPower() \n";

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
NTSTATUS SimpleWdmDevice::SystemControl(KIrp I) 
{
    T << "entered SystemControl() \n";

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Pdo.PnpCall(this, I);
}


///////////////////////////////////////////////////////////////////
// OnDeviceSleep
// 
NTSTATUS SimpleWdmDevice::OnDeviceSleep(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

    T << "entered OnDeviceSleep() \n";

	//The framework will call this function when the device transitions
	//from a higher power state to a lower power state, i.e. in response
	//to an IRP_MJ_POWER with minor function IRP_MN_SET_POWER.  This function
	//is called before forwarding this IRP to the next device in the
	//device stack.  For transitions out of the fully powered state PowerDeviceD0,
	//this means that power is still applied to the device and the 
	//hardware can be accessed.

	//a real driver would save context to the hardware so that it can be
	//restored when the device is powered up

	return status;
}

///////////////////////////////////////////////////////////////////
// OnDevicePowerUp
// 
NTSTATUS SimpleWdmDevice::OnDevicePowerUp(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

    T << "entered OnDevicePowerUp() \n";

	//The framework will call this function when the device transitions
	//from a lower power state to a higher power state, i.e. in response
	//to an IRP_MJ_POWER with minor function IRP_MN_SET_POWER.  This function
	//is called when this IRP is completed which means that power has been
	//applied to the device and the hardware can be accessed.

	//a real driver would restore context to the hardware previously saved
	//when the device powered down

	return status;
}


////////////////////////////////////////////////////////////////////////
// SimpleWdmDevice::OnQueryPower 
//
// handler for IRP_MN_QUERY_POWER 
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	
//
// Notes
//		This function is implemented to display trace messages indicating
//		when this type of IRP is received.  Since the base class Power handler
//		takes care of forwarding the IRP to the lower device and requesting
//		any device power state changes, most drivers do not need to implement
//		this method.
//
NTSTATUS SimpleWdmDevice::OnQueryPower(KIrp I) 
{
	NTSTATUS status = STATUS_SUCCESS;

	T	<< "entered OnQueryPower()\n" ;

	switch ( I.PowerStateType() ) 
	{
 	case SystemPowerState:
		T << "    " << "SystemPowerState " << (UCHAR)I.PowerStateSetting().SystemState << "\n" ;
		break;

 	case DevicePowerState:
		T << "    " << "DevicePowerState " << (UCHAR)I.PowerStateSetting().DeviceState << "\n" ;
		break;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////
// SimpleWdmDevice::OnSetPower 
//
// handler for IRP_MN_SET_POWER 
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	
//
// Notes
//		This function is implemented to display trace messages indicating
//		when this type of IRP is received.  Since the base class Power handler
//		takes care of forwarding the IRP to the lower device and requesting
//		any device power state changes, most drivers do not need to implement
//		this method.
//
NTSTATUS SimpleWdmDevice::OnSetPower(KIrp I) 
{
	NTSTATUS status = STATUS_SUCCESS;

	T	<< "entered OnSetPower()\n" ;

	switch ( I.PowerStateType() ) 
	{
 	case SystemPowerState:
		T << "    " << "SystemPowerState " << (UCHAR)I.PowerStateSetting().SystemState << "\n" ;
		break;

 	case DevicePowerState:
		T << "    " << "DevicePowerState " << (UCHAR)I.PowerStateSetting().DeviceState << "\n" ;
		break;
	}

	return status;
}


