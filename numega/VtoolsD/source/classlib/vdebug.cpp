// vdebug.cpp - implementation of debugging classes
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
//

#include <vtoolscp.h>

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

extern "C" void __cdecl DebugString(char* string);

#ifdef DEBUG

DEBUGLEVEL	_dbgBreakLevel   = DBG_ERROR;
DEBUGLEVEL	_dbgMessageLevel = DBG_HINT;
DEBUGOUTPUT	_dbgOutputDevice = DBG_SERIAL;

Vdbostream dout;
Vdbistream din;
Vmonostream dmono;

extern "C" DDB The_DDB;


// Function
//	Vdbostream::_output(const char* )
//
// Syntax
//	VOID Vdbostream::_output(const char* )
//
// Remarks
//	Sends a null-terminated ascii string to the debug serial port.
//	This is a virtual function that may be overridden by debug
//	streams that have a different output device (such as the 
//	monochrome device).
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the output stream to allow compound
//	stream expressions.
//
// Example
//	The code
//
//		dout << "This is a test"
//
//	sends the string "This is a test" to the output stream. 
//
VOID Vdbostream::_output(const char* string)
{
	LOCK_Out_Debug_String(string) ;
}

// Function
//	Vdbostream::operator<< (const char* )
//
// Syntax
//	Vdbostream& Vdbostream::operator<< (const char* )
//
// Remarks
//	Sends a null-terminated ascii string to the output stream.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the output stream to allow compound
//	stream expressions.
//
// Example
//	The code
//
//		dout << "This is a test"
//
//	sends the string "This is a test" to the output stream. 
//
//	Before sending output to a debug stream, send either 
//	DBG_SERIAL or DBG_MONO to set the output device. The device
//	setting persists through the life of the stream.
//
Vdbostream&  Vdbostream::operator << (const char* string)
{
	_output(string);
	return *this;
}


// Function
//	Vdbostream::operator<< (char)
//
// Syntax
//	Vdbostream& Vdbostream::operator << (char* asciiChar)
//
// Remarks
//	Sends an ascii to the output stream.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the output stream to allow compound
//	stream expressions.
//
// Example
//	The code
//
//		dout << 'X'
//
//	sends the character 'X' to the output stream. 
//
Vdbostream&  Vdbostream::operator << (const char dbch)
{
	char buf[2];
	
	buf[0] = dbch;
	buf[1] = 0;
	_output(buf);
	return *this;
}

// Function
//	Vdbostream::operator<< (BYTE)
//
// Syntax
//	Vdbostream& Vdbostream::operator << (BYTE unsByte)
//
// Remarks
//	Sends a string representing the value of the BYTE argument
//	in hexadecimal to the output stream.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the output stream to allow compound
//	stream expressions.
//
// Example
//	The code
//
//		BYTE b = 10;
//		dout << b;
//
//	sends the string "0A" to the output stream. 
//
Vdbostream&  Vdbostream::operator << (const BYTE bValue)
{
	const char* hexdigs="0123456789abcdef";

	return *this << hexdigs[bValue >> 4] << hexdigs[bValue & 0xf];
}

// Function
//	Vdbostream::operator<< (WORD)
//
// Syntax
//	Vdbostream& Vdbostream::operator << (WORD unsShort)
//
// Remarks
//	Sends a string representing the value of the WORD argument
//	in hexadecimal to the output stream.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the output stream to allow compound
//	stream expressions.
//
// Example
//	The code
//
//		WORD w = 256;
//		dout << w;
//
//	sends the string "0100" to the output stream. 
//
Vdbostream&  Vdbostream::operator << (const WORD wValue)
{
	return *this << (BYTE)(wValue >> 8) << (BYTE)(wValue & 0xff);
}

// Function
//	Vdbostream::operator<< (DWORD)
//
// Syntax
//	Vdbostream& Vdbostream::operator << (DWORD unsLong)
//
// Remarks
//	Sends a string representing the value of the DWORD argument
//	in hexadecimal to the output stream.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the output stream to allow compound
//	stream expressions.
//
// Example
//	The code
//
//		DWORD w = 256;
//		dout << w;
//
//	sends the string "00000100" to the output stream. 
//
Vdbostream&  Vdbostream::operator << (const DWORD dwValue)
{
	return *this << (WORD)(dwValue >> 16) << (WORD)(dwValue & 0xffffL);
}

// Function
//	Vdbistream::operator>> (char)
//
// Syntax
//	Vdbistream& Vdbistream::operator >> (char dbch)
//
// Remarks
//	Reads a character from the debug console.  
//
// Return Value
//	Returns a reference to the input stream to allow compound
//	stream expressions.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Example
//	The code
//		char ch
//		din >> ch;
//
//	reads a character into the variable ch.
//
Vdbistream&  Vdbistream::operator >> (char& dbch)
{
	dbch = In_Debug_Chr();
	dout << dbch;
	return *this;
}

