// KNdisPnpMode.h: interface for the KNdisPnpMode class.
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#if !defined(AFX_KNDISPNPMODE_H__79B03763_95F0_11D3_9002_00C04F7445B7__INCLUDED_)
#define AFX_KNDISPNPMODE_H__79B03763_95F0_11D3_9002_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KNdisPnpMode (NDIS 5 only)
//
// KNdisPnpMode encapsulates the current power state of an adapter or a
// filter driver.
//
// An instance of KNdisPnpMode is usually included as a data member of
// KNdisMiniAdapter-derived classes. The template parameter A is this
// adapter class.
//
// KNdisPnpMode also defines a number of helper static methods (such as IsPmAware),
// which return adapter properties defined in the KNdisAdapterTraits class
// for the given adapter class A.
//
template <class A>
class KNdisPnpMode
{
public:
	// Contrsuctor. Initializes the object to the default, D0, power state.
	KNdisPnpMode() : m_State(NdisDeviceStateD0) {}

	// Assign current power state
	NDIS_DEVICE_POWER_STATE operator=(NDIS_DEVICE_POWER_STATE s) { return (m_State=s); }

	// Retrieve current power state (conversion to NDIS_DEVICE_POWER_STATE)
	operator NDIS_DEVICE_POWER_STATE() const { return m_State; }

	// Return 'true' if the power state is on (D0)
	inline bool IsOn() const { return m_State == NdisDeviceStateD0; }

	// Return 'true' if the power state is off (anything but D0)
	inline bool IsOff() const { return m_State > NdisDeviceStateD0; }


// helpers

	// Check if the adapter A is declared as PM-aware (based on KNdisAdapterTraits<A>)
	static inline bool IsPmAware(){ return KNdisAdapterTraits<A>::IsPmAware(); }

	// Check if the adapter A is declared as WakeUp LAN enabled (based on KNdisAdapterTraits<A>)
	static inline bool IsWakeUpEnabled()
		{ return KNdisAdapterTraits<A>::WakeUpMagicPacket() ||
					KNdisAdapterTraits<A>::WakeUpPattern()	||
						KNdisAdapterTraits<A>::WakeUpLinkChange();
		}

	// Return PNP capabilities of adapter (based on KNdisAdapterTraits<A>)
	static inline NDIS_STATUS GetCapabilities(PNDIS_PNP_CAPABILITIES pCaps) {
		if (IsPmAware()) {
			if (pCaps == NULL)
				return NDIS_STATUS_INVALID_LENGTH; // to keep NDISTest happy :-(
			pCaps->Flags = IsWakeUpEnabled() ? 1:0;
			pCaps->WakeUpCapabilities.MinMagicPacketWakeUp = KNdisAdapterTraits<A>::WakeUpMagicPacket();
			pCaps->WakeUpCapabilities.MinPatternWakeUp = KNdisAdapterTraits<A>::WakeUpPattern();
			pCaps->WakeUpCapabilities.MinLinkChangeWakeUp = KNdisAdapterTraits<A>::WakeUpLinkChange();
			return NDIS_STATUS_SUCCESS;
		}
		return NDIS_STATUS_NOT_SUPPORTED;
	}

	friend A;		// accessed by the OID cracker

protected:
// current power state
	NDIS_DEVICE_POWER_STATE	m_State;

	// Current wakeup enable mask (used by framework)
	inline ULONG GetCurrentWakeUpEnableMask() { return
		( (KNdisAdapterTraits<A>::WakeUpMagicPacket() >= m_State)?NDIS_PNP_WAKE_UP_MAGIC_PACKET:0 ) |
		( (KNdisAdapterTraits<A>::WakeUpPattern() >= m_State)?NDIS_PNP_WAKE_UP_PATTERN_MATCH:0 ) |
		( (KNdisAdapterTraits<A>::WakeUpLinkChange() >= m_State)?NDIS_PNP_WAKE_UP_LINK_CHANGE:0 ) ;
	}
};

//////////////////////////////////////////////////////////////////
// KNdisPowerTransition (NDIS 5 only)
//
// This helper class combines KNdisPnpMode with an event object.
// Thus, it implements a synchronization object for power state
// transitions. The object is typically  used by two threads: one monitoring
// the state (performing Wait()) and another posting transitions (performing Set()).
//
// Used primarily by IM drivers.
// Parameter T - adapter type (usually, KNdisFilterAdapter-based).
//
template <class T>
class KNdisPowerTransition : public KNdisPnpMode<T>, protected KNdisEvent
{
public:
	// Suspend until state transition (PASSIVE_LEVEL only)
	NDIS_DEVICE_POWER_STATE Wait(UINT MsToWait=0);

