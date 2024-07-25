// S5933IsaDevice.cpp
// Implementation of S5933IsaDevice device class

#include <vdw.h>

#include "S5933Isa.h"
#include "S5933IsaDevice.h"
#include "..\S5933Isaioctl.h"
#include "..\ao59xxrd.h"

#pragma warning(disable:4065) // Allow switch statement with no cases

extern	KTrace	t;			// Global driver trace object	

////////////////////////////////////////////////////////////////////////
// S5933IsaDevice Destructor
//
//		This is the destructor for the S5933IsaDevice
//
// Input
//		None
//
// Output
//		None
//
// Notes
//		Disconnect and release resources here.
//
//		Although the use of SAFE_DESTRUCTORS in the class definition cures
//		improper emission of the destructor into the INIT section most of the
//		time, certain rare cases can still cause improper behavior.  To avoid
//		these cases, the destructor	must preceed the INIT section, causing it
//		to be referenced first by the default compiler section.

S5933IsaDevice::~S5933IsaDevice()
{
	ReleaseResources(*m_RegPath);
	delete m_RegPath;
}

#pragma code_seg("INIT")

////////////////////////////////////////////////////////////////////////////////
// S5933IsaDevice Constructor
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

S5933IsaDevice::S5933IsaDevice(ULONG Unit) :
	KDevice(
		KUnitizedName(L"S5933IsaDevice", Unit), 
		FILE_DEVICE_UNKNOWN,
		KUnitizedName(L"S5933IsaDevice", Unit),
		0,
		DO_DIRECT_IO
		),
	m_ReadQueue(),
	m_WriteQueue(),
	m_AddonOperationRegisters(),
	m_TransferCountRegisters(),
	m_AddonIncomingMailbox1(),
	m_AddonIncomingMailbox2(),
	m_AddonIncomingMailbox3(),
	m_AddonIncomingMailbox4(),
	m_AddonOutgoingMailbox1(),
	m_AddonOutgoingMailbox2(),
	m_AddonOutgoingMailbox3(),
	m_AddonOutgoingMailbox4(),
	m_AddonFIFORegisterPort(),
	m_MasterWriteAddress(),
	m_AddonPassThruAddress(),
	m_AddonPassThruData(),
	m_MasterReadAddress(),
	m_AddonMailboxEmptyFullStatus(),
	m_AddonInterruptControl(),
	m_AddonGeneralControlStatus(),
	m_MasterWriteTransferCount(),
	m_MasterReadTransferCount()
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create device S5933IsaDevice unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;
		return;
	}
	m_Unit = Unit;

	m_RegPath = CreateRegistryPath(L"S5933IsaDevice", Unit);
	if (m_RegPath == NULL)
	{
		// Error, cannot allocate memory for registry path
		t << "Failed to create registry path\n";
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}

	// Claim and initialize resources assigned to this device
	m_ConstructorStatus = InitResources();
}

////////////////////////////////////////////////////////////////////////
// InitResources (member of S5933IsaDevice)
//
//		Claim and initialize hardware resources for this device
//
// Input
//		None
//
// Output	
//		NTSTATUS		STATUS_SUCCESS, or an error code
//
// Notes
//		

