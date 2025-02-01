/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1998 Microsoft Corporation.  All Rights Reserved.
 * 
 *  File:       debug.c
 *  Content:    Debugging code, and Kernel mode debug interface
 **************************************************************************/

/*
 *  We only need this file in a checked build so this pragma turns off 
 *  the warning that there's nothing to compile
 */
#pragma warning( disable:4206 )
#if defined DEBUG || defined DEBUG_RETAIL

#ifdef DEBUG_RETAIL
#define DEBUG
#endif

/*
 *  configmg.h will be included but fail without these macros being defined
 */
#define CAT_HELPER(x, y)    x##y
#define CAT(x, y)       CAT_HELPER(x, y)
#define MAKE_HEADER(RetType, DecType, Function, Parameters)

#include <basedef.h>
#include <vmm.h>
#include <debug.h>

#ifdef DEBUG_RETAIL
#undef DEBUG
#endif

#include "digijoy.h"
#include "wraps.h"

#pragma VxD_DEBUG_ONLY_CODE_SEG
#pragma VxD_DEBUG_ONLY_DATA_SEG

#if defined DEBUG_RETAIL
DWORD DebugLevel = WARNING_LEVEL;
DWORD BreakLevel = QUIET_LEVEL;
#else
DWORD DebugLevel = DIAGNOSTIC_LEVEL;
DWORD BreakLevel = FATAL_LEVEL;
#endif

char szHeader[] = "DIGIJOY: ";
char szOoMHeader[] = "DIGIJOY Out of Memory: ";
char szEOL[] = "\r\n";

/*****************************************************************************
 *
 *  PVOID | DbgMalloc |
 *
 *          Non RETAIL Heap Allocation routine; keeps count of total number of
 *          allocations and prints diagnostics.
 *          Takes same parameters and returns same as _HeapAllocate.
 *
 *  @parm   ULONG | Size |
 *
 *          Number of bytes to allocate
 *
 *  @parm   ULONG | Flags |
 *
 *          Allocation flags
 *
 *  @returns
 *
 *          Pointer to allocated heap memory or NULL if out of memory.
 *
 *****************************************************************************/
PVOID __cdecl DbgMalloc( ULONG Size, ULONG Flags )
{
    PVOID pMem;
    pMem = _HeapAllocate( Size, Flags );
    IPF( "%08x bytes allocated at %08x", Size, pMem );
    return pMem;
}

/*****************************************************************************
 *
 *  PVOID | DbgReAlloc |
 *
 *          Non RETAIL Heap Re-allocation routine; keeps count of total number 
 *          of allocations and prints diagnostics.
 *          Takes same parameters and returns same as _HeapReAllocate.
 *
 *  @parm   PVOID | Ptr |
 *
 *          Pointer to buffer to be resized
 *
 *  @parm   ULONG | Size |
 *
 *          Number of bytes required in resized buffer
 *
 *  @parm   ULONG | Flags |
 *
 *          Allocation flags
 *
 *  @returns
 *
 *          Pointer to allocated heap memory or NULL if out of memory.
 *
 *****************************************************************************/
PVOID __cdecl DbgReAlloc( PVOID Ptr, ULONG Size, ULONG Flags )
{
    PVOID pMem;
    pMem = _HeapReAllocate( Ptr, Size, Flags );
    IPF( "%08x bytes reallocated from %08x to %08x ", Size, Ptr, pMem );
    return pMem;
}

/*****************************************************************************
 *
 *  PVOID | DbgFree |
 *
 *          Non RETAIL Heap Free routine; keeps count of total number 
 *          of allocations and prints diagnostics.
 *          Takes same parameters and returns same as _HeapFree.
 *
 *  @parm   PVOID | pMem |
 *
 *          Pointer to buffer to be freed
 *
 *  @parm   ULONG | Flags |
 *
 *          Allocation flags
 *
 *  @returns
 *
 *          0 if free failed.
 *
 *****************************************************************************/
ULONG __cdecl DbgFree( PVOID pMem, ULONG Flags )
{
    ULONG rc;
    ULONG Size;
    Size = _HeapGetSize( pMem, 0 );
    rc = _HeapFree( pMem, Flags );
    IPF( "%08x bytes freed from %08x", Size, pMem );
    if( rc == 0 )
    {
        EPF( "_HeapFree failed" );
    }
    return rc;
}

