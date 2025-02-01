/*==========================================================================;
 *
 *  Copyright (C) 1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3ddrv.h
 *  Content:    Header for Direct3D HAL driver.
 *
 ***************************************************************************/

#ifndef __D3DDRV__

#define __D3DDRV__
#include "s3_ddtk.h"
#ifdef WINNT
#include "dx95type.h"
#else
#include <windows.h>

#include <ddrawi.h>
#include <ddraw.h>

#include <d3dhal.h>
#include "s3kernel.h"
#endif

#include <d3dtypes.h>
#include "virge1.h"
#include "s3dmacro.h"

#if defined(DEBUG) || defined(DBG)
extern void __cdecl DPF( LPSTR szFormat, ... );
#else
    #define DPF         1 ? (void)0 : (void)
#endif

extern DWORD ComputeAlignedStride(LPDDRAWI_DDRAWSURFACE_LCL lpDDS);
extern double dTexValtoInt[];
extern double dTexValtoIntPerspective[];


BOOL OutOfUVRange( LPD3DTLVERTEX v[ 3 ], float uScale, float vScale, int *,LPS3THKINFO );
BOOL ChopInUVSpace( struct _S3_CONTEXT *ctxt,
                LPD3DTLVERTEX p0,
                LPD3DTLVERTEX p1,
                LPD3DTLVERTEX p2);

BOOL GenericOneWireFrame ( struct _S3_CONTEXT *ctxt,
        LPD3DTLVERTEX    p0,
        LPD3DTLVERTEX    p1,
        LPD3DTLVERTEX    p2);

void GenericWireFrame ( struct _S3_CONTEXT *pCtx,
        LPD3DINSTRUCTION pI,
        LPD3DTLVERTEX    pV,
        LPD3DTRIANGLE    pT);

void GenericPointTriangle ( struct _S3_CONTEXT *pCtx,
        LPD3DINSTRUCTION pI,
        LPD3DTLVERTEX    pV,
        LPD3DTRIANGLE    pT);

void GenericLine(  struct _S3_CONTEXT *ctxt,
                  LPD3DTLVERTEX    p0,
                  LPD3DTLVERTEX    p1,
                  LPD3DTLVERTEX    pFlat);

void GenericLines( struct _S3_CONTEXT *pCtx,
        LPD3DINSTRUCTION pI,
        LPD3DTLVERTEX    pV,
        LPD3DLINE        pL );

void GenericPoint( struct _S3_CONTEXT *ctxt,
                  LPD3DTLVERTEX    p0,
                  LPD3DTLVERTEX    p1);

void GenericPoints( struct _S3_CONTEXT *pCtx,
        LPD3DINSTRUCTION pI,
        LPD3DTLVERTEX    pV,
        LPD3DPOINT       pL );

typedef void (*PFNTRI)( struct _S3_CONTEXT *ctxt,
                        LPD3DINSTRUCTION pI,
                        LPD3DTLVERTEX    pV,
                        LPD3DTRIANGLE    pT);

#define MAX_STATE       D3DSTATE_OVERRIDE_BIAS
#define DWORD_BITS      32
#define DWORD_SHIFT     5

typedef struct _D3DSAMPStateSet {
    DWORD               bits[MAX_STATE >> DWORD_SHIFT];
} D3DSAMPStateSet;

typedef struct _render_style
{
    DWORD
        bGouraud : 1,       // Gouraud on (D3DSHADE_GOURAUD vs. D3DSHADE_FLAT)
        bDma    : 1,        // DMASUPPORT
        bZBuffer : 1,       // Z enabled
        bTextured : 1,
        bPerspective : 1,
        bFogged : 1,
        bFillmode : 2;  // Filled D3DFILL_SOLID vs WIREFRAME vs POINT
} RENDER_STYLE;
//following are corresponding masks for RENDR_STYLE
#define S3GOURAUD   0x0001
#define S3DMA       0x0002
#define S3ZBUFFER   0x0004
#define S3TEXTURED  0x0008
#define S3PERSPECTIVE   0x0010
#define S3FOGGED    0x0020
#define S3FILLMODE  0x00C0 


