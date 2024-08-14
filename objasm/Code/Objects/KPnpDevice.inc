;;
;; Author:   Steward Fu
;; Updated:  2024/08/12
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

MakeObjects KDeviceBase, KDevice

Object KPnpDevice, KPnpDeviceID, KDevice
    VirtualAbstract DefaultPnp, PKIrp

    VirtualMethod Pnp, PKIrp
    VirtualMethod SetLowerDevice, PKLowerDevice

    RedefineMethod Init, PWSTR, DWORD, PWSTR, DWORD, DWORD
    RedefineMethod DeviceIrpDispatch, PIRP

    DefineVariable m_pLowerDevice, PKLowerDevice, NULL
ObjectEnd

Method KPnpDevice.Init, uses esi, pDevName : PWSTR, dwDevType : DWORD, pLinkName : PWSTR, dwChar : DWORD, dwFlags : DWORD
    D $OfsCStr("KPnpDevice.Init()")

    SetObject esi
    push $MethodAddr(KPnpDevice.Pnp)
    pop [esi].IrpDispatchTable[IRP_MJ_PNP * (sizeof PVOID)]

    ACall Init, pDevName, dwDevType, pLinkName, dwChar, dwFlags
MethodEnd

Method KPnpDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    local I : PKIrp
    local status : NTSTATUS

    D $OfsCStr("KPnpDevice.DeviceIrpDispatch()")

    ACall DeviceIrpDispatch, pMyIrp

    New KIrp
    mov I, eax
    OCall I::KIrp.Init, pMyIrp
    OCall Pnp, I
    mov status, eax
    Destroy I

    mov eax, status
MethodEnd

Method KPnpDevice.SetLowerDevice, uses esi, pLowerDevice : PKLowerDevice
    D $OfsCStr("KPnpDevice.SetLowerDevice()")

    SetObject esi
    push pLowerDevice
    pop [esi].m_pLowerDevice
MethodEnd

Method KPnpDevice.Pnp, uses esi, I : PKIrp
    local Minor : DWORD
    local pMyIrp : PIRP

    D $OfsCStr("KPnpDevice.Pnp()")

    OCall I::KIrp.MinorFunction
    mov Minor, eax
    OCall I::KIrp.PIRP
    mov pMyIrp, eax

    D $OfsCStr("    Minor Function 0x%x"), Minor
    .if Minor == IRP_MN_REMOVE_DEVICE
        D $OfsCStr("    IRP_MN_REMOVE_DEVICE")

        SetObject esi
        OCall Detach, [esi].m_pLowerDevice
        OCall Delete
        invoke IoCompleteRequest, pMyIrp, IO_NO_INCREMENT
        mov eax, STATUS_SUCCESS
    .else
        OCall DefaultPnp, I
    .endif
MethodEnd
