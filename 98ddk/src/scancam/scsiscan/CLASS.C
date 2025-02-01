/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:
    class.c

Abstract:
    Contains a subset of routines in classpnp.sys.

Author:
    Ray Patrick (raypat)

Environment:
    kernel mode only

Notes:

Revision History:

--*/
#include <stdio.h>
#include "stddef.h"
#include "wdm.h"
#include "scsi.h"
#include "ntddstor.h"
#include "ntddscsi.h"
#include "scsiscan.h"
#include "private.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, ClassGetDescriptor)
#endif

NTSTATUS
ClassGetDescriptor(
    IN PDEVICE_OBJECT pDeviceObject,
    IN PSTORAGE_PROPERTY_ID pPropertyId,
    OUT PSTORAGE_DESCRIPTOR_HEADER *pDescriptor
    )
/*++

Routine Description:
    This routine will perform a query for the specified property id and will
    allocate a non-paged buffer to store the data in.  It is the responsibility
    of the caller to ensure that this buffer is freed.

    This routine must be run at IRQL_PASSIVE_LEVEL

Arguments:
    pDeviceObject - the device to query
    pDescriptor   - a location to store a pointer to the buffer we allocate

Return Value:
    Status
    if status is unsuccessful *DeviceInfo will be set to 0

--*/
{
    PIRP					   pIrp;
    PKEVENT					   pEvent;
    STORAGE_PROPERTY_QUERY	   Query;
    ULONG					   Buffer[2];
    PSTORAGE_DESCRIPTOR_HEADER pLocalDescriptor;
    ULONG					   Length;
    IO_STATUS_BLOCK			   StatusBlock;
    NTSTATUS				   Status;
    UCHAR					   Pass;

    //
    // Set the descriptor pointer to NULL and 
    // Initialize the event we're going to wait on.
    //

    *pDescriptor = NULL;
    pEvent = ExAllocatePool(NonPagedPool, sizeof(KEVENT));

    if(pEvent == NULL) {
		SCSISCAN_KdPrint(MAX_TRACE,("ClassGetDescriptor: Unable to allocate event\n"));
        Status = STATUS_INSUFFICIENT_RESOURCES;
        return Status;
    }
    KeInitializeEvent(pEvent, SynchronizationEvent, FALSE);

	Pass = 0;

    __try {

        //
        // Retrieve the property page
        //

		do {
			
			RtlZeroMemory(&Query, sizeof(STORAGE_PROPERTY_QUERY));
			Query.PropertyId = *pPropertyId;
			Query.QueryType = PropertyStandardQuery;

			switch( Pass ) {
				case 0:

                    //
					// On the first pass we just want to get the first few
					// bytes of the descriptor so we can read it's size
                    //

					pLocalDescriptor = (PVOID) &Buffer[0];
					Length = sizeof(ULONG) * 2;
					break;

				case 1:

                    //
					// This time we know how much data there is so we can
					// allocate a buffer of the correct size
                    //

					Length = ((PSTORAGE_DESCRIPTOR_HEADER) pLocalDescriptor)->Size;
					pLocalDescriptor = ExAllocatePool(NonPagedPool, Length);
					if (pLocalDescriptor == NULL) {
						Status = STATUS_INSUFFICIENT_RESOURCES;
						SCSISCAN_KdPrint(MAX_TRACE,
										 ("ClassGetDescriptor: unable to get memory for descriptor (%d bytes)\n",
										  Length));
						__leave;
					}
					break;
			}

            //
			// Build the query irp and wait for it to complete (if necessary)
            //

			pIrp = IoBuildDeviceIoControlRequest(
				IOCTL_STORAGE_QUERY_PROPERTY,
				pDeviceObject,
				&Query,
				sizeof(STORAGE_PROPERTY_QUERY),
				pLocalDescriptor,
				Length,
				FALSE,
				pEvent,
				&StatusBlock);

			if (pIrp == NULL) {
				SCSISCAN_KdPrint(MAX_TRACE,("ClassGetDescriptor: unable to allocate irp\n"));
				Status = STATUS_INSUFFICIENT_RESOURCES;
				__leave;
			}

			Status = IoCallDriver(pDeviceObject, pIrp);

			if (Status == STATUS_PENDING) {
				KeWaitForSingleObject(pEvent,
									  Executive,
									  KernelMode,
									  FALSE,
									  NULL);

				Status = StatusBlock.Status;
			}

			if (!NT_SUCCESS(Status)) {
				SCSISCAN_KdPrint(MAX_TRACE,
								 ("ClassGetDescriptor: error %#08lx trying to query properties\n",
								  Status));
				__leave;
			}

		} while (Pass++ != 1);

	} __finally {
		
		ExFreePool(pEvent);

		if((Pass != 0) && !NT_SUCCESS(Status)) {
			ExFreePool(pLocalDescriptor);
			pLocalDescriptor = NULL;
		} else {
			*pDescriptor = pLocalDescriptor;
		}
	}
	return Status;
}


