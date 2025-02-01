/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    IsoPnp.c 

Abstract:

    Isochronous USB device driver for Intel 82930 USB test board
    Plug and Play module

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.


Revision History:

    2/7/98: created

--*/


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "Iso82930.h"



NTSTATUS
IsoUsb_ProcessPnPIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++

Routine Description:

    Dispatch table routine for IRP_MJ_PNP.
    Process the Plug and Play IRPs sent to this device.

Arguments:

    DeviceObject - pointer to our FDO (Functional Device Object)

    Irp          - pointer to an I/O Request Packet

Return Value:

    NT status code

--*/
{

    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    NTSTATUS waitStatus;
    PDEVICE_OBJECT stackDeviceObject;
    KEVENT startDeviceEvent;

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    //
    // Get a pointer to the current location in the Irp. This is where
    //     the function codes and parameters are located.
    //
    irpStack = IoGetCurrentIrpStackLocation (Irp);

    //
    // Get a pointer to the device extension
    //
    deviceExtension = DeviceObject->DeviceExtension;
    stackDeviceObject = deviceExtension->TopOfStackDeviceObject;

    ISOUSB_KdPrint( DBGLVL_MEDIUM, ( "enter IsoUsb_ProcessPnPIrp() IRP_MJ_PNP, minor %s\n",
        ISOUSB_StringForPnpMnFunc( irpStack->MinorFunction ) ));

	// inc the FDO device extension's pending IO count for this Irp
    IsoUsb_IncrementIoCount(DeviceObject);

    ISOUSB_ASSERT( IRP_MJ_PNP == irpStack->MajorFunction );

    switch (irpStack->MinorFunction) {
    case IRP_MN_START_DEVICE:

		// The PnP Manager sends this IRP after it has assigned resources, 
		// if any, to the device. The device may have been recently enumerated
		// and is being started for the first time, or the device may be 
		// restarting after being stopped for resource reconfiguration. 

		// Initialize an event we can wait on for the PDO to be done with this irp
        KeInitializeEvent(&startDeviceEvent, NotificationEvent, FALSE);
        IoCopyCurrentIrpStackLocationToNext(Irp);

        // Set a completion routine so it can signal our event when
        //  the PDO is done with the Irp
        IoSetCompletionRoutine(Irp,
                               IsoUsb_IrpCompletionRoutine,
                               &startDeviceEvent,  // pass the event to the completion routine as the Context
                               TRUE,    // invoke on success
                               TRUE,    // invoke on error
                               TRUE);   // invoke on cancellation


        // let the PDO process the IRP
        ntStatus = IoCallDriver(stackDeviceObject,
                                Irp);

        // if PDO is not done yet, wait for the event to be set in our completion routine
        if (ntStatus == STATUS_PENDING) {
             // wait for irp to complete

            waitStatus = KeWaitForSingleObject(
                &startDeviceEvent,
                Suspended,
                KernelMode,
                FALSE,
                NULL);
        }

        // Now we're ready to do our own startup processing.
        // USB client drivers such as us set up URBs (USB Request Packets) to send requests
        // to the host controller driver (HCD). The URB structure defines a format for all
        // possible commands that can be sent to a USB device.
        // Here, we request the device descriptor and store it,
        // and configure the device.
        ntStatus = IsoUsb_StartDevice(DeviceObject);
        Irp->IoStatus.Status = ntStatus;

		IoCompleteRequest (Irp,
						   IO_NO_INCREMENT
						   );

		IsoUsb_DecrementIoCount(DeviceObject);
		return ntStatus;  // end, case IRP_MN_START_DEVICE

    case IRP_MN_QUERY_STOP_DEVICE:

		// The IRP_MN_QUERY_STOP_DEVICE/IRP_MN_STOP_DEVICE sequence only occurs
		// during "polite" shutdowns, such as the user explicitily requesting the
		// service be stopped in, or requesting unplug from the Pnp tray icon.
		// This sequence is NOT received during "impolite" shutdowns,
		// such as someone suddenly yanking the USB cord or otherwise 
		// unexpectedly disabling/resetting the device.

        // If a driver sets STATUS_SUCCESS for this IRP,
        // the driver must not start any operations on the device that
        // would prevent that driver from successfully completing an IRP_MN_STOP_DEVICE
        // for the device.
        // For mass storage devices such as disk drives, while the device is in the
        // stop-pending state,the driver holds IRPs that require access to the device,
        // but for most USB devices, there is no 'persistent storage', so we will just
        // refuse any more IO until restarted or the stop is cancelled

        // If a driver in the device stack determines that the device cannot be
        // stopped for resource reconfiguration, the driver is not required to pass
        // the IRP down the device stack. If a query-stop IRP fails,
        // the PnP Manager sends an IRP_MN_CANCEL_STOP_DEVICE to the device stack,
        // notifying the drivers for the device that the query has been cancelled
        // and that the device will not be stopped.


        // It is possible to receive this irp when the device has not been started
        //  ( as on a boot device )
        if (!deviceExtension->DeviceStarted) { // if get when never started, just pass on
            ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPnPIrp() IRP_MN_QUERY_STOP_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            IsoUsb_DecrementIoCount(DeviceObject);

            return ntStatus;
        }


        // fail the request if we have any read/write IRPS pending
        if( deviceExtension->PendingIoCount > 1 ) {
            ntStatus = STATUS_UNSUCCESSFUL;
        }
        else { 
			// We'll not veto it; pass it on and flag that stop was requested.
			// Once StopDeviceRequested is set no new IOCTL or read/write irps will be passed
			// down the stack to lower drivers; all will be quickly failed
             deviceExtension->StopDeviceRequested = TRUE;
        }

        break; // end, case IRP_MN_QUERY_STOP_DEVICE

    case IRP_MN_CANCEL_STOP_DEVICE:

		// The PnP Manager uses this IRP to inform the drivers for a device
		// that the device will not be stopped for resource reconfiguration.
		// This should only be received after a successful IRP_MN_QUERY_STOP_DEVICE.


        // It is possible to receive this irp when the device has not been started
        if (!deviceExtension->DeviceStarted) { // if get when never started, just pass on
            ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPnPIrp() IRP_MN_CANCEL_STOP_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            IsoUsb_DecrementIoCount(DeviceObject);
            return ntStatus;
        }

		// Reset this flag so new IOCTL and IO Irp processing will be re-enabled
        deviceExtension->StopDeviceRequested = FALSE;
        break; // end, case IRP_MN_CANCEL_STOP_DEVICE

    case IRP_MN_STOP_DEVICE:

		// The PnP Manager sends this IRP to stop a device so it can reconfigure
		// its hardware resources. The PnP Manager only sends this IRP if a prior
		// IRP_MN_QUERY_STOP_DEVICE completed successfully.


        //
        // Send the select configuration urb with a NULL pointer for the configuration
        // handle, this closes the configuration and puts the device in the 'unconfigured'
        // state.
        //
        ntStatus = IsoUsb_StopDevice(DeviceObject);

        break; // end, case IRP_MN_STOP_DEVICE
 

    case IRP_MN_QUERY_REMOVE_DEVICE:

        //  In response to this IRP, drivers indicate whether the device can be
        //  removed without disrupting the system.
        //  If a driver determines it is safe to remove the device,
        //  the driver completes any outstanding I/O requests, arranges to hold any subsequent
        //  read/write requests, and sets Irp->IoStatus.Status to STATUS_SUCCESS. Function
        //  and filter drivers then pass the IRP to the next-lower driver in the device stack.
        //  The underlying bus driver calls IoCompleteRequest.

        //  If a driver sets STATUS_SUCCESS for this IRP, the driver must not start any
        //  operations on the device that would prevent that driver from successfully completing
        //  an IRP_MN_REMOVE_DEVICE for the device. If a driver in the device stack determines
        //  that the device cannot be removed, the driver is not required to pass the
        //  query-remove IRP down the device stack. If a query-remove IRP fails, the PnP Manager
        //  sends an IRP_MN_CANCEL_REMOVE_DEVICE to the device stack, notifying the drivers for
        //  the device that the query has been cancelled and that the device will not be removed.

        // It is possible to receive this irp when the device has not been started
        if (!deviceExtension->DeviceStarted) { // if get when never started, just pass on
            ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPnPIrp() IRP_MN_QUERY_STOP_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            IsoUsb_DecrementIoCount(DeviceObject);

            return ntStatus;
        }

 		// Once RemoveDeviceRequested is set no new IOCTL or read/write irps will be passed
		// down the stack to lower drivers; all will be quickly failed
       deviceExtension->RemoveDeviceRequested = TRUE;

        // Wait for any io request pending in our driver to
        // complete before returning success.
		// This  event is set when deviceExtension->PendingIoCount goes to 1
        waitStatus = KeWaitForSingleObject(
                    &deviceExtension->NoPendingIoEvent,
                    Suspended,
                    KernelMode,
                    FALSE,
                    NULL);


        break; // end, case IRP_MN_QUERY_REMOVE_DEVICE

    case IRP_MN_CANCEL_REMOVE_DEVICE:

		// The PnP Manager uses this IRP to inform the drivers
		// for a device that the device will not be removed.
		// It is sent only after a successful IRP_MN_QUERY_REMOVE_DEVICE.

        if (!deviceExtension->DeviceStarted) { // if get when never started, just pass on
            ISOUSB_KdPrint( DBGLVL_MEDIUM,("IsoUsb_ProcessPnPIrp() IRP_MN_CANCEL_REMOVE_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            IsoUsb_DecrementIoCount(DeviceObject);
            return ntStatus;
        }

		// Reset this flag so new IOCTL and IO Irp processing will be re-enabled
        deviceExtension->RemoveDeviceRequested = FALSE;

        break; // end, case IRP_MN_CANCEL_REMOVE_DEVICE

    case IRP_MN_REMOVE_DEVICE:

		// The PnP Manager uses this IRP to direct drivers to remove a device. 
		// For a "polite" device removal, the PnP Manager sends an 
		// IRP_MN_QUERY_REMOVE_DEVICE prior to the remove IRP. In this case, 
		// the device is in the remove-pending state when the remove IRP arrives.
		// For a surprise-style device removal ( i.e. sudden cord yank ), 
		// the physical device has already been removed so the PnP Manager sends 
		// the remove IRP without a prior query-remove. A device can be in any state
		// when it receives a remove IRP as a result of a surprise-style removal.

        // match the inc at the begining of the dispatch routine
        IsoUsb_DecrementIoCount(DeviceObject);

		//
        // Once DeviceRemoved is set no new IOCTL or read/write irps will be passed
        // down the stack to lower drivers; all will be quickly failed
		//
        deviceExtension->DeviceRemoved = TRUE;

		// If any pipes are still open, call USBD with URB_FUNCTION_ABORT_PIPE
		// This call will also close the pipes; if any user close calls get through,
		// they will be noops
		IsoUsb_AbortPipes( DeviceObject );




		// We don't explicitly wait for the below driver to complete, but just make
		// the call and go on, finishing cleanup
        IoCopyCurrentIrpStackLocationToNext(Irp);

        ntStatus = IoCallDriver(stackDeviceObject,
                                Irp);
        //
        // The final decrement to device extension PendingIoCount == 0
		// will set deviceExtension->RemoveEvent, enabling device removal.
		// If there is no pending IO at this point, the below decrement will be it.
        //
        IsoUsb_DecrementIoCount(DeviceObject);


        // wait for any io request pending in our driver to
        // complete for finishing the remove

        KeWaitForSingleObject(
                    &deviceExtension->RemoveEvent,
                    Suspended,
                    KernelMode,
                    FALSE,
                    NULL);

        //
        // Delete the link and FDO we created
        //
        IsoUsb_RemoveDevice(DeviceObject);

        ISOUSB_KdPrint( DBGLVL_DEFAULT,("IsoUsb_ProcessPnPIrp() Detaching from %08X\n",
                         deviceExtension->TopOfStackDeviceObject));

        IoDetachDevice(deviceExtension->TopOfStackDeviceObject);

        ISOUSB_KdPrint( DBGLVL_DEFAULT,("IsoUsb_ProcessPnPIrp() Deleting %08X\n",
                         DeviceObject));

        IoDeleteDevice (DeviceObject);

        return ntStatus; // end, case IRP_MN_REMOVE_DEVICE


    default:
        ISOUSB_KdPrint( DBGLVL_MAXIMUM,("IsoUsb_ProcessPnPIrp() Minor PnP IOCTL not handled\n"));
    } /* case MinorFunction  */


    if (!NT_SUCCESS(ntStatus)) {

		// if anything went wrong, return failure  without passing Irp down
        Irp->IoStatus.Status = ntStatus;
		IoCompleteRequest (Irp,
						   IO_NO_INCREMENT
						   );

		IsoUsb_DecrementIoCount(DeviceObject);

		ISOUSB_KdPrint( DBGLVL_MAXIMUM,("IsoUsb_ProcessPnPIrp() Exit IsoUsb_ProcessPnPIrp FAILURE %x\n", ntStatus));
		return ntStatus;
    }

    IoCopyCurrentIrpStackLocationToNext(Irp);

    //
    // All PNP_POWER messages get passed to the TopOfStackDeviceObject
    // we were given in PnPAddDevice
    //

    ISOUSB_KdPrint( DBGLVL_MAXIMUM,("IsoUsb_ProcessPnPIrp() Passing PnP Irp down, status = %x\n", ntStatus));

    ntStatus = IoCallDriver(stackDeviceObject,
                            Irp);

    IsoUsb_DecrementIoCount(DeviceObject);

    ISOUSB_KdPrint( DBGLVL_MAXIMUM,("IsoUsb_ProcessPnPIrp() Exit IsoUsb_ProcessPnPIrp %x\n", ntStatus));

    return ntStatus;
}


NTSTATUS
IsoUsb_PnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Routine Description:

    This routine is called to create and initialize our Functional Device Object (FDO).
    For monolithic drivers, this is done in DriverEntry(), but Plug and Play devices
    wait for a PnP event

Arguments:

    DriverObject - pointer to the driver object for this instance of IsoUsb

    PhysicalDeviceObject - pointer to a device object created by the bus

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS                ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT          deviceObject = NULL;
    PDEVICE_EXTENSION       deviceExtension;
    USBD_VERSION_INFORMATION versionInformation;
    ULONG i;

    
    ISOUSB_KdPrint( DBGLVL_DEFAULT,("enter IsoUsb_PnPAddDevice()\n"));

    //
    // create our funtional device object (FDO)
    //

    ntStatus =
        IsoUsb_CreateDeviceObject(DriverObject, PhysicalDeviceObject, &deviceObject);

    if (NT_SUCCESS(ntStatus)) {
        deviceExtension = deviceObject->DeviceExtension;

        deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

        //
        // we support direct io for read/write
        //
        deviceObject->Flags |= DO_DIRECT_IO;


        //Set this flag causes the driver to not receive a IRP_MN_STOP_DEVICE
        //during suspend and also not get an IRP_MN_START_DEVICE during resume.  
        //This is neccesary because during the start device call, 
        // the GetDescriptors() call  will be failed by the USB stack.
        deviceObject->Flags |= DO_POWER_PAGABLE;

        // initialize our device extension
        //
        // remember the Physical device Object
        //
        deviceExtension->PhysicalDeviceObject=PhysicalDeviceObject;

        //
        // Attach to the PDO
        //

        deviceExtension->TopOfStackDeviceObject =
            IoAttachDeviceToDeviceStack(deviceObject, PhysicalDeviceObject);

        // Get a copy of the physical device's capabilities into a
        // DEVICE_CAPABILITIES struct in our device extension;
        // We are most interested in learning which system power states
        // are to be mapped to which device power states for handling
        // IRP_MJ_SET_POWER Irps.
        IsoUsb_QueryCapabilities(PhysicalDeviceObject,
                                 &deviceExtension->DeviceCapabilities);


		// We want to determine what level to auto-powerdown to; This is the lowest
		//  sleeping level that is LESS than D3; 
		// If all are set to D3, auto powerdown/powerup will be disabled.

        deviceExtension->PowerDownLevel = PowerDeviceUnspecified; // init to disabled
        for (i=PowerSystemSleeping1; i<= PowerSystemSleeping3; i++) {

			if ( deviceExtension->DeviceCapabilities.DeviceState[i] < PowerDeviceD3 )
				deviceExtension->PowerDownLevel = deviceExtension->DeviceCapabilities.DeviceState[i];
        }

#if DBG

        // May want override auto power-down level from registry;
        // ( CurrentControlSet\Services\IsoUsb\Parameters )
		// Setting to 0 or 1 in registry disables auto power-down
        IsoUsb_GetRegistryDword( ISOUSB_REGISTRY_PARAMETERS_PATH,
                                         L"PowerDownLevel",
                                         &(deviceExtension->PowerDownLevel) );



        //
        // display the device  caps
        //

        ISOUSB_KdPrint( DBGLVL_MEDIUM,(" >>>>>> DeviceCaps\n"));
        ISOUSB_KdPrint( DBGLVL_MEDIUM,(" SystemWake = %s\n",
            ISOUSB_StringForSysState( deviceExtension->DeviceCapabilities.SystemWake ) ));
        ISOUSB_KdPrint( DBGLVL_MEDIUM,(" DeviceWake = %s\n",
            ISOUSB_StringForDevState( deviceExtension->DeviceCapabilities.DeviceWake) ));

        for (i=PowerSystemUnspecified; i< PowerSystemMaximum; i++) {

            ISOUSB_KdPrint( DBGLVL_MEDIUM,(" Device State Map: sysstate %s = devstate %s\n",
                 ISOUSB_StringForSysState( i ),
                 ISOUSB_StringForDevState( deviceExtension->DeviceCapabilities.DeviceState[i] ) ));
        }
        ISOUSB_KdPrint( DBGLVL_MEDIUM,(" <<<<<<<<DeviceCaps\n"));
#endif
        // We keep a pending IO count ( extension->PendingIoCount )  in the device extension.
        // The first increment of this count is done on adding the device.
        // Subsequently, the count is incremented for each new IRP received and
        // decremented when each IRP is completed or passed on.

        // Transition to 'one' therefore indicates no IO is pending and signals
        // deviceExtension->NoPendingIoEvent. This is needed for processing
        // IRP_MN_QUERY_REMOVE_DEVICE

        // Transition to 'zero' signals an event ( deviceExtension->RemoveEvent )
        // to enable device removal. This is used in processing for IRP_MN_REMOVE_DEVICE
        //
        IsoUsb_IncrementIoCount(deviceObject);

    }

    USBD_GetUSBDIVersion(&versionInformation);



    if( NT_SUCCESS( ntStatus ) )  
    {
        NTSTATUS actStat;
        // try to power down device until IO actually requested
        actStat = IsoUsb_SelfSuspendOrActivate( deviceObject, TRUE );
    }


    ISOUSB_KdPrint( DBGLVL_DEFAULT,("exit IsoUsb_PnPAddDevice() (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
IsoUsb_StartDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    Called from IsoUsb_ProcessPnPIrp(), the dispatch routine for IRP_MJ_PNP.
    Initializes a given instance of the device on the USB.
    USB client drivers such as us set up URBs (USB Request Packets) to send requests
    to the host controller driver (HCD). The URB structure defines a format for all
    possible commands that can be sent to a USB device.
    Here, we request the device descriptor and store it, and configure the device.


Arguments:

    DeviceObject - pointer to the FDO (Functional Device Object)

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PUSB_DEVICE_DESCRIPTOR deviceDescriptor = NULL;
    PURB urb;
    ULONG siz;

    ISOUSB_KdPrint( DBGLVL_DEFAULT,("enter IsoUsb_StartDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    urb = ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    ISOUSB_KdPrintCond( DBGLVL_DEFAULT,!urb, ("IsoUsb_StartDevice() FAILED ExAllocatePool() for URB\n"));

    if (urb) {

        siz = sizeof(USB_DEVICE_DESCRIPTOR);

        deviceDescriptor = ExAllocatePool(NonPagedPool,
                                          siz);

        ISOUSB_KdPrintCond( DBGLVL_DEFAULT, !deviceDescriptor, ("IsoUsb_StartDevice() FAILED ExAllocatePool() for deviceDescriptor\n"));

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


            ntStatus = IsoUsb_CallUSBD(DeviceObject, urb);

            ISOUSB_KdPrintCond( DBGLVL_DEFAULT, !NT_SUCCESS(ntStatus), ("IsoUsb_StartDevice() FAILED IsoUsb_CallUSBD(DeviceObject, urb)\n"));

            if (NT_SUCCESS(ntStatus)) {
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("Device Descriptor = %x, len %x\n",
                                deviceDescriptor,
                                urb->UrbControlDescriptorRequest.TransferBufferLength));

                ISOUSB_KdPrint( DBGLVL_MEDIUM,("I82930 Device Descriptor:\n"));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("-------------------------\n"));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("bLength %d\n", deviceDescriptor->bLength));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("bDescriptorType 0x%x\n", deviceDescriptor->bDescriptorType));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("bcdUSB 0x%x\n", deviceDescriptor->bcdUSB));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("bDeviceClass 0x%x\n", deviceDescriptor->bDeviceClass));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("bDeviceSubClass 0x%x\n", deviceDescriptor->bDeviceSubClass));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("bDeviceProtocol 0x%x\n", deviceDescriptor->bDeviceProtocol));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("bMaxPacketSize0 0x%x\n", deviceDescriptor->bMaxPacketSize0));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("idVendor 0x%x\n", deviceDescriptor->idVendor));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("idProduct 0x%x\n", deviceDescriptor->idProduct));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("bcdDevice 0x%x\n", deviceDescriptor->bcdDevice));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("iManufacturer 0x%x\n", deviceDescriptor->iManufacturer));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("iProduct 0x%x\n", deviceDescriptor->iProduct));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("iSerialNumber 0x%x\n", deviceDescriptor->iSerialNumber));
                ISOUSB_KdPrint( DBGLVL_MEDIUM,("bNumConfigurations 0x%x\n", deviceDescriptor->bNumConfigurations));
            }
        } else {
			// if we got here we failed to allocate deviceDescriptor
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

        if (NT_SUCCESS(ntStatus)) {
            deviceExtension->UsbDeviceDescriptor = deviceDescriptor;
        } else if (deviceDescriptor) {
            ExFreePool(deviceDescriptor);
        }

        ExFreePool(urb);

    } else {
		// if we got here we failed to allocate the urb
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (NT_SUCCESS(ntStatus)) {
        ntStatus = IsoUsb_ConfigureDevice(DeviceObject);

        ISOUSB_KdPrintCond( DBGLVL_MEDIUM,!NT_SUCCESS(ntStatus),("IsoUsb_StartDevice IsoUsb_ConfigureDevice() FAILURE (%x)\n", ntStatus));
    }


    if (NT_SUCCESS(ntStatus)) {
        deviceExtension->DeviceStarted = TRUE;
    }
    ISOUSB_KdPrint( DBGLVL_DEFAULT, ("exit IsoUsb_StartDevice (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
IsoUsb_RemoveDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

	Called from IsoUsb_ProcessPnPIrp() to
    clean up our device instance's allocated buffers; free symbolic links

Arguments:

    DeviceObject - pointer to the FDO

Return Value:

    NT status code from free symbolic link operation

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    UNICODE_STRING deviceLinkUnicodeString;

    ISOUSB_KdPrint( DBGLVL_DEFAULT,("enter IsoUsb_RemoveDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    RtlInitUnicodeString (&deviceLinkUnicodeString,
                          deviceExtension->DeviceLinkNameBuffer);

    // remove the GUID-based symbolic link
    ntStatus = IoSetDeviceInterfaceState(&deviceLinkUnicodeString, FALSE);
    ISOUSB_ASSERT( NT_SUCCESS( ntStatus ) );

    //
    // Free device descriptor structure
    //

    if (deviceExtension->UsbDeviceDescriptor) {
        ExFreePool(deviceExtension->UsbDeviceDescriptor);
    }

    //
    // Free up the UsbInterface structure
    //
    if (deviceExtension->UsbInterface) {
        ExFreePool(deviceExtension->UsbInterface);
    }

	// free up the USB config discriptor
    if (deviceExtension->UsbConfigurationDescriptor) {
        ExFreePool(deviceExtension->UsbConfigurationDescriptor);
    }


    ISOUSB_KdPrint( DBGLVL_DEFAULT,("exit IsoUsb_RemoveDevice() status = 0x%x\n", ntStatus ));

    return ntStatus;
}




NTSTATUS
IsoUsb_StopDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    Stops a given instance of a 82930 device on the USB.
    We basically just tell USB this device is now 'unconfigured'

Arguments:

    DeviceObject - pointer to the device object for this instance of a 82930

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb;
    ULONG siz;

    ISOUSB_KdPrint( DBGLVL_DEFAULT,("enter IsoUsb_StopDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Send the select configuration urb with a NULL pointer for the configuration
    // handle. This closes the configuration and puts the device in the 'unconfigured'
    // state.
    //

    siz = sizeof(struct _URB_SELECT_CONFIGURATION);

    urb = ExAllocatePool(NonPagedPool,
                         siz);

    if (urb) {
        UsbBuildSelectConfigurationRequest(urb,
                                          (USHORT) siz,
                                          NULL);

        ntStatus = IsoUsb_CallUSBD(DeviceObject, urb);

        ISOUSB_KdPrintCond( DBGLVL_DEFAULT,!NT_SUCCESS(ntStatus),("IsoUsb_StopDevice() FAILURE Configuration Closed status = %x usb status = %x.\n", ntStatus, urb->UrbHeader.Status));

        ExFreePool(urb);
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }


    if (NT_SUCCESS(ntStatus)) {
        deviceExtension->DeviceStarted = FALSE;
    }

    deviceExtension->StopDeviceRequested = FALSE;

    ISOUSB_KdPrint( DBGLVL_DEFAULT,("exit IsoUsb_StopDevice() (%x)\n", ntStatus));

    return ntStatus;
}



NTSTATUS
IsoUsb_IrpCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:

    Used as a  general purpose completion routine so it can signal an event,
    passed as the Context, when the next lower driver is done with the input Irp.
	This routine is used by both PnP and Power Management logic.

    Even though this routine does nothing but set an event, it must be defined and
    prototyped as a completetion routine for use as such


Arguments:

    DeviceObject - Pointer to the device object for the class device.

    Irp - Irp completed.

    Context - Driver defined context, in this case a pointer to an event.

Return Value:

    The function value is the final status from the operation.

--*/
{
    PKEVENT event = Context;

    // Set the input event
    KeSetEvent(event,
               1,       // Priority increment  for waiting thread.
               FALSE);  // Flag this call is not immediately followed by wait.

    // This routine must return STATUS_MORE_PROCESSING_REQUIRED because we have not yet called
    // IoFreeIrp() on this IRP.
    return STATUS_MORE_PROCESSING_REQUIRED;

}

