// thrddev.h - include file for device class of thread example
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

extern BOOLEAN bUnloading; 

////////////////////////////////////////////////////////////////////
// TestThread
//
// TestThread is a subclass of KSystemThread. It adds a queue and
// a semaphore, and a method for queuing an IRP and signaling the
// thread.
//
class TestThread : public KSystemThread
{
	SAFE_DESTRUCTORS
public:
	TestThread(void);
	VOID Post(KIrp I);

	KInterlockedList<IRP>	m_Queue;
	KSemaphore				m_Mailbox;
};

////////////////////////////////////////////////////////////////////
// ThreadDevice
// 
// Sole device class for this driver.
//
class ThreadDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	ThreadDevice(void);
	~ThreadDevice(void);
	DEVMEMBER_DISPATCHERS

	MEMBER_THREAD	(ThreadDevice, Stage1)
	MEMBER_THREAD	(ThreadDevice, Stage2)

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	VOID Stage1(void); 								// COMMENT_ONLY
	VOID Stage2(void); 								// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
#endif

	TestThread				m_Stage1;
	TestThread				m_Stage2;
	KInterlockedCounter		m_OpenCounter;
};

#define MAX_IRP 1000000

