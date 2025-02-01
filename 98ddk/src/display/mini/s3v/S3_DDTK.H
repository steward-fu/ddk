/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       s3ddtk.h
 *  Content:    Thunk connection between 16-bit S3 driver and 32-bit piece
 *  History:
 *
 ***************************************************************************/
#ifndef __S3DDTK_INCLUDED__
#define __S3DDTK_INCLUDED__

#include <windows.h>
#include <string.h>

#pragma warning( disable: 4704)

#include "ddrawi.h"
#include "mpc.h"
#ifdef DX5
#include "lpb.h"
#endif

#ifdef  S3VIRGE
#include "virge1.h"
#define S3DD32_DLLNAME  "s3_32v.dll"
#else
#define S3DD32_DLLNAME  "s3_32.dll"
#endif // S3VIRGE

#define HWCODEC          0
#define SWCODEC          1
#define DIGITIZER        2
#define MAX_DEVICE       3
#define DSTWIN_SIZES     5

typedef struct _k2vRatio
{
    DWORD       dstWidth;
    DWORD       value;

}K2VRATIO;

typedef struct _k2vFifo
{
    DWORD       dstWidth;
    DWORD       value;

}K2VFIFO;

typedef struct _k2vEntry
{
    K2VRATIO    ratio;
    K2VFIFO     fifo;

}K2VENTRY;

typedef struct _k2vTable
{
    K2VENTRY    wStep[DSTWIN_SIZES];

}K2VTABLE;

#define SP_BASE         0x8180
#define GE_BASE         0x8100
#define SP_SIZE         0x20
#define GE_SIZE         0x26

// Equates for b3DCaps
#define Virge_3D        0x1        //Virge 3D engine
#define VirgeGX_3D  0x2     //Virge/GX 3D engine

// Equates for bAPMEvents

#define HK_DISPLAY_SWITCH   0x1     //display switch
#define HK_CENTER_EXPAND    0x2
#define STD_IN_GE_FUNCTION  0x4
#define STD_IN_SP_FUNCTION  0x8
#define STD_RESTORE_GE_REGS 0x10
#define STD_RESTORE_SP_REGS 0x20

// Equates for wSPStyle
#define SP_Trio     0x0001
#define SP_Virge        0x0002
#define SP_VX           0x0004
#define SP_VirgeGX  0x0008
#define SP_TrioV2       0x0010
#define SP_M5           0x0020
#define SP_Yosemite 0x0040

// Equates for dwMobileCaps
#ifndef MOBILE_FAMILY
#define MOBILE_FAMILY       0x00000001
#endif

#define PANNING_ENABLED     0x00000002
#define DUAL_IMAGE          0x00000004   //duoview with diff images
#define DUOVIEW_SAMEIMAGE       0x00000008   //same-image, different-timing
#define EXPANSION_ENABLED       0x00000010
#define CENTERING_ENABLED       0x00000020
#define PANNING_IGA1            0x00000040
#define SP_IGA1             0x00000080   //SP on IGA1
#define DSTN_ON             0x00000100   //STN pannel
#define SP_ON               0x00000200   //Streams processor is on
#define UNDERSCAN_ENABLED       0x00000400   //Underscanning is enabled
#define TV_PAL              0x00000800   //TV output is PAL
#define USER_SP_IGA1            0x00001000
#define PANNING_CENTER          0x00002000
#define VIDEO_ON_TV     0x00004000
#define PANNING_IGA1_IGA2   0x00008000

#define FIX_XPARENT_CURSOR          0x00010000   //disable SP in exp & duoView
#define LCD_IGA1_CRT_IGA2       0x00020000
#define SINGLE_DISPLAY          0x00040000
#define REJECT_MODESWITCH_IN_SP 0x00080000
#define M65_NEW_FIFO_CONTROL    0x00100000   //M1, rev. E : sr3f[7] = 1 :

#define PANNING_ON_CRT          0x00200000
#define DUOVIEW_DIFF_REFRESH    0x00400000
#define FORCED_VIDEO_CENTERING  0x00800000   //User wants video in centering
#define DEFAULT_TO_SIM          0x01000000   //set sim when panel size =
#define IN_DISPLAY_SWITCH       0x02000000
#define DUOVIEW_IN_8BPP         0x04000000
#define IN_FLIPPING             0x08000000
#define HIDE_OVERLAY            0x10000000
#define MULTI_UNITS             0x20000000


typedef struct
{
    WORD    wSize;
    BYTE    bCurrentDisplay;
    BYTE    bAPMEvents;
    DWORD   dwMobileCaps;
    WORD    GERegs[GE_SIZE];
    DWORD   SPRegs[SP_SIZE];
    WORD    wSPStyle;
    BYTE    b3DCaps;
    BYTE    Reserved;  //make it word aligned
} S3MOBILEDATA;

typedef S3MOBILEDATA FAR *LPS3MOBILEDATA;

