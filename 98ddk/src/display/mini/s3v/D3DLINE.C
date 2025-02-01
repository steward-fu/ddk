/*==========================================================================;
 *
 *  Copyright (C) 1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3dline.c
 *  Content:    Direct3D line rasterization code.
 *
 ***************************************************************************/

#include "precomp.h"
#include "d3ddrv.h"

#ifndef WRAP
// texture wrapping
_inline int _stdcall F32toS32( float f )
{
    _asm {
            fld     [f]
            fistp   [f]
    }
    return *(int*)&f;
}
_inline float _stdcall DblRound( float f )
{
    _asm {
            fld qword ptr    [f]
            frndint
            fstp    [f]
    }
    return *(float*)&f;
}

#define WRAP( p )                                       \
{                                                       \
    _PRECISION rp;                                      \
                                                        \
    rp = p##0 - p##1;                               \
    if( rp > (_PRECISION)0.5 )                      \
    {                                                   \
        p##1 += (_PRECISION)1.0;                        \
    }                               \
    else                            \
    if( rp < (_PRECISION)(-0.5) )               \
    {                                                   \
       p##0 += (_PRECISION)1.0;                     \
    }                               \
}

#endif

/*
 *
 *
 *
 */
void GenericLine( S3_CONTEXT *ctxt,
                  LPD3DTLVERTEX    p0, 
                  LPD3DTLVERTEX    p1, 
                  LPD3DTLVERTEX    pFlat)
{
    int iDy01;
    _PRECISION fDxr, fDx;
    _PRECISION fDyCC;
    _PRECISION fDy01, fDx01;
    _PRECISION fDxDy01, fDy01r;
    //_PRECISION fArea;
    int i1y, i0y;
    D3DCOLOR Color1, Color0;
    _PRECISION u1, u0;
    _PRECISION v1, v0;
    _PRECISION fDuDy, fDvDy;
    _PRECISION w1, w0, uBase, vBase;
    _PRECISION fDwY;
    double fDzY;
    double fRdY, fGdY, fBdY, fAdY;
    BYTE As;
    BOOL PerspectiveChange = FALSE,bXMajor;
    LPS3THKINFO sData=ctxt->sData;
    char *_TRI_BASE=(char*)sData->D3DGlobals.g_p3DTri;

    /* order the vertices */
    if( p1->sy > p0->sy )
        DWSWAP( p1, p0 );
    /* calculate integer y deltas */
    i1y = MYFLINT(p1->sy);
    iDy01 = (i0y = MYFLINT(p0->sy)) - i1y;
    fDy01 = p0->sy - p1->sy;
    if( iDy01 == 0 )
    {
        iDy01 = 1;
        fDy01 = 1.0f;
    }
    fDyCC = p0->sy - (float)i0y;
    fDxDy01 = (p1->sx - p0->sx)/(float)iDy01;
    fDx01 = fDxDy01;
    fDx = p1->sx - p0->sx;
    if( fDx < -1/524188. )
    {
        if( -fDx < fDy01 )
            fDx01 = -1.0f;
        fDxr = -1.0f / fDx;
    }
    else if (fDx > 1/524188.)
    {
        if( fDx < fDy01 )
            fDx01 = 1.0f;
        fDxr = 1.0f / fDx;
    }
    else            //fix missing vertical lines
    {
        fDx = fDxr = 1.0f;
        if( 1.0f < fDy01 )
            fDx01 = 1.0f;
    }
    if (fDy01>1.0f)
    {
        fDy01r = 1.0f/(float)iDy01;
    bXMajor=FALSE;
    } 
    else
    {
    fDy01r = fDxr;
    bXMajor=TRUE;
    }
    _WRITE_HEADER( DATA_SIZE + HDR_SIZE, ( ( ( ( TRI_3D + TRI_3D_DATA )& 0xFFFC) << 14) | ( DATA_SIZE >> 2 ) ));

    if( ctxt->dwTexture )
    {
        /* Textures */
        u1 = p1->tu;
        u0 = p0->tu;
        v1 = p1->tv;
        v0 = p0->tv;
        if( ctxt->TextureAddress == D3DTADDRESS_WRAP )
        {
        if( ctxt->bWrapU )
                WRAP( u );
            if( ctxt->bWrapV )
                WRAP( v );
        }
        if( !ctxt->bPerspective || p0->rhw == p1->rhw )
        {
        if (ctxt->bPerspective)
        {
                    PerspectiveChange = TRUE;
                    ctxt->dTexValtoInt = dTexValtoInt[ctxt->rsfMaxMipMapLevel];
                    NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 2 );
                    _WRITE_REG_TRI_LONG( TRI_3D_bU, 0 );
                    _WRITE_REG_TRI_LONG( TRI_3D_bV, 0 );
        }
            u1 *= ctxt->fTextureWidth;
            u0 *= ctxt->fTextureWidth;
            v1 *= ctxt->fTextureHeight;
            v0 *= ctxt->fTextureHeight;
            NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 9 );
            fDvDy = (v1-v0)*fDy01r;
            fDuDy = (u1-u0)*fDy01r;
            _WRITE_REG_TRI_LONG( TRI_3D_dDdX, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dDdY, 0 );
         if (bXMajor)
         {
            _WRITE_REG_TRI_LONG( TRI_3D_dVdY, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dUdY, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dVdX, FLOAT_TO_TEXPOINT(ctxt,fDvDy) );
            _WRITE_REG_TRI_LONG( TRI_3D_dUdX, FLOAT_TO_TEXPOINT(ctxt,fDuDy) );
            }
         else
         {
            _WRITE_REG_TRI_LONG( TRI_3D_dVdX, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dUdX, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dVdY, FLOAT_TO_TEXPOINT(ctxt,fDvDy) );
            _WRITE_REG_TRI_LONG( TRI_3D_dUdY, FLOAT_TO_TEXPOINT(ctxt,fDuDy) );
            }
            _WRITE_REG_TRI_LONG( TRI_3D_DS, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_VS,
                FLOAT_TO_TEXPOINT(ctxt,v0 + fDvDy*fDyCC + 0.501) );
            _WRITE_REG_TRI_LONG( TRI_3D_US,
                FLOAT_TO_TEXPOINT(ctxt,u0 + fDuDy*fDyCC + 0.501) );
        }
        else
        {
        int iMagic, iTextureSize;
        _PRECISION Magic;
            u1 *= ctxt->fTextureWidth;
            u0 *= ctxt->fTextureWidth;
            v1 *= ctxt->fTextureHeight;
            v0 *= ctxt->fTextureHeight;
            w1 = p1->rhw; w0 = p0->rhw;
        // setup dynamic MAGIC number for normalizing w's
        // the formula was found by trial and error
        // texture size * Magic = 32768
        // log2(texture size) + log2(Magic) = 15
        iTextureSize = (ctxt->rndCommand & 0x00000f00) >> 8;
        iMagic = 1 << (15 - iTextureSize);
        Magic = (_PRECISION)iMagic;
            if ( u0 < u1  )
            {
                uBase = u0;
            }
            else
            {
                uBase = u1;
            }
            if ( v0 < v1 )
            {
                vBase = v0;
            }
            else
            {
                vBase = v1;
            }
       //uBase = (int)uBase;
       // vBase = (int)vBase;
       uBase = DblRound(uBase);
       vBase = DblRound(vBase);

                //find max{w[i]} and scale all w with MAGIC/wMAX
                if ( w0 > w1 )
                {
              w0 = Magic;
              w1 = w1 * Magic / w0;
                }
                else
                {
              w1 = Magic;
              w0 = w0 * Magic / w1;
                }

            u0 = ( u0 - uBase ) * w0;
            u1 = ( u1 - uBase ) * w1;
            v0 = ( v0 - vBase ) * w0;
            v1 = ( v1 - vBase ) * w1;

            uBase -= sData->D3DGlobals.coord_adj;
            vBase -= sData->D3DGlobals.coord_adj;

            while ( uBase < 0 )
            {
                uBase += ctxt->fTextureWidth;
            }
            while ( vBase < 0 )
            {
                vBase += ctxt->fTextureHeight;
            }

            NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 14 );
            _WRITE_REG_TRI_LONG( TRI_3D_bV, FLOAT_TO_TEXPOINT(ctxt,vBase) );
            _WRITE_REG_TRI_LONG( TRI_3D_bU, ( FLOAT_TO_TEXPOINT(ctxt,uBase) | sData->D3DGlobals.uBaseHigh ) );
            _WRITE_REG_TRI_LONG( TRI_3D_dDdX, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dDdY, 0 );
            fDwY = (w1-w0)*fDy01r;
            fDvDy = (v1-v0)*fDy01r;
            fDuDy = (u1-u0)*fDy01r;
         if (bXMajor)
         {
            _WRITE_REG_TRI_LONG( TRI_3D_dWdY, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dVdY, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dUdY, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dWdX, FLOAT_TO_1319(fDwY) );
            _WRITE_REG_TRI_LONG( TRI_3D_dVdX, FLOAT_TO_2012(fDvDy)>>ctxt->sData->D3DGlobals.DXGX);
            _WRITE_REG_TRI_LONG( TRI_3D_dUdX, FLOAT_TO_2012(fDuDy)>>ctxt->sData->D3DGlobals.DXGX);
            }
         else
         {
            _WRITE_REG_TRI_LONG( TRI_3D_dWdX, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dVdX, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dUdX, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_dWdY, FLOAT_TO_1319(fDwY) );
            _WRITE_REG_TRI_LONG( TRI_3D_dVdY, FLOAT_TO_2012(fDvDy)>>ctxt->sData->D3DGlobals.DXGX);
            _WRITE_REG_TRI_LONG( TRI_3D_dUdY, FLOAT_TO_2012(fDuDy)>>ctxt->sData->D3DGlobals.DXGX);
         }
            _WRITE_REG_TRI_LONG( TRI_3D_DS, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_WS, FLOAT_TO_1319(w0 + fDwY*fDyCC) );
            _WRITE_REG_TRI_LONG( TRI_3D_VS, FLOAT_TO_2012(v0 + fDvDy*fDyCC )>>ctxt->sData->D3DGlobals.DXGX);
            _WRITE_REG_TRI_LONG( TRI_3D_US, FLOAT_TO_2012(u0 + fDuDy*fDyCC )>>ctxt->sData->D3DGlobals.DXGX);
        }
    }

    /* Color setup */
    if( (ctxt->ShadeMode == D3DSHADE_GOURAUD) )
    {
            if(ctxt->Alpha_workaround)
            {
               Color1 = p1->color | 0xff000000;
               Color0 = p0->color | 0xff000000;
            }
         else
            {
               Color1 = p1->color;
               Color0 = p0->color;
            }
        //As = MYRGB_GETALPHA(p0->color);

        if(ctxt->bSpecular)
        {   
            As = 255 - MYRGB_GETSPECULAR(p0->specular);
            fAdY = (double)(MYRGB_GETSPECULAR(p0->specular) -
                MYRGB_GETSPECULAR(p1->specular)) * fDy01r;
        
        }
        else if( ctxt->bFogEnabled )
        {
            fAdY = (double)(MYRGB_GETALPHA(p1->specular) -
                MYRGB_GETALPHA(p0->specular)) * fDy01r;
            As = MYRGB_GETALPHA(p0->specular);
        }
        else if(ctxt->rndCommand & cmdALP_BLD_CTL_SrcAlph)
        {  
            fAdY = (double)(MYRGB_GETALPHA(Color1) -
                MYRGB_GETALPHA(Color0)) * fDy01r;
            As = MYRGB_GETALPHA(Color0);
        }
        else
        {
            As = 255; fAdY = 0.0;
        }


//        if( ctxt->bSpecular )
//        {
//            CLAMP888( Color1, Color1, p1->specular );
//            CLAMP888( Color0, Color0, p0->specular );
//        }


        fGdY = (double)(MYRGB_GETGREEN(Color1) - MYRGB_GETGREEN(Color0)) * fDy01r;
        fBdY = (double)(MYRGB_GETBLUE(Color1) - MYRGB_GETBLUE(Color0)) * fDy01r;
        fRdY = (double)(MYRGB_GETRED(Color1) - MYRGB_GETRED(Color0)) * fDy01r;

        NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 6 );
     if (bXMajor)
     {
        _WRITE_REG_TRI_LONG( TRI_3D_dGdY_dBdY, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_dAdY_dRdY, 0);
        _WRITE_REG_TRI_LONG( TRI_3D_dGdX_dBdX, (FLOAT_TO_87( fGdY ) << 16) |
                                              (FLOAT_TO_87( fBdY ) & 0xFFFF) );
        _WRITE_REG_TRI_LONG( TRI_3D_dAdX_dRdX, (FLOAT_TO_87( fAdY ) << 16) |
                                              (FLOAT_TO_87( fRdY ) & 0xFFFF) );
     }
     else
     {
        _WRITE_REG_TRI_LONG( TRI_3D_dGdX_dBdX, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_dAdX_dRdX, 0);
        _WRITE_REG_TRI_LONG( TRI_3D_dGdY_dBdY, (FLOAT_TO_87( fGdY ) << 16) |
                                              (FLOAT_TO_87( fBdY ) & 0xFFFF) );
        _WRITE_REG_TRI_LONG( TRI_3D_dAdY_dRdY, (FLOAT_TO_87( fAdY ) << 16) |
                                              (FLOAT_TO_87( fRdY ) & 0xFFFF) );
         }
    }
    else
    {
       if(ctxt->Alpha_workaround)
       {
            Color0 = pFlat->color | 0xff000000;
       }
       else
            Color0 = pFlat->color; 
            
        
        if(ctxt->bSpecular)
        {
            As = 255 - MYRGB_GETSPECULAR(pFlat->specular);
        }
        else if( ctxt->bFogEnabled )
        {
            As = MYRGB_GETALPHA(pFlat->specular);
        }
        else if(ctxt->rndCommand & cmdALP_BLD_CTL_SrcAlph)
        {
            As = MYRGB_GETALPHA(Color0);
        }
        else
            As = 255;    
//        if( ctxt->bSpecular )
//        {
//                CLAMP888( Color0, Color0, pFlat->specular );
//        }

        NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 6 );
        _WRITE_REG_TRI_LONG( TRI_3D_dGdX_dBdX, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_dAdX_dRdX, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_dGdY_dBdY, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_dAdY_dRdY, 0 );
    }

    _WRITE_REG_TRI_LONG( TRI_3D_GS_BS,
        (COLOR_TO_87( MYRGB_GETGREEN(Color0) ) << 16) |
         COLOR_TO_87( MYRGB_GETBLUE(Color0) ) );
    _WRITE_REG_TRI_LONG( TRI_3D_AS_RS,
        (COLOR_TO_87( As ) << 16) |
         COLOR_TO_87( MYRGB_GETRED(Color0) )  );

    /* Z setup */
    if( ctxt->bZEnabled )
    {
        NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 3 );
        fDzY = (p1->sz - p0->sz) * fDy01r;
     if (bXMajor)
        {
        _WRITE_REG_TRI_LONG( TRI_3D_dZdY, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_dZdX, MYFLINT31( fDzY ) );
     }
     else
     {
        _WRITE_REG_TRI_LONG( TRI_3D_dZdX, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_dZdY, MYFLINT31( fDzY ) );
        }
        _WRITE_REG_TRI_LONG( TRI_3D_ZS02, MYFLINT31( p0->sz + (fDyCC * fDzY)) );
    }


    /* Draw the line */

    NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 10 );
    _WRITE_REG_TRI_LONG( TRI_3D_dXdY12, 0 );
    _WRITE_REG_TRI_LONG( TRI_3D_XEnd12, 0 );
    _WRITE_REG_TRI_LONG( TRI_3D_dXdY01, FLOAT_TO_1220(fDxDy01) );
    _WRITE_REG_TRI_LONG( TRI_3D_XEnd01, FLOAT_TO_1220( p0->sx + fDx01 ) );
    _WRITE_REG_TRI_LONG( TRI_3D_dXdY02, FLOAT_TO_1220( fDxDy01 ) );
    _WRITE_REG_TRI_LONG( TRI_3D_XStart02, FLOAT_TO_1220( p0->sx ) );
    _WRITE_REG_TRI_LONG( TRI_3D_YStart, i0y );
    _WRITE_REG_TRI_LONG( TRI_3D_Y01_Y12,
        (iDy01 << 16) | ((fDx > 0) ? 0x80000000 : 0) );
    // DPF(" %c %c", fDx > 0 ? '+' : '-', fArea > 0 ? '+' : '-' );

    _ADVANCE_INDEX( DATA_SIZE );
    _WRITE_HEADER( CMD_SIZE + HDR_SIZE, ( ( ( ( TRI_3D + CMD_OFFSET )& 0xFFFC) << 14) | ( CMD_SIZE >> 2 ) ));
    if ( PerspectiveChange ){
        _WRITE_REG_SET_LONG( sData->D3DGlobals.CMD_SET, ctxt->rndCommand & ~(cmdCMD_TYPE_UnlitTexPersp - cmdCMD_TYPE_UnlitTex) );
       ctxt->dTexValtoInt = dTexValtoIntPerspective[ctxt->rsfMaxMipMapLevel];
     }
     else
        _WRITE_REG_SET_LONG( sData->D3DGlobals.CMD_SET, ctxt->rndCommand );
    _ADVANCE_INDEX( CMD_SIZE );
    _START_DMA;


}

