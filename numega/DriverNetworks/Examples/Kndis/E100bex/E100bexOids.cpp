// E100bexOids.cpp: Definition of supported OIDs data.
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
// Portions copied from Microsoft Windows 2000 DDK sample driver containing the
// following copyright
//
/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/


#include <kndis.h>
#include "E100bexInc.h"


/////////////////////////////////////////////////////////////////////////////
// List of supported OIDs.
// NOTES
//	1. The static variable name is a standardized ("reserved") name in KNDIS
//  2. No compile-time provisions are made to guarantee the acsending order
//	   of the array. There is a run-time assertion in DBG builds, though.
/////////////////////////////////////////////////////////////////////////////
NDIS_OID E100bexAdapter::sm_OID_GEN_SUPPORTED_LIST[] = {
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
	OID_GEN_SUPPORTED_GUIDS,
#if (KNDIS_DDK_VERSION >= 5)
	OID_GEN_PHYSICAL_MEDIUM,
#endif
	OID_GEN_XMIT_OK,
	OID_GEN_RCV_OK,
	OID_GEN_XMIT_ERROR,
	OID_GEN_RCV_ERROR,
	OID_GEN_RCV_NO_BUFFER,
	OID_GEN_RCV_CRC_ERROR,
		//OID_GEN_TRANSMIT_QUEUE_LENGTH,
	OID_802_3_PERMANENT_ADDRESS,
	OID_802_3_CURRENT_ADDRESS,
	OID_802_3_MULTICAST_LIST,
	OID_802_3_MAXIMUM_LIST_SIZE,
	OID_802_3_RCV_ERROR_ALIGNMENT,
	OID_802_3_XMIT_ONE_COLLISION,
	OID_802_3_XMIT_MORE_COLLISIONS,
		//OID_802_3_XMIT_DEFERRED,
		//OID_802_3_XMIT_MAX_COLLISIONS,
		//OID_802_3_RCV_OVERRUN,
		//OID_802_3_XMIT_UNDERRUN,
		//OID_802_3_XMIT_HEARTBEAT_FAILURE,
		//OID_802_3_XMIT_TIMES_CRS_LOST,
		//OID_802_3_XMIT_LATE_COLLISIONS,
#if (KNDIS_PNP_AWARE && E100BEX_SUPPORT_PNP)
	OID_PNP_CAPABILITIES,
	OID_PNP_SET_POWER,
	OID_PNP_QUERY_POWER,
// This NIC does not have Wake-up capabilities so these are commented out.
//	OID_PNP_ADD_WAKE_UP_PATTERN,
//  OID_PNP_REMOVE_WAKE_UP_PATTERN,
//	OID_PNP_WAKE_UP_PATTERN_LIST,
//	OID_PNP_ENABLE_WAKE_UP,
#endif // KNDIS_PNP_AWARE

};


/////////////////////////////////////////////////////////////////////////////
// OID Query Map
/////////////////////////////////////////////////////////////////////////////
//{{AFX_DNW

// turn on optimization to compile out dead handler references
#pragma optimize("g", on)
BEGIN_OID_QUERY_MAP(E100bexAdapter)

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

	OID_QUERY_ENTRY( OID_GEN_SUPPORTED_GUIDS)

	OID_QUERY_ENTRY( OID_GEN_XMIT_OK)
	OID_QUERY_ENTRY( OID_GEN_RCV_OK)
	OID_QUERY_ENTRY( OID_GEN_XMIT_ERROR)
	OID_QUERY_ENTRY( OID_GEN_RCV_ERROR)
	OID_QUERY_ENTRY( OID_GEN_RCV_NO_BUFFER)
	OID_QUERY_ENTRY( OID_GEN_RCV_CRC_ERROR)

	// 802_3
	OID_QUERY_ENTRY( OID_802_3_PERMANENT_ADDRESS)
	OID_QUERY_ENTRY( OID_802_3_CURRENT_ADDRESS)
	OID_QUERY_ENTRY( OID_802_3_MULTICAST_LIST)
	OID_QUERY_ENTRY( OID_802_3_MAXIMUM_LIST_SIZE)
	OID_QUERY_ENTRY( OID_802_3_RCV_ERROR_ALIGNMENT)
	OID_QUERY_ENTRY( OID_802_3_XMIT_ONE_COLLISION)
	OID_QUERY_ENTRY( OID_802_3_XMIT_MORE_COLLISIONS)

