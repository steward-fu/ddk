// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vintrs.h - include file for interrupt classes
//

#ifndef __vintrs_h_
#define __vintrs_h_

#include <vmachine.h>
#include <vcallbak.h>
/*
	Interrupt classes
	-----------------
	The class library provides a set of classes that facilitate
	hooking various interrupts.  Because the VMM/VxD environment
	provides several methods for hooking various events that are
	related to interrupts, there are several classes defined here.

	NOTE: For specialized hardware interrupts, see virq.h.

	Classes defined in this file:

	VPreChainV86Int		Interrupts that occur in V86 mode, and
				are to be processed by the handler prior to
				any processing at application level.

	VInChainV86Int		Interrupts that occur in V86 mode, and
				whose handler function is invoked via
				a callback whose address is inserted into
				the V86 interrupt table.  In other words,								
				the handler is effectively chained in the
				application handler chain, and may be
				invoked before and/or after application
				level interrupt handlers have been invoked.
				If the V86 interrupt table vector for the 
				corresponding interrupt is written after
				the hook() function is called, the 
				handler will not be called unless chained
				to by the handler that overhooked it.

	VInChainPMInt		Interrupts that occur in protected mode.
				The handler is invoked via a callback
				inserted into the application level chain,
				and therefore faces the same restrictions
				as VInChainV86Int.


	Hooking Interrupts
	------------------
	By deriving a class from one of the above base classes, you
	implicitly associate a handler function with methods for hooking 
	and unhooking the interrupt.

	The method for hooking interrupts is the same for all types.
	Follow these steps:

	1) Derive a class from one of the base classes, depending on
	   the mode of interrupt you are handling, and (for V86 interrupts)
	   at what point you want the handler to be called.  Your class
	   must define a constructor, and must override the handler
	   member function.

	   example:

	   	class VDosInt : public VPreChainV86Int
		{
		public:
			VDosInt();
			virtual BOOL handler(VMHANDLE hVM, 
						CLIENT_STRUCT* pRegs, DWORD i);
		};
		
	2) Define a constructor that invokes the base class constructor to
	   specify the interrupt you are hooking.

	   example:

	   	VDosInt::VDosInt() : VPreChainV86Int(0x21)
		{
		}

	3) Create a handler member function.

	   example:

	   	BOOL VDosInt::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, DWORD i)
		{
			switch (pRegs->CBRS.Client_AH)
			{
			...
			}
		}

	4) At the appropriate point (usually in OnDeviceInit or OnVMInit),
	   create an instance of your class using the new operator.

		VDosInt* pDosEvent = new VDosInt();

	5) Invoke the hook() function and check the boolean result for
	   success.

	   	bStatus = pDosEvent->hook();


	Post-chain handlers
	-------------------

	The class VPreChainV86Int includes the member functions 
	postChainHandler and enablePostChainHandler.  This feature enables 
	you to examine the state of the interrupted virtual machine after all 
	other handlers have executed and before control passes back to the VM 
	thread.  

	To utilize a post chain handler, you must

	1) Override the virtual function postChainHandler in your interrupt
	   class, and

	2) Call enablePostChainHandler inside your member function handler.
	   Calling this function schedules just one invocation of 
	   postChainHandler.


	Returning from in-chain interrupt handlers
	------------------------------------------

	The handler for class VInChainV86Int and class VInChain

*/

#define PRETHUNKSIZE 	0x20
#define POSTTHUNKSIZE	0x14
#define INT_HOOKED	0x00000001

class VPreChainV86Int 
{
public:
	VPreChainV86Int(int);
	~VPreChainV86Int();
	BOOL hook();
	BOOL unhook();
	VOID enablePostChainHandler(LONG timeout=0, PVOID refData=0);

	// this is inlined to work around internal error in MSVC++ 4.2
	virtual BOOL handler(VMHANDLE,CLIENT_STRUCT*, DWORD)
	{
	  DEBUGEVENT(DBG_ERROR,"\"handler\" for VPreChainV86Int class is not defined correctly");
	  return FALSE;
	}

	virtual VOID postChainHandler(VMHANDLE,CLIENT_STRUCT*,PVOID);

protected:
	int m_iInt;
	DWORD m_flags;
	BYTE m_thunk[PRETHUNKSIZE];
	BYTE m_postThunk[POSTTHUNKSIZE];
};

class VInChainInt
{
public:
	VInChainInt(DWORD);
	BOOL hook();
	BOOL unhook();
	virtual VOID setReturn() 
#ifdef __BORLANDC__
	;
#else
	{
		Simulate_Iret();
	}
#endif
	VOID chainToPrevious();

protected:
	DWORD m_iInt;
	DWORD m_flags;
	WORD m_prevSegment;
	DWORD m_prevOffset;
private:
	virtual VOID _setVector(WORD s,  DWORD o)=0;
	virtual VOID _getVector(WORD* ps, DWORD* po)=0;
	virtual DWORD _getCallback()=0;
};

class VInChainV86Int : public VInChainInt, public VV86Callback
{
public:
	VInChainV86Int(DWORD);
	~VInChainV86Int() {unhook();}

private:
	virtual VOID _setVector(WORD s,  DWORD o);
	virtual VOID _getVector(WORD* ps, DWORD* po);
	virtual DWORD _getCallback() {return m_dwCBAddr;}
};

class VInChainPMInt : public VInChainInt, public VProtModeCallback
{
public:
	VInChainPMInt(DWORD);
	~VInChainPMInt() {unhook();}

private:
	virtual VOID _setVector(WORD s,  DWORD o);
	virtual VOID _getVector(WORD* ps, DWORD* po);
	virtual DWORD _getCallback() {return m_dwCBAddr;}
};

#endif