//dwStatus bits in _S3_CONTEXT
#define	S3ALPHABLENDENABLE	0x02
#define	S3FOGENABLE		    0x04
#define	S3SPECULARENABLE	0x08


typedef BOOL (*LPRENDERTRIANGLE)(LPS3_CONTEXT,LPD3DTLVERTEX,LPD3DTLVERTEX,LPD3DTLVERTEX);
extern LPRENDERTRIANGLE pRenderTriangle[];  //refer to RStyle definition

typedef struct _S3_CONTEXT {

#ifdef WINNT
    PDEV                *ppdev;         // associated pdev
    /* hw parm regs for 3D triangle */
    volatile vi13D_TRIANGLE *g_p3DTri;
    /* hw setup regs for 3D triangle */
    volatile vi13D_SETUP *g_p3DStp;
#endif

    ULONG               dwPID;          // This context's Process ID
    D3DSAMPStateSet     overrides;
    DWORD               dwTexture;
    ULONG               FrameAddr;      // DD object primary display.
    LPDDRAWI_DDRAWSURFACE_LCL lpLcl;    // Rendering surface.
    LPDDRAWI_DDRAWSURFACE_LCL lpLclZ;   // Z surface.
    D3DCULL             CullMode;       // NONE, CW (clockwise), CCW
    D3DTEXTUREBLEND     BlendMode;      // Texture blend mode (decal, modulate, etc.)
    D3DTEXTUREFILTER    TextureMode;    // Texture filtering function
    D3DCMPFUNC          ZCmpFunc;       // Z comparison function
    D3DSHADEMODE        ShadeMode;      // Shade mode (flat, Gouraud, etc.)
    D3DTEXTUREADDRESS   TextureAddress; // Texture address mode (wrap/clamp)
    D3DFILLMODE         FillMode;       // filled
    BOOL                bPerspective;   // Perspective correction textures
    BOOL                bSpecular;      // Specular lighting
    BOOL                bWrapU, bWrapV; // TRUE to enable wraps for each coord
    BOOL                bBlendEnabled;  // All blending
    DWORD               dwStatus;       // All blending and any other misc issues
    BOOL                bFogEnabled;    // Per-vertex fog
    BOOL                bZEnabled;      // Z buffer updates enabled
    BOOL                bZWriteEnabled;
    BOOL                bZVisible;
        union {
        RENDER_STYLE        RStyle;             // encoded render style
        DWORD               dwRCode;            // for binary value
        };

    DWORD               rndCommand;     // Command word ready to send to hardware
    vi13D_SETUP         rnd3DSetup;     // Setup registers
    DWORD               rsfMaxMipMapLevel; // size of texture as power of 2
    float               fTextureWidth;  // precalculate these
    float               fTextureHeight;
    double              dTexValtoInt;   // and this
    BOOL                bChopped;         // Indicated the triangle is already chopped in UV space
    BOOL                bChanged;       // setup needed
    BOOL                Alpha_workaround; // 24 bit alpha workaround
    D3DBLEND            SrcBlend;
    struct _S3_CONTEXT  *Last;
    struct _S3_CONTEXT  *Next;
    struct _S3_TEXTURES *TextureList;
//
// new for MM support
//
    LPS3THKINFO         sData;          // pointer to our sData

} S3_CONTEXT, *LPS3_CONTEXT;

#define TEXTURE_FACTOR (float)(1.0/256.0)

typedef struct _S3_TEXTURES {
    DWORD               pid;            // associated Process ID
    DWORD               hContext;       // Handle to owning context
    LPDDRAWI_DDRAWSURFACE_LCL lpLcl;    // Texture
#ifdef WINNT
    HANDLE              hTexture;
#else
    LPDIRECTDRAWSURFACE lpDDS;
#endif

    struct _S3_TEXTURES  *Last;
    struct _S3_TEXTURES  *Next;

} S3_TEXTURE, *LPS3_TEXTURE;

