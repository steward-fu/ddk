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

#ifndef __CODSTRM_H__
#define __CODSTRM_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    
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
    )
};

DEFINE_KSPROPERTY_TABLE( StreamAllocatorProperties )
{
	DEFINE_KSPROPERTY_ITEM_STREAM_ALLOCATOR
   (
   	FALSE,
       TRUE
   )
};

// ------------------------------------------------------------------------
// Per pin property set for VBI codec filtering
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_TABLE(VBICodecProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VBICODECFILTERING_SCANLINES_REQUESTED_BIT_ARRAY,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(VBICODECFILTERING_SCANLINES),    // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(VBICODECFILTERING_SCANLINES)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VBICODECFILTERING_SCANLINES_DISCOVERED_BIT_ARRAY,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(VBICODECFILTERING_SCANLINES),    // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(VBICODECFILTERING_SCANLINES)     // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_REQUESTED_BIT_ARRAY,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(VBICODECFILTERING_NABTS_SUBSTREAMS),// MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(VBICODECFILTERING_NABTS_SUBSTREAMS)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VBICODECFILTERING_SUBSTREAMS_DISCOVERED_BIT_ARRAY,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(VBICODECFILTERING_NABTS_SUBSTREAMS),// MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(VBICODECFILTERING_NABTS_SUBSTREAMS)                           // SerializedSize
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
        VideoStreamConnectionProperties,                // PropertyItems
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    (
    	&KSPROPSETID_Stream,
       SIZEOF_ARRAY( StreamAllocatorProperties ),
       StreamAllocatorProperties,
       0,
       NULL
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &KSPROPSETID_VBICodecFiltering,                 // Set
        SIZEOF_ARRAY(VBICodecProperties),               // PropertiesCount
        VBICodecProperties,                             // PropertyItems
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),
};

#define NUMBER_VIDEO_STREAM_PROPERTIES (SIZEOF_ARRAY(VideoStreamProperties))

//---------------------------------------------------------------------------
// All of the video and vbi data formats we might use
//---------------------------------------------------------------------------

// Warning, the following VBI geometry is governed by the capture driver NOT
// the codecs.  Therefore, any specification of a VBI capture format will be
// ignored by most capture drivers.  Look at the KS_VBI_FRAME_INFO data on each
// sample to determine the actual data characteristics of the samples.

#define NORMAL_VBI_START_LINE   10
#define NORMAL_VBI_STOP_LINE    21

#define MIN_VBI_X_SAMPLES (720*2)
#define AVG_VBI_X_SAMPLES (768*2)
#define MAX_VBI_X_SAMPLES (1135*2)

#define MIN_VBI_Y_SAMPLES (1)
#define AVG_VBI_Y_SAMPLES (12)  
#define MAX_VBI_Y_SAMPLES (21)

#define MIN_VBI_T_SAMPLES (50)
#define AVG_VBI_T_SAMPLES (59.94)
#define MAX_VBI_T_SAMPLES (60)

#define NTSC_FSC_FREQUENCY  3580000
#define PAL_FSC_FREQUENCY   4430000

#define MIN_SAMPLING_RATE   (min(8*NTSC_FSC_FREQUENCY,8*PAL_FSC_FREQUENCY))
#define AVG_SAMPLING_RATE   (8*NTSC_FSC_FREQUENCY)
#define MAX_SAMPLING_RATE   (max(8*NTSC_FSC_FREQUENCY,8*PAL_FSC_FREQUENCY))

#if 0

// Our "preferred" VBI format, not likely to be the one we actually receive.

