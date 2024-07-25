// E100bexCard.h: interface for the E100bexCard class.
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


#if !defined(AFX_E100BEXCARD_H__0E5098A8_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
#define AFX_E100BEXCARD_H__0E5098A8_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Forward
class E100bexAdapter;

///////////////////////////////////////////////////////////////////////
// E100bexCard class declaration
//
// This class models the NIC.  The Phy component is modeled by the
// E100bexPhy object.
//
class E100bexCard
{

	// construction
public:
	E100bexCard(E100bexAdapter& Adapter, KNdisMemoryRange& CSRRange, ADAPTER_INFO& Ai);

	// methods
public:
	NDIS_STATUS	ResumeCommandUnit(void);
	NDIS_STATUS	StartTransmitUnit(ULONG TcbPhys);

	VOID	ResetPhy(void);
	BOOLEAN	DetectPhy(void);

	VOID	AbortReceiveUnit(void);
	VOID	StartReceiveUnit(PNDIS_PHYSICAL_ADDRESS pPhysAddr);
	BOOLEAN ReceiveUnitStarted(void);

	VOID	IssueSelectiveReset(void);

	VOID	EnableInterrupt(void);
	VOID	DisableInterrupt(void);
	BOOLEAN	IsInterrupting(void);
	BOOLEAN AckPendingInterrupts(void);

	USHORT	ReadEEprom(USHORT Reg);
	NDIS_STATUS Init(ETHERNET_ADDRESS NodeAddress);

	NDIS_STATUS		 SelfTest(void);
	ETHERNET_ADDRESS ReadPermanentNodeAddress(void);

	VOID	SetMcTimeoutFlag(void);
	VOID	ClearAllCounters(void);
	ULONG	SetupSharedMemory(ULONG pCbPhys, KNdisSharedMemory& ShMem);
	ULONG	GetPacketFilter(void);
	UINT	GetMcTimeoutFlag(void) { return m_Ai.McTimeoutFlag; }
	ULONG	GetCurrentLineSpeed(void);
	VOID	DumpStatsCounters(KNdisStats<ERR_COUNT_STRUC>& Stats);
	NDIS_STATUS	SetPacketFilter(ULONG Param);
	NDIS_STATUS NICSetMulticastList( PUCHAR PrivateMulticastBuffer, UINT nAddressCount);
	NDIS_MEDIA_STATE GetConnectionStatus(void);

protected:
	VOID	EEpromShiftOutBits(USHORT data, USHORT count);
	VOID	EEpromRaiseClock(USHORT *x);
	VOID	EEpromLowerClock(USHORT *x);
	USHORT	EEpromShiftInBits(void);
	VOID	EEpromCleanup(void);

	VOID	SoftwareReset(void);

	VOID	SelectPhy(UINT SelectPhyAddress, BOOLEAN WaitAutoNeg);
	BOOLEAN SetupPhy(void);
	VOID	FindPhySpeedAndDpx(UINT PhyId);

	NDIS_STATUS IssueScbCommand(UCHAR ScbCommandLow,BOOLEAN WaitForScb);
	BOOLEAN WaitScb(void);
	NDIS_STATUS SubmitCommandBlockAndWait(void);

	NDIS_STATUS Configure(void);
	NDIS_STATUS SetupIAAddress(ETHERNET_ADDRESS NodeAddress);

	// data
protected:
public:////////////// TEMP!!!!!!!!!!!!!!!!!!
	E100bexAdapter&	m_Adapter;
	CSRStruc		m_CSRAddress;
	ADAPTER_INFO	m_Ai;

	UCHAR			m_OldParameterField; // old packet filter
	ULONG			m_PacketFilter; 	 // The Current Global Packet Filter

	E100bexPhy		m_Phy;

	// Shared Memory structures

	volatile PSELF_TEST_STRUC	m_SelfTest;			// 82557/82558 SelfTest
	ULONG						m_SelfTestPhys;

	volatile PNON_TRANSMIT_CB	m_NonTxCmdBlock;	// 82557/82558 (non transmit) Command Block
	ULONG						m_NonTxCmdBlockPhys;
	volatile PCB_HEADER_STRUC	m_NonTxCmdBlockHdr;

	volatile PDUMP_AREA_STRUC	m_DumpSpace;		// 82557/82558 dump buffer area
	ULONG						m_DumpSpacePhys;

	volatile PERR_COUNT_STRUC	m_StatsCounters;	// 82557/82558 statistics
	ULONG						m_StatsCounterPhys;

};


inline NDIS_MEDIA_STATE E100bexCard::GetConnectionStatus()
{
	return m_Phy.GetConnectionStatus();
}

inline ULONG E100bexCard::GetCurrentLineSpeed()
{
	return m_Phy.GetCurrentLineSpeed();
}

inline VOID E100bexCard::ResetPhy()
{
	m_Phy.Reset();
}

inline UCHAR E100bexCard::DetectPhy()
{
	return m_Phy.Detect();
}

inline ULONG E100bexCard::GetPacketFilter()
{
	return m_PacketFilter;
}

#endif // !defined(AFX_E100BEXCARD_H__0E5098A8_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
