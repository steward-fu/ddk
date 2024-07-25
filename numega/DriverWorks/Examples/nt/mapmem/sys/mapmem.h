// mapmem.h -  example driver, device class
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

class MapMemDriver : public KDriver
{
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
};

#define DEFAULT_DEVMEMORY_ADDRESS	0xF0000
#define DEFAULT_DEVMEMORY_SIZE		0x400
