/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3dmini.c
 *  Content:    Direct3D Sample HAL Driver
 *
 ***************************************************************************/

#include <windows.h>
#include <stddef.h>
#include "d3dhal.h"
#include "ddrvmem.h"    // Dynamic Array code

#define START_STR       "+++DRV: "
#define END_STR         "\r\n"

#ifndef DEBUG
    #define DPF         1 ? (void)0 : (void)
#else

/*
 * Msg
 * Defined in ddsamp.c
 */
//@@BEGIN_MSINTERNAL
#ifdef FAKE_HAL
static void __cdecl DPF(LPSTR szFormat, ...)
{
    char str[256];

    lstrcpy(str, START_STR);
    wvsprintf(str+lstrlen(str), szFormat, (LPVOID)(&szFormat+1));
    lstrcat(str, END_STR);
    OutputDebugString(str);
}
#else
//@@END_MSINTERNAL
extern void __cdecl DPF( LPSTR szFormat, ... );
//@@BEGIN_MSINTERNAL
#endif
//@@END_MSINTERNAL
#endif

#define IS_OVERRIDE(type)	((DWORD)(type) > D3DSTATE_OVERRIDE_BIAS)
#define GET_OVERRIDE(type)	((DWORD)(type) - D3DSTATE_OVERRIDE_BIAS)

#define MAX_STATE	D3DSTATE_OVERRIDE_BIAS
#define DWORD_BITS	32
#define DWORD_SHIFT	5

typedef struct _D3DSAMPStateSet {
    DWORD		bits[MAX_STATE >> DWORD_SHIFT];
} D3DSAMPStateSet;

#define STATESET_MASK(set, state)	\
	(set).bits[((state) - 1) >> DWORD_SHIFT]

#define STATESET_BIT(state)	(1 << (((state) - 1) & (DWORD_BITS - 1)))

#define STATESET_ISSET(set, state) \
	STATESET_MASK(set, state) & STATESET_BIT(state)

#define STATESET_SET(set, state) \
	STATESET_MASK(set, state) |= STATESET_BIT(state)

#define STATESET_CLEAR(set, state) \
	STATESET_MASK(set, state) &= ~STATESET_BIT(state)

#define STATESET_INIT(set)	memset(&(set), 0, sizeof(set))

/****************************************************************************
 *
 *
 * The following functions (my....) are the hardware specific routines
 * that the driver needs to implement to make Direct3D functionality work
 *
 *
 ***************************************************************************/
 
typedef struct _D3DSAMP_Context {
    DWORD               information_that_i_need;
    DWORD               for_my_context;
    D3DSAMPStateSet	    overrides;
    BOOL		        z_visible;
    LPDIRECTDRAWSURFACE lpDDS;
    LPDIRECTDRAWSURFACE lpDDSZ;
    DWORD		        pid;
} D3DSAMP_Context;

typedef struct _D3DSAMP_Texture {
    DWORD               information_that_i_need;
    DWORD               for_my_texture;
    LPDIRECTDRAWSURFACE lpDDS;      // Pointer to texture surface
    DWORD               hContext;   // Handle to texture's context
} D3DSAMP_Texture;

#define CONTEXTS_DELTA 32
#define TEXTURES_DELTA 128


//#define D3DSAMP_NUMCONTEXTS 256
//#define D3DSAMP_NUMTEXTURES 1024

//static BOOL myContextMap[D3DSAMP_NUMCONTEXTS];
//static D3DSAMP_Context myContexts[D3DSAMP_NUMCONTEXTS];
//static D3DSAMP_Texture myTextures[D3DSAMP_NUMTEXTURES];

// These dynamic arrays do not support locking/unlocking
// Pointers can become invalid, if a function call can
// be interrupted and re-entered.   This normally shouldn't
// be a problem.
static HDDRVITEMARRAY hContexts = 0;
static HDDRVITEMARRAY hTextures = 0;


/*
 * myContextCreate
 */
