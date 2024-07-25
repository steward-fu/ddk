// rddevice.h -  include file for example driver, device class
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

class RamdiskDevice : public KDevice
{
public:
	RamdiskDevice(	PCWSTR DriveLetter, 
			ULONG nRootDirs,
			ULONG nSectorsPerCluster, 
			ULONG DiskSize);
	virtual ~RamdiskDevice(void);

	BOOLEAN IsOpen(void);

	DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS ReadWrite(KIrp I);			  	// COMMENT_ONLY
#endif

protected:
	VOID FormatBootSector(void);

	PUCHAR	m_Image;
	ULONG	m_RootDirEntries;
	ULONG	m_SectorsPerCluster;
	ULONG	m_Size;
	ULONG	m_DiskImage;
	ULONG	m_OpenCount;
};


const USHORT BytesPerSector = 512;
const USHORT SectorsPerTrack = 32;
const USHORT TracksPerCylinder = 2;
