// Adapter.cpp: implementation of the Stream1394Adapter class.
//
//////////////////////////////////////////////////////////////////////

#include <vdw.h>
#include <kstream.h>
#include <ksmedia.h>
#include "Adapter.h"


extern KDebugOnlyTrace t;


//////////////////////////////////////////////////////////////////////
// Static Initialization
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Stream1394Adapter:: topology definition ... used in registration
//  process	provide the characteristics of the Stream1394Adapter class
//	back to the	Stream Class Driver.
// 
const GUID Stream1394Adapter::Categories[] = {
    STATIC_KSCATEGORY_VIDEO,
    STATIC_KSCATEGORY_CAPTURE
};

const KSTOPOLOGY Stream1394Adapter::AdapterTopology = {
    SIZEOF_ARRAY(Categories),           // CategoriesCount
    (GUID*) &Categories,                // Categories
    0,                                  // TopologyNodesCount
    NULL,                               // TopologyNodes
    0,                                  // TopologyConnectionsCount
    NULL,                               // TopologyConnections
    NULL,                               // TopologyNodesNames
    0                                  // Reserved
};
/*
////////////////////////////////////////////////////////////////////////
// Stream1394Adapter:: static medium definitions ... used in registration
//  process	provide the characteristics of the Stream1394Adapter class
//	back to the	Stream Class Driver.
//
// Note: To allow multiple instances of the same piece of hardware,
// set the first ULONG after the GUID in the Medium to a unique value.

const KSPIN_MEDIUM Stream1394Adapter::CaptureMediums[] = {
    {M_GUID4,           0, 0},  // Pin 0  Analog Video In
    {STATIC_GUID_NULL,  0, 0},  // Pin 1  Capture
    {STATIC_GUID_NULL,  0, 0},  // Pin 2  Preview
};

const BOOL Stream1394Adapter::CapturePinDirection [] = {
    FALSE,                      // Input  Pin 0
    TRUE,                       // Output Pin 1
    TRUE,                       // Output Pin 2
};
*/
// ------------------------------------------------------------------------
// Array of all of the property sets supported by the adapter
// ------------------------------------------------------------------------

const KSPROPERTY_SET Stream1394Adapter::AdapterPropertyTable[] =
{
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_VIDEOPROCAMP,	// Set
		VideoProcAmp::NumProperties(),	// PropertiesCount
        VideoProcAmp::PropertyList(),	// PropertyItem
        0,								// FastIoCount
        NULL							// FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_CAMERACONTROL,
        CameraControl::NumProperties(),
        CameraControl::PropertyList(),
        0, 
        NULL
    )
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	Stream1394Adapter::Stream1394Adapter 
//  -- Called from Stream1394Minidriver::OnCreateAdapter in response to
//		SRB_INITIALIZE_DEVICE.
//		
Stream1394Adapter::Stream1394Adapter(
	KStreamMinidriver* Driver,
	PORT_CONFIGURATION_INFORMATION* pConfigInfo
	) : 
		KStreamAdapter(
			Driver,									// KStreamMinidriver* Driver,
			pConfigInfo,							// PORT_CONFIGURATION_INFORMATION* pConfigInfo,
			1,										// ULONG NumberOfStreams,
			(SIZEOF_ARRAY (AdapterPropertyTable)),	// ULONG NumPropertySets,
			(PKSPROPERTY_SET)AdapterPropertyTable,	// PKSPROPERTY_SET PropertySets,
			0,										// ULONG NumberOfEvents,
			NULL,									// PKSEVENT_SET Events,
			(PKSTOPOLOGY)&AdapterTopology			// PKSTOPOLOGY Topology
			)
{
	ASSERT(pConfigInfo);

	// Initialize the lower device
	m_Device.Initialize(pConfigInfo->PhysicalDeviceObject, pConfigInfo->RealPhysicalDeviceObject);
}


