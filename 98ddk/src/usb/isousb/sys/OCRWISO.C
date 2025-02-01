/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

   OcrwIso.c

Abstract:

    Isochronous USB device driver for Intel 82930 USB test board
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
#include "Iso82930.h"




PURB
IsoUsb_BuildIsoRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PUSBD_PIPE_INFORMATION PipeHandle,
    IN BOOLEAN Read
    )
/*++

Routine Description:

     This is the routine used to build a USBD request for IRP_MJ_READ and IRP_MJ_WRITE ioctls.

	 IsoUsb_InitializeStreamUrb(), defined in IsoStrm.c,  performs an analogous role
	 for the continuous stream test ( as initiated by IOCTL_ISOUSB_START_ISO_STREAM )

Arguments:

    DeviceObject - pointer to the device extension for this instance of the
                     82930 device.

    Irp -

    PipeHandle - pointer to USBD-defined pipe descriptor

Return Value:

    initialized iso urb.

--*/
{
    ULONG siz;
    ULONG length, packetSize, numPackets, i;
    PURB urb = NULL;
    PIO_STACK_LOCATION irpSp;
    LARGE_INTEGER byteOffset;

    irpSp = IoGetCurrentIrpStackLocation(Irp);

    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" IsoUsb_BuildIsoRequest handle = 0x%x\n", PipeHandle));

    length = MmGetMdlByteCount(Irp->MdlAddress);

    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" length = 0x%x, decimal %d\n", length, length));

    byteOffset = irpSp->Parameters.Read.ByteOffset;

    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" offset = 0x%08X.%08X  decimal high %08d low %08d\n",
                     byteOffset.HighPart,
                     byteOffset.LowPart,
                     byteOffset.HighPart,
                     byteOffset.LowPart
					 ));

    packetSize = PipeHandle->MaximumPacketSize;
    numPackets = length/packetSize;
    if (numPackets*packetSize < length) {
        numPackets++;
    }

    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" PipeHandle->MaximumPacketSize = 0x%x, decimal %d\n", PipeHandle->MaximumPacketSize, PipeHandle->MaximumPacketSize));
    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" numPackets (length/maxPacketSize) = 0x%x, decimal %d\n", numPackets, numPackets));

    
	siz = GET_ISO_URB_SIZE(numPackets);
    urb = ExAllocatePool(NonPagedPool, siz);

    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" IsoUsb_BuildIsoRequest URB siz = 0x%x decimal %d urb 0x%x\n", siz, siz, urb));

    if (urb) {
        RtlZeroMemory(urb, siz);

        urb->UrbIsochronousTransfer.Hdr.Length = (USHORT) siz;
        urb->UrbIsochronousTransfer.Hdr.Function =
                    URB_FUNCTION_ISOCH_TRANSFER;
        urb->UrbIsochronousTransfer.PipeHandle =
                   PipeHandle->PipeHandle;
        urb->UrbIsochronousTransfer.TransferFlags =
            Read ? USBD_TRANSFER_DIRECTION_IN : 0;

        urb->UrbIsochronousTransfer.TransferBufferMDL =
            Irp->MdlAddress;
        urb->UrbIsochronousTransfer.TransferBufferLength =
            length;

        if (byteOffset.HighPart)
        {
            urb->UrbIsochronousTransfer.StartFrame =
                IsoUsb_GetCurrentFrame(DeviceObject, Irp) +
                byteOffset.LowPart;
        }
        else
        {
            // start sending/receiving right away
            urb->UrbIsochronousTransfer.TransferFlags |=
                USBD_START_ISO_TRANSFER_ASAP;
        }

        urb->UrbIsochronousTransfer.NumberOfPackets = numPackets;
        urb->UrbIsochronousTransfer.UrbLink = NULL;

        for (i=0; i< urb->UrbIsochronousTransfer.NumberOfPackets; i++) {
            urb->UrbIsochronousTransfer.IsoPacket[i].Offset
                        = i * packetSize;
        }

        ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" IsoUsb_BuildIsoRequest Init iso urb Length = 0x%x buf = 0x%x\n",
            urb->UrbIsochronousTransfer.TransferBufferLength,
            urb->UrbIsochronousTransfer.TransferBuffer));
    }

    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" EXIT IsoUsb_BuildIsoRequest()\n"));

    return urb;
}





NTSTATUS
IsoUsb_IsoReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:

     This is the routine used to complete IRP_MJ_READ and IRP_MJ_WRITE ioctls.

	 IsoUsb_IsoIrp_Complete(), defined in IsoStrm.c, performs an analogous role
	 for the continuous stream test ( as initiated by IOCTL_ISOUSB_START_ISO_STREAM )

Arguments:

    DeviceObject - Pointer to the device object for the ISOUSB device.

    Irp - Irp completed.

    Context - Driver defined context.

