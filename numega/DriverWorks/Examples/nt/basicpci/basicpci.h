// basicpci.h - include file for basic PCI driver example
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

class BasicPci : public KDriver
{
	SAFE_DESTRUCTORS
public:
	virtual VOID Unload(VOID);
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
};

// Defintions for PCI device identification. Reset for your device.

#define VENDOR_ID 0x10b7
#define DEVICE_ID 0x5900
