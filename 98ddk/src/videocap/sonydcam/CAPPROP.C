/*++

Copyright (c) 1996 Microsoft Corporation

Module Name:

    CapProp.c

Abstract:

    This is a driver for the Sony Desktop Camera (CCM-DS250 V1.0x).
    This file contains code to handle the video and camera control properties.

Author:
    
    Yee J. Wu  9-Sep-97

Environment:

    Kernel mode only

Revision History:


--*/

#define ValidValueRange(Range,Value) (Value <= Range.SignedMaximum && Value >= Range.SignedMinimum)

NTSTATUS
SonyDCamReadRegisterAbsolute(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension,
	ULONG ulLowAddress,
	ULONG * pulValue
)
{
	NTSTATUS status;
    LARGE_INTEGER deltaTime;
	PIRP Irp;


    //
    // Delay for camera before next request
    //
    SONYDCAM_ASSERT(DeviceExtension->BusDeviceObject != NULL);

    Irp = IoAllocateIrp(DeviceExtension->BusDeviceObject->StackSize, FALSE);

    if (!Irp) {

        SONYDCAM_ASSERT(FALSE);
        return (STATUS_INSUFFICIENT_RESOURCES);

    }

    deltaTime.LowPart = SONYDCAM_DELAY_VALUE;
    deltaTime.HighPart = -1;
    KeDelayExecutionThread(KernelMode, TRUE, &deltaTime);

    Irb->FunctionNumber = REQUEST_ASYNC_READ;
    Irb->Flags = 0;

    Irb->u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_High = INITIAL_REGISTER_SPACE_HI;
	Irb->u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_Low =  ulLowAddress;

	Irb->u.AsyncRead.nNumberOfBytesToRead = sizeof(ULONG);
	Irb->u.AsyncRead.nBlockSize = 0;
	Irb->u.AsyncRead.fulFlags = 0;
	Irb->u.AsyncRead.ulGeneration = DeviceExtension->CurrentGeneration;
	DeviceExtension->RegisterWorkArea.AsULONG = 0;    // Initilize the return buffer.
	Irb->u.AsyncRead.Mdl = MmCreateMdl(NULL, &DeviceExtension->RegisterWorkArea, sizeof(ULONG));
    MmBuildMdlForNonPagedPool(Irb->u.AsyncRead.Mdl);

	DbgMsg3(("\'SonyDCamReadRegisterAbsolute: Read from address (%x, %x)\n",
		Irb->u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_High,
		Irb->u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_Low));	
   
   status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

    if (status) {

        DbgMsg1(("\'SonyDCamReadRegisterAbsolute: Error %x while trying to read from register\n", status));       		
    } else {

		*pulValue = DeviceExtension->RegisterWorkArea.AsULONG;
        DbgMsg1(("\'SonyDCamReadRegisterAbsolute: status=0x%x, value=0x%x\n", status, *pulValue));       		
	}
	
	ExFreePool(Irb->u.AsyncRead.Mdl);

	IoFreeIrp(Irp);

	return status;
}

NTSTATUS
SonyDCamReadRegister(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension,
	ULONG ulFieldOffset,
	ULONG * pulValue
)
{
	NTSTATUS status;
    LARGE_INTEGER deltaTime;
	PIRP Irp;


    //
    // Delay for camera before next request
    //
    SONYDCAM_ASSERT(DeviceExtension->BusDeviceObject != NULL);

    Irp = IoAllocateIrp(DeviceExtension->BusDeviceObject->StackSize, FALSE);

    if (!Irp) {

        SONYDCAM_ASSERT(FALSE);
        return (STATUS_INSUFFICIENT_RESOURCES);

    }

    deltaTime.LowPart = SONYDCAM_DELAY_VALUE;
    deltaTime.HighPart = -1;
    KeDelayExecutionThread(KernelMode, TRUE, &deltaTime);

    Irb->FunctionNumber = REQUEST_ASYNC_READ;
    Irb->Flags = 0;

    Irb->u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_High = 
			INITIAL_REGISTER_SPACE_HI;

	Irb->u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_Low = 		
			DeviceExtension->BaseRegister + ulFieldOffset;

	Irb->u.AsyncRead.nNumberOfBytesToRead = sizeof(ULONG);
	Irb->u.AsyncRead.nBlockSize = 0;
	Irb->u.AsyncRead.fulFlags = 0;
	Irb->u.AsyncRead.ulGeneration = DeviceExtension->CurrentGeneration;
	DeviceExtension->RegisterWorkArea.AsULONG = 0;    // Initilize the return buffer.
	Irb->u.AsyncRead.Mdl = MmCreateMdl(NULL, &DeviceExtension->RegisterWorkArea, sizeof(ULONG));
    MmBuildMdlForNonPagedPool(Irb->u.AsyncRead.Mdl);

	DbgMsg3(("\'SonyDCamReadRegister: Read from address (%x, %x)\n",
		Irb->u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_High,
		Irb->u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_Low));	

	status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

    if (status) {

        DbgMsg1(("\'SonyDCamRange: Error %x while trying to read from register\n", status));       		
    } else {

		*pulValue = DeviceExtension->RegisterWorkArea.AsULONG;
        DbgMsg1(("\'SonyDCamReadRegister: status=0x%x, value=0x%x\n", status, *pulValue));       		
	}
	
	ExFreePool(Irb->u.AsyncRead.Mdl);

	IoFreeIrp(Irp);

	return status;
}


