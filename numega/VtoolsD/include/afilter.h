/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    afilter.h

Abstract:

    Header file for the address filtering library for NDIS MAC's.

Author:

    Alireza Dabagh creation-date 3-22-1993, mostly borrowed from efilter.h

Revision History:

--*/

#ifndef _ARC_FILTER_DEFS_
#define _ARC_FILTER_DEFS_


#define ARC_BUFFER_IN_USE           0x00000001
#define ARC_BUFFER_MEM_ALLOCATED    0x00000002

#define ARC_RECEIVE_PACKETS         8
#define ARC_RECEIVE_BUFFERS         32



#define ARC_SEND_PACKETS            32
#define ARC_SEND_BUFFERS            (ARC_SEND_PACKETS * 8)

struct _ARC_BUFFER;

//
// Arcnet specific buffer pool
//

typedef struct _ARC_BUFFER_POOL{
    NDIS_SPIN_LOCK SpinLock;
    struct _ARC_BUFFER *FreeList;
    UINT BufferLength;
    UINT PoolLength;
    UCHAR Buffer[1];
    } ARC_BUFFER_POOL, *PARC_BUFFER_POOL;

//
// Linked list Structure for keeping track of allocated memory so we can free them later
//
typedef struct _ARC_ALLOCATED_BUFFER_NODE{
    PUCHAR  Address;
    UINT    Size;
    struct _ARC_ALLOCATED_BUFFER_NODE *pNext;
    } ARC_ALLOCATED_BUFFER_NODE, *PARC_ALLOCATED_BUFFER_NODE;

//
// Generic buffer node in a linked list of buffers
//

struct _ARC_FILTER;

typedef struct _ARC_MEM_BUFFER_NODE{
    struct _ARC_MEM_BUFFER_NODE *pNext;
    struct _ARC_FILTER *Filter;
    UCHAR   Data;
    } ARC_MEM_BUFFER_NODE, *PARC_MEM_BUFFER_NODE;


//
// Arcnet specific buffer descriptor
//

typedef struct _ARC_BUFFER{
    PVOID   VirtualAddress;
    UINT    Length;
    UINT    Flags;
    INT     PhysBufferIndex;
    PARC_BUFFER_POOL Pool;
    struct _ARC_BUFFER *Next;
    } ARC_BUFFER, *PARC_BUFFER;

//
// Arcnet specific packet pool
//

struct _ARC_PACKET;

typedef struct _ARC_PACKET_POOL{
    NDIS_SPIN_LOCK SpinLock;            //
    struct _ARC_PACKET *FreeList;       // Pointer to the linked list of free packet descriptor
    UINT PacketLength;                  // Length of each packet descriptor
    UINT PoolLength;
    UCHAR Buffer[1];                    // start of packets
    } ARC_PACKET_POOL, * PARC_PACKET_POOL;

//
// This is the structure that is passed to the protocol as the packet
// header during receive indication. It is also the header expected from the protocol.
// This header is NOT the same as the header passed to the mac driver
//

#define ARCNET_ADDRESS_LEN                     1

typedef struct _ARC_PROTOCOL_HEADER {
    UCHAR   SourceId[ARCNET_ADDRESS_LEN];      // Source Address
    UCHAR   DestId[ARCNET_ADDRESS_LEN];        // Destination Address
    UCHAR   ProtId;                         // Protocol ID
    } ARC_PROTOCOL_HEADER, *PARC_PROTOCOL_HEADER;

//
// This structure keeps track of information about a received packet
//
typedef struct _ARC_PACKET_HEADER {
    ARC_PROTOCOL_HEADER ProtHeader;         // Protocol header
    USHORT  FrameSequence;                  // Frame sequence Number
    UCHAR   SplitFlag;                      // Split flag
    UCHAR   LastSplitFlag;                  // Split Flag for the last frame
    UCHAR   FramesReceived;                 // Frames in This Packet
    } ARC_PACKET_HEADER, * PARC_PACKET_HEADER;

