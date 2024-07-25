// rdcontrol.cpp - Code for control device class for ramdisk driver.
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

////////////////////////////////////////////////////////////////////////////
//
//	RamdiskControlDevice
//
//	This device provides the interface between the ramdisk device and the
//	GUI application.  It is responsible for handling requests to create
//	and delete the RAMDISK.
//	
#include <vdw.h>
#include "../common.h"
#include "rddevice.h"
#include "rdcntrl.h"

///////////////////////////////////////////////////////////////////////////
// Begin initialization time code
//
#pragma code_seg("INIT")
///////////////////////////////////////////////////////////////////////////
// Constructor
//
RamdiskControlDevice::RamdiskControlDevice(void) :
	KDevice( L"Ramdisk", FILE_DEVICE_UNKNOWN, L"Ramdisk" ),
	m_Ramdisk(NULL),
	m_Busy(FALSE)
{
// Make sure the base class construction was successful
	
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
#if DBG
		DbgPrint("RAMDISK: Failed to create device: %x\n",
			 m_ConstructorStatus);
#endif
		return;
	}
}
#pragma code_seg()
// End initialization time code
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// DeviceControl
// 
// The GUI application makes three IOCTL requests: one to create a disk,
// one requesting that the drive letter be deleted, and one requesting
// that the device itself be deleted.
//
// During removal of the disk, the GUI first locks the volume, ensuring
// that no other applications have open handles to it.  The volume is
// then dismounted from the system.  It then requests that the drive 
// letter be deleted (this simply deletes the symbolic link to the device), 
// ensuring that no new handles will be opened to it.  Finally, the 
// GUI's handle to device is closed, and the device object is be deleted.
//
NTSTATUS RamdiskControlDevice::DeviceControl(KIrp I)
{
	NTSTATUS status;
	
	I.Information() = 0;

	switch (I.IoctlCode())
	{

 // Create a new ram disk.
	case IOCTL_CREATE:
	{
		struct CREATE_PARMS * parms;
		WCHAR LinkName[3];

		parms = (struct CREATE_PARMS *) I.IoctlBuffer();

		// Ensure disk is not already created
		if ( m_Ramdisk != NULL ) 
		{
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		// Ensure parameters are reasonable
		if ( parms->DriveLetter < 'A'		||
		     parms->DriveLetter > 'Z'		||
		     parms->nRootDirEntries < 10	||
		     parms->nSectorsPerCluster == 0	||
		     parms->DiskSize < 10*512*parms->nSectorsPerCluster )
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		// Create the ram disk
		LinkName[0] = parms->DriveLetter;
		LinkName[1] = ':';
		LinkName[2] = '\0';

		m_Ramdisk = new(NonPagedPool) RamdiskDevice( 
				LinkName,
				parms->nRootDirEntries,
				parms->nSectorsPerCluster,
				parms->DiskSize
				);

		// Check creation status
		if ( m_Ramdisk == NULL  ||
		     !NT_SUCCESS(m_Ramdisk->ConstructorStatus()) )
		{
			if ( m_Ramdisk ) {
				delete m_Ramdisk;
				m_Ramdisk = NULL;
			}
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		status = STATUS_SUCCESS;
		break;
	}

	// Remove the drive letter associated with the ram disk.
	// This prevents new references to it from being established,
	// in preperation for deletion.
	case IOCTL_REMOVE:

		if ( m_Ramdisk->IsOpen() ) {
			status = STATUS_DEVICE_BUSY;
			break;
		}    

	// Delete the symbolic link created by the constructor
		m_Ramdisk->DestroyLink(NULL);
		status = STATUS_SUCCESS;
		break;

	// Delete the ramdisk device.  This can only be done once the
	// associated drive letter has been removed.
	case IOCTL_DELETE:

		if ( m_Ramdisk == NULL	||
		     m_Ramdisk->SymbolicLinkName()->Length )
		{
			status = STATUS_UNSUCCESSFUL;
			break;
		}    
		delete m_Ramdisk;
		m_Ramdisk = NULL;

		status = STATUS_SUCCESS;
		break;

	default:
		status = STATUS_INVALID_PARAMETER;
	}

	return I.Complete(status);
}

///////////////////////////////////////////////////////////////////////////
// Create
// 
// The Create dispatch function simply ensures that only one GUI at a
// time is attempting to control the ramdisk driver.
//
NTSTATUS RamdiskControlDevice::Create(KIrp I)
{
	m_Lock.Lock();

	if ( m_Busy ) 
	{
		m_Lock.Unlock();
		I.Information() = 0;
		return I.Complete(STATUS_DEVICE_BUSY);
	}
	else
	{
		m_Busy = TRUE;
		m_Lock.Unlock();
	}
	return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////
// Close
// 
NTSTATUS RamdiskControlDevice::Close(KIrp I)
{
	m_Busy = FALSE;
	return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////
// ReadWrite
// 
NTSTATUS RamdiskControlDevice::ReadWrite(KIrp I)
{
	return I.Complete(STATUS_SUCCESS);
}

