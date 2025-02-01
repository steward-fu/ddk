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

#ifndef __CAPSTRM_H__
#define __CAPSTRM_H__

#include "mediums.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "defaults.h"
#include "capvport.h"

// ------------------------------------------------------------------------
// The master list of all streams supported by this driver
// ------------------------------------------------------------------------

KSEVENT_ITEM VPEventItm[] =
{
    {
        KSEVENT_VPNOTIFY_FORMATCHANGE,
        0,
        0,
        NULL,
        NULL,
        NULL
    }
};

GUID MY_KSEVENTSETID_VPNOTIFY = {STATIC_KSEVENTSETID_VPNotify};

KSEVENT_SET VPEventSet[] =
{
    {
        &MY_KSEVENTSETID_VPNOTIFY,
        SIZEOF_ARRAY(VPEventItm),
        VPEventItm,
    }
};


KSEVENT_ITEM VPVBIEventItm[] =
{
    {
        KSEVENT_VPVBINOTIFY_FORMATCHANGE,
        0,
        0,
        NULL,
        NULL,
        NULL
    }
};

GUID MY_KSEVENTSETID_VPVBINOTIFY = {STATIC_KSEVENTSETID_VPVBINotify};

KSEVENT_SET VPVBIEventSet[] =
{
    {
        &MY_KSEVENTSETID_VPVBINOTIFY,
        SIZEOF_ARRAY(VPVBIEventItm),
        VPVBIEventItm,
    }
};


extern const KSPROPERTY_SET VideoPortPropertyTable[];
extern const KSPROPERTY_SET VideoPortVBIPropertyTable[];

// ------------------------------------------------------------------------
// Property set for video and VBI capture streams
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
// Array of the property sets supported by video and VBI capture streams
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

//---------------------------------------------------------------------------
// All of the video and vbi data formats we might use
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Capture Stream Formats
//---------------------------------------------------------------------------
KS_DATARANGE_VIDEO StreamFormatUYVY_Capture =
{
    // KSDATARANGE
    {
        sizeof( KS_DATARANGE_VIDEO ),     // FormatSize
        0,                                // Flags
        DefWidth * DefHeight * 2,         // SampleSize
        0,                                // Reserved

        STATIC_KSDATAFORMAT_TYPE_VIDEO,                 // aka. MEDIATYPE_Video
        0x59565955, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71, // MEDIASUBTYPE_UYVY
        STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO         // aka. FORMAT_VideoInfo
    },

    TRUE,                                // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,                                // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_CAPTURE,              // StreamDescriptionFlags  (KS_VIDEO_DESC_*)
    0,                                   // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS
    {
        {STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO},      // GUID
        {KS_AnalogVideo_NTSC_M | KS_AnalogVideo_PAL_B}, // AnalogVideoStandard
        {
            MaxInWidth, MaxInHeight      // SIZE InputSize
        },
        {
            MinInWidth, MinInHeight      // SIZE MinCroppingSize;       smallest rcSrc cropping rect allowed
        },
        {
            MaxInWidth, MaxInHeight      // SIZE MaxCroppingSize;       largest rcSrc cropping rect allowed
        },
        2,                               // int CropGranularityX;       // granularity of cropping size
        2,                               // int CropGranularityY;
        2,                               // int CropAlignX;             // alignment of cropping rect
        2,                               // int CropAlignY;
        {
            MinOutWidth, MinOutHeight    // SIZE MinOutputSize;         // smallest bitmap stream can produce
        },
        {
            MaxOutWidth, MaxOutHeight    // SIZE MaxOutputSize;         // largest  bitmap stream can produce
        },      
        80,                              // int OutputGranularityX;     // granularity of output bitmap size
        60,                              // int OutputGranularityY;
        0,                               // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
        0,                               // StretchTapsY
        2,                               // ShrinkTapsX 
        2,                               // ShrinkTapsY 
        NTSCFieldDuration,               // LONGLONG MinFrameInterval;  // 100 nS units
        (LONGLONG)NTSCFieldDuration*MAXULONG,   // LONGLONG MaxFrameInterval;
        30 * 80 * 40 * 2 * 8,            // LONG MinBitsPerSecond;
        30 * 720 * 480 * 2 * 8           // LONG MaxBitsPerSecond;
    },

    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                          //    RECT            rcSource;          // The bit we really want to use
        0,0,0,0,                          //    RECT            rcTarget;          // Where the video should go
        DefWidth * DefHeight * 2 * 30L,   //    DWORD           dwBitRate;         // Approximate bit data rate
        0L,                               //    DWORD           dwBitErrorRate;    // Bit error rate for this stream
        
        // 30 fps
        NTSCFieldDuration * 2,            //    REFERENCE_TIME  AvgTimePerFrame;   // Average time per frame (100ns units)
        
        sizeof KS_BITMAPINFOHEADER,       //    DWORD      biSize;
        DefWidth,                         //    LONG       biWidth;
        DefHeight,                        //    LONG       biHeight;
        1,                                //    WORD       biPlanes;
        16,                               //    WORD       biBitCount;
        FOURCC_UYVY,                      //    DWORD      biCompression;
        DefWidth * DefHeight * 2,         //    DWORD      biSizeImage;
        0,                                //    LONG       biXPelsPerMeter;
        0,                                //    LONG       biYPelsPerMeter;
        0,                                //    DWORD      biClrUsed;
        0                                 //    DWORD      biClrImportant;
    }
};

