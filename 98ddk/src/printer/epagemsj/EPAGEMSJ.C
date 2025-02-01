//----------------------------------------------------------------------------//
// Filename:    epagemsj.c                                                     //
//                                                                            //
// This file contains code for downloading bitmap TrueType fonts              //
// on Epson ESC/Page printers.                                                //
//                                                                            //
//                                                                            //
//  Copyright (c) 1992-1994 Microsoft Corporation                             //
//----------------------------------------------------------------------------//

//-----------------------------------------------------------------------------
// This files contains the module name for this mini driver.  Each mini driver
// must have a unique module name.  The module name is used to obtain the
// module handle of this Mini Driver.  The module handle is used by the
// generic library to load in tables from the Mini Driver.
//-----------------------------------------------------------------------------


#define PRINTDRIVER
#include <print.h>
#include "mdevice.h"
#include "gdidefs.inc"
#include "unidrv.h"
#include <memory.h>

#ifndef _INC_WINDOWSX
#include <windowsx.h>
#endif


// ---- M A C R O  D E F I N E ----
#define _CODESEG      "_CODESEG"
#define CODESEG    __based(__segname(_CODESEG))

#define CCHMAXCMDLEN                128
#define AIRBUFFLEN                  1024
#define SWAPW(x)                    (((WORD)(x)<<8) | ((WORD)(x)>>8))
#define FONT_HEADER_SIZE            0x86            // format type 2
#define SIZE_SYMBOLSET              28

#define PENCOLOR_WHITE              0
#define PENCOLOR_BLACK              1

#define TYPE_COLOR                  0
#define TYPE_WIDTH                  1

#define TYPE_FIRSTCHAR              0
#define TYPE_PROPFLAG               1

#define SET_BRUSH_DOWN(x) (lpEpage->wDownloadedBrushImage |= 0x01 << (x))
#define IS_DOWN_BRUSH(x)  (lpEpage->wDownloadedBrushImage & (0x01 << (x)))
#define ABS(x) ((x > 0)?(x):-(x))

// OutputCmd callback function ordinal
// Carousel
#define CAR_SELECT_PEN_COLOR        1
#define CAR_SET_PEN_WIDTH           2

// Brush
#define BRUSH_SELECT_HS_NULL        8
#define BRUSH_SELECT_HS_SOLID       9
#define BRUSH_SELECT_HS_HORIZONTAL  10
#define BRUSH_SELECT_HS_VERTICAL    11
#define BRUSH_SELECT_HS_FDIAGONAL   12
#define BRUSH_SELECT_HS_BDIAGONAL   13
#define BRUSH_SELECT_HS_CROSS       14
#define BRUSH_SELECT_HS_DIAGCROSS   15
#define BRUSH_CREATE_BIT_1          16
#define BRUSH_CREATE_END_1          17
#define BRUSH_SELECT_BRUSHSTYLE     18

// Text
#define TEXT_PRN_DIRECTION          20
#define TEXT_FS_SINGLE_BYTE         21
#define TEXT_FS_DOUBLE_BYTE         22

#define DOWNLOAD_SELECT_FONT_ID     23
#define DOWNLOAD_DELETE_FONT        24
#define DOWNLOAD_DELETE_ALLFONT     25

#define FS_BOLD_ON                  26
#define FS_BOLD_OFF                 27
#define FS_ITALIC_ON                28
#define FS_ITALIC_OFF               29

// Line joint
#define  ENDCAP_FLAT                40
#define  ENDCAP_ROUND               41
#define  ENDCAP_SQUARE              42
#define  JOIN_BEVEL                 43
#define  JOIN_MITER                 44
#define  JOIN_ROUND                 45

//Vector output
#define ELLIPSE                     50

// Line
#define LINE_SET_NULL               65
#define LINE_SET_SOLID              66
#define LINE_SET_DASH               67
#define LINE_SET_DOT                68
#define LINE_SET_DASHDOT            69
#define LINE_SET_DASHDOTDOT         70

#define CONTROL_BEGINDOC2           80
#define CONTROL_BEGINDOC3           81

// ---- S T R U C T U R E  D E F I N E ----
typedef struct
{
  BYTE  fGeneral;       // General purpose bitfield
  BYTE  bCmdCbId;       // Callback ID; 0 iff no callback
  WORD  wCount;         // # of EXTCD structures following
  WORD  wLength;        // length of the command
} CD, *PCD, FAR * LPCD;

typedef struct
{
  SIZEL sizlExtent;
  POINTFX  pfxOrigin;
  POINTFX  pfxCharInc;
} BITMAPMETRICS, FAR * LPBITMAPMETRICS;
typedef BYTE huge* LPDIBITS;

 typedef struct  {
    short       dfType;
    short       dfPoints;
    short       dfVertRes;
    short       dfHorizRes;
    short       dfAscent;
    short       dfInternalLeading;
    short       dfExternalLeading;
    BYTE        dfItalic;
    BYTE        dfUnderline;
    BYTE        dfStrikeOut;
    short       dfWeight;
    BYTE        dfCharSet;
    short       dfPixWidth;
    short       dfPixHeight;
    BYTE        dfPitchAndFamily;
    short       dfAvgWidth;
    short       dfMaxWidth;
    BYTE        dfFirstChar;
    BYTE        dfLastChar;
    BYTE        dfDefaultChar;
    BYTE        dfBreakChar;
    short       dfWidthBytes;
    DWORD       dfDevice;
    DWORD       dfFace;
    DWORD       dfBitsPointer;
    DWORD       dfBitsOffset;
    BYTE        dfReservedByte;
 } PFMHEADER, * PPFMHEADER, far * LPPFMHEADER;

typedef struct tagHEIGHTLIST {
    WORD    id;
    WORD    Height;
    WORD    Width;
    LPWORD  lpWidth;
    BYTE    fProp;
    BYTE    bFirstChar;
} HEIGHTLIST, FAR * LPHEIGHTLIST;

typedef struct tagEPAGEMDV {
    BYTE BrushImage[8];
    WORD wCurrentBrushImage;
    WORD wDownloadedBrushImage;
    WORD wCurrentDLFontID;
    WORD wPenWidth;
    WORD wEndCap;
    WORD wJoin;
    int  iPenColor;
    int  iSBCSX;
    int  iDBCSX;
    int  iSBCSXMove;
    int  iSBCSYMove;
    int  iDBCSXMove;
    int  iDBCSYMove;
    int  iEscapement;
    HEIGHTLIST HeightL[15];
    WORD wListNum;
    short sHeightMul;
    short sHeightDiv;
    short iDevCharOffset;
    WORD fGeneral;
    BYTE bCurrentCharSet;
    SHORT WidthTable[256];
    BYTE bFirstChar;
    LPWORD lpWidth;
} EPAGEMDV, FAR * LPEPAGEMDV;

// fGeneral flag
#define FLAG_DBCS        0x01
#define FLAG_VERT        0x02
#define FLAG_PROP        0x04
#define FLAG_DOUBLE      0x08
#define FLAG_NULL_PEN    0x10
#define FLAG_BOLD        0x20
#define FLAG_ITALIC      0x40

typedef struct {
    BYTE bFormat;
    BYTE bDataDir;
    WORD wCharCode;
    WORD wBitmapWidth;
    WORD wBitmapHeight;
    WORD wLeftOffset;
    WORD wAscent;
    DWORD CharWidth;
} ESCPAGECHAR;

typedef struct {
   WORD Integer;
   WORD Fraction;
} FRAC;

typedef struct {
   WORD wFormatType;
   WORD wDataSize;
   WORD wSymbolSet;
   WORD wCharSpace;
   FRAC CharWidth;
   FRAC CharHeight;
   WORD wFontID;
   WORD wWeight;  // Line Width
   WORD wEscapement;  // Rotation
   WORD wItalic;  // Slant
   WORD wLast;
   WORD wFirst;
   WORD wUnderline;
   WORD wUnderlineWidth;
   WORD wOverline;
   WORD wOverlineWidth;
   WORD wStrikeOut;
   WORD wStrikeOutWidth;
   WORD wCellWidth;
   WORD wCellHeight;
   WORD wCellLeftOffset;
   WORD wCellAscender;
   FRAC FixPitchWidth;
} ESCPAGEHEADER, FAR * LPESCPAGEHEADER;


#pragma code_seg("_DATASEG")
char *rgchModuleName = "EPAGEMSJ";

// ---- G L O B A L  V A L U A B L E  ----
short usHorzRes;
short usVertRes;
#ifdef DEBUG
char DebugBuff[512];
#endif


