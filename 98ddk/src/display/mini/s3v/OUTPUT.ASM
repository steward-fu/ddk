;------------------------------------------------------------------------------
; OUTPUT.ASM
; Copyright (c) 1994 Microsoft Corporation
; Copyright (c) 1996 S3 Incorporated
;------------------------------------------------------------------------------
;
.xlist
;
DOS5 = 1				; Disable DOS4 odd stack patches
include cmacros.inc
include dibeng.inc
include macros.inc
include S3.inc
incOutput = 1
incLogical = 1
include gdidefs.inc

;------------------------------------------------------------------------------
; LOCAL INPUT PARAMETER EQUATES:
;
O_LPDSTDEV		equ	28	; lpDstDev    Dest PDEVICE struct ptr
O_STYLE 		equ	26	; style       Output style (OS_xxxxx)
O_COUNT 		equ	24	; count       Point count in array
O_LPPOINTS		equ	20	; lpPoints    Points array ptr
O_LPPHYSPEN		equ	16	; lpPhysPen   Physical pen ptr
O_LPPHYSBRUSH		equ	12	; lpPhysBrush Physical brush ptr
O_LPDRAWMODE		equ	 8	; lpDrawMode  DRAWMODE struct ptr
O_LPCLIPRECT		equ	 4	; lpClipRect  Clip rect ptr (if != NULL)
O_FARRETURNADDR 	equ	 0	; (Caller FAR return address)
;
.list

;------------------------------------------------------------------------------
; E X T E R N
;------------------------------------------------------------------------------
;
externFP    DIB_Output

;------------------------------------------------------------------------------
; D A T A
;------------------------------------------------------------------------------
;
sBegin	Data
;
externW pPolygonFn			; VGA.ASM: Polygon drawing funct ptr
externW pEllipseFn			; VGA.ASM: Ellipse drawing funct ptr
externW pRectangleFn			; VGA.ASM: Rectangle drawing funct ptr
externW pPolylineFn			; VGA.ASM: Polyline drawing funct ptr
externW pScanlineFn			; VGA.ASM: Scanline drawing funct ptr
externW pBeginScanFn			; VGA.ASM: BeginScan function ptr
externW pEndScanFn			; VGA.ASM: EndScan function ptr
;
sEnd	Data

;------------------------------------------------------------------------------
; C O D E
;------------------------------------------------------------------------------
;
createSeg _OUTPUT,OutputSeg,para,public,CODE
sBegin	OutputSeg
assumes cs,OutputSeg

.386

;------------------------------------------------------------------------------
; PROCEDURE:	Output:
;
; DESCRIPTION:	Output is the entry point for output functions such as lines,
;		scanlines, arcs, etc.  Those functions which are supported will be
;		dispatched to.	If the function is not supported, an error code will
;		be returned.
;
; ASSUMPTIONS:	None
;
; CALLS:	See specific function ptrs
;
; ENTRY  REGS:	SS:SP = Stack frame (return address and parameters)
;
; EXIT	 REGS:	See specific functions
;
; MODIFY REGS:	EAX, EBX, ECX, EDX
;
; NOTES:	None
;------------------------------------------------------------------------------
;
	ALIGN	16

cProc	Output,<FAR,PUBLIC,PASCAL,NODATA>

	parmD	lpDstDev		; SP+28: Destination PDEVICE struct ptr
	parmW	style			; SP+26: Output operation (OS_xxxxx)
	parmW	count			; SP+24: Point count in array
	parmD	lpPoints		; SP+20: Points array ptr
	parmD	lpPhysPen		; SP+16: Physical pen ptr
	parmD	lpPhysBrush		; SP+12: Physical brush ptr
	parmD	lpDrawMode		; SP+08: DRAWMODE struct ptr
	parmD	lpClipRect		; SP+04: Clip rectangle ptr (if != NULL)
					; SP+00: (Caller FAR return address)
cBegin	<nogen>

	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
