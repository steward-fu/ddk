/*++

Copyright (c) 1996, 1997  Microsoft Corporation

Module Name:

   intelcam.c

Abstract:

   driver for the intel camera.

Author:


Environment:

   Kernel mode only


Revision History:

--*/

#include "warn.h"
#include "wdm.h"


#include "intelcam.h"
#include "strmini.h"

#include "prpobj.h"
#include "prpftn.h"


GUID FORMAT_VideoInfo = {
    0x05589f80, 0xc356, 0x11ce, 0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a};
	

// maps fps to an interface number
UCHAR InterfaceMap[] = {7, 7, 7, 7, 7, 7,       // 0..5
                        1, 1, 1, 1,             // 6.. 9
                        0, 0,                   // 10..11
                        3, 3, 3,                // 12..14
                        2, 2, 2,                // 15..17
                        4, 4,                   // 18..19
                        5, 5, 5, 5, 5,          // 20..24
                        6};                     // 25


 
// frame rates supported in the usb camera
LONGLONG SupportedFrameRates[FRAME_RATE_LIST_SIZE] = 
			{
				{NUM_100NANOSEC_UNITS_PERFRAME(0)},
				{NUM_100NANOSEC_UNITS_PERFRAME(6)},
				{NUM_100NANOSEC_UNITS_PERFRAME(10)},
				{NUM_100NANOSEC_UNITS_PERFRAME(12)},
				{NUM_100NANOSEC_UNITS_PERFRAME(15)},
				{NUM_100NANOSEC_UNITS_PERFRAME(18)},
				{NUM_100NANOSEC_UNITS_PERFRAME(20)},
				{NUM_100NANOSEC_UNITS_PERFRAME(25)}
			};


ULONG INTELCAM_DebugTraceLevel
#ifdef MAX_DEBUG
    = MAX_TRACE;
#else
    = MIN_TRACE;
#endif


// **
// format for each type of video stream we support
// **

#ifndef mmioFOURCC    
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                \
        ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
        ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif  

// ------------------------------------------------------------------------
// Property sets for all video capture streams
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


/*
 * The following are for FOURCC IYUV
 */

#define D_X 160
#define D_Y 120

KS_DATARANGE_VIDEO INTELCAM_StreamFormat_0 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,                      // Flags
        (D_X * D_Y * 12)/8,     // SampleSize
        0,                      // Reserved
//MEDIATYPE_Video
        STATIC_KSDATAFORMAT_TYPE_VIDEO,      
//  MEDIASUBTYPE_IYUV
        FCC_FORMAT_YUV12N, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71,
//FORMAT_VideoInfo
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE,   // StreamDescriptionFlags
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO,
        KS_AnalogVideo_None,    // VideoStandard
        D_X, D_Y,               // InputSize, (the inherent size of the incoming signal
                                //             with every digitized pixel unique)
        D_X, D_Y,               // MinCroppingSize, smallest rcSrc cropping rect allowed
        D_X, D_Y,               // MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,                      // CropGranularityX, granularity of cropping size
        1,                      // CropGranularityY
        1,                      // CropAlignX, alignment of cropping rect 
        1,                      // CropAlignY;
        D_X, D_Y,               // MinOutputSize, smallest bitmap stream can produce
        D_X, D_Y,               // MaxOutputSize, largest  bitmap stream can produce
        1,                      // OutputGranularityX, granularity of output bitmap size
        1,                      // OutputGranularityY;
        0,                      // StretchTapsX
    	0,                      // StretchTapsY
        0,                      // ShrinkTapsX
    	0,                      // ShrinkTapsY
        400000,         // MinFrameInterval, 100 nS units
        1666667,         // MaxFrameInterval, 100 nS units
        12 * 6 * D_X * D_Y,  // MinBitsPerSecond;
        12 * 25 * D_X * D_Y   // MaxBitsPerSecond;
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                        // RECT  rcSource; 
        0,0,0,0,                        // RECT  rcTarget; 
        D_X * D_Y * 12 * 25,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        400000,                             // REFERENCE_TIME  AvgTimePerFrame;   
        sizeof (KS_BITMAPINFOHEADER),       //    DWORD      biSize;
        D_X,                                //    LONG       biWidth;
        D_Y,                                //    LONG       biHeight;
        1,                                  //    WORD       biPlanes;
        12,                                 //    WORD       biBitCount;
        FCC_FORMAT_YUV12N,                  //    DWORD      biCompression;
        (D_X * D_Y * 12)/8,                 //    DWORD      biSizeImage;
        0,                                  //    LONG       biXPelsPerMeter;
        0,                                  //    LONG       biYPelsPerMeter;
        0,                                  //    DWORD      biClrUsed;
        0                                   //    DWORD      biClrImportant;

    }
}; 
    
#undef D_X
#undef D_Y
    
    
    

#define D_X 176
#define D_Y 144

KS_DATARANGE_VIDEO INTELCAM_StreamFormat_1 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,                      // Flags
        (D_X * D_Y * 12)/8,     // SampleSize
        0,                      // Reserved
//MEDIATYPE_Video
        STATIC_KSDATAFORMAT_TYPE_VIDEO,      
//MEDIASUBTYPE_IYUV        
        FCC_FORMAT_YUV12N, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71,
//FORMAT_VideoInfo        
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE,   // StreamDescriptionFlags
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO,
        KS_AnalogVideo_None,    // VideoStandard
        D_X, D_Y,               // InputSize, (the inherent size of the incoming signal
                                //             with every digitized pixel unique)
        D_X, D_Y,               // MinCroppingSize, smallest rcSrc cropping rect allowed
        D_X, D_Y,               // MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,                      // CropGranularityX, granularity of cropping size
        1,                      // CropGranularityY
        1,                      // CropAlignX, alignment of cropping rect 
        1,                      // CropAlignY;
        D_X, D_Y,               // MinOutputSize, smallest bitmap stream can produce
        D_X, D_Y,               // MaxOutputSize, largest  bitmap stream can produce
        1,                      // OutputGranularityX, granularity of output bitmap size
        1,                      // OutputGranularityY;
        0,                      // StretchTapsX
    	0,                      // StretchTapsY
        0,                      // ShrinkTapsX
    	0,                      // ShrinkTapsY
        400000,                 // MinFrameInterval, 100 nS units
        1666667,                 // MaxFrameInterval, 100 nS units
        12 * 6 * D_X * D_Y,     // MinBitsPerSecond;
        12 * 25 * D_X * D_Y      // MaxBitsPerSecond;
    }, 
        

    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                        // RECT  rcSource; 
        0,0,0,0,                        // RECT  rcTarget; 
        D_X * D_Y * 12 * 25,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        400000,                             // REFERENCE_TIME  AvgTimePerFrame;   
        sizeof (KS_BITMAPINFOHEADER),       //    DWORD      biSize;
        D_X,                                //    LONG       biWidth;
        D_Y,                                //    LONG       biHeight;
        1,                                  //    WORD       biPlanes;
        12,                                 //    WORD       biBitCount;
        FCC_FORMAT_YUV12N,                  //    DWORD      biCompression;
        (D_X * D_Y * 12)/8,                 //    DWORD      biSizeImage;
        0,                                  //    LONG       biXPelsPerMeter;
        0,                                  //    LONG       biYPelsPerMeter;
        0,                                  //    DWORD      biClrUsed;
        0                                   //    DWORD      biClrImportant;

    }
}; 

#undef D_X
#undef D_Y


    

#define D_X 320
#define D_Y 240

KS_DATARANGE_VIDEO INTELCAM_StreamFormat_2 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,                      // Flags
        (D_X * D_Y * 12)/8,     // SampleSize
        0,                      // Reserved
//MEDIATYPE_Video
        STATIC_KSDATAFORMAT_TYPE_VIDEO,      
//MEDIASUBTYPE_IYUV        
        FCC_FORMAT_YUV12N, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71,
//FORMAT_VideoInfo        
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE,   // StreamDescriptionFlags
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO,
        KS_AnalogVideo_None,    // VideoStandard
        D_X, D_Y,               // InputSize, (the inherent size of the incoming signal
                                //             with every digitized pixel unique)
        D_X, D_Y,               // MinCroppingSize, smallest rcSrc cropping rect allowed
        D_X, D_Y,               // MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,                      // CropGranularityX, granularity of cropping size
        1,                      // CropGranularityY
        1,                      // CropAlignX, alignment of cropping rect 
        1,                      // CropAlignY;
        D_X, D_Y,               // MinOutputSize, smallest bitmap stream can produce
        D_X, D_Y,               // MaxOutputSize, largest  bitmap stream can produce
        1,                      // OutputGranularityX, granularity of output bitmap size
        1,                      // OutputGranularityY;
        0,                      // StretchTapsX
    	0,                      // StretchTapsY
        0,                      // ShrinkTapsX
    	0,                      // ShrinkTapsY
        400000,                 // MinFrameInterval, 100 nS units
        1666667,                 // MaxFrameInterval, 100 nS units
        12 * 6 * D_X * D_Y,  // MinBitsPerSecond;
        12 * 25 * D_X * D_Y   // MaxBitsPerSecond;
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                        // RECT  rcSource; 
        0,0,0,0,                        // RECT  rcTarget; 
        D_X * D_Y * 12 * 25,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        400000,                             // REFERENCE_TIME  AvgTimePerFrame;   

        sizeof (KS_BITMAPINFOHEADER),       //    DWORD      biSize;
        D_X,                                //    LONG       biWidth;
        D_Y,                                //    LONG       biHeight;
        1,                                  //    WORD       biPlanes;
        12,                                 //    WORD       biBitCount;
        FCC_FORMAT_YUV12N,                  //    DWORD      biCompression;
        (D_X * D_Y * 12)/8,                 //    DWORD      biSizeImage;
        0,                                  //    LONG       biXPelsPerMeter;
        0,                                  //    LONG       biYPelsPerMeter;
        0,                                  //    DWORD      biClrUsed;
        0                                   //    DWORD      biClrImportant;

    }
}; 
    
#undef D_X
#undef D_Y



/*
 * The following are for FOURCC I420
 */

#define D_X 160
#define D_Y 120

KS_DATARANGE_VIDEO INTELCAM_StreamFormat_3 = 
//??KS_DATARANGE_VIDEO INTELCAM_StreamFormat_0 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,                      // Flags
        (D_X * D_Y * 12)/8,     // SampleSize
        0,                      // Reserved
