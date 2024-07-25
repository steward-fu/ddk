// VideoStream.cpp: implementation of the VideoStream class.
//
//////////////////////////////////////////////////////////////////////

#include <vdw.h>
#include <kstream.h>
#include "Adapter.h"
#include "StreamFormat.h"


extern KDebugOnlyTrace t;


// ------------------------------------------------------------------------
// Property set for all video capture streams
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_TABLE(VideoStreamConnectionProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CONNECTION_ALLOCATORFRAMING,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KSALLOCATOR_FRAMING),            // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
};

DEFINE_KSPROPERTY_TABLE(VideoStreamDroppedFramesProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_DROPPEDFRAMES_CURRENT,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_DROPPEDFRAMES_CURRENT_S),// MinProperty
        sizeof(KSPROPERTY_DROPPEDFRAMES_CURRENT_S),// MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
};



// ------------------------------------------------------------------------
// Array of all of the property sets supported by video streams
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_SET_TABLE(VideoStreamProperties)
{
    DEFINE_KSPROPERTY_SET
    ( 
        &KSPROPSETID_Connection,                        // Set
        SIZEOF_ARRAY(VideoStreamConnectionProperties),  // PropertiesCount
        VideoStreamConnectionProperties,                // PropertyItem
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_DROPPEDFRAMES,                // Set
        SIZEOF_ARRAY(VideoStreamDroppedFramesProperties),  // PropertiesCount
        VideoStreamDroppedFramesProperties,                // PropertyItem
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),

};


#define NUMBER_VIDEO_STREAM_PROPERTIES (SIZEOF_ARRAY(VideoStreamProperties))


// -----------------------------------------------------------------
// STREAM_Capture
// -----------------------------------------------------------------
const HW_STREAM_INFORMATION VideoStream::m_StreamInfo =
{
    // HW_STREAM_INFORMATION -------------------------------------------
    1,                                      // NumberOfPossibleInstances
    KSPIN_DATAFLOW_OUT,                     // DataFlow
    TRUE,                                   // DataAccessible
    SIZEOF_ARRAY(m_StreamFormats),          // NumberOfFormatArrayEntries
    (PKSDATAFORMAT*)m_StreamFormats,        // StreamFormatsArray
    0,                                      // ClassReserved[0]
    0,                                      // ClassReserved[1]
    0,                                      // ClassReserved[2]
    0,                                      // ClassReserved[3]
	NUMBER_VIDEO_STREAM_PROPERTIES,			// NumStreamPropArrayEntries
	(PKSPROPERTY_SET)VideoStreamProperties,	// StreamPropertiesArray
    0,                                      // NumStreamEventArrayEntries;
    0,                                      // StreamEventsArray;
    (GUID *) &PINNAME_VIDEO_CAPTURE,        // Category
    (GUID *) &PINNAME_VIDEO_CAPTURE,        // Name
    0,                                      // MediumsCount
    NULL,                                   // Mediums
	FALSE									// BridgeStream
};

