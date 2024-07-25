/////////////////////////////////////////////////////////////////////
// NmPacketBinding.h
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
//
// Packet NDIS binding related classes
//
#ifndef _NmPacketBinding_h_
#define _NmPacketBinding_h_

#include "NmPacket.h"
#include "NmPacketDevice.h"
#include	<kndis.h>

/////////////////////////////////////////////////////////////////////
// NmPacketBinding
//
// An instance of NmPacketBinding gets created for each NIC adapter
// found in the system. 
//
//
class NmPacketBinding : public KNdisProtocolBinding {
public:
	NmPacketBinding();
	~NmPacketBinding();

	// Helper methods
	NmPacketDevice* GetDevice() const { return m_pDevice; }

	// NDIS Protocol Handlers
	NDIS_STATUS Open(IN PNDIS_STRING DeviceName,
					 IN KNdisConfig& Config, 
					 OUT KNdisMedium& Medium, 
					 OUT PSTRING* AddressInfo);

	void OpenComplete();
	
	BOOLEAN Close();
    
	INT Receive(const IN PNDIS_PACKET Packet);
	
	NDIS_STATUS Receive(IN OUT KNdisPartialPacket& PacketToAccept,
						IN  PVOID HeaderBuffer,
						IN  UINT HeaderBufferSize,
						IN	PVOID LookAheadBuffer,
						IN	UINT LookaheadBufferSize,
						IN	UINT PacketSize);

	void TransferDataComplete(PNDIS_PACKET Packet,
								NDIS_STATUS Status,
								UINT BytesTransfered);

	void ReceiveComplete();

	void SendComplete(PNDIS_PACKET Packet, NDIS_STATUS Status);

	void StatusIndication(IN NDIS_STATUS GeneralStatus,
						  IN PVOID StatusBuffer,
						  IN UINT StatusBufferSize);

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

	friend class NmPacketDevice;
	friend class NmPacketControlDevice;

protected:
	// Device object representing this binding
	NmPacketDevice*		m_pDevice;
	KUstring			m_SymLinkForTestApp;

public:
	// Global list of active bindings maintanence
	static KNdisInterlockedList<NmPacketBinding>	m_BindingList;
	LIST_ENTRY		m_ListEntry;

	// Helpers
	PUNICODE_STRING GetSymLink() { return m_SymLinkForTestApp; }

};

/////////////////////////////////////////////////////
// Some useful constants

// Number of packet descriptors to allocate
const ULONG	NumberOfPacketDescriptors =	16;

const ULONG	ETHERNET_HEADER_SIZE = 14;


#endif // _NmPacketBinding_h_