// controls.cpp - control classes for Video Capture sample
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

//
// This file handles all adapter property sets
//

#include <vdw.h>
#include "kstream.h"
#include "capxfer.h"
#include "controls.h"
#define DEFINE_MEDIUMS
#include "mediums.h"
#undef	DEFINE_MEDIUMS

// -------------------------------------------------------------------
// A few notes about property set handling
//  
// Property sets used in Testcap are of two varieties, those that have
// default values, ranges, and stepping, such as VideoProcAmp and CameraControl,
// and those which don't have defaults and ranges, such as TVTuner and
// Crossbar.
// 
// Default values and stepping are established by tables in capprop.h,
// no code is required to implement this other than initally creating the tables.
// 
// Many of the property sets require the ability to modify a number
// of input parameters.  Since KS doesn't allow this inherently, you'll
// note that some property sets require copying the provided input parameters
// to the ouput parameter list, effectively creating a "read, modify, write"
// capability.  For this reason, the input and output parameter lists
// use identical structures.
//
// On an SRB_GET_DEVICE_PROPERTY, read-only input data to the driver is provided as:
//      pSrb->CommandData.PropertyInfo
//
// ... while the output data pointer is:
//      pSrb->CommandData.PropertyInfo.PropertyInfo
// 
// -------------------------------------------------------------------


Crossbar::XBAR_PIN_DESCRIPTION Crossbar::XBarInputPins[] = {

	// First list the video input pins, then the audio inputs, then the output pins
	// Note that audio pin index 6 is shared between two video inputs (index 1 and index 2)
	

	//    PinType                       SynthImageCommand                     RelatedPinIndex   Medium
	/*0*/ KS_PhysConn_Video_Tuner,         IMAGE_XFER_NTSC_EIA_100AMP_100SAT,    5,             &CrossbarMediums[0],
	/*1*/ KS_PhysConn_Video_Composite,     IMAGE_XFER_NTSC_EIA_75AMP_100SAT,     6,             &CrossbarMediums[1],
	/*2*/ KS_PhysConn_Video_SVideo,        IMAGE_XFER_BLACK,                     6,             &CrossbarMediums[2],
	/*3*/ KS_PhysConn_Video_Tuner,         IMAGE_XFER_WHITE,                     7,             &CrossbarMediums[3],
	/*4*/ KS_PhysConn_Video_Composite,     IMAGE_XFER_GRAY_INCREASING,           8,             &CrossbarMediums[4],
	
	/*5*/ KS_PhysConn_Audio_Tuner,         0,                                    0,             &CrossbarMediums[5],
	/*6*/ KS_PhysConn_Audio_Line,          0,                                    1,             &CrossbarMediums[6],
	/*7*/ KS_PhysConn_Audio_Tuner,         0,                                    3,             &CrossbarMediums[7],
	/*8*/ KS_PhysConn_Audio_Line,          0,                                    4,             &CrossbarMediums[8],

};

Crossbar::XBAR_PIN_DESCRIPTION Crossbar::XBarOutputPins[] = {

	//    PinType                       SynthImageCommand                     RelatedPinIndex

	/*0*/ KS_PhysConn_Video_VideoDecoder,  0,                                    1,             &CrossbarMediums[9],
	/*1*/ KS_PhysConn_Audio_AudioDecoder,  0,                                    0,             &CrossbarMediums[10],
};

const ULONG Crossbar::NumberOfXBarInputs() const
{
	return SIZEOF_ARRAY(XBarInputPins);
}

const ULONG Crossbar::NumberOfXBarOutputs() const
{
	return SIZEOF_ARRAY(XBarOutputPins);
}

////////////////////////////////////////////////////////////////////////
// Tuner Static medium definitions ... used in registration process
//	provide the characteristics of the tuner class back to the 
//	Stream Class Driver.
//
// Note: To allow multiple instances of the same piece of hardware,
// set the first ULONG after the GUID in the Medium to a unique value.

const KSPIN_MEDIUM Tuner::TVTunerMediums[] = {
    {M_GUID0,           0, 0},  // Pin 0
    {M_GUID1,           0, 0},  // Pin 1
    {M_GUID2,           0, 0},  // Pin 2
};

const BOOL Tuner::TVTunerPinDirection [] = {
    TRUE,                       // Output Pin 0
    TRUE,                       // Output Pin 1
    TRUE,                       // Output Pin 2
};

////////////////////////////////////////////////////////////////////////
// TV Tuner property table ... used in registration process
//	provide the characteristics of the Tuner class back to the 
//	Stream Class Driver.

const KSPROPERTY_ITEM Tuner::TVTunerProperties[] =
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TUNER_CAPS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_TUNER_CAPS_S),        // MinProperty
        sizeof(KSPROPERTY_TUNER_CAPS_S),        // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TUNER_MODE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_TUNER_MODE_S),        // MinProperty
        sizeof(KSPROPERTY_TUNER_MODE_S),        // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TUNER_MODE_CAPS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_TUNER_MODE_CAPS_S),   // MinProperty
        sizeof(KSPROPERTY_TUNER_MODE_CAPS_S),   // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TUNER_STANDARD,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_TUNER_STANDARD_S),    // MinProperty
        sizeof(KSPROPERTY_TUNER_STANDARD_S),    // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TUNER_FREQUENCY,
        FALSE,                                  // GetSupported or Handler
        sizeof(KSPROPERTY_TUNER_FREQUENCY_S),   // MinProperty
        sizeof(KSPROPERTY_TUNER_FREQUENCY_S),   // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TUNER_INPUT,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_TUNER_INPUT_S),       // MinProperty
        sizeof(KSPROPERTY_TUNER_INPUT_S),       // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TUNER_STATUS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_TUNER_STATUS_S),      // MinProperty
        sizeof(KSPROPERTY_TUNER_STATUS_S),      // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    )
};

