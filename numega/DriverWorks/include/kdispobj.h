// kdispobj.h - include file for class KDispatcherObject
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

#ifndef __KDISPOBJ__
#define __KDISPOBJ__

class KDispatcherObject
{

// Construction
protected:
	SAFE_DESTRUCTORS
	KDispatcherObject();
	KDispatcherObject(PVOID pObject, BOOLEAN bZeroHandle=TRUE);
	KDispatcherObject(HANDLE h, ACCESS_MASK access);
	KDispatcherObject(unsigned int size);
	VOID Initialize(PVOID pObject, BOOLEAN bZeroHandle=TRUE);
	VOID Initialize(HANDLE h, ACCESS_MASK access);
	VOID Initialize(unsigned int size);
#if !defined (VXD_COMPATLIB)
#if ! _WDM_
	KDispatcherObject(KUstring& Name, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
	VOID Initialize(KUstring& Name, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
#endif // ! _WDM_
#endif // VXD_COMPATLIB
	~KDispatcherObject(void){ if (IsValid()) Invalidate(); }
public:
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_pObject != NULL); }

// Methods
public:
	NTSTATUS Wait(
		KPROCESSOR_MODE mode=KernelMode,
		BOOLEAN bAlertable=TRUE,
		PLARGE_INTEGER pTimeOut=NULL,
		KWAIT_REASON reason=Executive
		);

#ifndef VXD_COMPATLIB
	static NTSTATUS WaitMultiple(
		ULONG  Count,
		PVOID  Object[],
		WAIT_TYPE  WaitType,
		KWAIT_REASON  WaitReason,
		KPROCESSOR_MODE  WaitMode,
		BOOLEAN  Alertable,
		PLARGE_INTEGER  Timeout,
		PKWAIT_BLOCK  WaitBlockArray=NULL
		);
#endif //VXD_COMPATLIB

	PVOID Object(void);
	HANDLE Handle(void);

// Data
protected:
	HANDLE m_Handle;
	PVOID m_pObject;
	BOOLEAN m_CreatedReference;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline KDispatcherObject::KDispatcherObject() :
	m_pObject(NULL),
	m_Handle((HANDLE)0),
	m_CreatedReference(FALSE)
{
}

inline KDispatcherObject::KDispatcherObject(PVOID pObject, BOOLEAN bZeroHandle) :
	m_pObject(pObject),
	m_CreatedReference(FALSE)
{
	if (bZeroHandle) m_Handle = (HANDLE)0;
}

inline KDispatcherObject::KDispatcherObject(HANDLE h, ACCESS_MASK access) :
	m_pObject(NULL),
	m_Handle(h)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	ObReferenceObjectByHandle(
		m_Handle,
		access,
		NULL,
		KernelMode,
		&m_pObject,
		NULL
		);
	m_CreatedReference = (m_pObject != NULL);
}

inline KDispatcherObject::KDispatcherObject(unsigned int size) :
	m_pObject(NULL),
	m_Handle((HANDLE)-1),
	m_CreatedReference(FALSE)
{
	m_pObject = (PVOID) new(NonPagedPool) CHAR[size];
}

#if !defined (VXD_COMPATLIB)
#if ! _WDM_
inline KDispatcherObject::KDispatcherObject(KUstring& Name, ACCESS_MASK access) :
	m_pObject(NULL),
	m_Handle((HANDLE)0)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	ObReferenceObjectByName(
		Name,
		0,
		NULL,
		access,
		NULL,
		KernelMode,
		NULL,
		&m_pObject
		);
	m_CreatedReference = (m_pObject != NULL);
}
#endif // ! _WDM_
#endif // VXD_COMPATLIB

inline VOID KDispatcherObject::Initialize(PVOID pObject, BOOLEAN bZeroHandle)
{
	if (IsValid())
	{
		ASSERT(m_pObject == NULL);
		Invalidate();
	}
	m_pObject = pObject;
	m_CreatedReference = FALSE;
	if (bZeroHandle) m_Handle = (HANDLE)0;
}

inline VOID KDispatcherObject::Initialize(HANDLE h, ACCESS_MASK access)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	if (IsValid())
	{
		ASSERT(m_pObject == NULL);
		Invalidate();
	}
	m_Handle = h;
	ObReferenceObjectByHandle(
		m_Handle,
		access,
		NULL,
		KernelMode,
		&m_pObject,
		NULL
		);
	m_CreatedReference = (m_pObject != NULL);
}

inline VOID KDispatcherObject::Initialize(unsigned int size)
{
	if (IsValid())
	{
		ASSERT(m_pObject == NULL);
		Invalidate();
	}
	m_Handle = (HANDLE)-1;
	m_pObject = (PVOID) new(NonPagedPool) CHAR[size];
	m_CreatedReference = FALSE;
}

#if !defined (VXD_COMPATLIB)
#if ! _WDM_
inline VOID KDispatcherObject::Initialize(KUstring& Name, ACCESS_MASK access)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	if (IsValid())
	{
		ASSERT(m_pObject == NULL);
		Invalidate();
	}
	m_Handle = (HANDLE)0;
	ObReferenceObjectByName(
		Name,
		0,
		NULL,
		access,
		NULL,
		KernelMode,
		NULL,
		&m_pObject
		);
	m_CreatedReference = (m_pObject != NULL);
}
#endif // ! _WDM_
#endif // VXD_COMPATLIB

inline VOID KDispatcherObject::Invalidate(void)
{
	if (m_pObject != NULL)
	{
		if (m_Handle == (HANDLE)-1)
		{
			delete m_pObject;
			m_Handle = (HANDLE)0;
		}

		if (m_CreatedReference)
		{
			ObDereferenceObject(m_pObject);
			m_CreatedReference = FALSE;
		}
	}
	m_pObject = NULL;
}

inline NTSTATUS KDispatcherObject::Wait(
	KPROCESSOR_MODE mode,
	BOOLEAN bAlertable,
	PLARGE_INTEGER pTimeOut,
	KWAIT_REASON reason
	)
{
	return KeWaitForSingleObject(m_pObject,reason,mode,bAlertable,pTimeOut);
}

#ifndef VXD_COMPATLIB
inline NTSTATUS KDispatcherObject::WaitMultiple(
	ULONG  Count,
	PVOID  Object[],
	WAIT_TYPE  WaitType,
	KWAIT_REASON  WaitReason,
	KPROCESSOR_MODE  WaitMode,
	BOOLEAN  Alertable,
	PLARGE_INTEGER  Timeout,
	PKWAIT_BLOCK  WaitBlockArray
	)
{
	return KeWaitForMultipleObjects(
		Count,
		Object,
		WaitType,
		WaitReason,
		WaitMode,
		Alertable,
		Timeout,
		WaitBlockArray
		);
}
#endif //VXD_COMPATLIB

inline PVOID KDispatcherObject::Object(void)
{
	return m_pObject;
}

inline HANDLE KDispatcherObject::Handle(void)
{
	return m_Handle;
}

#endif
