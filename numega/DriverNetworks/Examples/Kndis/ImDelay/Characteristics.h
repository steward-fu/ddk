//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
// file generated by the wizard

#ifndef __CHARACTERISTICS_H__
#define __CHARACTERISTICS_H__

// Optional NDIS MINIPORT handlers this driver supports
KNDIS_MINIPORT_HANDLER(ImDelayAdapter, Send)
KNDIS_MINIPORT_HANDLER(ImDelayAdapter, ReturnPacket)
KNDIS_MINIPORT_HANDLER(ImDelayAdapter, TransferData)
#if defined(NDIS51_MINIPORT)
KNDIS_MINIPORT_HANDLER(ImDelayAdapter, CancelSendPackets)
#endif

// Optional NDIS PROTOCOL handlers this driver supports
KNDIS_PROTOCOL_HANDLER(KNdisFilterBinding, SendComplete)
KNDIS_PROTOCOL_HANDLER(KNdisFilterBinding, TransferDataComplete)
KNDIS_PROTOCOL_HANDLER(KNdisFilterBinding, Receive)
KNDIS_PROTOCOL_HANDLER(KNdisFilterBinding, ReceiveComplete)
KNDIS_PROTOCOL_HANDLER(KNdisFilterBinding, ReceivePacket)
KNDIS_PROTOCOL_HANDLER(KNdisFilterBinding, Status)
KNDIS_PROTOCOL_HANDLER(KNdisFilterBinding, StatusComplete)
KNDIS_PROTOCOL_HANDLER(KNdisFilterBinding, PnpEvent)

// TODO:
// Add other #defines as needed


#endif // __CHARACTERISTICS_H__
 