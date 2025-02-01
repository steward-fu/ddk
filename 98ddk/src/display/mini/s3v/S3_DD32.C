/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:   s3dd32.c
 *  Content:    DirectDraw S3 driver 32-bit fns
 *  History:
 *   Date   By  Reason
 *   ====   ==  ======
 *   xx-jun-95  philh   development
 *   30-jul-95  craige  some cleanup
 *
 ***************************************************************************/

// I N C L U D E S ----------------------------------------------------------

#include "ddrawi.h"
#include "d3dhal.h"
#include "s3i.h"
#include "s3_ddtk.h"
#include "s3data.h"

#ifdef DX5
#include "ddkernel.h"
#include "s3color.h"

#ifdef S3VIRGE
/*-----------------01-12-96 11:39pm-----------------
 Support for Direct3D
--------------------------------------------------*/
extern DWORD __stdcall SetRenderTarget32(LPD3DHAL_SETRENDERTARGETDATA);  //lycheck
extern DWORD __stdcall DrawOnePrimitive32(LPD3DHAL_DRAWONEPRIMITIVEDATA);//lycheck
extern DWORD __stdcall DrawOneIndexedPrimitive32(LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA);//lycheck
extern DWORD __stdcall DrawPrimitives32(LPD3DHAL_DRAWPRIMITIVESDATA); //lycheck
#endif // S3VIRGE
#endif // DX5

// G L O B A L S ------------------------------------------------------------

LPS3THKINFO sDataGlobal;
HANDLE hSharedHeap;
HANDLE  g_hD3DHAL_DriverHeap;

// --------------------------------------------------------------------------
void getDisplayDuration(LPS3THKINFO sData);
void ClipSSWindow(LONG dwStride, BYTE bytesPerPixel, DWORD dwSrcXY,LPS3THKINFO sData);
void EnableStreamProcessor_M5( LPS3THKINFO sData );

// --------------------------------------------------------------------------

#ifdef DEBUG
LPS3THKINFO sDataCurrent;   // used by DEBUG code to verify no VGA IO access
#endif

LPS3THKINFO GetMyRefData(LPDDRAWI_DIRECTDRAW_GBL lpDD)
{
#ifdef DEBUG
    if (lpDD->dwReserved3)          // running on DX5 or greater?
        sDataCurrent = (LPS3THKINFO)lpDD->dwReserved3;
    else
        sDataCurrent = sDataGlobal; // our global value for this
#endif
    if (lpDD->dwReserved3)          // running on DX5 or greater?
        return (LPS3THKINFO)lpDD->dwReserved3;
    else
        return sDataGlobal;         // our global value for this
}
// --------------------------------------------------------------------------
SPOVERLAYDESC *AllocSPOD(LPS3THKINFO sData)
{
    SPOVERLAYDESC *p;

    for (p=sData->lpSpodList; p && p->lpDDSrcSurface; p=p->next)
        ;

    if (p)
        return p;

    p = HeapAlloc(hSharedHeap, HEAP_ZERO_MEMORY, sizeof(SPOVERLAYDESC));

    p->next = sData->lpSpodList;

    sData->lpSpodList = p;
    return p;
}

void FreeSPOD(SPOVERLAYDESC *p)
{
    p->lpDDSrcSurface = 0;

    //
    // we dont ever free the memory, just reuse the last one.
    //
    //HeapFree(hSharedHeap, 0, (LPVOID)lpd->lpDDSurface->lpGbl->dwReserved1);
}

#ifdef DX5
extern DWORD LPB_DriverInit(LPS3THKINFO sData);
extern const DDHAL_DDVIDEOPORTCALLBACKS LPBVideoPortCallbacks;

DWORD __stdcall GetDriverInfo32(LPDDHAL_GETDRIVERINFODATA lpInput);
DWORD __stdcall SyncSurfaceData32(LPDDHAL_SYNCSURFACEDATA lpInput);
DWORD __stdcall SyncVideoPortData32(LPDDHAL_SYNCVIDEOPORTDATA lpInput);
DWORD __stdcall ColorControl32(LPDDHAL_COLORCONTROLDATA lpInput);

/*
** Callback tables exported through the GetDriverInfo32 interface
*/
const DDHAL_DDKERNELCALLBACKS KernelCallbacks =
{
    sizeof(DDHAL_DDKERNELCALLBACKS),
    DDHAL_KERNEL_SYNCSURFACEDATA |
    DDHAL_KERNEL_SYNCVIDEOPORTDATA,
    &SyncSurfaceData32,
    &SyncVideoPortData32
};

const DDHAL_DDCOLORCONTROLCALLBACKS ColorControlCallbacks =
{
    sizeof(DDHAL_DDCOLORCONTROLCALLBACKS),
    DDHAL_COLOR_COLORCONTROL,
    &ColorControl32
};

const DDKERNELCAPS KernelCaps =
{
    sizeof(DDKERNELCAPS),
    DDKERNELCAPS_SKIPFIELDS     |
    DDKERNELCAPS_AUTOFLIP       |
    DDKERNELCAPS_SETSTATE       |
    DDKERNELCAPS_LOCK           |
    DDKERNELCAPS_FLIPVIDEOPORT  |
    DDKERNELCAPS_FLIPOVERLAY    |
    DDKERNELCAPS_FIELDPOLARITY  |
    0,
    DDIRQ_DISPLAY_VSYNC         |
    DDIRQ_VPORT0_VSYNC          |
    0
};
#endif



// D E F I N E S ------------------------------------------------------------

#define HEAP_SHARED      0x04000000
#define LA_GE  2            // chip in Bimodal mode
#define ENGINE 1            // chip in Engine mode
#define MEMORY 0            // chip in Memory mode
#define lpMemRegB       ((volatile BYTE  *)sData->lpMMReg)
#define lpMemReg        ((volatile WORD  *)sData->lpMMReg)

#define lpdwMemReg      ((volatile DWORD *)sData->lpMMReg)
#define S3SP_TOP P_CONTROL

#define CAPS_VERTICALINTERPOLATION  1

#define PIXEL_FORMATTER_PRESENT   ( sData->dwSpecialHW & PIXEL_FORMATTER )
#define STREAMS_PROCESSOR_PRESENT ( sData->dwSpecialHW & STREAMS_PROCESSOR )
#define BIMODAL_HW_PRESENT        ( sData->dwSpecialHW & BIMODAL_HW )
#define MMIOONLY_HW_PRESENT       ( sData->dwSpecialHW & MMIOONLY_HW )
#define COLOR_ADJUST_HW_PRESENT   ( (sData->bFamilyFlags & TRIOV2_FAMILY) | (sData->bFamilyFlags & VIRGEDXGX_FAMILY) )
#define VIRGE_FAMILY_PRESENT      ( sData->bFamilyFlags & VIRGE_FAMILY )
#define TRIO64V_FAMILY_PRESENT    ( sData->bFamilyFlags & TRIO64V_FAMILY )
#define IF_STREAMS_PROCESSOR_ON \
    if (sData->lpS3MobileData->wSPStyle & SP_M5 )\
    {\
        lpMemReg[0x83c4/2]=(WORD)0x4026; \
    }\
    lpMemRegB[0x83d4]=0x67; \
    if( (lpMemRegB[0x83d5] & 0x4) == 0x4)

BYTE Streams_Processor_On (LPS3THKINFO sData)
{
    if (sData->lpS3MobileData->wSPStyle & SP_M5 )
    {
        lpMemReg[0x83c4/2]=(WORD)0x4026;
    }
    if ( sData->lpS3DriverData->VGAFlag )
    {
        outp(0x3d4,0x67);
        return (inp(0x3d5) & 0x4);
    }
    else
    {
        lpMemRegB[0x83d4]=0x67;
        return (lpMemRegB[0x83d5] & 0x4);
    }
}


#define test_fifo( x )      ((getRegister( GP_STAT )) & FIFO_EMPTY_##x)
#define wait_for_pf_idle()  { while(mmGetRegister( PF_DSTADDR ) & PF_BUSY); }
#define IN_VBLANK ((sData->lpS3DriverData->VGAFlag) ? \
                  (inp(0x3da) & 0x08) : (lpMemRegB[0x8000+INPUT_STATUS_1_REG] & VBLANK_MASK))
#define IN_DISPLAY      (!IN_VBLANK)
#define IS_DISPLAY_ENABLE   (!IN_VBLANK)
#define WAIT_FOR_VBLANK { while(!IN_VBLANK); }
#define WAIT_FOR_VBLANK_M5 { lpMemRegB[0x83d4]=0x33; \
    for( ; !(lpMemRegB[0x83d5] & 0x04); lpMemRegB[0x83d4]=0x33 ) {;} }
#define WAIT_FOR_DISP_M5 { lpMemRegB[0x83d4]=0x33; \
    for( ; (lpMemRegB[0x83d5] & 0x01); lpMemRegB[0x83d4]=0x33 ) {;} }
#define CURRENT_VLINE GetCURRENT_VLINE( sData )
#define TV_UNDERSCAN_POSSIBLE   FALSE
//#define TV_UNDERSCAN_POSSIBLE   ( (sData->lpS3MobileData->wSPStyle & SP_M5) && \
//                                  ((sData->lpS3DriverData->wLogResolution == MODE_RES_800x600) || \
//                                   (sData->lpS3DriverData->wLogResolution == MODE_RES_640x480)) )



DWORD GetCURRENT_VLINE( LPS3THKINFO sData )
{
BOOL NotDone=TRUE;
volatile BYTE bTmp1, bTmp2;
    if (sData->lpS3MobileData->wSPStyle & SP_M5 )
    {
        lpMemReg[0x83c4/2]=(WORD)0x4026;
    }
    lpMemRegB[0x83d4]=0x3E;
    while (NotDone)
    {
    bTmp1 = lpMemRegB[0x83d5];
    bTmp2 = lpMemRegB[0x83d5];
    if (bTmp1 == bTmp2)
        NotDone = FALSE;
    }
    return  (((DWORD)bTmp1)*8);
}

// HW timing bug?
//
// Consecutive Streams Processor register writes seem to have problems
// in fast CPUs such as in P120 & P133 or even in P100 sometimes.
// The kludge is to waste some CPU cycles.

#if 1
#define mmdwSetRegister( x, y ) \
{   (lpdwMemReg[x/4])=(DWORD)(y); \
    sData->S3SP[(x-S3SP_TOP)/4]=(DWORD)(y); \
    sData->S3SP_SCRATCH[(x-S3SP_TOP)/4]=(DWORD)(y); \
    sData->S3SP_SCRATCH[(x-S3SP_TOP)/4]=(DWORD)(y); \
    sData->S3SP_SCRATCH[(x-S3SP_TOP)/4]=(DWORD)(y); \
    sData->S3SP_SCRATCH[(x-S3SP_TOP)/4]=(DWORD)(y); \
}
#else
#define mmdwSetRegister( x, y )      (lpdwMemReg[x/4])=(DWORD)(y)
#endif
#define mmdwSetPFRegister( x, y )  (lpdwMemReg[x/4])=(DWORD)(y)
#define mmdwGetRegister( x )       lpdwMemReg[x/4]
#define mmGetRegister( x )           (lpMemReg[x/2])
#define mmSetRegister( x, y )        (lpMemReg[x/2])=(WORD)(y)
#define ioGetRegister( x )           (inpw((WORD)(x)))
#define ioSetRegister( x, y )        (outpw((WORD)(x),(WORD)(y)))

// --------------------------------------------------------------------------
#ifdef DEBUG

// these are for debugging that you are not doing i/o when on the secondary
WORD debug_inpw(WORD index)
{
    if (sDataCurrent == 0 || sDataCurrent->dwDeviceHandle != 1)
    {
        OutputDebugString("*** ERROR DDHAL IS DOING VGA IO ***\r\n");
        _asm int 1
    }
    return inpw(index);
}
BYTE debug_inp(WORD index)
{
    if (sDataCurrent == 0 || sDataCurrent->dwDeviceHandle != 1)
    {
        OutputDebugString("*** ERROR DDHAL IS DOING VGA IO ***\r\n");
        _asm int 1
    }
    return inp(index);
}
void debug_outpw(WORD index, WORD data)
{
    if (sDataCurrent == 0 || sDataCurrent->dwDeviceHandle != 1)
    {
        OutputDebugString("*** ERROR DDHAL IS DOING VGA IO ***\r\n");
        _asm int 1
    }
    outpw(index,data);
}
void debug_outp(WORD index, BYTE data)
{
    if (sDataCurrent == 0 || sDataCurrent->dwDeviceHandle != 1)
    {
        OutputDebugString("*** ERROR DDHAL IS DOING VGA IO ***\r\n");
        _asm int 1
    }
    outp(index,data);
}
#define inp   debug_inp
#define inpw  debug_inpw
#define outp  debug_outp
#define outpw debug_outpw
#endif
// --------------------------------------------------------------------------
#if 0
WORD getRegister(WORD x )
{
    if (MMIOONLY_HW_PRESENT)
        return (lpMemReg[x/2]);
    else
                return (inpw((WORD)(x)));
}
#endif
#define getRegister(x) (MMIOONLY_HW_PRESENT) ? (lpMemReg[x/2]) : inpw((WORD)(x))
// --------------------------------------------------------------------------
#if 0
void setRegister(WORD x, WORD y)
{
    if (MMIOONLY_HW_PRESENT)
        lpMemReg[x/2]=(WORD)(y);
    else
                outpw((WORD)(x),(WORD)(y));
}
#endif
#define setRegister(x,y) (MMIOONLY_HW_PRESENT) ? lpMemReg[x/2]=(WORD)(y) : outpw((WORD)(x),(WORD)(y))
// --------------------------------------------------------------------------
#define mmvirSetRegister( x, y )        (lpdwMemReg[x/4])=(DWORD)(y)
#define mmvirGetRegister(x)         (lpdwMemReg[x/4])

#define convertToGlobalCord(x,y,offset,pitch,bytes_per_pixel) \
{                                 \
    y += (WORD) (offset / pitch);     \
    x += (WORD) (offset % pitch)/bytes_per_pixel;         \
}

#define convertToGlobalCordEx(x,y,offset,pitch,bytes_per_pixel) \
{                                 \
        offset += y*pitch+x*bytes_per_pixel;            \
    y = (WORD) (offset / sData->dwScreenWidthBytes);     \
    x = (WORD) ((offset % sData->dwScreenWidthBytes)*8/sData->ddpf.dwRGBBitCount);         \
}

// D E B U G   I N F O ------------------------------------------------------

#ifdef DEBUG
#define DPF     Msg
#define START_STR   "S332: "
#define END_STR     ""
/*
 * Msg
 */
void __cdecl Msg( LPSTR szFormat, ... )
{
    char    str[256];

#ifdef  DRVNAME32
    lstrcpy(str, DRVNAME32 ": ");
#else
    lstrcpy( str, START_STR );
#endif
    wvsprintf( str+lstrlen( str ), szFormat, (LPVOID)(&szFormat+1) );
    lstrcat( (LPSTR) str, END_STR );
    OutputDebugString( str );
    OutputDebugString( "\r\n" );

} /* Msg */
#else
#define DPF     1 ? (void)0 : (void)
#endif


// F U N C T I O N S --------------------------------------------------------
// --------------------------------------------------------------------------
#if 0
 BYTE check_fifo(WORD x)
{
    BYTE nRet;
    nRet = ((BYTE)(((mmvirGetRegister(0x8504)) >> 8) & 0x1fL) < x);
    return nRet;
}
#endif
#define check_fifo(x) ((BYTE)(((mmvirGetRegister(0x8504)) >> 8) & 0x1fL) < x)

// --------------------------------------------------------------------------

#define vir_wait_for_fifo( x )  while( check_fifo( x ) );

// --------------------------------------------------------------------------

void PASCAL K2V_WriteCrReg(BYTE byIndex, BYTE byValue, LPS3THKINFO sData)
{
    if ( sData->lpS3DriverData->VGAFlag )
    {
        outp( 0x3d4, byIndex);
        outp( 0x3d5, byValue);
    }
    else
    {
        lpMemReg[0x83d4/2]=(((WORD)(byValue)) << 8) |  byIndex;
    }
}
// --------------------------------------------------------------------------

BYTE PASCAL K2V_ReadCrReg(BYTE byIndex, LPS3THKINFO sData)
{
    if ( sData->lpS3DriverData->VGAFlag )
    {
        outp(0x3d4, byIndex);
        return inp(0x3d5);
    }
    else
    {
        lpMemRegB[0x83d4]=byIndex;
        return lpMemRegB[0x83d5];
    }
}

// --------------------------------------------------------------------------

void wait_for_fifo( int x, LPS3THKINFO sData )
{
WORD Fifo, temp;
BOOL NotDone=1;
        if ((sData->lpS3DriverData->VGAFlag) && STREAMS_PROCESSOR_PRESENT)
        {
                while (NotDone)
                {
                        K2V_WriteCrReg(0x40, (BYTE)(K2V_ReadCrReg(0x40, sData)|1), sData);
                        K2V_WriteCrReg(0x53, (BYTE)(K2V_ReadCrReg(0x53, sData)|0x08), sData);
                        K2V_WriteCrReg(0x58, (BYTE)(K2V_ReadCrReg(0x58, sData)|0x13), sData);

                        temp = (getRegister( GP_STAT ));
                        Fifo = (temp&0xff)<<5 | (temp&0xf800)>>11;
                        NotDone = (Fifo>>(13-x)) & 1;
                }
        }
        else
        {
        temp = (getRegister( GP_STAT ));
                Fifo = (temp&0xff)<<5 | (temp&0xf800)>>11;
                NotDone = (Fifo>>(13-x)) & 1;
                if (NotDone) wait_for_fifo( x,sData ); //call self recuresively
        }
}

// --------------------------------------------------------------------------

BOOL draw_engine_busy(LPS3THKINFO sData)
{
        if ((sData->lpS3DriverData->VGAFlag) && STREAMS_PROCESSOR_PRESENT)
        {
            //K2V_WriteCrReg(0x38, 0x48, sData);
            //K2V_WriteCrReg(0x39, 0xa5, sData);
            K2V_WriteCrReg(0x40, (BYTE)(K2V_ReadCrReg(0x40, sData)|1), sData);
            K2V_WriteCrReg(0x53, (BYTE)(K2V_ReadCrReg(0x53, sData)|0x08), sData);
            K2V_WriteCrReg(0x58, (BYTE)(K2V_ReadCrReg(0x58, sData)|0x13), sData);
            return ((getRegister( GP_STAT )) & HDW_BUSY);
        }
        else return ((getRegister( GP_STAT )) & HDW_BUSY);

}

// --------------------------------------------------------------------------
BOOL virge_draw_engine_busy(LPS3THKINFO sData)
{
    BOOL bRet;
    bRet = ((mmvirGetRegister(0x8504)) & 0x2000) ? FALSE : TRUE;
    return bRet;
}


// --------------------------------------------------------------------------
BOOL room_for_blt(LPS3THKINFO sData)
{
    return (test_fifo( 13 ));
}


// --------------------------------------------------------------------------
BOOL virge_room_for_blt(LPS3THKINFO sData)
{
    return (check_fifo( 13 ));
}

// --------------------------------------------------------------------------
void wait_for_idle(LPS3THKINFO sData)
{
        if ((sData->lpS3DriverData->VGAFlag) && STREAMS_PROCESSOR_PRESENT)
        {
                while ((getRegister( GP_STAT )) & HDW_BUSY)
        {
                        //K2V_WriteCrReg(0x38, 0x48, sData);
                        //K2V_WriteCrReg(0x39, 0xa5, sData);
                        K2V_WriteCrReg(0x40, (BYTE)(K2V_ReadCrReg(0x40, sData)|1), sData);
                        K2V_WriteCrReg(0x53, (BYTE)(K2V_ReadCrReg(0x53, sData)|0x08), sData);
                        K2V_WriteCrReg(0x58, (BYTE)(K2V_ReadCrReg(0x58, sData)|0x13), sData);
        }

        }
        else if ((getRegister( GP_STAT )) & HDW_BUSY)
        wait_for_idle(sData);//call self recursively

}

// --------------------------------------------------------------------------

void virge_wait_for_idle(LPS3THKINFO sData)
{
    while(virge_draw_engine_busy(sData));
}
// --------------------------------------------------------------------------

void trio_mm8200(LPS3THKINFO sData)
{
    //null for TRIO family
}
// --------------------------------------------------------------------------

void virge_mm8200(LPS3THKINFO sData)
{
    mmvirSetRegister(0x8200, ((sData->lpSpod->dwFifo) & 0x0000001F) |
                            (((sData->lpSpod->dwFifo) & 0x000003E0) << 1) |
                            (((sData->lpSpod->dwFifo) & 0x00007C00) << 2));
}
// --------------------------------------------------------------------------


 void check_DMA(LPS3THKINFO sData)
{

    if ( mmvirGetRegister(0x859C) & 1 )
    {
        sData->write_ptr = mmvirGetRegister(0x8594)&0xffc;
        sData->read_ptr  = mmvirGetRegister(0x8598)&0xffc;
        while ( sData->write_ptr != sData->read_ptr )
        {
            sData->read_ptr  = mmvirGetRegister(0x8598)&0xffc;
        }

//      virge_wait_for_idle(sData) ;

//fred        mmvirSetRegister(0x859C,0);
    }

}

/*---------------------------------------------------------------------------
 *
 * GetPaletteEntry
 *
 */
DWORD __stdcall GetPaletteEntry(LPS3THKINFO sData, BYTE index)
{
#if 0   // we cant read VGA IO in our HAL, if we want to work
          // as a multimonitor adapter

    BYTE bRed, bGrn, bBlu;

        outp(0x3c7, (BYTE) index);
        bRed = inp(0x3c9) <<2;
        bGrn = inp(0x3c9) <<2;
        bBlu = inp(0x3c9) <<2;
    return( (((DWORD)bRed <<16)&0xff0000) |
            (((DWORD)bGrn <<8 )&0x00ff00) |
                  (DWORD)bBlu );
#else
        return sData->lpColorTable[index] & 0x00FCFCFC;
#endif
}

/*---------------------------------------------------------------------------
 *
 * Get K2V FIFO and RAS Controls (MM81EC)
 *
 * For given device type and the destination width,
 * return optimum FIFO setting from K2VTABLE.
 *
 */
DWORD __stdcall GetK2vFifo( WORD deviceType, WORD wSize,LPS3THKINFO sData)
{
    int i;
    DWORD result;

    for( i = 0; i < DSTWIN_SIZES; i++)
        if( wSize <= sData->k2vTable[deviceType].wStep[i].fifo.dstWidth)
            break;

    if( i >= DSTWIN_SIZES) i--;

   result = sData->k2vTable[deviceType].wStep[i].fifo.value;

   if ((result == 0) || (sData->band.SWFlags & SW_CALCULATE))
   {
      result = sData->band.FifoCtrl;
   }

   DPF("Fifo: %x", result);

    return(result);
}

/*---------------------------------------------------------------------------
 *
 * GetK2vRatio
 *
 * For given device, return closest horizontal stretch ratio from K2VTABLE.
 *
 */
DWORD __stdcall GetK2vRatio( WORD deviceType, WORD wSize,LPS3THKINFO sData)
{
    int i;
    DWORD result;

    for( i = 0; i < DSTWIN_SIZES; i++)
        if( wSize <= sData->k2vTable[deviceType].wStep[i].ratio.dstWidth)
            break;

    if( i >= DSTWIN_SIZES) i--;

   result = sData->k2vTable[deviceType].wStep[i].ratio.value;

   if ((result == 0) || (sData->band.SWFlags & SW_CALCULATE))
   {
      result = sData->band.HStretch;
   }

   DPF("HStretch %d", result);

    return(result);
}

/*---------------------------------------------------------------------------
 *
 * updateFlipStatus
 *
 * checks and sees if the most recent flip has occurred
 *
 */
 __inline HRESULT updateFlipStatus( FLATPTR fpVidMem, LPS3THKINFO sData )
{
//  DPF("in updateFlipStatus");
    /*
     * get the monitor frequency after a mode reset
     */
    if( sData->bReset )
    {
    getDisplayDuration(sData);
    sData->bReset = FALSE;
    }

/*Added option switch for ViRGE Family
 *to Disable Waitfor Vertical Sync.
 *
 */
    if  (!sData->bWaitForVSync)
    {
    sData->flipRecord.bFlipFlag = FALSE;
    return DD_OK;
    }

    if( sData->flipRecord.bFlipFlag &&
    ((fpVidMem == 0) || (fpVidMem == sData->flipRecord.fpFlipFrom) ) )
    {
    __int64 ttime;

    if( IN_VBLANK )
    {
        if( sData->flipRecord.bWasEverInDisplay )
        {
        sData->flipRecord.bHaveEverCrossVBlank = TRUE;
        }
    }
    else //if( IS_DISPLAY_ENABLE )
    {
        if( sData->flipRecord.bHaveEverCrossVBlank )
        {
        sData->flipRecord.bFlipFlag = FALSE;
        return DD_OK;
        }
        sData->flipRecord.bWasEverInDisplay = TRUE;
    }
    QueryPerformanceCounter((LARGE_INTEGER *) &ttime);
    if( (DWORD)(ttime - sData->flipRecord.liFlipTime) <= sData->flipRecord.dwFlipDuration )
    {
        return DDERR_WASSTILLDRAWING;
    }
    sData->flipRecord.bFlipFlag = FALSE;
    }
    return DD_OK;

} /* updateFlipStatus */

/*---------------------------------------------------------------------------
 *
 * S3UpdateFlipStatus
 *
 * This is a stub function for 'updateFlipStatus'.
 * It is created so that inline function 'updateFlipStatus' can be
 * refrenced outside of this module.
 *
 */
HRESULT S3UpdateFlipStatus( FLATPTR fpVidMem, LPS3THKINFO sData )
{
   return updateFlipStatus( fpVidMem, sData );
}

/*---------------------------------------------------------------------------
 *
 * getDisplayDuration
 *
 * get the length (in milliseconds) of a refresh cycle
 *
 */
 void getDisplayDuration(LPS3THKINFO sData)
{
    int     i;
    __int64     start,end;
    DWORD   oldclass;
    HANDLE  hprocess;
    BOOL    bQueryReturnVal;

//  DPF("in getDisplayDuration");
    memset( &sData->flipRecord, 0, sizeof( sData->flipRecord ) );

    hprocess = GetCurrentProcess();
    oldclass = GetPriorityClass( hprocess );
    SetPriorityClass( hprocess, REALTIME_PRIORITY_CLASS );

    if (sData->lpS3MobileData->wSPStyle & SP_M5 )
    {
    lpMemReg[0x83c4/2]=(WORD)0x4026;
    }

    while(IN_VBLANK);
    while(IN_DISPLAY);
    bQueryReturnVal = QueryPerformanceCounter((LARGE_INTEGER *)&start);
    #define LOOP_CNT    20
    for( i=0;i<LOOP_CNT;i++ )
    {
    while(IN_VBLANK);
    while(IN_DISPLAY);
    }

    if(!QueryPerformanceCounter((LARGE_INTEGER *)&end) || !bQueryReturnVal)
    {
    DPF("Line %d.  Call to QueryPerformanceCounter() failed.",__LINE__);
    }
    SetPriorityClass( hprocess, oldclass );
    sData->flipRecord.dwFlipDuration = ((DWORD)(end-start) + LOOP_CNT/2)/LOOP_CNT;

#ifdef DEBUG
    QueryPerformanceFrequency((LARGE_INTEGER *)&end);

    DPF( "Flip duration = %ld ticks, %ldms, %ldHz",
        sData->flipRecord.dwFlipDuration,
    (sData->flipRecord.dwFlipDuration * 1000)/(DWORD)end,
    ((DWORD)end/sData->flipRecord.dwFlipDuration));
#endif

    sData->flipRecord.liFlipTime = start;
    sData->flipRecord.bFlipFlag = FALSE;
    sData->flipRecord.fpFlipFrom = 0;
    DPF( "dwFlipFail = %ld", sData->dwFlipFail );
    sData->dwFlipFail = 0;
} /* getDisplayDuration */

/*
 * Blt32Virge()
 *
 * ViRGE implementation of Blt32()
 */
