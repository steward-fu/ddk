

/*==========================================================================;
*
*  Copyright (C) 1996 Microsoft Corporation.  All Rights Reserved.
*
*  File:       d3dtri.c
*  Content:    Direct3D triangle rasterization code.
*
***************************************************************************/


#include "precomp.h"
#include "d3ddrv.h"
#include "s3data.h"
//#define MAGIC 256

// limit the max. and min. of u and v 

#define LIMIT_UV_HI(uv) if (uv > 1.0e6f) uv = 127.9f;
#define LIMIT_UV_LO(uv) if (uv < -1.0e6f) uv = -128.0f;
#define LIMIT_HI_TO_ONE(uv) if (uv > 0.99f) uv = 0.99f; 
#define LIMIT_LO_TO_ZRO(uv) if (uv < 0.01f) uv = 0.01f;  

BOOL DXGXSmartFilter( LPD3DTLVERTEX pp0, LPD3DTLVERTEX pp1, LPD3DTLVERTEX pp2, DWORD *rndCommand );


_inline int _stdcall F32toS32( float f )
{
    _asm {
            fld     [f]
            fistp   [f]
    }
    return *(int*)&f;
}

#if 0
_inline double _stdcall DblRound( double f )
{
    _asm {
            fld qword ptr    [f]
            frndint
            fstp    [f]
    }
    return *(double*)&f;
} 
#else
_inline float _stdcall DblRound( float f )
{
    _asm {
            fld dword ptr    [f]
            frndint
            fstp    [f]
    }
    return *(float*)&f;
}

#endif 

#undef GOURAUD
#undef DMASUPPORT
#undef ZBUFFER
#undef TEXTURED
#undef PERSPECTIVE
#undef FOGGED
#undef FNAME
#undef WRAPCODE

#define FNAME       ________Triangle
#include "gentri.c"

#define GOURAUD
#define FNAME       G_______Triangle
#include "gentri.c"

#define DMASUPPORT
#define FNAME       _S______Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define FNAME       GS______Triangle
#include "gentri.c"

#define ZBUFFER
#define FNAME       __Z_____Triangle
#include "gentri.c"

#define GOURAUD
#define ZBUFFER
#define FNAME       G_Z_____Triangle
#include "gentri.c"

#define DMASUPPORT
#define ZBUFFER
#define FNAME       _SZ_____Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define ZBUFFER
#define FNAME       GSZ_____Triangle
#include "gentri.c"

#define TEXTURED
#define FNAME       ___T____Triangle
#include "gentri.c"

#define GOURAUD
#define TEXTURED
#define FNAME       G__T____Triangle
#include "gentri.c"

#define DMASUPPORT
#define TEXTURED
#define FNAME       _S_T____Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define TEXTURED
#define FNAME       GS_T____Triangle
#include "gentri.c"

#define ZBUFFER
#define TEXTURED
#define FNAME       __ZT____Triangle
#include "gentri.c"

#define GOURAUD
#define ZBUFFER
#define TEXTURED
#define FNAME       G_ZT____Triangle
#include "gentri.c"

#define DMASUPPORT
#define ZBUFFER
#define TEXTURED
#define FNAME       _SZT____Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define ZBUFFER
#define TEXTURED
#define FNAME       GSZT____Triangle
#include "gentri.c"

#define TEXTURED
#define PERSPECTIVE
#define FNAME       ___TP___Triangle
#include "gentri.c"

#define GOURAUD
#define TEXTURED
#define PERSPECTIVE
#define FNAME       G__TP___Triangle
#include "gentri.c"

#define DMASUPPORT
#define TEXTURED
#define PERSPECTIVE
#define FNAME       _S_TP___Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define TEXTURED
#define PERSPECTIVE
#define FNAME       GS_TP___Triangle
#include "gentri.c"

#define ZBUFFER
#define TEXTURED
#define PERSPECTIVE
#define FNAME       __ZTP___Triangle
#include "gentri.c"

#define GOURAUD
#define ZBUFFER
#define TEXTURED
#define PERSPECTIVE
#define FNAME       G_ZTP___Triangle
#include "gentri.c"

#define DMASUPPORT
#define ZBUFFER
#define TEXTURED
#define PERSPECTIVE
#define FNAME       _SZTP___Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define ZBUFFER
#define TEXTURED
#define PERSPECTIVE
#define FNAME       GSZTP___Triangle
#include "gentri.c"

