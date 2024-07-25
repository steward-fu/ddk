''
'' There are some DDK header files are included in FreeBASIC package.
'' It is not possible to build the DDK example that is included in the release package 
'' because there are many errors got from FreeBASIC compiler when building.
'' Besides, these header files are proprietary goods according to the description found in file.
'' So, I decide to write a fresh new header file for Windows driver used in FreeBASIC compiler.
'' This header file is mainly used for the tutorial examples on my website.
'' This file can be redistributed without any limitation.
'' 
'' Most of structures are defined as PVOID for convenience.
'' They will be redefined as visible name or structure when I need them later.
'' I put all of strcutures and definitions in this file.
'' So, when developing Windows driver, what you need is to include this file only.
''
'' Author:  Steward Fu (steward.fu@gmail.com)
'' Created: 2024/07/24
'' Updated: 2024/07/25
''
'' https://steward-fu.github.io/website/index.htm
''

#define STATUS_SUCCESS 0
#define IRP_MJ_PNP &h1b
#define Null 0
#define FILE_DEVICE_UNKNOWN &h22
#define DO_DEVICE_INITIALIZING &h80
#define DO_BUFFERED_IO 4
#define IRP_MN_REMOVE_DEVICE 2
#define IO_NO_INCREMENT 0

type PVOID as any ptr
type NTSTATUS as ulong
type WCHAR as wstring
type PCWSTR as const WCHAR ptr

type PIRP as IRP ptr
type PIO_STACK_LOCATION as IO_STACK_LOCATION ptr
type PDEVICE_OBJECT as DEVICE_OBJECT ptr
type PDRIVER_OBJECT as DRIVER_OBJECT ptr
type PUNICODE_STRING as UNICODE_STRING ptr
type PDRIVER_EXTENSION as DRIVER_EXTENSION ptr

type IRP_Tail_Overlay
    DeviceQueueEntry(0 to &h0f) as byte     '' 0040h
    Thread as PVOID                         '' 0050h
    AuxiliaryBuffer as PVOID                '' 0054h
    ListEntry(0 to 7) as byte               '' 0058h
    CurrentStackLocation as PVOID           '' 0060h
    OriginalFileObject as PVOID             '' 0064h
end type

union IRP_Tail
    Overlay as IRP_Tail_Overlay
    Apc as PVOID 
    CompletionKey as PVOID
end union

type IRP
    Type as ushort                      '' 0000h
    Size as ushort                      '' 0002h
    MdlAddress as PVOID                 '' 0004h
    Flags as ulong                      '' 0008h
    AssociatedIrp as PVOID              '' 000ch
    ThreadListEntry(0 to 7) as byte     '' 0010h
    IoStatus(0 to 7) as byte            '' 0018h
    RequestorMode as byte               '' 0020h
    PendingReturned as byte             '' 0021h
    StackCount as byte                  '' 0022h
    CurrentLocation as byte             '' 0023h
    Cancel as byte                      '' 0024h
    CancelIrql as byte                  '' 0025h
    ApcEnvironment as byte              '' 0026h
    AllocationFlags as byte             '' 0027h
    UserIosb as PVOID                   '' 0028h
    UserEvent as PVOID                  '' 002ch
    Overlay(0 to 7) as byte             '' 0030h
    CancelRoutine as PVOID              '' 0038h
    UserBuffer as PVOID                 '' 003ch
    Tail as IRP_Tail                    '' 0040h
end type

type IO_STACK_LOCATION
    MajorFunction as byte               '' 0000h
    MinorFunction as byte               '' 0001h
    Flags as byte                       '' 0002h
    Control as byte                     '' 0003h
    Parameters(0 to &h0f) as byte       '' 0004h
    DeviceObject as PVOID               '' 0014h
    FileObject as PVOID                 '' 0018h
    CompletionRoutine as PVOID          '' 001ch
    Context as PVOID                    '' 0020h
end type

