/*++

Copyright (c) 1998  Microsoft Corporation

Module Name: 

    1394api.c

Abstract


Author:

    Peter Binder (pbinder) 5/07/98

Revision History:
Date     Who       What
-------- --------- ------------------------------------------------------------
5/07/98  pbinder   birth
--*/

// todo:
//      t1394_Control
//      t1394_GetMaxSpeedBetweenDevices
//      t1394_GetConfigurationInformation
//      t1394_SendPhyConfigurationPacket
//      t1394_GetSpeedTopologyMaps
//      t1394_BusResetNotification

NTSTATUS
t1394_GetLocalHostInformation(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN ULONG            nLevel,
    IN OUT PULONG       UserStatus,
    IN OUT PVOID        Information
    )
{
    NTSTATUS                ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION       deviceExtension = DeviceObject->DeviceExtension;
    PIRB                    pIrb;
    GET_LOCAL_HOST_INFO6    LocalHostInfo6;
    GET_LOCAL_HOST_INFO5    LocalHostInfo5;

    PIRP                    newIrp;
    BOOLEAN                 allocNewIrp = FALSE;
    KEVENT                  Event;
    IO_STATUS_BLOCK         ioStatus;
    
    ENTER("t1394_GetLocalHostInformation");

    TRACE(TL_TRACE, ("nLevel = 0x%x\n", nLevel));
    TRACE(TL_TRACE, ("Information = 0x%x\n", Information));

    //
    // If this is a UserMode request create a newIrp so that the request
    // will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_GetLocalHostInformation;            
        }
        allocNewIrp = TRUE;
    }
    
    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));

    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_GetLocalHostInformation;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_GET_LOCAL_HOST_INFO;
    pIrb->Flags = 0;
    pIrb->u.GetLocalHostInformation.nLevel = nLevel;

    // in the case of nLevel = 6, we need to reset Information,
    // because we have a variable sized data buffer being passed in
    // from user mode.
    if (nLevel == 6) {

        RtlCopyMemory(&LocalHostInfo6, Information, sizeof (GET_LOCAL_HOST_INFO6));
        LocalHostInfo6.CsrDataBuffer = &((PGET_LOCAL_HOST_INFO6)Information)->CsrDataBuffer;

#ifdef _WIN64
        // need to special case this in 64 bit because data padding causes an off by 1 error
        // since pointers are 64 bits, there is a 32 bit space between the CsrDataLength and CsrDataBuffer
        (PCHAR)LocalHostInfo6.CsrDataBuffer -= sizeof (ULONG);       
#endif

        pIrb->u.GetLocalHostInformation.Information = &LocalHostInfo6;

        TRACE(TL_TRACE, ("CsrBaseAddress.Off_High = 0x%x\n", LocalHostInfo6.CsrBaseAddress.Off_High));
        TRACE(TL_TRACE, ("CsrBaseAddress.Off_Low = 0x%x\n", LocalHostInfo6.CsrBaseAddress.Off_Low));
        TRACE(TL_TRACE, ("CsrDataLength = 0x%x\n", LocalHostInfo6.CsrDataLength));
        TRACE(TL_TRACE, ("CsrDataBuffer = 0x%p\n", LocalHostInfo6.CsrDataBuffer));
    }
    else if (nLevel == 5)
    {
        // need to ensure that no one passed us an GET_LOCAL_HOST_INFO5 structure that just 
        // piggy backed on the GET_LOCAL_HOST_INFORMATION struct, if they did this will cause
        // data misalignment errors in 64 bit because 1394bus will be expecting that structure to
        // be aligned correctly
        RtlCopyMemory (&LocalHostInfo5, Information, sizeof (GET_LOCAL_HOST_INFO5));
        pIrb->u.GetLocalHostInformation.Information = &LocalHostInfo5;
    
        TRACE(TL_TRACE, ("ConfigRom = 0x%p\n", LocalHostInfo5.ConfigRom));
        TRACE(TL_TRACE, ("ConfigRomLength = 0x%x\n", LocalHostInfo5.ConfigRomLength));
    }   
    else {

        pIrb->u.GetLocalHostInformation.Information = Information;
    }

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
    
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }
    
    if (!NT_SUCCESS(ntStatus)) {

        if (ntStatus == STATUS_INVALID_BUFFER_SIZE) {

            // we have to set status=success, otherwise, we don't get
            // the size value passed up to ring 3.
            ntStatus = STATUS_SUCCESS;
            *UserStatus = 122;

            TRACE(TL_WARNING, ("Invalid Buffer Size. nLevel = 0x%x\n", nLevel));

            // see if it's a config rom request or a csr request
            if (nLevel == 5) {

                // it is, lets print out the size                
                TRACE(TL_TRACE, ("ConfigRomLength = 0x%x\n", LocalHostInfo5.ConfigRomLength));

                // neet to set this in the information struct we were passed down
                RtlCopyMemory(Information, &LocalHostInfo5, sizeof (GET_LOCAL_HOST_INFO5));
            }
            else if (nLevel == 6) {

                // csr, print out size
                TRACE(TL_TRACE, ("CsrDataLength needed = 0x%x\n", LocalHostInfo6.CsrDataLength));

                // need to set this in the information struct we were passed down
                RtlCopyMemory(Information, &LocalHostInfo6, sizeof (GET_LOCAL_HOST_INFO6));
            }
        }
        else {

            TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
            TRAP;
        }
    }
    else {

        *UserStatus = 0;

        if (nLevel == 1) {

            PGET_LOCAL_HOST_INFO1   LocalHostInfo1;

            LocalHostInfo1 = (PGET_LOCAL_HOST_INFO1)Information;

            TRACE(TL_TRACE, ("UniqueId.LowPart = 0x%x\n", LocalHostInfo1->UniqueId.LowPart));
            TRACE(TL_TRACE, ("UniqueId.HighPart = 0x%x\n", LocalHostInfo1->UniqueId.HighPart));
        }
        else if (nLevel == 2) {

            PGET_LOCAL_HOST_INFO2   LocalHostInfo2;

            LocalHostInfo2 = (PGET_LOCAL_HOST_INFO2)Information;

            TRACE(TL_TRACE, ("HostCapabilities = 0x%x\n", LocalHostInfo2->HostCapabilities));
            TRACE(TL_TRACE, ("MaxAsyncReadRequest = 0x%x\n", LocalHostInfo2->MaxAsyncReadRequest));
            TRACE(TL_TRACE, ("MaxAsyncWriteRequest = 0x%x\n", LocalHostInfo2->MaxAsyncWriteRequest));
        }
        else if (nLevel == 3) {

            PGET_LOCAL_HOST_INFO3   LocalHostInfo3;

            LocalHostInfo3 = (PGET_LOCAL_HOST_INFO3)Information;

            TRACE(TL_TRACE, ("deciWattsSupplied = 0x%x\n", LocalHostInfo3->deciWattsSupplied));
            TRACE(TL_TRACE, ("Voltage = 0x%x\n", LocalHostInfo3->Voltage));
        }
        else if (nLevel == 4) {

            PGET_LOCAL_HOST_INFO4   LocalHostInfo4;

            LocalHostInfo4 = (PGET_LOCAL_HOST_INFO4)Information;

            TRACE(TL_TRACE, ("Context = 0x%x\n", LocalHostInfo4->Context));
        }
        else if (nLevel == 5) {

            PGET_LOCAL_HOST_INFO5   LocalHostInfo5;

            LocalHostInfo5 = (PGET_LOCAL_HOST_INFO5)Information;

            TRACE(TL_TRACE, ("ConfigRomLength = 0x%x\n", LocalHostInfo5->ConfigRomLength));

            // IDEAIDEA: add some debug spew for whatever we are getting...
        }
        else if (nLevel == 6) {

            // IDEAIDEA: add some debug spew for whatever we are getting...

        }
        else {

            TRACE(TL_WARNING, ("Returned success on invalid nLevel = 0x%x\n", nLevel));
        }
    }

    if (pIrb)
        ExFreePool(pIrb);