// Function
//	Vdbistream::operator>> (char*)
//
// Syntax
//	Vdbistream& Vdbistream::operator >> (char* string)
//
// Remarks
//	Reads a string from the debug console.  A string is terminated
//	by a carriage return, newline, escape, or Ctrl-C.  The terminating
//	character is consumed.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the input stream to allow compound
//	stream expressions.
//
// Example
//	The code
//		char buf[100];
//		din >> buf;
//
//	reads a string into buf.
//
//
Vdbistream&  Vdbistream::operator >> (char* string)
{
	char ch;

	*this >> ch;
	while ( (ch != 0x3) && (ch != '\r') && (ch != '\n') && (ch != 0x1B))
	{
		*string++ = ch;
		*this >> ch;
	} 

	return *this;
}

// Function
//	Vdbistream::operator>> (DWORD&)
//
// Syntax
//	Vdbistream& Vdbistream::operator >> (DWORD& dw)
//
// Remarks
//	Reads a DWORD value from the debug console.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the input stream to allow compound
//	stream expressions.
//
// Example
//	The code
//		DWORD dw;
//		din >> dw;
//	reads a DWORD from the debug console into the variable dw.
//
Vdbistream&  Vdbistream::operator >> (DWORD& dwordValue)
{
	dwordValue = this->getHex();
	return *this;
}

// Function
//	Vdbistream::operator>> (WORD)
//
// Syntax
//	Vdbistream& Vdbistream::operator >> (WORD& w)
//
// Remarks
//	Reads a word value from the debug console.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the input stream to allow compound
//	stream expressions.
//
// Example
//	The code
//		WORD w;
//		din >> w;
//	reads a WORD from the debug console into the variable w.
//
Vdbistream&  Vdbistream::operator >> (WORD& wordValue)
{
	wordValue = (WORD)this->getHex();
	return *this;
}

// Function
//	Vdbistream::operator>> (BYTE)
//
// Syntax
//	Vdbistream& Vdbistream::operator >> (BYTE& b)
//
// Remarks
//	Reads an unsigned character value from the debug console.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the input stream to allow compound
//	stream expressions.
//
// Example
//	The code
//		BYTE b;
//		din >> b;
//	reads a BYTE from the debug console into the variable b.
//
//
Vdbistream&  Vdbistream::operator >> (BYTE& byteValue)
{
	byteValue = (BYTE)this->getHex();
	return *this;
}

// Function
//	Vdbistream::getHex
//
// Syntax
//	DWORD Vdbistream::getHex()
//
// Remarks
//	Reads a numeric value, expressed in hex, from the debug console.
//	The number is terminated by any character that is not a hex digit.
//	Leading whitespace (tabs, spaces) is permitted.  The terminating
//	character is consumed.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns the numeric value read from the debug console.
//
DWORD Vdbistream::getHex()
{
	DWORD d;
	DWORD val;
	char ch;

	*this >> ch;

	while ( (ch == ' ') || (ch == '\t') )
		*this >> ch;

	for (d=0; TRUE; d <<= 4, d += val)
	{

		if ( ('0' <= ch) && (ch <= '9') )
		{
			val = (ch - '0');
		}
		else if ( ('a' <= ch) && (ch <= 'f') )
		{
			val = (ch - 'a') + 10;
		}
		else if ( ('A' <= ch) && (ch <= 'F') )
		{
			val = (ch - 'A') + 10;
		}
		else
			break;

		*this >> ch;
	}

	return d;
}

// Function
//	Vmonostream::_output(const char* )
//
// Syntax
//	VOID Vmonostream::_output(const char* )
//
// Remarks
//	Sends a null-terminated ascii string to the monochrome display.
//
//	Note: this function only works with the DEBUG version of Windows.
//
// Return Value
//	Returns a reference to the output stream to allow compound
//	stream expressions.
//
// Example
//	The code
//
//		dmono << "This is a test"
//
//	sends the string "This is a test" to the output stream. 
//
void Vmonostream::_output(const char* string)
{
	Out_Mono_String(string);
}

// Function
//	Vmonostream::setCursorPosition
//
// Syntax
//	VOID Vmonostream::setCursorPosition(DWORD row, DWORD col)
//
// Remarks
//	Sets the cursor position to the specified row and column 
//	on the monochrome display.
//
//	Note: this function only works with the DEBUG version of Windows.
//
void Vmonostream::setCursorPosition(DWORD row, DWORD col)
{
	Set_Mono_Cur_Pos(row, col);
}

// Function
//	Vmonostream::getCursorPosition
//
// Syntax
//	VOID Vmonostream::getCursorPosition(DWORD& row, DWORD& col)
//
// Remarks
//	Gets the cursor position (row and column) from the monochrome
//	display.
//
//	Note: this function only works with the DEBUG version of Windows.
//
void Vmonostream::getCursorPosition(DWORD& row, DWORD& col)
{
	DWORD pos = Get_Mono_Cur_Pos();

	row = (pos >> 8) & 0xff;
	col = pos & 0xff;
}

