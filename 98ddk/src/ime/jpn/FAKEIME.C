/**********************************************************************/
/*                                                                    */
/*      FAKEIME.C - Windows 95 FAKEIME                                */
/*                                                                    */
/*      Copyright (c) 1994-1995  Microsoft Corporation                */
/*                                                                    */
/**********************************************************************/

#include <windows.h>
#include "imm.h"
#include "fakeime.h"
#include "resource.h"


/**********************************************************************/
/*    DLLEntry()                                                      */
/**********************************************************************/
BOOL WINAPI DLLEntry (
    HINSTANCE    hInstDLL,
    DWORD        dwFunction,
    LPVOID       lpNot)
{
    LPSTR lpDicFileName;
#ifdef DEBUG
    char szDev[80];
#endif
	DebugPrint(TEXT("DLLEntry:dwFunc=%d\n"),dwFunction);

    switch(dwFunction)
    {
        case DLL_PROCESS_ATTACH:
            hInst= hInstDLL;
            IMERegisterClass( hInst );

            // Initialize for FAKEIME.
            lpDicFileName = (LPSTR)&szDicFileName;
            lpDicFileName += GetWindowsDirectory(lpDicFileName,256);
            if (*(lpDicFileName-1) != '\\')
                *lpDicFileName++ = '\\';
            LoadString( hInst, IDS_DICFILENAME, lpDicFileName, 128);

            SetGlobalFlags();

#ifdef DEBUG
            wsprintf(szDev,"DLLEntry Process Attach hInst is %lx",hInst);
            ImeLog(LOGF_ENTRY, szDev);
#endif
            break;

        case DLL_PROCESS_DETACH:
            UnregisterClass(szUIClassName,hInst);
            UnregisterClass(szCompStrClassName,hInst);
            UnregisterClass(szCandClassName,hInst);
            UnregisterClass(szStatusClassName,hInst);
#ifdef DEBUG
            wsprintf(szDev,"DLLEntry Process Detach hInst is %lx",hInst);
            ImeLog(LOGF_ENTRY, szDev);
#endif
            break;

        case DLL_THREAD_ATTACH:
#ifdef DEBUG
            wsprintf(szDev,"DLLEntry Thread Attach hInst is %lx",hInst);
            ImeLog(LOGF_ENTRY, szDev);
#endif
            break;

        case DLL_THREAD_DETACH:
#ifdef DEBUG
            wsprintf(szDev,"DLLEntry Thread Detach hInst is %lx",hInst);
            ImeLog(LOGF_ENTRY, szDev);
#endif
            break;
    }
    return TRUE;
}
