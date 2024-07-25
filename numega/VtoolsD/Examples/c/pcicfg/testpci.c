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

// TESTPCI.C - Win32 console application for use with PCICFG.VXD
// Copyright (c) 1996, Compuware Corporation

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

#define NotVxD
#include "pcicfg.h"

HANDLE	hDevice;

//
// -------------------------------------------------------------------------
//
// Main
//
void main(int ac, char* av[])
{
	int i;
	const PCHAR DynVxDName = "\\\\.\\PCICFG.VXD";
	const PCHAR StaticVxDName = "\\\\.\\PCICFG";

	PVOID ABigBuffer;
	DWORD nRet;
	BOOL status;


	struct 
	{
		WORD VendorId;
		WORD DeviceId;
	} input;

	PCI_CONFIG_HEADER_0 ConfigData;

	if ( ac == 3 )
	{
		sscanf(av[1], "%x", &input.VendorId);
		sscanf(av[2], "%x", &input.DeviceId);
	}

	
	if ( (ac != 3) || (input.VendorId==0) || (input.DeviceId==0) )
	{
		fprintf(stderr, "usage: testpci <hex-vendor-id>  <hex-device-id>\n");
		exit(1);
	}


// Load the VxD

	hDevice = CreateFile(StaticVxDName, 0,0,0,
                        CREATE_NEW, 
                        FILE_FLAG_DELETE_ON_CLOSE,
                        0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		hDevice = CreateFile(DynVxDName, 0,0,0,
                        CREATE_NEW, 
                        FILE_FLAG_DELETE_ON_CLOSE,
                        0);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
        	fprintf(stderr, "Cannot load driver, error=%08lx\n",
				GetLastError());
	 		exit(1);
		}
	}

	status =  DeviceIoControl(
				hDevice, 
				IOCTL_PCICFG_GETCFG, 
				&input, 
				sizeof(input), 
				&ConfigData, 
				sizeof(ConfigData), 
				&nRet, 
				NULL);

	if ( (!status) || (nRet == 0) )
	{
		printf("no information found for Vendor %x, Device %x\n", 
			input.VendorId, input.DeviceId);
		exit(1);
	}

	printf("\nConfig data for vendor %04x device %04x\n", input.VendorId, input.DeviceId);
    printf("  VendorID\t=\t%04x\tDeviceID=\t%04x\n", ConfigData.VendorID, ConfigData.DeviceID);
    printf("  Command\t=\t%04x\tStatus\t=\t%04x\n", ConfigData.Command, ConfigData.Status);
    printf("  RevisionID\t=\t%02x\tProgIf\t=\t%02x\n", ConfigData.RevisionID,ConfigData.ProgIf);
    printf("  SubClass\t=\t%02x\tBaseClass =\t%02x\n", ConfigData.SubClass, ConfigData.BaseClass);; 
    printf("  CacheLineSize\t=\t%02x\n", ConfigData.CacheLineSize);
    printf("  LatencyTimer\t=\t%02x\n", ConfigData.LatencyTimer);;
    printf("  HeaderType\t=\t%02x\n", ConfigData.HeaderType);;  
    printf("  BIST\t\t=\t%02x\n", ConfigData.BIST);;         
    printf("  BaseAddresses[0],[1] =\t%08x %08x\n", ConfigData.BaseAddresses[0], ConfigData.BaseAddresses[1]);
    printf("  BaseAddresses[2],[3] =\t%08x %08x\n", ConfigData.BaseAddresses[2], ConfigData.BaseAddresses[3]);
    printf("  BaseAddresses[4],[5] =\t%08x %08x\n", ConfigData.BaseAddresses[4], ConfigData.BaseAddresses[5]);
	printf("  CardBusCISPtr\t=\t%08x\n", ConfigData.CardBusCISPtr);
    printf("  SubsystemVendorID =\t%04x\n", ConfigData.SubsystemVendorID);
	printf("  SubsystemID\t=\t%04x\n", ConfigData.SubsystemID);
	printf("  ROMBaseAddress =\t%08x\n", ConfigData.ROMBaseAddress);
    printf("  InterruptLine\t=\t%02x\n", ConfigData.InterruptLine);
    printf("  InterruptPin\t=\t%02x\n", ConfigData.InterruptPin);
    printf("  MinimumGrant\t=\t%04x\n", ConfigData.MinimumGrant); 
    printf("  MaximumLatency =\t%04x\n", ConfigData.MaximumLatency);

	exit(0);
}