//MEDIATYPE_Video
        STATIC_KSDATAFORMAT_TYPE_VIDEO,      
//  MEDIASUBTYPE_I420
        FCC_FORMAT_YUV12A, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71,
//FORMAT_VideoInfo
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE,   // StreamDescriptionFlags
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO,
        KS_AnalogVideo_None,    // VideoStandard
        D_X, D_Y,               // InputSize, (the inherent size of the incoming signal
                                //             with every digitized pixel unique)
        D_X, D_Y,               // MinCroppingSize, smallest rcSrc cropping rect allowed
        D_X, D_Y,               // MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,                      // CropGranularityX, granularity of cropping size
        1,                      // CropGranularityY
        1,                      // CropAlignX, alignment of cropping rect 
        1,                      // CropAlignY;
        D_X, D_Y,               // MinOutputSize, smallest bitmap stream can produce
        D_X, D_Y,               // MaxOutputSize, largest  bitmap stream can produce
        1,                      // OutputGranularityX, granularity of output bitmap size
        1,                      // OutputGranularityY;
        0,                      // StretchTapsX
    	0,                      // StretchTapsY
        0,                      // ShrinkTapsX
    	0,                      // ShrinkTapsY
        400000,         // MinFrameInterval, 100 nS units
        1666667,         // MaxFrameInterval, 100 nS units
        12 * 6 * D_X * D_Y,  // MinBitsPerSecond;
        12 * 25 * 320 * 240   // MaxBitsPerSecond;
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                        // RECT  rcSource; 
        0,0,0,0,                        // RECT  rcTarget; 
        D_X * D_Y * 12 * 25,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        400000,                             // REFERENCE_TIME  AvgTimePerFrame;   
        sizeof (KS_BITMAPINFOHEADER),       //    DWORD      biSize;
        D_X,                                //    LONG       biWidth;
        D_Y,                                //    LONG       biHeight;
        1,                                  //    WORD       biPlanes;
        12,                                 //    WORD       biBitCount;
        FCC_FORMAT_YUV12A,                  //    DWORD      biCompression;
        (D_X * D_Y * 12)/8,                 //    DWORD      biSizeImage;
        0,                                  //    LONG       biXPelsPerMeter;
        0,                                  //    LONG       biYPelsPerMeter;
        0,                                  //    DWORD      biClrUsed;
        0                                   //    DWORD      biClrImportant;

    }
}; 
    
#undef D_X
#undef D_Y
    
    
    

#define D_X 176
#define D_Y 144

KS_DATARANGE_VIDEO INTELCAM_StreamFormat_4 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,                      // Flags
        (D_X * D_Y * 12)/8,     // SampleSize
        0,                      // Reserved
//MEDIATYPE_Video
        STATIC_KSDATAFORMAT_TYPE_VIDEO,      
//MEDIASUBTYPE_I420        
        FCC_FORMAT_YUV12A, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71,
//FORMAT_VideoInfo        
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE,   // StreamDescriptionFlags
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO,
        KS_AnalogVideo_None,    // VideoStandard
        D_X, D_Y,               // InputSize, (the inherent size of the incoming signal
                                //             with every digitized pixel unique)
        D_X, D_Y,               // MinCroppingSize, smallest rcSrc cropping rect allowed
        D_X, D_Y,               // MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,                      // CropGranularityX, granularity of cropping size
        1,                      // CropGranularityY
        1,                      // CropAlignX, alignment of cropping rect 
        1,                      // CropAlignY;
        D_X, D_Y,               // MinOutputSize, smallest bitmap stream can produce
        D_X, D_Y,               // MaxOutputSize, largest  bitmap stream can produce
        1,                      // OutputGranularityX, granularity of output bitmap size
        1,                      // OutputGranularityY;
        0,                      // StretchTapsX
    	0,                      // StretchTapsY
        0,                      // ShrinkTapsX
    	0,                      // ShrinkTapsY
        400000,                 // MinFrameInterval, 100 nS units
        1666667,                 // MaxFrameInterval, 100 nS units
        12 * 6 * D_X * D_Y,     // MinBitsPerSecond;
        12 * 25 * D_X * D_Y      // MaxBitsPerSecond;
    }, 
        

    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                        // RECT  rcSource; 
        0,0,0,0,                        // RECT  rcTarget; 
        D_X * D_Y * 12 * 25,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        400000,                             // REFERENCE_TIME  AvgTimePerFrame;   
        sizeof (KS_BITMAPINFOHEADER),       //    DWORD      biSize;
        D_X,                                //    LONG       biWidth;
        D_Y,                                //    LONG       biHeight;
        1,                                  //    WORD       biPlanes;
        12,                                 //    WORD       biBitCount;
        FCC_FORMAT_YUV12A,                  //    DWORD      biCompression;
        (D_X * D_Y * 12)/8,                 //    DWORD      biSizeImage;
        0,                                  //    LONG       biXPelsPerMeter;
        0,                                  //    LONG       biYPelsPerMeter;
        0,                                  //    DWORD      biClrUsed;
        0                                   //    DWORD      biClrImportant;

    }
}; 

#undef D_X
#undef D_Y


    

#define D_X 320
#define D_Y 240

KS_DATARANGE_VIDEO INTELCAM_StreamFormat_5 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,                      // Flags
        (D_X * D_Y * 12)/8,     // SampleSize
        0,                      // Reserved
//MEDIATYPE_Video
        STATIC_KSDATAFORMAT_TYPE_VIDEO,      
//MEDIASUBTYPE_I420        
        FCC_FORMAT_YUV12A, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71,
//FORMAT_VideoInfo        
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE,   // StreamDescriptionFlags
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO,
        KS_AnalogVideo_None,    // VideoStandard
        D_X, D_Y,               // InputSize, (the inherent size of the incoming signal
                                //             with every digitized pixel unique)
        D_X, D_Y,               // MinCroppingSize, smallest rcSrc cropping rect allowed
        D_X, D_Y,               // MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,                      // CropGranularityX, granularity of cropping size
        1,                      // CropGranularityY
        1,                      // CropAlignX, alignment of cropping rect 
        1,                      // CropAlignY;
        D_X, D_Y,               // MinOutputSize, smallest bitmap stream can produce
        D_X, D_Y,               // MaxOutputSize, largest  bitmap stream can produce
        1,                      // OutputGranularityX, granularity of output bitmap size
        1,                      // OutputGranularityY;
        0,                      // StretchTapsX
    	0,                      // StretchTapsY
        0,                      // ShrinkTapsX
    	0,                      // ShrinkTapsY
        400000,                 // MinFrameInterval, 100 nS units
        1666667,                 // MaxFrameInterval, 100 nS units
        12 * 6 * D_X * D_Y,  // MinBitsPerSecond;
        12 * 25 * D_X * D_Y   // MaxBitsPerSecond;
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                        // RECT  rcSource; 
        0,0,0,0,                        // RECT  rcTarget; 
        D_X * D_Y * 12 * 25,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        400000,                             // REFERENCE_TIME  AvgTimePerFrame;   

        sizeof (KS_BITMAPINFOHEADER),       //    DWORD      biSize;
        D_X,                                //    LONG       biWidth;
        D_Y,                                //    LONG       biHeight;
        1,                                  //    WORD       biPlanes;
        12,                                 //    WORD       biBitCount;
        FCC_FORMAT_YUV12A,                  //    DWORD      biCompression;
        (D_X * D_Y * 12)/8,                 //    DWORD      biSizeImage;
        0,                                  //    LONG       biXPelsPerMeter;
        0,                                  //    LONG       biYPelsPerMeter;
        0,                                  //    DWORD      biClrUsed;
        0                                   //    DWORD      biClrImportant;

    }
}; 
    
#undef D_X
#undef D_Y




//---------------------------------------------------------------------------
//  INTELCAM_Stream Formats
//---------------------------------------------------------------------------
PKSDATAFORMAT INTELCAM_StreamFormats[] = 
{
    (PKSDATAFORMAT) &INTELCAM_StreamFormat_0,
    (PKSDATAFORMAT) &INTELCAM_StreamFormat_1,
    (PKSDATAFORMAT) &INTELCAM_StreamFormat_2,
#if 1
    (PKSDATAFORMAT) &INTELCAM_StreamFormat_3,
    (PKSDATAFORMAT) &INTELCAM_StreamFormat_4,
    (PKSDATAFORMAT) &INTELCAM_StreamFormat_5
#endif
};

#define NUM_INTELCAM_STREAM_FORMATS \
             (sizeof (INTELCAM_StreamFormats) / sizeof (PKSDATAFORMAT))


typedef struct _ALL_STREAM_INFO {
    HW_STREAM_INFORMATION   hwStreamInfo;
//    HW_STREAM_OBJECT        hwStreamObject;
} ALL_STREAM_INFO, *PALL_STREAM_INFO;

BOOLEAN INTELCAM_RelatedStreams[] = 
{
    TRUE                                      // StreamIsRelated
};


//---------------------------------------------------------------------------
// Create an array that holds the list of all of the streams supported
//---------------------------------------------------------------------------

ALL_STREAM_INFO Streams [] =
{
    // -----------------------------------------------------------------
    // INTELCAM_Stream
    // -----------------------------------------------------------------

    {
    // HW_STREAM_INFORMATION -------------------------------------------
    1,                                      // NumberOfPossibleInstances
    KSPIN_DATAFLOW_OUT,                     // DataFlow
    TRUE,                                   // DataAccessible
    NUM_INTELCAM_STREAM_FORMATS,            // NumberOfFormatArrayEntries
    INTELCAM_StreamFormats,                 // StreamFormatsArray
    NULL,                                      // ClassReserved[0]
    NULL,                                      // ClassReserved[1]
    NULL,                                      // ClassReserved[2]
    NULL,                                      // ClassReserved[3]
    NUMBER_VIDEO_STREAM_PROPERTIES,         // NumStreamPropArrayEntries
    (PKSPROPERTY_SET) VideoStreamProperties,// StreamPropertiesArray
    0,                                      // NumStreamEventArrayEntries;
    0,                                      // StreamEventsArray;
    (GUID *) &PINNAME_VIDEO_CAPTURE,        // Category
    (GUID *) &PINNAME_VIDEO_CAPTURE,        // Name
    0,                                      // MediumsCount
    NULL,                                   // Mediums
    FALSE									// BridgeStream 
	}

};