// ----- S T A T I C  D A T A ---
#pragma code_seg(_CODESEG)
WORD CODESEG AsciiTable[] = {
        0x8140, 0x8149, 0x8168, 0x8194,
        0x8190, 0x8193, 0x8195, 0x8166,
        0x8169, 0x816A, 0x8196, 0x817B,
        0x8143, 0x817C, 0x8144, 0x815E,
        0x824F, 0x8250, 0x8251, 0x8252,
        0x8253, 0x8254, 0x8255, 0x8256,
        0x8257, 0x8258, 0x8146, 0x8147,
        0x8183, 0x8181, 0x8184, 0x8148,
        0x8197, 0x8260, 0x8261, 0x8262,
        0x8263, 0x8264, 0x8265, 0x8266,
        0x8267, 0x8268, 0x8269, 0x826A,
        0x826B, 0x826C, 0x826D, 0x826E,
        0x826F, 0x8270, 0x8271, 0x8272,
        0x8273, 0x8274, 0x8275, 0x8276,
        0x8277, 0x8278, 0x8279, 0x816D,
        0x818F, 0x816E, 0x814F, 0x8151,
        0x814D, 0x8281, 0x8282, 0x8283,
        0x8284, 0x8285, 0x8286, 0x8287,
        0x8288, 0x8289, 0x828A, 0x828B,
        0x828C, 0x828D, 0x828E, 0x828F,
        0x8290, 0x8291, 0x8292, 0x8293,
        0x8294, 0x8295, 0x8296, 0x8297,
        0x8298, 0x8299, 0x829A, 0x816F,
        0x8162, 0x8170, 0x8150
};

WORD CODESEG KanaTable[] = {
        0x8140, 0x8142, 0x8175, 0x8176,
        0x8141, 0x8145, 0x8392, 0x8340,
        0x8342, 0x8344, 0x8346, 0x8348,
        0x8383, 0x8385, 0x8387, 0x8362,
        0x815B, 0x8341, 0x8343, 0x8345,
        0x8347, 0x8349, 0x834A, 0x834C,
        0x834E, 0x8350, 0x8352, 0x8354,
        0x8356, 0x8358, 0x835A, 0x835C,
        0x835E, 0x8360, 0x8363, 0x8365,
        0x8367, 0x8369, 0x836A, 0x836B,
        0x836C, 0x836D, 0x836E, 0x8371,
        0x8374, 0x8377, 0x837A, 0x837D,
        0x837E, 0x8380, 0x8381, 0x8382,
        0x8384, 0x8386, 0x8388, 0x8389,
        0x838A, 0x838B, 0x838C, 0x838D,
        0x838F, 0x8393, 0x814A, 0x814B
};

BYTE CODESEG ShiftJisEPAGE[256] = {
//     +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //00
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //10
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //20
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //30
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //40
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //50
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //60
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //70
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //80
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //90
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //A0
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //B0
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //C0
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //D0
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //E0
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0   //F0
};

int  CODESEG ESin[4] = { 0, 1, 0, -1 };
int  CODESEG ECos[4] = { 1, 0, -1, 0 };
BYTE CODESEG bit_mask[] = {0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};
BYTE CODESEG Vertical[128]= { 0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3 };

BYTE CODESEG Horizontal[128]={0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0xff,0xff,0xff,0xff,
                              0xff,0xff,0xff,0xff };

BYTE CODESEG BDiagonal[128] ={0,0,0,3,
                              0,0,0,6,
                              0,0,0,0x0c,
                              0,0,0,0x18,
                              0,0,0,0x30,
                              0,0,0,0x60,
                              0,0,0,0xc0,
                              0,0,1,0x80,
                              0,0,3,0,
                              0,0,6,0,
                              0,0,0x0c,0,
                              0,0,0x18,0,
                              0,0,0x30,0,
                              0,0,0x60,0,
                              0,0,0xc0,0,
                              0,1,0x80,0,
                              0,3,0,0,
                              0,6,0,0,
                              0,0xc,0,0,
                              0,0x18,0,0,
                              0,0x30,0,0,
                              0,0x60,0,0,
                              0,0xc0,0,0,
                              1,0x80,0,0,
                              3,0,0,0,
                              6,0,0,0,
                              0x0c,0,0,0,
                              0x18,0,0,0,
                              0x30,0,0,0,
                              0x60,0,0,0,
                              0xc0,0,0,0,
                              0x80,0,0,1,
                              };

BYTE CODESEG FDiagonal[128] ={0x80,0,0,1,
                              0xc0,0,0,0,
                              0x60,0,0,0,
                              0x30,0,0,0,
                              0x18,0,0,0,
                              0x0c,0,0,0,
                              6,0,0,0,
                              3,0,0,0,
                              1,0x80,0,0,
                              0,0xc0,0,0,
                              0,0x60,0,0,
                              0,0x30,0,0,
                              0,0x18,0,0,
                              0,0xc,0,0,
                              0,6,0,0,
                              0,3,0,0,
                              0,1,0x80,0,
                              0,0,0xc0,0,
                              0,0,0x60,0,
                              0,0,0x30,0,
                              0,0,0x18,0,
                              0,0,0x0c,0,
                              0,0,6,0,
                              0,0,3,0,
                              0,0,1,0x80,
                              0,0,0,0xc0,
                              0,0,0,0x60,
                              0,0,0,0x30,
                              0,0,0,0x18,
                              0,0,0,0x0c,
                              0,0,0,6,
                              0,0,0,3};

BYTE CODESEG Cross[128]=    { 0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0,0,0,3,
                              0xff,0xff,0xff,0xff,
                              0xff,0xff,0xff,0xff };

BYTE CODESEG DCross[128]    ={0xc0,0,   0,   0x03,
                              0x60,0,   0,   0x06,
                              0x30,0,   0,   0x0c,
                              0x18,0,   0,   0x18,
                              0x0c,0,   0,   0x30,
                              6,   0,   0,   0x60,
                              3,   0,   0,   0xc0,
                              1,   0x80,1,   0x80,
                              0,   0xc0,3,   0,
                              0,   0x60,6,   0,
                              0,   0x30,0x0c,0,
                              0,   0x18,0x18,0,
                              0,   0x0c,0x30,0,
                              0,   0x06,0x60,0,
                              0,   0x03,0xc0,0,
                              0,   1,   0x80,0,
                              0,   3,   0xc0,0,
                              0,   6,   0x60,0,
                              0,   0x0c,0x30,0,
                              0,   0x18,0x18,0,
                              0,   0x30,0x0c,0,
                              0,   0x60,0x06,0,
                              0,   0xc0,0x03,0,
                              1,   0x80,0x01,0x80,
                              3,   0,   0,   0xc0,
                              6,   0,   0,   0x60,
                              0x0c,0,   0,   0x30,
                              0x18,0,   0,   0x18,
                              0x30,0,   0,   0x0c,
                              0x60,0,   0,   0x06,
                              0xc0,0,   0,   0x03,
                              0x80,0,   0,   0x01
                              };

LPBYTE CODESEG Pattern[6] = { Horizontal,
                              Vertical,
                              FDiagonal,
                              BDiagonal,
                              Cross,
                              DCross        };

char CODESEG SET_FONT_ID[]        = "\x1D%d;%ddh{F";
char CODESEG SEND_CHAR_CODE[]     = "\x1D%d;%dsc{F";
char CODESEG DLI_SELECT_FONT_ID[] = "\x1D%ddcF\x1D\x30;-%dcoP";
char CODESEG DLI_DELETE_FONT[]    = "\x1D%dddcF";

char CODESEG SELECT_PEN_COLOR[]   = "\x1D\x31;0;%dspE";
char CODESEG SET_PEN_WIDTH[]      = "\x1D%d;%d;%dlwG";
char CODESEG SELECT_PEN_PATTERN[] = "\x1D\x31;1;%dspE";
char CODESEG SET_PEN_STYLE[]      = "\x1D\x30;%dlpG";
char CODESEG PenStyle[5]          = {0, 3, 1, 4, 5};

char CODESEG CREATE_BRUSH[]       = "\x1D\x31\x32\x38;%d;32;32sp{E";
char CODESEG SELECT_BRUSH[]       = "\x1D\x30;1;%dspE";
char CODESEG SELECT_BRUSH_SOLID[] = "\x1D\x30;0;%dspE";
//CODESEG char SELECT_BRUSH[]       = "\x1D\x30;1;%dspE\x1D\x31;1;%dspE";
char CODESEG SET_FILL_MODE[]      = "\x1D%ddmG";

char CODESEG FS_SINGLE_BYTE[]     = "\x1D\x31;0mcF\x1D%d;%dpP";
char CODESEG FS_DOUBLE_BYTE[]     = "\x1D\x31;1mcF\x1D%d;%dpP";
char CODESEG CHAR_PITCH[]         = "\x1D\x30spF\x1D%d;%dpP";
char CODESEG PRN_DIRECTION[]      = "\x1D%droF";
char CODESEG SET_CHAR_OFFSET[]    = "\x1D\x30;%dcoP";
char CODESEG SET_CHAR_OFFSET_XY[] = "\x1D%d;%dcoP";
#if 0
char CODESEG CHAR_SPACE[]         = "\x1D%dspF";
#endif
char CODESEG FONT_ATTR_SET[]      = "\x1D\x30;%dmcf";

