// E100bexCard.cpp: implementation of the E100bexCard class.
//
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


#include <kndis.h>
#include "E100bexInc.h"


////////////////////////////////////////////////////////////////////
// E100bexCard::E100bexCard
//
// Constructor for E100bex card object
//
// Parameters:
//		Adapter
//			Reference to the E100bexAdapter.  This is necessary so we can
//			use the adapter's handle in NDIS calls.
//		CSRRange
//			Reference to the initialized memory range where our CSR
//			registers reside on the card
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE:
//

E100bexCard::E100bexCard(E100bexAdapter& Adapter, KNdisMemoryRange& CSRRange, ADAPTER_INFO& Ai) :
	m_Adapter(Adapter),
	m_CSRAddress(CSRRange),
	m_Ai(Ai),
	m_DumpSpace(NULL),
	m_DumpSpacePhys(0),
	m_NonTxCmdBlock(NULL),
	m_NonTxCmdBlockHdr(NULL),
	m_NonTxCmdBlockPhys(0),
	m_StatsCounters(NULL),
	m_StatsCounterPhys(0),
	m_SelfTest(NULL),
	m_SelfTestPhys(0),
	m_Phy(Ai.PhyAddress,Ai.Connector, Ai.AiForceDpx, Ai.AiTempSpeed, Ai.Congest, m_CSRAddress.MDIControl )
{
}

