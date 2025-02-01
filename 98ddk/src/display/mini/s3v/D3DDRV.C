
/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3dmini.c
 *  Content:    Direct3D Sample HAL Driver
 *
 ***************************************************************************/

#include "precomp.h"
#include "d3ddrv.h"

#define START_STR       "S3D3D: "
#define END_STR         "\r\n"



extern WORD    g_TriFuncStackAdj[];

#define OPCODE_SubESP   0xEC81
#define MAXCODEBYTES        60
#define ALIGNBYTEBOUNDARY    8

void CalcStackAdjust()
{
    int     i,j;
    char*   pCodeByte;
    WORD    stackSize;

    for( j=0; j<MAXNUMTRIFUNCS; j++ )
    {
        pCodeByte = (char*)pRenderTriangle[ j ];

        g_TriFuncStackAdj[ j ] = 0;

        for( i=0; i<MAXCODEBYTES; i++ )
        {
            if( *((WORD*)pCodeByte) == OPCODE_SubESP )
            {
                stackSize = *((WORD*)(pCodeByte + 2)) ;
                g_TriFuncStackAdj[ j ] = stackSize & ( ALIGNBYTEBOUNDARY - 1 );
                break;
            }
            pCodeByte++;
        }
    }
}



/****************************************************************************
 *
 *
 * The following functions (my....) are the hardware specific routines
 * that the driver needs to implement to make Direct3D functionality work
 *
 *
 ***************************************************************************/

/*----------------------------01-29-96 S3 06:58pm-----------------------------
 Globals used for DirectDraw purposes.
----------------------------------------------------------------------------*/
#if !WINNT
BOOL            main;
#endif

DWORD _fltused;

DWORD
ComputeAlignedStride(LPDDRAWI_DDRAWSURFACE_LCL lpDDS)
{
    DWORD width = DDSurf_Width(lpDDS);
    int bytesPerPix;
    switch (DDSurf_BitDepth(lpDDS)) {
        case 8 :
            bytesPerPix = 1;
            break;
        case 16:
            bytesPerPix = 2;
            break;
        case 24:
            bytesPerPix = 3;
            break;
        default:
            return (DWORD) -1;
    }
    return (DDSurf_Width(lpDDS)*bytesPerPix + 7) & ~7L;
}

/* --------------------------------------------------------------
 * Direct3D HAL Table.
 * Should be filled in to indicate which HAL calls we support.
 * NO EMULATION HERE.
 *
 * Calls take the form of:
 *      retcode = HalCall(HalCallData* lpData);
 */

extern DWORD __stdcall myContextCreate(LPD3DHAL_CONTEXTCREATEDATA pccd);
extern DWORD __stdcall myContextDestroy(LPD3DHAL_CONTEXTDESTROYDATA pcdd);
extern DWORD __stdcall myContextDestroyAll(LPD3DHAL_CONTEXTDESTROYALLDATA pccd);

extern DWORD __stdcall myRenderPrimitive(LPD3DHAL_RENDERPRIMITIVEDATA prd);
extern DWORD __stdcall myRenderState(LPD3DHAL_RENDERSTATEDATA prd);
extern DWORD __stdcall myGetState(LPD3DHAL_GETSTATEDATA pgsd);

extern DWORD __stdcall myTextureCreate(LPD3DHAL_TEXTURECREATEDATA ptcd);
extern DWORD __stdcall myTextureDestroy(LPD3DHAL_TEXTUREDESTROYDATA ptcd);
extern DWORD __stdcall myTextureSwap(LPD3DHAL_TEXTURESWAPDATA ptsd);
extern DWORD __stdcall myTextureGetSurf(LPD3DHAL_TEXTUREGETSURFDATA ptgd);