char CODESEG POLY_ELLIPSE[]       = "\x1D%d;%d;%d;%d;0eG";

char CODESEG CHAR_OUT[]           = "\x1D\x31ppP%c\x1D\x32ppP\x1D%dH\x1D%dV";
char CODESEG VERT_FONT_SET[]      = "\x1D%dvpC\x1D%d\x76pC";

char CODESEG BOLD_SET[]           = "\x1D%dweF";
char CODESEG ITALIC_SET[]         = "\x1D%dslF";

char CODESEG BEGINDOC2[]          = "\x1B\x01@EJL\x20\x0A@EJL\x20\x45N\x20LA=ESC/PAGE\x0A\x1B\x01@EJL\x20\x0A@EJL\x20SE\x20LA=ESC/PAGE\x0A@EJL\x20SET\x20\x45\x43=ON\x20PU=1\x20ZO=OFF\x0A@EJL\x20\x45N\x20LA=ESC/PAGE\x0A\x1DrhE\x1D\x30;0.24muE\x1D\x31\x34isE\x1D\x32iaF\x1D\x31\x30ifF\x1D\x31ipP";
char CODESEG BEGINDOC3[]          = "\x1B\x01@EJL\x20\x0A@EJL\x20\x45N\x20LA=ESC/PAGE\x0A\x1B\x01@EJL\x20\x0A@EJL\x20SE\x20LA=ESC/PAGE\x0A@EJL\x20SET\x20\x45\x43=ON\x20PU=1\x20ZO=OFF\x20RS=FN\x0A@EJL\x20\x45N\x20LA=ESC/PAGE\x0A\x1DrhE\x1D\x30;0.24muE\x1D\x31\x34isE\x1D\x32iaF\x1D\x31\x30ifF\x1D\x31ipP";
#ifdef DEBUG
char CODESEG DEBUG_1[]            = "bFormat      =%u(%x)\nbDataDir     =%u(%x)\nwCharCode    =%u(%x)\nwBitmapHeight=%u(%x)\nwBitmapWidth=%u(%x)\nwLeftOffset  =%u(%x)\nwAscent      =%u(%x)\nCharWidth    =%lu(%x)\n";
char CODESEG DEBUG_2[]            = "dwCellSize=%u\n\n";
char CODESEG DEBUG_3[]            = "(rcID=%d)lpString = %s\n";
char CODESEG DEBUG_4[]            = "CharSet = %d\nDLFontID = %d\nfGeneral = %d\n";
char CODESEG DEBUG_5[]            = "CharWidth[%x]=%4d CharWidth[%x]=%4d CharWidth[%x]=%4d CharWidth[%x]=%4d\n";
char CODESEG DEBUG_6[]            = "Font[%d]:wCharWidth[%x] = %4d\n";
#endif

// ---- F U N C T I O N  P R O T O T Y P E ----
VOID NEAR PASCAL SetPenStyle(LPDV lpdv, WORD wType);
WORD NEAR CODESEG PASCAL SJis2Jis(WORD usCode);
VOID NEAR CODESEG PASCAL EPAGECopy(DWORD dst, DWORD src, WORD cnt);
LPWORD NEAR CODESEG PASCAL lpGetWidthTable(LPDV lpdv, WORD id);

short FAR PASCAL OEMOutputChar(LPDV lpdv, LPSTR lpstr, WORD len, WORD rcID);

//----------------------------*IsDBCSLeadByte*-------------------------------
//
//---------------------------------------------------------------------------
BYTE NEAR CODESEG __loadds IsDBCSLeadByteEPAGE(BYTE Ch)
{
return ShiftJisEPAGE[Ch];
}

//----------------------------*Ank2Jis*--------------------------------------
//
//---------------------------------------------------------------------------

WORD NEAR CODESEG __loadds Ank2Jis(BYTE bAnk)
{
WORD wSJis;

    if (bAnk < 0xa0)
        wSJis =  AsciiTable[bAnk-0x20];
    else
        wSJis =  KanaTable[bAnk-0xA0];

    return SJis2Jis(wSJis);
}

//----------------------------*OEMTTBitmap*----------------------------------
// Action: Determine whether we should print as downloaded TT or else grx.
//         If the font is a DBCS TT font -> print as grx
//---------------------------------------------------------------------------

BOOL FAR CODESEG PASCAL OEMTTBitmap(lpdv, lpFont, lpDrawMode)
LPDV    lpdv;
LPFONTINFO lpFont;
LPDRAWMODE  lpDrawMode;
{
#if 0
    if(lpFont->dfCharSet == SHIFTJIS_CHARSET)
        return TRUE;
    else
#endif
        // The character whose height exceeds 64K can't be printed.
        if(lpFont->dfPixHeight > 256)
            return TRUE;
        else
            return FALSE;
        return TRUE;
}


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
short FAR CODESEG PASCAL OEMScaleWidth(width, masterUnits, newHeight, vRes, hRes)
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
short NEAR CODESEG PASCAL itoa(buf, n)
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
VOID FAR CODESEG PASCAL OEMSendScalableFontCmd(lpdv, lpcd, lpFont)
LPDV    lpdv;
LPCD    lpcd;     // offset to the command heap
LPFONTINFO lpFont;
{
    LPEPAGEMDV lpEpage;
    LPSTR   lpcmd;
    short   ocmd;
    WORD    wlen;
    WORD    i;
    BYTE    rgcmd[CCHMAXCMDLEN];    // build command here
    BYTE    ch[128];

    if (!lpcd || !lpFont)
        return;

    lpEpage = lpdv->lpMdv;

    // Initialize lpEpage
    lpEpage->bCurrentCharSet = lpFont->dfCharSet;
    if (lpFont->dfCharSet == 0x80)
        lpEpage->fGeneral |= FLAG_DOUBLE;
    else
        lpEpage->fGeneral &= ~FLAG_DOUBLE;
    lpEpage->fGeneral &=  ~FLAG_BOLD;
    lpEpage->fGeneral &=  ~FLAG_ITALIC;

    if('@' == *((LPSTR)lpFont+lpFont->dfFace))
        lpEpage->fGeneral |= FLAG_VERT;
    else
        lpEpage->fGeneral &= ~FLAG_VERT;

    if (lpFont->dfPitchAndFamily & 0x01)
        lpEpage->fGeneral |= FLAG_PROP;
    else
        lpEpage->fGeneral &= ~FLAG_PROP;

    if(lpEpage->fGeneral & FLAG_PROP)
    {
        lpEpage->bFirstChar = lpFont->dfFirstChar;
        EPAGECopy((DWORD)lpEpage->WidthTable,
                  (DWORD)((LPSTR)lpFont+sizeof(PFMHEADER)),
                  lpFont->dfLastChar - lpFont->dfFirstChar);
    }

    // be careful about integer overflow.
    lpcmd = (LPSTR)(lpcd+1);
    ocmd = 0;
    lpEpage->fGeneral &= ~FLAG_DBCS;

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
            unsigned long tmpWidth;

            if (lpcmd[i+2] == 'S')
            {
                tmpWidth = (unsigned long)lpFont->dfAvgWidth;
                lpEpage->fGeneral |= FLAG_DBCS;
            }
            else
            if (lpcmd[i+2] == 'D')
            {
                tmpWidth = (unsigned long)lpFont->dfAvgWidth*2;
                lpEpage->fGeneral |= FLAG_DBCS;
            }
            else
            {
                if (lpFont->dfPixWidth)
                    tmpWidth = (unsigned long)lpFont->dfPixWidth;
                else
                    tmpWidth = (unsigned long)lpFont->dfAvgWidth;

                i--;
            }

            ocmd += itoa((LPSTR)&rgcmd[ocmd],
            (short)((unsigned long)lpFont->dfHorizRes * 1000 / tmpWidth) /1000);
            rgcmd[ocmd] = '.';
            ocmd ++;
            ocmd += itoa((LPSTR)&rgcmd[ocmd],
            (short)((unsigned long)lpFont->dfHorizRes * 1000 / tmpWidth) %1000 /10);
            i += 3;

        }
        else
            rgcmd[ocmd++] = lpcmd[i++];

    WriteSpoolBuf(lpdv, (LPSTR) rgcmd, ocmd);
    lpEpage->iDevCharOffset = (lpFont->dfPixHeight-lpFont->dfAscent)
                            * lpEpage->sHeightMul / lpEpage->sHeightDiv;
    wlen = wsprintf(ch, SET_CHAR_OFFSET,
                    (lpEpage->fGeneral & FLAG_DBCS)?lpEpage->iDevCharOffset:0);

    if (lpEpage->fGeneral & FLAG_VERT)
    {
        wlen += wsprintf(&ch[wlen], VERT_FONT_SET, 1, 1);
    }
    WriteSpoolBuf(lpdv, ch, wlen);
    lpEpage->wCurrentDLFontID = 0;

    // save for FS_SINGLE_BYTE and FS_DOUBLE_BYTE
    if (lpEpage->fGeneral & FLAG_DBCS)
    {
        lpEpage->iSBCSX =
        lpEpage->iSBCSXMove =
           lpFont->dfAvgWidth * lpEpage->sHeightMul / lpEpage->sHeightDiv;
        lpEpage->iDBCSX =
        lpEpage->iDBCSXMove =
           lpFont->dfMaxWidth * lpEpage->sHeightMul / lpEpage->sHeightDiv;
    }
    else
    {
        WORD tmpWidth = lpFont->dfPixWidth;

        if (!tmpWidth)
            tmpWidth = lpFont->dfAvgWidth;

        lpEpage->iSBCSX =
        lpEpage->iSBCSXMove =
                       tmpWidth * lpEpage->sHeightMul / lpEpage->sHeightDiv;
        lpEpage->iDBCSX = lpEpage->iDBCSXMove = 0;
    }
    lpEpage->iSBCSYMove = lpEpage->iDBCSYMove = 0;

}

