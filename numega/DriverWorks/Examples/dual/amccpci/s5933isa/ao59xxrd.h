// ao59XXrd.h - include file for samples based on DK1 development kits
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in 
// Compuware Corporation license agreement and as provided in 
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995), 
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995), 
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.  
// Compuware Corporation.
// 
// This product contains confidential information and trade secrets 
// of Compuware Corporation. Use, disclosure, or reproduction is 
// prohibited without the prior express written permission of Compuware 
// Corporation.
//
//=============================================================================

#ifndef __AO59XXRD__
#define __AO59XXRD__

// Register Definitions for Addon side of AMCC S59XX

///////////////////////////////////////////////////////////////////////////////
// AMCC S5933 Addon Operations Registers
// Based on the AMCC S5933 register definitions.  The structure members occupy
// the same offset as their counterparts in the AMCC S5933 mapped I/O region.
//
// The AMCC S5920 definitions are a subset as noted.  They have the same offset, but
// only portions of the registers are valid for the S5920.
//
// Additionally, the S5920 has a register at offset 0x60 for configuration of
// pass-thru operation.
///////////////////////////////////////////////////////////////////////////////
// pack with byte alignment
#pragma pack(push,1)
typedef struct _AMCCS5933_ADDON_OPERATION_REGISTERS
{
	ULONG AddonIncomingMailbox1;	// S5933 AIMB1 - Addon Incoming Mailbox Reg 1
	ULONG AddonIncomingMailbox2;	// S5933 AIMB2 - Addon Incoming Mailbox Reg 2
	ULONG AddonIncomingMailbox3;	// S5933 AIMB3 - Addon Incoming Mailbox Reg 3
	ULONG AddonIncomingMailbox4;	// S5933 AIMB4 - Addon Incoming Mailbox Reg 4
									// S5920 AIMB  - Addon Incoming Mailbox Reg
	ULONG AddonOutgoingMailbox1;	// S5933 AIMB1 - Addon Outgoing Mailbox Reg 1
	ULONG AddonOutgoingMailbox2;	// S5933 AIMB2 - Addon Outgoing Mailbox Reg 2
	ULONG AddonOutgoingMailbox3;	// S5933 AIMB3 - Addon Outgoing Mailbox Reg 3
	ULONG AddonOutgoingMailbox4;	// S5933 AIMB4 - Addon Outgoing Mailbox Reg 4
									// S5920 AIMB  - Addon Outgoing Mailbox Reg
	ULONG AddonFIFORegisterPort;	// S5933 AFIFO - Addon FIFO Reg Port (bidirectional)
	ULONG MasterWriteAddress;		// S5933 MWAR - Master Write Address Reg
	ULONG AddonPassThruAddress;		// S5933 APTA - Addon Pass-Thru Address Reg
	ULONG AddonPassThruData;		// S5933 APTD - Addon Pass-Thru Data Reg
	ULONG MasterReadAddress;		// S5933 MRAR - Master Read Address Reg
	ULONG AddonMailboxEmptyFullStatus;	// S5933 AMBEF - Addon Mailbox Empty/Full Status Reg
	ULONG AddonInterruptControl;	// S5933 AINT - Interrupt Control Reg
									// S5920 AINT - Interrupt Control Reg
	ULONG AddonGeneralControlStatus;	// S5933 AGCSTS - Addon General Control/Status Reg
									// S5920 ARCR  - Reset Control Reg
} AMCCS5933_ADDON_OPERATION_REGISTERS, *PAMCCS5933_ADDON_OPERATION_REGISTERS;

// NOTE!! The following structure does NOT have the same offsets as the S5920 I/O region
typedef struct _AMCCS5920_ADDON_OPERATION_REGISTERS
{
	ULONG AddonIncomingMailbox;		// S5920 AIMB  - Addon Incoming Mailbox Reg
	ULONG AddonOutgoingMailbox;		// S5920 AOMB  - Addon Outgoin Mailbox Reg
	ULONG AddonPassThruAddress;		// S5920 APTA - Addon Pass-Thru Address Reg
	ULONG AddonPassThruData;		// S5920 APTD - Addon Pass-Thru Data Reg
	ULONG AddonMailboxEmptyFullStatus;	// S5920 AMBEF - Addon Mailbox Empty/Full Status Reg
	ULONG AddonInterruptControl;	// S5920 AINT - Interrupt Control Reg
	ULONG AddonResetControl;		// S5920 ARCR  - Addon Reset Control Reg
	ULONG AddonPassThruConfiguration;	// S5920 APTCR - Pass-Thru Configuration Reg
} AMCCS5920_ADDON_OPERATION_REGISTERS, *PAMCCS5920_ADDON_OPERATION_REGISTERS;
#pragma pack(pop)