#if (KNDIS_DDK_VERSION >= 5)
	OID_QUERY_ENTRY( OID_GEN_PHYSICAL_MEDIUM)
#endif

	// PNP
#if (KNDIS_PNP_AWARE && E100BEX_SUPPORT_PNP)
	OID_QUERY_ENTRY( OID_PNP_QUERY_POWER)
// This NIC does not have Wake-up capabilities so these are commented out.
//	OID_QUERY_ENTRY( OID_PNP_ENABLE_WAKE_UP	)
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
BEGIN_OID_SET_MAP(E100bexAdapter)

	// GEN
	OID_SET_ENTRY( OID_GEN_CURRENT_LOOKAHEAD)
	OID_SET_ENTRY( OID_GEN_CURRENT_PACKET_FILTER)
	OID_SET_ENTRY( OID_GEN_PROTOCOL_OPTIONS)

	// 802.3
	OID_SET_ENTRY( OID_802_3_MULTICAST_LIST)

	// PNP
#if (KNDIS_PNP_AWARE && E100BEX_SUPPORT_PNP)
	OID_SET_ENTRY( OID_PNP_SET_POWER)
// This NIC does not have Wake-up capabilities so these are commented out.
//    OID_SET_ENTRY( OID_PNP_ADD_WAKE_UP_PATTERN)
//    OID_SET_ENTRY( OID_PNP_REMOVE_WAKE_UP_PATTERN)
//    OID_SET_ENTRY( OID_PNP_ENABLE_WAKE_UP)
#endif // KNDIS_PNP_AWARE

END_OID_SET_MAP()
#pragma optimize("", on)	// restore default optimization
//}}AFX_DNW

///////////////////////////////////////////////////////////////////////////
// OID Handlers
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// OID Query Handlers

inline NDIS_HARDWARE_STATUS E100bexAdapter::getOID_GEN_HARDWARE_STATUS(void)
{
	return NdisHardwareStatusReady;
}

inline ULONG  E100bexAdapter::getOID_802_3_XMIT_ONE_COLLISION(void)
{
	m_pCard->DumpStatsCounters(m_Stats);
	return m_Stats->XmtSingleCollision;
}

inline ULONG  E100bexAdapter::getOID_802_3_XMIT_MORE_COLLISIONS(void)
{
	m_pCard->DumpStatsCounters(m_Stats);
	return m_Stats->XmtMultCollisions;
}

inline ULONG  E100bexAdapter::getOID_802_3_RCV_ERROR_ALIGNMENT(void)
{
	m_pCard->DumpStatsCounters(m_Stats);
	return m_Stats->RcvAlignmentErrors;
}

NDIS_STATUS  E100bexAdapter::getOID_802_3_MULTICAST_LIST
	(ETHERNET_ADDRESS *, IN OUT PULONG ByteCount, OUT PULONG BytesNeeded)
{
	return NDIS_STATUS_NOT_SUPPORTED;
}


inline ULONG  E100bexAdapter::getOID_802_3_MAXIMUM_LIST_SIZE(void)
{
	return NIC_MAX_MCAST_LIST;
}

inline ETHERNET_ADDRESS   E100bexAdapter::getOID_802_3_CURRENT_ADDRESS(void)
{
	return m_AiNodeAddress;
}

inline ETHERNET_ADDRESS   E100bexAdapter::getOID_802_3_PERMANENT_ADDRESS(void)
{
	return m_pCard->ReadPermanentNodeAddress();
}

inline ULONG  E100bexAdapter::getOID_GEN_RCV_NO_BUFFER(void)
{
	m_pCard->DumpStatsCounters(m_Stats);
	return m_Stats->RcvResourceErrors;
}

inline ULONG  E100bexAdapter::getOID_GEN_RCV_CRC_ERROR(void)
{
	m_pCard->DumpStatsCounters(m_Stats);
	return m_Stats->RcvCrcErrors;
}

