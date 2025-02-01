/******************************************************************************

  stillvue.h

  Copyright (c) 1997 Microsoft Corporation
  All rights reserved

Notes:
  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

******************************************************************************/

#pragma warning (disable:4001)          // ignore '//' comments

#define     _X86_   1
#define     WIN32_LEAN_AND_MEAN 1

#include    <windows.h>
#include    <stdio.h>
#include    <stdlib.h>                  // rand()
#include    <string.h>                  // strcat
#include    <time.h>                    // srand(time())

#include    <sti.h>                     // Still Image services

#include    "resource.h"                // resource defines

#include    "winx.h"


/*****************************************************************************

	global defines

*****************************************************************************/

#define LONGSTRING                  256
#define MEDSTRING                   128
#define SHORTSTRING                 32

#define OFF							0
#define ON							1
#define PEEK						2


/*****************************************************************************

	events

*****************************************************************************/

#define	STIEVENTARG					"StiEvent"
#define	STIDEVARG					"StiDevice"


/*****************************************************************************

    StiSelect contexts

*****************************************************************************/

#define	AUTO		1
#define	EVENT		4
#define	MANUAL		8


/*****************************************************************************

    External variables

*****************************************************************************/

extern  HMENU       hMenu;                     // current menu
extern  PSTR        pszMsg;                    // generic NEAR string 1


/*****************************************************************************

    stillvue.cpp prototypes

*****************************************************************************/

VOID    DisplayOutput(LPSTR,...);
int     EndTest(HWND,int);
BOOL    FirstInstance(HANDLE);
HWND    MakeWindow(HANDLE);
int     ParseCmdLine(LPSTR);

BOOL    CommandParse(HWND,UINT,WPARAM,LPARAM);
BOOL    Creation(HWND,UINT,WPARAM,LPARAM);
BOOL    Destruction(HWND,UINT,WPARAM,LPARAM);
BOOL    HScroll(HWND,UINT,WPARAM,LPARAM);
BOOL    VScroll(HWND,UINT,WPARAM,LPARAM);

long FAR PASCAL WiskProc(HWND,UINT,WPARAM,LPARAM);


/*****************************************************************************

    wsti.cpp prototypes

*****************************************************************************/

void	StiDisplayError(HRESULT,char *);
int     StiSelect(HWND,int);

HRESULT StiGetDeviceValue(LPWSTR,LPWSTR,DWORD *);
HRESULT StiGetDeviceInfo(LPWSTR);
HRESULT	StiGetStatus(int);
HRESULT	StiGetCaps();
HRESULT	StiReset();
HRESULT	StiDiagnostic();
HRESULT StiEnableHwNotification(LPWSTR,int *);
HRESULT	StiGetLastErrorInfo();
HRESULT StiClose();
HRESULT StiEnum();
HRESULT StiEvent(HWND);
HRESULT StiRegister(HWND,HINSTANCE,int);
HRESULT StiCreateInstance();
HRESULT StiImageRelease();
HRESULT StiDeviceRelease();
HRESULT StiWriteErrLog(DWORD,LPCWSTR);


BOOL FAR PASCAL     SelectDevice(HWND,UINT,WPARAM,LPARAM);
BOOL FAR PASCAL     Settings(HWND,UINT,WPARAM,LPARAM);


/*****************************************************************************

    acquire.cpp prototypes

*****************************************************************************/

int		IsScanDevice(PSTI_DEVICE_INFORMATION);
void    StiLamp(int);
INT     StiScan(HWND);

INT     CreateScanDIB(HWND);
INT		DeleteScanDIB();
INT     DisplayScanDIB(HWND);

HRESULT WINAPI      SendDeviceCommandString(PSTIDEVICE,LPSTR,...);
HRESULT WINAPI      TransactDevice(PSTIDEVICE,LPSTR,UINT,LPSTR,...);


/*****************************************************************************

    winx.cpp prototypes

*****************************************************************************/

BOOL GetFinalWindow (HANDLE hInst,LPRECT lprRect,LPSTR  lpzINI,LPSTR  lpzSection);
BOOL SaveFinalWindow (HANDLE hInst,HWND hWnd,LPSTR lpzINI,LPSTR lpzSection);
BOOL LastError(BOOL bNewOnly);

BOOL ErrorMsg(HWND hWnd, LPSTR lpzMsg, LPSTR lpzCaption, BOOL bFatal);
int  NextToken(char *pDest,char *pSrc);

