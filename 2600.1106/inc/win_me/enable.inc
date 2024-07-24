PAGE 58,132
;******************************************************************************
;TITLE Vmini.inc - Vmini VxD includes
;******************************************************************************
;
;   Title:      mini.inc
;
;   Version:    $Revision:   1.7  $
;		  $Modtime:   15 Sep 1993 15:31:18  $
;   Date:       $Date:   03 Jan 1994 09:57:36  $
;
;   Author:     
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE     REV                 DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;     3/18/93	1.0	BCK - Code
;==============================================================================

        .386p

;------------------------------------------------------------------------------
; 			Define Services 
;------------------------------------------------------------------------------

Begin_Service_Table ENABLE
  ENABLE_Service	VMINI_GetVersion,LOCAL
  ENABLE_Service 	VMINI_Update,LOCAL
  ENABLE_Service 	VMINI_Status,LOCAL
  ENABLE_Service	VMINI_DisplayError,LOCAL
  ENABLE_Service	VMINI_SetTimeStamp, LOCAL
  ENABLE_Service	VMINI_Siren, LOCAL
  ENABLE_Service	VMINI_RegisterAccess,LOCAL
  ENABLE_Service	VMINI_GetData,LOCAL
  ENABLE_Service	VMINI_ShutDownItem,LOCAL
  ENABLE_Service	VMINI_RegisterSK,LOCAL
End_Service_Table ENABLE

; ENABLE Version number

ENABLE_VERSION		equ 0400h


;
; Define equates for sound
;

no_tone_len     equ      1              ; very soft click                   
low_tone_len    equ     20              ; 0.01 seconds                      
high_tone_len   equ     80              ; 0.04 seconds                      
click_tone_len  equ      3              ;                                   
siren_duration  equ	   40
bell_duration   equ	   250

ifdef NEC_98

VSD_Sound_Port          EQU     35h             ; Hardware I/O port for sound
                                                ; enable/disable             
VSD_Beep		EQU	8

else

VSD_Sound_Port          EQU     61h             ; Hardware I/O port for sound
                                                ; enable/disable             

endif

VSD_Counter_Enable      EQU     00000001b       ; Bit enables counter        
VSD_Counter_Enable_Bit  EQU     0                                            
VSD_Data                EQU     00000010b       ; Sound "data" bit           
VSD_Data_Bit            EQU     1                                            

;
; Define siren parameters
;

	; Milliseconds between siren tone changes
TIMERBLOCK		equ	3		; milliseconds between timer events
ifdef NEC_98
LOW_TONE_VALUE_10     	equ 	0de7h		; low bell
LOW_TONE_VALUE_8     	equ 	0b4ch		; low bell
HIGH_TONE_VALUE_10   	equ	0a8eh		; high bell
HIGH_TONE_VALUE_8    	equ	894h		; high bell
BELL_VALUE_10		equ	0abfh		; Medium bell
BELL_VALUE_8		equ	8bbh		; Medium bell
SOFT_TONE_10		equ	29h		; Soft click
SOFT_TONE_8		equ	21h		; Soft click
CLICK_TONE_VALUE_10	equ	0de7h		; click
CLICK_TONE_VALUE_8	equ	0b4ch		; click
SIREN_INCREMENT_10	equ	62		; Increment on sound sirens
SIREN_INCREMENT_8	equ	50		; Increment on sound sirens
else
LOW_TONE_VALUE     	equ 	6c0h		; low bell
HIGH_TONE_VALUE    	equ	520h		; high bell
BELL_VALUE		equ	538h		; Medium bell
SOFT_TONE		equ	14h		; Soft click
CLICK_TONE_VALUE	equ	6c0h		; click
SIREN_INCREMENT	equ	30		; Increment on sound sirens
endif ; NEC_98

