//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1998  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#include <strmini.h>
#include <ksmedia.h>
#include "codmain.h"
#include "codstrm.h"
#include "codprop.h"
#include "coddebug.h"

/*
** DriverEntry()
**
**   This routine is called when the driver is first loaded by PnP.
**   It in turn, calls upon the stream class to perform registration services.
**
** Arguments:
**
**   DriverObject - 
**          Driver object for this driver 
**
**   RegistryPath - 
**          Registry path string for this driver's key
**
** Returns:
**
**   Results of StreamClassRegisterAdapter()
**
** Side Effects:  none
*/

ULONG 
DriverEntry( IN PDRIVER_OBJECT DriverObject,
			 IN PUNICODE_STRING RegistryPath )
{
    ULONG					status = 0;
    HW_INITIALIZATION_DATA	HwInitData;
    UNICODE_STRING          DeviceNameString, SymbolicNameString;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->DriverEntry(DriverObject=%x,RegistryPath=%x)\n", 
				DriverObject, RegistryPath));

    RtlZeroMemory(&HwInitData, sizeof(HwInitData));

    HwInitData.HwInitializationDataSize = sizeof(HwInitData);

    /*CDEBUG_BREAK();*/

    //
    // Set the codec entry points for the driver
    //

    HwInitData.HwInterrupt              = NULL; // HwInterrupt is only for HW devices

    HwInitData.HwReceivePacket          = CodecReceivePacket;
    HwInitData.HwCancelPacket           = CodecCancelPacket;
    HwInitData.HwRequestTimeoutHandler  = CodecTimeoutPacket;

    HwInitData.DeviceExtensionSize      = sizeof(HW_DEVICE_EXTENSION);
    HwInitData.PerRequestExtensionSize  = 0; 
    HwInitData.FilterInstanceExtensionSize = 0;
    HwInitData.PerStreamExtensionSize   = sizeof(STREAMEX); 
    HwInitData.BusMasterDMA             = FALSE;  
    HwInitData.Dma24BitAddresses        = FALSE;
    HwInitData.BufferAlignment          = 3;
    HwInitData.TurnOffSynchronization   = FALSE;
    HwInitData.DmaBufferSize            = 0;

    CDebugPrint(DebugLevelVerbose,(CODECNAME ": StreamClassRegisterAdapter\n"));

    status = StreamClassRegisterAdapter(DriverObject, RegistryPath, &HwInitData);

    RtlInitUnicodeString( &SymbolicNameString, CodecSymbolicName );
    RtlInitUnicodeString( &DeviceNameString, CodecDeviceName );
    IoCreateSymbolicLink( &SymbolicNameString, &DeviceNameString );

    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---DriverEntry(DriverObject=%x,RegistryPath=%x)=%d\n",
			    DriverObject, RegistryPath, status));

    return status;     
}

//==========================================================================;
//                   Codec Request Handling Routines
//==========================================================================;

/*
** CodecInitialize()
**
**   This routine is called when an SRB_INITIALIZE_DEVICE request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block for the Initialize command
**
** Returns:
**
** Side Effects:  none
*/

BOOLEAN 
CodecInitialize ( IN OUT PHW_STREAM_REQUEST_BLOCK pSrb )
{
    BOOLEAN							bStatus = FALSE;
    PPORT_CONFIGURATION_INFORMATION ConfigInfo = pSrb->CommandData.ConfigInfo;
    PHW_DEVICE_EXTENSION			pHwDevExt =
        (PHW_DEVICE_EXTENSION)ConfigInfo->HwDeviceExtension;
    int                             scanline, substream;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecInitialize(pSrb=%x)\n",pSrb));

    if (ConfigInfo->NumberOfAccessRanges == 0) 
    {
        CDebugPrint(DebugLevelVerbose,(CODECNAME ": CodecInitialize\n"));

        ConfigInfo->StreamDescriptorSize = sizeof (HW_STREAM_HEADER) +
            DRIVER_STREAM_COUNT * sizeof (HW_STREAM_INFORMATION);

        // These are the driver defaults for scanline filtering.
        // Modify these WHEN you change the codec type to be more correct.

        for ( scanline = 10; scanline <= 19; scanline++ )
            SETBIT( pHwDevExt->ScanlinesRequested.DwordBitArray, scanline );

        // These are the driver defaults for subtream filtering. 
        // Modify these WHEN you change the codec type (These are MS IP/NABTS GROUP ID specific)

        for ( substream = 0x800; substream <= 0x81F; substream++ )
            SETBIT( pHwDevExt->SubstreamsRequested.SubstreamMask, substream );
        
        pSrb->Status = STATUS_SUCCESS;
        bStatus = TRUE;
    }
    else
    {
        CDebugPrint(DebugLevelError,(CODECNAME ": illegal config info\n"));
        pSrb->Status = STATUS_NO_SUCH_DEVICE;
    }

    CDebugPrint(DebugLevelTrace,
                (CODECNAME ":<---CodecInitialize(pSrb=%x)=%d\n", pSrb, bStatus));
    return (bStatus);
}

/*
** CodecUnInitialize()
**
**   This routine is called when an SRB_UNINITIALIZE_DEVICE request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block for the UnInitialize command
**
** Returns:
**
** Side Effects:  none
*/

BOOLEAN 
CodecUnInitialize ( 
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecUnInitialize(pSrb=%x)\n",pSrb));

    pSrb->Status = STATUS_SUCCESS;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CodecUnInitialize(pSrb=%x)\n",pSrb));

    return TRUE;
}


/*
** CodecOpenStream()
**
**   This routine is called when an OpenStream SRB request is received.
**   A stream is identified by a stream number, which indexes an array
**   of KSDATARANGE structures.  The particular KSDATAFORMAT format to
**   be used is also passed in, which should be verified for validity.
**   
** Arguments:
**
**   pSrb - pointer to stream request block for the Open command
**
** Returns:
**
** Side Effects:  none
*/

VOID 
CodecOpenStream( PHW_STREAM_REQUEST_BLOCK pSrb )
{
    //
    // the stream extension structure is allocated by the stream class driver
    //

    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    int                     StreamNumber = pSrb->StreamObject->StreamNumber;

    PKSDATAFORMAT           pKSDataFormat =
				  (PKSDATAFORMAT)pSrb->CommandData.OpenFormat;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecOpenStream(pSrb=%x)\n", pSrb));
    CDebugPrint(DebugLevelVerbose,(CODECNAME ": CodecOpenStream : StreamNumber=%d\n", StreamNumber));

    RtlZeroMemory(pStrmEx, sizeof (STREAMEX));
    
    //
    // check that the stream index requested isn't too high
    // or that the maximum number of instances hasn't been exceeded
    //

    if ( 0 <= StreamNumber && StreamNumber < DRIVER_STREAM_COUNT ) 
	{
        unsigned StreamInstance;
        unsigned maxInstances =
                  Streams[StreamNumber].hwStreamInfo.NumberOfPossibleInstances;

		// Search for next open slot
	    for (StreamInstance=0; StreamInstance < maxInstances; ++StreamInstance)
		{
			if (pHwDevExt->pStrmEx[StreamNumber][StreamInstance] == NULL)
				break;
		}

	    if (StreamInstance < maxInstances)
		{
			if (CodecVerifyFormat(pKSDataFormat, StreamNumber)) 
			{
				CASSERT (pHwDevExt->pStrmEx[StreamNumber][StreamInstance] == NULL);

				// Maintain an array of all the StreamEx structures in the HwDevExt
				// so that we can reference IRPs from any stream

				pHwDevExt->pStrmEx[StreamNumber][StreamInstance] = pStrmEx;
    
                // Save the Stream Format in the Stream Extension as well.
                pStrmEx->OpenedFormat = *pKSDataFormat;

				// Set up pointers to the handlers for the stream data and control handlers

				pSrb->StreamObject->ReceiveDataPacket = 
						(PVOID) Streams[StreamNumber].hwStreamObject.ReceiveDataPacket;
				pSrb->StreamObject->ReceiveControlPacket = 
						(PVOID) Streams[StreamNumber].hwStreamObject.ReceiveControlPacket;
    
				//
				// The DMA flag must be set when the device will be performing DMA directly
				// to the data buffer addresses passed in to the ReceiceDataPacket routines.
				//

				pSrb->StreamObject->Dma = Streams[StreamNumber].hwStreamObject.Dma;

				//
				// The PIO flag must be set when the mini driver will be accessing the data
				// buffers passed in using logical addressing
				//

				pSrb->StreamObject->Pio = Streams[StreamNumber].hwStreamObject.Pio;
               
				pSrb->StreamObject->Allocator = Streams[StreamNumber].hwStreamObject.Allocator;
               
				//
				// How many extra bytes will be passed up from the driver for each frame?
				//

				pSrb->StreamObject->StreamHeaderMediaSpecific = 
					Streams[StreamNumber].hwStreamObject.StreamHeaderMediaSpecific;

				pSrb->StreamObject->StreamHeaderWorkspace =
					Streams[StreamNumber].hwStreamObject.StreamHeaderWorkspace;

				//
				// Indicate the clock support available on this stream
				//

				pSrb->StreamObject->HwClockObject = 
					Streams[StreamNumber].hwStreamObject.HwClockObject;

				// 
				// Increment the instance count on this stream
				//
				pStrmEx->StreamInstance = StreamInstance;
				pHwDevExt->ActualInstances[StreamNumber]++;


				// Retain a private copy of the HwDevExt and StreamObject in the stream extension
				// so we can use a timer 

				pStrmEx->pHwDevExt = pHwDevExt;                     // For timer use
				pStrmEx->pStreamObject = pSrb->StreamObject;        // For timer use

                // Copy the default filtering settings
                
                pStrmEx->ScanlinesRequested = pHwDevExt->ScanlinesRequested;
                pStrmEx->SubstreamsRequested = pHwDevExt->SubstreamsRequested;
			}
			else
			{
				CDebugPrint(DebugLevelError,
					(CODECNAME ": CodecOpenStream : Invalid Stream Format=%x\n", 
					pKSDataFormat ));
				pSrb->Status = STATUS_INVALID_PARAMETER;
			}
		}
		else
		{
		    CDebugPrint(DebugLevelError,
				(CODECNAME ": CodecOpenStream : Too Many Instances=%d\n", 
				pHwDevExt->ActualInstances[StreamNumber] ));
	        pSrb->Status = STATUS_INVALID_PARAMETER;
		}

	}
	else
	{
	    CDebugPrint(DebugLevelError,
			(CODECNAME ": CodecOpenStream : Invalid StreamNumber=%d\n", 
			StreamNumber ));
        pSrb->Status = STATUS_INVALID_PARAMETER;
    }

    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CodecOpenStream(pSrb=%x)\n", pSrb));
}

/*
** CodecCloseStream()
**
**   Close the requested data stream
**
** Arguments:
**
**   pSrb the request block requesting to close the stream
**
** Returns:
**
** Side Effects:  none
*/

VOID 
CodecCloseStream (
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PKSDATAFORMAT           pKSDataFormat = pSrb->CommandData.OpenFormat;
    ULONG                   StreamNumber = pSrb->StreamObject->StreamNumber;
    ULONG                   StreamInstance = pStrmEx->StreamInstance;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecCloseStream(pSrb=%x)\n", pSrb));

    // CDEBUG_BREAK(); // Uncomment this code to break here.


    // Decrement count on this stream (the actual number of infinite pins)
    pHwDevExt->ActualInstances[StreamNumber] -= 1;

    CASSERT (pHwDevExt->pStrmEx[StreamNumber][StreamInstance] != 0);

    pHwDevExt->pStrmEx[StreamNumber][StreamInstance] = 0;

    //
    // the minidriver may wish to free any resources that were allocate at
    // open stream time etc.
    //
    pStrmEx->hMasterClock = NULL;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CodecCloseStream(pSrb=%x)\n", pSrb));
}


/*
** CodecStreamInfo()
**
**   Returns the information of all streams that are supported by the
**   mini-driver
**
** Arguments:
**
**   pSrb - Pointer to the STREAM_REQUEST_BLOCK
**        pSrb->HwDeviceExtension - will be the hardware device extension for
**                                  as initialised in HwInitialise
**
** Returns:
**
** Side Effects:  none
*/

VOID 
CodecStreamInfo (
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{

    int j; 
    
    PHW_DEVICE_EXTENSION pHwDevExt =
        ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);

    //
    // pick up the pointer to header which preceeds the stream info structs
    //

    PHW_STREAM_HEADER pstrhdr =
            (PHW_STREAM_HEADER)&(pSrb->CommandData.StreamBuffer->StreamHeader);

     //
     // pick up the pointer to the array of stream information data structures
     //

    PHW_STREAM_INFORMATION pstrinfo =
            (PHW_STREAM_INFORMATION)&(pSrb->CommandData.StreamBuffer->StreamInfo);


	CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecStreamInfo(pSrb=%x)\n", pSrb));
  
    // 
    // verify that the buffer is large enough to hold our return data
    //

    CASSERT (pSrb->NumberOfBytesToTransfer >= 
            sizeof (HW_STREAM_HEADER) +
            sizeof (HW_STREAM_INFORMATION) * DRIVER_STREAM_COUNT);

     //
     // Set the header
     // 

#ifdef GLOBAL_PROPERTIES
     StreamHeader.NumDevPropArrayEntries = NUMBER_OF_CODEC_PROPERTY_SETS;
     StreamHeader.DevicePropertiesArray = (PKSPROPERTY_SET) CodecPropertyTable; 
#else // !GLOBAL_PROPERTIES
     StreamHeader.NumDevPropArrayEntries = 0;
     StreamHeader.DevicePropertiesArray = (PKSPROPERTY_SET)NULL; 
#endif // GLOBAL_PROPERTIES
     *pstrhdr = StreamHeader;

     // 
     // stuff the contents of each HW_STREAM_INFORMATION struct 
     //

     for (j = 0; j < DRIVER_STREAM_COUNT; j++) {
        *pstrinfo++ = Streams[j].hwStreamInfo;
     }

     CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CodecStreamInfo(pSrb=%x)\n", pSrb));
}


/*
** CodecReceivePacket()
**
**   Main entry point for receiving codec based request SRBs.  This routine
**   will always be called at High Priority.
**
**   Note: This is an asyncronous entry point.  The request does not complete
**         on return from this function, the request only completes when a
**         StreamClassDeviceNotification on this request block, of type
**         DeviceRequestComplete, is issued.
**
** Arguments:
**
**   pSrb - Pointer to the STREAM_REQUEST_BLOCK
**        pSrb->HwDeviceExtension - will be the hardware device extension for
**                                  as initialised in HwInitialise
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI 
CodecReceivePacket(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{

    //
    // pick up the device extension
    //
    PHW_DEVICE_EXTENSION pHwDevExt =
            ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);

    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecReceivePacket(pSrb=%x)\n", pSrb));

    //
    // Assume success
    //

    pSrb->Status = STATUS_SUCCESS;
 
    //
    // determine the type of packet.
    //

    CDebugPrint(DebugLevelVerbose,
		(CODECNAME ": CodecReceivePacket: pSrb->Command=0x%x\n", 
		pSrb->Command));

    switch (pSrb->Command)
    {

    case SRB_INITIALIZE_DEVICE:

        // open the device
        
        CodecInitialize(pSrb);

        break;

    case SRB_UNINITIALIZE_DEVICE:

        // close the device.  

        CodecUnInitialize(pSrb);

        break;

    case SRB_OPEN_STREAM:

        // open a stream

        CodecOpenStream(pSrb);

        break;

    case SRB_CLOSE_STREAM:

        // close a stream

        CodecCloseStream(pSrb);

        break;

    case SRB_GET_STREAM_INFO:

        //
        // return a block describing all the streams
        //

        CodecStreamInfo(pSrb);

        break;

    case SRB_GET_DATA_INTERSECTION:

        //
        // Return a format, given a range
        //

        CodecFormatFromRange(pSrb);

        break;

        // We should never get the following since this is a single instance
        // device
    case SRB_OPEN_DEVICE_INSTANCE:
    case SRB_CLOSE_DEVICE_INSTANCE:
        CDebugPrint(DebugLevelWarning,(CODECNAME ": Received SRB_OPEN/CLOSE_DEVICE_INSTANCE command\n"));
        CDEBUG_BREAK();
		// Fall through to not implemented

    case SRB_INITIALIZATION_COMPLETE:	// this one we don't care about
    case SRB_UNKNOWN_DEVICE_COMMAND:	// This one we don't care about

        pSrb->Status = STATUS_NOT_IMPLEMENTED;

        break;

    case SRB_GET_DEVICE_PROPERTY:

        // Get codec wide properties

        CodecGetProperty (pSrb);

        break;        

    case SRB_SET_DEVICE_PROPERTY:

        // Set codec wide properties

        CodecSetProperty (pSrb);

        break;

    case SRB_UNKNOWN_STREAM_COMMAND:

		// Note that this occured, but do not stop for it.
        CDebugPrint(DebugLevelWarning,(CODECNAME ": Received SRB_UNKNOWN_STREAM_COMMAND\n"));
        pSrb->Status = STATUS_NOT_IMPLEMENTED;

		break;

	default:

        CDebugPrint(DebugLevelWarning,(CODECNAME ": CodecReceivePacket: Received _unknown_ command!\n"));
        CDEBUG_BREAK();

        //
        // this is a request that we do not understand.  Indicate invalid
        // command and complete the request
        //
        pSrb->Status = STATUS_NOT_IMPLEMENTED;

    }

    //
    // NOTE:
    //
    // all of the commands that we do, or do not understand can all be completed
    // syncronously at this point, so we can use a common callback routine here.
    // If any of the above commands require asyncronous processing, this will
    // have to change
    //

	CDebugPrint(DebugLevelVerbose,
		(CODECNAME ": CodecReceivePacket : DeviceRequestComplete(pSrb->Status=0x%x)\n", 
		pSrb->Status));

    CompleteDeviceSRB (
		pSrb, 
        DeviceRequestComplete,
        TRUE);
    
    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CodecReceivePacket(pSrb=%x)\n", pSrb));
}

/*
** CodecCancelPacket ()
**
**   Request to cancel a packet that is currently in process in the minidriver
**
** Arguments:
**
**   pSrb - pointer to request packet to cancel
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI 
CodecCancelPacket(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = (PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension;
    PSTREAMEX               pStrmEx;
    ULONG                   j, k;
    BOOL                    bFound = FALSE;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecCancelPacket(pSrb=%x)\n", pSrb));

    //
    // Run through all the streams the driver has available
    //

    for (j = 0; j < DRIVER_STREAM_COUNT; j++) 
    {

        for( k = 0; k < pHwDevExt->ActualInstances[j]; k++ )
        {
            //
            // Check to see if the stream is in use
            //

            if (pStrmEx = (PSTREAMEX) pHwDevExt->pStrmEx[j][k])
            {
            
                //
                // Check whether the SRB to cancel is in use by this stream
                //

                if (pStrmEx->pCurrentSrb == pSrb) 
                {
    
                    // Forget we're working on this SRB

                    pStrmEx->pCurrentSrb = NULL;

                    pSrb->Status = STATUS_CANCELLED;

                    bFound = TRUE;

                    //
                    // it is necessary to call the request back correctly.  Determine which
                    // type of command it is
                    //
				    CDebugPrint(DebugLevelVerbose,
					    (CODECNAME ": CodecCancelPacket : pSrb->Flags=0x%x)\n", 
					    pSrb->Flags));

                    switch (pSrb->Flags & (SRB_HW_FLAGS_DATA_TRANSFER |
                                SRB_HW_FLAGS_STREAM_REQUEST))
                    {

                    //
                    // find all stream commands, and do stream notifications
                    //

                    case SRB_HW_FLAGS_STREAM_REQUEST | SRB_HW_FLAGS_DATA_TRANSFER:
    
					    CDebugPrint(DebugLevelVerbose,
						    (CODECNAME ":StreamRequestComplete(ReadyForNextStreamDataRequest,pSrb->Status=0x%x)\n", 
						    pSrb->Status));

                        CompleteStreamSRB (
                            pSrb, 
                            StreamRequestComplete,
                            TRUE,
                            ReadyForNextStreamDataRequest);

                        break;

                    case SRB_HW_FLAGS_STREAM_REQUEST:

					    CDebugPrint(DebugLevelVerbose,
						    (CODECNAME ":StreamRequestComplete(ReadyForNextStreamControlRequest,pSrb->Status=0x%x)\n", 
						    pSrb->Status));

                        CompleteStreamSRB (
                            pSrb, 
                            StreamRequestComplete,
                            TRUE,
                            ReadyForNextStreamControlRequest);

                        break;

                    default:

                        //
                        // this must be a device request.  Use device notifications
                        //
					    CDebugPrint(DebugLevelVerbose,
						    (CODECNAME ":DeviceRequestComplete(pSrb->Status=0x%x)\n", 
						    pSrb->Status));

                        CompleteDeviceSRB (
                            pSrb, 
                            DeviceRequestComplete,
                            TRUE);

                    }
                } // if the SRB in process matches the cancel request
            } // if the stream is open
        } // for all instances of each stream type
    } // for all streams

	if ( !bFound )
	{
		CDebugPrint(DebugLevelWarning,
			(CODECNAME ":CodecCancelPacket(pSrb=0x%x) Failed find SRB to cancel!\n", 
			pSrb));
	}

    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CodecCancelPacket(pSrb=%x)\n", pSrb));
}

/*
** CodecTimeoutPacket()
**
**   This routine is called when a packet has been in the minidriver for
**   too long.  The codec must decide what to do with the packet
**
** Arguments:
**
**   pSrb - pointer to the request packet that timed out
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI  
CodecTimeoutPacket(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecTimeoutPacket(pSrb=%x)\n", pSrb));

    //
    // if we timeout while playing, then we need to consider this
    // condition an error, and reset the hardware, and reset everything
    // as well as cancelling this and all requests
    //

    //
    // if we are not playing, and this is a CTRL request, we still
    // need to reset everything as well as cancelling this and all requests
    //

    //
    // if this is a data request, and the device is paused, we probably have
    // run out of data buffer, and need more time, so just reset the timer,
    // and let the packet continue
    //

//    pSrb->TimeoutCounter = pSrb->TimeoutOriginal;
    pSrb->TimeoutCounter = 0;
    
    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CodecTimeoutPacket(pSrb=%x)\n", pSrb));
}

/*
** CompleteStreamSRB ()
**
**   This routine is called when a packet is being completed.
**   The optional second notification type is used to indicate ReadyForNext
**
** Arguments:
**
**   pSrb - pointer to the request packet that timed out
**
**   NotificationType1 - what kind of notification to return
**
**   NotificationType2 - what kind of notification to return (may be 0)
**
**
** Returns:
**
** Side Effects:  none
*/

VOID 
CompleteStreamSRB (
     IN PHW_STREAM_REQUEST_BLOCK pSrb, 
     STREAM_MINIDRIVER_STREAM_NOTIFICATION_TYPE NotificationType1,
     BOOL fUseNotification2,
     STREAM_MINIDRIVER_STREAM_NOTIFICATION_TYPE NotificationType2
    )
{
    CDebugPrint(DebugLevelTrace,
		(CODECNAME ":--->CompleteStreamSRB(pSrb=%x)\n", pSrb));

	CDebugPrint(DebugLevelVerbose,
		(CODECNAME ": CompleteStreamSRB : NotificationType1=%d\n", 
		NotificationType1 ));

    StreamClassStreamNotification(
            NotificationType1,
            pSrb->StreamObject,
            pSrb);

    if (fUseNotification2) 
	{            
		// ReadyForNext

		CDebugPrint(DebugLevelVerbose,
			(CODECNAME ": CompleteStreamSRB : NotificationType2=%d\n", 
			NotificationType2 ));

		StreamClassStreamNotification(
			NotificationType2,
			pSrb->StreamObject);
	}

    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CompleteStreamSRB(pSrb=%x)\n", pSrb));
}

/*
** CompleteDeviceSRB ()
**
**   This routine is called when a packet is being completed.
**   The optional second notification type is used to indicate ReadyForNext
**
** Arguments:
**
**   pSrb - pointer to the request packet that timed out
**
**   NotificationType - what kind of notification to return
**
**   fReadyForNext - Send the "ReadyForNextSRB" 
**
**
** Returns:
**
** Side Effects:  none
*/

VOID
CompleteDeviceSRB (
     IN PHW_STREAM_REQUEST_BLOCK pSrb, 
     IN STREAM_MINIDRIVER_DEVICE_NOTIFICATION_TYPE NotificationType,
     BOOL fReadyForNext
    )
{
    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CompleteDeviceSRB(pSrb=%x)\n", pSrb));

	CDebugPrint(DebugLevelVerbose,
		(CODECNAME ": CompleteDeviceSRB : NotificationType=%d\n", 
		NotificationType ));

    StreamClassDeviceNotification(
            NotificationType,
            pSrb->HwDeviceExtension,
            pSrb);

    if (fReadyForNext) 
	{
		CDebugPrint(DebugLevelVerbose,
			(CODECNAME ": CompleteDeviceSRB : ReadyForNextDeviceRequest\n"));

		StreamClassDeviceNotification( 
			ReadyForNextDeviceRequest,
			pSrb->HwDeviceExtension);
    }

    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CompleteDeviceSRB(pSrb=%x)\n", pSrb));
}