typedef struct
{
   DWORD    InSrcBltAddress;
   BYTE     MCLKData;
   BYTE     VideoData;
   BYTE     VGAFlag;
   BYTE     reserved;
   WORD     DACType;
   DWORD    dwRasPreCharge;
   WORD     wPanelResolution;
   WORD     wLogResolution;
   WORD     wPhysResolution;
   DWORD    LAGEOnAddress;
   WORD     ScreenWidth;
   WORD     ScreenHeight;
} S3DRIVERDATA;

typedef S3DRIVERDATA FAR *LPS3DRIVERDATA;

// --------------------------------------------------------------------------

typedef struct tagSPOVERLAYDESC
{
    // NOTE the 16bit display driver assumes the first DWORD is
    // the DDRAWSURFACE_LCL, it will zero it when being free'ed
    LPDDRAWI_DDRAWSURFACE_LCL       lpDDSrcSurface;
    struct tagSPOVERLAYDESC *next;
    DWORD   dwS_0;
    DWORD   dwS_xy;
    DWORD   dwS_wh;
    DWORD   dwS_stride;
    DWORD   dwS_control;
    DWORD   dwS_hk1k2;
    DWORD   dwS_vk1;
    DWORD   dwS_vk2;
    DWORD   dwS_vdda;
    DWORD   dwC_low;
    DWORD   dwC_hi;
    DWORD   dwBlend;
    DWORD   dwOpaque;
    DWORD   dwFifo;

} SPOVERLAYDESC;
typedef SPOVERLAYDESC FAR *LPSPOVERLAYDESC;

// --------------------------------------------------------------------------
typedef struct
{
    FLATPTR     fpFlipFrom;
#ifdef IS_32
    __int64     liFlipTime;
#else
    DWORD       liFlipTime;
    DWORD       _liFlipTime;
#endif
    DWORD       dwFlipDuration;
#ifdef IS_32
    BOOL        bReaquire;
    BOOL        bHaveEverCrossVBlank;
    BOOL        bWasEverInDisplay;
    BOOL        bFlipFlag;
#else
    DWORD       bReaquire;
    DWORD       bHaveEverCrossVBlank;
    DWORD       bWasEverInDisplay;
    DWORD       bFlipFlag;
#endif
    DWORD               dwFlipScanLine;
} FLIPRECORD;
typedef FLIPRECORD FAR *LPFLIPRECORD;

#ifdef  S3VIRGE
#define __UVBASEDEL          0.01f

typedef struct {

    struct _S3_CONTEXT    *FirstContxt;
    /* hw parm regs for 3D triangle */
    volatile        vi13D_TRIANGLE FAR *g_p3DTri;
    /* hw setup regs for 3D triangle */
    volatile        vi13D_SETUP FAR *g_p3DStp;
#ifdef IS_32
    BOOL            bInitialized;
    BOOL            DMA_Choice;
#else
    DWORD           bInitialized;
    DWORD           DMA_Choice;
#endif
    DWORD           dwWhiteTexture;
    volatile ULONG  dma_possible;
    float           __UVRANGE;
    float           coord_adj;
    ULONG           DXGX;
    ULONG           uBaseHigh;
    ULONG           g_DmaReadPtr;
    ULONG           g_DmaIndex;
    ULONG           CMD_SET ;
    ULONG           *g_DmaLinAddr ;
    volatile ULONG  *g_lpReadPtrReg;
    volatile ULONG  *g_lpWritePtrReg;
    volatile ULONG  *g_lpDmaEnableReg;
    ULONG           bCheckUV;
    ULONG           bFastBilinear;
    ULONG           bCommandDMA;


} D3DGLOBALS;
typedef D3DGLOBALS FAR * LPD3DGLOBALS;
#endif  //S3VIRGE

