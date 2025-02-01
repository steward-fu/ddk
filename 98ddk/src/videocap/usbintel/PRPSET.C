/*++

Copyright (c) 1996, 1997  Intel Corporation

Module Name:

    prpset.c

Abstract:

    code to set various properties of the USB camera 

Author:


Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1996, 1997 Intel Corporation.  All Rights Reserved.


Revision History:

--*/
#include "warn.h"
#include "wdm.h"

#include <strmini.h>
#include <ksmedia.h>

#include "usbdi.h"
#include "usbcamdi.h"
#include "intelcam.h"
#include "prpftn.h"



/*
** INTELCAM_SetCameraProperty()
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
INTELCAM_SetCameraProperty(
    PINTELCAM_DEVICE_CONTEXT pDeviceContext,
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG PropertyID = pSPD->Property->Id;     // index of the property

    switch (PropertyID) {
        case  KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:
            return (pSrb->Status = SetPropertyCtrl(REQ_BRIGHTNESS,
                                   pDeviceContext,
                                   pSrb));
                
        default:
            TRAP();
            return STATUS_NOT_SUPPORTED;
    }

}




/*
** SetPropertyCtrl()
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
SetPropertyCtrl(
    IN REQUEST ReqID,
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
	PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    PKSPROPERTY_VIDEOPROCAMP_S pData =
        (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;  // pointer to data
//    ULONG DefOrCurr = pData->Flags; 

    USHORT RegAddr;
    NTSTATUS Status = STATUS_NOT_SUPPORTED;
    ULONG Length;
    ULONG Value;

    ASSERT(pDC);
    ASSERT(pSrb);
    ASSERT(pSPD);


    //
    // If necessary, one can branch here to set values (default...) other than
    // current values.  ?? check the srb flags anyway?
    //
    Value = pData->Value; 


    Length = 0;
    switch (ReqID)
    {
        case REQ_BRIGHTNESS:
            RegAddr = INDEX_PREF_BRIGHTNESS;
            Value /= STEPPING_DELTA_BRIGHTNESS; // map to camera 
            break;

        default:
            INTELCAM_KdPrint(MAX_TRACE,
                             ("SetPropertyCtrl: Invalid Property\n"));
            Status = STATUS_NOT_SUPPORTED;
            goto SetCtrlDone;
   }


    //
    // Set value...
    //
    Status = USBCAMD_ControlVendorCommand(pDC,
                                          REQUEST_SET_PREFERENCE,
                                          (USHORT)Value,
                                          RegAddr,
                                          NULL,
                                          &Length,
                                          FALSE,
										  NULL,
										  NULL);

    if(!NT_SUCCESS(Status))
    {
        INTELCAM_KdPrint(MIN_TRACE, ("ERROR: Setting property <0x%x>",
		                                                       RegAddr));
        goto SetCtrlDone;
    }


    //
    // Update current values cached in DC
    //
    SetPropertyCurrent(pDC, &Value, ReqID);

SetCtrlDone:
    return Status;
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
INTELCAM_SetVideoControlProperty(
    PINTELCAM_DEVICE_CONTEXT pDeviceContext,
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    return pSrb->Status = STATUS_NOT_SUPPORTED;
}
