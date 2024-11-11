;;
;; Purpose: Implement NuMega DriverWorks
;; Website: https://steward-fu.github.io/website/index.htm
;;

Object KPnpLowerDevice, KPnpLowerDeviceID, KLowerDevice
    RedefineMethod Initialize, PDEVICE_OBJECT, PDEVICE_OBJECT
ObjectEnd

Method KPnpLowerDevice.Initialize, uses esi, pMyDevice : PDEVICE_OBJECT, pPhyDevice : PDEVICE_OBJECT
    D $OfsCStr("KPnpLowerDevice.Initialize()")

    ACall Initialize

    SetObject esi
    invoke IoAttachDeviceToDeviceStack, pMyDevice, pPhyDevice
    mov [esi].m_pNextDevice, eax
    D $OfsCStr("    Next Device: 0x%x"), [esi].m_pNextDevice
MethodEnd
