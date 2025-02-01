;------------------------------------------------------------------------------
; POLYLINE.ASM
; Copyright (c) 1996 S3 Incorporated
;------------------------------------------------------------------------------
;
DOS5 = 1
.xlist
include 	cmacros.inc
include 	macros.inc
include 	dibeng.inc
incOutput	equ	1
incDrawMode	equ	1
incLogical	equ	1
include 	gdidefs.inc
include 	s3.inc
.list
;
;----------------------------------------------------------------------------
; E X T E R N
;----------------------------------------------------------------------------
		externFP BeginAccess
		externFP EndAccess
		externNP O_DIBEngineToScreen

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin		Data
;
externB 	bAUTOEnable		;autoexec flag
externB 	bInDDraw
externW 	wScreenWidth		;in VGA.ASM
externW 	wScreenHeight		;in VGA.ASM
externW 	wCursorType		;in VGA.ASM
externW 	FullScreenClipRect	;
externW 	wMMIOSelector		;
externW 	wBpp			;in VGA.ASM
externW 	wScreenWidthBytes	     ;bytes per scanlines
externD 	dDestFmt		;bpp
externD 	dCmd			;CMD_SET current value
externB 	BWFlag			;in VGA.ASM

sEnd		Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _OUTPUT,OutputSeg,para,public,CODE
sBegin	OutputSeg
assumes cs,OutputSeg
	.386
;
externB 	S3Rop2TranslateTable	;in output.asm
;
;----------------------------------------------------------------------
; Polyline_S3
; Uses the pen to draw each line segment from its starting point up to,
; but not including its end point.  If more than one line is drawn, each
; new line starts at the end point of the previous segment.
;----------------------------------------------------------------------
cProc	Polyline_S3_BT, <FAR,PUBLIC,PASCAL,NODATA>, <esi, edi>
	parmD	lpDstDev		;--> to the destination
	parmW	style			;Output operation
	parmW	count			;# of points
	parmD	lpPoints		;--> to a set of points
	parmD	lpPhysPen		;--> to physical pen
	parmD	lpPhysBrush		;must be NULL
	parmD	lpDrawMode		;--> to a Drawing mode
	parmD	lpClipRect		;--> to a clipping rectange if <> 0

	localW	X1
	localW	X2
	localW	Y1
	localW	Y2
	localW	org_X1
	localW	org_X2
	localD	lCmd			;current CMD_SET value
	localW	DeviceFlags

	localB	swapped
	localB	last_pixel
	localD	coef
	localD	xstart
	localD	ystart
	localD	delta_x
	localD	delta_y
	localW	adj

;
cBegin
	assumes ds,nothing
	assumes es,nothing
	mov	ax,es			;
	mov	gs,ax			;
	assumes gs,Data
;
	lfs	si,lpDstDev		;FS:SI-->lpDstDev
	test	fs:[si].deFlags,VRAM	;is this the screen ?
	jz	O_DIBEngineToScreen	;no, let DIB engine do it.
;
PL_Begin:
	les	di,lpPhysPen		;ES:DI -> lpPhysPen
	mov	ax,es:[di].dpPenStyle	;get pen style
	cmp	ax,LS_NOLINE		;is it null pen ?
	je	PL_Exit 		;yes, do nothing
	cmp	ax,LS_SOLID		;is it a solid line ?
	jne	O_DIBEngineToScreen	;no, let DIB engine do it
;
	mov	eax,dDestFmt		;initialize dCmd
	mov	lCmd,eax		;dCmd = destfmt
	cmp	lpClipRect,0		;any cliping rect ?
	jz	short PL_CheckCursor	;no,

	lds	si,lpClipRect		;
	mov	ax,[si] 		;
	cmp	ax,[si+4]		;
	jge	PL_Exit
	mov	ax,[si+2]		;
	cmp	ax,[si+6]		;
	jge	PL_Exit
;
	lds	si,lpDstDev		;FS:SI-->lpDstDev
	bts	[si].deFlags,BUSY_BIT	;Are we busy?
	jc	O_DIBEngineToScreen	;yes, let DIB engine do it.
;
;If using the software cursor, exclude the cursor.
;
PL_CheckCursor:
	cmp	bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	mov	ax,wMMIOSelector
	mov	fs,ax			;FS--> MMIO segment
	WaitDMA				; wait for DMA to stop before MMIO access
@@:
	cmp	wCursorType,SOFTWARECURSOR ;using software cursor ?
	jne	short PL_SetBusyBit	;no.
	xor	cx,cx			;assume full screen rect.
	mov	dx,cx			;
	mov	si,wScreenWidth
	mov	di,wScreenHeight
	cmp	lpClipRect,0		;Is there a cliping rect?
	jz	short @F		;no.
