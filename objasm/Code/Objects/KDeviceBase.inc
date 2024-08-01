;;
;; Author:   Steward Fu
;; Created:  2024/07/29
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ddk.inc
%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ntstatus.inc

    .const
Object KDeviceBase, KDeviceBaseID, Primer
    RedefineMethod Init

    DefineVariable m_pDeviceObject, POINTER, 0
ObjectEnd

    .code
Method KDeviceBase.Init, uses esi
    invoke DbgPrint, $OfsCStr("KDeviceBase.Init()")
MethodEnd
