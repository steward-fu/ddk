// NmOpenContext.h
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

#ifndef __NmOpenContext_h__
#define __NmOpenContext_h__

#include "kndis.h"

class NmProtBinding; // fwd

//////////////////////////////////////////////////////////////////
// NmOpenContext
//
// Object representing NDISPROT_OPEN_CONTEXT
//
class NmOpenContext
{
	// Constructors
public:
	SAFE_DESTRUCTORS;
	NmOpenContext(NmProtBinding* Binding, PNDIS_STRING pDeviceName);
	~NmOpenContext();

	inline PNDIS_STRING DeviceName() { return m_DeviceName; }

	// cancels all reads
	void CancelAllPendingReads();
	// function waits for pending operations to complete
	void WaitForIrpsToComplete();

protected:
	// Member Data
	KNdisString				m_DeviceName;

public:
	ULONG					m_Flags;		// State information
	KNdisSpinLock			m_Lock;

	PFILE_OBJECT			m_pFileObject;	// Set on OPEN_DEVICE

	NmProtBinding*			m_pBinding;		// Binding object represented by this device

	KNdisPacketPool			m_PacketPool;	// Packet descriptor pool to serve the i/o requests

	PacketList				m_RcvList;		// Interlocked Packet list for pending receives

	KInterlockedList<IRP>	m_ResetList;	// Interlocked Packet list for pending resets

	KNdisRefCounter			m_PendingIrps;	// Pending request counter
	KNdisRefCounter			m_RefCount;		// Reference Count used to decide when to delete the NmProtDevice instance

#if NDIS51
	// For XP, implement send packet cancellation
	//
	KInterlockedList<IRP>	m_SendList;		// Interlocked IRP list for pending sends
	KNdisPacketCancelId		m_CancelId;		// Cancel ID generator for sent packets
#endif // NDIS51
};

#endif		// __NmOpenContext_h__