;
	lds	si,lpClipRect		;DS:SI -> lpClipRect
	mov	cx,[si].left		;starting x
	mov	dx,[si].top		;starting y
	mov	di,[si].bottom		;ending y+1
	mov	si,[si].right		;ending x+1
;
@@:	push	es
	push	lpDstDev		;PDevice
	push	cx			;Left
	push	dx			;Top
	push	si			;Right
	push	di			;Bottom
	push	CURSOREXCLUDE		;Flags
	call	BeginAccess
	pop	es
;
PL_SetBusyBit:
	lds	si,lpDstDev		;DS:SI -> lpDstDev
	mov	ax,wMMIOSelector
	mov	fs,ax			;FS--> MMIO segment
if	1;pci_fix
	WaitEngineIdle fs
;	CheckFIFOSpace SIXTEEN_SLOTS
endif
;	cmp	bInDDraw,0		;need to patch?
;	je	short @F		;N, skip
	xor	eax,eax
	EngineWrite B_SRC_BASE
	EngineWrite B_DEST_BASE

	mov	ax,wScreenWidthBytes		;bytes per scanlines
	mov	dx,ax
	rol	eax,16				;destination stride
	or	ax,dx				;source stride

	EngineWrite B_DEST_SRC_STR
;@@:
;
;
; Initialize data for Rop2 and pen color
;
PL_InitData:
	xor	eax,eax
	xor	bx,bx
	lds	si,lpDrawMode		;DS:SI -> lpDrawMode
	mov	bl,bptr [si].Rop2	;get ROP 1-16
	dec	bx			;make it 0-15
	mov	al,cs:S3Rop2TranslateTable[bx] ;Translate to S3 h/w rop
	shl	eax,17			;CMD_SET[24:17]
	or	eax,LINE_DRAW
;	 or	 eax,LINE_DRAW+bAE_On
	or	lCmd,eax		;Rop256+destFmt+clip

	cmp	lpClipRect,0		;any cliping rect ?
	jz	short @F
	CheckFIFOSpace TWO_SLOTS
	lds	si,lpClipRect		;DS:SI -> lpClipRect
	mov	ax,[si].left		;left clipping
	rol	eax,16			;in high dword
	mov	ax,[si].right		;get right clipping
	dec	ax
	EngineWrite L_CLIP_L_R

	mov	ax,[si].top		;get top clipping
	rol	eax,16			;in high dword
	mov	ax,[si].bottom		;get bottom clipping
	dec	ax
	EngineWrite L_CLIP_T_B		;set Y-axis clipping
	or	lCmd,bClip_Enable
@@:
	les	di,lpPhysPen		;ES:DI -> lpPhysPen
	mov	ebx,es:[di].dpPenColor	;save pen color

	CheckFIFOSpace ONE_SLOT
	EngineWrite L_PAT_FG_CLR, fs, ebx

PL_SetupDrawLoop:
	mov	last_pixel,1		;last pixel off
	mov	cx,count		;# points to draw
	dec	cx			;
	lds	si,lpPoints		;DS:SI -> point array
;
;Next polyline draw enters here
;
PL_Loop:
	push	cx			;save our loop counter
	lodsw
	mov	X1,ax
	lodsw
	mov	Y1,ax
	mov	cx,[si] 		;cx = X2
	mov	dx,[si+2]		;dx = Y2
	mov	X2,cx
	mov	Y2,dx

	cCall	ClipLinePoints		;clip (x1,y1)(x2,y2)
	jc	PL_NextPolyline

	mov	ax,Y1
	cmp	ax,Y2
	jge	short PL_1
;swap them
	mov	ax,X1
	xchg	ax,X2
	mov	X1,ax
	mov	ax,Y1
	xchg	ax,Y2
	mov	Y1,ax
	mov	last_pixel,1		;adjust first scan
	jmp	short PL_CalcDelta
PL_1:
	mov	last_pixel,2		;adjust last scan

PL_CalcDelta:
	mov	ax,X1			;ax = X1
	mov	cx,X2			;cx = X2
	mov	org_X1,ax
	mov	org_X2,cx

;### patch for bug14
if	1
test	BWFlag,1			;ViRGE/VX rev C or up ?
jz	@F				;no. go check if it's 24 bits mode
test	BWFlag,8			;fast page mode too
jz	PL_8_16 			;no. skip it
cmp	wBpp,16 			;16 or 24 bits mode ?
jb	PL_8_16				;no. skip it
jmp	short dopatch14 		;apply the patch
@@:
cmp	wBpp,24
jnz	PL_8_16

