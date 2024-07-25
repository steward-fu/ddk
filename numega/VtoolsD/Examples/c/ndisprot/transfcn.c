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

#include "trans31.h"

// Copyright And Configuration Management ----------------------------------
//
// Stubs For NDIS 3.1 PROTOCOL Functions For TRANS31 Example - TransFcn.c
//
// Copyright (c) 1996, Compuware Corporation
// Copyright (c) 1995-1996, Printing Communications Associates, Inc.
//
//
//                             Thomas F. Divine
//                           4201 Brunswick Court
//                        Smyrna, Georgia 30080 USA
//                              (770) 436-1714
//
//

// TRANSFCN.C - stubs for NDIS PROTOCOL functions for TRANS31 example



/////////////////////////////////////////////////////////////////////////////
//// GLOBAL DATA

PROTOCOL_BLOCK TRANS31Context={0};


/////////////////////////////////////////////////////////////////////////////
//// NDIS 3.1 PROTOCOL FUNCTION STUBS

VOID NDIS_API
TRANS31OpenAdapterCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status,
    IN NDIS_STATUS OpenErrorStatus
    )
{
	DEBUGTRACE( "OpenAdapterCompleteHandler Entry..." );
}

VOID NDIS_API
TRANS31CloseAdapterCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    )
{
	DEBUGTRACE( "CloseAdapterCompleteHandler Entry..." );
}

VOID NDIS_API
TRANS31SendCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status
    )
{
	DEBUGTRACE( "SendCompleteHandler Entry..." );
}

VOID NDIS_API
TRANS31TransferDataCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status,
    IN UINT BytesTransferred
    )
{
	DEBUGTRACE( "TransferDataCompleteHandler Entry..." );
}

VOID NDIS_API
TRANS31ResetCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    )
{
	DEBUGTRACE( "ResetCompleteHandler Entry..." );
}

VOID NDIS_API
TRANS31RequestCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_STATUS Status
    )
{
	DEBUGTRACE( "RequestCompleteHandler Entry..." );
}

NDIS_STATUS NDIS_API
TRANS31ReceiveHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_HANDLE MacReceiveContext,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookAheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    )
{
	DEBUGTRACE( "ReceiveHandler Entry..." );

	return( NDIS_STATUS_SUCCESS );
}

VOID NDIS_API
TRANS31ReceiveCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext
    )
{
	DEBUGTRACE( "ReceiveCompleteHandler Entry..." );
}

VOID NDIS_API
TRANS31StatusHandler(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS GeneralStatus,
    IN PVOID StatusBuffer,
    IN UINT StatusBufferSize
    )
{
	DEBUGTRACE( "StatusHandler Entry..." );
}

VOID NDIS_API
TRANS31StatusCompleteHandler(
    IN NDIS_HANDLE NdisBindingContext
    )
{
	DEBUGTRACE( "StatusCompleteHandler Entry..." );
}


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
    )
{
	NDIS_STATUS OpenErrorStatus = NDIS_STATUS_FAILURE;
	UINT        Medium;
	NDIS_MEDIUM MediumArray=NdisMedium802_3;

	DEBUGTRACE( "BindAdapterHandler Entry..." );

	*pStatus = NDIS_STATUS_FAILURE;		// Assume Failure

	//
	//  Try to open the MAC
	//

	DEBUGTRACE( "Calling NdisOpenAdapter..." );

	NdisOpenAdapter(
		pStatus,
		&OpenErrorStatus,
		&TRANS31Context.NdisAdapterHandle,
		&Medium,
		&MediumArray,
		1,
		TRANS31Context.NdisProtocolHandle,
		(NDIS_HANDLE )&TRANS31Context,		// Specify ProtocolBindingContext
		AdapterName,
		0,
		NULL
		);

	if( *pStatus != NDIS_STATUS_PENDING )
	{
		TRANS31OpenAdapterCompleteHandler(
			(NDIS_HANDLE )&TRANS31Context,	// ProtocolBindingContext
			*pStatus,
			NDIS_STATUS_SUCCESS
			);
	}

	DEBUGTRACE( "NdisOpenAdapter succeeded\n" );
}


/* Note
-------
 * It appears that UnloadProtocolHandler() is not called if this function
 * returns a value other than NDIS_STATUS_SUCCESS.
 */
VOID NDIS_API
TRANS31UnbindAdapterHandler(
    OUT PNDIS_STATUS    pStatus,
    IN  NDIS_HANDLE     ProtocolBindingContext,
    IN  NDIS_HANDLE     UnbindAdapterContext
    )
{
	DEBUGTRACE( "UnbindAdapterHandler Entry..." );

	NdisCloseAdapter(
		pStatus,
		TRANS31Context.NdisAdapterHandle		// Handle From NdisOpenAdapter
		);

	if( *pStatus != NDIS_STATUS_PENDING )
	{
		TRANS31CloseAdapterCompleteHandler(
			ProtocolBindingContext,
			*pStatus
			);
	}
}

VOID NDIS_API
TRANS31UnloadProtocolHandler( void )
{
	NDIS_STATUS Status = NDIS_STATUS_FAILURE;

	NdisDeregisterProtocol(
		&Status,
		TRANS31Context.NdisProtocolHandle	// Handle From NdisRegisterProtocol
		);
}

