// BChannel.cpp: implementation of the CBChannel class.
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
//

#include "BChannel.h"
#include "CoIsdnAdap.h"
#include "NdisTapi.h"
#include "MiniportVcTraits.h"
#include "CallManagerVcTraits.h"
#include "DChannel.h"
#include "KNdisTrace.h"
#include "Card.h"

extern KNdisTrace g_trace;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBChannel::CBChannel(CoIsdnAdapter *pAdapter, NDIS_HANDLE NdisVcHandle):
    m_pAdapter(pAdapter), m_ListEntry(), m_CallParmsSize(0),
    m_ReceivePendingList((ULONG)offsetof(NDIS_PACKET, MiniportReservedEx)),
    m_TransmitBusyList((ULONG)offsetof(NDIS_PACKET, MiniportReservedEx)),
    m_NdisSapHandle(NULL), m_bOpen(false), m_CallState(0),
    m_bCallClosing(false), m_MediaMode(0), m_TotalRxPackets(0),
    m_TotalTxPackets(0), m_TotalTxBytes(0),
    m_pInCallParms(NULL), m_pOutCallParms(NULL), m_TotalRxBytes(0),
    m_bNeedReceiveCompleteIndication(false), m_BearerMode(0)
#if defined (SAMPLE_DRIVER)
    , m_PeerChannel(NULL)
#endif
{
    m_Handle = NdisVcHandle;
    NdisZeroMemory(&m_WanLinkInfo, sizeof(m_WanLinkInfo));
    NdisZeroMemory(&m_NdisTapiSap, sizeof(m_NdisTapiSap));

    // Setup the static features of the link.
    m_LinkSpeed = _64KBPS;
    m_BearerModesCaps = LINEBEARERMODE_DATA
                                 // | LINEBEARERMODE_VOICE
                                 ;
    m_MediaModesCaps = LINEMEDIAMODE_DIGITALDATA 
                     | LINEMEDIAMODE_UNKNOWN
                     // | LINEMEDIAMODE_DATAMODEM
                     ;

    // Initialize the TAPI event capabilities supported by the link.
    m_DevStatesCaps  = LINEDEVSTATE_RINGING
                       | LINEDEVSTATE_CONNECTED
                       | LINEDEVSTATE_DISCONNECTED
                       | LINEDEVSTATE_INSERVICE
                       | LINEDEVSTATE_OUTOFSERVICE
                       | LINEDEVSTATE_OPEN
                       | LINEDEVSTATE_CLOSE
                       | LINEDEVSTATE_REINIT
                       ;
    m_AddressStatesCaps = 0;
    m_CallStatesCaps = LINECALLSTATE_IDLE
                       | LINECALLSTATE_DIALING
                       | LINECALLSTATE_OFFERING
                       | LINECALLSTATE_CONNECTED
                       | LINECALLSTATE_DISCONNECTED
                       ;
}

CBChannel::~CBChannel()
{
    DestroyObjects();
}

NDIS_STATUS CBChannel::FinalConstruct(KNdisConfig& Config)
{
    NDIS_STATUS Result = ReadConfiguration(Config);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;

    return CreateObjects(Config);
}

