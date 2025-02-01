// TTY.H

#define PRINTDRIVER
#define CHICAGO
#define LPDV_DEFINED
#define LPMDV_DEFINED

#include "print.h"
#include "gdidefs.inc"
#include "minidriv.h"
#include "mdevice.h"
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <windows.h>
#include "windowsx.h"
#include "ttyfile.h"
#include "ttydlg.h"
#include "generic.h"


#define START_EXTCHARS  ((BYTE) 0x80) // Begining of extended chars in ANSI

// Command callback id#s. for TTY GPC and PFM data

#define CMDID_BEGINJOB      1  // Entered in PAGECONTROL.PC_OCD_BEGINDOC as %1
#define CMDID_ENDJOB        2
#define CMDID_PAPERSIZE     3
#define CMDID_PAPERSOURCE   4
#define CMDID_BOLD_ON       5
#define CMDID_BOLD_OFF      6
#define CMDID_UNDERLINE_ON  7
#define CMDID_UNDERLINE_OFF 8
#define CMDID_10CPI         9  // Entered in 10cpi font select string as %9
#define CMDID_12CPI         10
#define CMDID_17CPI         11
#define CMDID_5CPI          12
#define CMDID_6CPI          13
#define CMDID_8CPI          14
#define CMDID_XMOVE         15
#define CMDID_CR            16
#define CMDID_LF            17
#define CMDID_FF            18
#define CMDID_PAPRSRC_NOBRK 19  //special case for Continuous - No Page Break

#define CMDID_FONT_UNSELECT 255  // All fonts use same unselect id#. i.e. %255


#ifndef MAX_STRING_LENGTH
#define MAX_STRING_LENGTH 260
#endif

#define CAPTION_TEXT_LENGTH 40
#define MESSAGE_TEXT_LENGTH 120

// Minidriver definition of LPDV

typedef struct
    {
    WORD wXDPI;     //The font we use for x-movement - in spaces
    WORD wXMCmdLen;
    WORD wXMRound;  //collect rounding erros on xmovement commands
    BOOL bNoFF;     //special case needed for Continuous - No Page Break
    char XMCmd[256];//Store the font select command for our default font
    } PMDEVICE ,FAR  * LPMDV ;

typedef struct
    {
    short iType;
    short oBruteHdr;
    LPMDV   lpMdv;
    BOOL    fMdv;
    } PDEVICE, FAR * LPDV;

typedef LPFONTINFO LPFONT;

// Exported Functions. (from old TTYPROTO.h)

short CALLBACK    fnOEMGetFontCmd(LPDV, WORD, LPFONT, BOOL, LPBYTE, LPWORD);
VOID  CALLBACK    fnOEMOutputChar(LPDV, LPSTR, WORD, short);
VOID  CALLBACK    fnOEMOutputCmd(LPDV, WORD, LPDWORD);
BOOL  CALLBACK    TTYAddPropertySheet(HPROPSHEETPAGE, LPARAM);


// Local & Unidrv functions.

LRESULT CALLBACK TTYUIHookProc(int, WPARAM, LPARAM);
BOOL NEAR PASCAL PseudoInit(HANDLE);
BOOL NEAR PASCAL bSelectString(LPBYTE, LPBYTE, LPWORD);
VOID NEAR PASCAL __loadds SubstituteExtendedChar(LPDV, BYTE);
int  NEAR PASCAL EDMPropSheetDialogs(HWND,HINSTANCE, LPDM, LPSTR, LPSTR, LPDM,
                                     LPSTR, WORD, DWORD, LPFNADDPROPSHEETPAGE,
                                     LPARAM);
BOOL NEAR PASCAL bMiniEnable(LPDV);

UINT __loadds CALLBACK PropPageCallback(HWND,UINT,LPPROPSHEETPAGE);
short WINAPI DeviceMode(HWND, HANDLE, LPSTR, LPSTR);
short WINAPI Enable(LPDV, WORD, LPSTR, LPSTR, LPDM);
int   WINAPI ExtDeviceMode(HWND, HINSTANCE, LPDM, LPSTR, LPSTR,
                           LPDM, LPSTR, WORD);
int   WINAPI ExtDeviceModePropSheet(HWND, HINSTANCE, LPSTR, LPSTR,
                           DWORD, LPFNADDPROPSHEETPAGE, LPARAM);
