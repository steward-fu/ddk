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

// Optional NDIS PROTOCOL handlers this driver supports
KNDIS_PROTOCOL_HANDLER(NmPacketBinding, SendComplete)
KNDIS_PROTOCOL_HANDLER(NmPacketBinding, TransferDataComplete)
KNDIS_PROTOCOL_HANDLER(NmPacketBinding, Receive)
KNDIS_PROTOCOL_HANDLER(NmPacketBinding, ReceiveComplete)
KNDIS_PROTOCOL_HANDLER(NmPacketBinding, ReceivePacket)
KNDIS_PROTOCOL_HANDLER(NmPacketBinding, Status)
KNDIS_PROTOCOL_HANDLER(NmPacketBinding, StatusComplete)
KNDIS_PROTOCOL_HANDLER(NmPacketBinding, PnpEvent)

// TODO:
// Add other #defines as needed


#endif // __CHARACTERISTICS_H__
 