/*++

Copyright (c) 1996 Microsoft Corporation

Module Name:

    sonydcam.c

Abstract:

    This is a driver for the Sony Desktop Camera (CCM-DS250 V1.0x).
    This driver fits under the WDM stream class.

Author:
    
    Shaun Pierce  25-May-96

Modified:

    Yee J. Wu  15-Oct-97

Environment:

    Kernel mode only

Revision History:


--*/

#include "strmini.h"
#include "1394.h"
#include "dbg.h"
#include "ksmedia.h"
#include "dcamdef.h"
#include "sonydcam.h"

#include "capprop.h"   // Video and camera property function prototype


//
// For debug build only :
//
// Since this driver support multiple instance of 1394 DCAm (Sony and TI at this time), 
// this is one way to keep track of them, esp tracing what is happening.  
//
#if defined(DBG)
    LONG cntOpenStream;
#endif


NTSTATUS
DriverEntry(
    IN PVOID Context1,
    IN PVOID Context2
    )

/*++

Routine Description:

    This where life begins for a driver.  The stream class takes care
    of alot of stuff for us, but we still need to fill in an initialization
    structure for the stream class and call it.

Arguments:

    Context1 - Pointer to operating system context value (don't need to know)

    Context2 - Pointer to operating system context value (don't need to know)

Return Value:

    The function value is the final status from the initialization operation.

--*/

{

    HW_INITIALIZATION_DATA HwInitData;


    DbgMsg1(("\'SonyDCam DriverEntry: enter\n"));

    //
    // Fill in the HwInitData structure
    //

#if defined(DBG)
    cntOpenStream = 0;
#endif

    HwInitData.HwInitializationDataSize = sizeof(HwInitData);
    HwInitData.HwInterrupt = NULL;
    HwInitData.HwReceivePacket = SonyDCamReceivePacket;
    HwInitData.HwCancelPacket = SonyDCamCancelPacket;
    HwInitData.HwRequestTimeoutHandler = SonyDCamTimeoutHandler;
    HwInitData.DeviceExtensionSize = sizeof(SONYDCAM_EXTENSION);
    HwInitData.PerStreamExtensionSize   = sizeof(STREAMEX); 
    HwInitData.PerRequestExtensionSize = sizeof(IRB);
    HwInitData.FilterInstanceExtensionSize = 0;
    HwInitData.BusMasterDMA = FALSE;
    HwInitData.Dma24BitAddresses = FALSE;
    HwInitData.BufferAlignment = sizeof(ULONG) - 1;
    HwInitData.TurnOffSynchronization = TRUE;
    HwInitData.DmaBufferSize = 0;

    return (StreamClassRegisterAdapter(Context1, Context2, &HwInitData));

}



#define DEQUEUE_SETTLE_TIME      (ULONG)(-1 * MAX_BUFFERS_SUPPLIED * 10000) 

NTSTATUS
SonyDCamHwUnInitialize(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )
/*++

Routine Description:

    This where we perform the necessary initialization tasks.

Arguments:

    Srb - Pointer to stream request block

Return Value:

    Nothing

--*/
{
      NTSTATUS statusKeWait;

      PSONYDCAM_EXTENSION DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;

      DbgMsg2(("\'SonyDCamHwUnInitialize: enter with DevExt=0x%8x; Srb=0x%x; ThreadID=0x%x\n\n", DeviceExtension, Srb, PsGetCurrentThread()));


      // Stop any SRB_READ_DATA 
      DeviceExtension->bCameraStopped = TRUE;

      statusKeWait = KeWaitForSingleObject( &DeviceExtension->semaBusReset, Executive, KernelMode, FALSE, 0 );

      DbgMsg2(("\'%d:%s) SonyDCamHwUnInitialize: statusKeWait=%x, DevExt=%x;\n", 
            DeviceExtension->idxDev, DeviceExtension->pchVendorName, statusKeWait, DeviceExtension));


      ASSERT(DeviceExtension->PendingReadCount == 0);

      if (DeviceExtension->ConfigRom) {
         ExFreePool(DeviceExtension->ConfigRom);
         DeviceExtension->ConfigRom = 0;
      }

      if (DeviceExtension->VendorLeaf) {
         ExFreePool(DeviceExtension->VendorLeaf);
         DeviceExtension->VendorLeaf;
      }

      if (DeviceExtension->Irb) {
         ExFreePool(DeviceExtension->Irb);
         DeviceExtension->Irb = 0;
      }

      if (DeviceExtension->FormatBlocks) {
         ExFreePool(DeviceExtension->FormatBlocks);
         DeviceExtension->FormatBlocks = 0;
      }

      if (DeviceExtension->Irp) {
         IoFreeIrp(DeviceExtension->Irp);
         DeviceExtension->Irp = 0;
      }

      DbgMsg2(("\'SonyDCamHwUnInitialize:Leaving..  Resource freed (Isoch, Device Extension and all).\n"));

      return STATUS_SUCCESS;
}