Return Value:

    The function value is the final status from the operation.

--*/
{
    NTSTATUS			ntStatus = STATUS_SUCCESS;
    PURB				urb = Context;
    PUSBD_PIPE_INFORMATION pipeHandle;
    PFILE_OBJECT		fileObject;
    PIO_STACK_LOCATION	irpStack;
    ULONG               i;

    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    //
    // BUGBUG check here for interesting iso error conditions
    //
    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" IsoUsb_IsoReadWrite_Complete()\n   Iso Completion: StartFrame 0x%08X, Status 0x%08X\n",
                     urb->UrbIsochronousTransfer.StartFrame,
                     urb->UrbHeader.Status));

    for (i=0; i< urb->UrbIsochronousTransfer.NumberOfPackets; i++)
    {
        ISOUSB_KdPrint (  DBGLVL_MAXIMUM , (" IsoUsb_IsoReadWrite_Complete()  [%02d] Length 0x%08X, Status 0x%08X\n",
                         i,
                         urb->UrbIsochronousTransfer.IsoPacket[i].Length,
                         urb->UrbIsochronousTransfer.IsoPacket[i].Status
                        ));

        if (!USBD_SUCCESS(
            urb->UrbIsochronousTransfer.IsoPacket[i].Status)) {

            ISOUSB_KdPrint (  DBGLVL_MEDIUM , (" IsoUsb_IsoReadWrite_Complete() BAD USBD STATUS  [%02d] Length 0x%08X, Status 0x%08X\n",
                             i,
                             urb->UrbIsochronousTransfer.IsoPacket[i].Length,
                             urb->UrbIsochronousTransfer.IsoPacket[i].Status
                            ));
            ntStatus =  STATUS_UNSUCCESSFUL; // set  error return code           
                
        }                
        
    }

    //
    // set the length based on the TransferBufferLength
    // value in the URB
    //
    Irp->IoStatus.Information =
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength;

    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

	// get pipe handle
	pipeHandle = fileObject->FsContext;


    IsoUsb_DecrementIoCount(DeviceObject);                       

    ExFreePool(urb);

    return ntStatus;
}


NTSTATUS
IsoUsb_Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PUSBD_PIPE_INFORMATION pipeHandle;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack, nextStack;
    PDEVICE_EXTENSION deviceExtension;
    PURB urb;
    PISOUSB_RW_CONTEXT context = NULL;

    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" ENTER IsoUsb_Read()\n"));


    IsoUsb_IncrementIoCount(DeviceObject);

    deviceExtension = DeviceObject->DeviceExtension;

    // Can't accept a new io request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !IsoUsb_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        Irp->IoStatus.Status = ntStatus;
        IsoUsb_DecrementIoCount(DeviceObject);
		ISOUSB_KdPrint ( DBGLVL_DEFAULT, ("IsoUsb_Read() can't accept requests, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        IoCompleteRequest (Irp, IO_NO_INCREMENT );
        return ntStatus;
    }
    
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    pipeHandle =  fileObject->FsContext;

    if (!pipeHandle) {
       ntStatus = STATUS_INVALID_HANDLE;
       goto IsoUsb_Read_Reject;
    }

    //
    // submit the write request to USB
    //
	ISOUSB_ASSERT( UsbdPipeTypeIsochronous == pipeHandle->PipeType );

    IsoUsb_ResetPipe(DeviceObject, pipeHandle, FALSE);
    
    urb = IsoUsb_BuildIsoRequest(DeviceObject,
                                 Irp,
                                 pipeHandle,
                                 TRUE);
    if (urb) {

        nextStack = IoGetNextIrpStackLocation(Irp);
        ASSERT(nextStack != NULL);
        ASSERT(DeviceObject->StackSize>1);

        nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        nextStack->Parameters.Others.Argument1 = urb;
        nextStack->Parameters.DeviceIoControl.IoControlCode =
            IOCTL_INTERNAL_USB_SUBMIT_URB;

        IoSetCompletionRoutine(Irp,
                               IsoUsb_IsoReadWrite_Complete,
                               urb,   // pass URB as context
                               TRUE,  // invoke on success
                               TRUE,  // invoke on error
                               TRUE); // invoke on cancel

        ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" IsoUsb_Read() IRP = 0x%x current = 0x%x next = 0x%x\n",
            Irp, irpStack, nextStack));


        ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                Irp);
        goto IsoUsb_Read_Done;
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

IsoUsb_Read_Reject:

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

IsoUsb_Read_Done:

        ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" EXIT IsoUsb_Read()\n" ));
    return ntStatus;

    
}