static KSDATAFORMAT StreamFormatVideoPort = 
{
    {
        sizeof( KSDATAFORMAT),
        0,
        0,
        0,
        STATIC_KSDATAFORMAT_TYPE_VIDEO,
        STATIC_KSDATAFORMAT_SUBTYPE_VPVideo,
        STATIC_KSDATAFORMAT_SPECIFIER_NONE
    }
};

KS_DATARANGE_VIDEO_VBI StreamFormatVBI =
{
    // KSDATARANGE
    {
        {
            sizeof( KS_DATARANGE_VIDEO_VBI ),
            0,
            VBISamples * VBILines,         // SampleSize
            0,                             // Reserved
            { STATIC_KSDATAFORMAT_TYPE_VBI },
            { STATIC_KSDATAFORMAT_SUBTYPE_RAW8 },
            { STATIC_KSDATAFORMAT_SPECIFIER_VBI }
        }
    },
    TRUE,                                // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,                                // BOOL,  bTemporalCompression (all I frames?)
    KS_VIDEOSTREAM_VBI,                  // StreamDescriptionFlags  (KS_VIDEO_DESC_*)
    0,                                   // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

    // _KS_VIDEO_STREAM_CONFIG_CAPS
    {
        { STATIC_KSDATAFORMAT_SPECIFIER_VBI },
        KS_AnalogVideo_NTSC_M,                             // AnalogVideoStandard
        {
            VBISamples, VBILines  // SIZE InputSize
        },
        {
            VBISamples, VBILines  // SIZE MinCroppingSize;       smallest rcSrc cropping rect allowed
        },
        {
            VBISamples, VBILines  // SIZE MaxCroppingSize;       largest rcSrc cropping rect allowed
        },
        1,                  // int CropGranularityX;       // granularity of cropping size
        1,                  // int CropGranularityY;
        1,                  // int CropAlignX;             // alignment of cropping rect
        1,                  // int CropAlignY;
        {
            VBISamples, VBILines  // SIZE MinOutputSize;   // smallest bitmap stream can produce
        },
        {
            VBISamples, VBILines  // SIZE MaxOutputSize;   // largest  bitmap stream can produce
        },
        1,                  // int OutputGranularityX;     // granularity of output bitmap size
        2,                  // int OutputGranularityY;
        0,                  // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
        0,                  // StretchTapsY
        0,                  // ShrinkTapsX
        0,                  // ShrinkTapsY
        NTSCFieldDuration,  // LONGLONG MinFrameInterval;  // 100 nS units
        NTSCFieldDuration,  // LONGLONG MaxFrameInterval;
        VBISamples * 30 * VBILines * 2 * 8, // LONG MinBitsPerSecond;
        VBISamples * 30 * VBILines * 2 * 8  // LONG MaxBitsPerSecond;
    },

    // KS_VBIINFOHEADER (default format)
    {
        VBIStart,               // StartLine  -- inclusive
        VBIEnd,                 // EndLine    -- inclusive
        SamplingFrequency,      // SamplingFrequency
        454,                    // MinLineStartTime;    // (uS past HR LE) * 100
        900,                    // MaxLineStartTime;    // (uS past HR LE) * 100

        // empirically discovered
        780,                    // ActualLineStartTime  // (uS past HR LE) * 100

        5902,                   // ActualLineEndTime;   // (uS past HR LE) * 100
        KS_AnalogVideo_NTSC_M,  // VideoStandard;
        VBISamples,             // SamplesPerLine;
        VBISamples,             // StrideInBytes;
        VBISamples * VBILines   // BufferSize;
    }
};

