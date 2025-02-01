/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

   request.c

Abstract:


Author:

	William Ingle

Environment:

	Ndis 3.1 Windows 95 VxD

Notes:
	Copyright (c) 1996, Microsoft Corporation, all rights reserve

	This document is provided for informational purposes only and Microsoft 
	Corporation makes no warranties, either expressed or implied, in this document.
	Information in this document may be substantially changed without notice in
	subsequent versions of windows and does not represent a commitment on the 
	part of Microsoft Corporation. 

--*/

#include <basedef.h>
#include <vmm.h>
#include <ndis.h>
#include <vwin32.h>
#include "debug.h"
#include "packet.h"
#include "ntddpack.h"

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG



DWORD
PacketRequest(	POPEN_INSTANCE		Open,
				DWORD  				FunctionCode,
				DWORD  				dwDDB,
				DWORD				hDevice,
				PDIOCPARAMETERS 	pDiocParms )
/*++

Routine Description:

Arguments:

Return Value:

    Status is returned.

--*/

{
	PLIST_ENTRY       RequestListEntry;
	PINTERNAL_REQUEST	pRequest;
	PPACKET_RESERVED  pReserved;
	PPACKET_OID_DATA	OidData;
	NDIS_STATUS			Status;

	TRACE_ENTER( "Request Packet" );

	//
	// Acquire request element from list
	//
	NdisAcquireSpinLock( &Open->RequestSpinLock );

	RequestListEntry = PacketRemoveHeadList(&Open->RequestList);

	NdisReleaseSpinLock( &Open->RequestSpinLock );

	if ( RequestListEntry == NULL ) 
	{
		IF_TRACE( "Request List Error" );

		// Place the error in the buffer
		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;

		TRACE_LEAVE( "Request Packet" );

		return NDIS_STATUS_SUCCESS;
	}

	pReserved = CONTAINING_RECORD( RequestListEntry, PACKET_RESERVED, ListElement );
	pRequest  = CONTAINING_RECORD( pReserved, INTERNAL_REQUEST, Reserved );
	OidData   = (PPACKET_OID_DATA)(pDiocParms->lpvInBuffer);

	if ( ( pDiocParms->cbInBuffer == pDiocParms->cbOutBuffer )	&&
	( pDiocParms->cbInBuffer >= sizeof(PACKET_OID_DATA) - 1 + OidData->Length) ) 

	{
		// The buffer is valid

		pReserved->lpBuffer				= (PVOID)PacketPageLock( pDiocParms->lpvInBuffer, 
										 					 pDiocParms->cbInBuffer );

		pReserved->lpcbBytesReturned	= (PVOID)PacketPageLock( (PVOID)pDiocParms->lpcbBytesReturned,
														 sizeof(DWORD) );

		pReserved->lpoOverlapped		= (PVOID)PacketPageLock( (PVOID)pDiocParms->lpoOverlapped,
														 sizeof(OVERLAPPED) );

		pReserved->cbBuffer				= pDiocParms->cbInBuffer;
		pReserved->hDevice				= pDiocParms->hDevice;
		pReserved->tagProcess			= pDiocParms->tagProcess;

		if ( FunctionCode == IOCTL_PROTOCOL_SET_OID ) 
		{
			pRequest->Request.RequestType              						= NdisRequestSetInformation;
			pRequest->Request.DATA.SET_INFORMATION.Oid 						= OidData->Oid;
			pRequest->Request.DATA.SET_INFORMATION.InformationBufferLength	= OidData->Length;
			pRequest->Request.DATA.SET_INFORMATION.InformationBuffer 	  	= OidData->Data;

#if DEBUG
			IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )
			{
				IF_TRACE_MSG2( "Request Set: Oid=%08lx, Length=%08lx",
								OidData->Oid,
								OidData->Length );
			}
#endif
		} 
		else if ( FunctionCode == IOCTL_PROTOCOL_QUERY_OID )
		{
			pRequest->Request.RequestType									= NdisRequestQueryInformation;
			pRequest->Request.DATA.QUERY_INFORMATION.Oid					= OidData->Oid;
			pRequest->Request.DATA.QUERY_INFORMATION.InformationBufferLength= OidData->Length;
			pRequest->Request.DATA.QUERY_INFORMATION.InformationBuffer     	= OidData->Data;

			IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )		
			{
				IF_TRACE_MSG3( "Request Query: Type:%d Oid=%08lx, Length=%08lx",
								NdisRequestQueryInformation,
								OidData->Oid,
								OidData->Length );
			}
		}
		else
		{
			pRequest->Request.RequestType									= NdisRequestGeneric1;
			pRequest->Request.DATA.QUERY_INFORMATION.Oid					= OidData->Oid;
			pRequest->Request.DATA.QUERY_INFORMATION.InformationBufferLength= OidData->Length;
			pRequest->Request.DATA.QUERY_INFORMATION.InformationBuffer     	= OidData->Data;

			IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )		
			{
				IF_TRACE_MSG3( "Request Statistic: Type:%d Oid=%08lx, Length=%08lx",
								NdisRequestGeneric1,
								OidData->Oid,
								OidData->Length );
			}
		}

		//
		// submit the request
		//
		NdisRequest( &Status, Open->AdapterHandle, &pRequest->Request );

		if ( Status != NDIS_STATUS_PENDING )
		{
			PacketRequestComplete( Open, &pRequest->Request, Status );

			TRACE_LEAVE( "Request Packet" );

			return NDIS_STATUS_SUCCESS;
		}

		TRACE_LEAVE( "Request Packet" );

		return(-1);		// This will make DeviceIOControl return ERROR_IO_PENDING
	}
	else
	{
		IF_TRACE_MSG4( "Request Buffer Error: In=%lx Out=%lx Size=%lx Length=%lx",
						pDiocParms->cbInBuffer,
						pDiocParms->cbOutBuffer,
						sizeof( PACKET_OID_DATA ),
						OidData->Length
						);

		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;
	}

	TRACE_LEAVE( "Request Packet" );

	return NDIS_STATUS_SUCCESS;
}