KS_DATARANGE_VIDEO_VBI StreamFormatVBI =
{
   // KSDATARANGE
   {
      {
         sizeof( KS_DATARANGE_VIDEO_VBI ),
         0,
         0, // SampleSize (Warning, this will not be constant from all decoders!!!)
         0, // Reserved
         { STATIC_KSDATAFORMAT_TYPE_VBI },
         //{ STATIC_KSDATAFORMAT_SUBTYPE_WILDCARD },
         //{ STATIC_KSDATAFORMAT_SUBTYPE_UNKNOWN },
	 { STATIC_KSDATAFORMAT_SUBTYPE_RAW8 },
         { STATIC_KSDATAFORMAT_SPECIFIER_VBI }
      }
   },
   TRUE,    // BOOL,  bFixedSizeSamples (all samples same size?)
   TRUE,    // BOOL,  bTemporalCompression (all I frames?)

   KS_VIDEOSTREAM_VBI, // StreamDescriptionFlags  (KS_VIDEO_DESC_*)
   0,       // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

   // _KS_VIDEO_STREAM_CONFIG_CAPS
   {
      { STATIC_KSDATAFORMAT_SPECIFIER_VBI },
      KS_AnalogVideo_NTSC_M,                       // AnalogVideoStandard
      {
         AVG_VBI_X_SAMPLES, AVG_VBI_Y_SAMPLES   // SIZE InputSize
      },
      {
         MIN_VBI_X_SAMPLES, MIN_VBI_Y_SAMPLES   // SIZE MinCroppingSize;       smallest rcSrc cropping rect allowed
      },
      {
         MAX_VBI_X_SAMPLES, MAX_VBI_Y_SAMPLES   // SIZE MaxCroppingSize;       largest rcSrc cropping rect allowed
      },
      1,           // int CropGranularityX;       // granularity of cropping size
      1,           // int CropGranularityY;
      1,           // int CropAlignX;             // alignment of cropping rect
      1,           // int CropAlignY;
      {
         MIN_VBI_X_SAMPLES, MIN_VBI_Y_SAMPLES   // SIZE MinOutputSize;         // smallest bitmap stream can produce
      },
      {
         MAX_VBI_X_SAMPLES, MAX_VBI_Y_SAMPLES   // SIZE MaxOutputSize;         // largest  bitmap stream can produce
      },
      1,          // int OutputGranularityX;     // granularity of output bitmap size
      2,          // int OutputGranularityY;
      0,          // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
      0,          // StretchTapsY
      0,          // ShrinkTapsX
      0,          // ShrinkTapsY

      (LONGLONG)(10000000.0/MIN_VBI_T_SAMPLES), // LONGLONG MinFrameInterval;  // 100 nS units
      (LONGLONG)(10000000.0/MAX_VBI_T_SAMPLES), // LONGLONG MaxFrameInterval;

      MIN_SAMPLING_RATE, // LONG MinBitsPerSecond
      MAX_SAMPLING_RATE, //LONG MaxBitsPerSecond;
   },

   // KS_VBIINFOHEADER (default format)
   {
      NORMAL_VBI_START_LINE,                        // StartLine  -- inclusive
      NORMAL_VBI_STOP_LINE,                         // EndLine    -- inclusive
      AVG_SAMPLING_RATE,                            // SamplingFrequency
      10,                                           // MinLineStartTime;
      10,                                           // MaxLineStartTime;
      10,                                           // ActualLineStartTime
      10,                                           // ActualLineEndTime;
      KS_AnalogVideo_NTSC_M,                        // VideoStandard;
      AVG_VBI_X_SAMPLES,                            // SamplesPerLine;
      AVG_VBI_X_SAMPLES,                            // StrideInBytes;
      AVG_VBI_X_SAMPLES*AVG_VBI_Y_SAMPLES           // BufferSize;
   }
};

#else

// This format is the "arbitrary one" that was used in early sample capture drivers!