const ULONG Tuner::NumProperties()
{
	return SIZEOF_ARRAY(TVTunerProperties);
}

PKSPROPERTY_ITEM Tuner::PropertyList()
{
	return (PKSPROPERTY_ITEM)TVTunerProperties;
}

////////////////////////////////////////////////////////////////////////
// Tuner::Register
NTSTATUS Tuner::Register(PDEVICE_OBJECT pdo)
{
	return StreamClassRegisterFilterWithNoKSPins( 
		pdo,
		(const GUID*)&KSCATEGORY_TVTUNER,
		SIZEOF_ARRAY (TVTunerMediums),
		(int*)TVTunerPinDirection,
		(KSIDENTIFIER*)TVTunerMediums,
		NULL           
		);
}

////////////////////////////////////////////////////////////////////////
// AudioControl Static medium definitions ... used in registration process
//	provide the characteristics of the AudioControl class back to the 
//	Stream Class Driver.
//
// Note: To allow multiple instances of the same piece of hardware,
// set the first ULONG after the GUID in the Medium to a unique value.

const KSPIN_MEDIUM AudioControl::TVAudioMediums[] = {
    {M_GUID1,           0, 0},  // Pin 0
    {M_GUID3,           0, 0},  // Pin 1
};

const BOOL AudioControl::TVAudioPinDirection [] = {
    FALSE,                      // Input  Pin 0
    TRUE,                       // Output Pin 1
};

////////////////////////////////////////////////////////////////////////
// AudioControl property table ... used in registration process
//	provide the characteristics of the AudioControl class back to the 
//	Stream Class Driver.

const KSPROPERTY_ITEM AudioControl::TVAudioProperties[] =
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TVAUDIO_CAPS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_TVAUDIO_CAPS_S),      // MinProperty
        sizeof(KSPROPERTY_TVAUDIO_CAPS_S),      // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TVAUDIO_MODE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_TVAUDIO_S),           // MinProperty
        sizeof(KSPROPERTY_TVAUDIO_S),           // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_TVAUDIO_CURRENTLY_AVAILABLE_MODES,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_TVAUDIO_S),           // MinProperty
        sizeof(KSPROPERTY_TVAUDIO_S),           // MinData
        FALSE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
};

const ULONG AudioControl::NumProperties()
{
	return SIZEOF_ARRAY(TVAudioProperties);
}

PKSPROPERTY_ITEM AudioControl::PropertyList()
{
	return (PKSPROPERTY_ITEM)TVAudioProperties;
}

////////////////////////////////////////////////////////////////////////
// AudioControl::Register

NTSTATUS AudioControl::Register(PDEVICE_OBJECT pdo)
{
	return StreamClassRegisterFilterWithNoKSPins(
		pdo,
		(const GUID*)&KSCATEGORY_TVAUDIO,
		SIZEOF_ARRAY(TVAudioMediums),
		(int*)TVAudioPinDirection,
		(KSIDENTIFIER*)TVAudioMediums,
		NULL
		);
}

////////////////////////////////////////////////////////////////////////
// Crossbar Static medium definitions ... used in registration process
//	provide the characteristics of the Crossbar class back to the 
//	Stream Class Driver.
//
// Note: To allow multiple instances of the same piece of hardware,
// set the first ULONG after the GUID in the Medium to a unique value.

const KSPIN_MEDIUM Crossbar::CrossbarMediums[] = {
    {M_GUID0,           0, 0},  // Pin 0
    {STATIC_GUID_NULL,  0, 0},  // Pin 1
    {STATIC_GUID_NULL,  0, 0},  // Pin 2
    {STATIC_GUID_NULL,  0, 0},  // Pin 3
    {STATIC_GUID_NULL,  0, 0},  // Pin 4
    {M_GUID3,           0, 0},  // Pin 5
    {M_GUID2,           0, 0},  // Pin 6
    {STATIC_GUID_NULL,  0, 0},  // Pin 7
    {STATIC_GUID_NULL,  0, 0},  // Pin 8
    {M_GUID4,           0, 0},  // Pin 9
    {STATIC_GUID_NULL,  0, 0},  // Pin 10
};

const BOOL Crossbar::CrossbarPinDirection [] = {
    FALSE,                      // Input  Pin 0
    FALSE,                      // Input  Pin 1
    FALSE,                      // Input  Pin 2
    FALSE,                      // Input  Pin 3
    FALSE,                      // Input  Pin 4
    FALSE,                      // Input  Pin 5
    FALSE,                      // Input  Pin 6
    FALSE,                      // Input  Pin 7
    FALSE,                      // Input  Pin 8
    TRUE,                       // Output Pin 9
    TRUE,                       // Output Pin 10
};

////////////////////////////////////////////////////////////////////////
// Crossbar property table ... used in registration process
//	provide the characteristics of the Crossbar class back to the 
//	Stream Class Driver.
 
