// NmIntOids.cpp: Definition of supported OIDs data.
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


#define	KNDIS_OID_PNP_CAPABILITIES_HANDLER(Class)	\
	case OID_PNP_CAPABILITIES:		\
	MoveSource=NULL; *BytesNeeded = sizeof(NDIS_PNP_CAPABILITIES); BytesToMove =  \
	((_status = Class::GetCapabilities((PNDIS_PNP_CAPABILITIES)InformationBuffer))==NDIS_STATUS_SUCCESS) \
		? sizeof(NDIS_PNP_CAPABILITIES) : 0;		\
	break;											


#include <kndis.h>
#include "NmIntDrv.h"
#include "NmIntAdap.h"
#include "NmIntBind.h"


//=============================================================================
// List of supported OIDs. 
// NOTES 
//	1. The static variable name is a standartized ("reserved") name in KNDIS
//  2. No compile-time provisions are made to guarantee the acsending order
//	   of the array. There is a run-time assertion in DBG builds, though.
//=============================================================================
NDIS_OID NmIntAdapter::sm_OID_GEN_SUPPORTED_LIST[] = {
	OID_GEN_SUPPORTED_LIST, 
	OID_GEN_HARDWARE_STATUS, 
	OID_GEN_MEDIA_SUPPORTED, 
	OID_GEN_MEDIA_IN_USE, 
	OID_GEN_MAXIMUM_LOOKAHEAD, 
	OID_GEN_MAXIMUM_FRAME_SIZE, 
	OID_GEN_LINK_SPEED, 
	OID_GEN_TRANSMIT_BUFFER_SPACE, 
	OID_GEN_RECEIVE_BUFFER_SPACE, 
	OID_GEN_TRANSMIT_BLOCK_SIZE, 
	OID_GEN_RECEIVE_BLOCK_SIZE, 
	OID_GEN_VENDOR_ID, 
	OID_GEN_VENDOR_DESCRIPTION, 
	OID_GEN_CURRENT_PACKET_FILTER, 
	OID_GEN_CURRENT_LOOKAHEAD, 
	OID_GEN_DRIVER_VERSION, 
	OID_GEN_MAXIMUM_TOTAL_SIZE, 
	OID_GEN_PROTOCOL_OPTIONS, 
	OID_GEN_MAC_OPTIONS, 
	OID_GEN_MEDIA_CONNECT_STATUS, 
	OID_GEN_MAXIMUM_SEND_PACKETS, 
	OID_GEN_VENDOR_DRIVER_VERSION, 
	OID_GEN_XMIT_OK, 
	OID_GEN_RCV_OK, 
	OID_GEN_XMIT_ERROR, 
	OID_GEN_RCV_ERROR, 
	OID_GEN_RCV_NO_BUFFER, 
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_802_3_RCV_ERROR_ALIGNMENT,
    OID_802_3_XMIT_ONE_COLLISION,
    OID_802_3_XMIT_MORE_COLLISIONS,
#if (KNDIS_PNP_AWARE)
	OID_PNP_CAPABILITIES,
	OID_PNP_SET_POWER,
	OID_PNP_QUERY_POWER,
	OID_PNP_ADD_WAKE_UP_PATTERN,
	OID_PNP_REMOVE_WAKE_UP_PATTERN,
	OID_PNP_WAKE_UP_PATTERN_LIST,
	OID_PNP_ENABLE_WAKE_UP,
#endif // KNDIS_PNP_AWARE

};


//=============================================================================
// OID Query Map
//=============================================================================
//{{AFX_DNW

