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
    cnt DWORD ?             
    pNextDev PDEVICE_OBJECT ?
MyDeviceExtension ends
 
IOCTL_START equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_BUFFERED, FILE_ANY_ACCESS)
IOCTL_STOP  equ CTL_CODE(FILE_DEVICE_UNKNOWN, 801h, METHOD_BUFFERED, FILE_ANY_ACCESS)
 
.const
MSG_START byte "IOCTL_START",0
MSG_STOP  byte "IOCTL_STOP",0
 
.code
OnTimer proc uses esi pMyDevice : PDEVICE_OBJECT, pContext : PVOID
    mov eax, pMyDevice
    mov esi, (DEVICE_OBJECT ptr [eax]).DeviceExtension
    inc (MyDeviceExtension ptr [esi]).cnt
    invoke DbgPrint, $CTA0("IoTimer: %d\n"), (MyDeviceExtension ptr [esi]).cnt
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
 
IrpIOCTL proc uses esi pMyDevice : PDEVICE_OBJECT, pIrp : PIRP
    local hThread : DWORD
    local pThread : PVOID
 
    mov eax, pMyDevice
    mov esi, (DEVICE_OBJECT ptr [eax]).DeviceExtension
 
    IoGetCurrentIrpStackLocation pIrp
    mov eax, (IO_STACK_LOCATION ptr [eax]).Parameters.DeviceIoControl.IoControlCode
    .if eax == IOCTL_START
        invoke DbgPrint, offset MSG_START

        mov (MyDeviceExtension ptr [esi]).cnt, 0
        invoke IoStartTimer, pMyDevice
    .elseif eax == IOCTL_STOP
        invoke DbgPrint, offset MSG_STOP
        invoke IoStopTimer, pMyDevice
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
        invoke IoInitializeTimer, pMyDevice, offset OnTimer, NULL
        invoke IoCreateSymbolicLink, addr szSymName, addr szDevName
    .endif
    ret
DriverEntry endp
end DriverEntry
.end
