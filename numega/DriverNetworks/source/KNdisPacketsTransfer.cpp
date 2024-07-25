// KNdisPacketsTransfer.cpp: implementation of the KNdisPacketsTransfer class.
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

#include "kndis.h"

//////////////////////////////////////////////////////////////////////
// KNdisPacketsTransfer::KNdisPacketsTransfer
//
// Initializes packets transfer object
//
// Parameters:
//		Pool		-	Pool that should be used for packet descriptors
//		MaxPackets	-	Max size of transfer (usually <= OID_GEN_MAXIMUM_SEND_PACKETS)
//		Context		-	Opaque use context used in SendComplete()
//
KNdisPacketsTransfer::KNdisPacketsTransfer
	(KNdisPacketPool& Pool, UINT MaxPackets, PVOID Context) :
		m_PktPool(Pool), m_nMaxPackets(MaxPackets), m_UserContext(Context),
			m_nInFlight(0), m_Status(NDIS_STATUS_SUCCESS)
{
	m_Array = new PNDIS_PACKET [m_nMaxPackets];
	ASSERT(m_Array);
	if (m_Array)
		NdisZeroMemory(m_Array, m_nMaxPackets*sizeof(PNDIS_PACKET));
}

KNdisPacketsTransfer::~KNdisPacketsTransfer()
{
	ASSERT(m_nInFlight==0);
	delete m_Array;
}

//////////////////////////////////////////////////////////////////////
// KNdisPacketsTransfer::PreparePacket
//
// Prepares a supplied buffer (or chain) for transfer. This effectively
// initializes the next entry in the packet array for transfer.
//
// Parameters:
//		pBuffer			Optional Buffer descriptor to be chained to the packet.
// Returns:
//		Packet descriptor, or NULL, if an error occured. The user can chain
//		more buffer descriptors or OOB data to the packet before submitting
//		the array for transmission.
//
PNDIS_PACKET
KNdisPacketsTransfer::PreparePacket(PNDIS_BUFFER pBuffer)
{
	if (m_nInFlight <= m_nMaxPackets) {
		KNdisPacket pkt = m_PktPool.Allocate();
		if (pkt.IsValid()) {
			SetIdentity(pkt, this, m_nInFlight);
			m_Array[m_nInFlight++] = pkt;
			if (pBuffer)
				pkt.ChainAtBack(pBuffer);
			return pkt;
		}
		else {
			ASSERT(!"Pool");	// pool exhausted!
		}
	}
	else {
			ASSERT(!"Array");	 // too many prepared!
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// KNdisPacketsTransfer::ReturnPacket
//
// Returns a packet to an associated pool. Usually called from
// protocol's SendComplete() handler. The caller should (but doesn't have to)
// unchain and release all the buffers and OOBs chained to the packet.
// Otherwise the method does that itself by invoking KNdisPacket::ReinitializeEx()
//
// Parameters:
//		Packet		Packet being completed and returned.
//		Status		Status of this packet. Used for cummulative status
// Return:
//		None
//
void KNdisPacketsTransfer::ReturnPacket(PNDIS_PACKET Packet, NDIS_STATUS Status)
{
	if (IsKindOf(Packet)) {
			KNdisPacket pkt(Packet);				// for convenience

			PACKET_IDENTITY* pid = Identity(Packet);
			ASSERT(pid->object == this);			// programming error
			ASSERT(pid->index < m_nMaxPackets);		// programming error
			ASSERT(m_Array[pid->index] == Packet);	// programming error
			// ASSERT(pkt.QueryBufferCount() == 0);	// has chained buffers: prog error
			m_Array[pid->index] = NULL;
			m_nInFlight--;
			pkt.ReinitializeEx();					// unchain and free everything
			m_PktPool.Free(Packet);

			if (Status != NDIS_STATUS_SUCCESS)
				m_Status = Status;
	}
	else ASSERT(!"IsKindOf");
}

//////////////////////////////////////////////////////////////////////
// KNdisPacketsTransfer::Invalidate
//
// Invalidates the previously allocated packets in the array. Should
// be used with caution whenevre an error has occured while building
// the array using PreparePacket(). Should NEVER be called after the
// array has been submitted for transmission.
//
// Parameters:
//		None
// Return:
//		None
//
void KNdisPacketsTransfer::Invalidate()
{
	PNDIS_PACKET p;
	for (UINT i=0; i<m_nMaxPackets; i++) {
		if ((p = *this [i]) != NULL)
			ReturnPacket(p);
	}
}
