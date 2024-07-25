// rddevice.cpp -  example driver, device class
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

// This is the device class for the actual RAMDISK device seen
// by the system. The control device instances this class at
// the direction of the application.

#include <vdw.h>
#include "ramdisk.h"
#include "rddevice.h"

///////////////////////////////////////////////////////////////////////////
// RamdiskDevice
// 
// Initialize the ram disk
//
RamdiskDevice::RamdiskDevice(	
	PCWSTR DriveLetter, 
	ULONG nRootDirs,
	ULONG nSectorsPerCluster, 
	ULONG DiskSize 
	) : 

	KDevice(
		L"RAMDISK0", 
		FILE_DEVICE_VIRTUAL_DISK,
		DriveLetter,
		FILE_REMOVABLE_MEDIA,
		DO_DIRECT_IO
		),

	m_Image(NULL),
	m_OpenCount(0)
{
// Make sure the base object was constructed successfully

	if ( !NT_SUCCESS(m_ConstructorStatus) )
		return;

	m_pDeviceObject->AlignmentRequirement = FILE_WORD_ALIGNMENT;

// Round up number of root directories so it fills an even 
// number of sectors.
	m_RootDirEntries = (nRootDirs + DIR_ENTRIES_PER_SECTOR - 1) 
		     / DIR_ENTRIES_PER_SECTOR * DIR_ENTRIES_PER_SECTOR;
	m_SectorsPerCluster = nSectorsPerCluster;
	m_Size = DiskSize;


// Allocate the memory for the RAMDISK
	m_Image = new(NonPagedPool) UCHAR[m_Size];

	if ( m_Image == NULL )
	{
		m_ConstructorStatus = STATUS_UNSUCCESSFUL;
		return;
	}

// Place an empty FAT file system on drive
	FormatBootSector();

// This is needed because the device is not created during DriverEntry
	m_pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
}

///////////////////////////////////////////////////////////////////////////
// ~RamdiskDevice
// 
// Deallocate memory image for disk, if present
//
RamdiskDevice::~RamdiskDevice(void)
{
	if ( m_Image != NULL )
		delete m_Image;
}

///////////////////////////////////////////////////////////////////////////
// IsOpen
// 
// This function is called by the control device to ensure that there
// are no open handles to the drive before removing it.  The GUI
// pretty much guarantees this is the case by locking the drive
// before asking it to be deleted.
//
BOOLEAN RamdiskDevice::IsOpen(void)
{
	return m_OpenCount > 0;
}

///////////////////////////////////////////////////////////////////////////
// Create
// 
// Dispatch function called when device is opened.
//
NTSTATUS RamdiskDevice::Create(KIrp  I)
{
	++m_OpenCount;
	return DefaultDispatch(I);
}


///////////////////////////////////////////////////////////////////////////
// Close
// 
// Dispatch function called when device is closed.
//
NTSTATUS RamdiskDevice::Close(KIrp  I)
{
	--m_OpenCount;
	return DefaultDispatch(I);
}


///////////////////////////////////////////////////////////////////////////
// ReadWrite
// 
// Dispatch function called when device is read/written.  Validate
// parameters and perform operation.  The higher level FAT driver
// handles all file-offset to sector translation; all we do is read/write
// sectors.
//
NTSTATUS RamdiskDevice::ReadWrite(KIrp I)
{
	PUCHAR pDriveData;

	I.Information() = 0;

	// Ensure size of read/write is a multiple of sector size.
	if ( I.ReadOffset().QuadPart+I.ReadSize() > m_Size ||
		 I.ReadSize() & (BytesPerSector - 1) )
	{
		return I.Complete(STATUS_INVALID_PARAMETER);
	}

	// The address of the user buffer is in the field MDL of the IRP,
	// unless the routine is called by DeviceControl to do a verify,
	// in which case there is no buffer. We need a system space virtual
	// address for this buffer, so we call KMemory::MapToSystemSpace.

	if (I.Mdl())
		pDriveData = (PUCHAR) KMemory(I.Mdl()).MapToSystemSpace();

	// Do the operation
	switch (I.MajorFunction())
	{
	case IRP_MJ_DEVICE_CONTROL:	// called for verify operation - see below
	    break;

	case IRP_MJ_READ:
	    I.Information() = I.ReadSize();
	    RtlMoveMemory(pDriveData, m_Image+I.ReadOffset().LowPart, I.ReadSize());
	    break;

	case IRP_MJ_WRITE:
	    I.Information() = I.WriteSize();
	    RtlMoveMemory(m_Image+I.ReadOffset().LowPart, pDriveData, I.WriteSize());
	    break;

	default:
	    I.Information() = 0;
	    break;
	}

	return I.Complete(STATUS_SUCCESS);
}


