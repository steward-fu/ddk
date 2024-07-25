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

// PCICFG.h - include file for VxD PCICFG

#ifndef NotVxD

#include <vtoolsc.h>

#define PCICFG_Major		1
#define PCICFG_Minor		0
#define PCICFG_DeviceID		UNDEFINED_DEVICE_ID
#define PCICFG_Init_Order	UNDEFINED_INIT_ORDER


#define PCI_ENUM_FUNC_GET_DEVICE_INFO   0
#define PCI_ENUM_FUNC_SET_DEVICE_INFO   1

DEVNODE LocatePCIDevice(WORD VendorId, WORD DeviceId);

CONFIGRET SetPCIConfigData(
	DEVNODE Devnode, 
	DWORD 	StartByteOffset, 
	DWORD* 	ConfigDataBuffer, 
	ULONG 	nBytes);

CONFIGRET GetPCIConfigData(
	DEVNODE Devnode, 
	DWORD 	StartByteOffset, 
	DWORD* 	ConfigDataBuffer, 
	ULONG 	nBytes);

#endif

// shared with application


#define IOCTL_PCICFG_GETCFG \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _pci_config_header_0_t
{
    USHORT  VendorID;
    USHORT  DeviceID;
    USHORT  Command; 
    USHORT  Status;
    UCHAR   RevisionID;
    UCHAR   ProgIf;    
    UCHAR   SubClass;  
    UCHAR   BaseClass; 
    UCHAR   CacheLineSize;
    UCHAR   LatencyTimer; 
    UCHAR   HeaderType;   
    UCHAR   BIST;          
    ULONG   BaseAddresses[6];
	ULONG	CardBusCISPtr;
    USHORT	SubsystemVendorID;
	USHORT	SubsystemID;
	ULONG	ROMBaseAddress;
    ULONG   Reserved2[2];
    UCHAR   InterruptLine; 
    UCHAR   InterruptPin;  
    UCHAR   MinimumGrant;  
    UCHAR   MaximumLatency;
} PCI_CONFIG_HEADER_0;

