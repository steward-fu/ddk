/*==========================================================================;
*
*  Copyright (C) 1996 Microsoft Corporation.  All Rights Reserved.
*
*  File:       d3dtex.c
*  Content:    Direct3D texture management
*
***************************************************************************/

#include "precomp.h"
#include "d3ddrv.h"

/*----------------------------------------------------------------------------
myTextureCreate

LPD3DHAL_TEXTURECREATEDATA ptcd
- pointer to a structure containing (among other things) the surface
to be used for this texture.
----------------------------------------------------------------------------*/
DWORD __stdcall myTextureCreate(LPD3DHAL_TEXTURECREATEDATA ptcd) {
/*
* NOTES:
*
* This callback is invoked when a texture is to be created from a
* DirectDrawSurface.
* We must pass back the created handle.
*/
    LPS3_CONTEXT    ctxt;
    LPS3_TEXTURE    lpTexture;

// Get Pointer to context
    ctxt = (LPS3_CONTEXT)ptcd->dwhContext;
    if (! ctxt) {
        DPF( "in myTextureCreate, bad context =%08lx", ptcd->dwhContext);
        ptcd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
// Get Pointer to texture
    lpTexture = (LPS3_TEXTURE)MEMALLOC(sizeof(S3_TEXTURE));
    if (! lpTexture) {
        DPF( "in myTextureCreate, MEMALLOC returns NULL, failing");
        ptcd->ddrval = D3DERR_TEXTURE_CREATE_FAILED;
        return (DDHAL_DRIVER_HANDLED);
    }
    lpTexture->Next=ctxt->TextureList;
    if (lpTexture->Next)    lpTexture->Next->Last=lpTexture;
    ctxt->TextureList=lpTexture;
    ptcd->dwHandle = (DWORD)lpTexture;

// Generic initialization
    lpTexture->pid      = ctxt->dwPID;
    lpTexture->hContext = (DWORD)ctxt;
#ifndef WINNT
    lpTexture->lpDDS = ptcd->lpDDS;
    lpTexture->lpLcl    = ((LPDDRAWI_DDRAWSURFACE_INT)ptcd->lpDDS)->lpLcl;
#else
    lpTexture->hTexture = ptcd->hDDS;
#endif
    
    //DPF( "in myTextureCreate,context =%08lx lpTexture=%08lx lpDDS=%08lx", ctxt,lpTexture,lpTexture->lpDDS);
    ptcd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

}                                                 /* myTextureCreate */


/*----------------------------------------------------------------------------
myTextureDestroy

LPD3DHAL_TEXTUREDESTROYDATA ptdd
----------------------------------------------------------------------------*/
DWORD __stdcall myTextureDestroy(LPD3DHAL_TEXTUREDESTROYDATA ptdd) {
    LPS3_CONTEXT ctxt;
    LPS3_TEXTURE lpTexture;

// Get Pointer to context
    ctxt = (LPS3_CONTEXT) ptdd->dwhContext;
    if (! ctxt) {
        DPF( "in myTextureDestroy, bad context =%08lx", ptdd->dwhContext);
        ptdd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

// Get Pointer to texture
    lpTexture = (LPS3_TEXTURE) ptdd->dwHandle;
    //DPF( "in myTextureDestroy,context =%08lx lpTexture=%08lx", ctxt,lpTexture);

    #ifndef WINNT
    if (! lpTexture) {
        DPF( "in myTextureDestroy, bad texture =%08lx", ptdd->dwHandle);
        ptdd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }
    #endif

// Make sure this context no longer refers
// to this texture handle
        if (ctxt->dwTexture == (DWORD)lpTexture){
            ctxt->dwTexture = 0;
        ctxt->RStyle.bTextured=FALSE;
              ctxt->bChanged = TRUE;
        }
    if (lpTexture->Next)    lpTexture->Next->Last=lpTexture->Last;
    if (lpTexture->Last)    
        lpTexture->Last->Next=lpTexture->Next;
    else
        ctxt->TextureList=lpTexture->Next;
        MEMFREE(lpTexture);
        ptdd->dwHandle=0;

        ptdd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);

}                                                 /* myTextureDestroy */


/*
* myTextureSwap
*/
DWORD __stdcall myTextureSwap(LPD3DHAL_TEXTURESWAPDATA ptsd) {
/*
* NOTES:
*
* This callback is invoked when two texture handles are to be swapped.
* I.e. the data refered to by the two handles is to be swapped.
*/
    S3_CONTEXT *ctxt;
    LPS3_TEXTURE lpTex1, lpTex2;
    S3_TEXTURE TempTex;

// Get Pointer to context
    ctxt = (LPS3_CONTEXT) ptsd->dwhContext;
    if (! ctxt) {
        DPF( "in myTextureDestroy, bad context =%08lx", ptsd->dwhContext);
        ptsd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    DPF( "in TextureSwap, ptsd->dwhContext =%08lx,", ptsd->dwhContext);
    DPF( "                ptsd->dwHandle1 = %08lx, ptsd->dwHandle2 = %08lx",
    ptsd->dwHandle1, ptsd->dwHandle2);

// Get Texture 1
    lpTex1 = (LPS3_TEXTURE) ptsd->dwHandle1;
    if (! lpTex1) {
        DPF( "in myTextureSwap, bad texture =%08lx", ptsd->dwHandle1);
        ptsd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }

// Get Texture 2
    lpTex2 = (LPS3_TEXTURE) ptsd->dwHandle2;
    if (! lpTex2) {
        DPF( "in myTextureSwap, bad texture =%8lx", ptsd->dwHandle2);
        ptsd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }

    TempTex = *lpTex1;
    memcpy((LPBYTE)lpTex1,(LPBYTE)lpTex2,(sizeof(S3_TEXTURE)-2*sizeof(LPS3_TEXTURE)));  //Don't swap link list pointers
    memcpy((LPBYTE)lpTex2,(LPBYTE)&TempTex,(sizeof(S3_TEXTURE)-2*sizeof(LPS3_TEXTURE)));

    if (ctxt->dwTexture == ptsd->dwHandle1 ||
        ctxt->dwTexture == ptsd->dwHandle2) {
        ctxt->bChanged = TRUE;
    }

    ptsd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

}                                                 /* myTextureSwap */


/*
* myTextureGetSurf
*/
DWORD __stdcall myTextureGetSurf(LPD3DHAL_TEXTUREGETSURFDATA ptgd) {
/*
* NOTES:
*
* This callback is invoked when the d3d needs to obtain the surface
* refered to by a handle.
*/
    S3_CONTEXT *ctxt;
    LPS3_TEXTURE lpTexture;

// Get Pointer to context
    ctxt = (LPS3_CONTEXT)ptgd->dwhContext;
    if ( ! ctxt ) {
        DPF( "in myTextureGetSurf, bad context =%08lx", ptgd->dwhContext);
        ptgd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

// Get Texture
    lpTexture = (LPS3_TEXTURE) ptgd->dwHandle;
    if (! lpTexture) {
// Error
        DPF( "in myTextureGetSurf, bad texture handle =%08lx", ptgd->dwHandle);
        ptgd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }

    #ifndef WINNT
    ptgd->lpDDS = (DWORD)lpTexture->lpDDS;
    #else
    ptgd->hDDS = lpTexture->hTexture;
    #endif

    DPF( "in TextureGetSurf, ptgd->dwhContext =%08lx,", ptgd->dwhContext);
    DPF( "                   ptgd->dwHandle = %d", ptgd->dwHandle);

    ptgd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

}                                                 /* myTextureGetSurf */

