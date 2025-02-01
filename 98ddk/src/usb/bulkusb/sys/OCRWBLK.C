/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

   OcrwBulk.c

Abstract:

    Bulk USB device driver for Intel 82930 USB test board
    Read/write io test code

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.


Revision History:

    11/17/97 : created

--*/


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#define DRIVER 

#include "usbdi.h"
#include "usbdlib.h"
#include "Blk82930.h"



NTSTATUS
BulkUsb_StagedReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN BOOLEAN Read
    )
/*++

Routine Description:

    This routine is called by BulkUsb_Read() for IRP_MJ_READ.
    This routine is called by BulkUsb_Write() for IRP_MJ_WRITE.

    Breaks up a read or write in to specified sized chunks,
    as specified by deviceExtension->MaximumTransferSize

Arguments:

    DeviceObject - pointer to our FDO ( Functional Device Object )

    Irp - pointer to the IRP_MJ_READ or IRP_MJ_WRITE

    Read - TRUE for read, FALSE for write


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
	NTSTATUS resetPipeStatus;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack, nextStack;
    PURB urb;
    PIRP irp;
    PMDL mdl; 
    PVOID va;
    CHAR stackSize;
    KIRQL OldIrql;
	BOOLEAN fRes;
    NTSTATUS waitStatus;
    ULONG i, nIrps = 0, totalLength = 0, totalIrpsNeeded, used;
    PUSBD_PIPE_INFORMATION pipeHandle = NULL;
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PUCHAR pCon =  NULL;
	ULONG ChunkSize = deviceExtension->MaximumTransferSize;
	ULONG arraySize;
    PBULKUSB_RW_CONTEXT context = NULL;

    BULKUSB_KdPrint ( DBGLVL_DEFAULT, ("enter BulkUsb_StagedReadWrite()\n"));

    Irp->IoStatus.Information = 0;

    // Can't accept a new io request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !BulkUsb_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        Irp->IoStatus.Status = ntStatus;
		BULKUSB_KdPrint ( DBGLVL_DEFAULT, ("BulkUsb_StagedReadWrite() can't accept requests, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        IoCompleteRequest (Irp, IO_NO_INCREMENT );
        return ntStatus;
    }


    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    pipeHandle =  fileObject->FsContext;

    if (!pipeHandle)
    {
       ntStatus = STATUS_INVALID_HANDLE;
	   BULKUSB_KdPrint ( DBGLVL_DEFAULT, ("BulkUsb_StagedReadWrite() Rejecting on invalid pipeHandle 0x%x decimal %d\n",pipeHandle, pipeHandle ));
       Irp->IoStatus.Status = ntStatus;
       IoCompleteRequest (Irp, IO_NO_INCREMENT );
       return ntStatus;
    }

    //
    // submit the request to USB
    //
	BULKUSB_ASSERT( UsbdPipeTypeBulk == pipeHandle->PipeType );


	if ( Irp->MdlAddress )
		totalLength = MmGetMdlByteCount(Irp->MdlAddress);


	BULKUSB_KdPrint ( DBGLVL_DEFAULT, ("BulkUsb_StagedReadWrite() totalLength = decimal %d, Irp->MdlAddress = 0x%x\n",totalLength, Irp->MdlAddress ));

    if ( 0 == totalLength ) {
		// allow 0-len read or write; just return success
        ntStatus = STATUS_SUCCESS;
        Irp->IoStatus.Status = ntStatus;
		BULKUSB_KdPrint ( DBGLVL_DEFAULT, ("BulkUsb_StagedReadWrite() returning quick success on zero-len read/write request\n"));

        IoCompleteRequest (Irp, IO_NO_INCREMENT );
        return ntStatus;
    }
    

	// more memory than is on our test device?
 	if ( totalLength > BULKUSB_TEST_BOARD_TRANSFER_BUFFER_SIZE )
	{
       ntStatus = STATUS_INVALID_PARAMETER;
	   BULKUSB_KdPrint ( DBGLVL_DEFAULT, ("BulkUsb_StagedReadWrite() Rejecting on too large request 0x%x decimal %d\n",totalLength, totalLength ));
       Irp->IoStatus.Status = ntStatus;
       IoCompleteRequest (Irp, IO_NO_INCREMENT );
       return ntStatus;
	}

    // calculate total # of staged irps that will be needed
	totalIrpsNeeded =  totalLength / deviceExtension->MaximumTransferSize ;
	if ( totalLength % deviceExtension->MaximumTransferSize )
		totalIrpsNeeded++;


	BULKUSB_ASSERT( !deviceExtension->PendingIoIrps );  // this should have been cleaned up last time
	BULKUSB_ASSERT( !deviceExtension->BaseIrp );  // this should have been cleaned up last time

    used = 0;
	// alloc one extra for termination
	arraySize =  ( totalIrpsNeeded +1 ) * sizeof(BULKUSB_RW_CONTEXT);

    // allocate space for an array of BULKUSB_RW_CONTEXT structs for the staged irps
    deviceExtension->PendingIoIrps = BULKUSB_ExAllocatePool(NonPagedPool, arraySize );
    
    if ( !deviceExtension->PendingIoIrps ) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        BULKUSB_KdPrint ( DBGLVL_DEFAULT, ("BulkUsb_StagedReadWrite() !deviceExtension->PendingIoIrps STATUS_INSUFFICIENT_RESOURCES\n"));
        Irp->IoStatus.Status = ntStatus;
        IoCompleteRequest (Irp, IO_NO_INCREMENT );
        return ntStatus;
    }

    RtlZeroMemory(deviceExtension->PendingIoIrps, arraySize );

    // init ptr to 1st BULKUSB_RW_CONTEXT struct in array
    pCon =  (PUCHAR) deviceExtension->PendingIoIrps;

    deviceExtension->BaseIrp = Irp; // this is the original user's irp
    deviceExtension->StagedBytesTransferred = 0;
	deviceExtension->StagedPendingIrpCount = totalIrpsNeeded;

    // we need to build a series of irps & urbs to represent 
    // this request.

    while (NT_SUCCESS(ntStatus) ) {
        context =  (PBULKUSB_RW_CONTEXT) pCon;
        irp = NULL;
        urb = NULL;
        mdl = NULL;

        if ( !BulkUsb_CanAcceptIoRequests( DeviceObject ) ) {
            // got sudden remove! ( i.e. plug was yanked )
            ntStatus = STATUS_DELETE_PENDING;
            Irp->IoStatus.Status = ntStatus;
		    BULKUSB_KdPrint ( DBGLVL_MAXIMUM, ("BulkUsb_StagedReadWrite() got sudden remove, breaking out of URB-building loop\n"));
            break;
        }

        stackSize = (CCHAR)(deviceExtension->TopOfStackDeviceObject->StackSize + 1);
        irp = IoAllocateIrp(stackSize, FALSE);
        
        // Get the virtual address for the buffer described by 
        // our original input Irp's MDL. 
        va = MmGetMdlVirtualAddress(Irp->MdlAddress);

        if (irp) {
           // Each new Irp will 'see' the entire buffer, but map it's IO location
           // to a single ChunkSize section within it via IoBuildPartialMdl()
           mdl = IoAllocateMdl(va,
                                totalLength,
                                FALSE,
                                FALSE,
                                irp);
        }                                    
                            
        if (mdl) {        
			// see if we're done yet
			if( ( used + ChunkSize ) > totalLength  ) {
				// make sure to truncate last transfer if neccy
				ChunkSize = totalLength - used;

			}

            // Map the sub-area of the full user buffer this staged Irp will be using for IO
            IoBuildPartialMdl(Irp->MdlAddress, // Points to an MDL describing the original buffer,
                                               // of which a subrange is to be mapped
                              mdl,             // our allocated target mdl
                              (PUCHAR)va + used, // base virtual address of area to be mapped
                              ChunkSize);      // size of area to be mapped

            used+=ChunkSize;
                                              
            urb = BulkUsb_BuildAsyncRequest(DeviceObject,
                                           irp,
                                           pipeHandle,
                                           Read);
        }
        
        if (urb && irp && mdl) {

            context->Urb = urb;
            context->DeviceObject = DeviceObject;
            context->Irp = irp;
			nIrps++;
        
            // IoGetNextIrpStackLocation gives a higher level driver access to the next-lower 
            // driver's I/O stack location in an IRP so the caller can set it up for the lower driver.
            nextStack = IoGetNextIrpStackLocation(irp);
            BULKUSB_ASSERT(nextStack != NULL);
            BULKUSB_ASSERT(DeviceObject->StackSize>1);

            nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
            nextStack->Parameters.Others.Argument1 = urb;
            nextStack->Parameters.DeviceIoControl.IoControlCode =
                IOCTL_INTERNAL_USB_SUBMIT_URB;

            IoSetCompletionRoutine(irp,
                    BulkUsb_AsyncReadWrite_Complete,
                    context, // pass the context array element to completion routine
                    TRUE,    // invoke on success
                    TRUE,    // invoke on error
                    TRUE);   // invoke on cancellation of the Irp

            BULKUSB_KdPrint ( DBGLVL_MAXIMUM, ("BulkUsb_StagedReadWrite() created staged irp #%d %x\n", nIrps, irp));
                                   
            // We keep an array of all pending read/write Irps; we may have to cancel
            // them explicitly on sudden device removal or other error
			(( PBULKUSB_RW_CONTEXT) pCon)->Irp = irp;

            BulkUsb_IncrementIoCount(DeviceObject);

			ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject,
             (( PBULKUSB_RW_CONTEXT) pCon)->Irp);


        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
			BULKUSB_KdPrint ( DBGLVL_DEFAULT, ("BulkUsb_StagedReadWrite() Dumped from irp loop on failed Irp or urb allocate\n"));
            break;
        }

        if (used >= totalLength) {
            break;      // we're done
        }


        // point to next BULKUSB_RW_CONTEXT struct
        pCon +=  sizeof(BULKUSB_RW_CONTEXT);

    } // end while


    Irp->IoStatus.Status = ntStatus;

    if (!NT_SUCCESS(ntStatus)) {

        BULKUSB_KdPrint ( DBGLVL_DEFAULT, ("BulkUsb_StagedReadWrite() FAILED, ntStatus = 0x%x\n", ntStatus));

	    // try to reset the pipe on error ( unless device has been suddenly removed )
	    if ( pipeHandle  && BulkUsb_CanAcceptIoRequests( DeviceObject ) ) {

		    resetPipeStatus = BulkUsb_ResetPipe(DeviceObject, pipeHandle );

		    BULKUSB_KdPrint( DBGLVL_DEFAULT, ("BulkUsb_StagedReadWrite() Tried to reset pipe 0x%x, Status = 0x%x\n", pipeHandle, resetPipeStatus));
		    BULKUSB_KdPrintCond ( DBGLVL_DEFAULT, (!NT_SUCCESS(resetPipeStatus)), ("BulkUsb_StagedReadWrite() BulkUsb_ResetPipe() FAILED\n"));

		    if( !NT_SUCCESS(resetPipeStatus) ) {
			    // if can't reset pipe, try to reset device ( parent port )
			    BULKUSB_KdPrint( DBGLVL_DEFAULT, ("Will try to reset device \n"));

			    resetPipeStatus = BulkUsb_ResetDevice(DeviceObject);

			    BULKUSB_KdPrintCond ( DBGLVL_DEFAULT, (!NT_SUCCESS(resetPipeStatus)), ("BulkUsb_StagedReadWrite() BulkUsb_ResetDevice() FAILED\n"));
		    }
	    }
    } // end, if !NT_SUCCESS( ntStatus )


    if ( 0 == nIrps ) {
        // only complete the request here if we created no staged irps
		BULKUSB_KdPrint ( DBGLVL_HIGH, ("BulkUsb_StagedReadWrite() 0 irps staged, completing  base IRP now!\n"));
        IoCompleteRequest (Irp, IO_NO_INCREMENT  );
    } else {
		BULKUSB_KdPrint ( DBGLVL_HIGH, ("BulkUsb_StagedReadWrite() %d irps staged\n", nIrps));

        if ( deviceExtension->BaseIrp ) {
            //
            // Mark the original input Irp pending; it will be completed when the last staged irp
            //  is handled ( in BulkUsb_AsyncReadWrite_Complete() ).
            //
		    BULKUSB_KdPrint ( DBGLVL_HIGH, ("BulkUsb_StagedReadWrite(),marking base IRP  0x%x pending!\n", Irp));
            BULKUSB_ASSERT( Irp == deviceExtension->BaseIrp );
            ntStatus = STATUS_PENDING;
            Irp->IoStatus.Status = ntStatus;
            IoMarkIrpPending(Irp);
        } else {
            // It is possible for BulkUsb_AsyncReadWrite_Complete() to have completed the
            //  original irp before we even get here! 
            // If this happens, it will have NULLED-out deviceExtension->BaseIrp.
            ntStatus = STATUS_SUCCESS;
        }
    }

    BULKUSB_KdPrint ( DBGLVL_HIGH, ("BulkUsb_StagedReadWrite() StagedReadWrite  ntStatus = 0x%x decimal %d\n", ntStatus, ntStatus));
    BULKUSB_KdPrint ( DBGLVL_HIGH, ("EXIT BulkUsb_StagedReadWrite() gExAllocCount = dec %d\n", gExAllocCount ));

    return ntStatus;
}



PURB
BulkUsb_BuildAsyncRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PUSBD_PIPE_INFORMATION PipeHandle,
    IN BOOLEAN Read
    )
/*++

Routine Description:

    Called from BulkUsb_StageReadWrite() for IRP_MJ_READ or IRP_MJ_WRITE

Arguments:

    DeviceObject - pointer to the FDO ( Functional Device Object )

    Irp - A staged IRP allocated and mapped by this driver in BulkUsb_StageReadWrite()
          to perform a single deviceExtension->MaximumTransferSize IO request

    PipeHandle - handle to the endpoint we're reading or writing

    Read - TRUE for reads, FALSE for writes

Return Value:

    ptr to initialized async urb. ( USB Request Block )

--*/
{
    ULONG siz;
    ULONG length;
    PURB urb = NULL;

    length = MmGetMdlByteCount(Irp->MdlAddress);

    siz = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
    urb = BULKUSB_ExAllocatePool(NonPagedPool, siz);

    BULKUSB_KdPrint( DBGLVL_MAXIMUM,("Enter BulkUsb_BuildAsyncRequest() len = 0x%x decimal %d \n siz = 0x%x urb 0x%x\n Pipehandle 0x%x\n", length, length, siz, urb, PipeHandle));

    if (urb) {
        RtlZeroMemory(urb, siz);

        urb->UrbBulkOrInterruptTransfer.Hdr.Length = (USHORT) siz;
        urb->UrbBulkOrInterruptTransfer.Hdr.Function =
                    URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER;
        urb->UrbBulkOrInterruptTransfer.PipeHandle =
                   PipeHandle->PipeHandle;
        urb->UrbBulkOrInterruptTransfer.TransferFlags =
            Read ? USBD_TRANSFER_DIRECTION_IN : 0;

        // short packet is not treated as an error.
        urb->UrbBulkOrInterruptTransfer.TransferFlags |= 
            USBD_SHORT_TRANSFER_OK;            
                
        //
        // not using linked urb's
        //
        urb->UrbBulkOrInterruptTransfer.UrbLink = NULL;

        urb->UrbBulkOrInterruptTransfer.TransferBufferMDL =
            Irp->MdlAddress;
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength =
            length;

        BULKUSB_KdPrint( DBGLVL_MAXIMUM,("BulkUsb_BuildAsyncRequest() Init async urb Length = 0x%x decimal %d, buf = 0x%x\n",
            urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
            urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
            urb->UrbBulkOrInterruptTransfer.TransferBuffer));
    }

    BULKUSB_KdPrint( DBGLVL_MAXIMUM,("exit BulkUsb_BuildAsyncRequest\n"));

    return urb;
}


