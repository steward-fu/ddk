// kplx9056.h - PLX9056 register definitions
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

#ifndef __KPLX9056REG_H__
#define __KPLX9056REG_H__

/////////////////////////////////////////////////////////////////////////////////////
// PCI CONFIGURATION REGISTERS
//

// PCI Configuration ID
#define PLX9056_PCIIDR					ULONG(0x0000)
#define PLX9056_PCIIDR_VID(_r_)			USHORT((_r_) & 0xFFFF)
#define PLX9056_PCIIDR_DID(_r_)			USHORT(((_r_) >> 16) & 0xFFFF)

// PCI Command
#define PLX9056_PCICR					ULONG(0x0004)
#define PLX9056_PCICR_IOS				USHORT(1 << 0)
#define PLX9056_PCICR_MS				USHORT(1 << 1)
#define PLX9056_PCICR_ME				USHORT(1 << 2)
#define PLX9056_PCICR_SC				USHORT(1 << 3)
#define PLX9056_PCICR_MWIE				USHORT(1 << 4)
#define PLX9056_PCICR_VGAPS				USHORT(1 << 5)
#define PLX9056_PCICR_PER				USHORT(1 << 6)
#define PLX9056_PCICR_WCC				USHORT(1 << 7)
#define PLX9056_PCICR_SERRE				USHORT(1 << 8)
#define PLX9056_PCICR_FBTBE				USHORT(1 << 9)

// PCI Status
#define PLX9056_PCISR					ULONG(0x0006)
#define PLX9056_PCISR_NCFS				USHORT(1 << 4)
#define PLX9056_PCISR_66MHZC			USHORT(1 << 5)
#define PLX9056_PCISR_UDF				USHORT(1 << 6)
#define PLX9056_PCISR_FBTBC				USHORT(1 << 7)
#define PLX9056_PCISR_MDPED				USHORT(1 << 8)
#define PLX9056_PCISR_DEVSELT			USHORT(3 << 9)
#define PLX9056_PCISR_TA				USHORT(1 << 11)
#define PLX9056_PCISR_RTA				USHORT(1 << 12)
#define PLX9056_PCISR_RMA				USHORT(1 << 13)
#define PLX9056_PCISR_SSE				USHORT(1 << 14)
#define PLX9056_PCISR_DPE				USHORT(1 << 15)

// PCI Revision ID
#define PLX9056_PCIREV					ULONG(0x0008)

// PCI Class Code
#define PLX9056_PCICCR					ULONG(0x0009)
#define PLX9056_PCICCR_RLPI(_r_)		UCHAR((_r_) & 0xFF)
#define PLX9056_PCICCR_SC(_r_)			UCHAR(((_r_) >> 8) & 0xFF)
#define PLX9056_PCICCR_BCC(_r_)			UCHAR(((_r_) >> 16) & 0xFF)

// PCI Cache Line Size
#define PLX9056_PCICLSR					ULONG(0x000C)

// PCI Bus Latency Timer
#define PLX9056_PCILTR					ULONG(0x000D)

// PCI Header Type
#define PLX9056_PCIHTR					ULONG(0x000E)
#define PLX9056_PCIHTR_CLT				UCHAR(0x7F)
#define PLX9056_PCIHTR_HT				UCHAR(1 << 7)

// PCI Built-In Self Test
#define PLX9056_PCIBISTR				ULONG(0x000F)
#define PLX9056_PCIBISTR_PF				UCHAR(0x0F)
#define PLX9056_PCIBISTR_IE				UCHAR(1 << 6)
#define PLX9056_PCIBISTR_S				UCHAR(1 << 7)

// PCI Base Address Register for Memory Accesses to Local, Runtime, and DMA
#define PLX9056_PCIBAR0					ULONG(0x0010)
#define PLX9056_PCIBAR0_MSI				ULONG(1 << 0)
#define PLX9056_PCIBAR0_RL				ULONG(3 << 1)
#define PLX9056_PCIBAR0_P				ULONG(1 << 3)
#define PLX9056_PCIBAR0_MBA				ULONG(0xFFFFFFF0)

// PCI Base Address Register for I/O Accesses to Local, Runtime, and DMA
#define PLX9056_PCIBAR1					ULONG(0x0014)
#define PLX9056_PCIBAR1_MSI				ULONG(1 << 0)
#define PLX9056_PCIBAR1_IOBA			ULONG(0xFFFFFFFC)

