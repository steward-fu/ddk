// UsbThermometer.cpp - Cypress Thermometer implementation
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

#include "usbtherm.h"
#include "UsbThermometer.h"

#pragma hdrstop("ThermoDriver.pch")

extern KDebugOnlyTrace t;			// Global driver trace object	

NTSTATUS ReadThermoComplete(PDEVICE_OBJECT pDevice, PIRP pIrp, PVOID Context);
extern "C" BOOLEAN UseSynchronousForm=FALSE;

////////////////////////////////////////////////////////////////////////
// UsbThermometer::UsbThermometer
//
UsbThermometer::UsbThermometer(PDEVICE_OBJECT Pdo, ULONG Unit) :

   KPnpDevice(
	    Pdo,
		KUnitizedName(L"Thermometer_", Unit), 
		FILE_DEVICE_UNKNOWN,
		KUnitizedName(L"Thermometer_", Unit),
		0,
		DO_BUFFERED_IO | DO_EXCLUSIVE | DO_POWER_PAGABLE
		)
{
	t << "Entering UsbThermometer::UsbThermometer (constructor)\n";

	// Check constructor status
    if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
	    return;
	}

	// Remember our unit number
	m_Unit = Unit;

	// Initialize the lower device
	m_Usb.Initialize(this, Pdo);

	// Initialize the interface object
	m_Interface.Initialize(
		m_Usb,	// KUsbLowerDevice instance	
		0,		// Interface Number
		1,		// Configuration Value
		0		// Initial Interface Alternate Setting
		);

	// Initialize the pipe object
	m_UnusedEndpoint.Initialize(
		m_Usb,  // KUsbLowerDevice instance
		0x81	// Endpoint address for this pipe object
		);

    // Inform the base class of the lower edge device object
	SetLowerDevice(&m_Usb);

	// Initialize the PnP Policy settings to the "standard" policy
	SetPnpPolicy();

	// Initialize the Power Policy settings to the "standard" policy
	SetPowerPolicy();

	// Adjust "standard" Power Policy
	m_PowerPolicies.m_QueryPowerPolicy.m_FailIfOpen = FALSE; 
	m_PowerPolicies.m_QueryPowerPolicy.m_FailIfOutstandingIo = FALSE;

#if (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	// Usb selective suspend support

	// Initialize the work item object for use with the timer callback
	m_WorkItem.Initialize(m_pDeviceObject);

	// check if WDM version supported by Windows XP is available
	if (IoIsWdmVersionAvailable(1, 0x20)) 
		// enable selective suspend support
		m_fSSEnable = true;
	else
		m_fSSEnable = false;