Exit_GetLocalHostInformation:

    if (allocNewIrp) 
        Irp->IoStatus = ioStatus;
        
    EXIT("t1394_GetLocalHostInformation", ntStatus);
    return(ntStatus);
} // t1394_GetLocalHostInformation

NTSTATUS
t1394_Get1394AddressFromDeviceObject(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN ULONG            fulFlags,
    OUT PNODE_ADDRESS   pNodeAddress
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PIRB                pIrb;

    PIRP                newIrp;
    BOOLEAN             allocNewIrp = FALSE;
    KEVENT              Event;
    IO_STATUS_BLOCK     ioStatus;
    
    ENTER("t1394_Get1394AddressFromDeviceObject");

    TRACE(TL_TRACE, ("fulFlags = 0x%x\n", fulFlags));

    //
    // If this is a UserMode request create a newIrp so that the request
    // will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_Get1394AddressFromDeviceObject;            
        }
        allocNewIrp = TRUE;
    }
    
    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));

    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_Get1394AddressFromDeviceObject;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_GET_ADDR_FROM_DEVICE_OBJECT;
    pIrb->Flags = 0;
    pIrb->u.Get1394AddressFromDeviceObject.fulFlags = fulFlags;

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }
    
    if (NT_SUCCESS(ntStatus)) {

        *pNodeAddress = pIrb->u.Get1394AddressFromDeviceObject.NodeAddress;

        TRACE(TL_TRACE, ("NodeAddres.NA_Node_Number = 0x%x\n", pNodeAddress->NA_Node_Number));
        TRACE(TL_TRACE, ("NodeAddres.NA_Bus_Number = 0x%x\n", pNodeAddress->NA_Bus_Number));
    }
    else {

        TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        TRAP;
    }

    ExFreePool(pIrb);

