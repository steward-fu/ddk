/**********************************************************************
** Streams Bandwidth Constraints calculations
**
** Description:
**
***********************************************************************/

/* common definitions that should probably be picked up from some header */
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define DEV_TRIOV   0x8811
#define DEV_TRIOV2  0x8901
#define DEV_VIRGEVX 0x883D
#define DEV_VIRGEGX 0x8A01
#define DEV_VIRGE   0x5631
#define DEV_TRIOUVP 0x8814
#define DEV_YOSE    0x8902
#define DEV_M1      0x8812
#define DEV_M3      0x8C00
#define DEV_M5      0x8C01
#define DEV_M5P     0x8C02
#define DEV_M5PM    0x8C03
#define DEV_GX2     0x8A10
#define DEV_GX2P    0x8A12
#define DEV_GX2PM   0x8A11

#define EDO1cyc     0
#define SGRAM       1
#define ED02cyc     2
#define FPAGE       3
#define SDRAM       4

#ifdef	OS2_DRV
typedef short SHORT;
#endif

#ifndef LINEARPM
#include <windows.h>
#endif
#include "mpc.h"
#include "s3_ddtk.h"
//#include <conio.h>

/* KV2 MM81EC SS Slots definitions */
#define KV2_SSS_0          0x00
#define KV2_SSS_16         0x04
#define KV2_SSS_24         0x08
#define KV2_SSS_32         0x0C
#define KV2_SSS_40         0x10
#define KV2_SSS_44         0x14
#define KV2_SSS_48         0x18
#define KV2_SSS_64         0x1C
#define KV2_SSS_90         0x02

#define YOSE_SSS_0         0x00
#define YOSE_SSS_32        0x02
#define YOSE_SSS_24        0x03
#define YOSE_SSS_16        0x04
#define YOSE_SSS_48        0x07

#ifdef DEBUG
#ifndef DOS
#ifndef WIN95
   extern void _near dprintf(char *szFormat, ...);
   #define DPRINTF(x) dprintf##x
#else
   void __cdecl Msg(LPSTR szFormat, ...);
   #define DPRINTF(x) Msg##x
#endif
#else
   #define DPRINTF(x) ((void) 0)
#endif

#else
   #define DPRINTF(x) ((void) 0)
#endif

#define MIN_HSTRETCH (WORD)64
#define MAX_HSTRETCH (WORD)((1<<16)-1)
#define SETCR54(x) ((x & 0x1F) << 3) | ((x & 0x20) >> 5))
#define RANGE(x, old, new) (((DWORD)x * (DWORD)new + (DWORD) (old/2) ) / (DWORD)old )
#define FIFOCTRL(x, y, z) ((DWORD)x | (DWORD)(y<<5) | (DWORD)(z<<10))
#define FIFOCTRLV2(x,y,z) ((DWORD)x | (DWORD)(y<<3) | (DWORD)(z<<8))
#define FIFOCTRLYOSE(x,y,z,a)   ((DWORD)x | (DWORD)y<<3 | (DWORD)z<<9 | (DWORD)a<<19 | (DWORD)a<<25)

#ifndef WIN32
extern void GetOptionFlag(char *key, WORD *pflags, WORD flag);
extern SHORT GetOptionInt(char *key, WORD deflt);

/*---------------------------------------------------------------------------
 *
 * Bandwidth_Init
 *
 * One-time initialization for bandwidth/decimation/FIFO calculations
 *
 */
