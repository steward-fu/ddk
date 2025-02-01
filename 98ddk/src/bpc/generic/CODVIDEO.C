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

#include <strmini.h>
#include <ksmedia.h>
#include "codmain.h"
// #include "codstrm.h"
#include "coddebug.h"
#include <ntstatus.h>


#if 1 // defined(MULTIPLE_PIN_INSTANCES)

//==========================================================================;
// Routines for processing video streams
//==========================================================================;

/*
** VBIDecode()
**
**	Decode the VBI stream
**
** Arguments:
**
**	pHwDevExt - Pointer to the main Hardware Device Extension
**	pStrmEx - Pointer to Stream Extension for the incoming SRB.
**
** Returns: nothing
**
** Side-effects:
**	Completes pending SRBs on input and output pins.
*/


void
VBIDecode(PHW_DEVICE_EXTENSION pHwDevExt, PSTREAMEX pInStrmEx)
{
    PHW_STREAM_REQUEST_BLOCK    pInSrb = pInStrmEx->pCurrentSrb;
    PKSSTREAM_HEADER            pInStreamHeader = pInSrb->CommandData.DataBufferArray;
    PKS_VBI_FRAME_INFO          pInVBIFrameInfo = (PKS_VBI_FRAME_INFO)(pInStreamHeader+1);
    PUCHAR                      pInData =  (PUCHAR)pInStreamHeader->Data;
    ULONG                       i, j, iCnt, iMax;

    CASSERT((ULONG)pHwDevExt);
    CASSERT((ULONG)pInStrmEx);
    CASSERT((ULONG)pInStrmEx->pCurrentSrb);

    // Clear the current scanline & substream request masks
    RtlZeroMemory( &pInStrmEx->ScanlinesRequested, sizeof(pInStrmEx->ScanlinesRequested) );
    RtlZeroMemory( &pInStrmEx->SubstreamsRequested, sizeof(pInStrmEx->SubstreamsRequested) );

    // Calculate the current request from union of the output pins w/pending SRBs
    iMax = pHwDevExt->ActualInstances[STREAM_Decode];
    for (i = 0, iCnt = 0; i < MAX_PIN_INSTANCES && iCnt < iMax; ++i)
    {
        PSTREAMEX                   pOutStrmEx;
        PHW_STREAM_REQUEST_BLOCK    pOutSrb;

        pOutStrmEx = pHwDevExt->pStrmEx[STREAM_Decode][i];
	if (NULL == pOutStrmEx)
	    continue;

	++iCnt;
        pOutSrb = pOutStrmEx->pCurrentSrb;

        // Save the updated frame info for whenever we need to send out an SRB.
        if ( pInVBIFrameInfo->dwFrameFlags & KS_VBI_FLAG_VBIINFOHEADER_CHANGE )
            pInStrmEx->FrameInfo.VBIInfoHeader = pInVBIFrameInfo->VBIInfoHeader;
            
        // Save the discontinuity flag for whenever we need to send out an SRB.
        if ( pInVBIFrameInfo->dwFrameFlags & KS_VBI_FLAG_TVTUNER_CHANGE )
            pOutStrmEx->fDiscontinuity = TRUE;

        // Is there a pending output SRB?
        if ( pOutSrb )
        {
            PKSSTREAM_HEADER    pOutStreamHeader = pOutSrb->CommandData.DataBufferArray;
            PKS_VBI_FRAME_INFO  pOutVBIFrameInfo = (PKS_VBI_FRAME_INFO)(pOutStreamHeader+1);

            // Setup the basic outbound VBI_FRAME_INFO, needs further modification
            *pOutVBIFrameInfo = *pInVBIFrameInfo;

            // Create the union of all the requested scanlines
            for( j = 0; j < ENTRIES(pInStrmEx->ScanlinesRequested.DwordBitArray); j++ )
            {
                pInStrmEx->ScanlinesRequested.DwordBitArray[j] |= 
                    pOutStrmEx->ScanlinesRequested.DwordBitArray[j];
            }

            // Create the union of all the requested substreams
            for( j = 0; j < ENTRIES(pInStrmEx->SubstreamsRequested.SubstreamMask); j++ )
            {
                // For CC data, remove the [j] subscript entirely since it's only a single DWORD
                pInStrmEx->SubstreamsRequested.SubstreamMask[j] |= 
                    pOutStrmEx->SubstreamsRequested.SubstreamMask[j];
            }
        }
    }

    // Decode the union of all the pending decode requests into a local decode buffer.

    // pInStrmEx->ScanlinesRequested contains ALL currently requested scanlines
    // pInStrmEx->SubstreamsRequested contains ALL currently requested substreams

    // Insert call for input buffer ACTUAL decoding here!!!  (FIXME)

    // pInStrmEx->ScanlinesDiscovered needs to be ORed to indicate what was found "recently"
    // pInStrmEx->SubstreamsDiscovered needs to be ORed to indicate what was found "recently"

    // Loop through all pending outbound requests and fill each irp with the requested data then complete the IO
    iMax = pHwDevExt->ActualInstances[STREAM_Decode];
    for (i = 0, iCnt = 0; i < MAX_PIN_INSTANCES && iCnt < iMax; ++i)
    {
        PSTREAMEX                   pOutStrmEx;
        PHW_STREAM_REQUEST_BLOCK    pOutSrb;

        pOutStrmEx = pHwDevExt->pStrmEx[STREAM_Decode][i];
	if (NULL == pOutStrmEx)
	    continue;

	++iCnt;
        pOutSrb = pOutStrmEx->pCurrentSrb;

        // Only process the output streams which have an SRB ready.
        if ( pOutSrb )
        {
            PKSSTREAM_HEADER    pOutStreamHeader = pOutSrb->CommandData.DataBufferArray;
            PKS_VBI_FRAME_INFO  pOutVBIFrameInfo = (PKS_VBI_FRAME_INFO)(pOutStreamHeader+1);
            PUCHAR              pOutData =  (PUCHAR)pOutStreamHeader->Data;

            // pOutStrmEx->ScanlinesDiscovered needs to be OR'ed with (the request AND what was found)
            for( j = 0; j < ENTRIES(pInStrmEx->ScanlinesRequested.DwordBitArray); j++ )
            {
                pOutStrmEx->ScanlinesDiscovered.DwordBitArray[j] |= 
                    pInStrmEx->ScanlinesDiscovered.DwordBitArray[j] &
                    pOutStrmEx->ScanlinesRequested.DwordBitArray[j];
            }

            // pOutStrmEx->SubstreamsDiscovered needs to be OR'ed with (the request AND what was found)
            for( j = 0; j < ENTRIES(pInStrmEx->SubstreamsRequested.SubstreamMask); j++ )
            {
                // For CC data, remove the [j] subscript entirely since it's only a single DWORD
                pOutStrmEx->SubstreamsDiscovered.SubstreamMask[j] |= 
                    pInStrmEx->SubstreamsDiscovered.SubstreamMask[j] &
                    pOutStrmEx->SubstreamsRequested.SubstreamMask[j];
            }

            // Check on inbound & outbound data formats to decide whether to copy or decode the inbound data
            if ( CodecCompareGUIDsAndFormatSize( &pInStrmEx->OpenedFormat, &pOutStrmEx->OpenedFormat, FALSE ) )
            {
                // Just copy the data into the output buffer if the formats match.
                // Should be filterded based on the requested scanlines & substreams!

                RtlCopyMemory((PVOID)pOutData, (PVOID)pInData, pInStreamHeader->DataUsed);
                pOutStreamHeader->DataUsed = pInStreamHeader->DataUsed;
            }
            else
            {
                // Figure out how much of the decoded data was requested
                // Copy the specific decoded data that was requested here (Fix me)
                // pOutData is the output location.

                // pOutStreamHeader->DataUsed   // Copy data here that was decoded and requested
                pOutStreamHeader->DataUsed = 0; // Fix me to indicate how much was really copied!
            }

            // Complete the output SRB so we get another one.
            pOutStrmEx->pCurrentSrb = NULL;
            CompleteStreamSRB( pOutSrb, 
	                           StreamRequestComplete,
	                           TRUE,
	                           ReadyForNextStreamDataRequest);
        }
    }

    // Complete the input SRB so we get another one.
    pInStrmEx->pCurrentSrb = NULL;
    CompleteStreamSRB( 	pInSrb, 
	                    StreamRequestComplete,
	                    TRUE,
	                    ReadyForNextStreamDataRequest);
}

#else // !MULTIPLE_PIN_INSTANCES

/*
** VideoDecode()
**
**	Decode the VBI stream
**
** Arguments:
**
**	pInSrb - Pointer to Stream Request Block for input pin
**	pOutSrb - Pointer to Stream Request Block for output pin
**
** Returns: nothing
**
** Side-effects:
**	Completes both SRBs.
*/

void
VideoDecode(
	IN PHW_STREAM_REQUEST_BLOCK pInSrb,
	IN PHW_STREAM_REQUEST_BLOCK pOutSrb
    )
{
    PSTREAMEX           pInStrmEx = (PSTREAMEX)pInSrb->StreamObject->HwStreamExtension;
    PSTREAMEX           pOutStrmEx = (PSTREAMEX)pOutSrb->StreamObject->HwStreamExtension;
    PKSSTREAM_HEADER    pInDataPacket = pInSrb->CommandData.DataBufferArray;
    PKSSTREAM_HEADER    pOutDataPacket = pOutSrb->CommandData.DataBufferArray;
    PUCHAR              pInSamples =  (PUCHAR)pInDataPacket->Data;
    PUCHAR              pOutBuffer =  (PUCHAR)pOutDataPacket->Data;

    // Since this codec does no decoding, we just copy the data across.
    // This would be where a codec did its decoding.
    // Note that this code should be largely replaced in a real codec!!!

    pOutDataPacket->DataUsed = pInDataPacket->DataUsed;
    RtlCopyMemory((PVOID)pOutBuffer, (PVOID)pInSamples, pInDataPacket->DataUsed);

    // We're done.  Complete both SRBs for the caller.
    pOutStrmEx->pCurrentSrb = NULL;
    CompleteStreamSRB( pOutSrb, 
	                   StreamRequestComplete,
	                   TRUE,
	                   ReadyForNextStreamDataRequest);

    pInStrmEx->pCurrentSrb = NULL;
    CompleteStreamSRB( 	pInSrb, 
	                    StreamRequestComplete,
	                    TRUE,
	                    ReadyForNextStreamDataRequest);
}

#endif // MULTIPLE_PIN_INSTANCES

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

VOID 
STREAMAPI 
VideoReceiveDataPacket(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION  pHwDevExt = (PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension;
    PSTREAMEX             pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    int                   ThisStreamNr = (int)pSrb->StreamObject->StreamNumber;

    //
    // make sure we have a device extension
    //

    CASSERT((ULONG)pHwDevExt);

    CDebugPrint(DebugLevelTrace,( CODECNAME ":--->VideoReceiveDataPacket(pSrb=%x)\n", pSrb));

    // 
    // Default to success
    //

    pSrb->Status = STATUS_SUCCESS;
    pSrb->TimeoutCounter = 0;


    //
    // determine the type of packet.
    //

    switch (pSrb->Command)
    {
    case SRB_READ_DATA:
    case SRB_WRITE_DATA:

        CASSERT (pStrmEx->pCurrentSrb == NULL);

        // Rule: 
        // Only accept read requests when in either the Pause or Run
        // States.  If Stopped, immediately return the SRB.

        if (pStrmEx->KSState == KSSTATE_STOP) {

            CompleteStreamSRB (         
                    pSrb, 
                    StreamRequestComplete,
                    TRUE,
                    ReadyForNextStreamDataRequest);

            break;
        } 
        
#ifdef DRIVER_DEBUGGING_TEST

        // When initially bringing up a driver, it is useful to just
        // try immediately completing the SRB, thus verifying
        // the streaming process independent of really accessing
        // your hardware.  

        pStrmEx->pCurrentSrb = NULL;

        CompleteStreamSRB (         
            pSrb, 
            StreamRequestComplete,
            TRUE,
            ReadyForNextStreamDataRequest);

#else // DRIVER_DEBUGGING_TEST

#if 1 // defined(MULTIPLE_PIN_INSTANCES)
        // Handle data input, output requests differently.
        switch ( ThisStreamNr )
        {
        // For incoming data requests, figure out what to decode and then complete outbound IRPs
        case    STREAM_VBI:
            {
        		pStrmEx->pCurrentSrb = pSrb;
                VBIDecode(pHwDevExt, pStrmEx);
                break;
            }
        // For outgoing data requests, save it up so the next incoming request will complete it.
        case    STREAM_Decode:
            {
        		pStrmEx->pCurrentSrb = pSrb;
                break;
            }
        // Other "unknown" streams are not valid and will be rejected.
        default:
            {
                CDEBUG_BREAK();
                pSrb->Status = STATUS_NOT_IMPLEMENTED;

                CompleteStreamSRB (         
                    pSrb, 
                    StreamRequestComplete,
                    TRUE,
                    ReadyForNextStreamDataRequest);
                break;
            }
        }

#else // MULTIPLE_PIN_INSTANCES
        {
            // Keep a single SRB locally which is completed when we have
            // a buffer on both input and output pins.  

            int                   OtherStreamNr = 1 - ThisStreamNr;
            PSTREAMEX             pOtherStrmEx = pHwDevExt->pStrmEx[OtherStreamNr][0];
		    if (pOtherStrmEx->pCurrentSrb) 
            {
			    // Cool, we can complete the request now.
                PHW_STREAM_REQUEST_BLOCK pInSrb = pHwDevExt->pStrmEx[0][0]->pCurrentSrb;
                PHW_STREAM_REQUEST_BLOCK pOutSrb = pHwDevExt->pStrmEx[1][0]->pCurrentSrb;

                VideoDecode(pInSrb, pOutSrb);
            }
        }
#endif  // MULTIPLE_PIN_INSTANCES

#endif // DRIVER_DEBUGGING_TEST

        break;

    default:

        //
        // invalid / unsupported command. Fail it as such
        //

        CDEBUG_BREAK();

        pSrb->Status = STATUS_NOT_IMPLEMENTED;

        CompleteStreamSRB (         
            pSrb, 
            StreamRequestComplete,
            TRUE,
            ReadyForNextStreamDataRequest);

    }  // switch (pSrb->Command)

    CDebugPrint(DebugLevelTrace,( CODECNAME ":<---VideoReceiveDataPacket(pSrb=%x)\n", pSrb));
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

VOID 
STREAMAPI 
VideoReceiveCtrlPacket(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    CASSERT((ULONG)pHwDevExt);

    CDebugPrint(DebugLevelTrace,( CODECNAME ":--->VideoReceiveCtrlPacket(pSrb=%x)\n", pSrb));

    // 
    // Default to success
    //

    pSrb->Status = STATUS_SUCCESS;

    //
    // determine the type of packet.
    //

    switch (pSrb->Command)
    {
    case SRB_PROPOSE_DATA_FORMAT:
        if ( !CodecVerifyFormat( pSrb->CommandData.OpenFormat, 
                                 pSrb->StreamObject->StreamNumber ) )
        {
            pSrb->Status = STATUS_NO_MATCH;
        }
        break; 
    case SRB_SET_STREAM_STATE:

        VideoSetState(pSrb);
        break;

    case SRB_GET_STREAM_STATE:

        VideoGetState(pSrb);
        break;

    case SRB_GET_STREAM_PROPERTY:

        VideoGetProperty(pSrb);
        break;

    case SRB_SET_STREAM_PROPERTY:

        VideoSetProperty(pSrb);
        break;

    case SRB_INDICATE_MASTER_CLOCK:

        //
        // Assigns a clock to a stream
        //

        VideoIndicateMasterClock(pSrb);

        break;

    default:

        //
        // invalid / unsupported command. Fail it as such
        //

        CDEBUG_BREAK();

        pSrb->Status = STATUS_NOT_IMPLEMENTED;
    }

    CompleteStreamSRB (         
            pSrb, 
            StreamRequestComplete,
            TRUE,
            ReadyForNextStreamControlRequest);    

    CDebugPrint(DebugLevelTrace,( CODECNAME ":<---VideoReceiveCtrlPacket(pSrb=%x)\n", pSrb));
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

VOID 
VideoGetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    CDebugPrint(DebugLevelTrace,( CODECNAME ":--->VideoGetProperty(pSrb=%x)\n", pSrb));

    if (IsEqualGUID (&KSPROPSETID_Connection, &pSPD->Property->Set)) 
    {
        VideoStreamGetConnectionProperty (pSrb);
    }
    else if (IsEqualGUID (&KSPROPSETID_VBICodecFiltering, &pSPD->Property->Set))
    {
        VideoStreamGetVBIFilteringProperty (pSrb);
    }
    else 
    {
       pSrb->Status = STATUS_NOT_IMPLEMENTED;
    }

    CDebugPrint(DebugLevelTrace,( CODECNAME ":<---VideoGetProperty(pSrb=%x)\n", pSrb));
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

VOID 
VideoSetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    CDebugPrint(DebugLevelTrace,( CODECNAME ":--->VideoSetProperty(pSrb=%x)\n", pSrb));

    // VideoStreamSetConnectionProperty() is not required/implemented! Read only property set.
    // Currently only VideoStreamSetVBIFilteringProperty() has any writeable properties.
    
    if (IsEqualGUID (&KSPROPSETID_VBICodecFiltering, &pSPD->Property->Set))
    {
        VideoStreamSetVBIFilteringProperty (pSrb);
    }
    else if (IsEqualGUID (&KSPROPSETID_Stream, &pSPD->Property->Set))
    {
    	pSrb->Status = STATUS_SUCCESS;
    }
    else 
    {
        pSrb->Status = STATUS_NOT_IMPLEMENTED;
    }

    CDebugPrint(DebugLevelTrace,( CODECNAME ":<---VideoSetProperty(pSrb=%x)\n", pSrb));
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

VOID 
VideoSetState(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    CDebugPrint(DebugLevelTrace,( CODECNAME ":--->VideoSetState(pSrb=%x)\n", pSrb));

    //
    // For each stream, the following states are used:
    // 
    // Stop:    Absolute minimum resources are used.  No outstanding IRPs.
    // Acquire: KS only state that has no DirectShow correpondence
    //          Acquire needed resources.
    // Pause:   Getting ready to run.  Allocate needed resources so that 
    //          the eventual transition to Run is as fast as possible.
    //          Read SRBs will be queued at either the Stream class 
    //          or in your driver (depending on when you send "ReadyForNext")
    // Run:     Streaming. 
    //
    // Moving to Stop to Run always transitions through Pause.
    //
    // But since a client app could crash unexpectedly, drivers should handle
    // the situation of having outstanding IRPs cancelled and open streams
    // being closed WHILE THEY ARE STREAMING!
    //
    // Note that it is quite possible to transition repeatedly between states:
    // Stop -> Pause -> Stop -> Pause -> Run -> Pause -> Run -> Pause -> Stop
    //

    switch (pSrb->CommandData.StreamState)  

    {
    case KSSTATE_STOP:

        //
        // If transitioning to the stopped state, then complete any outstanding IRPs
        //

        if (pStrmEx->pCurrentSrb) {

            CDebugPrint(DebugLevelVerbose,( CODECNAME ": Cancelling %X\n", 
                    pStrmEx->pCurrentSrb));

            // Indicate the buffer is cancelled
            pStrmEx->pCurrentSrb->Status = STATUS_CANCELLED;

            // Set the size of data read to zero
            pStrmEx->pCurrentSrb->CommandData.DataBufferArray->DataUsed = 0;

            // Complete the IRP
            CompleteStreamSRB (
                    pStrmEx->pCurrentSrb,
                    StreamRequestComplete,
                    TRUE,
                    ReadyForNextStreamDataRequest);

            // Clear pCurrentSrb to indicate request has been completed
            InterlockedExchange ((PLONG) &pStrmEx->pCurrentSrb, 0);                
        }
        break;

    case KSSTATE_ACQUIRE:

        //
        // This is a KS only state, that has no correspondence in DirectShow
        // 
        break;

    case KSSTATE_PAUSE:

        //
        // On a transition to pause from acquire, start our timer running.
        //

        if (pStrmEx->KSState == KSSTATE_ACQUIRE || pStrmEx->KSState == KSSTATE_STOP) {  
            
            // Remember the time at which the clock was started

            pHwDevExt->QST_Start = VideoGetSystemTime();

            // And initialize the last frame timestamp

            pHwDevExt->QST_Now = pHwDevExt->QST_Start;

			// Fireup the codec HERE in preparation for receiving data & requests.

			// INSERT CODE HERE
        
		}
        break;

    case KSSTATE_RUN:

        // 
        // Begin Streaming.
        //

        // Remember the time at which the clock was started

        pHwDevExt->QST_Start = VideoGetSystemTime();

        // Zero the frame info, it should be reset when the first sample arrives.

        RtlZeroMemory (&pStrmEx->FrameInfo, sizeof (pStrmEx->FrameInfo));

        // Reset the discontinuity flag

        pStrmEx->fDiscontinuity = FALSE;

        break;

    } // end switch (pSrb->CommandData.StreamState)  
    
    //
    // Remember the state of this stream
    //

    pStrmEx->KSState = pSrb->CommandData.StreamState;

    CDebugPrint(DebugLevelTrace,( CODECNAME ":<---VideoSetState(pSrb=%x)\n", pSrb));
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

VOID 
VideoGetState(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX     pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->VideoGetState(pSrb=%x)\n", pSrb));

    pSrb->CommandData.StreamState = pStrmEx->KSState;
    pSrb->ActualBytesTransferred = sizeof (KSSTATE);

    // A very odd rule:
    // When transitioning from stop to pause, DShow tries to preroll
    // the graph.  Capture sources can't preroll, and indicate this
    // by returning VFW_S_CANT_CUE in user mode.  To indicate this
    // condition from drivers, they must return STATUS_NO_DATA_DETECTED

    if (pStrmEx->KSState == KSSTATE_PAUSE) {
       pSrb->Status = STATUS_NO_DATA_DETECTED;
    }

    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---VideoGetState(pSrb=%x)=%d\n", pSrb, pStrmEx->KSState));
}


/*
** VideoStreamGetConnectionProperty()
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

VOID
VideoStreamGetConnectionProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    CDebugPrint(DebugLevelTrace,
		( CODECNAME ":--->VideoStreamGetConnectionProperty(pSrb=%x)\n", 
		pSrb));

    pSrb->ActualBytesTransferred = 0;

    switch (Id) 
	{
		case KSPROPERTY_CONNECTION_ALLOCATORFRAMING:
		{
            PKSALLOCATOR_FRAMING Framing = 
                (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;

            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamGetConnectionProperty : KSPROPERTY_CONNECTION_ALLOCATORFRAMING\n"));

			Framing->RequirementsFlags =
                KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY |
                KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
                KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY;
            Framing->PoolType = NonPagedPool;
            Framing->Frames = 1;
	    Framing->FrameSize = pStrmEx->OpenedFormat.SampleSize;
            Framing->FileAlignment = 0;         // None OR FILE_QUAD_ALIGNMENT-1 OR PAGE_SIZE-1; 
            Framing->Reserved = 0;
            pSrb->ActualBytesTransferred = sizeof (KSALLOCATOR_FRAMING);
	        break;
        }
        
		default:
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamGetConnectionProperty : Unknown Property Id=%d\n", Id));
			CDEBUG_BREAK();
			break;
    }

    CDebugPrint(DebugLevelTrace,
		( CODECNAME ":<---VideoStreamGetConnectionProperty(pSrb=%x)\n", 
		pSrb));
}

/*
** VideoStreamGetVBIFilteringProperty()
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

VOID
VideoStreamGetVBIFilteringProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX                   pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    ULONG nBytes = pSPD->PropertyOutputSize;    // Size of the app buffer  

    CDebugPrint(DebugLevelTrace,
		( CODECNAME ":--->VideoStreamGetVBIFilteringProperty(pSrb=%x)\n", 
		pSrb));

    pSrb->ActualBytesTransferred = 0;

    switch (Id) 
	{
		case KSPROPERTY_VBICODECFILTERING_SCANLINES_REQUESTED_BIT_ARRAY:
		{
            PKSPROPERTY_VBICODECFILTERING_SCANLINES_S Property =
                (PKSPROPERTY_VBICODECFILTERING_SCANLINES_S) pSPD->PropertyInfo;

            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamGetVBIFilteringProperty : KSPROPERTY_VBICODECFILTERING_SCANLINES_REQUESTED_BIT_ARRAY\n"));
            nBytes = min( nBytes, sizeof( pStrmEx->ScanlinesRequested ) );
            RtlCopyMemory( &Property->Scanlines, &pStrmEx->ScanlinesRequested, nBytes );
            pSrb->ActualBytesTransferred = nBytes;
	        break;
        }
        
		case KSPROPERTY_VBICODECFILTERING_SCANLINES_DISCOVERED_BIT_ARRAY:
		{
            PKSPROPERTY_VBICODECFILTERING_SCANLINES_S Property =
                (PKSPROPERTY_VBICODECFILTERING_SCANLINES_S) pSPD->PropertyInfo;

            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamGetVBIFilteringProperty : KSPROPERTY_VBICODECFILTERING_SCANLINES_DISCOVERED_BIT_ARRAY\n"));
            nBytes = min( nBytes, sizeof( pStrmEx->ScanlinesDiscovered ) );
            RtlCopyMemory( &Property->Scanlines, &pStrmEx->ScanlinesDiscovered, nBytes );
            pSrb->ActualBytesTransferred = nBytes;
            // Clear the data after the read so that it's always "fresh"
            RtlZeroMemory( &pStrmEx->ScanlinesDiscovered, nBytes );
	        break;
        }
        
		case KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_REQUESTED_BIT_ARRAY:
		{
            PKSPROPERTY_VBICODECFILTERING_NABTS_SUBSTREAMS_S Property =
                (PKSPROPERTY_VBICODECFILTERING_NABTS_SUBSTREAMS_S) pSPD->PropertyInfo;

            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamGetVBIFilteringProperty : KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_REQUESTED_BIT_ARRAY\n"));
            nBytes = min( nBytes, sizeof( pStrmEx->SubstreamsRequested ) );
            RtlCopyMemory( &Property->Substreams, &pStrmEx->SubstreamsRequested, nBytes );
            pSrb->ActualBytesTransferred = nBytes;
	        break;
        }
        
		case KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_DISCOVERED_BIT_ARRAY:
		{
            PKSPROPERTY_VBICODECFILTERING_NABTS_SUBSTREAMS_S Property =
                (PKSPROPERTY_VBICODECFILTERING_NABTS_SUBSTREAMS_S) pSPD->PropertyInfo;

            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamGetVBIFilteringProperty : KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_DISCOVERED_BIT_ARRAY\n"));
            nBytes = min( nBytes, sizeof( pStrmEx->SubstreamsDiscovered ) );
            RtlCopyMemory( &Property->Substreams, &pStrmEx->SubstreamsDiscovered, nBytes );
            pSrb->ActualBytesTransferred = nBytes;
            // Clear the data after the read so that it's always "fresh"
            RtlZeroMemory( &pStrmEx->SubstreamsDiscovered, nBytes );
	        break;
        }
        
		default:
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamGetVBIFilteringProperty : Unknown Property Id=%d\n", Id));
			CDEBUG_BREAK();
			break;
    }

    CDebugPrint(DebugLevelTrace,
		( CODECNAME ":<---VideoStreamGetVBIFilteringProperty(pSrb=%x)\n", 
		pSrb));
}

/*
** VideoStreamSetVBIFilteringProperty()
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

VOID
VideoStreamSetVBIFilteringProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    ULONG nBytes = pSPD->PropertyOutputSize;    // Size of the app buffer  

    CDebugPrint(DebugLevelTrace,
		( CODECNAME ":--->VideoStreamSetVBIFilteringProperty(pSrb=%x)\n", 
		pSrb));

    pSrb->ActualBytesTransferred = 0;

    switch (Id) 
	{
		case KSPROPERTY_VBICODECFILTERING_SCANLINES_REQUESTED_BIT_ARRAY:
		{
            PKSPROPERTY_VBICODECFILTERING_SCANLINES_S Property =
                (PKSPROPERTY_VBICODECFILTERING_SCANLINES_S) pSPD->PropertyInfo;

            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamSetVBIFilteringProperty : KSPROPERTY_VBICODECFILTERING_SCANLINES_REQUESTED_BIT_ARRAY\n"));
            nBytes = min( nBytes, sizeof( pStrmEx->ScanlinesRequested ) );
            RtlCopyMemory( &pStrmEx->ScanlinesRequested, &Property->Scanlines, nBytes );
            pSrb->ActualBytesTransferred = nBytes;
	        break;
        }
        
		case KSPROPERTY_VBICODECFILTERING_SCANLINES_DISCOVERED_BIT_ARRAY:
		{
            PKSPROPERTY_VBICODECFILTERING_SCANLINES_S Property =
                (PKSPROPERTY_VBICODECFILTERING_SCANLINES_S) pSPD->PropertyInfo;

            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamSetVBIFilteringProperty : KSPROPERTY_VBICODECFILTERING_SCANLINES_DISCOVERED_BIT_ARRAY\n"));
            nBytes = min( nBytes, sizeof(pStrmEx->ScanlinesDiscovered ) );
            RtlCopyMemory( &pStrmEx->ScanlinesDiscovered, &Property->Scanlines, nBytes );
            pSrb->ActualBytesTransferred = nBytes;
	        break;
        }
        
		case KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_REQUESTED_BIT_ARRAY:
		{
            PKSPROPERTY_VBICODECFILTERING_NABTS_SUBSTREAMS_S Property =
                (PKSPROPERTY_VBICODECFILTERING_NABTS_SUBSTREAMS_S) pSPD->PropertyInfo;

            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamSetVBIFilteringProperty : KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_REQUESTED_BIT_ARRAY\n"));
            nBytes = min( nBytes, sizeof(pStrmEx->SubstreamsRequested ) );
            RtlCopyMemory( &pStrmEx->SubstreamsRequested, &Property->Substreams, nBytes );
            pSrb->ActualBytesTransferred = nBytes;
	        break;
        }
        
		case KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_DISCOVERED_BIT_ARRAY:
		{
            PKSPROPERTY_VBICODECFILTERING_NABTS_SUBSTREAMS_S Property =
                (PKSPROPERTY_VBICODECFILTERING_NABTS_SUBSTREAMS_S) pSPD->PropertyInfo;

            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamSetVBIFilteringProperty : KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_DISCOVERED_BIT_ARRAY\n"));
            nBytes = min( nBytes, sizeof(pStrmEx->SubstreamsDiscovered ) );
            RtlCopyMemory( &pStrmEx->SubstreamsDiscovered, &Property->Substreams, nBytes );
            pSrb->ActualBytesTransferred = nBytes;
	        break;
        }
        
		default:
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            CDebugPrint(DebugLevelVerbose,
				( CODECNAME ": VideoStreamSetVBIFilteringProperty : Unknown Property Id=%d\n", Id));
			CDEBUG_BREAK();
			break;
    }

    CDebugPrint(DebugLevelTrace,
		( CODECNAME ":<---VideoStreamSetVBIFilteringProperty(pSrb=%x)\n", 
		pSrb));
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

ULONGLONG 
VideoGetSystemTime()
{
    ULONGLONG ticks;
    ULONGLONG rate;

    CDebugPrint(DebugLevelTrace,( CODECNAME ":--->VideoGetSystemTime()\n"));

    ticks = (ULONGLONG)KeQueryPerformanceCounter((PLARGE_INTEGER)&rate).QuadPart;

    //
    // convert from ticks to 100ns clock
    //

    ticks = (ticks & 0xFFFFFFFF00000000) / rate * 10000000 +
            (ticks & 0x00000000FFFFFFFF) * 10000000 / rate;

    CDebugPrint(DebugLevelTrace,( CODECNAME ":<---VideoGetSystemTime()\n"));

    return(ticks);
}



//==========================================================================;
//                   Clock Handling Routines
//==========================================================================;


/*
** VideoIndicateMasterClock ()
**
**    This function is used to provide us with a handle to the clock to use.
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID 
VideoIndicateMasterClock(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    CDebugPrint(DebugLevelTrace,( CODECNAME ":--->VideoIndicateMasterClock(pSrb=%x)\n", pSrb));

    pStrmEx->hClock = pSrb->CommandData.MasterClockHandle;

    CDebugPrint(DebugLevelTrace,( CODECNAME ":<---VideoIndicateMasterClock(pSrb=%x)\n", pSrb));
}