#define DRIVER_STREAM_COUNT (sizeof (Streams) / sizeof (ALL_STREAM_INFO))




/*
** AdapterCompareGUIDsAndFormatSize()
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

BOOLEAN 
AdapterCompareGUIDsAndFormatSize(
    IN PKSDATARANGE DataRange1,
    IN PKSDATARANGE DataRange2)
{
    return (
        IsEqualGUID (
            &DataRange1->MajorFormat, 
            &DataRange2->MajorFormat) &&
        IsEqualGUID (
            &DataRange1->SubFormat, 
            &DataRange2->SubFormat) &&
        IsEqualGUID (
            &DataRange1->Specifier, 
            &DataRange2->Specifier) &&
        (DataRange1->FormatSize == DataRange2->FormatSize));
}




/*
** AdapterFormatFromRange()
**
**   Returns a DATAFORMAT from a DATARANGE
**
** Arguments:
**
**         IN PHW_STREAM_REQUEST_BLOCK pSrb 
**
** Returns:
** 
**  STATUS_SUCCESS if format is supported
**
** Side Effects:  none
*/


NTSTATUS 
AdapterFormatFromRange(
        IN PHW_STREAM_REQUEST_BLOCK Srb)
{
    PSTREAM_DATA_INTERSECT_INFO intersectInfo;
    PKSDATARANGE  dataRange;
    BOOL onlyWantsSize;
    ULONG formatSize;
    ULONG streamNumber;
    ULONG j;
    ULONG numberOfFormatArrayEntries;
    PKSDATAFORMAT *availableFormats;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    

    intersectInfo = Srb->CommandData.IntersectInfo;
    streamNumber = intersectInfo->StreamNumber;
    dataRange = intersectInfo->DataRange;

    //
    // Check that the stream number is valid
    //

    INTELCAM_KdPrint (MAX_TRACE, ("'AdapterFormatFromRange: Stream=%d\n", 
            streamNumber));

    ASSERT(streamNumber == 0);
    
    numberOfFormatArrayEntries = 
            Streams[0].hwStreamInfo.NumberOfFormatArrayEntries;

    //
    // Get the pointer to the array of available formats
    //

    availableFormats =
		Streams[0].hwStreamInfo.StreamFormatsArray;

    //
    // Is the caller trying to get the format, or the size of the format?
    //

    onlyWantsSize =
	    (intersectInfo->SizeOfDataFormatBuffer == sizeof(ULONG));

    //
    // Walk the formats supported by the stream searching for a match
    // of the three GUIDs which together define a DATARANGE
    //

    for (j = 0; j < numberOfFormatArrayEntries; j++, availableFormats++) {

        INTELCAM_KdPrint (MAX_TRACE, ("'checking format %d\n", 
            j));


        if (!AdapterCompareGUIDsAndFormatSize(dataRange,
		                                      *availableFormats)) {
            // not the format we want
            continue;
        }

        //
        // Now that the three GUIDs match, switch on the Specifier
        // to do a further type specific check
        //

        // -------------------------------------------------------------------
        // Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
        // -------------------------------------------------------------------

        if (IsEqualGUID (&dataRange->Specifier, 
                         &KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {

            PKS_DATARANGE_VIDEO dataRangeVideoToVerify = 
                    (PKS_DATARANGE_VIDEO) dataRange;
            PKS_DATARANGE_VIDEO dataRangeVideo = 
                    (PKS_DATARANGE_VIDEO) *availableFormats;
            PKS_DATAFORMAT_VIDEOINFOHEADER DataFormatVideoInfoHeaderOut;

            
            INTELCAM_KdPrint (MAX_TRACE, ("'guid matched\n")); 
            
            //
            // Check that the other fields match
            //
            if ((dataRangeVideoToVerify->bFixedSizeSamples !=
			                 dataRangeVideo->bFixedSizeSamples) ||
                (dataRangeVideoToVerify->bTemporalCompression !=
				             dataRangeVideo->bTemporalCompression) ||
                (dataRangeVideoToVerify->StreamDescriptionFlags !=
				             dataRangeVideo->StreamDescriptionFlags) ||
                (dataRangeVideoToVerify->MemoryAllocationFlags !=
				             dataRangeVideo->MemoryAllocationFlags) ||
                (RtlCompareMemory (&dataRangeVideoToVerify->ConfigCaps,
                        &dataRangeVideo->ConfigCaps,
                        sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)) != 
                        sizeof (KS_VIDEO_STREAM_CONFIG_CAPS))) {
                // not the format want                        
                INTELCAM_KdPrint (MAX_TRACE, ("'format does not match\n")); 
                continue;
            }

            INTELCAM_KdPrint (MAX_TRACE, ("'match found\n"));                 
            formatSize = sizeof (KSDATAFORMAT) + 
                KS_SIZE_VIDEOHEADER (&dataRangeVideoToVerify->
    				                        VideoInfoHeader);

            if (onlyWantsSize) {
                break;
            }
            
            // Caller wants the full data format, make sure we have room
            if (intersectInfo->SizeOfDataFormatBuffer < formatSize) {
                Srb->Status = ntStatus = STATUS_BUFFER_TOO_SMALL;
                INTELCAM_KdPrint (MAX_TRACE, ("'formatsize too small\n")); 
                break;
            }

            DataFormatVideoInfoHeaderOut = (PKS_DATAFORMAT_VIDEOINFOHEADER) intersectInfo->DataFormatBuffer;


            // Copy over the KSDATAFORMAT, followed by the 
            // actual VideoInfoHeader
                
            RtlCopyMemory(
                &DataFormatVideoInfoHeaderOut->DataFormat,
                &dataRangeVideoToVerify->DataRange,
                sizeof (KSDATARANGE));

            DataFormatVideoInfoHeaderOut->DataFormat.FormatSize = formatSize;

            RtlCopyMemory(
                &DataFormatVideoInfoHeaderOut->VideoInfoHeader,
                &dataRangeVideoToVerify->VideoInfoHeader,
                KS_SIZE_VIDEOHEADER (&dataRangeVideoToVerify->VideoInfoHeader));

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

        } else {
            Srb->Status = ntStatus = STATUS_NO_MATCH;
            INTELCAM_KdPrint (MAX_TRACE, ("'no match\n"));
            break;
        }

    } // End of loop on all formats for this stream

    if (NT_SUCCESS(ntStatus)) {
        if (onlyWantsSize) {
            *(PULONG) intersectInfo->DataFormatBuffer = formatSize;
            Srb->ActualBytesTransferred = sizeof(ULONG);
        } else {       
            Srb->ActualBytesTransferred = formatSize;
        }
    }    

    INTELCAM_KdPrint (MAX_TRACE, ("'AdapterFormatFromRange ret %x\n", ntStatus));
    
    return ntStatus;
}


/*
** AdapterVerifyFormat()
**
**   Checks the validity of a format request by walking through the
**       array of supported KSDATA_RANGEs for a given stream.
**
** Arguments:
**
**   pKSDataFormat - pointer of a KS_DATAFORMAT_VIDEOINFOHEADER structure.
**   StreamNumber - index of the stream being queried / opened.
**
** Returns:
** 
**   TRUE if the format is supported
**   FALSE if the format cannot be suppored
**
** Side Effects:  none
*/

BOOL 
AdapterVerifyFormat(
    PKS_DATAFORMAT_VIDEOINFOHEADER pKSDataFormatToVerify, 
    int StreamNumber
    )
{
    PKS_VIDEOINFOHEADER                 pVideoInfoHdrToVerify = 
                &pKSDataFormatToVerify->VideoInfoHeader;
    PKS_VIDEOINFOHEADER                 pVideoInfoHdr;
    PKSDATAFORMAT                       *pAvailableFormats;
    PKS_DATARANGE_VIDEO                 pKSDataRange;
    KS_VIDEO_STREAM_CONFIG_CAPS         *pConfigCaps;
    int                                 NumberOfFormatArrayEntries;
    int                                 nSize;
    int                                 j;
    RECT                                rcImage;
     
    //
    // Make sure the stream index is valid
    //
    if (StreamNumber >= 2 || StreamNumber < 0) {
        return FALSE;
    }

    //
    // How many formats does this stream support?
    //
    NumberOfFormatArrayEntries = 
                Streams[StreamNumber].hwStreamInfo.NumberOfFormatArrayEntries;

    nSize = sizeof (KS_VIDEOINFOHEADER) + 
                pVideoInfoHdrToVerify->bmiHeader.biSize;

    INTELCAM_KdPrint (MAX_TRACE, ("'AdapterVerifyFormat: Stream=%d\n", 
            StreamNumber));

    INTELCAM_KdPrint (MAX_TRACE, ("'AdapterVerifyFormat: FormatSize=%d\n", 
            pKSDataFormatToVerify->DataFormat.FormatSize));

    INTELCAM_KdPrint (MAX_TRACE, ("'AdapterVerifyFormat: MajorFormat=%x\n", 
            pKSDataFormatToVerify->DataFormat.MajorFormat));

    INTELCAM_KdPrint (MAX_TRACE, ("'AdapterVerifyFormat: pVideoInfoHdrToVerify=%x\n", 
                pVideoInfoHdrToVerify));

    INTELCAM_KdPrint (MAX_TRACE, ("'AdapterVerifyFormat: KS_VIDEOINFOHEADER size =%d\n", 
                nSize));

    INTELCAM_KdPrint (MIN_TRACE, ("'AdapterVerifyFormat: Width=%d Height=%d  biBitCount=%d\n", 
                pVideoInfoHdrToVerify->bmiHeader.biWidth, 
                pVideoInfoHdrToVerify->bmiHeader.biHeight,
                pVideoInfoHdrToVerify->bmiHeader.biBitCount));

    INTELCAM_KdPrint (MAX_TRACE, ("'AdapterVerifyFormat: biSizeImage =%d\n", 
                pVideoInfoHdrToVerify->bmiHeader.biSizeImage));

    //
    // Get the pointer to the array of available formats
    //
    pAvailableFormats = Streams[StreamNumber].hwStreamInfo.StreamFormatsArray;

    //
    // Walk the array, searching for a match
    //
    for (j = 0; j < NumberOfFormatArrayEntries; j++, pAvailableFormats++)
	{
        pKSDataRange = (PKS_DATARANGE_VIDEO) *pAvailableFormats;
        pVideoInfoHdr = &pKSDataRange->VideoInfoHeader;
        pConfigCaps = &pKSDataRange->ConfigCaps;
        
        //
        // Check for matching size, Major Type, Sub Type, and Specifier
        //

        if (!IsEqualGUID (&pKSDataRange->DataRange.MajorFormat, 
            &pKSDataFormatToVerify->DataFormat.MajorFormat))
		{
            continue;
        }

        if (!IsEqualGUID (&pKSDataRange->DataRange.SubFormat, 
            &pKSDataFormatToVerify->DataFormat.SubFormat))
		{
            continue;
        }

        if (!IsEqualGUID (&pKSDataRange->DataRange.Specifier,
            &pKSDataFormatToVerify->DataFormat.Specifier))
		{
            continue;
        }

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
             pVideoInfoHdrToVerify->rcTarget.top <= 0))
		{
            
             rcImage.left = rcImage.top = 0;
             rcImage.right = pVideoInfoHdrToVerify->bmiHeader.biWidth;
             rcImage.bottom = pVideoInfoHdrToVerify->bmiHeader.biHeight;
        }
        else
		{
             rcImage = pVideoInfoHdrToVerify->rcTarget;
        }

        if ( (pVideoInfoHdrToVerify->bmiHeader.biWidth != pVideoInfoHdr->bmiHeader.biWidth ) ||
            (pVideoInfoHdrToVerify->bmiHeader.biHeight != pVideoInfoHdr->bmiHeader.biHeight )) {
            continue;
        }

        if ( pVideoInfoHdrToVerify->bmiHeader.biSizeImage != pVideoInfoHdr->bmiHeader.biSizeImage) {
                INTELCAM_KdPrint (MIN_TRACE, ("***Error**:Format mismatch Width=%d Height=%d  image size=%d\n", 
                pVideoInfoHdrToVerify->bmiHeader.biWidth, 
                pVideoInfoHdrToVerify->bmiHeader.biHeight,
                pVideoInfoHdrToVerify->bmiHeader.biSizeImage));

            continue;
        }

        //
        // HOORAY, the format passed all of the tests, so we support it
        //

        return TRUE;
    } 

    //
    // The format requested didn't match any of our listed ranges,
    // so refuse the connection.
    //

    return FALSE;
}


