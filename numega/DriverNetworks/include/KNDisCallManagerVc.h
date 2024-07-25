// KNdisCallManagerVc.h: interface for the KNdisCallManagerVc class.
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

#ifndef __KNDIS_CALL_MANAGER_VC_H__
#define __KNDIS_CALL_MANAGER_VC_H__

#include "KNdis.h"
#include "KObject.h"


//////////////////////////////////////////////////////////////////////
// KNdisCallManagerVc
//
// The base class of all call managers virtual connections. The class
// provides do-nothing implementations of functions required from
// call manager virtual connection objects.
class KNdisCallManagerVc
{
public:
    NDIS_STATUS CmMakeCall(
            IN OUT PCO_CALL_PARAMETERS      CallParameters,
            IN NDIS_HANDLE                  NdisPartyHandle  OPTIONAL,
            OUT PNDIS_HANDLE                CallMgrPartyContext  OPTIONAL
            )
	{
		UNREFERENCED_PARAMETERS3(CallParameters, NdisPartyHandle, CallMgrPartyContext);
		return  NDIS_STATUS_NOT_SUPPORTED;
	}

    NDIS_STATUS CmCloseCall(
            IN NDIS_HANDLE                  CallMgrPartyContext  OPTIONAL,
            IN PVOID                        CloseData  OPTIONAL,
            IN UINT                         Size  OPTIONAL
            )
	{
		UNREFERENCED_PARAMETERS3(CallMgrPartyContext, CloseData, Size);
		return  NDIS_STATUS_NOT_SUPPORTED;
	}

    VOID CmIncomingCallComplete(
            IN NDIS_STATUS                  Status,
            IN PCO_CALL_PARAMETERS          CallParameters
            )
	{
		UNREFERENCED_PARAMETERS2(Status, CallParameters);
	}

    NDIS_STATUS CmAddParty(
            IN OUT PCO_CALL_PARAMETERS      CallParameters,
            IN NDIS_HANDLE                  NdisPartyHandle,
            OUT PNDIS_HANDLE                CallMgrPartyContext
            )
	{
		UNREFERENCED_PARAMETERS3(CallParameters, NdisPartyHandle, CallMgrPartyContext);
		return  NDIS_STATUS_NOT_SUPPORTED;
	}

    static NDIS_STATUS CmDropParty(
			IN NDIS_HANDLE					CallMgrPartyContext,
            IN PVOID                        CloseData  OPTIONAL,
            IN UINT                         Size  OPTIONAL
            )
	{
		UNREFERENCED_PARAMETERS3(CallMgrPartyContext, CloseData, Size);
		return  NDIS_STATUS_NOT_SUPPORTED;
	}

    VOID CmActivateVcComplete(
            IN NDIS_STATUS                  Status,
            IN PCO_CALL_PARAMETERS          CallParameters
            )
	{
		UNREFERENCED_PARAMETERS2(Status, CallParameters);
	}

    VOID CmDeactivateVcComplete(
            IN NDIS_STATUS                  Status
            )
	{
		UNREFERENCED_PARAMETER(Status);
	}

    NDIS_STATUS CmRequest(
            IN NDIS_HANDLE                  ProtocolPartyContext  OPTIONAL,
            IN OUT  PNDIS_REQUEST           NdisRequest
            )
	{
		UNREFERENCED_PARAMETERS2(ProtocolPartyContext, NdisRequest);
		return  NDIS_STATUS_NOT_SUPPORTED;
	}

    VOID CmRequestComplete(
            IN NDIS_STATUS                  Status,
            IN NDIS_HANDLE                  ProtocolPartyContext  OPTIONAL,
            IN OUT  PNDIS_REQUEST           NdisRequest
            )
	{
		UNREFERENCED_PARAMETERS3(Status, ProtocolPartyContext, NdisRequest);
	}

	NDIS_STATUS CmModifyCallQoS(
            IN PCO_CALL_PARAMETERS			CallParameters
            )
	{
		UNREFERENCED_PARAMETER(CallParameters);
		return  NDIS_STATUS_NOT_SUPPORTED;
	}
};

#endif // __KNDIS_CALL_MANAGER_VC_H__