inline ULONG  E100bexAdapter::getOID_GEN_RCV_ERROR(void)
{
	m_pCard->DumpStatsCounters(m_Stats);
	return m_Stats->RcvCrcErrors
		+ m_Stats->RcvAlignmentErrors
		+ m_Stats->RcvResourceErrors
		+ m_Stats->RcvOverrunErrors
		+ m_Stats->RcvShortFrames;
}

inline ULONG  E100bexAdapter::getOID_GEN_RCV_OK(void)
{
	m_pCard->DumpStatsCounters(m_Stats);
	return m_Stats->RcvGoodFrames;
}

inline ULONG  E100bexAdapter::getOID_GEN_XMIT_ERROR(void)
{
	m_pCard->DumpStatsCounters(m_Stats);
	return m_Stats->XmtMaxCollisions
		+ m_Stats->XmtUnderruns
		+ m_Stats->XmtLostCRS
		+ m_Stats->XmtLateCollisions;
}

inline ULONG  E100bexAdapter::getOID_GEN_XMIT_OK(void)
{
	m_pCard->DumpStatsCounters(m_Stats);
	return m_Stats->XmtGoodFrames;
}

inline ULONG  E100bexAdapter::getOID_GEN_MAXIMUM_SEND_PACKETS(void)
{
	return MAX_ARRAY_SEND_PACKETS;
}

inline NDIS_MEDIA_STATE  E100bexAdapter::getOID_GEN_MEDIA_CONNECT_STATUS(void)
{
	if ( m_LinkIsActive != m_pCard->GetConnectionStatus() )
	{
		// if status has changed
		switch ( m_LinkIsActive )
		{
		case NdisMediaStateConnected:
			// changing from connected
			m_LinkIsActive = NdisMediaStateDisconnected;
			break;

		case NdisMediaStateDisconnected:
			// changing from disconnected
			m_LinkIsActive = NdisMediaStateConnected;
			break;
		}
	}

	return m_LinkIsActive;
}

inline ULONG  E100bexAdapter::getOID_GEN_MAC_OPTIONS(void)
{
	return m_uMacOptions;
}

inline ULONG  E100bexAdapter::getOID_GEN_MAXIMUM_TOTAL_SIZE(void)
{
	return NIC_MAX_PACKET_SIZE;
}

inline USHORT E100bexAdapter::getOID_GEN_DRIVER_VERSION(void)
{
	return KNdisAdapterTraits<E100bexAdapter>::NdisVersion();
}

inline ULONG  E100bexAdapter::getOID_GEN_CURRENT_LOOKAHEAD(void)
{
	return NIC_MAX_PACKET_SIZE - NIC_HEADER_SIZE;
}

inline ULONG  E100bexAdapter::getOID_GEN_CURRENT_PACKET_FILTER(void)
{
	if( m_pCard )
	{
		return m_pCard->GetPacketFilter();
	}
	else
	{
		ASSERT( m_pCard );
		return 0;
	}
}

NDIS_STATUS  E100bexAdapter::getOID_GEN_VENDOR_DESCRIPTION
	(char* pDesc, IN OUT PULONG ByteCount, OUT PULONG BytesNeeded)
{
	static char Desc[] = "KNdis Miniport E100bex Driver";
	if ((*BytesNeeded = sizeof(Desc)) > *ByteCount) {
		*ByteCount = 0;
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}
	NdisMoveMemory(pDesc, Desc, (*ByteCount = sizeof(Desc)));
	return NDIS_STATUS_SUCCESS;
}

inline ULONG  E100bexAdapter::getOID_GEN_VENDOR_ID(void)
{
	// This OID specifies a three-byte IEEE-registered vendor code.
	ETHERNET_ADDRESS addr = getOID_802_3_PERMANENT_ADDRESS();
	ULONG id = addr;			// get 1st 4 bytes
	* (PUCHAR(&id)+3) = 0;		// my "NIC" id
	return id;
}

inline ULONG E100bexAdapter::getOID_GEN_VENDOR_DRIVER_VERSION()
{
	return (E100BEX_VERSION_MAJOR<<8) | E100BEX_VERSION_MINOR;
}

