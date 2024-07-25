// KNdisOidDefs.h: Defines OID handlers
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
// This file is to be included into the declaration of a
// KNdisMiniAdapter-derived class.
//
// This file also serves as input for the Wizard to generate handler
// prototypes for XxxxOids.cpp file.
//

#if !defined(AFX_KNDISOIDDEFS_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_KNDISOIDDEFS_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////
// OID handler definition
//
// OID Queries ("get") might come in one of the 3 forms:
//
//	1) short property (returning a value):			T getXXX();
//	2) short handler (returning NDIS_STATUS):		NDIS_STATUS getXXX(T*);
//  3) variable handler (returning NDIS_STATUS):	NDIS_STATUS getXXX(T*, PULONG, PULONG);
//
// OID Set ("set") might come in one of the 2 forms:
//
//	1) short handler (returning NDIS_STATUS):		NDIS_STATUS setXXX(T*);
//  2) variable handler (returning NDIS_STATUS):	NDIS_STATUS setXXX(T*, PULONG, PULONG);
//
// The naming convention: getOID_XXX_YYY() and setOID_XXX_YYY().
//
// The Query/Set dispatcher chooses one of the 3 forms based on
// the OID properties specified in KNdisOidTraits template.
//
/////////////////////////////////////////////////////////////////////
#define DECLARE_OID_HANDLERS(oid)	\
	KNdisOid<oid>::GetValueType get##oid();						\
	NDIS_STATUS get##oid(IN OUT KNdisOid<oid>::Type*);	\
	NDIS_STATUS get##oid(IN OUT KNdisOid<oid>::Type* , IN OUT PULONG ByteCount, OUT PULONG BytesNeeded); \
	NDIS_STATUS set##oid(IN KNdisOid<oid>::Type*);	\
	NDIS_STATUS set##oid(IN OUT KNdisOid<oid>::Type* , IN OUT PULONG ByteCount, OUT PULONG BytesNeeded);


//////////////////////////////////////////////////////////////////////
// Declare Handler Prototypes for ALL NDIS OIDs.
// Those get included in the declaration of a KNdisMiniAdapter-
// derived class.
//
// TODO: ifdef based on media types declared
//
/////////////////////////////////////////////////////////////////////

//////////
// GENERAL
DECLARE_OID_HANDLERS ( OID_GEN_HARDWARE_STATUS)
DECLARE_OID_HANDLERS ( OID_GEN_MEDIA_SUPPORTED)
DECLARE_OID_HANDLERS ( OID_GEN_MEDIA_IN_USE)
DECLARE_OID_HANDLERS ( OID_GEN_MAXIMUM_LOOKAHEAD)
DECLARE_OID_HANDLERS ( OID_GEN_MAXIMUM_FRAME_SIZE)
DECLARE_OID_HANDLERS ( OID_GEN_LINK_SPEED)
DECLARE_OID_HANDLERS ( OID_GEN_TRANSMIT_BUFFER_SPACE)
DECLARE_OID_HANDLERS ( OID_GEN_RECEIVE_BUFFER_SPACE)
DECLARE_OID_HANDLERS ( OID_GEN_TRANSMIT_BLOCK_SIZE)
DECLARE_OID_HANDLERS ( OID_GEN_RECEIVE_BLOCK_SIZE)
DECLARE_OID_HANDLERS ( OID_GEN_VENDOR_ID)
DECLARE_OID_HANDLERS ( OID_GEN_VENDOR_DESCRIPTION)
DECLARE_OID_HANDLERS ( OID_GEN_CURRENT_PACKET_FILTER)
DECLARE_OID_HANDLERS ( OID_GEN_CURRENT_LOOKAHEAD)
DECLARE_OID_HANDLERS ( OID_GEN_DRIVER_VERSION)
DECLARE_OID_HANDLERS ( OID_GEN_MAXIMUM_TOTAL_SIZE)
DECLARE_OID_HANDLERS ( OID_GEN_PROTOCOL_OPTIONS)
DECLARE_OID_HANDLERS ( OID_GEN_MAC_OPTIONS)
DECLARE_OID_HANDLERS ( OID_GEN_MEDIA_CONNECT_STATUS)
DECLARE_OID_HANDLERS ( OID_GEN_MAXIMUM_SEND_PACKETS)
DECLARE_OID_HANDLERS ( OID_GEN_VENDOR_DRIVER_VERSION)
#if (KNDIS_DDK_VERSION >= 5)
DECLARE_OID_HANDLERS ( OID_GEN_SUPPORTED_GUIDS)
DECLARE_OID_HANDLERS ( OID_GEN_NETWORK_LAYER_ADDRESSES)
DECLARE_OID_HANDLERS ( OID_GEN_TRANSPORT_HEADER_OFFSET)
DECLARE_OID_HANDLERS ( OID_GEN_PHYSICAL_MEDIUM)
DECLARE_OID_HANDLERS ( OID_GEN_MEDIA_CAPABILITIES)
#endif
#if (KNDIS_DDK_VERSION > 5)
DECLARE_OID_HANDLERS ( OID_GEN_MACHINE_NAME)
#endif

DECLARE_OID_HANDLERS ( OID_GEN_XMIT_OK)
DECLARE_OID_HANDLERS ( OID_GEN_RCV_OK)
DECLARE_OID_HANDLERS ( OID_GEN_XMIT_ERROR)
DECLARE_OID_HANDLERS ( OID_GEN_RCV_ERROR)
DECLARE_OID_HANDLERS ( OID_GEN_RCV_NO_BUFFER)

