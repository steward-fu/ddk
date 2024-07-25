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

/////////////////////////////////////////////////////////////////////////////
//// INCLUDE FILES

#ifndef __TRANS31_H__
#define __TRANS31_H__

#define  USE_NDIS
#include <vtoolsc.h>

// Copyright And Configuration Management ----------------------------------
//
//   Header For NDIS 3.1 Network Protocol (Transport) Example - Trans31.h
//
//   Copyright (c) 1996, Compuware Corporation
//   Copyright (c) 1995-1996, Printing Communications Associates, Inc.
//
//                             Thomas F. Divine
//                           4201 Brunswick Court
//                        Smyrna, Georgia 30080 USA
//                              (770) 436-1714
//


/////////////////////////////////////////////////////////////////////////////
//// VXD DEVICE INFORMATION

#define TRANS31_DEVICE_ID	NUMEGA_TEST_ID

#ifndef NDIS_Init_Order
#define NDIS_Init_Order         0x02000 + VNETBIOS_INIT_ORDER
#endif
#define MAC_Init_Order          0x02000 + NDIS_Init_Order
#define PROTOCOL_Init_Order     0x02000 + MAC_Init_Order

#define TRANS31_Major		1
#define TRANS31_Minor		0
#define TRANS31_DeviceID	TRANS31_DEVICE_ID
#define TRANS31_Init_Order	PROTOCOL_Init_Order


#define TRANS31_NDIS_MAJOR_VERSION	0x03
#define TRANS31_NDIS_MINOR_VERSION	0x0A

#define TRANSPORT_NAME "TRANS31"

// Handle structure

typedef struct _TRANS31_PROTOCOL {
    NDIS_HANDLE NdisWrapperHandle; // Returned by NdisInitializeWrapper.
    NDIS_HANDLE NdisMacHandle;	   // Returned by NdisRegisterMac.
} TRANS31_PROTOCOL, *PTRANS31_PROTOCOL;

/////////////////////////////////////////////////////////////////////////////
//// NDIS 3.1 PROTOCOL FUNCTION PROTOTYPES

// These functions are implemented in TRANSFCN.C.

VOID NDIS_API
TRANS31OpenAdapterCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status,
    IN NDIS_STATUS OpenErrorStatus
    );

VOID NDIS_API
TRANS31CloseAdapterCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    );

VOID NDIS_API
TRANS31SendCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status
    );

VOID NDIS_API
TRANS31TransferDataCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status,
    IN UINT BytesTransferred
    );

VOID NDIS_API
TRANS31ResetCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    );

VOID NDIS_API
TRANS31RequestCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_STATUS Status
    );

NDIS_STATUS NDIS_API
TRANS31ReceiveHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_HANDLE MacReceiveContext,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookAheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    );

VOID NDIS_API
TRANS31ReceiveCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext
    );

VOID NDIS_API
TRANS31StatusHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS GeneralStatus,
    IN PVOID StatusBuffer,
    IN UINT StatusBufferSize
    );

VOID NDIS_API
TRANS31StatusCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext
    );


//
// BIND_ADAPTER_HANDLER, UNBIND_ADAPTER_HANDLER, UNLOAD_PROTOCOL
// are valid for NDIS 3.1 protocols
//

VOID NDIS_API
TRANS31BindAdapterHandler(
    OUT PNDIS_STATUS    pStatus,
    IN  NDIS_HANDLE     BindAdapterContext,
    IN  PNDIS_STRING    AdapterName,
    IN  PVOID           SystemSpecific1,
    IN  PVOID           SystemSpecific2
    );


VOID NDIS_API
TRANS31UnbindAdapterHandler(
    OUT PNDIS_STATUS    pStatus,
    IN  NDIS_HANDLE     ProtocolBindingContext,
    IN  NDIS_HANDLE     UnbindAdapterContext
    );

VOID NDIS_API TRANS31UnloadProtocolHandler( void );


/////////////////////////////////////////////////////////////////////////////
//// PROTOCOL CONTEXT

// Only have one of these structures for the Protocol VxD

typedef
struct _PROTOCOL_BLOCK
{
	/* NDIS Binding Context
	----------------------- */
	NDIS_HANDLE NdisAdapterHandle;      // returned from NdisOpenAdapter
	NDIS_HANDLE NdisProtocolHandle;     // returned from NdisRegisterProtocol

	NDIS_PROTOCOL_CHARACTERISTICS ProtocolCharacteristics;
}
	PROTOCOL_BLOCK, *PPROTOCOL_BLOCK;

extern PROTOCOL_BLOCK TRANS31Context;

#endif // __TRANS31_H__