void  Bandwidth_Init(PBAND my, BYTE fam, WORD id, SHORT mType, SHORT mclk, SHORT mWidth)
{
   WORD PcntMemUse[ID_MAXCODEC+1] = {0,90,90,90};
   WORD PcntBlankUse[ID_MAXCODEC+1] = {0,100,100,100};
   WORD FSBltPerSec = 40;
   WORD *pSWFlags = (WORD *)&(my->SWFlags);
   WORD *pHWCaps = (WORD *)&(my->HWCaps);
   int i;

   my->fam = fam;
   my->id = id;
   my->MemType = (BYTE)mType;

   /*
   ** Set up bandwidth datastructure
   ** The key result is my->MBsTotal, which represents
   ** the total amount of memory bandwidth on the system
   */

   /***************************
   ** Initialize software and hardware features
   ** Many of these things do not directly affect bandwidth
   ** If we restructure the streams, LPB, etc. code to
   ** use a similar code structure, we should move some
   ** of the flags out of this module
   */

   /* defaults for all chips */
   my->SWFlags = (SW_DECIMATION);
   my->HWFlags = 0;
   my->HWCaps  = (HW_1CYCLELPB);
   my->FifoCtrl = FIFOCTRL(12, 9, 7);
   my->HStretch = 1000;
   my->HStretchVFilter = 1000;
   my->BFifo = 12 * 8;
   my->BFifoVFilter = 12 * 8;

   /* chip-specific overrides */
   switch(my->id)
   {
      case DEV_VIRGEVX:
         my->SWFlags |= (SW_CALCULATE | SW_YUVDECIMATION |
                         SW_FIX988DECIMATION |
                         SW_NOTSPT32BOVLY);
         my->HWCaps |= (HW_VFILTER);
         my->FifoCtrl = FIFOCTRL(12, 9, 7);
         PcntMemUse[ID_LVIDEO] = 60;
         PcntMemUse[ID_HWCODEC] = 65;
         FSBltPerSec = 16;
         my->SPDCLK_Cutoff = 80;     //80Mhz
         break;
      case DEV_VIRGEGX:
         my->SWFlags |= (SW_CALCULATE | SW_YUVDECIMATION |
                        SW_FIXWORDALIGN | SW_HANDSOFFRAS |
                        SW_NOTSPT32BOVLY | SW_INDEXCOLORKEY |
                        SW_SETCR23CR26);
         my->HWCaps |= (HW_VFILTER    | HW_VFILTERYONLY1|
                        HW_DECIMATE411 | HW_SAVEMODE1    |
                        HW_SAVEMODE2   | HW_DUALFIFO     |
                        HW_SETSSFETCH | HW_YUVDECIMATION |
                        HW_SETM1 | HW_SETM2 | HW_SETM3 |
                        HW_VFILTERYONLY2 | HW_DOUBLEBUFFER |
                        HW_EARLYHSYNCFIX);
         my->HWFlags |= (HW_SETSSFETCH | HW_YUVDECIMATION |
                        HW_DOUBLEBUFFER | HW_1CYCLELPB |
                        HW_SETM1 | HW_SETM2 | HW_SETM3 |
                        HW_EARLYHSYNCFIX);
         my->FifoCtrl = FIFOCTRL(KV2_SSS_90, 11, 8);
         for(i=0;(i <= ID_MAXCODEC);i++) PcntMemUse[i] = 95;
         PcntBlankUse[ID_LVIDEO] = 75;
         my->BFifo = 90 * 8;
         my->BFifoVFilter = 32 * 8;
         my->M1 = 0x2c;
         my->M2 = 0x69;
         my->M3 = 0x60;
         FSBltPerSec = 10;
         my->SPDCLK_Cutoff = 80;     //80Mhz
         break;
      case DEV_TRIOV2:
         my->SWFlags |= (SW_CALCULATE | SW_YUVDECIMATION |
                        SW_FIXWORDALIGN | SW_HANDSOFFRAS |
                        SW_NOTSPT24BOVLY | SW_INDEXCOLORKEY);
         my->HWCaps |= (HW_VFILTER    | HW_VFILTERYONLY1|
                        HW_DECIMATE411 | HW_SAVEMODE1    |
                        HW_SAVEMODE2   | HW_DUALFIFO     |
                        HW_SETSSFETCH | HW_YUVDECIMATION |
                        HW_SETM1 | HW_SETM2 | HW_SETM3 |
                        HW_VFILTERYONLY2 | HW_DOUBLEBUFFER |
                        HW_EARLYHSYNCFIX);
         my->HWFlags |= (HW_SETSSFETCH | HW_YUVDECIMATION |
                        HW_DOUBLEBUFFER | HW_1CYCLELPB |
                        HW_SETM1 | HW_SETM2 | HW_SETM3 |
                        HW_EARLYHSYNCFIX);
         my->FifoCtrl = FIFOCTRL(KV2_SSS_90, 11, 8);
         for(i=0;(i <= ID_MAXCODEC);i++) PcntMemUse[i] = 95;
         PcntBlankUse[ID_LVIDEO] = 75;
         my->BFifo = 90 * 8;
         my->BFifoVFilter = 32 * 8;
         my->M1 = 0x2c;
         my->M2 = 0x69;
         my->M3 = 0x60;
         FSBltPerSec = 10;
         my->SPDCLK_Cutoff = 80;     //80Mhz
         break;
      case DEV_TRIOV:
      case DEV_TRIOUVP:
         my->SWFlags |= (SW_NOTSPT24BOVLY);
         break;

      case DEV_YOSE:
         my->SWFlags |= (SW_CALCULATE | SW_YUVDECIMATION |
                        SW_FIXWORDALIGN | SW_HANDSOFFRAS |
                        SW_NOTSPT24BOVLY);
         my->HWCaps |=  (HW_SETSSFETCH | HW_YUVDECIMATION |
                        HW_SETM1 | HW_SETM2 | HW_UMA);
         my->HWFlags |= (HW_SETSSFETCH | HW_YUVDECIMATION |
                        HW_SETM1 | HW_SETM2 | HW_1CYCLELPB);
         my->FifoCtrl = FIFOCTRLYOSE(YOSE_SSS_24,8,16,23);
         for(i=0;(i <= ID_MAXCODEC);i++) PcntMemUse[i] = 95;
         PcntBlankUse[ID_LVIDEO] = 75;
         my->BFifo = 24 * 8;
         my->M1 = 0x2c;
         my->M2 = 0x17;
         FSBltPerSec = 10;
         my->UMAFactor = 70;
         break;

      case DEV_VIRGE:
         PcntMemUse[ID_SWCODEC] = 70;
         my->SWFlags |= (SW_NOTSPT32BOVLY);
         my->SWFlags |= (SW_NOTSPT24BOVLY);
         my->HWCaps |= HW_RASLOW25;
         break;

      case DEV_M1:
         my->SWFlags |= (SW_NOTSPT32BOVLY);
         my->SWFlags |= (SW_NOTSPT24BOVLY);
         break;

      case DEV_M3:
      case DEV_M5:
      case DEV_M5P:
      case DEV_M5PM:
      case DEV_GX2:
      case DEV_GX2P:
      case DEV_GX2PM:
         my->SWFlags |= SW_CALCULATE | SW_YUVDECIMATION |
                        SW_FIXWORDALIGN |
                        SW_NOTSPT32BOVLY | SW_INDEXCOLORKEY;
         my->SWFlags |= SW_NOTSPT24BOVLY;
         my->HWCaps |=  HW_VFILTER    | HW_DECIMATE411 |
                        HW_SAVEMODE2   | HW_DUALFIFO     |
                        HW_SETSSFETCH | HW_YUVDECIMATION |
                        HW_DUOVIEW |
                        HW_DOUBLEBUFFER | HW_EARLYHSYNCFIX;
         my->HWFlags |= HW_SETSSFETCH | HW_YUVDECIMATION |
                        HW_DOUBLEBUFFER |
                        HW_EARLYHSYNCFIX;

         my->BFifo = 720 * 2;
         my->BFifoVFilter = 24 * 16;
         FSBltPerSec = 10;
         my->SPDCLK_Cutoff = 135;     //135Mhz

         for(i=0;(i <= ID_MAXCODEC);i++) PcntMemUse[i] = 100;
         FSBltPerSec = 0;


         break;


      default:
         break;
   }

#ifndef DOS
   /* user overrides */
   GetOptionFlag("VIDEO-CALCULATE", pSWFlags, SW_CALCULATE);
   GetOptionFlag("VIDEO-DECIMATION", pSWFlags, SW_DECIMATION);
   GetOptionFlag("VIDEO-VFILTER", pHWCaps, HW_VFILTER);
   GetOptionFlag("VIDEO-YUVDECIMATION", pHWCaps, HW_YUVDECIMATION);
   GetOptionFlag("VIDEO-YUVDECIMATION", pSWFlags, SW_YUVDECIMATION);
#endif

   if (!(my->HWCaps & HW_VFILTER))
   {
      /* If vertical filter isn't supported, save mode 2 isn't either */
      my->HWCaps &= ~(HW_SAVEMODE2);

   }

#ifndef DOS
   my->PcntMemUse[ID_SWCODEC] = GetOptionInt("VIDEO-PCNTMEMUSE", PcntMemUse[ID_SWCODEC]);
   my->PcntMemUse[ID_HWCODEC] = GetOptionInt("VIDEO-PCNTMEMUSECP3", PcntMemUse[ID_HWCODEC]);
   my->PcntMemUse[ID_LVIDEO]  = GetOptionInt("VIDEO-PCNTMEMUSELV", PcntMemUse[ID_LVIDEO]);
   my->PcntBlankUse[ID_SWCODEC] = GetOptionInt("VIDEO-PCNTBLANKUSE", PcntBlankUse[ID_SWCODEC]);
   my->PcntBlankUse[ID_HWCODEC] = GetOptionInt("VIDEO-PCNTBLANKUSECP3", PcntBlankUse[ID_HWCODEC]);
   my->PcntBlankUse[ID_LVIDEO]  = GetOptionInt("VIDEO-PCNTBLANKUSELV", PcntBlankUse[ID_LVIDEO]);
   my->FSBltPerSec  = GetOptionInt("VIDEO-FSBLTPERSEC", FSBltPerSec);

   /*
   ** For validation only
   */
   GetOptionFlag("VIDEO-FIXWORDALIGN", pSWFlags, SW_FIXWORDALIGN);
   GetOptionFlag("VIDEO-FIX988DECIMATION", pSWFlags, SW_FIX988DECIMATION);
   my->M1 = (BYTE)GetOptionInt("VIDEO-CR54", (WORD)my->M1);
   my->M2 = (BYTE)GetOptionInt("VIDEO-CR60", (WORD)my->M2);
   my->M3 = (BYTE)GetOptionInt("VIDEO-CR72", (WORD)my->M3);
#endif

   /*
   ** Although % is 0..100 for users, convert to 0..64 internally
   ** to make calculations more efficient
   */
   for(i=0;(i <= ID_MAXCODEC);i++)
   {
      my->PcntMemUse[i]   = (WORD) RANGE(my->PcntMemUse[i],100,64);
      my->PcntBlankUse[i]  = (WORD) RANGE(my->PcntBlankUse[i],100,64);
   }

   /***************************
   /* Start bandwidth calculations
   */

   my->MHzMCLK = mclk;

   if ((mWidth == 0) && (!(my->HWCaps & HW_UMA)))
      my->BMemWidth = 4;         /* 32-bit memory access */
   else
      my->BMemWidth = 8;         /* 64-bit memory access */

   if ((mType == 0) || (mType == 1))
   {
      my->HWFlags |= HW_RASLOW25;
      my->MemCycles = 12;          /* avg cycles 1.2 */
   }
   else
   {
      my->MemCycles = 20;          /* cycles * 2.0 */
   }

   /* Here is the key calculation .... */
   DPRINTF(("my->MHzMCLK=%d",my->MHzMCLK));
   my->MBsTotal = my->MHzMCLK * my->BMemWidth * 10 / my->MemCycles;

   if ((my->BMemWidth >= 8) && (mType == 0))
   {
      /*
      ** 64bit memory width and 1-EDO
      ** Without this, get blue vertical speckles
      ** when running through LPB (or other problems)
      */
      my->HWFlags |= HW_1CYCLELPB;
   }

   /* mask features chosen against what the hardware supports */
   my->HWFlags &= my->HWCaps;

   /*
   ** if calculations are disabled, don't set any flags
   ** except those which are set through 81EC in the tables
   ** and overlay caps.
   */
   if (!(my->SWFlags & SW_CALCULATE))
   {
      my->HWCaps &= (HW_1CYCLELPB | HW_RASLOW25);
      my->HWFlags &= (HW_1CYCLELPB | HW_RASLOW25);
      my->SWFlags &= (SW_NOTSPT24BOVLY | SW_NOTSPT32BOVLY);
   }

   /*
   ** Initialize the latancy, random_read and page_miss
   */
   my->Latency[EDO1cyc]=3;
   my->Latency[SGRAM]=3;
   my->Latency[ED02cyc]=3;
   my->Latency[FPAGE]=3;
   my->Latency[SDRAM]=3;

   my->Random_Read[EDO1cyc]=6;
   my->Random_Read[SGRAM]=8;
   my->Random_Read[ED02cyc]=7;
   my->Random_Read[FPAGE]=7;
   my->Random_Read[SDRAM]=8;

   my->Page_Miss[EDO1cyc]=7;
   my->Page_Miss[SGRAM]=9;
   my->Page_Miss[ED02cyc]=8;
   my->Page_Miss[FPAGE]=8;
   my->Page_Miss[SDRAM]=9;


   return;
}
#endif WIN32

