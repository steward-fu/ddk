;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

    .const
Object KPnpLowerDevice, KPnpLowerDeviceID, KLowerDevice
    RedefineMethod Initialize, PDEVICE_OBJECT, PDEVICE_OBJECT
ObjectEnd

    .code
Method KPnpLowerDevice.Initialize, uses esi, pMyDevice : PDEVICE_OBJECT, pPhyDevice : PDEVICE_OBJECT
    D $OfsCStr("KPnpLowerDevice.Initialize()")

    SetObject esi
    invoke IoAttachDeviceToDeviceStack, pMyDevice, pPhyDevice
    mov [esi].m_pNextDevice, eax
MethodEnd