//---------------------------------------------------------------------------
// All of the video and vbi data formats we might use
//---------------------------------------------------------------------------
#define VBISamples (800*2)
#define VBIStart   10
#define VBIEnd     21
KS_DATARANGE_VIDEO_VBI StreamFormatVBI =
{
   // KSDATARANGE
   {
      {
         sizeof( KS_DATARANGE_VIDEO_VBI ),
         0,
         VBISamples * 12,            // SampleSize
         0,                          // Reserved
         { STATIC_KSDATAFORMAT_TYPE_VBI },
         //{ STATIC_KSDATAFORMAT_SUBTYPE_WILDCARD },
	 { STATIC_KSDATAFORMAT_SUBTYPE_RAW8 },
         { STATIC_KSDATAFORMAT_SPECIFIER_VBI }
      }
   },
   TRUE,    // BOOL,  bFixedSizeSamples (all samples same size?)
   TRUE,    // BOOL,  bTemporalCompression (all I frames?)

   KS_VIDEOSTREAM_VBI, // StreamDescriptionFlags  (KS_VIDEO_DESC_*)
   0,       // MemoryAllocationFlags   (KS_VIDEO_ALLOC_*)

   // _KS_VIDEO_STREAM_CONFIG_CAPS
   {
      { STATIC_KSDATAFORMAT_SPECIFIER_VBI },
      KS_AnalogVideo_NTSC_M,                       // AnalogVideoStandard
      {
         VBISamples, 480  // SIZE InputSize
      },
      {
         VBISamples, 12   // SIZE MinCroppingSize;       smallest rcSrc cropping rect allowed
      },
      {
         VBISamples, 12   // SIZE MaxCroppingSize;       largest rcSrc cropping rect allowed
      },
      1,           // int CropGranularityX;       // granularity of cropping size
      1,           // int CropGranularityY;
      1,           // int CropAlignX;             // alignment of cropping rect
      1,           // int CropAlignY;
      {
         VBISamples, 12   // SIZE MinOutputSize;         // smallest bitmap stream can produce
      },
      {
         VBISamples, 12   // SIZE MaxOutputSize;         // largest  bitmap stream can produce
      },
      1,          // int OutputGranularityX;     // granularity of output bitmap size
      2,          // int OutputGranularityY;
      0,          // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
      0,          // StretchTapsY
      0,          // ShrinkTapsX
      0,          // ShrinkTapsY
      333667,     // LONGLONG MinFrameInterval;  // 100 nS units
      333667,     // LONGLONG MaxFrameInterval;
      VBISamples * 30 * 12, // LONG MinBitsPerSecond;
      VBISamples * 30 * 12 //LONG MaxBitsPerSecond;
   },

   // KS_VBIINFOHEADER (default format)
   {
      VBIStart,      // StartLine  -- inclusive
      VBIEnd,        // EndLine    -- inclusive
      28636360,      // SamplingFrequency; Hz.
      732,           // MinLineStartTime;
      732,           // MaxLineStartTime;
      732,           // ActualLineStartTime
      0,             // ActualLineEndTime;
      KS_AnalogVideo_NTSC_M,      // VideoStandard;
      VBISamples,           // SamplesPerLine;
      VBISamples,       // StrideInBytes;
      VBISamples * 12   // BufferSize;
   }
};

#endif

//---------------------------------------------------------------------------
//  STREAM_Capture Formats
//---------------------------------------------------------------------------

static PKSDATAFORMAT Stream0Formats[] = 
{
    (PKSDATAFORMAT) &StreamFormatVBI,

    // Add more formats here for to mirror output formats for "passthrough" mode
    // The idea is that upstream capture drivers may have done some decoding already
    // or downstream drivers may wish to have the raw data without any decoding at all.
    // In that case all we need to do is copy the data(if there is a pending SRB) OR
    // forward the SRB to the downstream client.
};
#define NUM_STREAM_0_FORMATS (SIZEOF_ARRAY(Stream0Formats))


//---------------------------------------------------------------------------
//  STREAM_Preview Formats
//---------------------------------------------------------------------------

static PKSDATAFORMAT Stream1Formats[] = 
{
    (PKSDATAFORMAT) &StreamFormatVBI,

    // Add more formats here for whatever CODEC output formats are supported.
    // This should always include StreamFormatVBI since someone may want raw VBI data
    // Same "passthrough" issues as Stream0Formats
};
#define NUM_STREAM_1_FORMATS (SIZEOF_ARRAY (Stream1Formats))

