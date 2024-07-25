/*++

Copyright (c) 1990, 1991, 1992, 1993, 1994 - 1998  Microsoft Corporation

Module Name:

    kbdclass.h

Abstract:

    These are the structures and defines that are used in the
    keyboard class driver.

Revision History:

--*/


#ifndef _KBDCLASS_
#define _KBDCLASS_

#define KEYBOARD_POOL_TAG 'CdbK'
#undef ExAllocatePool
#define ExAllocatePool(Type, Bytes) ExAllocatePoolWithTag(Type, Bytes, KEYBOARD_POOL_TAG)

/*
The following definition was taken from ntddkbd.h because it could not be defined earlier
when ntddkbd.h was included.
*/
DEFINE_GUID( GUID_CLASS_KEYBOARD, 0x884b96c3, 0x56ef, 0x11d1, \
             0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd);

//
// On some busses, we can power down the bus, but not the system, in this case
// we still need to allow the device to wake said bus, therefore
// waitwake-supported should not rely on systemstate.
//
// #define WAITWAKE_SUPPORTED(port) ((port)->MinDeviceWakeState > PowerDeviceUnspecified) && \
//                                  (port)->MinSystemWakeState > PowerSystemWorking)
#define WAITWAKE_SUPPORTED(port) ((port)->MinDeviceWakeState > PowerDeviceD0 && \
                                  (port)->MinSystemWakeState > PowerSystemWorking)

// #define WAITWAKE_ON(port)        ((port)->WaitWakeIrp != 0)
#define WAITWAKE_ON(port) \
       (InterlockedCompareExchangePointer((PVOID*)&(port)->WaitWakeIrp, NULL, NULL) != NULL)

#define SHOULD_SEND_WAITWAKE(port) (WAITWAKE_SUPPORTED(port) && \
                                    !WAITWAKE_ON(port)       && \
                                    KeyboardClassCheckWaitWakeEnabled(port))



typedef struct _KBD_CALL_ALL_PORTS {
    //
    // Number of ports to call
    //
    ULONG   Len;

    //
    // Current Called port;
    //
    ULONG   Current;

    //
    // Array of Ports to call
    //
    PORT*    Port;

} KBD_CALL_ALL_PORTS, *PKBD_CALL_ALL_PORTS;

//
// Keyboard configuration information.
//

typedef struct _KEYBOARD_CONFIGURATION_INFORMATION {

    //
    // Maximum size of class input data queue, in bytes.
    //

    ULONG  DataQueueSize;

} KEYBOARD_CONFIGURATION_INFORMATION, *PKEYBOARD_CONFIGURATION_INFORMATION;

typedef struct _KEYBOARD_WORK_ITEM {
    PIRP                Irp;
    PDEVICE_EXTENSION   Data;
    WORK_QUEUE_ITEM     Item;
    BOOLEAN             WaitWakeState;
    BOOLEAN             _unused[3];
} KEYBOARD_WORK_ITEM, *PKEYBOARD_WORK_ITEM;

typedef enum _POWER_COMPLETE_STATE {
    KeyboardPowerStateStart,
    KeyboardPowerStateHaveSetLights
} POWER_COMPLETE_STATE;

typedef struct _KEYBOARD_POWER_CONTEXT {
    POWER_COMPLETE_STATE    State;
    PVOID                   SystemBuffer;
    NTSTATUS                Status;
} KEYBOARD_POWER_CONTEXT, * PKEYBOARD_POWER_CONTEXT;


//
// Function Declairations
//

