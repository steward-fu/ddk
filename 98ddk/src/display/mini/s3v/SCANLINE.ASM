;----------------------------------------------------------------------------
; SCANLINE.ASM
; Copyright (c) 1994 Microsoft Corporation
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
patch16 	equ	1
	.xlist
DOS5 = 1
	include cmacros.inc
	include macros.inc
	include dibeng.inc
	incOutput   = 1
	incDrawMode = 1
	incLogical  = 1
	include gdidefs.inc
	include s3.inc
.list
;
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
rscan_type	STRUC
SolidColor	    dd	    0	    ;color for solid cases
DrawFn		    dw	    0	    ;Drawing function
ScanFlags	    db	    0	    ;various scan flags
DIBFlag 	    db	    0	    ;###
rscan_type	ENDS

ScanStruct	STRUC
scnNumXs	dw	0
scnY0		dw	0
scnY1		dw	0
scnPntX0	dw	0		;this is an array
scnPntX1	dw	0		;this is an array
scnNumXsToo	dw	0
ScanStruct	ENDS

; flag bits for ScanFlags
BEGIN_SCAN_FORCED EQU	 10000000b	;begin_scanline has been done
BEGIN_SCAN_DONE   EQU	 01000000b	;begin_scanline has been done
HANDLED_BY_DIBENG EQU	 00100000b
PATTERN_SCANLINE  EQU	 00000001b	;indicate patterned scanline

;----------------------------------------------------------------------------
; E X T E R N
;----------------------------------------------------------------------------
externFP	BeginAccess
externFP	EndAccess
externNP	O_DIBEngineToScreen

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin		Data
;
externB 	bAUTOEnable		;autoexec flag
externB 	bInDDraw
externB 	bPatchFlag
externW 	wBpp
externW 	wBpp_Byte
externW 	wScreenWidth		;in VGA.ASM
externW 	wScreenHeight		;in VGA.ASM
externW 	wCursorType		;in VGA.ASM
externW 	FullScreenClipRect	;
externW 	wMMIOSelector
externW 	wScreenWidthBytes	     ;bytes per scanlines
externD 	dDestFmt		;bpp
externD 	dCmd			;CMD_SET current value
externB 	BWFlag			;in INIT.ASM
externB 	S3MobileData		;in MOBILE.ASM
ALIGN 4
public	rscan
rscan		db	(size rscan_type) DUP (0)
;
sEnd		Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _OUTPUT,OutputSeg,para,public,CODE
sBegin	OutputSeg
assumes cs,OutputSeg
		.386

externB S3Rop2TranslateTable

;------------------------------------------------------------------------
; Scanline_S3
; Entry:
;	es = Data
;	dx = style parameter
;------------------------------------------------------------------------
cProc	Scanline_S3, <FAR,PUBLIC,PASCAL,NODATA>, <esi,edi>
	parmD	lpDstDev		;--> to the destination
	parmW	style			;Output operation
	parmW	count			;# of points
	parmD	lpPoints		;--> to a set of points
	parmD	lpPhysPen		;--> to physical pen
	parmD	lpPhysBrush		;--> to physical brush
	parmD	lpDrawMode		;--> to a Drawing mode
	parmD	lpClipRect		;--> to a clipping rectange if <> 0
	localW	nScanStructs
	localW	BackgroundMode
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
;
	cmp	wBpp,4
	jle	O_DIBEngineToScreen
	lds	si,lpDstDev		;
	test	[si].deFlags,BUSY  ;
	jnz	SL_Exit
	test	rscan.ScanFlags,BEGIN_SCAN_DONE ;BeginScan called already?
	jz	SL_ForceBeginScan

PLABEL SL_GetDrawFn
	mov	bx,rscan.DrawFn
	test	bx,bx
	jz	SL_DetermineDrawFn

PLABEL SL_Draw
	lds	si,lpDstDev		;
	bts	[si].deFlags,BUSY_BIT	;
	jc	SL_DrawDone		;
;
	mov	ax,es:wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector
	cmp	es:bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	WaitDMA 			; wait for DMA to stop before MMIO access
