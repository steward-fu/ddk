/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	gentri.c
 *  Content:	Generate a triangle function
 *
 ***************************************************************************/

#pragma optimize( "a", on )
#pragma optimize( "y", off )

#ifndef GOURAUD
#define GOURAUD 0
#else
#undef GOURAUD
#define GOURAUD 1
#endif

#ifndef DMASUPPORT
#define DMASUPPORT 0
#else
#undef	DMASUPPORT
#define DMASUPPORT 1
#endif

#ifndef ZBUFFER
#define ZBUFFER 0
#else
#undef ZBUFFER
#define ZBUFFER 1
#endif

#ifndef TEXTURED
#define TEXTURED 0
#else
#undef TEXTURED
#define TEXTURED 1
#endif

#ifndef PERSPECTIVE
#define PERSPECTIVE 0
#else
#undef PERSPECTIVE
#if TEXTURED	//No Perspective if no texture
    #define PERSPECTIVE 1
#else	//TEXTURED
  #error  !ERROR: TEXTRED needs to be defined when PERSPECTIVE
#endif	//TEXTURED
#endif

#ifndef FOGGED
#define FOGGED 0
#else
#undef FOGGED
#define FOGGED 1
#endif

#ifndef WRAPCODE
#define WRAPCODE 1
#endif

#ifndef WRAP
// texture wrapping


