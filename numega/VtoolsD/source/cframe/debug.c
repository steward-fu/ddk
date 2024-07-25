//	DEBUG.C
//
//	Provide debug support
//
//	Copyright (c) 1994 by Compuware Corporation
//



#include <vtoolsc.h>

#ifdef DEBUG
extern DDB The_DDB;

// Default levels

DEBUGLEVEL	_dbgBreakLevel   = DBG_ERROR;
DEBUGLEVEL	_dbgMessageLevel = DBG_WARNING;
DEBUGOUTPUT	_dbgOutput	 = DBG_SERIAL;

void SetDebugLevel(DEBUGLEVEL breakLevel, DEBUGLEVEL messageLevel)
{
	_dbgBreakLevel = breakLevel;
	_dbgMessageLevel = messageLevel;
}

void DebugString(char* string)
{
	switch (_dbgOutput) {
		case DBG_MONO:
			_asm pushad
			Out_Mono_String(string);
			_asm popad
			break;
		case DBG_SERIAL:
			_asm pushad
			Out_Debug_String(string);
			_asm popad
			break;
		case DBG_NUL:
			break;
	}
}

void DebugEvent(DEBUGLEVEL dbgLevel, const char* message)
{
	char	temp[256];

	if (dbgLevel >= _dbgMessageLevel)
	{
		sprintf(temp, "%.8s: %s\n", &The_DDB.DDB_Name, message);
		DebugString(temp);
	}

	if (dbgLevel >= _dbgBreakLevel)
		DebugBreak();
}

void SetDebugOutput(DEBUGOUTPUT dd)
{
	switch (dd) {
		case DBG_MONOCLR:
			Clear_Mono_Screen();
			// fall through
		case DBG_MONO:
			_dbgOutput = DBG_MONO;
			break;
		case DBG_SERIAL:
			_dbgOutput = DBG_SERIAL;
			break;
		case DBG_NUL:
			_dbgOutput = DBG_NUL;
	}
}


VOID AssertFailure(char *string, int line)
{
	char temp[256];

	sprintf(temp, string, line);
	DEBUGERROR(temp);
}


#else
// NOT DEBUG

extern DDB The_DDB;

// Default levels

DEBUGLEVEL	_dbgBreakLevel   = DBG_ERROR;
DEBUGLEVEL	_dbgMessageLevel = DBG_WARNING;
DEBUGOUTPUT	_dbgOutput	 = DBG_SERIAL;

void SetDebugLevel(DEBUGLEVEL breakLevel, DEBUGLEVEL messageLevel)
{
	_dbgBreakLevel = breakLevel;
	_dbgMessageLevel = messageLevel;
}

void DebugString(char* string)
{
	switch (_dbgOutput) {
		case DBG_MONO:
			_asm pushad
					Out_Mono_String(string);
			_asm popad
					break;
		case DBG_SERIAL:
			_asm pushad
					Out_Debug_String(string);
			_asm popad
					break;
		case DBG_NUL:
			break;
	}
}

void DebugEvent(DEBUGLEVEL dbgLevel, const char* message)
{
	char	temp[256];

	if (dbgLevel >= _dbgMessageLevel)
	{
		sprintf(temp, "%.8s: %s\n", &The_DDB.DDB_Name, message);
		DebugString(temp);
	}

	if (dbgLevel >= _dbgBreakLevel)
		DebugBreak();
}

void SetDebugOutput(DEBUGOUTPUT dd)
{
	switch (dd) {
		case DBG_MONOCLR:
			Clear_Mono_Screen();
			// fall through
		case DBG_MONO:
			_dbgOutput = DBG_MONO;
			break;
		case DBG_SERIAL:
			_dbgOutput = DBG_SERIAL;
			break;
		case DBG_NUL:
			_dbgOutput = DBG_NUL;
	}
}


VOID AssertFailure(char *string, int line)
{
	char temp[256];

	sprintf(temp, string, line);
	DEBUGERROR(temp);
}


#endif
