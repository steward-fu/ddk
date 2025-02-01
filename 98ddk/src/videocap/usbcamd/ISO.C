/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

   iso.c

Abstract:

   Isochronous transfer code for usbcamd USB camera driver

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.


Revision History:


--*/

#include "usbcamd.h"

#define COPY_Y 0
#define COPY_U 1
#define COPY_V 2



#if DBG
// some global debug variables
ULONG USBCAMD_VideoFrameStop = 0;
#endif

NTSTATUS
USBCAMD_InitializeIsoTransfer(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBD_INTERFACE_INFORMATION InterfaceInformation,
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension
    )
/*++

Routine Description:

    Initializes an Iso transfer, an iso transfer consists of to parallel iso 
    requests, one on the sync pipe and one on the data pipe.

Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.

    ChannelExtension - extension specific to this video channel

    InterfaceInformation - pointer to USBD interface information structure 
        describing the currently active interface.

    TransferExtension - context information assocaited with this transfer set.        


Return Value:

    NT status code

--*/
{
    ULONG workspace;
    ULONG packetSize;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG offset = 0;
    PHYSICAL_ADDRESS highestAddress;

    ASSERT_CHANNEL(ChannelExtension);
       
    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_InitializeIsoTransfer\n"));

    //
    // allocate some contiguous memory for this request
    //

    TransferExtension->Sig = USBCAMD_TRANSFER_SIG;     
    TransferExtension->DeviceExtension = DeviceExtension;
    TransferExtension->ChannelExtension = 
                ChannelExtension;

    //
    // No pending transfers yet
    //
    TransferExtension->Pending = 0;                

    highestAddress.QuadPart = -1;

    packetSize = InterfaceInformation->Pipes[ChannelExtension->DataPipe].MaximumPacketSize;
    TransferExtension->BufferLength = 
        (packetSize*USBCAMD_NUM_ISO_PACKETS_PER_REQUEST) + USBCAMD_NUM_ISO_PACKETS_PER_REQUEST;

//     TransferExtension->SyncBuffer =       
//        TransferExtension->DataBuffer = 
//        MmAllocateContiguousMemory(TransferExtension->BufferLength, highestAddress);

    TransferExtension->SyncBuffer =       
        TransferExtension->DataBuffer =  
            USBCAMD_ExAllocatePool(NonPagedPool, 
                                   TransferExtension->BufferLength);       

    if (TransferExtension->SyncBuffer == NULL) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto USBCAMD_InitializeIsoTransfer_Done;
    }

    //
    // allow for one byte packets on the sync stream
    //
    
    TransferExtension->DataBuffer += USBCAMD_NUM_ISO_PACKETS_PER_REQUEST;   

    USBCAMD_KdPrint (MAX_TRACE, ("'Data Buffer = 0x%x\n", TransferExtension->DataBuffer));
    USBCAMD_KdPrint (MAX_TRACE, ("'Sync Buffer = 0x%x\n", TransferExtension->SyncBuffer));

    //
    // allocate working space
    //

    workspace = GET_ISO_URB_SIZE(USBCAMD_NUM_ISO_PACKETS_PER_REQUEST)*2;

    TransferExtension->WorkBuffer = USBCAMD_ExAllocatePool(NonPagedPool, workspace);
       
    if (TransferExtension->WorkBuffer) {

        TransferExtension->SyncUrb = 
            (PURB) TransferExtension->WorkBuffer; 
    
        TransferExtension->DataUrb = 
            (PURB) (TransferExtension->WorkBuffer + 
            GET_ISO_URB_SIZE(USBCAMD_NUM_ISO_PACKETS_PER_REQUEST));

        //
        // Build the sync request
        //

        if (ChannelExtension->SyncPipe == -1) {
            TransferExtension->SyncIrp = (PIRP)-1;
        } else {
            TransferExtension->SyncIrp = 
                USBCAMD_BuildIoRequest(DeviceExtension,
                                      TransferExtension,
                                      TransferExtension->SyncUrb);
                              
            USBCAMD_InitializeIsoUrb(DeviceExtension, 
                                    TransferExtension->SyncUrb, 
                                    &InterfaceInformation->Pipes[ChannelExtension->SyncPipe],
                                    TransferExtension->SyncBuffer);                 
        }                                    

        //
        // Build the data request
        //

        TransferExtension->DataIrp = 
            USBCAMD_BuildIoRequest(DeviceExtension,
                                  TransferExtension,
                                  TransferExtension->DataUrb);
                              
        USBCAMD_InitializeIsoUrb(DeviceExtension, 
                                TransferExtension->DataUrb, 
                                &InterfaceInformation->Pipes[ChannelExtension->DataPipe],
                                TransferExtension->DataBuffer);                                    

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        //MmFreeContiguousMemory(TransferExtension->SyncBuffer);
        USBCAMD_ExFreePool(TransferExtension->SyncBuffer);
    }

USBCAMD_InitializeIsoTransfer_Done:

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_InitializeIsoTransfer 0x%x\n", ntStatus));
    LOGENTRY("INit", ChannelExtension, TransferExtension, ntStatus); 

    return ntStatus;
}


