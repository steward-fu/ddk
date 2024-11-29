;;
;; Purpose: Thread
;; Website: https://steward-fu.github.io/website/index.htm
;;

%include @Environ(OBJASM_PATH)/Code/Macros/Model.inc

SysSetup OOP, WDK_WDM, ANSI_STRING

MakeObjects Primer, KDriver, KPnpDevice, KPnpLowerDevice, KSystemThread

IOCTL_START equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_BUFFERED, FILE_ANY_ACCESS)
IOCTL_STOP  equ CTL_CODE(FILE_DEVICE_UNKNOWN, 801h, METHOD_BUFFERED, FILE_ANY_ACCESS)

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

    DefineVariable m_Exit, BOOLEAN, FALSE

    Embed m_Thread, KSystemThread
    Embed m_pMyLowerDevice, KPnpLowerDevice
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

    SetObject esi
    OCall I::KIrp.IoctlCode
    mov code, eax
    .if code == IOCTL_START
        T $OfsCStr("IOCTL_START")
        mov [esi].m_Exit, FALSE
        OCall [esi].m_Thread::KSystemThread.Start, offset MyThread, pSelf, THREAD_ALL_ACCESS
    .elseif code == IOCTL_STOP
        T $OfsCStr("IOCTL_STOP")
        mov [esi].m_Exit, TRUE
    .endif

    OCall I::KIrp.Information
    mov dword ptr [eax], 0

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method MyDevice.Close, uses esi, I : PKIrp
    T $OfsCStr("IRP_MJ_CLOSE")

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

MyThread proc pSelf : PMyDevice
    local pStr : DWORD
    local stTime : LARGE_INTEGER

    or stTime.HighPart, -1
    mov stTime.LowPart, -10000000
  
    invoke IoGetCurrentProcess
    add eax, 174h
    mov pStr, eax
    T $OfsCStr("Current process: %s"), pStr
 
    SetObject esi, MyDevice, pSelf
    .while [esi].m_Exit == FALSE
        invoke KeDelayExecutionThread, KernelMode, FALSE, addr stTime
        T $OfsCStr("Sleep 1s")
    .endw

    T $OfsCStr("Exit MyThread")
    OCall [esi].m_Thread::KSystemThread.Terminate, STATUS_SUCCESS
    ret
MyThread endp
end
