// BusDeviceSim.h: interface for the CBusDeviceSim class.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUSDEVICESIM_H__E02CDA03_5C33_4858_A21D_2EDAFC4A0669__INCLUDED_)
#define AFX_BUSDEVICESIM_H__E02CDA03_5C33_4858_A21D_2EDAFC4A0669__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_MAX_RX_BURST_COUNT 20
#define DEFAULT_RX_LENGTH 1000

#pragma pack(1)

typedef struct {
	ETHERNET_ADDRESS	Destination;
	ETHERNET_ADDRESS	Source;
	USHORT				Length;		// of Data[]
	UCHAR				Data[1500];
} CARD_FRAME;

#pragma pack()

class CBusDeviceSim : public KPnpLowerDevice
{
// Methods
public:
	CBusDeviceSim();
	virtual ~CBusDeviceSim();
	
	VOID SetAdapter(KNdisMiniAdapter* pAdap) 
		{m_pAdap=pAdap;m_Timer.Initialize(this,KNDIS_MEMBER_CALLBACK(TimerCallback));}

	VOID SetTimerInterval(ULONG MSecs);
	VOID SetRxPacketLength(ULONG Length);
	VOID SetRxRandomize(BOOLEAN Random);
	VOID SetMaxRxBurstCount(ULONG Count);

	// We hide the KLowerDevice Call method
	NTSTATUS Call(KIrp I);

	KNDIS_DECLARE_TIMERCALLBACK				(CBusDeviceSim, TimerCallback)
	VOID TimerCallback( );		// COMMENT_ONLY

	operator NDIS_HANDLE() {
		return *m_pAdap;
	}

#ifdef __COMMENT_ONLY
	// The following member functions are actually defined by the
	// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
	// The macro __COMMENT_ONLY never gets defined.  These comment-only
	// definitions simply allow easy navigation to the functions within
	// the Developer Studio using the class browser.

#endif

protected:
	VOID ProcessTxIrp(void);
	VOID ProcessRxIrp(void);


// Data
protected:
	KNdisTimer m_Timer;
	KNdisEvent m_TxProcess;
	KNdisEvent m_RxProcess;
	KNdisInterlockedList <IRP> m_TxIrpList;
	KNdisInterlockedList <IRP> m_RxIrpList;
	ULONG m_Length;
	ULONG m_SeqNum;
	ULONG m_RxBurstCount;
	ULONG m_MaxRxBurstCount;
	BOOLEAN m_Randomize;

	KNdisMiniAdapter* m_pAdap;

	static ETHERNET_ADDRESS	sm_SimulatedSource;
	static ETHERNET_ADDRESS	sm_SimulatedDestination;
};

#endif // !defined(AFX_BUSDEVICESIM_H__E02CDA03_5C33_4858_A21D_2EDAFC4A0669__INCLUDED_)
