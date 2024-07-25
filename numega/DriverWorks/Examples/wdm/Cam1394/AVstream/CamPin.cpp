// CamPin.cpp: implementation of the CamPin class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

extern KDebugOnlyTrace	t;

#include	"Camera.h"
#include	"CamLowerDevice.h"
#include	"CamDevice.h"
#include	"CamPin.h"


///////////////////////////////////////////////////////////////////////
// CamPin::OnCreate
//
// Parameters:
// 	I                    - IRP_MJ_CREATE 
// Returns:
// 	STATUS_SUCCESS or STATUS_PENDING
// Comments:
// 	See AVStrMiniPinCreate
//
NTSTATUS CamPin::OnCreate(KIrp I)
{
    NTSTATUS status;

	// Get KS objects:

	PKSPIN Pin		 = PKSPIN(*this);

	t << "CamPin::OnCreate, id=" << Pin->Id << "\n";

    //
    // Indicate the extended header size.
    //
    Pin->StreamHeaderSize = sizeof(KSSTREAM_HEADER) + sizeof(KS_FRAME_INFO);

	// cache the header info

	m_VideoInfoHeader = new (Pin) KsVideoInfoHeader;

	if (m_VideoInfoHeader==NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	// lock framing descriptor for editing

    if ((status = KsEdit(Pin, &Pin->Descriptor, 'deks')) == STATUS_SUCCESS) 
		if ((status = KsEdit(Pin, &Pin->Descriptor->AllocatorFraming, 'deks')) == STATUS_SUCCESS) {

        PKSALLOCATOR_FRAMING_EX framing = const_cast<PKSALLOCATOR_FRAMING_EX>(Pin->Descriptor->AllocatorFraming);

        framing->FramingItem[0].Frames = 2;
        framing->FramingItem[0].PhysicalRange.MinFrameSize = 
        framing->FramingItem[0].PhysicalRange.MaxFrameSize = 
        framing->FramingItem[0].FramingRange.Range.MinFrameSize = 
        framing->FramingItem[0].FramingRange.Range.MaxFrameSize = (*m_VideoInfoHeader)->bmiHeader.biSizeImage;
        framing->FramingItem[0].PhysicalRange.Stepping = 
        framing->FramingItem[0].FramingRange.Range.Stepping = 0;
	}

	ASSERT(status==STATUS_SUCCESS);
                    
    return status;
}


///////////////////////////////////////////////////////////////////////
// CamPin::OnSetDeviceState
//
// Parameters:
// 	From                 - The target KSSTATE after receipt of the IRP
// 	To                   - The previous KSSTATE
// Returns:
// 	STATUS_SUCCESS or the error code.  Do NOT return STATUS_PENDING
//	from this method since the system holds a mutex.
// Comments:
// 	Pins that use the standard transport mechanism receive 
//  filtered (single step) state changes through the owning pipe through 
//  this routine.
//  Non-standard transports receive non-filtered (1+ step) state changes
//  through this routine. See AVStrMiniPinSetDeviceState
//
NTSTATUS CamPin::OnSetDeviceState(KSSTATE To, KSSTATE From) 
{
	t.Trace(TraceInfo,"CamPin::OnSetDeviceState, id=%d from %s to %s\n", 
		PKSPIN(*this)->Id, KsState(From), KsState(To));

    NTSTATUS  Status = STATUS_SUCCESS;
	PKSFILTER pfilter = KsPin::Parent();
	ASSERT(m_VideoInfoHeader);
	
    switch (To) 
    {
        case KSSTATE_STOP:
			// stop the host listening and the device transmitting
			Status = Device()->StopListen(this);

			if (STATUS_PENDING == Status) {
				m_StopEvent.Wait();
				// check status of the "stop" operation.  The camera
				// should not be in "started" state (CAMERA_STATE_STARTED)
				Status = (CAMERA_STATE_STARTED == Device()->State())?
								STATUS_UNSUCCESSFUL:STATUS_SUCCESS;
			}

            break;

        case KSSTATE_ACQUIRE:
            // This is a KS only state, that has no correspondence in DirectShow
            break;
            
        case KSSTATE_PAUSE:
			// Pause toggles
			if (From == KSSTATE_ACQUIRE || From == KSSTATE_STOP)
			{  
				// @@TODO Zero the frame counters
/*
				m_FrameCaptured = 0;
				m_FrameInfo.PictureNumber = 0;
				m_FrameInfo.DropCount = 0;
				m_FrameInfo.dwFrameFlags = 0;
*/
			}

			// @@TODO set to pause...
            break;
            
        case KSSTATE_RUN:
			// start the 1394 host isoc listening and the camera
			// transmitting frames to the host 
			Status = Device()->StartListen(this);

			if (STATUS_PENDING == Status) {
				m_StartEvent.Wait();
				// check status of the "start" operation.  The camera
				// should be in "started" state (CAMERA_STATE_STARTED)
				Status = (CAMERA_STATE_STARTED == Device()->State())?
								STATUS_SUCCESS:STATUS_UNSUCCESSFUL;
			}

			break;
    }
    
	return Status;
}


///////////////////////////////////////////////////////////////////////
// CamPin::OnIntersection
//
// Parameters:
// 	Irp                  - IRP with the request
// 	CallerDataRange      - Ptr to one of the data ranges supplied by the client
// 	DescriptorDataRange  - Ptr to one of the data ranges from the pin descriptor
// 	BufferSize           - Size in bytes of the buffer pointed to by the Data
// 	Data                 - Pointer to the buffer to contain the data format
//					       structure representing the best format in the intersection of the
//						   two data ranges. Can be NULL.
// 	DataSize             - Resulting buffer size copied
// Returns:
// 	STATUS_SUCCESS if there is an intersection and it fits in the supplied
//  buffer, STATUS_BUFFER_OVERFLOW for successful size queries, STATUS_NO_MATCH
//  if the intersection is empty, or STATUS_BUFFER_TOO_SMALL if the supplied
//  buffer is too small.
// Comments:
// 	none
NTSTATUS CamPin::OnIntersection(
	KIrp /*Irp*/, 
	PKSDATARANGE CallerDataRange, 
	PKSDATARANGE DescriptorDataRange, 
	ULONG BufferSize, 
	PVOID Data, 
	PULONG DataSize
	)
{
	t << "CamPin::OnIntersection, BufSize=" << BufferSize << " \n";

    PKS_DATAFORMAT_VIDEOINFOHEADER FormatVideoInfoHeader = PKS_DATAFORMAT_VIDEOINFOHEADER(Data);
    const GUID					   VideoInfoSpecifier = {STATICGUIDOF(KSDATAFORMAT_SPECIFIER_VIDEOINFO)};
    ULONG						   DataFormatSize;
    
    // Make sure this is the video data range

    if (!IsEqualGUID(CallerDataRange->Specifier, VideoInfoSpecifier ))  {
		t.Trace(TraceWarning, "Field mismatch: PKSDATARANGE->specifier\n");
	    return STATUS_NO_MATCH;
	}
	
    PKS_DATARANGE_VIDEO callerDataRange = PKS_DATARANGE_VIDEO(CallerDataRange);
    PKS_DATARANGE_VIDEO descriptorDataRange = PKS_DATARANGE_VIDEO(DescriptorDataRange);

    // Check that the other fields match

    if ((callerDataRange->bFixedSizeSamples		 != descriptorDataRange->bFixedSizeSamples) ||
        (callerDataRange->bTemporalCompression	 != descriptorDataRange->bTemporalCompression) ||
        (callerDataRange->StreamDescriptionFlags != descriptorDataRange->StreamDescriptionFlags) ||
        (callerDataRange->MemoryAllocationFlags  != descriptorDataRange->MemoryAllocationFlags) ||
        (RtlCompareMemory (&callerDataRange->ConfigCaps,
                &descriptorDataRange->ConfigCaps,
                sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)) != 
                sizeof (KS_VIDEO_STREAM_CONFIG_CAPS))) 
    {
		t.Trace(TraceWarning, "Field mismatch: callerDataRange != descriptorDataRange\n");
        return STATUS_NO_MATCH;
    }
    
	// get required size:

    DataFormatSize = sizeof( KSDATAFORMAT ) + KS_SIZE_VIDEOHEADER( &callerDataRange->VideoInfoHeader );

	// for size query, return the required size
        
    if (BufferSize == 0) 
    {
        *DataSize = DataFormatSize;
        t.Trace(TraceWarning, "Required size OnIntersection() = %d bytes\n", DataFormatSize);
        return STATUS_BUFFER_OVERFLOW;            
    } 
	else if (BufferSize < DataFormatSize)  {

        t.Trace(TraceWarning, "Required size OnIntersection() = %d bytes\n", DataFormatSize);
        return STATUS_BUFFER_TOO_SMALL;	// error
	}
    
    // Buffer is OK. Copy over the KSDATAFORMAT, followed by the actual VideoInfoHeader
        
    *DataSize = DataFormatSize;
        
    // Copy over the KSDATAFORMAT 
    
    memcpy(&FormatVideoInfoHeader->DataFormat, DescriptorDataRange, sizeof( KSDATARANGE ));
    FormatVideoInfoHeader->DataFormat.FormatSize = DataFormatSize;

    // Copy over the caller's requested VIDEOINFOHEADER

    memcpy(&FormatVideoInfoHeader->VideoInfoHeader, &callerDataRange->VideoInfoHeader,
        KS_SIZE_VIDEOHEADER( &callerDataRange->VideoInfoHeader ) );

    // Calculate biSizeImage for this request, and put the result in both
    // the biSizeImage field of the bmiHeader AND in the SampleSize field
    // of the DataFormat.
	//
	// Adjust for compression if any

    FormatVideoInfoHeader->VideoInfoHeader.bmiHeader.biSizeImage =
        FormatVideoInfoHeader->DataFormat.SampleSize = 
        KS_DIBSIZE( FormatVideoInfoHeader->VideoInfoHeader.bmiHeader );

    return STATUS_SUCCESS;
}
