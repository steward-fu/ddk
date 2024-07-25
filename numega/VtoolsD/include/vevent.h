// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vevent.h - include file for event classes
//

#ifndef __vevent_h_
#define __vevent_h_

#include <vmachine.h>

/* Event classes

  There are certain junctures when the system is not in the appropriate
  state for your VxD to perform some operation.  For example, you may
  be restricted from making VMM calls, you may not be in the correct
  virtual machine context, or you may want to defer to a virtual
  machine that is performing a time critical operation.  At these times, 
  Events provide a means for you to arrange with the VMM to be called back
  when the system is in the desired state.  The term Event refers to an
  asynchronous invocation of a handler function by the VMM under
  conditions requested by some VxD.  In general, you use Events to
  defer some action to a later point in time when the system is in the
  appropriate state for that action to take place.

  One common case where use of Events is required involves hardware
  interrupt processing.  If the VMM is interrupted by a hardware interrupt,
  the interrupt handler cannot make non-async VMM calls.  What it can do is
  arrange for the system to call back a second handler at a point in time
  when you are not restricted from making non-async calls.

  Another example of Event usage involves actions that must take place in
  the context of a specific virtual machine.  Using Events, you can request
  to be called back the next time the virtual machine is active.

  The VMM also allows you to schedule events when the global critical
  section is unowned.

  VGlobalEvent		event to run after interrupt processing complete
  VVMEvent		event to run in a specific virtual machine
  VPriorityVMEvent	event to run in a specific virtual machine with
			special options
  VEvent		base class for other event classes
*/

// Some of these constants are also defined in vevtutil.asm
#define EVENTTHUNKSIZE 		0x34
#define NEVENTHEAPPAGES		0x01
#define EVENTHEAPSIZE		(0x1000*NEVENTHEAPPAGES)
#define EVENTOBJECTMAXSIZE	0x60

class VEvent
{
public:
	void* _cdecl operator new(size_t n);
	void  _cdecl operator delete(void* p);
	static BOOL initEvents();
protected:
	BYTE  m_thunk[EVENTTHUNKSIZE];
	EVENTHANDLE m_handle;
	PVOID m_refData;
private:
	static BYTE* m_heap;
	static MEMHANDLE m_heapHandle;
#ifdef __BORLANDC__
// This virtual function is inserted in VEvent so that the Vtab pointer
// of in an instance of a derived class will be located at offset zero 
// in the object's store. Without this, the compiler (quite reasonably) 
// locates the Vtab pointer for derived classes above the store for VEvent, 
	virtual void ZeroVtab(void);
#endif
};

class VVMEvent : public VEvent
{
public:
	VVMEvent(VMHANDLE hVM, PVOID refData=0);
	VOID schedule();
	BOOL call();
	VOID cancel();
	virtual VOID handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
protected:
	VMHANDLE m_VM;
};

class VGlobalEvent : public VEvent
{
public:
	VGlobalEvent(PVOID refData=0);
	VOID schedule();
	BOOL call();
	VOID cancel();
	virtual VOID handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
};


class VPriorityVMEvent : public VEvent
{
public:
	VPriorityVMEvent(VMHANDLE hVM, DWORD boost, PVOID refData,
		DWORD timeout, DWORD flags);
	BOOL call();
	VOID cancel();
	virtual VOID handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData, BOOL timeout);
protected:
	VMHANDLE m_VM;
	DWORD m_boost;
	DWORD m_timeout;
	DWORD m_flags;
};

#ifdef WIN40
class VThreadEvent : public VEvent
{
public:
	VThreadEvent(THREADHANDLE hVM, PVOID refData=0);
	VOID schedule();
	VOID cancel();
	virtual VOID handler(THREADHANDLE hThread, VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
protected:
	THREADHANDLE m_Thread;
};
#endif

#endif

