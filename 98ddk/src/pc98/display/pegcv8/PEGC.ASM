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
; PEGC.ASM
;----------------------------------------------------------------------------
	.xlist
	include cmacros.inc
	include macros.inc
	include device.inc
	include gdc.inc
	.list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
_0040		equ	40h
BIOS_FLAG8	equ	5Ch
PACKED_FLAG	equ	40h
ROM		equ	0F8E8h
PC9821		equ	2198h
PEGC_FLAG	equ	80h

;----------------------------------------------------------------------------
; E X T E R N S	 and  P U B L I C S
;----------------------------------------------------------------------------
	externW ScreenWindowSelector0	;in VGA.ASM
	externW ScreenWindowSelector1	;in VGA.ASM
	externW ScreenSelector		;in VGA.ASM
	externW IOSelector		;in VGA.ASM

	externFP AllocSelector		;Selector Allocate
	externFP SetSelectorBase	;Change Selector Base
	externFP SetSelectorLimit	;Change Selector Limit

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
globalB	sync_mode,0
sEnd	Data

;----------------------------------------------------------------------------
; Memory Mapped I/O
;----------------------------------------------------------------------------
IOSeg	segment at 0
		dd	?
PEGC_BANK0	dw	?
PEGC_BANK1	dw	?
		db	0f8h dup (?)
PEGC_PACKED	dw	?
PEGC_PENABLE	dw	?
PEGC_ACTPLANE	dw	?
		dw	?
PEGC_MODE1	dw	?
PEGC_MODE2	dw	?
PEGC_MASK	dw	?
		dw	?
PEGC_PLENGTH	dw	?
PEGC_BITAD	dw	?
PEGC_FGC	dw	?
		dw	?
PEGC_BGC	dw	?
		db	6 dup (?)
PEGC_PATTERN	dd	16 dup (?)
IOSeg ends

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg

;----------------------------------------------------------------------------
; Is_PEGC
; Entry:
;   none
; Exit:
;   ax:dx == !0 (true) if detected.
; Setting:
;   none
; Used:
;   flags, ax, dx, si, es
;----------------------------------------------------------------------------
cProc Is_PEGC,<NEAR,PUBLIC,PASCAL,NODATA>,<si>
cBegin
PLABEL ISPEGC_Success
	xor	si,si

;----- Detection here! -----

	mov	si,PEGC		;Show success.

PLABEL ISPEGC_Not
	mov	ax,si		;Error is 0.
	xor	dx,dx
cEnd

;----------------------------------------------------------------------------
; SM_PEGC
;   Set graphics mode.
; Entry:
;   bx = mode number (21h:640x400 25line or 31h:640x480 25line)
; Exit:
;   CARRY if mode cannot be set.
; Setting:
;   System common areas.
; Used:
;   flags ax, bx, cx, es
;----------------------------------------------------------------------------
PPROC SM_PEGC near
	mov	ax,DGROUP
	mov	ds,ax
	assumes ds,Data

	push	bx
	mov	ah,31h				;Ask GDC
	int	18h
	mov	sync_mode,al			;Save 24/31KHz
	pop	bx

	mov	ah,42h
	mov	ch,0C0h				;640x400
	int	18h				;Setting system common areas.

	mov	bh,bl

	mov	al,00001100b			;31KHz
	cmp	bh,31h				;Screen is 640x480 25line
	je	@f				;Yes

	mov	bh,21h				;640x400 25line

; Graph BIOS emulate
	push	ax
	mov	ax,40h
	mov	es,ax
	mov	cl,bptr es:[197h]
	and	cl,00000011b
	cmp	cl,00000010b			;Screen is 400line mode
	jne	ExitMODCHG

;Set 256color
	mov	al,07h
	out	6Ah,al				;Enable Mode F/F
	mov	al,21h
	out	6Ah,al				;256color
	mov	al,06h
	out	6Ah,al				;Disable Mode F/F
	mov	al,41h
	out	6Ah,al				;Be accorded with Text and Graph

	or	bptr es:[14Dh],84h		;256color, 5MHz

PLABEL ExitMODCHG
	test	bptr es:[14Ch],00100000b	;Bit5 check 31KHz
	pop	ax

	jne	@f
	mov	al,00001000b			;24KHz

@@:	mov	ah,30h
	int	18h

;Set Mode F/F
	mov	al,07h				;Mode F/F(8)
	out	6Ah,al				;Enable Mode F/F
	mov	al,69h				;Mode F/F(13)
	out	6Ah,al				;VRAM 1 section
	mov	al,06h				;Mode F/F(8)
	out	6Ah,al				;Disable Mode F/F
	mov	al,41h				;Mode F/F(5)
	out	6Ah,al				;Be accorded with Text and Graph

;Set pitch
@@:	in	al,0A0h				;GDC status check
	IO_DELAY
	test	al,04h
	jz	@b

	mov	al,47h				;GDC pitch command
	out	0A2h,al				;GDC command write
	IO_DELAY

	mov	al,80h				;1024pixel
	out	0A0h,al				;GDC parameter write
	IO_DELAY

;Start GGDC
	mov	ah,40h
	int	18h

	clc
	ret

PLABEL SM_PEGC_Not
	stc
	ret
SM_PEGC endp

;----------------------------------------------------------------------------
; SM_ExtraInit_PEGC
;   Get Memory mapped I/O selector and set packed pixel mode.
; Entry:
;   bx = screen width in bytes
; Exit:
;   none
; Setting:
;   IOSelector
; Used:
;   flags, ax, bx, es
;----------------------------------------------------------------------------
PPROC SM_ExtraInit_PEGC near
	mov	ax,0002h
	mov	bx,0E000h			;MMI/O
	int	31h
	mov	IOSelector,ax

	mov	ax,IOSelector
	mov	es,ax

	mov	wptr es:PEGC_PACKED,0000h	;Select packed pixel mode
	mov	wptr es:PEGC_PENABLE,0001h	;ACSESS 15M

	ret
SM_ExtraInit_PEGC	endp

sEnd	InitSeg
end
