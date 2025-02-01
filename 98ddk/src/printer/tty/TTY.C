// TTY.C
// TTY minidriver callback & property sheet functions.
//
// TTY devices do not print graphics. Therefore, we add 'ttystubs.c' file to
// handle all graphics DDI calls directly (mostly, return 0) instead of
// calling into Unidrv. These calls include:
//  BitBlt, Pixel, Output, RealizeObject (except for the case of OBJ_FONT and
//  -OBJ_FONT), DeviceBitmapBits, SetDIBitsToDevice, StretchDIB, EnumObj.
//
// The DDI functions that require Unidrv's assistance include:
//  Enable, Disable, Control, EnumDFonts, GetCharWidth, ExtTextOut, ColorInfo.
//

#pragma code_seg("_CODESEG")

#define TTY_DI 2 // Two TTY dialog sheet at this time.
#define MAX_DI 8 // Unidrv's 4 + TTY's (2) + 2 (Room for some more)


#include "tty.h"
#include "unidrv.h"


typedef struct
{
    WORD   wCount;
    HPROPSHEETPAGE rglpPropPage[MAX_DI];
}TTYPSP, FAR *LPTTYPSP;


// Stuff to pull in functions explicitly -- use the names, instead of the
// ordinals for now, since the ordinals will probably change before ship.

#define PROPERTYSHEETFUNCTION            "PropertySheet"
#define CREATEPROPERTYSHEETPAGEFUNCTION  "CreatePropertySheetPage"

typedef HPROPSHEETPAGE (WINAPI* CREATEPROC)(LPPROPSHEETPAGE);
typedef int (WINAPI* PROPSHEETPROC)(LPCPROPSHEETHEADER);

LPCSTR  TTYTemplates[TTY_DI] = { MAKEINTRESOURCE(TTY_DEVICEOPTIONS_PROPSHEET),
    MAKEINTRESOURCE(TTY_FONTOPTIONS_PROPSHEET)};
DLGPROC TTYDlgProcs[TTY_DI]  = { (DLGPROC)TTYDeviceOptionsSheet,
    (DLGPROC)TTYFontOptionsSheet};

HANDLE hTTYInst = NULL;
HANDLE WINAPI GetStockObject(int);
char *rgchModuleName = "TTY";
char szName[]="PEdit";
BOOL bHasDeviceData = FALSE;
TTYDevice Device;


//--------------------------**fnOEMGetFontCmd*------------------------------//
// Function:    fnOEMGetFontCMd
//
// Action:      If fSelect is FALSE, we construct a font unselection string
//              which in the case of TTY is 10cpi.
//              If fSelect is TRUE, we construct a font selection string by
//              examining the bCmdCbId value for the font. bCmdCbId for a font
//              is passed by Unidrv and obtained from TTY's PFM data.
//              If the string does not fit in lpBuf then we set lpwSize to the
//              actual size of the string and return FALSE.
//              If the string does fit then we copy it to lpBuf, set lpwSize
//              to the actual size of the string and return TRUE.
//
// Notes:       wCmdCbId values for the TTY fonts are as follows:
//              2 = 10cpi, 3 = 12cpi, 4 = 17cpi, 5 = 5cpi, 6 = 6cpi, 7 = 8cpi
//              255 = Font Unselect (Only for 5, 6 and 8 CPI)