// turn on optimization to compile out dead handler references
#pragma optimize("g", on)	
BEGIN_OID_QUERY_MAP(NmIntAdapter)

	// GEN
	OID_QUERY_ENTRY( OID_GEN_HARDWARE_STATUS)
	OID_QUERY_ENTRY( OID_GEN_MEDIA_SUPPORTED)
	OID_QUERY_ENTRY( OID_GEN_MEDIA_IN_USE)
	OID_QUERY_ENTRY( OID_GEN_MAXIMUM_LOOKAHEAD)
	OID_QUERY_ENTRY( OID_GEN_MAXIMUM_FRAME_SIZE)
	OID_QUERY_ENTRY( OID_GEN_LINK_SPEED)

	OID_QUERY_ENTRY( OID_GEN_TRANSMIT_BUFFER_SPACE)
	OID_QUERY_ENTRY( OID_GEN_RECEIVE_BUFFER_SPACE)

	OID_QUERY_ENTRY( OID_GEN_TRANSMIT_BLOCK_SIZE)
	OID_QUERY_ENTRY( OID_GEN_RECEIVE_BLOCK_SIZE)
	OID_QUERY_ENTRY( OID_GEN_VENDOR_ID)
	OID_QUERY_ENTRY( OID_GEN_VENDOR_DESCRIPTION)
	OID_QUERY_ENTRY( OID_GEN_CURRENT_PACKET_FILTER)
	OID_QUERY_ENTRY( OID_GEN_CURRENT_LOOKAHEAD)
	OID_QUERY_ENTRY( OID_GEN_DRIVER_VERSION)
	OID_QUERY_ENTRY( OID_GEN_VENDOR_DRIVER_VERSION)
	OID_QUERY_ENTRY( OID_GEN_MAXIMUM_TOTAL_SIZE)
	OID_QUERY_ENTRY( OID_GEN_MAC_OPTIONS)
	OID_QUERY_ENTRY( OID_GEN_MEDIA_CONNECT_STATUS)
	OID_QUERY_ENTRY( OID_GEN_MAXIMUM_SEND_PACKETS)

	// OID_QUERY_ENTRY( OID_GEN_SUPPORTED_GUIDS)

    OID_QUERY_ENTRY( OID_GEN_XMIT_OK)
    OID_QUERY_ENTRY( OID_GEN_RCV_OK)
    OID_QUERY_ENTRY( OID_GEN_XMIT_ERROR)
    OID_QUERY_ENTRY( OID_GEN_RCV_ERROR)
    OID_QUERY_ENTRY( OID_GEN_RCV_NO_BUFFER)

	// 802_3
    OID_QUERY_ENTRY( OID_802_3_PERMANENT_ADDRESS)
    OID_QUERY_ENTRY( OID_802_3_CURRENT_ADDRESS)
    OID_QUERY_ENTRY( OID_802_3_MULTICAST_LIST)
    OID_QUERY_ENTRY( OID_802_3_MAXIMUM_LIST_SIZE)
    OID_QUERY_ENTRY( OID_802_3_RCV_ERROR_ALIGNMENT)
    OID_QUERY_ENTRY( OID_802_3_XMIT_ONE_COLLISION)
    OID_QUERY_ENTRY( OID_802_3_XMIT_MORE_COLLISIONS)

	// PNP
#if (KNDIS_PNP_AWARE)
	OID_QUERY_ENTRY( OID_PNP_QUERY_POWER)
// This NIC does not have Wake-up capabilities so these are commented out.
//	OID_QUERY_ENTRY( OID_PNP_ENABLE_WAKE_UP	)
#endif // KNDIS_PNP_AWARE

END_OID_QUERY_MAP()
#pragma optimize("", on)	// restore default optimization
//}}AFX_DNW

//=============================================================================
// OID Set Map
//=============================================================================
//{{AFX_DNW

// turn on optimization to compile out dead handler references
#pragma optimize("g", on)	
BEGIN_OID_SET_MAP(NmIntAdapter)

	// GEN
	OID_SET_ENTRY( OID_GEN_CURRENT_LOOKAHEAD)
	OID_SET_ENTRY( OID_GEN_CURRENT_PACKET_FILTER)
	OID_SET_ENTRY( OID_GEN_PROTOCOL_OPTIONS)

	// 802.3
    OID_SET_ENTRY( OID_802_3_MULTICAST_LIST)

	// PNP
#if (KNDIS_PNP_AWARE)
    OID_SET_ENTRY( OID_PNP_SET_POWER)
// This NIC does not have Wake-up capabilities so these are commented out.
//    OID_SET_ENTRY( OID_PNP_ADD_WAKE_UP_PATTERN)
//    OID_SET_ENTRY( OID_PNP_REMOVE_WAKE_UP_PATTERN)
//    OID_SET_ENTRY( OID_PNP_ENABLE_WAKE_UP)
#endif // KNDIS_PNP_AWARE

END_OID_SET_MAP()
#pragma optimize("", on)	// restore default optimization
//}}AFX_DNW

//=============================================================================
// OID Handlers
//=============================================================================

//=============================================================================
// OID Query Handlers

inline NDIS_HARDWARE_STATUS NmIntAdapter::getOID_GEN_HARDWARE_STATUS(void)
{
	return NdisHardwareStatusReady;
}

inline ULONG  NmIntAdapter::getOID_802_3_XMIT_ONE_COLLISION(void)
{
	return m_Stats->xmit_one_collision;
}

inline ULONG  NmIntAdapter::getOID_802_3_XMIT_MORE_COLLISIONS(void)
{
	return m_Stats->xmit_more_collision;
}