	// Set the new power state with signalling. Going OFF resets the event
	void Set(NDIS_DEVICE_POWER_STATE NewState);

	// Assign the state without signalling
	NDIS_DEVICE_POWER_STATE operator=(NDIS_DEVICE_POWER_STATE s) { return (m_State=s); }
};


//////////////////////////////////////////////////////////////////////
// KNdisPmPacketPattern (NDIS 5 only)
//
// KNdisPmPacketPattern is a wrapper around an array of NDIS_PM_PACKET_PATTERN.
//
// Usually used as a parameter object in appropriate NDIS requests
// (by protocols) and OID handlers (by miniports).
//
// NOTE: NDIS patterns are passed as a concatinated BLOBs of byte mask/value vectors
// each of var size. All we know is the total size of the entire blob.
// I chose to piggyback on DDK's STRING type to refer to such var length structs,
// although, the content is binary.
//
//typedef struct _NDIS_PM_PACKET_PATTERN
//{
//    ULONG  Priority;
//    ULONG  Reserved;
//    ULONG  MaskSize;
//    ULONG  PatternOffset;
//    ULONG  PatternSize;
//    ULONG  PatternFlags;
//} NDIS_PM_PACKET_PATTERN,  *PNDIS_PM_PACKET_PATTERN;
//
//
//
class KNdisPmPacketPattern {
public:
	// Constructor
	KNdisPmPacketPattern(PNDIS_PM_PACKET_PATTERN pPattern=NULL, ULONG nSize=1)
		: m_pPattern(pPattern) , m_nSize(nSize) {}

	// Methods. 'Ordinal' refers to the ordinal number of the pattern found at m_pPattern

	// Get Mask
	bool GetMask(OUT STRING& Value, IN ULONG Ordinal=0);
	// Get Mask
	bool GetData(OUT STRING& Value, IN ULONG Ordinal=0);

	// Set Mask
	bool SetMask(IN STRING& Value, IN ULONG Ordinal=0);
	// Set Mask
	bool SetData(IN STRING& Value, IN ULONG Ordinal=0);

protected:
	PNDIS_PM_PACKET_PATTERN	m_pPattern;		// points to the start of the var-el array
	ULONG					m_nSize;		// total size of the array
};


//////////////////////////////////////////////////////////////////
// Implementations (draft)

inline bool
KNdisPmPacketPattern::GetMask(OUT STRING& Value, IN ULONG /*Ordinal*/)
{
	if (!m_pPattern)
		return false;
	Value.Buffer = (PCHAR) (m_pPattern+1);
	Value.Length = Value.MaximumLength = (USHORT) m_pPattern->MaskSize;
	return true;
}

inline bool
KNdisPmPacketPattern::GetData(OUT STRING& Value, IN ULONG /*Ordinal*/)
{
	if (!m_pPattern)
		return false;
	Value.Buffer = PCHAR(m_pPattern) + m_pPattern->PatternOffset;
	Value.Length = Value.MaximumLength = (USHORT) m_pPattern->PatternSize;
	return true;
}

inline bool
KNdisPmPacketPattern::SetMask(OUT STRING&, IN ULONG /*Ordinal*/)
{
	ASSERT(!"TBD");
	return false;
}

inline bool
KNdisPmPacketPattern::SetData(OUT STRING&, IN ULONG /*Ordinal*/)
{
	ASSERT(!"TBD");
	return false;
}

///////////////////////////////////////////////////

template <class T> inline
NDIS_DEVICE_POWER_STATE KNdisPowerTransition<T>::Wait(UINT MsToWait)
{
	KNdisEvent::Wait(MsToWait);
	return m_State;
}

template <class T> inline
void KNdisPowerTransition<T>::Set(NDIS_DEVICE_POWER_STATE NewState)
{
	if (m_State < NewState)
		KNdisEvent::Reset(); // going stand by, reset the event
	else
		KNdisEvent::Set();
	m_State = NewState;
}

#endif // !defined(AFX_KNDISPNPMODE_H__79B03763_95F0_11D3_9002_00C04F7445B7__INCLUDED_)