@@:
	DisablePD es,fs
	xor	eax,eax 		; default base
	mov	cx,es:[wScreenWidthBytes];bytes per scanlines
	mov	dx,cx			;save dest stride
	shl	ecx,16			;destination stride high word
	mov	cx,dx			;source stride = dest stride

	CheckFIFOSpace THREE_SLOTS
	EngineWrite B_SRC_BASE
	EngineWrite B_DEST_BASE
	EngineWrite B_DEST_SRC_STR ,, ecx

	call	bx
;
PLABEL SL_DrawDone
	test	rscan.ScanFlags,BEGIN_SCAN_FORCED ;Was BeginScan forced?
	jnz	SL_ForceEndScan 	;yes, force EndScan.

PLABEL SL_ClearBusy
;###patch
	mov	ax,wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector
CheckFIFOSpace FOUR_SLOTS, fs
xor	eax,eax
EngineWrite B_RSRC_XY, fs
EngineWrite B_RDEST_XY, fs
inc	ax				;height is 1, width is 1 (zero based)
EngineWrite B_RWIDTH_HEIGHT, fs 	;height
mov	eax,07980000h+bDraw+CPUAlign_dword+bFirst_dword
or	eax,dDestFmt
EngineWrite B_CMD_SET, fs

	EnablePD es,fs
	lds	si,lpDstDev		;DS:SI -> lpDstDev
	and	[si].deFlags,not BUSY
PLABEL SL_Exit
	mov	ax,1			; show success
cEnd


PLABEL SL_ForceBeginScan
	call	IBeginScan		;Force BeginScan.
	test	ax,ax			;
	jz	O_DIBEngineToScreen	;let the DIB Engine handle it.
	or	rscan.ScanFlags,BEGIN_SCAN_FORCED
	jmp	SL_GetDrawFn
;
PLABEL SL_ForceEndScan
	call	IEndScan		;Force EndScan.
	jmp	SL_ClearBusy

PLABEL SL_DetermineDrawFn
	xor	bx,bx			;
	test	rscan.ScanFlags,PATTERN_SCANLINE
	jz	short @F		;solid scanlines
	or	bx,2			;patterned scanlines
;
@@:	cmp	style,OS_SCANLINES	;
	je	short @F		;OS_SCANLINES calls
	or	bx,4			;OS_POLYSCANLINES calls
;
@@:	mov	bx,word ptr cs:ScanFnTable[bx]
	mov	rscan.DrawFn,bx
	jmp	SL_Draw

ScanFnTable	label	word
	dw	SingleScanSolid
	dw	SingleScanPatterned
	dw	PolyScanSolid
	dw	PolyScanPatterned
;
;------------------------------------------------------------------------
; SingleScanSolid
; Draws one or more scanlines with different widths on the same ycoord
;------------------------------------------------------------------------
PPROC	SingleScanSolid    near

	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing


	lds	si,lpPoints	 ;DS:SI -> lpPoints
	mov	bx,[si].ycoord	 ;get ycoord
	mov	cx,count
	add	si,4		 ;fs:[si]-->set of x intervals
	dec	cx		 ;cx = # of x intervals

;
;Draws each line using rectfill with height equal to one
;
PLABEL SSS_XLoop
	CheckFIFOSpace THREE_SLOTS

	mov	dx,[si+2]	;get x1
	mov	ax,[si] 	;get x0
	sub	dx,ax		;x1 - x0
	dec	dx		;dx = (width - 1)
	js	short @F	;nothing to draw
if patch16
	test	BWFlag,8
	jz	skippat16
	push	eax
	push	ebx
	push	edx
	push	si
	cmp	dx,19
	jle	sss_1
	mov	dx,19
sss_1:
	rol	edx,16		;width in high dword
	mov	dx,1		;height = 1 (in low dword)
	EngineWrite B_RWIDTH_HEIGHT, fs, edx

	rol	eax,16		;x0 in high dword
	add	si,4		;move ptr to next set
	mov	ax,bx		;ycoord in low dword
	EngineWrite B_RDEST_XY

	mov	eax,dCmd
	WriteCMDAutoOFF B_CMD_SET
	pop	si
	pop	edx
	pop	ebx
	pop	eax
