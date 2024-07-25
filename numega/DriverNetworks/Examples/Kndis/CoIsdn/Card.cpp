// Card.cpp: implementation of the CCard class.
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

#include "Card.h"
#include "KNdisResource.h"
#include "KNdisConfig.h"
#include "CoIsdnAdap.h"
#include "KNdisPeripheralAddress.h"
#include "Port.h"

extern KNdisTrace g_trace;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCard::CCard(CoIsdnAdapter *pAdapter):
    m_pAdapter (pAdapter), m_BufferSize(CARD_DEFAULT_PACKET_SIZE),
    m_ReceiveBuffersPerLink(2), m_TransmitBuffersPerLink(2), m_NumDChannels(1),
    m_NumMessageBuffers(0), m_NumChannels(0)
#if defined(SAMPLE_DRIVER)
    , m_EventList(offsetof(CardEvent, Queue)),
    m_EventVector(MAX_EVENTS), m_NextEvent(0)
#endif    
{
#if defined(PCI_BUS)
    m_PciSlotNumber = 0;
#endif // PCI_BUS

}

NDIS_STATUS CCard::FinalConstruct(KNdisConfig& Config)
{
    NDIS_STATUS Result = ReadConfiguration(Config);
    
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
    
    Result = CreateObjects(Config);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
    
    m_pAdapter->SetAttributes(m_ResourceInfo.BusInterfaceType, m_ResourceInfo.Master);
    return Result;
}
    
CCard::~CCard()
{
    DestroyObjects();
}

