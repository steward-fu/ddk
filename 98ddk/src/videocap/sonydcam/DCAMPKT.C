/*++

Copyright (c) 1996 Microsoft Corporation

Module Name:

    dcampkt.c

Abstract:

    This is a driver for the Sony Desktop Camera (CCM-DS250 V1.0x).
    This file contains code to handle the stream class packets.

Author:
    
    Shaun Pierce  25-May-96

Modified:

    Yee J. Wu  15-Oct-97

Environment:

    Kernel mode only

Revision History:


--*/


#include "strmini.h"
#include "ksmedia.h"
#include "1394.h"
#include "wdm.h"       // for DbgBreakPoint() defined in dbg.h
#include "dbg.h"
#include "dcamdef.h"
#include "dcampkt.h"

#include "capprop.h"   // Video and camera property function prototype
#include "PropData.h"  // Define once property data definition - for Sony DCam
#include "PropDta2.h"  // Define once property data definition - for TI DCam
#include "CapProp.c"   // Source code to process VideoProcAmp and CameraControl

#if defined(DBG)
    extern ULONG cntOpenStream;
#endif


void 
tmGetStreamTime(
    IN PHW_STREAM_REQUEST_BLOCK Srb,
    PSTREAMEX pStrmEx, 
    ULONGLONG * ptmStream) 
