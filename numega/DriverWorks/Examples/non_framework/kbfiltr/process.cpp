// process.cpp - STL demonstration for Keyboard Filter sample
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

//////////////////////////////////////////////////////////////////////////////////
// This is a demonstration of STL code in a driver that uses none of the
// classes of DriverWorks. 
//
// Whenever the Escape key is pressed, this module will emit to the debugger
// (or to the output window of DriverWorkbench) a sorted list of all the
// strings that have been typed since boot up. The display will also show
// the number of times that each of the strings has been typed. For purposes
// of this demonstration, strings are considered to be consecutive letters,
// and the driver does not monitor the shift/control/alt states.

#pragma warning (disable:4786)

#undef  _CRTIMP
#define _CRTIMP

#include <vdw.h>
#include <ntddkbd.h>
#include <kstl.h>
#include <algorithm>
#include <map>
#include <string>
#include <new>

using namespace std;

#ifndef USE_CPP_EXCEPTIONS
#ifndef _IA64_
#define USE_CPP_EXCEPTIONS	1
#else
#define USE_CPP_EXCEPTIONS	0
#endif
#endif

typedef unsigned short scan_code;

// scan code for escape key
const char ESCAPE_CHAR = char(0x1b);

// current_string is the string that accumlates the string being typed
string current_string;

// output to debugger (this is a DriverWorks class)
KTrace word_trace;

// a scan_code_map is just a map from scan codes to ascii characters
class scan_code_map : public map<scan_code, char> 
{
public:
	scan_code_map()
	{
		scan_lookup[1] = ESCAPE_CHAR;
		scan_lookup[12] = '_';
		scan_lookup[30] = 'a';
		scan_lookup[48] = 'b';
		scan_lookup[46] = 'c';
		scan_lookup[32] = 'd';
		scan_lookup[18] = 'e';
		scan_lookup[33] = 'f';
		scan_lookup[34] = 'g';
		scan_lookup[35] = 'h';
		scan_lookup[23] = 'i';
		scan_lookup[36] = 'j';
		scan_lookup[37] = 'k';
		scan_lookup[38] = 'l';
		scan_lookup[50] = 'm';
		scan_lookup[49] = 'n';
		scan_lookup[24] = 'o';
		scan_lookup[25] = 'p';
		scan_lookup[16] = 'q';
		scan_lookup[19] = 'r';
		scan_lookup[31] = 's';
		scan_lookup[20] = 't';
		scan_lookup[22] = 'u';
		scan_lookup[47] = 'v';
		scan_lookup[17] = 'w';
		scan_lookup[45] = 'x';
		scan_lookup[21] = 'y';
		scan_lookup[44] = 'z';
	}

} scan_lookup;


// The frequency table associates strings with the number of times each has
// been typed.
map<const string, int> word_freq;

// this object is used to dump the frequency table
struct show_word
{
	void operator()(map<const string, int>::value_type item)
	{
		word_trace 
			<< ULONG(item.second) 
			<< "   "  
			<< item.first.c_str() 
			<< "\n";
	}
};

////////////////////////////////////////////////////////////////////////////////
// process_scan is called by the keyboard filtering code as each key is typed
//
void process_scan(PKEYBOARD_INPUT_DATA first, PKEYBOARD_INPUT_DATA last)
{

#if (USE_CPP_EXCEPTIONS)
	try // prevent throwing on bad_alloc (extremely unlikely)
#else
	_try
#endif
	{
		while (first != last)	// walk over input
		{
			if ( first->Flags == KEY_MAKE )	// if this is a key press
			{
				char ch = scan_lookup[first->MakeCode];	// look up the scan code

				switch (ch)	// branch on scan code
				{
				case char(): // not in talbe, must be a delimiter
					if (!current_string.empty())
					{
						word_freq[current_string]++;
						current_string="";
					}
					break;

				case ESCAPE_CHAR: // escape char - dump table
					for_each(word_freq.begin(), word_freq.end(), show_word() );
					break;

				default: // add character to current_string
					current_string += ch;
					break;
				}
			}

			++first;
		}
	}
#if (USE_CPP_EXCEPTIONS) //C++ exception handling
	catch(bad_alloc Exception)
	{
		word_trace << "caught exception . . ." << Exception.what() << "\n";
	}
	catch(...)
	{
		word_trace << "caught exception . . .\n";
	}
#else //SEH exception handling
	_except(EXCEPTION_EXECUTE_HANDLER)
	{
		word_trace << "caught exception . . .\n";
	}
#endif

}