NDIS_STATUS CCard::ReadConfiguration(KNdisConfig& Config)
{
    Config.Read(KNDIS_STRING_CONST("IsdnNumDChannels"), &m_NumDChannels);    
    Config.Read(KNDIS_STRING_CONST("ReceiveBuffersPerLink"), &m_ReceiveBuffersPerLink);    
    Config.Read(KNDIS_STRING_CONST("BufferSize"), &m_BufferSize);
    
#if defined(PCI_BUS)
    Config.Read(KNDIS_STRING_CONST("PCISlotNumber"), &m_PciSlotNumber);
#endif // PCI_BUS
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CCard::CreateObjects(KNdisConfig& Config)
{
    ULONG Index;
    NDIS_STATUS Result;
    
    Result = FindNIC(Config);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
    
    // Create a message interface to the NIC
    Result = CreateInterface();
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
    
    // Create the port objects
    ULONG nPorts = NumPorts();
    m_PortVector.resize(nPorts);
    for (Index = 0; Result == NDIS_STATUS_SUCCESS &&
        Index < nPorts; Index++)
    {
        CPort *pPort = new CPort(m_pAdapter, this);
        Result = pPort->FinalConstruct(Config);
        if (Result != NDIS_STATUS_SUCCESS)
        {
            delete pPort;
        }
        else
        {
            m_PortVector[Index] = pPort;
        }
    }

    // Allocate memory for incoming messages on the NIC
    if (Result == NDIS_STATUS_SUCCESS)
    {
        m_NumMessageBuffers = NumChannels() * m_ReceiveBuffersPerLink;
        Result = m_BufferHeap.Initialize(m_NumMessageBuffers, m_BufferSize);
        if (Result != NDIS_STATUS_SUCCESS)
            return Result;
    }

    // Prealocate the packet pool
    if (Result == NDIS_STATUS_SUCCESS)
    {
        Result = m_PacketPool.Initialize(m_NumMessageBuffers, 0);
        if (Result != NDIS_STATUS_SUCCESS)
            return Result;
    }
    return Result;
}

VOID CCard::DestroyObjects()
{
    // Deallocate all the resources allocated
#if defined CARD_MIN_IOPORT_SIZE    
    m_ResourceInfo.m_Ports.Invalidate();
#endif
    
#if defined CARD_MIN_MEMORY_SIZE
    m_ResourceInfo.m_Memory.Invalidate();
#endif
    
#if defined CARD_REQUEST_ISR
    m_ResourceInfo.m_Interrupt.Invalidate();
#endif
    
    // Delete all port objects
    vector<CPort*>::iterator itPorts = m_PortVector.begin();
    while (itPorts != m_PortVector.end())
    {
        CPort* pPort = *itPorts;
        delete pPort;
        itPorts = m_PortVector.erase(itPorts);
    }
    m_BufferHeap.Invalidate();
    m_PacketPool.Invalidate();
}

NDIS_STATUS CCard::FindNIC(KNdisConfig& Config)
{
    NDIS_STATUS Result;
    KNdisPnpResourceRequest request(Config);

#if defined(CARD_MIN_IOPORT_SIZE)
    KNdisResource<CmResourceTypePort> Port(request);
    if (!Port.IsValid())
        KNDIS_RETURN_ERROR (NDIS_STATUS_NOT_ACCEPTED);
    Result = m_ResourceInfo.m_Ports.Initialize(m_pAdapter, Port);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
#endif

#if defined(CARD_MIN_MEMORY_SIZE)
    KNdisResource<CmResourceTypeMemory>	Memory(request);
    if (!Memory.IsValid())
        KNDIS_RETURN_ERROR (NDIS_STATUS_NOT_ACCEPTED);
    Result = m_ResourceInfo.m_Memory.Initialize(m_pAdapter, Memory);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
#endif

#if defined (CARD_REQUEST_ISR)
    KNdisResource<CmResourceTypeInterrupt> Int(request);
    if (!Int.IsValid())
        KNDIS_RETURN_ERROR (NDIS_STATUS_NOT_ACCEPTED);
    Result = m_ResourceInfo.m_Interrupt.Initialize(m_pAdapter, Int);
    if (Result != NDIS_STATUS_SUCCESS)
        return Result;
#endif    
    // Make sure the resources are available:
    
    m_ResourceInfo.Master = CARD_IS_BUS_MASTER;
#if (CARD_IS_BUS_MASTER)
    m_ResourceInfo.Dma.DmaChannel = 0;
    m_ResourceInfo.Dma.Dma32BitAddresses = TRUE;
    m_ResourceInfo.Dma.MaximumPhysicalMapping = m_BufferSize;
    m_ResourceInfo.Dma.PhysicalMapRegistersNeeded = CARD_MAP_REGISTERS_NEEDED;

    if (m_ResourceInfo.Master)
    {
        ASSERT(m_ResourceInfo.DmaChannel == 0 ||
            m_ResourceInfo.BusInterfaceType == NdisInterfaceIsa);
        Result = NdisMAllocateMapRegisters(
            *m_pAdapter,
            m_ResourceInfo.DmaChannel,
            m_ResourceInfo.Dma32BitAddresses,
            m_ResourceInfo.PhysicalMapRegistersNeeded + 1,
            m_ResourceInfo.MaximumPhysicalMapping
            );
    }
        
#endif // (CARD_IS_BUS_MASTER)

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CCard::CreateInterface()
{
    return NDIS_STATUS_SUCCESS;
}

ULONG CCard::NumPorts() const
{
    return m_NumDChannels;
}

ULONG CCard::NumChannels() const
{
    if (m_NumChannels == 0)
    {
        for (vector<CPort*>::const_iterator itPort = m_PortVector.begin(); itPort != m_PortVector.end(); itPort++)
        {
            m_NumChannels += (*itPort)->NumChannels();
        }
    }
    return m_NumChannels;
}

#if defined(SAMPLE_DRIVER)
CardEvent* CCard::AllocateEvent(enuCardEventCode code,
        CEventReceiver *pSender, 
        CEventReceiver *pReceiver,
        PNDIS_PACKET pSentPacket,
        PNDIS_PACKET pReceivedPacket)
{
    CardEvent *pEvent = m_EventVector.begin() + m_NextEvent;
    ++m_NextEvent;

    m_NextEvent %= MAX_EVENTS;
    
    pEvent->Initialize(code, pSender, pReceiver, pSentPacket, pReceivedPacket);
    return pEvent;
}
#endif

VOID CCard::ReleaseEvent(CardEvent *pEvent)
{
    pEvent->pReceivingObject = NULL;
}

VOID CCard::NotifyEvent(CardEvent *pEvent)
{
    m_EventList.InsertTail(pEvent);
    if (pEvent->eEventCode == CARD_EVENT_RING || 
        pEvent->eEventCode == CARD_EVENT_DISCONNECT ||
        pEvent->eEventCode == CARD_EVENT_CONNECT)
    {
        m_pAdapter->ScheduleEvent(100);
    }
    else
    {
        m_pAdapter->ScheduleEvent(0);
    }
}

bool CCard::TransmitPacket(CBChannel *pBChannel, KNdisPacket& packet)
{
    bool bResult = false;

#if defined(SAMPLE_DRIVER)
    CBChannel *pPeerChannel = pBChannel->GetPeerChannel();
    if (pPeerChannel)
    {
        CardEvent *pEvent = AllocateEvent(CARD_EVENT_RECEIVE, pBChannel, pPeerChannel, packet);
        if (pEvent)
        {
            // Append the packet onto TransmitBusyList while it is being sent.
            // Then move it to the TransmitCompleteList in CardInterruptHandler
            // after the card is done with it.
            pBChannel->RecordInProgressTransmission(packet);
            pPeerChannel->GetAdapter()->GetCard()->NotifyEvent(pEvent);
            bResult = TRUE;
        }
    }
    else
    {
        g_trace.Trace("pPeerBChannel == NULL\n");
    }
#else //SAMPLE_DRIVER
    
    // TODO - Add code here to transmit the packet.\    
#endif

    return (bResult);    
}

VOID CCard::InterruptHandler()
{
#if defined(SAMPLE_DRIVER)
    m_EventList.Lock();
    while (!m_EventList.IsEmptyNoLock())
    {
        CardEvent *pEvent = m_EventList.RemoveHeadNoLock();
        m_EventList.Unlock();

        switch(pEvent->eEventCode)
        {
        case CARD_EVENT_RECEIVE:
            {
                // Allocate the received buffer and packet from 
                // the buffer heap and packet pool.
                ASSERT (pEvent->pNdisSentPacket);

                if (!pEvent->pNdisSentPacket)
                    break;

                KNdisBuffer IncomingBuffer = m_BufferHeap.Allocate();
                if (IncomingBuffer.IsValid())
                {
                    KNdisPacket sentPacket(pEvent->pNdisSentPacket);
                    KNdisPacket receivedPacket = m_PacketPool.Allocate();

                    if (receivedPacket.IsValid())
                    {
                        UINT BytesReceived = sentPacket.QueryTotalLength();
                        ASSERT(BytesReceived <= m_BufferSize);
                        
                        receivedPacket.ChainAtFront(IncomingBuffer);
                        UINT BytesCopied = sentPacket.CopyFrom(receivedPacket, 0, min((ULONG)BytesReceived, m_BufferSize), 0);
                        ASSERT (BytesCopied == BytesReceived);
                        IncomingBuffer.AdjustLength(BytesCopied);
                        pEvent->pNdisReceivedPacket = receivedPacket;
                    }
                    else
                    {
                        m_BufferHeap.Free(IncomingBuffer);
                    }
                }
            }
        case CARD_EVENT_RING:
        case CARD_EVENT_CONNECT:
        case CARD_EVENT_DISCONNECT:
        case CARD_EVENT_TRANSMIT_COMPLETE:
            pEvent->pReceivingObject->OnEvent(*pEvent);
            break;
        default:
            g_trace.Trace("Unknown event code: %d\n", pEvent->eEventCode);
            break;
        }
        ReleaseEvent(pEvent);
        
        m_EventList.Lock();
    }
    m_EventList.Unlock();
#else // SAMPLE_DRIVER
    // TODO - Add interrupt handler code here    
#endif // SAMPLE_DRIVER
}

VOID CCard::ReturnPacket(KNdisPacket& rPacket)
{
    KNdisBuffer retBuff = rPacket.UnChainAtFront();
    retBuff.AdjustLength(m_BufferSize);
    m_BufferHeap.Free(retBuff);
    m_PacketPool.FreeDpr(rPacket);
}

NDIS_STATUS CCard::Reset()
{
    return NDIS_STATUS_SUCCESS;
}