// PCI Base Address Register for Memory Accesses to Local Address Space 0
#define PLX9056_PCIBAR2					ULONG(0x0018)
#define PLX9056_PCIBAR2_MSI				ULONG(1 << 0)
#define PLX9056_PCIBAR2_RL				ULONG(3 << 1)
#define PLX9056_PCIBAR2_P				ULONG(1 << 3)
#define PLX9056_PCIBAR2_MBA				ULONG(0xFFFFFFF0)

// PCI Base Address Register for Memory Accesses to Local Address Space 1
#define PLX9056_PCIBAR3					ULONG(0x001C)
#define PLX9056_PCIBAR3_MSI				ULONG(1 << 0)
#define PLX9056_PCIBAR3_RL				ULONG(3 << 1)
#define PLX9056_PCIBAR3_P				ULONG(1 << 3)
#define PLX9056_PCIBAR3_MBA				ULONG(0xFFFFFFF0)

// PCI Base Address
#define PLX9056_PCIBAR4					ULONG(0x0020)

// PCI Base Address
#define PLX9056_PCIBAR5					ULONG(0x0024)

// PCI Cardbus CIS Pointer
#define PLX9056_PCICIS					ULONG(0x0028)

// PCI Subsystem Vendor ID
#define PLX9056_PCISVID					ULONG(0x002C)

// PCI Subsystem ID
#define PLX9056_PCISID					ULONG(0x002E)

// PCI Expansion ROM Base
#define PLX9056_PCIERBAR				ULONG(0x0030)
#define PLX9056_PCIERBAR_ADE			ULONG(1 << 0)
#define PLX9056_PCIERBAR_ERBA			ULONG(0xFFFFF800)

// New Capability Pointer
#define PLX9056_CAP_PTR					ULONG(0x0034)
#define PLX9056_CAP_PTR_NCP				ULONG(0xFC)

// PCI Interrupt Line
#define PLX9056_PCIILR					ULONG(0x003C)

// PCI Interrupt Pin
#define PLX9056_PCIIPR					ULONG(0x003D)

// PCI Min_Gnt
#define PLX9056_PCIMGR					ULONG(0x003E)

// PCI Max_Lat
#define PLX9056_PCIMLR					ULONG(0x003F)

// Power Management Capability ID
#define PLX9056_PMCAPID					ULONG(0x0040)

// Power Management Next Capability Pointer
#define PLX9056_PMNEXT					ULONG(0x0041)
#define PLX9056_PMNEXT_NCP				UCHAR(0xFC)

// Power Management Capabilities
#define PLX9056_PMC						ULONG(0x0042)
#define PLX9056_PMC_V					USHORT(3 << 0)
#define PLX9056_PMC_CRPMES				USHORT(1 << 3)
#define PLX9056_PMC_DSI					USHORT(1 << 5)
#define PLX9056_PMC_AUXC				USHORT(7 << 6)
#define PLX9056_PMC_D1S					USHORT(1 << 9)
#define PLX9056_PMC_D2S					USHORT(1 << 10)
#define PLX9056_PMC_PMES				USHORT(0xF800)

// Power Management Control/Status
#define PLX9056_PMCSR					ULONG(0x0044)
#define PLX9056_PMCSR_PS				USHORT(3 << 0)
#define PLX9056_PMCSR_PMEEN				USHORT(1 << 8)
#define PLX9056_PMCSR_DSEL				USHORT(0xF << 9)
#define PLX9056_PMCSR_DSC				USHORT(3 << 13)
#define PLX9056_PMCSR_PMES				USHORT(1 << 15)

// PMCSR Bridge Support Extensions
#define PLX9056_PMCSR_BSE				ULONG(0x0046)

// Power Management Data
#define PLX9056_PMDATA					ULONG(0x0047)

// Hot Swap Control
#define PLX9056_HS_CNTL					ULONG(0x0048)

// Hot Swap Next Capability Pointer
#define PLX9056_HS_NEXT					ULONG(0x0049)
#define PLX9056_HS_NEXT_NCP				UCHAR(0xFC)

// Hot Swap Control/Status
#define PLX9056_HS_CSR					ULONG(0x004A)
#define PLX9056_HS_CSR_ENUMIC			USHORT(1 << 1)
#define PLX9056_HS_CSR_LEDSS			USHORT(1 << 3)
#define PLX9056_HS_CSR_PI				USHORT(3 << 4)
#define PLX9056_HS_CSR_BRESI			USHORT(1 << 6)
#define PLX9056_HS_CSR_BIESI			USHORT(1 << 7)