//
// Arcnet specific packet header
//
typedef struct _ARC_PACKET{
    ARC_PACKET_HEADER Header;       // Information about the packet
    PARC_BUFFER Head;               // First buffer in the chain
    PARC_BUFFER Tail;               // Last buffer in the chain
    struct _ARC_PACKET * pNextInUse;// Next packet in use by filter
    struct _ARC_PACKET * pPrevInUse;// Previous packet in use by filter
    UINT TotalLength;               // Total Length of data in packet
    PARC_PACKET_POOL Pool;          // Packet pool
    } ARC_PACKET, * PARC_PACKET;


#define ARC_HEADER_SID_OFFSET   0
#define ARC_HEADER_DID_OFFSET   1
#define ARC_DATA_SP_OFFSET      1
#define ARC_DATA_FS_OFFSET      2
#define ARC_DATA_SC_OFFSET      0
#define ARC_DATA_DATA_OFFSET    4

#define ARC_PROTOCOL_HEADER_SIZE    (sizeof(ARC_PROTOCOL_HEADER))
#define ARC_MAX_FRAME_SIZE          504
#define ARC_MAX_ADDRESS_IDS         256
#define ARC_MAX_FRAME_HEADER_SIZE   6
#define ARC_MAX_PACKET_SIZE         576
#define ARC_MAX_FRAMES_IN_PACKET    ((ARC_MAX_PACKET_SIZE + ARC_MAX_FRAME_SIZE -1)/ARC_MAX_FRAME_SIZE)
//
// Number of free receive buffer
//
#define ARC_DEFAULT_RECEIVE_MEM_BUFFERS     8

#define ARC_DEFAULT_HEADER_BUFFERS          32

//
// Check whether an address is broadcast.
//

#define ARC_IS_BROADCAST(Address) \
    (BOOLEAN)(!(Address))


//
// This is the header that is passed to mac driver for each packet
//
typedef struct _ARC_MAC_PACKET_HEADER{
    UCHAR SourceId;
    UCHAR DestId;
    UCHAR ProtId;
    } ARC_MAC_PACKET_HEADER, * PARC_MAC_PACKET_HEADER;

#define ARC_MAC_HEADER_SIZE     (sizeof(ARC_MAC_PACKET_HEADER))

//
// This is the header that filter has to add to the beginning of each
// frame passed to mac driver to be sent. The SplitFlag2 and
// FrameSequence2 are present only if the frame is an exception frame
//

typedef struct _ARC_MAC_FRAME_HEADER{
    UCHAR   SplitFlag;
    USHORT  FrameSequence;
    UCHAR   SplitFlag2;
    USHORT  FrameSequence2;
    } ARC_MAC_FRAME_HEADER, * PARC_MAC_FRAME_HEADER;


//
// An action routine type.  The routines are called
// when a filter type is set for the first time or
// no more bindings require a particular type of filter.
//
// NOTE: THIS ROUTINE SHOULD ASSUME THAT THE LOCK IS ACQUIRED.
//
typedef
NDIS_STATUS
(*ARC_FILTER_CHANGE)(
    IN UINT OldFilterClasses,
    IN UINT NewFilterClasses,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );


//
// This action routine is called when the mac requests a close for
// a particular binding *WHILE THE BINDING IS BEING INDICATED TO
// THE PROTOCOL*.  The filtering package can't get rid of the open
// right away.  So this routine will be called as soon as the
// NdisIndicateReceive returns.
//
// NOTE: THIS ROUTINE SHOULD ASSUME THAT THE LOCK IS ACQUIRED.
//
typedef
VOID
(*ARC_DEFERRED_CLOSE)(
    IN NDIS_HANDLE MacBindingHandle
    );

typedef ULONG MASK,*PMASK;

//
// Maximum number of opens the filter package will support.  This is
// the max so that bit masks can be used instead of a spaghetti of
// pointers.
//
#define ARC_FILTER_MAX_OPENS (sizeof(ULONG) * 8)