const KSPROPERTY_ITEM Crossbar::XBarProperties[] =
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CROSSBAR_CAPS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CROSSBAR_CAPS_S),     // MinProperty
        sizeof(KSPROPERTY_CROSSBAR_CAPS_S),     // MinData
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
        sizeof(KSPROPERTY_CROSSBAR_PININFO_S),  // MinProperty
        sizeof(KSPROPERTY_CROSSBAR_PININFO_S),  // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),

};

const ULONG Crossbar::NumProperties()
{	
	return SIZEOF_ARRAY(XBarProperties);
}

PKSPROPERTY_ITEM Crossbar::PropertyList()
{
	return (PKSPROPERTY_ITEM)XBarProperties;
}

////////////////////////////////////////////////////////////////////////
// Crossbar::Register

NTSTATUS Crossbar::Register(PDEVICE_OBJECT pdo)
{
	return StreamClassRegisterFilterWithNoKSPins(
		pdo,
		(const GUID*)&KSCATEGORY_CROSSBAR,
		SIZEOF_ARRAY(CrossbarMediums),
		(int*)CrossbarPinDirection,
		(KSIDENTIFIER*)CrossbarMediums,
		NULL
		);
}

////////////////////////////////////////////////////////////////////////
// VideoProcAmp property table ... used in registration process
//	provide the characteristics of the VideoProcAmp class back to the 
//	Stream Class Driver.... lots of components defined first.

const ULONG VideoProcAmp::BrightnessDefault = 5000;
const ULONG VideoProcAmp::ContrastDefault = 5000;

//
// First define all of the ranges and stepping values
//

const KSPROPERTY_STEPPING_LONG VideoProcAmp::BrightnessRangeAndStep [] = 
{
    {
        10000 / 10,         // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (IRE * 100) units
        10000               // Maximum in (IRE * 100) units
    }
};


const KSPROPERTY_MEMBERSLIST VideoProcAmp::BrightnessMembersList [] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (BrightnessRangeAndStep),
            SIZEOF_ARRAY (BrightnessRangeAndStep),
            0
        },
        (PVOID) BrightnessRangeAndStep,
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (BrightnessDefault),
            sizeof (BrightnessDefault),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &BrightnessDefault,
    }    
};

const KSPROPERTY_VALUES VideoProcAmp::BrightnessValues =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (BrightnessMembersList),
    BrightnessMembersList
};

const KSPROPERTY_STEPPING_LONG VideoProcAmp::ContrastRangeAndStep[] = 
{
    {
        10000 / 256,        // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum in (gain * 100) units
        10000               // Maximum in (gain * 100) units
    }
};


const KSPROPERTY_MEMBERSLIST VideoProcAmp::ContrastMembersList[] = 
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
        (PVOID) &ContrastDefault,
    }    
};

const KSPROPERTY_VALUES VideoProcAmp::ContrastValues =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (ContrastMembersList),
    ContrastMembersList
};


const KSPROPERTY_ITEM VideoProcAmp::VideoProcAmpProperties[] =
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
        &BrightnessValues,                      // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
};

const ULONG VideoProcAmp::NumProperties()
{
	return SIZEOF_ARRAY(VideoProcAmpProperties);
}

PKSPROPERTY_ITEM VideoProcAmp::PropertyList()
{
	return (PKSPROPERTY_ITEM)VideoProcAmpProperties;
}

VideoProcAmp::VideoProcAmp() :
	m_Brightness(BrightnessDefault),
	m_BrightnessFlags(KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO),
	m_Contrast(ContrastDefault),
	m_ContrastFlags(KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO)
{

}

////////////////////////////////////////////////////////////////////////
// CameraControl property table ... used in registration process
//	provide the characteristics of the CameraControl class back to the 
//	Stream Class Driver.... lots of components... define those first

const ULONG CameraControl::ZoomDefault = 5000;
const ULONG CameraControl::FocusDefault = 5000;

//
// First define all of the ranges and stepping values
//

const KSPROPERTY_STEPPING_LONG CameraControl::ZoomRangeAndStep [] = 
{
    {
        10000 / 10,         // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum 
        10000               // Maximum 
    }
};

const KSPROPERTY_MEMBERSLIST CameraControl::ZoomMembersList[] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (ZoomRangeAndStep),
            SIZEOF_ARRAY (ZoomRangeAndStep),
            0
        },
        (PVOID) ZoomRangeAndStep,
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (ZoomDefault),
            sizeof (ZoomDefault),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &ZoomDefault,
    }    
};

const KSPROPERTY_VALUES CameraControl::ZoomValues =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (ZoomMembersList),
    ZoomMembersList
};

const KSPROPERTY_STEPPING_LONG CameraControl::FocusRangeAndStep[] = 
{
    {
        10000 / 256,        // SteppingDelta (range / steps)
        0,                  // Reserved
        0,                  // Minimum 
        10000               // Maximum 
    }
};


const KSPROPERTY_MEMBERSLIST CameraControl::FocusMembersList[] = 
{
    {
        {
            KSPROPERTY_MEMBER_RANGES,
            sizeof (FocusRangeAndStep),
            SIZEOF_ARRAY (FocusRangeAndStep),
            0
        },
        (PVOID) FocusRangeAndStep
     },
     {
        {
            KSPROPERTY_MEMBER_VALUES,
            sizeof (FocusDefault),
            sizeof (FocusDefault),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &FocusDefault,
    }    
};

const KSPROPERTY_VALUES CameraControl::FocusValues =
{
    {
        STATICGUIDOF (KSPROPTYPESETID_General),
        VT_I4,
        0
    },
    SIZEOF_ARRAY (FocusMembersList),
    FocusMembersList
};

const KSPROPERTY_ITEM CameraControl::CameraControlProperties[] =
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_ZOOM,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CAMERACONTROL_S),     // MinProperty
        sizeof(KSPROPERTY_CAMERACONTROL_S),     // MinData
        TRUE,                                   // SetSupported or Handler
        &ZoomValues,                            // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_FOCUS,   
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CAMERACONTROL_S),     // MinProperty
        sizeof(KSPROPERTY_CAMERACONTROL_S),     // MinData
        TRUE,                                   // SetSupported or Handler
        &FocusValues,                           // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
};