NTSTATUS
USBCAMD_FreeIsoTransfer(
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension
    )
/*++

Routine Description:

    Opposite of USBCAMD_InitializeIsoTransfer, frees resources allocated for an 
    iso transfer.

Arguments:

    ChannelExtension - extension specific to this video channel

    TransferExtension - context information for this transfer (pair of iso 
        urbs).

Return Value:

    NT status code

--*/
{
    ASSERT_TRANSFER(TransferExtension);
    ASSERT_CHANNEL(ChannelExtension);
  
    USBCAMD_KdPrint (MAX_TRACE, ("'Free Iso Transfer\n"));
    //
    // first free the Irps
    //
    
    IoFreeIrp(TransferExtension->DataIrp);
    LOGENTRY("fIRP", TransferExtension->DataIrp, 0, 0);   

    if (TransferExtension->SyncIrp != (PIRP)-1) {
        IoFreeIrp(TransferExtension->SyncIrp);
        LOGENTRY("fIRP", TransferExtension->SyncIrp, 0, 0);   
        
    }
    
    //
    // now free memory, syn buffer pts to both sync and dtat buffers
    //

    //MmFreeContiguousMemory(TransferExtension->SyncBuffer);
    USBCAMD_ExFreePool(TransferExtension->SyncBuffer);

    USBCAMD_ExFreePool(TransferExtension->WorkBuffer);

    LOGENTRY("FRit", ChannelExtension, TransferExtension, 0); 

    return STATUS_SUCCESS;
}


VOID
USBCAMD_SubmitIsoTransfer(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension,
    IN ULONG StartFrame,
    IN BOOLEAN Asap
    )
