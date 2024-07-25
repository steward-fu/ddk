// Ks59XXrd.h - include file for class KS59XX and related classes
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
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
#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

#ifndef __KS59XXRD__
#define __KS59XXRD__

// Register Definitions for AMCC S59XX

///////////////////////////////////////////////////////////////////////////////
// AMCC S5933 PCI Bus Operations Registers
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
typedef struct _AMCCS5933_PCI_BUS_OPERATION_REGISTERS
{
	ULONG OutgoingMailbox1;			// S5933 OMB1 - Outgoing Mailbox Reg 1
	ULONG OutgoingMailbox2;			// S5933 OMB2 - Outgoing Mailbox Reg 2
	ULONG OutgoingMailbox3;			// S5933 OMB3 - Outgoing Mailbox Reg 3
	ULONG OutgoingMailbox4;			// S5933 OMB4 - Outgoing Mailbox Reg 4
									// S5920 OMB  - Outgoing Mailbox Reg
	ULONG IncomingMailbox1;			// S5933 IMB1 - Incoming Mailbox Reg 1
	ULONG IncomingMailbox2;			// S5933 IMB2 - Incoming Mailbox Reg 2
	ULONG IncomingMailbox3;			// S5933 IMB3 - Incoming Mailbox Reg 3
	ULONG IncomingMailbox4;			// S5933 IMB4 - Incoming Mailbox Reg 4
									// S5920 IMB  - Incoming Mailbox Reg
	ULONG FIFORegisterPort;			// S5933 FIFO - FIFO Reg Port (bidirectional)
	ULONG MasterWriteAddress;		// S5933 MWAR - Master Write Address Reg
	ULONG MasterWriteTransferCount;	// S5933 MWTC - Master Write Transfer Count Reg
	ULONG MasterReadAddress;		// S5933 MRAR - Master Read Address Reg
	ULONG MasterReadTransferCount;	// S5933 MRTC - Master Read Transfer Count Reg
	ULONG MailboxEmptyFullStatus;	// S5933 MBEF - Mailbox Empty/Full Status Reg
	ULONG InterruptControlStatus;	// S5933 INTCSR - Interrupt Control/Status Reg
									// S5920 INTCSR - Interrupt Control/Status Reg
	ULONG BusMasterControlStatus;	// S5933 MCSR - Bus Master Control/Status Reg
									// S5920 RCR  - Reset Control Reg
} AMCCS5933_PCI_BUS_OPERATION_REGISTERS, *PAMCCS5933_PCI_BUS_OPERATION_REGISTERS;

// NOTE!! The following structure does NOT have the same offsets as the S5920 I/O region
typedef struct _AMCCS5920_PCI_BUS_OPERATION_REGISTERS
{
	ULONG OutgoingMailbox;			// S5920 OMB  - Outgoing Mailbox Reg
	ULONG IncomingMailbox;			// S5920 IMB  - Incoming Mailbox Reg
	ULONG InterruptControlStatus;	// S5920 INTCSR - Interrupt Control/Status Reg
	ULONG ResetControl;				// S5920 RCR  - Reset Control Reg
	ULONG PassThruConfiguration;	// S5920 PTCR - Pass-Thru Configuration Reg
} AMCCS5920_PCI_BUS_OPERATION_REGISTERS, *PAMCCS5920_PCI_BUS_OPERATION_REGISTERS;
#pragma pack(pop)

// Macro to determine the byte offset of the specified field within the AMCC S5933's
// register range.
#define OP_REG_OFFSET(field) (FIELD_OFFSET(AMCCS5933_PCI_BUS_OPERATION_REGISTERS,field))
#define AMCCS5920_PASS_THRU_CONFIG_OFFSET 0x60

#define AMCCS5933_PCI_BUS_OPERATION_REGISTERS_SIZE 0x40
#define AMCCS5920_PCI_BUS_OPERATION_REGISTERS_SIZE 0x64

///////////////////////////////////////////////////////////////////////////////
// AMCC59XX General definitions
///////////////////////////////////////////////////////////////////////////////
    // lowest legal mailbox number
const ULONG KS59XX_MAILBOX_MINIMUM = 1;
const ULONG KS59XX_MAILBOX_MAXIMUM = 4;
const ULONG KS59XX_BYTE_MINIMUM = 0;
const ULONG KS59XX_BYTE_MAXIMUM = 3;

	// Offset of Nvram size in 512 byte blocks
#define KS59XX_NVRAM_SIZE_OFFSET 0x2

	// Minimum offset of Nvram to clear locations that contain
	// basic device configuration neccesary for device enumeration.
#define KS59XX_NVRAM_PAST_CONFIG_OFFSET 0x80


// RO		- Read Only
// R/W		- Read/Write
// R/WC		- Read/Write_One_Clear
typedef enum {
	KS59XX_PCI_INTERFACE = 0,
	KS59XX_ADDON_INTERFACE = 1
} KS59XX_INTERFACE;

