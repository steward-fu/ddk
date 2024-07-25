// kgenlock.h - generic locking classes
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

#if !defined( __KGENLOCK__ )
#define  __KGENLOCK__

///////////////////////////////////////////////////////////////
// Use this as the locking class for classes that do not require
// any locking.
//
class KGenericLockNop
{
public:
	void Lock(void) 	{}
	void Unlock(void)	{}
	KGenericLockNop& operator=(const KGenericLockNop&) {return *this;} // work-around for compiler bug
};

///////////////////////////////////////////////////////////////
// Use this as the locking class for Spin Lock control of an object.
// This implies that the object must be in non-paged memory. Calling
// member function Lock raises IRQL to DISPATCH_LEVEL.
//
class KGenericLockSpinLock
{
protected:
	KSpinLock	m_Lock;

public:
	void Lock(void) 	{m_Lock.Lock();}
	void Unlock(void)	{m_Lock.Unlock();}
};

////////////////////////////////////////////////////////////////////
// The remaining classes use Dispatcher objects for synchronization.
// Therefore, the Lock method is callable only at IRQL=PASSIVE_LEVEL.
//
//

///////////////////////////////////////////////////////////////
// Event lock
//
class KGenericLockEvent
{
protected:
	KEvent		m_Event;

public:
	KGenericLockEvent(void) : m_Event(SynchronizationEvent, TRUE) {}

	void Lock(void) 	{m_Event.Wait();}
	void Unlock(void)	{m_Event.Set();}
};


///////////////////////////////////////////////////////////////
// Mutex lock
//
//
// A given thread can nest locking operations using this type of lock.
//
class KGenericLockMutex
{
protected:
	KMutex		m_Mutex;

public:
	KGenericLockMutex(void) : m_Mutex(ULONG(-1)) {}

	void Lock(void) 	{m_Mutex.Wait();}
	void Unlock(void)	{m_Mutex.Release();}
};



///////////////////////////////////////////////////////////////
// Semaphore lock
//
// By calling the Unlock method multiple times, a driver can allow
// a fixed number of simultaneous lockers.
//
class KGenericLockSemaphore
{
protected:
	KSemaphore	m_Semaphore;

protected:
	KGenericLockSemaphore(void) : m_Semaphore(1, 0x7fffffff) {}

	void Lock(void)		{ m_Semaphore.Wait(); }
	void Unlock(void)	{ m_Semaphore.Signal(); }
};

#endif // __KGENLOCK__
