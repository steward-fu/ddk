/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    packet32.c

Abstract:


Author:

    BrianL,William Ingle

Environment:

    User mode only.

Notes:


Future:


Revision History:
Edward Reusser		PacketOpenCloseAdapter needs to leave the adapter handle alone
					to run cleanly across calls to PacketCloseAdapter and PacketOpenAdapter 
					in the same process.

--*/

#ifndef ULONG_PTR
typedef unsigned long *ULONG_PTR;
#endif

#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>
#include <stdlib.h>

#include <ntddndis.h>
#include <ntddpack.h>

#include <packet32.h>

// Assignments from long to int are anticipated here
#pragma warning (disable : 4244)

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif /* __cplusplus */


typedef int NDIS_STATUS, *PNDIS_STATUS; // note default size

#define STATUS_NOT_SUPPORTED           (0xC00000BBL)
#define NDIS_STATUS_NOT_SUPPORTED               ((NDIS_STATUS)STATUS_NOT_SUPPORTED)


TCHAR           szWindowTitle[] = TEXT("PACKET32.DLL");
LPADAPTER       lpTheAdapter    = NULL;
ULONG           ulInstanceCount = 0;


#if DBG

#define ODS(_x) OutputDebugString(TEXT(_x))

#else

#define ODS(_x)

#endif


BOOLEAN StartPacketDriver( LPTSTR ServiceName );
BOOLEAN StopPacketDriver( VOID );


BOOL APIENTRY DllMain( HANDLE hModule, 
		       DWORD  dwReason, 
		       LPVOID lpReserved )
			
/*++

Routine Description:


Arguments:

    hModule - not used

    dwReason - Attach or Detach

	 lpReserved - not used

Return Value:

    SUCCESS - TRUE
    FAILURE - FALSE

--*/

{
	BOOLEAN Status;

	ODS("Packet32: DllEntry\n");

	switch ( dwReason ) 
	{
	case DLL_PROCESS_ATTACH:
		Status = StartPacketDriver( TEXT( "VPACKET" ) );
		break;

	case DLL_PROCESS_DETACH:
		Status = StopPacketDriver( );
		break;

	default:
		Status = TRUE;
		break;
	}

	return Status;
}


LPPACKET
PacketQueryOid( LPADAPTER       lpAdapter, 
				ULONG           ulOid, 
				ULONG           ulLength )
/*++

Routine Description:

    This routine obtains a response from the NIC to an OID

Arguments:

    Adapter object returned from PacketOpenAdapter

Return Value:

    SUCCESS - returns a pointer to an adapter object
    FAILURE - NULL

--*/

{
	ULONG    ioctl;
	LPPACKET lpPacket;


	#define pOidData ((PPACKET_OID_DATA)(lpPacket->Buffer))


	lpPacket = PacketAllocatePacket( lpAdapter );

	if ( lpPacket )
	{
		if ( PacketInitPacket( lpPacket, sizeof(PACKET_OID_DATA)-1+ulLength ) )
		{
			ioctl = ( ulOid >= OID_802_3_PERMANENT_ADDRESS ) 
			
					? IOCTL_PROTOCOL_QUERY_OID : IOCTL_PROTOCOL_STATISTICS;


			pOidData->Oid    = ulOid;
			pOidData->Length = ulLength;

			if ( PacketRequest( lpAdapter, lpPacket, FALSE ) )
			{
				return lpPacket;
			}
		}

		PacketFreePacket( lpPacket );
	}

	return 0;
}


ULONG 
PacketQueryULStatistic( LPADAPTER lpAdapter, 
						ULONG ulOid )
/*++

Routine Description:

    This routine opens an instance of the adapter

Arguments:

    lpstrAdapterName - unicode name of the adapter to open

Return Value:

    SUCCESS - returns a pointer to an adapter object
    FAILURE - NULL

--*/

{
	ULONG           ulStatistic = 0L;
	LPPACKET        lpPacket        = PacketQueryOid( lpAdapter, ulOid, 4L );

	if ( lpPacket )
	{
		ulStatistic = *(ULONG *)((BYTE *)(lpPacket->Buffer) + sizeof(PACKET_OID_DATA) - 1);

		PacketFreePacket( lpPacket );
	}

	return ulStatistic;
}


