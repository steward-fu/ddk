/*++
Copyright (c) 1997  Microsoft Corporation

Module Name:

    GAMEENUM.C

Abstract:

    This module contains contains the plugplay calls
    PNP / WDM BUS driver.

Author:

    Kenneth D. Ray

Environment:

    kernel mode only

Notes:


Revision History:


--*/

#include <wdm.h>
#include "gameport.h"
#include "gameenum.h"
#include "stdio.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, Game_AddDevice)
#pragma alloc_text (PAGE, Game_PnP)
#pragma alloc_text (PAGE, Game_FDO_PnP)
#pragma alloc_text (PAGE, Game_PDO_PnP)
#pragma alloc_text (PAGE, Game_Power)
#pragma alloc_text (PAGE, Game_FDO_Power)
#pragma alloc_text (PAGE, Game_PDO_Power)
#pragma alloc_text (PAGE, Game_Expose)
#endif


NTSTATUS
Game_AddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT BusPhysicalDeviceObject
    )
/*++
Routine Description.
    A bus has been found.  Attach our FDO to it.
    Allocate any required resources.  Set things up.  And be prepared for the
    first ``start device.''

Arguments:
    BusDeviceObject - Device object representing the bus.  That to which we
                      attach a new FDO.

    DriverObject - This very self referenced driver.

--*/
{
    NTSTATUS            status;
    PDEVICE_OBJECT      deviceObject;
    PFDO_DEVICE_DATA    deviceData;
    UNICODE_STRING      deviceNameUni;
    PWCHAR              deviceName;
    ULONG               nameLength;

    PAGED_CODE ();

    Game_KdPrint_Def (GAME_DBG_SS_TRACE, ("Add Device: 0x%x\n",
                                          BusPhysicalDeviceObject));

    status = IoCreateDevice (
                    DriverObject,  // our driver object
                    sizeof (FDO_DEVICE_DATA), // device object extension size
                    NULL, // FDOs do not have names
                    FILE_DEVICE_BUS_EXTENDER,
                    0, // No special characteristics
                    TRUE, // our FDO is exclusive
                    &deviceObject); // The device object created

    if (NT_SUCCESS (status)) {
        deviceData = (PFDO_DEVICE_DATA) deviceObject->DeviceExtension;
        RtlFillMemory (deviceData, sizeof (FDO_DEVICE_DATA), 0);

        deviceData->IsFDO = TRUE;
        deviceData->DebugLevel = GAME_DEFAULT_DEBUG_OUTPUT_LEVEL;
        deviceData->Self = deviceObject;
        ExInitializeFastMutex (&deviceData->Mutex);

        deviceData->Removed = FALSE;
        InitializeListHead (&deviceData->PDOs);

        // Set the PDO for use with PlugPlay functions
        deviceData->UnderlyingPDO = BusPhysicalDeviceObject;


        //
        // Attach our filter driver to the device stack.
        // the return value of IoAttachDeviceToDeviceStack is the top of the
        // attachment chain.  This is where all the IRPs should be routed.
        //
        // Our filter will send IRPs to the top of the stack and use the PDO
        // for all PlugPlay functions.
        //
        deviceData->TopOfStack = IoAttachDeviceToDeviceStack (
                                        deviceObject,
                                        BusPhysicalDeviceObject);

        // Bias outstanding request to 1 so that we can look for a
        // transition to zero when processing the remove device PlugPlay IRP.
        deviceData->OutstandingIO = 1;

        KeInitializeEvent(&deviceData->RemoveEvent,
                          SynchronizationEvent,
                          FALSE); // initialized to not signalled

        deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
        deviceObject->Flags |= DO_POWER_PAGABLE;

        //
        // Tell the PlugPlay system that this device will need an interface
        // device class shingle.
        //
        // It may be that the driver cannot hang the shingle until it starts
        // the device itself, so that it can query some of its properties.
        // (Aka the shingles guid (or ref string) is based on the properties
        // of the device.)
        //
        status = IoRegisterDeviceInterface (
                    BusPhysicalDeviceObject,
                    (LPGUID) &GUID_GAMEENUM_BUS_ENUMERATOR,
                    NULL, // No ref string
                    &deviceData->DevClassAssocName);

        if (!NT_SUCCESS (status)) {
            Game_KdPrint (deviceData, GAME_DBG_SS_ERROR,
                          ("AddDevice: IoRegisterDCA failed (%x)", status));
            IoDeleteDevice (deviceObject);
            return status;
        }

        //
        // If for any reason you need to save values in a safe location that
        // clients of this DeviceClassAssociate might be interested in reading
        // here is the time to do so, with the function
        // IoOpenDeviceClassRegistryKey
        // the symbolic link name used is was returned in
        // deviceData->DevClassAssocName (the same name which is returned by
        // IoGetDeviceClassAssociations and the SetupAPI equivs.
        //

#if DBG
        status = IoGetDeviceProperty (BusPhysicalDeviceObject,
                                      DevicePropertyPhysicalDeviceObjectName,
                                      0,
                                      NULL,
                                      &nameLength);

        ASSERT  (STATUS_BUFFER_TOO_SMALL == status);

        deviceName = ExAllocatePool (NonPagedPool, nameLength);

        if (NULL == deviceName) {
            IoDeleteDevice (deviceObject);
            Game_KdPrint (deviceData, GAME_DBG_SS_ERROR,
                          ("AddDevice: no memory to alloc DeviceName (0x%x)",
                           nameLength));
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        IoGetDeviceProperty (BusPhysicalDeviceObject,
                             DevicePropertyPhysicalDeviceObjectName,
                             nameLength,
                             deviceName,
                             &nameLength);

        RtlInitUnicodeString (&deviceNameUni, deviceName);

        Game_KdPrint (deviceData, GAME_DBG_SS_TRACE,
                      ("AddDevice: %x to %x->%x (%ws) \n",
                       deviceObject,
                       deviceData->TopOfStack,
                       BusPhysicalDeviceObject,
                       deviceName));
#endif

        //
        // Turn on the shingle and point it to the given device object.
        //
        status = IoSetDeviceInterfaceState (
                        &deviceData->DevClassAssocName,
                        TRUE);

        if (!NT_SUCCESS (status)) {
            Game_KdPrint (deviceData, GAME_DBG_SS_ERROR,
                          ("AddDevice: IoSetDeviceClass failed (%x)", status));
            return status;
        }
    }

    return status;
}

NTSTATUS
Game_FDO_PnPComplete (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Pirp,
    IN PVOID            Context
    );

NTSTATUS
Game_PnP (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
/*++
Routine Description:
    Answer the plithera of Irp Major PnP IRPS.
--*/
{
    PIO_STACK_LOCATION      irpStack;
    NTSTATUS                status;
    PCOMMON_DEVICE_DATA     commonData;
    KIRQL                   oldIrq;

    PAGED_CODE ();

    status = STATUS_SUCCESS;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    ASSERT (IRP_MJ_PNP == irpStack->MajorFunction);

    commonData = (PCOMMON_DEVICE_DATA) DeviceObject->DeviceExtension;

    if (commonData->IsFDO) {
        Game_KdPrint (commonData, GAME_DBG_PNP_TRACE,
                      ("PNP: Functional DO: %x IRP: %x\n", DeviceObject, Irp));

        status = Game_FDO_PnP (
                    DeviceObject,
                    Irp,
                    irpStack,
                    (PFDO_DEVICE_DATA) commonData);
    } else {
        Game_KdPrint (commonData, GAME_DBG_PNP_TRACE,
                      ("PNP: Physical DO: %x IRP: %x\n", DeviceObject, Irp));

        status = Game_PDO_PnP (
                    DeviceObject,
                    Irp,
                    irpStack,
                    (PPDO_DEVICE_DATA) commonData);
    }

    return status;
}

NTSTATUS
Game_FDO_PnP (
    IN PDEVICE_OBJECT       DeviceObject,
    IN PIRP                 Irp,
    IN PIO_STACK_LOCATION   IrpStack,
    IN PFDO_DEVICE_DATA     DeviceData
    )
/*++
Routine Description:
    Handle requests from the PlugPlay system for the BUS itself

    NB: the various Minor functions of the PlugPlay system will not be
    overlapped and do not have to be reentrant

--*/
{
    NTSTATUS    status;
    KIRQL       irql;
    KEVENT      event;
    ULONG       length;
    ULONG       i;
    PLIST_ENTRY entry;
    PPDO_DEVICE_DATA    pdoData;
    PDEVICE_RELATIONS   relations;
    PIO_STACK_LOCATION  stack;

    PAGED_CODE ();

    status = Game_IncIoCount (DeviceData);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    stack = IoGetCurrentIrpStackLocation (Irp);

    switch (IrpStack->MinorFunction) {
    case IRP_MN_START_DEVICE:
        //
        // BEFORE you are allowed to ``touch'' the device object to which
        // the FDO is attached (that send an irp from the bus to the Device
        // object to which the bus is attached).   You must first pass down
        // the start IRP.  It might not be powered on, or able to access or
        // something.
        //

        if (DeviceData->Started) {
            status = STATUS_SUCCESS;
            break;
        }

        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Start Device\n"));

        KeInitializeEvent (&event, NotificationEvent, FALSE);
        IoCopyCurrentIrpStackLocationToNext (Irp);

        IoSetCompletionRoutine (Irp,
                                Game_FDO_PnPComplete,
                                &event,
                                TRUE,
                                TRUE,
                                TRUE);

        status = IoCallDriver (DeviceData->TopOfStack, Irp);

        if (STATUS_PENDING == status) {
            // wait for it...

            status = KeWaitForSingleObject (&event,
                                            Executive,
                                            KernelMode,
                                            FALSE, // Not allertable
                                            NULL); // No timeout structure

            ASSERT (STATUS_SUCCESS == status);

            status = Irp->IoStatus.Status;
        }

        if (NT_SUCCESS(status)) {
            //
            // Now we can touch the lower device object as it is now started.
            //
            // Do what ever
            //

            if ((NULL == stack->Parameters.StartDevice.AllocatedResources) ||
                (NULL == stack->Parameters.StartDevice.AllocatedResourcesTranslated)) {

                status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }
            status = Game_StartFdo (DeviceData,
                                    &stack->Parameters.StartDevice.AllocatedResources->List[0].PartialResourceList,
                                    &stack->Parameters.StartDevice.AllocatedResourcesTranslated->List[0].PartialResourceList);


            //
            // find the translated resources and store them someplace
            // safe for given out for the PDOs.
            //

            if (NT_SUCCESS (status)) {
                DeviceData->Started = TRUE;
            }
        }

        //
        // We must now complete the IRP, since we stopped it in the
        // completetion routine with MORE_PROCESSING_REQUIRED.
        //

        Irp->IoStatus.Information = 0;
        break;

    case IRP_MN_QUERY_STOP_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Query Stop Device\n"));

        //
        // Test to see if there are any PDO created as children of this FDO
        // If there are then conclude the device is busy and fail the
        // query stop.
        //
        // BUGBUG
        // We could do better, by seing if the children PDOs are actually
        // currently open.  If they are not then we could stop, get new
        // resouces, fill in the new resouce values, and then when a new client
        // opens the PDO use the new resources.  But this works for now.
        //
        if (DeviceData->NumPDOs) {
            status = STATUS_UNSUCCESSFUL;
        } else {
            status = STATUS_SUCCESS;
        }

        Irp->IoStatus.Status = status;
        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (DeviceData->TopOfStack, Irp);

        Game_DecIoCount (DeviceData);
        return status;

    case IRP_MN_STOP_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Stop Device\n"));

        //
        // After the start IRP has been sent to the lower driver object, the
        // bus may NOT send any more IRPS down ``touch'' until another START
        // has occured.
        // What ever access is required must be done before the Irp is passed
        // on.
        //
        // Stop device means that the resources given durring Start device
        // are no revoked.  So we need to stop using them
        //
        if (DeviceData->Started) {
            //
            // Free resources given by start device.
            //
            if (DeviceData->MappedPorts) {
                MmUnmapIoSpace (DeviceData->GamePortAddress,
                                DeviceData->GamePortAddressLength);
            }
        }
        DeviceData->Started = FALSE;

        //
        // We don't need a completion routine so fire and forget.
        //
        // Set the current stack location to the next stack location and
        // call the next device object.
        //
        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (DeviceData->TopOfStack, Irp);

        Game_DecIoCount (DeviceData);
        return status;

    case IRP_MN_REMOVE_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Remove Device\n"));

        //
        // The PlugPlay system has dictacted the removal of this device.  We
        // have no choise but to detach and delete the device objecct.
        // (If we wanted to express and interest in preventing this removal,
        // we should have filtered the query remove and query stop routines.)
        //
        // Note! we might receive a remove WITHOUT first receiving a stop.
        // ASSERT (!DeviceData->Removed);
        //
        // We will accept no new requests
        //
        DeviceData->Removed = TRUE;

        //
        // Complete any outstanding IRPs queued by the driver here.
        //

        //
        // Make the DCA go away.  Some drivers may choose to remove the DCA
        // when they receive a stop or even a query stop.  We just don't care.
        //
        IoSetDeviceInterfaceState (&DeviceData->DevClassAssocName, FALSE);

        //
        // Here if we had any outstanding requests in a personal queue we should
        // complete them all now.
        //
        // Note, the device is guarenteed stopped, so we cannot send it any non-
        // PNP IRPS.
        //

        //
        // Fire and forget
        //
        IoSkipCurrentIrpStackLocation (Irp);
        IoCallDriver (DeviceData->TopOfStack, Irp);

        //
        // Wait for all outstanding requests to complete
        //
        i = InterlockedDecrement (&DeviceData->OutstandingIO);

        ASSERT (0 < i);

        if (0 != InterlockedDecrement (&DeviceData->OutstandingIO)) {
            Game_KdPrint (DeviceData, GAME_DBG_PNP_INFO,
                          ("Remove Device waiting for request to complete\n"));

            KeWaitForSingleObject (&DeviceData->RemoveEvent,
                                   Suspended,
                                   KernelMode,
                                   FALSE, // Not Alertable
                                   NULL); // No timeout
        }
        //
        // Free the associated resources
        //

        //
        // Detatch from the undelying devices.
        //
        Game_KdPrint(DeviceData, GAME_DBG_PNP_INFO,
                        ("IoDetachDevice: 0x%x\n", DeviceData->TopOfStack));
        IoDetachDevice (DeviceData->TopOfStack);

        //
        // Clean up any resources here
        //
        if (DeviceData->Started) {
            //
            // Free resources given by start device.
            //
            if (DeviceData->MappedPorts) {
                MmUnmapIoSpace (DeviceData->GamePortAddress, 1);
                // Here we are assuming that joysticks only use on port.
                // This is the way it has always been, and might always
                // continue to be.  This assumption is everywhere in this stack.
            }
        }

        ExFreePool (DeviceData->DevClassAssocName.Buffer);
        Game_KdPrint(DeviceData, GAME_DBG_PNP_INFO,
                        ("IoDeleteDevice1: 0x%x\n", DeviceObject));
        IoDeleteDevice (DeviceObject);

        return STATUS_SUCCESS;

    case IRP_MN_QUERY_DEVICE_RELATIONS:

        if (BusRelations != IrpStack->Parameters.QueryDeviceRelations.Type) {
            //
            // We don't support this
            //
            goto GAME_FDO_PNP_DEFAULT;
        }

        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Query Relations"));

        //
        // Tell the plug and play system about all the PDOs.
        //
        // There might also be device relations below and above this FDO,
        // so, be sure to propagate the relations from the upper drivers.
        //
        // No Completion routine is needed so long as the status is preset
        // to success.  (PDOs complete plug and play irps with the current
        // IoStatus.Status and IoStatus.Information as the default.)
        //
        ExAcquireFastMutex (&DeviceData->Mutex);

        i = (0 == Irp->IoStatus.Information) ? 0 :
            ((PDEVICE_RELATIONS) Irp->IoStatus.Information)->Count;
        // The current number of PDOs

        Game_KdPrint_Cont (DeviceData, GAME_DBG_PNP_TRACE,
                           ("#PDOS = %d + %d\n", i, DeviceData->NumPDOs));

        length = sizeof(DEVICE_RELATIONS) +
                ((DeviceData->NumPDOs + i) * sizeof (PDEVICE_OBJECT));

        relations = (PDEVICE_RELATIONS) ExAllocatePool (NonPagedPool, length);

        if (NULL == relations) {
            ExReleaseFastMutex (&DeviceData->Mutex);
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        // Copy in the device objects so far
        //
        if (i) {
            RtlCopyMemory (
                  relations->Objects,
                  ((PDEVICE_RELATIONS) Irp->IoStatus.Information)->Objects,
                  i * sizeof (PDEVICE_OBJECT));
        }
        relations->Count = DeviceData->NumPDOs + i;

        //
        // For each PDO on this bus add a pointer to the device relations
        // buffer, being sure to take out a reference to that object.
        // The PlugPlay system will dereference the object when it is done with
        // it and free the device relations buffer.
        //
        for (entry = DeviceData->PDOs.Flink;
             entry != &DeviceData->PDOs;
             entry = entry->Flink, i++) {

            pdoData = CONTAINING_RECORD (entry, PDO_DEVICE_DATA, Link);
            relations->Objects[i] = pdoData->Self;
            ObReferenceObject (pdoData->Self);
        }

        ExReleaseFastMutex (&DeviceData->Mutex);

        //
        // Set up and pass the IRP further down the stack
        //
        Irp->IoStatus.Status = STATUS_SUCCESS;

        if (0 != Irp->IoStatus.Information) {
            ExFreePool ((PVOID) Irp->IoStatus.Information);
        }
        Irp->IoStatus.Information = (ULONG) relations;

        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (DeviceData->TopOfStack, Irp);

        Game_DecIoCount (DeviceData);
        return status;

    case IRP_MN_QUERY_REMOVE_DEVICE:
        //
        // If we were to fail this call then we would need to complete the
        // IRP here.  Since we are not, set the status to SUCCESS and
        // call the next driver.
        //

        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (DeviceData->TopOfStack, Irp);
        Game_DecIoCount (DeviceData);
        return status;

GAME_FDO_PNP_DEFAULT:
    default:
        //
        // In the default case we merely call the next driver since
        // we don't know what to do.
        //

        //
        // Fire and Forget
        //
        IoSkipCurrentIrpStackLocation (Irp);

        //
        // Done, do NOT complete the IRP, it will be processed by the lower
        // device object, which will complete the IRP
        //

        status = IoCallDriver (DeviceData->TopOfStack, Irp);
        Game_DecIoCount (DeviceData);
        return status;
    }

    Irp->IoStatus.Status = status;
    DbgRaiseIrql (DISPATCH_LEVEL, &irql);
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    DbgLowerIrql (irql);

    Game_DecIoCount (DeviceData);
    return status;
}


NTSTATUS
Game_FDO_PnPComplete (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN PVOID            Context
    )
/*++
Routine Description:
    A completion routine for use when calling the lower device objects to
    which our bus (FDO) is attached.

--*/
{
    UNREFERENCED_PARAMETER (DeviceObject);

    if (Irp->PendingReturned) {
        IoMarkIrpPending( Irp );
    }

    KeSetEvent ((PKEVENT) Context, 1, FALSE);
    // No special priority
    // No Wait

    return STATUS_MORE_PROCESSING_REQUIRED; // Keep this IRP
}

NTSTATUS
Game_PDO_PnP (
    IN PDEVICE_OBJECT       DeviceObject,
    IN PIRP                 Irp,
    IN PIO_STACK_LOCATION   IrpStack,
    IN PPDO_DEVICE_DATA     DeviceData
    )
/*++
Routine Description:
    Handle requests from the PlugPlay system for the devices on the BUS

--*/
{
    PDEVICE_CAPABILITIES    deviceCapabilities;
    ULONG                   information;
    PWCHAR                  buffer, buffer2;
    ULONG                   length, i, j;
    NTSTATUS                status;
    KIRQL                   oldIrq;

    PAGED_CODE ();

    status = Irp->IoStatus.Status;

    //
    // NB: since we are a bus enumerator, we have no one to whom we could
    // defer these irps.  Therefore we do not pass them down but merely
    // return them.
    //

    switch (IrpStack->MinorFunction) {
    case IRP_MN_QUERY_CAPABILITIES:

        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Query Caps \n"));

        //
        // Get the packet.
        //
        deviceCapabilities=IrpStack->Parameters.DeviceCapabilities.Capabilities;

        //
        // Set the capabilities.
        //

        deviceCapabilities->Version = 1;
        deviceCapabilities->Size = sizeof (DEVICE_CAPABILITIES);

        // We cannot wake the system.
        deviceCapabilities->SystemWake = PowerSystemUnspecified;
        deviceCapabilities->DeviceWake = PowerDeviceUnspecified;

        // We have no latencies
        deviceCapabilities->D1Latency = 0;
        deviceCapabilities->D2Latency = 0;
        deviceCapabilities->D3Latency = 0;

        // No locking or ejection
        deviceCapabilities->LockSupported = FALSE;
        deviceCapabilities->EjectSupported = FALSE;

        // Device can be physically removed.
        // Technically there is no physical device to remove, but this bus
        // driver can yank the PDO from the PlugPlay system, when ever it
        // receives an IOCTL_GAMEENUM_REMOVE_PORT device control command.
        deviceCapabilities->Removable = TRUE;

        // not Docking device
        deviceCapabilities->DockDevice = FALSE;

        deviceCapabilities->UniqueID = FALSE;
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_ID:
        // Query the IDs of the device
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE,
                      ("QueryID: 0x%x\n", IrpStack->Parameters.QueryId.IdType));

        switch (IrpStack->Parameters.QueryId.IdType) {

        case BusQueryDeviceID:
            // return a WCHAR (null terminated) string describing the device
            // For symplicity we make it exactly the same as the Hardware ID.
        case BusQueryHardwareIDs:
            // return a multi WCHAR (null terminated) string (null terminated)
            // array for use in matching hardare ids in inf files;
            //

            buffer = DeviceData->HardwareIDs;

            while (*(buffer++)) {
                while (*(buffer++)) {
                    ;
                }
            }
            length = (buffer - DeviceData->HardwareIDs) * sizeof (WCHAR);

            buffer = ExAllocatePool (PagedPool, length);
            if (buffer) {
                RtlCopyMemory (buffer, DeviceData->HardwareIDs, length);
            }
            Irp->IoStatus.Information = (ULONG) buffer;
            break;

        case BusQueryInstanceID:
            // Build an instance ID.  This is what PnP uses to tell if it has
            // seen this thing before or not.  Build it from the first hardware
            // id and the port number.
            //
            // NB since we do not incorperate the port number
            // this method does not produce unique ids;
            //
            buffer = buffer2 = DeviceData->HardwareIDs;

            while (*(buffer++)) {
                while (*(buffer++)) {
                    ;
                }
            }
            while ('\\' != *(buffer2++)) {
                ;
            }
            length = (buffer - buffer2) * sizeof (WCHAR);

            buffer = ExAllocatePool (PagedPool, length);
            if (buffer) {
                RtlCopyMemory (buffer, buffer2, length);
            }
            Irp->IoStatus.Information = (ULONG) buffer;
            break;


        case BusQueryCompatibleIDs:
            // The generic ids for installation of this pdo.

            length = GAMEENUM_COMPATIBLE_IDS_LENGTH * sizeof (WCHAR);
            buffer = ExAllocatePool (PagedPool, length);
            if (buffer) {
                RtlCopyMemory (buffer, GAMEENUM_COMPATIBLE_IDS, length);
            }
            Irp->IoStatus.Information = (ULONG) buffer;
            break;

        }
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_START_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Start Device \n"));
        // Here we do what ever initialization and ``turning on'' that is
        // required to allow others to access this device.
        DeviceData->Started = TRUE;
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_STOP_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Stop Device \n"));
        // Here we shut down the device.  The opposite of start.
        DeviceData->Started = FALSE;
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_REMOVE_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Remove Device \n"));
        //
        // The remove IRP code for a PDO uses the following steps:
        //
        // � Complete any requests queued in the driver
        // � If the device is still attached to the system,
        //   then complete the request and return.
        // � Otherwise, cleanup device specific allocations, memory, events...
        // � Call IoDeleteDevice
        // � Return from the dispatch routine.
        //
        status= Game_PnPRemove(DeviceObject, DeviceData);
        break;

    case IRP_MN_QUERY_STOP_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Q Stop Device \n"));
        // No reason here why we can't stop the device.
        // If there were a reason we should speak now for answering success
        // here may result in a stop device irp.
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_STOP_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Cancel Stop Device \n"));
        //
        // The stop was canceled.  Whatever state we set, or resources we put
        // on hold in anticipation of the forcoming STOP device IRP should be
        // put back to normal.  Someone, in the long list of concerned parties,
        // has failed the stop device query.
        //
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_REMOVE_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Q Remove Device \n"));
        //
        // Just like Query Stop only now the impending doom is the remove irp
        //
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_REMOVE_DEVICE:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE, ("Can Remove Device \n"));
        //
        // Clean up a remove that did not go through, just like cancel STOP.
        //
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
    case IRP_MN_QUERY_DEVICE_RELATIONS:
    case IRP_MN_READ_CONFIG:
    case IRP_MN_WRITE_CONFIG: // we have no config space
    case IRP_MN_EJECT:
    case IRP_MN_SET_LOCK:
    case IRP_MN_QUERY_INTERFACE: // We do not have any non IRP based interfaces.
    default:
        Game_KdPrint (DeviceData, GAME_DBG_PNP_TRACE,
                      ("PNP Not handled 0x%x\n", IrpStack->MinorFunction));
        // this is a leaf node
        // status = STATUS_NOT_IMPLEMENTED
        // For PnP requests to the PDO that we do not understand we should
        // return the IRP WITHOUT setting the status or information fields.
        // They may have already been set by a filter (eg acpi).
        break;
    }

    Irp->IoStatus.Status = status;
    DbgRaiseIrql (DISPATCH_LEVEL, &oldIrq);
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    DbgLowerIrql (oldIrq);

    return status;
}

NTSTATUS
Game_PnPRemove (
    PDEVICE_OBJECT      Device,
    PPDO_DEVICE_DATA    PdoData
    )
/*++
Routine Description:
    The PlugPlay subsystem has instructed that this PDO should be removed.

    We should therefore
    � Complete any requests queued in the driver
    � If the device is still attached to the system,
      then complete the request and return.
    � Otherwise, cleanup device specific allocations, memory, events...
    � Call IoDeleteDevice
    � Return from the dispatch routine.

    Note that if the device is still connected to the bus (IE in this case
    the control panel has not yet told us that the game device has disappeared)
    then the PDO must remain around, and must be returned during any
    query Device relaions IRPS.

--*/

{
    PdoData->Removed = TRUE;

    //
    // Complete any outsanding requests with STATUS_DELETE_PENDING.
    //
    // Game enum does not queue any irps at this time so we have nothing to do.
    //
    if (PdoData->Attached) {
        return STATUS_SUCCESS;
    }

    //
    // Free any resources.
    //
    ExFreePool (PdoData->HardwareIDs);

    Game_KdPrint(PdoData, GAME_DBG_PNP_INFO,
                        ("IoDeleteDevice2: 0x%x\n", Device));
    IoDeleteDevice (Device);
    return STATUS_SUCCESS;
}

NTSTATUS
Game_StartFdo (
    IN  PFDO_DEVICE_DATA            FdoData,
    IN  PCM_PARTIAL_RESOURCE_LIST   PartialResourceList,
    IN  PCM_PARTIAL_RESOURCE_LIST   PartialResourceListTranslated
    )
/*++
--*/
{
    ULONG i;
    NTSTATUS status = STATUS_SUCCESS;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resourceTrans;

    Game_KdPrint (FdoData, GAME_DBG_PNP_TRACE, ("StartFdo\n"));

    for (i = 0,
         resource = &PartialResourceList->PartialDescriptors[0],
         resourceTrans = &PartialResourceListTranslated->PartialDescriptors[0];

         i < PartialResourceList->Count && NT_SUCCESS(status);
         i++, resource++, resourceTrans++) {

        switch (resource->Type) {
        case CmResourceTypePort:

            FdoData->ReadPort = READ_PORT_UCHAR;
            FdoData->WritePort = WRITE_PORT_UCHAR;
            FdoData->PhysicalAddress = resource->u.Port.Start;
            Game_KdPrint (FdoData, GAME_DBG_PNP_INFO,
                          ("HardwareResource: Port (%x) ->",
                           FdoData->PhysicalAddress.LowPart));

            switch (resourceTrans->Type) {
            case CmResourceTypePort:


                // Nothing to do here but note the address;
                FdoData->GamePortAddress =
                    (PVOID) resourceTrans->u.Port.Start.LowPart;

                ASSERT (resourceTrans->u.Port.Length == resource->u.Port.Length);
                FdoData->GamePortAddressLength = resourceTrans->u.Port.Length;

                Game_KdPrint_Cont (FdoData, GAME_DBG_PNP_INFO,
                                   ("Port: (%x)\n", FdoData->GamePortAddress));
                break;

            case CmResourceTypeMemory:
                //
                // We need to map the memory
                //

                FdoData->GamePortAddress =
                    MmMapIoSpace (resourceTrans->u.Memory.Start,
                                  resourceTrans->u.Memory.Length,
                                  MmNonCached);

                ASSERT (resourceTrans->u.Port.Length == resource->u.Port.Length);
                FdoData->GamePortAddressLength = resourceTrans->u.Memory.Length;

                FdoData->MappedPorts = TRUE;

                Game_KdPrint_Cont (FdoData, GAME_DBG_PNP_INFO,
                                   ("Mem: (%x)\n", FdoData->GamePortAddress));
                break;

            default:
                Game_KdPrint_Cont (FdoData, GAME_DBG_PNP_INFO,
                                   ("Unknown \n", FdoData->GamePortAddress));
                TRAP ();
            }

            break;

        case CmResourceTypeMemory:

            ASSERT (CmResourceTypeMemory == resourceTrans->Type);

            FdoData->ReadPort = READ_REGISTER_UCHAR;
            FdoData->WritePort = WRITE_REGISTER_UCHAR;
            FdoData->PhysicalAddress = resource->u.Memory.Start;
            FdoData->GamePortAddress =
                MmMapIoSpace (resourceTrans->u.Memory.Start,
                              resourceTrans->u.Memory.Length,
                              MmNonCached);

            FdoData->MappedPorts = TRUE;

            ASSERT (1 == resource->u.Memory.Length);

            Game_KdPrint (FdoData, GAME_DBG_PNP_INFO,
                          ("HardwareResource: Memory (%x) -> Mem (%x)",
                           FdoData->PhysicalAddress.LowPart,
                           FdoData->GamePortAddress));

            break;

        case CmResourceTypeInterrupt:
        default:
            // Hun?

            Game_KdPrint (FdoData, GAME_DBG_PNP_ERROR,
                          ("Unhandled resource type (0x%x)\n",
                           resource->Type));
            status = STATUS_UNSUCCESSFUL;
        }
    }
    return status;
}

NTSTATUS
Game_Expose (
    PGAMEENUM_EXPOSE_HARDWARE   Expose,
    ULONG                       ExposeSize,
    PFDO_DEVICE_DATA            FdoData
    )
/*++
Routine Description:
    This driver has just detected a new device on the bus.  (Actually the
    control panels has just told us that something has arived, but who is
    counting?)

    We therefore need to create a new PDO, initialize it, add it to the list
    of PDOs for this FDO bus, and then tell Plug and Play that all of this
    happened so that it will start sending prodding IRPs.
--*/
{
    UNICODE_STRING      pdoUniName;
    WCHAR               pdoName[] = GAMEENUM_PDO_NAME_BASE;
    UCHAR               nameIndex, i;
    PDEVICE_OBJECT      pdo;
    PPDO_DEVICE_DATA    pdoData;
    NTSTATUS            status = STATUS_SUCCESS;
    ULONG               length;
    PWCHAR              id;
    KIRQL               irql;
    PVOID               handle = NULL;
    BOOLEAN             first = TRUE;

    PAGED_CODE ();

    if (FdoData->Self != Expose->PortHandle) {
        return STATUS_INVALID_PARAMETER;
    }

    length = (ExposeSize - sizeof (GAMEENUM_EXPOSE_HARDWARE)) / sizeof (WCHAR);
    Game_KdPrint (FdoData, GAME_DBG_PNP_INFO, // Eliyas 4/21/97
                  ("Exposing PDO\n"
                   "======PortHandle: 0x%x\n"
                   "======HardwareHandle: 0x%x\n"
                   "======NumJoysticks: %d\n"
                   "======NumAxis: %d\n"
                   "======HardwareId: %ws\n"
                   "======Length: %d\n",
                   Expose->PortHandle,
                   Expose->HardwareHandle,
                   Expose->NumberJoysticks,
                   Expose->NumberAxis,
                   Expose->HardwareIDs,
                   length));

    if ((L'\0' != Expose->HardwareIDs[length - 1]) ||
        (L'\0' != Expose->HardwareIDs[length - 2])) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Create the PDOs
    //

    RtlInitUnicodeString (&pdoUniName, pdoName);
    nameIndex = 0;

    for (i=0; i<Expose->NumberJoysticks; i++) {
        do {
            pdoName [((sizeof(pdoName)/sizeof(WCHAR)) - 2)] = L'0' + nameIndex++;
            Game_KdPrint (FdoData, GAME_DBG_PNP_INFO,
                  ("PDO Name: %ws\n",
                   pdoName));

            status = IoCreateDevice(FdoData->Self->DriverObject,
                                    sizeof (PDO_DEVICE_DATA),
                                    &pdoUniName,
                                    FILE_DEVICE_BUS_EXTENDER,
                                    0,
                                    FALSE,
                                    &pdo);
        } while (STATUS_OBJECT_NAME_COLLISION == status);

        if (!NT_SUCCESS (status)) {
            return status; // BUGBUG memory leak (Device objects)
        }

        if (!handle) {
            Expose->HardwareHandle = handle = pdo;
        }

        pdoData = (PPDO_DEVICE_DATA) pdo->DeviceExtension;

        //
        // Copy the hardware IDs
        //

        length *= sizeof (WCHAR);
        if (NULL == (pdoData->HardwareIDs = ExAllocatePool(NonPagedPool, length))) {
            // BUGBUG memory leaks;
            return STATUS_INSUFFICIENT_RESOURCES;
        }
        RtlCopyMemory (pdoData->HardwareIDs, Expose->HardwareIDs, length);

        //
        // Initialize the rest
        //
        pdoData->IsFDO = FALSE;
        pdoData->DebugLevel = GAME_DEFAULT_DEBUG_OUTPUT_LEVEL;
        pdoData->Self = pdo;

        pdoData->ParrentFdo = FdoData->Self;


        pdoData->Started = FALSE; // irp_mn_start has yet to be received
        pdoData->Attached = TRUE; // attached to the bus
        pdoData->Removed = FALSE; // no irp_mn_remove as of yet
        pdoData->NumberAxis = Expose->NumberAxis;
        if (1 == Expose->NumberJoysticks) {
            pdoData->Portion = GameenumWhole;
        } else if (2 == Expose->NumberJoysticks) {
            if (first) {
                pdoData->Portion = GameenumFirstHalf;
                first = FALSE;
            } else {
                pdoData->Portion = GameenumSecondHalf;
            }
        } else {
            return STATUS_INVALID_PARAMETER; // BUGBUG memory leak;
        }


        pdo->Flags &= ~DO_DEVICE_INITIALIZING;
        pdo->Flags |= DO_POWER_PAGABLE;

        ExAcquireFastMutex (&FdoData->Mutex);
        InsertTailList(&FdoData->PDOs, &pdoData->Link);
        FdoData->NumPDOs++;
        ExReleaseFastMutex (&FdoData->Mutex);
    }

    IoInvalidateDeviceRelations (FdoData->UnderlyingPDO, BusRelations);

    return status;
}


NTSTATUS
Game_Remove (
    PGAMEENUM_REMOVE_HARDWARE   Remove,
    PFDO_DEVICE_DATA            FdoData
    )
/*++
Routine Description:
    This driver has just detected that a device has departed from the bus.
    (Actually the control panels has just told us that something has departed,
    but who is counting?

    We therefore need to flag the PDO as no longer attached, remove it from
    the linked list of PDOs for this bus, and then tell Plug and Play about it.
--*/
{
    PLIST_ENTRY         entry;
    PPDO_DEVICE_DATA    pdoData;
    BOOLEAN             found = FALSE;
    PVOID               handle = NULL;

    ExAcquireFastMutex (&FdoData->Mutex);

    for (entry = FdoData->PDOs.Flink;
         entry != &FdoData->PDOs;
         entry = entry->Flink) {

        pdoData = CONTAINING_RECORD (entry, PDO_DEVICE_DATA, Link);
        if (!handle) {
            handle = pdoData->Self;
        }
        if (handle == Remove->HardwareHandle) {
            pdoData->Attached = FALSE;
            RemoveEntryList (&pdoData->Link);
            FdoData->NumPDOs--;
            found = TRUE;
        }
    }
    ExReleaseFastMutex (&FdoData->Mutex);

    if (found) {
        IoInvalidateDeviceRelations (FdoData->UnderlyingPDO, BusRelations);
        return STATUS_SUCCESS;
    }
    return STATUS_INVALID_PARAMETER;
}


NTSTATUS
Game_ListPorts (
    PGAMEENUM_PORT_DESC Desc,
    PFDO_DEVICE_DATA    FdoData
    )
/*++
Routine Description:
    This driver has just detected that a device has departed from the bus.
    (Actually the control panels has just told us that something has departed,
    but who is counting?

    We therefore need to flag the PDO as no longer attached, remove it from
    the linked list of PDOs for this bus, and then tell Plug and Play about it.
--*/
{
    Desc->PortHandle = FdoData->Self;
    Desc->PortAddress = FdoData->PhysicalAddress;

    return STATUS_SUCCESS;
}

NTSTATUS
Game_Power (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++
    We do nothing special for power;

--*/
{
    PIO_STACK_LOCATION  irpStack;
    NTSTATUS            status;
    PCOMMON_DEVICE_DATA commonData;

    PAGED_CODE ();

    status = STATUS_SUCCESS;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    ASSERT (IRP_MJ_POWER == irpStack->MajorFunction);

    commonData = (PCOMMON_DEVICE_DATA) DeviceObject->DeviceExtension;

    if (commonData->IsFDO) {
        status = Game_FDO_Power ((PFDO_DEVICE_DATA) DeviceObject->DeviceExtension,
                                Irp);
    } else {
        status = Game_PDO_Power ((PPDO_DEVICE_DATA) DeviceObject->DeviceExtension,
                                Irp);
    }

    return status;
}


NTSTATUS
Game_PowerComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
Game_FDO_Power (
    PFDO_DEVICE_DATA    Data,
    PIRP                Irp
    )
{
    NTSTATUS            status;
    BOOLEAN             hookit = FALSE;
    POWER_STATE         powerState;
    POWER_STATE_TYPE    powerType;
    PIO_STACK_LOCATION  stack;

    stack = IoGetCurrentIrpStackLocation (Irp);
    powerType = stack->Parameters.Power.Type;
    powerState = stack->Parameters.Power.State;

    PAGED_CODE ();

    status = Game_IncIoCount (Data);
    if (!NT_SUCCESS (status)) {
        PoStartNextPowerIrp (Irp);
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    switch (stack->MinorFunction) {
    case IRP_MN_SET_POWER:
        Game_KdPrint(Data,
                     GAME_DBG_PNP_TRACE,
                     ("Game-PnP Setting %s state to %d\n",
                      ((powerType == SystemPowerState) ?  "System" : "Device"),
                      powerState.SystemState));

        switch (powerType) {
        case DevicePowerState:

            status = Irp->IoStatus.Status = STATUS_SUCCESS;

            if (Data->DeviceState == powerState.DeviceState) {
                break;

            } else if (Data->DeviceState < powerState.DeviceState) {
                //
                // Powering down
                //

                //
                // BUGBUG I need to check to see that the PDOs are powered off
                // before I power off the bus. (AKA transition D0 => ~D0)
                //

                PoSetPowerState (Data->Self, powerType, powerState);
                Data->DeviceState = powerState.DeviceState;

            } else {
                //
                // Powering Up
                //
                hookit = TRUE;
            }

            break;

        case SystemPowerState:
            if (Data->PowerQueryLock) {
                //
                // The reception of a power irp resolves the query lock.
                //
                Data->PowerQueryLock = FALSE;
            } else {
                ASSERT (Data->SystemState != powerState.SystemState);
            }

            status = Irp->IoStatus.Status = STATUS_SUCCESS;

            if (Data->SystemState == powerState.SystemState) {
                break;

            } else if (Data->SystemState < powerState.SystemState) {
                //
                // Powering down
                //

                Data->SystemState = powerState.SystemState;

                powerState.DeviceState = PowerDeviceD3;
                PoRequestPowerIrp (Data->Self,
                                   IRP_MN_SET_POWER,
                                   powerState,
                                   NULL, // no completion function
                                   NULL, // and no context
                                   NULL);

            } else {
                //
                // Powering Up
                //
                hookit = TRUE;
            }
            break;
        }

        break;

    case IRP_MN_QUERY_POWER:
        //
        Data->PowerQueryLock = TRUE;
        status = Irp->IoStatus.Status = STATUS_SUCCESS;
        break;

    default:
        break;
    }

    IoCopyCurrentIrpStackLocationToNext (Irp);

    if (hookit) {
        status = Game_IncIoCount (Data);
        ASSERT (STATUS_SUCCESS == status);
        IoSetCompletionRoutine (Irp,
                                Game_PowerComplete,
                                NULL,
                                TRUE,
                                TRUE,
                                TRUE);
        //
        // NOTE!!! PoCallDriver NOT IoCallDriver.
        //
        status = PoCallDriver (Data->TopOfStack, Irp);

    } else {
        //
        // Power IRPS come synchronously; drivers must call
        // PoStartNextPowerIrp, when they are ready for the next power
        // irp.  This can be called here, or in the completetion
        // routine, but never the less must be called.
        //
        PoStartNextPowerIrp (Irp);

        status =  PoCallDriver (Data->TopOfStack, Irp);
    }

    Game_DecIoCount (Data);
    return status;
}

NTSTATUS
Game_PowerComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
{
    POWER_STATE         powerState;
    POWER_STATE_TYPE    powerType;
    PIO_STACK_LOCATION  stack;
    PFDO_DEVICE_DATA    data;

    UNREFERENCED_PARAMETER (Context);

    data = (PFDO_DEVICE_DATA) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation (Irp);
    powerType = stack->Parameters.Power.Type;
    powerState = stack->Parameters.Power.State;

    switch (stack->MinorFunction) {
    case IRP_MN_SET_POWER:
        switch (powerType) {
        case DevicePowerState:
            ASSERT (powerState.DeviceState < data->DeviceState);
            data->DeviceState = powerState.DeviceState;

            PoSetPowerState (data->Self, powerType, powerState);
            break;

        case SystemPowerState:
            //
            // Ususally the work of requesting the Device Power IRP on
            // behalf of the SystemPower Irp is work done by the Function
            // (FDO) driver.  In order, however that Joystick function drivers
            // have a more simplified power code path (AKA they merely need
            // pass on ALL power IRPS) will will do this work for them in the
            // PDO.
            //
            // NB: This assumes that we will never have any "clever" power
            // management for a gaming device attached through a legacy
            // gaming port.  By which I mean that the HIDGame driver will not
            // be able to select a "D" state based on the "S" state; as it is
            // done for the HidGame driver.
            //
            // Any yahoo putting wakeup capabilities into a legacy joystick
            // should be shot.  It will require special hardware.  If you are
            // adding extra hardware then you should not be doing so to this
            // nasty RC circuit.
            //

            ASSERT (powerState.SystemState < data->SystemState);
            data->SystemState = powerState.SystemState;

            powerState.DeviceState = PowerDeviceD0;
            PoRequestPowerIrp (data->Self,
                               IRP_MN_SET_POWER,
                               powerState,
                               NULL, // no completion function
                               NULL, // and no context
                               NULL);
            break;
        }
        break;

    case IRP_MN_QUERY_POWER:



        ASSERT (IRP_MN_QUERY_POWER != stack->MinorFunction);
        break;

    default:
        ASSERT (0xBADBAD == IRP_MN_QUERY_POWER);
        break;
    }


    PoStartNextPowerIrp (Irp);
    Game_DecIoCount (data);

    return STATUS_SUCCESS;
}


NTSTATUS
Game_PDO_Power (
    PPDO_DEVICE_DATA    PdoData,
    PIRP                Irp
    )
{
    KIRQL               irql;
    NTSTATUS            status = STATUS_SUCCESS;
    PIO_STACK_LOCATION  stack;
    POWER_STATE         powerState;
    POWER_STATE_TYPE    powerType;

    stack = IoGetCurrentIrpStackLocation (Irp);
    powerType = stack->Parameters.Power.Type;
    powerState = stack->Parameters.Power.State;

    switch (stack->MinorFunction) {
    case IRP_MN_SET_POWER:
        switch (powerType) {
        case DevicePowerState:
            PoSetPowerState (PdoData->Self, powerType, powerState);
            PdoData->DeviceState = powerState.DeviceState;
            break;

        case SystemPowerState:
            if (PowerSystemWorking == powerState.SystemState) {

                powerState.DeviceState = PowerDeviceD0;
                PoRequestPowerIrp (PdoData->Self,
                                   IRP_MN_SET_POWER,
                                   powerState,
                                   NULL, // no completion
                                   NULL, // no context
                                   NULL); // no return IRP
            } else {

                powerState.DeviceState = PowerDeviceD3;
                PoRequestPowerIrp (PdoData->Self,
                                   IRP_MN_SET_POWER,
                                   powerState,
                                   NULL, // no completion
                                   NULL, // no context
                                   NULL); // no return IRP
            }
            PdoData->SystemState = powerState.SystemState;
            break;

        default:
            TRAP ();
            status = STATUS_NOT_IMPLEMENTED;
            break;
        }
        break;

    case IRP_MN_WAIT_WAKE:
    case IRP_MN_POWER_SEQUENCE:
    case IRP_MN_QUERY_POWER:
    default:
        status = STATUS_NOT_IMPLEMENTED;
        break;
    }

    PoStartNextPowerIrp (Irp);
    Irp->IoStatus.Status = status;
    DbgRaiseIrql (DISPATCH_LEVEL, &irql);
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    DbgLowerIrql (irql);

    return status;
}


