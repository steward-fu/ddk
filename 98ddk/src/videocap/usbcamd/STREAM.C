/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

   stream.c

Abstract:

    contains all the code that interfaces with the WDM stream class driver.

Author:


Environment:

   Kernel mode only


Revision History:

--*/

#include "usbcamd.h"

VOID STREAMAPI
AdapterReceivePacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
AdapterTimeoutPacket(
    PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
AdapterCancelPacket(
    PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID STREAMAPI
USBCAMD_ReceiveDataPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID STREAMAPI
USBCAMD_ReceiveCtrlPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID 
AdapterCloseStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );    

VOID
AdapterOpenStream(
    PHW_STREAM_REQUEST_BLOCK Srb
    );

VOID
AdapterStreamInfo(
    PHW_STREAM_REQUEST_BLOCK Srb
    );

#define COMPLETE_STREAM_READ(s) \
        StreamClassStreamNotification(StreamRequestComplete,\
                                      (s)->StreamObject,\
                                      (s))    

//
// camera ref count -- when this count goes to zero we
// should unload.
//

ULONG USBCAMD_CameraFRC = 0;

#if DBG
ULONG USBCAMD_HeapCount = 0;
#endif


//---------------------------------------------------------------------------
// Topology
//---------------------------------------------------------------------------

// Categories define what the device does.

static GUID Categories[] = {
    STATIC_KSCATEGORY_VIDEO,
    STATIC_KSCATEGORY_CAPTURE
};

#define NUMBER_OF_CATEGORIES  SIZEOF_ARRAY (Categories)

static KSTOPOLOGY Topology = {
    NUMBER_OF_CATEGORIES,
    (GUID*) &Categories,
    0,
    NULL,
    0,
    NULL
};

// ------------------------------------------------------------------------
// Property sets for all video capture streams
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_TABLE(VideoStreamConnectionProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CONNECTION_ALLOCATORFRAMING,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KSALLOCATOR_FRAMING),            // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        sizeof(ULONG)                           // SerializedSize
    ),
};

DEFINE_KSPROPERTY_TABLE(VideoStreamDroppedFramesProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_DROPPEDFRAMES_CURRENT,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_DROPPEDFRAMES_CURRENT_S),// MinProperty
        sizeof(KSPROPERTY_DROPPEDFRAMES_CURRENT_S),// MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
};


// ------------------------------------------------------------------------
// Array of all of the property sets supported by video streams
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_SET_TABLE(VideoStreamProperties)
{
    DEFINE_KSPROPERTY_SET
    ( 
        &KSPROPSETID_Connection,                        // Set
        SIZEOF_ARRAY(VideoStreamConnectionProperties),  // PropertiesCount
        VideoStreamConnectionProperties,                // PropertyItem
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_DROPPEDFRAMES,                // Set
        SIZEOF_ARRAY(VideoStreamDroppedFramesProperties),  // PropertiesCount
        VideoStreamDroppedFramesProperties,                // PropertyItem
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),
};

#define NUMBER_VIDEO_STREAM_PROPERTIES (SIZEOF_ARRAY(VideoStreamProperties))





ULONG
DriverEntry(
    PVOID Context1,
    PVOID Context2
    )
{
    // BUGBUG this function is not used
    return STATUS_SUCCESS;
}

/*
** DriverEntry()
**
** This routine is called when the mini driver is first loaded.  The driver
** should then call the StreamClassRegisterAdapter function to register with
** the stream class driver
**
** Arguments:
**
**  Context1:  The context arguments are private plug and play structures
**             used by the stream class driver to find the resources for this
**             adapter
**  Context2:
**
**      BUGBUG if we take the congig descriptor and the interface number
**              we can support multiple interafces
**
** Returns:
**
** This routine returns an NT_STATUS value indicating the result of the
** registration attempt. If a value other than STATUS_SUCCESS is returned, the
** minidriver will be unloaded.
**
** Side Effects:  none
*/

ULONG
USBCAMD_DriverEntry(
    PVOID Context1,
    PVOID Context2,
    ULONG DeviceContextSize,
    ULONG FrameContextSize,
    PADAPTER_RECEIVE_PACKET_ROUTINE AdapterReceivePacket
    )
{

    // Hardware Initialization data structure
    HW_INITIALIZATION_DATA hwInitData;

    USBCAMD_KdPrint(MAX_TRACE, ("'Enter DriverEntry\n"));

    // Note: all unused fields should be zero

    hwInitData.HwInitializationDataSize = sizeof(hwInitData);

    // Entry points for the mini Driver.

//	hwInitData.HwUnInitialize 	= HwUnInitialize;   // hardware uninit routine
    hwInitData.HwInterrupt = NULL;  // IRQ handling routine

    //
    // data handling routines
    //

    hwInitData.HwReceivePacket = AdapterReceivePacket;
    hwInitData.HwCancelPacket = AdapterCancelPacket;
    hwInitData.HwRequestTimeoutHandler = AdapterTimeoutPacket;

    // Sizes for data structure extensions.  See mpinit.h for definitions

    hwInitData.DeviceExtensionSize = sizeof(USBCAMD_DEVICE_EXTENSION) +
        DeviceContextSize;
    hwInitData.PerRequestExtensionSize = sizeof(USBCAMD_READ_EXTENSION) + 
        FrameContextSize;
    hwInitData.FilterInstanceExtensionSize = 0;
    hwInitData.PerStreamExtensionSize = sizeof(USBCAMD_CHANNEL_EXTENSION);

    // We do not use DMA in our driver,
    // since it does not use the hardware directly.

    hwInitData.BusMasterDMA = FALSE;
    hwInitData.Dma24BitAddresses = FALSE;
    hwInitData.DmaBufferSize = 0;
    hwInitData.BufferAlignment = 3;

    // Turn off synchronization - we support re-entrancy.

    hwInitData.TurnOffSynchronization = TRUE;

    //
    // attempt to register with the streaming class driver.  Note, this will
    // result in calls to the HwReceivePacket routine.
    //

    USBCAMD_CameraFRC++;

    return (StreamClassRegisterAdapter(Context1,
                                       Context2,
                                       &hwInitData));

}

/*
** HwInitialize()
**
**   Initializes an adapter accessed through the information provided in the
**   ConfigInfo structure
**
** Arguments:
**
**   SRB - pointer to the request packet for the initialise command
**
**    ->ConfigInfo - provides the I/O port, memory windows, IRQ, and DMA levels
**                that should be used to access this instance of the device
**
** Returns:
**
**       STATUS_SUCCESS - if the card initializes correctly
**       STATUS_NO_SUCH_DEVICE - or other if the card is not found, or does
**                               not initialize correctly.
**
**
** Side Effects:  none
*/

