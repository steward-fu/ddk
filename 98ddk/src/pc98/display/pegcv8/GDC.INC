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

;----------------------------------------------------------------------------
; GDC.INC
;----------------------------------------------------------------------------
;*
;*	GDC I/O port address
;*
;*	slave (graph) GDC
;*
GDC_S_STA	equ	0a0h	; status check
GDC_S_CMD	equ	0a2h	; command write
GDC_S_PARA	equ	0a0h	; parameter write
;*
;*      master (text) GDC
;*
GDC_M_STA	equ	60h	; status check
GDC_M_CMD	equ	62h	; command write
GDC_M_PARA	equ	60h	; parameter write
GDC_M_DATA	equ	62h	; data write

DTCLK_REG	equ	09a8h		; dot clock frequency register

;*
;*	GDC command
;*
GDC_RESET	equ	00h		; gdc reset command
GDC_RESET2	equ	01h
GDC_RESET3	equ	09h
GDC_START	equ	0dh		; gdc start command
GDC_STOP	equ	05h		; gdc stop command (for h-mode)
GDC_STOP1	equ	0ch
GDC_SYNC_IDCT	equ	0fh		; gdc sync command (enable display)
GDC_SYNC_NIDC	equ	0eh		; gdc sync command (disable display)
GDC_WRITE	equ	20h		; gdc write command
GDC_ZOOM	equ	46h		; gdc zoom command
GDC_PITCH	equ	47h		; gdc pitch command
GDC_CSRW	equ	49h		; gdc csrw command
GDC_VECTW	equ	4ch		; gdc vectw command
GDC_CSFORM	equ	4bh		; gdc csrform command
GDC_TEXTE	equ	68h		; gdc texte command
GDC_VECTE	equ	6ch		; gdc vecte command
GDC_SLAVE	equ	6eh		; gdc slave command
GDC_MASTER	equ	6fh		; gdc master command
GDC_SCROLL	equ	70h		; gdc scroll command
GDC_TEXTW	equ	78h		; gdc textw command
GDC_LPEN	equ	0c0h		; gdc lpen command
;*
;*	GDC WRITE command param
;*
GR_M_REPLACE	  equ	    00h
GR_M_COMPLEMENT	  equ	    01h
GR_M_CLEAR	  equ	    02h
GR_M_SET	  equ	    03h
;*
;*	GDC status flags
;*
GDC_DREADY		equ	01h
GDC_NOT_FULL		equ	02h
GDC_EMPTY		equ	04h
GDC_DRAWING		equ	08h
GDC_EMPTY_NDRAW		equ	0ch
GDC_M_NFULL_VSYNC	equ	20h
GDC_NFULL_VSYNC		equ	22h
GDC_EMPTY_VSYNC		equ	24h
GDF_HSYNC		equ	40h
GDC_D_BANK		equ	0a4h
GDC_A_BANK		equ	0a6h
GDC_1LINE		equ	40


;------------------------------------------------
;	GDC SETUP MACRO definition
;------------------------------------------------
;--------------------------------------
;	wait FIFO is not full (master GDC)
GDC_WAIT_M_FIFO_NOT_FULL	MACRO
		LOCAL	retry
retry:
		in	al,GDC_M_STA
		IO_DELAY
		test	al,GDC_NOT_FULL
		jnz	retry
		ENDM

;--------------------------------------
;	wait FIFO is not full (slave GDC)
GDC_WAIT_S_FIFO_NOT_FULL	MACRO
		LOCAL	retry
retry:
		in	al,GDC_S_STA
		IO_DELAY
		test	al,GDC_NOT_FULL
		jnz	retry
		ENDM

;--------------------------------------
;	wait FIFO is empty
GDC_WAIT_S_FIFO_EMPTY	MACRO
		LOCAL	retry
retry:
		in	al,GDC_S_STA
		IO_DELAY
		test	al,GDC_EMPTY
		jz	retry
		ENDM

;--------------------------------------
;	wait VSYNC is down
GDC_WAIT_VSYNC0	MACRO
		LOCAL	retry0,retry1
retry0:					; loop while VSYNC=0
		in	al,GDC_M_STA
		IO_DELAY
		test	al,GDC_M_NFULL_VSYNC
		jz	retry0
retry1:					; loop while VSYNC=1
		in	al,GDC_M_STA
		IO_DELAY
		test	al,GDC_M_NFULL_VSYNC
		jnz	retry1
		ENDM

;--------------------------------------
;	wait VSYNC is up
GDC_WAIT_VSYNC1	MACRO
		LOCAL	retry0,retry1
retry1:					; loop while VSYNC=1
		IO_DELAY
		in	al,GDC_S_STA
		test	al,GDC_M_NFULL_VSYNC
		jnz	retry1
retry0:					; loop while VSYNC=0
		sti
		IO_DELAY
		cli
		in	al,GDC_S_STA
		test	al,GDC_M_NFULL_VSYNC
		jz	retry0
		ENDM

;--------------------------------------
;	issue GDC parameter (master GDC)
GDC_M_PARAM	MACRO	arg1
		IFDIF	<arg1>,<al>
		mov	al,arg1
		ENDIF
		out	GDC_M_PARA,al
		IO_DELAY
		ENDM

;--------------------------------------
;	issue GDC parameter (slave GDC)
GDC_S_PARAM	MACRO	arg1
		IFDIF	<arg1>,<al>
		mov	al,arg1
		ENDIF
		out	GDC_S_PARA,al
		IO_DELAY
		ENDM

;--------------------------------------
;	issue GDC command (master GDC)
GDC_M_COMMAND	MACRO	cmd
		IFDIF	<cmd>,<al>
		mov	al,cmd
		ENDIF
		out	GDC_M_CMD,al
		IO_DELAY
		ENDM

;--------------------------------------
;	issue GDC command (slave GDC)
GDC_S_COMMAND	MACRO	cmd
		IFDIF	<cmd>,<al>
		mov	al,cmd
		ENDIF
		out	GDC_S_CMD,al
		IO_DELAY
		ENDM

;--------------------------------------
;	wait HSYNC is up
GDC_WAIT_HSYNC1	MACRO
		LOCAL	retry0,retry1
retry1:					; loop while HSYNC=1
		in	al,GDC_S_STA
		test	al,GDF_HSYNC
		jnz	retry1
retry0:					; loop while HSYNC=0
		in	al,GDC_S_STA
		test	al,GDF_HSYNC
		jz	retry0
		ENDM

;------------------------------------------------
;	I/O release time MACRO definition
;------------------------------------------------
IO_DELAY	MACRO
		rept	3
		out	5fh,al
		endm
		ENDM