VOID NEAR CODESEG PASCAL InsertHeightList(
LPDV lpdv,
WORD id,
WORD wHeight,
WORD wWidth,
BYTE fProp,
BYTE bFirstChar)
{
    LPEPAGEMDV   lpEpage = lpdv->lpMdv;
    LPHEIGHTLIST lpHeightList = lpEpage->HeightL;

    if (!lpEpage->lpWidth)
        lpEpage->lpWidth      = (LPWORD) GlobalAllocPtr(GHND, 8192);
                                                     //256 * 2 * 16

    lpHeightList         += lpEpage->wListNum;
    lpHeightList->id      = id;
    lpHeightList->Height  = wHeight;
    lpHeightList->Width   = wWidth;
    lpHeightList->fProp   = fProp;
    lpHeightList->bFirstChar = bFirstChar;
    lpHeightList->lpWidth = (lpEpage->lpWidth+lpEpage->wListNum * 256);

    lpEpage->wListNum++;
}


BYTE NEAR CODESEG PASCAL GetByteData(
LPDV lpdv,
WORD id,
WORD Type)
{
    LPEPAGEMDV   lpEpage = lpdv->lpMdv;
    LPHEIGHTLIST lpHeightList = lpEpage->HeightL;
    WORD i;
    BYTE bRet;

    for (i = 0; i < lpEpage->wListNum; i++, lpHeightList++)
        if (lpHeightList->id == id)
        {
            if (Type == TYPE_FIRSTCHAR)
                bRet = lpHeightList->bFirstChar;
            else
                bRet = lpHeightList->fProp;
            break;
        }

    return bRet;

}

WORD NEAR CODESEG PASCAL GetWidth(
LPDV lpdv,
WORD id)
{
    LPEPAGEMDV   lpEpage = lpdv->lpMdv;
    LPHEIGHTLIST lpHeightList = lpEpage->HeightL;
    WORD i, wRet;

    for (i = 0; i < lpEpage->wListNum; i++, lpHeightList++)
        if (lpHeightList->id == id)
        {
            wRet = lpHeightList->Width;
            break;
        }

    return wRet;

}

WORD NEAR CODESEG PASCAL GetHeight(
LPDV lpdv,
WORD id)
{
    LPEPAGEMDV   lpEpage = lpdv->lpMdv;
    LPHEIGHTLIST lpHeightList = lpEpage->HeightL;
    WORD i, wRet;

    for (i = 0; i < lpEpage->wListNum; i++, lpHeightList++)
        if (lpHeightList->id == id)
        {
            wRet = lpHeightList->Height;
            break;
        }

    return wRet;

}

LPWORD NEAR CODESEG PASCAL lpGetWidthTable(
LPDV lpdv,
WORD id)
{
    LPEPAGEMDV   lpEpage = lpdv->lpMdv;
    LPHEIGHTLIST lpHeightList = lpEpage->HeightL;
    LPWORD       lpWidth;
    WORD i;

    for (i = 0; i < lpEpage->wListNum; i++, lpHeightList++)
        if (lpHeightList->id == id)
        {
            lpWidth = lpHeightList->lpWidth;
            break;
        }
    return lpWidth;
}

VOID NEAR CODESEG PASCAL SetWidth(
LPDV lpdv,
WORD id,
WORD wCharCode,
WORD wCharWidth)
{
    LPEPAGEMDV   lpEpage = lpdv->lpMdv;
    LPHEIGHTLIST lpHeightList = lpEpage->HeightL;
    WORD i;

    for (i = 0; i < lpEpage->wListNum; i++, lpHeightList++)
        if (lpHeightList->id == id)
        {
            LPWORD lpWidth = lpHeightList->lpWidth;
            if (!lpWidth)
                break;

            *(lpWidth+wCharCode-lpHeightList->bFirstChar)  = wCharWidth*4;
            break;
        }
    lpEpage->WidthTable[wCharCode-lpHeightList->bFirstChar] = wCharWidth*4;
}
//-----------------------------------------------------------------------------
// void OEMDownloadFontHeader(lpdv, lpFont, id)
// LPDV        lpdv;
// LPFONTINFO  lpFont;     // as defined in "gdidefs.inc".
// WORD        id;         // primary font id. (id+1) for the secondary font.
//
// Function: download font headers for the given TT font.
//
//
//-----------------------------------------------------------------------------


VOID FAR CODESEG PASCAL OEMDownloadFontHeader(
    LPDV       lpdv,
    LPFONTINFO lpFont,
    WORD       id)
{

    ESCPAGEHEADER FontHeader;
    BYTE sFontName[54];
    BYTE Buff[32];
    int  iSizeOfBuf,iSizeFontName;
    LPEPAGEMDV lpEpage = (LPEPAGEMDV)lpdv->lpMdv;

    if(lpFont->dfPitchAndFamily & 0x01 || lpFont->dfCharSet == 0x80)
        lpEpage->fGeneral |= FLAG_PROP;
    else
        lpEpage->fGeneral &= ~FLAG_PROP;
    lpEpage->bCurrentCharSet = lpFont->dfCharSet;
    lpEpage->bFirstChar = (lpFont->dfType&PF_GLYPH_INDEX)?0:lpFont->dfFirstChar;
    lpEpage->iSBCSX=
           lpFont->dfAvgWidth * lpEpage->sHeightMul / lpEpage->sHeightDiv;
    if (lpFont->dfCharSet == 0x80)
        lpEpage->iDBCSX=
           2 * lpFont->dfAvgWidth * lpEpage->sHeightMul / lpEpage->sHeightDiv;
    else
        lpEpage->iDBCSX=  0;

    InsertHeightList(lpdv,
                     id,
                     lpFont->dfPixHeight
                          * ((LPEPAGEMDV)lpdv->lpMdv)->sHeightMul
                          / ((LPEPAGEMDV)lpdv->lpMdv)->sHeightDiv,
                     lpFont->dfAvgWidth
                          * ((LPEPAGEMDV)lpdv->lpMdv)->sHeightMul
                          / ((LPEPAGEMDV)lpdv->lpMdv)->sHeightDiv,
                     (BYTE)(lpEpage->fGeneral & FLAG_PROP),
                     //(lpFont->dfPitchAndFamily & 0x01),
                     lpEpage->bFirstChar);

    FontHeader.wFormatType     = SWAPW(0x0002);
    FontHeader.wDataSize       = SWAPW(0x0086);
    FontHeader.wSymbolSet      = SWAPW(id+0x4000); //id + 4000h
    if (lpEpage->fGeneral & FLAG_PROP)
    {
        FontHeader.wCharSpace         = SWAPW(1);
        FontHeader.CharWidth.Integer = 0;
        FontHeader.CharWidth.Fraction = 0;
    }
    else
    {
        FontHeader.wCharSpace         = 0;
        FontHeader.CharWidth.Integer  = SWAPW(lpFont->dfAvgWidth);
        FontHeader.CharWidth.Fraction = 0;
    }
    FontHeader.CharHeight.Integer
                               = SWAPW(lpFont->dfPixHeight);
    FontHeader.CharHeight.Fraction
                               = 0;
    // in the range 128 - 255
    FontHeader.wFontID         = SWAPW(id - 512 + (id < 0x80 ? 0x80 : 0x00));
    FontHeader.wWeight         = 0;
    FontHeader.wEscapement     = 0;
    FontHeader.wItalic         = 0;
    FontHeader.wFirst          = 0;
    FontHeader.wLast           = SWAPW (255);
    FontHeader.wUnderline      = 0;
    FontHeader.wUnderlineWidth = SWAPW(10);
    FontHeader.wOverline       = 0;
    FontHeader.wOverlineWidth  = 0;
    FontHeader.wStrikeOut      = 0;
    FontHeader.wStrikeOutWidth = 0;
    FontHeader.wCellWidth      = SWAPW(lpFont->dfPixWidth);
    FontHeader.wCellHeight     = SWAPW(lpFont->dfPixHeight);
    FontHeader.wCellLeftOffset = 0;
    FontHeader.wCellAscender   = SWAPW(lpFont->dfAscent+lpFont->dfPixHeight);
    FontHeader.FixPitchWidth.Integer  = SWAPW(lpFont->dfAvgWidth);
    FontHeader.FixPitchWidth.Fraction = 0;


    iSizeFontName = wsprintf(sFontName,
           "________________________EPSON_ESC_PAGE_DOWNLOAD_FONT%02d",id-512);
    iSizeOfBuf = wsprintf(Buff,SET_FONT_ID,FONT_HEADER_SIZE,id-512);
    WriteSpoolBuf(lpdv, Buff, iSizeOfBuf);
    WriteSpoolBuf(lpdv, (LPSTR)&FontHeader,sizeof(ESCPAGEHEADER));
    WriteSpoolBuf(lpdv, sFontName,iSizeFontName);
    WriteSpoolBuf(lpdv, "EPC_PAGE_DOWNLOAD_FONT_INDEX", SIZE_SYMBOLSET);

    iSizeOfBuf = wsprintf(Buff,DLI_SELECT_FONT_ID,id-512,GetHeight(lpdv, id));
    WriteSpoolBuf(lpdv, Buff, iSizeOfBuf);
    ((LPEPAGEMDV)lpdv->lpMdv)->wCurrentDLFontID = id;

}


