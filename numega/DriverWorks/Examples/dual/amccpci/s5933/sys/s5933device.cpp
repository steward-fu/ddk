// S5933Device.cpp
// Implementation of S5933Device device class

#include <vdw.h>

#include "S5933.h"
#include "S5933Device.h"
#include "..\S5933ioctl.h"

#pragma warning(disable:4065) // Allow switch statement with no cases

extern	KTrace	t;			// Global driver trace object	

////////////////////////////////////////////////////////////////////////
// S5933Device Destructor
//
//		This is the destructor for the S5933Device
//
// Input
//		None
//
// Output
//		None
//
// Notes
//		Our resources are disconnected and released by the data member m_S5933
//		upon its destruction.
//
//		Although the use of SAFE_DESTRUCTORS in the class definition cures
//		improper emission of the destructor into the INIT section most of the
//		time, certain rare cases can still cause improper behavior.  To avoid
//		these cases, the destructor	must preceed the INIT section, causing it
//		to be referenced first by the default compiler section.
//
S5933Device::~S5933Device()
{
	
	// Delete our dummy device.
	delete m_pDummyDevice;
}

#if ! _WDM_ // THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS AN NT4 DRIVER
//////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

////////////////////////////////////////////////////////////////////////////////
// S5933Device Constructor
//
//		The device constructor is typically responsible for allocating
//		any physical resources that are associated with the device.
//
// Input
//		Unit	Unit number - this is a number to append to the device's
//				base device name to distinguish multiple units of this
//				device type.
//
// Output
//		None
//
// Notes
//		The device constructor often reads the registry to setup
//		various configurable parameters.

S5933Device::S5933Device(ULONG Unit) :
	KDevice(),
	m_S5933(),
	m_pDummyDevice(NULL),
	m_NvramSize(0),
	m_ReadCancelSync(),
	m_ReadWaitForCancel(),
	m_ReadCancelFinished(SynchronizationEvent),
	m_WriteCancelSync(),
	m_WriteWaitForCancel(),
	m_WriteCancelFinished(SynchronizationEvent),
	m_PostMsgCancelSync(),
	m_PostMsgWaitForCancel(),
	m_PostMsgCancelFinished(SynchronizationEvent),
	m_RetrieveMsgCancelSync(),
	m_RetrieveMsgWaitForCancel(),
	m_RetrieveMsgCancelFinished(SynchronizationEvent)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create device S5933Device unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;
		return;
	}
	m_Unit = Unit;

	// Initialize work items used in cancelation logic
	m_WriteWaitForCancel.Initialize(*this);
	m_ReadWaitForCancel.Initialize(*this);
	m_PostMsgWaitForCancel.Initialize(*this);
	m_RetrieveMsgWaitForCancel.Initialize(*this);

	KUnitizedName* pRegistryPath;
	pRegistryPath = CreateRegistryPath(L"S5933Device", Unit);
	if (pRegistryPath == NULL)
	{
		// Error, cannot allocate memory for registry path
		t << "Failed to create registry path\n";
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}

	// Load data members from the registry
	LoadRegistryParameters();

	// Create a dummy device to be used for DMA writes.  This is necessary
	// to allow independent full duplex DMA operation since a device's
	// WaitContextBlock (WCB, which is part of the device object itself)
	// is used to queue the device on a DMA adapter object.  To have
	// two independent WCBs for use on two independent adapter objects,
	// we need a dummy device, otherwise the lists the adapter objects maintain
	// based on the device's WCB will become corrupt.
	m_pDummyDevice = new (NonPagedPool) KDevice(NULL, FILE_DEVICE_UNKNOWN,
		NULL, 0, DO_DIRECT_IO
		);
	m_ConstructorStatus = m_pDummyDevice->ConstructorStatus();
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create DMA dummy device member of S5933Device unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;
		
		// Clean up and exit
		delete pRegistryPath;
		return;
	}

	// Claim and initialize resources assigned to this device

	// Initialize the KS5933PciIoMapped class instance.  For the S5933DK1
	// evaluation board, 0 is the index of the Base Address Register
	// contiaining the information for the Io range for the control
	// and status registers of the S5933 PCI chip.
	m_ConstructorStatus = m_S5933.Initialize(
		static_cast<USHORT>(m_VendorID),
		static_cast<USHORT>(m_DeviceID),
		(KPciSlot *) NULL,
		this,
		*pRegistryPath,
		0,
		m_MaxDmaReadLength,
		m_MaxDmaWriteLength,
		static_cast<BOOLEAN>(m_NvramIsSerial),
		static_cast<USHORT>(m_NvramPageSize),
		TRUE
		);

	// We are now done with the registry path unitized name, so delete it
	delete pRegistryPath;

	if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to initialize S5933Device unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;
		return;
	}
	
	// Next using the KPciConfiguration data member embedded in our m_S5933
	// data member, initialize the pass thru region of memory.  First, we will
	// create an instance of KResourceAssignment that we can use to get the length
	// of the region.  One is the index of the BAR for the pass thru Io port range
	// of the S5933DK1. 
	KResourceAssignment theIoAssignment(
		m_S5933.m_pResourceList,
		CmResourceTypePort,
		1
		);

	m_ConstructorStatus = theIoAssignment.Status();
	if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to find pass thru region for S5933Device unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;

		// Clean up and exit
		m_S5933.Invalidate();
		return;
	}
	
	m_ConstructorStatus = m_PassThruRange.Initialize(
		PCIBus,
		m_S5933.m_PciConfig.Bus(),
		m_S5933.m_PciConfig.ReadBaseAddress(1) & PCI_BAR_IO_ADDRESS_MASK,
		theIoAssignment.Length(),
		TRUE
		);

	if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to initialize pass thru region for S5933Device unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;

		// Clean up and exit
		m_S5933.Invalidate();
		return;
	}

	// Determine the size of the Nvram attached to the S5933.  Note since not all
	// implementations will use a Nvram, this step is not automatically performed
	// by the S5933 class.
	UCHAR blocks;
	m_ConstructorStatus = m_S5933.ReadNvram(&blocks, KS59XX_NVRAM_SIZE_OFFSET);
	if (NT_SUCCESS(m_ConstructorStatus))
	{
		m_NvramSize = (512 * blocks);
	}
	else
	{
		t << "Failed to read Nvram size from Nvram" << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;

		// Clean up and exit
		m_PassThruRange.Invalidate();
		m_S5933.Invalidate();
	}

	// Setup the advance condition for Addon interface accesses to the FIFO so that it
	// advances when byte 2 is written.  This must be done for use in conjunction with
	// the AMCC S5933DK1 kit, since the ISA card that accesses the S5933 registers will
	// do so from a 16 bit bus, and will access the low 16 bit word first and then the
	// high 16 bit word.  We don't want the FIFO to advance until the second access.
	m_S5933.SetFifoBusProperties( KS59XX_NO_CONVERSION, 0, 2 );

}

//////////////////////////////////////////////////////////////////////
// End INIT section code
//
#pragma code_seg()

#else // ! _WDM_ THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS A WDM DRIVER

////////////////////////////////////////////////////////////////////////////////
// S5933Device Constructor
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
// Output
//		None   
//
// Notes   
//		The object being constructed contains a data member (m_Lower) of type
//		KPnpLowerDevice. By initializing it, the driver binds the FDO to the
//		PDO and creates an interface to the upper edge of the system class driver.

S5933Device::S5933Device(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KPnpDevice(
		Pdo,
		NULL
		),
	m_S5933(),
	m_pDummyDevice(NULL),
	m_NvramSize(0),
	m_ReadCancelSync(),
	m_ReadWaitForCancel(),
	m_ReadCancelFinished(SynchronizationEvent),
	m_WriteCancelSync(),
	m_WriteWaitForCancel(),
	m_WriteCancelFinished(SynchronizationEvent),
	m_PostMsgCancelSync(),
	m_PostMsgWaitForCancel(),
	m_PostMsgCancelFinished(SynchronizationEvent),
	m_RetrieveMsgCancelSync(),
	m_RetrieveMsgWaitForCancel(),
	m_RetrieveMsgCancelFinished(SynchronizationEvent)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create device S5933Device unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;
		return;
	}
	m_Unit = Unit;

	// Initialize work items used in cancelation logic
	m_WriteWaitForCancel.Initialize(Pdo);
	m_ReadWaitForCancel.Initialize(Pdo);
	m_PostMsgWaitForCancel.Initialize(Pdo);
	m_RetrieveMsgWaitForCancel.Initialize(Pdo);

	// Load data members from the registry
	LoadRegistryParameters();

	// Create a dummy device to be used for DMA writes.  This is necessary
	// to allow independent full duplex DMA operation since a device's
	// WaitContextBlock (WCB, which is part of the device object itself)
	// is used to queue the device on a DMA adapter object.  To have
	// two independent WCBs for use on two independent adapter objects,
	// we need a dummy device, otherwise the lists the adapter objects maintain
	// based on the device's WCB will become corrupt.
	m_pDummyDevice = new (NonPagedPool) KDevice(NULL, FILE_DEVICE_UNKNOWN,
		NULL, 0, DO_DIRECT_IO
		);
	m_ConstructorStatus = m_pDummyDevice->ConstructorStatus();
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create DMA dummy device member of S5933Device unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;
		return;
	}

	// Initialize the lower device
	m_Lower.Initialize(this, Pdo);

    // Inform the base class of the lower edge device object
	SetLowerDevice(&m_Lower);

	// Initialize the PnP Policy settings to the "standard" policy
	SetPnpPolicy();

	// Initialize the Power Policy settings to the "standard" policy
	SetPowerPolicy();

	m_PowerPolicies.m_GeneralPolicy.m_PowerDnOnClose = FALSE;
}


