// mmdevice.cpp -  example driver, device class
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
#include "mmioctl.h"
#include "mmdevice.h"

// This is a simple demonstration of mapping a bus physical address
// to process virtual space. In this example, the bus is assumed to
// be ISA, and there is assumed to be just one ISA bus. The address
// on the ISA bus is defaulted in mapmem.h, although DriverEntry will
// read both the bus address and memory window size from the registry.
// The address and size are recorded in the device object.

// Each time member Create is called, it means a new process is opening
// the device, and a new file object is created. This driver uses the
// FsContext field of the file object to store a pointer to the mapping
// object for the process that corresponds to the file object. 

// The mapping is not done until the process call DeviceIoControl to 
// request it. The driver returns a pointer in the process' virtual
// space to the device memory window.

// When member CleanUp is called, the process has closed all its handles
// to the driver, so the mapping can be deleted if it has not already
// been unmapped.

///////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////

#pragma code_seg("INIT")

MapMemDevice::MapMemDevice(ULONG DeviceAddress, ULONG WindowSize) :
	KDevice(L"MapMem0", FILE_DEVICE_UNKNOWN, L"MAPMEM0", 0, DO_BUFFERED_IO),
	m_Exclusive(SynchronizationEvent, TRUE)
{
	m_BusPhysicalAddress.QuadPart = DeviceAddress;
	m_WindowSize = WindowSize;
}

#pragma code_seg()

///////////////////////////////////////////////////////////////////////////
// Create - a process is opening the driver for the first time
///////////////////////////////////////////////////////////////////////////

NTSTATUS MapMemDevice::Create(KIrp I)
{
	I.FileObject()->FsContext = 0;

	return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////
// Create - a process is opening the driver for the first time
///////////////////////////////////////////////////////////////////////////

NTSTATUS MapMemDevice::Close(KIrp I)
{
	return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////
// CleanUp - all handles of the calling process for this driver are closed
///////////////////////////////////////////////////////////////////////////

NTSTATUS MapMemDevice::CleanUp(KIrp I)
{
	// Test and clear the file object context

	m_Exclusive.Wait();

	KMemoryToProcessMap* pMap = 
		(KMemoryToProcessMap*)I.FileObject()->FsContext;
	I.FileObject()->FsContext = 0;

	m_Exclusive.Set();

	// if it was mapped, delete the mapping

	if (pMap != NULL)
		delete pMap;

	return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////
// DeviceControl
///////////////////////////////////////////////////////////////////////////

NTSTATUS MapMemDevice::DeviceControl(KIrp I)
{
	NTSTATUS status;
	KMemoryToProcessMap* pMap;

	I.Information() = 0;

	switch (I.IoctlCode())
	{

	//----------------------------------------------------------------------------
	// Map the device to the calling process, return the process virtual address
	//----------------------------------------------------------------------------

	case IOCTL_MAPMEM_MAP_DEVICE:

		//--------------------------------------------------------------------
		//	If the buffer's too small, fail. 
		//--------------------------------------------------------------------

		if ( I.IoctlOutputBufferSize() < sizeof (int))

			return I.Complete(STATUS_BUFFER_TOO_SMALL);

		m_Exclusive.Wait();

		//--------------------------------------------------------------------
		// Check to see if the process has already mapped the device
		//--------------------------------------------------------------------

		pMap = (KMemoryToProcessMap*)I.FileObject()->FsContext;

		//--------------------------------------------------------------------
		// If not, create a new mapping for the physical address.
		//--------------------------------------------------------------------

		if (pMap == 0)	
		{
			unsigned int *buffer = (unsigned int *) I.UserBuffer(); 

			m_BusPhysicalAddress.QuadPart = buffer[0];
			
			HANDLE prochandle = (HANDLE *) buffer[1];

			pMap = new(NonPagedPool) KMemoryToProcessMap(
				Isa,
				0,
				m_BusPhysicalAddress.QuadPart,
				m_WindowSize,
				prochandle, 
				TRUE
				);
		}

		//--------------------------------------------------------------------
		// If there is now a mapping, move the data to the user buffer. 
		//--------------------------------------------------------------------

		if (pMap)			
		{
			status = pMap->ConstructorStatus();
			if ( NT_SUCCESS(status) )
			{
				*((ULONG *) I.IoctlBuffer()) = (ULONG) pMap->ProcessAddress(); 
				I.Information() = sizeof (ULONG); 
			}
		}
		else
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
		}

		//--------------------------------------------------------------------
		// Finally, complete the I/O. 
		//--------------------------------------------------------------------

		m_Exclusive.Set();
		return I.Complete(status);

	//----------------------------------------------------------------------------
	// Unmap the device for the calling process.
	//----------------------------------------------------------------------------

	case IOCTL_MAPMEM_UNMAP_DEVICE:

		m_Exclusive.Wait();
		delete (KMemoryToProcessMap*)I.FileObject()->FsContext;
		I.FileObject()->FsContext = 0;
		m_Exclusive.Set();
		return I.Complete(STATUS_SUCCESS);

	//----------------------------------------------------------------------------
	// If any other IoControl, say we don't handle it. 
	//----------------------------------------------------------------------------

	default:

		return I.Complete(STATUS_INVALID_PARAMETER);

	}
}
