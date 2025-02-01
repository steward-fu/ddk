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
extern "C"
{
#endif
#include "strmini.h"
#include "ksmedia.h"
#ifdef __cplusplus
}
#endif

#include "capmain.h"
#include "capdebug.h"

//#define INITGUID

#include "initguid.h"
#include "capvport.h"
#include "device.h"
#include "defaults.h"

// #define BT829_VPCONNECTIONS_NUMBER  2
#define BT829_VPCONNECTIONS_NUMBER  1
#define BT829_PIXELFORMATS_NUMBER   1
#define NTSC_FRAME_RATE 30
#define BT829_LOST_LINES	2  // BT829
#define BT829A_LOST_LINES	3   // BT829a

VOID GetVideoPortProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    Device *device = pHwDevExt->device;
    PSTREAM_PROPERTY_DESCRIPTOR pSpd = pSrb->CommandData.PropertyInfo;
    ULONG Id  = pSpd->Property->Id;              // index of the property
    ULONG nS  = pSpd->PropertyOutputSize;        // size of data supplied

    ASSERT( nS >= sizeof( ULONG));

    pSrb->Status = STATUS_SUCCESS;

    switch (Id)
    {
        case KSPROPERTY_VPCONFIG_NUMCONNECTINFO :
            // 2 VideoPort connections are possible
            *( PULONG)( pSpd->PropertyInfo) = BT829_VPCONNECTIONS_NUMBER;

            pSrb->ActualBytesTransferred = sizeof( ULONG);
            break;

        case KSPROPERTY_VPCONFIG_GETCONNECTINFO :

            ASSERT( nS >= BT829_VPCONNECTIONS_NUMBER * sizeof( DDVIDEOPORTCONNECT));

            {
                LPDDVIDEOPORTCONNECT pConnectInfo;

                pConnectInfo = ( LPDDVIDEOPORTCONNECT)  \
                               ( pSrb->CommandData.PropertyInfo->PropertyInfo);

                // fill in the DDVIDEOPORTCONNECT structure 1
                pConnectInfo->dwSize = sizeof( DDVIDEOPORTCONNECT);
                pConnectInfo->guidTypeID = DDVPTYPE_BROOKTREE;
                pConnectInfo->dwPortWidth = 8;
                pConnectInfo->dwFlags = DDVPCONNECT_INVERTPOLARITY;
                pConnectInfo->dwReserved1 = 0;

#if 0
                // fill in the DDVIDEOPORTCONNECT structure 2
                pConnectInfo ++;
                pConnectInfo->dwSize = sizeof( DDVIDEOPORTCONNECT);
                pConnectInfo->guidTypeID = DDVPTYPE_BROOKTREE;
                pConnectInfo->dwPortWidth = 16;
                pConnectInfo->dwFlags = DDVPCONNECT_INVERTPOLARITY;
                pConnectInfo->dwReserved1 = 0;
#endif
            }

            pSrb->ActualBytesTransferred = BT829_VPCONNECTIONS_NUMBER * sizeof( DDVIDEOPORTCONNECT);
            break;

        case KSPROPERTY_VPCONFIG_NUMVIDEOFORMAT :
            // 2 output pixel formats are possible
            *( PULONG)( pSpd->PropertyInfo) = BT829_PIXELFORMATS_NUMBER;

            pSrb->ActualBytesTransferred = sizeof( ULONG);
            break;

        case KSPROPERTY_VPCONFIG_GETVIDEOFORMAT :

            ASSERT( nS >= BT829_PIXELFORMATS_NUMBER * sizeof( DDPIXELFORMAT));

            {
                LPDDPIXELFORMAT pPixelFormat;

                pPixelFormat = ( LPDDPIXELFORMAT)   \
                               ( pSrb->CommandData.PropertyInfo->PropertyInfo);

                // fill in the DDPIXELFORMAT structure
                pPixelFormat->dwSize = sizeof( DDPIXELFORMAT);
                pPixelFormat->dwFourCC = FOURCC_UYVY;
                pPixelFormat->dwYUVBitCount = 16;
                pPixelFormat->dwFlags = DDPF_FOURCC;
                pPixelFormat->dwYBitMask = ( DWORD)0xFF00FF00;
                pPixelFormat->dwUBitMask = ( DWORD)0x000000FF;
                pPixelFormat->dwVBitMask = ( DWORD)0x00FF0000;
            }

            pSrb->ActualBytesTransferred = BT829_PIXELFORMATS_NUMBER * sizeof( DDPIXELFORMAT);
            break;

        case KSPROPERTY_VPCONFIG_VPDATAINFO :

            ASSERT( nS >= sizeof( KS_AMVPDATAINFO));

            {
                PKS_AMVPDATAINFO pAMVPDataInfo;

                pAMVPDataInfo = ( PKS_AMVPDATAINFO) \
                                ( pSrb->CommandData.PropertyInfo->PropertyInfo);

				int decoderLostLines = ( device->GetPartRev() >= 4) ?
					BT829A_LOST_LINES : BT829_LOST_LINES;

                // the values are sortof hardcoded for NTSC at this point
                // VBI values will need to be tweaked
                pAMVPDataInfo->dwSize = sizeof( PKS_AMVPDATAINFO);
                pAMVPDataInfo->dwMicrosecondsPerField = 16667;
                pAMVPDataInfo->bEnableDoubleClock = FALSE;
                pAMVPDataInfo->bEnableVACT = FALSE;

                pAMVPDataInfo->lHalfLinesOdd = 0;
                pAMVPDataInfo->lHalfLinesEven = 1;

                pAMVPDataInfo->bFieldPolarityInverted = FALSE;
                pAMVPDataInfo->bDataIsInterlaced = TRUE;
                pAMVPDataInfo->dwNumLinesInVREF = 6 - decoderLostLines;

                pAMVPDataInfo->amvpDimInfo.dwFieldWidth = device->GetDecoderWidth();
                
                pAMVPDataInfo->amvpDimInfo.dwFieldHeight =
                    device->GetDecoderHeight() +
                    (VBIEnd - 1) -  // the '- 1' makes VBIEnd zero-based
                    decoderLostLines -
                    pAMVPDataInfo->dwNumLinesInVREF;
                
                // BUGBUG. Beware of hard-coded numbers
                pAMVPDataInfo->amvpDimInfo.dwVBIWidth = VBISamples;
                pAMVPDataInfo->amvpDimInfo.dwVBIHeight = VBIEnd - decoderLostLines;

                pAMVPDataInfo->amvpDimInfo.rcValidRegion.left = 0;
                pAMVPDataInfo->amvpDimInfo.rcValidRegion.top = VBIEnd - decoderLostLines;
                pAMVPDataInfo->amvpDimInfo.rcValidRegion.right = pAMVPDataInfo->amvpDimInfo.dwFieldWidth;
                pAMVPDataInfo->amvpDimInfo.rcValidRegion.bottom = pAMVPDataInfo->amvpDimInfo.dwFieldHeight;

                pAMVPDataInfo->dwPictAspectRatioX = 4;
                pAMVPDataInfo->dwPictAspectRatioY = 3;
            }

            pSrb->ActualBytesTransferred = sizeof( KS_AMVPDATAINFO);
            break;

        case KSPROPERTY_VPCONFIG_MAXPIXELRATE :
            ASSERT( nS >= sizeof( PKSVPMAXPIXELRATE));

            {

                PKSVPMAXPIXELRATE pKSPixelRate;

                int decoderHeight = device->GetDecoderHeight();
                int decoderWidth = device->GetDecoderWidth();

                pKSPixelRate = ( PKSVPMAXPIXELRATE) \
                               ( pSrb->CommandData.PropertyInfo->PropertyInfo);

                pKSPixelRate->Size.dwWidth = decoderWidth;
                pKSPixelRate->Size.dwHeight = decoderHeight;
                pKSPixelRate->MaxPixelsPerSecond = decoderWidth * decoderHeight * NTSC_FRAME_RATE;
            }
            pSrb->ActualBytesTransferred = sizeof( KSVPMAXPIXELRATE);
            break;

        case KSPROPERTY_VPCONFIG_INFORMVPINPUT :
            pSrb->ActualBytesTransferred = 0;
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;

        case KSPROPERTY_VPCONFIG_DECIMATIONCAPABILITY :
            *( PBOOL)( pSpd->PropertyInfo) = TRUE;
            pSrb->ActualBytesTransferred = sizeof( BOOL);
            break;

        default:
            TRAP();
            pSrb->ActualBytesTransferred = 0;
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;
    }
}       