////////////////////////////////////////////////////////////////////////
// PNPMinorFunctionName
//
//		Return a string describing the Plug and Play minor function	
//
// Input	
//		mn		Minor function code
//
// Output	
//		char *	Ascii name of minor function
//
// Notes	
//		This function is used for tracing the IRPs.  Remove the function,
//		or conditionalize it for debug-only builds, if you want to save
//		space in the driver image.
	
char *PNPMinorFunctionName(ULONG mn)
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
}

////////////////////////////////////////////////////////////////////////
// DefaultPnp (member of S5933Device)
//
//		Default handler for IRP_MJ_PNP
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

NTSTATUS S5933Device::DefaultPnp(KIrp I) 
{
	t << "Entering S5933Device::DefaultPnp with IRP minor function="
	  << PNPMinorFunctionName(I.MinorFunction()) << EOL;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}


////////////////////////////////////////////////////////////////////////
// DefaultPower (member of S5933Device)
//
//		Default handler for IRP_MJ_POWER 
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

NTSTATUS S5933Device::DefaultPower(KIrp I) 
{
	t << "Entering S5933Device::DefaultPower\n";

	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Lower.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
//  S5933Device::SystemControl
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

NTSTATUS S5933Device::SystemControl(KIrp I) 
{
	t << "Entering S5933Device::SystemControl\n";

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////
// OnStartDevice (member of S5933Device)
//
//		Handler for IRP_MJ_PNP subfcn IRP_MN_START_DEVICE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//		Initialize the physical device.  Typically, the driver initializes
//		physical resources here.  Call I.AllocatedResources() for a list
//		of the raw resources that the system has assigned to the device,
//		or I.TranslatedResources() for the translated resource list.

NTSTATUS S5933Device::OnStartDevice(KIrp I)
{
	t << "Entering S5933Device::OnStartDevice\n";

   NTSTATUS status = STATUS_SUCCESS;

	I.Information() = 0;

	// The default Pnp policy has already cleared the IRP with the lower device
	// Initialize the physical device object.

	// First get the list of translated resources from the IRP
	PCM_RESOURCE_LIST pTranslatedList = I.TranslatedResources();
    PCM_RESOURCE_LIST pRawList = I.AllocatedResources();

	// Initialize the KS5933PciIoMapped class instance.  For the S5933DK1
	// evaluation board, 0 is the index of the Base Address Register
	// contiaining the information for the Io range for the control
	// and status registers of the S5933 PCI chip.
	status = m_S5933.Initialize(
		m_Lower.PDO(),
		pTranslatedList,
		pRawList,
		0,
		m_MaxDmaReadLength,
		m_MaxDmaWriteLength,
		static_cast<BOOLEAN>(m_NvramIsSerial),
		static_cast<USHORT>(m_NvramPageSize),
		TRUE
		);

	if ( !NT_SUCCESS(status) )
	{
		t << "Failed to initialize S5933Device" << " status " << (ULONG) status << EOL;
		return status;
	}
	
	// Next using the ResourceList and PciConfiguration data members embedded in
	// our m_S5933 data member, initialize the pass thru region of memory.  One is
	// the index of the BAR for the pass thru Io port range of the S5933DK1. 
	status = m_PassThruRange.Initialize(
		pTranslatedList,
		pRawList,
		m_S5933.m_PciConfig.BaseAddressIndexToOrdinal(1)
		);

	if ( !NT_SUCCESS(status) )
	{
		t << "Failed to initialize pass thru region for S5933Device" << " status " << (ULONG) status << EOL;

		// Clean up and exit
		m_S5933.Invalidate();
		return status;
	}

	// Determine the size of the Nvram attached to the S5933.  Note since not all
	// implementations will use a Nvram, this step is not automatically performed
	// by the S5933 class.
	UCHAR blocks;
	status = m_S5933.ReadNvram(&blocks, KS59XX_NVRAM_SIZE_OFFSET);
	if (NT_SUCCESS(status))
	{
		m_NvramSize = (512 * blocks);
	}
	else
	{
		t << "Failed to read Nvram size from Nvram status " << (ULONG) status << EOL;

		// Clean up and exit
		m_PassThruRange.Invalidate();
		m_S5933.Invalidate();
	}

	// Setup the advance condition for Addon interface accesses to the FIFO so that it
	// advances when byte 2 is written.  This must be done for use in conjunction with
	// the AMCC S5933DK1 kit, since the ISA card that accesses the S5933 registers will
	// do so from a 16 bit bus, and will access the low 16 bit word first and then the
	// high 16 bit word.  We don't want the FIFO to advance until the second access.
	m_S5933.SetFifoBusProperties( KS59XX_NO_CONVERSION, 0, 2 );

    // The base class will handle completion

	return status;
}


////////////////////////////////////////////////////////////////////////
// OnStopDevice (member of S5933Device)
//
//		Handler for IRP_MJ_PNP subfcn IRP_MN_STOP_DEVICE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//		The system calls this when the device is stopped.
//		The driver should release any hardware resources
//		in this routine.
//
//		The base class passes the irp to the lower device.

NTSTATUS S5933Device::OnStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering S5933Device::OnStopDevice\n";

	// Device stopped.  Free I/O range and S5933 device.
	m_PassThruRange.Invalidate();
	m_S5933.Invalidate();

	return status;
	
	// The following macro simply allows compilation at Warning Level 4
	UNREFERENCED_PARAMETER(I);
}


////////////////////////////////////////////////////////////////////////
// OnRemoveDevice (member of S5933Device)
//
//		Handler for IRP_MJ_PNP subfcn IRP_MN_REMOVE_DEVICE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//		The system calls this when the device is removed.
//		Our PnP policy will take care of 
//			(1) giving the IRP to the lower device
//			(2) detaching the PDO
//			(3) deleting the device object

NTSTATUS S5933Device::OnRemoveDevice(KIrp I)
{
	t << "Entering S5933Device::OnRemoveDevice\n";

	// Device stopped.  Free I/O range and S5933 device.
	m_PassThruRange.Invalidate();
	m_S5933.Invalidate();

	return STATUS_SUCCESS;

	// The following macro simply allows compilation at Warning Level 4
	UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  OnDevicePowerUp (member of S5933Device)
//
//		Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//		for a request to go to power on state from low power state
//
//	Input:
//		I - IRP containing POWER request
//
//	Output:
//		NTSTATUS - Status code indicating success or failure
//
//	Notes:
//		This routine implements the OnDevicePowerUp function.
//		This function was called by the framework from the completion
//		routine of the IRP_MJ_POWER dispatch handler in KPnpDevice.
//		The bus driver has completed the IRP and this driver can now
//		access the hardware device.  
//		This routine runs at dispatch level.
//	

NTSTATUS S5933Device::OnDevicePowerUp(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering S5933Device::OnDevicePowerUp\n";

	// Restore any context to the hardware device that
	// was saved during the handling of a power down request.
	m_S5933.WriteOperationRegisters(&m_DeviceStateRegs);

	// Disconnect the interrupt before going to sleep to avoid
	// spurious interrupt problems when waking.
	m_S5933.ConnectIsr();

	return status;

	// The following macro simply allows compilation at Warning Level 4
	UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  OnDeviceSleep (member of S5933Device)
//
//		Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//		for a request to go to a low power state from a high power state
//
//	Input:
//		I - IRP containing POWER request
//
//	Output:
//		NTSTATUS - Status code indicating success or failure
//
//	Notes:
//		This routine implements the OnDeviceSleep function.
//		This function was called by the framework from the IRP_MJ_POWER 
//		dispatch handler in KPnpDevice prior to forwarding to the PDO.
//		The hardware has yet to be powered down and this driver can now
//		access the hardware device.  
//		This routine runs at passive level.
//	

NTSTATUS S5933Device::OnDeviceSleep(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering S5933Device::OnDeviceSleep\n";

	// Disconnect the interrupt before going to sleep to avoid
	// spurious interrupt problems when waking.
	m_S5933.DisconnectIsr();

	// Save any context to the hardware device that will be required 
	// during a power up request.
	m_S5933.ReadOperationRegisters(&m_DeviceStateRegs);

	return status;

	// The following macro simply allows compilation at Warning Level 4
	UNREFERENCED_PARAMETER(I);
}

#endif // ! _WDM_

////////////////////////////////////////////////////////////////////////
// LoadRegistryParams
//
//		Load parameters from the registry and store them in class
//		member variables.
//
// Input
//		None
//
// Output	
//		Member variables are updated with values read from registry
//
// Notes
//		The registry variables we load are actually global to the driver
//		and are are loaded from the registry path:
//		HKEY_LOCAL_MACHINE\
//		  SYSTEM\
//		    CurrentControlSet\
//			  Services\
//			    S5933\
//				  Parameters\
//				    <param name>
//					    
void S5933Device::LoadRegistryParameters(void)
{

	KRegistryKey Params((KDriver::DriverInstance())->RegistryPath(), L"\\Parameters");
	if (!NT_SUCCESS(Params.LastError()))
	{
		// Error, cannot open device registry key
		t << "Failed to create registry key\n";
		m_ConstructorStatus = STATUS_UNSUCCESSFUL;
		return;
	}

	// Load the registry parameters
	m_MaxDmaReadLength = 0x2000;
	Params.QueryValue(L"MaxDmaReadLength", &m_MaxDmaReadLength);
	if (!NT_SUCCESS(Params.LastError()))
		t << "Registry key load failed, m_MaxDmaReadLength is default value: [" << m_MaxDmaReadLength << "]\n";
	else
		t << "m_MaxDmaReadLength loaded from registry, resulting value: [" << m_MaxDmaReadLength << "]\n";

	m_MaxDmaWriteLength = 0x2000;
	Params.QueryValue(L"MaxDmaWriteLength", &m_MaxDmaWriteLength);
	if (!NT_SUCCESS(Params.LastError()))
		t << "Registry key load failed, m_MaxDmaWriteLength is default value: [" << m_MaxDmaWriteLength << "]\n";
	else
		t << "m_MaxDmaWriteLength loaded from registry, resulting value: [" << m_MaxDmaWriteLength << "]\n";

	m_VendorID = 0x1234;
	Params.QueryValue(L"VendorID", &m_VendorID);
	if (!NT_SUCCESS(Params.LastError()))
		t << "Registry key load failed, m_VendorID is default value: [" << m_VendorID << "]\n";
	else
		t << "m_VendorID loaded from registry, resulting value: [" << m_VendorID << "]\n";

	m_DeviceID = 0x5678;
	Params.QueryValue(L"DeviceID", &m_DeviceID);
	if (!NT_SUCCESS(Params.LastError()))
		t << "Registry key load failed, m_DeviceID is default value: [" << m_DeviceID << "]\n";
	else
		t << "m_DeviceID loaded from registry, resulting value: [" << m_DeviceID << "]\n";

	m_NvramIsSerial = 0x0;
	Params.QueryValue(L"NvramIsSerial", &m_NvramIsSerial);
	if (!NT_SUCCESS(Params.LastError()))
		t << "Registry key load failed, m_NvramIsSerial is default value: [" << m_NvramIsSerial << "]\n";
	else
		t << "m_NvramIsSerial loaded from registry, resulting value: [" << m_NvramIsSerial << "]\n";

	// Note Page size for Serial NVRAM is 1, Page size for 29C512 Flash NVRAM is 128
	m_NvramPageSize = 128;
	Params.QueryValue(L"NvramPageSize", &m_NvramPageSize);
	if (!NT_SUCCESS(Params.LastError()))
		t << "Registry key load failed, m_NvramPageSize is default value: [" << m_NvramPageSize << "]\n";
	else
		t << "m_NvramPageSize loaded from registry, resulting value: [" << m_NvramPageSize << "]\n";
}

////////////////////////////////////////////////////////////////////////
// SerialRead (member of S5933Device)
//	
//		Handler for serialized READ
//
// Input	
//		I		Current IRP
//
// Output	
//		None
//
// Notes	
//		This routine is called when the IRP is removed from the
//		queue.  This guarantees that multiple requests are
//		never processed simultaneously.
//
//		This routine is called at dispatch level.
//
void S5933Device::SerialRead(KIrp I)
{
	t << "Entering S5933Device::SerialRead, " << I;

	// Acquire the spin lock protecting our read operation so that the cancel
	// routine cannot run while we are working on the device.
	m_ReadCancelSync.Lock();

	// We want to install a new cancel routine that will allow our transfer to be
	// canceled while being processed.  Cancel policy is up to the individual
	// driver design, and this step is optional.  We might for instance setup a
	// watchdog timer instead to cancel the transfer based on a time out.
	I.SetCancelRoutine(LinkTo(CancelInProgressRead));

	// Test to see if our IRP has been canceled.
	if (I.WasCanceled())
	{
		
		// The IRP has been canceled. We must clear the cancel routine
		// we just set and determine if it has been called.  We do this
		// by checking the return value of I.SetCancelRoutine()
		PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);
		if (pLastCancel == LinkTo(CancelInProgressRead))
		{

			// If the cancel routine we set was still on set on the IRP,
			// it hasn't been called.  In this case we can dispose of the
			// IRP here.  Release our spinlock, then complete the IRP with
			// a canceled status and start the next IRP on the device.
			m_ReadCancelSync.Unlock();
			I.Status() = STATUS_CANCELLED;
			I.Information() = 0;
			m_ReadQueue.DMQ_NEXT_IRP(I);
			return;
		}
		else
		{

			// If the cancel routine has changed (has been set to NULL by the
			// system), the cancel routine has been called and will run when
			// we release the spinlock.  We must proceed in initiating our
			// action on the device (starting the DMA read) and let the cancel
			// routine handle cleaning up the device state.
			ASSERT(pLastCancel == NULL);
		}
	}

	// Now we must start the DMA read transfer on the KS5933 object using the
	// context we created.  Note that it is still safe to touch the IRP even
	// after we have set the cancel routine, since a requirement of the class is
	// that all terminal processing of the transfer including cancel processing,
	// be performed by the PKS5933_DMA_DONE_HANDLER_ROUTINE function.  Therefore we
	// are assured that the IRP will be valid until that routine is called, which will
	// be sometime after we start the transfer by calling DmaRead().  See the
	// cancel routine below for more details.
	m_S5933.DmaRead(
		this,
		I.Mdl(),
		LinkTo(SerialReadCallback),
		this
		);

	// Now release the spinlock so that cancel processing can continue if
	// necessary.
	m_ReadCancelSync.Unlock();
	return;
}

////////////////////////////////////////////////////////////////////////
// CancelInProgressRead (member of S5933Device)
//	
//		Cancel routine for in progress DMA read
//
// Input	
//		I		Current IRP
//
// Output	
//		None
//
// Notes	
//
void S5933Device::CancelInProgressRead( KIrp I )
{
	
	// Release the cancel spin lock at the correct IRQL
	CancelSpinLock::Release(I.CancelIrql());

	// Acquire and release the cancel sync spin lock.  This prevents our cancel
	// routine from running until the DMA read operation has started on the
	// device.
	m_ReadCancelSync.Lock();
	m_ReadCancelSync.Unlock();

	// Try to cancel the operation on the KS5933 object.
	m_S5933.CancelDmaRead();	

	// We will set an event indicating the cancel logic is finished.  This
	// will be used by the callback logic in determining when the
	// device state is resolved and processing of the next IRP can
	// continue. Note that we don't try to start the next IRP here.
	// Calling cancel causes our completion callback routine to be called
	// immediately with a cancelled status if the transfer has not already
	// been completed.  The IRP will be completed there.  Calling the
	// cancel DMA routine if the transfer is already completed or just
	// completing does no harm.
	m_ReadCancelFinished.Set();
}

////////////////////////////////////////////////////////////////////////
// SerialReadCallback (member of S5933Device)
//	
//		Handler for serialized READ
//
// Input	
//		state		Flags describing the state of the DMA read transfer object.
//
// Output	
//		None
//
// Notes	
//		This routine is called at dispatch level.
//		
//
VOID S5933Device::SerialReadCallback( KS5933_TRANSFER_STATE state )
{
	t << "Entering S5933Device::SerialReadCallback.";

	// We need to find out what the current IRP in the driver managed read
	// queue is.
	KIrp I = m_ReadQueue.CurrentIrp();

	// Make the IRP noncancelable.
	PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);

	// Test to see if our IRP has been canceled.  Note that this must
	// be done after setting the cancel routine to NULL.
	if (I.WasCanceled())
	{
		
		// The IRP has been canceled.
		I.Status() = STATUS_CANCELLED;

		// It may be interesting to note the amount of data transferred so far
		// even though the IRP was canceled.  We can obtain that information from
		// the state structure.
		I.Information() = state.m_BytesTransferred;

		// We must check to see if our cancel routine was called.  We do
		// this by checking the value returned when we set the cancel routine.
		if (pLastCancel == LinkTo(CancelInProgressRead))
		{

			// The cancel routine was still set on the IRP when we made it
			// noncancelable.  This means our cancel routine was not called,
			// and that the IRP must have been canceled in the very small
			// window after we made it noncancelable.  Complete the IRP here
			// and start the next IRP on the device.
			m_ReadQueue.DMQ_NEXT_IRP(I);
		}
		else
		{

			// The cancel routine had already been cleared, indicating that our
			// cancel routine was called. We will complete the IRP here after
			// cleaning up our device state. Since we want all processing to be
			// done, we must wait to make sure the cancel routine has run,
			// otherwise it might try to cancel the DMA read operation for the
			// next IRP. In order to make sure the cancel routine has run,
			// we will wait for an event to be signaled in the cancel routine.
			// Since we are in a DPC at DIRQL we can't wait here, so we will
			// schedule a work item to call NextIrp once the wait has been
			// satisfied.  Note that our work item is a member of the class, and not
			// dynamically allocated.  We must be certain that the work item only
			// gets queued once (otherwise its list entry will be corrupted).  This
			// of course will be the case since we are serializing IRPs, and won't
			// start the next IRP until the work item completes.
			m_ReadWaitForCancel.Queue(LinkTo(ReadWaitForCancel), this, CriticalWorkQueue);
		}
	}
	else
	{
		
		// The IRP has not been canceled and is now in a noncancelable state.
		// Complete it and start the next IRP on the device.

		// Set the status and information fields for the Irp based on the state
		// structure we were passed.
		if (state.m_DataError || state.m_InitiateFailed)
		{
			I.Status() = STATUS_DRIVER_INTERNAL_ERROR;
		}
		else
		{
			I.Status() = STATUS_SUCCESS;
		}
		I.Information() = state.m_BytesTransferred;

		t << "S5933Device::SerialReadCallback: Read Irp completing " << I;

		// NextIrp completes this IRP and starts processing 
		// for the next IRP in the driver managed queue.
		m_ReadQueue.DMQ_NEXT_IRP(I);
	}
}

////////////////////////////////////////////////////////////////////////
// ReadWaitForCancel (member of S5933Device)
//
//		Waits for the cancel routine to signal before completing the
//		current IRP in a canceled state and starting the next IRP
//		on the device.
//
// Input
//		pItem		Pointer to our work item
//
// Output	
//
// Notes
//		This routine runs at passive level.
//

VOID S5933Device::ReadWaitForCancel()
{
	// Wait for cancel to signal that it is finished.
	m_ReadCancelFinished.Wait();

	// Now that the cancel routine has run, we can complete this IRP and
	// start the next one on the device.  Our IRP to complete is still the
	// current IRP, so use CurrentIrp() to get it.  The information and
	// status have already been set.
	KIrp I(m_ReadQueue.CurrentIrp());
	m_ReadQueue.DMQ_NEXT_IRP(I);
}

////////////////////////////////////////////////////////////////////////
// Read (member of S5933Device)
//
//		Handler for IRP_MJ_READ
//
// Input
//		I			Current IRP
//
// Output   
//		NTSTATUS	Result code
//
// Note   
//		This routine handles read requests.
//		The requests are queued to enforce serialization,
//		so that	multiple requests are not processed	
//		simultaneously.

NTSTATUS S5933Device::Read(KIrp I)
{
	t << "Entering read, " << I;

	// Declare a memory object and check to see if the Mdl is DWORD aligned.
	// If not return an error since the AMCC5933 requires DWORD alignment
	// for busmaster Dma operation.
	KMemory Mem(I.Mdl());
    if ( Mem.OffsetFromPageBase() & 0x3 )
	{
		I.Information() = 0;
		return I.COMPLETE(STATUS_INVALID_PARAMETER);
	}

	// Impose the further restriction that our size must be an
	// integral number of DWORDS.  Although the S5933 can handle
	// a partial transfer, extra logic is required to get the FIFO
	// advance conditions correct.
	if (I.ReadSize() % sizeof(ULONG))
	{
		I.Information() = 0;
		return I.COMPLETE(STATUS_INVALID_PARAMETER);
	}

	// Always ok to read 0 elements
	if (I.ReadSize() == 0)
	{
		I.Information() = 0;
		return I.COMPLETE(STATUS_SUCCESS);
	}

	// Queue the IRP for processing in the driver managed queue.
	// The actual read function is performed in SerialRead
	return m_ReadQueue.QueueIrp(I);
}

////////////////////////////////////////////////////////////////////////
// SerialWrite (member of S5933Device)
//	
//		Handler for serialized WRITE
//
// Input	
//		I		Current IRP
//
// Output	
//		None
//
// Notes	
//		This routine is called when the IRP is removed from the
//		queue.  This guarantees that multiple requests are
//		never processed simultaneously.

void S5933Device::SerialWrite(KIrp I)
{
	t << "Entering S5933Device::SerialWrite, " << I;

	// Acquire the spin lock protecting our write operation so that the cancel
	// routine cannot run while we are working on the device.
	m_WriteCancelSync.Lock();

	// We want to install a new cancel routine that will allow our transfer to be
	// canceled while being processed.  Cancel policy is up to the individual
	// driver design, and this step is optional.  We might for instance setup a
	// watchdog timer instead to cancel the transfer based on a time out.
	I.SetCancelRoutine(LinkTo(CancelInProgressWrite));

	// Test to see if our IRP has been canceled.
	if (I.WasCanceled())
	{
		
		// The IRP has been canceled. We must clear the cancel routine
		// we just set and determine if it has been called.  We do this
		// by checking the return value of I.SetCancelRoutine()
		PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);
		if (pLastCancel == LinkTo(CancelInProgressWrite))
		{

			// If the cancel routine we set was still on set on the IRP,
			// it hasn't been called.  In this case we can dispose of the
			// IRP here.  Release our spinlock, then complete the IRP with
			// a canceled status and start the next IRP on the device.
			m_WriteCancelSync.Unlock();
			I.Status() = STATUS_CANCELLED;
			I.Information() = 0;
			m_WriteQueue.DMQ_NEXT_IRP(I);
			return;
		}
		else
		{

			// If the cancel routine has changed (has been set to NULL by the
			// system), the cancel routine has been called and will run when
			// we release the spinlock.  We must proceed in initiating our
			// action on the device (starting the DMA Write) and let the cancel
			// routine handle cleaning up the device state.
			ASSERT(pLastCancel == NULL);
		}
	}

	// Now we must start the DMA write transfer on the KS5933 object using the
	// context we created.  Use the dummy device object instead of our actual
	// device object in the call.  Note that it is still safe to touch the IRP even
	// after we have set the cancel routine, since a requirement of the class
	// that all terminal processing of the transfer including cancel processing,
	// be performed by the PKS5933_DMA_DONE_HANDLER_ROUTINE function.  Therefore we
	// are assured that the IRP will be valid until that routine is called, which will
	// be sometime after we start the transfer by calling DmaWrite().  See the
	// cancel routine below for more details.
	m_S5933.DmaWrite(
		m_pDummyDevice,
		I.Mdl(),
		LinkTo(SerialWriteCallback),
		this
		);

	// Now release the spinlock so that cancel processing can continue if
	// necessary.
	m_WriteCancelSync.Unlock();
	return;
}

