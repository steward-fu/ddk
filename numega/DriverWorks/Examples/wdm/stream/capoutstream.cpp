// capoutstream.cpp - preview stream of Video Capture sample
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

#include "capoutstream.h"

//---------------------------------------------------------------------------
// All of the video and vbi data formats we might use
//---------------------------------------------------------------------------

// Warning, RGB8 support is not finished
// #define USE_RGB8


#ifdef USE_RGB8

#define D_X 320
#define D_Y 240

const KS_DATARANGE_VIDEO_PALETTE VidcapOutputStream::m_StreamFormatRGB8Bpp = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO_PALETTE),            // FormatSize
        0,                                              // Flags
        D_X * D_Y * 1,                                  // SampleSize
        0,                                              // Reserved

        STATIC_KSDATAFORMAT_TYPE_VIDEO,                 // aka. MEDIATYPE_Video
        0xe436eb7a, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB8,
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO         // aka. FORMAT_VideoInfo
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE,   // StreamDescriptionFlags
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)


    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
        KS_AnalogVideo_NTSC_M,                   // AnalogVideoStandard
        720,480,        // InputSize, (the inherent size of the incoming signal
	                //             with every digitized pixel unique)
        160,120,        // MinCroppingSize, smallest rcSrc cropping rect allowed
        720,480,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
        8,              // CropGranularityX, granularity of cropping size
        1,              // CropGranularityY, 
        8,              // CropAlignX, alignment of cropping rect 
        1,              // CropAlignY;
        160, 120,       // MinOutputSize, smallest bitmap stream can produce
        720, 480,       // MaxOutputSize, largest  bitmap stream can produce
        8,              // OutputGranularityX, granularity of output bitmap size
        1,              // OutputGranularityY;
        0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
        0,              // StretchTapsY
        0,              // ShrinkTapsX 
        0,              // ShrinkTapsY 
        333667,         // MinFrameInterval, 100 nS units
        333667,         // MaxFrameInterval, 100 nS units
        1 * 30 * 160 * 120,  // MinBitsPerSecond;
        1 * 30 * 720 * 480   // MaxBitsPerSecond;
    }, 

    // KS_VIDEOINFO (default format)
    {
        0,0,0,0,                            // RECT  rcSource; 
        0,0,0,0,                            // RECT  rcTarget; 
        D_X * D_Y * 1 * 30,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        333667,                             // REFERENCE_TIME  AvgTimePerFrame;   

        sizeof (KS_BITMAPINFOHEADER) + sizeof (KS_SIZE_PALETTE), // DWORD biSize;
        D_X,                                // LONG  biWidth;
        D_Y,                                // LONG  biHeight;
        1,                                  // WORD  biPlanes;
        8,                                  // WORD  biBitCount;
        KS_BI_RGB,                          // DWORD biCompression;
        D_X * D_Y * 1,                      // DWORD biSizeImage;
        0,                                  // LONG  biXPelsPerMeter;
        0,                                  // LONG  biYPelsPerMeter;
        16,                                 // DWORD biClrUsed;
        16,                                 // DWORD biClrImportant;
        
        // default palette
        {
        //  B       G       R       Reserved
            0x00,   0x00,   0x00,  0x00,       // 0  (black)
            0x10,   0x10,   0x10,  0x00,       // 1  
            0x20,   0x20,   0x20,  0x00,       // 2  
            0x30,   0x30,   0x30,  0x00,       // 3
            0x40,   0x40,   0x40,  0x00,       // 4
            0x50,   0x50,   0x50,  0x00,       // 5
            0x60,   0x60,   0x60,  0x00,       // 6  (gray ramp)
            0x70,   0x70,   0x70,  0x00,       // 7
            0x80,   0x80,   0x80,  0x00,       // 8
            0x90,   0x90,   0x90,  0x00,       // 9
            0xa0,   0xa0,   0xa0,  0x00,       // 10
            0xb0,   0xb0,   0xb0,  0x00,       // 11
            0xc0,   0xc0,   0xc0,  0x00,       // 12
            0xd0,   0xd0,   0xd0,  0x00,       // 13
            0xe0,   0xe0,   0xe0,  0x00,       // 14
            0xff,   0xff,   0xff,  0x00,       // 15 (white)

        }
    }
}; 
    
#undef D_X
#undef D_Y
        
#endif  // USE_RGB8


#define D_X 320
#define D_Y 240

