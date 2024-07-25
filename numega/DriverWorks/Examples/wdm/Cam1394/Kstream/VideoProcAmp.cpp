// VideoProcAmp.cpp: implementation of the VideoProcAmp class.
//
//////////////////////////////////////////////////////////////////////

#include <vdw.h>
#include <kstream.h>
#include <ksmedia.h>
#include "VideoProcAmp.h"


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

