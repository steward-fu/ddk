// kevent.cpp - implementation of class KEvent
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

VOID KEvent::Invalidate(void)
{
	if (m_CreatedByName && IsValid())
	{
		ZwClose(m_Handle);
		m_CreatedByName = FALSE;
	}
	KDispatcherObject::Invalidate();
}

/////////////////////////////////////////////////////////////////////////
#pragma code_seg("PAGE")
/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//	KEvent(
//		HANDLE hEvent,
//		ACCESS_MASK access )
//
//	Routine Description:
//
//		Constructor for class KEvent.
//
//	Parameters:
//
//		hEvent - Handle of the system event object on which this instance
//			is to be based.
//		access - Bit mask describing desired access rights. Default is
//			OBJECT_TYPE_ALL_ACCESS.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		Use this form when the underlying system object already exists, and
//		its handle is known.
//
KEvent::KEvent(HANDLE hEvent, ACCESS_MASK access) :
	KDispatcherObject(hEvent, access),
	m_CreatedByName(FALSE)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
}

VOID KEvent::Initialize(HANDLE hEvent, ACCESS_MASK access)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	KDispatcherObject::Initialize(hEvent, access);
	m_CreatedByName = FALSE;
}

#if !defined (VXD_COMPATLIB)

///////////////////////////////////////////////////////////////////////////////
//
//	KEvent(
//		EVENT_TYPE type,
//		BOOLEAN signaled )
//
//	Routine Description:
//
//		Constructor for class KEvent.
//
//	Parameters:
//
//		type - Specifies the event type. See comments.
//		signaled - Specifies the initial state of the event, TRUE for
//			signaled, FALSE for not signaled.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		Use this form to create a new system event object of the desired type.
//
KEvent::KEvent(EVENT_TYPE type, BOOLEAN signaled) :
	KDispatcherObject(&m_event),
	m_CreatedByName(FALSE)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	if (IsValid()) KeInitializeEvent((PKEVENT)m_pObject, type, signaled);
}

VOID KEvent::Initialize(EVENT_TYPE type, BOOLEAN signaled)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	KDispatcherObject::Initialize(&m_event);
	m_CreatedByName = FALSE;
	if (IsValid()) KeInitializeEvent((PKEVENT)m_pObject, type, signaled);
}

#if (!_WDM_ || (_WDM_ && (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))))

///////////////////////////////////////////////////////////////////////////////
//
//	KEvent(
//		KUstring& Name )
//
//	Routine Description:
//
//		Constructor for class KEvent.
//
//	Parameters:
//
//		Name - Name of event to be created.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This form allows two or more independent drivers to share access to a
//		common synchronization event. If an event by the supplied name does
//		not already exist, this call creates one. The class encapsulates the
//		system service; it is not required that the other driver utilize the
//		class library in order to share the event, rather it is sufficient to
//		supply matching names. The underlying system service is
//		IoCreateSynchronizationEvent. The constructor may only be called at
//		PASSIVE_LEVEL.
//
KEvent::KEvent(KUstring& Name) :
	KDispatcherObject(
		(PVOID)IoCreateSynchronizationEvent(&Name.UnicodeString(), &m_Handle),
		(BOOLEAN)FALSE
		),
	m_CreatedByName(m_Handle != 0)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
}

VOID KEvent::Initialize(KUstring& Name)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	KDispatcherObject::Initialize(
		(PVOID)IoCreateSynchronizationEvent(&Name.UnicodeString(), &m_Handle),
		(BOOLEAN)FALSE
		);
	m_CreatedByName = (m_Handle != 0);
}

#endif // (!_WDM_ || (_WDM_ && (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))))

#if (!_WDM_ || (_WDM_ && (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))))

///////////////////////////////////////////////////////////////////////////////
//
//	KEvent(
//		KUstring& Name,
//		EVENT_TYPE type )
//
//	Routine Description:
//
//		Constructor for class KEvent.
//
//	Parameters:
//
//		Name - Name of event to be created.
//		type - Either NotificationEvent or SynchronizationEvent
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This form allows two or more independent drivers to share access to a
//		common synchronization event. If an event by the supplied name does
//		not already exist, this call creates one. The class encapsulates the
//		system service; it is not required that the other driver utilize the
//		class library in order to share the event, rather it is sufficient to
//		supply matching names. The underlying system service is
//		IoCreateSynchronizationEvent. The constructor may only be called at
//		PASSIVE_LEVEL.
//
KEvent::KEvent(KUstring& Name, EVENT_TYPE type) :
	KDispatcherObject(
		(type == SynchronizationEvent) ?
			(PVOID)IoCreateSynchronizationEvent(&Name.UnicodeString(), &m_Handle) :
			(PVOID)IoCreateNotificationEvent(&Name.UnicodeString(), &m_Handle),
		(BOOLEAN)FALSE
		),
	m_CreatedByName(m_Handle != 0)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
}

VOID KEvent::Initialize(KUstring& Name, EVENT_TYPE type)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	KDispatcherObject::Initialize(
		(type == SynchronizationEvent) ?
			(PVOID)IoCreateSynchronizationEvent(&Name.UnicodeString(), &m_Handle) :
			(PVOID)IoCreateNotificationEvent(&Name.UnicodeString(), &m_Handle),
		(BOOLEAN)FALSE
		);
	m_CreatedByName = (m_Handle != 0);
}

#endif	// (!_WDM_ || (_WDM_ && (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))))

#endif // VXD_COMPATLIB
