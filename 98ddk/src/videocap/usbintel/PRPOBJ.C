/*++

Copyright (c) 1996, 1997  Intel Corporation

Module Name:

   prpobj.c

Abstract:

   Code related to "getting"  about properties 

Author:


Environment:

   Kernel mode only


Revision History:

--*/

#include "warn.h"
#include "wdm.h"

#include <strmini.h>
#include <ksmedia.h>

#include "usbdi.h"
#include "usbcamdi.h"

#include "intelcam.h"

#include "prpobj.h"

//
// First define all of the ranges and stepping values
//

// ------------------------------------------------------------------------
KSPROPERTY_STEPPING_LONG BrightnessRangeAndStep [] = 
{
    {
        STEPPING_DELTA_BRIGHTNESS,   // SteppingDelta
        0,                           // Reserved
        0,                           // Minimum in (IRE * 100) units
        MAX_BRIGHTNESS_IRE_UNITS     // Maximum in (IRE * 100) units
    }
};

const ULONG BrightnessDefault = (5*12); // Marketing suggested value

KSPROPERTY_MEMBERSLIST BrightnessMembersList [] = 
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
            sizeof(BrightnessDefault),
            KSPROPERTY_MEMBER_FLAG_DEFAULT
        },
        (PVOID) &BrightnessDefault
	}


};

KSPROPERTY_VALUES BrightnessValues =
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
DEFINE_KSPROPERTY_TABLE(VideoProcAmpProperties)
{
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



//
// Property set for the Video Camera
//

DEFINE_KSPROPERTY_TABLE(CameraProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_PAN,           // PropertyId
        TRUE,                                   // GetSupported or Handler
        sizeof(ULONG),                          // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_TILT,
        TRUE,                                   // GetSupported or Handler
        sizeof(ULONG),                          // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_ROLL,
        TRUE,                                   // GetSupported or Handler
        sizeof(ULONG),                          // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_ZOOM,
        TRUE,                                   // GetSupported or Handler
        sizeof(ULONG),                          // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_EXPOSURE,
        TRUE,                                   // GetSupported or Handler
        sizeof(ULONG),                          // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_IRIS,
        TRUE,                                   // GetSupported or Handler
        sizeof(ULONG),                          // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CAMERACONTROL_FOCUS,
        TRUE,                                   // GetSupported or Handler
        sizeof(ULONG),                          // MinProperty
        sizeof(ULONG),                          // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    )
};


GUID PROPSETID_INTEL_CUSTOM_PROP =
    {0xCD8C22E0, 0xCB4C, 0x11CF, {0x85, 0xC8, 0xDC, 0x17, 0x88, 0x39, 0xEC, 0x74}};



DEFINE_KSPROPERTY_TABLE(CustomProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CUSTOM_PROP_FIRMWARE_VER,    // PropertyId
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_CUSTOM_PROP_S),       // MinProperty
        sizeof(KSPROPERTY_CUSTOM_PROP_S),       // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    )
};
    



DEFINE_KSPROPERTY_TABLE(FrameRateProperties)
{

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
        sizeof(KSPROPERTY_VIDEOCONTROL_FRAME_RATES_S),    // MinProperty
        0 ,                                     // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),

};
    

//
// All of the property sets supported by the adapter
//

DEFINE_KSPROPERTY_SET_TABLE(AdapterPropertyTable)
{
#if 0
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_CAMERACONTROL,                // Set
        SIZEOF_ARRAY(CameraProperties),                 // PropertiesCount
        CameraProperties,                               // PropertyItem
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),
#endif

    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_VIDEOPROCAMP,                 // Set
        SIZEOF_ARRAY(VideoProcAmpProperties),           // PropertiesCount
        VideoProcAmpProperties,                         // PropertyItem
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),

    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_INTEL_CUSTOM_PROP,             // Set
        SIZEOF_ARRAY(CustomProperties),           // PropertiesCount
        CustomProperties,                         // PropertyItem
        0,                                        // FastIoCount
        NULL                                      // FastIoTable
    ),

    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_VIDEOCONTROL,           // Set
        SIZEOF_ARRAY(FrameRateProperties),        // PropertiesCount
        FrameRateProperties,                      // PropertyItem
        0,                                        // FastIoCount
        NULL                                      // FastIoTable
    )

};