////////////////////////////////////////////////////////////////////////
// CancelInProgressWrite (member of S5933Device)
//	
//		Cancel routine for in progress write
//
// Input	
//		I		Current IRP
//
// Output	
//		None
//
// Notes	
//
void S5933Device::CancelInProgressWrite( KIrp I )
{

	// Release the cancel spin lock at the correct IRQL
	CancelSpinLock::Release(I.CancelIrql());

	// Acquire and release the cancel sync spin lock.  This prevents our cancel
	// routine from running until the DMA write operation has started on the
	// device.
	m_WriteCancelSync.Lock();
	m_WriteCancelSync.Unlock();

	// Try to cancel the operation on the KS5933 object.
	m_S5933.CancelDmaWrite();	

	// We will set an event indicating the cancel logic is finished.  This
	// will be used by the callback logic in determining when the
	// device state is resolved and processing of the next IRP can
	// continue. Note that we don't try to start the next IRP here.
	// Calling cancel causes our completion callback routine to be called
	// immediately with a cancelled status if the transfer has not already
	// been completed.  The IRP will be completed there.  Calling the
	// cancel DMA routine if the transfer is already completed or just
	// completing does no harm.
	m_WriteCancelFinished.Set();
}

////////////////////////////////////////////////////////////////////////
// SerialWriteCallback (member of S5933Device)
//	
//		Handler for serialized WRITE
//
// Input	
//		state		Flags describing the state of the DMA read transfer object.
//
// Output	
//		None
//
// Notes	
//		This routine is called at dispatch level.
//		
//
VOID S5933Device::SerialWriteCallback( KS5933_TRANSFER_STATE state )
{
	t << "Entering S5933Device::SerialWriteCallback.";

	// We need to find out what the current IRP in the driver managed write
	// queue is.
	KIrp I = m_WriteQueue.CurrentIrp();

	// Make the IRP noncancelable.
	PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);

	// Test to see if our IRP has been canceled.  Note that this must
	// be done after setting the cancel routine to NULL.
	if (I.WasCanceled())
	{
		
		// The IRP has been canceled.
		I.Status() = STATUS_CANCELLED;

		// It may be interesting to note the amount of data transferred so far
		// even though the IRP was canceled.  We can obtain that information from
		// the state structure.
		I.Information() = state.m_BytesTransferred;

		// We must check to see if our cancel routine was called.  We do
		// this by checking the value returned when we set the cancel routine.
		if (pLastCancel == LinkTo(CancelInProgressWrite))
		{

			// The cancel routine was still set on the IRP when we made it
			// noncancelable.  This means our cancel routine was not called,
			// and that the IRP must have been canceled in the very small
			// window after we made it noncancelable.  Complete the IRP here
			// and start the next IRP on the device.
			m_WriteQueue.DMQ_NEXT_IRP(I);
		}
		else
		{

			// The cancel routine had already been cleared, indicating that our
			// cancel routine was called. We will complete the IRP here after
			// cleaning up our device state. Since we want all processing to be
			// done, we must wait to make sure the cancel routine has run,
			// otherwise it might try to cancel the DMA write operation for the
			// next IRP. In order to make sure the cancel routine has run,
			// we will wait for an event to be signaled in the cancel routine.
			// Since we are in a DPC at DIRQL we can't wait here, so we will
			// schedule a work item to call NextIrp once the wait has been
			// satisfied.  Note that our work item is a member of the class, and not
			// dynamically allocated.  We must be certain that the work item only
			// gets queued once (otherwise its list entry will be corrupted).  This
			// of course will be the case since we are serializing IRPs, and won't
			// start the next IRP until the work item completes.
			m_WriteWaitForCancel.Queue(LinkTo(WriteWaitForCancel), this, CriticalWorkQueue);
		}
	}
	else
	{
		
		// The IRP has not been canceled and is now in a noncancelable state.
		// Complete it and start the next IRP on the device.

		// Set the status and information fields for the Irp based on the state
		// structure we were passed.
		if (state.m_DataError || state.m_InitiateFailed)
		{
			I.Status() = STATUS_DRIVER_INTERNAL_ERROR;
		}
		else
		{
			I.Status() = STATUS_SUCCESS;
		}
		I.Information() = state.m_BytesTransferred;

		t << "S5933Device::SerialWriteCallback: Write Irp completing " << I;

		// NextIrp completes this IRP and starts processing 
		// for the next IRP in the driver managed queue.
		m_WriteQueue.DMQ_NEXT_IRP(I);
	}
}

