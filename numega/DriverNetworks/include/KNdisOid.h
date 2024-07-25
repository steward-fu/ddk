// KNdisOid.h: Declares OID properties and handler prototypes.
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

#if !defined(AFX_KNDISOID_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_KNDISOID_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

#if (KNDIS_DDK_VERSION >= 5)
#include	<atm.h>
#endif

///////////////////////////////////////////////////////////////////////
// KNdisOidBase
//
// Class KNdisOidBase introduces unmutable properties of NDIS OIDs, which
// define how OID handlers should treat the OID set and query requests.
// Those properties are:
//
//	Oid			-	Oid value (defined in ntddndis.h)
//  T			-	Type of the parameter this Oid describes
//	bVarSize	-	Whether the parameter is of fixed sizeof(T) or variable size
//
// Template class KNdisOid is intended to derive the properties of a specific
// OID based solely on its value. This is crucial for the OID message crackers
// defined elsewhere.
//
// The default implementation of KNdisOid defines properties of most commonly used
// NDIS OIDs, which are fixed size ULONGs.
// For all other OIDs (which are non-ULONGs or var size), template KNdisOid
// must be *specialized*. Macro KNDIS_OID_PROPERTY serves as a shortcut to do that.
//
// NOTE: The user should never bother to specialize or even use KNdisOid templates
//       unless he defines some non-standard OIDs for a private interface between
//		 custom protocol and miniport drivers. The user can, however, specify the
//		 signatures of OID handlers using KNdisOidTraits class; see below.
//
template<const NDIS_OID Oid, typename T, const bool bVarSize>
struct KNdisOidBase
{
// Properties crucial for handlers
	static inline bool IsVarSize() { return bVarSize; }
	typedef T Type;
	typedef T GetValueType;

// Informational Properties derived from the value according to the
// DDK Table 1.1 and Section 2.1. They're not really needed, but good to know.
	static inline NDIS_OID Value() { return Oid; }
	static inline NDIS_MEDIUM Medium() { return (NDIS_MEDIUM) ((Oid>>24)&0xFF)+1; }
	static inline bool IsMandatory() { return ((Oid>>8)&0xF)==1; }
	static inline bool IsStatistic() { return ((Oid>>16)&0xF)==2; }

// can be set?	static inline bool IsMutable()	{ return true; }
};

template<const NDIS_OID Oid>
struct KNdisOid : public KNdisOidBase<Oid, ULONG, false> {};

#define KNDIS_OID_PROPERTY(oid, type, varsize) \
	template<> struct KNdisOid<oid> : public KNdisOidBase<oid, type, varsize> {};

#define KNDIS_OID_PROPERTY_NONBASIC(oid, type, varsize) \
	template<> struct KNdisOid<oid> : public KNdisOidBase<oid, type, varsize> {typedef ULONG GetValueType;};

