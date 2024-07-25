// CamPin.h: interface for the CamPin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMPIN_H__3B2F6C08_F243_4C9A_8747_6A7F33A28252__INCLUDED_)
#define AFX_CAMPIN_H__3B2F6C08_F243_4C9A_8747_6A7F33A28252__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "StreamFormat.h"
#include "KSubscriber.h"


extern KDebugOnlyTrace t;


class CamPin : public KsPin, public KSubscriberImpl<CamPin> 
{
	SAFE_DESTRUCTORS
public:

	CamPin():m_StopEvent(SynchronizationEvent),m_StartEvent(SynchronizationEvent){}
	virtual ~CamPin(){}

	CamLowerDevice* Device() { return &reinterpret_cast<CamDevice*>(GetDevice())->m_Device; }

// Event handlers of interest

	NTSTATUS  OnCreate(KIrp);
	NTSTATUS  OnSetDeviceState(KSSTATE To, KSSTATE From); 

	static NTSTATUS  OnIntersection (KIrp, PKSDATARANGE, PKSDATARANGE, ULONG, PVOID, PULONG);

// Subscriber handlers of interest

	void OnStartListenComplete(NTSTATUS Status, PVOID) {  
		t.Trace(TraceInfo,"CamPin::OnStartListenComplete, Status = %X\n", Status);
		m_StartEvent.Set(); 
	}
	void OnStopListenComplete(NTSTATUS Status, PVOID) { 
		t.Trace(TraceInfo,"CamPin::OnStopListenComplete, Status = %X\n", Status);
		Device()->CancelAllBuffers();
		m_StopEvent.Set();
	}

public:

// Custom Properties. Implement non-defaults only
// These are used to initialize the pin descriptor struct KSPIN_DESCRIPTOR during
//		DriverEntry processing.

	const static ULONG					sm_Flags =  KSPIN_FLAG_INITIATE_PROCESSING_ON_EVERY_ARRIVAL |
													// KSPIN_FLAG_ASYNCHRONOUS_PROCESSING |
													KSPIN_FLAG_PROCESS_IN_RUN_STATE_ONLY |
													KSPIN_FLAG_SOME_FRAMES_REQUIRED_FOR_PROCESSING;
													// KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING;

//	const static ULONG					sm_InstancesPossible = 1;
//	const static ULONG					sm_InstancesNecessary = 1;

	static const KSPIN_INTERFACE		sm_Interfaces[]; 
	static const KSPIN_MEDIUM			sm_Mediums[];
	static const PKSDATARANGE			sm_DataRanges[];
//	static const PKSDATARANGE			sm_ConstrainedDataRanges[];

	static const KSPIN_DATAFLOW			sm_DataFlow = KSPIN_DATAFLOW_OUT;
	static const KSPIN_COMMUNICATION	sm_Communication = KSPIN_COMMUNICATION_BOTH;
	static const GUID					sm_Category;
	static const GUID					sm_Name;    

	static const KSALLOCATOR_FRAMING_EX	sm_AllocatorFraming; 

// Data members
protected:
	
	KsVideoInfoHeader*					m_VideoInfoHeader;

	KEvent								m_StartEvent;
	KEvent								m_StopEvent;

};


//////////////////////////////////////////////////////////////////////////
// Definitions of custom PIN properties. 
// Place them under #ifdef KAVSTREAM_MAIN to allow dimension-less array 
// initialization avoiding multiple definitions

#ifdef KAVSTREAM_MAIN

// Interfaces
const KSPIN_INTERFACE CamPin::sm_Interfaces[] =
{
   {
      STATICGUIDOF(KSINTERFACESETID_Standard),
      KSINTERFACE_STANDARD_STREAMING,
      0
   }
};

// Mediums
const KSPIN_MEDIUM    CamPin::sm_Mediums[] =
{
   {
      STATICGUIDOF(KSMEDIUMSETID_Standard),
      KSMEDIUM_TYPE_ANYINSTANCE,
      0
   }
};


// Formats supported
const PKSDATARANGE   CamPin::sm_DataRanges[] =
{
    (PKSDATARANGE) &StreamMode_0,
    (PKSDATARANGE) &StreamMode_1,
    (PKSDATARANGE) &StreamMode_2,
    (PKSDATARANGE) &StreamMode_3,
    (PKSDATARANGE) &StreamMode_4,
    (PKSDATARANGE) &StreamMode_5
};

//const PKSDATARANGE VcapPin::sm_ConstrainedDataRanges[] =
//{
//};

// Category
const GUID			CamPin::sm_Category = KSCATEGORY_VIDEO;

// Name
const GUID			CamPin::sm_Name = PINNAME_VIDEO_CAPTURE;    

// Framing
DECLARE_SIMPLE_FRAMING_EX(	
	CamPin::sm_AllocatorFraming,
    STATICGUIDOF(KSMEMORY_TYPE_KERNEL_PAGED),
    KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY | 
    KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
    KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY,
    2,
    0,
    2 * PAGE_SIZE,
    2 * PAGE_SIZE
	);


#endif // KAVSTREAM_MAIN


#endif // !defined(AFX_CAMPIN_H__3B2F6C08_F243_4C9A_8747_6A7F33A28252__INCLUDED_)
