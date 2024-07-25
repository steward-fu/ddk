// NmIntBind.h: interface for the NmIntBinding class.
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


#if !defined(AFX_NMINTBIND_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_NMINTBIND_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Forwards
class NmIntDriver;
class NmIntAdapter;

//=========================================================================
// NmIntBinding		IM (Generic) Binding Class 
//
// Implements the "protocol" personality of a Generic IM driver
// (MUX type of IM driver.)
//
// The instances of the class are created automatically
// by DriverNetworks framework. 
//=========================================================================
class NmIntBinding : public KNdisProtocolBinding 
{
	SAFE_DESTRUCTORS
public:
	NmIntBinding();
	virtual ~NmIntBinding();

// Virtual Overrides

	// Intercepting protocol reconfigurations (PnP)
	virtual NDIS_STATUS OnPnpSetPower(NDIS_DEVICE_POWER_STATE PowerState,PNET_PNP_EVENT);
	virtual NDIS_STATUS OnPnpReconfigure(PCWSTR wsSection,PNET_PNP_EVENT);
	static  NDIS_STATUS OnPnpBindListComplete(PNET_PNP_EVENT);

protected:
// NDIS protocol callback handlers: methods required by KNdisProtocolWrapper<Binding> template
// Lower-edge Handlers: delegated from KNdisProtocolWrapper<Binding>

	NDIS_STATUS Open(
			IN PNDIS_STRING DeviceName,
			IN KNdisConfig& Config, 
			OUT KNdisMedium& Medium, 
			OUT PSTRING*
			);

	void	OpenComplete();
	BOOLEAN Close();
	INT		Receive(PNDIS_PACKET p);
	NDIS_STATUS Receive(
			IN  OUT	KNdisPartialPacket& PacketToAccept,
			IN  PVOID					HeaderBuffer,
			IN  OUT UINT				HeaderBufferSize,
			IN	PVOID					LookaheadBuffer,
			IN	OUT UINT				LookaheadBufferSize,
			IN	UINT					PacketSize
			);

	void SendComplete(PNDIS_PACKET Packet, NDIS_STATUS Status);

	// Intercepting lower-layer Status indications
	void StatusIndication(
			IN NDIS_STATUS GeneralStatus,
			IN PVOID StatusBuffer,
			IN UINT StatusBufferSize
			);
	void StatusComplete();
	void RequestComplete(PNDIS_REQUEST, NDIS_STATUS Status);
	NDIS_STATUS TransferData(
			IN NDIS_HANDLE MacReceiveContext,
			IN UINT ByteOffset,
			IN UINT BytesToTransfer,
			IN OUT PNDIS_PACKET Packet,
			OUT PUINT BytesTransferred
			);

	// Partial receive complete
	void TransferDataComplete(
			PNDIS_PACKET Packet,
			NDIS_STATUS Status,
			UINT BytesTransferred
			);

// Helper methods
	static NmIntDriver* Driver();
	NmIntAdapter** GetAdapters() { return m_ppAdapters; }
	bool AddAdapter(NmIntAdapter*);
	ULONG RemoveAdapter(NmIntAdapter* a);

	// these methods send NDIS_REQUESTs to the underlying NIC
	void QueryAdapterInfo();
	ULONG GetOid(NDIS_OID Oid);
	NDIS_STATUS SetOid(NDIS_OID Oid, PULONG p);
	NDIS_STATUS GetCapabilities(PNDIS_PNP_CAPABILITIES pCaps);

	bool IsUp() { return (m_State==UP && m_BindingMediaState==NdisMediaStateConnected); }

	void IncAdapCount();
	void DecAdapCount();
protected:

	// configuration parameter read from registry
	KNdisString			m_sParam1;

	// Array of bound adapters
	NmIntAdapter**		m_ppAdapters;
	KNdisCounter		m_nAdapters;
	static const ULONG	m_nMaxAdapters;

	// Data cached from underlying NIC
	// 802_3 addresses
	ETHERNET_ADDRESS	m_CurrEthAddress;
	// OID data cached from the NIC
	ULONG				m_MaxLookAhead;
	ULONG				m_LinkSpeed;
	NDIS_PHYSICAL_MEDIUM m_PhysicalMedium;

	// current power state
	KNdisPowerTransition<NmIntBinding> m_Power;

