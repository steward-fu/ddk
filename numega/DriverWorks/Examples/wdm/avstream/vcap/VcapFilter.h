// VcapFilter.h: interface for the VcapFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCAPFILTER_H__EFBAF9D6_7F12_4B30_B56C_5E525E5F28CE__INCLUDED_)
#define AFX_VCAPFILTER_H__EFBAF9D6_7F12_4B30_B56C_5E525E5F28CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// VcapFilter  
//
//
class VcapFilter : public KsFilter
{
	SAFE_DESTRUCTORS
public:
	VcapFilter() : KsFilter() {};
	~VcapFilter() {};

// Event handlers. Declare & implement those of interest only.

	NTSTATUS OnCreate(KIrp I) ;
	NTSTATUS OnClose(KIrp I)  ;
	NTSTATUS OnProcess(PKSPROCESSPIN_INDEXENTRY pIndex) ;
//	NTSTATUS OnReset();


// Data structures required by the Framework.
// Commenting those out results in using the defaults (NULL) defined by KsFilter

  const static GUID  sm_Categories[];
//const static KSNODE_DESCRIPTOR  sm_Nodes;[]
  const static KSTOPOLOGY_CONNECTION  sm_Connections[];
//const static KSCOMPONENTID  sm_ComponentId;

// My Data
};



//////////////////////////////////////////////////////////////////////////
// Definitions of custom FILTER properties. 
// Place them under #ifdef KAVSTREAM_MAIN to allow dimension-less array 
// initialization avoiding multiple definitions

#ifdef KAVSTREAM_MAIN

const GUID  VcapFilter::sm_Categories[] =
{
	STATICGUIDOF( KSCATEGORY_VIDEO ),
	STATICGUIDOF( KSCATEGORY_CAPTURE )
};

//const KSNODE_DESCRIPTOR VcapFilter::sm_Nodes[] =
//{
//    DEFINE_NODE_DESCRIPTOR( NULL, &NodeType, NULL )
//};

const KSTOPOLOGY_CONNECTION  VcapFilter::sm_Connections[] = 
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

#endif // !defined(AFX_VCAPFILTER_H__EFBAF9D6_7F12_4B30_B56C_5E525E5F28CE__INCLUDED_)