NTSTATUS 
HwInitialize(
    IN PHW_STREAM_REQUEST_BLOCK Srb,
    IN PUSBCAMD_DEVICE_DATA DeviceData   
    )
{
    PPORT_CONFIGURATION_INFORMATION configInfo = Srb->CommandData.ConfigInfo;
    NTSTATUS ntStatus;
    PDEVICE_OBJECT deviceObject = configInfo->ClassDeviceObject;
    PUSBCAMD_DEVICE_EXTENSION deviceExtension =
        (PUSBCAMD_DEVICE_EXTENSION) configInfo->HwDeviceExtension;
    PDEVICE_OBJECT physicalDeviceObject = configInfo->PhysicalDeviceObject;

    USBCAMD_KdPrint(MAX_TRACE, ("'Enter HwInitialize\n"));

    if (configInfo->NumberOfAccessRanges > 0) {
        TRAP();
        USBCAMD_KdPrint(MIN_TRACE, ("'illegal config info"));

        return (STATUS_NO_SUCH_DEVICE);
    }
    // Initialize flags for the device object
    deviceObject->Flags |= DO_DIRECT_IO;
    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    //
    // remember the Physical device Object for apis to the
    // usb stack
    //
    deviceExtension->StackDeviceObject = physicalDeviceObject;
    deviceExtension->Sig = USBCAMD_EXTENSION_SIG;
    deviceExtension->DeviceData = *DeviceData;
    deviceExtension->ChannelExtension = NULL;
    deviceExtension->CurrentPowerState = PowerDeviceD0;
    deviceExtension->Initialized = DEVICE_INIT_STARTED;
    
    //
    // Configure the USB device
    //

    ntStatus = USBCAMD_StartDevice(deviceExtension);

    //
    // initialize the size of stream descriptor information.
    // we have one stream descriptor, and we attempt to dword align the
    // structure.
    //

	configInfo->StreamDescriptorSize =
        1 * (sizeof (HW_STREAM_INFORMATION)) + // 1 stream descriptor
        sizeof (HW_STREAM_HEADER);             // and 1 stream header

    USBCAMD_KdPrint(MIN_TRACE, ("'StreamDescriptorSize = %d\n", configInfo->StreamDescriptorSize));

    InitializeListHead (&deviceExtension->StreamControlSRBList);
    KeInitializeSpinLock (&deviceExtension->ControlSRBSpinLock);
    deviceExtension->ProcessingControlSRB = FALSE;
    deviceExtension->Initialized = DEVICE_INIT_COMPLETED;
    
    return (ntStatus);
}

/*
** HwUnInitialize()
**
**   Release all resources and clean up the hardware
**
** Arguments:
**
**      DeviceExtension - pointer to the deviceextension structure for the
**                       the device to be free'd
**
** Returns:
**
** Side Effects:  none
*/

NTSTATUS 
HwUnInitialize(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PUSBCAMD_DEVICE_EXTENSION deviceExtension =
        (PUSBCAMD_DEVICE_EXTENSION) Srb->HwDeviceExtension;
    PUSBCAMD_CHANNEL_EXTENSION channelExtension = 
        deviceExtension->ChannelExtension;    
    
    //
    // BUGBUG how does this function relate to STOP and REMOVE PNP
    // messages?
    //
    
    USBCAMD_KdPrint(MIN_TRACE, ("'HwUnintialize\n"));

    //
    // delay the call to remove until every stream is closed
    //
    
    if (channelExtension &&
        channelExtension->ImageCaptureStarted) {
        USBCAMD_StopChannel(deviceExtension,
                            channelExtension);                    
//       TEST_TRAP();                            
    } 

    deviceExtension->Initialized = DEVICE_UNINITIALIZED;

    if (deviceExtension->OpenFRC == 0) {
        USBCAMD_KdPrint(MIN_TRACE, ("'HwUnintialize, remove device\n"));
    
        ntStatus = USBCAMD_RemoveDevice(deviceExtension);
    }
    
    return ntStatus;
}


/*
** AdapterCancelPacket()
**
**   Request to cancel a packet that is currently in process in the minidriver
**
** Arguments:
**
**   Srb - pointer to request packet to cancel
**
** Returns:
**
** Side Effects:  none
*/

VOID
AdapterCancelPacket(
    PHW_STREAM_REQUEST_BLOCK pSrbToCancel
    )
{
	KIRQL oldIrql;
	PLIST_ENTRY ListEntry;
	BOOL Found= FALSE;
	PUSBCAMD_READ_EXTENSION pSrbExt;
	PHW_STREAM_REQUEST_BLOCK pSrb;
	PUSBCAMD_CHANNEL_EXTENSION channelExtension = 
		(PUSBCAMD_CHANNEL_EXTENSION) pSrbToCancel->StreamObject->HwStreamExtension;

//	TEST_TRAP();
	USBCAMD_KdPrint(MAX_TRACE, ("'USBCAMD_CANCEL Packet \n"));
	// check and see if the SRB is being processed by the bus stack currently.
	if ((channelExtension->CurrentRequest != NULL) && 
		(channelExtension->CurrentRequest->Srb == pSrbToCancel )) {
			USBCAMD_FreeRawFrameBuffer(channelExtension->CurrentRequest->RawFrameBuffer);
			channelExtension->CurrentRequest = NULL;
			Found = TRUE;
		    USBCAMD_KdPrint(MIN_TRACE, ("'Current Srb %x is Cancelled\n", pSrbToCancel));

	}
	// 
	// Loop through the circular doubly linked list of pending read SRBs 
	// from the beginning to end,trying to find the SRB to cancel
	//
	else {
		KeAcquireSpinLock (&channelExtension->PendingIoListSpin, &oldIrql);
		ListEntry =  channelExtension->PendingIoList.Flink;
		while (ListEntry != &channelExtension->PendingIoList) {
			pSrbExt = CONTAINING_RECORD(ListEntry, USBCAMD_READ_EXTENSION,ListEntry);
			pSrb = pSrbExt->Srb;
			if (pSrb == pSrbToCancel) {
				USBCAMD_FreeRawFrameBuffer (pSrbExt->RawFrameBuffer);
				RemoveEntryList(ListEntry);
	            USBCAMD_KdPrint(MIN_TRACE, ("'Queued Srb %x is Cancelled\n", pSrbToCancel));
				Found = TRUE;
				break;
			}
			ListEntry = ListEntry->Flink;
		}
		KeReleaseSpinLock (&channelExtension->PendingIoListSpin, oldIrql);
	}

	if (Found) {
        //
        // Make the length zero, and status cancelled
        //
		
		pSrbToCancel->CommandData.DataBufferArray->DataUsed =0;
		pSrbToCancel->Status = STATUS_CANCELLED;
		COMPLETE_STREAM_READ(pSrbToCancel);
	}
    else {
        USBCAMD_KdPrint(MIN_TRACE, ("'Srb %x not found\n", pSrbToCancel));            
    	// TEST_TRAP();
    }
}

/*
** AdapterTimeoutPacket()
**
**   This routine is called when a packet has been in the minidriver for
**   too long.  The adapter must decide what to do with the packet
**
** Arguments:
**
**   Srb - pointer to the request packet that timed out
**
** Returns:
**
** Side Effects:  none
*/