short CALLBACK fnOEMGetFontCmd(
                              LPDV lpdv,         // Pointer to PDEVICE structure
                              WORD wCmdCbId,     // Command callback id#, defined by minidriver
                              LPFONT lpfont,     // Pointer to PFM data
                              BOOL fSelect,      // TRUE for selection
                              LPBYTE lpBuf,      // Buffer to put command into
                              LPWORD lpwSize)    // Ptr to actual size of buffer
{
    BYTE tBuff[FONTESCLEN*2];
    LPBYTE    lpCmdStr;
    BOOL ret;

    if (!bHasDeviceData)
    {
        LoadTTYDataFile();
        GetDevice(GetCurDevice());
        bHasDeviceData = TRUE;
    }

    if (lpdv->fMdv == FALSE)
        if (!bMiniEnable(lpdv))
            return (0);

    if (!fSelect)   // Return Font Unselect
    {
        lstrcpy((LPBYTE)tBuff, (LPBYTE)(&Device.DeviceData.DoubleOff));
        return (bSelectString((LPBYTE)tBuff,lpBuf, lpwSize));
    }

    switch (wCmdCbId)
    {
        case CMDID_12CPI:
            ret = bSelectString((LPBYTE)(&Device.DeviceData.f12Pitch),
                                lpBuf, lpwSize);

            //for compatibility with 3.1 tty , if a 12cpi command exists
            //it must be our default for xmove in spaces
            if (*lpwSize > 0)
            {
                lstrcpy((LPBYTE)lpdv->lpMdv->XMCmd, lpBuf);
                lpdv->lpMdv->wXMCmdLen = *lpwSize;
                lpdv->lpMdv->wXDPI = 12;
            }
            return (ret);

        case CMDID_10CPI:

            ret = bSelectString((LPBYTE)(&Device.DeviceData.f10Pitch),
                                lpBuf, lpwSize);
            //Use 10cpi if no 12cpi font has been found yet.
            if (*lpwSize > 0 && lpdv->lpMdv->wXDPI != 12)
            {

                lstrcpy((LPBYTE)lpdv->lpMdv->XMCmd, lpBuf);
                lpdv->lpMdv->wXMCmdLen = *lpwSize;
                lpdv->lpMdv->wXDPI = 10;
            }
            return (ret);

        case CMDID_17CPI:
            ret  = bSelectString((LPBYTE)(&Device.DeviceData.f17Pitch),
                                 lpBuf, lpwSize);
            //Use 17dpi for movement if no other command exists
            if (*lpwSize > 0 && lpdv->lpMdv->wXDPI == 0)
            {
                lstrcpy((LPBYTE)lpdv->lpMdv->XMCmd, lpBuf);
                lpdv->lpMdv->wXMCmdLen = *lpwSize;
                lpdv->lpMdv->wXDPI = 17;
            }
            return (ret);

        case CMDID_5CPI:
        case CMDID_6CPI:
        case CMDID_8CPI:
            if (wCmdCbId == CMDID_5CPI)
                lpCmdStr = (LPBYTE)&Device.DeviceData.f10Pitch;
            else
                if (wCmdCbId == CMDID_6CPI)
                lpCmdStr = (LPBYTE)&Device.DeviceData.f12Pitch;
            else
                lpCmdStr = (LPBYTE)&Device.DeviceData.f17Pitch;

            if (!lstrlen(lpCmdStr) ||
                !lstrlen((LPBYTE)&Device.DeviceData.DoubleOn))
                goto null_cmd;

            lstrcpy((LPBYTE)tBuff, (LPBYTE)(&Device.DeviceData.DoubleOn));
            lstrcat((LPBYTE)tBuff, lpCmdStr);
            return (bSelectString((LPBYTE)tBuff, lpBuf, lpwSize));

        default:

            null_cmd:
            *lpwSize = 0;
            return (TRUE);
    }
}

//-----------------------------*bSelectString*------------------------------//
// Function:    bSelectString
//
// Action:      Take the font escape string from TTY data structures
//              and pass it to lpBuf.
//              Return TRUE if the string length < lpBuf, FALSE otherwise.
//
// Notes:       wCmdCbId values passed from TTY GPC data to fnOEMGetFontCmd()
//              determine which string is written to lpBuf.


BOOL NEAR PASCAL bSelectString(
                              LPBYTE byFontEscStr, // Font string to send
                              LPBYTE lpBuf,        // Buffer to place string into
                              LPWORD lpwSize)      // Actual buffer size
{
    WORD wSelectLen;

    wSelectLen = lstrlen((LPCSTR)byFontEscStr);
    if (wSelectLen > *lpwSize)
    {
        *lpwSize = wSelectLen;
        return (FALSE);
    }
    else // Font select string  < sizeof(lpBuf)
    {
        _fmemcpy(lpBuf, byFontEscStr, wSelectLen);
        *lpwSize = wSelectLen;
        return (TRUE);
    }
}