dopatch14:
xor	ebx,ebx
CheckFIFOSpace FOUR_SLOTS fs

EngineWrite B_RSRC_XY, fs, ebx
EngineWrite B_RDEST_XY, fs, ebx
inc	bx
EngineWrite B_RWIDTH_HEIGHT, fs, ebx
mov	ebx,05980020h
or	ebx,dDestFmt		;initialize dCmd
cmp	ax,cx
jge	short @F
or	ebx,02000000h
@@:
WriteCMDAutoOFF B_CMD_SET, fs, ebx

PL_8_16:
endif

	mov	di,Y1			;di = Y1
	sub	ax,X2			;deltaX = X1 - X2
	movsx	eax,ax
	mov	delta_x,eax
	sub	di,Y2			;deltaY = Y1 - Y2
	mov	cx,ax			;save delat x in cx
	movsx	edi,di
	mov	delta_y,edi
;
;eax =	delta x
;edi =	delta y
	or	ax,ax
	jnz	short not_vertical
	mov	coef,eax
	cmp	last_pixel,1
	jne	short @F
	dec	Y1
	jmp	short ver1
@@:
	cmp	last_pixel,2
	jne	short ver1
	inc	Y2
ver1:
	mov	ax,Y1
	cmp	ax,Y2
	jl	PL_NextPolyline
	mov	ax,X1
	shl	eax,20
	mov	xstart,eax
	jmp	prog_eng

not_vertical:
	mov	adj,1
	cmp	ax,0		;positive delta x?
	jg	short @F	;Y, continue
	mov	adj,-1
@@:
	or	di,di		;horizontal?
	jnz	short @F	;N, continue
	mov	coef,eax
	jmp	adj_endpoint

@@:	;calculate coeficient
;	 cdq
	xor	edx,edx
	test	eax,eax
	jns	short @F
	dec	edx
@@:
	sal	eax,20
	idiv	edi
	mov	coef,eax

	mov	bx,X1
	shl	ebx,20
	mov	xstart,ebx

	mov	ax,cx
	or	ax,ax
	jns	short @F
	neg	ax		;abs(delta x)
@@:
	cmp	ax,di
	jle	short y_major
;x major
	mov	eax,coef
	sar	eax,1
	sub	ebx,eax
	cmp	cx,0			;delat x > 0
	jle	short @F		;N, continue
	add	ebx,(1 shl 20) - 1	;Y, adjust
@@:
	mov	xstart,ebx
	jmp	short adj_endpoint
y_major:
	add	ebx,1 shl 19
	mov	xstart,ebx
adj_endpoint:
	mov	ax,adj
	cmp	last_pixel,1
	jne	short @F
	sub	X1,ax
	jmp	short prog_eng
@@:
	cmp	last_pixel,2
	jne	short prog_eng
	add	X2,ax

prog_eng:
if	1;pci_fix
	CheckFIFOSpace TWELVE_SLOTS, fs
endif
	mov	edx,coef
	neg	edx
	EngineWrite Ldx, fs, edx

	mov	ax,X1
	rol	eax,16			;End0 in high dword
	mov	ax,X2			;End1 in low dword
	EngineWrite LXEnd0_End1, fs, eax

	xor	eax,eax
	mov	edx,xstart
	EngineWrite LXStart, fs, edx

	mov	ax,Y1
	EngineWrite LYStart, fs, eax

	xor	edx,edx
	mov	dx,Y1
	sub	dx,Y2
	inc	dx
	mov	ax,org_X1
	cmp	ax,org_X2
	jge	short @F
	or	edx,80000000h
@@:
	EngineWrite LYCnt, fs, edx	;start draw

	mov	eax,lCmd
	EngineWrite L_CMD_SET, fs, eax

PL_NextPolyline:
	pop	cx			;line count
	dec	cx			;account for continous points
	jnz	PL_Loop 		;Next polyline

;###patch
if	1
CheckFIFOSpace FOUR_SLOTS, fs
mov	eax,00000001h
EngineWrite B_RWIDTH_HEIGHT, fs
EngineWrite B_RSRC_XY, fs
EngineWrite B_RDEST_XY, fs
mov	eax,07980000h+bDraw+CPUAlign_dword+bFirst_dword
or	eax,dDestFmt
EngineWrite B_CMD_SET, fs
endif

PL_CallEndAccess:
	lds	si,lpDstDev
	and	[si].deFlags,not BUSY
	cmp	wCursorType,SOFTWARECURSOR
	jne	short PL_Exit
	push	lpDstDev
	push	CURSOREXCLUDE
	call	EndAccess

