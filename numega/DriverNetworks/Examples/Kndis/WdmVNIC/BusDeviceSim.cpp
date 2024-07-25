// BusDeviceSim.cpp: implementation of the CBusDeviceSim class.
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

#include <kndis.h>
#include <kndisvdw.h>		  // DriverWorks
#include "BusDeviceSim.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBusDeviceSim::CBusDeviceSim() :
	KPnpLowerDevice(),
		m_Length(DEFAULT_RX_LENGTH),
		m_SeqNum(0),
		m_RxBurstCount(0),
		m_MaxRxBurstCount(DEFAULT_MAX_RX_BURST_COUNT),
		m_Randomize(FALSE),
		m_pAdap(NULL)
{
}

CBusDeviceSim::~CBusDeviceSim()
{
	m_Timer.Cancel();
}

VOID CBusDeviceSim::SetTimerInterval(ULONG MSecs)
{
	// Normally, the bus would service the Tx and Rx IRPs sent to it by either reading
	// or writing data and completing the IRP.  We simply set up a timer to emmulate
	// this process.
	m_Timer.SetPeriodic(MSecs);
}

VOID CBusDeviceSim::SetRxPacketLength(ULONG Length)
{
	m_Length = Length;
}

VOID CBusDeviceSim::SetRxRandomize(BOOLEAN Random)
{
	m_Randomize = Random;
}

VOID CBusDeviceSim::SetMaxRxBurstCount(ULONG Count)
{
	m_MaxRxBurstCount = Count;
}

//////////////////////////////////////////////////////////////////////////////
// TimerCallback
//
// This callback acts as a proxy for the bus device servicing the IRPs sent from
// the device.  It simply calls the routines to process any outstanding IRPs
//
VOID CBusDeviceSim::TimerCallback()
{
	ProcessTxIrp();
	m_RxBurstCount = m_MaxRxBurstCount;
	ProcessRxIrp();
}

//////////////////////////////////////////////////////////////////////////////
// Call
//
NTSTATUS CBusDeviceSim::Call(KIrp I)
{
	// To simulate a call to a lower device, we must advance the stack location
	I.SetNextStackLocation();
	
	TRACE("CBusDeviceSim::Call IRP %p\n", (PIRP) I);

	// If the IRP has an MDL, it is a Rx IRP, otherwise it is a Tx IRP.  Queue it
	// accordingly
	if (I.Mdl())
		m_RxIrpList.InsertTail(I);
	else
		m_TxIrpList.InsertTail(I);

	return STATUS_PENDING;
}

//////////////////////////////////////////////////////////////////////////////
// ProcessTxIrp
//
VOID CBusDeviceSim::ProcessTxIrp()
{
	PIRP pIrp;
	while ( pIrp = m_TxIrpList.RemoveHead() )
	{
		KIrp I(pIrp);

		// Complete the IRP, with a successful status and start then next IRP
		I.Information() = 0;
		I.Complete(STATUS_SUCCESS);
	}
}

//////////////////////////////////////////////////////////////////////////////
// ProcessRxIrp
//
VOID CBusDeviceSim::ProcessRxIrp()
{
	PIRP pIrp;
	while ( m_RxBurstCount && (pIrp = m_RxIrpList.RemoveHead()) )
	{
		m_RxBurstCount--;

		KIrp I(pIrp);
		
		// Get our Buffer from the IRP
		// Note: the underlying memory was allocated by the system
		// receive area class.
		PNDIS_BUFFER pBuf = (PNDIS_BUFFER) I.Mdl();
		ASSERT(pBuf);
		KNdisBuffer Buf(pBuf);
		
		// OK, get the address and fill the buffer in.
		// NOTE that the player is
		CARD_FRAME* pFrame = (CARD_FRAME*) Buf.Address();
		ASSERT(pFrame);

		ULONG length = 0;

		if (pFrame) {
			NdisMoveMemory(pFrame->Source, sm_SimulatedSource, sizeof(sm_SimulatedSource));
			NdisMoveMemory(pFrame->Destination, sm_SimulatedDestination, sizeof(sm_SimulatedDestination));

			length = (!m_Randomize) ?  m_Length : ((m_SeqNum % m_Length)+1);
			pFrame->Length = (USHORT) length;

			RtlFillMemory(pFrame->Data, length,  UCHAR(m_SeqNum));

			// adjust for next pass
			m_SeqNum++;
			TRACE("CBusDeviceSim::ProcessRxIrp() Sequence Number %u\n", m_SeqNum);
		}

		// Complete the IRP, with a successful status and start then next IRP.
		// Note that we don't let NextIrp complete the IRP for us since it would
		// do this after processing all of the IRPs waiting in the queue.  We want
		// our IRP completed in order, so we complete it first, and then start the
		// next irp on the device.  Passing a NULL IRP to NextIrp is fine.
		I.Information() = length;
		I.Complete(STATUS_SUCCESS);
	}
}

ETHERNET_ADDRESS CBusDeviceSim::sm_SimulatedDestination = {0, 0x12, 0x34,0x56,0x78,0x90};
ETHERNET_ADDRESS CBusDeviceSim::sm_SimulatedSource		= {0, 'a','b','c','d','e'};

