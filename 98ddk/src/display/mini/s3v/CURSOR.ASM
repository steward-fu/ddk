;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; CURSOR.ASM
; Copyright (c) 1994 Microsoft Corporation
;----------------------------------------------------------------------------
	.xlist
DOS5=1
	include cmacros.inc
	include dibeng.inc
	include s3.inc
	.list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
SOFTWARECURSOR	equ	0
HARDWARECURSOR	equ	1

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP DIB_SetCursorExt
	externFP DIB_CheckCursorExt
	externFP DIB_MoveCursorExt
	externFP DIB_Inquire
	externFP DisplaySwitchHandler
	externNP MoveHWCursor
	externNP CheckHWCursor
	externNP SaveCursorBitmap

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externW wCursorType
	externW wEnabled
	externD lpDriverPDevice
	externW wBpp
	externW wPDeviceFlags
	externB BDDrawActive
	externW MoveHWCursorFn
	externB S3MobileData
	externB bOutputType			;in MOBILE.ASM
	externB bTVParms_initialized
	externB bStreamProcessorFlags		;in VGA.ASM
	externW hWnd
	externW	wMMIOSelector			;in VGA.ASM
	externD PostMessage

align 4
	globalD MonoCursorFn,DIB_SetCursorExt
	globalD ColorCursorFn,DIB_SetCursorExt
	globalD SetCursorFn,DIB_SetCursorExt
	globalW CursorFmt,0101h
	globalW MonoCursorType,SOFTWARECURSOR
	globalW ColorCursorType,SOFTWARECURSOR
	globalW wAbsY,0
	globalW wAbsX,0
	globalW CursorFgColor,0
	globalW CursorBgColor,0
	globalB bRestoreCursorColor,0
	errnz	wAbsX-wAbsY-2
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
assumes cs,Code
	.386

;--------------------------------------------------------------------------
; SetCursor
; Possible transitions:
;   mono cursor --> mono cursor
;   mono cursor --> null
;   null	--> mono cursor
;   null	--> null
;
;   color cursor --> color cursor
;   color cursor --> null
;   null	 --> color cursor
;   null	 --> null
;
;   color cursor --> mono cursor
;   mono cursor --> color cursor
;--------------------------------------------------------------------------
cProc	SetCursor, <FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpCursorShape
cBegin
	mov	ax,DGROUP
	mov	es,ax
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	cmp	wEnabled,0			;If display is disabled, skip.
	je	SC_Exit
	lds	si,lpDriverPDevice
	test	[si].deFlags,BUSY
	jnz	SC_Exit
	cmp	word ptr lpCursorShape+2,0	;New cursor null?
	je	SC_CallSetFn			;yes, let current code handle it.
	lds	si,lpCursorShape		;ds:[si]-->Cursor shape
	mov	si,[si+10]			;si = cursor format
	cmp	si,[CursorFmt]			;new fmt = old fmt?
	je	SC_CallSetFn			;yes, call current code.
	mov	[CursorFmt],si			;no, update to new fmt.
	push	es
	pushd	0				;turn off old cursor.
	push	lpDriverPDevice
	call	[SetCursorFn]
	pop	es
	cmp	ax,0				; Success?
	jz	SC_Exit 			; No!
	mov	[CursorFmt],si			;no, update to new fmt.
	mov	ax,[MonoCursorType]
	mov	ebx,[MonoCursorFn]		;assume change over to mono cursor.
	cmp	si,0101h			;mono cursor?
	je	short @f			;yes, go set it.
	mov	ax,[ColorCursorType]
	mov	ebx,[ColorCursorFn]		;no, we'll use the color cursor.
@@:	mov	[SetCursorFn],ebx		;update cursor fn variable.
	mov	[wCursorType],ax
	push	es
	mov	eax,dword ptr wAbsY		;make sure the new cursor
	push	eax				; is in the right position before
	call	MoveCursor			; making it visible.
	pop	es

SC_CallSetFn:
	test	es:S3MobileData.wSPStyle,SP_M5
	jz	short SC_CallSetCFn
	cCall	SaveCursorBitmap
;
SC_CallSetCFn:
	push	lpCursorShape			;call the set cursor function.
	push	lpDriverPDevice
	call	[SetCursorFn]

SC_Exit:					;done.
cEnd


;--------------------------------------------------------------------------
; CheckCursor
;--------------------------------------------------------------------------
cProc	CheckCursor,<FAR,PUBLIC,PASCAL,NODATA>
cBegin <nogen>
	mov	ax,DGROUP
	mov	es,ax
	mov	ds,ax
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	cmp	wEnabled,0
	jne	short CC_CheckDisplayMode
	retf
;
public CC_CheckDisplayMode
CC_CheckDisplayMode:
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f
	pop	ecx
	push	lpDriverPDevice
	push	ecx
	jmp	DIB_CheckCursorExt
@@:	jmp	CheckHWCursor
cEnd <nogen>



