//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;
    
#ifdef __cplusplus

extern "C" {

#include "strmini.h"
#include "ksmedia.h"
#include "ddkmapi.h"

extern GUID MY_KSEVENTSETID_VPNOTIFY;
extern GUID MY_KSEVENTSETID_VPVBINOTIFY;

}

#endif

#include "capmain.h"
#include "capdebug.h"
#include "defaults.h"
#include "device.h"
    
#define DD_OK 0

DWORD FAR PASCAL DirectDrawEventCallback(DWORD dwEvent, PVOID pContext, DWORD dwParam1, DWORD dwParam2)
{
    switch (dwEvent)
    {
        case DDNOTIFY_PRERESCHANGE:
            {
                PSTREAMEX pStrmEx = (PSTREAMEX)pContext;
                PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
                int streamNumber = pStrmEx->pStreamObject->StreamNumber;
            
                DBGTRACE(("Bt829: DDNOTIFY_PRERESCHANGE; stream = %d\n", streamNumber));
    
                pHwDevExt->preEventOccurred = TRUE;
            }
            
            break;

        case DDNOTIFY_POSTRESCHANGE:
            {
                PSTREAMEX pStrmEx = (PSTREAMEX)pContext;
                PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
                int streamNumber = pStrmEx->pStreamObject->StreamNumber;
            
                DBGTRACE(("Bt829: DDNOTIFY_POSTRESCHANGE; stream = %d\n", streamNumber));
    
                pHwDevExt->postEventOccurred = TRUE;
                DBGTRACE(("Bt829: Before Attempted Renegotiation due to DDNOTIFY_POSTRESCHANGE\n"));
                AttemptRenegotiation(pStrmEx);
                DBGTRACE(("Bt829: Afer Attempted Renegotiation due to DDNOTIFY_POSTRESCHANGE\n"));
            }
            
            break;

        case DDNOTIFY_PREDOSBOX:
            {
                PSTREAMEX pStrmEx = (PSTREAMEX)pContext;
                PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
                int streamNumber = pStrmEx->pStreamObject->StreamNumber;
            
                DBGTRACE(("Bt829: DDNOTIFY_PREDOSBOX; stream = %d\n", streamNumber));
    
                pHwDevExt->preEventOccurred = TRUE;
            }
            
            break;

        case DDNOTIFY_POSTDOSBOX:
            {
                PSTREAMEX pStrmEx = (PSTREAMEX)pContext;
                PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
                int streamNumber = pStrmEx->pStreamObject->StreamNumber;
            
                DBGTRACE(("Bt829: DDNOTIFY_POSTDOSBOX; stream = %d\n", streamNumber));
    
                pHwDevExt->postEventOccurred = TRUE;
                DBGTRACE(("Bt829: Before Attempted Renegotiation due to DDNOTIFY_POSTDOSBOX\n"));
                AttemptRenegotiation(pStrmEx);
                DBGTRACE(("Bt829: After Attempted Renegotiation due to DDNOTIFY_POSTDOSBOX\n"));
            }
            
            break;

        case DDNOTIFY_CLOSEDIRECTDRAW:
            {
                PHW_DEVICE_EXTENSION    pHwDevExt = (PHW_DEVICE_EXTENSION)pContext;
            
                DBGTRACE(("Bt829: DDNOTIFY_CLOSEDIRECTDRAW\n"));
    
                pHwDevExt->ring0DirectDrawHandle = 0;
                pHwDevExt->ring3DirectDrawHandle = 0;
            }
            
            break;

#if 0
        case DDNOTIFY_CLOSESURFACE:
            {
                PHW_DEVICE_EXTENSION    pHwDevExt = (PHW_DEVICE_EXTENSION)pContext;
            
                DBGTRACE(("Bt829: DDNOTIFY_CLOSESURFACE\n"));
    
                pHwDevExt->ring0SurfaceHandle = 0;
                pHwDevExt->ring3SurfaceHandle = 0;
            }
            
            break;

#endif
        case DDNOTIFY_CLOSEVIDEOPORT:
            {
                PHW_DEVICE_EXTENSION    pHwDevExt = (PHW_DEVICE_EXTENSION)pContext;
            
                DBGTRACE(("Bt829: DDNOTIFY_CLOSEVIDEOPORT\n"));
    
                pHwDevExt->ring0VideoPortHandle = 0;
                pHwDevExt->ring3VideoPortHandle = -1;
            }
            
            break;

        case DDNOTIFY_CLOSECAPTURE:
            {
                PSTREAMEX pStrmEx = (PSTREAMEX)pContext;
                int streamNumber = pStrmEx->pStreamObject->StreamNumber;
            
                DBGTRACE(("Bt829: DDNOTIFY_CLOSECAPTURE; stream = %d\n", streamNumber));
    
                pStrmEx->hCapture = 0;
    
            }
            
            break;

        default:
            TRAP();
            break;
    }
    return 0;
}

BOOL RegisterForDirectDrawEvents(PSTREAMEX pStrmEx)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    DDREGISTERCALLBACK ddRegisterCallback;
    DWORD ddOut;

    DBGTRACE(("Bt829: Stream %d registering for DirectDraw events\n", streamNumber));
    
    // =============== DDEVENT_PRERESCHANGE ===============
    RtlZeroMemory(&ddRegisterCallback, sizeof(ddRegisterCallback));
    RtlZeroMemory(&ddOut, sizeof(ddOut));

    ddRegisterCallback.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    ddRegisterCallback.dwEvents = DDEVENT_PRERESCHANGE;
    ddRegisterCallback.pfnCallback = DirectDrawEventCallback;
    ddRegisterCallback.pContext = pStrmEx;

    DxApi(DD_DXAPI_REGISTER_CALLBACK, (DWORD) &ddRegisterCallback, sizeof(ddRegisterCallback), (DWORD)&ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_REGISTER_CALLBACK failed.\n"));
        TRAP();
        return FALSE;
    }

    // =============== DDEVENT_POSTRESCHANGE ==============
    RtlZeroMemory(&ddRegisterCallback, sizeof(ddRegisterCallback));
    RtlZeroMemory(&ddOut, sizeof(ddOut));

    ddRegisterCallback.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    ddRegisterCallback.dwEvents = DDEVENT_POSTRESCHANGE;
    ddRegisterCallback.pfnCallback = DirectDrawEventCallback;
    ddRegisterCallback.pContext = pStrmEx;

    DxApi(DD_DXAPI_REGISTER_CALLBACK, (DWORD) &ddRegisterCallback, sizeof(ddRegisterCallback), (DWORD)&ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_REGISTER_CALLBACK failed.\n"));
        TRAP();
        return FALSE;
    }

    // =============== DDEVENT_PREDOSBOX =================
    RtlZeroMemory(&ddRegisterCallback, sizeof(ddRegisterCallback));
    RtlZeroMemory(&ddOut, sizeof(ddOut));

    ddRegisterCallback.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    ddRegisterCallback.dwEvents = DDEVENT_PREDOSBOX;
    ddRegisterCallback.pfnCallback = DirectDrawEventCallback;
    ddRegisterCallback.pContext = pStrmEx;

    DxApi(DD_DXAPI_REGISTER_CALLBACK, (DWORD) &ddRegisterCallback, sizeof(ddRegisterCallback), (DWORD)&ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_REGISTER_CALLBACK failed.\n"));
        TRAP();
        return FALSE;
    }

    // =============== DDEVENT_POSTDOSBOX ================
    RtlZeroMemory(&ddRegisterCallback, sizeof(ddRegisterCallback));
    RtlZeroMemory(&ddOut, sizeof(ddOut));

    ddRegisterCallback.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    ddRegisterCallback.dwEvents = DDEVENT_POSTDOSBOX;
    ddRegisterCallback.pfnCallback = DirectDrawEventCallback;
    ddRegisterCallback.pContext = pStrmEx;

    DxApi(DD_DXAPI_REGISTER_CALLBACK, (DWORD) &ddRegisterCallback, sizeof(ddRegisterCallback), (DWORD)&ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_REGISTER_CALLBACK failed.\n"));
        TRAP();
        return FALSE;
    }
    pStrmEx->registered = TRUE;

    return TRUE;
}