DWORD __stdcall myContextCreate(LPD3DHAL_CONTEXTCREATEDATA pccd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a new surface is to be used as a
     * rendering target.
     * The context handled returned will be used whenever rendering
     * to this surface is to be performed.
     */
    D3DSAMP_Context* newContext;
    HDDRVITEM hContext;
    int i;
    
    // Create Context
    if (! DDrvItemArrayAlloc (hContexts, &hContext))
    {
        DPF( "in myContextCreate, unable to create context");
        pccd->ddrval = D3DHAL_OUTOFCONTEXTS;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Get Pointer to context
    newContext = (D3DSAMP_Context *)DDrvItemArrayGetPtr (hContexts, hContext);
    if (! newContext)
    {
        DPF( "in myContextCreate, unable to access context");
        pccd->ddrval = D3DHAL_OUTOFCONTEXTS;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Return handle to new context
    pccd->dwhContext = (DWORD)hContext;

    DPF( "in myContextCreate, pccd->lpDDGbl =%08lx, pccd->lpDDS =%08lx",
                                pccd->lpDDGbl, pccd->lpDDS);
    DPF( "                    pccd->dwhContext =%08lx, pccd->lpDDSZ= %08lx",
                                pccd->dwhContext, pccd->lpDDSZ);

    // Initialize new context

    // On context creation, no render states are overridden    
    STATESET_INIT(newContext->overrides);

    // Do any context specific initialization here !!!

    newContext->z_visible = FALSE;
    newContext->pid = pccd->dwPID;

    pccd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myContextCreate */

/*
 * myContextDestroy
 */
DWORD __stdcall myContextDestroy(LPD3DHAL_CONTEXTDESTROYDATA pcdd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a context is to be destroyed.
     * Perform any cleanup required.
     */
    D3DSAMP_Context*    aContext;

    // Get Pointer to context
    aContext = (D3DSAMP_Context *)DDrvItemArrayGetPtr (hContexts, (HDDRVITEM)pcdd->dwhContext);
    if (! aContext)
    {
        DPF( "in myContextDestroy, bad context =0x08lx", pcdd->dwhContext);
        pcdd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }


    // Do any context specfic cleanup here !!!

    // Delete Context
    DDrvItemArrayFree (hContexts, (HDDRVITEM)pcdd->dwhContext);

    DPF( "in myContextDestroy, pcdd->dwhContext =%08lx", pcdd->dwhContext);

    pcdd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myContextDestroy */



/*
 * myContextDestroyAllCallback
 */
DWORD myContextDestroyAllCallback (LPVOID lpData, HDDRVITEM hItem, DWORD dwData)
{
    D3DSAMP_Context * lpContext = (D3DSAMP_Context *)lpData;
    DWORD dwPID = dwData;

    if (!lpContext)
        return DDRV_SUCCESS_CONTINUE;

    // Destroy Context if Process ID matches
    if (lpContext->pid == dwPID)
    {
        DPF( "in myContextDestroyAll, cleaning up context %08lx", hItem);
        DDrvItemArrayFree(hContexts, hItem);
    }

    return DDRV_SUCCESS_CONTINUE;
} /* myContextDestroyAllCallback */


/*
 * myContextDestroyAll
 */
DWORD __stdcall myContextDestroyAll(LPD3DHAL_CONTEXTDESTROYALLDATA pcdd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a process dies.  All the contexts
     * which were created by this context need to be destroyed.
     */
    DPF( "in myContextDestroyAll, pcdd->dwPID =%08lx", pcdd->dwPID);

    // Destroy all valid context's with matching process ID
    DDrvItemArrayApplyValid (hContexts, myContextDestroyAllCallback, pcdd->dwPID);

    pcdd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
} /* myContextDestroyAll */

//@@BEGIN_MSINTERNAL


#ifdef IMPLEMENT_EXECUTE

#define NEXTINSTRUCTION(ptr, type, num) \
        ptr = (LPD3DINSTRUCTION)((LPBYTE)ptr + sizeof(D3DINSTRUCTION) + \
              (num * sizeof(type)))
/*
 * myExecute
 */
DWORD __stdcall myExecute(LPD3DHAL_EXECUTEDATA ped)
{
    /*
     * NOTES:
     *
     * This callback is invoked when an execute buffer is to be executed.
     * All the data is known to be clipped.
     */
    D3DSAMP_Context*    aContext;
    LPD3DINSTRUCTION    lpIns;
    LPD3DTLVERTEX       lpVertices;
    LPBYTE              prim;
    LPBYTE		        lpData;
    BOOL                oneInstruction = FALSE;
    
    // Get context from context handle
    aContext = (D3DSAMP_Context *)DDrvItemArrayGetPtr (hContexts, ped->dwhContext);
    if (! aContext)
    {
        DPF( "in myExecute, bad context =0x08lx", ped->dwhContext);
        ped->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    DPF( "in myExecute, ped->dwhContext =%08lx", ped->dwhContext);
    
    lpData = (LPBYTE)(((LPDDRAWI_DDRAWSURFACE_INT)ped->lpExeBuf)->lpLcl->lpGbl->fpVidMem);
    if (ped->dwFlags & D3DHAL_EXECUTE_OVERRIDE) {
        oneInstruction = TRUE;

        /*
         * NOTE: In this case dwOffset is the offset to the PRIMITIVE
         *       NOT the instruction.
         * You will only get called with handleable instructions.
         *
         * Examples follows:
         */
        lpIns = &ped->diInstruction;
        prim = lpData + ped->dwOffset;

    } else {
        /*
         * NOTE: In this case dwOffset is the offset to the INSTRUCTION
         *       offset from the data.instructionOffset.
         *
         * Examples follows:
         */
        lpIns = (LPD3DINSTRUCTION)(lpData +
                                   ped->deExData.instructionOffset +
                                   ped->dwOffset);
        prim = (LPBYTE)lpIns + sizeof(D3DINSTRUCTION);
    }

    lpVertices = (LPD3DTLVERTEX)(((LPDDRAWI_DDRAWSURFACE_INT)ped->lpTLBuf)->lpLcl->lpGbl->fpVidMem);
                               
    /*
     * Parse the instructions.
     */
    while (lpIns->bOpcode != D3DOP_EXIT) {
        switch (lpIns->bOpcode) {
            case D3DOP_POINT:
                DPF( "in myExecute, D3DOP_POINT instruction count = %d",
                        lpIns->wCount);
                NEXTINSTRUCTION(lpIns, D3DPOINT, lpIns->wCount);
                break;
            case D3DOP_LINE:
                DPF( "in myExecute, D3DOP_LINE instruction count = %d",
                        lpIns->wCount);
                NEXTINSTRUCTION(lpIns, D3DLINE, lpIns->wCount);
                break;
            case D3DOP_SPAN:
                DPF( "in myExecute, D3DOP_SPAN instruction count = %d",
                        lpIns->wCount);
                NEXTINSTRUCTION(lpIns, D3DSPAN, lpIns->wCount);
            case D3DOP_TRIANGLE:
	    	{
		    int i;
		    D3DTRIANGLE *triPtr = (D3DTRIANGLE *)prim;

		    DPF( "in myExecute, D3DOP_TRIANGLE instruction count = %d",
			    lpIns->wCount);

		    for( i = 0; i < lpIns.wCount; i++ )
		    {
			DPF( "Tri: v1 = %d, v2 = %d, v3 = %d\n",
				    triPtr->v1, triPtr->v2, triPtr->v3 );
			triPtr++;
		    }
    
		    NEXTINSTRUCTION(lpIns, D3DTRIANGLE, lpIns->wCount);
	    	}
                break;
            case D3DOP_STATERENDER:
                DPF( "in myExecute, D3DOP_STATERENDER instruction count = %d",
                        lpIns->wCount);
                NEXTINSTRUCTION(lpIns, D3DSTATE, lpIns->wCount);
                break;
            default:
                DPF( "in myExecute, Unhandled instruction opcode = %d",
                        lpIns->bOpcode);
                if (oneInstruction) {
                    DPF( "Asked to execute an instruction we don't understand.");
                    ped->ddrval = D3DHAL_EXECUTE_UNHANDLED;
                    return (DDHAL_DRIVER_NOTHANDLED);
                }
                ped->dwOffset = (DWORD)((LPBYTE)lpIns -
                                 (lpData +
                                  ped->deExData.instructionOffset));
                ped->ddrval = D3DHAL_EXECUTE_UNHANDLED;
                return (DDHAL_DRIVER_HANDLED);
        }
        if (oneInstruction) {
                break;
        }
        prim = (LPBYTE)lpIns + sizeof(D3DINSTRUCTION);
    }
    
    ped->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myExecute */

#endif /* IMPLEMENT_EXECUTE */

//@@END_MSINTERNAL


/*
 * myRenderstate
 */
DWORD __stdcall myRenderState(LPD3DHAL_RENDERSTATEDATA prd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a set of render states are changed.
     */
    D3DSAMP_Context*    aContext;
    LPBYTE		lpData;
    LPD3DSTATE		lpState;
    DWORD		i;
    
    // Get context from context handle
    aContext = (D3DSAMP_Context *)DDrvItemArrayGetPtr (hContexts, prd->dwhContext);
    if (! aContext)
    {
        DPF( "in myRenderState, bad context =0x08lx", prd->dwhContext);
        prd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in myRenderState, prd->dwhContext =%08lx", prd->dwhContext);
    
    lpData = (LPBYTE)(((LPDDRAWI_DDRAWSURFACE_INT)prd->lpExeBuf)->lpLcl->lpGbl->fpVidMem);
                               
    for (i = 0, lpState = (LPD3DSTATE) (lpData + prd->dwOffset);
	 i < prd->dwCount; i++, lpState++) {
	DWORD type = (DWORD) lpState->drstRenderStateType;

	DPF( "in myRenderState, state = %d", lpState->drstRenderStateType);
	if (IS_OVERRIDE(type)) {
	    DWORD override = GET_OVERRIDE(type);
	    if (lpState->dwArg[0]) {
		DPF( "in myRenderState, setting override for state %d", override);
		STATESET_SET(aContext->overrides, override);
	    } else {
		DPF( "in myRenderState, clearing override for state %d", override);
		STATESET_CLEAR(aContext->overrides, override);
	    }
	    continue;
	}

	if (STATESET_ISSET(aContext->overrides, type)) {
	    DPF( "in myRenderState, state %d is overridden, ignoring", type);
	    continue;
	}

	DPF( "in myRenderState, setting state %d to value %d", type, lpState->dwArg[0]);

	if (lpState->drstRenderStateType == D3DRENDERSTATE_ZVISIBLE) {
	    aContext->z_visible = lpState->dwArg[0];
	    DPF( "in myRenderState, z_visible = %d", aContext->z_visible);
	}
    }

    prd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myRenderState */


/*
 * myRenderPrimitive
 */
DWORD __stdcall myRenderPrimitive(LPD3DHAL_RENDERPRIMITIVEDATA prd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a primitive is to be rendered.
     * All the data is known to be clipped.
     */
    D3DSAMP_Context*    aContext;
    LPD3DINSTRUCTION    lpIns;
    LPD3DTLVERTEX       lpVertices;
    LPBYTE		lpData, prim;
    int			i;
    
    // Get context from context handle
    aContext = (D3DSAMP_Context *)DDrvItemArrayGetPtr (hContexts, prd->dwhContext);
    if (! aContext)
    {
        DPF( "in myRenderPrimitive, bad context =0x08lx", prd->dwhContext);
        prd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in myRenderPrimitive, prd->dwhContext =%08lx", prd->dwhContext);
    
    lpData = (LPBYTE)(((LPDDRAWI_DDRAWSURFACE_INT)prd->lpExeBuf)->lpLcl->lpGbl->fpVidMem);
    lpIns = &prd->diInstruction;
    prim = lpData + prd->dwOffset;

    lpVertices = (LPD3DTLVERTEX)((LPBYTE)((LPDDRAWI_DDRAWSURFACE_INT)prd->lpTLBuf)->lpLcl->lpGbl->fpVidMem
				 + prd->dwTLOffset);

#define CANT_DO_ZVISIBLE

#ifdef CANT_DO_ZVISIBLE
    if (aContext->z_visible) {
	/*
	 * If you don't implement Z visibility testing, just do this.
	 */
	prd->dwStatus &= ~D3DSTATUS_ZNOTVISIBLE;
	prd->ddrval = DD_OK;
	return (DDHAL_DRIVER_HANDLED);
    }
#endif
                               
#ifdef CAN_DO_ZVISIBLE
    if (aContext->z_visible) {
	/*
	 * If you can implement Z visibility testing, do this and clear
	 * the D3DSTATUS_ZNOTVISIBLE flag in dwStatus if at least one pixel
	 * of the primitive being rendered would be visible.
	 */
	if (!(prd->dwStatus & D3DSTATUS_ZNOTVISIBLE)) {
	    /*
	     * No point in trying if some other primitive has already
	     * triggered the visibility=TRUE flag.
	     */
	    prd->ddrval = DD_OK;
	    return (DDHAL_DRIVER_HANDLED);
	}
	/*
	 * You will need to turn off frame-buffer writes and z-buffer writes
	 * and be able to test if any pixel from the primitive would pass
	 * the z comparison function.  If so, then clear the bit.
	 */
	if (CanSeePrimitive(lpIns, prim))
	    prd->dwStatus &= ~D3DSTATUS_ZNOTVISIBLE;
	prd->ddrval = DD_OK;
	return (DDHAL_DRIVER_HANDLED);
    }
#endif 

    /*
     * Parse the instructions.
     */
    switch (lpIns->bOpcode) {
    /*
     * points and spans are different to lines and triangles.
     * They both use TL vertives in a contiguous batch.
     * and hence there can be multiple batches.
     * This is not the case for lines or triangles.
     */
    case D3DOP_POINT: {
            LPD3DPOINT point = (LPD3DPOINT)prim;
	    DPF( "in myRenderPrimitive, D3DOP_POINT instruction count = %d",
		 lpIns->wCount);
	    for (i = 0; i < lpIns->wCount; i++) {
		DPF( "in myRenderPrimitive, point count = %d", point->wCount);
		point++;	    
	    }
	}
	break;
    case D3DOP_SPAN: {
            LPD3DSPAN span = (LPD3DSPAN)prim; /* XXX check with doug */
	    DPF( "in myRenderPrimitive, D3DOP_SPAN instruction count = %d",
		 lpIns->wCount);
	    for (i = 0; i < lpIns->wCount; i++) {
		DPF( "in myRenderPrimitive, span count = %d", span->wCount);
		span++;	    
	    }
	}
	break;
    case D3DOP_LINE:
	DPF( "in myRenderPrimitive, D3DOP_LINE instruction count = %d",
	     lpIns->wCount);
	break;
    case D3DOP_TRIANGLE:
    	{
	    int i;
	    D3DTRIANGLE *triPtr = (D3DTRIANGLE *)prim;

	    DPF( "in myRenderPrimitive, D3DOP_TRIANGLE instruction count = %d",
		    lpIns->wCount);

	    for( i = 0; i < lpIns->wCount; i++ )
	    {
		DPF( "Tri: v1 = %d, v2 = %d, v3 = %d\n",
			    triPtr->v1, triPtr->v2, triPtr->v3 );
		triPtr++;
	    }
	}
	break;
    default:
	DPF( "in myRenderPrimitive, Unhandled instruction opcode = %d",
	     lpIns->bOpcode);
    }
    
    prd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myRenderPrimitive */


/*
 * myTextureCreate
 */
DWORD __stdcall myTextureCreate(LPD3DHAL_TEXTURECREATEDATA ptcd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a texture is to be created from a
     * DirectDrawSurface.
     * We must pass back the created handle.
     */
    //static D3DTEXTUREHANDLE nexthTex = 1;       // noddy
    //ptcd->dwHandle = nexthTex++;

    D3DSAMP_Context * aContext;
    D3DSAMP_Texture * lpTexture;
    HDDRVITEM hTexture;

    // Get context
    aContext = (D3DSAMP_Context *)DDrvItemArrayGetPtr (hContexts, ptcd->dwhContext);
    if (! aContext)
    {
        DPF( "in myTextureCreate, bad context =0x08lx", ptcd->dwhContext);
        ptcd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Create texture
    if (! DDrvItemArrayAlloc (hTextures, &hTexture))
    {
        ptcd->ddrval = DDERR_OUTOFMEMORY;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Get Pointer to texture
    lpTexture = (D3DSAMP_Texture *)DDrvItemArrayGetPtr (hTextures, hTexture);
    if (! lpTexture)
    {
        ptcd->ddrval = DDERR_GENERIC;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Success, fill in texture info
    lpTexture->lpDDS    = ptcd->lpDDS;
    lpTexture->hContext = ptcd->dwhContext;

    // return handle to texture
    ptcd->dwHandle  = hTexture;
    ptcd->ddrval    = DD_OK;

    DPF( "in TextureCreate, ptcd->dwhContext =%08lx, ptcd->dwHandle =%08lx",
                        ptcd->dwhContext, ptcd->dwHandle);

    ptcd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myTextureCreate */


/*
 * myTextureDestroy
 */
DWORD __stdcall myTextureDestroy(LPD3DHAL_TEXTUREDESTROYDATA ptcd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a texture is to be destroyed.
     */
    D3DSAMP_Context * aContext;

    // Get context
    aContext = (D3DSAMP_Context *)DDrvItemArrayGetPtr (hContexts, ptcd->dwhContext);
    if (! aContext)
    {
        DPF( "in myTextureDestroy, bad context =0x08lx", ptcd->dwhContext);
        ptcd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Delete Texture
    DDrvItemArrayFree (hTextures, (HDDRVITEM)ptcd->dwHandle);    
    
    DPF( "in TextureDestroy, ptcd->dwhContext =%08lx, ptcd->dwHandle =%08lx",
                             ptcd->dwhContext, ptcd->dwHandle);

    ptcd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myTextureDestroy */


/*
 * myTextureSwap
 */
DWORD __stdcall myTextureSwap(LPD3DHAL_TEXTURESWAPDATA ptsd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when two texture handles are to be swapped.
     * I.e. the data refered to by the two handles is to be swapped.
     */
    D3DSAMP_Context * aContext;
    D3DSAMP_Texture * lpTex1;
    D3DSAMP_Texture * lpTex2;
    D3DSAMP_Texture   hTmpTex;
    
    // Get context
    aContext = (D3DSAMP_Context *)DDrvItemArrayGetPtr (hContexts, ptsd->dwhContext);
    if (! aContext)
    {
        DPF( "in myTextureSwap, bad context =0x08lx", ptsd->dwhContext);
        ptsd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Get Texture 1
    lpTex1 = (D3DSAMP_Texture *)DDrvItemArrayGetPtr (hTextures, ptsd->dwHandle1);
    if (! lpTex1)
    {
        DPF( "in myTextureSwap, bad texture =0x08lx", ptsd->dwHandle1);
        ptsd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Get Texture 2
    lpTex2 = (D3DSAMP_Texture *)DDrvItemArrayGetPtr (hTextures, ptsd->dwHandle2);
    if (! lpTex2)
    {
        DPF( "in myTextureSwap, bad texture =0x08lx", ptsd->dwHandle2);
        ptsd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Swap textures
    hTmpTex = *lpTex1;
    *lpTex1 = *lpTex2;
    *lpTex2 = hTmpTex;

    DPF( "in TextureSwap, ptsd->dwhContext =%08lx,", ptsd->dwhContext);
    DPF( "                ptsd->dwHandle1 = %d, ptsd->dwHandle2 = %d",
	                      ptsd->dwHandle1, ptsd->dwHandle2);

    ptsd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myTextureTextureSwap */

/*
 * myTextureGetSurf
 */
DWORD __stdcall myTextureGetSurf(LPD3DHAL_TEXTUREGETSURFDATA ptgd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when the d3d needs to obtain the surface
     * refered to by a handle.
     */
    D3DSAMP_Context * aContext;
    D3DSAMP_Texture * lpTexture;

    // Get context
    aContext = (D3DSAMP_Context *)DDrvItemArrayGetPtr (hContexts, ptgd->dwhContext);
    if (! aContext)
    {
        DPF( "in myTextureGetSurf, bad context =0x08lx", ptgd->dwhContext);
        ptgd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    // Get Texture
    lpTexture = (D3DSAMP_Texture *)DDrvItemArrayGetPtr (hTextures, ptgd->dwHandle);
    if (! lpTexture)
    {
        // Error
        DPF( "in myTextureGetSurf, bad texture handle =0x08lx", ptgd->dwHandle);
        ptgd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    // Return pointer to texture surface
    ptgd->lpDDS = lpTexture->lpDDS;
    
    DPF( "in TextureGetSurf, ptgd->dwhContext =%08lx,", ptgd->dwhContext);
    DPF( "                   ptgd->dwHandle = %d", ptgd->dwHandle);

    ptgd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myTextureGetSurf */


/*
 * myMatrixCreate
 */
DWORD __stdcall myMatrixCreate(LPD3DHAL_MATRIXCREATEDATA pmcd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a matrix is to be created.
     * We must pass back the created handle.
     */
    static D3DMATRIXHANDLE nexthMat = 1;        // noddy

    if (myContextMap[pmcd->dwhContext] == 0) {
        DPF( "in myMatrixCreate, bad context =0x08lx", pmcd->dwhContext);
        pmcd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    pmcd->dwHandle = nexthMat++;
    DPF( "in MatrixCreate, pmcd->dwhContext =%08lx, pmcd->dwHandle =%08lx",
                        pmcd->dwhContext, pmcd->dwHandle);

    pmcd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myMatrixCreate */

/*
 * myMatrixDestroy
 */
DWORD __stdcall myMatrixDestroy(LPD3DHAL_MATRIXDESTROYDATA pmdd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a matrix is to be destroyed.
     */
    if (myContextMap[pmdd->dwhContext] == 0) {
        DPF( "in myMatrixDestroy, bad context =0x08lx", pmdd->dwhContext);
        pmdd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in MatrixDestroy, pmdd->dwhContext =%08lx, pmdd->dwHandle =%08lx",
                        pmdd->dwhContext, pmdd->dwHandle);

    pmdd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myMatrixDestroy */

/*
 * myMatrixSetData
 */
DWORD __stdcall myMatrixSetData(LPD3DHAL_MATRIXDESTROYDATA pmsd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when matrix data is to be changed.
     */
    if (myContextMap[pmsd->dwhContext] == 0) {
        DPF( "in myMatrixSetData, bad context =0x08lx", pmsd->dwhContext);
        pmsd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in MatrixSetData, pmsd->dwhContext =%08lx, pmsd->dwHandle =%08lx",
                        pmsd->dwhContext, pmsd->dwHandle);

    pmsd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myMatrixSetData */

/*
 * myMatrixGetData
 */
DWORD __stdcall myMatrixGetData(LPD3DHAL_MATRIXDESTROYDATA pmsd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when matrix data is to be retrieved.
     */
    if (myContextMap[pmsd->dwhContext] == 0) {
        DPF( "in myMatrixGetData, bad context =0x08lx", pmsd->dwhContext);
        pmsd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in MatrixGetData, pmsd->dwhContext =%08lx, pmsd->dwHandle =%08lx",
                        pmsd->dwhContext, pmsd->dwHandle);

    pmsd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myMatrixGetData */

/*
 * mySetViewportData
 */
DWORD __stdcall mySetViewportData(LPD3DHAL_SETVIEWPORTDATADATA psvd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when matrix data is to be changed.
     */
    if (myContextMap[psvd->dwhContext] == 0) {
        DPF( "in mySetViewportData, bad context =0x08lx", psvd->dwhContext);
        psvd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in SetViewportData, psvd->dwhContext =%08lx, psvd->dwViewportID =%08lx",
                        psvd->dwhContext, psvd->dwViewportID);

    psvd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* mySetViewportData */

/*
 * myMaterialCreate
 */
DWORD __stdcall myMaterialCreate(LPD3DHAL_MATERIALCREATEDATA pmcd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a material is to be created.
     * We must pass back the created handle.
     */
    static D3DMATERIALHANDLE nexthMat = 1;      // noddy

    if (myContextMap[pmcd->dwhContext] == 0) {
        DPF( "in myMaterialCreate, bad context =0x08lx", pmcd->dwhContext);
        pmcd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    pmcd->dwHandle = nexthMat++;
    DPF( "in MaterialCreate, pmcd->dwhContext =%08lx, pmcd->dwHandle =%08lx",
                        pmcd->dwhContext, pmcd->dwHandle);

    pmcd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myMaterialCreate */

/*
 * myMaterialDestroy
 */
DWORD __stdcall myMaterialDestroy(LPD3DHAL_MATERIALDESTROYDATA pmdd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when a material is to be destroyed.
     */
    if (myContextMap[pmdd->dwhContext] == 0) {
        DPF( "in myMaterialDestroy, bad context =0x08lx", pmdd->dwhContext);
        pmdd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in MaterialDestroy, pmdd->dwhContext =%08lx, pmdd->dwHandle =%08lx",
                        pmdd->dwhContext, pmdd->dwHandle);

    pmdd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myMaterialDestroy */

/*
 * myMaterialSetData
 */
DWORD __stdcall myMaterialSetData(LPD3DHAL_MATERIALSETDATADATA pmsd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when material data is to be changed.
     */
    if (myContextMap[pmsd->dwhContext] == 0) {
        DPF( "in myMaterialSetData, bad context =0x08lx", pmsd->dwhContext);
        pmsd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in MaterialSetData, pmsd->dwhContext =%08lx, pmsd->dwHandle =%08lx",
                        pmsd->dwhContext, pmsd->dwHandle);

    pmsd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myMaterialSetData */

/*
 * myMaterialGetData
 */
DWORD __stdcall myMaterialGetData(LPD3DHAL_MATERIALGETDATADATA pmgd)
{
    /*
     * NOTES:
     *
     * This callback is invoked when d3d needs to get the material data
     * refered to by a handle.
     */
    if (myContextMap[pmgd->dwhContext] == 0) {
        DPF( "in myMaterialGetData, bad context =0x08lx", pmgd->dwhContext);
        pmgd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in MaterialGetData, pmgd->dwhContext =%08lx, pmgd->dwHandle =%08lx",
                        pmgd->dwhContext, pmgd->dwHandle);

    pmgd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myMaterialGetData */

/*
 * myGetState
 */
DWORD __stdcall myGetState(LPD3DHAL_GETSTATEDATA pgsd)
{
    /*
     * NOTES:
     *
     * This callback is called when Direct3D requires information about
     * the state of a particular stage in the pipeline. If you only handle
     * rasterisation then you only need to respond to D3DHALSTATE_GET_RENDER
     * calls.
     * The state wanted is in pgsd->ddState.drstRenderStateType.
     * Return the answer in pgsd->ddState.dwArg[0].
     */
    if (myContextMap[pgsd->dwhContext] == 0) {
        DPF( "in myGetState, bad context =0x08lx", pgsd->dwhContext);
        pgsd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    
    DPF( "in GetState, pgsd->dwhContext =%08lx, pgsd->dwWhich =%08lx",
                        pgsd->dwhContext, pgsd->dwWhich);

    if (pgsd->dwWhich != D3DHALSTATE_GET_RENDER) {
	// You must be able to do transform/lighting
    }

    pgsd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

} /* myGetState */

/* --------------------------------------------------------------
 * Direct3D HAL Table.
 * Should be filled in to indicate which HAL calls we support.
 * NO EMULATION HERE.
 *
 * Calls take the form of:
 *      retcode = HalCall(HalCallData* lpData);
 */

static D3DHAL_CALLBACKS myD3DHALCallbacks = {
    sizeof(D3DHAL_CALLBACKS),

    // Device context
    myContextCreate,            // Required.
    myContextDestroy,           // Required.
    myContextDestroyAll,	// Required.

    // Scene capture
    NULL,			// Optional.
    
    // Execution
//@@BEGIN_MSINTERNAL
#ifdef IMPLEMENT_EXECUTE
    myExecute,
#else
//@@END_MSINTERNAL
    NULL,			// Optional.  Don't implement if just rasterization.
//@@BEGIN_MSINTERNAL
#endif
//@@END_MSINTERNAL
    NULL,
    myRenderState,		// Required if no Execute
    myRenderPrimitive,		// Required if no Execute
    0L,				// Reserved, must be zero
    
    // Textures
    myTextureCreate,            // If any of these calls are supported,
    myTextureDestroy,           // they must all be.
    myTextureSwap,              // ditto - but can always fail.
    myTextureGetSurf,           // ditto - but can always fail.
    
    // Transform - must be supported if lighting is supported.
    NULL, //myMatrixCreate,             // If any of these calls are supported,
    NULL, //myMatrixDestroy,            // they must all be.
    NULL, //myMatrixSetData,            // ditto
    NULL, //myMatrixGetData,            // ditto
    NULL, //mySetViewportData,          // ditto
    
    // Lighting
    NULL,                       // If any of these calls are supported,
    NULL, //myMaterialCreate,           // they must all be.
    NULL, //myMaterialDestroy,          // ditto
    NULL, //myMaterialSetData,          // ditto
    NULL, //myMaterialGetData,          // ditto

    // Pipeline state
    myGetState,			// Required if implementing Execute.

    0L,				// Reserved, must be zero
    0L,				// Reserved, must be zero
    0L,				// Reserved, must be zero
    0L,				// Reserved, must be zero
    0L,				// Reserved, must be zero
    0L,				// Reserved, must be zero
    0L,				// Reserved, must be zero
    0L,				// Reserved, must be zero
    0L,				// Reserved, must be zero
    0L,				// Reserved, must be zero
};

#define nullPrimCaps {                   		      	\
    sizeof(D3DPRIMCAPS), 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0, 0	\
}

#define triCaps {                                       \
    sizeof(D3DPRIMCAPS),				\
    0,                          /* miscCaps */          \
    D3DPRASTERCAPS_DITHER |     /* rasterCaps */        \
        D3DPRASTERCAPS_SUBPIXEL |                       \
        D3DPRASTERCAPS_ZTEST,                           \
    D3DPCMPCAPS_LESSEQUAL,      /* zCmpCaps */          \
    0,                          /* sourceBlendCaps */   \
    0,                          /* destBlendCaps */     \
    0,                          /* alphaBlendCaps */    \
    D3DPSHADECAPS_COLORFLATRGB|	/* shadeCaps */		\
	D3DPSHADECAPS_COLORGOURAUDRGB |			\
	D3DPSHADECAPS_SPECULARFLATRGB |			\
	D3DPSHADECAPS_SPECULARGOURAUDRGB |		\
	D3DPSHADECAPS_ALPHAFLATBLEND	|		\
	D3DPSHADECAPS_ALPHAGOURAUDBLEND |		\
	D3DPSHADECAPS_FOGFLAT |				\
	D3DPSHADECAPS_FOGGOURAUD,			\
    D3DPTEXTURECAPS_PERSPECTIVE |/* textureCaps */      \
        D3DPTEXTURECAPS_POW2 |                          \
        D3DPTEXTURECAPS_TRANSPARENCY,                   \
    D3DPTFILTERCAPS_NEAREST |   /* textureFilterCaps */ \
        D3DPTFILTERCAPS_LINEAR,                         \
    0,                          /* textureBlendCaps */  \
    D3DPTADDRESSCAPS_WRAP,	/* textureAddressCaps */\
    0,                          /* stippleWidth */      \
    0                           /* stippleHeight */     \
}                                                       \

static D3DDEVICEDESC myCaps = {
    sizeof(D3DDEVICEDESC),      /* dwSize */
    D3DDD_COLORMODEL |            /* dwFlags */
        D3DDD_DEVCAPS |
        D3DDD_TRICAPS |
	D3DDD_DEVICERENDERBITDEPTH |
	D3DDD_DEVICEZBUFFERBITDEPTH,
    D3DCOLOR_RGB,               /* dcmColorModel */
    D3DDEVCAPS_FLOATTLVERTEX | /* devCaps */
        D3DDEVCAPS_SORTEXACT,
    { sizeof(D3DTRANSFORMCAPS), 0 },		/* dtcTransformCaps */
    FALSE,                      /* bClipping */
    { sizeof(D3DLIGHTINGCAPS), 0 },  /* dlcLightingCaps */
    nullPrimCaps,               /* lineCaps */
    triCaps,                    /* triCaps */
    DDBD_8 | DDBD_16 | DDBD_32,	/* dwDeviceRenderBitDepth */
    DDBD_16,			/* dwDeviceZBufferBitDepth */
    0,			        /* dwMaxBufferSize */
    0      			/* dwMaxVertexCount */
};

static DDSURFACEDESC myTextureFormats = {
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
      0,				/* FOURCC code */
      32,                               /* ddpfPixelFormat.dwRGBBitCount */
      0xff0000,
      0xff00,
      0xff,
      0
    },
    DDSCAPS_TEXTURE                     /* ddscaps.dwCaps */
};

static D3DHAL_GLOBALDRIVERDATA myGlobal;
    
/* --------------------------------------------------------------
 * Functions used to instantiate the 3D portion of the DirectDraw HAL
 *
 * Will be called (for example) from invokeCreateDriver() from ddsamp.
 */
BOOL __stdcall D3DHALCreateDriver(LPD3DHAL_GLOBALDRIVERDATA* lplpGlobal,
                                  LPD3DHAL_CALLBACKS* lplpHALCallbacks)
{
    /*
     * You need to alloc space for vertex transform destination.
     */
    
    /*
     * Here we fill in the supplied structures.
     */
    memset(&myGlobal, 0, sizeof(D3DHAL_GLOBALDRIVERDATA));
    myGlobal.dwSize = sizeof(D3DHAL_GLOBALDRIVERDATA);
    myGlobal.hwCaps = myCaps;
    myGlobal.dwNumVertices = 0;
    myGlobal.dwNumClipVertices = 0;
    myGlobal.dwNumTextureFormats = 1;
    myGlobal.lpTextureFormats = &myTextureFormats;
    
    /*
     * You will need to initialise any context table here.
     */
    
    
    /*
     * Return the HAL table.
     */
    *lplpGlobal = &myGlobal;
    *lplpHALCallbacks = &myD3DHALCallbacks;
    
    return (TRUE);
}



/*
 * InitItemArrays
 */
BOOL InitItemArrays (void)
{
#ifdef DEBUG
    if (hContexts)
    {
        DPF ("InitItemArrays, hContexts should be NULL!!!");
        return FALSE;
    }

    if (hTextures)
    {
        DPF ("InitItemArrays, hTextures should be NULL!!!");
        return FALSE;
    }
#endif

    // Initialize Memory system
    if (! DDrvItemArrayInit ())
        return FALSE;

    // Create D3D Context array
    if (! DDrvItemArrayCreate(CONTEXTS_DELTA, sizeof (D3DSAMP_Context), CONTEXTS_DELTA, &hContexts))
    {
        DDrvItemArrayFini ();
        return FALSE;
    }

    // Create Texture map array
    if (! DDrvItemArrayCreate(TEXTURES_DELTA, sizeof (D3DSAMP_Texture), TEXTURES_DELTA, &hTextures))
    {
        DDrvItemArrayFini ();
        return FALSE;
    }

} // End InitItemArrays



/*
 * DestroyItemArrays
 */
BOOL DestroyItemArrays (void)
{
    // Cleanup contexts
    if (hTextures)
    {
        DDrvItemArrayDestroy (hTextures);   
        hTextures = 0;
    }

    // Cleanup contexts
    if (hContexts)
    {
        DDrvItemArrayDestroy (hContexts);   
        hContexts = 0;
    }

    // Cleanup Memory
    return DDrvItemArrayFini ();
} // End DestroyItemArrays


/*
 * DllMain
 */
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    switch( dwReason ) 
    {   
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls( hModule );

        // Initialize Memory system
        if (! InitItemArrays ())
            return FALSE;
        break;

    case DLL_PROCESS_DETACH:
        // Destroy Memory system
        DestroyItemArrays ();
        break;
    }

    return TRUE;

} /* DllMain */
