//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// MACFUNCS.C - stubs for NDIS MAC functions for NDISMAC example
// Copyright (c) 1994, Compuware Corporation

#include "ndismac.h"

NDIS_STATUS NDIS_API NetCardOpenAdapter(
	OUT PNDIS_STATUS OpenErrorStatus,
	OUT NDIS_HANDLE *MacBindingHandle,
	OUT PUINT SelectedMediumIndex,
	IN PNDIS_MEDIUM MediumArray,
	IN UINT MediumArraySize,
	IN NDIS_HANDLE NdisBindingContext,
	IN NDIS_HANDLE MacAdapterContext,
	IN UINT OpenOptions,
	IN PSTRING AddressingInformation OPTIONAL)
{	
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS NDIS_API NetCardCloseAdapter(IN NDIS_HANDLE MacBindingHandle)
{	
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS NDIS_API NetCardRequest(
	IN NDIS_HANDLE MacBindingHandle,
	IN PNDIS_REQUEST NdisRequest)
{	
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS NDIS_API NetCardSend(
	IN NDIS_HANDLE MacBindingHandle,
	IN PNDIS_PACKET Packet)
{	
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS NDIS_API NetCardTransferData(
	IN NDIS_HANDLE MacBindingHandle,
	IN NDIS_HANDLE MacReceiveContext,
	IN UINT ByteOffset,
	IN UINT BytesToTransfer,
	OUT PNDIS_PACKET Packet,
	OUT PUINT BytesTransferred)
{	
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS NDIS_API NetCardReset(IN NDIS_HANDLE MacBindingHandle)
{	
	return NDIS_STATUS_SUCCESS;
}

VOID NDIS_API NetCardUnload(IN NDIS_HANDLE MacMacContext)
{	
}

NDIS_STATUS NDIS_API NetCardQueryGlobalStatistics(
	IN NDIS_HANDLE MacAdapterContext,
	IN PNDIS_REQUEST NdisRequest)
{	
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS NDIS_API NetCardAddAdapter(
	IN NDIS_HANDLE MacMacContext,
	IN NDIS_HANDLE ConfigurationHandle,
	IN PNDIS_STRING AdapterName)
{	
	return NDIS_STATUS_SUCCESS;
}

VOID NDIS_API NetCardRemoveAdapter(IN PVOID MacAdapterContext)
{	
}
