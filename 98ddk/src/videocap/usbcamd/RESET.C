/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

  reset.c

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

NTSTATUS
USBCAMD_GetPortStatus(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PULONG PortStatus
    )
/*++

Routine Description:

    Passes a URB to the USBD class driver

Arguments:

    DeviceExtension - pointer to the device extension for this instance of an USB camera

    Urb - pointer to Urb request block

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_GetPortStatus\n"));

    *PortStatus = 0;

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_GET_PORT_STATUS,
                DeviceExtension->StackDeviceObject,
                NULL,
                0,
                NULL,
                0,
                TRUE, /* INTERNAL */
                &event,
                &ioStatus);

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);

    //
    // pass the URB to the USB driver stack
    //
    nextStack->Parameters.Others.Argument1 = PortStatus;

    USBCAMD_KdPrint (MAX_TRACE, ("'calling USBD port status api\n"));

    if (DeviceExtension->Initialized) {
        ntStatus = IoCallDriver(DeviceExtension->StackDeviceObject,
                                irp);
    } else {
        ntStatus = STATUS_DEVICE_DATA_ERROR;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

        USBCAMD_KdPrint (MAX_TRACE, ( "'Wait for single object\n"));

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

        USBCAMD_KdPrint (MAX_TRACE, ("'Wait for single object, returned %x\n", status));
        
    } else {
        ioStatus.Status = ntStatus;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'Port status = %x\n",
        *PortStatus));

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    USBCAMD_KdPrint(MAX_TRACE, ("'USBCAMD_GetPortStatus (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
USBCAMD_EnablePort(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension
    )
/*++

Routine Description:

    Passes a URB to the USBD class driver

Arguments:

    DeviceExtension - pointer to the device extension for this instance of an USB camera

    Urb - pointer to Urb request block

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_EnablePort\n"));

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_ENABLE_PORT,
                DeviceExtension->StackDeviceObject,
                NULL,
                0,
                NULL,
                0,
                TRUE, /* INTERNAL */
                &event,
                &ioStatus);

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);

    //
    // pass the URB to the USB driver stack
    //

    USBCAMD_KdPrint (MAX_TRACE, ("'calling USBD enable port api\n"));

    if (DeviceExtension->Initialized) {
        ntStatus = IoCallDriver(DeviceExtension->StackDeviceObject,
                                irp);
    } else {
        ntStatus = STATUS_DEVICE_DATA_ERROR;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

        USBCAMD_KdPrint (MAX_TRACE, ( "'Wait for single object\n"));

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

        USBCAMD_KdPrint (MAX_TRACE, ("'Wait for single object, returned %x\n", status));
        
    } else {
        ioStatus.Status = ntStatus;
    }

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    USBCAMD_KdPrint(MAX_TRACE, ("'USBCAMD_EnablePort (%x)\n", ntStatus));

    return ntStatus;
}

NTSTATUS
USBCAMD_ResetChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    )
/*++

Routine Description:

    This function restarts the streaming process from an error state at 
    PASSIVE_LEVEL.

Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.
                    
    ChannelExtension - Channel to reset.    

Return Value:

--*/        
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG portStatus;
	BOOLEAN portUnplugged= FALSE;

    USBCAMD_SERIALIZE(DeviceExtension);
    
    USBCAMD_KdPrint (MIN_TRACE, ("'USBCAMD_ResetChannel\n"));

    ASSERT_CHANNEL(ChannelExtension);
    ASSERT(DeviceExtension->OpenFRC > 0);
    ASSERT(ChannelExtension->ChannelPrepared == TRUE);

    LOGENTRY("Rset", 0, ChannelExtension, 0);  

#ifdef MAX_DEBUG
    TEST_TRAP();