VOID 
AdapterTimeoutPacket(
    PHW_STREAM_REQUEST_BLOCK Srb
    )
{
    PUSBCAMD_DEVICE_EXTENSION deviceExtension =
        (PUSBCAMD_DEVICE_EXTENSION) Srb->HwDeviceExtension;
    PUSBCAMD_CHANNEL_EXTENSION channelExtension = 
        deviceExtension->ChannelExtension;    

    //
    // schedule work item to perform a reset only if we are in a KSSTATE_RUN.
    // Stream class is agonostic about stream state when it fires off this timeout call.

    if (deviceExtension->Initialized & DEVICE_INIT_COMPLETED) {
        if ( channelExtension && 
            (channelExtension->KSState == KSSTATE_RUN)) {
	        USBCAMD_KdPrint(MIN_TRACE, ("'SRB %x Timed out \n", Srb));
			USBCAMD_ProcessResetRequest(deviceExtension,
                                    channelExtension);
        }
    }                                    

    Srb->TimeoutCounter = Srb->TimeoutOriginal;
}


/*
** AdapterReceivePacket()
**
**   Main entry point for receiving adapter based request SRBs.  This routine
**   will always be called at High Priority.
**
**   Note: This is an asynchronous entry point.  The request does not complete
**         on return from this function, the request only completes when a
**         StreamClassDeviceNotification on this request block, of type
**         DeviceRequestComplete, is issued.
**
** Arguments:
**
**   Srb - Pointer to the STREAM_REQUEST_BLOCK
**        Srb->HwDeviceExtension - will be the hardware device extension for
**                                 as initialized in HwInitialize
**
** Returns:
**
** Side Effects:  none
*/

PVOID
USBCAMD_AdapterReceivePacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb,
    IN PUSBCAMD_DEVICE_DATA DeviceData,
    IN PDEVICE_OBJECT *deviceObject,
    IN BOOLEAN NeedsCompletion
    )
{
    PUSBCAMD_DEVICE_EXTENSION deviceExtension =
        (PUSBCAMD_DEVICE_EXTENSION) Srb->HwDeviceExtension;
    //
    // determine the type of packet.
    //

    USBCAMD_KdPrint(MIN_TRACE, ("'USBCAMD_ReceivePacket command = %x\n", Srb->Command));

    if (deviceObject) {
        *deviceObject = deviceExtension->StackDeviceObject;
    }

    if (!NeedsCompletion) {
        //
        // the cam driver will handled it, just return
        //
        return USBCAMD_GET_DEVICE_CONTEXT(deviceExtension);
    }

    switch (Srb->Command) {

    case SRB_OPEN_STREAM:

        //
        // this is a request to open a specified stream.
        //
        
        USBCAMD_KdPrint(MAX_TRACE, ("'SRB_OPEN_STREAM\n"));
        AdapterOpenStream(Srb);
        break;

    case SRB_GET_STREAM_INFO:

        //
        // this is a request for the driver to enumerate requested streams
        //
        
        USBCAMD_KdPrint(MAX_TRACE, ("'SRB_GET_STREAM_INFO\n"));
        AdapterStreamInfo(Srb);
        break;

    case SRB_INITIALIZE_DEVICE:

        USBCAMD_KdPrint(MAX_TRACE, ("'SRB_INITIALIZE_DEVICE\n"));
        Srb->Status = HwInitialize(Srb, DeviceData);
        break;

    case SRB_UNINITIALIZE_DEVICE:
    
        USBCAMD_KdPrint(MAX_TRACE, ("'SRB_UNINITIALIZE_DEVICE\n"));
        Srb->Status = HwUnInitialize(Srb);
        break;

    case SRB_CHANGE_POWER_STATE:
    
        USBCAMD_KdPrint(MAX_TRACE, ("'SRB_CHANGE_POWER_STATE\n"));
        Srb->Status = USBCAMD_SetDevicePowerState(deviceExtension,Srb);
        break;

    case SRB_PAGING_OUT_DRIVER:
        USBCAMD_KdPrint(MAX_TRACE, ("'SRB_PAGING_OUT_DRIVER\n"));
		Srb->Status = STATUS_SUCCESS;
        break;

    case SRB_CLOSE_STREAM:
    
        USBCAMD_KdPrint(MAX_TRACE, ("'SRB_CLOSE_STREAM\n"));
        AdapterCloseStream(Srb);
        break;

    case SRB_UNKNOWN_DEVICE_COMMAND:
    
        {
#if DBG        
        PIRP irp;
        PIO_STACK_LOCATION ioStackLocation;

        irp = Srb->Irp;
        ioStackLocation = IoGetCurrentIrpStackLocation(irp);

        DbgPrint("USBCAMD: Stream class did not translate IRP_MJ = 0x%x IRP_MN = 0x%x\n",
                    ioStackLocation->MajorFunction,
                    ioStackLocation->MinorFunction);
#endif                    
        USBCAMD_KdPrint(MIN_TRACE, ("'SRB_UNKNOWN_DEVICE_COMMAND %x\n", Srb->Command));                       
        Srb->Status = STATUS_NOT_IMPLEMENTED;        
        }
        break;
        
    default:

        USBCAMD_KdPrint(MIN_TRACE, ("'Unknown SRB command %x\n", Srb->Command));        

        //
        // this is a request that we do not understand.  Indicate invalid
        // command and complete the request
        //

        Srb->Status = STATUS_NOT_IMPLEMENTED;
    }

    //
    // all commands complete synchronously
    //
    
    StreamClassDeviceNotification(DeviceRequestComplete,
                                  Srb->HwDeviceExtension,
                                  Srb);

    return USBCAMD_GET_DEVICE_CONTEXT(deviceExtension);
}


/*
** AdapterStreamInfo()
**
**   Returns the information of all streams that are supported by the
**   mini-driver
**
** Arguments:
**
**   Srb - Pointer to the STREAM_REQUEST_BLOCK
**        Srb->HwDeviceExtension - will be the hardware device extension for
**                                  as initialised in HwInitialise
**
** Returns:
**
** Side Effects:  none
*/

VOID
AdapterStreamInfo(
    PHW_STREAM_REQUEST_BLOCK Srb
    )
{

    //
    // pick up the pointer to the stream information data structure
    //

    PHW_STREAM_INFORMATION streamInformation =
        &(Srb->CommandData.StreamBuffer->StreamInfo);

    PHW_STREAM_HEADER streamHeader =
        &(Srb->CommandData.StreamBuffer->StreamHeader);        
        
    PUSBCAMD_DEVICE_EXTENSION deviceExtension =
        (PUSBCAMD_DEVICE_EXTENSION) Srb->HwDeviceExtension;

    USBCAMD_KdPrint(MIN_TRACE, ("'AdapterStreamInfo\n"));        

    //
    // set number of streams
    //

    ASSERT (Srb->NumberOfBytesToTransfer >= 
            sizeof (HW_STREAM_HEADER) +
            sizeof (HW_STREAM_INFORMATION));

    //
    // initialize stream header
    //

    streamHeader->SizeOfHwStreamInformation = sizeof(HW_STREAM_INFORMATION);

    //
    // initialize the stream information.
    //
    // The NumberOfInstances field indicates the number of concurrent
    // streamsof this type the device can support.
    //

    streamInformation->NumberOfPossibleInstances = 1;

    //
    // indicates the direction of data flow for this stream, relative to the
    // driver
    //

    streamInformation->DataFlow = KSPIN_DATAFLOW_OUT;

    //
    // dataAccessible - Indicates whether the data is "seen" by the host
    // processor.
    //

    streamInformation->DataAccessible = TRUE;

    //
    // store a pointer to the topology for the device
    //
        
    Srb->CommandData.StreamBuffer->StreamHeader.Topology = &Topology;

    //
    // indicate the pin name
    //
        
    streamInformation->Name = (GUID *)&PINNAME_VIDEO_CAPTURE;

    //
    // indicate the pin function
    //
        
    streamInformation->Category = (GUID *)&PINNAME_VIDEO_CAPTURE;


    streamInformation->StreamPropertiesArray = 
             (PKSPROPERTY_SET) VideoStreamProperties;
    streamInformation->NumStreamPropArrayEntries = NUMBER_VIDEO_STREAM_PROPERTIES; 
                 
    //
    // indicate success
    //

    Srb->Status = STATUS_SUCCESS;
}

