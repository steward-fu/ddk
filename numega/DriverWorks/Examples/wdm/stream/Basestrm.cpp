// basestrm.cpp - base class for streams of Video Capture sample
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#include <vdw.h>
#include <kstream.h>

#include "basestrm.h"

#ifdef __VIDCAP_TRACEON__
	KTrace TVS("VidCap Classes");
#endif


DEFINE_KSPROPERTY_TABLE(VidcapBaseStream::m_VideoStreamConnectionProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CONNECTION_ALLOCATORFRAMING,
        TRUE,										// GetSupported or Handler
        sizeof(KSPROPERTY),							// MinProperty
        sizeof(KSALLOCATOR_FRAMING),				// MinData
        FALSE,										// SetSupported or Handler
        NULL,										// Values
        0,											// RelationsCount
        NULL,										// Relations
        NULL,										// SupportHandler
        sizeof(ULONG)								// SerializedSize
    ),
};

DEFINE_KSPROPERTY_TABLE(VidcapBaseStream::m_VideoStreamDroppedFramesProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_DROPPEDFRAMES_CURRENT,
        TRUE,										// GetSupported or Handler
        sizeof(KSPROPERTY_DROPPEDFRAMES_CURRENT_S),	// MinProperty
        sizeof(KSPROPERTY_DROPPEDFRAMES_CURRENT_S),	// MinData
        FALSE,										// SetSupported or Handler
        NULL,										// Values
        0,											// RelationsCount
        NULL,										// Relations
        NULL,										// SupportHandler
        0											// SerializedSize
    ),
};


// ------------------------------------------------------------------------
// Array of all of the property sets supported by video streams
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_SET_TABLE(VidcapBaseStream::m_VideoStreamProperties)
{
    DEFINE_KSPROPERTY_SET
    ( 
        &KSPROPSETID_Connection,							// Set
        SIZEOF_ARRAY(m_VideoStreamConnectionProperties),	// PropertiesCount
        m_VideoStreamConnectionProperties,					// PropertyItem
        0,													// FastIoCount
        NULL												// FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_DROPPEDFRAMES,					// Set
        SIZEOF_ARRAY(m_VideoStreamDroppedFramesProperties), // PropertiesCount
        m_VideoStreamDroppedFramesProperties,               // PropertyItem
        0,													// FastIoCount
        NULL												// FastIoTable
    ),
};


const ULONG	VidcapBaseStream::m_VideoStreamPropertiesCount = SIZEOF_ARRAY(m_VideoStreamProperties);

VidcapBaseStream::VidcapBaseStream()
	: KStream(),
	  m_RequestQueueCount(0),
	  m_pVideoInfoHeader(NULL),
	  m_fDiscontinuity(0),
	  m_QST_Now(0),
	  m_QST_NextFrame(0),
	  m_QST_StreamTime(0)

{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::VidcapBaseStream -- constructing\n";
	#endif

	RtlZeroMemory(&m_FrameInfo, sizeof(KS_FRAME_INFO));   // PictureNumber, etc.
    for (int i = 0; i < (720*3); i++)
		m_LineBuffer[i] =  0;	 // working buffer (RGB24)
}

// -- our version of InitializeBaseClass called from derived stream classes --
void VidcapBaseStream::InitializeBaseClass(const HW_STREAM_INFORMATION& info, ULONG uStreamHeaderMediaSpecific)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::InitializeBaseClass -- calling KStream::InitializeBaseClass\n";
	#endif

	KStream::InitializeBaseClass(
		info,
		0,							 // uClockSupportFlags 
		FALSE,						 // Not a master clock
		FALSE,						 // bDma
		TRUE,						 // bPio,
	    uStreamHeaderMediaSpecific,	 // StreamHeaderMediaSpecific
	    0 );						 // StreamHeaderWorkspace 

}

void VidcapBaseStream::QueueSrb(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::QueueSrb -- Start\n";
	#endif

	PIRP pIrp(pSrb->Irp);

	// -- make the SRB part of the Irp's Driver Context and 
	//		then QUEUE the IRP on our IRP Queue --
	Lock();

	pIrp->Tail.Overlay.DriverContext[0] = pSrb;
	m_RequestQueue.InsertTail(pIrp);
	m_RequestQueueCount++;	

	Unlock();

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::QueueSrb -- End\n";
	#endif

}

PHW_STREAM_REQUEST_BLOCK VidcapBaseStream::DequeueSrb(void)
{
	// -- excessive tracing use for DEBUG only! --
	//	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VidcapBaseStream::DequeueSrb -- Start\n";
	//	#endif

	PHW_STREAM_REQUEST_BLOCK pSrb = NULL;

	// -- since the SRB actually contains the IRP, and our 
	//		driver context of the IRP now points to the SRB,
	//		we can dequeue the IRP, pull of the SRB and return
	//		only that object... the IRP is already pointed to
	//		BY the SRB itself! --

	Lock();

	if (!m_RequestQueue.IsEmpty())
	{
		PIRP pIrp = m_RequestQueue.RemoveHead();
		pSrb = (PHW_STREAM_REQUEST_BLOCK)pIrp->Tail.Overlay.DriverContext[0];
		m_RequestQueueCount--;	
	}
	else
		pSrb = NULL;

	Unlock();

	// -- excessive tracing use for DEBUG only! --
	//	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VidcapBaseStream::DequeueSrb -- End\n";
	//	#endif

	return pSrb;
}

