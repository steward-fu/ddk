/****************************************************************************
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *   PURPOSE.
 *
 *   Copyright (c) 1998 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       digijoy.h
 *  Content:    Common definitions for digijoy.vxd
 *
 ***************************************************************************/
#ifndef __DIGIJOY_INCLUDED__
#define __DIGIJOY_INCLUDED__

#include <vjoyd.h>

/*
 *  The TEXT macro is used to make sure strings are of the right char type
 *  if we failed to pick it up define it here.
 */
#ifndef TEXT
#ifdef UNICODE
#define TEXT(quote) L##quote
#else
#define TEXT(quote) quote
#endif /* UNICODE */
#endif /* TEXT */

/*
 *  If we failed to pick up UNREFERENCED_PARAMETER, define it
 */
#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (P)
#endif

/*
 * Internal polling flags
 */
#define SW_POLL    0
#define RTC_POLL   2
#define TSC_POLL   4

#define PF_INTMASK  1

#define DSW_POLL    SW_POLL 
#define ESW_POLL    ( SW_POLL | PF_INTMASK )
#define DRTC_POLL   RTC_POLL 
#define ERTC_POLL   ( RTC_POLL | PF_INTMASK )
#define DTSC_POLL   TSC_POLL
#define ETSC_POLL   ( TSC_POLL | PF_INTMASK )

/*
 * IBM standard joystick ports (NEC are different)
 */
#define MIN_JOY_PORT        0x200
#define MAX_JOY_PORT        0x20F

/*
 * joystick port bits
 */

#define UNUSED ( (DWORD)-1 )

/****************************************************************************
 *
 *  The DEVNODEDESC structure is to store all informations relating to a
 *  single gameport.
 *
 *  @field DWORD | id |
 *
 *  The id assigned to the gameport or UNUSED.
 *
 *  @field DWORD | dwPollMask |
 *
 *  The bitmask to reflect values returned by a successful poll.
 *
 *  @field DWORD | io |
 *
 *  i/o port asocated with this gameport.
 *
 *  @field DWORD | dn |
 *
 *  Devnode asocated with this gameport.  Note since dual gameport gamecards
 *  have multiple i/o resources on a single devnode, there may be multiple
 *  instances of this structure with the same devnode.
 *
 *  @field DWORD | rgdwLastValues[8] |
 *
 *  Array of previous values.
 *
 *  @field DWORD | dwGlitches |
 *
 *  Bitmask of axis polls hit by glitches in reading the RTC timer.
 *
 *  @field PCHAR  | pszDeviceId |
 *
 *  Pointer to string containing the device id for this devnode.  This is used 
 *  to determine if a new devnode is a replacement for an old one or a brand 
 *  new one.
 *
 *  @field WORD | rgwBtnMap[16] |
 *
 *  Array of mappings from button combos to button pressed.
 *
 *  @field WORD | rgwPovMap[16][4] |
 *
 *  Array of mappings from button combos to POV angle, per POV.
 *
 *
 ****************************************************************************/
typedef struct
{
    DWORD   id;
    DWORD   dwPollMask;
    DWORD   io;
    DEVNODE dn;
    DWORD   rgdwLastValues[8];
    DWORD   dwGlitches;
    PCHAR   pszDeviceId;
    WORD    rgwBtnMap[16];
    WORD    rgwPOVMap[16][4];
} DEVNODEDESC, *PDEVNODEDESC;

/*
 * these are found in main.c
 */
extern DWORD dwPollType;
extern void (__cdecl *fpPoll)( void );
extern void (__stdcall *NewTimeout)( DWORD );
extern int NumDevnodes;
extern int MaxDevnodes;
extern PDEVNODEDESC    pDevnodes;

