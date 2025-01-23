.386p
.model flat, stdcall
option casemap:none
     
include c:\masm32\include\windows.inc
include c:\masm32\include\masm32.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\msvcrt.inc
include c:\masm32\include\kernel32.inc
include c:\masm32\include\w2k\ntddkbd.inc
include c:\masm32\Macros\Strings.mac
      
includelib c:\masm32\lib\user32.lib
includelib c:\masm32\lib\masm32.lib
includelib c:\masm32\lib\msvcrt.lib
includelib c:\masm32\lib\kernel32.lib
     
IOCTL_QUEUE   equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_BUFFERED, FILE_ANY_ACCESS)
IOCTL_PROCESS equ CTL_CODE(FILE_DEVICE_UNKNOWN, 801h, METHOD_BUFFERED, FILE_ANY_ACCESS)
 
.data?
cnt         dd ?
hFile       dd ?
dwRet       dd ?
event       dd 3 dup(?)
ov          OVERLAPPED <?>
  
.code
start:
    invoke CreateFile, $CTA0("\\\\.\\MyDriver"), GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED or FILE_ATTRIBUTE_NORMAL, 0
    mov hFile, eax
 
    mov cnt, 3
    invoke crt_memset, offset ov, 0, sizeof OVERLAPPED
    .while cnt > 0
        invoke CreateEvent, NULL, TRUE, FALSE, NULL
        mov ov.hEvent, eax
          
        mov ecx, cnt
        mov edi, offset event
        mov [edi + ecx * 4], eax
 
        invoke DeviceIoControl, hFile, IOCTL_QUEUE, NULL, 0, NULL, 0, offset dwRet, offset ov
        invoke crt_printf, $CTA0("Queued Event\n")
        sub cnt, 1
    .endw
      
    invoke crt_printf, $CTA0("Processing All Events\n")
    invoke DeviceIoControl, hFile, IOCTL_PROCESS, NULL, 0, NULL, 0, offset dwRet, NULL
      
    mov cnt, 3
    .while cnt > 0
        mov ecx, cnt
        mov edi, offset event
        mov eax, [edi + ecx * 4]
        mov ov.hEvent, eax
          
        invoke WaitForSingleObject, ov.hEvent, INFINITE
        invoke CloseHandle, ov.hEvent
        invoke crt_printf, $CTA0("Complete\n")
        sub cnt, 1
    .endw
    invoke CloseHandle, hFile
    invoke ExitProcess, 0
end start
.end