skippat16:
endif
	rol	edx,16		;width in high dword
	mov	dx,1		;height = 1 (in low dword)
	EngineWrite B_RWIDTH_HEIGHT, fs, edx

	rol	eax,16		;x0 in high dword
	add	si,4		;move ptr to next set
	mov	ax,bx		;ycoord in low dword
	EngineWrite B_RDEST_XY

	mov	eax,dCmd
	WriteCMDAutoOFF B_CMD_SET
@@:
	dec	cx		;any more lines to do ?
	jnz	SSS_XLoop	;yes
	ret

SingleScanSolid    endp

;
;------------------------------------------------------------------------
; SingleScanPatterned
;
; Draws single scanline(s) with a color pattern on the same y-coord
;------------------------------------------------------------------------
PPROC	SingleScanPatterned    near

	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing


	lds	si,lpPoints
	mov	bx,[si].ycoord
	mov	cx,count
	add	si,4		;fs:[si]-->set of x intervals
	dec	cx		;cx = # of x intervals

PLABEL SSP_XLoop
	CheckFIFOSpace THREE_SLOTS
	mov	dx,[si+2]	;get x1
	mov	ax,[si] 	;get x0
	sub	dx,ax		;x1 - x0
	dec	dx		;dx = (width - 1)
	js	SSP_NextLoop	;nothing to draw

	rol	edx,16		;width in high dword
	mov	dx,1		;height = 1 (in low dword)
	EngineWrite B_RWIDTH_HEIGHT, fs, edx

	rol	eax,16		;x0 in high dword
	add	si,4		;move ptr to next set
	mov	ax,bx		;ycoord in low dword
	EngineWrite B_RDEST_XY

	mov	eax,dCmd
	WriteCMDAutoOFF B_CMD_SET

SSP_NextLoop:
	dec	cx
	jnz	SSP_XLoop
	ret

SingleScanPatterned    endp
;
;------------------------------------------------------------------------
; PolyScanSolid
; Draws a series of solid scanlines
;------------------------------------------------------------------------
PPROC	PolyScanSolid	   near

	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing


	lds	si,lpPoints
	mov	ax,count
	mov	nScanStructs,ax 	;so we can restart if necessary

	test	es:bPatchFlag,SPEEDY_24BPP_PATCH
	jz	short @F
	WaitEngineIdle fs
@@:

PLABEL PSS_ScanStructLoop
	mov	di,[si].scnY1
	mov	bx,[si].scnY0		;bx = y0
	sub	di,bx			;height = y1 - y0
	mov	cx,[si].scnNumXs
	lea	si,[si].scnPntX0

;
;di = height
;bx = scnY0
;
PLABEL PSS_XLoop
	CheckFIFOSpace THREE_SLOTS

	mov	dx,[si+2]		;x1
	mov	ax,[si] 		;x0
	sub	dx,ax			;(x1 - x0)
	dec	dx			;width-1
	js	short PSS_CheckLoop	;nothing to draw

if patch16
	test	BWFlag,8
	jz	skip_pat16
	push	eax
	push	ebx
	push	edx
	push	si
	cmp	dx,19
	jle	short pss_1
	mov	dx,19
pss_1:
	rol	edx,16		;width in high dword
	mov	dx,1		;height = 1 (in low dword)
	EngineWrite B_RWIDTH_HEIGHT, fs, edx

	rol	eax,16		;x0 in high dword
	add	si,4		;move ptr to next set
	mov	ax,bx		;ycoord in low dword
	EngineWrite B_RDEST_XY

	mov	eax,dCmd
	WriteCMDAutoOFF B_CMD_SET
	pop	si
	pop	edx
	pop	ebx
	pop	eax
