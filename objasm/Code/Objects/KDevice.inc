;;
;; Author:   Steward Fu
;; Updated:  2024/08/13
;; Purpose:  Implement the framework of NuMega's DriverWorks
;;

MakeObjects KLowerDevice

Object KDevice, KDeviceID, KDeviceBase
    RedefineMethod Done
    RedefineMethod Init, PWSTR, DWORD, PWSTR, DWORD, DWORD

    VirtualMethod Delete
    VirtualMethod Detach, PKLowerDevice
    VirtualMethod DeviceIrpDispatch, PIRP

    DefineVariable IrpDispatchTable, POINTER, IRP_MJ_MAXIMUM_FUNCTION dup(0)
ObjectEnd

Method KDevice.Init, uses esi, pDevName : PWSTR, dwType : DWORD, pLinkName : PWSTR, dwChar : DWORD, dwDevFlags : DWORD
    local status : NTSTATUS
    local DevName : UNICODE_STRING
    local pMyDevice : PDEVICE_OBJECT
    local pMyDriver : PDRIVER_OBJECT

    D $OfsCStr("KDevice.Init()")
    D $OfsCStr("    DevName:  %ws"),  pDevName
    D $OfsCStr("    Type:     0x%x"), dwType
    D $OfsCStr("    LinkName: 0x%p"), pLinkName
    D $OfsCStr("    Charact:  0x%x"), dwChar
    D $OfsCStr("    Flags:    0x%x"), dwDevFlags

    ACall Init

    OCall $ObjTmpl(KDriver)::KDriver.DriverInstance
    SetObject esi, KDriver, eax
    OCall eax::KDriver.DriverObject
    mov pMyDriver, eax
 
    invoke RtlInitUnicodeString, addr DevName, pDevName
    invoke IoCreateDevice, pMyDriver, 0, addr DevName, dwType, dwChar, FALSE, addr pMyDevice
    mov status, eax

    mov eax, pMyDevice
    mov esi, dwDevFlags
    or (DEVICE_OBJECT ptr [eax]).Flags, esi
    push pSelf
    pop (DEVICE_OBJECT ptr [eax]).DeviceExtension
    D $OfsCStr("    New Created Device: (dev:0x%x, ret:0x%x)"), pMyDevice, status

    SetObject esi
    mov eax, pMyDevice
    mov [esi].m_pMyDevice, eax
MethodEnd

Method KDevice.Done, uses esi
    D $OfsCStr("KDevice.Done(this:0x%x)"), pSelf
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
