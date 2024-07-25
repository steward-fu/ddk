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

// Optional NDIS PROTOCOL handlers this driver supports
KNDIS_PROTOCOL_HANDLER(NmProtBinding, SendComplete)
KNDIS_PROTOCOL_HANDLER(NmProtBinding, TransferDataComplete)
KNDIS_PROTOCOL_HANDLER(NmProtBinding, Receive)
KNDIS_PROTOCOL_HANDLER(NmProtBinding, ReceiveComplete)
KNDIS_PROTOCOL_HANDLER(NmProtBinding, ReceivePacket)
KNDIS_PROTOCOL_HANDLER(NmProtBinding, Status)
KNDIS_PROTOCOL_HANDLER(NmProtBinding, StatusComplete)
KNDIS_PROTOCOL_HANDLER(NmProtBinding, PnpEvent)

// TODO:
// Add other #defines as needed


#endif // __CHARACTERISTICS_H__