NTSTATUS
IsoUsb_Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PUSBD_PIPE_INFORMATION pipeHandle;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack, nextStack;
    PDEVICE_EXTENSION deviceExtension;
    PURB urb;
    PISOUSB_RW_CONTEXT context = NULL;

    ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" ENTER IsoUsb_Write()\n"));

    

    IsoUsb_IncrementIoCount(DeviceObject);
    
    deviceExtension = DeviceObject->DeviceExtension;

    // Can't accept a new io request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !IsoUsb_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        Irp->IoStatus.Status = ntStatus;
        IsoUsb_DecrementIoCount(DeviceObject);
		ISOUSB_KdPrint ( DBGLVL_DEFAULT, ("IsoUsb_Write() can't accept requests, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        IoCompleteRequest (Irp, IO_NO_INCREMENT );
        return ntStatus;
    }
   
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;


    pipeHandle =  fileObject->FsContext;
    if (!pipeHandle)
    {
       ntStatus = STATUS_INVALID_HANDLE;
       goto IsoUsb_Write_Reject;
    }

    //
    // submit the write request to USB
    //

    IsoUsb_ResetPipe(DeviceObject, pipeHandle, FALSE);
    
    urb = IsoUsb_BuildIsoRequest(DeviceObject,
                                 Irp,
                                 pipeHandle,
                                 FALSE);
    if (urb) {

        nextStack = IoGetNextIrpStackLocation(Irp);
        ASSERT(nextStack != NULL);
        ASSERT(DeviceObject->StackSize>1);

        nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        nextStack->Parameters.Others.Argument1 = urb;
        nextStack->Parameters.DeviceIoControl.IoControlCode =
            IOCTL_INTERNAL_USB_SUBMIT_URB;

        IoSetCompletionRoutine(Irp,
                               IsoUsb_IsoReadWrite_Complete,
                               urb,   // pass urb as context
                               TRUE,  // invoke on success
                               TRUE,  // invoke on error
                               TRUE); // invoke on cancel

        ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" IRP = 0x%x current = 0x%x next = 0x%x\n",
            Irp, irpStack, nextStack));


        ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                Irp);
        goto IsoUsb_Write_Done;
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }


IsoUsb_Write_Reject:

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

IsoUsb_Write_Done:


    return ntStatus;
}





NTSTATUS
IsoUsb_Close(
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

    ISOUSB_KdPrint( DBGLVL_DEFAULT,("ENTERing IsoUsb_Close()\n"));

    
    IsoUsb_IncrementIoCount(DeviceObject);

    deviceExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    if (fileObject->FsContext) {
        // closing pipe handle
        pipeHandle =  fileObject->FsContext;

		if ( pipeHandle->PipeFlags ) { // set if opened
			// may have been aborted
			ISOUSB_KdPrint( DBGLVL_DEFAULT,("closing pipe %x\n", pipeHandle));
			deviceExtension->OpenPipeCount--;
			pipeHandle->PipeFlags = 0;
		}
		else {
			// pipe was already closed; this can only be if we got a sudden REMOVE_DEVICE
			ISOUSB_ASSERT(  deviceExtension->DeviceRemoved );
			ISOUSB_KdPrint( DBGLVL_DEFAULT,("Pipe %x was already closed \n", pipeHandle));

		}
    }

	IsoUsb_DecrementIoCount(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    ntStatus = Irp->IoStatus.Status;


    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );
                       
	// try to power down device if this is the last pipe
	actStat = IsoUsb_SelfSuspendOrActivate( DeviceObject, TRUE );

    ISOUSB_KdPrint( DBGLVL_DEFAULT,("EXIT IsoUsb_Close() OpenPipeCount = decimal %d, status %x\n",deviceExtension->OpenPipeCount, ntStatus));

    return ntStatus;
}


