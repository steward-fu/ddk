// KNdisTrace.h: interface for the KNdisTrace class.
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
//=============================================================================

#if !defined(AFX_KNDISTRACE_H__5482FBA7_48EE_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISTRACE_H__5482FBA7_48EE_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////
//class KNdisTrace
//
// This is the main tracing class for DriverNetworks.
//
// The class can print a prefix of the form "Prefix: " at the beginning of
// each trace statement so that it is easy to tell where the trace statements
// originated.
//
// The class also supports trace levels.  There is a static class trace level, that
// is compared against the trace level of each individual object.
// For example, you may declare three trace objects:
// KNdisTrace T1("Trace1", TRACE_LEVEL_1);
// KNdisTrace T2("Trace2", TRACE_LEVEL_2);
// KNdisTrace T3("Trace3", TRACE_LEVEL_3);
//
// By default, only T1 will send traces to the debugger, but you may turn any trace
// object on or off in a single statement:
// KNdisTrace::SetClassTraceLevel(TRACE_LEVEL_1 | TRACE_LEVEL_3); //Turns on T1 and T3
//
// For compatibility KNdisTrace supports operator <<.  However, this is inefficient, and
//  is not thread-safe.  Also, operator << will no longer print a prefix at the beginning
//  of each new line.
//
// It is recommended that you use the printf-style Trace() instead.
//
class KNdisTrace
{
public:
	typedef enum{
		NO_LEVELS = 0,
			LEVEL_1 = 1,
			LEVEL_2 = 2,
			LEVEL_3 = 4,
			LEVEL_4 = 8,
			ALL_LEVELS = 0xF
	}TRACE_LEVEL;

//The default class trace level.  Redefine it to change the initial
// class trace value.
#ifndef DEFAULT_CLASS_TRACE_LEVEL
#define DEFAULT_CLASS_TRACE_LEVEL LEVEL_1
#endif


public:
	KNdisTrace(); //Default constructor
	KNdisTrace(PCHAR Prefix,
		TRACE_LEVEL TraceLevel = LEVEL_1);

	//printf style formatting
	VOID Trace(PCHAR fmt, ...);

	//Operator <<
	KNdisTrace&  operator << (const char Character);
	KNdisTrace&  operator << (char const* String);
	KNdisTrace&  operator << (const ULONG dwordValue);
	KNdisTrace&  operator << (const UINT dwordValue){return *this << (ULONG)dwordValue;}
	KNdisTrace&  operator << (const USHORT wordValue);
	KNdisTrace&  operator << (const UCHAR byteValue);
	KNdisTrace&  operator << (WCHAR const* pWideString);
	KNdisTrace&  operator << (NDIS_BUFFER const* pNdisBuffer);
	KNdisTrace&  operator << (NDIS_PACKET const* pNdisPacket);

	//Set the class' state
	VOID SetPrefix(PCHAR Prefix);
	VOID SetTraceLevel(TRACE_LEVEL TraceLevel){m_TraceLevel = TraceLevel;}
	static VOID SetClassTraceLevel(ULONG TraceLevel){sm_ClassTraceLevel = TraceLevel;}

protected:
	//Private function used by operator <<
	VOID TraceNoPrefix(PCHAR FormatString, ...);
	VOID PrintSafeString(PCHAR string);

	//Member variables
protected:
	//The prefix to be printed at the beginning of each line
	CHAR m_Prefix[16];
	UCHAR m_PrefixLength;

	//This object's trace level
	TRACE_LEVEL m_TraceLevel;

	//Class trace level
	static ULONG sm_ClassTraceLevel;
};


