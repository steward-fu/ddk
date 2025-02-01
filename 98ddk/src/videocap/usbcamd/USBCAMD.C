/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    usbcamd.c

Abstract:

    USB device driver for camera

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.


Revision History:


--*/

#include "usbcamd.h"

extern ULONG USBCAMD_CameraFRC;

#define DEADMAN_TIMEOUT     5000     //timeout in ms
//
// Global pointer to Driver Object
//
PDRIVER_OBJECT USBCAMD_DriverObject;

#if DBG
ULONG USBCAMD_StreamEnable = 1;
// Global debug vars
ULONG USBCAMD_DebugTraceLevel = 
#ifdef MAX_DEBUG
    MAX_TRACE;
#else
    MIN_TRACE;
#endif
#endif





#define USBCAMD_NUMBER_OF_PIPES_PER_INTERFACE   2

#ifdef DEADMAN_TIMER
VOID
USBCAMD_SyncUrbTimeoutDPC(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL. 

    
    
Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - 

    SystemArgument1 - not used.
    
    SystemArgument2 - not used.

Return Value:

    None.

--*/
{
    PIRP irp;
#if DBG
    BOOLEAN status;
#endif

    irp = DeferredContext;    

    
#if DBG
    status = 
#endif
        IoCancelIrp(irp);

#if DBG
    ASSERT(status == TRUE);    
#endif    
}
#endif /* DEADMAN_TIMER */


//---------------------------------------------------------------------------
// USBCAMD_StartDevice
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_StartDevice(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension
    )
/*++

Routine Description:

    Initializes a given instance of the camera device on the USB.

Arguments:

    deviceExtension - points to the driver specific DeviceExtension

    Irp - Irp associated with this request


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
    PUSB_DEVICE_DESCRIPTOR deviceDescriptor = NULL;
    PURB urb;
    ULONG siz;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_StartDevice\n"));

#ifdef DEBUG_LOG
    USBCAMD_LogInit();
#endif /* DEBUG_LOG */

    //
    // Fetch the device descriptor for the device
    //

    urb = USBCAMD_ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if (urb) {

        siz = sizeof(USB_DEVICE_DESCRIPTOR);

        deviceDescriptor = USBCAMD_ExAllocatePool(NonPagedPool,
                                                  siz);

        if (deviceDescriptor) {

            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_DEVICE_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         deviceDescriptor,
                                         NULL,
                                         siz,
                                         NULL);

            ntStatus = USBCAMD_CallUSBD(DeviceExtension, urb);

            if (NT_SUCCESS(ntStatus)) {
                USBCAMD_KdPrint (MAX_TRACE, ("'Device Descriptor = %x, len %x\n",
                                deviceDescriptor,
                                urb->UrbControlDescriptorRequest.TransferBufferLength));

                USBCAMD_KdPrint (MAX_TRACE, ("'USBCAMD Device Descriptor:\n"));
                USBCAMD_KdPrint (MAX_TRACE, ("'-------------------------\n"));
                USBCAMD_KdPrint (MAX_TRACE, ("'bLength %d\n", deviceDescriptor->bLength));
                USBCAMD_KdPrint (MAX_TRACE, ("'bDescriptorType 0x%x\n", deviceDescriptor->bDescriptorType));
                USBCAMD_KdPrint (MAX_TRACE, ("'bcdUSB 0x%x\n", deviceDescriptor->bcdUSB));
                USBCAMD_KdPrint (MAX_TRACE, ("'bDeviceClass 0x%x\n", deviceDescriptor->bDeviceClass));
                USBCAMD_KdPrint (MAX_TRACE, ("'bDeviceSubClass 0x%x\n", deviceDescriptor->bDeviceSubClass));
                USBCAMD_KdPrint (MAX_TRACE, ("'bDeviceProtocol 0x%x\n", deviceDescriptor->bDeviceProtocol));
                USBCAMD_KdPrint (MAX_TRACE, ("'bMaxPacketSize0 0x%x\n", deviceDescriptor->bMaxPacketSize0));
                USBCAMD_KdPrint (MAX_TRACE, ("'idVendor 0x%x\n", deviceDescriptor->idVendor));
                USBCAMD_KdPrint (MAX_TRACE, ("'idProduct 0x%x\n", deviceDescriptor->idProduct));
                USBCAMD_KdPrint (MAX_TRACE, ("'bcdDevice 0x%x\n", deviceDescriptor->bcdDevice));
                USBCAMD_KdPrint (MIN_TRACE, ("'iManufacturer 0x%x\n", deviceDescriptor->iManufacturer));
                USBCAMD_KdPrint (MAX_TRACE, ("'iProduct 0x%x\n", deviceDescriptor->iProduct));
                USBCAMD_KdPrint (MAX_TRACE, ("'iSerialNumber 0x%x\n", deviceDescriptor->iSerialNumber));
                USBCAMD_KdPrint (MAX_TRACE, ("'bNumConfigurations 0x%x\n", deviceDescriptor->bNumConfigurations));
            }
        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

        if (NT_SUCCESS(ntStatus)) {
            DeviceExtension->DeviceDescriptor = deviceDescriptor;
        } else if (deviceDescriptor) {
            USBCAMD_ExFreePool(deviceDescriptor);
        }

        USBCAMD_ExFreePool(urb);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Now configure the device.
    //

    if (NT_SUCCESS(ntStatus)) {
        ntStatus = USBCAMD_ConfigureDevice(DeviceExtension);
    }

    if (NT_SUCCESS(ntStatus)) {
        //
        // initialize our f ref count and semaphores
        //
        DeviceExtension->OpenFRC = 0;

        KeInitializeSemaphore(&DeviceExtension->Semaphore, 1, 1);

        DeviceExtension->TimeoutCount = -1;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_StartDevice (%x)\n", ntStatus));

    //TRAP_ERROR(ntStatus);
    LOGENTRY("STDV", 0, 0, ntStatus);

    return ntStatus;
}


//---------------------------------------------------------------------------
// USBCAMD_RemoveDevice
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_RemoveDevice(
    IN PUSBCAMD_DEVICE_EXTENSION  DeviceExtension
    )
/*++

Routine Description:

    Removes a given instance of the USB camera.

    NOTE: When we get a remove we can asume the device is gone.

Arguments:

    deviceExtension - points to the driver specific DeviceExtension

    Irp - Irp associated with this request

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_RemoveDevice\n"));

    ASSERT(DeviceExtension->OpenFRC == 0);

    ntStatus = (*DeviceExtension->DeviceData.CamUnInitialize)(
                     DeviceExtension->StackDeviceObject,
                     USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension));

    if (DeviceExtension->DeviceDescriptor) {
        USBCAMD_ExFreePool(DeviceExtension->DeviceDescriptor);
    }

    //
    // Free up any interface structures
    //

    if (DeviceExtension->Interface) {
        USBCAMD_ExFreePool(DeviceExtension->Interface);
    }

    USBCAMD_CameraFRC--;
    if (USBCAMD_CameraFRC == 0) {

#ifdef DEBUG_LOG
        USBCAMD_LogFree();
#endif        
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_RemoveDevice (%x)\n", ntStatus));

    TRAP_ERROR(ntStatus);

    LOGENTRY("RMDV", 0, 0, ntStatus);


    return ntStatus;
}

#if 0
//---------------------------------------------------------------------------
// USBCAMD_StopDevice
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_StopDevice(
    IN PUSBCAMD_DEVICE_EXTENSION  DeviceExtension
    )
/*++

Routine Description:

    Stops a given instance of the USB camera.

Arguments:

    deviceExtension - points to the driver specific DeviceExtension

    Irp - Irp associated with this request

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb;
    ULONG siz;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_StopDevice\n"));

    ASSERT(DeviceExtension->OpenFRC == 0);

    //
    // Send the select configuration urb with a NULL pointer for the configuration
    // handle, this closes the configuration and puts the device in the 'unconfigured'
    // state.
    //

    siz = sizeof(struct _URB_SELECT_CONFIGURATION);

    urb = USBCAMD_ExAllocatePool(NonPagedPool,
                         siz);

    if (urb) {
        NTSTATUS status;

        UsbBuildSelectConfigurationRequest(urb,
                                          (USHORT) siz,
                                          NULL);

        status = USBCAMD_CallUSBD(DeviceExtension, urb);

        USBCAMD_KdPrint (MAX_TRACE, ("'Device Configuration Closed status = %x usb status = %x.\n",
                        status, urb->UrbHeader.Status));

        USBCAMD_ExFreePool(urb);
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_StopDevice (%x)\n", ntStatus));

    TRAP_ERROR(ntStatus);

    LOGENTRY("SPDV", 0, 0, ntStatus);

    return ntStatus;
}
#endif


//---------------------------------------------------------------------------
// USBCAMD_CallUSBD
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_CallUSBD(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PURB Urb
    )
/*++

Routine Description:

    Passes a URB to the USBD class driver

Arguments:

    DeviceExtension - pointer to the device extension for this instance of an USB camera

    Urb - pointer to Urb request block

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_CallUSBD\n"));

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_SUBMIT_URB,
                DeviceExtension->StackDeviceObject,
                NULL,
                0,
                NULL,
                0,
                TRUE, /* INTERNAL */
                &event,
                &ioStatus);

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);

    //
    // pass the URB to the USB driver stack
    //
    nextStack->Parameters.Others.Argument1 = Urb;

    USBCAMD_KdPrint (MAX_TRACE, ("'calling USBD\n"));

    if (DeviceExtension->Initialized) {
        ntStatus = IoCallDriver(DeviceExtension->StackDeviceObject,
                                irp);
    } else {
        ntStatus = STATUS_DEVICE_DATA_ERROR;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

#ifdef DEADMAN_TIMER
        LARGE_INTEGER dueTime;

        KeInitializeTimer(&DeviceExtension->TimeoutTimer);
        KeInitializeDpc(&DeviceExtension->TimeoutDpc,
                        USBCAMD_SyncUrbTimeoutDPC,
                        irp);

        dueTime.QuadPart = -10000 * DEADMAN_TIMEOUT;

        KeSetTimer(&DeviceExtension->TimeoutTimer,
                   dueTime,
                   &DeviceExtension->TimeoutDpc);        

#endif /* DEADMAN_TIMER */  
        USBCAMD_KdPrint (MAX_TRACE, ( "'Wait for single object\n"));

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

        USBCAMD_KdPrint (MAX_TRACE, ("'Wait for single object, returned %x\n", status));
        
#ifdef DEADMAN_TIMER
        //
        // remove our timeoutDPC from the queue
        //
        KeCancelTimer(&DeviceExtension->TimeoutTimer);
#endif /* DEADMAN_TIMER */


    } else {
        ioStatus.Status = ntStatus;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'URB status = %x status = %x irp status %x\n",
        Urb->UrbHeader.Status, status, ioStatus.Status));

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    USBCAMD_KdPrint(MAX_TRACE, ("'exit USBCAMD_CallUSBD (%x)\n", ntStatus));

    return ntStatus;
}