LPADAPTER
PacketOpenAdapter( LPTSTR lpstrAdapterName )
/*++

Routine Description:

    This routine opens an instance of the adapter

Arguments:

    lpstrAdapterName - unicode name of the adapter to open

Return Value:

    SUCCESS - returns a pointer to an adapter object
    FAILURE - NULL

--*/

{
	LPPACKET lpPacket;

	ODS("Packet32: PacketOpenAdapter\n");

	// Make sure packet driver is started

	if ( !lpTheAdapter )
		StartPacketDriver( TEXT( "VPACKET" ) );

	if ( lpTheAdapter && ( lpTheAdapter->hFile != INVALID_HANDLE_VALUE ) )
	{
		// Get some adapter statistics to use later

		lpTheAdapter->ulMaxFrameSize     = PacketQueryULStatistic( lpTheAdapter, OID_GEN_MAXIMUM_FRAME_SIZE );

		lpTheAdapter->ulMaxLookAheadSize = PacketQueryULStatistic( lpTheAdapter, OID_GEN_MAXIMUM_LOOKAHEAD );

		// Get Current Address

		lpPacket = PacketQueryOid( lpTheAdapter, 
								   OID_802_3_CURRENT_ADDRESS, 
								   ETH_802_3_ADDRESS_LENGTH );

		if ( lpPacket )
		{

			memcpy( lpTheAdapter->CurrentAddress, 
					((BYTE *)(lpPacket->Buffer)) + sizeof(PACKET_OID_DATA) - 1, 
					ETH_802_3_ADDRESS_LENGTH );

			PacketFreePacket( lpPacket );
		}

		// Get Permanent Address

		lpPacket = PacketQueryOid( lpTheAdapter, 
								   OID_802_3_PERMANENT_ADDRESS, 
								   ETH_802_3_ADDRESS_LENGTH );

		if ( lpPacket )
		{

			memcpy( lpTheAdapter->PermanentAddress, 
					((BYTE *)lpPacket->Buffer ) + sizeof(PACKET_OID_DATA) - 1, 
					ETH_802_3_ADDRESS_LENGTH );

			PacketFreePacket( lpPacket );
		}

		return lpTheAdapter;
	}

	ODS("Packet32: PacketOpenAdapter Could not open adapter\n");

	return NULL;
}



VOID
PacketCloseAdapter( LPADAPTER lpAdapter )
{
/*++

Routine Description:

    This routine closes a previouly opened adapter

Arguments:

    Adapter object returned from PacketOpenAdapter

Return Value:


--*/

	ODS( "Packet32: PacketCloseAdapter\n" );

	StopPacketDriver();

	return;
}


LPPACKET
PacketAllocatePacket( LPADAPTER AdapterObject )
{
/*++

Routine Description:

    This rotine this routine allocates a packet object for use
    in sending a receiveing packets

Arguments:

    Adapter object returned from PacketOpenAdapter

Return Value:

    SUCCESS - returns packet object
    FAILURE - NULL

--*/

	LPPACKET lpPacket;

	lpPacket = (LPPACKET) GlobalAllocPtr( GMEM_MOVEABLE | GMEM_ZEROINIT,
										  sizeof(PACKET) );

	if ( lpPacket == NULL ) 
	{
		ODS("Packet32: PacketAllocateSendPacket: GlobalAlloc Failed\n");

		return NULL;
	}

	lpPacket->OverLapped.hEvent = CreateEvent( NULL,
											   FALSE,
											   FALSE,
											   NULL );

	if ( lpPacket->OverLapped.hEvent == NULL )
	{
		ODS("Packet32: PacketAllocateSendPacket: CreateEvent Failed\n");

		GlobalFreePtr(lpPacket);

		return NULL;
	}

	lpPacket->Buffer                        = NULL;
	lpPacket->Length                        = 0;
	lpPacket->ulBytesReceived       = 0;
	lpPacket->bIoComplete           = FALSE;

	return lpPacket;
}


VOID
PacketFreePacket( LPPACKET lpPacket )
{
	if ( lpPacket->Buffer )
		GlobalFreePtr( lpPacket->Buffer );

	CloseHandle(lpPacket->OverLapped.hEvent);

	GlobalFreePtr(lpPacket);
}


BOOLEAN
PacketInitPacket( LPPACKET      lpPacket,
				  UINT          Length )
/*++

Routine Description:

    This rotine initializes a packet object to point to
    a memory buffer described by Start address and length

Arguments:

    lpPacket   -  Packet object returned by PacketAllocatePacket

    Length     -  Length of memory buffer

Return Value:


--*/