/*
** CodecCompareGUIDsAndFormatSize()
**
**   Checks for a match on the three GUIDs and FormatSize
**
** Arguments:
**
**         IN DataRange1
**         IN DataRange2
**
** Returns:
** 
**   TRUE if all elements match
**   FALSE if any are different
**
** Side Effects:  none
*/

BOOL 
CodecCompareGUIDsAndFormatSize(
    IN PKSDATARANGE DataRange1,
    IN PKSDATARANGE DataRange2,
    BOOLEAN bCheckSize
    )
{
    BOOL	rval = FALSE;

    CDebugPrint(DebugLevelTrace,
		(CODECNAME ":--->CodecCompareGUIDsAndFormatSize(DataRange1=%x,DataRange2=%x,bCheckSize=%s)\n", 
        DataRange1, DataRange2, bCheckSize ? "TRUE":"FALSE"));

	if ( IsEqualGUID(&DataRange1->MajorFormat, &KSDATAFORMAT_TYPE_WILDCARD)
	  || IsEqualGUID(&DataRange2->MajorFormat, &KSDATAFORMAT_TYPE_WILDCARD)
	  || IsEqualGUID(&DataRange1->MajorFormat, &DataRange2->MajorFormat) )
	{
		if ( !IsEqualGUID(&DataRange1->MajorFormat, &DataRange2->MajorFormat) )
		{
			CDebugPrint(DebugLevelVerbose,
				(CODECNAME ": CodecCompareGUIDsAndFormatSize : Matched MajorFormat Using Wildcard:\n\t[%s] vs. [%s]\n", 
				&DataRange1->MajorFormat, &DataRange2->MajorFormat ));
		}

		if ( IsEqualGUID(&DataRange1->SubFormat, &KSDATAFORMAT_SUBTYPE_WILDCARD)
		  || IsEqualGUID(&DataRange2->SubFormat, &KSDATAFORMAT_SUBTYPE_WILDCARD)
	      || IsEqualGUID(&DataRange1->SubFormat, &DataRange2->SubFormat) )
		{
			if ( !IsEqualGUID(&DataRange1->SubFormat, &DataRange2->SubFormat) )
			{
				CDebugPrint(DebugLevelVerbose,
					(CODECNAME ": CodecCompareGUIDsAndFormatSize : Matched SubFormat Using Wildcard:\n\t[%s] vs. [%s]\n", 
					&DataRange1->SubFormat, &DataRange2->SubFormat ));
			}

			if ( IsEqualGUID(&DataRange1->Specifier, &KSDATAFORMAT_SPECIFIER_WILDCARD)
			  || IsEqualGUID(&DataRange2->Specifier, &KSDATAFORMAT_SPECIFIER_WILDCARD)
			  || IsEqualGUID(&DataRange1->Specifier, &DataRange2->Specifier) )
			{
				if ( !IsEqualGUID(&DataRange1->Specifier, &DataRange2->Specifier) )
				{
					CDebugPrint(DebugLevelVerbose,
						(CODECNAME ": CodecCompareGUIDsAndFormatSize : Matched Specifier Using Wildcard:\n\t[%s] vs. [%s]\n", 
						&DataRange1->Specifier, &DataRange2->Specifier ));
				}

				if ( !bCheckSize || DataRange1->FormatSize == DataRange2->FormatSize)
				{
					rval = TRUE;
				}
				else
				{
					CDebugPrint(DebugLevelVerbose,
						(CODECNAME ": CodecCompareGUIDsAndFormatSize : FormatSize mismatch=%d vs. %d\n", 
						DataRange1->FormatSize, DataRange2->FormatSize ));
				}
			}
			else
			{
				CDebugPrint(DebugLevelVerbose,
					(CODECNAME ": CodecCompareGUIDsAndFormatSize : Specifier mismatch:\n\t[%s] vs. [%s]\n", 
					&DataRange1->Specifier, &DataRange2->Specifier ));
			}
		}
		else
		{
			CDebugPrint(DebugLevelVerbose,
				(CODECNAME ": CodecCompareGUIDsAndFormatSize : Subformat mismatch:\n\t[%s] vs. [%s]\n", 
				&DataRange1->SubFormat, &DataRange2->SubFormat ));
		}
	}
    else
	{
		CDebugPrint(DebugLevelVerbose,
			(CODECNAME ": CodecCompareGUIDsAndFormatSize : MajorFormat mismatch:\n\t[%s] vs. [%s]\n", 
			&DataRange1->MajorFormat, &DataRange2->MajorFormat ));
	}

    CDebugPrint(DebugLevelTrace,
		(CODECNAME ":<---CodecCompareGUIDsAndFormatSize(DataRange1=%x,DataRange2=%x,bCheckSize=%s)=%s\n", 
		DataRange1, DataRange2, bCheckSize ? "TRUE":"FALSE", rval? "TRUE":"FALSE"));

    return rval;
}

/*
** CodecVerifyFormat()
**
**   Checks the validity of a format request
**
** Arguments:
**
**   pKSDataFormat - pointer to a KS_DATAFORMAT_VBIINFOHEADER structure.
**
** Returns:
** 
**   TRUE if the format is supported
**   FALSE if the format cannot be suppored
**
** Side Effects:  none
*/

