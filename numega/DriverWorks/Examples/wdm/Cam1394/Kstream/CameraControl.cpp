// CameraControl.cpp: implementation of the CameraControl class.
//
//////////////////////////////////////////////////////////////////////

#include <vdw.h>
#include <kstream.h>
#include <ksmedia.h>
#include "CameraControl.h"


////////////////////////////////////////////////////////////////////////
// CameraControl property table ... used in registration process
//	provide the characteristics of the CameraControl class back to the 
//	Stream Class Driver.... lots of components... define those first

const ULONG CameraControl::ZoomDefault = 5000;
const ULONG CameraControl::FocusDefault = 5000;

//
// First define all of the ranges and stepping values
//

const KSPROPERTY_STEPPING_LONG CameraControl::ZoomRangeAndStep[] = 
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
// CameraControl::GetProperty

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