const ULONG CameraControl::NumProperties()
{
	return SIZEOF_ARRAY(CameraControlProperties);
}

PKSPROPERTY_ITEM CameraControl::PropertyList()
{
	return (PKSPROPERTY_ITEM)CameraControlProperties; 
}

CameraControl::CameraControl() : 
	m_Focus(FocusDefault),
	m_FocusFlags(KSPROPERTY_CAMERACONTROL_FLAGS_AUTO),
	m_Zoom(ZoomDefault),
	m_ZoomFlags(KSPROPERTY_CAMERACONTROL_FLAGS_AUTO)
{
	
}

////////////////////////////////////////////////////////////////////////
// AnalogVideoDecoder property table ... used in registration process
//	provide the characteristics of the AnalogVideoDecoder class back to the 
//	Stream Class Driver....

const KSPROPERTY_ITEM AnalogVideoDecoder::AnalogVideoDecoderProperties[] =
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
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEODECODER_VCR_TIMING,
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

const ULONG AnalogVideoDecoder::NumProperties()
{
	return SIZEOF_ARRAY(AnalogVideoDecoderProperties); 
}

PKSPROPERTY_ITEM AnalogVideoDecoder::PropertyList()
{
	return (PKSPROPERTY_ITEM)AnalogVideoDecoderProperties;
}


////////////////////////////////////////////////////////////////////////
// VideoControl property table ... used in registration process
//	provide the characteristics of the VideoControl class back to the 
//	Stream Class Driver....

const KSPROPERTY_ITEM VideoControl::VideoControlProperties[] =
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOCONTROL_CAPS,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOCONTROL_CAPS_S), // MinProperty
        sizeof(KSPROPERTY_VIDEOCONTROL_CAPS_S), // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S),      // MinProperty
        sizeof(KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S),      // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOCONTROL_FRAME_RATES,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KSMULTIPLE_ITEM),                // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_VIDEOCONTROL_MODE,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_VIDEOCONTROL_MODE_S), // MinProperty
        sizeof(KSPROPERTY_VIDEOCONTROL_MODE_S), // MinData
        TRUE,                                   // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
};

const ULONG VideoControl::NumProperties()
{
	return SIZEOF_ARRAY(VideoControlProperties);
}

PKSPROPERTY_ITEM VideoControl::PropertyList()
{
	return (PKSPROPERTY_ITEM)VideoControlProperties;
}

////////////////////////////////////////////////////////////////////////
// Tuner::SetProperty