#define CONTEXTS_SIZE   64
#define CONTEXTS_DELTA  64
#define TEXTURES_SIZE   256
#define TEXTURES_DELTA  256

#ifndef WINNT
// Defines to instruct the asm
#define SETUPASM_DOALPHA        1
#define SETUPASM_DOCOLORDELTAS  2
#define SETUPASM_DOZ            4
#else
#define hSharedHeap NULL
#endif

// my colors

#define MYRGB_GETALPHA(c) (((BYTE *)&c)[3])
#define MYRGB_GETRED(c) (((BYTE *)&c)[2])
#define MYRGB_GETGREEN(c) (((BYTE *)&c)[1])
#define MYRGB_GETBLUE(c) (((BYTE *)&c)[0])

#define MYRGB_GETSPECULAR(c) ( (((((((BYTE *)&c)[0]) > (((BYTE *)&c)[1])) ? (((BYTE *)&c)[0]) : (((BYTE *)&c)[1]))) > (((BYTE *)&c)[2])) ? ((((((BYTE *)&c)[0]) > (((BYTE *)&c)[1])) ? (((BYTE *)&c)[0]) : (((BYTE *)&c)[1]))) : (((BYTE *)&c)[2]) )

#define CLAMP888(result, color, specular) \
     result = (color & 0xfefefe) + (specular & 0xfefefe);   \
     result |= ((0x808080 - ((result >> 8) & 0x010101)) & 0x7f7f7f) << 1;

// fast positive float to int

static float _myfltemp;
static float _myfltemp1;//for alignment, performance purpose.
static double _mydtemp;

#define TWOPOW32 ((double)(65536.0 * 65536.0))

#define TWOPOW(N) (((N) < 32) ? ((double)(1UL << (N))) : \
                        ((double)(1UL << (N - 32)) * TWOPOW32))
static const double _myfl0const = TWOPOW(52) + TWOPOW(51);
static const double _myfl7const = TWOPOW(45) + TWOPOW(44);
static const double _myfl8const = TWOPOW(44) + TWOPOW(43);
static const double _myfl12const = TWOPOW(40) + TWOPOW(39);
static const double _myfl15const = TWOPOW(37) + TWOPOW(36);
static const double _myfl16const = TWOPOW(36) + TWOPOW(35);
static const double _myfl18const = TWOPOW(34) + TWOPOW(33);
static const double _myfl19const = TWOPOW(33) + TWOPOW(32);
static const double _myfl20const = TWOPOW(32) + TWOPOW(31);
static const double _myfl24const = TWOPOW(28) + TWOPOW(27);
static const double _myfl27const = TWOPOW(25) + TWOPOW(24);
static const double _myfl28const = TWOPOW(24) + TWOPOW(23);
static const double _myfl31const = TWOPOW(21) + TWOPOW(20);
static const float _myflconst = (float)TWOPOW(23);
static const float _myflconst1 = (float)TWOPOW(23);     // for alignment