BOOLEAN
ClassInterpretSenseInfo(
    IN PDEVICE_OBJECT pDeviceObject,
    IN PSRB pSrb,
    IN UCHAR MajorFunctionCode,
    IN ULONG IoDeviceCode,
    IN ULONG RetryCount,
    OUT NTSTATUS *Status
    )
/*++

Routine Description:
    This routine interprets the data returned from the SCSI
    request sense. It determines the status to return in the
    IRP and whether this request can be retried.

Arguments:
    pDeviceObject - Supplies the device object associated with this request.
    pSrb - Supplies the scsi request block which failed.
    MajorFunctionCode - Supplies the function code to be used for logging.
    IoDeviceCode - Supplies the device code to be used for logging.
    Status - Returns the status for the request.

Return Value:
    BOOLEAN TRUE: Drivers should retry this request.
            FALSE: Drivers should not retry this request.

--*/
{
    PSCSISCAN_DEVICE_EXTENSION      pde;        
    PSENSE_DATA                     pSenseBuffer;
    BOOLEAN                         Retry;
    BOOLEAN                         LogError;
    ULONG                           BadSector;
    ULONG                           UniqueId;
    NTSTATUS                        LogStatus;
    ULONG                           ReadSector;
    ULONG                           Index;
    PIO_ERROR_LOG_PACKET            pErrorLogEntry;

    pde = (PSCSISCAN_DEVICE_EXTENSION)pDeviceObject -> DeviceExtension;
    pSenseBuffer = pSrb -> SenseInfoBuffer;
    Retry = TRUE;
    LogError = FALSE;
    BadSector = 0;
    
    //
    // Check that request sense buffer is valid.
    //
    
    if (pSrb -> SrbStatus & SRB_STATUS_AUTOSENSE_VALID &&
        pSrb -> SenseInfoBufferLength >= offsetof(SENSE_DATA, CommandSpecificInformation)) {
        SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Error code is %x\n",
                                    pSenseBuffer -> ErrorCode));
        SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Sense key is %x\n",
                                    pSenseBuffer -> SenseKey));
        SCSISCAN_KdPrint(MAX_TRACE,( "ScsiScannerInterpretSenseInfo: Additional sense code is %x\n",
                                     pSenseBuffer -> AdditionalSenseCode));
        SCSISCAN_KdPrint(MAX_TRACE,( "ScsiScannerInterpretSenseInfo: Additional sense code qualifier is %x\n",
                                     pSenseBuffer -> AdditionalSenseCodeQualifier));

        //
        // Zero the additional sense code and additional sense code qualifier
        // if they were not returned by the device.
        //

        ReadSector = pSenseBuffer -> AdditionalSenseLength +
            offsetof(SENSE_DATA, AdditionalSenseLength);

        if (ReadSector > pSrb -> SenseInfoBufferLength) {
            ReadSector = pSrb -> SenseInfoBufferLength;
        }

        if (ReadSector <= offsetof(SENSE_DATA, AdditionalSenseCode)) {
            pSenseBuffer -> AdditionalSenseCode = 0;
        }

        if (ReadSector <= offsetof(SENSE_DATA, AdditionalSenseCodeQualifier)) {
            pSenseBuffer -> AdditionalSenseCodeQualifier = 0;
        }

        switch (pSenseBuffer -> SenseKey & 0xf) {

            case SCSI_SENSE_NOT_READY:
                SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Device not ready\n"));
                *Status = STATUS_DEVICE_NOT_READY;

                switch (pSenseBuffer -> AdditionalSenseCode) {

                    case SCSI_ADSENSE_LUN_NOT_READY:
                        SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Lun not ready\n"));

                        switch (pSenseBuffer -> AdditionalSenseCodeQualifier) {
                            case SCSI_SENSEQ_BECOMING_READY:

                                SCSISCAN_KdPrint(MAX_TRACE,( "ScsiScannerInterpretSenseInfo:"
                                    " In process of becoming ready\n"));
                                break;

                            case SCSI_SENSEQ_MANUAL_INTERVENTION_REQUIRED:
                                SCSISCAN_KdPrint(MAX_TRACE,( "ScsiScannerInterpretSenseInfo:"
                                    " Manual intervention required\n"));
                                *Status = STATUS_NO_MEDIA_IN_DEVICE;
                                Retry = FALSE;
                                break;

                            case SCSI_SENSEQ_FORMAT_IN_PROGRESS:
                                SCSISCAN_KdPrint(MAX_TRACE,( "ScsiScannerInterpretSenseInfo: Format in progress\n"));
                                Retry = FALSE;
                                break;

                            case SCSI_SENSEQ_INIT_COMMAND_REQUIRED:
                                
                            default:
                                SCSISCAN_KdPrint(MAX_TRACE,( "ScsiScannerInterpretSenseInfo:"
                                    " Initializing command required\n"));
                                break;

                        } // end switch (pSenseBuffer -> AdditionalSenseCodeQualifier)
                        break;

                    case SCSI_ADSENSE_NO_MEDIA_IN_DEVICE:
                        SCSISCAN_KdPrint(MAX_TRACE,(
                            "ScsiScannerInterpretSenseInfo:"
                            " No Media in device.\n"));
                        *Status = STATUS_NO_MEDIA_IN_DEVICE;
                        Retry = FALSE;
                        break;
                        
                } // end switch (pSenseBuffer -> AdditionalSenseCode)
                break;

        case SCSI_SENSE_DATA_PROTECT:
            SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Media write protected\n"));
            *Status = STATUS_MEDIA_WRITE_PROTECTED;
            Retry = FALSE;
            break;

        case SCSI_SENSE_MEDIUM_ERROR:
            SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Bad media\n"));
            *Status = STATUS_DEVICE_DATA_ERROR;
            Retry = FALSE;
            LogError = TRUE;
            UniqueId = 256;
            LogStatus = IO_ERR_BAD_BLOCK;
            break;

        case SCSI_SENSE_HARDWARE_ERROR:
            SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Hardware error\n"));
            *Status = STATUS_IO_DEVICE_ERROR;
            LogError = TRUE;
            UniqueId = 257;
            LogStatus = IO_ERR_CONTROLLER_ERROR;
            break;

        case SCSI_SENSE_ILLEGAL_REQUEST:
            SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Illegal SCSI request\n"));
            *Status = STATUS_INVALID_DEVICE_REQUEST;
            switch (pSenseBuffer -> AdditionalSenseCode) {

                case SCSI_ADSENSE_ILLEGAL_COMMAND:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Illegal command\n"));
                    Retry = FALSE;
                    break;

                case SCSI_ADSENSE_ILLEGAL_BLOCK:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Illegal block address\n"));
                    *Status = STATUS_NONEXISTENT_SECTOR;
                    Retry = FALSE;
                    break;

                case SCSI_ADSENSE_INVALID_LUN:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Invalid LUN\n"));
                    *Status = STATUS_NO_SUCH_DEVICE;
                    Retry = FALSE;
                    break;

                case SCSI_ADSENSE_MUSIC_AREA:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Music area\n"));
                    Retry = FALSE;
                    break;

                case SCSI_ADSENSE_DATA_AREA:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Data area\n"));
                    Retry = FALSE;
                    break;

                case SCSI_ADSENSE_VOLUME_OVERFLOW:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Volume overflow\n"));
                    Retry = FALSE;
                    break;

                case SCSI_ADSENSE_INVALID_CDB:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Invalid CDB\n"));
                    Retry = FALSE;
                    break;

            } // end switch (pSenseBuffer -> AdditionalSenseCode)
            break;

        case SCSI_SENSE_UNIT_ATTENTION:
            switch (pSenseBuffer -> AdditionalSenseCode) {
                case SCSI_ADSENSE_MEDIUM_CHANGED:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Media changed\n"));
                    break;

                case SCSI_ADSENSE_BUS_RESET:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Bus reset\n"));
                    break;

                default:
                    SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Unit attention\n"));
                    break;

            } // end  switch (pSenseBuffer -> AdditionalSenseCode)
            *Status = STATUS_IO_DEVICE_ERROR;
            break;

        case SCSI_SENSE_ABORTED_COMMAND:
            SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Command aborted\n"));
            *Status = STATUS_IO_DEVICE_ERROR;
            break;

       case SCSI_SENSE_RECOVERED_ERROR:
            SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Recovered error\n"));
            *Status = STATUS_SUCCESS;
            Retry = FALSE;
            LogError = TRUE;
            UniqueId = 258;

            switch(pSenseBuffer -> AdditionalSenseCode) {
                case SCSI_ADSENSE_SEEK_ERROR:
                case SCSI_ADSENSE_TRACK_ERROR:
                    LogStatus = IO_ERR_SEEK_ERROR;
                    break;

                case SCSI_ADSENSE_REC_DATA_NOECC:
                case SCSI_ADSENSE_REC_DATA_ECC:
                    LogStatus = IO_RECOVERED_VIA_ECC;
                    break;

                default:
                    LogStatus = IO_ERR_CONTROLLER_ERROR;
                    break;

            } // end switch(pSenseBuffer -> AdditionalSenseCode)

            if (pSenseBuffer -> IncorrectLength) {
                SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Incorrect length detected.\n"));
                *Status = STATUS_INVALID_BLOCK_LENGTH ;
            }
            break;

        case SCSI_SENSE_NO_SENSE:

            //
            // Check other indicators.
            //
            
            if (pSenseBuffer -> IncorrectLength) {
                SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Incorrect length detected.\n"));
                *Status = STATUS_INVALID_BLOCK_LENGTH ;
                Retry   = FALSE;
            } else {
                SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: No specific sense key\n"));
                *Status = STATUS_IO_DEVICE_ERROR;
                Retry   = TRUE;
            }
            break;

        default:
            SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Unrecognized sense code\n"));
            *Status = STATUS_IO_DEVICE_ERROR;
            break;

        } // end switch (pSenseBuffer -> SenseKey & 0xf)

        //
        // Try to determine the bad sector from the inquiry data.
        //

        if ((((PCDB)pSrb -> Cdb) -> CDB10.OperationCode == SCSIOP_READ ||
            ((PCDB)pSrb -> Cdb) -> CDB10.OperationCode == SCSIOP_VERIFY ||
            ((PCDB)pSrb -> Cdb) -> CDB10.OperationCode == SCSIOP_WRITE)) {

            for (Index = 0; Index < 4; Index++) {
                BadSector = (BadSector << 8) | pSenseBuffer -> Information[Index];
            }

            ReadSector = 0;
            for (Index = 0; Index < 4; Index++) {
                ReadSector = (ReadSector << 8) | pSrb -> Cdb[Index+2];
            }

            Index = (((PCDB)pSrb -> Cdb) -> CDB10.TransferBlocksMsb << 8) |
                ((PCDB)pSrb -> Cdb) -> CDB10.TransferBlocksLsb;

            //
            // Make sure the bad sector is within the read sectors.
            //
            
            if (!(BadSector >= ReadSector && BadSector < ReadSector + Index)) {
                BadSector = ReadSector;
            }
        }

    } else {

        //
        // Request sense buffer not valid. No sense information
        // to pinpoint the error. Return general request fail.
        //

        SCSISCAN_KdPrint(MAX_TRACE,("ScsiScannerInterpretSenseInfo: Request sense info not valid. SrbStatus %2x\n",
                    SRB_STATUS(pSrb -> SrbStatus)));
        Retry = TRUE;

        switch (SRB_STATUS(pSrb -> SrbStatus)) {
        case SRB_STATUS_INVALID_LUN:
        case SRB_STATUS_INVALID_TARGET_ID:
        case SRB_STATUS_NO_DEVICE:
        case SRB_STATUS_NO_HBA:
        case SRB_STATUS_INVALID_PATH_ID:
            *Status = STATUS_NO_SUCH_DEVICE;
            Retry = FALSE;
            break;

        case SRB_STATUS_COMMAND_TIMEOUT:
        case SRB_STATUS_ABORTED:
        case SRB_STATUS_TIMEOUT:

            //
            // Update the error count for the device.
            //

            pde -> ErrorCount++;
            *Status = STATUS_IO_TIMEOUT;
            break;

        case SRB_STATUS_SELECTION_TIMEOUT:
            LogError = TRUE;
            LogStatus = IO_ERR_NOT_READY;
            UniqueId = 260;
            *Status = STATUS_DEVICE_NOT_CONNECTED;
            Retry = FALSE;
            break;

        case SRB_STATUS_DATA_OVERRUN:
            *Status = STATUS_DATA_OVERRUN;
            Retry = FALSE;
            break;

        case SRB_STATUS_PHASE_SEQUENCE_FAILURE:

            //
            // Update the error count for the device.
            //
            
            pde -> ErrorCount++;
            *Status = STATUS_IO_DEVICE_ERROR;

            //
            // If there was phase sequence error then limit the number of
            // retries.
            //
            
            if (RetryCount > 1 ) {
                Retry = FALSE;
            }
            break;

        case SRB_STATUS_REQUEST_FLUSHED:
            *Status = STATUS_IO_DEVICE_ERROR;
            break;			

        case SRB_STATUS_INVALID_REQUEST:

            //
            // An invalid request was attempted.
            //
            
            *Status = STATUS_INVALID_DEVICE_REQUEST;
            Retry = FALSE;
            break;

        case SRB_STATUS_UNEXPECTED_BUS_FREE:
        case SRB_STATUS_PARITY_ERROR:

            //
            // Update the error count for the device.
            //

            pde -> ErrorCount++;

            //
            // Fall through to below.
            //

        case SRB_STATUS_BUS_RESET:
            *Status = STATUS_IO_DEVICE_ERROR;
            break;

        case SRB_STATUS_ERROR:
            *Status = STATUS_IO_DEVICE_ERROR;
            if (pSrb -> ScsiStatus == 0) {

                //
                // This is some strange return code.  Update the error
                // count for the device.
                //
                
                pde -> ErrorCount++;
            }
            
            if (pSrb -> ScsiStatus == SCSISTAT_BUSY) {
                *Status = STATUS_DEVICE_NOT_READY;
            }
            
            if (pSrb -> ScsiStatus == SCSISTAT_RESERVATION_CONFLICT) {
                *Status = STATUS_DEVICE_BUSY;
                Retry = FALSE;
            }
            break;

        default:
            LogError = TRUE;
            LogStatus = IO_ERR_CONTROLLER_ERROR;
            UniqueId = 259;
            *Status = STATUS_IO_DEVICE_ERROR;
            break;

        }

        //
        // If the error count has exceeded the error limit, then disable
        // any tagged queuing, multiple requests per lu queueing
        // and sychronous data transfers.
        //

        if (pde -> ErrorCount == 4) {

            //
            // Clearing the no queue freeze flag prevents the port driver
            // from sending multiple requests per logical unit.
            //
            
            pde -> SrbFlags &= ~(SRB_FLAGS_QUEUE_ACTION_ENABLE |
                               SRB_FLAGS_NO_QUEUE_FREEZE);

            pde -> SrbFlags |= SRB_FLAGS_DISABLE_SYNCH_TRANSFER;
            SCSISCAN_KdPrint(MAX_TRACE,( "ScsiScannerInterpretSenseInfo: Too many errors disabling tagged queuing and synchronous data tranfers.\n"));

        } else if (pde -> ErrorCount == 8) {

            //
            // If a second threshold is reached, disable disconnects.
            //
            
            pde -> SrbFlags |= SRB_FLAGS_DISABLE_DISCONNECT;
            SCSISCAN_KdPrint(MAX_TRACE,( "ScsiScannerInterpretSenseInfo: Too many errors disabling disconnects.\n"));
        }
    }

    //
    // Log an error if necessary.
    //
    
    if (LogError) {
        pErrorLogEntry = (PIO_ERROR_LOG_PACKET)IoAllocateErrorLogEntry(
            pDeviceObject,
            sizeof(IO_ERROR_LOG_PACKET) + 5 * sizeof(ULONG));

        if (NULL == pErrorLogEntry) {

            //
            // Return if no packet could be allocated.
            //
            
            return Retry;

        }

        if (Retry && RetryCount < MAXIMUM_RETRIES) {
            pErrorLogEntry -> FinalStatus = STATUS_SUCCESS;
        } else {
            pErrorLogEntry -> FinalStatus = *Status;
        }

        //
        // Calculate the device offset if there is a geometry.
        //
        
        pErrorLogEntry -> ErrorCode = LogStatus;
        pErrorLogEntry -> SequenceNumber = 0;
        pErrorLogEntry -> MajorFunctionCode = MajorFunctionCode;
        pErrorLogEntry -> IoControlCode = IoDeviceCode;
        pErrorLogEntry -> RetryCount = (UCHAR) RetryCount;
        pErrorLogEntry -> UniqueErrorValue = UniqueId;
        pErrorLogEntry -> DumpDataSize = 6 * sizeof(ULONG);
        pErrorLogEntry -> DumpData[0] = pSrb -> PathId;
        pErrorLogEntry -> DumpData[1] = pSrb -> TargetId;
        pErrorLogEntry -> DumpData[2] = pSrb -> Lun;
        pErrorLogEntry -> DumpData[3] = 0;
        pErrorLogEntry -> DumpData[4] = pSrb -> SrbStatus << 8 | pSrb -> ScsiStatus;

        if (pSenseBuffer != NULL) {
            pErrorLogEntry -> DumpData[5] = pSenseBuffer -> SenseKey << 16 |
                                     pSenseBuffer -> AdditionalSenseCode << 8 |
                                     pSenseBuffer -> AdditionalSenseCodeQualifier;

        }
        //
        // Write the error log packet.
        //
        
        IoWriteErrorLogEntry(pErrorLogEntry);
    }

    return Retry;

} // end ScsiScannerInterpretSenseInfo()



