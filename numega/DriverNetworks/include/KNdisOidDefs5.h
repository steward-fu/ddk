// KNdisOidDefs5.h: Defines OID handlers
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
// Continuation of KNdisOidDefs.h -- Additional NDIS 5 handlers

#ifdef KNDIS_MEDIUM_INCLUDE_ATM

DECLARE_OID_HANDLERS ( OID_ATM_SUPPORTED_VC_RATES)
DECLARE_OID_HANDLERS ( OID_ATM_SUPPORTED_SERVICE_CATEGORY)
DECLARE_OID_HANDLERS ( OID_ATM_SUPPORTED_AAL_TYPES)
DECLARE_OID_HANDLERS ( OID_ATM_HW_CURRENT_ADDRESS)
DECLARE_OID_HANDLERS ( OID_ATM_MAX_ACTIVE_VCS)
DECLARE_OID_HANDLERS ( OID_ATM_MAX_ACTIVE_VCI_BITS)
DECLARE_OID_HANDLERS ( OID_ATM_MAX_ACTIVE_VPI_BITS)
DECLARE_OID_HANDLERS ( OID_ATM_MAX_AAL0_PACKET_SIZE)
DECLARE_OID_HANDLERS ( OID_ATM_MAX_AAL1_PACKET_SIZE)
DECLARE_OID_HANDLERS ( OID_ATM_MAX_AAL34_PACKET_SIZE)
DECLARE_OID_HANDLERS ( OID_ATM_MAX_AAL5_PACKET_SIZE)

DECLARE_OID_HANDLERS ( OID_ATM_SIGNALING_VPIVCI)
DECLARE_OID_HANDLERS ( OID_ATM_ASSIGNED_VPI)
DECLARE_OID_HANDLERS ( OID_ATM_ACQUIRE_ACCESS_NET_RESOURCES)
DECLARE_OID_HANDLERS ( OID_ATM_RELEASE_ACCESS_NET_RESOURCES)
DECLARE_OID_HANDLERS ( OID_ATM_ILMI_VPIVCI)
DECLARE_OID_HANDLERS ( OID_ATM_DIGITAL_BROADCAST_VPIVCI)
DECLARE_OID_HANDLERS ( OID_ATM_GET_NEAREST_FLOW)
DECLARE_OID_HANDLERS ( OID_ATM_ALIGNMENT_REQUIRED)
DECLARE_OID_HANDLERS ( OID_ATM_LECS_ADDRESS)
DECLARE_OID_HANDLERS ( OID_ATM_SERVICE_ADDRESS)
DECLARE_OID_HANDLERS ( OID_ATM_CALL_PROCEEDING)
DECLARE_OID_HANDLERS ( OID_ATM_CALL_ALERTING)
DECLARE_OID_HANDLERS ( OID_ATM_PARTY_ALERTING)
DECLARE_OID_HANDLERS ( OID_ATM_CALL_NOTIFY)
DECLARE_OID_HANDLERS ( OID_ATM_MY_IP_NM_ADDRESS)

DECLARE_OID_HANDLERS ( OID_ATM_RCV_CELLS_OK)
DECLARE_OID_HANDLERS ( OID_ATM_XMIT_CELLS_OK)
DECLARE_OID_HANDLERS ( OID_ATM_RCV_CELLS_DROPPED)

DECLARE_OID_HANDLERS ( OID_ATM_RCV_INVALID_VPI_VCI)
DECLARE_OID_HANDLERS ( OID_ATM_CELLS_HEC_ERROR)
DECLARE_OID_HANDLERS ( OID_ATM_RCV_REASSEMBLY_ERROR)
#endif