Exit_Get1394AddressFromDeviceObject:

    if (allocNewIrp) 
        Irp->IoStatus = ioStatus;
        
    EXIT("t1394_Get1394AddressFromDeviceObject", ntStatus);
    return(ntStatus);
} // t1394_Get1394AddressFromDeviceObject

NTSTATUS
t1394_Control(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;

    ENTER("t1394_Control");

    ntStatus = STATUS_NOT_IMPLEMENTED;

    EXIT("t1394_Control", ntStatus);
    return(ntStatus);
} // t1394_Control

NTSTATUS
t1394_GetMaxSpeedBetweenDevices(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN ULONG            fulFlags,
    IN ULONG            ulNumberOfDestinations,
    IN PDEVICE_OBJECT   hDestinationDeviceObjects[64],
    OUT PULONG          fulSpeed
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PIRB                pIrb;
    ULONG               i;

    PIRP                newIrp;
    BOOLEAN             allocNewIrp = FALSE;
    KEVENT              Event;
    IO_STATUS_BLOCK     ioStatus;
    
    ENTER("t1394_GetMaxSpeedBetweenDevices");

    TRACE(TL_TRACE, ("DeviceObject = 0x%x\n", DeviceObject));
    TRACE(TL_TRACE, ("Irp = 0x%x\n", Irp));
    TRACE(TL_TRACE, ("fulFlags = 0x%x\n", fulFlags));
    TRACE(TL_TRACE, ("ulNumberOfDestinations = 0x%x\n", ulNumberOfDestinations));

    for (i=0; i<ulNumberOfDestinations; i++) {

        TRACE(TL_TRACE, ("hDestinationDeviceObjects[%d] = 0x%x\n", i, hDestinationDeviceObjects[i]));
    }

    //
    // If this is a UserMode request create a newIrp so that the request
    // will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_GetMaxSpeedBetweenDevices;            
        }
        allocNewIrp = TRUE;
    }
    
    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));
    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_GetMaxSpeedBetweenDevices;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_GET_SPEED_BETWEEN_DEVICES;
    pIrb->Flags = 0;
    pIrb->u.GetMaxSpeedBetweenDevices.fulFlags = fulFlags;
    pIrb->u.GetMaxSpeedBetweenDevices.ulNumberOfDestinations = ulNumberOfDestinations;

    for (i=0; i<ulNumberOfDestinations; i++) {
    
        pIrb->u.GetMaxSpeedBetweenDevices.hDestinationDeviceObjects[i] = hDestinationDeviceObjects[i];
    }

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }
    
    if (NT_SUCCESS(ntStatus)) {

        *fulSpeed = pIrb->u.GetMaxSpeedBetweenDevices.fulSpeed;

        TRACE(TL_TRACE, ("fulSpeed = 0x%x\n", *fulSpeed));
    }
    else {

        TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        TRAP;
    }

    if (pIrb)
        ExFreePool(pIrb);

Exit_GetMaxSpeedBetweenDevices:

    if (allocNewIrp) 
        Irp->IoStatus = ioStatus;
        
    EXIT("t1394_GetMaxSpeedBetweenDevices", ntStatus);
    return(ntStatus);
} // t1394_GetMaxSpeedBetweenDevices

NTSTATUS
t1394_SetDeviceXmitProperties(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN ULONG            fulSpeed,
    IN ULONG            fulPriority
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PIRB                pIrb;

    PIRP                newIrp;
    BOOLEAN             allocNewIrp = FALSE;
    KEVENT              Event;
    IO_STATUS_BLOCK     ioStatus;
    
    ENTER("t1394_SetDeviceXmitProperties");

    TRACE(TL_TRACE, ("fulSpeed = 0x%x\n", fulSpeed));
    TRACE(TL_TRACE, ("fulPriority = 0x%x\n", fulPriority));

    //
    // If this is a UserMode request create a newIrp so that the request
    // will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_SetDeviceXmitProperties;            
        }
        allocNewIrp = TRUE;
    }
    
    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));
    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_SetDeviceXmitProperties;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_SET_DEVICE_XMIT_PROPERTIES;
    pIrb->Flags = 0;
    pIrb->u.SetDeviceXmitProperties.fulSpeed = fulSpeed;
    pIrb->u.SetDeviceXmitProperties.fulPriority = fulPriority;

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }
    
    if (!NT_SUCCESS(ntStatus)) {

        TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        TRAP;
    }

    ExFreePool(pIrb);

Exit_SetDeviceXmitProperties:

    if (allocNewIrp) 
        Irp->IoStatus = ioStatus;
        
    EXIT("t1394_SetDeviceXmitProperties", ntStatus);
    return(ntStatus);
} // t1394_SetDeviceXmitProperties

