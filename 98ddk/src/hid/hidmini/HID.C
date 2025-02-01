/*++

Copyright (c) 1996,1997  Microsoft Corporation

Module Name:

    hid.c

Abstract: Human Input Device (HID) minidriver that creates an example
        device.

--*/
#include <WDM.H>
#include <USBDI.H>

#include <HIDPORT.H>
#include <HIDMINI.H>

//
//  Our descriptors.  Normally you'd have to get these from the hardware, if it
//  was truly a HID device, or you'll have to build your own for non HID devices.
//

//
//  The report descriptor completely lays out what read and write packets will look like
//  and indicates what the semantics are for each field.
//

HID_REPORT_DESCRIPTOR           MyReportDescriptor[] = {
    0x05,   0x01,       // Usage Page (Generic Desktop),
    0x09,   0x06,       // Usage (Keyboard),
    0xA1,   0x01,       // Collection (Application),
    0x05,   0x07,       //  Usage Page (Key Codes);
    0x19,   0xE0,       //  Usage Minimum (224),
    0x29,   0xE7,       //  Usage Maximum (231),
    0x15,   0x00,       //  Logical Minimum (0),
    0x25,   0x01,       //  Logical Maximum (1),
    0x75,   0x01,       //  Report Size (1),
    0x95,   0x08,       //  Report Count (8),
    0x81,   0x02,       //  Input (Data, Variable, Absolute),;Modifier byte
    0x95,   0x01,       //  Report Count (1),
    0x75,   0x08,       //  Report Size (8),
    0x81,   0x01,       //  Input (Constant),                       ;Reserved byte
    0x95,   0x05,       //  Report Count (5),
    0x75,   0x01,       //  Report Size (1),
    0x05,   0x08,       //  Usage Page (Page# for LEDs),
    0x19,   0x01,       //  Usage Minimum (1),
    0x29,   0x05,       //  Usage Maximum (5),
    0x91,   0x02,       //  Output (Data, Variable, Absolute),      ;LED report
    0x95,   0x01,       //  Report Count (1),
    0x75,   0x03,       //  Report Size (3),
    0x91,   0x01,       //  Output (Constant),                      ;LED report padding
    0x95,   0x06,       //  Report Count (6),
    0x75,   0x08,       //  Report Size (8),
    0x15,   0x00,       //  Logical Minimum (0),
    0x25,   0x65,       //  Logical Maximum(101),
    0x05,   0x07,       //  Usage Page (Key Codes),
    0x19,   0x00,       //  Usage Minimum (0),
    0x29,   0x65,       //  Usage Maximum (101),
    0x81,   0x00,       //  Input (Data, Array),                    ;Key arrays (6 bytes)
    0xC0                // End Collection
};


//
//  The HID descriptor has some basic device info and tells how long the report
//  descriptor is.
//

USB_HID_DESCRIPTOR              MyHidDescriptor = {
    0x09,   // length of HID descriptor
    0x21,   // descriptor type == HID
    0x0100, // hid spec release
    0x00,   // country code == Not Specified
    0x01,   // number of HID class descriptors
    0x22,   // report descriptor type
    sizeof(MyReportDescriptor)  // total length of report descriptor
};

//
//  This buffer has all of the strings that we define.
//

UCHAR AStringDescriptor[] = {
    4,          // length of this string
    3,          // type == STRING
    0x09, 0x00, // language code == ENGLISH

    44,     3,
    'M',0,  'i',0,  'c',0,  'r',0,  'o',0,  's',0,  'o',0,  'f',0,  
    't',0,  ' ',0,  'C',0,  'o',0,  'r',0,  'p',0,  'o',0,  'r',0,  
    'a',0,  't',0,  'i',0,  'o',0,  'n',0,  

    46,     3,
    'S',0,  'y',0,  's',0,  't',0,  'e',0,  'm',0,  ' ',0,  'C',0,  
    'o',0,  'n',0,  't',0,  'r',0,  'o',0,  'l',0,  ' ',0,  'B',0,  
    'u',0,  't',0,  't',0,  'o',0,  'n',0,  's',0,  

    32,     3,
    'L',0,  'e',0,  'g',0,  'a',0,  'c',0,  'y',0,  ' ',0,  'K',0,  
    'e',0,  'y',0,  'b',0,  'o',0,  'a',0,  'r',0,  'd',0,  
};

