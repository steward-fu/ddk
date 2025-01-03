
/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Toaster.c

Abstract:

    Device driver to control the toaster device.


Environment:

    Kernel mode


Revision History:

    Eliyas Yakub Oct 6 1998

    Fixed cancel bug - 04/11/2000

--*/
#include "toaster.h"
#include <initguid.h>
#include "..\inc\driver.h"

NTSTATUS
ToasterGetStandardInterface(
    IN PDEVICE_OBJECT Pdo,
    OUT PTOASTER_INTERFACE_STANDARD BusInterface
    );

//
// Global debug error level
//

ULONG DebugLevel=3;

GLOBALS Globals;

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, ToasterAddDevice)
#pragma alloc_text (PAGE, ToasterCreate)
#pragma alloc_text (PAGE, ToasterClose)
#pragma alloc_text (PAGE, ToasterDispatchIoctl)
#pragma alloc_text (PAGE, ToasterReadWrite)
#pragma alloc_text (PAGE, ToasterDispatchPnp)
#pragma alloc_text (PAGE, ToasterStartDevice)
#pragma alloc_text (PAGE, ToasterUnload)
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object

    RegistryPath - pointer to a unicode string representing the path,
                   to driver-specific key in the registry.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;

    ToasterDebugPrint ((3, "Entered DriverEntry\n"));

    //
    // Save the RegistryPath.
    //

    Globals.RegistryPath.MaximumLength = RegistryPath->Length +
                                          sizeof(UNICODE_NULL);
    Globals.RegistryPath.Length = RegistryPath->Length;
    Globals.RegistryPath.Buffer = ExAllocatePoolWithTag (
                                       PagedPool,
                                       Globals.RegistryPath.MaximumLength,
                                       TOASTER_POOL_TAG);

    if (!Globals.RegistryPath.Buffer) {

        ToasterDebugPrint ((0,
                "Couldn't allocate pool for registry path."));

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyUnicodeString(&Globals.RegistryPath, RegistryPath);

    DriverObject->MajorFunction[IRP_MJ_PNP]            = ToasterDispatchPnp;
    DriverObject->MajorFunction[IRP_MJ_POWER]          = ToasterDispatchPower;
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = ToasterCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = ToasterClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ToasterDispatchIoctl;
    DriverObject->MajorFunction[IRP_MJ_READ]           = ToasterReadWrite;

    DriverObject->MajorFunction[IRP_MJ_CLEANUP]        = ToasterCleanup;

    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = ToasterSystemControl;
    DriverObject->DriverExtension->AddDevice           = ToasterAddDevice;
    DriverObject->DriverUnload                         = ToasterUnload;

    return status;
}


NTSTATUS
ToasterAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Routine Description:

    The Plug & Play subsystem is handing us a brand new PDO, for which we
    (by means of INF registration) have been asked to provide a driver.

    We need to determine if we need to be in the driver stack for the device.
    Create a function device object to attach to the stack
    Initialize that device object
    Return status success.

    Remember: We can NOT actually send ANY non-pnp IRPS to the given driver
    stack, UNTIL we have received an IRP_MN_START_DEVICE.

Arguments:

    DeviceObject - pointer to a device object.

    PhysicalDeviceObject -  pointer to a device object created by the
                            underlying bus driver.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                status = STATUS_SUCCESS;
    PDEVICE_OBJECT          deviceObject = NULL;
    PFDO_DATA               fdoData;
    POWER_STATE             powerState;

    PAGED_CODE();

    ToasterDebugPrint ((2, "AddDevice PDO (0x%x)\n", PhysicalDeviceObject));

    //
    // Create a function device object.
    //

    status = IoCreateDevice (DriverObject,
                             sizeof (FDO_DATA),
                             NULL,  // No Name
                             FILE_DEVICE_UNKNOWN,
                             FILE_DEVICE_SECURE_OPEN,
                             FALSE,
                             &deviceObject);


    if (!NT_SUCCESS (status)) {
        //
        // returning failure here prevents the entire stack from functioning,
        // but most likely the rest of the stack will not be able to create
        // device objects either, so it is still OK.
        //
        return status;
    }

    ToasterDebugPrint ((2, "AddDevice FDO (0x%x)\n", deviceObject));

    //
    // Initialize the device extension.
    //

    fdoData = (PFDO_DATA) deviceObject->DeviceExtension;

    //
    // Set the initial state of the FDO
    //

    INITIALIZE_PNP_STATE(fdoData);

    //
    // This field is set to TRUE in response to an
    // IOCTL_TOASTER_DONT_DISPLAY_IN_UI_DEVICE,
    //

    fdoData->DontDisplayInUI = FALSE;

    fdoData->UnderlyingPDO = PhysicalDeviceObject;

    //
    // We will hold all requests until we are started.
    // On W2K we will not get any I/O until the entire device
    // is started. On Win9x this may be required.
    //

    fdoData->QueueState = HoldRequests;

    fdoData->Self = deviceObject;
    fdoData->NextLowerDriver = NULL;

    InitializeListHead(&fdoData->NewRequestsQueue);
    KeInitializeSpinLock(&fdoData->QueueLock);

    //
    // Initialize the remove event to Not-Signaled
    //

    KeInitializeEvent(&fdoData->RemoveEvent,
                      SynchronizationEvent,
                      FALSE);
    //
    // Initialize the stop event to Signaled:
    // there are no Irps that prevent the device from being
    // stopped. This event will be set when the OutstandingIO
    // will become 1.
    //

    KeInitializeEvent(&fdoData->StopEvent,
                      SynchronizationEvent,
                      TRUE);



    fdoData->OutstandingIO = 1; // biased to 1.  Transition to zero during
                                // remove device means IO is finished.
                                // Transition to 1 means the device can be
                                // stopped.

    //
    // Register with WMI
    //

    status = ToasterWmiRegistration(fdoData);
    if (!NT_SUCCESS (status)) {
        ToasterDebugPrint ((0,
            "AddDevice: IoWMIRegistrationControl failed (%x)\n", status));
        IoDeleteDevice (deviceObject);
        return status;
    }

    deviceObject->Flags |= DO_POWER_PAGABLE;
    deviceObject->Flags |= DO_BUFFERED_IO;


    //
    // Typically, the function driver for a device is its
    // power policy owner, although for some devices another
    // driver or system component may assume this role.
    // Set the initial power state of the device, if known, by calling
    // PoSetPowerState.
    //

    fdoData->DevicePowerState = PowerDeviceD3;
    fdoData->SystemPowerState = PowerSystemWorking;

    powerState.DeviceState = PowerDeviceD3;
    PoSetPowerState ( deviceObject, DevicePowerState, powerState );


    //
    // Attach our driver to the device stack.
    // The return value of IoAttachDeviceToDeviceStack is the top of the
    // attachment chain.  This is where all the IRPs should be routed.
    //

    fdoData->NextLowerDriver = IoAttachDeviceToDeviceStack (deviceObject,
                                                       PhysicalDeviceObject);
    if(NULL == fdoData->NextLowerDriver) {

        IoDeleteDevice(deviceObject);
        return STATUS_NO_SUCH_DEVICE;
    }

    //
    // Tell the Plug & Play system that this device will need an interface
    //

    status = IoRegisterDeviceInterface (
                PhysicalDeviceObject,
                (LPGUID) &GUID_DEVINTERFACE_TOASTER,
                NULL,
                &fdoData->InterfaceName);

    if (!NT_SUCCESS (status)) {
        ToasterDebugPrint ((0,
            "AddDevice: IoRegisterDeviceInterface failed (%x)\n", status));
        ToasterWmiDeRegistration(fdoData);
        IoDetachDevice(fdoData->NextLowerDriver);
        IoDeleteDevice (deviceObject);
        return status;
    }

    //
    // Clear the DO_DEVICE_INITIALIZING flag.
    // Note: Do not clear this flag until the driver has set the
    // device power state and the power DO flags.
    //

    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return status;

}