// PCI Vital Product Data Control
#define PLX9056_PVPDCNTL				ULONG(0x004C)

// PCI Vital Product Data Next Capability Pointer
#define PLX9056_PVPD_NEXT				ULONG(0x004D)
#define PLX9056_PVPD_NEXT_NCP			UCHAR(0xFC)

// PCI Vital Product Data Address
#define PLX9056_PVPDAD					ULONG(0x004E)
#define PLX9056_PVPDAD_VPDA				USHORT(0x7FFF)
#define PLX9056_PVPDAD_F				USHORT(1 << 15)

// PCI VPD Data
#define PLX9056_PVPDATA					ULONG(0x0050)


/////////////////////////////////////////////////////////////////////////////////////
// LOCAL CONFIGURATION REGISTERS
//

// Local Address Space 0 Range Register for PCI-to-Local Bus
#define PLX9056_LAS0RR					ULONG(0x0000)
#define PLX9056_LAS0RR_MSI				ULONG(1 << 0)
#define PLX9056_LAS0RR_RL				ULONG(3 << 1)
#define PLX9056_LAS0RR_P				ULONG(1 << 3)
#define PLX9056_LAS0RR_R				ULONG(0xFFFFFFF0)

// Local Address Space 0 Local Base Address (Remap)
#define PLX9056_LAS0BA					ULONG(0x0004)
#define PLX9056_LAS0BA_E				ULONG(1 << 0)
#define PLX9056_LAS0BA_R				ULONG(0xFFFFFFF0)

// Mode/DMA Arbitration
#define PLX9056_MARBR					ULONG(0x0008)
#define PLX9056_MARBR_LBLT				ULONG(0xFF)
#define PLX9056_MARBR_LBPT				ULONG(0xFF00)
#define PLX9056_MARBR_LBLTE				ULONG(1 << 16)
#define PLX9056_MARBR_LBPTE				ULONG(1 << 17)
#define PLX9056_MARBR_LBBREQE			ULONG(1 << 18)
#define PLX9056_MARBR_DMACP				ULONG(3 << 19)
#define PLX9056_MARBR_LBDSRBM			ULONG(1 << 21)
#define PLX9056_MARBR_DSLE				ULONG(1 << 22)
#define PLX9056_MARBR_PCIRM				ULONG(1 << 23)
#define PLX9056_MARBR_DRM				ULONG(1 << 24)
#define PLX9056_MARBR_PCIRNWM			ULONG(1 << 25)
#define PLX9056_MARBR_PCIRWFM			ULONG(1 << 26)
#define PLX9056_MARBR_GLBLTBREQI		ULONG(1 << 27)
#define PLX9056_MARBR_PCIRNFM			ULONG(1 << 28)
#define PLX9056_MARBR_PCIVD				ULONG(1 << 29)
#define PLX9056_MARBR_FIFOFSF			ULONG(1 << 30)
#define PLX9056_MARBR_BIGENDWIOS		ULONG(1 << 31)

// Big/Little Endian Descriptor
#define PLX9056_BIGEND					ULONG(0x000C)
#define PLX9056_BIGEND_CRBEM			UCHAR(1 << 0)
#define PLX9056_BIGEND_DMBEM			UCHAR(1 << 1)
#define PLX9056_BIGEND_DSAS0BEM			UCHAR(1 << 2)
#define PLX9056_BIGEND_DSAER0BEM		UCHAR(1 << 3)
#define PLX9056_BIGEND_BEBLM			UCHAR(1 << 4)
#define PLX9056_BIGEND_DSAS1BEM			UCHAR(1 << 5)
#define PLX9056_BIGEND_DMA1BEM			UCHAR(1 << 6)
#define PLX9056_BIGEND_DMA0BEM			UCHAR(1 << 7)