#define NUMBER_OF_ADAPTER_PROPERTY_SETS (SIZEOF_ARRAY (AdapterPropertyTable))



////////////////
/////
/////  The following are Camera dependent
/////
////////////////

//
// Brightness.
//
const RANGE _Brightness =
{
    0, 9, 0, 9, 1,
    4                  // Default
};



//
// Format/resolution combinations.  The first resolution under
// each format is the default.
//
static const FORMAT _Format[] =
{
    { FCC_FORMAT_YUV12N, 176,144 },
    { FCC_FORMAT_YUV12N, 320,240 },
    {(FOURCC)-1, -1,-1 }
};

//
// Frame rates.
//
static const ULONG _Rate[] =
{
    1000000/25,                     // Default
    0,                              // Zero
    1000000/6,
    1000000/10,
    1000000/12,
    1000000/15,
    1000000/18,
    1000000/20,
    (ULONG)-1
};

//
// Scale.
//
static const SCALE _Scale[] =
{
// Setting     Ratio         ResX ResY NegX PosX NegY PosY DefXPan DefYPan Default
    {0x0,  MakeULong(1,1)  , 176, 144, -89, 89,  -71, 71,   0,      0,     FALSE},   // Default
    {0x1,  MakeULong(5,4)  , 176, 144, -66, 66,  -54, 54,   0,      0,     FALSE},
    {0x2,  MakeULong(3,2)  , 176, 144, -44, 44,  -36, 36,   0,      0,     FALSE},
    {0x3,  MakeULong(5,3)  , 176, 144, -29, 29,  -24, 24,   0,      0,     FALSE},
    {0x4,  MakeULong(2,1)  , 176, 144,   0,  0,    0,  0,   0,      0,     TRUE},
    {0x5,  MakeULong(1,1)  , 320, 240, -17, 17,  -23, 23,   0,      0,     TRUE},
    {0x6,  MakeULong(12,11), 320, 240, -17, 17,  -13, 13,   0,      0,     FALSE},
    {0x7,  MakeULong(5,4)  , 160, 131, -76, 76,  -59, 59,   0,      0,     FALSE},
    {0x8,  MakeULong(3,2)  , 160, 131, -56, 56,  -42, 42,   0,      0,     FALSE},
    {0x9,  MakeULong(5,3)  , 160, 131, -42, 43,  -30, 31,   0,      0,     FALSE},
    {0xa,  MakeULong(2,1)  , 160, 131, -16, 16,  -8,   8,   0,      0,     FALSE},
    {0xb,  MakeULong(12,11), 160, 120, -96, 96,  -78, 79,   0,      0,     TRUE},
    {(ULONG)-1,           0,   0,   0,   0,  0,    0,  0,   0,      0,     FALSE}
};




/*
** INTELCAM_GetStreamPropertiesArray()
**
** Returns the stream class defined properties fror the camera
**
**  see stream class documentation
**
** Arguments:
**
** DeviceContext - camera specific context
**
** NumberOfArrayEntries - pointer to ulong , should be filled
**      in with the number of elements in the array returned.
**
** Returns:
**
** Side Effects:  none
*/

PVOID   
INTELCAM_GetAdapterPropertyTable(
    PULONG NumberOfArrayEntries
    )
{

    INTELCAM_KdPrint (MAX_TRACE, ("GetStreamFormatsArray\n")); 
    
    *NumberOfArrayEntries = NUMBER_OF_ADAPTER_PROPERTY_SETS;
    
    return (PVOID) AdapterPropertyTable;
}




NTSTATUS
FormPropertyData(
    IN OUT PHW_STREAM_REQUEST_BLOCK pSrb,
    IN PVOID pData,
	IN REQUEST ReqID
    )
{

	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    
    switch( ReqID )
	{
        case REQ_BRIGHTNESS:
        case REQ_WHITEBALANCE:
        case REQ_ENHANCEMENT:
        case REQ_EXPOSURE:
        case REQ_SATURATION:
		{
            PKSPROPERTY_VIDEOPROCAMP_S pPropertyInfo =
                (PKSPROPERTY_VIDEOPROCAMP_S)pSPD->PropertyInfo;

            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOPROCAMP_S);
            pSPD->PropertyOutputSize =
	                           sizeof(KSPROPERTY_VIDEOPROCAMP_S);

			pPropertyInfo->Flags = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
			pPropertyInfo->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL; // no Auto mode

			if(pData)
				pPropertyInfo->Value = *(WORD *)pData;
			else
				return STATUS_NO_MEMORY;

			return STATUS_SUCCESS;
		}

		default:
		   return STATUS_NOT_SUPPORTED;
	}

}


