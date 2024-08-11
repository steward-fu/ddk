;;
;; Author:   Steward Fu
;; Updated:  2024/08/10
;; Purpose:  Try to implment the code found from NuMega's DriverWorks
;;

    .data
g_pMyDriverInst POINTER NULL

    .const
Object KDriver, KDriverID, Primer
    StaticMethod DriverInit, PDRIVER_OBJECT, PUNICODE_STRING
    StaticMethod DriverObject
    StaticMethod DriverInstance

    RedefineMethod Init

    VirtualAbstract AddDevice, PDEVICE_OBJECT
    VirtualAbstract DriverEntry, PUNICODE_STRING
    VirtualAbstract DriverIrpDispatch, PIRP

    VirtualMethod SetUpDispatchCalls

    DefineVariable m_pMyDriver, PDRIVER_OBJECT, NULL
ObjectEnd

    .code
Method KDriver.Init, uses esi
    D $OfsCStr("KDriver.Init()")

    push pSelf
    pop g_pMyDriverInst
MethodEnd

Method KDriver.DriverInstance, uses esi
    D $OfsCStr("KDevice.DriverInstance()")

    mov eax, g_pMyDriverInst
MethodEnd

Method KDriver.DriverObject, uses esi
    D $OfsCStr("KDevice.DriverObject()")

    SetObject esi
    mov eax, [esi].m_pMyDriver
MethodEnd

Method KDriver.SetUpDispatchCalls, uses esi
    D $OfsCStr("KDriver.SetUpDispatchCalls()")

    SetObject esi
    mov eax, [esi].m_pMyDriver
    mov (DRIVER_OBJECT PTR [eax]).MajorFunction[IRP_MJ_PNP * (sizeof PVOID)], offset DriverIrpDispatch
    mov (DRIVER_OBJECT PTR [eax]).DriverUnload, offset UnloadDispatch
    mov eax, (DRIVER_OBJECT PTR [eax]).DriverExtension
    mov (DRIVER_EXTENSION PTR [eax]).AddDevice, offset AddDeviceDispatch
MethodEnd

Method KDriver.DriverInit, uses esi, pMyDriver : PDRIVER_OBJECT, pMyRegistry : PUNICODE_STRING
    D $OfsCStr("KDriver.DriverInit()")

    invoke __create_driver_class_instance

    OCall $ObjTmpl(KDriver)::KDriver.DriverInstance
    SetObject esi, KDriver, eax
    push pMyDriver
    pop [esi].m_pMyDriver
    OCall esi::KDriver.SetUpDispatchCalls
    OCall esi::KDriver.DriverEntry, pMyRegistry
MethodEnd

AddDeviceDispatch proc uses esi, pMyDriver : PDRIVER_OBJECT, pPhyDevice : PDEVICE_OBJECT
    D $OfsCStr("AddDeviceDispatch()")

    OCall $ObjTmpl(KDriver)::KDriver.DriverInstance
    OCall eax::KDriver.AddDevice, pPhyDevice

    mov eax, pMyDriver
    mov eax, (DRIVER_OBJECT ptr [eax]).DeviceObject
    and (DEVICE_OBJECT ptr [eax]).Flags, not DO_DEVICE_INITIALIZING
    mov eax, STATUS_SUCCESS
    ret
AddDeviceDispatch endp

DriverIrpDispatch proc uses esi, pMyDevice : PDEVICE_OBJECT, pMyIrp : PIRP
    D $OfsCStr("DriverIrpDispatch()")

    OCall $ObjTmpl(KDriver)::KDriver.DriverInstance
    OCall eax::KDriver.DriverIrpDispatch, pMyIrp
    ret
DriverIrpDispatch endp

UnloadDispatch proc
    D $OfsCStr("UnloadDispatch()")  
    ret
UnloadDispatch endp

DriverEntry proc pMyDriver : PDRIVER_OBJECT, pMyRegistry : PUNICODE_STRING
    D $OfsCStr("DriverEntry()")

    OCall $ObjTmpl(KDriver)::KDriver.DriverInit, pMyDriver, pMyRegistry
    ret
DriverEntry endp