inline ULONG  NmIntAdapter::getOID_802_3_RCV_ERROR_ALIGNMENT(void)
{
	return m_Stats->rcv_err_align;
}

NDIS_STATUS  NmIntAdapter::getOID_802_3_MULTICAST_LIST
	(ETHERNET_ADDRESS *, IN OUT PULONG ByteCount, OUT PULONG BytesNeeded)
{
	return NDIS_STATUS_NOT_SUPPORTED;
}


inline ULONG  NmIntAdapter::getOID_802_3_MAXIMUM_LIST_SIZE(void)
{
	return MAX_MULTICAST_ADDRESSES;
}

inline ETHERNET_ADDRESS   NmIntAdapter::getOID_802_3_CURRENT_ADDRESS(void)
{
	return m_CurrEthAddress;
}

inline ETHERNET_ADDRESS   NmIntAdapter::getOID_802_3_PERMANENT_ADDRESS(void)
{
	return m_PermEthAddress;
}

inline ULONG  NmIntAdapter::getOID_GEN_RCV_NO_BUFFER(void)
{
	return m_GenStats->rcv_no_buffer;
}

inline ULONG  NmIntAdapter::getOID_GEN_RCV_ERROR(void)
{
	return m_GenStats->rcv_error 
		+ m_GenStats->rcv_no_buffer
		+ m_Stats->rcv_err_align
		+ m_Stats->rcv_overrun;
}

inline ULONG  NmIntAdapter::getOID_GEN_RCV_OK(void)
{
	return m_GenStats->rcv_ok;
}

inline ULONG  NmIntAdapter::getOID_GEN_XMIT_ERROR(void)
{
	return m_GenStats->xmit_error
		+ m_Stats->xmit_max_collisions 
		+ m_Stats->xmt_underrun 
		+ m_Stats->xmt_times_crs_lost 
		+ m_Stats->xmt_late_collisions;
}

inline ULONG  NmIntAdapter::getOID_GEN_XMIT_OK(void)
{
	return m_GenStats->xmit_ok;
}

inline ULONG  NmIntAdapter::getOID_GEN_MAXIMUM_SEND_PACKETS(void)
{
	return MAX_ARRAY_SEND_PACKETS;
}

inline NDIS_MEDIA_STATE  NmIntAdapter::getOID_GEN_MEDIA_CONNECT_STATUS(void)
{
	NDIS_MEDIA_STATE State = NdisMediaStateDisconnected;

	// prevent bindings from being destroyed
	IncBindCount();

	NmIntBinding** p = GetBindings();
	if (p && p[0] && p[0]->IsUp()) {
		// get the connection status from the the underlying NIC
		State = (NDIS_MEDIA_STATE) p[0]->GetOid(OID_GEN_MEDIA_CONNECT_STATUS);
	} 

	DecBindCount();

	return State;
}

inline ULONG  NmIntAdapter::getOID_GEN_MAC_OPTIONS(void)
{
	return	NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
			NDIS_MAC_OPTION_TRANSFERS_NOT_PEND |
			NDIS_MAC_OPTION_NO_LOOPBACK;
}

inline ULONG  NmIntAdapter::getOID_GEN_MAXIMUM_TOTAL_SIZE(void)
{
	return MAXIMUM_ETHERNET_PACKET_SIZE;
}

inline USHORT NmIntAdapter::getOID_GEN_DRIVER_VERSION(void)
{
	return KNdisAdapterTraits<NmIntAdapter>::NdisVersion();
}

inline ULONG  NmIntAdapter::getOID_GEN_CURRENT_LOOKAHEAD(void)
{
	return m_MaxLookAhead - ETHERNET_HEADER_SIZE;
}

inline ULONG  NmIntAdapter::getOID_GEN_CURRENT_PACKET_FILTER(void)
{
	return m_PacketFilter;
}

NDIS_STATUS  NmIntAdapter::getOID_GEN_VENDOR_DESCRIPTION
	(char* pDesc, IN OUT PULONG ByteCount, OUT PULONG BytesNeeded)
{
	static char Desc[] = "KNdis Intermediate NmInt Driver";
	if ((*BytesNeeded = sizeof(Desc)) > *ByteCount) {
		*ByteCount = 0; 
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}
	NdisMoveMemory(pDesc, Desc, (*ByteCount = sizeof(Desc)));
	return NDIS_STATUS_SUCCESS; 
}