void Tuner::SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property
	ULONG nS = pSPD->PropertyOutputSize;        // size of data supplied

	switch (Id)
	{
	case KSPROPERTY_TUNER_MODE:
	{
		PKSPROPERTY_TUNER_MODE_S pMode =
			(PKSPROPERTY_TUNER_MODE_S)pSPD->Property;
		ASSERT (pMode->Mode == KSPROPERTY_TUNER_MODE_TV);

		break;
	}

	case KSPROPERTY_TUNER_STANDARD:
	{
		PKSPROPERTY_TUNER_STANDARD_S pStandard_S = 
			(PKSPROPERTY_TUNER_STANDARD_S) pSPD->Property;
		m_VideoStandard = pStandard_S->Standard;

		break;
	}

	case KSPROPERTY_TUNER_FREQUENCY:
	{
		PKSPROPERTY_TUNER_FREQUENCY_S pFreq_S = 
			(PKSPROPERTY_TUNER_FREQUENCY_S) pSPD->Property;
		m_Frequency = pFreq_S->Frequency;

		break;
	}

	case KSPROPERTY_TUNER_INPUT:
	{
		PKSPROPERTY_TUNER_INPUT_S pInput_S = 
			(PKSPROPERTY_TUNER_INPUT_S) pSPD->Property;

		m_Input = pInput_S->InputIndex;

		break;
	}

	default:
		ASSERT(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////
// Tuner::GetProperty

void Tuner::GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property
	ULONG nS = pSPD->PropertyOutputSize;        // size of data supplied
	PVOID pV = pSPD->PropertyInfo;              // pointer to the output data

	ASSERT (nS >= sizeof (LONG));

	switch (Id)
	{
	case KSPROPERTY_TUNER_CAPS:
	{
		PKSPROPERTY_TUNER_CAPS_S pCaps =
			(PKSPROPERTY_TUNER_CAPS_S)pSPD->Property;
		ASSERT (nS >= sizeof( KSPROPERTY_TUNER_CAPS_S ) );

		 // now work with the output buffer
		pCaps =(PKSPROPERTY_TUNER_CAPS_S)pV;

		pCaps->ModesSupported = KSPROPERTY_TUNER_MODE_TV;
		pCaps->VideoMedium = TVTunerMediums[0];
		pCaps->TVAudioMedium = TVTunerMediums[1];
		pCaps->RadioAudioMedium = TVTunerMediums[2];   // No separate radio audio pin?

		pSrb->ActualBytesTransferred = sizeof( KSPROPERTY_TUNER_CAPS_S );

		break;
	}

	case KSPROPERTY_TUNER_MODE:
	{
		PKSPROPERTY_TUNER_MODE_S pMode =
			(PKSPROPERTY_TUNER_MODE_S)pSPD->Property;
		ASSERT (nS >= sizeof( KSPROPERTY_TUNER_MODE_S ) );

		// now work with the output buffer
		pMode =(PKSPROPERTY_TUNER_MODE_S)pV;

		pMode->Mode = KSPROPERTY_TUNER_MODE_TV;

		pSrb->ActualBytesTransferred = sizeof( KSPROPERTY_TUNER_MODE_S);

		break;
	}

	case KSPROPERTY_TUNER_MODE_CAPS:
	{
		PKSPROPERTY_TUNER_MODE_CAPS_S pCaps = 
				(PKSPROPERTY_TUNER_MODE_CAPS_S) pSPD->Property;

		ASSERT (nS >= sizeof (KSPROPERTY_TUNER_MODE_CAPS_S));
		ASSERT (pCaps->Mode == KSPROPERTY_TUNER_MODE_TV);
		
		// now work with the output buffer
		pCaps = (PKSPROPERTY_TUNER_MODE_CAPS_S) pV;

		//
		// List the formats actually supported by this tuner
		//

		pCaps->StandardsSupported =  

		KS_AnalogVideo_NTSC_M

				|  KS_AnalogVideo_PAL_B    
				|  KS_AnalogVideo_PAL_D    
			//  |  KS_AnalogVideo_PAL_H    
			//  |  KS_AnalogVideo_PAL_I    
				|  KS_AnalogVideo_PAL_M    
				|  KS_AnalogVideo_PAL_N    

			//  |  KS_AnalogVideo_SECAM_B  
			//  |  KS_AnalogVideo_SECAM_D  
			//  |  KS_AnalogVideo_SECAM_G  
			//  |  KS_AnalogVideo_SECAM_H  
			//  |  KS_AnalogVideo_SECAM_K  
			//  |  KS_AnalogVideo_SECAM_K1 
			//  |  KS_AnalogVideo_SECAM_L  
				;

			//
			// Get the min and max frequencies supported
			//

		pCaps->MinFrequency =  55250000L;
		pCaps->MaxFrequency = 997250000L;

		//
		// What is the frequency step size?
		//

		pCaps->TuningGranularity =  62500L;

		//
		// How many inputs are on the tuner?
		//

		pCaps->NumberOfInputs = 1;

		//
		// What is the maximum settling time in milliseconds?
		//

		pCaps->SettlingTime = 100;

		//
		// Strategy defines how the tuner knows when it is in tune:
		// 
		// KS_TUNER_STRATEGY_PLL (Has PLL offset information)
		// KS_TUNER_STRATEGY_SIGNAL_STRENGTH (has signal strength info)
		// KS_TUNER_STRATEGY_DRIVER_TUNES (driver handles all fine tuning)
		//

		pCaps->Strategy = KS_TUNER_STRATEGY_DRIVER_TUNES;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TUNER_MODE_CAPS_S);

		break;
	}
		
	case KSPROPERTY_TUNER_STANDARD:
	{
		// What is the currently selected video standard?

		((PKSPROPERTY_TUNER_STANDARD_S) pSPD->PropertyInfo)->Standard =
				m_VideoStandard;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TUNER_STANDARD_S);

		break;
	}

	case KSPROPERTY_TUNER_INPUT:
	{
		// What is the currently selected input?

		((PKSPROPERTY_TUNER_INPUT_S) pSPD->PropertyInfo)->InputIndex = 
				m_Input;

		pSrb->ActualBytesTransferred = sizeof (PKSPROPERTY_TUNER_INPUT_S);

		break;
	}


	case KSPROPERTY_TUNER_STATUS:
	{
		// Return the status of the tuner

		// PLLOffset is in units of TuningGranularity 
		// SignalStrength is 0 to 100
		// Set Busy to 1 if tuning is still in process

		PKSPROPERTY_TUNER_STATUS_S pStatus = 
			(PKSPROPERTY_TUNER_STATUS_S) pSPD->PropertyInfo;

		ASSERT (nS >= sizeof (KSPROPERTY_TUNER_STATUS_S));
		pStatus->CurrentFrequency = m_Frequency;
		pStatus->PLLOffset = 0;
		pStatus->SignalStrength = 100;
		pStatus->Busy = m_Busy;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TUNER_STATUS_S);

		break;
	}

	default:
		ASSERT(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////
// Crossbar::SetProperty

void Crossbar::SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id  = pSPD->Property->Id;              // index of the property
	ULONG nS  = pSPD->PropertyOutputSize;        // size of data supplied

	switch (Id)
	{
	case KSPROPERTY_CROSSBAR_ROUTE:
	{
		PKSPROPERTY_CROSSBAR_ROUTE_S  pRoute = 
			(PKSPROPERTY_CROSSBAR_ROUTE_S)pSPD->PropertyInfo;   

		ASSERT (nS >= sizeof (KSPROPERTY_CROSSBAR_ROUTE_S));

		// Copy the input property info to the output property info
		RtlCopyMemory(  pRoute, 
						pSPD->Property, 
						sizeof (KSPROPERTY_CROSSBAR_ROUTE_S));

		// BUGBUG, this should check for legality of request, 
		// ie. that video pin isn't being routed to an audio pin, etc.

		if ((pRoute->IndexInputPin < NumberOfXBarInputs()) &&
			(pRoute->IndexOutputPin < NumberOfXBarOutputs())) {
				m_InputConnected = pRoute->IndexInputPin;
				pRoute->CanRoute = 1;
		}
		else
			pRoute->CanRoute = 0;

		break;
	}

	default:
		ASSERT (FALSE);
		break;
	}
}