{
	if ( lpPacket->Buffer )
		GlobalFreePtr( lpPacket->Buffer );

	lpPacket->Buffer = (LPPACKET) GlobalAllocPtr( GMEM_MOVEABLE | GMEM_ZEROINIT,
																 Length );

	if ( lpPacket->Buffer )                                                                                                                                 
		lpPacket->Length = Length;
	else
		lpPacket->Length = 0;

	return (BOOLEAN) (lpPacket->Length != 0);
}



BOOLEAN
PacketDeviceIoControl( LPADAPTER        lpAdapterObject,
					   LPPACKET             lpPacket,
					   ULONG                ulIoctl,
					   BOOLEAN              bSync )
/*++

Routine Description:

    This rotine sends an ioctl packet to the adapter

Arguments:

    lpAdapterObject  - AdapterObject return by PacketOpenAdapter

    lpPacket         - Packet object returned by PacketAllocatePacket and initialized
		       by PacketInitPacket

    bSync            - TRUE if service should wait for packet to transmit

Return Value:

    SUCCESS - TRUE if succeeded and SYNC==TRUE
    FAILURE -

--*/
{
	BOOLEAN Result;

//  Too much !!!
//      ODS( "Packet32: PacketDeviceIoControl\n" );

	_ASSERTE( lpAdapterObject != NULL );
	_ASSERTE( lpPacket != NULL );

	lpPacket->OverLapped.Offset             = 0;
	lpPacket->OverLapped.OffsetHigh = 0;
	lpPacket->ulBytesReceived               = 0;

	if ( !ResetEvent( lpPacket->OverLapped.hEvent ) ) 
	{
		lpPacket->bIoComplete = FALSE;
		return FALSE;
	}

	Result = DeviceIoControl(       lpAdapterObject->hFile,
								ulIoctl, 
								lpPacket->Buffer, 
								lpPacket->Length, 
								lpPacket->Buffer, 
								lpPacket->Length, 
								&(lpPacket->ulBytesReceived), 
								&(lpPacket->OverLapped) );

	if ( !Result && bSync )
	{
		//
		//  They want to wait
		//

		if ( GetLastError() == ERROR_IO_PENDING )
		{
			Result = GetOverlappedResult(   lpAdapterObject->hFile,
											&(lpPacket->OverLapped),
											&(lpPacket->ulBytesReceived),
											TRUE );
		}
		else
			ODS( "Packet32: unsupported API call return error!\n" );
	} 

	lpPacket->bIoComplete = Result;

	return Result;
}



BOOLEAN
PacketSendPacket( LPADAPTER     AdapterObject,
				  LPPACKET  lpPacket,
				  BOOLEAN   Sync )
/*++

Routine Description:

    This rotine sends a packet to the adapter

Arguments:

    AdapterObject  - AdapterObject return by PacketOpenAdapter

    lpPacket       - Packet object returned by PacketAllocatePacket and initialized
		     by PacketInitPacket

    Sync           - TRUE if service should wait for packet to transmit


Return Value:

    SUCCESS - TRUE if succeeded and SYNC==TRUE
    FAILURE -

--*/

{
	return PacketDeviceIoControl( AdapterObject,
								  lpPacket,
								  IOCTL_PROTOCOL_WRITE,
								  Sync );
}



BOOLEAN
PacketReceivePacket(LPADAPTER   AdapterObject,
					LPPACKET    lpPacket,
					BOOLEAN     Sync )
/*++

Routine Description:

    This rotine issues a receive request from the adapter

Arguments:

    AdapterObject  - AdapterObject return by PacketOpenAdapter

    lpPacket       - Packet object returned by PacketAllocatePacket and initialized
		     by PacketInitPacket

    Sync           - TRUE if service should wait for packet to transmit


Return Value:

    SUCCESS - TRUE if succeeded and SYNC==TRUE
    FAILURE -

--*/

{
	return PacketDeviceIoControl(   AdapterObject,
									lpPacket,
									IOCTL_PROTOCOL_READ,
									Sync );
}



BOOLEAN
PacketWaitPacket( LPADAPTER     AdapterObject,
				  LPPACKET  lpPacket )
/*++

Routine Description:

    This routine waits for an overlapped IO on a packet to complete
    Called if the send or receive call specified FALSE for the Sync parmeter

Arguments:

    AdapterObject  - AdapterObject return by PacketOpenAdapter

    lpPacket       - Packet object returned by PacketAllocatePacket and initialized
		     by PacketInitPacket

Return Value:

    SUCCESS - TRUE if overlapped call succeeded
    FAILURE -

--*/