//---------------------------**fnOEMOutputCmd*------------------------------//
// Function: fnOEMOutputCmd
//
// Action:      Explicitly examine wCmdCbId returned from TTY's GPC data for
//              CMDID Values. This command will construct a command string
//              for the returned value and send it to the spooler via WriteSpoolBuf.
//
// Notes:       There can be up to 254 callback id#'s.
//              There can be at most 254 callback id#'s due to the size of
//              this id field in the Command Descriptor structure being BYTE.
//              We are not interested in handling parameters for any
//              string since it makes not sense at this time.

VOID CALLBACK  fnOEMOutputCmd(
                             LPDV lpdv,         // Pointer to PDEVICE structure
                             WORD wCmdCbId,     // Command callback id#, placed in minidriver GPC data
                             LPDWORD lpdwParams)// Pointer to optional parameters of the command

{
    LPSTR lpCmdStr;
    WORD wCmdStrlen;

    if (lpdv->fMdv == FALSE)
        if (!bMiniEnable(lpdv))
            return;

    if (wCmdCbId == CMDID_BEGINJOB)
        lpCmdStr = (LPSTR)(&(Device.DeviceData.BeginJob));
    else
        if (wCmdCbId == CMDID_ENDJOB)
    {
        lpCmdStr = (LPSTR)(&(Device.DeviceData.EndJob));
        if (lpdv->fMdv ==  TRUE)
        {
            GlobalFreePtr(lpdv->lpMdv); //free mem on private minidriver area
            lpdv->fMdv = FALSE;
        }
    }
    else
        if (wCmdCbId == CMDID_PAPERSIZE)
        lpCmdStr = (LPSTR)(&(Device.DeviceData.PaperSize));
    else
        if (wCmdCbId == CMDID_PAPERSOURCE)
        lpCmdStr = (LPSTR)(&(Device.DeviceData.PaperSource));
    else
        if (wCmdCbId == CMDID_PAPRSRC_NOBRK)
    {
        lpdv->lpMdv->bNoFF = TRUE; //no FF or linefeeds wanted at end of page
        lpCmdStr = (LPSTR)(&(Device.DeviceData.PaperSource));
    }
    else
        if (wCmdCbId == CMDID_BOLD_ON)
        lpCmdStr = (LPSTR)(&(Device.DeviceData.BoldOn));
    else
        if (wCmdCbId == CMDID_BOLD_OFF)
        lpCmdStr = (LPSTR)(&(Device.DeviceData.BoldOff));
    else
        if (wCmdCbId == CMDID_UNDERLINE_ON)
        lpCmdStr = (LPSTR)(&(Device.DeviceData.UnderlineOn));
    else
        if (wCmdCbId == CMDID_UNDERLINE_OFF)
        lpCmdStr = (LPSTR)(&(Device.DeviceData.UnderlineOff));
    else
        if (wCmdCbId == CMDID_LF)
    {
        lpdv->lpMdv->wXMRound = 0; //reset our x-move rounding count to zero
        WriteSpoolBuf(lpdv, "\x0A",1);
        return;
    }
    else
        if (wCmdCbId == CMDID_FF)
    {
        lpdv->lpMdv->wXMRound = 0; //reset our x-move rounding count to zero
        if (lpdv->lpMdv->bNoFF == FALSE)
            WriteSpoolBuf(lpdv, "\x0D\x0C",2);
        return;
    }
    else
        if (wCmdCbId == CMDID_CR)
    {
        lpdv->lpMdv->wXMRound = 0; //reset our x-move rounding count to zero
        WriteSpoolBuf(lpdv, "\x0D",1);
        return;
    }
    else
        if (wCmdCbId == CMDID_XMOVE)
    {
        WORD wNumSpaces;
        WORD i,wXM;

        static char SpaceBuf[400] ; //20 inches at 20dpi


        if (!SpaceBuf[0])
            for (i=0;i< 400;i++)
                SpaceBuf[i] = 0x20;

        if (lpdv->lpMdv->wXDPI == 0) //no cmd , assume 10cpi
        {
            lpdv->lpMdv->wXDPI = 10 ;
            lpdv->lpMdv->wXMCmdLen = 0 ;
        }

        //grap our parameter and add in the rounding to date for this line
        wXM = (WORD)*lpdwParams + lpdv->lpMdv->wXMRound;

        //Always round down - old tty did.
        wNumSpaces  = wXM * (WORD)lpdv->lpMdv->wXDPI / 600;

        WriteSpoolBuf(lpdv, lpdv->lpMdv->XMCmd, lpdv->lpMdv->wXMCmdLen);
        WriteSpoolBuf(lpdv, (LPBYTE)SpaceBuf, wNumSpaces);

        //save our rounding error, may be able to use it later
        lpdv->lpMdv->wXMRound = wXM-(wNumSpaces * 600 /(WORD)lpdv->lpMdv->wXDPI);

        return;
    }

    wCmdStrlen = _fstrlen(lpCmdStr);
    WriteSpoolBuf(lpdv, lpCmdStr, wCmdStrlen);

}


