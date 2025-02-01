/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    Iso82930.h

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
#ifndef ISO_8293_INCD
#define ISO_8293_INCD

#include "IusbDbg.h"

#define ISO_MAX_TRANSFER_SIZE 256	

// used to track driver-generated io irps for  read/write processing
typedef struct _ISOUSB_RW_CONTEXT {
    PURB Urb;
    PDEVICE_OBJECT DeviceObject;
    PIRP  Irp;
} ISOUSB_RW_CONTEXT, *PISOUSB_RW_CONTEXT;

#define ISOUSB_MAX_IRP  2		// number of transfer objects to keep recycling


#define ISOUSB_STREAM_TIMEOUT_INTERVAL  100 // MS


typedef struct _ISOUSB_TRANSFER_OBJECT {
    struct _ISOUSB_STREAM_OBJECT *StreamObject;
    PIRP Irp;
    PURB Urb;
    PUCHAR DataBuffer;
} ISOUSB_TRANSFER_OBJECT, *PISOUSB_TRANSFER_OBJECT;

typedef struct _ISOUSB_STREAM_OBJECT {
    PDEVICE_OBJECT DeviceObject;
    ULONG PendingIrps;
    PISOUSB_TRANSFER_OBJECT TransferObjectList[ISOUSB_MAX_IRP];
    PUSBD_PIPE_INFORMATION PipeInfo;
    KDPC TimeoutDpc;
    KTIMER  TimeoutTimer;
	KEVENT NoPendingIrpEvent;
    BOOLEAN IsoStreamStarted;
	ULONG TimesRecycled;
    ULONG TotalPacketsProcessed;
    ULONG TotalBytesProcessed;
    ULONG ErrorPacketCount;

} ISOUSB_STREAM_OBJECT, *PISOUSB_STREAM_OBJECT;

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


	// set when PendingIoCount goes to 0; flags device can be removed
    KEVENT RemoveEvent;


	// set when PendingIoCount goes to 1 ( 1st increment was on add device )
	// this indicates no IO requests outstanding, either user, system, or otherwise
    KEVENT NoPendingIoEvent;

	// set to signal driver-generated power request is finished
    KEVENT SelfRequestedPowerIrpEvent;

	// incremented when device is added and any IO request is received;
	// decremented when any io request is completed or passed on, and when device is removed
    ULONG PendingIoCount;

	// count of open pipes
	ULONG OpenPipeCount;


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

	// default maximum transfer per io		
	ULONG MaximumTransferSize;  

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


// function prototypes

NTSTATUS
IsoUsb_ProcessPnPIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
IsoUsb_StartIsoStream(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
IsoUsb_StopIsoStream(
    IN PDEVICE_OBJECT DeviceObject,
    IN PISOUSB_STREAM_OBJECT StreamObject,
    IN PIRP Irp
    );

NTSTATUS
IsoUsb_IsoIrp_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
	IN PVOID Context
    );

NTSTATUS
IsoUsb_ProcessSysControlIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

VOID
IsoUsb_Unload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
IsoUsb_StartDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IsoUsb_StopDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IsoUsb_RemoveDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IsoUsb_CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB Urb
    );

NTSTATUS
IsoUsb_PnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );

NTSTATUS
IsoUsb_CreateDeviceObject(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PDEVICE_OBJECT *DeviceObject
    );

NTSTATUS
IsoUsb_ConfigureDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IsoUsb_IrpCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
IsoUsb_PoRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    );

NTSTATUS
IsoUsb_PoSelfRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    );

PURB
IsoUsb_BuildIsoRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PUSBD_PIPE_INFORMATION PipeHandle,
    IN BOOLEAN Read
    );

NTSTATUS
IsoUsb_GetPortStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PULONG PortStatus
    );

NTSTATUS
IsoUsb_ResetParentPort(
    IN IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IsoUsb_SelfRequestPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN POWER_STATE PowerState
    );

BOOLEAN
IsoUsb_SetDevicePowerState(
    IN PDEVICE_OBJECT DeviceObject,
    IN DEVICE_POWER_STATE DeviceState
    );

NTSTATUS
IsoUsb_IsoReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
IsoUsb_PowerIrp_Complete(
    IN PDEVICE_OBJECT NullDeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
IsoUsb_QueryCapabilities(
    IN PDEVICE_OBJECT PdoDeviceObject,
    IN PDEVICE_CAPABILITIES DeviceCapabilities
    );


NTSTATUS
IsoUsb_Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
IsoUsb_Create(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
IsoUsb_Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
IsoUsb_AbortPipes(
    IN PDEVICE_OBJECT DeviceObject
    );


NTSTATUS
IsoUsb_ProcessIOCTL(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
IsoUsb_SelectInterface(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    );



NTSTATUS
IsoUsb_ResetDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IsoUsb_Close(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
IsoUsb_ResetPipe(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSBD_PIPE_INFORMATION Pipe,
    IN BOOLEAN IsoClearStall
    );

VOID
IsoUsb_IncrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    );

LONG
IsoUsb_DecrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    );   


NTSTATUS
IsoUsb_ProcessPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );    



NTSTATUS
IsoUsb_SelfSuspendOrActivate(
    IN PDEVICE_OBJECT DeviceObject,
	IN BOOLEAN fSuspend
    );

NTSTATUS 
IsoUsb_SymbolicLink(
    IN PDEVICE_OBJECT DeviceObject, 
	IN OUT PUNICODE_STRING deviceLinkUnicodeString
    );



BOOLEAN
IsoUsb_CanAcceptIoRequests(
    IN PDEVICE_OBJECT DeviceObject
    );

ULONG
IsoUsb_GetCurrentFrame (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
IsoUsb_CompletionStop (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN PVOID            Context
    );


#endif // already included