static D3DHAL_CALLBACKS myD3DHALCallbacks = {
    sizeof(D3DHAL_CALLBACKS),

    // Device context
    myContextCreate,            // Required.
    myContextDestroy,           // Required.
    myContextDestroyAll,

    // Scene capture
    NULL,                       // Optional.

    // Execution
    NULL,                       // Optional.  Don't implement if just rasterization.
    NULL,
    myRenderState,              // Required if no Execute
    myRenderPrimitive,          // Required if no Execute
    0L,                         // Reserved, must be zero

    // Textures
    myTextureCreate,            // If any of these calls are supported,
    myTextureDestroy,           // they must all be.
    myTextureSwap,              // ditto - but can always fail.
    myTextureGetSurf,       // ditto - but can always fail.

    // Transform - must be supported if lighting is supported.
    NULL, //myMatrixCreate, // If any of these calls are supported,
    NULL, //myMatrixDestroy,    // they must all be.
    NULL, //myMatrixSetData,    // ditto
    NULL, //myMatrixGetData,    // ditto
    NULL, //mySetViewportData,  // ditto

    // Lighting
    NULL,           // If any of these calls are supported,
    NULL, //myMaterialCreate,   // they must all be.
    NULL, //myMaterialDestroy,  // ditto
    NULL, //myMaterialSetData,  // ditto
    NULL, //myMaterialGetData,  // ditto

    // Pipeline state
    myGetState,         // GetState

    0L,             // Reserved, must be zero
    0L,             // Reserved, must be zero
    0L,             // Reserved, must be zero
    0L,             // Reserved, must be zero
    0L,             // Reserved, must be zero
    0L,             // Reserved, must be zero
    0L,             // Reserved, must be zero
    0L,             // Reserved, must be zero
    0L,             // Reserved, must be zero
    0L,             // Reserved, must be zero
};

#define nullPrimCaps {                                          \
    sizeof(D3DPRIMCAPS), 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0     \
}


#define lineCaps {                                           \
    sizeof(D3DPRIMCAPS),                                    \
        D3DPMISCCAPS_CULLNONE |                 \
        D3DPMISCCAPS_CULLCW |                   \
        D3DPMISCCAPS_CULLCCW, /* miscCaps */                  \
    D3DPRASTERCAPS_DITHER |         /* rasterCaps */        \
        D3DPRASTERCAPS_SUBPIXEL |       \
        D3DPRASTERCAPS_FOGVERTEX,       \
    D3DPCMPCAPS_NEVER |             \
        D3DPCMPCAPS_LESS |              \
        D3DPCMPCAPS_EQUAL |             \
        D3DPCMPCAPS_LESSEQUAL |         \
        D3DPCMPCAPS_GREATER |           \
        D3DPCMPCAPS_NOTEQUAL |          \
        D3DPCMPCAPS_GREATEREQUAL |      \
        D3DPCMPCAPS_ALWAYS,                  /* zCmpCaps */     \
    D3DPBLENDCAPS_ONE |              /* sourceBlendCaps */   \
        D3DPBLENDCAPS_SRCALPHA,                         \
    D3DPBLENDCAPS_ZERO |            /* destBlendCaps */ \
        D3DPBLENDCAPS_INVSRCALPHA,                         \
    0,                              /* alphaBlendCaps */    \
    D3DPSHADECAPS_COLORFLATRGB |    /* shadeCaps */         \
        D3DPSHADECAPS_SPECULARFLATRGB |                         \
        D3DPSHADECAPS_COLORGOURAUDRGB |                         \
        D3DPSHADECAPS_SPECULARGOURAUDRGB |                      \
        D3DPSHADECAPS_FOGFLAT |                             \
        D3DPSHADECAPS_FOGGOURAUD |                          \
        D3DPSHADECAPS_ALPHAFLATBLEND |                      \
		D3DPSHADECAPS_ALPHAGOURAUDBLEND,					\
    D3DPTEXTURECAPS_PERSPECTIVE |   /* textureCaps */       \
        D3DPTEXTURECAPS_POW2 |                              \
        D3DPTEXTURECAPS_SQUAREONLY |                            \
        D3DPTEXTURECAPS_ALPHA,                             \
    D3DPTFILTERCAPS_NEAREST |       /* textureFilterCaps */ \
        D3DPTFILTERCAPS_LINEAR |                            \
        D3DPTFILTERCAPS_MIPNEAREST | /* mipmap */           \
        D3DPTFILTERCAPS_MIPLINEAR |  /* mipmap */           \
        D3DPTFILTERCAPS_LINEARMIPNEAREST | /* mipmap */     \
        D3DPTFILTERCAPS_LINEARMIPLINEAR, /* mipmap */       \
    D3DPTBLENDCAPS_DECAL |          /* textureBlendCaps */  	\
        D3DPTBLENDCAPS_MODULATE |					\
        D3DPTBLENDCAPS_DECALALPHA |				\
        D3DPTBLENDCAPS_MODULATEALPHA |				\
        D3DPTBLENDCAPS_COPY |                               	\
        D3DPTBLENDCAPS_ADD,                           		\
    0,                              /* textureAddressCaps */\
    0,                              /* stippleWidth */      \
    0                               /* stippleHeight */     \
}