/*
** AdapterOpenStream()
**
**   This routine is called when an OpenStream SRB request is received
**
** Arguments:
**
**   Srb - pointer to stream request block for the Open command
**
** Returns:
**
** Side Effects:  none
*/

VOID
AdapterOpenStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb)
{
    //
    // the stream extension structure is allocated by the stream class driver
    //

    PUSBCAMD_CHANNEL_EXTENSION channelExtension =
        (PUSBCAMD_CHANNEL_EXTENSION) Srb->StreamObject->HwStreamExtension;
    PUSBCAMD_DEVICE_EXTENSION deviceExtension =
        (PUSBCAMD_DEVICE_EXTENSION) Srb->HwDeviceExtension;
    NTSTATUS status = STATUS_SUCCESS;      
    ULONG nSize;
    PKS_DATAFORMAT_VIDEOINFOHEADER  pKSDataFormat = 
                (PKS_DATAFORMAT_VIDEOINFOHEADER) Srb->CommandData.OpenFormat;
    PKS_VIDEOINFOHEADER     pVideoInfoHdrRequested = 
                &pKSDataFormat->VideoInfoHeader;



    USBCAMD_KdPrint(MIN_TRACE, ("'AdapterOpenStream\n"));
    
    //
    // determine which stream number is being opened.  This number indicates
    // the offset into the array of streaminfo structures that was filled out
    // in the AdapterStreamInfo call.
    //

    // save the channel extension for remove
    deviceExtension->ChannelExtension = 
        channelExtension;        

    status = USBCAMD_OpenChannel(deviceExtension, 
                                 channelExtension, 
                                 Srb->CommandData.OpenFormat);

    if (NT_SUCCESS(status)) {                                   
        //
        // this gets the bandwidth and memory we will need
        // for iso video streaming.
        //
        status = USBCAMD_PrepareChannel(deviceExtension, 
                                        channelExtension);
    }

    //
    // srb has been to the mini driver
    // 
    // save their routines
    channelExtension->CamReceiveDataPacket = (PSTREAM_RECEIVE_PACKET)
        Srb->StreamObject->ReceiveDataPacket;
    channelExtension->CamReceiveCtrlPacket = (PSTREAM_RECEIVE_PACKET)
        Srb->StreamObject->ReceiveControlPacket;

    channelExtension->KSState = KSSTATE_STOP;        

    Srb->StreamObject->ReceiveDataPacket = (PVOID) USBCAMD_ReceiveDataPacket;
    Srb->StreamObject->ReceiveControlPacket = (PVOID) USBCAMD_ReceiveCtrlPacket;

    Srb->StreamObject->HwClockObject.HwClockFunction = NULL;
	Srb->StreamObject->HwClockObject.ClockSupportFlags = 0;

    nSize = sizeof (KS_VIDEOINFOHEADER) + 
                pVideoInfoHdrRequested->bmiHeader.biSize;
    channelExtension->VideoInfoHeader = 
        USBCAMD_ExAllocatePool(NonPagedPool, nSize);

    if (channelExtension->VideoInfoHeader == NULL) {
        Srb->Status = STATUS_INSUFFICIENT_RESOURCES;
        return;
    }

    // Copy the VIDEOINFOHEADER requested to our storage
    RtlCopyMemory(
            channelExtension->VideoInfoHeader,
            pVideoInfoHdrRequested,
            nSize);
            

	USBCAMD_KdPrint(MIN_TRACE, ("USBCAMD: VideoInfoHdrRequested \n"));
    USBCAMD_KdPrint(MIN_TRACE, ("Width=%d  Height=%d  FrameTime (ms)= %d\n",
								pVideoInfoHdrRequested->bmiHeader.biWidth,
								pVideoInfoHdrRequested->bmiHeader.biHeight,
								pVideoInfoHdrRequested->AvgTimePerFrame/10000));

    // We don't use DMA.

    Srb->StreamObject->Dma = FALSE;

    
    Srb->StreamObject->StreamHeaderMediaSpecific = 
        sizeof(KS_FRAME_INFO);


    //
    // The PIO flag must be set when the mini driver will be accessing the
    // data
    // buffers passed in using logical addressing
    //
#if 0
    Srb->StreamObject->Pio = FALSE;
#else
    Srb->StreamObject->Pio = TRUE;
#endif

    Srb->Status = status;
}

/*
** AdapterCloseStream()
**
**   Close the requested data stream
**
** Arguments:
**
**   Srb the request block requesting to close the stream
**
** Returns:
**
** Side Effects:  none
*/

VOID 
AdapterCloseStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )
{

    PUSBCAMD_CHANNEL_EXTENSION channelExtension =
        (PUSBCAMD_CHANNEL_EXTENSION) Srb->StreamObject->HwStreamExtension;
    PUSBCAMD_DEVICE_EXTENSION deviceExtension =
        (PUSBCAMD_DEVICE_EXTENSION) Srb->HwDeviceExtension;
    NTSTATUS status = STATUS_SUCCESS;
    
    USBCAMD_KdPrint(MIN_TRACE, ("'AdapterCloseStream\n"));

    ASSERT_CHANNEL(channelExtension);
    //
    // stop streaming capture
    //
    if (channelExtension->ImageCaptureStarted) {
//        TEST_TRAP();
        USBCAMD_StopChannel(deviceExtension,
                            channelExtension);                    
    } 

    if (channelExtension->ChannelPrepared) {
        //
        // Free memory and bandwidth
        //
        USBCAMD_UnPrepareChannel(deviceExtension, 
                                 channelExtension);
    }

    status = USBCAMD_CloseChannel(deviceExtension, 
                                  channelExtension);

    if (channelExtension->VideoInfoHeader) { 
        USBCAMD_ExFreePool(channelExtension->VideoInfoHeader);
    }        

    // 
    // we no longer have a channel
    //
    deviceExtension->ChannelExtension = NULL;
    
    //
    // see if we need to remove the camera
    //
    if (deviceExtension->OpenFRC == 0 &&
        !deviceExtension->Initialized) {

        USBCAMD_KdPrint(MIN_TRACE, ("'AdapterCloseStream, remove device\n"));
        USBCAMD_RemoveDevice(deviceExtension);
    }        

    Srb->Status = status;
}