inline ULONG  NmIntAdapter::getOID_GEN_VENDOR_ID(void)
{
	// This OID specifies a three-byte IEEE-registered vendor code.
	ETHERNET_ADDRESS addr = getOID_802_3_PERMANENT_ADDRESS();
	ULONG id = addr;			// get 1st 4 bytes
	* (PUCHAR(&id)+3) = 0;		// my "NIC" id
	return id;
}

inline ULONG NmIntAdapter::getOID_GEN_VENDOR_DRIVER_VERSION()
{
	return (NMINT_VERSION_MAJOR<<8) | NMINT_VERSION_MINOR;
}

inline ULONG  NmIntAdapter::getOID_GEN_RECEIVE_BLOCK_SIZE(void)
{
	return MAXIMUM_ETHERNET_PACKET_SIZE;
}

ULONG  NmIntAdapter::getOID_GEN_TRANSMIT_BLOCK_SIZE(void)
{
	return MAXIMUM_ETHERNET_PACKET_SIZE;
}

inline ULONG  NmIntAdapter::getOID_GEN_RECEIVE_BUFFER_SPACE(void)
{
	return MAXIMUM_ETHERNET_PACKET_SIZE * m_MaxPoolSize;
}

inline ULONG  NmIntAdapter::getOID_GEN_TRANSMIT_BUFFER_SPACE(void)
{
	return MAXIMUM_ETHERNET_PACKET_SIZE * m_MaxPoolSize;
}

ULONG  NmIntAdapter::getOID_GEN_LINK_SPEED(void)
{
	return m_LinkSpeed;
}

ULONG  NmIntAdapter::getOID_GEN_MAXIMUM_FRAME_SIZE(void)
{
	return MAXIMUM_ETHERNET_PACKET_SIZE - ETHERNET_HEADER_SIZE;
}

ULONG  NmIntAdapter::getOID_GEN_MAXIMUM_LOOKAHEAD(void)
{
	return m_MaxLookAhead;
}

NDIS_STATUS  NmIntAdapter::getOID_GEN_MEDIA_SUPPORTED
	(NDIS_MEDIUM* pMedium, PULONG ByteCount, OUT PULONG BytesNeeded) 
{
	// supporting  Ethernet only
	static NDIS_MEDIUM medium[] = { NdisMedium802_3 };

	if ( (*BytesNeeded = sizeof(medium)) > *ByteCount) {
		*ByteCount = 0;
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}
	NdisMoveMemory(pMedium, medium, (*ByteCount = sizeof(medium)));
	return NDIS_STATUS_SUCCESS; 
}

NDIS_STATUS  NmIntAdapter::getOID_GEN_MEDIA_IN_USE
	(NDIS_MEDIUM* pMedium, PULONG ByteCount, OUT PULONG BytesNeeded) 
{
	if ( (*BytesNeeded = sizeof(NDIS_MEDIUM)) > *ByteCount) {
		*ByteCount = 0;
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}
	*pMedium = NdisMedium802_3;
	*ByteCount = sizeof(NDIS_MEDIUM);
	return NDIS_STATUS_SUCCESS; 
}


