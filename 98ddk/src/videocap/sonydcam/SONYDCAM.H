/*++

Copyright (c) 1996 Microsoft Corporation

Module Name:

    sonydcam.h

Abstract:

    Header file for sonydcam.c 

Author:
    
    Shaun Pierce  25-May-96

Environment:

    Kernel mode only

Revision History:


--*/



//
// Function prototypes
//

NTSTATUS
DriverEntry(
    IN PVOID Context1,
    IN PVOID Context2
    );

NTSTATUS
SonyDCamHwUnInitialize(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

NTSTATUS
SonyDCamHwInitialize(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

NTSTATUS
SonyDCamSubmitIrpSynch(
    PSONYDCAM_EXTENSION DeviceExtension,
    PIRP Irp,
    PIRB Irb
    );

NTSTATUS
SonyDCamSynchCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PKEVENT Event
    );

ULONG
bitcount(
    IN ULONG b
    );


//
// External functions
//

extern 
BOOLEAN
SonyDCamQueryUnload(
    IN PVOID DeviceExtension
    );

extern 
VOID
SonyDCamReceivePacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

extern 
VOID
SonyDCamCancelPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

extern 
VOID
SonyDCamTimeoutHandler(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

extern VOID
SonyDCamCloseStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );
VOID
SonyDCamStopStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );



