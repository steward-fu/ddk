/*==========================================================================;
*
*  Copyright (C) 1996 Microsoft Corporation.  All Rights Reserved.
*
*  File:       d3drendr.c
*  Content:    Direct3D triangle rasterization code.
*
***************************************************************************/

#include "precomp.h"
#include "d3ddrv.h"
#include "s3data.h"

#define lpMemRegB       ((volatile BYTE  *)sData->lpMMReg)
enum PIXELALPHA
{
    NONE,
    PRIMITIVE_ALPHA,
    TEXTURE_ALPHA,
} PixelAlpha;

extern WORD    g_TriFuncStackAdj[];

double dTexValtoInt[] = {
                                                  //   6.26     // 0      1x1
    TWOPOW(26)+TWOPOW(25),
    TWOPOW(27)+TWOPOW(26),                        //   7.25     // 1      2x2
    TWOPOW(28)+TWOPOW(27),                        //   8.24     // 2      4x4
    TWOPOW(29)+TWOPOW(28),                        //   9.23     // 3      8x8
    TWOPOW(30)+TWOPOW(29),                        //  10.22     // 4     16x16
    TWOPOW(31)+TWOPOW(30),                        //  11.21     // 5     32x32
    TWOPOW(32)+TWOPOW(31),                        //  12.20     // 6     64x64
    TWOPOW(33)+TWOPOW(32),                        //  13.19     // 7    128x128
    TWOPOW(34)+TWOPOW(33),                        //  14.18     // 8    256x256
    TWOPOW(35)+TWOPOW(34)                         //  15.17     // 9    512x512
};


double dTexValtoIntPerspective[] = {
                                                  //  17.15    // 0      1x1
    TWOPOW(37)+TWOPOW(36),
    TWOPOW(38)+TWOPOW(37),                        //  18.14    // 1      2x2
    TWOPOW(39)+TWOPOW(38),                        //  19.13    // 2      4x4
    TWOPOW(40)+TWOPOW(39),                        //  20.12    // 3      8x8
    TWOPOW(41)+TWOPOW(40),                        //  21.11    // 4     16x16
    TWOPOW(42)+TWOPOW(41),                        //  22.10    // 5     32x32
    TWOPOW(43)+TWOPOW(42),                        //  23.9     // 6     64x64
    TWOPOW(44)+TWOPOW(43),                        //  24.8     // 7    128x128
    TWOPOW(45)+TWOPOW(44),                        //  25.7     // 8    256x256
    TWOPOW(46)+TWOPOW(45),                        //  24.6     // 9    512x512
    TWOPOW(47)+TWOPOW(46),
};

