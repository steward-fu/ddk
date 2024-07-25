// Common definitions for exe/sys ramdisk sample
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

#define IOCTL_CREATE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_REMOVE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DELETE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct CREATE_PARMS {
    ULONG	nRootDirEntries;
    ULONG	nSectorsPerCluster;
    ULONG	DiskSize;
    WCHAR	DriveLetter;
};
