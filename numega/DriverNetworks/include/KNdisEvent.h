// KNdisEvent.h: interface for the KNdisEvent class.
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

#if !defined(AFX_KNDISEVENT_H__819D9678_9135_11D3_9001_00C04F7445B7__INCLUDED_)
#define AFX_KNDISEVENT_H__819D9678_9135_11D3_9001_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////
// KNdisEvent
//
// Encapsulates the event synchronization object
//
class KNdisEvent
{
	SAFE_DESTRUCTORS
public:
	inline KNdisEvent();

	inline void Set();
	inline void Reset();
	inline BOOLEAN Wait(UINT MsToWait=0);
	inline BOOLEAN WaitAndReset(UINT MsToWait=0);

protected:
	NDIS_EVENT m_Event;
};

////////////////////////////////////////////////////////////////////
// Implementation


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisEvent::KNdisEvent()
{
	NdisInitializeEvent(&m_Event);
}

///////////////////////////////////////////////////////////////////////
// Set the event
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
void KNdisEvent::Set()
{
	NdisSetEvent(&m_Event);
}


///////////////////////////////////////////////////////////////////////
// Reset the event
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
void KNdisEvent::Reset()
{
	NdisResetEvent(&m_Event);
}

///////////////////////////////////////////////////////////////////////
// Wait for event to be signaled
//
// Parameters:
// 	MsToWait             - number of milliseconds to wait before timeout
//						   or zero if no timeout
// Returns:
// 	Returns TRUE if the event was signaled, FALSE if timed out
// Comments:
// 	none
BOOLEAN KNdisEvent::Wait(UINT MsToWait)
{
	return NdisWaitEvent(&m_Event, MsToWait);
}

///////////////////////////////////////////////////////////////////////
// Wait for event to be signaled, then reset it
//
// Parameters:
// 	MsToWait             - number of milliseconds to wait before timeout,
//						   or zero if no timeout
// Returns:
// 	none
// Comments:
//	Emperically, "NDIS" events are mapped to kernel's NotificationEvent types.
//  To simulate a SynchronizationEvent-type behaviour we have to use this
//  WaitAndReset() method instead. Use with caution.
BOOLEAN KNdisEvent::WaitAndReset(UINT MsToWait)
{
	BOOLEAN bCool = NdisWaitEvent(&m_Event, MsToWait);
	NdisResetEvent(&m_Event);
	return bCool;
}

#endif // !defined(AFX_KNDISEVENT_H__819D9678_9135_11D3_9001_00C04F7445B7__INCLUDED_)
