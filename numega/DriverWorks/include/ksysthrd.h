// ksysthrd.h - include file for class KSystemThread
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

class KSystemThread : public KDispatcherObject
{
	SAFE_DESTRUCTORS
public:
// Called by thread creator
	KSystemThread(void);

	NTSTATUS Start(
		PKSTART_ROUTINE ThreadMain,
		PVOID Parameter = NULL,
		ULONG Access = THREAD_ALL_ACCESS
		);

	NTSTATUS ExitStatus(void);

#if !_WDM_
	LONG SetBasePriority(LONG delta);
#endif
	KPRIORITY SetPriority(KPRIORITY priority);

// Called by thread
	PVOID GetParameter(void);
	VOID Terminate(NTSTATUS status);

// Called by either
	operator PKTHREAD (void);

//Overides wait so that Win9x won't crash
	NTSTATUS Wait(
		KPROCESSOR_MODE mode=KernelMode,
		BOOLEAN bAlertable=TRUE,
		PLARGE_INTEGER pTimeOut=NULL,
		KWAIT_REASON reason=Executive
		);


// Data members
	PVOID m_parameter;
	NTSTATUS m_ExitStatus;
	BOOLEAN m_started;
	BOOLEAN m_terminated;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

inline NTSTATUS KSystemThread::Wait(
		KPROCESSOR_MODE mode,
		BOOLEAN bAlertable,
		PLARGE_INTEGER pTimeOut,
		KWAIT_REASON reason)
{
#if _WDM_ && !defined(NDIS_WDM)
	if(_bWindows98_)
	{
		ASSERT(!"Can't wait on Threads in Win9X.  Use a KEvent instead");
		return STATUS_NOT_SUPPORTED;
	}
	else
#endif
		return KDispatcherObject::Wait(mode, bAlertable, pTimeOut, reason);
}


inline NTSTATUS KSystemThread::ExitStatus(void)
{
	return m_ExitStatus;
}

#if !_WDM_
inline LONG KSystemThread::SetBasePriority(LONG delta)
{
	return KeSetBasePriorityThread((PKTHREAD)m_pObject, delta);
}
#endif

inline KPRIORITY KSystemThread::SetPriority(KPRIORITY priority)
{
	return KeSetPriorityThread((PKTHREAD)m_pObject, priority);
}

inline PVOID KSystemThread::GetParameter(void)
{
	return m_parameter;
}

inline VOID KSystemThread::Terminate(NTSTATUS status)
{
	m_terminated = TRUE;
	m_ExitStatus = status;
	PsTerminateSystemThread(status);
	BOUNDS_CHECKER(TERMINATED_THREAD, (this, status));

}

inline KSystemThread::operator PKTHREAD (void)
{
	return (PKTHREAD)m_pObject;
}

//////////////////////////////////////////////////////////////////////
//
// Macro MEMBER_THREAD
//
// To make a the thread function a member of any class, use the following
// macro inside the definition of the class. If the thread function member
// name is X, then the call to KSystemThread::Start must be passed
// LinkTo(X) as the address of start routine, and the address of the object
// (i.e."this") must be the parameter for the start routine.
//
// The first parameter to the macro is the name of class in which the
// thread function member is being declared. The second parameter is the
// name of the thread function.
//
// The callback member declared by this macro has the following prototype:
//
//		VOID ThreadMember(void)
//
// This macro may be used inside any class definition, including classes
// derived from KDevice and KSystemThread. It actually declares two members:
// One is a non-static member which is used as the thread function, and
// the other is a static member whose declaration matches that required by the
// system for a thread start function. The static member invokes the non-static
// member, relying on the fact that the object address was passed as
// parameter Parameter to KSystemThread::Start.
//

#define MEMBER_THREAD(objclass, func)				\
	VOID func(void);								\
	static VOID LinkTo(func)(PVOID StartContext)	\
	{												\
		PVOID p;									\
		p = ((KSystemThread*)StartContext)->		\
								GetParameter();		\
		((objclass*)p)->func();						\
	}
