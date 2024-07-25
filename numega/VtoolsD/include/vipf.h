// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vipf.h - include file for Invalid Page Fault class

#ifndef __vipf_h_
#define __vipf_h_

#include <vmachine.h>

/*
	Invalid Page Fault Class
	------------------------

	Use the VInvalidPageFault class to catch page faults that the
	system does not handle.  

	By deriving a class from VInvalidPageFault, you	implicitly associate 
	a handler function with the methods for hooking and unhooking 
	the fault.

	To hook invalid page faults, follow these steps:

	1) Derive a class from the base class VInvalidPageFault.
	   Your class must declare a constructor and override the
	   member function handler().

	   example:

		class MyIPF : public VInvalidPageFault
		{
			MyIPF();
			virtual BOOL handler(VMHANDLE hVM, IPF_Data* pIpf);
		};

	2) Write the constructor. It is sufficient to simply invoke the
	   constructor of VInvalidPageFault.

	   example:

		MyIPF::MyIPF() : VInvalidPageFault()
		{
		}

	3) Write the handler for your class.

	   example:

		BOOL MyIPF::handler(VMHANDLE hVM, IPF_Data* pIpf)
		{
		 . . . whatever your handler does
		}

	   See vmm.h for a description of struct IPF_Data.


	4) Create an instance of your class (ususally at Device_Init time)
	   using the new operator.

	   example:

		pMyIPF = new MyIPF();

	5) Call the hook() member function to hook invalid page faults to
	   your handler function.

	   example:

	   	bStatus = pMyIPF->hook();

	Your handler function should return TRUE if it handles the 
	fault. Otherwise it should return FALSE to signal the VMM
	that the next handler needs to be called.
*/

#define IPF_HOOKED 0x00000001
#define IPFTHUNKSIZE 0x14

class VInvalidPageFault
{
public:
	VInvalidPageFault()
		{m_flags = 0;};
	~VInvalidPageFault()
		{unhook();};
	BOOL hook();
	BOOL unhook();
	virtual BOOL handler(VMHANDLE hVM, PIPF_DATA pIpf, CLIENT_STRUCT* pRegs);

protected:
	DWORD m_flags;
private:
	BYTE m_thunk[IPFTHUNKSIZE];

};

#endif