void VidcapBaseStream::CancelAllRequests(void)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::CancelAllRequests -- Start\n";
	#endif

	PHW_STREAM_REQUEST_BLOCK pSrb;

	Lock();

	while ( !m_RequestQueue.IsEmpty() )
	{
		PIRP pIrp = m_RequestQueue.RemoveHead();
		pSrb = (PHW_STREAM_REQUEST_BLOCK)pIrp->Tail.Overlay.DriverContext[0];
		m_RequestQueueCount--;	
		pSrb->CommandData.DataBufferArray->DataUsed = 0;
		pSrb->Status = STATUS_CANCELLED;
		RequestComplete(pSrb);
	}
			
	Unlock();

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::CancelAllRequests -- End\n";
	#endif

}

void VidcapBaseStream::CancelRequest(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::CancelAllRequests -- Start\n";
	#endif

	PIRP pIrp;
	BOOLEAN Removed=FALSE;

	Lock();

	for (pIrp=m_RequestQueue.Head(); pIrp!=NULL; pIrp=m_RequestQueue.Next(pIrp))
	{
		if (pIrp->Tail.Overlay.DriverContext[0] == pSrb)
		{
			m_RequestQueue.Remove(pIrp);
			m_RequestQueueCount--;	
			Removed = TRUE;
			break;
		}
	}

	Unlock();

	if (Removed)
	{
		pSrb->CommandData.DataBufferArray->DataUsed = 0;
		RequestComplete(pSrb);
	}

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::CancelAllRequests -- End\n";
	#endif

}

void VidcapBaseStream::DispatchControl(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::DispatchControl -- Start\n";
	#endif

	if ( !m_ControlSrbList.AddIfBusy(pSrb) )
		do 
		{
			pSrb->Status = STATUS_SUCCESS; // default is success
			KStream::DispatchControl(pSrb);

		} while ( m_ControlSrbList.RemoveIfAvailable(pSrb) );

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::DispatchControl -- End\n";
	#endif

}

void VidcapBaseStream::OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::OnGetProperty -- Start\n";
	#endif

	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

	if (IsEqualGUID (KSPROPSETID_Connection, pSPD->Property->Set))
		GetConnectionProperty(pSrb);

	else if (IsEqualGUID (PROPSETID_VIDCAP_DROPPEDFRAMES, pSPD->Property->Set))
		GetDroppedFramesProperty(pSrb);

	else 
	   pSrb->Status = STATUS_NOT_IMPLEMENTED;

	NextSrb(pSrb, pSrb->Status);

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::OnGetProperty -- End\n";
	#endif

}


void VidcapBaseStream::OnGetState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::OnGetState -- Start\n";
	#endif

	pSrb->CommandData.StreamState = m_eState;
	pSrb->ActualBytesTransferred = sizeof (KSSTATE);

	// A very odd rule:
	// When transitioning from stop to pause, DShow tries to preroll
	// the graph.  Capture sources can't preroll, and indicate this
	// by returning VFW_S_CANT_CUE in user mode.  To indicate this
	// condition from drivers, they must return STATUS_NO_DATA_DETECTED

	if (m_eState == KSSTATE_PAUSE)
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VidcapBaseStream::OnGetState -- Paused, returning STATUS_NO_DATA_DETECTED\n";
		#endif

		NextSrb(pSrb,STATUS_NO_DATA_DETECTED);
	}
	else
	   NextSrb(pSrb,STATUS_SUCCESS);

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::OnGetState -- End\n";
	#endif

}

void VidcapBaseStream::OnSetState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::OnSetState -- Start\n";
	#endif

	KSSTATE    PreviousState;

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
    //          and whether you're using the Stream class for synchronization
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

    //
    // Remember the state we're transitioning away from
    //

    PreviousState = m_eState;

    //
    // Set the new state
    //

    m_eState = pSrb->CommandData.StreamState;

    switch (m_eState)  

    {
    case KSSTATE_STOP:

        //
        // The stream class will cancel all outstanding IRPs for us
        // (but only if it is maintaining the queue ie. using Stream Class synchronization)
        // Since Testcap is not using Stream Class synchronization, we must clear the queue here
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceAlways << "VidcapBaseStream::OnSetState -- New State = KSSTATE_STOP\n";
		#endif

        CancelAllRequests();

        break;

    case KSSTATE_ACQUIRE:

        //
        // This is a KS only state, that has no correspondence in DirectShow
        // 
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceAlways << "VidcapBaseStream::OnSetState -- New State = KSSTATE_ACQUIRE\n";
		#endif
        break;

    case KSSTATE_PAUSE:

        //
        // On a transition to pause from acquire or stop, start our timer running.
        //
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceAlways << "VidcapBaseStream::OnSetState -- New State = KSSTATE_PAUSE\n";
		#endif

        if (PreviousState == KSSTATE_ACQUIRE || PreviousState == KSSTATE_STOP)
		{  

            // Zero the frame counters
            m_FrameInfo.PictureNumber = 0;
            m_FrameInfo.DropCount = 0;
            m_FrameInfo.dwFrameFlags = 0;

            // Setup the next timer callback
            // Make it run at 2x the requested capture rate (which is in 100nS units)

            StreamClassScheduleTimer (
                    pSrb->StreamObject,									// StreamObject
                    Adapter(),											// HwDeviceExtension
                    (ULONG) (m_pVideoInfoHeader->AvgTimePerFrame / 20), // Microseconds
                    VideoTimerRoutine,									// TimerRoutine
                    this);												// Context (also this stream)
        }
        break;

    case KSSTATE_RUN:

        // 
        // Begin Streaming.
        //
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceAlways << "VidcapBaseStream::OnSetState -- New State = KSSTATE_RUN\n";
		#endif

        // Reset the discontinuity flag

        m_fDiscontinuity = FALSE;

        // Setting the NextFrame time to zero will cause the value to be
        // reset from the stream time 

        m_QST_NextFrame = 0;

        break;

    } // end switch (pSrb->CommandData.StreamState)  
	
	NextSrb(pSrb);

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::OnSetState -- End\n";
	#endif

}