NTSTATUS
SonyDCamWriteRegister(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension,
	ULONG ulFieldOffset,
	ULONG ulValue
)
{
	NTSTATUS status;
    LARGE_INTEGER deltaTime;
	PIRP Irp;

    SONYDCAM_ASSERT(DeviceExtension->BusDeviceObject != NULL);
    Irp = IoAllocateIrp(DeviceExtension->BusDeviceObject->StackSize, FALSE);

    if (!Irp) {

        SONYDCAM_ASSERT(FALSE);
        return (STATUS_INSUFFICIENT_RESOURCES);

    }

    //
    // Delay for camera before next request
    //
    deltaTime.LowPart = SONYDCAM_DELAY_VALUE;
    deltaTime.HighPart = -1;
    KeDelayExecutionThread(KernelMode, TRUE, &deltaTime);

    Irb->FunctionNumber = REQUEST_ASYNC_WRITE;
    Irb->Flags = 0;

    Irb->u.AsyncWrite.DestinationAddress.IA_Destination_Offset.Off_High = 
			INITIAL_REGISTER_SPACE_HI;

	Irb->u.AsyncWrite.DestinationAddress.IA_Destination_Offset.Off_Low = 		
			DeviceExtension->BaseRegister + ulFieldOffset;

	Irb->u.AsyncWrite.nNumberOfBytesToWrite = sizeof(ULONG);
	Irb->u.AsyncWrite.nBlockSize = 0;
	Irb->u.AsyncWrite.fulFlags = 0;
	Irb->u.AsyncWrite.ulGeneration = DeviceExtension->CurrentGeneration;
	DeviceExtension->RegisterWorkArea.AsULONG = ulValue;    // Initilize the return buffer.
	Irb->u.AsyncWrite.Mdl = MmCreateMdl(NULL, &DeviceExtension->RegisterWorkArea, sizeof(ULONG));
    MmBuildMdlForNonPagedPool(Irb->u.AsyncWrite.Mdl);
	
    DbgMsg3(("\'SonyDCamWriteRegister: Read from address (%x, %x)\n",
		Irb->u.AsyncWrite.DestinationAddress.IA_Destination_Offset.Off_High,
		Irb->u.AsyncWrite.DestinationAddress.IA_Destination_Offset.Off_Low));	

	status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

    if (status) {

        DbgMsg1(("\'SonyDCamRange: Error %x while trying to write to register\n", status));       		
    } 
	
	ExFreePool(Irb->u.AsyncWrite.Mdl);
	IoFreeIrp(Irp);
	return status;
}


