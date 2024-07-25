// common.h - sys/exe common defintions for KPeripheralAddress demonstration
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

#define IOCTL_BUSADDR_GETDATE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BUSADDR_MAP_VGATEXT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BUSADDR_UNMAP_VGATEXT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct 
{
	UCHAR day;
	UCHAR month;
	UCHAR year;
} DATEINFO;

typedef struct
{
	HANDLE hProcess;
	PVOID pointer;
} UNMAPINFO;