#endif  
    //
    // Check the port state, if it is disabled we will need 
    // to re-enable it
    //
    ntStatus = USBCAMD_GetPortStatus(DeviceExtension, &portStatus);

    if (NT_SUCCESS(ntStatus) && !(portStatus & USBD_PORT_CONNECTED)) {
        //
        // Camera is unplugged, proceed with canceling pending SRBs
        //
		portUnplugged = TRUE;
        
    }
	else {

		if (NT_SUCCESS(ntStatus) && !(portStatus & USBD_PORT_ENABLED)) {
        //
        // port is disabled, attempt reset
        //
			USBCAMD_EnablePort(DeviceExtension);
		}


		//
		// channel made not be in error mode, make sure and issue 
		// an abort beforing waiting for the channel to spin down
		//

		// if we don't already have an error set one
		ChannelExtension->StreamError = TRUE;

		USBCAMD_ResetPipes(DeviceExtension,
						   ChannelExtension, 
						   DeviceExtension->Interface,
						   TRUE);    
   	}

    //
    // Block here until the channel stream process is completely
    // stopped
    //

    {
    NTSTATUS status;    

    USBCAMD_KdPrint (MAX_TRACE, ("'Reset, Wait for stream to stop\n"));
    LOGENTRY("Rwat", 0, ChannelExtension, 0);  
    status = KeWaitForSingleObject(
                       &ChannelExtension->ResetEvent,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);    

    LOGENTRY("Rwak", 0, ChannelExtension, status);                                 
    USBCAMD_KdPrint (MAX_TRACE, ("'Reset, stream stopped\n"));
#ifdef MAX_DEBUG    
    TEST_TRAP();
#endif    
    }

    // 
    // we have been signalled by the timeout DPC that the stop is complete
    // go ahead and attempt to restart the channel.
    //
   
    //
    // Clear the error condition
    //
    
    ChannelExtension->StreamError = FALSE;

	if (portUnplugged == FALSE){

		//
		// now reset the pipes
		//

		ntStatus = USBCAMD_ResetPipes(DeviceExtension,
									  ChannelExtension,
									  DeviceExtension->Interface,
									  FALSE);

		//
		// BUGBUG
		// if we are in an error state due to a babble then we will
		// need to call the USB stack to reset our port, this will 
		// requre us to re-configure the device if we wish to recover
		// from this.

		//
		// only restart the stream if it is already in the running state
		//
    
		if (ChannelExtension->ImageCaptureStarted) {
			// send hardware stop and re-start
			if (NT_SUCCESS(ntStatus)) {
				ntStatus = (*DeviceExtension->DeviceData.CamStopCapture)(
							DeviceExtension->StackDeviceObject,      
							USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension));
			}                    

			if (NT_SUCCESS(ntStatus)) {
				ntStatus = (*DeviceExtension->DeviceData.CamStartCapture)(
							DeviceExtension->StackDeviceObject,
							USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension));    
			}                    

			if (NT_SUCCESS(ntStatus)) {

				ChannelExtension->DataPipe = DeviceExtension->DataPipe;
				ChannelExtension->SyncPipe = DeviceExtension->SyncPipe;

				USBCAMD_StartIsoStream(DeviceExtension, ChannelExtension, TRUE);
			}        
		}
	}
    // OK to handle another reset now
    InterlockedDecrement(&DeviceExtension->TimeoutCount);

    TRAP_ERROR(ntStatus);

    USBCAMD_RELEASE(DeviceExtension);

    return ntStatus;
}            


NTSTATUS
USBCAMD_ResetPipes(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBD_INTERFACE_INFORMATION InterfaceInformation,
    IN BOOLEAN Abort
    )
/*++

Routine Description:

    Reset both pipes associated with a video channel on the
    camera.

Arguments:

Return Value:


--*/
{
    NTSTATUS ntStatus;
	PURB urb;

    USBCAMD_KdPrint (MAX_TRACE, ("'USBCAMD_ResetPipes\n"));

	urb = USBCAMD_ExAllocatePool(NonPagedPool, 
						 sizeof(struct _URB_PIPE_REQUEST));

	if (urb) {
	
        urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
    	urb->UrbHeader.Function = (USHORT) (Abort ? URB_FUNCTION_ABORT_PIPE : 
                                                   	URB_FUNCTION_RESET_PIPE);
                                                           	
        urb->UrbPipeRequest.PipeHandle = 
            InterfaceInformation->Pipes[ChannelExtension->DataPipe].PipeHandle;

        ntStatus = USBCAMD_CallUSBD(DeviceExtension, urb);
        LOGENTRY("Rpip", urb, 0, ntStatus);  

        if (NT_SUCCESS(ntStatus) && ChannelExtension->SyncPipe != -1)  {
            urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
        	urb->UrbHeader.Function =(USHORT) (Abort ? URB_FUNCTION_ABORT_PIPE : 
                                                    	URB_FUNCTION_RESET_PIPE);
            urb->UrbPipeRequest.PipeHandle = 
                InterfaceInformation->Pipes[ChannelExtension->SyncPipe].PipeHandle;
                
            ntStatus = USBCAMD_CallUSBD(DeviceExtension, urb);
            LOGENTRY("Rpip", urb, 0, ntStatus);  
        }            

        USBCAMD_ExFreePool(urb);
        
	} else {
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;		
    }		

    USBCAMD_KdPrint (MAX_TRACE, ("'USBCAMD_ResetPipes 0x%x\n", ntStatus));

    TRAP_ERROR(ntStatus);

    return ntStatus;
}	


