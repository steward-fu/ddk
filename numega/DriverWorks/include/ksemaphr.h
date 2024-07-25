// ksemaphr.h - include file for class KSemaphore
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

#ifndef __KSEMAPHR__
#define __KSEMAPHR__

class KSemaphore : public KDispatcherObject
{

// Construction
public:
	SAFE_DESTRUCTORS
	KSemaphore(void);
	KSemaphore(PRKSEMAPHORE pSem);
	KSemaphore(HANDLE hSem, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
	KSemaphore(LONG count, LONG limit);
	VOID Initialize(PRKSEMAPHORE pSem);
	VOID Initialize(HANDLE hSem, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
	VOID Initialize(LONG count, LONG limit);

#if !defined (VXD_COMPATLIB)
#if ! _WDM_
	KSemaphore(KUstring& Name, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
	VOID Initialize(KUstring& Name, ACCESS_MASK access = OBJECT_TYPE_ALL_ACCESS);
#endif // ! _WDM_
#endif // VXD_COMPATLIB

// Methods
public:
	BOOLEAN Signal(
		LONG Adjustment=1,
		KPRIORITY Boost=IO_NO_INCREMENT,
		BOOLEAN bWaitAgain=FALSE);
	BOOLEAN State(void);

// Data
protected:
	KSEMAPHORE m_semaphore;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//	KSemaphore( void )
//
//	Routine Description:
//
//		Constructor for class KSemaphore.
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
inline KSemaphore::KSemaphore(void) :
	KDispatcherObject()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	KSemaphore(
//		PRKSEMAPHORE pSem )
//
//	Routine Description:
//
//		Constructor for class KSemaphore.
//
//	Parameters:
//
//		pSem - Pointer to the system semaphore object on which this instance
//			is to be based.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		Use this form when the underlying system semaphore object already exists
//		and its address is known.
//
inline KSemaphore::KSemaphore(PRKSEMAPHORE pSem) :
	KDispatcherObject(pSem)
{
}

inline VOID KSemaphore::Initialize(PRKSEMAPHORE pSem)
{
	KDispatcherObject::Initialize(pSem);
}

inline BOOLEAN KSemaphore::Signal(
	LONG Adjustment,
	KPRIORITY Boost,
	BOOLEAN bWaitAgain)
{
	return (BOOLEAN)KeReleaseSemaphore(
			(PKSEMAPHORE)m_pObject,
			Boost,
			Adjustment,
			bWaitAgain);
}

inline BOOLEAN KSemaphore::State(void)
{
	return (BOOLEAN)KeReadStateSemaphore( (PKSEMAPHORE)m_pObject );
}

#endif
