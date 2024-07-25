// KNdisCallManagerWrapper.h: interface for the KNdisCallManagerWrapper class.
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

#ifndef __KNDIS_CALL_MANAGER_WRAPPER_H__
#define __KNDIS_CALL_MANAGER_WRAPPER_H__

#include "KNdis.h"
#include "KNdisCallManagerVcTraits.h"

//////////////////////////////////////////////////////////////////////
// KNdisCallManagerWrapper
//
// A template class that maps native NDIS callbacks into method invokations
// on KNdisCallManagerVc-derived objects. Because it derives from
// KNdisWrapper<Adapter>, this class inherits the disptaching
// capabilities of the KNdisWrapper class: it acts as a dispatcher of
// calls to KNdisMiniAdapter-derived objects.
//
// This class redefines the CoSendPackets, CoActivateVc, CoDeactivateVc,
// CoRequest, CoCreateVc and CoDeleteVc methods of connection-oriented
// miniport drivers to implemented proper dispatching of calls to  KNdisVc
// -derived instance.
//
// This class has no dynamic instances. Its static methods are instantiated
// by means of references to them in Characteristics.h, which is generated
// by the Wizard.
template <class Binding, class Vc>
class KNdisCallManagerWrapper
{
public:
    static NDIS_STATUS CmCreateVc(
            IN NDIS_HANDLE              ProtocolAfContext,
            IN NDIS_HANDLE              NdisVcHandle,
            OUT PNDIS_HANDLE            ProtocolVcContext
            );

    static NDIS_STATUS CmDeleteVc(
            IN NDIS_HANDLE              ProtocolVcContext
            );

    static NDIS_STATUS CmOpenAf(
            IN NDIS_HANDLE              CallMgrBindingContext,
            IN PCO_ADDRESS_FAMILY       AddressFamily,
            IN NDIS_HANDLE              NdisAfHandle,
            OUT PNDIS_HANDLE            CallMgrAfContext
            );

    static NDIS_STATUS CmRegisterSap(
            IN NDIS_HANDLE              CallMgrAfContext,
            IN PCO_SAP                  Sap,
            IN NDIS_HANDLE              NdisSapHandle,
            OUT PNDIS_HANDLE            CallMgrSapContext
            );

    static NDIS_STATUS CmDeregisterSap(
            IN NDIS_HANDLE              CallMgrSapContext
            );

    static NDIS_STATUS CmCloseAf(
            IN NDIS_HANDLE              CallMgrAfContext
            );

    static NDIS_STATUS CmMakeCall(
            IN NDIS_HANDLE              CallMgrVcContext,
            IN OUT PCO_CALL_PARAMETERS  CallParameters,
            IN NDIS_HANDLE              NdisPartyHandle OPTIONAL,
            OUT PNDIS_HANDLE            CallMgrPartyContext OPTIONAL
            );

    static NDIS_STATUS CmCloseCall(
            IN NDIS_HANDLE              CallMgrVcContext,
            IN NDIS_HANDLE              CallMgrPartyContext OPTIONAL,
            IN PVOID                    CloseData OPTIONAL,
            IN UINT                     Size  OPTIONAL
            );

    static VOID CmIncomingCallComplete(
            IN NDIS_STATUS              Status,
            IN NDIS_HANDLE              CallMgrVcContext,
            IN PCO_CALL_PARAMETERS      CallParameters
            );

    static NDIS_STATUS CmAddParty(
            IN NDIS_HANDLE              CallMgrVcContext,
            IN OUT PCO_CALL_PARAMETERS  CallParameters,
            IN NDIS_HANDLE              NdisPartyHandle,
            OUT PNDIS_HANDLE            CallMgrPartyContext
            );

    static NDIS_STATUS CmDropParty(
            IN NDIS_HANDLE              CallMgrPartyContext,
            IN PVOID                    CloseData OPTIONAL,
            IN UINT                     Size OPTIONAL
            );