{
	lpPacket->bIoComplete =  GetOverlappedResult( AdapterObject->hFile,
												    &lpPacket->OverLapped,
												    &lpPacket->ulBytesReceived,
												    TRUE );

	return lpPacket->bIoComplete;
}



BOOLEAN
PacketResetAdapter( LPADAPTER  AdapterObject )
/*++

Routine Description:

    This rotuine resets the adapter. This will complete all pending sends receives and requests

Arguments:

    AdapterObject  - AdapterObject return by PacketOpenAdapter

Return Value:

    SUCCESS - TRUE if overlapped call succeeded
    FAILURE -

--*/

{
	UINT    BytesReturned;

	DeviceIoControl( AdapterObject->hFile,
						  (DWORD)IOCTL_PROTOCOL_RESET,
						  NULL,
						  0,
						  NULL,
						  0,
						  &BytesReturned,
						  NULL );

	return TRUE;
}



BOOLEAN
PacketRequest( LPADAPTER	lpAdapterObject,
			   LPPACKET     lpPacket,
			   BOOLEAN      bSet )

/*++

Routine Description:

    This routine sends issues a request to and adapter

Arguments:

    lpAdapterObject  - AdapterObject return by PacketOpenAdapter

    bSet             - True means that the request is SET otherwise it is a query

    lpOidData        - Structure containing the details of the OID

Return Value:

    SUCCESS -
    FAILURE -

--*/

{
	BOOLEAN Result = FALSE;

	
	ODS( "Packet32: PacketRequest\n" );

	_ASSERTE( lpAdapterObject != NULL );
	_ASSERTE( lpPacket != NULL );
	_ASSERTE( lpPacket->Buffer != NULL );


	Result = PacketDeviceIoControl( lpAdapterObject,
									lpPacket,
									(ULONG) ((bSet) ? IOCTL_PROTOCOL_SET_OID : IOCTL_PROTOCOL_QUERY_OID),
									TRUE );

	if ( lpPacket->ulBytesReceived == 0 )
	{
		// There was an ndis error
		ODS( "Packet32: Ndis returned error to OID\n" );

		Result = FALSE;
	}

	return Result;
}




BOOLEAN
PacketSetFilter( LPADAPTER  AdapterObject, ULONG Filter )
/*++

Routine Description:

    This routine sets the adapters packet filter

Arguments:

    AdapterObject  - AdapterObject return by PacketOpenAdapter

    Filter         - filter to be set

Return Value:

    SUCCESS -
    FAILURE -

--*/

{
	BOOLEAN		Status, bSet;
	ULONG		IoCtlBufferLength=(sizeof(PACKET_OID_DATA)+sizeof(ULONG)-1);
	LPPACKET	lpPacket;
	char 		cstr[48] = "Packet32: Filter  ";


	#define lpOidData       ((PPACKET_OID_DATA)(lpPacket->Buffer))


	ODS( "Packet32: PacketSetFilter\n" );

	_ASSERTE( AdapterObject != NULL );

	_itoa( Filter, (cstr + 18), 16 );
	strcat( cstr, "\n" );
	ODS( cstr );

	lpPacket = PacketAllocatePacket( AdapterObject );

	if ( lpPacket && PacketInitPacket( lpPacket, IoCtlBufferLength ) )
	{
		lpOidData->Oid			  	= OID_GEN_CURRENT_PACKET_FILTER;
		lpOidData->Length		  	= sizeof(ULONG);
		*(PULONG)(lpOidData->Data)	= 0;
		bSet					  	= FALSE;

		// Get filter setting from net card
		Status = PacketRequest( AdapterObject,
								lpPacket,
								bSet );

		if ( Status )						 
		{
			strcpy( (cstr + 10), "Current " );
			_itoa( *(PULONG)(lpOidData->Data), (cstr + 18), 16 );
			strcat( cstr, "\n" );
			ODS( cstr );

			// Are any bits different?  Bitwise Exclusive OR will be zero if
			// all bits are the same
			bSet = ( (*(PULONG)(lpOidData->Data)) ^ Filter ) ? TRUE : FALSE;
		}
		else
		{
			ODS( "Packet32: PacketRequest for OID_GEN_CURRENT_PACKET_FILTER Failed!\n" );
		}

		if ( bSet )
		{
			// Set/clear new bits
			lpOidData->Oid			  	= OID_GEN_CURRENT_PACKET_FILTER;
			lpOidData->Length		  	= sizeof(ULONG);
			*(PULONG)(lpOidData->Data)	= Filter;

			Status = PacketRequest( AdapterObject,
									lpPacket,
									bSet );
		}
		else
			ODS( "Packet32: Query failed or Filter bits equivalent.\n" );

		PacketFreePacket( lpPacket );                                                                   
	}
	else
	{
		ODS( "Packet32: PacketAllocatePacket or PacketInitPacket FAILED!\n" );
		Status = FALSE;
	}

	return Status;
}



