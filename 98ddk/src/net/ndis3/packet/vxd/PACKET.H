/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    packet.h

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


#define  MAX_REQUESTS   4


#ifdef DEBUG

#define PACKETASSERT(a)         if( !(a) ) { DbgPrint( "Packet: ASSERTION FAILED!\r\n" ); DbgBreakPoint(); }

#else

#define PACKETASSERT(a)

#endif

typedef struct _PACKET_RESERVED 
{
	LIST_ENTRY      ListElement;

	char*           lpBuffer;
	DWORD           cbBuffer;
	DWORD*          lpcbBytesReturned;
	OVERLAPPED*     lpoOverlapped;
	DWORD           hDevice;
	DWORD           tagProcess;
}  
PACKET_RESERVED, *PPACKET_RESERVED;


typedef struct _INTERNAL_REQUEST 
{
	PACKET_RESERVED Reserved;
   NDIS_REQUEST  Request;
} 
INTERNAL_REQUEST, *PINTERNAL_REQUEST;


//
// Port device extension.
//

typedef struct _OPEN_INSTANCE 
{
	LIST_ENTRY                      ListElement;

	NDIS_STATUS                     Status;
	NDIS_HANDLE                     AdapterHandle;
	NDIS_HANDLE                     PacketPool;
	NDIS_HANDLE                     BufferPool;
	NDIS_HANDLE                     BindAdapterContext;

	NDIS_SPIN_LOCK          RcvQSpinLock;
	LIST_ENTRY                      RcvList;

	NDIS_SPIN_LOCK          RequestSpinLock;
	LIST_ENTRY                      RequestList;

	NDIS_SPIN_LOCK          ResetSpinLock;
	LIST_ENTRY                      ResetIrpList;

	INTERNAL_REQUEST        Requests[MAX_REQUESTS];
} 
OPEN_INSTANCE, *POPEN_INSTANCE;


typedef struct _DEVICE_EXTENSION 
{
	PDRIVER_OBJECT          DriverObject;
	NDIS_HANDLE                     NdisProtocolHandle;
	LIST_ENTRY                      OpenList;
} 
DEVICE_EXTENSION, *PDEVICE_EXTENSION;


extern PDEVICE_EXTENSION GlobalDeviceExtension;


#define ETHERNET_HEADER_LENGTH  14

#define RESERVED(_p) ((PPACKET_RESERVED)((_p)->ProtocolReserved))

#define TRANSMIT_PACKETS        16

#define REGBUFSIZE                              256


extern void YieldExecution( void );


PLIST_ENTRY
PacketRemoveHeadList(
    IN PLIST_ENTRY pListHead
    );


VOID NDIS_API
PacketBindAdapterComplete(
    IN NDIS_HANDLE  ProtocolBindingContext,
    IN NDIS_STATUS  Status,
    IN NDIS_STATUS  OpenErrorStatus
    );

VOID NDIS_API
PacketUnbindAdapterComplete(
    IN NDIS_HANDLE  ProtocolBindingContext,
    IN NDIS_STATUS  Status
    );


NDIS_STATUS NDIS_API
PacketReceiveIndicate(
    IN NDIS_HANDLE ProtocolBindingContext,
    IN NDIS_HANDLE MacReceiveContext,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookAheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    );

VOID NDIS_API
PacketReceiveComplete(
    IN NDIS_HANDLE  ProtocolBindingContext
    );

DWORD
PacketRequest( POPEN_INSTANCE           pOpen,
					DWORD                           FunctionCode,
					DWORD                           dwDDB,
	       DWORD                                    hDevice,
	       PDIOCPARAMETERS  pDiocParms
	);

VOID NDIS_API
PacketRequestComplete(
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN PNDIS_REQUEST pRequest,
    IN NDIS_STATUS   Status
    );

VOID NDIS_API
PacketSendComplete(
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN PNDIS_PACKET  pPacket,
    IN NDIS_STATUS   Status
    );

VOID
PacketReset( PNDIS_STATUS               pStatus,
				 POPEN_INSTANCE pOpen );

VOID NDIS_API
PacketResetComplete(
    IN NDIS_HANDLE  ProtocolBindingContext,
    IN NDIS_STATUS  Status
    );


VOID NDIS_API
PacketStatus(
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN NDIS_STATUS   Status,
    IN PVOID         StatusBuffer,
    IN UINT          StatusBufferSize
    );


VOID NDIS_API
PacketStatusComplete(
    IN NDIS_HANDLE      ProtocolBindingContext
    );

VOID
PacketAllocatePacketBuffer( PNDIS_STATUS        pStatus,
						 POPEN_INSTANCE         pOpen,
						 PNDIS_PACKET           *lplpPacket,
						 PDIOCPARAMETERS        pDiocParms,
						 DWORD                          FunctionCode );

VOID NDIS_API
PacketTransferDataComplete(
    IN NDIS_HANDLE      ProtocolBindingContext,
    IN PNDIS_PACKET     Packet,
    IN NDIS_STATUS      Status,
    IN UINT                     BytesTransferred
    );


VOID
PacketRemoveReference(
    IN PDEVICE_EXTENSION DeviceExtension
    );


VOID 
PacketFreeResources( POPEN_INSTANCE Open );


VOID
PacketCleanUp( PNDIS_STATUS     Status,
					POPEN_INSTANCE Open );


NTSTATUS NDIS_API
PacketShutdown(
    IN PDEVICE_OBJECT DeviceObject
    );

VOID NDIS_API
PacketUnload();



VOID NDIS_API
PacketBindAdapter( OUT PNDIS_STATUS Status,
				IN  NDIS_HANDLE  BindAdapterContext,
				IN  PNDIS_STRING AdapterName,
				IN  PVOID        SystemSpecific1,
				IN  PVOID        SystemSpecific2 
				);

VOID NDIS_API
PacketUnbindAdapter( OUT PNDIS_STATUS   Status,
				 IN NDIS_HANDLE ProtocolBindingContext,
				 IN NDIS_HANDLE UnbindContext
				 );

DWORD
PacketWrite( POPEN_INSTANCE     Open,
				 DWORD                                  dwDDB,
	     DWORD                              hDevice,
				 PDIOCPARAMETERS        pDiocParms
	);

DWORD
PacketRead( POPEN_INSTANCE              Open,
				DWORD                           dwDDB,
	    DWORD                               hDevice,
				PDIOCPARAMETERS   pDiocParms
	);

DWORD _stdcall PacketIOControl( DWORD                   dwService,
				DWORD                   dwDDB,
				DWORD                   hDevice,
				PDIOCPARAMETERS lpDIOCParms );

//
// define wrapper for VWIN32_DIOCCompletionRoutine 
//

void VXDINLINE
VWIN32_DIOCCompletionRoutine( DWORD hEvent )
{
	_asm mov ebx, hEvent
	VxDCall( VWIN32_DIOCCompletionRoutine );
}


#define strcat( d, s )  NdisMoveMemory( d+strlen(d), s, strlen(s) )


//
// where is memset defined?  okay, let's make one!
//

#define memset( _S, _C, _N )\
{\
	UCHAR* _pS = _S;\
	ULONG  _I  = _N;\
	while ( _I-- )\
	{\
		*_pS++ = _C;\
	}\
}
