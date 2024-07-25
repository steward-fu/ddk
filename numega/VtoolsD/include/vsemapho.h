// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vsemapho.h - include file for semaphore class
//

#ifndef __vsemaphore_h_
#define __vsemaphore_h_

/*
	VSemaphore class
	----------------
	The VSemaphore class is a simple encapsulation of the VMM
	semaphore calls.  Semaphores are generally used to manage
	resources shared by multiple VMs or threads.

	When you create a VSemaphore object, you specify a token count.
	The token count corresponds to the number of clients which
	can simultaneously acquire the resource in question.

	Clients claim acquisition of the resource with the wait() member
	function.  If the number of clients that currently hold the
	resource is less than the initial token count, then wait() returns
	immediately and the caller holds the resource.  Otherwise, the
	client is blocked by the VMM until another client releases the
	resource.

	Clients release the semaphore with the signal() member function
	or the signal_noSwitch() member function (not available in version
	3.1).  The signal() member function causes immediate rescheduling
	of VMs and threads, whereas signal_noSwitch() does not.

	Depending on your needs, you can create a VSemaphore object with
	a token count of zero, and signal it as the corresponding resources
	become available.  Calling signal effectively increments the token
	count.

	Allocation
	----------
	In most cases it is necessary to allocate VSemaphore objects on
	the heap using the new operator.  However, in cases where the
	semphore is being used only within a single function, it is
	permissible to allocate it on the stack.

	The destructor for VSemaphore, invoked either by the delete
	operator (for objects created with new) or by a closing brace
	(for objects allocated on the stack), causes the semaphore
	to be destroyed. Any threads or VMs that were blocked on the
	destroyed semaphore become schedulable.

	Example
	-------
	Here is a sketch of a set of routines that manage an array of
	packets shared by multiple VMs or threads:

	VSemaphore* pSemHeap;
	PACKET packets[PACKET_HEAP_SIZE];

	void initPacketHeap()
	{
		// create semaphore with token count corresponding to 
		// resource size
		pSemHeap = new VSemaphore(PACKET_HEAP_SIZE);
		...
	}

	PACKET* allocPacket()
	{
		pSemHeap->wait();	// block if no packet available
		...
		return pnext;
	}

	void freePacket(PACKET* p)
	{
		pnext = p;
		...
		pSemHeap->signal();	// signal that packet is available
	}

	void closePacketHeap()
	{
		delete pSemHeap;	// release the semaphore
		...
	}

*/

class VSemaphore
{
public:
	VSemaphore(int tokenCount);
	~VSemaphore();

	VOID wait(DWORD flags=BLOCK_SVC_INTS);
	VOID signal();
#ifdef WIN40
	VOID signal_noSwitch();	
#endif
	DWORD getHandle() {return m_handle;};

protected:
	SEMHANDLE m_handle;
};
#endif