skip_pat16:
endif
	rol	edx,16			;width in high dword
	mov	dx,di			;get height in low dword
	EngineWrite B_RWIDTH_HEIGHT, fs, edx

	rol	eax,16			;get x0 in high dword
	add	si,4			;move ptr to next set
	mov	ax,bx			;get scnY0 in low dword
	EngineWrite B_RDEST_XY

	mov	eax,dCmd
	WriteCMDAutoOFF B_CMD_SET

PSS_CheckLoop:
	sub	cx,2
	jnz	PSS_XLoop
	add	si,2			;skip ScnNumXsToo
	dec	nScanStructs
	jnz	PSS_ScanStructLoop
;
PLABEL PSS_Exit
	ret

PolyScanSolid	endp

;------------------------------------------------------------------------
; PolyScanPatterned
; Draws a series of patterned scanlines
;------------------------------------------------------------------------
PPROC	PolyScanPatterned      near

	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing


	lds	si,lpPoints
	mov	ax,count
	mov	nScanStructs,ax 	;so we can restart if necessary

	test	es:bPatchFlag,SPEEDY_24BPP_PATCH
	jz	short @F
	WaitEngineIdle fs
@@:

PLABEL PSP_ScanStructLoop
	mov	di,[si].scnY1
	mov	bx,[si].scnY0		;bx = y0
	sub	di,bx			;height = y1 - y0
	mov	cx,[si].scnNumXs
	lea	si,[si].scnPntX0
;
;di = height
;bx = scnY0
;
PLABEL PSP_XLoop
	CheckFIFOSpace THREE_SLOTS
	mov	dx,[si+2]		;x1
	mov	ax,[si] 		;x0
	sub	dx,ax			;(x1 - x0)
	dec	dx			;width-1
	js	short PSP_NextLoop	;nothing to draw

	rol	edx,16			;width in high dword
	mov	dx,di			;get height in low dword
	EngineWrite B_RWIDTH_HEIGHT, fs, edx

	rol	eax,16			;get x0 in high dword
	add	si,4			;move ptr to next set
	mov	ax,bx			;get scnY0 in low dword
	EngineWrite B_RDEST_XY

	mov	eax,dCmd
	WriteCMDAutoOFF B_CMD_SET

PSP_NextLoop:
	sub	cx,2
	jnz	PSP_XLoop
	add	si,2			;skip ScnNumXsToo
	dec	nScanStructs
	jnz	PSP_ScanStructLoop
;
PLABEL PSP_Exit
	ret
PolyScanPatterned   endp

;------------------------------------------------------------------------
; IBeginScan
;------------------------------------------------------------------------
PPROC	IBeginScan   near
;
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing

	mov	ax,es
	mov	gs,ax
assumes gs,Data
;
	mov	ax,wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector

	mov	eax,dDestFmt
	mov	dCmd,eax
;
	cmp	wptr lpClipRect+2,0	;is there a cliping rect?
	jz	short @f		;no.
;
;Check clip region.
;
	lds	si,lpClipRect		;DS:SI--> lpClipRect
	mov	bx,[si] 		;
	cmp	bx,[si]+4		;
	jge	IBS_Fail		;
	mov	bx,[si]+2		;
	cmp	bx,[si]+6		;
	jge	IBS_Fail		;
;
	CheckFIFOSpace TWO_SLOTS
	mov	ax,[si].left		;left clipping
	rol	eax,16			;in high dword
	mov	ax,[si].right		;get right clipping
	dec	ax
	EngineWrite B_CLIP_L_R

	mov	ax,[si].top		;get top clipping
	rol	eax,16			;in high dword
	mov	ax,[si].bottom		;get bottom clipping
	dec	ax
	EngineWrite B_CLIP_T_B		;set Y-axis clipping

	or	dCmd,bClip_Enable    ;enable clipping
