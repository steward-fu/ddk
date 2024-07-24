;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;//typedef enum _WDF_FILEOBJECT_CLASS {
;//    WdfFileObjectInvalid = 0,
;//    WdfFileObjectNotRequired = 1,
;//    WdfFileObjectWdfCanUseFsContext = 2,
;//    WdfFileObjectWdfCanUseFsContext2 = 3,
;//    WdfFileObjectWdfCannotUseFsContexts = 4,
;//    WdfFileObjectCanBeOptional = 0x80000000,
;//} WDF_FILEOBJECT_CLASS, *PWDF_FILEOBJECT_CLASS;
WDF_FILEOBJECT_CLASS typedef DWORD
PWDF_FILEOBJECT_CLASS typedef ptr WDF_FILEOBJECT_CLASS
WdfFileObjectInvalid                equ 0
WdfFileObjectNotRequired            equ 1
WdfFileObjectWdfCanUseFsContext     equ 2
WdfFileObjectWdfCanUseFsContext2    equ 3
WdfFileObjectWdfCannotUseFsContexts equ 4
WdfFileObjectCanBeOptional          equ 80000000h

;//typedef
;//__drv_functionClass(EVT_WDF_FILE_CLOSE)
;//__drv_sameIRQL
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//VOID
;//EVT_WDF_FILE_CLOSE(
;//    __in
;//    WDFFILEOBJECT FileObject
;//    );
;//
;//typedef EVT_WDF_FILE_CLOSE *PFN_WDF_FILE_CLOSE;
EVT_WDF_FILE_CLOSE typedef proto :DWORD
PFN_WDF_FILE_CLOSE typedef ptr EVT_WDF_FILE_CLOSE

;//typedef
;//__drv_functionClass(EVT_WDF_FILE_CLEANUP)
;//__drv_sameIRQL
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//VOID
;//EVT_WDF_FILE_CLEANUP(
;//    __in
;//    WDFFILEOBJECT FileObject
;//    );
;//
;//typedef EVT_WDF_FILE_CLEANUP *PFN_WDF_FILE_CLEANUP;
EVT_WDF_FILE_CLEANUP typedef proto :DWORD
PFN_WDF_FILE_CLEANUP typedef ptr EVT_WDF_FILE_CLEANUP

;//typedef
;//__drv_functionClass(EVT_WDF_DEVICE_FILE_CREATE)
;//__drv_sameIRQL
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//VOID
;//EVT_WDF_DEVICE_FILE_CREATE(
;//    __in
;//    WDFDEVICE Device,
;//    __in
;//    WDFREQUEST Request,
;//    __in
;//    WDFFILEOBJECT FileObject
;//    );
;//
;//typedef EVT_WDF_DEVICE_FILE_CREATE *PFN_WDF_DEVICE_FILE_CREATE;
EVT_WDF_DEVICE_FILE_CREATE typedef proto :DWORD, :DWORD, :DWORD
PFN_WDF_DEVICE_FILE_CREATE typedef ptr EVT_WDF_DEVICE_FILE_CREATE

;//typedef struct _WDF_FILEOBJECT_CONFIG {
;//    //
;//    // Size of this structure in bytes
;//    //
;//    ULONG Size;
;//
;//    //
;//    // Event callback for create requests
;//    //
;//    PFN_WDF_DEVICE_FILE_CREATE  EvtDeviceFileCreate;
;//
;//    //
;//    // Event callback for close requests
;//    //
;//    PFN_WDF_FILE_CLOSE   EvtFileClose;
;//
;//    //
;//    // Event callback for cleanup requests
;//    //
;//    PFN_WDF_FILE_CLEANUP EvtFileCleanup;
;//
;//    //
;//    // If WdfTrue, create/cleanup/close file object related requests will be
;//    //      sent down the stack.
;//    //
;//    // If WdfFalse, create/cleanup/close will be completed at this location in
;//    //      the device stack.
;//    //
;//    // If WdfDefault, behavior depends on device type
;//    //      FDO, PDO, Control:  use the WdfFalse behavior
;//    //      Filter:  use the WdfTrue behavior
;//    //
;//    WDF_TRI_STATE AutoForwardCleanupClose;
;//
;//    //
;//    // Specify whether framework should create WDFFILEOBJECT and also
;//    // whether it can FsContexts fields in the WDM fileobject to store
;//    // WDFFILEOBJECT so that it can avoid table look up and improve perf.
;//    //
;//    WDF_FILEOBJECT_CLASS FileObjectClass;
;//
;//} WDF_FILEOBJECT_CONFIG, *PWDF_FILEOBJECT_CONFIG;
WDF_FILEOBJECT_CONFIG struct
    _Size DWORD ?
    EvtDeviceFileCreate PFN_WDF_DEVICE_FILE_CREATE ?
    EvtFileClose PFN_WDF_FILE_CLOSE ?
    EvtFileCleanup PFN_WDF_FILE_CLEANUP ?
    AutoForwardCleanupClose WDF_TRI_STATE ?
    FileObjectClass WDF_FILEOBJECT_CLASS ?
