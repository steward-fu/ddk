;;
;; Author:   Steward Fu
;; Created:  2024/07/29
;; Updated:  2024/07/29
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ddk.inc
%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ntstatus.inc

    .const
Object KPnpDevice, KPnpDeviceID, KDevice
    RedefineMethod Init, POINTER, DWORD
ObjectEnd

    .code
Method KPnpDevice.Init, uses esi, szDevName:POINTER, dwType:DWORD
    SetObject esi

    invoke DbgPrint, $OfsCStr("KPnpDevice.Init")
    ACall esi.Init
MethodEnd