/*
* myRenderPrimitive
*/
HRESULT __stdcall SetupPrimitive(S3_CONTEXT *ctxt)
{
        LPS3THKINFO sData=ctxt->sData;
        if( ctxt->bChanged )
        {
            int bytesPerPix;
            LPS3_TEXTURE lpTexture=(LPS3_TEXTURE)ctxt->dwTexture;
            DWORD rndCommand = cmd3D_CMD | cmdHWCLIP_EN;

// must reset all cached values
            ctxt->bChanged = FALSE;
            ctxt->rnd3DSetup.stp_DstSrc_Stride = 0;
            PixelAlpha = NONE;
// check textures

            if( lpTexture )
            {
                int i, val;
                DWORD dwTmp;

                #ifdef WINNT
                if( (lpTexture->lpLcl = EngLockDirectDrawSurface( lpTexture->hTexture )) == NULL) {
                    return (DDERR_CANTLOCKSURFACE);
                }
                #endif

                if( (ctxt->TextureAddress == D3DTADDRESS_WRAP) || (ctxt->TextureAddress == D3DTADDRESS_CLAMP) )
                        rndCommand |= cmdWRAP_EN;


                switch( DDSurf_BitDepth(lpTexture->lpLcl) )
                {
                    case 32 :
                        bytesPerPix = 4;
                        rndCommand |= cmdTEX_CLR_FMT_ARGB8888;
                        break;
                    case 16 :
                        bytesPerPix = 2;
                    #if 0
                        if ( DDSurf_AlphaBitDepth(lpTexture->lpLcl) == 4 )
                            rndCommand |= cmdTEX_CLR_FMT_ARGB4444;
                        else
                            rndCommand |= cmdTEX_CLR_FMT_ARGB1555;
                    #else
                        if ( DDSurf_BlueBitMask(((LPDDRAWI_DDRAWSURFACE_INT)
                                           (lpTexture->lpDDS))->lpLcl) == 0x001f)      //1555
                        {
                            rndCommand |= cmdTEX_CLR_FMT_ARGB1555;
                        }
                        else
                        {
                            rndCommand |= cmdTEX_CLR_FMT_ARGB4444;
                        }
                    #endif
                        break;
                    case 8:
                        bytesPerPix = 1;
                        rndCommand |= cmdTEX_CLR_FMT_8BPP_ClrInd;
                        break;
                }

                ctxt->rnd3DSetup.stp_DstSrc_Stride |= ( DDSurf_Width(lpTexture->lpLcl)*bytesPerPix & 0xFFFFL );
                ctxt->rnd3DSetup.stp_TEX_BASE =
                    (ULONG)( lpTexture->lpLcl->lpGbl->fpVidMem) - ctxt->FrameAddr;

                switch (ctxt->TextureMode)
                {
                    case D3DFILTER_NEAREST:
                        rndCommand |= cmdTEX_FILT_MODE_1Tpp;
                        break;
                    case D3DFILTER_LINEAR:
                        rndCommand |= cmdTEX_FILT_MODE_4Tpp;
                        break;
                    case D3DFILTER_MIPNEAREST:
                        rndCommand |= cmdTEX_FILT_MODE_1MTpp;
                        break;
                    case D3DFILTER_MIPLINEAR:
                        rndCommand |= cmdTEX_FILT_MODE_4MTpp;
                        break;
                    case D3DFILTER_LINEARMIPNEAREST:
                        rndCommand |= cmdTEX_FILT_MODE_2MTpp;
                        break;
                    case D3DFILTER_LINEARMIPLINEAR:
                        rndCommand |= cmdTEX_FILT_MODE_8MTpp;
                        break;
                }

                /* compute Max MipMap Level */
                ctxt->fTextureWidth = (float)(dwTmp = DDSurf_Width(lpTexture->lpLcl));

                // See if the current texture is a mipmap
                if ( (lpTexture->lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP) == DDSCAPS_MIPMAP )
                {   //If so, the real height we should use should be the same as
                    //the width.
                    ctxt->fTextureHeight = DDSurf_Width(lpTexture->lpLcl);
                }
                else
                {   //If not, business as usual
                    ctxt->fTextureHeight = DDSurf_Height(lpTexture->lpLcl);
                }

                for ( i = 0; i < 31; i++ ) {
                    if (  dwTmp & 0x80000000 ) {
                        val = 31 - i;
                        if ( dwTmp & 0x7FFFFFFF ) {
                            val++;
                        }

                        ctxt->rsfMaxMipMapLevel = ( val - 1 );
                        ctxt->dTexValtoInt = !ctxt->bPerspective ?
                            dTexValtoInt[val-1] : dTexValtoIntPerspective[val-1];
                        rndCommand |= ( val << 8 );
                        if (((sData->wDeviceId == D_S3VIRGE) || (sData->wDeviceId ==  D_S3VIRGEVX))
                            )
                            sData->D3DGlobals.coord_adj= 0.499f-0.7f*TEXTURE_FACTOR;   //fixed wrong offset problems
                        else if (sData->wDeviceId == D_S3VIRGEDXGX)
                            sData->D3DGlobals.coord_adj= -TEXTURE_FACTOR*0.99f;
                        else
                            sData->D3DGlobals.coord_adj= 0.000f;
// DPF( "MipMapLevel = %d", val );
                        break;
                    }
                    dwTmp <<= 1;
                }

         //set up texture blend mode
            switch (ctxt->BlendMode)
            {
                    case D3DTBLEND_COPY:
                        rndCommand |= cmdCMD_TYPE_UnlitTex;
                        rndCommand |= cmdLIT_TEX_MODE_Decal;
                        PixelAlpha = TEXTURE_ALPHA;
                        break;
                    case D3DTBLEND_DECAL:
                        rndCommand |= cmdCMD_TYPE_UnlitTex;
                        rndCommand |= cmdLIT_TEX_MODE_Decal;
                        PixelAlpha = TEXTURE_ALPHA;
                        break;
                    case D3DTBLEND_DECALALPHA:
                        rndCommand |= cmdCMD_TYPE_UnlitTex;
                        rndCommand |= cmdLIT_TEX_MODE_Decal;
                        PixelAlpha = PRIMITIVE_ALPHA;      //save for team47
                        //PixelAlpha = TEXTURE_ALPHA;          //for Tmark
                        break;
                    case D3DTBLEND_MODULATE:
                        rndCommand |= cmdCMD_TYPE_LitTex;
                        rndCommand |= cmdLIT_TEX_MODE_Modulate;
                        PixelAlpha = TEXTURE_ALPHA;
                        break;
                    case D3DTBLEND_MODULATEALPHA:
                        rndCommand |= cmdCMD_TYPE_LitTex;
                        rndCommand |= cmdLIT_TEX_MODE_Modulate;
                        PixelAlpha = TEXTURE_ALPHA;  //not very correct, but for hellbender
                        break; 
                    case D3DTBLEND_ADD: 
                        rndCommand |= cmdCMD_TYPE_LitTex;
                        rndCommand |= cmdLIT_TEX_MODE_CmplxRflct;
                        PixelAlpha = PRIMITIVE_ALPHA;
			   break;
                    default:
                        rndCommand |= cmdCMD_TYPE_LitTex;
                        rndCommand |= cmdLIT_TEX_MODE_Modulate;
                        PixelAlpha = PRIMITIVE_ALPHA;
                        break;
            } //texture blend mode



                if (ctxt->bPerspective)
                {
                    rndCommand |=
                        cmdCMD_TYPE_UnlitTexPersp - cmdCMD_TYPE_UnlitTex;

                }

                #ifdef WINNT
                EngUnlockDirectDrawSurface( lpTexture->lpLcl );
                #endif
            }  /* ctxt->dwTexture */
            else
            {
                rndCommand |= cmdCMD_TYPE_Gouraud;
                PixelAlpha = PRIMITIVE_ALPHA;
            }

            //set up alpha value for source and destination blending
        if(ctxt->dwStatus & S3ALPHABLENDENABLE){
            //get alpha from source instead of dest.
            if ( ctxt->SrcBlend == D3DBLEND_SRCALPHA )
            {
                if(PixelAlpha == PRIMITIVE_ALPHA)
                    rndCommand |= cmdALP_BLD_CTL_SrcAlph;
                else if((PixelAlpha == TEXTURE_ALPHA) && (lpTexture))
                {
                        if ( DDSurf_RGBAlphaBitMask(lpTexture->lpLcl) 
                           ) // bug for eggtimer    // fixed ID4 colorkey objects and MotorRacer's shadow
                        {
                            rndCommand |= cmdALP_BLD_CTL_TexAlph;
                        }
						else if (D3DTBLEND_MODULATE == ctxt->BlendMode || 
							D3DTBLEND_MODULATEALPHA == ctxt->BlendMode)
						{
							rndCommand |= cmdALP_BLD_CTL_SrcAlph;
						}	
				}
            }// srcalpha
        }//src and dest blending

            rndCommand &= cmdZ_BUF_MODE_MASK;
            if ( ctxt->lpLclZ && ctxt->bZEnabled)
            {
                ctxt->rnd3DSetup.stp_Z_BASE   = (ULONG)( ctxt->lpLclZ->lpGbl->fpVidMem) - ctxt->FrameAddr;
                ctxt->rnd3DSetup.stp_Z_STRIDE = ComputeAlignedStride(ctxt->lpLclZ);

/* Z-buffer update enable */
                rndCommand |= cmdZ_UP_EN;

// Break out if successfully enabled Z buffering
            }
            else
            {
// Disable if no Z buffer or Z disabled.
                rndCommand |= cmdZ_BUF_OFF;
            }

            if (ctxt->bZWriteEnabled)
                rndCommand |= cmdZ_UP_EN;
            else
                rndCommand &= cmdZ_UP_DIS;

            rndCommand &= cmdZ_BUF_CMP_MASK;

            switch (ctxt->ZCmpFunc) {
                case D3DCMP_NEVER:
                    rndCommand |= cmdZ_BUF_CMP_NeverPass;
                    break;
                case D3DCMP_LESS:
                    rndCommand |= cmdZ_BUF_CMP_ZsLsZfb;
                    break;
                case D3DCMP_EQUAL:
                    rndCommand |= cmdZ_BUF_CMP_ZsEqZfb;
                    break;
                case D3DCMP_LESSEQUAL:
                    rndCommand |= cmdZ_BUF_CMP_ZsLeZfb;
                    break;
                case D3DCMP_GREATER:
                    rndCommand |= cmdZ_BUF_CMP_ZsGtZfb;
                    break;
                case D3DCMP_NOTEQUAL:
                    rndCommand |= cmdZ_BUF_CMP_ZsNeZfb;
                    break;
                case D3DCMP_GREATEREQUAL:
                    rndCommand |= cmdZ_BUF_CMP_ZsGeZfb;
                    break;
                default:
                    rndCommand |= cmdZ_BUF_CMP_AlwaysPass;
                    break;
            }

            //found bug with texture alpha and fog on at same time in MechWarriorII.
        //so we don't enable fogging if texture alpha is enabled.
        //fogging, we let alpha blending override fogging, as hw only supports one at a time
	     if (rndCommand & (~cmdALP_BLD_CTL_MASK))
	     {	
	     	ctxt->bFogEnabled=FALSE;
		ctxt->bSpecular=FALSE;
	     }
	     else
	     if (ctxt->dwStatus & S3FOGENABLE)
	     {
	     	ctxt->bFogEnabled=TRUE;
		ctxt->bSpecular=FALSE;
	     }
	     else
	     if (ctxt->dwStatus & S3SPECULARENABLE)
	     {
	     	ctxt->bFogEnabled=FALSE;
		ctxt->bSpecular=TRUE;
	     }
	     else
	     {
	     	ctxt->bFogEnabled=FALSE;
		ctxt->bSpecular=FALSE;
	     }
	
	     ctxt->RStyle.bFogged=ctxt->bSpecular || ctxt->bFogEnabled;
	     if (ctxt->RStyle.bFogged)
	     {
                rndCommand &= cmdALP_BLD_CTL_MASK; //cleaning alpha blending bits
                rndCommand |= cmdFOG_EN;
                if( !ctxt->dwTexture ) {	
		      ctxt->bChanged= TRUE;		// this force reinitialize next time
                    rndCommand &= ~cmdCMD_TYPE_Gouraud;
                    rndCommand |= cmdCMD_TYPE_LitTex;
                    rndCommand |= cmdLIT_TEX_MODE_Modulate;
                    rndCommand |= cmdTEX_CLR_FMT_ARGB8888;
                    rndCommand |= cmdTEX_FILT_MODE_1Tpp;
                    rndCommand |= 3 << 8;
                    rndCommand |= cmdWRAP_EN;
                    ctxt->rnd3DSetup.stp_DstSrc_Stride |= 8;
                    #ifdef WINNT
                    ctxt->rnd3DSetup.stp_TEX_BASE = ctxt->ppdev->dwWhiteTexture;
                    #else
                    ctxt->rnd3DSetup.stp_TEX_BASE = sData->D3DGlobals.dwWhiteTexture;
                    #endif
                }
            }    /* if( ctxt->bFogEnabled )  */

            switch (DDSurf_BitDepth(ctxt->lpLcl) ) {
                    ctxt->Alpha_workaround = FALSE;
                case 8:
                    rndCommand |= cmdDEST_FMT_8BPP_PAL;
                    break;
                case 16:
                    rndCommand |= cmdDEST_FMT_ZRGB1555;
                    break;
                case 24:
                    rndCommand |= cmdDEST_FMT_RGB888;
                  #if 0
                    if( ((rndCommand & ~cmdALP_BLD_CTL_MASK) != cmdALP_BLD_CTL_SrcAlph)&&
                        ((rndCommand & ~cmdALP_BLD_CTL_MASK) != cmdALP_BLD_CTL_TexAlph) &&
                        !ctxt->bFogEnabled){
                            ctxt->Alpha_workaround = TRUE;
                            rndCommand &= cmdALP_BLD_CTL_MASK;
                            rndCommand |= cmdALP_BLD_CTL_SrcAlph;
                    }
                  #else
                    if( ((rndCommand & ~cmdALP_BLD_CTL_MASK) != cmdALP_BLD_CTL_SrcAlph)&&
                        ((rndCommand & ~cmdALP_BLD_CTL_MASK) != cmdALP_BLD_CTL_TexAlph) &&
                        !ctxt->bFogEnabled && !ctxt->bSpecular){
                            ctxt->Alpha_workaround = TRUE;
                            rndCommand &= cmdALP_BLD_CTL_MASK;
                            rndCommand |= cmdALP_BLD_CTL_SrcAlph;
                    }
                  
                  #endif


                    break;
            }
            ctxt->rnd3DSetup.stp_DstSrc_Stride &= 0xffffL;
            ctxt->rnd3DSetup.stp_DstSrc_Stride |= ( ComputeAlignedStride(ctxt->lpLcl) << 16 );
            ctxt->rnd3DSetup.stp_CLIP_LftRt  =  DDSurf_Width(ctxt->lpLcl) - 1;
            ctxt->rnd3DSetup.stp_CLIP_TopBtm =  DDSurf_Height(ctxt->lpLcl) - 1;

            ctxt->rndCommand = rndCommand;
        }  /* if( ctxt->bChanged ) */

        ctxt->rnd3DSetup.stp_DEST_BASE = ((ULONG) ctxt->lpLcl->lpGbl->fpVidMem) - ctxt->FrameAddr;


        if ( sData->D3DGlobals.dma_possible )
        {
            // for now, just make sure DMA is set up here.
            if ((*(sData->D3DGlobals.g_lpDmaEnableReg) & 1) == 0)
            {                               // DMA not enabled now
                WaitFifoEmpty();
		  WaitForEngineIdle();
                *(sData->D3DGlobals.g_lpDmaEnableReg) = 1;      // enable DMA transfer */
            }
            sData->D3DGlobals.g_DmaIndex = *(sData->D3DGlobals.g_lpWritePtrReg) & _DMAFILTER ;
        }


        _WRITE_HEADER( 0x24 + HDR_SIZE, ( ( ( ( TRI_3D + LINE_TRI_3D_SETUP )& 0xFFFC) << 14) | ( 0x24L >> 2 ) ));

        NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 9 );
        _WRITE_REG_SET_LONG( Z_BASE,       ctxt->rnd3DSetup.stp_Z_BASE );
        _WRITE_REG_SET_LONG( DEST_BASE,    ctxt->rnd3DSetup.stp_DEST_BASE );
        _WRITE_REG_SET_LONG( CLIP_L_R,     ctxt->rnd3DSetup.stp_CLIP_LftRt );
        _WRITE_REG_SET_LONG( CLIP_T_B,     ctxt->rnd3DSetup.stp_CLIP_TopBtm );
        _WRITE_REG_SET_LONG( DEST_SRC_STR, ctxt->rnd3DSetup.stp_DstSrc_Stride );
        _WRITE_REG_SET_LONG( Z_STRIDE,     ctxt->rnd3DSetup.stp_Z_STRIDE );
        _WRITE_REG_SET_LONG( TEX_BASE,     ctxt->rnd3DSetup.stp_TEX_BASE );
        _WRITE_REG_SET_LONG( TEX_BDR_CLR,                   0xFFFFFFFF );
        if (ctxt->bSpecular)
        {
            _WRITE_REG_SET_LONG( DC_FADE_CLR,  0xffffff );
        }
        else
        {
            _WRITE_REG_SET_LONG( DC_FADE_CLR,  ctxt->rnd3DSetup.stp_FOG_CLR );
        }

        _ADVANCE_INDEX( 0x24 );


        #if 0
        DPF( "Z_BASE 0x%08x", ctxt->rnd3DSetup.stp_Z_BASE );
        DPF( "DEST_BASE 0x%08x", ctxt->rnd3DSetup.stp_DEST_BASE );
        DPF( "CLIP_L_R 0x%08x", ctxt->rnd3DSetup.stp_CLIP_LftRt );
        DPF( "CLIP_T_B 0x%08x", ctxt->rnd3DSetup.stp_CLIP_TopBtm );
        DPF( "DEST_SRC_STR 0x%08x", ctxt->rnd3DSetup.stp_DstSrc_Stride );
        DPF( "Z_STRIDE 0x%08x", ctxt->rnd3DSetup.stp_Z_STRIDE );
        DPF( "TEX_BASE 0x%08x", ctxt->rnd3DSetup.stp_TEX_BASE );
        DPF( "TEX_BDR_CLR 0x%08x", ctxt->rnd3DSetup.stp_TEX_BDR_CLR );
        DPF( "DC_FADE_CLR 0x%08x", ctxt->rnd3DSetup.stp_FOG_CLR );
        DPF( "Cmd is %08x", ctxt->rndCommand );
        #endif
     return DD_OK;

};  //SetupPrimitive(S3_CONTEXT *ctxt)

