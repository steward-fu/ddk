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
; PALETTE.ASM
;----------------------------------------------------------------------------
	.xlist
DOS5 = 1
	include cmacros.inc
	include macros.inc
	include dibeng.inc
	include gdc.inc
	.list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
PAL_INDEX	equ	0a8h	;Palette index
PAL_G_INTENSITY	equ	0aah	;	 green
PAL_R_INTENSITY	equ	0ach	;	 red
PAL_B_INTENSITY	equ	0aeh	;	 blue

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP DIB_SetPaletteExt

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externD lpColorTable
	externW wBpp
	externD lpDriverPDevice
	externW wDeviceType
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
	.386

;---------------------------------------------------------------------------
; SetRAMDAC
;   Initialize the vga's DACs to the values stored in a color table
;   composed of triples.
; Entry:
;	ax  - index to 1st palette entry
;	cx  - count of indices to program
;    ds:si --> array of RGBs
;---------------------------------------------------------------------------
PPROC	SetRAMDAC near
	movzx	ebx,ax
	movzx	esi,si
PLABEL SR_DACLoop
	mov	al,bl
	out	PAL_INDEX,al
	GDC_WAIT_HSYNC1

	mov	al,[esi+ebx*4+2]	;Red
	out	PAL_R_INTENSITY,al
	GDC_WAIT_HSYNC1

	mov	al,[esi+ebx*4+1]	;Green
	out	PAL_G_INTENSITY,al
	GDC_WAIT_HSYNC1

	mov	al,[esi+ebx*4]		;Blue
	out	PAL_B_INTENSITY,al
	GDC_WAIT_HSYNC1

	inc	ebx
	dec	cx
	jnz	SR_DACLoop
	ret

SetRAMDAC  endp

PPROC	SetRAMDAC_far	far
	call	SetRAMDAC
	ret
SetRAMDAC_far	endp

;---------------------------------------------------------------------------
; SetPalette
;  Set the hardware palette from GDI.  This palette is also copied into
;  the screen DIB Color table.
; Entry:
;	start	    - first palette index to set
;	count	    - count of palette indices
;	lpPalette   -->array of RGBQUADs containing colors
;---------------------------------------------------------------------------
cProc	SetPalette,<FAR,PUBLIC,PASCAL,NODATA>,<esi,di>
	parmW	wStartIndex		;starting index in the Palette
	parmW	wNumEntries		;no of indexes in the Palette
	parmD	lpPalette		;long pointer to color Palette
cBegin
	mov	ax,DGROUP
	mov	ds,ax
	assumes ds,Data
	assumes es,nothing
	assumes fs,nothing
	assumes gs,nothing
	push	wStartIndex		;Let the DIB Engine move the
	push	wNumEntries		; palette into the color table.
	push	lpPalette
	push	lpDriverPDevice
	call	DIB_SetPaletteExt
	les	si,lpDriverPDevice
	test	es:[si].deFlags,BUSY	;Is the screen busy?
	jnz	SP_Exit			;Yes, can't touch the hardware.
	lds	si,lpColorTable		;ds:si-->device color table
	mov	ax,wStartIndex
	mov	cx,wNumEntries
	call	SetRAMDAC		;Set new colors into the h/w palette.

PLABEL SP_Exit
	xor	ax,ax			;show success
	cwd
cEnd
sEnd
end