VOID STREAMAPI
USBCAMD_ReceiveDataPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )
{
    PUSBCAMD_DEVICE_EXTENSION deviceExtension =
        (PUSBCAMD_DEVICE_EXTENSION) Srb->HwDeviceExtension;
    PUSBCAMD_CHANNEL_EXTENSION channelExtension =
        (PUSBCAMD_CHANNEL_EXTENSION) Srb->StreamObject->HwStreamExtension;        
    PUSBCAMD_READ_EXTENSION readExtension =
        (PUSBCAMD_READ_EXTENSION) Srb->SRBExtension;    
    BOOLEAN completedByCam = FALSE;        
    PKSSTREAM_HEADER streamHeader;

    USBCAMD_KdPrint(MAX_TRACE, ("'USBCAMD_ReceiveDataPacket\n"));

    //
    // determine the type of packet.
    //

    //
    // call the cam driver first
    //
    if (channelExtension->CamReceiveDataPacket) {
        (*channelExtension->CamReceiveDataPacket)(
            Srb,
            USBCAMD_GET_DEVICE_CONTEXT(deviceExtension),
            &completedByCam);
    }        

    if (completedByCam == TRUE) {
        return;
    }
    
    switch (Srb->Command) {
    case SRB_READ_DATA:

        // initialize the SRB extension
       
		// Rule: 
        // Only accept read requests when in either the Pause or Run
        // States.  If Stopped, immediately return the SRB.

        if (channelExtension->KSState == KSSTATE_STOP) {
            Srb->Status = STATUS_SUCCESS;
            COMPLETE_STREAM_READ(Srb);
            break;
        } 

        readExtension->Srb = (PVOID) Srb;
        readExtension->Irp = Srb->Irp;
        readExtension->Sig = USBCAMD_READ_SIG;

        // Queue the read to the camera driver
        // This request will be completed asynchronously...

        USBCAMD_KdPrint(MIN_TRACE, ("'SRB_READ_DATA\n"));

        // make sure that the buffer passed down from DirectShow is Bigger or equal the one
        // in biSizeImage associated with open stream.

        streamHeader = ((PHW_STREAM_REQUEST_BLOCK) Srb)->CommandData.DataBufferArray;

        if ( streamHeader->FrameExtent < channelExtension->VideoInfoHeader->bmiHeader.biSizeImage) {
           Srb->Status = STATUS_INSUFFICIENT_RESOURCES;
           USBCAMD_KdPrint(MIN_TRACE, ("Frame buffer (%d)< biSizeImage (%d)\n",
                         streamHeader->FrameExtent, 
                         channelExtension->VideoInfoHeader->bmiHeader.biSizeImage ));
           COMPLETE_STREAM_READ(Srb);
           break;
        }

        Srb->TimeoutCounter = Srb->TimeoutOriginal = 5;

        Srb->Status = 
            USBCAMD_ReadChannel(deviceExtension,
                                channelExtension,
                                readExtension);

        if (!NT_SUCCESS(Srb->Status)) {
            COMPLETE_STREAM_READ(Srb);
        }

        break;

    default:

        //
        // invalid / unsupported command. Fail it as such
        //

        TEST_TRAP();
        Srb->Status = STATUS_NOT_IMPLEMENTED;
        COMPLETE_STREAM_READ(Srb);
    }
}

/*
** VideoGetProperty()
**
**    Routine to process video property requests
**
** Arguments:
**
**    Srb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID VideoGetProperty(PHW_STREAM_REQUEST_BLOCK Srb)
{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD;

    pSPD = Srb->CommandData.PropertyInfo;

    if (IsEqualGUID (&KSPROPSETID_Connection, &pSPD->Property->Set)) {
        VideoStreamGetConnectionProperty (Srb);
    }
    else if (IsEqualGUID (&PROPSETID_VIDCAP_DROPPEDFRAMES, &pSPD->Property->Set)) {
        VideoStreamGetDroppedFramesProperty (Srb);
    }
    else {
       Srb->Status = STATUS_NOT_IMPLEMENTED;
    }

}

/*
** VideoStreamGetConnectionProperty()
**
**    Reports Frame size for the allocater.
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID VideoStreamGetConnectionProperty(
    PHW_STREAM_REQUEST_BLOCK Srb
    )
{
    PUSBCAMD_CHANNEL_EXTENSION channelExtension = 
        Srb->StreamObject->HwStreamExtension;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = Srb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id) {

    case KSPROPERTY_CONNECTION_ALLOCATORFRAMING:
        if (channelExtension->VideoInfoHeader) {
            PKSALLOCATOR_FRAMING Framing = 
                (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;
                
            Framing->RequirementsFlags =
                KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY |
                KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
                KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY;
            Framing->PoolType = PagedPool;
            Framing->Frames = 4;            
            Framing->FrameSize = 
                channelExtension->VideoInfoHeader->bmiHeader.biSizeImage;

             USBCAMD_KdPrint(MIN_TRACE, 
                ("'KSPROPERTY_CONNECTION_ALLOCATORFRAMING (%d)\n",
                     Framing->FrameSize));                
                           
            Framing->FileAlignment = 0; // FILE_BYTE_ALIGNMENT;
            Framing->Reserved = 0;
            Srb->ActualBytesTransferred = sizeof (KSALLOCATOR_FRAMING);
            Srb->Status = STATUS_SUCCESS;
        } else {
            Srb->Status = STATUS_INVALID_PARAMETER;
        }
        break;

    default:
        TEST_TRAP();
        break;
    }
}

/*
** VideoStreamGetDroppedFramesProperty()
**
**    Reports the number of dropped frmaes since START.
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
VideoStreamGetDroppedFramesProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PUSBCAMD_CHANNEL_EXTENSION channelExtension = pSrb->StreamObject->HwStreamExtension;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id) {

    case KSPROPERTY_DROPPEDFRAMES_CURRENT:
        {
            PKSPROPERTY_DROPPEDFRAMES_CURRENT_S pDroppedFrames = 
                (PKSPROPERTY_DROPPEDFRAMES_CURRENT_S) pSPD->PropertyInfo;

            pDroppedFrames->PictureNumber = channelExtension->FrameInfo.PictureNumber;
            pDroppedFrames->DropCount = channelExtension->FrameInfo.DropCount;
            pDroppedFrames->AverageFrameSize = channelExtension->VideoInfoHeader->bmiHeader.biSizeImage;
                
            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_DROPPEDFRAMES_CURRENT_S);
			pSrb->Status = STATUS_SUCCESS;
			USBCAMD_KdPrint(MAX_TRACE, ("'USBCAMD_GetDroppedFrames %d\n", channelExtension->FrameInfo.DropCount));

        }
        break;

    default:
        TEST_TRAP();
        break;
    }
}



//==========================================================================;
//                   Clock Handling Routines
//==========================================================================;

//
// Another clock is being assigned as the Master clock
//

VOID VideoIndicateMasterClock (PHW_STREAM_REQUEST_BLOCK Srb)
{
    PUSBCAMD_CHANNEL_EXTENSION channelExtension = 
        Srb->StreamObject->HwStreamExtension;

    USBCAMD_KdPrint(MAX_TRACE, 
        ("'VideoIndicateMasterClock\n"));
        
    channelExtension->MasterClockHandle = 
        Srb->CommandData.MasterClockHandle;

    Srb->Status = STATUS_SUCCESS;        
}

//
// GetSystemTime in 100 nS units
//

ULONGLONG GetSystemTime()
{
    ULONGLONG ticks;
    ULONGLONG rate;

    ticks = (ULONGLONG)KeQueryPerformanceCounter((PLARGE_INTEGER)&rate).QuadPart;

    //
    // convert from ticks to 100ns clock
    //

    ticks = (ticks & 0xFFFFFFFF00000000) / rate * 10000000 +
    		(ticks & 0xFFFFFFFF) * 10000000 / rate;

    return(ticks);

}

/*
** VideoSetFormat()
**
**   Sets the format for a video stream.  This happens both when the 
**   stream is first opened, and also when dynamically switching formats
**   on the preview pin.  
**
**   It is assumed that the format has been verified for correctness before
**   this call is made.
**
** Arguments:
**
**   pSrb - Stream request block for the Video stream
**
** Returns: 
**
**   TRUE if the format could be set, else FALSE
**
** Side Effects:  none
*/

