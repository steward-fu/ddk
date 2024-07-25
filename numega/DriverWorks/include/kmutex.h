// kmutex.h - include file for class KMutex
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

#ifndef __KMUTEX__
#define __KMUTEX__

class KMutex : public KDispatcherObject
{

// Construction
public:
	SAFE_DESTRUCTORS
	KMutex(void);
	KMutex(PKMUTEX pMutex);
	KMutex(HANDLE hMutex, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
	KMutex(ULONG Level);
	VOID Initialize(PKMUTEX pMutex);
	VOID Initialize(HANDLE hMutex, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
	VOID Initialize(ULONG Level);

#if !defined (VXD_COMPATLIB)
#if ! _WDM_
	KMutex(KUstring& Name, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
	VOID Initialize(KUstring& Name, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
#endif // ! _WDM_
#endif // VXD_COMPATLIB

// Methods
public:
	BOOLEAN State(void);
	LONG Release(BOOLEAN bWaitAgain=FALSE);

// Data
protected:
	KMUTEX m_mutex;
};

////////////////////////////////////////////////////////////////////
// KMutexFast
//
//
class KMutexFast {
public:
	SAFE_DESTRUCTORS
	inline KMutexFast() { ExInitializeFastMutex(&m_Mutex); }

	inline void Acquire() { ExAcquireFastMutex(&m_Mutex); }
	inline void AcquireUnsafe() { ExAcquireFastMutexUnsafe(&m_Mutex); }

	inline void Release() { ExReleaseFastMutex(&m_Mutex); }
	inline void ReleaseUnsafe() { ExReleaseFastMutexUnsafe(&m_Mutex); }

#if !_WDM_
	inline BOOLEAN TryAcquire() { return ExTryToAcquireFastMutex(&m_Mutex); }
#endif

protected:
	FAST_MUTEX  m_Mutex;
};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//	KMutex( void )
//
//	Routine Description:
//
//		Constructor for class KMutex.
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
inline KMutex::KMutex(void) :
	KDispatcherObject()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	KMutex(
//		PKMUTEX pMutex )
//
//	Routine Description:
//
//		Constructor for class KMutex.
//
//	Parameters:
//
//		pMutex - Pointer to the system mutex object on which this instance
//			is to be based.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		Use this form when the underlying system mutex object already exists
//		and its address is known.
//
inline KMutex::KMutex(PKMUTEX pMutex) :
	KDispatcherObject(pMutex)
{
}

inline VOID KMutex::Initialize(PKMUTEX pMutex)
{
	KDispatcherObject::Initialize(pMutex);
}

inline BOOLEAN KMutex::State(void)
{
	return ( ((PKMUTEX)m_pObject)->Header.SignalState == 1 );
}

inline LONG KMutex::Release(BOOLEAN bWaitAgain)
{
	return KeReleaseMutex((PKMUTEX)m_pObject, bWaitAgain);
}

#endif
