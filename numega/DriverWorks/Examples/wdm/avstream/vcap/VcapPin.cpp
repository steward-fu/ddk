// VcapPin.cpp: implementation of the VcapPin class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

#include "Vcap.h"
#include "VcapFormats.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VcapPin::VcapPin() :
 KsPin(), 
	 m_VideoInfoHeader(0), m_ulVideoDroppedFrames(0), m_bAnimate(true)
{

}

VcapPin::~VcapPin()
{
}



///////////////////////////////////////////////////////////////////////
// VcapPin::OnCreate
//
// Parameters:
// 	I                    - IRP_MJ_CREATE 
// Returns:
// 	STATUS_SUCCESS or STATUS_PENDING
// Comments:
// 	See AVStrMiniPinCreate
NTSTATUS VcapPin::OnCreate(KIrp I)
{
    NTSTATUS status;

	// Get KS objects:

	PKSPIN Pin		 = PKSPIN(*this);

	g_Trace << "VcapPin::OnCreate, id=" << Pin->Id << EOL;

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
// VcapPin::OnSetDeviceState
//
// Parameters:
// 	From                 - The target KSSTATE after receipt of the IRP
// 	To                   - The previous KSSTATE
// Returns:
// 	STATUS_SUCCESS or the error code
// Comments:
// 	Pins that use the standard transport mechanism receive 
//  filtered (single step) state changes through the owning pipe through 
//  this routine.
//  Non-standard transports receive non-filtered (1+ step) state changes
//  through this routine. See AVStrMiniPinSetDeviceState
//
NTSTATUS VcapPin::OnSetDeviceState(KSSTATE To, KSSTATE From) 
{
	g_Trace << "VcapPin::OnSetDeviceState, id=" << PKSPIN(*this)->Id 
	    << " from " << From << " to " << To << EOL;

    NTSTATUS  Status = STATUS_SUCCESS;
	PKSFILTER pfilter = KsPin::Parent();
	ASSERT(m_VideoInfoHeader);
	
    switch (To) 
    {
        case KSSTATE_STOP:
			// stop timer and clear stats
			//
			m_Timer.Stop();
			m_PictureNumber = m_ulVideoDroppedFrames = 0;
            break;
            
        case KSSTATE_ACQUIRE:
            //
            // This is a KS only state, that has no correspondence in DirectShow
            // 
            break;
            
        case KSSTATE_PAUSE:
			// Pause toggles
			if (From == KSSTATE_RUN) {
				m_Timer.Stop();
				break;
			}
			else; // fall through: pause toggles!
            //break;
            
        case KSSTATE_RUN:
			// start the timer
			//
			if (!m_Timer.IsOn())
				m_Timer.Start((*m_VideoInfoHeader)->AvgTimePerFrame, pfilter);
            break;
    }
    
	return Status;
}


///////////////////////////////////////////////////////////////////////
// VcapPin::OnIntersection
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
NTSTATUS VcapPin::OnIntersection (KIrp /*Irp*/, 
			PKSDATARANGE CallerDataRange, 
			PKSDATARANGE DescriptorDataRange, 
			ULONG BufferSize, 
			PVOID Data, 
			PULONG DataSize)
{
	g_Trace << "VcapPin::OnIntersection, BufSize=" << BufferSize << " " << EOL;

    PKS_DATAFORMAT_VIDEOINFOHEADER FormatVideoInfoHeader = PKS_DATAFORMAT_VIDEOINFOHEADER(Data);
    ULONG						   DataFormatSize;

    const GUID VideoInfoSpecifier = 
        {STATICGUIDOF(KSDATAFORMAT_SPECIFIER_VIDEOINFO)};
    

    // Make sure this is the videa data range

    if (!IsEqualGUID(CallerDataRange->Specifier, VideoInfoSpecifier ))  {
		    return STATUS_NO_MATCH;
	}

	
    PKS_DATARANGE_VIDEO callerDataRange = PKS_DATARANGE_VIDEO(CallerDataRange);
    PKS_DATARANGE_VIDEO descriptorDataRange = PKS_DATARANGE_VIDEO(DescriptorDataRange);

    //
    // Check that the other fields match
    //
    if ((callerDataRange->bFixedSizeSamples != 
            descriptorDataRange->bFixedSizeSamples) ||
        (callerDataRange->bTemporalCompression != 
            descriptorDataRange->bTemporalCompression) ||
        (callerDataRange->StreamDescriptionFlags != 
            descriptorDataRange->StreamDescriptionFlags) ||
        (callerDataRange->MemoryAllocationFlags != 
            descriptorDataRange->MemoryAllocationFlags) ||
        (RtlCompareMemory (&callerDataRange->ConfigCaps,
                &callerDataRange->ConfigCaps,
                sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)) != 
                sizeof (KS_VIDEO_STREAM_CONFIG_CAPS))) 
    {
		g_Trace.Trace(TraceWarning, "Field mismatch OnIntersect()");
        return STATUS_NO_MATCH;
    }
    
	// get required size:

    DataFormatSize = sizeof( KSDATAFORMAT ) + KS_SIZE_VIDEOHEADER( &callerDataRange->VideoInfoHeader );

	// for size query, return the required size
        
    if (BufferSize == 0) 
    {
        *DataSize = DataFormatSize;
        g_Trace.Trace(TraceWarning, "Required size OnIntersect() = %d bytes\n", DataFormatSize);
        return STATUS_BUFFER_OVERFLOW;            
    } else if (BufferSize < DataFormatSize)  {

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


///////////////////////////////////////////////////////////////////////
// VcapPin::GetImage
//
// Parameters:
// 	pStreamHeader        - Stream header for the frame
// 	Data                 - Data buffer to put the frame image in
// 	ByteCount            - Size of the buffer
// Returns:
// 	Nuber of actually copied bytes
// Comments:
// 	This method is called from the filter's OnProcess() to fill the 
//  available pin's buffer with a synthesized image.
//  This one produces NTSC color bars.
//	To generate some other type of image, override this method in
//  a derived pin class.
ULONG VcapPin::GetImage
	(PKSSTREAM_HEADER pStreamHeader, OUT PVOID Data, IN ULONG ByteCount)
{
	static UCHAR ColorBarsRGB24 [3][8] = 
	{
	//  Whi Yel Cya Grn Mag Red Blu Blk
		255,  0,255,  0,255,  0,255,  0,    // Blue
		255,255,255,255,  0,  0,  0,  0,    // Green
		255,255,  0,  0,255,255,  0,  0,    // Red
	};
	static UCHAR ColorBarsYUV [4][8] = 
	{
	//  Whi Yel Cya Grn Mag Red Blu Blk
		128, 16,166, 54,202, 90,240,128,    // U
		235,211,170,145,106, 81, 41, 16,    // Y
		128,146, 16, 34,222,240,109,128,    // V
		235,211,170,145,106, 81, 41, 16     // Y
	};

	ASSERT(pStreamHeader);
	ASSERT(Data && ByteCount);
	
	UINT biWidth        =   (*m_VideoInfoHeader)->bmiHeader.biWidth;
    UINT biHeight       =   (*m_VideoInfoHeader)->bmiHeader.biHeight;
    UINT biSizeImage    =   (*m_VideoInfoHeader)->bmiHeader.biSizeImage;
    UINT biWidthBytes   =   KS_DIBWIDTHBYTES( (*m_VideoInfoHeader)->bmiHeader );
    UINT biBitCount     =   (*m_VideoInfoHeader)->bmiHeader.biBitCount;
    UINT LinesToCopy    =   abs( biHeight );
    DWORD biCompression =   (*m_VideoInfoHeader)->bmiHeader.biCompression;

    // g_Trace.Trace(TraceInfo, "VcapPin::GetImage: biWidth=%d biHeight=%d biSizeImage=%d\n", 
    //    biWidth, biHeight, biSizeImage, biWidthBytes);

	if (biSizeImage > ByteCount) {
		ASSERT(!"Buffer size");
		return 0;
	}


    UINT		Line;
    PUCHAR		Image = (PUCHAR)Data;
	PUCHAR		p;

	m_PictureNumber = m_Timer.Ticks();	// save pic #
    
    // 
    // Synthesize a single line of image data
    //

    if ((biBitCount == 24) && (biCompression == KS_BI_RGB)) 
	{
		 p = (PUCHAR) m_LineBuffer;
        for (UINT x = 0; x < biWidth; x++) {
            UINT col = (x * 8) / biWidth;
            
            *p++ = ColorBarsRGB24[0][col]; // Red
            *p++ = ColorBarsRGB24[1][col]; // Green
            *p++ = ColorBarsRGB24[2][col]; // Blue
        }

		// modulate Green to get some appearence of motion
		if (m_bAnimate)
			for (UINT x=0; x<8; x++) 
				{ColorBarsRGB24[1][x]++;}
	}

    else if ((biBitCount == 16) && (biCompression == FOURCC_YUV422))
    {
		 p = (PUCHAR) m_LineBuffer;
         for (UINT x = 0; x < (biWidth / 2); x++) {
                UINT col = (x * 8) / (biWidth / 2);

                *p++ = ColorBarsYUV[0][col]; // U
                *p++ = ColorBarsYUV[1][col]; // Y
                *p++ = ColorBarsYUV[2][col]; // V
                *p++ = ColorBarsYUV[3][col]; // Y
        }

		// modulate Y to get some appearence of motion
		if (m_bAnimate)
			for (UINT x=0; x<8; x++) 
				{ColorBarsYUV[1][x]++;ColorBarsYUV[3][x]++;}
	}
    else      ASSERT(!"Known Format");

	// fill the rest of the image by copying that line

	for (Line = 0; Line < LinesToCopy; Line++, Image += biWidthBytes) 
		memcpy (Image, m_LineBuffer, biWidthBytes);

	// Fill in the stream header

	pStreamHeader->PresentationTime.Time = m_Timer.TickTime(); // important: relative to the start of the stream!
	pStreamHeader->PresentationTime.Numerator = 1;
	pStreamHeader->PresentationTime.Denominator = 1;
	pStreamHeader->Duration = (*m_VideoInfoHeader)->AvgTimePerFrame;
	pStreamHeader->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_TIMEVALID | 
									   KSSTREAM_HEADER_OPTIONSF_DURATIONVALID;

	return biSizeImage;

}