/*
 * Draw a list of wire framed triangle
 */
void GenericWireFrame ( S3_CONTEXT *ctxt,
            LPD3DINSTRUCTION pI,
            LPD3DTLVERTEX    pV,
            LPD3DTRIANGLE    pT)
{
    LPD3DTLVERTEX p2, p1, p0;
    int iCount;

    for( iCount = pI->wCount; iCount > 0; iCount-- )
    {
        p0 = pV + pT->v1;
        p1 = pV + pT->v2;
        p2 = pV + pT->v3;
        /* cull triangle if reqd */
       if (!Culling(p0,p1,p2))
       {
        if( pT->wFlags & D3DTRIFLAG_EDGEENABLE1 )
            GenericLine( ctxt, p0, p1, p0 );
        if( pT->wFlags & D3DTRIFLAG_EDGEENABLE2 )
            GenericLine( ctxt, p1, p2, p0 );
        if( pT->wFlags & D3DTRIFLAG_EDGEENABLE3 )
            GenericLine( ctxt, p2, p0, p0 );
    }
       pT = (LPD3DTRIANGLE)((char *)pT + pI->bSize);
    }
}


// draw ONE wireframe triangle
BOOL GenericOneWireFrame ( S3_CONTEXT *ctxt,
            LPD3DTLVERTEX    p0,
            LPD3DTLVERTEX    p1,
            LPD3DTLVERTEX    p2)
{
       GenericLine( ctxt, p0, p1, p0 );
       GenericLine( ctxt, p1, p2, p0 );
       GenericLine( ctxt, p2, p0, p0 );
    return  FALSE;
}

void GenericLines( LPS3_CONTEXT ctxt,
                   LPD3DINSTRUCTION pI,
                   LPD3DTLVERTEX    pV,
                   LPD3DLINE        pL )
{
    LPD3DTLVERTEX pA, pB;
    int iCount;

    for( iCount = pI->wCount; iCount != 0; iCount-- )
    {
        pA = pV + pL->v1;
        pB = pV + pL->v2;
        pL = (LPD3DLINE)((char *)pL + pI->bSize);
        GenericLine( ctxt, pA, pB, pA );
    }
}