static KSDATAFORMAT StreamFormatVideoPortVBI = 
{
    {
        sizeof( KSDATAFORMAT),
        0,
        0,
        0,
        STATIC_KSDATAFORMAT_TYPE_VIDEO,
        STATIC_KSDATAFORMAT_SUBTYPE_VPVBI,
        STATIC_KSDATAFORMAT_SPECIFIER_NONE
    }
};

static KS_DATARANGE_ANALOGVIDEO StreamFormatAnalogVideo = 
{
    // KS_DATARANGE_ANALOGVIDEO
    {   
        sizeof (KS_DATARANGE_ANALOGVIDEO),      // FormatSize
        0,                                      // Flags
        sizeof (KS_TVTUNER_CHANGE_INFO),        // SampleSize
        0,                                      // Reserved

        STATIC_KSDATAFORMAT_TYPE_ANALOGVIDEO,   // aka MEDIATYPE_AnalogVideo
        STATIC_KSDATAFORMAT_SUBTYPE_NONE,       // ie. Wildcard
        STATIC_KSDATAFORMAT_SPECIFIER_ANALOGVIDEO, // aka FORMAT_AnalogVideo
    },
    // KS_ANALOGVIDEOINFO
    {
        0, 0, 720, 480,         // rcSource;                
        0, 0, 720, 480,         // rcTarget;        
        720,                    // dwActiveWidth;   
        480,                    // dwActiveHeight;  
        0,                      // REFERENCE_TIME  AvgTimePerFrame; 
    }
};

//---------------------------------------------------------------------------
//  Capture Stream Formats, Mediums, and PinNames
//---------------------------------------------------------------------------

static PKSDATAFORMAT CaptureStreamFormats[] = 
{
    (PKSDATAFORMAT) &StreamFormatUYVY_Capture,
};
#define NUM_CAPTURE_STREAM_FORMATS (SIZEOF_ARRAY (CaptureStreamFormats))

static GUID CaptureStreamPinName = {STATIC_PINNAME_VIDEO_CAPTURE};


//---------------------------------------------------------------------------
//  Video Port Stream Formats, Mediums, and PinNames
//---------------------------------------------------------------------------

static PKSDATAFORMAT VPStreamFormats[] = 
{
    (PKSDATAFORMAT) &StreamFormatVideoPort,
};
#define NUM_VP_STREAM_FORMATS (SIZEOF_ARRAY (VPStreamFormats))

static GUID VideoPortPinName = {STATIC_PINNAME_VIDEO_VIDEOPORT};


//---------------------------------------------------------------------------
//  VBI Stream Formats, Mediums, and PinNames
//---------------------------------------------------------------------------

static PKSDATAFORMAT VBIStreamFormats[] = 
{
    (PKSDATAFORMAT) &StreamFormatVBI,
};
#define NUM_VBI_STREAM_FORMATS (SIZEOF_ARRAY (VBIStreamFormats))

static GUID VBIStreamPinName = {STATIC_PINNAME_VIDEO_VBI};


//---------------------------------------------------------------------------
//  Video Port VBI Stream Formats, Mediums, and PinNames
//---------------------------------------------------------------------------

static PKSDATAFORMAT VPVBIStreamFormats[] = 
{
    (PKSDATAFORMAT) &StreamFormatVideoPortVBI,
};
#define NUM_VPVBI_STREAM_FORMATS (SIZEOF_ARRAY (VPVBIStreamFormats))

static GUID VPVBIPinName = {STATIC_PINNAME_VIDEO_VIDEOPORT_VBI};


