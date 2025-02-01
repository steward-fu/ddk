/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

   IsoStrm.c

Abstract:

    Isochronous USB device driver for Intel 82930 USB test board
    Isochronous stream test code

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.


Revision History:

    7/7/98 : created

--*/


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#define DRIVER

#include "usbdi.h"
#include "usbdlib.h"
#include "Iso82930.h"





NTSTATUS
IsoUsb_ProcessTransfer(
    IN PISOUSB_TRANSFER_OBJECT TransferObject
    )
/*++

Routine Description:
	This is where a real driver would process the data from the Iso transfer in some way;
	In this sample all we do is check the IRP status from the OS and the URB status
	from USBD

Arguments:

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
	USBD_STATUS usbdStatus;
    PIRP irp;
	PURB urb;
    PISOUSB_STREAM_OBJECT StreamObject;
    ULONG i;

    irp = TransferObject->Irp;
    StreamObject = TransferObject->StreamObject;
    urb = TransferObject->Urb;
    ntStatus = irp->IoStatus.Status;

    //
    // check the irp
    //

    if (!NT_SUCCESS(ntStatus)) {
        ISOUSB_KdPrint (DBGLVL_MEDIUM,(" IsoUsb_ProcessTransfer() **BAD Iso IRP status = 0x%x\n", ntStatus));        

        ISOUSB_Trap(DBGLVL_MAXIMUM);
    }
	else {
        ISOUSB_KdPrint (DBGLVL_MAXIMUM,(" IsoUsb_ProcessTransfer() Good Iso IRP status = 0x%x\n", ntStatus));        
	}


    //
    // now check the urb header
    //
	usbdStatus = urb->UrbHeader.Status;

    if (!USBD_SUCCESS(usbdStatus)) {
        ISOUSB_KdPrint (DBGLVL_MEDIUM,(" IsoUsb_ProcessTransfer() **BAD URB  HEADER USBD_STATUS status = 0x%x\n", usbdStatus));        

        ISOUSB_Trap(DBGLVL_MAXIMUM);
    }
	else {
        ISOUSB_KdPrint (DBGLVL_MAXIMUM,(" IsoUsb_ProcessTransfer() Good URB HEADER USBD_STATUS status = 0x%x\n", usbdStatus));        
	}
    

    // check the Urb packets
    for (i=0; i< urb->UrbIsochronousTransfer.NumberOfPackets; i++)
    {

        StreamObject->TotalPacketsProcessed++;

        if (!USBD_SUCCESS( urb->UrbIsochronousTransfer.IsoPacket[i].Status)) {

            ISOUSB_KdPrint (  DBGLVL_MEDIUM , (" IsoUsb_ProcessTransfer() BAD PACKET USBD STATUS  [%02d] Length 0x%08X, Status 0x%08X\n",
                             i,
                             urb->UrbIsochronousTransfer.IsoPacket[i].Length,
                             urb->UrbIsochronousTransfer.IsoPacket[i].Status
                            ));
            ntStatus =  STATUS_UNSUCCESSFUL; // set  error return code           
                
            StreamObject->ErrorPacketCount++;
        }
        else {
            // Successfull;
            // Note that here is the place we would  do any data processing/copying to frame
            // buffers, for example , if we were a video capture device, etc ...

            StreamObject->TotalBytesProcessed += urb->UrbIsochronousTransfer.IsoPacket[i].Length;
        }
        
    }


    return ntStatus;
}


NTSTATUS
IsoUsb_InitializeStreamUrb(
    IN PDEVICE_OBJECT DeviceObject,
    IN PISOUSB_TRANSFER_OBJECT TransferObject
    )
/*++

Routine Description:

Arguments:

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG siz;
    ULONG packetSize, numPackets, maxXferSize, i;
    PURB urb;
    PISOUSB_STREAM_OBJECT streamObject = TransferObject->StreamObject;

    urb = TransferObject->Urb;

	maxXferSize = streamObject->PipeInfo->MaximumTransferSize;  // We set this
    packetSize = streamObject->PipeInfo->MaximumPacketSize;	    // USBD sets this
    numPackets = maxXferSize / packetSize;
    
    siz = GET_ISO_URB_SIZE(numPackets);

    ISOUSB_KdPrint (DBGLVL_MAXIMUM,("IsoUsb_InitializeStreamUrb() siz = 0x%x urb 0x%x\n", siz, urb));

    RtlZeroMemory(urb, siz);

    urb->UrbIsochronousTransfer.Hdr.Length = (USHORT) siz;
    urb->UrbIsochronousTransfer.Hdr.Function =
                URB_FUNCTION_ISOCH_TRANSFER;
    urb->UrbIsochronousTransfer.PipeHandle =
               streamObject->PipeInfo->PipeHandle;

	// We are reading from the device
    urb->UrbIsochronousTransfer.TransferFlags = USBD_TRANSFER_DIRECTION_IN;

	// A device will always use either TransferBufferMDL or TransferBuffer, NEVER both
    urb->UrbIsochronousTransfer.TransferBufferMDL = NULL;
    urb->UrbIsochronousTransfer.TransferBuffer = TransferObject->DataBuffer;
    
    urb->UrbIsochronousTransfer.TransferBufferLength =
        numPackets * packetSize;

    // start sending/receiving right away
    urb->UrbIsochronousTransfer.TransferFlags |=
            USBD_START_ISO_TRANSFER_ASAP;

    urb->UrbIsochronousTransfer.NumberOfPackets = numPackets;
    urb->UrbIsochronousTransfer.UrbLink = 0;

    for (i=0; i< urb->UrbIsochronousTransfer.NumberOfPackets; i++) {
        urb->UrbIsochronousTransfer.IsoPacket[i].Offset
                    = i * packetSize;
    }

    ISOUSB_KdPrint (DBGLVL_MAXIMUM,(" Init Stream iso urb Length = 0x%x buf = 0x%x\n",
        urb->UrbIsochronousTransfer.TransferBufferLength,
        urb->UrbIsochronousTransfer.TransferBuffer));

    return ntStatus;
}


NTSTATUS
IsoUsb_StartTransfer(
    IN PDEVICE_OBJECT DeviceObject,
    IN PISOUSB_STREAM_OBJECT StreamObject,
    IN ULONG Index
    )
/*++

Routine Description:

Arguments:

Return Value:

    nt status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    CCHAR stackSize;
    PDEVICE_EXTENSION deviceExtension;
    PISOUSB_TRANSFER_OBJECT transferObject;
    ULONG packetSize, numPackets, maxXferSize;

	maxXferSize = StreamObject->PipeInfo->MaximumTransferSize;  // We set this
    packetSize = StreamObject->PipeInfo->MaximumPacketSize;	    // USBD sets this
    numPackets = maxXferSize / packetSize;

    ISOUSB_KdPrint (DBGLVL_MEDIUM,(" ENTER IsoUsb_StartTransfer() \n"));

    deviceExtension = DeviceObject->DeviceExtension;
    
    transferObject = ExAllocatePool(NonPagedPool, sizeof(ISOUSB_TRANSFER_OBJECT));

    if (transferObject) {
        PIRP irp;

        ISOUSB_KdPrint (DBGLVL_MEDIUM,(" TransferObject = %x \n", transferObject));
		RtlZeroMemory( transferObject,sizeof(ISOUSB_TRANSFER_OBJECT));

        StreamObject->TransferObjectList[Index] = 
            transferObject;

        transferObject->StreamObject = StreamObject;

        stackSize = (CCHAR)(deviceExtension->TopOfStackDeviceObject->StackSize + 1);

        transferObject->Irp = 
            irp = IoAllocateIrp(stackSize,
                                FALSE);

        transferObject->DataBuffer = ExAllocatePool(NonPagedPool, 
                                                    numPackets * 
                                                    StreamObject->PipeInfo->MaximumPacketSize); 
                        
        transferObject->Urb = 
            ExAllocatePool(NonPagedPool, GET_ISO_URB_SIZE( numPackets ));
         
        if (transferObject->Urb && transferObject->DataBuffer) {
        
            PIO_STACK_LOCATION nextStack; 
            NTSTATUS status;
            
            IsoUsb_InitializeStreamUrb(DeviceObject, transferObject);

            nextStack = IoGetNextIrpStackLocation(irp);
            ASSERT(nextStack != NULL);

            nextStack->Parameters.Others.Argument1 = transferObject->Urb;
            nextStack->Parameters.DeviceIoControl.IoControlCode = 
                IOCTL_INTERNAL_USB_SUBMIT_URB;                    
            nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

            IoSetCompletionRoutine(irp,
                    IsoUsb_IsoIrp_Complete,
                    transferObject, //pass transfer object as Context
                    TRUE,  // Invoke on Success
                    TRUE,  // Invoke on Error
                    TRUE); // Invoke on Cancel

            //
            // submit the request
            //

            StreamObject->PendingIrps++;           // increment this stream's pending irp count
			IsoUsb_IncrementIoCount(DeviceObject); // also increment global pending IRP count

            status = IoCallDriver(deviceExtension->TopOfStackDeviceObject, 
                                  irp);

        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;

            if (transferObject->DataBuffer) { 
                ExFreePool(transferObject->DataBuffer);
            }

            if (transferObject->Urb) { 
                ExFreePool(transferObject->Urb);
            }
            
        }
            
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    ISOUSB_KdPrint (DBGLVL_MEDIUM,(" EXIT IsoUsb_StartTransfer() %x\n", ntStatus));
    
    return ntStatus;
}


NTSTATUS
IsoUsb_IsoIrp_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
	IN PVOID Context
    )
/*++

Routine Description:

    This routine is called when the port driver completes an IRP.  

Arguments:

    DeviceObject - Pointer to the device object for the class device.

    Irp - Irp completed.

	Context - Driver defined context, points to a transfer extension structure 
    	for a pair of parallel iso requests.

Return Value:

    The function value is the final status from the operation.

--*/
{
    NTSTATUS status;
    PDEVICE_EXTENSION deviceExtension;
    PDEVICE_OBJECT deviceObject;
    PISOUSB_TRANSFER_OBJECT transferObject;
    PISOUSB_STREAM_OBJECT streamObject;
    PIO_STACK_LOCATION nextStack;
    

    transferObject = Context;
    streamObject = transferObject->StreamObject;    
    deviceObject = streamObject->DeviceObject;
    deviceExtension = deviceObject->DeviceExtension;

    streamObject->PendingIrps--;           // Decrement this stream's pending irp count
	if ( !streamObject->PendingIrps ) {    // back to 0? signal no pending irps event
        KeSetEvent(&streamObject->NoPendingIrpEvent,
                   1,
                   FALSE);
 	}

	IsoUsb_DecrementIoCount(deviceObject); // also deccrement global pending IRP count
    
    ISOUSB_KdPrint (DBGLVL_MAXIMUM,(" ENTER IsoUsb_IsoIrp_Complete(), streamObject->PendingIrps = dec %d\n", streamObject->PendingIrps));

	// Check the IRP and URB status in the transferObject
    // Here is where a driver for a real device would collect and/or process data in the buffers
    status = IsoUsb_ProcessTransfer(transferObject);

	ISOUSB_KdPrintCond( DBGLVL_MEDIUM,(!NT_SUCCESS(status)), ("IsoUsb_IsoIrp_Complete() BAD transfer status 0x%x\n", status));

	if( !NT_SUCCESS(status) ) {
	    IsoUsb_ResetParentPort( DeviceObject ); 
    }

	// See if a stop stream has been requested..
	if( streamObject->IsoStreamStarted ) {// this is set FALSE when we get a stop stream ioctl request

		// Last xfer was OK and no stop has been requested;
		// Resubmit the whole thing again and recycle it..
		ISOUSB_KdPrint (DBGLVL_MAXIMUM,("IsoUsb_IsoIrp_Complete() ABOUT TO RECYCLE IRP AND URB!\n"));
		IsoUsb_InitializeStreamUrb(DeviceObject, transferObject);

		nextStack = IoGetNextIrpStackLocation(Irp);
		ASSERT(nextStack != NULL);

		nextStack->Parameters.Others.Argument1 = transferObject->Urb;
		nextStack->Parameters.DeviceIoControl.IoControlCode = 
			IOCTL_INTERNAL_USB_SUBMIT_URB;                    
		nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;


        IoSetCompletionRoutine(Irp,
                IsoUsb_IsoIrp_Complete,
                transferObject, //pass transfer object as Context
                TRUE,  // Invoke on Success
                TRUE,  // Invoke on Error
                TRUE); // Invoke on Cancel


		// increment this stream object's pending irp count
		streamObject->PendingIrps++; 
		// increment this stream object's total times recycled count
		streamObject->TimesRecycled++; 
		// also increment global pending IRP count
		IsoUsb_IncrementIoCount(deviceObject); 

		//
		// Resubmit the request...
        // Note that if the driver has actually done a  fair amount of data processing
        // or copying from packet buffers, you may want to schedule a dpc 
        // to resubmit instead of doing it here
		//
		status = IoCallDriver(deviceExtension->TopOfStackDeviceObject, 
							  Irp);
	}
	
    
    ISOUSB_KdPrint (DBGLVL_MAXIMUM,(" EXIT IsoUsb_IsoIrp_Complete(), streamObject->PendingIrps = dec %d\n", streamObject->PendingIrps));

    return STATUS_MORE_PROCESSING_REQUIRED;
}               