//-----------------------------------------------------------------------------
// void OEMDownloadChar(lpdv, lpFont, id, cp, wCharWidth,lpbmm, lpBitmap, dwBmpSize)
// LPDV           lpdv;       // PDEVICE
// LPFONTINFO      lpFont;
// WORD           id;         // id of the font that this char belongs to.
// WORD            cp;                  // character to be downloaded
// WORD            wCharWidth;          // char width from width table
// LPBITMAPMETRICS lpbmm;               // metrics of this TT character bitmap
// LPDIBITS        lpBitmap;            // bitmap data of this TT character
// DWORD              dwBmpSize;                // size of the character bitmap (bytes)
//
// Function: compose and download the character definition data for the
// given character. Output the data directly via WriteSpoolBuf.
//-----------------------------------------------------------------------------

VOID FAR CODESEG PASCAL OEMDownloadChar(
    LPDV            lpdv,
    LPFONTINFO      lpFont,
    WORD            id,         // primary font id. id+1 for secondary font
    WORD            cp,
    WORD            wCharWidth,
    LPBITMAPMETRICS lpbmm,
    LPDIBITS        lpBitmap,
    DWORD           dwBmpSize)
{
    ESCPAGECHAR     ESCPageChar;
    WORD            wWidth, wWidthTT;
    LPDIBITS        lpDst, lpSrc;
    BYTE            mask;
    int             iSizeOfBuf, i;
    unsigned int    j;
    DWORD           dwSize,dwCellSize, dwAirSize;
    BYTE            CellBuffer[AIRBUFFLEN] = {0};
    LPBYTE          lpCellBuffer;
    LPEPAGEMDV      lpEpage = lpdv->lpMdv;
    BYTE            Buff[32];

#ifdef DEBUG
    wsprintf(DebugBuff, DEBUG_6,id,cp,wCharWidth);
    OutputDebugString(DebugBuff);
#endif

    SetWidth(lpdv, id, cp, wCharWidth);

    // fill in the charcter header information.
    ESCPageChar.bFormat       = 0x01;
    ESCPageChar.bDataDir      = 0x10;
    ESCPageChar.wCharCode     = LOBYTE(cp);

    ESCPageChar.wBitmapHeight = SWAPW(lpFont->dfPixHeight);
    ESCPageChar.wLeftOffset   = SWAPW(lpbmm->pfxOrigin.x.value);
    ESCPageChar.wAscent       = SWAPW(lpbmm->pfxOrigin.y.value - lpbmm->sizlExtent.cy);
    ESCPageChar.CharWidth     = SWAPW(lpbmm->pfxCharInc.x.value);

  // There is a slight difference between the ESC/Page download format of a
  // bitmap and the TrueType bitmap: ESC/Page scanlines are byte-aligned and
  // TrueType scanlines are dword-aligned.  Thus there are 0-3 extra
  // bytes at the end of each scanline.

   wWidth = (LOWORD(lpbmm->sizlExtent.cx) + 7) >> 3;
   wWidthTT = ((LOWORD(lpbmm->sizlExtent.cx) + 31) / 32) * 4;
   if (wWidth != wWidthTT)
   {
       lpDst = lpSrc = lpBitmap;

       if (dwBmpSize > 0x10000)
           // must check segment boundary
           for (j = 0; j < LOWORD(lpbmm->sizlExtent.cy); j++)
           {
               if ((LONG)LOWORD(lpDst) + wWidth > 0x10000
                   || (LONG)LOWORD(lpSrc) + wWidth > 0x10000)
               {
                   WORD k;

                   for (k = 0; k < wWidth; k++)
                       *lpDst++ = *lpSrc++;
                    lpSrc += (wWidthTT - wWidth);
               }
               else
               {
                   _fmemcpy(lpDst, lpSrc, wWidth);
                   lpDst += wWidth;
                   lpSrc += wWidthTT;
               }
           }
       else
           // fast code for smaller gliphs
           for (j = 0; j < LOWORD(lpbmm->sizlExtent.cy); j++)
           {
               _fmemcpy(lpDst, lpSrc, wWidth);
               lpDst += wWidth;
               lpSrc += wWidthTT;
           }
    }

    // not multiple of 8, need to mask out unused last byte
    // This is done so that we do not advance beyond segment bound
    // which can happen if lpBitmap is just under 64K and adding
    // width to it will cause invalid segment register to be loaded.
    if (mask = bit_mask[LOWORD(lpbmm->sizlExtent.cx) & 0x7])
    {
        lpSrc = lpBitmap + wWidth - 1;
        i = LOWORD(lpbmm->sizlExtent.cy);
        while (TRUE)
        {
            (*lpSrc) &= mask;
            i--;
            if (i > 0)
                lpSrc += wWidth;
            else
                break;
        }
    }
    ESCPageChar.wBitmapWidth  = SWAPW(wWidth<<3);

    // Calculate the size of the font 'cell', so that we can determine how
    // many scanlines of air need to be sent i.e to adjust baseline.
    // ESC/Page requires that you send down a 'padded' bitmap, we cannot fool
    // the PDL by re-arranging the parameters from the GDI realized bitmap

    dwCellSize = (DWORD)lpFont->dfPixHeight * wWidth;
    dwSize = (DWORD)(LOWORD(lpbmm->sizlExtent.cy)) * wWidth;
    dwAirSize = dwCellSize - dwSize;    // ie font cell - bitmap data

    if(!dwSize)  // space char, download empty char with correct extent
    {
        ESCPageChar.wBitmapHeight = SWAPW(1);
        ESCPageChar.wBitmapWidth  = SWAPW(lpbmm->pfxCharInc.x.value);
        dwAirSize = dwCellSize    = lpbmm->pfxCharInc.x.value >> 3;

        *lpBitmap                 = 0;
    }


    if (id != lpEpage->wCurrentDLFontID)
    {
        lpEpage->wCurrentDLFontID = id;

        iSizeOfBuf=wsprintf(Buff,DLI_SELECT_FONT_ID,id-512,GetHeight(lpdv, id));
        WriteSpoolBuf(lpdv, Buff, iSizeOfBuf);
        lpEpage->iSBCSX = GetWidth(lpdv, id);
        lpEpage->iDBCSX = 0;
    }

    iSizeOfBuf = wsprintf(Buff,"\x1D%d;",dwCellSize + sizeof(ESCPAGECHAR));
    WriteSpoolBuf(lpdv, Buff, iSizeOfBuf);
    iSizeOfBuf = wsprintf(Buff,"%dsc{F",LOBYTE(cp));
    WriteSpoolBuf(lpdv, Buff, iSizeOfBuf);

    WriteSpoolBuf(lpdv, (LPSTR)&ESCPageChar, sizeof(ESCPAGECHAR));

#ifdef DEBUG
    iSizeOfBuf = wsprintf(DebugBuff,DEBUG_1,
    ESCPageChar.bFormat,         ESCPageChar.bFormat,
    ESCPageChar.bDataDir,        ESCPageChar.bDataDir,
    ESCPageChar.wCharCode,       ESCPageChar.wCharCode,
    ESCPageChar.wBitmapHeight,   ESCPageChar.wBitmapHeight,
    ESCPageChar.wBitmapWidth,    ESCPageChar.wBitmapWidth,
    ESCPageChar.wLeftOffset,     ESCPageChar.wLeftOffset,
    ESCPageChar.wAscent,         ESCPageChar.wAscent,
    ESCPageChar.CharWidth,       ESCPageChar.CharWidth);
    OutputDebugString(DebugBuff);
    wsprintf(DebugBuff, DEBUG_2,dwCellSize);
    OutputDebugString(DebugBuff);
#endif
    // write those blank scanlines ... this is terrrible, but...

    if ( dwAirSize <= AIRBUFFLEN )
        WriteSpoolBuf(lpdv, (LPSTR)CellBuffer,(WORD)dwAirSize );
    else        // greater than 1K of air needed
       {
       // Use the 1K from the stack and grab the rest as needed
       WriteSpoolBuf(lpdv, (LPSTR)CellBuffer,AIRBUFFLEN );
       lpCellBuffer = GlobalAllocPtr(GHND,(WORD)(dwAirSize - AIRBUFFLEN));
       WriteSpoolBuf(lpdv, (LPSTR)lpCellBuffer,(WORD)(dwAirSize - AIRBUFFLEN));
       GlobalFreePtr (lpCellBuffer);
       }

    for (lpDst = lpBitmap; dwSize; lpDst += wWidth)
    {
        if ( dwSize > 0x4000 )
            wWidth = 0x4000;
        else
            wWidth = LOWORD(dwSize);

        dwSize -= wWidth;

        WriteSpoolBuf(lpdv, (LPSTR)lpDst, (WORD)wWidth);
    }
}