////////////////////////////////////////////////////////////////////////
// WriteWaitForCancel (member of S5933Device)
//
//		Waits for the cancel routine to signal before completing the
//		current IRP in a canceled state and starting the next IRP
//		on the device.
//
// Input
//		pItem		Pointer to our work item
//
// Output	
//
// Notes
//		This routine runs at passive level.
//

VOID S5933Device::WriteWaitForCancel()
{
	// Wait for cancel to signal that it is finished.
	m_WriteCancelFinished.Wait();

	// Now that the cancel routine has run, we can complete this IRP and
	// start the next one on the device.  Our IRP to complete is still the
	// current IRP, so use CurrentIrp() to get it.  The information and
	// status have already been set.
	KIrp I(m_WriteQueue.CurrentIrp());
	m_WriteQueue.DMQ_NEXT_IRP(I);
}

////////////////////////////////////////////////////////////////////////
// Write (member of S5933Device)
//
//		Handler for IRP_MJ_WRITE
//
// Input
//		I			Current IRP
//
// Output   
//		NTSTATUS	Result code
//
// Note   
//		This routine handles write requests.
//		The requests are queued to enforce serialization,
//		so that	multiple requests are not processed	
//		simultaneously.

NTSTATUS S5933Device::Write(KIrp I)
{
	t << "Entering write, " << I;

	// Declare a memory object and check to see if the Mdl is DWORD aligned.
	// If not return an error since the AMCC5933 requires DWORD alignment
	// for busmaster Dma operation.
	KMemory Mem(I.Mdl());
    if ( Mem.OffsetFromPageBase() & 0x3 )
	{
		I.Information() = 0;
		return I.COMPLETE(STATUS_INVALID_PARAMETER);
	}

	// Impose the further restriction that our size must be an
	// integral number of DWORDS.  Although the S5933 can handle
	// a partial transfer, extra logic is required to get the FIFO
	// advance conditions correct.
	if (I.WriteSize() % sizeof(ULONG))
	{
		I.Information() = 0;
		return I.COMPLETE(STATUS_INVALID_PARAMETER);
	}

	// Always ok to write 0 elements
	if (I.WriteSize() == 0)
	{
		I.Information() = 0;
		return I.COMPLETE(STATUS_SUCCESS);
	}

	// Queue the IRP for processing in the driver managed queue.
	// The actual write function is performed in SerialWrite
	return m_WriteQueue.QueueIrp(I);
}

