//-----------------------------------------------------------------------------
// This files contains the module name for this mini driver.  Each mini driver
// must have a unique module name.  The module name is used to obtain the
// module handle of this Mini Driver.  The module handle is used by the
// generic library to load in tables from the Mini Driver.
//-----------------------------------------------------------------------------

char *rgchModuleName = "ESCPAGE";

#define PRINTDRIVER
#include <print.h>
#include "mdevice.h"
#include "gdidefs.inc"
#include "unidrv.h"

#ifndef _INC_WINDOWSX
#include <windowsx.h>
#endif

#define SHIFTJIS_CHARSET 128
#define CCHMAXCMDLEN 128
typedef struct
{
  BYTE  fGeneral;       // General purpose bitfield
  BYTE  bCmdCbId;       // Callback ID; 0 iff no callback
  WORD  wCount;         // # of EXTCD structures following
  WORD  wLength;        // length of the command
} CD, *PCD, FAR * LPCD;

//----------------------------*OEMScaleWidth*--------------------------------
// Action: return the scaled width which is calcualted based on the
//      assumption that ESC\Page assumes 72 points in one 1 inch.
//
// Formulas:
//  <extent> : <font units> = <base Width> : <hRes>
//  <base width> : <etmMasterHeight> = <newWidth> : <newHeight>
//  <etmMasterUnits> : <etmMasterHeight> = <font units> : <vRes>
// therefore,
//   <newWidth> = (<extent> * <hRes> * <newHeight>) / 
//                  (<etmMasterUnits> * <vRes>)
//---------------------------------------------------------------------------
short FAR PASCAL OEMScaleWidth(width, masterUnits, newHeight, vRes, hRes)
short width;        // in units specified by 'masterUnits'.
short masterUnits;
short newHeight;    // in units specified by 'vRes'.
short vRes, hRes;   // height and width device units.
{
    DWORD newWidth10;
    short newWidth;

    // assert that hRes == vRes to avoid overflow problem.
    if (vRes != hRes)
        return 0;

    newWidth10 = (DWORD)width * (DWORD)newHeight * 10;
    newWidth10 /= (DWORD)masterUnits;

    // we multiplied 10 first in order to maintain the precision of
    // the width calcution. Now convert it back and round to the
    // nearest integer.
    newWidth = (short)((newWidth10 + 5) / 10);

    return newWidth;
}

//-----------------------------------------------------------------------------
// Action:  itoa
//-----------------------------------------------------------------------------
short NEAR PASCAL itoa(buf, n)
LPSTR buf;
short n;
{

    BOOL    fNeg;
    short   i, j, k;

    if (fNeg = (n < 0))
        n = -n;

    for (i = 0; n; i++)
    {
        buf[i] = n % 10 + '0';
        n /= 10;
    }

    // n was zero
    if (i == 0)
        buf[i++] = '0';

    if (fNeg)
        buf[i++] = '-';

    for (j = 0, k = i-1 ; j < i / 2 ; j++,k--)
    {
        char tmp;

        tmp = buf[j];
        buf[j] = buf[k];
        buf[k] = tmp;
    }

    buf[i] = 0;

    return i;
}

//---------------------------*OEMSendScalableFontCmd*--------------------------
// Action:  send ESC\Page-style font selection command.
//-----------------------------------------------------------------------------
VOID FAR PASCAL OEMSendScalableFontCmd(lpdv, lpcd, lpFont)
LPDV    lpdv;
LPCD    lpcd;     // offset to the command heap
LPFONTINFO lpFont;
{
    LPSTR   lpcmd;
    short   ocmd;
    WORD    i;
    BYTE    rgcmd[CCHMAXCMDLEN];    // build command here

    if (!lpcd || !lpFont)
        return;

    // be careful about integer overflow.
    lpcmd = (LPSTR)(lpcd+1);
    ocmd = 0;

    for (i = 0; i < lpcd->wLength && ocmd < CCHMAXCMDLEN; )
        if (lpcmd[i] == '#' && lpcmd[i+1] == 'V')      // height
        {
            long    height;

            // use 1/300 inch unit, which should have already been set.
            // convert font height to 1/300 inch units
            height = ((long)(lpFont->dfPixHeight - lpFont->dfInternalLeading)
                      * 300)  / lpFont->dfVertRes ;
            ocmd += wsprintf(&rgcmd[ocmd], "%ld", height);
            i += 2;
        }
        else if (lpcmd[i] == '#' && lpcmd[i+1] == 'H')     // pitch
        {
            if (lpFont->dfPixWidth > 0)
            {
                short tmpWidth;

                if (lpcmd[i+2] == 'S')
                    tmpWidth = lpFont->dfAvgWidth;
                else
                if (lpcmd[i+2] == 'D')
                    tmpWidth = lpFont->dfMaxWidth;
                else
                    {
                    tmpWidth = lpFont->dfPixWidth;
                    i--;
                    }

                ocmd += itoa((LPSTR)&rgcmd[ocmd], 
                         (lpFont->dfHorizRes * 100 / tmpWidth) /100);
                rgcmd[ocmd] = '.';
                ocmd ++;
                ocmd += itoa((LPSTR)&rgcmd[ocmd], 
                         (lpFont->dfHorizRes * 100 / tmpWidth) %100);
            }
            i += 3;
            
        }
        else
            rgcmd[ocmd++] = lpcmd[i++];

    WriteSpoolBuf(lpdv, (LPSTR) rgcmd, ocmd);
}