//---------------------------------------------------------------------------
//  Analog Video Stream Formats, Mediums, and PinNames
//---------------------------------------------------------------------------

static PKSDATAFORMAT AnalogVideoFormats[] = 
{
    (PKSDATAFORMAT) &StreamFormatAnalogVideo,
};
#define NUM_ANALOG_VIDEO_FORMATS (SIZEOF_ARRAY (AnalogVideoFormats))

static GUID AnalogVideoStreamPinName = {STATIC_PINNAME_VIDEO_ANALOGVIDEOIN};

//---------------------------------------------------------------------------
// Create an array that holds the list of all of the streams supported
//---------------------------------------------------------------------------

typedef struct _ALL_STREAM_INFO {
    HW_STREAM_INFORMATION   hwStreamInfo;
    HW_STREAM_OBJECT        hwStreamObject;
} ALL_STREAM_INFO, *PALL_STREAM_INFO;

static ALL_STREAM_INFO Streams [] =
{
    // -----------------------------------------------------------------
    // The Video Capture output stream
    // -----------------------------------------------------------------
    {
        // HW_STREAM_INFORMATION -------------------------------------------
        {
            1,                                      // NumberOfPossibleInstances
            KSPIN_DATAFLOW_OUT,                     // DataFlow
            TRUE,                                   // DataAccessible
            NUM_CAPTURE_STREAM_FORMATS,             // NumberOfFormatArrayEntries
            CaptureStreamFormats,                   // StreamFormatsArray
            0,                                      // ClassReserved[0]
            0,                                      // ClassReserved[1]
            0,                                      // ClassReserved[2]
            0,                                      // ClassReserved[3]
            NUMBER_VIDEO_STREAM_PROPERTIES,         // NumStreamPropArrayEntries
            (PKSPROPERTY_SET)VideoStreamProperties, // StreamPropertiesArray
            0,                                      // NumStreamEventArrayEntries;
            0,                                      // StreamEventsArray;
            &CaptureStreamPinName,                  // Category
            &CaptureStreamPinName,                  // Name
            0,                                      // MediumsCount
            NULL,                                   // Mediums
        },

        // HW_STREAM_OBJECT ------------------------------------------------
        {
            sizeof (HW_STREAM_OBJECT),              // SizeOfThisPacket
            0,                                      // StreamNumber
            0,                                      // HwStreamExtension
            VideoReceiveDataPacket,                 // HwReceiveDataPacket
            VideoReceiveCtrlPacket,                 // HwReceiveControlPacket
            { NULL, 0 },                            // HW_CLOCK_OBJECT
            FALSE,                                  // Dma
            TRUE,                                   // Pio
            0,                                      // HwDeviceExtension
            sizeof (KS_FRAME_INFO),                 // StreamHeaderMediaSpecific
            0,                                      // StreamHeaderWorkspace 
            TRUE,                                   // Allocator 
            NULL,                                   // HwEventRoutine
            { 0, 0 },                               // Reserved[2]
        }
    },

    // -----------------------------------------------------------------
    // The Video Port output stream
    // -----------------------------------------------------------------
    {
        // HW_STREAM_INFORMATION -------------------------------------------
        {
            1,                                      // NumberOfPossibleInstances
            KSPIN_DATAFLOW_OUT,                     // DataFlow
            TRUE,                                   // DataAccessible.
            NUM_VP_STREAM_FORMATS,                  // NumberOfFormatArrayEntries
            VPStreamFormats,                        // StreamFormatsArray
            0,                                      // ClassReserved[0]
            0,                                      // ClassReserved[1]
            0,                                      // ClassReserved[2]
            0,                                      // ClassReserved[3]
            1, // BUGBUG. Fix hardcoded value       // NumStreamPropArrayEntries
            (PKSPROPERTY_SET)VideoPortPropertyTable,// StreamPropertiesArray
            SIZEOF_ARRAY(VPEventSet),               // NumStreamEventArrayEntries
            VPEventSet,                             // StreamEventsArray
            &VideoPortPinName,                      // Category
            &VideoPortPinName,                      // Name
            0,                                      // MediumsCount
            NULL,                                   // Mediums
        },

        // HW_STREAM_OBJECT ------------------------------------------------
        {
            sizeof (HW_STREAM_OBJECT),              // SizeOfThisPacket
            0,                                      // StreamNumber
            0,                                      // HwStreamExtension
            VideoReceiveDataPacket,                 // HwReceiveDataPacket
            VideoReceiveCtrlPacket,                 // HwReceiveControlPacket
            { NULL, 0 },                            // HW_CLOCK_OBJECT
            FALSE,                                  // Dma
            FALSE,                                   // Pio
            0,                                      // HwDeviceExtension
            0,                                      // StreamHeaderMediaSpecific
            0,                                      // StreamHeaderWorkspace 
            FALSE,                                  // Allocator 
            VPStreamEventProc,                      // HwEventRoutine;
            { 0, 0 },                               // Reserved[2]
        }
    },

    // -----------------------------------------------------------------
    // The VBI Capture output stream
    // -----------------------------------------------------------------
    {
        // HW_STREAM_INFORMATION -------------------------------------------
        {
            1,                                      // NumberOfPossibleInstances
            KSPIN_DATAFLOW_OUT,                     // DataFlow
            TRUE,                                   // DataAccessible
            NUM_VBI_STREAM_FORMATS,                 // NumberOfFormatArrayEntries
            VBIStreamFormats,                       // StreamFormatsArray
            0,                                      // ClassReserved[0]
            0,                                      // ClassReserved[1]
            0,                                      // ClassReserved[2]
            0,                                      // ClassReserved[3]
            NUMBER_VIDEO_STREAM_PROPERTIES,         // NumStreamPropArrayEntries
            (PKSPROPERTY_SET)VideoStreamProperties, // StreamPropertiesArray
            0,                                      // NumStreamEventArrayEntries;
            0,                                      // StreamEventsArray;
            &VBIStreamPinName,                      // Category
            &VBIStreamPinName,                      // Name
            0,                                      // MediumsCount
            NULL,                                   // Mediums
        },

        // HW_STREAM_OBJECT ------------------------------------------------
        {
            sizeof (HW_STREAM_OBJECT),              // SizeOfThisPacket
            0,                                      // StreamNumber
            0,                                      // HwStreamExtension
            VideoReceiveDataPacket,                 // HwReceiveDataPacket
            VideoReceiveCtrlPacket,                 // HwReceiveControlPacket
            { NULL, 0 },                            // HW_CLOCK_OBJECT
            FALSE,                                  // Dma
            TRUE,                                   // Pio
            0,                                      // HwDeviceExtension
            sizeof (KS_VBI_FRAME_INFO),             // StreamHeaderMediaSpecific
            0,                                      // StreamHeaderWorkspace 
            TRUE,                                   // Allocator 
            NULL,                                   // HwEventRoutine
            { 0, 0 },                               // Reserved[2]
        }
    },

    // -----------------------------------------------------------------
    // The Video Port VBI output stream
    // -----------------------------------------------------------------
    {
        // HW_STREAM_INFORMATION -------------------------------------------
        {
            1,                                      // NumberOfPossibleInstances
            KSPIN_DATAFLOW_OUT,                     // DataFlow
            TRUE,                                   // DataAccessible.
            NUM_VPVBI_STREAM_FORMATS,               // NumberOfFormatArrayEntries
            VPVBIStreamFormats,                     // StreamFormatsArray
            0,                                      // ClassReserved[0]
            0,                                      // ClassReserved[1]
            0,                                      // ClassReserved[2]
            0,                                      // ClassReserved[3]
            1, // BUGBUG. Fix hardcoded value       // NumStreamPropArrayEntries
            (PKSPROPERTY_SET)VideoPortVBIPropertyTable,    // StreamPropertiesArray
            SIZEOF_ARRAY(VPVBIEventSet),            // NumStreamEventArrayEntries
            VPVBIEventSet,                          // StreamEventsArray
            &VPVBIPinName,                          // Category
            &VPVBIPinName,                          // Name
            0,                                      // MediumsCount
            NULL,                                   // Mediums
        },

        // HW_STREAM_OBJECT ------------------------------------------------
        {
            sizeof (HW_STREAM_OBJECT),              // SizeOfThisPacket
            0,                                      // StreamNumber
            0,                                      // HwStreamExtension
            VideoReceiveDataPacket,                 // HwReceiveDataPacket
            VideoReceiveCtrlPacket,                 // HwReceiveControlPacket
            { NULL, 0 },                            // HW_CLOCK_OBJECT
            FALSE,                                  // Dma
            FALSE,                                   // Pio
            0,                                      // HwDeviceExtension
            0,                                      // StreamHeaderMediaSpecific
            0,                                      // StreamHeaderWorkspace 
            FALSE,                                  // Allocator 
            VPVBIStreamEventProc,                   // HwEventRoutine;
            { 0, 0 },                               // Reserved[2]
        }
    },

    // -----------------------------------------------------------------
    // The Analog Video Input Stream 
    // -----------------------------------------------------------------
    {
        // HW_STREAM_INFORMATION -------------------------------------------
        {
            1,                                      // NumberOfPossibleInstances
            KSPIN_DATAFLOW_IN,                      // DataFlow
            TRUE,                                   // DataAccessible
            NUM_ANALOG_VIDEO_FORMATS,               // NumberOfFormatArrayEntries
            AnalogVideoFormats,                     // StreamFormatsArray
            0,                                      // ClassReserved[0]
            0,                                      // ClassReserved[1]
            0,                                      // ClassReserved[2]
            0,                                      // ClassReserved[3]
            0,                                      // NumStreamPropArrayEntries
            0,                                      // StreamPropertiesArray
            0,                                      // NumStreamEventArrayEntries;
            0,                                      // StreamEventsArray;
            &AnalogVideoStreamPinName,              // Category
            &AnalogVideoStreamPinName,              // Name
            1,                                      // MediumsCount
            &CrossbarMediums[3],                    // Mediums
        },
           
        // HW_STREAM_OBJECT ------------------------------------------------
        {
            sizeof (HW_STREAM_OBJECT),              // SizeOfThisPacket
            0,                                      // StreamNumber
            0,                                      // HwStreamExtension
            VideoReceiveDataPacket,                 // HwReceiveDataPacket
            VideoReceiveCtrlPacket,                 // HwReceiveControlPacket
            NULL,                                   // HW_CLOCK_OBJECT.HWClockFunction
            0,                                      // HW_CLOCK_OBJECT.ClockSupportFlags
            FALSE,                                  // Dma
            TRUE,                                   // Pio
            0,                                      // HwDeviceExtension
            0,                                      // StreamHeaderMediaSpecific
            0,                                      // StreamHeaderWorkspace 
            FALSE,                                  // Allocator 
            NULL,                                   // HwEventRoutine
        },

    }
};