NTSTATUS S5933IsaDevice::InitResources(void)
{
	NTSTATUS	status;

	KRegistryKey Params((KDriver::DriverInstance())->RegistryPath(), L"\\Parameters");
	if (!NT_SUCCESS(Params.LastError()))
	{
		// Error, cannot open device registry key
		t << "Failed to create registry key\n";
	    return STATUS_UNSUCCESSFUL;			
	}

	// Create a resource request object, then add requests for each of the
	// resources required.
	t << "Building resource request for ISA device\n";

	KResourceRequest theRequest(Isa, (ULONG) 0, (ULONG) 0);
    PHYSICAL_ADDRESS theAddr;
	theAddr.HighPart = theAddr.LowPart = 0;

	// Query the registry to determine the address to use.
	Params.QueryValue(L"AddonOperationRegisters_high", (PULONG) &theAddr.HighPart);
	Params.QueryValue(L"AddonOperationRegisters_low", (PULONG) &theAddr.LowPart);
	if (!NT_SUCCESS(Params.LastError()))
	{
		// Error, cannot open registry key
		t << "Registry key load failed for key AddonOperationRegisters_low\n";
	    return STATUS_UNSUCCESSFUL;			
	}
	else
	{
		t << "AddonOperationRegisters_low loaded from registry, resulting value: [" << theAddr.LowPart << "]\n";
	}

	status = theRequest.AddPort(
		theAddr,
		theAddr,
		AMCCS5933_DK1_ADDON_OPERATION_REGISTERS_SIZE,
		1,
		0,
		CmResourceShareDeviceExclusive);
	if (!NT_SUCCESS(status)) return status;

	// Query the registry to determine the address to use.
	Params.QueryValue(L"TransferCountRegisters_high", (PULONG) &theAddr.HighPart);
	Params.QueryValue(L"TransferCountRegisters_low", (PULONG) &theAddr.LowPart);
	if (!NT_SUCCESS(Params.LastError()))
	{
		// Error, cannot open registry key
		t << "Registry key load failed for key TransferCountRegisters_low\n";
	    return STATUS_UNSUCCESSFUL;			
	}
	else
	{
		t << "TransferCountRegisters_low loaded from registry, resulting value: [" << theAddr.LowPart << "]\n";
	}

	status = theRequest.AddPort(
		theAddr,
		theAddr,
		AMCCS5933_DK1_ADDON_TRANSFER_REGISTERS_SIZE,
		1,
		0,
		CmResourceShareDeviceExclusive);
	if (!NT_SUCCESS(status)) return status;

    // Submit the request to the system
    status = theRequest.Submit(this, *m_RegPath);

	if (!NT_SUCCESS(status)) return status;


	// For each port I/O mapped region, setup a KResourceAssignment to probe
	// the assigned resource. Initialize the port I/O mapped range using
	// the start address and length obtained from the instance of
	// KResourceAssignment. Once initialized, use member functions such as
	// inb/outb, or the array element operator to access the port range.
	KResourceAssignment AssignedAddonOperationRegisters(&theRequest, CmResourceTypePort, 0);

	// Confirm that the assignment was successful
	if (!AssignedAddonOperationRegisters.Status())
	{
		// Error, could not assign required resource
		return STATUS_UNSUCCESSFUL;
	}

	status = m_AddonOperationRegisters.Initialize(
				Isa,												// Bus Type
				0,													// Bus Number
				AssignedAddonOperationRegisters.Start().QuadPart,	// Start Address
				AssignedAddonOperationRegisters.Length(),			// Length
				TRUE);

	if (!NT_SUCCESS(status)) return status;

	KResourceAssignment AssignedTransferCountRegisters(&theRequest, CmResourceTypePort, 1);

	// Confirm that the assignment was successful
	if (!AssignedTransferCountRegisters.Status())
	{
		// Error, could not assign required resource
		return STATUS_UNSUCCESSFUL;
	}

	status = m_TransferCountRegisters.Initialize(
				Isa,												// Bus Type
				0,													// Bus Number
				AssignedTransferCountRegisters.Start().QuadPart,	// Start Address
				AssignedTransferCountRegisters.Length(),			// Length
				TRUE);

	if (!NT_SUCCESS(status)) return status;

	m_AddonIncomingMailbox1 = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonIncomingMailbox1)];
	m_AddonIncomingMailbox2 = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonIncomingMailbox2)];
	m_AddonIncomingMailbox3 = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonIncomingMailbox3)];
	m_AddonIncomingMailbox4 = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonIncomingMailbox4)];
	m_AddonOutgoingMailbox1 = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonOutgoingMailbox1)];
	m_AddonOutgoingMailbox2 = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonOutgoingMailbox2)];
	m_AddonOutgoingMailbox3 = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonOutgoingMailbox3)];
	m_AddonOutgoingMailbox4 = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonOutgoingMailbox4)];
	m_AddonFIFORegisterPort = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonFIFORegisterPort)];
	m_MasterWriteAddress = 
		m_AddonOperationRegisters[OP_REG_OFFSET(MasterWriteAddress)];
	m_AddonPassThruAddress = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonPassThruAddress)];
	m_AddonPassThruData = m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonPassThruData)];
	m_MasterReadAddress = 
		m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(MasterReadAddress)];
	m_AddonMailboxEmptyFullStatus = 
		m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonMailboxEmptyFullStatus)];
	m_AddonInterruptControl = 
		m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonInterruptControl)];
	m_AddonGeneralControlStatus = 
		m_AddonOperationRegisters[ADDON_OP_REG_OFFSET(AddonGeneralControlStatus)];

	m_MasterWriteTransferCount = m_TransferCountRegisters[0];
	m_MasterReadTransferCount = m_TransferCountRegisters[4];

	return STATUS_SUCCESS;
}

