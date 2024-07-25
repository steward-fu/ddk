// EzUsbDevice.cpp - Implementation of EzUsbDevice device class
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1999 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#pragma warning(disable:4200)		  
#include <vdw.h>
#pragma warning (default:4710)

#include <kusb.h>
#include <KUsbBusIntf.h>

#include "EzUsbFirmware.h"
#include "EzUsbDeviceIoctl.h"
#include "EzUsbIsoTransfer.h"
#include "EzUsbIntTransfer.h"
#include "EzUsbDevice.h"

extern KDebugOnlyTrace t;				// Global driver trace object	

extern INTEL_HEX_RECORD loader[];		// Firmware for the EZUSB device
extern INTEL_HEX_RECORD EzUsbMonitor[]; // Firmware for the EZUSB device
extern INTEL_HEX_RECORD EzUsbMonitorFx2[]; // Firmware for the EZUSB-FX2 device

//=============================================================================
// EzUsbDevice Constructor
//
//		This is the constructor for the Functional Device Object, or FDO.
//		It is derived from KPnpDevice, which builds in automatic
//	    dispatching of subfunctions of IRP_MJ_POWER and IRP_MJ_PNP to
//		virtual member functions.
//
// Input
//		Pdo		Physical Device Object - this is a pointer to a system
//				device object that represents the physical device.
//
//		Unit	Unit number - this is a number to append to the device's
//				base device name to form the Logical Device Object's name
//
EzUsbDevice::EzUsbDevice(PDEVICE_OBJECT Pdo, ULONG Unit)
	:KPnpDevice(
		Pdo,
		KUnitizedName(L"Ezusb-", Unit), 
		FILE_DEVICE_UNKNOWN,
		KUnitizedName(L"Ezusb-", Unit),
		0,
		DO_DIRECT_IO | DO_POWER_PAGABLE
		),
	 m_pIsoDM(NULL),
	 m_LastFailedUrbStatus(0)
{
	t << "Entering EzUsbDevice (ctor)\n";

	// Check constructor status
    if( ! NT_SUCCESS(m_ConstructorStatus) )
	{
	    return;
	}

	// Remember our unit number
	m_Unit = Unit;

	// Initialize the lower device
	m_Lower.Initialize(this, Pdo);

	// Initialize the interface object
	m_UsbInterface.Initialize(m_Lower, 0, 1, 0); 

	// Initialize the pipe objects
	for( UCHAR i = 0; i < EZUSB_MAX_ENDPOINTS/2; i++ )
	{
		m_Endpoints[i].Initialize(m_Lower, static_cast<UCHAR>(i + 1));
		m_Endpoints[i + EZUSB_MAX_ENDPOINTS/2].Initialize(m_Lower,  static_cast<UCHAR>(i + 1 + 0x80));
	}

	//Most drivers would have KUsbPipe data members for each endpoint
	//and call the Initialize method for each member
	//m_Endpoint1_OUT.Initialize(m_Lower, 0x01); 
	//m_Endpoint2_IN .Initialize(m_Lower, 0x82); 
	//m_Endpoint2_OUT.Initialize(m_Lower, 0x02); 

	// Initialize the interrupt transfer object
	NTSTATUS Status = m_IntXfer.Initialize(
		&m_Lower,
		&m_Endpoints[EZUSB_MAX_ENDPOINTS/2 + 1]
		);

	ASSERT( STATUS_SUCCESS == Status );

	IntBuffLen = 4;
	pIntBuff = new (NonPagedPool) UCHAR[IntBuffLen];
	ASSERT( pIntBuff );

    // Inform the base class of the lower edge device object
	SetLowerDevice(&m_Lower);

	// Initialize the PnP Policy settings to the "standard" policy
	SetPnpPolicy();

	// Initialize the Power Policy settings to the "standard" policy
	SetPowerPolicy();

	// Adjust the power policy.  By default, the base class will power the
	// device down when the last file handle is closed.  For this 
	// development kit, this is not the desired behavior.  
    m_PowerPolicies.m_GeneralPolicy.m_PowerDnOnClose = FALSE; 

#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))
	Status = m_BusIntf.Initialize(m_Lower.TopOfStack());
	if (STATUS_SUCCESS == Status)
		m_fBusIntfAvailable = TRUE;
	else
		m_fBusIntfAvailable = FALSE;
#endif
}

//=============================================================================
// EzUsbDevice Destructor
//
//		This is the destructor for the Functional Device Object, or FDO.
//
EzUsbDevice::~EzUsbDevice()
{
	t << "Entering ~EzUsbDevice() (dtor)\n";

	delete pIntBuff;
}

//=============================================================================
// PNPMinorFunctionName
//
//		Return a string describing the Plug and Play minor function	
//
char *PNPMinorFunctionName(ULONG mn)
{
#ifdef 	DBG
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
		"<unknown minor function>",
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

	if (mn > IRP_MN_SURPRISE_REMOVAL) 
		return "<unknown minor function>";
	else
		return minors[mn];
#else
	return NULL;

	UNREFERENCED_PARAMETER(mn);
#endif
}

//=============================================================================
// EzUsbDevice::DefaultPnp - Default handler for IRP_MJ_PNP
//
//		This routine just passes the IRP through to the lower device. It is 
//		the default handler for IRP_MJ_PNP. IRPs that correspond to
//		any virtual members of KpnpDevice that handle minor functions of
//		IRP_MJ_PNP and that are not overridden get passed to this routine.
//
NTSTATUS EzUsbDevice::DefaultPnp(KIrp I) 
{
	t << "Entering DefaultPnp with IRP minor function="
	  << PNPMinorFunctionName(I.MinorFunction()) << "\n";

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}

//=============================================================================
// EzUsbDevice::DefaultPower - Default handler for IRP_MJ_POWER 
//
//		This routine just passes the IRP through to the lower device. It is 
//		the default handler for IRP_MJ_POWER.
//
NTSTATUS EzUsbDevice::DefaultPower(KIrp I) 
{
	t << "Entering DefaultPower\n";

	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Lower.PnpPowerCall(this, I);
}

//=============================================================================
// EzUsbDevice::SystemControl - Handler for IRP_MJ_SYSTEM_CONTROL
//
//		This routine just passes the IRP through to the next device since 
//		this driver is not a WMI provider.
//
NTSTATUS EzUsbDevice::SystemControl(KIrp I) 
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}