//-------------------------------------------------------------------
// OEMOutputCmd
// Action :
//-------------------------------------------------------------------


VOID FAR CODESEG PASCAL OEMOutputCmd(
LPDV     lpdv,
WORD     wCmdCbId,
LPDWORD  lpdwParams)
{
    LPEPAGEMDV lpEpage;
    int        iPenColor, i, j;
    BYTE       ch[256];
    WORD       wlen = 0;
    WORD       wBrushStyle;
#ifdef DEBUG
    short      iSizeOfBuf;
#endif

    lpEpage = lpdv->lpMdv;

    switch(wCmdCbId)
    {
    case CAR_SELECT_PEN_COLOR: // (1)
        switch(*lpdwParams)
        {
        case PENCOLOR_WHITE:
            iPenColor = 0;
            break;

        case PENCOLOR_BLACK:
        default:
            iPenColor = 100;
            break;
        }
        lpEpage->iPenColor = iPenColor;
        SetPenStyle(lpdv, TYPE_COLOR);
        break;

    case CAR_SET_PEN_WIDTH: // (2)
        lpEpage->wPenWidth = (WORD)*lpdwParams;
        SetPenStyle(lpdv, TYPE_WIDTH);
        break;

// Brush Pattern
// 0 - 5  Horizontal to Diacross
// 6 - 13 Created brush (8)

    case BRUSH_SELECT_HS_HORIZONTAL: // (10) ID for Brush 0
    case BRUSH_SELECT_HS_VERTICAL:   // (11) ID for Brush 1
    case BRUSH_SELECT_HS_FDIAGONAL:  // (12) ID for Brush 2
    case BRUSH_SELECT_HS_BDIAGONAL:  // (13) ID for Brush 3
    case BRUSH_SELECT_HS_CROSS:      // (14) ID for Brush 4
    case BRUSH_SELECT_HS_DIAGCROSS:  // (15) ID for Brush 5
        wBrushStyle = wCmdCbId - BRUSH_SELECT_HS_HORIZONTAL;
        if (!IS_DOWN_BRUSH(wCmdCbId - BRUSH_SELECT_HS_HORIZONTAL))
        {
            wlen = wsprintf(ch, CREATE_BRUSH, wBrushStyle);
            WriteSpoolBuf(lpdv, ch, wlen);

            WriteSpoolBuf(lpdv,Pattern[wBrushStyle],128);
            SET_BRUSH_DOWN(wBrushStyle);
        }
        wlen = wsprintf(ch, SELECT_BRUSH, wBrushStyle);
        wlen += wsprintf(&ch[wlen], SET_FILL_MODE, 2);
        WriteSpoolBuf(lpdv, ch, wlen);
        break;

    case  BRUSH_CREATE_BIT_1: // (16)
        lpEpage->BrushImage[lpEpage->wCurrentBrushImage/8]
            |= (BYTE)(0x0001 & *lpdwParams) << (lpEpage->wCurrentBrushImage%8);
        lpEpage->wCurrentBrushImage ++;
        break;

    case  BRUSH_CREATE_END_1: // (17)
        for (j = 0; j < 32; j ++)
            for (i = 0; i < 4; i ++)
                    ch[(j<<2) + i] = lpEpage->BrushImage[j % 8];
        WriteSpoolBuf(lpdv, (LPSTR)ch, CCHMAXCMDLEN);

        lpEpage->wCurrentBrushImage = 0;
        for (i = 0; i < 8; i++)
            lpEpage->BrushImage[i] = 0;
        break;

    case BRUSH_SELECT_BRUSHSTYLE: // (18)
        wlen = wsprintf(ch, SELECT_BRUSH, *(LPWORD)lpdwParams+5);
        wlen += wsprintf(&ch[wlen], SET_FILL_MODE, 2);
        WriteSpoolBuf(lpdv, (LPSTR)ch, wlen);
        break;

    case  TEXT_PRN_DIRECTION: // (20)
        {
            int   iEsc90;

            lpEpage->iEscapement = (int)*lpdwParams;
            iEsc90 = lpEpage->iEscapement/90;

            wlen = wsprintf(ch,"\x1D%dpmP",iEsc90?1:0);

            lpEpage->iSBCSXMove =  lpEpage->iSBCSX * ECos[ iEsc90 ];
            lpEpage->iSBCSYMove = -lpEpage->iSBCSX * ESin[ iEsc90 ];
            if (lpEpage->fGeneral & FLAG_DBCS)
            {
                lpEpage->iDBCSXMove =  lpEpage->iDBCSX * ECos[ iEsc90 ];
                lpEpage->iDBCSYMove = -lpEpage->iDBCSX * ESin[ iEsc90 ];
                wlen += wsprintf(&ch[wlen], SET_CHAR_OFFSET_XY,
                                 lpEpage->iDevCharOffset * ESin[ iEsc90 ],
                                 lpEpage->iDevCharOffset * ECos[ iEsc90 ]);
            }
            else
            if (lpEpage->wCurrentDLFontID && lpEpage->iEscapement)
            {
                WORD wHeight = GetHeight(lpdv, lpEpage->wCurrentDLFontID);
                short sXMove, sYMove;

                sXMove = -(short)wHeight * ESin[iEsc90];
                sYMove = -(short)wHeight * ECos[iEsc90];
                wlen += wsprintf(&ch[wlen], SET_CHAR_OFFSET_XY, sXMove,sYMove);

            }
            else
                wlen += wsprintf(&ch[wlen], SET_CHAR_OFFSET, 0);

            if (!(lpEpage->fGeneral & (FLAG_DBCS|FLAG_PROP))
            || (    (lpEpage->fGeneral & FLAG_DBCS)
                && !(lpEpage->fGeneral & FLAG_DOUBLE))
            ){
                wlen += wsprintf(&ch[wlen],CHAR_PITCH,lpEpage->iSBCSXMove,
                                                      lpEpage->iSBCSYMove);
            } else if ((FLAG_DBCS|FLAG_DOUBLE)
                        == (lpEpage->fGeneral & (FLAG_DBCS|FLAG_DOUBLE)))
            {
                wlen += wsprintf(&ch[wlen],CHAR_PITCH,lpEpage->iDBCSXMove,
                                                  lpEpage->iDBCSYMove);
            }
            wlen += wsprintf(&ch[wlen],PRN_DIRECTION,lpEpage->iEscapement);
            WriteSpoolBuf(lpdv, (LPSTR)ch, wlen);
        }
        break;

    case FS_BOLD_ON:
        lpEpage->fGeneral |=  FLAG_BOLD;
        goto SetBold;
    case FS_BOLD_OFF:
        lpEpage->fGeneral &=  ~FLAG_BOLD;

    SetBold:
        wlen = wsprintf(ch,BOLD_SET, (wCmdCbId==FS_BOLD_ON)?3:0);
        WriteSpoolBuf(lpdv, (LPSTR)ch, wlen);
        break;

    case FS_ITALIC_ON:
        lpEpage->fGeneral |=  FLAG_ITALIC;
        goto SetItalic;
    case FS_ITALIC_OFF:
        lpEpage->fGeneral &=  ~FLAG_ITALIC;

    SetItalic:
        wlen = wsprintf(ch,ITALIC_SET, (wCmdCbId==FS_ITALIC_ON)?346:0);
        WriteSpoolBuf(lpdv, (LPSTR)ch, wlen);
        break;

    case TEXT_FS_SINGLE_BYTE: // (21)
        wlen = wsprintf(ch,FS_SINGLE_BYTE,lpEpage->iSBCSXMove,
                                          lpEpage->iSBCSYMove);
        wlen += wsprintf(&ch[wlen],PRN_DIRECTION,lpEpage->iEscapement);
        if (lpEpage->fGeneral & FLAG_VERT)
        {
            wlen += wsprintf(&ch[wlen], VERT_FONT_SET, 0, 0);
        }
        lpEpage->fGeneral &= ~FLAG_DOUBLE;
        wlen += wsprintf(&ch[wlen],BOLD_SET,
                         (lpEpage->fGeneral & FLAG_BOLD)?15:0);
        wlen += wsprintf(&ch[wlen],ITALIC_SET,
                         (lpEpage->fGeneral & FLAG_ITALIC)?346:0);
        WriteSpoolBuf(lpdv, ch, wlen);
        break;

    case TEXT_FS_DOUBLE_BYTE: // (22)
        wlen = wsprintf(ch,FS_DOUBLE_BYTE,lpEpage->iDBCSXMove,
                                          lpEpage->iDBCSYMove);
        wlen += wsprintf(&ch[wlen],PRN_DIRECTION,lpEpage->iEscapement);
        if (lpEpage->fGeneral & FLAG_VERT)
        {
            wlen += wsprintf(&ch[wlen], VERT_FONT_SET, 1, 1);
        }
        lpEpage->fGeneral |= FLAG_DOUBLE;
        wlen += wsprintf(&ch[wlen],BOLD_SET,
                         (lpEpage->fGeneral & FLAG_BOLD)?15:0);
        wlen += wsprintf(&ch[wlen],ITALIC_SET,
                         (lpEpage->fGeneral & FLAG_ITALIC)?346:0);
        WriteSpoolBuf(lpdv, ch, wlen);
        break;

    case DOWNLOAD_SELECT_FONT_ID: // (23)
        lpEpage->bCurrentCharSet = 0;
        lpEpage->wCurrentDLFontID = (WORD)*lpdwParams;
        lpEpage->fGeneral &= ~(FLAG_DBCS | FLAG_VERT | FLAG_DOUBLE);
        if(GetByteData(lpdv,(WORD)*lpdwParams,TYPE_PROPFLAG))
            lpEpage->fGeneral |= FLAG_PROP;
        else
            lpEpage->fGeneral &= ~FLAG_PROP;

        wlen = wsprintf(ch, DLI_SELECT_FONT_ID,
                            (WORD)*lpdwParams-512,
                            GetHeight(lpdv, (WORD)*lpdwParams));
        WriteSpoolBuf(lpdv, (LPSTR)ch, wlen);
        lpEpage->bFirstChar = GetByteData(lpdv,(WORD)*lpdwParams,TYPE_FIRSTCHAR);
        if(lpEpage->fGeneral & FLAG_PROP)
        {
            EPAGECopy((DWORD)lpEpage->WidthTable,
                      (DWORD)lpGetWidthTable(lpdv, (WORD)*lpdwParams),
                      256);
        }
        else
        {
            lpEpage->iSBCSX = GetWidth(lpdv, lpEpage->wCurrentDLFontID);
            lpEpage->iDBCSX = 0;
        }
#ifdef DEBUG
       iSizeOfBuf = wsprintf(DebugBuff,DEBUG_4,
                             lpEpage->bCurrentCharSet,
                             lpEpage->wCurrentDLFontID,
                             lpEpage->fGeneral);
       OutputDebugString(DebugBuff);
       for (i = 0; i < 64; i ++)
       {
           iSizeOfBuf = wsprintf(DebugBuff, DEBUG_5,
                                 4*i, lpEpage->WidthTable[4*i],
                                 4*i+1, lpEpage->WidthTable[4*i+1],
                                 4*i+2, lpEpage->WidthTable[4*i+2],
                                 4*i+3, lpEpage->WidthTable[4*i+3]);
           OutputDebugString(DebugBuff);
       }
#endif
        break;

    case DOWNLOAD_DELETE_FONT:    // (24)
        wlen = wsprintf(ch, DLI_DELETE_FONT, (WORD)*lpdwParams-512);
        WriteSpoolBuf(lpdv, (LPSTR)ch, wlen);
        break;

    case DOWNLOAD_DELETE_ALLFONT: // (25)
        for (i = 0; i < (int)lpEpage->wListNum ; i++)
        {
            wlen = wsprintf(ch, DLI_DELETE_FONT,
                               (WORD)lpEpage->HeightL[i].id-512);
            WriteSpoolBuf(lpdv, (LPSTR)ch, wlen);
            lpEpage->HeightL[i].id = 0;
        }
        lpEpage->wListNum = 0;
        break;

    case ENDCAP_FLAT: // (40)
    case ENDCAP_ROUND:  // (41)
    case ENDCAP_SQUARE: // (42)
        lpEpage->wEndCap = (wCmdCbId - ENDCAP_FLAT);
        break;
    case JOIN_BEVEL: // (43)
        lpEpage->wJoin = 3;
        break;
    case JOIN_MITER: // (44)
        lpEpage->wJoin = 2;
        break;
    case JOIN_ROUND: // (45)
        lpEpage->wJoin = 1;
        break;

    case ELLIPSE: // (50)
        {
        POINT P0, // center
              P1;

            P0.x = ((WORD)*lpdwParams + (WORD)*(lpdwParams+2))/2;
            P0.y = ((WORD)*(lpdwParams+1) + (WORD)*(lpdwParams+3))/2;
            P1.x = ABS((short)*lpdwParams-(short)P0.x);
            P1.y = ABS((short)*(lpdwParams+1)-(short)P0.y);

            wlen = wsprintf(ch, POLY_ELLIPSE, P0.x, P0.y, P1.x, P1.y);
            WriteSpoolBuf(lpdv, (LPSTR)ch, wlen);

        }
        break;

    case LINE_SET_NULL: // (65)
        lpEpage->fGeneral |= FLAG_NULL_PEN;
        goto SetStyle;

    case LINE_SET_SOLID:      // (66)
    case LINE_SET_DOT:        // (67)
    case LINE_SET_DASH:       // (68)
    case LINE_SET_DASHDOT:    // (69)
    case LINE_SET_DASHDOTDOT: // (70)
        lpEpage->fGeneral &= ~FLAG_NULL_PEN;
        wlen = wsprintf(ch, SET_PEN_STYLE, PenStyle[wCmdCbId-LINE_SET_SOLID]);
        WriteSpoolBuf(lpdv, ch, wlen);

    SetStyle:
        SetPenStyle(lpdv, TYPE_COLOR);
        break;

    case CONTROL_BEGINDOC2:   // (80)
    case CONTROL_BEGINDOC3:   // (81)
        wlen = wsprintf(ch, (wCmdCbId==CONTROL_BEGINDOC2)?BEGINDOC2:BEGINDOC3);
        WriteSpoolBuf(lpdv, ch, wlen);
        break;


    }
}