//---------------------------------------------------------------------------

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
  // STREAM_VBI
  // -----------------------------------------------------------------
  {
    // HW_STREAM_INFORMATION -------------------------------------------
    {
	1,                                      // NumberOfPossibleInstances
	KSPIN_DATAFLOW_IN,                      // DataFlow
	TRUE,                                   // DataAccessible
	NUM_STREAM_0_FORMATS,                   // NumberOfFormatArrayEntries
	Stream0Formats,                         // StreamFormatsArray
	0,                                      // ClassReserved[0]
	0,                                      // ClassReserved[1]
	0,                                      // ClassReserved[2]
	0,                                      // ClassReserved[3]
	NUMBER_VIDEO_STREAM_PROPERTIES,         // NumStreamPropArrayEntries
	VideoStreamProperties,                  // StreamPropertiesArray
	0,                                      // NumStreamEventArrayEntries
	0,                                      // StreamEventsArray
	(GUID *)&PINNAME_VIDEO_VBI,             // Category
	(GUID *)&PINNAME_VIDEO_VBI,             // Name
	0,                                      // MediumsCount
	NULL,                                   // Mediums
    },
           
    // HW_STREAM_OBJECT ------------------------------------------------
    {
	sizeof (HW_STREAM_OBJECT),              // SizeOfThisPacket
	0,                                      // StreamNumber
	(PVOID)NULL,                            // HwStreamExtension
	VideoReceiveDataPacket,                 // HwReceiveDataPacket
	VideoReceiveCtrlPacket,                 // HwReceiveControlPacket
	{                                       // HW_CLOCK_OBJECT
	    NULL,                                // .HWClockFunction
	    0,                                   // .ClockSupportFlags
	},
	FALSE,                                  // Dma
	TRUE,                                   // Pio
	(PVOID)NULL,                            // HwDeviceExtension
	sizeof (KS_VBI_FRAME_INFO),             // StreamHeaderMediaSpecific
	0,                                      // StreamHeaderWorkspace 
	TRUE,                                  // Allocator 
	NULL,                                   // HwEventRoutine
    },
  },


  // -----------------------------------------------------------------
  // STREAM_Decode
  // -----------------------------------------------------------------
  {
    // HW_STREAM_INFORMATION -------------------------------------------
    {
	MAX_PIN_INSTANCES,                      // NumberOfPossibleInstances
	KSPIN_DATAFLOW_OUT,                     // DataFlow
	TRUE,                                   // DataAccessible
	NUM_STREAM_1_FORMATS,                   // NumberOfFormatArrayEntries
	Stream1Formats,                         // StreamFormatsArray
	0,                                      // ClassReserved[0]
	0,                                      // ClassReserved[1]
	0,                                      // ClassReserved[2]
	0,                                      // ClassReserved[3]
	NUMBER_VIDEO_STREAM_PROPERTIES,         // NumStreamPropArrayEntries
	VideoStreamProperties,                  // StreamPropertiesArray
	0,                                      // NumStreamEventArrayEntries;
	0,                                      // StreamEventsArray;
	(GUID *)&PINNAME_VIDEO_VBI,             // Category
	(GUID *)&PINNAME_VIDEO_VBI,             // Name
	0,                                      // MediumsCount
	NULL,                                   // Mediums
    },
           
    // HW_STREAM_OBJECT ------------------------------------------------
    {
	sizeof (HW_STREAM_OBJECT),              // SizeOfThisPacket
	1,                                      // StreamNumber
	(PVOID)NULL,                            // HwStreamExtension
	VideoReceiveDataPacket,                 // HwReceiveDataPacket
	VideoReceiveCtrlPacket,                 // HwReceiveControlPacket
	{                                       // HW_CLOCK_OBJECT
	    NULL,                                // .HWClockFunction
	    0,                                   // .ClockSupportFlags
	},
	FALSE,                                  // Dma
	TRUE,                                   // Pio
	(PVOID)NULL,                            // HwDeviceExtension
	sizeof (KS_VBI_FRAME_INFO),             // StreamHeaderMediaSpecific
	0,                                      // StreamHeaderWorkspace 
	TRUE,                                  // Allocator 
	NULL,                                   // HwEventRoutine
    },
  },
};

#define DRIVER_STREAM_COUNT (SIZEOF_ARRAY (Streams))


//---------------------------------------------------------------------------
// Topology
//---------------------------------------------------------------------------

// Categories define what the device does.

static GUID Categories[] = {
    STATIC_KSCATEGORY_VBICODEC
    // 0x7dad660, 0x22f1, 0x11d1, {0xa9, 0xf4, 0, 0xc0, 0x4f, 0xbb, 0xde, 0x8f}
};

#define NUMBER_OF_CATEGORIES  SIZEOF_ARRAY (Categories)

static KSTOPOLOGY Topology = {
    NUMBER_OF_CATEGORIES,
    (GUID*) &Categories,
    0,
    (GUID*) NULL,
    0,
    NULL
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

