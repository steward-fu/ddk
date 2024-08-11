%include @Environ(OBJASM_PATH)/Code/Macros/Model.inc
SysSetup OOP, DDK32, ANSI_STRING

MakeObjects Primer
MakeObjects KDriver
MakeObjects KDeviceBase, KDevice, KPnpDevice
MakeObjects KLowerDevice, KPnpLowerDevice

    .const
Object MyDriver, , KDriver
    RedefineMethod Init
    RedefineMethod AddDevice, PDEVICE_OBJECT
    RedefineMethod DriverEntry, PUNICODE_STRING
    RedefineMethod DriverIrpDispatch, PIRP
ObjectEnd

Object MyDevice, , KPnpDevice
    RedefineMethod Init, PDEVICE_OBJECT
    RedefineMethod DefaultPnp, POINTER
    RedefineMethod DeviceIrpDispatch, PIRP

    Embed m_pMyLowerDevice, KPnpLowerDevice
ObjectEnd

    .code
DECLARE_DRIVER_CLASS MyDriver, $OfsCStr("MyDriver")

Method MyDriver.Init, uses esi
    D $OfsCStr("MyDriver.Init()")

    ACall pSelf::MyDriver.Init
MethodEnd

Method MyDriver.DriverEntry, uses esi, pMyRegistry : PUNICODE_STRING
    D $OfsCStr("MyDriver.DriverEntry()")

    mov eax, STATUS_SUCCESS
MethodEnd

Method MyDriver.AddDevice, uses esi, pPhyDevice : PDEVICE_OBJECT
    D $OfsCStr("MyDriver.AddDevice()")
    T $OfsCStr("Hello, world\:")

    New MyDevice
    OCall eax::MyDevice.Init, pPhyDevice
MethodEnd

Method MyDriver.DriverIrpDispatch, uses esi, pMyIrp : PIRP
    SetObject esi
    mov eax, [esi].m_pMyDriver
    mov eax, (DRIVER_OBJECT ptr [eax]).DeviceObject
    mov eax, (DEVICE_OBJECT ptr [eax]).DeviceExtension
    OCall eax::MyDevice.DeviceIrpDispatch, pMyIrp
MethodEnd

Method MyDevice.Init, uses esi, pPhyDevice : PDEVICE_OBJECT
    D $OfsCStr("MyDevice.Init()")

    ACall pSelf::MyDevice.Init, $OfsCStrW("\Device\MyDriver"), FILE_DEVICE_UNKNOWN, NULL, 0, DO_BUFFERED_IO

    SetObject esi
    OCall [esi].m_pMyLowerDevice::KPnpLowerDevice.Initialize, [esi].m_pMyDevice, pPhyDevice
MethodEnd

Method MyDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    ACall DeviceIrpDispatch, pMyIrp
MethodEnd

Method MyDevice.DefaultPnp, uses esi, I : POINTER
    D $OfsCStr("MyDevice.DefaultPnp()")

    OCall I::KIrp.ForceReuseOfCurrentStackLocationInCalldown

    SetObject esi
    OCall [esi].m_pMyLowerDevice::KLowerDevice.PnpCall, I
MethodEnd
end
