// kspin.h - include file for class KSpinLockR, KSpinLock
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

#ifndef __KSPIN__
#define __KSPIN__

class KSpinLock
{

// Construction
public:
	SAFE_DESTRUCTORS
	KSpinLock(void);
	KSpinLock(PKSPIN_LOCK ptr);
	VOID Initialize();
	VOID Initialize(PKSPIN_LOCK ptr);
	VOID Invalidate(void){}
	BOOLEAN IsValid(void){ return TRUE; }

// Methods
public:
	VOID Lock(BOOLEAN AtDpcLevel=FALSE);
	VOID Unlock(void);

#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))
	BOOLEAN TryToLockAtDpcLevel() { return KeTryToAcquireSpinLockAtDpcLevel(m_ptr); }
#endif

	operator PKSPIN_LOCK () { return m_ptr; }

	KIRQL _lock(BOOLEAN AtDpcLevel=FALSE);
	VOID  _unlock(KIRQL restoreIrql);

	KIRQL& OldIrql(void) {	return m_PreviousIrql; }

// Data
protected:
	PKSPIN_LOCK m_ptr;
	KSPIN_LOCK m_lock;
	KIRQL m_PreviousIrql;
};

//////////////////////////////////////////////////////////////////////////////

class CancelSpinLock
{

// Construction
private:
	CancelSpinLock();	// don't allow construction

// Methods
public:
	static KIRQL Acquire(void);
	static VOID Release(void);
	static VOID Release(KIRQL irql);

// Data
protected:
	static KIRQL m_PreviousState;
};

//////////////////////////////////////////////////////////////////////////////
#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))
class KInterruptSpinLock
{
// Construction
public:
	SAFE_DESTRUCTORS
	KInterruptSpinLock(void) : m_pInterrupt(NULL) {}
	KInterruptSpinLock(PKINTERRUPT pInterrupt) : m_pInterrupt(pInterrupt) {}
	VOID Initialize(PKINTERRUPT pInterrupt) { m_pInterrupt = pInterrupt; }

	VOID Invalidate(void) { m_pInterrupt = NULL; }
	BOOLEAN IsValid(void){ return (m_pInterrupt != NULL); }

// Methods
public:
	VOID Lock() { m_PreviousIrql = KeAcquireInterruptSpinLock(m_pInterrupt); }
	VOID Unlock() { KeReleaseInterruptSpinLock(m_pInterrupt, m_PreviousIrql); }

	KIRQL& OldIrql(void) {	return m_PreviousIrql; }

// Data
protected:
	PKINTERRUPT	m_pInterrupt;
	KIRQL		m_PreviousIrql;
};
#endif


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

inline KSpinLock::KSpinLock(void) :
	m_PreviousIrql(PASSIVE_LEVEL)
{
	m_ptr = &m_lock;
	KeInitializeSpinLock(&m_lock);
}

inline VOID KSpinLock::Initialize(void)
{
	m_ptr = &m_lock;
	KeInitializeSpinLock(&m_lock);
}

inline KSpinLock::KSpinLock(PKSPIN_LOCK ptr) :
	m_ptr(ptr),
	m_PreviousIrql(PASSIVE_LEVEL)
{
}

inline VOID KSpinLock::Initialize(PKSPIN_LOCK ptr)
{
	m_ptr = ptr;
}

inline KIRQL KSpinLock::_lock(BOOLEAN AtDpcLevel)
{
	if (AtDpcLevel)
	{
		KeAcquireSpinLockAtDpcLevel(m_ptr);
		return DISPATCH_LEVEL;
	}
	else
	{
		KIRQL oldIrql;
		KeAcquireSpinLock(m_ptr, &oldIrql);
		return oldIrql;
	}
}

inline VOID KSpinLock::_unlock(KIRQL restoreIrql)
{
	if (restoreIrql == DISPATCH_LEVEL)
		KeReleaseSpinLockFromDpcLevel(m_ptr);
	else
		KeReleaseSpinLock(m_ptr, restoreIrql);
}

inline VOID KSpinLock::Lock(BOOLEAN AtDpcLevel)
{
	m_PreviousIrql = _lock(AtDpcLevel);
}

inline VOID KSpinLock::Unlock(void)
{
	_unlock(m_PreviousIrql);
}

//////////////////////////////////////////////////////////////////////////////

inline KIRQL CancelSpinLock::Acquire(void)
{
	KIRQL irql;
	IoAcquireCancelSpinLock(&irql);
	return (m_PreviousState = irql);
}

inline VOID CancelSpinLock::Release(void)
{
	IoReleaseCancelSpinLock(m_PreviousState);
}

inline VOID CancelSpinLock::Release(KIRQL irql)
{
	IoReleaseCancelSpinLock(irql);
}

#endif