// -- used in this sample for generating video frames --
void STREAMAPI VidcapBaseStream::VideoTimerRoutine(PVOID pContext)
{
	//	-- this trace tends to really make the monitor go crazy! --
	//	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VidcapBaseStream::VideoTimerRoutine\n";
	//	#endif

	((VidcapBaseStream*)pContext)->OnVideoTimer();
}

// -- can be overridden in derived classes to generate video for that class --
void VidcapBaseStream::OnVideoTimer()
{
    // If we're stopped and the timer is still running, just return.
    // This will stop the timer.
	//  -- excessive tracing... use during DEBUG only! --
	//	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VidcapBaseStream::OnVideoTimer -- Start\n";
	//	#endif

    if (KSSTATE_STOP == m_eState)
	{  
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceAlways << "VidcapBaseStream::OnVideoTimer -- State = KSSTATE_STOP... returning\n";
		#endif

		return;
    }

    // Capture a frame if it's time and we have a buffer

    CaptureFrame();

    // Schedule the next timer event
    // Make it run at 2x the requested capture rate (which is in 100nS units)

    StreamClassScheduleTimer (
            m_pClassObject,										// StreamObject
            Adapter(),											// HwDeviceExtension
            (ULONG) (m_pVideoInfoHeader->AvgTimePerFrame / 20), // Microseconds
            VideoTimerRoutine,									// TimerRoutine
            this);												// Context

	//  -- excessive tracing... use during DEBUG only! --
	//	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VidcapBaseStream::OnVideoTimer -- End\n";
	//	#endif

}


NTSTATUS VidcapBaseStream::OnOpen(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::OnOpen -- Start\n";
	#endif

    PKSDATAFORMAT           pKSDataFormat = pSrb->CommandData.OpenFormat;
    int                     StreamNumber = pSrb->StreamObject->StreamNumber;

	pSrb->Status = VerifyFormat(pSrb->CommandData.OpenFormat) ?
		STATUS_SUCCESS : STATUS_INVALID_PARAMETER;

	if (IsEqualGUID (pKSDataFormat->Specifier, KSDATAFORMAT_SPECIFIER_VIDEOINFO))
	{   
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceAlways << "VidcapBaseStream::OnOpen -- Format == KSDATAFORMAT_SPECIFIER_VIDEOINFO\n";
		#endif
		
		PKS_DATAFORMAT_VIDEOINFOHEADER  pVideoInfoHeader = 
					(PKS_DATAFORMAT_VIDEOINFOHEADER) pSrb->CommandData.OpenFormat;
		PKS_VIDEOINFOHEADER pVideoInfoHdrRequested = &pVideoInfoHeader->VideoInfoHeader;
	
		ULONG nSize = KS_SIZE_VIDEOHEADER (pVideoInfoHdrRequested);

		m_pVideoInfoHeader = (PKS_VIDEOINFOHEADER) new(NonPagedPool) UCHAR[nSize];

		if (m_pVideoInfoHeader == NULL)
			pSrb->Status = STATUS_INSUFFICIENT_RESOURCES;

		// Copy the VIDEOINFOHEADER requested to our storage
		RtlCopyMemory(m_pVideoInfoHeader, pVideoInfoHdrRequested, nSize);
	}

	// -------------------------------------------------------------------
	// Specifier FORMAT_AnalogVideo for KS_ANALOGVIDEOINFO
	// -------------------------------------------------------------------
	else if (IsEqualGUID (pKSDataFormat->Specifier, KSDATAFORMAT_SPECIFIER_ANALOGVIDEO))
	{
	  
		//
		// AnalogVideo DataRange == DataFormat!
		//

		// For now, don't even cache this
		// BUGBUG
		//
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceAlways << "VidcapBaseStream::OnOpen -- Format == KSDATAFORMAT_SPECIFIER_ANALOGVIDEO\n";
		#endif

		PKS_DATARANGE_ANALOGVIDEO pDataFormatAnalogVideo = 
					(PKS_DATARANGE_ANALOGVIDEO) pSrb->CommandData.OpenFormat;
	}

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::OnOpen -- End\n";
	#endif

	return pSrb->Status;
}

