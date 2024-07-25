// KNdisMiniportVc.h: interface for the KNdisMiniportVc class.
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

#ifndef __KNDIS_MINIPORT_VC_H__
#define __KNDIS_MINIPORT_VC_H__

#include "KObject.h"

class KNdisMiniportVc
{
public:
    VOID CoSendPackets(
            IN PPNDIS_PACKET  PacketArray,
            IN UINT  NumberOfPackets
            )
	{
		UNREFERENCED_PARAMETERS2(PacketArray, NumberOfPackets);
	}

    NDIS_STATUS CoActivateVc(
            IN OUT PCO_CALL_PARAMETERS  CallParameters
            )
	{
		UNREFERENCED_PARAMETER(CallParameters);
		return NDIS_STATUS_NOT_RECOGNIZED;
	}

    NDIS_STATUS CoDeactivateVc(
            ){return NDIS_STATUS_NOT_RECOGNIZED;}

    NDIS_STATUS CoRequest(
            IN OUT PNDIS_REQUEST  NdisRequest
            )
	{
		UNREFERENCED_PARAMETER(NdisRequest);
		return NDIS_STATUS_NOT_RECOGNIZED;
	}
};

#endif // __KNDIS_MINIPORT_VC_H__
