// cpprt.h - C++ definitions for kernel mode
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

#ifndef __STCINIT_H__
#define __STCINIT_H__


////////////////////////////////////////////////////////////////////////
// InitializeCppRunTime
//
// This function calls initializers for global variables. It should be
// called immediately upon entry to DriverEntry.
//
NTSTATUS InitializeCppRunTime(void);

////////////////////////////////////////////////////////////////////////
// InitCPP
//
// This is a helper function called within InitializeCppRunTime
//
VOID InitCPP();

////////////////////////////////////////////////////////////////////////
// TerminateCppRunTime
//
// This function calls all the functions on the AtExit list. It should be
// called at the very end of the driver Unload function.
//
VOID TerminateCppRunTime(void);

////////////////////////////////////////////////////////////////////////
// Safe Destructors
//
//	If a class does not explicitly define a destructor, the compiler
//	generates one if (1) a base class has an accessible destructor, or if
//	(2) a data member has an acessible destructor.
//
//	The code section into which the compiler emits a destructor depends on
//	whether or not the destructor is virtual. If not, it goes in the same
//	section as the first code that references it. If it is virtual, it goes in
//	the same section as the first invocation of a constructor seen by the
//	compiler. This is not documented, but it is the observed behavior of
//	MSVC++. ( In general, you can control the code section to which the
//	compiler emits code by using #pragma code_seg("section-name") ).
//
//	This behavior creates a problem if a compiler-generated destructor is
//	emitted to an INIT section. Attempting to invoke such a destructor after
//	the INIT sections have been discarded causes a fatal error.
//
//	Note that the problem can occur even if all member functions of the
//	subclass (including constructors) are outside the INIT section. As long
//	as the first reference to the destructor is in the INIT section, the
//	problem arises.
//
//	In the case where there is a virtual destructor in the base class, even
//	having the first reference to the destructor of the subclass outside of
//	the INIT section is not sufficient to avert the problem. The compiler
//	also generates a "scalar deleting destructor" and "vector deleting
//	destructor", and these will still be generated into the INIT section if
//	the constructor of the subclass is in that section, and there is no
//	prior invocation of the delete operator on the subclass outside the INIT
//	section.
//
//	Furthermore, the "scalar/vector deleting destructors are also
//	generated in a non-optimized build for any class that has a destructor,
//	and an instance of it gets deleted. So even classes that have destructors
//	are subject to having the compiler generate additional destructor-related
//	code, which can fall into the wrong code section.
//
//	To work around this problem, we recommend use of this macro:

#define SAFE_DESTRUCTORS void _self_delete(void){delete this;}

//	Using this macro inside the definition of a class creates an in-line
//	reference to the destructor while the current code section is still
//	".text" (assuming there is no #pragma code_seg(xxxx) ahead of the
//	include files). This is sufficient to cause the compiler to generate any
//	non-virtual destructors that it needs into ".text". For example:
//
//	        class MyDevice : public KDevice
//	        {
//	                SAFE_DESTRUCTORS
//	        public:
//	                MyDevice(whatever ...)
//	                ...
//	        };
//
//	NOTE: If you . . .
//		1. want to have a virtual destructor,
//		2. want to have a constructor in the INIT section, and
//		3. you don't want the virtual destructor to be in the INIT section,
//
//	Then you must have another constructor, which may be just a dummy, that
//		1. is in the section in which you want the virtual destructor to be, and
//		2. appears prior to the constructor in the INIT section in the
//		   same source module
//
//	As a general rule, you should always put the INIT section at the bottom
//	of the module. Also, remember that any subclasses of classes that have
//	virtual destructors will also have virtual destructors.
//
//	Note that you can achieve functionality equivalent to virtual destructors by inserting the
//  word 'virtual' before the invocation of SAFE_DESTRUCTORS in a base class, and then
//	calling member _self_delete to delete an instance of the class or a subclass. The
//  subclass should define a non-virtual destructor and use SAFE_DESTRUCTORS.
//
//	When should SAFE_DESTRUCTORS be used? Whenever the compiler might
//	generate a destructor, it's a good idea to use the macro. Remember that
//	even if a class defines its own destructor, the compiler may still
//	generate the "scalar/vector deleting destructors" which cause the same
//	result. Of course, it is always safe to use SAFE_DESTRUCTORS. It will
//	not cause any conflicts.
//
// 	The other problem to watch out for involves constructing arrays. Consider
//	the following code:
//
//		#pragma code_seg("INIT")
//		void func1(void)
//		{
//			CSomeClass ClassArray[10];
//			// ..
//		}
//
//		#pragma code_seg()
//		void func2(void)
//		{
//			CSomeClass ClassArray[10];
//			// ..
//		}
//
//	In this case, the compiler-generated helper routine that constructs
//	the array of instances ends up in the INIT section. This causes a
//	fatal error if func2 is executed after the INIT sections have been
//	discarded. Correct this problem by reordering the functions within
//	the module.

////////////////////////////////////////////////////////////////////////
// AtExitCall
//
// This object is used to maintain a list of functions to be called
// at unload time.

class AtExitCall
{
	SAFE_DESTRUCTORS
public:
	// The constructor links the object into the global list

	AtExitCall(void ( __cdecl *func )( void ))
	{
		m_Function = func;
		m_Next = m_ListHead;
		m_ListHead = this;
	}

	// The destructor removes the head of the list and calls the function.

	~AtExitCall(void)
	{
		m_Function();
		m_ListHead = this->m_Next;
	}

	// link to next list item
	AtExitCall* m_Next;

	// function pointer
	void ( __cdecl *m_Function)( void );

	// head of the list
	static AtExitCall* m_ListHead;

	// initialization status
	static NTSTATUS m_Status;
};

#endif	// __STCINIT_H__
