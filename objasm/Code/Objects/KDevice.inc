;;
;; Author:   Steward Fu
;; Created:  2024/07/29
;; Updated:  2024/07/29
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ddk.inc
%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ntstatus.inc

    .const
Object KDevice, KDeviceID, Primer
    RedefineMethod Init, POINTER, DWORD

    DefineVariable m_pDeviceObject, POINTER, 0
    DefineVariable m_pNextDeviceObject, POINTER, 0
ObjectEnd

    .code
Method KDevice.Init, uses esi, szDevName:POINTER, dwType:DWORD
    local pDriverObject : POINTER

    SetObject esi
    invoke DbgPrint, $OfsCStr("KDevice.Init")

    OCall $ObjTmpl(KDriver)::KDriver.DriverObject
    mov pDriverObject, eax
 
    invoke RtlInitUnicodeString, addr szDevName, szDevName
    invoke IoCreateDevice, pDriverObject, 0, addr szDevName, dwType, 0, FALSE, addr [esi].m_pDeviceObject
    invoke IoAttachDeviceToDeviceStack, [esi].m_pDeviceObject, pPhyDevice
  
    m2m [esi].m_pNextDeviceObject, eax
    or (DEVICE_OBJECT PTR [eax]).Flags, DO_BUFFERED_IO
    and (DEVICE_OBJECT PTR [eax]).Flags, not DO_DEVICE_INITIALIZING
    mov eax, STATUC_SUCCESS
MethodEnd
