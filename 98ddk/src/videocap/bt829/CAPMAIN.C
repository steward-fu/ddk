//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;
    
#ifdef __cplusplus

extern "C" {

#include "strmini.h"
#include "ksmedia.h"

}

#endif
    
#include "i2cgpio.h"

#define DEFINE_MEDIUMS
#include "mediums.h"
#undef DEFINE_MEDIUMS

#include "capmain.h"
#include "capstrm.h"
#include "capprop.h"
#include "capdebug.h"

#include "i2script.h"
#include "aticonfg.h"
#include "wdmdrv.h"

#include "device.h"

#ifdef DEBUG
// Change this at compile time or in a debugger to change the amount
// of debug spew. Any strings that start with a single quote show up in
// the buffer maintained by ntkern. Type '.ntkern' in the debugger to 
// see its menu.
int DebugLevel = 1;
#endif

/*
** DriverEntry()
**
**   This routine is called when the driver is first loaded by PnP.
**   It in turn, calls upon the stream class to perform registration services.
**
** Arguments:
**
**   DriverObject - 
**          Driver object for this driver 
**
**   RegistryPath - 
**          Registry path string for this driver's key
**
** Returns:
**
**   Results of StreamClassRegisterAdapter()
**
** Side Effects:  none
*/
    
ULONG DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{

    HW_INITIALIZATION_DATA HwInitData;
    ULONG                       ReturnValue;

    DBGTRACE(("Bt829: DriverEntry\n"));
     
    RtlZeroMemory(&HwInitData, sizeof(HwInitData));

    HwInitData.HwInitializationDataSize = sizeof(HwInitData);

    //
    // Set the Adapter entry points for the driver
    //

    HwInitData.HwInterrupt              = NULL; // HwInterrupt;

    HwInitData.HwReceivePacket          = AdapterReceivePacket;
    HwInitData.HwCancelPacket           = AdapterCancelPacket;
    HwInitData.HwRequestTimeoutHandler  = AdapterTimeoutPacket;

    HwInitData.DeviceExtensionSize      = sizeof(HW_DEVICE_EXTENSION);
    HwInitData.PerRequestExtensionSize  = sizeof(SRB_EXTENSION); 
    HwInitData.FilterInstanceExtensionSize = 0;
    HwInitData.PerStreamExtensionSize   = sizeof(STREAMEX); 
    HwInitData.BusMasterDMA             = FALSE;  
    HwInitData.Dma24BitAddresses        = FALSE;
    HwInitData.BufferAlignment          = 3;
    HwInitData.DmaBufferSize            = 0;

    // Don't rely on the stream class using raised IRQL to synchronize
    // execution.  This single paramter most affects the overall structure
    // of the driver.
    HwInitData.TurnOffSynchronization   = TRUE;

    DBGTRACE(("Bt829: Calling StreamClassRegisterAdapter\n"));

    ReturnValue = StreamClassRegisterAdapter(DriverObject, RegistryPath, &HwInitData);

    return ReturnValue;
}
    
//==========================================================================;
//                   Adapter Based Request Handling Routines
//==========================================================================;
  
/*
** HwInitialize()
**
**   This routine is called when an SRB_INITIALIZE_DEVICE request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block for the Initialize command
**
** Returns:
**
** Side Effects:  none
*/
VOID HwInitialize (PHW_STREAM_REQUEST_BLOCK pSrb)
{
    pSrb->Status = STATUS_SUCCESS;

    UINT    nErrorCode;
    
    PPORT_CONFIGURATION_INFORMATION ConfigInfo = pSrb->CommandData.ConfigInfo;

    PHW_DEVICE_EXTENSION pHwDevExt =
            (PHW_DEVICE_EXTENSION)ConfigInfo->HwDeviceExtension;

    DBGTRACE(("Bt829 : HwInitialize()\n"));

    InitializeListHead (&pHwDevExt->adapterSrbQueue);
    KeInitializeSpinLock (&pHwDevExt->deviceExtensionLock);
    
    pHwDevExt->PDO = ConfigInfo->PhysicalDeviceObject;

    DBGINFO(("Bt829: Physical Device Object = %lx\n", pHwDevExt->PDO));

    pHwDevExt->chipAddr = 0;    // this will be set below

    pHwDevExt->pI2cScript = ( CI2CScript *) new (( PVOID)&pHwDevExt->CScript) \
            CI2CScript(pHwDevExt->PDO, &nErrorCode);
    
    if (nErrorCode != WDMMINI_NOERROR)
    {
        DBGERROR(("Bt829: CI2CScript creation failure = %lx\n", nErrorCode));
        pSrb -> Status = STATUS_NO_SUCH_DEVICE;
        TRAP();
        goto done;
    }
    else
    {
        // =========================================================
        // This block is specific to the ATI All-In-Wonder card. On any other
        // hardware, you will probably need to do something analogous to enable
        // the Bt829.
    
        if (!pHwDevExt->pI2cScript->LockI2CProviderEx())
        {
            DBGERROR(("Bt829: Couldn't get I2CProvider.\n"));
            pSrb -> Status = STATUS_NO_SUCH_DEVICE;
            TRAP();
            goto done;
        }
        
        CATIHwConfiguration CATIConfig(pHwDevExt->PDO, pHwDevExt->pI2cScript, &nErrorCode);
    
        if (nErrorCode != WDMMINI_NOERROR)
        {
            DBGERROR(("Bt829: CATIConfig constructor failure = %lx\n", nErrorCode));
            pSrb -> Status = STATUS_NO_SUCH_DEVICE;
            TRAP();
            pHwDevExt->pI2cScript->ReleaseI2CProvider();
            goto done;
        }
        
        CATIConfig.EnableDecoderOutput(pHwDevExt->pI2cScript, TRUE);
    
        pHwDevExt->pI2cScript->ReleaseI2CProvider();
        
        // End of ATI All-In-Wonder specific code.
        // =========================================================
    }
    
    pHwDevExt->device = new Device(ConfigInfo);

    if (pHwDevExt->device == NULL)
    {
        DBGERROR(("Bt829: couldn't create device\n"));
        pSrb->Status = STATUS_NO_SUCH_DEVICE;
        TRAP();
        goto done;
    }

    if (pHwDevExt->chipAddr == 0)
    {
        DBGERROR(("Bt829: couldn't get chip address\n"));
        pSrb->Status = STATUS_NO_SUCH_DEVICE;
        TRAP();
        goto done;
    }

    // The Bt829 has an ID of 1110 (0xe)
    if (pHwDevExt->chipID != 0xe)
    {
        DBGERROR(("Bt829: I2c failure or wrong decoder.\n"));
        pSrb->Status = STATUS_NO_SUCH_DEVICE;
        TRAP();
        goto done;
    }

    DBGINFO(("Bt829: Chip ID: 0x%x\n", pHwDevExt->chipID));
    DBGINFO(("Bt829: Chip revision: 0x%x\n", pHwDevExt->chipRev));

    if (ConfigInfo->NumberOfAccessRanges != 0) {
        DBGERROR(("Bt829: illegal config info\n"));
        pSrb->Status = STATUS_NO_SUCH_DEVICE;
        TRAP();
        goto done;
    }
    
    ConfigInfo->StreamDescriptorSize = sizeof (HW_STREAM_HEADER) +
        DRIVER_STREAM_COUNT * sizeof (HW_STREAM_INFORMATION);

    // zero is a valid ID, therefore, set to something
    // else to initialize it.
    pHwDevExt->ring3VideoPortHandle = -1;
done:

    ;
    
    DBGTRACE(("Exit : HwInitialize()\n"));
}
    