//---------------------------------------------------------------------------
// USBCAMD_ConfigureDevice
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_ConfigureDevice(
    IN  PUSBCAMD_DEVICE_EXTENSION DeviceExtension
    )
/*++

Routine Description:

    Configure the USB camera.

Arguments:

    DeviceExtension - pointer to the device object for this instance of the USB camera
                    devcice.


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb;
    ULONG siz;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = NULL;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_ConfigureDevice\n"));

    //
    // configure the device
    //

    urb = USBCAMD_ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if (urb) {

        //
        // BUGBUG
        // USB camera chokes if on the next command if you don't get
        // the entire descriptor on the first try
        //

//        siz = sizeof(USB_CONFIGURATION_DESCRIPTOR)+256;
        siz = 0x40;

get_config_descriptor_retry:

        configurationDescriptor = USBCAMD_ExAllocatePool(NonPagedPool,
                                                 siz);

        if (configurationDescriptor) {

            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         configurationDescriptor,
                                         NULL,
                                         siz,
                                         NULL);

            ntStatus = USBCAMD_CallUSBD(DeviceExtension, urb);

            USBCAMD_KdPrint (MAX_TRACE, ("'Configuration Descriptor = %x, len %x\n",
                            configurationDescriptor,
                            urb->UrbControlDescriptorRequest.TransferBufferLength));
        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        // if we got some data see if it was enough.
        //
        // NOTE: we may get an error in URB because of buffer overrun
        //
        if (urb->UrbControlDescriptorRequest.TransferBufferLength>0 &&
                configurationDescriptor->wTotalLength > siz) {

            siz = configurationDescriptor->wTotalLength;
            USBCAMD_ExFreePool(configurationDescriptor);
            configurationDescriptor = NULL;
            goto get_config_descriptor_retry;
        }

        USBCAMD_ExFreePool(urb);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (configurationDescriptor) {

        //
        // Get our pipes
        //

        ntStatus = USBCAMD_SelectConfiguration(DeviceExtension, configurationDescriptor);

        if (NT_SUCCESS(ntStatus)) {
            ntStatus = (*DeviceExtension->DeviceData.CamInitialize)(
                      DeviceExtension->StackDeviceObject,
                      USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension));
                      
        }

        USBCAMD_ExFreePool(configurationDescriptor);

    }

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_ConfigureDevice (%x)\n", ntStatus));

    //TRAP_ERROR(ntStatus);

    return ntStatus;
}


//---------------------------------------------------------------------------
// USBCAMD_SelectConfiguration
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_SelectConfiguration(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    )
/*++

Routine Description:

    Initializes the USBCAMD camera to configuration one, interface zero

Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.

    ConfigurationDescriptor - pointer to the USB configuration
                    descriptor containing the interface and endpoint
                    descriptors.

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
    PURB urb;
    ULONG numberOfInterfaces;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor;
    PUSBD_INTERFACE_INFORMATION interface;
    PUSBD_INTERFACE_LIST_ENTRY interfaceList, tmp;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_SelectConfiguration\n"));

    //
    // get this from the config descriptor
    //
    numberOfInterfaces = ConfigurationDescriptor->bNumInterfaces;

    // We only support cameras with one interface
    ASSERT(numberOfInterfaces == 1);

    
    tmp = interfaceList =
        USBCAMD_ExAllocatePool(PagedPool, sizeof(USBD_INTERFACE_LIST_ENTRY) *
                       (numberOfInterfaces+1));


    if (tmp) {

        interfaceDescriptor =
            USBD_ParseConfigurationDescriptorEx(
                ConfigurationDescriptor,
                ConfigurationDescriptor,
                -1, //interface, don't care
                -1, //alt setting, don't care
                -1, // hub class
                -1, // subclass, don't care
                -1); // protocol, don't care
                           
        interfaceList->InterfaceDescriptor = 
            interfaceDescriptor;
        interfaceList++;

        interfaceList->InterfaceDescriptor = NULL;
        
        //
        // Allocate a URB big enough for this request
        //

        urb = USBD_CreateConfigurationRequestEx(ConfigurationDescriptor, tmp);

        if (urb) {
        
            ntStatus = 
                (*DeviceExtension->DeviceData.CamConfigure)(
                        DeviceExtension->StackDeviceObject,      
                        USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension),
                        tmp->Interface,
                        ConfigurationDescriptor,
                        &DeviceExtension->DataPipe,
                        &DeviceExtension->SyncPipe);

            USBCAMD_ExFreePool(tmp);
            
        } else { 
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
        
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (NT_SUCCESS(ntStatus)) {

        interface = &urb->UrbSelectConfiguration.Interface;

        USBCAMD_KdPrint (MAX_TRACE, ("'size of interface request = %d\n", interface->Length));

        ntStatus = USBCAMD_CallUSBD(DeviceExtension, urb);

        if (NT_SUCCESS(ntStatus) && USBD_SUCCESS(URB_STATUS(urb))) {

            //
            // Save the configuration handle for this device
            //

            DeviceExtension->ConfigurationHandle =
                urb->UrbSelectConfiguration.ConfigurationHandle;


            DeviceExtension->Interface = USBCAMD_ExAllocatePool(NonPagedPool,
                                                        interface->Length);

            if (DeviceExtension->Interface) {
                ULONG j;

                //
                // save a copy of the interface information returned
                //
                RtlCopyMemory(DeviceExtension->Interface, interface, interface->Length);

                //
                // Dump the interface to the debugger
                //
                USBCAMD_KdPrint (MAX_TRACE, ("'---------\n"));
                USBCAMD_KdPrint (MAX_TRACE, ("'NumberOfPipes 0x%x\n", DeviceExtension->Interface->NumberOfPipes));
                USBCAMD_KdPrint (MAX_TRACE, ("'Length 0x%x\n", DeviceExtension->Interface->Length));
                USBCAMD_KdPrint (MAX_TRACE, ("'Alt Setting 0x%x\n", DeviceExtension->Interface->AlternateSetting));
                USBCAMD_KdPrint (MAX_TRACE, ("'Interface Number 0x%x\n", DeviceExtension->Interface->InterfaceNumber));

                // Dump the pipe info

                for (j=0; j<interface->NumberOfPipes; j++) {
                    PUSBD_PIPE_INFORMATION pipeInformation;

                    pipeInformation = &DeviceExtension->Interface->Pipes[j];

                    USBCAMD_KdPrint (MAX_TRACE, ("'---------\n"));
                    USBCAMD_KdPrint (MAX_TRACE, ("'PipeType 0x%x\n", pipeInformation->PipeType));
                    USBCAMD_KdPrint (MAX_TRACE, ("'EndpointAddress 0x%x\n", pipeInformation->EndpointAddress));
                    USBCAMD_KdPrint (MAX_TRACE, ("'MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize));
                    USBCAMD_KdPrint (MAX_TRACE, ("'Interval 0x%x\n", pipeInformation->Interval));
                    USBCAMD_KdPrint (MAX_TRACE, ("'Handle 0x%x\n", pipeInformation->PipeHandle));
                }

                USBCAMD_KdPrint (MAX_TRACE, ("'---------\n"));

            }
        }

        // must call no wrapped function
        ExFreePool(urb);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_SelectConfiguration (%x)\n", ntStatus));

    TRAP_ERROR(ntStatus);

    return ntStatus;
}


//---------------------------------------------------------------------------
// USBCAMD_SelectAlternateInterface
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_SelectAlternateInterface(
    IN PVOID DeviceContext,
    IN PUSBD_INTERFACE_INFORMATION RequestInterface
    )
/*++

Routine Description:

    Select one of the cameras alternate interfaces

Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.

    ChannelExtension - extension specific to this video channel

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;
    PUSBD_INTERFACE_INFORMATION interface;
    PUSBCAMD_DEVICE_EXTENSION deviceExtension;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_SelectAlternateInterface\n"));

    deviceExtension = USBCAMD_GET_DEVICE_EXTENSION(DeviceContext);

    //
    // Dump the current interface
    //

    ASSERT(deviceExtension->Interface != NULL);

    //
    // we know that all interfaces have two pipes
    //

    siz = GET_SELECT_INTERFACE_REQUEST_SIZE(USBCAMD_NUMBER_OF_PIPES_PER_INTERFACE);

    USBCAMD_KdPrint (MAX_TRACE, ("'size of interface request Urb = %d\n", siz));

    urb = USBCAMD_ExAllocatePool(NonPagedPool,
                         siz);

    if (urb) {

        interface = &urb->UrbSelectInterface.Interface;

        RtlCopyMemory(interface,
                      RequestInterface,
                      RequestInterface->Length);
                      
        // set up the request for the first and only interface
       
        USBCAMD_KdPrint (MAX_TRACE, ("'size of interface request = %d\n", interface->Length));

        urb->UrbHeader.Function = URB_FUNCTION_SELECT_INTERFACE;
        urb->UrbSelectInterface.ConfigurationHandle =
            deviceExtension->ConfigurationHandle;

        ntStatus = USBCAMD_CallUSBD(deviceExtension, urb);


        if (NT_SUCCESS(ntStatus) && USBD_SUCCESS(URB_STATUS(urb))) {

            ULONG j;

            //
            // save a copy of the interface information returned
            //
            RtlCopyMemory(deviceExtension->Interface, interface, interface->Length);
            RtlCopyMemory(RequestInterface, interface, interface->Length);

            //
            // Dump the interface to the debugger
            //
            USBCAMD_KdPrint (MAX_TRACE, ("'---------\n"));
            USBCAMD_KdPrint (MAX_TRACE, ("'NumberOfPipes 0x%x\n", deviceExtension->Interface->NumberOfPipes));
            USBCAMD_KdPrint (MAX_TRACE, ("'Length 0x%x\n", deviceExtension->Interface->Length));
            USBCAMD_KdPrint (MAX_TRACE, ("'Alt Setting 0x%x\n", deviceExtension->Interface->AlternateSetting));
            USBCAMD_KdPrint (MAX_TRACE, ("'Interface Number 0x%x\n", deviceExtension->Interface->InterfaceNumber));

            // Dump the pipe info

            for (j=0; j<interface->NumberOfPipes; j++) {
                PUSBD_PIPE_INFORMATION pipeInformation;

                pipeInformation = &deviceExtension->Interface->Pipes[j];

                USBCAMD_KdPrint (MAX_TRACE, ("'---------\n"));
                USBCAMD_KdPrint (MAX_TRACE, ("'PipeType 0x%x\n", pipeInformation->PipeType));
                USBCAMD_KdPrint (MAX_TRACE, ("'EndpointAddress 0x%x\n", pipeInformation->EndpointAddress));
                USBCAMD_KdPrint (MAX_TRACE, ("'MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize));
                USBCAMD_KdPrint (MAX_TRACE, ("'Interval 0x%x\n", pipeInformation->Interval));
                USBCAMD_KdPrint (MAX_TRACE, ("'Handle 0x%x\n", pipeInformation->PipeHandle));
            }

            //
            // success update our internal state to
            // indicate the new frame rate
            //

            USBCAMD_KdPrint (MAX_TRACE, ("'Selecting Camera Interface\n"));
        }

        USBCAMD_ExFreePool(urb);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_SelectAlternateInterface (%x)\n", ntStatus));

//    TRAP_ERROR(ntStatus);

    return ntStatus;
}


//---------------------------------------------------------------------------
// USBCAMD_OpenChannel
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_OpenChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PVOID Format
    )
/*++

Routine Description:

    Opens a video stream on the device.

Arguments:

    DeviceExtension - points to the driver specific DeviceExtension
    ChannelExtension - context data for this channel.
    Format - pointer to format information associated with this 
            channel.

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG i;

    USBCAMD_SERIALIZE(DeviceExtension);
    
    USBCAMD_KdPrint( MIN_TRACE, ("'enter USBCAMD_OpenChannel %x\n", Format));

    //
    // Initialize structures for this channel
    //
    ChannelExtension->Sig = USBCAMD_CHANNEL_SIG;
    ChannelExtension->DeviceExtension = DeviceExtension;
    ChannelExtension->CurrentFormat = Format;
    ChannelExtension->RawFrameLength = 0;

#if DBG
    // verify our serialization is working
    ChannelExtension->InCam = 0;
    ChannelExtension->InCam++;
    ASSERT(ChannelExtension->InCam == 1);
#endif

    if (DeviceExtension->OpenFRC > 0) {
        // channel already open
        TEST_TRAP();
        ntStatus = STATUS_DEVICE_DATA_ERROR;
        goto USBCAMD_OpenChannel_Done;
    }
    
    DeviceExtension->OpenFRC++;

    //
    // empty read list
    //
    InitializeListHead(&ChannelExtension->PendingIoList);

    //
    // no current Irp
    //
    ChannelExtension->CurrentRequest = NULL;

    //
    // streaming is off
    //
    ChannelExtension->ImageCaptureStarted = FALSE;

    //
    // Channel not prepared
    //
    ChannelExtension->ChannelPrepared = FALSE;

    //
    // No error condition
    //
    ChannelExtension->StreamError = FALSE;

    //
    // no stop, reset requests are pending
    //
    ChannelExtension->Flags = 0;

    //
    // initialize the io list spin lock
    //

    KeInitializeSpinLock(&ChannelExtension->PendingIoListSpin);

	//
	//  initialize pin semaphore
	//
	
	KeInitializeSemaphore(&ChannelExtension->PinSemaphore, 1, 1);

    //
    // initialize streaming structures
    //

    for (i=0; i< USBCAMD_MAX_REQUEST; i++) {
        ChannelExtension->TransferExtension[i].ChannelExtension = NULL;
    }

//    USBCAMD_InitProperties(DeviceExtension, Irp, ChannelExtension);

USBCAMD_OpenChannel_Done:

    USBCAMD_KdPrint( MIN_TRACE, ("'exit USBCAMD_OpenChannel (%x)\n", ntStatus));

    TRAP_ERROR(ntStatus);
    LOGENTRY("OPNc", 0, 0, ntStatus);

#if DBG
    ChannelExtension->InCam--;
    ASSERT(ChannelExtension->InCam == 0);
#endif

    USBCAMD_RELEASE(DeviceExtension);

    return ntStatus;
}


//---------------------------------------------------------------------------
// USBCAMD_CloseChannel
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_CloseChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    )
/*++

Routine Description:

    Closes a video channel.

Arguments:

    DeviceExtension - points to the driver specific DeviceExtension

    ChannelExtension - context data for this channel.

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PUSBCAMD_READ_EXTENSION readExtension;

    USBCAMD_SERIALIZE(DeviceExtension);
    
    USBCAMD_KdPrint( MIN_TRACE, ("'enter USBCAMD_CloseChannel\n"));

#if DBG
    ChannelExtension->InCam++;
    ASSERT(ChannelExtension->InCam == 1);
#endif    

    LOGENTRY("CLSc", ChannelExtension, 0, 0);

    DeviceExtension->OpenFRC--;
    //
    // since we only support one channel this
    // should be zero
    //
    ASSERT(DeviceExtension->OpenFRC == 0);

    //
    // NOTE:
    // image capture should be stopped/unprepared when we get here
    //

    ASSERT_CHANNEL(ChannelExtension);
    ASSERT(ChannelExtension->ImageCaptureStarted == FALSE);
    ASSERT(ChannelExtension->CurrentRequest == NULL);
    ASSERT(ChannelExtension->ChannelPrepared == FALSE);


    //
    // We are going to complete any reads left here
    //
#pragma warning(disable:4127)
    do {
        PLIST_ENTRY listEntry;

        listEntry =
              ExInterlockedRemoveHeadList(&(ChannelExtension->PendingIoList),
                                          &ChannelExtension->PendingIoListSpin);

        if (listEntry != NULL) {
//            TEST_TRAP();
            readExtension = (PUSBCAMD_READ_EXTENSION) CONTAINING_RECORD(listEntry, 
                                             USBCAMD_READ_EXTENSION, 
                                             ListEntry);          

            LOGENTRY("cRDD", readExtension, 0, ntStatus);

            USBCAMD_CompleteRead(ChannelExtension,
                                 readExtension, 
                                 STATUS_DEVICE_DATA_ERROR, 
                                 0);
        } else {
            break;
        }

    } while (1);
#pragma warning(default:4127)

    USBCAMD_KdPrint( MIN_TRACE, ("'exit USBCAMD_CloseChannel (%x)\n", ntStatus));

    TRAP_ERROR(ntStatus);
    LOGENTRY("CLSc", ChannelExtension, 0, ntStatus);

#if DBG
    ChannelExtension->InCam--;
    ASSERT(ChannelExtension->InCam == 0);
#endif

    USBCAMD_RELEASE(DeviceExtension);

    //
    // allow any pending reset events to run now
    //
    while (DeviceExtension->TimeoutCount >= 0) {
        // BUGBUG
        // DelayExecutionThread Here??
    }

    return ntStatus;
}


//---------------------------------------------------------------------------
// USBCAMD_PrepareChannel
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_PrepareChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    )
/*++

Routine Description:

    Prepare the Video channel for streaming, this is where the necessary
        USB BW is allocated.

Arguments:

    DeviceExtension - points to the driver specific DeviceExtension

    Irp - Irp associated with this request.

    ChannelExtension - context data for this channel.

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG i;

    USBCAMD_SERIALIZE(DeviceExtension);
    
    USBCAMD_KdPrint (MIN_TRACE, ("'enter USBCAMD_PrepareChannel\n"));

    ASSERT_CHANNEL(ChannelExtension);
    ASSERT(DeviceExtension->OpenFRC > 0);
    //ASSERT(ChannelExtension->ImageCaptureStarted == FALSE);
    //ASSERT(ChannelExtension->ChannelPrepared == FALSE);
    if (ChannelExtension->ChannelPrepared ||
        ChannelExtension->ImageCaptureStarted) {
        // fail the call if the channel is not in the
        // proper state.
        TRAP();
        ntStatus = STATUS_UNSUCCESSFUL;
        goto USBCAMD_PrepareChannel_Done;
    }

    //
    // This driver function will select the appropriate alternate 
    // interface.  
    // This code performs the select_alt interface and gets us the 
    // pipehandles
    //
    
    ntStatus = 
        (*DeviceExtension->DeviceData.CamAllocateBandwidth)(
                DeviceExtension->StackDeviceObject,      
                USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension),
                &ChannelExtension->RawFrameLength,
                ChannelExtension->CurrentFormat);

    if (NT_SUCCESS(ntStatus)) {
        ntStatus = 
            (*DeviceExtension->DeviceData.CamStartCapture)(
                    DeviceExtension->StackDeviceObject,      
                    USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension));                
    }
    
    if (NT_SUCCESS(ntStatus)) {
    
        //
        // we have the BW, go ahead and initailize our Iso structures
        //

        ChannelExtension->DataPipe = DeviceExtension->DataPipe;
        ChannelExtension->SyncPipe = DeviceExtension->SyncPipe;
        
        for (i=0; i< USBCAMD_MAX_REQUEST; i++) {

            ntStatus = USBCAMD_InitializeIsoTransfer(DeviceExtension,
                                                     ChannelExtension,
                                                     DeviceExtension->Interface,
                                                     &ChannelExtension->TransferExtension[i]);

            if (!NT_SUCCESS(ntStatus)) {
                TEST_TRAP();

                // BUGBUG
                // The close channel code will clean up anything we
                // allocated
                //
                break;
            }
        }
    }

    if (NT_SUCCESS(ntStatus)) {

        //
        // we have the BW and memory we need, go ahead and start
        // our timeoutDPC
        //

        BOOLEAN inQueue;
        LARGE_INTEGER dueTime;

        ChannelExtension->ChannelPrepared = TRUE;
        //
        // start out ChannelTimeoutDPC here
        //
        ChannelExtension->Flags |= USBCAMD_ENABLE_TIMEOUT_DPC;

        KeInitializeTimer(&ChannelExtension->TimeoutTimer);
        KeInitializeDpc(&ChannelExtension->TimeoutDpc,
                        USBCAMD_ChannelTimeoutDPC,
                        ChannelExtension);
                        
        dueTime.QuadPart = -10000 * USBCAMD_TIMEOUT_INTERVAL;
        inQueue = KeSetTimer(&ChannelExtension->TimeoutTimer,
                             dueTime,
                             &ChannelExtension->TimeoutDpc);

        ASSERT(inQueue == FALSE);
    }

USBCAMD_PrepareChannel_Done:

    USBCAMD_KdPrint (MIN_TRACE, ("'exit USBCAMD_PrepareChannel (%x)\n", ntStatus));

    TRAP_ERROR(ntStatus);
    LOGENTRY("PRPc", 0, 0, ntStatus);

    USBCAMD_RELEASE(DeviceExtension);
    
    return ntStatus;
}


//---------------------------------------------------------------------------
// USBCAMD_UnPrepareChannel
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_UnPrepareChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    )
/*++

Routine Description:

    Frees resources allocated in PrepareChannel.

Arguments:

    DeviceExtension - points to the driver specific DeviceExtension

    Irp - Irp associated with this request.

    ChannelExtension - context data for this channel.

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG i;

    USBCAMD_SERIALIZE(DeviceExtension);
    
    USBCAMD_KdPrint (MIN_TRACE, ("'enter USBCAMD_UnPrepareChannel\n"));

    ASSERT_CHANNEL(ChannelExtension);
    ASSERT(DeviceExtension->OpenFRC > 0);

    if (!ChannelExtension->ChannelPrepared ||
        ChannelExtension->ImageCaptureStarted) {
        // fail the call if the channel is not in the
        // proper state.
        TRAP();
        ntStatus = STATUS_UNSUCCESSFUL;
        goto USBCAMD_UnPrepareChannel_Done;
    }

    //
    // stop our timeoutDPC routine
    //

    ChannelExtension->Flags &= ~USBCAMD_ENABLE_TIMEOUT_DPC;
    KeCancelTimer(&ChannelExtension->TimeoutTimer);

    //
    // hopefully put us in the mode that uses no bandwidth
    // ie select and alt interface that has a minimum iso
    // packet size
    //

    // attempt to stop
    (*DeviceExtension->DeviceData.CamStopCapture)(
            DeviceExtension->StackDeviceObject,      
            USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension));

    ntStatus = 
        (*DeviceExtension->DeviceData.CamFreeBandwidth)(
                DeviceExtension->StackDeviceObject,      
                USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension));

    if (!NT_SUCCESS(ntStatus)) {
        USBCAMD_KdPrint (MIN_TRACE, (
            "USBCAMD_UnPrepareChannel failed stopcapture  (%x)\n", ntStatus));
            
        TEST_TRAP();

        //
        // ignore any errors on the stop
        //
        ntStatus = STATUS_SUCCESS;
    }

    //
    // Note:
    // We may get an error here if the camera hs been unplugged,
    // if this is the case we still need to free up the
    // channel resources
    //

    for (i=0; i< USBCAMD_MAX_REQUEST; i++) {
        USBCAMD_FreeIsoTransfer(ChannelExtension,
                                &ChannelExtension->TransferExtension[i]);
    }

    //
    // channel is no longer prepared
    //
    ChannelExtension->ChannelPrepared = FALSE;

USBCAMD_UnPrepareChannel_Done:

    USBCAMD_KdPrint (MIN_TRACE, ("'exit USBCAMD_UnPrepareChannel (%x)\n", ntStatus));
    
    //TRAP_ERROR(ntStatus);
    LOGENTRY("UNPc", 0, 0, ntStatus);
    
    USBCAMD_RELEASE(DeviceExtension);

    return ntStatus;
}

//---------------------------------------------------------------------------
// USBCAMD_ReadChannel
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_ReadChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN PUSBCAMD_READ_EXTENSION ReadExtension
    )
/*++

Routine Description:

    Reads a video frame from a channel.

Arguments:

    DeviceExtension - points to the driver specific DeviceExtension

    Irp - Irp associated with this request.

    ChannelExtension - context data for this channel.

    Mdl - Mdl for this read request.

    Length - Number of bytes to read.

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_ReadChannel\n"));

    //
    // make sure we don't get reads on a closed channel
    //
    
    ASSERT_READ(ReadExtension);
    ASSERT_CHANNEL(ChannelExtension);
    ASSERT(DeviceExtension->OpenFRC > 0);
    ASSERT(ChannelExtension->ChannelPrepared == TRUE);

#if DBG
    if (USBCAMD_StreamEnable == 0) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }        
#endif

    // BUGBUG
    ASSERT(ChannelExtension->RawFrameLength > 0 && 
           ChannelExtension->RawFrameLength < 400000 );
    USBCAMD_KdPrint (MAX_TRACE, ("'RawFrameLength %d\n", 
        ChannelExtension->RawFrameLength));           

    if (  ChannelExtension->RawFrameLength == 0) {
		 return STATUS_INSUFFICIENT_RESOURCES;	
	} 
    

    ReadExtension->RawFrameLength = ChannelExtension->RawFrameLength; 

    ReadExtension->RawFrameBuffer = 
        USBCAMD_AllocateRawFrameBuffer(ReadExtension->RawFrameLength);
        

    if (ReadExtension->RawFrameBuffer == NULL) {
        //
        // failed to allocate a raw frame buffer
        //
        USBCAMD_KdPrint (MIN_TRACE, ("'Read alloc failed\n"));    
        TEST_TRAP();
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        
    } else {
        USBCAMD_KdPrint (MIN_TRACE, ("'Read SRB %x is Queued \n",ReadExtension->Srb ));           

        ExInterlockedInsertTailList( &(ChannelExtension->PendingIoList),
                                     &(ReadExtension->ListEntry),
                                     &ChannelExtension->PendingIoListSpin);
        ntStatus = STATUS_SUCCESS;                                     
    }                                     

    ntStatus = STATUS_SUCCESS;
    
    LOGENTRY("PENr", ReadExtension, &(ChannelExtension->PendingIoList), ntStatus);

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_ReadChannel 0x%x\n", ntStatus));

    return ntStatus;
}

//---------------------------------------------------------------------------
// USBCAMD_StartChannel
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_StartChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION  ChannelExtension
    )
/*++

Routine Description:

    Starts the streaming process for a video channel.

Arguments:

    DeviceExtension - points to the driver specific DeviceExtension

    ChannelExtension - context data for this channel.

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    USBCAMD_SERIALIZE(DeviceExtension);
    
    USBCAMD_KdPrint (MIN_TRACE, ("'enter USBCAMD_StartChannel\n"));
    LOGENTRY("Stch", 0, ChannelExtension, 0);

    ASSERT_CHANNEL(ChannelExtension);

    if (ChannelExtension->ImageCaptureStarted) {
        // fail the call if the channel is not in the
        // proper state.
        TRAP();
        ntStatus = STATUS_UNSUCCESSFUL;
        goto USBCAMD_StartChannel_Done;
    }
    
#if DBG
    {
    ULONG i;

    ASSERT(DeviceExtension->OpenFRC > 0);
    ASSERT(ChannelExtension->StreamError == FALSE);
    //ASSERT(ChannelExtension->Flags == 0);

    for (i=0; i< USBCAMD_MAX_REQUEST; i++) {
        ASSERT(ChannelExtension->TransferExtension[i].ChannelExtension != NULL);
    }
    }
#endif

    //
    // Perform a reset on the pipes
    //
    
    ntStatus = USBCAMD_ResetPipes(DeviceExtension,
                                  ChannelExtension,
                                  DeviceExtension->Interface,
                                  FALSE);
    
    //
    // start the stream up, we don't check for errors here
    //

    if (NT_SUCCESS(ntStatus)) {
        USBCAMD_StartIsoStream(DeviceExtension, ChannelExtension, TRUE);
    }        

USBCAMD_StartChannel_Done:

    USBCAMD_KdPrint (MIN_TRACE, ("'exit USBCAMD_StartChannel (%x)\n", ntStatus));

    TRAP_ERROR(ntStatus);
    LOGENTRY("STRc", 0, 0, ntStatus);

    USBCAMD_RELEASE(DeviceExtension);

    return ntStatus;
}

//---------------------------------------------------------------------------
// USBCAMD_StopChannel
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_StopChannel(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension
    )
/*++

Routine Description:

    Stops the streaming process for a video channel.

Arguments:

    DeviceExtension - points to the driver specific DeviceExtension

    Irp - Irp associated with this request.

    ChannelExtension - context data for this channel.

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    USBCAMD_SERIALIZE(DeviceExtension);
    
    USBCAMD_KdPrint (MIN_TRACE, ("'enter USBCAMD_StopChannel\n"));
    LOGENTRY("Spch", 0, ChannelExtension, 0);

    ASSERT_CHANNEL(ChannelExtension);
    ASSERT(ChannelExtension->ChannelPrepared == TRUE);
    ASSERT(DeviceExtension->OpenFRC > 0);
//    ASSERT(ChannelExtension->Flags == 0);

    if (!ChannelExtension->ImageCaptureStarted) {
        //
        // we are not started so we just return success
        //
        USBCAMD_KdPrint (MAX_TRACE, ("'stop before start -- return success\n"));
        ntStatus = STATUS_SUCCESS;
        goto USBCAMD_StopChannel_Done;
    }
    
    //
    // first we set our stop flag
    //
    // initialize the event before setting StopIrp
    // since StopIrp is what our DPC looks at.
    //
    //
    
    KeInitializeEvent(&ChannelExtension->StopEvent, NotificationEvent, FALSE);
    ChannelExtension->Flags |= USBCAMD_STOP_STREAM;

    //
    // now send an abort pipe for both our pipes, this should flush out any 
    // transfers that are running
    //

    USBCAMD_AbortPipe(DeviceExtension,
            DeviceExtension->Interface->Pipes[ChannelExtension->DataPipe].PipeHandle);

    if (ChannelExtension->SyncPipe != -1) {
        USBCAMD_AbortPipe(DeviceExtension,
                DeviceExtension->Interface->Pipes[ChannelExtension->SyncPipe].PipeHandle);
    }                

    //
    // block the stop for now, we will let our timeoutDPC complete
    // it when all iso irps are no longer pending
    //

    {
    NTSTATUS status;

    LOGENTRY("Swat", 0, ChannelExtension, 0);
    status = KeWaitForSingleObject(
                       &ChannelExtension->StopEvent,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

    LOGENTRY("Swak", 0, ChannelExtension, status);
    }

    //
    // clear the error state flag, we are now stopped
    //
    
    ChannelExtension->StreamError = FALSE;
    ChannelExtension->ImageCaptureStarted = FALSE;

USBCAMD_StopChannel_Done:

#if DBG
    USBCAMD_DebugStats(ChannelExtension);
#endif

    USBCAMD_KdPrint (MIN_TRACE, ("'exit USBCAMD_StopChannel (%x)\n", ntStatus));
    LOGENTRY("STPc", 0, 0, ntStatus);
    
    USBCAMD_RELEASE(DeviceExtension);

    return ntStatus;
}

//---------------------------------------------------------------------------
// USBCAMD_AbortPipe
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_AbortPipe(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN USBD_PIPE_HANDLE PipeHandle
    )
/*++

Routine Description:

    Abort pending transfers for a given USB pipe.

Arguments:

    DeviceExtension - Pointer to the device extension for this instance of the USB camera
                    devcice.

    PipeHandle - usb pipe handle to abort trasnsfers for.


Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    PURB urb;
    ULONG currentUSBFrame = 0;

    urb = USBCAMD_ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_PIPE_REQUEST));

    if (urb) {

        urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
        urb->UrbPipeRequest.PipeHandle = PipeHandle;

        ntStatus = USBCAMD_CallUSBD(DeviceExtension, urb);

        USBCAMD_ExFreePool(urb);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    LOGENTRY("ABRp", PipeHandle, 0, ntStatus);

    return ntStatus;
}


//---------------------------------------------------------------------------
// USBCAMD_StartStream
//---------------------------------------------------------------------------
VOID
USBCAMD_StartIsoStream(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PUSBCAMD_CHANNEL_EXTENSION ChannelExtension,
    IN BOOLEAN Initialize
    )
/*++

Routine Description:

    This is the code that starts the streaming process.

Arguments:

    DeviceExtension - Pointer to the device extension for this instance of the USB camera
                    device.

Return Value:

    NT status code.

--*/
{
    ULONG i;

#if DBG
    // initialize debug count variables
    ChannelExtension->IgnorePacketCount =
    ChannelExtension->ErrorDataPacketCount =
    ChannelExtension->ErrorSyncPacketCount =
    ChannelExtension->SyncNotAccessedCount =
    ChannelExtension->DataNotAccessedCount = 0;

    if (USBCAMD_StreamEnable == 0) {
        return;
    } 
#endif

    ChannelExtension->CurrentUSBFrame =
        USBCAMD_GetCurrentFrame(DeviceExtension) + 10;

    for (i=0; i<USBCAMD_MAX_REQUEST; i++) {
        PUSBCAMD_TRANSFER_EXTENSION transferExtension;

        transferExtension = &ChannelExtension->TransferExtension[i];

        if (Initialize) {

            if (ChannelExtension->SyncPipe != -1) {
                USBCAMD_RecycleIrp(transferExtension,
                                   transferExtension->SyncIrp,
                                   transferExtension->SyncUrb);

                USBCAMD_InitializeIsoUrb(DeviceExtension,
                                         transferExtension->SyncUrb,
                                         &DeviceExtension->Interface->Pipes[ChannelExtension->SyncPipe],
                                         transferExtension->SyncBuffer);

                RtlZeroMemory(transferExtension->SyncBuffer,
                        USBCAMD_NUM_ISO_PACKETS_PER_REQUEST);
            }
            
            USBCAMD_RecycleIrp(transferExtension,
                               transferExtension->DataIrp,
                               transferExtension->DataUrb);

            USBCAMD_InitializeIsoUrb(DeviceExtension,
                                     transferExtension->DataUrb,
                                     &DeviceExtension->Interface->Pipes[ChannelExtension->DataPipe],
                                     transferExtension->DataBuffer);

        }

        USBCAMD_SubmitIsoTransfer(DeviceExtension,
                                  transferExtension,
                                  ChannelExtension->CurrentUSBFrame,
                                  FALSE);

        //
        // BUGBUG
        // remove this when we have an api in the stack that lets
        // us sync two iso pipes.
        //
        ChannelExtension->CurrentUSBFrame +=
            USBCAMD_NUM_ISO_PACKETS_PER_REQUEST;

    }

    ChannelExtension->ImageCaptureStarted = TRUE;
}