	// current miniport status
	NDIS_MEDIA_STATE	m_BindingMediaState;

// Accessor to system-registered container handle for all instances of this adapter:
	static NDIS_HANDLE GetContainerHandle();

	NDIS_PNP_CAPABILITIES m_Caps;
	bool bIsPmAware;
protected:

	NDIS_STRING		m_wsDriverInstance;		// valid only during initialization 
	KNdisString		m_strDeviceName;		// cached name of the bound NIC 

	// Packet Pool. This pool is used for indicating packets up to bound protocols only!
	KNdisPacketPool	m_RxPktPool;

public:

	KNdisCounter	m_Count;

	// binding State (to handle unbind/halt conditions)
	enum State {
		DOWN,
		INITIALIZING,
		UP,
		CLOSING
	};

	State m_State;

	typedef struct {	// Format for Repackaged @ MiniportReserved
		NmIntBinding*		OriginalBinding;	
		PNDIS_PACKET		OriginalPacket;
	} RxRESERVED;

	typedef KNdisPacketWithContext<RxRESERVED,false> RxPacket;

	typedef struct {
		NmIntAdapter*		OriginalAdapter;	
		NmIntBinding*	    OriginalBinding;
		NDIS_HANDLE		    MacRcvCtx;
	} PARTIAL_RCV_CTX;

	// friends
	friend class KNdisProtocolWrapper<NmIntBinding>;
	friend class NmIntAdapter;
	friend class NmIntDriver;
};


//=========================================================================
//
//						Inline Implementations
//
//=========================================================================


//=========================================================================
// NmIntBinding::Driver
//
inline NmIntDriver* NmIntBinding::Driver()
{
	// Assumes the driver uses KNdisIMiniDriver-derived for driver singleton
	NmIntDriver* p = reinterpret_cast<NmIntDriver*>(KNdisMiniDriver::DriverInstance());
	ASSERT(p);
	return p;
}


//=========================================================================
// NmIntBinding::GetContainerHandle
//
inline NDIS_HANDLE NmIntBinding::GetContainerHandle()
{
	// Assumes the driver uses KNdisIMiniDriver-derived for driver singleton
	
	return Driver()->GetProtocolHandle();
}


//=========================================================================
//
//			KNdisProtocolWrapper<> Template specializations
//
//=========================================================================


//=========================================================================
// KNdisProtocolWrapper<NmIntBinding>::RequestComplete
//
// Implements the special case of protocol request completions
// for an our IM driver.
//
inline VOID KNdisProtocolWrapper<NmIntBinding>::RequestComplete(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	PNDIS_REQUEST			NdisRequest,
	IN	NDIS_STATUS				Status
	) {
	NmIntBinding* b = reinterpret_cast<NmIntBinding*>(ProtocolBindingContext);
	ASSERT(b->IsValid());
	b->RequestComplete(NdisRequest, Status);
}


//=========================================================================
// KNdisProtocolWrapper<NmIntBinding>::Receive
//
// Implements the special case of partial receives
// for an our IM driver.
//
inline NDIS_STATUS KNdisProtocolWrapper<NmIntBinding>::Receive(
	IN	NDIS_HANDLE				ProtocolBindingContext,
	IN	NDIS_HANDLE				MacReceiveContext,
	IN	PVOID					HeaderBuffer,
	IN	UINT					HeaderBufferSize,
	IN	PVOID					LookAheadBuffer,
	IN	UINT					LookAheadBufferSize,
	IN	UINT					PacketSize
	) 
{
	// Check if there is an associated packet for this indication.
	// If there is, delegete to the packet receive handler, otherwise
	// delegate to partial receive handler
	NmIntBinding* b = reinterpret_cast<NmIntBinding*>(ProtocolBindingContext);
	ASSERT (_KNDIS::IsValid(b));
	NDIS_STATUS Status;
#if (defined(NDIS50) || defined(NDIS51))
	PNDIS_PACKET pkt = NdisGetReceivedPacket(b->m_Handle, MacReceiveContext);
	if (pkt) {
		Status = ( b->Receive(pkt) ) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_NOT_ACCEPTED;
	}
	else
#endif
	{
		KNdisPartialPacket Partial(MacReceiveContext);

		Status = b->Receive(
			Partial,
			HeaderBuffer,
			HeaderBufferSize,
			LookAheadBuffer,
			LookAheadBufferSize,
			PacketSize
			);
	}
	return Status;
}


#endif // !defined(AFX_NMINTBIND_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