// Local Miscellaneous Control 1
#define PLX9056_LMISC1					ULONG(0x000D)
#define PLX9056_LMISC1_BAR1E			UCHAR(1 << 0)
#define PLX9056_LMISC1_BAR1S			UCHAR(1 << 1)
#define PLX9056_LMISC1_LIS				UCHAR(1 << 2)
#define PLX9056_LMISC1_DMWFIFOF			UCHAR(1 << 3)
#define PLX9056_LMISC1_DMDRE			UCHAR(1 << 4)
#define PLX9056_LMISC1_TEAIIM			UCHAR(1 << 5)
#define PLX9056_LMISC1_DMWFIFOAF		UCHAR(1 << 6)
#define PLX9056_LMISC1_DFRFIFO			UCHAR(1 << 7)

// Serial EEPROM Write-Protected Address Boundary
#define PLX9056_PROT_AREA				ULONG(0x000E)

// Local Miscellaneous Control 2
#define PLX9056_LMISC2					ULONG(0x000F)
#define PLX9056_LMISC2_RTE				UCHAR(1 << 0)
#define PLX9056_LMISC2_RTS				UCHAR(1 << 1)
#define PLX9056_LMISC2_DSWD				UCHAR(7 << 2)
#define PLX9056_LMISC2_DSWFFC			UCHAR(1 << 5)

// Expansion ROM Range
#define PLX9056_EROMRR					ULONG(0x0010)
#define PLX9056_EROMRR_ADE				ULONG(1 << 0)
#define PLX9056_EROMRR_R				ULONG(0xFFFFF800)

// Expansion ROM Local Base Address (Remap) and BREQo Control
#define PLX9056_EROMBA					ULONG(0x0014)
#define PLX9056_EROMBA_RSADC			ULONG(0xF)
#define PLX9056_EROMBA_LBBE				ULONG(1 << 4)
#define PLX9056_EROMBA_BTR				ULONG(1 << 5)
#define PLX9056_EROMBA_R				ULONG(0xFFFFF800)

// Local Address Space0/Expansion ROM Bus Region Descriptor
#define PLX9056_LBRD0					ULONG(0x0018)
#define PLX9056_LBRD0_MSLBW				ULONG(3 << 0)
#define PLX9056_LBRD0_MSIWS				ULONG(0xF << 2)
#define PLX9056_LBRD0_MSTRIE			ULONG(1 << 6)
#define PLX9056_LBRD0_MSBIE				ULONG(1 << 7)
#define PLX9056_LBRD0_MSPD				ULONG(1 << 8)
#define PLX9056_LBRD0_ERSPD				ULONG(1 << 9)
#define PLX9056_LBRD0_PCE				ULONG(1 << 10)
#define PLX9056_LBRD0_PC				ULONG(0xF << 11)
#define PLX9056_LBRD0_ERSLBW			ULONG(3 << 16)
#define PLX9056_LBRD0_ERSIWS			ULONG(0xF << 18)
#define PLX9056_LBRD0_ERSTRIE			ULONG(1 << 22)
#define PLX9056_LBRD0_ERSBIE			ULONG(1 << 23)
#define PLX9056_LBRD0_MSBE				ULONG(1 << 24)
#define PLX9056_LBRD0_ELLSE				ULONG(1 << 25)
#define PLX9056_LBRD0_ERSBE				ULONG(1 << 26)
#define PLX9056_LBRD0_DSWM				ULONG(1 << 27)
#define PLX9056_LBRD0_DSRDC				ULONG(0xF << 28)

// Local Range Register for Direct Master-to-PCI
#define PLX9056_DMRR					ULONG(0x001C)
#define PLX9056_DMRR_R					ULONG(0xFFFF0000)

// Local Bus Base Address Register for Direct Master-to-PCI Memory
#define PLX9056_DMLBAM					ULONG(0x0020)
#define PLX9056_DMLBAM_R				ULONG(0xFFFF0000)

// Local Base Address Register for Direct Master-to-PCI I/O Configuration
#define PLX9056_DMLBAI					ULONG(0x0024)
#define PLX9056_DMLBAI_R				ULONG(0xFFFF0000)

// PCI Base Address (Remap) Register for Direct Master-to-PCI Memory
#define PLX9056_DMPBAM					ULONG(0x0028)
#define PLX9056_DMPBAM_MAE				ULONG(1 << 0)
#define PLX9056_DMPBAM_IOAE				ULONG(1 << 1)
#define PLX9056_DMPBAM_CE				ULONG(1 << 2)
#define PLX9056_DMPBAM_RM				ULONG(1 << 4)
#define PLX9056_DMPBAM_MWIM				ULONG(1 << 9)
#define PLX9056_DMPBAM_PL				ULONG(1 << 11)
#define PLX9056_DMPBAM_IORS				ULONG(1 << 13)
#define PLX9056_DMPBAM_WD				ULONG(3 << 14)
#define PLX9056_DMPBAM_R				ULONG(0xFFFF0000)