BOOL UnregisterForDirectDrawEvents(PSTREAMEX pStrmEx)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    DDREGISTERCALLBACK ddRegisterCallback;
    DWORD ddOut;

    DBGTRACE(("Bt829: Stream %d UNregistering for DirectDraw events\n", streamNumber));
    
    // =============== DDEVENT_PRERESCHANGE ===============
    RtlZeroMemory(&ddRegisterCallback, sizeof(ddRegisterCallback));
    RtlZeroMemory(&ddOut, sizeof(ddOut));

    ddRegisterCallback.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    ddRegisterCallback.dwEvents = DDEVENT_PRERESCHANGE ;
    ddRegisterCallback.pfnCallback = DirectDrawEventCallback;
    ddRegisterCallback.pContext = pStrmEx;

    DxApi(DD_DXAPI_UNREGISTER_CALLBACK, (DWORD) &ddRegisterCallback, sizeof(ddRegisterCallback), (DWORD)&ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_UNREGISTER_CALLBACK failed.\n"));
        TRAP();
        return FALSE;
    }

    // =============== DDEVENT_POSTRESCHANGE ==============
    RtlZeroMemory(&ddRegisterCallback, sizeof(ddRegisterCallback));
    RtlZeroMemory(&ddOut, sizeof(ddOut));

    ddRegisterCallback.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    ddRegisterCallback.dwEvents = DDEVENT_POSTRESCHANGE;
    ddRegisterCallback.pfnCallback = DirectDrawEventCallback;
    ddRegisterCallback.pContext = pStrmEx;

    DxApi(DD_DXAPI_UNREGISTER_CALLBACK, (DWORD) &ddRegisterCallback, sizeof(ddRegisterCallback), (DWORD)&ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_UNREGISTER_CALLBACK failed.\n"));
        TRAP();
        return FALSE;
    }

    // =============== DDEVENT_PREDOSBOX ==================
    RtlZeroMemory(&ddRegisterCallback, sizeof(ddRegisterCallback));
    RtlZeroMemory(&ddOut, sizeof(ddOut));

    ddRegisterCallback.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    ddRegisterCallback.dwEvents = DDEVENT_PREDOSBOX;
    ddRegisterCallback.pfnCallback = DirectDrawEventCallback;
    ddRegisterCallback.pContext = pStrmEx;

    DxApi(DD_DXAPI_UNREGISTER_CALLBACK, (DWORD) &ddRegisterCallback, sizeof(ddRegisterCallback), (DWORD)&ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_UNREGISTER_CALLBACK failed.\n"));
        TRAP();
        return FALSE;
    }

    // =============== DDEVENT_POSTDOSBOX =================
    RtlZeroMemory(&ddRegisterCallback, sizeof(ddRegisterCallback));
    RtlZeroMemory(&ddOut, sizeof(ddOut));

    ddRegisterCallback.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    ddRegisterCallback.dwEvents = DDEVENT_POSTDOSBOX;
    ddRegisterCallback.pfnCallback = DirectDrawEventCallback;
    ddRegisterCallback.pContext = pStrmEx;

    DxApi(DD_DXAPI_UNREGISTER_CALLBACK, (DWORD) &ddRegisterCallback, sizeof(ddRegisterCallback), (DWORD)&ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_UNREGISTER_CALLBACK failed.\n"));
        TRAP();
        return FALSE;
    }
    pStrmEx->registered = FALSE;
    
    return TRUE;
}


BOOL GetRing0DirectDrawHandle(PSTREAMEX pStrmEx)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    
    if (pHwDevExt->ring0DirectDrawHandle == 0)
    {
        DBGTRACE(("Bt829: Stream %d getting ring0 ddraw handle\n", streamNumber));
        
        DDOPENDIRECTDRAWIN  ddOpenIn;
        DDOPENDIRECTDRAWOUT ddOpenOut;

        RtlZeroMemory(&ddOpenIn, sizeof(ddOpenIn));
        RtlZeroMemory(&ddOpenOut, sizeof(ddOpenOut));

        ddOpenIn.dwDirectDrawHandle = pHwDevExt->ring3DirectDrawHandle;
        ddOpenIn.pfnDirectDrawClose = DirectDrawEventCallback;
        ddOpenIn.pContext = pHwDevExt;

        DxApi(DD_DXAPI_OPENDIRECTDRAW, (DWORD) &ddOpenIn, sizeof(ddOpenIn), (DWORD)&ddOpenOut, sizeof(ddOpenOut));

        if (ddOpenOut.ddRVal != DD_OK)
        {
            pHwDevExt->ring0DirectDrawHandle = 0;
            DBGERROR(("Bt829: DD_DXAPI_OPENDIRECTDRAW failed.\n"));
            TRAP();
            return FALSE;
        }
        else
        {
            pHwDevExt->ring0DirectDrawHandle = ddOpenOut.hDirectDraw;
        }
    }
    return TRUE;
}
    