NTSTATUS
BulkUsb_AsyncReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:

  Completion routine for our staged read/write Irps


Arguments:

    DeviceObject - Pointer to the device object for next lower device
	in the  driver stack; 

    Irp - Irp completed.

    Context - Driver defined context.

Return Value:

    The function value is the final status from the operation.

--*/
{
    NTSTATUS			ntStatus = STATUS_SUCCESS;
    PURB				urb;
    PBULKUSB_RW_CONTEXT context = Context;
    PIO_STACK_LOCATION	irpStack;
    PDEVICE_OBJECT      deviceObject;
	PDEVICE_EXTENSION   deviceExtension;

	// We have to get the deviceObject from the context, since the DeviceObject passed in
	//  here belongs to the next lower driver in the stack because we were invoked via
	//   IoCallDriver in BulkUsb_StagedReadWrite()
    deviceObject = context->DeviceObject;
	deviceExtension = deviceObject->DeviceExtension;

    //  If the lower driver returned PENDING, mark our stack location as pending also.
    if ( Irp->PendingReturned ) {  
        IoMarkIrpPending(Irp);
    }

	BULKUSB_ASSERT( deviceExtension->PendingIoIrps );  
	BULKUSB_ASSERT( deviceExtension->BaseIrp );  

	BULKUSB_ASSERT( context->Irp == Irp ); 
    
    urb = context->Urb;

    
    BULKUSB_KdPrint( DBGLVL_MAXIMUM,  ("\n\n ENTER BulkUsb_AsyncReadWrite_Complete():  Length 0x%08X decimal %d\n   Status 0x%08X\n",
                     urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
                     urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
                     urb->UrbHeader.Status));

    // decrement count of staged pending irps
    deviceExtension->StagedPendingIrpCount--;

    // decrement the driver's overall pending irp count
    BulkUsb_DecrementIoCount(deviceObject);
    
    // 
    // IoCallDriver has been called on this Irp;
    // Set the length based on the TransferBufferLength
    // value in the URB
    //
    Irp->IoStatus.Information =
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength;

    ntStatus = STATUS_MORE_PROCESSING_REQUIRED;

    deviceExtension->StagedBytesTransferred += 
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength; 
    
    BULKUSB_KdPrint ( DBGLVL_MAXIMUM,("BulkUsb_AsyncReadWrite_Complete(): Staged Async Completion %d, bytes = %d\n", 
        deviceExtension->StagedPendingIrpCount,
        deviceExtension->StagedBytesTransferred)); 

    IoFreeIrp(context->Irp);
    context->Irp = NULL; 


    if (deviceExtension->StagedPendingIrpCount == 0) {
        
		BULKUSB_KdPrint ( DBGLVL_HIGH,("BulkUsb_AsyncReadWrite_Complete(): StagedPendingIrpCount == 0, completeting BaseIrp 0x%x\n    Total bytes xferred = 0x%x, decimal %d\n", deviceExtension->BaseIrp, deviceExtension->StagedBytesTransferred, deviceExtension->StagedBytesTransferred));

        deviceExtension->BaseIrp->IoStatus.Status = STATUS_SUCCESS; 

        deviceExtension->BaseIrp->IoStatus.Information = 
            deviceExtension->StagedBytesTransferred;

        IoCompleteRequest(deviceExtension->BaseIrp,
                          IO_NO_INCREMENT);

        BULKUSB_ExFreePool( deviceExtension->PendingIoIrps ); 
		deviceExtension->PendingIoIrps  = NULL;
		deviceExtension->BaseIrp = NULL;

        // the event is only waited on if BulkUsb_CancelPendingIo() has been called
        KeSetEvent(&deviceExtension->StagingDoneEvent, 1, FALSE);
    }
    BULKUSB_ExFreePool(urb);

    BULKUSB_KdPrint ( DBGLVL_HIGH, ("Exit BulkUsb_AsyncReadWrite_Complete() gExAllocCount = dec %d\n", gExAllocCount ));
    BULKUSB_KdPrint ( DBGLVL_MAXIMUM,("Exit BulkUsb_AsyncReadWrite_Complete(), ntStatus = 0x%x\n\n",ntStatus )); 
    return ntStatus;
}