extern "C" {

NTSTATUS
KeyboardAddDeviceEx(
    IN PDEVICE_EXTENSION NewDeviceObject,
    IN PWCHAR            FullClassName,
    IN PFILE_OBJECT      File
    );

NTSTATUS
KeyboardAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

NTSTATUS
KeyboardClassPassThrough(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
        );

VOID
KeyboardClassCancel(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
KeyboardClassCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
KeyboardClassDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
KeyboardClassFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
KeyboardClassCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
KeyboardClassClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
KeyboardClassRead(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

BOOLEAN
KeyboardClassCheckWaitWakeEnabled(
    IN PDEVICE_EXTENSION Data
    );

BOOLEAN
KeyboardClassCreateWaitWakeIrp (
    IN PDEVICE_EXTENSION Data
    );

NTSTATUS
KeyboardClassPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
KeyboardSendIrpSynchronously (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN BOOLEAN          CopyToNext
    );

NTSTATUS
KeyboardPnP (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
KeyboardClassServiceCallback(
    IN PDEVICE_OBJECT DeviceObject,
    IN PKEYBOARD_INPUT_DATA InputDataStart,
    IN PKEYBOARD_INPUT_DATA InputDataEnd,
    IN OUT PULONG InputDataConsumed
    );

VOID
KeyboardClassStartIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
KeyboardClassUnload(
    IN PDRIVER_OBJECT DriverObject
    );

BOOLEAN
KbdCancelRequest(
    IN PVOID Context
    );

VOID
KbdConfiguration();

NTSTATUS
KbdCreateClassObject(
    IN  PDRIVER_OBJECT      DriverObject,
    IN  PDEVICE_EXTENSION   TmpDeviceExtension,
    OUT PDEVICE_OBJECT    * ClassDeviceObject,
    OUT PWCHAR            * FullDeviceName,
    IN  BOOLEAN             Legacy
    );

VOID
KbdDebugPrint(
    ULONG DebugPrintLevel,
    PCCHAR DebugMessage,
    ...
    );

NTSTATUS
KbdDeterminePortsServiced(
    IN PUNICODE_STRING BasePortName,
    IN OUT PULONG NumberPortsServiced
    );

NTSTATUS
KbdDeviceMapQueryCallback(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    );

NTSTATUS
KbdEnableDisablePort(
    IN BOOLEAN EnableFlag,
    IN PIRP    Irp,
    IN PDEVICE_EXTENSION Port,
    IN PFILE_OBJECT * File
    );

NTSTATUS
KbdSendConnectRequest(
    IN PDEVICE_EXTENSION ClassData,
    IN PVOID ServiceCallback
    );

VOID
KbdInitializeDataQueue(
    IN PVOID Context
    );

NTSTATUS
KeyboardCallAllPorts (
   PDEVICE_OBJECT Device,
   PIRP           Irp,
   PVOID
   );

NTSTATUS
KeyboardClassEnableGlobalPort(
    IN PDEVICE_EXTENSION Port,
    IN BOOLEAN Enabled
    );

NTSTATUS
KeyboardClassPlugPlayNotification(
    IN PVOID NotificationStructure,
    IN PDEVICE_EXTENSION Port
    );

void
KbdSetLights (
    PKEYBOARD_WORK_ITEM Item
    );

void
KbdFlashLights (
    PKEYBOARD_WORK_ITEM Item
    );

BOOLEAN
KeyboardClassCreateWaitWakeIrp (
    IN PDEVICE_EXTENSION Data
    );

void
KeyboardClassCreateWaitWakeIrpWorker (
    IN PKEYBOARD_WORK_ITEM Item
    );

NTSTATUS
KeyboardToggleWaitWake(
    PDEVICE_EXTENSION Data,
    BOOLEAN           WaitWakeState
    );

void
KeyboardToggleWaitWakeWorker (
    IN PKEYBOARD_WORK_ITEM Item
    );

NTSTATUS
KeyboardQueryDeviceKey (
    IN  HANDLE  Handle,
    IN  PWCHAR  ValueNameString,
    OUT PVOID   Data,
    IN  ULONG   DataLength
    );

VOID
KeyboardClassFindMorePorts(
    PDRIVER_OBJECT  DriverObject,
    PVOID           Context,
    ULONG           Count
    );

NTSTATUS
KeyboardClassSystemControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
KeyboardClassSetWmiDataItem(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG DataItemId,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    );

NTSTATUS
KeyboardClassSetWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    );


NTSTATUS
KeyboardClassQueryWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG InstanceCount,
    IN OUT PULONG InstanceLengthArray,
    IN ULONG BufferAvail,
    OUT PUCHAR Buffer
    );

NTSTATUS
KeyboardClassQueryWmiRegInfo(
    IN PDEVICE_OBJECT DeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName,
    OUT PUNICODE_STRING *RegistryPath,
    OUT PUNICODE_STRING MofResourceName,
    OUT PDEVICE_OBJECT  *Pdo
    );


}

#endif // _KBDCLASS_