BOOL GetRing0VideoPortHandle(PSTREAMEX pStrmEx)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;

    if (pHwDevExt->ring0VideoPortHandle == 0)
    {
        DBGTRACE(("Bt829: Stream %d getting ring0 vport handle\n", streamNumber));
        
        DDOPENVIDEOPORTIN  ddOpenVPIn;
        DDOPENVIDEOPORTOUT ddOpenVPOut;
        RtlZeroMemory(&ddOpenVPIn, sizeof(ddOpenVPIn));
        RtlZeroMemory(&ddOpenVPOut, sizeof(ddOpenVPOut));

        ddOpenVPIn.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
        ddOpenVPIn.pfnVideoPortClose = DirectDrawEventCallback;
        ddOpenVPIn.pContext = pHwDevExt;

        ddOpenVPIn.dwVideoPortHandle = pHwDevExt->ring3VideoPortHandle;
        
        DxApi(DD_DXAPI_OPENVIDEOPORT, (DWORD)&ddOpenVPIn, sizeof(ddOpenVPIn), (DWORD)&ddOpenVPOut, sizeof(ddOpenVPOut));

        if (ddOpenVPOut.ddRVal != DD_OK)
        {
            pHwDevExt->ring0VideoPortHandle = 0;
            DBGERROR(("Bt829: DD_DXAPI_OPENVIDEOPORT failed.\n"));
            TRAP();
            return FALSE;
        }
        else
        {
            pHwDevExt->ring0VideoPortHandle = ddOpenVPOut.hVideoPort;
        }
    }    
    return TRUE;
}

    
BOOL GetCaptureHandle(PSTREAMEX pStrmEx)
{    
    PHW_DEVICE_EXTENSION pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;

    if (pStrmEx->hCapture == 0)
    {
        DBGTRACE(("Bt829: Stream %d getting capture handle\n", streamNumber));
        
        DDOPENVPCAPTUREDEVICEIN  ddOpenCaptureIn;
        DDOPENVPCAPTUREDEVICEOUT ddOpenCaptureOut;

        RtlZeroMemory(&ddOpenCaptureIn, sizeof(ddOpenCaptureIn));
        RtlZeroMemory(&ddOpenCaptureOut, sizeof(ddOpenCaptureOut));

        ddOpenCaptureIn.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
        ddOpenCaptureIn.hVideoPort = pHwDevExt->ring0VideoPortHandle;
        ddOpenCaptureIn.pfnCaptureClose = DirectDrawEventCallback;
        ddOpenCaptureIn.pContext = pStrmEx;

        // Now to get the size, etc
        switch (streamNumber)
        {
        case STREAM_VideoCapture:
            {
                KS_VIDEOINFOHEADER  *pVideoInfoHdr = pStrmEx->pVideoInfoHeader;
                RECT                rcImage;

                /* 
                **  HOW BIG IS THE IMAGE REQUESTED (pseudocode follows)
                **
                **  if (IsRectEmpty (&rcTarget) {
                **      SetRect (&rcImage, 0, 0, 
                **              BITMAPINFOHEADER.biWidth,
                                BITMAPINFOHEADER.biHeight);
                **  }
                **  else {
                **      // Probably rendering to a DirectDraw surface,
                **      // where biWidth is used to expressed the "stride" 
                **      // in units of pixels (not bytes) of the destination surface.
                **      // Therefore, use rcTarget to get the actual image size 
                **      
                **      rcImage = rcTarget;
                **  }
                */

                if ((pVideoInfoHdr->rcTarget.right - 
                     pVideoInfoHdr->rcTarget.left <= 0) ||
                    (pVideoInfoHdr->rcTarget.bottom - 
                     pVideoInfoHdr->rcTarget.top <= 0)) {

                     rcImage.left = rcImage.top = 0;
                     rcImage.right = pVideoInfoHdr->bmiHeader.biWidth - 1;
                     rcImage.bottom = pVideoInfoHdr->bmiHeader.biHeight - 1;
                }
                else {
                     rcImage = pVideoInfoHdr->rcTarget;
                }
                ddOpenCaptureIn.dwStartLine = rcImage.top + pHwDevExt->device->VideoSurfaceOriginY;
                ddOpenCaptureIn.dwEndLine = rcImage.bottom + pHwDevExt->device->VideoSurfaceOriginY;

                // Fail-safe
                if (ddOpenCaptureIn.dwStartLine > 500)
                {
                    DBGERROR(("Bt829: Unexpected capture start line. Using default\n"));
                    ddOpenCaptureIn.dwStartLine = 0;
                }

                if (ddOpenCaptureIn.dwEndLine > 500)
                {
                    Device *device = pHwDevExt->device;
                    DBGERROR(("Bt829: Unexpected capture end line. Using default\n"));
                    ddOpenCaptureIn.dwEndLine = device->GetDecoderHeight() - 1;
                }
                DBGINFO(("Bt829: Video surface: %d, %d\n",
                    ddOpenCaptureIn.dwStartLine,
                    ddOpenCaptureIn.dwEndLine));

                ddOpenCaptureIn.dwFlags = DDOPENCAPTURE_VIDEO;

                // Integer math, so it will throw away fractional part
                pStrmEx->everyNFields = min ( max (   1,
                                (ULONG) pVideoInfoHdr->AvgTimePerFrame/NTSCFieldDuration),
                                MAXULONG);

                // Now look at that fractional part. If there was a significant
                // amount, we'll need to round down to the next nearest
                // frame rate (i.e., skip additional field)

                // 'Significant' is currently assumed to be 1 uS. That
                // is '10' in units of 100ns 
                if ((pVideoInfoHdr->AvgTimePerFrame -
                     (NTSCFieldDuration * pStrmEx->everyNFields)) > 10)
                {
                    pStrmEx->everyNFields++;
                }

                ddOpenCaptureIn.dwCaptureEveryNFields = pStrmEx->everyNFields;
                       
                DBGINFO(("Bt829: Capturing every %d fields\n",
                                ddOpenCaptureIn.dwCaptureEveryNFields));
            }

            break;

        case STREAM_VBICapture:
            {
                KS_VBIINFOHEADER  *pVBIInfoHdr = pStrmEx->pVBIInfoHeader;

                ddOpenCaptureIn.dwStartLine = 0 + pHwDevExt->device->VBISurfaceOriginY;
                ddOpenCaptureIn.dwEndLine = VBILines - 1 + pHwDevExt->device->VBISurfaceOriginY;

                // Fail-safe
                if (ddOpenCaptureIn.dwStartLine > 500)
                {
                    DBGERROR(("Bt829: Unexpected VBI start line = %d. Using default\n"));
                    ddOpenCaptureIn.dwStartLine = pVBIInfoHdr->StartLine - VREFDiscard - 1;

                }

                if (ddOpenCaptureIn.dwEndLine > 500)
                {
                    DBGERROR(("Bt829: Unexpected VBI end line. Using default\n"));
                    ddOpenCaptureIn.dwEndLine = pVBIInfoHdr->EndLine - VREFDiscard - 1;
                }

                DBGINFO(("Bt829: VBI surface: %d, %d\n",
                    ddOpenCaptureIn.dwStartLine,
                    ddOpenCaptureIn.dwEndLine));

                ddOpenCaptureIn.dwFlags = DDOPENCAPTURE_VBI;
                pStrmEx->everyNFields = 1;
                ddOpenCaptureIn.dwCaptureEveryNFields = pStrmEx->everyNFields;
            }
            
            break;

        default:
            DBGERROR(("Bt829: Unexpected stream number trying to get Capture handle\n"));
            TRAP();
            return FALSE;
        }

        DxApi(DD_DXAPI_OPENVPCAPTUREDEVICE, (DWORD)&ddOpenCaptureIn, sizeof(ddOpenCaptureIn), (DWORD)&ddOpenCaptureOut, sizeof(ddOpenCaptureOut));

        if (ddOpenCaptureOut.ddRVal != DD_OK)
        {
            pStrmEx->hCapture = 0;
            DBGERROR(("Bt829: DD_DXAPI_OPENVPCAPTUREDEVICE failed.\n"));
            // TRAP();
            return FALSE;
        }
        else
        {
            pStrmEx->hCapture = ddOpenCaptureOut.hCapture;
        }
    }
    return TRUE;
}   

    
#if 0
BOOL GetRing0SurfaceHandle(PSTREAMEX pStrmEx)
{    
    PHW_DEVICE_EXTENSION pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;

    if (pHwDevExt->ring0SurfaceHandle == 0)
    {
        DBGTRACE(("Bt829: Stream %d getting ring0 surface handle\n", streamNumber));
        
        DDOPENSURFACEIN ddOpenSurfaceIn;
        DDOPENSURFACEOUT ddOpenSurfaceOut;

        RtlZeroMemory(&ddOpenSurfaceIn, sizeof(ddOpenSurfaceIn));
        RtlZeroMemory(&ddOpenSurfaceOut, sizeof(ddOpenSurfaceOut));

        ddOpenSurfaceIn.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    
        ddOpenSurfaceIn.pfnSurfaceClose = DirectDrawEventCallback;
        ddOpenSurfaceIn.pContext = pHwDevExt;

        if (streamNumber == STREAM_VPVideo)
        {
            ddOpenSurfaceIn.dwSurfaceHandle = pHwDevExt->ring3SurfaceHandles[0];
        }
        else if (streamNumber == STREAM_VPVBI)
        {
            ddOpenSurfaceIn.dwSurfaceHandle = pHwDevExt->ring3VBISurfaceHandles[0];
        }
    
        DxApi(DD_DXAPI_OPENSURFACE, (DWORD)&ddOpenSurfaceIn, sizeof(ddOpenSurfaceIn), (DWORD)&ddOpenSurfaceOut, sizeof(ddOpenSurfaceOut));

        if (ddOpenSurfaceOut.ddRVal != DD_OK)
        {
            pHwDevExt->ring0SurfaceHandle = 0;
            DBGERROR(("Bt829: DD_DXAPI_OPENSURFACE failed.\n"));
            TRAP();
            return FALSE;
        }
        else
        {
            pHwDevExt->ring0SurfaceHandle = ddOpenSurfaceOut.hSurface;
        }
    }
    return TRUE;
}
#endif