NTSTATUS
SonyDCamGetProperty(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension, 
	ULONG ulFieldOffset,
	LONG * plValue,
	ULONG * pulCapability,
	ULONG * pulFlags
)
{

	NTSTATUS status;
	DCamRegArea RegArea;


	// location of control regisgter - feature = 0x300 (== 768)
	status = SonyDCamReadRegister(Irb, DeviceExtension, ulFieldOffset - 768, &(RegArea.AsULONG));
    if (!status) {    

		RegArea.AsULONG = bswap(RegArea.AsULONG);

		if (RegArea.Feature.PresenceInq == 1) {

			*pulCapability = 0;  
			if (RegArea.Feature.AutoMode)
				*pulCapability |= KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;  // == KSPROPERTY_CAMERACONTROL_FLAGS_AUTO

			if (RegArea.Feature.ManualMode)
				*pulCapability |= KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;

			status = SonyDCamReadRegister(Irb, DeviceExtension, ulFieldOffset, &(RegArea.AsULONG));
			if (!status) {

				RegArea.AsULONG = bswap(RegArea.AsULONG);
				*plValue = (LONG) RegArea.Brightness.Value;

				// These only valid if it has these capabilities.
				if (RegArea.Brightness.AutoMode)
					*pulFlags = KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
				else 
					*pulFlags = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
				
            DbgMsg3(("\'SonyDCamGetProperty: 0x%x: PresenceIng=%d, ONOff=%d, AutoMode=%d, Value=%d\n", 
					RegArea.AsULONG, 
					RegArea.Brightness.PresenceInq,			
					RegArea.Brightness.OnOff,
					RegArea.Brightness.AutoMode,
					RegArea.Brightness.Value
					));

			}

			DbgMsg3(("\'SonyDCamGetProperty: Currrent value is %d and is %s mode.\n", *plValue, RegArea.Brightness.AutoMode ? "AUTO" : "MANUAL"));

		} else {

		   DbgMsg2(("\'SonyDCamGetProperty: PresenceInq not set, feature not supported.\n"));
			return STATUS_NOT_IMPLEMENTED;

		}
	}

	return status;
}



NTSTATUS
SonyDCamSetProperty(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension, 
	ULONG ulFieldOffset,
	ULONG ulFlags,
	LONG  lValue
)
{
	NTSTATUS status;
	DCamRegArea RegArea;

	status = SonyDCamReadRegister(Irb, DeviceExtension, ulFieldOffset, &(RegArea.AsULONG));
    if (!status) {    

		RegArea.AsULONG = bswap(RegArea.AsULONG);
		
      DbgMsg3(("\'SonyDCamSetProperty: 0x%x: PresenceIng=%d, ONOff=%d, AutoMode=%d, Value=%d\n", 
			RegArea.AsULONG, 
			RegArea.Brightness.PresenceInq,			
			RegArea.Brightness.OnOff,
			RegArea.Brightness.AutoMode,
			RegArea.Brightness.Value
			));


		// Feature supported and in Manual mode
		if (RegArea.Brightness.PresenceInq == 1) {

			if (ulFlags & KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO) {
				// if not already in auto mode, set it so.
				if (RegArea.Brightness.AutoMode == 0)
					status = SonyDCamSetAutoMode( Irb,DeviceExtension, ulFieldOffset, TRUE);

			} else {

				// if auto mode, set to manual mode
				if (RegArea.Brightness.AutoMode == 1) 
					status = SonyDCamSetAutoMode( Irb,DeviceExtension, ulFieldOffset, FALSE);
				else {

					// special case for white balance
					if (FIELDOFFSET(CAMERA_REGISTER_MAP, WhiteBalance) == ulFieldOffset) 
						RegArea.WhiteBalance.UValue = RegArea.WhiteBalance.VValue = lValue;
					else 
						RegArea.Brightness.Value = lValue;
				

					RegArea.AsULONG = bswap(RegArea.AsULONG);

					status = SonyDCamWriteRegister(Irb, DeviceExtension, ulFieldOffset, RegArea.AsULONG);

					if (status) { 

					DbgMsg1(("\'SonyDCamGetProperty: failed with status=0x%x\n", status));
					}						
				}
			}
		} else {

			DbgMsg2(("\'SonyDCamSetProperty: not available\n"));

			return STATUS_NOT_IMPLEMENTED;
		}
	}

	return status;

}



