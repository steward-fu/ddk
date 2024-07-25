// HwcapFormats.h: Data Format Definitions for HwcapPin class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HWCAPPINFORMATS_H__3B41C636_4EB6_47FC_A42C_F18BA23834BF__INCLUDED_)
#define AFX_HWCAPPINFORMATS_H__3B41C636_4EB6_47FC_A42C_F18BA23834BF__INCLUDED_

//
// Video Data Formats - the content is as per XP DDK Avssamp sample
//


// RGB24 - generates RGB images

const KS_DATARANGE_VIDEO HwcapPin::sm_RGB24_Capture = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),                // FormatSize
        0,                                          // Flags
        sm_X * sm_Y * 3,              // SampleSize
        0,                                          // Reserved

        STATICGUIDOF( KSDATAFORMAT_TYPE_VIDEO ),    // aka. MEDIATYPE_Video
        0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
        STATICGUIDOF( KSDATAFORMAT_SPECIFIER_VIDEOINFO ) // aka. FORMAT_VideoInfo
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    0,                  // Reserved (was StreamDescriptionFlags)
    0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATICGUIDOF( KSDATAFORMAT_SPECIFIER_VIDEOINFO ), // GUID
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
        640000000,      // MaxFrameInterval, 100 nS units
        8 * 3 * 30 * 160 * 120,  // MinBitsPerSecond;
        8 * 3 * 30 * 720 * 480   // MaxBitsPerSecond;
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                            // RECT  rcSource; 
        0,0,0,0,                            // RECT  rcTarget; 
        sm_X * sm_Y * 3 * 30,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        333667,                             // REFERENCE_TIME  AvgTimePerFrame;   
        sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
        sm_X,                                // LONG  biWidth;
        sm_Y,                                // LONG  biHeight;
        1,                                  // WORD  biPlanes;
        24,                                 // WORD  biBitCount;
        KS_BI_RGB,                          // DWORD biCompression;
        sm_X * sm_Y * 3,                      // DWORD biSizeImage;
        0,                                  // LONG  biXPelsPerMeter;
        0,                                  // LONG  biYPelsPerMeter;
        0,                                  // DWORD biClrUsed;
        0                                   // DWORD biClrImportant;
    }
}; 

// Generates YUV Images

const  KS_DATARANGE_VIDEO  HwcapPin::sm_YUV_Capture = 
{
    // KSDATARANGE
    {   
        sizeof (KS_DATARANGE_VIDEO),            // FormatSize
        0,                                      // Flags
        sm_X * sm_Y * 2,                          // SampleSize
        0,                                      // Reserved
        STATICGUIDOF( KSDATAFORMAT_TYPE_VIDEO ),         // aka. MEDIATYPE_Video
        0x59565955, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71, //MEDIASUBTYPE_UYVY,
        STATICGUIDOF( KSDATAFORMAT_SPECIFIER_VIDEOINFO ) // aka. FORMAT_VideoInfo
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    0,                  // Reserved (was StreamDescriptionFlags)
    0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    {
        STATICGUIDOF( KSDATAFORMAT_SPECIFIER_VIDEOINFO ), // GUID
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
        640000000,      // MaxFrameInterval, 100 nS units
        8 * 2 * 30 * 160 * 120,  // MinBitsPerSecond;
        8 * 2 * 30 * 720 * 480   // MaxBitsPerSecond;
    }, 
        
    // KS_VIDEOINFOHEADER (default format)
    {
        0,0,0,0,                            // RECT  rcSource; 
        0,0,0,0,                            // RECT  rcTarget; 
        sm_X * sm_Y * 2 * 30,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        333667,                             // REFERENCE_TIME  AvgTimePerFrame;   
        sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
        sm_X,                                // LONG  biWidth;
        sm_Y,                                // LONG  biHeight;
        1,                                  // WORD  biPlanes;
        16,                                 // WORD  biBitCount;
        FOURCC_YUV422,                      // DWORD biCompression;
        sm_X * sm_Y * 2,                      // DWORD biSizeImage;
        0,                                  // LONG  biXPelsPerMeter;
        0,                                  // LONG  biYPelsPerMeter;
        0,                                  // DWORD biClrUsed;
        0                                   // DWORD biClrImportant;
    }
}; 
    


#endif // !defined(AFX_HWCAPPINFORMATS_H__3B41C636_4EB6_47FC_A42C_F18BA23834BF__INCLUDED_)