VOID 
InitializeDeviceExtension(
    PPORT_CONFIGURATION_INFORMATION ConfigInfo
      )
{
    PSONYDCAM_EXTENSION DeviceExtension;

    DeviceExtension = (PSONYDCAM_EXTENSION) ConfigInfo->HwDeviceExtension;

    DeviceExtension->SpeedCode = (ULONG) ((PNODE_DEVICE_EXTENSION) ConfigInfo->PhysicalDeviceObject->DeviceExtension)->Speed;
    DbgMsg2(("\'InitializeDeviceExtension: Speed code from 1394bus driver (parent) =0x%x\n", DeviceExtension->SpeedCode));

    // 1:Signal; upper limit is 1
    KeInitializeSemaphore(&DeviceExtension->semaBusReset, 1, 1);

    DeviceExtension->SharedDeviceObject = ConfigInfo->ClassDeviceObject;
    DeviceExtension->BusDeviceObject = ConfigInfo->PhysicalDeviceObject;
    DeviceExtension->BaseRegister = 0;
    DeviceExtension->FrameRate = DEFAULT_FRAME_RATE;
    InitializeListHead(&DeviceExtension->IsochDescriptorList);
    KeInitializeSpinLock(&DeviceExtension->IsochDescriptorLock);
    DeviceExtension->bNeedToListen = FALSE;
    DeviceExtension->hResource = NULL;
    DeviceExtension->hBandwidth = NULL;
    DeviceExtension->IsochChannel = (ULONG) -1;
    DeviceExtension->PendingReadCount = 0; 
    DeviceExtension->pStrmEx = 0;

    InitializeListHead(&DeviceExtension->IsochWaitingList);
    KeInitializeSpinLock(&DeviceExtension->IsochWaitingLock);
}


