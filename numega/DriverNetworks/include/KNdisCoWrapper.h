// KNdisCoWrapper.h: interface for the KNdisCoWrapper class.
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

#ifndef __KNdisCoWrapper_h__
#define __KNdisCoWrapper_h__

#include "KNdis.h"
#include "KNdisWrapper.h"
#include "KNdisMiniportVcTraits.h"

//////////////////////////////////////////////////////////////////////
// KNdisCoWrapper
//
// A template class that maps native NDIS callbacks into method invokations
// on KNdisVc-derived objects. Because it derives from KNdisWrapper<Adapter>,
// this class inherits the disptaching capabilities of the KNdisWrapper
// class: it acts as a dispatcher of calls to KNdisMiniAdapter-derived
// objects.
//
// This class redefines the CoSendPackets, CoActivateVc, CoDeactivateVc,
// CoRequest, CoCreateVc and CoDeleteVc methods of connection-oriented
// miniport drivers to implemented proper dispatching of calls to  KNdisVc
// -derived instance.
//
// This class has no dynamic instances. Its static methods are instantiated
// by means of references to them in Characteristics.h, which is generated
// by the Wizard.
template <class Adapter, class Vc>
class KNdisCoWrapper: public KNdisWrapper<Adapter>
{
public:
    // called directly from NDIS: miniport handlers
    static VOID CoSendPackets(
            IN NDIS_HANDLE              MiniportVcContext,
            IN PPNDIS_PACKET            PacketArray,
            IN UINT                     NumberOfPackets
            );

    static NDIS_STATUS CoActivateVc(
            IN NDIS_HANDLE              MiniportVcContext,
            IN OUT PCO_CALL_PARAMETERS  CallParameters
            );

    static NDIS_STATUS CoDeactivateVc(
            IN NDIS_HANDLE              MiniportVcContext
            );

    static NDIS_STATUS CoRequest(
            IN NDIS_HANDLE              MiniportAdapterContext,
            IN NDIS_HANDLE              MiniportVcContext OPTIONAL,
            IN OUT PNDIS_REQUEST        NdisRequest
            );

    static NDIS_STATUS CoCreateVc(
            IN	NDIS_HANDLE				MiniportAdapterContext,
            IN	NDIS_HANDLE				NdisVcHandle,
            OUT	PNDIS_HANDLE			MiniportVcContext
            );

    static NDIS_STATUS CoDeleteVc(
            IN	NDIS_HANDLE				MiniportVcContext
            );
};

template <class Adapter, class Vc>
VOID KNdisCoWrapper<Adapter, Vc>::CoSendPackets(
            IN NDIS_HANDLE              MiniportVcContext,
            IN PPNDIS_PACKET            PacketArray,
            IN UINT                     NumberOfPackets
            )
{
    (static_cast<Vc*>(MiniportVcContext))->CoSendPackets(PacketArray, NumberOfPackets);
}

template <class Adapter, class Vc>
NDIS_STATUS KNdisCoWrapper<Adapter, Vc>::CoActivateVc(
            IN NDIS_HANDLE              MiniportVcContext,
            IN OUT PCO_CALL_PARAMETERS  CallParameters
            )
{
    return (static_cast<Vc*>(MiniportVcContext))->CoActivateVc(CallParameters);
}

template <class Adapter, class Vc>
NDIS_STATUS KNdisCoWrapper<Adapter, Vc>::CoDeactivateVc(
            IN NDIS_HANDLE              MiniportVcContext
            )
{
    return (static_cast<Vc*>(MiniportVcContext))->CoDeactivateVc();
}

template <class Adapter, class Vc>
NDIS_STATUS KNdisCoWrapper<Adapter, Vc>::CoRequest(
            IN NDIS_HANDLE              MiniportAdapterContext,
            IN NDIS_HANDLE              MiniportVcContext OPTIONAL,
            IN OUT PNDIS_REQUEST        NdisRequest
            )
{
    return (static_cast<Adapter*>(MiniportAdapterContext))->CoRequest(static_cast<Vc*>(MiniportVcContext), NdisRequest);
}

template <class Adapter, class Vc>
NDIS_STATUS KNdisCoWrapper<Adapter, Vc>::CoCreateVc(
            IN NDIS_HANDLE              MiniportAdapterContext,
            IN NDIS_HANDLE              NdisVcHandle,
            OUT PNDIS_HANDLE            MiniportVcContext
            )
{
    *MiniportVcContext = NULL;

    Vc *pVc = KNdisMiniportVcTraits<Adapter, Vc>::CreateInstance(reinterpret_cast<Adapter*>(MiniportAdapterContext), NdisVcHandle);
    if (pVc != NULL)
    {
        *MiniportVcContext = pVc;
        return NDIS_STATUS_SUCCESS;
    }

    return NDIS_STATUS_VC_NOT_AVAILABLE;
}

template <class Adapter, class Vc>
NDIS_STATUS KNdisCoWrapper<Adapter, Vc>::CoDeleteVc(
            IN	NDIS_HANDLE				MiniportVcContext
            )
{
    return KNdisMiniportVcTraits<Adapter, Vc>::DeleteInstance(reinterpret_cast<Vc*>(MiniportVcContext));
}

#endif // __KNdisCoWrapper_h__
