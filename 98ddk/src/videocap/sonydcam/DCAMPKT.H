/*++

Copyright (c) 1996 Microsoft Corporation

Module Name:

    dcampkt.h

Abstract:

    Header file for dcampkt.c

Author:
    
    Shaun Pierce  25-May-96

Environment:

    Kernel mode only

Revision History:


--*/


//
// Function prototypes
//

VOID
SonyDCamCancelPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
SonyDCamTimeoutHandler(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
SonyDCamReceivePacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
SonyDCamGetStreamInfo(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

NTSTATUS
SonyDCamAllocateIsochResource(    
	IN PHW_STREAM_REQUEST_BLOCK Srb
	);

VOID
SonyDCamFreeIsochResource(    
	IN PHW_STREAM_REQUEST_BLOCK Srb
	);

VOID
SonyDCamOpenStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
SonyDCamCloseStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
SonyDCamReceiveDataPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
SonyDCamReceiveCtrlPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
SonyDCamReadStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
SonyDCamReadStreamWorker(
    IN PHW_STREAM_REQUEST_BLOCK Srb,
    IN PISOCH_DESCRIPTOR IsochDescriptor
    );

NTSTATUS
SonyDCamStartStream(
    IN PSONYDCAM_EXTENSION DeviceExtension,
    IN PIRB Irb,
    IN ULONG ModeIndex,
    IN ULONG FrameRate
    );

VOID
SonyDCamIsochCallback(
    IN PVOID Context1,
    IN PVOID Context2
    );

NTSTATUS
SonyDCamCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
SonyDCamListenCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID SystemArgument
    );

VOID
SonyDCamStopStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

BOOL 
AdapterCompareGUIDsAndFormatSize(
    IN PKSDATARANGE DataRange1,
    IN PKSDATARANGE DataRange2
    );

BOOL 
AdapterVerifyFormat(
    PKS_DATAFORMAT_VIDEOINFOHEADER pKSDataFormatToVerify, 
    int StreamNumber
    );

BOOL 
AdapterFormatFromRange(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID VideoGetProperty(PHW_STREAM_REQUEST_BLOCK Srb);

VOID VideoGetState(PHW_STREAM_REQUEST_BLOCK Srb);

VOID VideoStreamGetConnectionProperty (PHW_STREAM_REQUEST_BLOCK Srb);
VOID VideoStreamGetDroppedFramesProperty(PHW_STREAM_REQUEST_BLOCK Srb);
VOID VideoOpenCloseMasterClock (PHW_STREAM_REQUEST_BLOCK Srb);

VOID VideoIndicateMasterClock (PHW_STREAM_REQUEST_BLOCK Srb);

ULONGLONG GetSystemTime();

VOID STREAMAPI StreamClockRtn(IN PHW_TIME_CONTEXT TimeContext);



//
// External functions
//
#include "sonydcam.h"

//
// External variables
//

extern KSDATAFORMAT StreamFormatYUV444;
extern KSDATAFORMAT StreamFormatYUV422;
extern KSDATAFORMAT StreamFormatYUV411;
extern KSDATAFORMAT StreamFormatRGB24Bpp;
extern KSDATAFORMAT StreamFormatMono8Bpp;


//
// Local variables
//

CAMERA_ISOCH_INFO IsochInfoTable[] = {

        0,      SPEED_FLAGS_100,        0,          // Reserved <---+
        0,      SPEED_FLAGS_100,        0,          // 3.75 fps     |
        15,     SPEED_FLAGS_100,        57600,      // 7.5  fps     |
        30,     SPEED_FLAGS_100,        57600,      // 15   fps     +- Mode0
        60,     SPEED_FLAGS_100,        57600,      // 30   fps     |
        0,      SPEED_FLAGS_100,        0,          // 60   fps <---+
        0,      SPEED_FLAGS_100,        0,          // Reserved <---+
        20,     SPEED_FLAGS_100,        153600,     // 3.75 fps     |
        40,     SPEED_FLAGS_100,        153600,     // 7.5  fps     |
        80,     SPEED_FLAGS_100,        153600,     // 15   fps     +- Mode1
        160,    SPEED_FLAGS_100,        153600,     // 30   fps     |
        0,      SPEED_FLAGS_100,        0,          // 60   fps <---+
        0,      SPEED_FLAGS_100,        0,          // Reserved <---+
        60,     SPEED_FLAGS_100,        460800,     // 3.75 fps     |
        120,    SPEED_FLAGS_100,        460800,     // 7.5  fps     |
        240,    SPEED_FLAGS_100,        460800,     // 15   fps     +- Mode2
        480,    SPEED_FLAGS_200,        460800,     // 30   fps     |
        0,      SPEED_FLAGS_100,        0,          // 60   fps <---+
        0,      SPEED_FLAGS_100,        0,          // Reserved <---+
        80,     SPEED_FLAGS_100,        614400,     // 3.75 fps     |
        160,    SPEED_FLAGS_100,        614400,     // 7.5  fps     |
        320,    SPEED_FLAGS_200,        614400,     // 15   fps     +- Mode3
        640,    SPEED_FLAGS_400,        614400,     // 30   fps     |
        0,      SPEED_FLAGS_100,        0,          // 60   fps <---+
        0,      SPEED_FLAGS_100,        0,          // Reserved <---+
        120,    SPEED_FLAGS_100,        921600,     // 3.75 fps     |
        240,    SPEED_FLAGS_100,        921600,     // 7.5  fps     |
        480,    SPEED_FLAGS_200,        921600,     // 15   fps     +- Mode4
        960,    SPEED_FLAGS_400,        921600,     // 30   fps     |
        0,      SPEED_FLAGS_100,        0,          // 60   fps <---+
        0,      SPEED_FLAGS_100,        0,          // Reserved <---+
        40,     SPEED_FLAGS_100,        307200,     // 3.75 fps     |
        80,     SPEED_FLAGS_100,        307200,     // 7.5  fps     |
        160,    SPEED_FLAGS_100,        307200,     // 15   fps     +- Mode5
        320,    SPEED_FLAGS_200,        307200,     // 30   fps     |
        640,    SPEED_FLAGS_400,        307200,     // 60   fps <---+
        
};




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
    
#ifndef mmioFOURCC    
#define mmioFOURCC( ch0, ch1, ch2, ch3 )				\
		( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
		( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif  

// Msyuv.dll suppport UYVY 
#define FOURCC_UYVY	mmioFOURCC('U', 'Y', 'V', 'Y')
// iyuv_32.dll support IYUV
#define FOURCC_IYUV	mmioFOURCC('I', 'Y', 'U', 'V')





#define DX			320
#define DY			240
#define DBITCOUNT	 16
#define DFPS_DEF	 15
#define DFPS_MAX	 30
#define DFPS_MIN	 (375/100)   // 3.75 FPS


KS_DATARANGE_VIDEO SONYDCAM_StreamFormat_1 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,							                    // Flags
        (DX * DY * DBITCOUNT) / 8,                      // SampleSize
        0,                                              // Reserved
        STATIC_KSDATAFORMAT_TYPE_VIDEO,
        //0x59565955, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71, //MEDIASUBTYPE_UYVY,
        FOURCC_UYVY, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71, //MEDIASUBTYPE_UYVY,

        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE, // StreamDescriptionFlags  (KS_VIDEO_DESC_*)
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, 
        KS_AnalogVideo_None, // AnalogVideoStandard
        DX,DY,		    // InputSize, (the inherent size of the incoming signal
	                    //             with every digitized pixel unique)
        DX,DY,			// MinCroppingSize, smallest rcSrc cropping rect allowed
        DX,DY,			// MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,              // CropGranularityX, granularity of cropping size
        1,              // CropGranularityY
        1,              // CropAlignX, alignment of cropping rect 
        1,              // CropAlignY;
        DX, DY,			// MinOutputSize, smallest bitmap stream can produce
        DX, DY,			// MaxOutputSize, largest  bitmap stream can produce
        DX,             // OutputGranularityX, granularity of output bitmap size
        DY,				// OutputGranularityY;
        0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
        0,              // StretchTapsY
        0,              // ShrinkTapsX 
        0,              // ShrinkTapsY 
        10000000/DFPS_MAX,	// MinFrameInterval (30.00FPS), 100nS units
        10000000/DFPS_MIN,	// MaxFrameInterval ( 3.75FPS), 100nS units
        (DX * DY * DBITCOUNT) * DFPS_MIN,	// MinBitsPerSecond (3.75 FPS);
        (DX * DY * DBITCOUNT) * DFPS_MAX	// MaxBitsPerSecond (30.0 FPS);
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                            // RECT  rcSource; 
        0,0,0,0,                            // RECT  rcTarget; 
        (DX * DY * DBITCOUNT) * DFPS_DEF,   // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate;   
        10000000/DFPS_DEF,			        // REFERENCE_TIME  AvgTimePerFrame (15 FPS); 
        sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
        DX,									// LONG  biWidth;
        DY,								    // LONG  biHeight;
        1,                                  // WORD  biPlanes;
        DBITCOUNT,                          // WORD  biBitCount;
        FOURCC_UYVY,						// DWORD biCompression;
        (DX * DY * DBITCOUNT)/8,            // DWORD biSizeImage;
        0,                                  // LONG  biXPelsPerMeter;
        0,                                  // LONG  biYPelsPerMeter;
        0,                                  // DWORD biClrUsed;
        0                                   // DWORD biClrImportant;
    }
}; 


#if 0 // Listed here for reference.

#undef DX			
#undef DY			
#undef DBITCOUNT	
#undef DFPS_DEF	 
#undef DFPS_MAX	 
#undef DFPS_MIN


#define DX			640
#define DY			480
#define DBITCOUNT	 12
#define DFPS_DEF	 75/10
#define DFPS_MAX	 15
#define DFPS_MIN	 375/100   // 3.75 FPS
// To support Mode 2, 640x480, IYUV (YUV411)
KS_DATARANGE_VIDEO SONYDCAM_StreamFormat_2 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,							                    // Flags
        (DX * DY * DBITCOUNT) / 8,                      // SampleSize
        0,                                              // Reserved
        STATIC_KSDATAFORMAT_TYPE_VIDEO,
        FOURCC_IYUV, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71, //MEDIASUBTYPE_UYVY,

        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE, // StreamDescriptionFlags  (KS_VIDEO_DESC_*)
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, 
        KS_AnalogVideo_None, // AnalogVideoStandard
        DX,DY,		    // InputSize, (the inherent size of the incoming signal
	                    //             with every digitized pixel unique)
        DX,DY,			// MinCroppingSize, smallest rcSrc cropping rect allowed
        DX,DY,			// MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,              // CropGranularityX, granularity of cropping size
        1,              // CropGranularityY
        1,              // CropAlignX, alignment of cropping rect 
        1,              // CropAlignY;
        DX, DY,			// MinOutputSize, smallest bitmap stream can produce
        DX, DY,			// MaxOutputSize, largest  bitmap stream can produce
        DX,             // OutputGranularityX, granularity of output bitmap size
        DY,				// OutputGranularityY;
        0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
        0,              // StretchTapsY
        0,              // ShrinkTapsX 
        0,              // ShrinkTapsY 
        10000000/DFPS_MAX,	// MinFrameInterval (30.00FPS), 100nS units
        10000000/DFPS_MIN,	// MaxFrameInterval ( 3.75FPS), 100nS units
        (DX * DY * DBITCOUNT) * DFPS_MIN,	// MinBitsPerSecond (3.75 FPS);
        (DX * DY * DBITCOUNT) * DFPS_MAX	// MaxBitsPerSecond (30.0 FPS);
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                            // RECT  rcSource; 
        0,0,0,0,                            // RECT  rcTarget; 
        (DX * DY * DBITCOUNT) * DFPS_DEF,   // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate;   
        10000000/DFPS_DEF,			        // REFERENCE_TIME  AvgTimePerFrame (15 FPS); 
        sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
        DX,									// LONG  biWidth;
        DY,									// LONG  biHeight;
        1,                                  // WORD  biPlanes;
        DBITCOUNT,                          // WORD  biBitCount;
        FOURCC_IYUV,						// DWORD biCompression;
        (DX * DY * DBITCOUNT)/8,            // DWORD biSizeImage;
        0,                                  // LONG  biXPelsPerMeter;
        0,                                  // LONG  biYPelsPerMeter;
        0,                                  // DWORD biClrUsed;
        0                                   // DWORD biClrImportant;
    }
};
    