// PCI_TO_ADDON is the direction OUTGOING wrt PCI side mailboxes.
// PCI_TO_ADDON transfers are write DMA operations wrt NT driver perspective
// PCI_TO_ADDON transfers are read DMA operations wrt S59XX perspective
// ADDON_TO_PCI is the direction INCOMING wrt PCI side mailboxes.
// ADDON_TO_PCI transfers are read DMA operations wrt NT driver perspective
// ADDON_TO_PCI transfers are write DMA operations wrt S59XX perspective
typedef enum {
	KS59XX_PCI_TO_ADDON = 0,
	KS59XX_ADDON_TO_PCI = 1
} KS59XX_DIRECTION;

typedef enum {
	KS59XX_NO_CONVERSION = 0,
	KS59XX_16_BIT_CONVERSION = 1,
	KS59XX_32_BIT_CONVERSION = 2,
	KS59XX_64_BIT_CONVERSION = 3
} KS59XX_ENDIAN_CONVERSION;

///////////////////////////////////////////////////////////////////////////////
// Interrupt Control/Status Register bit definitions
// This register shares the same name and bit definitions between the S5933
// and S5920
///////////////////////////////////////////////////////////////////////////////

#define KS59XX_INTCSR_INTERRUPT_ASSERTED				0x00800000	// RO
#define KS59XX_INTCSR_ADDON								0x00400000	// RO   S5920 only
#define KS59XX_INTCSR_TARGET_ABORT						0x00200000	// R/WC S5933 only
#define KS59XX_INTCSR_MASTER_ABORT						0x00100000	// R/WC S5933 only
#define KS59XX_INTCSR_READ_XFER_COMPLETE				0x00080000	// R/WC S5933 only
#define KS59XX_INTCSR_WRITE_XFER_COMPLETE				0x00040000	// R/WC S5933 only
#define KS59XX_INTCSR_INCOMING_MAILBOX					0x00020000	// R/WC
#define KS59XX_INTCSR_OUTGOING_MAILBOX					0x00010000	// R/WC
#define KS59XX_INTCSR_INT_ON_READ_XFER_COMPLETE			0x00008000	// R/W  S5933 only
#define KS59XX_INTCSR_INT_ON_WRITE_XFER_COMPLETE		0x00004000	// R/W  S5933 only
#define KS59XX_INTCSR_INT_ON_ADDON						0x00002000	// R/W  S5920 only
#define KS59XX_INTCSR_INT_ON_INCOMING_MAILBOX			0x00001000	// R/W
#define KS59XX_INTCSR_INT_ON_OUTGOING_MAILBOX			0x00000010	// R/W

// Macro to compose flags for mailbox interrupts based on direction (KS59XX_DIRECTION),
// mailbox id and byte number.  A single interrupt condition can be setup for the set of
// INCOMING (ADDON_TO_PCI) mailboxes and a separate single condition can be setup for
// the OUTGOING (PCI_TO_ADDON) mailboxes.
#define KS59XX_INTCSR_MAILBOX_INT_CONDITION(direction, id, byte)			\
	((direction) ? (((id - 1) << 2) | byte) << 8 : (((id - 1) << 2) | byte))

// FIFO and Endian Control
#define KS59XX_INTCSR_PCI_TO_ADDON_FIFO_DWORD_TOGGLE	0x80000000	// R/W  S5933 only
#define KS59XX_INTCSR_ADDON_TO_PCI_FIFO_DWORD_TOGGLE	0x40000000	// R/W  S5933 only
#define KS59XX_INTCSR_64BIT_ENDIAN_CONVERSION			0x03000000	// R/W  S5933 only
#define KS59XX_INTCSR_32BIT_ENDIAN_CONVERSION			0x02000000	// R/W  S5933 only
#define KS59XX_INTCSR_16BIT_ENDIAN_CONVERSION			0x01000000	// R/W  S5933 only
#define KS59XX_INTCSR_NO_ENDIAN_CONVERSION				0x00000000	// R/W  S5933 only

// Macro to compose flags for FIFO advance condition based on which interface
// (KS59XX_INTERFACE) the condition is being set for, and the byte on which the
// FIFO should advance.  The advance condition controls, on which byte select
// the FIFO will advance its counter.  See AMCC5933 documentation. Note the byte
// advance condition is applied after the endian conversion.
// Applies to S5933 only.
#define KS59XX_INTCSR_FIFO_ADVANCE_CONDITION(iface, byte)					\
	((iface) ? byte << 28 : byte << 26)

// INTCSR masks
#define KS59XX_INTCSR_INIT_MASK							0xFF000000
#define KS59XX_INTCSR_OUTGOING_MAILBOX_MASK				0xFF00DF00
#define KS59XX_INTCSR_INCOMING_MAILBOX_MASK				0xFF00C01F
#define KS59XX_INTCSR_SET_CONTROL_MASK					0xFF00FFFF
#define KS59XX_INTCSR_INTERRUPT_STATUS_MASK				0x003F0000
#define KS59XX_INTCSR_INTERRUPT_ENABLE_MASK				0x0000D010
#define KS59XX_INTCSR_FIFO_CONTROL_MASK					0xFF3FDF1F


