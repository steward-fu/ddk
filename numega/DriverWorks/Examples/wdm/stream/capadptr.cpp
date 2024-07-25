// capadptr.cpp - Sample stream minidriver adapter class
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

#include <vdw.h>
#include <kstream.h>

#include "capstream.h"
#include "mediums.h"


////////////////////////////////////////////////////////////////////////
// VideoCaptureAdapter:: topology definition ... used in registration
//  process	provide the characteristics of the VideoCaptureAdapter class
//	back to the	Stream Class Driver.
// 
const GUID VideoCaptureAdapter::Categories[] = {
    STATIC_KSCATEGORY_VIDEO,
    STATIC_KSCATEGORY_CAPTURE,
    STATIC_KSCATEGORY_TVTUNER,
    STATIC_KSCATEGORY_CROSSBAR,
    STATIC_KSCATEGORY_TVAUDIO
};

const KSTOPOLOGY VideoCaptureAdapter::AdapterTopology = {
    SIZEOF_ARRAY(Categories),           // CategoriesCount
    (GUID*) &Categories,                // Categories
    0,                                  // TopologyNodesCount
    NULL,                               // TopologyNodes
    0,                                  // TopologyConnectionsCount
    NULL,                               // TopologyConnections
    NULL,                               // TopologyNodesNames
    0,                                  // Reserved
};

////////////////////////////////////////////////////////////////////////
// VideoCaptureAdapter:: static medium definitions ... used in registration
//  process	provide the characteristics of the VideoCaptureAdapter class
//	back to the	Stream Class Driver.
//
// Note: To allow multiple instances of the same piece of hardware,
// set the first ULONG after the GUID in the Medium to a unique value.

const KSPIN_MEDIUM VideoCaptureAdapter::CaptureMediums[] = {
    {M_GUID4,           0, 0},  // Pin 0  Analog Video In
    {STATIC_GUID_NULL,  0, 0},  // Pin 1  Capture
    {STATIC_GUID_NULL,  0, 0},  // Pin 2  Preview
};

const BOOL VideoCaptureAdapter::CapturePinDirection [] = {
    FALSE,                      // Input  Pin 0
    TRUE,                       // Output Pin 1
    TRUE,                       // Output Pin 2
};

// ------------------------------------------------------------------------
// Array of all of the property sets supported by the adapter
// ------------------------------------------------------------------------

const KSPROPERTY_SET VideoCaptureAdapter::AdapterPropertyTable[] =
{
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_CROSSBAR,             // Set
		Crossbar::NumProperties(),		        // PropertiesCount
		Crossbar::PropertyList(),	            // PropertyItem
        0,                                      // FastIoCount
        NULL                                    // FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_TUNER,
		Tuner::NumProperties(),
		Tuner::PropertyList(),
        0, 
        NULL
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_TVAUDIO,
		AudioControl::NumProperties(),
		AudioControl::PropertyList(),
        0, 
        NULL
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_VIDEOPROCAMP,
		VideoProcAmp::NumProperties(),
        VideoProcAmp::PropertyList(),
        0, 
        NULL
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_CAMERACONTROL,
        CameraControl::NumProperties(),
        CameraControl::PropertyList(),
        0, 
        NULL
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_VIDEOCONTROL,
        VideoControl::NumProperties(),
        VideoControl::PropertyList(),
        0, 
        NULL
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_VIDEODECODER,
        AnalogVideoDecoder::NumProperties(),
        AnalogVideoDecoder::PropertyList(),
        0, 
        NULL
    )

};

VideoCaptureAdapter::VideoCaptureAdapter(
	KStreamMinidriver* driver, 
	PORT_CONFIGURATION_INFORMATION* pConfigInfo
	) :
	KStreamAdapter(
		driver,
		pConfigInfo,
		3,			// Number of streams
		(SIZEOF_ARRAY (AdapterPropertyTable)),
		(PKSPROPERTY_SET)AdapterPropertyTable,
		0,			// Number of Events
		NULL,		// Events
		(PKSTOPOLOGY)&AdapterTopology
		)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VideoCaptureAdapter::VideoCaptureAdapter -- Constructing\n";
	#endif
}

void VideoCaptureAdapter::Initialize()
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VideoCaptureAdapter::Initialize -- Start, Initializing each Stream\n";
	#endif

	m_Input.InitializeStream();
	m_Capture.InitializeStream();
	m_Preview.InitializeStream();

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VideoCaptureAdapter::Initialize -- End\n";
	#endif
}

KStream* VideoCaptureAdapter::GetStream(ULONG StreamNumber)
{

	switch (StreamNumber)
	{
	case 0:
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::GetStream -- input\n";
		#endif
		return &m_Input;
	case 1:
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::GetStream -- capture\n";
		#endif
		return &m_Capture;
	case 2:
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::GetStream -- preview\n";
		#endif
		return &m_Preview;
	default:
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceWarning << "VideoCaptureAdapter::GetStream -- invalid stream\n";
		#endif
		return NULL;
	}
}

