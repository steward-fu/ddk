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

#ifndef __CAPPROP_H
#define __CAPPROP_H

// ------------------------------------------------------------------------
// Property set for the VideoPort
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_TABLE(VideoPortConfiguration)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_NUMCONNECTINFO,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_NUMVIDEOFORMAT,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_GETCONNECTINFO,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSMULTIPLE_DATA_PROP),           // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_SETCONNECTINFO,
        FALSE,                                  // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(ULONG),                          // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_VPDATAINFO,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KS_AMVPDATAINFO),                // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_MAXPIXELRATE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSVPSIZE_PROP),                  // MinProperty
        sizeof(KSVPMAXPIXELRATE),               // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_INFORMVPINPUT,
        TRUE,                                   // GetSupported or Handler
        sizeof(PKSPROPERTY),                    // MinProperty
        sizeof(DDPIXELFORMAT),                  // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_DDRAWHANDLE,
        (PFNKSHANDLER)FALSE,
        sizeof (PKSPROPERTY),
        sizeof (ULONG),    // could be 0 too
        (PFNKSHANDLER) TRUE,
        NULL,
        0,
        NULL,
        NULL,
        0
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_VIDEOPORTID,
        (PFNKSHANDLER)FALSE,
        sizeof (PKSPROPERTY),
        sizeof (ULONG),    // could be 0 too
        (PFNKSHANDLER) TRUE,
        NULL,
        0,
        NULL,
        NULL,
        0
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_DDRAWSURFACEHANDLE,
        (PFNKSHANDLER)FALSE,
        sizeof (PKSPROPERTY),
        sizeof (ULONG),    // could be 0 too
        (PFNKSHANDLER) TRUE,
        NULL,
        0,
        NULL,
        NULL,
        0
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_GETVIDEOFORMAT,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSMULTIPLE_DATA_PROP),           // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_SETVIDEOFORMAT,
        FALSE,                                  // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(ULONG),                          // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_INVERTPOLARITY,
        FALSE,                                  // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        0,                                      // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_SURFACEPARAMS,
        FALSE,                                  // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KSVPSURFACEPARAMS),              // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_DECIMATIONCAPABILITY,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(BOOL),                           // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_SCALEFACTOR,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KS_AMVPSIZE),                    // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
};

DEFINE_KSPROPERTY_SET_TABLE(VideoPortPropertyTable)
{
    DEFINE_KSPROPERTY_SET
    ( 
        &KSPROPSETID_VPConfig,                  // Set
        SIZEOF_ARRAY(VideoPortConfiguration),   // PropertiesCount
        VideoPortConfiguration,                 // PropertyItem
        0,                                      // FastIoCount
        NULL                                    // FastIoTable
    )
};

// ------------------------------------------------------------------------
// Property set for the VideoPort VBI stream
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_TABLE(VideoPortVBIConfiguration)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_NUMCONNECTINFO,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_NUMVIDEOFORMAT,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_GETCONNECTINFO,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSMULTIPLE_DATA_PROP),           // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_SETCONNECTINFO,
        FALSE,                                  // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(ULONG),                          // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_VPDATAINFO,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KS_AMVPDATAINFO),                // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_MAXPIXELRATE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSVPSIZE_PROP),                  // MinProperty
        sizeof(KSVPMAXPIXELRATE),               // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_INFORMVPINPUT,
        TRUE,                                   // GetSupported or Handler
        sizeof(PKSPROPERTY),                    // MinProperty
        sizeof(DDPIXELFORMAT),                  // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_DDRAWHANDLE,
        (PFNKSHANDLER)FALSE,
        sizeof (PKSPROPERTY),
        sizeof (ULONG),    // could be 0 too
        (PFNKSHANDLER) TRUE,
        NULL,
        0,
        NULL,
        NULL,
        0
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_VIDEOPORTID,
        (PFNKSHANDLER)FALSE,
        sizeof (PKSPROPERTY),
        sizeof (ULONG),    // could be 0 too
        (PFNKSHANDLER) TRUE,
        NULL,
        0,
        NULL,
        NULL,
        0
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_DDRAWSURFACEHANDLE,
        (PFNKSHANDLER)FALSE,
        sizeof (PKSPROPERTY),
        sizeof (ULONG),    // could be 0 too
        (PFNKSHANDLER) TRUE,
        NULL,
        0,
        NULL,
        NULL,
        0
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_GETVIDEOFORMAT,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSMULTIPLE_DATA_PROP),           // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_SETVIDEOFORMAT,
        FALSE,                                  // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(ULONG),                          // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_INVERTPOLARITY,
        FALSE,                                  // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        0,                                      // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_SURFACEPARAMS,
        FALSE,                                  // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KSVPSURFACEPARAMS),              // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_DECIMATIONCAPABILITY,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(BOOL),                           // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VPCONFIG_SCALEFACTOR,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KS_AMVPSIZE),                    // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
};

DEFINE_KSPROPERTY_SET_TABLE(VideoPortVBIPropertyTable)
{
    DEFINE_KSPROPERTY_SET
    ( 
        &KSPROPSETID_VPVBIConfig,               // Set
        SIZEOF_ARRAY(VideoPortVBIConfiguration),// PropertiesCount
        VideoPortVBIConfiguration,              // PropertyItem
        0,                                      // FastIoCount
        NULL                                    // FastIoTable
    )
};

