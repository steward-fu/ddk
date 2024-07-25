// KNdisFilterBinding.h: interface for the KNdisFilterBinding class.
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

#if !defined(AFX_KNDISFILTERBINDING_H__78D864B4_938E_11D3_9002_00C04F7445B7__INCLUDED_)
#define AFX_KNDISFILTERBINDING_H__78D864B4_938E_11D3_9002_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
// KNdisFilterBinding
//
// Helper class used by IM miniports. Implements the "protocol"
// personality of an IM driver. Delegates NDIS protocol callbacks
// to appropriate handlers of the friendly KNdisFilterAdapter-derived
// class.
//
class KNdisFilterBinding : public KNdisProtocolBinding
{
	SAFE_DESTRUCTORS;
public:
	KNdisFilterBinding();
	virtual ~KNdisFilterBinding();

// NDIS protocol handlers
	virtual NDIS_STATUS Open(
							IN PNDIS_STRING DeviceName,
							IN KNdisConfig& Config,
							OUT KNdisMedium& Medium,
							OUT PSTRING* AddressInfo);

	virtual void OpenComplete();
	virtual BOOLEAN Close();

	virtual INT Receive(const IN PNDIS_PACKET Packet);
	virtual NDIS_STATUS Receive(IN OUT KNdisPartialPacket& PacketToAccept,
						IN  PVOID HeaderBuffer,
						IN  UINT HeaderBufferSize,
						IN	PVOID LookAheadBuffer,
						IN	UINT LookaheadBufferSize,
						IN	UINT PacketSize);

	virtual void TransferDataComplete(PNDIS_PACKET Packet,
									  NDIS_STATUS Status,
									  UINT BytesTransferred);

	virtual void ReceiveComplete();

	virtual void SendComplete(PNDIS_PACKET Packet, NDIS_STATUS Status);

	virtual void StatusIndication(IN NDIS_STATUS GeneralStatus,
						  IN PVOID StatusBuffer,
						  IN UINT StatusBufferSize);

	virtual void StatusComplete();

	virtual void RequestComplete(PNDIS_REQUEST NdisRequest, NDIS_STATUS Status);

#if KNDIS_PNP_AWARE

	virtual NDIS_STATUS OnPnpSetPower(NDIS_DEVICE_POWER_STATE PowerState,PNET_PNP_EVENT);
	virtual NDIS_STATUS OnPnpQueryPower(NDIS_DEVICE_POWER_STATE /*PowerState*/,PNET_PNP_EVENT);
	virtual NDIS_STATUS OnPnpQueryRemoveDevice(PNET_PNP_EVENT);
	virtual NDIS_STATUS OnPnpCancelRemoveDevice(PNET_PNP_EVENT);
	virtual NDIS_STATUS OnPnpReconfigure(PCWSTR /*wszProtocolSection*/,PNET_PNP_EVENT);
	virtual NDIS_STATUS OnPnpCapabilityChange(ULONG /*uWakeUpCapFlag*/,PNET_PNP_EVENT);

	static NDIS_STATUS OnPnpReconfigureGlobal(PCWSTR /*wszProtocolSection*/,PNET_PNP_EVENT);
	static NDIS_STATUS OnPnpBindListChange(PCWSTR /*wszNewBindList*/,PNET_PNP_EVENT);
	static NDIS_STATUS OnPnpBindListComplete(PNET_PNP_EVENT);

#endif // KNDIS_PNP_AWARE

// Accessor to system-registered container handle for all instances of this adapter:
	static NDIS_HANDLE GetContainerHandle();

	friend class KNdisFilterAdapter;

protected:
// Internals

	NDIS_STRING			m_wsDriverInstance;		// valid only during initialization (fix that?)
	KNdisFilterAdapter*	m_pAdapter;				// associated adapter: initialized by KNdisFilterAdapter::Initialize()
	KNdisString			m_strDeviceName;		// cached name of the bound NIC (needed?)

	KNdisBundle*		m_pBundle;				// A LBFO bundle (if any) this binding (real NIC) is to be a part of

};

////////////////////////////////////////////////////////////////////
// Inline Implementations

inline NDIS_HANDLE KNdisFilterBinding::GetContainerHandle()
{
	// Assumes the driver uses KNdisIMiniDriver-derived for driver singleton

	return
	static_cast<KNdisIMiniDriver*>(KNdisMiniDriver::DriverInstance())->GetProtocolHandle();
}

#endif // !defined(AFX_KNDISFILTERBINDING_H__78D864B4_938E_11D3_9002_00C04F7445B7__INCLUDED_)