//
// hooks for stream SRBs
//

VOID STREAMAPI
INTELCAM_ReceiveDataPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb,
    IN PVOID DeviceContext,
    IN PBOOLEAN Completed
    )
{
     INTELCAM_KdPrint (MAX_TRACE, ("'INTELCAM_ReceiveDataPacket\n"));
} 


VOID STREAMAPI
INTELCAM_ReceiveCtrlPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb,
    IN PVOID DeviceContext,
    IN PBOOLEAN Completed
    )
{

	
    //
    //  *** WARNING ***
	//  Make sure not to call StreamClassStreamNotification() since 
	//  USBCAMD still need to get informed of Format change if accepted by Cam
	//	driver.
    //	*** WARNING ***
	//

    INTELCAM_KdPrint (MAX_TRACE, ("'INTELCAM: Receive Ctrl SRB  %x\n", Srb->Command));
	
	*Completed = TRUE; 
    Srb->Status = STATUS_SUCCESS;

    switch (Srb->Command)
    {

	case SRB_PROPOSE_DATA_FORMAT:
		INTELCAM_KdPrint(MIN_TRACE, ("'Receiving SRB_PROPOSE_DATA_FORMAT  SRB  \n"));
		if (!(AdapterVerifyFormat (
				(PKS_DATAFORMAT_VIDEOINFOHEADER)Srb->CommandData.OpenFormat, 
				Srb->StreamObject->StreamNumber))) {
			Srb->Status = STATUS_NO_MATCH;
			INTELCAM_KdPrint(MIN_TRACE,("SRB_PROPOSE_DATA_FORMAT FAILED\n"));
		}
		break;

	case SRB_SET_DATA_FORMAT:
		INTELCAM_KdPrint(MIN_TRACE, ("'SRB_SET_DATA_FORMAT\n"));

		if (!(AdapterVerifyFormat (
				(PKS_DATAFORMAT_VIDEOINFOHEADER)Srb->CommandData.OpenFormat, 
				Srb->StreamObject->StreamNumber))) {
			Srb->Status = STATUS_NO_MATCH;
			INTELCAM_KdPrint(MIN_TRACE,(" SRB_SET_DATA_FORMAT FAILED\n"));
		} 

		break;

	case SRB_GET_DATA_FORMAT:
		INTELCAM_KdPrint(MIN_TRACE, ("' SRB_GET_DATA_FORMAT\n"));
		Srb->Status = STATUS_NOT_IMPLEMENTED;
		break;


	case SRB_SET_STREAM_STATE:

	case SRB_GET_STREAM_STATE:

	case SRB_GET_STREAM_PROPERTY:

	case SRB_SET_STREAM_PROPERTY:

	case SRB_INDICATE_MASTER_CLOCK:

	default:

 		*Completed = FALSE; // let USBCAMD handle these control SRBs
    }
} 


// 
// Describe the camera
//

USBCAMD_DEVICE_DATA INTELCAM_DeviceData = {
    0,

    INTELCAM_Initialize,
    INTELCAM_UnInitialize,
    INTELCAM_ProcessUSBPacket,
    INTELCAM_NewFrame,
    INTELCAM_ProcessRawVideoFrame,
    INTELCAM_StartVideoCapture,
    INTELCAM_StopVideoCapture,
    INTELCAM_Configure,
    INTELCAM_SaveState,
    INTELCAM_RestoreState,
    INTELCAM_AllocateBandwidth,
    INTELCAM_FreeBandwidth
    };


VOID 
INTELCAM_AdapterReceivePacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext;
    PHW_STREAM_INFORMATION streamInformation =
        &(Srb->CommandData.StreamBuffer->StreamInfo);
    PHW_STREAM_HEADER streamHeader =
        &(Srb->CommandData.StreamBuffer->StreamHeader);        
    PDEVICE_OBJECT deviceObject;        
    
    
    INTELCAM_KdPrint (MAX_TRACE, ("'AdapterReceivePacket\n"));
     
    switch (Srb->Command)
    {

    case SRB_GET_STREAM_INFO:
        //
        // this is a request for the driver to enumerate requested streams
        //
        INTELCAM_KdPrint (MAX_TRACE, ("'SRB_GET_STREAM_INFO\n"));

        //
        // we support one stream
        //
        
        ASSERT (Srb->NumberOfBytesToTransfer >=
              sizeof (HW_STREAM_HEADER) +
			             sizeof (HW_STREAM_INFORMATION) * DRIVER_STREAM_COUNT);

		streamHeader->NumberOfStreams = DRIVER_STREAM_COUNT;
        
        streamInformation->StreamFormatsArray = &INTELCAM_StreamFormats[0];
        streamInformation->NumberOfFormatArrayEntries =
		                                       NUM_INTELCAM_STREAM_FORMATS;

        //
        // set the property information for the video stream
        //

        streamHeader->DevicePropertiesArray =
            INTELCAM_GetAdapterPropertyTable(
			    &streamHeader->NumDevPropArrayEntries); 

        // pass to usbcamd to finish the job
        deviceContext =     
            USBCAMD_AdapterReceivePacket(Srb,
			                             &INTELCAM_DeviceData,
										 NULL,
										 TRUE);
            
        ASSERT_DEVICE_CONTEXT(deviceContext); 
        break;

    case SRB_GET_DEVICE_PROPERTY:
        //
        // we handle all the property stuff 
        //
        INTELCAM_KdPrint (MAX_TRACE, ("'SRB_GET_DEVICE_PROPERTY\n"));
        
        deviceContext =     
            USBCAMD_AdapterReceivePacket(Srb,
			                             &INTELCAM_DeviceData,
			                             &deviceObject,
										 FALSE);   
        ASSERT_DEVICE_CONTEXT(deviceContext);  
        
        INTELCAM_KdPrint (MAX_TRACE, ("'SRB_GET_STREAM_INFO\n"));
        INTELCAM_PropertyRequest(FALSE,
                                 deviceObject,
                                 deviceContext,
                                 Srb);

        StreamClassDeviceNotification(DeviceRequestComplete,
                                      Srb->HwDeviceExtension,
                                      Srb);
        break;            
            
    case SRB_SET_DEVICE_PROPERTY:
        //
        // we handle all the property stuff 
        //
        INTELCAM_KdPrint (MAX_TRACE, ("'SRB_SET_DEVICE_PROPERTY\n"));
        
        deviceContext =     
            USBCAMD_AdapterReceivePacket(Srb,
			                             &INTELCAM_DeviceData,
										 &deviceObject,
										 FALSE);   
        ASSERT_DEVICE_CONTEXT(deviceContext);  
        
        INTELCAM_KdPrint (MAX_TRACE, ("'SRB_GET_STREAM_INFO\n"));
        INTELCAM_PropertyRequest(
            TRUE,
            deviceObject,
            deviceContext,
            Srb);

        StreamClassDeviceNotification(DeviceRequestComplete,
                                      Srb->HwDeviceExtension,
                                      Srb);
        break;

    case SRB_OPEN_STREAM:        
        {
        PKS_DATAFORMAT_VIDEOINFOHEADER  pKSDataFormat = 
                (PKS_DATAFORMAT_VIDEOINFOHEADER) Srb->CommandData.OpenFormat;
        PKS_VIDEOINFOHEADER  pVideoInfoHdrRequested = 
                &pKSDataFormat->VideoInfoHeader;

        // pass to usbcamd to finish the job
        Srb->StreamObject->ReceiveDataPacket =
		                       (PVOID) INTELCAM_ReceiveDataPacket;
        Srb->StreamObject->ReceiveControlPacket =
		                       (PVOID) INTELCAM_ReceiveCtrlPacket;

  
        if (AdapterVerifyFormat(pKSDataFormat, 
                                Srb->StreamObject->StreamNumber))
		{
			deviceContext =     
				USBCAMD_AdapterReceivePacket(Srb,
			                             &INTELCAM_DeviceData,
										 NULL,
										 TRUE);

			deviceContext->StreamOpen = TRUE;
		}
		else
		{
            Srb->Status = STATUS_INVALID_PARAMETER;
            StreamClassDeviceNotification(DeviceRequestComplete,
                                      Srb->HwDeviceExtension,
                                      Srb);
        }
        }
        break;                    
        
	case SRB_GET_DATA_INTERSECTION:
		//
		// Return a format, given a range
		//
        //deviceContext =     
        //    USBCAMD_AdapterReceivePacket(Srb,
		//	                             &INTELCAM_DeviceData,
		//	                             &deviceObject,
		//								 FALSE);   

		Srb->Status = AdapterFormatFromRange(Srb);
		
        StreamClassDeviceNotification(DeviceRequestComplete,
                                      Srb->HwDeviceExtension,
                                      Srb);
		break;


	default:
        //
        // let usbcamd handle it
        //
    
        deviceContext =     
            USBCAMD_AdapterReceivePacket(Srb,
			                             &INTELCAM_DeviceData,
										 NULL,
										 TRUE);   
        ASSERT_DEVICE_CONTEXT(deviceContext);  
		break;
	}
}




