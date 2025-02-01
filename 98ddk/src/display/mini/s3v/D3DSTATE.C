/*==========================================================================;
*
*  Copyright (C) 1996 Microsoft Corporation.  All Rights Reserved.
*
*  File:       d3dstate.c
*  Content:    Direct3D state management
*
***************************************************************************/

#include "precomp.h" 
#include "d3ddrv.h"

#define IS_OVERRIDE(type)       ((DWORD)(type) > D3DSTATE_OVERRIDE_BIAS)
#define GET_OVERRIDE(type)      ((DWORD)(type) - D3DSTATE_OVERRIDE_BIAS)

#define STATESET_MASK(set, state)       \
(set).bits[((state) - 1) >> DWORD_SHIFT]

#define STATESET_BIT(state)     (1 << (((state) - 1) & (DWORD_BITS - 1)))

#define STATESET_ISSET(set, state) \
STATESET_MASK(set, state) & STATESET_BIT(state)

#define STATESET_SET(set, state) \
STATESET_MASK(set, state) |= STATESET_BIT(state)

#define STATESET_CLEAR(set, state) \
STATESET_MASK(set, state) &= ~STATESET_BIT(state)

#define STATESET_INIT(set)      memset(&(set), 0, sizeof(set))

extern DWORD dwTextureCount;

