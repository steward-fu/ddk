;;
;; Purpose: IOCTL IRP Test
;; Website: https://steward-fu.github.io/website/index.htm
;;

.386p
.model flat, stdcall
option casemap:none
  
include c:\masm32\include\windows.inc
include c:\masm32\include\masm32.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\msvcrt.inc
include c:\masm32\include\shell32.inc
include c:\masm32\include\kernel32.inc
include c:\masm32\include\w2k\ntddkbd.inc
include c:\masm32\Macros\Strings.mac
 
includelib c:\masm32\lib\user32.lib
includelib c:\masm32\lib\masm32.lib
includelib c:\masm32\lib\msvcrt.lib
includelib c:\masm32\lib\shell32.lib
includelib c:\masm32\lib\kernel32.lib
 
IOCTL_TEST      equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_BUFFERED,   FILE_ANY_ACCESS)
IOCTL_GET_BUF   equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_BUFFERED,   FILE_ANY_ACCESS)
IOCTL_SET_BUF   equ CTL_CODE(FILE_DEVICE_UNKNOWN, 801h, METHOD_BUFFERED,   FILE_ANY_ACCESS)
IOCTL_GET_DIR   equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
IOCTL_SET_DIR   equ CTL_CODE(FILE_DEVICE_UNKNOWN, 801h, METHOD_IN_DIRECT,  FILE_ANY_ACCESS)
IOCTL_GET_NEI   equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_NEITHER,    FILE_ANY_ACCESS)
IOCTL_SET_NEI   equ CTL_CODE(FILE_DEVICE_UNKNOWN, 801h, METHOD_NEITHER,    FILE_ANY_ACCESS)
 
.const 
DEV_NAME    db "\\.\MyDriver",0
MSG_SEND    db "I am error",0
MSG_WR      db "SET: %s, %d",10,13,0
MSG_RD      db "GET: %s, %d",10,13,0
  
.data
dwCode0     dd IOCTL_TEST
dwCode1     dd IOCTL_TEST
 
.data?
hFile       dd ?
dwRet       dd ?
szBuffer    db 255 dup(?)
  
.code
start:
    invoke GetCommandLineW
    invoke CommandLineToArgvW, eax, offset dwRet
     
    .if dwRet > 1
        invoke WideCharToMultiByte, CP_ACP, 0, dword ptr [eax + 4], -1, offset szBuffer, 255, 0, 0
 
        invoke lstrcmp, $CTA0("BUF"), offset szBuffer
        .if eax == 0
            mov dwCode0, IOCTL_SET_BUF
            mov dwCode1, IOCTL_GET_BUF
        .endif
         
        invoke lstrcmp, $CTA0("DIR"), offset szBuffer
        .if eax == 0
            mov dwCode0, IOCTL_SET_DIR
            mov dwCode1, IOCTL_GET_DIR
        .endif
         
        invoke lstrcmp, $CTA0("NEI"), offset szBuffer
        .if eax == 0
            mov dwCode0, IOCTL_SET_NEI
            mov dwCode1, IOCTL_GET_NEI
        .endif
    .endif
 
    invoke CreateFile, offset DEV_NAME, GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
    mov hFile, eax
 
    .if dwCode0 == IOCTL_TEST
        invoke DeviceIoControl, hFile, IOCTL_TEST, NULL, 0, NULL, 0, offset dwRet, NULL
    .else
        invoke StrLen, offset MSG_SEND
        mov dwRet, eax
 
        invoke crt_printf, offset MSG_WR, offset MSG_SEND, dwRet
        invoke DeviceIoControl, hFile, dwCode0, offset MSG_SEND, dwRet, NULL, 0, offset dwRet, NULL
     
        and dwRet, 0
        invoke crt_memset, offset szBuffer, 0, 255
        invoke DeviceIoControl, hFile, dwCode1, NULL, 0, offset szBuffer, 255, offset dwRet, NULL
        invoke crt_printf, offset MSG_RD, offset szBuffer, dwRet
    .endif
 
    invoke CloseHandle, hFile
    invoke ExitProcess, 0
end start
end