#if defined(SAMPLE_DRIVER)
void CBChannel::OnEvent(CardEvent& rEvent)
{
    NDIS_STATUS Result;
    CBChannel *pChannel = this;
    CBChannel *pPeerBChannel = static_cast<CBChannel*>(rEvent.pSendingObject);

    switch(rEvent.eEventCode)
    {
    case CARD_EVENT_RING:
        {
            ASSERT(pPeerBChannel);
            m_PeerChannel = pPeerBChannel;
            m_VcState |= VCF_INCOMING_CALL;
            Result = m_pAdapter->SetupIncomingCall(&pChannel);
            if (Result == NDIS_STATUS_SUCCESS)
            {
                //Make sure accepting the incoming call did not alter the VC
                ASSERT(pChannel = static_cast<CBChannel*>(rEvent.pReceivingObject));
            }
            else if (Result != NDIS_STATUS_PENDING)
            {
                m_pAdapter->GetDChannel()->RejectCall(this);
            }
            else
            {
                //Make sure accepting the incoming call did not alter the VC
                ASSERT(pChannel = static_cast<CBChannel*>(rEvent.pReceivingObject));                
            }
            break;
        }
    case CARD_EVENT_CONNECT:
        {
            if (m_VcState & VCF_OUTGOING_CALL)
            {
                CompleteCmMakeCall(NDIS_STATUS_SUCCESS);
            }
            break;
        }
    case CARD_EVENT_DISCONNECT:
        {
            m_PeerChannel = NULL;
            if (m_VcState & VCF_OUTGOING_CALL)
            {
                if (m_CallState != LINECALLSTATE_CONNECTED)
                {
                    // Call never made it to the connected state.
                    CompleteCmMakeCall(NDIS_STATUS_FAILURE);
                }
                else
                {
                    InitiateCallTearDown();
                }
            }
            else if (m_VcState & VCF_INCOMING_CALL)
            {
                InitiateCallTearDown();
            }
            break;
        }
    case CARD_EVENT_RECEIVE:
        {
            if (m_CallState & LINECALLSTATE_CONNECTED)
            {
                if (rEvent.pNdisReceivedPacket != NULL)
                {
                    KNdisPacket packet(rEvent.pNdisReceivedPacket);
                    ReceivePacketHandler(packet);
                }
            }
            if (m_PeerChannel)
            {
                CardEvent *pEvent = m_pAdapter->GetCard()->AllocateEvent(
                    CARD_EVENT_TRANSMIT_COMPLETE,
                    this,
                    m_PeerChannel
                    );
                if (pEvent)
                {
                    m_pAdapter->GetCard()->NotifyEvent(pEvent);
                }
            }
            else
            {
                g_trace.Trace("Receiving channel has NULL peer channel\n");
            }
            break;
        }
    case CARD_EVENT_TRANSMIT_COMPLETE:
        {
            PNDIS_PACKET pPacket = NULL;
            m_TransmitBusyList.Lock();
            if (!m_TransmitBusyList.IsEmptyNoLock())
            {
                pPacket = m_TransmitBusyList.RemoveHeadNoLock();                
            }
            m_TransmitBusyList.Unlock();
            if (pPacket)
                m_pAdapter->RecordTransmitCompletion(pPacket);
            
            break;
        }
    default:
        {
            g_trace.Trace("Unknown event code\n");
            break;
        }
    }
}
#endif // SAMPLE_DRIVER

