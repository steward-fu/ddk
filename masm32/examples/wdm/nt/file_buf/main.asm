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
    szBuffer byte 255 dup(?)
    pNextDevice PDEVICE_OBJECT ?
MyDeviceExtension ends
 
.code
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
  
IrpReadWrite proc uses esi edi pMyDevice : PDEVICE_OBJECT, pIrp : PIRP
    local len : dword
   
    and len, 0
    mov eax, pMyDevice
    mov eax, (DEVICE_OBJECT ptr [eax]).DeviceExtension
    lea edi, (MyDeviceExtension ptr [eax]).szBuffer

    mov eax, pIrp
    push (_IRP ptr [eax]).AssociatedIrp.SystemBuffer
    pop esi

    IoGetCurrentIrpStackLocation pIrp
    movzx eax, (IO_STACK_LOCATION ptr [eax]).MajorFunction
    .if eax == IRP_MJ_WRITE
        invoke DbgPrint, $CTA0("IRP_MJ_WRITE")
     
        IoGetCurrentIrpStackLocation pIrp
        push (IO_STACK_LOCATION ptr [eax]).Parameters.Write._Length
        pop len

        invoke memcpy, edi, esi, len
        invoke DbgPrint, $CTA0("Buffer: %s, Length: %d"), edi, len
    .elseif eax == IRP_MJ_READ
        invoke DbgPrint, $CTA0("IRP_MJ_READ")
     
        invoke strcpy, esi, edi
        invoke strlen, edi
        mov len, eax
    .endif
   
    mov eax, pIrp
    push len
    pop (_IRP ptr [eax]).IoStatus.Information
    mov (_IRP ptr [eax]).IoStatus.Status, STATUS_SUCCESS
    fastcall IofCompleteRequest, pIrp, IO_NO_INCREMENT
    mov eax, STATUS_SUCCESS
    ret
IrpReadWrite endp
 
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
    
    invoke RtlInitUnicodeString, addr szDevName, $CTW0("\\Device\\MyDriver")
    invoke RtlInitUnicodeString, addr szSymName, $CTW0("\\DosDevices\\MyDriver")

    mov eax, pMyDriver
    mov (DRIVER_OBJECT ptr [eax]).MajorFunction[IRP_MJ_CREATE * (sizeof PVOID)], offset IrpOpenClose
    mov (DRIVER_OBJECT ptr [eax]).MajorFunction[IRP_MJ_CLOSE  * (sizeof PVOID)], offset IrpOpenClose
    mov (DRIVER_OBJECT ptr [eax]).MajorFunction[IRP_MJ_WRITE  * (sizeof PVOID)], offset IrpReadWrite
    mov (DRIVER_OBJECT ptr [eax]).MajorFunction[IRP_MJ_READ   * (sizeof PVOID)], offset IrpReadWrite
    mov (DRIVER_OBJECT ptr [eax]).DriverUnload, offset Unload
    
    invoke IoCreateDevice, pMyDriver, sizeof MyDeviceExtension, addr szDevName, FILE_DEVICE_UNKNOWN, 0, FALSE, addr pMyDevice
    .if eax == STATUS_SUCCESS
        mov eax, pMyDevice
        or (DEVICE_OBJECT ptr [eax]).Flags, DO_BUFFERED_IO
        and (DEVICE_OBJECT ptr [eax]).Flags, not DO_DEVICE_INITIALIZING
        invoke IoCreateSymbolicLink, addr szSymName, addr szDevName
    .endif
    ret
DriverEntry endp
end DriverEntry
.end
