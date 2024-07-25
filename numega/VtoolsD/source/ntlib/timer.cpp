// timer.cpp - timer implementation for compatibility library
// Copyright (c) 1998, Compuware Corporation

#include <vdw.h>

NTSTATUS NTAPI KeWaitForSingleObject(
	IN PVOID  Object,
	IN KWAIT_REASON  WaitReason,
	IN KPROCESSOR_MODE  WaitMode,
	IN BOOLEAN  Alertable,
	IN PLARGE_INTEGER  Timeout 			/* optional */
	)
{
	ASSERT (Object != 0);
	ASSERT (KeGetCurrentIrql() != DISPATCH_LEVEL);

	PKTIMER pTimer = PKTIMER(Object);

	return pTimer->Wait();
}

VOID NTAPI KeInitializeTimer(IN PKTIMER Timer)
{
	KeInitializeTimerEx(Timer, NotificationTimer);
}

VOID NTAPI KeInitializeTimerEx(
	IN PKTIMER Timer,
	IN TIMER_TYPE Type
	)
{
	Timer->Initialize(Type);
}

BOOLEAN NTAPI KeSetTimer(
	IN PKTIMER  Timer,
	IN LARGE_INTEGER  DueTime,
	IN PKDPC  Dpc
	)
{
	BOOLEAN AlreadyQueued = Timer->m_Pending;

	if (AlreadyQueued)
		KeCancelTimer(Timer);

	Timer->Setup(DueTime, 0, Dpc);	
	return AlreadyQueued;
}

BOOLEAN NTAPI KeCancelTimer(IN PKTIMER Timer)
{
	BOOLEAN WasQueued = Timer->m_Pending;

	if (WasQueued)
	{
		Timer->m_Pending = FALSE;	
		Timer->Cancel();
		Timer->SetSignaledState(TRUE);
		if (Timer->m_Dpc) KeRemoveQueueDpc(Timer->m_Dpc);
	}

	return WasQueued;
}

BOOLEAN NTAPI KeSetTimerEx(
	IN PKTIMER  Timer,
	IN LARGE_INTEGER  DueTime,
	IN LONG  Period,
	IN PKDPC  Dpc 	
	)
{
	BOOLEAN AlreadyQueued = Timer->m_Pending;

	if (AlreadyQueued)
		KeCancelTimer(Timer);

	Timer->Setup(DueTime, Period, Dpc);	
	return AlreadyQueued;
}

BOOLEAN NTAPI KeReadStateTimer(IN PKTIMER Timer)
{
	return !Timer->m_Pending;
}

/////////////////////////////////////////////////////////////////

VOID KTIMER::Initialize(TIMER_TYPE Type)
{
	if (m_hSem != 0) Destroy_Semaphore(m_hSem);

	m_hSem = Create_Semaphore(0);
	m_Pending = FALSE;
	m_Type = Type;
	m_WaitCount = 0;
}


KTIMER::~KTIMER(void)
{
	SetSignaledState(TRUE);
	if (m_hSem) Destroy_Semaphore(m_hSem);
}


NTSTATUS KTIMER::Wait(void)
{
	KIRQL i;

	ASSERT (m_hSem);

	KeAcquireSpinLock(&m_Lock, &i);
	m_WaitCount++;
	KeReleaseSpinLock(&m_Lock, i);

	Wait_Semaphore(m_hSem, BLOCK_SVC_INTS);

	return STATUS_SUCCESS;
}

VOID KTIMER::Setup(
	IN LARGE_INTEGER  DueTime,
	IN LONG  Period,
	IN PKDPC  Dpc 
	)
{
	KIRQL i;

	KeAcquireSpinLock(&m_Lock, &i);

	LONGLONG Time = *(LONGLONG*)&DueTime;
	if (Time < 0)
	{
		Time = -Time;
		Time /= 10000; 	// 100ns to msec		
	}
	else
	{	
		ASSERT(FALSE); // only relative times allowed in compat lib
	}

	m_Dpc = Dpc;
	m_mSec = ULONG(Time);
	if (m_mSec == 0) m_mSec = 1;
	m_PeriodmSec = Period;

	KeReleaseSpinLock(&m_Lock, i);

	if ( (m_hSem != 0) && (m_WaitCount == 0))
	{
		Destroy_Semaphore(m_hSem);
		m_hSem = 0;
	}

	if (m_hSem == 0)
		m_hSem = Create_Semaphore(0);

	Set();
	m_Pending = TRUE;
}


VOID KTIMER::handler(
	VMHANDLE hVM, 
	THREADHANDLE hThread, 
	PCLIENT_STRUCT pRegs, 
	DWORD lagMsec
	)
{
	m_Pending = FALSE;

	if (m_Dpc)
	{
		KIRQL i;

		KeRaiseIrql(DISPATCH_LEVEL, &i);
		KeInsertQueueDpc(m_Dpc, 0, 0);
		KeLowerIrql(i);
	}

	if (m_PeriodmSec)
	{
		m_mSec = m_PeriodmSec;
		Set();
		m_Pending = TRUE;
	}
	
	SetSignaledState(FALSE);
}


VOID KTIMER::SetSignaledState(BOOLEAN doAll)
{
	KIRQL i;

	if ( (m_Type == NotificationTimer) || doAll)
	{
		while (m_WaitCount > 0)
		{
			KeAcquireSpinLock(&m_Lock, &i);
			m_WaitCount--;
			KeReleaseSpinLock(&m_Lock, i);

			Signal_Semaphore(m_hSem);
		}
	}
	else if (m_WaitCount)
	{
		KeAcquireSpinLock(&m_Lock, &i);
		m_WaitCount--;
		KeReleaseSpinLock(&m_Lock, i);

		Signal_Semaphore(m_hSem);
	}
}