//---------------------------------------------------------------------------
// USBCAMD_ControlVendorCommand
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_ControlVendorCommandWorker(
    IN PVOID DeviceContext,
    IN UCHAR Request,
    IN USHORT Value,
    IN USHORT Index,
    IN PVOID Buffer,
    IN OUT PULONG BufferLength,
    IN BOOLEAN GetData
    )
/*++

Routine Description:

    Send a vendor command to the camera to fetch data.

Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.

    Request - Request code for setup packet.

    Value - Value for setup packet.

    Index - Index for setup packet.

    Buffer - Pointer to input buffer

    BufferLength - pointer size of input/output buffer (optional)

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
    BOOLEAN allocated = FALSE;
    PUCHAR localBuffer;
    PUCHAR buffer;
    PURB urb;
    PUSBCAMD_DEVICE_EXTENSION deviceExtension;
    ULONG length = BufferLength ? *BufferLength : 0;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_ControlVendorCommand\n"));

    deviceExtension = USBCAMD_GET_DEVICE_EXTENSION(DeviceContext);

    buffer = USBCAMD_ExAllocatePool(NonPagedPool,
                            sizeof(struct
                            _URB_CONTROL_VENDOR_OR_CLASS_REQUEST) + length);


    if (buffer) {
        urb = (PURB) (buffer + length);

        USBCAMD_KdPrint (MIN_TRACE, ("'enter USBCAMD_ControlVendorCommand req %x val %x index %x\n",
            Request, Value, Index));

        if (BufferLength && *BufferLength != 0) {
            localBuffer = buffer;
            if (!GetData) {
                RtlCopyMemory(localBuffer, Buffer, *BufferLength);
            }    
        } else {
            localBuffer = NULL;
        }

        UsbBuildVendorRequest(urb,
                              URB_FUNCTION_VENDOR_DEVICE,
                              sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                              GetData ? USBD_TRANSFER_DIRECTION_IN : 
                                  0,
                              0,
                              Request,
                              Value,
                              Index,
                              localBuffer,
                              NULL,
                              length,
                              NULL);

        USBCAMD_KdPrint (MAX_TRACE, ("'BufferLength =  0x%x buffer = 0x%x\n", 
            length, localBuffer));

        ntStatus = USBCAMD_CallUSBD(deviceExtension, urb);

        if (NT_SUCCESS(ntStatus)) {
            if (BufferLength) {
                *BufferLength =
                    urb->UrbControlVendorClassRequest.TransferBufferLength;
                                
                USBCAMD_KdPrint (MAX_TRACE, ("'BufferLength =  0x%x buffer = 0x%x\n",
                    *BufferLength, localBuffer));
                if (localBuffer && GetData) {
                    RtlCopyMemory(Buffer, localBuffer, *BufferLength);
                }                
            }                
        }

        USBCAMD_ExFreePool(buffer);
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_ControlVendorCommand 0x%x\n", ntStatus));

    return ntStatus;

}


//---------------------------------------------------------------------------
// USBCAMD_ControlVendorCommand
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_ControlVendorCommand(
    IN PVOID DeviceContext,
    IN UCHAR Request,
    IN USHORT Value,
    IN USHORT Index,
    IN PVOID Buffer,
    IN OUT PULONG BufferLength,
    IN BOOLEAN GetData,
    IN PCOMMAND_COMPLETE_FUNCTION CommandComplete,
    IN PVOID CommandContext
    )
/*++

Routine Description:

    Send a vendor command to the camera to fetch data.

Arguments:

    DeviceExtension - pointer to the device extension for this instance of the USB camera
                    devcice.

    Request - Request code for setup packet.

    Value - Value for setup packet.

    Index - Index for setup packet.

    Buffer - Pointer to input buffer

    BufferLength - pointer size of input/output buffer (optional)

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
    PCOMMAND_WORK_ITEM workitem;

    USBCAMD_KdPrint (MAX_TRACE, ("'enter USBCAMD_ControlVendorCommand2\n"));

    if (KeGetCurrentIrql() < DISPATCH_LEVEL) {
        //
        // we are at passive level, just do the command
        //
        ntStatus = USBCAMD_ControlVendorCommandWorker(DeviceContext,
                                                Request,
                                                Value,
                                                Index,
                                                Buffer,
                                                BufferLength,
                                                GetData);

        if (CommandComplete) {
            // call the completion handler
            (*CommandComplete)(DeviceContext, CommandContext, ntStatus);
        }
        
    } else {
//        TEST_TRAP();
        //
        // schedule a work item
        //
        ntStatus = STATUS_PENDING;

        workitem = USBCAMD_ExAllocatePool(NonPagedPool,
                                          sizeof(COMMAND_WORK_ITEM));
        if (workitem) {
        
            ExInitializeWorkItem(&workitem->WorkItem,
                                 USBCAMD_CommandWorkItem,
                                 workitem);

            workitem->DeviceContext = DeviceContext;
            workitem->Request = Request;
            workitem->Value = Value;
            workitem->Index = Index;
            workitem->Buffer = Buffer;
            workitem->BufferLength = BufferLength;
            workitem->GetData = GetData;
            workitem->CommandComplete = CommandComplete;
            workitem->CommandContext = CommandContext;           

            ExQueueWorkItem(&workitem->WorkItem,
                            DelayedWorkQueue);
    
        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

    }

    return ntStatus;
}


VOID
USBCAMD_CommandWorkItem(
    PVOID Context
    )
/*++

Routine Description:

    Call the mini driver to convert a raw packet to the proper format.

Arguments:

Return Value:

    None.

--*/
{
    NTSTATUS ntStatus;
    PCOMMAND_WORK_ITEM workItem = Context;

    ntStatus = USBCAMD_ControlVendorCommandWorker(workItem->DeviceContext,
                                            workItem->Request,
                                            workItem->Value,
                                            workItem->Index,
                                            workItem->Buffer,
                                            workItem->BufferLength,
                                            workItem->GetData);

                                                
    if (workItem->CommandComplete) {
        // call the completion handler
        (*workItem->CommandComplete)(workItem->DeviceContext, 
                                   workItem->CommandContext, 
                                   ntStatus);
    }
        
    USBCAMD_ExFreePool(workItem);
}


