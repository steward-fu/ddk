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
    
IOCTL_START equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_BUFFERED, FILE_ANY_ACCESS)
IOCTL_STOP  equ CTL_CODE(FILE_DEVICE_UNKNOWN, 801h, METHOD_BUFFERED, FILE_ANY_ACCESS)
    
.const
DEV_NAME db "\\.\MyDriver",0
    
.data?
hFile dd ?
dwRet dd ?
    
.code
start:
    invoke CreateFile, offset DEV_NAME, GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
    mov hFile, eax
        
    invoke DeviceIoControl, hFile, IOCTL_START, NULL, 0, NULL, 0, offset dwRet, NULL
    invoke Sleep, 3000
    invoke DeviceIoControl, hFile, IOCTL_STOP, NULL, 0, NULL, 0, offset dwRet, NULL
 
    invoke CloseHandle, hFile
    invoke ExitProcess, 0
end start
