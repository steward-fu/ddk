// VcapTimer.h: interface for the VcapTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCAPTIMER_H__1D5D52D6_C501_4B5E_A8A8_8F00CEC7CDE9__INCLUDED_)
#define AFX_VCAPTIMER_H__1D5D52D6_C501_4B5E_A8A8_8F00CEC7CDE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// VcapTimer
//
// Implements a periodic timer, which schedules KS processing for
// the simulated video capture
//
class VcapTimer : protected KTimedCallback 
{
	SAFE_DESTRUCTORS
public:
	VcapTimer();
	~VcapTimer();

	void Start(REFERENCE_TIME Interval, PKSFILTER pFilter=NULL);
	void Stop();
	void Resume();

	REFERENCE_TIME Ticks() const { return m_Ticks; }
	REFERENCE_TIME TickTime() const { return m_Ticks*m_Interval;}
	LONG IsOn() const { return m_bOn; }

protected:
	MEMBER_DPC (VcapTimer, TimerCallback);

#ifdef __COMMENT_ONLY
	VOID TimerCallback(PVOID Arg1, PVOID Arg2);		
#endif __COMMENT_ONLY

	REFERENCE_TIME	m_Ticks;		// incremental tick counter
	REFERENCE_TIME	m_Interval;		// in 100-nanosecond units
	LARGE_INTEGER	m_StartTime;
	LONG			m_bOn;
	PKSFILTER		m_pFilter;
};


#endif // !defined(AFX_VCAPTIMER_H__1D5D52D6_C501_4B5E_A8A8_8F00CEC7CDE9__INCLUDED_)
