;;
;; Author:   Steward Fu
;; Created:  2024/07/24
;; Updated:  2024/07/29
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ddk.inc
%include @Environ(OBJASM_PATH)\Code\Inc\DDK\ntstatus.inc

    .data
m_TheDriver POINTER 0

    .const
Object KDriver, KDriverID, Primer
    VirtualAbstract DriverEntry, PUNICODE_STRING
    VirtualAbstract AddDevice, PDEVICE_OBJECT
    StaticMethod DriverInit, PDRIVER_OBJECT, PUNICODE_STRING
    VirtualMethod SetUpDispatchCalls

    DefineVariable m_pDriverObject, PDRIVER_OBJECT, NULL
ObjectEnd

    .code
Method KDriver.SetUpDispatchCalls, uses esi
    SetObject esi

    invoke DbgPrint, $OfsCStr("KDriver.SetUpDispatchCalls")

    mov eax, [esi].m_pDriverObject
    mov (DRIVER_OBJECT PTR [eax]).MajorFunction[IRP_MJ_PNP * (sizeof PVOID)], offset DriverIrpDispatch
    mov (DRIVER_OBJECT PTR [eax]).DriverUnload, offset UnloadDispatch
    mov eax, (DRIVER_OBJECT PTR [eax]).DriverExtension
    mov (DRIVER_EXTENSION PTR [eax]).AddDevice, AddDeviceDispatch
MethodEnd

Method KDriver.DriverInit, uses esi, pMyDriver:PDRIVER_OBJECT, pMyRegistry:PUNICODE_STRING
    SetObject esi

    m2m [esi].m_pDriverObject, pMyDriver
    invoke DbgPrint, $OfsCStr("KDriver.DriverInit")

    SetObject esi, KDriver, m_TheDriver
    m2m [esi].m_pDriverObject, pMyDriver
MethodEnd

AddDeviceDispatch proc uses esi, pMyDriver:PDRIVER_OBJECT, pPhyDevice:PDEVICE_OBJECT
    invoke DbgPrint, $OfsCStr("KDriver.AddDeviceDispatch")

    mov esi, m_TheDriver
    OCall esi::KDriver.AddDevice, pPhyDevice
    ret
AddDeviceDispatch endp

DriverIrpDispatch proc uses esi, pMyDevice:PDEVICE_OBJECT, pIrp:PIRP
    invoke DbgPrint, $OfsCStr("KDriver.DriverIrpDispatch")
    mov eax, -1
    ret
DriverIrpDispatch endp

UnloadDispatch proc uses esi
    invoke DbgPrint, $OfsCStr("KDriver.UnloadDispatch")
UnloadDispatch endp

DriverEntry proc uses esi, pMyDriver:PDRIVER_OBJECT, pMyRegistry:PUNICODE_STRING
    invoke DbgPrint, $OfsCStr("DriverEntry")

    invoke __create_driver_class_instance
    OCall $ObjTmpl(KDriver)::KDriver.DriverInit, pMyDriver, pMyRegistry
    OCall $ObjTmpl(KDriver)::KDriver.SetUpDispatchCalls

    mov esi, m_TheDriver
    OCall esi::KDriver.DriverEntry, pMyRegistry

    mov eax, STATUS_SUCCESS
    ret
DriverEntry endp