//////////////////////////////////////////////////////////////////////////
// Properties of non-normal OIDs (which are non-ULONGs or var size)
// These are NDIS properties and, thus, non-overridable.
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////
// GENERAL
KNDIS_OID_PROPERTY ( OID_GEN_HARDWARE_STATUS, NDIS_HARDWARE_STATUS, false)
KNDIS_OID_PROPERTY ( OID_GEN_MEDIA_SUPPORTED, NDIS_MEDIUM, true)
KNDIS_OID_PROPERTY ( OID_GEN_MEDIA_IN_USE, NDIS_MEDIUM, true)
KNDIS_OID_PROPERTY ( OID_GEN_VENDOR_DESCRIPTION, char, true)
KNDIS_OID_PROPERTY ( OID_GEN_DRIVER_VERSION, USHORT, false)
KNDIS_OID_PROPERTY ( OID_GEN_MEDIA_CONNECT_STATUS, NDIS_MEDIA_STATE, false)
#if (KNDIS_DDK_VERSION >= 5)
KNDIS_OID_PROPERTY_NONBASIC ( OID_GEN_SUPPORTED_GUIDS, NDIS_GUID, true)
KNDIS_OID_PROPERTY ( OID_GEN_NETWORK_LAYER_ADDRESSES, NETWORK_ADDRESS_LIST, true)
KNDIS_OID_PROPERTY ( OID_GEN_PHYSICAL_MEDIUM, NDIS_PHYSICAL_MEDIUM, false)
#endif
KNDIS_OID_PROPERTY ( OID_GEN_DIRECTED_BYTES_XMIT, ULONGLONG, false)
KNDIS_OID_PROPERTY ( OID_GEN_MULTICAST_BYTES_XMIT, ULONGLONG, false)
KNDIS_OID_PROPERTY ( OID_GEN_BROADCAST_BYTES_XMIT, ULONGLONG, false)
KNDIS_OID_PROPERTY ( OID_GEN_DIRECTED_BYTES_RCV, ULONGLONG, false)
KNDIS_OID_PROPERTY ( OID_GEN_MULTICAST_BYTES_RCV, ULONGLONG, false)
KNDIS_OID_PROPERTY ( OID_GEN_BROADCAST_BYTES_RCV, ULONGLONG, false)
#if (KNDIS_DDK_VERSION >= 5)
KNDIS_OID_PROPERTY ( OID_GEN_GET_TIME_CAPS, GEN_GET_TIME_CAPS, false)
KNDIS_OID_PROPERTY ( OID_GEN_GET_NETCARD_TIME, GEN_GET_NETCARD_TIME, false)
KNDIS_OID_PROPERTY ( OID_GEN_RESET_COUNTS, PVOID, false)
#endif
#if (KNDIS_DDK_VERSION > 5)
KNDIS_OID_PROPERTY ( OID_GEN_MACHINE_NAME, WCHAR, true)
#endif

////////////////////////////////////
// GEN_CO_ are mapped to normal GEN_
#if defined(CONDIS_MINIPORT) && (KNDIS_DDK_VERSION >= 5)
KNDIS_OID_PROPERTY_NONBASIC ( OID_GEN_CO_LINK_SPEED, NDIS_CO_LINK_SPEED, true)
KNDIS_OID_PROPERTY ( OID_GEN_CO_MINIMUM_LINK_SPEED, NDIS_CO_LINK_SPEED, false)
#endif

///////////////////////////////////
// 802.3
KNDIS_OID_PROPERTY ( OID_802_3_PERMANENT_ADDRESS, ETHERNET_ADDRESS, false)
KNDIS_OID_PROPERTY ( OID_802_3_CURRENT_ADDRESS, ETHERNET_ADDRESS, false)
KNDIS_OID_PROPERTY ( OID_802_3_MULTICAST_LIST, ETHERNET_ADDRESS, true)

//////////////////////////////////
// 802.5
KNDIS_OID_PROPERTY ( OID_802_5_PERMANENT_ADDRESS, TOKENRING_ADDRESS, false)
KNDIS_OID_PROPERTY ( OID_802_5_CURRENT_ADDRESS, TOKENRING_ADDRESS, false)

