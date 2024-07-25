// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vmutex.h - include file for mutex class
//

#ifndef __vmutex_h_
#define __vmutex_h_

/*
	VMutex class
	------------
	The VMutex class encapsulates the VMM functionality for mutex
	objects.  You use a VMutex object to guard a region of code
	against simultaneous execution by more than one thread.

	Upon creation, a mutex is unowned.  Calling the enter() member
	function causes the calling thread to become the owner.
	Invoking the enter() function of a mutex owned by another thread
	causes the calling thread to block.  If the owner of a mutex
	calls enter(), the mutex's entry count is incremented.  The
	owner of a mutex may call its leave() member function to decrement
	the entry count.  When the entry count becomes zero, ownership
	is released, and a blocked thread may gain ownership.

	The constructor for VMutex takes two optional parameters.  The
	first specifies the scheduling boost to be applied by the
	scheduler to a thread that gains ownership of the mutex by
	entering it.  The second parameter, the flags, may be set to
	MUTEX_MUST_COMPLETE to indicate that than the owning thread may 
	not be suspended or terminated until the mutex is released.

	The constructor may fail due to memory limitations. In this case,
	the getHandle() member function returns zero.

	Example
	-------

	VMutex* pmtxGuard;
	BOOL bMutexOK;

	init()
	{
		pmtxGuard = new VMutex();

		bMutexOK = (pmxtGuard!=NULL) && (pmtxGuard->getHandle()!=0);
	}

	function( )
	{
		pmtxGuard->enter();

		// code guaurded against simultaneous execution by multiple
		// threads is placed here

		pmtxGuard->leave();
	}

	terminate()
	{
		delete pmtxGuard;
	}
*/

class VMutex
{
public:
#ifdef WIN40
	VMutex(DWORD boost=0, DWORD flags=0);
	~VMutex();
	VOID enter(DWORD flags=BLOCK_SVC_INTS);
	VOID leave();
	THREADHANDLE owner();
	DWORD getHandle() {return m_handle;};
#endif

	// Global critical section calls
	static VOID enterGlobal(DWORD flags);
	static VOID leaveGlobal();

#ifdef WIN40
protected:
	MUTEXHANDLE m_handle;
#endif
};
#endif