//
// The binding info is threaded on two lists.  When
// the binding is free it is on a single freelist.
//
// When the binding is being used it is on a doubly linked
// index list.
//
typedef struct _ARC_BINDING_INFO {
    NDIS_HANDLE MacBindingHandle;
    NDIS_HANDLE NdisBindingContext;
    UINT PacketFilters;
    ULONG References;
    struct _ARC_BINDING_INFO *NextOpen;
    struct _ARC_BINDING_INFO *PrevOpen;
    BOOLEAN ReceivedAPacket;
    UCHAR FilterIndex;
} ARC_BINDING_INFO,*PARC_BINDING_INFO;

//
// An opaque type that contains a filter database.
// The MAC need not know how it is structured.
//
typedef struct _ARC_FILTER {

    //
    // Spin lock used to protect the filter from multiple accesses.
    //
    PNDIS_SPIN_LOCK Lock;

    //
    // Combination of all the filters of all the open bindings.
    //
    UINT CombinedPacketFilter;

    //
    // Pointer for traversing the open list.
    //
    PARC_BINDING_INFO OpenList;


    //
    // Action routines to be invoked on notable changes in the filter.
    //

    ARC_FILTER_CHANGE FilterChangeAction;
    ARC_DEFERRED_CLOSE CloseAction;

    //
    // Bit mask of opens that are available.
    //
    ULONG FreeBindingMask;
    UINT MaxPhysBuffers;
    UINT CurPhysBufferIndex;

    NDIS_HANDLE ReceivePacketPool;

    NDIS_HANDLE SendPacketPool;

    NDIS_HANDLE ReceiveBufferPool;

    NDIS_HANDLE SendBufferPool;

    NDIS_HANDLE PacketPoolHandle;

    NDIS_HANDLE BufferPoolHandle;

    PARC_PACKET pOutstandingPackets;

    NDIS_SPIN_LOCK OutstandingPacketsLock;

    PVOID * pPhysBufferArray;

    PUSHORT pSequenceNumbers;

    PARC_ALLOCATED_BUFFER_NODE pAllocatedBufferList;
    
    NDIS_SPIN_LOCK AllocatedBufferListLock;
    
    PARC_MEM_BUFFER_NODE pFreeReceiveBufferList;
    
    NDIS_SPIN_LOCK FreeReceiveBufferListLock;
    
    PARC_MEM_BUFFER_NODE pFreeHeaderBufferList;
    
    NDIS_SPIN_LOCK FreeHeaderBufferListLock;

    //
    // Address of the adapter.
    //
    UCHAR AdapterAddress;

} ARC_FILTER,*PARC_FILTER;




//
//UINT
//ARC_QUERY_FILTER_CLASSES(
//    IN PARC_FILTER Filter
//    )
//
// This macro returns the currently enabled filter classes.
//
// NOTE: THIS MACRO ASSUMES THAT THE FILTER LOCK IS HELD.
//
#define ARC_QUERY_FILTER_CLASSES(Filter) ((Filter)->CombinedPacketFilter)


//
//UINT
//ARC_QUERY_PACKET_FILTER(
//    IN ARC_FILTER Filter,
//    IN NDIS_HANDLE NdisFilterHandle
//    )
//
// This macro returns the currently enabled filter classes for a specific
// open instance.
//
// NOTE: THIS MACRO ASSUMES THAT THE FILTER LOCK IS HELD.
//
#define ARC_QUERY_PACKET_FILTER(Filter, NdisFilterHandle) \
       (((PARC_BINDING_INFO)(NdisFilterHandle))->PacketFilters)




VOID NDIS_API
ArcFilterAllocatePacketPool(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE PoolHandle,
    IN PARC_FILTER Filter,
    IN UINT NumberOfDescriptors
    );


NDIS_STATUS NDIS_API
ArcFilterFreePacketPool(
    IN NDIS_HANDLE PoolHandle
    );


VOID NDIS_API
ArcFilterAllocatePacket(
    OUT PNDIS_STATUS Status,
    OUT PARC_PACKET * Packet,
    IN NDIS_HANDLE PoolHandle
    );


VOID NDIS_API
ArcFilterFreePacket(
    IN PARC_PACKET Packet
    );


VOID NDIS_API
ArcFilterAllocateBufferPool(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE PoolHandle,
    IN UINT NumberOfDescriptors
    );


NDIS_STATUS NDIS_API
ArcFilterFreeBufferPool(
    IN NDIS_HANDLE PoolHandle
    );


