;----------------------------------------------------------------------------
; POLYLINE.ASM
; Copyright (c) 1995 S3 Incorporated
;----------------------------------------------------------------------------
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
VIRGE_ID_HIGH	equ	08ah
;
;----------------------------------------------------------------------------
; E X T E R N
;----------------------------------------------------------------------------
		externFP BeginAccess
		externFP EndAccess
		externNP O_DIBEngineToScreen
		externNP ClipLinePoints ; PLINE_BT.ASM

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
externB         bFamilyFlags            ;in VGA.ASM
externB 	DeviceId_High		;in VGA.ASM
externB 	S3MobileData			;in MOBILE.ASM

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
cProc	Polyline_S3, <FAR,PUBLIC,PASCAL,NODATA>, <esi, edi>
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
	assumes fs,nothing
	assumes gs,Data

	mov	ax,DGROUP		;
	mov	gs,ax			;
	lfs	si,[lpDstDev]		; FS:SI = lpDstDev dest PDEVICE ptr
	test	fs:[si].deFlags,VRAM	; on-scrn/offscrn mem dest ?
	jz	O_DIBEngineToScreen	; no, let DIB engine handle it
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
	mov	es,gs:[wMMIOSelector]	; ES: = engine MMIO selector
	DisablePD
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
PLABEL PL_SetBusyBit
	lds	si,lpDstDev		;DS:SI -> lpDstDev
	mov	ax,wMMIOSelector
	mov	fs,ax			;FS--> MMIO segment

	cmp	bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	WaitDMA 			; wait for DMA to stop before MMIO access
@@:
	xor	eax,eax 		; default base
	mov	bx,wScreenWidthBytes	;bytes per scanlines
	mov	dx,bx			;save dest stride
	shl	ebx,16			;destination stride high word
	mov	bx,dx			;source stride = dest

	CheckFIFOSpace THREE_SLOTS
	EngineWrite B_SRC_BASE
	EngineWrite B_DEST_BASE
	EngineWrite B_DEST_SRC_STR ,, ebx
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

public PL_SetupDrawLoop
PL_SetupDrawLoop:
	mov	last_pixel,1		;last pixel off
	mov	cx,count		;# points to draw
	dec	cx			;
	lds	si,lpPoints		;DS:SI -> point array
;
;Next polyline draw enters here
;
public	PL_Loop
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

;### patch for bug14 (pixel drop), it still happens on DX/GX/GX2.
if	1
;        test    bFamilyFlags, MOBILE_FAMILY
;        jnz     PL_8_16
;        cmp     DeviceId_High,VIRGE_ID_HIGH     ;ViRGE GX/DX or higher ?
;        je      PL_8_16                         ;yes. skip it
;        test    BWFlag,1                        ;ViRGE/VX rev C or up ?
;        jz      @F                              ;no. go check if it's 24 bits mode
;        test    BWFlag,8                        ;fast page mode too
;        jz      PL_8_16                         ;no. skip it
;        cmp	wBpp,16 			;16 bits mode ?
;        jb	PL_8_16
;        jmp     short dopatch14                 ;no. skip it
;@@:
;        cmp     wBpp,24
;        jnz     PL_8_16

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
	CheckFIFOSpace SIX_SLOTS, fs
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
xor	eax,eax
EngineWrite B_RSRC_XY, fs
EngineWrite B_RDEST_XY, fs
inc	ax				;height is 1, width is 1 (zero based)
EngineWrite B_RWIDTH_HEIGHT, fs 	;height
mov	eax,07980000h+bDraw+CPUAlign_dword+bFirst_dword
or	eax,dDestFmt
EngineWrite B_CMD_SET, fs
endif

PL_CallEndAccess:
	mov	si,word ptr lpDstDev
	lds	si,lpDstDev
	and	[si].deFlags,not BUSY
	cmp	wCursorType,SOFTWARECURSOR
	jne	short PL_CheckPD
	push	lpDstDev
	push	CURSOREXCLUDE
	call	EndAccess
PL_CheckPD:
	mov	es,gs:[wMMIOSelector]	; ES: = engine MMIO selector
	EnablePD

PLABEL PL_Exit
	stc				;

cEnd ;	Polyline_S3

sEnd	OutputSeg
end