// PCI Configuration Address Register for Direct Master-to-PCI I/O Configuration
#define PLX9056_DMCFGA					ULONG(0x002C)
#define PLX9056_DMCFGA_CT				ULONG(3 << 0)
#define PLX9056_DMCFGA_RN				ULONG(0xFC)
#define PLX9056_DMCFGA_FN				ULONG(7 << 8)
#define PLX9056_DMCFGA_DN				ULONG(0xF800)
#define PLX9056_DMCFGA_BN				ULONG(0xFF << 16)
#define PLX9056_DMCFGA_CE				ULONG(1 << 31)

// Local Address Space 1 Range Register for PCI-to-Local Bus
#define PLX9056_LAS1RR					ULONG(0x00F0)
#define PLX9056_LAS1RR_MSI				ULONG(1 << 0)
#define PLX9056_LAS1RR_RL				ULONG(3 << 1)
#define PLX9056_LAS1RR_P				ULONG(1 << 3)
#define PLX9056_LAS1RR_R				ULONG(0xFFFFFFF0)

// Local Address Space 1 Local Base Address (Remap)
#define PLX9056_LAS1BA					ULONG(0x00F4)
#define PLX9056_LAS1BA_E				ULONG(1 << 0)
#define PLX9056_LAS1BA_R				ULONG(0xFFFFFFF0)

// Local Address Space1 Bus Region Descriptor
#define PLX9056_LBRD1					ULONG(0x00F8)
#define PLX9056_LBRD1_MSLBW				ULONG(3 << 0)
#define PLX9056_LBRD1_MSIWS				ULONG(0xF << 2)
#define PLX9056_LBRD1_MSTRIE			ULONG(1 << 6)
#define PLX9056_LBRD1_MSBIE				ULONG(1 << 7)
#define PLX9056_LBRD1_MSBE				ULONG(1 << 8)
#define PLX9056_LBRD1_MSPD				ULONG(1 << 9)
#define PLX9056_LBRD1_PCE				ULONG(1 << 10)
#define PLX9056_LBRD1_PC				ULONG(0xF << 11)

// Direct Master PCI Dual Address Cycle Upper Address
#define PLX9056_DMDAC					ULONG(0x00FC)

// PCI Arbiter Control
#define PLX9056_PCIARB					ULONG(0x0100)
#define PLX9056_PCIARB_AE				ULONG(1 << 0)
#define PLX9056_PCIARB_HP				ULONG(1 << 1)
#define PLX9056_PCIARB_EGR				ULONG(1 << 2)
#define PLX9056_PCIARB_AP				ULONG(1 << 3)

// PCI Abort Address
#define PLX9056_PABTADR					ULONG(0x0104)


/////////////////////////////////////////////////////////////////////////////////////
// RUNTIME REGISTERS
//

// Mailbox Register 0
#define PLX9056_MBOX0					ULONG(0x0078)

// Mailbox Register 1
#define PLX9056_MBOX1					ULONG(0x007C)

// Mailbox Register 2
#define PLX9056_MBOX2					ULONG(0x0048)

// Mailbox Register 3
#define PLX9056_MBOX3					ULONG(0x004C)

// Mailbox Register 4
#define PLX9056_MBOX4					ULONG(0x0050)

// Mailbox Register 5
#define PLX9056_MBOX5					ULONG(0x0054)

// Mailbox Register 6
#define PLX9056_MBOX6					ULONG(0x0058)

// Mailbox Register 7
#define PLX9056_MBOX7					ULONG(0x005C)

// PCI-to-Local Doorbell
#define PLX9056_P2LDBELL				ULONG(0x0060)

// Local-to-PCI Doorbell
#define PLX9056_L2PDBELL				ULONG(0x0064)