/*
** HwUnInitialize()
**
**   This routine is called when an SRB_UNINITIALIZE_DEVICE request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block for the UnInitialize command
**
** Returns:
**
** Side Effects:  none
*/
VOID HwUnInitialize (PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);

    delete pHwDevExt->device;

    pHwDevExt->device = NULL;
  
    pSrb->Status = STATUS_SUCCESS;
}


/*
** AdapterCompleteInitialization()
**
**   This routine is called when an SRB_COMPLETE_INITIALIZATION request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block
**
** Returns:
**
** Side Effects:  none
*/

VOID STREAMAPI AdapterCompleteInitialization (PHW_STREAM_REQUEST_BLOCK pSrb)
{
    NTSTATUS                Status;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    ULONG *tmp = (ULONG *) &CrossbarPinDirection[0];

    // Create the Registry blobs that DShow uses to create
    // graphs via Mediums

    Status = StreamClassRegisterFilterWithNoKSPins ( 
                    pHwDevExt->PDO,                     // IN PDEVICE_OBJECT   DeviceObject,
                    &KSCATEGORY_CROSSBAR,               // IN GUID           * InterfaceClassGUID,
                    SIZEOF_ARRAY (CrossbarMediums),     // IN ULONG            PinCount,
                    (int *) CrossbarPinDirection,       // IN ULONG          * Flags,
                    (KSPIN_MEDIUM *) CrossbarMediums,   // IN KSPIN_MEDIUM   * MediumList,
                    NULL                                // IN GUID           * CategoryList
            );

    // Register the Capture filter
    // Note:  This should be done automatically be MSKsSrv.sys, 
    // when that component comes on line (if ever) ...
    Status = StreamClassRegisterFilterWithNoKSPins ( 
                    pHwDevExt->PDO,                     // IN PDEVICE_OBJECT   DeviceObject,
                    &KSCATEGORY_CAPTURE,                // IN GUID           * InterfaceClassGUID,
                    SIZEOF_ARRAY (CaptureMediums),      // IN ULONG            PinCount,
                    (int *) CapturePinDirection,        // IN ULONG          * Flags,
                    (KSPIN_MEDIUM *) CaptureMediums,    // IN KSPIN_MEDIUM   * MediumList,
                    NULL                                // IN GUID           * CategoryList
            );
    pSrb->Status = STATUS_SUCCESS;
}

/*
** AdapterOpenStream()
**
**   This routine is called when an OpenStream SRB request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block for the Open command
**
** Returns:
**
** Side Effects:  none
*/