// Function
//	Vmonostream::clearScreen
//
// Syntax
//	VOID Vmonostream::clearScreen()
//
// Remarks
//	Clears the monochrome display.
//
//	Note: this function only works with the DEBUG version of Windows.
//
void Vmonostream::clearScreen()
{
	Clear_Mono_Screen();
}


// Function
//	SetDebugLevel - set debug message/breakpoint system parameters
//
// Syntax
//	void SetDebugLevel(DEBUGLEVEL breakLevel, DEBUGLEVEL messageLevel)
//
// Remarks
//	Sets the threshold for issuing messages to the debug stream and
//	breaking (via int 3) for calls to DebugEvent.
//
//	Invoke this function via the macro SETDEBUGLEVEL(bl, ml), which is
//	a nop when DEBUG is not defined.
//
//	The enum DEBUGLEVEL is defined in vdebug.h.
//
// Example
//	The code
//		SETDEBUGLEVEL(DBG_ERROR, DBG_WARNING)
//		
//	causes calls to DebugEvent (usually via macro DEBUGEVENT) to break
//	if the level specified in the event is DBG_ERROR, and to issue a
//	message to the debug stream if the level specified in the event is
//	DBG_WARNING or higher.
//
void SetDebugLevel(DEBUGLEVEL breakLevel, DEBUGLEVEL messageLevel)
{
	_dbgBreakLevel = breakLevel;
	_dbgMessageLevel = messageLevel;
}

// Function
//	DebugEvent - issue a message and break, depending on debug levels
//
// Syntax
//	void DebugEvent(DEBUGLEVEL dbgLevel, const char* message)
//
// Remarks
//	If the current setting for message level is less than or equal
//	to parameter dbgLevel, then this routine issues a message via
//	the debug output stream, dout.  The message is prefixed by the
//	device name.
//
//	If the current setting for break level is less than or equal to
//	parameter dbgLevel, then this routine issues a breakpoint (int 3).
//
//	The enum DEBUGLEVEL is defined in vdebug.h.
//
// Example
//	The code
//		SETDEBUGLEVEL(DBG_ERROR, DBG_WARNING)
//		
//	causes calls to DebugEvent (usually via macro DEBUGEVENT) to break
//	if the level specified in the event is DBG_ERROR, and to issue a
//	message to the debug stream if the level specified in the event is
//	DBG_WARNING or higher.
//
// See Also
//	SetDebugLevel
//

void DebugEvent(DEBUGLEVEL dbgLevel, const char* message)
{
	char _device_name_[9];
	memset(_device_name_, 0, sizeof(_device_name_));
	memcpy(_device_name_, (char*)The_DDB.DDB_Name, 8);

	if (dbgLevel >= _dbgMessageLevel)
		switch (_dbgOutputDevice)
		{
		case DBG_SERIAL:
			dout << _device_name_ << ": " << message << endl;
			break;

		case DBG_MONOCLR:
			dmono.clearScreen();
			// fall thru
		case DBG_MONO:
			dmono << _device_name_ << ": " << message << endl;
			break;
		}
			
	if (dbgLevel >= _dbgBreakLevel)
		DebugBreak();
}

void __cdecl DebugString(char* string)
{
	switch (_dbgOutputDevice)
	{
	case DBG_SERIAL:
		dout << string << endl;
		break;

	case DBG_MONOCLR:
		dmono.clearScreen();
		// fall thru
	case DBG_MONO:
		dmono << string << endl;
		break;
	}
}

// Function
//	SetDebugOutput - specify where DebugEvent sends output
//
// Syntax
//	void SetDebugOutput(DEBUGOUTPUT dd)
//
// Remarks
//	The argument dd is one of 
//	DBG_SERIAL,	 Direct debug output to serial port
//	DBG_MONO,	 Direct debug output to mono screen
//	DBG_MONOCLR	 Mono screen, cleared first
//
// See Also
//	SetDebugOutput
//
void SetDebugOutput(DEBUGOUTPUT dd)
{
	_dbgOutputDevice = dd;
}

// Function
//	AssertFailure - report assertion failure
//
// Syntax
//	VOID AssertFailure(char *string, int line)
//
// Remarks
//	This routine is invoked when the condition specified as the
//	parameter to the Assert macro evaluates to FALSE.
//	
// See Also
//	Assert macro
//
VOID __cdecl AssertFailure(char *string, int line)
{
     dout << "Assertion failure: " << string << "0x" << (DWORD)line << endl;
}

#else	// DEBUG

void __cdecl DebugString(char* string)
{
	LOCK_Out_Debug_String(string);
}

#endif


