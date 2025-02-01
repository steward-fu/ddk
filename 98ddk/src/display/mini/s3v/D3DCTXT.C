/*==========================================================================;
*
*  Copyright (C) 1996 Microsoft Corporation.  All Rights Reserved.
*
*  File:       d3dctxt.c
*  Content:    Direct3D context management
*
***************************************************************************/

#include "precomp.h"
#include "d3ddrv.h"
#include "s3data.h"
#ifndef WINNT

#define S3DTK_KNOWN3DCHIPS      9
// check for P5, DMA possible on P5
#define DMA_Possible() \
{ \
        _asm { push eax } \
        _asm { push ebx } \
        _asm { push ecx } \
        _asm { push edx } \
        _asm { mov  eax, 1 } \
        _asm _emit 0x0f \
        _asm _emit 0xa2 \
        _asm { shr eax, 8 } \
        _asm { sub eax, 4 } \
        _asm { and eax, 0x00000001 } \
        _asm { mov dma_possible, eax } \
        _asm { pop edx } \
        _asm { pop ecx } \
        _asm { pop ebx } \
        _asm { pop eax } \
}

// problem was that on some k6's cpuid was not supported and on
// those system's dma would hang
#define CPUID_DMA_Possible()\
{                            \
            _asm { push    eax      }  \
            _asm { push    ebx       }  \
            _asm { push    ecx        }  \
            _asm { push    edx         }  \
            _asm { pushfd               }  \
            _asm { pop     eax           }  \
            _asm { mov     ebx, eax       }  \
            _asm { xor     eax, 00200000h  }  \
            _asm { push    eax              }  \
            _asm { popfd                     }  \
            _asm { pushfd                     }  \
            _asm { pop     eax                 }  \
            _asm { cmp     eax, ebx             }  \
            _asm { jne     CPUID_supported       }  \
            _asm { mov     eax, 0                 }  \
            _asm { mov     dma_possible, eax       }  \
            _asm { jmp     exit_CPUID               }  \
    CPUID_supported:                            \
            _asm { mov     eax, 1                    }   \
            _asm { mov     dma_possible, eax          }   \
    exit_CPUID:                                    \
            _asm { pop     edx                         }    \
            _asm { pop     ecx                          }    \
            _asm { pop     ebx                           }    \
            _asm { pop     eax                            }    \
}

