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


PDEVICE_EXTENSION GlobalDeviceExtension = 0;



ULONG
strlen( BYTE *s )
{
        ULONG len = 0;

        while ( *s++ ) len++;

        return len;
}



NTSTATUS
DriverEntry( IN PDRIVER_OBJECT	DriverObject,
				 IN PUNICODE_STRING	RegistryPath
	)

/*++

Routine Description:

   This routine initializes the Packet driver.

Arguments:

   DriverObject - Pointer to driver object created by system.

   RegistryPath - Pointer to the Unicode name of the registry path for this driver.

Return Value:

   The function value is the final status from the initialization operation.

--*/

{
	NDIS_PROTOCOL_CHARACTERISTICS	ProtocolChar;

	//
	// Name MUST match string used by ndisdev.asm to declare the driver name; case counts.
	// This is necessary for binding to proceed properly.
	//
	NDIS_STRING						ProtoName = NDIS_STRING_CONST("VPACKET");
	NDIS_HANDLE 					NdisProtocolHandle;
	NDIS_STATUS						Status;

	INIT_ENTER( "DriverEntry" );

	NdisAllocateMemory( (PVOID *)&GlobalDeviceExtension, sizeof( DEVICE_EXTENSION ), 0, -1 );

	if ( GlobalDeviceExtension != NULL )
	{
		NdisZeroMemory( (UCHAR*)GlobalDeviceExtension, sizeof(DEVICE_EXTENSION) );
		NdisZeroMemory( (UCHAR*)&ProtocolChar, sizeof(NDIS_PROTOCOL_CHARACTERISTICS) );

		ProtocolChar.MajorNdisVersion            = 0x03;
		ProtocolChar.MinorNdisVersion            = 0x0A;
		ProtocolChar.Reserved                    = 0;
		ProtocolChar.OpenAdapterCompleteHandler  = PacketBindAdapterComplete;
		ProtocolChar.CloseAdapterCompleteHandler = PacketUnbindAdapterComplete;
		ProtocolChar.SendCompleteHandler         = PacketSendComplete;
		ProtocolChar.TransferDataCompleteHandler = PacketTransferDataComplete;
		ProtocolChar.ResetCompleteHandler        = PacketResetComplete;
		ProtocolChar.RequestCompleteHandler      = PacketRequestComplete;
		ProtocolChar.ReceiveHandler              = PacketReceiveIndicate;
		ProtocolChar.ReceiveCompleteHandler      = PacketReceiveComplete;
		ProtocolChar.StatusHandler               = PacketStatus;
		ProtocolChar.StatusCompleteHandler       = PacketStatusComplete;
		ProtocolChar.BindAdapterHandler			 = PacketBindAdapter;
		ProtocolChar.UnbindAdapterHandler        = PacketUnbindAdapter;
		ProtocolChar.UnloadProtocolHandler       = PacketUnload;
		ProtocolChar.Name                        = ProtoName;

		NdisRegisterProtocol( &Status,
							  &GlobalDeviceExtension->NdisProtocolHandle,
							  &ProtocolChar,
							  sizeof(NDIS_PROTOCOL_CHARACTERISTICS) );

		if (Status != NDIS_STATUS_SUCCESS) 
   		{
			NdisFreeMemory( GlobalDeviceExtension, sizeof( DEVICE_EXTENSION ) ,  0 );
										  
			IF_TRACE( "Failed to register protocol with NDIS" );
			INIT_LEAVE( "DriverEntry" );

			return Status;
		}

		// initialize open list
		InitializeListHead( &GlobalDeviceExtension->OpenList );
			
		// initialize global device extension
		GlobalDeviceExtension->DriverObject = DriverObject;
  	
  		IF_TRACE( "protocol registered with NDIS!!! :-)" );
		INIT_LEAVE( "DriverEntry" );

		return Status;
	}

	IF_TRACE( "Memory Failure" );
	INIT_LEAVE( "DriverEntry" );

	return NDIS_STATUS_RESOURCES;
}

		

VOID NDIS_API
/*++

Routine Description:

Arguments:

Return Value:

	none

--*/

PacketUnload()
{
	NDIS_STATUS		Status;

	TRACE_ENTER( "Unload" );

	if ( GlobalDeviceExtension )
	{
	   NdisDeregisterProtocol( &Status, GlobalDeviceExtension->NdisProtocolHandle );

		if ( Status == NDIS_STATUS_SUCCESS )
		{
			NdisFreeMemory( GlobalDeviceExtension, sizeof( DEVICE_EXTENSION ) ,  0 );
			GlobalDeviceExtension = 0;
		}
	}

	TRACE_LEAVE( "Unload" );

	return;
}



POPEN_INSTANCE GetFirstAdapter( VOID )
/*++

Routine Description:

Arguments:

Return Value:

--*/

{
	PLIST_ENTRY		pEntry = GlobalDeviceExtension->OpenList.Flink;

	POPEN_INSTANCE pOpen  = CONTAINING_RECORD( pEntry, OPEN_INSTANCE, ListElement );

	return pOpen;
}