VOID NDIS_API
PacketRequestComplete( IN NDIS_HANDLE		ProtocolBindingContext,
							  IN PNDIS_REQUEST	NdisRequest,
							  IN NDIS_STATUS		Status )
/*++

Routine Description:

Arguments:

Return Value:

    Status is returned.

--*/

{
	POPEN_INSTANCE		Open;
	PINTERNAL_REQUEST	pRequest;
	PPACKET_RESERVED	pReserved;
	OVERLAPPED*			pOverlap;
	PPACKET_OID_DATA	oidData;

	TRACE_ENTER( "RequestComplete" );

	Open		= (POPEN_INSTANCE)ProtocolBindingContext;
	pRequest	= CONTAINING_RECORD( NdisRequest, INTERNAL_REQUEST, Request );
	pReserved 	= &pRequest->Reserved;
	pOverlap	= (OVERLAPPED *) pReserved->lpoOverlapped;
	oidData		= (PPACKET_OID_DATA)(pReserved->lpBuffer);


	#if DEBUG
		IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD )		
		{
			ULONG		i;
			UCHAR*	pcData = oidData->Data;

			DbgPrint( "Packet: OID=%lx Status=%lx Buffer Length=%ld Buffer=%lx", 
			oidData->Oid, Status, oidData->Length, pcData );

			for ( i=0; i<oidData->Length; i++, pcData++ )
			{
				if ( i%16 == 0 )
					DbgPrint( "\r\nPacket: " );

				DbgPrint( "%02x ", *pcData );
			}

			DbgPrint( "\r\n" );
			IF_BREAK_SET;
		}
	#endif


	if ( Status == NDIS_STATUS_SUCCESS )
	{
		// set total bytes returned

		*(pReserved->lpcbBytesReturned)	= oidData->Length + sizeof(PACKET_OID_DATA) - 1;
		pOverlap->O_InternalHigh		= *(pReserved->lpcbBytesReturned);
	}
	else
	{
		// set total bytes returned

		*(pReserved->lpcbBytesReturned)	= 0;
		pOverlap->O_InternalHigh		= 0;

		// return status in oidData if there is an error 

		oidData->Length = Status;
	}

	// The internal member of overlapped structure contains
	// a pointer to the event structure that will be signalled,
	// resuming the execution of the waitng GetOverlappedResult
	// call.

	VWIN32_DIOCCompletionRoutine( pOverlap->O_Internal );

	// Unlock buffers	

	PacketPageUnlock( pReserved->lpBuffer, pReserved->cbBuffer );
	PacketPageUnlock( pReserved->lpcbBytesReturned, sizeof(DWORD) );
	PacketPageUnlock( pReserved->lpoOverlapped, sizeof(OVERLAPPED) );

	// Return request element to list

	NdisAcquireSpinLock( &Open->RequestSpinLock );

	InsertTailList( &Open->RequestList, &pReserved->ListElement );

	NdisReleaseSpinLock( &Open->RequestSpinLock );

	TRACE_LEAVE( "RequestComplete" );

	return;
}
