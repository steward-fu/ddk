// CallManagerVcTraits.h: interface for the CallManagerVcTraits class.
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

#ifndef __CALL_MANAGER_VC_TRAITS_H__
#define __CALL_MANAGER_VC_TRAITS_H__

#include "Kndis.h"
#include "KNdisCallManagerVcTraits.h"
#include "CoIsdnAdap.h"
#include "BChannel.h"


template<>
CBChannel* KNdisCallManagerVcTraits<CoIsdnAdapter, CBChannel>::CreateInstance(
    NDIS_HANDLE ProtocolAfContext, NDIS_HANDLE NdisVcHandle)
{
    CBChannel *pBChannel = NULL;
    NDIS_STATUS Result = (reinterpret_cast<CoIsdnAdapter*>(ProtocolAfContext))->GetAvailableIsdnChannel(NdisVcHandle, &pBChannel);
    return pBChannel;
}

template<>
NDIS_STATUS KNdisCallManagerVcTraits<CoIsdnAdapter, CBChannel>::DeleteInstance(CBChannel *pBChannel)
{
    return pBChannel->GetAdapter()->FreeIsdnChannel(pBChannel);
}

#endif //__CALL_MANAGER_VC_TRAITS_H__