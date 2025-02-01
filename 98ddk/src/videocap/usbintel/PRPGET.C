/*++

Copyright (c) 1996, 1997  Intel Corporation

Module Name:

   prpget.c

Abstract:

   Code related to "getting"  about properties 

Author:


Environment:

   Kernel mode only


Revision History:

--*/

/*++
This Image Class Define...  Controls This Camera Feature...
==========================  ===============================

BRIGHTNESS                  BRIGHTNESS

--*/


#include "warn.h"
#include "wdm.h"

#include <strmini.h>
#include <ksmedia.h>

#include "usbdi.h"
#include "usbcamdi.h"

#include "intelcam.h"

//#include "prop.h"

#ifndef __PRPFTN_H__
#include "prpftn.h"
#endif

#ifndef __PRPOBJ_H__
#include "probj.h"
#endif



/*
** INTELCAM_GetCameraProperty()
**
** Arguments:
**
**  DeviceContext - driver context
**
** Returns:
**
**  NT status completion code 
**  
** Side Effects:  none
*/
NTSTATUS
INTELCAM_GetCameraProperty(
    PINTELCAM_DEVICE_CONTEXT pDeviceContext,
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
//    NTSTATUS ntStatus = STATUS_SUCCESS;

	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG PropertyID = pSPD->Property->Id;     // index of the property
    PKSPROPERTY_VIDEOPROCAMP_S pData =
        (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;  // pointer to data
    ULONG DefOrCurr = pData->Flags; 

    ASSERT(pSPD->PropertyOutputSize >= sizeof(KSPROPERTY_VIDEOPROCAMP_S));

    switch(PropertyID) {
        case  KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:
            return (pSrb->Status = GetPropertyCtrl(REQ_BRIGHTNESS,
                                   pDeviceContext,
                                   pSrb));

        default:
            return STATUS_NOT_SUPPORTED;
    }

}




/*
** GetPropertyCtrl()
**
** Arguments:
**
**  DeviceContext - driver context
**
** Returns:
**
**  NT status completion code 
**  
** Side Effects:  none
*/
NTSTATUS
GetPropertyCtrl(
    IN REQUEST ReqID,
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    PKSPROPERTY_VIDEOPROCAMP_S pData =
        (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;  // pointer to data
//    ULONG DefOrCurr = pData->Flags; 
    PVOID Value;
    BOOLEAN Status;

    ASSERT(pDC);
    ASSERT(pSrb);

    INTELCAM_KdPrint(MAX_TRACE, ("enter GetPropertyCtrl\n"));

    //
    // If necessary, one can branch here to get values (default...) other than
    // current values.    ?? check the srb flags anyway?
    //
    Status = GetPropertyCurrent(pDC, &Value, ReqID);
    FormPropertyData(pSrb, (PVOID)&Value, ReqID);
    if( Status == TRUE )
        return (pSrb->Status = STATUS_SUCCESS);
    else
        return (pSrb->Status = STATUS_NOT_SUPPORTED);
}



/*
** INTELCAM_GetVideoControlProperty()
**
** Arguments:
**
**  DeviceContext - driver context
**
** Returns:
**
**  NT status completion code 
**  
** Side Effects:  none
*/
NTSTATUS
INTELCAM_GetVideoControlProperty(
    PINTELCAM_DEVICE_CONTEXT pDeviceContext,
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo; 
    ULONG PropertyID = pSPD->Property->Id;     // index of the property
    ULONG nS  = pSPD->PropertyOutputSize;        // size of data supplied


    INTELCAM_KdPrint(MAX_TRACE, ("enter GetVideoControlProperty\n"));

    switch(PropertyID) {
 
    case  KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE:
    {
        PKSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S pData =
			(PKSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S) pSPD->PropertyInfo;  
		if (nS == 0)
		{
			pSrb->ActualBytesTransferred =
					sizeof(KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S);
			ntStatus = pSrb->Status = STATUS_BUFFER_OVERFLOW;
		}
		else if (nS >=
					sizeof(KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S))
		{
            if (pDeviceContext->StreamOpen == TRUE)
			{
				pSrb->ActualBytesTransferred =
					sizeof(KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S);
				pData-> CurrentActualFrameRate =
						pDeviceContext->CurrentProperty.Rate;
				pData->CurrentMaxAvailableFrameRate =
									NUM_100NANOSEC_UNITS_PERFRAME(20);
				pSrb->Status = STATUS_SUCCESS;
			}
			else
			{
				ntStatus = pSrb->Status = STATUS_NOT_SUPPORTED;
			}
		}
		else
		{
			ntStatus = pSrb->Status = STATUS_NOT_SUPPORTED;
		}

		break;
    }
    
    case  KSPROPERTY_VIDEOCONTROL_FRAME_RATES:
    {
		ULONG Nominator,Denominator, i,j;

		PKSPROPERTY_VIDEOCONTROL_FRAME_RATES_S pData =
			(PKSPROPERTY_VIDEOCONTROL_FRAME_RATES_S) pSPD->Property;
	
		if (nS == 0)
		{
			// 7 below is the number of frame rates supported in all cases
			pSrb->ActualBytesTransferred =
					sizeof(KSMULTIPLE_ITEM) + ((FRAME_RATE_LIST_SIZE -1) * sizeof(LONGLONG));
			ntStatus = pSrb->Status = STATUS_BUFFER_OVERFLOW;
		}
		else if (nS >=
					sizeof(KSMULTIPLE_ITEM) + ((FRAME_RATE_LIST_SIZE -1) * sizeof(LONGLONG))) {
			//
			// assuming this has the Supported Frame rates Struct
			//
			if (pData->Dimensions.cx == 160)
			{
                Nominator = 3;
                Denominator = 4;
			}
			else if (pData->Dimensions.cx == 320)
			{
                Nominator = 3;
                Denominator = 1;
			}
			else 
			{
                Nominator = 1;
                Denominator = 1;
			}
			
			{
				PKSMULTIPLE_ITEM pOutputBuf =
					(PKSMULTIPLE_ITEM) pSPD->PropertyInfo;
				LONGLONG * pDataPtr = (LONGLONG*) (pOutputBuf + 1);

				// Copy the input property info to the output property info
				pOutputBuf->Size =  (FRAME_RATE_LIST_SIZE -1) * sizeof(LONGLONG);
				pOutputBuf->Count = FRAME_RATE_LIST_SIZE - 1;

				for (i = 0, j=1; i < pOutputBuf->Count; i++,j++){
					pDataPtr[i] = SupportedFrameRates[j] * Nominator / Denominator;
				}

			}
			
			
			pSrb->ActualBytesTransferred =
					sizeof(KSMULTIPLE_ITEM) + ((FRAME_RATE_LIST_SIZE - 1) * sizeof(LONGLONG));

		    pSrb->Status = STATUS_SUCCESS;
		}
		else
		{
			ntStatus = pSrb->Status = STATUS_NOT_SUPPORTED;
		}
    }

    break;

    default:
            ntStatus = pSrb->Status = STATUS_NOT_SUPPORTED;
    }
    return ntStatus;
}



