VOID
ClassReleaseQueue(
    IN PDEVICE_OBJECT pDeviceObject
    )

/*++

Routine Description:
    This routine issues an internal device control command
    to the port driver to release a frozen queue. The call
    is issued asynchronously as ClassReleaseQueue will be invoked
    from the IO completion DPC (and will have no context to
    wait for a synchronous call to complete).

Arguments:
    pDeviceObject - The functional device object for the device with the frozen queue.

Return Value:
    None.

--*/
{
    PIO_STACK_LOCATION			    pIrpStack;
    PIRP						    pIrp;
    PSCSISCAN_DEVICE_EXTENSION      pde;        	
    PCOMPLETION_CONTEXT				pContext;
    PSCSI_REQUEST_BLOCK				pSrb;
    KIRQL							CurrentIrql;

    //
	// Get our device extension.
    //
    
    pde = (PSCSISCAN_DEVICE_EXTENSION)pDeviceObject -> DeviceExtension;                	

    //
    // Allocate context from nonpaged pool.
    //
    
    pContext = ExAllocatePool(NonPagedPoolMustSucceed,
                               sizeof(COMPLETION_CONTEXT));
	pContext -> Signature = 'pmoC';

    //
    // Save the device object in the context for use by the completion
    // routine.
    //
	
    pContext->pDeviceObject = pDeviceObject;
    pSrb = &(pContext->Srb);

    //
    // Zero out srb.
    //
	
    RtlZeroMemory(pSrb, SCSI_REQUEST_BLOCK_SIZE);

    //    
    // Write length to SRB.
    //    
	
    pSrb->Length = SCSI_REQUEST_BLOCK_SIZE;

    //
	// specify release queue command.
    //    
	
    pSrb->Function = SRB_FUNCTION_RELEASE_QUEUE;

    //    
    // Build the asynchronous request to be sent to the port driver.
    //    

    pIrp = IoAllocateIrp(pDeviceObject->StackSize, FALSE);

    IoSetCompletionRoutine(pIrp,
                           (PIO_COMPLETION_ROUTINE)ClassAsynchronousCompletion,
                           pContext,
                           TRUE,
                           TRUE,
                           TRUE);

    pIrpStack = IoGetNextIrpStackLocation(pIrp);

    pIrpStack->MajorFunction = IRP_MJ_SCSI;

    pSrb->OriginalRequest = pIrp;

    //
    // Store the SRB address in next stack for port driver.
    //

    pIrpStack->Parameters.Scsi.Srb = pSrb;

    //
    // Since this routine can cause outstanding requests to be completed, and
    // calling a completion routine at < DISPATCH_LEVEL is dangerous (if they
    // call IoStartNextPacket we will bugcheck) raise up to dispatch level before
    // issuing the request
    //

    CurrentIrql = KeGetCurrentIrql();

    if(CurrentIrql < DISPATCH_LEVEL) {
        KeRaiseIrql(DISPATCH_LEVEL, &CurrentIrql);
        IoCallDriver(pde->pStackDeviceObject, pIrp);
        KeLowerIrql(CurrentIrql);
    } else {
        IoCallDriver(pde->pStackDeviceObject, pIrp);
    }

    return;

} // end ClassReleaseQueue()
					


