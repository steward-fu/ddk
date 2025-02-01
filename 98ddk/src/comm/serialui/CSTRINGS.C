//---------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation 1993-1994
//
// File: cstrings.c
//
//  This file contains read-only string constants
//
// History:
//  12-23-93 ScottH     Created 
//
//---------------------------------------------------------------------------

#include "suiprv.h"

#pragma data_seg(DATASEG_READONLY)

char const FAR c_szNULL[] = "";
char const FAR c_szZero[] = "0";
char const FAR c_szDelim[] = " \t,";
char const FAR c_szBackslash[] = "\\";

char const FAR c_szWinHelpFile[] = "windows.hlp";

// Registry key names

char const FAR c_szClass[] = REGSTR_KEY_CLASS;
char const FAR c_szPortClass[] = "ports";
char const FAR c_szModemClass[] = "Modem";
char const FAR c_szDeviceDesc[] = "DeviceDesc";
char const FAR c_szPortName[] = "PortName";
char const FAR c_szFriendlyName[] = REGSTR_VAL_FRIENDLYNAME;
char const FAR c_szDCB[] = "DCB";

#ifdef DEBUG

// Ini file name

char const FAR c_szIniFile[] = "rover.ini";

// Ini section names

char const FAR c_szIniSecDebugUI[] = "SerialUIDebugOptions";

// Ini key names

char const FAR c_szIniKeyTraceFlags[] = "TraceFlags";
char const FAR c_szIniKeyDumpFlags[] = "DumpFlags";
char const FAR c_szIniKeyBreakOnOpen[] = "BreakOnOpen";
char const FAR c_szIniKeyBreakOnClose[] = "BreakOnClose";
char const FAR c_szIniKeyBreakOnRunOnce[] = "BreakOnRunOnce";
char const FAR c_szIniKeyBreakOnValidate[] = "BreakOnValidate";
char const FAR c_szIniKeyBreakOnThreadAtt[] = "BreakOnThreadAttach";
char const FAR c_szIniKeyBreakOnThreadDet[] = "BreakOnThreadDetach";
char const FAR c_szIniKeyBreakOnProcessAtt[] = "BreakOnProcessAttach";
char const FAR c_szIniKeyBreakOnProcessDet[] = "BreakOnProcessDetach";

#endif

#pragma data_seg()

