// KNdisMiniportVcTraits.h: interface for the KNdisMiniportVcTraits class.
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
#ifndef __MINIPORT_VC_TRAITS_H__
#define __MINIPORT_VC_TRAITS_H__

#include "Kndis.h"
#include "KNdisMiniportVcTraits.h"
#include "CoIsdnAdap.h"
#include "BChannel.h"

template<>
CBChannel* KNdisMiniportVcTraits<CoIsdnAdapter, CBChannel>::CreateInstance(
        CoIsdnAdapter *pAdapter, NDIS_HANDLE NdisVcHandle)
{
    CBChannel *pBChannel = NULL;
    NDIS_STATUS Result = pAdapter->GetAvailableIsdnChannel(NdisVcHandle, &pBChannel);
    return pBChannel;
}

template<>
NDIS_STATUS KNdisMiniportVcTraits<CoIsdnAdapter, CBChannel>::DeleteInstance(CBChannel *pBChannel)
{
    return pBChannel->GetAdapter()->FreeIsdnChannel(pBChannel);
}

#endif // __MINIPORT_VC_TRAITS_H__