NTSTATUS
IsoUsb_Create(
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
	PUSBD_PIPE_INFORMATION PipeInfo;
    PUSBD_INTERFACE_INFORMATION interface;


    deviceExtension = DeviceObject->DeviceExtension;
    interface = deviceExtension->UsbInterface;

    ISOUSB_KdPrint( DBGLVL_DEFAULT,("ENTERing IsoUsb_Create()\n"));

    IsoUsb_IncrementIoCount(DeviceObject);

    // Can't accept a new io request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !IsoUsb_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        Irp->IoStatus.Status = ntStatus;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest (Irp,
                           IO_NO_INCREMENT
                          );

        IsoUsb_DecrementIoCount(DeviceObject);                          
        
		ISOUSB_KdPrint( DBGLVL_DEFAULT,("ABORTING IsoUsb_Create\n"));
        return ntStatus;
    }
    
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    // fscontext is null for device
    fileObject->FsContext = NULL;
	nameLen = fileObject->FileName.Length;

    if (nameLen != 0) {

		ISOUSB_KdPrint( DBGLVL_DEFAULT,("IsoUsb_Create fileObject->FileName = %ws\n", fileObject->FileName.Buffer ));

		// Get pipe# to open
		ix = nameLen -1;  // index last char of pipe name

		// if last char isn't digit, decrement till it is
		while( ( (fileObject->FileName.Buffer[ ix ] < (WCHAR) '0') ||
				(fileObject->FileName.Buffer[ ix ] > (WCHAR) '9') ) && ix )
				ix--;  

		ISOUSB_ASSERT( ix ); 
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
					ISOUSB_KdPrint( DBGLVL_DEFAULT,("open pipe %d\n", uval));
					fileObject->FsContext = PipeInfo;
					PipeInfo->PipeFlags = TRUE; // set flag for opened
					ntStatus = STATUS_SUCCESS;

					deviceExtension->OpenPipeCount++;

					// try to power up device if its not in D0
					actStat = IsoUsb_SelfSuspendOrActivate( DeviceObject, FALSE );
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

    IsoUsb_DecrementIoCount(DeviceObject);                               

    ISOUSB_KdPrint( DBGLVL_DEFAULT,("EXIT IsoUsb_Create() %x\n", ntStatus));


    return ntStatus;
}



NTSTATUS
IsoUsb_AbortPipes(
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

			ISOUSB_KdPrint( DBGLVL_HIGH,("IsoUsb_AbortPipes() Aborting open  Pipe %d\n", i));

			urb = ExAllocatePool(NonPagedPool,
								 sizeof(struct _URB_PIPE_REQUEST));

			if (urb) {

				urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
				urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
				urb->UrbPipeRequest.PipeHandle =
					PipeInfo->PipeHandle;

				ntStatus = IsoUsb_CallUSBD(DeviceObject, urb);

				ExFreePool(urb);

			} else {
				ntStatus = STATUS_INSUFFICIENT_RESOURCES;
				ISOUSB_KdPrint( DBGLVL_HIGH,("IsoUsb_AbortPipes() FAILED urb alloc\n" ));
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
IsoUsb_CanAcceptIoRequests(
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

    ISOUSB_KdPrintCond( DBGLVL_MAXIMUM, !fCan, ("**** FALSE return from IsoUsb_CanAcceptIoRequests()!\n"));

	return fCan;
}



//******************************************************************************
//
// IsoUsb_CompletionStop()
//
// IO Completion Routine which just stops further completion of the Irp
//
//******************************************************************************


NTSTATUS
IsoUsb_CompletionStop (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN PVOID            Context
    )

{
    return STATUS_MORE_PROCESSING_REQUIRED;
}

//******************************************************************************
//
// IsoUsb_GetCurrentFrame()
//
//******************************************************************************

ULONG
IsoUsb_GetCurrentFrame (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{
    PDEVICE_EXTENSION           deviceExtension;
    PIO_STACK_LOCATION          nextStack;
    NTSTATUS                    ntStatus;
    struct _URB_GET_CURRENT_FRAME_NUMBER urb;

    deviceExtension   = DeviceObject->DeviceExtension;

    // Initialize the URB
    //
    urb.Hdr.Function = URB_FUNCTION_GET_CURRENT_FRAME_NUMBER;
    urb.Hdr.Length   = sizeof(urb);
    urb.FrameNumber = (ULONG)-1;

    // Set the IRP parameters to pass the URB down the stack
    //
    nextStack = IoGetNextIrpStackLocation(Irp);

    nextStack->Parameters.Others.Argument1 = &urb;

    nextStack->Parameters.DeviceIoControl.IoControlCode = 
        IOCTL_INTERNAL_USB_SUBMIT_URB;                    

    nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

    // Since this Irp is borrowed for URB_FUNCTION_GET_CURRENT_FRAME_NUMBER
    // before it is passed down later for the real URB request after this
    // routine returns, set a completion routine which stop further completion
    // of the Irp.
    //
    IoSetCompletionRoutine(
        Irp,
        IsoUsb_CompletionStop, // this routine does nothing but return STATUS_MORE_PROCESSING_REQUIRED
        NULL,   // Context
        TRUE,   // InvokeOnSuccess
        TRUE,   // InvokeOnError
        TRUE    // InvokeOnCancel
        );

    // Now pass the Irp down the stack
    //
    ntStatus = IoCallDriver(
                   deviceExtension->TopOfStackDeviceObject, 
                   Irp
                   );

    // Don't need to wait for completion because 
    // URB_FUNCTION_GET_CURRENT_FRAME_NUMBER will never return STATUS_PENDING

   ISOUSB_KdPrint (  DBGLVL_MEDIUM, (" IsoUsb_GetCurrentFrame() offset = 0x%08X, decimal %d\n",
                     urb.FrameNumber,
                     urb.FrameNumber));

   return urb.FrameNumber;
}

