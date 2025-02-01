/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//              vfilter.h
//
//	Abstract:
//
//	Author:
//		William Ingle
//
//	Environment:
//		Windows 95 filter sample 16-bit netdi install dll
//
//	Notes:
//
//	Future:
//
//	Revision History:
//
/////////////////////////////////////////////////////////////////////////////


#ifndef __VFILTER_H__
#define __VFILTER_H__

#define WINVER 0x030a		// Must be Win 3.1 compatible
#define STRICT

#include <windows.h>
#include <setupx.h>
#include <resource.h>
#include <debug.h>


#define MAX_MBSTRING 	400		// Maximum length of string for message box.
#define MAX_RES			256		// Maximum length of resource string
#define PRIVATE


typedef struct tagCUSTOMPROPS
{
	HGLOBAL		hMem;
	HINF		hInf;
	LONG		suFlags;	

	struct tagCUSTOMPROPS *pOriginalSettings;
	
} FILTERPROPS, *CUSTOMPROPS;


extern HINSTANCE ghInstance;


RETERR CustomInitialize( LPDEVICE_INFO lpdi );


#endif          /* __VFILTER_H__ */