NTSTATUS
SonyDCamHwInitialize(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    This where we perform the necessary initialization tasks.

Arguments:

    Srb - Pointer to stream request block

Return Value:

    Nothing

--*/

{

    PIRB Irb;
    CCHAR StackSize;
    ULONG i;
    ULONG DirectoryLength;
    NTSTATUS status;
    PSONYDCAM_EXTENSION DeviceExtension;
    PPORT_CONFIGURATION_INFORMATION ConfigInfo;
  
         

    ConfigInfo = Srb->CommandData.ConfigInfo;
    Irb = (PIRB) Srb->SRBExtension;
    DeviceExtension = (PSONYDCAM_EXTENSION) ConfigInfo->HwDeviceExtension;

    //
    // Initialize DeviceExtension
    //
    InitializeDeviceExtension(ConfigInfo);
           

    DbgMsg2(("\'DCamHwInitialize: IRQL=%d enter with DeviceExtension=0x%8x\n", KeGetCurrentIrql(), DeviceExtension));

    StackSize = DeviceExtension->BusDeviceObject->StackSize;
    DeviceExtension->Irp = IoAllocateIrp(StackSize, FALSE);
    if (!DeviceExtension->Irp) {

        SONYDCAM_ASSERT(FALSE);
        return (STATUS_INSUFFICIENT_RESOURCES);
    }

    DeviceExtension->Irb = ExAllocatePool(NonPagedPool, sizeof(IRB));
    if (!DeviceExtension->Irb) {

        SONYDCAM_ASSERT(FALSE);            
        return (STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // find what the host adaptor below us supports...
    //
    Irb->FunctionNumber = REQUEST_GET_LOCAL_HOST_INFO;
    Irb->Flags = 0;
    Irb->u.GetLocalHostInformation.nLevel = GET_HOST_CAPABILITIES;
    Irb->u.GetLocalHostInformation.Information = &DeviceExtension->HostControllerInfomation;
    status = SonyDCamSubmitIrpSynch(DeviceExtension, DeviceExtension->Irp, Irb);
    if (status) {

        DbgMsg1(("\'SonyDCamHwInitialize: Error (Status=%x) while trying to get local hsot info.\n", status));
        return (STATUS_UNSUCCESSFUL);
    }        

    //
    // Make a call to determine what the generation # is on the bus,
    // followed by a call to find out about ourself (config rom info)
    //
    //
    // Get the current generation count first
    //

    Irb->FunctionNumber = REQUEST_GET_GENERATION_COUNT;
    Irb->Flags = 0;

    status = SonyDCamSubmitIrpSynch(DeviceExtension, DeviceExtension->Irp, Irb);

    if (status) {

        DbgMsg1(("\'DCamHwInitialize: Error %x while trying to get generation number\n", status));
        return (STATUS_UNSUCCESSFUL);

    }

    DeviceExtension->CurrentGeneration = Irb->u.GetGenerationCount.GenerationCount;
    DbgMsg3(("\'DCamHwInitialize: Generation number = %x\n", DeviceExtension->CurrentGeneration));

    //
    // Now that we have the current generation count, find out how much
    // configuration space we need by setting lengths to zero.
    //

    Irb->FunctionNumber = REQUEST_GET_CONFIGURATION_INFO;
    Irb->Flags = 0;
    Irb->u.GetConfigurationInformation.UnitDirectoryBufferSize = 0;
    Irb->u.GetConfigurationInformation.UnitDependentDirectoryBufferSize = 0;
    Irb->u.GetConfigurationInformation.VendorLeafBufferSize = 0;
    Irb->u.GetConfigurationInformation.ModelLeafBufferSize = 0;

    status = SonyDCamSubmitIrpSynch(DeviceExtension, DeviceExtension->Irp, Irb);

    if (status) {

        DbgMsg1(("\'SonyDcamHwInitialize: Error %x while trying to get configuration info (1)\n", status));
        return (STATUS_UNSUCCESSFUL);

    }

    //
    // Now go thru and allocate what we need to so we can get our info.
    //

    DeviceExtension->ConfigRom = ExAllocatePool(PagedPool, sizeof(CONFIG_ROM));
    if (!DeviceExtension->ConfigRom) {

        DbgMsg1(("\'SonyDCamHwInitialize: Couldn't allocate memory for the Config Rom\n"));
        return (STATUS_INSUFFICIENT_RESOURCES);

    }

    DeviceExtension->UnitDirectory = ExAllocatePool(PagedPool, Irb->u.GetConfigurationInformation.UnitDirectoryBufferSize);
    if (!DeviceExtension->UnitDirectory) {

        DbgMsg1(("\'SonyDCamHwInitialize: Couldn't allocate memory for the UnitDirectory\n"));
        return (STATUS_INSUFFICIENT_RESOURCES);

    }

    if (Irb->u.GetConfigurationInformation.UnitDependentDirectoryBufferSize) {

        DeviceExtension->UnitDependentDirectory = ExAllocatePool(PagedPool, Irb->u.GetConfigurationInformation.UnitDependentDirectoryBufferSize);
        if (!DeviceExtension->UnitDependentDirectory) {

            DbgMsg1(("\'SonyDCamHwInitialize: Couldn't allocate memory for the UnitDependentDirectory\n"));
            return (STATUS_INSUFFICIENT_RESOURCES);

        }

    }


    if (Irb->u.GetConfigurationInformation.VendorLeafBufferSize) {

        DeviceExtension->VendorLeaf = ExAllocatePool(PagedPool, Irb->u.GetConfigurationInformation.VendorLeafBufferSize);
        if (!DeviceExtension->VendorLeaf) {

            DbgMsg1(("\'SonyDCamHwInitialize: Couldn't allocate memory for the VendorLeaf\n"));
            return (STATUS_INSUFFICIENT_RESOURCES);
        }
    }

    if (Irb->u.GetConfigurationInformation.ModelLeafBufferSize) {

        DeviceExtension->ModelLeaf = ExAllocatePool(PagedPool, Irb->u.GetConfigurationInformation.ModelLeafBufferSize);
        if (!DeviceExtension->ModelLeaf) {

            DbgMsg1(("\'SonyDCamHwInitialize: Couldn't allocate memory for the ModelLeaf\n"));
            return (STATUS_INSUFFICIENT_RESOURCES);
        }
    }

    //
    // Now resubmit the Irb with the appropriate pointers inside
    //

    Irb->FunctionNumber = REQUEST_GET_CONFIGURATION_INFO;
    Irb->Flags = 0;
    Irb->u.GetConfigurationInformation.ConfigRom = DeviceExtension->ConfigRom;
    Irb->u.GetConfigurationInformation.UnitDirectory = DeviceExtension->UnitDirectory;
    Irb->u.GetConfigurationInformation.UnitDependentDirectory = DeviceExtension->UnitDependentDirectory;
    Irb->u.GetConfigurationInformation.VendorLeaf = DeviceExtension->VendorLeaf;
    Irb->u.GetConfigurationInformation.ModelLeaf = DeviceExtension->ModelLeaf;

    status = SonyDCamSubmitIrpSynch(DeviceExtension, DeviceExtension->Irp, Irb);

    if (status) {

        DbgMsg1(("\'SonyDCamHwInitialize: Error %x while trying to get configuration info (2)\n", status));
        return (STATUS_UNSUCCESSFUL);

    }

    //
    // We might be able to return strings about a Device
    //

    if (DeviceExtension->VendorLeaf) {

        ANSI_STRING ansiVendorName;
        ULONG ByteSwappedData;

        ByteSwappedData = bswap(*(((PULONG) DeviceExtension->VendorLeaf+1)));
        if (ByteSwappedData & 0x80000000) 
            RtlUnicodeStringToAnsiString(&ansiVendorName, ((PUNICODE_STRING) &DeviceExtension->VendorLeaf->TL_Data), TRUE);
        else 
            RtlInitAnsiString(&ansiVendorName, &DeviceExtension->VendorLeaf->TL_Data);            

        DeviceExtension->pchVendorName = ansiVendorName.Buffer;
    }

    // IHV of this DCam
    DbgMsg2(("\'SonyDCamHwInitialize:IHV of this device: %s\n", DeviceExtension->pchVendorName));


    //
    // Now we chew thru the Unit Dependent Directory looking for our command
    // base register key.
    //

    DirectoryLength = Irb->u.GetConfigurationInformation.UnitDependentDirectoryBufferSize >> 2;
    for (i=1; i < DirectoryLength; i++) {

        if ((*(((PULONG) DeviceExtension->UnitDependentDirectory)+i) & CONFIG_ROM_KEY_MASK) == COMMAND_BASE_KEY_SIGNITURE) {

            //
            // Found the command base offset.  This is a quadlet offset from
            // the initial register space.  (Should come out to 0xf0f00000)
            //

            DeviceExtension->BaseRegister = bswap(*(((PULONG) DeviceExtension->UnitDependentDirectory)+i) & CONFIG_ROM_OFFSET_MASK);
            DeviceExtension->BaseRegister <<= 2;
            DeviceExtension->BaseRegister |= INITIAL_REGISTER_SPACE_LO;
            break;

        }
        
    }

    SONYDCAM_ASSERT ( DeviceExtension->BaseRegister );

    if ((Srb->Status = SonyDCamReadRegister ((PIRB) Srb->SRBExtension, DeviceExtension, 
                  FIELDOFFSET(CAMERA_REGISTER_MAP, VModeInq[0]), &DeviceExtension->RegisterWorkArea.AsULONG))) {
         DbgMsg2(("\'SonyDCamStopStream: Error %x while stopping at Iso Enable register\n", Srb->Status));            
    }

    DbgMsg3(("\'SonyDCamHwInitialize: VModes before bswap = %x\n", DeviceExtension->RegisterWorkArea.AsULONG));
    DeviceExtension->RegisterWorkArea.AsULONG = bswap(DeviceExtension->RegisterWorkArea.AsULONG);
    DbgMsg3(("\'SonyDCamHwInitialize: VModes = %x\n", DeviceExtension->RegisterWorkArea.AsULONG));

    //
    // For each mode supported, we setup a format block in our format array
    // for our stream.  We just copy all the supported stream formats into
    // one place in our DeviceExtension so when GetStreamInfo is called
    // we can return the array correctly.
    //

    // DEBUG

    DeviceExtension->RegisterWorkArea.AsULONG = 0;
    DeviceExtension->RegisterWorkArea.VMode.Mode1 = TRUE;

    // DEBUG 

    DeviceExtension->ModesSupported = bitcount(DeviceExtension->RegisterWorkArea.AsULONG);
    DeviceExtension->FormatBlocks = ExAllocatePool(NonPagedPool, sizeof(KSDATAFORMAT) * DeviceExtension->ModesSupported);
    if (!DeviceExtension->FormatBlocks) {

        DbgMsg1(("\'SonyDCamHwInitialize: Couldn't allocate memory for the FormatBlocks\n"));
        return (STATUS_INSUFFICIENT_RESOURCES);

    }

    //
    // Now let's actually do a write request to initialize the device
    //

    DeviceExtension->RegisterWorkArea.AsULONG = 0;
    DeviceExtension->RegisterWorkArea.Initialize.Initialize = TRUE;
    DeviceExtension->RegisterWorkArea.AsULONG = bswap(DeviceExtension->RegisterWorkArea.AsULONG);

    //
    // Delay for camera before next request
    //
    Srb->Status = SonyDCamWriteRegister ((PIRB) Srb->SRBExtension, DeviceExtension, 
                  FIELDOFFSET(CAMERA_REGISTER_MAP, Initialize), DeviceExtension->RegisterWorkArea.AsULONG);


    // Free resource (from below)
    ExFreePool(DeviceExtension->UnitDirectory);

    if (Srb->Status) {

        DbgMsg1(("\'SonyDCamHwInitialize: Error %x while trying to write to Initialize register\n", status));
        return (STATUS_UNSUCCESSFUL);      
    }


    if (DeviceExtension->UnitDependentDirectory)
        ExFreePool(DeviceExtension->UnitDependentDirectory);

    if (DeviceExtension->ModelLeaf)
        ExFreePool(DeviceExtension->ModelLeaf);

    //
    // Now we initialize the size of stream descriptor information.
    // We have one stream descriptor, and we attempt to dword align the
    // structure.
    //

    ConfigInfo->StreamDescriptorSize = 
        1 * (sizeof (HW_STREAM_INFORMATION)) +      // 1 stream descriptor
        sizeof(HW_STREAM_HEADER);                   // and 1 stream header


    return (STATUS_SUCCESS);

}


NTSTATUS
SonyDCamSubmitIrpSynch(
    PSONYDCAM_EXTENSION DeviceExtension,
    PIRP Irp,
    PIRB Irb
    )

/*++

Routine Description:

    This routine submits an Irp synchronously to the bus driver.  We'll
    wait here til the Irp comes back

Arguments:

    DeviceExtension - Pointer to my local device extension

    Irp - Pointer to Irp we're sending down to the port driver synchronously

    Irb - Pointer to Irb we're submitting to the port driver

Return Value:

    Status is returned from Irp

--*/

{


    ULONG Retries=RETRY_COUNT;
    KEVENT Event;
    NTSTATUS status;
    LARGE_INTEGER deltaTime;
    PIO_STACK_LOCATION NextIrpStack;



    do {

        NextIrpStack = IoGetNextIrpStackLocation(Irp);
        NextIrpStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        NextIrpStack->Parameters.DeviceIoControl.IoControlCode = IOCTL_1394_CLASS;
        NextIrpStack->Parameters.Others.Argument1 = Irb;

        KeInitializeEvent(&Event, SynchronizationEvent, FALSE);

        IoSetCompletionRoutine(
            Irp,
            SonyDCamSynchCompletionRoutine,
           &Event,
            TRUE,
            TRUE,
            TRUE
            );

        status = IoCallDriver(
                    DeviceExtension->BusDeviceObject,
                    Irp
                    );

        if (status == STATUS_PENDING) {

            //
            // Still pending, wait for the IRP to complete
            //

            DbgMsg3(("\'SonyDCamSubmitIrpSynch: Irp is pending, waiting...\n"));
            KeWaitForSingleObject(
               &Event,
                Executive,
                KernelMode,
                FALSE,
                NULL
                );

        }

        if (Irp->IoStatus.Status == STATUS_TIMEOUT) {

            //
            // Camera isn't fast enough to respond to us sometimes, so
            // we'll delay a little bit in order for it to recover
            //

            deltaTime.LowPart = SONYDCAM_DELAY_VALUE;
            deltaTime.HighPart = -1;
            KeDelayExecutionThread(KernelMode, TRUE, &deltaTime);

        }

    } while ((Irp->IoStatus.Status == STATUS_TIMEOUT) && (Retries--));

    return (Irp->IoStatus.Status);

}


NTSTATUS
SonyDCamSynchCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PKEVENT Event
    )

/*++

Routine Description:

    This routine is for use with synchronous IRP processing.  
    All it does is signal an event, so the driver knows it
    can continue.

Arguments:

    DriverObject - Pointer to driver object created by system.

    Irp - Irp that just completed

    Event - Event we'll signal to say Irp is done

Return Value:

    None.

--*/

{

    KeSetEvent((PKEVENT) Event, 0, FALSE);
    return (STATUS_MORE_PROCESSING_REQUIRED);

}


ULONG
bitcount(
    IN ULONG x
    )

{

    ULONG b;

    for (b=0; x != 0; x >>= 1) {

        if (x & 01) {

            b++;

        }

    }

    return b;

}

