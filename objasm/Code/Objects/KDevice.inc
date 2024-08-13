;;
;; Author:   Steward Fu
;; Updated:  2024/08/12
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

MakeObjects KLowerDevice

    .const
Object KDevice, KDeviceID, KDeviceBase
    RedefineMethod Init, PWSTR, DWORD, PWSTR, DWORD, DWORD

    VirtualMethod Delete
    VirtualMethod Detach, PKLowerDevice
    VirtualMethod DeviceIrpDispatch, PIRP

    DefineVariable IrpDispatchTable, POINTER, IRP_MJ_MAXIMUM_FUNCTION dup(0)
ObjectEnd

    .code
Method KDevice.Init, uses esi, pDevName : PWSTR, dwType : DWORD, pLinkName : PWSTR, dwChar : DWORD, dwDevFlags : DWORD
    local pMyDriver : POINTER
    local pMyDevice : POINTER
    local DevName : UNICODE_STRING

    D $OfsCStr("KDevice.Init()")
    D $OfsCStr("    DevName:  %ws"),  pDevName
    D $OfsCStr("    Type:     0x%x"), dwType
    D $OfsCStr("    LinkName: 0x%p"), pLinkName
    D $OfsCStr("    Charact:  0x%x"), dwChar
    D $OfsCStr("    Flags:    0x%x"), dwDevFlags

    ACall Init

    OCall $ObjTmpl(KDriver)::KDriver.DriverInstance
    SetObject esi, KDriver, eax
    push [esi].m_pMyDriver
    pop pMyDriver
 
    invoke RtlInitUnicodeString, addr DevName, pDevName
    invoke IoCreateDevice, pMyDriver, 0, addr DevName, dwType, dwChar, FALSE, addr pMyDevice
    D $OfsCStr("    New Created Device: 0x%x"), pMyDevice

    mov eax, pMyDevice
    .if eax == NULL
        mov eax, -1
        ret
    .endif

    mov esi, dwDevFlags
    or (DEVICE_OBJECT ptr [eax]).Flags, esi
    push pSelf
    pop (DEVICE_OBJECT ptr [eax]).DeviceExtension

    SetObject esi
    push pMyDevice
    pop [esi].m_pMyDevice
MethodEnd

Method KDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    D $OfsCStr("KDevice.DeviceIrpDispatch()")
MethodEnd

Method KDevice.Detach, uses esi, pLowerDevice : PKLowerDevice
    local pNextDevice : PDEVICE_OBJECT

    D $OfsCStr("KDevice.Detach()")

    OCall pLowerDevice::KLowerDevice.DeviceObject
    mov pNextDevice, eax

    D $OfsCStr("    Detach (dev:0x%x)"), pNextDevice
    invoke IoDetachDevice, pNextDevice
MethodEnd

Method KDevice.Delete, uses esi
    D $OfsCStr("KDevice.Delete()")

    SetObject esi
    D $OfsCStr("    Delete (dev:0x%x)"), [esi].m_pMyDevice
    invoke IoDeleteDevice, [esi].m_pMyDevice
MethodEnd
