// E100bexPci.h: interface for the E100bexPciInformation class.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//
// Portions copied from Microsoft Windows 2000 DDK sample driver containing the
// following copyright
//
/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/


#if !defined(AFX_E100BEXPCI_H__0E5098AA_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
#define AFX_E100BEXPCI_H__0E5098AA_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////
// Symbolic constants used in this driver for PCI configuration space		
//


//-------------------------------------------------------------------------
// PCI configuration hardware ports
//-------------------------------------------------------------------------
const USHORT	CF1_CONFIG_ADDR_REGISTER  =  0x0CF8;
const USHORT	CF1_CONFIG_DATA_REGISTER  =  0x0CFC;
const USHORT	CF2_SPACE_ENABLE_REGISTER =  0x0CF8;
const USHORT	CF2_FORWARD_REGISTER      =  0x0CFA;
const USHORT	CF2_BASE_ADDRESS          =  0xC000;

//-------------------------------------------------------------------------
// PCI Class Code Definitions
// Configuration Space Header
//-------------------------------------------------------------------------
const UCHAR		PCI_BASE_CLASS		=   0x02; // Base Class - Network Controller
const UCHAR		PCI_SUB_CLASS       =	0x00; // Sub Class - Ethernet Controller
const UCHAR		PCI_PROG_INTERFACE  =	0x00; // Prog I/F - Ethernet COntroller

//-------------------------------------------------------------------------
// PCI Command Register Bit Definitions
// Configuration Space Header
//-------------------------------------------------------------------------
const USHORT	CMD_IO_SPACE            =	BIT_0;
const USHORT	CMD_MEMORY_SPACE        =	BIT_1;
const USHORT	CMD_BUS_MASTER          =	BIT_2;
const USHORT	CMD_SPECIAL_CYCLES      =	BIT_3;
const USHORT	CMD_MEM_WRT_INVALIDATE  =	BIT_4;
const USHORT	CMD_VGA_PALLETTE_SNOOP  =	BIT_5;
const USHORT	CMD_PARITY_RESPONSE     =	BIT_6;
const USHORT	CMD_WAIT_CYCLE_CONTROL  =	BIT_7;
const USHORT	CMD_SERR_ENABLE         =	BIT_8;
const USHORT	CMD_BACK_TO_BACK        =	BIT_9;

//-------------------------------------------------------------------------
// PCI Status Register Bit Definitions
// Configuration Space Header
//-------------------------------------------------------------------------
const USHORT	STAT_BACK_TO_BACK           =	BIT_7;
const USHORT	STAT_DATA_PARITY            =	BIT_8;
const USHORT	STAT_DEVSEL_TIMING          =	BIT_9 | BIT_10;
const USHORT	STAT_SIGNAL_TARGET_ABORT    =	BIT_11;
const USHORT	STAT_RCV_TARGET_ABORT       =	BIT_12;
const USHORT	STAT_RCV_MASTER_ABORT       =	BIT_13;
const USHORT	STAT_SIGNAL_MASTER_ABORT    =	BIT_14;
const USHORT	STAT_DETECT_PARITY_ERROR    =	BIT_15;

//-------------------------------------------------------------------------
// PCI Base Address Register For Memory (BARM) Bit Definitions
// Configuration Space Header
//-------------------------------------------------------------------------
const USHORT	BARM_LOCATE_BELOW_1_MEG     =	BIT_1;
const USHORT	BARM_LOCATE_IN_64_SPACE     =	BIT_2;
const USHORT	BARM_PREFETCHABLE           =	BIT_3;

//-------------------------------------------------------------------------
// PCI Base Address Register For I/O (BARIO) Bit Definitions
// Configuration Space Header
//-------------------------------------------------------------------------
const USHORT	BARIO_SPACE_INDICATOR       =	BIT_0;

//-------------------------------------------------------------------------
// PCI BIOS Definitions
// Refer To The PCI BIOS Specification
//-------------------------------------------------------------------------
//- Function Code List
const UCHAR		PCI_FUNCTION_ID         =	0xB1;    // AH Register
const UCHAR		PCI_BIOS_PRESENT        =	0x01;    // AL Register
const UCHAR		FIND_PCI_DEVICE         =	0x02;    // AL Register
const UCHAR		FIND_PCI_CLASS_CODE     =	0x03;    // AL Register
const UCHAR		GENERATE_SPECIAL_CYCLE  =	0x06;    // AL Register
const UCHAR		READ_CONFIG_BYTE        =	0x08;    // AL Register
const UCHAR		READ_CONFIG_WORD        =	0x09;    // AL Register
const UCHAR		READ_CONFIG_DWORD       =	0x0A;    // AL Register
const UCHAR		WRITE_CONFIG_BYTE       =	0x0B;    // AL Register
const UCHAR		WRITE_CONFIG_WORD       =	0x0C;    // AL Register
const UCHAR		WRITE_CONFIG_DWORD      =	0x0D;    // AL Register

//- Function Return Code List
const UCHAR		SUCCESSFUL              =	0x00;
const UCHAR		FUNC_NOT_SUPPORTED      =	0x81;
const UCHAR		BAD_VENDOR_ID           =	0x83;
const UCHAR		DEVICE_NOT_FOUND        =	0x86;
const UCHAR		BAD_REGISTER_NUMBER     =	0x87;

//- PCI BIOS Calls
const UCHAR		PCI_BIOS_INTERRUPT      =	0x1A;        // PCI BIOS Int 1Ah Function Call
const ULONG		PCI_PRESENT_CODE        =	0x20494350;  // Hex Equivalent Of 'PCI '

const ULONG		PCI_SERVICE_IDENTIFIER  =	0x49435024;  // ASCII Codes for 'ICP$'

//- Device and Vendor IDs
const USHORT	D100_DEVICE_ID          =	0x1229;
const USHORT	D100_VENDOR_ID          =	0x8086;



///////////////////////////////////////////////////////////////////////////
// E100bexPciInformation		
//
// Class to do some device specific operations on the E100bex adapter's
// PCI configuration space.
//
//
class E100bexPciInformation : public KNdisPciInformation  
{
public:
// constructors

	E100bexPciInformation(NDIS_HANDLE hAdapter) : 
		KNdisPciInformation(hAdapter) {}

	E100bexPciInformation(KNdisMiniAdapter* pAdapter) :
		KNdisPciInformation(NDIS_HANDLE(*pAdapter)) {}

// functions

	UCHAR ReadRevisionId(void);

	VOID ReadSubsystemId(OUT USHORT& SubVendor, OUT USHORT& SubDevice);

	BOOLEAN GetMemoryWriteInvalidateBit(void);

	BOOLEAN TestAndSetDmaMasterBit(void);
};


#endif // !defined(AFX_E100BEXPCI_H__0E5098AA_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
