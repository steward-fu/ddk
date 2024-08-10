;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ddk.inc
%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ntstatus.inc

    .const
Object KLowerDevice, KLowerDeviceID, Primer
    VirtualMethod Initialize

    DefineVariable m_pNextDevice, PDEVICE_OBJECT, NULL
ObjectEnd

    .code
Method KLowerDevice.Initialize, uses esi
    T $OfsCStr("KLowerDevice.Initialize()")
MethodEnd
