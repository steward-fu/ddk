;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;******************************************************************************
;
;	VDD virtual mode services for the grabber
;
;	WARNING!  Do not define any service numbers greater than 0ffh,
;	because many 3rd-party VDDs do not parameter-validate the upper
;	byte of the function code!
;
GRB_Get_Version     EQU 0
GRB_Get_Mem	    EQU GRB_Get_Version + 1
GRB_Get_State	    EQU GRB_Get_Mem + 1
GRB_Get_Mod	    EQU GRB_Get_State + 1
GRB_Clear_Mod	    EQU GRB_Get_Mod + 1
GRB_Free_Mem	    EQU GRB_Clear_Mod + 1
GRB_Get_GrbMem	    EQU GRB_Free_Mem + 1
GRB_Free_Grab	    EQU GRB_Get_GrbMem + 1
GRB_Get_GrbState    EQU GRB_Free_Grab + 1
GRB_Unlock_APP	    EQU GRB_Get_GrbState + 1

; New API's for 3.1 display drivers
Private_DspDrvr_1   EQU 0Ah
			    .errnz Private_DspDrvr_1 - GRB_Unlock_APP - 1
DspDrvr_Version     EQU 0Bh
DspDrvr_Addresses   EQU 0Ch

; New API's for enhanced-mode DOS VDDs
DspDrvr_Clear_Addresses EQU	0Dh

Grabber_VerNum	    EQU     0400h	    ; version 4.00

;*******************************************************************************
;
;	Controller state structure
;
VDA_Stt_Struc	struc
	VDA_Stt_Mode	db	?		; Display mode
	VDA_Stt_Flags	db	?		; Flags
	VDA_Stt_PIF	dw	?		; PIF Information
	VDA_Stt_CurX	dw	?		; Cursor horizontal position
	VDA_Stt_CurY	dw	?		; Cursor vertical position
	VDA_Stt_CurBeg	dw	?		; Cursor start scan line
	VDA_Stt_CurEnd	dw	?		; Cursor end scan line
	VDA_Stt_Rows	db	?		; Text Rows
	VDA_Stt_ChrH	db	?		; Charactor Height
	VDA_Stt_Cols	db	?		; Text Colmuns
	VDA_Stt_ChrW	db	?		; Charactor Width
	VDA_Stt_ResY	dw	?		; Grph Y Resolution
	VDA_Stt_ResX	dw	?		; Grph X Resolution
	VDA_Stt_KCG	dd	?		; KCG's Font Address
	VDA_Stt_Font	dd	?		; EUDC's Font Address
	VDA_Stt_Pal	db	4 dup (?)	; 8 color palette
	VDA_Stt_Pal16	db	3*16 dup (?)	; 16 color palette
	VDA_Stt_Offset	dd	30 dup (?)	; Offset of each row
	VDA_Stt_PalEx	db	3*256 DUP (?)	; 256 color palette
VDA_Stt_Struc	ends

.ERRNZ SIZE VDA_Stt_Struc MOD 4

;*****************************
;       VDA_EGA_Mode
;
GCo_Bw_BIT		equ	00000001b	; 0:Color, 1:Mono
G8_16_BIT		equ	00000010b	; 0:8 color, 1:16 color
G400_200_BIT1		equ	00000100b	; 0:400line, 1:200line
G400_200_BIT2		equ	00001000b	; 0:400line, 1:480line
G200_U_D		equ	00010000b	; 0:Upper, 1:Lower (200line)
T25_20_BIT2		equ	00100000b	; 0:25/20Rows, 1:30Rows
T25_20_BIT		equ	01000000b	; 0:25Rows, 1:20Rows
T80_40_BIT		equ	10000000b	; 0:80Cols, (1:40Cols)

;*****************************
;       VDA_EGA_Flags
;
fVDA_V_ScOff		EQU	00000001b	; Screen off
fVDA_V_ScOffBit		EQU	0		;
fVDA_V_HCurTrk		EQU	00000010b	; Horizontal cursor track
fVDA_V_HCurTrkBit	EQU	1		;   - keep cursor position in window
fVDA_V_InvDAC		EQU	00000100b	; GEM Ventura programs DAC 0 as white
fVDA_V_InvDACBit	EQU	2		;   - set if DAC 0 is non-zero
fVDA_V_T_ScOff		EQU	00001000b	; Text off
fVDA_V_T_ScOffBit	EQU	3		;
fVDA_V_G_ScOff		EQU	00010000b	; Grph off
fVDA_V_G_ScOffBit	EQU	4		;
fVDA_V_ExColor		EQU	00100000b	; Extend color mode
fVDA_V_ExColorBit	EQU	5		;
fVDA_V_SemiGrp		EQU	01000000b	; Semi-Graphics Mode
fVDA_V_SemiGrpBit	EQU	6		;


