//---------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation 1993-1994
//
// File: err.c
//
//  This files contains all error handling routines.
//
// History:
//  12-23-93 ScottH     Created
//
//---------------------------------------------------------------------------


/////////////////////////////////////////////////////  INCLUDES

#include "suiprv.h"     // common headers

/////////////////////////////////////////////////////  TYPEDEFS

/////////////////////////////////////////////////////  CONTROLLING DEFINES

/////////////////////////////////////////////////////  DEFINES

/////////////////////////////////////////////////////  MODULE DATA

#ifdef DEBUG

#pragma data_seg(DATASEG_READONLY)

char const FAR c_szNewline[] = "\r\n";
char const FAR c_szTrace[] = "t SERIALUI  ";
char const FAR c_szDbg[] = "SERIALUI  ";
char const FAR c_szAssertFailed[] = "SERIALUI  Assertion failed in %s on line %d\r\n";

struct _RETERRMAP
    {
    RETERR ret;
    LPCSTR psz;
    } const c_rgreterrmap[] = {
        { OK,                                    "OK" },
        { DI_ERROR,                              "DI_ERROR" },
        { ERR_DI_INVALID_DEVICE_ID,              "ERR_DI_INVALID_DEVICE_ID" },
        { ERR_DI_INVALID_COMPATIBLE_DEVICE_LIST, "ERR_DI_INVALID_COMPATIBLE_DEVICE_LIST" },
        { ERR_DI_REG_API,                        "ERR_DI_REG_API" },
        { ERR_DI_LOW_MEM,                        "ERR_DI_LOW_MEM" },
        { ERR_DI_BAD_DEV_INFO,                   "ERR_DI_BAD_DEV_INFO" },
        { ERR_DI_INVALID_CLASS_INSTALLER,        "ERR_DI_INVALID_CLASS_INSTALLER" },
        { ERR_DI_DO_DEFAULT,                     "ERR_DI_DO_DEFAULT" },
        { ERR_DI_USER_CANCEL,                    "ERR_DI_USER_CANCEL" },
        { ERR_DI_NOFILECOPY,                     "ERR_DI_NOFILECOPY" },
        { ERR_DI_BAD_CLASS_INFO,                 "ERR_DI_BAD_CLASS_INFO" },
    };

#pragma data_seg()

#endif

/////////////////////////////////////////////////////  PUBLIC FUNCTIONS


#ifdef DEBUG

/*----------------------------------------------------------
Purpose: Return English reason for the int 3
Returns: String
Cond:    --
*/
LPCSTR PRIVATE GetReasonString(
    UINT flag)      // One of BF_ flags
    {
    LPCSTR psz;

    if (IsFlagSet(flag, BF_ONOPEN))
        psz = "BREAK ON OPEN BRIEFCASE\r\n";

    else if (IsFlagSet(flag, BF_ONCLOSE))
        psz = "BREAK ON CLOSE BRIEFCASE\r\n";

    else if (IsFlagSet(flag, BF_ONRUNONCE))
        psz = "BREAK ON RunDLL_RunOnlyOnce\r\n";

    else if (IsFlagSet(flag, BF_ONVALIDATE))
        psz = "BREAK ON VALIDATION FAILURE\r\n";

    else if (IsFlagSet(flag, BF_ONTHREADATT))
        psz = "BREAK ON THREAD ATTACH\r\n";

    else if (IsFlagSet(flag, BF_ONTHREADDET))
        psz = "BREAK ON THREAD DETACH\r\n";

    else if (IsFlagSet(flag, BF_ONPROCESSATT))
        psz = "BREAK ON PROCESS ATTACH\r\n";

    else if (IsFlagSet(flag, BF_ONPROCESSDET))
        psz = "BREAK ON PROCESS DETACH\r\n";

    else
        psz = c_szNewline;

    return psz;
    }


/*----------------------------------------------------------
Purpose: Perform an int 3 based on the flag
Returns: --
Cond:    --
*/
void PUBLIC DEBUG_BREAK(
    UINT flag)      // One of BF_ flags
    {
    BOOL bBreak;
    LPCSTR psz;

    ENTEREXCLUSIVE()
        {
        bBreak = IsFlagSet(g_uBreakFlags, flag);
        psz = GetReasonString(flag);
        }
    LEAVEEXCLUSIVE()

    if (bBreak)
        {
        TRACE_MSG(TF_ALWAYS, psz);
        _asm  int 3;
        }
    }


void PUBLIC MdmAssertFailed(
    LPCSTR pszFile, 
    int line)
    {
    LPCSTR psz;
    char ach[256];
    UINT uBreakFlags;

    ENTEREXCLUSIVE()
        {
        uBreakFlags = g_uBreakFlags;
        }
    LEAVEEXCLUSIVE()

    // Strip off path info from filename string, if present.
    //
    for (psz = pszFile + lstrlen(pszFile); psz != pszFile; psz=AnsiPrev(pszFile, psz))
        {
#ifdef  DBCS
        if ((AnsiPrev(pszFile, psz) != (psz-2)) && *(psz - 1) == '\\')
#else
        if (*(psz - 1) == '\\')
#endif
            break;
        }
    wsprintf(ach, c_szAssertFailed, psz, line);
    OutputDebugString(ach);
    
    if (IsFlagSet(uBreakFlags, BF_ONVALIDATE))
        _asm  int 3;
    }