NTSTATUS
t1394_GetConfigurationInformation(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PIRB                pIrb;

    PIRP                newIrp;
    BOOLEAN             allocNewIrp = FALSE;
    KEVENT              Event;
    IO_STATUS_BLOCK     ioStatus;
    
    ENTER("t1394_GetConfigurationInformation");
/*
        struct {
            PCONFIG_ROM     ConfigRom;                          // Pointer to config rom
            ULONG           UnitDirectoryBufferSize;
            PVOID           UnitDirectory;                      // Pointer to unit directory
            IO_ADDRESS      UnitDirectoryLocation;              // Starting Location of Unit Directory
            ULONG           UnitDependentDirectoryBufferSize;
            PVOID           UnitDependentDirectory;
            IO_ADDRESS      UnitDependentDirectoryLocation;
            ULONG           VendorLeafBufferSize;               // Size available to get vendor leafs
            PTEXTUAL_LEAF   VendorLeaf;                         // Pointer to vendor leafs
            ULONG           ModelLeafBufferSize;                // Size available to get model leafs
            PTEXTUAL_LEAF   ModelLeaf;                          // Pointer to model leafs
        } GetConfigurationInformation;
*/
    return(STATUS_NOT_IMPLEMENTED);

    //
    // If this is a UserMode request create a newIrp so that the request
    // will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_GetConfigurationInformation;            
        }
        allocNewIrp = TRUE;
    }
    
    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));
    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_GetConfigurationInformation;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_GET_CONFIGURATION_INFO;
    pIrb->Flags = 0;

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }
    
    if (NT_SUCCESS(ntStatus)) {

        TRACE(TL_TRACE, ("ConfigRom->CR_Info = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ConfigRom->CR_Info));
        TRACE(TL_TRACE, ("ConfigRom->CR_Signiture = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ConfigRom->CR_Signiture));
        TRACE(TL_TRACE, ("ConfigRom->CR_BusInfoBlockCaps = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ConfigRom->CR_BusInfoBlockCaps));
        TRACE(TL_TRACE, ("ConfigRom->CR_Node_UniqueID[0] = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ConfigRom->CR_Node_UniqueID[0]));
        TRACE(TL_TRACE, ("ConfigRom->CR_Node_UniqueID[1] = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ConfigRom->CR_Node_UniqueID[1]));
        TRACE(TL_TRACE, ("ConfigRom->CR_Root_Info = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ConfigRom->CR_Root_Info));

        TRACE(TL_TRACE, ("UnitDirectoryBufferSize = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDirectoryBufferSize));
        TRACE(TL_TRACE, ("UnitDirectory = 0x%x\n", pIrb->u.GetConfigurationInformation.UnitDirectory));
        TRACE(TL_TRACE, ("UnitDirectoryLocation.NA_Bus_Number = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDirectoryLocation.IA_Destination_ID.NA_Bus_Number));
        TRACE(TL_TRACE, ("UnitDirectoryLocation.NA_Node_Number = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDirectoryLocation.IA_Destination_ID.NA_Node_Number));
        TRACE(TL_TRACE, ("UnitDirectoryLocation.Off_High = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDirectoryLocation.IA_Destination_Offset.Off_High));
        TRACE(TL_TRACE, ("UnitDirectoryLocation.Off_Low = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDirectoryLocation.IA_Destination_Offset.Off_Low));

        TRACE(TL_TRACE, ("UnitDependentDirectoryBufferSize = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDependentDirectoryBufferSize));
        TRACE(TL_TRACE, ("UnitDependentDirectory = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDependentDirectory));
        TRACE(TL_TRACE, ("UnitDependentDirectoryLocation.NA_Bus_Number = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDependentDirectoryLocation.IA_Destination_ID.NA_Bus_Number));
        TRACE(TL_TRACE, ("UnitDependentDirectoryLocation.NA_Node_Number = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDependentDirectoryLocation.IA_Destination_ID.NA_Node_Number));
        TRACE(TL_TRACE, ("UnitDependentDirectoryLocation.Off_High = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDependentDirectoryLocation.IA_Destination_Offset.Off_High));
        TRACE(TL_TRACE, ("UnitDependentDirectoryLocation.Off_Low = 0x%x\n",
            pIrb->u.GetConfigurationInformation.UnitDependentDirectoryLocation.IA_Destination_Offset.Off_Low));

        TRACE(TL_TRACE, ("VendorLeafBufferSize = 0x%x\n",
            pIrb->u.GetConfigurationInformation.VendorLeafBufferSize));
        TRACE(TL_TRACE, ("VendorLeaf->TL_CRC = 0x%x\n",
            pIrb->u.GetConfigurationInformation.VendorLeaf->TL_CRC));
        TRACE(TL_TRACE, ("VendorLeaf->TL_Length = 0x%x\n",
            pIrb->u.GetConfigurationInformation.VendorLeaf->TL_Length));
        TRACE(TL_TRACE, ("VendorLeaf->TL_Spec_Id = 0x%x\n",
            pIrb->u.GetConfigurationInformation.VendorLeaf->TL_Spec_Id));
        TRACE(TL_TRACE, ("VendorLeaf->TL_Language_Id = 0x%x\n",
            pIrb->u.GetConfigurationInformation.VendorLeaf->TL_Language_Id));

        TRACE(TL_TRACE, ("ModelLeafBufferSize = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ModelLeafBufferSize));
        TRACE(TL_TRACE, ("ModelLeaf->TL_CRC = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ModelLeaf->TL_CRC));
        TRACE(TL_TRACE, ("ModelLeaf->TL_Length = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ModelLeaf->TL_Length));
        TRACE(TL_TRACE, ("ModelLeaf->TL_Spec_Id = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ModelLeaf->TL_Spec_Id));
        TRACE(TL_TRACE, ("ModelLeaf->TL_Language_Id = 0x%x\n",
            pIrb->u.GetConfigurationInformation.ModelLeaf->TL_Language_Id));
    }
    else {

        TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        TRAP;
    }

    if (pIrb)
        ExFreePool(pIrb);

Exit_GetConfigurationInformation:

    if (allocNewIrp) 
        Irp->IoStatus = ioStatus;
        
    EXIT("t1394_GetConfigurationInformation", ntStatus);
    return(ntStatus);
} // t1394_GetConfigurationInformation

NTSTATUS
t1394_BusReset(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN ULONG            fulFlags
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PIRB                pIrb;

    PIRP                newIrp;
    BOOLEAN             allocNewIrp = FALSE;
    KEVENT              Event;
    IO_STATUS_BLOCK     ioStatus;
    
    ENTER("t1394_BusReset");

    TRACE(TL_TRACE, ("fulFlags = 0x%x\n", fulFlags));

    //
    // If this is a UserMode request create a newIrp so that the request
    // will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_BusReset;            
        }
        allocNewIrp = TRUE;
    }
    
    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));

    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_BusReset;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_BUS_RESET;
    pIrb->Flags = 0;
    pIrb->u.BusReset.fulFlags = fulFlags;

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }
    
    if (!NT_SUCCESS(ntStatus)) {

        TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        TRAP;
    }

    ExFreePool(pIrb);

Exit_BusReset:

    if (allocNewIrp) 
        Irp->IoStatus = ioStatus;
        
    EXIT("t1394_BusReset", ntStatus);
    return(ntStatus);
} // t1394_BusReset

NTSTATUS
t1394_GetGenerationCount(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN OUT PULONG       GenerationCount
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PIRB                pIrb;

    PIRP                newIrp;
    BOOLEAN             allocNewIrp = FALSE;
    KEVENT              Event;
    IO_STATUS_BLOCK     ioStatus;
    
    ENTER("t1394_GetGenerationCount");

    //
    // If we were not passed an Irp or this is a UserMode request create a newIrp 
    // so that the request will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_GetGenerationCount;            
        }
        allocNewIrp = TRUE;
    }

    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));

    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_GetGenerationCount;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_GET_GENERATION_COUNT;
    pIrb->Flags = 0;

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }
    
    if (NT_SUCCESS(ntStatus)) {

        *GenerationCount = pIrb->u.GetGenerationCount.GenerationCount;
        TRACE(TL_TRACE, ("GenerationCount = 0x%x\n", *GenerationCount));
    }
    else {

        TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        TRAP;
    }

    ExFreePool(pIrb);

Exit_GetGenerationCount:

    if (Irp && allocNewIrp) 
        Irp->IoStatus = ioStatus;
        
    EXIT("t1394_GetGenerationCount", ntStatus);
    return(ntStatus);
} // t1394_GetGenerationCount

NTSTATUS
t1394_SendPhyConfigurationPacket(
    IN PDEVICE_OBJECT               DeviceObject,
    IN PIRP                         Irp,
    IN PHY_CONFIGURATION_PACKET     PhyConfigurationPacket
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PIRB                pIrb;

    PIRP                newIrp;
    BOOLEAN             allocNewIrp = FALSE;
    KEVENT              Event;
    IO_STATUS_BLOCK     ioStatus;
    
    ENTER("t1394_SendPhyConfigurationPacket");

    TRACE(TL_TRACE, ("PCP_Phys_ID = 0x%x\n", PhyConfigurationPacket.PCP_Phys_ID));
    TRACE(TL_TRACE, ("PCP_Packet_ID = 0x%x\n", PhyConfigurationPacket.PCP_Packet_ID));
    TRACE(TL_TRACE, ("PCP_Gap_Count = 0x%x\n", PhyConfigurationPacket.PCP_Gap_Count));
    TRACE(TL_TRACE, ("PCP_Set_Gap_Count = 0x%x\n", PhyConfigurationPacket.PCP_Set_Gap_Count));
    TRACE(TL_TRACE, ("PCP_Force_Root = 0x%x\n", PhyConfigurationPacket.PCP_Force_Root));
    TRACE(TL_TRACE, ("PCP_Reserved1 = 0x%x\n", PhyConfigurationPacket.PCP_Reserved1));
    TRACE(TL_TRACE, ("PCP_Reserved2 = 0x%x\n", PhyConfigurationPacket.PCP_Reserved2));
    TRACE(TL_TRACE, ("PCP_Inverse = 0x%x\n", PhyConfigurationPacket.PCP_Inverse));

    //
    // If this is a UserMode request create a newIrp so that the request
    // will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_SendPhyConfigurationPacket;            
        }
        allocNewIrp = TRUE;
    }
    
    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));

    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_SendPhyConfigurationPacket;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_SEND_PHY_CONFIG_PACKET;
    pIrb->Flags = 0;
    pIrb->u.SendPhyConfigurationPacket.PhyConfigurationPacket = PhyConfigurationPacket;

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }
    
    if (!NT_SUCCESS(ntStatus)) {

        TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        TRAP;
    }

    if (pIrb)
        ExFreePool(pIrb);

Exit_SendPhyConfigurationPacket:

    if (allocNewIrp) 
        Irp->IoStatus = ioStatus;

    EXIT("t1394_SendPhyConfigurationPacket", ntStatus);
    return(ntStatus);
} // t1394_SendPhyConfigurationPacket

NTSTATUS
t1394_SetLocalHostProperties(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN ULONG            nLevel,
    IN PVOID            Information
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PIRB                pIrb;

    PSET_LOCAL_HOST_PROPS3  R0_SetLocalHostProps3;
    PCROM_DATA              CromData;

    PIRP                newIrp;
    BOOLEAN             allocNewIrp = FALSE;
    KEVENT              Event;
    IO_STATUS_BLOCK     ioStatus;
    
    ENTER("t1394_SetLocalHostProperties");

    //
    // If this is a UserMode request create a newIrp so that the request
    // will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_SetLocalHostProperties;            
        }
        allocNewIrp = TRUE;
    }
    
    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));

    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_SetLocalHostProperties;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_SET_LOCAL_HOST_PROPERTIES;
    pIrb->Flags = 0;
    pIrb->u.SetLocalHostProperties.nLevel = nLevel;

    TRACE(TL_TRACE, ("nLevel = 0x%x\n", nLevel));
    TRACE(TL_TRACE, ("Information = 0x%x\n", Information));

    if (nLevel == SET_LOCAL_HOST_PROPERTIES_GAP_COUNT) {

        PSET_LOCAL_HOST_PROPS2  SetLocalHostProps2;

        SetLocalHostProps2 = (PSET_LOCAL_HOST_PROPS2)Information;

        TRACE(TL_TRACE, ("GapCountLowerBound = 0x%x\n", SetLocalHostProps2->GapCountLowerBound));

        pIrb->u.SetLocalHostProperties.Information = Information;
    }
    else if (nLevel == SET_LOCAL_HOST_PROPERTIES_MODIFY_CROM) {

        PSET_LOCAL_HOST_PROPS3  SetLocalHostProps3;

        SetLocalHostProps3 = (PSET_LOCAL_HOST_PROPS3)Information;

        TRACE(TL_TRACE, ("fulFlags = 0x%x\n", SetLocalHostProps3->fulFlags));
        TRACE(TL_TRACE, ("hCromData = 0x%x\n", SetLocalHostProps3->hCromData));
        TRACE(TL_TRACE, ("nLength = 0x%x\n", SetLocalHostProps3->nLength));

        // since we need to create a mdl, we'll create another setlocalhostprops3
        // and pass that down to the bus driver
        R0_SetLocalHostProps3 = ExAllocatePool(NonPagedPool, sizeof(SET_LOCAL_HOST_PROPS3));

        if (!R0_SetLocalHostProps3) {

            TRACE(TL_ERROR, ("Failed to allocate R0_SetLocalHostProps3!\n"));
            TRAP;

            if (pIrb)
                ExFreePool(pIrb);

            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_SetLocalHostProperties;
        } // if

        TRACE(TL_TRACE, ("R0_SetLocalHostProps3 = 0x%x\n", R0_SetLocalHostProps3));

        // copy over the contents...
        RtlCopyMemory( R0_SetLocalHostProps3, 
                       SetLocalHostProps3, 
                       sizeof(SET_LOCAL_HOST_PROPS3)
                       );

        // branch, depending if we are adding or removing
        if (R0_SetLocalHostProps3->fulFlags == SLHP_FLAG_ADD_CROM_DATA) {

            // we are adding an entry. let's get our crom data struct...
            CromData = ExAllocatePool(NonPagedPool, sizeof(CROM_DATA));

            if (!CromData) {

                TRACE(TL_ERROR, ("Failed to allocate CromData!\n"));
                TRAP;

                if (pIrb)
                    ExFreePool(pIrb);

                if (R0_SetLocalHostProps3)
                    ExFreePool(R0_SetLocalHostProps3);

                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                goto Exit_SetLocalHostProperties;
            }

            // let's allocate our buffer...
            CromData->Buffer = ExAllocatePool(NonPagedPool, R0_SetLocalHostProps3->nLength);

            TRACE(TL_TRACE, ("CromData->Buffer = 0x%x\n", CromData->Buffer));

            if (!CromData->Buffer) {

                TRACE(TL_ERROR, ("Failed to allocate CromData->Buffer!\n"));
                TRAP;

                if (pIrb)
                    ExFreePool(pIrb);

                if (R0_SetLocalHostProps3)
                    ExFreePool(R0_SetLocalHostProps3);

                if (CromData)
                    ExFreePool(CromData);

                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                goto Exit_SetLocalHostProperties;
            }

            // copy over contents (mdl == ring 3 buffer)
            RtlCopyMemory(CromData->Buffer, &SetLocalHostProps3->Mdl, SetLocalHostProps3->nLength);

            R0_SetLocalHostProps3->Mdl = IoAllocateMdl (CromData->Buffer,
                                                        R0_SetLocalHostProps3->nLength,
                                                        FALSE,
                                                        FALSE,
                                                        NULL);

            MmBuildMdlForNonPagedPool(R0_SetLocalHostProps3->Mdl);

            TRACE(TL_TRACE, ("Mdl = 0x%x\n", R0_SetLocalHostProps3->Mdl));
        }
        else if (SetLocalHostProps3->fulFlags == SLHP_FLAG_REMOVE_CROM_DATA) {

            TRACE(TL_TRACE, ("hCromData = 0x%x\n", R0_SetLocalHostProps3->hCromData));
        }

        pIrb->u.SetLocalHostProperties.Information = (PVOID)R0_SetLocalHostProps3;
    }

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }

    if (!NT_SUCCESS(ntStatus)) {

        if (nLevel == SET_LOCAL_HOST_PROPERTIES_MODIFY_CROM) {

            if (R0_SetLocalHostProps3->fulFlags == SLHP_FLAG_ADD_CROM_DATA) {

                if (R0_SetLocalHostProps3->Mdl)
                    ExFreePool(R0_SetLocalHostProps3->Mdl);

                if (CromData->Buffer)
                    ExFreePool(CromData->Buffer);

                if (CromData)
                    ExFreePool(CromData);
            }

            if (R0_SetLocalHostProps3)
                ExFreePool(R0_SetLocalHostProps3);
        }

        TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        TRAP;
    }
    else {

        if (nLevel == SET_LOCAL_HOST_PROPERTIES_MODIFY_CROM) {

            KIRQL           Irql;

            // branch, depending if we are adding or removing
            if (R0_SetLocalHostProps3->fulFlags == SLHP_FLAG_ADD_CROM_DATA) {

                PSET_LOCAL_HOST_PROPS3  SetLocalHostProps3;

                SetLocalHostProps3 = Information;
                SetLocalHostProps3->hCromData = R0_SetLocalHostProps3->hCromData;

                TRACE(TL_TRACE, ("hCromData = 0x%x\n", SetLocalHostProps3->hCromData));

                if (CromData) {

                    CromData->hCromData = SetLocalHostProps3->hCromData;
                    CromData->pMdl = R0_SetLocalHostProps3->Mdl;

                    // need to add to our list...
                    KeAcquireSpinLock(&deviceExtension->CromSpinLock, &Irql);
                    InsertHeadList(&deviceExtension->CromData, &CromData->CromList);
                    KeReleaseSpinLock(&deviceExtension->CromSpinLock, Irql);
                }
            }
            else if (R0_SetLocalHostProps3->fulFlags == SLHP_FLAG_REMOVE_CROM_DATA) {

                // have to find our struct...
                KeAcquireSpinLock(&deviceExtension->CromSpinLock, &Irql);

                CromData = (PCROM_DATA) deviceExtension->CromData.Flink;

                while (CromData) {

                    if (CromData->hCromData == R0_SetLocalHostProps3->hCromData) {

                        RemoveEntryList(&CromData->CromList);
                        break;
                    }
                    else if (CromData->CromList.Flink == &deviceExtension->CromData) {

                        CromData = NULL;
                        break;
                    }
                    else
                        CromData = (PCROM_DATA)CromData->CromList.Flink;
                }

                KeReleaseSpinLock(&deviceExtension->CromSpinLock, Irql);

                if (CromData) {

                    if (CromData->Buffer)
                        ExFreePool(CromData->Buffer);

                    if (CromData->pMdl)
                        ExFreePool(CromData->pMdl);

                    ExFreePool(CromData);
                }
            }

            if (R0_SetLocalHostProps3)
                ExFreePool(R0_SetLocalHostProps3);
        }
    }

    if (pIrb)
        ExFreePool(pIrb);

Exit_SetLocalHostProperties:

    if (allocNewIrp) 
        Irp->IoStatus = ioStatus;
        
    EXIT("t1394_SetLocalHostProperties", ntStatus);
    return(ntStatus);
} // t1394_SetLocalHostProperties

void
t1394_BusResetRoutine(
    IN PVOID    Context
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT      DeviceObject = Context;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PBUS_RESET_IRP      BusResetIrp = NULL;
    PDRIVER_CANCEL      prevCancel = NULL;
    PIRP                Irp;
    KIRQL               Irql;
    CCHAR               StackSize;
    
    ENTER("t1394_BusResetRoutine");

    TRACE(TL_TRACE, ("Context = 0x%x\n", Context));

    if (deviceExtension->bShutdown)
        goto Exit_BusResetRoutine;

    // update the generation count...
    StackSize = deviceExtension->StackDeviceObject->StackSize + 1;
    Irp = IoAllocateIrp(StackSize, FALSE);

    ntStatus = t1394_GetGenerationCount(DeviceObject, Irp, &deviceExtension->GenerationCount);
    TRACE (TL_TRACE, ("Retrieved generation count = 0x%x\n", deviceExtension->GenerationCount));

    // free the allocated Irp
    IoFreeIrp (Irp);

    // if we have any bus reset notify irps, then nows the time to complete them
    KeAcquireSpinLock(&deviceExtension->ResetSpinLock, &Irql);

    while (!IsListEmpty(&deviceExtension->BusResetIrps)) {

        // get the irp off of the list
        BusResetIrp = (PBUS_RESET_IRP)RemoveHeadList(&deviceExtension->BusResetIrps);

        TRACE(TL_TRACE, ("BusResetIrp = 0x%x\n", BusResetIrp));

        // make this irp non-cancelable...
        prevCancel = IoSetCancelRoutine(BusResetIrp->Irp, NULL);

        TRACE(TL_TRACE, ("Completing BusResetIrp->Irp = 0x%x\n", BusResetIrp->Irp));

        // and complete it...
        BusResetIrp->Irp->IoStatus.Status = STATUS_SUCCESS;
        IoCompleteRequest(BusResetIrp->Irp, IO_NO_INCREMENT);

        ExFreePool(BusResetIrp);
    }

    KeReleaseSpinLock(&deviceExtension->ResetSpinLock, Irql);
    
Exit_BusResetRoutine:

    EXIT("t1394_BusResetRoutine", ntStatus);
} // t1394_BusResetRoutine

NTSTATUS
t1394_BusResetNotification(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN ULONG            fulFlags
    )
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PIRB                pIrb;

    PIRP                newIrp;
    BOOLEAN             allocNewIrp = FALSE;
    KEVENT              Event;
    IO_STATUS_BLOCK     ioStatus;
    
    ENTER("t1394_BusResetNotification");

    TRACE(TL_TRACE, ("fulFlags = 0x%x\n", fulFlags));

    //
    // If this is a UserMode request create a newIrp so that the request
    // will be issued from KernelMode
    //
    if (Irp->RequestorMode == UserMode) {

        newIrp = IoBuildDeviceIoControlRequest (IOCTL_1394_CLASS, deviceExtension->StackDeviceObject, 
                            NULL, 0, NULL, 0, TRUE, &Event, &ioStatus);

        if (!newIrp) {

            TRACE(TL_ERROR, ("Failed to allocate newIrp!\n"));
            TRAP;
        
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit_BusResetNotification;            
        }
        allocNewIrp = TRUE;
    }
    
    pIrb = ExAllocatePool(NonPagedPool, sizeof(IRB));

    if (!pIrb) {

        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        TRAP;

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit_BusResetNotification;
    } // if

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_BUS_RESET_NOTIFICATION;
    pIrb->Flags = 0;
    pIrb->u.BusResetNotification.fulFlags = fulFlags;
    pIrb->u.BusResetNotification.ResetRoutine = t1394_BusResetRoutine;
    pIrb->u.BusResetNotification.ResetContext = DeviceObject;

    //
    // If we allocated this irp, submit it asynchronously and wait for its
    // completion event to be signaled.  Otherwise submit it synchronously
    //
    if (allocNewIrp) {

        KeInitializeEvent (&Event, NotificationEvent, FALSE);
        ntStatus = t1394_SubmitIrpAsync (deviceExtension->StackDeviceObject, newIrp, pIrb);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject (&Event, Executive, KernelMode, FALSE, NULL); 
            ntStatus = ioStatus.Status;
        }
    }
    else {
        ntStatus = t1394_SubmitIrpSynch(deviceExtension->StackDeviceObject, Irp, pIrb);
    }
    
    if (!NT_SUCCESS(ntStatus)) {

        TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        TRAP;
    }

    if (pIrb)
        ExFreePool(pIrb);

Exit_BusResetNotification:

    if (allocNewIrp) 
        Irp->IoStatus = ioStatus;
        
    EXIT("t1394_BusResetNotification", ntStatus);
    return(ntStatus);
} // t1394_BusResetNotification

