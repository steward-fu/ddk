// CardResource.h: Definition of the I/O resources wrapper for the logical card object.
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

#include "KNdis.h"
#include "KNdisPeripheralAddress.h"

///////////////////////////////////////////////////////////////////////
// A structure that wraps hardware resource requirements
//
struct CardResource
{
    CardResource()
    {
        NdisZeroMemory(this, sizeof(*this));
    }
    
    NDIS_INTERFACE_TYPE         BusInterfaceType;           // @field
    // This value is used to tell NDIS what type of adapter this is.
    // This is usually the same as the registry parameter BusType, but
    // may be different in the case of a bridged adapter.

    BOOLEAN                     Master;                     // @field
    // This is TRUE if the adapter is capable of bus master transfers.
    // Use the <t CARD_IS_BUS_MASTER> defininition to set this value
    // so the other bus master values will be included if needed.
    // See <f NdisMAllocateMapRegisters> for more details on the bus
    // master parameters.

    struct _tagDma{
        BOOLEAN                     Dma32BitAddresses;          // @field
        // This is TRUE if the bus master device uses 32-bit addresses.
        // Almost always TRUE for today's devices.
        
        ULONG                       PhysicalMapRegistersNeeded; // @field
        // This should be set to the maximum number of outstanding DMA
        // transfers that can be active at one time.  One for each physical
        // buffer segment.
        
        ULONG                       MaximumPhysicalMapping;     // @field
        // This should be set to the maximum number of contigous bytes that
        // can make up a single DMA transfer.
        
        ULONG                       DmaChannel;                 // @field
        // This should only be set if your adapter is an ISA bus master and
        // requires the use of one of the host DMA channels.
        
    }Dma;

#if defined CARD_MIN_IOPORT_SIZE
    // I/O Port
    KNdisIoRange		m_Ports;
#endif
    
#if defined CARD_MIN_MEMORY_SIZE
    // Memory Range
    KNdisMemoryRange	m_Memory;
#endif
    
#if defined CARD_REQUEST_ISR
    // Interrupt
    KNdisInterrupt		m_Interrupt;
#endif
};
