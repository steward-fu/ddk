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
}
#endif

#include "defaults.h"
#include "capmain.h"
#include "capdebug.h"

#include "device.h"

// -------------------------------------------------------------------
// XBar Property Set functions
// -------------------------------------------------------------------

//
// The only property to set on the XBar selects the input to use
//

/* Method: AdapterGetCrossbarProperty
 * Purpose:
 */
VOID AdapterSetCrossbarProperty( PHW_STREAM_REQUEST_BLOCK pSrb )
{
   PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
   PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
   Device *device = pHwDevExt->device;
   ULONG Id  = pSPD->Property->Id;              // index of the property
   ULONG nS  = pSPD->PropertyOutputSize;        // size of data supplied

   pSrb->Status = STATUS_SUCCESS;

   ASSERT(device != NULL);

   switch ( Id ) {
   case KSPROPERTY_CROSSBAR_ROUTE:
      {
         PKSPROPERTY_CROSSBAR_ROUTE_S  pRoute =
            (PKSPROPERTY_CROSSBAR_ROUTE_S)pSPD->PropertyInfo;

         ASSERT (nS >= sizeof (KSPROPERTY_CROSSBAR_ROUTE_S));

         // Copy the input property info to the output property info
         RtlCopyMemory( pRoute, pSPD->Property,
           sizeof KSPROPERTY_CROSSBAR_ROUTE_S );

         int InPin, OutPin;
         InPin  = pRoute->IndexInputPin;
         OutPin = pRoute->IndexOutputPin;

         if ( device->GoodPins( InPin, OutPin ) ) {

            if ( device->TestRoute( InPin, OutPin ) ) {
               pRoute->CanRoute = TRUE;
               device->SetVideoInput( (Connector) (InPin + 1) ); // our connectors are 1-based

               // this just sets the association
               device->Route( OutPin, InPin );
            } else
               pRoute->CanRoute = FALSE;
         } else
            pRoute->CanRoute = 0;
      }
      pSrb->ActualBytesTransferred = sizeof KSPROPERTY_CROSSBAR_ROUTE_S;
      break;
   default:
      TRAP();
      pSrb->Status = STATUS_NOT_IMPLEMENTED;
      pSrb->ActualBytesTransferred = 0;
      break;
   }
}

/* Method: AdapterGetCrossbarProperty
 * Purpose:
 */
VOID AdapterGetCrossbarProperty( PHW_STREAM_REQUEST_BLOCK pSrb )
{
   PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
   PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
   Device *device = pHwDevExt->device;
   ULONG Id = pSPD->Property->Id;              // index of the property
   PLONG pL = (PLONG) pSPD->PropertyInfo;     // pointer to the data
   ULONG nS = pSPD->PropertyOutputSize;        // size of data supplied

   pSrb->Status = STATUS_SUCCESS;

   ASSERT (device != NULL);

   // Property set specific structure

   switch ( Id ) {
   case KSPROPERTY_CROSSBAR_CAPS:                  // R
      if ( nS >= sizeof KSPROPERTY_CROSSBAR_CAPS_S ) {

         PKSPROPERTY_CROSSBAR_CAPS_S  pCaps =
            (PKSPROPERTY_CROSSBAR_CAPS_S)pSPD->PropertyInfo;

         // Copy the input property info to the output property info
         RtlCopyMemory( pCaps, pSPD->Property,
            sizeof KSPROPERTY_CROSSBAR_CAPS_S );

         pCaps->NumberOfInputs  = device->GetNoInputs();
         pCaps->NumberOfOutputs = device->GetNoOutputs();

         pSrb->ActualBytesTransferred = sizeof KSPROPERTY_CROSSBAR_CAPS_S;
      }
      break;
   case KSPROPERTY_CROSSBAR_CAN_ROUTE:
      if ( nS >= sizeof KSPROPERTY_CROSSBAR_ROUTE_S ) {

         PKSPROPERTY_CROSSBAR_ROUTE_S  pRoute =
            (PKSPROPERTY_CROSSBAR_ROUTE_S)pSPD->PropertyInfo;

         // Copy the input property info to the output property info
         RtlCopyMemory( pRoute, pSPD->Property,
            sizeof KSPROPERTY_CROSSBAR_ROUTE_S );

         int InPin, OutPin;
         InPin  = pRoute->IndexInputPin;
         OutPin = pRoute->IndexOutputPin;

         if ( device->GoodPins( InPin, OutPin ) ) {
            pRoute->CanRoute = device->TestRoute( InPin, OutPin );
         } else {
            pRoute->CanRoute = FALSE;
         }
         pSrb->ActualBytesTransferred = sizeof KSPROPERTY_CROSSBAR_ROUTE_S;
      }
      break;
   case KSPROPERTY_CROSSBAR_ROUTE:
      if ( nS >= sizeof KSPROPERTY_CROSSBAR_ROUTE_S ) {

         PKSPROPERTY_CROSSBAR_ROUTE_S  pRoute =
            (PKSPROPERTY_CROSSBAR_ROUTE_S)pSPD->PropertyInfo;

         // Copy the input property info to the output property info
         RtlCopyMemory( pRoute, pSPD->Property,
            sizeof KSPROPERTY_CROSSBAR_ROUTE_S );

         int OutPin = pRoute->IndexOutputPin;

         if ( OutPin < device->GetNoOutputs() )
            pRoute->IndexInputPin = device->GetRoute( OutPin );
         else
            pRoute->IndexInputPin = -1;

         pSrb->ActualBytesTransferred = sizeof KSPROPERTY_CROSSBAR_ROUTE_S;
      }
      break;
   case KSPROPERTY_CROSSBAR_PININFO:                     // R
      if ( nS >= sizeof KSPROPERTY_CROSSBAR_PININFO_S ) {

         PKSPROPERTY_CROSSBAR_PININFO_S  pPinInfo =
            (PKSPROPERTY_CROSSBAR_PININFO_S)pSPD->PropertyInfo;

         // Copy the input property info to the output property info
         RtlCopyMemory( pPinInfo, pSPD->Property,
            sizeof KSPROPERTY_CROSSBAR_PININFO_S );

         pPinInfo->PinType = device->GetPinInfo( pPinInfo->Direction,
                pPinInfo->Index, 
                pPinInfo->RelatedPinIndex );

         pPinInfo->Medium = * device->GetPinMedium( pPinInfo->Direction,
                pPinInfo->Index);

         pSrb->ActualBytesTransferred = sizeof KSPROPERTY_CROSSBAR_PININFO_S;
      }
   break;
   default:
       TRAP();
       pSrb->Status = STATUS_NOT_IMPLEMENTED;
       pSrb->ActualBytesTransferred = 0;
       break;
   }
}

