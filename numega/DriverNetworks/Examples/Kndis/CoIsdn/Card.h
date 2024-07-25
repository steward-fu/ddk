// Card.h: interface for the CCard class.
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

#if !defined(AFX_CARD_H__0386D37C_BCBA_4E5C_9B69_58AE4A46DD36__INCLUDED_)
#define AFX_CARD_H__0386D37C_BCBA_4E5C_9B69_58AE4A46DD36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _CTYPE_DISABLE_MACROS
#define _INC_WCHAR

#include "KNdis.h"
#include "CardResource.h"
#include <kstl.h>
#include "vector"
#include "KNdisBufferHeap.h"
#include "KNdisPacketPool.h"

#if defined(SAMPLE_DRIVER)
#include "CardEvents.h"
#endif
using namespace std;

#define CARD_IS_BUS_MASTER              FALSE
// TODO - Is the card a bus master device?  (TRUE or FALSE)
#if (CARD_IS_BUS_MASTER)
#   define CARD_MAP_REGISTERS_NEEDED    NUM_DEV_PER_ADAP
// TODO - How many map registers needed to transmit data to card.
#endif

//#define CARD_REQUEST_ISR                TRUE
// TODO - How do you want to handle interrupts from the card?
// TRUE if you want to always use MiniportISR().
// FALSE if you want to use MiniportDisable() and MiniportEnable().
// Undefined if your card does not generate interrupts.

#if defined(CARD_REQUEST_ISR)

#define CARD_INTERRUPT_SHARED           TRUE
// TODO - Is your interrupt shared? (TRUE or FALSE).

#define CARD_INTERRUPT_MODE             NdisInterruptLevelSensitive
// TODO - Is your interrupt latched or level sensitve?

#endif // defined(CARD_REQUEST_ISR)

// Number of digits allowed in a phone number (not including spaces).
#define CARD_MAX_DIAL_DIGITS            32

/*
// Maximum packet size allowed by the adapter -- must be restricted to
// 1500 bytes at this point, and must also allow for frames at least 32
// bytes longer.
*/
#define NDISWAN_EXTRA_SIZE              32
#define CARD_MIN_PACKET_SIZE            ( 480 + NDISWAN_EXTRA_SIZE)
#define CARD_MAX_PACKET_SIZE            (2016 + NDISWAN_EXTRA_SIZE)
#define CARD_DEFAULT_PACKET_SIZE        (1504 + NDISWAN_EXTRA_SIZE)

//////////////////////////////////////////////////////////////////////
//Forward declarations
class CoIsdnAdapter;
class KNdisConfig;
class CPort;
class CBChannel;
struct CardEvent;

//////////////////////////////////////////////////////////////////////
// CCard
//
// A class representing the physical ISDN device.
class CCard
{
public:
	CCard(CoIsdnAdapter *pAdapter);
	~CCard();

    ULONG NumPorts() const;
    ULONG NumChannels() const;
    NDIS_STATUS FinalConstruct(KNdisConfig& Config);
    
    inline ULONG GetBufferSize() const
    {
        return m_BufferSize;
    }
    inline ULONG TransmitBuffersPerLink() const
    {
        return m_TransmitBuffersPerLink;
    }

    bool TransmitPacket(CBChannel *pBChannel, KNdisPacket& packet);

    VOID InterruptHandler();
    VOID ReturnPacket(KNdisPacket& rPacket);
    NDIS_STATUS Reset();

#if defined(SAMPLE_DRIVER)
    CardEvent* AllocateEvent(enuCardEventCode code, 
        CEventReceiver *pSender = NULL, 
        CEventReceiver *pReceiver = NULL,
        PNDIS_PACKET pSentPacket = NULL,
        PNDIS_PACKET pReceivedPacket = NULL);
    VOID ReleaseEvent(CardEvent *pEvent);
    VOID NotifyEvent(CardEvent *pEvent);
#endif

protected:
    NDIS_STATUS ReadConfiguration(KNdisConfig& Config);
    NDIS_STATUS CreateObjects(KNdisConfig& config);
    VOID DestroyObjects();

private:
    // Called to locate the NIC on the PCI bus
    NDIS_STATUS FindNIC(KNdisConfig& Config);
    // Called to allocate a shared memory pool to be used to establish 
    // the message interface between the adapter and the NIC.
    NDIS_STATUS CreateInterface();

protected:
    // Owning adapter object
    CoIsdnAdapter *m_pAdapter;

    // Card configuration data
#if defined(PCI_BUS)
    int m_PciSlotNumber;
#endif // PCI_BUS
    
    ULONG m_BufferSize;
    ULONG m_ReceiveBuffersPerLink;
    ULONG m_TransmitBuffersPerLink;
    ULONG m_NumDChannels;

    // Total number of message buffers preallocated for incoming messages
    mutable ULONG m_NumMessageBuffers;
    // Total number of channels on the NIC
    mutable ULONG m_NumChannels;

    // Hardware resources requirements
    CardResource m_ResourceInfo;

    // An array of Port Objects created by PortCreate.
    // One entry for each port on NIC.
    vector<CPort*> m_PortVector;

    // Spin lock to protect the buffer list
    KNdisSpinLock m_Bufferlock;
    // Preallocated buffer space and descriptors for incoming packets
    KNdisBufferHeap m_BufferHeap;
    // Preallocated Packet Pool
    KNdisPacketPool m_PacketPool;

    
#if defined (SAMPLE_DRIVER)
    //Events lists managed by the card
    KNdisInterlockedList<CardEvent> m_EventList;
#define MAX_EVENTS 32
    // Card event allocation array.
    vector<CardEvent> m_EventVector;
    
    // Index into EventArray.
    ULONG m_NextEvent;    
#endif
};

#endif // !defined(AFX_CARD_H__0386D37C_BCBA_4E5C_9B69_58AE4A46DD36__INCLUDED_)