#undef DX			
#undef DY			
#undef DBITCOUNT	
#undef DFPS_DEF	 
#undef DFPS_MAX	 
#undef DFPS_MIN

#define DX			640
#define DY			480
#define DBITCOUNT	 16
#define DFPS_DEF	 750/100
#define DFPS_MAX	 750/100
#define DFPS_MIN	 375/100   // 3.75 FPS

// To support mode3, 640x480
KS_DATARANGE_VIDEO SONYDCAM_StreamFormat_3 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,							                    // Flags
        (DX * DY * DBITCOUNT) / 8,                      // SampleSize
        0,                                              // Reserved
        STATIC_KSDATAFORMAT_TYPE_VIDEO,
        FOURCC_UYVY, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71, //MEDIASUBTYPE_UYVY,

        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE, // StreamDescriptionFlags  (KS_VIDEO_DESC_*)
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, 
        KS_AnalogVideo_None, // AnalogVideoStandard
        DX,DY,		    // InputSize, (the inherent size of the incoming signal
	                    //             with every digitized pixel unique)
        DX,DY,			// MinCroppingSize, smallest rcSrc cropping rect allowed
        DX,DY,			// MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,              // CropGranularityX, granularity of cropping size
        1,              // CropGranularityY
        1,              // CropAlignX, alignment of cropping rect 
        1,              // CropAlignY;
        DX, DY,			// MinOutputSize, smallest bitmap stream can produce
        DX, DY,			// MaxOutputSize, largest  bitmap stream can produce
        DX,             // OutputGranularityX, granularity of output bitmap size
        DY,				// OutputGranularityY;
        0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
        0,              // StretchTapsY
        0,              // ShrinkTapsX 
        0,              // ShrinkTapsY 
        10000000/DFPS_MAX,	// MinFrameInterval (30.00FPS), 100nS units
        10000000/DFPS_MIN,	// MaxFrameInterval ( 3.75FPS), 100nS units
        (DX * DY * DBITCOUNT) * DFPS_MIN,	// MinBitsPerSecond (3.75 FPS);
        (DX * DY * DBITCOUNT) * DFPS_MAX	// MaxBitsPerSecond (30.0 FPS);
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                            // RECT  rcSource; 
        0,0,0,0,                            // RECT  rcTarget; 
        (DX * DY * DBITCOUNT) * DFPS_DEF,   // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate;   
        10000000/DFPS_DEF,			        // REFERENCE_TIME  AvgTimePerFrame (15 FPS); 
        sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
        DX,									// LONG  biWidth;
        DY,									// LONG  biHeight;
        1,                                  // WORD  biPlanes;
        DBITCOUNT,                          // WORD  biBitCount;
        FOURCC_UYVY,						// DWORD biCompression;
        (DX * DY * DBITCOUNT)/8,            // DWORD biSizeImage;
        0,                                  // LONG  biXPelsPerMeter;
        0,                                  // LONG  biYPelsPerMeter;
        0,                                  // DWORD biClrUsed;
        0                                   // DWORD biClrImportant;
    }
};

