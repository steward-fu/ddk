// BChannel.h: interface for the CBChannel class.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCHANNEL_H__2AF86DBB_F029_45C1_870F_A1B59C8BDBF3__INCLUDED_)
#define AFX_BCHANNEL_H__2AF86DBB_F029_45C1_870F_A1B59C8BDBF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _CTYPE_DISABLE_MACROS
#define _INC_WCHAR

#include "KNdis.h"
#include "KNdisCallManagerVc.h"
#include "KNdisMiniportVc.h"
#include "KNdisPacketList.h"
#include "NdisWAN.h"
#include "NdisTapi.h"
#include <kstl.h>
#include <string>
using namespace std;

#if defined (SAMPLE_DRIVER)
#include "CardEvents.h"
#endif

///////////////////////////////////////////////////////////////////////
// Forward declarations
class CoIsdnAdapter;
class CCard;

class CBChannel : 
    public KObject<NDIS_HANDLE, 'knvc'>,
    public KNdisCallManagerVc,
	public KNdisMiniportVc
#if defined (SAMPLE_DRIVER)
    , public CEventReceiver
#endif
{
public:
    friend class CoIsdnAdapter;
    friend class CCard;

    CBChannel(CoIsdnAdapter *pAdapter = NULL, NDIS_HANDLE NdisVcHandle = NULL);
	~CBChannel();

    NDIS_STATUS FinalConstruct(KNdisConfig& Config);
    
#if defined (SAMPLE_DRIVER)
    // CEventReceiver Overrides
    void OnEvent(CardEvent&);
#endif

    // Miniport Virtual-Connection operations
    VOID CoSendPackets(IN PPNDIS_PACKET PacketArray, IN UINT NumberOfPackets);
    NDIS_STATUS CoActivateVc(IN OUT PCO_CALL_PARAMETERS CallParameters);
    NDIS_STATUS CoDeactivateVc();
    NDIS_STATUS CoRequest(IN OUT PNDIS_REQUEST NdisRequest);

    // Call Manager Virtual Connection Operations
    NDIS_STATUS CmMakeCall(
        IN OUT PCO_CALL_PARAMETERS      CallParameters,
        IN NDIS_HANDLE                  NdisPartyHandle  OPTIONAL,
        OUT PNDIS_HANDLE                CallMgrPartyContext  OPTIONAL
        );
    
    NDIS_STATUS CmCloseCall(
        IN NDIS_HANDLE                  CallMgrPartyContext  OPTIONAL,
        IN PVOID                        CloseData  OPTIONAL,
        IN UINT                         Size  OPTIONAL
        );
    
    VOID CmIncomingCallComplete(
        IN NDIS_STATUS                  Status,
        IN PCO_CALL_PARAMETERS          CallParameters
        );
    
    NDIS_STATUS CmAddParty(
        IN OUT PCO_CALL_PARAMETERS      CallParameters,
        IN NDIS_HANDLE                  NdisPartyHandle,
        OUT PNDIS_HANDLE                CallMgrPartyContext
        );
    
    static NDIS_STATUS CmDropParty(
        IN NDIS_HANDLE					CallMgrPartyContext,
        IN PVOID                        CloseData  OPTIONAL,
        IN UINT                         Size  OPTIONAL
        );
    
    VOID CmActivateVcComplete(
        IN NDIS_STATUS                  Status,
        IN PCO_CALL_PARAMETERS           CallParameters
        );
    
    VOID CmDeactivateVcComplete(
        IN NDIS_STATUS                  Status
        );
    
    NDIS_STATUS CmRequest(
        IN NDIS_HANDLE                  ProtocolPartyContext  OPTIONAL,
        IN OUT  PNDIS_REQUEST           NdisRequest
        );
    
    VOID CmRequestComplete(
        IN NDIS_STATUS                  Status,
        IN NDIS_HANDLE                  ProtocolPartyContext  OPTIONAL, 
        IN OUT  PNDIS_REQUEST           NdisRequest
        );
    
    NDIS_STATUS CmModifyCallQoS(
        IN PCO_CALL_PARAMETERS			CallParameters
        );
    
    ///////////////////////////////////////////////////////////////////
    // Implementation Functions

    CoIsdnAdapter *GetAdapter()
    {
        return m_pAdapter;
    }

    // Channel Availability Status Operations
    VOID SetUnavailable();
    bool IsAvailable() const;

    // Get/Set Listening SAP handle associated with BChannel object (if any)
    NDIS_HANDLE GetNdisSapHandle() const
    {
        return m_NdisSapHandle;
    }
    VOID SetNdisSapHandle(NDIS_HANDLE NdisSapHandle)
    {
        m_NdisSapHandle = NdisSapHandle;
    }
    VOID SetTapiSap(PCO_AF_TAPI_SAP pTapiSap)
    {
        m_NdisTapiSap = *pTapiSap;
    }

    CO_AF_TAPI_SAP GetTapiSap()
    {
        return m_NdisTapiSap;
    }

    ULONG GetCallState() const
    {
        return m_CallState;
    }
    VOID SetCallState(ULONG CallState)
    {
        m_CallState = CallState;
    }

    ULONG GetLinkSpeed() const
    { 
        return m_LinkSpeed;
    }
    
#if defined (SAMPLE_DRIVER)
    CBChannel* GetPeerChannel()
    {
        return m_PeerChannel;
    }
    VOID SetPeerChannel(CBChannel *pPeerChannel)
    {
        m_PeerChannel = pPeerChannel;
    }
#endif

    // Open a Virtual circuit 
    NDIS_STATUS Open(NDIS_HANDLE NdisVcHandle);
    VOID Close();
    
    VOID RecordInProgressTransmission(PNDIS_PACKET pPacket);
    PCO_CALL_PARAMETERS AllocateIncomingCallParams();

protected:
    NDIS_STATUS ReadConfiguration(KNdisConfig& rConfig);
    NDIS_STATUS CreateObjects(KNdisConfig &rConfig);
    VOID DestroyObjects();

    VOID CleanPhoneNumber(basic_string<UCHAR>& strDst, PUSHORT pSrc, USHORT Length);
    VOID CompleteCmMakeCall(NDIS_STATUS Status) ;
    VOID CompleteCmCloseCall(NDIS_STATUS Status);
    VOID InitiateCallTearDown();
    VOID ReceivePacketHandler(KNdisPacket& rPacket);

protected:
    CoIsdnAdapter *m_pAdapter;

    // List entry within the BChannel object
    LIST_ENTRY m_ListEntry;

    // Buffers currently submitted to the controller waiting for receive.
    KNdisInterlockedPacketList m_ReceivePendingList;

    // Packets currently submitted to the controller waiting for completion.
    KNdisInterlockedPacketList m_TransmitBusyList;

    // The speed provided by the link in bits per second.  This value is
    // passed up by the Miniport during the LINE_UP indication.
    ULONG m_LinkSpeed;

    // TAPI bearer mode(s) supported by the card.
    ULONG m_BearerModesCaps;

    // Events currently supported
    ULONG m_MediaModesCaps;

    // Events currently supported
    ULONG m_DevStatesCaps;

    // Events currently supported
    ULONG m_AddressStatesCaps;

    // Events currently supported
    ULONG m_CallStatesCaps;

    // NdisSapHandle associated to a listening client on the channel
    NDIS_HANDLE m_NdisSapHandle;

    // The listening TAPI sap associated (if any) with this BChannel
    CO_AF_TAPI_SAP m_NdisTapiSap;

    // Virtual Circuit Open/Close Status
    bool m_bOpen;

    // The current TAPI LINECALLSTATE
    ULONG m_CallState;
    
    // Call being closed flag
    bool m_bCallClosing;

    // The current TAPI media mode(s) supported
    ULONG m_MediaMode;

    // The current TAPI bearer mode in use.
    ULONG  m_BearerMode;
    
    // Total packets read by channel since it was opened
    ULONG m_TotalRxPackets;

    // Total bytes read by driver during this session.
    ULONG m_TotalRxBytes;

    // Total packets written by driver during this session.
    ULONG m_TotalTxPackets;
    
    // Total bytes written by driver during this session.
    ULONG m_TotalTxBytes;
    
    // The current settings associated with this link as passed in via
    // the OID_WAN_SET_LINK_INFO request.
    NDIS_WAN_CO_SET_LINK_INFO m_WanLinkInfo;

    // The current state of the virtual connection
    ULONG m_VcState;
    // Bit flags used to keep track of VC state.
#   define  VCF_INCOMING_CALL   0x00000001
#   define  VCF_OUTGOING_CALL   0x00000002
#   define  VCF_VC_ACTIVE       0x00000004

#if defined (SAMPLE_DRIVER)
    CBChannel *m_PeerChannel;
#endif

    // Incoming call parameters.  Allocated as needed.
    PCO_CALL_PARAMETERS m_pInCallParms;
    
    // Size of the InCallParms struct
    ULONG m_CallParmsSize;
    
    // Pointer to the client's call parameters passed into
    PCO_CALL_PARAMETERS m_pOutCallParms;

    // This flag indicates whether or not NdisMCmIndicateReceiveComplete
    // needs to be called after the completion of the event processing loop.
    bool m_bNeedReceiveCompleteIndication;

    // Cached params of pended OID request
    KNdisPendingOid m_PendingOid;
};

#endif // !defined(AFX_BCHANNEL_H__2AF86DBB_F029_45C1_870F_A1B59C8BDBF3__INCLUDED_)