VOID
IsoUsb_StreamTimeoutDPC(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++

Routine Description:

  Check for data capture problems 
  Stop stream if problems found

Arguments:

Return Value:

    The function value is the final status from the operation.

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    PDEVICE_OBJECT deviceObject;
    PISOUSB_STREAM_OBJECT streamObject;
    BOOLEAN inQueue;
    LARGE_INTEGER dueTime;

    streamObject = DeferredContext;    
    deviceObject = streamObject->DeviceObject;
    deviceExtension = deviceObject->DeviceExtension;

    // 



    //
    // schedule next one
    //
    
    if (streamObject->IsoStreamStarted) {

        KeInitializeTimer(&streamObject->TimeoutTimer);
        KeInitializeDpc(&streamObject->TimeoutDpc,
                        IsoUsb_StreamTimeoutDPC,
                        streamObject);

        dueTime.QuadPart = -10000 * ISOUSB_STREAM_TIMEOUT_INTERVAL;

        inQueue = KeSetTimer(&streamObject->TimeoutTimer,
                             dueTime,
                             &streamObject->TimeoutDpc);        

        ASSERT(inQueue == FALSE);  // assert timer not already in system queue                           
        
    }
}


NTSTATUS
IsoUsb_StartIsoStream(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

	Handler for IOCTL_ISOUSB_START_ISO_STREAM

Arguments:

    DeviceObject - pointer to the device extension for this instance of the
                     82930 device.

    Irp -

Return Value:

    NT status code

--*/
{
    ULONG i;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpStack;
    ULONG inputBufferLength;
    ULONG outputBufferLength;
    PULONG streamObjectHandle;
    PISOUSB_STREAM_OBJECT streamObject;
    LARGE_INTEGER dueTime;
    BOOLEAN inQueue;
	PUSBD_PIPE_INFORMATION PipeInfo;
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;

    irpStack = IoGetCurrentIrpStackLocation (Irp);
     
    streamObjectHandle = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    streamObject = ExAllocatePool(NonPagedPool, sizeof(ISOUSB_STREAM_OBJECT));

    if (streamObject) {

		RtlZeroMemory( streamObject, sizeof(ISOUSB_STREAM_OBJECT));

		// hard code to 5th pipe; this is the iso in pipe on our test board
		PipeInfo = &(deviceExtension->UsbInterface->Pipes[4]);

		ISOUSB_KdPrint (DBGLVL_MEDIUM,(" ENTER IsoUsb_StartIsoStream()  pipe %x\n", PipeInfo));

		PipeInfo->PipeFlags = TRUE; // set flag for this pipe opened
		deviceExtension->OpenPipeCount++;

		// try to power up device if its not already in D0
		IsoUsb_SelfSuspendOrActivate( DeviceObject, FALSE );

        streamObject->DeviceObject = DeviceObject;
        streamObject->PipeInfo = PipeInfo;
        streamObject->IsoStreamStarted = TRUE;

		// event to be set when PendingIrps == 0; signals stream can be stopped
		KeInitializeEvent(&streamObject->NoPendingIrpEvent, NotificationEvent, FALSE);

        // This initializes the pair of IRP/URBS that we will keep endlessly recycling
		// until the Iso stream is stoppped; at least one of these pairs will always be in 
		// use and one will be available so continuous throughput is maintained.
		for (i=0; i< ISOUSB_MAX_IRP; i++) {

            ntStatus = IsoUsb_StartTransfer(DeviceObject,
                                            streamObject,
                                            i);

            if (!NT_SUCCESS(ntStatus)) {
				ISOUSB_Trap(DBGLVL_MAXIMUM);
                break;
            }
            
        }

		// We are returning the pointer to our stream object as an untyped handle to the user
        *streamObjectHandle = (ULONG) streamObject;

        //
        // start the timeout DPC
        //

        KeInitializeTimer(&streamObject->TimeoutTimer);
        KeInitializeDpc(&streamObject->TimeoutDpc,
                        IsoUsb_StreamTimeoutDPC,
                        streamObject);

        dueTime.QuadPart = -10000 * ISOUSB_STREAM_TIMEOUT_INTERVAL;

        inQueue = KeSetTimer(&streamObject->TimeoutTimer,
                             dueTime,
                             &streamObject->TimeoutDpc);        

        
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

	// We are returning the size of our stream object pointer to the user
    Irp->IoStatus.Information = sizeof(*streamObjectHandle);                
    Irp->IoStatus.Status = ntStatus;

    ISOUSB_KdPrint (DBGLVL_MEDIUM,(" EXIT IsoUsb_StartIsoStream() %x\n", ntStatus));

    return ntStatus;
}


NTSTATUS
IsoUsb_StopIsoStream(
    IN PDEVICE_OBJECT DeviceObject,
    IN PISOUSB_STREAM_OBJECT StreamObject,
    IN PIRP Irp
    )
/*++

Routine Description:

	Handler for IOCTL_ISOUSB_STOP_ISO_STREAM

Arguments:

    DeviceObject - pointer to the device extension for this instance of the
                     82930 device.

    StreamObject - pointer to the ISOUSB_STREAM_OBJECT; We returned this pointer to the user
	  in the return data buffer from their IOCTL_ISOUSB_START_ISO_STREAM call

    Irp - The current IRP for this request

Return Value:

    NT status code

--*/
{
	PUSBD_PIPE_INFORMATION pipe;
	int i;
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    ULONG packetSize, numPackets, maxXferSize;


	// validate the input parm; its DeviceObject member should match our DeviceObject
	ISOUSB_KdPrintCond (DBGLVL_MEDIUM, 
						(!StreamObject || (StreamObject->DeviceObject != DeviceObject)), 
						("ENTER IsoUsb_StopIsoStream() StreamObject parm INVALID!!!!!\n"));	

	// See if we were called with a bogus or NULL StreamObject parm;
	// This could happen if user calls without ever having successfully started a stream
    if (!StreamObject || (StreamObject->DeviceObject != DeviceObject))
		return STATUS_INVALID_PARAMETER;

	maxXferSize = StreamObject->PipeInfo->MaximumTransferSize;  // We set this
    packetSize = StreamObject->PipeInfo->MaximumPacketSize;	    // USBD sets this
    numPackets = maxXferSize / packetSize;

	// Tell IsoUsb_StreamTimeoutDPC() not to reschedule itself on next timeout
	// This also flags IsoUsb_IsoIrp_Complete() to stop recycling the pair of stream IRP/Urbs
	StreamObject->IsoStreamStarted = FALSE; 

    // Wait for any io request pending for this stream object to
    // complete before returning success.
	// This  event is set when streamObject->PendingIrpCount goes to 0
    KeWaitForSingleObject(
                &StreamObject->NoPendingIrpEvent,
                Suspended,
                KernelMode,
                FALSE,
                NULL);


    ISOUSB_KdPrint (DBGLVL_DEFAULT, ("IsoUsb_StopIsoStream() StreamObject->TimesRecycled = decimal %d,  0x%x\n",
		StreamObject->TimesRecycled , StreamObject->TimesRecycled));
    ISOUSB_KdPrint (DBGLVL_DEFAULT, ("IsoUsb_StopIsoStream() Total packets processed = decimal %d\n",
		StreamObject->TotalPacketsProcessed ));
    ISOUSB_KdPrint (DBGLVL_DEFAULT, ("IsoUsb_StopIsoStream() Total stream bytes processed  = decimal %d\n",
		StreamObject->TotalBytesProcessed ));
    ISOUSB_KdPrint (DBGLVL_DEFAULT, ("EXIT IsoUsb_StopIsoStream() Error Packet Count  = decimal %d\n",
		StreamObject->ErrorPacketCount ));


    // Free all the buffers, URBS, and Irps associated with our stream object
	for (i=0; i< ISOUSB_MAX_IRP; i++) {
		PISOUSB_TRANSFER_OBJECT transferObject;

		transferObject = StreamObject->TransferObjectList[i];

		ISOUSB_ASSERT( transferObject );

		ISOUSB_ASSERT( transferObject->Irp );
		IoFreeIrp( transferObject->Irp );

		ISOUSB_ASSERT( transferObject->Urb );
		ExFreePool( transferObject->Urb );

		ISOUSB_ASSERT( transferObject->DataBuffer );
		ExFreePool( transferObject->DataBuffer );

		ExFreePool( transferObject );
    }

	ExFreePool( StreamObject ); // also free the stream object itself


	// Close our Iso input pipe;
	// Hard-code to 5th pipe; this is the iso input pipe on our test board
	pipe = &(deviceExtension->UsbInterface->Pipes[4]);

	pipe->PipeFlags = FALSE; // set flag for this pipe closed
	deviceExtension->OpenPipeCount--;

	// BUGBUG? Not sure why this is neccesary, but it is...
	IsoUsb_ResetParentPort( DeviceObject ); 

	// try to power down device if we just closed the last open pipe
	IsoUsb_SelfSuspendOrActivate( DeviceObject, TRUE );

	return STATUS_SUCCESS;
    
}