BOOL VidcapBaseStream::VerifyFormat(PKSDATAFORMAT pKSDataFormatToVerify)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::VerifyFormat -- Start\n";
	#endif

	BOOL                        fOK = FALSE;
	ULONG                       i;
	PKSDATAFORMAT*				pAvailableFormats = StreamFormatsArray();

	KdPrint(("TestCap: FormatSize=%d\n",  pKSDataFormatToVerify->FormatSize));
	KdPrint(("TestCap: MajorFormat=%x\n", pKSDataFormatToVerify->MajorFormat));

	//
	// Walk the formats supported by the stream
	//

	for (i = 0; i < NumberOfStreamFormats(); i++, pAvailableFormats++)
	{
		// Check for a match on the three GUIDs and format size

		if (!CompareGUIDsAndFormatSize(
				pKSDataFormatToVerify, 
				*pAvailableFormats,
				FALSE /* CompareFormatSize */ ))
			continue;

		//
		// Now that the three GUIDs match, switch on the Specifier
		// to do a further type-specific check
		//

		// -------------------------------------------------------------------
		// Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
		// -------------------------------------------------------------------

		if (IsEqualGUID (pKSDataFormatToVerify->Specifier, 
				KSDATAFORMAT_SPECIFIER_VIDEOINFO))
		{				
			PKS_DATAFORMAT_VIDEOINFOHEADER  pDataFormatVideoInfoHeader = 
					(PKS_DATAFORMAT_VIDEOINFOHEADER) pKSDataFormatToVerify;
			PKS_VIDEOINFOHEADER  pVideoInfoHdrToVerify = 
					 (PKS_VIDEOINFOHEADER) &pDataFormatVideoInfoHeader->VideoInfoHeader;
			PKS_DATARANGE_VIDEO             pKSDataRangeVideo = (PKS_DATARANGE_VIDEO) *pAvailableFormats;
			KS_VIDEO_STREAM_CONFIG_CAPS    *pConfigCaps = &pKSDataRangeVideo->ConfigCaps;
			RECT                            rcImage;
	

			/* 
			**  HOW BIG IS THE IMAGE REQUESTED (pseudocode follows)
			**
			**  if (IsRectEmpty (&rcTarget) {
			**      SetRect (&rcImage, 0, 0, 
			**              BITMAPINFOHEADER.biWidth,
							BITMAPINFOHEADER.biHeight);
			**  }
			**  else {
			**      // Probably rendering to a DirectDraw surface,
			**      // where biWidth is used to expressed the "stride" 
			**      // in units of pixels (not bytes) of the destination surface.
			**      // Therefore, use rcTarget to get the actual image size 
			**      
			**      rcImage = rcTarget;
			**  }
			*/
		
			if ((pVideoInfoHdrToVerify->rcTarget.right - 
				 pVideoInfoHdrToVerify->rcTarget.left <= 0) ||
				(pVideoInfoHdrToVerify->rcTarget.bottom - 
				 pVideoInfoHdrToVerify->rcTarget.top <= 0)) {
			
				 rcImage.left = rcImage.top = 0;
				 rcImage.right = pVideoInfoHdrToVerify->bmiHeader.biWidth;
				 rcImage.bottom = pVideoInfoHdrToVerify->bmiHeader.biHeight;
			}
			else {
				 rcImage = pVideoInfoHdrToVerify->rcTarget;
			}

			//
			// BUGBUG, perform all other verification tests here!!!
			//

			//
			// HOORAY, the format passed all of the tests, so we support it
			//

			fOK = TRUE;
			break;

		} // End of VIDEOINFOHEADER specifier

		// -------------------------------------------------------------------
		// Specifier FORMAT_AnalogVideo for KS_ANALOGVIDEOINFO
		// -------------------------------------------------------------------

		else if (IsEqualGUID (pKSDataFormatToVerify->Specifier, 
				KSDATAFORMAT_SPECIFIER_ANALOGVIDEO)) {
	  
			//
			// For analog video, the DataRange and DataFormat
			// are identical, so just copy the whole structure
			//

			PKS_DATARANGE_ANALOGVIDEO DataRangeVideo = 
					(PKS_DATARANGE_ANALOGVIDEO) *pAvailableFormats;

			//
			// BUGBUG, perform all other verification tests here!!!
			//

			fOK = TRUE;
			break;

		} // End of KS_ANALOGVIDEOINFO specifier

	} // End of loop on all formats for this stream

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::VerifyFormat -- End\n";
	#endif

	return fOK;
}


BOOL VidcapBaseStream::CompareGUIDsAndFormatSize(
	IN PKSDATARANGE DataRange1,
	IN PKSDATARANGE DataRange2,
	BOOL fCompareFormatSize
	)
{

	return (
		IsEqualGUID (
			DataRange1->MajorFormat, 
			DataRange2->MajorFormat) &&
		IsEqualGUID (
			DataRange1->SubFormat, 
			DataRange2->SubFormat) &&
		IsEqualGUID (
			DataRange1->Specifier, 
			DataRange2->Specifier) && 
		(fCompareFormatSize ? 
				(DataRange1->FormatSize == DataRange2->FormatSize) : TRUE ));
}