#define triCaps {                                           \
    sizeof(D3DPRIMCAPS),                                    \
        D3DPMISCCAPS_CULLNONE |                 \
        D3DPMISCCAPS_CULLCW |                   \
        D3DPMISCCAPS_CULLCCW, /* miscCaps */                  \
    D3DPRASTERCAPS_DITHER |         /* rasterCaps */        \
        D3DPRASTERCAPS_SUBPIXEL |       \
        D3DPRASTERCAPS_FOGVERTEX,       \
    D3DPCMPCAPS_NEVER |             \
        D3DPCMPCAPS_LESS |              \
        D3DPCMPCAPS_EQUAL |             \
        D3DPCMPCAPS_LESSEQUAL |         \
        D3DPCMPCAPS_GREATER |           \
        D3DPCMPCAPS_NOTEQUAL |          \
        D3DPCMPCAPS_GREATEREQUAL |      \
        D3DPCMPCAPS_ALWAYS,                  /* zCmpCaps */     \
    D3DPBLENDCAPS_ONE |              /* sourceBlendCaps */   \
        D3DPBLENDCAPS_SRCALPHA,                         \
    D3DPBLENDCAPS_ZERO |            /* destBlendCaps */ \
        D3DPBLENDCAPS_INVSRCALPHA,                         \
    0,                              /* alphaBlendCaps */    \
    D3DPSHADECAPS_COLORFLATRGB |    /* shadeCaps */         \
        D3DPSHADECAPS_SPECULARFLATRGB |                         \
        D3DPSHADECAPS_COLORGOURAUDRGB |                         \
        D3DPSHADECAPS_SPECULARGOURAUDRGB |                      \
        D3DPSHADECAPS_FOGFLAT |                             \
        D3DPSHADECAPS_FOGGOURAUD |                          \
        D3DPSHADECAPS_ALPHAFLATBLEND |                      \
		D3DPSHADECAPS_ALPHAGOURAUDBLEND,					\
    D3DPTEXTURECAPS_PERSPECTIVE |   /* textureCaps */       \
        D3DPTEXTURECAPS_POW2 |                              \
        D3DPTEXTURECAPS_SQUAREONLY |                            \
        D3DPTEXTURECAPS_ALPHA,                             \
    D3DPTFILTERCAPS_NEAREST |       /* textureFilterCaps */ \
        D3DPTFILTERCAPS_LINEAR |                            \
        D3DPTFILTERCAPS_MIPNEAREST | /* mipmap */           \
        D3DPTFILTERCAPS_MIPLINEAR |  /* mipmap */           \
        D3DPTFILTERCAPS_LINEARMIPNEAREST | /* mipmap */     \
        D3DPTFILTERCAPS_LINEARMIPLINEAR, /* mipmap */       \
    D3DPTBLENDCAPS_DECAL |          /* textureBlendCaps */  	\
        D3DPTBLENDCAPS_MODULATE |					\
        D3DPTBLENDCAPS_DECALALPHA |				\
        D3DPTBLENDCAPS_MODULATEALPHA |				\
        D3DPTBLENDCAPS_COPY |                               	\
        D3DPTBLENDCAPS_ADD,                           		\
    D3DPTADDRESSCAPS_WRAP |         /* textureAddressCaps */\
		D3DPTADDRESSCAPS_CLAMP,                             \
    0,                              /* stippleWidth */      \
    0                               /* stippleHeight */     \
}