/**************************************************************************
 *  
 *  Diagnostic output declarations
 *  These declarations allow a set of diagnostic output routines to be 
 *  optionally defined depending on the type of build.  If the build is for 
 *  retail, these definitions will make sure that any diagnostic ouput will 
 *  be compiled to nothing otherwise the functions defined in debug.c will 
 *  be used.  To reduce the length of debug statements in the code six 
 *  variations of the diagnostic output are declared so that the severity 
 *  of the error reported is implied by which of the functions is used.  Each 
 *  variant causes a line of diagnostic to be output starting with a module 
 *  name and ending with "\r\n"; the string in between being the taken from 
 *  the passed parameters in the form of an Debug_Printf_Service call.
 *  
 *  The six variants are:
 *      FPF - Fatal PrintF
 *      MPF - Memory allocation failure PrintF
 *      EPF - Error PrintF
 *      WPF - Warning PrintF
 *      IPF - Information PrintF
 *      ZPF - diagnostic PrintF
 *  
 **************************************************************************/
#define QUIET_LEVEL         0
#define FATAL_LEVEL         1
#define ERROR_LEVEL         2
#define WARNING_LEVEL       3 
#define INFOMATION_LEVEL    4
#define DIAGNOSTIC_LEVEL    5

#define OUT_OF_MEMORY_LEVEL FATAL_LEVEL
#if defined DEBUG || defined DEBUG_RETAIL

/*
 *  found in debug.c
 */
void _cdecl FPF( PCHAR, ... );
void _cdecl MPF( PCHAR, ... );
void _cdecl EPF( PCHAR, ... );
void _cdecl WPF( PCHAR, ... );
void _cdecl IPF( PCHAR, ... );
void _cdecl ZPF( PCHAR, ... );
#else
/*
 * or turned into sizeof( PCHAR, <parm list> ) 
 * relying on the compiler to remove resultant isolated R-value
 */
#define FPF sizeof
#define MPF sizeof
#define EPF sizeof
#define WPF sizeof
#define IPF sizeof
#define ZPF sizeof

#endif

#ifdef RETAIL
#define INLINE_RETAIL __inline
#else
#define INLINE_RETAIL
#endif

/*
 * A bunch of macros to give some heap diagnostics
 */
#ifndef RETAIL
PVOID __cdecl DbgMalloc( ULONG, ULONG );
PVOID __cdecl DbgReAlloc( PVOID, ULONG, ULONG );
ULONG __cdecl DbgFree( PVOID, ULONG );
#define Malloc( Size, Flags ) DbgMalloc( Size, Flags )
#define Free( Ptr ) DbgFree( Ptr, 0 )
#define ReAlloc( Ptr, Size, Flags ) DbgReAlloc( Ptr, Size, Flags )
#else
#define Malloc( Size, Flags ) _HeapAllocate( Size, Flags )
#define Free( Ptr ) _HeapFree( Ptr, 0 )
#define ReAlloc( Ptr, Size, Flags ) _HeapReAllocate( Ptr, Size, Flags )
#endif

/*
 * found in the poll.asm
 */
extern HRESULT __stdcall jsPoll( DWORD dwMask, DWORD dwPort, PDWORD pdwButtons, PDWORD pdwAxes, DWORD dwTimeout ); // Set to NULL during init
extern void __cdecl DSWPoll( void );
extern void __cdecl ESWPoll( void );
extern void __cdecl DRTCPoll( void );
extern void __cdecl ERTCPoll( void );
extern void __cdecl DTSCPoll( void );
extern void __cdecl ETSCPoll( void );

/*
 * found in timing.asm
 */
extern DWORD dwReadLimit;
extern DWORD dwIntLimit;
extern int __stdcall IsTSCPresent( void );
extern int __stdcall InitTSC( DWORD port );
extern int __stdcall InitRTC( DWORD port );
extern int __stdcall FiniRTC( void );
extern int __stdcall InitSW( void );
extern void __stdcall CalcTSCTickTimeOut( DWORD );
extern void __stdcall CalcRTCTickTimeOut( DWORD );
extern void __stdcall CalcSWTickTimeOut( DWORD );

/*
 * found in digijoy.asm
 */
extern DWORD __stdcall VJoyDRegister( DWORD );

#endif