/*
** DriverEntry()
**
** This routine is called when the mini driver is first loaded.  The driver
** should then call the StreamClassRegisterAdapter function to register with
** the stream class driver
**
** Arguments:
**
**  Context1:  The context arguments are private plug and play structures
**             used by the stream class driver to find the resources for this
**             adapter
**  Context2:
**
** Returns:
**
** This routine returns an NT_STATUS value indicating the result of the
** registration attempt. If a value other than STATUS_SUCCESS is returned, the
** minidriver will be unloaded.
**
** Side Effects:  none
*/

ULONG
DriverEntry(
    PVOID Context1,
    PVOID Context2
    )
{

    INTELCAM_KdPrint (MAX_TRACE, ("'Driver Entry\n")); 


    return USBCAMD_DriverEntry(Context1, 
                               Context2, 
                               sizeof(INTELCAM_DEVICE_CONTEXT),
                               sizeof(INTELCAM_FRAME_CONTEXT),
                               INTELCAM_AdapterReceivePacket);
}



/*
** INTELCAM_Initialize()
**
** On entry the device has been configured and the initial alt
** interface selected -- this is where we may send additional 
** vendor commands to enable the device.
**
** Arguments:
**
** BusDeviceObject - pdo associated with this device
**
** DeviceContext - driver specific context
**
** Returns:
**
** NTSTATUS code
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_Initialize(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext;
    LARGE_INTEGER dueTime;
    ULONG wait = 0;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    USHORT powerState;
    ULONG length;

    deviceContext = DeviceContext;

    ASSERT_DEVICE_CONTEXT(deviceContext);
    //
    // perform any hardware specific 
    // initialization
    //

    INTELCAM_KdPrint (MAX_TRACE, ("'power up the camera\n"));

#ifdef WAIT_CAMERA_INIT

    // wait 10 seconds for the beta camera

    dueTime.QuadPart = -10000 * 10000;

    (VOID) KeDelayExecutionThread(KernelMode,
                                  FALSE,
                                  &dueTime);
#else

    //
    // NOTE
    // This only works on the production level cameras.
    //

    do {

        dueTime.QuadPart = -10000 * 1000;

        (VOID) KeDelayExecutionThread(KernelMode,
                                      FALSE,
                                      &dueTime);
        length = sizeof(powerState);
        
        ntStatus = USBCAMD_ControlVendorCommand(DeviceContext,
                                         REQUEST_GET_PREFERENCE,
                                         0,
                                         INDEX_PREF_POWER,
                                         &powerState,
                                         &length,
                                         TRUE,
										 NULL,
										 NULL);

        ILOGENTRY("PWck", wait, powerState, ntStatus);

        INTELCAM_KdPrint (MAX_TRACE, ("'get power state power = 0x%x, ntStatus %x\n",
            powerState, ntStatus));

        // timeout ater ~10 seconds
        wait++;
        if (wait > 9 ) {
            ILOGENTRY("HWto", wait, 0, ntStatus);
            TEST_TRAP();
            ntStatus = STATUS_UNSUCCESSFUL;
        }

    } while (NT_SUCCESS(ntStatus) && powerState < 3);

#endif /* WAIT_CAMERA_INIT */


    //
    // initialize the camera to default settings
    //

    if (NT_SUCCESS(ntStatus)) {
        ntStatus = USBCAMD_ControlVendorCommand(
                                DeviceContext,
                                REQUEST_SET_PREFERENCE,
                                0,
                                INDEX_PREF_INITIALIZE,
                                NULL,
                                NULL,
                                FALSE,
								NULL,
								NULL);
    }

    
    deviceContext->StreamOpen = FALSE;
	
    INTELCAM_KdPrint (MAX_TRACE, ("'InitializeHardware 0x%x\n", ntStatus));

    ILOGENTRY("inHW", 0, 0, ntStatus);
    
    return ntStatus;
}


/*
** INTELCAM_UnInitialize()
**
** Assume the device hardware is gone -- all that needs to be done is to 
** free any allocated resources (like memory).
**
** Arguments:
**
** BusDeviceObject - pdo associated with this device
**
** DeviceContext - driver specific context
**
** Returns:
**
** NTSTATUS code
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_UnInitialize(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext;
    NTSTATUS ntStatus = STATUS_SUCCESS;

    deviceContext = DeviceContext;
    ASSERT_DEVICE_CONTEXT(deviceContext);

    ExFreePool(deviceContext->Interface);

    INTELCAM_KdPrint (MAX_TRACE, ("'UnInitialize 0x%x\n", ntStatus));
    
    return ntStatus;
}


/*
** INTELCAM_Configure()
**
** Configure the iso streaming Interface:
**
** Called just before the device is configured, this is where we tell
** usbcamd which interface and alternate setting to use for the idle state.
**
** NOTE: The assumption here is that the device will have a single interface 
**  with multiple alt settings and each alt setting has the same number of 
**  pipes.
**
** Arguments:
**
**  BusDeviceObject - device object created by the hub whe can submit
**                  urbs to our device through this deviceObject
**
**  DeviceContext - minidriver device  context
**
**  Interface - USBD interface structure initialized with the proper values
**              for select_configuration. This Interface structure corresponds 
**              a single iso interafce on the device.  This is the drivers 
**              chance to pick a particular alternate setting and pipe 
**              parameters.
**
**
**  ConfigurationDescriptor - USB configuration Descriptor for
**      this device.
**
** Returns:
**
**  NTSTATUS code
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_Configure(
    IN PDEVICE_OBJECT BusDeviceObject,
    IN PVOID DeviceContext,
    IN OUT PUSBD_INTERFACE_INFORMATION Interface,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    IN OUT PLONG DataPipeIndex,
    IN OUT PLONG SyncPipeIndex
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext;
    NTSTATUS ntStatus = STATUS_SUCCESS;

    deviceContext = DeviceContext;

    deviceContext->Sig = INTELCAM_DEVICE_SIG;
    deviceContext->DefaultFrameRate = INTELCAM_DEFAULT_FRAME_RATE;

    //
    // initilialize any other context stuff
    //

    deviceContext->Interface = ExAllocatePool(NonPagedPool,
                                              Interface->Length);

    *DataPipeIndex = 1;
    *SyncPipeIndex = 0;
    
    if (deviceContext->Interface) {
    
        Interface->AlternateSetting = INTELCAM_IDLE_ALT_SETTING;

        // This interface has two pipes,
        // initialize input parameters to USBD for both pipes.
        // The MaximumTransferSize is the size of the largest
        // buffer we want to submit for an aingle iso urb
        // request.
        //
        Interface->Pipes[INTELCAM_SYNC_PIPE].MaximumTransferSize =
            USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
        Interface->Pipes[INTELCAM_DATA_PIPE].MaximumTransferSize =
            1024*32;       // 32k transfer per urb

        RtlCopyMemory(deviceContext->Interface,
                      Interface,
                      Interface->Length);                 

        INTELCAM_KdPrint (MAX_TRACE, ("''size of interface request = %d\n", Interface->Length));

                
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    

    //
    // return interface number and alternate setting
    //
   
    INTELCAM_KdPrint (MAX_TRACE, ("''Configure 0x%x\n", ntStatus));

    return ntStatus;
}



/*
** INTELCAM_StartVideoCapture()
**
** Arguments:
**
**  BusDeviceObject - device object created by the hub we can submit
**                  urbs to our device through this deviceObject
**
**  DeviceContext - minidriver device  context
**
** Returns:
**
** NTSTATUS code
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_StartVideoCapture(
    IN PDEVICE_OBJECT BusDeviceObject,
    IN PVOID DeviceContext
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;
    NTSTATUS ntStatus;
    
    ASSERT_DEVICE_CONTEXT(deviceContext);
    
    //
    // This is where we select the interface we need and send
    // commands to start capturing
    //
    INTELCAM_KdPrint (MAX_TRACE, ("'starting capture \n"));

    ntStatus = USBCAMD_ControlVendorCommand(DeviceContext,
                                            REQUEST_SET_PREFERENCE,
                                            1,
                                            INDEX_PREF_CAPMOTION,
                                            NULL,
                                            NULL,
                                            FALSE,
											NULL,
											NULL);
    return ntStatus;        
}

/*
** INTELCAM_AllocateBandwidth()
**
** Called just before the iso video capture stream is
** started, here is where we select the appropriate 
** alternate interface and set up the device to stream.
**
**  Called in connection with the stream class RUN command
**
** Arguments:
**
**  BusDeviceObject - device object created by the hub we can submit
**                  urbs to our device through this deviceObject
**
**  DeviceContext - minidriver device  context
**
**  RawFrameLength - pointer to be filled in with size of buffer needed to 
**                  receive the raw frame data from the packet stream.
**
**  Format - pointer to PKS_DATAFORMAT_VIDEOINFOHEADER associated with this 
**          stream.          
**
** Returns:
**
** NTSTATUS code
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_AllocateBandwidth(
    IN PDEVICE_OBJECT BusDeviceObject,
    IN PVOID DeviceContext,
    OUT PULONG RawFrameLength,
    IN PVOID Format            // BUGBUG
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;
    ULONG PixelCount, bytesInImage, maxDataPacketSize;
    ULONG AvgTimePerFrame, FrameRate,MicroSecPerFrame;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PKS_DATAFORMAT_VIDEOINFOHEADER dataFormatHeader;
    PKS_BITMAPINFOHEADER bmInfoHeader;
    
    USHORT hwSetting;
    
    ASSERT_DEVICE_CONTEXT(deviceContext);
    
    //
    // This is where we select the interface we need and send
    // commands to start capturing
    //
    INTELCAM_KdPrint (MAX_TRACE, ("'starting capture %x\n", Format));

    ntStatus = INTELCAM_ReadRegistry(BusDeviceObject,
                                     DeviceContext);

    *RawFrameLength = 0;
    dataFormatHeader = Format;
    bmInfoHeader = &dataFormatHeader->VideoInfoHeader.bmiHeader;

    AvgTimePerFrame = (ULONG) dataFormatHeader->VideoInfoHeader.AvgTimePerFrame; // 100NS units
    MicroSecPerFrame = AvgTimePerFrame/10; 

    if ( MicroSecPerFrame == 0 ) {
        FrameRate = 0;
    }
    else {
        FrameRate =  (1000000/MicroSecPerFrame);
    }
    
    // special case to handle 0 < fps < 1.

    if (AvgTimePerFrame > 10000000 ) {
        FrameRate = 1;
    }
 

    //
    // BUGBUG sanity check the format
    //      NOTE: we could look at the haeader here to determine
    //            frame rate to use.

    if (NT_SUCCESS(ntStatus)) {

        INTELCAM_KdPrint (MAX_TRACE, ("'format %d x %d\n", bmInfoHeader->biWidth,
                            bmInfoHeader->biHeight));
    
        PixelCount = bmInfoHeader->biWidth * bmInfoHeader->biHeight;

        bytesInImage = (bmInfoHeader->biWidth * bmInfoHeader->biHeight * 12)/8;
        
        deviceContext->StartOffset_y = 0;
        deviceContext->StartOffset_u = PixelCount;
        deviceContext->StartOffset_v = PixelCount + PixelCount/4;        

		//
		// ??
		//
        if (bmInfoHeader->biWidth == 176)
		{
            hwSetting = 0x4;
        }
        else if (bmInfoHeader->biWidth == 160)
		{
            hwSetting = 0xa;
        }
		else
		{
            hwSetting = 0x5;
        }

        ntStatus = USBCAMD_ControlVendorCommand(DeviceContext,
                                                REQUEST_SET_PREFERENCE,
                                                hwSetting,
                                                INDEX_PREF_SCALING,
                                                NULL,
                                                NULL,
                                                FALSE,
												NULL,
												NULL);
    }        

    if (NT_SUCCESS(ntStatus))
	{
        //
        // bugbug this is where we select the interface
        // based on requested frame rate.
        //
        if ((FrameRate > 0) && (FrameRate <= 6))
			goto FPS_6;
		else if ((FrameRate > 6) && (FrameRate <= 10))
			goto FPS_10;
		else if ((FrameRate > 10) && (FrameRate <= 12))
			goto FPS_12;
		else if ((FrameRate > 12) && (FrameRate <= 15))
			goto FPS_15;
		else if ((FrameRate > 15) && (FrameRate <= 18))
			goto FPS_18;
		else if ((FrameRate > 18) && (FrameRate <= 20))
			goto FPS_20;
		else if (FrameRate > 20)
			goto FPS_25;
		else  
			goto FPS_0;


		/*
		 * We will keep ratcheting down the frame rate until our call to get
		 * the bandwidth succeeds.
		 */