//--------------------------**fnOEMOutputChar*---------------------------------//
// Function:    fnOEMOutputChar()
//
// Action:      Output n characters pointed to by lpStr. Translation happens
//              on any extended characters in lpStr before printing.
//
// Notes:       We go through lpStr printing each character until we need to
//              translate for an extended character.
//              We call SubstituteExtendedChar() which will read the device
//              data for this model if it has not already been retrieved.
//              We  get an offset into the translation table and print
//              those characters which represent that extended character.
//
//              We then return from SubstituteExtendedChar() and continue
//              printing the contents of lpStr.
//              We currently do not call Unidrv to handle this if no change
//              has been made to the default translation set via the TTY UI.

VOID CALLBACK fnOEMOutputChar(LPDV lpdv, LPSTR lpStr, WORD n, short rcID)
{
    UINT i;
    BYTE CharToSubstitute[2] = {0,0}; // Need null termination here.

    for (i = 0;  i < n; i++)
    {
        CharToSubstitute[0] = (BYTE)lpStr[i];
        if (CharToSubstitute[0] >= START_EXTCHARS)
            SubstituteExtendedChar(lpdv, CharToSubstitute[0]);
        else
            WriteSpoolBuf(lpdv, (LPSTR)&CharToSubstitute, 1);
    }
}

VOID NEAR PASCAL __loadds SubstituteExtendedChar(LPDV lpdv, BYTE ExtendedChar)
{
    UINT   CharTablePos;
    WORD   wBufSize;
    LPSTR  lpBufStr;
    BYTE   buff[2] = {0,0};

    if (!bHasDeviceData)
    {
        LoadTTYDataFile();
        GetDevice(GetCurDevice());
        bHasDeviceData = TRUE;
    }

    CharTablePos = ExtendedChar - START_EXTCHARS;

    if (Device.DeviceData.CodePage == RES_DB_CUSTOM)
    {
        lpBufStr = (LPSTR)(&(Device.CTTCode[CharTablePos][0]));
    }
    else
        if (Device.DeviceData.CodePage == RES_DB_CP437)
    {
        buff[0] = CodePage437CharSet[CharTablePos];
        lpBufStr = (LPBYTE)buff;
    }
    else
        if (Device.DeviceData.CodePage == RES_DB_CP850)
    {
        buff[0] = CodePage850CharSet[CharTablePos];
        lpBufStr = (LPBYTE)buff;
    }


    wBufSize = _fstrlen(lpBufStr);
    WriteSpoolBuf(lpdv, lpBufStr, wBufSize);
}

//-----------------------------**Enable**-----------------------------------//
// Function: Enable()
// Action:   We setup the callback to fnOEMOutPutChar and call UniEnable.

short CALLBACK Enable(
                     LPDV lpdv,
                     WORD style,
                     LPSTR lpModel,
                     LPSTR lpPort,
                     LPDM lpStuff)
{
    CUSTOMDATA cd;

    cd.cbSize = sizeof(CUSTOMDATA);
    cd.hMd = GetModuleHandle((LPSTR)rgchModuleName);
    cd.fnOEMDump = NULL;
    cd.fnOEMOutputChar = (LPFNOEMOUTPUTCHAR)fnOEMOutputChar;
    return (UniEnable(lpdv, style, lpModel, lpPort, lpStuff, &cd));
}

//Setup private minidriver area on lpdv

BOOL bMiniEnable(lpdv)
LPDV lpdv;

