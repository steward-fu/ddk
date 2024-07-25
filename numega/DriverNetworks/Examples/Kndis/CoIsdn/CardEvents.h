// CardEvents.h Declaration of the event class used to simulate the hardware
//              operation.
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

#ifndef __CARD_EVENT_H__
#define __CARD_EVENT_H__

#include "KNdis.h"

#if !defined(CARD_REQUEST_ISR)

///////////////////////////////////////////////////////////////////////
// Forward declarations
class CEventReceiver;

///////////////////////////////////////////////////////////////////////
// This enumeration defines the events generated by the card.
typedef enum
{
    CARD_EVENT_NULL,
    // Not used for anything.

    CARD_EVENT_RING,
    // Indicates that a call is incoming on the given BChannel.

    CARD_EVENT_CONNECT,
    // Indicates that a call is connected on the given BChannel.

    CARD_EVENT_DISCONNECT,
    // Indicates that a call is disconnected on the given BChannel.

    CARD_EVENT_RECEIVE,
    // Indicates that a packet is incoming on the given BChannel.

    CARD_EVENT_TRANSMIT_COMPLETE
    // Indicates that the transmit is complete on the given BChannel.

} enuCardEventCode;

///////////////////////////////////////////////////////////////////////
// This structure is used to keep track of events passed between the
// callee and caller.  Each <t CARD_OBJECT> keeps a list of these events.
//
struct CardEvent
{
    CardEvent(enuCardEventCode code = CARD_EVENT_NULL, 
        CEventReceiver *pSender = NULL, 
        CEventReceiver *pReceiver = NULL,
        PNDIS_PACKET pSentPacket = NULL,
        PNDIS_PACKET pReceivedPacket = NULL
        ):
        eEventCode(code), pSendingObject(pSender), pReceivingObject(pReceiver), 
        pNdisSentPacket(pSentPacket), pNdisReceivedPacket(pReceivedPacket)
    {
    }

    VOID Initialize(enuCardEventCode code, 
        CEventReceiver *pSender = NULL, 
        CEventReceiver *pReceiver = NULL,
        PNDIS_PACKET pSentPacket = NULL,
        PNDIS_PACKET pReceivedPacket = NULL
        )
    {
        eEventCode = code;
        pSendingObject = pSender;
        pReceivingObject = pReceiver;
        pNdisSentPacket = pSentPacket;
        pNdisReceivedPacket = pReceivedPacket;
    }

    LIST_ENTRY                  Queue;
    // Used to place the buffer on one of the receive lists.

    enuCardEventCode            eEventCode;
    // Reason for event notification.

    CEventReceiver              *pSendingObject;
    // Interface object that is notifying.

    CEventReceiver              *pReceivingObject;
    // Interface object that is notifying.

    PNDIS_PACKET                pNdisSentPacket;
    // A pointer to the sent NDIS packet structure

    PNDIS_PACKET                pNdisReceivedPacket;
    // A pointer to the received NDIS packet structure
};

///////////////////////////////////////////////////////////////////////
// An abstract base class representing a subscriber in an instantiation
// of the publisher-subscriber pattern
class CEventReceiver
{
public:
    virtual void OnEvent(CardEvent&) = 0;
};

#endif // !defined(CARD_REQUEST_ISR)

#endif //__CARD_EVENT_H__