BOOL ReleaseRing0VideoPortHandle(PSTREAMEX pStrmEx)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    
    DWORD ddOut;

    DDCLOSEHANDLE ddClose;

    if (pHwDevExt->ring0VideoPortHandle != 0)
    {
        DBGTRACE(("Bt829: Stream %d releasing ring0 vport handle\n", streamNumber));
        
        ddClose.hHandle = pHwDevExt->ring0VideoPortHandle;

        DxApi(DD_DXAPI_CLOSEHANDLE, (DWORD)&ddClose, sizeof(ddClose), (DWORD) &ddOut, sizeof(ddOut));

        if (ddOut != DD_OK)
        {
            DBGERROR(("Bt829: DD_DXAPI_CLOSEHANDLE failed.\n"));
            TRAP();
            return FALSE;
        }
        pHwDevExt->ring0VideoPortHandle = 0;
    }
    return TRUE;
}

BOOL ReleaseRing0DirectDrawHandle(PSTREAMEX pStrmEx)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    DWORD ddOut;
    DDCLOSEHANDLE ddClose;

    if (pHwDevExt->ring0DirectDrawHandle != 0)
    {
        DBGTRACE(("Bt829: Stream %d releasing ring0 ddraw handle\n", streamNumber));
        
        ddClose.hHandle = pHwDevExt->ring0DirectDrawHandle;

        DxApi(DD_DXAPI_CLOSEHANDLE, (DWORD)&ddClose, sizeof(ddClose), (DWORD) &ddOut, sizeof(ddOut));

        if (ddOut != DD_OK)
        {
            DBGERROR(("Bt829: DD_DXAPI_CLOSEHANDLE failed.\n"));
            TRAP();
            return FALSE;
        }
        pHwDevExt->ring0DirectDrawHandle = 0;
    }
    return TRUE;
}

BOOL ReleaseCaptureHandle(PSTREAMEX pStrmEx)
{
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    DWORD ddOut;
    DDCLOSEHANDLE ddClose;

    if (pStrmEx->hCapture != 0)
    {
        DBGTRACE(("Bt829: Stream %d releasing capture handle\n", streamNumber));
        
        ddClose.hHandle = pStrmEx->hCapture;

        DxApi(DD_DXAPI_CLOSEHANDLE, (DWORD)&ddClose, sizeof(ddClose), (DWORD) &ddOut, sizeof(ddOut));

        if (ddOut != DD_OK)
        {
            DBGERROR(("Bt829: DD_DXAPI_CLOSEHANDLE failed.\n"));
            TRAP();
            return FALSE;
        }
        pStrmEx->hCapture = 0;
    }
    return TRUE;
}

#if 0
BOOL ReleaseRing0SurfaceHandle(PPSTREAMEX pStrmEx)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    DWORD ddOut;

    DDCLOSEHANDLE ddClose;

    if (pHwDevExt->ring0SurfaceHandle != 0)
    {
        DBGTRACE(("Bt829: Stream %d releasing ring0 surface handle\n", streamNumber));
        
        ddClose.hHandle = pHwDevExt->ring0SurfaceHandle;

        DxApi(DD_DXAPI_CLOSEHANDLE, (DWORD)&ddClose, sizeof(ddClose), (DWORD) &ddOut, sizeof(ddOut));

        if (ddOut != DD_OK)
        {
            DBGERROR(("Bt829: DD_DXAPI_CLOSEHANDLE failed.\n"));
            TRAP();
            return FALSE;
        }
        pHwDevExt->ring0SurfaceHandle = 0;
    }

    return TRUE;
}
#endif