// ------------------------------------------------------------------------
// Property set for the Video Crossbar
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_TABLE(XBarProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CROSSBAR_CAPS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CROSSBAR_CAPS_S),    // MinProperty
        sizeof(KSPROPERTY_CROSSBAR_CAPS_S),    // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CROSSBAR_CAN_ROUTE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CROSSBAR_ROUTE_S),    // MinProperty
        sizeof(KSPROPERTY_CROSSBAR_ROUTE_S),    // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CROSSBAR_ROUTE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CROSSBAR_ROUTE_S),    // MinProperty
        sizeof(KSPROPERTY_CROSSBAR_ROUTE_S),    // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CROSSBAR_PININFO,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CROSSBAR_PININFO_S),    // MinProperty
        sizeof(KSPROPERTY_CROSSBAR_PININFO_S),    // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),

};


// ------------------------------------------------------------------------
// Property set for VideoProcAmp
// ------------------------------------------------------------------------

// defaults
static const ULONG BrightnessDefault = 128;
static const ULONG ContrastDefault = 128;
static const ULONG HueDefault = 128;
static const ULONG SaturationDefault = 128;

//
// First define all of the ranges and stepping values
//

// ------------------------------------------------------------------------
static KSPROPERTY_STEPPING_LONG BrightnessRangeAndStep [] = 
{
    {
        // Eventually need to convert these to IRE * 100 unites
        256/1,              // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum
        255                 // Maximum
    }
};

static KSPROPERTY_MEMBERSLIST BrightnessMembersList [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (BrightnessRangeAndStep),
            SIZEOF_ARRAY (BrightnessRangeAndStep),
            0
        },
        (PVOID) BrightnessRangeAndStep
    },
    {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (BrightnessDefault),
            sizeof (BrightnessDefault),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &BrightnessDefault
    }
};

static KSPROPERTY_VALUES BrightnessValues =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (BrightnessMembersList),
    BrightnessMembersList
};

// ------------------------------------------------------------------------
static KSPROPERTY_STEPPING_LONG ContrastRangeAndStep [] = 
{
    {
        256/1,        // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (gain * 100) units
        255               // Maximum in (gain * 100) units
    }
};

static KSPROPERTY_MEMBERSLIST ContrastMembersList [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (ContrastRangeAndStep),
            SIZEOF_ARRAY (ContrastRangeAndStep),
            0
        },
        (PVOID) ContrastRangeAndStep
    },
    {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (ContrastDefault),
            sizeof (ContrastDefault),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &ContrastDefault
    }    
};

static KSPROPERTY_VALUES ContrastValues =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (ContrastMembersList),
    ContrastMembersList
};

KSPROPERTY_STEPPING_LONG HueRangeAndStep [] = 
{
    {
        256/1,        // SteppingDelta
        0,                                // Reserved
        0,                                // Minimum 
        255         // Maximum 
    }
};

KSPROPERTY_MEMBERSLIST HueMembersList [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (HueRangeAndStep),
            SIZEOF_ARRAY (HueRangeAndStep),
            0
        },
        (PVOID) HueRangeAndStep
    },
    {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (HueDefault),
            sizeof (HueDefault),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &HueDefault
    }
};

KSPROPERTY_VALUES HueValues =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (HueMembersList),
    HueMembersList
};

KSPROPERTY_STEPPING_LONG SaturationRangeAndStep [] = 
{
    {
        256/1,        // SteppingDelta
        0,                                // Reserved
        0,                                // Minimum 
        255         // Maximum 
    }
};

KSPROPERTY_MEMBERSLIST SaturationMembersList [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (SaturationRangeAndStep),
            SIZEOF_ARRAY (SaturationRangeAndStep),
            0
        },
        (PVOID) SaturationRangeAndStep
    },
    {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (SaturationDefault),
            sizeof (SaturationDefault),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &SaturationDefault
    }    
};

KSPROPERTY_VALUES SaturationValues =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (SaturationMembersList),
    SaturationMembersList
};

// ------------------------------------------------------------------------
DEFINE_KSPROPERTY_TABLE(VideoProcAmpProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOPROCAMP_CONTRAST,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinData
        TRUE,                                   // SetSupported or Handler
        &ContrastValues,                        // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEOPROCAMP_S),      // MinData
        TRUE,                                   // SetSupported or Handler
        &BrightnessValues,                       // Values
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
        &HueValues,                       // Values
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
        &SaturationValues,                       // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
};

// ------------------------------------------------------------------------
// Property set for AnalogVideoDecoder
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_TABLE(AnalogVideoDecoder)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEODECODER_CAPS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEODECODER_CAPS_S), // MinProperty
        sizeof(KSPROPERTY_VIDEODECODER_CAPS_S), // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEODECODER_STANDARD,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEODECODER_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEODECODER_S),      // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEODECODER_STATUS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEODECODER_STATUS_S),// MinProperty
        sizeof(KSPROPERTY_VIDEODECODER_STATUS_S),// MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEODECODER_OUTPUT_ENABLE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEODECODER_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEODECODER_S),      // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
};

// ------------------------------------------------------------------------
// Array of all of the property sets supported by the adapter
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_SET_TABLE(AdapterPropertyTable)
{
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_VIDEOPROCAMP,
        SIZEOF_ARRAY(VideoProcAmpProperties),
        VideoProcAmpProperties,
        0, 
        NULL,
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_CROSSBAR,             // Set
        SIZEOF_ARRAY(XBarProperties),           // PropertiesCount
        XBarProperties,                         // PropertyItem
        0,                                      // FastIoCount
        NULL                                    // FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_VIDEODECODER,
        SIZEOF_ARRAY(AnalogVideoDecoder),
        AnalogVideoDecoder,
        0, 
        NULL,
    )
};

#define NUMBER_OF_ADAPTER_PROPERTY_SETS (SIZEOF_ARRAY (AdapterPropertyTable))

VOID AdapterSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
VOID AdapterGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

#endif // __CAPPROP_H