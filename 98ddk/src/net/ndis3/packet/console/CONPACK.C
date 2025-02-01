/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright 1993-95  Microsoft Corporation.  All Rights Reserved.           *
*                                                                           *
****************************************************************************/

/****************************************************************************
*
* PROGRAM: CONPACK.C
*
* PURPOSE: Simple console application for calling Packet Driver
*
* FUNCTIONS:
*  main() - Console application calls Packet Driver which supports 
*           DeviceIoControl. The Packet Driver will return values to this
*           application through this same DeviceIoControl interface.
*
* SPECIAL INSTRUCTIONS:
*								  
****************************************************************************/


#include <stdio.h>
#include <windows.h>
#include <vmm.h>
#include <conio.h>
#include <stdlib.h>

#include "ntddndis.h"
#include "ntddpack.h"
#include "packet32.h"


#define ERROR_OUT(str) { printf( str ); CloseHandle( hEvent ); return( 0 ); }

#define SIMULTANEOUS_READS	5

ULONG   Filters[6] = { 	0,
						NDIS_PACKET_TYPE_DIRECTED,
						NDIS_PACKET_TYPE_MULTICAST,
						NDIS_PACKET_TYPE_BROADCAST,
						NDIS_PACKET_TYPE_ALL_MULTICAST,
						NDIS_PACKET_TYPE_PROMISCUOUS
					 };


LPPACKET QueryOid( LPADAPTER lpAdapter, ULONG ulOid, ULONG ulLength )
{
	ULONG		ioctl;
	LPPACKET lpPacket;


	#define pOidData	((PPACKET_OID_DATA)(lpPacket->Buffer))


	lpPacket = PacketAllocatePacket( lpAdapter );

	if ( lpPacket )
	{
		if ( PacketInitPacket( lpPacket, sizeof(PACKET_OID_DATA)-1+ulLength ) )
		{
			ioctl = ( ulOid >= OID_802_3_PERMANENT_ADDRESS ) 
		  	
		  			? IOCTL_PROTOCOL_QUERY_OID : IOCTL_PROTOCOL_STATISTICS;


			pOidData->Oid    = ulOid;
			pOidData->Length = ulLength;

			printf("DevIoctl OID:%lx Length:%ld\n", pOidData->Oid, pOidData->Length );

			if ( PacketRequest( lpAdapter, lpPacket, FALSE ) )
			{
				printf( "%-20s: %08lx      Length : %lx\n", "DevIoctl Oid", pOidData->Oid, pOidData->Length );

				return lpPacket;
			}

			printf( "%-20s: %08lx      Status : %08lx\n", "DevIoctl Oid", pOidData->Oid, pOidData->Length );
		}
	}

	return 0;
}


BOOLEAN SendPacket(	LPADAPTER lpAdapter, UINT uiPacketSize )
{
	LPPACKET lpPacket;
	        
	lpPacket = PacketAllocatePacket( lpAdapter );

	if ( lpPacket )
	{
		if ( PacketInitPacket( lpPacket, uiPacketSize+ETH_802_3_HEADER_LENGTH ) )
		{
			int i;

			// Set destination address to broadcast

			memset( lpPacket->Buffer, 0xff, ETH_802_3_ADDRESS_LENGTH );

			// Set source address

			memcpy( ((PETH_802_3_FRAME)(lpPacket->Buffer))->SourceAddress, 
					lpAdapter->CurrentAddress,
					ETH_802_3_ADDRESS_LENGTH );

			// Set length count

			((PETH_802_3_FRAME)(lpPacket->Buffer))->LengthCount[0] = HIBYTE(uiPacketSize);
			((PETH_802_3_FRAME)(lpPacket->Buffer))->LengthCount[1] = LOBYTE(uiPacketSize);

			for ( i=0; i<5; i++ )
			{
				if ( PacketSendPacket( lpAdapter, lpPacket, FALSE ) )
					printf( "%-20s: Returned SUCCESS\n", "PacketSendPacket" );
				else
					printf( "%-20s: Returned Pending\n", "PacketSendPacket" );
			}

			printf( "\n" );
		}

		PacketFreePacket( lpPacket );
	}

	return FALSE;
}



BOOLEAN ReadPacket(	LPADAPTER	lpAdapter, 
					LPPACKET   *lplpPacket, 
					BOOLEAN		bSync )
{
	*lplpPacket = PacketAllocatePacket( lpAdapter );

	if ( *lplpPacket )
	{
		if ( PacketInitPacket( *lplpPacket, lpAdapter->ulMaxFrameSize+ETH_802_3_HEADER_LENGTH ) )

			return PacketReceivePacket( lpAdapter, *lplpPacket, bSync );

		PacketFreePacket( *lplpPacket );
	}

	return FALSE;
}