void AddBuffersToDirectDraw(PSTREAMEX pStrmEx)
{
    KIRQL Irql;
    
    KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
    
    while (!IsListEmpty(&pStrmEx->incomingDataSrbQueue))
    {
        PHW_STREAM_REQUEST_BLOCK pCurrentSrb = NULL;
        PSRB_EXTENSION pSrbExt;
        PUCHAR ptr;

        // So if we've reached this point, we are in the run state, and
        // we have an SRB on our incoming queue, and we are holding the
        // the stream lock
        ptr = (PUCHAR)RemoveHeadList(&pStrmEx->incomingDataSrbQueue);
        pSrbExt = (PSRB_EXTENSION) (((PUCHAR) ptr) - FIELDOFFSET(SRB_EXTENSION, listEntry));
        pCurrentSrb = pSrbExt->pSrb;

        DBGINFO(("'Bt829: Removed 0x%x from data queue\n", pCurrentSrb));

        if (AddBuffer(pCurrentSrb))
        {
            DBGINFO(("'Bt829: Adding 0x%x to wait queue\n", pCurrentSrb));
            InsertTailList(&pStrmEx->waitQueue, &pSrbExt->listEntry);
        }
        else
        {
            DBGINFO(("'Bt829: Adding 0x%x back to dataqueue\n", pCurrentSrb));

            // put it back where it was
            InsertHeadList(&pStrmEx->incomingDataSrbQueue, &pSrbExt->listEntry);
            break;
        }
    }
    KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
}

BOOL AddBuffer(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    DDADDVPCAPTUREBUFF ddAddVPCaptureBuffIn;
    DWORD ddOut;

    PIRP irp = pSrb->Irp;
    PSRB_EXTENSION pSrbExt = (PSRB_EXTENSION)pSrb->SRBExtension;
    
    PKEVENT pKEvent = &pSrbExt->bufferDoneEvent;


    DBGINFO(("'Bt829: In AddBuffer. pSrb: 0x%x.\n", pSrb));

    // For handling full-screen DOS, res changes, etc.
    if (pStrmEx->hCapture == 0)
    {
        if (!GetCaptureHandle(pStrmEx))
        {
            return FALSE;
        }
    }

    ddAddVPCaptureBuffIn.hCapture = pStrmEx->hCapture;
    ddAddVPCaptureBuffIn.dwFlags = DDADDBUFF_SYSTEMMEMORY;
    ddAddVPCaptureBuffIn.pMDL = irp->MdlAddress;

    ddAddVPCaptureBuffIn.lpBuffInfo = &pSrbExt->ddCapBuffInfo;
    ddAddVPCaptureBuffIn.pKEvent = pKEvent;

    DxApi(DD_DXAPI_ADDVPCAPTUREBUFFER, (DWORD) &ddAddVPCaptureBuffIn, sizeof(ddAddVPCaptureBuffIn), (DWORD) &ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        // Not necessarily an error.
        DBGINFO(("'Bt829: DD_DXAPI_ADDVPCAPTUREBUFFER failed.\n"));
        // TRAP();
        return FALSE;
    }

    return TRUE;
}

VOID StreamThread(PVOID pVoid)
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pVoid;
    PHW_STREAM_REQUEST_BLOCK pCurrentSrb = NULL;
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    KEVENT DummyEvent;
    const int numEvents = 3;

    NTSTATUS status;
    PUCHAR ptr;

    // Wo unto you if you overrun this array
    PVOID eventArray[numEvents];

    KeInitializeEvent(&DummyEvent, SynchronizationEvent, FALSE);

    ASSERT(pStrmEx->stateChange == Starting);

    // Indicates to AdapterOpenStream to continue
    pStrmEx->stateChange = ChangeComplete;
    KeSetEvent(&pStrmEx->specialEvent, 0, FALSE);

    pCurrentSrb = NULL;

    // These should remain constant the whole time
    eventArray[0] = &pStrmEx->stateTransitionEvent;
    eventArray[1] = &pStrmEx->SrbAvailableEvent;

    // eventArray[2] changes, so it is set below

    // This runs until the thread terminates itself
    // inside of HandleStateTransition
    while (1)
    {
// BUGBUG. May not be necessary
#define ENABLE_TIMEOUT
#ifdef ENABLE_TIMEOUT
        LARGE_INTEGER i;
#endif

        PSRB_EXTENSION pSrbExt = NULL;
        if (pCurrentSrb == NULL)
        {
            ptr = (PUCHAR)ExInterlockedRemoveHeadList(&pStrmEx->waitQueue, &pStrmEx->streamExtensionLock);

            if (ptr == NULL)
            {
#ifdef DEBUG
                if (pStrmEx->KSState == KSSTATE_RUN &&
                    pStrmEx->stateChange == ChangeComplete &&
                    pHwDevExt->preEventOccurred == FALSE)
                {
                    static int j;

                    // Indicates that we are starved for buffers. Probably
                    // a higher level is not handing them to us in a timely
                    // fashion for some reason
                    DBGWARN((" S "));
                    if ((++j % 10) == 0)
                    {
                        DBGWARN(("\n"));
                    }
                }
#endif
                pCurrentSrb = NULL;
                eventArray[2] = &DummyEvent;
            }
            else
            {
                pSrbExt = (PSRB_EXTENSION) (((PUCHAR) ptr) - FIELDOFFSET(SRB_EXTENSION, listEntry));
                pCurrentSrb = pSrbExt->pSrb;
                eventArray[2] = &pSrbExt->bufferDoneEvent;
            }
        }

#ifdef ENABLE_TIMEOUT
        // BUGBUG. Since this is meant mainly as a failsafe measure, I
        // just picked a value pretty much out of thin air.
        i.QuadPart = -2000000;      // 200 ms
#endif
        
        status = KeWaitForMultipleObjects(  numEvents,  // count
                                            eventArray, // DispatcherObjectArray
                                            WaitAny,    // WaitType
                                            Executive,  // WaitReason
                                            KernelMode, // WaitMode
                                            FALSE,      // Alertable
#ifdef ENABLE_TIMEOUT
                                            &i,         // Timeout (Optional)
#else
                                            NULL,
#endif
                                            NULL);      // WaitBlockArray (Optional)

        switch (status)
        {
            // State transition. May including killing this very thread
            case 0:
                HandleStateTransition(pStrmEx);
                break;

            // New Srb available
            case 1:
                if (pStrmEx->KSState == KSSTATE_RUN && pStrmEx->stateChange == ChangeComplete)
                {
                    AddBuffersToDirectDraw(pStrmEx);
                }
                break;

            // Busmaster complete
            case 2:
                HandleBusmasterCompletion(pCurrentSrb);
                pCurrentSrb = NULL;
                break;

#ifdef ENABLE_TIMEOUT
            // If we timeout in the RUN state, this is our chance to try again
            // to add buffers. May not be necessary, since currently, we go
            // through a state transition for DOS boxes, etc.
            case STATUS_TIMEOUT:
                if (pStrmEx->KSState == KSSTATE_RUN &&
                    pStrmEx->stateChange == ChangeComplete &&
                    pHwDevExt->preEventOccurred == FALSE)
                {
                    AddBuffersToDirectDraw(pStrmEx);
                }
                break;
#endif

            default:
                TRAP();
                break;
        }
    }
}