VOID NDIS_API
ArcFilterAllocateBuffer(
    OUT PNDIS_STATUS Status,
    OUT PARC_BUFFER * Buffer,
    IN NDIS_HANDLE PoolHandle,
    IN PVOID VirtualAddress,
    IN UINT Length
    );


VOID NDIS_API
ArcFilterFreeBuffer(
    IN PARC_BUFFER Buffer
    );

VOID NDIS_API
ArcFilterChainBufferAtBack(
    IN OUT PARC_PACKET Packet,
    IN OUT PARC_BUFFER Buffer
    );


VOID NDIS_API
ArcFilterDiscardPacket(
    IN PARC_FILTER Filter,
    IN PARC_PACKET pPacket
    );

NDIS_STATUS NDIS_API
ArcFilterConvertToNdisPacket(
    IN PARC_FILTER Filter,
    IN PARC_PACKET pArcPacket,
    IN PNDIS_PACKET pNdisPacket
    );

BOOLEAN NDIS_API
ArcCreateFilter(
    IN ARC_FILTER_CHANGE FilterChangeAction,
    IN ARC_DEFERRED_CLOSE CloseAction,
    IN UCHAR AdapterAddress,
    IN PNDIS_SPIN_LOCK Lock,
    IN UINT MaxPhysBuffers,
    IN UINT MaxBuffers,
    OUT PARC_FILTER *Filter
    );


VOID NDIS_API
ArcDeleteFilter(
    IN PARC_FILTER Filter
    );

BOOLEAN NDIS_API
ArcNoteFilterOpenAdapter(
    IN PARC_FILTER Filter,
    IN NDIS_HANDLE MacBindingHandle,
    IN NDIS_HANDLE NdisBindingContext,
    OUT PNDIS_HANDLE NdisFilterHandle
    );

NDIS_STATUS NDIS_API
ArcDeleteFilterOpenAdapter(
    IN PARC_FILTER Filter,
    IN NDIS_HANDLE NdisFilterHandle,
    IN PNDIS_REQUEST NdisRequest
    );

NDIS_STATUS NDIS_API
ArcFilterAdjust(
    IN PARC_FILTER Filter,
    IN NDIS_HANDLE NdisFilterHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN UINT FilterClasses,
    IN BOOLEAN Set
    );


VOID NDIS_API
ArcFilterIndicateReceive(
    IN PARC_FILTER Filter,
    IN NDIS_HANDLE MacReceiveContext,
    IN UCHAR Address,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookaheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    );

VOID NDIS_API
ArcFilterIndicateReceiveComplete(
    IN PARC_FILTER Filter
    );

VOID NDIS_API
ArcFilterIndicateReceiveFrame(
    IN PARC_FILTER Filter,          // Pointer to filter database
    IN PUCHAR pRawHeader,           // Pointer to Arcnet frame header
    IN PUCHAR pData,                // Pointer to data portion of Arcnet frame
    IN UINT Length,                 // Data Length
    IN UINT LookaheadBufferSize     //
    );

NDIS_STATUS NDIS_API
ArcFilterTransferData(
    IN PARC_FILTER Filter,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer,
    OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransfered
    );



typedef
VOID
(*ARC_SEND_RAW)(
    IN NDIS_HANDLE  MacBindingHandle,
    IN PNDIS_PACKET FragmentPacket,
    IN PNDIS_PACKET OriginalPacket,
    IN BOOLEAN IsLastFrame
    );


NDIS_STATUS NDIS_API
ArcFilterSendData(
    IN PARC_FILTER Filter,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet,
    IN ARC_SEND_RAW ArcSendRaw
    );

VOID NDIS_API
ArcFilterSendDataComplete(
    IN PNDIS_PACKET Packet,
    IN BOOLEAN IsLastFrame
    );

NDIS_STATUS NDIS_API
ArcFilterAllocateBufferList(
        PARC_FILTER Filter,
        UINT NumBuffers,
        UINT BufferSize,
        PARC_MEM_BUFFER_NODE *pHead
        );

#endif // _ARC_FILTER_DEFS_