/*
* myRenderPrimitive
*/
DWORD __stdcall myRenderPrimitive(LPD3DHAL_RENDERPRIMITIVEDATA prd) {
/*
* NOTES:
*
* This callback is invoked when a primitive is to be rendered.
* All the data is known to be clipped.
*/  LPS3THKINFO sData;
    LPD3DINSTRUCTION    lpIns;
    LPD3DTLVERTEX       lpVertices,p0,p1,p2;
    LPD3DTRIANGLE      pT;
    LPBYTE              lpData, prim;
    S3_CONTEXT *ctxt;
    int iCount;

    short OldCW, TruncateCW;
    _asm fstcw   OldCW
    TruncateCW = OldCW | 0x0e00;    //PC:Double Precision RC:truncate
    _asm fldcw   TruncateCW

    #ifdef WINNT
    __try {
    #endif
        ctxt = (LPS3_CONTEXT) prd->dwhContext;

        if ( ! ctxt) {
            DPF( "in myRenderPrimitive, bad context =0x08lx", prd->dwhContext);
            prd->ddrval = D3DHAL_CONTEXT_BAD;
            return (DDHAL_DRIVER_HANDLED);
        }
        sData = ctxt->sData;

        if (ctxt->bZVisible) {
/*
* If you don't implement Z visibility testing, just do this.
*/
            prd->dwStatus &= ~D3DSTATUS_ZNOTVISIBLE;
            prd->ddrval = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }
     if ((prd->ddrval=SetupPrimitive(ctxt)) != DD_OK)   return  (DDHAL_DRIVER_HANDLED);
        lpData = (LPBYTE)(DDS_LCL(prd->lpExeBuf)->lpGbl->fpVidMem);
        lpIns = &prd->diInstruction;
        prim = lpData + prd->dwOffset;
        lpVertices = (LPD3DTLVERTEX)((LPBYTE)DDS_LCL(prd->lpTLBuf)->lpGbl->fpVidMem
                     + prd->dwTLOffset);
    //DPF("Entering myRenderPrimitive");

/*
* Parse the instructions.
*/
        switch (lpIns->bOpcode) {
            case D3DOP_POINT:
//DPF( "in myRenderPrimitive, D3DOP_POINT instruction count = %d", lpIns->wCount);
                GenericPoints( ctxt, lpIns, lpVertices, (D3DPOINT*)prim );
                break;
            case D3DOP_LINE:
//DPF( "in myRenderPrimitive, D3DOP_LINE instruction count = %d", lpIns->wCount);
                GenericLines( ctxt, lpIns, lpVertices, (D3DLINE*)prim );
                break;
            case D3DOP_TRIANGLE:
// DPF( "in myRenderPrimitive, D3DOP_TRIANGLELIST instruction count = %d", lpIns->wCount);
        if (ctxt->FillMode == D3DFILL_POINT)
            GenericPointTriangle( ctxt, lpIns, lpVertices, (D3DTRIANGLE*)prim );
        else
        if (ctxt->FillMode == D3DFILL_WIREFRAME)
            GenericWireFrame( ctxt, lpIns, lpVertices, (D3DTRIANGLE*)prim );
        else
        {
            LPRENDERTRIANGLE    pTriangle;
            BYTE noOneLimit = 0;
#if 0
            if((ctxt->RStyle.bTextured) && (ctxt->sData->D3DGlobals.bCheckUV) && (ctxt->FillMode != D3DFILL_POINT)){            //changed

                pTriangle=ChopInUVSpace;
            }
             else if((ctxt->RStyle.bTextured) && ( ctxt->fTextureWidth >= ctxt->sData->D3DGlobals.__UVRANGE ))
            {
                pTriangle=ChopInUVSpace;
            }
             else
            {
                pTriangle=pRenderTriangle[ctxt->dwRCode];
            }
#else

            if((ctxt->RStyle.bTextured) && (ctxt->sData->D3DGlobals.bCheckUV) && (ctxt->FillMode != D3DFILL_POINT))
            {            //changed

                pTriangle=ChopInUVSpace;
            }
            else if(ctxt->RStyle.bTextured){ //lynew
                if( ((ctxt->TextureAddress==D3DTADDRESS_WRAP)&&(ctxt->bWrapU)&&(ctxt->bWrapV)) ||
                    (ctxt->TextureAddress==D3DTADDRESS_CLAMP) )
                {   //all u,v value are limited to (0, 1) by WRAP and CLAMP definition
                    if( ctxt->fTextureWidth >= ctxt->sData->D3DGlobals.__UVRANGE )
                       pTriangle=ChopInUVSpace;
                    else
                       pTriangle=pRenderTriangle[ctxt->dwRCode];
                }
                else  //here uv should be bounded by(-128, 128)
                {
                      if( (ctxt->fTextureWidth * 256) <= ctxt->sData->D3DGlobals.__UVRANGE )
                          pTriangle=pRenderTriangle[ctxt->dwRCode];
                      else
                      {
                           pTriangle=ChopInUVSpace;
                           noOneLimit = 1;

                      }
                }
           }
           else
           {
              pTriangle=pRenderTriangle[ctxt->dwRCode];
           }
#endif
            {
                DWORD   saveESP;
                DWORD   stackAlignAdj = g_TriFuncStackAdj[ctxt->dwRCode];
                _asm
                {
                    mov saveESP, esp
                    sub esp, 8
                    and esp, -8
                    sub esp, 8
                    sub esp, stackAlignAdj
                }

                for(/*do_dma = */iCount = lpIns->wCount,pT=(LPD3DTRIANGLE)prim; iCount > 0; iCount-- )
                {
                    p0 = lpVertices + pT->v1;
                    p1 = lpVertices + pT->v2;
                    p2 = lpVertices + pT->v3;
                    pT = (LPD3DTRIANGLE)((char *)pT + lpIns->bSize);

#ifdef  DEBUG
                    if (p0->sx >1024.0 || p0->sy >1024.0 ||
                        p1->sx >1024.0 || p1->sy >1024.0 ||
                        p2->sx >1024.0 || p2->sy >1024.0 ){
                        DPF("In myRenderPrimitive Invalid (%d,%d),(%d,%d),(%d,%d)",(int)p0->sx,(int)p0->sy,(int)p1->sx,(int)p1->sy,(int)p2->sx,(int)p2->sy);
                            continue;
                        }
#endif  //DEBUG
                    if (!Culling(p0,p1,p2))
                    {
                        //for those triangle list which have been decided as need to go through
                        //chop becasuse their uv value may bigger than 1, we check the first and
                        //last triangles in this triangle list. if these 2 triangles don't have to
                        //go through the chop, we let the whole triangle list no going through
                        //the chop.
                        if((noOneLimit == 1))
                        {
                            _PRECISION du01 = (p0->tu - p1->tu)*ctxt->fTextureWidth;
                            noOneLimit = 0;

                            if ((du01 < ctxt->sData->D3DGlobals.__UVRANGE) &&
                                (du01 >-ctxt->sData->D3DGlobals.__UVRANGE) )
                            {
                                _PRECISION du02 = (p0->tu - p2->tu)*ctxt->fTextureWidth;
                                if ((du02 < ctxt->sData->D3DGlobals.__UVRANGE) &&
                                    (du02 >-ctxt->sData->D3DGlobals.__UVRANGE) )
                                {
                                    _PRECISION du12 = (p1->tu - p2->tu)*ctxt->fTextureWidth;
                                    if ((du12 < ctxt->sData->D3DGlobals.__UVRANGE) &&
                                        (du12 >-ctxt->sData->D3DGlobals.__UVRANGE) )
                                    {
                                        _PRECISION ddu01;
                                        LPD3DTLVERTEX  pp0,pp1,pp2;
                                        LPD3DTRIANGLE  ppT;
                                        ppT =(LPD3DTRIANGLE)prim;
                                        ppT = (LPD3DTRIANGLE)((char *)ppT + lpIns->bSize * (iCount-1));
                                        pp0 = lpVertices + ppT->v1;
                                        pp1 = lpVertices + ppT->v2;
                                        pp2 = lpVertices + ppT->v3;
                                        ddu01 = (pp0->tu - pp1->tu)*ctxt->fTextureWidth;

                                        if ((ddu01 < ctxt->sData->D3DGlobals.__UVRANGE) &&
                                            (ddu01 >-ctxt->sData->D3DGlobals.__UVRANGE) )
                                        {
                                            _PRECISION ddu02 = (pp0->tu - pp2->tu)*ctxt->fTextureWidth;

                                            if ((ddu02 < ctxt->sData->D3DGlobals.__UVRANGE) &&
                                                (ddu02 >-ctxt->sData->D3DGlobals.__UVRANGE) )
                                            {
                                                _PRECISION ddu12 = (pp1->tu - pp2->tu)*ctxt->fTextureWidth;
                                                if ((ddu12 < ctxt->sData->D3DGlobals.__UVRANGE) &&
                                                    (ddu12 >-ctxt->sData->D3DGlobals.__UVRANGE) )
                                                {
                                                    pTriangle=pRenderTriangle[ctxt->dwRCode];
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        pTriangle(ctxt,p0,p1,p2);
                    }
                }
                _asm mov esp, saveESP
            }
        }
            break;
            default:
                DPF( "in myRenderPrimitive, Unhandled instruction opcode = %d", lpIns->bOpcode);
        }
        #ifdef WINNT
    }
    __finally {
        ctxt->ppdev->bAfter3D = TRUE;
    }
    #endif

    _asm fldcw OldCW
    //DPF("Leaving myRenderPrimitive");

    prd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myRenderPrimitive */




