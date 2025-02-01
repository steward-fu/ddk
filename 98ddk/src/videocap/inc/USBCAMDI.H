/*++

Copyright (c) 1997      Microsoft Corporation

Module Name:

        USBCAMDI.H

Abstract:

   interface definition for .

Environment:

    Kernel & user mode

Revision History:

    6-6-97 : created by John Dunn

--*/


#ifndef   __USBCAMDI_H__
#define   __USBCAMDI_H__

typedef
VOID
(*PCOMMAND_COMPLETE_FUNCTION)(
    PVOID DeviceContext,
    PVOID CommandContext,
    NTSTATUS NtStatus
    );    

typedef
VOID  
(*PADAPTER_RECEIVE_PACKET_ROUTINE) (
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

typedef
NTSTATUS
(*PCAM_INITIALIZE_ROUTINE)(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    );

typedef
NTSTATUS
(*PCAM_CONFIGURE_ROUTINE)(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext,
    PUSBD_INTERFACE_INFORMATION Interface,
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    PLONG DataPipeIndex,
    PLONG SyncPipeIndex
    );    

typedef
NTSTATUS
(*PCAM_START_CAPTURE_ROUTINE)(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    );    

typedef
NTSTATUS
(*PCAM_ALLOCATE_BW_ROUTINE)(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext,
    PULONG RawFrameLength,
    PVOID Format
    );      

typedef
NTSTATUS
(*PCAM_FREE_BW_ROUTINE)(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    );      

typedef
NTSTATUS
(*PCAM_STOP_CAPTURE_ROUTINE)(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    );      

typedef
ULONG
(*PCAM_PROCESS_PACKET_ROUTINE) (
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext,
    PVOID CurrentFrameContext,
    PUSBD_ISO_PACKET_DESCRIPTOR SyncPacket,
    PVOID SyncBuffer,
    PUSBD_ISO_PACKET_DESCRIPTOR DataPacket,
    PVOID DataBuffer,
    PBOOLEAN FrameComplete,
    PBOOLEAN NextFrameIsStill
    );    

typedef
VOID
(*PCAM_NEW_FRAME_ROUTINE) (
    PVOID DeviceContext,
    PVOID FrameContext
    );

typedef
NTSTATUS
(*PCAM_PROCESS_RAW_FRAME_ROUTINE) (
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext,
    PVOID FrameContext,
    PVOID FrameBuffer,
    ULONG FrameLength,
    PVOID RawFrameBuffer,
    ULONG RawFrameLength,
    ULONG NumberOfPackets,
    PULONG BytesReturned
    );

typedef
NTSTATUS
(*PCAM_STOP_CAPTURE_ROUTINE)(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    );      

typedef
NTSTATUS
(*PCAM_STATE_ROUTINE) (
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    );      

typedef
VOID 
(*PSTREAM_RECEIVE_PACKET) (
    IN PVOID Srb,
    IN PVOID DeviceContext,
    IN PBOOLEAN Completed
    );

#ifdef DEBUG_LOG

VOID
USBCAMD_Debug_LogEntry(
    IN CHAR *Name,
    IN ULONG Info1,
    IN ULONG Info2,
    IN ULONG Info3
    );

#define ILOGENTRY(sig, info1, info2, info3) \
    USBCAMD_Debug_LogEntry(sig, (ULONG)info1, (ULONG)info2, (ULONG)info3)

#else

#define ILOGENTRY(sig, info1, info2, info3)

#endif /* DEBUG_LOG */


typedef struct _USBCAMD_DEVICE_DATA {
    ULONG Sig;
    PCAM_INITIALIZE_ROUTINE                 CamInitialize;
    PCAM_INITIALIZE_ROUTINE                 CamUnInitialize;
    PCAM_PROCESS_PACKET_ROUTINE             CamProcessUSBPacket;
    PCAM_NEW_FRAME_ROUTINE                  CamNewVideoFrame;
    PCAM_PROCESS_RAW_FRAME_ROUTINE          CamProcessRawVideoFrame;
    PCAM_START_CAPTURE_ROUTINE              CamStartCapture;
    PCAM_STOP_CAPTURE_ROUTINE               CamStopCapture;
    PCAM_CONFIGURE_ROUTINE                  CamConfigure;
    PCAM_STATE_ROUTINE                      CamSaveState;
    PCAM_STATE_ROUTINE                      CamRestoreState;
    PCAM_ALLOCATE_BW_ROUTINE                CamAllocateBandwidth;
    PCAM_FREE_BW_ROUTINE                    CamFreeBandwidth;
    
} USBCAMD_DEVICE_DATA, *PUSBCAMD_DEVICE_DATA;


//
// USBCAMD services
//

/* 

This function is called from the "cam" driver DriverEntry routine to register 
the driver with USBCAMD.  This effectively binds the two drivers together.

Context1, Context2 - these are passed to the "cam" drivers DriverEntry and 
    should simply be passed on 
DeviceContextSize - amount of memory needed for the "cam" driver's device 
    specific context. 
FrameContextSize - size of an optional per video frame structure passed to 
    NewVideoFrame and ProcessRawVideoFrame.

*/

ULONG
USBCAMD_DriverEntry(
    PVOID Context1,
    PVOID Context2,
    ULONG DeviceContextSize,
    ULONG FrameContextSize,
    PADAPTER_RECEIVE_PACKET_ROUTINE ReceivePacket
    );

/*
This function is called by the "cam" driver from the AdapterReceivePacket 
routine to allow USBCAMD to process an SRB.  

Srb - The srb passed to the "cam" driver's AdapterReceievPacket Routine.
DeviceData - filled in USBCAMD_DEVICE_DATA_STRUCTURE with entry points to the 
    "cam" driver for iso stream processing.
DeviceObject - this pointer will be filled in the device PDO, this is used by 
    the "cam" driver for calls to the USB stack through IoCallDriver. 
NeedsCompletion - if this flag is true USBCAMD will process the SRB and 
    complete it, otherwise USBCAMD will ignore the SRB and just return the 
    DeveiceConext.

Returns:
    Returns a pointer to the device specific context for this instance of the 
    camera "DeviceContext".

*/

PVOID
USBCAMD_AdapterReceivePacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb,
    IN PUSBCAMD_DEVICE_DATA DeviceData,
    IN PDEVICE_OBJECT *DeviceObject,
    IN BOOLEAN NeedsCompletion
    );    