VOID AdapterOpenStream (PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    Device                  *device = pHwDevExt->device;
    int                     StreamNumber = pSrb->StreamObject->StreamNumber;
    UINT                    nSize;
    PKSDATAFORMAT           pKSDataFormat = pSrb->CommandData.OpenFormat;

    RtlZeroMemory(pStrmEx, sizeof(STREAMEX));

    DBGINFO(("Bt829: ADAPTEROPENSTREAM ------- StreamNumber=%d\n", StreamNumber));

    //
    // check that the stream index requested isn't too high
    // or that the maximum number of instances hasn't been exceeded
    //

    if (StreamNumber >= DRIVER_STREAM_COUNT || StreamNumber < 0) {

        pSrb->Status = STATUS_INVALID_PARAMETER;
        goto AdapterOpenStreamExit;
    }

    //
    // Check the validity of the format being requested
    //

    if (!AdapterVerifyFormat (pKSDataFormat, StreamNumber)) {

        pSrb->Status = STATUS_INVALID_PARAMETER;
        goto AdapterOpenStreamExit;
    }


    // -------------------------------------------------------------------
    // Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
    // -------------------------------------------------------------------

    if (IsEqualGUID (pKSDataFormat->Specifier, 
                KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {
        
        PKS_DATAFORMAT_VIDEOINFOHEADER  pVideoInfoHeader = 
                    (PKS_DATAFORMAT_VIDEOINFOHEADER) pSrb->CommandData.OpenFormat;
        PKS_VIDEOINFOHEADER     pVideoInfoHdrRequested = 
                    &pVideoInfoHeader->VideoInfoHeader;
    
        ASSERT(pStrmEx->pVideoInfoHeader == NULL);
        ASSERT(StreamNumber == STREAM_VideoCapture);

        // Since the VIDEOINFOHEADER is of potentially variable size
        // allocate memory for it

        nSize = KS_SIZE_VIDEOHEADER(pVideoInfoHdrRequested);

        DBGINFO(("Bt829: pVideoInfoHdrRequested=%x\n", pVideoInfoHdrRequested));
        DBGINFO(("Bt829: KS_VIDEOINFOHEADER size=%d\n", nSize));
        DBGINFO(("Bt829: Width=%d  Height=%d  BitCount=%d\n", 
                    pVideoInfoHdrRequested->bmiHeader.biWidth,
                    pVideoInfoHdrRequested->bmiHeader.biHeight,
                    pVideoInfoHdrRequested->bmiHeader.biBitCount));
        DBGINFO(("Bt829: biSizeImage=%d\n", 
                    pVideoInfoHdrRequested->bmiHeader.biSizeImage));
        DBGINFO(("Bt829: AvgTimePerFrame=%d\n", 
                    pVideoInfoHdrRequested->AvgTimePerFrame));

        pStrmEx->pVideoInfoHeader = (PKS_VIDEOINFOHEADER)ExAllocatePool(NonPagedPool, nSize);

        if (pStrmEx->pVideoInfoHeader == NULL) {
            DBGERROR(("Bt829: ExAllocatePool failed\n"));
            pSrb->Status = STATUS_INSUFFICIENT_RESOURCES;
            goto AdapterOpenStreamExit;
        }

        // Copy the VIDEOINFOHEADER requested to our storage
        RtlCopyMemory(
                pStrmEx->pVideoInfoHeader,
                pVideoInfoHdrRequested,
                nSize);
        
        // Now set up the decoder, and force a reconnect on the VP stream
        // if it has already connected.
        MRect t(0, 0,   pVideoInfoHdrRequested->bmiHeader.biWidth,
                        pVideoInfoHdrRequested->bmiHeader.biHeight);
        device->SetRect(t);
        
        if (pHwDevExt->pVpStrmEx)
        {
            AttemptRenegotiation(pHwDevExt->pVpStrmEx);
        }
        pHwDevExt->pVideoCaptureStrmEx = pStrmEx;
   }

    // -------------------------------------------------------------------
    // Specifier FORMAT_AnalogVideo for KS_ANALOGVIDEOINFO
    // -------------------------------------------------------------------
    else if (IsEqualGUID (pKSDataFormat->Specifier, 
                KSDATAFORMAT_SPECIFIER_ANALOGVIDEO)) {
      
            //
            // AnalogVideo DataRange == DataFormat!
            //

            // For now, don't even cache this
            // BUGBUG
            //

            PKS_DATARANGE_ANALOGVIDEO pDataFormatAnalogVideo = 
                    (PKS_DATARANGE_ANALOGVIDEO) pSrb->CommandData.OpenFormat;
            ASSERT(StreamNumber == STREAM_AnalogVideoInput);
    }

    // -------------------------------------------------------------------
    // Specifier FORMAT_VideoInfo for VBIINFOHEADER
    // -------------------------------------------------------------------
    else if (IsEqualGUID (pKSDataFormat->Specifier, 
                  KSDATAFORMAT_SPECIFIER_VBI)) {

        PKS_DATAFORMAT_VBIINFOHEADER  pVBIInfoHeader = 
                    (PKS_DATAFORMAT_VBIINFOHEADER) pSrb->CommandData.OpenFormat;

        PKS_VBIINFOHEADER     pVBIInfoHdrRequested = 
                    &pVBIInfoHeader->VBIInfoHeader;
    
        ASSERT(pStrmEx->pVBIInfoHeader == NULL);
        ASSERT(StreamNumber == STREAM_VBICapture);
        
        DBGINFO(("Bt829: pVBIInfoHdrRequested->StartLine = %d\n", pVBIInfoHdrRequested->StartLine));
        DBGINFO(("Bt829: pVBIInfoHdrRequested->EndLine = %d\n", pVBIInfoHdrRequested->EndLine));
        DBGINFO(("Bt829: pVBIInfoHdrRequested->MinLineStartTime = %d\n", pVBIInfoHdrRequested->MinLineStartTime));
        DBGINFO(("Bt829: pVBIInfoHdrRequested->MaxLineStartTime = %d\n", pVBIInfoHdrRequested->MaxLineStartTime));
        DBGINFO(("Bt829: pVBIInfoHdrRequested->ActualLineStartTime = %d\n", pVBIInfoHdrRequested->ActualLineStartTime));
        DBGINFO(("Bt829: pVBIInfoHdrRequested->VideoStandard = 0x%x\n", pVBIInfoHdrRequested->VideoStandard));
        DBGINFO(("Bt829: pVBIInfoHdrRequested->SamplesPerLine = %d\n", pVBIInfoHdrRequested->SamplesPerLine));
        DBGINFO(("Bt829: pVBIInfoHdrRequested->StrideInBytes = %d\n", pVBIInfoHdrRequested->StrideInBytes));
        DBGINFO(("Bt829: pVBIInfoHdrRequested->BufferSize = %d\n", pVBIInfoHdrRequested->BufferSize));

        ASSERT(pStrmEx->pVBIInfoHeader == NULL);

        nSize = sizeof (KS_VBIINFOHEADER);

        pStrmEx->pVBIInfoHeader = (PKS_VBIINFOHEADER)ExAllocatePool(NonPagedPool, nSize);

        if (pStrmEx->pVBIInfoHeader == NULL) {
            DBGERROR(("Bt829: ExAllocatePool failed\n"));
            pSrb->Status = STATUS_INSUFFICIENT_RESOURCES;
            goto AdapterOpenStreamExit;
        }

        // Copy the VBIINFOHEADER requested to our storage
        RtlCopyMemory(
                pStrmEx->pVBIInfoHeader,
                pVBIInfoHdrRequested,
                nSize);

        // This will get picked up the when the first buffer
        // gets filled.
        pHwDevExt->VBIInfoChanged = TRUE;

        pHwDevExt->VBICaptureStreamConnected = TRUE;
    }

    // -------------------------------------------------------------------
    // Specifier STATIC_KSDATAFORMAT_TYPE_VIDEO for Video Port
    // -------------------------------------------------------------------

    else if (IsEqualGUID (pKSDataFormat->Specifier, 
                  KSDATAFORMAT_SPECIFIER_NONE) &&
                  IsEqualGUID (pKSDataFormat->SubFormat, KSDATAFORMAT_SUBTYPE_VPVideo)) {

        ASSERT(StreamNumber == STREAM_VPVideo);
        
        if (pHwDevExt->pVideoCaptureStrmEx == NULL)
        {
            MRect t(0, 0,   device->GetDefaultDecoderWidth(),
                            device->GetDefaultDecoderHeight());
            device->SetRect(t);
        }
        pHwDevExt->pVpStrmEx = pStrmEx;
    }
  
  
    // -------------------------------------------------------------------
    // Specifier KSDATAFORMAT_SPECIFIER_NONE for VP VBI
    // -------------------------------------------------------------------

    else if (IsEqualGUID (pKSDataFormat->Specifier, 
                  KSDATAFORMAT_SPECIFIER_NONE) &&
                  IsEqualGUID (pKSDataFormat->SubFormat, KSDATAFORMAT_SUBTYPE_VPVBI)) {
        
        ASSERT(StreamNumber == STREAM_VPVBI);
        
        device->SetVBIEN(TRUE);
        device->SetVBIFMT(TRUE);
    }
  
    else
    {
        pSrb->Status = STATUS_UNSUCCESSFUL;
        goto AdapterOpenStreamExit;
    }
    

    // Set up pointers to the handlers for the stream data and control handlers
    pSrb->StreamObject->ReceiveDataPacket = Streams[StreamNumber].hwStreamObject.ReceiveDataPacket;
    pSrb->StreamObject->ReceiveControlPacket = Streams[StreamNumber].hwStreamObject.ReceiveControlPacket;

    //
    // The DMA flag must be set when the device will be performing DMA directly
    // to the data buffer addresses passed in to the ReceiceDataPacket routines.
    //
    pSrb->StreamObject->Dma = Streams[StreamNumber].hwStreamObject.Dma;

    //
    // The PIO flag must be set when the mini driver will be accessing the data
    // buffers passed in using logical addressing
    //
    pSrb->StreamObject->Pio = Streams[StreamNumber].hwStreamObject.Pio;

    //
    // How many extra bytes will be passed up from the driver for each frame?
    //
    pSrb->StreamObject->StreamHeaderMediaSpecific = 
        Streams[StreamNumber].hwStreamObject.StreamHeaderMediaSpecific;

    pSrb->StreamObject->StreamHeaderWorkspace =
        Streams[StreamNumber].hwStreamObject.StreamHeaderWorkspace;

    //
    // Indicate the clock support available on this stream
    //

    pSrb->StreamObject->HwClockObject = 
        Streams[StreamNumber].hwStreamObject.HwClockObject;

    //
    // Indicate the event support available on this stream
    //

    pSrb->StreamObject->HwEventRoutine = 
        Streams[StreamNumber].hwStreamObject.HwEventRoutine;

    //
    // Indicate the allocator support available on this stream
    //

    pSrb->StreamObject->Allocator = Streams[StreamNumber].hwStreamObject.Allocator;

    // Retain a private copy of the HwDevExt and StreamObject in the stream extension
   
    pStrmEx->pHwDevExt = pHwDevExt;
    pStrmEx->pStreamObject = pSrb->StreamObject;
    
    InitializeListHead (&pStrmEx->ctrlSrbQueue);
    
    KeInitializeSpinLock (&pStrmEx->streamExtensionLock);
    
    KeInitializeEvent(&pStrmEx->specialEvent, SynchronizationEvent, FALSE);
    KeInitializeEvent(&pStrmEx->stateTransitionEvent, SynchronizationEvent, FALSE);
    KeInitializeEvent(&pStrmEx->SrbAvailableEvent, SynchronizationEvent, FALSE);

    if (StreamNumber == STREAM_VideoCapture || StreamNumber == STREAM_VBICapture)
    {
        InitializeListHead (&pStrmEx->incomingDataSrbQueue);
        InitializeListHead (&pStrmEx->waitQueue);
        InitializeListHead (&pStrmEx->reversalQueue);
        
        ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
        
        ASSERT(pStrmEx->stateChange == ChangeComplete);
        pStrmEx->stateChange = Starting;
        KeResetEvent(&pStrmEx->specialEvent);
        
        if (!CreateStreamThread(pStrmEx))
        {
            DBGERROR(("Bt829: CreateStreamThread failed\n"));
            pSrb->Status = STATUS_INSUFFICIENT_RESOURCES;
            goto AdapterOpenStreamExit;
        }

        KeWaitForSingleObject( &pStrmEx->specialEvent, Suspended, KernelMode, FALSE, NULL);
        ASSERT(pStrmEx->stateChange == ChangeComplete);
        
        DBGTRACE(("Bt829: AdapterOpenStream got notification that thread started\n"));
        
        // Don't need this for anything, so might as well close it now.
        // The thread will call PsTerminateThread on itself when it
        // is done.
        ZwClose(pStrmEx->threadHandle);
    }

    pHwDevExt->openStreams++;
    pSrb->Status = STATUS_SUCCESS;

AdapterOpenStreamExit:
    ;
    DBGTRACE(("Bt829: AdapterOpenStream Exit\n"));
}
    
/*
** AdapterCloseStream()
**
**   Close the requested data stream
**
** Arguments:
**
**   pSrb the request block requesting to close the stream
**
** Returns:
**
** Side Effects:  none
*/

VOID AdapterCloseStream (PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    Device                  *device = pHwDevExt->device;
    int                     StreamNumber = pSrb->StreamObject->StreamNumber;
    PKSDATAFORMAT           pKSDataFormat = pSrb->CommandData.OpenFormat;
    KS_VIDEOINFOHEADER      *pVideoInfoHdr = NULL;
    KS_VBIINFOHEADER        *pVBIInfoHdr = NULL;
    KIRQL                   Irql;

    DBGINFO(("Bt829: ADAPTERCLOSESTREAM ------- StreamNumber=%d\n", StreamNumber));
    
    //
    // the minidriver may wish to free any resources that were allocated at
    // open stream time etc.
    //

    KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
    if (!IsListEmpty(&pStrmEx->ctrlSrbQueue))
    {
        TRAP();
    }

    if (!IsListEmpty(&pHwDevExt->adapterSrbQueue))
    {
        TRAP();
    }
    KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);

    // Free the variable length VIDEOINFOHEADER, VBIInfoHeader, surface handle pointer
    if (StreamNumber == STREAM_VideoCapture || StreamNumber == STREAM_VBICapture)
    {
        ASSERT(pStrmEx->stateChange == ChangeComplete);
        pStrmEx->stateChange = Closing;
        KeResetEvent(&pStrmEx->specialEvent);
        KeSetEvent(&pStrmEx->stateTransitionEvent, 0, TRUE);
        KeWaitForSingleObject( &pStrmEx->specialEvent, Suspended, KernelMode, FALSE, NULL);
        ASSERT(pStrmEx->stateChange == ChangeComplete);
        

        KeAcquireSpinLock(&pStrmEx->streamExtensionLock, &Irql);
        if (!IsListEmpty(&pStrmEx->incomingDataSrbQueue))
        {
            TRAP();
        }
    
        if (!IsListEmpty(&pStrmEx->waitQueue))
        {
            TRAP();
        }
        KeReleaseSpinLock(&pStrmEx->streamExtensionLock, Irql);
    
        ReleaseCaptureHandle(pStrmEx);
        
        if (StreamNumber == STREAM_VideoCapture)
        {
            pVideoInfoHdr = pStrmEx->pVideoInfoHeader;
            if (pVideoInfoHdr) {
                ExFreePool(pVideoInfoHdr);
                pStrmEx->pVideoInfoHeader = NULL;
            }
            pHwDevExt->pVideoCaptureStrmEx = NULL;
        }

        else if (StreamNumber == STREAM_VBICapture)
        {
            pVBIInfoHdr = pStrmEx->pVBIInfoHeader;
            if (pVBIInfoHdr) {
                ExFreePool(pVBIInfoHdr);
                pStrmEx->pVBIInfoHeader = NULL;
            }
            
            pHwDevExt->VBICaptureStreamConnected = FALSE;
            if (pHwDevExt->TVTunerChangedSrb)
            {
                PHW_STREAM_REQUEST_BLOCK    TVTunerChangedSrb; 
                
                TVTunerChangedSrb = pHwDevExt->TVTunerChangedSrb;
                pHwDevExt->TVTunerChangedSrb = NULL;
                pHwDevExt->TVTunerChanged = FALSE;
                StreamClassStreamNotification(  StreamRequestComplete,
                                                TVTunerChangedSrb->StreamObject,
                                                TVTunerChangedSrb);
        
            }
        }
    }

    if (StreamNumber == STREAM_VPVideo || StreamNumber == STREAM_VPVBI)
    {
        if (StreamNumber == STREAM_VPVideo)
        {
            pHwDevExt->pVpStrmEx = NULL;
        }
        
        if (pStrmEx->registered)
        {
            UnregisterForDirectDrawEvents(pStrmEx);
        }
    }

    if (--pHwDevExt->openStreams == 0)
    {
        DBGINFO(("Bt829: Last one out turns off the lights\n"));

        ReleaseRing0VideoPortHandle(pStrmEx);
        pHwDevExt->ring3VideoPortHandle = -1;
        
        ReleaseRing0DirectDrawHandle(pStrmEx);
        pHwDevExt->ring3DirectDrawHandle = 0;

        pHwDevExt->preEventOccurred = FALSE;
        pHwDevExt->postEventOccurred = FALSE;

        device->SaveState();

        if (pHwDevExt->ring3SurfaceHandles) {
            ExFreePool(pHwDevExt->ring3SurfaceHandles);
            pHwDevExt->ring3SurfaceHandles = NULL;
        }
        pHwDevExt->numRing3SurfaceHandles = 0;

        if (pHwDevExt->ring3VBISurfaceHandles) {
            ExFreePool(pHwDevExt->ring3VBISurfaceHandles);
            pHwDevExt->ring3VBISurfaceHandles = NULL;
        }
        pHwDevExt->numRing3VBISurfaceHandles = 0;
    }

    // Succeed the call
    pSrb->Status = STATUS_SUCCESS;
}
    
    
/*
** AdapterStreamInfo()
**
**   Returns the information of all streams that are supported by the
**   mini-driver
**
** Arguments:
**
**   pSrb - Pointer to the STREAM_REQUEST_BLOCK
**        pSrb->HwDeviceExtension - will be the hardware device extension for
**                                  as initialised in HwInitialise
**
** Returns:
**
** Side Effects:  none
*/

VOID AdapterStreamInfo(PHW_STREAM_REQUEST_BLOCK pSrb)

{
    int j; 
      
    PHW_DEVICE_EXTENSION pHwDevExt =
    ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);

    //
    // pick up the pointer to header which preceeds the stream info structs
    //

    PHW_STREAM_HEADER pstrhdr =
        (PHW_STREAM_HEADER)&(pSrb->CommandData.StreamBuffer->StreamHeader);

    //
    // pick up the pointer to the stream information data structure
    //

    PHW_STREAM_INFORMATION pstrinfo =
    (PHW_STREAM_INFORMATION)&(pSrb->CommandData.StreamBuffer->StreamInfo);


    // 
    // verify that the buffer is large enough to hold our return data
    //
    DEBUG_ASSERT (pSrb->NumberOfBytesToTransfer >= 
        sizeof (HW_STREAM_HEADER) +
        sizeof (HW_STREAM_INFORMATION) * DRIVER_STREAM_COUNT);

    //
    // Set the header
    // 
    StreamHeader.NumDevPropArrayEntries = NUMBER_OF_ADAPTER_PROPERTY_SETS;
    StreamHeader.DevicePropertiesArray = (PKSPROPERTY_SET) AdapterPropertyTable; 
    *pstrhdr = StreamHeader;

    // 
    // stuff the contents of each HW_STREAM_INFORMATION struct 
    //

    for (j = 0; j < DRIVER_STREAM_COUNT; j++) {
        *pstrinfo++ = Streams[j].hwStreamInfo;
    }

    //
    // indicate success
    //
    
    pSrb->Status = STATUS_SUCCESS;
     
}
    
    
/*
** AdapterReceivePacket()
**
**   Main entry point for receiving adapter based request SRBs.  This routine
**   will always be called at High Priority.
**
**   Note: This is an asyncronous entry point.  The request does not complete
**         on return from this function, the request only completes when a
**         StreamClassDeviceNotification on this request block, of type
**         DeviceRequestComplete, is issued.
**
** Arguments:
**
**   pSrb - Pointer to the STREAM_REQUEST_BLOCK
**        pSrb->HwDeviceExtension - will be the hardware device extension for
**                                  as initialised in HwInitialise
**
** Returns:
**
** Side Effects:  none
*/

