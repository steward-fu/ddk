// USBNICOids.cpp	Definition of supported OIDs data 
//											- generated by wizard
//===================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2002 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//====================================================================

#include	"USBNICAdap.h"

/////////////////////////////////////////////////////////////////////////////
// List of supported OIDs. 
// NOTES 
//	1. The static variable name is a standartized ("reserved") name in KNDIS
//  2. No compile-time provisions are made to guarantee the acsending order
//	   of the array. There is a run-time assertion in DBG builds, though.
/////////////////////////////////////////////////////////////////////////////
NDIS_OID USBNICAdapter::sm_OID_GEN_SUPPORTED_LIST[] = {
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
	// 802_3 specific
	OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_802_3_RCV_ERROR_ALIGNMENT,
    OID_802_3_XMIT_ONE_COLLISION,
    OID_802_3_XMIT_MORE_COLLISIONS,

#if KNDIS_PNP_AWARE
	OID_PNP_CAPABILITIES,
	OID_PNP_SET_POWER,
	OID_PNP_QUERY_POWER,
	OID_PNP_ADD_WAKE_UP_PATTERN,
    OID_PNP_REMOVE_WAKE_UP_PATTERN,
	OID_PNP_WAKE_UP_PATTERN_LIST,
	OID_PNP_ENABLE_WAKE_UP,
#endif // KNDIS_PNP_AWARE

};


/////////////////////////////////////////////////////////////////////////////
// OID Query Map
/////////////////////////////////////////////////////////////////////////////
//{{AFX_DNW 
// turn on optimization to compile out dead handler references
#pragma optimize("g", on)	
BEGIN_OID_QUERY_MAP(USBNICAdapter)

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
	// 802_3 specific
	OID_QUERY_ENTRY( OID_802_3_PERMANENT_ADDRESS)
    OID_QUERY_ENTRY( OID_802_3_CURRENT_ADDRESS)
    OID_QUERY_ENTRY( OID_802_3_MULTICAST_LIST)
    OID_QUERY_ENTRY( OID_802_3_MAXIMUM_LIST_SIZE)
    OID_QUERY_ENTRY( OID_802_3_RCV_ERROR_ALIGNMENT)
    OID_QUERY_ENTRY( OID_802_3_XMIT_ONE_COLLISION)
    OID_QUERY_ENTRY( OID_802_3_XMIT_MORE_COLLISIONS)


	// PNP
#if KNDIS_PNP_AWARE
	// NOTE: OID_PNP_CAPABILITIES is handled by the framework. See KNdisPnpMode::GetCapabilities().
	//       The driver writer defines custom support for PNP by overriding attributes
	//       KNdisAdapterTraits<USBNICAdapter>::IsPmAware() and 
	//		 KNdisAdapterTraits<USBNICAdapter>::WakeUpExxx() attributes.
	//		 For NDIS 5 builds, IsPmAware() is TRUE by default and WakeUp-s are not supported.
	OID_QUERY_ENTRY( OID_PNP_QUERY_POWER)
	OID_QUERY_ENTRY( OID_PNP_ENABLE_WAKE_UP	)
#endif // KNDIS_PNP_AWARE

END_OID_QUERY_MAP()
#pragma optimize("", on)	// restore default optimization
//}}AFX_DNW


///////////////////////////////////////////////////////////////////////////
// OID Set Map
/////////////////////////////////////////////////////////////////////////////
//{{AFX_DNW 
// turn on optimization to compile out dead handler references
#pragma optimize("g", on)	
BEGIN_OID_SET_MAP(USBNICAdapter)

	// GEN
	OID_SET_ENTRY( OID_GEN_CURRENT_LOOKAHEAD)
	OID_SET_ENTRY( OID_GEN_CURRENT_PACKET_FILTER)
	OID_SET_ENTRY( OID_GEN_PROTOCOL_OPTIONS)
	// 802_3 specific
    OID_SET_ENTRY( OID_802_3_MULTICAST_LIST)


	// PNP