{
    if (! (lpdv->lpMdv =(LPMDV)GlobalAllocPtr(GMEM_MOVEABLE, sizeof(LPMDV))))
        return (FALSE);

    lpdv->lpMdv->wXMCmdLen = 0;
    lpdv->lpMdv->wXMRound = 0;
    lpdv->lpMdv->wXDPI = 0;
    lpdv->lpMdv->bNoFF = FALSE;
    lpdv->fMdv = TRUE;
    return (TRUE);

}




//--------------------------**PseudoInit**----------------------------------//
// Function:    PseudoInit
//
// Action:      We subcalss each of the TTY dialog controls in order
//              to handle & display extended keyboard input such as esc,
//              ctrl and any ALT+0XXX numeric keypad input.
//              Return TRUE if class is successfully initialised, else FALSE.


BOOL NEAR PASCAL PseudoInit(HANDLE hInstance)
{
    WNDCLASS   class;

    hTTYInst            = hInstance;
    class.style         = CS_HREDRAW | CS_VREDRAW;
    class.lpfnWndProc   = (WNDPROC)TTYPseudoEdit;
    class.cbClsExtra    = 0;
    class.cbWndExtra    = 8; // Reserve extra space for dialog window.
    class.hInstance     = hInstance;
    class.hIcon         = NULL;
    class.hCursor       = LoadCursor(NULL, IDC_IBEAM);
    class.hbrBackground = (HBRUSH)GetStockObject(TRANSPARENT);
    class.lpszMenuName  = NULL;
    class.lpszClassName = szName;

    return (RegisterClass((LPWNDCLASS)&class));
}

// TTY Property Sheet Functions.
// 5/26/1993 ZhanW  Microsoft Corp.
// pseudo code for propertysheet UI
// Code complete 16/6/93. PaulMcB, WPG Ireland.

short WINAPI DeviceMode(
                       HWND   hWnd,
                       HANDLE hInstMiniDrv,    // Instance handle of mini driver
                       LPSTR  lpDevName,
                       LPSTR  lpPort)
{
    return (EDMPropSheetDialogs(hWnd, hInstMiniDrv, NULL, lpDevName, lpPort,
                                NULL, NULL, DM_PROMPT | DM_UPDATE, NULL, NULL, NULL) == IDOK);
}

int WINAPI ExtDeviceMode(
                        HANDLE    hWnd,           // parent for DM_PROMPT dialog box
                        HINSTANCE hInstMiniDrv,   // Instance handle of the mini driver
                        LPDM      lpdmOut,        // output DEVMODE for DM_COPY
                        LPSTR     lpDevName,      // device name
                        LPSTR     lpPort,         // port name
                        LPDM      lpdmIn,         // input DEVMODE for DM_MODIFY
                        LPSTR     lpProfile,      // alternate .INI file
                        WORD      wMode)          // operation(s) to carry out
{

    if (wMode & DM_PROMPT)
        return (EDMPropSheetDialogs(hWnd, hInstMiniDrv,lpdmOut,lpDevName,
                                    lpPort,lpdmIn,lpProfile,wMode, NULL, NULL, NULL));

    return (UniExtDeviceMode(hWnd,hInstMiniDrv,lpdmOut,lpDevName,lpPort,
                             lpdmIn,lpProfile,wMode));
}

int WINAPI ExtDeviceModePropSheet(
                                 HWND                 hWnd,
                                 HINSTANCE            hInstMiniDrv,
                                 LPSTR                lpDevName,
                                 LPSTR                lpPort,
                                 DWORD                dwReserved,
                                 LPFNADDPROPSHEETPAGE lpfnAdd,
                                 LPARAM               lParam)
{
    return (EDMPropSheetDialogs(hWnd, hInstMiniDrv, NULL, lpDevName,
                                lpPort, NULL, NULL, DM_PROMPT | DM_UPDATE,
                                dwReserved, lpfnAdd, lParam));
}