// Interrupt Control/Status
#define PLX9056_INTCSR					ULONG(0x0068)
#define PLX9056_INTCSR_ELBTL			ULONG(1 << 0)
#define PLX9056_INTCSR_ELBTLP			ULONG(1 << 1)
#define PLX9056_INTCSR_GSI				ULONG(1 << 2)
#define PLX9056_INTCSR_MIE				ULONG(1 << 3)
#define PLX9056_INTCSR_PMIE				ULONG(1 << 4)
#define PLX9056_INTCSR_PMI				ULONG(1 << 5)
#define PLX9056_INTCSR_DMWDSRPE			ULONG(1 << 6)
#define PLX9056_INTCSR_DMWDSRP			ULONG(1 << 7)
#define PLX9056_INTCSR_IE				ULONG(1 << 8)
#define PLX9056_INTCSR_DIE				ULONG(1 << 9)
#define PLX9056_INTCSR_AIE				ULONG(1 << 10)
#define PLX9056_INTCSR_LIIE				ULONG(1 << 11)
#define PLX9056_INTCSR_RAE				ULONG(1 << 12)
#define PLX9056_INTCSR_DIA				ULONG(1 << 13)
#define PLX9056_INTCSR_AIA				ULONG(1 << 14)
#define PLX9056_INTCSR_LIIA				ULONG(1 << 15)
#define PLX9056_INTCSR_LIOE				ULONG(1 << 16)
#define PLX9056_INTCSR_LDIE				ULONG(1 << 17)
#define PLX9056_INTCSR_DMA0IE			ULONG(1 << 18)
#define PLX9056_INTCSR_DMA1IE			ULONG(1 << 19)
#define PLX9056_INTCSR_LDIA				ULONG(1 << 20)
#define PLX9056_INTCSR_DMA0IA			ULONG(1 << 21)
#define PLX9056_INTCSR_DMA1IA			ULONG(1 << 22)
#define PLX9056_INTCSR_BISTIA			ULONG(1 << 23)
#define PLX9056_INTCSR_DMBM				ULONG(1 << 24)
#define PLX9056_INTCSR_DMA0BM			ULONG(1 << 25)
#define PLX9056_INTCSR_DMA1BM			ULONG(1 << 26)
#define PLX9056_INTCSR_TA256			ULONG(1 << 27)
#define PLX9056_INTCSR_MBOX0			ULONG(1 << 28)
#define PLX9056_INTCSR_MBOX1			ULONG(1 << 29)
#define PLX9056_INTCSR_MBOX2			ULONG(1 << 30)
#define PLX9056_INTCSR_MBOX3			ULONG(1 << 31)

// Serial EEPROM Control, PCI Command Codes, User I/O Control, and Init Control
#define PLX9056_CNTRL					ULONG(0x006C)
#define PLX9056_CNTRL_GPO				ULONG(1 << 16)
#define PLX9056_CNTRL_GPI				ULONG(1 << 17)
#define PLX9056_CNTRL_SELECT_USERI		ULONG(1 << 18)
#define PLX9056_CNTRL_SELECT_USERO		ULONG(1 << 19)
#define PLX9056_CNTRL_LINT_STATUS		ULONG(1 << 20)
#define PLX9056_CNTRL_LSERR_STATUS		ULONG(1 << 21)
#define PLX9056_CNTRL_CLOCK				ULONG(1 << 24)
#define PLX9056_CNTRL_CHIP_SELECT		ULONG(1 << 25)
#define PLX9056_CNTRL_WRITE_BIT			ULONG(1 << 26)
#define PLX9056_CNTRL_READ_BIT			ULONG(1 << 27)
#define PLX9056_CNTRL_PRESENT			ULONG(1 << 28)
#define PLX9056_CNTRL_RELOAD_CONFIG		ULONG(1 << 29)
#define PLX9056_CNTRL_RESET				ULONG(1 << 30)
#define PLX9056_CNTRL_EEDO_ENABLE		ULONG(1 << 31)

#define EE46_CMD_LEN					9
#define EE56_CMD_LEN					11
#define EE66_CMD_LEN					11
#define EE_READ							0x0180
#define EE_WRITE						0x0140
#define EE_WREN							0x0130
#define EE_WRALL						0x0110
#define EE_PRREAD						0x0180
#define EE_PRWRITE						0x0140
#define EE_WDS							0x0100
#define EE_PREN							0x0130
#define EE_PRCLEAR						0x01FF
#define EE_PRDS							0x0100

// PCI Hardwired Configuration ID
#define PLX9056_PCIHIDR					ULONG(0x0070)

// PCI Hardwired Revision ID
#define PLX9056_PCIHREV					ULONG(0x0074)


/////////////////////////////////////////////////////////////////////////////////////
// DMA REGISTERS
//