FPS_25:
		deviceContext->Interface->AlternateSetting = 6;  // 25 Fps
		ntStatus = USBCAMD_SelectAlternateInterface(deviceContext,
													deviceContext->Interface);
		if (NT_SUCCESS(ntStatus))
		{
			deviceContext->CurrentProperty.Rate =
			                    NUM_100NANOSEC_UNITS_PERFRAME(25);
			goto FrameRateSet;
		}


FPS_20:
		deviceContext->Interface->AlternateSetting = 5;  // 20 Fps
		ntStatus = USBCAMD_SelectAlternateInterface(deviceContext,
													deviceContext->Interface);

		if (NT_SUCCESS(ntStatus))
		{
			deviceContext->CurrentProperty.Rate =
			                    NUM_100NANOSEC_UNITS_PERFRAME(20);
			goto FrameRateSet;
		}



FPS_18:
		deviceContext->Interface->AlternateSetting = 4; // 18FPS
		ntStatus = USBCAMD_SelectAlternateInterface( deviceContext,
													deviceContext->Interface);
		if (NT_SUCCESS(ntStatus))
		{
			deviceContext->CurrentProperty.Rate =
			                   NUM_100NANOSEC_UNITS_PERFRAME(18);
			goto FrameRateSet;
		}


FPS_15:
		deviceContext->Interface->AlternateSetting = 2; // 15FPS
		ntStatus = USBCAMD_SelectAlternateInterface( deviceContext,
													deviceContext->Interface);

		if (NT_SUCCESS(ntStatus))
		{
			deviceContext->CurrentProperty.Rate =
			                   NUM_100NANOSEC_UNITS_PERFRAME(15);
			goto FrameRateSet;
		}


FPS_12:
		deviceContext->Interface->AlternateSetting = 3; // 12FPS
		ntStatus = USBCAMD_SelectAlternateInterface( deviceContext,
													deviceContext->Interface);

		if (NT_SUCCESS(ntStatus))
		{
			deviceContext->CurrentProperty.Rate =
			                   NUM_100NANOSEC_UNITS_PERFRAME(12);
			goto FrameRateSet;
		}


FPS_10:
		deviceContext->Interface->AlternateSetting = 0; // 10FPS
		ntStatus = USBCAMD_SelectAlternateInterface( deviceContext,
													deviceContext->Interface);

		if (NT_SUCCESS(ntStatus))
		{
			deviceContext->CurrentProperty.Rate =
			                   NUM_100NANOSEC_UNITS_PERFRAME(10);
			goto FrameRateSet;
		}


FPS_6:
		deviceContext->Interface->AlternateSetting = 1; // 6FPS
		ntStatus = USBCAMD_SelectAlternateInterface( deviceContext,
													deviceContext->Interface);

		if (NT_SUCCESS(ntStatus))
		{
			deviceContext->CurrentProperty.Rate =
			                   NUM_100NANOSEC_UNITS_PERFRAME(6);
			goto FrameRateSet;
		}

FPS_0:
        /*
		 * If we fail to set any valid frame rate, set it for zero bandwidth
		 * use
		 */
		deviceContext->Interface->AlternateSetting = 7; // 0FPS
		ntStatus = USBCAMD_SelectAlternateInterface( deviceContext,
													deviceContext->Interface);
		deviceContext->CurrentProperty.Rate =
			                   NUM_100NANOSEC_UNITS_PERFRAME(0);
		goto FrameRateSet;


    }			

FrameRateSet:
    if (NT_SUCCESS(ntStatus))
	{
    
		// ?? BUGBUG - fill in the actual rate here later

//		deviceContext->CurrentProperty.Rate = MicroSecPerFrame;

        maxDataPacketSize = 
            deviceContext->Interface->Pipes[INTELCAM_DATA_PIPE].MaximumPacketSize;            
        
        if (maxDataPacketSize != 0)
		{
			*RawFrameLength = ( (bytesInImage / maxDataPacketSize) + 
							   ((bytesInImage % maxDataPacketSize) ? 1:0)) * maxDataPacketSize;
		}
                
		//
		// Set the camera controls to desired values
		//
		deviceContext->FirstTime = TRUE;
		ntStatus = INTELCAM_CameraToDriverDefaults(BusDeviceObject,
												   deviceContext);
		deviceContext->FirstTime = FALSE;
		deviceContext->IsVideoFrameGood = TRUE;
        deviceContext->BadPackets = 0;      

    }

    return ntStatus;        
}

/*
** INTELCAM_FreeBandwidth()
**
** Called after the iso video stream is stopped, this is where we 
** select an alternate interface that uses no bandwidth.
**
** Arguments:
**
**  BusDeviceObject - device object created by the hub we can submit
**                  urbs to our device through this deviceObject
**
**  DeviceContext - minidriver device  context
**
** Returns:
**
**  NTSTATUS code
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_FreeBandwidth(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    )
{
    NTSTATUS ntStatus;
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;

    // turn off streaming on the device

    ASSERT_DEVICE_CONTEXT(deviceContext);

    deviceContext->Interface->AlternateSetting = 
        INTELCAM_IDLE_ALT_SETTING;

    ntStatus = USBCAMD_SelectAlternateInterface(
                    deviceContext,
                    deviceContext->Interface);

    return ntStatus;
}


/*
** INTELCAM_StopVideoCapture()
**
** Called after the iso video stream is stopped, this is where we 
** select an alternate interface that uses no bandwidth.
**
** Arguments:
**
**  BusDeviceObject - device object created by the hub we can submit
**                  urbs to our device through this deviceObject
**
**  DeviceContext - minidriver device  context
**
** Returns:
**
**  NTSTATUS code
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_StopVideoCapture(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    )
{
    NTSTATUS ntStatus;
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;

    // turn off streaming on the device

    ASSERT_DEVICE_CONTEXT(deviceContext);

    ntStatus = USBCAMD_ControlVendorCommand(DeviceContext,
                                            REQUEST_SET_PREFERENCE,
                                            0,
                                            INDEX_PREF_CAPMOTION,
                                            NULL,
                                            NULL,
                                            FALSE,
											NULL,
											NULL);

    ntStatus = INTELCAM_SaveControlsToRegistry(BusDeviceObject,
		                                       deviceContext);

    return ntStatus;
}


/*
** INTELCAM_NewFrame()
**
**  called at DPC level to allow driver to initialize a new video frame
**  context structure
**
** Arguments:
**
**  DeviceContext - minidriver device  context
**
**  FrameContext - frame context to be initialized
**
** Returns:
**
**  NTSTATUS code
**  
** Side Effects:  none
*/

VOID
INTELCAM_NewFrame(
    PVOID DeviceContext,
    PVOID FrameContext
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;
     
    INTELCAM_KdPrint (MAX_TRACE, ("'INTELCAM_NewFrame\n"));
    ASSERT_DEVICE_CONTEXT(deviceContext); 
}



