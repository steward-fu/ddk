/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    BulkUsb.c 

Abstract:

    Bulk USB device driver for Intel 82930 USB test board
    Main module

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.


Revision History:

    11/17/97: created

--*/


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "Blk82930.h"
#include "GUID829.h"


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object

    RegistryPath - pointer to a unicode string representing the path
                   to driver-specific key in the registry

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT deviceObject = NULL;
    BOOLEAN fRes;

#if DBG
	// should be done before any debug output is done.
    // read our debug verbosity level from the registry
    BulkUsb_GetRegistryDword( BULKUSB_REGISTRY_PARAMETERS_PATH, //absolute registry path
                                     L"DebugLevel",     // REG_DWORD ValueName
                                     &gDebugLevel );    // Value receiver
#endif

    BULKUSB_KdPrint( DBGLVL_MINIMUM ,("Entering DriverEntry(), RegistryPath=\n    %ws\n", RegistryPath->Buffer ));

    //
    // Create dispatch points for create, close, unload
    DriverObject->MajorFunction[IRP_MJ_CREATE] = BulkUsb_Create;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = BulkUsb_Close;
    DriverObject->DriverUnload = BulkUsb_Unload;

    // User mode DeviceIoControl() calls will be routed here
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = BulkUsb_ProcessIOCTL;

    // User mode ReadFile()/WriteFile() calls will be routed here
    DriverObject->MajorFunction[IRP_MJ_WRITE] = BulkUsb_Write;
    DriverObject->MajorFunction[IRP_MJ_READ] = BulkUsb_Read;

    // routines for handling system PNP and power management requests
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = BulkUsb_ProcessSysControlIrp;
    DriverObject->MajorFunction[IRP_MJ_PNP] = BulkUsb_ProcessPnPIrp;
    DriverObject->MajorFunction[IRP_MJ_POWER] = BulkUsb_ProcessPowerIrp;

    // The Functional Device Object (FDO) will not be created for PNP devices until 
    // this routine is called upon device plug-in.
    DriverObject->DriverExtension->AddDevice = BulkUsb_PnPAddDevice;


    BULKUSB_KdPrint( DBGLVL_DEFAULT,("exiting DriverEntry (%x)\n", ntStatus));

    return ntStatus;
}





NTSTATUS
BulkUsb_ProcessSysControlIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++

Routine Description:

    Main dispatch table routine for IRP_MJ_SYSTEM_CONTROL
	We basically just pass these down to the PDO

Arguments:

    DeviceObject - pointer to FDO device object

    Irp          - pointer to an I/O Request Packet

Return Value:

	Status returned from lower driver


--*/
{

    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    NTSTATUS waitStatus;
    PDEVICE_OBJECT stackDeviceObject;

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

    BULKUSB_KdPrint( DBGLVL_HIGH, ( "enter BulkUsb_ProcessSysControlIrp()\n") );

    BulkUsb_IncrementIoCount(DeviceObject);

    BULKUSB_ASSERT( IRP_MJ_SYSTEM_CONTROL == irpStack->MajorFunction );

    IoCopyCurrentIrpStackLocationToNext(Irp);


    ntStatus = IoCallDriver(stackDeviceObject,
                            Irp);

    BulkUsb_DecrementIoCount(DeviceObject);

    BULKUSB_KdPrint( DBGLVL_HIGH,("BulkUsb_ProcessSysControlIrp() Exit BulkUsb_ProcessSysControlIrp %x\n", ntStatus));

    return ntStatus;
}


VOID
BulkUsb_Unload(
    IN PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    Free all the allocated resources, etc.

Arguments:

    DriverObject - pointer to a driver object

Return Value:


--*/
{
    BULKUSB_KdPrint( DBGLVL_HIGH,("enter BulkUsb_Unload\n"));

    //
    // Free any global resources allocated
    // in DriverEntry.
	// We have few or none because for a PNP device, almost all
	// allocation is done in PnpAddDevice() and all freeing 
	// while handling IRP_MN_REMOVE_DEVICE:
    //
	BULKUSB_ASSERT( gExAllocCount == 0 );

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("exit BulkUsb_Unload\n"));

}


NTSTATUS
BulkUsb_SymbolicLink(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PUNICODE_STRING deviceLinkUnicodeString

    )
