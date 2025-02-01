#ifndef MPC_H
#define MPC_H

#ifndef _H2INC

#define ID_MAXCODEC 3

#include "coretype.h"

typedef short SHORT;

/* Fifo Data needed to be calcuated */
typedef struct _FIFO
{
   BYTE    PS1Threshold;                 /* Primary stream 1 threshold    */
   BYTE    PS2Threshold;                 /* Primary stream 2 threshold    */
   BYTE    SSThreshold;                  /* Secondary stream threshold    */
   BYTE    Reserved;                     /* Reserved */
} FIFO, *PFIFO;


/* Data needed to calcuate available and used bandwidth (for decimation) */
typedef struct _BAND
{
   DWORD   SWFlags;                      /* Enabled Software Features     */
   DWORD   HWCaps;                       /* Available Hardware Features   */
   DWORD   HWFlags;                      /* Enabled Hardware Features     */
   WORD    DuoViewFlags;                 /* Flags for DuoView             */
   WORD    SPStyle;                      /* Stream processor style        */

   WORD    id;                           /* chip id                       */
   BYTE    fam;                          /* chip family                   */

   BYTE    MemType;                      /* ( CR36 & 0x0C ) >> 2          */
   WORD    BMemWidth;                    /* Memory width in bytes         */
   WORD    MemCycles;                    /* Memory cycles                 */
   WORD    MHzMCLK;                      /* Memory Clock                  */

   WORD    MBsTotal;                     /* Total Bandwidth               */
   WORD    MBsPrimary;                   /* Bandwidth Consumed by PS      */

   WORD    BFifo;                        /* SS FIFO size in bytes         */
   WORD    BFifoVFilter;                 /* SS FIFO size with V. filter   */
   WORD    BBlank;                       /* FIFO bytes filled during blank*/
   WORD    PcntMemUse[ID_MAXCODEC+1];    /* VIDEO-PCNTMEMUSE              */
   WORD    PcntBlankUse[ID_MAXCODEC+1];  /* VIDEO-PCNTBLANKUSE            */
   WORD    FSBltPerSec;                  /* VIDEO-FSBLTPERSEC             */
   WORD    HStretch;                     /* HStretch for no decimation    */
   WORD    HStretchVFilter;              /* for no decimation with VFILTER*/

   WORD    GBPP;                         /* Graphics mode BPP             */
   WORD    GSizeX;                       /* Graphics mode size  x         */
   WORD    GSizeY;                       /* Graphics mode size  y         */
   WORD    GRefreshRate;                 /* RefreshRate                   */
   WORD    MHzDCLK;                      /* Pixel (Dot) Clock             */

   WORD    GSizeX2;                      /* DuoView mode2 size  x         */
   WORD    GSizeY2;                      /* DuoView mode2 size  y         */
   WORD    GRefreshRate2;                /* RefreshRate2                  */
   WORD    MHzDCLK2;                     /* Pixel (Dot) Clock2            */

   union
   {
        DWORD   FifoCtrl;                /* FIFO control MM81EC format    */
        FIFO    Fifo;                    /* FIFO control individually     */
   };

   union
   {
        BYTE    M1;                      /* CR54 mclk count               */
        BYTE    PS1TimeOut;              /* IGA1 Primary Stream time out  */
   };

   union
   {
        BYTE    M2;                      /* CR60 mclk count               */
        BYTE    PS2TimeOut;              /* IGA2 Primary Stream time out  */
   };

   union
   {
        BYTE    M3;                      /* CR72 mclk count               */
        BYTE    SSTimeOut;               /* Secondary Stream time out     */
   };

   BYTE    STNRdThreshold;               /* STN Read Fifo threshold       */
   BYTE    STNWrThreshold;               /* STN Write Fifo threshold      */

   BYTE    STNRdTimeOut;                 /* STN read time out             */
   BYTE    STNWrTimeOut;                 /* STN write time out            */
   BYTE    FRCBPP;                       /* BPP for frame rate control    */

   WORD    SPDCLK_Cutoff;                /* Streams processor DCLK cutoff */
   BYTE    IsSecFormatYUV;               /* Secondary Stream Format       */
   BYTE    UMAFactor;                    /* Factor used for system bandwidth*/

   WORD    Latency[5];                   /* CPU/BLT latancy in MCLK       */
   WORD    Random_Read[5];               /* random read in MCLK           */
   WORD    Page_Miss[5];                 /* Page miss in MCLK             */


} BAND, *PBAND;