/*
** INTELCAM_ProcessUSBPacket()
**
**  called at DPC level to allow driver to determine if this packet is part
**  of the current video frame or a new video frame.
**
**  This function should complete as quickly as possible, any image processing 
**  should be deferred to the ProcessRawFrame routine.
**
** Arguments:
**
**  BusDeviceObject - device object created by the hub we can submit
**                  urbs to our device through this deviceObject
**
**  DeviceContext - minidriver device  context
**
**  CurrentFrameContext - some context for this particular frame
**
**  SyncPacket - iso packet descriptor from sync pipe, not used if the interface
**              has only one pipe.
**
**  SyncBuffer - pointer to data for the sync packet
**
**  DataPacket - iso packet descriptor from data pipe
**
**  DataBuffer - pointer to data for the data packet
**
**  FrameComplete - indicates to usbcamd that this is the first data packet 
**          for a new video frame 
**
** Returns:
**  
** number of bytes that should be copied in to the rawFrameBuffer of FrameBuffer.
**
** Side Effects:  none
*/


ULONG
INTELCAM_ProcessUSBPacket(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext,
    PVOID CurrentFrameContext,
    PUSBD_ISO_PACKET_DESCRIPTOR SyncPacket,
    PVOID SyncBuffer,
    PUSBD_ISO_PACKET_DESCRIPTOR DataPacket,
    PVOID DataBuffer,
    PBOOLEAN FrameComplete,
    PBOOLEAN NextFrameIsStill
    )
{
    UCHAR syncByte;
    ULONG length = 0, maxDataPacketSize;
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;

    INTELCAM_KdPrint (MAX_TRACE, ("'INTELCAM_ProcessUSBPacket\n"));

    *NextFrameIsStill = FALSE;
    ASSERT_DEVICE_CONTEXT(deviceContext);

    maxDataPacketSize = 
        deviceContext->Interface->Pipes[INTELCAM_DATA_PIPE].MaximumPacketSize; 

    syncByte = * ((PUCHAR) SyncBuffer);

    if (!USBD_SUCCESS(
        SyncPacket->Status)) {

        //
        // if we get an error on the sync frame process the data packet 
        // anyway so that our buffer pointers will be updated
        //
        
        syncByte = 0;
        
    }                
    
    if (syncByte & USBCAMD_SOV) {
        
        //                       
        // start of new video frame
        //

        *FrameComplete = TRUE;
        INTELCAM_KdPrint (MIN_TRACE, ("'Start of New Frame #%d\n",syncByte & 0x7f)); 

        //check if existing frame has bad packets.

        if ( deviceContext->BadPackets > 0 ) {
            // if so, set the flag.
            deviceContext->IsVideoFrameGood = FALSE;
            // and reset the bad pckt counter.
            deviceContext->BadPackets = 0;
        }
        else {
            deviceContext->IsVideoFrameGood = TRUE;
        }

		if ((DataPacket->Length == 0) || (DataPacket->Length != maxDataPacketSize)) {
			INTELCAM_KdPrint (MIN_TRACE, ("**Error** packet length = %d,  Expected Pkt Len= %d\n", 
				DataPacket->Length, maxDataPacketSize));
				deviceContext->BadPackets++;
			length= 0;
		}
		else {
			length = maxDataPacketSize;
			
		}

     } else {
     
        if (syncByte & USBCAMD_ERROR) {
        
            //                    
            // error reported for this packet, I beleive we are 
            // supposed to ignore the packet.
            //
#if DBG
            deviceContext->IgnorePacketCount++;  
#endif                
//            ILOGENTRY("igPe", FrameContext, 0, 0);
            
        } else if (syncByte & USBCAMD_I) {
        
            //
            // interfield data reported for this packet, I beleive we are 
            // supposed to ignore the packet.
            //
#if DBG
            deviceContext->IgnorePacketCount++;  
#endif                
//            ILOGENTRY("igPi", FrameContext, 0, 0);

        } else {
        
            //
            // copy the packet to the raw frame buffer
            //
			// if the packet length is 0, return 0 bytes.
			if ((DataPacket->Length == 0) || (DataPacket->Length != maxDataPacketSize)) {
				INTELCAM_KdPrint (MIN_TRACE, ("**Error** packet length = %d,  Expected Pkt Len= %d\n", 
					DataPacket->Length, maxDataPacketSize));
				deviceContext->BadPackets++;
				length= 0;
			}
			else {
				length = maxDataPacketSize;
			}
        }
        
    }   

    if (!USBD_SUCCESS(SyncPacket->Status) ||
	    !USBD_SUCCESS(DataPacket->Status))
	{
				deviceContext->BadPackets++;
	}

    return length;
}




/*
** INTELCAM_ProcessRawVideoFrame()
**
**  Called at PASSIVE level to allow driver to perform any decoding of the 
**  raw video frame.
**
**    This routine will convert the packetized data in to the fromat 
**    the CODEC expects, ie y,u,v
**
**    data is always of the form 256y 64u 64v (384 byte chunks) regardless of USB
**    packet size.
**
**
** Arguments:
**
**  DeviceContext - driver context
**
**  FrameContext - driver context for this frame
**
**  FrameBuffer - pointer to the buffer that should receive the final 
**              processed video frame.
**
**  FrameLength - length of the Frame buffer (from the original read 
**                  request)
**
**  RawFrameBuffer - pointer to buffer containing the received USB packets
** 
**  RawFrameLength - length of the raw frame.
**
**  NumberOfPackets - number of USB packets received in to the RawFrameBuffer
**
**  BytesReturned - pointer to value to return for number of bytes read.
**              
** Returns:
**
**  NT status completion code for the read irp
**  
** Side Effects:  none
*/

NTSTATUS
INTELCAM_ProcessRawVideoFrame(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext,
    PVOID FrameContext,
    PVOID FrameBuffer,
    ULONG FrameLength,
    PVOID RawFrameBuffer,
    ULONG RawFrameLength,
    ULONG NumberOfPackets,
    PULONG BytesReturned
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;
    PINTELCAM_FRAME_CONTEXT frameContext = FrameContext;
    ULONG i, rawDataLength, processedDataLength, maxDataPacketSize;
    PUCHAR frameBuffer, rawFrameBuffer, frameBufferEnd;

    //TEST_TRAP();
    ASSERT_DEVICE_CONTEXT(deviceContext);

    frameBuffer = FrameBuffer;
    rawFrameBuffer = RawFrameBuffer;
    frameBufferEnd = frameBuffer+FrameLength;
    processedDataLength = 0;        
    
	if (deviceContext->IsVideoFrameGood == FALSE){
		*BytesReturned = 0;
        return STATUS_SUCCESS;
	}
	

    ASSERT(RawFrameLength != 0);
    
    //
    // raw frame is in the format 256 bytes y 64 bytes u 64 bytes v
    //
    // we need to convert this to y u v in the frame buffer
    //

    INTELCAM_KdPrint (MAX_TRACE, ("'frameBuffer = 0x%x  max = %d numpackets = %d\n", frameBuffer,
        FrameLength, NumberOfPackets)); 

    maxDataPacketSize = 
            deviceContext->Interface->Pipes[INTELCAM_DATA_PIPE].MaximumPacketSize;            

    rawDataLength = NumberOfPackets * maxDataPacketSize;        

    INTELCAM_KdPrint (MAX_TRACE, ("'maxDataPacketSize = 0x%x  rawDataLength = 0x%x\n",
        maxDataPacketSize, rawDataLength)); 
    ILOGENTRY("prfr", 0, 0, 0);        

    INTELCAM_KdPrint (MAX_TRACE, ("'so_u = 0x%x so_v 0x%x\n",
        deviceContext->StartOffset_u, deviceContext->StartOffset_v)); 
    
    for (i=0; i<rawDataLength/384; i++) {

        //
        // copy the y
        //
        
        // check for buffer overrun
        //ASSERT(frameBuffer + (i*256) + 256 <= frameBufferEnd);

        if (i*256+256 <= deviceContext->StartOffset_u) {
            RtlCopyMemory(frameBuffer + (i*256), 
                          rawFrameBuffer,
                          256);
            processedDataLength+=256;                          
        }                          

        //
        // copy the u
        //

        // check for buffer overrun
        //ASSERT(frameBuffer + deviceContext->StartOffset_u + 
        //     (i*64) + 64 <= frameBufferEnd);

        if (i*64+deviceContext->StartOffset_u+64 <= deviceContext->StartOffset_v) {
            RtlCopyMemory(frameBuffer +
                          deviceContext->StartOffset_u + (i*64), 
                          rawFrameBuffer+256,
                          64);                        
            processedDataLength+=64;              
        }                          

        //
        // copy the v
        //

        // check for buffer overrun
        //ASSERT(frameBuffer + deviceContext->StartOffset_v + 
        //    (i*64) + 64 <= frameBufferEnd);

        if (deviceContext->StartOffset_v+(i*64)+64 <= FrameLength) {
            RtlCopyMemory(frameBuffer+
                          deviceContext->StartOffset_v + (i*64), 
                          rawFrameBuffer+256+64,
                          64);      
            processedDataLength+=64;                          
        }                          

        rawFrameBuffer+=384;
        //processedDataLength+=384;

    }

	*BytesReturned = processedDataLength;
	

    INTELCAM_KdPrint (MAX_TRACE, ("'ProcessRawVideoFrame return length = %d\n", 
        processedDataLength));

    return STATUS_SUCCESS;
    
}

/*
** INTELCAM_PropertyRequest()
**
** Arguments:
**
**  DeviceContext - driver context
**
** Returns:
**
**  NT status completion code for the read irp
**  
** Side Effects:  none
*/

NTSTATUS
INTELCAM_PropertyRequest(
    BOOLEAN SetProperty,
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext,
    PVOID PropertyContext
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PHW_STREAM_REQUEST_BLOCK srb = (PHW_STREAM_REQUEST_BLOCK)PropertyContext;
    PSTREAM_PROPERTY_DESCRIPTOR propertyDescriptor;

    propertyDescriptor = srb->CommandData.PropertyInfo;


    //
    // identify the property to set
    //
    
    if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEOPROCAMP,
        &propertyDescriptor->Property->Set)){

        if (SetProperty){
            ntStatus = INTELCAM_SetCameraProperty(DeviceContext, srb);
        } else {
            ntStatus = INTELCAM_GetCameraProperty(DeviceContext, srb);
        }
    }

    else if (IsEqualGUID(&PROPSETID_INTEL_CUSTOM_PROP,
                         &propertyDescriptor->Property->Set)) {
        if (SetProperty) {
            ntStatus = INTELCAM_SetCustomProperty(DeviceContext, srb);
        } else {
            ntStatus = INTELCAM_GetCustomProperty(DeviceContext, srb);
        }
    }
    
    else if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEOCONTROL,
                         &propertyDescriptor->Property->Set))    {
        if (SetProperty)    {
            ntStatus = INTELCAM_SetVideoControlProperty(DeviceContext, srb);
        } else {
            ntStatus = INTELCAM_GetVideoControlProperty(DeviceContext, srb);
        }
    }
    
    else {
        ntStatus = STATUS_NOT_SUPPORTED;
    }

    return ntStatus;
}