LONG WINAPI AdvancedSetUpDialog(
                               HWND    hWnd,
                               HANDLE  hInstMiniDrv,   // handle of the driver module
                               LPDM    lpdmIn,         // initial device settings
                               LPDM    lpdmOut)        // final device settings
{
    // If user tries to set up from common print dialogs, display
    // dialog message to say that there is nothing to configure.
    // ( ie we can only do this from control panel/print manager )

    char szMessage[MESSAGE_TEXT_LENGTH];
    char szCaption[CAPTION_TEXT_LENGTH];

    if (hInstMiniDrv != NULL)
    {
        LoadString(hInstMiniDrv, IDS_MSG_NOCONFIGURE, (LPSTR)szMessage, sizeof(szMessage));
        LoadString(hInstMiniDrv, IDS_MSG_CAPTION, (LPSTR)szCaption, sizeof(szCaption));

        MessageBox(NULL, (LPSTR)szMessage, (LPSTR)szCaption, MB_ICONEXCLAMATION | MB_OK);


    }

    return (-1L);
}


int NEAR PASCAL EDMPropSheetDialogs(
                                   HWND                 hWnd,
                                   HINSTANCE            hInstMiniDrv,
                                   LPDM                 lpdmOut,
                                   LPSTR                lpDevName,
                                   LPSTR                lpPort,
                                   LPDM                 lpdmIn,
                                   LPSTR                lpProfile,
                                   WORD                 wMode,
                                   DWORD                dwReserved,
                                   LPFNADDPROPSHEETPAGE lpfnAdd,   // could be NULL, if called from ExtDeviceMode
                                   LPARAM               lParam)
{
    int                  result, i;
    LPFNADDPROPSHEETPAGE lpfnAddProc;
    long                 lAddProcParam;
    PROPSHEETPAGE        pspTmp;
    PROPSHEETPROC        psProc;
    CREATEPROC           CreateProc;
    LPTTYPSP             lpTTYPSP;
    LPBYTE               lpDlgData;     // ptr to TTY's private dialog data
    WORD                 wLoop;
    HINSTANCE            hInstShell;

    _fmemset((LPSTR)&pspTmp,0,sizeof(PROPSHEETPAGE));


    // The function TTYAddPropertySheet, which processes dialogs sheets
    // returned from Unidrv, must have the same interface as 'lpfnAdd' passed
    // in from PrintMan.

    lpfnAddProc = lpfnAdd ? lpfnAdd : TTYAddPropertySheet;

    // Collect dialog sheets from Unidrv. If 'lpfnAdd' is not NULL, then
    // they are passed back to PrintMan directly. Otherwise, TTY driver
    // spools the dialog sheets info in its own buffer lpTTYPSI, which is
    // large enough to hold dialog sheets of both Unidrv and TTY minidriver.

    if (!(lpTTYPSP = (LPTTYPSP)GlobalAllocPtr(GHND, sizeof(TTYPSP))))
    {
        result = -1;
        goto EDM_exit;
    }

    lAddProcParam = lpfnAdd ? lParam : (long)lpTTYPSP;

    result = UniExtDeviceModePropSheet(hWnd,hInstMiniDrv,lpDevName,lpPort,
                                       dwReserved,lpfnAddProc,lAddProcParam);

    if (result != IDOK)
        goto EDM_exit;

    // First, allocate global memory for data that need to be
    // carried around during dialog time. Refer to Unidrv's DIALOGINFO.
    // We don't use DlgData yet, we use the global variable Printer.
    // Since we don't yet use this, we allocate a byte for the time being.

    if (!(lpDlgData = GlobalAllocPtr(GHND, 4)))
    {
        result = -1;
        goto EDM_exit;
    }

    // Initialize this data structure as needed. Initialise the TTY property
    // sheet dialog controls.

    PseudoInit(hInstMiniDrv);

    // Get the proc address of PropertySheet & Shell_CreateDlgPropSheetPage
    // from commctrl.dll rather than explicitly importing them.

    if (((hInstShell=LoadLibrary("COMMCTRL.DLL")) < HINSTANCE_ERROR) ||
        (!(psProc=(PROPSHEETPROC)GetProcAddress(hInstShell,
                                                PROPERTYSHEETFUNCTION))) ||
        (!(CreateProc=(CREATEPROC)GetProcAddress(hInstShell,
                                                 CREATEPROPERTYSHEETPAGEFUNCTION))))
    {
        goto EDM_exit;
    }

    // Now, continue to enumerate TTY's dialog sheets.
    // NOTE: do NOT modify any of Unidrv's dialog sheet because it increases
    // the inter-dependency between Unidrv and minidriver unnecessarily.
    // Add new entries to lpTTYPSP->rglpPropPage array.
    // Increment lpTTYPSP->wCount accordingly.

    for (i = 0; i < TTY_DI && lpTTYPSP->wCount < MAX_DI; i++)
    {
        // Set up PROPSHEETPAGE structure

        pspTmp.hInstance = hTTYInst;
        pspTmp.pszTemplate = TTYTemplates[i];
        pspTmp.pfnDlgProc = TTYDlgProcs[i];
        pspTmp.lParam = (LPARAM)lpDlgData;
        pspTmp.dwSize=sizeof(PROPSHEETPAGE);

        if (i==0)
        {
            pspTmp.pfnCallback=PropPageCallback;
            pspTmp.dwFlags=PSP_USECALLBACK;
        }
        else
        {
            pspTmp.pfnCallback=NULL;
            pspTmp.dwFlags=PSP_DEFAULT;
        }

        // Create dialog prop sheet interface

        lpTTYPSP->rglpPropPage[lpTTYPSP->wCount] = CreateProc(&pspTmp);

        if (!lpTTYPSP->rglpPropPage[lpTTYPSP->wCount])
        {
            // WORKITEM -> error message here.
            // bypass this dialog sheet.
            continue;
        }

        // increment the count
        lpTTYPSP->wCount++;
    }

    if (lpfnAdd)
    {
        // Called from ExtDeviceModePropSheet...
        // We know that Unidrv's dialog sheets have already been returned
        // to PrintMan via 'lpfnAdd' and they are NOT in TTYPSP data
        // structure. Now, continue to enumerate TTY's own dialog sheets.

        for (wLoop = 0; wLoop < lpTTYPSP->wCount; wLoop++)
            lpfnAdd(lpTTYPSP->rglpPropPage[wLoop], lParam);

        // At this point, we're done, so just return success. Later,
        // PropPageCallback() will be called to save the setting changes.

        result = IDOK;
        goto EDM_exit;
    }
    else        // We have to bring up the dialogs ourselves.
    {
        PROPSHEETHEADER psh;
        char            szTitle[MAX_STRING_LENGTH];

        // SetByteValue((LPSTR)&psh,0,sizeof(PROPSHEETHEADER));
        // Don't link to Unidrv's utils.asm - use _fmemset instead of SetByteValue
        // Initialise psh.lppsInfo & psh.uStructSize also - PaulMcB

        _fmemset((LPSTR)&psh,0,sizeof(PROPSHEETHEADER));
        lstrcpy(szTitle, lpDevName);

        psh.pszCaption = szTitle;
        psh.dwSize = sizeof(PROPSHEETHEADER);
        psh.hwndParent = hWnd;
        psh.phpage = lpTTYPSP->rglpPropPage;
        psh.nPages = lpTTYPSP->wCount;
        psh.dwFlags = PSH_PROPTITLE;
        psh.hInstance = hTTYInst;

        result = psProc(&psh);

    }   // end else

    EDM_exit:
    if (lpTTYPSP)
        GlobalFreePtr(lpTTYPSP);
    if (lpDlgData)
        GlobalFreePtr(lpDlgData);
    if (hInstShell)
        FreeLibrary(hInstShell);

    return (result);
}

