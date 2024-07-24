;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;//typedef struct _WDF_DRIVER_CONFIG {
;//    //
;//    // Size of this structure in bytes
;//    //
;//    ULONG Size;
;//
;//    //
;//    // Event callbacks
;//    //
;//    PFN_WDF_DRIVER_DEVICE_ADD EvtDriverDeviceAdd;
;//
;//    PFN_WDF_DRIVER_UNLOAD    EvtDriverUnload;
;//
;//    //
;//    // Combination of WDF_DRIVER_INIT_FLAGS values
;//    //
;//    ULONG DriverInitFlags;
;//
;//    //
;//    // Pool tag to use for all allocations made by the framework on behalf of
;//    // the client driver.
;//    //
;//    ULONG DriverPoolTag;
;//
;//} WDF_DRIVER_CONFIG, *PWDF_DRIVER_CONFIG;
WDF_DRIVER_CONFIG struct
  _Size DWORD ?
  EvtDriverDeviceAdd PVOID ?
  EvtDriverUnload PVOID ?
  DriverInitFlags DWORD ?
  DriverPoolTag DWORD ?
WDF_DRIVER_CONFIG ends
PWDF_DRIVER_CONFIG typedef ptr WDF_DRIVER_CONFIG

;//VOID
;//FORCEINLINE
;//WDF_DRIVER_CONFIG_INIT(
;//    __out PWDF_DRIVER_CONFIG Config,
;//    __in_opt PFN_WDF_DRIVER_DEVICE_ADD EvtDriverDeviceAdd
;//    )
;//{
;//    RtlZeroMemory(Config, sizeof(WDF_DRIVER_CONFIG));
;//
;//    Config->Size = sizeof(WDF_DRIVER_CONFIG);
;//    Config->EvtDriverDeviceAdd = EvtDriverDeviceAdd;
;//}
WDF_DRIVER_CONFIG_INIT proc stdcall Config:DWORD, EvtDriverDeviceAdd:DWORD
  push Config
  invoke RtlZeroMemory, Config, sizeof WDF_DRIVER_CONFIG
  pop eax
  mov (WDF_DRIVER_CONFIG PTR [eax])._Size, sizeof WDF_DRIVER_CONFIG
  push EvtDriverDeviceAdd
  pop (WDF_DRIVER_CONFIG PTR [eax]).EvtDriverDeviceAdd
  ret
WDF_DRIVER_CONFIG_INIT endp

;//
;// WDF Function: WdfDriverCreate
;//
;//typedef
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//WDFAPI
;//NTSTATUS
;//(*PFN_WDFDRIVERCREATE)(
;//    __in
;//    PWDF_DRIVER_GLOBALS DriverGlobals,
;//    __in
;//    PDRIVER_OBJECT DriverObject,
;//    __in
;//    PCUNICODE_STRING RegistryPath,
;//    __in_opt
;//    PWDF_OBJECT_ATTRIBUTES DriverAttributes,
;//    __in
;//    PWDF_DRIVER_CONFIG DriverConfig,
;//    __out_opt
;//    WDFDRIVER* Driver
;//    );
;//
;//__checkReturn
;//__drv_maxIRQL(PASSIVE_LEVEL)
;//NTSTATUS
;//FORCEINLINE
;//WdfDriverCreate(
;//    __in
;//    PDRIVER_OBJECT DriverObject,
;//    __in
;//    PCUNICODE_STRING RegistryPath,
;//    __in_opt
;//    PWDF_OBJECT_ATTRIBUTES DriverAttributes,
;//    __in
;//    PWDF_DRIVER_CONFIG DriverConfig,
;//    __out_opt
;//    WDFDRIVER* Driver
;//    )
;//{
;//    return ((PFN_WDFDRIVERCREATE) WdfFunctions[WdfDriverCreateTableIndex])(WdfDriverGlobals, DriverObject, RegistryPath, DriverAttributes, DriverConfig, Driver);
;//}
FN_WDFDRIVERCREATE typedef proto WDFAPI :dword, :dword, :dword, :dword, :dword, :dword
PFN_WDFDRIVERCREATE typedef ptr FN_WDFDRIVERCREATE
.code
WdfDriverCreate proc DriverObject:PDRIVER_OBJECT, RegistryPath:PUNICODE_STRING, DriverAttributes:PWDF_OBJECT_ATTRIBUTES, DriverConfig:PWDF_DRIVER_CONFIG, Driver:PWDFDRIVER
  mov eax, WdfFunctions
  add eax, WdfDriverCreateTableIndex * PFN_WDFDRIVERCREATE
  invoke (FN_WDFDRIVERCREATE ptr [eax]), WdfDriverGlobals, DriverObject, RegistryPath, DriverAttributes, DriverConfig, Driver
  ret
WdfDriverCreate endp