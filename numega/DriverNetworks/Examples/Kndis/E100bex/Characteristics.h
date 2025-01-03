// Characteristics.h: originally generated by the Quick Miniport wizard.
//
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


#ifndef __CHARACTERISTICS_H__
#define __CHARACTERISTICS_H__


// Declare Optional NDIS MiniportXxx handlers this driver supports

KNDIS_MINIPORT_HANDLER(E100bexAdapter, CheckForHang)
KNDIS_MINIPORT_HANDLER(E100bexAdapter, DisableInterrupt)
KNDIS_MINIPORT_HANDLER(E100bexAdapter, EnableInterrupt)
KNDIS_MINIPORT_HANDLER(E100bexAdapter, HandleInterrupt)
KNDIS_MINIPORT_HANDLER(E100bexAdapter, Isr)
KNDIS_MINIPORT_HANDLER(E100bexAdapter, SendPackets)
KNDIS_MINIPORT_HANDLER(E100bexAdapter, ReturnPacket)
KNDIS_MINIPORT_HANDLER(E100bexAdapter, AllocateComplete)

// Driver version
#define	E100BEX_VERSION_MAJOR		1
#define	E100BEX_VERSION_MINOR		0

// OID Constants
#define	E100BEX_MAX_LOOKAHEAD		256
#define	E100BEX_MAX_FRAMESIZE		1500		// w/o header
#define	E100BEX_MAX_TOTAL_SIZE		1514		// incl. header
#define	E100BEX_MAX_LINKSPEED		10000000L
#define	E100BEX_TRANSMIT_BLOCK		E100BEX_MAX_LOOKAHEAD
#define	E100BEX_RECEIVE_BLOCK		E100BEX_MAX_LOOKAHEAD

#define E100BEX_TX_BUFFER_SPACE		16000				
#define E100BEX_RX_BUFFER_SPACE		16000

#define	E100BEX_MAX_SEND_PACKETS	4

#define	E100BEX_802_3_MAX_LIST_SIZE	8

// PnP Support
#define E100BEX_SUPPORT_PNP			1


#endif // __CHARACTERISTICS_H__
 