///////////////////////////////////////////////////////////////////////////
// DeviceControl
// 
// Dispatch function called for device control requests.
//
NTSTATUS RamdiskDevice::DeviceControl(KIrp I)
{
	NTSTATUS status;

	I.Information() = 0;

	switch ( I.IoctlCode() )
	{
	case IOCTL_DISK_GET_MEDIA_TYPES:
	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
	//
	// Return the drive geometry for the ram disk.  Note that
	// we return values which were made up to suit the disk size.
	//

	    if ( I.IoctlOutputBufferSize() < sizeof( DISK_GEOMETRY ) )
			status = STATUS_INVALID_PARAMETER;
	    else
	    {
			PDISK_GEOMETRY Geometry = ( PDISK_GEOMETRY )I.IoctlBuffer();

			Geometry->MediaType = RemovableMedia;
			Geometry->Cylinders.QuadPart = m_Size/(512*32*2);
			Geometry->TracksPerCylinder = TracksPerCylinder;
			Geometry->SectorsPerTrack = SectorsPerTrack;
			Geometry->BytesPerSector = BytesPerSector;

			status = STATUS_SUCCESS;
			I.Information() = sizeof( DISK_GEOMETRY );
	    }
		break;

	case IOCTL_DISK_GET_PARTITION_INFO: 
	//
	// Return the information about the partition.
	//

		if ( I.IoctlOutputBufferSize() < sizeof( PARTITION_INFORMATION ) )
			status = STATUS_INVALID_PARAMETER;
		else
		{
			PPARTITION_INFORMATION ParInfo = 
			    ( PPARTITION_INFORMATION )I.IoctlBuffer();

			PBOOT_SECTOR BootSector = (PBOOT_SECTOR)m_Image;

			ParInfo->PartitionType = 
			    (BootSector->bsFileSystemType[4] == '6')
			    ? PARTITION_FAT_16 : PARTITION_FAT_12;
			
			ParInfo->BootIndicator = FALSE;
			ParInfo->RecognizedPartition = TRUE;
			ParInfo->RewritePartition = FALSE;
			ParInfo->StartingOffset = RtlConvertUlongToLargeInteger(0);
			ParInfo->PartitionLength = RtlConvertUlongToLargeInteger(m_Size);
			ParInfo->HiddenSectors =  1L;
			
			status = STATUS_SUCCESS;
			I.Information() = sizeof( PARTITION_INFORMATION );
		}

		break;

	case IOCTL_DISK_VERIFY:
	{
	//
	// Move offset and length information to locations in Irp
	// customary for READ requests.
	//
		PVERIFY_INFORMATION	verifyInformation = 
				(PVERIFY_INFORMATION) I.IoctlBuffer();

		I.ReadOffset().LowPart = verifyInformation->StartingOffset.LowPart;
		I.ReadOffset().HighPart =  verifyInformation->StartingOffset.HighPart;
		I.ReadSize() = verifyInformation->Length;

			//
			// A VERIFY is identical to a READ, except for the fact that no
			// data gets transferred.  So follow the READ code path.
			//
		return ReadWrite(I);
	}

	case IOCTL_DISK_CHECK_VERIFY:
	    //
	    // This is called for removable media to determine whether
	    // the media has been ejected and/or replaced since the last 
	    // access to it.  Our disk isn't ejectable, so we always
	    // return SUCCESS.  Otherwise, we could return
	    // STATUS_VERIFY_REQUIRED.
			//
			
	    status = STATUS_SUCCESS;
	    I.Information() = 0;
	    break;


	case IOCTL_DISK_IS_WRITABLE:
	    //
	    // This is called for removable media to determine whether
	    // the media is write-protected.  Ours isn't.  If it were,
	    // we could return STATUS_MEDIA_WRITE_PROTECTED.
			//
			
	    status = STATUS_SUCCESS;
	    I.Information() = 0;
	    break;


	default:
	    status = STATUS_INVALID_DEVICE_REQUEST;
	    break;
	}

	//
	// Finish the I/O operation by simply completing the packet and returning
	// the same status as in the packet itself.
	//

	return I.Complete(status);
}