////////////////////////////////////////////////////////////////////
// E100bexCard::ReadPermanentNodeAddress
//
// Reads the permanent node address stored in the EEprom on the card
//
// Parameters:
//		none
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE:
//
ETHERNET_ADDRESS E100bexCard::ReadPermanentNodeAddress(void)
{
	ETHERNET_ADDRESS PermanentNodeAddress;
	USHORT EepromWordValue;
	for (int i=0; i<6; i += 2)
	{
		EepromWordValue = ReadEEprom((USHORT) (EEPROM_NODE_ADDRESS_BYTE_0 + (i/2)));

		TRACE("EEPROM word %x reads %x\n",
			EEPROM_NODE_ADDRESS_BYTE_0 + (i/2), EepromWordValue);

		PermanentNodeAddress.m_bytes[i] = (UCHAR) EepromWordValue;
		PermanentNodeAddress.m_bytes[i+1] = (UCHAR) (EepromWordValue >> 8);
	}
	return PermanentNodeAddress;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::DisableInterrupt
//
// Disables the interrupt on the card.
//
// Parameters:
//		none
// IRQL:
//		Any (called from the ISR)
// Return Mode:
//		Synchronous
//
// NOTE:
//
VOID E100bexCard::DisableInterrupt(void)
{
	TRACE3(("E100bexCard::DisableInterrupt() Entered\n"))

	// set the M (mask) bit in the adapter's CSR SCB command word
	m_CSRAddress.ScbCommandHigh = (UCHAR)SCB_INT_MASK;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::EnableInterrupt
//
// This routine enables interrupts at the hardware, by resetting
//              the M (mask) bit in the adapter's CSR SCB command word
//
// Parameters:
//		none
// IRQL:
//		Any (called from the ISR)
// Return Mode:
//		Synchronous
//
// NOTE:
//
VOID E100bexCard::EnableInterrupt(void)
{
	TRACE3(("E100bexCard::EnableInterrupt() Entered\n"))

	// clear the M (mask) bit in the adapter's CSR SCB command word
	m_CSRAddress.ScbCommandHigh = (UCHAR)0;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::ReadEEprom
//
// This routine serially reads one word out of the EEPROM.
//
// Parameters:
//      Reg - EEPROM word to read.
// IRQL:
//
// Return Mode:
//		Synchronous
// NOTE:
//		Returns contents of EEPROM word (Reg).
//
USHORT E100bexCard::ReadEEprom(IN USHORT Reg)
{
	USHORT x;
	USHORT data;

	// select EEPROM, reset bits, set EECS
	x = m_CSRAddress.EepromControl;

	x &= ~(EEDI | EEDO | EESK);
	x |= EECS;

	m_CSRAddress.EepromControl = x;

	// write the read opcode and register number in that order
	// The opcode is 3bits in length, reg is 6 bits long
	EEpromShiftOutBits(EEPROM_READ_OPCODE, 3);
	EEpromShiftOutBits(Reg, 6);

	// Now read the data (16 bits) in from the selected EEPROM word
	data = EEpromShiftInBits();

	EEpromCleanup();
	return data;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::EEpromRaiseClock
//
// This routine raises the EEPOM's clock input (EESK)
//
// Parameters:
//      x - Ptr to the EEPROM control register's current value
// IRQL:
//
// Return Mode:
//		Synchronous
// NOTE:
//
VOID E100bexCard::EEpromRaiseClock(IN OUT USHORT *x)
{
	*x = *x | EESK;

	m_CSRAddress.EepromControl = *x;

	NdisStallExecution(100);
}

////////////////////////////////////////////////////////////////////
// E100bexCard::EEpromLowerClock
//
// This routine lowers the EEPOM's clock input (EESK)
//
// Parameters:
//      x - Ptr to the EEPROM control register's current value
// IRQL:
//
// Return Mode:
//		Synchronous
// NOTE:
//
VOID E100bexCard::EEpromLowerClock(IN OUT USHORT *x)
{
	*x = *x & ~EESK;

	m_CSRAddress.EepromControl = *x;

	NdisStallExecution(100);
}

////////////////////////////////////////////////////////////////////
// E100bexCard::EEpromShiftOutBits
//
// This routine shifts data bits out to the EEPROM.
//
// Parameters:
//      data - data to send to the EEPROM.
//      count - number of data bits to shift out.
// IRQL:
//
// Return Mode:
//		Synchronous
// NOTE:
//
VOID E100bexCard::EEpromShiftOutBits(USHORT data, USHORT count)
{
	USHORT x,mask;

	mask = 0x01 << (count - 1);
	x = m_CSRAddress.EepromControl;
	x &= ~(EEDO | EEDI);

	do
	{
		x &= ~EEDI;
		if (data & mask)
			x |= EEDI;

		m_CSRAddress.EepromControl = x;

		NdisStallExecution(100);
		EEpromRaiseClock(&x);
		EEpromLowerClock(&x);
		mask = mask >> 1;
	} while (mask);

	x &= ~EEDI;

	m_CSRAddress.EepromControl = x;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::EEpromShiftInBits
//
// This routine shifts data bits in from the EEPROM.
//
// Parameters:
//		none
// IRQL:
//
// Return Mode:
//		Synchronous
// NOTE:
//		Returns the contents of that particular EEPROM word.
//
USHORT E100bexCard::EEpromShiftInBits(void)
{
	USHORT x,d,i;

	x = m_CSRAddress.EepromControl;

	x &= ~( EEDO | EEDI);
	d = 0;

	for (i=0; i<16; i++)
	{
		d = d << 1;
		EEpromRaiseClock(&x);

		x = m_CSRAddress.EepromControl;

		x &= ~(EEDI);
		if (x & EEDO)
			d |= 1;

		EEpromLowerClock(&x);
	}

	return d;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::EEpromCleanup
//
// This routine returns the EEPROM to an idle state.
//
// Parameters:
//		none
// IRQL:
//
// Return Mode:
//		Synchronous
// NOTE:
//
VOID E100bexCard::EEpromCleanup(void)
{
	USHORT x;

	x = m_CSRAddress.EepromControl;

	x &= ~(EECS | EEDI);
	m_CSRAddress.EepromControl = x;

	EEpromRaiseClock(&x);
	EEpromLowerClock(&x);
}

////////////////////////////////////////////////////////////////////
// E100bexCard::SelfTest
//
//	This routine will issue PORT Self-test command to the D100.
//
// Parameters:
//		none
// IRQL:
//
// Return Mode:
//		Synchronous
// Returns:
//    NDIS_STATUS_SUCCESS - If the adapter passes the self-test
//    NDIS_STATUS_FAILURE- If the adapter fails the self-test
// NOTE:
//	The self-test will fail if the adapter's master-enable
//              bit is not set in the PCI Command Register, of if the adapter
//              is not seated in a PCI master-enabled slot.
//
NDIS_STATUS E100bexCard::SelfTest(void)
{
	TRACE("E100bexCard::SelfTest() Entered\n");

	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

	// Issue a software reset to the adapter
	SoftwareReset();

	// Execute The PORT Self Test Command On The 82558.
	ASSERT(m_SelfTestPhys != 0);
	ULONG dwSelfTestCommandCode = m_SelfTestPhys;

	// Setup SELF TEST Command Code in D3 - D0
	dwSelfTestCommandCode |= PORT_SELFTEST;

	// Initialize the self-test signature and results DWORDS
	m_SelfTest->StSignature = 0;
	m_SelfTest->StResults = 0xffffffff;

	// Do the port command
	m_CSRAddress.Port = dwSelfTestCommandCode;

	// Wait 5 milliseconds for the self-test to complete
	StallExecution(5);

	// if The First Self Test DWORD Still Zero, We've timed out.  If the second
	// DWORD is not zero then we have an error.
	if ( (m_SelfTest->StSignature == 0) || (m_SelfTest->StResults != 0))
	{
		TRACE("Adapter self-test failed.\n");
		Status = NDIS_STATUS_FAILURE;
	}

	return Status;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::SoftwareReset
//
// This routine resets the D100 by issuing a PORT SOFTWARE RESET.
//
// Parameters:
//		none
// IRQL:
//
// Return Mode:
//		Synchronous
// NOTE:
//
VOID E100bexCard::SoftwareReset(void)
{
	// Issue a PORT command with a data word of 0
	m_CSRAddress.Port = (USHORT)PORT_SOFTWARE_RESET;

	// wait 20 milliseconds for the reset to take effect
	StallExecution(20);

	// Mask off our interrupt line -- its unmasked after reset
	DisableInterrupt();
}

////////////////////////////////////////////////////////////////////
// E100bexCard::Init
//
// This routine will perform the initial configuration on the
//              the 82557 (D100) chip.  This will include loading the CU and
//              RU base values (0 in both cases), and calling other routines
//              that will issue a configure command to the 82257, notify the
//              82557 of its node address, and clear all of the on-chip
//              counters.
//
// Returns:
//      TRUE - If 82557 chip was initialized
//      FALSE - If 82557 failed initialization
//
NDIS_STATUS E100bexCard::Init(ETHERNET_ADDRESS NodeAddress)
{
	TRACE("E100bexCard::Init");
	NDIS_STATUS     Status;

	do
	{
		// Issue a software reset to the D100
		SoftwareReset();

		// Load the CU BASE (set to 0, because we use linear mode)
		m_CSRAddress.ScbGeneralPointer = (ULONG)0;
		Status = IssueScbCommand(SCB_CUC_LOAD_BASE,FALSE);
		if (Status != NDIS_STATUS_SUCCESS)
		{
			break;
		}

		// Wait for the SCB command word to clear before we set the general pointer
		if (!WaitScb())
		{
			Status = NDIS_STATUS_HARD_ERRORS;
			break;
		}

		// Load the RU BASE (set to 0, because we use linear mode)
		m_CSRAddress.ScbGeneralPointer = (ULONG)0;
		Status = IssueScbCommand(SCB_RUC_LOAD_BASE, FALSE);
		if (Status != NDIS_STATUS_SUCCESS)
		{
			break;
		}

		// Configure the adapter
		Status = Configure();
		if (Status != NDIS_STATUS_SUCCESS)
		{
			break;
		}

		Status = SetupIAAddress(NodeAddress);
		if (Status != NDIS_STATUS_SUCCESS)
		{
			break;
		}

		// Clear the internal counters
		ClearAllCounters();

	} while (FALSE);

	return Status;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::WaitScb
//
// This routine checks to see if the D100 has accepted a command.
//              It does so by checking the command field in the SCB, which will
//              be zeroed by the D100 upon accepting a command.  The loop waits
//              for up to 600 milliseconds for command acceptance.
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// Returns:
//      TRUE if the SCB cleared within 600 milliseconds.
//      FALSE if it didn't clear within 600 milliseconds
// NOTE:
//
inline BOOLEAN E100bexCard::WaitScb(void)
{
	TRACE3(("Entered E100bexCard::WaitScb"));

	UINT count = 60000;

	do
	{
		if (! (UCHAR) m_CSRAddress.ScbCommandLow )
		{
			return TRUE;
		}

		NdisStallExecution(10);
	} while (count--);

	// If we fall through, we have a problem.
	TRACE("HARDWARE_NOT_RESPONDING\n");

	return FALSE;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::IssueScbCommand
//
// This routine will issue a command to the D100.
//
// Parameters:
//      ScbCommandLow - The command that is to be issued
//      WaitForSCB    - A boolean value indicating whether or not a wait for SCB
//                      must be done before the command is issued to the chip
// IRQL:
//
// Return Mode:
//		Synchronous
// Returns:
//      TRUE if the command was issued to the chip successfully
//      FALSE if the command was not issued to the chip
// NOTE:
//
inline NDIS_STATUS E100bexCard::IssueScbCommand(UCHAR ScbCommandLow, BOOLEAN WaitForScb)
{
	TRACE3(("Entered E100bexCard::IssueScbCommand"));

	if (TRUE == WaitForScb)
	{
		if (!WaitScb())
		{
			TRACE("WaitScb failed\n");
			return NDIS_STATUS_HARD_ERRORS;
		}
	}

	m_CSRAddress.ScbCommandLow = ScbCommandLow;

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::SubmitCommandBlockAndWait
//
//	This routine will submit a command block to be executed, and
//              then it will wait for that command block to be executed.  Since
//              board ints will be disabled, we will ack the interrupt in
//              this routine.
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// Returns:
//      NDIS_STATUS_SUCCESS - If we successfully submitted and completed the command.
//      NDIS_STATUS_HARD_ERRORS - If we didn't successfully submit and complete the command.
// NOTE:
//
NDIS_STATUS E100bexCard::SubmitCommandBlockAndWait(void)
{
	TRACE("Entered E100bexCard::SubmitCommandBlockAndWait\n");

	NDIS_STATUS		Status;

	// Points to the Non Tx Command Block.
	volatile PNON_TRANSMIT_CB CommandBlock = m_NonTxCmdBlock;

	// Set the Command Block to be the last command block
	CommandBlock->NonTxCb.Config.ConfigCBHeader.CbCommand |= CB_EL_BIT;

	// Clear the status of the command block
	CommandBlock->NonTxCb.Config.ConfigCBHeader.CbStatus = 0;

#if DBG
	// Don't try to start the CU if the command unit is active.
	if ((USHORT(m_CSRAddress.ScbStatus) & SCB_CUS_MASK) == SCB_CUS_ACTIVE)
	{
		TRACE("Scb %08x ScbStatus %04x\n", &m_CSRAddress, USHORT(m_CSRAddress.ScbStatus) );
		ASSERT(0);
		return NDIS_STATUS_HARD_ERRORS;
	}
#endif


	// Start the command unit.
	IssueScbCommand(SCB_CUC_START, FALSE);

	// Wait for the SCB to clear, indicating the completion of the command.
	if (WaitScb() == FALSE)
	{
		TRACE("WaitScb failed\n");
		return NDIS_STATUS_HARD_ERRORS;
	}

	// Wait for some status, timeout value 3 secs
	UINT Delay = 300000;
	while ((!(CommandBlock->NonTxCb.Config.ConfigCBHeader.CbStatus & CB_STATUS_COMPLETE)) && Delay)
	{
		NdisStallExecution(10);
		Delay--;
	}

	if (!Delay)
	{
		TRACE("HARDWARE_NOT_RESPONDING\n");
		return NDIS_STATUS_HARD_ERRORS;
	}

	// Ack any interrupts
	USHORT ScbStatus = USHORT(m_CSRAddress.ScbStatus);
	if ( ScbStatus & SCB_ACK_MASK)
	{
		// Ack all pending interrupts now
		m_CSRAddress.ScbStatus = (USHORT)(ScbStatus & SCB_ACK_MASK);
	}

	// Check the status of the command, and if the command failed return FALSE,
	// otherwise return TRUE.
	if (!(CommandBlock->NonTxCb.Config.ConfigCBHeader.CbStatus & CB_STATUS_OK))
	{
		TRACE("Command failed\n");
		Status = NDIS_STATUS_HARD_ERRORS;
	}
	else
	{
		Status = NDIS_STATUS_SUCCESS;
	}

	return Status;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::Configure
//
//	This routine will issue a configure command to the 82557.
//              This command will be executed in polled mode.  The
//              Configuration parameters that are user configurable will
//              have been set when the driver parsed the configuration
//              parameters out of the registry.
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// Returns:
//      NDIS_STATUS_SUCCESS - If the configure command was successfully issued and completed
//      NDIS_STATUS_HARD_ERRORS - If the configure command failed to complete properly
// NOTE:
//
NDIS_STATUS E100bexCard::Configure(void)
{
	TRACE("E100bexCard::Configure");
	NDIS_STATUS         Status;

	// Init the packet filter to nothing.
	//m_OldPacketFilter = m_PacketFilter;
	m_PacketFilter = 0;

	// Store the current setting for BROADCAST/PROMISCUOS modes
	m_OldParameterField = CB_557_CFIG_DEFAULT_PARM15;

	// Setup the non-transmit command block header for the configure command.
	m_NonTxCmdBlockHdr->CbStatus = 0;
	m_NonTxCmdBlockHdr->CbCommand = CB_CONFIGURE;
	m_NonTxCmdBlockHdr->CbLinkPointer = DRIVER_NULL;

	// Fill in the configure command data.

	// First fill in the static (end user can't change) config bytes
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[0] = CB_557_CFIG_DEFAULT_PARM0;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[2] = CB_557_CFIG_DEFAULT_PARM2;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[3] = CB_557_CFIG_DEFAULT_PARM3;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[6] = CB_557_CFIG_DEFAULT_PARM6;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[9] = CB_557_CFIG_DEFAULT_PARM9;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[10] = CB_557_CFIG_DEFAULT_PARM10;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[11] = CB_557_CFIG_DEFAULT_PARM11;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[12] = CB_557_CFIG_DEFAULT_PARM12;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[13] = CB_557_CFIG_DEFAULT_PARM13;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[14] = CB_557_CFIG_DEFAULT_PARM14;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[16] = CB_557_CFIG_DEFAULT_PARM16;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[17] = CB_557_CFIG_DEFAULT_PARM17;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[18] = CB_557_CFIG_DEFAULT_PARM18;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[20] = CB_557_CFIG_DEFAULT_PARM20;
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[21] = CB_557_CFIG_DEFAULT_PARM21;

	// Now fill in the rest of the configuration bytes (the bytes that contain
	// user configurable parameters).

	// Set the Tx and Rx Fifo limits
	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[1] =
		(UCHAR) ((m_Ai.AiTxFifo << 4) | m_Ai.AiRxFifo);

	if (m_Ai.MWIEnable)
	{
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[3] |= CB_CFIG_B3_MWI_ENABLE;
	}

	// Set the Tx and Rx DMA maximum byte count fields.
	if ((m_Ai.AiRxDmaCount) || (m_Ai.AiTxDmaCount))
	{
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[4] =
			m_Ai.AiRxDmaCount;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[5] =
			(UCHAR) (m_Ai.AiTxDmaCount | CB_CFIG_DMBC_EN);
	}
	else
	{
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[4] =
			CB_557_CFIG_DEFAULT_PARM4;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[5] =
			CB_557_CFIG_DEFAULT_PARM5;
	}


	m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[7] =
		(UCHAR) ((CB_557_CFIG_DEFAULT_PARM7 & (~CB_CFIG_URUN_RETRY)) |
		(m_Ai.AiUnderrunRetry << 1)
		);

	// Setup for MII or 503 operation.  The CRS+CDT bit should only be set
	// when operating in 503 mode.
	if ( m_Phy.GetPhyAddress() == 32)
	{
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[8] =
			(CB_557_CFIG_DEFAULT_PARM8 & (~CB_CFIG_503_MII));
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[15] =
			(CB_557_CFIG_DEFAULT_PARM15 | CB_CFIG_CRS_OR_CDT);
	}
	else
	{
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[8] =
			(CB_557_CFIG_DEFAULT_PARM8 | CB_CFIG_503_MII);
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[15] =
			((CB_557_CFIG_DEFAULT_PARM15 & (~CB_CFIG_CRS_OR_CDT)) | CB_CFIG_BROADCAST_DIS);
	}


	// Setup Full duplex stuff

	// If forced to half duplex
	if (m_Ai.AiForceDpx == 1)
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[19] =
		(CB_557_CFIG_DEFAULT_PARM19 &
		(~(CB_CFIG_FORCE_FDX| CB_CFIG_FDX_ENABLE)));

	// If forced to full duplex
	else if (m_Ai.AiForceDpx == 2)
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[19] =
		(CB_557_CFIG_DEFAULT_PARM19 | CB_CFIG_FORCE_FDX);

	// If auto-duplex
	else
	{
		// We must force full duplex on if we are using PHY 0, and we are
		// supposed to run in FDX mode.  We do this because the D100 has only
		// one FDX# input pin, and that pin will be connected to PHY 1.
		if ((m_Phy.GetPhyAddress() == 0) && (m_Phy.GetCurrentDuplexMode() == 2))
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[19] =
			(CB_557_CFIG_DEFAULT_PARM19 | CB_CFIG_FORCE_FDX);
		}
		else
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[19] =
			CB_557_CFIG_DEFAULT_PARM19;
		}
	}


	// display the config info to the debugger
	TRACE("Issuing Configure command\n");
	TRACE("Config Block at virt addr %p, phys address %x\n",
		&m_NonTxCmdBlockHdr->CbStatus, m_NonTxCmdBlockPhys);

	for (UINT i=0; i < CB_CFIG_BYTE_COUNT; i++)
	{
		TRACE("  Config byte %x = %.2x\n", i, m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[i]);
	}

	// Wait for the SCB command word to clear before we set the general pointer
	if (!WaitScb())
	{
		Status = NDIS_STATUS_HARD_ERRORS;
	}
	else
	{
		ASSERT( 0 == (UCHAR) m_CSRAddress.ScbCommandLow) ;
		m_CSRAddress.ScbGeneralPointer = m_NonTxCmdBlockPhys;

		// Submit the configure command to the chip, and wait for it to complete.
		Status = SubmitCommandBlockAndWait();
	}

	return Status;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::SetupIAAddress
//
//	This routine will issue the IA setup command.  This command
//              will notify the 82557 (D100) of what its individual (node)
//              address is.  This command will be executed in polled mode.
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// Returns:
//      NDIS_STATUS_SUCCESS - If the IA setup command was successfully issued and completed
//      NDIS_STATUS_HARD_ERRORS - If the IA setup command failed to complete properly
// NOTE:
//
NDIS_STATUS E100bexCard::SetupIAAddress(ETHERNET_ADDRESS NodeAddress)
{
	TRACE("E100bexCard::SetupIAAddress\n");

	// Individual Address Setup
	m_NonTxCmdBlockHdr->CbStatus = 0;
	m_NonTxCmdBlockHdr->CbCommand = CB_IA_ADDRESS;
	m_NonTxCmdBlockHdr->CbLinkPointer = DRIVER_NULL;

	// Copy in the station's individual address
	for (UINT i = 0; i < ETH_LENGTH_OF_ADDRESS; i++)
	{
		m_NonTxCmdBlock->NonTxCb.Setup.IaAddress[i] = NodeAddress.m_bytes[i];
	}

	// Update the command list pointer.  We don't need to do a WaitSCB here
	// because this command is either issued immediately after a reset, or
	// after another command that runs in polled mode.  This guarantees that
	// the low byte of the SCB command word will be clear.  The only commands
	// that don't run in polled mode are transmit and RU-start commands.
	ASSERT( UCHAR(m_CSRAddress.ScbCommandLow) == 0);
	m_CSRAddress.ScbGeneralPointer = m_NonTxCmdBlockPhys;

	// Submit the IA configure command to the chip, and wait for it to complete.
	return SubmitCommandBlockAndWait();
}

////////////////////////////////////////////////////////////////////
// E100bexCard::ClearAllCounters
//
//	This routine will dump/reset all of the statistics.
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// NOTE:
//
VOID E100bexCard::ClearAllCounters()
{
	TRACE("E100bexCard::ClearAllCounters\n");

	// Load the dump counters pointer.  Since this command is generated only
	// after the IA setup has complete, we don't need to wait for the SCB
	// command word to clear
	ASSERT( UCHAR(m_CSRAddress.ScbCommandLow) == 0);
	m_CSRAddress.ScbGeneralPointer = m_StatsCounterPhys;

	// Issue the load dump counters address command
	IssueScbCommand(SCB_CUC_DUMP_ADDR, FALSE);

	// Now dump and reset all of the statistics
	IssueScbCommand(SCB_CUC_DUMP_RST_STAT, TRUE);

	// Now wait for the dump/reset to complete
	for (UINT counter = 100000; counter != 0; counter--)
	{
		if (m_StatsCounters->CommandComplete == 0xA007)
		{
			break;
		}

		NdisStallExecution(20);
	}

	if (!counter)
	{
		TRACE("HARDWARE_NOT_RESPONDING\n");
		return;
	}
}

////////////////////////////////////////////////////////////////////
// E100bexCard::ReceiveUnitStarted
//
//	This routine checks the status of the 82557's receive unit(RU),
//	to see if it is started
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// Returns:
//      TRUE - The receive unit is started
//      FALSE - The receive unit is not started
// NOTE:
//
BOOLEAN E100bexCard::ReceiveUnitStarted()
{
	// If the receiver is ready, return TRUE.
	if (( USHORT(m_CSRAddress.ScbStatus) & SCB_RUS_MASK ) == SCB_RUS_READY)
	{
		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::StartReceiveUnit
//
//	This routine checks the status of the 82557's receive unit(RU),
//              and starts the RU if it was not already active.  However,
//              before restarting the RU, the driver cleans up any recent
//              pending receives (this is very important).
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// Returns:
//      TRUE - If we indicated any receives during this function call
//      FALSE - If we didn't indicate any receives
// NOTE:
//
VOID E100bexCard::StartReceiveUnit(PNDIS_PHYSICAL_ADDRESS pPhysAddr)
{
	ASSERT( pPhysAddr );

	// Wait for the SCB to clear before we set the general pointer
	if ( FALSE == WaitScb() )
	{
		ASSERT(0);
	}

	// Set the SCB General Pointer to point the current Rfd
	m_CSRAddress.ScbGeneralPointer = NdisGetPhysicalAddressLow(*pPhysAddr);

	// Issue the SCB RU start command
	IssueScbCommand(SCB_RUC_START, FALSE);

	// wait for the command to be accepted
	WaitScb();

	UINT WaitCount = 80000;

	// wait for RUS to be Ready
	while (WaitCount != 0)
	{
		if (( USHORT(m_CSRAddress.ScbStatus) & SCB_RUS_MASK) == SCB_RUS_READY)
			break;

		NdisStallExecution(10);
		WaitCount--;
	}
	if (!WaitCount)
	{
		TRACE("HARDWARE_NOT_RESPONDING");
	}

#if DBG
	// If we fall through, we have a problem.
	if (WaitCount == 0)
		TRACE("Failed, RU won't ready -- ScbStatus %08x\n", USHORT(m_CSRAddress.ScbStatus) );
#endif
}

////////////////////////////////////////////////////////////////////
// E100bexCard::IsInterrupting
//
//	This routine checks if the card is interrupting (the interrupt line
//	is high, and the interrupt is not masked.)  If the interrupt line
//	is already masked, then the driver is currently processing interrupts
//
// Parameters:
//		None
// IRQL:
//		DIRQL - called from Adapter class ISR
// Return Mode:
//
// Returns:
//      TRUE - If the card is interrupting
//      FALSE - Its not this card's interrupt or its masked
// NOTE:
//

BOOLEAN	E100bexCard::IsInterrupting(void)
{
	if ((!( UCHAR(m_CSRAddress.ScbCommandHigh) & SCB_INT_MASK)) &&
		( ( USHORT(m_CSRAddress.ScbStatus) & SCB_ALL_INTERRUPT_BITS) != SCB_ALL_INTERRUPT_BITS) &&
		  ( USHORT(m_CSRAddress.ScbStatus) & SCB_ACK_MASK))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////
// E100bexCard::AckPendingInterrupts
//
//	This routine reads the ScbStatus register and checks the reason
//	for the interrupt.  If there are no interrupts to process,
//	returns FALSE.  If there are pending interrupts, ack the
//	interrupt and return TRUE.
//
// Parameters:
//		None
// IRQL:
//		Any - called from Adapter class DPC
// Return Mode:
//
// NOTE:
//
BOOLEAN E100bexCard::AckPendingInterrupts(void)
{
	// Check the interrupt status bits of the SCB to determine the reason
	// for the interrupt.

	volatile USHORT AckCommand = USHORT(m_CSRAddress.ScbStatus);

	TRACE3(("E100bexCard::AckPendingInterrupts, ScbStatus = %04x\n", AckCommand))

	// Strip off the ACK bits.
	AckCommand &= SCB_ACK_MASK;

	m_CSRAddress.ScbStatus = AckCommand;

	return TRUE;
}


////////////////////////////////////////////////////////////////////
// E100bexCard::SetMcTimeoutFlag
//
// Set the McTimeoutFlag variable
//
// Parameters:
//		none
// IRQL:
//
// Return Mode:
//
// NOTE:
//  This McTimeoutFlag will determine whether the
//	driver issues a periodic multicast command or not.  Issuing a periodic
//	multicast command will reset the 82557's internal receive state machine,
//	and bring the 82557 out of a "Receive-lockup" state. This is a documented
//	errata in the 82557 software developement manual.
//	If the end user didn't over-ride the McTimeout flag, then it should be
//	enabled only for 10mb operation, and only if the EEPROM indicates that
//	this adapter has a potential rcv-lockup problem.
//
VOID E100bexCard::SetMcTimeoutFlag()
{
	if (m_Ai.McTimeoutFlag == 2)
	{
		// Read the EEPROM flags register
		USHORT EepromFlags = ReadEEprom(EEPROM_FLAGS_WORD_3);
		if ((( m_Phy.GetCurrentLineSpeed() == 10) && (!(EepromFlags & EEPROM_FLAG_10MC))) ||
			(( m_Phy.GetCurrentLineSpeed() == 100) && (!(EepromFlags & EEPROM_FLAG_100MC))))
		{
			m_Ai.McTimeoutFlag = 1;
		}
		else
		{
			m_Ai.McTimeoutFlag = 0;
		}
	}

	TRACE("MC Timeout Flag = %x\n", m_Ai.McTimeoutFlag);
}


////////////////////////////////////////////////////////////////////
// E100bexCard::Setupsharedmemory
//
// Initalize E100bexCard data members with shared memory.
//
// Parameters:
//		MemPhys	- Physical address
//		ShMem	- KNdisSharedMemory object
// IRQL:
//		Any
// Return Mode:
//		Synchronous
// NOTE:
//		The Adapter will call this method after allocating
//	shared memory.  The method will initialize the data members that
//	point into shared memory.  Alignment will be performed.  The
//	method returns the physical address pointing just beyond the
//	area of shared memory used by E100bexCard.
//
ULONG E100bexCard::SetupSharedMemory(ULONG MemPhys, KNdisSharedMemory& ShMem)
{
	if ( !ShMem.IsValid() )
	{
		ASSERT(ShMem.IsValid());
		return MemPhys;
	}

	// Setup SelfTest Command Block Pointers
	PUCHAR pMem = m_Adapter.AlignSharedMemory(&MemPhys, ALIGN_16, ShMem);
	m_SelfTest = reinterpret_cast<PSELF_TEST_STRUC>(pMem);
	m_SelfTestPhys = MemPhys;
	MemPhys += sizeof(SELF_TEST_STRUC);

	// Setup non-Transmit Command Block Pointers
	pMem = m_Adapter.AlignSharedMemory(&MemPhys, ALIGN_16, ShMem);
	m_NonTxCmdBlock = reinterpret_cast<PNON_TRANSMIT_CB>(pMem);
	m_NonTxCmdBlockHdr = reinterpret_cast<PCB_HEADER_STRUC>(pMem);
	m_NonTxCmdBlockPhys = MemPhys;
	MemPhys += sizeof(NON_TRANSMIT_CB);

	// Setup dump buffer area
	pMem = m_Adapter.AlignSharedMemory(&MemPhys, ALIGN_16, ShMem);
	m_DumpSpace = reinterpret_cast<PDUMP_AREA_STRUC>(pMem);
	m_DumpSpacePhys = MemPhys;
	MemPhys += sizeof(DUMP_AREA_STRUC);

	// Setup stats counters area
	pMem = m_Adapter.AlignSharedMemory(&MemPhys, ALIGN_16, ShMem);
	m_StatsCounters = reinterpret_cast<PERR_COUNT_STRUC>(pMem);
	m_StatsCounterPhys = MemPhys;
	MemPhys += sizeof(ERR_COUNT_STRUC);

	TRACE("m_SelfTest     =%p\n", m_SelfTest);
	TRACE("m_NonTxCmdBlock=%p\n", m_NonTxCmdBlock);
	TRACE("m_DumpSpace    =%p\n", m_DumpSpace);
	TRACE("m_StatsCounters=%p\n", m_StatsCounters);

	return MemPhys;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::SetPacketFilter
//
//
// Parameters:
//		NewPacketFilter
// IRQL:
//
// Return Mode:
//
// NOTE:
//
NDIS_STATUS	E100bexCard::SetPacketFilter(ULONG PacketFilter)
{
	TRACE("E100bexCard::SetPacketFilter++\n");

	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

	// Need to enable or disable broadcast and promiscuous support depending
	// on the new filter
	UCHAR NewParameterField = CB_557_CFIG_DEFAULT_PARM15;

	if (PacketFilter & NDIS_PACKET_TYPE_BROADCAST)
	{
		NewParameterField &= ~CB_CFIG_BROADCAST_DIS;
	}
	else
	{
		NewParameterField |= CB_CFIG_BROADCAST_DIS;
	}

	if (PacketFilter & NDIS_PACKET_TYPE_PROMISCUOUS)
	{
		NewParameterField |= CB_CFIG_PROMISCUOUS;
	}
	else
	{
		NewParameterField &= ~CB_CFIG_PROMISCUOUS;
	}

	do
	{
		// Only need to do something to the HW if the filter bits have changed.
		if ((m_OldParameterField == NewParameterField ) &&
			!(PacketFilter & NDIS_PACKET_TYPE_ALL_MULTICAST))
		{
			break;
		}

		TRACE("Filter Type %08x changed -- doing re-config\n", PacketFilter);

		m_OldParameterField = NewParameterField;
		m_NonTxCmdBlockHdr->CbCommand = CB_CONFIGURE;
		m_NonTxCmdBlockHdr->CbStatus = 0;
		m_NonTxCmdBlockHdr->CbLinkPointer = DRIVER_NULL;

		// First fill in the static (end user can't change) config bytes
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[0] = CB_557_CFIG_DEFAULT_PARM0;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[2] = CB_557_CFIG_DEFAULT_PARM2;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[3] = CB_557_CFIG_DEFAULT_PARM3;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[9] = CB_557_CFIG_DEFAULT_PARM9;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[10] = CB_557_CFIG_DEFAULT_PARM10;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[11] = CB_557_CFIG_DEFAULT_PARM11;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[12] = CB_557_CFIG_DEFAULT_PARM12;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[13] = CB_557_CFIG_DEFAULT_PARM13;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[14] = CB_557_CFIG_DEFAULT_PARM14;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[15] = CB_557_CFIG_DEFAULT_PARM15;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[16] = CB_557_CFIG_DEFAULT_PARM16;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[17] = CB_557_CFIG_DEFAULT_PARM17;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[18] = CB_557_CFIG_DEFAULT_PARM18;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[20] = CB_557_CFIG_DEFAULT_PARM20;
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[21] = CB_557_CFIG_DEFAULT_PARM21;

		// Set the Tx underrun retries
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[7] =
			(UCHAR) (CB_557_CFIG_DEFAULT_PARM7 | (m_Ai.AiUnderrunRetry << 1));

		// Set the Tx and Rx Fifo limits
		m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[1] =
			(UCHAR) ((m_Ai.AiTxFifo << 4) | m_Ai.AiRxFifo);

		// set the MWI enable bit if needed
		if (m_Ai.MWIEnable)
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[3] |= CB_CFIG_B3_MWI_ENABLE;

		// Set the Tx and Rx DMA maximum byte count fields.
		if ((m_Ai.AiRxDmaCount) || (m_Ai.AiTxDmaCount))
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[4] =
				(UCHAR) m_Ai.AiRxDmaCount;
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[5] =
				(UCHAR) (m_Ai.AiTxDmaCount | CB_CFIG_DMBC_EN);
		}
		else
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[4] =
				CB_557_CFIG_DEFAULT_PARM4;
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[5] =
				CB_557_CFIG_DEFAULT_PARM5;
		}

		// Setup for MII or 503 operation.  The CRS+CDT bit should only be
		// set when operating in 503 mode.
		if (m_Phy.GetPhyAddress() == 32)
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[8] =
				(CB_557_CFIG_DEFAULT_PARM8 & (~CB_CFIG_503_MII));
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[15] =
				(UCHAR) (NewParameterField | CB_CFIG_CRS_OR_CDT);
		}
		else
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[8] =
				(CB_557_CFIG_DEFAULT_PARM8 | CB_CFIG_503_MII);
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[15] =
				(UCHAR) (NewParameterField & (~CB_CFIG_CRS_OR_CDT));
		}

		// Setup Full duplex stuff

		// If forced to half duplex
		if (m_Ai.AiForceDpx == 1)
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[19] =
				(CB_557_CFIG_DEFAULT_PARM19 &
				(~(CB_CFIG_FORCE_FDX| CB_CFIG_FDX_ENABLE)));
		}
		// If forced to full duplex
		else if (m_Ai.AiForceDpx == 2)
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[19] =
				(CB_557_CFIG_DEFAULT_PARM19 | CB_CFIG_FORCE_FDX);
		}
		// If auto-duplex
		else
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[19] =
				CB_557_CFIG_DEFAULT_PARM19;
		}

		// if multicast all is being turned on, set the bit
		if (PacketFilter & NDIS_PACKET_TYPE_ALL_MULTICAST)
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[21] =
				(CB_557_CFIG_DEFAULT_PARM21 | CB_CFIG_MULTICAST_ALL);
		}
		else
		{
			m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[21] =
				CB_557_CFIG_DEFAULT_PARM21;
		}

		// Wait for the SCB to clear before we check the CU status.
		if (!WaitScb())
		{
			Status = NDIS_STATUS_HARD_ERRORS;
			break;
		}

		// If any transmits have been issued , then the CU will either be active,
		// or in the suspended state.  If the CU is active, then wait for
		// it to be suspended.
		if (m_Adapter.m_TransmitIdle == FALSE)
		{
			// Wait for suspended state
			for (UINT i = 5000*50; i != 0; i--)
			{
				if (( USHORT(m_CSRAddress.ScbStatus) & SCB_CUS_MASK) != SCB_CUS_ACTIVE)
					break;
				NdisStallExecution(20);
			}
			if (!i)
			{
				TRACE("HARDWARE_NOT_RESPONDING\n");
				Status = NDIS_STATUS_HARD_ERRORS;
				break;
			}

			// Check the current status of the receive unit
			if ((USHORT(m_CSRAddress.ScbStatus) & SCB_RUS_MASK) != SCB_RUS_IDLE)
			{
				// Issue an RU abort.  Since an interrupt will be issued, the
				// RU will be started by the DPC.
				Status = IssueScbCommand(SCB_RUC_ABORT, TRUE);
				if (Status != NDIS_STATUS_SUCCESS)
				{
					break;
				}
			}

			if (!WaitScb())
			{
				Status = NDIS_STATUS_HARD_ERRORS;
				break;
			}

			// Restore the transmit software flags.  After the multicast
			// command is issued, the command unit will be idle, because the
			// EL bit will be set in the multicast commmand block.
			m_Adapter.m_TransmitIdle = TRUE;
			m_Adapter.m_ResumeWait = TRUE;
		}

		// Display config info
		TRACE("Re-Issuing Configure command for filter change\n");
		TRACE("Config Block at virt addr %p, phys address %x\n", &m_NonTxCmdBlockHdr->CbStatus, m_NonTxCmdBlockPhys);

		for (UINT i=0; i < CB_CFIG_BYTE_COUNT; i++)
		{
			TRACE("  Config byte %x = %.2x\n", i, m_NonTxCmdBlock->NonTxCb.Config.ConfigBytes[i]);
		}

		// Submit the configure command to the chip, and wait for it to complete.
		m_CSRAddress.ScbGeneralPointer = m_NonTxCmdBlockPhys;
		Status = SubmitCommandBlockAndWait();
		if (Status != NDIS_STATUS_SUCCESS)
		{
			Status = NDIS_STATUS_NOT_ACCEPTED;
		}
	} while (FALSE);

	if(Status == NDIS_STATUS_SUCCESS)
		m_PacketFilter = PacketFilter;
	TRACE("E100bexCard::SetPacketFilter--, Status=%x\n", Status);
	return Status;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::AbortReceiveUnit
//
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// NOTE:
//
VOID E100bexCard::AbortReceiveUnit()
{
	IssueScbCommand(SCB_RUC_ABORT, TRUE);
}

////////////////////////////////////////////////////////////////////
// E100bexCard::DumpStatsCounters
//
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// NOTE:
//
VOID E100bexCard::DumpStatsCounters(KNdisStats<ERR_COUNT_STRUC>& Stats)
{
	TRACE("E100bexCard::DumpStatsCounters\n");

	// The query is for a driver statistic, so we need to first
	// update our statistics in software.

	// clear the dump counters complete DWORD
	m_StatsCounters->CommandComplete = 0;
	m_Adapter.m_Lock.Lock();

	// Dump and reset the hardware's statistic counters
	IssueScbCommand(SCB_CUC_DUMP_RST_STAT, TRUE);

	// Restore the resume transmit software flag.  After the dump counters
	// command is issued, we should do a WaitSCB before issuing the next send.
	m_Adapter.m_ResumeWait = TRUE;
	m_Adapter.m_Lock.Unlock();

	// Now wait for the dump/reset to complete
	for (UINT i=100000; i != 0; i--)
	{
		if (m_StatsCounters->CommandComplete == 0xA007)
			break;
		NdisStallExecution(20);
	}

	if (!i)
	{
		TRACE("DumpStatsCounters HARDWARE_NOT_RESPONDING\n");
		return;
	}

	// Output the debug counters to the debug terminal.
	TRACE3(("Good Transmits %d\n", m_StatsCounters->XmtGoodFrames));
	TRACE3(("Good Transmits %d\n", m_StatsCounters->XmtGoodFrames));
	TRACE3(("Good Receives %d\n", m_StatsCounters->RcvGoodFrames));
	TRACE3(("Max Collisions %d\n", m_StatsCounters->XmtMaxCollisions));
	TRACE3(("Late Collisions %d\n", m_StatsCounters->XmtLateCollisions));
	TRACE3(("Transmit Underruns %d\n", m_StatsCounters->XmtUnderruns));
	TRACE3(("Transmit Lost CRS %d\n", m_StatsCounters->XmtLostCRS));
	TRACE3(("Transmits Deferred %d\n", m_StatsCounters->XmtDeferred));
	TRACE3(("One Collision xmits %d\n", m_StatsCounters->XmtSingleCollision));
	TRACE3(("Mult Collision xmits %d\n", m_StatsCounters->XmtMultCollisions));
	TRACE3(("Total Collisions %d\n", m_StatsCounters->XmtTotalCollisions));

	TRACE3(("Receive CRC errors %d\n", m_StatsCounters->RcvCrcErrors));
	TRACE3(("Receive Alignment errors %d\n", m_StatsCounters->RcvAlignmentErrors));
	TRACE3(("Receive no resources %d\n", m_StatsCounters->RcvResourceErrors));
	TRACE3(("Receive overrun errors %d\n", m_StatsCounters->RcvOverrunErrors));
	TRACE3(("Receive CDT errors %d\n", m_StatsCounters->RcvCdtErrors));
	TRACE3(("Receive short frames %d\n", m_StatsCounters->RcvShortFrames));

	// update packet counts
	Stats->XmtGoodFrames += m_StatsCounters->XmtGoodFrames;
	Stats->RcvGoodFrames += m_StatsCounters->RcvGoodFrames;

	// update transmit error counts
	Stats->XmtMaxCollisions += m_StatsCounters->XmtMaxCollisions;
	Stats->XmtLateCollisions += m_StatsCounters->XmtLateCollisions;
	Stats->XmtUnderruns += m_StatsCounters->XmtUnderruns;
	Stats->XmtLostCRS += m_StatsCounters->XmtLostCRS;
	Stats->XmtDeferred += m_StatsCounters->XmtDeferred;
	Stats->XmtSingleCollision += m_StatsCounters->XmtSingleCollision;
	Stats->XmtMultCollisions += m_StatsCounters->XmtMultCollisions;
	Stats->XmtTotalCollisions += m_StatsCounters->XmtTotalCollisions;

	// update receive error counts
	Stats->RcvCrcErrors += m_StatsCounters->RcvCrcErrors;
	Stats->RcvAlignmentErrors += m_StatsCounters->RcvAlignmentErrors;
	Stats->RcvResourceErrors += m_StatsCounters->RcvResourceErrors;
	Stats->RcvOverrunErrors += m_StatsCounters->RcvOverrunErrors;
	Stats->RcvCdtErrors += m_StatsCounters->RcvCdtErrors;
	Stats->RcvShortFrames += m_StatsCounters->RcvShortFrames;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::NICSetMulticastList
//
//
// Parameters:
//		PrivateMulticastBuffer
//		nAddressCount
// IRQL:
//
// Return Mode:
//
// NOTE:
//
NDIS_STATUS E100bexCard::NICSetMulticastList(PUCHAR PrivateMulticastBuffer, UINT nAddressCount)
{
	TRACE("E100bexCard::NICSetMulticastList\n");
	NDIS_STATUS Status = NDIS_STATUS_FAILURE;

	if ( NULL == PrivateMulticastBuffer )
	{
		ASSERT( PrivateMulticastBuffer );
		return Status;
	}

	TRACE1(("Configuring for %x mc addresses\n", nAddressCount))

	// Holds the change that should be returned to the filtering package.
	PUCHAR McAddress;

	// Setup the command block for the multicast command.
	for (UINT i = 0;(i < nAddressCount) && (i < NIC_MAX_MCAST_LIST);i++)
	{
		TRACE1((" mc %d =%.2x %.2x %.2x %.2x %.2x %.2x\n", i,
			PrivateMulticastBuffer[i*ETH_LENGTH_OF_ADDRESS + 0],
			PrivateMulticastBuffer[i*ETH_LENGTH_OF_ADDRESS + 1],
			PrivateMulticastBuffer[i*ETH_LENGTH_OF_ADDRESS + 2],
			PrivateMulticastBuffer[i*ETH_LENGTH_OF_ADDRESS + 3],
			PrivateMulticastBuffer[i*ETH_LENGTH_OF_ADDRESS + 4],
			PrivateMulticastBuffer[i*ETH_LENGTH_OF_ADDRESS + 5]
			));

		McAddress = &m_NonTxCmdBlock->NonTxCb.Multicast.McAddress[i*ETHERNET_ADDRESS_LENGTH];

		for (UINT j = 0; j < ETH_LENGTH_OF_ADDRESS; j++)
		{
			*(McAddress++) = PrivateMulticastBuffer[i*ETH_LENGTH_OF_ADDRESS + j];
		}
	}


	m_NonTxCmdBlock->NonTxCb.Multicast.McCount = (USHORT) (nAddressCount * ETH_LENGTH_OF_ADDRESS);
	m_NonTxCmdBlockHdr->CbStatus = 0;
	m_NonTxCmdBlockHdr->CbCommand = CB_MULTICAST;

	// Wait for the SCB to clear before we check the CU status.
	if (!WaitScb())
		return NDIS_STATUS_HARD_ERRORS;

	// If we have issued any transmits, then the CU will either be active, or
	// in the suspended state.  If the CU is active, then we wait for it to be
	// suspended.
	if (m_Adapter.m_TransmitIdle == FALSE)
	{
		// Wait for suspended state
		for (UINT i = 200000; i != 0; i--)
		{
			if (( USHORT(m_CSRAddress.ScbStatus) & SCB_CUS_MASK) != SCB_CUS_ACTIVE)
				break;
			NdisStallExecution(20);
		}
		if (!i)
		{
			TRACE("HARDWARE_NOT_RESPONDING\n");
			Status = NDIS_STATUS_HARD_ERRORS;
		}

		// Restore the transmit software flags.  After the multicast command is
		// issued, the command unit will be idle, because the EL bit will be
		// set in the multicast commmand block.
		m_Adapter.m_TransmitIdle = TRUE;
		m_Adapter.m_ResumeWait = TRUE;
	}

	// Update the command list pointer.
	m_CSRAddress.ScbGeneralPointer = m_NonTxCmdBlockPhys;

	// Submit the multicast command to the adapter and wait for it to complete.
	Status = SubmitCommandBlockAndWait();
	if (Status != NDIS_STATUS_SUCCESS)
	{
		Status = NDIS_STATUS_NOT_ACCEPTED;
	}
	return Status;
}

////////////////////////////////////////////////////////////////////
// E100bexCard::StartTransmitUnit
//
//
// Parameters:
//		TcbPhys
// IRQL:
//
// Return Mode:
//
// NOTE:
//
NDIS_STATUS E100bexCard::StartTransmitUnit(ULONG TcbPhys)
{
	// Wait for the SCB to clear before we set the general pointer
	if (!WaitScb())
		return NDIS_STATUS_HARD_ERRORS;

	// Don't try to start the transmitter if the command unit is not
	// idle ((not idle) == (Cu-Suspended or Cu-Active)).
	if (( USHORT(m_CSRAddress.ScbStatus) & SCB_CUS_MASK) != SCB_CUS_IDLE)
	{
		TRACE("Adapter = %p, CU Not IDLE\n", *this);
		//MP_SET_HARDWARE_ERROR(Adapter);
		ASSERT(0);
		NdisStallExecution(25);
	}

	m_CSRAddress.ScbGeneralPointer = TcbPhys;

	return IssueScbCommand(SCB_CUC_START, FALSE);
}

////////////////////////////////////////////////////////////////////
// E100bexCard::ResumeCommandUnit
//
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// NOTE:
//
NDIS_STATUS E100bexCard::ResumeCommandUnit()
{
	// Issue a CU-Resume command to the device.  We only need to do a
	// WaitScb if the last command was NOT a RESUME.
	return IssueScbCommand( SCB_CUC_RESUME, m_Adapter.m_ResumeWait);
}

////////////////////////////////////////////////////////////////////
// E100bexCard::IssueSelectiveReset
//
//
// Parameters:
//		None
// IRQL:
//
// Return Mode:
//
// NOTE:
//
VOID E100bexCard::IssueSelectiveReset()
{
	TRACE2(("Entered E100bexCard::IssueSelectiveReset\n"))

	// Wait for the SCB to clear before we check the CU status.
	WaitScb();

	// If we have issued any transmits, then the CU will either be active, or
	// in the suspended state.  If the CU is active, then we wait for it to be
	// suspended.  If the the CU is suspended, then we need to put the CU back
	// into the idle state by issuing a selective reset.
	if (m_Adapter.m_TransmitIdle == FALSE)
	{
		// Wait up to 2 seconds for suspended state
		for (UINT i=1000; (i != 0) && ( USHORT(m_CSRAddress.ScbStatus) & SCB_CUS_MASK) == SCB_CUS_ACTIVE; i--)
			//        while ((Adapter->CSRAddress->ScbStatus & SCB_CUS_MASK) == SCB_CUS_ACTIVE)
		{
			TRACE("CU active -- wait for it to suspend. ScbStatus=%04x\n", m_CSRAddress.ScbStatus );
			NdisStallExecution(20);
		}

		if (!i)
		{
			TRACE("HARDWARE_NOT_RESPONDING\n");
			return;
		}

		// Check the current status of the receive unit
		if (( USHORT(m_CSRAddress.ScbStatus) & SCB_RUS_MASK) != SCB_RUS_IDLE)
		{
			// Issue an RU abort.  Since an interrupt will be issued, the
			// RU will be started by the DPC.
			IssueScbCommand(SCB_RUC_ABORT, TRUE);
		}

		// Issue a selective reset.
		TRACE("CU suspended. ScbStatus=%04x Issue selective reset\n", m_CSRAddress.ScbStatus);
		m_CSRAddress.Port = (USHORT)PORT_SELECTIVE_RESET;

		// stall 20 us (only need 10) after a port sel-reset command
		NdisStallExecution(20);

		for (i=100; i != 0; i--)
		{
			if ( ULONG(m_CSRAddress.Port) == 0)
				break;
			NdisStallExecution(10);
		}
		if (!i)
		{
			TRACE("HARDWARE_NOT_RESPONDING\n");
			return;
		}

		// disable interrupts after issuing reset, because the int
		// line gets raised when reset completes.
		DisableInterrupt();

		// Restore the transmit software flags.
		m_Adapter.m_TransmitIdle = TRUE;
		m_Adapter.m_ResumeWait = TRUE;
	}
}