////////////////////////////////////////////////////////////////////////
// Crossbar::GetProperty

void Crossbar::GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id  = pSPD->Property->Id;              // index of the property
	ULONG nS  = pSPD->PropertyOutputSize;        // size of data supplied

	switch (Id)
	{
	case KSPROPERTY_CROSSBAR_CAPS:                  // R 
	{
		PKSPROPERTY_CROSSBAR_CAPS_S  pCaps = 
			(PKSPROPERTY_CROSSBAR_CAPS_S)pSPD->PropertyInfo;   

		if (nS < sizeof (KSPROPERTY_CROSSBAR_CAPS_S))
			break;

		// Copy the input property info to the output property info
		RtlCopyMemory(pCaps, pSPD->Property, sizeof (KSPROPERTY_CROSSBAR_CAPS_S));

		pCaps->NumberOfInputs  = NumberOfXBarInputs();
		pCaps->NumberOfOutputs = NumberOfXBarOutputs();

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CROSSBAR_CAPS_S);

		break;
	}

	case KSPROPERTY_CROSSBAR_CAN_ROUTE:                   // R 
	{
		PKSPROPERTY_CROSSBAR_ROUTE_S  pRoute = 
			(PKSPROPERTY_CROSSBAR_ROUTE_S)pSPD->PropertyInfo;   

		if (nS < sizeof (KSPROPERTY_CROSSBAR_ROUTE_S))
			break;

		// Copy the input property info to the output property info
		RtlCopyMemory(pRoute, pSPD->Property, sizeof (KSPROPERTY_CROSSBAR_ROUTE_S));

		// Sanity check
		if ((pRoute->IndexInputPin  >= NumberOfXBarInputs()) ||
			(pRoute->IndexOutputPin >= NumberOfXBarOutputs()))
			pRoute->CanRoute = FALSE;
					
		else if ((pRoute->IndexInputPin <= 4) &&
			(pRoute->IndexOutputPin == 0) ||
			(pRoute->IndexInputPin >= 5) &&
			(pRoute->IndexOutputPin == 1))
		{
			// This driver allows any video input to connect to any video output
			// and any audio input to connect to any audio output
			pRoute->CanRoute = TRUE;
		}
		else
			pRoute->CanRoute = FALSE;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CROSSBAR_ROUTE_S);

		break;
	}

	case KSPROPERTY_CROSSBAR_PININFO:                     // R
	{ 
		PKSPROPERTY_CROSSBAR_PININFO_S  pPinInfo = 
			(PKSPROPERTY_CROSSBAR_PININFO_S)pSPD->PropertyInfo;   

		if (nS < sizeof (KSPROPERTY_CROSSBAR_PININFO_S))
			break;

		// Copy the input property info to the output property info
		RtlCopyMemory(pPinInfo, pSPD->Property, sizeof (KSPROPERTY_CROSSBAR_PININFO_S));

		if (pPinInfo->Direction == KSPIN_DATAFLOW_IN)
		{
			ASSERT (pPinInfo->Index < NumberOfXBarInputs());

			pPinInfo->PinType          = XBarInputPins[pPinInfo->Index].PinType;
			pPinInfo->RelatedPinIndex  = XBarInputPins[pPinInfo->Index].RelatedPinIndex;
			pPinInfo->Medium           = *XBarInputPins[pPinInfo->Index].Medium;
		}
		else
		{
			ASSERT (pPinInfo->Index < NumberOfXBarOutputs());

			pPinInfo->PinType          = XBarOutputPins[pPinInfo->Index].PinType;
			pPinInfo->RelatedPinIndex  = XBarOutputPins[pPinInfo->Index].RelatedPinIndex;
			pPinInfo->Medium           = *XBarOutputPins[pPinInfo->Index].Medium;
		}

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CROSSBAR_PININFO_S);

		break;
	}

	case KSPROPERTY_CROSSBAR_ROUTE:                   // R 
	{
		PKSPROPERTY_CROSSBAR_ROUTE_S  pRoute = 
			(PKSPROPERTY_CROSSBAR_ROUTE_S)pSPD->PropertyInfo;   

		if (nS < sizeof (KSPROPERTY_CROSSBAR_ROUTE_S))
			break;

		// Copy the input property info to the output property info
		RtlCopyMemory(pRoute, pSPD->Property, sizeof (KSPROPERTY_CROSSBAR_ROUTE_S));

		// Sanity check
		if (pRoute->IndexOutputPin >= NumberOfXBarOutputs())
			pRoute->CanRoute = FALSE;
				
		else
		{
			pRoute->IndexInputPin = m_InputConnected;
			pRoute->CanRoute = TRUE;
		}

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CROSSBAR_ROUTE_S);

		break;
	}

	default:
		ASSERT(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////
// VideoProcAmp::SetProperty

void VideoProcAmp::SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property
	PKSPROPERTY_VIDEOPROCAMP_S pS = (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;

	ASSERT (pSPD->PropertyInputSize >= sizeof (KSPROPERTY_VIDEOPROCAMP_S));

	switch (Id)
	{
	case KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:
		m_Brightness = pS->Value;
		m_BrightnessFlags = pS->Flags;
		break;
		
	case KSPROPERTY_VIDEOPROCAMP_CONTRAST:
		m_Contrast = pS->Value;
		m_ContrastFlags = pS->Flags;
		break;

	default:
		ASSERT(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////
// VideoProcAmp::GetProperty

void VideoProcAmp::GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property
	PKSPROPERTY_VIDEOPROCAMP_S pS = (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;  

	ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOPROCAMP_S));

	switch (Id)
	{
	case KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:
		pS->Value = m_Brightness;
		pS->Flags = m_BrightnessFlags;
		pS->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL | 
						   KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
		break;
		
	case KSPROPERTY_VIDEOPROCAMP_CONTRAST:
		pS->Value = m_Contrast;
		pS->Flags = m_ContrastFlags;
		pS->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL | 
						   KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
		break;

	default:
		ASSERT(FALSE);
	}

	pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOPROCAMP_S);
}



////////////////////////////////////////////////////////////////////////
// CameraControl::SetProperty

void CameraControl::SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property
	PKSPROPERTY_CAMERACONTROL_S pS = (PKSPROPERTY_CAMERACONTROL_S) pSPD->PropertyInfo;

	ASSERT (pSPD->PropertyInputSize >= sizeof (KSPROPERTY_CAMERACONTROL_S));

	switch (Id)
	{
	case KSPROPERTY_CAMERACONTROL_ZOOM:
		m_Zoom = pS->Value;
		m_ZoomFlags = pS->Flags;
		break;
		
	case KSPROPERTY_CAMERACONTROL_FOCUS:
		m_Focus = pS->Value;
		m_FocusFlags = pS->Flags;
		break;

	default:
		ASSERT(FALSE);
	}
}