void Query802_3( LPADAPTER lpAdapter, ULONG ulOid, ULONG ulLength, char *name )
{
	LPPACKET lpPacket = QueryOid( lpAdapter, ulOid, ulLength );

	if ( lpPacket )
	{
		ULONG i;
		BYTE* buffer = lpPacket->Buffer;
		
		buffer += sizeof(PACKET_OID_DATA) - 1;

		printf( "%-20s: ", name );

		for ( i=0; i<ulLength; i++ )
			printf( "%02x ", buffer[i] );

		PacketFreePacket( lpPacket );
	}

	printf( "\n\n" );

	return;
}


ULONG QueryULStatistic( LPADAPTER lpAdapter, ULONG ulOid, char *name )
{
	ULONG		ulStatistic = 0L;
	ULONG		ulLength = 4L;
	LPPACKET lpPacket = QueryOid( lpAdapter, ulOid, ulLength );

	if ( lpPacket )
	{
		ULONG i;
		BYTE* buffer = lpPacket->Buffer;
		
		buffer     += sizeof(PACKET_OID_DATA) - 1;
		ulStatistic = *(ULONG *)buffer;

		printf( "%-20s: %lu     ", name, ulStatistic );

		for ( i=0; i<ulLength; i++ )
			printf( "%02x ", buffer[i] );

		PacketFreePacket( lpPacket );
	}

	printf( "\n\n" );

	return ulStatistic;
}


void QueryMacName( LPADAPTER lpAdapter )
{
	DWORD		dwLength = 32;
	BYTE		name[10] = "Mac Name";
	LPPACKET lpPacket;

	//
	// Prepare data for call to VxD.
	//
	lpPacket = PacketAllocatePacket( lpAdapter );

	if ( lpPacket )
	{
		if ( PacketInitPacket( lpPacket, dwLength ) )
		{
			if ( PacketGetAdapterNames( lpAdapter, lpPacket ) )
			{
				printf( "%-20s: %s		", name, lpPacket->Buffer );
			}

			printf( "\n\n" );

			PacketFreePacket( lpPacket );
		}
	}

	return;
}


BOOLEAN CheckForData( LPADAPTER lpAdapter, LPPACKET *lplpPacket )
{
	LPPACKET lpPacket = *lplpPacket;

	ULONG	i, j, ulLines, ulLen, ulBytesReceived;
	BOOLEAN bIoComplete;
	char	*pChar, *pLine;

	if ( !lpPacket || lpPacket->bIoComplete )
	{
		if ( !lpPacket )
		{
			// Create lpPacket and read for the first time

			bIoComplete = ReadPacket( lpAdapter, lplpPacket, FALSE );

			lpPacket    = *lplpPacket;

			if ( bIoComplete )

				printf("%-20s: %s \n", "ReadPacket", "Returned Synchronously" );
		}
		else if ( lpPacket->bIoComplete )
		{
			// We completed on the last loop so get another one

			bIoComplete = PacketReceivePacket( lpAdapter, 
											   lpPacket, 
											   FALSE );
		}


		if ( !bIoComplete && ( (i = GetLastError()) != ERROR_IO_PENDING ) )
		{
			printf( "%-20s: i != ERROR_IO_PENDING (%lx) \n", 
		 			"GetLastError", 
		 			i, 
		 			ERROR_IO_PENDING );

		 	// Let's try again
			
			if ( *lplpPacket )
				PacketFreePacket( *lplpPacket );

			*lplpPacket = 0;
		 
		 	return TRUE;
		}
	}
	else
	{
		// We are trying to complete a pending read

		bIoComplete = GetOverlappedResult( lpAdapter->hFile,
					 						&lpPacket->OverLapped,
					 						&lpPacket->ulBytesReceived,
					 						FALSE );

		// We have to set the complete status ourselves

		lpPacket->bIoComplete = bIoComplete;
	}


	if ( bIoComplete )
	{
		// We completed either synchronously or assynchronously

		ulBytesReceived = lpPacket->ulBytesReceived;

		ulLines = (ulBytesReceived + 15) / 16;

		if ( ulLines > 10 ) ulLines = 10;

		printf( "%-20s: %08lx  %lu  %lu  %lu\n", 
				"Packet Received",  
				lpPacket,
				ulBytesReceived,
				lpPacket->Length,
				ulLines );

		pChar = lpPacket->Buffer;

		for ( i=0; i<ulLines; i++ )
		{
			pLine = pChar;

			printf( "%08lx : ", pChar );

			ulLen = ulBytesReceived;
			ulLen = ( ulLen > 16 ) ? 16 : ulLen;
			ulBytesReceived -= ulLen;

			for ( j=0; j<ulLen; j++ )
				printf( "%02x ", *(BYTE *)pChar++ );

			if ( ulLen < 16 )
				printf( "%*s", (16-ulLen)*3, " " );

			pChar = pLine;

			for ( j=0; j<ulLen; j++, pChar++ )
				printf( "%c", isprint( *pChar ) ? *pChar : '.' );

			printf( "\n" );
		}

		printf( "\n" );
	} 

	return bIoComplete;
}