const PKSDATAFORMAT VideoStream::m_StreamFormats[] = 
{
    (PKSDATAFORMAT) &StreamMode_0,
    (PKSDATAFORMAT) &StreamMode_1,
    (PKSDATAFORMAT) &StreamMode_2,
    (PKSDATAFORMAT) &StreamMode_3,
    (PKSDATAFORMAT) &StreamMode_4,
    (PKSDATAFORMAT) &StreamMode_5
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// VideoStream::VideoStream
//	-- The Adapter object constructs an instance of this object in
//	response to SRB_OPEN_STREAM.  After success constructing the 
//	object the adapter object calls the VideoStream::OnOpen method.
//
VideoStream::VideoStream() :
	m_pVideoInfoHeader(NULL),
		m_StopEvent(SynchronizationEvent),
			m_StartEvent(SynchronizationEvent),
				m_FrameCaptured(0),
					m_Mutex(ULONG(0))
{
	RtlZeroMemory(&m_FrameInfo, sizeof(KS_FRAME_INFO));   // PictureNumber, etc.
}


VideoStream::~VideoStream()
{

}


void VideoStream::InitializeStream()
{
	InitializeBaseClass(
		m_StreamInfo,
		0,				// uClockSupportFlags 
		FALSE,			// Not a master clock
		FALSE,			// bDma
		FALSE,			// bPio,
		sizeof(KS_FRAME_INFO),	// StreamHeaderMediaSpecific
		0				// StreamHeaderWorkspace 
		);

	m_sInfo.DataAccessible = TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//	VideoStream::OnGetDataIntersection -- SRB_GET_DATA_INTERSECTION
//	-- called by the system to query for the best matching data format
//		for a data range.  TODO:
//		1) Verify correctness of data range requested
//		2) Create a format (called "perform a data intersection")
//		3) Calculate certain fields with the format
//
NTSTATUS VideoStream::GetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "VideoStream::GetDataIntersection\n";

	PSTREAM_DATA_INTERSECT_INFO IntersectInfo = pSrb->CommandData.IntersectInfo;
	PKSDATARANGE                DataRange = IntersectInfo->DataRange;
	BOOL                        MatchFound = FALSE;
	ULONG                       FormatSize;

	// Get the pointer to the array of available formats
	PKSDATAFORMAT* pAvailableFormats = StreamFormatsArray();

	// Is the caller trying to get the format, or the size of the format?
	// the system usually queries for the size first, and then queries again
	// with the correct sized buffer
	BOOL OnlyWantsSize = IntersectInfo->SizeOfDataFormatBuffer == sizeof(ULONG);

	// Walk the formats supported by the stream searching for a match
	// of the three GUIDs which together define a DATARANGE
	for (ULONG i = 0; i < NumberOfStreamFormats(); i++, pAvailableFormats++) {

		if (!CompareGUIDsAndFormatSize(
				DataRange, 
				*pAvailableFormats,
				TRUE /* CompareFormatSize */)) {

			continue;
		}

		// Now that the three GUIDs match, switch on the Specifier
		// to do a further type-specific check

		// -------------------------------------------------------------------
		// Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
		// -------------------------------------------------------------------

		if (IsEqualGUID (DataRange->Specifier, KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {

			PKS_DATARANGE_VIDEO DataRangeVideoToVerify = 
					(PKS_DATARANGE_VIDEO) DataRange;
			PKS_DATARANGE_VIDEO DataRangeVideo = 
					(PKS_DATARANGE_VIDEO) *pAvailableFormats;
			PKS_DATAFORMAT_VIDEOINFOHEADER DataFormatVideoInfoHeaderOut;

			// Check that the other fields match
			if ((DataRangeVideoToVerify->bFixedSizeSamples		!= DataRangeVideo->bFixedSizeSamples) ||
				(DataRangeVideoToVerify->bTemporalCompression	!= DataRangeVideo->bTemporalCompression) ||
				(DataRangeVideoToVerify->StreamDescriptionFlags != DataRangeVideo->StreamDescriptionFlags) ||
				(DataRangeVideoToVerify->MemoryAllocationFlags	!= DataRangeVideo->MemoryAllocationFlags) ||
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

			// @@BUGBUG Perform other validation such as cropping and scaling checks

			break;
		} 
		else {

			t << "VideoStream::GetDataIntersection -- End, Returning STATUS_NO_MATCH\n";
			return STATUS_NO_MATCH;
		}
	} 
	
	if (OnlyWantsSize) {
		*(PULONG) IntersectInfo->DataFormatBuffer = FormatSize;
		pSrb->ActualBytesTransferred = sizeof(ULONG);

		t << "VideoStream::GetDataIntersection -- End, Size Only, STATUS_SUCCESS\n";
		return STATUS_SUCCESS;
	}        

	pSrb->ActualBytesTransferred = FormatSize;

	return STATUS_SUCCESS;
}


BOOL VideoStream::CompareGUIDsAndFormatSize(
	PKSDATARANGE DataRange1,
	PKSDATARANGE DataRange2,
	BOOL fCompareFormatSize
	)
{
	return 
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
				(DataRange1->FormatSize == DataRange2->FormatSize) : TRUE );
}


/////////////////////////////////////////////////////////////////////////////
//	VideoStream::OnOpen -- SRB_OPEN_STREAM
//	-- called by the adapter object 
//		for a data range.  TODO:
//		1) Verify data format
//		2) Initialize hardware device as required with requested data format
//		3) Allocate 1394 Isochronous resources based on format
//
NTSTATUS VideoStream::OnOpen(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "VideoStream::OnOpen\n";

	m_pClassObject->HwEventRoutine = NULL;

    PKSDATAFORMAT	pKSDataFormat = pSrb->CommandData.OpenFormat;
    int				StreamNumber  = pSrb->StreamObject->StreamNumber;

	// Verify the data format
	NTSTATUS Status = VerifyFormat(pKSDataFormat) ? STATUS_SUCCESS : STATUS_INVALID_PARAMETER;

		// @@BUGBUG this is the only format, right?   so why check?
	if (STATUS_SUCCESS == Status && IsEqualGUID (pKSDataFormat->Specifier, KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {
		
		PKS_DATAFORMAT_VIDEOINFOHEADER pVIHdr = 
			(PKS_DATAFORMAT_VIDEOINFOHEADER) pSrb->CommandData.OpenFormat;

		PKS_VIDEOINFOHEADER pVIHdrRequested = &pVIHdr->VideoInfoHeader;
		ULONG nSize = KS_SIZE_VIDEOHEADER (pVIHdrRequested);
		m_pVideoInfoHeader = (PKS_VIDEOINFOHEADER) new (NonPagedPool) UCHAR[nSize];

		if (m_pVideoInfoHeader == NULL) {
			Status = STATUS_INSUFFICIENT_RESOURCES;
		}
		else {
			// Copy the VIDEOINFOHEADER requested to our storage
			RtlCopyMemory(m_pVideoInfoHeader, pVIHdrRequested, nSize);

			// Determine requested frame rate
			ULONG FrameRate;
			DWORD dwAvgTimePerFrame = (DWORD) m_pVideoInfoHeader->AvgTimePerFrame;

#define TIME_ROUNDING                        1000   // Give it some rounding error of 100microsec
#define TIME_0750FPS      (1333333+TIME_ROUNDING)   // 1/7.50 * 10,000,000 (unit=100ns)
#define TIME_1500FPS       (666666+TIME_ROUNDING)   // 1/15.0 * 10,000,000 (unit=100ns)  do not round to 666667
#define TIME_3000FPS       (333333+TIME_ROUNDING)   // 1/30.0 * 10,000,000 (unit=100ns)

			if (dwAvgTimePerFrame > TIME_0750FPS) 
				FrameRate = 1;        //  3.75FPS
			else if (dwAvgTimePerFrame >  TIME_1500FPS) 
				FrameRate = 2;        //  7.5FPS
			else if (dwAvgTimePerFrame >  TIME_3000FPS) 
				FrameRate = 3;        // 15 FPS
			else 
				FrameRate = 4;        // 30 FPS

			// Determine requested video mode
			ULONG VideoMode;
			DWORD dwCompression = (DWORD) m_pVideoInfoHeader->bmiHeader.biCompression;

		    switch (dwCompression) {

				case FOURCC_Y444:     // Mode 0
					VideoMode = VMODE0_YUV444;
					break;

				case FOURCC_UYVY:     // Mode 1 or 3
					if (m_pVideoInfoHeader->bmiHeader.biWidth == 640 &&
							(m_pVideoInfoHeader->bmiHeader.biHeight == 480 || 
							 m_pVideoInfoHeader->bmiHeader.biHeight == -480)) {
						VideoMode = VMODE3_YUV422;
						// Max frame rate is 15
						if (FrameRate > 3)
							FrameRate = 3;
					}
					else
						VideoMode = VMODE1_YUV422;
         
					break;

				case FOURCC_Y411:     // Mode 2
					VideoMode = VMODE2_YUV411;
					break;

				case KS_BI_RGB:  // = 0
					VideoMode = VMODE4_RGB24;
					// Max frame rate is 15
					if (FrameRate > 3)
						FrameRate = 3;
					break;

				case FOURCC_Y800:  
					VideoMode = VMODE5_YUV800;
					break;

				default:          
					Status = STATUS_NOT_IMPLEMENTED;
					break;
			}

			if (STATUS_SUCCESS == Status) {
				
				// Update the device with the new Frame Rate and Video Mode
				Status = Device()->SetVideoInfo(FrameRate, VideoMode);

				if (STATUS_SUCCESS == Status) {
					// Allocate Isoch resources from 1394 stack
					Status = Device()->AllocateIsochResources();

					// @@TODO update device properties...
					// SetCurrentDevicePropertyValues();
				}
			}

			if (STATUS_SUCCESS != Status) {
				delete m_pVideoInfoHeader;
				m_pVideoInfoHeader = NULL;
			}
		}
	}
	else {
		ASSERT(!"Invalid format");
	}

	pSrb->Status = Status;

	return Status;
}


BOOL VideoStream::VerifyFormat(PKSDATAFORMAT pKSDataFormatToVerify)
{
	t << "VideoStream::VerifyFormat\n";

	BOOL           fOK = FALSE;
	PKSDATAFORMAT* pAvailableFormats = StreamFormatsArray();

	// Walk the formats supported by the stream
	for (ULONG i = 0; i < NumberOfStreamFormats(); i++, pAvailableFormats++) {

		// Check for a match on the three GUIDs and format size
		if (!CompareGUIDsAndFormatSize(
				pKSDataFormatToVerify, 
				*pAvailableFormats,
				FALSE /* CompareFormatSize */ ))
			continue;

		// Now that the three GUIDs match, switch on the Specifier
		// to do a further type-specific check

		// -------------------------------------------------------------------
		// Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
		// -------------------------------------------------------------------

		// @@BUGBUG this is the only format, right?   so why check?
		if (IsEqualGUID (pKSDataFormatToVerify->Specifier, 
				KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {				

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

			// @@BUGBUG, perform all other verification tests here!!!

			// YAY, the format passed all of the tests, so we support it

			fOK = TRUE;
			break;
		} 
		else {
			ASSERT(!"Invalid format");
		}
	} 

	return fOK;
}


CamLowerDevice* VideoStream::Device()
{ 
	return reinterpret_cast<Stream1394Adapter*>(Adapter())->Device(); 
}


/////////////////////////////////////////////////////////////////////////////
//	VideoStream::OnSetState -- SRB_SET_STREAM_STATE
//	-- called by the system to update the state of the stream 
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
void VideoStream::OnSetState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    // Remember the state we're transitioning away from
	KSSTATE PreviousState = m_eState;

    // Set the new state
    m_eState = pSrb->CommandData.StreamState;

	t.Trace(TraceInfo,"VideoStream::OnSetState, from %s to %s\n", 
		KsState(PreviousState), KsState(m_eState));

	NTSTATUS status = STATUS_SUCCESS;

    switch (m_eState)  
    {
    case KSSTATE_STOP:

        // The stream class will cancel all outstanding IRPs for us
        // (but only if it is maintaining the queue ie. using Stream Class synchronization)
        // If not using Stream Class synchronization, we must clear the queue here

		m_Mutex.Wait();
		m_Mutex.Release();

		status = Device()->StopListen(this);

		if (STATUS_PENDING == status) {
			m_StopEvent.Wait();
			// check status of the "stop" operation.  The camera
			// should not be in "started" state (CAMERA_STATE_STARTED)
			status = (CAMERA_STATE_STARTED == Device()->State())?
							STATUS_UNSUCCESSFUL:STATUS_SUCCESS;

			// cancel any remaining requests
			Device()->CancelAllBuffers();
		}

        break;

    case KSSTATE_ACQUIRE:
        break;

    case KSSTATE_PAUSE:

        if (PreviousState == KSSTATE_ACQUIRE || PreviousState == KSSTATE_STOP)
		{  
            // Zero the frame counters
			m_FrameCaptured = 0;
            m_FrameInfo.PictureNumber = 0;
            m_FrameInfo.DropCount = 0;
            m_FrameInfo.dwFrameFlags = 0;
		}

		// @@TODO set to pause...
        break;

    case KSSTATE_RUN:

        // Begin Streaming.
		status = Device()->StartListen(this);

		if (STATUS_PENDING == status) {
			m_StartEvent.Wait();
			// check status of the "start" operation.  The camera
			// should be in "started" state (CAMERA_STATE_STARTED)
			status = (CAMERA_STATE_STARTED == Device()->State())?
							STATUS_SUCCESS:STATUS_UNSUCCESSFUL;
		}
        break;

    }   
	
	NextSrb(pSrb, status);
}


void VideoStream::OnGetState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "VideoStream::OnGetState";

    // Set the new state
    pSrb->CommandData.StreamState = m_eState;
	NextSrb(pSrb);
}


void VideoStream::OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "VideoStream::OnGetProperty";

	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

	if (IsEqualGUID (KSPROPSETID_Connection, pSPD->Property->Set))
		GetConnectionProperty(pSrb);

	else if (IsEqualGUID (PROPSETID_VIDCAP_DROPPEDFRAMES, pSPD->Property->Set))
		GetDroppedFramesProperty(pSrb);

	else 
	   pSrb->Status = STATUS_NOT_IMPLEMENTED;

	NextSrb(pSrb, pSrb->Status);
}


void VideoStream::GetConnectionProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "VideoStream::GetConnectionProperty\n";

    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id)
	{
    case KSPROPERTY_CONNECTION_ALLOCATORFRAMING:
        if (NULL != m_pVideoInfoHeader) {

			t << "VideoStream::GetConnectionProperty -- Got ALLOCATORFRAMING\n";

            PKSALLOCATOR_FRAMING Framing = (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;
            Framing->RequirementsFlags =
                KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY |
                KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
                KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY;
            Framing->PoolType = PagedPool;
            Framing->Frames = 8;
            Framing->FrameSize = m_pVideoInfoHeader->bmiHeader.biSizeImage;
            Framing->FileAlignment = 4096-1; // PAGE ALIGNMENT
            Framing->Reserved = 0;
            pSrb->ActualBytesTransferred = sizeof (KSALLOCATOR_FRAMING);
			pSrb->Status = STATUS_SUCCESS;

			t.Trace(TraceAlways, "VideoStream::GetConnectionProperty: status=0x%x"
								 ", Alignment %d, Frame %d, FrameSize %d\n",
                    pSrb->Status, Framing->FileAlignment+1, Framing->Frames, Framing->FrameSize);

        }
        else {

			t << "VideoStream::GetConnectionProperty -- KSPROPERTY_CONNECTION_ALLOCATORFRAMING Invalid Parameter\n";
            pSrb->Status = STATUS_INVALID_PARAMETER;
        }
        break;

    default:
		t << "VideoStream::GetConnectionProperty -- STATUS_NOT_IMPLEMENTED\n";
		pSrb->Status = STATUS_NOT_IMPLEMENTED;
        break;
    }
}


void VideoStream::GetDroppedFramesProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "VideoStream::GetDroppedFramesProperty\n";

    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id)
	{
    case KSPROPERTY_DROPPEDFRAMES_CURRENT:
        {
			t << "VideoStream::GetDroppedFramesProperty -- Got KSPROPERTY_DROPPEDFRAMES_CURRENT\n";

			PKSPROPERTY_DROPPEDFRAMES_CURRENT_S pDroppedFrames = 
                (PKSPROPERTY_DROPPEDFRAMES_CURRENT_S) pSPD->PropertyInfo;

            pDroppedFrames->PictureNumber =		m_FrameInfo.PictureNumber;
            pDroppedFrames->DropCount =			m_FrameInfo.DropCount;
            pDroppedFrames->AverageFrameSize =	m_pVideoInfoHeader->bmiHeader.biSizeImage;
                
            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_DROPPEDFRAMES_CURRENT_S);
			pSrb->Status = STATUS_SUCCESS;

       }
		break;

    default:
 		t << "VideoStream::GetDroppedFramesProperty -- STATUS_NOT_IMPLEMENTED\n";
		pSrb->Status = STATUS_NOT_IMPLEMENTED;
        break;
    }
}