VOID
USBCAMD_ChannelTimeoutDPC(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL. 

    
    
Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - 

    SystemArgument1 - not used.
    
    SystemArgument2 - not used.

Return Value:

    None.

--*/
{
    PUSBCAMD_CHANNEL_EXTENSION channelExtension;
    LARGE_INTEGER dueTime;
    BOOLEAN inQueue;

    channelExtension = DeferredContext;    

    LOGENTRY("Rdpc", 
              channelExtension, 
              channelExtension->StreamError, 
              channelExtension->Flags); 
    
    ASSERT_CHANNEL(channelExtension);
    ASSERT(channelExtension->ChannelPrepared == TRUE);
    
    if ((channelExtension->Flags & USBCAMD_TIMEOUT_STREAM_WAIT) ||
        (channelExtension->Flags & USBCAMD_STOP_STREAM)) {

        BOOLEAN idle = TRUE;
        ULONG i;

        // First clean out any pending reads
        
        // 
        // We have an error on the stream or a stop request
        // pending.  Scan through the transfers if nothiing 
        // is pending then go ahead and complete any outstanding 
        // read requests with an error.
        //
        
        for (i=0; i<USBCAMD_MAX_REQUEST; i++) {
            if (channelExtension->TransferExtension[i].Pending > 0) {
                idle = FALSE;
                break;
            }                
        }            

        if (channelExtension->ImageCaptureStarted == FALSE) {
            //
            // reset called before stream fully started
            // just ignore for now.
            idle = FALSE;
        }
        
        LOGENTRY("Res1", channelExtension, idle, 0);  

        if (idle) {

            //
            // now complete any pending reads in the queue with an error
            //
#pragma warning(disable:4127)
            do {
                PLIST_ENTRY listEntry;
                PUSBCAMD_READ_EXTENSION readExtension;

                listEntry = 
                    ExInterlockedRemoveHeadList(&(channelExtension->PendingIoList),
                                                &channelExtension->PendingIoListSpin);
                
                if (listEntry != NULL) {
                        LOGENTRY("ResR", channelExtension, listEntry, 0); 
                    readExtension =     
                        (PUSBCAMD_READ_EXTENSION) CONTAINING_RECORD(listEntry, 
                                                 USBCAMD_READ_EXTENSION  , 
                                                 ListEntry);                        
                    USBCAMD_KdPrint (MIN_TRACE, 
                        ("Timeout completing read with error\n"));    
                        
                    LOGENTRY("toCP", readExtension, 0, STATUS_DEVICE_DATA_ERROR); 

                    USBCAMD_CompleteRead(channelExtension,
                                         readExtension, 
                                         //STATUS_DEVICE_DATA_ERROR, 
										 STATUS_CANCELLED,
                                         0);
                    
                } else {
                    break;
                }

            } while (1);
#pragma warning(default:4127)

            //
            // if we had an irp for the current frame complete it as well
            //
            if (channelExtension->CurrentRequest) {
                PUSBCAMD_READ_EXTENSION readExtension = 
                    channelExtension->CurrentRequest;

                channelExtension->CurrentRequest = NULL;                        
                LOGENTRY("xxIR", readExtension, 0, STATUS_DEVICE_DATA_ERROR); 

                USBCAMD_CompleteRead(channelExtension,
                                     readExtension, 
                                     //STATUS_DEVICE_DATA_ERROR, 
									 STATUS_CANCELLED,
                                     0);
            }
            
            // 
            // signal the reset request if one is waiting
            //
            
            if (channelExtension->Flags & USBCAMD_TIMEOUT_STREAM_WAIT) {
                channelExtension->Flags &= ~USBCAMD_TIMEOUT_STREAM_WAIT;
                LOGENTRY("RSIG", 0, 0, 0); 
                KeSetEvent(&channelExtension->ResetEvent,
                           1,
                           FALSE);
            }

            //
            // stream has stopped go ahead and signal the stop event
            // if we have one
            //

            if (channelExtension->Flags & USBCAMD_STOP_STREAM) {
                channelExtension->Flags &= ~USBCAMD_STOP_STREAM;
                
                //
                // image capture has been stopped
                //
                
                LOGENTRY("SSIG", 0, 0, 0); 
                KeSetEvent(&channelExtension->StopEvent,
                           1,
                           FALSE);
            } 
        }
    }
    
    //
    // Schedule the next one
    //
    
    if (channelExtension->Flags & USBCAMD_ENABLE_TIMEOUT_DPC) {

        KeInitializeTimer(&channelExtension->TimeoutTimer);
        KeInitializeDpc(&channelExtension->TimeoutDpc,
                        USBCAMD_ChannelTimeoutDPC,
                        channelExtension);
        dueTime.QuadPart = -10000 * USBCAMD_TIMEOUT_INTERVAL;

        inQueue = KeSetTimer(&channelExtension->TimeoutTimer,
                             dueTime,
                             &channelExtension->TimeoutDpc);        

        ASSERT(inQueue == FALSE);                             
        
    }
}


VOID
USBCAMD_ProcessResetRequest(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    )
/*++

Routine Description:

    Request a reset of the ISO stream.
    This function is re-entarnt and can be called at DPC level

Arguments:

Return Value:

    None.

--*/
{
    PUSBCAMD_WORK_ITEM usbWorkItem;

    ASSERT_CHANNEL(ChannelExtension);

    if (InterlockedIncrement(&DeviceExtension->TimeoutCount) > 0) {
        USBCAMD_KdPrint (MIN_TRACE, ("'timeout already scheduled\n"));
        InterlockedDecrement(&DeviceExtension->TimeoutCount);
        return;
    }
    
    usbWorkItem = USBCAMD_ExAllocatePool(NonPagedPool, sizeof(*usbWorkItem));

    if (usbWorkItem) {
        LOGENTRY("WKto", 0, usbWorkItem, 0);

        //
        // set the reset event now so that any subsequent reset 
        // requests get ignored
        //

        KeInitializeEvent(&ChannelExtension->ResetEvent, NotificationEvent, FALSE);
        
        //
        // reset request is now active tell our timeout DPC to signal
        // us when the stream is idle
        //
        
        ChannelExtension->Flags |= USBCAMD_TIMEOUT_STREAM_WAIT;
        
        ExInitializeWorkItem(&usbWorkItem->WorkItem,
                             USBCAMD_ResetWorkItem,
                             usbWorkItem);

        usbWorkItem->ChannelExtension = ChannelExtension;
        ExQueueWorkItem(&usbWorkItem->WorkItem,
                        CriticalWorkQueue);
                        //DelayedWorkQueue);

    } else {
        //
        // failed to schedule the timeout
        //
        InterlockedDecrement(&DeviceExtension->TimeoutCount);
    }
}


VOID
USBCAMD_ResetWorkItem(
    PVOID Context
    )
/*++

Routine Description:

    Work item executed at passive level to reset the camera

Arguments:

Return Value:

    None.

--*/
{
    PUSBCAMD_WORK_ITEM usbWorkItem = Context;
    PUSBCAMD_CHANNEL_EXTENSION channelExtension;    
    PUSBCAMD_DEVICE_EXTENSION deviceExtension;

    channelExtension = usbWorkItem->ChannelExtension;
    deviceExtension = channelExtension->DeviceExtension;
    
    ASSERT_CHANNEL(channelExtension);

    USBCAMD_ResetChannel(deviceExtension,
                         channelExtension);    

    USBCAMD_ExFreePool(usbWorkItem);
}
