// VWriteDevice.cpp
// Implementation of VWriteDevice device class
//
// Generated by DriverWizard version DriverStudio 2.7.0 (Build 554)
// Requires Compuware's DriverWorks classes
//

#pragma warning(disable:4065) // Allow switch statement with no cases
		  
#include <vdw.h>
#include <initguid.h>
#include "..\vinterface.h"

#include "vwrite.h"
#include "vwritdev.h"

#pragma hdrstop("vwrite.pch")

////////////////////////////////////////////////////////////////////////
//  VWriteDevice::VWriteDevice
//
//	Routine Description:
//		This is the constructor for the Functional Device Object, or FDO.
//		It is derived from KPnpDevice, which builds in automatic
//	    dispatching of subfunctions of IRP_MJ_POWER and IRP_MJ_PNP to
//		virtual member functions.
//
//	Parameters:
//		Pdo - Physical Device Object - this is a pointer to a system
//			device object that represents the physical device.
//
//		Unit - Unit number. This is a number to append to the device's
//			base device name to form the Logical Device Object's name
//
//	Return Value:
//		None   
//
//	Comments:
//		The object being constructed contains a data member (m_Lower) of type
//		KPnpLowerDevice. By initializing it, the driver binds the FDO to the
//		PDO and creates an interface to the upper edge of the system class driver.
//

VWriteDevice::VWriteDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KPnpDevice(Pdo, &VWRITEDEVICE_CLASS_GUID),
	m_Unit(Unit)
{
	t << "VWriteDevice::VWriteDevice " << Unit << EOL;

    if ( NT_SUCCESS(m_ConstructorStatus) )
	{
		m_Lower.Initialize(this, Pdo);
		SetLowerDevice(&m_Lower);

		SetPnpPolicy();
		SetPowerPolicy();

		RtlZeroMemory(&m_Interface, sizeof(m_Interface));
	}
}


////////////////////////////////////////////////////////////////////////
//  VWriteDevice::~VWriteDevice
//
//	Routine Description:
//		This is the destructor for the Functional Device Object, or FDO.
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//

VWriteDevice::~VWriteDevice()
{
	t << "VWriteDevice::~VWriteDevice() " << m_Unit << EOL;
}

////////////////////////////////////////////////////////////////////////
//  VWriteDevice::DefaultPnp
//
//	Routine Description:
//		Default handler for IRP_MJ_PNP
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result returned from lower device
//
//	Comments:
//		This routine just passes the IRP through to the lower device. It is 
//		the default handler for IRP_MJ_PNP. IRPs that correspond to
//		any virtual members of KpnpDevice that handle minor functions of
//		IRP_MJ_PNP and that are not overridden get passed to this routine.
//

NTSTATUS VWriteDevice::DefaultPnp(KIrp I) 
{
	t << "VWriteDevice::DefaultPnp " << I;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}


////////////////////////////////////////////////////////////////////////
//  VWriteDevice::DefaultPower
//
//	Routine Description:
//		Default handler for IRP_MJ_POWER 
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result returned from lower device
//
//	Comments:
//		This routine just passes the IRP through to the lower device. It is 
//		the default handler for IRP_MJ_POWER.
//