/*++

Routine Description:

Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.

    TransferExtension - context information for this transfer (pair of iso 
        urbs).

    StartFrame - usb frame number to begin transmiting this pair of iso 
        requests.

    Asap - if false transfers are started on StartFrame otherwise they are 
        scheduled to start after the current transfer queued for the endpoint.            

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
    PUSBCAMD_CHANNEL_EXTENSION channelExtension;

    ASSERT_TRANSFER(TransferExtension);
    channelExtension = TransferExtension->ChannelExtension;
    ASSERT_CHANNEL(channelExtension);
    

//    TransferExtension->DataUrb->UrbIsochronousTransfer.StartFrame =
//        TransferExtension->SyncUrb->UrbIsochronousTransfer.StartFrame = 
//        StartFrame;

    RtlZeroMemory(TransferExtension->SyncBuffer, 
        USBCAMD_NUM_ISO_PACKETS_PER_REQUEST);

    if (Asap) {
        // set the asap flag
        TransferExtension->DataUrb->UrbIsochronousTransfer.TransferFlags |= 
            USBD_START_ISO_TRANSFER_ASAP;  
        if (channelExtension->SyncPipe != -1) {           
            TransferExtension->SyncUrb->UrbIsochronousTransfer.TransferFlags |= 
                USBD_START_ISO_TRANSFER_ASAP;              
        }                
    } else {
        // clear asap flag
        TransferExtension->DataUrb->UrbIsochronousTransfer.TransferFlags &= 
            (~USBD_START_ISO_TRANSFER_ASAP);  
        TransferExtension->DataUrb->UrbIsochronousTransfer.StartFrame = StartFrame;
        
        if (channelExtension->SyncPipe != -1) {    
            TransferExtension->SyncUrb->UrbIsochronousTransfer.TransferFlags &= 
                (~USBD_START_ISO_TRANSFER_ASAP);              
            // set the start frame
            TransferExtension->SyncUrb->UrbIsochronousTransfer.StartFrame = StartFrame;
        }            
    }

    {
    PIRP irp;       

    // we always submit both,
    // we'll let the completion routine handle the errors

    if (channelExtension->SyncPipe == -1) {
        TransferExtension->Pending = 1;
        irp = TransferExtension->DataIrp;
    } else {
        TransferExtension->Pending = 2;
        irp = TransferExtension->SyncIrp;            
    }
    
    for(;;) {

        //
        // Note we have one more irp pending for this transfer
        //

        if (DeviceExtension->Initialized) {
            ntStatus = IoCallDriver(DeviceExtension->StackDeviceObject, 
                                    irp);
        } else {
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
                            
        LOGENTRY("sIrp", irp, TransferExtension, ntStatus); 

        //
        // handle errors returned from submitting requests
        //

        if (ntStatus != STATUS_PENDING) {
            // stream error if we get an immediate error
            channelExtension->StreamError = TRUE;
            TEST_TRAP();
            LOGENTRY("eIrp", irp, TransferExtension, ntStatus); 
            // we have an error on the submission set the stream error flag
            // and exit.
            //
            // Note the completion routine will handle cleanup
        } 

        if (irp == TransferExtension->DataIrp) {
            break;
        }

        irp = TransferExtension->DataIrp;
    }
    }

    LOGENTRY("SITc", 0, TransferExtension, ntStatus); 
}


NTSTATUS
USBCAMD_IsoIrp_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
	IN PVOID Context
    )
/*++

Routine Description:

    This routine is called when the port driver completes an IRP, if this is
    the second irp of a transfer pair then the TransferComplete routine is 
    called to process the urbs associated with both irps in the transfer.  

Arguments:

    DeviceObject - Pointer to the device object for the class device.

    Irp - Irp completed.

	Context - Driver defined context, points to a transfer extension structure 
    	for a pair of parallel iso requests.

Return Value:

    The function value is the final status from the operation.

--*/
{
    PUSBCAMD_TRANSFER_EXTENSION transferExtension;
    PUSBCAMD_CHANNEL_EXTENSION channelExtension;
    NTSTATUS ntStatus;

    LOGENTRY("ISIc", Irp, 0, 0); 
    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_IsoIrp_Complete = 0x%x\n", Irp));
   
    transferExtension = Context;
    channelExtension = transferExtension->ChannelExtension;

    ASSERT_TRANSFER(transferExtension);
    ASSERT_CHANNEL(channelExtension);

    if (channelExtension->ImageCaptureStarted == FALSE) {
        //
        // we've hit the first completion routine before 
        // the stream is started.  
        // This means the system is really screwed up -- or
        // is running with excessive debug spew.
        // We raise a stream error here so that we won't try 
        // to re-submit.
        //
        TEST_TRAP();
        channelExtension->StreamError = TRUE;
    }
    
    //
    // one less irp pending
    //
    transferExtension->Pending--;
    LOGENTRY("Pen-", transferExtension, transferExtension->Pending, 0); 

    if ((channelExtension->Flags & USBCAMD_STOP_STREAM) ||
        channelExtension->StreamError) {
        
        //
        // We have a stream error or a stop request.
        //

        LOGENTRY("chkE", 
                  0, 
                  channelExtension->Flags, 
                  channelExtension->StreamError); 

        //
        // when the pending count for the transfer drops to zero 
        // our timer DPC will know that it is safe to 
        // complete the stop request or complete reads with
        // errors.
        //            
        // free it later
        //
        
        ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
        
    } else {
        //
        // No stop flag process this channel, process the irp
        //

        LOGENTRY("Icmp", Irp, transferExtension, channelExtension); 
        
        ntStatus = Irp->IoStatus.Status;

        USBCAMD_KdPrint (MAX_TRACE, 
            ("'USBCAMD_IsoIrp_Complete irp status = 0x%x\n", ntStatus));

        LOGENTRY("chkI", Irp, transferExtension->Pending, 0); 
        
        if (transferExtension->Pending == 0) {

            //
            // all irps completed for transfer
            //
        
            USBCAMD_KdPrint (MAX_TRACE, ("'pending Irps Completed for transfer\n"));

            if (USBD_ERROR(transferExtension->DataUrb->UrbHeader.Status) ||
                (channelExtension->SyncPipe != -1 &&
                 USBD_ERROR(transferExtension->SyncUrb->UrbHeader.Status))) {
                
                channelExtension->StreamError = TRUE;
            }                

            // 
            // check for certain error conditions, if they occur set the stream
            // error flag and stop the stream
            //
            if ((transferExtension->DataUrb->UrbIsochronousTransfer.ErrorCount == 
                 transferExtension->DataUrb->UrbIsochronousTransfer.NumberOfPackets) ||
                (channelExtension->SyncPipe != -1 &&
                 (transferExtension->SyncUrb->UrbIsochronousTransfer.ErrorCount == 
                 transferExtension->SyncUrb->UrbIsochronousTransfer.NumberOfPackets)))  {                 
                
                channelExtension->StreamError = TRUE;
                
                LOGENTRY("Serr", 
                          Irp, 
                          transferExtension->SyncUrb->UrbIsochronousTransfer.ErrorCount, 
                          transferExtension->DataUrb->UrbIsochronousTransfer.ErrorCount); 
                
                //
                // one less transfer for this channel
                //

                LOGENTRY("Tr2-", transferExtension, 0, 0); 

            } else {
            
                //
                // Call the comnpletion handler for this transfer
                //

                LOGENTRY("chkC", Irp, 0, 0); 
                USBCAMD_TransferComplete(transferExtension);
            }                
            
        }        

        ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
    }        

    return ntStatus;        
}                    


PIRP
USBCAMD_BuildIoRequest(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension,
    IN PURB Urb
    )
/*++

Routine Description:

    Allocate an Irp and attach a urb to it.
    
Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.

    TransferExtension - context information for this transfer (pair of iso 
        urbs).

    Urb - Urb to attach to this irp.

Return Value:

    Allocated irp or NULL.

--*/    
{
    CCHAR stackSize;
    PIRP irp;
    PIO_STACK_LOCATION nextStack;

    stackSize = (CCHAR)(DeviceExtension->StackDeviceObject->StackSize + 1);

    irp = IoAllocateIrp(stackSize,
                        FALSE);
    LOGENTRY("aIRP", irp, 0, 0);                             
            
    IoInitializeIrp(irp,
                    IoSizeOfIrp(stackSize),
   	                stackSize);

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);

    nextStack->Parameters.Others.Argument1 = Urb;
    nextStack->Parameters.DeviceIoControl.IoControlCode = 
        IOCTL_INTERNAL_USB_SUBMIT_URB;                    
    nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