;
@@:	lds	si,lpDrawMode		;DS:SI -> lpDrawMode
	xor	bx,bx
	xor	eax,eax 		;
	mov	bl,bptr [si].Rop2	;get ROP 1-16
	dec	bx			;make 0-15
	mov	al,cs:S3Rop2TranslateTable[bx] ;Translate to S3 h/w rop
	shl	eax,17			;get Rop into CMD_SET[24:17]
	or	dCmd,eax		;destFmt+clip+Rop256
	mov	ax,[si].bkMode		;
	cmp	ax,2			;is background opaque ?
	je	short @F
	jmp	IBS_Fail		;exit
@@:
	mov	BackgroundMode,ax	;
;
	cmp	wptr lpPhysBrush+2,0	;is there a brush?
	je	IBS_FillWithPen 	;no.
;
PLABEL IBS_FillWithBrush
	lds	si,lpPhysBrush		;DS:SI-->lpPhysBrush
	mov	ax,[si].dp8BrushStyle	;AL contains style
	mov	ah,[si].dp8BrushFlags	;AH contains flags
;
	cmp	al,BS_PATTERN		;patterned brush ?
	jne	short @F		;no, check for solid brush
;
	test	ah,PATTERNMONO		;monochrome pattern ?
	jz	IBS_ColorPatternBrush;no, it is a color pattern brush
	jmp	IBS_MonoPatternBrush ;yes, it is a mono pattern brush
;
@@:	cmp	al,BS_SOLID		;solid brush ?
	jne	short @F		;no, check for hatched brush
	test	ah,COLORSOLID		;solid color ?
	jnz	IBS_SolidBrush		;yes
	jmp	IBS_ColorPatternBrush;no, we have dithered color pattern
;
@@:	cmp	al,BS_HATCHED		;hatched brush ?
	je	IBS_HatchPatternBrush;yes,
;
;The brush is hollow. We perform the drawing if the binary raster op does
;not include source operands.
;
	lgs	di,lpDrawMode		;GS:DI--> lpDrawMode
	mov	ax,gs:[di].Rop2 	;get Rop2
	cmp	ax,R2_BLACK		;Black ?
	je	IBS_HollowBrush 	;yes, go handle it
	cmp	ax,R2_WHITE		;White ?
	je	IBS_HollowBrush 	;yes, go handle it
	cmp	ax,R2_NOT		;Dn ?
	je	IBS_HollowBrush 	;yes, go handle it
	cmp	ax,R2_NOP		;D ?
	je	IBS_HollowBrush 	;yes, go handle it
	jmp	IBS_Fail		;exit

;
;For color patterned brush, we load the pattern into the pattern registers
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
PLABEL IBS_ColorPatternBrush
	or	dCmd,CLRPATBLT
	lea	si,[si].dp8BrushBits	;DS:SI-->color pattern bitmap

;	 mov	 eax,dCmd
;	 WriteCMDAutoON B_CMD_SET
;
;load 8x8 color pattern into the color pattern registers
;
	cld
	push	es
	mov	ax,wMMIOSelector
	mov	es,ax
assumes es,nothing
	mov	ax,gs:wBpp_Byte 	;get bytes_per_pixel
	mov	di,COLOR_PATTERN	;fs:di -> color pattern regs
;
;ds:si -> dp8BrushBits
;es:di -> color pattern registers
;
	WaitEngineIdle es
@@:
	mov	cx,16			;16 dwords
	rep	movsd
	dec	ax			;16bpp, xfer 16*2 dwords
	jnz	short @b		;24bpp, xfer 16*3 dwords
	pop	es
	or	rscan.ScanFlags,PATTERN_SCANLINE
	jmp	IBS_CheckCursor

;
;Enter here for hatch patterned brush
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
PLABEL IBS_HatchPatternBrush
	lea	si,[si].dp8BrushMask	;DS:SI-->hatch masks
;
;For Opaque case, load the hatched patterned into the MONO pat regs
;
IBS_HatchOpaque:
	or	dCmd,MONOPATBLT
