;------------------------------------------------------------------------------
; Copyright (c) 1996 S3 Inc.
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
; STRBLT.ASM
; Copyright (c) 1992 Microsoft Corporation
;------------------------------------------------------------------------------
;
.xlist
include cmacros.inc
include dibeng.inc
include macros.inc
include S3.inc				; S3 specific defines
incFont = 1
incDrawMode = 1
include gdidefs.inc
.list
;------------------------------------------------------------------------------
; E X T E R N S
;------------------------------------------------------------------------------
;
externFP    DIB_ExtTextOut		; DIBENGINE - DIB Engine handler
externFP    DIB_ExtTextOutExt		; DIBENGINE - DIB Engine handler

;------------------------------------------------------------------------------
; D A T A
;------------------------------------------------------------------------------
;
sBegin	Data
;
externW     wCursorType 		; VGA.ASM - Cursor type (Hard/software)
externW     RowTextFn			; VGA.ASM - ExtTextOut RowText near ptr
externW     DrawTextBitmapFn		; VGA.ASM - ExtTextOut TextBmp near ptr
externW     DrawOrectFn 		; VGA.ASM - ExtTextOut OpaqueRect ptr
;
sEnd	Data

;------------------------------------------------------------------------------
; C O D E
;------------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
.386

;------------------------------------------------------------------------------
; PROCEDURE:	Strblt:
;
; DESCRIPTION:	String block transfer.	Pushes additional NULL parameters onto
;		the stack and drops into ExtTextOut.
;
; ASSUMPTIONS:	None
;
; CALLS:	None
;
; ENTRY  REGS:	None
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	AX, BX, CX, SP (adds stack params)
;
; NOTES:	Entry for backwards compatibility only; not commonly used.
;------------------------------------------------------------------------------
;
cProc	Strblt,<FAR,PUBLIC,PASCAL,WIN>

	parmD	lpDevice		; Destination device
	parmW	x			; Left origin of string
	parmW	y			; Top  origin of string
	parmD	lpClipRect		; Clipping rectangle
	parmD	lpString		; The string itself
	parmW	count			; Number of characters in the string
	parmD	lpFont			; Font to use
	parmD	lpDrawMode		; Drawmode structure to use
	parmD	lpXform 		; Current text transform

cBegin	<nogen>
	pop	cx			; CX = caller's return offset
	pop	bx			; BX = caller's return selector
	xor	ax,ax			; AX = NULL parameter value
	push	ax			; push NULL lpDx
	push	ax			;
	push	ax			; push NULL lpOpaqueRect
	push	ax			;
	push	ax			; push NULL options
	push	bx			; push BX = caller's return selector
	push	cx			; push CX = caller's return offset

cEnd	<nogen>

	ALIGN	16			; Align on CPU code prefetch boundary

	errn$	ExtTextOut		; (drop into standard ExtTextOut code)


;------------------------------------------------------------------------------
; PROCEDURE:	ExtTextOut:
;
; DESCRIPTION:	Extended text output.  This dispatch routine decides if the
;		text destination is the screen and routes it to the appropriate
;		text display routine.
;
; ASSUMPTIONS:	None
;
; CALLS:	[RowTextFn] = Text output function near ptr
;
; ENTRY  REGS:	SS:SP = Stack frame (return address and parameters)
;
; EXIT	 REGS:	AX = 0000h (formerly text's X extent)
;		DX = 0000h (formerly text's Y extent)
;
; MODIFY REGS:	EAX, EBX, ECX, EDX
;
; NOTES:	The RowTextMM2 function relies upon the following registers
;		loaded on entry:
;		    FS:BX = lpDevice destination device structure far ptr
;		      GS: = Driver data selector (DGROUP)
;------------------------------------------------------------------------------
;
cProc	ExtTextOut,<FAR,PUBLIC,PASCAL,NODATA>

	parmD	lpDevice		; Destination device
	parmW	x			; Left origin of string
	parmW	y			; Top  origin of string
	parmD	lpClipRect		; Clipping rectangle
	parmD	lpString		; The string itself
	parmW	count			; Number of characters in the string
	parmD	lpFont			; Font to use
	parmD	lpDrawMode		; Drawmode structure to use
	parmD	lpXform 		; Current text transform
	parmD	lpDx			; Widths for the characters
	parmD	lpOpaqueRect		; Opaquing rectangle
	parmW	etoOptions		; ExtTextOut options

cBegin <nogen>

	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	mov	ax,DGROUP		; AX = driver data selector
	mov	bx,sp			; SS:BX = alternate stack param ptr
	mov	gs,ax			; GS: = driver data selector

;
; Check the ExtTextOut params; only text rendering to the screen is supported
; NOTE: The RowTextMM2 function relies upon the following registers loaded:
;	FS:BX = lpDevice destination device structure far ptr
;	  GS: = Driver data selector (DGROUP)
;
	mov	cx,ss:[bx][26]		; CX = text string count input param
	mov	ax,ss:[bx][4]		; AX = etoOptions input param
	or	cx,cx			;is this an extent call
	jl      ETO_CallExtTextOut      ; yes, let the DIB Engine handle it

	lfs	bx,ss:[bx][40]		; FS:BX=lpDevice dest device struct ptr
	test	ax,ETO_LEVEL_MODE	; Special TextOutput ?
	jnz	ETO_CallExtTextOut      ; yes, give to DIB engine
	cmp	fs:[bx.deType],TYPE_DIBENG ;check for mono bitmap in case deFlags
					   ;is not valid
	jne	ETO_CallExtTextOut	; no, let DIB Engine handle it

	mov	dx,fs:[bx.deFlags]	; get flags
	test	dx,VRAM			; destination is the screen ?
	jz	ETO_CallExtTextOut 	; give to DIB engine

	test	dx,BUSY+PALETTE_XLAT	; engine busy or palette translate?
	jnz	ETO_CallExtTextOut	; yes, let the DIB Engine handle it
	test	ax,ETO_BYTE_PACKED	; new glyph format ?
	jz	short ETO_SetupForDTB	; no, call DrawTextBitmap
        test    dx,OFFSCREEN            ; LCB if the bitmap is in offscreen mem
        jnz     @f                      ; LCB skip CursorType check
	cmp	gs:[wCursorType],HARDWARECURSOR ; using a hardware cursor ?
	jne	short ETO_SetupForDTB	; no, let DrawTextBitmap handle it
@@:
	jmp	gs:[RowTextFn]		; yes, NOTE: FS:BX=lpDevice struct ptr

PLABEL ETO_CallExtTextOut
	jmp	DIB_ExtTextOut

	

PLABEL ETO_SetupForDTB
	mov	ax,cs			; AX = driver code selector
	pop	ebx			; EBX = caller's return address far ptr
	shl	eax,16			; EAX.H = driver code selector
	mov	ecx,eax 		; ECX.H = driver code selector
	mov	ax,gs:[DrawTextBitmapFn] ; EAX = DrawTextBitmap funct far ptr
	mov	cx,gs:[DrawOrectFn]	 ; ECX = DrawOrect	funct far ptr
	push	eax			; save DrawTextBitmap funct far ptr
	push	ecx			; save DrawOrect      funct far ptr
	push	ebx			; save caller's return address far ptr
	jmp	DIB_ExtTextOutExt	; goto DIB Engine code

cEnd	<nogen>
sEnd	Code
	end
