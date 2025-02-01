;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; SSB.ASM
; Copyright (c) 1992 Microsoft Corporation
;----------------------------------------------------------------------------
        .xlist
DOS5 = 1
        include cmacros.inc
        include dibeng.inc
        include gdidefs.inc
	include macros.inc
	include	S3.inc	; S3 specific defines
        .list

;----------------------------------------------------------------------------
; S T R U C T U R E S
;----------------------------------------------------------------------------
SCREENRGN	struc
	sr_rect		db size(RECT) dup (?)
	sr_width	dw		   ?
	sr_height	dw		   ?
SCREENRGN ends

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP	BeginAccess
	externFP	EndAccess

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
SSB_SAVE	equ	0
SSB_RESTORE	equ	1
SSB_FORGET	equ	2

SSB_AVAILABLE	equ	0
SSB_INUSE	equ	1
SSB_TRASHED	equ	2

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
globalB bSSBStatus,SSB_AVAILABLE
externB Rgn1
externB Rgn2
externW wMaxHeight
externW wMaxWidth
externW wMMIOSelector
externW wCursorType
externD lpDriverPDevice
externD dDestFmt                ;bpp
externB BDDrawActive

ALIGN 2
SaveRect RECT <>
SaveX      dw ?
SaveY      dw ?
SaveWidth  dw ?
SaveHeight dw ?

sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
	.386

cProc   SaveScreenBitmap,<FAR,PUBLIC,PASCAL,NODATA>,<si,di>
	parmD	lpRect			;--> rectangle to operate on
	parmW	wCmd			;0 = save, 1 = restore, 2 = forget
	localW	DeviceFlags
cBegin
	mov	ax,DGROUP
	mov	ds,ax
	assumes ds,Data
	assumes es,nothing
	assumes fs,nothing
	assumes gs,nothing

	cmp	BDDrawActive,0		;make sure offscreen area is NOT
	jne	SSB_Disabled		;being used by DirectDraw

	les	si,lpDriverPDevice
        bts     es:[si].deFlags,BUSY_BIT;lock the screen
        jc      SSB_Disabled            ;
	movzx	ebx,wCmd
	jmp	SSBAction[ebx*2]

public SSBAction
SSBAction	label	word
	dw	SSB_Save
	dw	SSB_Restore
	dw	SSB_Forget

;----------------------------------------------------------------------------
; SSB_Save
;   Copy the screen area to the off-screen save buffer.
;----------------------------------------------------------------------------
PLABEL SSB_Save
	cmp	bSSBStatus,SSB_AVAILABLE
	jne	SSB_Fail
	les	si,lpRect
	xor	ebx,ebx
	mov	ax,es:[si].left
	mov	bx,es:[si].right
	mov	di,bx
	mov	cx,es:[si].top
	mov	dx,es:[si].bottom
	mov	si,dx

	sub	di,ax			;di = width of save area
	sub	si,cx			;si = height of save area
	mov	SaveRect.right,bx
	mov	SaveRect.top,cx

PLABEL SSB_CheckRgn1
	cmp	di,Rgn1.sr_width
	jg	short SSB_CheckRgn2
	cmp	si,Rgn1.sr_height
	jg	short SSB_CheckRgn2
	mov	bx,Rgn1.left
	mov	cx,Rgn1.top
	jmp	short SSB_CopyToOffScreen

PLABEL SSB_CheckRgn2
	cmp	di,Rgn2.sr_width
	jg	SSB_Fail
	cmp	si,Rgn2.sr_height
	jg	SSB_Fail
	mov	bx,Rgn2.left
	mov	cx,Rgn2.top

PLABEL SSB_CopyToOffScreen
	mov	SaveWidth,di
	mov	SaveHeight,si
	mov	SaveRect.left,ax
	mov	SaveRect.bottom,dx
	mov	SaveX,bx
	mov	SaveY,cx
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f
	push	ebx
	push	lpDriverPDevice
	push	SaveRect.left
	push	SaveRect.top
	push	SaveRect.right
	push	SaveRect.bottom
	push	CURSOREXCLUDE
	call	BeginAccess		;exclude cursor
	pop	ebx