NTSTATUS VidcapBaseStream::GetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::GetDataIntersection -- Start\n";
	#endif

	PSTREAM_DATA_INTERSECT_INFO IntersectInfo;
	PKSDATARANGE                DataRange;
	BOOL                        OnlyWantsSize;
	BOOL                        MatchFound = FALSE;
	ULONG                       FormatSize;
	PKSDATAFORMAT*              pAvailableFormats;

	IntersectInfo = pSrb->CommandData.IntersectInfo;
	DataRange = IntersectInfo->DataRange;

	//
	// Get the pointer to the array of available formats
	//
	pAvailableFormats = StreamFormatsArray();

	//
	// Is the caller trying to get the format, or the size of the format?
	//

	OnlyWantsSize = (IntersectInfo->SizeOfDataFormatBuffer == sizeof(ULONG));

	//
	// Walk the formats supported by the stream searching for a match
	// of the three GUIDs which together define a DATARANGE
	//

	for (ULONG i = 0; i < NumberOfStreamFormats(); i++, pAvailableFormats++) {

		if (!CompareGUIDsAndFormatSize(
				DataRange, 
				*pAvailableFormats,
				TRUE /* CompareFormatSize */))
		{
			continue;
		}

		//
		// Now that the three GUIDs match, switch on the Specifier
		// to do a further type-specific check
		//

		// -------------------------------------------------------------------
		// Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
		// -------------------------------------------------------------------

		if (IsEqualGUID (DataRange->Specifier, KSDATAFORMAT_SPECIFIER_VIDEOINFO))
		{
			PKS_DATARANGE_VIDEO DataRangeVideoToVerify = 
					(PKS_DATARANGE_VIDEO) DataRange;
			PKS_DATARANGE_VIDEO DataRangeVideo = 
					(PKS_DATARANGE_VIDEO) *pAvailableFormats;
			PKS_DATAFORMAT_VIDEOINFOHEADER DataFormatVideoInfoHeaderOut;

			//
			// Check that the other fields match
			//
			if ((DataRangeVideoToVerify->bFixedSizeSamples != DataRangeVideo->bFixedSizeSamples) ||
				(DataRangeVideoToVerify->bTemporalCompression != DataRangeVideo->bTemporalCompression) ||
				(DataRangeVideoToVerify->StreamDescriptionFlags != DataRangeVideo->StreamDescriptionFlags) ||
				(DataRangeVideoToVerify->MemoryAllocationFlags != DataRangeVideo->MemoryAllocationFlags) ||
				(RtlCompareMemory (&DataRangeVideoToVerify->ConfigCaps,
						&DataRangeVideo->ConfigCaps,
						sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)) != 
						sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)))
			{
				continue;
			}
			
			// MATCH FOUND!
			MatchFound = TRUE;            
			FormatSize = sizeof (KSDATAFORMAT) + 
				KS_SIZE_VIDEOHEADER (&DataRangeVideoToVerify->VideoInfoHeader);

			if (OnlyWantsSize)
				break;
			
			// Caller wants the full data format
			if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize)
				return(STATUS_BUFFER_TOO_SMALL);

			// Copy over the KSDATAFORMAT, followed by the 
			// actual VideoInfoHeader
				
			DataFormatVideoInfoHeaderOut = (PKS_DATAFORMAT_VIDEOINFOHEADER) IntersectInfo->DataFormatBuffer;

			// Copy over the KSDATAFORMAT 
			RtlCopyMemory(
				&DataFormatVideoInfoHeaderOut->DataFormat,
				&DataRangeVideoToVerify->DataRange,
				sizeof (KSDATARANGE));

			DataFormatVideoInfoHeaderOut->DataFormat.FormatSize = FormatSize;

			// Copy over the callers requested VIDEOINFOHEADER

			RtlCopyMemory(
				&DataFormatVideoInfoHeaderOut->VideoInfoHeader,
				&DataRangeVideoToVerify->VideoInfoHeader,
				KS_SIZE_VIDEOHEADER (&DataRangeVideoToVerify->VideoInfoHeader));

			// Calculate biSizeImage for this request, and put the result in both
			// the biSizeImage field of the bmiHeader AND in the SampleSize field
			// of the DataFormat.
			//
			// Note that for compressed sizes, this calculation will probably not
			// be just width * height * bitdepth

			DataFormatVideoInfoHeaderOut->VideoInfoHeader.bmiHeader.biSizeImage =
				DataFormatVideoInfoHeaderOut->DataFormat.SampleSize = 
				KS_DIBSIZE(DataFormatVideoInfoHeaderOut->VideoInfoHeader.bmiHeader);

			//
			// BUGBUG Perform other validation such as cropping and scaling checks
			// 

			break;

		} // End of VIDEOINFOHEADER specifier

		// -------------------------------------------------------------------
		// Specifier FORMAT_AnalogVideo for KS_ANALOGVIDEOINFO
		// -------------------------------------------------------------------

		else if (IsEqualGUID (DataRange->Specifier, 
				KSDATAFORMAT_SPECIFIER_ANALOGVIDEO))
		{
			//
			// For analog video, the DataRange and DataFormat
			// are identical, so just copy the whole structure
			//

			PKS_DATARANGE_ANALOGVIDEO DataRangeVideo = 
					(PKS_DATARANGE_ANALOGVIDEO) *pAvailableFormats;

			// MATCH FOUND!
			MatchFound = TRUE;            
			FormatSize = sizeof (KS_DATARANGE_ANALOGVIDEO);

			if (OnlyWantsSize)
				break;
			
			// Caller wants the full data format
			if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize)
				return(STATUS_BUFFER_TOO_SMALL);

			RtlCopyMemory(
				IntersectInfo->DataFormatBuffer,
				DataRangeVideo,
				sizeof (KS_DATARANGE_ANALOGVIDEO));

			((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;

			break;

		} // End of KS_ANALOGVIDEOINFO specifier

		else
		{
			#ifdef __VIDCAP_TRACEON__
			TVS << TraceAlways << "VidcapBaseStream::GetDataIntersection -- End, Returning STATUS_NO_MATCH\n";
			#endif
			return(STATUS_NO_MATCH);
		}

	} // End of loop on all formats for this stream
	
	if (OnlyWantsSize)
	{
		*(PULONG) IntersectInfo->DataFormatBuffer = FormatSize;
		pSrb->ActualBytesTransferred = sizeof(ULONG);

		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VidcapBaseStream::GetDataIntersection -- End, Size Only, STATUS_SUCCESS\n";
		#endif

		return(STATUS_SUCCESS);
	}        

	pSrb->ActualBytesTransferred = FormatSize;

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::GetDataIntersection -- End, Returning STATUS_SUCCESS\n";
	#endif

	return(STATUS_SUCCESS);
}