NTSTATUS
BulkUsb_Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

	This is the IRP_MJ_READ routine set in our dispatch table;
	ReadFile() calls from user mode ultimately land here

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.

    IRP - pointer to the IRP_MJ_READ

Return Value:

    NT status code

--*/
{

    NTSTATUS ntStatus = BulkUsb_StagedReadWrite(DeviceObject,
                                  Irp,
                                  TRUE);	// false to write, true to read

    return ntStatus;                                  

}

NTSTATUS
BulkUsb_Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:
	This is the IRP_MJ_WRITE routine set in our dispatch table;
	WriteFile() calls from user mode ultimately land here

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.

    IRP - pointer to the IRP_MJ_WRITE

Return Value:

    NT status code

--*/
{

    NTSTATUS ntStatus = BulkUsb_StagedReadWrite(DeviceObject,
                                  Irp,
                                  FALSE);	// false to write, true to read

    return ntStatus;                                  

}



NTSTATUS
BulkUsb_Close(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This is the dispatch table routine for IRP_MJ_CLOSE.
    It handles user mode CloseHandle() calls for a pipe
    It closes the File Object for the pipe handle it represents.

Arguments:

    DeviceObject - pointer to our FDO (Functional Device Object )


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
	NTSTATUS actStat;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    PUSBD_PIPE_INFORMATION pipeHandle = NULL;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("entering BulkUsb_Close\n"));

    
    BulkUsb_IncrementIoCount(DeviceObject);

    deviceExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    if (fileObject->FsContext) {
        // closing pipe handle
        pipeHandle =  fileObject->FsContext;

		if ( pipeHandle->PipeFlags ) { // set if opneed
			// may have been aborted
			BULKUSB_KdPrint( DBGLVL_DEFAULT,("closing pipe %x\n", pipeHandle));
			deviceExtension->OpenPipeCount--;
			pipeHandle->PipeFlags = 0;
		}
		else {
			// pipe was already closed; this can only be if we got a sudden REMOVE_DEVICE
			BULKUSB_ASSERT(  deviceExtension->DeviceRemoved );
			BULKUSB_KdPrint( DBGLVL_DEFAULT,("Pipe %x was already closed \n", pipeHandle));

		}
    }

	BulkUsb_DecrementIoCount(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    ntStatus = Irp->IoStatus.Status;


    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );
                       
	// try to power down device if this is the last pipe
	actStat = BulkUsb_SelfSuspendOrActivate( DeviceObject, TRUE );

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("exit BulkUsb_Close OpenPipeCount = decimal %d, status %x\n",deviceExtension->OpenPipeCount, ntStatus));

    return ntStatus;
}


