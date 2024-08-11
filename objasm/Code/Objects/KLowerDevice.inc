;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

    .const
Object KLowerDevice, KLowerDeviceID, Primer
    VirtualMethod Initialize
    VirtualMethod PnpCall, POINTER

    DefineVariable m_pNextDevice, PDEVICE_OBJECT, NULL
ObjectEnd

    .code
Method KLowerDevice.Initialize, uses esi
    D $OfsCStr("KLowerDevice.Initialize()")
MethodEnd

Method KLowerDevice.PnpCall, uses esi, I : POINTER
    local pMyIrp : PIRP

    D $OfsCStr("KLowerDevice.PnpCall()")

    OCall I::KIrp.PIRP
    mov pMyIrp, eax

    SetObject esi
    invoke IoCallDriver, [esi].m_pNextDevice, pMyIrp
MethodEnd
