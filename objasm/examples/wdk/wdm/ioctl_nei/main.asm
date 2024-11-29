;;
;; Purpose: IOCTL IRP with METHOD_NEITHER
;; Website: https://steward-fu.github.io/website/index.htm
;;

%include @Environ(OBJASM_PATH)/Code/Macros/Model.inc

SysSetup OOP,WDK_WDM, ANSI_STRING

MakeObjects Primer, KDriver, KPnpDevice, KPnpLowerDevice

IOCTL_GET equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_NEITHER, FILE_ANY_ACCESS)
IOCTL_SET equ CTL_CODE(FILE_DEVICE_UNKNOWN, 801h, METHOD_NEITHER, FILE_ANY_ACCESS)

Object MyDriver, , KDriver
    RedefineMethod Unload
    RedefineMethod AddDevice, PDEVICE_OBJECT
    RedefineMethod DriverEntry, PUNICODE_STRING
    RedefineMethod DriverIrpDispatch, PIRP
ObjectEnd

Object MyDevice, , KPnpDevice
    RedefineMethod Close, PKIrp
    RedefineMethod Create, PKIrp
    RedefineMethod DeviceControl, PKIrp

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
    ACall Init, $OfsCStrW("\Device\MyDriver"), FILE_DEVICE_UNKNOWN, $OfsCStrW("\DosDevices\MyDriver"), 0, DO_BUFFERED_IO

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

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method MyDevice.DeviceControl, uses esi, I : PKIrp
    local code : DWORD
    local dwSize : DWORD
    local pBuffer : POINTER

    and dwSize, 0

    OCall I::KIrp.IoctlCode
    mov code, eax
    .if code == IOCTL_SET
        OCall I::KIrp.IoctlType3InputBuffer
        mov pBuffer, eax

        SetObject esi
        OCall I::KIrp.IoctlInputBufferSize
        push dword ptr [eax]
        pop dwSize
        invoke memcpy, addr [esi].m_Buffer, pBuffer, dwSize

        T $OfsCStr("Buffer: %s, Length: %d"),addr [esi].m_Buffer, dwSize
    .elseif code == IOCTL_GET
        OCall I::KIrp.UserBuffer
        mov pBuffer, eax

        SetObject esi
        invoke strlen, pBuffer
        mov dwSize, eax
        invoke memcpy, pBuffer, addr [esi].m_Buffer, dwSize
    .endif

    OCall I::KIrp.Information
    push dwSize
    pop dword ptr [eax]

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method MyDevice.Close, uses esi, I : PKIrp
    T $OfsCStr("IRP_MJ_CLOSE")

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd
end
