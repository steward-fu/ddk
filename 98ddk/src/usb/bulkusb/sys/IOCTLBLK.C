/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

   ioctlblk.c

Abstract:

    USB device driver for Intel 82930 USB test board.
    IOCTL handlers

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.


Revision History:

    11/17/97 : created

--*/


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "Blk82930.h"

#include "BulkUsb.h"
#include "usbdlib.h"



NTSTATUS
BulkUsb_ProcessIOCTL(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Dispatch table handler for IRP_MJ_DEVICE_CONTROL; 
    Handle DeviceIoControl() calls  from User mode


Arguments:

    DeviceObject - pointer to the FDO for this instance of the 82930 device.


Return Value:

    NT status code

--*/
{
    PIO_STACK_LOCATION irpStack;
    PVOID ioBuffer;
    ULONG inputBufferLength;
    ULONG outputBufferLength;
    PDEVICE_EXTENSION deviceExtension;
    ULONG ioControlCode;
    NTSTATUS ntStatus;
    ULONG length;
    PUCHAR pch;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("IRP_MJ_DEVICE_CONTROL\n"));

    BulkUsb_IncrementIoCount(DeviceObject);

    //
    // Get a pointer to the current location in the Irp. This is where
    //     the function codes and parameters are located.
    //

    deviceExtension = DeviceObject->DeviceExtension;
    

    // Can't accept a new io request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !BulkUsb_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        Irp->IoStatus.Status = ntStatus;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest( Irp, IO_NO_INCREMENT );

        BulkUsb_DecrementIoCount(DeviceObject);                          
        return ntStatus;
    }

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    // get pointers and lengths of the caller's (user's) IO buffer
    ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

    //
    // Handle Ioctls from User mode
    //

    switch (ioControlCode) {

    case IOCTL_BULKUSB_RESET_PIPE:
        {
            PUSBD_PIPE_INFORMATION pipe;
	        PFILE_OBJECT fileObject;

		    // get our context and see if it is a pipe
	        fileObject = irpStack->FileObject;

		    pipe = (PUSBD_PIPE_INFORMATION) fileObject->FsContext;    

		    if(pipe == NULL) {
			    // error, this is not a pipe
	            ntStatus =
		            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
		    } else {            
                BulkUsb_ResetPipe(DeviceObject, pipe );
            
                ntStatus = Irp->IoStatus.Status = STATUS_SUCCESS;
            }
        }
        break;


     case IOCTL_BULKUSB_GET_CONFIG_DESCRIPTOR:

        //
        // This api returns a copy of the configuration descriptor
        // and all endpoint/interface descriptors.
        //

        //
        // inputs  - none
        // outputs - configuration descriptor plus interface
        //          and endpoint descriptors
        //

        pch = (PUCHAR) ioBuffer;

        configurationDescriptor =
            deviceExtension->UsbConfigurationDescriptor;

        if (configurationDescriptor) {
            
            length = configurationDescriptor->wTotalLength;

            RtlCopyMemory(pch,
                          (PUCHAR) configurationDescriptor,
                          length);

            Irp->IoStatus.Information = length;

            ntStatus = Irp->IoStatus.Status = STATUS_SUCCESS;
        }
        else {

            Irp->IoStatus.Information = 0;

            ntStatus = Irp->IoStatus.Status = STATUS_DEVICE_DATA_ERROR;
        }

        break;



     case IOCTL_BULKUSB_RESET_DEVICE:
        
		ntStatus = BulkUsb_ResetDevice( DeviceObject );
        break;               

    default:

        ntStatus =
            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
    }

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

    BulkUsb_DecrementIoCount(DeviceObject);                       

    return ntStatus;

}




NTSTATUS
BulkUsb_ResetDevice(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:
	Checks port status; if OK, return success and  do no more;
	If bad, attempt reset

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
    ULONG portStatus;

    BULKUSB_KdPrint(5,("Enter BulkUsb_ResetDevice()\n"));
    
    //
    // Check the port state, if it is disabled we will need 
    // to re-enable it
    //
    ntStatus = BulkUsb_GetPortStatus(DeviceObject, &portStatus);

    if (NT_SUCCESS(ntStatus) && !(portStatus & USBD_PORT_ENABLED) &&
        portStatus & USBD_PORT_CONNECTED) {
        //
        // port is disabled, attempt reset
        //
		BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_ResetDevice() will reset\n"));
        ntStatus = BulkUsb_ResetParentPort(DeviceObject);
    }
	return ntStatus;
}



NTSTATUS
BulkUsb_GetPortStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PULONG PortStatus
    )
/*++

Routine Description:

    returns the port status for our device

Arguments:

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
    PDEVICE_EXTENSION deviceExtension;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("enter BulkUsb_GetPortStatus\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    *PortStatus = 0;

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    // IoBuildDeviceIoControlRequest allocates and sets up an IRP for a device control request
    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_GET_PORT_STATUS,
                deviceExtension->TopOfStackDeviceObject, //next-lower driver's device object, representing the target device.
                NULL, // no input or output buffers
                0,
                NULL,
                0,
                TRUE, // internal ( use IRP_MJ_INTERNAL_DEVICE_CONTROL )
                &event, // event to be signalled on completion ( we wait for it below )
                &ioStatus);

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    // IoGetNextIrpStackLocation gives a higher level driver access to the next-lower 
    // driver's I/O stack location in an IRP so the caller can set it up for the lower driver.
    nextStack = IoGetNextIrpStackLocation(irp);
    BULKUSB_ASSERT(nextStack != NULL);

    nextStack->Parameters.Others.Argument1 = PortStatus;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_GetPortStatus() calling USBD port status api\n"));

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject,
                            irp);

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_GetPortStatus() return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

        BULKUSB_KdPrint( DBGLVL_DEFAULT,("Wait for single object\n"));

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

        BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_GetPortStatus() Wait for single object, returned %x\n", status));
        
    } else {
        ioStatus.Status = ntStatus;
    }

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_GetPortStatus() Port status = %x\n", *PortStatus));

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("Exit BulkUsb_GetPortStatus (%x)\n", ntStatus));


    return ntStatus;
}


NTSTATUS
BulkUsb_ResetParentPort(
    IN IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    Reset the our parent port

Arguments:

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
    PDEVICE_EXTENSION deviceExtension;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("enter BulkUsb_ResetParentPort\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_RESET_PORT,
                deviceExtension->TopOfStackDeviceObject,
                NULL,
                0,
                NULL,
                0,
                TRUE, // internal ( use IRP_MJ_INTERNAL_DEVICE_CONTROL )
                &event,
                &ioStatus);

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    nextStack = IoGetNextIrpStackLocation(irp);
    BULKUSB_ASSERT(nextStack != NULL);

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_ResetParentPort() calling USBD enable port api\n"));

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject,
                            irp);
                            
    BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_ResetParentPort() return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

        BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_ResetParentPort() Wait for single object\n"));

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

        BULKUSB_KdPrint( DBGLVL_DEFAULT,("BulkUsb_ResetParentPort() Wait for single object, returned %x\n", status));
        
    } else {
        ioStatus.Status = ntStatus;
    }

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    BULKUSB_KdPrint( DBGLVL_DEFAULT,("Exit BulkUsb_ResetPort (%x)\n", ntStatus));

    return ntStatus;
}