inline ULONG  E100bexAdapter::getOID_GEN_RECEIVE_BLOCK_SIZE(void)
{
	return NIC_MAX_PACKET_SIZE;
}

ULONG  E100bexAdapter::getOID_GEN_TRANSMIT_BLOCK_SIZE(void)
{
	return NIC_MAX_PACKET_SIZE;
}

inline ULONG  E100bexAdapter::getOID_GEN_RECEIVE_BUFFER_SPACE(void)
{
	return NIC_MAX_PACKET_SIZE * m_pRxArea->GetNumberDescriptors();
}

inline ULONG  E100bexAdapter::getOID_GEN_TRANSMIT_BUFFER_SPACE(void)
{
	return NIC_MAX_PACKET_SIZE * m_RegNumTcb;
}

ULONG  E100bexAdapter::getOID_GEN_LINK_SPEED(void)
{
	// The driver does not always know what is current line speed, but
	// reports the value that the we detected when the driver loaded.
	return m_pCard ? (m_pCard->GetCurrentLineSpeed() * 10000) : 0;
}

ULONG  E100bexAdapter::getOID_GEN_MAXIMUM_FRAME_SIZE(void)
{
	return NIC_MAX_PACKET_SIZE - NIC_HEADER_SIZE;
}

ULONG  E100bexAdapter::getOID_GEN_MAXIMUM_LOOKAHEAD(void)
{
	return NIC_MAX_PACKET_SIZE - NIC_HEADER_SIZE;
}

NDIS_STATUS  E100bexAdapter::getOID_GEN_MEDIA_SUPPORTED
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

NDIS_STATUS  E100bexAdapter::getOID_GEN_MEDIA_IN_USE
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


NDIS_STATUS  E100bexAdapter::getOID_GEN_SUPPORTED_GUIDS
	(NDIS_GUID* pGUID, PULONG ByteCount, OUT PULONG BytesNeeded)
{
	// registers the miniport's custom GUIDs with WMI

	// TODO: Error check the supplied parameters
/*
	if ( (*BytesNeeded = sizeof(NDIS_GUID)) > *ByteCount) {
		*ByteCount = 0;
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}
*/
	// TODO: Assign custom GUIDs
	//*pGUID = ;
	//*ByteCount = sizeof(NDIS_GUID);
	*BytesNeeded = 0;
	*ByteCount = 0;
	return NDIS_STATUS_SUCCESS;
}

NDIS_PHYSICAL_MEDIUM  E100bexAdapter::getOID_GEN_PHYSICAL_MEDIUM()
{
	return NdisPhysicalMediumUnspecified;
}


/////////////////////////////////////////////////////////////////
// OID Set Handlers
NDIS_STATUS  E100bexAdapter::setOID_802_3_MULTICAST_LIST
	(ETHERNET_ADDRESS * pEA, PULONG ByteCount, OUT PULONG BytesNeeded)
{
	// The data must be a multiple of the Ethernet address size.
	if ( (*ByteCount % ETH_LENGTH_OF_ADDRESS != 0) )
	{
		return NDIS_STATUS_INVALID_LENGTH;
	}

	// Save the number of MC address in our adapter object
	m_MCAddressCount = *ByteCount / ETH_LENGTH_OF_ADDRESS;
	ASSERT(m_MCAddressCount <= NIC_MAX_MCAST_LIST);

	// Save these new MC addresses to our adapter object
	NdisMoveMemory(
		m_MCList,
		pEA,
		*ByteCount
		);

	*BytesNeeded = *ByteCount;
	m_Lock.LockDpr();
	m_RcvLock.LockDpr();
	// have the hardware change its mulicast address filter
	NDIS_STATUS Status = m_pCard->NICSetMulticastList( &m_MCList[0][0], m_MCAddressCount );
	m_RcvLock.UnlockDpr();
	m_Lock.UnlockDpr();
	return Status;
}

NDIS_STATUS  E100bexAdapter::setOID_GEN_PROTOCOL_OPTIONS(PULONG Param)
{
	return NDIS_STATUS_NOT_SUPPORTED;
}