class KNdisDebugTrace
#if DBG
: public KNdisTrace
{
public:
	KNdisDebugTrace(PCHAR Prefix,
		TRACE_LEVEL TraceLevel = LEVEL_1):KNdisTrace(Prefix, TraceLevel){}
};
#else
{
public:
	typedef enum{
		NO_LEVELS = 0,
			LEVEL_1 = 1,
			LEVEL_2 = 2,
			LEVEL_3 = 4,
			LEVEL_4 = 8,
			ALL_LEVELS = 0xF
	}TRACE_LEVEL;

public:
	KNdisDebugTrace(PCHAR Prefix,
		TRACE_LEVEL TraceLevel = LEVEL_1){UNREFERENCED_PARAMETERS2(Prefix, TraceLevel);}

	VOID Trace(PCHAR fmt, ...){UNREFERENCED_PARAMETER(fmt);}

	template <typename T> KNdisDebugTrace&  operator << (const T)	{return *this;}
	template <typename T> KNdisDebugTrace&  operator << (const T*)	{return *this;}

	VOID SetPrefix(PCHAR Prefix){UNREFERENCED_PARAMETER(Prefix);}
	VOID SetTraceLevel(TRACE_LEVEL TraceLevel){UNREFERENCED_PARAMETER(TraceLevel);}
	static VOID SetClassTraceLevel(ULONG TraceLevel){UNREFERENCED_PARAMETER(TraceLevel);}
};
#endif

//////////////////////////////////////////////////////////////////////////
//KNdisTrace inline implementations
//////////////////////////////////////////////////////////////////////////
inline KNdisTrace::KNdisTrace():
m_PrefixLength(0),
m_TraceLevel(LEVEL_1)
{
	m_Prefix[0] = '\0';
}

//////////////////////////////////////////////////////////////////////////
inline KNdisTrace::KNdisTrace(PCHAR Prefix,
							  TRACE_LEVEL TraceLevel):
m_TraceLevel(TraceLevel)
{
	SetPrefix(Prefix);
}

//////////////////////////////////////////////////////////////////////////
inline VOID KNdisTrace::SetPrefix(PCHAR Prefix)
{
	m_PrefixLength = (UCHAR)strlen(Prefix);
	ASSERT(m_PrefixLength < 14); //If it is longer we will overflow the buffer

	strcpy(m_Prefix, Prefix);
	m_Prefix[m_PrefixLength++]=':';
	m_Prefix[m_PrefixLength++]=' ';
	m_Prefix[m_PrefixLength]='\0';
}


//////////////////////////////////////////////////////////////////////////
inline KNdisTrace&  KNdisTrace::operator << (const char Character)
{
	if(sm_ClassTraceLevel & m_TraceLevel)
		TraceNoPrefix("%c", Character);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
inline	KNdisTrace&  KNdisTrace::operator << (char const* String)
{
	if(sm_ClassTraceLevel & m_TraceLevel)
		TraceNoPrefix((PCHAR)String);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
inline KNdisTrace&  KNdisTrace::operator << (WCHAR const* pWideString)
{
	if(sm_ClassTraceLevel & m_TraceLevel)
	{
#if !defined (VTOOLSD)
		TraceNoPrefix("%S", pWideString);
#else
		TraceNoPrefix("%U", pWideString);
#endif
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
inline KNdisTrace&  KNdisTrace::operator << (const ULONG dwordValue)
{
	if(sm_ClassTraceLevel & m_TraceLevel)
		TraceNoPrefix("0x%08lx", dwordValue);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
inline KNdisTrace&  KNdisTrace::operator << (const USHORT wordValue)
{
	if(sm_ClassTraceLevel & m_TraceLevel)
		TraceNoPrefix("0x%04x", wordValue);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
inline KNdisTrace&  KNdisTrace::operator << (const UCHAR byteValue)
{
	if(sm_ClassTraceLevel & m_TraceLevel)
		TraceNoPrefix("0x%02x", byteValue);
	return *this;
}

////////////////////////////////////////////////////////////////////
//Old code . . .
#ifndef VTOOLSD
extern "C" {
	ULONG DbgPrint(PCHAR Format,...);
}
#endif
#ifndef TRACE
# if DBG
#  define TRACE	DbgPrint
# else
#  define TRACE
# endif
#endif

// Internal tracing. Define it to something to allow debug traces
// generated by the framework classes
#ifndef KNDIS_TRACE
#define KNDIS_TRACE
#endif

#endif // !defined(AFX_KNDISTRACE_H__5482FBA7_48EE_11D3_8FA3_00C04F7445B7__INCLUDED_)