#pragma code_seg()

////////////////////////////////////////////////////////////////////////
// SerialRead (member of S5933IsaDevice)
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

void S5933IsaDevice::SerialRead(KIrp I)
{
	t << "Entering S5933IsaDevice::SerialRead, " << I;
	NTSTATUS status		= STATUS_SUCCESS;

	// Declare a memory object
	KMemory Mem(I.Mdl());

	// Use the memory object to create a pointer to the caller's buffer
	PULONG	pBuffer		= static_cast<PULONG>(Mem.MapToSystemSpace());

	ULONG   dwTotalSize = I.ReadSize(CURRENT);	// Requested read size
	ULONG	dwUlongs = dwTotalSize / sizeof(ULONG);
	ULONG   dwBytesRead = 0;					// Count of bytes read
	ULONG	AddonGeneralControlStatus;
	ULONG	Retries;

    for (; dwUlongs > 0; dwUlongs--)
	{
        Retries = AMCCS5933_DK1_RETRIES;
        while (( (AddonGeneralControlStatus = m_AddonGeneralControlStatus) &
			KS59XX_AGCSTS_PCI_TO_ADDON_FIFO_EMPTY )	&&
			Retries--)
		{
			LARGE_INTEGER Pause;
			Pause.QuadPart = -10000 * static_cast<LONGLONG>(AMCCS5933_DK1_TIMEOUT_MS);
			KeDelayExecutionThread(KernelMode, TRUE, &Pause);
        }

		if (AddonGeneralControlStatus & KS59XX_AGCSTS_PCI_TO_ADDON_FIFO_EMPTY)
		{
			dwBytesRead = 0;
			status = STATUS_DEVICE_NOT_READY;
			break;
		}

        *pBuffer++ = m_AddonFIFORegisterPort;
		dwBytesRead += sizeof(ULONG);
    }

	I.Information() = dwBytesRead;
	I.Status() = status;

	// NextIrp completes this IRP and starts processing 
	// for the next IRP in the driver managed queue.
	m_ReadQueue.NextIrp(I);
}

////////////////////////////////////////////////////////////////////////
// Read (member of S5933IsaDevice)
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

NTSTATUS S5933IsaDevice::Read(KIrp I)
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
// SerialWrite (member of S5933IsaDevice)
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

void S5933IsaDevice::SerialWrite(KIrp I)
{
	t << "Entering S5933IsaDevice::SerialWrite, " << I;
	NTSTATUS status		= STATUS_SUCCESS;

	// Declare a memory object
	KMemory Mem(I.Mdl());
	// Use the memory object to create a pointer to the caller's buffer
	PULONG	pBuffer		= static_cast<PULONG>(Mem.MapToSystemSpace());

	ULONG   dwTotalSize = I.WriteSize(CURRENT);
	ULONG	dwUlongs = dwTotalSize / sizeof(ULONG);
	ULONG   dwBytesSent = 0;
	ULONG	AddonGeneralControlStatus;
	ULONG	Retries;

    for (; dwUlongs > 0; dwUlongs--)
	{
        Retries = AMCCS5933_DK1_RETRIES;
        while (( (AddonGeneralControlStatus = m_AddonGeneralControlStatus) &
			KS59XX_AGCSTS_ADDON_TO_PCI_FIFO_FULL )	&&
			Retries--)
		{
			LARGE_INTEGER Pause;
			Pause.QuadPart = -10000 * static_cast<LONGLONG>(AMCCS5933_DK1_TIMEOUT_MS);
			KeDelayExecutionThread(KernelMode, TRUE, &Pause);
        }

		if (AddonGeneralControlStatus & KS59XX_AGCSTS_ADDON_TO_PCI_FIFO_FULL)
		{
			dwBytesSent = 0;
			status = STATUS_DEVICE_NOT_READY;
			break;
		}

        m_AddonFIFORegisterPort = *pBuffer++;
		dwBytesSent += sizeof(ULONG);
    }


	I.Information() = dwBytesSent;
	I.Status() = status;

	// NextIrp completes this IRP and starts processing 
	// for the next IRP in the driver managed queue.
	m_WriteQueue.NextIrp(I);
}


