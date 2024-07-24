''
'' Windows Driver Header for FreeBASIC Compiler
'' (Only Test on Windows 7 and XP)
''
'' Author:  Steward Fu (steward.fu@gmail.com)
'' Created: 2024/07/24
'' Updated: 2024/07/24
''

type FAST_IO_DISPATCH as _FAST_IO_DISPATCH
type PFAST_IO_DISPATCH as _FAST_IO_DISPATCH ptr

type DEVICE_OBJECT as _DEVICE_OBJECT
type PDEVICE_OBJECT as DEVICE_OBJECT ptr

type DRIVER_OBJECT as _DRIVER_OBJECT
type PDRIVER_OBJECT as DRIVER_OBJECT ptr

type IRP as _IRP
type PIRP as IRP ptr

type DRIVER_EXTENSION as _DRIVER_EXTENSION
type PDRIVER_EXTENSION as _DRIVER_EXTENSION ptr

type _DRIVER_OBJECT
    Type as _CSHORT
    Size as _CSHORT
    DeviceObject as PDEVICE_OBJECT
    Flags as ULONG
    DriverStart as PVOID
    DriverSize as ULONG
    DriverSection as PVOID
    DriverExtension as PDRIVER_EXTENSION
    DriverName as UNICODE_STRING
    HardwareDatabase as PUNICODE_STRING
    FastIoDispatch as PFAST_IO_DISPATCH
    DriverInit as PVOID
    DriverStartIo as PVOID
    DriverUnload as PVOID
    MajorFunction(0 to &H1b+1-1) as PVOID
end type