void Bandwidth_SetPrimary(PBAND my, SHORT dclk, SHORT xRes, SHORT yRes,
                          SHORT bpp, BYTE fInterlaced, SHORT dclksDuringBlank)
{
   WORD  MBsDraw;
   my->MHzDCLK = dclk;      /* store dot clock for this mode */
   my->MBsPrimary = 0;

   if (bpp < 8) bpp = 16;
   if (dclk == 0) dclk = 94;

   my->GBPP = bpp;      /*store the bpp for the mode*/
   my->GSizeX = xRes;   /*store the screen width*/
   my->GSizeY = yRes;   /*store the screen height*/


   if (my->id != DEV_VIRGEVX)
   {
      /*
      ** Single-port DRAM.  Must subtract for primary stream refresh.
      ** For dual-ported VRAM part, primary stream refresh is accessed seperate
      ** from secondary stream refresh.
      */

      my->MBsPrimary = (WORD) ( dclk * (bpp / 8) );

   }

   /*
   ** Subtract for other drawing operations ocurring at the same time
   ** user can adjust the quantity of drawing operations accounted for
   ** by adjusting VIDEO-FSBLTPERSEC, which is then scaled to the
   ** resolution to convert to Megabytes per second
   */
   MBsDraw = (WORD)((xRes >> 5) * (yRes >> 5) * bpp / 8);
   my->MBsPrimary += ((MBsDraw >> 5) * my->FSBltPerSec) >> 5;

   /*
   ** Subtract for system memory usage for uma
   ** The system usage is about half of the PCI bandwidth.
   ** The PCI memory width is 4Byte.
   */
   if (my->HWCaps & HW_UMA)
      my->MBsPrimary += my->MHzMCLK * my->UMAFactor / (my->MemCycles*10);


   /*
   ** Consider the device with DUOView capability
   ** and LCD is in use
   */
   if (my->DuoViewFlags & DVW_DSTN_ON)
        my->MBsPrimary +=2*my->MHzDCLK*my->FRCBPP/8; /*Consider Frame rate control*/

   if  ((my->DuoViewFlags & DVW_DualImage) ||
        (my->DuoViewFlags & DVW_DualViewSameImage) ) 
        my->MBsPrimary += my->MHzDCLK2 * (my->GBPP / 8) ; /*Consider the second iga*/


   /* Some features may not be supported in interlaced modes*/
   if (fInterlaced)
      my->SWFlags |= SW_INTERLACED;
   else
      my->SWFlags &= ~(SW_INTERLACED);

   return;
}


