;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ddk.inc
%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ntstatus.inc

    .const
Object KDeviceBase, KDeviceBaseID, Primer
    RedefineMethod Init

    DefineVariable m_pMyDevice, POINTER, 0
ObjectEnd

    .code
Method KDeviceBase.Init
    T $OfsCStr("KDeviceBase.Init()")
MethodEnd