NTSTATUS 
USBCAMD_GetRegistryKeyValue (
    IN HANDLE Handle,
    IN PWCHAR KeyNameString,
    IN ULONG KeyNameStringLength,
    IN PVOID Data,
    IN ULONG DataLength
    )
/*++

Routine Description:
    
Arguments:

Return Value:

--*/
{
    NTSTATUS ntStatus = STATUS_NO_MEMORY;
    UNICODE_STRING keyName;
    ULONG length;
    PKEY_VALUE_FULL_INFORMATION fullInfo;

    RtlInitUnicodeString(&keyName, KeyNameString);
    
    length = sizeof(KEY_VALUE_FULL_INFORMATION) + 
            KeyNameStringLength + DataLength;
            
    fullInfo = USBCAMD_ExAllocatePool(PagedPool, length); 
    USBCAMD_KdPrint(MAX_TRACE, ("' USBD_GetRegistryKeyValue buffer = 0x%x\n", (ULONG) fullInfo));  
    
    if (fullInfo) {        
        ntStatus = ZwQueryValueKey(Handle,
                        &keyName,
                        KeyValueFullInformation,
                        fullInfo,
                        length,
                        &length);
                        
        if (NT_SUCCESS(ntStatus)){
            ASSERT(DataLength == fullInfo->DataLength);                       
            RtlCopyMemory(Data, ((PUCHAR) fullInfo) + fullInfo->DataOffset, DataLength);
        }            

        USBCAMD_ExFreePool(fullInfo);
    }        
    
    return ntStatus;
}