;--------------------------------------------------------------------------
; MoveCursor
;--------------------------------------------------------------------------
cProc	MoveCursor,<FAR,PUBLIC,PASCAL,NODATA>
	parmW	absX
	parmW	absY
cBegin <nogen>
	mov	ax,DGROUP
	mov	es,ax
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

;;
;; If MoveCursor is called when we are in Flip32 code, bail out immediately.
;;
        test    S3MobileData.dwMobileCaps,IN_FLIPPING
        jz      SHORT @F                ; Not in Flip32 code, continue
        retf    4                       ; Exit prematurely.
@@:
	cmp	wEnabled,0
	jne	short @f
	retf	4
@@:	mov	bx,sp
	mov	eax,ss:[bx+4]
	mov	dword ptr [wAbsY],eax
;
	test	S3MobileData.dwMobileCaps,PANNING_ENABLED
	jnz	short MC_CallMoveHWCursor
	cmp	wCursorType,SOFTWARECURSOR
	jne	short MC_CallMoveHWCursor
	pop	ecx
	push	lpDriverPDevice
	push	ecx
	jmp	DIB_MoveCursorExt
;
MC_CallMoveHWCursor:
	jmp	MoveHWCursorFn
cEnd <nogen>


;--------------------------------------------------------------------------
; Inquire
;--------------------------------------------------------------------------
cProc	Inquire,<PUBLIC,FAR,PASCAL,NODATA>
	parmD	lpCursorInfo
cBegin <nogen>
	jmp	DIB_Inquire
cEnd	<nogen>

;--------------------------------------------------------------------------
;  FixCursorColor
;--------------------------------------------------------------------------

CURSOR_FG_COLOR 	equ	04ah
CURSOR_BG_COLOR 	equ	04bh
DAC_READ		equ	03c7h
DAC_DATA		equ	03c9h

cProc	FixCursorColor,<PUBLIC,FAR,PASCAL,NODATA>
cBegin
.386
	mov	ax,DGROUP
	mov	es,ax
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	mov	bRestoreCursorColor,0
	cmp	wCursorType,SOFTWARECURSOR
	je	FCCDone
	mov	bRestoreCursorColor,1
;###
if	1
	mov	bx,0ff00h		;fg,bg color

	mov	dx,3d4h
	mov	al,045h 		;reset color stack reading CR45
	out	dx,al
	inc	dx
	in	al,dx
	dec	dx
	mov	al,04ah 		;set cursor foreground color
	out	dx,al
	inc	dx
	mov	al,bh
	out	dx,al
	out	dx,al
	out	dx,al
	dec	dx
	mov	al,045h 		;reset color stack reading CR45
	out	dx,al
	inc	dx
	in	al,dx
	dec	dx
	mov	al,04bh 		;set cursor background color
	out	dx,al
	inc	dx
	mov	al,bl
	sub	al,al
	out	dx,al
	out	dx,al
	out	dx,al
else
;
;Read contents of cr4a and cr4b to get cursor foreground and background colors
;
	mov	dx,03d4h
	mov	al,045h
	out	dx,al
	inc	dx
	in	al,dx				;reset cursor stack
	dec	dx
	mov	al,CURSOR_FG_COLOR
	out	dx,al
	inc	dx
	in	al,dx
	cmp	wBpp,16
	jne	short FCC8Bpp
	mov	bl,al
	out	dx,al				;increment stack pointer
	in	al,dx
	mov	bh,al				;bx = 16-bit foreground color
;
	mov	dx,03d4h
	mov	al,045h
	out	dx,al
	inc	dx
	in	al,dx				;reset cursor stack
	dec	dx
	mov	al,CURSOR_BG_COLOR
	out	dx,al
	inc	dx
	in	al,dx
	out	dx,al				;increment stack pointer
	mov	cl,al
	in	al,dx
	mov	ch,al				;cx = 16-bit background color

;bx	foreground color
;cx	background color
;
	mov	CursorFgColor,bx
	mov	CursorBgColor,cx
	test	wPDeviceFlags,FIVE6FIVE
	jz	short FCC15Bpp
	mov	ax,bx
	shl	ax,5				;ah = GGGGGG00b
	shl	bl,3				;bl = BBBBB000b
	and	bh,0f8h 			;bh = RRRRR000b
	mov	dx,cx
	shl	dx,5				;dh = GGGGGG00b
	shl	edx,16
	shl	cl,3				;cl = BBBBB000b
	and	ch,0f8h 			;ch = RRRRR000b
	jmp	short FCCSetCursorColor

;bx	foreground color
;cx	background color
;
FCC15Bpp:
	mov	ax,bx
	shl	ax,6				;ah = GGGGG000b
	shl	bl,3				;bl = BBBBB000b
	and	bh,0f8h 			;bh = RRRRR000b
	mov	dx,cx
	shl	dx,6				;dh = GGGGG000b
	shl	edx,16
	shl	cl,3				;cl = BBBBB000b
	and	ch,0f8h 			;ch = RRRRR000b
	jmp	short FCCSetCursorColor

