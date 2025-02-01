//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#ifdef __cplusplus
extern "C" {
#include "strmini.h"
#include "ksmedia.h"
}
#endif
#include "defaults.h"
#include "capmain.h"
#include "capdebug.h"
#include "device.h"

//==========================================================================;
//                   Data Packet Handling Routines
//==========================================================================;

/*
** VideoReceiveDataPacket()
**
**   Receives Video data packet commands
**
** Arguments:
**
**   pSrb - Stream request block for the Video stream
**
** Returns: nothing
**
** Side Effects:  none
*/

VOID STREAMAPI VideoReceiveDataPacket(IN PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    UINT                    streamNumber = pSrb->StreamObject->StreamNumber;
    KIRQL                   Irql;
    PSRB_EXTENSION          pSrbExt;
    PKEVENT                 pKEvent;
    //
    // make sure we have a device extension
    //

    DEBUG_ASSERT((ULONG)pHwDevExt);

    ASSERT(pSrb->Irp->MdlAddress);
    
    DBGINFO(("'Bt829: Receiving SD---- SRB=%x\n", pSrb));

    pSrb->Status = STATUS_SUCCESS;

    switch (pSrb->Command)
        {
        case SRB_WRITE_DATA:
            ASSERT (streamNumber == STREAM_AnalogVideoInput);
            
            // We only care about this if there is a VBI capture stream AND we are
            // not stopped
            if (pHwDevExt->VBICaptureStreamConnected && pStrmEx->KSState != KSSTATE_STOP) {
                PKSSTREAM_HEADER pDataPacket = pSrb->CommandData.DataBufferArray;
    
                if (pDataPacket->FrameExtent == sizeof (KS_TVTUNER_CHANGE_INFO)) {
                    KeAcquireSpinLock(&pHwDevExt->deviceExtensionLock, &Irql);
                    RtlCopyMemory(  &pHwDevExt->TVTunerChangeInfo,
                                    pDataPacket->Data,
                                    sizeof (KS_TVTUNER_CHANGE_INFO));
                    
                    ASSERT(pHwDevExt->TVTunerChangedSrb == NULL);
                    pHwDevExt->TVTunerChangedSrb = pSrb;
                    
                    ASSERT(pHwDevExt->TVTunerChanged == FALSE);
                    pHwDevExt->TVTunerChanged = TRUE;
                    
                    KeReleaseSpinLock(&pHwDevExt->deviceExtensionLock, Irql);
                }
            } 
            else
            {
                StreamClassStreamNotification(  StreamRequestComplete,
                                                pSrb->StreamObject,
                                                pSrb);
            }
            break;

        case SRB_READ_DATA:

            if (streamNumber != STREAM_VideoCapture && streamNumber != STREAM_VBICapture)
            {
                DBGERROR(("Bt829: Unexpected SRB_READ_DATA on non-capture stream.\n"));
                TRAP();
                StreamClassStreamNotification(  StreamRequestComplete,
                                                pSrb->StreamObject,
                                                pSrb);
                break;
            }

            // Rule: 
            // Only accept read requests when in either the Pause or Run
            // States.  If Stopped, immediately return the SRB.

            if (pStrmEx->KSState == KSSTATE_STOP) {
                StreamClassStreamNotification(  StreamRequestComplete,
                                                pSrb->StreamObject,
                                                pSrb);
                break;
            } 

            pSrbExt = (PSRB_EXTENSION)pSrb->SRBExtension;
            RtlZeroMemory (pSrbExt, sizeof (SRB_EXTENSION));
            pSrbExt->pSrb = pSrb;
            pKEvent = &pSrbExt->bufferDoneEvent;
            KeInitializeEvent(pKEvent, SynchronizationEvent, FALSE);

            DBGINFO(("'Bt829: Adding 0x%x to data queue\n", pSrb));

            KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
            InsertTailList(&pStrmEx->incomingDataSrbQueue, &pSrbExt->listEntry);
            KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
            KeSetEvent(&pStrmEx->SrbAvailableEvent, 0, FALSE);

            break;

        default:

            //
            // invalid / unsupported command. Fail it as such
            //

            TRAP();

            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            StreamClassStreamNotification(  StreamRequestComplete,
                                            pSrb->StreamObject,
                                            pSrb);
            break;
    }
}


