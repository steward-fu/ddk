// capinstrm.cpp - analog video input stream of Video Capture sample
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
#include "capinstream.h"
#include "mediums.h"


const KS_DATARANGE_ANALOGVIDEO VidcapInputStream::m_StreamFormatAnalogVideo = 
{
    // KS_DATARANGE_ANALOGVIDEO
    {   
        sizeof (KS_DATARANGE_ANALOGVIDEO),      // FormatSize
        0,                                      // Flags
        sizeof (KS_TVTUNER_CHANGE_INFO),        // SampleSize
        0,                                      // Reserved

        STATIC_KSDATAFORMAT_TYPE_ANALOGVIDEO,   // aka MEDIATYPE_AnalogVideo
        0x482dde2, 0x7817, 0x11cf, 0x8a, 0x3, 0x0, 0xaa, 0x0, 0x6e, 0xcb, 0x65,  // WILDCARD
        STATIC_KSDATAFORMAT_SPECIFIER_ANALOGVIDEO, // aka FORMAT_AnalogVideo
    },
    // KS_ANALOGVIDEOINFO
    {
        0, 0, 720, 480,         // rcSource;                
        0, 0, 720, 480,         // rcTarget;        
        720,                    // dwActiveWidth;   
        480,                    // dwActiveHeight;  
        0,                      // REFERENCE_TIME  AvgTimePerFrame; 
    }
};

//---------------------------------------------------------------------------
//  STREAM_AnalogVideoInput Formats
//---------------------------------------------------------------------------

const PKSDATAFORMAT VidcapInputStream::m_StreamFormats[] = 
{
    (PKSDATAFORMAT) &m_StreamFormatAnalogVideo,
};

// -----------------------------------------------------------------
// STREAM_AnalogVideoInput
// -----------------------------------------------------------------
const KSPIN_MEDIUM VidcapInputStream::m_Medium =
    {M_GUID4,  0, 0};  // Corresponds to Pin 9 of Crossbar....


const HW_STREAM_INFORMATION VidcapInputStream::m_StreamInfo =
{
    // HW_STREAM_INFORMATION -------------------------------------------
    1,                                      // NumberOfPossibleInstances
    KSPIN_DATAFLOW_IN,                      // DataFlow
    TRUE,                                   // DataAccessible
    SIZEOF_ARRAY(m_StreamFormats),          // NumberOfFormatArrayEntries
    (PKSDATAFORMAT*)m_StreamFormats,        // StreamFormatsArray
    0,                                      // ClassReserved[0]
    0,                                      // ClassReserved[1]
    0,                                      // ClassReserved[2]
    0,                                      // ClassReserved[3]
    0,                                      // NumStreamPropArrayEntries
    0,                                      // StreamPropertiesArray
    0,                                      // NumStreamEventArrayEntries;
    0,                                      // StreamEventsArray;
    (GUID *) &PINNAME_VIDEO_ANALOGVIDEOIN,  // Category
    (GUID *) &PINNAME_VIDEO_ANALOGVIDEOIN,  // Name
    1,                                      // MediumsCount
    (PKSPIN_MEDIUM)&m_Medium,               // Mediums
	FALSE,									// BridgeStream
};           


VidcapInputStream::VidcapInputStream(void) :
	VidcapBaseStream()
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapInputStream::VidcapInputStream -- Constructing\n";
	#endif
}

// -- stream initialization code --
void VidcapInputStream::InitializeStream()
{
	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapInputStream::InitializeStream -- calling InitializeBaseClass\n";
	#endif
	InitializeBaseClass(m_StreamInfo, 0);
}

// -- methods called from DispatchData method for hardware read/write --
void VidcapInputStream::OnWrite(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    // 
    // This data packet contains the channel change information
    // passed on the AnalogVideoIn stream.  Devices which support
    // VBI data streams need to pass this info on their output pins.
    //

	#ifdef __VIDCAP_TRACEON__
	TVS << TraceInfo << "VidcapInputStream::OnWrite -- called to change input channel\n";
	#endif

	PKSSTREAM_HEADER   pDataPacket = pSrb->CommandData.DataBufferArray;
    

    if (pDataPacket->FrameExtent == sizeof (KS_TVTUNER_CHANGE_INFO)) 
		((VideoCaptureAdapter*)Adapter())->ChangeChannel((PKS_TVTUNER_CHANGE_INFO)pDataPacket);
	
	NextSrb(pSrb);

}

void VidcapInputStream::OnSetState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __VIDCAP_TRACEON__
	TVS.Trace(TraceInfo, "VidcapInputStream::OnSetState -- new state %d\n", pSrb->CommandData.StreamState);
	#endif

	// -- don't want to start another timer... keep it simple --
    m_eState = pSrb->CommandData.StreamState;
	NextSrb(pSrb);
}

/*
VOID VidcapInputStream::GetInformation(PHW_STREAM_INFORMATION pInfo)
{
	pInfo->NumberOfPossibleInstances =	m_StreamInfo.NumberOfPossibleInstances;
	pInfo->DataFlow =					m_StreamInfo.DataFlow;
	pInfo->DataAccessible =				m_StreamInfo.DataAccessible;
	pInfo->NumberOfFormatArrayEntries = m_StreamInfo.NumberOfFormatArrayEntries;
	pInfo->StreamFormatsArray =			m_StreamInfo.StreamFormatsArray;
	pInfo->Category =					m_StreamInfo.Category;
	pInfo->Name =						m_StreamInfo.Name;
	pInfo->MediumsCount =				m_StreamInfo.MediumsCount;
	pInfo->Mediums =					m_StreamInfo.Mediums;
	pInfo->BridgeStream =				m_StreamInfo.BrigeStream;
}
*/