#endif
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::~UsbThermometer
//
// This is the destructor for the Functional Device Object, or FDO.
//
UsbThermometer::~UsbThermometer()
{
	t << "Entering UsbThermometer::~UsbThermometer() (destructor)\n";
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::DefaultPnp 
//
// Default handler for IRP_MJ_PNP
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result returned from lower device
//
// Notes   
//		This routine just passes the IRP through to the lower device. It is 
//		the default handler for IRP_MJ_PNP. IRPs that correspond to
//		any virtual members of KpnpDevice that handle minor functions of
//		IRP_MJ_PNP and that are not overridden get passed to this routine.

NTSTATUS UsbThermometer::DefaultPnp(KIrp I) 
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Usb.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::DefaultPower 
//
// Default handler for IRP_MJ_POWER 
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result returned from lower device
//
// Notes
//		This routine just passes the IRP through to the lower device. It is 
//		the default handler for IRP_MJ_POWER.

NTSTATUS UsbThermometer::DefaultPower(KIrp I) 
{
	t << "Entering UsbThermometer::DefaultPower\n";

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
NTSTATUS UsbThermometer::SystemControl(KIrp I) 
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Usb.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::OnStartDevice 
//
// Handler for IRP_MJ_PNP subfcn IRP_MN_START_DEVICE
//
// Input
//		I			Current IRP
//
// Notes
//		This routine configures the thermometer device
//
NTSTATUS UsbThermometer::OnStartDevice(KIrp I)
{
	t << "Entering UsbThermometer::OnStartDevice\n";

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	AC_STATUS		acStatus;

	I.Information() = 0;

// The default Pnp policy has already cleared the IRP with the lower device

// Initialize the physical device object.

	acStatus = m_Usb.ActivateConfiguration( 1);	// Configuration value 1

	switch (acStatus)
	{
		case AC_SUCCESS:
			t << "USB Configuration OK\n";
			GetStringDescriptors();
			status = STATUS_SUCCESS;
			break;

		case AC_COULD_NOT_LOCATE_INTERFACE:
			t << "Could not locate the interface in the config descriptor\n";
			//Check that the Interface Number and Alternate Setting 
			//for the KUsbInterface object initialized in the constructor match
			//an interface descriptor reported by the hardware.
			break;

		case AC_COULD_NOT_PRECONFIGURE_INTERFACE:
			t << "Could not get configuration descriptor\n";
			break;

		case AC_CONFIGURATION_REQUEST_FAILED:
			t << "Board did not accept configuration URB\n";
			break;

		case AC_FAILED_TO_INITIALIZE_INTERFACE_OBJECT:
			t << "Failed to initialize interface object\n";
			break;

		case AC_FAILED_TO_LOCATE_ENDPOINT_ADDRESS:
			t << "Failed to locate endpoint address for pipe\n";
			break;

		case AC_FAILED_TO_OPEN_PIPE_OBJECT:
			//NOTE: this may or may not be fatal.  It could mean that
			//the device has an endpoint for which a KUsbPipe object has
			//not been instanced.  If the intention is to not use this pipe,
			//then it's ok.  Otherwise, there is a failure.  Clients can
			//iterate through the pipe array  in KUsbLowerDevice to check 
			//which pipes are open/closed.
			t << "Failed to open pipe object \n";
			break;

		default:
			t << "Unexpected error activating USB configuration\n";
			break;
	}

#if (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	// Usb selective suspend support

	if (status == STATUS_SUCCESS && m_fSSEnable) {

		// the device should be powered up on return from OnStartDevice
		// so schedule a callback to check if the device is idle
		// and power it down if idle

		// increment the i/o count to prevent removal before the callback 
		IncrementOutstandingRequestCount();

		LARGE_INTEGER dueTime;
		dueTime.QuadPart = -10000 * 500;	// 500 ms
		m_Timer.Set(dueTime,LinkTo(TimerCallback),this);
	}
#endif

   return status;  // base class completes the IRP
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::OnStopDevice 
//
// Handler for IRP_MJ_PNP subfunction IRP_MN_STOP_DEVICE
//
// Input
//		I			Current IRP
//
// Notes
//		The system calls this when the device is stopped.
//		The driver should release any hardware resources
//		in this routine.
//
//		The base class passes the IRP to the lower device.

NTSTATUS UsbThermometer::OnStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering UsbThermometer::OnStopDevice\n";

	m_Usb.DeActivateConfiguration();

	return status;
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::OnRemoveDevice 
//
// Handler for IRP_MJ_PNP subfunction IRP_MN_REMOVE_DEVICE
//
// Input
//		I			Current IRP
//
// Notes
//		The system calls this when the device is removed.
//		Our PnP policy will take care of 
//			(1) giving the IRP to the lower device
//			(2) detaching the PDO
//			(3) deleting the device object

NTSTATUS UsbThermometer::OnRemoveDevice(KIrp I)
{
	t << "Entering UsbThermometer::OnRemoveDevice\n";

	return STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////
// UsbThermometer::OnQueryCapabilities
//
// Handler for IRP_MJ_PNP subfunction IRP_MN_QUERY_DEVICE_CAPABILITIES
//
// Input
//		I			Current IRP
//
//	Notes
//		This function is implemented to allow surprise removal of the
//		thermometer
//
NTSTATUS UsbThermometer::OnQueryCapabilities(KIrp I)
{
	t << "Entering UsbThermometer::OnQueryCapabilities\n";

	I.CopyParametersDown();
	I.SetCompletionRoutine(LinkTo(OnQueryCapabilitiesComplete), this, TRUE, TRUE, TRUE);


	return m_Usb.PnpCall(this, I);
}


NTSTATUS UsbThermometer::OnQueryCapabilitiesComplete(KIrp I)
{
	if (I->PendingReturned)
		I.MarkPending();

	I.DeviceCapabilities()->SurpriseRemovalOK = TRUE;

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::Create 
//
//	Handler for IRP_MJ_CREATE
//
NTSTATUS UsbThermometer::Create(KIrp I)
{ 
	I.ForceReuseOfCurrentStackLocationInCalldown();
	NTSTATUS status = m_Usb.PnpCall(this, I);

	CurrentIrp() = NULL;

	return status;
}


////////////////////////////////////////////////////////////////////////
// UsbThermometer::Close 
//
//	Handler for IRP_MJ_CLOSE
//
NTSTATUS UsbThermometer::Close(KIrp I)
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	NTSTATUS status = m_Usb.PnpCall(this, I);

#if (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	// Usb selective suspend support

	if (m_fSSEnable) {
		// Check if this is the last open file handle.  If so,
		// perform selective suspend of the device
		ULONG dwOpenHandles = 1;

		if (CanDeviceSuspend(dwOpenHandles)) {

			SubmitIdleNotification(IdleNotificationCallback,this);
		}
	}
#endif

    return status;
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::Cleanup 
//
//	Handler for IRP_MJ_CLEANUP	
//
NTSTATUS UsbThermometer::CleanUp(KIrp I)
{
	return I.PnpComplete(this, STATUS_SUCCESS);
}


////////////////////////////////////////////////////////////////////////
// UsbThermometer::Cancel
//
// Cancel routine for ReadThermometerAsynch
//
VOID UsbThermometer::Cancel(KIrp I)
{
	if ( PIRP(I) == CurrentIrp() )
		CurrentIrp() = NULL;

	CancelSpinLock::Release();

	I.Information() = 0;
	I.PnpComplete(this, STATUS_CANCELLED);
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::ReadRamSynch
//
// Perform a synchronous read of the thermometer's RAM
// 
// Input
//		offset		RAM address to read
//
// Output
//		*output		data read from offset
//
NTSTATUS UsbThermometer::ReadRamSynch(UCHAR offset, UCHAR* output)
{
	t << "Entering UsbThermometer::ReadRamSynch\n";

	PURB pUrb;
	NTSTATUS status;
	UCHAR buffer[8];

	pUrb = m_Usb.BuildVendorRequest(
				buffer,							// transfer buffer
				8,								// transfer buffer size
				0,								// request reserved bits
				2,								// request
				offset,							// Value
				TRUE,							// In
				FALSE,							// Short Ok
				NULL,							// link urb
				0,								// index
				URB_FUNCTION_VENDOR_ENDPOINT	// function
				);

	if ( pUrb != NULL )
	{
		status = m_Usb.SubmitUrb(pUrb);
		
		if ( !NT_SUCCESS(status) )
		{
			t << "Synch read error: status=" << ULONG(status) 
				<< " Urb status=" << ULONG(pUrb->UrbHeader.Status) << "\n";
		}
		else
			*output = buffer[1];

		delete pUrb;
	}
	else
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
	}

	return status;
}


////////////////////////////////////////////////////////////////////////
// UsbThermometer::WriteRamSynch
//
// Peform a synchronous write of the thermometer's RAM
//
// Input
//		offset		RAM address to write
//		value		value to write to RAM
//
NTSTATUS UsbThermometer::WriteRamSynch(UCHAR offset, UCHAR value)
{
	PURB pUrb;
	NTSTATUS status;

	UCHAR buffer[8];
	memset(buffer, 0, 8);

	pUrb = m_Usb.BuildVendorRequest(
				buffer,							// transfer buffer
				8,								// transfer buffer size
				0,								// request reserved bits
				3,								// request
				offset,							// Value
				TRUE,							// In (??)
				TRUE,							// Short Ok
				NULL,							// link urb
				value,							// index
				URB_FUNCTION_VENDOR_ENDPOINT	// function
				);

	status = m_Usb.SubmitUrb(pUrb);
	delete pUrb;

	return status;
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::ReadRamAsynch
//
// Perform an asynchronous read of the thermometer's RAM
//
// Input
//		offset			RAM address to read
//		pCompRoutine	Completion routine
//
// Note: the completion routine context is a pointer to a THERMO_READ_COMPLETION_INFO structure
//
NTSTATUS UsbThermometer::ReadRamAsynch( UCHAR offset, PIO_COMPLETION_ROUTINE pCompRoutine)
{
	t << "Entering UsbThermometer::ReadRamAsynch\n";

	KIrp I = KIrp::Allocate( m_Usb.StackRequirement() );
	if ( I.IsNull() )
		return STATUS_INSUFFICIENT_RESOURCES;

	// allocate a new context structure
	THERMO_READ_COMPLETION_INFO* pCompInfo = new (NonPagedPool) THERMO_READ_COMPLETION_INFO;

	// make sure it succeeded
	if ( pCompInfo == NULL )
	{
		KIrp::Deallocate(I);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlZeroMemory(pCompInfo,sizeof(THERMO_READ_COMPLETION_INFO));

	// initialize the context structure
	pCompInfo->m_pClass = this;
	pCompInfo->m_OffsetRead = offset;

	// allocate and initialize an URB, and store the pointer in the context structure
	pCompInfo->m_pUrb = 
			m_Usb.BuildVendorRequest(
				pCompInfo->m_buffer,			// transfer buffer
				8,								// transfer buffer size
				0,								// request reserved bits
				2,								// request
				offset,							// Value
				TRUE,							// In
				FALSE,							// Short Ok
				NULL,							// link urb
				0,								// index
				URB_FUNCTION_VENDOR_ENDPOINT	// function
				);


	if ( pCompInfo->m_pUrb == NULL )
	{
		delete pCompInfo;
		KIrp::Deallocate(I);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// submit the URB to USBD
	return m_Usb.SubmitUrb(I, pCompInfo->m_pUrb, pCompRoutine, pCompInfo);
}

/////////////////////////////////////////////////////////////////
// UsbThermometer::ReadThermometer
//
// This is the synchronous version.
//
// data[0] = status
// data[1] = temperature
// data[2] = sign
// data[3] = button
//
NTSTATUS UsbThermometer::ReadThermometer(PUCHAR data)
{
	NTSTATUS status = STATUS_SUCCESS;

	data[0] = 0;

	if ( !NT_SUCCESS(status = ReadRamSynch(THERMO_DATA_OFFSET, &data[1]))) 
		return status;

	if ( !NT_SUCCESS(status = ReadRamSynch(THERMO_DATA_OFFSET + 1, &data[2]))) 
		return status;

	if ( !NT_SUCCESS(status = ReadRamSynch(THERMO_DATA_OFFSET + 2, &data[3]))) 
		return status;

	return status;
}

/////////////////////////////////////////////////////////////////
// UsbThermometer::ReadThermometerAsynch
//
// This version queues three URBs simultaneously, and collects the
// returned data in a completion routine.
//
NTSTATUS UsbThermometer::ReadThermometerAsynch(KIrp I)
{
	PVOID CI = InterlockedExchangePointer( (PVOID*)&CurrentIrp(), PVOID( PIRP(I))  );

	// Allow only one request at a time	
	if ( CI != NULL )
		return STATUS_DEVICE_BUSY;

	// initialize the status to success
	I.Status() = STATUS_SUCCESS;

	CancelSpinLock::Acquire();
	if ( I.WasCanceled() )
	{
		CancelSpinLock::Release();
		return I.PnpComplete(this, STATUS_CANCELLED);
	}

	I.SetCancelRoutine( LinkTo(Cancel) );
	CancelSpinLock::Release();

	// Read the three locations

		// Store outstanding request count in current stack loc
	*(ULONG_PTR*)&I.CurrentStackLocation()->Parameters.Others.Argument1 = 3; 

	I.MarkPending();
	ReadRamAsynch(THERMO_DATA_OFFSET,   LinkTo(ReadThermoComplete));
	ReadRamAsynch(THERMO_DATA_OFFSET+1, LinkTo(ReadThermoComplete));
	ReadRamAsynch(THERMO_DATA_OFFSET+2, LinkTo(ReadThermoComplete));

	return STATUS_PENDING;
}

/////////////////////////////////////////////////////////////////////////////////
// UsbThermometer::ReadThermoComplete
//
// Completion routine for ReadThermometerAsynch
// 
// Input:
//		pDevice			pointer to the device object
//		pIrp			pointer to the irp being completed
//		pContext		pointer to a THERMO_READ_COMPLETION_INFO
//
NTSTATUS UsbThermometer::ReadThermoComplete(KIrp I, THERMO_READ_COMPLETION_INFO* pContext)
{
	// Variable Current represents the original 'read thermometer' IRP sent
	// by the application.
	KIrp Current( CurrentIrp() );

	// If there is no current IRP, just release resources and return
	if ( !Current.IsNull() )
	{
		// find the buffer pointer in the URB
 		PUCHAR buffer = (PUCHAR)pContext->m_pUrb->UrbControlVendorClassRequest.TransferBuffer;
		PUCHAR IrpBuffer;

		if ( I.Status() == STATUS_SUCCESS )
		{
			// Dump the buffer		
			t << "Urb data[2], data[1]: " << buffer[2] << " " << buffer[1] << "\n";

			// Data from the IRP read is stored in the ioctl buffer associated with this
			// IRP:
			IrpBuffer = PUCHAR(Current.IoctlBuffer());
			IrpBuffer[0] = 0;
			IrpBuffer[pContext->m_OffsetRead-THERMO_DATA_OFFSET+1] = buffer[1];
		}
		else
		{
			t << "Error: IRP completion status = " << ULONG(I.Status()) << " URB status=" << ULONG(pContext->m_pUrb->UrbHeader.Status) << "\n";
			Current.Status() = I.Status();
		}

		// When the count reaches zero, complete the original IRP_MJ_DEVICE_CONTROL IRP that
		// was sent to this device object.

	 	if (--*(ULONG*)&Current.CurrentStackLocation()->Parameters.Others.Argument1 == 0)
		{
			NTSTATUS status = Current.Status();

			CancelSpinLock::Acquire();
			if ( ! Current.WasCanceled() )
			{
				Current.SetCancelRoutine(NULL);
				CancelSpinLock::Release();
				Current.Information() = (status == STATUS_SUCCESS) ? 4 : 0;
				Current.PnpComplete(this, status);
			}
			else
				CancelSpinLock::Release();

		// Clear the CurrentIrp pointer in the device so that we can handle the next request
		// that arrives
			CurrentIrp() = NULL;
		}
	}

	// release all resources associated with this request
	KIrp::Deallocate(I);
	if ( pContext )
	{
		delete pContext->m_pUrb;
		delete pContext;
	}

	// return indicates that system is to quit processing IRP completion
	return STATUS_MORE_PROCESSING_REQUIRED;
}

///////////////////////////////////////////////////////////////
// UsbThermometer::SetLEDBrightness
//
// Input
//	brightness		how bright it should be
//
NTSTATUS UsbThermometer::SetLEDBrightness(UCHAR brightness)
{
	NTSTATUS status = STATUS_SUCCESS;

	status = WriteRamSynch( LED_BRIGHTNESS_OFFSET, brightness );	
	
	if( NT_SUCCESS(status) )
		status = WriteRamSynch( LED_SEMAPHORE_OFFSET, 1 );	
	
	return status;
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::DeviceControl 
//
//		Handler for IRP_MJ_DEVICE_CONTROL
//
// Input
//		I		Current IRP
// 
// Output
//		None
//
// Notes:
//		This routine is the first handler for Device Control requests.
//
NTSTATUS UsbThermometer::DeviceControl(KIrp I) 
{
	NTSTATUS status;

	t << "Entering UsbThermometer::Device Control, " << I << EOL;

	switch (I.IoctlCode())
	{
	case CTL_CODE(0,1,METHOD_BUFFERED, 0):

		switch ( *(PUCHAR)I.IoctlBuffer() )
		{
		case 0xB:		// Read Thermometer data

// The driver works equally well with either ReadThermometer or 
// ReadThermometerAsynch. Both forms are provided for illustration.

			if (UseSynchronousForm)
				status = ReadThermometer(PUCHAR(I.IoctlBuffer()));
			else
				status = ReadThermometerAsynch(I);

			if ( status == STATUS_SUCCESS )
				I.Information() = 4;
			else if ( status != STATUS_PENDING )
				I.Information() = 0;
			break;

		case 0xE:	// Set LED Brightness
			status = SetLEDBrightness( ((PUCHAR)I.IoctlBuffer())[1] );
			I.Information() = 0;
			break;

		default:
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		break;

	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	if ( status != STATUS_PENDING )
		return I.PnpComplete(this, status);
	else
	{
		return STATUS_PENDING;
	}
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::OnQueryPower 
//
// handler for IRP_MJ_POWER subfunction IRP_MN_QUERY_POWER 
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	
//
// Notes
//		This routine outputs debug messages with information about the IRP.
//		The base class will handle the IRP based on power policies that were
//		set in the constructor.
//
NTSTATUS UsbThermometer::OnQueryPower(KIrp I) 
{
	NTSTATUS status = STATUS_SUCCESS;

	t	<< "Entering UsbThermometer::OnQueryPower " ;

	switch ( I.PowerStateType() ) 
	{
 	case SystemPowerState:
		t << SystemPowerStateName(I.PowerStateSetting().SystemState) << "\n" ;
		break;

 	case DevicePowerState:
		t << DevicePowerStateName(I.PowerStateSetting().DeviceState) << "\n" ;
		break;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////
// UsbThermometer::OnSetPower 
//
// handler for IRP_MJ_POWER subfunction IRP_MN_SET_POWER 
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	
//
// Notes
//		This routine outputs debug messages with information about the IRP.
//		The base class will handle the IRP based on power policies that were
//		set in the constructor.
//
NTSTATUS UsbThermometer::OnSetPower(KIrp I) 
{
	NTSTATUS status = STATUS_SUCCESS;

	t	<< "Entering UsbThermometer::OnSetPower " ;

	switch ( I.PowerStateType() ) 
	{
 	case SystemPowerState:
		t << SystemPowerStateName(I.PowerStateSetting().SystemState) << "\n" ;
		break;

 	case DevicePowerState:
		t << DevicePowerStateName(I.PowerStateSetting().DeviceState) << "\n" ;
		break;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////////////////
// UsbThermometer::OnDevicePowerUp
//
//	The base class handler for IRP_MJ_POWER subfunction IRP_MN_SET_POWER will call
//	this function for a device power IRP requesting a transition to a more powered 
//	device power state.  The driver can restore hardware context previously saved
//	handling an IRP to power down. 
// 
NTSTATUS UsbThermometer::OnDevicePowerUp(KIrp I)
{
	t << "Entering UsbThermometer::OnDevicePowerUp \n";

	//@@TODO
	//	Drivers should restore any context saved when the
	//	device was powered down.
	//

	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
// UsbThermometer::OnDeviceSleep
//
//	The base class handler for IRP_MJ_POWER subfunction IRP_MN_SET_POWER will call
//	this function for a device power IRP requesting a transition to a less powered 
//	device power state.  The driver can save hardware context that needs to be restored
//	when the device power is restored.    
// 
NTSTATUS UsbThermometer::OnDeviceSleep(KIrp I)
{
	t << "Entering UsbThermometer::OnDeviceSleep \n";

	//@@TODO
	//	Drivers should save any context that will be needed
	//	when the device is powered up.
	//

	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
// UsbThermometer::GetStringDescriptors
//
//	This function uses member GetStringDescriptor of KUsbLowerDevice to retrieve
//	the USB string descriptors from the USB thermometer.  The strings are displayed
//	in the debugger.  The GetDeviceDescriptor method is also demonstrated.  Some
//	values of the descriptor are shown in the debugger.
// 
void UsbThermometer::GetStringDescriptors(void)
{
	NTSTATUS status = STATUS_SUCCESS;

	PWCHAR String = (PWCHAR) new (NonPagedPool) WCHAR[MAXIMUM_USB_STRING_LENGTH];

	if(NULL == String)
	{
		//error during allocation
		t << "ERROR: Memory allocation error in GetStringDescriptors().\n";
		return;
	}

	USB_DEVICE_DESCRIPTOR desc;
	status = m_Usb.GetDeviceDescriptor( &desc );
	if ( !NT_SUCCESS(status) )
	{
		t << "ERROR: Could not get Device Descriptor.\n";
		delete String;
		return;
	}

	t << "Index of Manufacturer string = "	<< desc.iManufacturer		<< "\n";
	t << "Index of Product string = "		<< desc.iProduct			<< "\n";
	t << "Index of Serial Number string = " << desc.iSerialNumber		<< "\n";
	t << "Number of configurations = "		<< desc.bNumConfigurations	<< "\n";
	t << "Index of configuration string = " << m_Usb.m_Config->iConfiguration << "\n";

	t << "*****  USB Thermometer String Descriptors  *****\n";

	for(UCHAR i = 0; i <= NUM_STRING_DESCRIPTORS; i++)
	{
		RtlZeroMemory(String, MAXIMUM_USB_STRING_LENGTH * sizeof(WCHAR));

		if(NT_SUCCESS(status = m_Usb.GetStringDescriptor(
														i, 
														String, 
														MAXIMUM_USB_STRING_LENGTH, 
														0x409)))
		{
			t << "String " << i << ": " << String << "\n";
		}
		else
		{
			t << "GetStringDescriptor returns status = " << ULONG(status) << "\n";
		}
	}

	t << "\n";

	delete String;
}

// The following methods are for Usb selective suspend which is supported in WDM version >= 1.20
#if (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))

NTSTATUS UsbThermometer::SubmitIdleNotification(USB_IDLE_CALLBACK pfn, PVOID ctx)
{
	t << "Entering UsbThermometer::SubmitIdleNotification\n";

	NTSTATUS status;

	// check the current power level
	if (PowerDeviceD0 != m_CurrentDevicePowerState) 
	{
		return STATUS_POWER_STATE_INVALID;
	}

	// We will wait on an event, so we must be at PASSIVE_LEVEL
	ASSERT (KeGetCurrentIrql() == PASSIVE_LEVEL);

	m_IdleLock.Lock();

	if (InterlockedExchange(&m_IdleReqPend,1))
	{
		m_IdleLock.Unlock();
		return STATUS_DEVICE_BUSY;
	}

	PUSB_IDLE_CALLBACK_INFO pUsbIdleInfo = new (NonPagedPool) USB_IDLE_CALLBACK_INFO;

	if (!pUsbIdleInfo) 
	{
		m_IdleLock.Unlock();
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	pUsbIdleInfo->IdleCallback = IdleNotificationCallback;
	pUsbIdleInfo->IdleContext = this;

	m_pUsbIdleInfo = pUsbIdleInfo;

	// Allocate and initialize the IRP for IOCTL request
	KIrp I = KIrp::Allocate(m_Usb.StackRequirement());

	if (I.IsNull())
	{
		m_IdleLock.Unlock();
		delete pUsbIdleInfo;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Setup Irp
	I.MajorFunction(NEXT) = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	I.IoctlCode(NEXT) = IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION;
	I.IoctlInputBufferSize(NEXT) = sizeof(_USB_IDLE_CALLBACK_INFO);
	I.IoctlType3InputBuffer(NEXT) = pUsbIdleInfo;

	I.SetCompletionRoutine(LinkTo(IdleNotificationRequestComplete),this);

	m_PendingIdleIrp = (PIRP) I;
	m_FreeIdleIrpCount = 2;

	m_IdleLock.Unlock();
	
	if(!CanDeviceSuspend() || PowerDeviceD0 != m_CurrentDevicePowerState)
	{
		if (PowerDeviceD0 != m_CurrentDevicePowerState)
			status = STATUS_POWER_STATE_INVALID;
		else
			status = STATUS_DEVICE_BUSY;

		// abort
		m_IdleLock.Lock();

		m_pUsbIdleInfo = NULL;
		m_PendingIdleIrp = NULL;
		InterlockedExchange(&m_IdleReqPend, 0);

		m_IdleLock.Unlock();

		I.Deallocate(I);

		delete pUsbIdleInfo;
	}
	else {

		// increment the i/o count to prevent unloading of driver
		IncrementOutstandingRequestCount();

		status = m_Usb.PnpCall(this,I);
	}

	// Return the status
	return status;

}


NTSTATUS UsbThermometer::IdleNotificationRequestComplete(KIrp I)
{
	t << "Entering UsbThermometer::IdleNotificationRequestComplete\n";

	NTSTATUS status = I.Status();

	if (!NT_SUCCESS(status) && STATUS_NOT_SUPPORTED != status &&
		STATUS_POWER_STATE_INVALID != status)
	{
		switch (status) {

		case STATUS_INVALID_DEVICE_REQUEST:

			break;

		case STATUS_CANCELLED:

			break;

		case STATUS_DEVICE_BUSY:

			break;

		default:

			break;
		}

		IncrementOutstandingRequestCount();

		// if the IRP completes with error, request power change to D0
		status = RequestDevicePowerIrp(
			IRP_MN_SET_POWER,
			PowerDeviceD0,
			LinkTo(SetPowerComplete),
			this
			);
	}

	m_IdleLock.Lock();

	PUSB_IDLE_CALLBACK_INFO pIdleInfo = m_pUsbIdleInfo;
	m_pUsbIdleInfo = NULL;

	PIRP pIdleIrp = (PIRP) InterlockedExchangePointer((PVOID*)&m_PendingIdleIrp, NULL);
	InterlockedExchange(&m_IdleReqPend, 0);

	m_IdleLock.Unlock();

	if (pIdleInfo) delete pIdleInfo;

	if (pIdleIrp) {
		I.Deallocate(I);
	}
	else {
		// the CancelSelectiveSuspend method has taken control of 
		// the IRP

		if (0 == InterlockedDecrement(&m_FreeIdleIrpCount)) {
			I.Deallocate(I);
		}
	}

	// dec i/o count to release any waiting threads on i/o 
	// completion (Pnp removal)
	DecrementOutstandingRequestCount();

	return STATUS_MORE_PROCESSING_REQUIRED;
}


VOID UsbThermometer::SetPowerComplete(						
	UCHAR MinorFunc,						
	POWER_STATE ps,							
	PIO_STATUS_BLOCK IoStatus
	)				
{
	t << "Entering UsbThermometer::SetPowerComplete\n";

	DecrementOutstandingRequestCount();
}


bool UsbThermometer::CanDeviceSuspend(ULONG dwOpenHandles, ULONG dwNumIoReqs)
{
	if ( dwOpenHandles == m_OpenCounter.Test() && dwNumIoReqs == m_RequestCounter.Test() )
		return true;
	else
		return false;
}


VOID UsbThermometer::IdleNotificationCallback()
{
	t << "Entering UsbThermometer::IdleNotificationCallback\n";

	if (m_State.m_Started && !m_State.m_StopPending && !m_State.m_RemovePending)
	{
		IncrementOutstandingRequestCount();

		// request device power irp to lower power state
		NTSTATUS status = RequestDevicePowerIrpWait(
			IRP_MN_SET_POWER,
			PowerDeviceD2
			);

		DecrementOutstandingRequestCount();
	}
}


void UsbThermometer::CancelSelectSuspend()
{
	t << "Entering UsbThermometer::CancelSelectSuspend\n";

	PIRP pIrp = NULL;

	m_IdleLock.Lock();

	if(!CanDeviceSuspend()) {
		// Device is not idle - cancel the IRP

		pIrp = (PIRP) InterlockedExchangePointer(
			(PVOID*)&m_PendingIdleIrp, 
			NULL
			);
    }

	m_IdleLock.Unlock();

	if (pIrp) {

		IoCancelIrp(pIrp);

		if(0 == InterlockedDecrement(&m_FreeIdleIrpCount)) {

			IoFreeIrp(pIrp);
			// dec i/o count to release any waiting threads on i/o 
			// completion (Pnp removal)
			DecrementOutstandingRequestCount();
		}
	}
}


void UsbThermometer::TimerCallback(PVOID p1, PVOID p2)
{
	t << "Entering UsbThermometer::TimerCallback\n";

	ULONG dwHandlesOpen=0;  // # open file handles
	ULONG dwIoRequests=2;	// # outstanding i/o reqs.  1 additional from OnStartDevice

	if (CanDeviceSuspend(dwHandlesOpen,dwIoRequests)) {

		// Increment the i/o count to prevent removal while
		// the work item callback is pending.  This prevents
		// the class from being delete'd from memory before
		// the callback executes
		IncrementOutstandingRequestCount();

		// schedule a work item to continue processing at PASSIVE_LEVEL
		m_WorkItem.Queue(LinkTo(WorkItemCallback),this);
	}
		
	// dec the i/o count incremented in OnStartDevice
	DecrementOutstandingRequestCount();
}


void UsbThermometer::WorkItemCallback()
{
	t << "Entering UsbThermometer::WorkItemCallback\n";

	if (CanDeviceSuspend()) {

		SubmitIdleNotification(IdleNotificationCallback,this);
	}

	DecrementOutstandingRequestCount();
}


NTSTATUS UsbThermometer::Pnp(KIrp I)
{
	if ( !m_State.m_Removed && IRP_MN_START_DEVICE != I.MinorFunction())
	{
		CancelSelectSuspend();		
	}

	return KPnpDevice::Pnp(I);
}
#endif   // Usb selective suspend methods



///////////////////////////////////////////////////////////////
// PowerMinorFunctionName
//
//	This routine takes as input the IRP_MJ_POWER minor function 
//	code and returns the string describing the minor function
//
PCHAR PowerMinorFunctionName(ULONG mn)
{
	static PCHAR minors[] = {
		"IRP_MN_WAIT_WAKE",             
		"IRP_MN_POWER_SEQUENCE",           
		"IRP_MN_SET_POWER",                    
		"IRP_MN_QUERY_POWER",                  
	};

	if (mn > IRP_MN_QUERY_POWER) 
		return "<unknown minor function>";
	else
		return minors[mn];
}

///////////////////////////////////////////////////////////////
// DevicePowerStateName
//
//	This routine takes as input the DEVICE_POWER_STATE value for 
//	a device power IRP and returns a string describing the state
//
PCHAR DevicePowerStateName(DEVICE_POWER_STATE ps)
{
	static PCHAR PowerStates[] = {
		"PowerDeviceUnspecified",
		"PowerDeviceD0",
		"PowerDeviceD1",
		"PowerDeviceD2",
		"PowerDeviceD3"                  
	};

	if (ps > PowerDeviceD3) 
		return "<undefined power state>";
	else
		return PowerStates[ps];
}

///////////////////////////////////////////////////////////////
// DevicePowerStateName
//
//	This routine takes as input the SYSTEM_POWER_STATE value for 
//	a system power IRP and returns a string describing the state
//
PCHAR SystemPowerStateName(SYSTEM_POWER_STATE ps)
{
	static PCHAR PowerStates[] = {
		"PowerSystemUnspecified",
		"PowerSystemWorking",
		"PowerSystemSleeping1",
		"PowerSystemSleeping2",
		"PowerSystemSleeping3",
		"PowerSystemHibernate",
		"PowerSystemShutdown"
	};

	if (ps > PowerSystemShutdown) 
		return "<undefined power state>";
	else
		return PowerStates[ps];
}