/*
** VideoReceiveCtrlPacket()
**
**   Receives packet commands that control the Video stream
**
** Arguments:
**
**   pSrb - The stream request block for the Video stream
**
** Returns: nothing
**
** Side Effects:  none
*/

VOID STREAMAPI VideoReceiveCtrlPacket(IN PHW_STREAM_REQUEST_BLOCK pSrb)
{
    KIRQL Irql;
    PHW_STREAM_REQUEST_BLOCK pCurrentSrb = NULL;

    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PSRB_EXTENSION pSrbExt = (PSRB_EXTENSION)pSrb->SRBExtension;

    KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
    if (pStrmEx->processingCtrlSrb)
    {
        pSrbExt->pSrb = pSrb;
        InsertTailList(&pStrmEx->ctrlSrbQueue, &pSrbExt->listEntry);
        KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
        return;
    }

    pStrmEx->processingCtrlSrb = TRUE;
    pCurrentSrb = pSrb;
    KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);

    // This will run until the queue is empty
    while (TRUE)
    {
        // Assume success. Might be changed below
    
        pCurrentSrb->Status = STATUS_SUCCESS;

        switch (pCurrentSrb->Command)
        {
            case SRB_GET_STREAM_STATE:
                VideoGetState(pCurrentSrb);
                break;

            case SRB_SET_STREAM_STATE:
                VideoSetState(pCurrentSrb);
                break;

            case SRB_GET_STREAM_PROPERTY:
                VideoGetProperty(pCurrentSrb);
                break;

            case SRB_SET_STREAM_PROPERTY:
                VideoSetProperty(pCurrentSrb);
                break;

            case SRB_INDICATE_MASTER_CLOCK:
                VideoIndicateMasterClock (pCurrentSrb);
                break;

           case SRB_PROPOSE_DATA_FORMAT:
                // BUGBUG. This may be inappropriate for Bt829. CHECK!!!
                DBGERROR(("Bt829: Propose Data format\n"));

                if (!(AdapterVerifyFormat (
                    pCurrentSrb->CommandData.OpenFormat, 
                    pCurrentSrb->StreamObject->StreamNumber))) {
                    pCurrentSrb->Status = STATUS_NO_MATCH;
                }
                break;
            default:
                TRAP();
                pCurrentSrb->Status = STATUS_NOT_IMPLEMENTED;
                break;
        }

        StreamClassStreamNotification(  StreamRequestComplete,
                                        pCurrentSrb->StreamObject,
                                        pCurrentSrb);

        KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
        if (IsListEmpty(&pStrmEx->ctrlSrbQueue))
        {
            pStrmEx->processingCtrlSrb = FALSE;
            KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
            return;
        }
        else
        {
            PUCHAR ptr;
            ptr = (PUCHAR)RemoveHeadList(&pStrmEx->ctrlSrbQueue);
            KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);

            pSrbExt = (PSRB_EXTENSION) (((PUCHAR) ptr) - 
                         FIELDOFFSET(SRB_EXTENSION, listEntry));

            pCurrentSrb = pSrbExt->pSrb;
        }
    }
}

