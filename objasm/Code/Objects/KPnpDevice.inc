;;
;; Author:   Steward Fu
;; Created:  2024/07/29
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ddk.inc
%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ntstatus.inc

    .const
Object KPnpDevice, KPnpDeviceID, KDevice
    RedefineMethod Init, POINTER, DWORD
ObjectEnd

    .code
Method KPnpDevice.Init, uses esi, pszDevName:POINTER, dwDevType:DWORD
    invoke DbgPrint, $OfsCStr("KPnpDevice.Init()")

    SetObject esi
    ACall esi.Init, pszDevName, dwDevType, NULL, 0, DO_BUFFERED_IO
MethodEnd