////////////////////////////////////////////////////////////////////////
// Write (member of S5933IsaDevice)
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

NTSTATUS S5933IsaDevice::Write(KIrp I)
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
// DeviceControl (member of S5933IsaDevice)
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

NTSTATUS S5933IsaDevice::DeviceControl(KIrp I) 
{
	NTSTATUS status;

	t << "Entering S5933IsaDevice::Device Control, " << I;
	switch (I.IoctlCode())
	{
		case S5933ISA_IOCTL_800_POST_MSG:
		    status = S5933ISA_IOCTL_800_POST_MSG_Handler(I);
			break;

		case S5933ISA_IOCTL_801_RETRIEVE_MSG:
		    status = S5933ISA_IOCTL_801_RETRIEVE_MSG_Handler(I);
			break;

		case S5933ISA_IOCTL_802_READ_PASSTHRU:
		    status = S5933ISA_IOCTL_802_READ_PASSTHRU_Handler(I);
			break;

		case S5933ISA_IOCTL_803_DUMP_REGISTERS:
		    status = S5933ISA_IOCTL_803_DUMP_REGISTERS_Handler(I);
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
		return I.Complete(status);
	}
}

////////////////////////////////////////////////////////////////////////
//	S5933ISA_IOCTL_800_POST_MSG_Handler (member of S5933IsaDevice)
//
//		Handler for IO Control Code S5933ISA_IOCTL_800_POST_MSG
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933ISA_IOCTL_800_POST_MSG function.
//		This function was queued, so this
//		handler is serialized with other requests.
//		This routine runs at dispatch level.
//

