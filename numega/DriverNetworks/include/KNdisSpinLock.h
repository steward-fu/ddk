// KNdisSpinLock.h: interface for the KNdisSpinLock class.
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
//=============================================================================

#if !defined(AFX_KNDISSPINLOCK_H__4490AB65_3A07_11D3_8FA2_00C04F7445B7__INCLUDED_)
#define AFX_KNDISSPINLOCK_H__4490AB65_3A07_11D3_8FA2_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////
// KNdisSpinLock
//
// Used to synchronize access to resources shared among nonISR driver
// functions.
//
class KNdisSpinLock
{
public:
	inline KNdisSpinLock(void);
	inline ~KNdisSpinLock(void);

// locking at arbitrary level
	inline VOID Lock(void);
	inline VOID Unlock(void);

// locking at DPC level
	inline VOID LockDpr(void);
	inline VOID UnlockDpr(void);

	inline operator PNDIS_SPIN_LOCK ();

protected:
	NDIS_SPIN_LOCK	m_lock;
};

/////////////////////////////////////////////////////////////////////
// KNdisRWLock
//
// Used to optimize synchronized access to resources shared among
// nonISR driver functions (W2K only)
//
// TODO (NdisAcquireReadWriteLock)
//


////////////////////////////////////////////////////////////////////
// Implementations


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
KNdisSpinLock::KNdisSpinLock(void)
{
	NdisAllocateSpinLock(&m_lock);
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
KNdisSpinLock::~KNdisSpinLock(void)
{
	// NOTE: This is NOP but gotta have it for purity :(
	NdisFreeSpinLock(&m_lock);
}


///////////////////////////////////////////////////////////////////////
// Acquire the lock
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	IRQL is raised to DISPATCH_LEVEL while the lock is held.
//	Release the lock with Unlock.
VOID KNdisSpinLock::Lock(void)
{
	NdisAcquireSpinLock(&m_lock);
}


///////////////////////////////////////////////////////////////////////
// Release the lock
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisSpinLock::Unlock(void)
{
	NdisReleaseSpinLock(&m_lock);
}


///////////////////////////////////////////////////////////////////////
// Acquire the lock at IRQL==DISPATCH_LEVEL
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	This is an optimized version of Lock.
VOID KNdisSpinLock::LockDpr(void)
{
	NdisDprAcquireSpinLock(&m_lock);
}


///////////////////////////////////////////////////////////////////////
// Release lock acquired with LockDpr
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisSpinLock::UnlockDpr(void)
{
	NdisDprReleaseSpinLock(&m_lock);
}


///////////////////////////////////////////////////////////////////////
// operator cast to PNDIS_SPIN_LOCK
//
// Parameters:
// 	none
// Returns:
// 	Returns reference to the underlying NDIS_SPIN_LOCK object.
// Comments:
// 	none
KNdisSpinLock::operator PNDIS_SPIN_LOCK ()
{
	return &m_lock;
}

#endif // !defined(AFX_KNDISSPINLOCK_H__4490AB65_3A07_11D3_8FA2_00C04F7445B7__INCLUDED_)