DWORD __stdcall Blt32Virge( LPDDHAL_BLTDATA lpBlt)
{
    LPDDRAWI_DDRAWSURFACE_LCL   srcSurfx;
    LPDDRAWI_DDRAWSURFACE_GBL   srcSurf;
    LPDDRAWI_DDRAWSURFACE_LCL   destSurfx;
    LPDDRAWI_DDRAWSURFACE_GBL   destSurf;

    HRESULT ddrval;
    DWORD   destOffset;
    WORD    destPitch;
    WORD    destX;
    WORD    bytes_per_pixel;
    WORD    dst_bytes_per_pixel, fudge;
    WORD    destY;
    WORD    direction;
    DWORD   dwFlags;
    WORD    height;
    RECTL   rDest;
    RECTL   rSrc;
    BYTE    rop;
    DWORD   sourceOffset;
    WORD    srcPitch;
    WORD    srcX;
    WORD    srcY;
    WORD    width;
    LPS3THKINFO sData = GetMyRefData(lpBlt->lpDD);

    check_DMA(sData) ;

//  DPF("in Blt32Virge");
    destSurfx = lpBlt->lpDDDestSurface;
    destSurf  = destSurfx->lpGbl;

//    if (destSurfx->dwFlags & DDRAWISURF_HASPIXELFORMAT)
//    {
//        switch( destSurf->ddpfSurface.dwFourCC)
//        {
//            case FOURCC_YUY2:
//            case FOURCC_Y211:
//                lpBlt->ddRVal = DDERR_UNSUPPORTED;
//                return DDHAL_DRIVER_NOTHANDLED;
//        }
//    }

    /*
     * is a flip in progress?
     */
    ddrval = updateFlipStatus( destSurf->fpVidMem, sData );
    if( ddrval != DD_OK )
    {
        lpBlt->ddRVal = ddrval;
        return DDHAL_DRIVER_HANDLED;
    }

    /*
     * If async, then only work if bltter isn't busy
     * This should probably be a little more specific to each call, but
     * waiting for 16 is pretty close
     */
    dwFlags = lpBlt->dwFlags;
    if( dwFlags & DDBLT_ASYNC )
    {
        if( check_fifo( 13 ) )
        {
            DPF( "ASYNC FAILED" );
            lpBlt->ddRVal = DDERR_WASSTILLDRAWING;
            return DDHAL_DRIVER_HANDLED;
        }
    }

    /*
     * copy src/dest rects
     */
    bytes_per_pixel = (WORD)sData->ddpf.dwRGBBitCount /8;
    if (destSurfx->dwFlags & DDRAWISURF_HASPIXELFORMAT)
    {
        bytes_per_pixel = dst_bytes_per_pixel = (WORD)(destSurf->ddpfSurface.dwRGBBitCount)>>3;
    }
    else
    {
        dst_bytes_per_pixel = bytes_per_pixel;
    }
    fudge = dst_bytes_per_pixel / bytes_per_pixel;
    rDest = lpBlt->rDest;

    destX     = (WORD)rDest.left;
    destY     = (WORD)rDest.top;
    width     = (WORD)(rDest.right - rDest.left);
    height    = (WORD)(rDest.bottom - rDest.top);
    destPitch = (WORD)destSurf->lPitch;
    destOffset= (destSurf->fpVidMem - sData->dwScreenFlatAddr);

//### patch: do a dummy screen to screen blt in case we were called right
//###        after a 3D command
    vir_wait_for_fifo( 7 );
    mmvirSetRegister(VI_SRC_BASE, 0L);
    mmvirSetRegister(VI_DEST_BASE, 0L);
    mmvirSetRegister(VI_DEST_SRC_STR, (DWORD)destPitch << 16 | destPitch);
    mmvirSetRegister(VI_RWIDTH_HEIGHT, 1L);
    mmvirSetRegister(VI_RSRC_XY, 0L);
    mmvirSetRegister(VI_RDEST_XY, 0L);
    mmvirSetRegister(VI_2D_CMD_SET, VI_CMD_BITBLT |
                                    (0xCCL << 17) |
                                    (1L << 25) |                 // left to right
                                    (1L << 26) |                 // top to bottom
                                    VI_2DCMD_DE |
                                    ((DWORD)(bytes_per_pixel-1) << 2));

    if( !(dwFlags & DDBLT_ROP) )
    {
        if( (dwFlags & DDBLT_COLORFILL) || (dwFlags & DDBLT_DEPTHFILL) )
        {
            vir_wait_for_fifo( 8 );
            mmvirSetRegister(VI_DEST_BASE, destOffset);                        // setup dest. base address
                if( dwFlags & DDBLT_COLORFILL )
                mmvirSetRegister(VI_PAT_FG_CLR, (DWORD)lpBlt->bltFX.dwFillColor);    // setup fill color
                else
                    mmvirSetRegister(VI_PAT_FG_CLR, (DWORD)lpBlt->bltFX.dwFillDepth);
            mmvirSetRegister(VI_DEST_SRC_STR, (DWORD)destPitch << 16);
            mmvirSetRegister(0xa4e8, -1L);
            mmvirSetRegister(0xa4ec, -1L);
            mmvirSetRegister(VI_RWIDTH_HEIGHT, ((DWORD)(width-1) << 16) | (DWORD)height);
            mmvirSetRegister(VI_RDEST_XY, (DWORD)(destX << 16) | destY);
            mmvirSetRegister(VI_2D_CMD_SET, VI_CMD_BITBLT |
                                           (0xF0L << 17) |
                                           (1L << 25) |                 // left to right
                                           (1L << 26) |                 // top to bottom
                                           VI_2DCMD_MP |
                                           VI_2DCMD_DE |
                                           ((DWORD)(bytes_per_pixel-1) << 2));
            lpBlt->ddRVal = DD_OK;
            return DDHAL_DRIVER_HANDLED;
        }
        DPF("Blt Not Handled");
        return DDHAL_DRIVER_NOTHANDLED;
    }
    srcSurfx = lpBlt->lpDDSrcSurface;
    if( srcSurfx )
    {
        srcSurf   = srcSurfx->lpGbl;
        rSrc      = lpBlt->rSrc;
        srcX      = (WORD)rSrc.left;
        srcY      = (WORD)rSrc.top;
        srcPitch  = (WORD)srcSurf->lPitch;
        sourceOffset = (srcSurf->fpVidMem -  sData->dwScreenFlatAddr);

        direction = TOP_TO_BOTTOM;
        if( (destSurf == srcSurf) && (srcX + width  > destX) &&
            (srcY + height > destY) && (destX + width > srcX) &&
            (destY + height > srcY) &&
            (((srcY == destY) && (destX > srcX) )
            || ((srcY != destY) && (destY > srcY) )) )
        {
            direction = BOTTOM_TO_TOP;
            srcX = (WORD)rSrc.right - 1;
            srcY = (WORD)rSrc.bottom - 1;
            destX = (WORD)rDest.right - 1;
            destY = (WORD)rDest.bottom - 1;
        }
    }

    /*
     * get offset, width, and height for source
     */
    rop = (BYTE) (lpBlt->bltFX.dwROP >> 16);

    if( dwFlags & DDBLT_ROP )
    {
//      if( rop == (SRCCOPY >> 16) )
//      {
            WORD sxf, dxf;
            DWORD dwCmd;
                DWORD srcDiff, destDiff;        // kclxxx

            dwCmd = VI_CMD_BITBLT |
                          VI_2DCMD_HC |
                    ((DWORD)rop << 17) |
                    VI_2DCMD_DE |
                    ((DWORD)((direction==TOP_TO_BOTTOM)?1:0) << 25) |
                    ((DWORD)((direction==TOP_TO_BOTTOM)?1:0) << 26) |
                    ((DWORD)(bytes_per_pixel-1) << 2);
            sxf = (srcX + width-1)*bytes_per_pixel + bytes_per_pixel-1;
            dxf = (destX + width-1)*bytes_per_pixel + bytes_per_pixel-1;
            vir_wait_for_fifo( 12 );
         mmvirSetRegister(VI_CLIP_L_R, 0x000007ff);
         mmvirSetRegister(VI_CLIP_T_B, 0x000007ff);
            if ((sxf&0x07) > (dxf&0x07))    // patch for screen to screen copy
            {
                mmvirSetRegister(VI_CLIP_L_R, ((DWORD)rDest.left << 16) | (rDest.left + width-1));
                mmvirSetRegister(VI_CLIP_T_B, ((DWORD)rDest.top << 16)  | (rDest.top + height-1));      // kclxxx
//www                mmvirSetRegister(VI_CLIP_L_R, ((DWORD)destX << 16) | (destX + width-1));
//wwww                mmvirSetRegister(VI_CLIP_T_B, ((DWORD)destY << 16) | (destY + height-1));     // kclxxx
//###                width = width + (8-(sxf&0x07))/bytes_per_pixel;
                width = width + (8-(sxf&0x07)+ bytes_per_pixel/2)/bytes_per_pixel;
                dwCmd |= VI_2DCMD_HC;       // hardware clipping
            }
            if (dwFlags & DDBLT_KEYSRCOVERRIDE)
            {
                if (bytes_per_pixel == 1)
                {
                    mmvirSetRegister(VI_SRC_FG_CLR, (lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue & 0xff) |
//                                                   ((lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue & 0xff) << 24) |
//                                                   ((lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue & 0xff) << 16) |
                                                   ((lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue & 0xff) << 8));  // need to setup bit 8-15 as well
                }
                else
                    mmvirSetRegister(VI_SRC_FG_CLR, lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue);
                dwCmd |= VI_2DCMD_TP;       // transparent blt
            }
            // kclxxx begin
            srcDiff = sourceOffset - (sourceOffset & 0xFFFFFFF8);
         mmvirSetRegister(VI_SRC_BASE, sourceOffset & 0xfffffff8);     // setup source base address
         mmvirSetRegister(VI_DEST_SRC_STR, ((DWORD)(destPitch & 0xFFF8) << 16) | (DWORD)(srcPitch & 0xFFF8));
            destDiff = destOffset - (destOffset & 0xFFFFFFF8);
         mmvirSetRegister(VI_DEST_BASE, destOffset & 0xfffffff8);      // setup dest. base address
         mmvirSetRegister(VI_RWIDTH_HEIGHT, ((DWORD)(width-1) << 16) | (DWORD)height);
         mmvirSetRegister(VI_RSRC_XY, ((DWORD)(srcX << 16) | (DWORD)srcY));
         mmvirSetRegister(VI_RDEST_XY, ((DWORD)(destX) << 16) | (DWORD)(destY));
         mmvirSetRegister(VI_2D_CMD_SET, dwCmd);
            // kclxxx end
//      }
    }
    else
    {
        return DDHAL_DRIVER_NOTHANDLED;
    }
    lpBlt->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

}

/*---------------------------------------------------------------------------
 *
 * Blt32
 *
 * 32-bit blt routine
 *
 */
DWORD __stdcall Blt32( LPDDHAL_BLTDATA lpBlt )
{
    LPDDRAWI_DDRAWSURFACE_LCL   srcSurfx;
    LPDDRAWI_DDRAWSURFACE_GBL   srcSurf;
    LPDDRAWI_DDRAWSURFACE_LCL   destSurfx;
    LPDDRAWI_DDRAWSURFACE_GBL   destSurf;

    HRESULT ddrval;
    DWORD   destOffset;
    WORD    destPitch;
    WORD    destX;
    WORD    bytes_per_pixel,src_bytes_per_pixel;
    WORD    dst_bytes_per_pixel, fudge, fudge2;
    WORD    destY;
    WORD    direction;
    DWORD   dwFlags;
    WORD    height;
    RECTL   rDest;
    RECTL   rSrc;
    BYTE    rop;
    DWORD   sourceOffset;
    WORD    srcPitch;
    WORD    srcX;
    WORD    srcY;
    WORD    width;
    LPS3THKINFO sData = GetMyRefData(lpBlt->lpDD);

//  DPF("in Blt32");
    // Let DIB engine handles this blt if 24bpp modes

    if (sData->ddpf.dwRGBBitCount == 24)
    {
    lpBlt->ddRVal = DDERR_UNSUPPORTED;
    return DDHAL_DRIVER_NOTHANDLED;
    }

    destSurfx = lpBlt->lpDDDestSurface;
    destSurf  = destSurfx->lpGbl;

//    if (destSurfx->dwFlags & DDRAWISURF_HASPIXELFORMAT)
//    {
//        switch( destSurf->ddpfSurface.dwFourCC)
//        {
//            case FOURCC_YUY2:
//            case FOURCC_Y211:
//                lpBlt->ddRVal = DDERR_UNSUPPORTED;
//                return DDHAL_DRIVER_NOTHANDLED;
//        }
//    }

    /*
     * is a flip in progress?
     */
    ddrval = updateFlipStatus( destSurf->fpVidMem, sData );
    if( ddrval != DD_OK )
    {

    lpBlt->ddRVal = ddrval;
    return DDHAL_DRIVER_HANDLED;
    }

    /*
     * If async, then only work if bltter isn't busy
     * This should probably be a little more specific to each call, but
     * waiting for 16 is pretty close

     */

    dwFlags = lpBlt->dwFlags;
    if( dwFlags & DDBLT_ASYNC )
    {
    if( test_fifo( 13 ) )
    {
        DPF( "ASYNC FAILED" );
        lpBlt->ddRVal = DDERR_WASSTILLDRAWING;
        return DDHAL_DRIVER_HANDLED;
    }
    }

    /*
     * copy src/dest rects
     */
    bytes_per_pixel = (WORD)sData->ddpf.dwRGBBitCount /8;
   if (destSurfx->dwFlags & DDRAWISURF_HASPIXELFORMAT)
    {
        dst_bytes_per_pixel = (WORD)(destSurf->ddpfSurface.dwRGBBitCount)>>3;
    }
    else
    {
        dst_bytes_per_pixel = bytes_per_pixel;
    }
    fudge = dst_bytes_per_pixel / bytes_per_pixel;
    fudge2= bytes_per_pixel / dst_bytes_per_pixel;
    rDest = lpBlt->rDest;

    destX     = (WORD)rDest.left;
    destY     = (WORD)rDest.top;
    width     = (WORD)(rDest.right - rDest.left);
    height    = (WORD)(rDest.bottom - rDest.top);
    destPitch = (WORD)destSurf->lPitch;
    destOffset= (destSurf->fpVidMem - sData->dwScreenFlatAddr);

    if( !(dwFlags & DDBLT_ROP) )
    {
    if( dwFlags & DDBLT_COLORFILL )
    {
        convertToGlobalCordEx(destX,destY,destOffset,destPitch,dst_bytes_per_pixel);
                if (destPitch != (WORD)sData->dwScreenWidthBytes){
                        height=(WORD)((destPitch*height+sData->dwScreenWidthBytes-1)/sData->dwScreenWidthBytes);
                        width=(WORD)sData->dwScreenWidth;
                }
        wait_for_fifo( 6, sData );  //911 has max of 8 fifo

        if (bytes_per_pixel > 2)
        setRegister( MULTILFUNCTION_CONTROL, DISABLE_COLOR_COMPARE ); // Clear RSF bit
        setRegister( FRGD_MIX, COLOR_FILL_MIX );
       setRegister( FRGD_COLOR, (WORD)lpBlt->bltFX.dwFillColor );
        if (bytes_per_pixel > 2)
            setRegister( FRGD_COLOR, (WORD)(lpBlt->bltFX.dwFillColor >> 16));
        setRegister( PIXEL_CNTL, FORGROUND_SELECTED );
        setRegister( CUR_X, destX );
        wait_for_fifo( 6, sData );
        setRegister( CUR_Y, destY );
        setRegister( MAJ_AXIS_PCNT, (WORD)(width-1) );
        setRegister( MIN_AXIS_PCNT, (WORD)(height-1) );
        setRegister( CMD, COLOR_FILL_COMMAND );
        setRegister( MULTILFUNCTION_CONTROL, (WORD)sData->dwMULT_MISC); // Clear RSF bit
        setRegister( FRGD_MIX, 0x27 );

        lpBlt->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }
    DPF("Blt Not Handled");
    return DDHAL_DRIVER_NOTHANDLED;
    }
    srcSurfx = lpBlt->lpDDSrcSurface;
    if( srcSurfx )
    {
    srcSurf   = srcSurfx->lpGbl;
    rSrc      = lpBlt->rSrc;
    srcX      = (WORD)rSrc.left;
    srcY      = (WORD)rSrc.top;
    srcPitch  = (WORD)srcSurf->lPitch;
    sourceOffset = (srcSurf->fpVidMem -  sData->dwScreenFlatAddr);
   if (srcSurfx->dwFlags & DDRAWISURF_HASPIXELFORMAT)
    {
        src_bytes_per_pixel = (WORD)(srcSurf->ddpfSurface.dwRGBBitCount)>>3;
    }
    else
    {
        src_bytes_per_pixel = bytes_per_pixel;
    }

    direction = TOP_TO_BOTTOM;
    if( (destSurf == srcSurf) && (srcX + width  > destX) &&
        (srcY + height > destY) && (destX + width > srcX) &&
        (destY + height > srcY) &&
        (((srcY == destY) && (destX > srcX) )
         || ((srcY != destY) && (destY > srcY) )) )
     {
          direction = BOTTOM_TO_TOP;
          srcX = (WORD)rSrc.right - 1;
          srcY = (WORD)rSrc.bottom - 1;
          destX = (WORD)rDest.right - 1;
          destY = (WORD)rDest.bottom - 1;
     }
    }

    /*
     * get offset, width, and height for source
     */
    rop = (BYTE) (lpBlt->bltFX.dwROP >> 16);

    if( dwFlags & DDBLT_ROP )
    {
      if( rop == (SRCCOPY >> 16) )
      {
    convertToGlobalCordEx(srcX,srcY,sourceOffset,srcPitch,src_bytes_per_pixel);
    convertToGlobalCordEx(destX,destY,destOffset,destPitch,dst_bytes_per_pixel);
                if (destPitch != (WORD)sData->dwScreenWidthBytes){
                        height=(WORD)((destPitch*height+sData->dwScreenWidthBytes-1)/sData->dwScreenWidthBytes);
                        width=(WORD)sData->dwScreenWidth;
                }

    if( dwFlags & DDBLT_KEYSRCOVERRIDE )
    {
//      DPF("Transparent blt color key = 0x%04x",(WORD)lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue);
//      DPF("From org = 0x%08lx x= %d y = %d",sourceOffset,srcX,srcY);
//      DPF("From org = 0x%08lx x= %d y = %d",destOffset,destX,destY);
//      DPF("width = %d height = %d",width,height);

        wait_for_fifo( 13, sData );
        setRegister( MULTILFUNCTION_CONTROL, ENABLE_COLOR_COMPARE );
       setRegister( COLOR_CMP,(WORD)lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue );
        if (bytes_per_pixel > 2)
          setRegister( COLOR_CMP,(WORD)(lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue >> 16) );
        setRegister( PIXEL_CNTL,FORGROUND_SELECTED);
        setRegister( FRGD_MIX,SCR_COPY_MIX );
        setRegister( CUR_X,srcX );
        setRegister( CUR_Y,srcY );
        setRegister( DESTX_DIASTP, destX );
        setRegister( DESTY_AXSTP, destY );
        setRegister( MAJ_AXIS_PCNT, (WORD)(width-1) );
        setRegister( MIN_AXIS_PCNT, (WORD)(height-1) );
        setRegister( CMD, (WORD)(SCRN_SCRN_BLT | direction) );
        setRegister( FRGD_MIX, 0x27 );
        setRegister( MULTILFUNCTION_CONTROL, (WORD)sData->dwMULT_MISC); // Clear RSF bit

    }
    else
    {
//      DPF("Src Scr blt");
//      DPF("From org = 0x%08lx x= %d y = %d",sourceOffset,srcX,srcY);
//      DPF("From org = 0x%08lx x= %d y = %d",destOffset,destX,destY);
//      DPF("width = %d height = %d",width,height);

        if (fudge !=0)
            width *= fudge; // K L U D G E !!!!!
        if (fudge2 !=0)
            width /= fudge2; // K L U D G E !!!!!

        wait_for_fifo( 8, sData );
        setRegister( PIXEL_CNTL,FORGROUND_SELECTED);
        setRegister( FRGD_MIX,SCR_COPY_MIX );
        setRegister( CUR_X,srcX );
        setRegister( CUR_Y,srcY );
        setRegister( DESTX_DIASTP, destX );
        setRegister( DESTY_AXSTP, destY );
        setRegister( MAJ_AXIS_PCNT, (WORD)(width - 1) );
        setRegister( MIN_AXIS_PCNT, (WORD)(height - 1) );
        wait_for_fifo( 2, sData );
        setRegister( CMD, (WORD)(SCRN_SCRN_BLT | direction) );
        setRegister( FRGD_MIX, 0x27 );
    }
      }
    }
    else
    {
    return DDHAL_DRIVER_NOTHANDLED;
    }
    lpBlt->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

} /* Blt32 */

/*---------------------------------------------------------------------------
 *
 * Blt32_968
 *
 * 32-bit blt routine for S3 868/968
 *
 */