///////////////////////////////////////////////////////////////////////////////
// Mailbox Empty Full Status Register
///////////////////////////////////////////////////////////////////////////////

// MBEF masks
#define KS59XX_MBEF_INCOMING_MAILBOX_4_MASK				0xF0000000
#define KS59XX_MBEF_INCOMING_MAILBOX_3_MASK				0x0F000000
#define KS59XX_MBEF_INCOMING_MAILBOX_2_MASK				0x00F00000
#define KS59XX_MBEF_INCOMING_MAILBOX_1_MASK				0x000F0000
#define KS59XX_MBEF_OUTGOING_MAILBOX_4_MASK				0x0000F000
#define KS59XX_MBEF_OUTGOING_MAILBOX_3_MASK				0x00000F00
#define KS59XX_MBEF_OUTGOING_MAILBOX_2_MASK				0x000000F0
#define KS59XX_MBEF_OUTGOING_MAILBOX_1_MASK				0x0000000F


///////////////////////////////////////////////////////////////////////////////
// Bus Master Control/Status Register bit definitions
// Definitions based on S5933 naming convention.  The S5920 RCR matches the
// S5933's MCSR in bit location and function.
///////////////////////////////////////////////////////////////////////////////
// RO		- Read Only
// WO		- Write Only
// R/W		- Read/Write
// R/WC		- Read/Write_One_Clear
// WC		- Write_One_Clear


#define KS59XX_MCSR_NVRAM_INACTIVE						0x00		// WO
#define KS59XX_MCSR_NVRAM_LOAD_LOW_ADDR					0x80		// WO
#define KS59XX_MCSR_NVRAM_LOAD_HIGH_ADDR				0xA0		// WO
#define KS59XX_MCSR_NVRAM_BEGIN_WRITE					0xC0		// WO
#define KS59XX_MCSR_NVRAM_BEGIN_READ					0xE0		// WO
#define KS59XX_MCSR_NVRAM_BUSY							0x80000000	// RO
#define KS59XX_MCSR_FIFO_LOOPBACK						0x10000000	// R/W? S5933 only
#define KS59XX_MCSR_MAILBOX_FLAG_RESET					0x08000000	// WC
#define KS59XX_MCSR_ADDON_TO_PCI_FIFO_RESET				0x04000000	// WC   S5933 only
#define KS59XX_MCSR_PCI_TO_ADDON_FIFO_RESET				0x02000000	// WC   Read FIFO reset for S5920
#define KS59XX_MCSR_ADDON_PIN_RESET						0x01000000	// R/W
#define KS59XX_MCSR_MEMORY_READ_MULTIPLE_ENABLE			0x00008000	// R/W? S5933 only
#define KS59XX_MCSR_READ_XFER_ENABLE					0x00004000	// R/W  S5933 only
#define KS59XX_MCSR_READ_FIFO_SCHEME					0x00002000	// R/W  S5933 only
#define KS59XX_MCSR_READ_VS_WRITE_PRIORITY				0x00001000	// R/W  S5933 only
#define KS59XX_MCSR_WRITE_XFER_ENABLE					0x00000400	// R/W  S5933 only
#define KS59XX_MCSR_WRITE_FIFO_SCHEME					0x00000200	// R/W  S5933 only
#define KS59XX_MCSR_WRITE_VS_READ_PRIORITY				0x00000100	// R/W  S5933 only
#define KS59XX_MCSR_ADDON_TO_PCI_XFER_COUNT_ZERO		0x00000080	// RO   S5933 only
#define KS59XX_MCSR_PCI_TO_ADDON_XFER_COUNT_ZERO		0x00000040	// RO   S5933 only
#define KS59XX_MCSR_ADDON_TO_PCI_FIFO_EMPTY				0x00000020	// RO   S5933 only
#define KS59XX_MCSR_ADDON_TO_PCI_FIFO_4_WORDS			0x00000010	// RO   S5933 only
#define KS59XX_MCSR_ADDON_TO_PCI_FIFO_FULL				0x00000008	// RO   S5933 only
#define KS59XX_MCSR_PCI_TO_ADDON_FIFO_EMPTY				0x00000004	// RO   S5933 only
#define KS59XX_MCSR_PCI_TO_ADDON_FIFO_4_SPACES			0x00000002	// RO   S5933 only
#define KS59XX_MCSR_PCI_TO_ADDON_FIFO_FULL				0x00000001	// RO   S5933 only

// MCSR masks
#define KS59XX_MCSR_PRESERVE_MASK						0x11FFF700
#define KS59XX_MCSR_NVRAM_PRESERVE_MASK					0x11

#endif // __KS59XXRD__

#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