BOOLEAN
StartPacketDriver( LPTSTR lpstrServiceName )
/*++

Routine Description:

    This routine Atempts to start the kernel mode packet driver

Arguments:

    lpstrServiceName  - Name of packet driver

Return Value:

    SUCCESS -
    FAILURE -

--*/

{
	BOOLEAN Result;
	char cstr[] = "Packet32: ulInstanceCount = 00\n";

	ODS( "Packet32: StartPacketDriver\n" );

	if ( lpTheAdapter )
	{
		ulInstanceCount++;

		Result = TRUE;
	}
	else
	{
		lpTheAdapter = (LPADAPTER) GlobalAllocPtr( GMEM_MOVEABLE | GMEM_ZEROINIT,
																 sizeof(ADAPTER) );

		if ( lpTheAdapter == NULL ) 
		{
			ODS( "Packet32: StartPacketDriver GlobalAlloc Failed\n" );

			return FALSE;
		}

		wsprintf( lpTheAdapter->SymbolicLink,
					 TEXT( "\\\\.\\%s" ),
					 lpstrServiceName );

		lpTheAdapter->hFile = CreateFile( lpTheAdapter->SymbolicLink,
										  GENERIC_WRITE | GENERIC_READ,
										  0,
										  NULL,
										  OPEN_EXISTING,
										  FILE_FLAG_OVERLAPPED,
										  0 );

		if ( lpTheAdapter->hFile != INVALID_HANDLE_VALUE )
		{
			ulInstanceCount++;

			Result = TRUE;
		}
		else
		{
			ODS("Packet32: StartPacketDriver Could not open adapter\n");

			GlobalFreePtr( lpTheAdapter );

			lpTheAdapter = NULL;

			Result = FALSE;
		}
	}

	cstr[28] += (( ulInstanceCount / 10 ) % 10 );
	cstr[29] += ( ulInstanceCount % 10 );
	ODS( cstr );

	return Result;
}



BOOLEAN
StopPacketDriver( VOID )
/*++

Routine Description:

    This routine Atempts to start the kernel mode packet driver

Arguments:

    none

Return Value:

    SUCCESS -
    FAILURE -

--*/
{
	ODS( "Packet32: StopPacketDriver\n" );

	if ( lpTheAdapter )
	{
		char cstr[] = "Packet32: ulInstanceCount = 00\n";

		cstr[28] += (( ulInstanceCount / 10 ) % 10 );
		cstr[29] += ( ulInstanceCount % 10 );
		ODS( cstr );

		if ( --ulInstanceCount == 0 )
		{
			CloseHandle( lpTheAdapter->hFile );

			GlobalFreePtr( lpTheAdapter );

			lpTheAdapter = NULL;
		}
	}

	return TRUE;
}



BOOLEAN
PacketGetAdapterNames( LPADAPTER lpAdapterObject,
					   LPPACKET  lpPacket )
/*++

Routine Description:

    This routine returns the names all adapters availible

Arguments:

    AdapterObject  - AdapterObject return by PacketOpenAdapter

    Pstr       -  Pointer to a buffer which recieves the UNICODE names
		  Each name is NULL terminated with a second NULL at the end
		  of the list.

    BufferSize -  Size of the buffer passed in


Return Value:


    SUCCESS -
    FAILURE -

--*/

{
	ODS( "Packet32: PacketGetAdapterNames\n" );

	_ASSERTE( lpAdapterObject != NULL );
	_ASSERTE( lpPacket != NULL );

	//
	// Make Synchronous VxD call here
	//
	
	return PacketDeviceIoControl( lpAdapterObject,
								  lpPacket,
								  (ULONG)IOCTL_PROTOCOL_MACNAME,
								  TRUE );
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