#endif

/* HWCaps & HWFlags */
#define HW_VFILTER        0x0001       /* Vertical Interpolation */
#define HW_VFILTERYONLY1  0x0002       /* 81E8_15 (for quality purposes) */
#define HW_VFILTERYONLY2  0x0004       /* CR87_0 (for VFILTER FIFO fit) */
#define HW_SAVEMODE1      0x0008       /* only on with no VFILTER */
#define HW_SAVEMODE2      0x0010       /* only on with VFILTER */
#define HW_1CYCLELPB      0x0020       /* blue dots if not set when needed */
#define HW_YUVDECIMATION  0x0040       /* enable YUV-aware LPB decimation */
#define HW_RASLOW25       0x0080       /* 81EC_15 needs set for older BIOS */
#define HW_SETSSFETCH     0x0100       /* program L parameters for ss stream */
#define HW_DECIMATE411    0x0200       /* turn on special 422->411 dec */
#define HW_SETM1          0x0400       /* set CR54 MCLK counts */
#define HW_SETM2          0x0800       /* set CR60 MCLK counts */
#define HW_SETM3          0x1000       /* set CR72 MCLK counts */
#define HW_DOUBLEBUFFER   0x2000       /* LPB-Stream double buffering */
#define HW_DUALFIFO       0x4000       /* 2 stream FIFOs? */
#define HW_EARLYHSYNCFIX  0x8000       /* set FF00_20, new on 775 Rev A1 */
#define HW_UMA            0x00010000   /* UMA? */
#define HW_DUOVIEW        0x00020000   /* DUOVIEW? */


/* SWFlags */
#define SW_CALCULATE      0x0001       /* are calculations enabled? */
#define SW_DECIMATION     0x0002       /* is decimation enabled? */
#define SW_YUVDECIMATION  0x0008       /* use YUV422 decimation table */
#define SW_FIXLVIDEO      0x0010       /* temp: don't use V. filter or
                                          new decimation for LVIDEO */
#define SW_FIXWORDALIGN   0x0020       /* 2 bits/word alignment in FF2C */
#define SW_YUV8LEVEL      0x0080       /* 8 levels of YUV specific data */
#define SW_FIX988VFILTER  0x0100
#define SW_FIX988DECIMATION 0x0200
#define SW_INTERLACED     0x0400       /* We are in an interlaced mode */
#define SW_HANDSOFFRAS    0x0800       /* Don't touch RAS timings */
#define SW_NOTSPT24BOVLY  0x1000       /* Don't support 24BPP overlay */
#define SW_NOTSPT32BOVLY  0x2000       /* Don't support 32BPP overlay */
#define SW_INDEXCOLORKEY  0x4000       /* Support Color Key based on index */
#define SW_SETCR23CR26    0x8000       /* For Virge/DX/GX Kludge */


/* DUOVIEW Flags, use the same values as in display driver */
#define DVW_MobilePart        0x0001       /* Is it a mobile part   */
#define DVW_Panning_ON        0x0002       /* Is panning on         */
#define DVW_DualImage         0x0004       /* Is Dual Image         */
#define DVW_DualViewSameImage 0x0008       /* Is DuoView same image */
#define DVW_Expand_ON         0x0010       /* Is LCD expanding on   */
#define DVW_Center_ON         0x0020       /* Is LCD centering on   */
#define DVW_Panning_IGA1      0x0040       /* Is Panning on IGA1    */
#define DVW_SP_IGA1           0x0080       /* Is Stream on IGA1     */
#define DVW_DSTN_ON           0x0100       /* Is DSTN pannel        */
#define DVW_SP_ON             0x0200       /* Is Streams processer on*/