BOOL 
CodecVerifyFormat(IN KSDATAFORMAT *pKSDataFormat, UINT StreamNumber )
{
    BOOL	rval = FALSE;
    ULONG     FormatCount;
    PKS_DATARANGE_VIDEO ThisFormat;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecVerifyFormat(%x)\n", pKSDataFormat));
    
    for( FormatCount = 0; !rval && FormatCount < Streams[StreamNumber].hwStreamInfo.NumberOfFormatArrayEntries;
        FormatCount++ )
    {
        CDebugPrint(DebugLevelTrace,(CODECNAME , "Testing stream %d against format %x\n", StreamNumber, FormatCount ));
        
        ThisFormat = ( PKS_DATARANGE_VIDEO )Streams[StreamNumber].hwStreamInfo.StreamFormatsArray[FormatCount];

        if ( CodecCompareGUIDsAndFormatSize( pKSDataFormat, &ThisFormat->DataRange, FALSE ) )
        {
            // Our normal VBI input pin format; expand tests as necessary for other pins

            if( IsEqualGUID( &ThisFormat->DataRange.Specifier, &KSDATAFORMAT_SPECIFIER_VBI ) )
            {
                //
                // Do some VBI-specific tests, generalize this for different capture sources
                // And if you use the VBIINFOHEADER on any other pins (input or output)
                //
                PKS_DATAFORMAT_VBIINFOHEADER    pKSVBIDataFormat = ( PKS_DATAFORMAT_VBIINFOHEADER )pKSDataFormat;

                CDebugPrint(DebugLevelTrace,(CODECNAME , "This is a VBIINFOHEADER format pin.\n" ));

		//
		// Check VideoStandard, we only support NTSC_M
		//
		if (pKSVBIDataFormat->VBIInfoHeader.VideoStandard != KS_AnalogVideo_NTSC_M)
		{
		    CDebugPrint(DebugLevelVerbose,
			(CODECNAME ": CodecOpenStream : VideoStandard(%d) != NTSC_M\n", 
			 pKSVBIDataFormat->VBIInfoHeader.VideoStandard));
		}
		else if ( pKSVBIDataFormat->VBIInfoHeader.StartLine >= MIN_VBI_Y_SAMPLES )
			    {
        			if ( pKSVBIDataFormat->VBIInfoHeader.EndLine <= MAX_VBI_Y_SAMPLES )
        			{
        				if ( pKSVBIDataFormat->VBIInfoHeader.SamplesPerLine >= MIN_VBI_X_SAMPLES )
        				{
        				    if ( pKSVBIDataFormat->VBIInfoHeader.SamplesPerLine <= MAX_VBI_X_SAMPLES )
                            {
        					    rval = TRUE;
                            }
        				    else
        				    {
        					    CDebugPrint(DebugLevelVerbose,
        						    (CODECNAME ": CodecVerifyFormat : SamplesPerLine Too Large=%d vs. %d\n", 
        						    pKSVBIDataFormat->VBIInfoHeader.SamplesPerLine, MAX_VBI_X_SAMPLES ));
        				    }
        				}
        				else
        				{
        					CDebugPrint(DebugLevelVerbose,
        						(CODECNAME ": CodecVerifyFormat : SamplesPerLine Too Small=%d vs. %d\n", 
        						pKSVBIDataFormat->VBIInfoHeader.SamplesPerLine, MIN_VBI_X_SAMPLES ));
        				}
        			}
        			else
        			{
        				CDebugPrint(DebugLevelVerbose,
        					(CODECNAME ": CodecVerifyFormat : EndLine Too Large=%d vs. %d\n", 
        					pKSVBIDataFormat->VBIInfoHeader.EndLine, MAX_VBI_Y_SAMPLES ));
        			}
        		}
        		else
        		{
        			CDebugPrint(DebugLevelVerbose,
        				(CODECNAME ": CodecVerifyFormat : StartLine Too Small=%d vs. %d\n", 
        				pKSVBIDataFormat->VBIInfoHeader.StartLine, MIN_VBI_Y_SAMPLES ));
        		}
            }
            // Fix this to either test for other formats OR just rubber stamp/ignore the SPECIFIER.
            else if ( 1 /*Some other NON-VBI input/output format*/ )  // Fix Me
            {
                CDebugPrint(DebugLevelTrace,(CODECNAME , "Other general format requested\n" ));
                // Verify the format detail(ie xxINFOHEADER fields) and set rval=TRUE iff OK
                if ( 1 )
                {
                    rval = TRUE;
                }
            }
            else
            {
                CDebugPrint(DebugLevelTrace,(CODECNAME , "Unrecognized format requested\n" ));
            }

        }
        else
        {
	        CDebugPrint(DebugLevelTrace, ( CODECNAME, "General Format Mismatch\n" ));
        }
    }
    CDebugPrint(DebugLevelTrace,(CODECNAME ":<---CodecVerifyFormat(%x)=%s\n", pKSDataFormat, rval? "TRUE":"FALSE"));
	return rval;
}

/*
** CodecFormatFromRange()
**
**   Returns a DATAFORMAT from a DATARANGE
**
** Arguments:
**
**         IN PHW_STREAM_REQUEST_BLOCK pSrb 
**
** Returns:
** 
**   TRUE if the format is supported
**   FALSE if the format cannot be suppored
**
** Side Effects:  none
*/