#undef DX			
#undef DY			
#undef DBITCOUNT	
#undef DFPS_DEF	 
#undef DFPS_MAX	 
#undef DFPS_MIN

#define DX			640
#define DY			480
#define DBITCOUNT	 24
#define DFPS_DEF	 750/100
#define DFPS_MAX	 750/100
#define DFPS_MIN	 375/100   // 3.75 FPS

// To support mode4, 640x480, RGB
KS_DATARANGE_VIDEO SONYDCAM_StreamFormat_4 = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),
        0,							                    // Flags
        (DX * DY * DBITCOUNT) / 8,                      // SampleSize
        0,                                              // Reserved
        STATIC_KSDATAFORMAT_TYPE_VIDEO,
        //FOURCC_UYVY, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71, //MEDIASUBTYPE_UYVY,
        0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,

        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE, // StreamDescriptionFlags  (KS_VIDEO_DESC_*)
    0,                  // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, 
        KS_AnalogVideo_None, // AnalogVideoStandard
        DX,DY,		    // InputSize, (the inherent size of the incoming signal
	                    //             with every digitized pixel unique)
        DX,DY,			// MinCroppingSize, smallest rcSrc cropping rect allowed
        DX,DY,			// MaxCroppingSize, largest  rcSrc cropping rect allowed
        1,              // CropGranularityX, granularity of cropping size
        1,              // CropGranularityY
        1,              // CropAlignX, alignment of cropping rect 
        1,              // CropAlignY;
        DX, DY,			// MinOutputSize, smallest bitmap stream can produce
        DX, DY,			// MaxOutputSize, largest  bitmap stream can produce
        DX,             // OutputGranularityX, granularity of output bitmap size
        DY,				// OutputGranularityY;
        0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
        0,              // StretchTapsY
        0,              // ShrinkTapsX 
        0,              // ShrinkTapsY 
        10000000/DFPS_MAX,	// MinFrameInterval (30.00FPS), 100nS units
        10000000/DFPS_MIN,	// MaxFrameInterval ( 3.75FPS), 100nS units
        (DX * DY * DBITCOUNT) * DFPS_MIN,	// MinBitsPerSecond (3.75 FPS);
        (DX * DY * DBITCOUNT) * DFPS_MAX	// MaxBitsPerSecond (30.0 FPS);
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                            // RECT  rcSource; 
        0,0,0,0,                            // RECT  rcTarget; 
        (DX * DY * DBITCOUNT) * DFPS_DEF,   // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate;   
        10000000/DFPS_DEF,			        // REFERENCE_TIME  AvgTimePerFrame (15 FPS); 
        sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
        DX,									// LONG  biWidth;
        DY,									// LONG  biHeight;
        1,                                  // WORD  biPlanes;
        DBITCOUNT,                          // WORD  biBitCount;
        KS_BI_RGB,							// DWORD biCompression;
        (DX * DY * DBITCOUNT)/8,            // DWORD biSizeImage;
        0,                                  // LONG  biXPelsPerMeter;
        0,                                  // LONG  biYPelsPerMeter;
        0,                                  // DWORD biClrUsed;
        0                                   // DWORD biClrImportant;
    }
};
    
#endif

PKSDATAFORMAT SONYDCAM_StreamFormats[] = 
{
	(PKSDATAFORMAT) &SONYDCAM_StreamFormat_1,
#if 0 // For reference only
	(PKSDATAFORMAT) &SONYDCAM_StreamFormat_2,
	(PKSDATAFORMAT) &SONYDCAM_StreamFormat_3,
	(PKSDATAFORMAT) &SONYDCAM_StreamFormat_4
#endif

};

#define NUM_DCAM_STREAM_FORMATS (sizeof (SONYDCAM_StreamFormats) / sizeof (PKSDATAFORMAT))


//---------------------------------------------------------------------------
// Topology
//---------------------------------------------------------------------------

// Categories define what the device does.

static GUID Categories[] = {
    STATIC_KSCATEGORY_VIDEO,
    STATIC_KSCATEGORY_CAPTURE
};

#define NUMBER_OF_CATEGORIES  SIZEOF_ARRAY (Categories)

static KSTOPOLOGY Topology = {
    NUMBER_OF_CATEGORIES,
    (GUID*) &Categories,
    0,
    NULL,
    0,
    NULL
};