///////////////////////////////////////////////////////////////////////
//	VideoStream::OnRead -- SRB_READ_DATA
//		Called by DispatchData to read data from hardware.
//
void VideoStream::OnRead(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t.Trace(TraceInfo, "VideoStream::OnRead, pSrb = %p\n", pSrb);

	NTSTATUS status;

	m_Mutex.Wait();

	// Check the state of the stream
	if (KSSTATE_STOP == m_eState || KSSTATE_ACQUIRE == m_eState) {
        pSrb->Status = STATUS_UNSUCCESSFUL;
		RequestComplete(pSrb);
	}
	else if (Device()->CurrentReadSize() > pSrb->CommandData.DataBufferArray->FrameExtent) {

		// Validate the buffer size

        ASSERT(Device()->CurrentReadSize() <= pSrb->CommandData.DataBufferArray->FrameExtent);
        pSrb->Status = STATUS_INVALID_PARAMETER;
		RequestComplete(pSrb);
	}
	else {

		pSrb->Status = STATUS_PENDING;
		status = Device()->ReadFrame(pSrb->Irp->MdlAddress, this, pSrb);

		ASSERT (STATUS_PENDING == status); 
	}

	m_Mutex.Release();
}


///////////////////////////////////////////////////////////////////////
//	VideoStream::OnClose -- SRB_CLOSE_STREAM
//		Called by DispatchData to read data from hardware.
//
NTSTATUS VideoStream::OnClose(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "VideoStream::OnClose\n";

	KStream::OnClose(pSrb);

	Device()->DeallocateIsochResources();

	delete m_pVideoInfoHeader;
	m_pVideoInfoHeader = NULL;

	return STATUS_SUCCESS;
}