BOOL 
CodecFormatFromRange( IN PHW_STREAM_REQUEST_BLOCK pSrb )
{
	BOOL						bStatus = FALSE;
    PSTREAM_DATA_INTERSECT_INFO IntersectInfo;
    PKSDATARANGE                DataRange;
    BOOL                        OnlyWantsSize;
    ULONG                       FormatSize = 0;
    ULONG                       StreamNumber;
    ULONG                       j;
    ULONG                       NumberOfFormatArrayEntries;
    PKSDATAFORMAT               *pAvailableFormats;

    CDebugPrint(DebugLevelTrace,(CODECNAME ":--->CodecFormatFromRange(pSrb=%x)\n", pSrb));

    IntersectInfo = pSrb->CommandData.IntersectInfo;
    StreamNumber = IntersectInfo->StreamNumber;
    DataRange = IntersectInfo->DataRange;

    pSrb->ActualBytesTransferred = 0;

    //
    // Check that the stream number is valid
    //

    if (StreamNumber < DRIVER_STREAM_COUNT) 
	{
		NumberOfFormatArrayEntries = 
				Streams[StreamNumber].hwStreamInfo.NumberOfFormatArrayEntries;

		//
		// Get the pointer to the array of available formats
		//

		pAvailableFormats = Streams[StreamNumber].hwStreamInfo.StreamFormatsArray;

		//
		// Is the caller trying to get the format, or the size of the format?
		//

		OnlyWantsSize = (IntersectInfo->SizeOfDataFormatBuffer == sizeof(ULONG));

		//
		// Walk the formats supported by the stream searching for a match
		// of the three GUIDs which together define a DATARANGE
		//
		for (j = 0; j < NumberOfFormatArrayEntries; j++, pAvailableFormats++) 
		{
			if ( CodecCompareGUIDsAndFormatSize(DataRange, *pAvailableFormats, TRUE) )
			{
#ifdef KS_DATARANGE_VIDEO_VBI__EQ__KS_DATAFORMAT_VBIINFOHEADER
				ULONG	FormatSize = (*pAvailableFormats)->FormatSize;
#else
                PKS_DATARANGE_VIDEO_VBI pDataRangeVBI = (PKS_DATARANGE_VIDEO_VBI)*pAvailableFormats;
				ULONG	FormatSize = sizeof( KS_DATAFORMAT_VBIINFOHEADER );
#endif

				// Is the caller trying to get the format, or the size of the format?

				if ( IntersectInfo->SizeOfDataFormatBuffer == sizeof(FormatSize) )
				{					
       				CDebugPrint(DebugLevelVerbose,
						(CODECNAME ": CodecFormatFromRange : Format Size=%d\n", 
						FormatSize));
					*(PULONG)IntersectInfo->DataFormatBuffer = FormatSize;
					pSrb->ActualBytesTransferred = sizeof(FormatSize);
					bStatus = TRUE;
				}
				else
				{
					// Verify that there is enough room in the supplied buffer for the whole thing
					if ( IntersectInfo->SizeOfDataFormatBuffer >= FormatSize ) 
					{
#ifdef KS_DATARANGE_VIDEO_VBI__EQ__KS_DATAFORMAT_VBIINFOHEADER
						RtlCopyMemory(IntersectInfo->DataFormatBuffer, *pAvailableFormats, FormatSize);
						pSrb->ActualBytesTransferred = FormatSize;
#else
                        PKS_DATAFORMAT_VBIINFOHEADER InterVBIHdr =
							(PKS_DATAFORMAT_VBIINFOHEADER)IntersectInfo->DataFormatBuffer;

						RtlCopyMemory(&InterVBIHdr->DataFormat, &pDataRangeVBI->DataRange, sizeof(KSDATARANGE));

						((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;

						RtlCopyMemory(&InterVBIHdr->VBIInfoHeader, &pDataRangeVBI->VBIInfoHeader, sizeof(KS_VBIINFOHEADER));
						pSrb->ActualBytesTransferred = FormatSize;
#endif
						bStatus = TRUE;
					}
					else
					{
       					CDebugPrint(DebugLevelVerbose,
							(CODECNAME ": CodecFormatFromRange : Buffer Too Small=%d vs. %d\n", 
							IntersectInfo->SizeOfDataFormatBuffer, FormatSize));
						pSrb->Status = STATUS_BUFFER_TOO_SMALL;
					}
				}
				break;
			}
		}

		if ( j == NumberOfFormatArrayEntries )
		{
			CDebugPrint(DebugLevelVerbose,(CODECNAME ": CodecFormatFromRange : Stream Format not found.\n" ));
		}

	}
	else
	{
		CDebugPrint(DebugLevelVerbose,(CODECNAME ": CodecFormatFromRange : StreamNumber too big=%d\n", StreamNumber));
        CDEBUG_BREAK();
        pSrb->Status = STATUS_NOT_IMPLEMENTED;
        bStatus = FALSE;
    }

    CDebugPrint(DebugLevelTrace,
		(CODECNAME ":<---CodecFormatFromRange(pSrb=%x)=%s\n", 
		pSrb, bStatus ? "TRUE" : "FALSE" ));
    return bStatus;
}