#define WRAP( p )					\
{							\
							\
    if( (tl.lp = p##1 - p##2) < 0 )			   \
	tl.lp = -tl.lp; 				      \
    if( (tl.rp = p##0 - p##1) < 0 )			   \
	tl.rp = -tl.rp; 				      \
    if( (tl.mp = p##2 - p##0) < 0 )			   \
	tl.mp = -tl.mp; 				      \
    if( tl.lp > (_PRECISION)0.5 && tl.rp > (_PRECISION)0.5 )	      \
    {							\
	if( p##1 < p##0 )				\
	    p##1 += (_PRECISION)1.0;			    \
	else						\
	{						\
	    p##0 += (_PRECISION)1.0;			    \
	    p##2 += (_PRECISION)1.0;			    \
	}						\
    }							\
    else if( tl.rp > (_PRECISION)0.5 && tl.mp > (_PRECISION)0.5 )     \
    {							\
	if( p##0 < p##2 )				\
	    p##0 += (_PRECISION)1.0;			    \
	else						\
	{						\
	    p##2 += (_PRECISION)1.0;			    \
	    p##1 += (_PRECISION)1.0;			    \
	}						\
    }							\
    else if( tl.mp > (_PRECISION)0.5 && tl.lp > (_PRECISION)0.5 )     \
    {							\
	if( p##2 < p##1 )				\
	    p##2 += (_PRECISION)1.0;			    \
	else						\
	{						\
	    p##1 += (_PRECISION)1.0;			    \
	    p##0 += (_PRECISION)1.0;			    \
	}						\
    }							\
}


#endif




#undef	CMD_SET
#undef	WaitFifo
#undef	WaitFifoEmpty
#undef	_WRITE_REG_TRI_LONG
#undef	_WRITE_REG_SET_LONG
#undef	_INDEX_ADVANCE
#undef	_ADVANCE_INDEX
#undef	_WRITE_HEADER
#undef	_UNDO_DMA_HDR
#undef	_START_DMA
#undef _UBASEHIGH
#undef	 FLOAT_TO_TEXPOINT
#define FLOAT_TO_TEXPOINT( c, x )  (_mydtemp = (float)(x) + c->dTexValtoInt, (int)(*(int *)&_mydtemp))

//    #define _WRITE_REG_TRI_LONG( Offset, Value ) { *( volatile ULONG * )( ( char * )ctxt->sData->D3DGlobals.g_p3DTri + ((ctxt->sData->D3DGlobals.g_DmaIndex+Offset)&_DMAFILTER)) =  Value;}
//    #define _WRITE_REG_SET_LONG( Offset, Value ) { *( volatile ULONG * )( ( char * )ctxt->sData->D3DGlobals.g_p3DStp + ((ctxt->sData->D3DGlobals.g_DmaIndex+Offset)&_DMAFILTER)) =  Value;}
#if DMASUPPORT
    #define CMD_SET 0
    #define WaitFifo(x)
    #define WaitFifoEmpty()
    #define _WRITE_REG_TRI_LONG( Offset, Value ) { *( ULONG * )( ( char * )ctxt->sData->D3DGlobals.g_p3DTri + ((ctxt->sData->D3DGlobals.g_DmaIndex+Offset)&_DMAFILTER)) =  Value;}
    #define _WRITE_REG_SET_LONG( Offset, Value ) { *( ULONG * )( ( char * )ctxt->sData->D3DGlobals.g_p3DStp + ((ctxt->sData->D3DGlobals.g_DmaIndex+Offset)&_DMAFILTER)) =  Value;}
    #define _INDEX_ADVANCE  4
    #define _WRITE_HEADER( SLOTS_NEEDED, DMAVALUE ) { \
	ULONG tmp; \
	do { \
	    tmp = ctxt->sData->D3DGlobals.g_DmaReadPtr; \
	    if (ctxt->sData->D3DGlobals.g_DmaReadPtr <= ctxt->sData->D3DGlobals.g_DmaIndex) \
		tmp += _DMAFILTER + 1; \
	    if ( (tmp - ctxt->sData->D3DGlobals.g_DmaIndex) <= (SLOTS_NEEDED) ) { /*need to re-read read ptr*/ \
		tmp = ctxt->sData->D3DGlobals.g_DmaReadPtr = *(ctxt->sData->D3DGlobals.g_lpReadPtrReg) & 0xFFC; \
		if (tmp <= ctxt->sData->D3DGlobals.g_DmaIndex) \
		    tmp += _DMAFILTER + 1; \
	    } \
	} while ((tmp - ctxt->sData->D3DGlobals.g_DmaIndex) <= (SLOTS_NEEDED) ); \
	_WRITE_REG_SET_LONG( 0, DMAVALUE ); \
	ctxt->sData->D3DGlobals.g_DmaIndex += _INDEX_ADVANCE; \
    }

    #define _ADVANCE_INDEX( ADVANCE ) { \
	ctxt->sData->D3DGlobals.g_DmaIndex += ADVANCE; \
	ctxt->sData->D3DGlobals.g_DmaIndex &= _DMAFILTER; \
    }

    #define _START_DMA { \
	*(ctxt->sData->D3DGlobals.g_lpWritePtrReg) = 0x00010000 | ctxt->sData->D3DGlobals.g_DmaIndex; \
    }
    #define _UBASEHIGH	0x80000000
#else	//DMASUPPORT
    #define CMD_SET 0x002C
    #define WaitFifo(x)     {while(((MmioRegRead(0x8504) & 0x1F00) >> 8) < x);}
    #define WaitFifoEmpty() {while(((MmioRegRead(0x8504) & 0x1F00) >> 8) != 16 );}
    #define _WRITE_REG_TRI_LONG( Offset, Value ) { *( volatile ULONG * )( ( char * )ctxt->sData->D3DGlobals.g_p3DTri + Offset) =  Value;}
    #define _WRITE_REG_SET_LONG( Offset, Value ) { *( volatile ULONG * )( ( char * )ctxt->sData->D3DGlobals.g_p3DStp + Offset) =  Value;}
    #define _WRITE_HEADER
    #define _ADVANCE_INDEX
    #define _START_DMA
    #define _UBASEHIGH	(ctxt->sData->D3DGlobals.uBaseHigh)
#endif	//DMASUPPORT


#ifndef MYLOG
#define MYLOG

float mylog2(float x)
{
    float ftemp;
    unsigned int wtemp;
    DWORD *dtemp = (DWORD *)&x;

    if(x>0) {
	wtemp = ((*dtemp >> 23) & 0xff) - 127;
       ftemp = (float)(wtemp) + ((float)((*dtemp>>15) & 0xff))/256.f;
    }
    else
       ftemp = 0.0f;
    return ftemp;
}

#define MYLOG2(x)				 \
{						 \
    float ftemp;				 \
    unsigned int wtemp; 			 \
    DWORD *dtemp;				 \
    ftemp = x;					 \
    dtemp = (DWORD *)&ftemp;			 \
    if(x>0){					 \
	wtemp = ((*dtemp >> 23) & 0xff) - 127;	 \
	tl.lt = (float)(wtemp) + ((float)((*dtemp>>15) & 0xff))/256.f;\
    }						 \
    else					 \
	tl.lt = 0.0f;				 \
}
#endif


#define _SETUP_D_PERSPECTIVE \
    if ( ctxt->TextureMode > D3DFILTER_LINEAR ) \
    /*if(ctxt->bMip)*/\
    { \
	/* a[][] is transform from X,Y into barycentric coordinates, */ \
	/*	 the matrix is not normalized */ \
	\
	tl.a[0][0] = p1->sy - p2->sy; \
	tl.a[1][0] = p2->sy - p0->sy; \
	tl.a[2][0] = p0->sy - p1->sy; \
	\
	tl.a[0][1] = p2->sx - p1->sx; \
	tl.a[1][1] = p0->sx - p2->sx; \
	tl.a[2][1] = p1->sx - p0->sx; \
	\
	tl.a[0][2] = p1->sx * p2->sy - p2->sx * p1->sy; \
	tl.a[1][2] = p2->sx * p0->sy - p0->sx * p2->sy; \
	tl.a[2][2] = p0->sx * p1->sy - p1->sx * p0->sy; \
	\
	tl.det = tl.a[0][2] + tl.a[1][2] + tl.a[2][2];	\
	\
	/* weight by w */ \
	tl.tmpW[0] = (float)(1.f / tl.w0); \
	tl.tmpW[1] = (float)(1.f / tl.w1); \
	tl.tmpW[2] = (float)(1.f / tl.w2); \
	for (tl.i = 0; tl.i < 3; tl.i++) \
	    for (tl.j = 0; tl.j < 3; tl.j++) \
		tl.a[tl.j][tl.i] /= tl.tmpW[tl.j] * tl.det; \
	\
	tl.tmpU[0] = (float)tl.u0; tl.tmpU[1] = (float)tl.u1; tl.tmpU[2] = (float)tl.u2; \
	tl.tmpV[0] = (float)tl.v0; tl.tmpV[1] = (float)tl.v1; tl.tmpV[2] = (float)tl.v2; \
	for (tl.i = 0; tl.i < 3; tl.i++) \
	{ \
	    tl.Du[tl.i] =tl.Dv[tl.i] =tl.Dw[tl.i] = 0.0f; \
	    for (tl.j = 0; tl.j < 3; tl.j++) \
	    { \
	       tl.Du[tl.i] += tl.a[tl.j][tl.i] * tl.tmpU[tl.j]; \
	       tl.Dv[tl.i] += tl.a[tl.j][tl.i] * tl.tmpV[tl.j]; \
	       tl.Dw[tl.i] += tl.a[tl.j][tl.i]; \
	    } \
	} \
	\
	/* u[i], v[i], w[i] are rows of 2D projective */ \
	/* transformation in homogenoious represenntation */ \
	\
	/* for each vertex compute Jacobian and compression */ \
	for (tl.i = 0; tl.i < 3; tl.i++) \
	{ \
	    tl.diff[0][0] = (tl.Du[0] -tl.Dw[0] * tl.tmpU[tl.i]) * tl.tmpW[tl.i]; \
	    tl.diff[0][1] = (tl.Du[1] -tl.Dw[1] * tl.tmpU[tl.i]) * tl.tmpW[tl.i]; \
	    \
	    tl.diff[1][0] = (tl.Dv[0] -tl.Dw[0] * tl.tmpV[tl.i]) * tl.tmpW[tl.i]; \
	    tl.diff[1][1] = (tl.Dv[1] -tl.Dw[1] * tl.tmpV[tl.i]) * tl.tmpW[tl.i]; \
	    \
	    tl.diff_2[0][0] = tl.diff[0][0] * tl.diff[0][0] + tl.diff[1][0] * tl.diff[1][0]; \
	    tl.diff_2[0][1] = tl.diff[0][0] * tl.diff[0][1] + tl.diff[1][0] * tl.diff[1][1]; \
	    \
	    tl.diff_2[1][0] = tl.diff[0][1] * tl.diff[0][0] + tl.diff[1][1] * tl.diff[1][0]; \
	    tl.diff_2[1][1] = tl.diff[0][1] * tl.diff[0][1] + tl.diff[1][1] * tl.diff[1][1]; \
	    \
	    tl.tr = tl.diff_2[0][0] + tl.diff_2[1][1]; \
	    tl.dt = tl.diff_2[0][0] * tl.diff_2[1][1] - tl.diff_2[0][1] * tl.diff_2[1][0]; \
	    \
	    /* if not to speak about accuracy related problems (near singular case), */ \
	    /* there should not be exeptions in the expression below ever */ \
	    \
	    MYLOG2((tl.tr + (float)sqrt(tl.tr * tl.tr - 4.0f * tl.dt)))\
	    /*lt = mylog2((tr + (float)sqrt(tr * tr - 4.0f * dt)));*/\
	    tl.tmpD[tl.i] = (float) (tl.lt/2.0f  - 0.5f ) ; \
	    /*tmpD[i] = ( float ) ( (log(tr + sqrt(tr * tr - 4.0f * dt)) / 2.f) / log(2.f) - 0.5f ) ;*/\
	    if ( (tl.tmpD[tl.i] < 0.0f) || (tl.tmpD[tl.i] > 9.0f) ) \
	    { \
		 tl.tmpD[tl.i] = 0.0f; \
	    } \
	} \
	tl.ddy = ( tl.tmpD[2] - tl.tmpD[0] ) * tl.fDy02r; \
	tl.d0 =  tl.tmpD[0] + tl.ddy * tl.fDyCC; \
	tl.ddx = ( tl.tmpD[1] - ( tl.fDy01 * tl.ddy + tl.tmpD[0] ) ) * tl.fDxr; \
	_WRITE_REG_TRI_LONG( TRI_3D_DS, FLOAT_TO_427(tl.d0) );	  \
	_WRITE_REG_TRI_LONG( TRI_3D_dDdX, FLOAT_TO_427(tl.ddx) ); \
	_WRITE_REG_TRI_LONG( TRI_3D_dDdY, FLOAT_TO_427(tl.ddy) ); \
    } \
    else \
    { \
	_WRITE_REG_TRI_LONG( TRI_3D_DS, 0 );   \
	_WRITE_REG_TRI_LONG( TRI_3D_dDdX, 0 ); \
	_WRITE_REG_TRI_LONG( TRI_3D_dDdY, 0 ); \
    }\




#define DWSWAP1( x, y ) { tl.temp=y; y=x; x=tl.temp; }


#ifndef CONSTDEFD
#define CONSTDEFD
const float fDxConstNeg = -1.0f/524188.0f;
const float fDxConstPos =  1.0f/524188.0f;
const float MAGIC = 256.f;
#endif

	float	vltmp;
	float	tlvBase;

/*
 * The strange way this function is written allows the compiler to produce
 * fully optimized code for a given case, with source level debugging
 * possible.
 * Another advantage is that a mixed code and assembler file produced
 * by the compiler contains real code lines, whereas using the
 * preprocessor all you get is on code line for the whole routine.
 * Comments about the code..
 * 1. As far as possible, float multiplies are used instead of integer.
 *    Although instinct would tend to suggest otherwise, the Pentium
 *    float multiply is 3 times faster than integer, and is twice as
 *    fast as an FIMUL.
 * 2. Conversion from float to integer is costly, and is only done once
 *    where possible for each value.
 * 3. Float comparison is bad.. avoid if possible.
 */
BOOL	FNAME ( S3_CONTEXT *ctxt,
		LPD3DTLVERTEX p0,
		LPD3DTLVERTEX p1,
		LPD3DTLVERTEX p2)
{

#pragma pack(8)

struct	_TRILOCALS
{
    BYTE As;
    LPD3DTLVERTEX temp;
#if GOURAUD
    D3DCOLOR Color2, Color1;
#endif
    D3DCOLOR Color0;
//    int iCount;
    int iDy12, iDy02, iDy01;
    int i2y, i1y, i0y;
#if WRAPCODE
    float lp, rp, mp;
#endif
    float fDxr, fDx;
    float fDyCC;
    float fDy01, fDy12;
    double fDxDy01, fDxDy12, fDxDy02;
    float fDy02r;
#if TEXTURED
    float u2, u1, u0;
    float v2, v1, v0;
//    float fDuDx, fDvDx, fDuDy, fDvDy;
    float fDvDx, fDuDy, fDvDy, fDuDx;
#endif


#if PERSPECTIVE
    float w2, w1, w0, uBase, vBase,tmp1;
    float fDwY,fDwX;
    float Magic;
    float a[3][3];
    float det;
    int   i, j;
    float Du[3],Dv[3],Dw[3];
    float tmpW[3], tmpU[3], tmpV[3], tmpD[3];
    float diff[2][2];
    float diff_2[2][2];
    float tr, dt;
    float lt;
    float d0, ddx, ddy;
    BOOL PerspectiveChange;


#endif
//#if GOURAUD //put these 2 condition in will slow polythrough from 178 to 145
    float fRdX, fRdY, fGdX, fGdY, fBdX, fBdY, fAdX, fAdY;
//#endif
//#if ZBUFFER
    float fDzX, fDzY;
//#endif

} tl ;

    //	  tl.PerspectiveChange = FALSE;

#if !GOURAUD	// in flat shading, the following must be done before we order vertices
    if(ctxt->Alpha_workaround)
    {
	tl.Color0 = p0->color | 0xff000000;
    }
    else
       tl.Color0 = p0->color;
#if  FOGGED
    if (ctxt->bSpecular)
    {
	tl.As = 255 - MYRGB_GETSPECULAR(p0->specular);
    }
    else
	tl.As = MYRGB_GETALPHA(p0->specular);
#else	 //FOGGED

    if(ctxt->rndCommand & cmdALP_BLD_CTL_SrcAlph)
    {
	tl.As = MYRGB_GETALPHA(tl.Color0);
    }
    else
	tl.As =255;
#endif	//FOGGED
//    if (ctxt->bSpecular)
//    {
//	  tl.Color0=CLAMP888( tl.Color0, p0->color, p0->specular );
//    }
#endif	//!GOUAUD
	/* order the vertices */
    if( p2->sy > p1->sy )
	DWSWAP1( p2, p1 );
    if( p2->sy > p0->sy )
	DWSWAP1( p2, p0 );
    if( p1->sy > p0->sy )
	DWSWAP1( p1, p0 );

	/* calculate integer y deltas */

	tl.i2y = F32toS32(p2->sy);
	tl.iDy02 = (tl.i0y = F32toS32(p0->sy)) - tl.i2y;

//	  if( tl.iDy02 == 0 && ((float)tl.i2y < p2->sy))


       if(tl.iDy02 == 0) //fix hang bug in baseball
       {
		return FALSE;
       }

	// tl.iDy12 = (tl.i1y = MYFLINT(p1->sy)) - tl.i2y;
	tl.iDy12 = (tl.i1y = F32toS32(p1->sy)) - tl.i2y;
	tl.fDy02r = 1.0f/(p0->sy - p2->sy);
	tl.fDy01 = p0->sy - p1->sy;
	tl.fDy12 = p1->sy - p2->sy;
	tl.iDy01 = tl.i0y - tl.i1y;
	tl.fDyCC = p0->sy - (float)tl.i0y;

	//take the following part out as it could generate case y0 = y1 = y2 which will hang engine
	//here is a case, y0 = y1 = 2, y2 = 1, after following calculation, y0 and y1 will become 1
//#if 0
	if (tl.fDyCC == 0.0f && tl.iDy01 == 0.0f){
	    if (tl.iDy02 <= 1) return FALSE;	//not drawn according to topleft fill rule
	    tl.i0y --;
	    tl.i1y --;
	    tl.fDyCC = 1.0f;
	}
//#endif
	tl.fDxDy12 = tl.fDy12 ? (p2->sx - p1->sx)/tl.fDy12 : 0.0f;
	tl.fDxDy01 = tl.fDy01 ? (p1->sx - p0->sx)/tl.fDy01 : 0.0f;
	tl.fDxDy02 = (p2->sx - p0->sx)*tl.fDy02r;
	// tl.fDx = p1->sx - (tl.fDy01 * tl.fDxDy02 + p0->sx);
	tl.fDx = (float)(p1->sx - (tl.fDy01 * tl.fDxDy02 + p0->sx));

	if( tl.fDx < fDxConstNeg )
	    tl.fDxr = -1.0f / tl.fDx;
	else if (tl.fDx > fDxConstPos)
	    tl.fDxr = 1.0f / tl.fDx;
	else
		{
			return FALSE;//otherwise it will lose sign info
	    tl.fDx = tl.fDxr = 0.0f;
		}
	_WRITE_HEADER( DATA_SIZE + HDR_SIZE, ( ( ( ( TRI_3D + TRI_3D_DATA )& 0xFFFC) << 14) | ( DATA_SIZE >> 2 ) ));
#ifndef WINNT
	//WaitFifoEmpty();
#endif

	// Textures
#if TEXTURED
	tl.u2 = p2->tu;
	tl.u1 = p1->tu;
	tl.u0 = p0->tu;
	tl.v2 = p2->tv;
	tl.v1 = p1->tv;
	tl.v0 = p0->tv;

//#if WRAPCODE	//lytemp
       /*     if( ctxt->TextureAddress == D3DTADDRESS_WRAP )
	    {
		if( ctxt->bWrapU )
		{
		    WRAP( tl.u );
		}

		if( ctxt->bWrapV )
		{
		    WRAP( tl.v );
		}

	    }
       */
    if( ctxt->TextureAddress == D3DTADDRESS_WRAP )
    {
	if ( ctxt->bWrapU ) {
		tl.lp = tl.u2 - tl.u0;
		tl.rp = tl.u1 - tl.u2;
		tl.mp = tl.u0 - tl.u1;

		if ( (tl.lp > 0.5f || tl.lp < -0.5f) && ( tl.rp > 0.5f || tl.rp < -0.5f) ) {
		    if ( tl.u2 < tl.u1 ) {
			tl.u2 += 1.0f;
		    }
		    else {
			tl.u1 += 1.0f;
			tl.u0 += 1.0f;
		    }
		}
		else if ( ( tl.rp > 0.5f || tl.rp < -0.5f ) && ( tl.mp > 0.5f || tl.mp < -0.5f )) {
		    if ( tl.u1 < tl.u0 ) {
			tl.u1 += 1.0f;
		    }
		    else {
			tl.u0 += 1.0f;
			tl.u2 += 1.0f;
		    }
		}
		else if ( ( tl.mp > 0.5f || tl.mp < -0.5f ) && ( tl.lp > 0.5f || tl.lp < -0.5f ) ) {
		    if ( tl.u0 < tl.u2 ) {
			tl.u0 += 1.0f;
		    }
		    else {
			tl.u2 += 1.0f;
			tl.u1 += 1.0f;
		    }
		}
	    }


	    if ( ctxt->bWrapV ) {

		tl.lp = tl.v2 - tl.v0;
		tl.rp = tl.v1 - tl.v2;
		tl.mp = tl.v0 - tl.v1;

		if ( (tl.lp > 0.5f || tl.lp < -0.5f ) && ( tl.rp > 0.5f || tl.rp < -0.5f) ) {
		    if ( tl.v2 < tl.v1 ) {
			tl.v2 += 1.0f;
		    }
		    else {
			tl.v1 += 1.0f; tl.v0 += 1.0f;
		    }
		}
		else if ( ( tl.rp > 0.5f || tl.rp < -0.5f ) && ( tl.mp > 0.5f || tl.mp < -0.5f )) {
		    if ( tl.v1 < tl.v0 ) {
			tl.v1 += 1.0f;
		    }
		    else {
			tl.v0 += 1.0f; tl.v2 += 1.0f;
		    }
		}
		else if ( ( tl.mp > 0.5f || tl.mp < -0.5f ) && ( tl.lp > 0.5f || tl.lp < -0.5f ) ) {
		    if ( tl.v0 < tl.v2 ) {
			tl.v0 += 1.0f;
		    }
	   else {
		    tl.v2 += 1.0f; tl.v1 += 1.0f;
		}
	    }
	}
    } //WRAP
//#endif   //lytemp

	else if ( ctxt->TextureAddress == D3DTADDRESS_CLAMP )
	{
		LIMIT_HI_TO_ONE(tl.u0);
		LIMIT_LO_TO_ZRO(tl.u0);
		LIMIT_HI_TO_ONE(tl.u1);
		LIMIT_LO_TO_ZRO(tl.u1);
		LIMIT_HI_TO_ONE(tl.u2);
		LIMIT_LO_TO_ZRO(tl.u2);

		LIMIT_HI_TO_ONE(tl.v0);
		LIMIT_LO_TO_ZRO(tl.v0);
		LIMIT_HI_TO_ONE(tl.v1);
		LIMIT_LO_TO_ZRO(tl.v1);
		LIMIT_HI_TO_ONE(tl.v2);
		LIMIT_LO_TO_ZRO(tl.v2);
	} //CLA,P

	//smart filter
    if( (ctxt->TextureMode	  == D3DFILTER_LINEAR)
#if !DMASUPPORT
	      &&  ( (ctxt->sData->wDeviceId == D_S3VIRGEDXGX) ||
		    (ctxt->sData->wDeviceId == D_S3VIRGEGX2)  ||
		    (ctxt->sData->wDeviceId == D_S3VIRGEGX2P)  ||
		    (ctxt->sData->wDeviceId == D_S3VIRGEGX2PM)	||
		    (ctxt->sData->wDeviceId == D_S3M5)	||
		    (ctxt->sData->wDeviceId == D_S3M5P)  ||
		    (ctxt->sData->wDeviceId == D_S3M5PM)      )
#endif	//!DMASUPPORT
	      )
    {

       DXGXSmartFilter(p0,p1,p2, &(ctxt->rndCommand) );
    }

#if PERSPECTIVE
    if((p2->rhw != p1->rhw) || (p0->rhw != p1->rhw))	/* Perspective */
    {

	tl.PerspectiveChange = FALSE; //ly
	tl.u2 *= ctxt->fTextureWidth;
	tl.u1 *= ctxt->fTextureWidth;
	tl.u0 *= ctxt->fTextureWidth;
	tl.v2 *= ctxt->fTextureHeight;
	tl.v1 *= ctxt->fTextureHeight;
	tl.v0 *= ctxt->fTextureHeight;

	if ( tl.u0 < tl.u1  )
	{
	    if ( tl.u1 < tl.u2 )
	    {  /* 0 1 2 */
		tl.uBase = tl.u0;
	    }
	    else if ( tl.u0 < tl.u2 )
	    {	/* 0 2 1 */
		tl.uBase = tl.u0;
	    }
	    else
	    {	/* 2 0 1 */
		tl.uBase = tl.u2;
	    }
	}
	else
	{
	    if ( tl.u0 < tl.u2 )
	    {	/* 1 0 2 */
		tl.uBase = tl.u1;
	    }
	    else if ( tl.u1 < tl.u2 )
	    {	/*  1 2 0 */
		tl.uBase = tl.u1;
	    }
	    else
	    {	/* 2 1 0 */
			    tl.uBase = tl.u2;
	    }
	}
	if ( tl.v0 < tl.v1 )
	{
	    if ( tl.v1 < tl.v2 )
	    {	/* 0 1 2 */
		tl.vBase = tl.v0;
	    }
	    else if ( tl.v0 < tl.v2 )
	    {	/* 0 2 1 */
		tl.vBase = tl.v0;
	    }
	    else
	    {	/* 2 0 1 */
		tl.vBase = tl.v2;
	    }
	}
	else
	{
	    if ( tl.v0 < tl.v2 )
	    {	/* 1 0 2 */
		tl.vBase = tl.v1;
	    }
	    else if ( tl.v1 < tl.v2 )
	    {	/* 1 2 0 */
		tl.vBase = tl.v1;
	    }
	    else
	    {	/* 2 1 0 */
		tl.vBase = tl.v2;
	    }
	}

	tl.uBase = DblRound(tl.uBase);
	tl.vBase = DblRound(tl.vBase);
	tl.Magic = (_PRECISION)MAGIC;
	tl.w2 = p2->rhw;
	tl.w1 = p1->rhw;
	tl.w0 = p0->rhw;

	_SETUP_D_PERSPECTIVE;

	//find max{w[i]} and scale all w with MAGIC/wMAX
	if ( tl.w0 > tl.w1 )
	{
	    if ( tl.w0 > tl.w2 ){
		tl.tmp1 = tl.Magic / tl.w0;
		tl.w0 = tl.Magic;
		tl.w1 = tl.w1 * tl.tmp1;
		tl.w2 = tl.w2 * tl.tmp1;
	    }
	    else {
		tl.tmp1 = tl.Magic / tl.w2;
		tl.w0 = tl.w0 * tl.tmp1;
		tl.w1 = tl.w1 * tl.tmp1;
		tl.w2 = tl.Magic;
	    }
	}
	else
	{
	    if ( tl.w1 > tl.w2 ) {
		tl.tmp1 = tl.Magic / tl.w1;
		tl.w0 = tl.w0 * tl.tmp1;
		tl.w1 = tl.Magic;
		tl.w2 = tl.w2 * tl.tmp1;
	    }
	    else {
		tl.tmp1 = tl.Magic / tl.w2;
		tl.w0 = tl.w0 * tl.tmp1;
		tl.w1 = tl.w1 * tl.tmp1;
		tl.w2 = tl.Magic;
	    }
	}
	//tl.w0 = tl.w0 * tl.tmp1;
	//tl.w1 = tl.w1 * tl.tmp1;
	//tl.w2 = tl.w2 * tl.tmp1;

	tl.u0 = ( tl.u0 - tl.uBase ) * tl.w0;
	tl.u1 = ( tl.u1 - tl.uBase ) * tl.w1;
	tl.u2 = ( tl.u2 - tl.uBase ) * tl.w2;
	tl.v0 = ( tl.v0 - tl.vBase ) * tl.w0;
	tl.v1 = ( tl.v1 - tl.vBase ) * tl.w1;
	tl.v2 = ( tl.v2 - tl.vBase ) * tl.w2;

	tl.uBase -= ctxt->sData->D3DGlobals.coord_adj;
	tl.vBase -= ctxt->sData->D3DGlobals.coord_adj;

	while ( tl.uBase < 0 )
	{
	    tl.uBase += ctxt->fTextureWidth;
	    ; //ly, fix hang in ID day, compiler bug.
	}

//MemphisBug 1526 < Begin >
		  if ( tl.vBase < 0 )
	{
		      tlvBase = tl.vBase / ctxt->fTextureHeight;
				if (tlvBase < -1.0e10 )
					tl.vBase = tl.vBase - tlvBase * ctxt->fTextureHeight ;
	    while ( tl.vBase < 0 )
		tl.vBase += ctxt->fTextureHeight;
	}
//MemphisBug 1526 < End >

	tl.fDwY = (tl.w2-tl.w0)*tl.fDy02r;
	tl.fDwX = ( tl.w1 - (tl.fDwY*tl.fDy01 + tl.w0) ) * tl.fDxr;
	tl.fDvDy = (tl.v2-tl.v0)*tl.fDy02r;
	tl.fDvDx = ( tl.v1 - (tl.fDvDy*tl.fDy01 + tl.v0) ) * tl.fDxr;
	tl.fDuDy = (tl.u2-tl.u0)*tl.fDy02r;
	tl.fDuDx = ( tl.u1 - (tl.fDuDy*tl.fDy01 + tl.u0) ) * tl.fDxr;

	WaitFifo( 14 );

	_WRITE_REG_TRI_LONG( TRI_3D_bV, FLOAT_TO_TEXPOINT(ctxt,tl.vBase) );
	_WRITE_REG_TRI_LONG( TRI_3D_bU, ( FLOAT_TO_TEXPOINT(ctxt,tl.uBase) | _UBASEHIGH ) );

	_WRITE_REG_TRI_LONG( TRI_3D_dWdX, FLOAT_TO_1319(tl.fDwX) );
	_WRITE_REG_TRI_LONG( TRI_3D_dWdY, FLOAT_TO_1319(tl.fDwY) );
	_WRITE_REG_TRI_LONG( TRI_3D_WS, FLOAT_TO_1319(tl.w0 + tl.fDwY*tl.fDyCC) );


	//  _WRITE_REG_TRI_LONG( TRI_3D_dDdX, FLOAT_TO_427(tl.ddx) );
	//  _WRITE_REG_TRI_LONG( TRI_3D_dVdX, FLOAT_TO_2012DXGX(tl.fDvDx));
	//  _WRITE_REG_TRI_LONG( TRI_3D_dUdX, FLOAT_TO_2012DXGX(tl.fDuDx));
	_WRITE_REG_TRI_LONG( TRI_3D_dVdX, FLOAT_TO_2012(tl.fDvDx)>>ctxt->sData->D3DGlobals.DXGX);
	_WRITE_REG_TRI_LONG( TRI_3D_dUdX, FLOAT_TO_2012(tl.fDuDx)>>ctxt->sData->D3DGlobals.DXGX);


	// _WRITE_REG_TRI_LONG( TRI_3D_dDdY, FLOAT_TO_427(tl.ddy) );
	// _WRITE_REG_TRI_LONG( TRI_3D_dVdY, FLOAT_TO_2012DXGX(tl.fDvDy));
	// _WRITE_REG_TRI_LONG( TRI_3D_dUdY, FLOAT_TO_2012DXGX(tl.fDuDy));
	_WRITE_REG_TRI_LONG( TRI_3D_dVdY, FLOAT_TO_2012(tl.fDvDy)>>ctxt->sData->D3DGlobals.DXGX);
	_WRITE_REG_TRI_LONG( TRI_3D_dUdY, FLOAT_TO_2012(tl.fDuDy)>>ctxt->sData->D3DGlobals.DXGX);


	// _WRITE_REG_TRI_LONG( TRI_3D_DS, FLOAT_TO_427(tl.d0) );
	// _WRITE_REG_TRI_LONG( TRI_3D_VS, FLOAT_TO_2012DXGX(tl.v0 + tl.fDvDy*tl.fDyCC));
	// _WRITE_REG_TRI_LONG( TRI_3D_US, FLOAT_TO_2012DXGX(tl.u0 + tl.fDuDy*tl.fDyCC));
	_WRITE_REG_TRI_LONG( TRI_3D_VS, FLOAT_TO_2012(tl.v0 + tl.fDvDy*tl.fDyCC)>>ctxt->sData->D3DGlobals.DXGX);
	_WRITE_REG_TRI_LONG( TRI_3D_US, FLOAT_TO_2012(tl.u0 + tl.fDuDy*tl.fDyCC)>>ctxt->sData->D3DGlobals.DXGX);
    }
    else  /*  non - perspective */
    {
	tl.PerspectiveChange = TRUE; //ly
	ctxt->rndCommand &= ~(cmdCMD_TYPE_UnlitTexPersp - cmdCMD_TYPE_UnlitTex);
	ctxt->dTexValtoInt = dTexValtoInt[ctxt->rsfMaxMipMapLevel];
	WaitFifo( 11 );
	_WRITE_REG_TRI_LONG( TRI_3D_bU, 0 );
	_WRITE_REG_TRI_LONG( TRI_3D_bV, 0 );
#else	//PERSPECTIVE
	{
#endif	//PERSPECTIVE
		tl.u2 *= ctxt->fTextureWidth;
		tl.u1 *= ctxt->fTextureWidth;
		tl.u0 *= ctxt->fTextureWidth;
		tl.v2 *= ctxt->fTextureHeight;
		tl.v1 *= ctxt->fTextureHeight;
		tl.v0 *= ctxt->fTextureHeight;
	if (!ctxt->bChopped)
	{
	  //if (tl.u0==(_PRECISION)((int)tl.u0))
	  if (tl.u0==(DblRound(tl.u0)))
	  {
	    if (tl.u0>=tl.u1 && tl.u0>=tl.u2) tl.u0-=TEXTURE_FACTOR;
	  }
	  //if (tl.u1==(_PRECISION)((int)tl.u1))
	  if (tl.u1==(DblRound(tl.u1)))
	  {
	    if (tl.u1>=tl.u0 && tl.u1>=tl.u2)	tl.u1-=TEXTURE_FACTOR;
	  }
	  //if (tl.u2==(_PRECISION)((int)tl.u2))
	  if (tl.u2==(DblRound(tl.u2)))
	  {
	    if (tl.u2>=tl.u0 && tl.u2>=tl.u1)	tl.u2-=TEXTURE_FACTOR;
	  }
	  //if (tl.v0==(_PRECISION)((int)tl.v0))
	  if (tl.v0==(DblRound(tl.v0)))
	  {
	    if (tl.v0>=tl.v1 && tl.v0>=tl.v2)	tl.v0-=TEXTURE_FACTOR;
	  }
	  //if (tl.v1==(_PRECISION)((int)tl.v1))
	  if (tl.v1==(DblRound(tl.v1)))
	  {
	    if (tl.v1>=tl.v0 && tl.v1>=tl.v2)	tl.v1-=TEXTURE_FACTOR;
	  }
	  //if (tl.v2==(_PRECISION)((int)tl.v2))
	  if (tl.v2==(DblRound(tl.v2)))
	  {
	    if (tl.v2>=tl.v0 && tl.v2>=tl.v1)	tl.v2-=TEXTURE_FACTOR;
	  }
	}   //!ctxt->bChopped
		tl.fDvDy = (tl.v2-tl.v0)*tl.fDy02r;
		tl.fDvDx = ( tl.v1 - (tl.fDvDy*tl.fDy01 + tl.v0) ) * tl.fDxr;
		tl.fDuDy = (tl.u2-tl.u0)*tl.fDy02r;
		tl.fDuDx = ( tl.u1 - (tl.fDuDy*tl.fDy01 + tl.u0) ) * tl.fDxr;
#if !PERSPECTIVE
		WaitFifo( 9 );
#endif	//!PERSPECTIVE
		_WRITE_REG_TRI_LONG( TRI_3D_dDdX, 0 );
		_WRITE_REG_TRI_LONG( TRI_3D_dVdX, FLOAT_TO_TEXPOINT(ctxt,tl.fDvDx) );
		_WRITE_REG_TRI_LONG( TRI_3D_dUdX, FLOAT_TO_TEXPOINT(ctxt,tl.fDuDx) );
		_WRITE_REG_TRI_LONG( TRI_3D_dDdY, 0 );
		_WRITE_REG_TRI_LONG( TRI_3D_dVdY, FLOAT_TO_TEXPOINT(ctxt,tl.fDvDy) );
		_WRITE_REG_TRI_LONG( TRI_3D_dUdY, FLOAT_TO_TEXPOINT(ctxt,tl.fDuDy) );
		_WRITE_REG_TRI_LONG( TRI_3D_DS, 0 );
		_WRITE_REG_TRI_LONG( TRI_3D_VS, FLOAT_TO_TEXPOINT(ctxt,tl.v0 + tl.fDvDy*tl.fDyCC - ctxt->sData->D3DGlobals.coord_adj) );
		_WRITE_REG_TRI_LONG( TRI_3D_US, FLOAT_TO_TEXPOINT(ctxt,tl.u0 + tl.fDuDy*tl.fDyCC - ctxt->sData->D3DGlobals.coord_adj) );

	    }
#else	//TEXTURED
    #if FOGGED
	    // special case fogged without texture

	    WaitFifo( 8 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dVdX, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dUdX, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dDdX, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dDdY, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dVdY, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dUdY, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_DS, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_VS, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_US, 0 );
    #endif  //FOGGED
#endif	//TEXTURED
	/* Color setup */
#if GOURAUD
	{
	    if(ctxt->Alpha_workaround)
	    {
	       tl.Color2 = p2->color | 0xff000000;
	       tl.Color1 = p1->color | 0xff000000;
	       tl.Color0 = p0->color | 0xff000000;
	    }
	 else
	    {
	       tl.Color2 = p2->color;
	       tl.Color1 = p1->color;
	       tl.Color0 = p0->color;
	    }
#if FOGGED
	    if (ctxt->bSpecular)
	    {
		tl.fAdY =  ((float)(MYRGB_GETSPECULAR(p2->specular) -
		    MYRGB_GETSPECULAR(p0->specular)) * tl.fDy02r);
		tl.fAdX =  - (((float)(MYRGB_GETSPECULAR(p1->specular) -
		    MYRGB_GETSPECULAR(p0->specular)) - tl.fAdY*tl.fDy01) * tl.fDxr);
		tl.fAdY = - tl.fAdY;
		tl.As = 255 - MYRGB_GETSPECULAR(p0->specular);
	    }
	    else
	    {
		tl.fAdY = (float)(MYRGB_GETALPHA(p2->specular) -
		    MYRGB_GETALPHA(p0->specular)) * tl.fDy02r;
		tl.fAdX = ((float)(MYRGB_GETALPHA(p1->specular) -
		    MYRGB_GETALPHA(p0->specular)) - tl.fAdY*tl.fDy01) * tl.fDxr;
		tl.As = MYRGB_GETALPHA(p0->specular);
	    }

#else	//FOGGED
	    if(ctxt->rndCommand & cmdALP_BLD_CTL_SrcAlph){
		tl.fAdY = (float)(MYRGB_GETALPHA(tl.Color2) -
		    MYRGB_GETALPHA(tl.Color0)) * tl.fDy02r;
		tl.fAdX = ((float)(MYRGB_GETALPHA(tl.Color1) -
		    MYRGB_GETALPHA(tl.Color0)) - tl.fAdY*tl.fDy01) * tl.fDxr;
		tl.As = MYRGB_GETALPHA(tl.Color0);
	    }
	    else
	    {
		tl.As = 255; tl.fAdY = 0.0f; tl.fAdX = 0.0f;
	    }
#endif	//FOGGED
	//    if (ctxt->bSpecular)
	//    {
	//	  CLAMP888( tl.Color2, p2->color, p2->specular );
	//	  CLAMP888( tl.Color1, p1->color, p1->specular );
	//	  CLAMP888( tl.Color0, p0->color, p0->specular );
	//    }
	    tl.fGdY = (float)(MYRGB_GETGREEN(tl.Color2) - MYRGB_GETGREEN(tl.Color0)) * tl.fDy02r;
	    tl.fGdX = ((float)(MYRGB_GETGREEN(tl.Color1) - MYRGB_GETGREEN(tl.Color0)) - tl.fGdY*tl.fDy01) * tl.fDxr;
	    tl.fBdY = (float)(MYRGB_GETBLUE(tl.Color2) - MYRGB_GETBLUE(tl.Color0)) * tl.fDy02r;
	    tl.fBdX = ((float)(MYRGB_GETBLUE(tl.Color1) - MYRGB_GETBLUE(tl.Color0)) - tl.fBdY*tl.fDy01) * tl.fDxr;
	    tl.fRdY = (float)(MYRGB_GETRED(tl.Color2) - MYRGB_GETRED(tl.Color0)) * tl.fDy02r;
	    tl.fRdX = ((float)(MYRGB_GETRED(tl.Color1) - MYRGB_GETRED(tl.Color0)) - tl.fRdY*tl.fDy01) * tl.fDxr;
    #if ZBUFFER
	    WaitFifo( 6 );
    #else   //ZBUFFER
	    WaitFifo( 15 );
    #endif  //ZBUFFER
	    _WRITE_REG_TRI_LONG( TRI_3D_dGdX_dBdX, (FLOAT_TO_87( tl.fGdX ) << 16) |
						  (FLOAT_TO_87( tl.fBdX ) & 0xFFFF) );
	    _WRITE_REG_TRI_LONG( TRI_3D_dAdX_dRdX, (FLOAT_TO_87( tl.fAdX ) << 16) |
						  (FLOAT_TO_87( tl.fRdX ) & 0xFFFF) );
	    _WRITE_REG_TRI_LONG( TRI_3D_dGdY_dBdY, (FLOAT_TO_87( tl.fGdY ) << 16) |
						  (FLOAT_TO_87( tl.fBdY ) & 0xFFFF) );
	    _WRITE_REG_TRI_LONG( TRI_3D_dAdY_dRdY, (FLOAT_TO_87( tl.fAdY ) << 16) |
						  (FLOAT_TO_87( tl.fRdY ) & 0xFFFF) );
	    _WRITE_REG_TRI_LONG( TRI_3D_GS_BS,
		(COLOR_TO_87( MYRGB_GETGREEN(tl.Color0) ) << 16) |
		 COLOR_TO_87( MYRGB_GETBLUE(tl.Color0) ) );
	    _WRITE_REG_TRI_LONG( TRI_3D_AS_RS,
		(COLOR_TO_87( tl.As ) << 16) |
		 COLOR_TO_87( MYRGB_GETRED(tl.Color0) )  );
	}
 #else	//GOURAUD
	{
    #if ZBUFFER
	    WaitFifo( 6 );
    #else   //ZBUFFER
	    WaitFifo( 15 );
    #endif  //ZBUFFER
	    _WRITE_REG_TRI_LONG( TRI_3D_dGdX_dBdX, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dAdX_dRdX, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dGdY_dBdY, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dAdY_dRdY, 0 );
	    _WRITE_REG_TRI_LONG( TRI_3D_GS_BS,
		(COLOR_TO_87( MYRGB_GETGREEN(tl.Color0) ) << 16) |
		 COLOR_TO_87( MYRGB_GETBLUE(tl.Color0) ) );
	    _WRITE_REG_TRI_LONG( TRI_3D_AS_RS,
		(COLOR_TO_87( tl.As ) << 16) |
		 COLOR_TO_87( MYRGB_GETRED(tl.Color0) ) );
	}
#endif	//GOURAUD
	/* Z setup */
#if ZBUFFER
	{
	    tl.fDzY = (p2->sz - p0->sz) * tl.fDy02r;
	    tl.fDzX = (p1->sz - (tl.fDzY*tl.fDy01 + p0->sz)) * tl.fDxr;
	    WaitFifo( 12 );
	    _WRITE_REG_TRI_LONG( TRI_3D_dZdX, MYFLINT31( tl.fDzX ) );
	    _WRITE_REG_TRI_LONG( TRI_3D_dZdY, MYFLINT31( tl.fDzY ) );
	    _WRITE_REG_TRI_LONG( TRI_3D_ZS02, MYFLINT31( p0->sz + (tl.fDyCC * tl.fDzY)) );
	}
#endif	//ZBUFFER
	/* Draw the triangle */

	_WRITE_REG_TRI_LONG( TRI_3D_dXdY12, FLOAT_TO_1220(tl.fDxDy12) );
	_WRITE_REG_TRI_LONG( TRI_3D_XEnd12,
	    FLOAT_TO_1220( p1->sx  + (tl.fDxDy12 * (p1->sy - (float)tl.i1y)) ) );
	_WRITE_REG_TRI_LONG( TRI_3D_dXdY01, FLOAT_TO_1220(tl.fDxDy01) );
	_WRITE_REG_TRI_LONG( TRI_3D_XEnd01,
	    FLOAT_TO_1220( p0->sx + (tl.fDxDy01 * tl.fDyCC)) );
	_WRITE_REG_TRI_LONG( TRI_3D_dXdY02, FLOAT_TO_1220( tl.fDxDy02 ) );
	_WRITE_REG_TRI_LONG( TRI_3D_XStart02,
	    FLOAT_TO_1220( (tl.fDxDy02 * tl.fDyCC) + p0->sx ) );
	_WRITE_REG_TRI_LONG( TRI_3D_YStart, tl.i0y );
	_WRITE_REG_TRI_LONG( TRI_3D_Y01_Y12,
	    (tl.iDy01 << 16) | (tl.iDy12 + (p2->sy == (float)tl.i2y) ) |
	    ((tl.fDx > 0) ? 0x80000000 : 0) );
	_ADVANCE_INDEX( DATA_SIZE );
	_WRITE_HEADER( CMD_SIZE + HDR_SIZE, ( ( ( ( TRI_3D + CMD_OFFSET )& 0xFFFC) << 14) | ( CMD_SIZE >> 2 ) ));
	_WRITE_REG_SET_LONG( CMD_SET, ctxt->rndCommand );
	_ADVANCE_INDEX( CMD_SIZE );

	_START_DMA;
#if PERSPECTIVE
	if(tl.PerspectiveChange)
	{
	    tl.PerspectiveChange  = FALSE;
	    ctxt->dTexValtoInt = dTexValtoIntPerspective[ctxt->rsfMaxMipMapLevel];  //ly
	    ctxt->rndCommand  |= (cmdCMD_TYPE_UnlitTexPersp - cmdCMD_TYPE_UnlitTex);
	}
#endif
	return FALSE;
}

#undef GOURAUD
#undef DMASUPPORT
#undef ZBUFFER
#undef TEXTURED
#undef PERSPECTIVE
#undef FOGGED
#undef FNAME
#undef WRAPCODE

#pragma optimize( "a", off )