////////////////////////////////////////////////////////////////////////
// CameraControl::SetProperty

void CameraControl::GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property
	PKSPROPERTY_CAMERACONTROL_S pS = (PKSPROPERTY_CAMERACONTROL_S) pSPD->PropertyInfo;    // pointer to the output data

	ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_CAMERACONTROL_S));

	switch (Id)
	{
	case KSPROPERTY_CAMERACONTROL_ZOOM:
		pS->Value = m_Zoom;
		pS->Flags = m_ZoomFlags;
		pS->Capabilities = KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL | 
						   KSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
		break;
		
	case KSPROPERTY_CAMERACONTROL_FOCUS:
		pS->Value = m_Focus;
		pS->Flags = m_FocusFlags;
		pS->Capabilities = KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL | 
						   KSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
		break;

	default:
		ASSERT(FALSE);
	}
	pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CAMERACONTROL_S);
}


////////////////////////////////////////////////////////////////////////
// AudioControl::SetProperty

void AudioControl::SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property


	switch (Id)
	{
	case KSPROPERTY_TVAUDIO_MODE:
	{
		PKSPROPERTY_TVAUDIO_S pS = (PKSPROPERTY_TVAUDIO_S) pSPD->PropertyInfo;    

		m_Mode = pS->Mode;

		break;
	}

	default:
		ASSERT(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////
// AudioControl::GetProperty

void AudioControl::GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property

	switch (Id) {

	case KSPROPERTY_TVAUDIO_CAPS:
	{
		PKSPROPERTY_TVAUDIO_CAPS_S pS = (PKSPROPERTY_TVAUDIO_CAPS_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_TVAUDIO_CAPS_S));
		
		pS->InputMedium  = TVAudioMediums[0];
		pS->OutputMedium = TVAudioMediums[1];
		// Report all of the possible audio decoding modes the hardware is capabable of
		pS->Capabilities = KS_TVAUDIO_MODE_MONO   |
						   KS_TVAUDIO_MODE_STEREO |
						   KS_TVAUDIO_MODE_LANG_A |
						   KS_TVAUDIO_MODE_LANG_B ;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TVAUDIO_CAPS_S);

		break;
	}
		
	case KSPROPERTY_TVAUDIO_MODE:
	{
		PKSPROPERTY_TVAUDIO_S pS = (PKSPROPERTY_TVAUDIO_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_TVAUDIO_S));
		// Report the currently selected mode
		pS->Mode = m_Mode;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TVAUDIO_S);

		break;
	}

	case KSPROPERTY_TVAUDIO_CURRENTLY_AVAILABLE_MODES:
	{
		PKSPROPERTY_TVAUDIO_S pS = (PKSPROPERTY_TVAUDIO_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_TVAUDIO_S));
		// Report which audio modes could potentially be selected right now
		pS->Mode = KS_TVAUDIO_MODE_MONO   |
				   KS_TVAUDIO_MODE_STEREO |
				   KS_TVAUDIO_MODE_LANG_A ;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TVAUDIO_S);

		break;
	}
	
	default:
		ASSERT(FALSE);
	}
}


////////////////////////////////////////////////////////////////////////
// AnalogVideoDecoder::SetProperty

void AnalogVideoDecoder::SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property
	PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;

	ASSERT (pSPD->PropertyInputSize >= sizeof (KSPROPERTY_VIDEODECODER_S));
	
	switch (Id) 
	{
	case KSPROPERTY_VIDEODECODER_STANDARD:
		m_VideoStandard = pS->Value;
		break;

	case KSPROPERTY_VIDEODECODER_OUTPUT_ENABLE:
		m_OutputEnable = pS->Value;
		break;

	case KSPROPERTY_VIDEODECODER_VCR_TIMING:
		m_VCRTiming = pS->Value;
		break;

	default:
		ASSERT(FALSE);
	}
}


