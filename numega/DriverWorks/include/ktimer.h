// ktimer.h - Include file for class KTimer and KDeferredCall
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __KTIMER__
#define __KTIMER__

#if defined __BORLANDC__
#pragma warn -hid
#endif

class KTimer : public  KDispatcherObject
{

// Construction
public:
	SAFE_DESTRUCTORS
	KTimer(void);
	VOID Initialize(void);

	KTimer(TIMER_TYPE type);
	VOID Initialize(TIMER_TYPE type);

	~KTimer(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);

// Methods
public:
 	BOOLEAN State(void);

	BOOLEAN Cancel(void);

	virtual BOOLEAN Set(LARGE_INTEGER DueTime);
	virtual BOOLEAN Set(LONGLONG DueTime);
	virtual BOOLEAN SetPeriodic(LARGE_INTEGER DueTime, LONG PeriodmSec);
	virtual BOOLEAN SetPeriodic(LONGLONG DueTime, LONG PeriodmSec);

// Data
protected:
	KTIMER m_timer;
};

////////////////////////////////////////////////////////////////////////////////////

class KTimedCallback : public KTimer
{

// Construction
public:
	SAFE_DESTRUCTORS
	KTimedCallback(void);

// Methods
public:
	virtual BOOLEAN Set(
		LARGE_INTEGER DueTime,
		PKDEFERRED_ROUTINE Callback,
		PVOID Context=NULL );

	virtual BOOLEAN Set(
		LONGLONG DueTime,
		PKDEFERRED_ROUTINE Callback,
		PVOID Context=NULL );

	virtual BOOLEAN SetPeriodic(
		LARGE_INTEGER DueTime,
		LONG PeriodmSec,
		PKDEFERRED_ROUTINE Callback,
		PVOID Context=NULL );

	virtual BOOLEAN SetPeriodic(
		LONGLONG DueTime,
		LONG PeriodmSec,
		PKDEFERRED_ROUTINE Callback,
		PVOID Context=NULL );

// Data
protected:
	KDPC m_dpc;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

inline KTimer::KTimer(void) : KDispatcherObject(&m_timer)
{
	KeInitializeTimer(&m_timer);
}

inline VOID KTimer::Initialize(void)
{
	KDispatcherObject::Initialize(&m_timer);
	KeInitializeTimer(&m_timer);
}

inline KTimer::KTimer(TIMER_TYPE  type) : KDispatcherObject(&m_timer)
{
	KeInitializeTimerEx(&m_timer, type);
}

inline VOID KTimer::Initialize(TIMER_TYPE  type)
{
	KDispatcherObject::Initialize(&m_timer);
	KeInitializeTimerEx(&m_timer, type);
}

inline VOID KTimer::Invalidate(void)
{
	Cancel();
	KDispatcherObject::Invalidate();
}

inline BOOLEAN KTimer::State(void)
{
	return KeReadStateTimer(&m_timer);
}

inline BOOLEAN KTimer::Cancel(void)
{
	return KeCancelTimer(&m_timer);
}

inline KTimedCallback::KTimedCallback(void) : KTimer()
{
}

#if defined __BORLANDC__
#pragma warn +hid
#endif

#endif