// dialog sheet callback (release) function:

UINT _loadds FAR PASCAL PropPageCallback(HWND            hWnd,
                                         UINT            uMsg,
                                         LPPROPSHEETPAGE lpPSP)
{
    if (PSPCB_RELEASE==uMsg)
    {
        // LPBYTE    lpDlgData;       // pointer to TTY's private data structure

        // if (!(lpDlgData = lpPSP->lParam))
        //   return;

        // WORKITEM -> code here to save user's changes to files or whatever.
        // @@@ We make changes to data just before new data is displayed
        // @@@ in Ttydlg.c. We commit these changes to disk.
        // @@@ When we move all operations to memory, we save the changes here.
        // !!! code here to release memory, resource, libraries, if any.
        // don't forget to free TTY's private data structure

        // GlobalFreePtr(lpDlgData);

    }

    return (1);
}

BOOL FAR PASCAL TTYAddPropertySheet(HPROPSHEETPAGE hPage, long lParam)
{

    static bCount = 0; // We only want to use 1st returned prop sheet.
                       // Which in this case is Unidrv's paper propertysheet.

    LPTTYPSP    lpTTYPSP = (LPTTYPSP)lParam;

    if (!lpTTYPSP || lpTTYPSP->wCount >= MAX_DI)
        return (FALSE);

    if (!bCount)
    {
        lpTTYPSP->rglpPropPage[lpTTYPSP->wCount] = hPage;
        lpTTYPSP->wCount++;
    }
    bCount ++;

    return (TRUE);
}





















#ifdef WANT_WIN31

// Just keep this here in case we want to test functionality and don't have
// property sheets working.


//--------------------------**ExtDeviceMode**-------------------------------//
// ! Functionality currently limited to 3.1 Unidrv
//
// Function:    ExtDeviceMode // Currently Win31 enabled.
//
// Action:      Before doing anything here, we must first initialise the
//              TTY UI edit controls with their own class (this handles
//              stuff like displaying <ESC> in the UI when the user hits
//              the ESC key on the keyboard. If this fails then we don't
//              do anything except call UniDrv's UniExtDecviceMode().
//
//              Before calling UniDeviceMode, we call SetWindowsHook
//              with the WH_MSGFILTER flag. This causes all messages to
//              get passed to the filter function TTYUIHookProc before
//              being passed to to the dialog proc. The filter function
//              passes everything bar WM_COMMAND from the Options... button
//              When the filter receives notification that options has
//              been pressed, it calls the TTY UI and absorbs the message
//              so UniDrv's dialog proc never knows that the button was
//              pressed. The user makes changes in the TTY UI and returns
//              to the UniDrv UI.
//
//--------------------------------------------------------------------------//

short CALLBACK ExtDeviceMode(
                            HWND       hWnd,
                            HINSTANCE  hInst,         //  Minidriver hInst
                            LPDM       lpdmOut,
                            LPSTR      lpDevName,
                            LPSTR      lpPort,
                            LPDM       lpdmIn,
                            LPSTR      lpProfile,
                            WORD       wMode)
{
    int nReturn=-1;

    // Only install hook if (1) no hook already installed, (2) we're
    // going to display a dialog, and (3) We can initialize our classes.

    if (!hOldHook && (wMode & DM_PROMPT) && PseudoInit(hInst))
        hOldHook=SetWindowsHookEx(WH_CBT,TTYUIHookProc,hInst,
                                  GetCurrentTask());

    nReturn = UniExtDeviceMode(hWnd, hInst, lpdmOut, lpDevName, lpPort,
                               lpdmIn, lpProfile, wMode);


    if (hOldHook)
    {
        UnhookWindowsHookEx(hOldHook);
        UnregisterClass(szName,hInst);
        hOldHook=NULL;
        OldDlgProc=NULL;
    }

    return (nReturn);

}


BOOL FAR PASCAL MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam)
{
    if ((WM_COMMAND==uMsg) && ((WORD)OPTIONS_BUTTON_HIT==wParam))
    {
        DialogBox(hTTYInst,MAKEINTRESOURCE(100),hWnd,(FARPROC)MainTTYDialog);
        return (TRUE);
    }
    return (OldDlgProc(hWnd,uMsg,wParam,lParam));
}


LRESULT CALLBACK TTYUIHookProc(
                              int       nCode,
                              WPARAM    wParam,
                              LPARAM    lParam)
{
    static HWND hwndMain=NULL;

    if (nCode >= 0)
    {
        // This code is completely insane, but it works--basically, we
        // receive a CBT_CREATEWND notification, but have no way to change
        // its dialog proc. Remember it, then when we get the second
        // CBT_CREATEWND notification, redirect the dialog proc from the
        // first one.

        if (!OldDlgProc && (HCBT_CREATEWND==nCode))
        {
            if (hwndMain)
            {
                if (OldDlgProc=(DLGPROC)GetWindowLong(hwndMain,DWL_DLGPROC))
                {
                    SetWindowLong(hwndMain,DWL_DLGPROC,(LONG)MainDlgProc);
                    hwndMain=NULL;
                }
            }
            else
                hwndMain=(HWND)wParam;
        }
    }
    return (CallNextHookEx(hOldHook,nCode,wParam,lParam));
}

#endif