HRESULT __stdcall SetupState(S3_CONTEXT *ctxt,DWORD StateType,DWORD StateValue) 
{
// The state to deal with comes in the renderStateType var.
	switch(StateType) {
	    case D3DRENDERSTATE_TEXTUREHANDLE:
	    {
// Get Texture  
		ctxt->bChanged = TRUE;	//fixed #1004	
		if( ctxt->dwTexture == StateValue )
			return  DD_OK;                 // no change  
			
		ctxt->dwTexture = StateValue;
		ctxt->RStyle.bTextured= (StateValue!=0);
		
		if(ctxt->dwTexture){
		  
		    LPS3_TEXTURE lpTexture=(LPS3_TEXTURE)ctxt->dwTexture;
		    if (! lpTexture) {
		     //   DPF( "in myRenderState, bad texture handle =0x08lx", dwTexture);
			//prd->ddrval = DDERR_INVALIDPARAMS;
			return (DDHAL_DRIVER_HANDLED);
		    }
		}   
	    }  //texture handle
		
		break;


	    case D3DRENDERSTATE_TEXTUREADDRESS:
		ctxt->TextureAddress = (D3DTEXTUREADDRESS) StateValue;
		ctxt->bChanged = TRUE;
		break;
	    case D3DRENDERSTATE_TEXTUREPERSPECTIVE:
// Turn our global to the value sent (0=off - 1=on)
		ctxt->bPerspective = StateValue;
		ctxt->RStyle.bPerspective= StateValue;
		ctxt->bChanged = TRUE;
		break;
	    case D3DRENDERSTATE_ZENABLE:
		ctxt->bZEnabled = (BOOL) StateValue;
		ctxt->RStyle.bZBuffer = StateValue;
		ctxt->bChanged = TRUE;
		break;

	    case D3DRENDERSTATE_SHADEMODE:
		ctxt->ShadeMode = (D3DSHADEMODE) StateValue;
		ctxt->RStyle.bGouraud= ((D3DSHADEMODE) StateValue != D3DSHADE_FLAT);
		ctxt->bChanged = TRUE;

		break;
	    case D3DRENDERSTATE_FILLMODE:
		ctxt->FillMode = (D3DFILLMODE) StateValue;
		ctxt->bChanged = TRUE;
		break;
//case D3DRENDERSTATE_LINEPATTERN:
//break;
//case D3DRENDERSTATE_ROP2:
//break;
//case D3DRENDERSTATE_PLANEMASK:
//break;
	    case D3DRENDERSTATE_ZWRITEENABLE:
		ctxt->bZWriteEnabled = StateValue;
		ctxt->bChanged = TRUE;
		break;
//case D3DRENDERSTATE_ALPHATESTENABLE:
//break;
//case D3DRENDERSTATE_LASTPIXEL:
//break;
	    case D3DRENDERSTATE_TEXTUREMAG:/* Virge has only one setting for both MAG and MIN*/
		ctxt->bChanged = TRUE;
		ctxt->TextureMode = (D3DTEXTUREFILTER) StateValue;
		break;
	    case D3DRENDERSTATE_TEXTUREMIN:       /* D3DTEXTUREFILTER */
		ctxt->bChanged = TRUE;
		ctxt->TextureMode = (D3DTEXTUREFILTER) StateValue;
		break;
	    case D3DRENDERSTATE_SRCBLEND:   /* D3DBLEND */
		ctxt->bChanged = TRUE;
		ctxt->SrcBlend = StateValue;
		break;
	    case D3DRENDERSTATE_DESTBLEND:  /* D3DBLEND */
		ctxt->bChanged = TRUE;
		if ( StateValue == D3DBLEND_ZERO )
		    ctxt->SrcBlend = D3DBLEND_ONE;
		if ( StateValue == D3DBLEND_INVSRCALPHA )
		    ctxt->SrcBlend = D3DBLEND_SRCALPHA;
		break;
	    case D3DRENDERSTATE_TEXTUREMAPBLEND:  /* D3DTEXTUREBLEND */
		ctxt->bChanged = TRUE;
		ctxt->BlendMode = StateValue;
		break;
	    case D3DRENDERSTATE_CULLMODE:         /* D3DCULL */
		ctxt->CullMode = StateValue;
		break;
	    case D3DRENDERSTATE_ZFUNC:            /* D3DCMPFUNC */
		ctxt->bChanged = TRUE;
		ctxt->ZCmpFunc = (D3DCMPFUNC) StateValue;
		break;
//case D3DRENDERSTATE_ALPHAREF:   /* D3DFIXED */
//break;
//case D3DRENDERSTATE_ALPHAFUNC:  /* D3DCMPFUNC */
//break;
//case D3DRENDERSTATE_DITHERENABLE:       /* TRUE to enable dithering */
//break;
	    case D3DRENDERSTATE_ALPHABLENDENABLE: 
		ctxt->bChanged = TRUE;
		  if (StateValue)
			ctxt->dwStatus |= S3ALPHABLENDENABLE;
		  else
			ctxt->dwStatus &= ~S3ALPHABLENDENABLE;
		  //DPF("ALPHA StateValue=%d",StateValue);
		  break;
			
	    case D3DRENDERSTATE_FOGENABLE:
		ctxt->bChanged = TRUE;
		  if (StateValue)
			ctxt->dwStatus |= S3FOGENABLE;
		  else
			ctxt->dwStatus &= ~S3FOGENABLE;
		break;
	    case D3DRENDERSTATE_FOGCOLOR:         /* D3DCOLOR */
		ctxt->rnd3DSetup.stp_FOG_CLR = StateValue;
		break;
	    case D3DRENDERSTATE_SPECULARENABLE:
		ctxt->bChanged = TRUE;
		  if (StateValue)
			ctxt->dwStatus |= S3SPECULARENABLE;
		  else
			ctxt->dwStatus &= ~S3SPECULARENABLE;
		break;
	    case D3DRENDERSTATE_ZVISIBLE:
		ctxt->bZVisible = StateValue;
		return  DD_OK;
	    case D3DRENDERSTATE_WRAPU:
		ctxt->bWrapU = StateValue;
		return  DD_OK;
	    case D3DRENDERSTATE_WRAPV:
		ctxt->bWrapV = StateValue;
		return  DD_OK;

	    default:
// not handled is a do not care
		return  DD_OK;
	}
     return DD_OK;
}
/*----------------------------------------------------------------------------
myStateRender

LPD3DHAL_RENDERSTATEDATA
- Points to the instruction in the execute buffer.

We get the count of states to set and then walk the execute buffer
handling each one at a time.
----------------------------------------------------------------------------*/
DWORD __stdcall myRenderState(LPD3DHAL_RENDERSTATEDATA prd) {
/*
* NOTES:
*
* This callback is invoked when a set of render states are changed.
*/
    LPBYTE              lpData;
    LPD3DSTATE          lpState;
    DWORD               i;
// Get Pointer to context
    S3_CONTEXT *ctxt=(LPS3_CONTEXT)prd->dwhContext;

    if ( ! ctxt ) {
	DPF( "in myRenderState, bad context =0x%08lx", prd->dwhContext);
	prd->ddrval = D3DHAL_CONTEXT_BAD;
	return (DDHAL_DRIVER_HANDLED);
    }

    #if 0
    DPF( "in myRenderState, prd->dwhContext =%08lx", prd->dwhContext);
    #endif

    lpData = (LPBYTE)(DDS_LCL(prd->lpExeBuf)->lpGbl->fpVidMem);

    for (i = 0, lpState = (LPD3DSTATE) (lpData + prd->dwOffset);
	i < prd->dwCount; i++, lpState++)
    {
	DWORD type = (DWORD) lpState->drstRenderStateType;

	#if 0
	DPF( "in myRenderState, state = %d", lpState->drstRenderStateType);
	#endif
	if (IS_OVERRIDE(type)) {
	    DWORD override = GET_OVERRIDE(type);
	    if (lpState->dwArg[0]) {
		DPF( "in myRenderState, setting override for state %d", override);
		STATESET_SET(ctxt->overrides, override);
	    }
	    else {
		DPF( "in myRenderState, clearing override for state %d", override);
		STATESET_CLEAR(ctxt->overrides, override);
	    }
	    continue;
	}

	if (STATESET_ISSET(ctxt->overrides, type)) {
	    DPF( "in myRenderState, state %d is overridden, ignoring", type);
	    continue;
	}
//DPF( "in myRenderState, setting state %d to value %d", type, lpState->dwArg[0]);
	if ((prd->ddrval=SetupState(ctxt,type,(DWORD)lpState->dwArg[0]))!=DD_OK)
	    return (DDHAL_DRIVER_HANDLED);

// default is changed
//        ctxt->bChanged = TRUE;
    }

    prd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

}                                                 /* myRenderState */


/*--------------------------------------------------------------------------
myGetState

LPD3DHAL_GETSTATEDATA
- Structure pointing to the state to get data from this driver for.
----------------------------------------------------------------------------*/
DWORD __stdcall myGetState(LPD3DHAL_GETSTATEDATA pgsd) {
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
    S3_CONTEXT *ctxt;

    ctxt = (LPS3_CONTEXT)pgsd->dwhContext;
    if ( ! ctxt ) {
	DPF( "in myGetState, bad context =0x%08lx", pgsd->dwhContext);
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

}                                                 /* myGetState */


