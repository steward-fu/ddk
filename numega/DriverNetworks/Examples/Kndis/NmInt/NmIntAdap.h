// NmIntAdap.h: interface for the NmIntAdapter class.
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


#if !defined(AFX_NMINTADAP_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_NMINTADAP_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>
#include	"Characteristics.h" 

// Forwards
class NmIntDriver;


//=============================================================================
// NmIntAdapter		IM (Filter) Adapter Class 
//
// This class defines member methods which are passed control on
// NDIS callbacks. 
//
// The instances of the class are created automatically
// by DriverNetworks framework. The class exposes no public methods
// since its methods are called only from the "friendly"
// KNdisWrapper and NmIntBinding classes.
//
class NmIntAdapter : public KNdisMiniAdapter 
{
	SAFE_DESTRUCTORS
public:
	NmIntAdapter();
	virtual ~NmIntAdapter();

protected:
// Base class handler overrides
	// First call on a new object: a chance to accept or reject the binding
	NDIS_STATUS Initialize(KNdisMedium&, KNdisConfig&);

	// Last call on the object about to be destroyed
	VOID Halt();

	NDIS_STATUS Reset(OUT PBOOLEAN AddressingReset);

	NDIS_STATUS Send(IN	PNDIS_PACKET Packet, IN	UINT Flags);
	void SendComplete(
			IN PNDIS_PACKET Packet,
			IN NDIS_STATUS Status
			);

	VOID ReturnPacket(IN PNDIS_PACKET Packet);

	NDIS_STATUS TransferData(
		OUT PNDIS_PACKET			Packet,
		OUT PUINT					BytesTransferred,
		IN  PVOID					MiniportReceiveContext,
		IN	UINT					ByteOffset,
		IN	UINT					BytesToTransfer
		);

	void TransferDataComplete(
		IN PNDIS_PACKET Packet, 
		IN NDIS_STATUS Status, 
		IN UINT BytesTransferred
		);

	NDIS_STATUS DeInitialize() { return NdisIMDeInitializeDeviceInstance(*this); }

	// OID processing (via OID_MAP)
	NDIS_STATUS QueryInformation(
		IN	NDIS_OID				Oid,
		IN	PVOID					InformationBuffer,
		IN	ULONG					InformationBufferLength,
		OUT PULONG					BytesWritten,
		OUT PULONG					BytesNeeded
		);
	NDIS_STATUS SetInformation(
		IN	NDIS_OID				Oid,
		IN	PVOID					InformationBuffer,
		IN	ULONG					InformationBufferLength,
		OUT PULONG					BytesRead,
		OUT PULONG					BytesNeeded
		);

	// Standard OID handlers. This should be included in every adapter class declaration.
	#include	<KNdisOidDefs.h>

	// A list of supported OIDs. Inserted by the wizard
	static NDIS_OID sm_OID_GEN_SUPPORTED_LIST[];

// Helper methods
protected:
	static NDIS_HANDLE GetContainerHandle();
	static NmIntDriver* Driver();

	bool  AddBinding(NmIntBinding* b);
	ULONG RemoveBinding(NmIntBinding* b);

	NmIntBinding** GetBindings() { return m_ppBindings; }
	NmIntBinding*  GetBinding(const KNdisPacket& P);

	void DecBindCount();
	void IncBindCount();

	NDIS_STATUS MatchPacketFilter(KNdisPacket& P);
	NDIS_STATUS MatchPacketFilter(PUCHAR pEthHdr);
	NDIS_STATUS SetNicPacketFilter(PULONG Filter);

	NDIS_STATUS GetCapabilities(PNDIS_PNP_CAPABILITIES pCaps);

	// Data Members
protected:
	// Array of bindings for this adapter instance
	NmIntBinding**		m_ppBindings;
	KNdisCounter		m_nBindings;
	static const ULONG	m_nMaxBindings;

	// Packet filter
	ULONG				m_PacketFilter;

	// 802_3 addresses
	ETHERNET_ADDRESS	m_CurrEthAddress;
	ETHERNET_ADDRESS	m_PermEthAddress;
	ETHERNET_ADDRESS	m_MulticastAddrs[MAX_MULTICAST_ADDRESSES];
	UINT				m_NumMacAddr;

	// Statistics
	KNdisStatsEx<NdisMedium802_3>	m_Stats;
	KNdisStatsGen					m_GenStats;

	// OID data cached from the NIC
	ULONG				m_MaxLookAhead;
	ULONG				m_LinkSpeed;
	NDIS_PHYSICAL_MEDIUM m_PhysicalMedium;

	UINT				m_MaxPoolSize;

	// Power Management State
	KNdisPnpMode<NmIntAdapter>	m_Power;

	// Packet Pool. This pool is used for sending packets only!
	KNdisPacketPool		m_TxPktPool;

	// Virtual NIC GUID name (cached)
	NDIS_STRING			m_wsInstance;

	// Outstanding request count
	KNdisCounter		m_Count;

	// Number of instances
	static	UCHAR		m_AdapNum;

public:

	bool IsUp() { return m_State==UP; }

	// Adapter State (to handle halt/unbind conditions)
	enum State {
		DOWN,
		INITIALIZING,
		UP,
		DEINITIALIZING,
		HALTING
	};

	State m_State;

	typedef struct {	// Format for Repackaged @ ProtocolReserved	
		NmIntAdapter*		OriginalAdapter;	
		PNDIS_PACKET	    OriginalPacket;
	} TxRESERVED;

	typedef KNdisPacketWithContext<TxRESERVED, TRUE> TxPacket;


	// friends
	friend class KNdisWrapper<NmIntAdapter>;
	friend class NmIntBinding;
	friend class NmIntDriver;
};


//=============================================================================
//	NmIntAdapter::Driver
//
inline NmIntDriver* NmIntAdapter::Driver()
{
	// Assumes the driver uses KNdisIMiniDriver-derived for driver singleton
	NmIntDriver* p = reinterpret_cast<NmIntDriver*>(KNdisMiniDriver::DriverInstance());
	ASSERT(p);
	return p;
}


//=========================================================================
// NmIntAdapter::GetContainerHandle
//
inline NDIS_HANDLE NmIntAdapter::GetContainerHandle()
{
	return Driver()->GetMiniportHandle();
}


#endif // !defined(AFX_NMINTADAP_H__8534F9A9_27F5_11D3_8F9E_00C04F7445B7__INCLUDED_)
 