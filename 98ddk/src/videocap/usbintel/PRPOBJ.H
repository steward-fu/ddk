#ifndef __PRPOBJ_H__
#define __PRPOBJ_H__

#if defined( mmioFOURCC )
    #undef mmioFOURCC
#endif // mmioFOURCC

#include "windef.h"
#include "mmsystem.h"



//
// for set/get props
//
#define CURRENT_VALUE 0
#define DEFAULT_VALUE 4



////////////////
/////
/////  The following are IRU Units wherever possible.  A mapping between
/////  these and the camera properties will be defined later in this file.
/////  The values are however based on the camera.
/////
////////////////
#define MAX_BRIGHTNESS_IRE_UNITS   119      // approx. measured value on UCAM1
#define STEPS_BRIGHTNESS 10 
#define STEPPING_DELTA_BRIGHTNESS    \
           ((MAX_BRIGHTNESS_IRE_UNITS+1)/STEPS_BRIGHTNESS)


#define MS_PROPERTYSET_INDEX    0
#define MANF_PROPERTYSET_INDEX  1

//
//  Custom properties
//
typedef enum _REQUEST_CUSTOM {
    FIRMWARE_VERSION = 3,
} REQUEST_CUSTOM;


#define MANF_NUMBEROFPROPERTIES 1


extern GUID PROPSETID_INTEL_CUSTOM_PROP;

typedef enum {
    KSPROPERTY_CUSTOM_PROP_FIRMWARE_VER            // RW O
} KSPROPERTY_CUSTOM_PROP;


typedef struct {
    KSPROPERTY Property;
    ULONG  Instance;                    // 0 for first PropAmp, 1 for 2nd, ...
    LONG   Value;                       // Value to set or get
    ULONG  Flags;                       // KSPROPERTY_CUSTOM_PROP_FLAGS_*
    ULONG  Capabilities;                // KSPROPERTY_CUSTOM_PROP_FLAGS_*
} KSPROPERTY_CUSTOM_PROP_S, *PKSPROPERTY_CUSTOM_PROP_S;

#define KSPROPERTY_CUSTOM_PROP_FLAGS_MANUAL      0X0000L
#define KSPROPERTY_CUSTOM_PROP_FLAGS_AUTO        0X0001L
#define KSPROPERTY_CUSTOM_PROP_FLAGS_DEFAULT     0X0004L





//
// Macros for packing/unpacking pan/scale.
//
#define MakeULong(l,h) ((unsigned long)(((unsigned long)(l)) | ((unsigned long)((unsigned short)(h))) << 16))
#define LoUShort(r) ((unsigned short)(r))
#define HiUShort(r) ((unsigned short)(((unsigned long)(r)) >> 16))
#define PackUShortAsULong(us,ul) \
    ul = us >> 8; \
    ul <<= 16; \
    ul |= (unsigned char)us
#define PackULongAsUShort(ul,us) \
    us = (unsigned short)ul; \
    us |= (unsigned short)(ul >> 8)

//
// Format four character codes.
//
#define FCC_FORMAT_YUV12N mmioFOURCC('I','Y','U','V')
#define FCC_FORMAT_YUV12A mmioFOURCC('I','4','2','0')

//
// Format.
//
typedef struct _FORMAT {
    FOURCC Fcc;
    LONG lWidth;
    LONG lHeight;
} FORMAT;

//
// Generic range of control values.
//
typedef struct _RANGE {
    union {
        LONG Min;
        LONG NegX;
    };
    union {
        LONG Max;
        LONG PosX;
    };
    union {
        LONG PMin;
        LONG NegY;
    };
    union {
        LONG PMax;
        LONG PosY;
    };
    ULONG Step;
    const LONG Default;
} RANGE;

//
// Control request IDs.
//
typedef enum _REQUEST {
    REQ_BRIGHTNESS      = 1,
    REQ_DIB             = 2,
    REQ_ENHANCEMENT     = 3,
    REQ_EXPOSURE        = 4,
    REQ_FORMAT          = 5,
    REQ_PAN             = 6,
    REQ_RATE            = 7,
    REQ_SATURATION      = 8,
    REQ_SCALE           = 9,
    REQ_SEEK            = 10,
    REQ_WHITEBALANCE    = 11
} REQUEST;


typedef enum _ATTRIBUTE {
    ATTR_DURATION       = 1,
    ATTR_DIB            = 2,
    ATTR_FORMAT         = 3,
    ATTR_RATIO          = 4,
    ATTR_NEGX           = 5,
    ATTR_NEGY           = 6
} ATTRIBUTE;

//
// Scale table
//
typedef struct _SCALE {
    ULONG Setting;              // Device setting
    ULONG Ratio;                // Scale ratio
    LONG ResX;                  // Scale x resolution
    LONG ResY;                  // Scale y resolution
    LONG NegX;                  // Negative x pan range
    LONG PosX;                  // Positive x pan range
    LONG NegY;                  // Negative y pan range
    LONG PosY;                  // Positive y pan range
    SHORT DefXPan;              // Default x pan
    SHORT DefYPan;              // Default y pan
    BOOL Default;               // Default scale flag
} SCALE;

//
// Seek interval.
//
typedef struct _SEEK {
    ULONG Setting;
    ULONG Duration;
} SEEK;

//
// Defines all modifiable properties.
//
typedef struct _USBCAMD_PROPERTY {
    FORMAT Format;          // Current format
    LONGLONG Rate;             // Current frame rate

    ULONG ScaleRatio;       // Current scale ratio
    ULONG SeekDuration;     // Current seek duration
    ULONG PanPos;           // Current pan position
    SHORT ScaleNegX;        // Current negative x pan range
    SHORT ScaleNegY;        // Current negative y pan range

    //
    // Actually used only for SetPropertyDefaults() and not for
    // current values now.  
    //
    ULONG Brightness;       // Current brightness
    ULONG Saturation;       // Current saturation
    ULONG Exposure;         // Current exposure
    ULONG Enhancement;      // Current enhancement
    ULONG WhiteBalance;     // Current white balance

} USBCAMD_PROPERTY;



//
// (Usec) Frame rate macro.
//
#define MICROSEC_PERFRAME(x)    ((x > 0) ? ((LONGLONG)1000000/x) : 0)
#define NUM_100NANOSEC_UNITS_PERFRAME(x) ((x > 0) ? ((LONGLONG)10000000 / x) :0 )

#define DRIVER_DATA_DONTCARE 0xffffffff
#define FRAME_RATE_LIST_SIZE   8


extern const RANGE _Brightness;
extern const RANGE _Enhancement;
extern const RANGE _Exposure;
extern const RANGE _Saturation;
extern const SEEK _Seek[];
extern const RANGE _WhiteBalance;

extern LONGLONG SupportedFrameRates[FRAME_RATE_LIST_SIZE];


#endif