////////////////////////////////////////////////////////////////////////
// AnalogVideoDecoder::GetProperty

void AnalogVideoDecoder::GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property

	switch (Id) {

	case KSPROPERTY_VIDEODECODER_CAPS:
	{
		PKSPROPERTY_VIDEODECODER_CAPS_S pS = (PKSPROPERTY_VIDEODECODER_CAPS_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_CAPS_S));
		
		pS->StandardsSupported =         
				   KS_AnalogVideo_NTSC_M

				|  KS_AnalogVideo_PAL_B    
				|  KS_AnalogVideo_PAL_D    
			//  |  KS_AnalogVideo_PAL_H    
			//  |  KS_AnalogVideo_PAL_I    
				|  KS_AnalogVideo_PAL_M    
				|  KS_AnalogVideo_PAL_N    

			//  |  KS_AnalogVideo_SECAM_B  
			//  |  KS_AnalogVideo_SECAM_D  
			//  |  KS_AnalogVideo_SECAM_G  
			//  |  KS_AnalogVideo_SECAM_H  
			//  |  KS_AnalogVideo_SECAM_K  
			//  |  KS_AnalogVideo_SECAM_K1 
			//  |  KS_AnalogVideo_SECAM_L  
				   ;

		pS->Capabilities = KS_VIDEODECODER_FLAGS_CAN_DISABLE_OUTPUT  |
						   KS_VIDEODECODER_FLAGS_CAN_USE_VCR_LOCKING |
						   KS_VIDEODECODER_FLAGS_CAN_INDICATE_LOCKED ;


		pS->SettlingTime = 10;          // How long to delay after tuning before 
										// Locked indicator is valid
										
		pS->HSyncPerVSync = 6;          // HSync per VSync

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_CAPS_S);
	}
	break;
		
	case KSPROPERTY_VIDEODECODER_STANDARD:
	{
		PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_S));

		pS->Value = m_VideoStandard;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
	}
	break;

	case KSPROPERTY_VIDEODECODER_STATUS:
	{
		PKSPROPERTY_VIDEODECODER_STATUS_S pS = (PKSPROPERTY_VIDEODECODER_STATUS_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_STATUS_S));

		pS->NumberOfLines = (m_VideoStandard & KS_AnalogVideo_NTSC_Mask)
							 ? 525 : 625;

		// Just to make things interesting, simulate that some channels aren't locked
		// In the US, these are channels 54 through 70
		// pS->SignalLocked = (pHwDevExt->Frequency < 400000000 || pHwDevExt->Frequency > 500000000);

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
	}
	break;

	case KSPROPERTY_VIDEODECODER_OUTPUT_ENABLE:
	{
		PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_S));

		pS->Value = m_OutputEnable;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
	}
	break;

	case KSPROPERTY_VIDEODECODER_VCR_TIMING:
	{
		PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_S));

		pS->Value = m_VCRTiming;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
	}
	break;

	default:
		// TRAP --
		break;
	}
}

////////////////////////////////////////////////////////////////////////
// VideoControl::SetProperty

void VideoControl::SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property
	PKSPROPERTY_VIDEOCONTROL_MODE_S pS = (PKSPROPERTY_VIDEOCONTROL_MODE_S) pSPD->PropertyInfo;

	ASSERT (pSPD->PropertyInputSize >= sizeof (KSPROPERTY_VIDEOCONTROL_MODE_S));
	
	switch (Id)
	{
	case KSPROPERTY_VIDEOCONTROL_MODE:
		m_Mode = pS->Mode;
		break;

	default:
		ASSERT(FALSE);
	}
}


////////////////////////////////////////////////////////////////////////
// VideoControl::GetProperty

void VideoControl::GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	ULONG Id = pSPD->Property->Id;              // index of the property

	switch (Id)
	{
	case KSPROPERTY_VIDEOCONTROL_CAPS:
	{
		PKSPROPERTY_VIDEOCONTROL_CAPS_S pS = (PKSPROPERTY_VIDEOCONTROL_CAPS_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOCONTROL_CAPS_S));
		
		pS->VideoControlCaps =    
			  KS_VideoControlFlag_FlipHorizontal       
			| KS_VideoControlFlag_FlipVertical         
			| KS_VideoControlFlag_ExternalTriggerEnable
			| KS_VideoControlFlag_Trigger              
			;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCONTROL_CAPS_S);

		break;
	}
		
	case KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE:
	{
		PKSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S pS = 
			(PKSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S));

		pS->CurrentActualFrameRate = 15;        // bugbug
		pS->CurrentMaxAvailableFrameRate = 15;  // bugbug
		

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S);

		break;
	}

	case KSPROPERTY_VIDEOCONTROL_FRAME_RATES:
	{
		// bugbug 
		// todo
		break;
	}

	case KSPROPERTY_VIDEOCONTROL_MODE:
	{
		PKSPROPERTY_VIDEOCONTROL_MODE_S pS = (PKSPROPERTY_VIDEOCONTROL_MODE_S) pSPD->PropertyInfo;    // pointer to the data

		ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOCONTROL_MODE_S));

		pS->Mode = m_Mode;

		pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCONTROL_MODE_S);

		break;
	}

	default:
		ASSERT(FALSE);
	}
}