//=============================================================================
// EzUsbDevice::OnStartDevice - Handler for IRP_MJ_PNP / IRP_MN_START_DEVICE
//
//		This is the routine where the driver configures the USB device and
//		sets up its USB related data members.
//
//		The EZUSB device has one interface and numerous endpoints. Other USB 
//		devices likely have less endpoints, but it simply a matter of adding 
//		or removing data members to the device class in order to support more 
//		interfaces and model the USB device endpoints.
//
NTSTATUS EzUsbDevice::OnStartDevice(KIrp I)
{
	t << "Entering OnStartDevice\n";

	NTSTATUS status = STATUS_UNSUCCESSFUL;
	BOOLEAN bDownloadMonitor = FALSE;
	AC_STATUS acStatus = AC_SUCCESS;

	I.Information() = 0;

	// Since this driver supports all of the EZUSB family of chips, determine
	// the device and vendor ID of this device.  If its the default device,
	// change the alternate setting of the interface to 1 and download the 
	// 8051 debug monitor.
	USB_DEVICE_DESCRIPTOR desc;
	status = m_Lower.GetDeviceDescriptor( &desc );
	if ( !NT_SUCCESS(status) )
	{ 
		return status;
	}

	if(desc.idVendor == 0x0547 && desc.idProduct == 0x0080)
	{
		// default Anchor device, change the initial alternate setting to 1.
		m_UsbInterface.m_CurrentAlternateSetting = 1;
		bDownloadMonitor = TRUE;
	}

	if(desc.idVendor == 0x04B4 && desc.idProduct == 0x0081)
	{
		// default Anchor device, change the initial alternate setting to 1.
		m_UsbInterface.m_CurrentAlternateSetting = 1;
		bDownloadMonitor = TRUE;
	}

	// Activate Configuration value 1, the first configuration (index 0) of the device
	// described by the first configuration descriptor
	acStatus = m_Lower.ActivateConfiguration(1);

	switch (acStatus)
	{
		case AC_SUCCESS:
			t << "USB Configuration OK\n";

			if(TRUE	== bDownloadMonitor)
			{
				status = DownloadFirmware();
			}
			else
			{
				status = STATUS_SUCCESS;
			}

			//This is the Product ID for Walter Oney's sample device
			//for testing interrupt transfers
			if(desc.idVendor == 0x0547 && desc.idProduct == 0x102D)
			{
				//Start an interrupt transfer
				m_IntXfer.StartPolling(
					pIntBuff,
					IntBuffLen,
					OnInterrupt
					);
			}

			TestBusInterface();
			break;

		case AC_COULD_NOT_LOCATE_INTERFACE:
			t << "Could not locate interface\n";
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

		case AC_FAILED_TO_GET_DESCRIPTOR:
			t << "Failed to get device descriptor\n";
			break;

		case AC_FAILED_TO_OPEN_PIPE_OBJECT:
			//NOTE: this may or may not be fatal.  It could mean that
			//the device has an endpoint for which a KUsbPipe object has
			//not been instanced.  If the intention is to not use this pipe,
			//then it's ok.  Otherwise, there could be a failure.  Clients can
			//iterate through the pipe array to check which are open/closed
			//vs. what interface info says to figure out what is going on.
			status = STATUS_SUCCESS;
			t << "Failed to open pipe object \n";
			break;

		default:
			t << "Unexpected error activating USB configuration\n";
			break;
	}

   return status;  // base class completes the IRP
}


//=============================================================================
// EzUsbDevice::TestBusInterface
//
void EzUsbDevice::TestBusInterface()
{
#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))

	if (m_fBusIntfAvailable) {
		t << "USB Bus Interface Size: " << m_BusIntf.Size() << "\n";
		t << "USB Bus Interface Version: " << m_BusIntf.Version() << "\n";
		t << "High Speed Device: " << (m_BusIntf.IsDeviceHighSpeed()?"TRUE":"FALSE") << "\n";

		USBD_VERSION_INFORMATION UsbVerInfo;
		RtlZeroMemory(&UsbVerInfo, sizeof(USBD_VERSION_INFORMATION));
		ULONG HcdCapabilities = 0;

		m_BusIntf.GetUSBDIVersion(&UsbVerInfo, &HcdCapabilities);

		t << "USBDI_Version: " << UsbVerInfo.USBDI_Version << "\n";
		t << "Supported_USB_Version: " << UsbVerInfo.Supported_USB_Version << "\n";


		ULONG TotalBW, ConsumedBW;
		NTSTATUS Status = m_BusIntf.GetBandwidth(&TotalBW,&ConsumedBW);

		if (STATUS_SUCCESS == Status) {
			t << "Total Bandwidth: " << TotalBW << "\n";
			t << "Consumed Bandwidth: " << ConsumedBW << "\n";
		}

		PWSTR HcName = NULL;
		Status = m_BusIntf.GetControllerName(HcName);

		if (STATUS_SUCCESS == Status && HcName) {

			t << "HC Name: " << HcName << "\n";
			delete HcName;
		}

		ULONG CurrentFrame;
		m_BusIntf.QueryBusTime(&CurrentFrame);

		t << "Current Frame: " << CurrentFrame << "\n";
	}
#endif
}


//=============================================================================
// EzUsbDevice::OnQueryCapabilities
//
//		This function is implemented to allow surprise removal of EzUsb so
//		that the annoying dialog is not presented to the user.
//
NTSTATUS EzUsbDevice::OnQueryCapabilities(KIrp I)
{
	t << "Entering OnQueryCapabilitiesn";

	I.CopyParametersDown();
	I.SetCompletionRoutine(LinkTo(OnQueryCapabilitiesComplete), this, TRUE, TRUE, TRUE);

	return m_Lower.PnpCall(this, I);
}


NTSTATUS EzUsbDevice::OnQueryCapabilitiesComplete(KIrp I)
{
	if (I->PendingReturned)
		I.MarkPending();

	I.DeviceCapabilities()->SurpriseRemovalOK = TRUE;

	return STATUS_SUCCESS;
}

