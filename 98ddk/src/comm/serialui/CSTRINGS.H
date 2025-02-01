//---------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation 1993-1994
//
// File: cstrings.h
//
//---------------------------------------------------------------------------

#ifndef _CSTRINGS_H_
#define _CSTRINGS_H_

extern char const FAR c_szNULL[];
extern char const FAR c_szZero[];
extern char const FAR c_szDelim[];
extern char const FAR c_szBackslash[];

extern char const FAR c_szWinHelpFile[];

// Registry key names

extern char const FAR c_szClass[];
extern char const FAR c_szPortClass[];
extern char const FAR c_szModemClass[];
extern char const FAR c_szDeviceDesc[];
extern char const FAR c_szPortName[];
extern char const FAR c_szFriendlyName[];
extern char const FAR c_szDCB[];

#ifdef DEBUG

// These declarations are located in err.c
//
extern char const FAR c_szNewline[];
extern char const FAR c_szTrace[];
extern char const FAR c_szDbg[];
extern char const FAR c_szAssertFailed[];

#endif

#ifdef DEBUG

// Ini file name

extern char const FAR c_szIniFile[];

// Ini section names

extern char const FAR c_szIniSecDebugUI[];

// Ini key names

extern char const FAR c_szIniKeyTraceFlags[];
extern char const FAR c_szIniKeyDumpFlags[];
extern char const FAR c_szIniKeyBreakOnOpen[];
extern char const FAR c_szIniKeyBreakOnClose[];
extern char const FAR c_szIniKeyBreakOnRunOnce[];
extern char const FAR c_szIniKeyBreakOnValidate[];
extern char const FAR c_szIniKeyBreakOnThreadAtt[];
extern char const FAR c_szIniKeyBreakOnThreadDet[];
extern char const FAR c_szIniKeyBreakOnProcessAtt[];
extern char const FAR c_szIniKeyBreakOnProcessDet[];

#endif


#endif  // _CSTRINGS_H_