VOID STREAMAPI AdapterReceivePacket(IN PHW_STREAM_REQUEST_BLOCK pSrb)
{
    KIRQL Irql;
    PHW_STREAM_REQUEST_BLOCK pCurrentSrb = NULL;

    PHW_DEVICE_EXTENSION pHwDevExt =
        ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);

    PSRB_EXTENSION pSrbExt;

    // This needs to be a special cased because no spinlocks, etc
    // have been initialized until HwInitialize runs. Even though
    // this minidriver handles synchronization itself, it assumes
    // that no adapter SRBs will arrive until after this one
    // completes.
    if (pSrb->Command == SRB_INITIALIZE_DEVICE)
    {
        DBGINFO(("Bt829: SRB_INITIALIZE_DEVICE; SRB=%x\n", pSrb));

        HwInitialize(pSrb);
        StreamClassDeviceNotification(DeviceRequestComplete, pSrb->HwDeviceExtension, pSrb);
        return;
    }

    ASSERT (pSrb->Command != SRB_INITIALIZE_DEVICE);

    KeAcquireSpinLock(&pHwDevExt->deviceExtensionLock, &Irql);
    if (pHwDevExt->processingAdapterSrb)
    {
        pSrbExt = (PSRB_EXTENSION)pSrb->SRBExtension;
        pSrbExt->pSrb = pSrb;
        InsertTailList(&pHwDevExt->adapterSrbQueue, &pSrbExt->listEntry);
        KeReleaseSpinLock(&pHwDevExt->deviceExtensionLock, Irql);
        return;
    }

    pHwDevExt->processingAdapterSrb = TRUE;
    pCurrentSrb = pSrb;
    KeReleaseSpinLock(&pHwDevExt->deviceExtensionLock, Irql);

    // This will run until the queue is empty
    while (TRUE)
    {
        ASSERT(pCurrentSrb != NULL);
        
        // Assume success. Might be changed below

        pCurrentSrb->Status = STATUS_SUCCESS;
 
        switch (pCurrentSrb->Command)
        {
        case SRB_INITIALIZATION_COMPLETE:
            DBGINFO(("Bt829: SRB_INITIALIZATION_COMPLETE; SRB=%x\n", pCurrentSrb));

            //
            // Stream class has finished initialization.
            // Now create DShow Medium interface BLOBs.
            // This needs to be done at low priority since it uses the registry
            //

            AdapterCompleteInitialization(pSrb);
            break;
        

        case SRB_UNINITIALIZE_DEVICE:
            DBGINFO(("Bt829: SRB_UNINITIALIZE_DEVICE; SRB=%x\n", pCurrentSrb));

            HwUnInitialize(pCurrentSrb);
            break;

        case SRB_PAGING_OUT_DRIVER:
            DBGINFO(("Bt829: SRB_PAGING_OUT_DRIVER; SRB=%x\n", pCurrentSrb));
            //
            // The driver is being paged out
            // Disable Interrupts if you have them!
            //
            break;
    
        case SRB_CHANGE_POWER_STATE:
            DBGINFO(("Bt829: SRB_CHANGE_POWER_STATE. SRB=%x. State=%d\n",
                                                    pCurrentSrb,
                                                    pCurrentSrb->CommandData.DeviceState));

            switch (pCurrentSrb->CommandData.DeviceState)
            {
                case PowerDeviceD3:
                    pHwDevExt->preEventOccurred = TRUE;
                    pHwDevExt->device->SaveState();
                    break;
                case PowerDeviceD2:
                    pHwDevExt->preEventOccurred = TRUE;
                    pHwDevExt->device->SaveState();
                    break;
                case PowerDeviceD1:
                    pHwDevExt->preEventOccurred = TRUE;
                    pHwDevExt->device->SaveState();
                    break;
                case PowerDeviceD0:
                    pHwDevExt->postEventOccurred = TRUE;
                    pHwDevExt->device->RestoreState();
                    break;
            }
            break;

        case SRB_OPEN_STREAM:
            DBGINFO(("Bt829: SRB_OPEN_STREAM; SRB=%x\n", pCurrentSrb));

            AdapterOpenStream(pCurrentSrb);
            break;

        case SRB_CLOSE_STREAM:
            DBGINFO(("Bt829: SRB_CLOSE_STREAM; SRB=%x\n", pCurrentSrb));

            AdapterCloseStream(pCurrentSrb);
            break;

        case SRB_GET_STREAM_INFO:
            DBGINFO(("Bt829: SRB_GET_STREAM_INFO; SRB=%x\n", pCurrentSrb));

            AdapterStreamInfo(pCurrentSrb);
            break;

        case SRB_GET_DEVICE_PROPERTY:
            // DBGINFO(("'Bt829: SRB_GET_DEVICE_PROPERTY; SRB=%x\n", pCurrentSrb));

            AdapterGetProperty(pCurrentSrb);
            break;        
    
        case SRB_SET_DEVICE_PROPERTY:
            // DBGINFO(("'Bt829: SRB_SET_DEVICE_PROPERTY; SRB=%x\n", pCurrentSrb));

            AdapterSetProperty(pCurrentSrb);
            break;
    
        case SRB_GET_DATA_INTERSECTION:
            // DBGINFO(("'Bt829: SRB_GET_DATA_INTERSECTION; SRB=%x\n", pCurrentSrb));

            AdapterFormatFromRange(pCurrentSrb);
            break;

        case SRB_UNKNOWN_DEVICE_COMMAND:
            // not sure why this gets called every time.
            DBGWARN(("Bt829: SRB_UNKNOWN_DEVICE_COMMAND; SRB=%x\n", pCurrentSrb));

            // TRAP();
            pCurrentSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;

            // We should never get the following since this is a single instance
            // device
        case SRB_OPEN_DEVICE_INSTANCE:
            DBGINFO(("Bt829: SRB_OPEN_DEVICE_INSTANCE; SRB=%x\n", pCurrentSrb));

            TRAP();
            pCurrentSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;

        case SRB_CLOSE_DEVICE_INSTANCE:
            DBGINFO(("Bt829: SRB_CLOSE_DEVICE_INSTANCE; SRB=%x\n", pCurrentSrb));

            TRAP();
            pCurrentSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;

        default:
            DBGINFO(("Bt829: default; SRB=%x\n", pCurrentSrb));

            TRAP();
    
            //
            // this is a request that we do not understand.  Indicate invalid
            // command.
            //
    
            pCurrentSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;
        }

        StreamClassDeviceNotification(  DeviceRequestComplete, pCurrentSrb->HwDeviceExtension,
                                        pCurrentSrb);
        KeAcquireSpinLock(&pHwDevExt->deviceExtensionLock, &Irql);
        if (IsListEmpty(&pHwDevExt->adapterSrbQueue))
        {
            pHwDevExt->processingAdapterSrb = FALSE;
            KeReleaseSpinLock(&pHwDevExt->deviceExtensionLock, Irql);
            return;
        }
        else
        {
            PUCHAR ptr;

            ptr = (PUCHAR)RemoveHeadList(&pHwDevExt->adapterSrbQueue);
            KeReleaseSpinLock(&pHwDevExt->deviceExtensionLock, Irql);
            pSrbExt = (PSRB_EXTENSION) (((PUCHAR) ptr) - 
                         FIELDOFFSET(SRB_EXTENSION, listEntry));

            pCurrentSrb = pSrbExt->pSrb;
        }
    }
}
    