/////////////////////////////////////////////////////////////////////////////
//	Stream1394Adapter::Initialize -- SRB_INITIALIZE_DEVICE
//  -- Called from Stream1394Minidriver::OnCreateAdapter in response to
//		after successful construction of the object
//		
void Stream1394Adapter::Initialize()
{
	t << "Stream1394Adapter::Initialize\n";

	// initialize streams
	m_Video.InitializeStream();
	NTSTATUS status = STATUS_SUCCESS;
/*
// TODO do this when a stream is opened...

	// Allocate 1394 Isochronous resources
	NTSTATUS status = m_Device.AllocateIsochResources();
*/
	// Execute 1394 device-specific code to initialize the device.
	if (NT_SUCCESS(status))
		status = m_Device.InitializeHardware();
}


/////////////////////////////////////////////////////////////////////////////
//	Stream1394Adapter::GetStream 
//  -- returns a pointer to stream object given the stream number.   
//		MUST implement this pure virtual method.  -- called 
//		during processing of SRB_OPEN_STREAM and SRB_GET_STREAM_INFO 
//		
KStream* Stream1394Adapter::GetStream(ULONG StreamNumber)
{
	t << "Stream1394Adapter::GetStream Number:" << StreamNumber << "\n";

	switch(StreamNumber)
	{
	case 0:
		return &m_Video;
	default:
		return NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
//	Stream1394Adapter::OnGetDataIntersection -- SRB_GET_DATA_INTERSECTION
//		
void Stream1394Adapter::OnGetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "Stream1394Adapter::OnGetDataIntersection\n";

    // -- called BEFORE opening the stream (SRB_OPEN_STREAM), so stream 
	//		pointer comes from stream number,
	//		NOT the actual stream pointer in SRB --
	PSTREAM_DATA_INTERSECT_INFO IntersectInfo;
    ULONG                       StreamNumber;

    IntersectInfo = pSrb->CommandData.IntersectInfo;
    StreamNumber = IntersectInfo->StreamNumber;

	 //
    // Get the correct pointer to stream 
    //
	VideoStream* pStream = (VideoStream*) GetStream(StreamNumber);

	NextSrb(pSrb, pStream?pStream->GetDataIntersection(pSrb):STATUS_NOT_IMPLEMENTED);
}


/////////////////////////////////////////////////////////////////////////////
//	Stream1394Adapter::OnGetProperty -- SRB_GET_DEVICE_PROPERTY
//		
void Stream1394Adapter::OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "Stream1394Adapter::OnGetProperty ";

	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	GUID Set = pSPD->Property->Set;

	if (IsEqualGUID(PROPSETID_VIDCAP_VIDEOPROCAMP, Set)) {

		t << "Video Procamp\n";
		m_ProcAmp.GetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_CAMERACONTROL, Set)) {

		t << "Camera Control\n";
		m_CameraControl.GetProperty(pSrb);
	}
	else {

		t << "Invalid Property ID\n";
		ASSERT(FALSE); 
		NextSrb(pSrb, STATUS_INVALID_PARAMETER);
		return;
	}

	NextSrb(pSrb);
}


/////////////////////////////////////////////////////////////////////////////
//	Stream1394Adapter::OnSetProperty -- SRB_SET_DEVICE_PROPERTY
//		
void Stream1394Adapter::OnSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "Stream1394Adapter::OnSetProperty ";

	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
	GUID Set = pSPD->Property->Set;

	if (IsEqualGUID(PROPSETID_VIDCAP_VIDEOPROCAMP, Set)) {

		t << "Video Procamp\n";
		m_ProcAmp.SetProperty(pSrb);
	}
	else if (IsEqualGUID(PROPSETID_VIDCAP_CAMERACONTROL, Set)) {

		t << "Camera Control\n";
		m_CameraControl.SetProperty(pSrb);
	}
	else {

		t << "Invalid Property ID\n";
		ASSERT(FALSE); // We should never get here/
		NextSrb(pSrb, STATUS_INVALID_PARAMETER);
		return;
	}

	NextSrb(pSrb);
}


/////////////////////////////////////////////////////////////////////////////
//	Stream1394Adapter::OnSurpriseRemoval -- SRB_SURPRISE_REMOVAL
//		
void Stream1394Adapter::OnSurpriseRemoval(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	t << "Stream1394Adapter::OnSurpriseRemoval\n";

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}
