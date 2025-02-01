; Copyright (c) 1998  Bull Smart Cards & Terminals
;
; Module Name:
;
;         TLP3Strt.asm
;
; Abstract:
;
;         Startup module for Smart Card Template Driver
;
; Environment:
;
;         Windows 9x
;

;
;
TITLE TLP3Strt.asm - SmarTLP3 Smart Card Reader Driver for Windows 9x
PAGE 58,132

;
; Instruction set
;
        .386p
;
; Inclusions
;
        .xlist
        include vmm.inc
        include debug.inc
        include configmg.inc
        .list

;
; Definitions
; (raise init order to make sure the driver is loaded after smclib.vxd)
;
BULLTLP3_DYNAMIC            EQU             1
BULLTLP3_DEVICE_ID          EQU             08000H 
BULLTLP3_INIT_ORDER         EQU             ( UNDEFINED_INIT_ORDER + 100 )

;
; Device Declaration
;
DECLARE_VIRTUAL_DEVICE  BULLTLP3, PNPDRVS_Major_Ver, PNPDRVS_Minor_Ver, _TLP3ControlProcedure, \
                        BULLTLP3_DEVICE_ID, \
                        BULLTLP3_INIT_ORDER


;
; Code Segment
;
VXD_LOCKED_CODE_SEG

;
BeginProc       _TLP3ControlProcedure
;++
;
;   descr:
;               BULLTLP3.vxd device control procedure
;
;   entry:
;               EAX = Control call ID
;
;   exit:
;               ~C  SUCCESS
;               C   control failed
;
;   uses:
;               EAX, EBX, ECX, EDX, ESI, EDI, Flags
;--

        Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, TLP3SysDynamicDeviceInit,      \
                sCall, <0>

        Control_Dispatch SYS_DYNAMIC_DEVICE_EXIT, TLP3SysDynamicDeviceExit,      \
                sCall, <0>

        Control_Dispatch PNP_NEW_DEVNODE,         TLP3PnPNewDevnode,             \
                sCall, <ebx, edx>

        Control_Dispatch W32_DEVICEIOCONTROL,     TLP3DeviceIOControl,           \
                sCall, <ecx, ebx, edx, esi>

        clc
        ret

EndProc         _TLP3ControlProcedure
Public          _TLP3ControlProcedure

VXD_LOCKED_CODE_ENDS

END