static D3DDEVICEDESC_V1 myCaps = {
    sizeof(D3DDEVICEDESC_V1),      /* dwSize */
    D3DDD_COLORMODEL |            /* dwFlags */
        D3DDD_DEVCAPS |
        D3DDD_TRICAPS |
        D3DDD_LINECAPS |
        D3DDD_DEVICERENDERBITDEPTH |
        D3DDD_DEVICEZBUFFERBITDEPTH,
    D3DCOLOR_RGB,               /* dcmColorModel */
    D3DDEVCAPS_EXECUTESYSTEMMEMORY | /* devCaps */
    	D3DDEVCAPS_TEXTUREVIDEOMEMORY |
    	D3DDEVCAPS_TLVERTEXSYSTEMMEMORY |
        D3DDEVCAPS_FLOATTLVERTEX |
	 D3DDEVCAPS_DRAWPRIMTLVERTEX |
        D3DDEVCAPS_SORTINCREASINGZ,
//    D3DDEVCAPS_FLOATTLVERTEX |
//        D3DDEVCAPS_TEXTUREVIDEOMEMORY |
//        D3DDEVCAPS_SORTINCREASINGZ,   /* devCaps */
    { sizeof(D3DTRANSFORMCAPS), 0 },            /* dtcTransformCaps */
    FALSE,                      /* bClipping */
    { sizeof(D3DLIGHTINGCAPS),0,0,0 },  /* dlcLightingCaps */
    lineCaps,                   /* lineCaps */
    triCaps,                    /* triCaps */
    DDBD_16 | DDBD_24,          /* dwDeviceRenderBitDepth */
    DDBD_16,                    /* dwDeviceZBufferBitDepth */
    0,                          /* dwMaxBufferSize */
    0                           /* dwMaxVertexCount */
};

