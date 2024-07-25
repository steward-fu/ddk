// CamFilter.h: interface for the CamFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMFILTER_H__1BBAD7D4_46F8_4B00_B629_1F825380ED2F__INCLUDED_)
#define AFX_CAMFILTER_H__1BBAD7D4_46F8_4B00_B629_1F825380ED2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CamFilter : public KsFilter, public KSubscriberImpl<CamFilter> 
{
	SAFE_DESTRUCTORS
public:

	CamFilter(){}
	virtual ~CamFilter(){}

	CamLowerDevice* Device() { return &reinterpret_cast<CamDevice*>(GetDevice())->m_Device; }

// Event handlers. Declare & implement those of interest only.

	NTSTATUS OnCreate(KIrp I);
	NTSTATUS OnClose(KIrp I);
	NTSTATUS OnProcess(PKSPROCESSPIN_INDEXENTRY pIndex);

// Callback event handlers from CameraLowerDevice class
	void OnReadFrameComplete(NTSTATUS Status, ULONG ulLen, PVOID pCtx);
	
// Data structures required by the Framework.
// Commenting out results in using the defaults (NULL) defined by KsFilter

  const static GUID						sm_Categories[];
//const static KSNODE_DESCRIPTOR		sm_Nodes;[]
  const static KSTOPOLOGY_CONNECTION	sm_Connections[];
//const static KSCOMPONENTID			sm_ComponentId;

};


//////////////////////////////////////////////////////////////////////////
// Definitions of custom FILTER properties. 
// Place them under #ifdef KAVSTREAM_MAIN to allow dimension-less array 
// initialization avoiding multiple definitions

#ifdef KAVSTREAM_MAIN

const GUID  CamFilter::sm_Categories[] =
{
	STATICGUIDOF( KSCATEGORY_VIDEO ),
	STATICGUIDOF( KSCATEGORY_CAPTURE )
};

//const KSNODE_DESCRIPTOR CamFilter::sm_Nodes[] =
//{
//    DEFINE_NODE_DESCRIPTOR( NULL, &NodeType, NULL )
//};

const KSTOPOLOGY_CONNECTION  CamFilter::sm_Connections[] = 
//	  -------------
//  2-|           |- 0 Video
//    |			  |
//  3-|	0 Node0 1 |- 1 Audio
//    -------------
{
    { KSFILTER_NODE, 3, 0, 0 },
//  { 0, 1, KSFILTER_NODE, 1 },
};

#endif // KAVSTREAM_MAIN


#endif // !defined(AFX_CAMFILTER_H__1BBAD7D4_46F8_4B00_B629_1F825380ED2F__INCLUDED_)