#define FOGGED
#define FNAME       _____F__Triangle
#include "gentri.c"

#define GOURAUD
#define FOGGED
#define FNAME       G____F__Triangle
#include "gentri.c"

#define DMASUPPORT
#define FOGGED
#define FNAME       _S___F__Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define FOGGED
#define FNAME       GS___F__Triangle
#include "gentri.c"

#define ZBUFFER
#define FOGGED
#define FNAME       __Z__F__Triangle
#include "gentri.c"

#define GOURAUD
#define ZBUFFER
#define FOGGED
#define FNAME       G_Z__F__Triangle
#include "gentri.c"

#define DMASUPPORT
#define ZBUFFER
#define FOGGED
#define FNAME       _SZ__F__Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define ZBUFFER
#define FOGGED
#define FNAME       GSZ__F__Triangle
#include "gentri.c"

#define TEXTURED
#define FOGGED
#define FNAME       ___T_F__Triangle
#include "gentri.c"

#define GOURAUD
#define TEXTURED
#define FOGGED
#define FNAME       G__T_F__Triangle
#include "gentri.c"

#define DMASUPPORT
#define TEXTURED
#define FOGGED
#define FNAME       _S_T_F__Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define TEXTURED
#define FOGGED
#define FNAME       GS_T_F__Triangle
#include "gentri.c"

#define ZBUFFER
#define TEXTURED
#define FOGGED
#define FNAME       __ZT_F__Triangle
#include "gentri.c"

#define GOURAUD
#define ZBUFFER
#define TEXTURED
#define FOGGED
#define FNAME       G_ZT_F__Triangle
#include "gentri.c"

#define DMASUPPORT
#define ZBUFFER
#define TEXTURED
#define FOGGED
#define FNAME       _SZT_F__Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define ZBUFFER
#define TEXTURED
#define FOGGED
#define FNAME       GSZT_F__Triangle
#include "gentri.c"

#define TEXTURED
#define PERSPECTIVE
#define FOGGED
#define FNAME       ___TPF__Triangle
#include "gentri.c"

#define GOURAUD
#define TEXTURED
#define PERSPECTIVE
#define FOGGED
#define FNAME       G__TPF__Triangle
#include "gentri.c"

#define DMASUPPORT
#define TEXTURED
#define PERSPECTIVE
#define FOGGED
#define FNAME       _S_TPF__Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define TEXTURED
#define PERSPECTIVE
#define FOGGED
#define FNAME       GS_TPF__Triangle
#include "gentri.c"

#define ZBUFFER
#define TEXTURED
#define PERSPECTIVE
#define FOGGED
#define FNAME       __ZTPF__Triangle
#include "gentri.c"

#define GOURAUD
#define ZBUFFER
#define TEXTURED
#define PERSPECTIVE
#define FOGGED
#define FNAME       G_ZTPF__Triangle
#include "gentri.c"

#define DMASUPPORT
#define ZBUFFER
#define TEXTURED
#define PERSPECTIVE
#define FOGGED
#define FNAME       _SZTPF__Triangle
#include "gentri.c"

#define GOURAUD
#define DMASUPPORT
#define ZBUFFER
#define TEXTURED
#define PERSPECTIVE
#define FOGGED
#define FNAME       GSZTPF__Triangle
#include "gentri.c"

WORD    g_TriFuncStackAdj[MAXNUMTRIFUNCS];

LPRENDERTRIANGLE pRenderTriangle[] =    //refer to RStyle definition
{
    ________Triangle,   G_______Triangle,   _S______Triangle,   GS______Triangle,
    __Z_____Triangle,   G_Z_____Triangle,   _SZ_____Triangle,   GSZ_____Triangle,
    ___T____Triangle,   G__T____Triangle,   _S_T____Triangle,   GS_T____Triangle,
    __ZT____Triangle,   G_ZT____Triangle,   _SZT____Triangle,   GSZT____Triangle,
    ________Triangle,   G_______Triangle,   _S______Triangle,   GS______Triangle,
    __Z_____Triangle,   G_Z_____Triangle,   _SZ_____Triangle,   GSZ_____Triangle,
    ___TP___Triangle,   G__TP___Triangle,   _S_TP___Triangle,   GS_TP___Triangle,
    __ZTP___Triangle,   G_ZTP___Triangle,   _SZTP___Triangle,   GSZTP___Triangle,
    _____F__Triangle,   G____F__Triangle,   _S___F__Triangle,   GS___F__Triangle,
    __Z__F__Triangle,   G_Z__F__Triangle,   _SZ__F__Triangle,   GSZ__F__Triangle,
    ___T_F__Triangle,   G__T_F__Triangle,   _S_T_F__Triangle,   GS_T_F__Triangle,
    __ZT_F__Triangle,   G_ZT_F__Triangle,   _SZT_F__Triangle,   GSZT_F__Triangle,
    _____F__Triangle,   G____F__Triangle,   _S___F__Triangle,   GS___F__Triangle,
    __Z__F__Triangle,   G_Z__F__Triangle,   _SZ__F__Triangle,   GSZ__F__Triangle,
    ___TPF__Triangle,   G__TPF__Triangle,   _S_TPF__Triangle,   GS_TPF__Triangle,
    __ZTPF__Triangle,   G_ZTPF__Triangle,   _SZTPF__Triangle,   GSZTPF__Triangle,
};