DECLARE_OID_HANDLERS ( OID_GEN_DIRECTED_BYTES_XMIT)
DECLARE_OID_HANDLERS ( OID_GEN_DIRECTED_FRAMES_XMIT)
DECLARE_OID_HANDLERS ( OID_GEN_MULTICAST_BYTES_XMIT)
DECLARE_OID_HANDLERS ( OID_GEN_MULTICAST_FRAMES_XMIT)
DECLARE_OID_HANDLERS ( OID_GEN_BROADCAST_BYTES_XMIT)
DECLARE_OID_HANDLERS ( OID_GEN_BROADCAST_FRAMES_XMIT)
DECLARE_OID_HANDLERS ( OID_GEN_DIRECTED_BYTES_RCV)
DECLARE_OID_HANDLERS ( OID_GEN_DIRECTED_FRAMES_RCV)
DECLARE_OID_HANDLERS ( OID_GEN_MULTICAST_BYTES_RCV)
DECLARE_OID_HANDLERS ( OID_GEN_MULTICAST_FRAMES_RCV)
DECLARE_OID_HANDLERS ( OID_GEN_BROADCAST_BYTES_RCV)
DECLARE_OID_HANDLERS ( OID_GEN_BROADCAST_FRAMES_RCV)

DECLARE_OID_HANDLERS ( OID_GEN_RCV_CRC_ERROR)
DECLARE_OID_HANDLERS ( OID_GEN_TRANSMIT_QUEUE_LENGTH)

#if (KNDIS_DDK_VERSION >= 5)
DECLARE_OID_HANDLERS ( OID_GEN_GET_TIME_CAPS)
DECLARE_OID_HANDLERS ( OID_GEN_GET_NETCARD_TIME)

DECLARE_OID_HANDLERS ( OID_GEN_NETCARD_LOAD)
DECLARE_OID_HANDLERS ( OID_GEN_DEVICE_PROFILE)

DECLARE_OID_HANDLERS ( OID_GEN_INIT_TIME_MS)
DECLARE_OID_HANDLERS ( OID_GEN_RESET_COUNTS)
DECLARE_OID_HANDLERS ( OID_GEN_MEDIA_SENSE_COUNTS)
#endif

////////////////////////////////////
// GEN_CO_ are mapped to normal GEN_

///////////////////////////////////
// Ethertnet
DECLARE_OID_HANDLERS ( OID_802_3_PERMANENT_ADDRESS)
DECLARE_OID_HANDLERS ( OID_802_3_CURRENT_ADDRESS)
DECLARE_OID_HANDLERS ( OID_802_3_MULTICAST_LIST)
DECLARE_OID_HANDLERS ( OID_802_3_MAXIMUM_LIST_SIZE)
DECLARE_OID_HANDLERS ( OID_802_3_MAC_OPTIONS)

DECLARE_OID_HANDLERS ( OID_802_3_RCV_ERROR_ALIGNMENT)
DECLARE_OID_HANDLERS ( OID_802_3_XMIT_ONE_COLLISION)
DECLARE_OID_HANDLERS ( OID_802_3_XMIT_MORE_COLLISIONS)

DECLARE_OID_HANDLERS ( OID_802_3_XMIT_DEFERRED)
DECLARE_OID_HANDLERS ( OID_802_3_XMIT_MAX_COLLISIONS)
DECLARE_OID_HANDLERS ( OID_802_3_RCV_OVERRUN)
DECLARE_OID_HANDLERS ( OID_802_3_XMIT_UNDERRUN)
DECLARE_OID_HANDLERS ( OID_802_3_XMIT_HEARTBEAT_FAILURE)
DECLARE_OID_HANDLERS ( OID_802_3_XMIT_TIMES_CRS_LOST)
DECLARE_OID_HANDLERS ( OID_802_3_XMIT_LATE_COLLISIONS)

////////////////////////////////////
//	PnP and PM OIDs
//
#if (KNDIS_PNP_AWARE)
DECLARE_OID_HANDLERS( OID_PNP_CAPABILITIES	)
DECLARE_OID_HANDLERS( OID_PNP_SET_POWER		)
DECLARE_OID_HANDLERS( OID_PNP_QUERY_POWER	)
DECLARE_OID_HANDLERS( OID_PNP_ADD_WAKE_UP_PATTERN)
DECLARE_OID_HANDLERS( OID_PNP_REMOVE_WAKE_UP_PATTERN)
// DECLARE_OID_HANDLERS( OID_PNP_WAKE_UP_PATTERN_LIST) -- not handled by miniports
DECLARE_OID_HANDLERS( OID_PNP_ENABLE_WAKE_UP	)
DECLARE_OID_HANDLERS( OID_PNP_WAKE_UP_OK		)
DECLARE_OID_HANDLERS( OID_PNP_WAKE_UP_ERROR	)
#endif

///////////////////////////////////
// Other Mediums
#include <KNdisOidDefs4.h>
#if KNDIS_DDK_VERSION >= 5
#include <KNdisOidDefs5.h>
#endif

/////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////
// Add framework's FRIEND classes that can access protected methods of
// the adapters. Piggyback on the fact that this file is to be included
// in the adapter class declaration.
//

#endif // !defined(AFX_KNDISOIDDEFS_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_)