#if KNDIS_PNP_AWARE
    OID_SET_ENTRY( OID_PNP_SET_POWER)
    OID_SET_ENTRY( OID_PNP_ADD_WAKE_UP_PATTERN)
    OID_SET_ENTRY( OID_PNP_REMOVE_WAKE_UP_PATTERN)
    OID_SET_ENTRY( OID_PNP_ENABLE_WAKE_UP)
#endif // KNDIS_PNP_AWARE

END_OID_SET_MAP()
#pragma optimize("", on)	// restore default optimization
//}}AFX_DNW

///////////////////////////////////////////////////////////////////////////
// OID Handlers
// Don't use inlines for debug builds to allow breakpoints on handlers.
#ifdef _inline_
#undef _inline_
#endif
#if DBG
# define _inline_
#else
# define _inline_	inline
#endif 
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// OID Query Handlers


_inline_ NDIS_HARDWARE_STATUS USBNICAdapter::getOID_GEN_HARDWARE_STATUS(void)
{
	return NdisHardwareStatusReady;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_RCV_NO_BUFFER(void)
{
	return m_GenStats->rcv_no_buffer;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_RCV_ERROR(void)
{
	return m_GenStats->rcv_error;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_RCV_OK(void)
{
	return m_GenStats->rcv_ok;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_XMIT_ERROR(void)
{
	return m_GenStats->xmit_error;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_XMIT_OK(void)
{
	return m_GenStats->xmit_ok;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_MAXIMUM_SEND_PACKETS(void)
{
	return USBNIC_MAX_SEND_PACKETS;
}

_inline_ NDIS_MEDIA_STATE  USBNICAdapter::getOID_GEN_MEDIA_CONNECT_STATUS(void)
{
	// TODO:	Read status form h/w
	return NdisMediaStateConnected;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_MAC_OPTIONS(void)
{
	return m_uMacOptions;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_MAXIMUM_TOTAL_SIZE(void)
{
	return USBNIC_MAX_TOTAL_SIZE;
}

_inline_ USHORT USBNICAdapter::getOID_GEN_DRIVER_VERSION(void)
{
	return KNdisAdapterTraits<USBNICAdapter>::NdisVersion();
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_CURRENT_LOOKAHEAD(void)
{
	return USBNIC_MAX_LOOKAHEAD;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_CURRENT_PACKET_FILTER(void)
{
	return m_uPacketFilter;
}

NDIS_STATUS  USBNICAdapter::getOID_GEN_VENDOR_DESCRIPTION
	(char* pDesc, IN OUT PULONG ByteCount, OUT PULONG BytesNeeded)
{
	static char Desc[] = USBNIC_VENDOR_DESCRIPTION;
	if ((*BytesNeeded = sizeof(Desc)) > *ByteCount) {
		*ByteCount = 0; 
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}
	NdisMoveMemory(pDesc, Desc, (*ByteCount = sizeof(Desc)));
	return NDIS_STATUS_SUCCESS; 
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_VENDOR_ID(void)
{
	// This OID specifies a three-byte IEEE-registered vendor code.
	ETHERNET_ADDRESS addr = m_PermanentAddress;
	ULONG id = addr;			// get 1st 4 bytes
	* (PUCHAR(&id)+3) = 0;		// my "NIC" id
	return id;
}

_inline_ ULONG USBNICAdapter::getOID_GEN_VENDOR_DRIVER_VERSION()
{
	return (USBNIC_VERSION_MAJOR<<8) | USBNIC_VERSION_MINOR;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_RECEIVE_BLOCK_SIZE(void)
{
	return USBNIC_RECEIVE_BLOCK;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_TRANSMIT_BLOCK_SIZE(void)
{
	return USBNIC_TRANSMIT_BLOCK;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_RECEIVE_BUFFER_SPACE(void)
{
	return USBNIC_RX_BUFFER_SPACE;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_TRANSMIT_BUFFER_SPACE(void)
{
	return USBNIC_TX_BUFFER_SPACE;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_LINK_SPEED(void)
{
	return USBNIC_MAX_LINKSPEED/100;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_MAXIMUM_FRAME_SIZE(void)
{
	return USBNIC_MAX_FRAMESIZE;
}

_inline_ ULONG  USBNICAdapter::getOID_GEN_MAXIMUM_LOOKAHEAD(void)
{
	return USBNIC_MAX_LOOKAHEAD;
}

NDIS_STATUS  USBNICAdapter::getOID_GEN_MEDIA_SUPPORTED
	(NDIS_MEDIUM* pMedium, PULONG ByteCount, OUT PULONG BytesNeeded) 
{
	// supported medium
	static NDIS_MEDIUM medium[] = { USBNIC_MEDIUM_TYPE };

	if ( (*BytesNeeded = sizeof(medium)) > *ByteCount) {
		*ByteCount = 0;
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}
	NdisMoveMemory(pMedium, medium, (*ByteCount = sizeof(medium)));
	return NDIS_STATUS_SUCCESS; 
}

NDIS_STATUS  USBNICAdapter::getOID_GEN_MEDIA_IN_USE
	(NDIS_MEDIUM* pMedium, PULONG ByteCount, OUT PULONG BytesNeeded) 
{
	if ( (*BytesNeeded = sizeof(NDIS_MEDIUM)) > *ByteCount) {
		*ByteCount = 0;
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}
	*pMedium = USBNIC_MEDIUM_TYPE;
	*ByteCount = sizeof(NDIS_MEDIUM);
	return NDIS_STATUS_SUCCESS; 
}


/////////////////////////////////////////////////////////////////
// OID Set Handlers

NDIS_STATUS  USBNICAdapter::setOID_GEN_PROTOCOL_OPTIONS(PULONG Param)
{
	m_uMacOptions = *Param;
	return NDIS_STATUS_SUCCESS;	// ignore for now
}

NDIS_STATUS  USBNICAdapter::setOID_GEN_CURRENT_PACKET_FILTER(PULONG Param)
{
	m_uPacketFilter = *Param;
	return SetReceiveFilter(m_uPacketFilter);
}

NDIS_STATUS  USBNICAdapter::setOID_GEN_CURRENT_LOOKAHEAD(PULONG pLookahead)
{
	// TODO: If the driver uses partial rcv indications save the value and
	//       honor it when indicating. Drivers indicating full packets shouldn't care.
	if (*pLookahead > USBNIC_MAX_LOOKAHEAD)
		return NDIS_STATUS_INVALID_LENGTH;
	// m_uMaxLookahead = *pLookahead;
	return NDIS_STATUS_SUCCESS;	
}

////////////////////////////////////////////////////////
// Medium specific handlers
//
// 802_3 specific queries

_inline_ ULONG  USBNICAdapter::getOID_802_3_XMIT_ONE_COLLISION(void)
{
	return m_MediumStats->xmit_one_collision;
}

_inline_ ULONG  USBNICAdapter::getOID_802_3_XMIT_MORE_COLLISIONS(void)
{
	return m_MediumStats->xmit_more_collision;
}

_inline_ ULONG  USBNICAdapter::getOID_802_3_RCV_ERROR_ALIGNMENT(void)
{
	return m_MediumStats->rcv_err_align;
}

NDIS_STATUS  USBNICAdapter::getOID_802_3_MULTICAST_LIST
	(ETHERNET_ADDRESS *, IN OUT PULONG ByteCount, OUT PULONG BytesNeeded)
{
	// TODO: Read the mulicast list from the card
	*ByteCount = 0;

	// make sure the list is well-formated and do not exceed capacity
	if (*ByteCount % sizeof(ETHERNET_ADDRESS)) {
		return NDIS_STATUS_INVALID_DATA;
	}

	return NDIS_STATUS_NOT_SUPPORTED;
}


_inline_ ULONG  USBNICAdapter::getOID_802_3_MAXIMUM_LIST_SIZE(void)
{
	return USBNIC_802_3_MAX_LIST_SIZE;
}

_inline_ ETHERNET_ADDRESS   USBNICAdapter::getOID_802_3_CURRENT_ADDRESS(void)
{
	return m_CurrentAddress;
}

_inline_ ETHERNET_ADDRESS   USBNICAdapter::getOID_802_3_PERMANENT_ADDRESS(void)
{
	return 	m_PermanentAddress;
}

// 802_3 specific set

NDIS_STATUS  USBNICAdapter::setOID_802_3_MULTICAST_LIST
	(ETHERNET_ADDRESS *pAddress, PULONG ByteCount, OUT PULONG BytesNeeded) 
{
	// TODO: program the list into the NIC
	UNREFERENCED_PARAMETER(pAddress);

	if (ByteCount == NULL || BytesNeeded == NULL)
		return NDIS_STATUS_INVALID_DATA;

	ULONG nAddr = *ByteCount/sizeof(ETHERNET_ADDRESS); // # of addresses

	if (*ByteCount%sizeof(ETHERNET_ADDRESS)) {
		*ByteCount = 0; 
		*BytesNeeded = nAddr*sizeof(ETHERNET_ADDRESS);
		return NDIS_STATUS_INVALID_LENGTH;
	}

	return NDIS_STATUS_SUCCESS;
}




///////////////////////////////////////////////////////////////
// PnP Hanlders

#if (KNDIS_PNP_AWARE)

NDIS_STATUS USBNICAdapter::getOID_PNP_QUERY_POWER(PNDIS_DEVICE_POWER_STATE DesiredState)
{
	// TODO: Check if it is safe to switch to the specified lower-power state.
	//       On successful return, NDIS will immediately call OID_PNP_SET_POWER
	UNREFERENCED_PARAMETER(DesiredState);
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS USBNICAdapter::setOID_PNP_SET_POWER(PNDIS_DEVICE_POWER_STATE DesiredState)
{
	// TODO: Do switch to the specified lower-power state. Can't fail.

	m_Power = *DesiredState;
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS USBNICAdapter::setOID_PNP_ADD_WAKE_UP_PATTERN(PNDIS_PM_PACKET_PATTERN Pattern)
{
	// TODO: Program the pattern into the NIC. NOTE that this handler can never be
	//       called unless we had specified the "wake-up" support in our 
	//		 KNdisPnpMode<USBNICAdapter> policies.

	KNdisPmPacketPattern pattern(Pattern);

	STRING mask, data;
	pattern.GetMask(mask);
	pattern.GetData(data);

	// ...

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS USBNICAdapter::setOID_PNP_REMOVE_WAKE_UP_PATTERN(PNDIS_PM_PACKET_PATTERN Pattern)
{
	// TODO: Remove the pattern from the NIC. NOTE that this handler can never be
	//       called unless we had specified the "wake-up" support in our 
	//		 KNdisPnpMode<USBNICAdapter> policies.

	KNdisPmPacketPattern pattern(Pattern);

	STRING mask, data;
	pattern.GetMask(mask);
	pattern.GetData(data);

	// ...

	return NDIS_STATUS_SUCCESS;
}

ULONG USBNICAdapter::getOID_PNP_ENABLE_WAKE_UP()
{
	// TODO: Return those wake up methods (a mask) that are safe to turn on now.
	//       NDIS will select one of those in a following set request. Those
	//		 should be a subset of flags retrieved from the m_Power member.

	ULONG uMask = m_Power.GetCurrentWakeUpEnableMask();
	return uMask;
}

NDIS_STATUS USBNICAdapter::setOID_PNP_ENABLE_WAKE_UP(PULONG uMask)
{
	// TODO: Enable wake/disable methods specified in the uMask.
	//       Note that NDIS would never set all 3 simultaneously.

	// "Magic Packet": a packet that contains 16 contiguous copies of the receiving
	// NIC's Phys address
	if (*uMask & NDIS_PNP_WAKE_UP_MAGIC_PACKET) { // enable
	} else {									 // disable
	}

	// "Pattern Match": signal on receipt of a packet that contains a pattern
	// specified by the protocol with setOID_PNP_ADD_WAKE_UP_PATTERN
	if (*uMask & NDIS_PNP_WAKE_UP_PATTERN_MATCH) { // enable
	} else {									 // disable
	}

	// "Link Change": signal in response to the connection or disconnection of 
	//  the NIC's network cable. 
	if (*uMask & NDIS_PNP_WAKE_UP_LINK_CHANGE) { // enable
	} else {									 // disable
	}


	return NDIS_STATUS_SUCCESS;
}

#endif // KNDIS_PNP_AWARE
 