NTSTATUS
BulkUsb_Create(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This is the dispatch table routine for IRP_MJ_CREATE.
    It's the entry point for CreateFile() calls
    user mode apps may open "<name genned fron GUID>.\yy"
    where yy is the internal pipe id

Arguments:

    DeviceObject - pointer to our FDO ( Functional Device Object )


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    ULONG i, nameLen, ix, uval, umultiplier;
	NTSTATUS actStat;
    PUSBD_INTERFACE_INFORMATION interface;
	PUSBD_PIPE_INFORMATION PipeInfo;

    deviceExtension = DeviceObject->DeviceExtension;
    interface = deviceExtension->UsbInterface;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("entering BulkUsb_Create\n"));

    BulkUsb_IncrementIoCount(DeviceObject);

    // Can't accept a new io request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !BulkUsb_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        Irp->IoStatus.Status = ntStatus;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest (Irp,
                           IO_NO_INCREMENT
                          );

        BulkUsb_DecrementIoCount(DeviceObject);                          
        
		BULKUSB_KdPrint( DBGLVL_DEFAULT,("ABORTING BulkUsb_Create\n"));
        return ntStatus;
    }
    
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    // fscontext is null for device
    fileObject->FsContext = NULL;
	nameLen = fileObject->FileName.Length;

    if (nameLen != 0) {

		BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_Create fileObject->FileName = %ws\n", fileObject->FileName.Buffer ));

		// Get pipe# to open
		ix = nameLen -1;  // index last char of pipe name

		// if last char isn't digit, decrement till it is
		while( ( (fileObject->FileName.Buffer[ ix ] < (WCHAR) '0') ||
				(fileObject->FileName.Buffer[ ix ] > (WCHAR) '9') ) && ix )
				ix--;  

		BULKUSB_ASSERT( ix ); 
		if (  !ix  )  {  //  filename better have had at least one ascii digit!    
			ntStatus = STATUS_INVALID_PARAMETER;
		} else {

			//
			// A name was specified, convert it to a pipe id.
			// Parse the ansi ascii decimal 0-based pipe number 
			//
			uval = 0;
			umultiplier = 1;
			// we're traversing least-to-most significant digits
			while( ( (fileObject->FileName.Buffer[ ix ] >= (WCHAR) '0') &&
				(fileObject->FileName.Buffer[ ix ] <= (WCHAR) '9') ) && ix ) {

				uval +=  (umultiplier *
					     (ULONG) (fileObject->FileName.Buffer[ ix ] - (WCHAR) '0'));
				ix--;
				umultiplier *= 10; 
			}

			// init status to bad; will set good in below loop on success
			ntStatus = STATUS_INSUFFICIENT_RESOURCES;

			for (i=0; i<interface->NumberOfPipes; i++) {

				PipeInfo =  &interface->Pipes[i]; // PUSBD_PIPE_INFORMATION  PipeInfo;

				// find the corresponding unopened pipe
				if ( ( uval == i) && !PipeInfo->PipeFlags) { // PipeFlags set if open
					//
					// found a match
					//
					BULKUSB_KdPrint( DBGLVL_DEFAULT,("open pipe %d\n", uval));
					fileObject->FsContext = PipeInfo;
					PipeInfo->PipeFlags = TRUE; // set flag for opened
					ntStatus = STATUS_SUCCESS;

					deviceExtension->OpenPipeCount++;

					// try to power up device if its not in D0
					actStat = BulkUsb_SelfSuspendOrActivate( DeviceObject, FALSE );
					break;
				}
			}
		}

	} // if ix

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;


    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

    BulkUsb_DecrementIoCount(DeviceObject);                               

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("exit BulkUsb_Create %x\n", ntStatus));


    return ntStatus;
}




