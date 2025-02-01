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

_inline int _stdcall F32toS32( float f )
{
    _asm {
            fld     [f]
            fistp   [f]
    }
    return *(int*)&f;
}
#ifndef WRAP


#define WRAP( p )                                       \
{                                                       \
    _PRECISION lp, rp, mp;                              \
                                                        \
    if( (lp = p##1 - p##2) < 0 )                        \
        lp = -lp;                                       \
    if( (rp = p##0 - p##1) < 0 )                        \
        rp = -rp;                                       \
    if( (mp = p##2 - p##0) < 0 )                        \
        mp = -mp;                                       \
    if( lp > (_PRECISION)0.5 && rp > (_PRECISION)0.5 )  \
    {                                                   \
        if( p##1 < p##0 )                               \
            p##1 += (_PRECISION)1.0;                    \
        else                                            \
        {                                               \
            p##0 += (_PRECISION)1.0;                    \
            p##2 += (_PRECISION)1.0;                    \
        }                                               \
    }                                                   \
    else if( rp > (_PRECISION)0.5 && mp > (_PRECISION)0.5 )     \
    {                                                   \
        if( p##0 < p##2 )                               \
            p##0 += (_PRECISION)1.0;                    \
        else                                            \
        {                                               \
            p##2 += (_PRECISION)1.0;                    \
            p##1 += (_PRECISION)1.0;                    \
        }                                               \
    }                                                   \
    else if( mp > (_PRECISION)0.5 && lp > (_PRECISION)0.5 )     \
    {                                                   \
        if( p##2 < p##1 )                               \
            p##2 += (_PRECISION)1.0;                    \
        else                                            \
        {                                               \
            p##1 += (_PRECISION)1.0;                    \
            p##0 += (_PRECISION)1.0;                    \
        }                                               \
    }                                                   \
}

#endif



void GenericPoint( S3_CONTEXT *ctxt,
                  LPD3DTLVERTEX    p0,
                  LPD3DTLVERTEX    p1)
{
    int i0y;
    D3DCOLOR Color0;
    _PRECISION u0;
    _PRECISION v0;
    _PRECISION w0, uBase, vBase;
    BYTE As;
    LPS3THKINFO     sData = ctxt->sData;
    char *_TRI_BASE=(char*)sData->D3DGlobals.g_p3DTri;


    i0y = MYFLINT(p0->sy);

    _WRITE_HEADER( DATA_SIZE + HDR_SIZE, ( ( ( ( TRI_3D + TRI_3D_DATA )& 0xFFFC) << 14) | ( DATA_SIZE >> 2 ) ));

#if 0      //useless
    if( (ctxt->dwTexture && !ctxt->bPerspective) ||
        (ctxt->bFogEnabled && !ctxt->dwTexture) )
    {
        NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 2 );
        _WRITE_REG_TRI_LONG( TRI_3D_bV, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_bU, ( sData->D3DGlobals.uBaseHigh ) );
    }
#endif

    if( ctxt->dwTexture )
    {
        /* Textures */
        u0 = p0->tu;
        v0 = p0->tv;
        if( ctxt->TextureAddress == D3DTADDRESS_WRAP )
        {
        //    if( ctxt->bWrapU )
        //        WRAP( u );
        //    if( ctxt->bWrapV )
        //        WRAP( v );
        }
        u0 *= ctxt->fTextureWidth;
        v0 *= ctxt->fTextureHeight;
        if( !ctxt->bPerspective )
        {
            NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 3 );
            _WRITE_REG_TRI_LONG( TRI_3D_DS, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_VS,
                FLOAT_TO_TEXPOINT(ctxt,v0 + 0.501) );
            _WRITE_REG_TRI_LONG( TRI_3D_US,
                FLOAT_TO_TEXPOINT(ctxt,u0 + 0.501) );
        }
        else
        {
            w0 = p0->rhw;
            uBase = u0;
            vBase = v0;
            while ( uBase < 0 )
            {
                uBase += ctxt->fTextureWidth;
            }
            while ( vBase < 0 )
            {
                vBase += ctxt->fTextureHeight;
            }
            NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 6 );

            _WRITE_REG_TRI_LONG( TRI_3D_bV, FLOAT_TO_TEXPOINT(ctxt,vBase) );
            _WRITE_REG_TRI_LONG( TRI_3D_bU, ( FLOAT_TO_TEXPOINT(ctxt,uBase) | sData->D3DGlobals.uBaseHigh ) );
            _WRITE_REG_TRI_LONG( TRI_3D_WS, FLOAT_TO_1319(w0) );
            _WRITE_REG_TRI_LONG( TRI_3D_DS, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_VS, 0 );
            _WRITE_REG_TRI_LONG( TRI_3D_US, 0 );
        }
    }
#if 0
    else if( ctxt->bFogEnabled )
    {
        // special case fogged without texture

        NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 3 );
        _WRITE_REG_TRI_LONG( TRI_3D_DS, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_VS, 0 );
        _WRITE_REG_TRI_LONG( TRI_3D_US, 0 );
    }
#endif
    
    /* Color setup */
    Color0 = p0->color;

#if 0    
    if( ctxt->bSpecular )
    {
        CLAMP888( Color0, p0->color, p0->specular );
    }
    else
    {
        Color0 = p0->color;
    }
#endif

    if (ctxt->bSpecular)
    	{
             As = 255 - MYRGB_GETSPECULAR(p0->specular);
    	}
    else
    if( (ctxt->ShadeMode == D3DSHADE_GOURAUD) )
    {
        if( ctxt->bFogEnabled )
        {
            As = MYRGB_GETALPHA(p0->specular);
        }
        else
        {
            As = MYRGB_GETALPHA(p0->color);
        }
    }
    else    // flat shadeing
    {
        if( ctxt->bFogEnabled )
        {
            As = MYRGB_GETALPHA(p0->specular);
        }
        else
        {
            As = 255;
        }
    }

    NW_FIFO_WAIT( ctxt->ppdev, ctxt->ppdev->pjMmBase, 8 );
    _WRITE_REG_TRI_LONG( TRI_3D_GS_BS,
        (COLOR_TO_87( MYRGB_GETGREEN(Color0) ) << 16) |
         COLOR_TO_87( MYRGB_GETBLUE(Color0) ) );
    _WRITE_REG_TRI_LONG( TRI_3D_AS_RS,
        (COLOR_TO_87( As ) << 16) |
         COLOR_TO_87( MYRGB_GETRED(Color0) )  );

    /* Z setup */
    if( ctxt->bZEnabled )
    {
        _WRITE_REG_TRI_LONG( TRI_3D_ZS02, MYFLINT31( p0->sz ) );
    }

    _WRITE_REG_TRI_LONG( TRI_3D_XEnd01, FLOAT_TO_1220( p0->sx + 1.0 ) );
    _WRITE_REG_TRI_LONG( TRI_3D_XStart02, FLOAT_TO_1220( p0->sx ) );
    _WRITE_REG_TRI_LONG( TRI_3D_YStart, i0y );
    _WRITE_REG_TRI_LONG( TRI_3D_Y01_Y12, (1 << 16) | 0x80000000 );

    _ADVANCE_INDEX( DATA_SIZE );
    _WRITE_HEADER( CMD_SIZE + HDR_SIZE, ( ( ( ( TRI_3D + CMD_OFFSET )& 0xFFFC) << 14) | ( CMD_SIZE >> 2 ) ));
    _WRITE_REG_SET_LONG( sData->D3DGlobals.CMD_SET, ctxt->rndCommand );
    _ADVANCE_INDEX( CMD_SIZE );
    _START_DMA;
}




/*
 * Draw a wire framed triangle
 */
void GenericPointTriangle ( S3_CONTEXT *ctxt,
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
            GenericPoint( ctxt, p2, p1 );
        if( pT->wFlags & D3DTRIFLAG_EDGEENABLE2 )
            GenericPoint( ctxt, p1, p0 );
        if( pT->wFlags & D3DTRIFLAG_EDGEENABLE3 )
            GenericPoint( ctxt, p0, p2 );
    }
       pT = (LPD3DTRIANGLE)((char *)pT + pI->bSize);
    }
}


void GenericPoints( LPS3_CONTEXT ctxt,
                   LPD3DINSTRUCTION pI,
                   LPD3DTLVERTEX    pV,
                   LPD3DPOINT        pL )
{
    LPD3DTLVERTEX pA;
    int iCount;

    for( iCount = pI->wCount; iCount; iCount-- )
    {
        int iiCount;

        for( iiCount = pL->wCount,pA = pV + pL->wFirst ; iiCount; iiCount--, pA++ )
        {
                GenericPoint( ctxt, pA, pA );
        }
        pL = (LPD3DPOINT)((char *)pL + pI->bSize);
    } /* end for */   
}