;al	LUT index of foreground color
;
FCC8Bpp:
	mov	byte ptr CursorFgColor,al
	ror	eax,8
	mov	dx,03d4h
	mov	al,045h
	out	dx,al
	inc	dx
	in	al,dx				;reset cursor stack
	dec	dx
	mov	al,CURSOR_BG_COLOR
	out	dx,al
	inc	dx
	in	al,dx				;LUT index of background color
	mov	byte ptr CursorBgColor,al
	mov	dx,DAC_READ
	out	dx,al				;point to this index
	mov	dx,DAC_DATA
	in	al,dx
	shl	al,2				;red
	mov	ch,al
	in	al,dx
	shl	al,2
	mov	ah,al				;green
	in	al,dx
	shl	al,2
	mov	cl,al				;blue
	mov	dl,ah
	ror	edx,8				;green
;
	rol	eax,8				;LUT index of foreground color
	mov	dx,DAC_READ
	out	dx,al				;point to this index
	mov	dx,DAC_DATA
	in	al,dx
	shl	al,2				;red
	mov	bh,al
	in	al,dx
	shl	al,2
	mov	ah,al				;green
	in	al,dx
	shl	al,2
	mov	bl,al				;blue
;
;Set cursor foreground color register with 24-bit foreground color
;bl	blue
;ah	green
;bh	red
;
FCCSetCursorColor:
	mov	dx,03d4h
	mov	al,045h
	out	dx,al
	inc	dx
	in	al,dx				;reset cursor stack
	dec	dx
	mov	al,CURSOR_FG_COLOR
	out	dx,al
	inc	dx
	mov	al,bl				;blue
	out	dx,al
	mov	al,ah				;green
	out	dx,al
	mov	al,bh				;red
	out	dx,al
;
;Set cursor background color register with 24-bit background color
;ch	red
;cl	blue
;MSB of edx =  green
;
	mov	dx,03d4h
	mov	al,045h
	out	dx,al
	inc	dx
	in	al,dx				;reset cursor stack
	dec	dx
	mov	al,CURSOR_BG_COLOR
	out	dx,al
	inc	dx
	mov	al,cl				;blue
	out	dx,al
	mov	eax,edx
	shr	eax,24				;green
	out	dx,al
	mov	al,ch				;red
	out	dx,al
;
endif
FCCDone:
cEnd

;--------------------------------------------------------------------------
;  RestoreCursorColor
;--------------------------------------------------------------------------

cProc	RestoreCursorColor,<PUBLIC,FAR,PASCAL,NODATA>
cBegin
.386
	mov	ax,DGROUP
	mov	es,ax
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	cmp	bRestoreCursorColor,0
	je	RCCDone
;###
if	1
	mov	bx,0ff00h		;fg,bg color

	mov	dx,3d4h
	mov	al,045h 		;reset color stack reading CR45
	out	dx,al
	inc	dx
	in	al,dx
	dec	dx
	mov	al,04ah 		;set cursor foreground color
	out	dx,al
	inc	dx
	mov	al,bh
	out	dx,al
	out	dx,al
	out	dx,al
	dec	dx
	mov	al,045h 		;reset color stack reading CR45
	out	dx,al
	inc	dx
	in	al,dx
	dec	dx
	mov	al,04bh 		;set cursor background color
	out	dx,al
	inc	dx
	mov	al,bl
	sub	al,al
	out	dx,al
	out	dx,al
	out	dx,al
else
;
;Restore cursor color in K2 mode
;
	cmp	wBpp,32
	je	RCCDone
	mov	dx,03d4h
	mov	al,045h
	out	dx,al
	inc	dx
	in	al,dx				;reset cursor stack
	dec	dx
	mov	al,CURSOR_FG_COLOR
	out	dx,al
	inc	dx
	cmp	wBpp,16
	jne	short RCC8Bpp
	mov	ax,CursorFgColor
	out	dx,al
	shr	ax,8
	out	dx,al				;restore cursor foreground color
;
	mov	dx,03d4h
	mov	al,045h
	out	dx,al
	inc	dx
	in	al,dx				;reset cursor stack
	dec	dx
	mov	al,CURSOR_BG_COLOR
	out	dx,al
	inc	dx
	mov	ax,CursorBgColor
	out	dx,al
	shr	ax,8
	out	dx,al				;restore cursor foreground color
	jmp	short RCCDone
;
RCC8Bpp:
	mov	ax,CursorFgColor
	out	dx,al				;restore cursor foreground color
;
	mov	dx,03d4h
	mov	al,045h
	out	dx,al
	inc	dx
	in	al,dx				;reset cursor stack
	dec	dx
	mov	al,CURSOR_BG_COLOR
	out	dx,al
	inc	dx
	mov	ax,CursorBgColor
	out	dx,al				;restore cursor foreground color
;
endif
RCCDone:
.286c
cEnd
sEnd		Code
end
