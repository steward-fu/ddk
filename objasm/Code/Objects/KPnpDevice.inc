;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

MakeObjects KIrp

    .const
Object KPnpDevice, KPnpDeviceID, KDevice
    VirtualAbstract DefaultPnp, POINTER

    RedefineMethod Init, POINTER, DWORD, POINTER, DWORD, DWORD
    RedefineMethod DeviceIrpDispatch, PIRP
ObjectEnd

    .code
Method KPnpDevice.Init, uses esi, pszDevName:POINTER, dwDevType:DWORD, pszLinkName : POINTER, dwChar : DWORD, dwFlags : DWORD
    D $OfsCStr("KPnpDevice.Init()")

    ACall Init, pszDevName, dwDevType, pszLinkName, dwChar, dwFlags
MethodEnd

Method KPnpDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    local I : POINTER

    D $OfsCStr("KPnpDevice.DeviceIrpDispatch()")

    ACall DeviceIrpDispatch, pMyIrp

    New KIrp
    mov I, eax
    OCall I::KIrp.Init, pMyIrp
    OCall DefaultPnp, I
    ;;;Destroy I
MethodEnd
