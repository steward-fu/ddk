.386p
.model flat, stdcall
option casemap:none
    
include c:\masm32\include\w2k\ntstatus.inc
include c:\masm32\include\w2k\ntddk.inc
include c:\masm32\include\w2k\ntoskrnl.inc
include c:\masm32\include\w2k\ntddkbd.inc
include c:\masm32\Macros\Strings.mac

includelib c:\masm32\lib\wxp\i386\ntoskrnl.lib
 
public DriverEntry
 
MyDeviceExtension struct
    dpc KDPC <>
    timer KTIMER <>
    queue LIST_ENTRY <>
    pNextDev PDEVICE_OBJECT ?
MyDeviceExtension ends
 
IOCTL_QUEUE   equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_BUFFERED, FILE_ANY_ACCESS)
IOCTL_PROCESS equ CTL_CODE(FILE_DEVICE_UNKNOWN, 801h, METHOD_BUFFERED, FILE_ANY_ACCESS)
 
.const
MSG_QUEUE   byte "IOCTL_QUEUE",0
MSG_PROCESS byte "IOCTL_PROCESS",0
 
.code
OnTimer proc uses esi edi pDpc : PKDPC, pContext : PVOID, pArg1 : PVOID, PArg2 : PVOID
    mov eax, pContext
    mov esi, (DEVICE_OBJECT ptr [eax]).DeviceExtension
    lea edi, (MyDeviceExtension ptr [esi]).queue

    IsListEmpty edi
    .if eax == TRUE
        invoke KeCancelTimer, addr (MyDeviceExtension ptr [esi]).timer
        invoke DbgPrint, $CTA0("Finish")
    .else
        RemoveHeadList edi
         
        ; CONTAINING_RECORD 
        sub eax, _IRP.Tail.Overlay.ListEntry
        mov (_IRP ptr [eax]).IoStatus.Status, STATUS_SUCCESS
        and (_IRP ptr [eax]).IoStatus.Information, 0
        fastcall IofCompleteRequest, eax, IO_NO_INCREMENT
        invoke DbgPrint, $CTA0("Complete Irp")
    .endif
    ret
OnTimer endp
 
IrpOpenClose proc pMyDevice : PDEVICE_OBJECT, pIrp : PIRP
    IoGetCurrentIrpStackLocation pIrp
    movzx eax, (IO_STACK_LOCATION ptr [eax]).MajorFunction

    .if eax == IRP_MJ_CREATE
        invoke DbgPrint, $CTA0("IRP_MJ_CREATE")
    .elseif eax == IRP_MJ_CLOSE
        invoke DbgPrint, $CTA0("IRP_MJ_CLOSE")
    .endif
 
    mov eax, pIrp
    and (_IRP ptr [eax]).IoStatus.Information, 0
    mov (_IRP ptr [eax]).IoStatus.Status, STATUS_SUCCESS
    fastcall IofCompleteRequest, pIrp, IO_NO_INCREMENT
    mov eax, STATUS_SUCCESS
    ret
IrpOpenClose endp
 
IrpIOCTL proc uses esi edi ebx pMyDevice : PDEVICE_OBJECT, pIrp : PIRP
    local period : LARGE_INTEGER
 
    mov eax, pMyDevice
    mov esi, (DEVICE_OBJECT ptr [eax]).DeviceExtension
    lea edi, (MyDeviceExtension ptr [esi]).queue
 
    IoGetCurrentIrpStackLocation pIrp
    mov eax, (IO_STACK_LOCATION ptr [eax]).Parameters.DeviceIoControl.IoControlCode
    .if eax == IOCTL_QUEUE
        invoke DbgPrint, offset MSG_QUEUE
 
        mov ebx, pIrp
        lea ebx, (_IRP ptr [ebx]).Tail.Overlay.ListEntry
        InsertHeadList edi, ebx
        IoMarkIrpPending pIrp
        mov eax, STATUS_PENDING
        ret
    .elseif eax == IOCTL_PROCESS
        invoke DbgPrint, offset MSG_PROCESS
         
        or period.HighPart, -1
        mov period.LowPart, -10000000
        invoke KeSetTimerEx, addr (MyDeviceExtension ptr [esi]).timer, period.LowPart, period.HighPart, 10, addr (MyDeviceExtension ptr [esi]).dpc
    .endif
 
    mov eax, pIrp
    mov (_IRP ptr [eax]).IoStatus.Status, STATUS_SUCCESS
    and (_IRP ptr [eax]).IoStatus.Information, 0
    fastcall IofCompleteRequest, pIrp, IO_NO_INCREMENT
    mov eax, STATUS_SUCCESS
    ret
IrpIOCTL endp
 
Unload proc pMyDriver : PDRIVER_OBJECT
    local szSymName : UNICODE_STRING

    invoke RtlInitUnicodeString, addr szSymName, $CTW0("\\DosDevices\\MyDriver")
    invoke IoDeleteSymbolicLink, addr szSymName
                    
    mov eax, pMyDriver
    invoke IoDeleteDevice, (DRIVER_OBJECT ptr [eax]).DeviceObject
    ret
Unload endp
     
DriverEntry proc pMyDriver : PDRIVER_OBJECT, pMyRegistry : PUNICODE_STRING
    local pMyDevice : PDEVICE_OBJECT
    local szDevName : UNICODE_STRING
    local szSymName : UNICODE_STRING
         
    mov eax, pMyDriver
    mov (DRIVER_OBJECT ptr [eax]).MajorFunction[IRP_MJ_CREATE * (sizeof PVOID)], offset IrpOpenClose
    mov (DRIVER_OBJECT ptr [eax]).MajorFunction[IRP_MJ_CLOSE * (sizeof PVOID)], offset IrpOpenClose
    mov (DRIVER_OBJECT ptr [eax]).MajorFunction[IRP_MJ_DEVICE_CONTROL * (sizeof PVOID)], offset IrpIOCTL
    mov (DRIVER_OBJECT ptr [eax]).DriverUnload, offset Unload
         
    invoke RtlInitUnicodeString, addr szDevName, $CTW0("\\Device\\MyDriver")
    invoke RtlInitUnicodeString, addr szSymName, $CTW0("\\DosDevices\\MyDriver")

    invoke IoCreateDevice, pMyDriver, sizeof MyDeviceExtension, addr szDevName, FILE_DEVICE_UNKNOWN, 0, FALSE, addr pMyDevice
    .if eax == STATUS_SUCCESS
        mov eax, pMyDevice
        or (DEVICE_OBJECT ptr [eax]).Flags, DO_BUFFERED_IO
        and (DEVICE_OBJECT ptr [eax]).Flags, not DO_DEVICE_INITIALIZING
         
        mov eax, pMyDevice
        mov eax, (DEVICE_OBJECT ptr [eax]).DeviceExtension
        lea eax, (MyDeviceExtension ptr [eax]).queue
        InitializeListHead eax
 
        mov eax, pMyDevice
        mov eax, (DEVICE_OBJECT ptr [eax]).DeviceExtension
        invoke KeInitializeTimer, addr (MyDeviceExtension ptr [eax]).timer
 
        mov eax, pMyDevice
        mov eax, (DEVICE_OBJECT ptr [eax]).DeviceExtension
        invoke KeInitializeDpc, addr (MyDeviceExtension ptr [eax]).dpc, offset OnTimer, pMyDevice
        invoke IoCreateSymbolicLink, addr szSymName, addr szDevName
    .endif
    ret
DriverEntry endp
end DriverEntry
.end