@@:	mov	cx,SaveY
	call	CopySaveRect
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f
	push	lpDriverPDevice
	push	CURSOREXCLUDE		;unexclude cursor
	call	EndAccess
@@:	mov	bSSBStatus,SSB_INUSE
	jmp	short SSB_Success

;----------------------------------------------------------------------------
; SSB_Forget
;   Forget the image in the off-screen save bitmap buffer.
;----------------------------------------------------------------------------
PLABEL SSB_Forget
	mov	bSSBStatus,SSB_AVAILABLE
	jmp	short SSB_Success

;----------------------------------------------------------------------------
; SSB_Restore
;   Copy the off-screen save buffer to the screen.
;----------------------------------------------------------------------------
PLABEL SSB_Restore
	cmp	bSSBStatus,SSB_INUSE
	jne	short SSB_Fail
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f
	push	lpDriverPDevice
	push	SaveRect.left
	push	SaveRect.top
	push	SaveRect.right
	push	SaveRect.bottom
	push	CURSOREXCLUDE
	call	BeginAccess		;exclude cursor
@@:	xor	ebx,ebx			;set direction for copy.
	dec	ebx
	mov	bx,SaveX
	mov	cx,SaveY
	mov	si,SaveHeight
	mov	di,SaveWidth
	call	CopySaveRect
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f
	push	lpDriverPDevice
	push	CURSOREXCLUDE		;unexclude cursor.
	call	EndAccess
@@:	mov	bSSBStatus,SSB_AVAILABLE

PLABEL SSB_Success
	les	si,lpDriverPDevice
        and     es:[si].deFlags,not BUSY;
	mov	ax,1
PLABEL SSB_Exit
cEnd

PLABEL SSB_Disabled
	mov	bSSBStatus,SSB_AVAILABLE
	xor	ax,ax
	jmp	SSB_Exit

PLABEL SSB_Fail
	les	si,lpDriverPDevice
        and     es:[si].deFlags,not BUSY;
	xor	ax,ax
	jmp	SSB_Exit


;----------------------------------------------------------------------------
; CopySaveRect
;   Copies SaveRect to/from onscreen from/to offscreen.
; Entry:
;  bx,cx = offscreen x,y coordinates
;  msb of ebx: 0 = copy on-screen to off-screen
;              1 = copy off-screen to on-screen
;  si,di = height,width of rectangle
;  SaveRect = on-screen rectangle
;----------------------------------------------------------------------------
PPROC	CopySaveRect	near

        push    es
        mov     ax,wMMIOSelector
        mov     es,ax

        CheckFIFOSpace SIX_SLOTS es
                                        ;left = 0
	mov	ax,wMaxWidth		;set right
	dec	ax			;
        EngineWrite B_CLIP_L_R, es, eax
                                        ;top = 0
	mov	ax,wMaxHeight		;set bottom
	dec	ax			;
        EngineWrite B_CLIP_T_B, es, eax ;set Y-axis clipping

 	mov	ax,di                   ;get width
	dec	ax                      ;width--
        rol     eax,16                  ;in high dword
	mov	ax,si
	dec	ax
	EngineWrite B_RWIDTH_HEIGHT, es, eax

        mov     esi,B_RSRC_XY
        mov     edi,B_RDEST_XY

	mov	ax,SaveRect.left        ;
        rol     eax,16                  ;
	mov	ax,SaveRect.top         ;eax = coord(on-screen)

        mov     dx,bx                   ;get offscreen-x
        rol     edx,16                  ;in high dword
        mov     dx,cx                   ;get offscreen-y in low dword

        or      ebx,ebx                 ;copy from on-screen to off-screen
        jns     short @f                ;yes, go do it
        xchg    esi,edi                 ;switch the coordinates
@@:
	EngineWrite esi, es, eax        ;set on-screen
	EngineWrite edi, es, edx        ;set off-screen

        mov     eax,dDestFmt+BITBLT
        EngineWrite B_CMD_SET, es, eax  ;do screen to screen blt

        pop     es
	ret

CopySaveRect	endp

sEnd	Code
	end
