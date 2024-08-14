;;
;; Author:   Steward Fu
;; Updated:  2024/08/13
;; Purpose:  Implement the framework of NuMega's DriverWorks
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
