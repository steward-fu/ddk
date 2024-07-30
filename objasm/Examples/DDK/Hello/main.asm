%include @Environ(OBJASM_PATH)/Code/Macros/Model.inc
SysSetup OOP, WIN32, ANSI_STRING

MakeObjects Primer, KDriver, KDevice, KPnpDevice

    .const
Object MyDriver, , KDriver
    RedefineMethod DriverEntry, PUNICODE_STRING
    RedefineMethod AddDevice, PDEVICE_OBJECT

    DefineVariable m_pDeviceObject, POINTER, 0
    DefineVariable m_pNextDevice, PDEVICE_OBJECT, 0
ObjectEnd

DECLARE_DRIVER_CLASS MyDriver, $OfsCStr("MyDriver")

Method MyDriver.AddDevice, uses esi, pPhyDevice : PDEVICE_OBJECT
    OCall $ObjTmpl(KPnpDevice)::KPnpDevice.Init, $OfsCStrW("\\Device\\MyDriver"), FILE_DEVICE_UNKNOWN

    SetObject esi
    invoke DbgPrint, $OfsCStr("MyDriver.AddDevice")
    mov eax, STATUS_SUCCESS
MethodEnd

Method MyDriver.DriverEntry, uses esi, pMyRegistry : PUNICODE_STRING
    invoke DbgPrint, $OfsCStr("MyDriver.DriverEntry")
    mov eax, STATUS_SUCCESS
MethodEnd
    
    .const
DEV_NAME word "\","D","e","v","i","c","e","\","M","y","D","r","i","v","e","r",0
   
end