WDF_FILEOBJECT_CONFIG ends
PWDF_FILEOBJECT_CONFIG typedef ptr WDF_FILEOBJECT_CONFIG

;//VOID
;//FORCEINLINE
;//WDF_FILEOBJECT_CONFIG_INIT(
;//    __out PWDF_FILEOBJECT_CONFIG FileEventCallbacks,
;//    __in_opt PFN_WDF_DEVICE_FILE_CREATE EvtDeviceFileCreate,
;//    __in_opt PFN_WDF_FILE_CLOSE EvtFileClose,
;//    __in_opt PFN_WDF_FILE_CLEANUP EvtFileCleanup
;//    )
;//{
;//    FileEventCallbacks->Size = sizeof(WDF_FILEOBJECT_CONFIG);
;//
;//    FileEventCallbacks->EvtDeviceFileCreate  = EvtDeviceFileCreate;
;//    FileEventCallbacks->EvtFileClose   = EvtFileClose;
;//    FileEventCallbacks->EvtFileCleanup = EvtFileCleanup;
;//
;//    FileEventCallbacks->FileObjectClass = WdfFileObjectWdfCannotUseFsContexts;
;//    FileEventCallbacks->AutoForwardCleanupClose = WdfUseDefault;
;//}
.code 
WDF_FILEOBJECT_CONFIG_INIT proc FileEventCallbacks:PWDF_FILEOBJECT_CONFIG, EvtDeviceFileCreate:PFN_WDF_DEVICE_FILE_CREATE, EvtFileClose:PFN_WDF_FILE_CLOSE, EvtFileCleanup:PFN_WDF_FILE_CLEANUP
  mov eax, FileEventCallbacks
  mov (WDF_FILEOBJECT_CONFIG ptr [eax])._Size, sizeof WDF_FILEOBJECT_CONFIG
  push EvtDeviceFileCreate
  pop (WDF_FILEOBJECT_CONFIG ptr [eax]).EvtDeviceFileCreate
  push EvtFileClose
  pop (WDF_FILEOBJECT_CONFIG ptr [eax]).EvtFileClose
  push EvtFileCleanup
  pop (WDF_FILEOBJECT_CONFIG ptr [eax]).EvtFileCleanup
  mov (WDF_FILEOBJECT_CONFIG ptr [eax]).FileObjectClass, WdfFileObjectWdfCannotUseFsContexts
  mov (WDF_FILEOBJECT_CONFIG ptr [eax]).AutoForwardCleanupClose, WdfUseDefault
  ret
WDF_FILEOBJECT_CONFIG_INIT endp