//=============================================================================
// EzUsbDevice::DownloadFirmware
//
//		This routine will download firmware to the default USB device -
//		VendorID = 0x0547 and DeviceID = 0x0080.  It is called from
//		EzUsbDevice::OnStartDevice
//
NTSTATUS EzUsbDevice::DownloadFirmware(void)
{
	t << "Entering DownloadFirmware\n";

	NTSTATUS status = STATUS_SUCCESS;

	Reset8051(1);

	status = DownloadIntelHex(loader);
	if( !NT_SUCCESS(status) )
		return status;

	Reset8051(0);

	if (IsFx2()) {
		status = DownloadIntelHex(EzUsbMonitor);
	} 
	else {
		status = DownloadIntelHex(EzUsbMonitorFx2);
	}

	if( !NT_SUCCESS(status) )
		return status;

	Reset8051(1);
	Reset8051(0);
	
	return status;
}

//=============================================================================
// EzUsbDevice::Reset8051
//
//		This routine will set or clear the 8051 reset bit on the EZUSB chip.
//		It issues the vendor command EZUSB_LOAD_INTERNAL.  byResetBit = 1 will
//		set the bit which will hold the 8051 in reset.  byResetBit = 0 will
//		clear the bit which will start the 8051 running again.
//
NTSTATUS EzUsbDevice::Reset8051(UCHAR byResetBit)
{
	t << "Entering Reset8051\n";

	NTSTATUS status = STATUS_SUCCESS;

	PURB pUrb = m_Lower.BuildVendorRequest(
							&byResetBit,
							1,
							0,
							EZUSB_LOAD_INTERNAL,
							EZUSB_CPU_CONTROL_STATUS_REG,
							FALSE,						//default
							FALSE,						//default
							NULL,						//default
							0,							//default
							URB_FUNCTION_VENDOR_DEVICE	//default
							);

	if( NULL == pUrb )
	{
		return STATUS_NO_MEMORY;
	}

	status = m_Lower.SubmitUrb(pUrb);

	delete pUrb;

	return status;
}

//=============================================================================
// EzUsbDevice::DownloadRAM
//
//		This routine will download firmware to internal or external RAM of the
//		EZUSB.  It uses vendor commands EZUSB_LOAD_EXTERNAL or 
//		EZUSB_LOAD_INTERNAL.
//
NTSTATUS EzUsbDevice::DownloadRAM(PINTEL_HEX_RECORD pHexRecord, bool fExternal)
{
	t << "Entering DownloadRAM\n";

	NTSTATUS status = STATUS_SUCCESS;
	URB Urb;

	while( 0 == pHexRecord->Type )
	{
		if( fExternal ? 
						(! EZUSB_INTERNAL_RAM( pHexRecord->Address )) : 
						(EZUSB_INTERNAL_RAM( pHexRecord->Address ))  
		  )
		{
			m_Lower.BuildVendorRequest(
				pHexRecord->Data,
				pHexRecord->Length,
				0,
				static_cast<UCHAR>( 
					fExternal ? EZUSB_LOAD_EXTERNAL : EZUSB_LOAD_INTERNAL),
				pHexRecord->Address,
				FALSE,						//default
				FALSE,						//default
				NULL,						//default
				0,							//default
				URB_FUNCTION_VENDOR_DEVICE,	//default
				&Urb
				);

			status = m_Lower.SubmitUrb(&Urb);

			if( !NT_SUCCESS(status) )
			{
				break;
			}
		}

		pHexRecord++;
	}

	return status;
}

//=============================================================================
// EzUsbDevice::DownloadIntelHex
//
//		This routine will download firmware to the EZUSB.  
//
NTSTATUS EzUsbDevice::DownloadIntelHex(PINTEL_HEX_RECORD pHexRecord)
{
	t << "Entering DownloadIntelHex\n";

	NTSTATUS status = STATUS_SUCCESS;

	// Download external RAM 
	status = DownloadRAM(pHexRecord, true);

	if( !NT_SUCCESS(status) )
		return status;

	Reset8051(1);

	// Download internal RAM 
	status = DownloadRAM(pHexRecord, false);

	return status;
}

//=============================================================================
// EzUsbDevice::AnchorDownload
//
//		This routine will download firmware to the EZUSB.  The download is
//		accomplished by issuing an _URB_CONTROL_VENDOR_OR_CLASS_REQUEST with
//		vendor specific command EZUSB_LOAD_INTERNAL.  The buffer of firmware
//		to download (pDownloadBuffer) will be split into nTransferLength 
//		requests of size nDownloadSize.
//
NTSTATUS EzUsbDevice::AnchorDownload(
   USHORT wOffset,
   PUCHAR pDownloadBuffer,
   ULONG nDownloadSize,
   ULONG nTransferLength,
   ULONG nTransferCount
   )
{
	t << "Entering AnchorDownload\n";

	NTSTATUS status = STATUS_SUCCESS;
	URB u;
	RtlZeroMemory(&u, sizeof(URB));

	for (ULONG i = 0; i < nTransferCount; i++)
	{
		//Initialize the URB
		 m_Lower.BuildVendorRequest(
			pDownloadBuffer,
			( (i == (nTransferCount - 1)) && 
			  (nDownloadSize % nTransferLength) ) ?
			  (nDownloadSize % nTransferLength) :
			   nTransferLength,
			0,
			static_cast<UCHAR>( EZUSB_LOAD_INTERNAL ),
			static_cast<USHORT>( (i * nTransferLength) + wOffset ),
			FALSE,						//default
			FALSE,						//default
			NULL,						//default
			0,							//default
			URB_FUNCTION_VENDOR_DEVICE,	//default
			&u
			);

		status = m_Lower.SubmitUrb(&u);

		if( !NT_SUCCESS(status) )
		{
			break;
		}

		pDownloadBuffer += nTransferLength;
	}

	return status;
}

//=============================================================================
// EzUsbDevice::InternalDeviceControl - Handler for IRP_MJ_INTERNAL_DEVICE_CONTROL
//
//		This handler is implemented in case there is an upper filter driver
//		which submits URBs to USBD for this USB device.  
//
NTSTATUS EzUsbDevice::InternalDeviceControl(KIrp I)
{ 
	t << "Entering InternalDeviceControl\n";

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}

//=============================================================================
// EzUsbDevice::OnStopDevice - Handler for IRP_MJ_PNP / IRP_MN_STOP_DEVICE
//
//		The system calls this when the device is stopped.  The driver should 
//		unconfigure the USB device. 
//
NTSTATUS EzUsbDevice::OnStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering OnStopDevice\n";

	m_IntXfer.StopPolling();

	m_Lower.DeActivateConfiguration();

	return status;
	
	UNREFERENCED_PARAMETER(I);
}