/*
** VideoSetProperty()
**
**    Routine to process video property requests
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID VideoSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    if (IsEqualGUID (KSPROPSETID_VPConfig, pSPD->Property->Set)) {
        SetVideoPortProperty (pSrb);
    }
    else if (IsEqualGUID (KSPROPSETID_VPVBIConfig, pSPD->Property->Set)) {
        SetVideoPortVBIProperty (pSrb);
    }
    else {
       pSrb->Status = STATUS_NOT_IMPLEMENTED;
    }
}

/*
** VideoGetProperty()
**
**    Routine to process video property requests
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID VideoGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    if (IsEqualGUID (KSPROPSETID_Connection, pSPD->Property->Set)) {
        VideoStreamGetConnectionProperty (pSrb);
    }
    else if (IsEqualGUID (PROPSETID_VIDCAP_DROPPEDFRAMES, pSPD->Property->Set)) {
        VideoStreamGetDroppedFramesProperty (pSrb);
    }
    else if (IsEqualGUID (KSPROPSETID_VPConfig, pSPD->Property->Set)) {
        GetVideoPortProperty (pSrb);
    }
    else if (IsEqualGUID (KSPROPSETID_VPVBIConfig, pSPD->Property->Set)) {
        GetVideoPortVBIProperty (pSrb);
    }
    else {
       pSrb->Status = STATUS_NOT_IMPLEMENTED;
    }
}


/*
** VideoSetState()
**
**    Sets the current state of the requested stream
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID VideoSetState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    Device                  *device = pHwDevExt->device;
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    UINT                    streamNumber = pSrb->StreamObject->StreamNumber;

    //
    // For each stream, the following states are used:
    // 
    // Stop:    Absolute minimum resources are used.  No outstanding IRPs.
    // Pause:   Getting ready to run.  Allocate needed resources so that 
    //          the eventual transition to Run is as fast as possible.
    //          SRBs will be queued at either the Stream class or in your
    //          driver.
    // Run:     Streaming. 
    //
    // Moving to Stop or Run ALWAYS transitions through Pause, so that ONLY 
    // the following transitions are possible:
    //
    // Stop -> Pause
    // Pause -> Run
    // Run -> Pause
    // Pause -> Stop
    //
    // Note that it is quite possible to transition repeatedly between states:
    // Stop -> Pause -> Stop -> Pause -> Run -> Pause -> Run -> Pause -> Stop
    //

    ASSERT (device != NULL);

    pSrb->Status = STATUS_SUCCESS;

    switch (pSrb->CommandData.StreamState)  

    {
        case KSSTATE_STOP:
            DBGINFO(("'Bt829: Entering KSSTATE_STOP for stream %d\n", streamNumber));            //
            switch (streamNumber)
            {
                case STREAM_VPVideo:
                case STREAM_VPVBI:
                    device->SetOutputEnabled(FALSE);
                    break;

                case STREAM_VideoCapture:
                case STREAM_VBICapture:
                    ASSERT(pStrmEx->stateChange == ChangeComplete);
                    pStrmEx->stateChange = Stopping;
                    FlushBuffers(pStrmEx);
                    KeResetEvent(&pStrmEx->specialEvent);
                    KeSetEvent(&pStrmEx->stateTransitionEvent, 0, TRUE);
                    KeWaitForSingleObject( &pStrmEx->specialEvent, Suspended, KernelMode, FALSE, NULL);
                    ASSERT(pStrmEx->stateChange == ChangeComplete);
                    break;
            }

            // Remember the state
            pStrmEx->KSState = pSrb->CommandData.StreamState;
            break;

        case KSSTATE_ACQUIRE:
            ASSERT( pStrmEx->KSState == KSSTATE_STOP);

            DBGINFO(("'Bt829: Entering KSSTATE_ACQUIRE for stream %d\n", streamNumber));
            pStrmEx->KSState = pSrb->CommandData.StreamState;
            break;

        case KSSTATE_PAUSE:
            
            // BUGBUG. Check this. Is it really a problem?
            if ((pHwDevExt->preEventOccurred) &&
                        (pStrmEx->KSState == KSSTATE_STOP || pStrmEx->KSState == KSSTATE_ACQUIRE))
            {
                pSrb->Status = STATUS_UNSUCCESSFUL;
            }

            else if (   (streamNumber == STREAM_VideoCapture || streamNumber == STREAM_VBICapture) &&
                        (pStrmEx->KSState == KSSTATE_STOP || pStrmEx->KSState == KSSTATE_ACQUIRE))
            {
                // Zero the frame counters
                if (streamNumber == STREAM_VideoCapture)
                {
                        RtlZeroMemory (&pStrmEx->FrameInfo, sizeof (KS_FRAME_INFO));
                }
                else
                {
                        RtlZeroMemory (&pStrmEx->VBIFrameInfo, sizeof (KS_VBI_FRAME_INFO));
                }

                ResetFieldNumber(pStrmEx);
                
                if (!GetCaptureHandle(pStrmEx))
                {
                    pSrb->Status = STATUS_UNSUCCESSFUL;
                }
            }

            else if (   (streamNumber == STREAM_VideoCapture || streamNumber == STREAM_VBICapture) &&
                        (pStrmEx->KSState == KSSTATE_RUN))
            {
                // Transitioning from run to pause
                ASSERT(pStrmEx->stateChange == ChangeComplete);
                pStrmEx->stateChange = Pausing;
                FlushBuffers(pStrmEx);
                KeResetEvent(&pStrmEx->specialEvent);
                KeSetEvent(&pStrmEx->stateTransitionEvent, 0, TRUE);
                KeWaitForSingleObject( &pStrmEx->specialEvent, Suspended, KernelMode, FALSE, NULL);
                ASSERT(pStrmEx->stateChange == ChangeComplete);
            }
            
            if (pSrb->Status == STATUS_SUCCESS)
            {
                DBGINFO(("'Bt829: Entering KSSTATE_PAUSE for stream %d\n", streamNumber));

                // Remember the state
                pStrmEx->KSState = pSrb->CommandData.StreamState;
            }
            else
            {
                DBGERROR(("Bt829: NOT entering KSSTATE_PAUSE for stream %d\n", streamNumber));
            }
            break;

        case KSSTATE_RUN:

            ASSERT( pStrmEx->KSState == KSSTATE_ACQUIRE ||
                    pStrmEx->KSState == KSSTATE_PAUSE);

            if (pHwDevExt->preEventOccurred)
            {
                pSrb->Status = STATUS_UNSUCCESSFUL;
                DBGERROR(("Bt829: NOT entering KSSTATE_RUN for stream %d\n", streamNumber));
            }
            else
            {
                DBGINFO(("'Bt829: Entering KSSTATE_RUN for stream %d\n", streamNumber));

                if (streamNumber == STREAM_VPVideo || streamNumber == STREAM_VPVBI )
                {
                    device->SetOutputEnabled(TRUE);
                }
    
                if (streamNumber == STREAM_VideoCapture || streamNumber == STREAM_VBICapture)
                {
                    ResetFieldNumber(pStrmEx);

                    // Transitioning from pause to run
                    ASSERT(pStrmEx->stateChange == ChangeComplete);
                    pStrmEx->stateChange = Running;
                    KeResetEvent(&pStrmEx->specialEvent);
                    KeSetEvent(&pStrmEx->stateTransitionEvent, 0, TRUE);
                    KeWaitForSingleObject( &pStrmEx->specialEvent, Suspended, KernelMode, FALSE, NULL);
                    ASSERT(pStrmEx->stateChange == ChangeComplete);
                }

                // Remember the state
                pStrmEx->KSState = pSrb->CommandData.StreamState;
            }
            break;
    }
}

/*
** VideoGetState()
**
**    Gets the current state of the requested stream
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID VideoGetState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    pSrb->CommandData.StreamState = pStrmEx->KSState;
    pSrb->ActualBytesTransferred = sizeof (KSSTATE);

    // A very odd rule:
    // When transitioning from stop to pause, DShow tries to preroll
    // the graph.  Capture sources can't preroll, and indicate this
    // by returning VFW_S_CANT_CUE in user mode.  To indicate this
    // condition from drivers, they must return ERROR_NO_DATA_DETECTED

    if (pStrmEx->KSState == KSSTATE_PAUSE) {
       pSrb->Status = STATUS_NO_DATA_DETECTED;
    }
}

VOID  VideoStreamGetConnectionProperty (PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    UINT                    streamNumber = pSrb->StreamObject->StreamNumber;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id) {

    case KSPROPERTY_CONNECTION_ALLOCATORFRAMING:
        if (streamNumber == STREAM_VideoCapture) {
            PKSALLOCATOR_FRAMING Framing = (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;
            Framing->RequirementsFlags   =
            KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY |
            KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
            KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY;
            Framing->PoolType = NonPagedPool;
            Framing->Frames = NumBuffers;
            Framing->FrameSize = pStrmEx->pVideoInfoHeader->bmiHeader.biSizeImage;
            Framing->FileAlignment = 0;//FILE_QUAD_ALIGNMENT;// PAGE_SIZE - 1;
            Framing->Reserved = 0;
            pSrb->ActualBytesTransferred = sizeof( KSALLOCATOR_FRAMING );
        }
        else if (streamNumber == STREAM_VBICapture) {
            PKSALLOCATOR_FRAMING Framing = (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;
            Framing->RequirementsFlags   =
            KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY |
            KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
            KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY;
            Framing->PoolType = NonPagedPool;
            Framing->Frames = NumBuffers;
            Framing->FrameSize = pStrmEx->pVBIInfoHeader->BufferSize;
            Framing->FileAlignment = 0;//FILE_QUAD_ALIGNMENT;// PAGE_SIZE - 1;
            Framing->Reserved = 0;
            pSrb->ActualBytesTransferred = sizeof( KSALLOCATOR_FRAMING );
        }
        else if (streamNumber == STREAM_AnalogVideoInput) {
            PKSALLOCATOR_FRAMING Framing = (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;
            Framing->RequirementsFlags   =
            KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY |
            KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
            KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY;
            Framing->PoolType = NonPagedPool;
            Framing->Frames = 1;
            Framing->FrameSize = sizeof(KS_TVTUNER_CHANGE_INFO);
            Framing->FileAlignment = 0;//FILE_QUAD_ALIGNMENT;// PAGE_SIZE - 1;
            Framing->Reserved = 0;
            pSrb->ActualBytesTransferred = sizeof( KSALLOCATOR_FRAMING );
        }
        else {
            PKSALLOCATOR_FRAMING Framing = (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;
            Framing->RequirementsFlags   =
            KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY |
            KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
            KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY;
            Framing->PoolType = NonPagedPool;
            Framing->Frames = 1;
            Framing->FrameSize = 1;
            Framing->FileAlignment = 0;//FILE_QUAD_ALIGNMENT;// PAGE_SIZE - 1;
            Framing->Reserved = 0;
            pSrb->ActualBytesTransferred = sizeof( KSALLOCATOR_FRAMING );

        }
        break;
        
    default:
        TRAP();
        break;
    }
}

/*
** VideoStreamGetDroppedFramesProperty
**
**    Gets dropped frame information
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID VideoStreamGetDroppedFramesProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    UINT streamNumber = pSrb->StreamObject->StreamNumber;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id) {

    case KSPROPERTY_DROPPEDFRAMES_CURRENT:
        {
            PKSPROPERTY_DROPPEDFRAMES_CURRENT_S pDroppedFrames = 
                (PKSPROPERTY_DROPPEDFRAMES_CURRENT_S) pSPD->PropertyInfo;

            if (streamNumber == STREAM_VideoCapture)
            {
                pDroppedFrames->PictureNumber = pStrmEx->FrameInfo.PictureNumber;
                pDroppedFrames->DropCount = pStrmEx->FrameInfo.DropCount;
                pDroppedFrames->AverageFrameSize = pStrmEx->pVideoInfoHeader->bmiHeader.biSizeImage;
            }
            else if (streamNumber == STREAM_VBICapture)
            {
                pDroppedFrames->PictureNumber = pStrmEx->VBIFrameInfo.PictureNumber;
                pDroppedFrames->DropCount = pStrmEx->VBIFrameInfo.DropCount;
                pDroppedFrames->AverageFrameSize = pStrmEx->pVBIInfoHeader->BufferSize;
            }
            else
            {
                pSrb->Status = STATUS_UNSUCCESSFUL;
                DBGERROR(("Bt829: Bad stream number for dropped frames\n"));
            }

            DBGINFO(("'Bt829: PictNumber: 0x%x; DropCount: 0x%x; BufSize: 0x%x\n",
                (ULONG) pDroppedFrames->PictureNumber,
                (ULONG) pDroppedFrames->DropCount,
                (ULONG) pDroppedFrames->AverageFrameSize));
            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_DROPPEDFRAMES_CURRENT_S);
        }
        break;

    default:
        TRAP();
        break;
    }
}


//==========================================================================;
//                   Clock Handling Routines
//==========================================================================;


/*
** VideoIndicateMasterClock ()
**
**    If this stream is not being used as the master clock, this function
**      is used to provide us with a handle to the clock to use.
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID VideoIndicateMasterClock(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    pStrmEx->hMasterClock = pSrb->CommandData.MasterClockHandle;
}

/*
** GetSystemTime ()
**
**    Returns the system time in 100 nS units
**
** Arguments:
**
** Returns:
**
** Side Effects:  none
*/

ULONGLONG VideoGetSystemTime()
{
    ULONGLONG ticks;
    ULONGLONG rate;

    ticks = (ULONGLONG)KeQueryPerformanceCounter((PLARGE_INTEGER)&rate).QuadPart;

    //
    // convert from ticks to 100ns clock
    //

    ticks = (ticks & 0xFFFFFFFF00000000) / rate * 10000000 +
            (ticks & 0x00000000FFFFFFFF) * 10000000 / rate;

    return(ticks);

}

