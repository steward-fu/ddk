// ksemaphr.cpp - implementation of class KMutex
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

/////////////////////////////////////////////////////////////////////////
#pragma code_seg("PAGE")
/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//	KSemaphore(
//		HANDLE hSem,
//		ACCESS_MASK access )
//
//	Routine Description:
//
//		Constructor for class KSemaphore.
//
//	Parameters:
//
//		hSem - Handle of the system semaphore object on which this instance
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
KSemaphore::KSemaphore(HANDLE hSem, ACCESS_MASK access) :
	KDispatcherObject(hSem, access)
{
}

VOID KSemaphore::Initialize(HANDLE hSem, ACCESS_MASK access)
{
	KDispatcherObject::Initialize(hSem, access);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KSemaphore(
//		LONG count,
//		LONG limit )
//
//	Routine Description:
//
//		Constructor for class KSemaphore.
//
//	Parameters:
//
//		count - The initial count of the semaphore.
//		limit - The maximum count the semaphore can attain.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This form allocates storage for and initializes a new system semaphore object.
//
//		Parameter count must be greater than or equal to zero. If greater than zero,
//		the semaphore is signaled.
//
//		The underlying system service is KeInitializeSemaphore. The caller must
//		be running at PASSIVE_LEVEL.
//
KSemaphore::KSemaphore(LONG count, LONG limit) :
	KDispatcherObject(&m_semaphore)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	if (IsValid()) KeInitializeSemaphore((PKSEMAPHORE)m_pObject, count, limit);
}

VOID KSemaphore::Initialize(LONG count, LONG limit)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	KDispatcherObject::Initialize(&m_semaphore);
	if (IsValid()) KeInitializeSemaphore((PKSEMAPHORE)m_pObject, count, limit);
}

#if !defined (VXD_COMPATLIB)
#if ! _WDM_
///////////////////////////////////////////////////////////////////////////////
//
//	KSemaphore(
//		KUstring& Name )
//
//	Routine Description:
//
//		Constructor for class KSemaphore.
//
//	Parameters:
//
//		Name - Name of semaphore to be created.
//		access - Bit mask describing desired access rights. Default is
//			OBJECT_TYPE_ALL_ACCESS.
//
//	Return Value:
//
//		None
//
//	Comments:
//		This form allows a named semaphore to be shared. The supplied name must
//		include a path in the object directory. The constructor attempts to
//		open the semaphore based on that name. If it fails, the object is not created,
//		and IsValid() will return FALSE. For a driver that shares a
//		semaphore with an application, the path must be	L"\\BaseNamedObjects".
//		For example,
//
//			KSemaphore SemObj(L"\\BaseNamedObjects\\XyzSem");
//
//		The above line constructs a semaphore object that opens a named semaphore
//		that an application has created, and can access with
//		the following Win32 code:
//
//			HANDLE hSemaphore = CreateSemaphore(NULL, FALSE, "XyzSem");
//
//		The underlying system service is ObReferenceObjectByName. The
//		constructor may only be called at PASSIVE_LEVEL.
//
KSemaphore::KSemaphore(KUstring& Name, ACCESS_MASK access) :
	KDispatcherObject(Name, access)
{
}

VOID KSemaphore::Initialize(KUstring& Name, ACCESS_MASK access)
{
	KDispatcherObject::Initialize(Name, access);
}
#endif // ! _WDM_
#endif // !defined (VXD_COMPATLIB)