//=============================================================================
// EzUsbDevice::OnRemoveDevice - Handler for IRP_MJ_PNP / IRP_MN_REMOVE_DEVICE
//
//		The system calls this when the device is removed.
//
NTSTATUS EzUsbDevice::OnRemoveDevice(KIrp I)
{
	t << "Entering OnRemoveDevice\n";

	m_IntXfer.StopPolling();

	return STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(I);
}

//=============================================================================
// EzUsbDevice::Create - Handler for IRP_MJ_CREATE
//
NTSTATUS EzUsbDevice::Create(KIrp I)
{
	t << "Entering Create\n";

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}

//=============================================================================
// EzUsbDevice::Close - Handler for IRP_MJ_CLOSE
//
NTSTATUS EzUsbDevice::Close(KIrp I)
{
	t << "Entering Close\n";

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}

//=============================================================================
// EzUsbDevice::DeviceControl - Handler for IRP_MJ_DEVICE_CONTROL
//
//		The system calls this when an application issues DeviceIoControl
//
NTSTATUS EzUsbDevice::DeviceControl(KIrp I) 
{
	t << "Entering DeviceControl\n";

	NTSTATUS status = STATUS_SUCCESS;
	PVOID pBuffer = I.IoctlBuffer();
	ULONG dwInputBufferSize = I.IoctlInputBufferSize();
	ULONG dwOutputBufferSize = I.IoctlOutputBufferSize();

	I.Information() = 0;
	I.Status() = STATUS_SUCCESS;

	switch( I.IoctlCode() )
	{

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_GET_PIPE_INFO:

		t << "IOCTL_EZUSB_GET_PIPE_INFO \n";

		if( m_UsbInterface.m_Information )
		{
			//Copy the USBD_INTERFACE_INFORMATION structure stored in the KUsbInterface object
			//to the user's buffer
			RtlCopyMemory(
				reinterpret_cast<PUCHAR>( pBuffer ),
				reinterpret_cast<PUCHAR>( m_UsbInterface.m_Information ),
				m_UsbInterface.m_Information->Length
				);

			I.Information() = m_UsbInterface.m_Information->Length;
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
		}

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_GET_DEVICE_DESCRIPTOR:
	{
		t << "IOCTL_EZUSB_GET_DEVICE_DESCRIPTOR \n";

		status = m_Lower.GetDeviceDescriptor( PUSB_DEVICE_DESCRIPTOR(pBuffer) );

		if( NT_SUCCESS(status) )
		{
			I.Information() = PUSB_DEVICE_DESCRIPTOR(pBuffer)->bLength;
		}

		break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	case IOCTL_EZUSB_GET_CONFIG_DESCRIPTOR:
	{
		t << "IOCTL_EZUSB_GET_CONFIG_DESCRIPTOR \n";

		// The KUsbLowerDevice object contains a pointer to the configuration descriptor.
		// We simply copy the requested number of bytes from it to the IRP buffer.
		RtlCopyMemory(pBuffer, m_Lower.m_Config, dwOutputBufferSize);
		I.Information() = dwOutputBufferSize;

		break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	case IOCTL_EZUSB_VENDOR_REQUEST:
	
		t << "IOCTL_EZUSB_VENDOR_REQUEST \n";

		VendorRequest( 
			reinterpret_cast<PVENDOR_REQUEST_IN>(pBuffer), 
			I.Information()
			);

		status = STATUS_SUCCESS; // This prevents error messages in EzMr when downloading firmware
		
		break;

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	case IOCTL_EZUSB_ANCHOR_DOWNLOAD2:

		t << "IOCTL_EZUSB_ANCHOR_DOWNLOAD2 \n";

		status = AnchorDownload(
					0,
					reinterpret_cast<PUCHAR>(pBuffer),
					dwInputBufferSize,
					EZUSB_ANCHOR_DOWNLOAD2_SEGMENT_SIZE,
					dwInputBufferSize / EZUSB_ANCHOR_DOWNLOAD2_SEGMENT_SIZE
					);

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	case IOCTL_EZUSB_RESET:
	{
		t << "IOCTL_EZUSB_RESET \n";

		ULONG_PTR nInfo = NULL;

		status = m_Lower.DeviceIoControl(
							IOCTL_INTERNAL_USB_RESET_PORT,
							NULL,
							0,
							NULL,
							0,
							TRUE,
							&nInfo
							);
	
		break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_RESETPIPE:
	{
		t << "IOCTL_EZUSB_RESETPIPE \n";

		ULONG dwPipeNum = *( reinterpret_cast<PULONG>(pBuffer) );

		KUsbPipe* pipe = FindPipe(dwPipeNum);

		if(NULL == pipe)
		{
			status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			status = pipe->Reset();
		}

		break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	case IOCTL_EZUSB_ABORTPIPE:
	{
		t << "IOCTL_EZUSB_ABORTPIPE \n";

		ULONG dwPipeNum = *( reinterpret_cast<PULONG>(pBuffer) );

		KUsbPipe* pipe = FindPipe(dwPipeNum);

		if(NULL == pipe)
		{
			status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			status = pipe->Abort();
		}

		break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	case IOCTL_EZUSB_SETINTERFACE:
	{
		t << "IOCTL_EZUSB_SETINTERFACE \n";

		PSET_INTERFACE_IN p = reinterpret_cast<PSET_INTERFACE_IN>(pBuffer);

		SA_STATUS saStatus = m_UsbInterface.SelectAlternate(p->alternateSetting);

		if( SA_SUCCESS != saStatus )
		{
			t << "Error SelectAlternate returns " << static_cast<ULONG>(saStatus) << "\n";
			status = STATUS_UNSUCCESSFUL;
		}
		else
		{
			status = STATUS_SUCCESS;
		}

		break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_GET_STRING_DESCRIPTOR:

		t << "IOCTL_EZUSB_GET_STRING_DESCRIPTOR \n";

		if( (dwInputBufferSize >= sizeof(GET_STRING_DESCRIPTOR_IN)) &&
			(dwOutputBufferSize > 0)
		  )
		{
			PGET_STRING_DESCRIPTOR_IN p = reinterpret_cast<PGET_STRING_DESCRIPTOR_IN>(pBuffer);
			URB u;

			UsbBuildGetDescriptorRequest(
				&u,
				sizeof(_URB_CONTROL_DESCRIPTOR_REQUEST),
				USB_STRING_DESCRIPTOR_TYPE,
				p->Index,
				p->LanguageId,
				p,
				PMDL(0),
				dwOutputBufferSize,
				NULL
				);

			status = m_Lower.SubmitUrb(&u);

			if( NT_SUCCESS(status) )
			{
				I.Information() = u.UrbControlDescriptorRequest.TransferBufferLength;
				status = STATUS_SUCCESS;
			}
			else
			{
				status = STATUS_UNSUCCESSFUL;
			}
		}
		else
		{
			status = STATUS_INVALID_PARAMETER;
		}

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_BULK_READ:

		t << "IOCTL_EZUSB_BULK_READ \n";

		status = BulkReadWrite(I);

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_BULK_WRITE:

		t << "IOCTL_EZUSB_BULK_WRITE \n";

		status = BulkReadWrite(I);

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_GET_CURRENT_FRAME_NUM:
	{
		t << "IOCTL_EZUSB_GET_CURRENT_FRAME_NUM \n";

		if (dwOutputBufferSize < sizeof(ULONG))
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		ULONG nFrameNum = m_Lower.GetCurrentFrameNumber();

		if( -1 != nFrameNum )
		{
			*(reinterpret_cast<PULONG>(pBuffer)) = nFrameNum;
			I.Information() = sizeof(ULONG);
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
		}

		break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST:

		t << "IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST \n";

		status = VendorRequest(I);

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_GET_LAST_ERROR:

		t << "IOCTL_EZUSB_GET_LAST_ERROR \n";

		if (dwOutputBufferSize >= sizeof(ULONG))
		{
			*(reinterpret_cast<PULONG>(pBuffer)) = m_LastFailedUrbStatus;
			status = STATUS_SUCCESS;
			I.Information() = sizeof(ULONG);
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
		}

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_ISO_READ:

		t << "IOCTL_EZUSB_ISO_READ \n";

        status = IsoReadWrite(I);
        I.Information() = 0;

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_ISO_WRITE:

		t << "IOCTL_EZUSB_ISO_WRITE \n";

        status = IsoReadWrite(I);
        I.Information() = 0;

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_ANCHOR_DOWNLOAD:
	{
		t << "IOCTL_EZUSB_ANCHOR_DOWNLOAD \n";

		PANCHOR_DOWNLOAD_CONTROL p = reinterpret_cast<PANCHOR_DOWNLOAD_CONTROL>(pBuffer);

		if ( dwInputBufferSize != sizeof(ANCHOR_DOWNLOAD_CONTROL) ||
			 dwOutputBufferSize == 0)
		{
			t << "Error: Invalid Parameter\n";
			status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			ULONG dwTransferCount = 
				(dwOutputBufferSize + EZUSB_ANCHOR_DOWNLOAD_SEGMENT_SIZE - 1) / 
				EZUSB_ANCHOR_DOWNLOAD_SEGMENT_SIZE;

			KMemory M(I.Mdl());

			status = AnchorDownload(
						p->Offset,
						reinterpret_cast<PUCHAR>(M.MapToSystemSpace()),
						dwOutputBufferSize,
						EZUSB_ANCHOR_DOWNLOAD_SEGMENT_SIZE,
						dwTransferCount
						);
		}

		break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

	case IOCTL_EZUSB_GET_DRIVER_VERSION:
	{
		t << "IOCTL_EZUSB_GET_DRIVER_VERSION \n";

		PEZUSB_DRIVER_VERSION p = reinterpret_cast<PEZUSB_DRIVER_VERSION>(pBuffer);

		if (dwOutputBufferSize >= sizeof(EZUSB_DRIVER_VERSION))
		{
			p->MajorVersion = EZUSB_MAJOR_VERSION;
			p->MinorVersion = EZUSB_MINOR_VERSION;
			p->BuildVersion = EZUSB_BUILD_VERSION;

			status = STATUS_SUCCESS;
			I.Information() = sizeof(EZUSB_DRIVER_VERSION);
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
		}

		break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	case IOCTL_EZUSB_START_ISO_STREAM:

		t << "IOCTL_EZUSB_START_ISO_STREAM \n";

		status = StartIsoStream(I);
		I.Information() = 0;

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	case IOCTL_EZUSB_STOP_ISO_STREAM:

		t << "IOCTL_EZUSB_STOP_ISO_STREAM \n";

		if( m_pIsoDM )
		{
			if( m_pIsoDM->IsStarted() )
			{
				status = m_pIsoDM->Stop(
							TRUE,	//wait for stop?
							10		//timeout
							);
			}
		}

		status = STATUS_SUCCESS;
		I.Information() = 0;

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	case IOCTL_EZUSB_READ_ISO_BUFFER:
	
		t << "IOCTL_EZUSB_READ_ISO_BUFFER \n";

		status = ReadIsoBuffer(I);

		break;

///////////////////////////////////////////////////////////////////////////////////////////////////
		
	default:
		// Unrecognized IOCTL request
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	// If the IRP's IOCTL handler deferred processing using some driver
	// specific scheme, the status variable is set to STATUS_PENDING.
	// In this case we simply return that status, and the IRP will be
	// completed later.  Otherwise, complete the IRP using the status
	// returned by the IOCTL handler.
	if (status == STATUS_PENDING)
	{
		return status;
	}
	else
	{	
		return I.PnpComplete(this, status);
	}
}

//=============================================================================
// EzUsbDevice::VendorRequest - Handler for IOCTL_EZUSB_VENDOR_REQUEST
//
NTSTATUS EzUsbDevice::VendorRequest(PVENDOR_REQUEST_IN pVendorRequest, ULONG_PTR& nLength)
{
	t << "Entering VendorRequest\n";

	NTSTATUS status = STATUS_SUCCESS;
	ULONG nBufferLength = pVendorRequest->wLength;
	PUCHAR pBuffer = NULL;
	BOOLEAN bIn = FALSE;

	nLength = 0;

	if(pVendorRequest->direction)
	{
		//IN transfer
		pBuffer = reinterpret_cast<PUCHAR>(pVendorRequest);
		bIn = TRUE;
	}
	else
	{
		//OUT transfer

		pBuffer = new (NonPagedPool) UCHAR[nBufferLength];

		if( pBuffer )
		{
			if( 1 == nBufferLength )
			{
				pBuffer[0] = pVendorRequest->bData;
			}
			else
			{
				PUCHAR pTemp = pBuffer;

				for( UCHAR i = 0; i < nBufferLength; i++)
				{
					*pTemp = i;
					pTemp++;
				}
			}
		}
		else
		{
			return STATUS_INSUFFICIENT_RESOURCES;
		}
	}

	PURB pUrb = m_Lower.BuildVendorRequest(
							pBuffer,
							nBufferLength,
							0,
							pVendorRequest->bRequest,
							pVendorRequest->wValue,
							bIn,
							FALSE,
							NULL,
							pVendorRequest->wIndex
							);

	if (pUrb) {
		status = m_Lower.SubmitUrb(pUrb);
	}
	else {
		status = STATUS_INSUFFICIENT_RESOURCES;
	}
	
	if( NT_SUCCESS(status) )
	{
		if(pVendorRequest->direction)
		{
			nLength = pUrb->UrbControlVendorClassRequest.TransferBufferLength;
		}
	}

	if(!pVendorRequest->direction)
	{
		delete pBuffer;
	}

	delete pUrb;

	return status;
}

//=============================================================================
// EzUsbDevice::VendorRequest - Handler for IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST
//
NTSTATUS EzUsbDevice::VendorRequest(KIrp I)
{
	t << "Entering VendorRequest\n";

	NTSTATUS status = STATUS_SUCCESS;

	//Check parameters
	if(I.IoctlInputBufferSize() != sizeof(VENDOR_OR_CLASS_REQUEST_CONTROL))
	{
		return STATUS_INVALID_PARAMETER;
	}

	PVENDOR_OR_CLASS_REQUEST_CONTROL p =
		reinterpret_cast<PVENDOR_OR_CLASS_REQUEST_CONTROL>( I.IoctlBuffer() );

	USHORT nUrbFunction = 0;

	// Determine the URB function based on input parameters
	// requestType and recepient passed down from user mode
	switch( (p->requestType << 2) | p->recepient )
	{
	case 0x04:
		nUrbFunction = URB_FUNCTION_CLASS_DEVICE;
		break;
	case 0x05:
		nUrbFunction = URB_FUNCTION_CLASS_INTERFACE;
		break;
	case 0x06:
		nUrbFunction = URB_FUNCTION_CLASS_ENDPOINT;
		break;
	case 0x07:
		nUrbFunction = URB_FUNCTION_CLASS_OTHER;
		break;
	case 0x08:
		nUrbFunction = URB_FUNCTION_VENDOR_DEVICE;
		break;
	case 0x09:
		nUrbFunction = URB_FUNCTION_VENDOR_INTERFACE;
		break;
	case 0x0A:
		nUrbFunction = URB_FUNCTION_VENDOR_ENDPOINT;
		break;
	case 0x0B:
		nUrbFunction = URB_FUNCTION_VENDOR_OTHER;
		break;
	default:
		return STATUS_INVALID_PARAMETER;
		break;
	}

	KMemory M(I.Mdl());

	PURB pUrb = m_Lower.BuildVendorRequest(
							M,
							I.IoctlOutputBufferSize(),
							0,
							p->request,
							p->value,
							p->direction,
							TRUE,
							NULL,
							p->index,
							nUrbFunction
							);

	if (pUrb) {
		status = m_Lower.SubmitUrb(pUrb);
	}
	else {
		status = STATUS_INSUFFICIENT_RESOURCES;
	}

	if( NT_SUCCESS(status) )
	{
		I.Information() = pUrb->UrbControlVendorClassRequest.TransferBufferLength;
	}

	delete pUrb;

	return status;
}

//=============================================================================
// EzUsbDevice::FindPipe 
//
//		Since the applications in the development kit determine which pipe to
//		use for a particular request to the USB device, the driver cannot be
//		hard coded with KUsbPipe objects.  Rather, the KUsbPipe object to use
//		must be determined by a search of all the pipes.  When a match is
//		found by endpoint address, this KUsbPipe object is returned.  Most
//		drivers would not use this sort of logic.  Instead a KUsbPipe object
//		would be hardcoded to perform transfers.
//
KUsbPipe* EzUsbDevice::FindPipe(ULONG nPipeNum)
{
	if (nPipeNum > m_UsbInterface.NumberOfPipes())
	{
		ASSERT( nPipeNum <= m_UsbInterface.NumberOfPipes() );
		return NULL;
	}

	UCHAR byEndpointAddress = m_UsbInterface.Pipes(nPipeNum)->EndpointAddress;

	int num = m_Lower.PipeArray().MaxInserted() + 1; //number of elements in the array

	for( int i = 0; i < num; i++ )
	{
		if( m_Lower.PipeArray()[i]->m_EndpointAddress == byEndpointAddress )
			break;
	}

	if( num == i )
	{
		t << "WARNING: No Pipe Object for Endpoint!\n";
		return NULL;
	}

	return m_Lower.PipeArray()[i];
}

//=============================================================================
// EzUsbDevice::BulkReadWrite 
//
//		This routine handles Bulk reads and writes in response to 
//		IOCTL_EZUSB_BULK_READ and IOCTL_EZUSB_BULK_WRITE.
//
NTSTATUS EzUsbDevice::BulkReadWrite(KIrp I)
{
	t << "Entering BulkReadWrite\n";

	NTSTATUS status = STATUS_SUCCESS;
	PBULK_TRANSFER_CONTROL p = reinterpret_cast<PBULK_TRANSFER_CONTROL>(I.IoctlBuffer());
	ULONG nBufferLength = I.IoctlOutputBufferSize();

	KUsbPipe* pipe = FindPipe(p->pipeNum);

	if(NULL == pipe)
	{
		t << "Error: Invalid pipe parameter\n";
		return STATUS_INVALID_PARAMETER;
	}

	if( nBufferLength > pipe->MaximumTransferSize() )
	{
		t << "Error: Invalid transfer size parameter\n";
		return STATUS_INVALID_PARAMETER;
	}

	KMemory M( I.Mdl() );

	PURB pUrb = pipe->BuildBulkTransfer(
						M,
						nBufferLength,
						static_cast<BOOLEAN>( USB_ENDPOINT_DIRECTION_IN(pipe->EndpointAddress()) ),
						NULL,
						TRUE
						);

	if (pUrb) {
		status = pipe->SubmitUrb(pUrb);
	}
	else {
		status = STATUS_INSUFFICIENT_RESOURCES;
	}

	if( STATUS_SUCCESS == status )
	{
		I.Information() = pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength;
	}

	delete pUrb;

	return status;
}

//=============================================================================
//	EzUsbDevice::OnDevicePowerUp 
//
//		Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//		for a request to go to power on state from low power state
//		This routine runs at dispatch level.
//	
NTSTATUS EzUsbDevice::OnDevicePowerUp(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering OnDevicePowerUp\n";

	return status;

	UNREFERENCED_PARAMETER(I);
}

//=============================================================================
// DevicePowerStateName
//
//		This routine takes as input the DEVICE_POWER_STATE value for 
//		a device power IRP and returns a string describing the state
//
PCHAR DevicePowerStateName(DEVICE_POWER_STATE ps)
{
#ifdef _DEBUG
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
#else
	return NULL;

	UNREFERENCED_PARAMETER(ps);
#endif
}

//=============================================================================
//	EzUsbDevice::OnDeviceSleep 
//
//		Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//		for a request to go to a low power state from a high power state
//		This routine runs at passive level.
//	
NTSTATUS EzUsbDevice::OnDeviceSleep(KIrp I)
{
	t << "Entering OnDeviceSleep\n";

	NTSTATUS status = STATUS_SUCCESS;

	t << DevicePowerStateName(I.PowerStateSetting().DeviceState) << "\n" ;

	return status;
}

//=============================================================================
//	EzUsbDevice::IsoStreamCompleteCallback 
//
//		This callback routine is called when an Isochronous stream is stopped.
//		All Isochronous URBs have completed.  It is called from a completion
//		routine so assume at DISPATCH_LEVEL.
//
VOID __stdcall EzUsbDevice::IsoStreamCompleteCallback(PVOID pContext)
{
	t << "Entering IsoStreamCompleteCallback\n";

	EzUsbDevice* p = (EzUsbDevice*)pContext;

	ASSERT( p );
	if( p )
	{
		//Since the Iso Transfer is completed, delete the Iso Transfer Object
		delete p->m_pIsoDM;
		p->m_pIsoDM = NULL;
		//Matching call of IncrementOutstandingRequestCount in StartIsoStream
		p->DecrementOutstandingRequestCount();
	}
}

//=============================================================================
// EzUsbDevice::StartIsoStream - Handler for IOCTL_EZUSB_START_ISO_STREAM
//
//		This starts continuous Isochronous Reads from the USB device.  The
//		data is stored in a buffer managed by the driver.  The data is 
//		accessible to user mode by IOCTL_EZUSB_READ_ISO_BUFFER.  The 
//		continuous Isochronous Reads are stopped either on error from the 
//		device or on request by IOCTL_EZUSB_STOP_ISO_STREAM.
//
NTSTATUS EzUsbDevice::StartIsoStream(KIrp I)
{
	t << "Entering StartIsoStream\n";

	NTSTATUS status = STATUS_SUCCESS;
	PISO_TRANSFER_CONTROL p = reinterpret_cast<PISO_TRANSFER_CONTROL>(I.IoctlBuffer());

	if( ! NT_SUCCESS(status = CheckIsoParameters(p)) )
	{
		return status;
	}

	if( m_pIsoDM && m_pIsoDM->IsStarted() )
	{
		return STATUS_DEVICE_BUSY;
	}

	USHORT nPacketSize = static_cast<USHORT>( p->PacketSize );
	KUsbPipe* pPipe = FindPipe(p->PipeNum);

	if( NULL == pPipe )
	{
		ASSERT( NULL != pPipe );
		return STATUS_INVALID_PARAMETER;
	}

	m_pIsoDM = new (NonPagedPool) EzUsbDriverManagedIsoTransfer(
		*pPipe,
		nPacketSize * p->PacketCount,			//Size of data Buffer
		nPacketSize,			
		static_cast<UCHAR>(p->FramesPerBuffer),
		static_cast<UCHAR>(p->BufferCount),
		p->PacketCount							//Size of Packet Buffer
		);

	if( NULL == m_pIsoDM )
	{
		ASSERT( NULL != m_pIsoDM );
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	status = m_pIsoDM->Start(
		IsoStreamCompleteCallback,
		this
		);
	
	if( NT_SUCCESS(status) )
	{
		//This will prevent removal
		IncrementOutstandingRequestCount();
	}

	return status;
}

//=============================================================================
// EzUsbDevice::ReadIsoBuffer - Handler for IOCTL_EZUSB_READ_ISO_BUFFER
//
//		This routine will read data stored in the driver from continuous 
//		Isochronous reads.
//
NTSTATUS EzUsbDevice::ReadIsoBuffer(KIrp I)
{
	t << "Entering ReadIsoBuffer\n";

	//Check input parameters
	PISO_TRANSFER_CONTROL p = (PISO_TRANSFER_CONTROL)I.IoctlBuffer();

	if( NULL == p )
	{
		t << "Error: Invalid parameter \n";
		ASSERT( NULL != p );
		return STATUS_INVALID_PARAMETER;
	}

	if(I.IoctlInputBufferSize() != sizeof(ISO_TRANSFER_CONTROL))
	{
		t << "Error: Invalid parameter \n";
		I.Information() = 0; 
		return STATUS_INVALID_PARAMETER;
	}

	if( I.IoctlOutputBufferSize() !=
			p->PacketCount * 
			(p->PacketSize + sizeof(USBD_ISO_PACKET_DESCRIPTOR))
	  )
	{
		t << "Error: OutputBuffer too small \n";
		I.Information() = 0; 
		return STATUS_INVALID_PARAMETER;
	}

	if(NULL == m_pIsoDM)
	{
		I.Information() = 0; 
		return STATUS_UNSUCCESSFUL;
	}

	//done error checking.  Perform the operation

	KMemory M(I.Mdl());
	PVOID ptr = M.MapToSystemSpace();
	ASSERT( ptr );

	ULONG dwDataBytesToRead = p->PacketCount * p->PacketSize;

	ULONG dwDataBytesRead = m_pIsoDM->ReadBuffer(
										ptr,
										dwDataBytesToRead
										);

	if( ! p->PacketSize )
	{
		ASSERT( p->PacketSize );
		return STATUS_INVALID_PARAMETER;
	}

	ULONG dwDescriptorsToRead = dwDataBytesRead / p->PacketSize;

	ULONG dwDescriptorsRead = m_pIsoDM->ReadDescriptorBuffer(
											reinterpret_cast<PUCHAR>(ptr) + dwDataBytesRead, 
											dwDescriptorsToRead
											);

	ULONG dwDescrBytesRead = dwDescriptorsRead * sizeof(USBD_ISO_PACKET_DESCRIPTOR);

	I.Information() = dwDataBytesRead + dwDescrBytesRead;
	return STATUS_SUCCESS;
}

//=============================================================================
// EzUsbDevice::CheckIsoParameters 
//
//		This routine performs some error checking on the PISO_TRANSFER_CONTROL
//		passed down from user mode to control Isochronous transfers
//
NTSTATUS EzUsbDevice::CheckIsoParameters(PISO_TRANSFER_CONTROL p)
{
	if( NULL == p )
	{
		ASSERT( NULL != p );
		return STATUS_INVALID_PARAMETER;
	}
	
	if( p->PipeNum > m_UsbInterface.NumberOfPipes() )
	{
		t << "Error: Invalid pipe parameter\n";
		return STATUS_INVALID_PARAMETER;
	}

	PUSBD_PIPE_INFORMATION pPipeInfo = m_UsbInterface.Pipes(p->PipeNum);

	if (!(pPipeInfo->PipeType == UsbdPipeTypeIsochronous))
	{
		t << "Error: Invalid pipe parameter\n";
		return STATUS_INVALID_PARAMETER;
	}

	USBD_PIPE_HANDLE PipeHandle = pPipeInfo->PipeHandle;

	if(!PipeHandle)
	{
		t << "Error: Invalid pipe parameter\n";
		return STATUS_UNSUCCESSFUL;
	}

	//Check that the FramesPerBuffer + BufferCount parameters are valid.
	//This function enforces that PacketCount must be zero
	//modulus (FramesPerBuffer * BufferCount).
	if( p->PacketCount % (p->FramesPerBuffer * p->BufferCount) )
	{
		t << "Error: Invalid parameter\n";
		return STATUS_INVALID_PARAMETER;
	}

	return STATUS_SUCCESS;
}

//=============================================================================
// EzUsbDevice::IsoReadWrite - Handler for IOCTL_EZUSB_ISO_READ and
//								   IOCTL_EZUSB_ISO_WRITE
//
//		This starts Isochronous Transfers to/from the USB device.  
//		The data buffer is split into multiple URB's.  A feature of the kit is
//		that the USBD_ISO_PACKET_DESCRIPTOR's are made available to the 
//		application for analysis.
//
NTSTATUS EzUsbDevice::IsoReadWrite(KIrp I)
{
	t << "Entering IsoReadWrite\n";

	PISO_TRANSFER_CONTROL p = (PISO_TRANSFER_CONTROL) I.IoctlBuffer();

	NTSTATUS status = STATUS_SUCCESS;

	if( ! NT_SUCCESS(status = CheckIsoParameters(p)) )
	{
		return status;
	}

	USHORT nPacketSize = static_cast<USHORT>( p->PacketSize );

	// Check that the caller has provided a buffer large enough for
	// all of the Isochronous data and all of the Iso Packet Descriptors
	// (1 descriptor for each packet transferred)
	if( I.IoctlOutputBufferSize() < (p->PacketCount * 
							(nPacketSize + sizeof(USBD_ISO_PACKET_DESCRIPTOR)))
	  )
	{
		t << "Error: OutputBuffer too small \n";
		return STATUS_UNSUCCESSFUL;
	}

	// Initiate the transfers

	//Obtain a KUsbPipe object for the transfer
	KUsbPipe* pPipe = FindPipe(p->PipeNum);
	//Instance a KMemory object for the Mdl
	KMemory M(I.Mdl());

	//Obtain a system virtual address for the Isochronous packet descriptor buffer
	PVOID ptr = reinterpret_cast<PUCHAR>(M.MapToSystemSpace()) + p->PacketCount * nPacketSize;

	//Instance an object to manage the transfer
	EzUsbIsoTransfer* pIsoTransfer = new (NonPagedPool) EzUsbIsoTransfer(
		M,								//KMemory object
		nPacketSize * p->PacketCount,	//Size of data Buffer in bytes
		*pPipe,							//KUsbPipe object
		nPacketSize,					//USHORT PacketSize = 0,
		UCHAR(p->FramesPerBuffer),		//UCHAR PacketsPerUrb = USB_MAX_NUM_ISO_PACKETS_IN_AN_URB,
		(PUCHAR) ptr,					//pointer to descriptor buffer
		UCHAR(p->BufferCount)			//UCHAR NumUrbs = 2
		);

	if( NULL == pIsoTransfer )
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	if( STATUS_SUCCESS == pIsoTransfer->Status() )
	{
		//start the transfer(s) using the synchronous form 
		status = pIsoTransfer->Start();
	}

	delete pIsoTransfer ;

	I.Information() = 0;
	return status;
}


//=============================================================================
// EzUsbDevice::OnInterrupt 
//
//		This routine is called when the USB device interrupts.
//
ON_INTERRUPT_STATUS EzUsbDevice::OnInterrupt(
	PURB pUrb,
	PVOID pContext
	)
{
	t << "Entering OnInterrupt\n";

	ASSERT( pUrb );
	if( pUrb )
	{
		t << *pUrb;
	}

	return ON_INTERRUPT_SUCCESS;

	UNREFERENCED_PARAMETER(pContext);
}


//=============================================================================
// EzUsbDevice::IsFx2 
//
//		This routine checks if the hardware is the FX2 model.  The device 
//	must implement the vendor specific IN SETUP command 0xAC, which returns 1 
//	if the chip is FX2 and 0 otherwise.
//
bool EzUsbDevice::IsFx2()
{
	t << "Entering IsFx2\n";

	bool bRet = false;

	NTSTATUS status = STATUS_SUCCESS;
	UCHAR Buffer;

	PURB pUrb = m_Lower.BuildVendorRequest(
							&Buffer,		//PUCHAR Buffer
							1,				//Buffer Length
							0,				//RequestTypeReservedBits
							EZUSB_ISFX2,	//Request
							0				//Value							
							);

	if( pUrb )
	{
		// submit the URB with a timeout since not all 
		// devices will implement this command.  
		ULONG Timeout = 100;  // Timeout in milliseconds
		status = m_Lower.SubmitUrb(pUrb,NULL,NULL,Timeout);
		delete pUrb;

		if (STATUS_SUCCESS == status && 1 == Buffer)
			bRet = true;
	}

	return bRet;
}