VOID SetVideoPortProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (( PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    Device *device = pHwDevExt->device;
    PSTREAM_PROPERTY_DESCRIPTOR pSpd = pSrb->CommandData.PropertyInfo;
    ULONG Id  = pSpd->Property->Id;            // index of the property
    ULONG nS  = pSpd->PropertyOutputSize;        // size of data supplied

    pSrb->Status = STATUS_SUCCESS;

    ASSERT (device != NULL);
    switch (Id)
    {
        case KSPROPERTY_VPCONFIG_DDRAWHANDLE:

            if (pHwDevExt->ring3DirectDrawHandle == NULL)
            {
                pHwDevExt->ring3DirectDrawHandle =
                    (*(PULONG)pSrb->CommandData.PropertyInfo->PropertyInfo);
                
                if (!GetRing0DirectDrawHandle(pStrmEx))
                {
                    pSrb->Status = STATUS_UNSUCCESSFUL;
                    break;
                }
            }
            
            if (!pStrmEx->registered) {
                if (!RegisterForDirectDrawEvents(pStrmEx))
                {
                    pSrb->Status = STATUS_UNSUCCESSFUL;
                    break;
                }
            }
            break;

        case KSPROPERTY_VPCONFIG_VIDEOPORTID:

            if (pHwDevExt->ring3VideoPortHandle == -1)
            {
                pHwDevExt->ring3VideoPortHandle =
                    (*(PULONG)pSrb->CommandData.PropertyInfo->PropertyInfo);
                
                if (!GetRing0VideoPortHandle(pStrmEx))
                {
                    pSrb->Status = STATUS_UNSUCCESSFUL;
                    break;
                }
            }

            break;

        case KSPROPERTY_VPCONFIG_DDRAWSURFACEHANDLE:

#if 0
            PULONG pHandles;
            PULONG pUlong;

            // This should be non-null only after full-screen DOS, for example
            if (pHwDevExt->ring3SurfaceHandles)
            {
                ExFreePool(pHwDevExt->ring3SurfaceHandles);
                pHwDevExt->ring3SurfaceHandles = NULL;
                pHwDevExt->numRing3SurfaceHandles = 0;
            }

            pUlong = ((PULONG)pSrb->CommandData.PropertyInfo->PropertyInfo);

            if (*pUlong == 0)
            {
                DBGERROR(("Bt829: Didn't expect zero handles\n"));
                TRAP();
                break;
            }

            pHwDevExt->numRing3SurfaceHandles = *pUlong;
            pHwDevExt->ring3SurfaceHandles = (PULONG)ExAllocatePool(NonPagedPool, ((*pUlong) * sizeof(HANDLE)));

            if (pHwDevExt->ring3SurfaceHandles == NULL)
            {
                DBGERROR(("Bt829: ExAllocatePool failed\n"));
                pSrb->Status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }

            pHandles = (pUlong + 1);

            RtlCopyMemory(
                         pHwDevExt->ring3SurfaceHandles,
                         pHandles,
                         ((*pUlong) * sizeof(ULONG)));

#endif
            pHwDevExt->preEventOccurred = pHwDevExt->postEventOccurred = FALSE;
            
            break;

        case KSPROPERTY_VPCONFIG_SETCONNECTINFO :
            {
                LPDDVIDEOPORTCONNECT pConnectInfo;

                pConnectInfo = ( LPDDVIDEOPORTCONNECT)  \
                               ( pSrb->CommandData.PropertyInfo->PropertyInfo);

                if ( pConnectInfo->dwPortWidth == 16)
                {
                    device->Set16BitDataStream(TRUE);
                }
                else
                {
                    device->Set16BitDataStream(FALSE);
                }
            } 
            break;

        case KSPROPERTY_VPCONFIG_INVERTPOLARITY :
            if ( *( PBOOL)( pSpd->PropertyInfo) == TRUE)
            {
                device->SetHighOdd(FALSE);
            }
            else
            {
                device->SetHighOdd(TRUE);
            }
            break;

        case KSPROPERTY_VPCONFIG_SETVIDEOFORMAT :
            //
            // pSrb->CommandData.PropertInfo->PropertyInfo
            // points to a ULONG which is an index into the array of
            // VIDEOFORMAT structs returned to the caller from the
            // Get call to FORMATINFO
            //
            // Since the sample only supports one FORMAT type right
            // now, we will ensure that the requested index is 0.
            //

            switch (*(PULONG)pSrb->CommandData.PropertyInfo->PropertyInfo)
            {
                case 0:

                    //
                    // at this point, we would program the hardware to use
                    // the right connection information for the videoport.
                    // since we are only supporting one connection, we don't
                    // need to do anything, so we will just indicate success
                    //

                    break;

                default:

                    pSrb->Status = STATUS_NO_MATCH;
                    break;
            }

            break;

        case KSPROPERTY_VPCONFIG_SCALEFACTOR :
            // TBD
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;

        case KSPROPERTY_VPCONFIG_SURFACEPARAMS :
            {
                PKSVPSURFACEPARAMS pSurfaceParams;

                ASSERT(nS >= sizeof( PKSVPSURFACEPARAMS));
                pSurfaceParams = (PKSVPSURFACEPARAMS)(pSrb->CommandData.PropertyInfo->PropertyInfo);

                DBGINFO(("Bt829: VP Surface Params:\n"));
                DBGINFO(("Bt829: dwPitch    = %d\n",pSurfaceParams->dwPitch));
                DBGINFO(("Bt829: dwXOrigin  = %d\n",pSurfaceParams->dwXOrigin));
                DBGINFO(("Bt829: dwYOrigin  = %d\n",pSurfaceParams->dwYOrigin));

                device->VideoSurfaceOriginX = pSurfaceParams->dwXOrigin;
                device->VideoSurfaceOriginY = pSurfaceParams->dwYOrigin;
                device->VideoSurfacePitch = pSurfaceParams->dwPitch;
            }
            break;

        default:
            TRAP();
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;
    }
}


VOID GetVideoPortVBIProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    Device *device = pHwDevExt->device;
    PSTREAM_PROPERTY_DESCRIPTOR pSpd = pSrb->CommandData.PropertyInfo;
    ULONG Id  = pSpd->Property->Id;              // index of the property
    ULONG nS  = pSpd->PropertyOutputSize;        // size of data supplied

    ASSERT( nS >= sizeof( ULONG));

    pSrb->Status = STATUS_SUCCESS;

    switch (Id)
    {
        case KSPROPERTY_VPCONFIG_NUMCONNECTINFO :
            // 2 VideoPort connections are possible
            *( PULONG)( pSpd->PropertyInfo) = BT829_VPCONNECTIONS_NUMBER;

            pSrb->ActualBytesTransferred = sizeof( ULONG);
            break;

        case KSPROPERTY_VPCONFIG_GETCONNECTINFO :

            ASSERT( nS >= BT829_VPCONNECTIONS_NUMBER * sizeof( DDVIDEOPORTCONNECT));

            {
                LPDDVIDEOPORTCONNECT pConnectInfo;

                pConnectInfo = ( LPDDVIDEOPORTCONNECT)  \
                               ( pSrb->CommandData.PropertyInfo->PropertyInfo);

                // fill in the DDVIDEOPORTCONNECT structure 1
                pConnectInfo->dwSize = sizeof( DDVIDEOPORTCONNECT);
                pConnectInfo->guidTypeID = DDVPTYPE_BROOKTREE;
                pConnectInfo->dwPortWidth = 8;
                pConnectInfo->dwFlags = DDVPCONNECT_INVERTPOLARITY;
                pConnectInfo->dwReserved1 = 0;

#if 0
                // fill in the DDVIDEOPORTCONNECT structure 2
                pConnectInfo ++;
                pConnectInfo->dwSize = sizeof( DDVIDEOPORTCONNECT);
                pConnectInfo->guidTypeID = DDVPTYPE_BROOKTREE;
                pConnectInfo->dwPortWidth = 16;
                pConnectInfo->dwFlags = DDVPCONNECT_INVERTPOLARITY;
                pConnectInfo->dwReserved1 = 0;
#endif
            }

            pSrb->ActualBytesTransferred = BT829_VPCONNECTIONS_NUMBER * sizeof( DDVIDEOPORTCONNECT);
            break;

        case KSPROPERTY_VPCONFIG_NUMVIDEOFORMAT :
            // 2 output pixel formats are possible
            *( PULONG)( pSpd->PropertyInfo) = BT829_PIXELFORMATS_NUMBER;

            pSrb->ActualBytesTransferred = sizeof( ULONG);
            break;

        case KSPROPERTY_VPCONFIG_GETVIDEOFORMAT :

            ASSERT( nS >= BT829_PIXELFORMATS_NUMBER * sizeof( DDPIXELFORMAT));

            {
                LPDDPIXELFORMAT pPixelFormat;

                pPixelFormat = ( LPDDPIXELFORMAT)   \
                               ( pSrb->CommandData.PropertyInfo->PropertyInfo);

                // fill in the DDPIXELFORMAT structure
                pPixelFormat->dwSize = sizeof( DDPIXELFORMAT);
                pPixelFormat->dwFourCC = FOURCC_VBID;
                pPixelFormat->dwYUVBitCount = 8;
                pPixelFormat->dwFlags = DDPF_FOURCC;
            }

            pSrb->ActualBytesTransferred = BT829_PIXELFORMATS_NUMBER * sizeof( DDPIXELFORMAT);
            break;

        case KSPROPERTY_VPCONFIG_VPDATAINFO :

            ASSERT( nS >= sizeof( KS_AMVPDATAINFO));

            {
                PKS_AMVPDATAINFO pAMVPDataInfo;

                pAMVPDataInfo = ( PKS_AMVPDATAINFO) \
                                ( pSrb->CommandData.PropertyInfo->PropertyInfo);

				int decoderLostLines = ( device->GetPartRev() >= 4) ?
					BT829A_LOST_LINES : BT829_LOST_LINES;

                // the values are sortof hardcoded for NTSC at this point
                // VBI values will need to be tweaked
                pAMVPDataInfo->dwSize = sizeof( PKS_AMVPDATAINFO);
                pAMVPDataInfo->dwMicrosecondsPerField = 16667;
                pAMVPDataInfo->bEnableDoubleClock = FALSE;
                pAMVPDataInfo->bEnableVACT = FALSE;

                pAMVPDataInfo->lHalfLinesOdd = 0;
                pAMVPDataInfo->lHalfLinesEven = 1;

                pAMVPDataInfo->bFieldPolarityInverted = FALSE;
                pAMVPDataInfo->bDataIsInterlaced = TRUE;
                pAMVPDataInfo->dwNumLinesInVREF = 6 - decoderLostLines;

                pAMVPDataInfo->amvpDimInfo.dwFieldWidth = device->GetDecoderWidth();
                
                pAMVPDataInfo->amvpDimInfo.dwFieldHeight =
                    device->GetDecoderHeight() +
                    (VBIEnd - 1) -  // the '- 1' makes VBIEnd zero-based
                    decoderLostLines -
                    pAMVPDataInfo->dwNumLinesInVREF;
                
                // BUGBUG. Beware of hard-coded numbers
                pAMVPDataInfo->amvpDimInfo.dwVBIWidth = VBISamples;
                pAMVPDataInfo->amvpDimInfo.dwVBIHeight = VBIEnd - decoderLostLines;

                pAMVPDataInfo->amvpDimInfo.rcValidRegion.left = 0;
                pAMVPDataInfo->amvpDimInfo.rcValidRegion.top = VBIStart - 1 - decoderLostLines;
                pAMVPDataInfo->amvpDimInfo.rcValidRegion.right = pAMVPDataInfo->amvpDimInfo.dwVBIWidth;
                pAMVPDataInfo->amvpDimInfo.rcValidRegion.bottom = pAMVPDataInfo->amvpDimInfo.dwVBIHeight;
            }

            pSrb->ActualBytesTransferred = sizeof( KS_AMVPDATAINFO);
            break;

        case KSPROPERTY_VPCONFIG_MAXPIXELRATE :
            ASSERT( nS >= sizeof( PKSVPMAXPIXELRATE));

            {

                PKSVPMAXPIXELRATE pKSPixelRate;

                int decoderHeight = device->GetDecoderHeight();
                int decoderWidth = device->GetDecoderWidth();

                pKSPixelRate = ( PKSVPMAXPIXELRATE) \
                               ( pSrb->CommandData.PropertyInfo->PropertyInfo);

                pKSPixelRate->Size.dwWidth = decoderWidth;
                pKSPixelRate->Size.dwHeight = decoderHeight;
                pKSPixelRate->MaxPixelsPerSecond = decoderWidth * decoderHeight * NTSC_FRAME_RATE;
            }
            pSrb->ActualBytesTransferred = sizeof( KSVPMAXPIXELRATE);
            break;

        case KSPROPERTY_VPCONFIG_INFORMVPINPUT :
            pSrb->ActualBytesTransferred = 0;
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;

        case KSPROPERTY_VPCONFIG_DECIMATIONCAPABILITY :
            pSrb->ActualBytesTransferred = 0;
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;

        default:
            TRAP();
            pSrb->ActualBytesTransferred = 0;
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;
    }
}       