NTSTATUS
ToasterDispatchPnp (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    The plug and play dispatch routines.

    Most of these requests the driver will completely ignore.
    In all cases it must pass on the IRP to the lower driver.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

      NT status code

--*/
{
    PFDO_DATA               fdoData;
    PIO_STACK_LOCATION      stack;
    NTSTATUS                status = STATUS_SUCCESS;
    PDEVICE_CAPABILITIES    deviceCapabilities;
    ULONG                   requestCount;
    PPNP_DEVICE_STATE       deviceState;

    PAGED_CODE ();

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    stack = IoGetCurrentIrpStackLocation (Irp);

    ToasterDebugPrint((2, "FDO %s \n",
                PnPMinorFunctionString(stack->MinorFunction)));
    ToasterIoIncrement (fdoData);

    if (Deleted == fdoData->DevicePnPState) {

        //
        // Since the device is removed, we will not hold any IRPs.
        // We just fail it.
        //
        Irp->IoStatus.Status = STATUS_DELETE_PENDING;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        ToasterIoDecrement(fdoData);
        return STATUS_DELETE_PENDING;
    }

    switch (stack->MinorFunction) {
    case IRP_MN_START_DEVICE:

        //
        // The device is starting.
        //
        // We cannot touch the device (send it any non pnp irps) until a
        // start device has been passed down to the lower drivers.
        // First pass the IRP down.
        //

        status = ToasterSendIrpSynchronously(fdoData->NextLowerDriver, Irp);
        if (NT_SUCCESS (status)) {
            //
            // Lower drivers have finished their start operation, so now
            // we can finish ours.
            //
            status = ToasterStartDevice (fdoData, Irp);
        }

        //
        // We must now complete the IRP, since we stopped it in the
        // completion routine with MORE_PROCESSING_REQUIRED.
        //
        break;


    case IRP_MN_QUERY_STOP_DEVICE:

        //
        // If we can stop the device, we need to set the QueueState to
        // HoldRequests so further requests will be queued.
        //

        status = ToasterCanStopDevice(DeviceObject, Irp);

        if (NT_SUCCESS(status)) {

            //
            // OK, we can stop our device.
            // First, don't allow any requests to be passed down.
            //

            SET_NEW_PNP_STATE(fdoData, StopPending);
            fdoData->QueueState = HoldRequests;

            ToasterDebugPrint((3, "Holding requests...\n"));

            //
            // Then, wait for the existing ones to be finished
            // (since we expect to give up our resources, we
            // can't allow such requests). First, we need to decrement
            // this very operation (and to keep in mind not to decrement
            // after the wait).
            //

            ToasterIoDecrement(fdoData);

            KeWaitForSingleObject(
               &fdoData->StopEvent,
               Executive, // Waiting for reason of a driver
               KernelMode, // Waiting in kernel mode
               FALSE, // No alert
               NULL); // No timeout


            //
            // We must set Irp->IoStatus.Status to STATUS_SUCCESS before
            // passing it down.
            //
            Irp->IoStatus.Status = STATUS_SUCCESS;
            IoSkipCurrentIrpStackLocation (Irp);
            status = IoCallDriver (fdoData->NextLowerDriver, Irp);

            return status;
        }

        break;

   case IRP_MN_CANCEL_STOP_DEVICE:

        //
        // Send this IRP down and wait for it to come back.
        // Set the QueueState flag to AllowRequests,
        // and process all the previously queued up IRPs.
        //

        //
        // First check to see whether you have received cancel-stop
        // without first receiving a query-stop. This could happen if someone
        // above us fails a query-stop and passes down the subsequent
        // cancel-stop.
        //

        if(StopPending == fdoData->DevicePnPState) {

            status = ToasterSendIrpSynchronously(fdoData->NextLowerDriver,Irp);
            if(NT_SUCCESS(status))
            {

                fdoData->QueueState = AllowRequests;

                RESTORE_PREVIOUS_PNP_STATE(fdoData);

                ASSERT(fdoData->DevicePnPState == Started);
                //
                // Process the queued requests
                //

                ToasterProcessQueuedRequests(fdoData);
            } else {
                //
                // Somebody below us failed cancel-stop.
                // This is a fatal error.
                //
                ASSERTMSG("Cancel stop failed. Fatal error!", FALSE);
                ToasterDebugPrint((3, "Failure statute = 0x%x\n", status));
            }
        } else {
            //
            // Spurious cancel-stop so let us just complete this here.
            //
            status = STATUS_SUCCESS;
        }
        break;


    case IRP_MN_STOP_DEVICE:

        //
        // After the stop IRP has been sent to the lower driver object, the
        // driver must not send any more IRPs down that touch the device until
        // another START has occurred.  For this reason we are holding IRPs
        // and waiting for the outstanding requests to be finished when
        // QUERY_STOP is received.
        // IRP_MN_STOP_DEVICE doesn't change anything in this behavior
        // (we continue to hold IRPs until a IRP_MN_START_DEVICE is issued).
        //

        //
        // This is the right place to actually give up all the resources used
        // This might include calls to IoDisconnectInterrupt, MmUnmapIoSpace,
        // etc.
        //

        status = ToasterReturnResources(DeviceObject);

        //
        // Mark the device as stopped.
        //

        SET_NEW_PNP_STATE(fdoData, Stopped);


        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (fdoData->NextLowerDriver, Irp);

        ToasterIoDecrement(fdoData);

        return status;

    case IRP_MN_QUERY_REMOVE_DEVICE:

        //
        // If we can allow removal of the device, we should set the QueueState
        // to HoldRequests so further requests will be queued. This is required
        // so that we can process queued up requests in cancel-remove just in
        // case somebody else in the stack fails the query-remove.
        //

        status = ToasterCanRemoveDevice(DeviceObject, Irp);

        if (NT_SUCCESS(status)) {

            //
            // Now prepare to hold the new ones (eventually we might
            // get a IRP_MN_CANCEL_REMOVE_DEVICE) and we need to
            // process the queued requests then.
            //

            fdoData->QueueState = HoldRequests;

            SET_NEW_PNP_STATE(fdoData, RemovePending);

            ToasterDebugPrint((3, "Query - remove holding requests...\n"));

            ToasterIoDecrement(fdoData);

            //
            // Wait for all the requests to be completed
            //

            KeWaitForSingleObject(
                &fdoData->StopEvent,
                Executive, // Waiting for reason of a driver
                KernelMode, // Waiting in kernel mode
                FALSE, // No alert
                NULL); // No timeout


            Irp->IoStatus.Status = STATUS_SUCCESS;
            IoSkipCurrentIrpStackLocation (Irp);

            status = IoCallDriver (fdoData->NextLowerDriver, Irp);
            return status;
        }
        break;

    case IRP_MN_CANCEL_REMOVE_DEVICE:

        //
        // We need to reset the QueueState flag to ProcessRequest,
        // since the device resume its normal activities.
        //

        //
        // First check to see whether you have received cancel-remove
        // without first receiving a query-remove. This could happen if
        // someone above us fails a query-remove and passes down the
        // subsequent cancel-remove.
        //

        if(RemovePending == fdoData->DevicePnPState)
        {

            status = ToasterSendIrpSynchronously(fdoData->NextLowerDriver,Irp);

            if(NT_SUCCESS(status))
            {
                fdoData->QueueState = AllowRequests;

                RESTORE_PREVIOUS_PNP_STATE(fdoData);

                //
                // Process the queued requests that arrived between
                // QUERY_REMOVE and CANCEL_REMOVE.
                //

                ToasterProcessQueuedRequests(fdoData);
            } else {
                //
                // Nobody can fail this IRP. This is a fatal error.
                //
                ASSERTMSG("Cancel remove failed. Fatal error!", FALSE);
                ToasterDebugPrint((3, "Failure status = 0x%x\n", status));
            }
        }
        else {
            //
            // Spurious cancel-remove so let us just complete it here.
            //
            status = STATUS_SUCCESS;
        }

        break;

   case IRP_MN_SURPRISE_REMOVAL:

        //
        // The device has been unexpectedly removed from the machine
        // and is no longer available for I/O ("surprise" removal).
        // We must return device and memory resources,
        // disable interfaces. We will defer failing any outstanding
        // request to IRP_MN_REMOVE.
        //

        fdoData->QueueState = FailRequests;

        //
        // Fail all the pending request. Since the QueueState is FailRequests
        // ToasterProcessQueuedRequests will simply flush the queue,
        // completing each IRP with STATUS_DELETE_PENDING
        //

        ToasterProcessQueuedRequests(fdoData);

        SET_NEW_PNP_STATE(fdoData, SurpriseRemovePending);

        //
        // Disable the device interface.
        //

        status = IoSetDeviceInterfaceState(&fdoData->InterfaceName, FALSE);

        if (!NT_SUCCESS (status)) {
            ToasterDebugPrint(
                      (0, "IoSetDeviceInterfaceState failed: 0x%x\n", status));
        }

        //
        // Return any resources acquired during device startup.
        //

        ToasterReturnResources(DeviceObject);

        //
        // We must set Irp->IoStatus.Status to STATUS_SUCCESS before
        // passing it down.
        //
        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (fdoData->NextLowerDriver, Irp);
        ToasterIoDecrement(fdoData);

        return status;



   case IRP_MN_REMOVE_DEVICE:

        //
        // The Plug & Play system has dictated the removal of this device.  We
        // have no choice but to detach and delete the device object.
        // (If we wanted to express an interest in preventing this removal,
        // we should have failed the query remove IRP).
        //


        if(SurpriseRemovePending != fdoData->DevicePnPState)
        {
            //
            // This means we are here after query-remove.
            // So first stop the device, disable the interface,
            // return resources, and fail all the pending request,.
            //

            fdoData->QueueState = FailRequests;

            //
            // Fail all the pending request. Since the QueueState is FailRequests
            // ToasterProcessQueuedRequests will simply flush the queue,
            // completing each IRP with STATUS_DELETE_PENDING
            //

            ToasterProcessQueuedRequests(fdoData);

            //
            // Disable the Interface
            //

            status = IoSetDeviceInterfaceState(&fdoData->InterfaceName, FALSE);

            if (!NT_SUCCESS (status)) {
                ToasterDebugPrint((0,
                        "IoSetDeviceInterfaceState failed: 0x%x\n", status));
            }

            //
            // Return hardware resources.
            //

            ToasterReturnResources(DeviceObject);

        }

        SET_NEW_PNP_STATE(fdoData, Deleted);

        //
        // Inform WMI to remove this DeviceObject from its
        // list of providers.
        //

        ToasterWmiDeRegistration(fdoData);


        //
        // We need two decrements here, one for the increment in
        // ToasterPnpDispatch, the other for the 1-biased value of
        // OutstandingIO. Also, we need to wait that all the requests
        // are served.
        //

        requestCount = ToasterIoDecrement (fdoData);

        //
        // The requestCount is a least one here (is 1-biased)
        //
        ASSERT(requestCount > 0);

        requestCount = ToasterIoDecrement (fdoData);

        KeWaitForSingleObject (
                &fdoData->RemoveEvent,
                Executive,
                KernelMode,
                FALSE,
                NULL);


        //
        // Send on the remove IRP.
        // We need to send the remove down the stack before we detach,
        // but we don't need to wait for the completion of this operation
        // (and to register a completion routine).
        //

        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (fdoData->NextLowerDriver, Irp);

        //
        // Detach the FDO from the device stack
        //
        IoDetachDevice (fdoData->NextLowerDriver);

        //
        // Free up interface memory
        //

        RtlFreeUnicodeString(&fdoData->InterfaceName);
        IoDeleteDevice (fdoData->Self);

        return status;

    case IRP_MN_QUERY_CAPABILITIES:

        //
        // We will provide here an example of an IRP that is processed
        // both on its way down and on its way up: there might be no need for
        // a function driver process this Irp (the bus driver will do that).
        // The driver will wait for the lower drivers (the bus driver among
        // them) to process this IRP, then it processes it again.
        //
        //

        deviceCapabilities = stack->Parameters.DeviceCapabilities.Capabilities;

        if(deviceCapabilities->Version != 1 ||
            deviceCapabilities->Size < sizeof(DEVICE_CAPABILITIES))
        {
            //
            // We don't support this version. Fail the requests
            //
            status = STATUS_UNSUCCESSFUL;
            break;
        }
        //
        // Set some values in deviceCapabilities here...
        //
        // .............................................
        //
        //
        // Prepare to pass the IRP down
        //

        status = ToasterSendIrpSynchronously(fdoData->NextLowerDriver, Irp);
        //
        // Lower drivers have finished their operation, so now
        // we can finish ours.
        //

        if (NT_SUCCESS (status) ) {
            //
            // Here you can override the values set by the underlying drivers.
            //

            //
            // Let us capture the devicecaps.
            //

            fdoData->DeviceCaps = *deviceCapabilities;
        }

        break;

    case IRP_MN_QUERY_PNP_DEVICE_STATE:

        //
        // Pass the IRP down because the modification to the Irp is done
        // on the way up.
        //

        status = ToasterSendIrpSynchronously(fdoData->NextLowerDriver, Irp);

        //
        // We have an example of how to handle invalidate device request.
        // Not all drivers are expected to handle this request.
        // If DontDisplayInUI is TRUE then we have earlier received
        // an ioctl to invalidate our device state. As soon as this
        // IRP is completed the device will marked hidden and it
        // wouldn't show up in the device manager unless the "Show hidden
        // devices" is selected.
        //

        if(fdoData->DontDisplayInUI)
        {
            deviceState = (PPNP_DEVICE_STATE) &(Irp->IoStatus.Information);
            if(deviceState) {
                SETMASK((*deviceState), PNP_DEVICE_DONT_DISPLAY_IN_UI);
            } else {
                ASSERT(deviceState);
            }
            status = STATUS_SUCCESS;
        }
        break;

    default:
        //
        // Pass down all the unhandled Irps.
        //

        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (fdoData->NextLowerDriver, Irp);
        ToasterIoDecrement(fdoData);

        return status;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    ToasterIoDecrement(fdoData);

    return status;
}


NTSTATUS
ToasterDispatchPnpComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:
    The lower-level drivers completed the pnp IRP.
    Signal this to whoever registered us.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

   Context - pointer to the event to be signaled.
Return Value:

    NT status code


--*/
{
    PKEVENT             event = (PKEVENT)Context;

    UNREFERENCED_PARAMETER (DeviceObject);

    //
    // Wait for lower drivers to be done with the Irp.
    // Important thing to note here is when you allocate
    // the memory for an event in the stack you must do a
    // KernelMode wait instead of UserMode to prevent
    // the stack from getting paged out.
    //

    KeSetEvent (event, IO_NO_INCREMENT, FALSE);

    //
    // Take the IRP back so that we can continue using it during
    // the dispatch routine.
    // NB: The dispatch routine will have to call IoCompleteRequest
    //

    return STATUS_MORE_PROCESSING_REQUIRED;
}



NTSTATUS
ToasterReadWrite (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Performs read/write to the toaster device.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

    NT status code


--*/

{
    PFDO_DATA   fdoData;
    NTSTATUS    status;

    PAGED_CODE();

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    ToasterDebugPrint((3, "ReadWrite called\n"));

    ToasterIoIncrement (fdoData);

    if (HoldRequests == fdoData->QueueState) {

        return ToasterQueueRequest(fdoData, Irp);

    } else if (Deleted == fdoData->DevicePnPState) {

        //
        // Since the device is removed, we will not hold any IRPs.
        // We just fail it.
        //
        Irp->IoStatus.Status = STATUS_DELETE_PENDING;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        ToasterIoDecrement(fdoData);
        return STATUS_DELETE_PENDING;
    }


    //
    // Perform read/write operation here
    //
    status = STATUS_SUCCESS;

    Irp->IoStatus.Information = 0; // fill in the correct length
    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    ToasterIoDecrement(fdoData);

    return status;

}

NTSTATUS
ToasterCreate (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

   Dispatch routine to handle Create commands for the toaster device.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

   NT status code

--*/
{
    PFDO_DATA    fdoData;
    NTSTATUS     status;
    PIO_STACK_LOCATION  irpStack;
    TOASTER_INTERFACE_STANDARD busInterface;

    PAGED_CODE ();

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    ToasterDebugPrint((3, "Create \n"));

    //
    // Since we don't access the hardware to process create, we don't have to
    // worry about about the current device state and whether or not to queue
    // this request.
    //

    ToasterIoIncrement (fdoData);

    if (Deleted == fdoData->DevicePnPState) {

        //
        // Since the device is removed, we will not hold any IRPs.
        // We just fail it.
        //
        Irp->IoStatus.Status = STATUS_DELETE_PENDING;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        ToasterIoDecrement(fdoData);
        return STATUS_DELETE_PENDING;
    }

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    //
    // We will provide an example on how to get a bus-specific direct
    // call interface from a bus driver using IRP_MN_QUERY_INTERFACE Irp.
    //
    status = ToasterGetStandardInterface(fdoData->NextLowerDriver, &busInterface);
    if(NT_SUCCESS(status))
    {
        UCHAR powerlevel;

        (*busInterface.GetCrispinessLevel)(busInterface.Context, &powerlevel);
        (*busInterface.SetCrispinessLevel)(busInterface.Context, 8);
        (*busInterface.IsSafetyLockEnabled)(busInterface.Context);
        //
        // The provider of this interface (in this case bus driver)
        // takes a reference count to avoid itself being removed when
        // somebody has pointer to its callback functions. It's our
        // responsibility to remove the reference as soon as we are done using
        // the interface. If the device interface is from an unrelated
        // device we must register a PnP notification for device removal
        // to dereference and stop using the interface.
        //
        (*busInterface.InterfaceDereference)((PVOID)busInterface.Context);
    }

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    ToasterIoDecrement(fdoData);


    return status;
}


NTSTATUS
ToasterClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

   Dispatch routine to handle Close for the toaster device.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

   NT status code

--*/
{
    PFDO_DATA    fdoData;
    NTSTATUS     status;
    TOASTER_INTERFACE_STANDARD busInterface;

    PAGED_CODE ();

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    ToasterDebugPrint((3, "Close \n"));

    //
    // Since we don't access the hardware to process close requests, we don't
    // have to worry about the current device state and whether or not to queue
    // this request.
    //
    // Note that we don't check to see if the device is deleted. On Win2K and
    // above, we will never see a close IRP after we've been removed (unless
    // some 3rd party driver in our stack is faking them)
    //
    // On Win9x however, we can see these after the remove. For surprise
    // removal, Win2K will send IRP_MN_SURPRISE_REMOVAL, wait for the handles
    // to close, then send IRP_MN_REMOVE_DEVICE. Win9x never sends
    // IRP_MN_SURPRISE_REMOVAL - it immediately sends a IRP_MN_REMOVE_DEVICE,
    // even if we still have open handles! To keep from leaking memory, we
    // will handle the closes even after we are deleted.
    //

    ToasterIoIncrement (fdoData);

    status = STATUS_SUCCESS;

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    ToasterIoDecrement(fdoData);


    return status;
}


NTSTATUS
ToasterDispatchIoctl(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
/*++
Routine Description:

    Handle user mode DeviceIoControl requests.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

   NT status code

--*/
{
    PIO_STACK_LOCATION      irpStack;
    NTSTATUS                status= STATUS_SUCCESS;
    PFDO_DATA               fdoData;

    PAGED_CODE ();

    ToasterDebugPrint((3, "Ioctl called\n"));

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    ToasterIoIncrement (fdoData);

    if (HoldRequests == fdoData->QueueState) {

        return ToasterQueueRequest(fdoData, Irp);

    } else if (Deleted == fdoData->DevicePnPState) {

        //
        // Since the device is removed, we will not hold any IRPs.
        // We just fail it.
        //
        Irp->IoStatus.Status = STATUS_DELETE_PENDING;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        ToasterIoDecrement(fdoData);
        return STATUS_DELETE_PENDING;
    }

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {

    case IOCTL_TOASTER_DONT_DISPLAY_IN_UI_DEVICE:

        fdoData->DontDisplayInUI = TRUE;
        IoInvalidateDeviceState(fdoData->UnderlyingPDO);
        Irp->IoStatus.Information = 0;
        break;

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    ToasterIoDecrement(fdoData);
    return status;
}



NTSTATUS
ToasterStartDevice (
    IN PFDO_DATA     FdoData,
    IN PIRP             Irp
    )
/*++

Routine Description:

    Performs whatever initialization is needed to setup the
    device, namely connecting to an interrupt,
    setting up a DMA channel or mapping any I/O port resources.

Arguments:

   Irp - pointer to an I/O Request Packet.

   FdoData - pointer to a FDO_DATA structure

Return Value:

    NT status code


--*/
{
    NTSTATUS    status = STATUS_SUCCESS;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resourceTrans;
    PCM_PARTIAL_RESOURCE_LIST   partialResourceList;
    PCM_PARTIAL_RESOURCE_LIST   partialResourceListTranslated;
    PIO_STACK_LOCATION stack;
    POWER_STATE powerState;
    ULONG i;

    stack = IoGetCurrentIrpStackLocation (Irp);

    PAGED_CODE();


    //
    // Do whatever initialization needed when starting the device:
    // gather information about it,  update the registry, etc.
    // At this point, the lower level drivers completed the IRP
    //

    if ((NULL != stack->Parameters.StartDevice.AllocatedResources) &&
        (NULL != stack->Parameters.StartDevice.AllocatedResourcesTranslated)) {

    //
    // Parameters.StartDevice.AllocatedResources points to a
    // CM_RESOURCE_LIST describing the hardware resources that
    // the PnP Manager assigned to the device. This list contains
    // the resources in raw form. Use the raw resources to program
    // the device.
    //

    partialResourceList =
    &stack->Parameters.StartDevice.AllocatedResources->List[0].PartialResourceList;

    resource = &partialResourceList->PartialDescriptors[0];

    //
    // Parameters.StartDevice.AllocatedResourcesTranslated points
    // to a CM_RESOURCE_LIST describing the hardware resources that
    // the PnP Manager assigned to the device. This list contains
    // the resources in translated form. Use the translated resources
    // to connect the interrupt vector, map I/O space, and map memory.
    //

    partialResourceListTranslated =
    &stack->Parameters.StartDevice.AllocatedResourcesTranslated->List[0].PartialResourceList;

    resourceTrans = &partialResourceListTranslated->PartialDescriptors[0];

    for (i = 0;
            i < partialResourceList->Count; i++, resource++, resourceTrans++) {

        switch (resource->Type) {
        case CmResourceTypePort:

            ToasterDebugPrint((2, "Resource RAW Port: (%x) Length: (%d)\n",
                resource->u.Port.Start.LowPart, resource->u.Port.Length));

            switch (resourceTrans->Type) {

            case CmResourceTypePort:

                ToasterDebugPrint((2,
                    "Resource Translated Port: (%x) Length: (%d)\n",
                    resourceTrans->u.Port.Start.LowPart,
                    resourceTrans->u.Port.Length));


                break;

            case CmResourceTypeMemory:

                ToasterDebugPrint((2,
                    "Resource Translated Memory: (%x) Length: (%d)\n",
                    resourceTrans->u.Memory.Start.LowPart,
                    resourceTrans->u.Memory.Length));
                //
                // Perform memory mapping here.
                //

                break;

            default:
                ToasterDebugPrint((0, "Unhandled resource_type (0x%x)\n",
                                        resourceTrans->Type));
                TRAP ();
            }
            break;

        case CmResourceTypeMemory:


            ToasterDebugPrint((2, "Resource RAW Memory: (%x) Length: (%d)\n",
                resource->u.Memory.Start.LowPart, resource->u.Memory.Length));

            ToasterDebugPrint((2, "Resource Translated Memory: (%x) Length: (%d)\n",
                resourceTrans->u.Memory.Start.LowPart, resourceTrans->u.Memory.Length));
            //
            // Perform mapping.
            //

            break;

        case CmResourceTypeInterrupt:

        default:

            ToasterDebugPrint((0, "Unhandled resource type (0x%x)\n", resource->Type));
            break;

        }
    }
    }

    //
    // Start is an implicit power-up. Since we are done programming the
    // hardware, we should be in D0.
    //
    FdoData->DevicePowerState = PowerDeviceD0;

    powerState.DeviceState = PowerDeviceD0;

    PoSetPowerState ( FdoData->Self, DevicePowerState, powerState );

    //
    // Enable the device interface. Return status
    // STATUS_OBJECT_NAME_EXISTS means we are enabling the interface
    // that was already enabled, which could happen if the device
    // is stopped and restarted for resource rebalancing.
    //
    status = IoSetDeviceInterfaceState(&FdoData->InterfaceName, TRUE);

    if (!NT_SUCCESS (status)){
        ToasterDebugPrint((0, "IoSetDeviceInterfaceState failed: 0x%x\n",
                                status));
        return status;
    }

    SET_NEW_PNP_STATE(FdoData, Started);

    //
    // Mark the device as active and not holding IRPs
    //

    FdoData->QueueState = AllowRequests;

    //
    // The last thing to do is to process the pending IRPs.
    //

    ToasterProcessQueuedRequests(FdoData);

    //
    // Fire a device arrival event.
    //
    ToasterFireArrivalEvent(FdoData->Self);

    return status;

}


NTSTATUS
ToasterCleanup (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    The dispatch routine for cleanup: we need to walk the Irp queue and
    to cancel all the requests for which the file object is the same with
    the one in the Irp.


Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

   NT status code

--*/
{
    PFDO_DATA              fdoData;
    KIRQL                  oldIrql;
    LIST_ENTRY             cleanupList;
    PLIST_ENTRY            thisEntry, nextEntry, listHead;
    PIRP                   pendingIrp;
    PIO_STACK_LOCATION     pendingIrpStack, irpStack;


    ToasterDebugPrint((3, "Cleanup called\n"));

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;
    ToasterIoIncrement (fdoData);

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    InitializeListHead(&cleanupList);

    //
    // We must acquire queue lock first.
    //

    KeAcquireSpinLock(&fdoData->QueueLock, &oldIrql);

    //
    // Walk through the list and remove all the IRPs
    // that belong to the input irp's fileobject.
    //

    listHead = &fdoData->NewRequestsQueue;
    for(thisEntry = listHead->Flink,nextEntry = thisEntry->Flink;
       thisEntry != listHead;
       thisEntry = nextEntry,nextEntry = thisEntry->Flink)
    {

        pendingIrp = CONTAINING_RECORD(thisEntry, IRP,
                                Tail.Overlay.ListEntry);
        pendingIrpStack = IoGetCurrentIrpStackLocation(pendingIrp);

        if (irpStack->FileObject == pendingIrpStack->FileObject)
        {
            RemoveEntryList(thisEntry);

            //
            // Set the cancel routine to NULL
            //
            if(NULL == IoSetCancelRoutine (pendingIrp, NULL))
            {
                //
                // The cancel routine has run but it must be waiting to hold
                // the queue lock. It will cancel the IRP as soon as we
                // drop the lock outside this loop. We will initialize
                // the IRP's listEntry so that the cancel routine wouldn't barf
                // when it tries to remove the IRP from the queue, and
                // leave the this IRP alone.
                //
                InitializeListHead(thisEntry);
            } else {
                //
                // Cancel routine is not called and will never be
                // called. So we queue the IRP in the cleanupList
                // and cancel it after dropping the lock
                //
                InsertTailList(&cleanupList, thisEntry);
            }
        }
    }

    //
    // Release the spin lock.
    //

    KeReleaseSpinLock(&fdoData->QueueLock, oldIrql);

    //
    // Walk through the cleanup list and cancel all
    // the Irps.
    //

    while(!IsListEmpty(&cleanupList))
    {
        //
        // Complete the IRP
        //

        thisEntry = RemoveHeadList(&cleanupList);
        pendingIrp = CONTAINING_RECORD(thisEntry, IRP,
                                Tail.Overlay.ListEntry);
        //
        // You must clear the cancel routine before completing the IRP.
        //

        IoSetCancelRoutine (pendingIrp, NULL);
        pendingIrp->IoStatus.Information = 0;
        pendingIrp->IoStatus.Status = STATUS_CANCELLED;
        IoCompleteRequest(pendingIrp, IO_NO_INCREMENT);
    }

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    ToasterIoDecrement (fdoData);
    return STATUS_SUCCESS;
}




VOID
ToasterUnload(
    IN PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    Free all the resources allocated in DriverEntry.

Arguments:

    DriverObject - pointer to a driver object.

Return Value:

    VOID.

--*/
{
    PAGED_CODE ();

    //
    // The device object(s) should be NULL now
    // (since we unload, all the devices objects associated with this
    // driver must be deleted.
    //
    ASSERT(DriverObject->DeviceObject == NULL);

    //
    // We should not be unloaded until all the devices we control
    // have been removed from our queue.
    //
    ToasterDebugPrint ((2, "unload\n"));

    if(Globals.RegistryPath.Buffer)
        ExFreePool(Globals.RegistryPath.Buffer);

    return;
}



NTSTATUS
ToasterQueueRequest    (
    IN OUT PFDO_DATA FdoData,
    IN PIRP Irp
    )

/*++

Routine Description:

    Queues the Irp in the device queue. This routine will be called whenever
    the device receives IRP_MN_QUERY_STOP_DEVICE or IRP_MN_QUERY_REMOVE_DEVICE

Arguments:

    FdoData - pointer to the device's extension.

    Irp - the request to be queued.

Return Value:

    NT status code.

--*/
{

    KIRQL               oldIrql;
    NTSTATUS status = STATUS_PENDING;

    ToasterDebugPrint ((2, "Queuing Requests\n"));

    //
    // Check whether we are allowed to queue requests.
    //

    ASSERT(HoldRequests == FdoData->QueueState);

    KeAcquireSpinLock(&FdoData->QueueLock, &oldIrql);

    InsertTailList(&FdoData->NewRequestsQueue,
                        &Irp->Tail.Overlay.ListEntry);

    IoMarkIrpPending(Irp);

    //
    // Set the cancel routine
    //

    IoSetCancelRoutine (Irp, ToasterCancelQueued);

    KeReleaseSpinLock(&FdoData->QueueLock, oldIrql);

    ToasterIoDecrement(FdoData);
    return status;
}



VOID
ToasterProcessQueuedRequests    (
    IN OUT PFDO_DATA FdoData
    )

/*++

Routine Description:

    Removes and processes the entries in the queue. If this routine is called
    when processing IRP_MN_CANCEL_STOP_DEVICE, IRP_MN_CANCEL_REMOVE_DEVICE
    or IRP_MN_START_DEVICE, the requests are passed to the next lower driver.
    If the routine is called when IRP_MN_REMOVE_DEVICE is received, the IRPs
    are completed with STATUS_DELETE_PENDING.


Arguments:

    FdoData - pointer to the device's extension (where is the held IRPs queue).


Return Value:

    VOID.

--*/
{

    KIRQL               oldIrql;
    PIRP                nextIrp, cancelledIrp;
    PLIST_ENTRY         listEntry;
    LIST_ENTRY          cancelledIrpList;
    PVOID               cancelRoutine= NULL;

    ToasterDebugPrint ((2, "Process or fail queued Requests\n"));

    InitializeListHead(&cancelledIrpList);

    //
    // We need to dequeue all the entries in the queue, reset the cancel
    // routine for each of them and then process then:
    // - if the device is active, we will send them down
    // - else we will complete them with STATUS_DELETE_PENDING
    //

    for(;;)
    {
        //
        // Acquire the queue lock before manipulating the list entries.
        //
        KeAcquireSpinLock(&FdoData->QueueLock, &oldIrql);

        if(IsListEmpty(&FdoData->NewRequestsQueue))
        {
            KeReleaseSpinLock(&FdoData->QueueLock, oldIrql);
            break;
        }

        //
        // Remove a request from the queue.
        //
        listEntry = RemoveHeadList(&FdoData->NewRequestsQueue);

        nextIrp = CONTAINING_RECORD(listEntry, IRP, Tail.Overlay.ListEntry);

        //
        // Set the cancel routine to NULL. This is an atomic operation.
        //
        cancelRoutine = IoSetCancelRoutine (nextIrp, NULL);

        //
        // Check to see if it's cancelled.
        //
        if (nextIrp->Cancel)
        {
            //
            // The I/O manager sets the CancelRoutine pointer in the IRP
            // to NULL before calling the routine. So check the previous
            // value to decide whether cancel routine has been
            // called or not.
            //
            if(cancelRoutine)
            {
                //
                // This IRP was just cancelled but the cancel routine
                // hasn't been called yet. So it's safe to cancel the IRP,
                // Let's queue the IRP in the cancelledIrp list and complete
                // them after releasing the lock. This is to ensure that
                // we don't call out of the driver while holding a lock.
                //

                InsertTailList(&cancelledIrpList, listEntry);
            } else {
                //
                // The cancel routine has run but it must be waiting to hold
                // the queue lock. It will cancel the IRP as soon as we
                // drop the lock. So initialize the IRPs
                // listEntry so that the cancel routine wouldn't barf
                // when it tries to remove the IRP from the queue.
                //
                InitializeListHead(listEntry);
            }

            KeReleaseSpinLock(&FdoData->QueueLock, oldIrql);
        }
        else
        {
            //
            // Release the lock before we call out of the driver
            //

            KeReleaseSpinLock(&FdoData->QueueLock, oldIrql);

            if (FailRequests == FdoData->QueueState) {
                //
                // The device was removed, we need to fail the request
                //
                nextIrp->IoStatus.Information = 0;
                nextIrp->IoStatus.Status = STATUS_DELETE_PENDING;
                IoCompleteRequest (nextIrp, IO_NO_INCREMENT);

            } else {
                //
                // Process the IRP. Depending on the type of the
                // IRP either we pass it down or complete it here.
                //
                ToasterIoIncrement (FdoData);

                IoSkipCurrentIrpStackLocation (nextIrp);
                IoCallDriver (FdoData->NextLowerDriver, nextIrp);

                ToasterIoDecrement(FdoData);
            }

        }
    }// end of loop

    //
    // Walk through the cancelledIrp list and cancel all
    // the Irps.
    //

    while(!IsListEmpty(&cancelledIrpList))
    {
        //
        // Complete the IRP
        //

        PLIST_ENTRY cancelledListEntry = RemoveHeadList(&cancelledIrpList);
        cancelledIrp = CONTAINING_RECORD(cancelledListEntry, IRP,
                                Tail.Overlay.ListEntry);
        cancelledIrp->IoStatus.Information = 0;
        cancelledIrp->IoStatus.Status = STATUS_CANCELLED;
        IoCompleteRequest(cancelledIrp, IO_NO_INCREMENT);
    }

    return;

}

VOID
ToasterCancelQueued (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )

/*++

Routine Description:

    The cancel routine. It will remove the IRP from the queue
    and will complete it. The cancel spin lock is already acquired
    when this routine is called.

Arguments:

    DeviceObject - pointer to the device object.

    Irp - pointer to the IRP to be cancelled.


Return Value:

    VOID.

--*/
{
    PFDO_DATA fdoData = DeviceObject->DeviceExtension;
    KIRQL               oldIrql = Irp->CancelIrql;

    ToasterDebugPrint ((2, "Canceling Requests\n"));

    //
    // Release the cancel spinlock
    //

    IoReleaseCancelSpinLock( Irp->CancelIrql);

    //
    // Acquire the local spinlock
    //

    KeAcquireSpinLockAtDpcLevel(&fdoData->QueueLock);


    //
    // Remove the cancelled IRP from queue and
    // release the queue lock.
    //
    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);

    KeReleaseSpinLock(&fdoData->QueueLock, oldIrql);

    //
    // Complete the request with STATUS_CANCELLED.
    //

    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    return;

}


NTSTATUS
ToasterCanStopDevice    (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )

/*++

Routine Description:

    This routine determines whether the device can be safely stopped. In our
    particular case, we'll assume we can always stop the device.
    A device might fail the request if it doesn't have a queue for the
    requests it might come or if it was notified that it is in the paging
    path.

Arguments:

    DeviceObject - pointer to the device object.

    Irp - pointer to the current IRP.


Return Value:

    STATUS_SUCCESS if the device can be safely stopped, an appropriate
    NT Status if not.

--*/
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    return STATUS_SUCCESS;
}

NTSTATUS
ToasterCanRemoveDevice    (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )

/*++

Routine Description:

    This routine determines whether the device can be safely removed. In our
    particular case, we'll assume we can always remove the device.
    A device shouldn't be removed if, for example, it has open handles or
    removing the device could result in losing data (plus the reasons
    mentioned at ToasterCanStopDevice). The PnP manager on Windows 2000 fails
    on its own any attempt to remove, if there any open handles to the device.
    However on Win9x, the driver must keep count of open handles and fail
    query_remove if there are any open handles.

Arguments:

    DeviceObject - pointer to the device object.

    Irp - pointer to the current IRP.


Return Value:

    STATUS_SUCCESS if the device can be safely removed, an appropriate
    NT Status if not.

--*/
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    return STATUS_SUCCESS;
}

NTSTATUS
ToasterReturnResources (
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine returns all the resources acquired during
    device startup.

Arguments:

    DeviceObject - pointer to the device object.


Return Value:

    STATUS_SUCCESS if the device can be safely removed, an appropriate
    NT Status if not.

--*/
{
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    // Disconnect from the interrupt and unmap any I/O ports
    // that are mapped in StartDevice.
    //

    return STATUS_SUCCESS;
}

NTSTATUS
ToasterSendIrpSynchronously (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Sends the Irp down the stack and waits for it to complete.

Arguments:
    DeviceObject - pointer to the device object.

    Irp - pointer to the current IRP.

    NotImplementedIsValid -

Return Value:

    NT status code

--*/
{
    KEVENT   event;
    NTSTATUS status;

    PAGED_CODE();

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    IoCopyCurrentIrpStackLocationToNext(Irp);

    IoSetCompletionRoutine(Irp,
                           ToasterDispatchPnpComplete,
                           &event,
                           TRUE,
                           TRUE,
                           TRUE
                           );

    status = IoCallDriver(DeviceObject, Irp);

    //
    // Wait for lower drivers to be done with the Irp.
    // Important thing to note here is when you allocate
    // memory for an event in the stack you must do a
    // KernelMode wait instead of UserMode to prevent
    // the stack from getting paged out.
    //
    //

    if (status == STATUS_PENDING) {
       KeWaitForSingleObject(&event,
                             Executive,
                             KernelMode,
                             FALSE,
                             NULL
                             );
       status = Irp->IoStatus.Status;
    }

    return status;
}

NTSTATUS
ToasterGetStandardInterface(
    IN PDEVICE_OBJECT DeviceObject,
    OUT PTOASTER_INTERFACE_STANDARD BusInterface
    )
/*++

Routine Description:

    This routine gets the bus interface standard information from the PDO.

Arguments:

    DeviceObject - Device object to query for this information.

    BusInterface - Supplies a pointer to the retrieved information.

Return Value:

    NT status.

--*/
{
    KEVENT event;
    NTSTATUS status;
    PIRP irp;
    IO_STATUS_BLOCK ioStatusBlock;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_OBJECT targetObject;

    ToasterDebugPrint((2, "ToasterGetBusStandardInterface entered.\n"));

    KeInitializeEvent( &event, NotificationEvent, FALSE );

    targetObject = IoGetAttachedDeviceReference( DeviceObject );

    irp = IoBuildSynchronousFsdRequest( IRP_MJ_PNP,
                                        targetObject,
                                        NULL,
                                        0,
                                        NULL,
                                        &event,
                                        &ioStatusBlock );

    if (irp == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto End;
    }

    irpStack = IoGetNextIrpStackLocation( irp );
    irpStack->MinorFunction = IRP_MN_QUERY_INTERFACE;
    irpStack->Parameters.QueryInterface.InterfaceType =
                        (LPGUID) &GUID_TOASTER_INTERFACE_STANDARD ;
    irpStack->Parameters.QueryInterface.Size = sizeof(TOASTER_INTERFACE_STANDARD);
    irpStack->Parameters.QueryInterface.Version = 1;
    irpStack->Parameters.QueryInterface.Interface = (PINTERFACE) BusInterface;
    irpStack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

    //
    // Initialize the status to error in case the bus driver decides not to
    // set it correctly.
    //

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED ;


    status = IoCallDriver( targetObject, irp );

    if (status == STATUS_PENDING) {

        KeWaitForSingleObject( &event, Executive, KernelMode, FALSE, NULL );
        status = ioStatusBlock.Status;
    }

    if (!NT_SUCCESS( status)) {
        ToasterDebugPrint((2, "Failure status :0x%x\n", status));
    }

End:
    //
    // Done with reference
    //
    ObDereferenceObject( targetObject );

    return status;

}



LONG
ToasterIoIncrement    (
    IN  OUT PFDO_DATA   FdoData
    )

/*++

Routine Description:

    This routine increments the number of requests the device receives


Arguments:

    DeviceObject - pointer to the device object.

Return Value:

    The value of OutstandingIO field in the device extension.


--*/

{

    LONG            result;


    result = InterlockedIncrement(&FdoData->OutstandingIO);

    //ToasterDebugPrint((2, "ToasterIoIncrement %d\n", result));

    ASSERT(result > 0);
    //
    // Need to clear StopEvent (when OutstandingIO bumps from 1 to 2)
    //
    if (result == 2) {
        //
        // We need to clear the event
        //
        KeClearEvent(&FdoData->StopEvent);
    }

    return result;
}

LONG
ToasterIoDecrement    (
    IN  OUT PFDO_DATA  FdoData
    )

/*++

Routine Description:

    This routine decrements as it complete the request it receives

Arguments:

    DeviceObject - pointer to the device object.

Return Value:

    The value of OutstandingIO field in the device extension.


--*/
{

    LONG            result;

    result = InterlockedDecrement(&FdoData->OutstandingIO);

    //ToasterDebugPrint((2, "ToasterIoDecrement %d\n", result));

    ASSERT(result >= 0);

    if (result == 1) {
        //
        // Set the stop event. Note that when this happens
        // (i.e. a transition from 2 to 1), the type of requests we
        // want to be processed are already held instead of being
        // passed away, so that we can't "miss" a request that
        // will appear between the decrement and the moment when
        // the value is actually used.
        //

        KeSetEvent (&FdoData->StopEvent,
                    IO_NO_INCREMENT,
                    FALSE);

    }

    if (result == 0) {

        //
        // The count is 1-biased, so it can be zero only if an
        // extra decrement is done when a remove Irp is received
        //

        ASSERT(Deleted == FdoData->DevicePnPState);

        //
        // Set the remove event, so the device object can be deleted
        //

        KeSetEvent (&FdoData->RemoveEvent,
                    IO_NO_INCREMENT,
                    FALSE);

    }


    return result;
}


#if DBG

#define     TEMP_BUFFER_SIZE        1024


VOID
DriverDbgPrint    (
    ULONG   DebugPrintLevel,
    PCCHAR  DebugMessage,
    ...
    )

/*++

Routine Description:

    Debug print for the sample driver.

Arguments:

    DebugPrintLevel - print level between 0 and 3, with 3 the most verbose

Return Value:

    None.

 --*/
 {

    va_list    list;
    UCHAR      debugMessageBuffer[TEMP_BUFFER_SIZE];


    va_start(list, DebugMessage);
    if (DebugPrintLevel <= DebugLevel) {

        DbgPrint ("Toaster.sys: ");
        vsprintf(debugMessageBuffer, DebugMessage, list);

        DbgPrint(debugMessageBuffer);
    }

    va_end(list);

    return;

}


PCHAR
PnPMinorFunctionString (
    UCHAR MinorFunction
)
{
    switch (MinorFunction)
    {
        case IRP_MN_START_DEVICE:
            return "IRP_MN_START_DEVICE";
        case IRP_MN_QUERY_REMOVE_DEVICE:
            return "IRP_MN_QUERY_REMOVE_DEVICE";
        case IRP_MN_REMOVE_DEVICE:
            return "IRP_MN_REMOVE_DEVICE";
        case IRP_MN_CANCEL_REMOVE_DEVICE:
            return "IRP_MN_CANCEL_REMOVE_DEVICE";
        case IRP_MN_STOP_DEVICE:
            return "IRP_MN_STOP_DEVICE";
        case IRP_MN_QUERY_STOP_DEVICE:
            return "IRP_MN_QUERY_STOP_DEVICE";
        case IRP_MN_CANCEL_STOP_DEVICE:
            return "IRP_MN_CANCEL_STOP_DEVICE";
        case IRP_MN_QUERY_DEVICE_RELATIONS:
            return "IRP_MN_QUERY_DEVICE_RELATIONS";
        case IRP_MN_QUERY_INTERFACE:
            return "IRP_MN_QUERY_INTERFACE";
        case IRP_MN_QUERY_CAPABILITIES:
            return "IRP_MN_QUERY_CAPABILITIES";
        case IRP_MN_QUERY_RESOURCES:
            return "IRP_MN_QUERY_RESOURCES";
        case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
            return "IRP_MN_QUERY_RESOURCE_REQUIREMENTS";
        case IRP_MN_QUERY_DEVICE_TEXT:
            return "IRP_MN_QUERY_DEVICE_TEXT";
        case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:
            return "IRP_MN_FILTER_RESOURCE_REQUIREMENTS";
        case IRP_MN_READ_CONFIG:
            return "IRP_MN_READ_CONFIG";
        case IRP_MN_WRITE_CONFIG:
            return "IRP_MN_WRITE_CONFIG";
        case IRP_MN_EJECT:
            return "IRP_MN_EJECT";
        case IRP_MN_SET_LOCK:
            return "IRP_MN_SET_LOCK";
        case IRP_MN_QUERY_ID:
            return "IRP_MN_QUERY_ID";
        case IRP_MN_QUERY_PNP_DEVICE_STATE:
            return "IRP_MN_QUERY_PNP_DEVICE_STATE";
        case IRP_MN_QUERY_BUS_INFORMATION:
            return "IRP_MN_QUERY_BUS_INFORMATION";
        case IRP_MN_DEVICE_USAGE_NOTIFICATION:
            return "IRP_MN_DEVICE_USAGE_NOTIFICATION";
        case IRP_MN_SURPRISE_REMOVAL:
            return "IRP_MN_SURPRISE_REMOVAL";
        case IRP_MN_QUERY_LEGACY_BUS_INFORMATION:
            return "IRP_MN_QUERY_LEGACY_BUS_INFORMATION";
        default:
            return "unknown_pnp_irp";
    }
}

#endif