//
//  String descriptors are 

PUSB_STRING_DESCRIPTOR          MyStringDescriptor = (PUSB_STRING_DESCRIPTOR) AStringDescriptor;

//
//  No designator descriptors.
//

PUSB_PHYSICAL_DESCRIPTOR        MyPhysicalDescriptor = NULL;

//
//  IO lists
//

KSPIN_LOCK  HidMini_IrpReadLock;
KSPIN_LOCK  HidMini_IrpWriteLock;
LIST_ENTRY  HidMini_ReadIrpHead;
LIST_ENTRY  HidMini_WriteIrpHead;

BOOLEAN IsRunning = FALSE;

LONG ReadsCompleting = 0;

NTSTATUS HidMiniGetHIDDescriptor(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
/*++

Routine Description:

    Finds the HID descriptor and copies it into the buffer provided by the Irp.

Arguments:

    DeviceObject - pointer to a device object.

    Irp - Pointer to Interrupt Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   DeviceExtension;
    PIO_STACK_LOCATION  IrpStack;
    ULONG  bytesToCopy;

    DBGPrint(("HIDMINI.SYS: HidMiniGetHIDDescriptor Entry\n"));

    //
    // Get a pointer to the current location in the Irp
    //

    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get a pointer to the device extension
    //

    DeviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Copy device descriptor to HIDCLASS buffer
    //

    DBGPrint(("HIDMINI.SYS: HIDCLASS Buffer = 0x%x, Buffer length = 0x%x\n", Irp->UserBuffer, IrpStack->Parameters.DeviceIoControl.OutputBufferLength));

    //
    // Copy MIN (OutputBufferLength, DeviceExtension->HidDescriptor->bLength)
    //

    bytesToCopy = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

    if (bytesToCopy > DeviceExtension->HidDescriptor.bLength)
    {
        bytesToCopy = DeviceExtension->HidDescriptor.bLength;
    }

    DBGPrint(("HIDMINI.SYS: Copying %d bytes to HIDCLASS buffer\n", bytesToCopy));

    RtlCopyMemory((PUCHAR) Irp->UserBuffer, (PUCHAR) &DeviceExtension->HidDescriptor, bytesToCopy);

    //
    // Report how many bytes were copied
    //
    Irp->IoStatus.Information = bytesToCopy;

    DBGPrint(("HIDMINI.SYS: HidMiniGetHIDDescriptor Exit = 0x%x\n", ntStatus));

    return ntStatus;
}


NTSTATUS HidMiniGetReportDescriptor(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
/*++

Routine Description:

    Finds the Report descriptor and copies it into the buffer provided by the Irp.

Arguments:

    DeviceObject - pointer to a device object.

    Irp - Pointer to Interrupt Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   DeviceExtension;
    PIO_STACK_LOCATION  IrpStack;
    ULONG  bytesToCopy;

    DBGPrint(("HIDMINI.SYS: HidMiniGetReportDescriptor Entry\n"));

    //
    // Get a pointer to the current location in the Irp
    //

    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get a pointer to the device extension
    //

    DeviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Copy device descriptor to HIDCLASS buffer
    //

    DBGPrint(("HIDMINI.SYS: HIDCLASS Buffer = 0x%x, Buffer length = 0x%x\n", Irp->UserBuffer, IrpStack->Parameters.DeviceIoControl.OutputBufferLength));

    //
    // Copy MIN (OutputBufferLength, DeviceExtension->HidDescriptor->bLength)
    //

    bytesToCopy = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

    if (bytesToCopy > DeviceExtension->HidDescriptor.wReportLength)
    {
        bytesToCopy = DeviceExtension->HidDescriptor.wReportLength;
    }

    DBGPrint(("HIDMINI.SYS: Copying %d bytes to HIDCLASS buffer\n", bytesToCopy));

    RtlCopyMemory((PUCHAR) Irp->UserBuffer, (PUCHAR) DeviceExtension->ReportDescriptor, bytesToCopy);

    //
    // Report how many bytes were copied
    //
    Irp->IoStatus.Information = bytesToCopy;

    DBGPrint(("HIDMINI.SYS: HidMiniGetReportDescriptor Exit = 0x%x\n", ntStatus));

    return ntStatus;
}


NTSTATUS HidMiniGetDeviceAttributes(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
/*++

Routine Description:

    Fill in the given struct _HID_DEVICE_ATTRIBUTES

Arguments:

    DeviceObject - pointer to a device object.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension;
    PIO_STACK_LOCATION  irpStack;
    PHID_DEVICE_ATTRIBUTES deviceAttributes;

    DBGPrint(("HIDMINI.SYS: HidMiniGetDeviceAttributes Entry\n"));

    //
    // Get a pointer to the current location in the Irp
    //

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get a pointer to the device extension
    //

    deviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);
    deviceAttributes = (PHID_DEVICE_ATTRIBUTES) Irp->UserBuffer;

    ASSERT (sizeof (HID_DEVICE_ATTRIBUTES) ==
            irpStack->Parameters.DeviceIoControl.OutputBufferLength);


    //
    // Report how many bytes were copied
    //
    Irp->IoStatus.Information = sizeof (HID_DEVICE_ATTRIBUTES);

    deviceAttributes->Size = sizeof (HID_DEVICE_ATTRIBUTES);
    deviceAttributes->VendorID = HIDMINI_VID;
    deviceAttributes->ProductID = HIDMINI_PID;
    deviceAttributes->VersionNumber = HIDMINI_VERSION;

    DBGPrint(("HIDMINI.SYS: HidMiniGetAttributes Exit = 0x%x\n", ntStatus));

    return ntStatus;
}

VOID HidMiniIncrementPendingRequestCount(IN PDEVICE_EXTENSION DeviceExtension)
/*++

Routine Description:

    Increments the number of outstanding requests on the DeviceObject with this extension.

Arguments:

    DeviceExtension - the mini driver extension area of the device that is being made busy.

Return Value:

    VOID.

--*/
{
    DeviceExtension->NumPendingRequests++;
    DBGPrint(("HIDMINI.SYS: Bumping requests on device extension 0x%08x up to %d\n",
              DeviceExtension,
              DeviceExtension->NumPendingRequests));
}



VOID HidMiniDecrementPendingRequestCount(IN PDEVICE_EXTENSION DeviceExtension)
/*++

Routine Description:

    Decrements the number of outstanding requests on the DeviceObject with this extension.
    If we get to zero outstanding IOs, set the device's all requests complete event.

Arguments:

    DeviceExtension - the mini driver extension area of the device that is being made busy.

Return Value:

    VOID.

--*/
{
    ASSERT( DeviceExtension->NumPendingRequests > 0 );
    DeviceExtension->NumPendingRequests--;

    DBGPrint(("HIDMINI.SYS: Bumping requests on device extension 0x%08x down to %d\n",
              DeviceExtension,
              DeviceExtension->NumPendingRequests));

    if ( DeviceExtension->NumPendingRequests == 0 &&
         DeviceExtension->DeviceState != DEVICE_STATE_RUNNING )
    {

        //
        // The device state is stopping, and the last outstanding request
        // has just completed.
        //

        DBGPrint(("HIDMINI.SYS: last request completed, signalling event\n"));
        KeSetEvent( &DeviceExtension->AllRequestsCompleteEvent,
                    0, FALSE );
    }
}




NTSTATUS HidMiniReadReport(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
/*++

Routine Description:

    Process a read HID packet request.

Arguments:

    DeviceObject - pointer to a device object.

    Irp - Pointer to Interrupt Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION DeviceExtension;
    PIO_STACK_LOCATION IrpStack;
    PVOID ReportBuffer;
    ULONG ReportTotalSize;
    PNODE Node;

    DBGPrint(("HIDMINI.SYS: HidMiniReadReport Enter\n"));

    DBGPrint(("HIDMINI.SYS: DeviceObject = 0x%x\n", DeviceObject));

    //
    // Get a pointer to the device extension.
    //

    DeviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    DBGPrint(("HIDMINI.SYS: DeviceExtension = 0x%x\n", DeviceExtension));

    //
    // Get Stack location
    //

    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    //  Get the buffer and its size, make sure they're valid.
    //

    ReportBuffer = Irp->UserBuffer;
    ReportTotalSize = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

    DBGPrint(("HIDMINI.SYS: ReportBuffer = 0x%x, ReportTotalSize = 0x%x\n", ReportBuffer, ReportTotalSize));

    if (IsRunning)
    {
        if (ReportTotalSize && ReportBuffer)
        {
            Node = (PNODE)ExAllocatePool(NonPagedPool, sizeof(NODE));
            if (Node)
            {

                //
                //  Increase the count of outstanding IOs, mark the Irp pending.
                //
                HidMiniIncrementPendingRequestCount(DeviceExtension);

                //
                //  Hook the Irp onto the pending IO list
                //
                Node->Irp = Irp;
                ExInterlockedInsertTailList(&HidMini_ReadIrpHead, &Node->List, &HidMini_IrpReadLock);
                ntStatus = STATUS_PENDING;

            }
            else
            {
                ntStatus = STATUS_NO_MEMORY;
            }
        }
        else
        {

            //
            // No buffer, or buffer of zero size
            //
            ntStatus = STATUS_INVALID_PARAMETER;
        }
    }
    else
    {
        //
        //  We're shutting down
        //
        ntStatus = STATUS_NO_SUCH_DEVICE;
    }

    DBGPrint(("HIDMINI.SYS: HidMiniReadReport Exit = 0x%x\n", ntStatus));

    return ntStatus;
}

/***********************************************************************
 *
 * CharToDescriptor
 *
 * Purpose:
 *	 convert common ascii characters to hid descriptor string
 *	
 * Parameters:
 *   char c;	// character to convert
 *	 buf[8+];	// buffer to store hid descriptor, must be 8 bytes long
 *				
 *	
 * Returns:
 *	 void
 *
 ***********************************************************************/ 
void CharToDescriptor( char c, char buf[] )
{   
    RtlZeroMemory (buf, 8);

    if ( 32 == c )  // space character
    {
        buf[2] = 0x2c;
    }
    else if ( (c >= 'A') && (c <= 'Z') )
    {
        buf[0] = '\x02';        // indicate shift depressed
        buf[2] = c - 'A' + 4;
    }
    else if ( (c >= 'a') && (c <= 'z') )
    {
        buf[2] = c - 'a' + 4;
    }
    else if ( (c >= '1') && (c <= '9') )
    {
        buf[2] = c - '1' + 0x1E; 
    }
    else if ( '0' == c )
    {
        buf[2] = 0x27;
    }
}

NTSTATUS HidMiniReadCompletion(PVOID Context)
/*++

Routine Description:

    HID read packet completion routine.

Arguments:

Return Value:

   STATUS_SUCCESS, STATUS_UNSUCCESSFUL.

--*/
{
    PIO_STACK_LOCATION IrpStack;
    ULONG bytesRead;
    PDEVICE_EXTENSION deviceExtension;
    PNODE IrpNode;
    PIRP Irp;
    PDEVICE_OBJECT DeviceObject;
    int i;

    static int Rest = 0;

    UCHAR ReadBuffer[8];


    DBGPrint(("HIDMINI.SYS: HidMiniReadCompletion Enter\n"));

    //
    //  Free workitem that started us, check to see if we're already completing reads
    //

    ExFreePool((PWORK_QUEUE_ITEM)Context);

    ASSERT(ReadsCompleting > 0);
    if (InterlockedDecrement(&ReadsCompleting))
    {
		// Another readcompletion routinge is already running so exit.
        return STATUS_SUCCESS;
    }

    //
    //  Loop around completing Irps.  When we run out, break 
    //  out of the loop.
    //
    while ( IsRunning )
    {
        PHIDMINI_RAW_OUTPUT rawData;
        //
        //  Make sure we don't overrun our list of replies.  If we get to the end of it, 
        //  we'll exit this loop early and let the IRPs queue up again.
        //

        //
        //  Any Irps to complete?
        //
        IrpNode = (PNODE)ExInterlockedRemoveHeadList(&HidMini_ReadIrpHead, &HidMini_IrpReadLock);

        if (IrpNode)
        {
            char c = '\0';

            //
            //  Find all the pieces
            //
            Irp = IrpNode->Irp;

            IrpStack = IoGetCurrentIrpStackLocation(Irp);

            DeviceObject = IrpStack->DeviceObject;

            deviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION( DeviceObject );

            HidMiniDecrementPendingRequestCount( deviceExtension );

            if ( !Rest )
            {
                rawData = (PHIDMINI_RAW_OUTPUT) ExInterlockedRemoveHeadList(
                                                                           (PLIST_ENTRY)&deviceExtension->OutputRawQueue.List,
                                                                           &deviceExtension->OutputRawQueue.SpinLock );
                if ( rawData )
                {
                    deviceExtension->OutputRawQueue.nElements--;
                    c = (char)rawData->PacketNum;
                    ExFreeToNPagedLookasideList( &deviceExtension->OutputRawQueue.MemListPool,
                                                 rawData );         
                }
                CharToDescriptor( c, ReadBuffer );
            }
            //
            // Get the bytes read and store in the status block
            //
            bytesRead = 8;

            if (bytesRead > IrpStack->Parameters.DeviceIoControl.OutputBufferLength)
            {
                bytesRead = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
            }

            if (Rest)
            {
                RtlCopyMemory((PUCHAR) Irp->UserBuffer, "\x00\x00\0\0\0\0\0\0", bytesRead);
            }
            else
            {
                RtlCopyMemory((PUCHAR) Irp->UserBuffer, (PUCHAR)ReadBuffer, bytesRead);
            }
            Rest ^= 1;

            Irp->IoStatus.Information = bytesRead;

            Irp->IoStatus.Status = STATUS_SUCCESS;

            IoCompleteRequest(Irp, IO_NO_INCREMENT);

            DBGPrint(("HIDMINI.SYS: Read report DeviceObject (%x) completed, %d bytes!\n",
                      DeviceObject, bytesRead ));

            //
            //  Free up the Node
            //
            ExFreePool(IrpNode);

            if (IsListEmpty(&deviceExtension->OutputRawQueue.List) && !Rest)
            {
                break;
            }

        }
        else
        {

            //
            //  No Irps
            //
            break;
        }
    }

    return STATUS_SUCCESS;
}

/***********************************************************************
 *
 * QueueRawData
 *
 * Purpose:
 *	 Store characters which will be fed into keyboard buffer by
 *	 HidMiniReadCompletion
 *
 * Parameters:
 *	 char 				*str 			// null terminated string of characters
 *	 PDEVICE_EXTENSION 	deviceExtension	// Hidmini device Extension
 *	
 * Returns:
 *	 STATUS_SUCCESS or STATUS_NO_MEMORY
 *
 ***********************************************************************/ 

NTSTATUS QueueRawData( char *str, PDEVICE_EXTENSION deviceExtension )
{ 
    PHIDMINI_RAW_OUTPUT rawData;

    while ( *str )
    {
        rawData = (PHIDMINI_RAW_OUTPUT)ExAllocateFromNPagedLookasideList(
                                       &deviceExtension->OutputRawQueue.MemListPool );
        if ( NULL == rawData )
        {
            return STATUS_NO_MEMORY;
        }
        rawData->PacketNum = (ULONG)*(str++);

        ExInterlockedInsertTailList((PLIST_ENTRY)&deviceExtension->OutputRawQueue.List,
                                    (PLIST_ENTRY)rawData,
                                    &deviceExtension->OutputRawQueue.SpinLock);
        deviceExtension->OutputRawQueue.nElements++;
    }
    return STATUS_SUCCESS;
} 

/***********************************************************************
 *
 * HidMiniWriteDPC
 *
 * Purpose:
 *	Simulate ISR which would get fired when device inidicates it's
 *	ready to receive more data.  Simply calls HidMiniWriteCompletion
 *	
 * Parameters:
 *   DeferredContext is a pointer to the HidMiniDeviceExtension
 *	
 * Returns: VOID
 *
 ***********************************************************************/
VOID HidMiniWriteDPC(
                    IN PKDPC Dpc,
                    IN PVOID DeferredContext,
                    IN PVOID SystemArgument1,
                    IN PVOID SystemArgument2
                    )
{
    PDEVICE_EXTENSION DeviceExtension = (PDEVICE_EXTENSION)DeferredContext;

    DBGPrint(("HIDMINI.SYS: HidMiniWriteDPC Enter\n"));

    HidMiniWriteCompletion( DeviceExtension );

    DBGPrint(("HIDMINI.SYS: HidMiniWriteDPC Exit\n"));
}


NTSTATUS HidMiniWriteReport(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
/*++

Routine Description:

    Process a write HID packet request.  This is also how we start up the read 
    completion process.

Arguments:

    DeviceObject - pointer to a device object.

    Irp - Pointer to Interrupt Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION   DeviceExtension;
    PHIDMINI_WRITE_IRP  WriteIrp;
    PIO_STACK_LOCATION  IrpStack;
    PVOID               UserBufferTmp;
    PHID_XFER_PACKET    hidWritePacket;

    DBGPrint(("HIDMINI.SYS: HidMiniWriteReport Enter\n"));

    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    hidWritePacket = (PHID_XFER_PACKET)Irp->UserBuffer;     

#if DBG
    {
        //
        // dump report being written to theoretical hidmini keyboard
        // 
        unsigned int i;

        if (hidWritePacket->reportBuffer && hidWritePacket->reportBufferLen)
        {
            DBGPrint(("****HIDMINI.SYS: Write report ID 0x%x, 0x%lx bytes :", 
                      (unsigned)hidWritePacket->reportId,
                      hidWritePacket->reportBufferLen ));

            for ( i=0; i<hidWritePacket->reportBufferLen; i++)
            {
                DBGPrint(( "%2x ", (unsigned)hidWritePacket->reportBuffer[i] ));
            }
            DBGPrint(( "\n" ));
        }
        else
        {
            DBGPrint(("HIDMINI.SYS: Write report ID 0x%x w/o buffer\n",
                      (unsigned)hidWritePacket->reportId ));
        }
    }
#endif 

    DeviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    if ( IsRunning )
    {
        WriteIrp = (PHIDMINI_WRITE_IRP)ExAllocatePool(NonPagedPool, sizeof(HIDMINI_WRITE_IRP));

        if (WriteIrp)
        {
            if ( hidWritePacket && hidWritePacket->reportBufferLen )
            {
                //
                // Convert the data in hidWritePacket into data that will be used
                // by HidMiniWriteCompletion.  NOTE: data in Irp->UserBuffer is not
                // valid upon exiting this function (e.g. it cannot be use in 
                // HidMiniWriteCompletion). Thus it must be convererted here.
                //
                WriteIrp->LedDelta = DeviceExtension->LedState ^ 
                                     (ULONG)hidWritePacket->reportBuffer[0];
                DeviceExtension->LedState = WriteIrp->LedState =
                                            (ULONG)hidWritePacket->reportBuffer[0];
            }
            else
                WriteIrp->LedDelta = 0;

            WriteIrp->Irp = Irp;

            HidMiniIncrementPendingRequestCount(DeviceExtension);
            IoMarkIrpPending(Irp);

            ExInterlockedInsertTailList(&HidMini_WriteIrpHead, &WriteIrp->List, &HidMini_IrpWriteLock);

            ntStatus = STATUS_PENDING;
            //
            //  Simulate wait for hardware to complete write by launching 
            //  write completion routinte after delay.  If timer is already going
            //  (i.e. not signaled), then timer will be reset by write completion 
            //	routinte
            //

            if ( KeReadStateTimer( &DeviceExtension->HidMiniWriteTimer) )
            {
                KeSetTimer( &DeviceExtension->HidMiniWriteTimer,
                            RtlConvertLongToLargeInteger( -1 * 10 * 1000 * 500 ),
                            &DeviceExtension->HidMiniWriteDPCObject );              
            }
        }
        else
        {
            ntStatus = STATUS_NO_MEMORY;
        }
    }
    else
    {
        //
        //  We're shutting down
        //
        ntStatus = STATUS_NO_SUCH_DEVICE;
    }
    DBGPrint(("HIDMINI.SYS: HidMiniWriteReportExit = 0x%x\n", ntStatus));

    return ntStatus;
}


NTSTATUS HidMiniWriteCompletion( PDEVICE_EXTENSION DeviceExtension )
/*++

Routine Description:

    Complete processing a write HID packet request.

Arguments:

Return Value:

    NT status code.

--*/
{
    PHIDMINI_WRITE_IRP  WriteIrp;
    PIRP Irp;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PWORK_QUEUE_ITEM StartCompletingReads;
    ULONG   LedDelta;
    ULONG   LedState;

    //
    //  Any Irps to complete?
    //
    WriteIrp = (PHIDMINI_WRITE_IRP)ExInterlockedRemoveHeadList(&HidMini_WriteIrpHead, 
                                                               &HidMini_IrpWriteLock);
    if (WriteIrp)
    {
        //
        //  Find all the pieces
        //
        Irp = WriteIrp->Irp;
        LedDelta = WriteIrp->LedDelta;
        LedState = WriteIrp->LedState;

        if (LedDelta & NUM_LOCK_MASK)
        {
            if ( LedState & NUM_LOCK_MASK )
            {
                ntStatus = QueueRawData( "NUM LOCK ON ", DeviceExtension );
            }
            else
            {
                ntStatus = QueueRawData( "NUM LOCK OFF ", DeviceExtension );
            }
        }
        if (NT_SUCCESS(ntStatus) && LedDelta & CAP_LOCK_MASK)
        {
            if ( LedState & CAP_LOCK_MASK )
            {
                ntStatus = QueueRawData( "CAP LOCK ON ", DeviceExtension );
            }
            else
            {
                ntStatus = QueueRawData( "CAP LOCK OFF ", DeviceExtension );
            }
        }
        if (NT_SUCCESS(ntStatus) && LedDelta & SCROLL_LOCK_MASK)
        {
            if ( LedState & SCROLL_LOCK_MASK )
            {
                ntStatus = QueueRawData( "SCROLL LOCK ON ", DeviceExtension );
            }
            else
            {
                ntStatus = QueueRawData( "SCROLL LOCK OFF ", DeviceExtension );
            }
        }

        StartCompletingReads = (PWORK_QUEUE_ITEM)
                               ExAllocatePool(NonPagedPool, sizeof(WORK_QUEUE_ITEM));
        if ( StartCompletingReads )
        {
            //
            //  Queue up a work item to start completing reads.
            //

            ASSERT(ReadsCompleting >= 0);
            InterlockedIncrement(&ReadsCompleting);

            ExInitializeWorkItem(StartCompletingReads, 
                                 HidMiniReadCompletion, 
                                 (PVOID)StartCompletingReads);
            ExQueueWorkItem(StartCompletingReads, DelayedWorkQueue);
        }

        //
        //  Finish off the IRP
        //
        HidMiniDecrementPendingRequestCount( DeviceExtension );
        Irp->IoStatus.Status = STATUS_SUCCESS;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        //
        //  Free up the Node
        //
        ExFreePool(WriteIrp);
    }

    //
    // Reset timer if more write packets left to be processed.
    //
    if ( !IsListEmpty( &HidMini_WriteIrpHead ) )
    {
        KeSetTimer( &DeviceExtension->HidMiniWriteTimer,
                    RtlConvertLongToLargeInteger( -1 * 10 * 1000 * 500),
                    &DeviceExtension->HidMiniWriteDPCObject );
    }
    return ntStatus;
}

NTSTATUS HidMiniGetStringDescriptor( IN PDEVICE_OBJECT DeviceObject, 
                                     IN PIRP Irp)
/*++

Routine Description:

    Get the device string descriptor, if any.

Arguments:

    DeviceObject - pointer to a device object.

    Irp - Pointer to Interrupt Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION DeviceExtension;
    PIO_STACK_LOCATION IrpStack;
    ULONG  bytesToCopy;

    DBGPrint(("HIDMINI.SYS: HidMiniGetStringDescriptor Enter\n"));

    DeviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    //  Get the buffer size to write into
    //
    bytesToCopy = IrpStack->Parameters.DeviceIoControl.InputBufferLength;

    //
    //  Make sure we have a buffer and it has some space
    //
    if (Irp->UserBuffer && bytesToCopy)
    {

        //
        //  Adjust the size to the amount we have to write
        //

        if (bytesToCopy > sizeof(AStringDescriptor))
        {
            bytesToCopy = sizeof(AStringDescriptor);
        }

        DBGPrint(("HIDMINI.SYS: Copying %d bytes to STRING buffer\n", bytesToCopy));

        RtlCopyMemory((PUCHAR) Irp->UserBuffer, (PUCHAR) DeviceExtension->StringDescriptor, bytesToCopy);

        //
        // Report how many bytes were copied
        //
        Irp->IoStatus.Information = bytesToCopy;

    }
    else
    {
        ntStatus = STATUS_INVALID_USER_BUFFER;
    }

    Irp->IoStatus.Status = ntStatus;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DBGPrint(("HIDMINI.SYS: HidMiniGetStringDescriptor Exit = 0x%x\n", ntStatus));

    return ntStatus;
}

NTSTATUS HidMiniOpenCollection(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)

/*++

Routine Description:

    Called when a HIDCLASS client opens this collection

Arguments:

   DeviceObject - Pointer to class device object.

   IrpStack     - Pointer to Interrupt Request Packet.


Return Value:

   STATUS_SUCCESS, STATUS_PENDING.

--*/

{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    DBGPrint(("HIDMINI.SYS: HidMiniOpenCollection Enter\n"));

    Irp->IoStatus.Status = ntStatus;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DBGPrint(("HIDMINI.SYS: HidMiniOpenCollection Exit = 0x%x\n", ntStatus));

    return ntStatus;
}




NTSTATUS HidMiniCloseCollection(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)

/*++

Routine Description:

    Called when a HIDCLASS client closes this collection

Arguments:

   DeviceObject - Pointer to class device object.

   IrpStack     - Pointer to Interrupt Request Packet.


Return Value:

   STATUS_SUCCESS, STATUS_PENDING.

--*/

{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    DBGPrint(("HIDMINI.SYS: HidMiniCloseCollection Enter\n"));

    Irp->IoStatus.Status = ntStatus;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DBGPrint(("HIDMINI.SYS: HidMiniCloseCollection Exit = 0x%x\n", ntStatus));

    return ntStatus;
}

