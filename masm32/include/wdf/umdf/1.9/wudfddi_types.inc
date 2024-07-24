;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;
;  Written by Steward_Fu (steward.fu@gmail.com)
;  https://steward-fu.github.io/website/index.htm
;  
;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
;//typedef enum _WDF_DEVICE_IO_TYPE {
    WdfDeviceIoUndefined        equ 0
    WdfDeviceIoNeither          equ 1
    WdfDeviceIoBuffered         equ 2
    WdfDeviceIoDirect           equ 3
    WdfDeviceIoBufferedOrDirect equ 4
    WdfDeviceIoMaximum          equ 5
;//} WDF_DEVICE_IO_TYPE, *PWDF_DEVICE_IO_TYPE;
WDF_DEVICE_IO_TYPE typedef DWORD
PWDF_DEVICE_IO_TYPE typedef ptr WDF_DEVICE_IO_TYPE