//---------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation 1993-1994
//
// File: init.c
//
//  This file contains the library entry points 
//
// Usage and assumptions used in this DLL.
//
//  1) Message crackers are used.  See windowsx.h and windowsx.txt.
//
//  2) Many functions are considered "member functions" of a 
//     particular class.  Because this is not C++, the function
//     names follow a special naming convention: "Class_Name".
//     In addition, it is common practice that the first 
//     argument for these type of functions is a "this" pointer
//     to the particular object.
//
// History:
//  12-23-93 ScottH     Created
//
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////  INCLUDES

#include "suiprv.h"         // common headers

#include "res.h"

/////////////////////////////////////////////////////  TYPEDEFS

/////////////////////////////////////////////////////  DEFINES

/////////////////////////////////////////////////////  PRIVATE FUNCTIONS


/////////////////////////////////////////////////////  MODULE DATA


/*----------------------------------------------------------
Purpose: Initialize the DLL
Returns: 
Cond:    --
*/
BOOL PRIVATE SUI_Initialize(void)
    {
    return TRUE;
    }


/*----------------------------------------------------------
Purpose: Register window classes per process
Returns: 
Cond:    --
*/
BOOL PRIVATE InitWindowClasses(
    HINSTANCE hinst)
    {
    return TRUE;
    }


/*----------------------------------------------------------
Purpose: Terminate DLL
Returns: --
Cond:    --
*/
BOOL PRIVATE SUI_Terminate(
    HINSTANCE hinst)
    {
    return TRUE;
    }


/*----------------------------------------------------------
Purpose: Unregister window classes per process
Returns: 
Cond:    --
*/
void PRIVATE TermWindowClasses(
    HINSTANCE hinst)
    {
    }


// Per-instance data
//
#pragma data_seg(DATASEG_PERINSTANCE)

HINSTANCE g_hinst = 0;

#ifdef DEBUG

UINT g_uBreakFlags = 0;         // Controls when to int 3
UINT g_uTraceFlags = 0;         // Controls what trace messages are spewed
UINT g_uDumpFlags = 0;          // Controls what structs get dumped

BOOL g_bExclusive=FALSE;

#endif

#pragma data_seg()

// Global data
//
int g_cProcesses = 0;


#ifdef WIN32

CRITICAL_SECTION g_csMdmUI;


/*----------------------------------------------------------
Purpose: Enter a critical section
Returns: --
Cond:    --
*/
void PUBLIC SUI_EnterExclusive(void)
    {
    EnterCriticalSection(&g_csMdmUI);
#ifdef DEBUG
    g_bExclusive = TRUE;
#endif
    }


/*----------------------------------------------------------
Purpose: Leave a critical section
Returns: --
Cond:    --
*/
void PUBLIC SUI_LeaveExclusive(void)
    {
#ifdef DEBUG
    g_bExclusive = FALSE;
#endif
    LeaveCriticalSection(&g_csMdmUI);
    }

#endif  // WIN32


/*----------------------------------------------------------
Purpose: Attach a process to this DLL
Returns: --
Cond:    --
*/
BOOL SUI_ProcessAttach(HINSTANCE hDll)
    {
    BOOL bSuccess = TRUE;

#ifdef WIN32

    // It's okay to use a critical section in Chicago because (unlike
    //  NT) they work across processes.
    //
    if (g_cProcesses == 0)
        {
        InitializeCriticalSection(&g_csMdmUI);
        }

#endif

    ENTEREXCLUSIVE()
        {
        g_hinst = hDll;

#ifdef DEBUG

        // We do this simply to load the debug .ini flags
        //
        ProcessIniFile();

        TRACE_MSG(TF_GENERAL, "Process Attach [%d] (hDll = %lx)", g_cProcesses, hDll);
        DEBUG_BREAK(BF_ONPROCESSATT);

#endif

        if (g_cProcesses++ == 0)
            {
            bSuccess = SUI_Initialize();
            }
        }
    LEAVEEXCLUSIVE()

    if (bSuccess)
        {
        // (Only do this if we succeeded above)
        //
        // Register the window classes again (they are per-process)
        //
        bSuccess = InitWindowClasses(hDll);
        }

    return bSuccess;
    }


/*----------------------------------------------------------
Purpose: Detach a process from the DLL
Returns: --
Cond:    --
*/
BOOL SUI_ProcessDetach(HINSTANCE hDll)
    {
    BOOL bSuccess = TRUE;

    ENTEREXCLUSIVE()
        {
        ASSERT(hDll == g_hinst);

        DEBUG_CODE( TRACE_MSG(TF_GENERAL, "Process Detach [%d] (hDll = %lx)", 
            g_cProcesses-1, hDll); )

        DEBUG_CODE( DEBUG_BREAK(BF_ONPROCESSDET); )

        if (--g_cProcesses == 0)
            {
            bSuccess = SUI_Terminate(g_hinst);
            }
        }
    LEAVEEXCLUSIVE()

#ifdef WIN32

    if (g_cProcesses == 0)
        {
        DeleteCriticalSection(&g_csMdmUI);
        }

#endif

    TermWindowClasses(hDll);

    return bSuccess;
    }


#ifdef WIN32

/*----------------------------------------------------------
Purpose: Win32 Libmain
Returns: --
Cond:    --
*/
BOOL APIENTRY LibMain(
    HANDLE hDll, 
    DWORD dwReason,  
    LPVOID lpReserved)
    {
    switch(dwReason)
        {
    case DLL_PROCESS_ATTACH:
        SUI_ProcessAttach(hDll);
        break;

    case DLL_PROCESS_DETACH:
        SUI_ProcessDetach(hDll);
        break;

    case DLL_THREAD_ATTACH:

#ifdef DEBUG

        ENTEREXCLUSIVE()
            {
            // We do this simply to load the debug .ini flags
            //
            ProcessIniFile();

            TRACE_MSG(TF_GENERAL, "Thread Attach [%d] (hDll = %lx)", 
                      g_cProcesses, hDll);
            }
        LEAVEEXCLUSIVE()

        DEBUG_BREAK(BF_ONTHREADATT);

#endif

        break;

    case DLL_THREAD_DETACH:

#ifdef DEBUG

        ENTEREXCLUSIVE()
            {
            TRACE_MSG(TF_GENERAL, "Thread Detach [%d] (hDll = %lx)", 
                g_cProcesses, hDll);
            }
        LEAVEEXCLUSIVE()

        DEBUG_BREAK(BF_ONTHREADDET);

#endif

        break;

    default:
        break;
        } 
    
    return TRUE;
    } 

#else   // WIN32

// **************************************************************************
// WIN16 specific code
// **************************************************************************


BOOL CALLBACK LibMain(HINSTANCE hinst, UINT wDS, DWORD unused)
    {
    return SUI_ProcessAttach(hinst);
    }

BOOL CALLBACK WEP(BOOL fSystemExit)
    {
    return SUI_ProcessDetach(g_hinst);
    }

#endif  // WIN32