#ifdef KNDIS_MEDIUM_INCLUDE_WW
DECLARE_OID_HANDLERS ( OID_WW_GEN_NETWORK_TYPES_SUPPORTED)
DECLARE_OID_HANDLERS ( OID_WW_GEN_NETWORK_TYPE_IN_USE)
DECLARE_OID_HANDLERS ( OID_WW_GEN_HEADER_FORMATS_SUPPORTED)
DECLARE_OID_HANDLERS ( OID_WW_GEN_HEADER_FORMAT_IN_USE)
DECLARE_OID_HANDLERS ( OID_WW_GEN_INDICATION_REQUEST)
DECLARE_OID_HANDLERS ( OID_WW_GEN_DEVICE_INFO)
DECLARE_OID_HANDLERS ( OID_WW_GEN_OPERATION_MODE)
DECLARE_OID_HANDLERS ( OID_WW_GEN_LOCK_STATUS)
DECLARE_OID_HANDLERS ( OID_WW_GEN_DISABLE_TRANSMITTER)
DECLARE_OID_HANDLERS ( OID_WW_GEN_NETWORK_ID)
DECLARE_OID_HANDLERS ( OID_WW_GEN_PERMANENT_ADDRESS)
DECLARE_OID_HANDLERS ( OID_WW_GEN_CURRENT_ADDRESS)
DECLARE_OID_HANDLERS ( OID_WW_GEN_SUSPEND_DRIVER)
DECLARE_OID_HANDLERS ( OID_WW_GEN_BASESTATION_ID)
DECLARE_OID_HANDLERS ( OID_WW_GEN_CHANNEL_ID)
DECLARE_OID_HANDLERS ( OID_WW_GEN_ENCRYPTION_SUPPORTED)
DECLARE_OID_HANDLERS ( OID_WW_GEN_ENCRYPTION_IN_USE)
DECLARE_OID_HANDLERS ( OID_WW_GEN_ENCRYPTION_STATE)
DECLARE_OID_HANDLERS ( OID_WW_GEN_CHANNEL_QUALITY)
DECLARE_OID_HANDLERS ( OID_WW_GEN_REGISTRATION_STATUS)
DECLARE_OID_HANDLERS ( OID_WW_GEN_RADIO_LINK_SPEED)
DECLARE_OID_HANDLERS ( OID_WW_GEN_LATENCY)
DECLARE_OID_HANDLERS ( OID_WW_GEN_BATTERY_LEVEL)
DECLARE_OID_HANDLERS ( OID_WW_GEN_EXTERNAL_POWER)
DECLARE_OID_HANDLERS ( OID_WW_GEN_PING_ADDRESS)
DECLARE_OID_HANDLERS ( OID_WW_GEN_RSSI)
DECLARE_OID_HANDLERS ( OID_WW_GEN_SIM_STATUS)
DECLARE_OID_HANDLERS ( OID_WW_GEN_ENABLE_SIM_PIN)
DECLARE_OID_HANDLERS ( OID_WW_GEN_CHANGE_SIM_PIN)
DECLARE_OID_HANDLERS ( OID_WW_GEN_SIM_PUK)
DECLARE_OID_HANDLERS ( OID_WW_GEN_SIM_EXCEPTION)
DECLARE_OID_HANDLERS ( OID_WW_MBX_TRAFFIC_AREA)
DECLARE_OID_HANDLERS ( OID_WW_MBX_LIVE_DIE)
DECLARE_OID_HANDLERS ( OID_WW_MBX_TEMP_DEFAULTLIST)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_AREA_COLOR)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_TX_POWER_LEVEL)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_EID)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_HEADER_COMPRESSION)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_DATA_COMPRESSION)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CHANNEL_SELECT)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CHANNEL_STATE)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_NEI)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_NEI_STATE)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_SERVICE_PROVIDER_IDENTIFIER)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_SLEEP_MODE)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CIRCUIT_SWITCHED)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_TEI)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_RSSI)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CS_SERVICE_PREFERENCE)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CS_SERVICE_STATUS)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CS_INFO)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CS_SUSPEND)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CS_DEFAULT_DIAL_CODE)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CS_CALLBACK)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CS_SID_LIST)
DECLARE_OID_HANDLERS ( OID_WW_CDPD_CS_CONFIGURATION)