void VidcapBaseStream::GetConnectionProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::GetConnectionProperty -- Start\n";
	#endif


    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id)
	{
    case KSPROPERTY_CONNECTION_ALLOCATORFRAMING:
        if (NULL != m_pVideoInfoHeader)
		{
			#ifdef __VIDCAP_TRACEON__
			TVS << TraceInfo << "VidcapBaseStream::GetConnectionProperty -- Got ALLOCATORFRAMING\n";
			#endif

            PKSALLOCATOR_FRAMING Framing = (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;
            Framing->RequirementsFlags =
                KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY |
                KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
                KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY;
            Framing->PoolType = PagedPool;
            Framing->Frames = 1;
            Framing->FrameSize = m_pVideoInfoHeader->bmiHeader.biSizeImage;
            Framing->FileAlignment = 0; // FILE_LONG_ALIGNMENT???;
            Framing->Reserved = 0;
            pSrb->ActualBytesTransferred = sizeof (KSALLOCATOR_FRAMING);
			pSrb->Status = STATUS_SUCCESS;
			#ifdef __VIDCAP_TRACEON__
			TVS << TraceInfo << "VidcapBaseStream::GetConnectionProperty -- STATUS_SUCCESS\n";
			#endif
        }
        else
		{
			#ifdef __VIDCAP_TRACEON__
			TVS << TraceInfo << "VidcapBaseStream::GetConnectionProperty -- KSPROPERTY_CONNECTION_ALLOCATORFRAMING Invalid Parameter\n";
			#endif
            pSrb->Status = STATUS_INVALID_PARAMETER;
        }
        break;

    default:
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VidcapBaseStream::GetConnectionProperty -- STATUS_NOT_IMPLEMENTED\n";
		#endif
		pSrb->Status = STATUS_NOT_IMPLEMENTED;
        break;
    }

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::GetConnectionProperty -- End\n";
	#endif

}

void VidcapBaseStream::GetDroppedFramesProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::GetDroppedFramesProperty -- Start\n";
	#endif

    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id)
	{
    case KSPROPERTY_DROPPEDFRAMES_CURRENT:
        {
			#ifdef __VIDCAP_TRACEON__
			TVS << TraceInfo << "VidcapBaseStream::GetDroppedFramesProperty -- Got KSPROPERTY_DROPPEDFRAMES_CURRENT\n";
			#endif

			PKSPROPERTY_DROPPEDFRAMES_CURRENT_S pDroppedFrames = 
                (PKSPROPERTY_DROPPEDFRAMES_CURRENT_S) pSPD->PropertyInfo;

            pDroppedFrames->PictureNumber =		m_FrameInfo.PictureNumber;
            pDroppedFrames->DropCount =			m_FrameInfo.DropCount;
            pDroppedFrames->AverageFrameSize =	m_pVideoInfoHeader->bmiHeader.biSizeImage;
                
            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_DROPPEDFRAMES_CURRENT_S);
			pSrb->Status = STATUS_SUCCESS;

 			#ifdef __VIDCAP_TRACEON__
			TVS << TraceInfo << "VidcapBaseStream::GetDroppedFramesProperty -- STATUS_SUCCESS\n";
			#endif
       }
        break;

    default:
 		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VidcapBaseStream::GetDroppedFramesProperty -- STATUS_NOT_IMPLEMENTED\n";
		#endif

		pSrb->Status = STATUS_NOT_IMPLEMENTED;
        break;
    }

 	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::GetDroppedFramesProperty -- End\n";
	#endif
}

/*
** VideoCaptureRoutine()
**
**    Routine to capture video frames based on a timer.
**
**    Note:  Devices capable of using interrupts should always
**           trigger capture on a VSYNC interrupt, and not use a timer.
**
** Arguments:
**
** Returns: nothing
**
** Side Effects:  none
*/