BOOL ResetFieldNumber(PSTREAMEX pStrmEx)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int                     streamNumber = pStrmEx->pStreamObject->StreamNumber;
    DDSETFIELDNUM           ddSetFieldNum;
    DWORD                   ddOut;

    ASSERT(streamNumber == STREAM_VideoCapture || streamNumber == STREAM_VBICapture);

    if (pHwDevExt->ring0DirectDrawHandle == 0) {
        DBGERROR(("Bt829: Didn't expect pHwDevExt->ring0DirectDrawHandle to be zero.\n"));
        TRAP();
        return FALSE;
    }
    
    if (pHwDevExt->ring0VideoPortHandle == 0) {
        DBGERROR(("Bt829: Didn't expect pHwDevExt->ring0VideoPortHandle to be zero.\n"));
        TRAP();
        return FALSE;
    }
    
    RtlZeroMemory(&ddSetFieldNum, sizeof(ddSetFieldNum));
    RtlZeroMemory(&ddOut, sizeof(ddOut));

    ddSetFieldNum.hDirectDraw = pHwDevExt->ring0DirectDrawHandle;
    ddSetFieldNum.hVideoPort = pHwDevExt->ring0VideoPortHandle;

    if (streamNumber == STREAM_VideoCapture)
    {
        ddSetFieldNum.dwFieldNum = ((ULONG) pStrmEx->FrameInfo.PictureNumber + 1) *
                                    pStrmEx->everyNFields;
    }
    else if (streamNumber == STREAM_VBICapture)
    {
        ddSetFieldNum.dwFieldNum = ((ULONG) pStrmEx->VBIFrameInfo.PictureNumber + 1) *
                                    pStrmEx->everyNFields;
    }
    
    DxApi(DD_DXAPI_SET_VP_FIELD_NUMBER, (DWORD) &ddSetFieldNum, sizeof(ddSetFieldNum), (DWORD) &ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_SET_VP_FIELD_NUMBER failed.\n"));
        TRAP();
        return FALSE;
    }
    else
    {
#ifdef DEBUG
        if (streamNumber == STREAM_VideoCapture)
        {
            DBGINFO(("Bt829: PictureNumber: %d; ", pStrmEx->FrameInfo.PictureNumber));
            DBGINFO(("Bt829: DropCount: %d\n", pStrmEx->FrameInfo.DropCount));
        }
        else
        {
            DBGINFO(("Bt829: PictureNumber: %d; ", pStrmEx->VBIFrameInfo.PictureNumber));
            DBGINFO(("Bt829: DropCount: %d\n", pStrmEx->VBIFrameInfo.DropCount));
        }
#endif
        return TRUE;
    }
}

BOOL FlushBuffers(PSTREAMEX pStrmEx)
{
    DWORD ddOut;

    if (pStrmEx->hCapture == NULL) {
       DBGERROR(("Bt829: hCapture === NULL in FlushBuffers.\n"));
       TRAP();
       return FALSE;
    }

    DxApi(DD_DXAPI_FLUSHVPCAPTUREBUFFERS, (DWORD) &pStrmEx->hCapture, sizeof(HANDLE), (DWORD) &ddOut, sizeof(ddOut));

    if (ddOut != DD_OK)
    {
        DBGERROR(("Bt829: DD_DXAPI_FLUSHVPCAPTUREBUFFERS failed.\n"));
        TRAP();
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
    

VOID HandleBusmasterCompletion(PHW_STREAM_REQUEST_BLOCK pCurrentSrb)
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pCurrentSrb->StreamObject->HwStreamExtension;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pCurrentSrb->HwDeviceExtension);
    PSRB_EXTENSION      pSrbExt = (PSRB_EXTENSION)pCurrentSrb->SRBExtension;
    PKSSTREAM_HEADER    pDataPacket = pCurrentSrb->CommandData.DataBufferArray;
    KIRQL Irql;
    
    // First handle case where we get a Busmaster completion
    // indication while we are trying to pause or stop
    if (pStrmEx->stateChange == Pausing || pStrmEx->stateChange == Stopping)
    {
        PUCHAR ptr;
        KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);

        // Put it at the head of the temporary 'reversal' queue.
        InsertHeadList(&pStrmEx->reversalQueue, &pSrbExt->listEntry);
        
        if (IsListEmpty(&pStrmEx->waitQueue))
        {
            // if there is nothing left in the wait queue we can now
            // proceed to move everything back to the incoming queue.
            // This whole ugly ordeal is to
            // make sure that they end up in the original order
            while (!IsListEmpty(&pStrmEx->reversalQueue))
            {
                ptr = (PUCHAR)RemoveHeadList(&pStrmEx->reversalQueue);
                InsertHeadList(&pStrmEx->incomingDataSrbQueue, (PLIST_ENTRY) ptr);
            }
            
            KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
            
            if (pStrmEx->stateChange == Stopping)
            {
                EmptyIncomingDataSrbQueue(pStrmEx);
            }
            
            // Indicate that we have successfully completed this part
            // of the transition to the pause state.
            pStrmEx->stateChange = ChangeComplete;
            KeSetEvent(&pStrmEx->specialEvent, 0, FALSE);
            return;
        }

        KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
        return;
    }

    // else it is a regular busmaster completion while in the run state
    else
    {
        ASSERT (pCurrentSrb);
        PKSSTREAM_HEADER    pDataPacket = pCurrentSrb->CommandData.DataBufferArray;
        pDataPacket->OptionsFlags = 0;

        pSrbExt = (PSRB_EXTENSION)pCurrentSrb->SRBExtension;

        DBGINFO(("'Bt829: FieldNum: %d; ddRVal: 0x%x; polarity: 0x%x\n",
                 pSrbExt->ddCapBuffInfo.dwFieldNumber,
                 pSrbExt->ddCapBuffInfo.ddRVal,
                 pSrbExt->ddCapBuffInfo.bPolarity));

        // It's possible that the srb got cancelled while we were waiting.
        // Currently, this status is reset below
        if (pCurrentSrb->Status == STATUS_CANCELLED)
        {
            DBGINFO(("'Bt829: pCurrentSrb 0x%x was cancelled while we were waiting\n", pCurrentSrb));
            pDataPacket->DataUsed = 0;
        }

        // It's also possible that there was a problem in DD-land
        else if (pSrbExt->ddCapBuffInfo.ddRVal != DD_OK)
        {
            // Two cases of which I am aware.
            // 1) flushed buffers
            if (pSrbExt->ddCapBuffInfo.ddRVal == 0x80004005)
            {
                DBGINFO(("Bt829: ddRVal = 0x%x. Assuming we flushed\n", pSrbExt->ddCapBuffInfo.ddRVal));
                pDataPacket->DataUsed = 0;
            }
            // 2) something else
            else
            {
                DBGERROR(("Bt829: = 0x%x. Problem in Busmastering?\n", pSrbExt->ddCapBuffInfo.ddRVal));
                pDataPacket->DataUsed = 0;
            }
        }

        // There is also the remote possibility that everything is OK
        else
        {
            SetFrameInfo(pCurrentSrb);
            TimeStampSrb(pCurrentSrb);
            pDataPacket->DataUsed = pDataPacket->FrameExtent;
        }
        
        DBGINFO(("'Bt829: StreamRequestComplete for SRB 0x%x\n", pCurrentSrb));

        // BUGBUG. For the time being, always return success. Failure
        // is indicated by setting DataUsed to 0.
        pCurrentSrb->Status = STATUS_SUCCESS;

        ASSERT(pCurrentSrb->Irp->MdlAddress);

        StreamClassStreamNotification(  StreamRequestComplete,
                                        pCurrentSrb->StreamObject,
                                        pCurrentSrb);
    }
}

