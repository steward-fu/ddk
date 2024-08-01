;;
;; Author:   Steward Fu
;; Created:  2024/07/29
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ddk.inc
%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ntstatus.inc

    .const
Object KDevice, KDeviceID, KDeviceBase
    RedefineMethod Init, POINTER, DWORD, POINTER, DWORD, DWORD
ObjectEnd

    .code
Method KDevice.Init, uses esi, pszDevName : POINTER, dwDeviceType : DWORD, pszLinkName : POINTER, dwChar : DWORD, dwDevFlags : DWORD
    local pMyDevice : POINTER
    local pDriverObject : POINTER
    local szDevName : UNICODE_STRING

    invoke DbgPrint, $OfsCStr("KDevice.Init()")
    invoke DbgPrint, $OfsCStr("    DevName:  %ws"),  pszDevName
    invoke DbgPrint, $OfsCStr("    DevType:  0x%x"), dwDeviceType
    invoke DbgPrint, $OfsCStr("    LinkName: %p"),   pszLinkName
    invoke DbgPrint, $OfsCStr("    Char:     0x%x"), dwChar
    invoke DbgPrint, $OfsCStr("    Flags:    0x%x"), dwDevFlags
    ACall esi.Init

    OCall $ObjTmpl(KDriver)::KDriver.DriverObject
    mov pDriverObject, eax
 
    invoke RtlInitUnicodeString, addr szDevName, pszDevName
    invoke IoCreateDevice, pDriverObject, 0, addr szDevName, dwDeviceType, 0, FALSE, addr pMyDevice

    mov eax, pMyDevice
    m2m (DEVICE_OBJECT ptr [eax]).Flags, dwDevFlags
MethodEnd