DECLARE_OID_HANDLERS ( OID_WW_ARD_SNDCP)
DECLARE_OID_HANDLERS ( OID_WW_ARD_TMLY_MSG)
DECLARE_OID_HANDLERS ( OID_WW_ARD_DATAGRAM)

DECLARE_OID_HANDLERS ( OID_WW_TAC_COMPRESSION)
DECLARE_OID_HANDLERS ( OID_WW_TAC_SET_CONFIG)
DECLARE_OID_HANDLERS ( OID_WW_TAC_GET_STATUS)
DECLARE_OID_HANDLERS ( OID_WW_TAC_USER_HEADER)
DECLARE_OID_HANDLERS ( OID_WW_TAC_UNIQUE_SDU_TAG)
DECLARE_OID_HANDLERS ( OID_WW_TAC_SEND_COMMAND)
DECLARE_OID_HANDLERS ( OID_WW_TAC_GET_RESPONSE)
DECLARE_OID_HANDLERS ( OID_WW_TAC_DISABLE_RECEIVER)
DECLARE_OID_HANDLERS ( OID_WW_TAC_ANTENNA_MODE)
DECLARE_OID_HANDLERS ( OID_WW_TAC_FLUSH_DATA)
DECLARE_OID_HANDLERS ( OID_WW_TAC_SHUTDOWN_DEVICE)
DECLARE_OID_HANDLERS ( OID_WW_TAC_DEVICE_EXCEPTION)
DECLARE_OID_HANDLERS ( OID_WW_TAC_RECEIVE_EXCEPTION)
DECLARE_OID_HANDLERS ( OID_WW_TAC_BUFFER_EXCEPTION)
DECLARE_OID_HANDLERS ( OID_WW_TAC_BATTERY_EXCEPTION)
DECLARE_OID_HANDLERS ( OID_WW_TAC_TRANSMITTER_KEYED)
DECLARE_OID_HANDLERS ( OID_WW_TAC_SYSTEM_TABLE)
DECLARE_OID_HANDLERS ( OID_WW_TAC_CHANNEL_TABLE)
DECLARE_OID_HANDLERS ( OID_WW_TAC_DCHANNEL_TABLE)
DECLARE_OID_HANDLERS ( OID_WW_TAC_RECEIVE_QUEUE_COUNT)
DECLARE_OID_HANDLERS ( OID_WW_TAC_STATISTICS)
DECLARE_OID_HANDLERS ( OID_WW_MET_FUNCTION)

#endif



#ifdef KNDIS_MEDIUM_INCLUDE_IRDA

DECLARE_OID_HANDLERS ( OID_IRDA_RECEIVING)
DECLARE_OID_HANDLERS ( OID_IRDA_TURNAROUND_TIME)
DECLARE_OID_HANDLERS ( OID_IRDA_SUPPORTED_SPEEDS)
DECLARE_OID_HANDLERS ( OID_IRDA_LINK_SPEED)
DECLARE_OID_HANDLERS ( OID_IRDA_MEDIA_BUSY)
DECLARE_OID_HANDLERS ( OID_IRDA_EXTRA_RCV_BOFS)
DECLARE_OID_HANDLERS ( OID_IRDA_RATE_SNIFF)
DECLARE_OID_HANDLERS ( OID_IRDA_UNICAST_LIST)
DECLARE_OID_HANDLERS ( OID_IRDA_MAX_UNICAST_LIST_SIZE)
DECLARE_OID_HANDLERS ( OID_IRDA_MAX_RECEIVE_WINDOW_SIZE)
DECLARE_OID_HANDLERS ( OID_IRDA_MAX_SEND_WINDOW_SIZE)

DECLARE_OID_HANDLERS ( OID_IRDA_RESERVED1)
DECLARE_OID_HANDLERS ( OID_IRDA_RESERVED2)
#endif