void CPUBLIC MdmAssertMsg(
    BOOL f, 
    LPCSTR pszMsg, ...)
    {
    char ach[MAX_PATH+40];    // Largest path plus extra

    if (!f)
        {
        lstrcpy(ach, c_szTrace);
        wvsprintf(&ach[sizeof(c_szTrace)-1], pszMsg, ((LPCSTR FAR*)&pszMsg + 1));
        OutputDebugString(ach);
        OutputDebugString(c_szNewline);
        }
    }


void CPUBLIC MdmDebugMsg(
    UINT uFlag, 
    LPCSTR pszMsg, ...)
    {
    char ach[MAX_PATH+40];    // Largest path plus extra
    UINT uTraceFlags;

    ENTEREXCLUSIVE()
        {
        uTraceFlags = g_uTraceFlags;
        }
    LEAVEEXCLUSIVE()

    if (uFlag == TF_ALWAYS || IsFlagSet(uTraceFlags, uFlag))
        {
        lstrcpy(ach, c_szTrace);
        wvsprintf(&ach[sizeof(c_szTrace)-1], pszMsg, ((LPCSTR FAR*)&pszMsg + 1));
        OutputDebugString(ach);
        OutputDebugString(c_szNewline);
        }
    }

/*----------------------------------------------------------
Purpose: Returns the string form of a RETERR.

Returns: String ptr
Cond:    --
*/
LPCSTR PUBLIC Dbg_GetReterr(
    RETERR ret)
    {
    int i;

    for (i = 0; i < ARRAYSIZE(c_rgreterrmap); i++)
        {
        if (ret == c_rgreterrmap[i].ret)
            return c_rgreterrmap[i].psz;
        }
    return "Unknown RETERR";
    }


/*----------------------------------------------------------
Purpose: Returns a string safe enough to print...and I don't
         mean swear words.

Returns: String ptr
Cond:    --
*/
LPCSTR PUBLIC Dbg_SafeStr(
    LPCSTR psz)
    {
    if (psz)
        return psz;
    else
        return "NULL";
    }

#endif  // DEBUG


/*----------------------------------------------------------
Purpose: Invoke a user error message box.  Default values are
         obtained from vappinfo struct.
Returns: value of MessageBox()
Cond:    --
*/
int PUBLIC MsgBoxSz(
    HWND hwndParent,    // parent window (may be NULL)
    LPCSTR lpsz,        // message
    UINT idsCaption,    // resource ID for caption
    UINT nBoxType,      // message box type (ERR_ERROR, ERR_INFO, ERR_QUESTION)
    HANDLE hres)        // Resource instance handle (may be NULL)
    {
    int nRet = 0;
    
    if (hres == NULL)
        {
        ENTEREXCLUSIVE()
            {
            hres = g_hinst;
            }
        LEAVEEXCLUSIVE()
        }
    
    ASSERT(hres != NULL);
    
    // Load in error description string
    //
    if (lpsz)
        {
        char szCap[MAXBUFLEN];

        UINT nType = MB_OK;
        
        // Load the caption string
        //
        SzFromIDS(idsCaption, szCap, sizeof(szCap));

        // Determine type of message box
        //
        nType |= (nBoxType == MSG_ERROR) ? MB_ICONEXCLAMATION : 0;
        nType |= (nBoxType == MSG_INFO) ? MB_ICONINFORMATION : 0;
        if (nBoxType == MSG_QUESTION)
            nType = MB_ICONQUESTION | MB_YESNO;
        
        nRet = MessageBox(hwndParent, lpsz, szCap, nType);
        }
    
    return nRet;
    }


/*----------------------------------------------------------
Purpose: Invoke a user error message box.  
Returns: value of MessageBox()
Cond:    --
*/
int PUBLIC MsgBoxIds(
    HWND hwndParent,    // parent window (may be NULL)
    UINT ids,           // message resource ID
    UINT idsCaption,    // resource ID for caption
    UINT nBoxType)      // message box type (ERR_ERROR, ERR_INFO, ERR_QUESTION)
    {
    char sz[MAXMSGLEN];
    int nRet = 0;
    HINSTANCE hinst;
    
    ENTEREXCLUSIVE()
        {
        hinst = g_hinst;
        }
    LEAVEEXCLUSIVE()
    
    // Load in error description string
    //
    if (LoadString(hinst, ids, sz, sizeof(sz)) > 0)
        nRet = MsgBoxSz(hwndParent, sz, idsCaption, nBoxType, hinst);
    
    return nRet;
    }

