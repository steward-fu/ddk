// TTYSTUBS.C - TTY Stub functions, remove TTY dependencies on minidriv.c

#include "tty.h"
#include "unidrv.h"

extern char *rgchModuleName;    // global module name


short FAR PASCAL Control(lpdv, function, lpInData, lpOutData)
LPDV    lpdv;
WORD    function;
LPSTR   lpInData;
LPSTR   lpOutData;
{
  // TTY does not support GETSETPRINTORIENT which Unidrv supports.
  if (function == QUERYESCSUPPORT && *((LPWORD)lpInData) == GETSETPRINTORIENT)
    return 0;

  if (function == GETSETPRINTORIENT)
    return 0;

  // General case
  return UniControl(lpdv, function, lpInData, lpOutData);
}

BOOL FAR PASCAL DevBitBlt(lpdv, DstxOrg, DstyOrg, lpSrcDev, SrcxOrg, SrcyOrg,
                    xExt, yExt, lRop, lpPBrush, lpDrawmode)
LPDV        lpdv;           // --> to destination bitmap descriptor
short       DstxOrg;        // Destination origin - x coordinate
short       DstyOrg;        // Destination origin - y coordinate
LPBITMAP    lpSrcDev;       // --> to source bitmap descriptor
short       SrcxOrg;        // Source origin - x coordinate
short       SrcyOrg;        // Source origin - y coordinate
WORD        xExt;           // x extent of the BLT
WORD        yExt;           // y extent of the BLT
long        lRop;           // Raster operation descriptor
LPPBRUSH    lpPBrush;       // --> to a physical brush (pattern)
LPDRAWMODE  lpDrawmode;
{
  return 0;
}

WORD FAR PASCAL DevStretchBlt(lpdv, DstxOrg, DstyOrg, DstxExt, DstyExt,
                               lpSrcDev, SrcxOrg, SrcyOrg, xExt, yExt, 
                               lRop, lpPBrush, lpDrawmode, lpClip)
LPDV        lpdv;           // --> to destination bitmap descriptor
short       DstxOrg;        // Destination origin - x coordinate
short       DstyOrg;        // Destination origin - y coordinate
WORD        DstxExt;
WORD        DstyExt;
LPBITMAP    lpSrcDev;       // --> to source bitmap descriptor
short       SrcxOrg;        // Source origin - x coordinate
short       SrcyOrg;        // Source origin - y coordinate
WORD        xExt;           // x extent of the BLT
WORD        yExt;           // y extent of the BLT
long        lRop;           // Raster operation descriptor
LPPBRUSH    lpPBrush;       // --> to a physical brush (pattern)
LPDRAWMODE  lpDrawmode;
LPRECT      lpClip;
{
  return 0;
}

DWORD FAR PASCAL Pixel(lpdv, x, y, Color, lpDrawMode)
LPDV        lpdv;
short       x;
short       y;
DWORD       Color;
LPDRAWMODE  lpDrawMode;
{
  return 0;
}

short FAR PASCAL Output(lpdv, style, count, lpPoints, lpPPen, lpPBrush, lpDrawMode, lpCR)
LPDV        lpdv;       // --> to the destination
WORD        style;      // Output operation
WORD        count;      // # of points
LPPOINT     lpPoints;   // --> to a set of points
LPVOID      lpPPen;     // --> to physical pen
LPPBRUSH    lpPBrush;   // --> to physical brush
LPDRAWMODE  lpDrawMode; // --> to a Drawing mode
LPRECT      lpCR;       // --> to a clipping rectange if <> 0
{
  return 0;
}

DWORD FAR PASCAL StrBlt(lpdv, x, y, lpCR, lpStr, count, lpFont, lpDrawMode, lpXform)
LPDV        lpdv;
short       x;
short       y;
LPRECT      lpCR;
LPSTR       lpStr;
int         count;
LPFONTINFO  lpFont;
LPDRAWMODE  lpDrawMode;           // includes background mode and bkColor
LPTEXTXFORM lpXform;
{
  // StrBlt is never called by GDI.
  // Keep a stub function here so nobody complains.
  //
  return 0;
}