#if DBG

typedef struct _RAW_SIG {
    ULONG Sig;
    ULONG length;
} RAW_SIG, *PRAW_SIG;    


PVOID 
USBCAMD_AllocateRawFrameBuffer(
    ULONG RawFrameLength
    )
/*++

Routine Description:
    
Arguments:

Return Value:

--*/
{
    PRAW_SIG rawsig;
    PUCHAR pch;

    pch = USBCAMD_ExAllocatePool(NonPagedPool,
                         RawFrameLength + sizeof(*rawsig)*2);

    if (pch) {
        // begin sig
        rawsig = (PRAW_SIG) pch;
        rawsig->Sig = USBCAMD_RAW_FRAME_SIG;
        rawsig->length = RawFrameLength;
        

        // end sig                    
        rawsig = (PRAW_SIG) (pch+RawFrameLength+sizeof(*rawsig));
        rawsig->Sig = USBCAMD_RAW_FRAME_SIG;
        rawsig->length = RawFrameLength;
       
        pch += sizeof(*rawsig);
    }

    return pch;
}


VOID 
USBCAMD_FreeRawFrameBuffer(
    PVOID RawFrameBuffer
    )
/*++

Routine Description:
    
Arguments:

Return Value:

--*/
{
    PUCHAR pch;

    USBCAMD_CheckRawFrameBuffer(RawFrameBuffer);

    pch = RawFrameBuffer;
    pch -= sizeof(RAW_SIG);

    USBCAMD_ExFreePool(pch);
}