#ifdef KNDIS_MEDIUM_INCLUDE_BPC
DECLARE_OID_HANDLERS ( OID_BPC_ADAPTER_CAPS)
DECLARE_OID_HANDLERS ( OID_BPC_DEVICES)
DECLARE_OID_HANDLERS ( OID_BPC_DEVICE_CAPS)
DECLARE_OID_HANDLERS ( OID_BPC_DEVICE_SETTINGS)
DECLARE_OID_HANDLERS ( OID_BPC_CONNECTION_STATUS)
DECLARE_OID_HANDLERS ( OID_BPC_ADDRESS_COMPARE)
DECLARE_OID_HANDLERS ( OID_BPC_PROGRAM_GUIDE)

DECLARE_OID_HANDLERS ( OID_BPC_POOL)


DECLARE_OID_HANDLERS ( OID_BPC_CONNECT)
DECLARE_OID_HANDLERS ( OID_BPC_COMMIT)
DECLARE_OID_HANDLERS ( OID_BPC_DISCONNECT)
DECLARE_OID_HANDLERS ( OID_BPC_CONNECTION_ENABLE)
DECLARE_OID_HANDLERS ( OID_BPC_POOL_RESERVE)
DECLARE_OID_HANDLERS ( OID_BPC_POOL_RETURN)
DECLARE_OID_HANDLERS ( OID_BPC_FORCE_RECEIVE)

DECLARE_OID_HANDLERS ( OID_BPC_LAST_ERROR)

DECLARE_OID_HANDLERS ( OID_BPC_PROVIDER_SPECIFIC)
DECLARE_OID_HANDLERS ( OID_BPC_ADAPTER_SPECIFIC)

DECLARE_OID_HANDLERS ( OID_BPC_LAST)
#endif


#ifdef KNDIS_MEDIUM_INCLUDE_1394
DECLARE_OID_HANDLERS ( OID_1394_LOCAL_NODE_INFO)
DECLARE_OID_HANDLERS ( OID_1394_VC_INFO)
#endif

DECLARE_OID_HANDLERS ( OID_QOS_TC_SUPPORTED)
DECLARE_OID_HANDLERS ( OID_QOS_REMAINING_BANDWIDTH)
DECLARE_OID_HANDLERS ( OID_QOS_ISSLOW_FLOW)
DECLARE_OID_HANDLERS ( OID_QOS_BESTEFFORT_BANDWIDTH)
DECLARE_OID_HANDLERS ( OID_QOS_LATENCY)
DECLARE_OID_HANDLERS ( OID_QOS_FLOW_IP_CONFORMING)
DECLARE_OID_HANDLERS ( OID_QOS_FLOW_COUNT)
DECLARE_OID_HANDLERS ( OID_QOS_NON_BESTEFFORT_LIMIT)
DECLARE_OID_HANDLERS ( OID_QOS_SCHEDULING_PROFILES_SUPPORTED)
DECLARE_OID_HANDLERS ( OID_QOS_CURRENT_SCHEDULING_PROFILE)
DECLARE_OID_HANDLERS ( OID_QOS_TIMER_RESOLUTION)
DECLARE_OID_HANDLERS ( OID_QOS_STATISTICS_BUFFER)
DECLARE_OID_HANDLERS ( OID_QOS_HIERARCHY_CLASS)
DECLARE_OID_HANDLERS ( OID_QOS_FLOW_MODE)
DECLARE_OID_HANDLERS ( OID_QOS_ISSLOW_FRAGMENT_SIZE)
DECLARE_OID_HANDLERS ( OID_QOS_FLOW_IP_NONCONFORMING)
DECLARE_OID_HANDLERS ( OID_QOS_FLOW_8021P_CONFORMING)
DECLARE_OID_HANDLERS ( OID_QOS_FLOW_8021P_NONCONFORMING)

DECLARE_OID_HANDLERS ( OID_TCP_TASK_OFFLOAD)
DECLARE_OID_HANDLERS ( OID_TCP_TASK_IPSEC_ADD_SA)
DECLARE_OID_HANDLERS ( OID_TCP_TASK_IPSEC_DELETE_SA)
DECLARE_OID_HANDLERS ( OID_TCP_SAN_SUPPORT)