BOOL 
STREAMAPI 
VideoSetFormat(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{

    PUSBCAMD_CHANNEL_EXTENSION channelExtension =
        (PUSBCAMD_CHANNEL_EXTENSION) pSrb->StreamObject->HwStreamExtension;    
    PUSBCAMD_DEVICE_EXTENSION    pHwDevExt = pSrb->HwDeviceExtension;
    UINT                    nSize;
    PKSDATAFORMAT           pKSDataFormat = pSrb->CommandData.OpenFormat;
    
    // -------------------------------------------------------------------
    // Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
    // -------------------------------------------------------------------

    if (IsEqualGUID (&pKSDataFormat->Specifier, 
                &KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {
        
        PKS_DATAFORMAT_VIDEOINFOHEADER  pVideoInfoHeader = 
                    (PKS_DATAFORMAT_VIDEOINFOHEADER) pSrb->CommandData.OpenFormat;
        PKS_VIDEOINFOHEADER     pVideoInfoHdrRequested = 
                    &pVideoInfoHeader->VideoInfoHeader;
    
        nSize = sizeof (KS_VIDEOINFOHEADER) + pVideoInfoHdrRequested->bmiHeader.biSize;

        USBCAMD_KdPrint(MIN_TRACE, ("USBCAMD: New VideoInfoHdrRequested\n"));
		USBCAMD_KdPrint(MIN_TRACE, ("Width=%d  Height=%d  FrameTime (ms)= %d\n",
								pVideoInfoHdrRequested->bmiHeader.biWidth,
								pVideoInfoHdrRequested->bmiHeader.biHeight,
								pVideoInfoHdrRequested->AvgTimePerFrame/10000));
        //
        // If a previous format was in use, release the memory
        //
        if (channelExtension->VideoInfoHeader) {
            ExFreePool(channelExtension->VideoInfoHeader);
            channelExtension->VideoInfoHeader = NULL;
        }

        // Since the VIDEOINFOHEADER is of potentially variable size
        // allocate memory for it

        channelExtension->VideoInfoHeader = ExAllocatePool(NonPagedPool, nSize);

        if (channelExtension->VideoInfoHeader == NULL) {
            USBCAMD_KdPrint(MIN_TRACE, ("USBCAMD: ExAllocatePool failed\n"));
            pSrb->Status = STATUS_INSUFFICIENT_RESOURCES;
            return FALSE;
        }

        // Copy the VIDEOINFOHEADER requested to our storage
        RtlCopyMemory(
                channelExtension->VideoInfoHeader,
                pVideoInfoHdrRequested,
                nSize);
    }

    else {
        // Unknown format
        pSrb->Status = STATUS_INVALID_PARAMETER;
        return FALSE;
    }

    return TRUE;
}

/*
** USBCAMD_ReceiveCtrlPacket()
**
**   Receives packet commands that control the Audio stream
**
** Arguments:
**
**   Srb - The stream request block for the Audio stream
**
** Returns:
**
** Side Effects:  none
*/

VOID STREAMAPI 
USBCAMD_ReceiveCtrlPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )
{
    PUSBCAMD_DEVICE_EXTENSION deviceExtension = 
         Srb->HwDeviceExtension;
    BOOLEAN completedByCam = FALSE;    
    KSSTATE    PreviousState;
    BOOL                    Busy;
	
    PUSBCAMD_CHANNEL_EXTENSION channelExtension =
        (PUSBCAMD_CHANNEL_EXTENSION) Srb->StreamObject->HwStreamExtension;    
    PreviousState = channelExtension->KSState;

    USBCAMD_KdPrint(MIN_TRACE, ("'USBCAMD_ReceiveCtrlPacket %x\n", Srb->Command));
   
    //
    // If we're already processing an SRB, add it to the queue
    //
    Busy = AddToListIfBusy (
                        Srb,
                        &deviceExtension->ControlSRBSpinLock,
                        &deviceExtension->ProcessingControlSRB,
                        &deviceExtension->StreamControlSRBList);

    if (Busy) {
        return;
    }

    while (TRUE) {

		//
		// determine the type of packet.
		//

		//
		// call the cam driver first
		//
		if (channelExtension->CamReceiveCtrlPacket) {
			(*channelExtension->CamReceiveCtrlPacket)(
				Srb,
				USBCAMD_GET_DEVICE_CONTEXT(deviceExtension),
				&completedByCam);
		}        


		switch (Srb->Command) {

		case SRB_PROPOSE_DATA_FORMAT:

			if ( completedByCam == FALSE) {
				USBCAMD_KdPrint(MIN_TRACE, ("Cam driver should have handled PrposeDataFormat SRB.\n"));
				Srb->Status = STATUS_NOT_IMPLEMENTED;
			}
			break;

		case SRB_SET_DATA_FORMAT:

			if (completedByCam == TRUE) {
				if (Srb->Status == STATUS_SUCCESS) {
					// Cam driver accepted the request to change format. replace the existing one 
					// in stream ext. with the new format.
					VideoSetFormat(Srb);
				}
			}
			else {

				USBCAMD_KdPrint(MIN_TRACE, ("Cam driver should have handled SetDataFormat SRB.\n"));
				Srb->Status = STATUS_NOT_IMPLEMENTED;
			}
			break;

		case SRB_GET_DATA_FORMAT:

			if ( completedByCam == FALSE) {
				USBCAMD_KdPrint(MIN_TRACE, ("Cam driver should have handled GetDataFormat SRB.\n"));
				Srb->Status = STATUS_NOT_IMPLEMENTED;
			}

		case SRB_GET_STREAM_STATE:

			Srb->CommandData.StreamState = channelExtension->KSState;
			Srb->ActualBytesTransferred = sizeof (KSSTATE);
			Srb->Status = STATUS_SUCCESS;

			// A very odd rule:
			// When transitioning from stop to pause, DShow tries to preroll
			// the graph.  Capture sources can't preroll, and indicate this
			// by returning VFW_S_CANT_CUE in user mode.  To indicate this
			// condition from drivers, they must return ERROR_NO_DATA_DETECTED

			if (channelExtension->KSState == KSSTATE_PAUSE) {
				Srb->Status = STATUS_NO_DATA_DETECTED;
			}

			break;
    
		case SRB_SET_STREAM_STATE:
			{
			PUSBCAMD_CHANNEL_EXTENSION channelExtension =
				(PUSBCAMD_CHANNEL_EXTENSION) Srb->StreamObject->HwStreamExtension;   

			USBCAMD_KdPrint(MIN_TRACE, ("'set stream state %x\n", Srb->CommandData.StreamState));
			channelExtension->KSState = Srb->CommandData.StreamState;
    
			switch (Srb->CommandData.StreamState)  {

			case KSSTATE_STOP:

				USBCAMD_KdPrint(MIN_TRACE, ("'KSSTATE_STOP\n"));

				if (channelExtension->ImageCaptureStarted) {
					Srb->Status =                     
						USBCAMD_StopChannel(deviceExtension,
											channelExtension);                    
                                    
				} else {
					Srb->Status = STATUS_SUCCESS;
				}
        
				break;

			case KSSTATE_PAUSE:

				USBCAMD_KdPrint(MIN_TRACE, ("'KSSTATE_PAUSE\n")); 
				//
				// On a transition to pause from acquire or stop, start our timer running.
				//

				if (PreviousState == KSSTATE_ACQUIRE || PreviousState == KSSTATE_STOP) {  

					// Zero the frame counters
#if DBG
                    channelExtension->VideoFrameLostCount = 0; // actual dropped frames
#endif
					channelExtension->FrameInfo.PictureNumber = 0;
					channelExtension->FrameInfo.DropCount = 0;
					channelExtension->FrameInfo.dwFrameFlags = 0;
					channelExtension->FrameCaptured = -1;   // actual frames captured
					channelExtension->FirstFrame = TRUE;
				}
				Srb->Status = STATUS_SUCCESS;
				break;

			case KSSTATE_ACQUIRE:
    
				USBCAMD_KdPrint(MIN_TRACE, ("'KSSTATE_ACQUIRE\n"));
				Srb->Status = STATUS_SUCCESS;
				break;

			case KSSTATE_RUN:

				USBCAMD_KdPrint(MIN_TRACE, ("'KSSTATE_RUN\n"));


				// we will not start the channel again if we are toggling between pause & run.
				if (channelExtension->ImageCaptureStarted == FALSE) {           
					Srb->Status =
						 USBCAMD_StartChannel(deviceExtension,
											  channelExtension);
				}
				else {
					Srb->Status = STATUS_SUCCESS;
				}
				break;
        
			default:
    
				TEST_TRAP();
				Srb->Status = STATUS_NOT_IMPLEMENTED;
				break;
			}  
			}
			break;

		case SRB_INDICATE_MASTER_CLOCK:

			//
			// Assigns a clock to a stream
			//

			VideoIndicateMasterClock (Srb);

			break;
    
		case SRB_GET_STREAM_PROPERTY:

			VideoGetProperty(Srb);
			break;

		default:

			//
			// invalid / unsupported command. Fail it as such
			//

			TEST_TRAP();

			Srb->Status = STATUS_NOT_IMPLEMENTED;
		}

		COMPLETE_STREAM_READ(Srb);

        // 
        // See if there's anything else on the queue
        //
        Busy = RemoveFromListIfAvailable (
                        &Srb,
                        &deviceExtension->ControlSRBSpinLock,
                        &deviceExtension->ProcessingControlSRB,
                        &deviceExtension->StreamControlSRBList);

        if (!Busy) {
            break;
        }

	}
}



/*
** USBCAMD_CompleteRead()
**
**   Complete am Srb
**
** Arguments:
**
**
** Returns:
**
** Side Effects:  none
*/

VOID 
USBCAMD_CompleteRead(
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBCAMD_READ_EXTENSION ReadExtension, 
    IN NTSTATUS NtStatus,
    IN ULONG BytesTransferred
    )
{
    PHW_STREAM_REQUEST_BLOCK srb;
    PKSSTREAM_HEADER dataPacket;
    PKS_FRAME_INFO    pFrameInfo;


    srb = ReadExtension->Srb;
    srb->Status = NtStatus;
    dataPacket = srb->CommandData.DataBufferArray;
    dataPacket ->DataUsed = BytesTransferred;
    pFrameInfo = (PKS_FRAME_INFO) (dataPacket + 1);

	// Set additional info fields about the data captured such as:
    //   Frames Captured
    //   Frames Dropped
    //   Field Polarity
                
    ChannelExtension->FrameInfo.ExtendedHeaderSize = pFrameInfo->ExtendedHeaderSize;


	//
    // Return the timestamp for the frame
    //
    dataPacket->PresentationTime.Numerator = 1;
    dataPacket->PresentationTime.Denominator = 1;
    dataPacket->Duration = ChannelExtension->VideoInfoHeader->AvgTimePerFrame;


    if ( ChannelExtension->MasterClockHandle) {

        dataPacket->PresentationTime.Time = GetStreamTime(srb, ChannelExtension);
	}
	else {
        //
        // no clock eview stream, so just mark the time as unknown
        //
        dataPacket->PresentationTime.Time = 0;
	} // end Masterclaockhandle

    ChannelExtension->FrameInfo.PictureNumber = (dataPacket->PresentationTime.Time - ChannelExtension->StartTime) / 
							   ChannelExtension->VideoInfoHeader->AvgTimePerFrame + 1;

	// calculate the drop frame count expected by Direct Show based on the data passed down in vidoeinfohdr
	// and current stream time, start time.
	
	ChannelExtension->FrameInfo.DropCount = ChannelExtension->FrameInfo.PictureNumber - 
							                ChannelExtension->FrameCaptured;
	if (ChannelExtension->FrameInfo.DropCount < 0 ) {
		ChannelExtension->FrameInfo.DropCount = 0;
	}
	
	
	USBCAMD_KdPrint(MIN_TRACE, ("'Picture #  = %16d\n",ChannelExtension->FrameInfo.PictureNumber));
	USBCAMD_KdPrint(MIN_TRACE, ("'Actual Frame #  = %16d\n",ChannelExtension->FrameCaptured));
	USBCAMD_KdPrint(MIN_TRACE, ("'Drop #  = %16d\n",ChannelExtension->FrameInfo.DropCount));

#if DBG
	USBCAMD_KdPrint(MIN_TRACE, ("'Actual Drop #  = %16d\n",ChannelExtension->VideoFrameLostCount));
#endif
	
	// we set the options flags to key frames only if Cam driver didn't indicate otherwise.
	if ( dataPacket->OptionsFlags == 0 ) {
		// Every frame we generate is a key frame (aka SplicePoint)
	    dataPacket->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_SPLICEPOINT;
	}
  
	//
    // if we have a master clock 
    // 
    if (ChannelExtension->MasterClockHandle ) {
			dataPacket->OptionsFlags |= 
				KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
				KSSTREAM_HEADER_OPTIONSF_DURATIONVALID;
    }
    else {
    // clear the timestamp valid flags
			dataPacket->OptionsFlags &= 
				~(KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
                KSSTREAM_HEADER_OPTIONSF_DURATIONVALID);
    }

    *pFrameInfo = ChannelExtension->FrameInfo;

    USBCAMD_FreeRawFrameBuffer(ReadExtension->RawFrameBuffer);

#ifdef MAX_DEBUG
    if (!NT_SUCCESS(NtStatus)) {
        TEST_TRAP();
    }
#endif    
    USBCAMD_KdPrint(MIN_TRACE, ("'Read Srb %x is completed, status = %x\n",srb, srb->Status));
    COMPLETE_STREAM_READ(srb);        
}

/*
** USBCAMD_GetFrameBufferFromSrb()
**
**   Complete am Srb
**
** Arguments:
**
**
** Returns:
**
** Side Effects:  none
*/

PVOID
USBCAMD_GetFrameBufferFromSrb(
    IN PVOID Srb, 
    OUT PULONG MaxLength
    )
{
    PVOID frameBuffer;
    PKSSTREAM_HEADER streamHeader;

#if 0	// PIO = FALSE
	PIRP irp;
        
    irp = ((PHW_STREAM_REQUEST_BLOCK) Srb)->Irp;

    USBCAMD_KdPrint(MIN_TRACE, ("'SRB MDL = %x\n", 
        irp->MdlAddress));
      
    frameBuffer = MmGetSystemAddressForMdl(irp->MdlAddress);        
#else
	// PIO = TRUE
    frameBuffer = ((PHW_STREAM_REQUEST_BLOCK) Srb)->CommandData.DataBufferArray->Data;
#endif
    streamHeader = ((PHW_STREAM_REQUEST_BLOCK) Srb)->CommandData.DataBufferArray;
    USBCAMD_KdPrint(MAX_TRACE, ("'SRB Length = %x\n", 
        streamHeader->FrameExtent));
    USBCAMD_KdPrint(MAX_TRACE, ("'frame buffer = %x\n", frameBuffer));
    *MaxLength = streamHeader->FrameExtent;

    return frameBuffer;
}

/*
** AddToListIfBusy ()
**
**   Grabs a spinlock, checks the busy flag, and if set adds an SRB to a queue
**
** Arguments:
**
**   pSrb - Stream request block
**
**   SpinLock - The spinlock to use when checking the flag
**
**   BusyFlag - The flag to check
**
**   ListHead - The list onto which the Srb will be added if the busy flag is set
**
** Returns: 
**
**   The state of the busy flag on entry.  This will be TRUE if we're already 
**   processing an SRB, and FALSE if no SRB is already in progress.
**
** Side Effects:  none
*/

BOOL
STREAMAPI 
AddToListIfBusy (
    IN PHW_STREAM_REQUEST_BLOCK pSrb,
    IN KSPIN_LOCK              *SpinLock,
    IN OUT BOOL                *BusyFlag,
    IN LIST_ENTRY              *ListHead
    )
{
    KIRQL                       Irql;
    PUSBCAMD_READ_EXTENSION    pSrbExt = (PUSBCAMD_READ_EXTENSION)pSrb->SRBExtension;

    KeAcquireSpinLock (SpinLock, &Irql);

    // If we're already processing another SRB, add this current request
    // to the queue and return TRUE

    if (*BusyFlag == TRUE) {
        // Save the SRB pointer away in the SRB Extension
        pSrbExt->Srb = pSrb;
		USBCAMD_KdPrint(MIN_TRACE, ("'Queuing CtrlPacket %x\n", pSrb->Command));
        InsertTailList(ListHead, &pSrbExt->ListEntry);
        KeReleaseSpinLock(SpinLock, Irql);
        return TRUE;
    }

    // Otherwise, set the busy flag, release the spinlock, and return FALSE

    *BusyFlag = TRUE;
    KeReleaseSpinLock(SpinLock, Irql);

    return FALSE;
}

/*
** RemoveFromListIfAvailable ()
**
**   Grabs a spinlock, checks for an available SRB, and removes it from the list
**
** Arguments:
**
**   &pSrb - where to return the Stream request block if available
**
**   SpinLock - The spinlock to use 
**
**   BusyFlag - The flag to clear if the list is empty
**
**   ListHead - The list from which an SRB will be removed if available
**
** Returns: 
**
**   TRUE if an SRB was removed from the list
**   FALSE if the list is empty
**
** Side Effects:  none
*/

BOOL
STREAMAPI 
RemoveFromListIfAvailable (
    IN OUT PHW_STREAM_REQUEST_BLOCK *pSrb,
    IN KSPIN_LOCK                   *SpinLock,
    IN OUT BOOL                     *BusyFlag,
    IN LIST_ENTRY                   *ListHead
    )
{
    KIRQL                       Irql;

    KeAcquireSpinLock (SpinLock, &Irql);

    //
    // If the queue is now empty, clear the busy flag, and return 
    //
    if (IsListEmpty(ListHead)) {
        *BusyFlag = FALSE;
        KeReleaseSpinLock(SpinLock, Irql);
        return FALSE;
    }
    //
    // otherwise extract the SRB
    //
    else {
        PUSBCAMD_READ_EXTENSION  pSrbExt;
		PLIST_ENTRY listEntry;

        listEntry = RemoveHeadList(ListHead);

        pSrbExt = (PUSBCAMD_READ_EXTENSION) CONTAINING_RECORD(listEntry, 
                                             USBCAMD_READ_EXTENSION, 
                                             ListEntry);          
        *BusyFlag = TRUE;
        KeReleaseSpinLock(SpinLock, Irql);
        *pSrb = pSrbExt->Srb;
    }
    return TRUE;
}

/*
** GetStreamTime ()
**
**   Get current stream time from the graph master clock
**
** Arguments:
**
**   Srb - pointer to current SRB
**
**   ChannelExtension - ptr to current channel extension 
**
**
** Returns: 
**
**   current stream time in ULONGULONG
**
** Side Effects:  none
*/

ULONGLONG GetStreamTime( 
			IN PHW_STREAM_REQUEST_BLOCK Srb, 
			IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension)
{

	HW_TIME_CONTEXT  timeContext;

	timeContext.HwDeviceExtension = 
		(struct _HW_DEVICE_EXTENSION *)ChannelExtension->DeviceExtension;
	timeContext.HwStreamObject = Srb->StreamObject;
	timeContext.Function = TIME_GET_STREAM_TIME;
	timeContext.Time = timeContext.SystemTime =0;

    if ( ChannelExtension->MasterClockHandle) {

		StreamClassQueryMasterClockSync(ChannelExtension->MasterClockHandle,&timeContext);

		USBCAMD_KdPrint(MIN_TRACE, ("'Time (ms) = %d\n",timeContext.Time / 10000));
	}
	return (timeContext.Time);
}




