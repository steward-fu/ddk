
#ifndef _PROPDATA2_H
#define _PROPDATA2_H

// ------------------------------------------------------------------------
//  T I    D i g i t a l    C a m e r a
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// Property set for VideoProcAmp
// ------------------------------------------------------------------------

// Default values for some of the properties

#define TIDCAM_DEF_BRIGHTNESS      120
#define TIDCAM_DEF_SHARPNESS         2
#define TIDCAM_DEF_WHITEBALANCE    112
#define TIDCAM_DEF_FOCUS            80


//
// First define all of the ranges and stepping values
//

// ------------------------------------------------------------------------
static KSPROPERTY_STEPPING_LONG BrightnessRangeAndStep2 [] = 
{
    {
        1,                  // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (IRE * 100) units
        255                 // Maximum in (IRE * 100) units
    }
};

const static LONG BrightnessDefault2 = TIDCAM_DEF_BRIGHTNESS;


static KSPROPERTY_MEMBERSLIST BrightnessMembersList2 [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (BrightnessRangeAndStep2),
            SIZEOF_ARRAY (BrightnessRangeAndStep2),
            0
        },
        (PVOID) BrightnessRangeAndStep2,
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (BrightnessDefault2),
            sizeof (BrightnessDefault2),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &BrightnessDefault2,
    }    
};

static KSPROPERTY_VALUES BrightnessValues2 =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (BrightnessMembersList2),
    BrightnessMembersList2
};

// ------------------------------------------------------------------------
static KSPROPERTY_STEPPING_LONG SharpnessRangeAndStep2 [] = 
{
    {
        1,                  // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (gain * 100) units
        2                   // Maximum in (gain * 100) units
    }
};

const static LONG SharpnessDefault2 = TIDCAM_DEF_SHARPNESS;


static KSPROPERTY_MEMBERSLIST SharpnessMembersList2 [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (SharpnessRangeAndStep2),
            SIZEOF_ARRAY (SharpnessRangeAndStep2),
            0
        },
        (PVOID) SharpnessRangeAndStep2
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (SharpnessDefault2),
            sizeof (SharpnessDefault2),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &SharpnessDefault2,
    }    
};

static KSPROPERTY_VALUES SharpnessValues2 =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (SharpnessMembersList2),
    SharpnessMembersList2
};

// ------------------------------------------------------------------------
static KSPROPERTY_STEPPING_LONG HueRangeAndStep2 [] = 
{
    {
        1,                  // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (gain * 100) units
        10000               // Maximum in (gain * 100) units
    }
};

static LONG HueDefault2 = 5;


static KSPROPERTY_MEMBERSLIST HueMembersList2 [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (HueRangeAndStep2),
            SIZEOF_ARRAY (HueRangeAndStep2),
            0
        },
        (PVOID) HueRangeAndStep2
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (HueDefault2),
            sizeof (HueDefault2),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &HueDefault2,
    }    
};

static KSPROPERTY_VALUES HueValues2 =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (HueMembersList2),
    HueMembersList2
};

// ------------------------------------------------------------------------
static KSPROPERTY_STEPPING_LONG SaturationRangeAndStep2 [] = 
{
    {
        1,                  // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (gain * 100) units
        10000               // Maximum in (gain * 100) units
    }
};

static LONG SaturationDefault2 = 5;


static KSPROPERTY_MEMBERSLIST SaturationMembersList2 [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (SaturationRangeAndStep2),
            SIZEOF_ARRAY (SaturationRangeAndStep2),
            0
        },
        (PVOID) SaturationRangeAndStep2
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (SaturationDefault2),
            sizeof (SaturationDefault2),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &SaturationDefault2,
    }    
};

static KSPROPERTY_VALUES SaturationValues2 =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (SaturationMembersList2),
    SaturationMembersList2
};

// ------------------------------------------------------------------------
static KSPROPERTY_STEPPING_LONG WhiteBalanceRangeAndStep2 [] = 
{
    {
        1,                  // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (gain * 100) units
        255                 // Maximum in (gain * 100) units
    }
};

const static LONG WhiteBalanceDefault2 = TIDCAM_DEF_WHITEBALANCE;


