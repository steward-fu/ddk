/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    usbcamd.h

Abstract:



Environment:

    Kernel & user mode

Revision History:

    5-10-96 : created

--*/

#ifndef   __USBCAMD_H__
#define   __USBCAMD_H__

#include "warn.h"
#include <wdm.h>
#include <usbdi.h>
#include <usbdlib.h>

#include <strmini.h>
#include <ksmedia.h>
#include "usbcamdi.h"


#define USBCAMD_NUM_ISO_PACKETS_PER_REQUEST  32

#define USBCAMD_MAX_REQUEST   2

//#define DEADMAN_TIMER

#define USBCAMD_EXTENSION_SIG 0x45564544    //"DEVE"
#define USBCAMD_CHANNEL_SIG 0x4348414e      //"CHAN"
#define USBCAMD_TRANSFER_SIG 0x5452414e     //"TRAN"
#define USBCAMD_READ_SIG 0x45425253         //"SRBE"

#define USBCAMD_RAW_FRAME_SIG 0x46574152    //"RAWF"

//
// A structure representing the instance information associated with
// this particular device.
//

typedef struct _USBCAMD_DEVICE_EXTENSION {

    ULONG Sig;
    
 
	// Control Queues for each data stream
    LIST_ENTRY               StreamControlSRBList;
    BOOL                     ProcessingControlSRB;
    KSPIN_LOCK               ControlSRBSpinLock;        // Multiprocessor safe access to AdapterSRBList

    // Device object we call when submitting Urbs
    PDEVICE_OBJECT StackDeviceObject;
    PDEVICE_OBJECT PhysicalDeviceObject;

    // configuration handle for the configuration the
    // device is currently in
    USBD_CONFIGURATION_HANDLE ConfigurationHandle;

    // ptr to the USB device descriptor
    // for this device
    PUSB_DEVICE_DESCRIPTOR DeviceDescriptor;

    // we support one interface
    PUSBD_INTERFACE_INFORMATION Interface;

    LONG TimeoutCount;

    KSEMAPHORE Semaphore;

    LONG SyncPipe;
    LONG DataPipe;

    DEVICE_POWER_STATE CurrentPowerState;

    struct _USBCAMD_CHANNEL_EXTENSION *ChannelExtension;

#ifdef DEADMAN_TIMER
    KDPC TimeoutDpc;
    KTIMER TimeoutTimer;
#endif
    ULONG OpenFRC;  //inc'ed every time a channel gets opened.

    USBCAMD_DEVICE_DATA DeviceData;

    ULONG Initialized;

    PUCHAR CameraDeviceContext[0];
    
} USBCAMD_DEVICE_EXTENSION, *PUSBCAMD_DEVICE_EXTENSION;

#define DEVICE_UNINITIALIZED  0x00000000
#define DEVICE_INIT_STARTED   0x00000001 
#define DEVICE_INIT_COMPLETED 0x00000002

//
// this structure defines the per request extension.  It defines any storage
// space that the mini driver may need in each request packet.
//

typedef struct _USBCAMD_READ_EXTENSION {
    ULONG Sig;
    LIST_ENTRY ListEntry;
    PIRP Irp;
    PVOID Srb;
    ULONG NumberOfPackets;
    PUCHAR RawFrameBuffer;
    ULONG RawFrameLength;
    ULONG RawFrameOffset;
    PUCHAR MinDriverExtension[0];
} USBCAMD_READ_EXTENSION, *PUSBCAMD_READ_EXTENSION;


typedef struct _USBCAMD_TRANSFER_EXTENSION {
    ULONG Sig;
    ULONG Pending;
    ULONG BufferLength;
    PUCHAR DataBuffer;
    PUCHAR SyncBuffer;
    PURB SyncUrb;
    PURB DataUrb;
    PIRP SyncIrp;
    PIRP DataIrp;
    PUCHAR WorkBuffer;
    KTIMER Timer;
    KDPC Dpc;
    PUSBCAMD_DEVICE_EXTENSION DeviceExtension;
    struct _USBCAMD_CHANNEL_EXTENSION *ChannelExtension;
} USBCAMD_TRANSFER_EXTENSION, *PUSBCAMD_TRANSFER_EXTENSION;