NDIS_STATUS  E100bexAdapter::setOID_GEN_CURRENT_PACKET_FILTER(PULONG Param)
{
	if( Param && m_pCard && m_pCard->GetPacketFilter() != *Param)
	{
		m_Lock.LockDpr();
		m_RcvLock.LockDpr();
		NDIS_STATUS Status = m_pCard->SetPacketFilter(*Param);
		m_Lock.UnlockDpr();
		m_RcvLock.UnlockDpr();
		return Status;
	}
	else
	{
		ASSERT( Param );
		ASSERT( m_pCard );
		return NDIS_STATUS_FAILURE;
	}
}

NDIS_STATUS  E100bexAdapter::setOID_GEN_CURRENT_LOOKAHEAD(PULONG)
{
	return NDIS_STATUS_SUCCESS;	// ignore for now
}


///////////////////////////////////////////////////////////////
// PnP Handlers

#if (KNDIS_PNP_AWARE && E100BEX_SUPPORT_PNP)

NDIS_STATUS E100bexAdapter::getOID_PNP_QUERY_POWER(PNDIS_DEVICE_POWER_STATE DesiredState)
{
	// TODO: Check if it is safe to switch to the specified lower-power state.
	//       On successful return, NDIS will immediately call OID_PNP_SET_POWER
	UNREFERENCED_PARAMETER(DesiredState);
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS E100bexAdapter::setOID_PNP_SET_POWER(PNDIS_DEVICE_POWER_STATE DesiredState)
{
	// switch to the specified lower-power state. Can't fail.

	if (m_Power == NdisDeviceStateD0 && *DesiredState != NdisDeviceStateD0)
	{
		DisableCard();
	}
	else if (m_Power != NdisDeviceStateD0 && *DesiredState == NdisDeviceStateD0)
	{
		m_Lock.LockDpr();
		m_pCard->SetPacketFilter(m_pCard->GetPacketFilter());
		m_Lock.UnlockDpr();
		NDIS_STATUS Status = EnableCard();
		ASSERT(Status == NDIS_STATUS_SUCCESS);
	}
	m_Power = *DesiredState;

	return NDIS_STATUS_SUCCESS;
}
/*
// This NIC does not have Wake-up capabilities so Wake-up OID handlers are commented out.

NDIS_STATUS E100bexAdapter::setOID_PNP_ADD_WAKE_UP_PATTERN(PNDIS_PM_PACKET_PATTERN Pattern)
{
	// TODO: Program the pattern into the NIC. NOTE that this handler can never be
	//       called unless we had specified the "wake-up" support in our
	//		 KNdisPnpMode<E100bexAdapter> policies.

	KNdisPmPacketPattern pattern(Pattern);

	STRING mask, data;
	pattern.GetMask(mask);
	pattern.GetData(data);

	// ...

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS E100bexAdapter::setOID_PNP_REMOVE_WAKE_UP_PATTERN(PNDIS_PM_PACKET_PATTERN Pattern)
{
	// TODO: Remove the pattern from the NIC. NOTE that this handler can never be
	//       called unless we had specified the "wake-up" support in our
	//		 KNdisPnpMode<E100bexAdapter> policies.

	KNdisPmPacketPattern pattern(Pattern);

	STRING mask, data;
	pattern.GetMask(mask);
	pattern.GetData(data);

	// ...

	return NDIS_STATUS_SUCCESS;
}

ULONG E100bexAdapter::getOID_PNP_ENABLE_WAKE_UP()
{
	// TODO: Return those wake up methods (a mask) that are safe to turn on now.
	//       NDIS will select one of those in a following set request. Those
	//		 should be a subset of flags retrieved from the m_Power member.

	ULONG uMask = m_Power.GetCurrentWakeUpEnableMask();
	return uMask;
}

NDIS_STATUS E100bexAdapter::setOID_PNP_ENABLE_WAKE_UP(PULONG uMask)
{
	// TODO: Enable wake/disable methods specified in the uMask.
	//       Note that NDIS would never set all 3 simultaneously.

	// "Magic Packet": a packet that contains 16 contiguous copies of the receiving
	// NIC's Ethernet address
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
*/
#endif // KNDIS_PNP_AWARE
 