VOID SetFrameInfo(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSRB_EXTENSION      pSrbExt = (PSRB_EXTENSION)pSrb->SRBExtension;
    PKSSTREAM_HEADER    pDataPacket = pSrb->CommandData.DataBufferArray;
    
    LONGLONG droppedThisTime = 0;
    if (streamNumber == STREAM_VideoCapture)
    {
        PKS_FRAME_INFO pFrameInfo = (PKS_FRAME_INFO) (pDataPacket + 1);

        pStrmEx->FrameInfo.dwFrameFlags = 0;
        pStrmEx->FrameInfo.ExtendedHeaderSize = pFrameInfo->ExtendedHeaderSize;

        // Set the discontinuity flag if frames have been previously dropped.
        if ((pStrmEx->FrameInfo.PictureNumber + 1) <
            pSrbExt->ddCapBuffInfo.dwFieldNumber/pStrmEx->everyNFields)
        {
            droppedThisTime =
            pSrbExt->ddCapBuffInfo.dwFieldNumber/pStrmEx->everyNFields -
            (pStrmEx->FrameInfo.PictureNumber + 1);
            pStrmEx->FrameInfo.DropCount += droppedThisTime;
            pDataPacket->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_DATADISCONTINUITY;
#ifdef DEBUG
            static int j;
            DBGERROR((" D%d ", droppedThisTime));
            if ((++j % 10) == 0)
            {
                DBGERROR(("\n"));
            }
#endif
        }
        pStrmEx->FrameInfo.PictureNumber = pSrbExt->ddCapBuffInfo.dwFieldNumber/pStrmEx->everyNFields;
        pStrmEx->FrameInfo.dwFrameFlags |= KS_VIDEO_FLAG_FRAME;
        *pFrameInfo = (KS_FRAME_INFO)pStrmEx->FrameInfo;

    }

    else if (streamNumber == STREAM_VBICapture)
    {
        KIRQL   Irql;
        PKS_VBI_FRAME_INFO  pFrameInfo = (PKS_VBI_FRAME_INFO) (pDataPacket + 1);

        pStrmEx->VBIFrameInfo.dwFrameFlags = 0;
        pStrmEx->VBIFrameInfo.ExtendedHeaderSize = pFrameInfo->ExtendedHeaderSize;

        // Set the discontinuity flag if frames have been previously dropped.
        if ((pStrmEx->VBIFrameInfo.PictureNumber + 1) <
            pSrbExt->ddCapBuffInfo.dwFieldNumber)
        {
            droppedThisTime =
            pSrbExt->ddCapBuffInfo.dwFieldNumber -
            (pStrmEx->VBIFrameInfo.PictureNumber + 1);
            pStrmEx->VBIFrameInfo.DropCount += droppedThisTime;
            pDataPacket->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_DATADISCONTINUITY;
#ifdef DEBUG
            static int j;
            DBGERROR((" D%d ", droppedThisTime));
            if ((++j % 10) == 0)
            {
                DBGERROR(("\n"));
            }
#endif
        }
        pStrmEx->VBIFrameInfo.PictureNumber = pSrbExt->ddCapBuffInfo.dwFieldNumber;
        pStrmEx->VBIFrameInfo.dwSamplingFrequency = SamplingFrequency;

        if (pHwDevExt->TVTunerChanged == TRUE)
        {
            PHW_STREAM_REQUEST_BLOCK    TVTunerChangedSrb; 
            KeAcquireSpinLock(&pHwDevExt->deviceExtensionLock, &Irql);
            RtlCopyMemory(&pStrmEx->VBIFrameInfo.TvTunerChangeInfo,
                          &pHwDevExt->TVTunerChangeInfo,
                          sizeof (KS_TVTUNER_CHANGE_INFO));

            pStrmEx->VBIFrameInfo.dwFrameFlags |= KS_VBI_FLAG_TVTUNER_CHANGE;
            TVTunerChangedSrb = pHwDevExt->TVTunerChangedSrb;
            pHwDevExt->TVTunerChangedSrb = NULL;
            pHwDevExt->TVTunerChanged = FALSE;
            KeReleaseSpinLock(&pHwDevExt->deviceExtensionLock, Irql);
            StreamClassStreamNotification(  StreamRequestComplete,
                                            TVTunerChangedSrb->StreamObject,
                                            TVTunerChangedSrb);
#ifdef DEBUG
            if (pHwDevExt->TVTunerChangeInfo.dwFlags & KS_TVTUNER_CHANGE_BEGIN_TUNE)
            {
                DBGTRACE(("Bt829: Sending KS_TVTUNER_CHANGE_BEGIN_TUNE.\n"));
            }
            else if (pHwDevExt->TVTunerChangeInfo.dwFlags & KS_TVTUNER_CHANGE_END_TUNE)
            {
                DBGTRACE(("Bt829: Sending KS_TVTUNER_CHANGE_END_TUNE.\n"));
            }
            else
            {
                DBGERROR(("Bt829: Unexpected value in TVTunerChangeInfo.dwFlags\n"));
            }
#endif
        }

        if (pHwDevExt->VBIInfoChanged == TRUE)
        {
            KeAcquireSpinLock(&pHwDevExt->deviceExtensionLock, &Irql);
            RtlCopyMemory(&pStrmEx->VBIFrameInfo.VBIInfoHeader,
                          pStrmEx->pVBIInfoHeader,
                          sizeof (KS_VBIINFOHEADER));

            pStrmEx->VBIFrameInfo.dwFrameFlags |= KS_VBI_FLAG_VBIINFOHEADER_CHANGE;
            pHwDevExt->VBIInfoChanged = FALSE;
            KeReleaseSpinLock(&pHwDevExt->deviceExtensionLock, Irql);
        }

        if (pSrbExt->ddCapBuffInfo.bPolarity)
        {
            pStrmEx->VBIFrameInfo.dwFrameFlags |= KS_VIDEO_FLAG_FIELD2;
        }
        else
        {
            pStrmEx->VBIFrameInfo.dwFrameFlags |= KS_VIDEO_FLAG_FIELD1;
        }

        *pFrameInfo = (KS_VBI_FRAME_INFO)pStrmEx->VBIFrameInfo;
    }
}

