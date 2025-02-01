/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    Blk82930.h

Abstract:

	Kernel mode definitions and function prototypes

Environment:

    Kernel mode

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.

Revision History:

    11/18/97 : created

--*/
#ifndef BLK829_INCD
#define BLK829_INCD

#include "BusbDbg.h"

// the size of the transfer buffer on your test board or device
#define BULKUSB_TEST_BOARD_TRANSFER_BUFFER_SIZE (64 *1024 )

#define BULKUSB_MAX_TRANSFER_SIZE	256

// used to track driver-generated io irps for staged read/write processing
typedef struct _BULKUSB_RW_CONTEXT {
    PURB Urb;
    PDEVICE_OBJECT DeviceObject;
    PIRP  Irp;
} BULKUSB_RW_CONTEXT, *PBULKUSB_RW_CONTEXT;


//
// A structure representing the instance information associated with
// this particular device.
//

typedef struct _DEVICE_EXTENSION {

    // Device object we call when submitting Urbs
    PDEVICE_OBJECT TopOfStackDeviceObject;

	// The bus driver object
    PDEVICE_OBJECT PhysicalDeviceObject;

    DEVICE_POWER_STATE CurrentDevicePowerState;

    // USB configuration handle and ptr for the configuration the
    // device is currently in
    USBD_CONFIGURATION_HANDLE UsbConfigurationHandle;
	PUSB_CONFIGURATION_DESCRIPTOR UsbConfigurationDescriptor;


    // ptr to the USB device descriptor
    // for this device
    PUSB_DEVICE_DESCRIPTOR UsbDeviceDescriptor;

    // we support one interface
    // this is a copy of the info structure
    // returned from select_configuration or
    // select_interface
    PUSBD_INTERFACE_INFORMATION UsbInterface;

	//Bus drivers set the appropriate values in this structure in response
	//to an IRP_MN_QUERY_CAPABILITIES IRP. Function and filter drivers might
	//alter the capabilities set by the bus driver.
    DEVICE_CAPABILITIES DeviceCapabilities;

	// used to save the currently-being-handled system-requested power irp request
    PIRP PowerIrp;

	// used to save base Irp ( user-originated via IOCTL ) of staged read/write request
	PIRP BaseIrp;

	// count of self-staged irps pending
    ULONG StagedPendingIrpCount;

	// count of self-staged bytes read or written so far
    ULONG StagedBytesTransferred;

	// set when PendingIoCount goes to 0; flags device can be removed
    KEVENT RemoveEvent;

	// set when BulkUsb_AsyncReadWrite_Complete() finishes or cancels last staged io irp
    KEVENT StagingDoneEvent;

	// set when PendingIoCount goes to 1 ( 1st increment was on add device )
	// this indicates no IO requests outstanding, either user, system, or self-staged
    KEVENT NoPendingIoEvent;

	// set to signal driver-generated power request is finished
    KEVENT SelfRequestedPowerIrpEvent;

	// spinlock used to protect inc/dec iocount logic
	KSPIN_LOCK	IoCountSpinLock;

	// incremented when device is added and any IO request is received;
	// decremented when any io request is completed or passed on, and when device is removed
    ULONG PendingIoCount;

	// count of open pipes
	ULONG OpenPipeCount;

	// save ptr to array of info on self-generated IRPS for staged read/writes;
	//  will allocate this separately
	PBULKUSB_RW_CONTEXT PendingIoIrps;

    // Name buffer for our named Functional device object link
	// The name is generated based on the driver's class GUID
    WCHAR DeviceLinkNameBuffer[ MAXIMUM_FILENAME_LENGTH ];  // MAXIMUM_FILENAME_LENGTH defined in wdm.h

	//flag set when processing IRP_MN_REMOVE_DEVICE
    BOOLEAN DeviceRemoved;

 	// flag set when driver has answered success to IRP_MN_QUERY_REMOVE_DEVICE
    BOOLEAN RemoveDeviceRequested;

	// flag set when driver has answered success to IRP_MN_QUERY_STOP_DEVICE
    BOOLEAN StopDeviceRequested;

	// flag set when device has been successfully started
	BOOLEAN DeviceStarted;

    // flag set when IRP_MN_WAIT_WAKE is received and we're in a power state
    // where we can signal a wait
    BOOLEAN EnabledForWakeup;

	// used to flag that we're currently handling a self-generated power request
    BOOLEAN SelfPowerIrp;

	// default power state to power down to on self-suspend 
	ULONG PowerDownLevel; 

	// default maximum transfer per staged irp size		
	ULONG MaximumTransferSize;  

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


// function prototypes

NTSTATUS
BulkUsb_ProcessPnPIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
BulkUsb_ProcessSysControlIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

VOID
BulkUsb_Unload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
BulkUsb_StartDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
BulkUsb_StopDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
BulkUsb_RemoveDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
BulkUsb_CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB Urb
    );

NTSTATUS
BulkUsb_PnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );

NTSTATUS
BulkUsb_CreateDeviceObject(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PDEVICE_OBJECT *DeviceObject
    );

NTSTATUS
BulkUsb_ConfigureDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
BulkUsb_IrpCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
BulkUsb_PoRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    );

NTSTATUS
BulkUsb_PoSelfRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    );

PURB
BulkUsb_BuildAsyncRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PUSBD_PIPE_INFORMATION PipeHandle,
    IN BOOLEAN Read
    );

NTSTATUS
BulkUsb_GetPortStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PULONG PortStatus
    );

NTSTATUS
BulkUsb_ResetParentPort(
    IN IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
BulkUsb_SelfRequestPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN POWER_STATE PowerState
    );

BOOLEAN
BulkUsb_SetDevicePowerState(
    IN PDEVICE_OBJECT DeviceObject,
    IN DEVICE_POWER_STATE DeviceState
    );

NTSTATUS
BulkUsb_AsyncReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
BulkUsb_PowerIrp_Complete(
    IN PDEVICE_OBJECT NullDeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
BulkUsb_QueryCapabilities(
    IN PDEVICE_OBJECT PdoDeviceObject,
    IN PDEVICE_CAPABILITIES DeviceCapabilities
    );


NTSTATUS
BulkUsb_Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BulkUsb_Create(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
BulkUsb_Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
BulkUsb_AbortPipes(
    IN PDEVICE_OBJECT DeviceObject
    );


NTSTATUS
BulkUsb_ProcessIOCTL(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BulkUsb_SelectInterface(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    );



NTSTATUS
BulkUsb_ResetDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
BulkUsb_Close(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BulkUsb_ResetPipe(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSBD_PIPE_INFORMATION PipeInfo
    );


VOID
BulkUsb_IncrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    );

LONG
BulkUsb_DecrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    );   


NTSTATUS
BulkUsb_ProcessPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );    


NTSTATUS
BulkUsb_StagedReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN BOOLEAN Read
    );

NTSTATUS
BulkUsb_SelfSuspendOrActivate(
    IN PDEVICE_OBJECT DeviceObject,
	IN BOOLEAN fSuspend
    );

NTSTATUS 
BulkUsb_SymbolicLink(
    IN PDEVICE_OBJECT DeviceObject, 
	IN OUT PUNICODE_STRING deviceLinkUnicodeString
    );


BOOLEAN
BulkUsb_CancelPendingIo(
    IN PDEVICE_OBJECT DeviceObject
    );

BOOLEAN
BulkUsb_CanAcceptIoRequests(
    IN PDEVICE_OBJECT DeviceObject
    );


#endif // already included