DWORD __stdcall Blt32_968( LPDDHAL_BLTDATA lpBlt )
{
    LPDDRAWI_DDRAWSURFACE_LCL   srcSurfx;
    LPDDRAWI_DDRAWSURFACE_GBL   srcSurf;
    LPDDRAWI_DDRAWSURFACE_LCL   destSurfx;
    LPDDRAWI_DDRAWSURFACE_GBL   destSurf;

    HRESULT ddrval;
    DWORD   destOffset;
    WORD    destPitch;
    WORD    destX;
    WORD    bytes_per_pixel;
    WORD    dst_bytes_per_pixel, fudge, fudge2;
    WORD    destY;
    WORD    direction;
    DWORD   dwFlags;
    WORD    height;
    RECTL   rDest;
    RECTL   rSrc;
    BYTE    rop;
    DWORD   sourceOffset;
    WORD    srcPitch;
    WORD    srcX;
    WORD    srcY;
    WORD    width;

    LPDDPIXELFORMAT lpDPFsrc, lpDPFdest;
    WORD        src_bytes_per_pixel;
    WORD        srcWidth, srcHeight, srcSurfWidthBytes;
    BYTE        bitsPerPixel;
    WORD        destFinalX,destFinalY;
    WORD        destFinalWidth;
    DWORD       dwVEctrl;
    DWORD       dwVEdda;
    DWORD       dwVEstep;
    DWORD       dwVEcrop;
    DWORD       dwVEdestAddr;
    DWORD       dwVEsrcAddr;
    LONG        YPos, YNeg, YErr, YDDAAdj;
    WORD        i;
    LPS3THKINFO sData = GetMyRefData(lpBlt->lpDD);

    //  DPF("in Blt32");
    // Let DIB engine handles this blt if 24bpp modes

    if (sData->ddpf.dwRGBBitCount == 24)
    {
    lpBlt->ddRVal = DDERR_UNSUPPORTED;
    return DDHAL_DRIVER_NOTHANDLED;
    }

    destSurfx = lpBlt->lpDDDestSurface;
    destSurf  = destSurfx->lpGbl;

//   if (destSurfx->dwFlags & DDRAWISURF_HASPIXELFORMAT)
//    {
//        switch( destSurf->ddpfSurface.dwFourCC)
//        {
//            case FOURCC_YUY2:
//            case FOURCC_Y211:
//                lpBlt->ddRVal = DDERR_UNSUPPORTED;
//                return DDHAL_DRIVER_NOTHANDLED;
//        }
//    }

    /*
     * is a flip in progress?
     */
    ddrval = updateFlipStatus( destSurf->fpVidMem, sData );
    if( ddrval != DD_OK )
    {

    lpBlt->ddRVal = ddrval;
    return DDHAL_DRIVER_HANDLED;
    }

    /*
     * If async, then only work if bltter isn't busy
     * This should probably be a little more specific to each call, but
     * waiting for 16 is pretty close
     */

    dwFlags = lpBlt->dwFlags;
    if( dwFlags & DDBLT_ASYNC )
    {
    if( test_fifo( 13 ) )
    {
        DPF( "ASYNC FAILED" );
        lpBlt->ddRVal = DDERR_WASSTILLDRAWING;
        return DDHAL_DRIVER_HANDLED;
    }
    }

    /*
     * copy src/dest rects
     */
    bytes_per_pixel = (WORD)sData->ddpf.dwRGBBitCount /8;
   if (destSurfx->dwFlags & DDRAWISURF_HASPIXELFORMAT)
    {
        dst_bytes_per_pixel = (WORD)(destSurf->ddpfSurface.dwRGBBitCount)>>3;
    }
    else
    {
        dst_bytes_per_pixel = bytes_per_pixel;
    }
    fudge = dst_bytes_per_pixel / bytes_per_pixel;
    fudge2= bytes_per_pixel / dst_bytes_per_pixel;
    rDest = lpBlt->rDest;

    destX     = (WORD)rDest.left;
    destY     = (WORD)rDest.top;
    width = (WORD)(rDest.right - rDest.left);
    height= (WORD)(rDest.bottom - rDest.top);
    destPitch = (WORD)destSurf->lPitch;
    destOffset= (destSurf->fpVidMem - sData->dwScreenFlatAddr);

    if( !(dwFlags & DDBLT_ROP) )
    {
    if( dwFlags & DDBLT_COLORFILL )
    {
        convertToGlobalCord(destX,destY,destOffset,destPitch,bytes_per_pixel);

        wait_for_fifo( 6, sData ); //911 has max of 8 fifo
//        setRegister( MULTILFUNCTION_CONTROL, DISABLE_COLOR_COMPARE ); // Clear RSF bit
        setRegister( FRGD_MIX, COLOR_FILL_MIX );
       setRegister( FRGD_COLOR, (WORD)lpBlt->bltFX.dwFillColor );
        if (bytes_per_pixel > 2)
            setRegister( FRGD_COLOR, (WORD)(lpBlt->bltFX.dwFillColor >> 16));
        setRegister( PIXEL_CNTL, FORGROUND_SELECTED );
        setRegister( CUR_X, destX );
        wait_for_fifo( 6, sData );
        setRegister( CUR_Y, destY );
        setRegister( MAJ_AXIS_PCNT, (WORD)(width-1) );
        setRegister( MIN_AXIS_PCNT, (WORD)(height-1) );
        setRegister( CMD, COLOR_FILL_COMMAND );
        /*
         * 911/924 is brain-dead for everything on 16bpp!
         */
        if( (bytes_per_pixel == 2) &&
         (sData->wChipId == S3911) )
        {
         wait_for_fifo( 7, sData );
         setRegister( FRGD_MIX, COLOR_FILL_MIX );
         setRegister( FRGD_COLOR, (WORD)(lpBlt->bltFX.dwFillColor >> 8) );
         setRegister( CUR_X,(WORD)(destX+1024) );
         setRegister( CUR_Y, destY );
         setRegister( CMD, COLOR_FILL_COMMAND );
        }
//        setRegister( MULTILFUNCTION_CONTROL, (WORD)sData->dwMULT_MISC); // Clear RSF bit
        setRegister( FRGD_MIX, 0x27 );

        lpBlt->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }
    DPF("Blt Not Handled");
    return DDHAL_DRIVER_NOTHANDLED;
    }
    srcSurfx = lpBlt->lpDDSrcSurface;
    if( srcSurfx )
    {
    srcSurf   = srcSurfx->lpGbl;
    rSrc      = lpBlt->rSrc;
    srcX      = (WORD)rSrc.left;
    srcY      = (WORD)rSrc.top;
    srcPitch  = (WORD)srcSurf->lPitch;
    sourceOffset = (srcSurf->fpVidMem -  sData->dwScreenFlatAddr);

    direction = TOP_TO_BOTTOM;
    if( (destSurf == srcSurf) && (srcX + width  > destX) &&
        (srcY + height > destY) && (destX + width > srcX) &&
        (destY + height > srcY) &&
        (((srcY == destY) && (destX > srcX) )
         || ((srcY != destY) && (destY > srcY) )) )
     {
          direction = BOTTOM_TO_TOP;
          srcX = (WORD)rSrc.right - 1;
          srcY = (WORD)rSrc.bottom - 1;
          destX = (WORD)rDest.right - 1;
          destY = (WORD)rDest.bottom - 1;
     }
    }

    /*
     * get offset, width, and height for source
     */
    rop = (BYTE) (lpBlt->bltFX.dwROP >> 16);

    if( dwFlags & DDBLT_ROP )
    {
      if( rop == (SRCCOPY >> 16) )
      {
         GetValidDDPF(srcSurfx, lpDPFsrc);
         srcWidth  = (WORD)(rSrc.right - rSrc.left);
         srcHeight = (WORD)(rSrc.bottom - rSrc.top);

            /* Check for Video Acceleration */
         if( (width == srcWidth) && (height == srcHeight) &&
                !(lpDPFsrc->dwFlags & DDPF_FOURCC) )
               goto BLT32_SRCCOPY;      // Don't bother to use PIXEL_FORMATTER if it's SRCCOPY

         if( (PIXEL_FORMATTER_PRESENT) && !(dwFlags & DDBLT_KEYSRCOVERRIDE) )
         {
            if( ( width <= 32 ) ||
                ( srcWidth > 1280 ) )
               goto BLT32_PF_UNSUPPORTED;       //Don't handle tiny cases.

#define STRETCHFACTOR   4
            convertToGlobalCord(destX,destY,destOffset,destPitch,bytes_per_pixel);
            destFinalX=destX;
            destFinalY=destY;
            destFinalWidth=width;
            //DPF("destFinalX = %d, destFinalY = %d, destFinalWidth = %d",destFinalX,destFinalY,destFinalWidth);

PFStretch:

            if (destFinalWidth > srcWidth*STRETCHFACTOR )
            {
            //First pass sets the width to 4*src and destX should be 0
            //because we're now using the PFRgn in offscreen memory.  We'll
            //also adjust destY to point to the PFRgn start.
               width=srcWidth*STRETCHFACTOR;
               destX=0;
               destY=(WORD)((sData->dwPFRgnOffset-sData->dwScreenFlatAddr)/sData->dwScreenWidthBytes);
               //DPF("destX = %d, destY = %d, width = %d",destX,destY,width);
            }
            else
            {
               width=destFinalWidth;
               destX=destFinalX;
               destY=destFinalY;
            }
            dwVEctrl=0; //Initialize all PF variables.
            dwVEdda =0x10000000;
            dwVEstep=0x00080008; // Assume Step for 2MB+.
            dwVEcrop=0;
            dwVEdestAddr=0;
            dwVEsrcAddr=0;

            // X-DDA.

            if( width >= srcWidth )
            {
               dwVEctrl = ~(width) & 0x00000FFF; // Accumulator
               dwVEdda |= STRETCH | SCREEN;         // Scale, Host
               dwVEdda |= (srcWidth << 16);         // K1
               dwVEdda |= (srcWidth - width) & 0x000007FF; // K2
            }
            else
            {
               dwVEctrl= ~(srcWidth) & 0x00000FFF;
               dwVEdda |= SHRINK | SCREEN;         // Scale, Host
               dwVEdda |= (width << 16);        // K1
               dwVEdda |= (width - srcWidth) & 0x000007FF; // K2
            }

            // Y-SOFT DDA.

            if( height >= srcHeight )
            {
               YPos =   (LONG)(2 * (srcHeight - height));   // YPos
               YNeg =   (LONG)(2 * srcHeight);                 // YNeg
               YDDAAdj= (LONG)(srcHeight - height);         // fudge
               YErr =   (LONG)(2 * srcHeight - height + YDDAAdj); // YErr
            }
            else
            {
               YPos =   (LONG)(2 * (height - srcHeight));   // YPos
               YNeg =   (LONG)(2 * height);                 // YNeg
               YDDAAdj= (LONG)(height - srcHeight);         // fudge
               YErr =   (LONG)(2 * height - srcHeight + YDDAAdj); // YErr
            }

            // Set Color Formats.

            if (lpDPFsrc->dwFlags & DDPF_FOURCC)
            {
                bitsPerPixel = (BYTE) lpDPFsrc->dwYUVBitCount;
                srcSurfWidthBytes = srcPitch;

                switch (lpDPFsrc->dwFourCC)
                {
                        case FOURCC_YUY2:
                                dwVEctrl |= INPUT_YCrCb422 | CSCENABLE; //Not INPUT_YUV422!
                                //DPF("Src YUV");
                                break;
                        default:
                                goto BLT32_PF_UNSUPPORTED;
                }
            }
            else
            if (lpDPFsrc->dwFlags & DDPF_RGB)
            {
                bitsPerPixel = (BYTE) lpDPFsrc->dwRGBBitCount;
                srcSurfWidthBytes = destPitch;

                switch (bitsPerPixel)
                {
                        case  8:
                                dwVEctrl |= INPUT_RGB8;
                                //DPF("Src RGB8");
                                break;

                case 16:
                    if (IsRGB15(lpDPFsrc))
                        dwVEctrl |= INPUT_RGB15;
                    else
                        if (IsRGB16(lpDPFsrc))
                            dwVEctrl |= INPUT_RGB16;
                        else
                            goto BLT32_PF_UNSUPPORTED;
                                //DPF("Src RGB15/16");
                    break;

                case 32:
                    if (IsRGB32(lpDPFsrc))
                        dwVEctrl |= INPUT_RGB32;
                    else
                        goto BLT32_PF_UNSUPPORTED;
                                //DPF("Src RGB32");
                    break;
                default:
                    goto BLT32_PF_UNSUPPORTED;
                    break;
                }
            }

            src_bytes_per_pixel = bitsPerPixel>>3;

            GetValidDDPF(destSurfx, lpDPFdest);
            if (destSurfx->dwFlags & DDRAWISURF_HASPIXELFORMAT)
                bitsPerPixel = (BYTE)(destSurf->ddpfSurface.dwRGBBitCount);
            else
                bitsPerPixel = (BYTE)sData->ddpf.dwRGBBitCount;

            switch (bitsPerPixel)
            {
                case 8:
                        dwVEctrl |=OUTPUT_RGB8;
                        //DPF("dest RGB8");
                        break;

                case 16:
                        dwVEctrl |=OUTPUT_RGB16;
                        //DPF("dest RGB16");
                        break;

                case 32:
                        dwVEctrl |=OUTPUT_RGB32;
                        //DPF("dest RGB32");
                        break;
                default:
                        dwVEctrl |=OUTPUT_RGB8;
                        break;
            }

            // Set Filter Characteristics.

            if ( (dst_bytes_per_pixel > 1) || (lpDPFsrc->dwFlags & DDPF_FOURCC) )
            {
                dwVEctrl |= FILTERENABLE;
                if (width > (srcWidth << 1))
                        dwVEdda |= LINEAR12221 ; //linear, 1-2-2-2-1, >2X stretch.
                else
                if (width > (srcWidth))
                        dwVEdda |= LINEAR02420 ; //linear, 0-2-4-2-0, 1-2X stretch.
                else
                        dwVEdda |= BILINEAR    ; //bi-linear, 0 stretch.
            }

            // Set Step.

            if( sData->dwVideoMemorySize < (1024 * 1024) )
                dwVEstep = 0x00040004;      // Setup 4byte/write step.

            // Set Source Base Address.

            convertToGlobalCord(srcX,srcY,sourceOffset,destPitch,bytes_per_pixel);
            dwVEsrcAddr = (srcY * destPitch) +
                          (srcX * src_bytes_per_pixel);

            // Set Destination Base Address.

            dwVEdestAddr = (destY * sData->dwScreenWidthBytes) +
                          (destX * bytes_per_pixel);

            //DPF("From org(%d,%d) size(%d,%d)",srcX,srcY,srcWidth,srcHeight);
            //DPF("To org(%d,%d) size(%d,%d)",destX,destY,width,height);
            // SW WORK AROUND #2:if dest right boundary at 4byte boundry, increase crop.

            // Set Crop.

            dwVEcrop = width;

            if( (((dwVEcrop << (sData->ddpf.dwRGBBitCount >> 4)) +
                   dwVEdestAddr) & 0x07) == 4 )
                dwVEcrop++;

            // Output data to Pixel Formatter.

            wait_for_fifo( 1, sData );
            setRegister( CMD, 0x00 );
            sData->lpwait_for_idle( sData );

            wait_for_fifo( 4, sData );
            mmdwSetPFRegister( PF_CONTROL, dwVEctrl );
            mmdwSetPFRegister( PF_DDA,     dwVEdda  );
            mmdwSetPFRegister( PF_STEP,    dwVEstep );
            mmdwSetPFRegister( PF_CROP,    dwVEcrop );

            if( height >= srcHeight )
                for(i = height;i > 0; i--)
                {
                        wait_for_fifo( 3, sData );
                        mmdwSetPFRegister( PF_SRCADDR, dwVEsrcAddr );
                        mmdwSetPFRegister( PF_DSTADDR, dwVEdestAddr );
                        mmdwSetPFRegister( PF_NOP    , 0 );
                        wait_for_pf_idle();

                        dwVEdestAddr += sData->dwScreenWidthBytes;
                        dwVEsrcAddr += (DWORD)srcSurfWidthBytes;

                        if( YErr > 0 )
                                YErr += YPos;
                        else
                        {
                                YErr += YNeg;
                                dwVEsrcAddr -= (DWORD)srcSurfWidthBytes;
                        }
                }
            else
                for(i = height;i > 0; i--)
                {
                        wait_for_fifo( 3, sData );
                        mmdwSetPFRegister( PF_SRCADDR, dwVEsrcAddr );
                        mmdwSetPFRegister( PF_DSTADDR, dwVEdestAddr );
                        mmdwSetPFRegister( PF_NOP    , 0 );
                        wait_for_pf_idle();
                        dwVEdestAddr += sData->dwScreenWidthBytes;
PF1CPerformDDA:
                        dwVEsrcAddr += (DWORD)srcSurfWidthBytes;

                        if( YErr < 0 )
                        {
                                YErr += YNeg;
                                goto PF1CPerformDDA;
                        }
                        YErr += YPos;
                }

        if ( width < destFinalWidth)
        {
            srcWidth=width;
            srcY=destY;
            srcX=destX;
            srcHeight=height;
            destOffset=sourceOffset=0;  //(X,Y) are already global at this point
        //By doing any color space conversion on the 1st pass, the pixel
        //format should be the same for both the source and destination for
        //the 2nd pass.
            lpDPFsrc = lpDPFdest;
            //DPF("Start Pass2");
            goto    PFStretch;
        }

            lpBlt->ddRVal = DD_OK;
            return DDHAL_DRIVER_HANDLED;

BLT32_PF_UNSUPPORTED:
            lpBlt->ddRVal = DDERR_UNSUPPORTED;
            return DDHAL_DRIVER_HANDLED;
         }

BLT32_SRCCOPY:

    convertToGlobalCord(srcX,srcY,sourceOffset,destPitch,bytes_per_pixel);
    convertToGlobalCord(destX,destY,destOffset,destPitch,bytes_per_pixel);

    if( dwFlags & DDBLT_KEYSRCOVERRIDE )
    {
//      DPF("Transparent blt color key = 0x%04x",(WORD)lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue);
//      DPF("From org = 0x%08lx x= %d y = %d",sourceOffset,srcX,srcY);
//      DPF("From org = 0x%08lx x= %d y = %d",destOffset,destX,destY);
//      DPF("width = %d height = %d",width,height);

        wait_for_fifo( 13, sData );
        setRegister( MULTILFUNCTION_CONTROL, ENABLE_COLOR_COMPARE );
       setRegister( COLOR_CMP,(WORD)lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue );
        if (bytes_per_pixel > 2)
          setRegister( COLOR_CMP,(WORD)(lpBlt->bltFX.ddckSrcColorkey.dwColorSpaceLowValue >> 16) );
        setRegister( PIXEL_CNTL,FORGROUND_SELECTED);
        setRegister( FRGD_MIX,SCR_COPY_MIX );
        setRegister( CUR_X,srcX );
        setRegister( CUR_Y,srcY );
        setRegister( DESTX_DIASTP, destX );
        setRegister( DESTY_AXSTP, destY );
        setRegister( MAJ_AXIS_PCNT, (WORD)(width-1) );
        setRegister( MIN_AXIS_PCNT, (WORD)(height-1) );
        setRegister( CMD, (WORD)(SCRN_SCRN_BLT | direction) );
        setRegister( FRGD_MIX, 0x27 );
        setRegister( MULTILFUNCTION_CONTROL, (WORD)sData->dwMULT_MISC); // Clear RSF bit

// in order to correctly disable color compare for next blt,
// we need to issue a bogus blt command to flush out the fifo.

#if 0
        wait_for_fifo( 10, sData );
        setRegister( PIXEL_CNTL, FORGROUND_SELECTED );
        setRegister( FRGD_MIX,SCR_COPY_MIX );
        setRegister( CUR_X, 0 );
        setRegister( CUR_Y, 0 );
        setRegister( DESTX_DIASTP, 0 );
        setRegister( DESTY_AXSTP, 0 );
        setRegister( MAJ_AXIS_PCNT, 0 );
        setRegister( MIN_AXIS_PCNT, 0 );
        setRegister( CMD, (WORD)(SCRN_SCRN_BLT | direction) );
        setRegister( FRGD_MIX, 0x27 );
#endif

    }
    else
    {
//      DPF("Src Scr blt");
//      DPF("From org = 0x%08lx x= %d y = %d",sourceOffset,srcX,srcY);
//      DPF("From org = 0x%08lx x= %d y = %d",destOffset,destX,destY);
//      DPF("width = %d height = %d",width,height);

        if (fudge !=0)
            width *= fudge; // K L U D G E !!!!!
        if (fudge2 !=0)
            width /= fudge2; // K L U D G E !!!!!

        wait_for_fifo( 8, sData );
        setRegister( PIXEL_CNTL,FORGROUND_SELECTED);
        setRegister( FRGD_MIX,SCR_COPY_MIX );
        setRegister( CUR_X,srcX );
        setRegister( CUR_Y,srcY );
        setRegister( DESTX_DIASTP, destX );
        setRegister( DESTY_AXSTP, destY );
        setRegister( MAJ_AXIS_PCNT, (WORD)(width - 1) );
        setRegister( MIN_AXIS_PCNT, (WORD)(height - 1) );
        wait_for_fifo( 2, sData );
        setRegister( CMD, (WORD)(SCRN_SCRN_BLT | direction) );
        setRegister( FRGD_MIX, 0x27 );
        /*
         * 911/924 is brain-dead for bltting on 16bpp!
         */
        if( (bytes_per_pixel == 2) &&
            (sData->wChipId == S3911) )
        {
            wait_for_fifo( 5, sData );
            setRegister( CUR_X,(WORD)(srcX+1024) );
            setRegister( CUR_Y,srcY );

            setRegister( DESTX_DIASTP, (WORD)(destX+1024) );
            setRegister( DESTY_AXSTP, destY );
            setRegister( CMD, (WORD)(SCRN_SCRN_BLT | direction) );
        }
    }
      }
    }
    else
    {
    return DDHAL_DRIVER_NOTHANDLED;
    }
    lpBlt->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

} /* Blt32_968 */

/*---------------------------------------------------------------------------
 *
 * Flip32
 *
 */
