// ramdisk.h -  inlude file for example driver
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

class RamDiskDriver : public KDriver
{
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
};


#include <ntdddisk.h>       // disk device driver I/O control codes
#include <ntiologc.h>

#define RAMDISK_MEDIA_TYPE      0xF8
#define DIR_ENTRIES_PER_SECTOR  16
#define WIN32_PATH              L"\\DosDevices\\"

#pragma pack(1)

typedef struct  _BOOT_SECTOR
{
    UCHAR       bsJump[3];          // x86 jmp instruction, checked by FS
    CCHAR       bsOemName[8];       // OEM name of formatter
    USHORT      bsBytesPerSec;      // Bytes per Sector
    UCHAR       bsSecPerClus;       // Sectors per Cluster
    USHORT      bsResSectors;       // Reserved Sectors
    UCHAR       bsFATs;             // Number of FATs - we always use 1
    USHORT      bsRootDirEnts;      // Number of Root Dir Entries
    USHORT      bsSectors;          // Number of Sectors
    UCHAR       bsMedia;            // Media type - we use RAMDISK_MEDIA_TYPE
    USHORT      bsFATsecs;          // Number of FAT sectors
    USHORT      bsSecPerTrack;      // Sectors per Track - we use 32
    USHORT      bsHeads;            // Number of Heads - we use 2
    ULONG       bsHiddenSecs;       // Hidden Sectors - we set to 0
    ULONG       bsHugeSectors;      // Number of Sectors if > 32 MB size
    UCHAR       bsDriveNumber;      // Drive Number - not used
    UCHAR       bsReserved1;        // Reserved
    UCHAR       bsBootSignature;    // New Format Boot Signature - 0x29
    ULONG       bsVolumeID;         // VolumeID - set to 0x12345678
    CCHAR       bsLabel[11];        // Label - set to RamDisk
    CCHAR       bsFileSystemType[8];// File System Type - FAT12 or FAT16
    CCHAR       bsReserved2[448];   // Reserved
    UCHAR       bsSig2[2];          // Originial Boot Signature - 0x55, 0xAA
}   BOOT_SECTOR, *PBOOT_SECTOR;

typedef struct  _DIR_ENTRY
{
    UCHAR       deName[8];          // File Name
    UCHAR       deExtension[3];     // File Extension
    UCHAR       deAttributes;       // File Attributes
    UCHAR       deReserved;         // Reserved
    USHORT      deTime;             // File Time
    USHORT      deDate;             // File Date
    USHORT      deStartCluster;     // First Cluster of file
    ULONG       deFileSize;         // File Length
}   DIR_ENTRY, *PDIR_ENTRY;

#pragma pack()

//
// Directory Entry Attributes
//

#define DIR_ATTR_READONLY   0x01
#define DIR_ATTR_HIDDEN     0x02
#define DIR_ATTR_SYSTEM     0x04
#define DIR_ATTR_VOLUME     0x08
#define DIR_ATTR_DIRECTORY  0x10
#define DIR_ATTR_ARCHIVE    0x20
