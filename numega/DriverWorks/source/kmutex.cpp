// kmutex.cpp - implementation of class KMutex
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
//	KMutex(
//		HANDLE hMutex,
//		ACCESS_MASK access )
//
//	Routine Description:
//
//		Constructor for class KMutex.
//
//	Parameters:
//
//		hMutex - Handle of the system mutex object on which this instance
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
KMutex::KMutex(HANDLE hMutex, ACCESS_MASK access) :
	KDispatcherObject(hMutex, access)
{
}

VOID KMutex::Initialize(HANDLE hMutex, ACCESS_MASK access)
{
	KDispatcherObject::Initialize(hMutex, access);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KMutex(
//		ULONG level )
//
//	Routine Description:
//
//		Constructor for class KMutex.
//
//	Parameters:
//
//		level - Specifies the event type. See comments.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		Use this form to create a new system mutex object.
//
//		Parameter Level is relevant to drivers whose threads synchronize
//		using multiple mutexes. It enables the system to enforce acquisition
//		of mutexes in a controlled order. If a thread is holding a mutex of
//		level L, then it cannot attempt to acquire a mutex with a level less
//		than L. This prevents threads sharing a set of mutexes from deadlocking.
//
//		The underlying system service is KeInitializeMutex. The caller must
//		be running at PASSIVE_LEVEL.
//
KMutex::KMutex(ULONG level) :
	KDispatcherObject(&m_mutex)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	if (IsValid()) KeInitializeMutex((PKMUTEX)m_pObject, level);
}

VOID KMutex::Initialize(ULONG level)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
	KDispatcherObject::Initialize(&m_mutex);
	if (IsValid()) KeInitializeMutex((PKMUTEX)m_pObject, level);
}

#if !defined (VXD_COMPATLIB)
#if ! _WDM_

///////////////////////////////////////////////////////////////////////////////
//
//	KMutex(
//		KUstring& Name )
//
//	Routine Description:
//
//		Constructor for class KMutex.
//
//	Parameters:
//
//		Name - Name of mutex to be created.
//		access - Bit mask describing desired access rights. Default is
//			OBJECT_TYPE_ALL_ACCESS.
//
//	Return Value:
//
//		None
//
//	Comments:
//		This form allows a named mutex to be shared. The supplied name must
//		include a path in the object directory. The constructor attempts to
//		open the mutex based on that name. If it fails, the object is not created,
//		and IsValid() will return FALSE. For a driver that shares a
//		mutex with an application, the path must be	L"\\BaseNamedObjects".
//		For example,
//
//			KMutex MutexObj(L"\\BaseNamedObjects\\XyzSynch");
//
//		The above line constructs a mutex object that opens a named mutex
//		that an application has created, and can access with
//		the following Win32 code:
//
//			HANDLE hMutex = CreateMutex(NULL, FALSE, "XyzSynch");
//
//		The underlying system service is ObReferenceObjectByName. The
//		constructor may only be called at PASSIVE_LEVEL.
//
KMutex::KMutex(KUstring& Name, ACCESS_MASK access) :
	KDispatcherObject(Name, access)
{
}

VOID KMutex::Initialize(KUstring& Name, ACCESS_MASK access)
{
	KDispatcherObject::Initialize(Name, access);
}
#endif // ! _WDM_
#endif // !defined (VXD_COMPATLIB)