////////////////////////////////////////////////////////////////////////
// DeviceControl (member of S5933Device)
//
//		Handler for IRP_MJ_DEVICE_CONTROL
//
// Input
//		I			Current IRP
// 
// Output
//		NTSTATUS	Result code
//
// Notes:
//		This routine is the first handler for Device Control requests.
//		Some function codes may be handled immediately, 
//		while others may be serialized.
//		

NTSTATUS S5933Device::DeviceControl(KIrp I) 
{
	NTSTATUS status;

	t << "Entering S5933Device::Device Control, " << I;
	switch (I.IoctlCode())
	{
		case S5933_IOCTL_800_CHIP_RESET:
		    status = S5933_IOCTL_800_CHIP_RESET_Handler(I);
			break;

		case S5933_IOCTL_801_POST_MSG:
			// Queue this request for serialized handling
		    status = m_PostMsgQueue.QueueIrp(I);
			break;

		case S5933_IOCTL_802_RETRIEVE_MSG:
			// Queue this request for serialized handling
		    status = m_RetrieveMsgQueue.QueueIrp(I);
			break;

		case S5933_IOCTL_803_WRITE_PASSTHRU:
		    status = S5933_IOCTL_803_WRITE_PASSTHRU_Handler(I);
			break;

		case S5933_IOCTL_804_WRITE_NVRAM:
		    status = S5933_IOCTL_804_WRITE_NVRAM_Handler(I);
			break;

		case S5933_IOCTL_805_READ_NVRAM:
		    status = S5933_IOCTL_805_READ_NVRAM_Handler(I);
			break;

		case S5933_IOCTL_806_DUMP_REGISTERS:
		    status = S5933_IOCTL_806_DUMP_REGISTERS_Handler(I);
			break;

		default:
			// Unrecognized IOCTL request
			status = STATUS_INVALID_PARAMETER;
			break;
	}

	// If the IRP was queued, or its IOCTL handler deferred processing using some
	// driver specific scheme, the status variable is set to STATUS_PENDING.
	// In this case we simply return that status, and the IRP will be completed
	// later.  Otherwise, complete the IRP using the status returned by the
	// IOCTL handler.
	if (status == STATUS_PENDING)
	{
		return status;
	}
	else
	{
		return I.COMPLETE(status);
	}
}

////////////////////////////////////////////////////////////////////////
//	S5933_IOCTL_800_CHIP_RESET_Handler (member of S5933Device)
//
//		Handler for IO Control Code S5933_IOCTL_800_CHIP_RESET
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933_IOCTL_800_CHIP_RESET function.
//		This routine runs at passive level.
//