void VideoStream::OnReadFrameComplete(NTSTATUS Status, ULONG ulLen, PVOID Context) {

	PHW_STREAM_REQUEST_BLOCK pSrb = (PHW_STREAM_REQUEST_BLOCK) Context; 

	t.Trace(TraceInfo, "VideoStream::OnReadFrameComplete, pSrb = %p, Status = %X\n", 
		pSrb, Status);

	if (STATUS_SUCCESS == Status) {
		// Set information fields about the data captured
		pSrb->ActualBytesTransferred = ulLen;

		m_FrameCaptured++;
		m_FrameInfo.PictureNumber = m_FrameCaptured + m_FrameInfo.DropCount;

		PKSSTREAM_HEADER pDataPacket = pSrb->CommandData.DataBufferArray;
		ASSERT(pDataPacket);

		pDataPacket->DataUsed = ulLen;
		pDataPacket->PresentationTime.Numerator = 1;
		pDataPacket->PresentationTime.Denominator = 1;
		pDataPacket->Duration = m_pVideoInfoHeader->AvgTimePerFrame;

		// if we have a master clock
		if (m_hMasterClockHandle) {

			HW_TIME_CONTEXT Ctx;
			Ctx.HwDeviceExtension = (_HW_DEVICE_EXTENSION *) pSrb->HwDeviceExtension;
			Ctx.HwStreamObject    = pSrb->StreamObject;
			Ctx.Function          = TIME_GET_STREAM_TIME;
			Ctx.Time              = 0;
			Ctx.SystemTime        = 0;

			QueryMasterClockSync(m_hMasterClockHandle, &Ctx);
                
			pDataPacket->PresentationTime.Time = Ctx.Time;
			pDataPacket->OptionsFlags = 0;
      
			pDataPacket->OptionsFlags |= 
				 KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
				 KSSTREAM_HEADER_OPTIONSF_DURATIONVALID |
				 KSSTREAM_HEADER_OPTIONSF_SPLICEPOINT;     // Every frame we generate is a key frame (aka SplicePoint)
		} 
		else {
			pDataPacket->PresentationTime.Time = 0;
			pDataPacket->OptionsFlags &=                       
				~(KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
				KSSTREAM_HEADER_OPTIONSF_DURATIONVALID);
		}

		PKS_FRAME_INFO pFrameInfo = (PKS_FRAME_INFO) (pDataPacket + 1);
		m_FrameInfo.ExtendedHeaderSize = pFrameInfo->ExtendedHeaderSize;
		*pFrameInfo = m_FrameInfo;

	}

	pSrb->Status = Status;
	RequestComplete(pSrb);
}