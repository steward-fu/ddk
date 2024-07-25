// ktrace.h - include file for trace debug class
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

#ifndef __TRACE__
#define __TRACE__

typedef enum
{
	TraceAlways,
	TraceInfo,
	TraceWarning,
	TraceError,
	TraceFatal,
	TraceNever
} TRACE_LEVEL;

typedef enum
{
	BreakAlways,
	BreakInfo,
	BreakWarning,
	BreakError,
	BreakFatal,
	BreakNever
} BREAK_LEVEL;

typedef enum
{
	TraceHex,
	TraceDecimal
} TRACE_RADIX;

CONST ULONG TRACE_MONITOR	= 1;
CONST ULONG TRACE_DEBUGGER	= 2;
CONST ULONG TRACE_CHECK_FORMAT	= 4;

#if defined(DISABLE_KTRACE)
#error Do not define DISABLE_KTRACE - use class KDebugOnlyTrace
#endif

class KTrace
{

// Construction
public:
	SAFE_DESTRUCTORS

	KTrace(
		PCHAR prefix = NULL,
#if defined _X86_
		ULONG targetmask = TRACE_DEBUGGER|TRACE_CHECK_FORMAT,
#else
		ULONG targetmask = TRACE_DEBUGGER|TRACE_MONITOR|TRACE_CHECK_FORMAT,
#endif
		TRACE_LEVEL outputlevel = TraceAlways,
		BREAK_LEVEL breaklevel = BreakNever,
		PUNICODE_STRING ChannelName = NULL,
		PKINTERRUPT intobject = NULL
		);
	KTrace(
		PUNICODE_STRING prefix,
#if defined _X86_
		ULONG targetmask = TRACE_DEBUGGER|TRACE_CHECK_FORMAT,
#else
		ULONG targetmask = TRACE_DEBUGGER|TRACE_MONITOR|TRACE_CHECK_FORMAT,
#endif
		TRACE_LEVEL outputlevel = TraceAlways,
		BREAK_LEVEL breaklevel = BreakNever,
		PUNICODE_STRING ChannelName = NULL,
		PKINTERRUPT intobject = NULL
		);
	~KTrace(void);
protected:
	VOID __construct(
		PANSI_STRING prefix,
		PUNICODE_STRING ChannelName,
		PKINTERRUPT intobject
		);

private:
	KTrace(KTrace&);
	KTrace& operator=(KTrace&);

// Methods
public:
	KTrace&  operator << (const char v);
	KTrace&  operator << (const unsigned char v);

	KTrace&  operator << (char const* string);
	KTrace&  operator << (WCHAR const* pUnicodeString);
	KTrace&  operator << (UNICODE_STRING const* pUS);

	KTrace&  operator << (const short int v);
	KTrace&  operator << (const unsigned short int v);
	KTrace&  operator << (const int v);
	KTrace&  operator << (const unsigned int v);
	KTrace&  operator << (const long int v);
	KTrace&  operator << (const unsigned long int v);

#if !defined VXD_COMPATLIB
	KTrace&  operator << (const __int64 v);
	KTrace&  operator << (const unsigned __int64 v);
#endif

	KTrace&  operator << (void const* v);
	KTrace&  operator << (const LARGE_INTEGER);

#if !defined VXD_COMPATLIB
	KTrace&  operator << (const KIrp I);
#endif

	KTrace&  operator << (TRACE_LEVEL level);
	KTrace&  operator << (TRACE_RADIX radix);

	VOID SetTarget(ULONG TargetMask);
	VOID SetOutputLevel(TRACE_LEVEL newTraceLevel);
	VOID SetBreakLevel(BREAK_LEVEL newBreakLevel);

	VOID __cdecl Trace(TRACE_LEVEL Level, PCHAR fmt, ...);
protected:
	static	BOOLEAN	Connect(void);
	static	HANDLE (__stdcall *m_Open)(
		PUNICODE_STRING Name,
		PKINTERRUPT pInterrupt,
		ULONG TargetMask
		);
	static	VOID (__stdcall *m_Post)(HANDLE h, PCHAR sz);
	static	VOID (__stdcall *m_Close)(HANDLE h);

// Data
public:
	ULONG		m_TargetMask;
	TRACE_LEVEL	m_TraceLevel;
	BREAK_LEVEL	m_BreakLevel;
	PCHAR		m_Prefix;
	ULONG		m_PrefixLength;
	HANDLE		m_Channel;
	BOOLEAN		m_NeedPrefix;
	TRACE_LEVEL	m_StreamTraceLevel;
	TRACE_RADIX m_Radix;
protected:
	BOOLEAN		m_FreeOnDestroy;
};

////////////////////////////////////////////////////////////////////////////////////
// class KDebugOnlyTrace
//
// If DEBUG is defined, works exactly as KTrace, else does nothing
//
#if DBG
typedef KTrace KDebugOnlyTrace;
#else
class KDebugOnlyTrace
{
// Construction
public:
	SAFE_DESTRUCTORS
	KDebugOnlyTrace(PCHAR prefix = NULL,
		ULONG targetmask = 0,
		TRACE_LEVEL outputlevel = TraceAlways,
		BREAK_LEVEL breaklevel = BreakNever,
		PUNICODE_STRING ChannelName = NULL,
		PKINTERRUPT intobject = NULL
		)
		{
			UNREFERENCED_PARAMETER(prefix);
			UNREFERENCED_PARAMETER(targetmask);
			UNREFERENCED_PARAMETER(outputlevel);
			UNREFERENCED_PARAMETER(breaklevel);
			UNREFERENCED_PARAMETER(ChannelName);
			UNREFERENCED_PARAMETER(intobject);
		}