/*
** AdapterCancelPacket ()
**
**   Request to cancel a packet that is currently in process in the minidriver
**
** Arguments:
**
**   pSrb - pointer to request packet to cancel
**
** Returns:
**
** Side Effects:  none
*/

VOID STREAMAPI AdapterCancelPacket(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    DBGINFO(("Attempting to cancel Srb 0x%x\n", pSrb));
    pSrb->Status = STATUS_CANCELLED;
}

/*
** AdapterTimeoutPacket()
**
**   This routine is called when a packet has been in the minidriver for
**   too long.  The adapter must decide what to do with the packet
**
** Arguments:
**
**   pSrb - pointer to the request packet that timed out
**
** Returns:
**
** Side Effects:  none
*/

VOID AdapterTimeoutPacket(PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    UINT                    streamNumber = pSrb->StreamObject->StreamNumber;
    PHW_DEVICE_EXTENSION    pHwDevExt =
        ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    
    // Incomplete!!!
    if (pHwDevExt->TVTunerChangedSrb)
    {
        PHW_STREAM_REQUEST_BLOCK    TVTunerChangedSrb; 

        DBGINFO(("Bt829: AdapterTimeoutPacket: SRB %8x. Completing.\n", pSrb));
        TVTunerChangedSrb = pHwDevExt->TVTunerChangedSrb;
        pHwDevExt->TVTunerChangedSrb = NULL;
        pHwDevExt->TVTunerChanged = FALSE;
        StreamClassStreamNotification(  StreamRequestComplete,
                                        TVTunerChangedSrb->StreamObject,
                                        TVTunerChangedSrb);
        return;

    }

    if (pStrmEx->KSState == KSSTATE_STOP || !pHwDevExt->preEventOccurred)
    {
        DBGINFO(("Bt829: Suspicious timeout. SRB %8x. \n", pSrb));
        if (streamNumber == STREAM_VideoCapture || streamNumber == STREAM_VBICapture)
        {
            DBGINFO(("Attempting to complete Srbs.\n"));
            EmptyIncomingDataSrbQueue(pStrmEx);
        }
    }
    else
    {
        DBGINFO(("Bt829: AdapterTimeoutPacket: SRB %8x. Resetting.\n", pSrb));
        pSrb->TimeoutCounter = pSrb->TimeoutOriginal;
    }
    return;
}

