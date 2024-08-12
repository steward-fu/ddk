;;
;; Author:   Steward Fu
;; Updated:  2024/08/12
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

MakeObjects KIrp

    .const
Object KLowerDevice, KLowerDeviceID, Primer
    VirtualMethod Initialize
    VirtualMethod DeviceObject
    VirtualMethod PnpCall, POINTER

    DefineVariable m_pNextDevice, PDEVICE_OBJECT, NULL
ObjectEnd

    .code
Method KLowerDevice.Initialize, uses esi
    D $OfsCStr("KLowerDevice.Initialize()")
MethodEnd

Method KLowerDevice.DeviceObject, uses esi
    D $OfsCStr("KLowerDevice.DeviceObject()")

    SetObject esi
    push [esi].m_pNextDevice
    pop eax
MethodEnd

Method KLowerDevice.PnpCall, uses esi, I : POINTER
    local pMyIrp : PIRP

    D $OfsCStr("KLowerDevice.PnpCall()")

    OCall I::KIrp.PIRP
    mov pMyIrp, eax

    SetObject esi
    invoke IoCallDriver, [esi].m_pNextDevice, pMyIrp
MethodEnd