NTSTATUS VWriteDevice::DefaultPower(KIrp I) 
{
	t << "Entering VWriteDevice::DefaultPower " << I << EOL;

	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Lower.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
//  VWriteDevice::SystemControl
//
//	Routine Description:
//		Default handler for IRP_MJ_SYSTEM_CONTROL
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result returned from lower device
//
//	Comments:
//		This routine just passes the IRP through to the next device since this driver
//		is not a WMI provider.
//

NTSTATUS VWriteDevice::SystemControl(KIrp I) 
{
	t << "VWriteDevice::SystemControl " << m_Unit << EOL;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////
//  VWriteDevice::OnStartDevice
//
//	Routine Description:
//		Handler for IRP_MJ_PNP subfcn IRP_MN_START_DEVICE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		Initialize the physical device. Typically, the driver initializes
//		physical resources here.  Call I.AllocatedResources() for a list
//		of the raw resources that the system has assigned to the device,
//		or I.TranslatedResources() for the translated resource list.
//

NTSTATUS VWriteDevice::OnStartDevice(KIrp I)
{
	t << "VWriteDevice::OnStartDevice\n";

	NTSTATUS status = STATUS_SUCCESS;

	I.Information() = 0;

	PCM_RESOURCE_LIST pResListRaw = I.AllocatedResources();
	PCM_RESOURCE_LIST pResListTranslated = I.TranslatedResources();

	EnableIdleDetection(10, 10, PowerDeviceD1);

	return status;
}


////////////////////////////////////////////////////////////////////////
//  VWriteDevice::OnStopDevice
//
//	Routine Description:
//		Handler for IRP_MJ_PNP subfcn IRP_MN_STOP_DEVICE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		The system calls this when the device is stopped.
//		The driver should release any hardware resources
//		in this routine.
//
//		The base class passes the irp to the lower device.
//

NTSTATUS VWriteDevice::OnStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "VWriteDevice::OnStopDevice\n";

	DisableIdleDetection();

	return status;
	UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  VWriteDevice::OnRemoveDevice
//
//	Routine Description:
//		Handler for IRP_MJ_PNP subfcn IRP_MN_REMOVE_DEVICE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		The system calls this when the device is removed.
//		Our PnP policy will take care of 
//			(1) giving the IRP to the lower device
//			(2) detaching the PDO
//			(3) deleting the device object
//

NTSTATUS VWriteDevice::OnRemoveDevice(KIrp I)
{
	t << "VWriteDevice::OnRemoveDevice\n";

	return STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  VWriteDevice::OnDevicePowerUp
//
//	Routine Description:
//		Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//		for a request to go to power on state from low power state
//
//	Parameters:
//		I - IRP containing POWER request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		This routine implements the OnDevicePowerUp function.
//		This function was called by the framework from the completion
//		routine of the IRP_MJ_POWER dispatch handler in KPnpDevice.
//		The bus driver has completed the IRP and this driver can now
//		access the hardware device.  
//		This routine runs at dispatch level.
//	

NTSTATUS VWriteDevice::OnDevicePowerUp(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering VWriteDevice::OnDevicePowerUp " << I;

	EnableIdleDetection(10, 10, PowerDeviceD1);

	return status;
	UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  VWriteDevice::OnDeviceSleep
//
//	Routine Description:
//		Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//		for a request to go to a low power state from a high power state
//
//	Parameters:
//		I - IRP containing POWER request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		This routine implements the OnDeviceSleep function.
//		This function was called by the framework from the IRP_MJ_POWER 
//		dispatch handler in KPnpDevice prior to forwarding to the PDO.
//		The hardware has yet to be powered down and this driver can now
//		access the hardware device.  
//		This routine runs at passive level.
//	

NTSTATUS VWriteDevice::OnDeviceSleep(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering VWriteDevice::OnDeviceSleep " << I;

	DisableIdleDetection();

	return status;
	UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  VWriteDevice::Create
//
//	Routine Description:
//		Handler for IRP_MJ_CREATE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//

NTSTATUS VWriteDevice::Create(KIrp I)
{
	t << "VWriteDevice::Create " << m_Unit << EOL;

	NTSTATUS status = STATUS_SUCCESS;

	PDEVICE_OBJECT pDevice = IoGetAttachedDeviceReference(*this);

	KEvent Event(NotificationEvent, FALSE);

	IO_STATUS_BLOCK IoStatus;

	KIrp NewIrp = IoBuildSynchronousFsdRequest(IRP_MJ_PNP, pDevice, NULL, 0, NULL, Event, &IoStatus);

	if ( !NewIrp.IsNull() )
	{
		PIO_STACK_LOCATION stack = NewIrp.NextStackLocation();

		stack->MinorFunction = IRP_MN_QUERY_INTERFACE;
		stack->Parameters.QueryInterface.InterfaceType = (LPGUID)&GUID_VBUS_WRITE_INTERFACE;
		stack->Parameters.QueryInterface.Size = sizeof(_VBUS_WRITE_INTERFACE);
		stack->Parameters.QueryInterface.Version = 1;
		stack->Parameters.QueryInterface.Interface = (PINTERFACE)&m_Interface;
		stack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

		NewIrp.Status() = STATUS_NOT_SUPPORTED;

		status = IoCallDriver(pDevice, NewIrp);
		if ( status == STATUS_PENDING )
		{
			Event.Wait(KernelMode, FALSE);
			status = IoStatus.Status;
		}
	}
	else
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
	}

	ObDereferenceObject(pDevice);

	if ( !NT_SUCCESS(status) )
	{
		t << "VRead::Create failed " << (ULONG)status << EOL;
		RtlZeroMemory(&m_Interface, sizeof(m_Interface));
	}

	return I.PnpComplete(status);
}


////////////////////////////////////////////////////////////////////////
//  VWriteDevice::Close
//
//	Routine Description:
//		Handler for IRP_MJ_CLOSE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//

NTSTATUS VWriteDevice::Close(KIrp I)
{
	t << "VWriteDevice::Close " << m_Unit << EOL;

	if ( m_Interface.Release )
	{
		m_Interface.Release(m_Interface.Context);
	}

	RtlZeroMemory(&m_Interface, sizeof(m_Interface));

    return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  VWriteDevice::Cleanup
//
//	Routine Description:
//		Handler for IRP_MJ_CLEANUP	
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//

NTSTATUS VWriteDevice::CleanUp(KIrp I)
{
	t << "Entering CleanUp, " << I;

	return I.PnpComplete(STATUS_SUCCESS);
}


////////////////////////////////////////////////////////////////////////
//  VWriteDevice::Write
//
//	Routine Description:
//		Handler for IRP_MJ_WRITE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		This routine handles write requests.
//
//		The KPnpDevice class handles restricting IRP flow
//		if the device is stopping or being removed.
//

NTSTATUS VWriteDevice::Write(KIrp I) 
{
	t << "Entering VWriteDevice::Write " << m_Unit << EOL;

	NTSTATUS status = STATUS_SUCCESS;

	I.Information() = 0;

	if ( I.WriteSize() > 0)
	{
		if ( m_Interface.Write )
		{
			ULONG nWritten = 0;

			if ( m_Interface.Write(0, I.BufferedWriteSource(), I.WriteSize(), &nWritten) )
			{
				I.Information() = nWritten;
			}
			else
			{
				status = STATUS_UNSUCCESSFUL;
			}

		}
		else
		{
			status = STATUS_NOINTERFACE;
		}
	}

	return I.PnpComplete(status);
}