VOID TimeStampSrb(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX           pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    PKSSTREAM_HEADER    pDataPacket = pSrb->CommandData.DataBufferArray;
    PSRB_EXTENSION      pSrbExt = (PSRB_EXTENSION)pSrb->SRBExtension;
    BOOL                Failed = FALSE;

    pDataPacket->Duration = pStrmEx->everyNFields * NTSCFieldDuration;

    pDataPacket->OptionsFlags |= 
        KSSTREAM_HEADER_OPTIONSF_DURATIONVALID |
        KSSTREAM_HEADER_OPTIONSF_SPLICEPOINT;

    // Find out what time it is, if we're using a clock

    if (pStrmEx->hMasterClock ) {
        LARGE_INTEGER Delta;

        HW_TIME_CONTEXT TimeContext;

        TimeContext.HwDeviceExtension = pHwDevExt; 
        TimeContext.HwStreamObject = pStrmEx->pStreamObject;
        TimeContext.Function = TIME_GET_STREAM_TIME;

        StreamClassQueryMasterClockSync (
            pStrmEx->hMasterClock,
            &TimeContext);

        // This calculation should result in the stream time WHEN the buffer
        // was filled.
        Delta.QuadPart = TimeContext.SystemTime -
                            pSrbExt->ddCapBuffInfo.liTimeStamp.QuadPart;

        if (TimeContext.Time > (ULONGLONG) Delta.QuadPart)
        {
            pDataPacket->PresentationTime.Time = TimeContext.Time - Delta.QuadPart;
        }
        else
        {
            // BUGBUG!!!
            // There's a bug in Ks or Stream after running for 2 hours
            // that makes this hack necessary.  Will be fixed soon...
            pDataPacket->PresentationTime.Time = TimeContext.Time;
            Failed = TRUE;
        }

#if 0 //def DEBUG
        ULONG *tmp1, *tmp2, *tmp3, *tmp4;

        tmp1 = (ULONG *)&Delta;
        tmp2 = (ULONG *)&TimeContext.SystemTime;
        tmp3 = (ULONG *)&pSrbExt->ddCapBuffInfo.liTimeStamp;
        tmp4 = (ULONG *)&pDataPacket->PresentationTime.Time;
        DBGINFO(("Bt829: Delta: 0x%x%x; ST: 0x%x%x; DD: 0x%x%x; PT: 0x%x%x\n", 
            tmp1[1], tmp1[0], tmp2[1], tmp2[0], tmp3[1], tmp3[0], tmp4[1], tmp4[0]));

        if (Failed) {
            TRAP();
        }
#endif


        pDataPacket->PresentationTime.Numerator = 1;
        pDataPacket->PresentationTime.Denominator = 1;

        pDataPacket->OptionsFlags |= 
            KSSTREAM_HEADER_OPTIONSF_TIMEVALID;
    }
    else
    {
        pDataPacket->OptionsFlags &= 
            ~KSSTREAM_HEADER_OPTIONSF_TIMEVALID;
    }
}

VOID HandleStateTransition(PSTREAMEX pStrmEx)
{
    KIRQL Irql;
    switch (pStrmEx->stateChange)
    {
        case Running:
            AddBuffersToDirectDraw(pStrmEx);
            pStrmEx->stateChange = ChangeComplete;
            KeSetEvent(&pStrmEx->specialEvent, 0, FALSE);
            break;

        case Pausing:
            KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
            if (IsListEmpty(&pStrmEx->waitQueue))
            {
                KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
                pStrmEx->stateChange = ChangeComplete;
                KeSetEvent(&pStrmEx->specialEvent, 0, FALSE);
            }
            else
            {
                KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
            }
            break;

        case Stopping:
            KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
            if (IsListEmpty(&pStrmEx->waitQueue))
            {
                KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
                EmptyIncomingDataSrbQueue(pStrmEx);
                pStrmEx->stateChange = ChangeComplete;
                KeSetEvent(&pStrmEx->specialEvent, 0, FALSE);
            }
            else
            {
                KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
            }
            break;

        case Closing:
            pStrmEx->stateChange = ChangeComplete;
            KeSetEvent(&pStrmEx->specialEvent, 0, FALSE);
            DBGTRACE(("Bt829: StreamThread exiting\n"));
            
            PsTerminateSystemThread(STATUS_SUCCESS);

            DBGERROR(("Bt829: Shouldn't get here\n"));
            TRAP();
            break;

        case ChangeComplete:
            DBGTRACE(("'Bt829: Must have completed transition in HandleBusMasterCompletion\n"));
            break;

        default:
            TRAP();
            break;
    }
}

VOID AttemptRenegotiation(PSTREAMEX pStrmEx)
{
    int streamNumber = pStrmEx->pStreamObject->StreamNumber;
    if (pStrmEx->EventCount)
    {
        DBGINFO(("Bt829: Attempting renegotiation on stream %d\n", streamNumber));

        if (streamNumber == STREAM_VPVideo)
        {
            StreamClassStreamNotification(
                SignalMultipleStreamEvents,
                pStrmEx->pStreamObject,
                &MY_KSEVENTSETID_VPNOTIFY,
                KSEVENT_VPNOTIFY_FORMATCHANGE);
        }
        else if (streamNumber == STREAM_VPVBI)
        {
            StreamClassStreamNotification(
                SignalMultipleStreamEvents,
                pStrmEx->pStreamObject,
                &MY_KSEVENTSETID_VPVBINOTIFY,
                KSEVENT_VPVBINOTIFY_FORMATCHANGE);
        }
    }
    else
    {
        DBGINFO(("Bt829: NOT attempting renegotiation on stream %d\n", streamNumber));
    }
}

VOID EmptyIncomingDataSrbQueue(PSTREAMEX pStrmEx)
{
    KIRQL Irql;
    
    // BUGBUG. Think about replacing with ExInterlockedRemoveHeadList. 
    KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
    
    while (!IsListEmpty(&pStrmEx->incomingDataSrbQueue))
    {
        PUCHAR ptr;
        PSRB_EXTENSION pSrbExt;
        PHW_STREAM_REQUEST_BLOCK pCurrentSrb = NULL;
        PKSSTREAM_HEADER pDataPacket;

        ptr = (PUCHAR)RemoveHeadList(&pStrmEx->incomingDataSrbQueue);
        pSrbExt = (PSRB_EXTENSION) (((PUCHAR) ptr) - FIELDOFFSET(SRB_EXTENSION, listEntry));
        pCurrentSrb = pSrbExt->pSrb;
        
        pCurrentSrb->Status = STATUS_SUCCESS;
        pDataPacket = pCurrentSrb->CommandData.DataBufferArray;
        pDataPacket->DataUsed = 0;
        
        KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
        DBGINFO(("'Bt829: Completing Srb 0x%x in STATE_STOP\n", pCurrentSrb));
        StreamClassStreamNotification(  StreamRequestComplete,
                                        pCurrentSrb->StreamObject,
                                        pCurrentSrb);
        KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
    }
    
    KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
}