DWORD
PacketGetMacNameList( DWORD  				dwDDB,
                      DWORD  				hDevice,
                      PDIOCPARAMETERS	pDiocParms ) 
{
	PLIST_ENTRY	pHead = &(GlobalDeviceExtension->OpenList);
	PLIST_ENTRY pEntry;
	DWORD			dwBytes = 0;

   TRACE_ENTER( "GetMacNameList" );

	for ( pEntry=pHead->Flink; pEntry != pHead; pEntry=pEntry->Flink )
	{
		BYTE 	*lpzName;
		ULONG	uLength;
		POPEN_INSTANCE pOpen = CONTAINING_RECORD( pEntry, OPEN_INSTANCE, ListElement );

		PWRAPPER_MAC_BLOCK			pWMBlock;
		PNDIS_MAC_CHARACTERISTICS	pNMChar;

		pWMBlock = ((PWRAPPER_OPEN_BLOCK)(pOpen->AdapterHandle))->MacHandle;
		pNMChar  = &pWMBlock->MacCharacteristics;
		lpzName  = pNMChar->Name.Buffer;
		uLength  = strlen( lpzName );
	
		IF_TRACE_MSG2( "     %s  %lu",  lpzName, uLength );
	
		if ( uLength < pDiocParms->cbOutBuffer - dwBytes - 1 )
		{
			strcat( (BYTE*)(pDiocParms->lpvOutBuffer), lpzName );
			strcat( (BYTE*)(pDiocParms->lpvOutBuffer), " " );
			dwBytes += (uLength + 1);
		}
		else
			break;
	}

	*(ULONG*)(pDiocParms->lpcbBytesReturned) = dwBytes;

	IF_TRACE_MSG( "     Bytes Returned: %lu", *(ULONG*)(pDiocParms->lpcbBytesReturned) );

	TRACE_LEAVE( "GetMacNameList" );

	return NDIS_STATUS_SUCCESS;
}


DWORD _stdcall PacketIOControl( DWORD  			dwService,
                                DWORD  			dwDDB,
                                DWORD  			hDevice,
                                PDIOCPARAMETERS pDiocParms ) 
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
   POPEN_INSTANCE    Open;
   NDIS_STATUS     	Status;

   TRACE_ENTER( "DeviceIoControl" );

//BUGBUG
//For now we utilize only the FIRST adapter bound to the protocol
   Open = GetFirstAdapter();

   IF_TRACE_MSG3( "    Function code is %08lx  buff size=%08lx  %08lx",
						dwService,
						pDiocParms->cbInBuffer,
						pDiocParms->cbOutBuffer );

	switch ( dwService )
	{
	case DIOC_OPEN:
		//
		// Just return success,  This is required for Win95
		//
		break;

	case DIOC_CLOSEHANDLE:
		// Make sure there are no pending i/o
		Status = NDIS_STATUS_SUCCESS;
		PacketCleanUp( &Status, GetFirstAdapter() );
		break;

	case IOCTL_PROTOCOL_RESET:
		PacketReset( &Status, Open );
		break;

	case IOCTL_PROTOCOL_SET_OID:
	case IOCTL_PROTOCOL_QUERY_OID:
	case IOCTL_PROTOCOL_STATISTICS:
		return PacketRequest( Open, dwService, dwDDB, hDevice, pDiocParms );

	case IOCTL_PROTOCOL_READ:
		return PacketRead( Open, dwDDB, hDevice, pDiocParms );

	case IOCTL_PROTOCOL_WRITE:
		return PacketWrite( Open, dwDDB, hDevice, pDiocParms );

	case IOCTL_PROTOCOL_MACNAME:
		PacketGetMacNameList( dwDDB, hDevice, pDiocParms );
		break;
      
	default: 
		// Error, Unrecognized IOCTL
		*(DWORD *)(pDiocParms->lpcbBytesReturned) = 0;
		break;
	}

   TRACE_LEAVE( "DeviceIoControl" );
   
   return NDIS_STATUS_SUCCESS;
}



VOID
PacketStatus(
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN NDIS_STATUS   Status,
    IN PVOID         StatusBuffer,
    IN UINT          StatusBufferSize
    )
/*++

Routine Description:

Arguments:

Return Value:

    Status is returned.

--*/

{
   TRACE_ENTER( "Status Indication" );
   TRACE_LEAVE( "Status Indication" );

   return;
}



VOID NDIS_API
PacketStatusComplete(
    IN NDIS_HANDLE  ProtocolBindingContext
    )
/*++

Routine Description:

Arguments:

Return Value:

    Status is returned.

--*/

{
   TRACE_ENTER( "StatusIndicationComplete" );
   TRACE_LEAVE( "StatusIndicationComplete" );

   return;
}



PLIST_ENTRY
PacketRemoveHeadList(
    IN PLIST_ENTRY pListHead
    )
/*++

Routine Description:

Arguments:

Return Value:

    First PLIST_ENTRY from list is returned
	 Returns NULL if list is empty

--*/

{
	if ( !IsListEmpty( pListHead ) )
	{
		PLIST_ENTRY pLE = RemoveHeadList( pListHead );

		return pLE;
	}

	return NULL;
}