//////////////////////////////////
// 802.11
#if (KNDIS_DDK_VERSION > 5)
KNDIS_OID_PROPERTY( OID_802_11_BSSID, UCHAR, true)
KNDIS_OID_PROPERTY_NONBASIC( OID_802_11_SSID, NDIS_802_11_SSID, false)
KNDIS_OID_PROPERTY_NONBASIC( OID_802_11_NETWORK_TYPES_SUPPORTED, NDIS_802_11_NETWORK_TYPE_LIST, true)
KNDIS_OID_PROPERTY( OID_802_11_NETWORK_TYPE_IN_USE, NDIS_802_11_NETWORK_TYPE, false)
KNDIS_OID_PROPERTY( OID_802_11_TX_POWER_LEVEL, NDIS_802_11_TX_POWER_LEVEL, false)
KNDIS_OID_PROPERTY( OID_802_11_RSSI, NDIS_802_11_RSSI, false)
KNDIS_OID_PROPERTY( OID_802_11_RSSI_TRIGGER, NDIS_802_11_RSSI, false)
KNDIS_OID_PROPERTY( OID_802_11_INFRASTRUCTURE_MODE, NDIS_802_11_NETWORK_INFRASTRUCTURE, false)
KNDIS_OID_PROPERTY( OID_802_11_FRAGMENTATION_THRESHOLD, NDIS_802_11_FRAGMENTATION_THRESHOLD, false)
KNDIS_OID_PROPERTY( OID_802_11_RTS_THRESHOLD, NDIS_802_11_RTS_THRESHOLD, false)
KNDIS_OID_PROPERTY( OID_802_11_NUMBER_OF_ANTENNAS, ULONG, false)
KNDIS_OID_PROPERTY( OID_802_11_RX_ANTENNA_SELECTED, NDIS_802_11_ANTENNA, false)
KNDIS_OID_PROPERTY( OID_802_11_TX_ANTENNA_SELECTED, NDIS_802_11_ANTENNA, false)
KNDIS_OID_PROPERTY_NONBASIC( OID_802_11_SUPPORTED_RATES, NDIS_802_11_RATES, true)
KNDIS_OID_PROPERTY_NONBASIC( OID_802_11_DESIRED_RATES, NDIS_802_11_RATES, true)
KNDIS_OID_PROPERTY_NONBASIC( OID_802_11_CONFIGURATION, NDIS_802_11_CONFIGURATION, false)
KNDIS_OID_PROPERTY_NONBASIC( OID_802_11_STATISTICS, NDIS_802_11_STATISTICS, false)
KNDIS_OID_PROPERTY( OID_802_11_ADD_WEP, NDIS_802_11_WEP, true)
KNDIS_OID_PROPERTY( OID_802_11_REMOVE_WEP, NDIS_802_11_KEY_INDEX, false)
KNDIS_OID_PROPERTY( OID_802_11_DISASSOCIATE, PVOID, true)
KNDIS_OID_PROPERTY( OID_802_11_POWER_MODE,  NDIS_802_11_POWER_MODE, false)
KNDIS_OID_PROPERTY_NONBASIC( OID_802_11_BSSID_LIST,  NDIS_802_11_BSSID_LIST, true)
KNDIS_OID_PROPERTY( OID_802_11_AUTHENTICATION_MODE,  NDIS_802_11_AUTHENTICATION_MODE, false)
KNDIS_OID_PROPERTY( OID_802_11_PRIVACY_FILTER,  NDIS_802_11_PRIVACY_FILTER, false)
KNDIS_OID_PROPERTY( OID_802_11_BSSID_LIST_SCAN,  PVOID, true)
KNDIS_OID_PROPERTY( OID_802_11_WEP_STATUS,  NDIS_802_11_WEP_STATUS, false)
KNDIS_OID_PROPERTY( OID_802_11_RELOAD_DEFAULTS,  NDIS_802_11_RELOAD_DEFAULTS, false)
#endif

//////////////////////////////////
// ATM
#if (KNDIS_DDK_VERSION >= 5)
KNDIS_OID_PROPERTY ( OID_ATM_SUPPORTED_VC_RATES, ATM_VC_RATES_SUPPORTED, false)
KNDIS_OID_PROPERTY ( OID_ATM_SUPPORTED_SERVICE_CATEGORY, ATM_SERVICE_CATEGORY, false)
KNDIS_OID_PROPERTY ( OID_ATM_SUPPORTED_AAL_TYPES, ATM_AAL_TYPE, false)
KNDIS_OID_PROPERTY ( OID_ATM_HW_CURRENT_ADDRESS, ETHERNET_ADDRESS, false)

KNDIS_OID_PROPERTY_NONBASIC (OID_ATM_SIGNALING_VPIVCI, ATM_VPIVCI, false)
KNDIS_OID_PROPERTY ( OID_ATM_ILMI_VPIVCI, ATM_VPIVCI, false)
KNDIS_OID_PROPERTY ( OID_ATM_LECS_ADDRESS, ATM_ADDRESS, false)
KNDIS_OID_PROPERTY ( OID_ATM_SERVICE_ADDRESS, ATM_SERVICE_ADDRESS_LIST, true)
#endif

