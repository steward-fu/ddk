/******************************************************************************

  stivar.h

  Copyright (c) 1997 Microsoft Corporation
  All rights reserved

Notes:
  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

******************************************************************************/

#include "resource.h"

//
// reset listbox window display after this many iterations
//
#define MAX_LOOP 16383

//
// INF defines
//
#define INFBUFSIZ                   0x8000      // assume 32k for largest INF

//
// current test status defines
//
#define STATUS_SUCCESS              0           // SUCCESS
#define STATUS_NEXT                 1           // run next test
#define STATUS_PRIOR                2           // re-run last test
#define STATUS_TERSE                3           // stop writing to log
#define STATUS_VERBOSE              4           // start writing to log
#define STATUS_PAUSE                5           // PAUSE pause
#define STATUS_EXIT                 6           // EXIT end of script exit
#define STATUS_ERROR                7           // can continue error
#define STATUS_FATAL                8           // cannot continue error

//
// current test defines
//
#define TEST_CREATE					1			// open Sti subsystem
#define TEST_ENUM					2			// enumerate all devices
#define TEST_SELECT					3			// select next device
#define TEST_ACQUIRE				4			// scan from device
#define TEST_CLOSE					5			// close device
#define TEST_RELEASE				6			// close Sti subsystem
#define TEST_PAUSE					20			// pause the test

//
// timers
//
#define TIMER_INF                   3001


//
// GLOBAL VARIABLES
//

//
// global window handles
//
HANDLE      hThisInstance;              // current instance
HWND        hThisWindow;                // current window
HMENU       hMenu;                      // current menu

//
// general purpose strings
//
HGLOBAL     hLHand[5];                  // utility string handles
LPSTR       lpzString;                  // utility string
PSTR        pszOut,                     // TextOut string
            pszMsg,                     // generic NEAR string 1
            pszString,                  // generic NEAR string 2
            pszCommand,                 // current INF command
            pszComment;                 // any comments

//
// global test settings
//
int         nAuto       = 0,            // 1 = run 60 second timed test
            nError      = 0,            // number of errors
            nEvent      = 0,            // 1 = StiEvent, 2 = StiDevice
            nFatal      = 0,            // can't continue after this...
            nGo         = 0,            // 1 = nonstop timed test
            nICanScan   = 0,            // Stillvue can / can't scan this device
            nLastLine   = 1,            // last line number in script
            nMaxScan    = 0,            // maximum number of scans (0 = infinite)
            nNextLine   = 1,            // next line of inf to run
            nNextTest   = TEST_CREATE,  // next test to run
			nPause      = 0,			// toggle for run (0) pause test (! 0)
            nRunInf     = 0,            // 0 = no INF, 1 = INF is loaded
            nRadix      = 10,           // base is decimal (or hex)
            nSaveLog    = 0,            // always write out log
            nScanCount  = 0,            // number of scans run so far
            nScriptLine = 1,            // next script line to parse
            nStatus     = STATUS_NEXT,  // state of the tests
            nStress     = 0,            // 1 = running the stress test
            nTestCount  = 0,            // number of tests run so far
            nTimeState  = 0,            // 0 timer is off, 1 timer is on
            nTimeNext   = 10,           // wait time between timer in seconds
            nTimeScan   = 60,           // wait before next scan in nTimeNext units
            nTTMax      = 0,            // temp var
            nTTNext     = 0,            // temp var
            nTTScan     = 0,            // temp var
            nWHQL       = 0;            // 1 = WHQL STI compliance test
DWORD       dwLastError = 0;            // last GetLastError found

//
// defined in wsti.cpp
//
extern      DWORD dwStiTotal;
extern      PSTI_DEVICE_INFORMATION pStiInfoPtr;

//
// text display
//
HWND        hLogWindow;
ULONG       ulCount1,ulCount2;
int         cxChar,cxCaps,cyChar,cxClient,cyClient,iMaxWidth,
            iHscrollPos,iHscrollMax,
            iVscrollPos,iVscrollMax;

//
// inf, logfile handles
//
HANDLE      hLog = NULL;                // output log file handle
char        szInfName[LONGSTRING] = "", // input INF file name
            szLogName[LONGSTRING] = ""; // output log file name
LPSTR       lpInf = NULL,               // buffer for INF commands
            lpLine;

