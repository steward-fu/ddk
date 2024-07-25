// ktimer.cpp - implementation of class KTimer and KTimedCallback
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

#include <vdw.h>

BOOLEAN KTimer::Set(LARGE_INTEGER DueTime)
{
	return KeSetTimer(&m_timer, DueTime, NULL);
}

BOOLEAN KTimer::Set(LONGLONG DueTime)
{
	LARGE_INTEGER LargeDueTime;

	LargeDueTime.QuadPart = (ULONGLONG)DueTime;

	return KeSetTimer(&m_timer, LargeDueTime, NULL);
}

BOOLEAN KTimer::SetPeriodic(LARGE_INTEGER DueTime, LONG PeriodmSec)
{
	return KeSetTimerEx(&m_timer, DueTime, PeriodmSec, NULL);
}

BOOLEAN KTimer::SetPeriodic(LONGLONG DueTime, LONG PeriodmSec)
{
	LARGE_INTEGER LargeDueTime;

	LargeDueTime.QuadPart = (ULONGLONG)DueTime;

	return KeSetTimerEx(&m_timer, LargeDueTime, PeriodmSec, NULL);
}

BOOLEAN KTimedCallback::Set(
	LARGE_INTEGER DueTime,
	PKDEFERRED_ROUTINE Callback,
	PVOID Context)
{
	KeInitializeDpc(&m_dpc, Callback, Context);
	return KeSetTimer(&m_timer, DueTime, &m_dpc);
}

BOOLEAN KTimedCallback::Set(
	LONGLONG DueTime,
	PKDEFERRED_ROUTINE Callback,
	PVOID Context)
{
	LARGE_INTEGER LargeDueTime;

	KeInitializeDpc(&m_dpc, Callback, Context);

	LargeDueTime.QuadPart = (ULONGLONG)DueTime;

	return KeSetTimer(&m_timer, LargeDueTime, &m_dpc);
}


BOOLEAN KTimedCallback::SetPeriodic(
	LARGE_INTEGER DueTime,
	LONG PeriodmSec,
	PKDEFERRED_ROUTINE Callback,
	PVOID Context)
{
	KeInitializeDpc(&m_dpc, Callback, Context);
	return KeSetTimerEx(&m_timer, DueTime, PeriodmSec, &m_dpc);
}

BOOLEAN KTimedCallback::SetPeriodic(
	LONGLONG DueTime,
	LONG PeriodmSec,
	PKDEFERRED_ROUTINE Callback,
	PVOID Context)
{
	LARGE_INTEGER LargeDueTime;
	LargeDueTime.QuadPart = (ULONGLONG)DueTime;

	KeInitializeDpc(&m_dpc, Callback, Context);
	return KeSetTimerEx(&m_timer, LargeDueTime, PeriodmSec, &m_dpc);
}