/////////////////////////////////////
// TODO: other media types

////////////////////////////////////
//	PnP and PM OIDs
//
#if (KNDIS_PNP_AWARE)
KNDIS_OID_PROPERTY( OID_PNP_CAPABILITIES, NDIS_PNP_CAPABILITIES, false)
KNDIS_OID_PROPERTY( OID_PNP_SET_POWER, NDIS_DEVICE_POWER_STATE, false)
KNDIS_OID_PROPERTY( OID_PNP_QUERY_POWER, NDIS_DEVICE_POWER_STATE, false)
KNDIS_OID_PROPERTY( OID_PNP_ADD_WAKE_UP_PATTERN, NDIS_PM_PACKET_PATTERN, false)
KNDIS_OID_PROPERTY( OID_PNP_REMOVE_WAKE_UP_PATTERN, NDIS_PM_PACKET_PATTERN, false)
KNDIS_OID_PROPERTY( OID_PNP_WAKE_UP_PATTERN_LIST, NDIS_PM_PACKET_PATTERN, false)
#endif

#include	<KNdisOidTraits.h>

//////////////////////////////////////////////////////////
// Predefined OID 'case'  handlers (for use in OID maps)
//////////////////////////////////////////////////////////

// OID list
#ifndef KNDIS_OID_GEN_SUPPORTED_LIST_HANDLER
#define KNDIS_OID_GEN_SUPPORTED_LIST_HANDLER(list)	\
		case OID_GEN_SUPPORTED_LIST:				\
			ASSERT(KNDIS_CHECK_OID_LIST(list));		\
			MoveSource = list;						\
			BytesToMove = sizeof(list);				\
			break;
#else
#define KNDIS_OID_GEN_SUPPORTED_LIST_HANDLER(list)
#endif

// PNP capabilities
#if !defined(KNDIS_OID_PNP_CAPABILITIES_HANDLER)
 #if (KNDIS_PNP_AWARE)
 #define	KNDIS_OID_PNP_CAPABILITIES_HANDLER(Class)	\
	case OID_PNP_CAPABILITIES:	\
		MoveSource=NULL; *BytesNeeded = sizeof(NDIS_PNP_CAPABILITIES); BytesToMove =  \
		((_status = KNdisPnpMode<Class>::GetCapabilities((PNDIS_PNP_CAPABILITIES)InformationBuffer))==NDIS_STATUS_SUCCESS) \
			? (NDIS_STATUS)sizeof(NDIS_PNP_CAPABILITIES) : 0;	\
		break;
 #else
 #define	KNDIS_OID_PNP_CAPABILITIES_HANDLER(Class) /* for NT4 DDK builds */
 #endif
#endif


// a bogus type for convenience
typedef KNdisOidTraitsBase<NDIS_OID(0)> KNdisOidTrait;

///////////////////////////////////////////////////////////////////////////////
// KNdisPendingOid
//
// Helper class for caching OID request information for miniport OID handlers
// that return NDIS_STATUS_PENDING.
//
struct KNdisPendingOid
{
	NDIS_REQUEST_TYPE	Type;
	NDIS_OID	Oid;
	PVOID		InformationBuffer;
	ULONG		InformationBufferLength;
	PULONG		BytesMoved;
	PULONG		BytesNeeded;
};

// Helper macro to use within QueryInformation/SetInformation handlers below
#define	CACHE_OID_REQUEST(type)												\
			m_PendingOid.Type = type;										\
			m_PendingOid.Oid = Oid;											\
			m_PendingOid.InformationBuffer = InformationBuffer;				\
			m_PendingOid.InformationBufferLength = InformationBufferLength; \
			m_PendingOid.BytesMoved = BytesMoved;							\
			m_PendingOid.BytesNeeded = BytesNeeded;

////////////////////////////////////////////////////////////////////////////////
// OID Cracker Definition (OID Maps)
////////////////////////////////////////////////////////////////////////////////

