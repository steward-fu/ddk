// enumpci.cpp - application for PCI enumerator driver
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

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

struct PCI_CONFIG_HEADER_0
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
};

VOID Dump(PCI_CONFIG_HEADER_0* cfg, USHORT vendor, USHORT device)
{
	printf("\nConfig data for vendor %04x device %04x\n", vendor, device);
    printf("  VendorID\t=\t%04x\tDeviceID=\t%04x\n", cfg->VendorID, cfg->DeviceID);
    printf("  Command\t=\t%04x\tStatus\t=\t%04x\n", cfg->Command, cfg->Status);
    printf("  RevisionID\t=\t%02x\tProgIf\t=\t%02x\n", cfg->RevisionID,cfg->ProgIf);
    printf("  SubClass\t=\t%02x\tBaseClass =\t%02x\n", cfg->SubClass, cfg->BaseClass);; 
    printf("  CacheLineSize\t=\t%02x\n", cfg->CacheLineSize);
    printf("  LatencyTimer\t=\t%02x\n", cfg->LatencyTimer);;
    printf("  HeaderType\t=\t%02x\n", cfg->HeaderType);;  
    printf("  BIST\t\t=\t%02x\n", cfg->BIST);;         
    printf("  BaseAddresses[0],[1] =\t%08x %08x\n", cfg->BaseAddresses[0], cfg->BaseAddresses[1]);
    printf("  BaseAddresses[2],[3] =\t%08x %08x\n", cfg->BaseAddresses[2], cfg->BaseAddresses[3]);
    printf("  BaseAddresses[4],[5] =\t%08x %08x\n", cfg->BaseAddresses[4], cfg->BaseAddresses[5]);
	printf("  CardBusCISPtr\t=\t%08x\n", cfg->CardBusCISPtr);
    printf("  SubsystemVendorID =\t%04x\n", cfg->SubsystemVendorID);
	printf("  SubsystemID\t=\t%04x\n", cfg->SubsystemID);
	printf("  ROMBaseAddress =\t%08x\n", cfg->ROMBaseAddress);
    printf("  InterruptLine\t=\t%02x\n", cfg->InterruptLine);
    printf("  InterruptPin\t=\t%02x\n", cfg->InterruptPin);
    printf("  MinimumGrant\t=\t%04x\n", cfg->MinimumGrant); 
    printf("  MaximumLatency =\t%04x\n", cfg->MaximumLatency);
}

// -------------------------------------------------------------------------
//
// Main
//
void __cdecl main(int ac, char* av[])
{
	HANDLE h;
	ULONG nRead;
	PCI_CONFIG_HEADER_0 cfghead;
	BOOL status;

	h = CreateFile("\\\\.\\PCIENUM",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		printf("failed to open PCIENUM device: %x\n", 
				GetLastError());
		exit(1);
	}

	while (TRUE)
	{
		status = ReadFile(h, &cfghead, sizeof(cfghead), &nRead, NULL);		

		if ( ! status )
		{
			fprintf(stderr, "Error reading device: %x\n", GetLastError());
			exit(1);
		}

		if (nRead == 0)
			break;

		Dump(&cfghead, cfghead.VendorID, cfghead.DeviceID);
		printf("Press any key to continue...\n"); _getch();
	}
}