/*
 *  Macro which is used as core of the formatted diagnostic output functions.
 *  The passed message severity level is compared against the global message
 *  display filter level, DebugLevel and if the error is severe enough the
 *  passed message is output, between the header string and a line termin-
 *  ation.  The length of the passed format string is taken to try to avoid 
 *  page faults in the _Debug_Printf_Service where they cannot be handled 
 *  normally.  The call to the _Debug_Printf_Service is coded with _emit
 *  statements due to header conflicts.
 *  The passed message severity level is then compared against the global 
 *  break level, BreakLevel, and if the error is severe enough a debug 
 *  interrupt (int 3) is executed.  Note the break point will be in this 
 *  debug code, so it will be necessary to step out of this code to see the 
 *  calling function.
 */
#define REPORT_PROBLEM( MsgLevel, Header ) \
{ \
    if( MsgLevel <= DebugLevel ) \
    { \
        Debug_Printf( Header ); \
        _lstrlen( pMsg ); \
        __asm lea  eax,(pMsg + 4) \
        __asm push eax \
        __asm push pMsg \
 \
        __asm _emit 0xcd \
        __asm _emit 0x20 \
        __asm _emit (GetVxDServiceOrdinal(_Debug_Printf_Service) & 0xff) \
        __asm _emit (GetVxDServiceOrdinal(_Debug_Printf_Service) >> 8) & 0xff \
        __asm _emit (GetVxDServiceOrdinal(_Debug_Printf_Service) >> 16) & 0xff \
        __asm _emit (GetVxDServiceOrdinal(_Debug_Printf_Service) >> 24) & 0xff \
 \
        __asm { add esp, 2*4 } \
        Debug_Printf( szEOL ); \
    } \
 \
    if( MsgLevel <= BreakLevel ) \
    { \
        __asm int 3 \
    } \
}

/*
 *  FPF FatalPrintf, print a message before the world ends
 */
void _cdecl FPF( PCHAR pMsg, ... ) 
{
    REPORT_PROBLEM( FATAL_LEVEL, szHeader )
}

/*
 *  MPF MemoryPrintf, special case of FPF which adds "Out of Memory " 
 *      into the message
 */
void _cdecl MPF( PCHAR pMsg, ... ) 
{
    REPORT_PROBLEM( OUT_OF_MEMORY_LEVEL, szOoMHeader )
}

/*
 *  EPF ErrorPrintf, something bad has happened but we can carry on
 */
void _cdecl EPF( PCHAR pMsg, ... ) 
{
    REPORT_PROBLEM( ERROR_LEVEL, szHeader )
}

/*
 *  WPF WarningPrintf, this may not be a problem, but probably is
 */
void _cdecl WPF( PCHAR pMsg, ... ) 
{
    REPORT_PROBLEM( WARNING_LEVEL, szHeader )
}

/*
 *  IPF InformationPrintf, nothing bad, just marking a significant event
 */
void _cdecl IPF( PCHAR pMsg, ... ) 
{
    REPORT_PROBLEM( INFOMATION_LEVEL, szHeader )
}

/*
 *  ZPF DiagnosticPrintf, anything that might show what's happening
 */
void _cdecl ZPF( PCHAR pMsg, ... ) 
{
    REPORT_PROBLEM( DIAGNOSTIC_LEVEL, szHeader )
}

/*
 *  DPF DebugPrintf, print always without header or CR/LF
 *  This is used exclusively by the following DebugQuery code.
 */
void _cdecl DPF( PCHAR pMsg, ... ) 
{
    __asm lea  eax,(pMsg + 4)
    __asm push eax
    __asm push pMsg

    __asm _emit 0xcd
    __asm _emit 0x20
    __asm _emit (GetVxDServiceOrdinal(_Debug_Printf_Service) & 0xff)
    __asm _emit (GetVxDServiceOrdinal(_Debug_Printf_Service) >> 8) & 0xff
    __asm _emit (GetVxDServiceOrdinal(_Debug_Printf_Service) >> 16) & 0xff
    __asm _emit (GetVxDServiceOrdinal(_Debug_Printf_Service) >> 24) & 0xff

    __asm { add esp, 2*4 }
}