int __cdecl main( int argc, char *argv[] )
{
	LPADAPTER	lpAdapter 	= 0;
	LPPACKET	lpPacket[SIMULTANEOUS_READS];
	ULONG		ulFilter    = NDIS_PACKET_TYPE_DIRECTED|NDIS_PACKET_TYPE_BROADCAST;
	DWORD		dwErrorCode;
	char		cKeyPress;
	int			i, j;
	int			iSimultaneousReads	= SIMULTANEOUS_READS;
	int			iPacketSize			= 64;
	int			iLoopCount			= 1;


	while ( --argc && *++argv )
	{
		char *pChar = *argv;

		if ( (*pChar == '-') || (*pChar == '/') )
			*pChar++;

		switch ( *pChar )
		{
		case 'p':
		case 'P':
			iPacketSize = atoi( pChar+1 );
			break;

		case 's':
		case 'S':
			iSimultaneousReads = atoi( pChar+1 );
			break;

		case 'l':
		case 'L':
			iLoopCount = atoi( pChar+1 );
			break;

		default:
			printf( "USAGE: conpack [-pn] [-sn] [-?|h]\n\n" );
			printf( "\tpn\tpacket size for sends\n" );
			printf( "\tsn\tnumber of simultaneous reads\n" );
			printf( "\tln\tnumber of times to run operation\n" );
			printf( "\t?\tthis help text\n\n" );
			return (-1);
		}
	}

	//
	// Dynamically prepare to call VPACKET
	// The FILE_FLAG_OVERLAPPED flag is used to inform the Win32 subsystem
	// that the VxD will be processing some DeviceIOControl calls
	// asynchronously.
	//

	#define hVxD (lpAdapter->hFile)

	for ( j=0; j<iLoopCount; j++ )
	{
		// Let's do this twice for testing purposes

		memset( lpPacket, 0, sizeof( LPPACKET ) * iSimultaneousReads );

		lpAdapter = PacketOpenAdapter( TEXT( "_PACKET_PPPMac" ) );

		if ( !lpAdapter || lpAdapter->hFile == INVALID_HANDLE_VALUE )
		{
			dwErrorCode = GetLastError();

			if ( dwErrorCode == ERROR_NOT_SUPPORTED )
				printf("Unable to open VxD,\ndevice does not support DeviceIOCTL\n");
			else
				printf("Unable to open VxD, Error code: %lx\n", dwErrorCode);
			
			return(0);
		}


		QueryMacName( lpAdapter );
		QueryULStatistic( lpAdapter, OID_GEN_MAC_OPTIONS, "Mac Options" );
		QueryULStatistic( lpAdapter, OID_GEN_HARDWARE_STATUS, "Hardware Status" );


		printf( "%-20s: ", "Current Address" );

		for ( i=0; i<ETH_802_3_ADDRESS_LENGTH; i++ )
			printf( "%02x ", *((BYTE *)lpAdapter->CurrentAddress + i) );

		printf( "\n" );

		printf( "%-20s: ", "Permanent Address" );

		for ( i=0; i<ETH_802_3_ADDRESS_LENGTH; i++ )
			printf( "%02x ", *((BYTE *)lpAdapter->PermanentAddress + i) );

		printf( "\n" );

		printf( "%-20s: %lu     \n", "Max Frame Size", lpAdapter->ulMaxFrameSize );
		printf( "%-20s: %lu     \n", "Max Look Ahead Size", lpAdapter->ulMaxLookAheadSize );
		printf( "%-20s: %d      \n", "Send Packet Size", iPacketSize );


		for ( i=0; i<iSimultaneousReads; i++ )
		{
			CheckForData( lpAdapter, &(lpPacket[i]) );
			printf( "%-20s: %d %x\n", "Simultaneous Read", i, lpPacket[i] );
		}

		printf( "\nPress Any Key to Continue\n\n" );
		while( !_kbhit() );
		_getch();


		// Get's directed and broadcast packets
		PacketSetFilter( lpAdapter, ulFilter );
		printf( "%-20s: %lu     \n\n", "OID_GEN_CURRENT_PACKET_FILTER", ulFilter );


		i = 0;

		do
		{
			while ( !_kbhit() )
			{
				CheckForData( lpAdapter, &(lpPacket[i]) );

				i++;
				if ( i >= iSimultaneousReads ) i = 0;
			}

			// A key was pressed
			cKeyPress = toupper( _getch() );

			switch ( cKeyPress )
			{
			case 'H':
				printf( "\nHalting: Press Any Key To Continue.\n" );
				_getch();
				break;

			case 'S':
				// Let's send a packet
				SendPacket( lpAdapter, iPacketSize );
				break;

			case 'Q':
				// We're gone!
				break;
			}
		}			 
		while ( cKeyPress != 'Q' );


		for ( i=0; i<iSimultaneousReads; i++ )	
		{
			PacketFreePacket( lpPacket[i] );
		}


		PacketSetFilter( lpAdapter, 0 );
		printf( "%-20s: %lu     \n\n", "OID_GEN_CURRENT_PACKET_FILTER", 0 );

		PacketCloseAdapter( lpAdapter );
	}

	return( 0 );
}