/*
** AdapterCompareGUIDsAndFormatSize()
**
**   Checks for a match on the three GUIDs and FormatSize
**
** Arguments:
**
**         IN DataRange1
**         IN DataRange2
**         BOOL fCompareFormatSize - TRUE when comparing ranges
**                                 - FALSE when comparing formats
**
** Returns:
** 
**   TRUE if all elements match
**   FALSE if any are different
**
** Side Effects:  none
*/

BOOL 
AdapterCompareGUIDsAndFormatSize(
    IN PKSDATARANGE DataRange1,
    IN PKSDATARANGE DataRange2,
    BOOL fCompareFormatSize
    )
{
    return (
        IsEqualGUID (
            DataRange1->MajorFormat, 
            DataRange2->MajorFormat) &&
        IsEqualGUID (
            DataRange1->SubFormat, 
            DataRange2->SubFormat) &&
        IsEqualGUID (
            DataRange1->Specifier, 
            DataRange2->Specifier) && 
        (fCompareFormatSize ? 
                (DataRange1->FormatSize == DataRange2->FormatSize) : TRUE ));
}

/*
** AdapterVerifyFormat()
**
**   Checks the validity of a format request by walking through the
**       array of supported KSDATA_RANGEs for a given stream.
**
** Arguments:
**
**   pKSDataFormat - pointer of a KSDATAFORMAT structure.
**   StreamNumber - index of the stream being queried / opened.
**
** Returns:
** 
**   TRUE if the format is supported
**   FALSE if the format cannot be suppored
**
** Side Effects:  none
*/

