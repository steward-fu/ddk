/*==========================================================================
 *
 *      Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *      File:           dddrv.c
 *      Content:        S3 DirectDraw code
 *
 ***************************************************************************/

#pragma code_seg("_DCISEG")

#include <windows.h>
#include "ddrawi.h"
#include "s3_ddtk.h"
#include "s3data.h"
#include "ddhook.h"     //### VL
#include "dibeng.inc"

#define MAX_NUM_HEAPS 4
#define MIN_RECT_SIZE   32
#define DSTWIN_SIZES     5

#define OVERLAY_DESTROYED   0x4026
#define NOTIFY_MODE_CHANGE  0x4027

extern DWORD FAR CDECL CallProc32ExW(DWORD num_params, DWORD dwFlags, DWORD Callback32, ...);
extern DWORD FAR PASCAL MapSL(LPVOID);
extern LPVOID FAR PASCAL MapLS(DWORD);
extern void   FAR PASCAL UnMapLS(LPVOID);
extern WORD FAR PASCAL VFDQuerySel( void );
extern DWORD FAR PASCAL VFDBeginLinearAccess( void );
extern DWORD FAR PASCAL VFDEndLinearAccess( void );
extern DWORD FAR PASCAL VFDQueryBase( void );
extern DWORD FAR PASCAL LinearAddressOf( DWORD );
extern void PASCAL K2V_WriteCrReg(BYTE byIndex, BYTE byValue);
extern BYTE PASCAL K2V_ReadCrReg(BYTE byIndex);
extern void PASCAL K2V_WriteSrReg(BYTE byIndex, BYTE byValue);
extern BYTE PASCAL K2V_ReadSrReg(BYTE byIndex);
extern int __loadds FAR PASCAL InitK2vTable( void );
extern DWORD __loadds FAR PASCAL GetK2vRatio( WORD deviceType);
extern DWORD __loadds FAR PASCAL GetK2vFifo( WORD deviceType, WORD wSize);
extern WORD FAR PASCAL FixCursorColor( void );
extern WORD FAR PASCAL RestoreCursorColor( void );
extern WORD FAR PASCAL SetDispStart( void );
extern WORD FAR PASCAL Switch2HWCursor( void );
extern K2VTABLE k2vTable[MAX_DEVICE];
extern PBAND    pBand;

extern DWORD dwDeviceHandle;     // in drv2.c, ==1 primary, >1 secondary adapter

extern WORD PASCAL wBusType;    //from vga.asm
extern BYTE PASCAL CurCR36;   //from vga.asm
extern BYTE PASCAL OrgCR36;   //from init.asm
extern BYTE PASCAL BWFlag;         //from init.asm
extern BYTE PASCAL P_CONTROL_FLG;  //from init.asm
extern DWORD     PASCAL dwColorAdjustFlags; //from control.asm
extern DWORD     PASCAL dwColorAdjustReg;   //from control.asm
extern BYTE PASCAL bShowSSWindow;               //hwcursor.asm
extern BYTE PASCAL bFamilyFlags;
extern BYTE PASCAL bStreamProcessorFlags;
#define BUS_PCI 3

BYTE bCR63=0xff;
BYTE bCR6D=0xff;
BYTE SP_SUPPORTED;
BYTE CR6D_80;
DWORD mmr8208;

void FAR PASCAL TurnOnStreamsProcessorMode( void );
void FAR PASCAL TurnOffStreamsProcessorMode( void );
void FAR PASCAL DisableStreamProcessor_Mobile( void );
static void FixMissingPixels( void );
static void UnfixMissingPixels( void );

LPDDHAL_SETINFO lpDDHAL_SetInfo;

BYTE PASCAL bDDrawActive = FALSE;


#define RoundUp(x,m)    ( (((x) % (m)) == 0) ? (x) : ((((x) / (m)) + 1) * (m)) )

#define INPUT_STATUS_1_REG 0x3da
#define VSYNC_MASK 0x08            // bit set => vertical retrace
#define IN_VSYNC (mmdwGetRegister(0x83da) & 0x00000008)
#define WAIT_FOR_VSYNC { while(!(((DIBENGINE FAR *)lpDriverPDevice)->deFlags & BUSY) && !IN_VSYNC); }
//###
#define WAIT_NO_VSYNC { while( !(((DIBENGINE FAR *)lpDriverPDevice)->deFlags & BUSY) && IN_VSYNC ); }

void FAR PASCAL dwMMOUT( WORD, WORD, DWORD );
DWORD FAR PASCAL dwMMIN( WORD, WORD);
void  FAR PASCAL dwAddMMOUT( WORD, DWORD, DWORD );
//###
void FAR PASCAL wait_virge_idle( void );

#define mmdwSetRegister( x, y ) dwMMOUT( wMMIOSelector, x, y )
#define mmdwGetRegister( x ) dwMMIN( wMMIOSelector, x)
#define CRTC_REG 0x3d4
#define GP_STAT 0x9ae8
#define HDW_BUSY 0x0200
//###
//#define wait_for_idle()               { while(inp( GP_STAT ) & HDW_BUSY); }
#define wait_for_idle()         { wait_virge_idle(); }

#ifdef DEBUG
#define START_STR      "S3v: "
#define END_STR         "\r\n"
#define DPF             Msg
void __cdecl Msg( LPSTR szFormat, ... );

/*
 * Msg
 *
 * outputs a debug string.
 */
void __cdecl Msg( LPSTR szFormat, ... )
{
    static int (WINAPI *fpwvsprintf)(LPSTR lpszOut, LPCSTR lpszFmt, const void FAR* lpParams);
    char        str[256];
    HANDLE      hnd;

    if( fpwvsprintf == NULL )
    {
        hnd = GetModuleHandle("USER");
        if( hnd == NULL )
        {
            return;
        }
        fpwvsprintf = (LPVOID) GetProcAddress(hnd, "wvsprintf");
        if( fpwvsprintf == NULL )
        {
            return;
        }
    }

#ifdef  DRVNAME
    lstrcpy(str, DRVNAME ": ");
#else
    lstrcpy( str, START_STR );
#endif
    fpwvsprintf( str+lstrlen( str ), szFormat, (LPVOID)(&szFormat+1) );
    lstrcat( str, END_STR );
    OutputDebugString( str );

} /* Msg */

#define BKPT            _asm {int 3}

#else
#define DPF             1 ? (void)0 : (void)
#define BKPT            {}
#endif

#define NUMENTRIES(x)   (sizeof(x)/sizeof(x[0]))
#define PIXEL_FORMATTER_PRESENT ( sData.dwSpecialHW & PIXEL_FORMATTER )
#define STREAMS_PROCESSOR_PRESENT ( sData.dwSpecialHW & STREAMS_PROCESSOR )
#define BIMODAL_HW_PRESENT ( sData.dwSpecialHW & BIMODAL_HW )
#define CAN_READ_SCANLINE  ( sData.dwSpecialHW & READ_SCANLINE )

/*
 * external driver state variables
 */

extern DWORD     PASCAL dwBlendCtrl;
extern DWORD     PASCAL dwFlipOffset;
extern DWORD     PASCAL dwClipOffset;
extern DWORD     PASCAL dwPanelSize;
extern DWORD     PASCAL dwSrcWH;
extern DWORD     PASCAL dwSSXY;
extern DWORD     PASCAL dwTVCenteringAdj;
extern DWORD     PASCAL dwSSWH;
extern DWORD     PASCAL dwSSAddr;
extern DWORD     PASCAL dwVideoType;
extern DWORD     PASCAL SavedSPFIFOReg;
extern DWORD     PASCAL dwColorAdjustFlags;
extern DWORD     PASCAL dwColorAdjustReg;
extern DWORD     PASCAL dwPSAddr;
extern DWORD     PASCAL dwPSAddr2;
extern WORD  PASCAL MoveHWCursorFn;
extern WORD  PASCAL wAbsX;
extern WORD  PASCAL wAbsY;
extern WORD  PASCAL wLogResolution;
extern WORD  PASCAL wPhysResolution;
extern WORD  PASCAL wPanelResolution;
extern WORD  PASCAL wCenterX;
extern WORD  PASCAL wCenterY;
extern WORD  PASCAL wVWTop;
extern WORD  PASCAL wVWLeft;
extern WORD  PASCAL wVWBottom;
extern WORD  PASCAL wVWRight;
extern WORD  PASCAL wVWTop1;
extern WORD  PASCAL wVWLeft1;
extern WORD  PASCAL wVWBottom1;
extern WORD  PASCAL wVWRight1;
extern WORD  PASCAL wVWTop2;
extern WORD  PASCAL wVWLeft2;
extern WORD  PASCAL wVWBottom2;
extern WORD  PASCAL wVWRight2;
extern WORD      PASCAL wScreenWidth;
extern WORD      PASCAL wScreenHeight;
extern WORD      PASCAL wPhysScreenWidth;
extern WORD      PASCAL wPhysScreenHeight;
extern WORD      PASCAL wScreenWidthBytes;
extern WORD      PASCAL wBpp;
extern DWORD     PASCAL dwScreenFlatAddr;
extern DWORD     PASCAL dwVideoMemorySize;
extern DWORD     PASCAL dwTotalMemorySize;
extern DWORD     PASCAL dwDRAMSize;
extern WORD  PASCAL CurrentChipMode;
extern WORD      PASCAL KernelsScreenSel;
extern WORD      PASCAL wPDeviceFlags;
extern LPVOID    PASCAL lpDriverPDevice;
extern WORD      PASCAL wMMIOSelector;
extern WORD      PASCAL wChipId;
extern WORD      PASCAL wDeviceId;
extern BYTE      PASCAL bRevisionId;
extern BYTE  PASCAL bShiftPelsToBytes;
extern SCREENRGN PASCAL Rgn1;
extern SCREENRGN PASCAL Rgn2;
extern SCREENRGN PASCAL Rgn3;
extern SCREENRGN PASCAL Rgn4;           //### for VX
extern BYTE      PASCAL bInDDraw;
extern WORD    PASCAL wMULT_MISC;
extern S3MOBILEDATA PASCAL S3MobileData;
extern S3DRIVERDATA PASCAL S3DriverData;

#define MMRSelector KernelsScreenSel


DWORD   dwRealScreenFlatAddr;
BOOL    bUsingVFlatD;
DWORD   k2vRatioOverride;