VOID 
USBCAMD_CheckRawFrameBuffer(
    PVOID RawFrameBuffer
    )
/*++

Routine Description:
    
Arguments:

Return Value:

--*/
{
        
}

typedef struct _NODE_HEADER {
    ULONG Length;
    ULONG Sig;
} NODE_HEADER, *PNODE_HEADER;

PVOID
USBCAMD_ExAllocatePool(
    IN POOL_TYPE PoolType,
    IN ULONG NumberOfBytes
    )
{
    PNODE_HEADER tmp;
    
    tmp = ExAllocatePool(PoolType, NumberOfBytes+sizeof(*tmp));

    if (tmp) {
        USBCAMD_HeapCount += NumberOfBytes;
        tmp->Length = NumberOfBytes;
        tmp->Sig = 0xDEADBEEF;
        tmp++;
    }

    return tmp;
}


VOID
USBCAMD_ExFreePool(
    IN PVOID p
    )
{
    PNODE_HEADER tmp;

    tmp = p;
    tmp--;
    ASSERT(tmp->Sig == 0xDEADBEEF);
    tmp->Sig = 0;

    USBCAMD_HeapCount-=tmp->Length;
    
    ExFreePool(tmp);

}

#endif


//---------------------------------------------------------------------------
// USBCAMD_SetDevicePowerState
//---------------------------------------------------------------------------
NTSTATUS
USBCAMD_SetDevicePowerState(
    IN PUSBCAMD_DEVICE_EXTENSION DeviceExtension,
    IN PHW_STREAM_REQUEST_BLOCK Srb
    )