DWORD __stdcall Flip32( LPDDHAL_FLIPDATA lpFlipData)
{
    HRESULT ddrval;
    BYTE bVideoPanning;
    DWORD dwTmp, dwTmp2;

    union LongWord videoMemOffset;
     union IndexReg lowAddress  =  {0x0d};
     union IndexReg middleAddress =  {0x0c};
     union IndexReg highAddress =  {0x69};
     union IndexReg ext31Address    =  {0x31};
     union IndexReg ext51Address    =  {0x51};
    LPS3THKINFO sData = GetMyRefData(lpFlipData->lpDD);

//  DPF("in Flip32");
    /*
     * is the current flip still in progress?
     *
     * don't want a flip to work until after the last flip is done,
     * so we ask for the general flip status and ignore the vmem
     */

    ddrval = updateFlipStatus( 0, sData );
    if( (ddrval != DD_OK) || (sData->lpdraw_engine_busy(sData)) )
    {
        lpFlipData->ddRVal = DDERR_WASSTILLDRAWING;
        return DDHAL_DRIVER_HANDLED;
    }

    /*
     * do the flip
     */

    sData->lpS3MobileData->dwMobileCaps |= IN_FLIPPING;
#if 1   //disable DuoView Flip for now
        if ( !(sData->bFamilyFlags & MOBILE_FAMILY) &&
              (sData->lpS3MobileData->wSPStyle & SP_M5) ){
        BYTE bCurrentDisplay;
        DWORD dwTmp;

      lpMemRegB[0x83d4]=0x6B;
      bCurrentDisplay = lpMemRegB[0x83d5];
        if (bCurrentDisplay & DUOVIEW){
            check_DMA(sData);
            videoMemOffset.dword = lpFlipData->lpSurfTarg->lpGbl->fpVidMem - sData->dwScreenFlatAddr;

            if (Streams_Processor_On(sData))
            {
                if( (lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                (lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_VISIBLE) )
                {
                    //if flip and pan on same IGA then do panning adjustment
                    if ((sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED) &&
                         (((sData->lpS3MobileData->dwMobileCaps & (PANNING_IGA1 | SP_IGA1)) == (PANNING_IGA1 | SP_IGA1)) ||
                          ((sData->lpS3MobileData->dwMobileCaps & (PANNING_IGA1 | SP_IGA1)) == 0)
                         )
                        )
                    {
                    *sData->lpSSAddr      = videoMemOffset.dword;
                    videoMemOffset.dword += sData->dwUpdateOverlayFlipOffset +
                        *sData->lpClipOffset;
                    }
                    else
                    {
                    videoMemOffset.dword += sData->dwUpdateOverlayFlipOffset;
                    }
                    mmdwSetRegister(S_0, videoMemOffset.dword);
                }
            }
            else //the stream processor is not on
            {
                //flip IGA1 first
//### For now, the good WaitForVSync test combination is:
//### IGA1 use 3DA and IGA2 use CR33
                dwTmp = videoMemOffset.dword;

//### Take care of BOTH IGA's panning case seperately
                if (sData->lpS3MobileData->dwMobileCaps & PANNING_IGA1_IGA2)
                     {
                     *sData->lpFlipOffset = videoMemOffset.dword;
                     dwTmp += *sData->lpPSOffset;
                     lpMemReg[0x83d4/2]=(WORD) (0xd | ((dwTmp>>2)&0xff)<<8);
                     lpMemReg[0x83d4/2]=(WORD) (0xc | (dwTmp>>2)&0xff00);
                     lpMemReg[0x83d4/2]=(WORD) (0x69 | ((dwTmp>>2)&0x0f0000)>>8);

                     videoMemOffset.dword+=*sData->lpPS2Offset;
                     lpMemRegB[0x83c4] = 0x26;
                     lpMemRegB[0x83c5] = 0x4F;
                     WAIT_FOR_DISP_M5
                     lpMemReg[0x83d4/2]=(WORD) (0xd | ((videoMemOffset.dword>>2)&0xff)<<8);
                     lpMemReg[0x83d4/2]=(WORD) (0xc | (videoMemOffset.dword>>2)&0xff00);
                     lpMemReg[0x83d4/2]=(WORD) (0x69 | ((videoMemOffset.dword>>2)&0x0f0000)>>8);
                     WAIT_FOR_VBLANK_M5
                     lpMemRegB[0x83c4] = 0x26;
                     lpMemRegB[0x83c5] = 0x40;

                     goto flip32_done;
                     }

                if (sData->lpS3MobileData->dwMobileCaps & PANNING_IGA1)
                {
                *sData->lpFlipOffset = videoMemOffset.dword;
                dwTmp += *sData->lpPSOffset;
                }
                 lpMemReg[0x83d4/2]=(WORD) (0xd | ((dwTmp>>2)&0xff)<<8);
                lpMemReg[0x83d4/2]=(WORD) (0xc | (dwTmp>>2)&0xff00);
                lpMemReg[0x83d4/2]=(WORD) (0x69 | ((dwTmp>>2)&0x0f0000)>>8);

//### For IGA2 flipping, we need to do thing a little different, especially
//### in DOUBLE buffer case:
//### 1. Do the CR's programming during display period. Since they won't get
//###    loaded until VSync, it doesn't hurt.  If we do it during VSync, we
//###    cannot be sure whether the loading stage has passed or not.
//### 2. Wait until IGA2 Vsync happen before exit.  If we didn't do this,
//###    the apps may start updating the buffer we're displaying right now
//###    before flipping takes place, then tearing appears.

                if ((sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED)
                    && !(sData->lpS3MobileData->dwMobileCaps & PANNING_IGA1))
                {
                *sData->lpFlipOffset = videoMemOffset.dword;
                videoMemOffset.dword+=*sData->lpPSOffset;
                }
                lpMemRegB[0x83c4] = 0x26;
                lpMemRegB[0x83c5] = 0x4F;
                WAIT_FOR_DISP_M5
                lpMemReg[0x83d4/2]=(WORD) (0xd | ((videoMemOffset.dword>>2)&0xff)<<8);
                lpMemReg[0x83d4/2]=(WORD) (0xc | (videoMemOffset.dword>>2)&0xff00);
                lpMemReg[0x83d4/2]=(WORD) (0x69 | ((videoMemOffset.dword>>2)&0x0f0000)>>8);
                WAIT_FOR_VBLANK_M5
                lpMemRegB[0x83c4] = 0x26;
                lpMemRegB[0x83c5] = 0x40;
            }
            goto flip32_done;
        }
     }
#endif

    /*
     * Enter this case if
     *     1/ stream processor is supported and
     *     2/ if on-board chip is a mobile product, flipping of primary
     *        surface must be done in enhanced mode (Cr67[3,2] = 0xc)
    */
    if( STREAMS_PROCESSOR_PRESENT && VIRGE_FAMILY_PRESENT)
    {
    check_DMA(sData);
    videoMemOffset.dword =
    lpFlipData->lpSurfTarg->lpGbl->fpVidMem - sData->dwScreenFlatAddr;
    if ( ( !IS_DISPLAY_ENABLE ) & sData->bWaitForVSync )
    {
        sData->dwFlipFail++;
        lpFlipData->ddRVal = DDERR_WASSTILLDRAWING;
        return DDHAL_DRIVER_HANDLED;
    }
    if ( Streams_Processor_On(sData) )
    //IF_STREAMS_PROCESSOR_ON
    {
        if(lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
        {
            if (sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED)
               videoMemOffset.dword+=*sData->lpPSOffset;

            mmdwSetRegister(P_0, videoMemOffset.dword);
        }
        else
        if( (lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
            (lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_VISIBLE) )
        {
            if  ( sData->lpS3MobileData->wSPStyle & SP_M5 )
        {
            dwTmp  = dwTmp2 = sData->lpS3MobileData->dwMobileCaps;
            dwTmp2 &=  (SP_IGA1 | PANNING_IGA1);
            if ( (dwTmp & PANNING_IGA1_IGA2) ||
             !dwTmp2             ||
             (dwTmp2 == (PANNING_IGA1 | SP_IGA1)) )
            {
                bVideoPanning = 1;
            }
            else
            {
                bVideoPanning = 0;
            }
        }
        else
        {
            bVideoPanning = 1;
        }

            if ((sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED) &&
        (bVideoPanning))
            {
                *sData->lpSSAddr      = videoMemOffset.dword;
                videoMemOffset.dword += sData->dwUpdateOverlayFlipOffset +
                                        *sData->lpClipOffset;
        }
            else
            {
                videoMemOffset.dword += sData->dwUpdateOverlayFlipOffset;
            }

            mmdwSetRegister(S_0, videoMemOffset.dword);
    }
    }
    else //the stream processor is not on
    {
        if ((sData->lpS3MobileData->dwMobileCaps & PANNING_IGA1) ||
                        (sData->lpS3MobileData->dwMobileCaps & PANNING_IGA1_IGA2))
//        if (sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED)
        {
            *sData->lpFlipOffset = videoMemOffset.dword;
            videoMemOffset.dword+=*sData->lpPSOffset;
        }

        lpMemReg[0x83d4/2]=(WORD) (0xd | ((videoMemOffset.dword>>2)&0xff)<<8);
        lpMemReg[0x83d4/2]=(WORD) (0xc | (videoMemOffset.dword>>2)&0xff00);
        lpMemReg[0x83d4/2]=(WORD) (0x69 | ((videoMemOffset.dword>>2)&0x0f0000)>>8);
    }
    }
//TRIO and Mobile go thru here
    else if( (sData->dwSpecialHW & STREAMS_PROCESSOR ) &&
    (((sData->lpS3MobileData->dwMobileCaps & EXPANSION_ENABLED) == 0 ) ||
    (!(lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE))) )
    {
    videoMemOffset.dword = lpFlipData->lpSurfTarg->lpGbl->fpVidMem - sData->dwScreenFlatAddr;
    if ( sData->bWaitForVSync & (!IS_DISPLAY_ENABLE) )
    {
        sData->dwFlipFail++;
        lpFlipData->ddRVal = DDERR_WASSTILLDRAWING;
        return DDHAL_DRIVER_HANDLED;
    }
    if( Streams_Processor_On(sData))
    //IF_STREAMS_PROCESSOR_ON
    {
        if(lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
        {
            if (sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED)
            {
                *sData->lpPSAddr  = videoMemOffset.dword;
                videoMemOffset.dword+=*sData->lpPSOffset;
            }
            mmdwSetRegister(P_0, videoMemOffset.dword);
        }
        else
        if( (lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
        (lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_VISIBLE) )
        {
        if (sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED)
        {
            *sData->lpSSAddr      = videoMemOffset.dword;
            videoMemOffset.dword += sData->dwUpdateOverlayFlipOffset +
                        *sData->lpClipOffset;
        }
        else
        {
            videoMemOffset.dword += sData->dwUpdateOverlayFlipOffset;
        }
        mmdwSetRegister(S_0, videoMemOffset.dword);
        }
    }
    }
    else if( (sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED) &&
    (lpFlipData->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) )
    {
          videoMemOffset.dword = lpFlipData->lpSurfTarg->lpGbl->fpVidMem - sData->dwScreenFlatAddr;
        *sData->lpFlipOffset = videoMemOffset.dword;
        videoMemOffset.dword+=*sData->lpPSOffset;

        lpMemReg[0x83d4/2]=(WORD) (0xd | ((videoMemOffset.dword>>2)&0xff)<<8);
        lpMemReg[0x83d4/2]=(WORD) (0xc | (videoMemOffset.dword>>2)&0xff00);
        lpMemReg[0x83d4/2]=(WORD) (0x69 | ((videoMemOffset.dword>>2)&0x0f0000)>>8);
     }
    else
    {
        videoMemOffset.dword      = (lpFlipData->lpSurfTarg->lpGbl->fpVidMem - sData->dwScreenFlatAddr) >> 2;
        //DPF("videoMemOffset.dword=0x%08lx",videoMemOffset.dword);
        lowAddress.byte.value     = videoMemOffset.byte.low;
        middleAddress.byte.value  = videoMemOffset.byte.middle;
        if( (sData->wChipId == S3911) ||    // 911 == 924
            (sData->wChipId == S3928) ||
            (sData->wChipId == S3928PCI) ||
            (sData->wChipId == S3801) )   // 801 == 805
        {
            // DPF("Flip Old way");
            outp(CRTC_REG, ext31Address.byte.index);
            ext31Address.byte.value = ((videoMemOffset.byte.high & 0x03) << 4) | (inp(CRTC_REG+1) & 0xcf);
            outp(CRTC_REG, ext51Address.byte.index);
            ext51Address.byte.value = ((videoMemOffset.byte.high & 0x0c) >> 2) | (inp(CRTC_REG+1) & 0xfc);

            if ( !IS_DISPLAY_ENABLE & sData->bWaitForVSync )
            {
                // DPF("In flip Was still drawing");
                lpFlipData->ddRVal = DDERR_WASSTILLDRAWING;
                return DDHAL_DRIVER_HANDLED;
            }
            outpw(CRTC_REG,lowAddress.word);
            outpw(CRTC_REG,middleAddress.word);
            outpw(CRTC_REG,ext31Address.word);
            outpw(CRTC_REG,ext51Address.word);
        }
        else // Trio64V+/V2 go through here.
        {
        lpMemRegB[0x83d4]=highAddress.byte.index;
        highAddress.byte.value  = videoMemOffset.byte.high | (lpMemRegB[0x83d5] & 0xf0);
        // ACKACK: seems like doing this is just going to up the odds that
        // we can't flip and will burn more cycles.    does just waiting
        // make more sense?  craige 7/30/95
        //
        #if 1
        if ( !IS_DISPLAY_ENABLE & sData->bWaitForVSync )
        {
            sData->dwFlipFail++;
            lpFlipData->ddRVal = DDERR_WASSTILLDRAWING;
            return DDHAL_DRIVER_HANDLED;
        }
        #else
            while( !IS_DISPLAY_ENABLE & bWaitForVSync );
        #endif
        //DPF("videoMemOffset.dword=%08lx",videoMemOffset.dword);
        if ((sData->lpS3MobileData->dwMobileCaps & MOBILE_FAMILY) &&
            (sData->lpS3MobileData->dwMobileCaps & DUOVIEW_SAMEIMAGE) )
        {

            if (sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED)
            {
                lpMemRegB[0x83c4] = 0x26;
                lpMemRegB[0x83c5] = 0x40;
                if  ((sData->lpS3MobileData->bCurrentDisplay & (CRT_ONLY + FP_ONLY))
                    != (CRT_ONLY + FP_ONLY))   // CRT only or FP only
                {
                    lowAddress.byte.value += (BYTE)(*sData->lpPSOffset & 0x0FF);
                    middleAddress.byte.value += (BYTE)((*sData->lpPSOffset >> 8) & 0x0FF);
                    highAddress.byte.value += (BYTE)((*sData->lpPSOffset >> 16) & 0x0F);
                    lpMemReg[0x83d4/2]=lowAddress.word;             //IGA1
                    lpMemReg[0x83d4/2]=middleAddress.word;
                    lpMemReg[0x83d4/2]=highAddress.word;
                }
                else
                {
                    if ((sData->lpS3DriverData->wPanelResolution <
                         sData->lpS3DriverData->wLogResolution))
                    {
                        lowAddress.byte.value += (BYTE)(*sData->lpPSOffset & 0x0FF);
                        middleAddress.byte.value += (BYTE)((*sData->lpPSOffset >> 8) & 0x0FF);
                        highAddress.byte.value += (BYTE)((*sData->lpPSOffset >> 16) & 0x0F);
                    }
                    lpMemReg[0x83d4/2]=lowAddress.word;             //IGA1
                    lpMemReg[0x83d4/2]=middleAddress.word;
                    lpMemReg[0x83d4/2]=highAddress.word;


                     lowAddress.byte.value    = videoMemOffset.byte.low;
                     middleAddress.byte.value  = videoMemOffset.byte.middle;
                     outp(CRTC_REG, highAddress.byte.index);
                     highAddress.byte.value = videoMemOffset.byte.high | (inp(CRTC_REG+1) & 0xf0);

                    if ((sData->lpS3DriverData->wPhysResolution <
                         sData->lpS3DriverData->wLogResolution))
                    {
                        lowAddress.byte.value += (BYTE)(*sData->lpPSOffset & 0x0FF);
                        middleAddress.byte.value += (BYTE)((*sData->lpPSOffset >> 8) & 0x0FF);
                        highAddress.byte.value += (BYTE)((*sData->lpPSOffset >> 16) & 0x0F);
                    }
                    lpMemRegB[0x83c4] = 0x26;
                    lpMemRegB[0x83c5] = 0x4F;
                    lpMemReg[0x83d4/2]=lowAddress.word;             //IGA1
                    lpMemReg[0x83d4/2]=middleAddress.word;
                    lpMemReg[0x83d4/2]=highAddress.word;
                    WAIT_FOR_VBLANK;
                }
                *sData->lpPSAddr = videoMemOffset.dword;
            }
            else   // NOT Panning
            {
                lpMemRegB[0x83c4] = 0x26;
                lpMemRegB[0x83c5] = 0x44;
                lpMemReg[0x83d4/2]=lowAddress.word;
                lpMemReg[0x83d4/2]=middleAddress.word;
                lpMemReg[0x83d4/2]=highAddress.word;
                lpMemRegB[0x83c4] = 0x26;
                lpMemRegB[0x83c5] = 0x40;
            }
        }
        else // NOT Mobile
            {
                lpMemReg[0x83d4/2]=lowAddress.word;
                lpMemReg[0x83d4/2]=middleAddress.word;
                lpMemReg[0x83d4/2]=highAddress.word;
            }
        } // end Trio and Mobile NON Streams.
    } // end Trio and Mobile Flip.

#if 1   //disable DuoView Flip for now
flip32_done:
#endif
    /*
     * remember were/when we were when we did the flip
     */
    sData->lpS3MobileData->dwMobileCaps &= ~IN_FLIPPING;
    QueryPerformanceCounter((LARGE_INTEGER *) &sData->flipRecord.liFlipTime);
    sData->flipRecord.bFlipFlag     = TRUE;
    sData->flipRecord.bHaveEverCrossVBlank = FALSE;
    sData->flipRecord.bWasEverInDisplay    = FALSE;
    sData->flipRecord.fpFlipFrom        = lpFlipData->lpSurfCurr->lpGbl->fpVidMem;

    if ( sData->dwSpecialHW & READ_SCANLINE )
    {
    if( IN_VBLANK )
    {
        sData->flipRecord.dwFlipScanLine = 0;
        sData->flipRecord.bWasEverInDisplay = FALSE;
    }
    else
    {
        sData->flipRecord.dwFlipScanLine = CURRENT_VLINE;
        sData->flipRecord.bWasEverInDisplay = TRUE;
    }
    }


    lpFlipData->ddRVal = DD_OK;
    if (lpFlipData->lpSurfTarg->lpGbl->dwReserved1 == 0)
    lpFlipData->lpSurfTarg->lpGbl->dwReserved1=(DWORD)sData->lpSpod;
    return DDHAL_DRIVER_HANDLED;

} /* Flip32 */

/*---------------------------------------------------------------------------
 *
 * WaitForVerticalBlank32
 *
 */
DWORD __stdcall WaitForVerticalBlank32(
    LPDDHAL_WAITFORVERTICALBLANKDATA lpWaitForVerticalBlank )
{
    LPS3THKINFO sData = GetMyRefData(lpWaitForVerticalBlank->lpDD);

//  DPF("in WaitForVerticalBlank32");
    if (sData->lpS3MobileData->wSPStyle & SP_M5 )
    {
    lpMemReg[0x83c4/2] = (WORD)0x4026;
    }

    switch( lpWaitForVerticalBlank->dwFlags )
    {
    case DDWAITVB_I_TESTVB:
    /*
     * if testvb is just a request for the current vertical blank status
     */
    lpWaitForVerticalBlank->ddRVal = DD_OK;
    lpWaitForVerticalBlank->bIsInVB = IN_VBLANK;
    return DDHAL_DRIVER_HANDLED;

    case DDWAITVB_BLOCKBEGIN:
    /*
     * if blockbegin is requested we wait until the vertical blank
     * is over, and then wait for the display period to end.
     */
    while(IN_VBLANK);
    while(!IN_VBLANK);
    lpWaitForVerticalBlank->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

    case DDWAITVB_BLOCKEND:
    /*
     * if blockend is requested we wait for the vblank interval to end.
     */
    if( IN_VBLANK )
    {
        while( IN_VBLANK );
    }
    else
    {
        while(!IN_VBLANK);
        while(IN_VBLANK);
    }
    lpWaitForVerticalBlank->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
    }

    return DDHAL_DRIVER_NOTHANDLED;

} /* WaitForVerticalBlank32 */

/*---------------------------------------------------------------------------
 *
 * GetScanLine32
 *
 */
DWORD __stdcall GetScanLine32(
    LPDDHAL_GETSCANLINEDATA lpGetScanLine )
{
    LPS3THKINFO sData = GetMyRefData(lpGetScanLine->lpDD);
    //DPF("in GetScanLine32");
    /*
     * If a vertical blank is in progress the scan line is in
     * indeterminant. If the scan line is indeterminant we return
     * the error code DDERR_VERTICALBLANKINPROGRESS.
     * Otherwise we return the scan line and a success code
     */
    if( IN_VBLANK )
    {
    lpGetScanLine->ddRVal = DDERR_VERTICALBLANKINPROGRESS;
    }
    else
    {
    if ( sData->dwSpecialHW & READ_SCANLINE )
    {
        lpGetScanLine->dwScanLine = CURRENT_VLINE;
    }
    else
    {
        lpGetScanLine->dwScanLine = 0;      // zero 'cause we don't know!
    }
    lpGetScanLine->ddRVal = DD_OK;
    }
    return DDHAL_DRIVER_HANDLED;

} /* GetScanLine32 */

/*---------------------------------------------------------------------------
 *
 * GetBltStatus32
 *
 * Doesn't currently really give a damn what surface is specified, just
 * checks and goes
 *
 */
DWORD __stdcall GetBltStatus32(
    LPDDHAL_GETBLTSTATUSDATA lpGetBltStatus )
{
    LPS3THKINFO sData = GetMyRefData(lpGetBltStatus->lpDD);

//  DPF("in GetBltStatus32");
    /*
     * CANBLT: can we add a blt?
     */

    if( lpGetBltStatus->dwFlags == DDGBS_CANBLT )
    {
    lpGetBltStatus->ddRVal = updateFlipStatus(
            lpGetBltStatus->lpDDSurface->lpGbl->fpVidMem, sData );
    if( lpGetBltStatus->ddRVal == DD_OK )
    {
        /*
         * so there was no flip going on, is there room in the fifo
         * to add a blt?
         */
        if( sData->lproom_for_blt(sData) )
        {
        lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
        }
        else
        {
        lpGetBltStatus->ddRVal = DD_OK;
        }
    }
    }
    /*
     * DONEBLT: is a blt in progress?
     */
    else
    {
    if( sData->lpdraw_engine_busy(sData) )
    {
        lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
    }
    else
    {
        lpGetBltStatus->ddRVal = DD_OK;
    }
    }
    return DDHAL_DRIVER_HANDLED;

} /* GetBltStatus32 */

/*---------------------------------------------------------------------------
 *
 * GetFlipStatus32
 *
 * If the display has went through one refresh cycle since the flip
 * occurred we return DD_OK.  If it has not went through one refresh
 * cycle we return DDERR_WASSTILLDRAWING to indicate that this surface
 * is still busy "drawing" the flipped page.   We also return
 * DDERR_WASSTILLDRAWING if the bltter is busy and the caller wanted
 * to know if they could flip yet
 *
 */
DWORD __stdcall GetFlipStatus32(
    LPDDHAL_GETFLIPSTATUSDATA lpGetFlipStatus )
{
    LPS3THKINFO sData = GetMyRefData(lpGetFlipStatus->lpDD);

//  DPF("in GetFlipStatus32");
    /*
     * don't want a flip to work until after the last flip is done,
     * so we ask for the general flip status and ignore the vmem
     */
    lpGetFlipStatus->ddRVal = updateFlipStatus( 0, sData );

    /*
     * check if the bltter busy if someone wants to know if they can flip
     */
    if( lpGetFlipStatus->dwFlags == DDGFS_CANFLIP )
    {
    if( (lpGetFlipStatus->ddRVal == DD_OK) && sData->lpdraw_engine_busy(sData) )
    {
//      DPF("Set was still drawing");
        lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;
    }
    }
    return DDHAL_DRIVER_HANDLED;

} /* GetFlipStatus32 */

/*---------------------------------------------------------------------------
 *
 * Lock32
 *
 */
DWORD __stdcall Lock32( LPDDHAL_LOCKDATA lpLockData )
{
    HRESULT ddrval;
    LPS3THKINFO sData = GetMyRefData(lpLockData->lpDD);

//  DPF("in Lock32");
#ifndef S3VIRGE
    if( sData->lpLinearCurrentChipMode == NULL )
        sData->lpLinearCurrentChipMode = (WORD *) sData->lpCurrentChipMode;

    /*
     * check to see if any pending physical flip has occurred
     * don't allow a lock if a blt is in progress
     */
   END_ACCESS;
#endif

    ddrval = updateFlipStatus( lpLockData->lpDDSurface->lpGbl->fpVidMem, sData );
    if( (ddrval != DD_OK) || (sData->lpdraw_engine_busy(sData)) )
    {
    lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
    return DDHAL_DRIVER_HANDLED;
    }
#ifndef S3VIRGE
   BEGIN_ACCESS;
#endif

   return DDHAL_DRIVER_NOTHANDLED;

} /* Lock32 */

/*---------------------------------------------------------------------------
 *
 * UnLock32
 *
 */
DWORD __stdcall UnLock32( LPDDHAL_UNLOCKDATA lpLockData )
{
    LPS3THKINFO sData = GetMyRefData(lpLockData->lpDD);
//  DPF("in UnLock32");

    LPDDRAWI_DDRAWSURFACE_LCL lpSrc;
    WORD srcWidth, srcHeight, i;
    LONG dwStride;
    DWORD SrcBase, DstBase,  dwStart, bytes_per_pixel;

    lpSrc = lpLockData->lpDDSurface;

    if ( (sData->lpS3MobileData->dwMobileCaps & UNDERSCAN_ENABLED) &&
         (sData->lpVisibleOverlay == lpSrc ) )
    {
        dwStride =  lpSrc->lpGbl->lPitch;
        srcWidth = lpSrc->lpGbl->wWidth;
        srcHeight = lpSrc->lpGbl->wHeight;
        bytes_per_pixel = dwStride/srcWidth;

        vir_wait_for_fifo( 7 );

        dwStart = lpSrc->lpGbl->fpVidMem - sData->dwScreenFlatAddr;


        if (sData->lpS3MobileData->dwMobileCaps & TV_PAL)
        {
        for (i=0; i<srcHeight/15; i++)
        {

            SrcBase = dwStart + dwStride*i*15;
            DstBase = dwStart + dwStride*i*14 + dwStride*srcHeight;

            mmvirSetRegister(VI_SRC_BASE, SrcBase & 0xfffffff8);     // setup source base address
            mmvirSetRegister(VI_DEST_SRC_STR, ((DWORD)(dwStride & 0xFFF8) << 16) | (DWORD)(dwStride & 0xFFF8));
            mmvirSetRegister(VI_DEST_BASE, DstBase & 0xfffffff8);      // setup dest. base address
            mmvirSetRegister(VI_RWIDTH_HEIGHT, ((DWORD)(srcWidth-1) << 16) | (DWORD)14);

            mmvirSetRegister(VI_RSRC_XY, 0);
            mmvirSetRegister(VI_RDEST_XY, 0);

            mmvirSetRegister(VI_2D_CMD_SET, VI_CMD_BITBLT |
                    (0xCCL << 17) |
                    (1L << 25) |         // left to right
                    (1L << 26) |         // top to bottom
                    VI_2DCMD_DE |
                    ((DWORD)(bytes_per_pixel-1) << 2));
        }
        if (srcHeight%15)
        {
            SrcBase = dwStart + dwStride*(srcHeight/15)*15;
            DstBase = dwStart + dwStride*(srcHeight/15)*14 + dwStride*srcHeight;

            mmvirSetRegister(VI_SRC_BASE, SrcBase & 0xfffffff8);     // setup source base address
            mmvirSetRegister(VI_DEST_SRC_STR, ((DWORD)(dwStride & 0xFFF8) << 16) | (DWORD)(dwStride & 0xFFF8));
            mmvirSetRegister(VI_DEST_BASE, DstBase & 0xfffffff8);      // setup dest. base address
            mmvirSetRegister(VI_RWIDTH_HEIGHT, ((DWORD)(srcWidth-1) << 16) | (DWORD)(srcHeight%15));

            mmvirSetRegister(VI_RSRC_XY, 0);
            mmvirSetRegister(VI_RDEST_XY, 0);

            mmvirSetRegister(VI_2D_CMD_SET, VI_CMD_BITBLT |
                    (0xCCL << 17) |
                    (1L << 25) |         // left to right
                    (1L << 26) |         // top to bottom
                    VI_2DCMD_DE |
                    ((DWORD)(bytes_per_pixel-1) << 2));
        }
        }
        else
        {
        for (i=0; i<srcHeight/6; i++)
        {

            SrcBase = dwStart + dwStride*i*6;
            DstBase = dwStart + dwStride*i*5 + dwStride*srcHeight;

            mmvirSetRegister(VI_SRC_BASE, SrcBase & 0xfffffff8);     // setup source base address
            mmvirSetRegister(VI_DEST_SRC_STR, ((DWORD)(dwStride & 0xFFF8) << 16) | (DWORD)(dwStride & 0xFFF8));
            mmvirSetRegister(VI_DEST_BASE, DstBase & 0xfffffff8);      // setup dest. base address
            mmvirSetRegister(VI_RWIDTH_HEIGHT, ((DWORD)(srcWidth-1) << 16) | (DWORD)5);

            mmvirSetRegister(VI_RSRC_XY, 0);
            mmvirSetRegister(VI_RDEST_XY, 0);

            mmvirSetRegister(VI_2D_CMD_SET, VI_CMD_BITBLT |
                    (0xCCL << 17) |
                    (1L << 25) |         // left to right
                    (1L << 26) |         // top to bottom
                    VI_2DCMD_DE |
                    ((DWORD)(bytes_per_pixel-1) << 2));
        }
        if (srcHeight%6)
        {
            SrcBase = dwStart + dwStride*(srcHeight/6)*6;
            DstBase = dwStart + dwStride*(srcHeight/6)*5 + dwStride*srcHeight;

            mmvirSetRegister(VI_SRC_BASE, SrcBase & 0xfffffff8);     // setup source base address
            mmvirSetRegister(VI_DEST_SRC_STR, ((DWORD)(dwStride & 0xFFF8) << 16) | (DWORD)(dwStride & 0xFFF8));
            mmvirSetRegister(VI_DEST_BASE, DstBase & 0xfffffff8);      // setup dest. base address
            mmvirSetRegister(VI_RWIDTH_HEIGHT, ((DWORD)(srcWidth-1) << 16) | (DWORD)(srcHeight%6));

            mmvirSetRegister(VI_RSRC_XY, 0);
            mmvirSetRegister(VI_RDEST_XY, 0);

            mmvirSetRegister(VI_2D_CMD_SET, VI_CMD_BITBLT |
                    (0xCCL << 17) |
                    (1L << 25) |         // left to right
                    (1L << 26) |         // top to bottom
                    VI_2DCMD_DE |
                    ((DWORD)(bytes_per_pixel-1) << 2));
        }


        }
       }

#ifndef S3VIRGE
   END_ACCESS;
#endif
   return DDHAL_DRIVER_NOTHANDLED;

} /* UnLock32 */

/*---------------------------------------------------------------------------
 *
 * CanCreateSurface32
 *
 */
DWORD __stdcall CanCreateSurface32(LPDDHAL_CANCREATESURFACEDATA pccsd)
{
    LPDDSURFACEDESC lpDDSurfaceDesc;
    LPDDPIXELFORMAT lpDPF;
    DWORD caps, flags;
    LPS3THKINFO sData = GetMyRefData(pccsd->lpDD);

//  DPF("in CanCreateSurface32");
    // check pixel formats

    if( !pccsd->bIsDifferentPixelFormat )
    {
        if (sData->wDeviceId == D_S3VIRGE && sData->ddpf.dwRGBBitCount==24
            && (pccsd->lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OVERLAY)) {
            DPF("pccsd->lpDDSurfaceDesc->ddsCaps.dwCaps=%08lx",pccsd->lpDDSurfaceDesc->ddsCaps.dwCaps);
            goto CCS_DDERR_INVALIDPIXELFORMAT;
        }

CCS_DD_OK:
        pccsd->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }

    // don't handle diff. pixel format unless there is Streams Processor

    lpDDSurfaceDesc = pccsd->lpDDSurfaceDesc;  //ly
    caps = lpDDSurfaceDesc->ddsCaps.dwCaps; //ly


    if ( (!(caps & DDSCAPS_TEXTURE)) && ( (!STREAMS_PROCESSOR_PRESENT && !PIXEL_FORMATTER_PRESENT) ||
       (PIXEL_FORMATTER_PRESENT && ((BYTE)sData->ddpf.dwRGBBitCount == 8) &&
       (((LPDDPIXELFORMAT)&(pccsd->lpDDSurfaceDesc->ddpfPixelFormat))->dwFlags & DDPF_FOURCC))) )
//ly    if( (!STREAMS_PROCESSOR_PRESENT && !PIXEL_FORMATTER_PRESENT) ||
//ly       (PIXEL_FORMATTER_PRESENT && ((BYTE)sData->ddpf.dwRGBBitCount == 8) &&
//ly       (((LPDDPIXELFORMAT)&(pccsd->lpDDSurfaceDesc->ddpfPixelFormat))->dwFlags & DDPF_FOURCC)) )
    {
CCS_DDERR_INVALIDPIXELFORMAT:
        DPF( "Can not create surface in diff. pixel format");
        pccsd->ddRVal = DDERR_INVALIDPIXELFORMAT;
        return DDHAL_DRIVER_HANDLED;
    }
    else
    {
//ly        lpDDSurfaceDesc = pccsd->lpDDSurfaceDesc;
//ly        caps = lpDDSurfaceDesc->ddsCaps.dwCaps;
        lpDPF = (LPDDPIXELFORMAT)&(lpDDSurfaceDesc->ddpfPixelFormat);
        flags = lpDPF->dwFlags;

        if( (caps & DDSCAPS_OVERLAY) || PIXEL_FORMATTER_PRESENT )
        {
            if (flags & DDPF_FOURCC)
            {
                switch (lpDPF->dwFourCC)
                {
                case FOURCC_YUY2:
                    lpDPF->dwYUVBitCount = 16;
                    goto CCS_DD_OK;
                    break;

                case FOURCC_Y211:
                    if (!(VIRGE_FAMILY_PRESENT))
                    {
                        // Y211 linear format not supported by Trio
                        // Blt32() function (limitation)
                        goto CCS_DDERR_INVALIDPIXELFORMAT;
                    }
                    lpDPF->dwYUVBitCount = 8;
                    goto CCS_DD_OK;
                    break;

                default:
                    DPF( "Can't create fourcc code 0x%08x", lpDPF->dwFourCC);
                    goto CCS_DDERR_INVALIDPIXELFORMAT;
                    break;
                }
            }
            else
            if (flags & DDPF_PALETTEINDEXED8)
            {
                    DPF( "Can't create RGB8 overlay");
                    goto CCS_DDERR_INVALIDPIXELFORMAT;
            }
            else
            if (flags & DDPF_RGB)
                //The overlay surface is in RGB format
                switch (lpDPF->dwRGBBitCount)
                {
                case 16:
                    if( IsRGB15(lpDPF) )
                    {
                        DPF( "Overlay RGB15 ok");
                        goto CCS_DD_OK;
                    }
                    else
                    if( IsRGB16(lpDPF) )
                    {
                        DPF( "Overlay RGB16 ok");
                        goto CCS_DD_OK;
                    }
                    break;
                case 24:
               if( IsRGB24(lpDPF) && !(sData->band.SWFlags&SW_NOTSPT24BOVLY ))
                    {
                        DPF( "Overlay RGB24 ok");
                        goto CCS_DD_OK;
                    }
                    break;
                case 32:
               if( pccsd->bIsDifferentPixelFormat )
                    {
                        DPF( "Can not create surface in diff. pixel format");
                        goto CCS_DDERR_INVALIDPIXELFORMAT;
                    }
               else if( IsRGB32(lpDPF) && !(sData->band.SWFlags&SW_NOTSPT32BOVLY ) )
                    {
                        DPF( "Overlay RGB32 ok");
                        goto CCS_DD_OK;
                    }
                    break;
                default:
                    DPF( "Can't create RGB%d", lpDPF->dwRGBBitCount);
                    goto CCS_DDERR_INVALIDPIXELFORMAT;
                    break;
                }
            else
            {
                DPF( "Unsupported overlay surface pixel format");
                goto CCS_DDERR_INVALIDPIXELFORMAT;
            }
        }
        else
        if( caps & DDSCAPS_ALPHA )
        {
            //we can only support 1:5:5:5 format.
            if (flags & DDPF_RGB)
                if (lpDPF->dwRGBBitCount == 16)
                    if( IsRGB15(lpDPF) )
                        goto CCS_DD_OK;
                    else
                    {
                        DPF( "Can't create alpha RGB%d", lpDPF->dwRGBBitCount);
                        goto CCS_DDERR_INVALIDPIXELFORMAT;
                    }
                else
                {
                    DPF( "Can only support RGB1555 alpha surface");
                    goto CCS_DDERR_INVALIDPIXELFORMAT;
                }
            else
            {
                DPF( "Unsupported alpha surface pixel format");
                goto CCS_DDERR_INVALIDPIXELFORMAT;
            }
        }
        else
        if( caps & DDSCAPS_ZBUFFER )
        {
            //we can only support 16 bit format.
            if (flags & DDPF_RGB)
            {
                if (lpDPF->dwRGBBitCount == 16)
                {   goto CCS_DD_OK; }
                else
                {
                    DPF( "Can't create alpha ZBUFFER%d", lpDPF->dwRGBBitCount);
                    goto CCS_DDERR_INVALIDPIXELFORMAT;
                }
            }
            else
            {
                DPF( "Unsupported ZBUFFER surface pixel format");
                goto CCS_DDERR_INVALIDPIXELFORMAT;
            }
        }
        else
        if( caps & DDSCAPS_TEXTURE )
        {
            if (flags & DDPF_RGB)
            {   goto CCS_DD_OK; }
            else
            {
                DPF( "Unsupported TEXTURE surface pixel format");
                goto CCS_DDERR_INVALIDPIXELFORMAT;
            }
        }
        else
        {
            DPF( "Can only support diff. pixel format in overlay & alpha surfaces");
            goto CCS_DDERR_INVALIDPIXELFORMAT;
        }
    }

        goto CCS_DDERR_INVALIDPIXELFORMAT;

} /* CanCreateSurface32 */


#ifdef MIP

/* ---------------------------------------------------------------
                          MIPMAP support

    AddAttachedSurface is to support the mode of loading mipmaps by
    allocating each mip individually and attaching them later.

    There are many methods to accomplish this.  There is only one method
    supported here, but with some modification this routine can handle most
    scenarios.

    This method supports software that:
        1.  Creates system memory surfaces as individual levels of
            a mipmap.
        2.  Loads the data from disk into each surface.
        3.  Creates video memory surfaces identical to the system memory
            surfaces.
        4.  Performs individual "D3D Load" calls on each level to load from
            system memory to video memory (this is handled in the HEL).
        5.  Use AddAttached Surface to attach the individual video memory
            surfaces.

            Surfaces must be attached in specific order (this needs more code
            to support other ways of attaching).

            Attach surface(n) to surface(n-1) where (n) starts out as the
            smallest mip level.


    NOTE:   Detaching mipmap surfaces is NOT supported.  A callback for
            detaching surfaces must be added.  This support would create a lot
            of scenarios and detaching mipmaps would be very difficult to
            support reliably.
 --------------------------------------------------*/
/*
 *
 * AddAttachedSurface32
 *
 */
#pragma optimize("",off)
DWORD __stdcall AddAttachedSurface32(LPDDHAL_ADDATTACHEDSURFACEDATA aasd)
{
    void WINAPI DDHAL32_VidMemFree( LPDDRAWI_DIRECTDRAW_GBL lpDD, int heap, FLATPTR ptr );
    DWORD   Hght,Ptch;
    DWORD   *Src,*Dst;

    // Make sure both are mipmaps.
    if ( ((aasd->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_MIPMAP) == DDSCAPS_MIPMAP) &&
         ((aasd->lpSurfAttached->ddsCaps.dwCaps & DDSCAPS_MIPMAP) == DDSCAPS_MIPMAP) )
    {
        // Make sure the "attach to" surface is equal or larger than the "attach from"
        if ( aasd->lpDDSurface->lpGbl->wWidth >= aasd->lpSurfAttached->lpGbl->wWidth )
        {
            if ( ((aasd->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) ==
                    DDSCAPS_SYSTEMMEMORY) ||
                 ((aasd->lpSurfAttached->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) ==
                    DDSCAPS_SYSTEMMEMORY) )
            {
                // Let the DirectDraw HEL handle
                aasd->ddRVal = DD_OK;
                return DDHAL_DRIVER_NOTHANDLED;
            }

            // Get height and pitch of "attach to" surface
            Hght = (DWORD)aasd->lpDDSurface->lpGbl->wHeight;
            Ptch = (DWORD)aasd->lpDDSurface->lpGbl->lPitch;
            // Offset the destination pointer by it's size so the next mip level winds
            // up in the right place.
            Dst = (DWORD *)(aasd->lpDDSurface->lpGbl->fpVidMem + ( Hght * Ptch ));
            // Simply retrieve the source pointer from the "attach from" surface
            Src = (DWORD *)aasd->lpSurfAttached->lpGbl->fpVidMem;
            // Get the count of DWORDS to copy based on the "attach from" surface's
            // size
            Hght = (DWORD)aasd->lpSurfAttached->lpGbl->wHeight;
            Hght *= (DWORD)aasd->lpSurfAttached->lpGbl->lPitch;
            Hght /= 4;  // Make sure we're doing DWORDS
            // Copy the "attach from" surface to the slot provided in the "attach to"
            while ( Hght-- ) *Dst++ = *Src++;

            // Free the original "attach from" surface.
            DDHAL32_VidMemFree( aasd->lpDD, (int)aasd->lpSurfAttached->lpGbl->lpVidMemHeap,
                aasd->lpSurfAttached->lpGbl->fpVidMem );
        }
    }

    // Let the DirectDraw HEL handle
    aasd->ddRVal = DD_OK;
    return DDHAL_DRIVER_NOTHANDLED;
}
#pragma optimize("",on)

#endif //MIP


#if 0 //lymip
//if adopte this method, we don't have to allocate extra memory in SurfaceCreate function
#pragma optimize("",off)
DWORD __stdcall AddAttachedSurface32(LPDDHAL_ADDATTACHEDSURFACEDATA aasd)
{

    FLATPTR WINAPI DDHAL32_VidMemAlloc(LPDDRAWI_DIRECTDRAW_GBL lpDD, int heap, DWORD dwWidth, DWORD dwHeight);//lymip
    void WINAPI DDHAL32_VidMemFree( LPDDRAWI_DIRECTDRAW_GBL lpDD, int heap, FLATPTR ptr );
    DWORD   *Dst, *Src1, *Src2;
    DWORD   Hght1, Hght2,Hght3,Ptch3,VidMem3;
    int     vHeap;
    DDRAWI_DIRECTDRAW_GBL tempDD;

    // Make sure both are mipmaps.
    if ( ((aasd->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_MIPMAP) == DDSCAPS_MIPMAP) &&
         ((aasd->lpSurfAttached->ddsCaps.dwCaps & DDSCAPS_MIPMAP) == DDSCAPS_MIPMAP) )
    {
#if 1 //lymip
        // Make sure the "attach to" surface is equal or larger than the "attach from"
        if ( aasd->lpDDSurface->lpGbl->wWidth >= aasd->lpSurfAttached->lpGbl->wWidth )
        {
            if ( ((aasd->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) ==
                    DDSCAPS_SYSTEMMEMORY) ||
                 ((aasd->lpSurfAttached->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) ==
                    DDSCAPS_SYSTEMMEMORY) )
            {
                // Let the DirectDraw HEL handle
                aasd->ddRVal = DD_OK;
                return DDHAL_DRIVER_NOTHANDLED;
            }


            // Get height and pitch of "attach to" surface
//            Hght = (DWORD)aasd->lpDDSurface->lpGbl->wHeight;
//            Ptch = (DWORD)aasd->lpDDSurface->lpGbl->lPitch;
/*
            // Offset the destination pointer by it's size so the next mip level winds
            // up in the right place.
            Dst = (DWORD *)(aasd->lpDDSurface->lpGbl->fpVidMem + ( Hght * Ptch ));
            // Simply retrieve the source pointer from the "attach from" surface
            Src = (DWORD *)aasd->lpSurfAttached->lpGbl->fpVidMem;

            // Get the count of DWORDS to copy based on the "attach from" surface's
            // size
            Hght = (DWORD)aasd->lpSurfAttached->lpGbl->wHeight;
            Hght *= (DWORD)aasd->lpSurfAttached->lpGbl->lPitch;
            Hght /= 4;  // Make sure we're doing DWORDS
            // Copy the "attach from" surface to the slot provided in the "attach to"

//ly            while ( Hght-- ) *Dst++ = *Src++;

            // Free the original "attach from" surface.
//ly            DDHAL32_VidMemFree( aasd->lpDD, (int)aasd->lpSurfAttached->lpGbl->lpVidMemHeap,
//ly                aasd->lpSurfAttached->lpGbl->fpVidMem );
*/

            //set parameter for new surface
            //have to pick to smaller pitch among 2 surface so the data won't end
            //in the middle of pitch
            Ptch3 = (DWORD)aasd->lpSurfAttached->lpGbl->lPitch;

            Hght1 = (DWORD)aasd->lpDDSurface->lpGbl->wHeight;
            Hght2 = (DWORD)aasd->lpSurfAttached->lpGbl->wHeight;
            Hght3 = Hght2 + ( Hght1 * (DWORD)aasd->lpDDSurface->lpGbl->lPitch)/Ptch3;

            Src1 = (DWORD *)aasd->lpDDSurface->lpGbl->fpVidMem;
            Src2 = (DWORD *)aasd->lpSurfAttached->lpGbl->fpVidMem;

            //total data from surface 1
            Hght1 *= (DWORD)aasd->lpDDSurface->lpGbl->lPitch;
            Hght1 /= 4;  // Make sure we're doing DWORDS

            //total data from surface 2
            Hght2 *= (DWORD)aasd->lpSurfAttached->lpGbl->lPitch;
            Hght2 /= 4;  // Make sure we're doing DWORDS

            vHeap = 0;
            VidMem3 = 0;

            tempDD = *(aasd->lpDD);
            while ( (!VidMem3) && (vHeap < 3) )
            {
                //memory location for new surface;
                VidMem3 = DDHAL32_VidMemAlloc( &tempDD, vHeap, Ptch3, Hght3 );
               // VidMem3 = DDHAL32_VidMemAlloc(aasd->lpDD, vHeap, Ptch3, Hght3 ); //??keep lpDD???
                vHeap++;
            }

            if ( !VidMem3 )
            {
                aasd->ddRVal = DDERR_OUTOFMEMORY;
                return DDHAL_DRIVER_HANDLED;
            }

            Dst  = (DWORD *)VidMem3;
            while ( Hght1-- ) *Dst++ = *Src1++;
            while ( Hght2-- ) *Dst++ = *Src2++;

            //delete 2 old surfaces
            DDHAL32_VidMemFree( aasd->lpDD, (int)aasd->lpDDSurface->lpGbl->lpVidMemHeap,
                aasd->lpDDSurface->lpGbl->fpVidMem );

            DDHAL32_VidMemFree( aasd->lpDD, (int)aasd->lpSurfAttached->lpGbl->lpVidMemHeap,
                aasd->lpSurfAttached->lpGbl->fpVidMem );

            //set parameter for new surface
            aasd->lpDD = &tempDD;
            aasd->lpDDSurface->lpGbl->lpVidMemHeap = (LPVMEMHEAP)vHeap; // Indicate which heap the memory came from
            aasd->lpDDSurface->lpGbl->fpVidMem = VidMem3;
            aasd->lpDDSurface->lpGbl->lPitch = Ptch3;
            aasd->lpDDSurface->lpGbl->wHeight= Hght3;
         }
    #endif
    }

    // Let the DirectDraw HEL handle
    aasd->ddRVal = DD_OK;
    return DDHAL_DRIVER_NOTHANDLED;
}
#pragma optimize("",on)


#endif


/*---------------------------------------------------------------------------
 *
 * CreateSurface32
 *
 */
DWORD __stdcall CreateSurface32(LPDDHAL_CREATESURFACEDATA pcsd)
{
    LPDDRAWI_DDRAWSURFACE_LCL FAR *lplpSList;
    LPDDRAWI_DDRAWSURFACE_LCL lpSurf;
    LPDDPIXELFORMAT lpDPF;
    int i;
    LPS3THKINFO sData = GetMyRefData(pcsd->lpDD);
#ifdef MIP
    /*-------------------------------------------*/
    //   MIPMAP support
    //
    // Here is where we create a mipmap surface by allocating 1.5 times
    // the required memory up front so later, when attaching single levels, we
    // already have the memory we need.  After attaching is complete, the ending
    // mipmap had the right amount of memory allocated and the rest are freed
    // after copying them into the parent surface.

    if ( pcsd->lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_MIPMAP )
    {
        // This routine is exported in DDRAW.DLL for drivers that want to
        // handle their own memory allocation.
        FLATPTR WINAPI DDHAL32_VidMemAlloc(LPDDRAWI_DIRECTDRAW_GBL lpDD, int heap, DWORD dwWidth, DWORD dwHeight);
        DWORD   Hght,Ptch,VidMem;
        int     vHeap;

        // Start out with information from the base surface
        lplpSList = pcsd->lplpSList;
        lpSurf = lplpSList[0];

        // Make sure the mipmap is square.
        if ( lpSurf->lpGbl->wWidth != lpSurf->lpGbl->wHeight
             && lpSurf->lpGbl->wWidth != ( lpSurf->lpGbl->wHeight / 2 ) )
        {
            pcsd->ddRVal = DDERR_OUTOFMEMORY;
            return DDHAL_DRIVER_HANDLED;
        }

        // For memory allocation, make sure the height is enough to contain
        // the entire mipmap down to the smallest level regardless of how
        // many levels the app is requesting
        Hght = (DWORD)lpSurf->lpGbl->wHeight;
//ly        Hght += Hght / 2;
        Hght += (Hght/4 + Hght/8);
        vHeap=0; VidMem = 0;

        // Loop through all three possible heaps to allocate the memory we
        // need.
        while ( !VidMem && vHeap < 3 )
        {
            VidMem = lpSurf->lpGbl->fpVidMem = DDHAL32_VidMemAlloc( pcsd->lpDD, vHeap,
            (DWORD)lpSurf->lpGbl->lPitch, Hght );
            vHeap++;
        }

        // If we couldn't get the memory, return error and bail out.
        if ( !VidMem )
        {
            DPF("CreateSurface32:DDERR_OUTOFVIDEOMEMORY");
            pcsd->ddRVal = DDERR_OUTOFVIDEOMEMORY;
            return DDHAL_DRIVER_HANDLED;
        }
         --vHeap;

        // Fill in the fpVidMem pointers in each surface
        for ( i=1; i<=(int)pcsd->dwSCnt; i++ )
        {
            lpSurf->lpGbl->lpVidMemHeap = (LPVMEMHEAP)vHeap;    // Indicate which heap the memory came from
            lpSurf->lpGbl->fpVidMem = VidMem;           // Fill in the pointer to the memory
            Hght = (DWORD)lpSurf->lpGbl->wHeight;       // Get pitch and height
            Ptch = (DWORD)lpSurf->lpGbl->lPitch;
            VidMem += (Hght * Ptch);                    // Increment video pointer by surface size
            lpSurf = lplpSList[i];                      // Get pointer to next surface in list
        }

        // Return that everything is ok and we handled it.
        pcsd->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }

#endif //MIP

//  DPF("in CreateSurface32");

    // refuse to create flippable video overlay surfaces
    // when TV underscanning is enabled

//www : We need to put this back when underscan is supported

//        if ( TV_UNDERSCAN_POSSIBLE &&
//         (pcsd->lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
//         ( pcsd->dwSCnt > 1)  )
//    {
//        pcsd->ddRVal = DDERR_UNSUPPORTED;
//        return DDHAL_DRIVER_HANDLED;
//    }

    lplpSList = pcsd->lplpSList;

    for(i = 0;i < (int)pcsd->dwSCnt; i++)
    {
        // Assume same pixel format

        BYTE bCount = (BYTE) (sData->ddpf.dwRGBBitCount /8);

        // We need to set dwYUVBitCount during CreateSurface time

        lpSurf = lplpSList[i];
      GetValidDDPF(lpSurf, lpDPF);
        {
            if( lpDPF->dwFlags & DDPF_FOURCC )
            {

                switch (lpDPF->dwFourCC)
                {
                case FOURCC_YUY2:
                    lpDPF->dwYUVBitCount = 16;
                    break;

                case FOURCC_Y211:
                    lpDPF->dwYUVBitCount = 8;
                    break;
                }
            }
            bCount = (BYTE) (lpDPF->dwYUVBitCount /8);
        }

        // set lPitch, fpVidMem, dwBlockSizeX & Y

        if( lpDPF->dwFlags & DDPF_FOURCC  )
        {
            WORD wWidth;
            WORD wHeight;
            DWORD dwLinearPitch;
            DWORD dwLinearWidth;
            DWORD dwLinearHeight;

            bCount = (BYTE) (lpDPF->dwYUVBitCount /8);
            wWidth = lpSurf->lpGbl->wWidth;
            wHeight = lpSurf->lpGbl->wHeight;
            dwLinearPitch = ((DWORD)wWidth * bCount + 7) & 0xfffffff8;
            dwLinearWidth = sData->dwScreenWidthBytes;
            dwLinearHeight = (dwLinearPitch * wHeight + sData->dwScreenWidthBytes - 1) / sData->dwScreenWidthBytes;

            pcsd->lpDDSurfaceDesc->lPitch =
            lpSurf->lpGbl->lPitch = dwLinearPitch;
            pcsd->lpDDSurfaceDesc->dwFlags |= DDSD_PITCH;
            lpSurf->lpGbl->fpVidMem = DDHAL_PLEASEALLOC_BLOCKSIZE;
            lpSurf->lpGbl->dwBlockSizeX = dwLinearWidth;
            lpSurf->lpGbl->dwBlockSizeY = dwLinearHeight+1;

            if ( COLOR_ADJUST_HW_PRESENT )
            {
//Commented to fix SPR #15731                WAIT_FOR_VBLANK;
                *sData->lpColorAdjustFlags |= 0x02; //Indicate YUV overlay.
                if (*sData->lpColorAdjustReg & 0xFF1F9FFF )
                {
                  // first, try color adjust set by control() call
                  *sData->lpColorAdjustReg |= 0x80008000; //enable color adjust
                  mmdwSetRegister(COLOR_ADJUST_REG, *sData->lpColorAdjustReg);
                }
                else if (sData->dwColorAdjustDefault)
                {
                  // if not, try color adjust read from registry in DriverInit()
                  *sData->lpColorAdjustReg = sData->dwColorAdjustDefault;
                  mmdwSetRegister(COLOR_ADJUST_REG, *sData->lpColorAdjustReg);
                }
            }

        }
        else
        {

            // set lPitch, fpVidMem, dwBlockSizeX & Y

            if (VIRGE_FAMILY_PRESENT)
                pcsd->lpDDSurfaceDesc->lPitch =
                lpSurf->lpGbl->lPitch = ((DWORD)lpSurf->lpGbl->wWidth * bCount + 7) & 0xfffffff8;   // quad word aligned
            else
                pcsd->lpDDSurfaceDesc->lPitch =
                lpSurf->lpGbl->lPitch = sData->dwScreenWidthBytes;
            pcsd->lpDDSurfaceDesc->dwFlags |= DDSD_PITCH;
            lpSurf->lpGbl->fpVidMem = DDHAL_PLEASEALLOC_BLOCKSIZE;
            lpSurf->lpGbl->dwBlockSizeX = ((DWORD)lpSurf->lpGbl->wWidth * bCount + 7) & 0xfffffff8;
            lpSurf->lpGbl->dwBlockSizeY = (DWORD)lpSurf->lpGbl->wHeight;

            if ( COLOR_ADJUST_HW_PRESENT )
            {
//Commented to fix SPR #15731                WAIT_FOR_VBLANK;
                *sData->lpColorAdjustFlags &= ~0x02; //Don't indicate YUV overlay.
                if (*sData->lpColorAdjustReg & 0xFF1F9FFF )
                {
                *sData->lpColorAdjustReg &= ~0x80008000; //disable color adjust
                mmdwSetRegister(COLOR_ADJUST_REG, *sData->lpColorAdjustReg);
                }
            }


        }
    }

//        if ( TV_UNDERSCAN_POSSIBLE && (pcsd->lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OVERLAY) )
//    {
//         lpSurf->lpGbl->dwBlockSizeY *=2; //Cheat DirectDraw for TV underscanning
//    }

    if (pcsd->lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
    {
        //turn on streams processor if it is not turned on for M3/M5/GX2
        if (sData->lpS3MobileData->wSPStyle & SP_M5 )
        {
        EnableStreamProcessor_M5( sData );
        }

    }

    pcsd->ddRVal = DD_OK;
    return DDHAL_DRIVER_NOTHANDLED;

} /* CreateSurface32 */

/*---------------------------------------------------------------------------
 *
 * DestroySurface32
 *
 * NOTE this function is called now
 *
 */
DWORD __stdcall DestroySurface32( LPDDHAL_DESTROYSURFACEDATA lpd)
{
DWORD dwCaps = lpd->lpDDSurface->ddsCaps.dwCaps;
LPS3THKINFO sData = GetMyRefData(lpd->lpDD);
#ifdef MIP

    /* --------------Mipmapping support------------------------*/
    if ( lpd->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_MIPMAP )
    {   // This routine is exported by DDRAW.DLL for memory management
        void WINAPI DDHAL32_VidMemFree( LPDDRAWI_DIRECTDRAW_GBL lpDD, int heap, FLATPTR ptr );

        // Simply free the memory associated with this surface
        DDHAL32_VidMemFree( lpd->lpDD,
            (int)lpd->lpDDSurface->lpGbl->lpVidMemHeap,
            lpd->lpDDSurface->lpGbl->fpVidMem );
    }
    /* --------------------------------------------------*/

#endif
//  DPF("in DestroySurface32");
    if( lpd->lpDDSurface->lpGbl->dwReserved1)
    {
        FreeSPOD((SPOVERLAYDESC*)lpd->lpDDSurface->lpGbl->dwReserved1);
        sData->lpDstSaved=NULL;
    }
    lpd->ddRVal = DD_OK;
    return DDHAL_DRIVER_NOTHANDLED;
}

DWORD __stdcall SetColorKey32( LPDDHAL_SETCOLORKEYDATA lpd);
DWORD __stdcall UpdateOverlay32( LPDDHAL_UPDATEOVERLAYDATA lpd);
DWORD __stdcall SetOverlayPosition32( LPDDHAL_SETOVERLAYPOSITIONDATA lpd);

/*-----------------01-12-96 11:40pm-----------------
 Support for Direct3D
--------------------------------------------------*/
#ifdef  S3VIRGE
extern BOOL __stdcall D3DHALCreateDriver(LPD3DHAL_GLOBALDRIVERDATA *lplpGlobal,
                                  LPD3DHAL_CALLBACKS* lplpHALCallbacks);
extern BOOL InitItemArrays (void);
extern BOOL DestroyItemArrays (void);
#endif  //S3VIRGE
/*---------------------------------------------------------------------------
 *
 * DriverInit
 *
 */
LPS3THKINFO __stdcall DriverInit( LPVOID ptr )
{
   DWORD        type, size;
   HKEY         hKey;
   char         buff[10];
   LPS3THKINFO  sData;
   LONG         ret;
   DWORD        dwBuff, dwSize;

   extern LPVOID _stdcall MapSL(LPVOID);
   extern LPVOID _stdcall MapLS(LPVOID);

   dwSize = sizeof(DWORD);

   DPF( "DriverInit()" );

    //assuming TRIO family
    sDataGlobal = sData = (LPS3THKINFO)MapSL(ptr);
    // new for MM, dynamic vars. need to init'd dynamically.
    sData->bWaitForVSync = 1;
    sData->dwUpdateOverlayFlipOffset = 0;
    // end new
    sData->lpdraw_engine_busy = draw_engine_busy;
    sData->lproom_for_blt = room_for_blt;
    sData->lpwait_for_idle = wait_for_idle;
    sData->lpwrite_mm8200 = trio_mm8200;
   //
   //  ptr is a 16:16 pointer to the lpInfo variable in the
   //  16 bit display driver, we need to give the driver a
   //  pointer to our sData structure.
   //
   sData->dwSize = sizeof(S3THKINFO);
   sData->lpVisibleOverlay=NULL;    //manbug#1995
   sData->bReset = TRUE;
   sData->lpBlt32 = Blt32;
   sData->lpBlt32_968 = Blt32_968;
   sData->lpFlip32 = Flip32;
   sData->lpLock32 = Lock32;
   sData->lpUnLock32 = UnLock32;
   sData->lpGetBltStatus32 = GetBltStatus32;
   sData->lpGetFlipStatus32 = GetFlipStatus32;
   sData->lpSetColorKey32 = SetColorKey32;
   sData->lpUpdateOverlay32 = UpdateOverlay32;
   sData->lpSetOverlayPosition32 = SetOverlayPosition32;
   sData->lpWaitForVerticalBlank32 = WaitForVerticalBlank32;
   sData->lpCanCreateSurface32 = CanCreateSurface32;
   sData->lpCreateSurface32 = CreateSurface32;
   sData->lpDestroySurface32 = DestroySurface32;
#ifdef MIP

   /* ------------- MIPMAP support --------------------*/
   sData->lpAddAttachedSurface32 = AddAttachedSurface32;
   /* --------------------------------------------------*/

#endif //MIP
   sData->lpGetScanLine32 = GetScanLine32;

#define     ddHALInfo       sData->ddHALInfo
#ifdef  DX5
   sData->lpGetDriverInfo32=GetDriverInfo32;
   ddHALInfo.dwFlags |= DDHALINFO_GETDRIVERINFOSET;
#endif  //DX5
#ifdef  S3VIRGE
   sData->D3DGlobals.DMA_Choice = FALSE;
    if (VIRGE_FAMILY_PRESENT){
        sData->lpdraw_engine_busy = virge_draw_engine_busy;
        sData->lproom_for_blt = virge_room_for_blt;
        sData->lpwait_for_idle = virge_wait_for_idle;
        if (!(sData->lpS3MobileData->wSPStyle & SP_M5)) //not for M5
            sData->lpwrite_mm8200 = virge_mm8200;
        sData->lpBlt32 = Blt32Virge;

       /*
        * fill out the D3D callbacks
        */
        /*-----------------01-12-96 11:42pm-----------------
         Direct3D support
        --------------------------------------------------*/
        if ( (sData->lpS3MobileData->b3DCaps & Virge_3D) ||
             (sData->lpS3MobileData->b3DCaps & VirgeGX_3D) )
        {
            D3DHALCreateDriver((LPD3DHAL_GLOBALDRIVERDATA*)&ddHALInfo.lpD3DGlobalDriverData,
                       (LPD3DHAL_CALLBACKS*)&ddHALInfo.lpD3DHALCallbacks);
        }
    }
#endif  //S3VIRGE

   if ((sData->band.SWFlags & SW_HANDSOFFRAS) &&
       (sData->band.SWFlags & SW_CALCULATE))
   {
      /*
      ** On TrioV2 and later, BIOS always programs RAS settings
      ** so the driver shouldn't touch them.  We shouldn't touch these
      ** on Trio64V+ either, but unfortunately some BIOS versions
      ** didn't do this programming.
      */
      sData->dwRasPreCharge = sData->lpS3DriverData->dwRasPreCharge;
   }

   /* Read option WaitForVSync in Registry,
   ** if the option turned off, do not wait for VSync in
   ** UpdateFlipStatus and page flip.
   */

//   #define HKEY_CURRENT_CONFIG 0x80000005
    RegOpenKeyEx((HKEY)HKEY_CURRENT_CONFIG, "Display\\Settings", 0, KEY_READ, &hKey);
    size=10;

    ret = RegQueryValueEx(hKey, "WaitForVSync", 0, &type, buff, &size);
    if ( ret == ERROR_SUCCESS )
    {
        if (lstrcmp(buff,"OFF") == 0)
            sData->bWaitForVSync=0;
        else
            sData->bWaitForVSync=1;
        DPF("buff=%s",buff);
        DPF("size=%d", size);
        DPF("bWaitForVSync=%d", sData->bWaitForVSync);
    }

#ifdef  S3VIRGE
    sData->D3DGlobals.bCommandDMA=2;
    ret = RegQueryValueEx(hKey, "CommandDMA", 0, &type, buff, &size);
    if ( ret == ERROR_SUCCESS )
    {
        //make it case nonsensitive
        if (lstrcmpi(buff,"ON") == 0) sData->D3DGlobals.bCommandDMA=1;
        if (lstrcmpi(buff,"OFF") == 0) sData->D3DGlobals.bCommandDMA=0;
        DPF("bCommandDMA=%d", sData->D3DGlobals.bCommandDMA);
    }

    sData->D3DGlobals.bCheckUV=0;
    ret = RegQueryValueEx(hKey, "CheckUV", 0, &type, buff, &size);
    if ( ret == ERROR_SUCCESS )
        if (lstrcmpi(buff,"ON") == 0)  //case nonsensitive
            sData->D3DGlobals.bCheckUV=1;

    sData->D3DGlobals.bFastBilinear=0;
    ret = RegQueryValueEx(hKey, "FastBiLin", 0, &type, buff, &size);
    if ( ret == ERROR_SUCCESS )
        if (lstrcmp(buff,"ON") == 0)
            sData->D3DGlobals.bFastBilinear=1;

#endif  //S3VIRGE
#ifdef DX5
    /*
    ** Chip-Specific capability adjustments.
    ** We first initialize capabilities
    ** as if we're on a new chip.  Then, if its actually one of our old
    ** chips (and we know all their IDs), then cut back on the capabilities
    ** that arn't supported by that hardware.
    */
    memcpy(&(sData->KernelCaps), &KernelCaps, sizeof(KernelCaps));
    switch(sData->wDeviceId)
    {
        case D_S3764:
        case D_S3VIRGE:
        case D_S3VIRGEVX:
        case D_S3M65:
        case D_S3767:
            // Hardware with no odd/even field detection
            sData->KernelCaps.dwCaps &= ~(DDKERNELCAPS_SETSTATE |
                                          DDKERNELCAPS_FIELDPOLARITY);
            sData->KernelCaps.dwIRQCaps &= ~(DDIRQ_DISPLAY_VSYNC);
            break;
        case 0x0189: // TBD: add D_S3775 to s3data.h
            sData->KernelCaps.dwIRQCaps &= ~(DDIRQ_DISPLAY_VSYNC);
            break;
        default:
            break;
    }

    ret = RegQueryValueEx(hKey, "USEIRQ", 0, &type, buff, &size);
    if ( ret == ERROR_SUCCESS )
    {
        if (lstrcmp(buff,"OFF") == 0)
        {
          sData->KernelCaps.dwIRQCaps = 0;
          DPF("IRQs for autoflipping, etc. disabled");
        }
    }

    ret = RegQueryValueEx(hKey, "ColorAdjust", 0, &type, (LPBYTE) &dwBuff, &dwSize);
    if ( ret == ERROR_SUCCESS )
    {
        sData->dwColorAdjustDefault = dwBuff;
        DPF("ColorAdjustDefault=%lx", sData->dwColorAdjustDefault);
    }
    else
        sData->dwColorAdjustDefault = 0;

    LPB_DriverInit(sData);
#endif

    return sData;
} /* DriverInit */

/*---------------------------------------------------------------------------
 *
 * DllMain
 *
 */
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    long lProcessCount = 0;
   long tmp;

    DPF( "DllMain dwReason = 0x%x",dwReason );
    switch( dwReason )
    {
    case DLL_PROCESS_ATTACH:
    DisableThreadLibraryCalls( hModule );

        do {
            tmp = InterlockedExchange(&lProcessCount, -1);
        } while (tmp == -1);

        if( tmp == 0)         // First process?
    {
        hSharedHeap = HeapCreate( HEAP_SHARED, 2048, 0);
#ifdef  S3VIRGE
        /*ly
        // Initialize Item arrays (Contexts, Textures)
        if (! InitItemArrays ())
        {
            DPF( "DllMain : failed to initialize arrays");
            return FALSE;
        }
        */
         g_hD3DHAL_DriverHeap = HeapCreate(HEAP_SHARED, 0x2000, 0 );//ly

#endif  //S3VIRGE
    }
        tmp += 1;
        InterlockedExchange(&lProcessCount, tmp);

    break;

    case DLL_PROCESS_DETACH:
        do {
            tmp = InterlockedExchange(&lProcessCount, -1);
        } while (tmp == -1);
        tmp -= 1;

        if( tmp == 0)         // Last process?
    {
        HeapDestroy( hSharedHeap);
        hSharedHeap = NULL;
#ifdef  S3VIRGE
        // Destroy Item Arrays
 //ly       DestroyItemArrays ();
        HeapDestroy (g_hD3DHAL_DriverHeap);
#endif  //S3VIRGE
    }
        InterlockedExchange(&lProcessCount, tmp);
    break;
    }

    return TRUE;

} /* DllMain */

/*---------------------------------------------------------------------------
 *
 * SetColorKey32
 *
 */
DWORD __stdcall SetColorKey32( LPDDHAL_SETCOLORKEYDATA lpd)
{
    LPDDRAWI_DDRAWSURFACE_LCL lpSurf, lpSrcSurface;
    LPDDPIXELFORMAT lpDPF;
    DWORD dwFlags;
    DWORD dwBlendCtrl, dwTmp;
    DWORD dwKeyLow, dwKeyHigh;
    BOOL RGB15 = 0;
    LPS3THKINFO sData = GetMyRefData(lpd->lpDD);

//  DPF("in SetColorKey32");
// Handle overlay color key only

//        if ( (sData->lpSpod != (LPSPOVERLAYDESC)(lpd->lpDDSurface->lpGbl->dwReserved1)) &&
//             (sData->lpSpod !=NULL) )

        if (((sData->lpDstSaved !=lpd->lpDDSurface) &&
             (sData->lpDstSaved !=NULL)) || (sData->lpDstSaved == NULL) )
        {
            lpd->ddRVal = DDERR_UNSUPPORTED;
            return DDHAL_DRIVER_NOTHANDLED;
        }

    dwFlags = lpd->dwFlags;

    if (dwFlags & DDCKEY_SRCBLT)
    {
        lpd->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }
    else
    if (dwFlags & DDCKEY_SRCOVERLAY)
    {
        dwBlendCtrl = KeyOnS;
    }
    else
    if (dwFlags & DDCKEY_DESTOVERLAY)
    {
        dwBlendCtrl = KeyOnP;
    }
    else
    {
        goto SCK_DDERR_UNSUPPORTED;
    }

// Extract color keys

    dwKeyLow = lpd->ckNew.dwColorSpaceLowValue;
    dwKeyHigh = lpd->ckNew.dwColorSpaceHighValue;

// Check the pixel format and adjust

    lpSurf = lpd->lpDDSurface;
    GetValidDDPF(lpSurf, lpDPF);

    if (lpDPF->dwFlags & DDPF_FOURCC)
    {
        switch (lpDPF->dwFourCC)
        {
        case FOURCC_YUY2:
        case FOURCC_Y211:
//            //We need to adjust U and V
//            dwKeyLow = (dwKeyLow & 0x00ff7f7f) | (~(dwKeyLow | 0x00ff7f7f));
//            dwKeyHigh = (dwKeyHigh & 0x00ff7f7f) | (~(dwKeyHigh | 0x00ff7f7f));
            break;
        default:
            goto SCK_DDERR_UNSUPPORTED;
            break;
        }
    }
    else
    if (lpDPF->dwFlags & DDPF_PALETTEINDEXED8)
    {
                //Does it support color key based on index?
                if (sData->band.SWFlags & SW_INDEXCOLORKEY)
                {
                        dwKeyLow |= IndexColorKey;
                        dwKeyLow |= CompareBits0t7;
                }
                else
                {
                        dwKeyLow = GetPaletteEntry(sData, (BYTE)dwKeyLow);
                        dwKeyHigh = dwKeyLow;
                        dwKeyLow |= CompareBits2t7;
                }
    }
    else
    if (lpDPF->dwFlags & DDPF_RGB)
    {
        switch (lpDPF->dwRGBBitCount)
        {
        case 8:
                        //Does it support color key based on index?
                        if (sData->band.SWFlags & SW_INDEXCOLORKEY)
                        {
                                dwKeyLow |= IndexColorKey;
                                dwKeyLow |= CompareBits0t7;
                        }
                        else
                        {
                                dwKeyLow = GetPaletteEntry(sData, (BYTE)dwKeyLow);
                                dwKeyHigh = dwKeyLow;
                                dwKeyLow |= CompareBits2t7;
                        }
            break;

        case 16:
            if (IsRGB15(lpDPF))
            {
                dwKeyLow = RGB15to32(dwKeyLow);
                dwKeyHigh = RGB15to32(dwKeyHigh);
                RGB15 = 1;
            }
            else
            {
                if (IsRGB16(lpDPF))
                {
                    dwKeyLow = RGB16to32(dwKeyLow);
                    dwKeyHigh = RGB16to32(dwKeyHigh);
                }
                else
                    goto SCK_DDERR_UNSUPPORTED;
            }
            dwKeyLow |= CompareBits3t7;
            break;

        case 24:
            if (IsRGB24(lpDPF))
            {
                dwKeyLow |= CompareBits0t7;
                break; //needn't do anything.
            }
            else
                goto SCK_DDERR_UNSUPPORTED;
            break;

        case 32:
            if (IsRGB32(lpDPF))
            {
                dwKeyLow |= CompareBits0t7;
                break; //needn't do anything.
            }
            else
                goto SCK_DDERR_UNSUPPORTED;
            break;

        default:
            goto SCK_DDERR_UNSUPPORTED;
            break;
        }
    }
//        if( RGB15 ) //always compare!!!
         dwKeyLow |= KeyFromCompare;

// Check for valid lpSpod.

    lpSrcSurface = lpd->lpDDSurface;
    if (lpSrcSurface)
        sData->lpSpod = (LPSPOVERLAYDESC) (lpSrcSurface->lpGbl->dwReserved1);
    else
        sData->lpSpod = NULL;

    //Adjust dwKeyLow and dwBlendCtrl when the streams processor is
    //M3/M5/GX2 type

    if (sData->lpS3MobileData->wSPStyle & SP_M5)
    {
        dwKeyLow &=0x07FFFFFF; //clean KC bit, which is not supported inM5
        dwKeyLow |= ColorKeying;
        dwTmp = dwBlendCtrl;
        dwBlendCtrl = 8<<Ks_Shift;

        if ((dwTmp & KeyOnP) == KeyOnP )
        {
            dwKeyLow |= KeySelectP;
        }
        else if ((dwTmp & KeyOnS) == KeyOnS)
        {
            dwKeyLow |= KeySelectS;
            dwBlendCtrl = 8<<Kp_Shift;
        }
    }

    if( sData->lpSpod)
    {
        sData->lpSpod->dwBlend = dwBlendCtrl;
        sData->lpSpod->dwC_low = dwKeyLow;
        sData->lpSpod->dwC_hi = dwKeyHigh;
    }

// Blast out to Streams Processor

    if( Streams_Processor_On(sData))
    //IF_STREAMS_PROCESSOR_ON
    {
//      if ( sData->lpVisibleOverlay == lpSrcSurface )
        //Above compare is not valid.
        //If it is src overlay, lpSrcSurface is current src surface;
        //If it is dst overlay, lpSrcSurface is current dst surface;
        //For now, since we have only one overlay, it is either src or dst
        //if it is a valid surface.
        //So the right condition is the following
        if ( lpSrcSurface !=NULL )
        {
            if (!(sData->lpS3MobileData->wSPStyle & SP_M5))
                WAIT_FOR_VBLANK;

            mmdwSetRegister(BLEND_CONTROL, dwBlendCtrl);    // do it here?
            mmdwSetRegister(CKEY_LOW, dwKeyLow);
            mmdwSetRegister(CKEY_HI, dwKeyHigh);

           // Save current BlendControl value for panning cursor

            if(!VIRGE_FAMILY_PRESENT)
        if (sData->lpS3MobileData->dwMobileCaps & MOBILE_FAMILY)
                {
                    *sData->lpBlendCtrl = dwBlendCtrl;
                    (sData->lpS3MobileData)->SPRegs[(BLEND_CONTROL-SP_BASE)/4] = dwBlendCtrl;
                }
        }
    }
    lpd->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

SCK_DDERR_UNSUPPORTED:
    lpd->ddRVal = DDERR_UNSUPPORTED;
    return DDHAL_DRIVER_HANDLED;
}


/*---------------------------------------------------------------------------
 *
 * SetOverlayPosition32
 *
 */
DWORD __stdcall SetOverlayPosition32( LPDDHAL_SETOVERLAYPOSITIONDATA lpd)
{
    LPS3THKINFO sData = GetMyRefData(lpd->lpDD);
    DWORD dwTmp;
    DWORD dwBlendCtrl=0;

    sData->lpSpod = (LPSPOVERLAYDESC) (lpd->lpDDSrcSurface->lpGbl->dwReserved1);

    if( sData->lpSpod)
    {
        sData->lpSpod->dwS_xy = XY(lpd->lXPos, lpd->lYPos);
    }

    if ( Streams_Processor_On(sData) )
    //IF_STREAMS_PROCESSOR_ON
    {
        if (!(sData->lpS3MobileData->wSPStyle & SP_M5))
        WAIT_FOR_VBLANK;

        if(!( (sData->bFamilyFlags & MOBILE_FAMILY) ||
              ( sData->lpS3MobileData->wSPStyle & SP_M5 )) )
            mmdwSetRegister(S_XY, XY(lpd->lXPos, lpd->lYPos))

    else        // M65 and M3/M5
        {
            DWORD dXY, dwTmp2;
        BYTE  bVideoPanning;

            if  ( sData->lpS3MobileData->wSPStyle & SP_M5 )
        {
            dwTmp  = dwTmp2 = sData->lpS3MobileData->dwMobileCaps;
            dwTmp2 &=  (SP_IGA1 | PANNING_IGA1);
            if ( (dwTmp & PANNING_IGA1_IGA2) ||
             !dwTmp2             ||
             (dwTmp2 == (PANNING_IGA1 | SP_IGA1)) )
            {
                bVideoPanning = 1;
            }
            else
            {
                bVideoPanning = 0;
            }
        }
        else
        {
            bVideoPanning = 1;          // M65
        }


            if (sData->lpS3MobileData->dwMobileCaps & CENTERING_ENABLED)
            {
                dXY=XY(lpd->lXPos + (*sData->lpCenterOffset >> 16),
                   lpd->lYPos + (*sData->lpCenterOffset & 0x0fff));
                mmdwSetRegister(S_XY, dXY)
            }
            else
        if ((sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED) &&
            (bVideoPanning))
            {
            dwTmp = XY(lpd->lXPos, lpd->lYPos) - *sData->lpVWOffset;

            if (((dwTmp & 0x8000) == 0) && (((dwTmp >> 16) & 0x8000) == 0 ))
            {
                    dXY = sData->lpSpod->dwS_xy = dwTmp;

                    if (*sData->lpShowSSWindow==-1)
                    {
                    *sData->lpShowSSWindow=0;
                    mmdwSetRegister(BLEND_CONTROL,*sData->lpBlendCtrl);
                    }
                    *sData->lpClipOffset = *sData->lpFlipOffset = 0;
                    if (sData->lpS3MobileData->wSPStyle & SP_M5)
                    {
                    *sData->lpSSXY = XY(lpd->lXPos, lpd->lYPos) + *sData->lpTVCenterAdj;
                    mmdwSetRegister(S_XY, sData->lpSpod->dwS_xy + *sData->lpTVCenterAdj);
                    }
                    else
                    {
                    *sData->lpSSXY = XY(lpd->lXPos, lpd->lYPos);
                    mmdwSetRegister(S_XY, sData->lpSpod->dwS_xy);
                    }
            }

            else
            {
                    sData->lpSpod->dwS_wh = *sData->lpSSWH;
                    sData->lpSpod->dwS_0 = *sData->lpSSAddr ;
                    ClipSSWindow(sData->lpSpod->dwS_stride,
                     sData->bytesPerPixel, sData->dwSrcWH, sData);

                    if (sData->lpSpod->dwS_wh & 0xFFFF == 0)
                    {
                        dwBlendCtrl = (dwBlendCtrl & Compose_Mask) | POnS;
                        mmdwSetRegister(BLEND_CONTROL,  dwBlendCtrl);
                        *sData->lpShowSSWindow=0xff;
                    }
                    else
                    {
                        *sData->lpShowSSWindow=0;
                        mmdwSetRegister(BLEND_CONTROL,  *sData->lpBlendCtrl);
                    }

                    if (sData->lpSpod->dwS_wh & 0xFFFF )
                    {
                       if (sData->lpS3MobileData->wSPStyle & SP_M5)
                       {
                       mmdwSetRegister(S_XY, sData->lpSpod->dwS_xy + *sData->lpTVCenterAdj);
                       }
                       else
                       {
                       mmdwSetRegister(S_XY, sData->lpSpod->dwS_xy);
                       }
                    }
                    else
                    {
                        mmdwSetRegister(S_XY, 0x07FF07FF );
                    }
                    mmdwSetRegister(S_WH, sData->lpSpod->dwS_wh);
                    while( IN_VBLANK);
                    while( !IN_VBLANK);
                    mmdwSetRegister(S_0, sData->lpSpod->dwS_0);
                    dXY=sData->lpSpod->dwS_xy;
            }

                    if (sData->lpS3MobileData->wSPStyle & SP_M5)
                    {
                    *sData->lpSSXY = XY(lpd->lXPos, lpd->lYPos) + *sData->lpTVCenterAdj;
                    }
                    else
                    {
                    *sData->lpSSXY = XY(lpd->lXPos, lpd->lYPos);
                    }

            }      // video panning

            else   // full image
            {
            dXY=XY(lpd->lXPos, lpd->lYPos);
                    if (sData->lpS3MobileData->wSPStyle & SP_M5)
                    {
                    mmdwSetRegister(S_XY, sData->lpSpod->dwS_xy + *sData->lpTVCenterAdj);
                    }
                    else
                    {
                    mmdwSetRegister(S_XY, sData->lpSpod->dwS_xy);
                    }
            }

            if ( (sData->lpS3MobileData->dwMobileCaps & MOBILE_FAMILY)&&
                 (sData->lpSpod->dwOpaque & 0x80000000 ) &&
                 (sData->lpS3MobileData->dwMobileCaps & M65_NEW_FIFO_CONTROL) )
            //M1,rev E
            {
                 WORD bytes_per_pixel;
                 DWORD dwOpaqueCtrl;

          bytes_per_pixel = (WORD)sData->ddpf.dwRGBBitCount /8;

          //calculate the pixel stop Fetch
          dwTmp=( ((dXY&X_Mask)>>X_Shift)
              -((mmdwGetRegister(P_XY)&X_Mask)>>X_Shift) );

          dwTmp = (( ((dwTmp * bytes_per_pixel)>>3) + 4) >> 2)<<2;
          dwOpaqueCtrl =  (dwTmp << 3);

          //calculate the pixel start Fetch.
          dwTmp=( ((dXY&X_Mask)>>X_Shift)
              -((mmdwGetRegister(P_XY)&X_Mask)>>X_Shift) );
          dwTmp= (((sData->lpSpod->dwS_wh&W_Mask)>>W_Shift))+dwTmp;

          if ((sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED) && (dwTmp > sData->dwPhysScreenWidth ))
            dwTmp=sData->dwPhysScreenWidth;
          dwTmp=(( ((( dwTmp * bytes_per_pixel)>>3)-1) >> 2)<<5);
          if (dwTmp < dwOpaqueCtrl )
             dwTmp=dwOpaqueCtrl;
          dwOpaqueCtrl |= (dwTmp << 16);
          dwOpaqueCtrl |=(1L << 31);
          sData->lpSpod->dwOpaque = dwOpaqueCtrl;
          if ( *sData->lpShowSSWindow==0 )
            {
            mmdwSetRegister(OPAQUE_CONTROL, sData->lpSpod->dwOpaque);
            }
          else
            mmdwSetRegister(OPAQUE_CONTROL, 0);

        }
    }

    }   // Stream processor

    lpd->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*---------------------------------------------------------------------------
 *
 * UpdateOverlay32
 *
 */
DWORD __stdcall UpdateOverlay32( LPDDHAL_UPDATEOVERLAYDATA lpd)
{
    LPDDRAWI_DDRAWSURFACE_LCL lpSrc, lpDst;
    LPDDPIXELFORMAT lpDPFsrc;
    LPDDPIXELFORMAT lpDPF;
    LPDDOVERLAYFX lpFX;
    DWORD dwFlags;
    DWORD dwCaps;
    DWORD dwKeyLow, dwKeyHigh;
    DWORD dwBlendCtrl, dwOpaqueCtrl;
    DWORD dwSecCtrl;
    DWORD dwFifo, dwRatio;
    DWORD dwTmp, dwTmp2;
    LONG  srcWidth, srcHeight, dstWidth, dstHeight, savedWidth;
    LONG dwTmpSrcWidth;
    LONG dwStride;
    LONG dwStart;
    BYTE bSPOP; //can be either BLENDING, KEYING, or OPAQUEING.
    BYTE bChromaKey = 0;
    WORD wDevice;
    BYTE bCurrentDisplay, bVideoPanning;
    LPS3THKINFO sData = GetMyRefData(lpd->lpDD);

//  DPF("in UpdateOverlay32");
    lpd->ddRVal = DD_OK;

// We can't handle both KEYING & BLENDING

    dwFlags = lpd->dwFlags;
    if ((dwFlags & DDOVER_KEY) && (dwFlags & DDOVER_ALPHA))
    {
        lpd->ddRVal = DDERR_INVALIDPARAMS;
        return DDHAL_DRIVER_HANDLED;
    }

// Allocate Spod and save pointer in dwReserved1 if new overlay

    lpSrc = lpd->lpDDSrcSurface;
    lpDst = lpd->lpDDDestSurface;

    sData->lpDstSaved = lpDst;

    if(lpSrc->lpGbl->dwReserved1 == 0)
    {
        lpSrc->lpGbl->dwReserved1 = (DWORD)AllocSPOD(sData);
        lpDst->lpGbl->dwReserved1 = (DWORD)lpSrc;
        if( lpSrc->lpGbl->dwReserved1 == 0 )
        {
            lpd->ddRVal = DDERR_OUTOFMEMORY;
            return DDHAL_DRIVER_HANDLED;
        }
    }
    sData->lpSpod = (LPSPOVERLAYDESC)lpSrc->lpGbl->dwReserved1;

// Hide

    if (dwFlags & DDOVER_HIDE)
    {
UO_HIDE_OVERLAY:

      if( sData->lpVisibleOverlay == lpSrc)
        {
            sData->lpVisibleOverlay = NULL;
            *(sData->lpbStreamProcessorFlags) |= SP_HIDE;

            if(Streams_Processor_On(sData))
            //IF_STREAMS_PROCESSOR_ON
            {
                if  ( sData->lpS3MobileData->wSPStyle & SP_M5 )
                {
                    mmdwSetRegister(BLEND_CONTROL, 8<<Kp_Shift);//Kp=8, Ks=0;
                } else
                {
                    WAIT_FOR_VBLANK;
                    mmdwSetRegister(BLEND_CONTROL, POnS);
                }
                sData->lpS3MobileData->dwMobileCaps |= HIDE_OVERLAY;
                mmdwSetRegister(S_WH, WH(10,2));      //set to 10X2 square.
                mmdwSetRegister(OPAQUE_CONTROL, 0);   //disable opaque control
            }
        }
        return DDHAL_DRIVER_HANDLED;
    }

// Trivial show reject

    dwCaps = lpSrc->ddsCaps.dwCaps;
    if( dwFlags & DDOVER_SHOW)
    {
      if( sData->lpVisibleOverlay == NULL)
        {
         sData->lpVisibleOverlay = lpSrc;
        }

      if( sData->lpVisibleOverlay != lpSrc)
        {

            if( dwCaps & (DDSCAPS_LIVEVIDEO | DDSCAPS_HWCODEC))
            {
            sData->lpVisibleOverlay->ddsCaps.dwCaps &= ~DDSCAPS_VISIBLE;
            sData->lpVisibleOverlay = lpSrc;
            }
            else
            {
                lpd->ddRVal = DDERR_OUTOFCAPS;
                return DDHAL_DRIVER_HANDLED;
            }
        }
    }

    dwStride =  lpSrc->lpGbl->lPitch;
    srcWidth =  lpd->rSrc.right   - lpd->rSrc.left;
    srcHeight = lpd->rSrc.bottom  - lpd->rSrc.top;
    dstWidth =  lpd->rDest.right  - lpd->rDest.left;
    dstHeight = lpd->rDest.bottom - lpd->rDest.top;
//Fail when vertical stretch ratio is greater than 8
         if (dstHeight/srcHeight >= 9){
                lpd->ddRVal = DDERR_TOOBIGHEIGHT;
      return DDHAL_DRIVER_HANDLED;
         }
    savedWidth = dstWidth;

#ifdef DX5
    if (dwFlags & DDOVER_INTERLEAVED)
    {
        /*
        ** When creating an interleaved overlay, the client is expected
        ** to pass in rectangles which assume "weave" mode.  However,
        ** we are required to start out in "bob" mode, and will switch
        ** to weave later (in KMVT).  So adjust stride and height for "bob"
        ** mode on an interleaved surface.
        */
        DPF("UpdateOverlay32: Interleaved Bob Mode");
        dwStride *= 2;
        srcHeight /= 2;
    }
#endif

    // Adjust dstHeight for KV2 and VIRGEDXGX interlaced modes
    if  ( ((sData->bFamilyFlags & TRIOV2_FAMILY)
         | (sData->bFamilyFlags & VIRGEDXGX_FAMILY))
        && (sData->band.SWFlags & SW_INTERLACED) )
    {
        dstHeight += 2;
        dstHeight &= 0xfffe;

    }

    if  (sData->lpS3MobileData->dwMobileCaps & UNDERSCAN_ENABLED)
    {
        if  (sData->lpS3MobileData->dwMobileCaps & TV_PAL)
        {
        srcHeight = 14*srcHeight/15;
        dstHeight = 14*dstHeight/15;
        }
        else
        {
        srcHeight = 5*srcHeight/6;
        dstHeight = 5*dstHeight/6;
        }
    }

    *sData->lpSSWH   = (DWORD)((dstWidth << 16) | dstHeight);
    *sData->lpSrcWH  = (DWORD)((srcWidth << 16) | srcHeight);

    // Adjust dstWidth & dstHeight for modes required expansion

    sData->dwSrcWH    = *sData->lpSrcWH = (DWORD)((srcWidth << 16) | srcHeight);

    if  ( (!(sData->lpS3MobileData->wSPStyle & SP_M5)) &&
          (sData->lpS3MobileData->dwMobileCaps & MOBILE_FAMILY) &&
          (sData->lpS3MobileData->dwMobileCaps & EXPANSION_ENABLED) )  //m1
    {
        *sData->lpSSWH   = (DWORD)((dstWidth << 16) | dstHeight);
        *sData->lpSrcWH  = (DWORD)((srcWidth << 16) | srcHeight);
        dstWidth   = (dstWidth * (sData->dwPanelSize >> 16)) / sData->dwScreenWidth;
        dstHeight  = (dstHeight * (sData->dwPanelSize & 0x7fff)) / sData->dwScreenHeight;
    }

    if  ( (sData->lpS3MobileData->wSPStyle & SP_M5) &&
          (sData->lpS3MobileData->dwMobileCaps & EXPANSION_ENABLED) )
    {
        if (2*sData->dwScreenWidth <= sData->dwPhysScreenWidth)
        {
        dstWidth   =  dstWidth * 2;
        } else
        {
        if (3*sData->dwScreenWidth <= 2*sData->dwPhysScreenWidth)
        {
             dstWidth   =  dstWidth * 3 / 2;
        } else
        {
            if (5*sData->dwScreenWidth <= 4*sData->dwPhysScreenWidth)
            {
             dstWidth   =  dstWidth * 5 / 4;
            }
        }
        }

        if (2*sData->dwScreenHeight <= sData->dwPhysScreenHeight)
        {
        dstHeight   =  dstHeight * 2;
        } else
        {
        if (3*sData->dwScreenHeight <= 2*sData->dwPhysScreenHeight)
        {
             dstHeight   =  dstHeight * 3 / 2;
        } else
        {
            if (4*sData->dwScreenHeight <= 3*sData->dwPhysScreenHeight)
            {
             dstHeight   =  dstHeight * 4 / 3;
            } else
            {
            if (5*sData->dwScreenHeight <= 4*sData->dwPhysScreenHeight)
            {
                 dstHeight   =  dstHeight * 5 / 4;
            } else
            {
                if (6*sData->dwScreenHeight <= 5*sData->dwPhysScreenHeight)
                {
                 dstHeight   =  dstHeight * 6 / 5;
                }
            }
            }
        }
        }
    }


// Calculate DDA horizonal accumulator initial value

    if ((sData->wDeviceId == D_S3VIRGEVX) || (sData->bFamilyFlags & VIRGEDXGX_FAMILY)){
        dwSecCtrl = VHDDA( srcWidth, dstWidth );
    }
    else {
        dwSecCtrl = HDDA( srcWidth, dstWidth );
    }

// Overlay input data format

    GetValidDDPF(lpSrc, lpDPFsrc);
        sData->band.IsSecFormatYUV = 0;
    if (lpDPFsrc->dwFlags & DDPF_FOURCC)
    {
        bChromaKey = 1;
        sData->bytesPerPixel = (BYTE) lpDPFsrc->dwYUVBitCount;

        switch (lpDPFsrc->dwFourCC)
        {
        case FOURCC_YUY2:
            dwSecCtrl |= S_YCrCb422;    //Not S_YUV422!
                        sData->band.IsSecFormatYUV = 1;
            break;
        case FOURCC_Y211:
            dwSecCtrl |= S_YUV211;
            break;
        default:
            goto UO_DDERR_INVALIDPIXELFORMAT;
        }
    }
    else
    if (lpDPFsrc->dwFlags & DDPF_PALETTEINDEXED8)
    {
        goto UO_DDERR_INVALIDPIXELFORMAT;
    }
    else
    if (lpDPFsrc->dwFlags & DDPF_RGB)
    {
        //The overlay surface is in RGB format
        sData->bytesPerPixel = (BYTE) lpDPFsrc->dwRGBBitCount;

        switch (sData->bytesPerPixel) {
        case 16:
            if (IsRGB15(lpDPFsrc))
                dwSecCtrl |= S_RGB15;
            else
                if (IsRGB16(lpDPFsrc))
                    dwSecCtrl |= S_RGB16;
                else
                    goto UO_DDERR_INVALIDPIXELFORMAT;
            break;
        case 24:
            if (IsRGB24(lpDPFsrc))
                dwSecCtrl |= S_RGB24;
            else
                goto UO_DDERR_INVALIDPIXELFORMAT;
            break;
        case 32:
            if (IsRGB32(lpDPFsrc))
                dwSecCtrl |= S_RGB32;
            else
                goto UO_DDERR_INVALIDPIXELFORMAT;
            break;
        default:
            goto UO_DDERR_INVALIDPIXELFORMAT;
            break;
        }
    }

// Calculate start of video memory in QWORD boundary

    sData->bytesPerPixel >>= 3;
    dwStart = (lpd->rSrc.top * dwStride) + (lpd->rSrc.left * sData->bytesPerPixel);

    if (sData->bytesPerPixel != 3)
        dwStart &= 0xfffffff8;
    else
    {
        //here I want to make sure that for 24bpp,
        //the QWORD boundary is also pixel boundary

        dwTmp = (lpd->rSrc.left & 0x7);
        dwTmp = (dwTmp << 1) + dwTmp;
        dwStart = dwStart - dwTmp;
    }

    sData->dwUpdateOverlayFlipOffset = dwStart;    //save for flip
    dwStart += (lpSrc->lpGbl->fpVidMem - sData->dwScreenFlatAddr);

        *sData->lpSSAddr  = dwStart; //Remember it.

    if (sData->lpS3MobileData->dwMobileCaps & UNDERSCAN_ENABLED)
    {
        dwStart += (lpd->rSrc.bottom  - lpd->rSrc.top) * dwStride;
    }

        if (TV_UNDERSCAN_POSSIBLE)
    {
            sData->dwSSOffset = (lpd->rSrc.bottom  - lpd->rSrc.top) * dwStride;
    }

#ifdef DX5

    if (dwFlags & DDOVER_AUTOFLIP)
    {
        DWORD dwStart2;
        DWORD dwDoubleBuffer;

        dwDoubleBuffer = 0x00000044;

        if (sData->lpS3MobileData->wSPStyle & SP_M5)
        {
            dwDoubleBuffer |= 0x80;
        }
        else
        {

            if (dwFlags & DDOVER_BOB)
            {
                /* ** Can't Bob and Hardware autoflip until M5/GX2 */
                DPF("Can't autoflip BOB in hardware");
                lpd->ddRVal = DDERR_OUTOFCAPS;
                return DDHAL_DRIVER_HANDLED;
            }
        }

        dwStart2 = sData->dwUpdateOverlayFlipOffset;
        dwStart2 += (lpSrc->lpAttachList->lpAttached->lpGbl->fpVidMem - sData->dwScreenFlatAddr);
        mmdwSetRegister(S_1, dwStart2);

        mmdwSetRegister(LPB_DB, dwDoubleBuffer);

        DPF("Overlay Autoflipping enabled at %lx", dwStart2);
    }
#endif

// Set overlay filter characteristics

    if ((dstWidth != srcWidth) || (dstHeight != srcHeight))
    {
        if (dstWidth >= (srcWidth << 2))
            dwSecCtrl |= S_Beyond4x;    //linear, 1-2-2-2-1, for >4X stretch
        else
        if (dstWidth >= (srcWidth << 1))
            dwSecCtrl |= S_2xTo4x;      //bi-linear, for 2X to 4X stretch
        else
            dwSecCtrl |= S_Upto2x;      //linear, 0-2-4-2-0, for X stretch
    }

   // Convert to RFMK2V definitions
    if (dwCaps & DDSCAPS_LIVEVIDEO) wDevice = ID_LVIDEO;
    else if (dwCaps & DDSCAPS_HWCODEC) wDevice = ID_HWCODEC;
    else wDevice = ID_SWCODEC;

        // Assign stream processor to the correct IGA based on the
        // starting video window coordinate x. SP will get IGA 1
        // if SS_X in between (0, physScreenX), otherwise SP is
        // set to IGA2. This checking 's only applicable on M3/M5/GX2
        // in DUAL_IMAGE case.

        if ( (sData->lpS3MobileData->wSPStyle & SP_M5) &&
             (sData->lpS3MobileData->dwMobileCaps  & DUAL_IMAGE))
        {
            lpMemRegB[0x83c4]=0x30;

            if ((WORD)lpd->rDest.left > (WORD)sData->dwScreenWidth / 2)
            {
                // Assign SP to IGA2

                lpd->rDest.left -= (LONG)(sData->dwScreenWidth / 2);
                lpd->rDest.right -= (LONG)(sData->dwScreenWidth / 2);

                if( !(lpMemRegB[0x83C5] & 0x02) )
                {
                    lpMemRegB[0x83c5]=(lpMemRegB[0x83c5] | 0x2);
                    sData->lpS3MobileData->dwMobileCaps &=  ~SP_IGA1;

                    // Reset v.sync counter after re-assigning SP

                    if ( !( sData->lpS3MobileData->bCurrentDisplay & TV_ONLY) )
                    {
                        lpMemReg[0x83d4/2]=(WORD)0xA539;
                        lpMemReg[0x83c4/2]=(WORD)0x4F26;
                        WAIT_FOR_VBLANK_M5
                        WAIT_FOR_DISP_M5
                        lpMemReg[0x83c4/2]=(WORD)0x4F26;
                        lpMemReg[0x83d4/2]=(WORD)0x0026;
                        lpMemReg[0x83d4/2]=(WORD)0xA039;
                        lpMemReg[0x83c4/2]=(WORD)0x4026;
                    }

                }

            }
            else
            {
                // Assign SP to IGA1

                if( (lpMemRegB[0x83C5] & 0x02) )
                {
                    lpMemRegB[0x83c5]=(lpMemRegB[0x83c5] & 0xfd);
                    sData->lpS3MobileData->dwMobileCaps |=  SP_IGA1;

                    // Reset v.sync counter after re-assigning SP

                    lpMemRegB[0x83D4] = 0x6B;
                    if ( !( sData->lpS3MobileData->bCurrentDisplay & TV_ONLY) )
                    {
                        lpMemReg[0x83d4/2]=(WORD)0xA539;
                        lpMemReg[0x83c4/2]=(WORD)0x4026;
                        WAIT_FOR_VBLANK_M5
                        WAIT_FOR_DISP_M5
                        lpMemReg[0x83c4/2]=(WORD)0x4026;
                        lpMemReg[0x83d4/2]=(WORD)0x0026;
                        lpMemReg[0x83d4/2]=(WORD)0xA039;
                    }

                }

            }

        }

    if (sData->band.SWFlags & SW_CALCULATE)
       Bandwidth_SetSecondary(&(sData->band), wDevice,
                     (WORD) (lpDPFsrc->dwRGBBitCount),
                     (SWORD) srcWidth, (SWORD) srcHeight,
                     (SWORD) dstWidth, (SWORD) dstHeight,
                     (SWORD) srcWidth, (SWORD) srcHeight);

// Get optimum fifo settings for given device and destination width

    if ( dwCaps & DDSCAPS_LIVEVIDEO)
        dwFifo = GetK2vFifo( DIGITIZER, (WORD) dstWidth, sData);
    else
    if ( dwCaps & DDSCAPS_HWCODEC)
        dwFifo = GetK2vFifo( HWCODEC, (WORD) dstWidth, sData);
    else
        dwFifo = GetK2vFifo( SWCODEC, (WORD) dstWidth, sData);

   if ((sData->band.SWFlags & SW_HANDSOFFRAS) &&
       (sData->band.SWFlags & SW_CALCULATE))
   {
      /*
      ** On TrioV2 and later, BIOS always programs RAS settings
      ** so the driver shouldn't touch them.  We shouldn't touch these
      ** on Trio64V+ either, but unfortunately some BIOS versions
      ** didn't do this programming.
      */
      dwFifo &= ~(0x18000);
      dwFifo |=  sData->dwRasPreCharge;
   }

    // Return error if decimation too small

    if ( (!(dwCaps & DDSCAPS_LIVEVIDEO)) && (!(dwCaps & DDSCAPS_HWCODEC)) )
    {
        dwRatio = GetK2vRatio( SWCODEC, (WORD) dstWidth, sData);
        dwTmpSrcWidth = (dstWidth * 1000 ) / dwRatio;
        if (dwTmpSrcWidth < srcWidth ){
//                        sData->lpVisibleOverlay = NULL;
                    //in case we were asked to show
            lpd->ddRVal = DDERR_TOOBIGWIDTH;
            return DDHAL_DRIVER_HANDLED;
    }
    }

    // Return if K2 horizontal factor is bigger than 1024.

    if (sData->lpS3MobileData->wSPStyle & SP_M5 )
    {
    if ( (dstWidth - srcWidth) > 1024 )
    {
            lpd->ddRVal = DDERR_TOOBIGWIDTH;
            return DDHAL_DRIVER_HANDLED;
    }
    }


    // Adjust Stream Width for M5 in Dual Image mode when the surface accross the
    // screen at the situation that wPhysResolution is less than wPanelresolution

    sData->lpS3MobileData->dwMobileCaps &= ~HIDE_OVERLAY;
    if  ( (sData->lpS3MobileData->wSPStyle & SP_M5) &&
          (sData->lpS3MobileData->dwMobileCaps & MOBILE_FAMILY) &&
          (sData->lpS3MobileData->dwMobileCaps & DUAL_IMAGE) )
    {
         lpMemRegB[0x83d4]=0x6B;
         bCurrentDisplay = lpMemRegB[0x83d5];
         if ( (bCurrentDisplay & FP_ONLY)            &&
             (sData->lpS3DriverData->wPanelResolution >
             sData->lpS3DriverData->wPhysResolution) )
         {
             if (((WORD)sData->dwPhysScreenWidth > (WORD)lpd->rDest.left) &&
                ((WORD)lpd->rDest.right >= (WORD)sData->dwPhysScreenWidth))
             {
                dstWidth = sData->dwPhysScreenWidth - lpd->rDest.left;
             }
         }
    }

// Extract Color/Chroma Key

    bSPOP = 0;
    dwKeyLow = 0; //set to zero to make sure bit28 will be programmed as 0 in case of KRGB1555.
    dwKeyHigh = 0;
    dwBlendCtrl = 0;
    lpFX = (LPDDOVERLAYFX)&(lpd->overlayFX);

    if (dwFlags & DDOVER_KEYSRC)
    {
        lpDPF = lpDPFsrc;
        bSPOP = KEYING;
        bChromaKey = 1;
        dwKeyLow = lpSrc->ddckCKSrcOverlay.dwColorSpaceLowValue;
        dwKeyHigh = lpSrc->ddckCKSrcOverlay.dwColorSpaceHighValue;
        dwBlendCtrl |= KeyOnS;
    }
    else
    if (dwFlags & DDOVER_KEYSRCOVERRIDE)
    {
        lpDPF = lpDPFsrc;
        bSPOP = KEYING;
        bChromaKey = 1;
        dwKeyLow = lpFX->dckSrcColorkey.dwColorSpaceLowValue;
        dwKeyHigh = lpFX->dckSrcColorkey.dwColorSpaceHighValue;
        dwBlendCtrl |= KeyOnS;
//If key on src, for M5/GX2, we cannot turn on vertical interpolation
        if (sData->lpS3MobileData->wSPStyle & SP_M5)
           sData->band.HWFlags &= ~HW_VFILTER;
    }
    else
    if (dwFlags & DDOVER_KEYDEST)
    {
        GetValidDDPF(lpDst, lpDPF);
        bSPOP = KEYING;
        dwKeyLow = lpDst->ddckCKSrcOverlay.dwColorSpaceLowValue;
        dwKeyHigh = lpDst->ddckCKSrcOverlay.dwColorSpaceHighValue;
        dwBlendCtrl |= KeyOnP;
    }
    else
    if (dwFlags & DDOVER_KEYDESTOVERRIDE)
    {
        GetValidDDPF(lpDst, lpDPF);
        bSPOP = KEYING;
        dwKeyLow = lpFX->dckDestColorkey.dwColorSpaceLowValue;
        dwKeyHigh = lpFX->dckDestColorkey.dwColorSpaceHighValue;
        dwBlendCtrl |= KeyOnP;
    }

// Adjust Color/Chroma Key

    if (bSPOP == KEYING)
    {
        if (!bChromaKey)
        {
            // can't support RGB range.
            if ((dwKeyLow & 0x00ffffff)  != dwKeyHigh)
            {
                lpd->ddRVal = DDERR_INVALIDPARAMS;
                return DDHAL_DRIVER_HANDLED;
            }
        }
        if (lpDPF->dwFlags & DDPF_FOURCC)
        {
            switch (lpDPF->dwFourCC)
            {
            case FOURCC_YUY2:
            case FOURCC_Y211:
                //We need to adjust U and V
//                dwKeyLow = (dwKeyLow & 0x00ff7f7f) | (~(dwKeyLow | 0x00ff7f7f));
//                dwKeyHigh = (dwKeyHigh & 0x00ff7f7f) | (~(dwKeyHigh | 0x00ff7f7f));
                break;
            default:
                goto UO_DDERR_INVALIDPARAMS;
                break;
            }
                dwKeyLow |=  KeyFromCompare;
        }
        else
        if (lpDPF->dwFlags & DDPF_PALETTEINDEXED8)
        {
                         //Does it support color key based on index?
                        if (sData->band.SWFlags & SW_INDEXCOLORKEY)
                        {
                                dwKeyLow |= IndexColorKey;
                                dwKeyLow |= CompareBits0t7;
                        }
                        else
                        {
                                dwKeyLow = GetPaletteEntry(sData, (BYTE)dwKeyLow);
                                dwKeyHigh = dwKeyLow;
                                dwKeyLow |= CompareBits2t7;
                        }

                        dwKeyLow |= KeyFromCompare;
        }
        else
        if (lpDPF->dwFlags & DDPF_RGB)
        {
            switch (lpDPF->dwRGBBitCount)
            {
            case 8:
                dwKeyLow = GetPaletteEntry(sData, (BYTE)dwKeyLow);
                dwKeyHigh = dwKeyLow;
                dwKeyLow |= CompareBits2t7 | KeyFromCompare;
                break;

            case 16:
                if (IsRGB15(lpDPF))
                {
                    dwKeyLow = RGB15to32(dwKeyLow);
                    dwKeyHigh = RGB15to32(dwKeyHigh);
                }
                else
                {
                    if (IsRGB16(lpDPF))
                    {
                        dwKeyLow = RGB16to32(dwKeyLow);
                        dwKeyHigh = RGB16to32(dwKeyHigh);
                    }
                    else
                        goto UO_DDERR_INVALIDPARAMS;
                }
                dwKeyLow |= CompareBits3t7 | KeyFromCompare;
                break;

            case 24:
                if (IsRGB24(lpDPF)){
                    dwKeyLow |= CompareBits0t7 | KeyFromCompare;
                }
                else
                    goto UO_DDERR_INVALIDPARAMS;
                break;

            case 32:
                if (IsRGB32(lpDPF)){
                    dwKeyLow |= CompareBits0t7 | KeyFromCompare;
                    break; //needn't do anything.
                }
                else
                    goto UO_DDERR_INVALIDPARAMS;
                break;

            default:
                goto UO_DDERR_INVALIDPARAMS;
                break;
            }
        }
    }

// Take care of blending

    if (dwFlags & DDOVER_ALPHASRC)
    {
        bSPOP = KEYING;
        dwBlendCtrl |= KeyOnS;
        dwKeyLow &= 0x00ffffff;
    }
    else
    if (dwFlags & DDOVER_ALPHADEST)
    {
        bSPOP = KEYING;
        dwBlendCtrl |= KeyOnP;
        dwKeyLow &= 0x00ffffff;
    }
    else
    if (dwFlags & DDOVER_ALPHASRCCONSTOVERRIDE)
    {
        bSPOP = BLENDING;
        dwTmp = lpFX->dwAlphaSrcConst;
        dwTmp = dwTmp & 0x07;   //Ks: only take 3 bits alpha value.
        if (!dwTmp)
            //we want to see the primary stream only.
            goto UO_HIDE_OVERLAY;

        dwTmp = 8-dwTmp;        //Kp
        dwBlendCtrl |= Dissolve | ((dwTmp << Kp_Shift) & Kp_Mask);
    }
    else
    if (dwFlags & DDOVER_ALPHADESTCONSTOVERRIDE)
    {
        bSPOP = BLENDING;
        dwTmp = lpFX->dwAlphaDestConst;
        dwTmp = dwTmp & 0x07;  //Kp: only take 3 bits alpha value.
        if (!dwTmp)
            //we want to see secondary only, let it go thru opaque.
            dwFlags &= ~DDOVER_ALPHADESTCONSTOVERRIDE;
        else
            dwBlendCtrl |= Dissolve | ((dwTmp << Kp_Shift) & Kp_Mask);
    }

// if neither keying nor blending, use opaque to reduce bandwidth

    if (!(bSPOP & (BLENDING | KEYING)))
    {
       WORD bytes_per_pixel;

       bytes_per_pixel = (WORD)sData->ddpf.dwRGBBitCount /8;
        bSPOP = OPAQUEING;

        if (sData->lpS3MobileData->wSPStyle & SP_M5)
        {
        dwTmp = ((lpd->rDest.right * bytes_per_pixel ) >> 3) - 1;
        } else
        {
        dwTmp = (lpd->rDest.right * bytes_per_pixel + 2) >> 3;
        }
        dwOpaqueCtrl = (1L << 31) | (dwTmp << 19);

        //calculate the pixel stop Fetch.
        dwTmp = ((lpd->rDest.left * bytes_per_pixel) >> 3) + 1;
        dwOpaqueCtrl |= (dwTmp << 3);
    }

    //Adjust dwKeyLow and dwBlendCtrl when the streams processor is
    //M3/M5/GX2 type
    if (sData->lpS3MobileData->wSPStyle & SP_M5)
    {

        if  (bSPOP & KEYING)
        {
            dwKeyLow &=0x07FFFFFF; //clean KC bit, which is not supported inM5
            dwKeyLow |= ColorKeying;

            if ((dwBlendCtrl & KeyOnP) == KeyOnP )
            {
                dwKeyLow |= KeySelectP;
                dwBlendCtrl = 8<<Ks_Shift;
            }
            else
            {
                dwKeyLow |= KeySelectS;
            dwBlendCtrl = 8<<Kp_Shift;
            }
        }
    else
    {
            dwBlendCtrl = 8<<Ks_Shift;
    }
    }

// Update Streams Processor overlay description

    sData->lpSpod->lpDDSrcSurface = lpSrc;
    sData->lpSpod->dwS_0 =         dwStart;
    sData->lpSpod->dwS_xy =        XY(lpd->rDest.left, lpd->rDest.top);
    sData->lpSpod->dwS_wh =        WH(dstWidth, dstHeight);
//sss   sData->lpSpod->dwS_wh =        WH(savedWidth, dstHeight);
    sData->lpSpod->dwS_stride =    dwStride;
    sData->lpSpod->dwS_control =   dwSecCtrl;
    if ((sData->wDeviceId == D_S3VIRGEVX) || (sData->bFamilyFlags & VIRGEDXGX_FAMILY)){
        sData->lpSpod->dwS_hk1k2 =     VHK1K2(srcWidth, dstWidth);
        sData->lpSpod->dwS_vk1 =       VVK1(srcHeight);
        sData->lpSpod->dwS_vk2 =       VVK2(srcHeight, dstHeight);
        sData->lpSpod->dwS_vdda =      VVDDA(dstHeight);
    }
    else {
        sData->lpSpod->dwS_hk1k2 =     HK1K2(srcWidth, dstWidth);
        sData->lpSpod->dwS_vk1 =       VK1(srcHeight);
        sData->lpSpod->dwS_vk2 =       VK2(srcHeight, dstHeight);
        sData->lpSpod->dwS_vdda =      VDDA(dstHeight);
    }
#ifdef DX5
    if ((dwFlags & DDOVER_BOB) && (sData->lpS3MobileData->wSPStyle & SP_M5))
    {
        DWORD dwTemp;
        dwTemp = sData->lpSpod->dwS_vdda;

        /*
        ** ViRGE GX2 ViRGE MX support for Bob in hardware
        */

        dwTemp <<= 1;                   // For BOB DDAV1 = DDAV0 << 1
        dwTemp &= 0x00000FFF;           // DDA Vertical 1
        dwTemp <<= 16;                  // shift into bits 16->27
        dwTemp |= 0x10000000;           // use both DDA 0 and 1

        sData->lpSpod->dwS_vdda &= 0x0000FFFF;
        sData->lpSpod->dwS_vdda |= dwTemp;
    }
#endif

    sData->lpSpod->dwC_low =       dwKeyLow;
    sData->lpSpod->dwC_hi =        dwKeyHigh;
    sData->lpSpod->dwBlend =       dwBlendCtrl;
    sData->lpSpod->dwOpaque =      dwOpaqueCtrl;
    sData->lpSpod->dwFifo =        dwFifo;

   if (sData->band.SWFlags & SW_CALCULATE)
   {
      DWORD HWFlags = sData->band.HWFlags;
      DPF("HWFlags =%x", HWFlags);
      /* HWFlags are determined and set by the bandwidth calculations
      ** in mpc.c (Memory Port Controller) */

      /* These are new feture bits supported by TrioV2 and/or ViRGE/VX */
      if (HWFlags & HW_VFILTER)     sData->lpSpod->dwS_vdda |= 0x00008000;
      if (HWFlags & HW_SAVEMODE2)   sData->lpSpod->dwS_vdda |= 0x00004000;
      if (HWFlags & HW_SAVEMODE1)   sData->lpSpod->dwS_vdda |= 0x00002000;
      if (HWFlags & HW_RASLOW25)    sData->lpSpod->dwFifo   |= 0x00008000;
      if (HWFlags & HW_1CYCLELPB)   sData->lpSpod->dwFifo   |= 0x00040000;

      if( sData->lpVisibleOverlay == lpSrc)
        {
        /* MCLK timeout for CPU, HW CURSOR, and LPB */
          if (HWFlags & HW_SETM1)
          {
           lpMemReg[0x83d4/2]=(((WORD)(sData->band.M1)) << 8) | 0x54;
          }

          /* MCLK timeout for PS/SS (pre-775) or just PS(775+) */
       if (HWFlags & HW_SETM2)
        {
           lpMemReg[0x83d4/2]=(((WORD)(sData->band.M2)) << 8) | 0x60;
          }

          /* MCLK timeout for SS for 775+ */
       if (HWFlags & HW_SETM3)
        {
           lpMemReg[0x83d4/2]=(((WORD)(sData->band.M3)) << 8) | 0x72;
          }

          /* Y-only V Filter, necessary for Live Video width on 775 */
           if (HWFlags & HW_VFILTERYONLY2)
        {

        if (sData->lpS3DriverData->MCLKData == 0 )
        {
            lpMemRegB[0x83d4]=0x87;
            lpMemRegB[0x83d5]=(lpMemRegB[0x83d5] | 1);
        }
        else
        {
            lpMemRegB[0x83d4]=0x87;
            lpMemRegB[0x83d5]=(lpMemRegB[0x83d5] & 0xFE);

        }

          }
       else
          {
          if (sData->band.HWCaps & HW_VFILTERYONLY2)
            {
            lpMemRegB[0x83d4]=0x87;
            lpMemRegB[0x83d5]=(lpMemRegB[0x83d5] & 0xFE);
            }
          }

       if (HWFlags & HW_SETSSFETCH)
        {
            BYTE L1;                      /* CR 93 */
            BYTE L2;                      /* CR 92 */
            DWORD stride;

            /* calculate source width in slots */
            stride = (srcWidth * lpDPFsrc->dwRGBBitCount + 63) / 64;

            L1 = (BYTE)stride & 0xFF;
            L2 = (BYTE)(stride>>8) & 0x7F | 0x80;

            lpMemReg[0x83d4/2]=((WORD)L2 << 8) | 0x92;
            lpMemReg[0x83d4/2]=((WORD)L1 << 8) | 0x93;

          }
        }
   }
//wwww
    if  ( sData->lpS3MobileData->wSPStyle & SP_M5 ) {
        *sData->lpVideoType     = dwCaps;
        *sData->lpSSXY     = sData->lpSpod->dwS_xy;
        *sData->lpBlendCtrl     = dwBlendCtrl;
        *sData->lpShowSSWindow   = 0;
     }

    if ((sData->lpS3MobileData->dwMobileCaps & MOBILE_FAMILY) ||
                        ( sData->lpS3MobileData->wSPStyle & SP_M5 ) )
    {
        *sData->lpVideoType     = dwCaps;
        *sData->lpSSXY     = sData->lpSpod->dwS_xy;
        *sData->lpBlendCtrl     = dwBlendCtrl;
        *sData->lpShowSSWindow   = 0;

        if  ( sData->lpS3MobileData->wSPStyle & SP_M5 )
    {
        dwTmp  = dwTmp2 = sData->lpS3MobileData->dwMobileCaps;
        dwTmp2 &=  (SP_IGA1 | PANNING_IGA1);
        if ( (dwTmp & PANNING_IGA1_IGA2) ||
         !dwTmp2             ||
         (dwTmp2 == (PANNING_IGA1 | SP_IGA1)) )
        {
            bVideoPanning = 1;
        }
        else
        {
            bVideoPanning = 0;
        }
    }
    else
    {
        bVideoPanning = 1;          // M65
    }

        if ((sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED) &&
            (bVideoPanning)  &&
            !(sData->lpS3MobileData->dwMobileCaps & DUAL_IMAGE))
        {
        *sData->lpSSAddr   = sData->lpSpod->dwS_0;
        *sData->lpSSWH     = sData->lpSpod->dwS_wh;
        dwTmp              = sData->lpSpod->dwS_xy - *sData->lpVWOffset;
        if (((dwTmp & 0x8000) == 0) && (((dwTmp >> 16) & 0x8000) == 0 ))
        {
            sData->lpSpod->dwS_xy = dwTmp;
            *sData->lpClipOffset = *sData->lpFlipOffset = 0;
        }
        else
        {
            ClipSSWindow(dwStride, sData->bytesPerPixel, sData->dwSrcWH, sData);

            // Overlay window is completely clipped from the
            // visible region. To hide overlay window, we have
            // to change compose mode to POnS
            //
            if (sData->lpSpod->dwS_wh & 0xFFFF == 0)
            {
            if  ( sData->lpS3MobileData->wSPStyle & SP_M5 )
            {
                dwBlendCtrl = 8<<Kp_Shift; //Kp=8, Ks=0;
            } else
            {
                dwBlendCtrl = (dwBlendCtrl & Compose_Mask) | POnS;
            }
            mmdwSetRegister(BLEND_CONTROL,  dwBlendCtrl);
            *sData->lpShowSSWindow=0xff;
            }
        }
        }
        else if ((sData->lpS3MobileData->dwMobileCaps & CENTERING_ENABLED) &&
                (sData->lpS3MobileData->dwMobileCaps & SP_IGA1))
        {
            sData->lpSpod->dwS_xy += *sData->lpCenterOffset;
        }
        else if (sData->lpS3MobileData->dwMobileCaps & EXPANSION_ENABLED)
        {
        if  ( (!(sData->lpS3MobileData->wSPStyle & SP_M5)) &&
              (sData->lpS3MobileData->dwMobileCaps & MOBILE_FAMILY) )
        {
            dwTmp  = sData->lpSpod->dwS_xy & 0x7fff;   // SS height
            dwTmp  = (dwTmp * (sData->dwPanelSize & 0x7fff)) / sData->dwScreenHeight;
            sData->lpSpod->dwS_xy = (sData->lpSpod->dwS_xy & 0x7fff0000) + dwTmp;
        }
        if  ( sData->lpS3MobileData->wSPStyle & SP_M5 )
        {
            dwTmp  = sData->lpSpod->dwS_xy & 0x7fff;   // SS height
            dwTmp2 = sData->lpSpod->dwS_xy & 0x7fff0000;       // SS width
            dwTmp2 >>=16;
            dwTmp--;
            dwTmp2--;
            if (2*sData->dwScreenWidth <= sData->dwPhysScreenWidth)
            {
            dwTmp2= dwTmp2 * 2;
            } else
            {
            if (3*sData->dwScreenWidth <= 2*sData->dwPhysScreenWidth)
            {
                dwTmp2 = dwTmp2 * 3 / 2;
            } else
            {
                if (5*sData->dwScreenWidth <= 4*sData->dwPhysScreenWidth)
                {
                dwTmp2 = dwTmp2 * 5 / 4;
                }
            }
            }


           if (2*sData->dwScreenHeight <= sData->dwPhysScreenHeight)
           {
               dwTmp = dwTmp * 2;
           } else
           {
               if (3*sData->dwScreenHeight <= 2*sData->dwPhysScreenHeight)
               {
               dwTmp = dwTmp * 3 / 2;
               } else
               {
               if (4*sData->dwScreenHeight <= 3*sData->dwPhysScreenHeight)
               {
                   dwTmp = dwTmp * 4 / 3;
               } else
               {
                   if (5*sData->dwScreenHeight <= 4*sData->dwPhysScreenHeight)
                   {
                   dwTmp = dwTmp * 5 / 4;
                   } else
                   {
                   if (6*sData->dwScreenHeight <= 5*sData->dwPhysScreenHeight)
                   {
                       dwTmp = dwTmp * 6 / 5;
                   }
                   }
               }
               }
           }
           sData->lpSpod->dwS_xy = XY(dwTmp2, dwTmp);
           sData->lpSpod->dwS_xy += *sData->lpCenterOffset;
        }
        }
        else
        {
        *sData->lpSSWH     = sData->lpSpod->dwS_wh;
        }
        (sData->lpS3MobileData)->SPRegs[(BLEND_CONTROL-SP_BASE)/4] = dwBlendCtrl;
        (sData->lpS3MobileData)->SPRegs[(S_CONTROL-SP_BASE)/4]     = sData->lpSpod->dwS_control;
        (sData->lpS3MobileData)->SPRegs[(S_HK1K2-SP_BASE)/4]          = sData->lpSpod->dwS_hk1k2;
    }
//www
    if  (( sData->lpS3MobileData->wSPStyle & SP_M5 ) &&
            (sData->lpS3MobileData->dwMobileCaps & DUAL_IMAGE))
    {
        lpMemRegB[0x83c4]=ARCH_CONFIG_REG;
        if( lpMemRegB[0x83C5] & SPSRC_IGA2 )
        {
            if ( (sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED) &&
                !(sData->lpS3MobileData->dwMobileCaps & PANNING_IGA1) )
                sData->lpSpod->dwS_xy  = sData->lpSpod->dwS_xy - (*sData->lpVWOffset - (((DWORD)sData->dwScreenWidth/2)<<16));
        }
        else
        {
            if ( (sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED) &&
                 (sData->lpS3MobileData->dwMobileCaps & PANNING_IGA1) )
                sData->lpSpod->dwS_xy  = sData->lpSpod->dwS_xy - (*sData->lpVWOffset - (((DWORD)sData->dwScreenWidth/2)<<16));
        }
     }
// Update + show

   if( sData->lpVisibleOverlay == lpSrc)
    {
        if(Streams_Processor_On(sData))
        //IF_STREAMS_PROCESSOR_ON
        {
            *(sData->lpbStreamProcessorFlags) &= ~SP_HIDE;
            sData->lpwait_for_idle(sData);
            if  ( !( sData->lpS3MobileData->wSPStyle & SP_M5 ) )
                WAIT_FOR_VBLANK;

            if ( sData->band.HWFlags & HW_VFILTER )
            {
                sData->lpS3DriverData->VideoData = 1;// Tell vdd that we have video on screen

                if (sData->lpS3DriverData->MCLKData == 1 )
                {
                    sData->lpSpod->dwFifo &=0xFFFF8000;
                    sData->lpSpod->dwFifo |=0x3282;
                    sData->lpSpod->dwS_vdda &= 0xFFFF1FFF;

                }
            }
            if (sData->lpS3MobileData->wSPStyle & SP_M5)
            {
              *sData->lpSSXY += *sData->lpTVCenterAdj;
              sData->lpSpod->dwS_xy += *sData->lpTVCenterAdj;
            }

            mmdwSetRegister(S_0,            sData->lpSpod->dwS_0);
            mmdwSetRegister(S_XY,           sData->lpSpod->dwS_xy);
            mmdwSetRegister(S_WH,           sData->lpSpod->dwS_wh);
            mmdwSetRegister(S_STRIDE,       sData->lpSpod->dwS_stride);
            mmdwSetRegister(S_CONTROL,      sData->lpSpod->dwS_control);
            mmdwSetRegister(S_HK1K2,        sData->lpSpod->dwS_hk1k2);
            mmdwSetRegister(S_VK1,          sData->lpSpod->dwS_vk1);
            mmdwSetRegister(S_VK2,          sData->lpSpod->dwS_vk2);
            mmdwSetRegister(S_VDDA,         sData->lpSpod->dwS_vdda);

            if ( (bSPOP == KEYING) || (sData->lpS3MobileData->wSPStyle & SP_M5) )
            {
                mmdwSetRegister(CKEY_LOW,       sData->lpSpod->dwC_low);
                mmdwSetRegister(CKEY_HI,        sData->lpSpod->dwC_hi);
            }

//          if( dwFlags & (DDOVER_SHOW | DDOVER_KEY | DDOVER_ALPHA))
            {
                mmdwSetRegister(BLEND_CONTROL,  dwBlendCtrl);
            }



            if  ( (!(sData->lpS3MobileData->wSPStyle & SP_M5)) &&
                  (sData->lpS3MobileData->dwMobileCaps & MOBILE_FAMILY) &&
               (sData->lpS3MobileData->dwMobileCaps & M65_NEW_FIFO_CONTROL) )
                    //m1,rev E
            {
                if ( bSPOP == OPAQUEING )
                {
                  WORD bytes_per_pixel;

                  bytes_per_pixel = (WORD)sData->ddpf.dwRGBBitCount /8;

                  //calculate the pixel stop Fetch
                  dwTmp=( ((sData->lpSpod->dwS_xy&X_Mask)>>X_Shift)
                      -((mmdwGetRegister(P_XY)&X_Mask)>>X_Shift) );

                  dwTmp = (( ((dwTmp * bytes_per_pixel)>>3) + 4) >> 2)<<2;
                  dwOpaqueCtrl =  (dwTmp << 3);

                  //calculate the pixel start Fetch.
                  dwTmp=( ((sData->lpSpod->dwS_xy&X_Mask)>>X_Shift)
                      -((mmdwGetRegister(P_XY)&X_Mask)>>X_Shift) );
                  dwTmp= (((sData->lpSpod->dwS_wh&W_Mask)>>W_Shift))+dwTmp;

                  if ((sData->lpS3MobileData->dwMobileCaps & PANNING_ENABLED) && (dwTmp > sData->dwPhysScreenWidth ))
                    dwTmp=sData->dwPhysScreenWidth;
                  dwTmp=(( ((( dwTmp * bytes_per_pixel)>>3)-1) >> 2)<<5);
                  if (dwTmp < dwOpaqueCtrl )
                 dwTmp=dwOpaqueCtrl;
                  dwOpaqueCtrl |= (dwTmp << 16);
                  dwOpaqueCtrl |=(1L << 31);
                  sData->lpSpod->dwOpaque = dwOpaqueCtrl;
                  if (*sData->lpShowSSWindow==0)
                 mmdwSetRegister(OPAQUE_CONTROL, sData->lpSpod->dwOpaque);
                }
              else
                {
                  sData->lpSpod->dwOpaque = 0;
                  mmdwSetRegister(OPAQUE_CONTROL, sData->lpSpod->dwOpaque);
                }
            }

            switch ( sData->lpS3MobileData->wSPStyle )
            {
                case SP_Trio:
                case SP_TrioV2:
                case SP_Yosemite:
                            //For ViRGE family, we need to set the FIFO_CONTROL register
                            //otherwise, it will get PAJAMA in LPB (Live video and HW MPEG)
                            case SP_Virge:
                            case SP_VX:
                            case SP_VirgeGX:
                mmdwSetRegister(FIFO_CONTROL,   sData->lpSpod->dwFifo);
                break;

                case SP_M5:
                    lpMemReg[0x83d4/2]=
                    (((WORD)(sData->band.PS1TimeOut)) << 8) | 0x7b;

                    lpMemReg[0x83d4/2]=
                    (((WORD)(sData->band.Fifo.PS1Threshold)) << 8) | 0x85;

                    lpMemReg[0x83d4/2]=
                    (((WORD)(sData->band.PS2TimeOut)) << 8) | 0x7c;

                    lpMemReg[0x83d4/2]=
                    (((WORD)(sData->band.Fifo.PS2Threshold)) << 8) | 0x86;

                //Secondary Stream Fifo
                lpMemReg[0x83d4/2]=
                (((WORD)(sData->band.SSTimeOut)) << 8) | 0x7d;
                lpMemReg[0x83d4/2]=
                (((WORD)(sData->band.Fifo.SSThreshold)) << 8) | 0x87;
                break;

                default:
                break;

            }



        if(!VIRGE_FAMILY_PRESENT)
        *sData->lpSPFIFOReg = sData->lpSpod->dwFifo;
            sData->lpwrite_mm8200(sData);
        }

    }

    return DDHAL_DRIVER_HANDLED;

UO_DDERR_INVALIDPIXELFORMAT:
    lpd->ddRVal = DDERR_INVALIDPIXELFORMAT;
    return DDHAL_DRIVER_HANDLED;

UO_DDERR_INVALIDPARAMS:
    lpd->ddRVal = DDERR_INVALIDPARAMS;
    return DDHAL_DRIVER_HANDLED;
}

/*---------------------------------------------------------------------------
 *
 * ClipSSWindow
 *
 */
void ClipSSWindow(LONG dwStride,BYTE bytesPerPixel,DWORD dwSrcWH,LPS3THKINFO sData)
{
    WORD newX, newY, newWidth, newHeight, clipped_x, clipped_y;
    WORD xOff, yOff, wTmp;
    DWORD dwTmp, dwAddr;

    clipped_x   = (WORD)((sData->lpSpod->dwS_xy >> 16) - (*sData->lpVWOffset >> 16));
    clipped_y   = (WORD)((sData->lpSpod->dwS_xy & 0xffff) - (*sData->lpVWOffset & 0xffff));
    newX    = clipped_x;
    newY    = clipped_y;
    newWidth    = (WORD) (sData->lpSpod->dwS_wh >> 16);
    newHeight   = (WORD) (sData->lpSpod->dwS_wh);
    xOff    = yOff = 0;

    // clipped in x

    if ( clipped_x & 0x8000 )
    {
    newX      = 1;
    xOff     -= clipped_x;
    wTmp      = newWidth;
    if (newWidth < xOff)
        newWidth = newHeight = 0;
    else
        newWidth -= xOff;

    if (((WORD)(dwSrcWH >> 16)) != wTmp)
        xOff  = (((WORD)(dwSrcWH >> 16)) * xOff) / wTmp;
    }

    // clipped in y

    if ( clipped_y & 0x8000 )
    {
        newY       = 1;
        yOff      -= clipped_y;
        wTmp       = newHeight;
        if (newHeight < yOff)
            newWidth = newHeight = 0;
        else
            newHeight -= yOff;

        if ( (((WORD)dwSrcWH) != wTmp) && (wTmp != 0))
    {
            yOff  = (((WORD)dwSrcWH) * yOff) / wTmp;
    }
    }

    dwAddr = ( ((DWORD)yOff) * dwStride) + ( ((DWORD) xOff)  * bytesPerPixel);
    *sData->lpClipOffset        =  dwAddr;
    dwAddr                    += sData->lpSpod->dwS_0;

    if (bytesPerPixel != 3)
    {
        dwTmp   = dwAddr & 7;
        dwAddr -= dwTmp;
    }
    else
    {
        dwTmp     = (DWORD) (xOff & 0x7);
        dwTmp     = (dwTmp << 1) + dwTmp;
        dwAddr    = dwAddr - dwTmp;
    }

    sData->lpSpod->dwS_0  = dwAddr;
    sData->lpSpod->dwS_xy = (((DWORD)newX) << 16 ) + ((DWORD)newY);
    sData->lpSpod->dwS_wh = (((DWORD)newWidth) << 16) + ((DWORD)newHeight);
}

/*---------------------------------------------------------------------------
 *
 * EnableStreamProcessor_M5
 */
void EnableStreamProcessor_M5( LPS3THKINFO sData )
{
    BYTE bReg, bReg2, bCurrentDisplay;
    WORD wWidth, wHeight;
    DWORD dwTmp;


    //make sure read from IGA1
    lpMemReg[0x83c4/2] = (WORD)0x4026;

    lpMemRegB[0x83d4]=0x67;
    if( (lpMemRegB[0x83d5] & 0xC) == 0xC)
    return;

    //Clear the centering and expansion bits
    sData->lpS3MobileData->dwMobileCaps &=~CENTERING_ENABLED;
    sData->lpS3MobileData->dwMobileCaps &=~EXPANSION_ENABLED;

    lpMemRegB[0x83d4]=0x6B;
    bCurrentDisplay = lpMemRegB[0x83d5];

    lpMemRegB[0x83c4]=0x31;

    if ( (bCurrentDisplay & FP_ONLY)            &&
     ( (sData->lpS3DriverData->wPanelResolution >
        sData->lpS3DriverData->wLogResolution)     ||

      ((sData->lpS3DriverData->wPanelResolution >
        sData->lpS3DriverData->wPhysResolution) &&
           (sData->lpS3MobileData->dwMobileCaps & DUAL_IMAGE))) )

    {
       lpMemRegB[0x83c4] =FP_HORZ_EXP_1;
       bReg = lpMemRegB[0x83c5] & H_GRPH_EXP_MASK;
       lpMemRegB[0x83c4] = FP_HORZ_EXP_2;
       bReg2 = lpMemRegB[0x83c5] & ALL_H_EXP;

       if ((! bReg)  ||
           (! bReg2) ||
           (sData->lpS3DriverData->wLogResolution < MODE_RES_640x400))
       {
       sData->lpS3MobileData->dwMobileCaps |= CENTERING_ENABLED;

       // Initialize actual with and height

       wWidth  = (WORD)sData->dwScreenWidth;
       wHeight = (WORD)sData->dwScreenHeight;

       if (wWidth > sData->dwPhysScreenWidth)
           wWidth = (WORD)sData->dwPhysScreenWidth;

       if (wHeight > sData->dwPhysScreenHeight)
           wHeight = (WORD)sData->dwPhysScreenHeight;

       *sData->lpCenterOffset = ((sData->dwPanelSize&0xffff) - wHeight)/2;
       *sData->lpCenterOffset += (((sData->dwPanelSize>>16) - wWidth)/2)<<16;

       }
       else  // set  expansion flag
       {
       sData->lpS3MobileData->dwMobileCaps |= EXPANSION_ENABLED;
       *sData->lpCenterOffset = (DWORD)0;

       //make sure VWOffset is at 8 pixel boudary (i.e. charater boundary)

       lpMemRegB[0x83c4]=FP_HORZ_EXP_1;
       if (lpMemRegB[0x83c5] & H_CENTERING)
       {
       if (2*sData->dwScreenWidth <= sData->dwPhysScreenWidth)
       {
           *sData->lpCenterOffset = 8*(((sData->dwPhysScreenWidth - 2*sData->dwScreenWidth)/2)/8)<<16;
       } else
       {
           if (3*sData->dwScreenWidth <= 2*sData->dwPhysScreenWidth)
           {
           *sData->lpCenterOffset = 8*(((sData->dwPhysScreenWidth - 3*sData->dwScreenWidth/2)/2)/8)<<16;
           } else
           {
           if (5*sData->dwScreenWidth <= 4*sData->dwPhysScreenWidth)
           {
               *sData->lpCenterOffset = 8*(((sData->dwPhysScreenWidth - 5*sData->dwScreenWidth/4)/2)/8)<<16;
           }
           }
       }
       }


       lpMemRegB[0x83c4]=FP_VERT_EXP_1;
       if (lpMemRegB[0x83c5] & V_CENTERING)
       {
       if (2*sData->dwScreenHeight <= sData->dwPhysScreenHeight)
       {
           *sData->lpCenterOffset += (sData->dwPhysScreenHeight - 2*sData->dwScreenHeight)/2;
       } else
       {
           if (3*sData->dwScreenWidth <= 2*sData->dwPhysScreenWidth)
           {
           *sData->lpCenterOffset += (sData->dwPhysScreenHeight - 3*sData->dwScreenHeight/2)/2;
           } else
           {
           if (4*sData->dwScreenWidth <= 3*sData->dwPhysScreenWidth)
           {
               *sData->lpCenterOffset += (sData->dwPhysScreenHeight - 4*sData->dwScreenHeight/3)/2;
           } else
           {
               if (5*sData->dwScreenWidth <= 4*sData->dwPhysScreenWidth)
               {
               *sData->lpCenterOffset += (sData->dwPhysScreenHeight - 5*sData->dwScreenHeight/4)/2;
               } else
               {
               if (6*sData->dwScreenWidth <= 5*sData->dwPhysScreenWidth)
               {
                   *sData->lpCenterOffset += (sData->dwPhysScreenHeight - 6*sData->dwScreenHeight/5)/2;
               }
               }
           }
           }
       }
       }
       }
    }

    sData->lpwait_for_idle(sData);

    lpMemReg[0x83c4/2] = (WORD)0x4026;

    WAIT_FOR_VBLANK;

    lpMemRegB[0x83d4]=0x67;
    bReg = lpMemRegB[0x83d5];
    lpMemRegB[0x83d5]=bReg | 0xC;  // Enable stream processor

    sData->lpS3MobileData->dwMobileCaps |= HIDE_OVERLAY;
    mmdwSetRegister( P_0,        0);
    mmdwSetRegister( P_STRIDE,      sData->dwScreenWidthBytes);
    mmdwSetRegister( S_WH,       WH(10, 2) );
    mmdwSetRegister( BLEND_CONTROL,  8<<Kp_Shift );
    mmdwSetRegister( OPAQUE_CONTROL, 0 );

    //set SP 's related flags

    lpMemRegB[0x83c4] = ARCH_CONFIG_REG;    // SR30
    bReg = lpMemRegB[0x83c5];

    if (bReg & SPSRC_IGA2)
        sData->lpS3MobileData->dwMobileCaps &= ~SP_IGA1;
    else
        sData->lpS3MobileData->dwMobileCaps |= SP_IGA1;

    if (!(sData->lpS3MobileData->dwMobileCaps & SP_IGA1))
    {
        sData->lpS3MobileData->dwMobileCaps &=~CENTERING_ENABLED;
        sData->lpS3MobileData->dwMobileCaps &=~EXPANSION_ENABLED;
    }


    if (sData->lpS3MobileData->dwMobileCaps & PANNING_IGA1_IGA2)
    {
        bReg &= (SPSRC_IGA2 | TV_IGA2);

        if (bReg == (SPSRC_IGA2 | TV_IGA2))
        {
            dwTmp = *sData->lpVWTop1;
            *sData->lpVWOffset = dwTmp;
            dwTmp = *sData->lpVWBottom1;
            *sData->lpVWBottom = dwTmp;
            sData->lpS3MobileData->dwMobileCaps |= VIDEO_ON_TV;
        }
        else
        {
            dwTmp = *sData->lpVWTop2;
            *sData->lpVWOffset = dwTmp;
            dwTmp = *sData->lpVWBottom2;
            *sData->lpVWBottom = dwTmp;
            sData->lpS3MobileData->dwMobileCaps &= ~VIDEO_ON_TV;
        }
    }

    sData->lpS3MobileData->dwMobileCaps |=SP_ON;
    sData->lpS3MobileData->dwMobileCaps &= ~UNDERSCAN_ENABLED;
    sData->lpS3MobileData->dwMobileCaps &= ~TV_PAL;

    if (bCurrentDisplay & TV_ONLY)
    {
        lpMemRegB[0x83d4]=0x3D;
        bReg = lpMemRegB[0x83d5];
        if (bReg&0x20)      //Is it PAL output
        {
            sData->lpS3MobileData->dwMobileCaps |= TV_PAL;

        }
    }


} /* EnableStreamProcessor_M5 */

#ifdef DX5

/*
 ** GetDriverInfo32
 *
 *  FILENAME: C:\win9x\display\mini\s3\S3_DD32.C
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: DirectDraw has had many compatability problems
 *               in the past, particularly from adding or modifying
 *               members of public structures.  GetDriverInfo is an extension
 *               architecture that intends to allow DirectDraw to
 *               continue evolving, while maintaining backward compatability.
 *               This function is passed a GUID which represents some DirectDraw
 *               extension.  If the driver recognises and supports this extension,
 *               it fills out the required data and returns.
 *  RETURNS:
 *
 */

DWORD __stdcall GetDriverInfo32(LPDDHAL_GETDRIVERINFODATA lpInput)
{
    DWORD dwSize;
    LPS3THKINFO sData = VERIFYSDATA((LPS3THKINFO)(lpInput->dwContext));

#ifdef DEBUG
#define CHECKSIZE(x) if (lpInput->dwExpectedSize != sizeof(x)) \
            DPF("GetDriverInfo: #x structure size mismatch");
#else
#define CHECKSIZE(x) ((void)0)
#endif

    lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;

#ifdef S3VIRGE
    if (VIRGE_FAMILY_PRESENT)
    {
        if (IsEqualIID(&lpInput->guidInfo, &GUID_D3DCallbacks2) )
        {
            D3DHAL_CALLBACKS2 D3DCallbacks2;
            memset(&D3DCallbacks2, 0, sizeof(D3DCallbacks2));

            dwSize = min(lpInput->dwExpectedSize, sizeof(D3DHAL_CALLBACKS2));
            lpInput->dwActualSize = sizeof(D3DHAL_CALLBACKS2);
            CHECKSIZE(D3DHAL_CALLBACKS2);

            D3DCallbacks2.dwSize = dwSize;
            D3DCallbacks2.dwFlags = D3DHAL2_CB32_SETRENDERTARGET  |
                                    D3DHAL2_CB32_DRAWONEPRIMITIVE |
                                    D3DHAL2_CB32_DRAWONEINDEXEDPRIMITIVE |
                                    D3DHAL2_CB32_DRAWPRIMITIVES |
                                    0 ;
             D3DCallbacks2.SetRenderTarget = SetRenderTarget32;
             D3DCallbacks2.DrawOnePrimitive = DrawOnePrimitive32;
             D3DCallbacks2.DrawOneIndexedPrimitive = DrawOneIndexedPrimitive32;
             D3DCallbacks2.DrawPrimitives = DrawPrimitives32;

            memcpy(lpInput->lpvData, &D3DCallbacks2, dwSize);
            lpInput->ddRVal = DD_OK;
        }
    }
#endif // S3VIRGE
    if (STREAMS_PROCESSOR_PRESENT)
    {

        if (IsEqualIID(&lpInput->guidInfo, &GUID_MiscellaneousCallbacks) )
        {
            DPF("GetDriverInfo: MiscellaneousCallbacks");
        }

        else if (IsEqualIID(&lpInput->guidInfo, &GUID_VideoPortCallbacks) )
        {
            dwSize = min(lpInput->dwExpectedSize, sizeof(DDHAL_DDVIDEOPORTCALLBACKS));
            lpInput->dwActualSize = sizeof(DDHAL_DDVIDEOPORTCALLBACKS);
            CHECKSIZE(DDHAL_DDVIDEOPORTCALLBACKS);

            DPF("GetDriverInfo: VideoPortCallbacks");

            memcpy(lpInput->lpvData, &LPBVideoPortCallbacks, dwSize);
            lpInput->ddRVal = DD_OK;
        }

        else if (IsEqualIID(&lpInput->guidInfo, &GUID_VideoPortCaps) )
        {
            dwSize = min(lpInput->dwExpectedSize, sizeof(DDVIDEOPORTCAPS));
            lpInput->dwActualSize = sizeof(DDVIDEOPORTCAPS);
            CHECKSIZE(DDVIDEOPORTCAPS);

            DPF("GetDriverInfo: VideoPort Caps");

            memcpy(lpInput->lpvData, &(sData->LPBData.ddVideoPortCaps), dwSize);
            lpInput->ddRVal = DD_OK;
        }

        else if (IsEqualIID(&lpInput->guidInfo, &GUID_KernelCallbacks) )
        {
            dwSize = min(lpInput->dwExpectedSize, sizeof(DDHAL_DDKERNELCALLBACKS));
            lpInput->dwActualSize = sizeof(DDHAL_DDKERNELCALLBACKS);
            CHECKSIZE(DDHAL_DDKERNELCALLBACKS);

            DPF("GetDriverInfo: DD Kernel Callbacks");

            memcpy(lpInput->lpvData, &KernelCallbacks, dwSize);
            lpInput->ddRVal = DD_OK;
        }

        else if (IsEqualIID(&lpInput->guidInfo, &GUID_KernelCaps))
        {
            dwSize = min(lpInput->dwExpectedSize, sizeof(DDKERNELCAPS));
            lpInput->dwActualSize = sizeof(DDHAL_DDKERNELCALLBACKS);
            CHECKSIZE(DDKERNELCAPS);

            DPF("GetDriverInfo: DD Kernel Capabilities");

            memcpy(lpInput->lpvData, &(sData->KernelCaps), dwSize );
            lpInput->ddRVal = DD_OK;
        }

        if (COLOR_ADJUST_HW_PRESENT)
        {
            if (IsEqualIID(&lpInput->guidInfo, &GUID_ColorControlCallbacks) )
            {
                dwSize = min(lpInput->dwExpectedSize, sizeof(DDHAL_DDCOLORCONTROLCALLBACKS));
                lpInput->dwActualSize = sizeof(DDHAL_DDCOLORCONTROLCALLBACKS);
                CHECKSIZE(DDHAL_DDCOLORCONTROLCALLBACKS);

                DPF("GetDriverInfo: Color Control Callbacks");

                memcpy(lpInput->lpvData, &ColorControlCallbacks, dwSize);
                lpInput->ddRVal = DD_OK;
            }
        }
    }

    return DDHAL_DRIVER_HANDLED;
}

/*
 ** SyncSurfaceData32
 *
 *  FILENAME: C:\win9x\display\mini\s3\S3_DD32.C
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
 */
DWORD __stdcall SyncSurfaceData32(LPDDHAL_SYNCSURFACEDATA lpInput)
{
    LPS3THKINFO sData = VERIFYSDATA((LPS3THKINFO)(lpInput->lpDD->lpGbl->dwReserved3));

    if (lpInput->lpDDSurface->lpGbl->dwGlobalFlags & DDRAWISURFGBL_SOFTWAREAUTOFLIP)
    {
        DPF("Autoflipping in software");
    }
    lpInput->dwSurfaceOffset = lpInput->lpDDSurface->lpGbl->fpVidMem - sData->dwScreenFlatAddr;
    lpInput->dwOverlayOffset = lpInput->dwSurfaceOffset + sData->dwUpdateOverlayFlipOffset;

    DPF("SyncSurfaceData dwOverlayOffset=%lx", lpInput->dwOverlayOffset);
    return DDHAL_DRIVER_HANDLED;
}
/*
 ** ColorControl32
 *
 *  FILENAME: C:\win9x\display\mini\s3\S3_DD32.C
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
 *  HISTORY: 02031997 Laurie Thompson Created
 *
 */
DWORD __stdcall ColorControl32(LPDDHAL_COLORCONTROLDATA lpInput)
{
    LPS3THKINFO sData = GetMyRefData(lpInput->lpDD);
    DWORD dwReg;

    DPF("ColorControl");

    if ( COLOR_ADJUST_HW_PRESENT )
      if( ((sData->lpSpod->dwS_control & 0x07000000) == 0x01000000) ||
          ((sData->lpSpod->dwS_control & 0x07000000) == 0x02000000) )   /* is it YUV data? */
      {
         if (lpInput->dwFlags == DDRAWI_GETCOLOR)
         {
            dwReg = mmdwGetRegister(COLOR_ADJUST_REG) & 0xFF1F9FFF;
            if (dwReg & 0x00008000)  //brightness and contrast enabled, so use real values
            {  if (lpInput->lpColorData->dwFlags & DDCOLOR_BRIGHTNESS)
                  lpInput->lpColorData->lBrightness = HWtoDD_Bright(dwReg & 0x000000FF);
               if (lpInput->lpColorData->dwFlags & DDCOLOR_CONTRAST)
                  lpInput->lpColorData->lContrast = HWtoDD_Cont(dwReg & 0x00001F00);
            }
            else                 //not enabled, so return 'no effect' values
            {  lpInput->lpColorData->lBrightness = DD_DEF_BRIGHT;
               lpInput->lpColorData->lContrast  = DD_DEF_CONT;
            }

            if (dwReg & 0x80000000)  //saturation and hue enabled, so use real values
            {  if (lpInput->lpColorData->dwFlags & DDCOLOR_SATURATION)
                 lpInput->lpColorData->lSaturation = HWtoDD_Sat(dwReg & 0x1F1F0000);
               if (lpInput->lpColorData->dwFlags & DDCOLOR_HUE)
                 lpInput->lpColorData->lHue = HWtoDD_Hue(dwReg & 0x1F1F0000);
            }
            else                 //not enabled, so return 'no effect' values
            {  lpInput->lpColorData->lSaturation = DD_DEF_SAT;
               lpInput->lpColorData->lHue   = DD_DEF_HUE;
            }

            /* Turn off flags we do not support */
            lpInput->lpColorData->dwFlags &=
                ~(DDCOLOR_GAMMA | DDCOLOR_COLORENABLE | DDCOLOR_SHARPNESS);
         } /* end if GetColor */

         else if (lpInput->dwFlags == DDRAWI_SETCOLOR)
         {  DWORD dwReg;
            dwReg = mmdwGetRegister(COLOR_ADJUST_REG);

            /* if Brightness or Contrast need set, do both together */
            if (lpInput->lpColorData->dwFlags &
                                          (DDCOLOR_BRIGHTNESS | DDCOLOR_CONTRAST) )
            {  /* In case no value is given for one of them, use defaults */
               LONG lDDBright = DD_DEF_BRIGHT;
               LONG lDDCont   = DD_DEF_CONT;

               if (lpInput->lpColorData->dwFlags & DDCOLOR_BRIGHTNESS)
                  lDDBright = lpInput->lpColorData->lBrightness;
               if (lpInput->lpColorData->dwFlags & DDCOLOR_CONTRAST)
                  lDDCont = lpInput->lpColorData->lContrast;

               mmdwSetRegister( COLOR_ADJUST_REG,
               /*clear B and C*/   (dwReg & 0xFFFF0000) |
               /*or in values*/    DDtoHW_BrightCont(lDDBright, lDDCont) |
               /*set enable bit*/  0x00008000) ;
            }

            /* if Hue or Saturation need set, do both together */
            if (lpInput->lpColorData->dwFlags &
                                          (DDCOLOR_HUE | DDCOLOR_SATURATION) )
            {  /* In case no value is given for one of them, use defaults */
               LONG lDDHue = DD_DEF_HUE;
               LONG lDDSat = DD_DEF_SAT;

               if (lpInput->lpColorData->dwFlags & DDCOLOR_HUE)
                  lDDHue = lpInput->lpColorData->lHue;
               if (lpInput->lpColorData->dwFlags & DDCOLOR_SATURATION)
                  lDDSat = lpInput->lpColorData->lSaturation;

               mmdwSetRegister( COLOR_ADJUST_REG,
               /*clear H and S*/   (dwReg & 0x0000FFFF) |
               /*or in values*/    DDtoHW_HueSat(lDDHue, lDDSat) |
               /*set enable bit*/  0x80000000) ;
            }
         } /* end if SetColor */
         else  /* not Get or Set Color */
           lpInput->ddRVal = DDERR_INVALIDPARAMS;
       }
       else /* not YUV surface */
         lpInput->ddRVal = DDERR_INVALIDOBJECT;
    else  /* no Color Control HW */
      lpInput->ddRVal = DDERR_UNSUPPORTED;
    return DDHAL_DRIVER_HANDLED;
}

#endif //DX5