NTSTATUS S5933Device::S5933_IOCTL_800_CHIP_RESET_Handler(KIrp I)
{
	t << "Entering S5933Device::S5933_IOCTL_800_CHIP_RESET_Handler, " << I;

	m_S5933.ResetFifoStatusFlags(KS59XX_PCI_TO_ADDON);
	m_S5933.ResetFifoStatusFlags(KS59XX_ADDON_TO_PCI);
	m_S5933.SetFifoProperties(KS59XX_PCI_TO_ADDON, FALSE, FALSE);
	m_S5933.SetFifoProperties(KS59XX_ADDON_TO_PCI, FALSE, FALSE);
	m_S5933.SetFifoBusProperties( KS59XX_NO_CONVERSION, 0, 2 );
	m_S5933.ResetMailboxFlags();
	m_S5933.AddonReset(FALSE);

    I.Information() = 0;

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//	S5933_IOCTL_801_POST_MSG_Handler (member of S5933Device)
//
//		Handler for IO Control Code S5933_IOCTL_801_POST_MSG
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933_IOCTL_801_POST_MSG function.
//		This routine runs at passive level.
//

VOID S5933Device::S5933_IOCTL_801_POST_MSG_Handler(KIrp I)
{
	t << "Entering S5933Device::S5933_IOCTL_801_POST_MSG_Handler, " << I;

	// Make sure input parameters look good
	if(I.IoctlInputBufferSize() < sizeof(KS5933_MAILBOX_MESSAGE) || !I.IoctlBuffer())
	{
		I.Status() = STATUS_INVALID_PARAMETER;
		I.Information() = 0;
		m_PostMsgQueue.DMQ_NEXT_IRP(I);
		return;
	}

	// Get a pointer to our message structure
	PKS5933_MAILBOX_MESSAGE pMsg = static_cast<PKS5933_MAILBOX_MESSAGE>(I.IoctlBuffer());

	// Check the mailbox parameters.  This could be done in our call to set the
	// interrupt, but if we found an error, we would have to check for cancellation
	// before completing, since we would have already setup our cancellation routine.
	// This is simpler.
	if( (pMsg->MessageMailBox < KS59XX_MAILBOX_MINIMUM)
		|| (pMsg->MessageMailBox > KS59XX_MAILBOX_MAXIMUM) )
	{
		I.Status() = STATUS_INVALID_PARAMETER;
		I.Information() = 0;
		m_PostMsgQueue.DMQ_NEXT_IRP(I);
		return;
    }	
	
	// Acquire the spin lock protecting our post operation so that the cancel
	// routine cannot run while we are working on the device.
	m_PostMsgCancelSync.Lock();

	// Setup a cancel routine for this IRP, since it will pend until the mailbox
	// message has been read by the add-on device. Make our IRP cancelable again
	// by setting a new cancel routine on it.
	I.SetCancelRoutine(LinkTo(CancelInProgressPostMessage));

	// Test to see if our IRP has been canceled.
	if (I.WasCanceled())
	{
		
		// The IRP has been canceled.  We must clear the cancel routine
		// we just set and determine if it has been called.  We do this
		// by checking the return value of I.SetCancelRoutine().
		PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);
		if (pLastCancel == LinkTo(CancelInProgressPostMessage))
		{

			// If the cancel routine we set was still on set on the IRP,
			// it hasn't been called.  In this case we can dispose of the
			// IRP here.  Release our spinlock, then complete the IRP with
			// a canceled status and start the next IRP on the device.
			m_PostMsgCancelSync.Unlock();
			I.Status() = STATUS_CANCELLED;
			I.Information() = 0;
			m_PostMsgQueue.DMQ_NEXT_IRP(I);
			return;
		}
		else
		{

			// If the cancel routine has changed (has been set to NULL by the
			// system), the cancel routine has been called and will run when
			// we release the spinlock.  We must proceed in initiating our
			// action on the device (posting the message) and let the cancel
			// routine handle cleaning up the device state.
			ASSERT(pLastCancel == NULL);
		}
	}

	// Setup our S5933 device so we recieve an interrupt when the message is read.
	//
	// SetupMailboxInterrupt(
	//  KS5933_DIRECTION direction,
	//  ULONG MessageMailBox,
	//  ULONG ByteTrigger,
	//  PKS5933_MAILBOX_INTERRUPT_HANDLER_ROUTINE pCallback,
	//  PVOID pContext,
	//  BOOLEAN bCallAtDirql,
	//  BOOLEAN bSingleShot,
	//  BOOLEAN bLock )
	//
	m_S5933.SetupMailboxInterrupt(
		KS59XX_PCI_TO_ADDON,
		pMsg->MessageMailBox,
		0,
		LinkTo(PostMessageCallback),
		this
		);

	// Post our message to the S5933 object's mailbox.
	switch(pMsg->MessageMailBox)
	{
	case 1:
		m_S5933.m_OutgoingMailbox1 = pMsg->MessageValue;
		break;
	case 2:
		m_S5933.m_OutgoingMailbox2 = pMsg->MessageValue;
		break;
	case 3:
		m_S5933.m_OutgoingMailbox3 = pMsg->MessageValue;
		break;
	case 4:
		m_S5933.m_OutgoingMailbox4 = pMsg->MessageValue;
		break;
	default:
		// This can't happen since it would have been caught above when we checked
		// the parameters.
		ASSERT(FALSE);
		break;
	}

	// Now release the spinlock so that cancel processing can continue if
	// necessary.
	m_PostMsgCancelSync.Unlock();
	return;
}

////////////////////////////////////////////////////////////////////////
// CancelInProgressPostMessage (member of S5933Device)
//	
//		Cancel routine for in progress post message
//
// Input	
//		I		Current IRP
//
// Output	
//		None
//
// Notes	
//
void S5933Device::CancelInProgressPostMessage( KIrp I )
{

	// Release the cancel spin lock at the correct IRQL
	CancelSpinLock::Release(I.CancelIrql());

	// Acquire and release the cancel sync spin lock.  This prevents our cancel
	// routine from running until the post message operation has started on the
	// device.
	m_PostMsgCancelSync.Lock();
	m_PostMsgCancelSync.Unlock();

	// Try to cancel the operation on the KS5933 object.
	if ( m_S5933.CancelMailboxInterrupt( KS59XX_PCI_TO_ADDON ) )
	{

		// We were successful.  The callback will not occur, so we
		// will complete this IRP here with STATUS_CANCELLED,
		// and start the next one in the queue.
		I.Status() = STATUS_CANCELLED;
		I.Information() = 0;
		m_PostMsgQueue.DMQ_NEXT_IRP(I);
	}
	else
	{

		// We did not succeed in canceling operation.  This means our callback
		// will be running soon.  When it does, it will find that the IRP
		// has been canceled.  We cannot complete the IRP here, or start the
		// next IRP, since the device state is not fully resolved.  We
		// will set an event indicating the cancel logic is finished.  This
		// will be used by the callback logic in determining when the
		// device state is resolved and processing of the next IRP can
		// continue.
		m_PostMsgCancelFinished.Set();
	}
}

////////////////////////////////////////////////////////////////////////
// PostMessageCallback (member of S5933Device)
//	
//		Completion handler for Post Message
//
// Input	
//
// Output	
//		None
//
// Notes	
//		This routine is called at dispatch level.
//		
//
VOID S5933Device::PostMessageCallback()
{

	KIrp I = m_PostMsgQueue.CurrentIrp();

	// Make the IRP noncancelable.
	PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);

	// Test to see if our IRP has been canceled.  Note that this must
	// be done after setting the cancel routine to NULL.
	if (I.WasCanceled())
	{
		
		// The IRP has been canceled.
		I.Status() = STATUS_CANCELLED;
		I.Information() = 0;

		// We must check to see if our cancel routine was called.  We do
		// this by checking the value returned when we set the cancel routine.
		if (pLastCancel == LinkTo(CancelInProgressPostMessage))
		{

			// The cancel routine was still set on the IRP when we made it
			// noncancelable.  This means our cancel routine was not called,
			// and that the IRP must have been canceled in the very small
			// window after we made it noncancelable.  Complete the IRP here
			// and start the next IRP on the device.
			m_PostMsgQueue.DMQ_NEXT_IRP(I);
		}
		else
		{

			// The cancel routine had already been cleared, indicating that our
			// cancel routine was called.  Normally if this happens, the cancel routine
			// will have succesfully canceled the post message operation and
			// called NextIrp, so that we never end up here with a cancelled IRP.
			// There is a window, however, where the cancel routine was unable to
			// cancel the post message operation, since it was just completing.
			// We will complete the IRP here after cleaning up our device state.
			// Since we want all processing to be done, we must wait to make sure
			// the cancel routine has run, otherwise it might try to cancel the
			// post message operation  for the next IRP. In order to make sure the cancel
			// routine has run, we will wait for an event to be signaled in the
			// cancel routine.  Since we are in a DPC at DIRQL we can't wait here,
			// so we will schedule a work item to call NextIrp once the wait has been
			// satisfied.  Note that our work item is a member of the class, and not
			// dynamically allocated.  We must be certain that the work item only
			// gets queued once (otherwise its list entry will be corrupted).  This
			// of course will be the case since we are serializing IRPs, and won't
			// start the next IRP until the work item completes.
			m_PostMsgWaitForCancel.Queue(LinkTo(PostMessageWaitForCancel), this, CriticalWorkQueue);
		}
	}
	else
	{
		
		// The IRP has not been canceled and is now in a noncancelable state.
		// Complete it and start the next IRP on the device.
		I.Status() = STATUS_SUCCESS;
		I.Information() = 0;
		m_PostMsgQueue.DMQ_NEXT_IRP(I);
	}
}

////////////////////////////////////////////////////////////////////////
// PostMessageWaitForCancel (member of S5933Device)
//
//		Waits for the cancel routine to signal before completing the
//		current IRP in a canceled state and starting the next IRP
//		on the device.
//
// Input
//		pItem		Pointer to our work item
//
// Output	
//
// Notes
//		This routine runs at passive level.
//

VOID S5933Device::PostMessageWaitForCancel()
{
	// Wait for cancel to signal that it is finished.
	m_PostMsgCancelFinished.Wait();

	// Now that the cancel routine has run, we can complete this IRP and
	// start the next one on the device.  Our IRP to complete is still the
	// current IRP, so use CurrentIrp() to get it.  The information and
	// status have already been set.
	KIrp I(m_PostMsgQueue.CurrentIrp());
	m_PostMsgQueue.DMQ_NEXT_IRP(I);
}