#define NUMENTRIES1 (sizeof(modeInfo1)/sizeof(modeInfo1[0]))
static DDHALMODEINFO modeInfo1[] = {
    {  320, 200,  320,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  320, 200,  640, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  320, 200,  960, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  320, 240,  320,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  320, 240,  640, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  320, 240,  960, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  400, 300,  400,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  400, 300,  800, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  400, 300, 1200, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  512, 384,  512,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  512, 384, 1024, 16,                                                   0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  512, 384, 1536, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  640, 400,  640,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  640, 400, 1280, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  640, 400, 1920, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  640, 480,  640,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  640, 480, 1280, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  640, 480, 1920, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  720, 480,  720,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  720, 480, 1440, 16,             0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  720, 480, 2160, 24,                        0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  800, 600,  800,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  800, 600, 1600, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  800, 600, 2400, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { 1024, 768, 1024,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1024, 768, 2048, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    { 1024, 768, 2400, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { 1152, 864, 1152,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1280,1024, 1280,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1280,1024, 2560, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    { 1600,1200, 1600,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
};

#define NUMENTRIES_VX   (sizeof(modeInfo_VX)/sizeof(modeInfo_VX[0]))
static DDHALMODEINFO modeInfo_VX[] = {
    {  320, 200,  320,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  320, 200,  640, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  320, 200,  960, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  320, 240,  320,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  320, 240,  640, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  320, 240,  960, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  400, 300,  400,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  400, 300,  800, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  400, 300, 1200, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  512, 384,  512,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  512, 384, 1024, 16,                                                   0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  512, 384, 1536, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  640, 400,  640,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  640, 400, 1280, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  640, 400, 1920, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  640, 480,  640,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  640, 480, 1280, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  640, 480, 1920, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  800, 600,  800,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  800, 600, 1600, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  800, 600, 2400, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { 1024, 768, 1024,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1024, 768, 2048, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    { 1024, 768, 2400, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { 1152, 864, 1152,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1280,1024, 1280,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1280,1024, 2560, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    { 1280,1024, 3840, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { 1600,1200, 1600,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1600,1200, 3200, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    { 1600,1200, 4800, 24,                                                0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
};

//M5 modes supported by BIOS
#define NUMENTRIES2 (sizeof(modeInfo2)/sizeof(modeInfo2[0]))
static DDHALMODEINFO modeInfo2[] = {
    {  320, 200,  320,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  320, 200,  640, 16,             0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  320, 240,  320,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  320, 240,  640, 16,             0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  400, 300,  400,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  400, 300,  800, 16,                     0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
     {  512, 384,  512, 8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
     {  512, 384, 1024, 16,                           0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  640, 400,  640,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  640, 400, 1280, 16,             0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  640, 480,  640,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  640, 480, 1280, 16,             0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  640, 480, 1920, 24,                        0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    {  800, 600,  800,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    {  800, 600, 1600, 16,             0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    {  800, 600, 2400, 24,                        0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { 1024, 768, 1024,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1024, 768, 2048, 16,             0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    { 1024, 768, 2400, 24,                        0, 0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { 1280,1024, 1280,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1280,1024, 2560, 16,             0, 0, 0x00007C00, 0x000003e0, 0x0000001f, 0x00000000 },
    { 1280, 480, 1280,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1280, 480, 2560, 16,             0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1600, 600, 1600,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1600, 600, 3200, 16,             0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1600, 1200, 1600,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 1600, 1200, 3200, 16,             0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 2048, 768, 2048,  8, DDMODEINFO_PALETTIZED, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 2048, 768, 4096, 16,                     0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
};


static DDHALMODEINFO *modeInfo;
int num_entry;

#define CRTC_VBLANK_INT_EN                      0x2     //S3?
#define CRTC_VBLANK_INT                         0x4     //S3?

/*
 * pre-declare our HAL fns
 */
DWORD __loadds FAR PASCAL HALDestroyDriver( LPDDHAL_DESTROYDRIVERDATA );

/*
 * video memory pool usage
 */
static VIDMEM vidMem [] = {
  { VIDMEM_ISLINEAR, 0x00000000, 0x00000000,
                { 0 },
                { 0 } },
  { VIDMEM_ISLINEAR, 0x00000000, 0x00000000,
                { 0 },
                { 0 } },
//### for VX DRAM
  { VIDMEM_ISLINEAR, 0x00000000, 0x00000000,
                { 0 }, { 0 } }
};

/*
 * callbacks from the DIRECTDRAW object
 */
static DDHAL_DDCALLBACKS cbDDCallbacks = {
    sizeof ( DDHAL_DDCALLBACKS ),
    0,                          // dwFlags
    HALDestroyDriver,           // DestroyDriver
    NULL,                       // CreateSurface
    NULL,                       // SetColorKey
    NULL,                       // SetMode
    NULL,                       // WaitForVerticalBlank -> uses 32-bit
    NULL,                       // CanCreateSurface
    NULL,                       // CreatePalette
    NULL                        // lpReserved1
};

/*
 * callbacks from the DIRECTDRAWSURFACE object
 */
static DDHAL_DDSURFACECALLBACKS cbDDSurfaceCallbacks =
{
    sizeof ( DDHAL_DDSURFACECALLBACKS ),
    0,                          // dwFlags
    NULL,                       // DestroySurface
    NULL,                       // Flip -> uses 32-bit
    NULL,                       // SetClipList
    NULL,                       // Lock -> uses 32-bit
    NULL,                       // Unlock
    NULL,                       // Blt -> uses 32-bit
    NULL,                       // SetColorKey
    NULL,                       // AddAttachedSurface
    NULL,                       // lpReserved
    NULL,                       // lpReserved
    NULL,                       // UpdateOverlay
    NULL,                       // lpReserved
    NULL,                       // lpReserved
    NULL                        // SetPalette
};

/*
 * callbacks from the DIRECTDRAWPALETTE object
 */
static DDHAL_DDPALETTECALLBACKS cbDDPaletteCallbacks =
{
    sizeof ( DDHAL_DDPALETTECALLBACKS ),
    0,                          // dwFlags
    NULL,                       // DestroyPalette
    NULL                        // SetEntries
};

/*
 * bits indicating which ROPs is supported
 *         DWORD 0, bit 0  = ROP 0
 *         DWORD 8, bit 31 = ROP 255
 */
static DWORD ropsSupported [ DD_ROP_SPACE ] =
{
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000
};

static WORD s3ropsSupported[] =
{
    SRCCOPY >> 16 //,
/*
    NOTSRCCOPY >> 16,
    SRCPAINT >> 16,
    SRCAND >> 16,
    SRCINVERT >> 16,
    DSTINVERT >> 16,
    MERGEPAINT >> 16,
    NOTSRCERASE >> 16,
    SRCERASE >> 16,
    PATCOPY >> 16,
    PATINVERT >> 16,
    BLACKNESS >> 16,
    WHITENESS >> 16,
*/
};

/*
 * misc variables
 */
S3THKINFO      sData;
#define ddHALInfo       sData.ddHALInfo

/*
 * build array for supported ROPS
 */
static void setupRops(WORD *pSupportedRop, DWORD *pRopTable, int cnt)
{
    int         x;
    DWORD       idx;
    DWORD       bit;
    DWORD       rop;

    for( x=0; x<cnt; x++ )
    {
        rop = LOBYTE(pSupportedRop[x]);
        idx = rop / 32;
        bit = 1L << ((DWORD)(LOBYTE(rop) % 32));
        pRopTable[idx] |= bit;
    }

} /* setupRops */

/*
 * buildPixelFormat
 *
 * build DDPIXELFORMAT structure
 */
static void buildPixelFormat( LPDDHALMODEINFO lpMode, LPDDPIXELFORMAT lpddpf )
{
    lpddpf->dwFlags = DDPF_RGB;
    lpddpf->dwRGBBitCount = lpMode->dwBPP;

    if( lpMode->wFlags & DDMODEINFO_PALETTIZED)
    {
        lpddpf->dwFlags  |= DDPF_PALETTEINDEXED8;
    }

    lpddpf->dwRBitMask = lpMode->dwRBitMask;
    lpddpf->dwGBitMask = lpMode->dwGBitMask;
    lpddpf->dwBBitMask = lpMode->dwBBitMask;
    lpddpf->dwRGBAlphaBitMask = lpMode->dwAlphaBitMask;

} /* buildPixelFormat */

/*
 * buildDDHALInfo
 *
 * build DDHALInfo structure
 */
static void buildDDHALInfo( int modeidx )
{
    static DWORD        AlignTbl [ 9 ] = { 64, 64, 64, 64, 64, 64, 64, 64, 64 };
//    static DWORD      AlignTbl [ 9 ] = { 8, 8, 8, 8, 16, 8, 24, 8, 32 };
    int                 ii;
    BOOL                can_flip;
    WORD                heap;
    WORD                bytes_per_pixel;
    static DWORD    dwpFOURCCs[3];

    DWORD pGbl, pHAL;
    LPDDHAL_DDEXEBUFCALLBACKS pExeBuf;

    pGbl = ddHALInfo.lpD3DGlobalDriverData;
    pHAL = ddHALInfo.lpD3DHALCallbacks;
    pExeBuf = ddHALInfo.lpDDExeBufCallbacks;

    // dont trash the 3D callbacks. only the 2D info.
    memset(&ddHALInfo, 0, sizeof (ddHALInfo));

//    if(wBpp >=16 ){//fix page fault in screen saver Biology_lewis #SPR13476
        ddHALInfo.lpD3DHALCallbacks = pHAL;
//        ddHALInfo.lpDDExeBufCallbacks = pExeBuf;
        ddHALInfo.lpD3DGlobalDriverData = pGbl;
//    }
      //clean 3d callback pointers on 8bpps
      if(wBpp > 8)
          ddHALInfo.lpDDExeBufCallbacks = pExeBuf;


    bytes_per_pixel = wBpp/8;
    ddHALInfo.dwSize = sizeof ( ddHALInfo );

    ddHALInfo.hInstance = SELECTOROF( (LPVOID)&sData);
    ddHALInfo.lpPDevice = lpDriverPDevice;

    /*
     * current video mode
     */
    ddHALInfo.dwModeIndex = (DWORD)modeidx;

    /*
     * current primary surface attributes
     */
    ddHALInfo.vmiData.fpPrimary       = dwRealScreenFlatAddr;
    ddHALInfo.vmiData.dwDisplayWidth  = (DWORD) wScreenWidth;
    ddHALInfo.vmiData.dwDisplayHeight = (DWORD) wScreenHeight;
    ddHALInfo.vmiData.lDisplayPitch   = (DWORD) wScreenWidthBytes;

        _fmemset(&ddHALInfo.vmiData.ddpfDisplay,0,sizeof(ddHALInfo.vmiData.ddpfDisplay));
    ddHALInfo.vmiData.ddpfDisplay.dwSize = sizeof ( DDPIXELFORMAT );
    if( modeidx >= 0 )
    {
        modeInfo[modeidx].lPitch = wScreenWidthBytes;
        buildPixelFormat( &modeInfo[modeidx], &ddHALInfo.vmiData.ddpfDisplay );
    }
    DPF("Screen width Bytes = %d",wScreenWidthBytes);

    /*
     * video memory pool information
     */
    ddHALInfo.vmiData.pvmList      = vidMem;

    /*
     * New memory divisions, see vga.asm for details
     */
    DPF( "Video Base Addr = %08lx", ddHALInfo.vmiData.fpPrimary );
    DPF( "Video Memory = %08lx", dwVideoMemorySize );

    /*
     * set up the pointer to the first available video memory after
     * the primary surface
     */
    DPF("wScreenWidth = %d wScreenHeight = %d  wScreenWidthBytes = %d",
        wScreenWidth ,wScreenHeight ,wScreenWidthBytes );

    DPF("Rgn2 top = %d bottom = %d left = %d right = %d width = %d height = %d",
        Rgn2.sr_rect.top ,Rgn2.sr_rect.bottom,
        Rgn2.sr_rect.left ,Rgn2.sr_rect.right,
        Rgn2.sr_width ,Rgn2.sr_height );
    DPF("Rgn3 top = %d bottom = %d left = %d right = %d width = %d height = %d",
        Rgn3.sr_rect.top ,Rgn3.sr_rect.bottom,
        Rgn3.sr_rect.left ,Rgn3.sr_rect.right,
        Rgn3.sr_width ,Rgn3.sr_height );
    DPF("Rgn4 top = %d bottom = %d left = %d right = %d width = %d height = %d",
        Rgn4.sr_rect.top ,Rgn4.sr_rect.bottom,
        Rgn4.sr_rect.left ,Rgn4.sr_rect.right,
        Rgn4.sr_width ,Rgn4.sr_height );

    ddHALInfo.vmiData.dwNumHeaps  = 0;
    heap = 0;
    can_flip = FALSE;

    vidMem[0].dwFlags = VIDMEM_ISLINEAR;
    vidMem[1].dwFlags = VIDMEM_ISLINEAR;
    vidMem[2].dwFlags = VIDMEM_ISLINEAR;

// region 2
        if ((wScreenWidthBytes == (Rgn2.sr_width * bytes_per_pixel)) &&
                ((int)Rgn2.sr_height > 0)) {
                //For ViRGE, I will ignore the memory piece, whose width is
                //not the same as screen width.
                vidMem[heap].dwFlags |= VIDMEM_ISLINEAR;
                vidMem[heap].fpStart = ddHALInfo.vmiData.fpPrimary +
                        (DWORD)Rgn2.sr_rect.top * (DWORD)wScreenWidthBytes;
                vidMem[heap].fpEnd  = vidMem[heap].fpStart +
                        (DWORD)Rgn2.sr_height * (DWORD)wScreenWidthBytes - 1;
                vidMem[heap].ddsCaps.dwCaps = 0;
                vidMem[heap].ddsCapsAlt.dwCaps = 0;
                ddHALInfo.vmiData.dwNumHeaps++;
                DPF( "Added Rgn2: fpStart=%08lx, fpEnd=%08lx",
                        vidMem[heap].fpStart,
                        vidMem[heap].fpEnd);
                heap++;
                if( wScreenHeight <= Rgn2.sr_height )
                        can_flip = TRUE;
        }

// region 3

        if ((wScreenWidthBytes == (Rgn3.sr_width * bytes_per_pixel)) &&
                ((int)Rgn3.sr_height > 0)) {
                //For ViRGE, I will ignore the memory piece, whose width is
                //not the same as screen width.
                vidMem[heap].fpStart = ddHALInfo.vmiData.fpPrimary +
                        (DWORD)Rgn3.sr_rect.top * (DWORD)wScreenWidthBytes;
                vidMem[heap].fpEnd  = vidMem[heap].fpStart +
                        (DWORD)Rgn3.sr_height * (DWORD)wScreenWidthBytes - 1;
                vidMem[heap].ddsCaps.dwCaps = 0;
                vidMem[heap].ddsCapsAlt.dwCaps = 0;
                ddHALInfo.vmiData.dwNumHeaps++;
                DPF( "Added Rgn3: fpStart=%08lx, fpEnd=%08lx",
                        vidMem[heap].fpStart,
                        vidMem[heap].fpEnd);
                heap++;
                if( wScreenHeight <= Rgn3.sr_height )
                        can_flip = TRUE;
        }

// region 4, only VX can possibly have it

        if ((wDeviceId == D_S3VIRGEVX) && (dwDRAMSize != 0)) {
                vidMem[heap].fpStart = ddHALInfo.vmiData.fpPrimary + dwTotalMemorySize;
                vidMem[heap].fpEnd  = vidMem[heap].fpStart + dwDRAMSize - 1;
                vidMem[heap].ddsCaps.dwCaps = DDSCAPS_FRONTBUFFER | DDSCAPS_BACKBUFFER | DDSCAPS_FLIP;
                vidMem[heap].ddsCapsAlt.dwCaps = DDSCAPS_FRONTBUFFER | DDSCAPS_BACKBUFFER | DDSCAPS_FLIP;//no flip is allowed#21284
                ddHALInfo.vmiData.dwNumHeaps++;
                DPF( "Added Rgn4: fpStart=%08lx, fpEnd=%08lx",
                vidMem[heap].fpStart,
                vidMem[heap].fpEnd);
                heap++;
        }

    DPF( "*** HeapList ***" );
    DPF( "dwNumHeaps=%ld", ddHALInfo.vmiData.dwNumHeaps );
    for( heap=0;heap< ddHALInfo.vmiData.dwNumHeaps;heap++ )
    {
                DPF( "heap %d: fpStart=%08lx, fpEnd=%08lx",
                heap,
                vidMem[heap].fpStart,
                vidMem[heap].fpEnd);
    }

    /*
     * capabilities supported
     */

    if (bFamilyFlags & MOBILE_FAMILY)
    {   //fixed #530 in AMD K6, as its CR6B is not default to 0 for non-mobile parts like Diamond 3000 3D
        outp(CRTC_REG, 0x6B);
        S3MobileData.bCurrentDisplay = inp(CRTC_REG+1);

        if ( S3MobileData.bCurrentDisplay & DUOVIEW)
        can_flip = 0;
    }
    ddHALInfo.ddCaps.dwCaps         = DDCAPS_GDI |
                                      DDCAPS_BLT |
                                      DDCAPS_3D |  //fix #SPR 15230
                                      DDCAPS_ALPHA |
                                      DDCAPS_BLTDEPTHFILL |
                                      DDCAPS_BLTCOLORFILL |
                                      DDCAPS_COLORKEY ;

    ddHALInfo.ddCaps.dwCKeyCaps     = DDCKEYCAPS_SRCBLT;

    ddHALInfo.ddCaps.dwFXCaps       = 0x00000000;

    ddHALInfo.ddCaps.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
                                      DDSCAPS_PRIMARYSURFACE |
                                      DDSCAPS_ALPHA;//ly |               //###
//ly                                      DDSCAPS_3DDEVICE |
//ly                                      DDSCAPS_TEXTURE |
//ly                                      DDSCAPS_ZBUFFER;

//    if (S3MobileData.b3DCaps & (Virge_3D | VirgeGX_3D)){
    if (wBpp >= 16){ //ly
    ddHALInfo.ddCaps.dwCaps     |= DDCAPS_BLTDEPTHFILL;
    ddHALInfo.ddCaps.ddsCaps.dwCaps |=
                       DDSCAPS_3DDEVICE |
                       DDSCAPS_TEXTURE |
                       DDSCAPS_ZBUFFER |
                       DDSCAPS_MIPMAP;

    ddHALInfo.ddCaps.dwZBufferBitDepths = DDBD_16;
    }


        if( STREAMS_PROCESSOR_PRESENT )
        {
        ddHALInfo.ddCaps.dwCaps |=
                    DDCAPS_ALPHA |
                    DDCAPS_COLORKEY |
#if     0
                    DDCAPS_ALIGNBOUNDARYDEST |
                    DDCAPS_ALIGNSIZEDEST |
                    DDCAPS_ALIGNBOUNDARYSRC |
                    DDCAPS_ALIGNSIZESRC |
#endif
                    DDCAPS_OVERLAY |
                    DDCAPS_OVERLAYSTRETCH |
                    DDCAPS_OVERLAYFOURCC |
                    DDCAPS_OVERLAYCANTCLIP;

        ddHALInfo.ddCaps.dwCKeyCaps |=
                    DDCKEYCAPS_SRCOVERLAY |
                    DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV |
                    DDCKEYCAPS_SRCOVERLAYONEACTIVE |
                    DDCKEYCAPS_SRCOVERLAYYUV |
                    DDCKEYCAPS_DESTOVERLAY |
                    DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV |
                    DDCKEYCAPS_DESTOVERLAYONEACTIVE |
                    DDCKEYCAPS_DESTOVERLAYYUV;

          ddHALInfo.ddCaps.ddsCaps.dwCaps |= DDSCAPS_OVERLAY;   //###

        ddHALInfo.ddCaps.dwFXAlphaCaps |= DDFXALPHACAPS_OVERLAYALPHAPIXELS;
        ddHALInfo.ddCaps.dwFXCaps |=
                    DDFXCAPS_OVERLAYSTRETCHX |
                    DDFXCAPS_OVERLAYSTRETCHY;
        }

    if( can_flip )
    ddHALInfo.ddCaps.ddsCaps.dwCaps |= DDSCAPS_FLIP;
     else
    ddHALInfo.ddCaps.ddsCaps.dwCaps &= ~DDSCAPS_FLIP;

    /*
     * turn off transparent bls for 24bpp
     */
    if (wBpp == 24)
    {
        ddHALInfo.ddCaps.dwCKeyCaps &= ~DDCKEYCAPS_SRCBLT;
    }

    /*
     * fill in rops supported
     */
    setupRops( s3ropsSupported, ropsSupported, NUMENTRIES(s3ropsSupported));
    for( ii=0;ii<DD_ROP_SPACE;ii++ )
    {
        ddHALInfo.ddCaps.dwRops[ii] = ropsSupported[ii];
    }

    /*
     * required alignments of the scan lines for each kind of memory
     */
    ddHALInfo.vmiData.dwOffscreenAlign = AlignTbl[ wBpp >> 2 ];
    ddHALInfo.vmiData.dwOverlayAlign = 8;
        ddHALInfo.vmiData.dwTextureAlign = 8;
        ddHALInfo.vmiData.dwZBufferAlign = 8;

    /*
     * callback functions
     */
        memset(&cbDDCallbacks,0,sizeof(cbDDCallbacks));
        memset(&cbDDSurfaceCallbacks,0,sizeof(cbDDSurfaceCallbacks));
        memset(&cbDDPaletteCallbacks,0,sizeof(cbDDPaletteCallbacks));

        cbDDCallbacks.dwSize = sizeof(cbDDCallbacks);
        cbDDCallbacks.DestroyDriver = (LPDDHAL_DESTROYDRIVER) HALDestroyDriver;
        cbDDSurfaceCallbacks.dwSize = sizeof(cbDDSurfaceCallbacks);
        cbDDPaletteCallbacks.dwSize = sizeof(cbDDPaletteCallbacks);

    ddHALInfo.lpDDCallbacks        = &cbDDCallbacks;
    ddHALInfo.lpDDSurfaceCallbacks = &cbDDSurfaceCallbacks;
    ddHALInfo.lpDDPaletteCallbacks = &cbDDPaletteCallbacks;

#ifdef DX5
        /*
         * Video Port Capabilities
         */
        ddHALInfo.ddCaps.dwCaps2 =
                                    DDCAPS2_VIDEOPORT |
                                    DDCAPS2_COLORCONTROLOVERLAY |
                                    DDCAPS2_CANBOBINTERLEAVED |
                                    DDCAPS2_CANBOBNONINTERLEAVED |
                                    DDCAPS2_AUTOFLIPOVERLAY |
                                    DDCAPS2_COPYFOURCC |
                                    0;
        ddHALInfo.ddCaps.dwMaxVideoPorts = 1;
        ddHALInfo.ddCaps.dwCurrVideoPorts = 0;

    switch(wDeviceId)
    {
        case D_S3764:
        case D_S3VIRGE:
        case D_S3VIRGEVX:
        case D_S3M65:
        case D_S3767:
            // Hardware with no overlay color control or odd/even detection
            // Disabling Video port on these also, since functionality
            // is limited and not well tested
            ddHALInfo.ddCaps.dwCaps2 &= ~(DDCAPS2_COLORCONTROLOVERLAY |
                                          DDCAPS2_VIDEOPORT |
                                          DDCAPS2_CANBOBINTERLEAVED |
                                          DDCAPS2_CANBOBNONINTERLEAVED |
                                          DDCAPS2_AUTOFLIPOVERLAY);
            break;
        default:
            break;
    }
    ddHALInfo.GetDriverInfo = sData.lpGetDriverInfo32;
    ddHALInfo.dwFlags |= DDHALINFO_GETDRIVERINFOSET;
    DPF("Display: Filling in ddHALInfo.GetDriverInfo: %lx", sData.lpGetDriverInfo32);

#if 1
    /*
    ** Windows 98
    ** Some new Win98 functionality has not gotten proper testing
    ** It will be disabled for now, we should re-enable it after
    ** we have time to test against the final version of Win98
    ** BJT 1/30/98
    */
    ddHALInfo.ddCaps.dwCaps2 &= ~(DDCAPS2_COLORCONTROLOVERLAY |
                                  DDCAPS2_CANBOBINTERLEAVED);
#endif

#endif //DX5
    /*
     *  FOURCCs supported
     */
    ddHALInfo.ddCaps.dwNumFourCCCodes = 0;
    ddHALInfo.lpdwFourCC = NULL;

#define FOURCC_YUY2   0x32595559
#define FOURCC_Y211   0x31313259

        if( STREAMS_PROCESSOR_PRESENT )
        {
//              ddHALInfo.ddCaps.dwNumFourCCCodes = 2;
// Y211  is S3 format, AP may not know, so disable Y211
//
                ddHALInfo.ddCaps.dwNumFourCCCodes = 1;
                ddHALInfo.ddCaps.dwMaxVisibleOverlays = 1;
                ddHALInfo.lpdwFourCC = dwpFOURCCs;
                dwpFOURCCs[0] = FOURCC_YUY2;
//                dwpFOURCCs[1] = FOURCC_Y211;

                ddHALInfo.ddCaps.dwMinOverlayStretch   =
                ddHALInfo.ddCaps.dwMinLiveVideoStretch =
                ddHALInfo.ddCaps.dwMinHwCodecStretch   = 1000;

//### Make MinLiveVideoStretch and MinHwCodecStretch == 1000
//### in order to work for DX3

                if( k2vRatioOverride)
                {
                        ddHALInfo.ddCaps.dwMinOverlayStretch   = k2vRatioOverride;
//                      ddHALInfo.ddCaps.dwMinLiveVideoStretch =
//                      ddHALInfo.ddCaps.dwMinHwCodecStretch   = k2vRatioOverride;
                }
                else
                {
                        ddHALInfo.ddCaps.dwMinOverlayStretch   = (DWORD) GetK2vRatio( SWCODEC);
//                      ddHALInfo.ddCaps.dwMinLiveVideoStretch = (DWORD) GetK2vRatio( DIGITIZER);
//                      ddHALInfo.ddCaps.dwMinHwCodecStretch   = (DWORD) GetK2vRatio( HWCODEC);
                }

                ddHALInfo.ddCaps.dwMaxOverlayStretch   =
                ddHALInfo.ddCaps.dwMaxLiveVideoStretch =
                ddHALInfo.ddCaps.dwMaxHwCodecStretch   = 9999;

#if     0       //### need to set these to 1
      ddHALInfo.ddCaps.dwAlignBoundaryDest = 1;
      ddHALInfo.ddCaps.dwAlignSizeDest = 1;
      ddHALInfo.ddCaps.dwAlignBoundarySrc = 1;
      ddHALInfo.ddCaps.dwAlignSizeSrc = 1;
#endif
        }

    /*
     * mode information
     */
    ddHALInfo.dwNumModes = num_entry;
    ddHALInfo.lpModeInfo = modeInfo;

} /* buildDDHALInfo */

extern DWORD FAR CDECL CallProc32ExW(DWORD num_params, DWORD dwFlags, DWORD Callback32, ...);
extern DWORD FAR PASCAL MapSL(LPVOID);
extern LPVOID FAR PASCAL MapLS(DWORD);
extern void   FAR PASCAL UnMapLS(LPVOID);
/*
 * DestroySurface16
 */
DWORD CALLBACK __loadds DestroySurface16(LPDDHAL_DESTROYSURFACEDATA lpd)
{
        DWORD dwCaps = lpd->lpDDSurface->ddsCaps.dwCaps;
        DWORD rc;



        if(     lpd->lpDDSurface->lpGbl->dwReserved1 != 0)
        {
                if (MapSL(lpd->lpDDSurface) == (DWORD)(sData.lpVisibleOverlay)) {
                WAIT_FOR_VSYNC;         //fixes #17584
                mmdwSetRegister(BLEND_CONTROL, POnS);
                mmdwSetRegister(S_WH, WH(10,2));      //set to 10X2 square.
                mmdwSetRegister(OPAQUE_CONTROL, 0);   //disable opaque control
                }
                if( dwCaps & (DDSCAPS_LIVEVIDEO | DDSCAPS_HWCODEC))
                {
                        // notify the MPEG driver to suspend, just like a DOS box
                        // is becoming foreground.
                        _asm mov ax, OVERLAY_DESTROYED
                        _asm int 2fh
                }

#if 0
//
// we cant call the 32-bit driver, because this will release
// the Win16Lock. and we will get reentered, bad news.
//
                // call the 32-bit piece and let it clean up.
                (DWORD)lpd->lpDDSurface->lpData = MapSL(lpd->lpDDSurface->lpData);
                (DWORD)lpd->lpDDSurface = MapSL(lpd->lpDDSurface);
                rc = CallProc32ExW(1, 1, (DWORD)sData.lpDestroySurface32, (DWORD)lpd);
#else
                {
                    DWORD FAR *pdw;

                    //
                    // mark the SPOD as not in use
                    //
                    pdw = MapLS(lpd->lpDDSurface->lpGbl->dwReserved1);
                    *pdw = 0;
                    UnMapLS(pdw);

                    lpd->ddRVal = DD_OK;
                    rc = DDHAL_DRIVER_NOTHANDLED;
                }
#endif
        }
        else
        {
                lpd->ddRVal = DD_OK;
                rc = DDHAL_DRIVER_NOTHANDLED;
        }



/*----------------mipmap--------------------------*/
// we cant call the 32-bit driver, because this will release
// the Win16Lock. and we will get reentered, bad news.
//
// call the 32-bit piece and let it clean up.
    (DWORD)lpd->lpDDSurface->lpGbl  = MapSL(lpd->lpDDSurface->lpGbl);
    (DWORD)lpd->lpDDSurface         = MapSL(lpd->lpDDSurface);
    (DWORD)lpd->lpDD                = MapSL(lpd->lpDD);
    rc = CallProc32ExW(1, 1, (DWORD)sData.lpDestroySurface32, (DWORD)lpd);
/*--------------------------------------------------*/

        return rc;
}

/*
 * DDCreateDriverObject
 *
 * DDCREATEDRIVEROBJECT handler.  Called at ESCAPE time or when the
 * mode is set.
 */
BOOL __loadds FAR PASCAL DDCreateDriverObject ( int bReset )
{
    int                 modeidx;

    if( (lpDDHAL_SetInfo == NULL) || (sData.dwSize != sizeof(S3THKINFO)) )
    {
        return FALSE;
    }
    DPF( "DDCreateDriverObject( %d )", bReset );
    DPF( "Chip id = 0x%x",wChipId);
    DPF( "Device id = 0x%x",wDeviceId);
    DPF( "Revision id = 0x%x",bRevisionId);


         num_entry = NUMENTRIES1;
         modeInfo = modeInfo1;

    switch(wDeviceId){
        case D_S3VIRGEVX:
            num_entry = NUMENTRIES_VX;
            modeInfo = modeInfo_VX;
            break;
        case D_S3M3:
        case D_S3M5:
        case D_S3M5P:
        case D_S3M5PM:
        case D_S3VIRGEGX2:
        case D_S3VIRGEGX2P:
        case D_S3VIRGEGX2PM:
            num_entry = NUMENTRIES2;
            modeInfo = modeInfo2;
            break;
        default:    //default use Virge table
            num_entry = NUMENTRIES1;
            modeInfo = modeInfo1;
            break;
    }

    /*
     * determine current video mode
     */
    for( modeidx=0; modeidx < num_entry; modeidx++ )
    {
        if( (modeInfo[modeidx].dwWidth == wScreenWidth) &&
            (modeInfo[modeidx].dwHeight == wScreenHeight) &&
            (modeInfo[modeidx].dwBPP == wBpp) )
        {
            DPF("Found mode %d x %d, %dbpp, idx %d",wScreenWidth, wScreenHeight, wBpp, modeidx);
            if (wBpp == 16)
            {
                if( wPDeviceFlags & FIVE6FIVE )
                {
                    modeInfo[modeidx].dwRBitMask     = 0xf800;
                    modeInfo[modeidx].dwGBitMask     = 0x07e0;
                    modeInfo[modeidx].dwBBitMask     = 0x001f;
                    modeInfo[modeidx].dwAlphaBitMask = 0x0000;
                }
                else
                {
                    modeInfo[modeidx].dwRBitMask     = 0x7c00;
                    modeInfo[modeidx].dwGBitMask     = 0x03e0;
                    modeInfo[modeidx].dwBBitMask     = 0x001f;
                    modeInfo[modeidx].dwAlphaBitMask = 0x0000;
                }
            }
            break;
        }
    }

    if (modeidx == num_entry)
    {
        modeidx = -1;
        sData.dwSpecialHW &= ~STREAMS_PROCESSOR;
    }

    /*
     * Do we need vflatd for access to the frame buffer?
     */
    if( bUsingVFlatD )
    {
        bUsingVFlatD = FALSE;
        VFDEndLinearAccess();
        dwRealScreenFlatAddr = 0;
    }
    if( dwScreenFlatAddr == 0 )
    {
        if( VFDQuerySel() )
        {
            VFDBeginLinearAccess();
            dwRealScreenFlatAddr = VFDQueryBase();
            bUsingVFlatD = TRUE;
            DPF( "Using VFLATD, addr=%08lx", dwRealScreenFlatAddr );
        }
    }
    else
    {
        dwRealScreenFlatAddr = dwScreenFlatAddr;
    }
    if( dwRealScreenFlatAddr == 0 )
    {
        return FALSE;
    }

    sData.dwScreenFlatAddr = dwRealScreenFlatAddr;

        /*
         * detect chip families for enhanced support
         */

        sData.dwSpecialHW = BIMODAL_HW | MMIOONLY_HW;

    // Detect stream processor capability based on chip, resolution, DuoView,
    // clock mode, ...

    if (modeidx != -1)
    {

    // Resolution
    // ViRGE
    // ViRGE VX
    // ViRGE GX/DX : can support up to 1280x1020
    // M3/M5/GX2   :  ??? need to check w/ HW group

    // Turn off stream support for doubled DAC clock mode
          outp(SEQ_REG, 0x18);

    if ( (inp(SEQ_REG+1) & 0x80) ||
         ((wDeviceId == D_S3VIRGEDXGX) && (wScreenWidth >= 1280)) ||
         (((wDeviceId == D_S3VIRGE) || (wDeviceId == D_S3VIRGEVX))
           && (wScreenWidth > 1280))                  ||
         (((wDeviceId == D_S3M3) ||
           (wDeviceId == D_S3M5) ||
           (wDeviceId == D_S3M5P) ||
           (wDeviceId == D_S3M5PM) ||
           (wDeviceId == D_S3VIRGEGX2P) ||
           (wDeviceId == D_S3VIRGEGX2PM) ||
           (wDeviceId == D_S3VIRGEGX2)) &&
           ((wScreenWidth > 1600) &&
            (!(S3MobileData.dwMobileCaps & DUAL_IMAGE)))) )
    {
       sData.dwSpecialHW &= ~STREAMS_PROCESSOR;
    }

    else
    {
        DPF( "Streams Processor found" );
        sData.dwSpecialHW |= STREAMS_PROCESSOR;

        if (wDeviceId == D_S3VIRGEVX)
        {
            if (modeInfo[modeidx].dwWidth == 1280)
            CR6D_80 = 1;
            else
            CR6D_80 = 0;

            //patch for SC EDO bug with SP on

            WAIT_NO_VSYNC
            WAIT_FOR_VSYNC
            mmr8208 = mmdwGetRegister(0x8208);
            mmdwSetRegister(0x8208, 0x3030L); // default SP timeout value
            outpw(CRTC_REG, 0x4838);
            outpw(CRTC_REG, 0xA539);
            outp(CRTC_REG, 0x36);
            CurCR36 = CurCR36 | 0x0c;     //switch to fast page mode
            outp(CRTC_REG+1, CurCR36);
        }

        // Set up and load fifo table into memory for later use. For chips
        // which use formula to

        if( InitK2vTable() == -1)
        {
            sData.dwSpecialHW &= ~STREAMS_PROCESSOR;
        }
        else
        {
            hmemcpy(sData.k2vTable,k2vTable,MAX_DEVICE*sizeof( K2VTABLE));
            k2vRatioOverride =
           (DWORD) GetPrivateProfileInt("display","ratio", 0, "system.ini");
            hmemcpy(&(sData.band), pBand, sizeof(BAND));
        }
        }
    }

    sData.dwSpecialHW &= ~READ_SCANLINE;
    if ( S3MobileData.wSPStyle & SP_M5 )
    {
        sData.dwSpecialHW |= READ_SCANLINE;
    }


    /*
     * set up hal info
     */
    buildDDHALInfo( modeidx );

    /*
     * copy new data into area of memory shared with 32-bit DLL
     */

    sData.lpMMReg = GetSelectorBase(wMMIOSelector);
    sData.dwScreenHeight = (DWORD) wScreenHeight;
    sData.dwScreenWidth = (DWORD) wScreenWidth;
    sData.dwPhysScreenHeight = (DWORD) wPhysScreenHeight;
    sData.dwPhysScreenWidth = (DWORD) wPhysScreenWidth;
    sData.dwScreenWidthBytes = (DWORD) wScreenWidthBytes;
    sData.ddpf = ddHALInfo.vmiData.ddpfDisplay;
    if(bReset)
        sData.bReset = bReset;
    sData.wChipId = wChipId;
    sData.wDeviceId = wDeviceId;
    sData.dwMULT_MISC=wMULT_MISC;
    sData.bFamilyFlags = bFamilyFlags;
    sData.dwVideoMemorySize = dwVideoMemorySize;
    sData.lpCurrentChipMode = LinearAddressOf( (DWORD)&CurrentChipMode );
    sData.lpSrcWH         = (DWORD FAR *)LinearAddressOf((DWORD)&dwSrcWH);
    sData.lpSSXY          = (DWORD FAR *)LinearAddressOf((DWORD)&dwSSXY);
    sData.lpSSWH          = (DWORD FAR *)LinearAddressOf((DWORD)&dwSSWH);
    sData.lpSSAddr        = (DWORD FAR *)LinearAddressOf((DWORD)&dwSSAddr);
    sData.lpTVCenterAdj   = (DWORD FAR *)LinearAddressOf((DWORD)&dwTVCenteringAdj);
    sData.lpCenterOffset  = (DWORD FAR *)LinearAddressOf((DWORD)&wCenterY );
    sData.lpVWOffset      = (DWORD FAR *)LinearAddressOf((DWORD)&wVWTop );
    sData.lpVWBottom      = (DWORD FAR *)LinearAddressOf((DWORD)&wVWBottom);
    sData.lpVWTop1        = (DWORD FAR *)LinearAddressOf((DWORD)&wVWTop1);
    sData.lpVWBottom1     = (DWORD FAR *)LinearAddressOf((DWORD)&wVWBottom1);
    sData.lpVWTop2        = (DWORD FAR *)LinearAddressOf((DWORD)&wVWTop2);
    sData.lpVWBottom2     = (DWORD FAR *)LinearAddressOf((DWORD)&wVWBottom2);
    sData.lpPSOffset      = (DWORD FAR *)LinearAddressOf((DWORD)&dwPSAddr );
    sData.lpPS2Offset     = (DWORD FAR *)LinearAddressOf((DWORD)&dwPSAddr2 );
    sData.lpFlipOffset    = (DWORD FAR *)LinearAddressOf((DWORD)&dwFlipOffset );
    sData.lpClipOffset    = (DWORD FAR *)LinearAddressOf((DWORD)&dwClipOffset );
    sData.lpMobileCaps    = (DWORD FAR *)LinearAddressOf((DWORD)&(S3MobileData.dwMobileCaps));
    sData.lpVideoType     = (DWORD FAR *)LinearAddressOf((DWORD)&dwVideoType);
    sData.lpBlendCtrl     = (DWORD FAR *)LinearAddressOf((DWORD)&dwBlendCtrl);
    sData.lpSPFIFOReg     = (DWORD FAR *)LinearAddressOf((DWORD)&SavedSPFIFOReg);
    sData.lpShowSSWindow  = (BYTE FAR *)LinearAddressOf((DWORD)&bShowSSWindow);
    sData.lpS3MobileData  = (LPS3MOBILEDATA)LinearAddressOf((DWORD)&S3MobileData);
    sData.lpS3DriverData    = (LPS3DRIVERDATA)LinearAddressOf((DWORD)&S3DriverData);
    sData.dwPanelSize       = dwPanelSize;
    sData.lpColorAdjustFlags= (DWORD FAR *)LinearAddressOf((DWORD)&dwColorAdjustFlags);
    sData.lpColorAdjustReg  = (DWORD FAR *)LinearAddressOf((DWORD)&dwColorAdjustReg);
    sData.lpColorTable      = (DWORD FAR *)LinearAddressOf(
        (DWORD)((DIBENGINE FAR *)lpDriverPDevice)->deBitmapInfo +
        ((DIBENGINE FAR *)lpDriverPDevice)->deBitmapInfo->bmiHeader.biSize);
    sData.lpbStreamProcessorFlags = (DWORD FAR *)LinearAddressOf((DWORD)&bStreamProcessorFlags);

    /* more data copied to S3DriverData
     */
    S3DriverData.wPanelResolution   = wPanelResolution;
    S3DriverData.wLogResolution     = wLogResolution;
    S3DriverData.wPhysResolution    = wPhysResolution;

    /*
     * get addresses of 32-bit routines
     */
        cbDDCallbacks.WaitForVerticalBlank = sData.lpWaitForVerticalBlank32;
        cbDDCallbacks.CanCreateSurface = sData.lpCanCreateSurface32;
        cbDDCallbacks.dwFlags =
                                DDHAL_CB32_WAITFORVERTICALBLANK |
                                DDHAL_CB32_CANCREATESURFACE;

        cbDDSurfaceCallbacks.Blt = sData.lpBlt32;
        cbDDSurfaceCallbacks.Flip = sData.lpFlip32;
        cbDDSurfaceCallbacks.Lock = sData.lpLock32;
        cbDDSurfaceCallbacks.Unlock = sData.lpUnLock32;
        cbDDSurfaceCallbacks.GetBltStatus = sData.lpGetBltStatus32;
        cbDDSurfaceCallbacks.GetFlipStatus = sData.lpGetFlipStatus32;
        cbDDSurfaceCallbacks.dwFlags =
                                DDHAL_SURFCB32_BLT |
                                DDHAL_SURFCB32_FLIP |
                                DDHAL_SURFCB32_LOCK |
                                DDHAL_SURFCB32_UNLOCK |
                                DDHAL_SURFCB32_GETBLTSTATUS |
                                DDHAL_SURFCB32_GETFLIPSTATUS;

          cbDDCallbacks.CreateSurface = sData.lpCreateSurface32;
          cbDDCallbacks.dwFlags |=
             DDHAL_CB32_CREATESURFACE;

          cbDDSurfaceCallbacks.DestroySurface = sData.lpDestroySurface32;
          cbDDSurfaceCallbacks.dwFlags |=
                                DDHAL_SURFCB32_DESTROYSURFACE;

       /* ----------------mipmap----------------  */

        cbDDSurfaceCallbacks.AddAttachedSurface = sData.lpAddAttachedSurface32;
        cbDDSurfaceCallbacks.dwFlags |= DDHAL_SURFCB32_ADDATTACHEDSURFACE;
        /* --------------------------------------------------*/

        if( STREAMS_PROCESSOR_PRESENT )
        {
                cbDDSurfaceCallbacks.SetColorKey = sData.lpSetColorKey32;
                cbDDSurfaceCallbacks.UpdateOverlay = sData.lpUpdateOverlay32;
                cbDDSurfaceCallbacks.SetOverlayPosition = sData.lpSetOverlayPosition32;
                cbDDSurfaceCallbacks.dwFlags |=
                                DDHAL_SURFCB32_SETCOLORKEY |
                                DDHAL_SURFCB32_UPDATEOVERLAY |
                                DDHAL_SURFCB32_SETOVERLAYPOSITION;
        }

    if( CAN_READ_SCANLINE )
        {
        cbDDCallbacks.dwFlags |=
                DDHAL_CB32_GETSCANLINE;
        cbDDCallbacks.GetScanLine = sData.lpGetScanLine32;
            ddHALInfo.ddCaps.dwCaps |= DDCAPS_READSCANLINE;
        }

        /*
         * go turn it on
         */
    if ( ( STREAMS_PROCESSOR_PRESENT ) &&  (!( S3MobileData.wSPStyle & SP_M5 )) )
        {
                TurnOnStreamsProcessorMode();
        }

    /*
     * go initialize DirectDraw
     */
    bDDrawActive = TRUE;
         bInDDraw = 1;
//### VL
    return lpDDHAL_SetInfo (DDHookOnDDCreateDriverObject (&ddHALInfo, bReset), bReset);
//    return lpDDHAL_SetInfo( &ddHALInfo,bReset);

} /* DDCreateDriverObject */

/*
 * HALDestroyDriver
 */
extern DWORD VDDCall(DWORD DeviceHandle, DWORD function, DWORD flags, LPVOID buffer, DWORD buffer_size);
#define HWEnable(f)  ((dwDeviceHandle==1) ? 0 : VDDCall(dwDeviceHandle, VDD_ENABLE, f, NULL, 0))
#include "minivdd.h"

DWORD __loadds FAR PASCAL HALDestroyDriver(
                LPDDHAL_DESTROYDRIVERDATA lpDestroyDriverData )
{
   DWORD t;
   DWORD dw;
   dw = HWEnable(ENABLE_ALL);

         if ((sData.dwSpecialHW & STREAMS_PROCESSOR) != 0) {
//        if (SP_SUPPORTED) {

        S3DriverData.VideoData = 0;   //we closed video

        /*
         * go turn it off
         */
        if ( (dwColorAdjustFlags & 2) && (dwColorAdjustReg & 0xFF1F9FFF ) )
        {
            dwColorAdjustFlags &= ~0x02;    //Don't indicate YUV overlay.
                                            //disable CA_YUV support
            dwColorAdjustReg &= 0x7FFF7FFF; //disable color adjust
            mmdwSetRegister(COLOR_ADJUST_REG, dwColorAdjustReg);
        }


//###
//we have to reprogram P_CONTROL with 0x0 before we turn off SP,
//fail to do so when black out the Windows screen after you ran
//modes (like 320x240x16) with MM8180[30-28]=001.
    if (S3MobileData.wSPStyle & (SP_Virge | SP_VX | SP_VirgeGX))
    {
        if (wDeviceId == D_S3VIRGEVX){
                mmdwSetRegister( P_CONTROL,    0x00000000L);
                mmdwSetRegister( 0x8208, mmr8208 );     //restore stream timeout value
        }else
                mmdwSetRegister( 0x8208, 0x2010L );     //default stream timeout value
        mmdwSetRegister( 0x81f8, 0x07f007f0L ); //move secondary stream away
        if ((wBpp != 24)  || ((wDeviceId != D_S3VIRGE) && ((wDeviceId & 0x00ff) != D_S3VIRGEFamily)))
//  if ((wBpp != 24)  || (wDeviceId != D_S3VIRGE))
                TurnOffStreamsProcessorMode();
        else
                mmdwSetRegister( 0x8200, 0x12000L );

        if (wDeviceId == D_S3VIRGEVX) {
                //patch for SC EDO

                WAIT_NO_VSYNC   //###
                WAIT_FOR_VSYNC

                outpw(CRTC_REG, 0x4838);        // it's probably already unlocked ...
                outpw(CRTC_REG, 0xA539);
                outp(CRTC_REG, 0x36);
                outp(CRTC_REG+1, OrgCR36);                      //restore original CR36
                CurCR36 = OrgCR36;

                //patch16
                if( (OrgCR36 & 0xc) == 0x0)
                BWFlag = BWFlag & 0xf7;

        }
        } //SPStyle for virge, virge/vx virge/dx/gx
    else if (S3MobileData.wSPStyle & SP_M5){
        mmdwSetRegister( 0x81f8, 0x07f007f0L ); //move secondary stream away
        TurnOffStreamsProcessorMode();
    } //SPStyle for SP_M5
        } //SP_SUPPORTED

//### reinitialize src_base, dst_base and stride registers
        mmdwSetRegister( 0xa4d4, 0L );
        mmdwSetRegister( 0xa4d8, 0L );
        mmdwSetRegister( 0xa4e4, ((t = wScreenWidthBytes) <<16 | wScreenWidthBytes));

    DPF("Destroy Driver");
    lpDDHAL_SetInfo = NULL;
    if( bUsingVFlatD )
    {
        DPF( "Done using VFlatD" );
        bUsingVFlatD = FALSE;
        VFDEndLinearAccess();
        dwRealScreenFlatAddr = 0;
    }
    bDDrawActive = FALSE;
         bInDDraw = 0;
    lpDestroyDriverData->ddRVal = DD_OK;

    HWEnable(dw);
    return DDHAL_DRIVER_HANDLED;

} /* HALDestroyDriver */

/*
 * DDGet32BitDriverName
 */
void __loadds FAR PASCAL DDGet32BitDriverName( LPDD32BITDRIVERDATA p32dd )
{
#ifdef DD32_DLLNAME // MSBUILT always define this
    lstrcpy( p32dd->szName, DD32_DLLNAME );
#else
    lstrcpy( p32dd->szName, S3DD32_DLLNAME );
#endif
    lstrcpy( p32dd->szEntryPoint, "DriverInit" );
    p32dd->dwContext = (DWORD)(LPVOID)&sData;

    sData.lpS3MobileData    = (LPS3MOBILEDATA)LinearAddressOf((DWORD)&S3MobileData);
    sData.wChipId = wChipId;
    sData.wDeviceId = wDeviceId;
    sData.bRevisionId = bRevisionId;
    sData.bFamilyFlags = bFamilyFlags;
    sData.dwDeviceHandle = dwDeviceHandle;
    sData.ddpf.dwRGBBitCount = (DWORD)wBpp;

} /* DDGet32BitDriverName

/*
 * DDNewCallbackFns
 */
void __loadds FAR PASCAL DDNewCallbackFns( LPDCICMD pdci )
{
    LPDDHALDDRAWFNS pfns;

    pfns = (LPVOID) pdci->dwParam1;
    lpDDHAL_SetInfo = pfns->lpSetInfo;
    DPF( "lpDDHAL_SetInfo = %08lx", lpDDHAL_SetInfo );

} /* DDNewCallbackFns */

/*
 * DDGetHALVersion
 */
int FAR PASCAL DDGetHALVersion( void )
{
    return DD_HAL_VERSION;

} /* DDGetHALVersion */

/*
 * TurnOnStreamsProcessorMode
 */
void FAR PASCAL TurnOnStreamsProcessorMode( void )
{
        LPDDPIXELFORMAT lpDPF;
        DWORD dwPFormat = (DWORD)-1;
        DWORD fifo_setting;
        BYTE bReg;

        bStreamProcessorFlags |= (SP_IN_ENHANCE_MODE | SP_ENABLED);
        S3MobileData.dwMobileCaps |= SP_ON;
        fifo_setting = GetK2vFifo( SWCODEC, 320);
          if (SavedSPFIFOReg == 0) {
            SavedSPFIFOReg = ((fifo_setting & 0x0000001fL) |
                            ((fifo_setting & 0x000003e0L) << 1) |
                             ((fifo_setting & 0x0000007c00L) << 2));
          }
        outp(CRTC_REG, 0x67);
        if( (inp(CRTC_REG+1) & 0xc) == 0xc)
                return;

        wait_for_idle();

        WAIT_NO_VSYNC   //###
        WAIT_FOR_VSYNC

        outp(CRTC_REG, 0x67);
        bReg = inp(CRTC_REG+1);
        bReg |= 0x0c;
        outp(CRTC_REG+1, bReg);

        if (wDeviceId == D_S3VIRGEVX) {

                //patch for screen shift

                outp(CRTC_REG, 0x63);
                     if (bCR63 == 0xff)
                    bCR63 = inp(CRTC_REG+1);
                outp(CRTC_REG+1, 0x49);
                outp(CRTC_REG, 0x6D);
                     if (bCR6D == 0xff)
                    bCR6D = inp(CRTC_REG+1);
                if      (CR6D_80)
                        if (wBpp == 24)
                                outp(CRTC_REG+1, 0x30);
                        else
                                outp(CRTC_REG+1, 0x80);
                else
                        outp(CRTC_REG+1, 0x40);

                //patch16
                BWFlag = BWFlag | 8;
        }

        // various fixes

      if ((wBpp >= 16) && (wDeviceId == D_S3VIRGEVX)) {
        mmdwSetRegister( 0x8208, 0x2010L );     //default stream timeout value
        }
        else {
        mmdwSetRegister( 0x8208, 0x3030L );     //default stream timeout value
        }

//wjm   if( wBpp == 8 )
        if((wBpp == 8) && (wDeviceId == D_S3VIRGE))
                FixMissingPixels();

        if( (wBpp == 8) || (wBpp == 16) )
                FixCursorColor();

        lpDPF = &sData.ddpf;

        switch( lpDPF->dwRGBBitCount )
        {
                case 8:
                        dwPFormat = P_RGB8;
                        break;
                case 16:
                        if (IsRGB15(lpDPF))
                                dwPFormat = P_RGB15;
                        else
                        if (IsRGB16(lpDPF))
                                dwPFormat = P_RGB16;
                        break;
                case 24:
                        if (IsRGB24(lpDPF))
                                dwPFormat = P_RGB24;
                        break;
                case 32:
                        if (IsRGB32(lpDPF))
                                dwPFormat = P_RGB32;
                        break;
        }
        if( dwPFormat != -1 )
        {
//### This is THE place to safely set this register.
// Miscellaneous timeout register.  Need to program this register when
// playing an .avi file under certain refresh rate in order to get rid
// of some screen noise.
                mmdwSetRegister( 0x820c, 0x08080804L );

                mmdwSetRegister( P_CONTROL,      dwPFormat );
                if (wBpp == 16)
                        mmdwSetRegister( 0x8200,   ((0xfL << FifoAlloc_Shift)|
                                                    (4L << 12) |
                                                    (4L << S_FifoThresh_Shift)));
                else
                        mmdwSetRegister( 0x8200,   ((0xcL << FifoAlloc_Shift)|
                                                    (4L << 12) |
                                                    (4L << S_FifoThresh_Shift)));
                mmdwSetRegister( P_0,            0);
                mmdwSetRegister( P_STRIDE,       sData.dwScreenWidthBytes);
                mmdwSetRegister( P_XY,           0x010001L);

                //### patch for lowres modes
                if ( wPhysResolution < MODE_RES_640x400 )
                {
                    if ( wDeviceId==D_S3VIRGEVX )
                    {
                        if ( (sData.dwScreenWidth==320) && (wBpp==8) )
                            mmdwSetRegister( 0x81f8, 0x07f007f0L ); //move secondary stream away
                        mmdwSetRegister( P_WH, WH((sData.dwScreenWidth << 1), sData.dwScreenHeight << 1) );
                    }
                    else // wDeviceId=D_S3VIRGEVX
                        mmdwSetRegister( P_WH, WH(sData.dwScreenWidth, sData.dwScreenHeight << 1) );
                }
                else // wPhysResolution
                    mmdwSetRegister( P_WH, WH(sData.dwScreenWidth, sData.dwScreenHeight) );

                mmdwSetRegister( S_WH,           WH(10, 2) );
                if ( S3MobileData.wSPStyle & SP_M5 )
                {
                    mmdwSetRegister(BLEND_CONTROL, 8<<Kp_Shift);//Kp=8, Ks=0;
                }
                else
                    mmdwSetRegister( BLEND_CONTROL,  POnS );
                mmdwSetRegister( OPAQUE_CONTROL, 0 );

                mmdwSetRegister( 0x8200, ((fifo_setting & 0x0000001fL) |
                                         ((fifo_setting & 0x000003e0L) << 1) |
                                         ((fifo_setting & 0x0000007c00L) << 2)));
        }

} /* TurnOnStreamsProcessorMode */

/*
 * TurnOffStreamsProcessorMode
 */
void FAR PASCAL TurnOffStreamsProcessorMode( void )
{
        sData.lpVisibleOverlay=0;       // clear overlay pointer manbug#1995


    if (S3MobileData.dwMobileCaps & MOBILE_FAMILY)
    {
        DisableStreamProcessor_Mobile();
        return;
    }

        bStreamProcessorFlags &= ~(SP_IN_ENHANCE_MODE | SP_ENABLED);
        S3MobileData.dwMobileCaps &= ~SP_ON;
        outpw(CRTC_REG, 0x4838);        // it's probably already unlocked ...
        outpw(CRTC_REG, 0xA039);

        outp(CRTC_REG, 0x67);
        if( (inp(CRTC_REG+1) & 0xc) != 0xc)
                return;


        // notify the MPEG driver to suspend, just like a DOS box
        // is becoming foreground.
        _asm mov ax, NOTIFY_MODE_CHANGE
        _asm int 2fh

        wait_for_idle();

//###
//we have to reprogram P_CONTROL with 0x0 before we turn off SP,
//fail to do so when black out the Windows screen after you ran
//modes (like 320x240x16) with MM8180[30-28]=001.
        if (wDeviceId == D_S3VIRGEVX)
                mmdwSetRegister( P_CONTROL,    0x00000000L);
        if (wBpp == 16)
                mmdwSetRegister( 0x8200, 0xf000L );
        else
                mmdwSetRegister( 0x8200, 0xc000L );
// Miscellaneous timeout register.
        mmdwSetRegister( 0x820c, 0x08080810L );
//      mmdwSetRegister( 0x8200, 0x3000L );

        WAIT_NO_VSYNC
        WAIT_FOR_VSYNC;

        outp(CRTC_REG, 0x67);
        outp(CRTC_REG+1, (inp(CRTC_REG+1) & 0xf3));

        P_CONTROL_FLG = 0;

        if (wDeviceId == D_S3VIRGEVX) {
                //patch for SC EDO

                WAIT_NO_VSYNC   //###
                WAIT_FOR_VSYNC

                //patch for screen shift

                outp(CRTC_REG, 0x63);
                outp(CRTC_REG+1, bCR63);                        //restore original CR63,CR6D
                     bCR63 = 0xff;
                outp(CRTC_REG, 0x6D);
                outp(CRTC_REG+1, bCR6D);
                     bCR6D = 0xff;

        }

        // undo various fix

//wjm   if( wBpp == 8 )
        if( (wBpp == 8) && (wDeviceId == D_S3VIRGE) )
                UnfixMissingPixels();

        if( (wBpp == 8) || (wBpp == 16) )
                RestoreCursorColor();

} /* TurnOffStreamsProcessorMode */

/*
 * DisableStreamProcessor_Mobile
 */
void FAR PASCAL DisableStreamProcessor_Mobile( void )
{
    BYTE bReg;

    outp(CRTC_REG, 0x67);
    bReg = inp(CRTC_REG+1) & 0x4;

    if (bReg != 0x4)
    return;

    wait_for_idle();
    WAIT_FOR_VSYNC;

    if ( S3MobileData.wSPStyle & SP_M5 )
    {
    outp(CRTC_REG, 0x87);
    outp(CRTC_REG+1, 0);
    }

    outp(CRTC_REG,0x67);
    outp(CRTC_REG+1,(inp(CRTC_REG+1) & 0xF3));

    if (S3MobileData.dwMobileCaps & PANNING_ENABLED)
    {
    SetDispStart();
    }

    dwFlipOffset = 0;

    if (S3MobileData.dwMobileCaps & FIX_XPARENT_CURSOR)
    {
        Switch2HWCursor();
    }

    S3MobileData.dwMobileCaps &= ~SP_ON;

} /* DisableStreamProcessorMobile */


/*
 * FixMissingPixels
 */

//On 1024x768x8 and 800x600x8 modes, switching from K2 to stream processor
//results in 1 character clock pixels on the right handed side of the screen
//missing. This problem can be worked-around by adjusting CR2 register.

static BYTE SavedCR2Reg = 0;

static void FixMissingPixels( void )
{
BYTE bTmp;

    outp( CRTC_REG, 0x11 );     //unlock CRTC control registers
    outp( CRTC_REG+1, inp( CRTC_REG+1 ) & 0x7f );

    outp( CRTC_REG, 2 );
    bTmp = inp( CRTC_REG+1 );   //content of cr2

    if( bTmp != SavedCR2Reg )
    {
        SavedCR2Reg = ++bTmp;   //add 1 character clock
        outp( CRTC_REG+1, bTmp );
    }

    outp( CRTC_REG, 0x11 );     //lock CRTC control registers
    outp( CRTC_REG+1, inp( CRTC_REG+1 ) | 0x80 );
}

/*
 * UnfixMissingPixels
 */

static void UnfixMissingPixels( void )
{
    outp( CRTC_REG, 0x11 );     //unlock CRTC control registers
    outp( CRTC_REG+1, inp( CRTC_REG+1 ) & 0x7f );

    outp( CRTC_REG, 2 );
    outp( CRTC_REG+1, --SavedCR2Reg );
    SavedCR2Reg = 0;

    outp( CRTC_REG, 0x11 );     //lock CRTC control registers
    outp( CRTC_REG+1, inp( CRTC_REG+1 ) | 0x80 );
}


/*
 * EnableCenteringInSPMode
 */
void FAR PASCAL EnableCenteringInSPMode( void )
{
WORD dstWidth, dstHeight;
WORD wWidth, wHeight;
DWORD dwTmp, dwTmp2;

    S3MobileData.dwMobileCaps |= CENTERING_ENABLED;
    S3MobileData.dwMobileCaps &= ~EXPANSION_ENABLED;

    // Initialize actual with and height

    wWidth  = wScreenWidth;
    wHeight = wScreenHeight;

    if (wWidth > wPhysScreenWidth)
        wWidth = wPhysScreenWidth;

    if (wHeight > wPhysScreenHeight)
        wHeight = wPhysScreenHeight;

    wCenterY = (WORD)(((dwPanelSize&0xffff) - wHeight)/2);
    wCenterX = (WORD)(((dwPanelSize>>16) - wWidth)/2);

    // adjust S_XY, S_WH, ...
    dwTmp  = dwSSXY & 0x7fff;   // SS height
    dwTmp2 = dwSSXY & 0x7fff0000;   // SS width
    dwTmp2 >>=16;

    dstWidth   = (WORD)(dwSSWH >> 16);
    dstHeight  = (WORD)(dwSSWH & 0x7fff);

    dwTmp2 +=wCenterX;
    dwTmp  +=wCenterY;
    dwTmp2 <<=16;
    dwTmp +=dwTmp2;

    mmdwSetRegister(S_XY, dwTmp);

    dwTmp2 = dstWidth;
    dwTmp  = dstHeight;
    dwTmp2 <<=16;
    dwTmp +=dwTmp2;

    mmdwSetRegister(S_WH, dwTmp);
    mmdwSetRegister(S_HK1K2, HK1K2((dwSrcWH >> 16), dstWidth));
    mmdwSetRegister(S_VK1, VK1(dwSrcWH & 0x7fff));
    mmdwSetRegister(S_VK2, VK2((dwSrcWH & 0x7fff), dstHeight));
    mmdwSetRegister(S_VDDA, VDDA(dstHeight));
    mmdwSetRegister(S_0, dwSSAddr);

} /* EnableCenteringInSPMode */


/*
 * DisableCenteringInSPMode
 */
void FAR PASCAL DisableCenteringInSPMode( void )
{
WORD dstWidth, dstHeight;
DWORD dwTmp, dwTmp2;

    //Clear the centering and expansion bits
    S3MobileData.dwMobileCaps &=~CENTERING_ENABLED;


    // adjust S_XY, S_WH, ...

    dwTmp  = dwSSXY & 0x7fff;   // SS height
    dwTmp2 = dwSSXY & 0x7fff0000;   // SS width
    dwTmp2 >>=16;
    dwTmp--;
    dwTmp2--;

    dstWidth   = (WORD)(dwSSWH >> 16);
    dstHeight  = (WORD)(dwSSWH & 0x7fff);

    mmdwSetRegister(S_XY, XY(dwTmp2, dwTmp));

    dwTmp2 = dstWidth;
    dwTmp  = dstHeight;
    dwTmp2 <<=16;
    dwTmp +=dwTmp2;

    mmdwSetRegister(S_WH, dwTmp);
    mmdwSetRegister(S_HK1K2, HK1K2((dwSrcWH >> 16), dstWidth));
    mmdwSetRegister(S_VK1, VK1(dwSrcWH & 0x7fff));
    mmdwSetRegister(S_VK2, VK2((dwSrcWH & 0x7fff), dstHeight));
    mmdwSetRegister(S_VDDA, VDDA(dstHeight));
    mmdwSetRegister(S_0, dwSSAddr);

} /* DisableCenteringInSPMode */

/*
 * DisableExpansionInSPMode
 */
void FAR PASCAL DisableExpansionInSPMode( void )
{
WORD dstWidth, dstHeight;
DWORD dwTmp, dwTmp2;

    //Clear the centering and expansion bits
    S3MobileData.dwMobileCaps &=~EXPANSION_ENABLED;


    // adjust S_XY, S_WH, ...

    dwTmp  = dwSSXY & 0x7fff;   // SS height
    dwTmp2 = dwSSXY & 0x7fff0000;   // SS width
    dwTmp2 >>=16;
    dwTmp--;
    dwTmp2--;

    dstWidth   = (WORD)(dwSSWH >> 16);
    dstHeight  = (WORD)(dwSSWH & 0x7fff);

    mmdwSetRegister(S_XY, XY(dwTmp2, dwTmp));

    dwTmp2 = dstWidth;
    dwTmp  = dstHeight;
    dwTmp2 <<=16;
    dwTmp +=dwTmp2;

    mmdwSetRegister(S_WH, dwTmp);
    mmdwSetRegister(S_HK1K2, HK1K2((dwSrcWH >> 16), dstWidth));
    mmdwSetRegister(S_VK1, VK1(dwSrcWH & 0x7fff));
    mmdwSetRegister(S_VK2, VK2((dwSrcWH & 0x7fff), dstHeight));
    mmdwSetRegister(S_VDDA, VDDA(dstHeight));
    mmdwSetRegister(S_0, dwSSAddr);

} /* DisableExpansionInSPMode */


/*
 * EnableExpansionInSPMode
 */
void FAR PASCAL EnableExpansionInSPMode( void )
{
    WORD dstWidth, dstHeight, dstY, dstX;
    DWORD dwVDDA;

    //Clear the centering and expansion bits

    S3MobileData.dwMobileCaps |=EXPANSION_ENABLED;
    S3MobileData.dwMobileCaps &= ~CENTERING_ENABLED;


    dstWidth   = (WORD)(dwSSWH >> 16);
    dstHeight  = (WORD)(dwSSWH & 0x7fff);

    dstY       = (WORD)(dwSSXY & 0x7fff);
    dstX       = (WORD)(dwSSXY >> 16);

    if (2*wScreenWidth <= wPhysScreenWidth)
    {
        dstWidth   =  dstWidth * 2;
        dstX       =  dstX * 2;
    } else
    {
        if (3*wScreenWidth <= 2*wPhysScreenWidth)
        {
             dstWidth   =  dstWidth * 3 / 2;
             dstX       =  dstX * 3 / 2;
        } else
        {
            if (5*wScreenWidth <= 4*wPhysScreenWidth)
            {
                dstWidth   =  dstWidth * 5 / 4;
                dstX       =  dstX * 5 / 4;
            }
        }
    }

    if (2*wScreenHeight <= wPhysScreenHeight)
    {
        dstHeight   =  dstHeight * 2;
        dstY        =  dstY * 2;
    } else
    {
        if (3*wScreenHeight <= 2*wPhysScreenHeight)
        {
             dstHeight   =  dstHeight * 3 / 2;
             dstY        =  dstY * 3 / 2;
        } else
        {
            if (4*wScreenHeight <= 3*wPhysScreenHeight)
            {
                dstHeight   =  dstHeight * 4 / 3;
                dstY        =  dstY * 4 / 3;
            } else
            {
                if (5*wScreenHeight <= 4*wPhysScreenHeight)
                {
                     dstHeight   =  dstHeight * 5 / 4;
                     dstY        =  (dstY * 5) / 4;
                } else
                {
                    if (6*wScreenHeight <= 5*wPhysScreenHeight)
                    {
                         dstHeight   =  dstHeight * 6 / 5;
                         dstY        =  dstY * 6 / 5;
                    }
                }
            }
        }
    }

    dstY  += wCenterY;
    dstX  += wCenterX;

    mmdwSetRegister(S_XY, (((DWORD)dstX) << 16) | dstY);
    mmdwSetRegister(S_WH, (((DWORD)dstWidth) << 16) | dstHeight);
    mmdwSetRegister(S_HK1K2,(((DWORD)((WORD)(dwSrcWH >> 16)-dstWidth ))<< 16) |
                             ((dwSrcWH >> 16) - 1));
    mmdwSetRegister(S_VK1, (dwSrcWH & 0x7fff) - 1);
    mmdwSetRegister(S_VK2, (WORD)(dwSrcWH & 0x7fff) - dstHeight);
    dwVDDA= mmdwGetRegister(S_VDDA);
    mmdwSetRegister(S_VDDA, (WORD)(1-dstHeight) | (WORD)(dwVDDA & 0xc000));

} /* EnableExpansionInSPMode */




/*
 * EnableExpansionCenteringInSPMode
 */
void FAR PASCAL EnableExpansionCenteringInSPMode( void )
{
    BYTE bCurrentDisplay;

    S3MobileData.dwMobileCaps &= ~EXPANSION_ENABLED;
    S3MobileData.dwMobileCaps &= ~CENTERING_ENABLED;

    outp(CRTC_REG, 0x6B);
    bCurrentDisplay = inp(CRTC_REG+1);

    outp(SEQ_REG, 0x31);

    if ( (bCurrentDisplay & FP_ONLY) &&
         (wPanelResolution > wLogResolution) )
    {
    outp(SEQ_REG, FP_HORZ_EXP_2);
    if ((!(inp(SEQ_REG+1) & ALL_H_EXP)) || (wLogResolution < MODE_RES_640x400))
    {
        EnableCenteringInSPMode();
    }
    else  // set  expansion flag
    {
        EnableExpansionInSPMode();
    }
   }

} /* EnableExpansionCenteringInSPMode */


/*
 * EnableUnderscanningInSPMode
 */
void FAR PASCAL EnableUnderscanningInSPMode ( void )
{
WORD dstWidth, dstHeight;
WORD srcWidth, srcHeight;
DWORD dwTmp, dwTmp2;
BYTE bReg;

    srcWidth   = (WORD)(dwSrcWH >> 16);
    srcHeight  = (WORD)(dwSrcWH & 0x7fff);

    dstWidth   = (WORD)(dwSSWH >> 16);
    dstHeight  = (WORD)(dwSSWH & 0x7fff);

    S3MobileData.dwMobileCaps &= ~UNDERSCAN_ENABLED;
    S3MobileData.dwMobileCaps &= ~TV_PAL;

    outp(CRTC_REG, 0x3D);
    bReg = inp(CRTC_REG+1);
    if (bReg&0x20)          //Is it PAL output
    {
        S3MobileData.dwMobileCaps |= TV_PAL;
        if (wLogResolution == MODE_RES_800x600)
        {
            S3MobileData.dwMobileCaps |= UNDERSCAN_ENABLED;

            srcHeight = 14*srcHeight/15;
            dstHeight = 14*dstHeight/15;

        }
    }
    else          //it is NTSC output
    {
        if (wLogResolution == MODE_RES_640x480)
        {
            S3MobileData.dwMobileCaps |= UNDERSCAN_ENABLED;

            srcHeight = 5*srcHeight/6;
            dstHeight = 5*dstHeight/6;

        }
    }

    //Update overlay surface
    if (S3MobileData.dwMobileCaps & UNDERSCAN_ENABLED)
    {

    dwTmp2 = dstWidth;
    dwTmp  = dstHeight;
    dwTmp2 <<=16;
        dwTmp +=dwTmp2;
    mmdwSetRegister(S_WH, dwTmp);

        mmdwSetRegister(S_VK1, VK1(srcHeight));
        mmdwSetRegister(S_VK2, VK2(srcHeight, dstHeight));
    mmdwSetRegister(S_VDDA, VDDA(dstHeight));
        mmdwSetRegister(S_0, dwSSAddr+sData.dwSSOffset);
    }

} /* EnableUnderscanningInSPMode */


/*
 * DisableUnderscanningInSPMode
 */
void FAR PASCAL DisableUnderscanningInSPMode ( void )
{
WORD dstWidth, dstHeight;
WORD srcWidth, srcHeight;
DWORD dwTmp, dwTmp2;


    srcWidth   = (WORD)(dwSrcWH >> 16);
    srcHeight  = (WORD)(dwSrcWH & 0x7fff);

    dstWidth   = (WORD)(dwSSWH >> 16);
    dstHeight  = (WORD)(dwSSWH & 0x7fff);

    //Update overlay surface
    if (S3MobileData.dwMobileCaps & UNDERSCAN_ENABLED)
    {
    dwTmp2 = dstWidth;
    dwTmp  = dstHeight;
    dwTmp2 <<=16;
    dwTmp +=dwTmp2;
    mmdwSetRegister(S_WH, dwTmp);

        mmdwSetRegister(S_VK1, VK1(srcHeight));
        mmdwSetRegister(S_VK2, VK2(srcHeight, dstHeight));
    mmdwSetRegister(S_VDDA, VDDA(dstHeight));
        mmdwSetRegister(S_0, dwSSAddr);
    }

    S3MobileData.dwMobileCaps &= ~UNDERSCAN_ENABLED;
    S3MobileData.dwMobileCaps &= ~TV_PAL;

} /* DisableUnderscanningInSPMode */