/*
 *  Output a single character to the debug terminal
 *  This is used exclusively by the following DebugQuery code.
 */
void VXDINLINE Out_Debug_Chr( char cOut )
{
    __asm mov al, cOut
    VMMCall( Out_Debug_Chr )
}

/*
 *  Read a single character from the debug terminal
 *  This is used exclusively by the following DebugQuery code.
 */
__inline char GetDebugInput( void )
{
    char cIn;

    VMMCall( In_Debug_Chr );
    __asm jnz ExitGetDebugInput
    __asm xor al,al
ExitGetDebugInput:
    __asm mov cIn,al;
    return( cIn );
}

/*
 * Handle the DEBUG_QUERY message sent by the debugger.  
 * Display a menu to the debugger output and respond to options requested.
 */
void __stdcall DIGIJOY_DebugQuery( void )
{
    char    cIn = '?';
    int     Idx;

    DPF( "DigiJoy - Debug Services\r\n\n" );
    DPF( "Current debug message level: %d  current break level: %d\r\n\n", DebugLevel, BreakLevel );
    DPF( "[1] Display currently handled devnodes\r\n" );
    DPF( "[2] Display polling setting\r\n" );
    DPF( "[4] Change debug message level\r\n" );
    DPF( "[5] Change break message level\r\n" );
    DPF( "Enter selection or [ESC] to quit\r\n" );

    while( cIn != 0 )
    {
        switch( cIn )
        {
        case '1':
            if( NumDevnodes )
            {
                DPF( "%d devnodes are handled\r\n", NumDevnodes );
                DPF( "    Devnode   IO addr   ID    Mask\r\n" );
                for( Idx=0; Idx<NumDevnodes; Idx++ )
                {
                    DPF( "    %08x  %4x      %2d    %08x\r\n", 
                        pDevnodes[Idx].dn, pDevnodes[Idx].io, 
                        pDevnodes[Idx].id, pDevnodes[Idx].dwPollMask );
                }
            }
            else
            {
                DPF( "No devnodes are handled\r\n" );
            }
            return;
        case '2':
            switch( dwPollType )
            {
            case DSW_POLL:
                DPF( "Interrupt disabled software loop count polling\r\n" );
                break;

            case ESW_POLL:
                DPF( "Interrupt enabled software loop count polling\r\n" );
                break;

            case DRTC_POLL:
                DPF( "Interrupt disabled RealTime Clock polling\r\n" );
                break;

            case ERTC_POLL:
                DPF( "Interrupt enabled RealTime Clock polling\r\n" );
                break;
        
            case DTSC_POLL:
                DPF( "Interrupt disabled Time Stamp Counter polling\r\n" );
                break;

            case ETSC_POLL:
                DPF( "Interrupt enabled Time Stamp Counter polling\r\n" );
                break;

            default:
                DPF( "No polling is set up\r\n" );
                break;
            }
            return;

        case '3':
            DPF( "Not implemented\r\n" );
            return;

        case '4':
            DPF( "Enter new debug level 0 (nothing) to 5 (everything)\r\n" );
            cIn = '?';
            while( cIn != 0 )
            {
                cIn = GetDebugInput();
                if( ( cIn >= '0' ) && ( cIn <= '5' ) )
                {
                    DebugLevel = cIn - '0';
                    DPF( "New debug = %d\r\n\n", DebugLevel );
                    break;
                }
            }
            return;

        case '5':
            DPF( "Enter new break level 0 (no breaks) to 5 (silly)\r\n" );
            cIn = '?';
            while( cIn != 0 )
            {
                cIn = GetDebugInput();
                if( ( cIn >= '0' ) && ( cIn <= '5' ) )
                {
                    BreakLevel = cIn - '0';
                    DPF( "New break = %d\r\n\n", BreakLevel );
                    break;
                }
            }
            return;
        }

        cIn = GetDebugInput();
    }
}

#endif /* defined DEBUG || defined DEBUG_RETAIL */