//-------------------------------------------------------------------
// Function: Enable()
// Action  : call UniEnable and setup Mdv
//-------------------------------------------------------------------

short CALLBACK Enable(
LPDV lpdv,
WORD style,
LPSTR lpModel,
LPSTR lpPort,
LPDM lpStuff)
{
    CUSTOMDATA    cd;
    short         sRet, i;
    short          sTmpMul, sTmpDiv;

    cd.cbSize = sizeof(CUSTOMDATA);
    cd.hMd = GetModuleHandle((LPSTR)rgchModuleName);
    cd.fnOEMDump = NULL;
    cd.fnOEMOutputChar = (LPFNOEMOUTPUTCHAR) OEMOutputChar;

    sRet = UniEnable(lpdv, style, lpModel, lpPort, lpStuff, &cd);

    if (style == 0x0000 && sRet != 0)
    {
        lpdv->fMdv = FALSE;
        if (!(lpdv->lpMdv = GlobalAllocPtr(GHND,sizeof(EPAGEMDV))))
        {
            UniDisable( lpdv );
            return FALSE;
        }
        lpdv->fMdv = TRUE;
        ((LPEPAGEMDV)lpdv->lpMdv)->lpWidth               = NULL;
        ((LPEPAGEMDV)lpdv->lpMdv)->wListNum              = 0;
        ((LPEPAGEMDV)lpdv->lpMdv)->wCurrentBrushImage    = 0;
        ((LPEPAGEMDV)lpdv->lpMdv)->wDownloadedBrushImage = 0;
        if (usHorzRes == 600)
            {
            sTmpMul = 1;
            sTmpDiv = 2;
            }
        else if (usHorzRes == 300)
            {
            sTmpMul = 1;
            sTmpDiv = 1;
            }
        else if (usHorzRes == 240)
            {
            sTmpMul = 5;
            sTmpDiv = 4;
            }

        ((LPEPAGEMDV)lpdv->lpMdv)->sHeightMul     =  sTmpMul;
        ((LPEPAGEMDV)lpdv->lpMdv)->sHeightDiv     =  sTmpDiv;
        for (i = 0; i < 8; i++)
            ((LPEPAGEMDV)lpdv->lpMdv)->BrushImage[i]     = 0;
        ((LPEPAGEMDV)lpdv->lpMdv)->iPenColor             = 0;
        ((LPEPAGEMDV)lpdv->lpMdv)->wPenWidth             = 0;
        ((LPEPAGEMDV)lpdv->lpMdv)->wJoin                 = 0;
        ((LPEPAGEMDV)lpdv->lpMdv)->wEndCap               = 0;

    } else
    if( style == 0x0001)
    {
        //INQUIREINFO
        usHorzRes = ((LPGDIINFO)lpdv)->dpAspectX;
        usVertRes = ((LPGDIINFO)lpdv)->dpAspectY;

    }
    return sRet;
}

