// glbtrace.h - global trace object
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

// Global Trace Object

// Use this include file to declare a global instance of KTrace.
// Include the file in all modules that need to do tracing. The
// output is to the default channel. 
//
// To use global trace, you must:
//
// 1) Declare a global variable KTrace* Gt=0;
// 2) Call SetupGlobalTrace(prefix) during initialization
// 3) Call ShutdownGlobalTrace during Unload
//

// !! Comment out this line to enable tracing !!
#define DISABLE_TRACE 1

// These are default levels for tracing and breaking
#ifndef TLEVEL
#define TLEVEL TraceInfo
#endif

#ifndef BLEVEL
#define BLEVEL BreakNever
#endif

// Macro GTRACE
//
// When tracing is enabled, the macro calls KTrace::Trace. The
// argument list must be enclosed in parentheses, e.g.:
//
//		GTRACE( (TLEVEL,"the result is %x\n", variablename) );
//
#ifdef DISABLE_TRACE
#define GTRACE(x)
#else
#define GTRACE(x) Gt->Trace x
#endif

extern KTrace* Gt;

//
// Call this during initialization
//
inline VOID SetupGlobalTrace(PUNICODE_STRING prefix)
{
	Gt = new (NonPagedPool) 
		KTrace(
			prefix,
			TRACE_DEBUGGER|TRACE_MONITOR,
			TraceInfo,
			BreakNever,
			NULL,
			NULL
			);

	ASSERT(Gt);
}

//
// Call this during Unload
//
inline VOID ShutdownGlobalTrace(void)
{
	if (Gt) delete Gt;
}