/*++

Routine Description:

   Query the current time used to timestamp the frame or calculating the dropped frame.

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/
{

    HW_TIME_CONTEXT TimeContext;

    TimeContext.HwDeviceExtension = (PVOID) Srb->HwDeviceExtension;
    TimeContext.HwStreamObject = Srb->StreamObject;
    TimeContext.Function = TIME_GET_STREAM_TIME;
    TimeContext.Time = 0;
    TimeContext.SystemTime = 0;

    StreamClassQueryMasterClockSync(
          pStrmEx->hMasterClock,
          &TimeContext);

    *ptmStream = TimeContext.Time;
}

VOID
SonyDCamSetKSStatePAUSE(
    IN PHW_STREAM_REQUEST_BLOCK Srb
                         )
/*++

Routine Description:

    Set KSSTATE to KSSTATE_PAUSE.

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/
{
    PIRB Irb;
    PIRP Irp;
    CCHAR StackSize;
    PSONYDCAM_EXTENSION DeviceExtension;
    PSTREAMEX pStrmEx;
    Srb->Status = STATUS_SUCCESS;
    Irb = (PIRB) Srb->SRBExtension;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;

    DbgMsg2(("\'SonyDCamSetKSStatePAUSE: setting to PAUSE; PendingRead=%d, IsochDescriptorList(%s)\n", 
          DeviceExtension->PendingReadCount, IsListEmpty(&DeviceExtension->IsochDescriptorList)?"Empty":"!Empty"));

    Srb->Status = STATUS_SUCCESS;
    if ((pStrmEx->KSState == KSSTATE_ACQUIRE) ||
        (pStrmEx->KSState == KSSTATE_STOP)) {                          

       pStrmEx->FrameCaptured                = 0;     // Actual count
       pStrmEx->FrameInfo.DropCount      = 0;     
       pStrmEx->FrameInfo.PictureNumber = 0;
       pStrmEx->FrameInfo.dwFrameFlags      = 0;
          
       // Advanced one frame.
       pStrmEx->FirstFrameTime                = pStrmEx->pVideoInfoHeader->AvgTimePerFrame; //0;

       DbgMsg2(("\'SonyDCamSetKSStatePAUSE: FirstFrameTime(%d)\n", pStrmEx->FirstFrameTime));
    }

    if (pStrmEx->KSState == KSSTATE_RUN) {

       StackSize = DeviceExtension->BusDeviceObject->StackSize;
       Irp = IoAllocateIrp(StackSize, FALSE);

       if (!Irp) {
    
          SONYDCAM_ASSERT(Irp);
          Srb->Status = STATUS_INSUFFICIENT_RESOURCES;
          return;
       }


       //
       // stop the stream internally inside 1394 stack
       //
       
       if (DeviceExtension->hResource) {

          Irb->FunctionNumber = REQUEST_ISOCH_STOP;
          Irb->Flags = 0;
          Irb->u.IsochStop.hResource = DeviceExtension->hResource;
          Irb->u.IsochStop.fulFlags = 0;

          Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

          if (Srb->Status) {

             DbgMsg1(("\'SonyDCamSetKSStatePAUSE: Error %x while trying to Stop Isoch stream\n", Srb->Status));
             IoFreeIrp(Irp);
             return;
          }

       }
     
       IoFreeIrp(Irp);

     }

     DeviceExtension->bCameraStopped = FALSE;

}


VOID
SonyDCamSetKSStateRUN(
    IN PHW_STREAM_REQUEST_BLOCK Srb
                         )
/*++

Routine Description:

    Set KSSTATE to KSSTATE_RUN.

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/
{
    PIRB Irb;
    PIRP Irp;
    CCHAR StackSize;
    PSONYDCAM_EXTENSION DeviceExtension;


    DbgMsg2(("\'SonyDCamSetKSStateRUN: setting RUN stream state\n"));

    //
    // determine the type of packet.
    //

    Irb = (PIRB) Srb->SRBExtension;
    Srb->Status = STATUS_SUCCESS;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;

    StackSize = DeviceExtension->BusDeviceObject->StackSize;
    Irp = IoAllocateIrp(StackSize, FALSE);

    if (!Irp) {
       SONYDCAM_ASSERT(Irp);
       Srb->Status = STATUS_INSUFFICIENT_RESOURCES;
       DbgMsg1(("\'SonyDCamSetKSStateRUN: IoAllocateIrp failed\n"));
       return;
    }

    Irb->FunctionNumber = REQUEST_ISOCH_LISTEN;
    Irb->Flags = 0;
    Irb->u.IsochListen.hResource = DeviceExtension->hResource;
    Irb->u.IsochListen.fulFlags = 0;

    Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

    //
    // If we get an insufficient resources error that means
    // we don't have any Reads down yet.  Set flag to TRUE
    // indicating that when we do get a Read down we'll 
    // actually need to begin the listening process.
    //

    if (Srb->Status == STATUS_INSUFFICIENT_RESOURCES) {

       DeviceExtension->bNeedToListen = TRUE;
       DbgMsg1(("\'SonyDCamSetKSStateRUN: no read yet! set DeviceExtension->bNeedToListen = TRUE\n"));
    }
                        
    if ((Srb->Status) &&
        (Srb->Status != STATUS_INSUFFICIENT_RESOURCES)) {

        DbgMsg1(("\'SonyDCamSetKSStateRUN: Error %x while trying to start listening\n", Srb->Status));
        IoFreeIrp(Irp);
        return;
    }


    IoFreeIrp(Irp);  // not used below so free it.

    //
    // Four writes to the registers on the camera will 
    // get it going
    //

    //
    // Bit[24..26]0:0000 = CurrentFrameRate
    //
    DeviceExtension->RegisterWorkArea.AsULONG = DeviceExtension->FrameRate << 5;

    if ((Srb->Status = SonyDCamWriteRegister ((PIRB) Srb->SRBExtension, DeviceExtension, 
                        FIELDOFFSET(CAMERA_REGISTER_MAP, CurrentVFrmRate), DeviceExtension->RegisterWorkArea.AsULONG))) {
        DbgMsg1(("\'SonyDCamSetKSStateRUN: Error %x while writing to frame rate register\n", Srb->Status));
        return;
    }

    
    //
    // Bit[24..26]0:0000 = CurrentVideoMode
    //
    DeviceExtension->RegisterWorkArea.AsULONG = DeviceExtension->CurrentModeIndex << 5; 

    DbgMsg3(("\'SonyDCamSetKSStateRUN: CurrentVideoMode (ModeIndex=0x%8x) = 0x%8x\n", DeviceExtension->CurrentModeIndex, DeviceExtension->CurrentModeIndex << VIDEO_MODE_SHIFT, DeviceExtension->RegisterWorkArea.AsULONG));
    if ((Srb->Status = SonyDCamWriteRegister ((PIRB) Srb->SRBExtension, DeviceExtension, 
                        FIELDOFFSET(CAMERA_REGISTER_MAP, CurrentVMode), DeviceExtension->RegisterWorkArea.AsULONG))) {
        DbgMsg1(("SonyDCamReceiveCtrlPacket: Error %x while writing to video mode register\n", Srb->Status));
        return;
    }


    DeviceExtension->RegisterWorkArea.AsULONG = FORMAT_VGA_NON_COMPRESSED;
    if ((Srb->Status = SonyDCamWriteRegister ((PIRB) Srb->SRBExtension, DeviceExtension, 
                        FIELDOFFSET(CAMERA_REGISTER_MAP, CurrentVFormat), DeviceExtension->RegisterWorkArea.AsULONG))) {
        DbgMsg1(("\'SonyDCamSetKSStateRUN: Error %x while writing to Iso Channel register\n", Srb->Status));
        return;
    }


    //
    // Bit [24..27]:00[30..31] = IsoChannel:00SpeedCode
    // 
    DeviceExtension->RegisterWorkArea.AsULONG = (DeviceExtension->IsochChannel << 4) | DeviceExtension->SpeedCode;

    DbgMsg3(("\'SonyDCamSetKSStateRUN: IsochChannel=%d; IsoChannelSpeed(AsULONG)=0x%8x\n", DeviceExtension->IsochChannel, DeviceExtension->RegisterWorkArea.AsULONG));
    if (Srb->Status = SonyDCamWriteRegister ((PIRB) Srb->SRBExtension, DeviceExtension, 
                              FIELDOFFSET(CAMERA_REGISTER_MAP, IsoChannel), DeviceExtension->RegisterWorkArea.AsULONG)) {
        DbgMsg1(("\'SonyDCamSetKSStateRUN: Error %x while writing to Iso Channel register\n", Srb->Status)); 
        return;
    }


    //
    // Bit[24]000:0000 = start ? 1 : 0;
    //
    DeviceExtension->RegisterWorkArea.AsULONG = START_ISOCH_TRANSMISSION;
    if ((Srb->Status = SonyDCamWriteRegister ((PIRB) Srb->SRBExtension, DeviceExtension, 
                              FIELDOFFSET(CAMERA_REGISTER_MAP, IsoEnable), DeviceExtension->RegisterWorkArea.AsULONG))) {
        DbgMsg1(("\'SonyDCamSetKSStateRUN: Error %x while writing to Iso Enable register\n", Srb->Status));
        return;
    }

    DeviceExtension->bCameraStopped = FALSE;

    DbgMsg2(("\'SonyDCamSetKSStateRUN: Complete setting RUN stream state\n"));

}


#define DELAY_TIME_MSEC          1200
#define BUS_SETTLE_TIME          (ULONG)(-1 * DELAY_TIME_MSEC * 10000) 

NTSTATUS
SonyDCamStreamReset(
    PHW_STREAM_REQUEST_BLOCK Srb
    )
/*++

Routine Description:

    After bus reset, the original streaming state is restored.

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    STATUS_SUCCESS

--*/
{
     
    PSONYDCAM_EXTENSION DeviceExtension;
    PSTREAMEX pStrmEx;
    KEVENT hEvent;
    LARGE_INTEGER WaitCount;
    NTSTATUS statusWait;
    ULONG     CR_Node_UniqueID[2];
    KSSTATE KSStateSave;

     
     
    ASSERT(Srb != NULL);
    if (Srb == 0) {
        DbgMsg1(("\'SonyDCamStreamReset:Srb is 0 (cannot get DeviceExtension).\n\n"));
        return STATUS_SUCCESS;
    }

    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    ASSERT(DeviceExtension != NULL);
    if (DeviceExtension == 0) {
        DbgMsg1(("\'%d:%s) SonyDCamStreamReset:DeviceExtension is 0.\n\n", DeviceExtension->idxDev, DeviceExtension->pchVendorName));
        return STATUS_SUCCESS;
    }

    //
    // Block all SRB_READ_DATA; will be enabled when state is set to PAUSE or RUN.
    //
    DeviceExtension->bCameraStopped = TRUE;
          

    //
    // Get New Unique ID: 
    //          ResetStream if the same 
    //          Device unplug if the operation may fail or the number has been changed.
    //
    // Config Rom format (always at 0xffff f000:0400)
    // Unique id start at 040c
    //
    // Read each number twice.
    //
    DbgMsg2(("\'SonyDCamStreamReset: >>>> Begin: Try to read CR_Node_UniqueID\n"));

    Srb->Status = SonyDCamReadRegisterAbsolute ((PIRB) Srb->SRBExtension, DeviceExtension, 
          0xf000040c, &CR_Node_UniqueID[0]);
    if (Srb->Status) {

        DbgMsg2(("\'%d:%s) SonyDCamStreamReset:Read ID failed (status=%x; ID=%x); try again.\n", DeviceExtension->idxDev, DeviceExtension->pchVendorName, Srb->Status, CR_Node_UniqueID[0]));
        Srb->Status = SonyDCamReadRegisterAbsolute ((PIRB) Srb->SRBExtension, DeviceExtension, 
               0xf000040c, &CR_Node_UniqueID[0]);
    }

    if (!Srb->Status) {
        Srb->Status = SonyDCamReadRegisterAbsolute ((PIRB) Srb->SRBExtension, DeviceExtension, 
                         0xf0000410, &CR_Node_UniqueID[1]);

        if (Srb->Status) {
            DbgMsg2(("\'%d:%s) SonyDCamStreamReset:Read ID failed (status=%x; ID=%x); try again.\n", DeviceExtension->idxDev, DeviceExtension->pchVendorName, Srb->Status, CR_Node_UniqueID[1]));
            Srb->Status = SonyDCamReadRegisterAbsolute ((PIRB) Srb->SRBExtension, DeviceExtension, 
                    0xf0000410, &CR_Node_UniqueID[1]);

        }          
    }

    DbgMsg2(("\'SonyDCamStreamReset: End: Try to read CR_Node_UniqueID <<<< \n"));


    //
    // Wait for a fix time for the bus noise to settle down
    //
    KeInitializeEvent( &hEvent, SynchronizationEvent, FALSE );
    WaitCount.HighPart = -1;
    WaitCount.LowPart = BUS_SETTLE_TIME; // * 100nsec
    statusWait = KeWaitForSingleObject( &hEvent, Executive, KernelMode, FALSE, &WaitCount );


    pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;
    DbgMsg2(("\'%d:%s) SonyDCamStreamReset: !!! DevExt(0x%8x); StrmEx(0x%8x)!!!\n", 
          DeviceExtension->idxDev, DeviceExtension->pchVendorName, DeviceExtension, pStrmEx));

    if (pStrmEx == 0) {

        DbgMsg2(("\'%d:%s) SonyDCamStreamReset: pStrmEx(=0, no open stream); Do nothing.\n\n", DeviceExtension->idxDev, DeviceExtension->pchVendorName));
        return STATUS_SUCCESS;
    }


    //
    // If the stream was open (pStrmEx != NULL) when the bus reset 
    // and it stream was open successfull (pStrmEx->pVideoInfoHeader != NULL)
    //
 
    if (pStrmEx->pVideoInfoHeader != NULL) {

        DbgMsg2(("\'%d:%s) SonyDCamStreamReset: Stream was open; Try allocate them again.\n", DeviceExtension->idxDev, DeviceExtension->pchVendorName));
#if 1
        KSStateSave = pStrmEx->KSState;
#endif
        // Close the existing stream 
        if (pStrmEx->KSState == KSSTATE_PAUSE ||
            pStrmEx->KSState == KSSTATE_RUN) {

            // All running stream must stop since the bus is also resetted.
            SonyDCamStopStream(Srb);
            pStrmEx->KSState = KSSTATE_STOP;     // Suceeded or not; it is in STOP_STATE;
        }

        // All resources must be freed as well.
        SonyDCamFreeIsochResource(Srb);

        DbgMsg2(("\'%d:%s) SonyDCamStreamReset: CR_Node_UniqueID: New>>(%x:%x); Original>>(%x:%x)\n", 
               DeviceExtension->idxDev, DeviceExtension->pchVendorName,
               CR_Node_UniqueID[0], CR_Node_UniqueID[1],
               DeviceExtension->ConfigRom->CR_Node_UniqueID[0], DeviceExtension->ConfigRom->CR_Node_UniqueID[1]));
          
        //
        // If read the UNIQUE ID has failed (Srb->Status != 0) or
        // if the new Unique ID does not match the original one.
        //
        if (Srb->Status ||
            DeviceExtension->ConfigRom->CR_Node_UniqueID[0] != CR_Node_UniqueID[0] ||
            DeviceExtension->ConfigRom->CR_Node_UniqueID[1] != CR_Node_UniqueID[1]) {

            DbgMsg1(("\'%d:%s) SonyDCamStreamReset: Getting UniqueID has failed (%d) or not my ID so assume the device is removed.\n", 
                    DeviceExtension->idxDev, DeviceExtension->pchVendorName, Srb->Status));

            return STATUS_SUCCESS;
        }


        // Save new unique IDs
        DeviceExtension->ConfigRom->CR_Node_UniqueID[0] = CR_Node_UniqueID[0];
        DeviceExtension->ConfigRom->CR_Node_UniqueID[1] = CR_Node_UniqueID[1];


        //
        // Initialize the device to a known state 
        //
        DeviceExtension->RegisterWorkArea.AsULONG = 0;
        DeviceExtension->RegisterWorkArea.Initialize.Initialize = TRUE;
        DeviceExtension->RegisterWorkArea.AsULONG = bswap(DeviceExtension->RegisterWorkArea.AsULONG);
        Srb->Status = SonyDCamWriteRegister ((PIRB) Srb->SRBExtension, DeviceExtension, 
                  FIELDOFFSET(CAMERA_REGISTER_MAP, Initialize), DeviceExtension->RegisterWorkArea.AsULONG);

        //  If it failed, then it is an unplug??
        if (Srb->Status) {

            DbgMsg1(("\'%d:%s) SonyDCamStreamReset: Intialize DCam failed(Staus=%x) so assume the device is removed.\n\n",
                   DeviceExtension->idxDev, DeviceExtension->pchVendorName, Srb->Status));
#if 1
            ASSERT(Srb->Status == STATUS_SUCCESS);
#endif
            return STATUS_SUCCESS;
        }


        //
        // set the VideoProcAmp and CameraControl property information in a static array
        // for the video stream.  
        // 
        SetCurrentDevicePropertyValues(Srb, DeviceExtension);


        // Subset of DCamOpenStream()
        Srb->Status = SonyDCamAllocateIsochResource(Srb);

        if (Srb->Status) {

            DbgMsg1(("\'%d:%s) SonyDCamStreamReset: Re-AllocateIsochResource failed! Status=%x\n\n", 
                    DeviceExtension->idxDev, DeviceExtension->pchVendorName, Srb->Status));
#if 1
            ASSERT(Srb->Status == STATUS_SUCCESS);
#endif
            return STATUS_SUCCESS; // Srb->Status;
        }


        // restore its original running state
#if 0
        switch (pStrmEx->KSState) {
#else
        switch (KSStateSave) {
#endif
        case KSSTATE_RUN:
               
             SonyDCamSetKSStateRUN(Srb);
             pStrmEx->KSState = KSStateSave;
             break;

        case KSSTATE_PAUSE:

             SonyDCamSetKSStatePAUSE(Srb);               
             pStrmEx->KSState = KSStateSave;
             break;

        case KSSTATE_STOP:
             pStrmEx->KSState = KSStateSave;
             break;

        default:
             DbgMsg1(("\'SonyDCamStreamReset: Unknown KSSTATE=%d !! KSSTATE_STOP=%d\n", KSStateSave, KSSTATE_STOP));
             ASSERT(pStrmEx->KSState != KSSTATE_STOP);
             pStrmEx->KSState = KSSTATE_STOP;             
             break;
        }

      } else {

        DbgMsg2(("\'%d:%s) SonyDCamStreamReset: Stream was not open successful earlier. Do nothing!\n\n", 
               DeviceExtension->idxDev, DeviceExtension->pchVendorName));

     }


     return STATUS_SUCCESS;
}

VOID
SonyDCamProcessPnpIrp(
    IN PHW_STREAM_REQUEST_BLOCK Srb,
    PIO_STACK_LOCATION IrpStack,
    PSONYDCAM_EXTENSION DeviceExtension
    )

/*++

Routine Description:

    Process PnP Irp.

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/
{
    ULONG ulOrgGeneration = DeviceExtension->CurrentGeneration;


    switch (IrpStack->MinorFunction) {
  
    case IRP_MN_BUS_RESET: 

         ulOrgGeneration = DeviceExtension->CurrentGeneration;
         DeviceExtension->CurrentGeneration = *((PULONG) &IrpStack->Parameters.Others.Argument4);

         //
         // We're going to restart the stream at this point
         //
         // If the current state of the event object is Signaled, 
         // a nonzero value is returned. Otherwise, zero is returned. 

         // 0:Non-Signal; !0:Signal
         if (KeReadStateSemaphore(&DeviceExtension->semaBusReset)) {

             NTSTATUS statusKeWait;

             DbgMsg3(("\'SonyDCamProcessPnpIrp: Enter KeWaitForSingleObject\n"));

             // KeWait* decrement the count: was 1 (initialize) decrment to 0=>Non-signal;Other wait.
             statusKeWait = KeWaitForSingleObject( &DeviceExtension->semaBusReset, Executive, KernelMode, TRUE, 0 );

             DbgMsg3(("\'SonyDCamProcessPnpIrp: statusKeWait=%d\n", statusKeWait));
             DbgMsg2(("\'%d:%s) SonyDCamProcessPnpIrp: Bus Reset: OldGen=(%d) NewGen=%d\n", 
                      DeviceExtension->idxDev, DeviceExtension->pchVendorName,
                      ulOrgGeneration, *((PULONG) &IrpStack->Parameters.Others.Argument4)));
                         
             Srb->Status = SonyDCamStreamReset(Srb);

             // No priority increment; adjust by 1 to singal; not followed by KeWait*
             KeReleaseSemaphore(&DeviceExtension->semaBusReset, 0, 1, 0);       
             DbgMsg2(("\'SonyDCamProcessPnpIrp: KeReleaseSemaphore\n"));

         } else {

             Srb->Status = STATUS_SUCCESS;
             DbgMsg2(("\'SonyDCamProcessPnpIrp: >> Generation++=(%d); ignore this bus reset.\n", DeviceExtension->CurrentGeneration));
         }

         break;
            
    case IRP_MN_QUERY_CAPABILITIES: 
         DbgMsg1(("\'SonyDCamProcessPnpIrp: IRP_MN_QUERY_CAPABILITIES: Srb->Status = STATUS_NOT_IMPLEMENTED.\n"));
    default:
         Srb->Status = STATUS_NOT_IMPLEMENTED;
         break;
    }
}

VOID 
SonyDCamChangePower(
    IN PHW_STREAM_REQUEST_BLOCK Srb
)

/*++

Routine Description:

    Process chnaging this device's power state.  

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/
{
    PSONYDCAM_EXTENSION DeviceExtension;
     PSTREAMEX pStrmEx;

    Srb->Status = STATUS_SUCCESS;

    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    ASSERT(DeviceExtension != NULL);
    if (DeviceExtension ==NULL)
        return;


    pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;     
    if (pStrmEx ==NULL) {
        DbgMsg1(("SonyDCamChangePower: stream is not yet running; setting its power state.\n"));
        return;
    }

    //
    // Only when it is not streaming, its power state can be changed.
    // We have "DontSuspendIfStreamsAreRunning" turn on in the INF.
    //
    if (pStrmEx->KSState == KSSTATE_PAUSE ||
        pStrmEx->KSState == KSSTATE_RUN) {

        Srb->Status = STATUS_UNSUCCESSFUL;
    }      
}

VOID
SonyDCamReceivePacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    This is where most of the interesting Stream requests come to us

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/

{

    PIO_STACK_LOCATION IrpStack;
    PSONYDCAM_EXTENSION DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;


    //
    // Switch on the command within the Srb itself
    //

    switch (Srb->Command) {

    case SRB_INITIALIZE_DEVICE:     // Per device
          
         Srb->Status = SonyDCamHwInitialize(Srb);
         break;

    case SRB_GET_STREAM_INFO:     // Per Device

         //
         // this is a request for the driver to enumerate requested streams
         //
         SonyDCamGetStreamInfo(Srb);
         break;

    case SRB_OPEN_STREAM:          // Per stream

         SonyDCamOpenStream(Srb);
         break;

    case SRB_CLOSE_STREAM:          // Per Stream

         if (DeviceExtension->pStrmEx != (PSTREAMEX) Srb->StreamObject->HwStreamExtension) {
             DbgMsg3(("\'%d:%s) SonyDCamReceivePacket/SRB_CLOSE_STREAM: DeviceExtension->pStrmEx != (PSTREAMEX) Srb->StreamObject->HwStreamExtension\n",
                      DeviceExtension->idxDev, DeviceExtension->pchVendorName));
             DbgMsg3(("\'%d:%s) This instance already been unuinitialized !!\n",
                      DeviceExtension->idxDev, DeviceExtension->pchVendorName));
         }

         SonyDCamCloseStream(Srb);
         break;

    case SRB_UNINITIALIZE_DEVICE:     // Per device

         Srb->Status = SonyDCamHwUnInitialize(Srb);
         break;

    case SRB_GET_DATA_INTERSECTION:

         //
         // Return a format, given a range
         //
         AdapterFormatFromRange(Srb);
         break;

    case SRB_CHANGE_POWER_STATE:

         SonyDCamChangePower(Srb);
         DbgMsg1(("\'SonyDCamReceivePacket/SRB_CHANGE_POWER_STATE: status=%s\n", Srb->Status == STATUS_SUCCESS ? "STATUS_SUCCESS" : "STATUS_UNSUCCESSFUL"));
         break;
            
    case SRB_UNKNOWN_DEVICE_COMMAND:     // Per device

         //
         // We might be interested in unknown commands if they pertain
         // to bus resets.  Bus resets are important cuz we need to know
         // what the current generation count is.
         //
         IrpStack = IoGetCurrentIrpStackLocation(Srb->Irp);

         if (IrpStack->MajorFunction == IRP_MJ_PNP)
             SonyDCamProcessPnpIrp(Srb, IrpStack, DeviceExtension);
         else            
             Srb->Status = STATUS_NOT_IMPLEMENTED;
         break;



    // VideoProcAmp and CameraControl requests
    case SRB_GET_DEVICE_PROPERTY:

         AdapterGetProperty(Srb);
         break;
          
    case SRB_SET_DEVICE_PROPERTY:
    
         AdapterSetProperty(Srb);
         break;

    case SRB_PAGING_OUT_DRIVER:

         Srb->Status = STATUS_NOT_IMPLEMENTED;
         DbgMsg1(("\'SonyDCamReceivePacket: SRB_PAGING_OUT_DRIVER; not implemented.\n"));
         break;

    default:   

         DbgMsg1(("\'SonyDCamReceivePacket: entry with unknown command = 0x%x\n", Srb->Command));
         //
         // this is a request that we do not understand.  Indicate invalid
         // command and complete the request
         //

         Srb->Status = STATUS_NOT_IMPLEMENTED;
         break;
    }

    //
    // NOTE:
    //
    // all of the commands that we do, or do not understand can all be completed
    // synchronously at this point, so we can use a common callback routine here.
    // If any of the above commands require asynchronous processing, this will
    // have to change
    //

#if defined(DBG)
    if (Srb->Status != STATUS_SUCCESS) {
        DbgMsg1(("\'Srb->Command(0x%x) does not return STATUS_SUCCESS but 0x%x\n", Srb->Command, Srb->Status));
    }
#endif

    StreamClassDeviceNotification(DeviceRequestComplete, Srb->HwDeviceExtension, Srb);

}



VOID
SonyDCamGetStreamInfo(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    Returns the information of all streams that are supported by the driver

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/

{
     
    //
    // pick up the pointer to the stream information data structure
    //

    PHW_STREAM_HEADER StreamHeader = &(Srb->CommandData.StreamBuffer->StreamHeader);        
    PSONYDCAM_EXTENSION DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    PHW_STREAM_INFORMATION StreamInfo = &(Srb->CommandData.StreamBuffer->StreamInfo);


    DbgMsg2(("\'SonyDCamGetStreamInfo: enter\n"));

    //
    // set number of streams
    //

    ASSERT (Srb->NumberOfBytesToTransfer >= 
            sizeof (HW_STREAM_HEADER) +
            sizeof (HW_STREAM_INFORMATION));

    //
    // initialize stream header
    //

    RtlZeroMemory(StreamHeader, 
                sizeof (HW_STREAM_HEADER) +
                sizeof (HW_STREAM_INFORMATION));

    //
    // initialize the number of streams supported
    //

    StreamHeader->NumberOfStreams = 1;
    StreamHeader->SizeOfHwStreamInformation = sizeof(HW_STREAM_INFORMATION);


    //
    // set the VideoProcAmp and CameraControl property information in a static array
    // for the video stream.  
    // It also queries the property value range, set default value.
    // 
    InitilizePropertyArray(Srb);


    //
    // Initialize the stream structure.
    //
    // Number of instances field indicates the number of concurrent streams
    // of this type the device can support.  
    //

    StreamInfo->NumberOfPossibleInstances = 1;

    //
    // indicates the direction of data flow for this stream, relative to 
    // the driver
    //

    StreamInfo->DataFlow = KSPIN_DATAFLOW_OUT;

    //
    // dataAccessible - Indicates whether the data is "seen" by the host
    // processor.
    //

    StreamInfo->DataAccessible = TRUE;

    //
    // Return how many formats this stream supports 
    //

    StreamInfo->NumberOfFormatArrayEntries = NUM_DCAM_STREAM_FORMATS;
    StreamInfo->StreamFormatsArray = &SONYDCAM_StreamFormats[0];

    //
    // set the property information for the video stream
    //

    StreamInfo->NumStreamPropArrayEntries = NUMBER_VIDEO_STREAM_PROPERTIES;
    StreamInfo->StreamPropertiesArray = (PKSPROPERTY_SET) VideoStreamProperties;

    //
    // set the pin name and category
    //

    StreamInfo->Name = (GUID *) &PINNAME_VIDEO_CAPTURE;
    StreamInfo->Category = (GUID *) &PINNAME_VIDEO_CAPTURE;


    //
    // store a pointer to the topology for the device
    //
        
    Srb->CommandData.StreamBuffer->StreamHeader.Topology = &Topology;


    //
    // indicate success
    //

    Srb->Status = STATUS_SUCCESS;

}


NTSTATUS
SonyDCamAllocateIsochResource(
    IN PHW_STREAM_REQUEST_BLOCK Srb
     )
{
    PIRB Irb;
    PIRP Irp;
    CCHAR StackSize;
    ULONG ModeIndex;
    PSONYDCAM_EXTENSION DeviceExtension;
    PSTREAMEX pStrmEx;
    DWORD dwAvgTimePerFrame, dwCompression;
    ULONG fulSpeed;


    Irb = (PIRB) Srb->SRBExtension;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;

    DbgMsg2(("\'SonyDCamAllocateIsochResource: enter (pStrmEx(0x%8x) pVideoInfo(0x%8x))\n", pStrmEx, pStrmEx->pVideoInfoHeader));
    //
    // Now if they're on a YUV4:2:2 format, we've gotta check what
    // resolution they want it at, since we support this format
    // but in two different resolutions (modes on the camera).
    //

    // This is the INDEX to the frame rate and resource allocation; see IsochInfoTable.
    // 0 : reserved
    // 1 : 3.75
    // 2 : 7.5
    // 3 : 15 (DEFAULT_FRAME_RATE)
    // 4 : 30 
    // 5 : 60 (Not supported for Mode 1 & 3)
    dwAvgTimePerFrame = (DWORD) pStrmEx->pVideoInfoHeader->AvgTimePerFrame;
    dwCompression = (DWORD) pStrmEx->pVideoInfoHeader->bmiHeader.biCompression;

#define TIME_ROUNDING                       10   // Give it some rounding error of 1microsec
#define TIME_0750FPS      1333333+TIME_ROUNDING   // 1/7.50 * 10,000,000 (unit=100ns)
#define TIME_1500FPS       666666+TIME_ROUNDING   // 1/15.0 * 10,000,000 (unit=100ns)  do not round to 666667
#define TIME_3000FPS       333333+TIME_ROUNDING   // 1/30.0 * 10,000,000 (unit=100ns)


    // Determine the Frame rate
    if (dwAvgTimePerFrame      > TIME_0750FPS) 
        DeviceExtension->FrameRate = 1;        //  3.75FPS
    else if (dwAvgTimePerFrame >  TIME_1500FPS) 
        DeviceExtension->FrameRate = 2;        //  7.5FPS
    else if (dwAvgTimePerFrame >  TIME_3000FPS) 
        DeviceExtension->FrameRate = 3;        // 15 FPS
    else 
        DeviceExtension->FrameRate = 4;        // 30 FPS


    DbgMsg2(("\'SonyDCamAllocateIsochResource: FrameRate: %d FPS\n", (1 << (DeviceExtension->FrameRate-1)) * 15 / 4));

    // Determine the Video Mode
    switch(dwCompression) {
          
    case FOURCC_UYVY:     // Mode 1 or 3
         if (pStrmEx->pVideoInfoHeader->bmiHeader.biWidth == 640 &&
             (pStrmEx->pVideoInfoHeader->bmiHeader.biHeight == 480 || 
              pStrmEx->pVideoInfoHeader->bmiHeader.biHeight == -480)) 
              ModeIndex = 3;
         else
              ModeIndex = 1;
         break;

    case FOURCC_IYUV:     // Mode 2
         ModeIndex = 2;
         break;

    case KS_BI_RGB:  // = 0
         ModeIndex = 4;
         break;

    default:          
         Srb->Status = STATUS_NOT_IMPLEMENTED;;
         return Srb->Status;;
    }


    DbgMsg2(("\'SonyDCamAllocateIsochResource: ModeIndex=%d, AvgTimePerFrame=%d, FrameRate=%d\n", 
             ModeIndex, dwAvgTimePerFrame, DeviceExtension->FrameRate));

    //
    // Get an Irp so we can send some allocation commands down
    //

    StackSize = DeviceExtension->BusDeviceObject->StackSize;
    Irp = IoAllocateIrp(StackSize, FALSE);

    if (!Irp) {
        Srb->Status = STATUS_INSUFFICIENT_RESOURCES;
        SONYDCAM_ASSERT(Irp != NULL);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Calculate the index to use to reference the ISOCH table
    //
    pStrmEx->idxIsochTable = ModeIndex * NUM_POSSIBLE_RATES + DeviceExtension->FrameRate;

    SONYDCAM_ASSERT(pStrmEx->pVideoInfoHeader->bmiHeader.biSizeImage == IsochInfoTable[pStrmEx->idxIsochTable].CompletePictureSize);
    DbgMsg2(("\'SonyDCamAllocateIsochResource: ModeIndex=%d, idxIsochTable=%d, biSizeImage=%d, CompletePictureSize=%d\n", 
             ModeIndex, pStrmEx->idxIsochTable, pStrmEx->pVideoInfoHeader->bmiHeader.biSizeImage, IsochInfoTable[pStrmEx->idxIsochTable].CompletePictureSize));          

#if 1
    //
    // 0. Determine the MAX_SPEED and not use the speed defined in the static table.
    //
    Irb->FunctionNumber = REQUEST_GET_SPEED_BETWEEN_DEVICES;
    Irb->Flags = 0;
    Irb->u.GetMaxSpeedBetweenDevices.fulFlags = USE_LOCAL_NODE;
    Irb->u.GetMaxSpeedBetweenDevices.ulNumberOfDestinations = 0;
    Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);
    if (Srb->Status) {
            DbgMsg1(("\'SonyDCamAllocateIsochResource: Error %x while trying to get maximun speed between devices.\n", Srb->Status));        

        IoFreeIrp(Irp);
        return STATUS_INSUFFICIENT_RESOURCES;
    }     
    
    fulSpeed = Irb->u.GetMaxSpeedBetweenDevices.fulSpeed;
#endif


#if defined(DBG)
    
    switch (fulSpeed) {
    case SPEED_FLAGS_100:
        DbgMsg1(("\'SonyDCamAllocateIsochResource: fulSpeed=100\n"));
        break;
    case SPEED_FLAGS_200:
        DbgMsg1(("\'SonyDCamAllocateIsochResource: fulSpeed=200\n"));
        break;
    case SPEED_FLAGS_400:
        DbgMsg1(("\'SonyDCamAllocateIsochResource: fulSpeed=200\n"));
        break;
    case SPEED_FLAGS_1600:
        DbgMsg1(("\'SonyDCamAllocateIsochResource: fulSpeed=400\n"));
        break;
    default:
        DbgMsg1(("\'SonyDCamAllocateIsochResource: SpeedRequired(%d)=unknown; Set to known(%d).\n", 
            fulSpeed, IsochInfoTable[pStrmEx->idxIsochTable].SpeedRequired));
        ASSERT(fulSpeed != SPEED_FLAGS_100 || fulSpeed != SPEED_FLAGS_200 || fulSpeed != SPEED_FLAGS_400 || fulSpeed != SPEED_FLAGS_1600);
        fulSpeed = IsochInfoTable[pStrmEx->idxIsochTable].SpeedRequired;
        break;
    }
#endif

    //
    // 1. Allocate BANDWIDTH
    //
    Irb->FunctionNumber = REQUEST_ISOCH_ALLOCATE_BANDWIDTH;
    Irb->Flags = 0;
    Irb->u.IsochAllocateBandwidth.nMaxBytesPerFrameRequested = IsochInfoTable[pStrmEx->idxIsochTable].QuadletPayloadPerPacket << 2;
    Irb->u.IsochAllocateBandwidth.fulSpeed = fulSpeed;
    Irb->u.IsochAllocateBandwidth.hBandwidth = 0;
    Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);
    
    if (Srb->Status) {

        DbgMsg1(("\'SonyDCamAllocateIsochResource: Error %x while trying to allocate Isoch bandwidth\n", Srb->Status));                  
        IoFreeIrp(Irp);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    DeviceExtension->hBandwidth = Irb->u.IsochAllocateBandwidth.hBandwidth;

    //
    // 2. Allocate CHANNEL
    //
    Irb->FunctionNumber = REQUEST_ISOCH_ALLOCATE_CHANNEL;
    Irb->Flags = 0;

    //
    //      ULONG           nRequestedChannel;      // Need a specific channel
    //      ULONG           Channel;                // Returned channel
    //          LARGE_INTEGER   ChannelsAvailable;      // Channels available
    // Instead of hardcoded '0'; use -1 to ask the bus driver to get the next available channel for us.

    Irb->u.IsochAllocateChannel.nRequestedChannel = ISOCH_ANY_CHANNEL; // ISOCH_ANY_CHANNEL == -1
    Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);
    if (Srb->Status) {

        DbgMsg1(("\'SonyDCamAllocateIsochResource: Error %x while trying to allocate Isoch channel\n", Srb->Status));
             
        // Free bandwidth
        Irb->FunctionNumber = REQUEST_ISOCH_FREE_BANDWIDTH;
        Irb->Flags = 0;
        Irb->u.IsochFreeBandwidth.hBandwidth = DeviceExtension->hBandwidth;
        Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);
        DeviceExtension->hBandwidth = NULL;
        
        if (Srb->Status) {
            DbgMsg1(("\'SonyDCamAllocateIsochResource: Error %x while trying to free Isoch bandwidth\n", Srb->Status));
        }

        IoFreeIrp(Irp);
        return STATUS_INSUFFICIENT_RESOURCES;
    }     
    DbgMsg2(("\'SonyDCamAllocateIsochResource: DeviceExtension->IsochChannel(was %d) = %d\n", DeviceExtension->IsochChannel, Irb->u.IsochAllocateChannel.Channel));
    DeviceExtension->IsochChannel = Irb->u.IsochAllocateChannel.Channel;  // Save for de-allocation



    //
    // 3. Allocate RESOURCES
    //
    Irb->FunctionNumber = REQUEST_ISOCH_ALLOCATE_RESOURCES;
    Irb->Flags = 0;

    Irb->u.IsochAllocateResources.fulSpeed = IsochInfoTable[pStrmEx->idxIsochTable].SpeedRequired;
    Irb->u.IsochAllocateResources.nChannel = DeviceExtension->IsochChannel;
    Irb->u.IsochAllocateResources.nMaxBytesPerFrame = IsochInfoTable[pStrmEx->idxIsochTable].QuadletPayloadPerPacket << 2;
    // For slower frame rate use smaller quadlets
    // smaller frame size will use more packet to fill the same amount of data
    // this is why smaller frame rate actually demand more resource !!
    Irb->u.IsochAllocateResources.nNumberOfBuffers = MAX_BUFFERS_SUPPLIED;
    Irb->u.IsochAllocateResources.nMaxBufferSize = IsochInfoTable[pStrmEx->idxIsochTable].CompletePictureSize;
    if (DeviceExtension->HostControllerInfomation.HostCapabilities & HOST_INFO_SUPPORTS_RETURNING_ISO_HDR) {       
        Irb->u.IsochAllocateResources.nQuadletsToStrip = 1;
        Irb->u.IsochAllocateResources.fulFlags = RESOURCE_USED_IN_LISTENING | RESOURCE_STRIP_ADDITIONAL_QUADLETS;

    } else {
        Irb->u.IsochAllocateResources.nQuadletsToStrip = 0;
        Irb->u.IsochAllocateResources.fulFlags = RESOURCE_USED_IN_LISTENING;
    }

    Irb->u.IsochAllocateResources.hResource = 0;
    DbgMsg3(("\'SonyDCamAllocateIsochResource: fullSpeed(%d), nMaxBytesPerFrame(%d), nMaxBufferSize(%d)\n", 
                              Irb->u.IsochAllocateResources.fulSpeed,
                              Irb->u.IsochAllocateResources.nMaxBytesPerFrame,
                              Irb->u.IsochAllocateResources.nMaxBufferSize));
    Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);
          
    if (Srb->Status) {

        DbgMsg1(("\'SonyDCamAllocateIsochResource: Error %x while trying to allocate Isoch resources\n", Srb->Status));

        // Free bandwidth
        Irb->FunctionNumber = REQUEST_ISOCH_FREE_BANDWIDTH;
        Irb->Flags = 0;
        Irb->u.IsochFreeBandwidth.hBandwidth = DeviceExtension->hBandwidth;
        Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);
        DeviceExtension->hBandwidth = NULL;
        if (Srb->Status) {
            DbgMsg1(("\'SonyDCamAllocateIsochResource: Error %x while trying to free Isoch bandwidth\n", Srb->Status));
        }

        // Free channel
        Irb->FunctionNumber = REQUEST_ISOCH_FREE_CHANNEL;
        Irb->Flags = 0;
        Irb->u.IsochFreeChannel.nChannel = DeviceExtension->IsochChannel;
        Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);
        DeviceExtension->IsochChannel = (ULONG) -1;
        if (Srb->Status) {
            DbgMsg1(("\'SonyDCamAllocateIsochResource: Error %x while trying to free Isoch resource\n", Srb->Status));
        }

        IoFreeIrp(Irp);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    DeviceExtension->hResource = Irb->u.IsochAllocateResources.hResource;
    DeviceExtension->CurrentModeIndex = ModeIndex;

    DeviceExtension->bCameraStopped = TRUE; 
    IoFreeIrp(Irp);     

    return STATUS_SUCCESS;
}



VOID 
InitializeStreamExtension(
    PSONYDCAM_EXTENSION DeviceExtension,
    PHW_STREAM_OBJECT   pStreamObject,
    PSTREAMEX           pStrmEx
    )
{

    pStrmEx->hMasterClock = 0;
    pStrmEx->FrameInfo.ExtendedHeaderSize = sizeof(KS_FRAME_INFO);
    pStrmEx->FrameInfo.PictureNumber = 0;
    pStrmEx->FrameInfo.DropCount     = 0;
    pStrmEx->FrameInfo.dwFrameFlags  = 0;     
    pStrmEx->FirstFrameTime    = 0;
    pStrmEx->pVideoInfoHeader  = 0;
    pStrmEx->KSState           = KSSTATE_STOP;
}


VOID
SonyDCamOpenStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    Called when an OpenStream Srb request is received

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/

{

    PIRB Irb;
    ULONG nSize;
    PSONYDCAM_EXTENSION DeviceExtension;
    PSTREAMEX pStrmEx;
    PKS_DATAFORMAT_VIDEOINFOHEADER  pKSDataFormat = 
                (PKS_DATAFORMAT_VIDEOINFOHEADER) Srb->CommandData.OpenFormat;
    PKS_VIDEOINFOHEADER     pVideoInfoHdrRequested = 
                &pKSDataFormat->VideoInfoHeader;




    Irb = (PIRB) Srb->SRBExtension;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    pStrmEx = (PSTREAMEX)Srb->StreamObject->HwStreamExtension;

    DbgMsg2(("\'SonyDCamOpenStream: >>> !!! DevEx(0x%8x); StrmEx(0x%8x) !!!\n", DeviceExtension, pStrmEx));

    //
    // Cache the stream extension.
    //

    DeviceExtension->pStrmEx = pStrmEx; 


    //
    // default to success
    //

    Srb->Status = STATUS_SUCCESS;

    //
    // determine which stream number is being opened.  This number indicates
    // the offset into the array of streaminfo structures that was filled out
    // in the AdapterStreamInfo call.
    //
    // So:
    //   0 - Video data from camera
    //

    switch (Srb->StreamObject->StreamNumber) {

    case 0:

         //
         // Figure out what format they're trying to open first
         //

         if (!AdapterVerifyFormat (pKSDataFormat, Srb->StreamObject->StreamNumber)) {
             DbgMsg1(("\'SonyDCamOpenStream: AdapterVerifyFormat failed.\n"));
             Srb->Status = STATUS_INVALID_PARAMETER;
             return;
         }

         InitializeStreamExtension(DeviceExtension, Srb->StreamObject, pStrmEx);

         // It should already been freed by SonyDCamCloseStream()
         ASSERT(pStrmEx->pVideoInfoHeader == NULL);
         ASSERT(pVideoInfoHdrRequested != (PKS_VIDEOINFOHEADER) 0);

         // Use this instead of sizeof(KS_VIDEOINFOHEADER) to handle variable size structure
         nSize = KS_SIZE_VIDEOHEADER (pVideoInfoHdrRequested);
         pStrmEx->pVideoInfoHeader = ExAllocatePool(NonPagedPool, nSize);
         if (pStrmEx->pVideoInfoHeader == NULL) {

             DbgMsg1(("\'SonyDCamOpenStream: ExAllocatePool (->pVideoInfoHeader) failed!\n"));
             ASSERT(pStrmEx->pVideoInfoHeader != NULL);

			 DeviceExtension->pStrmEx = NULL;

             Srb->Status = STATUS_INSUFFICIENT_RESOURCES;
             return;
         }

         // Copy the VIDEOINFOHEADER requested to our storage
         RtlCopyMemory(
                    pStrmEx->pVideoInfoHeader,
                    pVideoInfoHdrRequested,
                    nSize);

         DbgMsg3(("\'SonyDCamOpenStream: Copied biSizeImage=%d Duration=%ld (100ns)\n", 
                    pStrmEx->pVideoInfoHeader->bmiHeader.biSizeImage, (DWORD) pStrmEx->pVideoInfoHeader->AvgTimePerFrame));

         // Allocate ISOCH resource
         Srb->Status = SonyDCamAllocateIsochResource(Srb);               
		 
         if (Srb->Status) {

             DbgMsg1(("\'SonyDCamOpenStream: !!!! Allocate ISOCH resource failed.  CanNOT STREAM!!!!!\n"));
             
			 ExFreePool(pStrmEx->pVideoInfoHeader);
             pStrmEx->pVideoInfoHeader = NULL;			 

             DeviceExtension->pStrmEx = NULL;
             Srb->Status = STATUS_INSUFFICIENT_RESOURCES;			 
             return;
         } 

         Srb->StreamObject->ReceiveDataPacket = (PVOID) SonyDCamReceiveDataPacket;
         Srb->StreamObject->ReceiveControlPacket = (PVOID) SonyDCamReceiveCtrlPacket;

         //
         // initialize the stream extension data handling information
         //

         break;

    default:
         DbgMsg1(("\'SonyDCamOpenStream: Hit a non-support Srb->StreamObject->StreamNumber (%d).\n", Srb->StreamObject->StreamNumber));
         ASSERT(FALSE);
         Srb->Status = STATUS_INVALID_PARAMETER;
         return;
    }


    Srb->StreamObject->HwClockObject.ClockSupportFlags = 0;

    // We don't use DMA.
    Srb->StreamObject->Dma = FALSE;
    Srb->StreamObject->StreamHeaderMediaSpecific = sizeof(KS_FRAME_INFO);

    //
    // The PIO flag must be set when the mini driver will be accessing the data
    // buffers passed in using logical addressing
    //
#if 1
    Srb->StreamObject->Pio = FALSE;
#else
    Srb->StreamObject->Pio = TRUE;  // So we can touch the data buffer;
#endif

#if defined(DBG)
    cntOpenStream++;
    DeviceExtension->idxDev = cntOpenStream;
#endif

    //
    // Cache the stream extension.
    //
    ASSERT(Srb->Status == STATUS_SUCCESS);

    DbgMsg2(("\'%d:%s) SonyDCamOpenStream exit: IRQL=%d; DevExt=%x; StrmEx=%x; Status=%x\n", 
          DeviceExtension->idxDev, DeviceExtension->pchVendorName, 
          KeGetCurrentIrql(), DeviceExtension, pStrmEx, Srb->Status));

}


VOID
SonyDCamFreeIsochResource (
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )
/*++

Routine Description:

    Free resource allocated in SonyDCamAllocateIsochResource().

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/
{
    PIRB Irb;
    PIRP Irp;
    CCHAR StackSize;
    PSONYDCAM_EXTENSION DeviceExtension;


    DbgMsg2(("\'SonyDCamFreeIsochResource: enter\n"));

    Irb = (PIRB) Srb->SRBExtension;
    Srb->Status = STATUS_SUCCESS;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    ASSERT(DeviceExtension != NULL);

    //
    // Get an Irp so we can send some free commands down
    //
    StackSize = DeviceExtension->BusDeviceObject->StackSize;
    Irp = IoAllocateIrp(StackSize, FALSE);

    if (!Irp) {
        Srb->Status = STATUS_INSUFFICIENT_RESOURCES;    
        DbgMsg1(("\'SonyDCamFreeIsochResource: Error %x while trying to allocate an Irp\n\n", Srb->Status));
        SONYDCAM_ASSERT(Irp != NULL);
        return;
    }


    //
    // 1. Free Resource
    //
    if (DeviceExtension->hResource) {
#if 1
        DbgMsg2(("\'SonyDCamFreeIsochResource: Attempt to free ->hResource\n"));
#endif
        Irb->FunctionNumber = REQUEST_ISOCH_FREE_RESOURCES;
        Irb->Flags = 0;
        Irb->u.IsochFreeResources.hResource = DeviceExtension->hResource;
        Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

        DeviceExtension->hResource = NULL;
        if (Srb->Status) {

            DbgMsg1(("\'SonyDCamFreeIsochResource: Error %x while trying to free Isoch resources\n\n", Srb->Status));
            ASSERT(Srb->Status == STATUS_SUCCESS);
#if 0
            IoFreeIrp(Irp);
            return;
#endif
        }
    }

    //
    // 2. Free Channel
    //
    if (DeviceExtension->IsochChannel != (ULONG) -1) {
#if 1
        DbgMsg2(("\'SonyDCamFreeIsochResource: Attempt to free ->IsochChannel\n"));
#endif
        Irb->FunctionNumber = REQUEST_ISOCH_FREE_CHANNEL;
        Irb->Flags = 0;
        Irb->u.IsochFreeChannel.nChannel = DeviceExtension->IsochChannel;
        Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

        DeviceExtension->IsochChannel = (ULONG) -1;

        if (Srb->Status) {
            
            DbgMsg1(("\'SonyDCamFreeIsochResource: Error %x while trying to free Isoch channel\n\n", Srb->Status));
            ASSERT(Srb->Status == STATUS_SUCCESS);
#if 0
            IoFreeIrp(Irp);
            return;
#endif
        }
    }

    //
    // 3. Free Bandwidth
    //
    if (DeviceExtension->hBandwidth) {
#if 1
        DbgMsg2(("\'SonyDCamFreeIsochResource: Attempt to free ->hBandwidth\n"));
#endif
        Irb->FunctionNumber = REQUEST_ISOCH_FREE_BANDWIDTH;
        Irb->Flags = 0;
        Irb->u.IsochFreeBandwidth.hBandwidth = DeviceExtension->hBandwidth;
        Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

        DeviceExtension->hBandwidth = NULL;

        if (Srb->Status) {

            DbgMsg1(("\'SonyDCamFreeIsochResource: Error %x while trying to free Isoch bandwidth\n", Srb->Status));
            ASSERT(Srb->Status == STATUS_SUCCESS);
#if 0
            IoFreeIrp(Irp);
            return;
#endif
        }
    }

    IoFreeIrp(Irp);
}


VOID
SonyDCamCloseStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )
/*++

Routine Description:

    Called when an CloseStream Srb request is received

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/
{

    PIRB Irb;
    PSONYDCAM_EXTENSION DeviceExtension;
    PSTREAMEX     pStrmEx;




    Irb = (PIRB) Srb->SRBExtension;
    Srb->Status = STATUS_SUCCESS;

    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    ASSERT(DeviceExtension != NULL);

    if (!DeviceExtension) {
        DbgMsg1(("\'SonyDcamCloseStream: DevExt=0; Can this happen ???"));
        return;
    }

    pStrmEx = (PSTREAMEX)DeviceExtension->pStrmEx;

    DbgMsg2(("\'%d:%s) SonyDCamCloseStream: DevEx(0x%x)\n", 
          DeviceExtension->idxDev, DeviceExtension->pchVendorName, DeviceExtension, pStrmEx));

    if ( !pStrmEx ) {

        DbgMsg1(("\'%d:%s) SonyDCamCloseStream: Stream (pStrmEx=0) is not open.\n", DeviceExtension->idxDev, DeviceExtension->pchVendorName));

        return;
    
    } else {
        DbgMsg2(("\'%d:%s) SonyDCamCloseStream: Stream (pStrmEx=%x) is open; Release IsochResource.\n", DeviceExtension->idxDev, DeviceExtension->pchVendorName, pStrmEx));
    }


#if 0
     //
     // What if device is unplug while streaming ?
     // BusRest shoudl take care of it.
     //
     if (pStrmEx->KSState == KSSTATE_PAUSE ||
          pStrmEx->KSState == KSSTATE_RUN) {

          DbgMsg2(("\'SonyDCamCloseStream: Closing but stream is not yet stop; Stop it here!\n"));

          SonyDCamStopStream(Srb);
          pStrmEx->KSState == KSSTATE_STOP;     // Suceeded or not; it is in STOP_STATE;

                        ASSERT(Srb->Status == STATUS_SUCCESS);
          DbgMsg2(("\'SonyDCamCloseStream: Rtn from StopStream; Srb->Status ?= 0x%x (SU&CCESS=0x%x)\n", Srb->Status, STATUS_SUCCESS));
     }
#else
     //
     // Instead of automatically run StopStream, let's catch it here.  
     // We should be stop before close.
     //
     DbgMsg2(("\'SonyDCamCloseStream: KSSTATE=%d\n", pStrmEx->KSState )); 
     ASSERT(pStrmEx->KSState != KSSTATE_PAUSE && pStrmEx->KSState != KSSTATE_RUN);
#endif


    // Reset all handle
    pStrmEx->hMasterClock = 0;

    //
    // Save the current property to the registry
    //
    SetPropertyValuesToRegistry(DeviceExtension);

    SonyDCamFreeIsochResource (Srb);

    if (pStrmEx->pVideoInfoHeader) {

        ExFreePool(pStrmEx->pVideoInfoHeader);
        pStrmEx->pVideoInfoHeader = NULL;
    }

    DbgMsg3(("\'%d:%s)SonyDCamCloseStream: DeviceExtension->pStrmEx = 0\n", 
          DeviceExtension->idxDev, DeviceExtension->pchVendorName));

    DeviceExtension->pStrmEx = 0;
#if defined(DBG)
    cntOpenStream--;
    DbgMsg2(("\'SonyDcamCloseStreeam:<< Currently, %d stream(s) open. >>>\n", cntOpenStream));
#endif


    // Just in case it is changed by other all.
    Srb->Status = STATUS_SUCCESS;
}


VOID
SonyDCamReceiveDataPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    Called with video data packet commands

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/

{
 
    //
    // determine the type of packet.
    //

    switch (Srb->Command) {

    case SRB_READ_DATA:
    
         DbgMsg3(("\'SonyDCamReceiveDataPacket: SRB_READ_DATA\n"));        
         SonyDCamReadStream(Srb);

         // This request will be completed asynchronously...
        
         break;
            
    case SRB_WRITE_DATA:

         DbgMsg3(("\'SonyDCamReceiveDataPacket: SRB_WRITE_DATA\n"));
         Srb->Status = STATUS_NOT_IMPLEMENTED;

         // This request will be completed asynchronously...
            
         break;

     default:

         //
         // invalid / unsupported command. Fail it as such
         //

         Srb->Status = STATUS_NOT_IMPLEMENTED;
    
         StreamClassStreamNotification(StreamRequestComplete, Srb->StreamObject, Srb);

    }

}


VOID
SonyDCamReceiveCtrlPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    Called with packet commands that control the video stream

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/

{

    PSONYDCAM_EXTENSION DeviceExtension;
    PSTREAMEX pStrmEx;


    //
    // determine the type of packet.
    //

    Srb->Status = STATUS_SUCCESS;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;     
    pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;

    switch (Srb->Command) {

    case SRB_GET_STREAM_STATE:
         
         VideoGetState (Srb);
         break;

    case SRB_SET_STREAM_STATE:
     
         DbgMsg3(("\'%d:%s) SonyDCamReceiveCtrlPacket: Setting state from %d to %d\n", 
                    DeviceExtension->idxDev, DeviceExtension->pchVendorName, pStrmEx->KSState, Srb->CommandData.StreamState));
               
         if (pStrmEx->KSState < KSSTATE_STOP ||
             pStrmEx->KSState > KSSTATE_RUN) {
         
             DbgMsg1(("\'%d:%s) SonyDCamReceiveCtrlPacket: Setting from unknown state (%d) break; do nothing.\n",
                      DeviceExtension->idxDev, DeviceExtension->pchVendorName, pStrmEx->KSState));
             DbgMsg1(("\'%d:%s) SonyDCamReceiveCtrlPacket: DevExt=%x; has this been uninitialized ?\n\n",
                      DeviceExtension->idxDev, DeviceExtension->pchVendorName, DeviceExtension));

             Srb->Status = STATUS_NOT_IMPLEMENTED;
             break;
         }

         switch (Srb->CommandData.StreamState) {

         case KSSTATE_STOP:

              SonyDCamStopStream(Srb);
              break;

         case KSSTATE_PAUSE:

              SonyDCamSetKSStatePAUSE(Srb);
              break;

         case KSSTATE_RUN:

              SonyDCamSetKSStateRUN(Srb);
              break;

         case KSSTATE_ACQUIRE:

              Srb->Status = STATUS_SUCCESS;
              break;

         default:

              DbgMsg1(("\'SonyDCamReceiveCtrlPacket: Error unknown state\n"));
              Srb->Status = STATUS_NOT_IMPLEMENTED;
              break;
         }

         pStrmEx->KSState = Srb->CommandData.StreamState;
         break;

    case SRB_GET_STREAM_PROPERTY:

         DbgMsg3(("\'SonyDCamReceiveDataPacket: SRB_GET_STREAM_PROPERTY\n"));
         VideoGetProperty(Srb);
         break;

    case SRB_INDICATE_MASTER_CLOCK:

         //
         // Assigns a clock to a stream
         //
         VideoIndicateMasterClock (Srb);
         break;

    default:

         //
         // invalid / unsupported command. Fail it as such
         //    
         Srb->Status = STATUS_NOT_IMPLEMENTED;
    }

    StreamClassStreamNotification(StreamRequestComplete, Srb->StreamObject, Srb);

}


VOID
SonyDCamReadStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    Called when an Read Data Srb request is received

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/

{

    PIRB Irb;
    PIRP Irp;
    KIRQL oldIrql;
    PSONYDCAM_EXTENSION DeviceExtension;
    PSTREAMEX     pStrmEx;
    PISOCH_DESCRIPTOR IsochDescriptor;
    PISOCH_DESCRIPTOR_RESERVED IsochDescriptorReserved;


    Irb = (PIRB) Srb->SRBExtension;
    Irp = Srb->Irp;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    ASSERT(DeviceExtension != NULL);

    pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;
    ASSERT(pStrmEx != NULL);

    if (pStrmEx == NULL) {

        Srb->Status = STATUS_UNSUCCESSFUL;
        Srb->CommandData.DataBufferArray->DataUsed = 0;
        StreamClassStreamNotification(StreamRequestComplete, Srb->StreamObject, Srb);
        return;        
    }
    
    DbgMsg3(("\'%d:%s) SonyDCamReadStream: enter with Srb = %x, DevExt=0x%x\n", 
            DeviceExtension->idxDev, DeviceExtension->pchVendorName, Srb, DeviceExtension));

    //
    // If the camera is stopped right now, return an error.
    //

    // Rule: 
    // Only accept read requests when in either the Pause or Run
    // States.  If Stopped, immediately return the SRB.

    if (pStrmEx->KSState == KSSTATE_STOP ||
        pStrmEx->KSState == KSSTATE_ACQUIRE || 
        DeviceExtension->bCameraStopped) {

        DbgMsg2(("\'%d:%s)SonyDCamReadStream: KSState(%d) is not RUN/PAUSE or bCameraStopped(TRUE); Srb=0x%x; DevExt=0x%x\n", 
                 DeviceExtension->idxDev, DeviceExtension->pchVendorName, pStrmEx->KSState,  Srb, DeviceExtension));

        Srb->Status = STATUS_UNSUCCESSFUL;
        Srb->CommandData.DataBufferArray->DataUsed = 0;
        StreamClassStreamNotification(StreamRequestComplete, Srb->StreamObject, Srb);
        return;        
    }

#if defined(DBG)
    if (IsochInfoTable[pStrmEx->idxIsochTable].CompletePictureSize != Srb->CommandData.DataBufferArray->FrameExtent) {
        DbgMsg3(("\'SonyDCamReadStream: ExpectedBufLen=%d; Client:FrameExt=%d\n", 
                 IsochInfoTable[pStrmEx->idxIsochTable].CompletePictureSize, Srb->CommandData.DataBufferArray->FrameExtent));
    }
#endif

    // Buffer need to be big enough
    if (IsochInfoTable[pStrmEx->idxIsochTable].CompletePictureSize > Srb->CommandData.DataBufferArray->FrameExtent) {

        SONYDCAM_ASSERT(IsochInfoTable[pStrmEx->idxIsochTable].CompletePictureSize <= Srb->CommandData.DataBufferArray->FrameExtent);
        Srb->Status = STATUS_INVALID_PARAMETER;
        StreamClassStreamNotification(StreamRequestComplete, Srb->StreamObject, Srb);
        return;
    }


    IsochDescriptor = ExAllocatePool(NonPagedPool, sizeof(ISOCH_DESCRIPTOR));
    if (!IsochDescriptor) {

        SONYDCAM_ASSERT(FALSE);
        Srb->Status = STATUS_INSUFFICIENT_RESOURCES;
        StreamClassStreamNotification(StreamRequestComplete, Srb->StreamObject, Srb);
        return;
    }


    DbgMsg3(("\'SonyDCamReadStream: IsochDescriptor = %x\n", IsochDescriptor));
    IsochDescriptor->fulFlags = SYNCH_ON_SY;

    DbgMsg3(("\'SonyDCamReadStream: Incoming Mdl = %x\n", Irp->MdlAddress));
    IsochDescriptor->Mdl = Irp->MdlAddress;


    // Use size match what we originally requested in AllocateIsoch
    IsochDescriptor->ulLength = IsochInfoTable[pStrmEx->idxIsochTable].CompletePictureSize;
    IsochDescriptor->nMaxBytesPerFrame = IsochInfoTable[pStrmEx->idxIsochTable].QuadletPayloadPerPacket << 2;

    IsochDescriptor->ulSynch = START_OF_PICTURE;
    IsochDescriptor->ulTag = 0;
    IsochDescriptor->Callback = SonyDCamIsochCallback;
    IsochDescriptor->Context1 = DeviceExtension;
    IsochDescriptor->Context2 = IsochDescriptor;
    IsochDescriptorReserved = (PISOCH_DESCRIPTOR_RESERVED) &IsochDescriptor->DeviceReserved[0];
    IsochDescriptorReserved->Srb = Srb;
    IsochDescriptorReserved->Flags = 0;

    //
    // Checking here to see if we have enuff resources to put this read
    // down right away.  Since we only allocated N amount of resources
    // from the 1394 stack beneath us, we'll have to stay within that 
    // limit and do some of the throttling ourself.
    //

    KeAcquireSpinLock(&DeviceExtension->IsochWaitingLock, &oldIrql);
    if (InterlockedIncrement(&DeviceExtension->PendingReadCount) > MAX_BUFFERS_SUPPLIED) {

        //
        // don't have enuff resources to do an attach buffers right now.
        // we'll queue this request and pull it off later when another
        // read completes.
        //

        DbgMsg2(("\'SonyDCamReadStream: Queueing request - Read Count = %x\n", DeviceExtension->PendingReadCount));
        InsertTailList(
           &DeviceExtension->IsochWaitingList,
           &IsochDescriptorReserved->DescriptorList
           );

        KeReleaseSpinLock(&DeviceExtension->IsochWaitingLock, oldIrql);
        return;

    }

    //
    // Do actual read work here via our Read worker function
    //

    KeReleaseSpinLock(&DeviceExtension->IsochWaitingLock, oldIrql);
    SonyDCamReadStreamWorker(Srb, IsochDescriptor);

}


VOID
SonyDCamReadStreamWorker(
    IN PHW_STREAM_REQUEST_BLOCK Srb,
    IN PISOCH_DESCRIPTOR IsochDescriptor
    )

/*++

Routine Description:

    Does most of the work for handling reads via Attach buffers

Arguments:

    Srb - Pointer to Stream request block

    IsochDescriptor - Pointer to IsochDescriptor to be used

Return Value:

    Nothing

--*/

{

    PIRB Irb;
    PIRP Irp;
    KIRQL oldIrql;
    NTSTATUS status;
    PIO_STACK_LOCATION NextIrpStack;
    PSONYDCAM_EXTENSION DeviceExtension;
    PISOCH_DESCRIPTOR_RESERVED IsochDescriptorReserved;


    Irb = (PIRB) Srb->SRBExtension;
    Irp = Srb->Irp;
    Srb->Status = STATUS_PENDING;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    IsochDescriptorReserved = (PISOCH_DESCRIPTOR_RESERVED) &IsochDescriptor->DeviceReserved[0];

    DbgMsg3(("\'%d:%s) SonyDCamReadStreamWorker: enter with Srb = %x, DevExt=0x%x\n", 
             DeviceExtension->idxDev, DeviceExtension->pchVendorName, Srb, DeviceExtension));

    //
    // Attach descriptor onto our pending descriptor list
    //

    ExInterlockedInsertTailList(
       &DeviceExtension->IsochDescriptorList,
       &IsochDescriptorReserved->DescriptorList,
       &DeviceExtension->IsochDescriptorLock
       );

    Irb->FunctionNumber = REQUEST_ISOCH_ATTACH_BUFFERS;
    Irb->Flags = 0;
    Irb->u.IsochAttachBuffers.hResource = DeviceExtension->hResource;
    Irb->u.IsochAttachBuffers.nNumberOfDescriptors = 1;
    Irb->u.IsochAttachBuffers.pIsochDescriptor = IsochDescriptor;

    status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

#if defined(DBG)
    if (DeviceExtension->pStrmEx->hMasterClock) {
        ULONGLONG tmStream;
                    
        tmGetStreamTime(IsochDescriptorReserved->Srb, DeviceExtension->pStrmEx, &tmStream);

        DbgMsg3(("\'SonyDCamReadStreamWorker: BufferAttached time(%d), PendingRead=%d\n", 
                 (ULONG) tmStream/10000, DeviceExtension->PendingReadCount));
          
    }
#endif

    if ((status) &&
        (status != STATUS_PENDING)) {

        DbgMsg1(("\'%d:%s) SonyDCamReadStreamWorker: Error %x while trying to attach Isoch buffers\n", 
                 DeviceExtension->idxDev, DeviceExtension->pchVendorName, status));

        if (!(IsochDescriptorReserved->Flags & STATE_SRB_IS_COMPLETE)) {

            IsochDescriptorReserved->Flags |= STATE_SRB_IS_COMPLETE;
            Srb->Status = status;

            KeAcquireSpinLock(&DeviceExtension->IsochDescriptorLock, &oldIrql);
            RemoveEntryList(&IsochDescriptorReserved->DescriptorList);  // Remove an random entry from list
            KeReleaseSpinLock(&DeviceExtension->IsochDescriptorLock, oldIrql);

            ExFreePool(IsochDescriptor);
            StreamClassStreamNotification(StreamRequestComplete, Srb->StreamObject, Srb);

            KeAcquireSpinLock(&DeviceExtension->IsochWaitingLock, &oldIrql);

            if (InterlockedDecrement(&DeviceExtension->PendingReadCount) >= MAX_BUFFERS_SUPPLIED) {

                KeReleaseSpinLock(&DeviceExtension->IsochWaitingLock, oldIrql);
               
                                            //
                // We had someone blocked waiting for us to complete.  Pull
                // them off the waiting list and get them running
                //

                DbgMsg3(("\'%d:%s) SonyDCamReadStreamWorker: Dequeueing request - Read Count = %x\n", 
                         DeviceExtension->idxDev, DeviceExtension->pchVendorName, DeviceExtension->PendingReadCount));
                SONYDCAM_ASSERT ( !IsListEmpty(&DeviceExtension->IsochWaitingList) );
                IsochDescriptorReserved = \
                    (PISOCH_DESCRIPTOR_RESERVED) ExInterlockedRemoveHeadList(
                       &DeviceExtension->IsochWaitingList,
                       &DeviceExtension->IsochWaitingLock
                        );

                IsochDescriptor = \
                    (PISOCH_DESCRIPTOR) (((PUCHAR) IsochDescriptorReserved) - 
                        FIELDOFFSET(ISOCH_DESCRIPTOR, DeviceReserved[0]));

                SonyDCamReadStreamWorker(IsochDescriptorReserved->Srb, IsochDescriptor);
                                 }

            KeReleaseSpinLock(&DeviceExtension->IsochWaitingLock, oldIrql);

        } else {

            DbgMsg1(("\'SonyDCamReadStreamWorker: IsochDescriptorReserved->Flags contain STATE_SRB_IS_COMPLETE\n")); 
                      }

        return;

    }

    if (DeviceExtension->bNeedToListen) {

        DbgMsg2(("\'SonyDCamReadStreamWorker: DeviceExtension->bNeedToListen\n"));

        DeviceExtension->bNeedToListen = FALSE;
        DeviceExtension->Irb->FunctionNumber = REQUEST_ISOCH_LISTEN;
        DeviceExtension->Irb->Flags = 0;
        DeviceExtension->Irb->u.IsochListen.hResource = DeviceExtension->hResource;
        DeviceExtension->Irb->u.IsochListen.fulFlags = 0;
 
        NextIrpStack = IoGetNextIrpStackLocation(DeviceExtension->Irp);
        NextIrpStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        NextIrpStack->Parameters.DeviceIoControl.IoControlCode = IOCTL_1394_CLASS;
        NextIrpStack->Parameters.Others.Argument1 = DeviceExtension->Irb;

        IoSetCompletionRoutine(
            DeviceExtension->Irp,
            SonyDCamListenCompletionRoutine,
            NULL,
            TRUE,
            TRUE,
            TRUE
            );

        IoCallDriver(DeviceExtension->BusDeviceObject, DeviceExtension->Irp);

    }

}
          
DWORD FlipImage (
    PBYTE pFlipBuf,
    PBYTE pSrc, 
    PKS_BITMAPINFOHEADER pbmHdr)
/*++
Routine Description:
     
       Flip a frame of image (not reverse it).  
       The size of the buffer must be greater than or equal to pbmHdr->biSizeImage!

Argument:

      pFlipBuf - Temp space (size == one line of data) used to swap data.
      pSrc - Point to the byte0 of the data
       pbmHdr - Pointer to the bitmapinfo header.

Return Value:

      0 : if it failed.
       biSizeImage : success.
--*/
{
    int   i, Height, Width;
    PBYTE pTemp;

    if (pFlipBuf == NULL || pSrc == NULL || pbmHdr == 0) {
         DbgMsg1(("\'FlipImage: pFlipBuf=0x%x; pSrc=0x%x; pbmHdr=0x%x\n", pFlipBuf, pSrc, pbmHdr));
         return 0;
    }

    Height = pbmHdr->biHeight >= 0 ? pbmHdr->biHeight : -pbmHdr->biHeight;
    Width  = pbmHdr->biWidth * pbmHdr->biBitCount / 8;

    ASSERT((DWORD) Width * Height == pbmHdr->biSizeImage);     
    ASSERT(Width > 0);

    pTemp  = pSrc + pbmHdr->biSizeImage - Width;

    DbgMsg3(("\'FlipImage:pSrc=%x; pTemp=%x, Width=%d, SizeImage=%d\n", pSrc, pTemp, Width, pbmHdr->biSizeImage));

    for(i=0; i < Height/2; i++) {               

        RtlCopyMemory(pFlipBuf, pSrc,     Width);
        RtlCopyMemory(pSrc,     pTemp,    Width);
        RtlCopyMemory(pTemp,    pFlipBuf, Width);
                    
        pSrc  += Width;
        pTemp -= Width;
    }

    return (pbmHdr->biSizeImage);
}


VOID
SonyDCamIsochCallback(
    IN PSONYDCAM_EXTENSION DeviceExtension,
    IN PISOCH_DESCRIPTOR IsochDescriptor
    )

/*++

Routine Description:

    Called when an Isoch Descriptor completes

Arguments:

    DeviceExtension - Pointer to our DeviceExtension

    IsochDescriptor - IsochDescriptor that completed

Return Value:

    Nothing

--*/

{
    PIRB Irb;
    PIRP Irp;
    KIRQL oldIrql;
    PSTREAMEX pStrmEx;
    PIO_STACK_LOCATION NextIrpStack;
    PISOCH_DESCRIPTOR_RESERVED IsochDescriptorReserved;
    PKSSTREAM_HEADER pDataPacket;
    PKS_FRAME_INFO pFrameInfo;


    //
    // Debug check to make sure we're dealing with a real IsochDescriptor
    //

    SONYDCAM_ASSERT ( IsochDescriptor );

    IsochDescriptorReserved = (PISOCH_DESCRIPTOR_RESERVED) &IsochDescriptor->DeviceReserved[0];
    pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;

    ASSERT(pStrmEx == (PSTREAMEX)IsochDescriptorReserved->Srb->StreamObject->HwStreamExtension);

    pStrmEx->FrameCaptured++;
    pStrmEx->FrameInfo.PictureNumber = pStrmEx->FrameCaptured + pStrmEx->FrameInfo.DropCount;

    DbgMsg3(("\'%d) SonyDCamIsochCallback(IRQL=%d): FrameCaptured=%d\n", 
          DeviceExtension->idxDev, KeGetCurrentIrql(), pStrmEx->FrameCaptured));


    //
    // Make sure this IsochDescriptor is not already marked as completed
    //

    if (!(IsochDescriptorReserved->Flags & STATE_SRB_IS_COMPLETE)) {

        //
        // Return the timestamp for the frame
        //

        pDataPacket = IsochDescriptorReserved->Srb->CommandData.DataBufferArray;
        pFrameInfo = (PKS_FRAME_INFO) (pDataPacket + 1);

        SONYDCAM_ASSERT ( pDataPacket );
        SONYDCAM_ASSERT ( pFrameInfo );

        //
        // Return the timestamp for the frame
        //
        pDataPacket->PresentationTime.Numerator = 1;
        pDataPacket->PresentationTime.Denominator = 1;
        pDataPacket->Duration = pStrmEx->pVideoInfoHeader->AvgTimePerFrame;

        //
        // if we have a master clock
        // 
        if (pStrmEx->hMasterClock) {

            ULONGLONG tmStream;
                    
            tmGetStreamTime(IsochDescriptorReserved->Srb, pStrmEx, &tmStream);

            pDataPacket->PresentationTime.Time = tmStream;

            pDataPacket->OptionsFlags = 0;

            //if (pStrmEx->bDiscontinue) {                    
            //     pDataPacket->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_DATADISCONTINUITY;
            //     pStrmEx->bDiscontinue = FALSE;
            //}
          
            pDataPacket->OptionsFlags |= 
                 KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
                 KSSTREAM_HEADER_OPTIONSF_DURATIONVALID |
                 KSSTREAM_HEADER_OPTIONSF_SPLICEPOINT;     // Every frame we generate is a key frame (aka SplicePoint)

            //pStrmEx->PreviousFrameTime = pDataPacket->PresentationTime.Time;
               
            DbgMsg3(("\'IsochCallback: Time(%dms); P#(%d)=Cap(%d)+Drp(%d); Pend%d\n",
                    (ULONG) tmStream/10000,
                    (ULONG) pStrmEx->FrameInfo.PictureNumber,
                    (ULONG) pStrmEx->FrameCaptured,
                    (ULONG) pStrmEx->FrameInfo.DropCount,
                    DeviceExtension->PendingReadCount));

        } else {

            pDataPacket->PresentationTime.Time = 0;
            pDataPacket->OptionsFlags &=                       
                    ~(KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
                  KSSTREAM_HEADER_OPTIONSF_DURATIONVALID);
        }

        // Set additional info fields about the data captured such as:
        //   Frames Captured
        //   Frames Dropped
        //   Field Polarity
                
        pStrmEx->FrameInfo.ExtendedHeaderSize = pFrameInfo->ExtendedHeaderSize;
        *pFrameInfo = pStrmEx->FrameInfo;

        //
        // Indicate that we're in the Detachment phase
        //

        IsochDescriptorReserved->Flags |= STATE_DETACHING_BUFFERS;
        Irb = DeviceExtension->Irb;
        Irp = DeviceExtension->Irp;  
        Irb->FunctionNumber = REQUEST_ISOCH_DETACH_BUFFERS;
        Irb->u.IsochDetachBuffers.hResource = DeviceExtension->hResource;
        Irb->u.IsochDetachBuffers.nNumberOfDescriptors = 1;
        Irb->u.IsochDetachBuffers.pIsochDescriptor = IsochDescriptor;

        NextIrpStack = IoGetNextIrpStackLocation(Irp);
        NextIrpStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        NextIrpStack->Parameters.DeviceIoControl.IoControlCode = IOCTL_1394_CLASS;
        NextIrpStack->Parameters.Others.Argument1 = Irb;

        IoSetCompletionRoutine(
            Irp,
            SonyDCamCompletionRoutine,
            IsochDescriptor,
            TRUE,
            TRUE,
            TRUE
            );
          
        IoCallDriver(DeviceExtension->BusDeviceObject, Irp);


        //
        // Indicate that the Srb should be complete
        //

        IsochDescriptorReserved->Flags |= STATE_SRB_IS_COMPLETE;
        IsochDescriptorReserved->Srb->Status = STATUS_SUCCESS;
        IsochDescriptorReserved->Srb->CommandData.DataBufferArray->DataUsed = IsochDescriptor->ulLength;
        IsochDescriptorReserved->Srb->ActualBytesTransferred = IsochDescriptor->ulLength;

        DbgMsg3(("\'SonyDCamIsochCallback: Completing Srb %x\n", IsochDescriptorReserved->Srb));

        KeAcquireSpinLock(&DeviceExtension->IsochDescriptorLock, &oldIrql);
        RemoveEntryList(&IsochDescriptorReserved->DescriptorList);
        KeReleaseSpinLock(&DeviceExtension->IsochDescriptorLock, oldIrql);

        StreamClassStreamNotification(
               StreamRequestComplete, 
               IsochDescriptorReserved->Srb->StreamObject, 
               IsochDescriptorReserved->Srb);

        // Free it here instead of in SonyDcamComjpletionRoutine.
        ExFreePool(IsochDescriptor);     


        KeAcquireSpinLock(&DeviceExtension->IsochWaitingLock, &oldIrql);
        if (InterlockedDecrement(&DeviceExtension->PendingReadCount) >= MAX_BUFFERS_SUPPLIED) {
            KeReleaseSpinLock(&DeviceExtension->IsochWaitingLock, oldIrql);

            //
            // We had someone blocked waiting for us to complete.  Pull
            // them off the waiting list and get them running
            //

            DbgMsg3(("SonyDCamIsochCallback: Dequeueing request - Read Count = %x\n", DeviceExtension->PendingReadCount));
            SONYDCAM_ASSERT ( !IsListEmpty(&DeviceExtension->IsochWaitingList) );
            IsochDescriptorReserved = \
                   (PISOCH_DESCRIPTOR_RESERVED) ExInterlockedRemoveHeadList(
                   &DeviceExtension->IsochWaitingList,
                   &DeviceExtension->IsochWaitingLock
                   );

            IsochDescriptor = \
                   (PISOCH_DESCRIPTOR) (((PUCHAR) IsochDescriptorReserved) - 
                   FIELDOFFSET(ISOCH_DESCRIPTOR, DeviceReserved[0]));

            SonyDCamReadStreamWorker(IsochDescriptorReserved->Srb, IsochDescriptor);

        }
        KeReleaseSpinLock(&DeviceExtension->IsochWaitingLock, oldIrql);

    }
    
}


NTSTATUS
SonyDCamCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PISOCH_DESCRIPTOR IsochDescriptor
    )

