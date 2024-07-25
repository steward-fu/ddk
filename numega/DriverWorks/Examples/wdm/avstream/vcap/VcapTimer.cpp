// VcapTimer.cpp: implementation of the VcapTimer class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

#include "Vcap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VcapTimer::VcapTimer() :
 KTimedCallback(), m_Ticks(0), m_pFilter(NULL), m_bOn(0)
{

}

VcapTimer::~VcapTimer()
{
	Stop();
}


///////////////////////////////////////////////////////////////////////
// VcapTimer::Start
//
// Parameters:
// 	Interval             - Time to trigger event, in 100-nanosecond units
// 	pFilter=NULL         - Filter object to trigger processing after expiration
// Returns:
// 	none
// Comments:
// 	Starts the timer. Interval==0 is a special case to resume from the stopped state
//
void VcapTimer::Start(REFERENCE_TIME Interval, PKSFILTER pFilter)
{
	if (Interval) {
		m_pFilter = pFilter;
		m_Interval = Interval;
	}
	else ASSERT(pFilter);

	m_Ticks = 0;

	KeQuerySystemTime(&m_StartTime);

	REFERENCE_TIME NextTime = m_StartTime.QuadPart + m_Interval;

	InterlockedExchange(&m_bOn, TRUE);

	KTimedCallback::Set(NextTime, LinkTo(TimerCallback), this);
}


///////////////////////////////////////////////////////////////////////
// VcapTimer::Stop
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Stops timer
void VcapTimer::Stop()
{
	if (InterlockedExchange(&m_bOn, FALSE))
		KTimedCallback::Cancel();
}

///////////////////////////////////////////////////////////////////////
// VcapTimer::Resume
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Resumes the stopped timer with existing parameters
void VcapTimer::Resume()
{
	if (InterlockedExchange(&m_bOn, TRUE)==0)
		Start(0);
}


///////////////////////////////////////////////////////////////////////
// VcapTimer::TimerCallback
//
// Parameters:
//	none
// Returns:
// 	none
// Comments:
// 	Reschedule callback and trigger KS processing
//
void VcapTimer::TimerCallback(PVOID, PVOID)
{
	// g_Trace.Trace(TraceInfo, "VcapTimer::TimerCallback #%ld\n", ULONG(m_Ticks));

	if (!m_bOn)
		return;
			  
	REFERENCE_TIME NextTime = m_StartTime.QuadPart + (++m_Ticks*m_Interval);

	KTimedCallback::Set(NextTime, LinkTo(TimerCallback), this);

	ASSERT(m_pFilter);
	if (m_pFilter)
		::KsFilterAttemptProcessing(m_pFilter, FALSE);
}
