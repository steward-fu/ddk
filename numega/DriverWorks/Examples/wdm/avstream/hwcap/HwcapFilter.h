// HwcapFilter.h: interface for the HwcapFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HWCAPFILTER_H__EFBAF9D6_7F12_4B30_B56C_5E525E5F28CE__INCLUDED_)
#define AFX_HWCAPFILTER_H__EFBAF9D6_7F12_4B30_B56C_5E525E5F28CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// HwcapFilter  
//
//
class HwcapFilter : public KsFilter
{
	SAFE_DESTRUCTORS
public:
	HwcapFilter() : KsFilter() {};
	~HwcapFilter() {};

// Event handlers. Declare & implement those of interest.
// In this pin-centric sample OnProcess() is provided in on the pin objects only

	NTSTATUS OnCreate(KIrp I) ;


// KS FILTER Properties. Implemenet non-defaults only

  const static GUID  sm_Categories[];
//const static KSNODE_DESCRIPTOR  sm_Nodes;[]
//const static KSTOPOLOGY_CONNECTION  sm_Connections[];
//const static KSCOMPONENTID  sm_ComponentId;

// My Data
};



//////////////////////////////////////////////////////////////////////////
// Definitions of custom FILTER properties. 
// Place them under #ifdef KAVSTREAM_MAIN to allow dimension-less array 
// initialization avoiding multiple definitions

#ifdef KAVSTREAM_MAIN

const GUID  HwcapFilter::sm_Categories[] =
{
	STATICGUIDOF( KSCATEGORY_VIDEO ),
	STATICGUIDOF( KSCATEGORY_CAPTURE )
};

//const KSNODE_DESCRIPTOR HwcapFilter::sm_Nodes[] =
//{
//    DEFINE_NODE_DESCRIPTOR( NULL, &NodeType, NULL )
//};


#endif // KAVSTREAM_MAIN

#endif // !defined(AFX_HWCAPFILTER_H__EFBAF9D6_7F12_4B30_B56C_5E525E5F28CE__INCLUDED_)