#define FLOATONE    (1.0-1.0e-5)
#define __TRIQUE_FIRST_ALLOC 64
#define UVCLIP_U    1
#define UVCLIP_V    2



#define max(a,b)    (((a) > (b)) ? (a) : (b))
void    LITR(S3_CONTEXT *ctxt,LPD3DTLVERTEX pr,LPD3DTLVERTEX p0,LPD3DTLVERTEX p1,_PRECISION ratio,D3DCOLOR FlatColor,D3DCOLOR FlatSpecular)
{
        _PRECISION  diffUV, sUV, eUV, mUV, prpInXYSpace;
    _PRECISION  diffU,diffV;
    diffU=(_PRECISION)p1->tu-(_PRECISION)p0->tu;
    diffV=(_PRECISION)p1->tv-(_PRECISION)p0->tv;
    pr->tu=(D3DVALUE)((_PRECISION)p0->tu+diffU*ratio);
    pr->tv=(D3DVALUE)((_PRECISION)p0->tv+diffV*ratio);
       pr->rhw=(D3DVALUE)(1.0/(1.0/(_PRECISION)p0->rhw+(1.0/(_PRECISION)p1->rhw-1.0/(_PRECISION)p0->rhw)*ratio));
        if(ctxt->bPerspective){
            if ( fabs(diffU) > fabs(diffV)) {
                sUV = p0->tu * p0->rhw;
                eUV = p1->tu * p1->rhw;
                if ( ( diffUV = eUV - sUV ) != 0.0 ) {
                    mUV = pr->tu * pr->rhw;
                    prpInXYSpace = ( mUV - sUV ) / diffUV;
                }
                else {
                    prpInXYSpace = 0.0f;
                }
            }
            else {
                sUV = p0->tv * p0->rhw;
                eUV = p1->tv * p1->rhw;
                if ( ( diffUV = eUV - sUV ) != 0.0 ) {
                    mUV = pr->tv * pr->rhw;
                    prpInXYSpace = ( mUV - sUV ) / diffUV;
                }
                else {
                    prpInXYSpace = 0.0f;
                }
            }
    }
    else
    {
        prpInXYSpace = ratio;
    }
        pr->sx=(D3DVALUE)((_PRECISION)p0->sx+((_PRECISION)p1->sx-(_PRECISION)p0->sx)*prpInXYSpace);
        pr->sy=(D3DVALUE)((_PRECISION)p0->sy+((_PRECISION)p1->sy-(_PRECISION)p0->sy)*prpInXYSpace);
        pr->sz=(D3DVALUE)((_PRECISION)p0->sz+((_PRECISION)p1->sz-(_PRECISION)p0->sz)*prpInXYSpace);
            if(ctxt->ShadeMode == D3DSHADE_GOURAUD)
            {
                BYTE r,g,b,a;
                r = (BYTE) ((RGBA_GETRED  (p0->color) + MYFLINTUCHAR(prpInXYSpace * ((float) RGBA_GETRED  (p1->color) - RGBA_GETRED  (p0->color))))&0xFF);
                g = (BYTE) ((RGBA_GETGREEN(p0->color) + MYFLINTUCHAR(prpInXYSpace * ((float) RGBA_GETGREEN(p1->color) - RGBA_GETGREEN(p0->color))))&0xFF);
                b = (BYTE) ((RGBA_GETBLUE (p0->color) + MYFLINTUCHAR(prpInXYSpace * ((float) RGBA_GETBLUE (p1->color) - RGBA_GETBLUE (p0->color))))&0xFF);
                a = (BYTE) ((RGBA_GETALPHA(p0->color) + MYFLINTUCHAR(prpInXYSpace * ((float) RGBA_GETALPHA(p1->color) - RGBA_GETALPHA(p0->color))))&0xFF);
                pr->color = RGBA_MAKE( r, g, b, a );
                r = (BYTE) ((RGBA_GETRED  (p0->specular) + MYFLINTUCHAR(prpInXYSpace * ((float) RGBA_GETRED  (p1->specular) - RGBA_GETRED  (p0->specular))))&0xFF);
                g = (BYTE) ((RGBA_GETGREEN(p0->specular) + MYFLINTUCHAR(prpInXYSpace * ((float) RGBA_GETGREEN(p1->specular) - RGBA_GETGREEN(p0->specular))))&0xFF);
                b = (BYTE) ((RGBA_GETBLUE (p0->specular) + MYFLINTUCHAR(prpInXYSpace * ((float) RGBA_GETBLUE (p1->specular) - RGBA_GETBLUE (p0->specular))))&0xFF);
                a = (BYTE) ((RGBA_GETALPHA(p0->specular) + MYFLINTUCHAR(prpInXYSpace * ((float) RGBA_GETALPHA(p1->specular) - RGBA_GETALPHA(p0->specular))))&0xFF);
                pr->specular = RGBA_MAKE( r, g, b, a );
            }
            else {
                pr->color = FlatColor;
                pr->specular = FlatSpecular;
            }
} 