//
// values for channel extension Flags field
//

#define USBCAMD_STOP_STREAM             0x00000001
#define USBCAMD_TIMEOUT_STREAM_WAIT     0x00000004
#define USBCAMD_ENABLE_TIMEOUT_DPC      0x00000008

typedef struct _USBCAMD_CHANNEL_EXTENSION {
    ULONG Sig;
    PUSBCAMD_DEVICE_EXTENSION DeviceExtension;
    USBCAMD_TRANSFER_EXTENSION TransferExtension[USBCAMD_MAX_REQUEST];

	// Read SRB Queue mgmt.
    LIST_ENTRY PendingIoList;		
    KSPIN_LOCK PendingIoListSpin;

    PUSBCAMD_READ_EXTENSION CurrentRequest;
    KEVENT ResetEvent;
    KEVENT StopEvent;

    ULONG RawFrameLength;

    KSEMAPHORE PinSemaphore;

    LONG SyncPipe;
    LONG DataPipe;

    HANDLE MasterClockHandle;
    KS_FRAME_INFO               FrameInfo;          // PictureNumber, etc.


    PKS_VIDEOINFOHEADER         VideoInfoHeader;    // format (variable size!)
    KSSTATE                     KSState;            // Run, Stop, Pause
    
    PSTREAM_RECEIVE_PACKET CamReceiveCtrlPacket;
    PSTREAM_RECEIVE_PACKET CamReceiveDataPacket;

    //
    // the format number of the currently active stream
    //
    PVOID CurrentFormat;
    
    ULONG CurrentUSBFrame;

	//
	// Statistic of the frame information since last start stream
	//	
    //
    // inc'ed each time we advance to a new video frame.
    //
	LONGLONG       FrameCaptured;		// Number of actual frames cptured
    
    ULONGLONG		StartTime;		    // Use to calculate drop frame and actual frame rate.
	BOOLEAN			FirstFrame;			// when TRUE, get start time.

#if DBG
    //
    // DEBUG perf variables, these are reset each time the
    // channel is started.
    //
    //
    // Frames we have seen but had to toss because
    // no client request was available.
    //

    ULONG VideoFrameLostCount;

    // count of video data packets ignored due to error
    ULONG IgnorePacketCount;

    // inc'ed for each packet that completes with an error
    ULONG ErrorDataPacketCount;
    ULONG ErrorSyncPacketCount;

    // inc'ed for each packet not accessed by the HC
    ULONG SyncNotAccessedCount;
    ULONG DataNotAccessedCount;

    // DEBUG Flags
    BOOLEAN InCam;
    UCHAR Pad[3];

#endif
    // total packets received for the current video frame
    ULONG PacketCount;
    KTIMER TimeoutTimer;
    KDPC TimeoutDpc;

    ULONG Flags;       

    
    BOOLEAN StreamError;
    BOOLEAN ImageCaptureStarted;
    BOOLEAN ChannelPrepared;
    
} USBCAMD_CHANNEL_EXTENSION, *PUSBCAMD_CHANNEL_EXTENSION;

typedef struct _COMMAND_WORK_ITEM {
    PVOID DeviceContext;
    WORK_QUEUE_ITEM WorkItem;
    UCHAR Request;
    USHORT Value;
    USHORT Index;
    PVOID Buffer;
    OUT PULONG BufferLength;
    BOOLEAN GetData;
    PCOMMAND_COMPLETE_FUNCTION CommandComplete;
    PVOID CommandContext;
} COMMAND_WORK_ITEM, *PCOMMAND_WORK_ITEM;



