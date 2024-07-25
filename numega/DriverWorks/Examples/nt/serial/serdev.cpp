// serdev.cpp - device class implementation for serial driver example
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
#include "serdev.h"
#include "glbtrace.h"

// Class SerialDevice implements an abstract serial device that conforms
// to the NT serial interface. It is designed as a base class from which
// you can derive classes specific to a particular kind of hardware, most
// obviously a UART type device. 
//
// IRPs are managed as follows. 
// 1. IOCTLs are handled immediately, with the exception of 
//    IOCTL_SERIAL_WAIT_ON_MASK.
//
// 2. Reads are queued to data member m_ReadIrpQueue, an instance of
//	  SerialReadQueue, which is derived from KDriverManagedQueueEx.
//
// 3. Writes and flushes are queued to data member m_WriteIrpQueue, an
//    an instance of SerialWriteQueue, also derived from KDriverManagedQueueEx.
//
// 4. IOCTL_SERIAL_SET_WAIT_MASK and IOCTL_SERIAL_WAIT_ON_MASK are 
//    serialized through m_WaitIrpQueue, an instance of SerialWaitQueue.
//
// 5. Create, CleanUp, Close, SetFileInformation, and QueryFileInformation
//    are handled immediately.
//
// The base class takes care of most all the IRP processing, so derived
// classes do not use DEVMEMBER_DISPATCHERS. Instead, they declare
// overrides of the IRP handling member functions that require special
// treatment.
//
// See serrdwr.cpp for information on how subclasses implement read/write
// functionality, and serevent.cpp for information on how subclasses implement
// event generation.
//

/////////////////////////////////////////////////////////////////////////////
// Begin INIT code section
#pragma code_seg("INIT")