int
OutOfUVRange(
LPD3DTLVERTEX v[ 3 ], float uScale, float vScale, int *piMaxInd,
LPS3THKINFO sData) {
    _PRECISION delt_max_U = 0.0f, delt_max_V = 0.0f;
    int i, j, k;
    int uMax = 0, vMax = 0;
    for ( i=0; i<3; i++ )
    {
        j=(i+1)%3;
        k=(i+2)%3;
        if (v[i]->tu==(D3DVALUE)((int)v[i]->tu))
        {
            if (v[i]->tu>=v[j]->tu && v[i]->tu>=v[k]->tu)   v[i]->tu-=(D3DVALUE)(TEXTURE_FACTOR/uScale);
        }
        if (v[i]->tv==(D3DVALUE)((int)v[i]->tv))
        {
            if (v[i]->tv>=v[j]->tv && v[i]->tv>=v[k]->tv)   v[i]->tv-=(D3DVALUE)(TEXTURE_FACTOR/vScale);
        }
    }
    k = 1;
    for( i = 0; i < 3 ; i++ ) {
        float delt = (float)fabs((v[i]->tu - v[k]->tu ) * uScale);

        if ( delt > delt_max_U ) {
            delt_max_U = delt;
            uMax = i;
        }

        delt = (float)fabs((v[i]->tv - v[k]->tv) * vScale);

        if ( delt > delt_max_V ) {
            delt_max_V = delt;
            vMax = i;
        }
        if (++k == 3)
            k = 0;
    }

    if ( delt_max_U > sData->D3DGlobals.__UVRANGE || delt_max_V > sData->D3DGlobals.__UVRANGE ) {

        if ( delt_max_U > delt_max_V ) {
            *piMaxInd = uMax;
            return(UVCLIP_U);
        }
        else {
            *piMaxInd = vMax;
            return(UVCLIP_V);

        }
    }
    return ( 0 );
}



