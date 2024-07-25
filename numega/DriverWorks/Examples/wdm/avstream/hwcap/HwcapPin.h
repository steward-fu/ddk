// HwcapPin.h: interface for the HwcapPin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HWCAPPIN_H__3B41C636_4EB6_47FC_A42C_F18BA23834BF__INCLUDED_)
#define AFX_HWCAPPIN_H__3B41C636_4EB6_47FC_A42C_F18BA23834BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HwcapSim.h"

//////////////////////////////////////////////////////////////////////
// HwcapPin
//
// Pin class - acts as both AvStream pin event processor (OnXxx() methods) 
// and  the sink for simulated hardware interrupts (the DPC() method)
//
class HwcapPin : public KsPin, public HwcapSimulator::IInterrupt
{
	SAFE_DESTRUCTORS
public:
	HwcapPin();
	virtual ~HwcapPin();

// Event handlers of interest

	NTSTATUS  OnCreate(KIrp);
	NTSTATUS  OnSetDeviceState(KSSTATE, KSSTATE); 
	NTSTATUS  OnProcess();

	static NTSTATUS  OnSetDataFormat(PKSPIN, const KSDATARANGE*, const KSATTRIBUTE_LIST*);
	NTSTATUS  OnSetDataFormat(PKSDATAFORMAT, PKSMULTIPLE_ITEM, const KSDATARANGE*, const KSATTRIBUTE_LIST*);

	static NTSTATUS  OnIntersection (KIrp, PKSDATARANGE, PKSDATARANGE, ULONG, PVOID, PULONG);

// KS PIN Properties. Implemenet non-defaults only

	// Pin Flags
	const static ULONG	sm_Flags =  KSPIN_FLAG_GENERATE_MAPPINGS /* stream ptr to use s/g mappings */ |      
									KSPIN_FLAG_DISPATCH_LEVEL_PROCESSING |
									KSPIN_FLAG_PROCESS_IN_RUN_STATE_ONLY;

//	const static ULONG	sm_InstancesPossible = 1;
//	const static ULONG	sm_InstancesNecessary = 1;

//	static const KSPIN_INTERFACE sm_Interfaces[]; 
//	static const KSPIN_MEDIUM    sm_Mediums[];
	static const PKSDATARANGE    sm_DataRanges[];
//	static const PKSDATARANGE sm_ConstrainedDataRanges[];

	static const KSPIN_DATAFLOW				sm_DataFlow = KSPIN_DATAFLOW_OUT;
	static const KSPIN_COMMUNICATION		sm_Communication = KSPIN_COMMUNICATION_BOTH;
	static const GUID						sm_Category;
	static const GUID						sm_Name;    

	static const KSALLOCATOR_FRAMING_EX		sm_AllocatorFraming; 

// My methods

	// IInterrupt handler: DPC simulation; called asynchronously from Hardware simulator

	virtual void DPC();

// Stream Pointer Context:
	struct StreamPointerContext_t {
		PUCHAR VirtualBuffer; // saves the data buffer ptr of the current frame
	};

protected:

// Internal Helper methods

	inline NTSTATUS ReturnPending() { m_bPending=TRUE; return STATUS_PENDING; }
	void DeleteClones();


// My data

protected:

	friend class HwcapFilter;
	friend class HwcapSimulator;

	// video format descriptions
	
	static const ULONG	sm_X = 320;		// image width
	static const ULONG	sm_Y = 240;		// image height

	static const KS_DATARANGE_VIDEO sm_RGB24_Capture;
	static const KS_DATARANGE_VIDEO sm_YUV_Capture;

	HwcapSimulator*				m_pHardware;				// h/w simulating timer

	KsVideoInfoHeader*			m_VideoInfoHeader;			// video header cached in OnCreate()

	BOOLEAN						m_bPending;					// when TRUE kick off processing at the end of DPC

	KsStreamPointer				m_Current;					// Currently processed stream ptr. Can be NULL

	IKsReferenceClock*			m_pClock;					// assigned KS clock interface (if any)

};

/*class HwcapPinDPC : public HwcapPin, public HwcapSimulator::IInterrupt
{
public: 	virtual void DPC();
			HwcapPinDPC() : HwcapPin() {}
};
*/


//////////////////////////////////////////////////////////////////////////
// Definitions of custom PIN properties. 
// Place them under #ifdef KAVSTREAM_MAIN to allow dimension-less array 
// initialization avoiding multiple definitions

#ifdef KAVSTREAM_MAIN


// Formats supported
const PKSDATARANGE   HwcapPin::sm_DataRanges[] =
{
	(PKSDATARANGE) &sm_RGB24_Capture,
	(PKSDATARANGE) &sm_YUV_Capture,
};


// Category
const GUID			HwcapPin::sm_Category = KSCATEGORY_VIDEO;

// Name
const GUID			HwcapPin::sm_Name = PINNAME_VIDEO_CAPTURE;    

// Framing
DECLARE_SIMPLE_FRAMING_EX(	
	HwcapPin::sm_AllocatorFraming,
    STATICGUIDOF(KSMEMORY_TYPE_KERNEL_NONPAGED),
    KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY | 
    KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY,
    2,
    0,
    2 * PAGE_SIZE,
    2 * PAGE_SIZE);

#endif // KAVSTREAM_MAIN

#endif // !defined(AFX_HWCAPPIN_H__3B41C636_4EB6_47FC_A42C_F18BA23834BF__INCLUDED_)