NTSTATUS S5933IsaDevice::S5933ISA_IOCTL_800_POST_MSG_Handler(KIrp I)
{
	t << "Entering S5933IsaDevice::S5933ISA_IOCTL_800_POST_MSG_Handler, " << I;

	// Make sure input parameters look good
	if(I.IoctlInputBufferSize() < sizeof(KS5933ISA_MAILBOX_MESSAGE) || !I.IoctlBuffer())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	// Get a pointer to our message structure
	PKS5933ISA_MAILBOX_MESSAGE pMsg = static_cast<PKS5933ISA_MAILBOX_MESSAGE>(I.IoctlBuffer());

	// Check the mailbox parameters.  This could be done in our call to set the
	// interrupt, but if we found an error, we would have to check for cancellation
	// before completing, since we would have already setup our cancellation routine.
	// This is simpler.
	if( (pMsg->MessageMailBox < KS59XX_MAILBOX_MINIMUM)
		|| (pMsg->MessageMailBox > KS59XX_MAILBOX_MAXIMUM) )
	{
		I.Status() = STATUS_INVALID_PARAMETER;
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
    }	
	
	// Post our message to the S5933 object's mailbox.
	switch(pMsg->MessageMailBox)
	{
	case 1:
		m_AddonOutgoingMailbox1 = pMsg->MessageValue;
		break;
	case 2:
		m_AddonOutgoingMailbox2 = pMsg->MessageValue;
		break;
	case 3:
		m_AddonOutgoingMailbox3 = pMsg->MessageValue;
		break;
	case 4:
		m_AddonOutgoingMailbox4 = pMsg->MessageValue;
		break;
	default:
		// This can't happen since it would have been caught above when we checked
		// the parameters.
		ASSERT(FALSE);
		break;
	}

    I.Information() = 0;
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//	S5933ISA_IOCTL_801_RETRIEVE_MSG_Handler (member of S5933IsaDevice)
//
//		Handler for IO Control Code S5933ISA_IOCTL_801_RETRIEVE_MSG
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933ISA_IOCTL_801_RETRIEVE_MSG function.
//		This function was queued, so this
//		handler is serialized with other requests.
//		This routine runs at dispatch level.
//

NTSTATUS S5933IsaDevice::S5933ISA_IOCTL_801_RETRIEVE_MSG_Handler(KIrp I)
{
	// Make sure input parameters look good
	if(I.IoctlInputBufferSize() < sizeof(KS5933ISA_MAILBOX_MESSAGE) || !I.IoctlBuffer())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	// Get a pointer to our message structure
	PKS5933ISA_MAILBOX_MESSAGE pMsg = static_cast<PKS5933ISA_MAILBOX_MESSAGE>(I.IoctlBuffer());

	// Check the mailbox parameters.  This could be done in our call to set the
	// interrupt, but if we found an error, we would have to check for cancellation
	// before completing, since we would have already setup our cancellation routine.
	// This is simpler.
	if( (pMsg->MessageMailBox < KS59XX_MAILBOX_MINIMUM)
		|| (pMsg->MessageMailBox > KS59XX_MAILBOX_MAXIMUM) )
	{
		I.Status() = STATUS_INVALID_PARAMETER;
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
    }	
	
	// Retrieve our message from the S5933 object's mailbox.
	switch(pMsg->MessageMailBox)
	{
	case 1:
		pMsg->MessageValue = m_AddonIncomingMailbox1;
		break;
	case 2:
		pMsg->MessageValue = m_AddonIncomingMailbox2;
		break;
	case 3:
		pMsg->MessageValue = m_AddonIncomingMailbox3;
		break;
	case 4:
		pMsg->MessageValue = m_AddonIncomingMailbox4;
		break;
	default:
		// This can't happen since it would have been caught above when we checked
		// the parameters.
		ASSERT(FALSE);
		break;
	}

	I.Information() = sizeof(KS5933ISA_MAILBOX_MESSAGE);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//	S5933ISA_IOCTL_802_READ_PASSTHRU_Handler (member of S5933IsaDevice)
//
//		Handler for IO Control Code S5933ISA_IOCTL_802_READ_PASSTHRU
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933ISA_IOCTL_802_READ_PASSTHRU function.
//
//		Since this operation reads a single aligned DWORD, no synchronization
//		is required, since this operation is atomic already.
//

NTSTATUS S5933IsaDevice::S5933ISA_IOCTL_802_READ_PASSTHRU_Handler(KIrp I)
{
	t << "Entering S5933IsaDevice::S5933ISA_IOCTL_802_READ_PASSTHRU_Handler, " << I;

	// Make sure input/output parameters look good
	if(I.IoctlInputBufferSize() < sizeof(KS5933ISA_PASSTHRU) ||
		I.IoctlOutputBufferSize() < sizeof(KS5933ISA_PASSTHRU) ||
		!I.IoctlBuffer())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	// Input and output share the same buffer
	PKS5933ISA_PASSTHRU pOutput = static_cast<PKS5933ISA_PASSTHRU>(I.IoctlBuffer());

	// Read the value from the passthru region at offset 0
	pOutput->Data = m_AddonPassThruData;

    I.Information() = sizeof(pOutput);

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//	S5933ISA_IOCTL_803_DUMP_REGISTERS_Handler (member of S5933IsaDevice)
//
//		Handler for IO Control Code S5933ISA_IOCTL_803_DUMP_REGISTERS
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the S5933ISA_IOCTL_803_DUMP_REGISTERS function.
//		This routine runs at passive level.
//

NTSTATUS S5933IsaDevice::S5933ISA_IOCTL_803_DUMP_REGISTERS_Handler(KIrp I)
{
	t << "Entering S5933IsaDevice::S5933ISA_IOCTL_803_DUMP_REGISTERS_Handler, " << I;

	// make sure the size of the buffer passed down is OK
	if(I.IoctlOutputBufferSize() < sizeof(AMCCS5933_ADDON_OPERATION_REGISTERS) 
		|| !I.IoctlBuffer())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	// Get a pointer to the caller's output buffer
	PAMCCS5933_ADDON_OPERATION_REGISTERS pRegs = 
		(PAMCCS5933_ADDON_OPERATION_REGISTERS)I.IoctlBuffer();

	// Fill in all of the mailbox registers
	pRegs->AddonOutgoingMailbox1 = m_AddonOutgoingMailbox1;
	pRegs->AddonOutgoingMailbox2 = m_AddonOutgoingMailbox2;
	pRegs->AddonOutgoingMailbox3 = m_AddonOutgoingMailbox3;
	pRegs->AddonOutgoingMailbox4 = m_AddonOutgoingMailbox4;
	pRegs->AddonIncomingMailbox1 = m_AddonIncomingMailbox1;
	pRegs->AddonIncomingMailbox2 = m_AddonIncomingMailbox2;
	pRegs->AddonIncomingMailbox3 = m_AddonIncomingMailbox3;
	pRegs->AddonIncomingMailbox4 = m_AddonIncomingMailbox4;

	// Skip the FIFO port, since reading this at the wrong time can hang the bus
    pRegs->AddonFIFORegisterPort = 0;

	// Fill in all of the FIFO and status registers
    pRegs->MasterWriteAddress = m_MasterWriteAddress;
    pRegs->AddonPassThruAddress = m_AddonPassThruAddress;
    pRegs->AddonPassThruData = m_AddonPassThruData;
    pRegs->MasterReadAddress = m_MasterReadAddress;
    pRegs->AddonMailboxEmptyFullStatus = m_AddonMailboxEmptyFullStatus;
    pRegs->AddonInterruptControl = m_AddonInterruptControl;
    pRegs->AddonGeneralControlStatus = m_AddonGeneralControlStatus;

	I.Information() = sizeof(AMCCS5933_ADDON_OPERATION_REGISTERS);

	// Assuming that the request was handled:
	return STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////
// Create (member of S5933IsaDevice)
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

NTSTATUS S5933IsaDevice::Create(KIrp I)
{
	t << "Entering Create, " << I;
	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
// Close (member of S5933IsaDevice)
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

NTSTATUS S5933IsaDevice::Close(KIrp I)
{
	t << "Entering Close, " << I;
	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}


////////////////////////////////////////////////////////////////////////////////
// StartIo (member of S5933IsaDeviceReadQueue)
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

VOID S5933IsaDeviceReadQueue::StartIo(KIrp I)
{
	t  << "Entering S5933IsaDeviceReadQueue StartIo, " << I;

	// Find the device class so we can call the serialized
	// routines in the device class.  The handlers can be
	// moved to the DriverManagedQueue class if it is more
	// convenient.
	S5933IsaDevice *pDev = (S5933IsaDevice *) KDevicePTR(I.DeviceObject());

	// This should be a read
	ASSERT ( I.MajorFunction() == IRP_MJ_READ );

	// Start processing request.
	pDev->SerialRead(I);
}

////////////////////////////////////////////////////////////////////////////////
// StartIo (member of S5933IsaDeviceWriteQueue)
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
VOID S5933IsaDeviceWriteQueue::StartIo(KIrp I)
{
	t  << "Entering S5933IsaDeviceWriteQueue StartIo, " << I;

	// Find the device class so we can call the serialized
	// routines in the device class.  The handlers can be
	// moved to the DriverManagedQueue class if it is more
	// convenient.
	S5933IsaDevice *pDev = (S5933IsaDevice *) KDevicePTR(I.DeviceObject());

	// This should be a write
	ASSERT ( I.MajorFunction() == IRP_MJ_WRITE );

	// Start processing request.
	pDev->SerialWrite(I);
}