BOOL ChopInUVSpace ( S3_CONTEXT *ctxt,
                LPD3DTLVERTEX p0,
                LPD3DTLVERTEX p1,
                LPD3DTLVERTEX p2)
{
    _PRECISION  stepratio01,stepratio02,ratio01,ratio02,u12,v12,u02,v02,tempfloat;
    LPD3DTLVERTEX   g_s[3];
    D3DTLVERTEX     tempvtx[4],startvtx[5];
    static      D3DTLVERTEX choplist[__TRIQUE_FIRST_ALLOC];
    D3DCOLOR        FlatColor,FlatSpecular;
    int         g_Flag,g_MaxInd,i,chopcount,newchopcount;
    BOOL        PerspectiveChange=FALSE,StripEnd,Overflow;
    BOOL        Alpha_workaround=ctxt->Alpha_workaround;
    D3DTEXTUREADDRESS TextureAddress=ctxt->TextureAddress;

        tempvtx[0] = *p0;
        tempvtx[1] = *p1;
        tempvtx[2] = *p2;
        g_s[0] = tempvtx;
        g_s[1] = tempvtx+1;
        g_s[2] = tempvtx+2;
    
        // ID4 demo hang and MotoRacer demo hang without these checks.
        // One of the u or v value that passed in has a huge value which
        // causes the following chop algorithm to stay in an infinite loop.
        LIMIT_UV_HI(g_s[0]->tu);
        LIMIT_UV_HI(g_s[1]->tu);
        LIMIT_UV_HI(g_s[2]->tu);
        LIMIT_UV_HI(g_s[0]->tv);
        LIMIT_UV_HI(g_s[1]->tv);
        LIMIT_UV_HI(g_s[2]->tv);
        LIMIT_UV_LO(g_s[0]->tu);
        LIMIT_UV_LO(g_s[1]->tu);
        LIMIT_UV_LO(g_s[2]->tu);
        LIMIT_UV_LO(g_s[0]->tv);
        LIMIT_UV_LO(g_s[1]->tv);
        LIMIT_UV_LO(g_s[2]->tv);


        if( !(ctxt->ShadeMode == D3DSHADE_GOURAUD) )
        {
            FlatColor    = g_s[2]->color= g_s[1]->color= g_s[0]->color;
            FlatSpecular = g_s[2]->specular= g_s[1]->specular= g_s[0]->specular;
        }

        if(Alpha_workaround)
        {
            g_s[0]->color |= 0xff000000;
            g_s[1]->color |= 0xff000000;
            g_s[2]->color |= 0xff000000;
         ctxt->Alpha_workaround=FALSE;
        }

        if((ctxt->bPerspective)&&( g_s[0]->rhw == g_s[1]->rhw) &&
                                 ( g_s[1]->rhw == g_s[2]->rhw ))
        {
            ctxt->bPerspective = FALSE;
            ctxt->RStyle.bPerspective = FALSE;
            PerspectiveChange  = TRUE;
            ctxt->rndCommand  &= ~(cmdCMD_TYPE_UnlitTexPersp - cmdCMD_TYPE_UnlitTex);
            ctxt->dTexValtoInt = dTexValtoInt[ctxt->rsfMaxMipMapLevel];   //useful
        }


        ctxt->TextureAddress=0;
        if( TextureAddress == D3DTADDRESS_WRAP ) {
            float   el,er,em;

            if ( ctxt->bWrapU ) {
            #if 0 
                LIMIT_HI_TO_ONE(g_s[0]->tu);  
                LIMIT_LO_TO_ZRO(g_s[0]->tu);  
                LIMIT_HI_TO_ONE(g_s[1]->tu);  
                LIMIT_LO_TO_ZRO(g_s[1]->tu);  
                LIMIT_HI_TO_ONE(g_s[2]->tu);  
                LIMIT_LO_TO_ZRO(g_s[2]->tu); 
            #endif  
                el = g_s[2]->tu - g_s[0]->tu;
                er = g_s[1]->tu - g_s[2]->tu;
                em = g_s[0]->tu - g_s[1]->tu;

                if ( (el > 0.5f || el < -0.5f) && ( er > 0.5f || er < -0.5f) ) {
                    if ( g_s[2]->tu < g_s[1]->tu ) {
                        g_s[2]->tu += 1.0f;
                    }
                    else {
                        g_s[1]->tu += 1.0f;
                        g_s[0]->tu += 1.0f;
                    }
                }
                else if ( ( er > 0.5f || er < -0.5f ) && ( em > 0.5f || em < -0.5f )) {
                    if ( g_s[1]->tu < g_s[0]->tu ) {
                        g_s[1]->tu += 1.0f;
                    }
                    else {
                        g_s[0]->tu += 1.0f;
                        g_s[2]->tu += 1.0f;
                    }
                }
                else if ( ( em > 0.5f || em < -0.5f ) && ( el > 0.5f || el < -0.5f ) ) {
                    if ( g_s[0]->tu < g_s[2]->tu ) {
                        g_s[0]->tu += 1.0f;
                    }
                    else {
                        g_s[2]->tu += 1.0f;
                        g_s[1]->tu += 1.0f;
                    }
                }
            }
            else{
            #if 0
                LIMIT_UV_HI(g_s[0]->tu);
                LIMIT_UV_HI(g_s[1]->tu);
                LIMIT_UV_HI(g_s[2]->tu);
                LIMIT_UV_LO(g_s[0]->tu);
                LIMIT_UV_LO(g_s[1]->tu);
                LIMIT_UV_LO(g_s[2]->tu);
            #endif
            }

            if ( ctxt->bWrapV ) {
            #if 0
                LIMIT_HI_TO_ONE(g_s[0]->tv);  
                LIMIT_LO_TO_ZRO(g_s[0]->tv);  
                LIMIT_HI_TO_ONE(g_s[1]->tv);  
                LIMIT_LO_TO_ZRO(g_s[1]->tv);  
                LIMIT_HI_TO_ONE(g_s[2]->tv);  
                LIMIT_LO_TO_ZRO(g_s[2]->tv);
            #endif  
                el = g_s[2]->tv - g_s[0]->tv;
                er = g_s[1]->tv - g_s[2]->tv;
                em = g_s[0]->tv - g_s[1]->tv;

                if ( (el > 0.5f || el < -0.5f ) && ( er > 0.5f || er < -0.5f) ) {
                    if ( g_s[2]->tv < g_s[1]->tv ) {
                        g_s[2]->tv += 1.0f;
                    }
                    else {
                        g_s[1]->tv += 1.0f; g_s[0]->tv += 1.0f;
                    }
                }
                else if ( ( er > 0.5f || er < -0.5f ) && ( em > 0.5f || em < -0.5f )) {
                    if ( g_s[1]->tv < g_s[0]->tv ) {
                        g_s[1]->tv += 1.0f;
                    }
                    else {
                        g_s[0]->tv += 1.0f; g_s[2]->tv += 1.0f;
                    }
                }
                else if ( ( em > 0.5f || em < -0.5f ) && ( el > 0.5f || el < -0.5f ) ) {
                    if ( g_s[0]->tv < g_s[2]->tv ) {
                        g_s[0]->tv += 1.0f;
                    }
                    else {
                        g_s[2]->tv += 1.0f; g_s[1]->tv += 1.0f;
                    }
                }
            }
            else
            {   
            #if 0
                LIMIT_UV_HI(g_s[0]->tv);
                LIMIT_UV_HI(g_s[1]->tv);
                LIMIT_UV_HI(g_s[2]->tv);
                LIMIT_UV_LO(g_s[0]->tv);
                LIMIT_UV_LO(g_s[1]->tv);
                LIMIT_UV_LO(g_s[2]->tv); 
            #endif
            }
            
        }  /* address wrap */
        else if ( TextureAddress == D3DTADDRESS_CLAMP )
        {
        for ( i=0; i<3; i++ )
              {
                    if ( g_s[i]->tu > (float)1.0f )
                    {
                        g_s[i]->tu = (float)1.0f ;
                    }
                    else if ( g_s[i]->tu < (float)0.0f )
                    {
                        g_s[i]->tu = (float)0.0f ;
                    }
                    if ( g_s[i]->tv > (float)1.0f )
                    {
                        g_s[i]->tv = (float)1.0f ;
                    }
                    else if ( g_s[i]->tv < (float)0.0f )
                    {
                        g_s[i]->tv = (float)0.0f ;
                    }
                }
        }
        else{
        #if 0
            LIMIT_UV_HI(g_s[0]->tu);
            LIMIT_UV_HI(g_s[1]->tu);
            LIMIT_UV_HI(g_s[2]->tu);
            LIMIT_UV_HI(g_s[0]->tv);
            LIMIT_UV_HI(g_s[1]->tv);
            LIMIT_UV_HI(g_s[2]->tv);
            LIMIT_UV_LO(g_s[0]->tu);
            LIMIT_UV_LO(g_s[1]->tu);
            LIMIT_UV_LO(g_s[2]->tu);
            LIMIT_UV_LO(g_s[0]->tv);
            LIMIT_UV_LO(g_s[1]->tv);
            LIMIT_UV_LO(g_s[2]->tv); 
        #endif
        }
        
        ctxt->bChopped=TRUE;
        if( (g_Flag = OutOfUVRange( g_s, ctxt->fTextureWidth,
            ctxt->fTextureHeight, &g_MaxInd,ctxt->sData)) == 0)
        {
            pRenderTriangle[ctxt->dwRCode](ctxt,g_s[0], g_s[1], g_s[2]);
                goto ContextRestore;
        }

    if (g_MaxInd==1)
    {
        DWSWAP(g_s[0],g_s[2]);
    }
    else
    if (g_MaxInd==2)
    {
        DWSWAP(g_s[1],g_s[2]);
    }
    // Now side 01 always has the biggest span of u or v depending on g_Flag
    if (g_Flag == UVCLIP_U)
    {
        stepratio01=ctxt->sData->D3DGlobals.__UVRANGE/ctxt->fTextureWidth/(float)fabs(g_s[1]->tu-g_s[0]->tu);
    }
    else
    {
        stepratio01=ctxt->sData->D3DGlobals.__UVRANGE/ctxt->fTextureHeight/(float)fabs(g_s[1]->tv-g_s[0]->tv);
    }
    // Now we have to decide whether side 02 has the 2nd biggest span of uv
    u12=(float)fabs(g_s[2]->tu-g_s[1]->tu);
    v12=(float)fabs(g_s[2]->tv-g_s[1]->tv);
    u02=(float)fabs(g_s[2]->tu-g_s[0]->tu);
    v02=(float)fabs(g_s[2]->tv-g_s[0]->tv);
    if (max(u12,v12) > max(u02,v02))
    {
        DWSWAP(g_s[0],g_s[1]);  //swap to always make 02 bigger
        u02=(float)fabs(g_s[2]->tu-g_s[0]->tu);
        v02=(float)fabs(g_s[2]->tv-g_s[0]->tv);
    }

    if (u02 > v02)
    {
        stepratio02=ctxt->sData->D3DGlobals.__UVRANGE/ctxt->fTextureWidth/u02;
    }
    else
    {
        stepratio02=ctxt->sData->D3DGlobals.__UVRANGE/ctxt->fTextureHeight/v02;
    }
    #if 0
    if (stepratio02 > 1.0f)  stepratio02=1.0f;    //then no chop needed for 02 side
    #else
    stepratio02=stepratio01;    
    #endif
    // Now we have 01 as the bigest and 02 as the second biggest side in terms of uv
    // we are going to start chopping the triangle along 01 and 02
    u12=stepratio01;
    v12=stepratio02;
    startvtx[3]=startvtx[0]=*g_s[0];
    startvtx[4]=startvtx[1]=*g_s[1];
    startvtx[2]=*g_s[2];
do{
    stepratio01=u12;    //get new stepratio01
    stepratio02=v12;    //restore stepratio02
    choplist[0]=startvtx[0]=startvtx[3];
    startvtx[1]=startvtx[4];
    chopcount=0;
    for (ratio01=stepratio01,Overflow=FALSE;ratio01<FLOATONE;ratio01 +=stepratio01)
    {
        chopcount++;    //gentri.c is having hard time to render some near degenerated triangles, so try not produce them here
        if (ratio01 > (1.0f - stepratio01/2.0f))  ratio01 -=stepratio01/2.0f;
        LITR(ctxt,&choplist[chopcount],&startvtx[0],&startvtx[1],ratio01,FlatColor,FlatSpecular);
        if (chopcount > (__TRIQUE_FIRST_ALLOC-2))
        {
            Overflow=TRUE;
            break;  //need to program later
        }
    }
    if (Overflow)
    {
        DPF("Overflowed ratio01=%d",(int)(ratio01*1000));
        u12=stepratio01/(1.0f-ratio01);          //for the next stepratio01
        stepratio01=stepratio01/ratio01;            //for the current stepratio01
        startvtx[1]=startvtx[3]=choplist[chopcount];    //for the next startvtx[0] and current startvtx[0]
#if 0
    for(i=0;i<=chopcount;i++) DPF("choplist[%d]=(%d,%d)",i,(int)choplist[i].sx,(int)choplist[i].sy);
    DPF("ratios=(%d %d) P0(%d,%d) P1(%d,%d) P2(%d,%d)",
        (int)(stepratio01*1000),(int)(stepratio02*1000),(int)g_s[0]->sx,(int)g_s[0]->sy
        ,(int)g_s[1]->sx,(int)g_s[1]->sy,(int)g_s[2]->sx,(int)g_s[2]->sy);
#endif
    }
    else
    {
        chopcount++;
        choplist[chopcount]=startvtx[4];
    }
    newchopcount=chopcount;
    for (ratio02=stepratio02;ratio02<=1.0;ratio02+=stepratio02)
    {
        chopcount=newchopcount;
        newchopcount=0;
        if (ratio02 < FLOATONE)
        {
           if (ratio02 > (1.0 - stepratio02))
            { 
                if (ratio02 > (1.0 - stepratio02/2.0))
                {   
                ratio02 -=stepratio02/2.0f;   
                }
                stepratio02=1.0f-ratio02;    //make the last loop just fit
            }
            LITR(ctxt,&tempvtx[0],&startvtx[0],&startvtx[2],ratio02,FlatColor,FlatSpecular);            
            LITR(ctxt,&tempvtx[1],&startvtx[1],&startvtx[2],ratio02,FlatColor,FlatSpecular);
            tempvtx[3]=tempvtx[0];  //initial tempvtx[2]
            tempfloat=stepratio01/(1.0f-ratio02);
            StripEnd=FALSE;
        }
        else
        {
            tempvtx[3]=startvtx[2]; //initial tempvtx[2]
            StripEnd=TRUE;
        }
        //DPF("chopcount=%d StripEnd=%d tempfloat=%d ratio02=%d",chopcount,(int)StripEnd,(int)(tempfloat*1000),(int)(ratio02*1000));
        for (ratio01=tempfloat,i=0;i<chopcount;i++)
        {
            tempvtx[2]=tempvtx[3];
            //DPF("choplist[%d]=(%d,%d)",i,(int)choplist[i].sx,(int)choplist[i].sy);
                pRenderTriangle[ctxt->dwRCode](ctxt,&choplist[i], &choplist[i+1], &tempvtx[2]);
           
            if (!StripEnd)
            {
                choplist[newchopcount]=tempvtx[2];
                newchopcount++;
                if (ratio01 < FLOATONE)
                {
                    if (ratio01 > (1.0f - tempfloat/2.0f))    ratio01 -=tempfloat/2.0f;
                    LITR(ctxt,&tempvtx[3],&tempvtx[0],&tempvtx[1],ratio01,FlatColor,FlatSpecular);
                }
                else
                {
                    tempvtx[3]=tempvtx[1];
                    StripEnd=TRUE;  // we hit the end of this TriangleStrip
                    //DPF("we hit the end of this TriangleStrip");
                }
                pRenderTriangle[ctxt->dwRCode](ctxt,&choplist[i+1], &tempvtx[2], &tempvtx[3]);
                
            }       
            
            ratio01 +=tempfloat;
        }
        choplist[newchopcount]=tempvtx[3];  //pick up the last updated tempvtx[3]
    }
}while(Overflow);
ContextRestore:
        if ( PerspectiveChange ){
            PerspectiveChange  = FALSE;
            ctxt->bPerspective = TRUE;
            ctxt->RStyle.bPerspective = TRUE;
            ctxt->dTexValtoInt = dTexValtoIntPerspective[ctxt->rsfMaxMipMapLevel];  //LYLY
            ctxt->rndCommand  |= (cmdCMD_TYPE_UnlitTexPersp - cmdCMD_TYPE_UnlitTex);
        }
    ctxt->bChopped=FALSE;
    ctxt->TextureAddress=TextureAddress;
    ctxt->Alpha_workaround=Alpha_workaround;
    return  FALSE;  //not Culled
}


BOOL DXGXSmartFilter( LPD3DTLVERTEX pp0, LPD3DTLVERTEX pp1, LPD3DTLVERTEX pp2, DWORD *rndCommand )
{
    float s1, s2;

    D3DTLVERTEX *p0 = pp0, *p1 = pp1, *p2 = pp2;

    s1 = (p0->sx - p2->sx) * (p1->sy - p2->sy) - (p1->sx - p2->sx) * (p0->sy - p2->sy);
    if ( s1 < 0.0 )
    {
        s1 = -s1;
    }
    s2 = (p0->tu - p2->tu) * (p1->tv - p2->tv) - (p1->tu - p2->tu) * (p0->tv - p2->tv);
    if ( s2 < 0.0 )
    {
        s2 = -s2;
    }

    if (s1 <= s2 * 4.0){
        *rndCommand &= cmdTEX_FILT_MODE_MASK;
        *rndCommand |= cmdTEX_FILT_MODE_2VTpp;
        return TRUE;
    }
    else{
       *rndCommand &= cmdTEX_FILT_MODE_MASK;
       *rndCommand |= cmdTEX_FILT_MODE_4Tpp;
       return FALSE;
    }
    return FALSE;
}