type DEVICE_OBJECT
    Type as short                       '' 0000h
    Size as ushort                      '' 0002h
    ReferenceCount as long              '' 0004h
    DriverObject as PVOID               '' 0008h
    NextDevice as PVOID                 '' 000ch
    AttachedDevice as PVOID             '' 0010h
    CurrentIrp as PVOID                 '' 0014h
    Timer as PVOID                      '' 0018h
    Flags as ulong                      '' 001ch
    Characteristics as ulong            '' 0020h
    Vpb as PVOID                        '' 0024h
    DeviceExtension as PVOID            '' 0028h
    DeviceType as ulong                 '' 002ch
    StackSize as byte                   '' 0030h
    _rev(0 to &h2) as byte              '' 0031h
    Queue(0 to &h27) as byte            '' 0034h
    AlignmentRequirement as ulong       '' 005ch
    DeviceQueue(0 to &h13) as byte      '' 0060h
    Dpc(0 to &h1f) as byte              '' 0074h
    ActiveThreadCount as ulong          '' 0094h
    SecurityDescriptor as PVOID         '' 0098h
    DeviceLock(0 to &h0f) as byte       '' 009ch
    SectorSize as ushort                '' 00ach
    Spare1 as ushort                    '' 00aeh
    DeviceObjectExtension as PVOID      '' 00b0h
    Reserved as PVOID                   '' 00b4h
end type

type UNICODE_STRING
    Length as ushort                    '' 0000h
    MaximumLength as ushort             '' 0002h
    Buffer as PVOID                     '' 0004h
end type

type DRIVER_EXTENSION
    DriverObject as PVOID                   '' 0000h
    AddDevice as PVOID                      '' 0004h
    Count as ulong                          '' 0008h
    ServiceKeyName as UNICODE_STRING        '' 000ch
    ClientDriverExtension as PVOID          '' 0014h
end type

type DRIVER_OBJECT
    Type as short                           '' 0000h
    Size as short                           '' 0002h
    DeviceObject as PVOID                   '' 0004h 
    Flags as ulong                          '' 0008h
    DriverStart as PVOID                    '' 000ch
    DriverSize as ulong                     '' 0010h
    DriverSection as PVOID                  '' 0014h
    DriverExtension as PDRIVER_EXTENSION    '' 0018h
    DriverName as UNICODE_STRING            '' 001ch
    HardwareDatabase as PVOID               '' 0024h
    FastIoDispatch as PVOID                 '' 0028h
    DriverInit as PVOID                     '' 002ch
    DriverStartIo as PVOID                  '' 0030h
    DriverUnload as PVOID                   '' 0034h
    MajorFunction(0 to &h1b) as PVOID       '' 0038h
end type

declare function DbgPrint cdecl alias "DbgPrint" (byval Format as byte ptr, ...) as ulong
declare function IoCreateDevice stdcall alias "IoCreateDevice" (_
    byval DriverObject as PDRIVER_OBJECT, _
    byval DeviceExtensionSize as ulong, _
    byval DeviceName as PUNICODE_STRING, _
    byval DeviceType as ulong, _
    byval DeviceCharacteristics as ulong, _
    byval Exclusive as ulong, _
    byval DeviceObject as PDEVICE_OBJECT ptr) as NTSTATUS
declare function IoAttachDeviceToDeviceStack stdcall alias "IoAttachDeviceToDeviceStack" (byval SourceDevice as PDEVICE_OBJECT, byval TargetDevice as PDEVICE_OBJECT) as PDEVICE_OBJECT
declare function IoGetCurrentIrpStackLocation stdcall alias "IoGetCurrentIrpStackLocation" (byval pIrp as PIRP) as PIO_STACK_LOCATION
declare function IoCallDriver stdcall alias "IoCallDriver" (byval DeviceObject as PDEVICE_OBJECT, byval Irp as PIRP) as NTSTATUS

declare sub IoCompleteRequest stdcall alias "IoCompleteRequest" (byval Irp as PIRP, byval PriorityBoost as ulong)
declare sub IoDeleteDevice stdcall alias "IoDeleteDevice" (byval DeviceObject as PDEVICE_OBJECT)
declare sub IoDetachDevice stdcall alias "IoDetachDevice" (byval TargetDevice as PDEVICE_OBJECT)
declare sub RtlInitUnicodeString stdcall alias "RtlInitUnicodeString" (byval DestinationString as PUNICODE_STRING, byval SourceString as PCWSTR)

sub IoSkipCurrentIrpStackLocation(byval pIrp as PIRP)
    (*pIrp).CurrentLocation = (*pIrp).CurrentLocation + 1
    (*pIrp).Tail.Overlay.CurrentStackLocation = (*pIrp).Tail.Overlay.CurrentStackLocation + 36 
end sub

function IoGetCurrentIrpStackLocation(byval pIrp as PIRP) as PIO_STACK_LOCATION
    return (*pIrp).Tail.Overlay.CurrentStackLocation
end function