NTSTATUS
SonyDCamSetAutoMode(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension, 
	ULONG ulFieldOffset,
	BOOL bAutoMode
)
{
	NTSTATUS status;
	DCamRegArea RegArea;

	status = SonyDCamReadRegister(Irb, DeviceExtension, ulFieldOffset, &(RegArea.AsULONG));
    if (!status) {    

		RegArea.AsULONG = bswap(RegArea.AsULONG);

		DbgMsg3(("\'SonyDCamSetAutoMode: 0x%x: PresenceIng=%d, ONOff=%d, AutoMode=%d, Value=%d\n", 
			RegArea.AsULONG, 
			RegArea.Brightness.PresenceInq,			
			RegArea.Brightness.OnOff,
			RegArea.Brightness.AutoMode,
			RegArea.Brightness.Value
			));

		// Feature supported and in Manual mode
		if (RegArea.Brightness.PresenceInq == 1) {

			RegArea.Brightness.AutoMode = bAutoMode ? 1 : 0;

			RegArea.AsULONG = bswap(RegArea.AsULONG);

			status = SonyDCamWriteRegister(Irb, DeviceExtension, ulFieldOffset, RegArea.AsULONG);

			if (status) { 

				DbgMsg2(("\'SonyDCamSetAutoMode: failed with status=0x%x\n", status));

			}
		} else {

			DbgMsg2(("\'SonyDCamSetAutoMode: not available !\n"));

			return STATUS_NOT_IMPLEMENTED;
		}
	}

	return status;

}


NTSTATUS
SonyDCamGetRange(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension,
	ULONG ulFieldOffset,
	LONG * pMinValue,
	LONG * pMaxValue
)
{
	NTSTATUS status;
	DCamRegArea RegArea;


	status = SonyDCamReadRegister(Irb, DeviceExtension, ulFieldOffset, &(RegArea.AsULONG));

    if (!status) {    
		
		RegArea.AsULONG = bswap(RegArea.AsULONG);

		DbgMsg3(("\'SonyDCamGetRange: 0x%x: PresenceIng=%d, ReadOut=%d, ONOff=%d, AutoMode=%d, Manual=%d, min=%d, max=%d\n", 
			RegArea.AsULONG, 
			RegArea.Feature.PresenceInq,			
			RegArea.Feature.ReadOut_Inq,
			RegArea.Feature.OnOff,
			RegArea.Feature.AutoMode,
			RegArea.Feature.ManualMode,
			RegArea.Feature.MIN_Value,
			RegArea.Feature.MAX_Value
			));

		if (RegArea.Feature.PresenceInq == 1) {

			*pMinValue = RegArea.Feature.MIN_Value;		
			*pMaxValue = RegArea.Feature.MAX_Value; 
 
			DbgMsg3(("\'SonyDCamGetRange: status=0x%x, (min=%d, max=%d)\n", status, *pMinValue, *pMaxValue));

			// Set to Manual Mode
			if (RegArea.Feature.AutoMode == 1) {
				SonyDCamSetAutoMode(Irb, DeviceExtension, ulFieldOffset, FALSE);
			}

		} else {	    
		    DbgMsg2(("\'SonyDCamFeature: PresenceInq not set, feature not supported.\n"));
			*pMinValue = *pMinValue = 0;

			return STATUS_NOT_IMPLEMENTED;
		}
	}

	return status;
	
}


