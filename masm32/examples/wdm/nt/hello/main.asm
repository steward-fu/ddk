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
 
.code
Unload proc pMyDriver : PDRIVER_OBJECT
    ret
Unload endp
  
DriverEntry proc pMyDriver : PDRIVER_OBJECT, pMyRegistry : PUNICODE_STRING
    invoke DbgPrint, $CTA0("Hello, world\:")
 
    mov eax, pMyDriver
    mov (DRIVER_OBJECT PTR [eax]).DriverUnload, offset Unload
    mov eax, STATUS_SUCCESS
    ret
DriverEntry endp
end
