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

// NDISMAC.H - include file for VxD NDISMAC
// Copyright  1994-1996 (C) Compuware Corporation 

#define  USE_NDIS
#include <vtoolsc.h>

#define NDISMAC_Major		1
#define NDISMAC_Minor		0
#define NDISMAC_DeviceID	UNDEFINED_DEVICE_ID
#define NDISMAC_Init_Order	MAC_Init_Order

#define NETCARD_NDIS_MAJOR_VERSION	3
#define NETCARD_NDIS_MINOR_VERSION	0

#define CARDNAME "NDISMAC"			// THIS STRING MUST MATCH THE DEVICE NAME

// Handle structure

typedef struct _NetCard_MAC {
    NDIS_HANDLE NdisWrapperHandle; // Returned by NdisInitializeWrapper.
    NDIS_HANDLE NdisMacHandle;	   // Returned by NdisRegisterMac.
} NETCARD_MAC, *PNETCARD_MAC;

// Prototypes for various handlers in MACFUNCS.C

NDIS_STATUS NDIS_API NetCardOpenAdapter(
	OUT PNDIS_STATUS OpenErrorStatus,
	OUT NDIS_HANDLE *MacBindingHandle,
	OUT PUINT SelectedMediumIndex,
	IN PNDIS_MEDIUM MediumArray,
	IN UINT MediumArraySize,
	IN NDIS_HANDLE NdisBindingContext,
	IN NDIS_HANDLE MacAdapterContext,
	IN UINT OpenOptions,
	IN PSTRING AddressingInformation OPTIONAL);


NDIS_STATUS NDIS_API NetCardCloseAdapter(IN NDIS_HANDLE MacBindingHandle);

NDIS_STATUS NDIS_API NetCardRequest(
	IN NDIS_HANDLE MacBindingHandle,
	IN PNDIS_REQUEST NdisRequest);

NDIS_STATUS NDIS_API NetCardSend(
	IN NDIS_HANDLE MacBindingHandle,
	IN PNDIS_PACKET Packet);

NDIS_STATUS NDIS_API NetCardTransferData(
	IN NDIS_HANDLE MacBindingHandle,
	IN NDIS_HANDLE MacReceiveContext,
	IN UINT ByteOffset,
	IN UINT BytesToTransfer,
	OUT PNDIS_PACKET Packet,
	OUT PUINT BytesTransferred);

NDIS_STATUS NDIS_API NetCardReset(IN NDIS_HANDLE MacBindingHandle);

VOID NDIS_API NetCardUnload(IN NDIS_HANDLE MacMacContext);

NDIS_STATUS NDIS_API NetCardQueryGlobalStatistics(
	IN NDIS_HANDLE MacAdapterContext,
	IN PNDIS_REQUEST NdisRequest);

NDIS_STATUS NDIS_API NetCardAddAdapter(
	IN NDIS_HANDLE MacMacContext,
	IN NDIS_HANDLE ConfigurationHandle,
	IN PNDIS_STRING AdapterName);

VOID NDIS_API NetCardRemoveAdapter(IN PVOID MacAdapterContext);


