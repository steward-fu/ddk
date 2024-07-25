// Device class for KPeripheralAddress test driver
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
#include <busaddev.h>
#include <common.h>

/////////////////////////////////////////////////////
// Constructor for device object
//
#pragma code_seg("INIT")

BusAddressTestDevice::BusAddressTestDevice() :
	KDevice(
		L"BusAddr0",
		FILE_DEVICE_UNKNOWN,
		L"BusAddr0"
		), 

		m_CmosPorts(Isa, 0, 0x70, 2, TRUE)
{
	if ( !NT_SUCCESS(m_ConstructorStatus) )
		return;
}
#pragma code_seg()

/////////////////////////////////////////////////////
// Destructor for device object
//
BusAddressTestDevice::~BusAddressTestDevice()
{
}

/////////////////////////////////////////////////////
// Close
//
// Clean up the VGA mapping if still present
//
NTSTATUS BusAddressTestDevice::Close(KIrp I)
{
	delete (KMemoryToProcessMap*)I.FileObject()->FsContext;
	return I.Complete(STATUS_SUCCESS);
}

/////////////////////////////////////////////////////
// Create
//
// In the create routine, set up a process mapping
// for the process that owns the current file object
//
NTSTATUS BusAddressTestDevice::Create(KIrp I)
{
	ULONGLONG vgatext = 0xb8000;

	KMemoryToProcessMap* p = new (NonPagedPool) 
		KMemoryToProcessMap(
			Isa,
			0,
			vgatext,
			50*80*2, 
			(HANDLE)-1,
			TRUE
			);

	if (p && NT_SUCCESS(p->ConstructorStatus()))
		I.FileObject()->FsContext = p;
	else
	{
		DbgPrint("Failed to map VGA in Create\n");
		I.FileObject()->FsContext = 0;
	}

	return I.Complete(STATUS_SUCCESS);
}

/////////////////////////////////////////////////////
// Read
//
// The read function reads data from the VGA Text
// mapping
// 
NTSTATUS BusAddressTestDevice::Read(KIrp I)
{
	ULONG readoff = I.ReadOffset().LowPart;
	ULONG readcnt = I.ReadSize();

	KMemoryToProcessMap* VGAText = 
		(KMemoryToProcessMap*)I.FileObject()->FsContext;

	if (readoff + readcnt > VGAText->Count())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	PVOID addr = (PVOID)((PUCHAR)VGAText->SystemAddress()+readoff);

	memcpy(I.BufferedReadDest(), addr, readcnt);

	I.Information() = readcnt;

	return I.Complete(STATUS_SUCCESS);
}

/////////////////////////////////////////////////////
// Write
//
// The write function writes data to the VGA Text 
// mapping.
// 
NTSTATUS BusAddressTestDevice::Write(KIrp I)
{
	ULONG writeoff = I.WriteOffset().LowPart;
	ULONG writecnt = I.WriteSize();

	KMemoryToProcessMap* VGAText = 
		(KMemoryToProcessMap*)I.FileObject()->FsContext;

	if (writeoff + writecnt > VGAText->Count())
	{
		I.Information() = 0;
		return STATUS_INVALID_PARAMETER;
	}

	PVOID addr = (PVOID)((PUCHAR)VGAText->SystemAddress()+writeoff);

	memcpy(addr, I.BufferedWriteSource(), writecnt);
	I.Information() = writecnt;

	return I.Complete(STATUS_SUCCESS);
}

/////////////////////////////////////////////////////
// DeviceControl
//
NTSTATUS BusAddressTestDevice::DeviceControl(KIrp I)
{
	NTSTATUS status;
	I.Information() = 0;

	switch (I.IoctlCode())
	{

// The GETDATE function reads the calendar date from
// the CMOS RAM.

	  case IOCTL_BUSADDR_GETDATE:
	  {
		if (I.IoctlOutputBufferSize() < sizeof(DATEINFO) )
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

	  	DATEINFO* p = (DATEINFO*)I.IoctlBuffer();
		p->day = ReadCmosPort(7);
		p->month = ReadCmosPort(8);
		p->year = ReadCmosPort(9);

		I.Information() = sizeof(DATEINFO);
		status = STATUS_SUCCESS;
		break;
	  }

// Map VGATEXT returns the process virtual address to the
// VGA text area.

	  case IOCTL_BUSADDR_MAP_VGATEXT:
	  {
		KMemoryToProcessMap* p =
			(KMemoryToProcessMap*)I.FileObject()->FsContext;

		*(PVOID*)I.IoctlBuffer() = p->ProcessAddress();

		I.Information() = sizeof(PVOID);
		status = STATUS_SUCCESS;
		break;
	  }	

	  default:
		status = STATUS_INVALID_PARAMETER;
	}

	return I.Complete(status);
}

/////////////////////////////////////////////////////
// ReadCmosPort
//
UCHAR BusAddressTestDevice::ReadCmosPort(UCHAR offset)
{
	m_CmosPorts[0] = offset;
	return m_CmosPorts[1];
}