/*
** Bandwidth_SetSecondary
**
** Src is the UNDECIMATED source image size in pixels
** Trg is the target window size (fixed by user/OS) in pixels
** Max is the maximum source image size which fits in off-screen VRAM in pixels
**
** Notes:
** (1) There are Direct Draw specific #ifdefs in the module,
** because in Win95 these calculations are currently called twice:
** once in RFMK2V, where the Hstretch and LPB flags are used,
** and once in Direct Draw, where the FIFO and streams flags are used
** The only problem is, RFM is given the pre-decimated stream source,
** while Direct Draw only has the post-decimated stream source. So
** to make the calculations decide to do the same things at the same
** times, Direct Draw do a few things different (like not consider FIFO fit).
** In the future, we should thunk between DD and RFMK2V rather than
** have the code in both places.
*/
void Bandwidth_SetSecondary(PBAND my, WORD device, SHORT bpp,
   SHORT xSrc, SHORT ySrc, SHORT xTrg, SHORT yTrg, SHORT xMax, SHORT yMax)
{
   WORD MBsUsed;                      /* MBs Consumed by secondary stream    */
   SHORT MBsFree;                     /* MBs memory bandwidth available      */
   WORD OrgStride;                    /* original width in bytes             */
   WORD NewStride;                    /* new width in bytes                  */
   WORD TrgStride;                    /* destination width in bytes          */
   WORD HStretchActual;
   WORD HStretchFinal;
   WORD HStretchVFilter;
   WORD HStretchBandwidth;
   WORD HStretch;
   WORD Transfer_Time;
   WORD Fifo_Read;
   WORD TH_Pri, TH_Sec;
   WORD MBsPriLevel;                     /* MBs level used by primary          */
#ifndef WIN32
   WORD MBsSecLevel;                     /* MBs level used by secondary        */
#endif
   WORD wDeciLevel = 0;                  /* Decimation level for secondary        */
   WORD DCLK;


   DPRINTF(("xSrc=%d ySrc=%d xTrg=%d yTrg=%d xMax=%d yMax=%d",
             xSrc, ySrc, xTrg, yTrg, xMax, yMax));

   if (bpp == 0) bpp = 16;
   if (device == 0) device = ID_SWCODEC;
   if (xSrc == 0) xSrc = 320;
   if (ySrc == 0) ySrc = 240;
   if (xTrg == 0) xTrg = xSrc;
   if (yTrg == 0) yTrg = ySrc;
   if (xMax == 0) xMax = xSrc;
   if (yMax == 0) yMax = ySrc;

   OrgStride = (WORD) ( (xSrc * bpp) / 8 );
   TrgStride = (WORD) ( (xTrg * bpp) / 8 );

   /* reset flags that this function is responsible for */
   my->HWFlags &= ~(HW_VFILTER | HW_VFILTERYONLY2 | HW_DECIMATE411 |
                    HW_SAVEMODE1 | HW_SAVEMODE2);

   /*
   ** Clip for downscaling and VRAM
   */
   if (xSrc > xTrg) xSrc = xTrg;
   if (xSrc > xMax) xSrc = xMax;


   DPRINTF(("my->GRefreshRate = %d", my->GRefreshRate));

   /* Adjust and subtract for MBs consumed by Primary Stream and drawing */

   DPRINTF(("my->MHzMCLK=%d",my->MHzMCLK));
   my->MBsTotal = my->MHzMCLK * my->BMemWidth * 10 / my->MemCycles;

   if ((my->DuoViewFlags & DVW_DualImage) ||
       (my->DuoViewFlags & DVW_DualViewSameImage)) 
   {
       if (my->DuoViewFlags & DVW_DSTN_ON)
       {
       //The penalty for each switch is 10%
       //with DSTN on memory access has to switch among
       //DSTN, primary 1, primary2, secondary
       //with DSTN on memory access has to switch among
       //DSTN, primary 1, primary2, secondary
           my->MBsTotal = my->MBsTotal *7/10;  // (70% effective)
       } else
       {
       // It seems that in GX2 dualview mode, the memory efficiency is
       // not good as in LCD case.
           if ( (my->id == DEV_GX2) ||
                (my->id == DEV_GX2P) ||
                (my->id == DEV_GX2PM) )
               my->MBsTotal = my->MBsTotal *6/10;  // (60% effective)
           else
               my->MBsTotal = my->MBsTotal *8/10;  // (80% effective)
       }

       MBsFree = my->MBsTotal  - my->MBsPrimary;

   } else
   {
       if (my->DuoViewFlags & DVW_DSTN_ON)
       {
       //The penalty for each switch is 10%
       //with DSTN on memory access has to switch among
       //DSTN, primary 1, primary2, secondary
       //with DSTN on memory access has to switch among
       //DSTN, primary 1, primary2, secondary
           my->MBsTotal = my->MBsTotal *8/10;  // (80% effective)
           MBsFree = my->MBsTotal  - my->MBsPrimary;
       } else
           MBsFree = (((my->MBsTotal >> 3) * my->PcntMemUse[device]) >> 3) - my->MBsPrimary;
   }


   DPRINTF(("MBsTotal = %d", my->MBsTotal));
   DPRINTF(("MBsPrimary = %d", my->MBsPrimary));
   DPRINTF(("MBsFree = %d", MBsFree));


   /* This is a key calculation: MBs used by the overlay stream */
   if (((my->DuoViewFlags & DVW_DualImage) ||
        (my->DuoViewFlags & DVW_DualViewSameImage))  &&
       (!(my->DuoViewFlags & DVW_SP_IGA1))) 
        MBsUsed = (SHORT)((my->MHzDCLK2 * bpp) /8);
    else
        MBsUsed = (SHORT)((my->MHzDCLK * bpp) /8);

   /*
   ** substract for LPB memory writes to update video in offscreen VRAM
   ** assumes 32 frames per second, since we don't have the real number
   */
   if (device != ID_SWCODEC)
      MBsUsed += ((xMax >> 5) * (yMax >> 5) * (bpp/8)) >> 5;

   DPRINTF(("MBsUsed = %d", MBsUsed));

   /*
   ** All HStretch calculations:   1:1 is 64  2:1 is 128 ...
   */
   HStretchVFilter = MAX_HSTRETCH;     // assume VFILTER unafordable

   if (xTrg > xSrc)
   {
      HStretchActual = (WORD)((DWORD)xTrg * 64L / (DWORD)xSrc);
   }
   else
   {
      HStretchActual = MIN_HSTRETCH;
   }

   if ((my->HWCaps & HW_VFILTER) && !(my->SWFlags & SW_INTERLACED))
   {
      WORD MBsUsedVFilter = MBsUsed;
      SHORT MBsFreeVFilter = MBsFree;
      WORD HStretchBandwidth = 0;
      WORD HStretchFifo = 0;

      if (!(my->HWCaps & HW_DUALFIFO))
      {
         /* ViRGE/VX seems to have a fixed overhead for VFILTER */
         MBsUsedVFilter += 15;

         /*
         ** Without a dual-FIFO architecture (like TrioV2),
         ** V. Filter doubles required bandwidth
         */
         MBsUsedVFilter *= 2;

         /* especially expensive when going through LPB */
         if (device != ID_SWCODEC)
            MBsFreeVFilter -= MBsUsedVFilter;

      }

      DPRINTF(("MBsUsedVFilter = %d", MBsUsedVFilter));

      if ((SHORT)MBsUsedVFilter > MBsFreeVFilter)
      {
         if (MBsFreeVFilter <= 0)
            HStretchBandwidth = MAX_HSTRETCH;
         else
         {
            HStretchBandwidth = (WORD)((DWORD)MBsUsedVFilter * 64L
                                 / (DWORD)MBsFreeVFilter) + 1;
         }
      }
      else
      {
         HStretchBandwidth = MIN_HSTRETCH;
      }

      /*
      ** decimate enough to satisfy the memory bandwidth
      */
      HStretchVFilter = HStretchBandwidth;

      DPRINTF(("HStretchVFilter=%d",HStretchVFilter));

   }


   if ((SHORT)MBsUsed > MBsFree)
   {
      if (MBsFree <= 0)
      {
         HStretchBandwidth = MAX_HSTRETCH;
      }
      else
      {
         HStretchBandwidth = (WORD)((DWORD)MBsUsed * 64L / (DWORD)MBsFree) + 1;
      }
   }
   else
   {
      HStretchBandwidth = MIN_HSTRETCH;
   }

   /*
   ** decimate enough to satisfy the memory bandwidth
   */
   HStretch = HStretchBandwidth;

   if ( my->HWCaps & HW_VFILTER                 &&

        ( ( (HStretchVFilter < MAX_HSTRETCH)    &&
            (my->IsSecFormatYUV==1)             &&
            (my->HWCaps&HW_VFILTERYONLY2)       &&
            (OrgStride <= 2*my->BFifo) )        ||

          ( (HStretchVFilter < MAX_HSTRETCH)    &&
            (OrgStride <= my->BFifo) ) )        &&

        (my->MHzDCLK < my->SPDCLK_Cutoff ) )
   {
      /*
      ** The Decimation ratio is low enough that we
      ** want to turn on vertical interpolation in this case.
      */
      if (!( (my->SPStyle & SP_M5) && (OrgStride%16) ) )
      {
          my->HWFlags |= HW_VFILTER;
          HStretchFinal = HStretchVFilter;
      } else
      {
          HStretchFinal = HStretch;
          DPRINTF(("Vertical Filtering not affordable"));
      }
   }
   else
   {
      HStretchFinal = HStretch;
      DPRINTF(("Vertical Filtering not affordable"));
   }
        // Calculate the primary bandwidth level within total bandwidth
   MBsPriLevel = (WORD)((my->MBsPrimary * 100L) / my->MBsTotal);
   DPRINTF(("MBsPriLevel = %d", MBsPriLevel));

#ifndef WIN32
   if (HStretchFinal > MIN_HSTRETCH)
      NewStride = (WORD)((DWORD)TrgStride * 64L / (DWORD)HStretchFinal);
   else
                { //Decimation adjustment in 4MB high resolution modes for live video
       if((device == ID_LVIDEO) && (my->BMemWidth <= 4) && (my->GSizeX == 1024))
         {
          switch (my->GRefreshRate)
            {
             case 75:
             case 85:
               HStretchFinal = (WORD) (HStretchFinal + 40);
               break;

             case 70:
               HStretchFinal = (WORD) (HStretchFinal + 32);
               break;

             default:
               HStretchFinal = (WORD) (HStretchFinal + 16);
               break;
             }
         }

       if((device == ID_LVIDEO) && (my->GBPP >= 24L) && (MBsPriLevel > 40L))
         {
          MBsSecLevel = (WORD) (xTrg >> 7) ;
          if (MBsPriLevel > 45L) {
              switch (xTrg >> 8)
                {
                   case 0:
                   case 2:
                   case 3:
                     wDeciLevel = (WORD) ((MBsSecLevel * 7) >> 2);
                                                        break;
                   case 1:
                     wDeciLevel = (WORD) ((MBsSecLevel * 8) >> 2);
                                                        break;
                   default:
                     wDeciLevel = (WORD) ((MBsSecLevel * 7) >> 2);
                     break;
                }
            }
         else
            wDeciLevel += 3;
         HStretchFinal = (WORD)(HStretchFinal + (8L * wDeciLevel));
         NewStride = (WORD)((DWORD)TrgStride * 64L / (DWORD)HStretchFinal);
        }
      NewStride = TrgStride;
     }
   NewStride = min(OrgStride,NewStride);
#else
   /*
   ** For direct draw, if it's HW MPEG or Live Video xSrc has already
   ** been decimated
   */
   if (device == ID_SWCODEC)
   {
      if (HStretchFinal > MIN_HSTRETCH)
         NewStride = (WORD)((DWORD)TrgStride * 64L / (DWORD)HStretchFinal);
      else
         NewStride = TrgStride;
      NewStride = min(OrgStride,NewStride);
   }
   else
   {
      /*
      ** Decimation has already been done by RFMK2V.DLL
      */
      NewStride = OrgStride;
   }
#endif

   /* special case if decimation is turned off */
   if (!(my->SWFlags & SW_DECIMATION)) HStretchFinal = 64;

   // Special decimation case for YOSE
   if((device == ID_LVIDEO) && (my->id == DEV_YOSE)) {
      HStretchFinal = (WORD) (((my->GSizeX >> 8) + (my->GBPP >> 3)) * 64);
      HStretchFinal += (WORD) (((xTrg >> 8) + (my->MHzDCLK >> 5)) * 8);
   }

   /* convert HStretch to decimal % */
   HStretchFinal = (WORD)RANGE(HStretchFinal, 64, 100);
   HStretchVFilter = (WORD)RANGE(HStretchVFilter, 64, 100);

   my->HStretch = (WORD) ( HStretchFinal * 10 );
   my->HStretchVFilter = (WORD) ( HStretchVFilter * 10 );
   DPRINTF(("Calculated HStretch: %d", my->HStretch));
   DPRINTF(("Calculated HStretchVFilter: %d", my->HStretchVFilter));

   if (my->HWFlags & HW_VFILTER)
      my->HWFlags |= HW_SAVEMODE2;
   else if (NewStride < my->BFifo)
      my->HWFlags |= HW_SAVEMODE1;

   if ( (my->HWFlags & HW_VFILTER) && (my->IsSecFormatYUV==1) )
   {
      my->HWFlags |= HW_VFILTERYONLY2;
   }

   /*
   ** Mask features chosen against what the hardware supports.
   ** This is an important step: Above, we may set flags which are
   ** unsupported for certain chips. But this command will make
   ** sure to only turn on features which are available
   */
   my->HWFlags &= my->HWCaps;

   if (my->HWFlags & HW_VFILTER) DPRINTF(("HW_VFILTER Enabled!"));

   /*
   ** Chip-specific fixups
   */
   switch(my->id)
   {
      case DEV_VIRGEVX:
         if (device == ID_HWCODEC)
            if (my->SWFlags & SW_FIX988DECIMATION) my->SWFlags |= SW_YUV8LEVEL;
         if (my->HWFlags & HW_VFILTER)
            my->FifoCtrl = FIFOCTRL(12, 11, 11);
         else
            if (device == ID_SWCODEC)
               my->FifoCtrl = FIFOCTRL(24, 23, 0);
            else
               my->FifoCtrl = FIFOCTRL(24, 14, 0);

         if ( ( my->GSizeX == 1280 && my->GBPP >= 16 && my->MHzDCLK > 133 )  ||
              my->GSizeX > 1280 )
         {
             my->HStretch = 9000;
             my->HStretchVFilter = 9000;
         }


         break;
      case DEV_VIRGE:
         my->FifoCtrl = FIFOCTRL(16, 10, 7);
         if (my->HWFlags & HW_RASLOW25)
         {
            /* Set bit 15: RAS LOW = 2.5 MCLKs */
            my->FifoCtrl |= 0x8000;
         }
         if (my->HWFlags & HW_1CYCLELPB)
         {
            my->FifoCtrl |= 0x40000;
         }
         break;
      case DEV_TRIOV2:
      case DEV_VIRGEGX:

         if ( (my->GSizeX==1024)&&(my->GBPP>16) )
         {
             my->HWFlags &= ~HW_VFILTER; //Disable vertical filtering
             my->HWFlags &= ~HW_SAVEMODE2;
             my->HWFlags &= ~HW_VFILTERYONLY2;
             if (NewStride < my->BFifo)
                 my->HWFlags |= HW_SAVEMODE1;
         }

         /*
         ** Thresholds must be in multiples of 4
         */
         if (my->HWFlags & HW_VFILTER)
         {
            /*
             * Check the memory type to set different FIFO for SGRAM
             * chip to avoid noise
             */

            if ( my->MemType == 1 )
            {
                // SGRAM
                 my->FifoCtrl = FIFOCTRL(KV2_SSS_32, 10, 10);
             }
            else
            {
                 my->FifoCtrl = FIFOCTRL(KV2_SSS_32, 10, 8);
            }

         }
         else
         {
               my->FifoCtrl = FIFOCTRLV2(KV2_SSS_90, 80, 48);
         }

         my->M3 = 0x50;

         break;
      case DEV_TRIOV:
         my->FifoCtrl = FIFOCTRL(12, 9, 7);
         if (my->HWFlags & HW_RASLOW25)
         {
            /* Set bit 15: RAS LOW = 2.5 MCLKs */
            my->FifoCtrl |= 0x8000;
         }
         if (my->HWFlags & HW_1CYCLELPB)
         {
            my->FifoCtrl |= 0x40000;
         }
         break;

      case DEV_YOSE:
         if ( my->MemType == SDRAM )
         {
            #define BLT_LATENCY 3
            #define RANDOM_READ 8
            #define PAGE_MISS   9
            #define SYSTEM_LATENCY 10

            my->M1 = 0x2c;
            my->M2 = 0x17;

            /* For secondary fifo
             */
            Transfer_Time = BLT_LATENCY + RANDOM_READ + 3*PAGE_MISS;  //in mclk cycles
            Transfer_Time += SYSTEM_LATENCY;                  //in mclk cycles
            Transfer_Time = my->MHzDCLK*Transfer_Time/my->MHzMCLK;   //in dclk
            Transfer_Time +=16;  //latancy to read fifo 8DCLK        //in dclk
            Fifo_Read = (Transfer_Time*bpp+7)/8 ;               //read in byte
            TH_Sec = (Fifo_Read+7)/8 + 4;                            //in slot
            //Consider special case when read rate is very close to write rate
            //otherwise, constant page miss
            TH_Sec += ((my->MHzDCLK*(bpp/8))*16)/(my->MHzMCLK*my->BMemWidth);  //in slot
            if (TH_Sec > 23) TH_Sec=23;

            /* For primary fifo
             */
            Transfer_Time = BLT_LATENCY + RANDOM_READ + 3*PAGE_MISS; //in mclk cycle
            Transfer_Time += SYSTEM_LATENCY;                  //in mclk cycles
            Transfer_Time = my->MHzDCLK*Transfer_Time/my->MHzMCLK;   //in dclk
            Transfer_Time +=16;  //latancy to read fifo 8DCLK        //in dclk
            Fifo_Read = (Transfer_Time*my->GBPP+7)/8;           //read in byte
            TH_Pri = (Fifo_Read+7)/8 + 4;                            //in slot
            //Consider special case when read rate is very close to write rate
            TH_Pri += ((my->MHzDCLK*(my->GBPP/8))*16)/(my->MHzMCLK*my->BMemWidth);  //in slot
            if (TH_Pri > 23) TH_Pri=23;

         }
         else                    // 2-Cycle EDO
         {

            #define BLT_LATENCY_D    3
            #define RANDOM_READ_2CYC 7
            #define PAGE_MISS_2CYC   8
            #define SYSTEM_LATENCY_2CYC   10

            my->M1 = 0x2c;
            my->M2 = 0x2f;

            /* For secondary fifo
             */
            Transfer_Time = BLT_LATENCY_D+RANDOM_READ_2CYC+3*PAGE_MISS_2CYC; //in mclk cycles
            Transfer_Time += SYSTEM_LATENCY_2CYC;             //in mclk cycles
            Transfer_Time = my->MHzDCLK*Transfer_Time/my->MHzMCLK;   //in dclk
            Transfer_Time +=16;  //latancy to read fifo 8DCLK        //in dclk
            Fifo_Read = (Transfer_Time*bpp+7)/8 ;               //read in byte
            TH_Sec = (Fifo_Read+7)/8;                                //in slot
            //Consider special case when read rate is very close to write rate.
            //Added a factor dependent on DCLK for high read/write rates.
            TH_Sec += ((my->MHzDCLK*(bpp/8))*16)/(my->MHzMCLK*my->BMemWidth);  //in slot
            if (TH_Sec > 23) TH_Sec=23;

            /* For primary fifo
             */
            Transfer_Time = BLT_LATENCY_D+RANDOM_READ_2CYC+3*PAGE_MISS_2CYC; //in mclk cycles
            Transfer_Time += SYSTEM_LATENCY_2CYC;             //in mclk cycles
            Transfer_Time = my->MHzDCLK*Transfer_Time/my->MHzMCLK;   //in dclk
            Transfer_Time +=16;  //latancy to read fifo 8DCLK        //in dclk
            Fifo_Read = (Transfer_Time*my->GBPP+7)/8;           //read in byte
            TH_Pri = (Fifo_Read+7)/8;                               //in slots
            //Consider special case when read rate is very close to
            //write rate.
            TH_Pri += ((my->MHzDCLK*(my->GBPP/8))*16)/(my->MHzMCLK*my->BMemWidth);  //in slot
            if (TH_Pri > 23) TH_Pri=23;
         }
         TH_Sec=23;
         TH_Pri=12;
         HStretchFinal=64; //HStretch = 1000
         if ( ((my->GBPP==16) && (my->GSizeX==1024)) || (my->MHzDCLK > 80) )
         {
            HStretchFinal=112; //HStretch = 1750
         }

         HStretchFinal = (WORD)RANGE(HStretchFinal, 64, 100);
         my->HStretch=(WORD)(HStretchFinal * 10);
         DPRINTF(("Adjusted (MAB) HStretch: %d", my->HStretch));

         my->FifoCtrl = FIFOCTRLYOSE(YOSE_SSS_24, TH_Sec, TH_Pri, 23);
         DPRINTF(("TH_Sec=%d, TH_Pri=%d", TH_Sec, TH_Pri));
         break;

      case DEV_M3:
      case DEV_M5:
      case DEV_M5P:
      case DEV_M5PM:
      case DEV_GX2:
      case DEV_GX2P:
      case DEV_GX2PM:

        /* For primary stream 1 fifo
         */
        DCLK = my->MHzDCLK;
        /* Transfering time for the interrupt
        */
        Transfer_Time = my->Latency[my->MemType] +
                        my->Random_Read[my->MemType] +
                        3*my->Page_Miss[my->MemType];     //in mclk cycles
        Transfer_Time = DCLK*Transfer_Time/my->MHzMCLK;   //in dclk
        Transfer_Time +=32;  //latancy to read fifo 8DCLK //in dclk

        Fifo_Read = (Transfer_Time*my->GBPP+7)/8;         //read in byte
        my->Fifo.PS1Threshold = (BYTE)((Fifo_Read+15)/16 + 2);  //in slot
        if (my->Fifo.PS1Threshold > 23) my->Fifo.PS1Threshold=23;

        /* For primary stream 2 fifo
         */
        DCLK = my->MHzDCLK2;
        /* Transfering time for the interrupt
        */
        Transfer_Time = my->Latency[my->MemType] +
                        my->Random_Read[my->MemType] +
                        3*my->Page_Miss[my->MemType];     //in mclk cycles
        Transfer_Time = DCLK*Transfer_Time/my->MHzMCLK;   //in dclk
        Transfer_Time +=32;  //latancy to read fifo 8DCLK //in dclk

        Fifo_Read = (Transfer_Time*my->GBPP+7)/8;         //read in byte
        my->Fifo.PS2Threshold = (BYTE)((Fifo_Read+15)/16 + 2);  //in slot
        if (my->Fifo.PS2Threshold > 23) my->Fifo.PS2Threshold=23;

        my->PS1TimeOut = (BYTE)(12*128/my->BMemWidth);
        my->PS2TimeOut = (BYTE)(12*128/my->BMemWidth);


        // Overwite the calculations
        my->Fifo.PS1Threshold = 0x14;
        my->Fifo.PS2Threshold = 0x13;
        my->PS1TimeOut = 0xC0;
        my->PS2TimeOut = 0x20;


        if (my->DuoViewFlags & DVW_SP_IGA1)
           DCLK = my->MHzDCLK;
        else DCLK = my->MHzDCLK2;

        /* Transfering time for the interrupt
         */
        Transfer_Time = my->Latency[my->MemType] +
                        my->Random_Read[my->MemType] +
                        3*my->Page_Miss[my->MemType];     //in mclk cycles
        Transfer_Time = DCLK*Transfer_Time/my->MHzMCLK;   //in dclk
        Transfer_Time +=32;  //latancy to read fifo 8DCLK //in dclk

        /* For secondary fifo
        */
        Fifo_Read = (Transfer_Time*bpp+7)/8 ;              //read in byte
        my->Fifo.SSThreshold = (BYTE)((Fifo_Read+15)/16 + 2);  //in slot
        if (my->Fifo.SSThreshold > 23) my->Fifo.SSThreshold=23;

        my->SSTimeOut  = (BYTE)(12*128/my->BMemWidth);


        // Overwite the calculations
        my->Fifo.SSThreshold = 0x12;
        my->SSTimeOut = 0xC0;


        if ( (my->DuoViewFlags & DVW_DSTN_ON) )
        {
            /* For STN read fifo
             */
            Transfer_Time = my->Latency[my->MemType] +
                            my->Random_Read[my->MemType] +
                            3*my->Page_Miss[my->MemType];     //in mclk cycles
            Transfer_Time = my->MHzDCLK*Transfer_Time/my->MHzMCLK; //in dclk
            Transfer_Time +=16;  //latancy to read fifo 8DCLK //in dclk

            Fifo_Read = (Transfer_Time*my->FRCBPP+7)/8;       //read in byte
            my->STNRdThreshold = (BYTE)((Fifo_Read+7)/8 + 2);   //in slot
            if (my->STNRdThreshold > 15) my->STNRdThreshold=15;

           /* For STN write fifo
            */
           Fifo_Read = (Transfer_Time*my->FRCBPP+7)/8;       //read in byte
           my->STNWrThreshold = (BYTE)((Fifo_Read+7)/8 + 2);   //in slot
           if (my->STNWrThreshold > 15) my->STNWrThreshold=15;

           my->STNRdTimeOut = (BYTE)(16*64/my->BMemWidth);
           my->STNWrTimeOut = (BYTE)(16*64/my->BMemWidth);

        }  else
        {
            my->STNRdThreshold = 0;
            my->STNWrThreshold = 0;
            my->STNRdTimeOut = 0;
            my->STNWrTimeOut = 0;

        }

        if ( (HStretchVFilter== MAX_HSTRETCH) || (HStretch==MAX_HSTRETCH) )
        {
            /*Bandwidth is not enough */
            my->HStretch = 9000;
            my->HStretchVFilter = 9000;
            my->Fifo.SSThreshold = 0;
            my->SSTimeOut  = 0;
        }


        break;


      default:
         break;
   }

   return;
}

/*
 * Given the screen width, find out the maximum DCLK in MCLK  for the mode.
 * i.e. the DCLK for the 85Hz refresh rate
 */
WORD GetMaxDCLK(WORD wScreenWidth)
{
WORD DCLK;
    switch (wScreenWidth)
    {
        case 640:
            DCLK = 36;
            break;
        case 800:
            DCLK = 56;
            break;
        case 1024:
            DCLK = 95;
            break;
        case 1152:
            DCLK = 95;   //We only support 60Hz 
            break;
        case 1280:
            DCLK = 135;
            break;
        case 1600:
            DCLK = 162;
            break;
        default:
            DCLK = 135;
            break;
    }    
    return (DCLK);
}/*GetMaxDCLK*/