//-------------------------------------------------------------------
// OEMGetFontCmd
// Action: Dummy
//-------------------------------------------------------------------
short CALLBACK OEMGetFontCmd(
LPDV     lpdv,
WORD     wCmdCbId,
LPSTR    lpfont,
BOOL     fSelect,
LPBYTE   lpBuf,
LPWORD   lpwSize)
{
return (TRUE);
}

//-------------------------------------------------------------------
// Function: Disable()
// Action  : free Mdv and call Mdv
//-------------------------------------------------------------------
VOID FAR CODESEG PASCAL Disable(lpdv)
LPDV lpdv;
{
    if (lpdv->fMdv)
    {
        if (((LPEPAGEMDV)lpdv->lpMdv)->lpWidth)
            GlobalFreePtr( ((LPEPAGEMDV)lpdv->lpMdv)->lpWidth);
        GlobalFreePtr (lpdv->lpMdv);
        lpdv->fMdv = FALSE;
    }

    UniDisable(lpdv);
}

VOID NEAR CODESEG PASCAL SetPenStyle(
LPDV lpdv,
WORD wType)
{
    WORD wlen = 0;
    LPEPAGEMDV lpEpage = lpdv->lpMdv;
    BYTE ch[128];

    if (lpEpage->fGeneral & FLAG_NULL_PEN)
    {
        wlen = wsprintf(ch, SELECT_PEN_COLOR, 0);
    }
    else
    {
        switch(wType)
        {
        case TYPE_COLOR:
            wlen = wsprintf(ch, SELECT_PEN_COLOR, lpEpage->iPenColor);
            break;

        case TYPE_WIDTH:
            wlen = wsprintf(ch, SET_PEN_WIDTH, lpEpage->wPenWidth,
                                               lpEpage->wEndCap,
                                               lpEpage->wJoin);
            break;
        }
    }
    WriteSpoolBuf(lpdv,(LPSTR)ch, wlen);
}

short FAR CODESEG PASCAL OEMOutputChar( lpdv, lpstr, len, rcID )
LPDV    lpdv;
LPSTR   lpstr;
WORD    len;
WORD    rcID;
{
    WORD       wlen;
    WORD       i;
    WORD       wTmpChar;
    short      sRetSize;
    LPSTR      lpTmpStr;
    LPEPAGEMDV lpEpage = lpdv->lpMdv;
    BYTE       ch[512];
    BYTE       bChar, sPoint;
#ifdef DEBUG
    short      iSizeOfBuf;
#endif

#ifdef DEBUG
    iSizeOfBuf = wsprintf(DebugBuff,DEBUG_3, rcID, lpstr);
    OutputDebugString(DebugBuff);
#endif
    sRetSize = wlen = 0;
    lpTmpStr = lpstr;

    if (lpEpage->bCurrentCharSet == 0x80)
    {
        for(i = 0; i < len;i ++)
        {
            if(IsDBCSLeadByteEPAGE(*lpTmpStr))
            {
                if(!(lpEpage->fGeneral & FLAG_DOUBLE))
                {
                    WriteSpoolBuf(lpdv, ch, wlen);
                    OEMOutputCmd(lpdv, TEXT_FS_DOUBLE_BYTE, NULL);
                    sRetSize += wlen;
                    wlen = 0;
                }
                wTmpChar  = SJis2Jis(SWAPW((WORD)*(LPWORD)lpTmpStr));
                *(LPWORD)(ch+wlen) = SWAPW(wTmpChar);
                wlen+=2;
                lpTmpStr+=2;
                i++;
            }
            else
            {
                if(lpEpage->fGeneral & FLAG_DOUBLE)
                {
                    WriteSpoolBuf(lpdv, ch, wlen);
                    OEMOutputCmd(lpdv, TEXT_FS_SINGLE_BYTE, NULL);
                    sRetSize += wlen;
                    wlen = 0;
                }
                wTmpChar = Ank2Jis(*lpTmpStr);
                *(LPWORD)(ch+wlen) = SWAPW(wTmpChar);
                wlen+=2;
                lpTmpStr++;
            }
        }
        WriteSpoolBuf(lpdv, ch, wlen);
        sRetSize += wlen;
    }
    else if ((lpEpage->fGeneral & FLAG_PROP) && lpEpage->iEscapement)
    {
        short XMove, YMove;
        BYTE bFirstChar = lpEpage->bFirstChar;

        switch(lpEpage->iEscapement)
        {
        case 90:
            YMove = -1;
            XMove = 0;
            break;
        case 180:
            YMove = 0;
            XMove = -1;
            break;
        case 270:
            YMove = 1;
            XMove = 0;
            break;
        default:
            YMove = 0;
            XMove = -1;
            break;
        }
        if (rcID)
        {
            for (i = 0; i < len; i ++, lpstr++)
            {
                bChar = *(LPBYTE)lpstr;
                sPoint = bChar - bFirstChar;
                if (sPoint < 0  || sPoint > 255)
                    sPoint = bFirstChar;
                wlen = wsprintf(ch, CHAR_OUT, bChar,
                          XMove * lpEpage->WidthTable[sPoint]
                                / 4,
                          YMove * lpEpage->WidthTable[sPoint]
                                / 4 );
                WriteSpoolBuf(lpdv, ch, wlen);
                sRetSize += wlen;
            }
        }
        else
        {
            for (i = 0; i < len; i ++, lpstr++)
            {
                bChar = *(LPBYTE)lpstr;
                sPoint = bChar - bFirstChar;
                if (sPoint < 0  || sPoint > 255)
                    sPoint = bFirstChar;
                wlen = wsprintf(ch, CHAR_OUT, bChar,
                               (lpEpage->sHeightMul
                                * lpEpage->WidthTable[sPoint]
                                / 4 / lpEpage->sHeightDiv) * XMove,
                               (lpEpage->sHeightMul
                                * lpEpage->WidthTable[sPoint]
                                / 4 / lpEpage->sHeightDiv) * YMove);
                WriteSpoolBuf(lpdv, ch, wlen);
                sRetSize += wlen;
            }
        }
    }
    else
    {
        WriteSpoolBuf(lpdv, lpstr, len);
        sRetSize += len;
    }

    return sRetSize;

}

//-----------------------------------------------------------------------------
// Action:  Copy
//-----------------------------------------------------------------------------
//       Copy:   copy 'cnt' number of Bytes from 'src' to 'dst'
#pragma optimize("eg",off)
VOID NEAR CODESEG PASCAL EPAGECopy(
DWORD dst,
DWORD src,
WORD cnt)
{
   _asm
       {
        push    bp
        mov     bx, di          ; faster to save in registers
        mov     dx, si
        mov     ax, ds

        cld

        les     di, dst
        lds     si, src
        mov     cx, cnt
        mov     bp, cx          ; store count (bp no longer needed)
        shr     cx, 1

        rep     movsw

        mov     cx, bp
        and     cx, 1

        rep     movsb

        mov     di, bx          ; restore registers
        mov     si, dx
        mov     ds, ax
        pop     bp

       }
}

WORD NEAR CODESEG PASCAL SJis2Jis(WORD usCode)
{
    unsigned short usRetCode;

    _asm {
        mov     ax,usCode       ; Entry AH = first byte of Shift JIS
                                ;       AL = second byte of Shift JIS
        cmp     al,80h          ;if (al >= 0x80)
        cmc                     ;       then al=al-1 ;
        sbb     al,0            ;
        sub     al,(40h-21h)    ;al=al-(0x40-0x21) ;
        and     ah,10111111b    ;ah-=(ah<0xe0) ? 0 : 0xe0-0xa0 ;
        shl     ah,1            ;ah=(ah-0x81)*2 + 0x21 ;
        add     ah,21h-2h       ;
        cmp     al,7eh          ;if (al <= 0x7e)
        jbe     SJIStoJIS20     ;       then ;
        sub     al,(7fh-21h)    ;       else { al=al-(0x7f-0x21) ;
        inc     ah              ;              ah++ ; }
SJIStoJIS20:                    ;
        mov     usRetCode,ax    ; Exit  AH = first byte of Shift JIS
                                ;       AL = second byte of Shift JIS
    }
        return  usRetCode;
}