const KS_DATARANGE_VIDEO VidcapOutputStream::m_StreamFormatRGB24Bpp_Capture = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),            // FormatSize
        0,                                      // Flags
        D_X * D_Y * 3,                          // SampleSize
        0,                                      // Reserved

        STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
        0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE,   // StreamDescriptionFlags
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
        KS_AnalogVideo_NTSC_M |
        KS_AnalogVideo_PAL_B,                    // AnalogVideoStandard
        720,480,        // InputSize, (the inherent size of the incoming signal
	                //             with every digitized pixel unique)
        160,120,        // MinCroppingSize, smallest rcSrc cropping rect allowed
        720,480,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
        8,              // CropGranularityX, granularity of cropping size
        1,              // CropGranularityY
        8,              // CropAlignX, alignment of cropping rect 
        1,              // CropAlignY;
        160, 120,       // MinOutputSize, smallest bitmap stream can produce
        720, 480,       // MaxOutputSize, largest  bitmap stream can produce
        8,              // OutputGranularityX, granularity of output bitmap size
        1,              // OutputGranularityY;
        0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
        0,              // StretchTapsY
        0,              // ShrinkTapsX 
        0,              // ShrinkTapsY 
        333667,         // MinFrameInterval, 100 nS units
        333667,         // MaxFrameInterval, 100 nS units
        3 * 30 * 160 * 120,  // MinBitsPerSecond;
        3 * 30 * 720 * 480   // MaxBitsPerSecond;
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                            // RECT  rcSource; 
        0,0,0,0,                            // RECT  rcTarget; 
        D_X * D_Y * 3 * 30,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        333667,                             // REFERENCE_TIME  AvgTimePerFrame;   

        sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
        D_X,                                // LONG  biWidth;
        D_Y,                                // LONG  biHeight;
        1,                                  // WORD  biPlanes;
        24,                                 // WORD  biBitCount;
        KS_BI_RGB,                          // DWORD biCompression;
        D_X * D_Y * 3,                      // DWORD biSizeImage;
        0,                                  // LONG  biXPelsPerMeter;
        0,                                  // LONG  biYPelsPerMeter;
        0,                                  // DWORD biClrUsed;
        0                                   // DWORD biClrImportant;
    }
}; 

#undef D_X
#undef D_Y

//---------------------------------------------------------------------------
//  STREAM_Capture Formats
//---------------------------------------------------------------------------
const PKSDATAFORMAT VidcapOutputStream::m_StreamFormats[] = 
{
    (PKSDATAFORMAT) &m_StreamFormatRGB24Bpp_Capture,
#ifdef USE_RGB8
    (PKSDATAFORMAT) &m_StreamFormatRGB8Bpp,
#endif
};

#define NUM_STREAM_0_FORMATS (SIZEOF_ARRAY(Stream0Formats))

// -----------------------------------------------------------------
// STREAM_Capture
// -----------------------------------------------------------------
const HW_STREAM_INFORMATION VidcapOutputStream::m_StreamInfo =
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
	VidcapBaseStream::VideoStreamPropertiesCount(),     // NumStreamPropArrayEntries
	VidcapBaseStream::VideoStreamProperties(),// StreamPropertiesArray
    0,                                      // NumStreamEventArrayEntries;
    0,                                      // StreamEventsArray;
    (GUID *) &PINNAME_VIDEO_CAPTURE,        // Category
    (GUID *) &PINNAME_VIDEO_CAPTURE,        // Name
    0,                                      // MediumsCount
    NULL,                                   // Mediums
	FALSE									// BridgeStream
};


VidcapOutputStream::VidcapOutputStream(void) :
	VidcapBaseStream()
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapOutputStream::VidcapOutputStream -- contructed\n";
	#endif

}

// -- stream initialization code --
void VidcapOutputStream::InitializeStream()
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapOutputStream::InitializeStream -- calling InitializeBaseClass\n";
	#endif

	InitializeBaseClass(m_StreamInfo, sizeof (KS_FRAME_INFO));
}


// -- methods called from DispatchData method for hardware read/write --
void VidcapOutputStream::OnRead(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    // Rule: 
    // Only accept read requests when in either the Pause or Run
    // States.  If reading, then queue it.  If Stopped, immediately
	// return the SRB.
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapOutputStream::OnRead -- Start\n";
	#endif

    if (KSSTATE_STOP != m_eState)
		QueueSrb(pSrb);
	else
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceAlways << "VidcapOutputStream::OnRead -- KSSTATE_STOP... Can't accept read\n";
		#endif

        NextSrb(pSrb);  // -- should this indicate an error?
	}

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapOutputStream::OnRead -- End\n";
	#endif

}

/*
VOID VidcapOutputStream::GetInformation(PHW_STREAM_INFORMATION pInfo)
{
	pInfo->NumberOfPossibleInstances = 1;
	pInfo->DataFlow = KSPIN_DATAFLOW_OUT;
	pInfo->DataAccessible = TRUE;
	pInfo->NumStreamPropArrayEntries = NUM_PREVIEW_STREAM_PROPERTIES;
	pInfo->StreamPropertiesArray = m_Properties;
	pInfo->NumberOfFormatArrayEntries = NUM_PREVIEW_STREAM_FORMATS;
	pInfo->StreamFormatsArray = m_Formats;
	pInfo->Category = &m_Category;
	pInfo->Name = &m_Name;
}
*/