typedef struct {
    DWORD                    dwSize; // size of this structure
    DWORD FAR                  *lpSrcWH;
    DWORD FAR                  *lpSSXY;
    DWORD FAR                  *lpSSWH;
    DWORD FAR                  *lpSSAddr;
    DWORD FAR                  *lpVWOffset;
    DWORD FAR                  *lpVWBottom;
    DWORD FAR                  *lpVWTop1;
    DWORD FAR                  *lpVWBottom1;
    DWORD FAR                  *lpVWTop2;
    DWORD FAR                  *lpVWBottom2;
    DWORD FAR                  *lpCenterOffset;
    DWORD FAR                  *lpPSAddr;
    DWORD FAR                  *lpPSOffset;
    DWORD FAR                  *lpFlipOffset;
    DWORD FAR                  *lpClipOffset;
    DWORD FAR                  *lpMobileCaps;
    DWORD FAR                  *lpVideoType;
    DWORD FAR                  *lpBlendCtrl;
    DWORD FAR                  *lpSPFIFOReg;
    BYTE  FAR                  *lpShowSSWindow;
    LPS3MOBILEDATA              lpS3MobileData;
    LPS3DRIVERDATA              lpS3DriverData;
    DWORD                       dwPanelSize;
    DWORD FAR                  *lpColorAdjustFlags;
    DWORD FAR                  *lpColorAdjustReg;
    DWORD                       lpMMReg;
    DWORD                       dwScreenFlatAddr;
    DWORD               dwScreenWidth;
    DWORD               dwScreenHeight;
    DWORD               dwScreenWidthBytes;
    DWORD               dwPhysScreenWidth;
    DWORD               dwPhysScreenHeight;
    DWORD               dwVideoMemorySize;
    DWORD               lpCurrentChipMode;
    LPDDRAWI_DDRAWSURFACE_LCL   lpVisibleOverlay;   //manbug#1995
    DDPIXELFORMAT           ddpf;
     DDHALINFO          ddHALInfo;
    LPDDHALSURFCB_BLT           lpBlt32;
    LPDDHALSURFCB_BLT           lpBlt32_968;
    LPDDHALSURFCB_FLIP          lpFlip32;
    LPDDHALSURFCB_LOCK          lpLock32;
    LPDDHALSURFCB_UNLOCK        lpUnLock32;
    LPDDHALSURFCB_GETBLTSTATUS      lpGetBltStatus32;
    LPDDHALSURFCB_GETFLIPSTATUS     lpGetFlipStatus32;
    LPDDHALSURFCB_SETCOLORKEY       lpSetColorKey32;
    LPDDHALSURFCB_UPDATEOVERLAY     lpUpdateOverlay32;
    LPDDHALSURFCB_SETOVERLAYPOSITION    lpSetOverlayPosition32;
    LPDDHALSURFCB_DESTROYSURFACE    lpDestroySurface32;
    LPDDHALSURFCB_ADDATTACHEDSURFACE    lpAddAttachedSurface32;  //mipmap
    LPDDHAL_WAITFORVERTICALBLANK    lpWaitForVerticalBlank32;
    LPDDHAL_CANCREATESURFACE        lpCanCreateSurface32;
    LPDDHAL_CREATESURFACE       lpCreateSurface32;
    LPDDHAL_GETSCANLINE         lpGetScanLine32;
    DWORD               bReset;
    DWORD               bUsingVFlatD;
    DWORD               wChipId;
    DWORD               wDeviceId;
    DWORD               bRevisionId;
    DWORD               dwSpecialHW;
    DWORD               bFamilyFlags;
    DWORD               dwMULT_MISC;
    K2VTABLE            k2vTable[MAX_DEVICE];
    BAND                band;
    DWORD               dwPFRgnOffset;
    WORD                wVWTop;
    WORD                wVWLeft;
    DWORD FAR           *lpPS2Offset;
    DWORD FAR           *lpTVCenterAdj;
    DWORD               dwSSOffset;
    DWORD FAR           *lpbStreamProcessorFlags;
//
// new for Multi monitor support DPIKE 1/10/97
//
        FLIPRECORD      flipRecord;
        DWORD           dwFlipFail;
        DWORD           dwUpdateOverlayFlipOffset;
        WORD  FAR     *lpLinearCurrentChipMode;
        DWORD           S3SP[32];
        volatile  DWORD S3SP_SCRATCH[32];
        DWORD           dwRasPreCharge;
        DWORD           dwDeviceHandle;
        DWORD FAR       * lpColorTable;       // points to color table
        WORD            wPrevChipMode;
        BYTE            bWaitForVSync;
        BYTE            bytesPerPixel;
        DWORD           dwSrcWH;
        BOOL            (*lpdraw_engine_busy)();
        BOOL            (*lproom_for_blt)();
        void            (*lpwait_for_idle)();
        void            (*lpwrite_mm8200)();

        volatile long   read_ptr;
        volatile long   write_ptr;

        LPSPOVERLAYDESC lpSpod;
        LPSPOVERLAYDESC lpSpodList;

#ifdef S3VIRGE  // these variables are for D3D
        D3DGLOBALS      D3DGlobals;     // these only compile with 32 bit compiler.
#endif    // S3VIRGE
// These are new for DX5/VPE support
        LPDDHAL_GETDRIVERINFO   lpGetDriverInfo32;
#ifdef DX5
        LPB             LPBData;
        DDKERNELCAPS    KernelCaps;
#endif
        LPDDRAWI_DDRAWSURFACE_LCL lpDstSaved;
        DWORD           dwColorAdjustDefault; // Default HSB setting for all YUV surfaces
} S3THKINFO;

typedef S3THKINFO FAR *LPS3THKINFO;

extern LPS3THKINFO sDataGlobal;
#define VERIFYSDATA(x) (((x) ? x : sDataGlobal))
typedef LPS3THKINFO (FAR PASCAL *LPGETDATAPTR)(void);

typedef double doubletest;
typedef float floattest;

#define CAssert(f)  switch (0)  case 0: case f:
#define TRINDEX_GenericTri              0
#define TRINDEX_TGZTri                  1
#define TRINDEX_TGFZTri                 2
#define TRINDEX_GenericWireFrame        3
#define TRINDEX_GenericPointTri         4
#define TRINDEX_ChopInUVSpace           5
#define MAXNUMTRIFUNCS                  64

#endif