	KDebugOnlyTrace(
		PUNICODE_STRING prefix,
		ULONG targetmask = 0,
		TRACE_LEVEL outputlevel = TraceAlways,
		BREAK_LEVEL breaklevel = BreakNever,
		PUNICODE_STRING ChannelName = NULL,
		PKINTERRUPT intobject = NULL
		)
		{
			UNREFERENCED_PARAMETER(prefix);
			UNREFERENCED_PARAMETER(targetmask);
			UNREFERENCED_PARAMETER(outputlevel);
			UNREFERENCED_PARAMETER(breaklevel);
			UNREFERENCED_PARAMETER(ChannelName);
			UNREFERENCED_PARAMETER(intobject);
		}

	KDebugOnlyTrace(KTrace&) {}
	~KDebugOnlyTrace(void) {}

// Methods
public:
	VOID SetTarget(ULONG) {}
	VOID SetOutputLevel(TRACE_LEVEL) {}
	VOID SetBreakLevel(BREAK_LEVEL) {}
	VOID __cdecl Trace(TRACE_LEVEL, PCHAR, ...) {}

};	 // KDebugOnlyTrace

template <class U> KDebugOnlyTrace& operator << (KDebugOnlyTrace& t, const U&) { return t; }

#endif	// DBG==1


///////////////////////////////////////////////////////////////////
// Set the output target
//
inline VOID KTrace::SetTarget(ULONG TargetMask)
{
	m_TargetMask = TargetMask;
	if ( (m_Open == 0) && (m_TargetMask & TRACE_MONITOR) )
		Connect();
}

///////////////////////////////////////////////////////////////////
// Set the output threshold
//
inline VOID KTrace::SetOutputLevel(TRACE_LEVEL newTraceLevel)
{
	m_TraceLevel = newTraceLevel;
}

///////////////////////////////////////////////////////////////////
// Set the breakpoint threshold
//
inline VOID KTrace::SetBreakLevel(BREAK_LEVEL newBreakLevel)
{
	m_BreakLevel = newBreakLevel;
}

///////////////////////////////////////////////////////////////////
// Stream-style overrides
//
inline KTrace& KTrace::operator << (const char v)
{
	Trace(m_StreamTraceLevel, "%c", v);
	return *this;
}

inline KTrace&  KTrace::operator << (const unsigned char v)
{
	Trace(m_StreamTraceLevel, m_Radix == TraceHex ? "0x%02x":"%u", v);
	return *this;
}

inline KTrace&  KTrace::operator << (char const* string)
{
	Trace(m_StreamTraceLevel, "%s", string);
	return *this;
}

inline KTrace&  KTrace::operator << (const short int v)
{
	Trace(m_StreamTraceLevel, m_Radix == TraceHex ? "0x%04hx":"%d", v);
	return *this;
}

inline KTrace&  KTrace::operator << (const unsigned short int v)
{
	Trace(m_StreamTraceLevel, m_Radix == TraceHex ? "0x%04hx":"%u", v);
	return *this;
}

inline KTrace&  KTrace::operator << (const int v)
{
	Trace(m_StreamTraceLevel, m_Radix == TraceHex ? "0x%08x":"%d", v);
	return *this;
}

inline KTrace&  KTrace::operator << (const unsigned int v)
{
	Trace(m_StreamTraceLevel, m_Radix == TraceHex ? "0x%08x":"%u", v);
	return *this;
}

inline KTrace&  KTrace::operator << (const long int v)
{
	Trace(m_StreamTraceLevel, m_Radix == TraceHex ? "0x%08lx":"%d", v);
	return *this;
}

inline KTrace&  KTrace::operator << (const unsigned long int v)
{
	Trace(m_StreamTraceLevel, m_Radix == TraceHex ? "0x%08lx":"%u", v);
	return *this;
}

#if !defined VXD_COMPATLIB
inline KTrace&  KTrace::operator << (const unsigned __int64 v)
{
	Trace(m_StreamTraceLevel, m_Radix == TraceHex ? "%016I64x":"I64u", v);
	return *this;
}

inline KTrace&  KTrace::operator << (const __int64 v)
{
	Trace(m_StreamTraceLevel, m_Radix == TraceHex ? "0x%016I64x":"%I64d", v);
	return *this;
}
#endif

inline KTrace&  KTrace::operator << (void const* p)
{
	Trace(m_StreamTraceLevel, "%p", p);
	return *this;
}

inline KTrace&  KTrace::operator << (const LARGE_INTEGER i)
{
	Trace(m_StreamTraceLevel, "%I64x", i.QuadPart);
	return *this;
}

inline KTrace&  KTrace::operator << (WCHAR const* pUnicodeString)
{
#if !defined (VXD_COMPATLIB)
	Trace(m_StreamTraceLevel, "%S", pUnicodeString);
#else
	Trace(m_StreamTraceLevel, "%U", pUnicodeString);
#endif
	return *this;
}

inline KTrace&  KTrace::operator << (UNICODE_STRING const* pUnicodeString)
{
#if !defined (VXD_COMPATLIB)
	Trace(m_StreamTraceLevel, "%-*S", pUnicodeString->Length/sizeof(WCHAR), pUnicodeString->Buffer);
#else
	Trace(m_StreamTraceLevel, "%-*U", pUnicodeString->Length/sizeof(WCHAR), pUnicodeString->Buffer);
#endif
	return *this;
}

inline KTrace&  KTrace::operator << (TRACE_LEVEL level)
{
	m_StreamTraceLevel = level;
	return *this;
}

inline KTrace&  KTrace::operator << (TRACE_RADIX radix)
{
	m_Radix = radix;
	return *this;
}

#endif	// __TRACE__
