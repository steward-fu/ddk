


#ifndef __PACKET32
#define __PACKET32


#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif /* __cplusplus */


#define     DOSNAMEPREFIX   TEXT("Packet_")

#define     MAX_LINK_NAME_LENGTH		64

#define		ETH_802_3_HEADER_LENGTH		14
#define		ETH_802_3_ADDRESS_LENGTH	6

typedef struct _ADAPTER 
{
    HANDLE  hFile;

    TCHAR   SymbolicLink[ MAX_LINK_NAME_LENGTH ];
	TCHAR	CurrentAddress[ ETH_802_3_ADDRESS_LENGTH ];
	TCHAR	PermanentAddress[ ETH_802_3_ADDRESS_LENGTH ];

	ULONG	ulMaxFrameSize;
	ULONG	ulMaxLookAheadSize;
} 
ADAPTER, *LPADAPTER;


typedef struct _ETH_802_3_FRAME
{
	BYTE	DestinationAddress[ ETH_802_3_ADDRESS_LENGTH ];
	BYTE	SourceAddress[ ETH_802_3_ADDRESS_LENGTH ];
	BYTE	LengthCount[2];
	BYTE	PduBuffer[1];
}
ETH_802_3_FRAME, *PETH_802_3_FRAME;


typedef struct _PACKET 
{
    HANDLE       hEvent;
    OVERLAPPED   OverLapped;
    PVOID        Buffer;
    UINT         Length;
	ULONG		 ulBytesReceived;
	BOOLEAN		 bIoComplete;
} 
PACKET, *LPPACKET;



LPADAPTER
PacketOpenAdapter(
    LPTSTR   AdapterName
    );


BOOLEAN
PacketSendPacket(
    LPADAPTER   AdapterObject,
    LPPACKET    lpPacket,
    BOOLEAN     Sync
    );


LPPACKET
PacketAllocatePacket(
    LPADAPTER   AdapterObject
    );


BOOLEAN
PacketInitPacket(
    LPPACKET    lpPacket,
    UINT        Length
    );

VOID
PacketFreePacket(
    LPPACKET    lpPacket
    );



BOOLEAN
PacketResetAdapter(
    LPADAPTER  AdapterObject
    );


BOOLEAN
PacketGetAddress(
    LPADAPTER  AdapterObject,
    PUCHAR     AddressBuffer,
    PUINT       Length
    );



BOOLEAN
PacketDeviceIoControl( LPADAPTER	lpAdapterObject,
					   LPPACKET		lpPacket,
					   ULONG		ulIoctl,
					   BOOLEAN		bSync );

BOOLEAN
PacketRequest( LPADAPTER	lpAdapterObject,
			   LPPACKET		lpPacket,
			   BOOLEAN		bSet );

BOOLEAN
PacketWaitPacket( LPADAPTER  AdapterObject,
				  LPPACKET   lpPacket );

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

BOOLEAN
PacketReceivePacket(
    LPADAPTER   AdapterObject,
    LPPACKET    lpPacket,
    BOOLEAN     Sync
    );
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



VOID
PacketCloseAdapter(
    LPADAPTER   lpAdapter
    );

BOOLEAN
PacketSetFilter(
    LPADAPTER  AdapterObject,
    ULONG      Filter
    );
/*++

Routine Description:

    This rotine sets the adapters packet filter

Arguments:

    AdapterObject  - AdapterObject return by PacketOpenAdapter

    Filter         - filter to be set

Return Value:

    SUCCESS -
    FAILURE -

--*/


BOOLEAN
PacketGetAdapterNames( LPADAPTER lpAdapter,
					   LPPACKET  lpPacket );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __PACKET32 */
