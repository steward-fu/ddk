;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

    .const
Object KDevice, KDeviceID, KDeviceBase
    RedefineMethod Init, POINTER, DWORD, POINTER, DWORD, DWORD

    VirtualMethod DeviceIrpDispatch, PIRP

    DefineVariable IrpDispatchTable, POINTER, IRP_MJ_MAXIMUM_FUNCTION dup(0)
ObjectEnd

    .code
Method KDevice.Init, uses esi, pDevName : POINTER, dwType : DWORD, pLinkName : POINTER, dwChar : DWORD, dwDevFlags : DWORD
    local pMyDriver : POINTER
    local pMyDevice : POINTER
    local DevName : UNICODE_STRING

    D $OfsCStr("KDevice.Init()")
    D $OfsCStr("    DeviceName: %ws"),  pDevName
    D $OfsCStr("    Type:       0x%x"), dwType
    D $OfsCStr("    LinkName:   %p"),   pLinkName
    D $OfsCStr("    Charact:    0x%x"), dwChar
    D $OfsCStr("    Flags:      0x%x"), dwDevFlags

    ACall Init

    OCall $ObjTmpl(KDriver)::KDriver.DriverInstance
    SetObject esi, KDriver, eax
    push [esi].m_pMyDriver
    pop pMyDriver
 
    invoke RtlInitUnicodeString, addr DevName, pDevName
    invoke IoCreateDevice, pMyDriver, 0, addr DevName, dwType, 0, FALSE, addr pMyDevice

    mov eax, pMyDevice
    push dwDevFlags
    pop (DEVICE_OBJECT ptr [eax]).Flags
    push pSelf
    pop (DEVICE_OBJECT ptr [eax]).DeviceExtension

    SetObject esi
    push pMyDevice
    pop [esi].m_pMyDevice
MethodEnd

Method KDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    D $OfsCStr("KDevice.DeviceIrpDispatch()")
MethodEnd
