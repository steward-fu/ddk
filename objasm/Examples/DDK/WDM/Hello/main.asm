;;
;; Author: Steward Fu
;; Update: 2024/08/12
;;
;; The example of hello based on WDM model
;;

%include @Environ(OBJASM_PATH)/Code/Macros/Model.inc

SysSetup OOP, DDK32, ANSI_STRING

MakeObjects Primer, KDriver, KPnpDevice, KPnpLowerDevice

    .const
Object MyDriver, , KDriver
    RedefineMethod Init
    RedefineMethod AddDevice, PDEVICE_OBJECT
    RedefineMethod DriverEntry, PUNICODE_STRING
    RedefineMethod DriverIrpDispatch, PIRP

    DefineVariable m_pMyDeviceInst, PMyDevice, NULL
ObjectEnd

Object MyDevice, , KPnpDevice
    RedefineMethod Init, PDEVICE_OBJECT
    RedefineMethod DefaultPnp, PKIrp
    RedefineMethod DeviceIrpDispatch, PIRP

    Embed m_pMyLowerDevice, KPnpLowerDevice
ObjectEnd

    .code
DECLARE_DRIVER_CLASS MyDriver, $OfsCStr("MyDriver")

Method MyDriver.Init, uses esi
    D $OfsCStr("MyDriver.Init(this:0x%x)"), pSelf

    ACall Init
MethodEnd

Method MyDriver.DriverEntry, uses esi, pMyRegistry : PUNICODE_STRING
    D $OfsCStr("MyDriver.DriverEntry()")

    mov eax, STATUS_SUCCESS
MethodEnd

Method MyDriver.AddDevice, uses esi, pPhyDevice : PDEVICE_OBJECT
    D $OfsCStr("MyDriver.AddDevice()")

    SetObject esi
    New MyDevice
    mov [esi].m_pMyDeviceInst, eax

    OCall eax::MyDevice.Init, pPhyDevice
    mov eax, STATUS_SUCCESS
MethodEnd

Method MyDriver.DriverIrpDispatch, uses esi, pMyIrp : PIRP
    D $OfsCStr("MyDriver.DriverIrpDispatch()")

    SetObject esi
    mov eax, [esi].m_pMyDriver
    mov eax, (DRIVER_OBJECT ptr [eax]).DeviceObject
    mov eax, (DEVICE_OBJECT ptr [eax]).DeviceExtension
    OCall eax::MyDevice.DeviceIrpDispatch, pMyIrp
MethodEnd

Method MyDevice.Init, uses esi, pPhyDevice : PDEVICE_OBJECT
    D $OfsCStr("MyDevice.Init(this:0x%x)"), pSelf

    ACall Init, $OfsCStrW("\Device\MyDriver"), FILE_DEVICE_UNKNOWN, NULL, 0, DO_BUFFERED_IO

    SetObject esi
    OCall [esi].m_pMyLowerDevice::KPnpLowerDevice.Initialize, [esi].m_pMyDevice, pPhyDevice
    OCall SetLowerDevice, addr [esi].m_pMyLowerDevice
MethodEnd

Method MyDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    D $OfsCStr("MyDevice.DeviceIrpDispatch()")

    ACall DeviceIrpDispatch, pMyIrp
MethodEnd

Method MyDevice.DefaultPnp, uses esi, I : PKIrp
    D $OfsCStr("MyDevice.DefaultPnp()")

    SetObject esi
    OCall I::KIrp.ForceReuseOfCurrentStackLocationInCalldown
    OCall [esi].m_pMyLowerDevice::KLowerDevice.PnpCall, I
MethodEnd
end
