;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;******************************************************************************
;                                M A C R O S
;******************************************************************************

IOdelay		macro
		jmp	$+2
		jmp	$+2
		jmp	$+2
		out	5fh, al
		out	5fh, al
		out	5fh, al
		endm

ABEFlag	macro	nFlag, bFlag
b&nFlag		equ	bFlag
f&nFlag		equ	(1 shl bFlag)
endm

PPROC	macro	n, nf
n proc nf
public n
endm


;******************************************************************************
;                               E Q U A T E S
;******************************************************************************

; H/W cursor
ABE_CursorSizeX	equ	32
ABE_CursorSizeY	equ	32

; Glue logic
OPTION_PORTBASE	equ	0FA2h
MOTHER_PORTBASE	equ	0FAAh
DUMMY_ID	equ	0FFh

vGL_PRODUCT_ID	equ	00h
vGL_VGA_REMAP	equ	01h
vGL_BASE_ADDR	equ	02h	; High area remap base address
vGL_BOARD_CMD	equ	03h
vGL_STATUS_CMD	equ	04h

; ABE_Flags
ABEFlag	ABE_Enable, 0
ABEFlag	ABE_Cursor_Enable, 1

; Product ID  (Index 0 of Glue register)
mID_CHIP	equ	11001000b	; Chip ID
mID_BUS		equ	00110000b	; Bus ID
mID_COMP	equ	00000111b	; Board Comp

vID_S3_928	equ	00000000b	; 000
vID_S3_864	equ	00001000b	; 001
vID_GD5428	equ	01000000b	; 010
vID_GD5430	equ	01001000b	; 011
vID_MGA2	equ	10000000b	; 100
vID_GD5440	equ	10001000b	; 101

fID_GD5430	equ	00001000b

vID_ML_BUS	equ	00010000b	; 01
vID_C_BUS	equ	00100000b	; 10
vID_MLNf_BUS	equ	00110000b	; 11

; Cirrus
vID_B3		equ	(vID_GD5428 + vID_C_BUS  + 000b)	; PC-9801-96

vID_Cx2		equ	(vID_GD5430 + vID_ML_BUS + 101b)	; PC-9821Cx2
vID_Cb2		equ	(vID_GD5430 + vID_ML_BUS + 100b)	; PC-9821Cb2
vID_Xe10	equ	(vID_GD5430 + vID_ML_BUS + 011b)	; PC-9821Xe10
vID_Cf_Cx	equ	(vID_GD5430 + vID_ML_BUS + 010b)	; PC-9821Cx/Cf
vID_Cb		equ	(vID_GD5430 + vID_ML_BUS + 001b)	; PC-9821Cb
vID_Xe		equ	(vID_GD5430 + vID_ML_BUS + 000b)	; PC-9821Xe
vID_Nf		equ	(vID_GD5428 + vID_MLNf_BUS + 000b)	; PC-9821Nf
vID_Bf_SV98	equ	(vID_GD5428 + vID_ML_BUS + 010b)	; PC-9821Bf/SV98model1/2
vID_Np_Es	equ	(vID_GD5428 + vID_ML_BUS + 101b)	; PC-9821Np/Es
vID_Ns		equ	(vID_GD5428 + vID_ML_BUS + 011b)	; PC-9821Ns
vID_Ne2		equ	(vID_GD5428 + vID_ML_BUS + 110b)	; PC-9821Ne2
vID_Nd		equ	(vID_GD5428 + vID_ML_BUS + 111b)	; PC-9821Nd
vID_Cs2		equ	(vID_GD5428 + vID_ML_BUS + 001b)	; PC-9821Cs2
vID_Ts		equ	(vID_GD5428 + vID_ML_BUS + 100b)	; PC-9821Ts
vID_Bp_Bs_Be	equ	(vID_GD5428 + vID_ML_BUS + 000b)	; PC-9821Bp/Bs/Be

; MGA-II
vID_A2		equ	(vID_MGA2 + vID_ML_BUS + 000b)		; PC-9821A-E11

; S3
vID_A		equ	(vID_S3_928 + vID_ML_BUS + 001b)	; PC-9821A-E09
vID_B		equ	(vID_S3_928 + vID_C_BUS  + 001b)	; PC-9801-91
vID_FA		equ	(vID_S3_928 + vID_ML_BUS + 000b)	; PC-9821A-E01
vID_FB		equ	(vID_S3_928 + vID_C_BUS  + 000b)	; PC-9801-85
vID_864		equ	(vID_S3_864 + vID_ML_BUS + 000b)	; PC-9821Xs/Xp/Xn
vID_928		equ	(vID_S3_928 + vID_ML_BUS + 010b)	; PC-9821As2/Ap2

; SessionMode
vSM_400_256	equ	10h		;640x400 256
vSM_480_256	equ	5Fh		;640x480 256
vSM_600_256	equ	5Ch		;800x600 256
vSM_768_256	equ	60h		;1024x768 256
vSM_400_16M	equ	11h		;640x400 16M
vSM_600_64K	equ	65h		;800x600 64K(5:6:5)
vSM_480_16M	equ	71h		;640x480 16M
vSM_480_64K	equ	64h		;640x480 64K(5:6:5)

; VESA video mode
vVM_400_256	equ	0100h
vVM_480_256	equ	0101h
vVM_600_16	equ	0102h
vVM_600_256	equ	0103h
vVM_768_16	equ	0104h
vVM_768_256	equ	0105h
vVM_1024_16	equ	0106h
vVM_1024_256	equ	0107h

vVM_80x60	equ	0108h
vVM_132x25	equ	0109h
vVM_132x43	equ	010Ah
vVM_132x50	equ	010Bh
vVM_132x60	equ	010Ch

vVM_200_32K	equ	010Dh
vVM_200_64K	equ	010Eh
vVM_200_16M	equ	010Fh
vVM_480_32K	equ	0110h
vVM_480_64K	equ	0111h
vVM_480_16M	equ	0112h
vVM_600_32K	equ	0113h
vVM_600_64K	equ	0114h
vVM_600_16M	equ	0115h
vVM_768_32K	equ	0116h
vVM_768_64K	equ	0117h
vVM_768_16M	equ	0118h
vVM_1024_32K	equ	0119h
vVM_1024_64K	equ	011Ah
vVM_1024_16M	equ	011Bh

HWCursorOn	equ	1
HWCursorOff	equ	0

;******************************************************************************
;                       D A T A   S T R U C T U R E S
;******************************************************************************

VACL_CB_Struct struc
	ABE_Flags		dd	?
	ABE_WinfuncPtr		dd	?
	ABE_Video_Mode		dw	?
	ABE_Window_Pos		dw	?

	ABE_BytesPerScanline	dd	?
	ABE_DisplayStartX	dw	?
	ABE_DisplayStartY	dw	?

	ABE_Cursor_X		dw	?
	ABE_Cursor_Y		dw	?

	ABE_Palette		dd	256 dup (?)

	ABE_SessionMode		dw	?

	ABE_Cursor_Shape	db	((ABE_CursorSizeX/8)*ABE_CursorSizeY)*2 dup (0)
	ABE_Cursor_Enable	db	HWCursorOff
VACL_CB_Struct ends

