;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1998  Microsoft Corporation.  All Rights Reserved.
;
;   File:   poll.inc
;   Content:    include file for polling and timing routines
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Port equates
;
System_Clock_Port   EQU 40h         ; Counter 0 port
Sys_Clock_Ctrl_Port EQU 43h         ; Counter control port

; macro to read the system timer, leaving the value in eax
; MUST be used with interrupts disabled!
;
Read_Sys_Clock MACRO
    xor     eax,eax
    out     Sys_Clock_Ctrl_Port, al     ; Current count has been latched
    IO_DELAY
    IO_DELAY                   
    in      al, System_Clock_Port       ; read the low byte of cur count
    IO_DELAY
    IO_DELAY
    mov     ah, al
    in      al, System_Clock_Port       ; read high byte of the count
    xchg    ah, al
ENDM

; Number of shifts to convert scale to fixed point binary
BIN_POINT EQU 20

; macro to output a string, leaving registers untouched
;
DPF MACRO Message
IFDEF DEBUG
    pushfd
    pushad
    mov     esi,OFFSET32 &Message
    VMMCall Out_Debug_String
    popad
    popfd
ENDIF
ENDM

