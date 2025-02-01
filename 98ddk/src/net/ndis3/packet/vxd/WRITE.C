/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

   packet.c

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

DWORD _stdcall MyPageLock(DWORD, DWORD);
void  _stdcall MyPageUnlock(DWORD, DWORD);



DWORD
PacketWrite(POPEN_INSTANCE	Open,
			DWORD  			dwDDB,
			DWORD  			hDevice,
			PDIOCPARAMETERS	pDiocParms
	)


/*++

Routine Description:

    This is the dispatch routine for create/open and close requests.
    These requests complete successfully.

Arguments:

    DeviceObject - Pointer to the device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
	PNDIS_PACKET	pPacket;
	PNDIS_BUFFER 	pNdisBuffer;
	NDIS_STATUS		Status;


	TRACE_ENTER( "SendPacket" );

	PacketAllocatePacketBuffer( &Status, Open, &pPacket, pDiocParms, IOCTL_PROTOCOL_WRITE );


	if ( Status != NDIS_STATUS_SUCCESS )
	{
		return 0;	// This will return immediately with no data written
	}

	//  Call the MAC

	NdisSend( &Status, Open->AdapterHandle, pPacket );

	if ( Status != NDIS_STATUS_PENDING ) 
	{
		//  The send didn't pend so call the completion handler now

		PacketSendComplete( Open, pPacket, Status );
	}

	TRACE_LEAVE( "SendPacket" );

	return(-1);		// This will make DeviceIOControl return ERROR_IO_PENDING
}



VOID NDIS_API
PacketSendComplete(	IN NDIS_HANDLE	ProtocolBindingContext,
					IN PNDIS_PACKET	pPacket,
					IN NDIS_STATUS	Status
   )
/*++

Routine Description:

Arguments:

Return Value:

    Status is returned.

--*/

{
	PNDIS_BUFFER 		pNdisBuffer;
	PPACKET_RESERVED	Reserved = (PPACKET_RESERVED) pPacket->ProtocolReserved;

	TRACE_ENTER( "SendComplete" );

	// free buffer descriptor
	
	NdisUnchainBufferAtFront( pPacket, &pNdisBuffer );

	if ( pNdisBuffer )

		NdisFreeBuffer( pNdisBuffer );

	// The internal member of overlapped structure contains
	// a pointer to the event structure that will be signalled,
	// resuming the execution of the waiting GetOverlappedResult
	// call.
	//
	
	VWIN32_DIOCCompletionRoutine( Reserved->lpoOverlapped->O_Internal );

	// Unlock buffers	
	
	PacketPageUnlock( Reserved->lpBuffer, Reserved->cbBuffer );
	PacketPageUnlock( Reserved->lpcbBytesReturned, sizeof(DWORD) );
	PacketPageUnlock( Reserved->lpoOverlapped, sizeof(OVERLAPPED) );

	// recycle the packet
	
	NdisReinitializePacket(pPacket);

	// Put the packet back in the free list
	
	NdisFreePacket(pPacket);

	TRACE_LEAVE( "SendComplete" );

	return;
}