#define DRIVER_STREAM_COUNT (sizeof (Streams) / sizeof (ALL_STREAM_INFO))

//---------------------------------------------------------------------------
// Topology
//---------------------------------------------------------------------------

// Categories define what the device does.

static GUID Categories[] = {
    STATIC_KSCATEGORY_VIDEO,
    STATIC_KSCATEGORY_CAPTURE,
    STATIC_KSCATEGORY_CROSSBAR,
};

#define NUMBER_OF_CATEGORIES  SIZEOF_ARRAY (Categories)

static KSTOPOLOGY Topology = {
    NUMBER_OF_CATEGORIES,               // CategoriesCount
    (GUID*) &Categories,                // Categories
    0,                                  // TopologyNodesCount
    NULL,                               // TopologyNodes
    0,                                  // TopologyConnectionsCount
    NULL,                               // TopologyConnections
    NULL,                               // TopologyNodesNames
    0,                                  // Reserved
};

//---------------------------------------------------------------------------
// The Main stream header
//---------------------------------------------------------------------------

static HW_STREAM_HEADER StreamHeader = 
{
    DRIVER_STREAM_COUNT,                // NumberOfStreams
    sizeof (HW_STREAM_INFORMATION),     // Future proofing
    0,                                  // NumDevPropArrayEntries set at init time
    NULL,                               // DevicePropertiesArray  set at init time
    0,                                  // NumDevEventArrayEntries;
    NULL,                               // DeviceEventsArray;
    &Topology                           // Pointer to Device Topology
};


#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // __CAPSTRM_H__