// -------------------------------------------------------------------
// VideoProcAmp functions
// -------------------------------------------------------------------

VOID AdapterSetVideoProcAmpProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    Device *device = pHwDevExt->device;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PKSPROPERTY_VIDEOPROCAMP_S pS = (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;    // pointer to the data

    pSrb->Status = STATUS_SUCCESS;

    ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOPROCAMP_S));
    ASSERT (device != NULL);

    switch (Id) {

    case KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:

        device->SetBrightness(pS->Value);
        break;
        
    case KSPROPERTY_VIDEOPROCAMP_CONTRAST:

        device->SetContrast(pS->Value);
        break;

    case KSPROPERTY_VIDEOPROCAMP_HUE:

        device->SetHue(pS->Value);
        break;

    case KSPROPERTY_VIDEOPROCAMP_SATURATION:

        device->SetSaturation(pS->Value);
        break;

    default:
        TRAP();
        pSrb->Status = STATUS_NOT_IMPLEMENTED;
        break;
    }
}

VOID AdapterGetVideoProcAmpProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    Device *device = pHwDevExt->device;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PKSPROPERTY_VIDEOPROCAMP_S pS = (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;    // pointer to the data

    pSrb->Status = STATUS_SUCCESS;

    ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOPROCAMP_S));
    ASSERT (device != NULL);

    switch (Id) {

    case KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:
        pS->Value = device->GetBrightness();
		  pS->Flags = pS->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOPROCAMP_S);
        break;
        
    case KSPROPERTY_VIDEOPROCAMP_CONTRAST:
        pS->Value = device->GetContrast();
		  pS->Flags = pS->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOPROCAMP_S);
        break;

    case KSPROPERTY_VIDEOPROCAMP_HUE:
        pS->Value = device->GetHue();
		  pS->Flags = pS->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOPROCAMP_S);
        break;

    case KSPROPERTY_VIDEOPROCAMP_SATURATION:
        pS->Value = device->GetSaturation();
		  pS->Flags = pS->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOPROCAMP_S);
        break;

    default:
        TRAP();
        pSrb->ActualBytesTransferred = 0;
        pSrb->Status = STATUS_NOT_IMPLEMENTED;
        break;
    }
}

// -------------------------------------------------------------------
// AnalogVideoDecoder functions
// -------------------------------------------------------------------

