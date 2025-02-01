
/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3dcb2.c
 *  Content:    Direct3D Sample HAL Driver
 *
 ***************************************************************************/

#include "d3ddrv.h"
extern HRESULT S3UpdateFlipStatus( FLATPTR fpVidMem, LPS3THKINFO sData );
extern	HRESULT __stdcall SetupState(S3_CONTEXT*,DWORD,DWORD); 
extern HRESULT __stdcall SetupPrimitive(S3_CONTEXT *ctxt); 
extern DWORD __stdcall SetRenderTarget32(LPD3DHAL_SETRENDERTARGETDATA);
extern DWORD __stdcall DrawOnePrimitive32(LPD3DHAL_DRAWONEPRIMITIVEDATA);
extern DWORD __stdcall DrawOneIndexedPrimitive32(LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA);
extern DWORD __stdcall DrawPrimitives32(LPD3DHAL_DRAWPRIMITIVESDATA);


DWORD __stdcall SetRenderTarget32(LPD3DHAL_SETRENDERTARGETDATA lpsrtd)
{
    S3_CONTEXT *ctxt;
    //DPF("SetRenderTarget called");
    ctxt = (LPS3_CONTEXT) lpsrtd->dwhContext;
    if (! ctxt) {
        DPF( "in SetRenderTarget32, bad context handle %08lx", lpsrtd->dwhContext);
        lpsrtd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

// cannot render into system memory
    ctxt->lpLcl = DDS_LCL(lpsrtd->lpDDS);
    if (! ctxt->lpLcl) {
        lpsrtd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }

    if ( DDSurf_Get_dwCaps(DDS_LCL(lpsrtd->lpDDS)) & DDSCAPS_SYSTEMMEMORY ) {
        DPF( "in SetRenderTarget32, surface in SYSTEM MEMORY, failing" );
        lpsrtd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }

    if(lpsrtd->lpDDSZ)
    {
    	if (DDSurf_Get_dwCaps(DDS_LCL(lpsrtd->lpDDSZ)) & DDSCAPS_SYSTEMMEMORY) {
        DPF( "in SetRenderTarget32, Z surface in SYSTEM MEMORY, failing" );
        lpsrtd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
	 }
        ctxt->lpLclZ = DDS_LCL(lpsrtd->lpDDSZ);
    }
    else
	 ctxt->lpLclZ = NULL;	//detatched
	 	 
    if( ctxt->lpLclZ ) {
        ctxt->bZEnabled = TRUE;
        ctxt->bZWriteEnabled = TRUE;
    	 ctxt->RStyle.bZBuffer=TRUE;
    }
    else
    {
        ctxt->bZEnabled = FALSE;
        ctxt->bZWriteEnabled = FALSE;
    	 ctxt->RStyle.bZBuffer=FALSE;
    }
    DPF( "in SetRenderTarget32, dwhContext=%08lx lpLcl =%08lx, lpLclZ =%08lx",
    	lpsrtd->dwhContext,ctxt->lpLcl,ctxt->lpLclZ);
    ctxt->bChanged=TRUE;
    lpsrtd->ddrval = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

DWORD __stdcall doDrawOnePrimitive32(LPD3DHAL_DRAWONEPRIMITIVEDATA lpdopd)
{
	DWORD	dwNumPrimitives;
	S3_CONTEXT *ctxt = (LPS3_CONTEXT) lpdopd->dwhContext;
	LPD3DTLVERTEX p0,p1,p2,pV=(LPD3DTLVERTEX)lpdopd->lpvVertices;
	DWORD	i;
	LPRENDERTRIANGLE	pTriangle;
	if ((lpdopd->ddrval=SetupPrimitive(ctxt)) != DD_OK)	return	(DDHAL_DRIVER_HANDLED);
	if (ctxt->FillMode == D3DFILL_WIREFRAME)
	{
		pTriangle=GenericOneWireFrame; 
	}
	else
       if((ctxt->RStyle.bTextured) && ( ctxt->fTextureWidth >= ctxt->sData->D3DGlobals.__UVRANGE ))
	{
		pTriangle=ChopInUVSpace;
	}
	else
	{
		pTriangle=pRenderTriangle[ctxt->dwRCode];
	}
	//DPF("Entering doDrawOnePrimitive32");
	switch (lpdopd->PrimitiveType)
	{
		case D3DPT_POINTLIST:
			//DPF("D3DPT_POINTLIST");
            		dwNumPrimitives = lpdopd->dwNumVertices;
			for (i=dwNumPrimitives;i>0; i--)
			{ 
				p0=pV++;
				GenericPoint(ctxt,p0,p0);	
			}
			break;
		case D3DPT_LINELIST:
			//DPF("D3DPT_LINELIST");
            		dwNumPrimitives = lpdopd->dwNumVertices / 2;
			for (i=dwNumPrimitives;i>0; i--)
			{ 
				p0=pV++;
				p1=pV++;
				GenericLine(ctxt,p0,p1,p0);	//p0 as pFlat
			}
			break;
		case D3DPT_LINESTRIP:
			//DPF("D3DPT_LINESTRIP");
            		dwNumPrimitives = lpdopd->dwNumVertices - 1;
			p1=pV++;		//For initial p0
			for (i=dwNumPrimitives;i>0; i--)
			{ 
				p0=p1;
				p1=pV++;
				GenericLine(ctxt,p0,p1,p0);	//p0 as pFlat
			}
			break;
		case D3DPT_TRIANGLELIST:
            		dwNumPrimitives = lpdopd->dwNumVertices / 3;
			//DPF("D3DPT_TRIANGLELIST lpdopd->dwNumVertices=%d",lpdopd->dwNumVertices);
			for (i=dwNumPrimitives;i>0;i--)
			{ 
				p0=pV++;
				p1=pV++;
				p2=pV++;
				if (!Culling(p0,p1,p2))	pTriangle(ctxt,p0,p1,p2);
			}
			break;
		case D3DPT_TRIANGLESTRIP:
            		dwNumPrimitives = lpdopd->dwNumVertices - 2;
			//DPF("D3DPT_TRIANGLESTRIP dwNumPrimitives=%d",dwNumPrimitives);
			for (i=dwNumPrimitives;i>0;i--)
			{ 
				p0=pV++;
				p1=pV++;
				p2=pV++;
			//DPF("pV=%08lx at (%d,%d)",(ULONG)pV,(int)pV->sx,(int)pV->sy);
				if (!Culling(p0,p1,p2))	pTriangle(ctxt,p0,p1,p2);
				i--;
				if (i > 0)
				{
					p0=p2;
					p1=p1;
					p2=pV--;
					if (!Culling(p1,p2,p0))	pTriangle(ctxt,p1,p2,p0);
				}
				else
					break;
			}
			break;
		case D3DPT_TRIANGLEFAN:
            		dwNumPrimitives = lpdopd->dwNumVertices - 2;
			//DPF("D3DPT_TRIANGLEFAN dwNumPrimitives=%d",dwNumPrimitives);
			p0=pV++;
			p2=pV++;	//For initial p1
			for (i=dwNumPrimitives;i>0;i--)
			{ 
			//DPF("pV=%08lx at (%d,%d)",(ULONG)pV,(int)pV->sx,(int)pV->sy);
				p1=p2;
				p2=pV++;
				if (!Culling(p1,p2,p0))	pTriangle(ctxt,p1,p2,p0);
			}
			break;
		default:
			DPF( "Unknown or unsupported primitive type requested of doDrawOnePrimitive32" );
    			return DDHAL_DRIVER_NOTHANDLED;
			
	}
	//DPF("Leaving doDrawOnePrimitive32");
    lpdopd->ddrval = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}	//doDrawOnePrimitive32

DWORD __stdcall DrawOnePrimitive32(LPD3DHAL_DRAWONEPRIMITIVEDATA lpdopd)
{
	S3_CONTEXT *ctxt = (LPS3_CONTEXT) lpdopd->dwhContext;
    	DWORD	dwRet;
    	short OldCW, TruncateCW;
    //DPF("DrawOnePrimitive32 %08lx %08lx %08lx %08lx %08lx %08lx",
    //lpdopd->dwhContext,lpdopd->dwFlags,lpdopd->PrimitiveType,lpdopd->VertexType,lpdopd->dwNumVertices,lpdopd->lpvVertices);	
	if (!ctxt){
        	DPF( "in DrawPrimitives32, dwhContext=NULL");
        	lpdopd->ddrval = D3DHAL_CONTEXT_BAD;
        	return (DDHAL_DRIVER_HANDLED);
	}
    	if( (lpdopd->ddrval = S3UpdateFlipStatus( ctxt->lpLcl->lpGbl->fpVidMem, ctxt->sData )) != DD_OK )
    	{
        	return DDHAL_DRIVER_HANDLED;
    	}
    	_asm fstcw   OldCW
        TruncateCW = OldCW | 0x0e00;    //PC:Double Precision RC:truncate
    	_asm fldcw   TruncateCW
	dwRet=doDrawOnePrimitive32(lpdopd);		
    	_asm fldcw OldCW
	return	dwRet;
}

DWORD __stdcall DrawOneIndexedPrimitive32(LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA lpdoipd)
{
    //DPF("DrawOneIndexedPrimitive called");
	DWORD	dwNumPrimitives;
	S3_CONTEXT *ctxt = (LPS3_CONTEXT)lpdoipd->dwhContext;
	LPD3DTLVERTEX p0,p1,p2,pV=(LPD3DTLVERTEX)lpdoipd->lpvVertices;
	DWORD	i,dwIndex=0;
	LPWORD	lpwIndices=lpdoipd->lpwIndices;
	LPRENDERTRIANGLE	pTriangle;
    	short OldCW, TruncateCW;
	if (!ctxt){
        	DPF( "in DrawPrimitives32, dwhContext=NULL");
        	lpdoipd->ddrval = D3DHAL_CONTEXT_BAD;
        	return (DDHAL_DRIVER_HANDLED);
	}
    	if( (lpdoipd->ddrval = S3UpdateFlipStatus( ctxt->lpLcl->lpGbl->fpVidMem, ctxt->sData )) != DD_OK )
    	{
        	return DDHAL_DRIVER_HANDLED;
    	}
	if ((lpdoipd->ddrval=SetupPrimitive(ctxt)) != DD_OK)	return	(DDHAL_DRIVER_HANDLED);
	if (ctxt->FillMode == D3DFILL_WIREFRAME)
	{
		pTriangle=GenericOneWireFrame; 
	}
	else
       if((ctxt->RStyle.bTextured) && ( ctxt->fTextureWidth >= ctxt->sData->D3DGlobals.__UVRANGE ))
	{
		pTriangle=ChopInUVSpace;
	}
	else
	{
		pTriangle=pRenderTriangle[ctxt->dwRCode];
	}
    	_asm fstcw   OldCW
        TruncateCW = OldCW | 0x0e00;    //PC:Double Precision RC:truncate
    	_asm fldcw   TruncateCW
	switch (lpdoipd->PrimitiveType)
	{
		case D3DPT_POINTLIST:
			//DPF("D3DPT_POINTLIST");
            		dwNumPrimitives = lpdoipd->dwNumIndices;
			for (i=dwNumPrimitives;i>0; i--)
			{ 
				p0=pV+lpwIndices[dwIndex++];
				GenericPoint(ctxt,p0,p0);	
			}
			break;
		case D3DPT_LINELIST:
			//DPF("D3DPT_LINELIST");
            		dwNumPrimitives = lpdoipd->dwNumIndices / 2;
			for (i=dwNumPrimitives;i>0; i--)
			{ 
				p0=pV+lpwIndices[dwIndex++];
				p1=pV+lpwIndices[dwIndex++];
				GenericLine(ctxt,p0,p1,p0);	//p0 as pFlat
			}
			break;
		case D3DPT_LINESTRIP:
			//DPF("D3DPT_LINESTRIP");
            		dwNumPrimitives = lpdoipd->dwNumIndices - 1;
			p1=pV+lpwIndices[dwIndex++];	//For initial p0
			for (i=dwNumPrimitives;i>0; i--)
			{ 
				p0=p1;
				p1=pV+lpwIndices[dwIndex++];
				GenericLine(ctxt,p0,p1,p0);	//p0 as pFlat
			}
			break;
		case D3DPT_TRIANGLELIST:
			//DPF("D3DPT_TRIANGLELIST");
            		dwNumPrimitives = lpdoipd->dwNumIndices / 3;
			for (i=dwNumPrimitives;i>0; i--)
			{ 
				p0=pV+lpwIndices[dwIndex++];
				p1=pV+lpwIndices[dwIndex++];
				p2=pV+lpwIndices[dwIndex++];
				if (!Culling(p0,p1,p2))	pTriangle(ctxt,p0,p1,p2);
			}
			break;
		case D3DPT_TRIANGLESTRIP:
			//DPF("D3DPT_TRIANGLESTRIP");
            		dwNumPrimitives = lpdoipd->dwNumIndices - 2;
			for (i=dwNumPrimitives;i>0; i--)
			{ 
				p0=pV+lpwIndices[dwIndex++];
				p1=pV+lpwIndices[dwIndex++];
				p2=pV+lpwIndices[dwIndex++];
				if (!Culling(p0,p1,p2))	pTriangle(ctxt,p0,p1,p2);
				i--;
				if (i > 0)
				{
					p0=p2;
					p1=p1;
					p2=pV+lpwIndices[dwIndex--];
					if (!Culling(p1,p2,p0))	pTriangle(ctxt,p1,p2,p0);
				}
				else
					break;
			}
			break;
		case D3DPT_TRIANGLEFAN:
			//DPF("D3DPT_TRIANGLEFAN");
            		dwNumPrimitives = lpdoipd->dwNumIndices - 2;
			p0=pV+lpwIndices[dwIndex++];
			p2=pV+lpwIndices[dwIndex++];	//For initial p1
			for (i=dwNumPrimitives;i>0;i--)
			{ 
				p1=p2;
				p2=pV+lpwIndices[dwIndex++];
				if (!Culling(p1,p2,p0))	pTriangle(ctxt,p1,p2,p0);
			}
			break;
		default:
			DPF( "Unknown or unsupported primitive type requested of DrawIndexedPrimitive" );
    			return DDHAL_DRIVER_NOTHANDLED;
			
	}
    	_asm fldcw OldCW
    lpdoipd->ddrval = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

DWORD __stdcall DrawPrimitives32(LPD3DHAL_DRAWPRIMITIVESDATA lpdpd)
{
	S3_CONTEXT *ctxt = (LPS3_CONTEXT)lpdpd->dwhContext;
    	LPBYTE              		lpData=(LPBYTE)lpdpd->lpvData;
	LPD3DHAL_DRAWPRIMCOUNTS	lpDrawPrimitiveCounts;	
	LPDWORD			lpStateChange;
	DWORD				j,StateType,StateValue;
	D3DHAL_DRAWONEPRIMITIVEDATA	DrawOnePrimitiveData;
    	short OldCW, TruncateCW;
	if (!ctxt){
        	DPF( "in DrawPrimitives32, dwhContext=NULL");
        	lpdpd->ddrval = D3DHAL_CONTEXT_BAD;
        	return (DDHAL_DRIVER_HANDLED);
	}
    	if( (lpdpd->ddrval = S3UpdateFlipStatus( ctxt->lpLcl->lpGbl->fpVidMem, ctxt->sData )) != DD_OK )
    	{
        	return DDHAL_DRIVER_HANDLED;
    	}
    	_asm fstcw   OldCW
        TruncateCW = OldCW | 0x0e00;    //PC:Double Precision RC:truncate
    	_asm fldcw   TruncateCW
	DrawOnePrimitiveData.dwFlags=lpdpd->dwFlags;
	DrawOnePrimitiveData.dwhContext=lpdpd->dwhContext;
	do{
		lpDrawPrimitiveCounts=(LPD3DHAL_DRAWPRIMCOUNTS)(lpData);
		lpData += sizeof(D3DHAL_DRAWPRIMCOUNTS);
		lpStateChange=(LPDWORD)lpData;
		for (j=lpDrawPrimitiveCounts->wNumStateChanges;j>0;j--)
		{
			StateType=*lpStateChange;
			lpStateChange++;
			StateValue=*lpStateChange;
			lpStateChange++;
#ifdef	DEBUG
			if ( !StateType){
			DPF("[%d]StateType=%08lx,StateValue=%08lx",j,StateType,StateValue);
				_asm int 3
				continue;
			}
#endif	//DEBUG
			if ((lpdpd->ddrval = SetupState(ctxt,StateType,StateValue)) != DD_OK)
    				return DDHAL_DRIVER_HANDLED;
		}
		lpData +=(lpDrawPrimitiveCounts->wNumStateChanges*sizeof(DWORD)*2)+31;
		lpData = (LPBYTE)((ULONG)lpData & (~31));
		//DPF("lpData=%08lx PrimitiveType=%d wNumVertices=%d",lpData,lpDrawPrimitiveCounts->wPrimitiveType,lpDrawPrimitiveCounts->wNumVertices);
		if (lpDrawPrimitiveCounts->wNumVertices){
			DrawOnePrimitiveData.PrimitiveType=lpDrawPrimitiveCounts->wPrimitiveType;
			DrawOnePrimitiveData.lpvVertices=(LPVOID)lpData;
			DrawOnePrimitiveData.VertexType=lpDrawPrimitiveCounts->wVertexType;
			DrawOnePrimitiveData.dwNumVertices=lpDrawPrimitiveCounts->wNumVertices;
			doDrawOnePrimitive32((LPD3DHAL_DRAWONEPRIMITIVEDATA)&DrawOnePrimitiveData); 
			if ((lpdpd->ddrval=DrawOnePrimitiveData.ddrval) != DD_OK)	return DDHAL_DRIVER_HANDLED;
			lpData+=(lpDrawPrimitiveCounts->wNumVertices*sizeof(D3DTLVERTEX));
		}
	}while(lpDrawPrimitiveCounts->wNumVertices);
    	_asm fldcw OldCW
    lpdpd->ddrval = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}