//#define MYFLINT(x) (_myfltemp = (float)(x) + _myflconst, (int)(*(WORD *)&_myfltemp))
#define MYFLINT(x) F32toS32(x)
#define MYFLINTUCHAR(x) (_mydtemp = (double)(x) + _myfl0const, (BYTE)(*(BYTE *)&_mydtemp))
#define MYFLINT0(x) (_mydtemp = (double)(x) + _myfl0const, (int)(*(int *)&_mydtemp))
#define MYFLINT7(x) (_mydtemp = (double)(x) + _myfl7const, (int)(*(int *)&_mydtemp))
#define MYFLINT12(x) (_mydtemp = (double)(x) + _myfl12const, (int)(*(int *)&_mydtemp))
#define MYFLINT15(x) (_mydtemp = (double)(x) + _myfl15const, (int)(*(int *)&_mydtemp))
#define MYFLINT16(x) (_mydtemp = (double)(x) + _myfl16const, (int)(*(int *)&_mydtemp))
#define MYFLINT18(x) (_mydtemp = (double)(x) + _myfl18const, (int)(*(int *)&_mydtemp))
#define MYFLINT19(x) (_mydtemp = (double)(x) + _myfl19const, (int)(*(int *)&_mydtemp))
#define MYFLINT20(x) (_mydtemp = (double)(x) + _myfl20const, (int)(*(int *)&_mydtemp))
#define MYFLINT27(x) (_mydtemp = (double)(x) + _myfl27const, (int)(*(int *)&_mydtemp))
#define MYFLINT24(x) (_mydtemp = (double)(x) + _myfl24const, (int)(*(int *)&_mydtemp))
#define MYFLINT28(x) (_mydtemp = (double)(x) + _myfl28const, (int)(*(int *)&_mydtemp))
#define MYFLINT31(x) (_mydtemp = (double)(x) + _myfl31const, (int)(*(int *)&_mydtemp))

extern double dTexValtoInt[];

#define FLOAT_TO_TEXPOINT( c, x )  (_mydtemp = (double)(x) + c->dTexValtoInt, (int)(*(int *)&_mydtemp))
#define FLOAT_TO_2012DXGX( x )  (_mydtemp = (double)(x) + ctxt->sData->D3DGlobals.DXGX, (int)(*(int *)&_mydtemp))

#define FLOAT_TO_2012( x ) MYFLINT12(x)
#define FLOAT_TO_1715( x ) MYFLINT15(x)
#define FLOAT_TO_1616( x ) MYFLINT16(x)
#define FLOAT_TO_1220( x ) MYFLINT20(x)
#define FLOAT_TO_1418( x ) MYFLINT18(x)
#define FLOAT_TO_1319( x ) MYFLINT19(x)
#define FLOAT_TO_824( x ) MYFLINT24(x)
#define FLOAT_TO_427( x ) MYFLINT27(x)
#define FLOAT_TO_428( x ) MYFLINT28(x)
#define FLOAT_TO_87( x ) MYFLINT7(x)
#define COLOR_TO_87( x ) ( (x) << 7 )

// non-specific

#define DWSWAP( x, y ) { DWORD temp=(DWORD)y; y = x; (DWORD)x = temp; }

#define Culling(p0,p1,p2) ((ctxt->CullMode!=D3DCULL_NONE) && \
    ((ctxt->CullMode==D3DCULL_CCW && (p1->sx-p0->sx)*(p2->sy-p0->sy)<=(p2->sx-p0->sx)*(p1->sy-p0->sy)) || \
     (ctxt->CullMode== D3DCULL_CW && (p1->sx-p0->sx)*(p2->sy-p0->sy)>=(p2->sx-p0->sx)*(p1->sy-p0->sy)) ) )

#ifdef WINNT
#define D3DERR_TEXTURE_CREATE_FAILED    MAKE_DDHRESULT(721)
#else
#define DDS_LCL(x) ((LPDDRAWI_DDRAWSURFACE_INT)x)->lpLcl
#define NW_FIFO_WAIT(a,b,c) WaitFifo(c)
#endif
extern  LPS3THKINFO GetMyRefData(LPDDRAWI_DIRECTDRAW_GBL);
extern HANDLE g_hD3DHAL_DriverHeap;
#define MEMALLOC(cbSize)    HeapAlloc (g_hD3DHAL_DriverHeap, HEAP_ZERO_MEMORY, (cbSize))
#define MEMFREE(lpPtr)      HeapFree (g_hD3DHAL_DriverHeap, 0, (lpPtr))

#endif /* __D3DDRV__ */