#pragma warning(disable:4127)
    IoSetCompletionRoutine(irp,
            USBCAMD_IsoIrp_Complete,
            TransferExtension,
            TRUE,
            TRUE,
            TRUE);
#pragma warning(default:4127)

    LOGENTRY("BLio", irp, TransferExtension, 0); 

    return irp;
}


NTSTATUS
USBCAMD_InitializeIsoUrb(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN OUT PURB Urb,
    IN PUSBD_PIPE_INFORMATION PipeInformation,
    IN PUCHAR Buffer
    )
/*++

Routine Description:

    Packetizes a buffer and initializes an iso urb request based on 
        charateristics of the input USB pipe.

Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.

    Urb - iso urb to initialize.

    PipeInformation - Usbd pipe information for the pipe this urb will be 
        submitted to.

    Buffer - Data buffer to packetize for this request

Return Value:

    NT status code.

--*/
{
    ULONG packetSize = PipeInformation->MaximumPacketSize;
    ULONG i;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_InitializeIsoUrb = 0x%x packetSize = 0x%x\n",
        Urb, packetSize, PipeInformation->PipeHandle));

    USBCAMD_KdPrint (MAX_TRACE, ("'handle = 0x%x\n", PipeInformation->PipeHandle));        
        
    RtlZeroMemory(Urb, GET_ISO_URB_SIZE(USBCAMD_NUM_ISO_PACKETS_PER_REQUEST));
    
    Urb->UrbIsochronousTransfer.Hdr.Length = 
                GET_ISO_URB_SIZE(USBCAMD_NUM_ISO_PACKETS_PER_REQUEST);
    Urb->UrbIsochronousTransfer.Hdr.Function = 
                URB_FUNCTION_ISOCH_TRANSFER;
    Urb->UrbIsochronousTransfer.PipeHandle = 
                PipeInformation->PipeHandle;
    Urb->UrbIsochronousTransfer.TransferFlags = 
                USBD_START_ISO_TRANSFER_ASAP | USBD_TRANSFER_DIRECTION_IN;
                
    Urb->UrbIsochronousTransfer.NumberOfPackets = USBCAMD_NUM_ISO_PACKETS_PER_REQUEST;
	Urb->UrbIsochronousTransfer.UrbLink = NULL;

    for (i=0; i< Urb->UrbIsochronousTransfer.NumberOfPackets; i++) {
        Urb->UrbIsochronousTransfer.IsoPacket[i].Offset
                    = i * packetSize;
    }

    Urb->UrbIsochronousTransfer.TransferBuffer = Buffer;
        
    Urb->UrbIsochronousTransfer.TransferBufferMDL = NULL;
    Urb->UrbIsochronousTransfer.TransferBufferLength = 
        Urb->UrbIsochronousTransfer.NumberOfPackets * packetSize;     

    USBCAMD_KdPrint (MAX_TRACE, ("'Init Iso Urb Length = 0x%x buf = 0x%x start = 0x%x\n", 
        Urb->UrbIsochronousTransfer.TransferBufferLength,
        Urb->UrbIsochronousTransfer.TransferBuffer,
        Urb->UrbIsochronousTransfer.StartFrame));     

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_InitializeIsoUrb\n"));        

    LOGENTRY("iURB", 
             Urb->UrbIsochronousTransfer.TransferBufferLength, 
             Urb->UrbIsochronousTransfer.TransferBuffer,
             Urb->UrbIsochronousTransfer.StartFrame);

    return STATUS_SUCCESS;        
}


ULONG
USBCAMD_GetCurrentFrame(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension
    )
/*++

Routine Description:

    Get the current USB frame number.
    BUGBUG
    We only need this function until we have any api to synchronize two iso
    pipes.

Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.

Return Value:

	Current Frame Number

--*/
{
    NTSTATUS ntStatus;
	PURB urb;
	ULONG currentUSBFrame = 0;

	urb = USBCAMD_ExAllocatePool(NonPagedPool, 
						 sizeof(struct _URB_GET_CURRENT_FRAME_NUMBER));
						 
	if (urb) {

		urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_GET_CURRENT_FRAME_NUMBER);
		urb->UrbHeader.Function = URB_FUNCTION_GET_CURRENT_FRAME_NUMBER;

		ntStatus = USBCAMD_CallUSBD(DeviceExtension, urb);

		USBCAMD_KdPrint (MAX_TRACE, ("'Current Frame = 0x%x\n", 
			urb->UrbGetCurrentFrameNumber.FrameNumber));

		if (NT_SUCCESS(ntStatus) && USBD_SUCCESS(URB_STATUS(urb))) {
			currentUSBFrame = urb->UrbGetCurrentFrameNumber.FrameNumber;
	    }

		USBCAMD_ExFreePool(urb);
		
	} else {
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;		
    }		

	USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_GetCurrentFrame status = 0x%x current frame = 0x%x\n", 
		ntStatus, currentUSBFrame));	

    LOGENTRY("cFRM", currentUSBFrame, ntStatus, 0);

    TRAP_ERROR(ntStatus);
    
	return currentUSBFrame;			
}	


