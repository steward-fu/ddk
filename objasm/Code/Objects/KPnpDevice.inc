;;
;; Purpose: Implement NuMega DriverWorks
;; Website: https://steward-fu.github.io/website/index.htm
;;

MakeObjects KDeviceBase, KDevice

Object KPnpDevice, KPnpDeviceID, KDevice
    VirtualAbstract DefaultPnp, PKIrp

    VirtualMethod Pnp, PKIrp
    VirtualMethod Create, PKIrp
    VirtualMethod Read, PKIrp
    VirtualMethod Write, PKIrp
    VirtualMethod Close, PKIrp
    VirtualMethod DeviceControl, PKIrp
    VirtualMethod SetLowerDevice, PKLowerDevice

    RedefineMethod Init, PWSTR, DWORD, PWSTR, DWORD, DWORD
    RedefineMethod DeviceIrpDispatch, PIRP

    DefineVariable m_pLowerDevice, PKLowerDevice, NULL
ObjectEnd

Method KPnpDevice.Init, uses esi, pDevName : PWSTR, dwDevType : DWORD, pSymName : PWSTR, dwChar : DWORD, dwFlags : DWORD
    D $OfsCStr("KPnpDevice.Init()")

    ACall Init, pDevName, dwDevType, pSymName, dwChar, dwFlags
MethodEnd

Method KPnpDevice.SetLowerDevice, uses esi, pLowerDevice : PKLowerDevice
    D $OfsCStr("KPnpDevice.SetLowerDevice()")

    SetObject esi
    push pLowerDevice
    pop [esi].m_pLowerDevice
MethodEnd

Method KPnpDevice.Create, uses esi, I : PKIrp
    D $OfsCStr("KPnpDevice.Create()")

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method KPnpDevice.Read, uses esi, I : PKIrp
    D $OfsCStr("KPnpDevice.Read()")

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method KPnpDevice.Write, uses esi, I : PKIrp
    D $OfsCStr("KPnpDevice.Write()")

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method KPnpDevice.Close, uses esi, I : PKIrp
    D $OfsCStr("KPnpDevice.Close()")

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method KPnpDevice.DeviceControl, uses esi, I : PKIrp
    D $OfsCStr("KPnpDevice.DeviceControl()")

    OCall I::KIrp.PnpComplete, STATUS_SUCCESS, IO_NO_INCREMENT
MethodEnd

Method KPnpDevice.Pnp, uses esi, I : PKIrp
    local Minor : DWORD
    local pFunc : POINTER
    local pMyIrp : PIRP

    D $OfsCStr("KPnpDevice.Pnp()")

    OCall I::KIrp.MinorFunction
    mov Minor, eax

    OCall I::KIrp.PIRP
    mov pMyIrp, eax

    .if Minor == IRP_MN_REMOVE_DEVICE
        SetObject esi
        OCall Detach, [esi].m_pLowerDevice
        OCall Delete
        invoke IoCompleteRequest, pMyIrp, IO_NO_INCREMENT
        mov eax, STATUS_SUCCESS
    .else
        OCall DefaultPnp, I
    .endif
MethodEnd

Method KPnpDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    local I : PKIrp
    local Major : DWORD
    local status : NTSTATUS

    D $OfsCStr("KPnpDevice.DeviceIrpDispatch()")

    ACall DeviceIrpDispatch, pMyIrp

    New KIrp
    mov I, eax
    OCall I::KIrp.Init, pMyIrp
    OCall I::KIrp.MajorFunction
    mov Major, eax

    .if Major == IRP_MJ_PNP
        OCall Pnp, I
        mov status, eax
    .elseif Major == IRP_MJ_CREATE
        OCall Create, I
        mov status, eax
    .elseif Major == IRP_MJ_READ
        OCall Read, I
        mov status, eax
    .elseif Major == IRP_MJ_WRITE
        OCall Write, I
        mov status, eax
    .elseif Major == IRP_MJ_CLOSE
        OCall Close, I
        mov status, eax
    .elseif Major == IRP_MJ_DEVICE_CONTROL
        OCall DeviceControl, I
        mov status, eax
    .else
        mov eax, STATUS_INVALID_DEVICE_REQUEST
    .endif

    Destroy I
    mov eax, status
MethodEnd