// Macro to determine the byte offset of the specified field within the AMCC S5933's
// register range.
#define ADDON_OP_REG_OFFSET(field) (FIELD_OFFSET(AMCCS5933_ADDON_OPERATION_REGISTERS,field))
#define AMCCS5933_ADDON_BUS_MASTER_WRITE_TRANSFER_COUNT_OFFSET 0x58	// MWTC
#define AMCCS5920_ADDON_BUS_MASTER_WRITE_TRANSFER_COUNT_OFFSET 0x5C // MRTC
#define AMCCS5920_ADDON_PASS_THRU_CONFIG_OFFSET 0x60 // APTCR

#define AMCCS5933_ADDON_OPERATION_REGISTERS_SIZE 0x60
#define AMCCS5920_ADDON_OPERATION_REGISTERS_SIZE 0x64

///////////////////////////////////////////////////////////////////////////////
// AMCC59XX General definitions
///////////////////////////////////////////////////////////////////////////////

// RO		- Read Only
// R/W		- Read/Write
// R/WC		- Read/Write_One_Clear
// Uncomment if needed
//typedef enum {
//	KS59XX_PCI_INTERFACE = 0,
//	KS59XX_ADDON_INTERFACE = 1
//} KS59XX_INTERFACE;

// PCI_TO_ADDON is the direction INCOMING wrt ADDON side mailboxes.
// PCI_TO_ADDON transfers are write DMA operations wrt NT driver perspective
// PCI_TO_ADDON transfers are read DMA operations wrt S59XX perspective
// ADDON_TO_PCI is the direction OUTGOING wrt ADDON side mailboxes. 
// ADDON_TO_PCI transfers are read DMA operations wrt NT driver perspective
// ADDON_TO_PCI transfers are write DMA operations wrt S59XX perspective
// Uncomment if needed
//typedef enum {
//	KS59XX_PCI_TO_ADDON = 0,
//	KS59XX_ADDON_TO_PCI = 1
//} KS59XX_DIRECTION;

///////////////////////////////////////////////////////////////////////////////
// Addon Interrupt Control/Status Register bit definitions
// This register shares the same name and bit definitions between the S5933
// and S5920
///////////////////////////////////////////////////////////////////////////////

#define KS59XX_AINT_INTERRUPT_ASSERTED					0x00800000	// RO
#define KS59XX_AINT_BUS_MASTERING_ERROR					0x00200000	// R/WC S5933 only
#define KS59XX_AINT_BIST								0x00100000	// R/WC
#define KS59XX_AINT_READ_XFER_COMPLETE					0x00080000	// R/WC S5933 only
#define KS59XX_AINT_WRITE_XFER_COMPLETE					0x00040000	// R/WC S5933 only
#define KS59XX_AINT_OUTGOING_MAILBOX					0x00020000	// R/WC
#define KS59XX_AINT_INCOMING_MAILBOX					0x00010000	// R/WC
#define KS59XX_AINT_INT_ON_READ_XFER_COMPLETE			0x00008000	// R/W  S5933 only
#define KS59XX_AINT_INT_ON_WRITE_XFER_COMPLETE			0x00004000	// R/W  S5933 only
#define KS59XX_AINT_INT_ON_OUTGOING_MAILBOX				0x00001000	// R/W
#define KS59XX_AINT_INT_ON_INCOMING_MAILBOX				0x00000010	// R/W

// Macro to compose flags for mailbox interrupts based on direction (KS59XX_DIRECTION),
// mailbox id and byte number.  A single interrupt condition can be setup for the set of
// INCOMING (ADDON_TO_PCI) mailboxes and a separate single condition can be setup for
// the OUTGOING (PCI_TO_ADDON) mailboxes.
#define KS59XX_AINT_MAILBOX_INT_CONDITION(direction, id, byte)			\
	((direction) ? (((id - 1) << 2) | byte) << 8 : (((id - 1) << 2) | byte))		