BOOL AdapterVerifyFormat(PKSDATAFORMAT pKSDataFormatToVerify, int StreamNumber)
{
    BOOL                        fOK = FALSE;
    ULONG                       j;
    ULONG                       NumberOfFormatArrayEntries;
    PKSDATAFORMAT               *pAvailableFormats;


    //
    // Check that the stream number is valid
    //

    if (StreamNumber >= DRIVER_STREAM_COUNT) {
        TRAP();
        return FALSE;
    }
    
    NumberOfFormatArrayEntries = 
            Streams[StreamNumber].hwStreamInfo.NumberOfFormatArrayEntries;

    //
    // Get the pointer to the array of available formats
    //

    pAvailableFormats = Streams[StreamNumber].hwStreamInfo.StreamFormatsArray;

    DBGINFO(("Bt829: AdapterVerifyFormat, Stream=%d\n", StreamNumber));
    DBGINFO(("Bt829: FormatSize=%d\n", 
            pKSDataFormatToVerify->FormatSize));
    DBGINFO(("Bt829: MajorFormat=%x\n", 
            pKSDataFormatToVerify->MajorFormat));

    //
    // Walk the formats supported by the stream
    //

    for (j = 0; j < NumberOfFormatArrayEntries; j++, pAvailableFormats++) {

        // Check for a match on the three GUIDs and format size

        if (!AdapterCompareGUIDsAndFormatSize(
                        pKSDataFormatToVerify, 
                        *pAvailableFormats,
                        FALSE /* CompareFormatSize */ )) {
            continue;
        }

        //
        // Now that the three GUIDs match, switch on the Specifier
        // to do a further type-specific check
        //

        // -------------------------------------------------------------------
        // Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
        // -------------------------------------------------------------------

        if (IsEqualGUID (pKSDataFormatToVerify->Specifier, 
                KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {
                
            PKS_DATAFORMAT_VIDEOINFOHEADER  pDataFormatVideoInfoHeader = 
                    (PKS_DATAFORMAT_VIDEOINFOHEADER) pKSDataFormatToVerify;
            PKS_VIDEOINFOHEADER  pVideoInfoHdrToVerify = 
                     (PKS_VIDEOINFOHEADER) &pDataFormatVideoInfoHeader->VideoInfoHeader;
            PKS_DATARANGE_VIDEO             pKSDataRangeVideo = (PKS_DATARANGE_VIDEO) *pAvailableFormats;
            KS_VIDEO_STREAM_CONFIG_CAPS    *pConfigCaps = &pKSDataRangeVideo->ConfigCaps;

            //
            // BUGBUG, perform some verification tests here!!!
            //

            fOK = TRUE;
            break;

        } // End of VIDEOINFOHEADER specifier

        // -------------------------------------------------------------------
        // Specifier FORMAT_VideoInfo for VBIINFOHEADER
        // -------------------------------------------------------------------

        if (IsEqualGUID (pKSDataFormatToVerify->Specifier, 
                KSDATAFORMAT_SPECIFIER_VBI)) {
                
            //
            // BUGBUG, perform some verification tests here!!!
            //

            fOK = TRUE;
            break;

        } // End of VBI specifier

        // -------------------------------------------------------------------
        // Specifier FORMAT_AnalogVideo for KS_ANALOGVIDEOINFO
        // -------------------------------------------------------------------

        else if (IsEqualGUID (pKSDataFormatToVerify->Specifier, 
                KSDATAFORMAT_SPECIFIER_ANALOGVIDEO)) {
      
            //
            // BUGBUG, perform some verification tests here!!!
            //

            fOK = TRUE;
            break;

        } // End of KS_ANALOGVIDEOINFO specifier

        // -------------------------------------------------------------------
        // Specifier STATIC_KSDATAFORMAT_TYPE_VIDEO for Video Port
        // -------------------------------------------------------------------

        else if (IsEqualGUID (pKSDataFormatToVerify->Specifier, 
                      KSDATAFORMAT_SPECIFIER_NONE) &&
                      IsEqualGUID (pKSDataFormatToVerify->SubFormat, KSDATAFORMAT_SUBTYPE_VPVideo)) {
      
      
            //
            // BUGBUG, perform some verification tests here!!!
            //

            fOK = TRUE;
            break;
        }  // End of Video port section
        
        // -------------------------------------------------------------------
        // Specifier KSDATAFORMAT_SPECIFIER_NONE for VP VBI
        // -------------------------------------------------------------------

        else if (IsEqualGUID (pKSDataFormatToVerify->Specifier, 
                      KSDATAFORMAT_SPECIFIER_NONE) &&
                      IsEqualGUID (pKSDataFormatToVerify->SubFormat, KSDATAFORMAT_SUBTYPE_VPVBI)) {
            //
            // BUGBUG, perform some verification tests here!!!
            //

            fOK = TRUE;
            break;
        }  // End of VP VBI section
      
    } // End of loop on all formats for this stream
    
    return fOK;
}

/*
** AdapterFormatFromRange()
**
**   Returns a DATAFORMAT from a DATARANGE
**
** Arguments:
**
**         IN PHW_STREAM_REQUEST_BLOCK pSrb 
**
** Returns:
** 
**   TRUE if the format is supported
**   FALSE if the format cannot be suppored
**
** Side Effects:  none
*/

BOOL AdapterFormatFromRange(IN PHW_STREAM_REQUEST_BLOCK pSrb)
{
    PSTREAM_DATA_INTERSECT_INFO IntersectInfo;
    PKSDATARANGE                DataRange;
    BOOL                        OnlyWantsSize;
    BOOL                        MatchFound = FALSE;
    ULONG                       FormatSize;
    ULONG                       StreamNumber;
    ULONG                       j;
    ULONG                       NumberOfFormatArrayEntries;
    PKSDATAFORMAT               *pAvailableFormats;

    IntersectInfo = pSrb->CommandData.IntersectInfo;
    StreamNumber = IntersectInfo->StreamNumber;
    DataRange = IntersectInfo->DataRange;

    //
    // Check that the stream number is valid
    //

    if (StreamNumber >= DRIVER_STREAM_COUNT) {
        pSrb->Status = STATUS_NOT_IMPLEMENTED;
        TRAP();
        return FALSE;
    }
    
    NumberOfFormatArrayEntries = 
            Streams[StreamNumber].hwStreamInfo.NumberOfFormatArrayEntries;

    //
    // Get the pointer to the array of available formats
    //

    pAvailableFormats = Streams[StreamNumber].hwStreamInfo.StreamFormatsArray;

    //
    // Is the caller trying to get the format, or the size of the format?
    //

    OnlyWantsSize = (IntersectInfo->SizeOfDataFormatBuffer == sizeof(ULONG));

    //
    // Walk the formats supported by the stream searching for a match
    // of the three GUIDs which together define a DATARANGE
    //

    for (j = 0; j < NumberOfFormatArrayEntries; j++, pAvailableFormats++) {

        if (!AdapterCompareGUIDsAndFormatSize(
                        DataRange, 
                        *pAvailableFormats,
                        TRUE /* CompareFormatSize */)) {
            continue;
        }

        //
        // Now that the three GUIDs match, switch on the Specifier
        // to do a further type-specific check
        //

        // -------------------------------------------------------------------
        // Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
        // -------------------------------------------------------------------

        if (IsEqualGUID (DataRange->Specifier, 
                KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {
                
            PKS_DATARANGE_VIDEO DataRangeVideoToVerify = 
                    (PKS_DATARANGE_VIDEO) DataRange;
            PKS_DATARANGE_VIDEO DataRangeVideo = 
                    (PKS_DATARANGE_VIDEO) *pAvailableFormats;
            PKS_DATAFORMAT_VIDEOINFOHEADER DataFormatVideoInfoHeaderOut;

            //
            // Check that the other fields match
            //
            if ((DataRangeVideoToVerify->bFixedSizeSamples != DataRangeVideo->bFixedSizeSamples) ||
                (DataRangeVideoToVerify->bTemporalCompression != DataRangeVideo->bTemporalCompression) ||
                (DataRangeVideoToVerify->StreamDescriptionFlags != DataRangeVideo->StreamDescriptionFlags) ||
                (DataRangeVideoToVerify->MemoryAllocationFlags != DataRangeVideo->MemoryAllocationFlags) ||
                (RtlCompareMemory (&DataRangeVideoToVerify->ConfigCaps,
                        &DataRangeVideo->ConfigCaps,
                        sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)) != 
                        sizeof (KS_VIDEO_STREAM_CONFIG_CAPS))) {
                continue;
            }
            
            // MATCH FOUND!
            MatchFound = TRUE;            
            FormatSize = sizeof (KSDATAFORMAT) + 
                KS_SIZE_VIDEOHEADER (&DataRangeVideoToVerify->VideoInfoHeader);

            if (OnlyWantsSize) {
                break;
            }
            
            // Caller wants the full data format
            if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize) {
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

            // Copy over the KSDATAFORMAT, followed by the 
            // actual VideoInfoHeader
                
            DataFormatVideoInfoHeaderOut = (PKS_DATAFORMAT_VIDEOINFOHEADER) IntersectInfo->DataFormatBuffer;

            // Copy over the KSDATAFORMAT 
            RtlCopyMemory(
                &DataFormatVideoInfoHeaderOut->DataFormat,
                &DataRangeVideoToVerify->DataRange,
                sizeof (KSDATARANGE));

            DataFormatVideoInfoHeaderOut->DataFormat.FormatSize = FormatSize;

            // Copy over the callers requested VIDEOINFOHEADER

            RtlCopyMemory(
                &DataFormatVideoInfoHeaderOut->VideoInfoHeader,
                &DataRangeVideoToVerify->VideoInfoHeader,
                KS_SIZE_VIDEOHEADER (&DataRangeVideoToVerify->VideoInfoHeader));

            // Calculate biSizeImage for this request, and put the result in both
            // the biSizeImage field of the bmiHeader AND in the SampleSize field
            // of the DataFormat.
            //
            // Note that for compressed sizes, this calculation will probably not
            // be just width * height * bitdepth

            DataFormatVideoInfoHeaderOut->VideoInfoHeader.bmiHeader.biSizeImage =
                DataFormatVideoInfoHeaderOut->DataFormat.SampleSize = 
                KS_DIBSIZE(DataFormatVideoInfoHeaderOut->VideoInfoHeader.bmiHeader);

            //
            // BUGBUG Perform other validation such as cropping and scaling checks
            // 

            break;

        } // End of VIDEOINFOHEADER specifier

        // -------------------------------------------------------------------
        // Specifier FORMAT_AnalogVideo for KS_ANALOGVIDEOINFO
        // -------------------------------------------------------------------

        else if (IsEqualGUID (DataRange->Specifier, 
                KSDATAFORMAT_SPECIFIER_ANALOGVIDEO)) {
      
            //
            // For analog video, the DataRange and DataFormat
            // are identical, so just copy the whole structure
            //

            PKS_DATARANGE_ANALOGVIDEO DataRangeVideo = 
                    (PKS_DATARANGE_ANALOGVIDEO) *pAvailableFormats;

            // MATCH FOUND!
            MatchFound = TRUE;            
            FormatSize = sizeof (KS_DATARANGE_ANALOGVIDEO);

            if (OnlyWantsSize) {
                break;
            }
            
            // Caller wants the full data format
            if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize) {
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

            RtlCopyMemory(
                IntersectInfo->DataFormatBuffer,
                DataRangeVideo,
                sizeof (KS_DATARANGE_ANALOGVIDEO));

            ((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;

            break;

        } // End of KS_ANALOGVIDEOINFO specifier

        // -------------------------------------------------------------------
        // Specifier STATIC_KSDATAFORMAT_TYPE_VIDEO for Video Port
        // -------------------------------------------------------------------

        else if (IsEqualGUID (DataRange->Specifier, 
                      KSDATAFORMAT_SPECIFIER_NONE) &&
                      IsEqualGUID (DataRange->SubFormat, KSDATAFORMAT_SUBTYPE_VPVideo)) {
      
      
            // MATCH FOUND!
            MatchFound = TRUE;            
            FormatSize = sizeof (KSDATAFORMAT);

            if (OnlyWantsSize) {
                break;
            }
            
            // Caller wants the full data format
            if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize) {
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

   
            RtlCopyMemory(
            IntersectInfo->DataFormatBuffer,
            &StreamFormatVideoPort,
            sizeof (KSDATAFORMAT));

            ((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;

            break;
        }

        // -------------------------------------------------------------------
        // Specifier KSDATAFORMAT_SPECIFIER_NONE for VP VBI
        // -------------------------------------------------------------------

        else if (IsEqualGUID (DataRange->Specifier, 
                      KSDATAFORMAT_SPECIFIER_NONE) &&
                      IsEqualGUID (DataRange->SubFormat, KSDATAFORMAT_SUBTYPE_VPVBI)) {
      
            // MATCH FOUND!
            MatchFound = TRUE;            
            FormatSize = sizeof (KSDATAFORMAT);

            if (OnlyWantsSize) {
                break;
            }
            
            // Caller wants the full data format
            if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize) {
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

   
            RtlCopyMemory(
            IntersectInfo->DataFormatBuffer,
            &StreamFormatVideoPortVBI,
            sizeof (KSDATAFORMAT));

            ((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;

            break;
        }

        // -------------------------------------------------------------------
        // Specifier STATIC_KSDATAFORMAT_TYPE_NONE for VBI capture stream
        // -------------------------------------------------------------------

        else if (IsEqualGUID (DataRange->Specifier, 
                      KSDATAFORMAT_SPECIFIER_VBI)) {
      
            PKS_DATARANGE_VIDEO_VBI DataRangeVBIToVerify = 
                    (PKS_DATARANGE_VIDEO_VBI) DataRange;
            PKS_DATARANGE_VIDEO_VBI DataRangeVBI = 
                    (PKS_DATARANGE_VIDEO_VBI) *pAvailableFormats;

            //
            // Check that the other fields match
            //
            if ((DataRangeVBIToVerify->bFixedSizeSamples != DataRangeVBI->bFixedSizeSamples) ||
                (DataRangeVBIToVerify->bTemporalCompression != DataRangeVBI->bTemporalCompression) ||
                (DataRangeVBIToVerify->StreamDescriptionFlags != DataRangeVBI->StreamDescriptionFlags) ||
                (DataRangeVBIToVerify->MemoryAllocationFlags != DataRangeVBI->MemoryAllocationFlags) ||
                (RtlCompareMemory (&DataRangeVBIToVerify->ConfigCaps,
                        &DataRangeVBI->ConfigCaps,
                        sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)) != 
                        sizeof (KS_VIDEO_STREAM_CONFIG_CAPS))) {
                continue;
            }
            
            // MATCH FOUND!
            MatchFound = TRUE;            
            FormatSize = sizeof (KS_DATAFORMAT_VBIINFOHEADER);

            if (OnlyWantsSize) {
                break;
            }
            
            // Caller wants the full data format
            if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize) {
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

            // Copy over the KSDATAFORMAT, followed by the 
            // actual VBIInfoHeader
                
            RtlCopyMemory(
                &((PKS_DATAFORMAT_VBIINFOHEADER)IntersectInfo->DataFormatBuffer)->DataFormat,
                &DataRangeVBIToVerify->DataRange,
                sizeof (KSDATARANGE));

            ((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;

            RtlCopyMemory(
                &((PKS_DATAFORMAT_VBIINFOHEADER) IntersectInfo->DataFormatBuffer)->VBIInfoHeader,
                &DataRangeVBIToVerify->VBIInfoHeader,
                sizeof (KS_VBIINFOHEADER));
        }

        else {
            pSrb->Status = STATUS_NO_MATCH;
            return FALSE;
        }

    } // End of loop on all formats for this stream
    
    if (OnlyWantsSize) {
        *(PULONG) IntersectInfo->DataFormatBuffer = FormatSize;
        pSrb->ActualBytesTransferred = sizeof(ULONG);
        return TRUE;
    }        
    pSrb->ActualBytesTransferred = FormatSize;
    return TRUE;
}


NTSTATUS STREAMAPI VPStreamEventProc (PHW_EVENT_DESCRIPTOR pEvent)
{
    PSTREAMEX pstrm=(PSTREAMEX)(pEvent->StreamObject->HwStreamExtension);

    if (pEvent->Enable)
    {
        pstrm->EventCount++;
    }
    else
    {
        pstrm->EventCount--;
    }
    return STATUS_SUCCESS;
}

NTSTATUS STREAMAPI VPVBIStreamEventProc (PHW_EVENT_DESCRIPTOR pEvent)
{
    PSTREAMEX pstrm=(PSTREAMEX)(pEvent->StreamObject->HwStreamExtension);

    if (pEvent->Enable)
    {
        pstrm->EventCount++;
    }
    else
    {
        pstrm->EventCount--;
    }
    return STATUS_SUCCESS;
}

BOOL CreateStreamThread(PSTREAMEX   pStrmEx)
{
    NTSTATUS status;

    status = PsCreateSystemThread(  &pStrmEx->threadHandle,
                                    (ACCESS_MASK) 0L,
                                    NULL,
                                    NULL,
                                    NULL,
                                    StreamThread,
                                    pStrmEx);
    
    return (status == STATUS_SUCCESS);
}

