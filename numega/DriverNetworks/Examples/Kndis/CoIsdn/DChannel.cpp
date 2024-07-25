// DChannel.cpp: implementation of the CDChannel class.
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

#include "DChannel.h"
#include "CoIsdnAdap.h"
#include "NdisTapi.h"
#include "Card.h"

extern KNdisTrace g_trace;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDChannel::CDChannel(CoIsdnAdapter *pAdapter):
    m_pAdapter(pAdapter), m_OpenCount(0), m_TotalMakeCalls(0),
    m_TotalAnswerCalls(0)
{
}

CDChannel::~CDChannel()
{
    DestroyObjects();
}

NDIS_STATUS CDChannel::FinalConstruct(KNdisConfig& Config)
{
    NDIS_STATUS Result = ReadConfiguration(Config);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
    
    return CreateObjects(Config);
}

NDIS_STATUS CDChannel::ReadConfiguration(KNdisConfig& Config)
{
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CDChannel::CreateObjects(KNdisConfig& config)
{
    return NDIS_STATUS_SUCCESS;
}

VOID CDChannel::DestroyObjects()
{
}

NDIS_STATUS CDChannel::CloseCall(CBChannel *pBChannel)
{
    NDIS_STATUS Result = NDIS_STATUS_SUCCESS;
    ULONG CallState = pBChannel->GetCallState();

    if (CallState != 0 && CallState != LINECALLSTATE_IDLE)
    {
#if defined (SAMPLE_DRIVER)
        CBChannel *pPeerBChannel = pBChannel->GetPeerChannel();
        if (pPeerBChannel)
        {
            CardEvent *pEvent = pPeerBChannel->GetAdapter()->GetCard()->AllocateEvent(
                CARD_EVENT_DISCONNECT,
                pBChannel,
                pPeerBChannel
                );
            if (pEvent)
                pPeerBChannel->GetAdapter()->GetCard()->NotifyEvent(pEvent);

            pBChannel->SetPeerChannel(NULL);
        }
#else
        // TODO - Add code here to drop a call.        
#endif        
        pBChannel->GetAdapter()->FlushSendPackets(pBChannel);
    }
    else
    {
        g_trace.Trace("Channel %p ALREADY IDLE - CALLSTATE=%X\n", pBChannel, pBChannel->GetCallState());
    }
    return Result;
}

NDIS_STATUS CDChannel::Open()
{
    if (++m_OpenCount == 1)
    {
        g_trace.Trace("Opening DChannel %p\n", this);
    }
    else
    {
        g_trace.Trace("DChannel %p already opened\n", this);
    }
    return NDIS_STATUS_SUCCESS;
}

VOID CDChannel::Close()
{
    if (m_OpenCount == 1)
    {
        g_trace.Trace("Closing DChannel %p\n", this);

        --m_OpenCount;
    }
    else if (m_OpenCount == 0)
    {

        g_trace.Trace("DChannel %p already closed\n", this);
    }
    else
    {
        --m_OpenCount;
    }
}

NDIS_STATUS CDChannel::MakeCall(CBChannel *pBChannel, const basic_string<UCHAR>& rDialString, PLINE_CALL_PARAMS pCallParams)
{
    NDIS_STATUS Result = NDIS_STATUS_RESOURCES;
    CBChannel *pPeerBChannel = NULL;

    ++m_TotalMakeCalls;    

#if defined(SAMPLE_DRIVER)
    m_pAdapter->BChannelFromPhoneNumber(rDialString, &pPeerBChannel);
    if (pPeerBChannel)
    {
        pBChannel->SetPeerChannel(pPeerBChannel);
        CardEvent *pEvent = m_pAdapter->GetCard()->AllocateEvent(CARD_EVENT_RING, pBChannel, pPeerBChannel);
        if (pEvent)
        {
            m_pAdapter->GetCard()->NotifyEvent(pEvent);
            Result = NDIS_STATUS_PENDING;
        }
    }
    else
    {
        g_trace.Trace("Could not map phone number '%s' to BChannel\n", rDialString.c_str());
    }
#else // SAMPLE_DRIVER
    // TODO - Add code here to place a call.    
#endif // SAMPLE_DRIVER
    return Result;
}

VOID CDChannel::RejectCall(CBChannel *pBChannel)
{
#if defined(SAMPLE_DRIVER)
    CBChannel *pPeerBChannel = pBChannel->GetPeerChannel();

    if (pPeerBChannel)
    {
        CardEvent *pEvent = m_pAdapter->GetCard()->AllocateEvent(CARD_EVENT_DISCONNECT, pBChannel, pPeerBChannel);
        if (pEvent)
        {
            m_pAdapter->GetCard()->NotifyEvent(pEvent);
        }
        pBChannel->SetPeerChannel(NULL);

        //Put the BChannel back into the pool of available BChannels
        m_pAdapter->FreeIsdnChannel(pBChannel);        
    }
    else
    {
        g_trace.Trace("Channel %p had no peer channel - callstate=%X\n");
    }
#else // SAMPLE_DRIVER
    // TODO - Add code here to reject an incoming call.    
#endif //SAMPLE_DRIVER
}

NDIS_STATUS CDChannel::AnswerCall(CBChannel *pBChannel)
{
    ++m_TotalAnswerCalls;

#if defined(SAMPLE_DRIVER)
    CBChannel *pPeerChannel = pBChannel->GetPeerChannel();
    if (pPeerChannel)
    {
        CardEvent *pEvent = m_pAdapter->GetCard()->AllocateEvent(CARD_EVENT_CONNECT, pBChannel, pPeerChannel);
        if (pEvent == NULL)
            return  NDIS_STATUS_RESOURCES;
        m_pAdapter->GetCard()->NotifyEvent(pEvent);
    }
    else
    {
        g_trace.Trace("Invalid NULL Peer Channel Ecountered\n");
        return NDIS_STATUS_FAILURE;
    }
#else // SAMPLE_DRIVER
    // TODO - Add code here to answer a call.    
#endif // SAMPLE_DRIVER
    return NDIS_STATUS_SUCCESS;
}