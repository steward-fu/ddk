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

// smioctl.h - common definitions for SHAREMEM example
// Copyright (c) 1998 Compuware Corporation


// Define IOCTL codes

#define SHAREMEM_IOCTL_MAP_BUFFER \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_NEITHER, FILE_ANY_ACCESS)
 
#define SHAREMEM_IOCTL_UNMAP_BUFFER \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_NEITHER, FILE_ANY_ACCESS)


typedef struct _map_buffer_args
{
	PVOID 	m_address;
	DWORD	m_size;
} MAP_BUFFER_ARGS;

 