HRESULT
S3VirgeInit(ULONG VideoBufferLinearAddr,LPS3THKINFO sData)
    {
    int i;
    ULONG IDs[ S3DTK_KNOWN3DCHIPS ] = {
        D_S3VIRGE,
        D_S3VIRGEVX,
        D_S3VIRGEDXGX,
        D_S3VIRGEGX2,
        D_S3VIRGEGX2P,
        D_S3VIRGEGX2PM,
        D_S3M5,
        D_S3M5P,
        D_S3M5PM// D_S3VIRGEM5

    };
    ULONG   revision = 0;
    HANDLE  g_hS3knlVxd = 0;
    BOOL    result;
    DWORD   code = 12;              // S3KERNEL_GETDMABUF in miniVDD
    DWORD   outbuffer[] = { 0,0,0 };
    DWORD   cboutbuffer = 3;
    DWORD   dma_possible = FALSE;

/* test for type of 3D chip */

    revision = sData->bRevisionId;

    for( i = 0; i < S3DTK_KNOWN3DCHIPS; i++ ) {
        if ( IDs[ i ] == sData->wDeviceId ) {
            break;
        }
    }
    if ( i == S3DTK_KNOWN3DCHIPS ) {
        return DDERR_CURRENTLYNOTAVAIL;
    }

    if ( (sData->wDeviceId !=  D_S3VIRGE) && (sData->wDeviceId !=  D_S3VIRGEVX)  )
    {
        if(     sData->D3DGlobals.bCommandDMA == 1)
        {
            dma_possible = TRUE;
        }
        else if(sData->D3DGlobals.bCommandDMA == 0)
        {
            dma_possible = FALSE;
        }
        else if(sData->D3DGlobals.bCommandDMA == 2)
        {
            CPUID_DMA_Possible();     //check for CPUID support, problem with some K6's
            if(dma_possible)
            {
                if(sData->wDeviceId == D_S3VIRGEGX2)
                {
// Commented out to allow Microsoft source to build without changes
// to D3DGlobals. OK since Microsoft doesn't support GX2 yet.  Restore
// this code when GX2 support is merged in. -BJT
//                    if(sData->D3DGlobals.Gx2DmaBrdFix)
//                    {
//                        dma_possible = TRUE;
//                    }
//                    else
//                    {
                        dma_possible = FALSE;
//                    }
                }
                else if((sData->wDeviceId == D_S3M5) ||     // <@ WCS 11/10/97
                        (sData->wDeviceId == D_S3M5P) ||
                        (sData->wDeviceId == D_S3M5PM ))    // $> WCS 11/10/97
                {
                    dma_possible = FALSE;
                }
                else
                {
                    DMA_Possible();   //check for P5, if P5 then DMA is possible
                    if(dma_possible)
                    {
                        if (sData->wDeviceId == D_S3VIRGEDXGX)
                        {
                            if( revision == 0)
                            {
                                dma_possible = FALSE;    //DX GX revA dma doesn't work
                            }
                            else if(revision ==1)
                            {       //DX revB work, GX revB doesn't
                                if ( sData->band.MemType == SGRAM )
                                { // is GX
                                    dma_possible = FALSE;
                                }
                                else
                                { // is DX
                                    dma_possible = TRUE;
                                }
                            } //rev B
                        } //GXDX
                    }  //is P5
                }    //not GX2 & not M5
            } // cpuid check
        }    //bCommandDMA = 2
        //sData->D3DGlobals.DXGX            = _myfl8const ;   //FLINT24:8
        sData->D3DGlobals.DXGX            = 4 ;
        sData->D3DGlobals.uBaseHigh       = 0x80000000;             //ViRGE DX/GX without D change
        //sData->D3DGlobals.coord_adj       = -0.001f;//0.000;//merge with ms
        sData->D3DGlobals.__UVRANGE       = ( 2048.0f - __UVBASEDEL ) ;
    }
    else {
        sData->D3DGlobals.dma_possible    = FALSE;
        //sData->D3DGlobals.DXGX            = _myfl12const ;  //FLINT20:12
        sData->D3DGlobals.DXGX            = 0 ;  //FLINT20:12
        sData->D3DGlobals.uBaseHigh       = 0;
        //sData->D3DGlobals.coord_adj       = 0.000f;//merge with ms
        sData->D3DGlobals.__UVRANGE       = ( 128.0f  - __UVBASEDEL ) ;
    }


    VideoBufferLinearAddr = sData->dwScreenFlatAddr;

    sData->D3DGlobals.g_p3DStp   = ( vi13D_SETUP * )   ( sData->lpMMReg + 0xB4D4L );
    sData->D3DGlobals.g_p3DTri   = ( vi13D_TRIANGLE * )( sData->lpMMReg + 0xB504L );

    *(DWORD*)(VideoBufferLinearAddr+
    (sData->D3DGlobals.dwWhiteTexture = sData->dwVideoMemorySize-8)) = 0xFFFFFFFF;

    if ( !sData->D3DGlobals.DMA_Choice ) // do following only once
    {
        sData->D3DGlobals.DMA_Choice = TRUE;

        sData->D3DGlobals.g_DmaIndex  = 0;
        sData->D3DGlobals.dma_possible = dma_possible;

        if ( (sData->dwDeviceHandle == 1) && dma_possible )
        {
            ULONG   DMAsetup;
            ULONG   DMAlinear;
            ULONG   DMAphysical;
            g_hS3knlVxd = CreateFile("\\\\.\\S3VSMP.VXD", 0,0,0,
                CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
            if ( g_hS3knlVxd != INVALID_HANDLE_VALUE )
            {
                result = DeviceIoControl(g_hS3knlVxd,code,NULL,0,
                    (LPVOID)outbuffer,cboutbuffer,NULL,NULL);
                DMAphysical = outbuffer[0];
                DMAlinear   = outbuffer[1];
                DMAsetup    = outbuffer[2];
            }
            sData->D3DGlobals.g_DmaLinAddr     = (ULONG *)DMAlinear;
            sData->D3DGlobals.g_lpWritePtrReg  = (ULONG *)(sData->lpMMReg + 0x8594L);
            sData->D3DGlobals.g_lpReadPtrReg   = (ULONG *)(sData->lpMMReg + 0x8598L);
            sData->D3DGlobals.g_lpDmaEnableReg = (ULONG *)(sData->lpMMReg + 0x859CL);
            if ( !DMAsetup ) // 0 first time VDD called, 1 ever after
            {
                *(ULONG *)(sData->lpMMReg + 0x8590L) = DMAphysical ; //4k only
                *(sData->D3DGlobals.g_lpReadPtrReg)  = *(sData->D3DGlobals.g_lpWritePtrReg) = 0;
                sData->D3DGlobals.g_DmaReadPtr = 0;
            }
            sData->D3DGlobals.CMD_SET                 = 0;
            sData->D3DGlobals.g_p3DTri                = ( vi13D_TRIANGLE * )((void *)sData->D3DGlobals.g_DmaLinAddr)     ;
            sData->D3DGlobals.g_p3DStp                = ( vi13D_SETUP * )((void *)sData->D3DGlobals.g_DmaLinAddr)       ;
        }
        else
        {
            sData->D3DGlobals.dma_possible = FALSE;     // force if on secondary
            sData->D3DGlobals.CMD_SET                 = 0x2CL;
        }
    }

    return D3D_OK;
}


#endif

/*----------------------------01-26-96 S3 11:14am-----------------------------
myContextCreate

LPD3DHAL_CONTEXTCREATEDATA pccd
- Pointer to a structure containing things including the current
rendering surface, the current Z surface, and the DirectX object
handle, etc.
----------------------------------------------------------------------------*/
DWORD __stdcall myContextCreate(LPD3DHAL_CONTEXTCREATEDATA pccd) {
/*
* NOTES:
*
* This callback is invoked when a new surface is to be used as a
* rendering target.
* The context handled returned will be used whenever rendering
* to this surface is to be performed.
*/
    S3_CONTEXT *ctxt;
    ULONG FrameAddress;
    LPS3THKINFO sData = GetMyRefData(pccd->lpDDGbl);
// Get Pointer to context
    ctxt = (LPS3_CONTEXT)MEMALLOC(sizeof(S3_CONTEXT));
    if (! ctxt) {
        DPF( "in myContextCreate, MEMALLOC returns NULL, failing" );
        pccd->ddrval = D3DHAL_OUTOFCONTEXTS;
        return (DDHAL_DRIVER_HANDLED);
    }
    memset(ctxt, 0, sizeof(S3_CONTEXT));
    ctxt->Next=sData->D3DGlobals.FirstContxt;
    if (ctxt->Next){
        ctxt->Next->Last=ctxt;
    }
    sData->D3DGlobals.FirstContxt=ctxt;
    pccd->dwhContext = (ULONG)ctxt;
    FrameAddress = (ULONG) pccd->lpDDGbl->fpPrimaryOrig;

    #ifndef WINNT
    if ( ! sData->D3DGlobals.bInitialized ) {
        HRESULT hr = S3VirgeInit(FrameAddress,sData);
        if (hr != D3D_OK) {
            pccd->ddrval = hr;
            return DDHAL_DRIVER_HANDLED;
        }
        sData->D3DGlobals.bInitialized = TRUE;
    }
    #endif

// cannot render into system memory
    if ( DDSurf_Get_dwCaps(DDS_LCL(pccd->lpDDS)) & DDSCAPS_SYSTEMMEMORY ) {
        DPF( "in myContextCreate, surface in SYSTEM MEMORY, failing" );
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }
    if ( pccd->lpDDSZ && (DDSurf_Get_dwCaps(DDS_LCL(pccd->lpDDSZ)) & DDSCAPS_SYSTEMMEMORY) ) {
        DPF( "in myContextCreate, Z surface in SYSTEM MEMORY, failing" );
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }


    // Note that this context is now valid by inserting the PID
    ctxt->dwPID = pccd->dwPID;

    DPF( "in myContextCreate, pccd->lpDD =%08lx, pccd->lpDDS =%08lx",
    pccd->lpDDGbl, pccd->lpDDS);
    DPF( "                    pccd->dwhContext =%08lx, pccd->lpDDSZ= %08lx",
    pccd->dwhContext, pccd->lpDDSZ);
    DPF( "                    pccd->dwPID =%08lx",
    pccd->dwPID);

    #ifndef WINNT
    // Save the address to video memory for finding offsets later.
    ctxt->FrameAddr = (ULONG) pccd->lpDDGbl->fpPrimaryOrig;
    #else
    ctxt->ppdev = (PDEV *)pccd->lpDDGbl->dhpdev;
    ctxt->FrameAddr = 0;                          // ctxt->ppdev->pjScreen;
    ctxt->g_p3DStp   = ( vi13D_SETUP * )( ctxt->ppdev->pjMmBase + 0xB4D4L );
    ctxt->g_p3DTri   = ( vi13D_TRIANGLE * )( ctxt->ppdev->pjMmBase + 0xB504L );
    #endif

    ctxt->lpLcl = DDS_LCL(pccd->lpDDS);
    if (! ctxt->lpLcl) {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }

    if(pccd->lpDDSZ)
        ctxt->lpLclZ = DDS_LCL(pccd->lpDDSZ);

    // On context creation, no render states are overridden

    // set default context
    if( ctxt->lpLclZ ) {
        ctxt->bZEnabled = TRUE;
        ctxt->bZWriteEnabled = TRUE;
        ctxt->dwRCode=S3GOURAUD | S3ZBUFFER;
    }
    else{
        ctxt->bZEnabled = FALSE;
        ctxt->bZWriteEnabled = FALSE;
        ctxt->dwRCode=S3GOURAUD;
    }
    if (sData->D3DGlobals.dma_possible) ctxt->RStyle.bDma=TRUE;
    ctxt->bChopped = FALSE;
    ctxt->bChanged = TRUE;
    ctxt->bZVisible = FALSE;
    ctxt->bPerspective = FALSE;
    ctxt->BlendMode = D3DTBLEND_MODULATE;
    ctxt->TextureAddress = D3DTADDRESS_WRAP;
    ctxt->bWrapU = ctxt->bWrapV = FALSE;
    ctxt->ShadeMode = D3DSHADE_GOURAUD;
    ctxt->bSpecular = FALSE;
    ctxt->ZCmpFunc = D3DCMP_LESS;
    ctxt->FillMode = D3DFILL_SOLID;
    ctxt->CullMode = D3DCULL_CCW;
    ctxt->Alpha_workaround = FALSE;
    ctxt->SrcBlend = D3DPBLENDCAPS_ONE;
    ctxt->bBlendEnabled = FALSE;
    ctxt->dwTexture=0;
    // This loads the pointer of the sData into all the contexts
    ctxt->sData = sData;

// Report success
    pccd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);

}                                                 /* myContextCreate */