;
;Load hatched pattern brush into the mono pat regs
;
	CheckFIFOSpace FOUR_SLOTS
	mov	al,[si]
	ror	eax,8
	mov	al,[si+4]
	ror	eax,8
	mov	al,[si+8]
	ror	eax,8
	mov	al,[si+12]
	ror	eax,8
	EngineWrite B_MONO_PAT_0

	mov	al,[si+16]
	ror	eax,8
	mov	al,[si+20]
	ror	eax,8
	mov	al,[si+24]
	ror	eax,8
	mov	al,[si+28]
	ror	eax,8
	EngineWrite B_MONO_PAT_1

	lds	si,lpPhysBrush		;DS:SI-->lpPhysBrush
	mov	eax,[si].dp8FgColor	;
	EngineWrite B_PAT_FG_CLR

	mov	eax,[si].dp8BgColor
	EngineWrite B_PAT_BG_CLR

	or	rscan.ScanFlags,PATTERN_SCANLINE
	jmp	IBS_CheckCursor

;
;For monochrome patterned brush, we load the pattern into the MONO_PAT regs
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
PLABEL IBS_MonoPatternBrush
	or	dCmd,MONOPATBLT
	lea	si,[si].dp8BrushMono	;DS:SI-->mono pattern bitmap

	CheckFIFOSpace FOUR_SLOTS

	mov	al,[si]
	ror	eax,8
	mov	al,[si+4]
	ror	eax,8
	mov	al,[si+8]
	ror	eax,8
	mov	al,[si+12]
	ror	eax,8
	EngineWrite B_MONO_PAT_0

	mov	al,[si+16]
	ror	eax,8
	mov	al,[si+20]
	ror	eax,8
	mov	al,[si+24]
	ror	eax,8
	mov	al,[si+28]
	ror	eax,8
	EngineWrite B_MONO_PAT_1

;
;Our brush is monochrome.  Retrieve the pattern and determine the foreground
;and background colours for the draw.
;
	lds	si,lpDrawMode		;DS:SI-->lpDrawMode
	mov	eax,[si].TextColor	;
	EngineWrite B_PAT_FG_CLR

	mov	eax,[si].bkColor	;
	EngineWrite B_PAT_BG_CLR

	or	rscan.ScanFlags,PATTERN_SCANLINE
	jmp	IBS_CheckCursor

;
;  DS:SI-->lpPhysBrush
;
PLABEL	IBS_SolidBrush
	mov	eax,dword ptr [si].dp8BrushBits ;get brush solid colour
;
;eax = solid brush/pen color
PLABEL IBS_SetSolidColor		;for solid brush & pen
	or	dCmd, RECT_FILL + bPAT_Mono
	and	rscan.ScanFlags,not PATTERN_SCANLINE

	mov	ebx,eax

	CheckFIFOSpace ONE_SLOT
	EngineWrite B_PAT_FG_CLR, fs, ebx

	jmp	short IBS_CheckCursor

PLABEL IBS_HollowBrush
	or	dCmd, RECT_FILL + bPAT_Mono
	and	rscan.ScanFlags,not PATTERN_SCANLINE

PLABEL IBS_CheckCursor
	cmp	es:wCursorType,SOFTWARECURSOR ;using software cursor ?
	jne	IBS_SetClippingRect  ;no.
	xor	cx,cx			;assume full screen rect.
	mov	dx,cx			;
	mov	si,es:wScreenWidth	;
	mov	di,es:wScreenHeight	;
	cmp	wptr lpClipRect+2,cx	;Is there a cliping rect?
	jz	short @f		;no.
	lds	si,lpClipRect		;point to passed clipping rectangle
	mov	cx,[si].left		;starting x
	mov	dx,[si].top		;starting y
	mov	di,[si].bottom		;ending y+1
	mov	si,[si].right		;ending x+1
@@:	push	es
	push	lpDstDev		;PDevice
	push	cx			;Left
	push	dx			;Top
	push	si			;Right
	push	di			;Bottom
	push	CURSOREXCLUDE		;Flags
	call	BeginAccess
	pop	es
assumes es,Data
assumes gs,nothing
;
PLABEL IBS_SetClippingRect

PLABEL IBS_Exit
	or	es:rscan.ScanFlags,BEGIN_SCAN_DONE
	mov	ax,1
	ret
