// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vfault.h - include file for fault classes
//

#include <vmachine.h>

#ifndef __vfault_h_
#define __vfault_h_

/*
	Fault classes
	-------------

	The class library provides classes that facilitate hooking
	faults.  You can hook these different kinds of faults:

	class VV86ModeFault	Faults occurring in V86 mode
	class VProtModeFault	Faults occurring in protected mode
	class VVMMFault		Faults occurring at the VxD level 
	class VNMIEvent		Non-maskable interrupt event

	By deriving a class from one of the above base classes, you
	implicitly associate a handler function with methods for hooking 
	and unhooking the fault.

	Follow these steps to hook a fault:

	
	1) Depending on the kind of fault you wish to hook, derive a class 
	   from either VProtModeFault, VV86ModeFault, or VVMMFault. Your
	   class must define a constructor and override the handler member
	   function of VFault.

	   example:

		class My_V86_GPFault : public VV86ModeFault
		{
		public:
			My_V86_GPFault();	// constructor
			virtual BOOL handler(VMHANDLE, CLIENT_STRUCT*, int);
		}

	2) Define a constructor for your class that calls the base class
	   constructor, passing the fault number.
	
	   example:

		My_V86_GPFault::My_V86_GPFault() : VV86ModeFault(0xD)
		{
		}

	3) Override VFault::handler with the handler for your fault.

	   example:

		BOOL My_V86_GPFault::handler(VMHANDLE h, CLIENT_STRUCT* pRegs, int i)
		{
			// whatever your handler does
		}

	4) At the appropriate point (usually at Device_Init time), create
	   an instance with new:

	   example:

		My_V86_GPFault* pGP = new My_V86_GPFault();


	5) Call the hook member function of the fault class instance.

	   example:

		bStatus = pGP->hook();


	6) When you wish to unhook the fault, call the unhook member.

	   example:

	   	bStatus = pGP->unhook();
*/

// These constants are used internally by the VFault class

#define FAULTTHUNKSIZE	0x40
#define FAULT_VALID	0x00000001
#define FAULT_HOOKED	0x00000002

// the base class for faults

class VFault	
{
public:
	VFault(int);
	virtual BOOL hook();
	virtual BOOL unhook();
	virtual BOOL handler(VMHANDLE, CLIENT_STRUCT*, int);

protected:
	DWORD	m_flags;
	BYTE	m_iFault;
	BYTE	m_thunk[FAULTTHUNKSIZE];

private:
	virtual BOOL _hookProc(int, PVOID, PVOID*)=0;
	virtual BOOL _unhookProc(int, PVOID)=0;
};


class VProtModeFault : public VFault
{
public:
	VProtModeFault(int);	

private:
	virtual BOOL _hookProc(int,PVOID, PVOID*);
	virtual BOOL _unhookProc(int,PVOID);
};

class VV86ModeFault : public VFault
{
public:
	VV86ModeFault(int);	

private:
	virtual BOOL _hookProc(int,PVOID, PVOID*);
	virtual BOOL _unhookProc(int,PVOID);
};

class VVMMFault : public VFault
{
public:
	VVMMFault(int);	

private:
	virtual BOOL _hookProc(int,PVOID, PVOID*);
	virtual BOOL _unhookProc(int,PVOID);
};

class VNMIEvent : public VFault
{
public:
	VNMIEvent();	

private:
	virtual BOOL _hookProc(int,PVOID, PVOID*);
	virtual BOOL _unhookProc(int,PVOID);
};

#endif