/*++

Routine Description:

Arguments:

    DeviceExtension - points to the driver specific DeviceExtension

    DevicePowerState - Device power state to enter.

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
	DEVICE_POWER_STATE DevicePowerState = Srb->CommandData.DeviceState;

    USBCAMD_SERIALIZE(DeviceExtension);
 
    USBCAMD_KdPrint (MIN_TRACE, ("'enter SetDevicePowerState\n"));
    LOGENTRY("SPst", 0, DeviceExtension, 0);

    if (DeviceExtension->CurrentPowerState != DevicePowerState) {

        switch (DevicePowerState) {
			case PowerDeviceD0:
				if ( DeviceExtension->CurrentPowerState == PowerDeviceD3 ) {
					USBCAMD_KdPrint (MIN_TRACE, ("USBCAMD: Switching from D3 to D0\n"));
//					TEST_TRAP();
					ntStatus = 
						(*DeviceExtension->DeviceData.CamRestoreState)(
						DeviceExtension->StackDeviceObject,      
						USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension));                
					if (NT_ERROR (ntStatus)) {
						USBCAMD_KdPrint (MIN_TRACE, ("USBCAMD: Cam Driver Failed to restore its state\n"));
					}
					ntStatus = STATUS_SUCCESS;
				}
				break;
			case PowerDeviceD1:
			case PowerDeviceD2:
				ntStatus = STATUS_SUCCESS;
				break;
			case PowerDeviceD3:
				
				if ( DeviceExtension->CurrentPowerState == PowerDeviceD0 ) {
					USBCAMD_KdPrint (MIN_TRACE, ("'USBCAMD: Switching from D0 to D3\n"));
//					TEST_TRAP();
					ntStatus = 
						(*DeviceExtension->DeviceData.CamSaveState)(
						DeviceExtension->StackDeviceObject,      
						USBCAMD_GET_DEVICE_CONTEXT(DeviceExtension));                
					if (NT_ERROR (ntStatus)) {
						USBCAMD_KdPrint (MIN_TRACE, ("'USBCAMD: Cam Driver Failed to save its state\n"));
					}
				}
				ntStatus = STATUS_SUCCESS;
				break;
			default:
					ntStatus = STATUS_INVALID_PARAMETER;
					break;
        }            
        if (NT_SUCCESS(ntStatus)) 
			DeviceExtension->CurrentPowerState = DevicePowerState;
    }
    
    USBCAMD_RELEASE(DeviceExtension);
    USBCAMD_KdPrint (MAX_TRACE, ("'exit USBCAMD_SetDevicePowerState 0x%x\n", ntStatus));

    return ntStatus;
}    