DECLARE_OID_HANDLERS ( OID_FFP_SUPPORT)
DECLARE_OID_HANDLERS ( OID_FFP_FLUSH)
DECLARE_OID_HANDLERS ( OID_FFP_CONTROL)
DECLARE_OID_HANDLERS ( OID_FFP_PARAMS)
DECLARE_OID_HANDLERS ( OID_FFP_DATA)
DECLARE_OID_HANDLERS ( OID_FFP_DRIVER_STATS)
DECLARE_OID_HANDLERS ( OID_FFP_ADAPTER_STATS)

#ifdef KNDIS_MEDIUM_INCLUDE_CO
DECLARE_OID_HANDLERS ( OID_GEN_CO_HARDWARE_STATUS )
DECLARE_OID_HANDLERS ( OID_GEN_CO_MEDIA_SUPPORTED )
DECLARE_OID_HANDLERS ( OID_GEN_CO_MEDIA_IN_USE )
DECLARE_OID_HANDLERS ( OID_GEN_CO_LINK_SPEED )
DECLARE_OID_HANDLERS ( OID_GEN_CO_VENDOR_ID )
DECLARE_OID_HANDLERS ( OID_GEN_CO_VENDOR_DESCRIPTION )
DECLARE_OID_HANDLERS ( OID_GEN_CO_DRIVER_VERSION )
DECLARE_OID_HANDLERS ( OID_GEN_CO_PROTOCOL_OPTIONS )
DECLARE_OID_HANDLERS ( OID_GEN_CO_MAC_OPTIONS )
DECLARE_OID_HANDLERS ( OID_GEN_CO_MEDIA_CONNECT_STATUS )
DECLARE_OID_HANDLERS ( OID_GEN_CO_VENDOR_DRIVER_VERSION )
DECLARE_OID_HANDLERS ( OID_GEN_CO_SUPPORTED_GUIDS )
DECLARE_OID_HANDLERS ( OID_GEN_CO_GET_TIME_CAPS )
DECLARE_OID_HANDLERS ( OID_GEN_CO_GET_NETCARD_TIME )
DECLARE_OID_HANDLERS ( OID_GEN_CO_MINIMUM_LINK_SPEED )

DECLARE_OID_HANDLERS ( OID_GEN_CO_XMIT_PDUS_OK )
DECLARE_OID_HANDLERS ( OID_GEN_CO_RCV_PDUS_OK )
DECLARE_OID_HANDLERS ( OID_GEN_CO_XMIT_PDUS_ERROR )
DECLARE_OID_HANDLERS ( OID_GEN_CO_RCV_PDUS_ERROR )
DECLARE_OID_HANDLERS ( OID_GEN_CO_RCV_PDUS_NO_BUFFER )
DECLARE_OID_HANDLERS ( OID_GEN_CO_RCV_CRC_ERROR )
DECLARE_OID_HANDLERS ( OID_GEN_CO_TRANSMIT_QUEUE_LENGTH )
DECLARE_OID_HANDLERS ( OID_GEN_CO_BYTES_XMIT )
DECLARE_OID_HANDLERS ( OID_GEN_CO_BYTES_RCV )
DECLARE_OID_HANDLERS ( OID_GEN_CO_NETCARD_LOAD )
DECLARE_OID_HANDLERS ( OID_GEN_CO_DEVICE_PROFILE )
DECLARE_OID_HANDLERS ( OID_GEN_CO_BYTES_XMIT_OUTSTANDING )

DECLARE_OID_HANDLERS ( OID_CO_ADD_PVC)
DECLARE_OID_HANDLERS ( OID_CO_DELETE_PVC)
DECLARE_OID_HANDLERS ( OID_CO_GET_CALL_INFORMATION)
DECLARE_OID_HANDLERS ( OID_CO_ADD_ADDRESS)
DECLARE_OID_HANDLERS ( OID_CO_DELETE_ADDRESS)
DECLARE_OID_HANDLERS ( OID_CO_GET_ADDRESSES)
DECLARE_OID_HANDLERS ( OID_CO_ADDRESS_CHANGE)
DECLARE_OID_HANDLERS ( OID_CO_SIGNALING_ENABLED)
DECLARE_OID_HANDLERS ( OID_CO_SIGNALING_DISABLED)
DECLARE_OID_HANDLERS ( OID_CO_AF_CLOSE)

