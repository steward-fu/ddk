//**********************************************************************
//
// SETUPX.H
//
//  Copyright (c) 1993 - Microsoft Corp.
//  All rights reserved.
//
// Public include file for specific Setup and device management
// services.
//
//**********************************************************************

#ifndef SETUP4_INC
#define SETUP4_INC

#if (WINVER < 0x0400)
// Do a warning message here
#endif

#ifndef RC_INVOKED
#pragma warning(disable:4201)       // Non-standard extensions
#pragma warning(disable:4209)       // Non-standard extensions
#pragma warning(disable:4214)       // Non-standard extensions
#endif

#include <prsht.h>
#include <commctrl.h>              // Need this for the following functions.
RETERR WINAPI DiGetClassImageList(HIMAGELIST  FAR *lpMiniIconList);
RETERR WINAPI DiGetClassImageIndex(LPCSTR lpszClass, int FAR *lpiImageIndex);

RETERR WINAPI DiGetClassDevPropertySheets(LPDEVICE_INFO lpdi, LPPROPSHEETHEADER lppsh, WORD wFlags);

// Flags for the DiGetClassDevPropertySheets API
#define DIGCDP_FLAG_BASIC           0x0001
#define DIGCDP_FLAG_ADVANCED        0x0002

#define IDI_CLASSICON_OVERLAYFIRST  500
#define IDI_CLASSICON_OVERLAYLAST   502
#define IDI_PROBLEM_OVL             500
#define IDI_DISABLED_OVL            501
#define IDI_FORCED_OVL              502


#endif  // SETUP4_INC