// DMA Channel 0 Mode
#define PLX9056_DMAMODE0				ULONG(0x0080)
#define PLX9056_DMAMODE0_LBW			ULONG(3 << 0)
#define PLX9056_DMAMODE0_IWS			ULONG(0xF << 2)
#define PLX9056_DMAMODE0_TARIE			ULONG(1 << 6)
#define PLX9056_DMAMODE0_BIE			ULONG(1 << 7)
#define PLX9056_DMAMODE0_LBE			ULONG(1 << 8)
#define PLX9056_DMAMODE0_SGM			ULONG(1 << 9)
#define PLX9056_DMAMODE0_DIE			ULONG(1 << 10)
#define PLX9056_DMAMODE0_LAM			ULONG(1 << 11)
#define PLX9056_DMAMODE0_DM				ULONG(1 << 12)
#define PLX9056_DMAMODE0_MWIM			ULONG(1 << 13)
#define PLX9056_DMAMODE0_EOTE			ULONG(1 << 14)
#define PLX9056_DMAMODE0_FSTMS			ULONG(1 << 15)
#define PLX9056_DMAMODE0_CCM			ULONG(1 << 16)
#define PLX9056_DMAMODE0_IS				ULONG(1 << 17)
#define PLX9056_DMAMODE0_DACCL			ULONG(1 << 18)
#define PLX9056_DMAMODE0_EOTEL			ULONG(1 << 19)
#define PLX9056_DMAMODE0_VME			ULONG(1 << 20)
#define PLX9056_DMAMODE0_VSC			ULONG(1 << 21)

// DMA Channel 0 PCI Address
#define PLX9056_DMAPADR0				ULONG(0x0084)

// DMA Channel 0 Local Address
#define PLX9056_DMALADR0				ULONG(0x0088)

// DMA Channel 0 Transfer Size
#define PLX9056_DMASIZ0					ULONG(0x008C)
#define PLX9056_DMASIZ0_S				ULONG(0x007FFFFF)
#define PLX9056_DMASIZ0_V				ULONG(1 << 31)

// DMA Channel 0 Descriptor Pointer
#define PLX9056_DMADPR0					ULONG(0x0090)
#define PLX9056_DMADPR0_DL				ULONG(1 << 0)
#define PLX9056_DMADPR0_EC				ULONG(1 << 1)
#define PLX9056_DMADPR0_ITC				ULONG(1 << 2)
#define PLX9056_DMADPR0_DT				ULONG(1 << 3)
#define PLX9056_DMADPR0_NDA				ULONG(0xFFFFFFF0)

// DMA Channel 1 Mode
#define PLX9056_DMAMODE1				ULONG(0x0094)
#define PLX9056_DMAMODE1_LBW			ULONG(3 << 0)
#define PLX9056_DMAMODE1_IWS			ULONG(0xF << 2)
#define PLX9056_DMAMODE1_TARIE			ULONG(1 << 6)
#define PLX9056_DMAMODE1_BIE			ULONG(1 << 7)
#define PLX9056_DMAMODE1_LBE			ULONG(1 << 8)
#define PLX9056_DMAMODE1_SGM			ULONG(1 << 9)
#define PLX9056_DMAMODE1_DIE			ULONG(1 << 10)
#define PLX9056_DMAMODE1_LAM			ULONG(1 << 11)
#define PLX9056_DMAMODE1_DM				ULONG(1 << 12)
#define PLX9056_DMAMODE1_MWIM			ULONG(1 << 13)
#define PLX9056_DMAMODE1_EOTE			ULONG(1 << 14)
#define PLX9056_DMAMODE1_FSTMS			ULONG(1 << 15)
#define PLX9056_DMAMODE1_CCM			ULONG(1 << 16)
#define PLX9056_DMAMODE1_IS				ULONG(1 << 17)
#define PLX9056_DMAMODE1_DACCL			ULONG(1 << 18)
#define PLX9056_DMAMODE1_EOTEL			ULONG(1 << 19)
#define PLX9056_DMAMODE1_VME			ULONG(1 << 20)
#define PLX9056_DMAMODE1_VSC			ULONG(1 << 21)

// DMA Channel 1 PCI Address
#define PLX9056_DMAPADR1				ULONG(0x0098)

// DMA Channel 1 Local Address
#define PLX9056_DMALADR1				ULONG(0x009C)