static DDSURFACEDESC myTextureFormats[] = {
    {
    sizeof(DDSURFACEDESC),              /* dwSize */
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */
    0,                                  /* dwHeight */
    0,                                  /* dwWidth */
    0,                                  /* lPitch */
    0,                                  /* dwBackBufferCount */
    0,                                  /* dwZBufferBitDepth */
    0,                                  /* dwAlphaBitDepth */
    0,                                  /* dwReserved */
    NULL,                               /* lpSurface */
    { 0, 0 },                           /* ddckCKDestOverlay */
    { 0, 0 },                           /* ddckCKDestBlt */
    { 0, 0 },                           /* ddckCKSrcOverlay */
    { 0, 0 },                           /* ddckCKSrcBlt */
    {
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */
      DDPF_RGB,                         /* ddpfPixelFormat.dwFlags */
      0,                                /* FOURCC code */
      16,                               /* ddpfPixelFormat.dwRGBBitCount */
      0x7c00,
      0x03e0,
      0x001f,
      0
    },
    DDSCAPS_TEXTURE, /* ddscaps.dwCaps */
    },
    {
    sizeof(DDSURFACEDESC),              /* dwSize */
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */
    0,                                  /* dwHeight */
    0,                                  /* dwWidth */
    0,                                  /* lPitch */
    0,                                  /* dwBackBufferCount */
    0,                                  /* dwZBufferBitDepth */
    0,                                  /* dwAlphaBitDepth */
    0,                                  /* dwReserved */
    NULL,                               /* lpSurface */
    { 0, 0 },                           /* ddckCKDestOverlay */
    { 0, 0 },                           /* ddckCKDestBlt */
    { 0, 0 },                           /* ddckCKSrcOverlay */
    { 0, 0 },                           /* ddckCKSrcBlt */
    {
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */
      DDPF_RGB,                         /* ddpfPixelFormat.dwFlags */
      0,                                /* FOURCC code */
      32,                               /* ddpfPixelFormat.dwRGBBitCount */
      0xff0000,
      0xff00,
      0xff,
      0
    },
    DDSCAPS_TEXTURE,                   /* ddscaps.dwCaps */
    },
    {
    sizeof(DDSURFACEDESC),              /* dwSize */
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */
    0,                                  /* dwHeight */
    0,                                  /* dwWidth */
    0,                                  /* lPitch */
    0,                                  /* dwBackBufferCount */
    0,                                  /* dwZBufferBitDepth */
    0,                                  /* dwAlphaBitDepth */
    0,                                  /* dwReserved */
    NULL,                               /* lpSurface */
    { 0, 0 },                           /* ddckCKDestOverlay */
    { 0, 0 },                           /* ddckCKDestBlt */
    { 0, 0 },                           /* ddckCKSrcOverlay */
    { 0, 0 },                           /* ddckCKSrcBlt */
    {
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */
      DDPF_RGB | DDPF_ALPHAPIXELS,      /* ddpfPixelFormat.dwFlags */
      0,                                /* FOURCC code */
      16,                               /* ddpfPixelFormat.dwRGBBitCount */
      0x7c00,
      0x03e0,
      0x001f,
      0x8000
    },
    DDSCAPS_TEXTURE, /* ddscaps.dwCaps */
    },
    {
    sizeof(DDSURFACEDESC),              /* dwSize */
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */
    0,                                  /* dwHeight */
    0,                                  /* dwWidth */
    0,                                  /* lPitch */
    0,                                  /* dwBackBufferCount */
    0,                                  /* dwZBufferBitDepth */
    0,                                  /* dwAlphaBitDepth */
    0,                                  /* dwReserved */
    NULL,                               /* lpSurface */
    { 0, 0 },                           /* ddckCKDestOverlay */
    { 0, 0 },                           /* ddckCKDestBlt */
    { 0, 0 },                           /* ddckCKSrcOverlay */
    { 0, 0 },                           /* ddckCKSrcBlt */
    {
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */
      DDPF_RGB | DDPF_ALPHAPIXELS,      /* ddpfPixelFormat.dwFlags */
      0,                                /* FOURCC code */
      32,                               /* ddpfPixelFormat.dwRGBBitCount */
      0xff0000,
      0xff00,
      0xff,
      0xff000000
    },
    DDSCAPS_TEXTURE                    /* ddscaps.dwCaps */
    },
    {
    sizeof(DDSURFACEDESC),              /* dwSize */
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */
    0,                                  /* dwHeight */
    0,                                  /* dwWidth */
    0,                                  /* lPitch */
    0,                                  /* dwBackBufferCount */
    0,                                  /* dwZBufferBitDepth */
    0,                                  /* dwAlphaBitDepth */
    0,                                  /* dwReserved */
    NULL,                               /* lpSurface */
    { 0, 0 },                           /* ddckCKDestOverlay */
    { 0, 0 },                           /* ddckCKDestBlt */
    { 0, 0 },                           /* ddckCKSrcOverlay */
    { 0, 0 },                           /* ddckCKSrcBlt */
    {
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */
      DDPF_RGB | DDPF_ALPHAPIXELS,      /* ddpfPixelFormat.dwFlags */
      0,                                /* FOURCC code */
      16,                               /* ddpfPixelFormat.dwRGBBitCount */
      0xf00,
      0xf0,
      0xf,
      0xf000
    },
    DDSCAPS_TEXTURE                    /* ddscaps.dwCaps */
    },
#if 0
    // possible YUV texture
    {
    sizeof(DDSURFACEDESC),              /* dwSize */
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */
    0,                                  /* dwHeight */
    0,                                  /* dwWidth */
    0,                                  /* lPitch */
    0,                                  /* dwBackBufferCount */
    0,                                  /* dwZBufferBitDepth */
    0,                                  /* dwAlphaBitDepth */
    0,                                  /* dwReserved */
    NULL,                               /* lpSurface */
    { 0, 0 },                           /* ddckCKDestOverlay */
    { 0, 0 },                           /* ddckCKDestBlt */
    { 0, 0 },                           /* ddckCKSrcOverlay */
    { 0, 0 },                           /* ddckCKSrcBlt */
    {
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */
      DDPF_YUV,                         /* ddpfPixelFormat.dwFlags */
      0,                                /* FOURCC code */
      32,                               /* ddpfPixelFormat.dwYUVBitCount */
      0xff0000,
      0xff00,
      0xff,
      0xff000000
    },
    DDSCAPS_TEXTURE                    /* ddscaps.dwCaps */
    },
#endif
};