short FAR PASCAL ScanLR(lpdv, x, y, Color, DirStyle)
LPDV    lpdv;
short   x;
short   y;
DWORD   Color;
WORD    DirStyle;
{
  // ScanLR is only called for RASDISPLAY devices.
  // Keep a stub function here so nobody complains.
  //
  return 0;
}

short FAR PASCAL EnumObj(lpdv, style, lpCallbackFunc, lpClientData)
LPDV    lpdv;
WORD    style;
FARPROC lpCallbackFunc;
LPVOID  lpClientData;
{
  return 1;
}

DWORD FAR PASCAL ColorInfo(lpdv, ColorIn, lpPhysBits)
LPDV    lpdv;
DWORD   ColorIn;
LPDWORD lpPhysBits;
{

  return UniColorInfo(lpdv, ColorIn, lpPhysBits);
}

DWORD FAR PASCAL RealizeObject(lpdv, sStyle, lpInObj, lpOutObj, lpTextXForm)
LPDV        lpdv;
short       sStyle;
LPSTR       lpInObj;
LPSTR       lpOutObj;
LPTEXTXFORM lpTextXForm;
{
  if (sStyle == OBJ_FONT || sStyle == - OBJ_FONT)
    return UniRealizeObject(lpdv, sStyle, lpInObj, lpOutObj, lpTextXForm);
  else
    return 0;
}

short FAR PASCAL EnumDFonts(lpdv, lpFaceName, lpCallbackFunc, lpClientData)
LPDV    lpdv;
LPSTR   lpFaceName;
FARPROC lpCallbackFunc;
LPVOID  lpClientData;
{
  return UniEnumDFonts(lpdv, lpFaceName, lpCallbackFunc, lpClientData);
}

DWORD FAR PASCAL DevExtTextOut(lpdv, x, y, lpCR, lpStr, count, lpFont,
                        lpDrawMode, lpXform, lpWidths, lpOpaqRect, options)
LPDV        lpdv;
short       x;
short       y;
LPRECT      lpCR;
LPSTR       lpStr;
int         count;
LPFONTINFO  lpFont;
LPDRAWMODE  lpDrawMode;
LPTEXTXFORM lpXform;
LPSHORT     lpWidths;
LPRECT      lpOpaqRect;
WORD        options;
{
  DRAWMODE  dm;

  // 12/1/94 (ZhanW)
  // Now Unidrv looks at lpDrawMode->bkMode to opaque out text extent as well.
  // Change it to TRANSPARENT if necessary.
  if (lpDrawMode->bkMode == OPAQUE)
    {
    dm = *lpDrawMode;
    dm.bkMode = TRANSPARENT;
    lpDrawMode = (LPDRAWMODE)&dm;
    }
  if (!lpdv->iType)
    return 0;
  // no opaque background support
  return(UniExtTextOut(lpdv, x, y, lpCR, lpStr, count, lpFont,
                       lpDrawMode, lpXform, lpWidths, lpOpaqRect,
                       options & ~ETO_OPAQUE));
}

short FAR PASCAL DevGetCharWidth(lpdv, lpBuf, chFirst, chLast, lpFont, lpDrawMode,
                        lpXForm)
LPDV        lpdv;
LPSHORT     lpBuf;
WORD        chFirst;
WORD        chLast;
LPFONTINFO  lpFont;
LPDRAWMODE  lpDrawMode;
LPTEXTXFORM lpXForm;
{
  return(UniGetCharWidth(lpdv, lpBuf, chFirst, chLast, lpFont,lpDrawMode,
                         lpXForm));
}