// DMA Channel 1 Transfer Size
#define PLX9056_DMASIZ1					ULONG(0x00A0)
#define PLX9056_DMASIZ1_S				ULONG(0x007FFFFF)
#define PLX9056_DMASIZ1_V				ULONG(1 << 31)

// DMA Channel 0 Descriptor Pointer
#define PLX9056_DMADPR1					ULONG(0x0090)
#define PLX9056_DMADPR1_DL				ULONG(1 << 0)
#define PLX9056_DMADPR1_EC				ULONG(1 << 1)
#define PLX9056_DMADPR1_ITC				ULONG(1 << 2)
#define PLX9056_DMADPR1_DT				ULONG(1 << 3)
#define PLX9056_DMADPR1_NDA				ULONG(0xFFFFFFF0)

// DMA Channel 0 Command/Status
#define PLX9056_DMACSR0					ULONG(0x00A8)
#define PLX9056_DMACSR0_E				UCHAR(1 << 0)
#define PLX9056_DMACSR0_S				UCHAR(1 << 1)
#define PLX9056_DMACSR0_A				UCHAR(1 << 2)
#define PLX9056_DMACSR0_CI				UCHAR(1 << 3)
#define PLX9056_DMACSR0_D				UCHAR(1 << 4)

// DMA Channel 1 Command/Status
#define PLX9056_DMACSR1					ULONG(0x00A9)
#define PLX9056_DMACSR1_E				UCHAR(1 << 0)
#define PLX9056_DMACSR1_S				UCHAR(1 << 1)
#define PLX9056_DMACSR1_A				UCHAR(1 << 2)
#define PLX9056_DMACSR1_CI				UCHAR(1 << 3)
#define PLX9056_DMACSR1_D				UCHAR(1 << 4)

// DMA Arbitration
#define PLX9056_DMAARB					ULONG(0x00AC)

// DMA Threshold
#define PLX9056_DMATHR					ULONG(0x00B0)

// DMA Channel 0 PCI Dual Address Cycle Upper Address
#define PLX9056_DMADAC0					ULONG(0x00B4)

// DMA Channel 1 PCI Dual Address Cycle Upper Address
#define PLX9056_DMADAC1					ULONG(0x00B8)

#include <pshpack4.h>
typedef struct _PLX9056_DESCRIPTOR
{
	ULONG		PciAddrLow;
	ULONG		LocalAddr;
	ULONG		Size;
	ULONG		NextDescPtr;
	ULONG		PciAddrHigh;
} PLX9056_DESCRIPTOR, *PPLX9056_DESCRIPTOR;
#include <poppack.h>

/////////////////////////////////////////////////////////////////////////////////////
// MESSAGING QUEUE REGISTERS
//

// Outbound Post Queue Interrupt Status
#define PLX9056_OPQIS					ULONG(0x0030)
#define PLX9056_OPQIS_I					ULONG(1 << 3)

// Outbound Post Queue Interrupt Mask
#define PLX9056_OPQIM					ULONG(0x0034)
#define PLX9056_OPQIM_M					ULONG(1 << 3)

// Inbound Queue Port
#define PLX9056_IQP						ULONG(0x0040)

// Outbound Queue Port
#define PLX9056_OQP						ULONG(0x0044)

// Messaging Queue Configuration
#define PLX9056_MQCR					ULONG(0x00C0)

// Queue Base Address
#define PLX9056_QBAR					ULONG(0x00C4)

// Inbound Free Head Pointer
#define PLX9056_IFHPR					ULONG(0x00C8)

// Inbound Free Tail Pointer
#define PLX9056_IFTPR					ULONG(0x00CC)

// Inbound Post Head Pointer
#define PLX9056_IPHPR					ULONG(0x00D0)

// Inbound Post Tail Pointer
#define PLX9056_IPTPR					ULONG(0x00D4)

// Outbound Free Head Pointer
#define PLX9056_OFHPR					ULONG(0x00D8)

// Outbound Free Tail Pointer
#define PLX9056_OFTPR					ULONG(0x00DC)

// Outbound Post Head Pointer
#define PLX9056_OPHPR					ULONG(0x00E0)

// Outbound Post Tail Pointer
#define PLX9056_OPTPR					ULONG(0x00E4)

// Queue Status/Control
#define PLX9056_QSR						ULONG(0x00E8)

#endif // __KPLX9056REG_H__
