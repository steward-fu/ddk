// cpprt.cpp - C++ runtime support for NT/WDM kernel mode drivers
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

// nonstandard extension used : nameless struct/union
#pragma warning (disable:4201)

// unreferenced inline/local function has been removed
#pragma warning (disable:4514)

//Some DDK include files.  We are really only using a few typedefs
// these files can be included independant of the driver type.
extern "C"{
#include <excpt.h>
#include <ntdef.h>
#include <ntstatus.h>
}
#include <..\include\stcinit.h>


/////////////////////////////////////////////////////////////////////////
// Boundary sections for C++ initializer addresses
//
// The linker alphabetically orders the .CRT$ sections. This enables to
// place bounds on the function pointers that the compiler emits for
// static initializations. The following lines declare the lower and
// upper bounds for the initializer pointers array.

#if defined(_IA64_) || defined(_AMD64_)
#pragma section(".CRT$XCA",long,read)
__declspec(allocate(".CRT$XCA")) void (*___StartInitCalls__[1])(void)={0};
#pragma section(".CRT$XCZ",long,read)
__declspec(allocate(".CRT$XCZ")) void (*___EndInitCalls__[1])(void)={0};
#pragma data_seg()
#else
#pragma data_seg(".CRT$XCA")
void (*___StartInitCalls__[1])(void)={0};
#pragma data_seg(".CRT$XCZ")
void (*___EndInitCalls__[1])(void)={0};
#pragma data_seg()
#endif

// List head for list of rundown calls. When compiler-generated initializer
// code calls _atexit, the run time environment generates a new AtExitCall
// object. The AtExitCall objects are kept in a link list, for processing
// during TerminateCppRuntime. The following line declares the list head.

AtExitCall* AtExitCall::m_ListHead=NULL;

// Status of allocations of AtExitCall objects.  When an allocation
// of an AtExitCall object fails, this is set to STATUS_INSUFFICIENT_RESOURCES.
// see _atext function.  When this occurs, the destructor will not be called
// for the C++ object that was constructed.  This could result in a memory leak.
// So the return value of InitializeCppRunTime() should be checked in
// DriverEntry().  If it is a failure, TerminiateCppRunTime() should be called,
// and DriverEntry should return STATUS_INSUFFICIENT_RESOURCES
NTSTATUS AtExitCall::m_Status=STATUS_SUCCESS;

/////////////////////////////////////////////////////////////////////////
// Support for STL initialization
#pragma data_seg(".STL$A")
void (*___StlStartInitCalls__[1])(void)={0};
#pragma data_seg(".STL$L")
void (*___StlEndInitCalls__[1])(void)={0};
#pragma data_seg(".STL$M")
void (*___StlStartTerminateCalls__[1])(void)={0};
#pragma data_seg(".STL$Z")
void (*___StlEndTerminateCalls__[1])(void)={0};
#pragma data_seg()

#pragma optimize("", off) // avoid MSVC 6.0 optimizer bug

////////////////////////////////////////////////////////////////////////
//	InitializeCppRunTime
//		Initialize the C++ run time environment
//
//	Return Value
//		Returns STATUS_INSUFFICIENT_RESOURCES if the allocation of
//			AtExitCall objects fails.  Otherwise, STATUS_SUCCESS
//
//	Comments
// 		This function calls initializers for global variables. It should be
// 		called immediately upon entry to DriverEntry.  The return value
//		should be checked for the case that Cpp Initialization fails.
//
NTSTATUS InitializeCppRunTime(void)
{
	// special init for STL
	void (**p)(void);
 	p = ___StlStartInitCalls__+(ULONG)1;
	while (p < ___StlEndInitCalls__)
	{
		(*p)();
		p++;
	}

	InitCPP();

	return AtExitCall::m_Status;
}

//////////////////////////////////////////////////////////////////////////
// Init CPP
VOID InitCPP()
{
	void (**p)(void);

	// normal C++ init
	p = ___StartInitCalls__+1;
	while (p < ___EndInitCalls__)
	{
		(*p)();
		p++;
	}
}

////////////////////////////////////////////////////////////////////////
//	TerminateCppRunTime
//		Run the exit time calls
//
//	Comments
// 		This function calls all the functions on the AtExit list. It should be
// 		called at the very end of the driver Unload function.
//
VOID TerminateCppRunTime(void)
{
	// call termination handlers
	while (AtExitCall::m_ListHead)
	{
		// invoking the destructor causes the termination handler to be called
		delete AtExitCall::m_ListHead;
	}

	// STL run down
 	void (**p)(void);
 	p = ___StlStartTerminateCalls__+1;
	while (p < ___StlEndTerminateCalls__)
	{
		(*p)();
		p++;
	}
}
#pragma optimize("", on) // avoid MSVC 6.0 optimizer bug

/////////////////////////////////////////////////////////////////
//	atexit
//		Set up an exit time call
//
//	Return Value
//		Returns 0 if not succcessful.
//
//	Comments
// 		This is called by the start-up code to set up calls to
// 		destructors at termination (unload time). It attaches an
// 		AtExitCall object to the global list.
//
int __cdecl atexit(void ( __cdecl *func )( void ))
{
	// Allocate an AtExitCall object
	int i = (new AtExitCall(func) == NULL) ? 0 : 1;

	// Check if the allocation failed
	if (i==0) {

		// call the destructor since it will never be called otherwise
		(*func)();

		// set an error status that always be be checked when
		// InitializeCppRunTime returns
		AtExitCall::m_Status=STATUS_INSUFFICIENT_RESOURCES;
	}

	return i;
}