/*++

Routine Description:

    This routine is called to create and initialize
    a GUID-based symbolic link to our device to be used to open/create 
    instances of us from user mode.

    Called from BulkUsb_CreateDeviceObject() to create the link. 

Arguments:

    DeviceObject - pointer to our Physical Device Object ( PDO )

    deviceLinkUnicodeString - Points to a unicode string structure allocated by the caller. 
        If this routine is successful, it initializes the unicode string and allocates 
        the string buffer containing the kernel-mode path to the symbolic link for this 
        device interface. 


Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/{
    NTSTATUS ntStatus = STATUS_SUCCESS;


    //  Create the symbolic link
     
    // IoRegisterDeviceInterface registers device functionality (a device interface) 
    // that a driver will enable for use by applications or other system components.

    ntStatus = IoRegisterDeviceInterface(
                DeviceObject,
                (LPGUID)&GUID_CLASS_I82930_BULK,
                NULL,
                deviceLinkUnicodeString);

    BULKUSB_KdPrintCond( DBGLVL_MEDIUM, (!(NT_SUCCESS(ntStatus))),
            ("FAILED to IoRegisterDeviceInterface()\n"));

   if (NT_SUCCESS(ntStatus)) {

       // IoSetDeviceInterfaceState enables or disables a previously 
       // registered device interface. Applications and other system components 
       // can open only interfaces that are enabled.

        ntStatus = IoSetDeviceInterfaceState(deviceLinkUnicodeString, TRUE);

        BULKUSB_KdPrintCond( DBGLVL_MEDIUM,
                (!(NT_SUCCESS(ntStatus))),
                ("FAILED to IoSetDeviceInterfaceState()\n"));

        BULKUSB_KdPrintCond( DBGLVL_MEDIUM,
                ((NT_SUCCESS(ntStatus))),
                ("SUCCEEDED  IoSetDeviceInterfaceState()\n"));

    }

    return ntStatus;
}



NTSTATUS
BulkUsb_CreateDeviceObject(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PDEVICE_OBJECT *DeviceObject
    )
/*++

Routine Description:

    Creates a Functional DeviceObject

Arguments:

    DriverObject - pointer to the driver object for device

    DeviceObject - pointer to DeviceObject pointer to return
                    created device object.

    Instance - instance of the device create.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus;
    UNICODE_STRING deviceLinkUnicodeString;
    PDEVICE_EXTENSION deviceExtension;
    USHORT i;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("enter BulkUsb_CreateDeviceObject() \n"));

    ntStatus = BulkUsb_SymbolicLink( PhysicalDeviceObject, &deviceLinkUnicodeString );

    BULKUSB_KdPrintCond( DBGLVL_DEFAULT,
            (NT_SUCCESS(ntStatus)),
            ("BulkUsb_CreateDeviceObject() SUCCESS Create GUID_CLASS_BulkUsb_BULK-based Device name\n   %ws\n Length decimal %d, MaximumLength decimal %d\n",
            deviceLinkUnicodeString.Buffer,
            deviceLinkUnicodeString.Length,
            deviceLinkUnicodeString.MaximumLength));

    BULKUSB_KdPrintCond( DBGLVL_DEFAULT,
            (!(NT_SUCCESS(ntStatus))),
            ("BulkUsb_CreateDeviceObject() FAILED to Create GUID_CLASS_BulkUsb_BULK-based Device name\n"));

    if (NT_SUCCESS(ntStatus)) {

        ntStatus = IoCreateDevice (DriverObject,
                           sizeof (DEVICE_EXTENSION),
                           NULL,
                           FILE_DEVICE_UNKNOWN,
                           FILE_AUTOGENERATED_DEVICE_NAME,
                           FALSE,
                           DeviceObject);

        if (NT_SUCCESS(ntStatus))  {
             deviceExtension = (PDEVICE_EXTENSION) ((*DeviceObject)->DeviceExtension);

        }

        BULKUSB_KdPrintCond( DBGLVL_DEFAULT,
                (!(NT_SUCCESS(ntStatus))),
                ("BulkUsb_CreateDeviceObject() IoCreateDevice() FAILED\n"));

 
        if (!NT_SUCCESS(ntStatus))  {
             return ntStatus;
        }


        //default maximum transfer size per staged io request
        deviceExtension->MaximumTransferSize =  BULKUSB_MAX_TRANSFER_SIZE ;

#if DBG
        // may be overridden in registry
        BulkUsb_GetRegistryDword( BULKUSB_REGISTRY_PARAMETERS_PATH,
                                         L"MaximumTransferSize",
                                         &(deviceExtension->MaximumTransferSize) );
#endif

        // Name buffer for our named Functional device object link
        // The name is generated based on the driver's class GUID
        RtlCopyMemory(deviceExtension->DeviceLinkNameBuffer,
                      deviceLinkUnicodeString.Buffer,
                      deviceLinkUnicodeString.Length);


        // this event is triggered when there is no pending io of any kind and device is removed
        KeInitializeEvent(&deviceExtension->RemoveEvent, NotificationEvent, FALSE);

        // this event is triggered when self-requested power irps complete
        KeInitializeEvent(&deviceExtension->SelfRequestedPowerIrpEvent, NotificationEvent, FALSE);

        // this event is triggered when when BulkUsb_AsyncReadWrite_Complete() finishes or cancels last staged io irp
        KeInitializeEvent(&deviceExtension->StagingDoneEvent, NotificationEvent, FALSE);

        // this event is triggered when there is no pending io  (pending io count == 1 )
        KeInitializeEvent(&deviceExtension->NoPendingIoEvent, NotificationEvent, FALSE);

		// spinlock used to protect inc/dec iocount logic
		KeInitializeSpinLock (&deviceExtension->IoCountSpinLock);

		//free buffer from unicode string we used to init interface
		RtlFreeUnicodeString( &deviceLinkUnicodeString );
    }


    return ntStatus;
}


NTSTATUS
BulkUsb_CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB Urb
    )
/*++

Routine Description:

    Passes a URB to the USBD class driver
	The client device driver passes USB request block (URB) structures 
	to the class driver as a parameter in an IRP with Irp->MajorFunction
	set to IRP_MJ_INTERNAL_DEVICE_CONTROL and the next IRP stack location 
	Parameters.DeviceIoControl.IoControlCode field set to 
	IOCTL_INTERNAL_USB_SUBMIT_URB. 

Arguments:

    DeviceObject - pointer to the physical device object (PDO)

    Urb - pointer to an already-formatted Urb request block

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;

    BULKUSB_KdPrint( DBGLVL_MAXIMUM,("enter BulkUsb_CallUSBD\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_SUBMIT_URB,
                deviceExtension->TopOfStackDeviceObject, //Points to the next-lower driver's device object
                NULL, // optional input bufer; none needed here
                0,	  // input buffer len if used
                NULL, // optional output bufer; none needed here
                0,    // output buffer len if used
                TRUE, // If InternalDeviceControl is TRUE the target driver's Dispatch
				      //  outine for IRP_MJ_INTERNAL_DEVICE_CONTROL or IRP_MJ_SCSI 
					  // is called; otherwise, the Dispatch routine for 
					  // IRP_MJ_DEVICE_CONTROL is called.
                &event,     // event to be signalled on completion
                &ioStatus);  // Specifies an I/O status block to be set when the request is completed the lower driver. 

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    nextStack = IoGetNextIrpStackLocation(irp);
    BULKUSB_ASSERT(nextStack != NULL);

    //
    // pass the URB to the USB driver stack
    //
    nextStack->Parameters.Others.Argument1 = Urb;

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    BULKUSB_KdPrint( DBGLVL_MAXIMUM,("BulkUsb_CallUSBD() return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

    } else {
        ioStatus.Status = ntStatus;
    }

    BULKUSB_KdPrint( DBGLVL_MAXIMUM,("BulkUsb_CallUSBD() URB status = %x status = %x irp status %x\n",
        Urb->UrbHeader.Status, status, ioStatus.Status));

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    BULKUSB_KdPrintCond( DBGLVL_MAXIMUM, !NT_SUCCESS( ntStatus ), ("exit BulkUsb_CallUSBD FAILED (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
BulkUsb_ConfigureDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    Initializes a given instance of the device on the USB and
	selects and saves the configuration.

Arguments:

    DeviceObject - pointer to the physical device object for this instance of the 82930
                    device.


Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;

    BULKUSB_KdPrint( DBGLVL_HIGH,("enter BulkUsb_ConfigureDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

	BULKUSB_ASSERT( deviceExtension->UsbConfigurationDescriptor == NULL );

    urb = BULKUSB_ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
	if ( !urb )
		return STATUS_INSUFFICIENT_RESOURCES;

	// When USB_CONFIGURATION_DESCRIPTOR_TYPE is specified for DescriptorType
	// in a call to UsbBuildGetDescriptorRequest(),
	// all interface, endpoint, class-specific, and vendor-specific descriptors 
	// for the configuration also are retrieved. 
	// The caller must allocate a buffer large enough to hold all of this 
	// information or the data is truncated without error.
	// Therefore the 'siz' set below is just a 'good guess', and we may have to retry

    siz = sizeof(USB_CONFIGURATION_DESCRIPTOR) + 512;  

	// We will break out of this 'retry loop' when UsbBuildGetDescriptorRequest()
	// has a big enough deviceExtension->UsbConfigurationDescriptor buffer not to truncate
	while( 1 ) {

		deviceExtension->UsbConfigurationDescriptor = BULKUSB_ExAllocatePool(NonPagedPool, siz);

		if ( !deviceExtension->UsbConfigurationDescriptor ) {
		    BULKUSB_ExFreePool(urb);
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		UsbBuildGetDescriptorRequest(urb,
									 (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
									 USB_CONFIGURATION_DESCRIPTOR_TYPE,
									 0,
									 0,
									 deviceExtension->UsbConfigurationDescriptor,
									 NULL,
									 siz,
									 NULL);

		ntStatus = BulkUsb_CallUSBD(DeviceObject, urb);

		BULKUSB_KdPrint( DBGLVL_HIGH,("BulkUsb_CallUSBD() Configuration Descriptor = %x, len %x\n",
						deviceExtension->UsbConfigurationDescriptor,
						urb->UrbControlDescriptorRequest.TransferBufferLength));
		//
		// if we got some data see if it was enough.
		// NOTE: we may get an error in URB because of buffer overrun
		if (urb->UrbControlDescriptorRequest.TransferBufferLength>0 &&
				deviceExtension->UsbConfigurationDescriptor->wTotalLength > siz) {

			siz = deviceExtension->UsbConfigurationDescriptor->wTotalLength;
			BULKUSB_ExFreePool(deviceExtension->UsbConfigurationDescriptor);
			deviceExtension->UsbConfigurationDescriptor = NULL;
		} else {
			break;  // we got it on the first try
		}

	} // end, while (retry loop )

    BULKUSB_ExFreePool(urb);
	BULKUSB_ASSERT( deviceExtension->UsbConfigurationDescriptor );

    //
    // We have the configuration descriptor for the configuration we want.
    // Now we issue the select configuration command to get
    // the  pipes associated with this configuration.
    //

    ntStatus = BulkUsb_SelectInterface(DeviceObject,
        deviceExtension->UsbConfigurationDescriptor);


    BULKUSB_KdPrint( DBGLVL_HIGH,("exit BulkUsb_ConfigureDevice (%x)\n", ntStatus));

    return ntStatus;
} 


NTSTATUS
BulkUsb_SelectInterface(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    )
/*++

Routine Description:

    Initializes an 82930 with (possibly) multiple interfaces;
	This minidriver only supports one interface (with multiple endpoints).

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.

    ConfigurationDescriptor - pointer to the USB configuration
                    descriptor containing the interface and endpoint
                    descriptors.

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb = NULL;
    ULONG i;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor = NULL;
	PUSBD_INTERFACE_INFORMATION Interface = NULL;
    USHORT siz;

    BULKUSB_KdPrint( DBGLVL_MEDIUM,("enter BulkUsb_SelectInterface\n"));

    deviceExtension = DeviceObject->DeviceExtension;


    BULKUSB_KdPrint( DBGLVL_HIGH,("BulkUsb_SelectInterface() called with NULL Interface\n"));
    //
    // BulkUsb driver only supports one interface, we must parse
    // the configuration descriptor for the interface 
    // and remember the pipes.
    //

    urb = USBD_CreateConfigurationRequest(ConfigurationDescriptor, &siz);

    if (urb) {

		//
		// USBD_ParseConfigurationDescriptorEx searches a given configuration
		// descriptor and returns a pointer to an interface that matches the 
		//  given search criteria. We only support one interface on this device
		//
        interfaceDescriptor =
            USBD_ParseConfigurationDescriptorEx(ConfigurationDescriptor,
								  ConfigurationDescriptor, //search from start of config  descriptro
								  -1,	// interface number not a criteria; we only support one interface
								  -1,   // not interested in alternate setting here either
								  -1,   // interface class not a criteria
								  -1,   // interface subclass not a criteria
								  -1    // interface protocol not a criteria
								  );

		if ( !interfaceDescriptor ) {

			BULKUSB_KdPrint( DBGLVL_HIGH,("BulkUsb_SelectInterface() ParseConfigurationDescriptorEx() failed\n  returning STATUS_INSUFFICIENT_RESOURCES\n"));
			BULKUSB_ExFreePool(urb);
			return STATUS_INSUFFICIENT_RESOURCES;
		}

        Interface = &urb->UrbSelectConfiguration.Interface;

        for (i=0; i< Interface->NumberOfPipes; i++) {
            //
            // perform any pipe initialization here
            //
            Interface->Pipes[i].MaximumTransferSize = deviceExtension->MaximumTransferSize;
            Interface->Pipes[i].PipeFlags = 0;
        }

        UsbBuildSelectConfigurationRequest(urb,
                                          (USHORT) siz,
                                          ConfigurationDescriptor);


        ntStatus = BulkUsb_CallUSBD(DeviceObject, urb);

        deviceExtension->UsbConfigurationHandle =
            urb->UrbSelectConfiguration.ConfigurationHandle;

    } else {
        BULKUSB_KdPrint( DBGLVL_HIGH,("BulkUsb_SelectInterface() USBD_CreateConfigurationRequest() failed\n  returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }


    if (NT_SUCCESS(ntStatus)) {

        //
        // Save the configuration handle for this device
        //

        deviceExtension->UsbConfigurationHandle =
            urb->UrbSelectConfiguration.ConfigurationHandle;

        deviceExtension->UsbInterface = BULKUSB_ExAllocatePool(NonPagedPool,
                                                    Interface->Length);

        if (deviceExtension->UsbInterface) {
            ULONG j;

            //
            // save a copy of the interface information returned
            //
            RtlCopyMemory(deviceExtension->UsbInterface, Interface, Interface->Length);

            //
            // Dump the interface to the debugger
            //
            BULKUSB_KdPrint( DBGLVL_MEDIUM,("---------\n"));
            BULKUSB_KdPrint( DBGLVL_MEDIUM,("NumberOfPipes 0x%x\n", deviceExtension->UsbInterface->NumberOfPipes));
            BULKUSB_KdPrint( DBGLVL_MEDIUM,("Length 0x%x\n", deviceExtension->UsbInterface->Length));
            BULKUSB_KdPrint( DBGLVL_MEDIUM,("Alt Setting 0x%x\n", deviceExtension->UsbInterface->AlternateSetting));
            BULKUSB_KdPrint( DBGLVL_MEDIUM,("Interface Number 0x%x\n", deviceExtension->UsbInterface->InterfaceNumber));
            BULKUSB_KdPrint( DBGLVL_MEDIUM,("Class, subclass, protocol 0x%x 0x%x 0x%x\n",
                deviceExtension->UsbInterface->Class,
                deviceExtension->UsbInterface->SubClass,
                deviceExtension->UsbInterface->Protocol));

            // Dump the pipe info

            for (j=0; j<Interface->NumberOfPipes; j++) {
                PUSBD_PIPE_INFORMATION pipeInformation;

                pipeInformation = &deviceExtension->UsbInterface->Pipes[j];

                BULKUSB_KdPrint( DBGLVL_MEDIUM,("---------\n"));
                BULKUSB_KdPrint( DBGLVL_MEDIUM,("PipeType 0x%x\n", pipeInformation->PipeType));
                BULKUSB_KdPrint( DBGLVL_MEDIUM,("EndpointAddress 0x%x\n", pipeInformation->EndpointAddress));
                BULKUSB_KdPrint( DBGLVL_MEDIUM,("MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize));
                BULKUSB_KdPrint( DBGLVL_MEDIUM,("Interval 0x%x\n", pipeInformation->Interval));
                BULKUSB_KdPrint( DBGLVL_MEDIUM,("Handle 0x%x\n", pipeInformation->PipeHandle));
                BULKUSB_KdPrint( DBGLVL_MEDIUM,("MaximumTransferSize 0x%x\n", pipeInformation->MaximumTransferSize));
            }

            BULKUSB_KdPrint( DBGLVL_MEDIUM,("---------\n"));
        }
    }

    if (urb) {
		// don't call the BULKUSB_ExFreePool since the buffer was 
		//  alloced by USBD_CreateConfigurationRequest, not BULKUSB_ExAllocatePool()
        ExFreePool(urb);
    }
    BULKUSB_KdPrint( DBGLVL_HIGH,("exit BulkUsb_SelectInterface (%x)\n", ntStatus));

    return ntStatus; 
}



NTSTATUS
BulkUsb_ResetPipe(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSBD_PIPE_INFORMATION PipeInfo
    )
/*++

Routine Description:

    Reset a given USB pipe.

    NOTES:

    This will reset the host to Data0 and should also reset the device to Data0 

Arguments:

    Ptrs to our FDO and a USBD_PIPE_INFORMATION struct

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
    PURB urb;
    PDEVICE_EXTENSION deviceExtension;

    deviceExtension = DeviceObject->DeviceExtension;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_ResetPipe() Reset Pipe %x\n", PipeInfo));

    urb = BULKUSB_ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_PIPE_REQUEST));

    if (urb) {

        urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
        urb->UrbPipeRequest.PipeHandle =
            PipeInfo->PipeHandle;

        ntStatus = BulkUsb_CallUSBD(DeviceObject, urb);

        BULKUSB_ExFreePool(urb);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (!(NT_SUCCESS(ntStatus))) {
#if DBG
		if ( gpDbg )
			gpDbg->PipeErrorCount++;
#endif

        BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_ResetPipe() FAILED, ntStatus =0x%x\n", ntStatus ));
    }
    else {
#if DBG
		if ( gpDbg )
			gpDbg->ResetPipeCount++;
#endif

        BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_ResetPipe() SUCCESS, ntStatus =0x%x\n", ntStatus ));
    }

    return ntStatus;
}




LONG
BulkUsb_DecrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

        We keep a pending IO count ( extension->PendingIoCount )  in the device extension.
        The first increment of this count is done on adding the device.
        Subsequently, the count is incremented for each new IRP received and
        decremented when each IRP is completed or passed on.

        Transition to 'one' therefore indicates no IO is pending and signals
        deviceExtension->NoPendingIoEvent. This is needed for processing
        IRP_MN_QUERY_REMOVE_DEVICE

        Transition to 'zero' signals an event ( deviceExtension->RemoveEvent )
        to enable device removal. This is used in processing for IRP_MN_REMOVE_DEVICE
 
Arguments:

        DeviceObject -- ptr to our FDO

Return Value:

        deviceExtension->PendingIoCount


--*/