ULONG
GetPropertyCount(
    IN REQUEST ReqID
    )
/*++
    Routine Description:
    Arguments:
    Return Value:
--*/
{
    DWORD n = 0;
        
    switch( ReqID ) {

        default:
            break;
    }

    return 0;

}


/*
** GetPropertyCurrent()
**
** Arguments:
**
**  pDC - driver context
**
** Returns:
**
**  BOOLEAN 
**  
** Side Effects:  none
*/
BOOLEAN
GetPropertyCurrent(
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    OUT PVOID pProperty,
    IN REQUEST ReqID
    )
/*++
    Routine Description:
    Arguments:
    Return Value:
--*/
{
    USHORT Index;
    WORD Value;
    NTSTATUS Status = STATUS_NOT_SUPPORTED;
    ULONG Length = sizeof(WORD);

    ASSERT(pDC);
    ASSERT(pProperty);

    switch ( ReqID )
    {
        case REQ_BRIGHTNESS:
            Index = INDEX_PREF_BRIGHTNESS;
            break;

		default:
            return FALSE;
    }

    Status = USBCAMD_ControlVendorCommand(pDC,
                                          REQUEST_GET_PREFERENCE,
                                          0,
                                          Index,
                                          &Value,
                                          &Length,
                                          TRUE,
										  NULL,
										  NULL);
            
    if(!NT_SUCCESS(Status)){
        INTELCAM_KdPrint(MAX_TRACE, ("ERROR: Getting property - %0x\n", Index));
        return FALSE;
    }
            

    switch ( ReqID )
    {
        case REQ_BRIGHTNESS:
            *((ULONG*)pProperty) =
                (Value * STEPPING_DELTA_BRIGHTNESS);
            break;
        
        INTELCAM_KdPrint(MAX_TRACE, ("PROP: Getting Curr property - %d\n",  *((ULONG*)pProperty) ));
    }

    return TRUE;

}


/*
** GetPropertyDefault()
**
** Arguments:
**
**  pDC - driver context
**
** Returns:
**
**  BOOLEAN 
**  
** Side Effects:  none
*/
BOOLEAN
GetPropertyDefault(
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    OUT PVOID pProperty,
    IN REQUEST ReqID
    )
/*++
    Routine Description:
    Arguments:
    Return Value:
--*/
{
    ASSERT(pProperty);

    switch( ReqID ) {
        case REQ_BRIGHTNESS:
            *((ULONG*)pProperty) =
                 (_Brightness.Default * STEPPING_DELTA_BRIGHTNESS);
            return TRUE;

        default:
            break;
    }

    return FALSE;

}



/*
** SetPropertyCurrent()
**
** Arguments:
**
**  pDC - driver context
**
** Returns:
**
**  BOOLEAN 
**  
** Side Effects:  none
*/
BOOLEAN
SetPropertyCurrent(
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    IN PVOID pProperty,
    IN REQUEST ReqID
    )
{
    ASSERT(pDC);
    ASSERT(pProperty);

    switch( ReqID )
    {
        //
        // No need to do anything as we read off of the camera for the
        // current value.
        //
        case REQ_BRIGHTNESS:
            return TRUE;
        default:
            break;
    }

    return FALSE;
}


/*
** SetPropertyDefaults()
**
** Arguments:
**
**  pDC - driver context
**
** Returns:
**
**  void 
**  
** Side Effects:  none
*/
VOID
SetPropertyDefaults(
    IN PINTELCAM_DEVICE_CONTEXT pDC
    )
{

    //
    // Initialize current settings to defaults.
    //

    ASSERT(pDC);

    GetPropertyDefault(pDC,
                       &(pDC->CurrentProperty.Brightness),
                       REQ_BRIGHTNESS);

}



