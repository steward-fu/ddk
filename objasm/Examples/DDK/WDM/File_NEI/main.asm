;;
;; Author: Steward Fu
;; Update: 2024/08/15
;;
;; Choose DO_NEITHER_IO for File IRP
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
    RedefineMethod Create, PKIrp
    RedefineMethod Read, PKIrp
    RedefineMethod Write, PKIrp
    RedefineMethod Close, PKIrp

    RedefineMethod Init, PDEVICE_OBJECT
    RedefineMethod DefaultPnp, PKIrp
    RedefineMethod DeviceIrpDispatch, PIRP

    Embed m_pMyLowerDevice, KPnpLowerDevice

    DefineVariable m_Buffer, BYTE, 255 dup(0)
ObjectEnd

DECLARE_DRIVER_CLASS MyDriver, $OfsCStr("MyDriver")

Method MyDriver.DriverEntry, uses esi, pMyRegistry : PUNICODE_STRING
    mov eax, STATUS_SUCCESS
MethodEnd

Method MyDriver.AddDevice, uses esi, pPhyDevice : PDEVICE_OBJECT
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
    ACall Init, $OfsCStrW("\Device\MyDriver"), FILE_DEVICE_UNKNOWN, $OfsCStrW("\DosDevices\MyDriver"), 0, 0

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

Method MyDevice.Create, uses esi, I : PKIrp
    T $OfsCStr("IRP_MJ_CREATE")

    OCall I::KIrp.Information
    mov dword ptr [eax], 0
    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method MyDevice.Read, uses esi, I : PKIrp
    local dwSize : DWORD
    local pBuffer : POINTER

    T $OfsCStr("IRP_MJ_READ")

    SetObject esi
    OCall I::KIrp.UserBuffer
    mov pBuffer, eax

    invoke strcpy, pBuffer, addr [esi].m_Buffer
    invoke strlen, pBuffer
    mov dwSize, eax

    OCall I::KIrp.Information
    push dwSize
    pop dword ptr [eax]

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method MyDevice.Write, uses esi, I : PKIrp
    local dwSize : DWORD
    local pBuffer : POINTER

    T $OfsCStr("IRP_MJ_WRITE")

    SetObject esi
    OCall I::KIrp.UserBuffer
    mov pBuffer, eax

    OCall I::KIrp.WriteSize
    push dword ptr [eax]
    pop dwSize

    invoke strcpy, addr [esi].m_Buffer, pBuffer

    OCall I::KIrp.Information
    push dwSize
    pop dword ptr [eax]

    T $OfsCStr("Address: 0x%x, Length: %d"), pBuffer, dwSize
    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method MyDevice.Close, uses esi, I : PKIrp
    T $OfsCStr("IRP_MJ_CLOSE")

    OCall I::KIrp.Information
    mov dword ptr [eax], 0
    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd
end
