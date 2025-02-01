;******************************************************************************
TITLE CONTROL - DigiJoy - Gameport digital joystick mini-driver
;******************************************************************************
;
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
; WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
;
;
;
;  Copyright (c) 1998  Microsoft Corporation.  All Rights Reserved.
; 
;   Notes:
; 
;   File:       digijoy.asm
;   Content:    This file handles driver init, control entry point and exit
;

PAGE 58,132
    .386p

;******************************************************************************
;                I N C L U D E S
;******************************************************************************

    .xlist
    include vmm.inc
    include vjoyd.inc
    .list

DPF MACRO message
IFNDEF RETAIL
    SaveReg <ad>
    mov esi,OFFSET32 message
    VMMCall Out_Debug_String
    RestoreReg <ad>
ENDIF
ENDM
    

EXTRN _RegisterLoad@0:PROC
EXTRN _InitRoutine:PROC
EXTRN _ExitRoutine:PROC

IFNDEF RETAIL
EXTRN _DIGIJOY_DebugQuery@0:PROC
ENDIF

PUBLIC _VJoyDRegister@4

; the following equate makes the VXD dynamically loadable.

DIGIJOY_DYNAMIC EQU 1

;---------------------------------------------------------------------------;
;                   P A G E A B L E   D A T A
;---------------------------------------------------------------------------;
VxD_PAGEABLE_DATA_SEG

IFNDEF RETAIL
    ReInitMsg               db "DIGIJOY: ReInit",13,10,0
    EOLMsg                  db 13,10,0
ENDIF

VxD_PAGEABLE_DATA_ENDS

VxD_PAGEABLE_CODE_SEG
BeginProc DIGIJOY_Dynamic_ReInit
    DPF ReInitMsg

    call _RegisterLoad@0

    clc
    ret
EndProc DIGIJOY_Dynamic_ReInit

;===========================================================================
;
;   PROCEDURE: _VJoyDRegister@4
;
;   DESCRIPTION:
;       Calls DX5 registration
;
;   ENTRY:
;       pRegInfo filled in with driver registration details
;
;   EXIT:
;       
;
;   USES:
;       Flags
;
;============================================================================
BeginProc _VJoyDRegister@4,SCALL,PUBLIC,ESP
    ArgVar      pRegInfo,   DWORD

    EnterProc
    SaveReg <ebx>

    mov eax,0ffffffffh  ; signal new style registrqtion
    xor ebx,ebx
    mov edx,pRegInfo
    xor ecx,ecx

    VxDCall VJOYD_Register_Device_Driver

    RestoreReg <ebx>
    LeaveProc
    return

EndProc _VJoyDRegister@4

VxD_PAGEABLE_CODE_ENDS

VxD_IDATA_SEG
;       Initialization data here - discarded after Init_Complete
IFNDEF RETAIL
    NoVjoyDMsg                  db "DIGIJOY: Fatal error, VJoyD not loaded at DeviceInit",13,10,0
    BadVjoyDMsg                 db "DIGIJOY: Fatal error, wrong version (#ax) of VJoyD",13,10,0
    VerMsg                      db "DIGIJOY: Init - VJoyD version #ax",13,10,0
ENDIF

VxD_IDATA_ENDS

VxD_ICODE_SEG
BeginProc DIGIJOY_Dynamic_Init

    ; Check that VJoyD is present, if not fail to load 
    ; so we get new init if VJoyD loads us
    mov eax, VJOYD_Device_ID
    xor edi,edi
    VMMCall Get_DDB

    test ecx,ecx
    jz FailNoVJoyD

    ; Check we have at least DX5 VJoyD
    ; Sadly VJoyD does no expose the conventional Get_Version service
    ; so just check the ddb version
    mov eax,DWORD PTR ( ( VxD_Desc_Block PTR[ecx]).DDB_Dev_Major_Version )
    and eax,0ffffh  ; High word is flags
    xchg al,ah
    DPF VerMsg
    cmp eax,0102h   ; DX5 initial version
    
    jb FailVers

    ; Do our init before registering as VJOYD calls _HWCapsRoutine before returning
    call _InitRoutine
    test eax,eax
    jnz FailInit

    clc                                     ; succeed
    ret
FailVers:
    DPF BadVJoyDMsg
    jmp FailInit
FailNoVJoyD:
    DPF NoVJoyDMsg
FailInit:
    stc
    ret

EndProc DIGIJOY_Dynamic_Init

VxD_ICODE_ENDS

;============================================================================
;        V I R T U A L   D E V I C E   D E C L A R A T I O N
;============================================================================

DECLARE_VIRTUAL_DEVICE  DIGIJOY, 1, 0, \
            DIGIJOY_Control, UNDEFINED_DEVICE_ID, UNDEFINED_INIT_ORDER

VxD_LOCKED_CODE_SEG
;===========================================================================
;
;   PROCEDURE: DIGIJOY_Control
;
;   DESCRIPTION:
;    Device control procedure for the DIGIJOY VxD
;
;   ENTRY:
;    EAX = Control call ID
;
;   EXIT:
;    If carry clear then
;        Successful
;    else
;        Control call failed
;
;   USES:
;    EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;============================================================================
SYS_DYNAMIC_DEVICE_REINIT EQU DESTROY_PROCESS+1

BeginProc DIGIJOY_Control
    Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, DIGIJOY_Dynamic_Init
    Control_Dispatch SYS_DYNAMIC_DEVICE_EXIT, DIGIJOY_Dynamic_Exit
    Control_Dispatch SYS_DYNAMIC_DEVICE_REINIT, DIGIJOY_Dynamic_ReInit
    Control_Dispatch BEGIN_RESERVED_PRIVATE_SYSTEM_CONTROL, DIGIJOY_Dynamic_ReInit
IFNDEF RETAIL
    Control_Dispatch DEBUG_QUERY,            _DIGIJOY_DebugQuery@0
ENDIF

    ;Control only reaches here if no match was found
    clc
    ret
EndProc DIGIJOY_Control

BeginProc DIGIJOY_Dynamic_Exit
    call _ExitRoutine
    clc                             ; succeed
    ret
EndProc DIGIJOY_Dynamic_Exit

VxD_LOCKED_CODE_ENDS

END