VOID
USBCAMD_RecycleIrp(
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension,
    IN PIRP Irp,
    IN PURB Urb
    )
/*++

Routine Description:

    Get the current USB frame number.

Arguments:

    TransferExtension - context information for this transfer (pair of iso 
        urbs).

    Irp - Irp to recycle.

    Urb - Urb associated with this irp.

Return Value:

	None.

--*/    
{
    PIO_STACK_LOCATION nextStack;

    Irp->Cancel = FALSE;        
    nextStack = IoGetNextIrpStackLocation(Irp);
    ASSERT(nextStack != NULL);

    nextStack->Parameters.Others.Argument1 = Urb;
    nextStack->Parameters.DeviceIoControl.IoControlCode = 
        IOCTL_INTERNAL_USB_SUBMIT_URB;                    
    nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

#pragma warning(disable:4127)
    IoSetCompletionRoutine(Irp,
            USBCAMD_IsoIrp_Complete,
            TransferExtension,
            TRUE,
            TRUE,
            TRUE);
#pragma warning(default:4127)            

    LOGENTRY("Rirp", Irp, Urb, 0);                
}   


NTSTATUS
USBCAMD_TransferComplete(
    IN PUSBCAMD_TRANSFER_EXTENSION TransferExtension
    )
/*++

Routine Description:

    Called when the both the data and sync request are complete for a transfer
    this is the guts of the stream processing code.

Arguments:

    TransferExtension - context information for this transfer (pair of iso 
        urbs).

Return Value:

    NT status code.

--*/    
{
    PUSBCAMD_DEVICE_EXTENSION deviceExtension;
    PUSBCAMD_CHANNEL_EXTENSION channelExtension;
    ULONG numPackets, i;
    PLIST_ENTRY listEntry;
    ULONG packetSize;
    LARGE_INTEGER dueTime;
    BOOLEAN inQueue, newFrame;
    ULONG receiveLength = 0;
    PURB syncUrb, dataUrb;
    BOOLEAN nextFrameIsStill;

    ASSERT_TRANSFER(TransferExtension);
    deviceExtension = TransferExtension->DeviceExtension;
    
    channelExtension = TransferExtension->ChannelExtension;
    ASSERT_CHANNEL(channelExtension);
	
    
    packetSize = deviceExtension->Interface->Pipes[channelExtension->DataPipe].MaximumPacketSize;

    // 
    // walk through the buffer extracting video frames
    //
    numPackets = 
        TransferExtension->DataUrb->UrbIsochronousTransfer.NumberOfPackets;

#if DBG
    if (TransferExtension->SyncUrb && channelExtension->SyncPipe != -1) {
        ASSERT(TransferExtension->SyncUrb->UrbIsochronousTransfer.NumberOfPackets ==
                TransferExtension->DataUrb->UrbIsochronousTransfer.NumberOfPackets);        
    }  
#endif    

    LOGENTRY("xCPL", TransferExtension, 0, numPackets);
    USBCAMD_KdPrint (MAX_TRACE, ("'Transfer req. completed \n"));

    for (i=0; i<numPackets; i++) {               

        syncUrb = TransferExtension->SyncUrb;
        dataUrb = TransferExtension->DataUrb;

#if DBG   
        //
        // DEBUG stats
        //
        // keep a count of the number of packets processed for this
        // vid frame.
        //
        if (USBCAMD_VideoFrameStop &&
            channelExtension->FrameCaptured == USBCAMD_VideoFrameStop) {
            //
            // This will cause us to stop when we begin processing 
            // video frame number x where x=USBCAMD_VideoFrameStop
            //
            
            TRAP();
        }           

        if (syncUrb && USBD_ERROR(syncUrb->UrbIsochronousTransfer.IsoPacket[i].Status)) {
            channelExtension->ErrorSyncPacketCount++;    
        }            

        if (USBD_ERROR(dataUrb->UrbIsochronousTransfer.IsoPacket[i].Status)) {
            channelExtension->ErrorDataPacketCount++;    
        }            

        if (syncUrb && 
            (syncUrb->UrbIsochronousTransfer.IsoPacket[i].Status & 0x0FFFFFFF)
              == (USBD_STATUS_NOT_ACCESSED & 0x0FFFFFFF)) {   
            channelExtension->SyncNotAccessedCount++;    
        }            

        if ((dataUrb->UrbIsochronousTransfer.IsoPacket[i].Status & 0x0FFFFFFF)
            == (USBD_STATUS_NOT_ACCESSED & 0x0FFFFFFF)) {   
            channelExtension->DataNotAccessedCount++;    
        }       

        if (syncUrb) {
            LOGENTRY("sync", 
                     TransferExtension->SyncBuffer[i],            
                     syncUrb->UrbIsochronousTransfer.IsoPacket[i].Length,
                     syncUrb->UrbIsochronousTransfer.IsoPacket[i].Status);
        }                     

        LOGENTRY("data", 
                 0,            
                 dataUrb->UrbIsochronousTransfer.IsoPacket[i].Length,
                 dataUrb->UrbIsochronousTransfer.IsoPacket[i].Status);
#endif    

        // process the packet
        newFrame = FALSE;
        
        receiveLength =  (*deviceExtension->DeviceData.CamProcessUSBPacket)(
                deviceExtension->StackDeviceObject,
                USBCAMD_GET_DEVICE_CONTEXT(deviceExtension),
                channelExtension->CurrentRequest ?                                     
                    USBCAMD_GET_FRAME_CONTEXT(channelExtension->CurrentRequest) : NULL,
                &syncUrb->UrbIsochronousTransfer.IsoPacket[i],
                TransferExtension->SyncBuffer+i,
                &dataUrb->UrbIsochronousTransfer.IsoPacket[i],
                TransferExtension->DataBuffer + 
                   TransferExtension->DataUrb->UrbIsochronousTransfer.IsoPacket[i].Offset,
                &newFrame,
                &nextFrameIsStill);                    

        if (newFrame) {
            
            //                       
            // start of new video frame
            //
            channelExtension->FrameCaptured++;  


            if (channelExtension->CurrentRequest) {

                PIRP irp = USBCAMD_GET_IRP(channelExtension->CurrentRequest);
                
                //
                // if we have an Irp for the current video frame complete it.
                //

                USBCAMD_CompleteReadRequest( channelExtension, 
                                             channelExtension->CurrentRequest );
            }

            //
            // Select the next frame
            //

            listEntry = 
                ExInterlockedRemoveHeadList( &(channelExtension->PendingIoList),
                                             &channelExtension->PendingIoListSpin);         

            LOGENTRY("lVID",
                      listEntry,
                      0,
                      0);
            
            if (listEntry != NULL) {
                PIRP irp;

                PUCHAR dst, end;
                
                channelExtension->CurrentRequest = 
                    (PUSBCAMD_READ_EXTENSION) CONTAINING_RECORD(listEntry, 
                                             USBCAMD_READ_EXTENSION, 
                                             ListEntry);                        

                irp = USBCAMD_GET_IRP(channelExtension->CurrentRequest);                                             

                //
                // use the data in the packet
                //

                channelExtension->CurrentRequest->RawFrameOffset = 0;
                channelExtension->CurrentRequest->NumberOfPackets = 0;

                (*deviceExtension->DeviceData.CamNewVideoFrame)
                    (USBCAMD_GET_DEVICE_CONTEXT(deviceExtension),
                     USBCAMD_GET_FRAME_CONTEXT(channelExtension->CurrentRequest));

                ASSERT_READ(channelExtension->CurrentRequest);                            
                
                if (receiveLength)  {

                    channelExtension->CurrentRequest->NumberOfPackets = 1;                    

                    dst = channelExtension->CurrentRequest->RawFrameBuffer +
                               channelExtension->CurrentRequest->RawFrameOffset +
                               receiveLength;
                    end = channelExtension->CurrentRequest->RawFrameBuffer + 
                               channelExtension->CurrentRequest->RawFrameLength;
                               
                    USBCAMD_KdPrint (MAX_TRACE, ("'Raw buff = 0x%x\n", 
                        channelExtension->CurrentRequest->RawFrameBuffer));
                    USBCAMD_KdPrint (MAX_TRACE, ("'Raw Offset = 0x%x rec length = 0x%x\n", 
                        channelExtension->CurrentRequest->RawFrameOffset,
                        receiveLength));

                    if (dst <= end) {   
                        RtlCopyMemory(channelExtension->CurrentRequest->RawFrameBuffer +
                                        channelExtension->CurrentRequest->RawFrameOffset,
                                      TransferExtension->DataBuffer + 
                                        TransferExtension->DataUrb->UrbIsochronousTransfer.IsoPacket[i].Offset,
                                      receiveLength);
                                  
                        channelExtension->CurrentRequest->RawFrameOffset += 
                            receiveLength;
                    }                            
                        
                }

                ASSERT_READ(channelExtension->CurrentRequest);
                USBCAMD_CheckRawFrameBuffer(channelExtension->CurrentRequest->RawFrameBuffer);    
                
            } else {
                //
                // No irps are queued we'll have to miss
                // this frame
                //
                
                channelExtension->CurrentRequest = NULL;
                //
                // No buffer was available when we should have captured one

                // Increment the counter which keeps track of
                // actual dropped frames

#if DBG
                channelExtension->VideoFrameLostCount++;
#endif
            }

            LOGENTRY("nVID", channelExtension->CurrentRequest, 0, 0);

            USBCAMD_KdPrint (MAX_TRACE, ("'Get next frame request, Irp = 0x%x\n", 
                channelExtension->CurrentRequest));
            
        } else {   

            PUCHAR dst, end;
            //
            // video data is for current frame
            //
            
            if (receiveLength && channelExtension->CurrentRequest != NULL) {

                //
                // No errors, if we have a video frame copy the data
                //

                //
                // check for buffer overrun
                // if the camera is using two pipes it is possible we
                // will miss the sync info on and keep trying to 
                // recieve data frame data into the raw buffer, if this
                // happens we just trow the extra data away.
                //
                
                dst = channelExtension->CurrentRequest->RawFrameBuffer +
                           channelExtension->CurrentRequest->RawFrameOffset +
                           receiveLength;
                end = channelExtension->CurrentRequest->RawFrameBuffer + 
                           channelExtension->CurrentRequest->RawFrameLength;
                           
                USBCAMD_KdPrint (MAX_TRACE, ("'Raw buff = 0x%x\n", 
                    channelExtension->CurrentRequest->RawFrameBuffer));
                USBCAMD_KdPrint (MAX_TRACE, ("'Raw Offset = 0x%x rec length = 0x%x\n", 
                    channelExtension->CurrentRequest->RawFrameOffset,
                    receiveLength));

                if (dst <= end) {   
                    channelExtension->CurrentRequest->NumberOfPackets++;    
                    RtlCopyMemory(channelExtension->CurrentRequest->RawFrameBuffer +
                                      channelExtension->CurrentRequest->RawFrameOffset,
                                  TransferExtension->DataBuffer + 
                                      TransferExtension->DataUrb->UrbIsochronousTransfer.IsoPacket[i].Offset,
                                  receiveLength);
                                  
                    channelExtension->CurrentRequest->RawFrameOffset += 
                            receiveLength;
                }                             
                        
            }
        }  /* process packet */
        
    } /* end for loop*/

    //
    // Schedule a timer DPC routine to re-submit this request,
    // we have taken up all our time by walking through the 
    // the packet list.
    //

    KeInitializeTimer(&TransferExtension->Timer);
    KeInitializeDpc(&TransferExtension->Dpc,
                    USBCAMD_SubitIsoRequestDpc,
                    TransferExtension);

    //
    // schedule re-submit 2ms in the future
    //
    
    dueTime.QuadPart = -10000 * 2;

    inQueue = KeSetTimer(&TransferExtension->Timer,
                         dueTime,
                         &TransferExtension->Dpc);    

    ASSERT(inQueue == FALSE);      
	

    return STATUS_SUCCESS;
}