short FAR PASCAL DeviceBitmap(lpdv, command, lpBitMap, lpBits)
LPDV     lpdv;
WORD     command;
LPBITMAP lpBitMap;
LPSTR    lpBits;
{
  return 0;
}

short FAR PASCAL FastBorder(lpRect, width, depth, lRop, lpdv, lpPBrush,
                                          lpDrawmode, lpCR)
LPRECT  lpRect;
short   width;
short   depth;
long    lRop;
LPDV    lpdv;
long    lpPBrush;
long    lpDrawmode;
LPRECT  lpCR;
{
  return 0;
}

short FAR PASCAL SetAttribute(lpdv, statenum, index, attribute)
LPDV    lpdv;
WORD    statenum;
WORD    index;
WORD    attribute;
{
  return 0;
}

short FAR PASCAL DeviceBitmapBits(lpdv, style, iStart, sScans, lpDIBits,
                                    lpDIBHdr, lpDrawMode, lpConvInfo)
LPDV                lpdv;
WORD                style;
WORD                iStart;
WORD                sScans;
LPSTR               lpDIBits;
LPBITMAPINFOHEADER  lpDIBHdr;
LPDRAWMODE          lpDrawMode;
LPSTR               lpConvInfo;
{
  return -1;
}


short FAR PASCAL SetDIBitsToDevice(lpdv, DstXOrg, DstYOrg, StartScan, NumScans,
                         lpCR, lpDrawMode, lpDIBits, lpDIBHdr, lpConvInfo)
LPDV                lpdv;
WORD                DstXOrg;
WORD                DstYOrg;
WORD                StartScan;
WORD                NumScans;
LPRECT              lpCR;
LPDRAWMODE          lpDrawMode;
LPSTR               lpDIBits;
LPBITMAPINFOHEADER  lpDIBHdr;
LPSTR               lpConvInfo;
{
  return 0;
}

int FAR PASCAL StretchDIB(lpdv, wMode, DstX, DstY, DstXE, DstYE,
                SrcX, SrcY, SrcXE, SrcYE, lpBits, lpDIBHdr,
                lpConvInfo, dwRop, lpbr, lpdm, lpClip)
LPDV                lpdv;
WORD                wMode;
short               DstX, DstY, DstXE, DstYE;
short               SrcX, SrcY, SrcXE, SrcYE;
LPSTR               lpBits;             /* pointer to DIBitmap Bits */
LPBITMAPINFOHEADER  lpDIBHdr;           /* pointer to DIBitmap info Block */
LPSTR               lpConvInfo;         /* not used */
DWORD               dwRop;
LPPBRUSH            lpbr;
LPDRAWMODE          lpdm;
LPRECT              lpClip;
{
  return 0;
}

short FAR PASCAL CreateDIBitmap()
{
  // CreateDIBitmap is never called by GDI.
  // Keep a stub function here so nobody complains.
  //
  return(0);
}

void FAR PASCAL Disable(lpdv)
LPDV lpdv;
{
  UniDisable(lpdv);
}


DWORD FAR PASCAL DeviceCapabilities(lpDevName, lpPort, wIndex, lpOutput, lpdm)
LPSTR   lpDevName;
LPSTR   lpPort;
WORD    wIndex;
LPSTR   lpOutput;
LPDM    lpdm;
{
  return(UniDeviceCapabilities(lpDevName, lpPort, wIndex, lpOutput, lpdm,
                               GetModuleHandle((LPSTR)rgchModuleName)));
}


int FAR PASCAL DevInstall(hWnd, lpDevName, lpOldPort, lpNewPort)
HWND    hWnd;
LPSTR   lpDevName;
LPSTR   lpOldPort, lpNewPort;
{
  return UniDevInstall(hWnd, lpDevName, lpOldPort, lpNewPort);
}


VOID FAR PASCAL WEP(fExitWindows)
short fExitWindows;
{
}

int FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg, WORD cbHeapSize,
               LPSTR lpszCmdLine)
{
  return 1;
}