/*----------------------------01-26-96 S3 11:12am-----------------------------
myContextDestroy

LPD3DHAL_CONTEXTDESTROYDATA pcdd
- Pointer to Context destroy information.
----------------------------------------------------------------------------*/
DWORD __stdcall myContextDestroy(LPD3DHAL_CONTEXTDESTROYDATA pcdd) {
/*
 * NOTES:
 *
 * This callback is invoked when a context is to be destroyed.
 * Perform any cleanup required.
 */
    S3_TEXTURE  *texture,*temp;
    S3_CONTEXT  *ctxt=(LPS3_CONTEXT)pcdd->dwhContext;
    if (ctxt){
// Cleanup any associated textures for this
        texture=ctxt->TextureList;
        while(texture)
        {
            temp=texture;
            texture=texture->Next;
            MEMFREE(temp);
        }

// Destroy context
            DPF( "in myContextDestroy, pcdd->dwhContext =%08lx", ctxt);
        if (ctxt->Next) ctxt->Next->Last=ctxt->Last;
        if (ctxt->Last)
            ctxt->Last->Next=ctxt->Next;
        else
            ctxt->sData->D3DGlobals.FirstContxt=ctxt->Next;
            MEMFREE(ctxt);
            pcdd->dwhContext=0;
    }
        pcdd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
}                                                 /* myContextDestroy */


/*----------------------------03-22-96 09:12am--------------------------------
myContextDestroyAll

LPD3DHAL_CONTEXTDESTROYALLDATA
- Pointer to a structure for contexts to destroy based on a given
process
----------------------------------------------------------------------------*/
DWORD __stdcall myContextDestroyAll(LPD3DHAL_CONTEXTDESTROYALLDATA pcdd) {
/*
 * NOTES: This call back should be obsolete in DX5, therefore it's OK to use
 *     sDataGlobal as all pre-DX5 runtime only supports single Monitor.
 * This callback is invoked when a process dies.  All the contexts
 * which were created by this context need to be destroyed.
 */
    D3DHAL_CONTEXTDESTROYDATA   cdd;
    S3_CONTEXT  *ctxt=sDataGlobal->D3DGlobals.FirstContxt,*temp;
        DPF( "in myContextDestroyAll, pcdd->dwPID =%08lx", pcdd->dwPID);
        while(ctxt){
        temp=ctxt->Next;
        if (ctxt->dwPID == pcdd->dwPID){
            cdd.dwhContext=(DWORD)ctxt;
            myContextDestroy(&cdd);
        }
        ctxt=temp;
        }
        pcdd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
}