D3DHAL_GLOBALDRIVERDATA myGlobal;

/*----------------------------01-26-96 S3 11:36am-----------------------------
 D3DHALCreateDriver

    LPD3DHAL_GLOBALDRIVERDATA* lplpGlobal
        - A pointer to a structure containing alot of global information about
          our driver.

    LPD3DHAL_CALLBACKS* lplpHALCallbacks
        - A pointer to a structure we fill in with the callbacks that this
          driver supports.
----------------------------------------------------------------------------*/
/* --------------------------------------------------------------
 * Functions used to instantiate the 3D portion of the DirectDraw HAL
 *
 * Will be called (for example) from invokeCreateDriver() from ddsamp.
 */

BOOL __stdcall
D3DHALCreateDriver(LPD3DHAL_GLOBALDRIVERDATA *lplpGlobal,
                   LPD3DHAL_CALLBACKS *lplpHALCallbacks)
{   /*
     * You need to alloc space for vertex transform destination.
     */


    DPF("CreateDriver");
    DPF("S3THKINFO=%d D3DGLOBALS=%d K2VTABLE=%d BAND=%d FLIPRECORD=%d LPB=%d DDKERNELCAPS=%d DDHALINFO=%d DDPIXELFORMAT=%d",
    sizeof(S3THKINFO),sizeof(D3DGLOBALS),sizeof(K2VTABLE),sizeof(BAND),sizeof(FLIPRECORD),sizeof(LPB),
    sizeof(DDKERNELCAPS),sizeof(DDHALINFO),sizeof(DDPIXELFORMAT));
    /*
     * Here we fill in the supplied structures.
     */
    memset(&myGlobal, 0, sizeof(D3DHAL_GLOBALDRIVERDATA));
    myGlobal.dwSize = sizeof(D3DHAL_GLOBALDRIVERDATA);
    myGlobal.hwCaps = myCaps;
    myGlobal.dwNumVertices = 0;
    myGlobal.dwNumClipVertices = 0;
    myGlobal.dwNumTextureFormats = sizeof(myTextureFormats)/sizeof(DDSURFACEDESC);
    myGlobal.lpTextureFormats = &myTextureFormats[0];

   /*
    * Return the HAL table.
    */
    *lplpGlobal                 = &myGlobal;
    *lplpHALCallbacks       = &myD3DHALCallbacks;

    CalcStackAdjust();

    return (TRUE);
}


#ifdef WINNT
BOOL DrvGetDirect3DInfo(DHPDEV dhpdev,
                        LPD3DNTHAL_CALLBACKS pCallbacks,
                        LPD3DNTHAL_GLOBALDRIVERDATA pDriverData)
{
    LPD3DNTHAL_GLOBALDRIVERDATA pdnhgdd;
    LPD3DNTHAL_CALLBACKS pdnhc;

    DPF("DrvGetDirect3DInfo");

    if (!D3DHALCreateDriver(&pdnhgdd, &pdnhc))
    {
        return FALSE;
    }

    *pCallbacks = *pdnhc;
    *pDriverData = *pdnhgdd;

    return TRUE;
}
#endif

/*
 * DPF debug stuff
 */
#if defined(DEBUG) || defined(DBG)

/*
 * Msg
 */
#ifdef WINNT
void __cdecl DPF(LPSTR szFormat, ...)
{
    va_list args;

    va_start(args, szFormat);
    EngDebugPrint("Mga:", szFormat, args);
    EngDebugPrint("", "\n", NULL);
    va_end(args);
}
#else
void __cdecl DPF(LPSTR szFormat, ...)
{
    char str[256];

    lstrcpy(str, START_STR);
    wvsprintf(str+lstrlen(str), szFormat, (LPVOID)(&szFormat+1));
    lstrcat(str, END_STR);
    OutputDebugString(str);
}
#endif
#endif