VOID
USBCAMD_SubitIsoRequestDpc(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL. 

    We re-submit a transfer request from this routine, if any errors occur 
    trying to start it the ChannelTimeoutDPC will clean up for us.

Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - 

    SystemArgument1 - not used.
    
    SystemArgument2 - not used.

Return Value:

    None.

--*/
{
    PUSBCAMD_DEVICE_EXTENSION deviceExtension;
    PUSBCAMD_CHANNEL_EXTENSION channelExtension;
    PUSBCAMD_TRANSFER_EXTENSION transferExtension; 

    USBCAMD_KdPrint (MAX_TRACE, ("'enter UHCD_SubitIsoRequestDpc\n")); 

    LOGENTRY("sDPC", DeferredContext, 0, 0);

    transferExtension = DeferredContext;
    deviceExtension = transferExtension->DeviceExtension;
    channelExtension = transferExtension->ChannelExtension;

    ASSERT_TRANSFER(transferExtension);
    ASSERT_CHANNEL(channelExtension);
    
    //
    // we should be streaming
    //
    
    ASSERT(channelExtension->ImageCaptureStarted == TRUE);

    //
    // re-submit this request
    //
    
    USBCAMD_KdPrint (MAX_TRACE, ("'re-submit this request\n")); 

    //
    // Build the Sync request
    //

    if (channelExtension->SyncPipe != -1) {
        USBCAMD_RecycleIrp(transferExtension, 
                           transferExtension->SyncIrp,
                           transferExtension->SyncUrb);
    
        USBCAMD_InitializeIsoUrb(deviceExtension, 
                                 transferExtension->SyncUrb, 
                                 &deviceExtension->Interface->Pipes[channelExtension->SyncPipe],
                                 transferExtension->SyncBuffer);           
    }                                         

    RtlZeroMemory(transferExtension->SyncBuffer,  
        USBCAMD_NUM_ISO_PACKETS_PER_REQUEST);                                

    //
    // Build the data request
    //

    USBCAMD_RecycleIrp(transferExtension, 
                      transferExtension->DataIrp,
                      transferExtension->DataUrb);

                              
    USBCAMD_InitializeIsoUrb(deviceExtension, 
                            transferExtension->DataUrb, 
                            &deviceExtension->Interface->Pipes[channelExtension->DataPipe],
                            transferExtension->DataBuffer);           
    
    USBCAMD_SubmitIsoTransfer(deviceExtension,
                              transferExtension,
                              0,
                              TRUE);

    LOGENTRY("xDPC", DeferredContext, 0, 0);                                            

}


#if DBG
VOID
USBCAMD_DebugStats(
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension    
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
    USBCAMD_KdPrint (MIN_TRACE, ("'**VideoFrameLostCount %d\n", 
                        ChannelExtension->VideoFrameLostCount)); 
	USBCAMD_KdPrint (MIN_TRACE, ("'**VideoFrameCount %d\n", 
                        ChannelExtension->FrameCaptured));	
    USBCAMD_KdPrint (MIN_TRACE, ("'**ErrorSyncPacketCount %d\n",
                        ChannelExtension->ErrorSyncPacketCount));                         
    USBCAMD_KdPrint (MIN_TRACE, ("'**ErrorDataPacketCount %d\n", 
                        ChannelExtension->ErrorDataPacketCount));                         
    USBCAMD_KdPrint (MIN_TRACE, ("'**IgnorePacketCount %d\n", 
                        ChannelExtension->IgnorePacketCount));                              
    USBCAMD_KdPrint (MIN_TRACE, ("'**Sync Not Accessed Count %d\n", 
                        ChannelExtension->SyncNotAccessedCount));                                   
    USBCAMD_KdPrint (MIN_TRACE, ("'**Data Not Accessed Count %d\n", 
                        ChannelExtension->DataNotAccessedCount));                                
}
#endif /* DBG */


VOID
USBCAMD_CompleteReadRequest(
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBCAMD_READ_EXTENSION ReadExtension
    )
/*++

Routine Description:

    This routine completes the read for the camera

Arguments:

Return Value:

--*/    
{
    PUSBCAMD_WORK_ITEM usbWorkItem;


    //
    // Queue a work item for this Irp
    //

    usbWorkItem = USBCAMD_ExAllocatePool(NonPagedPool, sizeof(*usbWorkItem));
    if (usbWorkItem) {
        LOGENTRY("WKin", ReadExtension, usbWorkItem, 0);
        ExInitializeWorkItem(&usbWorkItem->WorkItem,
                             USBCAMD_ProcessWorkItem,
                             usbWorkItem);

       usbWorkItem->Request = ReadExtension;
        
        usbWorkItem->ChannelExtension = ChannelExtension;
        ExQueueWorkItem(&usbWorkItem->WorkItem,
                        CriticalWorkQueue);

    } else {
        //
        // no memory for work item,
        // complete the read with an error
        //
        LOGENTRY("WKer", ReadExtension, 0, 0);
        USBCAMD_CompleteRead(ChannelExtension,
                             ReadExtension, 
                             STATUS_INSUFFICIENT_RESOURCES, 
                             0);
    }
}

//
// code to handle packet processing outside the DPC routine
//

VOID
USBCAMD_ProcessWorkItem(
    PVOID Context
    )
/*++

Routine Description:

    Call the mini driver to convert a raw packet to the proper format.

Arguments:

Return Value:

    None.

--*/
{
    PUSBCAMD_WORK_ITEM usbWorkItem = Context;
    PVOID frameBuffer;
    ULONG maxLength;
    PUSBCAMD_CHANNEL_EXTENSION channelExtension;    
    PUSBCAMD_READ_EXTENSION readExtension;
    PUSBCAMD_DEVICE_EXTENSION deviceExtension;
    ULONG bytesTransferred;
    NTSTATUS status;
    PHW_STREAM_REQUEST_BLOCK srb;
    PKSSTREAM_HEADER dataPacket;

	// before we pass this raw frame to Cam driver, we will clear the stream header options flag
	// and let the Cam driver set it appropriately if it needs to indicate anything other than 
	// key frames in there in case it process compressed data (ex. h.263, etc..). Otherwise, we 
	// set the default flag (key frames only) in USBCAMD_CompleteRead.

    
	channelExtension = usbWorkItem->ChannelExtension;

	ACQUIRE_PIN_SEMAPHORE(channelExtension);

    readExtension = usbWorkItem->Request;
    deviceExtension = channelExtension->DeviceExtension;
    srb = readExtension->Srb;

    ASSERT_CHANNEL(channelExtension);
    ASSERT_READ(readExtension);
   
	dataPacket = srb->CommandData.DataBufferArray;
	dataPacket->OptionsFlags =0;    

	if ( channelExtension->FirstFrame) {
		channelExtension->StartTime = GetStreamTime(srb, channelExtension);
		channelExtension->FirstFrame = FALSE;
	}

    frameBuffer = USBCAMD_GetFrameBufferFromSrb(readExtension->Srb,
                                                &maxLength);
    
    status = 
        (*deviceExtension->DeviceData.CamProcessRawVideoFrame)(
             deviceExtension->StackDeviceObject,
             USBCAMD_GET_DEVICE_CONTEXT(deviceExtension),
             USBCAMD_GET_FRAME_CONTEXT(readExtension),
             frameBuffer,
             maxLength,
             readExtension->RawFrameBuffer,
             readExtension->RawFrameLength,
             readExtension->NumberOfPackets,             
             &bytesTransferred);
#if DBG                
            USBCAMD_KdPrint (MAX_TRACE, ("'Frame Complete, length = %d cnt = %d\n",
                bytesTransferred,
                channelExtension->FrameCaptured));

            LOGENTRY("xVID",
                      channelExtension,  
                      channelExtension->FrameCaptured, 
                      bytesTransferred);                
#endif                


	// The number of bytes transfer of the read is set above just before
	// USBCAMD_CompleteReadRequest is called.
    
	USBCAMD_CompleteRead(channelExtension,
						 readExtension, 
						 status, 
						 bytesTransferred); 

    USBCAMD_ExFreePool(usbWorkItem);

	RELEASE_PIN_SEMAPHORE(channelExtension);
}