void VideoCaptureAdapter::OnInitializationComplete(PHW_STREAM_REQUEST_BLOCK Srb)
{
	PDEVICE_OBJECT pdo = PDO();
	NTSTATUS	t_status = STATUS_SUCCESS;

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VideoCaptureAdapter::OnInitializationComplete -- Start\n";
	#endif
	
	// -- keep going as long as we don't fail a registration --
	if (STATUS_SUCCESS == (t_status = m_Tuner.Register(pdo)))
		if (STATUS_SUCCESS == (t_status = m_Crossbar.Register(pdo)))
			if (STATUS_SUCCESS == (t_status = m_Audio.Register(pdo)))
				t_status = Register();


	NextSrb(Srb, t_status);		// -- return our return value! --

	#ifdef __VIDCAP_TRACEON__
	if (STATUS_SUCCESS == t_status)
		TVS << TraceInfo << "VideoCaptureAdapter::OnInitializationComplete -- End, Succeeded\n";
	else
		TVS << TraceInfo << "VideoCaptureAdapter::On InitializationComplete -- End, Failed to register all devices\n";
	#endif
}

void VideoCaptureAdapter::OnSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	GUID Set = pSPD->Property->Set;

	//  -- excessive trace, use only for DEBUG --
	//	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- Start\n";
	//	#endif

	if (IsEqualGUID(PROPSETID_VIDCAP_CROSSBAR, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- Crossbar\n";
		#endif
		m_Crossbar.SetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_TUNER, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- Tuner\n";
		#endif
		m_Tuner.SetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEOPROCAMP, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- Video Procamp\n";
		#endif
		m_ProcAmp.SetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_CAMERACONTROL, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- Camera Control\n";
		#endif
		m_CameraControl.SetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_TVAUDIO, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- Audio Control\n";
		#endif
		m_Audio.SetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEODECODER, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- Video Decoder\n";
		#endif
		m_AVDecoder.SetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEOCONTROL, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- Video Control\n";
		#endif
		m_VideoControl.SetProperty(pSrb);
	}
	else 
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- End, Invalid PropertySet ID\n";
		#endif

		ASSERT(FALSE); // We should never get here/
		NextSrb(pSrb, STATUS_INVALID_PARAMETER);
		return;
	}

	NextSrb(pSrb);

	//  -- excessive trace, use only for DEBUG --
	//	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VideoCaptureAdapter::OnSetProperty -- End\n";
	//	#endif
}

void VideoCaptureAdapter::OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	GUID Set = pSPD->Property->Set;

	//  -- excessive trace, use only for DEBUG --
	//	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- Start\n";
	//	#endif

	if (IsEqualGUID (PROPSETID_VIDCAP_CROSSBAR, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- Crossbar\n";
		#endif
		m_Crossbar.GetProperty(pSrb);
	}
	else if (IsEqualGUID (PROPSETID_TUNER, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- Tuner\n";
		#endif
		m_Tuner.GetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEOPROCAMP, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- Video Procamp\n";
		#endif
		m_ProcAmp.GetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_CAMERACONTROL, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- Camera Control\n";
		#endif
		m_CameraControl.GetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_TVAUDIO, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- Audio Control\n";
		#endif
		m_Audio.GetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEODECODER, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- Video Decoder\n";
		#endif
		m_AVDecoder.GetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEOCONTROL, Set))
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- Video Control\n";
		#endif
		m_VideoControl.GetProperty(pSrb);
	}
	else 
	{
		#ifdef __VIDCAP_TRACEON__
		TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- End, Invalid Property ID\n";
		#endif

		ASSERT(FALSE); // We should never get here/
		NextSrb(pSrb, STATUS_INVALID_PARAMETER);
		return;
	}

	NextSrb(pSrb);

	//  -- excessive trace, use only for DEBUG --
	//	#ifdef __VIDCAP_TRACEON__
	//	TVS << TraceInfo << "VideoCaptureAdapter::OnGetProperty -- End\n";
	//	#endif
}

void VideoCaptureAdapter::OnChangePowerState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VideoCaptureAdapter::OnChangePowerState \n";
	#endif

	m_PowerState = pSrb->CommandData.DeviceState;
	NextSrb(pSrb);
}

NTSTATUS VideoCaptureAdapter::Register(void)
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VideoCaptureAdapter::Register \n";
	#endif

	return StreamClassRegisterFilterWithNoKSPins(
		PDO(),
		&KSCATEGORY_CAPTURE,
		SIZEOF_ARRAY (CaptureMediums),
		(int*)CapturePinDirection,
		(KSIDENTIFIER*)CaptureMediums,
		NULL
		);
}

void VideoCaptureAdapter::OnGetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb)
{

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VideoCaptureAdapter::OnGetDataIntersection -- Start\n";
	#endif

    // -- called BEFORE open, so stream pointer comes from stream number,
	//		NOT the actual stream pointer in SRB --
	PSTREAM_DATA_INTERSECT_INFO IntersectInfo;
    ULONG                       StreamNumber;

    IntersectInfo = pSrb->CommandData.IntersectInfo;
    StreamNumber = IntersectInfo->StreamNumber;

	 //
    // Get the correct pointer to stream 
    //
	VidcapBaseStream* t_pStream = (VidcapBaseStream*)GetStream(StreamNumber);
    if (NULL == t_pStream)
    {
		NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
        return;
    }

	NextSrb(pSrb, t_pStream->GetDataIntersection(pSrb));

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VideoCaptureAdapter::OnGetDataIntersection -- End\n";
	#endif

}