;
PLABEL IBS_Fail
	or	es:rscan.ScanFlags,HANDLED_BY_DIBENG
	mov	es:rscan.DIBFlag,1	;###
	xor	ax,ax
	ret

PLABEL IBS_FillWithPen
	cmp	wptr lpPhysPen+2,0	;Is there a pen?
	je	O_DIBEngineToScreen	;no, let the DIB Engine handle it.
	lds	si,lpPhysPen		;ds:[si]-->pen
	mov	eax,[si].dpPenColor	;get the pen color
	jmp	IBS_SetSolidColor	;go set it in PAT_FG_CLR

IBeginScan   endp

;------------------------------------------------------------------------
; IEndScan
;------------------------------------------------------------------------
PPROC	IEndScan     near
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
;
	xor	ax,ax
	test	rscan.ScanFlags,HANDLED_BY_DIBENG
	mov	rscan.ScanFlags,al	;show EndScan has been done.
	jnz	IES_Fail		;
;###
	test	es:rscan.DIBFlag,1
	mov	rscan.DrawFn,ax 	;clear draw function.
	mov	es:rscan.DIBFlag,al
	jnz	IES_Fail
;
;
PLABEL IES_CallEndAccess
	cmp	es:wCursorType,SOFTWARECURSOR
	jne	@f
	push	es
	push	lpDstDev
	push	CURSOREXCLUDE
	call	EndAccess
	pop	es
@@:	mov	ax,1

PLABEL IES_Fail
	ret

IEndScan     endp
;
;
;------------------------------------------------------------------------
; BeginScan_S3
; OUTPUT(OS_BEGINNSCAN)
; Prepares for a series of calls to OUTPUT(OS_SCANLINES)
;------------------------------------------------------------------------
cProc	BeginScan_S3, <FAR,PUBLIC,PASCAL,NODATA>, <esi,edi>
	parmD	lpDstDev		;--> to the destination
	parmW	style			;Output operation
	parmW	count			;# of points
	parmD	lpPoints		;--> to a set of points
	parmD	lpPhysPen		;--> to physical pen
	parmD	lpPhysBrush		;--> to physical brush
	parmD	lpDrawMode		;--> to a Drawing mode
	parmD	lpClipRect		;--> to a clipping rectange if <> 0
	localW	nScanStructs
	localW	BackgroundMode
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
;
	cmp	wBpp,4
	jle	O_DIBEngineToScreen
	lds	si,lpDstDev
	bts	[si].deFlags,BUSY_BIT	;
	jc	short BS1

	cmp	bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	mov	ax,wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector
	WaitDMA 			; wait for DMA to stop before MMIO access
@@:
	call	IBeginScan
	lds	si,lpDstDev		;
	and	[si].deFlags,not BUSY	;
	test	ax,ax
	jz	O_DIBEngineToScreen	;let the DIB Engine handle it.
BS1:
cEnd

;------------------------------------------------------------------------
; EndScan_S3
; OUTPUT(OS_ENDNSCAN)
; Ends the series of scanlines calls
;------------------------------------------------------------------------
cProc	EndScan_S3, <FAR,PUBLIC,PASCAL,NODATA>, <esi,edi>
	parmD	lpDstDev		;--> to the destination
	parmW	style			;Output operation
	parmW	count			;# of points
	parmD	lpPoints		;--> to a set of points
	parmD	lpPhysPen		;--> to physical pen
	parmD	lpPhysBrush		;--> to physical brush
	parmD	lpDrawMode		;--> to a Drawing mode
	parmD	lpClipRect		;--> to a clipping rectange if <> 0
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
;
	cmp	wBpp,4
	jle	O_DIBEngineToScreen
	lds	si,lpDstDev
	bts	[si].deFlags,BUSY_BIT	;
	jc	short ES1
	call	IEndScan
	lds	si,lpDstDev		;
	and	[si].deFlags,not BUSY	;
	test	ax,ax
	jz	O_DIBEngineToScreen	;let the DIB Engine handle it.
ES1:
cEnd

sEnd	OutputSeg
end