// AINT masks
#define KS59XX_AINT_INIT_MASK							0x00000000
#define KS59XX_AINT_INCOMING_MAILBOX_MASK				0x0000DF00
#define KS59XX_AINT_OUTGOING_MAILBOX_MASK				0x0000C01F
#define KS59XX_AINT_SET_CONTROL_MASK					0x0000FFFF
#define KS59XX_AINT_INTERRUPT_STATUS_MASK				0x003F0000
#define KS59XX_AINT_INTERRUPT_ENABLE_MASK				0x0000D010
#define KS59XX_AINT_FIFO_CONTROL_MASK					0x003FDF1F


///////////////////////////////////////////////////////////////////////////////
// Addon Mailbox Empty Full Status Register
///////////////////////////////////////////////////////////////////////////////

// MBEF masks
#define KS59XX_AMBEF_OUTGOING_MAILBOX_4_MASK			0xF0000000
#define KS59XX_AMBEF_OUTGOING_MAILBOX_3_MASK			0x0F000000
#define KS59XX_AMBEF_OUTGOING_MAILBOX_2_MASK			0x00F00000
#define KS59XX_AMBEF_OUTGOING_MAILBOX_1_MASK			0x000F0000
#define KS59XX_AMBEF_INCOMING_MAILBOX_4_MASK			0x0000F000
#define KS59XX_AMBEF_INCOMING_MAILBOX_3_MASK			0x00000F00
#define KS59XX_AMBEF_INCOMING_MAILBOX_2_MASK			0x000000F0
#define KS59XX_AMBEF_INCOMING_MAILBOX_1_MASK			0x0000000F

    
///////////////////////////////////////////////////////////////////////////////
// Addon General Control/Status Register bit definitions
// Definitions based on S5933 naming convention.  The S5920 ARCR matches the
// S5933's AGCSTS in bit location and function.
///////////////////////////////////////////////////////////////////////////////
// RO		- Read Only
// WO		- Write Only
// R/W		- Read/Write
// R/WC		- Read/Write_One_Clear
// WC		- Write_One_Clear


#define KS59XX_AGCSTS_NVRAM_INACTIVE					0x00		// WO
#define KS59XX_AGCSTS_NVRAM_LOAD_LOW_ADDR				0x80		// WO
#define KS59XX_AGCSTS_NVRAM_LOAD_HIGH_ADDR				0xA0		// WO
#define KS59XX_AGCSTS_NVRAM_BEGIN_WRITE					0xC0		// WO
#define KS59XX_AGCSTS_NVRAM_BEGIN_READ					0xE0		// WO
#define KS59XX_AGCSTS_NVRAM_BUSY						0x80000000	// RO
#define KS59XX_AGCSTS_TRANSFER_COUNT_ENABLE				0x10000000	// R/W? S5933 only
#define KS59XX_AGCSTS_MAILBOX_FLAG_RESET				0x08000000	// WC
#define KS59XX_AGCSTS_PCI_TO_ADDON_FIFO_RESET			0x04000000	// WC   S5933 only
#define KS59XX_AGCSTS_ADDON_TO_PCI_FIFO_RESET			0x02000000	// WC   Read FIFO reset for S5920
#define KS59XX_AGCSTS_ADDON_TO_PCI_XFER_COUNT_ZERO		0x00000080	// RO   S5933 only
#define KS59XX_AGCSTS_PCI_TO_ADDON_XFER_COUNT_ZERO		0x00000040	// RO   S5933 only
#define KS59XX_AGCSTS_PCI_TO_ADDON_FIFO_EMPTY			0x00000020	// RO   S5933 only
#define KS59XX_AGCSTS_PCI_TO_ADDON_FIFO_4_WORDS			0x00000010	// RO   S5933 only
#define KS59XX_AGCSTS_PCI_TO_ADDON_FIFO_FULL			0x00000008	// RO   S5933 only
#define KS59XX_AGCSTS_ADDON_TO_PCI_FIFO_EMPTY			0x00000004	// RO   S5933 only
#define KS59XX_AGCSTS_ADDON_TO_PCI_FIFO_4_SPACES		0x00000002	// RO   S5933 only
#define KS59XX_AGCSTS_ADDON_TO_PCI_FIFO_FULL			0x00000001	// RO   S5933 only

// AGCSTS masks
#define KS59XX_AGCSTS_PRESERVE_MASK						0x10FFF000
#define KS59XX_AGCSTS_NVRAM_PRESERVE_MASK				0x10

#endif // __AO59XXRD__