/*
** AdapterSetAnalogVideoDecoderProperty ()
**
**    Handles Set operations on the AnalogVideoDecoder property set.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID AdapterSetAnalogVideoDecoderProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    Device                  *device = pHwDevExt->device;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;

    ASSERT (pSPD->PropertyInputSize >= sizeof (PKSPROPERTY_VIDEODECODER_S));

    pSrb->Status = STATUS_SUCCESS;

    switch (Id) {

        case KSPROPERTY_VIDEODECODER_STANDARD:
            DBGTRACE(("'Bt829: KSPROPERTY_VIDEODECODER_STANDARD.\n"));

            if (pS->Value != KS_AnalogVideo_NTSC_M)
            {
                DBGERROR(("Bt829: Unsupported video standard.\n"));
                pSrb->Status = STATUS_UNSUCCESSFUL;
            }
            break;

        case KSPROPERTY_VIDEODECODER_OUTPUT_ENABLE:
            DBGTRACE(("'Bt829: KSPROPERTY_VIDEODECODER_OUTPUT_ENABLE.\n"));

            device->SetOutputEnabled(pS->Value ? TRUE : FALSE);
            break;

        default:
            TRAP();
            break;
    }
}

/*
** AdapterGetAnalogVideoDecoderProperty ()
**
**    Handles Get operations on the AnalogVideoDecoder property set.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID AdapterGetAnalogVideoDecoderProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    Device                  *device = pHwDevExt->device;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id) {

        case KSPROPERTY_VIDEODECODER_CAPS:
        {
            DBGTRACE(("'Bt829: KSPROPERTY_VIDEODECODER_CAPS\n"));

            PKSPROPERTY_VIDEODECODER_CAPS_S pS = (PKSPROPERTY_VIDEODECODER_CAPS_S) pSPD->PropertyInfo;    // pointer to the data

            ASSERT (pSPD->PropertyOutputSize >= sizeof (PKSPROPERTY_VIDEODECODER_CAPS_S));
        
            pS->StandardsSupported = KS_AnalogVideo_NTSC_M;

            pS->Capabilities = KS_VIDEODECODER_FLAGS_CAN_DISABLE_OUTPUT  |
                               KS_VIDEODECODER_FLAGS_CAN_INDICATE_LOCKED ;

            // How long (ms) til locked indicator is valid.
            // 31 line periods * 63.5uS per line.
            pS->SettlingTime = 2;       

            // BUGBUG. Not sure about this
            // HSync per VSync
            pS->HSyncPerVSync = 6;

            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_CAPS_S);
        }
        break;
        
        case KSPROPERTY_VIDEODECODER_STANDARD:
        {
            DBGTRACE(("'Bt829: KSPROPERTY_VIDEODECODER_STANDARD\n"));

            PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;    // pointer to the data

            ASSERT (pSPD->PropertyOutputSize >= sizeof (PKSPROPERTY_VIDEODECODER_S));

            pS->Value = KS_AnalogVideo_NTSC_M;

            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
        }
        break;

        case KSPROPERTY_VIDEODECODER_STATUS:
        {
            DBGTRACE(("'Bt829: KSPROPERTY_VIDEODECODER_STATUS\n"));

            PKSPROPERTY_VIDEODECODER_STATUS_S pS = (PKSPROPERTY_VIDEODECODER_STATUS_S) pSPD->PropertyInfo;    // pointer to the data

            ASSERT (pSPD->PropertyOutputSize >= sizeof (PKSPROPERTY_VIDEODECODER_STATUS_S));

            pS->NumberOfLines = device->Is525LinesVideo() ? 525 : 625;

            pS->SignalLocked = device->IsDeviceInHLock();

            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
        }
        break;

        case KSPROPERTY_VIDEODECODER_OUTPUT_ENABLE:
        {
            DBGTRACE(("'Bt829: KSPROPERTY_VIDEODECODER_OUTPUT_ENABLE\n"));

            PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;    // pointer to the data

            ASSERT (pSPD->PropertyOutputSize >= sizeof (PKSPROPERTY_VIDEODECODER_S));

            pS->Value = device->IsOutputEnabled();

            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
        }
        break;

        default:
            TRAP();
            break;
    }
}

// -------------------------------------------------------------------
// General entry point for all get/set adapter properties
// -------------------------------------------------------------------

VOID AdapterSetProperty (PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    if (IsEqualGUID(PROPSETID_VIDCAP_CROSSBAR, pSPD->Property->Set)) {
        AdapterSetCrossbarProperty (pSrb);
    }
    else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEOPROCAMP, pSPD->Property->Set)) {
        AdapterSetVideoProcAmpProperty (pSrb);
    }
    else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEODECODER, pSPD->Property->Set)) {
        AdapterSetAnalogVideoDecoderProperty (pSrb);
    }
}

VOID AdapterGetProperty (PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    if (IsEqualGUID (PROPSETID_VIDCAP_CROSSBAR, pSPD->Property->Set)) {
        AdapterGetCrossbarProperty (pSrb);
    }
    else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEOPROCAMP, pSPD->Property->Set)) {
        AdapterGetVideoProcAmpProperty (pSrb);
    }
    else if (IsEqualGUID(PROPSETID_VIDCAP_VIDEODECODER, pSPD->Property->Set)) {
        AdapterGetAnalogVideoDecoderProperty (pSrb);
    }
}