{
    PDEVICE_EXTENSION deviceExtension;
    LONG ioCount;
    KIRQL             oldIrql;

    deviceExtension = DeviceObject->DeviceExtension;
	KeAcquireSpinLock (&deviceExtension->IoCountSpinLock, &oldIrql);

    ioCount = InterlockedDecrement(&deviceExtension->PendingIoCount);
#if DBG
    InterlockedDecrement(&gpDbg->PendingIoCount);
#endif


    BULKUSB_TrapCond( DBGLVL_HIGH,( 0 > ioCount ) );

    if (ioCount==1) {
        // trigger no pending io
        KeSetEvent(&deviceExtension->NoPendingIoEvent,
                   1,
                   FALSE);
    }

    if (ioCount==0) {
        // trigger remove-device event
        KeSetEvent(&deviceExtension->RemoveEvent,
                   1,
                   FALSE);
    }
	KeReleaseSpinLock (&deviceExtension->IoCountSpinLock, oldIrql);

    BULKUSB_KdPrint( DBGLVL_HIGH,("Exit BulkUsb_DecrementIoCount() Pending io count = %x\n", ioCount));
    return ioCount;
}


VOID
BulkUsb_IncrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

        We keep a pending IO count ( extension->PendingIoCount )  in the device extension.
        The first increment of this count is done on adding the device.
        Subsequently, the count is incremented for each new IRP received and
        decremented when each IRP is completed or passed on.

 
Arguments:

        DeviceObject -- ptr to our FDO

Return Value:

        None


--*/
{
    PDEVICE_EXTENSION deviceExtension;
    KIRQL             oldIrql;

    deviceExtension = DeviceObject->DeviceExtension;

    BULKUSB_KdPrint( DBGLVL_MAXIMUM,("Enter BulkUsb_IncrementIoCount() Pending io count = %x\n", deviceExtension->PendingIoCount));

	KeAcquireSpinLock (&deviceExtension->IoCountSpinLock, &oldIrql);

    InterlockedIncrement(&deviceExtension->PendingIoCount);
#if DBG
    InterlockedIncrement(&gpDbg->PendingIoCount);
#endif
	KeReleaseSpinLock (&deviceExtension->IoCountSpinLock, oldIrql);

    BULKUSB_KdPrint( DBGLVL_HIGH,("Exit BulkUsb_IncrementIoCount() Pending io count = %x\n", deviceExtension->PendingIoCount));
}