/*
This function is available to the "cam" driver to send vendor commands on the 
control pipe.   This function may be called at IRQL >= PASSIVE_LEVEL.  If the 
function is called at IRQL > PASSIVE_LEVEL the command will be deferred until 
passive level, once completed the callers CommandComplete function is called 
with a pointer to the CommandContext.

DeviceContext - device specific context.
Request - value for the bRequest field for the vendor command.
Value - value for the wValue field of the vendor command.
Index - value for the wIndex field of the vendor conmmand.
Buffer - data buffer if the command has data, may be NULL.
BufferLength - pointer length of Buffer in bytes, may be NULL if buffer is NULL
GetData - flag indicating data direction, "GetData" indicates data is send 
    device-to-host.
CommandComplete - function called when command is completed.
CommandContext - context passed to CommandComplete function

Returns:
    Returns NTSTATUS code from command of STTAUS_PENDING if command is deferred.
    
*/

NTSTATUS
USBCAMD_ControlVendorCommand(
    IN PVOID DeviceContext,
    IN UCHAR Request,
    IN USHORT Value,
    IN USHORT Index,
    IN PVOID Buffer,
    IN OUT PULONG BufferLength,
    IN BOOLEAN GetData,
    IN PCOMMAND_COMPLETE_FUNCTION CommandComplete,
    IN PVOID CommandContext
    );    

/*
This function is called by the "cam" driver to select an alternate interface, 
The RequestInterface structure is filled in with the information from the 
SELECT_INTERFACE request on successful completion.  This function is 
typically called from the "cam" driver in response to a request to allocate 
or free bandwidth.

DeviceContext - device specific context.
RequestInterface - USBD_INTERFACE_INFORMATION structure for the appropriate 
    interface to select. The same rules apply to this structure as for the 
    SELECT_INTERFACE USBD request (see USBDI doc). 

Returns:
    Returns the status from the SELECT_INTERFACE request to USBD.

*/

NTSTATUS
USBCAMD_SelectAlternateInterface(
    IN PVOID DeviceContext,
    IN OUT PUSBD_INTERFACE_INFORMATION RequestInterface
    );  

/*
    Handy function to get a device instance specific registry key value
*/

NTSTATUS 
USBCAMD_GetRegistryKeyValue (
    IN HANDLE Handle,
    IN PWCHAR KeyNameString,
    IN ULONG KeyNameStringLength,
    IN PVOID Data,
    IN ULONG DataLength
    );    

//
// These services are not available yet
//

#if 0
VOID
USBCAMD_ResetCurrentRead(
    IN PVOID DeviceContext
    );

VOID
USBCAMD_QueueStillReadToVideoStream(
    IN PSRB Srb
    //??RawBufferSize
    );    

VOID
USBCAMD_StopStream(
    IN PVOID DeviceContext
    );    

VOID
USBCAMD_StartStream(
    IN PVOID DeviceContext
    );  
#endif    

#endif /*  __USBCAMDI_H__ */    