//=============================================================================
// OID Set Handlers
NDIS_STATUS  NmIntAdapter::setOID_802_3_MULTICAST_LIST
	(ETHERNET_ADDRESS * pEA, PULONG ByteCount, OUT PULONG BytesNeeded) 
{
	// The data must be a multiple of the Ethernet address size.
	if ( (NULL == ByteCount) || (NULL == BytesNeeded) ) {
		return NDIS_STATUS_INVALID_DATA;
	}

	// The data must be a multiple of the Ethernet address size.
	if ( (*ByteCount % ETH_LENGTH_OF_ADDRESS != 0) ) {
		*BytesNeeded = ETH_LENGTH_OF_ADDRESS;
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(*ByteCount / ETH_LENGTH_OF_ADDRESS <= MAX_MULTICAST_ADDRESSES);

	if ( 0 != *ByteCount ) {
		// Save these new MAC addresses to our adapter object
		NdisMoveMemory(
			m_MulticastAddrs,
			pEA,
			*ByteCount
			);
	}

	// Save the number of MAC address in our adapter object
	m_NumMacAddr = *ByteCount / ETH_LENGTH_OF_ADDRESS;

	*BytesNeeded = *ByteCount;

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS  NmIntAdapter::setOID_GEN_PROTOCOL_OPTIONS(PULONG Param)
{
	return NDIS_STATUS_NOT_SUPPORTED;
}

NDIS_STATUS  NmIntAdapter::setOID_GEN_CURRENT_PACKET_FILTER(PULONG Param)
{
	NDIS_STATUS Status = NDIS_STATUS_FAILURE;

	if( Param ) {

		m_PacketFilter = *Param;
		TRACE("NmIntAdapter::setOID_GEN_CURRENT_PACKET_FILTER  Filter=%X\n",m_PacketFilter);

#ifdef GENERATE_MAC_ADDRESS
		Status = NDIS_STATUS_SUCCESS;
#else
		Status = SetNicPacketFilter(Param);
#endif

	}

	return Status;
}

NDIS_STATUS  NmIntAdapter::setOID_GEN_CURRENT_LOOKAHEAD(PULONG Param)
{
	return NDIS_STATUS_SUCCESS;	// ignore for now
}


//=============================================================================
// PnP Handlers

#if (KNDIS_PNP_AWARE)

NDIS_STATUS NmIntAdapter::getOID_PNP_QUERY_POWER(PNDIS_DEVICE_POWER_STATE DesiredState)
{
	// The DDK states:
	// "An intermediate driver must always return NDIS_STATUS_SUCCESS to a query of 
	// OID_PNP_QUERY_POWER. An intermediate driver should never propagate an OID_PNP_QUERY_POWER
	// request to an underlying miniport". Amen.
	//
	UNREFERENCED_PARAMETER(DesiredState);
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS NmIntAdapter::setOID_PNP_SET_POWER(PNDIS_DEVICE_POWER_STATE DesiredState)
{
	// The DDK states:
	// "An intermediate driver must always return NDIS_STATUS_SUCCESS to a query of 
	// OID_PNP_SET_POWER. An intermediate driver should never propagate an OID_PNP_SET_POWER 
	// request to an underlying miniport". Amen.

	// switch to the specified lower-power state. Can't fail.

	if (m_Power == NdisDeviceStateD0 && *DesiredState != NdisDeviceStateD0)
	{
		// @@TODO handle powering down
	}
	else if (m_Power != NdisDeviceStateD0 && *DesiredState == NdisDeviceStateD0)
	{
		// @@TODO handle powering up
	}
	m_Power = *DesiredState;

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS NmIntAdapter::setOID_PNP_ADD_WAKE_UP_PATTERN(PNDIS_PM_PACKET_PATTERN Pattern)
{
	// The DDK states:
	// "An intermediate driver whose upper edge receives this OID request must always 
	// propagate the request to the underlying miniport"
	
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

	// prevent bindings from being destroyed
	IncBindCount();

	NmIntBinding** p = GetBindings();
	if (p && p[0] && p[0]->IsUp()) {

		Status = p[0]->SetOid(OID_PNP_ADD_WAKE_UP_PATTERN, (PULONG) Pattern);
	}

	DecBindCount();

	return Status;
}

NDIS_STATUS NmIntAdapter::setOID_PNP_REMOVE_WAKE_UP_PATTERN(PNDIS_PM_PACKET_PATTERN Pattern)
{
	// The DDK states:
	// "An intermediate driver whose upper edge receives this OID request must always 
	// propagate the request to the underlying miniport"

	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

	// prevent bindings from being destroyed
	IncBindCount();

	NmIntBinding** p = GetBindings();
	if (p && p[0] && p[0]->IsUp()) {

		Status = p[0]->SetOid(OID_PNP_REMOVE_WAKE_UP_PATTERN, (PULONG) Pattern);
	}

	DecBindCount();

	return Status;
}

ULONG NmIntAdapter::getOID_PNP_ENABLE_WAKE_UP()
{
	// The DDK states:
	// "An intermediate driver whose upper edge receives this OID request must always 
	// propagate the request to the underlying miniport"

	ULONG ulRet = 0;

	// prevent bindings from being destroyed
	IncBindCount();

	NmIntBinding** p = GetBindings();
	if (p && p[0] && p[0]->IsUp()) {

		ulRet = p[0]->GetOid(OID_PNP_ENABLE_WAKE_UP);
	}

	DecBindCount();

	return ulRet;
}

NDIS_STATUS NmIntAdapter::setOID_PNP_ENABLE_WAKE_UP(PULONG uMask)
{
	// The DDK states:
	// "An intermediate driver whose upper edge receives this OID request must always 
	// propagate the request to the underlying miniport"

	NDIS_STATUS Status = NDIS_STATUS_FAILURE;

	// prevent bindings from being destroyed
	IncBindCount();

	NmIntBinding** p = GetBindings();
	if (p && p[0] && p[0]->IsUp()) {

		Status = p[0]->SetOid(OID_PNP_ENABLE_WAKE_UP, uMask);
	}

	DecBindCount();

	return Status;
}


#endif // KNDIS_PNP_AWARE
 