////////////////////////////////////////////////////////////////////////
//	S5933_IOCTL_802_RETRIEVE_MSG_Handler (member of S5933Device)
//
//		Handler for IO Control Code S5933_IOCTL_802_RETRIEVE_MSG
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933_IOCTL_802_RETRIEVE_MSG function.
//		This routine runs at passive level.  Comments have been eliminated
//		where similarities exist to the post message operation.
//

VOID S5933Device::S5933_IOCTL_802_RETRIEVE_MSG_Handler(KIrp I)
{
	t << "Entering S5933Device::S5933_IOCTL_802_RETRIEVE_MSG_Handler, " << I;

	// Make sure input/output parameters look good
	if(I.IoctlInputBufferSize() < sizeof(KS5933_MAILBOX_MESSAGE) ||
		I.IoctlOutputBufferSize() < sizeof(KS5933_MAILBOX_MESSAGE) ||
		!I.IoctlBuffer())
	{
		I.Status() = STATUS_INVALID_PARAMETER;
		I.Information() = 0;
		m_RetrieveMsgQueue.DMQ_NEXT_IRP(I);
		return;
	}

	PKS5933_MAILBOX_MESSAGE pMsg = static_cast<PKS5933_MAILBOX_MESSAGE>(I.IoctlBuffer());

	if( (pMsg->MessageMailBox < KS59XX_MAILBOX_MINIMUM)
		|| (pMsg->MessageMailBox > KS59XX_MAILBOX_MAXIMUM) )
	{
		I.Status() = STATUS_INVALID_PARAMETER;
		I.Information() = 0;
		m_RetrieveMsgQueue.DMQ_NEXT_IRP(I);
		return;
    }
	
	m_RetrieveMsgCancelSync.Lock();

	I.SetCancelRoutine(LinkTo(CancelInProgressRetrieveMessage));

	if (I.WasCanceled())
	{
		PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);
		if (pLastCancel == LinkTo(CancelInProgressRetrieveMessage))
		{
			m_RetrieveMsgCancelSync.Unlock();
			I.Status() = STATUS_CANCELLED;
			I.Information() = 0;
			m_RetrieveMsgQueue.DMQ_NEXT_IRP(I);
			return;
		}
		else
		{
			ASSERT(pLastCancel == NULL);
			return;
		}
	}

	// Setup our S5933 device so we recieve an interrupt when the message is sent.
	//
	// SetupMailboxInterrupt(
	//  KS5933_DIRECTION direction,
	//  ULONG MessageMailBox,
	//  ULONG ByteTrigger,
	//  PKS5933_MAILBOX_INTERRUPT_HANDLER_ROUTINE pCallback,
	//  PVOID pContext,
	//  BOOLEAN bCallAtDirql,
	//  BOOLEAN bSingleShot,
	//  BOOLEAN bLock )
	//
	m_S5933.SetupMailboxInterrupt(
		KS59XX_ADDON_TO_PCI,
		pMsg->MessageMailBox,
		0,
		LinkTo(RetrieveMessageCallback),
		this
		);

	m_RetrieveMsgCancelSync.Unlock();
	return;
}

////////////////////////////////////////////////////////////////////////
// CancelInProgressRetrieveMessage (member of S5933Device)
//	
//		Cancel routine for in progress post message
//
// Input	
//		I		Current IRP
//
// Output	
//		None
//
// Notes
//		Comments have been eliminated where similarities exist to the
//		post message operation.
//
void S5933Device::CancelInProgressRetrieveMessage( KIrp I )
{

	CancelSpinLock::Release(I.CancelIrql());

	m_RetrieveMsgCancelSync.Lock();
	m_RetrieveMsgCancelSync.Unlock();

	if ( m_S5933.CancelMailboxInterrupt( KS59XX_ADDON_TO_PCI ) )
	{
		I.Status() = STATUS_CANCELLED;
		I.Information() = 0;
		m_RetrieveMsgQueue.DMQ_NEXT_IRP(I);
	}
	else
	{
		m_RetrieveMsgCancelFinished.Set();
	}
}

////////////////////////////////////////////////////////////////////////
// RetrieveMessageCallback (member of S5933Device)
//	
//		Completion handler for Retrieve Message
//
// Input	
//
// Output	
//		None
//
// Notes	
//		This routine is called at dispatch level.
//		Comments have been eliminated where similarities exist to the
//		post message operation.
//		
//
VOID S5933Device::RetrieveMessageCallback()
{

	KIrp I = m_RetrieveMsgQueue.CurrentIrp();

	PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);

	if (I.WasCanceled())
	{
		I.Status() = STATUS_CANCELLED;
		I.Information() = 0;
		if (pLastCancel == LinkTo(CancelInProgressRetrieveMessage))
		{
			m_RetrieveMsgQueue.DMQ_NEXT_IRP(I);
		}
		else
		{
			m_RetrieveMsgWaitForCancel.Queue(LinkTo(RetrieveMessageWaitForCancel), this, CriticalWorkQueue);
		}
	}
	else
	{

		// Get a pointer to our message structure
		PKS5933_MAILBOX_MESSAGE pMsg = static_cast<PKS5933_MAILBOX_MESSAGE>(I.IoctlBuffer());

		// Retrieve our message from the S5933 object's mailbox.
		switch(pMsg->MessageMailBox)
		{
		case 1:
			pMsg->MessageValue = m_S5933.m_IncomingMailbox1;
			break;
		case 2:
			pMsg->MessageValue = m_S5933.m_IncomingMailbox2;
			break;
		case 3:
			pMsg->MessageValue = m_S5933.m_IncomingMailbox3;
			break;
		case 4:
			pMsg->MessageValue = m_S5933.m_IncomingMailbox4;
			break;
		default:
			// This can't happen since it would have been caught above when we checked
			// the parameters.
			ASSERT(FALSE);
			break;
		}

		I.Status() = STATUS_SUCCESS;
		I.Information() = sizeof(KS5933_MAILBOX_MESSAGE);
		m_RetrieveMsgQueue.DMQ_NEXT_IRP(I);
	}
}

////////////////////////////////////////////////////////////////////////
// RetrieveMessageWaitForCancel (member of S5933Device)
//
//		Waits for the cancel routine to signal before completing the
//		current IRP in a canceled state and starting the next IRP
//		on the device.
//
// Input
//		pItem		Pointer to our work item
//
// Output	
//
// Notes
//		This routine runs at passive level.
//		Comments have been eliminated where similarities exist to the
//		post message operation.
//

VOID S5933Device::RetrieveMessageWaitForCancel()
{
	m_RetrieveMsgCancelFinished.Wait();

	KIrp I(m_RetrieveMsgQueue.CurrentIrp());
	m_RetrieveMsgQueue.DMQ_NEXT_IRP(I);
}

////////////////////////////////////////////////////////////////////////
//	S5933_IOCTL_803_WRITE_PASSTHRU_Handler (member of S5933Device)
//
//		Handler for IO Control Code S5933_IOCTL_803_WRITE_PASSTHRU
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933_IOCTL_803_WRITE_PASSTHRU function.
//
//		When the ISA board is attached to the PCI card, the passthru logic
//		immediately completes all read and write requests (without any
//		arbitration from the ADD-ON side).  Because of this, it only makes
//		sense to read or write a single DWORD at a time since that is all
//		the S5933's internal register can hold.  This register can be
//		accessed from the ADD-ON side.
//
//		Since this operation writes a single aligned DWORD, no synchronization
//		is required, since this operation is atomic already.
//

NTSTATUS S5933Device::S5933_IOCTL_803_WRITE_PASSTHRU_Handler(KIrp I)
{
	t << "Entering S5933Device::S5933_IOCTL_803_WRITE_PASSTHRU_Handler, " << I;

	// Make sure input/output parameters look good
	if(I.IoctlInputBufferSize() < sizeof(KS5933_PASSTHRU) ||
		!I.IoctlBuffer())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	PKS5933_PASSTHRU pInput = static_cast<PKS5933_PASSTHRU>(I.IoctlBuffer());

	// Write the value to the passthru region at offset 0
	m_PassThruRange.outd(0, pInput->Data);

    // We aren't returning any data
	I.Information() = 0;

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//	S5933_IOCTL_804_WRITE_NVRAM_Handler (member of S5933Device)
//
//		Handler for IO Control Code S5933_IOCTL_804_WRITE_NVRAM
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933_IOCTL_804_WRITE_NVRAM function.
//

NTSTATUS S5933Device::S5933_IOCTL_804_WRITE_NVRAM_Handler(KIrp I)
{
	t << "Entering S5933Device::S5933_IOCTL_804_WRITE_NVRAM_Handler, " << I;

	// Make sure input/output parameters look good
	
	// Check the size of the input buffer and make sure the pointer is not NULL
	if(I.IoctlInputBufferSize() < sizeof(KS5933_NVRAM) ||
		!I.IoctlBuffer())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	PKS5933_NVRAM pInput = static_cast<PKS5933_NVRAM>(I.IoctlBuffer());

	// Now check the actual size of the input buffer
	if( I.IoctlInputBufferSize() < (sizeof(KS5933_NVRAM) + pInput->Count -
		sizeof(pInput->Data)) )
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
    }

	// Check the start offset, and limit to beyond the configuration area.
	// NOTE: This check is a safety feature to ensure that the
	// configuration area of the Nvram is not accidentally modified.
	// This check can be modified or commented out to allow these areas
	// to be written.  WARNING: if not done properly, this could cause
	// the card to become unrecognizable by the system.
	if( pInput->Offset < KS59XX_NVRAM_PAST_CONFIG_OFFSET )
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
    }

	// Check that the write request does not extend beyond the end of Nvram.
    if ( static_cast<ULONG>(pInput->Offset + pInput->Count) > m_NvramSize)
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
    }

	// Write the data to the Nvram at the specified offset
	I.Status() = m_S5933.WriteNvram(pInput->Data, pInput->Offset, pInput->Count);

    // We aren't returning any data
	I.Information() = 0;

	return I.Status();
}

