// Copyright (c) 1994, Compuware Corporation All Rights Reserved
// 
// vdebug.h - include file for debugging classes
//

#ifndef __vdebug_h_
#define __vdebug_h_

/*
	Debug streams
	-------------

	The class library provides input and output debug streams for
	use in VxDs.  The output stream base class is Vdbostream; the input
	base class is Vdbistream.  

	The class library provides a default output debug stream, named
	dout, and a default input debug stream, named din.  

	The output stream class, Vdbostream, has a virtual member function,
	_output, which can be overridden in derived classes in order to
	send output to devices other than the serial port.  An example of
	such a derived class is Vmonostream, whose _output function writes
	to the monochrome device.

	The class library provides an instance of a Vmonostream, named
	dmono, which is declared in vdebug.cpp.

	Performing I/O to streams
	--------------------------
	
	I/O to debug streams follows the basic C++ stream conventions.
	For example, the code

		DWORD dw = 0x12345678;
		dout << "The value of dw is " << dw << endl;

	produces

		The value of dw is 12345678 

	The string is sent to the debug serial port.  If you want to send
	output to the monochrome device, use "dmono" in place of "dout".

	The following types may be sent to output streams or read from
	input streams:
			
		DWORD 	unsigned long
		WORD  	unsigned short
		BYTE  	unsigned char
		char	signed char
		char*	Null-terminated ascii string

	All unsigned integers are represented in hexadecimal on the 
	output device. All signed character output is represented as
	ascii.

	On input, a carriage return or newline terminates a string.  A
	character other than [0-9a-fA-F] terminates a numeric value.
*/


#ifdef DEBUG		

// Define a string to serve as the "end of line" indicator

#ifndef _BCSTREAM_
#define endl "\n\r"
#endif

// Define the base class for output debug streams. Output to this stream
// goes to the serial port.

class Vdbostream
{
public:
	Vdbostream&  operator << (const char dbch);
	Vdbostream&  operator << (const char* string);
	Vdbostream&  operator << (const DWORD dwordValue);
	Vdbostream&  operator << (const WORD wordValue);
	Vdbostream&  operator << (const BYTE byteValue);
protected:
	virtual VOID _output(const char* s);
};

// Define an output stream that uses the monochrome device

class Vmonostream : public Vdbostream
{
public:
	void clearScreen();
	void setCursorPosition(DWORD row, DWORD col);
	void getCursorPosition(DWORD& row, DWORD& col);
protected:
	virtual void _output(const char* s);
};

// Define the class for input debug streams

class Vdbistream
{
public:
	Vdbistream&  operator >> (char& dbch);
	Vdbistream&  operator >> (char* string);
	Vdbistream&  operator >> (DWORD& dwordValue);
	Vdbistream&  operator >> (WORD& wordValue);
	Vdbistream&  operator >> (BYTE& byteValue);
	DWORD getHex();
};

// The default input and output streams - these are defined in library
// module vdebug.cpp

extern Vdbostream dout;
extern Vmonostream dmono;
extern Vdbistream din;

#endif		// DEBUG

#endif		// __vdebug_h_