VOID SetVideoPortVBIProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (( PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    Device *device = pHwDevExt->device;
    PSTREAM_PROPERTY_DESCRIPTOR pSpd = pSrb->CommandData.PropertyInfo;
    ULONG Id  = pSpd->Property->Id;             // index of the property
    ULONG nS  = pSpd->PropertyOutputSize;       // size of data supplied

    pSrb->Status = STATUS_SUCCESS;

    ASSERT (device != NULL);
    switch (Id)
    {
        case KSPROPERTY_VPCONFIG_DDRAWHANDLE:

            if (pHwDevExt->ring3DirectDrawHandle == NULL)
            {
                pHwDevExt->ring3DirectDrawHandle =
                    (*(PULONG)pSrb->CommandData.PropertyInfo->PropertyInfo);
                
                if (!GetRing0DirectDrawHandle(pStrmEx))
                {
                    pSrb->Status = STATUS_UNSUCCESSFUL;
                    break;
                }
            }

            if (!pStrmEx->registered) {
                if (!RegisterForDirectDrawEvents(pStrmEx))
                {
                    pSrb->Status = STATUS_UNSUCCESSFUL;
                    break;
                }
            }
            break;

        case KSPROPERTY_VPCONFIG_VIDEOPORTID:

            if (pHwDevExt->ring3VideoPortHandle == -1)
            {
                pHwDevExt->ring3VideoPortHandle =
                    (*(PULONG)pSrb->CommandData.PropertyInfo->PropertyInfo);
                
                if (!GetRing0VideoPortHandle(pStrmEx))
                {
                    pSrb->Status = STATUS_UNSUCCESSFUL;
                    break;
                }
            }

            break;

        case KSPROPERTY_VPCONFIG_DDRAWSURFACEHANDLE:

#if 0
            // What is being passed here is essentially
            // a pointer to a structure which is a 'count' followed
            // by 'count' handles.

            PULONG pHandles;
            PULONG pUlong;

            // This should be non-null only after full-screen DOS, for example
            if (pHwDevExt->ring3VBISurfaceHandles)
            {
                ExFreePool(pHwDevExt->ring3VBISurfaceHandles);
                pHwDevExt->ring3VBISurfaceHandles = NULL;
                pHwDevExt->numRing3VBISurfaceHandles = 0;
            }

            pUlong = ((PULONG)pSrb->CommandData.PropertyInfo->PropertyInfo);

            if (*pUlong == 0)
            {
                DBGERROR(("Bt829: Didn't expect zero handles\n"));
                TRAP();
                break;
            }

            pHwDevExt->numRing3VBISurfaceHandles = *pUlong;
            pHwDevExt->ring3VBISurfaceHandles = (PULONG)ExAllocatePool(NonPagedPool, ((*pUlong) * sizeof(ULONG)));

            if (pHwDevExt->ring3VBISurfaceHandles == NULL)
            {
                DBGERROR(("Bt829: ExAllocatePool failed\n"));
                pSrb->Status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }

            pHandles = (pUlong + 1);

            RtlCopyMemory(
                         pHwDevExt->ring3VBISurfaceHandles,
                         pHandles,
                         ((*pUlong) * sizeof(HANDLE)));

#endif            
            pHwDevExt->preEventOccurred = pHwDevExt->postEventOccurred = FALSE;

            break;

        case KSPROPERTY_VPCONFIG_SETCONNECTINFO :
            {
                LPDDVIDEOPORTCONNECT pConnectInfo;

                pConnectInfo = ( LPDDVIDEOPORTCONNECT)  \
                               ( pSrb->CommandData.PropertyInfo->PropertyInfo);

                if ( pConnectInfo->dwPortWidth == 16)
                {
                    device->Set16BitDataStream(TRUE);
                }
                else
                {
                    device->Set16BitDataStream(FALSE);
                }
            } 
            break;

        case KSPROPERTY_VPCONFIG_INVERTPOLARITY :
            if ( *( PBOOL)( pSpd->PropertyInfo) == TRUE)
            {
                device->SetHighOdd(FALSE);
            }
            else
            {
                device->SetHighOdd(TRUE);
            }
            break;

        case KSPROPERTY_VPCONFIG_SETVIDEOFORMAT :
            //
            // pSrb->CommandData.PropertInfo->PropertyInfo
            // points to a ULONG which is an index into the array of
            // VIDEOFORMAT structs returned to the caller from the
            // Get call to FORMATINFO
            //
            // Since the sample only supports one FORMAT type right
            // now, we will ensure that the requested index is 0.
            //

            switch (*(PULONG)pSrb->CommandData.PropertyInfo->PropertyInfo)
            {
                case 0:

                    //
                    // at this point, we would program the hardware to use
                    // the right connection information for the videoport.
                    // since we are only supporting one connection, we don't
                    // need to do anything, so we will just indicate success
                    //

                    break;

                default:

                    pSrb->Status = STATUS_NO_MATCH;
                    break;
            }

            break;

        case KSPROPERTY_VPCONFIG_SCALEFACTOR :
            // TBD
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;

        case KSPROPERTY_VPCONFIG_SURFACEPARAMS :
            {
                PKSVPSURFACEPARAMS pSurfaceParams;

                ASSERT(nS >= sizeof( PKSVPSURFACEPARAMS));
                pSurfaceParams = (PKSVPSURFACEPARAMS)(pSrb->CommandData.PropertyInfo->PropertyInfo);

                DBGINFO(("Bt829: VP VBI Surface Params:\n"));
                DBGINFO(("Bt829: dwPitch    = %d\n",pSurfaceParams->dwPitch));
                DBGINFO(("Bt829: dwXOrigin  = %d\n",pSurfaceParams->dwXOrigin));
                DBGINFO(("Bt829: dwYOrigin  = %d\n",pSurfaceParams->dwYOrigin));

                device->VBISurfaceOriginX = pSurfaceParams->dwXOrigin;
                device->VBISurfaceOriginY = pSurfaceParams->dwYOrigin;
                device->VBISurfacePitch = pSurfaceParams->dwPitch;
            }
            break;

        default:
            TRAP();
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;
    }
}