DECLARE_OID_HANDLERS ( OID_CO_TAPI_CM_CAPS)
DECLARE_OID_HANDLERS ( OID_CO_TAPI_LINE_CAPS)
DECLARE_OID_HANDLERS ( OID_CO_TAPI_ADDRESS_CAPS)
DECLARE_OID_HANDLERS ( OID_CO_TAPI_TRANSLATE_TAPI_CALLPARAMS)
DECLARE_OID_HANDLERS ( OID_CO_TAPI_TRANSLATE_NDIS_CALLPARAMS)
DECLARE_OID_HANDLERS ( OID_CO_TAPI_TRANSLATE_TAPI_SAP)
DECLARE_OID_HANDLERS ( OID_CO_TAPI_GET_CALL_DIAGNOSTICS)
DECLARE_OID_HANDLERS ( OID_CO_TAPI_REPORT_DIGITS)
DECLARE_OID_HANDLERS ( OID_CO_TAPI_DONT_REPORT_DIGITS)
#endif

#ifdef	KNDIS_MEDIUM_INCLUDE_802_11
DECLARE_OID_HANDLERS ( OID_802_11_BSSID)
DECLARE_OID_HANDLERS ( OID_802_11_SSID)
DECLARE_OID_HANDLERS ( OID_802_11_NETWORK_TYPES_SUPPORTED)
DECLARE_OID_HANDLERS ( OID_802_11_NETWORK_TYPE_IN_USE)
DECLARE_OID_HANDLERS ( OID_802_11_TX_POWER_LEVEL)
DECLARE_OID_HANDLERS ( OID_802_11_RSSI)
DECLARE_OID_HANDLERS ( OID_802_11_RSSI_TRIGGER)
DECLARE_OID_HANDLERS ( OID_802_11_INFRASTRUCTURE_MODE)
DECLARE_OID_HANDLERS ( OID_802_11_FRAGMENTATION_THRESHOLD)
DECLARE_OID_HANDLERS ( OID_802_11_RTS_THRESHOLD)
DECLARE_OID_HANDLERS ( OID_802_11_NUMBER_OF_ANTENNAS)
DECLARE_OID_HANDLERS ( OID_802_11_RX_ANTENNA_SELECTED)
DECLARE_OID_HANDLERS ( OID_802_11_TX_ANTENNA_SELECTED)
DECLARE_OID_HANDLERS ( OID_802_11_SUPPORTED_RATES)
DECLARE_OID_HANDLERS ( OID_802_11_DESIRED_RATES)
DECLARE_OID_HANDLERS ( OID_802_11_CONFIGURATION)
DECLARE_OID_HANDLERS ( OID_802_11_STATISTICS)
DECLARE_OID_HANDLERS ( OID_802_11_ADD_WEP)
DECLARE_OID_HANDLERS ( OID_802_11_REMOVE_WEP)
DECLARE_OID_HANDLERS ( OID_802_11_DISASSOCIATE)
DECLARE_OID_HANDLERS ( OID_802_11_POWER_MODE)
DECLARE_OID_HANDLERS ( OID_802_11_BSSID_LIST)
DECLARE_OID_HANDLERS ( OID_802_11_AUTHENTICATION_MODE)
DECLARE_OID_HANDLERS ( OID_802_11_PRIVACY_FILTER)
DECLARE_OID_HANDLERS ( OID_802_11_BSSID_LIST_SCAN)
DECLARE_OID_HANDLERS ( OID_802_11_WEP_STATUS)
DECLARE_OID_HANDLERS ( OID_802_11_RELOAD_DEFAULTS)
#endif
