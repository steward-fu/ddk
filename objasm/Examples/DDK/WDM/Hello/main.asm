;;
;; Purpose: Hello, world!
;; Website: https://steward-fu.github.io/website/index.htm
;;

%include @Environ(OBJASM_PATH)/Code/Macros/Model.inc

SysSetup OOP, DDK32, ANSI_STRING

MakeObjects Primer, KDriver, KPnpDevice, KPnpLowerDevice

Object MyDriver, , KDriver
    RedefineMethod Unload
    RedefineMethod AddDevice, PDEVICE_OBJECT
    RedefineMethod DriverEntry, PUNICODE_STRING
    RedefineMethod DriverIrpDispatch, PIRP
ObjectEnd

Object MyDevice, , KPnpDevice
    RedefineMethod Init, PDEVICE_OBJECT
    RedefineMethod DefaultPnp, PKIrp
    RedefineMethod DeviceIrpDispatch, PIRP

    Embed m_pMyLowerDevice, KPnpLowerDevice
ObjectEnd

DECLARE_DRIVER_CLASS MyDriver, $OfsCStr("MyDriver")

Method MyDriver.DriverEntry, uses esi, pMyRegistry : PUNICODE_STRING
    mov eax, STATUS_SUCCESS
MethodEnd

Method MyDriver.AddDevice, uses esi, pPhyDevice : PDEVICE_OBJECT
    T $OfsCStr("Hello, world%c"), 21h

    New MyDevice
    push eax
    OCall eax::MyDevice.Init, pPhyDevice
    pop eax
MethodEnd

Method MyDriver.DriverIrpDispatch, uses esi, pMyIrp : PIRP
    SetObject esi
    OCall DriverObject
    mov eax, (DRIVER_OBJECT ptr [eax]).DeviceObject
    mov eax, (DEVICE_OBJECT ptr [eax]).DeviceExtension
    OCall eax::MyDevice.DeviceIrpDispatch, pMyIrp
MethodEnd

Method MyDriver.Unload, uses esi
    ACall Unload
MethodEnd

Method MyDevice.Init, uses esi, pPhyDevice : PDEVICE_OBJECT
    ACall Init, $OfsCStrW("\Device\MyDriver"), FILE_DEVICE_UNKNOWN, NULL, 0, DO_BUFFERED_IO

    SetObject esi
    OCall [esi].m_pMyLowerDevice::KPnpLowerDevice.Initialize, [esi].m_pMyDevice, pPhyDevice
    OCall SetLowerDevice, addr [esi].m_pMyLowerDevice
MethodEnd

Method MyDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    ACall DeviceIrpDispatch, pMyIrp
MethodEnd

Method MyDevice.DefaultPnp, uses esi, I : PKIrp
    SetObject esi
    OCall I::KIrp.ForceReuseOfCurrentStackLocationInCalldown
    OCall [esi].m_pMyLowerDevice::KLowerDevice.PnpCall, I
MethodEnd
end