//#define USBCAMD_SYNC_PIPE    0
//#define USBCAMD_DATA_PIPE    1

typedef struct _USBCAMD_WORK_ITEM  {
    WORK_QUEUE_ITEM WorkItem;
    PUSBCAMD_READ_EXTENSION Request;	
    ULONG PacketDataStart;		// not used
    ULONG NumberOfPackets;		// not used
    PUSBCAMD_CHANNEL_EXTENSION ChannelExtension;
} USBCAMD_WORK_ITEM, *PUSBCAMD_WORK_ITEM;

#define USBCAMD_TIMEOUT_INTERVAL    250

#define USBCAMD_GET_IRP(s) (s)->Irp
#define USBCAMD_GET_FRAME_CONTEXT(se)      (&se->MinDriverExtension[0]) 
#define USBCAMD_GET_DEVICE_CONTEXT(de)     ((PVOID)(&(de)->CameraDeviceContext[0])) 
#define USBCAMD_GET_DEVICE_EXTENSION(dc)    (PUSBCAMD_DEVICE_EXTENSION) (((PUCHAR)(dc)) - \
                                            sizeof(USBCAMD_DEVICE_EXTENSION))


#if DBG

#define MAX_TRACE 2
#define MIN_TRACE 1

#ifndef DEBUG_LOG
#define DEBUG_LOG
#endif

extern ULONG USBCAMD_DebugTraceLevel;

#define USBCAMD_KdPrint(_t_, _x_) \
    if (USBCAMD_DebugTraceLevel >= _t_) { \
        DbgPrint("'USBCAMD: "); \
        DbgPrint _x_ ;\
    }

#ifdef NTKERN
#define TRAP()  _asm {int 3}
#define TEST_TRAP() _asm {int 3}
#define TRAP_ERROR(e) if (!NT_SUCCESS(e)) { _asm {int 3} }
#else
#define TRAP()  DbgBreakPoint()
#define TEST_TRAP() DbgBreakPoint()
#define TRAP_ERROR(e) if (!NT_SUCCESS(e)) { DbgBreakPoint(); }
#endif

#define ASSERT_CHANNEL(c) ASSERT((c)->Sig == USBCAMD_CHANNEL_SIG)
#define ASSERT_TRANSFER(t) ASSERT((t)->Sig == USBCAMD_TRANSFER_SIG)
#define ASSERT_DEVICE(d) ASSERT((d)->Sig == USBCAMD_DEVICE_SIG)
#define ASSERT_READ(s) ASSERT((s)->Sig == USBCAMD_READ_SIG)

PVOID
USBCAMD_ExAllocatePool(
    IN POOL_TYPE PoolType,
    IN ULONG NumberOfBytes
    );

VOID
USBCAMD_ExFreePool(
    IN PVOID p
    );    

extern ULONG USBCAMD_HeapCount;

#else

#define MAX_TRACE 2
#define MIN_TRACE 1

#define USBCAMD_KdPrint(_t_, _x_)

#define TRAP()

#define TEST_TRAP()

#define TRAP_ERROR(e)

#define ASSERT_CHANNEL(c)
#define ASSERT_TRANSFER(t)
#define ASSERT_DEVICE(d) 
#define ASSERT_READ(s) 

#define USBCAMD_ExAllocatePool(x, y) ExAllocatePool(x, y) 
#define USBCAMD_ExFreePool(x) ExFreePool(x) 

#endif /* DBG */

#ifdef DEBUG_LOG

VOID
USBCAMD_Debug_LogEntry(
    IN CHAR *Name,
    IN ULONG Info1,
    IN ULONG Info2,
    IN ULONG Info3
    );

#define LOGENTRY(sig, info1, info2, info3) \
    USBCAMD_Debug_LogEntry(sig, (ULONG)info1, (ULONG)info2, (ULONG)info3)

VOID
USBCAMD_LogInit(
    VOID
    );

VOID
USBCAMD_LogFree(
    VOID
    );