NTSTATUS
ClassAsynchronousCompletion(
    PDEVICE_OBJECT pDeviceObject,
    PIRP pIrp,
    PCOMPLETION_CONTEXT pContext
    )
/*++

Routine Description:
    This routine is called when an asynchronous I/O request
    which was issused by the class driver completes.  Examples of such requests
    are release queue or START UNIT. This routine releases the queue if
    necessary.  It then frees the context and the IRP.

Arguments:
    pDeviceObject - The device object for the logical unit; however since this
                    is the top stack location the value is NULL.
    pIrp		  - Supplies a pointer to the Irp to be processed.
    pContext      - Supplies the context to be used to process this request.

Return Value:
    None.

--*/

{
    PSCSI_REQUEST_BLOCK pSrb;
	
    pSrb = &(pContext->Srb);

    //
    // If this is an execute srb, then check the return status and make sure.
    // the queue is not frozen.
    //

    if (pSrb->Function == SRB_FUNCTION_EXECUTE_SCSI) {

        //
        // Check for a frozen queue.
        //

        if (pSrb->SrbStatus & SRB_STATUS_QUEUE_FROZEN) {

            //
            // Unfreeze the queue getting the device object from the context.
            //

            ClassReleaseQueue(pContext->pDeviceObject);
        }
    }

    //
    // Free the context and the Irp.
    //

    if (pIrp->MdlAddress != NULL) {
        MmUnlockPages(pIrp->MdlAddress);
        IoFreeMdl(pIrp->MdlAddress);
        pIrp->MdlAddress = NULL;
    }

    ExFreePool(pContext);
    IoFreeIrp(pIrp);

    //
    // Indicate the I/O system should stop processing the Irp completion.
    //

    return STATUS_MORE_PROCESSING_REQUIRED;

} // ClassAsynchronousCompletion()