NDIS_STATUS CBChannel::ReadConfiguration(KNdisConfig& rConfig)
{
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CBChannel::CreateObjects(KNdisConfig& rConfig)
{
    return NDIS_STATUS_SUCCESS;
}

VOID CBChannel::DestroyObjects()
{
    if (m_pInCallParms != NULL)
    {
        NdisFreeMemory(m_pInCallParms, m_CallParmsSize, 0);
        m_pInCallParms = NULL;
    }
}

///////////////////////////////////////////////////////////////////////
// Channel Availability Status Operations
VOID CBChannel::SetUnavailable()
{
    InitializeListHead(&m_ListEntry);
}

bool CBChannel::IsAvailable() const
{
    return IsListEmpty(&m_ListEntry)?FALSE: TRUE;
}

///////////////////////////////////////////////////////////////////////
// Miniport Virtual Connection Operations
VOID CBChannel::CoSendPackets(IN PPNDIS_PACKET PacketArray, IN UINT NumberOfPackets)
{
    if (!m_bOpen || m_bCallClosing)
    {
        g_trace.Trace("BChannel %p Closed\n", this);
    }
    for (UINT Index = 0; Index < NumberOfPackets; ++Index)
    {
        KNdisPacket packet(PacketArray[Index]);
        
        if (!m_bOpen || m_bCallClosing)
        {
            NDIS_SET_PACKET_STATUS(PacketArray[Index], NDIS_STATUS_CLOSED);
        }

        UINT BytesToSend = packet.QueryTotalLength();
        if (BytesToSend == 0 || BytesToSend > m_pAdapter->GetCard()->GetBufferSize())
        {
            g_trace.Trace("Bad packet size = %d\n", BytesToSend);

            NdisMCoSendComplete(NDIS_STATUS_INVALID_PACKET,
                m_Handle,
                PacketArray[Index]
                );
        }
        else
        {
            ++m_TotalTxPackets;
            m_TotalTxBytes += packet.QueryTotalLength();
            m_pAdapter->TransmitStartPacket(this, PacketArray[Index]);
        }
        NDIS_SET_PACKET_STATUS(PacketArray[Index], NDIS_STATUS_PENDING);
    }
}

NDIS_STATUS CBChannel::CoActivateVc(IN OUT PCO_CALL_PARAMETERS CallParameters)
{
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CBChannel::CoDeactivateVc()
{
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CBChannel::CoRequest(IN OUT PNDIS_REQUEST NdisRequest)
{
    return NDIS_STATUS_NOT_ACCEPTED;
}

///////////////////////////////////////////////////////////////////////
// Call Manager Virtual Connection Operations
NDIS_STATUS CBChannel::CmMakeCall(
        IN OUT PCO_CALL_PARAMETERS      pCallParameters,
        IN NDIS_HANDLE                  NdisPartyHandle  OPTIONAL,
        OUT PNDIS_HANDLE                pCallMgrPartyContext  OPTIONAL
        )
{
    PCO_AF_TAPI_MAKE_CALL_PARAMETERS pTapiCallParameters;    
    // Points to the LINE call parameters contained in pTapiCallParameters.
    PLINE_CALL_PARAMS pLineCallParams;
    // Length of the dial string in bytes.
    USHORT DialStringLength;
    // Points to the dial string contained in pTapiCallParameters.
    PUSHORT pDialString;    
    // Temporary copy of dial string.
    basic_string<UCHAR> DialString;
    // Holds the result code returned by this function.    
    NDIS_STATUS Result = NDIS_STATUS_SUCCESS;
    
    ASSERT(NdisPartyHandle == NULL);
    ASSERT(pCallParameters->Flags == 0);
    ASSERT(pCallParameters->CallMgrParameters);
    ASSERT(pCallParameters->CallMgrParameters->Transmit.TokenRate ==
        GetLinkSpeed()/8);
    ASSERT(pCallParameters->CallMgrParameters->Receive.TokenRate ==
        GetLinkSpeed()/8);
    ASSERT(pCallParameters->CallMgrParameters->CallMgrSpecific.ParamType == 0);
    ASSERT(pCallParameters->CallMgrParameters->CallMgrSpecific.Length == 0);
    ASSERT(pCallParameters->MediaParameters);
    ASSERT(pCallParameters->MediaParameters->Flags & TRANSMIT_VC);
    ASSERT(pCallParameters->MediaParameters->Flags & RECEIVE_VC);
    ASSERT(pCallParameters->MediaParameters->ReceiveSizeHint >=
        m_pAdapter->GetCard()->GetBufferSize());
    ASSERT(pCallParameters->MediaParameters->MediaSpecific.ParamType == 0);
    ASSERT(pCallParameters->MediaParameters->MediaSpecific.Length >=
        sizeof(CO_AF_TAPI_MAKE_CALL_PARAMETERS));
    pTapiCallParameters = (PCO_AF_TAPI_MAKE_CALL_PARAMETERS)
        pCallParameters->MediaParameters->MediaSpecific.Parameters;
    ASSERT(pTapiCallParameters->ulLineID < m_pAdapter->GetNumBChannels());
    ASSERT(pTapiCallParameters->ulAddressID == TSPI_ADDRESS_ID);
    ASSERT(pTapiCallParameters->ulFlags & CO_TAPI_FLAG_OUTGOING_CALL);
    ASSERT(pTapiCallParameters->DestAddress.Length > sizeof(USHORT));
    ASSERT(pTapiCallParameters->DestAddress.MaximumLength >=
        pTapiCallParameters->DestAddress.Length);
    ASSERT(pTapiCallParameters->DestAddress.Offset >=
        sizeof(NDIS_VAR_DATA_DESC));
    DialStringLength = pTapiCallParameters->DestAddress.Length;
    pDialString = (PUSHORT)
        ((PUCHAR)&pTapiCallParameters->DestAddress +
        pTapiCallParameters->DestAddress.Offset);
    ASSERT(pTapiCallParameters->LineCallParams.Length >= sizeof(LINE_CALL_PARAMS));
    ASSERT(pTapiCallParameters->LineCallParams.MaximumLength >=
        pTapiCallParameters->LineCallParams.Length);
    ASSERT(pTapiCallParameters->LineCallParams.Offset >=
        sizeof(NDIS_VAR_DATA_DESC));
    
    pLineCallParams = (PLINE_CALL_PARAMS)
        ((PUCHAR)&pTapiCallParameters->LineCallParams +
        pTapiCallParameters->LineCallParams.Offset);
    
    // TODO: The sample driver doesn't support multi-party calls.
    *pCallMgrPartyContext = NULL;
    if (pLineCallParams->ulBearerMode & ~m_BearerModesCaps)
    {
        g_trace.Trace("Invalid TAPI BearerMode=0x%X\n", pLineCallParams->ulBearerMode);
        Result = NDIS_STATUS_NOT_SUPPORTED;
    }
    else if (pLineCallParams->ulMediaMode & ~m_MediaModesCaps)
    {
        g_trace.Trace("Invalid TAPI MediaModes=0x%X\n", pLineCallParams->ulMediaMode);
        Result = NDIS_STATUS_NOT_SUPPORTED;
    }
    else if (pLineCallParams->ulMinRate > _64KBPS ||
        pLineCallParams->ulMinRate > pLineCallParams->ulMaxRate)
    {
        g_trace.Trace("Invalid TAPI Rate =%d:%d\n", pLineCallParams->ulMinRate, pLineCallParams->ulMaxRate);
        Result = NDIS_STATUS_NOT_SUPPORTED;
    }
    else if (pLineCallParams->ulMaxRate && pLineCallParams->ulMaxRate < _56KBPS)
    {
        g_trace.Trace("Invalid TAPI Rate = %d:%d\n", pLineCallParams->ulMinRate, pLineCallParams->ulMaxRate);
        Result = NDIS_STATUS_NOT_SUPPORTED;
    }
    else if (DialStringLength == 0)
    {
        g_trace.Trace("Invalid TAPI Param = No dial string\n");
        Result = NDIS_STATUS_NOT_SUPPORTED;
    }
    else
    {
        CleanPhoneNumber(DialString, pDialString, DialStringLength);
        if (!DialString.empty())
        {
            // Save the call parameters.
            m_MediaMode  = pLineCallParams->ulMediaMode;
            m_BearerMode = pLineCallParams->ulBearerMode;
            m_LinkSpeed  = pLineCallParams->ulMaxRate == 0 ?
                                    _64KBPS : pLineCallParams->ulMaxRate;
            m_pOutCallParms  = pCallParameters;

            m_VcState |= VCF_OUTGOING_CALL;
            m_CallState = LINECALLSTATE_DIALING;
            Result = m_pAdapter->GetDChannel()->MakeCall(this, DialString, pLineCallParams);
            if (Result != NDIS_STATUS_PENDING)
            {
                CompleteCmMakeCall(Result);
            }
        }
    }
    
    return  Result;
}

NDIS_STATUS CBChannel::CmCloseCall(
        IN NDIS_HANDLE                  CallMgrPartyContext  OPTIONAL,
        IN PVOID                        CloseData  OPTIONAL,
        IN UINT                         Size  OPTIONAL
        )
{
    NDIS_STATUS Result = GetAdapter()->GetDChannel()->CloseCall(this);
    if (Result != NDIS_STATUS_PENDING)
    {
        CompleteCmCloseCall(Result);
        Result = NDIS_STATUS_PENDING;
    }
    
    return Result;
}

VOID CBChannel::CmIncomingCallComplete(
        IN NDIS_STATUS                  Status,
        IN PCO_CALL_PARAMETERS          CallParameters
        )
{
    NDIS_STATUS Result;

    if (Status != NDIS_STATUS_SUCCESS)
    {
        if (m_CallState != 0 && m_CallState != LINECALLSTATE_IDLE)
        {
            m_pAdapter->GetDChannel()->RejectCall(this);
            m_CallState = LINECALLSTATE_IDLE;
        }
    }
    else
    {
        ASSERT (m_bOpen);
        Status = m_pAdapter->GetDChannel()->AnswerCall(this);
        if (Status == NDIS_STATUS_SUCCESS)
        {
            m_CallState = LINECALLSTATE_CONNECTED;
            NdisMCmDispatchCallConnected(m_Handle);
        }
        else if (Status != NDIS_STATUS_PENDING)
        {
            InitiateCallTearDown();
        }
    }
}

NDIS_STATUS CBChannel::CmAddParty(
        IN OUT PCO_CALL_PARAMETERS      CallParameters,
        IN NDIS_HANDLE                  NdisPartyHandle,
        OUT PNDIS_HANDLE                CallMgrPartyContext
        )
{
    return  NDIS_STATUS_NOT_SUPPORTED;
}

NDIS_STATUS CBChannel::CmDropParty(
        IN NDIS_HANDLE					CallMgrPartyContext,
        IN PVOID                        CloseData  OPTIONAL,
        IN UINT                         Size  OPTIONAL
        )
{
    return  NDIS_STATUS_NOT_SUPPORTED;
}

VOID CBChannel::CmActivateVcComplete(
        IN NDIS_STATUS                  Status,
        IN PCO_CALL_PARAMETERS           CallParameters
        )
{
}

VOID CBChannel::CmDeactivateVcComplete(
        IN NDIS_STATUS                  Status
        )
{
}

NDIS_STATUS CBChannel::CmRequest(
        IN NDIS_HANDLE                  ProtocolPartyContext  OPTIONAL,
        IN OUT  PNDIS_REQUEST           NdisRequest
        )
{
    return  NDIS_STATUS_NOT_ACCEPTED;
}

VOID CBChannel::CmRequestComplete(
        IN NDIS_STATUS                  Status,
        IN NDIS_HANDLE                  ProtocolPartyContext  OPTIONAL, 
        IN OUT  PNDIS_REQUEST           NdisRequest
        )
{
}

NDIS_STATUS CBChannel::CmModifyCallQoS(
        IN PCO_CALL_PARAMETERS			CallParameters
        )
{
    // TODO: Handle this request appropriately
    return  NDIS_STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Implementation Functions
NDIS_STATUS CBChannel::Open(NDIS_HANDLE NdisVcHandle)
{
    NDIS_STATUS Result = NDIS_STATUS_SUCCESS;
    if (m_bOpen == false)
    {
        m_Handle = NdisVcHandle;
        
        //Reset all the state information for this BChannel
        m_bCallClosing = false;
        m_CallState = 0;
        m_MediaMode = 0;
        m_TotalRxPackets = 0;
        
        m_pAdapter->ConfigureLinkInfo(m_WanLinkInfo);
        m_bOpen = true;
    }
    else
    {
        Result = NDIS_STATUS_FAILURE;        
    }
    return Result;
}

VOID CBChannel::Close()
{
    if (m_bOpen)
    {
        //Set the state of call as being about to be closed
        m_bCallClosing = true;

        // Make sure call is cleared and B channel is disabled.
        m_pAdapter->GetDChannel()->CloseCall(this);
    
        m_VcState = 0;
        m_CallState = 0;
        m_Handle = NULL;
        m_bOpen = false;
    }
    else
    {
        g_trace.Trace("BChannel %p already closed\n", this);
    }
}

VOID CBChannel::RecordInProgressTransmission(PNDIS_PACKET pPacket)
{
    m_TransmitBusyList.InsertTail(pPacket);
}

VOID CBChannel::CleanPhoneNumber(basic_string<UCHAR>& strDst, PUSHORT pSrc , USHORT Length)
{
    USHORT NumDigits;

    // Strip out any character which are not digits or # or *.
    for (NumDigits = 0; Length > 0; --Length)
    {
        if ((*pSrc >= '0' && *pSrc <= '9') ||
            (*pSrc == '#' || *pSrc == '*'))
        {
            /*
            // Make sure dial string is within the limit of the adapter.
            */
            if (NumDigits < CARD_MAX_DIAL_DIGITS)
            {
                strDst.append(1, (UCHAR) *pSrc);
            }
            else
            {
                break;
            }
        }
        pSrc++;
    }
}

VOID CBChannel::CompleteCmMakeCall(NDIS_STATUS Status)
{
    if (Status == NDIS_STATUS_SUCCESS)
    {
        m_pOutCallParms->CallMgrParameters->Receive.PeakBandwidth =
        m_pOutCallParms->CallMgrParameters->Transmit.PeakBandwidth =
        m_LinkSpeed/8;
        
        Status = NdisMCmActivateVc(m_Handle, m_pOutCallParms);
        if (Status == NDIS_STATUS_SUCCESS)
        {
            m_VcState |= VCF_VC_ACTIVE;
            m_CallState = LINECALLSTATE_CONNECTED;
        }
        else
        {
            g_trace.Trace("NdisMCmActivateVc returned with error=0x%X\n",Status);
        }
    }
    NdisMCmMakeCallComplete(Status, m_Handle, NULL, NULL, m_pOutCallParms);
    if (Status != NDIS_STATUS_SUCCESS)
    {
        // The call failed, so cleanup and bail out.
        m_VcState &= ~VCF_OUTGOING_CALL;
    }    
}

VOID CBChannel::CompleteCmCloseCall(NDIS_STATUS Status)
{
    if (m_VcState & VCF_VC_ACTIVE)
    {
        m_VcState &= ~VCF_VC_ACTIVE;
        NdisMCmDeactivateVc(m_Handle);
    }

    // Tell NDPROXY we're done.
    NdisMCmCloseCallComplete(Status, m_Handle, NULL);
    
    // If it was an incoming call, it's up to us to delete the VC.
    if (m_VcState & VCF_INCOMING_CALL)
    {
        m_VcState &= ~VCF_INCOMING_CALL;
        if (m_Handle)
        {
            NdisMCmDeleteVc(m_Handle);
            m_Handle = NULL;
            m_pAdapter->FreeIsdnChannel(this);
        }
    }
    else if (m_VcState & VCF_OUTGOING_CALL)
    {
        m_VcState &= ~VCF_OUTGOING_CALL;
    }
}

PCO_CALL_PARAMETERS CBChannel::AllocateIncomingCallParams()
{
    PCO_CALL_PARAMETERS  pCp;
    PCO_CALL_MANAGER_PARAMETERS pCmp;
    PCO_MEDIA_PARAMETERS pMp;
    PCO_AF_TAPI_INCOMING_CALL_PARAMETERS pTcp;
    PLINE_CALL_INFO pLci;
    
    ASSERT(m_NdisTapiSap.ulMediaModes & LINEMEDIAMODE_DIGITALDATA);
    m_MediaMode = LINEMEDIAMODE_DIGITALDATA;
    m_BearerMode = LINEBEARERMODE_DATA;
    m_LinkSpeed = _64KBPS;
    
    if (m_pInCallParms != NULL)
    {
        // TO DO - Change this if incoming call parameters may
        // change from one incoming call to the next.
        return m_pInCallParms;
    }
    m_CallParmsSize = sizeof(CO_CALL_PARAMETERS)
        + sizeof(CO_CALL_MANAGER_PARAMETERS)
        + sizeof(CO_MEDIA_PARAMETERS)
        + sizeof(CO_AF_TAPI_INCOMING_CALL_PARAMETERS)
        + sizeof(LINE_CALL_INFO);
 
    NdisAllocateMemoryWithTag((PVOID*)&m_pInCallParms, m_CallParmsSize, 'knvc');
    if (m_pInCallParms == NULL)
        return NULL;
    
    NdisZeroMemory(m_pInCallParms, m_CallParmsSize);
 
    pCp = (PCO_CALL_PARAMETERS) m_pInCallParms;
    pCmp = (PCO_CALL_MANAGER_PARAMETERS)(pCp + 1);
    pMp = (PCO_MEDIA_PARAMETERS) (pCmp + 1);
    pTcp = (PCO_AF_TAPI_INCOMING_CALL_PARAMETERS)pMp->MediaSpecific.Parameters;
    pLci = (PLINE_CALL_INFO) (pTcp + 1);
    
    // TODO: Fill in the call parameters as needed.

    pCp->Flags                          = PERMANENT_VC;
    pCp->CallMgrParameters              = pCmp;
    pCp->MediaParameters                = pMp;

    pCmp->Transmit.TokenRate            = m_LinkSpeed / 8;
    pCmp->Transmit.TokenBucketSize      = m_pAdapter->GetCard()->GetBufferSize();
    pCmp->Transmit.PeakBandwidth        = m_LinkSpeed / 8;
    pCmp->Transmit.Latency              = 0;
    pCmp->Transmit.DelayVariation       = 0;
    pCmp->Transmit.ServiceType          = SERVICETYPE_BESTEFFORT;
    pCmp->Transmit.MaxSduSize           = m_pAdapter->GetCard()->GetBufferSize();
    pCmp->Transmit.MinimumPolicedSize   = 0;
    pCmp->Receive                       = pCmp->Transmit;
    pCmp->CallMgrSpecific.ParamType     = 0;
    pCmp->CallMgrSpecific.Length        = 0;

    pMp->Flags                          = TRANSMIT_VC | RECEIVE_VC;
    pMp->ReceiveSizeHint                = m_pAdapter->GetCard()->GetBufferSize();
    pMp->MediaSpecific.ParamType        = 0;
    pMp->MediaSpecific.Length           = sizeof(*pTcp) + sizeof(*pLci);

    pTcp->ulLineID                      = m_NdisTapiSap.ulLineID;
    pTcp->ulAddressID                   = TSPI_ADDRESS_ID;
    pTcp->ulFlags                       = CO_TAPI_FLAG_INCOMING_CALL;
    pTcp->LineCallInfo.Length           = sizeof(*pLci);
    pTcp->LineCallInfo.MaximumLength    = sizeof(*pLci);
    pTcp->LineCallInfo.Offset           = sizeof(NDIS_VAR_DATA_DESC);

    pLci->ulTotalSize =
    pLci->ulNeededSize =
    pLci->ulUsedSize = sizeof(*pLci);

    /*
    // The link has all the call information we need to return.
    */
    pLci->hLine = (ULONG) (ULONG_PTR) this;
    pLci->ulLineDeviceID = pTcp->ulLineID;
    pLci->ulAddressID = pTcp->ulAddressID;

    pLci->ulBearerMode = m_BearerMode;
    pLci->ulRate = m_LinkSpeed;
    pLci->ulMediaMode = m_MediaMode;

    pLci->ulCallParamFlags = LINECALLPARAMFLAGS_IDLE;
    pLci->ulCallStates = m_CallStatesCaps;
    
    /*
    // We don't support any of the callerid functions.
    */
    pLci->ulCallerIDFlags =
        pLci->ulCalledIDFlags =
        pLci->ulConnectedIDFlags =
        pLci->ulRedirectionIDFlags =
        pLci->ulRedirectingIDFlags = LINECALLPARTYID_UNAVAIL;
    
    return m_pInCallParms;    
}

VOID CBChannel::InitiateCallTearDown()
{
    NDIS_STATUS Result;
    
    if (m_VcState & VCF_VC_ACTIVE)
        Result = NDIS_STATUS_SUCCESS;
    else
        Result = NDIS_STATUS_FAILURE;
    
    m_CallState = LINECALLSTATE_DISCONNECTED;
    
    // Make sure there are no packets left on this channel before it closes.
    m_pAdapter->FlushSendPackets(this);
    
    // Notify NDPROXY that the call's connection has been lost.
    NdisMCmDispatchIncomingCloseCall(Result, m_Handle, NULL, 0);
}

VOID CBChannel::ReceivePacketHandler(KNdisPacket& rPacket)
{
    //Forward if there's a protocol driver up that cares
    if (m_Handle)
    {
        PNDIS_PACKET pPacket = rPacket;
        NDIS_SET_PACKET_STATUS(pPacket, NDIS_STATUS_SUCCESS);
        NDIS_SET_PACKET_HEADER_SIZE(pPacket, 0);
        NdisMCoIndicateReceivePacket(m_Handle, &pPacket, 1);
        m_bNeedReceiveCompleteIndication = true;
        ++m_TotalRxPackets;
        m_TotalRxBytes += rPacket.QueryTotalLength();
    }
}