void VidcapBaseStream::CaptureFrame()
{
	// -- excessive trace... use ONLY for DEBUG --
	// 	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VidcapBaseStream::CaptureFrame -- Start\n";
	//	#endif

    PKSSTREAM_HEADER        pDataPacket;
    PKS_FRAME_INFO          pFrameInfo;

    // If we're stopped and the timer is still running, just return.
    // This will stop the timer.

    if (KSSTATE_STOP == m_eState)
	{
	 	#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VidcapBaseStream::CaptureFrame -- KSSTATE_STOP, returning without capture\n";
		#endif

		return;
	}

    
    // Find out what time it is, if we're using a clock

    if (NULL != m_hMasterClockHandle)
	{
        HW_TIME_CONTEXT TimeContext;

        TimeContext.HwDeviceExtension = (_HW_DEVICE_EXTENSION*)Adapter(); 
        TimeContext.HwStreamObject = m_pClassObject;
        TimeContext.Function = TIME_GET_STREAM_TIME;
	    TimeContext.Time = 0;
		TimeContext.SystemTime = 0;

        StreamClassQueryMasterClockSync (
            m_hMasterClockHandle,
            &TimeContext);

        m_QST_StreamTime = TimeContext.Time;
        m_QST_Now = TimeContext.SystemTime;
    
        if (m_QST_NextFrame == 0)
		{  
            m_QST_NextFrame = m_QST_StreamTime + m_pVideoInfoHeader->AvgTimePerFrame;
        }
		
		//		-- excessive TRACE... use for DEBUG ONLY --
		//	 	#ifdef __VIDCAP_TRACEON__
		//       TVS.Trace(TraceInfo, "VidcapBaseStream::CaptureFrame -- Time=%16lx\n", TimeContext.Time);
		//       TVS.Trace(TraceInfo, "VidcapBaseStream::CaptureFrame -- SysTime=%16lx\n", TimeContext.SystemTime);
		//		#endif
    }


    // Only capture in the RUN state

    if (KSSTATE_RUN == m_eState)
	{  

        //
        // Determine if it is time to capture a frame based on 
        // how much time has elapsed since capture started.
        // If there isn't a clock available, then capture immediately.
        //

        if ((NULL == m_hMasterClockHandle) ||
             (m_QST_StreamTime >= m_QST_NextFrame))
		{

            PHW_STREAM_REQUEST_BLOCK pSrb;

            // Increment the picture count (usually this is VSYNC count)

            m_FrameInfo.PictureNumber++;

            //
            // Get the next queue SRB (if any)
            //

            pSrb = DequeueSrb();

            if (NULL != pSrb)
			{

	 			#ifdef __VIDCAP_TRACEON__
					#ifdef __VIDCAP_CAPTURE_DEBUG__
					TVS << TraceAlways << "VidcapBaseStream::CaptureFrame -- KSSTATE_RUN, Capturing Frame\n";
					#endif
				#endif
                pDataPacket = pSrb->CommandData.DataBufferArray;
                pFrameInfo = (PKS_FRAME_INFO) (pDataPacket + 1);

                //
                // Call the routine which synthesizes images
                //
                
				// -- need to get image type from Adapter... jak
                ImageSynth (pSrb, IMAGE_XFER_NTSC_EIA_100AMP_100SAT);

                // Set additional info fields about the data captured such as:
                //   Frames Captured
                //   Frames Dropped
                //   Field Polarity
                
                m_FrameInfo.ExtendedHeaderSize = pFrameInfo->ExtendedHeaderSize;

                *pFrameInfo = m_FrameInfo;

                // Init the flags to zero
                pDataPacket->OptionsFlags = 0;

                // Set the discontinuity flag if frames have been previously
                // dropped, and then reset our internal flag

                if (m_fDiscontinuity) {
                    pDataPacket->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_DATADISCONTINUITY;
                    m_fDiscontinuity = FALSE;
                }

                //
                // Return the timestamp for the frame
                //
                pDataPacket->PresentationTime.Numerator = 1;
                pDataPacket->PresentationTime.Denominator = 1;
                pDataPacket->Duration = m_pVideoInfoHeader->AvgTimePerFrame;

                //
                // if we have a master clock AND this is the capture stream
                // 
                if (m_hMasterClockHandle && (Adapter()->IsStream(0,this)))
				{

                    pDataPacket->PresentationTime.Time = m_QST_StreamTime;
                    pDataPacket->OptionsFlags |= 
                        KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
                        KSSTREAM_HEADER_OPTIONSF_DURATIONVALID;
                }
                else
				{
                    //
                    // no clock or the preview stream, so just mark the time as unknown
                    //
                    pDataPacket->PresentationTime.Time = 0;
                    // clear the timestamp valid flags
                    pDataPacket->OptionsFlags &= 
                        ~(KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
                          KSSTREAM_HEADER_OPTIONSF_DURATIONVALID);
                }

                // Every frame we generate is a key frame (aka SplicePoint)
                // Delta frames (B or P) should not set this flag

                pDataPacket->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_SPLICEPOINT;

                NextSrb(pSrb);
                
            } // if we have an SRB

            else
			{

                //
                // No buffer was available when we should have captured one

                // Increment the counter which keeps track of
                // dropped frames

	 			#ifdef __VIDCAP_TRACEON__
				TVS << TraceAlways << "VidcapBaseStream::CaptureFrame -- KSSTATE_RUN, No SRB, Frame Dropped\n";
				#endif

                m_FrameInfo.DropCount++;

                // Set the (local) discontinuity flag
                // This will cause the next packet processed to have the
                //   KSSTREAM_HEADER_OPTIONSF_DATADISCONTINUITY flag set.

                m_fDiscontinuity = TRUE;

            }

            // Figure out when to capture the next frame
            m_QST_NextFrame += m_pVideoInfoHeader->AvgTimePerFrame;

        } // endif time to capture a frame
    } // endif we're running

	//  -- excessive trace use for DEBUG only --
	// 	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VidcapBaseStream::CaptureFrame -- End\n";
	//	#endif
}

/*
** ImageSynth()
**
**   Synthesizes NTSC color bars, white, black, and grayscale images
**
** Arguments:
**
**   pSrb - The stream request block for the Video stream
**   ImageXferCommands - Index specifying the image to generate
**
** Returns:
**
**   Nothing
**
** Side Effects:  none
*/

//
// EIA-189-A Standard color bar definitions
//

// 75% Amplitude, 100% Saturation
const static UCHAR NTSCColorBars75Amp100SatRGB24 [3][8] = 
{
//  Whi Yel Cya Grn Mag Red Blu Blk
    191,191,  0,  0,191,191,  0,  0,    // Red
    191,191,191,191,  0,  0,  0,  0,    // Green
    191,  0,191,  0,191,  0,191,  0     // Blue
};

// 100% Amplitude, 100% Saturation
const static UCHAR NTSCColorBars100Amp100SatRGB24 [3][8] = 
{
//  Whi Yel Cya Grn Mag Red Blu Blk
    255,255,  0,  0,255,255,  0,  0,    // Red
    255,255,255,255,  0,  0,  0,  0,    // Green
    255,  0,255,  0,255,  0,255,  0     // Blue
};




