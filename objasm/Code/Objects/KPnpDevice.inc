;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ddk.inc
%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ntstatus.inc

    .const
Object KPnpDevice, KPnpDeviceID, KDevice
    VirtualAbstract DefaultPnp, PIRP

    RedefineMethod Init, POINTER, DWORD, POINTER, DWORD, DWORD
    RedefineMethod DeviceIrpDispatch, PIRP
ObjectEnd

    .code
Method KPnpDevice.Init, uses esi, pszDevName:POINTER, dwDevType:DWORD, pszLinkName : POINTER, dwChar : DWORD, dwFlags : DWORD
    T $OfsCStr("KPnpDevice.Init()")

    ACall pSelf::KPnpDevice.Init, pszDevName, dwDevType, pszLinkName, dwChar, dwFlags
MethodEnd

Method KPnpDevice.DeviceIrpDispatch, uses esi, pMyIrp : PIRP
    T $OfsCStr("KPnpDevice.DeviceIrpDispatch()")

    ACall pSelf::KPnpDevice.DeviceIrpDispatch, pMyIrp
MethodEnd