/////////////////////////////////////////////////////////////////////////////
// Constructor
//
SerialDevice::SerialDevice(
		PCWSTR DeviceName,
		PCWSTR LinkName,
		ULONG Characteristics,
		ULONG Flags
		) :

	KDevice(
		DeviceName,
		FILE_DEVICE_SERIAL_PORT,
		NULL,					// create unprotected link later
		Characteristics,
		Flags
		),

	m_ReadMutex(ULONG(0))
{
	GTRACE((TLEVEL,"Constructor for Serial Device\n"));

// Initialize the IRP queues
	m_ReadIrpQueue.SetDevice(this);
	m_WriteIrpQueue.SetDevice(this);
	m_WaitIrpQueue.SetDevice(this);

// Set up all DPCs
	m_ReadCompleteDpc.Setup(LinkTo(ReadComplete), this);
	m_WriteCompleteDpc.Setup(LinkTo(WriteComplete), this);
	m_EventDpc.Setup(LinkTo(EventDetected), this);
	m_StartReadTimerDpc.Setup(LinkTo(StartReadTimer), this);
	m_StartWriteTimerDpc.Setup(LinkTo(StartWriteTimer), this);

	if (LinkName)
	{
		if ( (m_Link = new (PagedPool) WCHAR[ustrlen(LinkName)+1]) != NULL )
		{
			m_ConstructorStatus = CreateLink(LinkName, FALSE); // unprotected link
			ustrcpy(m_Link, LinkName);
		}
		else
			m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
	}
	else
		m_Link = NULL;

// Zero out miscellaneous data members
	m_WaitMask = 0;
#if NTVERSION > 351
	IoctlClearStatistics();
#endif
	RtlZeroMemory(&m_Timeouts, sizeof(SERIAL_TIMEOUTS));
	RtlZeroMemory(&m_Chars, sizeof(SERIAL_CHARS));
	RtlZeroMemory(&m_Handflow, sizeof(SERIAL_HANDFLOW));
	m_ReadBuffer = NULL;
	m_WriteBuffer = NULL;
	m_ReadCount = 0;
	m_WriteCount = 0;
	m_SupportedEvents = 0;
	m_EventState = 0;
	m_ErrorState = 0;
	m_DeviceMapName = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// AddDeviceMapEntry
//
// This function writes the device map entry. It is the responsibility of
// the subclass to call this if it constructs successfully.
//
NTSTATUS SerialDevice::AddDeviceMapEntry(PCWSTR DeviceName, PCWSTR LinkName)
{
	m_DeviceMapName = new (PagedPool) WCHAR[ustrlen(DeviceName)+1];
	if ( !m_DeviceMapName )
		return STATUS_INSUFFICIENT_RESOURCES;

	ustrcpy(m_DeviceMapName, DeviceName);

	KRegistryKey DeviceMapKey(
		REGISTRY_DEVICEMAP,
		L"SERIALCOMM",
		TRUE,
		REG_OPTION_VOLATILE,
		KEY_DEFAULT_ACCESS | KEY_SET_VALUE
		);

	if ( NT_SUCCESS(DeviceMapKey.LastError()) )
		DeviceMapKey.WriteValue(DeviceName, LinkName);

	GTRACE((TLEVEL, "AddDeviceMapEntry status=%x\n", DeviceMapKey.LastError()));

	return DeviceMapKey.LastError();
}

#pragma code_seg()
// End INIT code section
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Destructor
//
SerialDevice::~SerialDevice(void)
{
	GTRACE((TLEVEL,"Destructor for Serial Device\n"));

	if (m_Link)
	{
		DestroyLink(m_Link);
		delete m_Link;
	}

	if (m_DeviceMapName != NULL)
	{
		KRegistryKey DeviceMapKey(
			REGISTRY_DEVICEMAP,
			L"SERIALCOMM",
			TRUE,
			REG_OPTION_VOLATILE,
			KEY_DEFAULT_ACCESS | KEY_SET_VALUE
			);

		DeviceMapKey.DeleteValue(m_DeviceMapName);

		delete m_DeviceMapName;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Create
//
// This is an exclusive device.
//
NTSTATUS SerialDevice::Create(KIrp I)
{
	GTRACE((TLEVEL,"SerialDevice::Create\n"));

#if NTVERSION > 351
	IoctlClearStatistics();
#endif

	return DefaultDispatch(I);
}

/////////////////////////////////////////////////////////////////////////////
// Close
//
NTSTATUS SerialDevice::Close(KIrp I)
{
	GTRACE((TLEVEL,"SerialDevice::Close\n"));
	return DefaultDispatch(I);
}

/////////////////////////////////////////////////////////////////////////////
// Read
//
NTSTATUS SerialDevice::Read(KIrp I)
{
//	GTRACE((TLEVEL,"SerialDevice::Read\n"));

	NTSTATUS status;

	m_ReadMutex.Wait();

	status = m_ReadIrpQueue.MaybeQueueIrp(I);

	m_ReadMutex.Release();

	return status;
}

/////////////////////////////////////////////////////////////////////////////
// Write
//
NTSTATUS SerialDevice::Write(KIrp I)
{
	GTRACE((TLEVEL,"SerialDevice::Write\n"));

	return m_WriteIrpQueue.QueueIrp(I);
}

/////////////////////////////////////////////////////////////////////////////
// CleanUp
//
NTSTATUS SerialDevice::CleanUp(KIrp I)
{
	GTRACE((TLEVEL,"SerialDevice::CleanUp\n"));

	m_ReadIrpQueue.CleanUp(I.FileObject());
	m_WriteIrpQueue.CleanUp(I.FileObject());
	m_WaitIrpQueue.CleanUp(I.FileObject());

	CancelCurrentRead();
	CancelCurrentWrite();
	CancelCurrentEventWait();

	return DefaultDispatch(I);
}

/////////////////////////////////////////////////////////////////////////////
// SetInformation
//
NTSTATUS SerialDevice::SetInformation(KIrp I)
{
	GTRACE((TLEVEL,"SerialDevice::SetInformation\n"));
	NTSTATUS status;

	I.Information() = 0;

	FILE_INFORMATION_CLASS fc = I.SetFileInformationClass();

	if ( (fc==FileEndOfFileInformation) || (fc==FileAllocationInformation) )
		status = STATUS_SUCCESS;
	else
		status = STATUS_INVALID_PARAMETER;

	return I.Complete(status);
}

/////////////////////////////////////////////////////////////////////////////
// QueryInformation
//
NTSTATUS SerialDevice::QueryInformation(KIrp I)
{
	GTRACE((TLEVEL,"SerialDevice::QueryInformation\n"));

	FILE_INFORMATION_CLASS fc = I.QueryFileInformationClass();

	NTSTATUS status = STATUS_SUCCESS;

	switch (fc)
	{
	case FileStandardInformation:
	{
        PFILE_STANDARD_INFORMATION si = (PFILE_STANDARD_INFORMATION)I.BufferedReadDest();
		si->AllocationSize.QuadPart = 0;
		si->EndOfFile = si->AllocationSize;
		si->NumberOfLinks = 0;
		si->DeletePending = FALSE;
		si->Directory = FALSE;
		I.Information() = sizeof(FILE_STANDARD_INFORMATION);		
		break;
	}
	case FilePositionInformation:
	{
        PFILE_POSITION_INFORMATION pi = (PFILE_POSITION_INFORMATION)I.BufferedReadDest();
		pi->CurrentByteOffset.QuadPart = 0;
		I.Information() = sizeof(FILE_POSITION_INFORMATION);
		break;
	}
	default:
		I.Information() = 0;
		status = STATUS_INVALID_PARAMETER;
	}

	return I.Complete(status);
}

/////////////////////////////////////////////////////////////////////////////
// FlushBuffers
//
NTSTATUS SerialDevice::FlushBuffers(KIrp I)
{
	GTRACE((TLEVEL,"SerialDevice::FlushBuffers\n"));

// Flush goes to the write queue.

	return m_WriteIrpQueue.QueueIrp(I);
}