void VidcapBaseStream::ImageSynth (
    PHW_STREAM_REQUEST_BLOCK pSrb,
    ImageXferCommands Command
    )
{
    UINT biWidth        =   m_pVideoInfoHeader->bmiHeader.biWidth;
    UINT biHeight       =   m_pVideoInfoHeader->bmiHeader.biHeight;
    UINT biSizeImage    =   m_pVideoInfoHeader->bmiHeader.biSizeImage;
    UINT biWidthBytes   =   KS_DIBWIDTHBYTES (m_pVideoInfoHeader->bmiHeader);
    UINT biBitCount     =   m_pVideoInfoHeader->bmiHeader.biBitCount;

    UINT                    Line;
    PUCHAR                  pLineBuffer;

    PKSSTREAM_HEADER        pDataPacket = pSrb->CommandData.DataBufferArray;
    PUCHAR                  pImage =  (PUCHAR)pDataPacket->Data;

    DEBUG_ASSERT (pSrb->NumberOfBuffers == 1);

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::ImageSynth -- Start\n";
		#ifdef __VIDCAP_CAPTURE_DEBUG__
		TVS.Trace(TraceInfo, "biSizeImage=%d, DataUsed=%d\n", 
            biSizeImage, pDataPacket->DataUsed);
	    TVS.Trace(TraceInfo, "biWidth=%d biHeight=%d WidthBytes=%d bpp=%d\n", 
            biWidth, biHeight, biWidthBytes, biBitCount);
		TVS.Trace(TraceInfo, "pImage=%p\n", pImage);
		#endif
	#endif

    // 
    // Synthesize a single line of image data, which will then be replicated
    //

    pLineBuffer = &m_LineBuffer[0];

    switch (biBitCount) {

    case 24: 
    {

        switch (Command) {

        case IMAGE_XFER_NTSC_EIA_100AMP_100SAT:
            // 100% saturation
            {
                UINT x, col;
                PUCHAR pT = pLineBuffer;
        
                for (x = 0; x < biWidth; x++) {
                    col = (x * 8) / biWidth;
                    *pT++ = NTSCColorBars100Amp100SatRGB24[0][col]; // Red
                    *pT++ = NTSCColorBars100Amp100SatRGB24[1][col]; // Green
                    *pT++ = NTSCColorBars100Amp100SatRGB24[2][col]; // Blue
                }
            }
            break;

        case IMAGE_XFER_NTSC_EIA_75AMP_100SAT:
            // 75% Saturation
            {
                UINT x, col;
                PUCHAR pT = pLineBuffer;
        
                for (x = 0; x < biWidth; x++) {
                    col = (x * 8) / biWidth;
                    *pT++ = NTSCColorBars75Amp100SatRGB24[0][col]; // Red
                    *pT++ = NTSCColorBars75Amp100SatRGB24[1][col]; // Green
                    *pT++ = NTSCColorBars75Amp100SatRGB24[2][col]; // Blue
                }
            }
            break;

        case IMAGE_XFER_BLACK:
            // All black
            RtlFillMemory(
		        pLineBuffer,
		        biWidthBytes,
		        (UCHAR) 0);
            break;

        case IMAGE_XFER_WHITE:
            // All white
            RtlFillMemory(
		        pLineBuffer,
		        biWidthBytes,
		        (UCHAR) 255);
            break;

        case IMAGE_XFER_GRAY_INCREASING:
            // grayscale ramp
            RtlFillMemory(
		        pLineBuffer,
		        biWidthBytes,
		        (UCHAR) (m_FrameInfo.PictureNumber * 8));
            break;

        default:
            break;
        }

    } // switch 24

    break;

    case 8: 
    {

        switch (Command) {

        case IMAGE_XFER_NTSC_EIA_75AMP_100SAT:
            {
                // TODO
            }
            break;

        case IMAGE_XFER_NTSC_EIA_100AMP_100SAT:
            {
                // TODO
            }
            break;

        case IMAGE_XFER_BLACK:
            RtlFillMemory(
		        pLineBuffer,
		        biWidthBytes,
		        (UCHAR) 0);
            break;

        case IMAGE_XFER_WHITE:
            RtlFillMemory(
		        pLineBuffer,
		        biWidthBytes,
		        (UCHAR) 15);
            break;

        case IMAGE_XFER_GRAY_INCREASING:
            RtlFillMemory(
		        pLineBuffer,
		        biWidthBytes,
		        (UCHAR) (m_FrameInfo.PictureNumber % 16));
            break;

        default:
            break;
        }

    } // switch 8

        break;

    default:
        break;
    }

    // 
    // Copy the single line synthesized to all rows of the image
    //

    for (Line = 0; Line < biHeight; Line++, pImage += biWidthBytes) {
        RtlCopyMemory(
                pImage,
		pLineBuffer,
		biWidthBytes);
    }

    //
    // Report back the actual number of bytes copied to the destination buffer
    // (This can be smaller than the allocated buffer for compressed images)
    //

    pDataPacket->DataUsed = biSizeImage;
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapBaseStream::ImageSynth -- End\n";
	#endif
}


///////////////////////////////////////////////////////////////////////
//	VidcapBaseStream::OnProposeDataFormat -- SRB_PROPOSE_DATA_FORMAT
//		Called by DispatchControl. Proposes new data format but doesn't
//		change it.
//
void VidcapBaseStream::OnProposeDataFormat(PHW_STREAM_REQUEST_BLOCK pSrb)
{
   if (!(VerifyFormat (pSrb->CommandData.OpenFormat)))
   {
	   	#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VidcapBaseStream::OnProposeDataFormat -- STATUS_NO_MATCH\n";
		#endif

		NextSrb(pSrb, STATUS_NO_MATCH);
   }
   else
   {
	   	#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VidcapBaseStream::OnProposeDataFormat -- STATUS_SUCCESS\n";
		#endif

		NextSrb(pSrb, STATUS_SUCCESS);
   }

}