#define BEGIN_OID_QUERY_MAP(Class)				\
NDIS_STATUS Class::QueryInformation (			\
	IN NDIS_OID	Oid,							\
	IN PVOID	InformationBuffer,				\
	IN ULONG	InformationBufferLength,		\
	OUT PULONG	BytesMoved,						\
	OUT PULONG	BytesNeeded						\
	)											\
{												\
	typedef Class _Aclass ;						\
	NDIS_STATUS _status = NDIS_STATUS_SUCCESS;  \
	*BytesMoved = 0;						    \
	union										\
	{											\
		KNdisOidTrait::ItemType		val;		\
		KNdisOidTrait::LongItemType valX2;		\
	} item;										\
	ULONG	 BytesToMove = (ULONG)sizeof(item.val);	\
	PVOID	 MoveSource=&item.val;				\
	if (TRACE_OID_WANTED(KNDIS_OIDTRACE_QUERY)) \
		TRACE_OID(#Class"::QueryInformation: %s (%X)\n", KNDIS_GET_OID_NAME(Oid), Oid); \
	if (KNdisAdapterTraits<Class>::OidQueryCanPend())	\
	{													\
		CACHE_OID_REQUEST(NdisRequestQueryInformation);	\
	}													\
	switch (Oid)										\
	{													\
		KNDIS_OID_GEN_SUPPORTED_LIST_HANDLER(sm_OID_GEN_SUPPORTED_LIST); \
		KNDIS_OID_PNP_CAPABILITIES_HANDLER(Class);

// QUERY entry: only one inner "if" gets evaluated to TRUE. All the others should be compiled out.
#define OID_QUERY_ENTRY(oid)												\
	case oid:																\
		/* can't-fail items of fixed size */								\
		if (!KNdisOidTraits<oid>::Failable() && !KNdisOidTraitsBase<oid>::IsVarSize()) \
		{																	\
			/** normal size or less */										\
			if (KNdisOidTraitsBase<oid>::Length() <= sizeof(item.val))		\
			{																\
				item.val = (KNdisOidTrait::ItemType) get##oid ();			\
				if (KNdisOidTraitsBase<oid>::Length() < sizeof(item.val)) 	\
					BytesToMove = (ULONG)KNdisOidTraitsBase<oid>::Length();	\
			}																\
			/** double size or less */										\
			if (KNdisOidTraitsBase<oid>::Length() > sizeof(item.val) &&		\
						KNdisOidTraitsBase<oid>::Length() <= sizeof(item.valX2)) \
			{																\
				item.valX2 = (KNdisOidTrait::LongItemType) get##oid ();		\
				BytesToMove = (ULONG)KNdisOidTraitsBase<oid>::Length();		\
			}																\
		}																	\
		/* can-fail/async or var-size items */								\
		if (KNdisOidTraits<oid,_Aclass>::Failable() || KNdisOidTraitsBase<oid>::IsVarSize()) \
		{																	\
			/** fixed size */												\
			if (!KNdisOidTraitsBase<oid>::IsVarSize())						\
			{																\
				MoveSource = NULL;											\
				_status = get##oid((KNdisOid<oid>::Type*)InformationBuffer);\
			}																\
			/** var size */													\
			if (KNdisOidTraitsBase<oid>::IsVarSize())						\
			{																\
				MoveSource = NULL;											\
				BytesToMove = InformationBufferLength;						\
				_status = get##oid((KNdisOid<oid>::Type*)InformationBuffer, &BytesToMove, BytesNeeded);	\
			}																\
		}																	\
		break;

#define	END_OID_QUERY_MAP()													\
		default:															\
			TRACE_OID(" => Non Supported OID Query: %s (%X)\n", KNDIS_GET_OID_NAME(Oid), Oid); \
			_status = NDIS_STATUS_NOT_SUPPORTED;							\
			return _status;													\
	} /*switch*/															\
	if (MoveSource)															\
	{																		\
		/* simple types done */												\
		if (BytesToMove > InformationBufferLength)							\
		{																	\
			*BytesNeeded = BytesToMove;										\
			*BytesMoved = 0;												\
			_status = NDIS_STATUS_BUFFER_TOO_SHORT;							\
		}																	\
		else																\
		{																	\
			NdisMoveMemory(InformationBuffer, MoveSource, BytesToMove);		\
			*BytesNeeded = *BytesMoved = BytesToMove;						\
		}																	\
	}																		\
	else																	\
	{ /* var stuff: the status and "needed" taken care of by the handler */	\
		*BytesMoved = BytesToMove;											\
	}																		\
	if (TRACE_OID_WANTED(KNDIS_OIDTRACE_STATUS) || ((TRACE_OID_WANTED(KNDIS_OIDTRACE_ERROR)&&_status))) \
			TRACE_OID(" => %s(%X): %X, %X\n",								\
			(_status)?((_status==NDIS_STATUS_PENDING)?"PEND":"ERR"):"OK",_status, *BytesMoved, (*BytesMoved == 4)?*PULONG(InformationBuffer):0); \
		return _status;														\
};

#define BEGIN_OID_SET_MAP(Class)				\
NDIS_STATUS Class::SetInformation (				\
	IN NDIS_OID Oid,							\
	IN PVOID	InformationBuffer,				\
	IN ULONG	InformationBufferLength,		\
	OUT PULONG	BytesMoved /*BytesRead*/,		\
	OUT PULONG	BytesNeeded 					\
	)											\
{												\
	*BytesMoved=*BytesNeeded=InformationBufferLength;	\
	NDIS_STATUS _status;								\
	if (TRACE_OID_WANTED(KNDIS_OIDTRACE_SET))			\
		TRACE_OID(#Class"::SetInformation: %s (%X): %X\n", KNDIS_GET_OID_NAME(Oid), Oid, (InformationBufferLength)?*PULONG(InformationBuffer):0); \
	if (KNdisAdapterTraits<Class>::OidSetCanPend())		\
	{													\
		CACHE_OID_REQUEST(NdisRequestSetInformation);	\
	}													\
	switch(Oid)											\
	{


#define OID_SET_ENTRY(oid)													\
	case oid:																\
		/* non-var struct param  */											\
		if (!KNdisOidTraitsBase<oid>::IsVarSize())							\
		{																	\
			if (KNdisOidTraitsBase<oid>::Length() > InformationBufferLength)\
			{																\
				*BytesMoved = 0;											\
				*BytesNeeded = (ULONG)KNdisOidTraitsBase<oid>::Length();	\
				return NDIS_STATUS_BUFFER_TOO_SHORT;						\
			}																\
			_status = set##oid ((KNdisOid<oid>::Type*)InformationBuffer);	\
		}																	\
		/* var stuff or can-fail or async stuff */							\
		if (KNdisOidTraitsBase<oid>::IsVarSize())							\
		{																	\
			_status = set##oid((KNdisOid<oid>::Type*)InformationBuffer, BytesMoved, BytesNeeded);	\
		}																	\
		break;																\

#define	END_OID_SET_MAP()													\
		default:															\
			TRACE_OID(" => Non Supported OID Set: %s (%X)\n", KNDIS_GET_OID_NAME(Oid), Oid); \
			_status = NDIS_STATUS_NOT_SUPPORTED;							\
			*BytesMoved=*BytesNeeded=0;										\
			return _status;													\
	} /*switch*/															\
	if (TRACE_OID_WANTED(KNDIS_OIDTRACE_STATUS) || ((TRACE_OID_WANTED(KNDIS_OIDTRACE_ERROR)&&_status))) \
			TRACE_OID(" => %s(%X): %X\n",									\
			(_status)?((_status==NDIS_STATUS_PENDING)?"PEND":"ERR"):"OK",_status, *BytesMoved); \
		return _status;														\
};

////////////////////////////////////////////////////////////////////////////////
// CO_OID Maps
////////////////////////////////////////////////////////////////////////////////
#define BEGIN_CO_OID_QUERY_MAP(Class, VCClass, TraitsClass)	\
NDIS_STATUS Class::CoQueryInformation (			\
	IN VCClass	*pVC,							\
	IN NDIS_OID	Oid,							\
	IN PVOID	InformationBuffer,				\
	IN ULONG	InformationBufferLength,		\
	OUT PULONG	BytesMoved,						\
	OUT PULONG	BytesNeeded						\
	)											\
{												\
	typedef Class _Aclass ;						\
	NDIS_STATUS _status = NDIS_STATUS_SUCCESS;  \
	*BytesMoved = 0;						    \
	union										\
	{											\
		KNdisOidTrait::ItemType		val;		\
		KNdisOidTrait::LongItemType valX2;		\
	} item;									    \
	ULONG	 BytesToMove = (ULONG)sizeof(item.val);	\
	PVOID	 MoveSource=&item.val;				\
	if (TRACE_OID_WANTED(KNDIS_OIDTRACE_QUERY)) \
	TRACE_OID(#Class"::CoQueryInformation: %s (%X)\n", KNDIS_GET_OID_NAME(Oid), Oid); \
	if (TraitsClass::OidQueryCanPend())			\
	{											\
		CACHE_OID_REQUEST(NdisRequestQueryInformation);	\
	}											\
	switch (Oid)								\
	{							    			\
		KNDIS_OID_GEN_SUPPORTED_LIST_HANDLER(sm_OID_GEN_SUPPORTED_LIST); \
		KNDIS_OID_PNP_CAPABILITIES_HANDLER(Class);

	// QUERY entry: only one inner "if" gets evaluated to TRUE. All the others should be compiled out.
#define CO_OID_QUERY_ENTRY(oid)												\
	case oid:																\
		/* can't-fail items of fixed size */								\
		if (!KNdisOidTraits<oid>::Failable() && !KNdisOidTraitsBase<oid>::IsVarSize()) \
		{																	\
			/** normal size or less */										\
			if (KNdisOidTraitsBase<oid>::Length() <= sizeof(item.val))		\
			{																\
				item.val = (KNdisOidTrait::ItemType) get##oid (pVC);		\
				if (KNdisOidTraitsBase<oid>::Length() < sizeof(item.val)) 	\
					BytesToMove = (ULONG)KNdisOidTraitsBase<oid>::Length();	\
			}																\
			/** double size or less */										\
			if (KNdisOidTraitsBase<oid>::Length() > sizeof(item.val) &&		\
				KNdisOidTraitsBase<oid>::Length() <= sizeof(item.valX2))	\
			{																\
				item.valX2 = (KNdisOidTrait::LongItemType) get##oid (pVC);	\
				BytesToMove = (ULONG)KNdisOidTraitsBase<oid>::Length();		\
			}																\
		}																	\
		/* can-fail/async or var-size items */								\
		if (KNdisOidTraits<oid,_Aclass>::Failable() || KNdisOidTraitsBase<oid>::IsVarSize()) \
		{																	\
			/** fixed size */												\
			if (!KNdisOidTraitsBase<oid>::IsVarSize())						\
			{																\
				MoveSource = NULL;											\
				_status = get##oid(pVC, (KNdisOid<oid>::Type*)InformationBuffer); \
			}																\
			/** var size */													\
			if (KNdisOidTraitsBase<oid>::IsVarSize())						\
			{																\
				MoveSource = NULL;											\
				BytesToMove = InformationBufferLength;						\
				_status = get##oid(pVC, (KNdisOid<oid>::Type*)InformationBuffer, &BytesToMove, BytesNeeded); \
			}																\
		}																	\
		break;

#define	END_CO_OID_QUERY_MAP()												\
		default:															\
			TRACE_OID(" => Non Supported OID Query: %s (%X)\n", KNDIS_GET_OID_NAME(Oid), Oid); \
			_status = NDIS_STATUS_NOT_SUPPORTED;							\
			return _status;													\
	} /*switch*/															\
	if (MoveSource)															\
	{																		\
		/* simple types done */												\
		if (BytesToMove > InformationBufferLength)							\
		{																	\
			*BytesNeeded = BytesToMove;										\
			*BytesMoved = 0;												\
			_status = NDIS_STATUS_BUFFER_TOO_SHORT;							\
		}																	\
		else																\
		{																	\
			NdisMoveMemory(InformationBuffer, MoveSource, BytesToMove);		\
			*BytesNeeded = *BytesMoved = BytesToMove;						\
		}																	\
	}																		\
	else																	\
	{ /* var stuff: the status and "needed" taken care of by the handler */	\
		*BytesMoved = BytesToMove;											\
	}																		\
	if (TRACE_OID_WANTED(KNDIS_OIDTRACE_STATUS) || ((TRACE_OID_WANTED(KNDIS_OIDTRACE_ERROR)&&_status))) \
			TRACE_OID(" => %s(%X): %X, %X\n",								\
			(_status)?((_status==NDIS_STATUS_PENDING)?"PEND":"ERR"):"OK",_status, *BytesMoved, (*BytesMoved == 4)?*PULONG(InformationBuffer):0); \
		return _status;														\
	};


#define BEGIN_CO_OID_SET_MAP(Class, VCClass, TraitsClass)					\
NDIS_STATUS Class::CoSetInformation (										\
	IN VCClass	*pVC,														\
	IN NDIS_OID	Oid,														\
	IN PVOID	InformationBuffer,											\
	IN ULONG	InformationBufferLength,									\
	OUT PULONG	BytesMoved /*BytesRead*/,									\
	OUT PULONG	BytesNeeded 												\
	)																		\
{																			\
	*BytesMoved=*BytesNeeded=InformationBufferLength;						\
	NDIS_STATUS _status;													\
	if (TRACE_OID_WANTED(KNDIS_OIDTRACE_SET))								\
		TRACE_OID(#Class"::CoSetInformation: %s (%X): %X\n", KNDIS_GET_OID_NAME(Oid), Oid, (InformationBufferLength)?*PULONG(InformationBuffer):0); \
	if (TraitsClass::OidSetCanPend())										\
	{																		\
		CACHE_OID_REQUEST(NdisRequestSetInformation);						\
	}																		\
	switch(Oid)																\
	{


#define CO_OID_SET_ENTRY(oid)												\
	case oid:																\
		/* non-var struct param  */											\
		if (!KNdisOidTraitsBase<oid>::IsVarSize())							\
		{																	\
			if (KNdisOidTraitsBase<oid>::Length() > InformationBufferLength)\
			{																\
				*BytesMoved = 0;											\
				*BytesNeeded = (ULONG)KNdisOidTraitsBase<oid>::Length();	\
				return NDIS_STATUS_BUFFER_TOO_SHORT;						\
			}																\
			_status = set##oid (pVC, (KNdisOid<oid>::Type*)InformationBuffer);	\
		}																	\
		/* var stuff or can-fail or async stuff */							\
		if (KNdisOidTraitsBase<oid>::IsVarSize())							\
		{																	\
			_status = set##oid(pVC, (KNdisOid<oid>::Type*)InformationBuffer, BytesMoved, BytesNeeded);	\
		}																	\
		break;																\

#define	END_CO_OID_SET_MAP()												\
		default:															\
			TRACE_OID(" => Non Supported OID Set: %s (%X)\n", KNDIS_GET_OID_NAME(Oid), Oid); \
			_status = NDIS_STATUS_NOT_SUPPORTED;							\
			*BytesMoved=*BytesNeeded=0;										\
			return _status;													\
	} /*switch*/															\
	if (TRACE_OID_WANTED(KNDIS_OIDTRACE_STATUS) || ((TRACE_OID_WANTED(KNDIS_OIDTRACE_ERROR)&&_status))) \
			TRACE_OID(" => %s(%X): %X\n",									\
			(_status)?((_status==NDIS_STATUS_PENDING)?"PEND":"ERR"):"OK",_status, *BytesMoved); \
		return _status;														\
};

#endif // !defined(AFX_KNDISOID_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_)