PL_Exit:
	stc				;

cEnd ;	Polyline_S3_BT


;------------------------------------------------------------------------------
;Entry:
;	X1, Y1, X2, Y2
;
;Exit:
;	CY	if line is rejected.
;	NC	X1,Y1,X2,Y2: clipped accordingly.
;------------------------------------------------------------------------------
cProc	ClipLinePoints, <NEAR, PUBLIC>
cBegin	<nogen>
	assumes gs,Data
	push	bx
	push	cx
	push	si
mov	swapped,0	;default no swap

CLP_Loop:
	mov	si,X1
	mov	di,Y1
	call	OutCode
	mov	cx,ax			;cx = outcode of X1,Y1
	mov	si,X2
	mov	di,Y2
	call	OutCode 		;ax = outcode of X2,Y2
	mov	bx,ax			;bx = outcode of X2,Y2
	or	ax,cx			;Check for trivial accept.
	jz	short CLP_AcceptLine

	test	bx,cx			;line completely outside ?
	jnz	CLP_RejectLine

	or	cx,cx			;X1,Y1 : outside, X2,Y2 : inside ?
	jnz	short @f		;yes, continue

	xchg	cx,bx			;swap P1 and P2
	mov	ax,X1
	xchg	ax,X2
	mov	X1,ax
	mov	ax,Y1
	xchg	ax,Y2
	mov	Y1,ax
mov	swapped,1

@@:
	mov	ax,X2
	sub	ax,X1			;ax = (X2-x1)
	mov	bx,Y2
	sub	bx,Y1			;bx = (Y2-y1)
	rcr	cx,1
	jc	short CLP_DivideAtLeft
	rcr	cx,1
	jc	short CLP_DivideAtRight
	rcr	cx,1
	jc	short CLP_DivideAtBottom

CLP_DivideAtTop:			;ax=(X2-X1)
	xor	dx,dx			;cx=ymin
@@:
	mov	cx,dx
	xchg	cx,Y1			;cx=y1, y1=ymin
	sub	cx,dx			;cx=(Y1-ymin)
	imul	cx			;ax=(X2-X1)*(Y1-ymin)
	neg	bx			;bx =(Y1-Y2)
	idiv	bx			;ax=(X2-X1)*(Y1-ymin)/(y1-Y2)
	add	X1,ax			;ax=x1+(X2-x1)*(Y1-ymin)/(y1-Y2)
	jmp	CLP_Loop

CLP_DivideAtBottom:			;ax=(X2-X1)
	mov	dx,gs:wScreenHeight	;cx=ymax
	jmp	@b

CLP_DivideAtLeft:
	xor	dx,dx			;cx=xmin
@@:
	xchg	ax,bx			;ax=(Y2-Y1)
	mov	cx,dx
	xchg	cx,X1			;cx=x1, x1=xmin
	sub	dx,cx			;dx=(xmin-X1)
	imul	dx			;ax=(Y2-Y1)*(xmin-X1)
	idiv	bx			;ax=(Y2-Y1)*(xmin-X1)/(X2-x1)
	add	Y1,ax			;y1=y1+(Y2-y1)*(xmin-X1)/(X2-x1)
	jmp	CLP_Loop

CLP_DivideAtRight:
	mov	dx,gs:wScreenWidth	;cx=xmax
	jmp	@b

CLP_AcceptLine:

cmp	swapped,0
je	short @F
;preserve original order
	mov	ax,X1
	xchg	ax,X2
	mov	X1,ax
	mov	ax,Y1
	xchg	ax,Y2
	mov	Y1,ax
@@:
	clc
	pop	si
	pop	cx
	pop	bx
	ret

CLP_RejectLine:
	stc
	pop	si
	pop	cx
	pop	bx
	ret

cEnd	<nogen> ; ClipLinePoints



;------------------------------------------------------------------------------
;OutCode
;Entry:
;	si,di: x,y
;Exit:
;	ax = outcode
;------------------------------------------------------------------------------
cProc	OutCode, <NEAR, PUBLIC>
cBegin	<nogen>
	assumes gs,Data
	xor	ax,ax
	mov	bx,di			;y-ymin
	shl	bx,1
	rcl	ax,1

	mov	dx,gs:wScreenHeight	;ymax-y
	sub	dx,di
	shl	dx,1
	rcl	ax,1

	mov	dx,gs:wScreenWidth	;xmax-x
	sub	dx,si
	shl	dx,1
	rcl	ax,1

	mov	bx,si			;x-xmin
	shl	bx,1
	rcl	ax,1
	ret
cEnd	<nogen> ; OutCode


sEnd    OutputSeg

end