////////////////////////////////////////////////////////////////////////
//	S5933_IOCTL_805_READ_NVRAM_Handler (member of S5933Device)
//
//		Handler for IO Control Code S5933_IOCTL_805_READ_NVRAM
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933_IOCTL_805_READ_NVRAM function.
//

NTSTATUS S5933Device::S5933_IOCTL_805_READ_NVRAM_Handler(KIrp I)
{
	t << "Entering S5933Device::S5933_IOCTL_805_READ_NVRAM_Handler, " << I;

	// Make sure input/output parameters look good
	
	// Check the size of the input buffer and make sure the pointer is not NULL
	if(I.IoctlInputBufferSize() < sizeof(KS5933_NVRAM) ||
		!I.IoctlBuffer())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	PKS5933_NVRAM pInput = static_cast<PKS5933_NVRAM>(I.IoctlBuffer());

	// Now check the actual size of the input buffer
	if( I.IoctlInputBufferSize() < (sizeof(KS5933_NVRAM) + pInput->Count -
		sizeof(pInput->Data)) )
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
    }

	// Check that the read request does not extend beyond the end of Nvram.
    if ( static_cast<ULONG>(pInput->Offset + pInput->Count) > m_NvramSize)
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
    }

	// Write the data to the Nvram at the specified offset
	I.Status() = m_S5933.ReadNvram(pInput->Data, pInput->Offset, pInput->Count);

    // We aren't returning any data
	if (NT_SUCCESS(I.Status()))
		I.Information() = sizeof(KS5933_NVRAM) + pInput->Count -
			sizeof(pInput->Data);
	else
		I.Information() = 0;

	return I.Status();
}

////////////////////////////////////////////////////////////////////////
//	S5933_IOCTL_806_DUMP_REGISTERS_Handler (member of S5933Device)
//
//		Handler for IO Control Code S5933_IOCTL_806_DUMP_REGISTERS
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933_IOCTL_806_DUMP_REGISTERS function.
//		This routine runs at passive level.
//

NTSTATUS S5933Device::S5933_IOCTL_806_DUMP_REGISTERS_Handler(KIrp I)
{
	t << "Entering S5933Device::S5933_IOCTL_806_DUMP_REGISTERS_Handler, " << I;

	// make sure the size of the buffer passed down is OK
	if(I.IoctlOutputBufferSize() < sizeof(AMCCS5933_PCI_BUS_OPERATION_REGISTERS) 
		|| !I.IoctlBuffer())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	// Get a pointer to the caller's output buffer
	PAMCCS5933_PCI_BUS_OPERATION_REGISTERS pRegs = 
		(PAMCCS5933_PCI_BUS_OPERATION_REGISTERS)I.IoctlBuffer();

	m_S5933.ReadOperationRegisters(pRegs);
	I.Information() = sizeof(AMCCS5933_PCI_BUS_OPERATION_REGISTERS);

	// Assuming that the request was handled:
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
// Cleanup (member of S5933Device)
//
//		Handler for IRP_MJ_CLEANUP	
//
// Input
//		I			Current IRP
//
// Output	
//		NTSTATUS	Result code
//
// Notes
//

NTSTATUS S5933Device::CleanUp(KIrp I)
{
	t << "Entering CleanUp, " << I;

    KDeviceQueue dq(DeviceQueue());
	dq.CleanUp(I.FileObject());
	return I.COMPLETE(STATUS_SUCCESS);
}


////////////////////////////////////////////////////////////////////////
// Create (member of S5933Device)
//
//		Handler for IRP_MJ_CREATE
//
// Input
//		I			Current IRP
//
// Output   
//		NTSTATUS	Result code
//
// Notes   
//

NTSTATUS S5933Device::Create(KIrp I)
{
	t << "Entering Create, " << I;
	I.Information() = 0;
	return I.COMPLETE(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
// Close (member of S5933Device)
//
//		Handler for IRP_MJ_CLOSE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//

NTSTATUS S5933Device::Close(KIrp I)
{
	t << "Entering Close, " << I;
	I.Information() = 0;
	return I.COMPLETE(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
// StartIo (member of S5933DeviceReadQueue)
//	
//		This routine is called when an IRP is taken off
//		the Driver Managed Queue (used for serializing I/O) and
//		presented for processing.
//
// Input
//		I			IRP removed from queue
//
// Output
//		None
//
// Notes
//

VOID S5933DeviceReadQueue::StartIo(KIrp I)
{
	t  << "Entering S5933DeviceReadQueue StartIo, " << I;

	// Find the device class so we can call the serialized
	// routines in the device class.  The handlers can be
	// moved to the DriverManagedQueue class if it is more
	// convenient.
	S5933Device *pDev = (S5933Device *) KDevicePTR(I.DeviceObject());

	// This should be a read
	ASSERT ( I.MajorFunction() == IRP_MJ_READ );

	// Start processing request.
	pDev->SerialRead(I);
}

////////////////////////////////////////////////////////////////////////////////
// StartIo (member of S5933DeviceWriteQueue)
//	
//		This routine is called when an IRP is taken off
//		the Driver Managed Queue (used for serializing I/O) and
//		presented for processing.
//
// Input
//		I			IRP removed from queue
//
// Output
//		None
//
// Notes
//
VOID S5933DeviceWriteQueue::StartIo(KIrp I)
{
	t  << "Entering S5933DeviceWriteQueue StartIo, " << I;

	// Find the device class so we can call the serialized
	// routines in the device class.  The handlers can be
	// moved to the DriverManagedQueue class if it is more
	// convenient.
	S5933Device *pDev = (S5933Device *) KDevicePTR(I.DeviceObject());

	// This should be a write
	ASSERT ( I.MajorFunction() == IRP_MJ_WRITE );

	// Start processing request.
	pDev->SerialWrite(I);
}

////////////////////////////////////////////////////////////////////////////////
// StartIo (member of S5933DevicePostMsgQueue)
//	
//		This routine is called when an IRP is taken off
//		the Driver Managed Queue (used for serializing I/O) and
//		presented for processing.
//
// Input
//		I			IRP removed from queue
//
// Output
//		None
//
// Notes
//

VOID S5933DevicePostMsgQueue::StartIo(KIrp I)
{
	t  << "Entering S5933DevicePostMsgQueue StartIo, " << I;

	// Find the device class so we can call the serialized
	// routines in the device class.  The handlers can be
	// moved to the DriverManagedQueue class if it is more
	// convenient.
	S5933Device *pDev = (S5933Device *) KDevicePTR(I.DeviceObject());

	// This should be a device control for posting a message
	ASSERT ( I.MajorFunction() == IRP_MJ_DEVICE_CONTROL );
	ASSERT ( I.IoctlCode() == S5933_IOCTL_801_POST_MSG );

	// Start processing request.
	pDev->S5933_IOCTL_801_POST_MSG_Handler(I);
}

////////////////////////////////////////////////////////////////////////////////
// StartIo (member of S5933DeviceRetrieveMsgQueue)
//	
//		This routine is called when an IRP is taken off
//		the Driver Managed Queue (used for serializing I/O) and
//		presented for processing.
//
// Input
//		I			IRP removed from queue
//
// Output
//		None
//
// Notes
//

VOID S5933DeviceRetrieveMsgQueue::StartIo(KIrp I)
{
	t  << "Entering S5933DeviceRetrieveMsgQueue StartIo, " << I;

	// Find the device class so we can call the serialized
	// routines in the device class.  The handlers can be
	// moved to the DriverManagedQueue class if it is more
	// convenient.
	S5933Device *pDev = (S5933Device *) KDevicePTR(I.DeviceObject());

	// Start processing request.
	// This should be a device control for retrieving a message
	ASSERT ( I.MajorFunction() == IRP_MJ_DEVICE_CONTROL );
	ASSERT ( I.IoctlCode() == S5933_IOCTL_802_RETRIEVE_MSG );

	// Start processing request.
	pDev->S5933_IOCTL_802_RETRIEVE_MSG_Handler(I);
}