#else

#define LOGENTRY(sig, info1, info2, info3)

#endif /* DEVUG_LOG */

#define USBCAMD_SERIALIZE(de)  { USBCAMD_KdPrint(MAX_TRACE, ("'***WAIT dev mutex %x\n", &(de)->Semaphore)); \
                                          KeWaitForSingleObject(&(de)->Semaphore, \
                                                                Executive,\
                                                                KernelMode, \
                                                                FALSE, \
                                                                NULL); \
                                            }                                                                 

#define USBCAMD_RELEASE(de)   { USBCAMD_KdPrint(MAX_TRACE, ("'***RELEASE dev mutex %x\n", &(de)->Semaphore));\
                                          KeReleaseSemaphore(&(de)->Semaphore,\
                                                             LOW_REALTIME_PRIORITY,\
                                                             1,\
                                                             FALSE);\
                                            }

#define ACQUIRE_PIN_SEMAPHORE(se)  { USBCAMD_KdPrint(MAX_TRACE, ("'***WAIT stream mutex %x\n", &(se)->PinSemaphore)); \
                                          KeWaitForSingleObject(&(se)->PinSemaphore, \
                                                                Executive,\
                                                                KernelMode, \
                                                                FALSE, \
                                                                NULL); \
                                            }                                                                 

#define RELEASE_PIN_SEMAPHORE(se)   { USBCAMD_KdPrint(MAX_TRACE, ("'***RELEASE stream mutex %x\n", &(se)->PinSemaphore));\
                                          KeReleaseSemaphore(&(se)->PinSemaphore,\
                                                             LOW_REALTIME_PRIORITY,\
                                                             1,\
                                                             FALSE);\
                                            }

NTSTATUS
USBCAMD_StartDevice(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension
    );

NTSTATUS
USBCAMD_RemoveDevice(
    IN PUSBCAMD_DEVICE_EXTENSION  DeviceExtension
    );

NTSTATUS
USBCAMD_StopDevice(
    IN PUSBCAMD_DEVICE_EXTENSION  DeviceExtension
    );

NTSTATUS
USBCAMD_CallUSBD(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PURB Urb
    );

NTSTATUS
USBCAMD_ConfigureDevice(
    IN  PUSBCAMD_DEVICE_EXTENSION DeviceExtension
    );

NTSTATUS
USBCAMD_SelectConfiguration(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    );

NTSTATUS
USBCAMD_IsoIrp_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

ULONG
USBCAMD_GetCurrentFrame(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension
    );

NTSTATUS
USBCAMD_InitializeIsoUrb(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN OUT PURB Urb,
    IN PUSBD_PIPE_INFORMATION PipeInformation,
    IN PUCHAR Buffer
    );

VOID
USBCAMD_SubmitIsoTransfer(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension,
    IN ULONG StartFrame,
    IN BOOLEAN Asap
    );

 NTSTATUS
 USBCAMD_TransferComplete(
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension
    );

PIRP
USBCAMD_BuildIoRequest(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension,
    IN PURB Urb
    );

NTSTATUS
USBCAMD_OpenChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PVOID Format
    );

NTSTATUS
USBCAMD_PrepareChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );

NTSTATUS
USBCAMD_ReadChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBCAMD_READ_EXTENSION ReadExtension
    );

NTSTATUS
USBCAMD_StartChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );

VOID
USBCAMD_CopyPacketToFrameBuffer(
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension,
    IN ULONG PacketSize,
    IN ULONG Index
    );

NTSTATUS
USBCAMD_StopChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );

NTSTATUS
USBCAMD_InitializeIsoTransfer(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBD_INTERFACE_INFORMATION InterfaceInformation,
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension
    );

NTSTATUS
USBCAMD_AbortPipe(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN USBD_PIPE_HANDLE PipeHandle
    );

NTSTATUS
USBCAMD_UnPrepareChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );

NTSTATUS
USBCAMD_FreeIsoTransfer(
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension
    );

NTSTATUS
USBCAMD_CloseChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );

VOID
USBCAMD_RecycleIrp(
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension,
    IN PIRP Irp,
    IN PURB Urb
    );

VOID
USBCAMD_ProcessStall(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );

VOID
USBCAMD_SubitIsoRequestDpc(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

NTSTATUS
USBCAMD_ResetPipes(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBD_INTERFACE_INFORMATION InterfaceInformation,
    IN BOOLEAN Abort
    );

NTSTATUS
USBCAMD_ResetChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );

VOID
USBCAMD_ChannelTimeoutDPC(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

VOID
USBCAMD_CompleteReadRequest(
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBCAMD_READ_EXTENSION ReadExtension
    );

VOID
USBCAMD_StartIsoStream(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN BOOLEAN Initialize
    );

VOID
USBCAMD_ProcessWorkItem(
    PVOID Context
    );

NTSTATUS
USBCAMD_CleanupChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );

VOID
USBCAMD_ReadIrpCancel(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
USBCAMD_ChangeMode(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PIRP Irp,
    IN OUT PULONG NewMode
    );

VOID 
USBCAMD_CompleteRead(
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBCAMD_READ_EXTENSION ReadExtension, 
    IN NTSTATUS NtStatus,
    IN ULONG BytesTransferred
    );   

PVOID
USBCAMD_GetFrameBufferFromSrb(
    IN PVOID Srb, 
    OUT PULONG MaxLength
    );    

VOID
USBCAMD_ResetWorkItem(
    PVOID Context
    );

VOID
USBCAMD_ProcessResetRequest(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );    

NTSTATUS
USBCAMD_OpenStream(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PVOID Format
    );    


NTSTATUS
USBCAMD_CloseStream(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );    

NTSTATUS
USBCAMD_SetDevicePowerState(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );    

VOID
USBCAMD_CommandWorkItem(
    PVOID Context
    );    

VOID VideoGetProperty(
    PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID VideoStreamGetConnectionProperty(
    PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID VideoStreamGetDroppedFramesProperty(
	PHW_STREAM_REQUEST_BLOCK pSrb
	);

ULONGLONG 
    GetSystemTime(
    VOID
    );
    
//
// prototypes for general queue management using a busy flag
//
BOOL
STREAMAPI 
AddToListIfBusy (
    IN PHW_STREAM_REQUEST_BLOCK pSrb,
    IN KSPIN_LOCK              *SpinLock,
    IN OUT BOOL                *BusyFlag,
    IN LIST_ENTRY              *ListHead
    );

BOOL
STREAMAPI 
RemoveFromListIfAvailable (
    IN OUT PHW_STREAM_REQUEST_BLOCK *pSrb,
    IN KSPIN_LOCK                   *SpinLock,
    IN OUT BOOL                     *BusyFlag,
    IN LIST_ENTRY                   *ListHead
    );

ULONGLONG GetStreamTime( 
	IN PHW_STREAM_REQUEST_BLOCK Srb, 
	IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
	);

BOOL 
STREAMAPI 
VideoSetFormat(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    );


#if DBG
VOID
USBCAMD_DebugStats(
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    );

PVOID 
USBCAMD_AllocateRawFrameBuffer(
    ULONG RawFrameLength
    );

VOID 
USBCAMD_FreeRawFrameBuffer(
    PVOID RawFrameBuffer
    );

VOID 
USBCAMD_CheckRawFrameBuffer(
    PVOID RawFrameBuffer
    );
#else

#define USBCAMD_AllocateRawFrameBuffer(l)  USBCAMD_ExAllocatePool(NonPagedPool, l)

#define USBCAMD_FreeRawFrameBuffer(p) ExFreePool(p)

#define USBCAMD_CheckRawFrameBuffer(p)

#endif

#endif /*  __USBCAMD_H__ */
