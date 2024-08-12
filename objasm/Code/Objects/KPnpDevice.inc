;;
;; Author:   Steward Fu
;; Updated:  2024/08/12
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

MakeObjects KDeviceBase, KDevice

    .const
Object KPnpDevice, KPnpDeviceID, KDevice
    VirtualAbstract DefaultPnp, POINTER

    VirtualMethod Pnp, POINTER
    VirtualMethod SetLowerDevice, PKLowerDevice

    RedefineMethod Init, PWSTR, DWORD, PWSTR, DWORD, DWORD
    RedefineMethod DeviceIrpDispatch, PIRP

    DefineVariable m_pLowerDevice, PKLowerDevice, NULL
ObjectEnd

    .code
Method KPnpDevice.Init, uses esi, pDevName : PWSTR, dwDevType : DWORD, pLinkName : PWSTR, dwChar : DWORD, dwFlags : DWORD
    D $OfsCStr("KPnpDevice.Init()")

    SetObject esi
    push $MethodAddr(KPnpDevice.Pnp)
    pop [esi].IrpDispatchTable[IRP_MJ_PNP * (sizeof PVOID)]

    ACall Init, pDevName, dwDevType, pLinkName, dwChar, dwFlags
MethodEnd

Method KPnpDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    local I : PKIrp
    local Minor : DWORD

    D $OfsCStr("KPnpDevice.DeviceIrpDispatch()")

    ACall DeviceIrpDispatch, pMyIrp

    New KIrp
    mov I, eax
    OCall I::KIrp.Init, pMyIrp
    OCall I::KIrp.MinorFunction
    mov Minor, eax

    OCall DefaultPnp, I
    Destroy I::KIrp.Delete

    .if Minor == IRP_MN_REMOVE_DEVICE
        ;SetObject esi
        ;OCall Detach, [esi].m_pLowerDevice
        ;ACall Delete
    .endif
MethodEnd

Method KPnpDevice.SetLowerDevice, uses esi, pLowerDevice : PKLowerDevice
    D $OfsCStr("KPnpDevice.SetLowerDevice()")

    SetObject esi
    push pLowerDevice
    pop [esi].m_pLowerDevice
MethodEnd

Method KPnpDevice.Pnp, uses esi, I : PKIrp
    D $OfsCStr("KPnpDevice.Pnp()")
MethodEnd