///////////////////////////////////////////////////////////////////////////
// FormatBootSector
// 
// Places an image of an empty FAT drive in the memory buffer 
// containing the ramdisk.  This is complicated somewhat by the
// fact that the FAT can have either 12 or 16 bit entries, based
// on the size of the drive.
//
VOID RamdiskDevice::FormatBootSector()
{
	PBOOT_SECTOR BootSector = (PBOOT_SECTOR) m_Image;
	ULONG FATEntries;
	ULONG FATType, FATSectorCount;
	PDIR_ENTRY RootDir;        // Pointer to first entry in root dir    
	PUCHAR FirstFatSector;

	BootSector->bsJump[0] = 0xeb;
	BootSector->bsJump[1] = 0x3c;
	BootSector->bsJump[2] = 0x90;

	strncpy(BootSector->bsOemName, "Compuware Corp", 8);
	BootSector->bsBytesPerSec = (SHORT)BytesPerSector;
	BootSector->bsResSectors = 1;
	BootSector->bsFATs = 1;
	BootSector->bsRootDirEnts = (USHORT)m_RootDirEntries;

	//Calculate the number of sectors on the disk.  If there are few enough
	// sectors to fit in a USHORT, it goes in bsSectors.  Otherwise bsSectors
	// must be zero, and the number of sectors goes in bsHugeSectors. The cutoff
	// as far as disk size is around 32 Meg.
	ULONG nSectors = m_Size / BytesPerSector;
	if(nSectors > 0xFFFF) 
	{
		//Too many sectors for bsSectors.  Use bsHugeSectors.
		BootSector->bsSectors = 0;
		BootSector->bsHugeSectors = nSectors;
	}
	else
	{
		//Small number of sectors.  Use bsSectors.
		BootSector->bsSectors = (USHORT)nSectors;
		BootSector->bsHugeSectors = 0;
	}
	
	BootSector->bsMedia = RAMDISK_MEDIA_TYPE;

	BootSector->bsSecPerClus = (UCHAR)m_SectorsPerCluster;

	//
	// Calculate number of sectors required for FAT
	//
	FATEntries =
		(nSectors - BootSector->bsResSectors -
			BootSector->bsRootDirEnts / DIR_ENTRIES_PER_SECTOR) /
				BootSector->bsSecPerClus + 2;

	//
	// Choose between 12 and 16 bit FAT based on number of clusters we
	// need to map
	//
	if (FATEntries > 4087)
	{
		FATType =  16;
		FATSectorCount = (FATEntries * 2 + 511) / 512;
		FATEntries -= FATSectorCount;
		FATSectorCount = (FATEntries * 2 + 511) / 512;
	}
	else
	{
		FATType =  12;
		FATSectorCount = (((FATEntries * 3 + 1) / 2) + 511) / 512;
		FATEntries -= FATSectorCount;
		FATSectorCount = (((FATEntries * 3 + 1) / 2) + 511) / 512;
	}

	BootSector->bsFATsecs = (USHORT)FATSectorCount;
	BootSector->bsSecPerTrack = SectorsPerTrack;
	BootSector->bsHeads = TracksPerCylinder;
	BootSector->bsBootSignature = 0x29;
	BootSector->bsVolumeID = 0x12345678;
	strncpy(BootSector->bsLabel, "RamDisk    ", 11);
	strncpy(BootSector->bsFileSystemType, "FAT1?   ", 8);
	BootSector->bsFileSystemType[4] = ( FATType == 16 ) ? '6' : '2';
	
	BootSector->bsSig2[0] = 0x55;
	BootSector->bsSig2[1] = 0xAA;
	
	//
	// The FAT is located immediately following the boot sector.
	//
	FirstFatSector = (PUCHAR)(BootSector + 1);
	FirstFatSector[0] = RAMDISK_MEDIA_TYPE;
	FirstFatSector[1] = 0xFF;
	FirstFatSector[2] = 0xFF;

	if (FATType == 16)
		FirstFatSector[3] = 0xFF;

	//
	// The Root Directory follows the FAT
	//
	RootDir = (PDIR_ENTRY)(BootSector + 1 + FATSectorCount);
	strcpy((PCHAR)RootDir->deName, "MS-RAMDR");
	strcpy((PCHAR)RootDir->deExtension, "IVE");
	RootDir->deAttributes = DIR_ATTR_VOLUME;
}