BOOLEAN
BulkUsb_CancelPendingIo(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:
	Cancels pending IO, as on a sudden IRP_MN_REMOVE_DEVICE 
	This driver maintains and array of info structs (BULKUSB_RW_CONTEXT)
	on self-generated IRPS for staged read/writes; This routine traverses
	it and cancels all pending IO irps

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.


Return Value:

    TRUE if cancelled any, else FALSE

--*/
{
	PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PUCHAR pCon =  (PUCHAR) deviceExtension->PendingIoIrps;
	ULONG i = 0;
	PIRP Irp;
	USHORT uDriverCancel = 0;  // count cancelled via iocancelirp()
	BOOLEAN cRes; 
    NTSTATUS ntStatus, waitStatus;

	// nothing pending
	if ( !deviceExtension->PendingIoIrps )
		return FALSE;


    BULKUSB_KdPrint ( DBGLVL_MAXIMUM, ("enter BulkUsb_CancelPendingIo()\n"));


    // the BULKUSB_RW_CONTEXT array is terminated by an entry with a NULL Irp
	for ( i = 0; ((PBULKUSB_RW_CONTEXT)pCon)->Irp ;  i++ ) {

		Irp = ((PBULKUSB_RW_CONTEXT) pCon)->Irp;
		//
		// Since IoCallDriver has been called on this request, we call IoCancelIrp
		//  and let our completion routine handle it
		//
		cRes = IoCancelIrp( Irp );

		BULKUSB_KdPrint ( DBGLVL_MAXIMUM,  ("BulkUsb_CancelPendingIo() IoCancelIrp() cRes=%d, IRP 0x%x\n", cRes, Irp));

		// if cancel call failed, they all will, so dump out
		if ( !cRes )
			break;

        uDriverCancel++; // flag we tried to cancel at least one


        // point to next context struct in array
        pCon +=  sizeof( BULKUSB_RW_CONTEXT);
    } // end, for


    if ( uDriverCancel && cRes ) {

		// We only get here if we cancelled at least one and all cancellations were successfull.
        // Wait on the event set on last cancel in BulkUsb_AsyncReadWriteComplete();
	    BULKUSB_KdPrint ( DBGLVL_MAXIMUM, ("BulkUsb_CancelPendingIo() before waiting for StagingDoneEvent()\n" ));
        waitStatus = KeWaitForSingleObject(
                       &deviceExtension->StagingDoneEvent,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);
	    BULKUSB_KdPrint ( DBGLVL_MAXIMUM, ("BulkUsb_CancelPendingIo() finished waiting for StagingDoneEvent()\n" ));
    }

	BULKUSB_KdPrintCond ( DBGLVL_HIGH, uDriverCancel,
        ("BulkUsb_CancelPendingIo() cancelled %d via IoCancelIrp()\n",uDriverCancel));

    return (BOOLEAN) uDriverCancel;                                  

}



NTSTATUS
BulkUsb_AbortPipes(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

	Called as part of sudden device removal handling.
    Cancels any pending transfers for all open pipes. 
	If any pipes are still open, call USBD with URB_FUNCTION_ABORT_PIPE
	Also marks the pipe 'closed' in our saved  configuration info.

Arguments:

    Ptrs to our FDO

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb;
    PDEVICE_EXTENSION deviceExtension;
	ULONG i;

    PUSBD_INTERFACE_INFORMATION interface;
	PUSBD_PIPE_INFORMATION PipeInfo;

    deviceExtension = DeviceObject->DeviceExtension;
    interface = deviceExtension->UsbInterface;

    for (i=0; i<interface->NumberOfPipes; i++) {

        PipeInfo =  &interface->Pipes[i]; // PUSBD_PIPE_INFORMATION  PipeInfo;

		if ( PipeInfo->PipeFlags ) { // we set this if open, clear if closed

			BULKUSB_KdPrint( DBGLVL_HIGH,("BulkUsb_AbortPipes() Aborting open  Pipe %d\n", i));

			urb = BULKUSB_ExAllocatePool(NonPagedPool,
								 sizeof(struct _URB_PIPE_REQUEST));

			if (urb) {

				urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
				urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
				urb->UrbPipeRequest.PipeHandle =
					PipeInfo->PipeHandle;

				ntStatus = BulkUsb_CallUSBD(DeviceObject, urb);

				BULKUSB_ExFreePool(urb);

			} else {
				ntStatus = STATUS_INSUFFICIENT_RESOURCES;
				BULKUSB_KdPrint( DBGLVL_HIGH,("BulkUsb_AbortPipes() FAILED urb alloc\n" ));
				break;
			}


			if (!(NT_SUCCESS(ntStatus))) {
				// if we failed, dump out
#if DBG
				if ( gpDbg )
					gpDbg->PipeErrorCount++;
#endif
				break;
			}
			else {
				PipeInfo->PipeFlags = FALSE; // mark the pipe 'closed'

				deviceExtension->OpenPipeCount--;
#if DBG
				if ( gpDbg )
					gpDbg->AbortPipeCount++;
#endif


			}

		} // end, if pipe open
	} // end, for all pipes


    return ntStatus;
}



BOOLEAN
BulkUsb_CanAcceptIoRequests(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

  Check device extension status flags; 

     Can't accept a new io request if device:
      1) is removed, 
      2) has never been started, 
      3) is stopped,
      4) has a remove request pending, or
      5) has a stop device pending


Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.


Return Value:

    return TRUE if can accept new io requests, else FALSE

--*/
{
    PDEVICE_EXTENSION deviceExtension;
	BOOLEAN fCan = FALSE;

    deviceExtension = DeviceObject->DeviceExtension;

	//flag set when processing IRP_MN_REMOVE_DEVICE
    if ( !deviceExtension->DeviceRemoved &&
		 // device must be started( enabled )
		 deviceExtension->DeviceStarted &&
 		 // flag set when driver has answered success to IRP_MN_QUERY_REMOVE_DEVICE
		 !deviceExtension->RemoveDeviceRequested &&
		 // flag set when driver has answered success to IRP_MN_QUERY_STOP_DEVICE
		 !deviceExtension->StopDeviceRequested ){
			fCan = TRUE;
	}

    BULKUSB_KdPrintCond( DBGLVL_MAXIMUM, !fCan, ("**** FALSE return from BulkUsb_CanAcceptIoRequests()!\n"));

	return fCan;
}