static KSPROPERTY_MEMBERSLIST WhiteBalanceMembersList2 [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (WhiteBalanceRangeAndStep2),
            SIZEOF_ARRAY (WhiteBalanceRangeAndStep2),
            0
        },
        (PVOID) WhiteBalanceRangeAndStep2
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (WhiteBalanceDefault2),
            sizeof (WhiteBalanceDefault2),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &WhiteBalanceDefault2,
    }    
};

static KSPROPERTY_VALUES WhiteBalanceValues2 =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (WhiteBalanceMembersList2),
    WhiteBalanceMembersList2
};

// ------------------------------------------------------------------------
static KSPROPERTY_STEPPING_LONG FocusRangeAndStep2 [] = 
{
    {
        1,                  // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (IRE * 100) units
        165                 // Maximum in (IRE * 100) units
    }
};

const static LONG FocusDefault2 = TIDCAM_DEF_FOCUS;


static KSPROPERTY_MEMBERSLIST FocusMembersList2 [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (FocusRangeAndStep2),
            SIZEOF_ARRAY (FocusRangeAndStep2),
            0
        },
        (PVOID) FocusRangeAndStep2,
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (FocusDefault2),
            sizeof (FocusDefault2),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &FocusDefault2,
    }    
};

static KSPROPERTY_VALUES FocusValues2 =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (FocusMembersList2),
    FocusMembersList2
};

// ------------------------------------------------------------------------
static KSPROPERTY_STEPPING_LONG ZoomRangeAndStep2 [] = 
{
    {
        1,                  // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (IRE * 100) units
        100                 // Maximum in (IRE * 100) units
    }
};

static LONG ZoomDefault2 = 4;


static KSPROPERTY_MEMBERSLIST ZoomMembersList2 [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (ZoomRangeAndStep2),
            SIZEOF_ARRAY (ZoomRangeAndStep2),
            0
        },
        (PVOID) ZoomRangeAndStep2,
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (ZoomDefault2),
            sizeof (ZoomDefault2),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &ZoomDefault2,
    }    
};

static KSPROPERTY_VALUES ZoomValues2 =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (ZoomMembersList2),
    ZoomMembersList2
};

// ------------------------------------------------------------------------
DEFINE_KSPROPERTY_TABLE(VideoProcAmpProperties2)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinData
        TRUE,                                   // SetSupported or Handler
        &BrightnessValues2,                     // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),

    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOPROCAMP_SHARPNESS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinData
        TRUE,                                   // SetSupported or Handler
        &SharpnessValues2,                      // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),

    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOPROCAMP_HUE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinData
        TRUE,                                   // SetSupported or Handler
        &HueValues2,                            // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),    

	DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOPROCAMP_SATURATION,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinData
        TRUE,                                   // SetSupported or Handler
        &SaturationValues2,                     // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),

	DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOPROCAMP_WHITEBALANCE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinData
        TRUE,                                   // SetSupported or Handler
        &WhiteBalanceValues2,                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
};

DEFINE_KSPROPERTY_TABLE(CameraControlProperties2)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_FOCUS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CAMERACONTROL_S),     // MinProperty
        sizeof(KSPROPERTY_CAMERACONTROL_S),     // MinData
        TRUE,                                   // SetSupported or Handler
        &FocusValues2,                           // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_ZOOM,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CAMERACONTROL_S),     // MinProperty
        sizeof(KSPROPERTY_CAMERACONTROL_S),     // MinData
        TRUE,                                   // SetSupported or Handler
        &ZoomValues2,                           // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
};


// ------------------------------------------------------------------------
// Array of all of the property sets supported by the adapter
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_SET_TABLE(AdapterPropertyTable2)
{
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_VIDEOPROCAMP,
        SIZEOF_ARRAY(VideoProcAmpProperties),
        VideoProcAmpProperties2,
        0, 
        NULL,
    ),

    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_CAMERACONTROL,
        SIZEOF_ARRAY(CameraControlProperties2),
        CameraControlProperties2,
        0, 
        NULL,
    )
};

#define NUMBER_OF_ADAPTER_PROPERTY_SETS2 (SIZEOF_ARRAY (AdapterPropertyTable2))


#endif