;////
;//// WDF Function: WdfDeviceCreate
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFDEVICECREATE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __inout
;//    PWDFDEVICE_INIT* DeviceInit,
;//    __in_opt
;//    PWDF_OBJECT_ATTRIBUTES DeviceAttributes,
;//    __out
;//    WDFDEVICE* Device
;//    );
;//
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfDeviceCreate(
;//    __inout
;//    PWDFDEVICE_INIT* DeviceInit,
;//    __in_opt
;//    PWDF_OBJECT_ATTRIBUTES DeviceAttributes,
;//    __out
;//    WDFDEVICE* Device
;//    )
;//{
;//    return ((PFN_WDFDEVICECREATE) WdfFunctions[WdfDeviceCreateTableIndex])(WdfDriverGlobals, DeviceInit, DeviceAttributes, Device);
;//}
FN_WDFDEVICECREATE typedef proto WDFAPI :dword, :dword, :dword, :dword
PFN_WDFDEVICECREATE typedef ptr FN_WDFDEVICECREATE
.code
WdfDeviceCreate proc DeviceInit:PPWDFDEVICE_INIT, DeviceAttributes:PWDF_OBJECT_ATTRIBUTES, Device:PWDFDEVICE
  mov eax, WdfFunctions
  add eax, WdfDeviceCreateTableIndex * PFN_WDFDEVICECREATE
  invoke (FN_WDFDEVICECREATE ptr [eax]), WdfDriverGlobals, DeviceInit, DeviceAttributes, Device
  ret
WdfDeviceCreate endp

;////
;//// WDF Function: WdfDeviceCreateSymbolicLink
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFDEVICECREATESYMBOLICLINK)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFDEVICE Device,
;//    __in
;//    PCUNICODE_STRING SymbolicLinkName
;//    );
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfDeviceCreateSymbolicLink(
;//    __in
;//    WDFDEVICE Device,
;//    __in
;//    PCUNICODE_STRING SymbolicLinkName
;//    )
;//{
;//    return ((PFN_WDFDEVICECREATESYMBOLICLINK) WdfFunctions[WdfDeviceCreateSymbolicLinkTableIndex])(WdfDriverGlobals, Device, SymbolicLinkName);
;//}
FN_WDFDEVICECREATESYMBOLICLINK typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFDEVICECREATESYMBOLICLINK typedef ptr FN_WDFDEVICECREATESYMBOLICLINK
.code
WdfDeviceCreateSymbolicLink proc Device:WDFDEVICE, SymbolicLinkName:PUNICODE_STRING
  mov eax, WdfFunctions
  add eax, WdfDeviceCreateSymbolicLinkTableIndex * PFN_WDFDEVICECREATESYMBOLICLINK
  invoke (FN_WDFDEVICECREATESYMBOLICLINK ptr [eax]), WdfDriverGlobals, Device, SymbolicLinkName
  ret
WdfDeviceCreateSymbolicLink endp

;////
;//// WDF Function: WdfDeviceCreateDeviceInterface
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFDEVICECREATEDEVICEINTERFACE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFDEVICE Device,
;//    __in
;//    CONST GUID* InterfaceClassGUID,
;//    __in_opt
;//    PCUNICODE_STRING ReferenceString
;//    );
;//
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfDeviceCreateDeviceInterface(
;//    __in
;//    WDFDEVICE Device,
;//    __in
;//    CONST GUID* InterfaceClassGUID,
;//    __in_opt
;//    PCUNICODE_STRING ReferenceString
;//    )
;//{
;//    return ((PFN_WDFDEVICECREATEDEVICEINTERFACE) WdfFunctions[WdfDeviceCreateDeviceInterfaceTableIndex])(WdfDriverGlobals, Device, InterfaceClassGUID, ReferenceString);
;//}
FN_WDFDEVICECREATEDEVICEINTERFACE typedef proto WDFAPI :dword, :dword, :dword, :dword
PFN_WDFDEVICECREATEDEVICEINTERFACE typedef ptr FN_WDFDEVICECREATEDEVICEINTERFACE
.code
WdfDeviceCreateDeviceInterface proc Device:WDFDEVICE, InterfaceClassGUID:DWORD, ReferenceString:PCUNICODE_STRING
  mov eax, WdfFunctions
  add eax, WdfDeviceCreateDeviceInterfaceTableIndex * PFN_WDFDEVICECREATEDEVICEINTERFACE
  invoke (FN_WDFDEVICECREATEDEVICEINTERFACE ptr [eax]), WdfDriverGlobals, Device, InterfaceClassGUID, ReferenceString
  ret
WdfDeviceCreateDeviceInterface endp

;////
;//// WDF Function: WdfDeviceInitAssignName
;////
;//typedef
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFDEVICEINITASSIGNNAME)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    PWDFDEVICE_INIT DeviceInit,
;//    __in_opt
;//    PCUNICODE_STRING DeviceName
;//    );
;//
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfDeviceInitAssignName(
;//    __in
;//    PWDFDEVICE_INIT DeviceInit,
;//    __in_opt
;//    PCUNICODE_STRING DeviceName
;//    )
;//{
;//    return ((PFN_WDFDEVICEINITASSIGNNAME) WdfFunctions[WdfDeviceInitAssignNameTableIndex])(WdfDriverGlobals, DeviceInit, DeviceName);
;//}
FN_WDFDEVICEINITASSIGNNAME typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFDEVICEINITASSIGNNAME typedef ptr FN_WDFDEVICEINITASSIGNNAME
.code
WdfDeviceInitAssignName proc DeviceInit:PWDFDEVICE_INIT, DeviceName:PCUNICODE_STRING
  mov eax, WdfFunctions
  add eax, WdfDeviceInitAssignNameTableIndex * PFN_WDFDEVICEINITASSIGNNAME
  invoke (FN_WDFDEVICEINITASSIGNNAME ptr [eax]), WdfDriverGlobals, DeviceInit, DeviceName
  ret
WdfDeviceInitAssignName endp

;////
;//// WDF Function: WdfDeviceInitSetFileObjectConfig
;////
;//typedef
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//VOID
;//(*PFN_WDFDEVICEINITSETFILEOBJECTCONFIG)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    PWDFDEVICE_INIT DeviceInit,
;//    __in
;//    PWDF_FILEOBJECT_CONFIG FileObjectConfig,
;//    __in_opt
;//    PWDF_OBJECT_ATTRIBUTES FileObjectAttributes
;//    );
;//
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//FORCEINLINE
;//WdfDeviceInitSetFileObjectConfig(
;//    __in
;//    PWDFDEVICE_INIT DeviceInit,
;//    __in
;//    PWDF_FILEOBJECT_CONFIG FileObjectConfig,
;//    __in_opt
;//    PWDF_OBJECT_ATTRIBUTES FileObjectAttributes
;//    )
;//{
;//    ((PFN_WDFDEVICEINITSETFILEOBJECTCONFIG) WdfFunctions[WdfDeviceInitSetFileObjectConfigTableIndex])(WdfDriverGlobals, DeviceInit, FileObjectConfig, FileObjectAttributes);
;//}
FN_WDFDEVICEINITSETFILEOBJECTCONFIG typedef proto WDFAPI :dword, :dword, :dword, :dword
PFN_WDFDEVICEINITSETFILEOBJECTCONFIG typedef ptr FN_WDFDEVICEINITSETFILEOBJECTCONFIG
.code
WdfDeviceInitSetFileObjectConfig proc DeviceInit:PWDFDEVICE_INIT, FileObjectConfig:PWDF_FILEOBJECT_CONFIG, FileObjectAttributes:PWDF_OBJECT_ATTRIBUTES
  mov eax, WdfFunctions
  add eax, WdfDeviceInitSetFileObjectConfigTableIndex * PFN_WDFDEVICEINITSETFILEOBJECTCONFIG
  invoke (FN_WDFDEVICEINITSETFILEOBJECTCONFIG ptr [eax]), WdfDriverGlobals, DeviceInit, FileObjectConfig, FileObjectAttributes
  ret
WdfDeviceInitSetFileObjectConfig endp

;////
;//// WDF Function: WdfDeviceInitSetIoType
;////
;//typedef
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//VOID
;//(*PFN_WDFDEVICEINITSETIOTYPE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    PWDFDEVICE_INIT DeviceInit,
;//    __in
;//    WDF_DEVICE_IO_TYPE IoType
;//    );
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//VOID
;//FORCEINLINE
;//WdfDeviceInitSetIoType(
;//    __in
;//    PWDFDEVICE_INIT DeviceInit,
;//    __in
;//    WDF_DEVICE_IO_TYPE IoType
;//    )
;//{
;//    ((PFN_WDFDEVICEINITSETIOTYPE) WdfFunctions[WdfDeviceInitSetIoTypeTableIndex])(WdfDriverGlobals, DeviceInit, IoType);
;//}
FN_WDFDEVICEINITSETIOTYPE typedef proto WDFAPI :dword, :dword, :dword
PFN_WDFDEVICEINITSETIOTYPE typedef ptr FN_WDFDEVICEINITSETIOTYPE
.code
WdfDeviceInitSetIoType proc DeviceInit:PWDFDEVICE_INIT, IoType:WDF_DEVICE_IO_TYPE
  mov eax, WdfFunctions
  add eax, WdfDeviceInitSetIoTypeTableIndex * PFN_WDFDEVICEINITSETIOTYPE
  invoke (FN_WDFDEVICEINITSETIOTYPE ptr [eax]), WdfDriverGlobals, DeviceInit, IoType
  ret
WdfDeviceInitSetIoType endp

;////
;//// WDF Function: WdfDeviceWdmGetDeviceObject
;////
;//typedef
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//WDFAPI
;//PDEVICE_OBJECT
;//(*PFN_WDFDEVICEWDMGETDEVICEOBJECT)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    WDFDEVICE Device
;//    );
;//__drv_maxIRQL(DISPATCH_LEVEL)
;//PDEVICE_OBJECT
;//FORCEINLINE
;//WdfDeviceWdmGetDeviceObject(
;//    __in
;//    WDFDEVICE Device
;//    )
;//{
;//    return ((PFN_WDFDEVICEWDMGETDEVICEOBJECT) WdfFunctions[WdfDeviceWdmGetDeviceObjectTableIndex])(WdfDriverGlobals, Device);
;//}
FN_WDFDEVICEWDMGETDEVICEOBJECT typedef proto WDFAPI :dword, :dword
PFN_WDFDEVICEWDMGETDEVICEOBJECT typedef ptr FN_WDFDEVICEWDMGETDEVICEOBJECT
.code
WdfDeviceWdmGetDeviceObject proc Device:WDFDEVICE
  mov eax, WdfFunctions
  add eax, WdfDeviceWdmGetDeviceObjectTableIndex * PFN_WDFDEVICEWDMGETDEVICEOBJECT
  invoke (FN_WDFDEVICEWDMGETDEVICEOBJECT ptr [eax]), WdfDriverGlobals, Device
  ret
WdfDeviceWdmGetDeviceObject endp