;******************************************************************************
;
;	Modification state structure
;
VDA_Mod_Struc	struc
	VDA_Mod_Flags	dw	?		; Modified state flag
	VDA_Mod_Count	dw	?		; Count of entries in modification list
	VDA_Mod_List	db	30*8 dup (?)	; The modification list
	VDA_Mod_CountG	dw	?		; Count of entries in modification list
	VDA_Mod_ListG	db	8*8 dup (?)	; The modification list
VDA_Mod_Struc	ends

;*******
;
; Masks and bit numbers for VDD_Mod_Flag
;
fVDD_M_Type	EQU 0000000000000111B	; Mask for value field
fVDD_M_TypeBit	EQU 0			; start bit for value field
fVDD_M_Ctlr	EQU 0000000000001000B	; flag indicating controller state chg
fVDD_M_CtlrBit	EQU 3			; bit for controller state change
fVDD_M_VRAM	EQU 0000000000010000B	; flag indicating Video RAM change
fVDD_M_VRAMBit	EQU 4			; bit for Video RAM change
fVDD_M_Curs	EQU 0000000000100000B	; flag indicating Cursor change
fVDD_M_CursBit	EQU 5			; bit for Cursor change
fVDD_M_ScOff	EQU 0000000001000000B	; flag indicating screen is off
fVDD_M_ScOffBit EQU 6			; bit for screen off
fVDD_M_Err	EQU 1000000000000000B	; flag indicating error, can't pass mods
fVDD_M_ErrBit	EQU 15			; bit for error
;
; Values for fVDD_M_TypeMsk field
;
fVDD_M_Type_Page	EQU    0	; Bit field of modified pages
fVDD_M_Type_Rect	EQU    1	; Array of RECT structures

;*******
;
; Structure for accessing RECTs
;
ifndef Rect
    ??$need_rect equ 1
endif

ifdef ??$need_rect
Rect    struc
        rcLeft          dw      ?
        rcTop           dw      ?
        rcRight         dw      ?
        rcBottom        dw      ?
Rect    ends
endif

;********
;
;	Scroll rect definition
;
; If the high bit of R_Left is set in one of these RECTs, then it is
;  not a rect but a scroll event specification.
;  THIS APPLIES TO TEXT MODE ONLY (modes 0-3)!!
;  A scroll event spec looks like this:
;
ScrollRect STRUC
    ScrCnt  DB	?	    ; Count of lines to scroll in scroll region
    ScrFlgs DB	?	    ; Flags
    ScrRgt  DB	?	    ; Right Col of scroll (0 indexed)(ignore if full width bit set)
    ScrLft  DB	?	    ; Left Col of scroll (0 indexed)(ignore if full width bit set)
    ScrBot  DB	?	    ; Bottom line of scroll (0 indexed)
    ScrTop  DB	?	    ; Top line of scroll (0 indexed)
    ScrFch  DB	?	    ; Fill character for blank lines of scroll
    ScrFatt DB	?	    ; Fill attribute for blank lines of scroll
ScrollRect ENDS

; Masks for ScrFlgs
Scr_M_Scroll	    EQU 10000000B   ; High bit of flags indicates scroll
Scr_M_ScrollBit     EQU 7
Scr_M_FullWid	    EQU 00000001B   ; Set if scroll is full width (ignore Rgt Lft)
Scr_M_FullWidBit    EQU 0
Scr_M_Up	    EQU 00000010B   ; Set if scroll is UP, clear if DOWN
Scr_M_UpBit	    EQU 1

ifndef  MASM6
IF2
    IF (SIZE ScrollRect) NE (SIZE Rect)
	%out ERROR -- ScrollRect is wrong size!!!!!
    ENDIF
ENDIF
else
    IF (SIZE ScrollRect) NE (SIZE Rect)
	%out ERROR -- ScrollRect is wrong size!!!!!
    ENDIF
endif   ; MASM6



;*******************************************************************************
;
;	Memory state structure
;
VDA_Mem_Struc	struc
	VDA_Mem_AddrT	dd	?		; Address of text memory (16K)
	VDA_Mem_AddrG	dd	?		; Address of plane memory (128K)
	VDA_Mem_AddrX	dd	?		; Address of packed memory (512K)
VDA_Mem_Struc	ends

