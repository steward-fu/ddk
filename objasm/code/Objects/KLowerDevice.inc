;;
;; Purpose: Implement NuMega DriverWorks
;; Website: https://steward-fu.github.io/website/index.htm
;;

MakeObjects KIrp

Object KLowerDevice, KLowerDeviceID, Primer
    VirtualMethod Initialize
    VirtualMethod DeviceObject
    VirtualMethod PnpCall, PKIrp

    DefineVariable m_pNextDevice, PDEVICE_OBJECT, NULL
ObjectEnd

Method KLowerDevice.Initialize, uses esi
    D $OfsCStr("KLowerDevice.Initialize(this:0x%x)"), pSelf
MethodEnd

Method KLowerDevice.DeviceObject, uses esi
    D $OfsCStr("KLowerDevice.DeviceObject()")

    SetObject esi
    push [esi].m_pNextDevice
    pop eax
MethodEnd

Method KLowerDevice.PnpCall, uses esi, I : PKIrp
    local pMyIrp : PIRP

    D $OfsCStr("KLowerDevice.PnpCall()")

    OCall I::KIrp.PIRP
    mov pMyIrp, eax

    SetObject esi
    invoke IoCallDriver, [esi].m_pNextDevice, pMyIrp
MethodEnd
