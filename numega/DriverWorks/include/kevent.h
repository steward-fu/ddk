// kevent.h - include file for class KEvent
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

#ifndef __KEVENT__
#define __KEVENT__

class KEvent : public KDispatcherObject
{

// Construction
public:
	SAFE_DESTRUCTORS
	KEvent();
	KEvent(PKEVENT);
	KEvent(HANDLE hEvent, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
	VOID Initialize(PKEVENT);
	VOID Initialize(HANDLE hEvent, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);

#if !defined (VXD_COMPATLIB)
	KEvent(EVENT_TYPE type, BOOLEAN signaled = FALSE);
	VOID Initialize(EVENT_TYPE type, BOOLEAN signaled = FALSE);
#if (!_WDM_ || (_WDM_ && (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))))
	KEvent(KUstring& Name);
	VOID Initialize(KUstring& Name);
#endif // !_WDM_
#if (!_WDM_ || (_WDM_ && (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))))
	KEvent(KUstring& Name, EVENT_TYPE type);
	VOID Initialize(KUstring& Name, EVENT_TYPE type);
#endif	// !_WDM_
#endif // VXD_COMPATLIB

	~KEvent(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);

// Methods
public:
	VOID Clear(void);
#if !_WDM_
	BOOLEAN State(void);
#endif // WDM
	BOOLEAN Reset(void);
	BOOLEAN Set(KPRIORITY Increment=IO_NO_INCREMENT, BOOLEAN Wait=FALSE);
	VOID Pulse(void);
	operator PKEVENT();

// Data
protected:
	BOOLEAN m_CreatedByName;
	KEVENT m_event;
};

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//	KEvent( void )
//
//	Routine Description:
//
//		Constructor for class KEvent.
//
//	Parameters:
//
//		None
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This form is the default constructor.  One of the Initialize() methods
//		must be invoked before the object can be used.
//
inline KEvent::KEvent() :
	KDispatcherObject(),
	m_CreatedByName(FALSE)
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	KEvent(
//		PKEVENT pEvent )
//
//	Routine Description:
//
//		Constructor for class KEvent.
//
//	Parameters:
//
//		pEvent - Pointer to the system event object on which this instance
//			is to be based.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		Use this form when the underlying system event object already exists
//		and its address is known.
//
//		This form of the ctor may be used at any IRQL. All others
//		are safe only below DISPATCH_LEVEL
//
inline KEvent::KEvent(PKEVENT pEvent) :
	KDispatcherObject((PVOID)pEvent),
	m_CreatedByName(FALSE)
{
}

inline VOID KEvent::Initialize(PKEVENT pEvent)
{
	KDispatcherObject::Initialize((PVOID)pEvent);
	m_CreatedByName = FALSE;
}

#if !defined (VXD_COMPATLIB)

inline VOID KEvent::Clear(void)
{
	KeClearEvent( (PKEVENT) m_pObject);
}

#if !_WDM_
inline BOOLEAN KEvent::State(void)
{
	return (BOOLEAN)KeReadStateEvent( (PKEVENT)m_pObject);
}
#endif // !_WDM_

inline BOOLEAN KEvent::Reset(void)
{
	return (BOOLEAN)KeResetEvent( (PKEVENT)m_pObject);
}

inline BOOLEAN KEvent::Set(KPRIORITY Increment, BOOLEAN Wait)
{
	return (BOOLEAN)KeSetEvent( (PKEVENT)m_pObject, Increment, Wait);
}

inline KEvent::operator PKEVENT()
{
	return (PKEVENT)m_pObject;
}

#else // VXD_COMPATLIB

inline VOID KEvent::Clear(void)
{
	KeClearEvent( (PKEVENT) m_Handle);
}

inline BOOLEAN KEvent::Reset(void)
{
	return (BOOLEAN)KeResetEvent( (PKEVENT)m_Handle);
}

inline BOOLEAN KEvent::Set(KPRIORITY Increment, BOOLEAN Wait)
{
	return (BOOLEAN)KeSetEvent( (PKEVENT)m_Handle, Increment, Wait);
}

inline KEvent::operator PKEVENT()
{
	return (PKEVENT)m_Handle;
}

#endif // VXD_COMPATLIB


inline VOID KEvent::Pulse(void)
{
	Set();
	Clear();
}

#endif // __KEVENT__