/*
** INTELCAM_SaveState()
**
** Arguments:
**
** Returns:
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_SaveState(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;
	NTSTATUS ntStatus;

    ntStatus = INTELCAM_SaveControlsToRegistry(BusDeviceObject,
		                                       deviceContext);

    return ntStatus;
}    


/*
** INTELCAM_RestoreState()
**
** Arguments:
**
** Returns:
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_RestoreState(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;
    LARGE_INTEGER dueTime;
    ULONG wait = 0;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    USHORT powerState;
    ULONG length;


    //
    // NOTE
    // This only works on the production level cameras.
    //

    do {

        dueTime.QuadPart = -10000 * 1000;

        (VOID) KeDelayExecutionThread(KernelMode,
                                      FALSE,
                                      &dueTime);
        length = sizeof(powerState);
        
        ntStatus = USBCAMD_ControlVendorCommand(DeviceContext,
                                         REQUEST_GET_PREFERENCE,
                                         0,
                                         INDEX_PREF_POWER,
                                         &powerState,
                                         &length,
                                         TRUE,
										 NULL,
										 NULL);
#if DBG

        ILOGENTRY("PWck", wait, powerState, ntStatus);

        INTELCAM_KdPrint (MAX_TRACE, ("'get power state power = 0x%x, ntStatus %x\n",
            powerState, ntStatus));
#endif
        // timeout ater ~10 seconds
        wait++;
        if (wait > 9 ) {
#if DBG
            ILOGENTRY("HWto", wait, 0, ntStatus);
            TEST_TRAP();
#endif
            ntStatus = STATUS_DEVICE_NOT_READY;
			break;
        }

    } while ( powerState < 3);


    return ntStatus;
}    


/*
** INTELCAM_ReadRegistry()
**
** Arguments:
**
** Returns:
**
** Side Effects:  none
*/

NTSTATUS
INTELCAM_ReadRegistry(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID DeviceContext
    )
{
    PINTELCAM_DEVICE_CONTEXT deviceContext = DeviceContext;
    NTSTATUS ntStatus;
    HANDLE handle;

    //keys we have
    WCHAR defaultFrameRateKey[] = L"FrameRate";

    INTELCAM_KdPrint (MAX_TRACE, ("'read registry parameters\n")); 
    
    ntStatus=IoOpenDeviceRegistryKey(BusDeviceObject,
                                     PLUGPLAY_REGKEY_DRIVER,
                                     STANDARD_RIGHTS_ALL,
                                     &handle);
                                     
    if (NT_SUCCESS(ntStatus)) {

        USBCAMD_GetRegistryKeyValue(handle,
                                 defaultFrameRateKey,
                                 sizeof(defaultFrameRateKey),
                                 &deviceContext->DefaultFrameRate,
                                 sizeof(deviceContext->DefaultFrameRate));

        ZwClose(handle);
    }

    if (deviceContext->DefaultFrameRate > INTELCAM_MAX_FRAME_RATE) {
        deviceContext->DefaultFrameRate = INTELCAM_MAX_FRAME_RATE;
    }        

    INTELCAM_KdPrint (MAX_TRACE, ("'REG: DefaultFrameRate = %d %x\n", 
        deviceContext->DefaultFrameRate, ntStatus)); 

    return ntStatus;
}    






//---------------------------------------------------------------------------
// INTELCAM_CameraToDriverDefaults
//---------------------------------------------------------------------------
NTSTATUS
INTELCAM_CameraToDriverDefaults(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID pDeviceContext
    )
/*++

Routine Description:

Arguments:

    DeviceContext - points to the driver specific DeviceContext


Return Value:

    NT status code

--*/
{
    PINTELCAM_DEVICE_CONTEXT pDC = pDeviceContext;
    HW_STREAM_REQUEST_BLOCK Srb;
    STREAM_PROPERTY_DESCRIPTOR PropertyDescriptor;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    KSPROPERTY_VIDEOPROCAMP_S Video; 
    PKSPROPERTY_VIDEOPROCAMP_S pVideo; 
    ULONG n;
    ULONG UseRegVals = 0;
    ULONG RegVals[3];
    ULONG Value;
    PULONG pRegVal;
    BOOLEAN RegistryEntryPresent = FALSE;
    WCHAR BasePath[] = L"\\Registry\\MACHINE\\SOFTWARE\\Intel\\USBCAMERA1";
	RTL_QUERY_REGISTRY_TABLE Table[3];
    UNICODE_STRING RegPath;



    //
    // Get the actual values for the controls
    //

    RtlZeroMemory (Table, sizeof(Table));
    pRegVal = RegVals;   // for convenience sake
    RegPath.Buffer = BasePath;
    RegPath.MaximumLength = sizeof(BasePath + 32); //32 chars for keys
    RegPath.Length = 0;
	

    Table[0].Name = L"Brightness";
    Table[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    Table[0].EntryContext = pRegVal++;

    //
	// Initilaize the default value to -1. if this entry is not
	// present this value will not change
	//
    RegVals[1] = 0xffffffff;
    Table[1].Name = L"ApplyRegistryValues";
    Table[1].Flags = RTL_QUERY_REGISTRY_DIRECT;
    Table[1].EntryContext = pRegVal++;
       

	ntStatus = RtlQueryRegistryValues(
    	                   RTL_REGISTRY_ABSOLUTE,
                           RegPath.Buffer,
    				       Table,
    				       NULL,
    				       NULL );


    UseRegVals = pDC->ApplyRegistryValues = RegVals[1];
    if( NT_SUCCESS(ntStatus))
        RegistryEntryPresent = TRUE;

    pVideo = &Video;
    for(n = 0; n < sizeof(KSPROPERTY_VIDEOPROCAMP_S); n++)
	{
        ((BYTE*)pVideo)[n] = 0;
    }


    //
	// Fill in Property Descriptor field
	//
    Srb.CommandData.PropertyInfo = &PropertyDescriptor;
	PropertyDescriptor.Property = &Video.Property;
	PropertyDescriptor.PropertyInfo = &Video;
	PropertyDescriptor.PropertyInputSize = sizeof(KSPROPERTY_VIDEOPROCAMP_S);
	PropertyDescriptor.PropertyOutputSize = sizeof(KSPROPERTY_VIDEOPROCAMP_S);
	

	Video.Property.Set = PROPSETID_VIDCAP_VIDEOPROCAMP;

    // set brightness
	Video.Property.Id = KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS;
    if ( (RegistryEntryPresent == TRUE) &&
	     (UseRegVals == 1) &&
	     ((RegVals[0] >=
		      ((ULONG)_Brightness.Min * STEPPING_DELTA_BRIGHTNESS)) &&
          (RegVals[0] <=
		      ((ULONG)_Brightness.Max * STEPPING_DELTA_BRIGHTNESS))) )
        Video.Value = RegVals[0];
    else if ( (RegistryEntryPresent == TRUE) &&
	          (UseRegVals == 0) &&
		      (pDC->FirstTime != TRUE) )
	{
        GetPropertyCurrent(pDC, &Value, REQ_BRIGHTNESS);
        Video.Value = Value; 
	}
	else
	{
        // Marketing suggested Value
        Video.Value = (ULONG)_Brightness.Default * STEPPING_DELTA_BRIGHTNESS;
	}

	SetPropertyCtrl(REQ_BRIGHTNESS, pDC, &Srb);


    return ntStatus = STATUS_SUCCESS;
}



//---------------------------------------------------------------------------
// INTELCAM_SaveControlsToRegistry
//---------------------------------------------------------------------------
NTSTATUS
INTELCAM_SaveControlsToRegistry(
    PDEVICE_OBJECT BusDeviceObject,
    PVOID pDeviceContext
    )
/*++

Routine Description:

    This function saves the camera controls - brightness,
	WhiteBalance, Saturation, Sharpness, and Contrast values
	to the registry.  These will be read by the driver at
	startchannel time and restored.

Arguments:

Return Value:

    NT status code

--*/
{
    PINTELCAM_DEVICE_CONTEXT pDC = pDeviceContext;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG Value;
    BOOLEAN SaveToRegistry = FALSE;
    WCHAR BasePath[] = L"\\Registry\\MACHINE\\SOFTWARE\\Intel\\USBCAMERA1";
    UNICODE_STRING RegPath;
  

	RegPath.Buffer = BasePath;
	RegPath.MaximumLength = sizeof (BasePath + 32);
	RegPath.Length = 0;

    //
	// The DeviceContext has three possible values for saving controls -
	//   0 => Don't Save - Registry entry present
	//   1 => Save  - Registry entry present
	//   0xffffffff => Save  - No Registry entry present
    if ( (pDC->ApplyRegistryValues == 0xffffffff) ||
         (pDC->ApplyRegistryValues == 0x1) )
	    SaveToRegistry = TRUE;
	else if (pDC->ApplyRegistryValues == 0x0)
	    SaveToRegistry = FALSE;
	else
	{
	    SaveToRegistry = FALSE; // check ?? - shouldn't happen
	}


    if (SaveToRegistry == TRUE)
	{
        Value = 1; // implies save values
        ntStatus = RtlWriteRegistryValue(
    	                   RTL_REGISTRY_ABSOLUTE,
                           RegPath.Buffer,
    				       L"ApplyRegistryValues",
    					   REG_DWORD,
                           &Value,
    				       sizeof (ULONG));

        if( NT_SUCCESS(ntStatus) ){
            GetPropertyCurrent(pDC, &Value, REQ_BRIGHTNESS);
		    ntStatus = RtlWriteRegistryValue(
    		                   RTL_REGISTRY_ABSOLUTE,
				               RegPath.Buffer,
    					       L"Brightness",
    						   REG_DWORD,
							   &Value,
    						   sizeof (ULONG));
		}

	}
    return ntStatus;
}