    static VOID CmActivateVcComplete(
            IN NDIS_STATUS              Status,
            IN NDIS_HANDLE              CallMgrVcContext,
            IN PCO_CALL_PARAMETERS       CallParameters
            );

    static VOID CmDeactivateVcComplete(
            IN NDIS_STATUS              Status,
            IN NDIS_HANDLE              CallMgrVcContext
            );

    static NDIS_STATUS CmModifyCallQoS(
            IN NDIS_HANDLE              CallMgrVcContext,
            IN PCO_CALL_PARAMETERS       CallParameters
            );

    static NDIS_STATUS CmRequest(
            IN NDIS_HANDLE              ProtocolAfContext,
            IN NDIS_HANDLE              ProtocolVcContext OPTIONAL,
            IN NDIS_HANDLE              ProtocolPartyContext OPTIONAL,
            IN OUT  PNDIS_REQUEST       NdisRequest
            );

    static VOID CmRequestComplete(
            IN NDIS_STATUS              Status,
            IN NDIS_HANDLE              ProtocolAfContext OPTIONAL,
            IN NDIS_HANDLE              ProtocolVcContext OPTIONAL,
            IN NDIS_HANDLE              ProtocolPartyContext OPTIONAL,
            IN OUT  PNDIS_REQUEST       NdisRequest
            );
};

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmCreateVc(
            IN NDIS_HANDLE              ProtocolAfContext,
            IN NDIS_HANDLE              NdisVcHandle,
            OUT PNDIS_HANDLE            ProtocolVcContext
            )
{
    *ProtocolVcContext = NULL;

    Vc *pVc = KNdisCallManagerVcTraits<Binding, Vc>::CreateInstance(ProtocolAfContext, NdisVcHandle);
    if (pVc != NULL)
    {
        *ProtocolVcContext = pVc;
        return NDIS_STATUS_SUCCESS;
    }

    return NDIS_STATUS_FAILURE;
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmDeleteVc(
            IN NDIS_HANDLE              ProtocolVcContext
            )
{
    return KNdisCallManagerVcTraits<Binding, Vc>::DeleteInstance(reinterpret_cast<Vc*>(ProtocolVcContext));
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmOpenAf(
            IN NDIS_HANDLE              CallMgrBindingContext,
            IN PCO_ADDRESS_FAMILY       AddressFamily,
            IN NDIS_HANDLE              NdisAfHandle,
            OUT PNDIS_HANDLE            CallMgrAfContext
            )
{
    NDIS_STATUS retStatus;

    return (reinterpret_cast<Binding*>(CallMgrBindingContext))->CmOpenAf(AddressFamily, NdisAfHandle, CallMgrAfContext);
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmCloseAf(
            IN NDIS_HANDLE              CallMgrAfContext
            )
{
    return Binding::CmCloseAf(CallMgrAfContext);
}


template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmRegisterSap(
            IN NDIS_HANDLE              CallMgrAfContext,
            IN PCO_SAP                  Sap,
            IN NDIS_HANDLE              NdisSapHandle,
            OUT PNDIS_HANDLE            CallMgrSapContext
            )
{
    return Binding::CmRegisterSap(
                    CallMgrAfContext,
                    Sap,
                    NdisSapHandle,
                    CallMgrSapContext
                    );
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmDeregisterSap(
            IN NDIS_HANDLE              CallMgrSapContext
            )
{
    return Binding::CmDeregisterSap(CallMgrSapContext);
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmMakeCall(
            IN NDIS_HANDLE              CallMgrVcContext,
            IN OUT PCO_CALL_PARAMETERS  CallParameters,
            IN NDIS_HANDLE              NdisPartyHandle  OPTIONAL,
            OUT PNDIS_HANDLE            CallMgrPartyContext  OPTIONAL
            )
{
    return (reinterpret_cast<Vc*>(CallMgrVcContext))->CmMakeCall(CallParameters, NdisPartyHandle, CallMgrPartyContext);
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmCloseCall(
            IN NDIS_HANDLE              CallMgrVcContext,
            IN NDIS_HANDLE              CallMgrPartyContext  OPTIONAL,
            IN PVOID                    CloseData  OPTIONAL,
            IN UINT                     Size  OPTIONAL
            )
{
    return (reinterpret_cast<Vc*>(CallMgrVcContext))->CmCloseCall(CallMgrPartyContext, CloseData, Size);
}

template <class Binding, class Vc>
VOID KNdisCallManagerWrapper<Binding, Vc>::CmIncomingCallComplete(
            IN NDIS_STATUS              Status,
            IN NDIS_HANDLE              CallMgrVcContext,
            IN PCO_CALL_PARAMETERS      CallParameters
            )
{
    (reinterpret_cast<Vc*>(CallMgrVcContext))->CmIncomingCallComplete(Status, CallParameters);
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmAddParty(
            IN NDIS_HANDLE              CallMgrVcContext,
            IN OUT PCO_CALL_PARAMETERS  CallParameters,
            IN NDIS_HANDLE              NdisPartyHandle,
            OUT PNDIS_HANDLE            CallMgrPartyContext
            )
{
    return (reinterpret_cast<Vc*>(CallMgrVcContext))->CmAddParty(CallParameters, NdisPartyHandle, CallMgrPartyContext);
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmDropParty(
            IN NDIS_HANDLE              CallMgrPartyContext,
            IN PVOID                    CloseData OPTIONAL,
            IN UINT                     Size OPTIONAL
            )
{
    return Vc::CmDropParty(CallMgrPartyContext, CloseData, Size);
}

template <class Binding, class Vc>
VOID KNdisCallManagerWrapper<Binding, Vc>::CmActivateVcComplete(
            IN NDIS_STATUS              Status,
            IN NDIS_HANDLE              CallMgrVcContext,
            IN PCO_CALL_PARAMETERS      CallParameters
            )
{
    (reinterpret_cast<Vc*>(CallMgrVcContext))->CmActivateVcComplete(Status, CallParameters);
}

template <class Binding, class Vc>
VOID KNdisCallManagerWrapper<Binding, Vc>::CmDeactivateVcComplete(
            IN NDIS_STATUS              Status,
            IN NDIS_HANDLE              CallMgrVcContext
            )
{
    (reinterpret_cast<Vc*>(CallMgrVcContext))->CmDeactivateVcComplete(Status);
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmModifyCallQoS(
            IN NDIS_HANDLE              CallMgrVcContext,
            IN PCO_CALL_PARAMETERS       CallParameters
            )
{
    return (reinterpret_cast<Vc*>(CallMgrVcContext))->CmModifyCallQoS(CallParameters);
}

template <class Binding, class Vc>
NDIS_STATUS KNdisCallManagerWrapper<Binding, Vc>::CmRequest(
            IN NDIS_HANDLE              ProtocolAfContext,
            IN NDIS_HANDLE              ProtocolVcContext OPTIONAL,
            IN NDIS_HANDLE              ProtocolPartyContext OPTIONAL,
            IN OUT  PNDIS_REQUEST       NdisRequest
            )
{
    return Binding::CmRequest(ProtocolAfContext, reinterpret_cast<Vc*>(ProtocolVcContext), ProtocolPartyContext, NdisRequest);
}

template <class Binding, class Vc>
VOID KNdisCallManagerWrapper<Binding, Vc>::CmRequestComplete(
            IN NDIS_STATUS              Status,
            IN NDIS_HANDLE              ProtocolAfContext OPTIONAL,
            IN NDIS_HANDLE              ProtocolVcContext OPTIONAL,
            IN NDIS_HANDLE              ProtocolPartyContext OPTIONAL,
            IN OUT  PNDIS_REQUEST       NdisRequest
            )
{
    Binding::CmRequestComplete(Status, ProtocolAfContext, reinterpret_cast<Vc*>(ProtocolVcContext), ProtocolPartyContext, NdisRequest);
}

#endif //__KNDIS_CALL_MANAGER_WRAPPER_H__