;
; Jump to the specific Output function based on the style (OS_xxxx parameter)
;
	mov	bx,sp				; SS:BX = stack parameter ptr
	mov	ax,DGROUP			; AX = driver data selector
	mov	dx,ss:[bx + O_STYLE]	; DX = Output style operation
	lfs	bx,ss:[bx + O_LPDSTDEV] ; FS:BX = lpDstDev dest PDEVICE ptr
	mov	es,ax				; ES: = driver data selector
	cmp	fs:[bx.deType],TYPE_DIBENG ;Check if mono bitmap
	jne	O_LetDIBEngineDoIt	; no, let DIB Engine handle it
	mov	ax,fs:[bx.deFlags]	; AX = dest PDEVICE status flags

	test	ax,VRAM			; on-screen/offscreen mem destination ?
	jz	O_LetDIBEngineDoIt	; no, let DIB Engine handle it

	test	ax,PALETTE_XLAT 	; backgnd palette translation needed ?
	jnz	O_LetDIBEngineDoIt	; yes, let DIB Engine handle it

PLABEL O_Dispatch
	cmp	dx,OS_POLYLINE		; polyline ?
	jne	short @F		; no, continue
	jmp	[pPolylineFn]		; jump to polyline handler
@@:
	cmp	dx,OS_POLYGON		; polygon ?
	jne	short @F		; no, continue
	jmp	[pPolygonFn]		; jump to polygon handler
@@:
	cmp	dx,OS_RECTANGLE 	; rectangle ?
	jne	short @F		; no, continue
	jmp	[pRectangleFn]		; jump to rectangle handler
@@:
	cmp	dx,OS_ELLIPSE		; ellipse ?
	jne	short @F		; no, continue
	jmp	[pEllipseFn]		; jump to ellipse handler
@@:
	cmp	dx,OS_CIRCLE		; circle ?
	jne	short @F		; no, continue
	jmp	[pEllipseFn]		; jump to circle handler
@@:
	cmp	dx,OS_SCANLINES 	; scanline ?
	jne	short @F		; no, continue
	jmp	[pScanlineFn]		; jump to scanline handler
@@:
	cmp	dx,OS_POLYSCANLINE	; polyscanline ?
	jne	short @F		; no, continue
	jmp	[pScanlineFn]		; jump to scanline handler
@@:
	cmp	dx,OS_BEGINNSCAN	; BeginScan ?
	jne	short @F		; no, continue
	jmp	[pBeginScanFn]		; jump to BeginScan handler
@@:
	cmp	dx,OS_ENDNSCAN		; EndScan ?
	jne	short @F		; no, continue
	jmp	[pEndScanFn]		; jump to EndScan handler
@@:

PLABEL O_LetDIBEngineDoIt
	jmp	DIB_Output		; jump to DIB Engine Output handler



cEnd   <nogen>	; Output

;------------------------------------------------------------------------------
; Jump label for Output functions not handled by the individual procedures
; NOTE: If the driver is unable to handle the requested operation, this is
;	the place to jump to.  All registers saved in the stack frame will be
;	restored and the DIB Engine will be called to perform the action.
;
PLABEL O_DIBEngineToScreen
	pop	edi			; restore regs saved in our routines
	pop	esi			;
	lea	sp,[bp - 2]		; remove local variables off the stack
	pop	ds			;
	pop	bp			;
	jmp	DIB_Output		; jump to DIB Engine Output handler

;------------------------------------------------------------------------------
; Translate lookup table (equivalent of the 256 ROPS)
;
public S3Rop2TranslateTable
S3Rop2TranslateTable	label	byte

	db	000h			; 00 Black
	db	005h			; 01 DPon
	db	00Ah			; 02 DPna
	db	00Fh			; 03 Pn
	db	050h			; 04 PDna
	db	055h			; 05 Dn
	db	05Ah			; 06 DPx
	db	05Fh			; 07 DPan
	db	0A0h			; 08 DPa
	db	0A5h			; 09 DPxn
	db	0AAh			; 10 D
	db	0AFh			; 11 DPno
	db	0F0h			; 12 P
	db	0F5h			; 13 PDno
	db	0FAh			; 14 DPo
	db	0FFh			; 15 White

.ERRE	(((offset $) - (offset S3Rop2TranslateTable)) EQ 16) ; ERR: Length wrong
;
sEnd    OutputSeg
end