/* CODEC ID */
#ifndef ID_HWCODEC
#define ID_HWCODEC 1
#endif

#ifndef ID_SWCODEC
#define ID_SWCODEC 2
#endif

#ifndef ID_LVIDEO
#define ID_LVIDEO  3
#endif

#ifndef _H2INC

/*
** Bandwidth_Init()
**    Called once at initialization, takes chip and video memory specific
**    parameters.
**
** PBAND pb:     This structure needs to be allocated by the client of the
**               bandwidth library. A pointer is to the structure is passed
**               to each Bandwidth_ API.  The structure is the complete
**               instance data for the library.
** BYTE fam:     This is CR2D
** WORD id:      This is CR2D in the high byte and CR2E in the low byte
** SHORT mType:  0 or 1 for 1-cycle memory, 2 or 3 for 2-cycle memory
**               generally, bits 2-3 of CR36
** SHORT mclk:   This is the memory clock derived from SR10 and SR11, in
**               units of MHz
** SHORT mWidth: 0 for 32bit memory width, 1 for 64bit memory width
**               generally, 1MB VRAM is 32bit width, all others are 64.
*/
void  Bandwidth_Init(PBAND pb, BYTE fam, WORD id,
                     SHORT mType, SHORT mclk, SHORT mWidth);


/*
** Bandwidth_SetPrimary()
**    Called whenever the Primary Stream (display mode or refresh rate)
**    changes.
**
** SHORT dclk:   Pixel (dot) clock back-calculated from SR12 & SR 13
** SHORT xRes:   Screen X Resolution in pixels
** SHORT yRes:   Screen Y Resolution in pixels
** SHORT bpp:    Bits per pixel (8, 16, 24, 32)
** BYTE fInterlaced: True if interlaced mode (check CR42_5)
** SHORT dclksduringBlank: calculated from CR 2 & 3 + 5, 5D. Use VESA
**               timing spec to verify if calculations are correct
*/
void  Bandwidth_SetPrimary(PBAND pb, SHORT dclk, SHORT xRes, SHORT yRes,
                           SHORT bpp, BYTE fInterlaced, SHORT dclksDuringBlank);

/*
** Bandwidth_SetSecondary()
**    Called whenever the Secondary Steram (UpdateOverlay) changes, AND/OR
**    must be called after SetPrimary before HStrech and FIFO values are valid
**
** WORD device:  SWCODEC, HWCODEC, or DIGITIZER (RFMK2V definitions)
** SHORT bpp:    Bits per pixel of the overlay stream
** SHORT xSrc:   Source width of overlay stream
** SHORT ySrc:   Source height of overlay stream
** SHORT xTrg:   Target width of overlay stream
** SHORT yTrg:   Target height of overlay stream
** SHORT xMax:   Maximum width of overlay stream source
** SHORT yMax:   Maximum height of overlay stream source
*/
void  Bandwidth_SetSecondary(PBAND pb, WORD device, SHORT bpp,
      SHORT xSrc, SHORT ySrc, SHORT xTrg, SHORT yTrg, SHORT xMax, SHORT yMax);

/*
** Bandwidth_Exit()
**    Called at exit for any cleanup.
*/
void  Bandwidth_Exit(PBAND pb);

/*
 * Given the screen width, find out the maximum DCLK in MCLK  for the mode.
 * i.e. the DCLK for the 85Hz refresh rate
 */
WORD GetMaxDCLK(WORD wScreenWidth);


#endif /* H2INC */
#endif /* INCLUDE */