/*
** AdapterGetVideoProcAmpProperty ()
**
**    Handles Set operations on the VideoProcAmp property set.
**      Testcap uses this for demo purposes only.
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

VOID 
AdapterGetVideoProcAmpProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
	NTSTATUS status;

    PSONYDCAM_EXTENSION 
		DeviceExtension = (PSONYDCAM_EXTENSION) pSrb->HwDeviceExtension;

    PSTREAM_PROPERTY_DESCRIPTOR 
		pSPD = pSrb->CommandData.PropertyInfo;

    PKSPROPERTY_VIDEOPROCAMP_S 
		pS = (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;    // pointer to the data

    ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOPROCAMP_S));   

    switch (pSPD->Property->Id) {

    case KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:		
		status = SonyDCamGetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Brightness), &pS->Value, &pS->Capabilities, &pS->Flags);
		break;


    case KSPROPERTY_VIDEOPROCAMP_SHARPNESS:		
		status = SonyDCamGetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Sharpness), &pS->Value, &pS->Capabilities, &pS->Flags);
		break;

    case KSPROPERTY_VIDEOPROCAMP_HUE:		
		status = SonyDCamGetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Hue), &pS->Value, &pS->Capabilities, &pS->Flags); 
        break;
        
    case KSPROPERTY_VIDEOPROCAMP_SATURATION:
		status = SonyDCamGetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Saturation), &pS->Value, &pS->Capabilities, &pS->Flags);
        break;

    case KSPROPERTY_VIDEOPROCAMP_WHITEBALANCE:
		status = SonyDCamGetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, WhiteBalance), &pS->Value, &pS->Capabilities, &pS->Flags);
        break;


    default:
	    DbgMsg2(("\'AdapterGetVideoProcAmpProperty, Id (%x)not supported.\n", pSPD->Property->Id));
        TRAP();
		status = STATUS_NOT_IMPLEMENTED;
        break;
    }

	pSrb->Status = status;
    pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOPROCAMP_S);

}

/*
** AdapterGetCameraControlProperty ()
**
**    Handles Set operations on the VideoProcAmp property set.
**      Testcap uses this for demo purposes only.
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

VOID 
AdapterGetCameraControlProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
	NTSTATUS status;

    PSONYDCAM_EXTENSION 
		DeviceExtension = (PSONYDCAM_EXTENSION) pSrb->HwDeviceExtension;

    PSTREAM_PROPERTY_DESCRIPTOR 
		pSPD = pSrb->CommandData.PropertyInfo;

    PKSPROPERTY_CAMERACONTROL_S 
		pS = (PKSPROPERTY_CAMERACONTROL_S) pSPD->PropertyInfo;    // pointer to the data

    ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_CAMERACONTROL_S));

    switch (pSPD->Property->Id) {

    case KSPROPERTY_CAMERACONTROL_FOCUS:

		status = SonyDCamGetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Focus), &pS->Value, &pS->Capabilities, &pS->Flags);
        break;       

    case KSPROPERTY_CAMERACONTROL_ZOOM:

		status = SonyDCamGetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Zoom), &pS->Value, &pS->Capabilities, &pS->Flags);
        break;       

    default:	    
		DbgMsg2(("\'AdapterGetCameraControlProperty, Id (%x)not supported.\n", pSPD->Property->Id));
        TRAP();
		status = STATUS_NOT_IMPLEMENTED;		
        break;
    }

	pSrb->Status = status;
    pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CAMERACONTROL_S);

}


/*
** AdapterGetProperty ()
**
**    Handles Get operations for all adapter properties.
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

VOID
STREAMAPI 
AdapterGetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )

{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

	if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEOPROCAMP, &pSPD->Property->Set)) {
        AdapterGetVideoProcAmpProperty (pSrb);
    } else 	if (IsEqualGUID(&PROPSETID_VIDCAP_CAMERACONTROL, &pSPD->Property->Set)) {
        AdapterGetCameraControlProperty (pSrb);
    } else {
        //
        // We should never get here
        //

        TRAP();
    }
}

/*
** AdapterSetVideoProcAmpProperty ()
**
**    Handles Set operations on the VideoProcAmp property set.
**      Testcap uses this for demo purposes only.
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

VOID 
AdapterSetVideoProcAmpProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
	NTSTATUS status;

    PSONYDCAM_EXTENSION 
		DeviceExtension = (PSONYDCAM_EXTENSION) pSrb->HwDeviceExtension;

    PSTREAM_PROPERTY_DESCRIPTOR 
		pSPD = pSrb->CommandData.PropertyInfo;

    PKSPROPERTY_VIDEOPROCAMP_S 
		pS = (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;    // pointer to the data

    ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOPROCAMP_S));    

    switch (pSPD->Property->Id) {

    case KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:				

		if (ValidValueRange(DeviceExtension->BrightnessRange,pS->Value)) 
			status = SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Brightness), pS->Flags, pS->Value);
		else 
			status = STATUS_INVALID_PARAMETER;
        break;
        
    case KSPROPERTY_VIDEOPROCAMP_SHARPNESS:

		if (ValidValueRange(DeviceExtension->SharpnessRange,pS->Value)) 
			status = SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Sharpness), pS->Flags, pS->Value);       
 		else 
			status = STATUS_INVALID_PARAMETER;
        break;

    case KSPROPERTY_VIDEOPROCAMP_HUE:

		if (ValidValueRange(DeviceExtension->HueRange,pS->Value)) 
			status = SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Hue), pS->Flags, pS->Value);       
 		else 
			status = STATUS_INVALID_PARAMETER;
        break;

    case KSPROPERTY_VIDEOPROCAMP_SATURATION:

		if (ValidValueRange(DeviceExtension->SaturationRange,pS->Value)) 
			status = SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Saturation), pS->Flags, pS->Value);       
 		else 
			status = STATUS_INVALID_PARAMETER;
        break;

    case KSPROPERTY_VIDEOPROCAMP_WHITEBALANCE:

		if (ValidValueRange(DeviceExtension->WhiteBalanceRange,pS->Value)) 
			status = SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, WhiteBalance), pS->Flags, pS->Value);       
 		else 
			status = STATUS_INVALID_PARAMETER;
        break;

    default:
		status = STATUS_NOT_IMPLEMENTED;        
        break;
    }

	pSrb->Status = status;
	pSrb->ActualBytesTransferred = (status == STATUS_SUCCESS ? sizeof (KSPROPERTY_VIDEOPROCAMP_S) : 0);
	

}


/*
** AdapterSetCameraControlProperty ()
**
**    Handles Set operations on the CameraControl property set.
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

VOID 
AdapterSetCameraControlProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
	NTSTATUS status;

    PSONYDCAM_EXTENSION 
		DeviceExtension = (PSONYDCAM_EXTENSION) pSrb->HwDeviceExtension;

    PSTREAM_PROPERTY_DESCRIPTOR 
		pSPD = pSrb->CommandData.PropertyInfo;

    PKSPROPERTY_CAMERACONTROL_S 
		pS = (PKSPROPERTY_CAMERACONTROL_S) pSPD->PropertyInfo;    // pointer to the data

    ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_CAMERACONTROL_S));    

    switch (pSPD->Property->Id) {

    case KSPROPERTY_CAMERACONTROL_FOCUS:
		
		if (ValidValueRange(DeviceExtension->FocusRange,pS->Value)) 
			status = SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Focus), pS->Flags, pS->Value);
 		else 
			status = STATUS_INVALID_PARAMETER;
		
        break;

    case KSPROPERTY_CAMERACONTROL_ZOOM:

		if (ValidValueRange(DeviceExtension->ZoomRange,pS->Value)) 
			status = SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Zoom), pS->Flags, pS->Value); 
		else 
			status = STATUS_INVALID_PARAMETER;		       	

        break;
 
    default:
	    status = STATUS_NOT_IMPLEMENTED;
        break;
    }

	pSrb->Status = status;
	pSrb->ActualBytesTransferred = (status == STATUS_SUCCESS ? sizeof (KSPROPERTY_CAMERACONTROL_S) : 0);

}


/*
** AdapterSetProperty ()
**
**    Handles Get operations for all adapter properties.
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

VOID
STREAMAPI 
AdapterSetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )

{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

	if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEOPROCAMP, &pSPD->Property->Set)) {
        AdapterSetVideoProcAmpProperty (pSrb);
    } else 	if (IsEqualGUID(&PROPSETID_VIDCAP_CAMERACONTROL, &pSPD->Property->Set)) {
        AdapterSetCameraControlProperty (pSrb);
    } else {
        //
        // We should never get here
        //

        TRAP();
    }
}


BOOL
GetPropertyValuesFromRegistry(
	PSONYDCAM_EXTENSION DeviceExtension
	)
{
	// Get the default from :
	//		HLM\Software\DeviceExtension->pchVendorName\1394DCam


	// Not implemented so always return FALSE to use the defaults.
	return FALSE;
}


BOOL
SetPropertyValuesToRegistry(	
	PSONYDCAM_EXTENSION DeviceExtension
	)
{
	// Set the default to :
	//		HLM\Software\DeviceExtension->pchVendorName\1394DCam


	// Not implemented!
	return FALSE;
}


VOID
SetCurrentDevicePropertyValues(
    PHW_STREAM_REQUEST_BLOCK pSrb,
	PSONYDCAM_EXTENSION DeviceExtension
				 )
{
	// Set to the last saved values or the defaults
	SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Brightness),  KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL, DeviceExtension->Brightness);
	SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Hue),         KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL, DeviceExtension->Hue);
	SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Saturation),  KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL, DeviceExtension->Saturation);
	SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Sharpness),   KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL, DeviceExtension->Sharpness);
	SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, WhiteBalance),KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL, DeviceExtension->WhiteBalance);
	SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Zoom),        KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL, DeviceExtension->Zoom);
	SonyDCamSetProperty((PIRB) pSrb->SRBExtension, DeviceExtension, FIELDOFFSET(CAMERA_REGISTER_MAP, Focus),       KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL, DeviceExtension->Focus);	
}

void
InitilizePropertyArray(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
	)
{

    PHW_STREAM_HEADER StreamHeader = &(pSrb->CommandData.StreamBuffer->StreamHeader);        
    PSONYDCAM_EXTENSION DeviceExtension = (PSONYDCAM_EXTENSION) pSrb->HwDeviceExtension;

	// Unknown vendor
	if (!DeviceExtension->pchVendorName)
		return;

	// Sony Dcam's range
	if (RtlCompareMemory(DeviceExtension->pchVendorName, "SONY", 4) == 4) {

		// Should read from registry only if not present, these default should be used
		if (!GetPropertyValuesFromRegistry(
				DeviceExtension)) {

			DeviceExtension->Brightness = SONYDCAM_DEF_BRIGHTNESS;
			DeviceExtension->BrightnessRange = BrightnessRangeAndStep[0].Bounds;
			DbgMsg3(("\'InitilizePropertyArray: Brightness range is (%d, %d, %d)\n", 
				DeviceExtension->BrightnessRange.SignedMinimum, DeviceExtension->Brightness, DeviceExtension->BrightnessRange.SignedMaximum)); 

			DeviceExtension->Hue        = SONYDCAM_DEF_HUE;
			DeviceExtension->HueRange   = HueRangeAndStep[0].Bounds;

			DeviceExtension->Saturation      = SONYDCAM_DEF_SATURATION;
			DeviceExtension->SaturationRange = SaturationRangeAndStep[0].Bounds;

			DeviceExtension->Sharpness      = SONYDCAM_DEF_SHARPNESS;
			DeviceExtension->SharpnessRange = SharpnessRangeAndStep[0].Bounds;

			DeviceExtension->WhiteBalance      = SONYDCAM_DEF_WHITEBALANCE;
			DeviceExtension->WhiteBalanceRange = WhiteBalanceRangeAndStep[0].Bounds;

			DeviceExtension->Zoom       = SONYDCAM_DEF_ZOOM;
			DeviceExtension->ZoomRange  = ZoomRangeAndStep[0].Bounds;

			DeviceExtension->Focus      = SONYDCAM_DEF_FOCUS;
			DeviceExtension->FocusRange = FocusRangeAndStep[0].Bounds;

		}
		
		StreamHeader->NumDevPropArrayEntries = NUMBER_OF_ADAPTER_PROPERTY_SETS;
		StreamHeader->DevicePropertiesArray = (PKSPROPERTY_SET) AdapterPropertyTable; 

	} else {

	if (RtlCompareMemory(DeviceExtension->pchVendorName, "TI", 2) == 2) {

		// Should read from registry only if not present, these default should be used
		if (!GetPropertyValuesFromRegistry(
				DeviceExtension)) {

			// Onlye those that are know to be activated are initialized.  
			// All others: ranges are queries and the default is 0.
			DeviceExtension->Brightness   = TIDCAM_DEF_BRIGHTNESS;
			DeviceExtension->BrightnessRange = BrightnessRangeAndStep2[0].Bounds;
			DbgMsg3(("\'InitilizePropertyArray: Brightness range is (%d, %d, %d)\n", 
				DeviceExtension->BrightnessRange.SignedMinimum, DeviceExtension->Brightness, DeviceExtension->BrightnessRange.SignedMaximum)); 


			DeviceExtension->Sharpness      = TIDCAM_DEF_SHARPNESS;
			DeviceExtension->SharpnessRange = SharpnessRangeAndStep2[0].Bounds;

			DeviceExtension->WhiteBalance      = TIDCAM_DEF_WHITEBALANCE;
			DeviceExtension->WhiteBalanceRange = WhiteBalanceRangeAndStep2[0].Bounds;

			DeviceExtension->Focus        = TIDCAM_DEF_FOCUS;
			DeviceExtension->FocusRange   = FocusRangeAndStep2[0].Bounds;
		}

		StreamHeader->NumDevPropArrayEntries = NUMBER_OF_ADAPTER_PROPERTY_SETS2;
		StreamHeader->DevicePropertiesArray = (PKSPROPERTY_SET) AdapterPropertyTable2; 

		} else {
			// other camera ?
			DbgMsg1(("\'InitilizePropertyArray: Unknown camera from Vendor %s\n", DeviceExtension->pchVendorName));
			ASSERT(FALSE);
			return;
		}
	}

	SetCurrentDevicePropertyValues(pSrb, DeviceExtension);
//#endif
}
