// VcapPin.h: interface for the VcapPin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCAPPIN_H__3B41C636_4EB6_47FC_A42C_F18BA23834BF__INCLUDED_)
#define AFX_VCAPPIN_H__3B41C636_4EB6_47FC_A42C_F18BA23834BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VcapTimer.h"

class VcapPin : public KsPin
{
	SAFE_DESTRUCTORS
public:
	VcapPin();
	virtual ~VcapPin();

// Event handlers of interest

	NTSTATUS  OnCreate(KIrp);
	NTSTATUS  OnSetDeviceState(KSSTATE To, KSSTATE From); 

	static NTSTATUS  OnIntersection (KIrp, PKSDATARANGE, PKSDATARANGE, ULONG, PVOID, PULONG);


// Custom Properties. Implemenet non-defaults only

	const static ULONG	sm_Flags =  KSPIN_FLAG_DO_NOT_INITIATE_PROCESSING |
									KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING;

//	const static ULONG	sm_InstancesPossible = 1;
//	const static ULONG	sm_InstancesNecessary = 1;

	static const KSPIN_INTERFACE sm_Interfaces[]; 
	static const KSPIN_MEDIUM    sm_Mediums[];
	static const PKSDATARANGE    sm_DataRanges[];
//	static const PKSDATARANGE sm_ConstrainedDataRanges[];

	static const KSPIN_DATAFLOW				sm_DataFlow = KSPIN_DATAFLOW_OUT;
	static const KSPIN_COMMUNICATION		sm_Communication = KSPIN_COMMUNICATION_BOTH;
	static const GUID						sm_Category;
	static const GUID						sm_Name;    

	static const KSALLOCATOR_FRAMING_EX		sm_AllocatorFraming; 

// My methods

	virtual ULONG GetImage(PKSSTREAM_HEADER pStream, OUT PVOID Data, IN ULONG ByteCount);

// My data

protected:

	friend class VcapFilter;

	// video format descriptions
	
	static const ULONG	sm_X = 320;		// image width
	static const ULONG	sm_Y = 240;		// image height

	static const KS_DATARANGE_VIDEO sm_RGB24_Capture;
	static const KS_DATARANGE_VIDEO sm_YUV_Capture;

	VcapTimer					m_Timer;					// simulating timer

	KsVideoInfoHeader*			m_VideoInfoHeader;			// video header cached in OnCreate()
	LONGLONG					m_PictureNumber;			//
    LONGLONG					m_ulVideoDroppedFrames;		// frames dropped due to no buffer avail
	UCHAR						m_LineBuffer[720*3];		// temp line storage for synth - large enough

	// User-settable properties
	bool						m_bAnimate;				// animate image

};


//////////////////////////////////////////////////////////////////////////
// Definitions of custom PIN properties. 
// Place them under #ifdef KAVSTREAM_MAIN to allow dimension-less array 
// initialization avoiding multiple definitions

#ifdef KAVSTREAM_MAIN

// Interfaces
const KSPIN_INTERFACE VcapPin::sm_Interfaces[] =
{
   {
      STATICGUIDOF(KSINTERFACESETID_Standard),
      KSINTERFACE_STANDARD_STREAMING,
      0
   }
};

// Mediums
const KSPIN_MEDIUM    VcapPin::sm_Mediums[] =
{
   {
      STATICGUIDOF(KSMEDIUMSETID_Standard),
      KSMEDIUM_TYPE_ANYINSTANCE,
      0
   }
};


// Formats supported
const PKSDATARANGE   VcapPin::sm_DataRanges[] =
{
	(PKSDATARANGE) &sm_RGB24_Capture,
	(PKSDATARANGE) &sm_YUV_Capture,
};

//const PKSDATARANGE VcapPin::sm_ConstrainedDataRanges[] =
//{
//};

// Category
const GUID			VcapPin::sm_Category = KSCATEGORY_VIDEO;

// Name
const GUID			VcapPin::sm_Name = PINNAME_VIDEO_CAPTURE;    

// Framing
DECLARE_SIMPLE_FRAMING_EX(	
	VcapPin::sm_AllocatorFraming,
    STATICGUIDOF(KSMEMORY_TYPE_KERNEL_PAGED),
    KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY | 
    KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
    KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY,
    2,
    0,
    2 * PAGE_SIZE,
    2 * PAGE_SIZE);



#endif // KAVSTREAM_MAIN

#endif // !defined(AFX_VCAPPIN_H__3B41C636_4EB6_47FC_A42C_F18BA23834BF__INCLUDED_)