/*++

Routine Description:

    Returns more processing required so the IO Manager will leave us alone

Arguments:

    DriverObject - Pointer to driver object created by system.

    Irp - Irp that just completed

    IsochDescriptor - This Isoch descriptor will be freed

Return Value:

    None.

--*/

{

    return (STATUS_MORE_PROCESSING_REQUIRED);

}


VOID
SonyDCamCancelPacket(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    Cancel routine for Srbs (not implemented yet)

Arguments:

    Srb - Pointer to the stream request block

Return Value:

    None.

--*/

{
    PSONYDCAM_EXTENSION DeviceExtension;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;

    DbgMsg2(( "%d) SonyDcamCancelPacket: Do nothing!!\n", DeviceExtension->idxDev));
    return;

}


VOID
SonyDCamTimeoutHandler(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    This routine is called when a packet has been in the minidriver too long

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/

{

    DbgMsg2(("\'SonyDCamTimeoutHandler: TIMEOUT!  Srb %x hasn't completed!\n", Srb));

}


NTSTATUS
SonyDCamListenCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID SystemArgument
    )

/*++

Routine Description:

    Returns more processing required so the IO Manager will leave us alone

Arguments:

    DriverObject - Pointer to driver object created by system.

    Irp - Irp that just completed

    SystemArgument - Pointer to NULL

Return Value:

    None.

--*/

{

    return (STATUS_MORE_PROCESSING_REQUIRED);

}


VOID
SonyDCamStopStream(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )

/*++

Routine Description:

    Called to stop the stream (if running)

Arguments:

    Srb - Pointer to Stream request block

Return Value:

    Nothing

--*/

{

    PIRB Irb;
    PIRP Irp;
    CCHAR StackSize;
    PSONYDCAM_EXTENSION DeviceExtension;
    PSTREAMEX pStrmEx;
    PISOCH_DESCRIPTOR IsochDescriptor;
    PISOCH_DESCRIPTOR_RESERVED IsochDescriptorReserved;
    

    //
    // Get the Irb and DeviceExtension out of the Srb
    //



    Irb = (PIRB) Srb->SRBExtension;
    DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;

    DbgMsg2(("\'%d:%s)SonyDCamStopStream: enter with DevExt=0x%8x; StrmEx=0x%8x\n", 
          DeviceExtension->idxDev, DeviceExtension->pchVendorName, DeviceExtension, pStrmEx));

    if (!pStrmEx) {

        DbgMsg2(("\'SonyDCamStopStream: StrmEx(=0); there is nothing to stop.\n"));
        return;
    }

    StackSize = DeviceExtension->BusDeviceObject->StackSize;

    Irp = IoAllocateIrp(StackSize, FALSE);

    if (!Irp) {
    
        SONYDCAM_ASSERT(Irp);
        Srb->Status = STATUS_INSUFFICIENT_RESOURCES;
        return;

    }

    //
    // First stop the stream internally inside the PC's 1394
    // stack
    //

    if (DeviceExtension->hResource) {

        Irb->FunctionNumber = REQUEST_ISOCH_STOP;
        Irb->Flags = 0;
        Irb->u.IsochStop.hResource = DeviceExtension->hResource;
        Irb->u.IsochStop.fulFlags = 0;

        Srb->Status = SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

        if (Srb->Status) {

            DbgMsg1(("\'%d) SonyDCamStopStream: Error %x while trying to Stop Isoch stream\n", Srb->Status));
            IoFreeIrp(Irp);
            return;

        }

    }

    //
    // Now stop the stream at the device itself
    //
    DeviceExtension->RegisterWorkArea.AsULONG = STOP_ISOCH_TRANSMISSION;
    if ((Srb->Status = SonyDCamWriteRegister ((PIRB) Srb->SRBExtension, DeviceExtension, 
                  FIELDOFFSET(CAMERA_REGISTER_MAP, IsoEnable), DeviceExtension->RegisterWorkArea.AsULONG))) {
         DbgMsg1(("\'SonyDCamStopStream: Error %x while stopping at Iso Enable register\n", Srb->Status));            
    }

    //
    // Detach all buffers that might still be attached.
    //

    while (!IsListEmpty(&DeviceExtension->IsochDescriptorList)) {

        IsochDescriptorReserved = \
            (PISOCH_DESCRIPTOR_RESERVED) ExInterlockedRemoveHeadList(
               &DeviceExtension->IsochDescriptorList,
               &DeviceExtension->IsochDescriptorLock
                );

        IsochDescriptor = \
            (PISOCH_DESCRIPTOR) (((PUCHAR) IsochDescriptorReserved) -
                FIELDOFFSET(ISOCH_DESCRIPTOR, DeviceReserved[0]));

        if (!(IsochDescriptorReserved->Flags & STATE_SRB_IS_COMPLETE)) {

            DbgMsg1(("\'SonyDCamStopStream: stopping IsochDescriptor %x Srb %x\n", IsochDescriptor, IsochDescriptorReserved->Srb));

            IsochDescriptorReserved->Flags |= STATE_SRB_IS_COMPLETE;
            IsochDescriptorReserved->Srb->Status = STATUS_UNSUCCESSFUL;
            IsochDescriptorReserved->Srb->CommandData.DataBufferArray->DataUsed = 0;
            StreamClassStreamNotification(
                StreamRequestComplete,
                IsochDescriptorReserved->Srb->StreamObject,
                IsochDescriptorReserved->Srb
                );

            IsochDescriptorReserved->Flags |= STATE_DETACHING_BUFFERS;
            Irb->FunctionNumber = REQUEST_ISOCH_DETACH_BUFFERS;
            Irb->Flags = 0;
            Irb->u.IsochDetachBuffers.hResource = DeviceExtension->hResource;
            Irb->u.IsochDetachBuffers.nNumberOfDescriptors = 1;
            Irb->u.IsochDetachBuffers.pIsochDescriptor = IsochDescriptor;

            SonyDCamSubmitIrpSynch(DeviceExtension, Irp, Irb);

            ExFreePool(IsochDescriptor);

            if (InterlockedDecrement(&DeviceExtension->PendingReadCount) >= MAX_BUFFERS_SUPPLIED) {

                //
                // We had someone blocked waiting for us to complete.  Pull
                // them off the waiting list and get them running
                //

                DbgMsg1(("\'SonyDCamStopStream: Dequeueing request - Read Count = %x\n", DeviceExtension->PendingReadCount));
                SONYDCAM_ASSERT ( !IsListEmpty(&DeviceExtension->IsochWaitingList) );
                IsochDescriptorReserved = \
                    (PISOCH_DESCRIPTOR_RESERVED) ExInterlockedRemoveHeadList(
                       &DeviceExtension->IsochWaitingList,
                       &DeviceExtension->IsochWaitingLock
                        );

                IsochDescriptor = \
                    (PISOCH_DESCRIPTOR) (((PUCHAR) IsochDescriptorReserved) - 
                        FIELDOFFSET(ISOCH_DESCRIPTOR, DeviceReserved[0]));

                SonyDCamReadStreamWorker(IsochDescriptorReserved->Srb, IsochDescriptor);

            }

        }

    }

    IoFreeIrp(Irp);
    DeviceExtension->bCameraStopped = TRUE;

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
    IN PKSDATARANGE DataRange2)
{
    return (
        IsEqualGUID (
            &DataRange1->MajorFormat, 
            &DataRange2->MajorFormat) &&
        IsEqualGUID (
            &DataRange1->SubFormat, 
            &DataRange2->SubFormat) &&
        IsEqualGUID (
            &DataRange1->Specifier, 
            &DataRange2->Specifier) &&
        (DataRange1->FormatSize == DataRange2->FormatSize));
}

/*
** AdapterVerifyFormat()
**
**   Checks the validity of a format request by walking through the
**       array of supported KSDATA_RANGEs for a given stream.
**
** Arguments:
**
**   pKSDataFormat - pointer of a KS_DATAFORMAT_VIDEOINFOHEADER structure.
**   StreamNumber - index of the stream being queried / opened.
**
** Returns:
** 
**   TRUE if the format is supported
**   FALSE if the format cannot be suppored
**
** Side Effects:  none
*/

BOOL 
AdapterVerifyFormat(
        PKS_DATAFORMAT_VIDEOINFOHEADER pKSDataFormatToVerify, 
        int StreamNumber)
{
    PKS_VIDEOINFOHEADER                 pVideoInfoHdrToVerify = 
                &pKSDataFormatToVerify->VideoInfoHeader;
    PKS_VIDEOINFOHEADER                 pVideoInfoHdr;
    PKSDATAFORMAT                       *pAvailableFormats;
    PKS_DATARANGE_VIDEO                 pKSDataRange;
    KS_VIDEO_STREAM_CONFIG_CAPS         *pConfigCaps;
    int                                 NumberOfFormatArrayEntries;
    int                                 nSize;
    int                                 j;
    RECT                                rcImage;
     
    //
    // Make sure the stream index is valid
    //
    if (StreamNumber >= 1) {
        return FALSE;
    }

    //
    // How many formats does this stream support?
    //
    NumberOfFormatArrayEntries = NUM_DCAM_STREAM_FORMATS;

    nSize = KS_SIZE_VIDEOHEADER (pVideoInfoHdrToVerify);

    DbgMsg3(("\'AdapterCompareGUIDsAndFormatSize: AdapterVerifyFormat, Stream=%d\n",StreamNumber));
    DbgMsg3(("\'AdapterCompareGUIDsAndFormatSize: FormatSize=%d\n", pKSDataFormatToVerify->DataFormat.FormatSize));
    DbgMsg3(("\'AdapterCompareGUIDsAndFormatSize: MajorFormat=%x\n", pKSDataFormatToVerify->DataFormat.MajorFormat));    
    DbgMsg3(("\'AdapterCompareGUIDsAndFormatSize: pVideoInfoHdrToVerify=%x\n", pVideoInfoHdrToVerify));
    DbgMsg3(("\'AdapterCompareGUIDsAndFormatSize: KS_VIDEOINFOHEADER size =%d\n", nSize));
    DbgMsg2(("\'AdapterCompareGUIDsAndFormatSize: Width=%d Height=%d  biBitCount=%d\n", pVideoInfoHdrToVerify->bmiHeader.biWidth, 
                pVideoInfoHdrToVerify->bmiHeader.biHeight, pVideoInfoHdrToVerify->bmiHeader.biBitCount));
    DbgMsg3(("\'AdapterCompareGUIDsAndFormatSize: biSizeImage =%d\n", pVideoInfoHdrToVerify->bmiHeader.biSizeImage));

    //
    // Get the pointer to the array of available formats
    //
    pAvailableFormats = &SONYDCAM_StreamFormats[0];

    //
    // Walk the array, searching for a match
    //
    for (j = 0; j < NumberOfFormatArrayEntries; j++, pAvailableFormats++) {
        pKSDataRange = (PKS_DATARANGE_VIDEO) *pAvailableFormats;
        pVideoInfoHdr = &pKSDataRange->VideoInfoHeader;
        pConfigCaps = &pKSDataRange->ConfigCaps;
        
        //
        // Check for matching size, Major Type, Sub Type, and Specifier
        //

        if (!IsEqualGUID (&pKSDataRange->DataRange.MajorFormat, 
            &pKSDataFormatToVerify->DataFormat.MajorFormat)) {
               DbgMsg1(("\'%d) AdapterCompareGUIDsAndFormatSize: MajorFormat mismatch!\n", j));
               continue;
        }

        if (!IsEqualGUID (&pKSDataRange->DataRange.SubFormat, 
            &pKSDataFormatToVerify->DataFormat.SubFormat)) {
               DbgMsg1(("\'%d) AdapterCompareGUIDsAndFormatSize: SubFormat mismatch!\n", j));
               continue;
        }

        if (!IsEqualGUID (&pKSDataRange->DataRange.Specifier,
            &pKSDataFormatToVerify->DataFormat.Specifier)) {
               DbgMsg1(("\'%d) AdapterCompareGUIDsAndFormatSize: Specifier mismatch!\n", j));
               continue;
        }

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
        
        if ((pVideoInfoHdrToVerify->rcTarget.right - 
             pVideoInfoHdrToVerify->rcTarget.left <= 0) ||
            (pVideoInfoHdrToVerify->rcTarget.bottom - 
             pVideoInfoHdrToVerify->rcTarget.top <= 0)) {
            
             rcImage.left = rcImage.top = 0;
             rcImage.right = pVideoInfoHdrToVerify->bmiHeader.biWidth;
             rcImage.bottom = pVideoInfoHdrToVerify->bmiHeader.biHeight > 0 ? pVideoInfoHdrToVerify->bmiHeader.biHeight : -pVideoInfoHdrToVerify->bmiHeader.biHeight;
        }
        else {
             rcImage = pVideoInfoHdrToVerify->rcTarget;
        }

        //
        // Perform all other verification tests here!!!
        //




        //
        // HOORAY, the format passed all of the tests, so we support it
        //

        return TRUE;
    } 

    //
    // The format requested didn't match any of our listed ranges,
    // so refuse the connection.
    //

    return FALSE;
}

/*
** AdapterFormatFromRange()
**
**   Returns a DATAFORMAT from a DATARANGE
**
** Arguments:
**
**         IN PHW_STREAM_REQUEST_BLOCK Srb 
**
** Returns:
** 
**   TRUE if the format is supported
**   FALSE if the format cannot be suppored
**
** Side Effects:  none
*/

BOOL 
AdapterFormatFromRange(
        IN PHW_STREAM_REQUEST_BLOCK Srb)
{
    PSONYDCAM_EXTENSION DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    PSTREAM_DATA_INTERSECT_INFO IntersectInfo;
    PKSDATARANGE                DataRange;
    BOOL                        OnlyWantsSize;
    BOOL                        MatchFound = FALSE;
    ULONG                       FormatSize;
    ULONG                       StreamNumber;
    ULONG                       j;
    ULONG                       NumberOfFormatArrayEntries;
    PKSDATAFORMAT               *pAvailableFormats;
    PKS_BITMAPINFOHEADER        pbmiHeader;

    IntersectInfo = Srb->CommandData.IntersectInfo;
    StreamNumber = IntersectInfo->StreamNumber;
    DataRange = IntersectInfo->DataRange;

    //
    // Check that the stream number is valid
    //

    if (StreamNumber >= 1) {

        Srb->Status = STATUS_NOT_IMPLEMENTED;
        DbgMsg1(("\'AdapterFormatFromRange: StreamNumber(=%d) is not implemented.\n", StreamNumber));
        TRAP();
        return FALSE;
    }
    
    NumberOfFormatArrayEntries = NUM_DCAM_STREAM_FORMATS;
    Srb->Status = STATUS_SUCCESS;

    //
    // Get the pointer to the array of available formats
    //

    pAvailableFormats = &SONYDCAM_StreamFormats[0];

    //
    // Is the caller trying to get the format, or the size of the format?
    //


    OnlyWantsSize = (IntersectInfo->SizeOfDataFormatBuffer == sizeof(ULONG));

    //
    // Walk the formats supported by the stream searching for a match
    // of the three GUIDs which together define a DATARANGE
    //

    for (j = 0; j < NumberOfFormatArrayEntries; j++, pAvailableFormats++) {

        if (!AdapterCompareGUIDsAndFormatSize(DataRange, *pAvailableFormats)) {
            continue;
        }

        //
        // Now that the three GUIDs match, switch on the Specifier
        // to do a further type specific check
        //

        // -------------------------------------------------------------------
        // Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
        // -------------------------------------------------------------------

        if (IsEqualGUID (&DataRange->Specifier, 
                &KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {
                
            PKS_DATARANGE_VIDEO DataRangeVideoToVerify = 
                    (PKS_DATARANGE_VIDEO) DataRange;
            PKS_DATARANGE_VIDEO DataRangeVideo = 
                    (PKS_DATARANGE_VIDEO) *pAvailableFormats;

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
            
            //
            // biSizeImage should match from biWidth * abs(biHeight) * biBitCount / 8
            // 
               pbmiHeader = &DataRangeVideoToVerify->VideoInfoHeader.bmiHeader;
               if(pbmiHeader->biSizeImage != (DWORD) pbmiHeader->biWidth * (pbmiHeader->biHeight > 0 ? pbmiHeader->biHeight : -pbmiHeader->biHeight) * pbmiHeader->biBitCount/8) {
                  ASSERT(pbmiHeader->biSizeImage == (DWORD) pbmiHeader->biWidth * (pbmiHeader->biHeight > 0 ? pbmiHeader->biHeight : -pbmiHeader->biHeight) * pbmiHeader->biBitCount/8);
               Srb->Status = STATUS_NO_MATCH;
               return FALSE;
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
                Srb->Status = STATUS_BUFFER_TOO_SMALL;
                DbgMsg2(("IntersectInfo->SizeOfDataFormatBuffer=%d, FormatSize=%d\n", IntersectInfo->SizeOfDataFormatBuffer, FormatSize));
                return FALSE;
            }

            // Copy over the KSDATAFORMAT, followed by the 
            // actual VideoInfoHeader
                
            RtlCopyMemory(
                &((PKS_DATAFORMAT_VIDEOINFOHEADER)IntersectInfo->DataFormatBuffer)->DataFormat,
                &DataRangeVideoToVerify->DataRange,
                sizeof (KSDATARANGE));

            ((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;

            RtlCopyMemory(
                &((PKS_DATAFORMAT_VIDEOINFOHEADER) IntersectInfo->DataFormatBuffer)->VideoInfoHeader,
                &DataRangeVideoToVerify->VideoInfoHeader,
                KS_SIZE_VIDEOHEADER (&DataRangeVideoToVerify->VideoInfoHeader));

            DbgMsg2(("\'AdapterFormatFromRange: Found intersection and biSizeImage=%d\n", pbmiHeader->biSizeImage ));

            break;

        } // End of VIDEOINFOHEADER specifier

        else {
            Srb->Status = STATUS_NO_MATCH;
            return FALSE;
        }

    } // End of loop on all formats for this stream

    
    if (OnlyWantsSize) {
        *(PULONG) IntersectInfo->DataFormatBuffer = FormatSize;
        Srb->ActualBytesTransferred = sizeof(ULONG);
    } else {
        Srb->ActualBytesTransferred = FormatSize;
    }

    DbgMsg2(("Suceeded (status=%x) with ForamtSize=%d; ActualByteTransferred=%d\n", 
        Srb->Status, FormatSize, Srb->ActualBytesTransferred));

    return TRUE;
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
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = Srb->CommandData.PropertyInfo;


    // preset to success

    Srb->Status = STATUS_SUCCESS;

    if (IsEqualGUID (&KSPROPSETID_Connection, &pSPD->Property->Set)) {
        VideoStreamGetConnectionProperty (Srb);
    } else if (IsEqualGUID (&PROPSETID_VIDCAP_DROPPEDFRAMES, &pSPD->Property->Set)) {
        VideoStreamGetDroppedFramesProperty (Srb);
    } else {
        Srb->Status = STATUS_NOT_IMPLEMENTED;
    }

}


/*
** VideoGetState()
**
**    Gets the current state of the requested stream
**
** Arguments:
**
**    Srb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID VideoGetState(PHW_STREAM_REQUEST_BLOCK Srb)
{
    PSONYDCAM_EXTENSION DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    PSTREAMEX pStrmEx = DeviceExtension->pStrmEx;

    DbgMsg2(("\'%d:%s) VideoGetState: KSSTATE=%s.\n", 
          DeviceExtension->idxDev, DeviceExtension->pchVendorName, 
          pStrmEx->KSState == KSSTATE_STOP ? "STOP" : 
          pStrmEx->KSState == KSSTATE_PAUSE ? "PAUSE" :     
          pStrmEx->KSState == KSSTATE_RUN ? "RUN" : "ACQUIRE"));

    Srb->CommandData.StreamState = pStrmEx->KSState;
    Srb->ActualBytesTransferred = sizeof (KSSTATE);

    // A very odd rule:
    // When transitioning from stop to pause, DShow tries to preroll
    // the graph.  Capture sources can't preroll, and indicate this
    // by returning VFW_S_CANT_CUE in user mode.  To indicate this
    // condition from drivers, they must return ERROR_NO_DATA_DETECTED

    Srb->Status = STATUS_SUCCESS;


    if (pStrmEx->KSState == KSSTATE_PAUSE) {
       Srb->Status = STATUS_NO_DATA_DETECTED;
    }
}


VOID  VideoStreamGetConnectionProperty (PHW_STREAM_REQUEST_BLOCK Srb)
{
    PSONYDCAM_EXTENSION DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = Srb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PSTREAMEX pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;
    ASSERT(pStrmEx == (PSTREAMEX)Srb->StreamObject->HwStreamExtension);

    switch (Id) {

    case KSPROPERTY_CONNECTION_ALLOCATORFRAMING:

        if (pStrmEx->pVideoInfoHeader) {

            PKSALLOCATOR_FRAMING Framing = 
                (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;
            Framing->RequirementsFlags =
                KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY |
                KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
                KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY;
            Framing->PoolType = PagedPool;
            Framing->Frames = MAX_BUFFERS_SUPPLIED; //1; 
            Framing->FrameSize = pStrmEx->pVideoInfoHeader->bmiHeader.biSizeImage;
            Framing->FileAlignment = 0; // FILE_LONG_ALIGNMENT;
            Framing->Reserved = 0;
            Srb->ActualBytesTransferred = sizeof (KSALLOCATOR_FRAMING);
            Srb->Status = STATUS_SUCCESS;
               DbgMsg2(("\'VideoStreamGetConnectionProperty: status=0x%x, Frame=%d, FrameSize=%d\n",
                    Srb->Status, Framing->Frames, Framing->FrameSize));

        } else {
               DbgMsg2(("\'VideoStreamGetConnectionProperty:\n"));
            Srb->Status = STATUS_INVALID_PARAMETER;
               DbgMsg2(("\'VideoStreamGetConnectionProperty: status=0x\n",Srb->Status));
        }
        break;

    default:
        TRAP();
        break;
    }
}

/*
** VideoStreamGetConnectionProperty()
**
**    Gets the current state of the requested stream
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
    PHW_STREAM_REQUEST_BLOCK Srb
    )
{
    PSTREAMEX pStrmEx = (PSTREAMEX)Srb->StreamObject->HwStreamExtension;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = Srb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    ULONGLONG tmStream;

    switch (Id) {

    case KSPROPERTY_DROPPEDFRAMES_CURRENT:
         {

         PKSPROPERTY_DROPPEDFRAMES_CURRENT_S pDroppedFrames = 
                     (PKSPROPERTY_DROPPEDFRAMES_CURRENT_S) pSPD->PropertyInfo;

         if (pStrmEx->hMasterClock) {
                    
             tmGetStreamTime(Srb, pStrmEx, &tmStream);

             if (tmStream < pStrmEx->FirstFrameTime) {
                 DbgMsg2(("\'*DroppedFP: Tm(%dms) < 1stFrameTm(%d)\n",
                           (LONG) tmStream/10000, (LONG)pStrmEx->FirstFrameTime));
                 pDroppedFrames->DropCount = 0;
             } else {
                 pDroppedFrames->DropCount = (tmStream - pStrmEx->FirstFrameTime)
                                / pStrmEx->pVideoInfoHeader->AvgTimePerFrame + 1 - pStrmEx->FrameCaptured;
             }

             if (pDroppedFrames->DropCount < 0)
                 pDroppedFrames->DropCount = 0;
                    
         } else {
             pDroppedFrames->DropCount = 0;
         }

         // Update our drop frame here. "pDroppedFrames->DropCount" is return when a frame is returned. 
         if (pDroppedFrames->DropCount > pStrmEx->FrameInfo.DropCount) {
             pStrmEx->FrameInfo.DropCount = pDroppedFrames->DropCount;
             //pStrmEx->bDiscontinue = TRUE;
         } else {
             pDroppedFrames->DropCount = pStrmEx->FrameInfo.DropCount;
         }

         pDroppedFrames->AverageFrameSize = pStrmEx->pVideoInfoHeader->bmiHeader.biSizeImage;
         pDroppedFrames->PictureNumber = pStrmEx->FrameCaptured + pDroppedFrames->DropCount;

         // Correction if no picture has been successfully capture in the IsochCallback.
         if (pDroppedFrames->PictureNumber < pDroppedFrames->DropCount)
             pDroppedFrames->PictureNumber = pDroppedFrames->DropCount;

         DbgMsg2(("\'*DroppedFP: tm(%d); Pic#(%d)=?Cap(%d)+Drp(%d)\n",
                  (ULONG) tmStream/10000,
                  (LONG) pDroppedFrames->PictureNumber,
                  (LONG) pStrmEx->FrameCaptured,
                  (LONG) pDroppedFrames->DropCount));
               
         Srb->ActualBytesTransferred = sizeof (KSPROPERTY_DROPPEDFRAMES_CURRENT_S);
               Srb->Status = STATUS_SUCCESS;

         }
         break;

    default:
         TRAP();
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
    PSONYDCAM_EXTENSION DeviceExtension = (PSONYDCAM_EXTENSION) Srb->HwDeviceExtension;
    PSTREAMEX pStrmEx = (PSTREAMEX) DeviceExtension->pStrmEx;
    ASSERT(pStrmEx == (PSTREAMEX)Srb->StreamObject->HwStreamExtension);

    pStrmEx->hMasterClock = Srb->CommandData.MasterClockHandle;

    DbgMsg2(("\'%d:%s)VideoIndicateMasterClock: hMasterClock = 0x%x\n", DeviceExtension->idxDev, DeviceExtension->pchVendorName, pStrmEx->hMasterClock));

}

