/////////////////////////////////////////////////////////////////////
// NmProtBinding.h
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
// NDIS binding related classes
//
#ifndef _NmProtBinding_h_
#define _NmProtBinding_h_

#include "NmProt.h"
#include "NmOpenContext.h"
#include	<kndis.h>

/////////////////////////////////////////////////////////////////////
// NmProtBinding
//
// An instance of NmProtBinding gets created for each NIC adapter
// found in the system.
//
//
class NmProtBinding : public KNdisProtocolBinding
{
public:
	NmProtBinding();
	~NmProtBinding();

	// Helper methods
	inline NmOpenContext* GetOpenContext() const { return m_pOpenContext; }

	// NDIS Protocol Handlers
	NDIS_STATUS Open(
			IN	PNDIS_STRING			DeviceName,
			IN	KNdisConfig&			Config,
			OUT KNdisMedium&			Medium,
			OUT PSTRING*				AddressInfo
			);

	void OpenComplete();

	BOOLEAN Close();

	INT Receive(const IN PNDIS_PACKET Packet);

	NDIS_STATUS Receive(
			IN	OUT KNdisPartialPacket&	PacketToAccept,
			IN  PVOID					HeaderBuffer,
			IN  UINT					HeaderBufferSize,
			IN	PVOID					LookAheadBuffer,
			IN	UINT					LookaheadBufferSize,
			IN	UINT					PacketSize
			);

	void TransferDataComplete(
			IN	PNDIS_PACKET			Packet,
			IN	NDIS_STATUS				Status,
			IN	UINT					BytesTransfered
			);

	void ReceiveComplete();

	void SendComplete(
			IN	PNDIS_PACKET			Packet,
			IN	NDIS_STATUS				Status
			);

	void StatusIndication(
			IN	NDIS_STATUS				GeneralStatus,
			IN	PVOID					StatusBuffer,
			IN	UINT					StatusBufferSize
			);

	void StatusComplete();

	void ResetComplete(NDIS_STATUS Status);

	void RequestComplete(PNDIS_REQUEST NdisRequest, NDIS_STATUS Status);

	// PnP Events:
	NDIS_STATUS OnPnpSetPower(NDIS_DEVICE_POWER_STATE PowerState, PNET_PNP_EVENT PowerEvent);
	NDIS_STATUS OnPnpQueryPower(NDIS_DEVICE_POWER_STATE PowerState, PNET_PNP_EVENT PowerEvent);
	NDIS_STATUS OnPnpQueryRemoveDevice(PNET_PNP_EVENT PowerEvent);
	NDIS_STATUS OnPnpCancelRemoveDevice(PNET_PNP_EVENT PowerEvent);
	NDIS_STATUS OnPnpReconfigure(PCWSTR /*wszProtocolSection*/, PNET_PNP_EVENT PowerEvent);
	NDIS_STATUS OnPnpCapabilityChange(ULONG uWakeUpCapFlag, PNET_PNP_EVENT /*PowerEvent*/);
	static NDIS_STATUS OnPnpReconfigureGlobal(PCWSTR /*wszProtocolSection*/, PNET_PNP_EVENT /*PowerEvent*/);
	static NDIS_STATUS OnPnpBindListChange(PCWSTR wszNewBindList, PNET_PNP_EVENT /*PowerEvent*/);
	static NDIS_STATUS OnPnpBindListComplete(PNET_PNP_EVENT /*PowerEvent*/);

	friend class NmOpenContext;

protected:
	// Device open context representing this binding
	NmOpenContext*		m_pOpenContext;

public:
	// Global list of active bindings maintanence
	static KNdisInterlockedList<NmProtBinding>	m_BindingList;
	LIST_ENTRY			m_ListEntry;
	static KNdisEvent	m_BindsComplete;
};

/////////////////////////////////////////////////////
// Some useful constants

// Number of packet descriptors to allocate
#define NUMBER_OF_PACKET_DESCRIPTORS	16
#define ETHERNET_HEADER_SIZE			14


#endif // _NmProtBinding_h_
