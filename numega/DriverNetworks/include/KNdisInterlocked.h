// KNdisInterlocked.h: interface for the KNdisInterlocked class.
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

#if !defined(AFX_KNDISINTERLOCKED_H__95213765_51A3_11D3_8FA9_00C04F7445B7__INCLUDED_)
#define AFX_KNDISINTERLOCKED_H__95213765_51A3_11D3_8FA9_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <kndis.h>

#if	!BINARY_COMPATIBLE && !defined(VTOOLSD)
#include <ntddk.h>
#endif

//////////////////////////////////////////////////////////////////////
// Interlocked Operations
//
// NDIS requires no spinlock for increment/decrement operations with
// ULONGs - assumes an underlying CPU support for that. Generic add
// and subtruct do require a spin lock.
//
// For BINARY_COMPATIBLE builds the class is trivialized to ULONG
// (review it!!)
//
//////////////////////////////////////////////////////////////////////
// KNdisCounter		simple incrementing counter abstract type
//
class KNdisCounter {
public:
// Basic manipulations
#if !BINARY_COMPATIBLE && !defined(VTOOLSD) && (KNDIS_DDK_VERSION > 4)
	inline KNdisCounter(ULONG value=0) { NdisStoreUlong(&m_value, value);}
	inline ULONG operator++ () { return NdisInterlockedIncrement((PLONG)&m_value); }
	inline ULONG operator-- () { return NdisInterlockedDecrement((PLONG)&m_value); }
	inline operator ULONG() { ULONG u; NdisRetrieveUlong(&u, &m_value); return u; }
#if _MSC_VER < 1300 //Get around a compiler bug in VC 7.0
	inline ULONG operator=(ULONG n) { InterlockedExchange((PLONG)&m_value, (LONG)n); return n; }
#else
	inline ULONG operator=(ULONG n) { return m_value=n; }
#endif
#else
	inline KNdisCounter(ULONG value=0) {m_value=value;}
	inline ULONG operator++ () { return ++m_value; }
	inline ULONG operator-- () { return --m_value; }
	inline operator ULONG() { return m_value; }
	inline ULONG operator=(ULONG n) { return m_value=n; }
#endif
	inline const ULONG operator++(int) {ULONG n = ++(*this); return n - 1; }
	inline const ULONG operator--(int) {ULONG n = --(*this); return n + 1; }

// comparisons
	inline bool operator==(ULONG n) { return n==m_value; }
	inline bool operator!=(ULONG n) { return n!=m_value; }
	inline bool operator>(ULONG n)  { return n<m_value; }
	inline bool operator>=(ULONG n) { return n<=m_value; }
	inline bool operator<=(ULONG n) { return n>=m_value; }

	// Atomic Test&Set: sets NewValue if OldValue==m_value. Returns original value
	inline ULONG TestAndSet(ULONG NewValue=1, ULONG OldValue=0) {
#if	!BINARY_COMPATIBLE && !defined(VTOOLSD) && !defined(TDI_WINDOWS_95) && (KNDIS_DDK_VERSION >= 5)
		return InterlockedCompareExchange ((LONG*)&m_value, (LONG)NewValue, (LONG)OldValue);
#else
		ULONG cur;
		if ((cur=m_value) == OldValue) m_value = NewValue; return cur;
#endif
	}

protected:
	ULONG		m_value;
};

#if	(KNDIS_DDK_VERSION >= 5) && !BINARY_COMPATIBLE
//////////////////////////////////////////////////////////////////////
// KNdisGenCounter	A KNdisCounter that can be arbitraly updated
//
class KNdisGenCounter : public KNdisCounter {
public:
// Constructor
	inline KNdisGenCounter(ULONG value=0) : KNdisCounter(value)
		{ NdisAllocateSpinLock(&m_Lock);}

// Atomic Increment
	inline void operator++ (int n) { NdisInterlockedAddUlong(&m_value, (ULONG)n, &m_Lock); }

// conversion
	inline operator ULONG() { ULONG u; NdisRetrieveUlong(&u, &m_value); return u; }

// assignment
	inline ULONG operator=(ULONG n) { NdisStoreUlong(&m_value, n); return n; }

protected:
	NDIS_SPIN_LOCK  m_Lock;
};
#endif


//////////////////////////////////////////////////////////////////////
// KNdisRefCounter
//
// Dispatchable reference counter. Combines interlocked counter with event.
// When 0, it is in signaled state, otherwise not.
// Use methods: Increment(), Decrement() and Wait()
//
class KNdisRefCounter : public KNdisCounter, public KNdisEvent
{
public:
	inline KNdisRefCounter(ULONG Start=0) :
	  KNdisCounter(Start), KNdisEvent() { if (m_value==0) Set(); }

	  inline ULONG Increment() { ULONG u=++*this; if (u==1) Reset(); return u; }
	  inline ULONG Decrement() { ULONG u=--*this; if (u==0) Set(); return u; }
};

#endif // !defined(AFX_KNDISINTERLOCKED_H__95213765_51A3_11D3_8FA9_00C04F7445B7__INCLUDED_)
