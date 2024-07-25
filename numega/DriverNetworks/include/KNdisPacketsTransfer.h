// KNdisPacketsTransfer.h: interface for the KNdisPacketsTransfer class.
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

#if !defined(AFX_KNDISPACKETSTRANSFER_H__AC58CA03_9888_11D3_9002_00C04F7445B7__INCLUDED_)
#define AFX_KNDISPACKETSTRANSFER_H__AC58CA03_9888_11D3_9002_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KNdisPacketsTransfer
//
// Encapsulates the pattern of multiple packet transfer used by
// NDIS 5 protocol drivers.
//
// When a protoool driver has to send a large chunk of user data
// over a NIC binding, it has to packetize the chunk into smaller
// chunks according to the NIC's OID_GEN_MAXIMUM_TOTAL_SIZE. Then it
// can submit the array of packets using NdisSendPackets. Up to NIC's
// OID_GEN_MAXIMUM_SEND_PACKETS can be submitted at once.
//
// As each packet gets "completed" by the miniport, the protocol driver
// "recycles" the packet making it ready for further use. The protocol driver
// might also want to know when *all* the packets for the big chunk get done
// (e.g. a packet driver would complete an associated IRP in this case).
//
// Note that in case the underlying miniport doesn't support multiple send,
// NDIS will simulate that by single sends. Thus, the class can be used
// regardless the underlying capabilities of the miniport.
//
// The usage pattern of the class is as follows (assuming KNdisProtocolBinding-
// derived class).
//
// 1. Before transfer, the protocol instantiates a packets transfer object:
//
//		KNdisPacketsTransfer* PktArray = new KNdisPacketsTransfer(m_PktPool, N, irp);
//
//   m_PktPool specifies the member pool to allocate descriptors from,  N shouldn't
//	 exceed OID_GEN_MAXIMUM_SEND_PACKETS, irp is a driver-defined context associated
//	 with this transfer.
//
// 2. Then the driver forms the array by calling
//
//		while (i++ < N) {
//			{ Describe/fill in buffer KNdisBuf udata }
//			KNdisPacket packet = pTrans->PreparePacket(udata);
//			{ Optionlly, chain more buffers to the packet }
//			{ Optionally, attach OOB to the packet; }
//		}
//
//	  as many times as needed for for the array of packets.
//
// 3. Finally, the driver calls KNdisProtocolBinding's method
//
//		Send(PktArray);
//
//	  This always completes asynchronously. Note: after submitting for send
//	  the PktArray should not be touched by anything but SendComplete() routine!
//
// 4. Driver's SendComplete is called. Note that it is called separetely for each
//	  packet from the array. Here we recycle the packet and check if the whole thing
//    is done with.
//
//	  void MyBinding::SendComplete(PNDIS_PACKET packet, NDIS_STATUS Status)
//	  {
//		KNdisPacketsTransfer* PktArray = KNdisPacketsTransfer::GetObject(packet);
//		{ Unchain buffers from the packet; }
//		{ Optionally, detach OOB from the packet; }
//		PktArray->ReturnPacket(packet);
//		if (PktArray->InFlight() == 0) { // completed
//			PIRP pirp = (PIRP) PktArray->GetContext();
//			pirp->IoComplete(PktArray->Status());
//			delete PktArray;
//		}
//
// NOTE1:	 Protocol-reserved fields [0..12] are used by the object.
// NOTE2:	 Use care mixing single and multiple packets transfer over one binding.
//			 The result of GetObject() should always be checked for NULL in this case.
// CONSIDER: Placement new() for faster allocation.
//
class KNdisPacketsTransfer
{
	SAFE_DESTRUCTORS;
public:
// Constructor
	KNdisPacketsTransfer(KNdisPacketPool& Pool, UINT MaxPackets=1, PVOID Context=NULL);
// Destructor
	~KNdisPacketsTransfer();

public:
// Allocate packet for the array befor transfer
	PNDIS_PACKET PreparePacket(PNDIS_BUFFER pBuffer=NULL);

// Return packet form complete routine:
	void ReturnPacket(PNDIS_PACKET Packet, NDIS_STATUS Status=NDIS_STATUS_SUCCESS);

// Reset the array (must not be called after submission)
	void Invalidate();

// Context accessor
	PVOID GetContext() const { return m_UserContext; }
// Packets in-the-fly counter accessor
	UINT InFlight() const { return m_nInFlight; }
// Packet array accessor
	operator PPNDIS_PACKET() { return m_Array; }
// Subscription to packet at index ix
	PNDIS_PACKET operator [] (UINT ix) { return (ix<m_nMaxPackets) ? m_Array[ix]:NULL; }

// Get Cummulative status:
	NDIS_STATUS Status() const { return m_Status; }

// Retrieving object from the packet descriptor
	static KNdisPacketsTransfer* GetObject(PNDIS_PACKET packet);

// Checking if a packet belongs to this transfer
	static bool IsKindOf(PNDIS_PACKET packet);

// Binding base controls the array processing
	friend class KNdisProtocolBinding;

protected:
// internals
	KNdisPacketPool&	m_PktPool;			// pool designated for packet descriptors
	UINT				m_nMaxPackets;		// array size
	UINT				m_nInFlight;		// # of prepared but not returned
	PNDIS_PACKET* 		m_Array;			// packet array
	PVOID				m_UserContext;		// user-supplied opaque
	NDIS_STATUS			m_Status;			// cummulative status: is it useful?

// the following structure is placed at ProtocolReserved
	struct PACKET_IDENTITY {
		UINT					signature;
		UINT					index;		// packet allocation index (in m_Array[])
		KNdisPacketsTransfer*	object;		// owner-transfer

		enum { Signature = 'stkp' };
	};

// packet context using 2 first dwords of ProtocolReserved
	static PACKET_IDENTITY* Identity(PNDIS_PACKET packet);
	static void SetIdentity(PNDIS_PACKET packet, KNdisPacketsTransfer* obj, UINT ix);

	PREVENT_COPY_CONSTRUCTORS(KNdisPacketsTransfer);
};


//////////////////////////////////////////////////////////////////////////////
// inline Implementations

inline KNdisPacketsTransfer::PACKET_IDENTITY*
KNdisPacketsTransfer::Identity(PNDIS_PACKET packet)
{
	// using 3 first dwords of ProtocolReserved
	return (PACKET_IDENTITY*) packet->ProtocolReserved;
}

inline KNdisPacketsTransfer*
KNdisPacketsTransfer::GetObject(PNDIS_PACKET packet)
{
	return (IsKindOf(packet)) ? Identity(packet)->object : NULL;
}

inline void
KNdisPacketsTransfer::SetIdentity(PNDIS_PACKET packet, KNdisPacketsTransfer* obj, UINT ix)
{
	PACKET_IDENTITY* pid = Identity(packet);
	pid->object=obj; pid->index=ix; pid->signature = PACKET_IDENTITY::Signature;
}

inline bool
KNdisPacketsTransfer::IsKindOf(PNDIS_PACKET packet)
{
	return Identity(packet)->signature == PACKET_IDENTITY::Signature;
}

#endif // !defined(AFX_KNDISPACKETSTRANSFER_H__AC58CA03_9888_11D3_9002_00C04F7445B7__INCLUDED_)
