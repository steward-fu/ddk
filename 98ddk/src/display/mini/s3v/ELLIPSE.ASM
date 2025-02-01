;----------------------------------------------------------------------------
; ELLIPSE.ASM
; Copyright (c) 1995 S3 Incorporated
;----------------------------------------------------------------------------
;
patch16 	equ	1
.xlist
DOS5 = 1
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
NumEntries	equ	2048	;Number of entries in the sine & cosine table
IEntries	equ	6	;Number of 1's in the table
;
EF_OUTLINE	equ	0001h
EF_CLIP 	equ	0002h
EF_INTERIOR	equ	0004h
;
;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------
;
;----------------------------------------------------------------------------
; E X T E R N
;----------------------------------------------------------------------------
externFP	BeginAccess
externFP	EndAccess
externNP	O_DIBEngineToScreen
;
;
;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin		Data
;
externB 	InSrcBlt		;in S3BLT.ASM
externB 	bAUTOEnable		;autoexec flag
externB 	bEllipseFlag		;in INIT.ASM
externB 	bInDDraw
externW 	wBpp_Byte
externW 	wMMIOSelector
externW 	wScreenWidth		;in VGA.ASM
externW 	wScreenHeight		;in VGA.ASM
externW 	wCursorType		;in VGA.ASM
externW 	FullScreenClipRect	;
externW 	wScreenWidthBytes	     ;bytes per scanlines
externD 	dCmd			;CMD_SET current value
externD 	dDestFmt		;bpp
externB 	BWFlag			;in INIT.ASM
externB 	S3MobileData		;in MOBILE.ASM
;
Align 4
public	centerX
centerX 	dw	?	;x comp. of center of a circle (or ellipse)
 public  centerY
centerY 	dw	?	;y comp. of center of a circle (or ellipse)
;
public	radiusX
radiusX 	dw	?	;circle if radiusX == radiusY
public	radiusY
radiusY 	dw	?
;
public	lastRadiusX
lastRadiusX	dw	?	;optimization: if same circle (or ellipse)
lastRadiusY	dw	?	;we don't have to calc pixel array again
;
public	evenWidth
evenWidth	dw	?	;1 if even width (i.e. non-integer center)
public	evenHeight
evenHeight	dw	?	;1 if even height (i.e. non-integer center)
;
	dd	-1
	dd	-1
public	PGScratchBuffer
PGScratchBuffer db	8192 dup (0)
sEnd		Data
;
;
;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _OUTPUT,OutputSeg,para,public,CODE
sBegin	OutputSeg
assumes cs,OutputSeg
.386
;
externB 	S3Rop2TranslateTable	;in OUTPUT.ASM
;
public	SinCos
SinCos		label	near
include 	sincos.inc	;sine(cosine) table if read forward(backward)
;
;
;------------------------------------------------------------------------
; Ellipse
; Draw an ellipse the list of vertices.
;
; Notes:
;  The Foreground Color is set as follows:
;      (a) from the Brush struct for Solid and Color-patterned style brushes.
;	   The colors for these two cases are located at different
;	   locations in the Physical Brush Struct. (refer to the code
;	   and see documentation in RealizeObject procedure)
;      (b) from the DrawMode struct for Mono-patterned style brushes.
;
;------------------------------------------------------------------------
cProc	Ellipse_S3, <FAR,PUBLIC,PASCAL,NODATA>, <esi, edi>
	parmD	lpDstDev		;--> to the destination
	parmW	style			;Output operation
	parmW	Count			;# of points
	parmD	lpPoints		;--> to a set of points
	parmD	lpPhysPen		;--> to physical pen
	parmD	lpPhysBrush		;--> to physical brush
	parmD	lpDrawMode		;--> to a Drawing mode
	parmD	lpClipRect		;--> to a clipping rectange if <> 0

	localW	DeviceFlags
	localW	BackgroundMode
	localW	EllipseFlags
	localW	X1
	localW	Y1
	localW	X2
	localW	Y2
	localW	wWidth
	localD	Rop2Code
	localD	lCmd
;
cBegin
	mov	ax,es
	mov	gs,ax
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data
;
; Return to GDI if we are unable to support rectangle. Don't go to the DIB
; engine, DIB engine doesn't support rectangles.
;
	lfs	si,lpDstDev		;FS:SI-->lpDstDev
	bts	fs:[si].deFlags,BUSY_BIT;
	jc	O_DIBEngineToScreen	;

ECheckBrush:
	mov	EllipseFlags,0		;initialize local flags
	cmp	lpPhysPen,0		;is there a pen ?
	je	short ECheckCoordinates ;no,
	les	di,lpPhysPen		;
	cmp	es:[di].dpPenStyle,LS_NOLINE
	je	short ECheckCoordinates ;
	or	EllipseFlags,EF_OUTLINE ;
;
ECheckCoordinates:
	lds	si,lpPoints		;
	mov	ax,word ptr ds:[si]	;
	mov	X1,ax			;
	mov	bx,word ptr ds:[si+2]	;
	mov	Y1,bx			;
	mov	cx,word ptr ds:[si+4]	;
	mov	X2,cx			;
	mov	dx,word ptr ds:[si+6]	;
	mov	Y2,dx			;
;
	sub	cx,ax			;don't handle size smaller than 2x2
	cmp	cx,2			;i.e. radius=1
	jl	EReturnToGDI		;
	sub	dx,bx			;
	cmp	dx,2			;
	jl	EReturnToGDI		;
	cmp	cx,2048 		;or bigger than 2048x2048
	jg	EReturnToGDI		;
	cmp	dx,2048 		;
	jg	EReturnToGDI		;
;
ECheckClipRect:
	mov	eax,dDestFmt
	mov	lCmd,eax
	cmp	lpClipRect,0		;any clipping rectangle passed?
	je	short ECheckCursor	;
	or	lCmd,bClip_Enable
	or	EllipseFlags,EF_CLIP	;indicate we have clip rect present
;
; Check the passed clipping rectangle. There is nothing to draw, if
; either width or height <= 0.
;
	lds	si,lpClipRect
	mov	ax,[si+4]		;clip rectangle right
	sub	ax,[si] 		;make sure width > 0
	jle	EReturnToGDI		;
	mov	ax,[si+6]		;clip rectangle bottom
	sub	ax,[si+2]		;make sure height > 0
	jle	EReturnToGDI		;
;
;If using the software cursor, exclude the cursor
;
;  DS:SI --> clipping rectangle if the clipping rectangle present
;
ECheckCursor:
	cmp	gs:bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	mov	ax,gs:[wMMIOSelector]	;
	mov	fs,ax			;set FS to point to MMIO seg base
	WaitDMA 			; wait for DMA to stop before MMIO access
@@:
	mov	ax,gs:[wMMIOSelector]	;
	mov	fs,ax			;set FS to point to MMIO seg base
	DisablePD gs,fs
	cmp	gs:wCursorType,SOFTWARECURSOR ;using software cursor ?
	jne	short ESetBusyBit	;no
;
	xor	cx,cx			;assume full screen rect.
	xor	dx,dx			;
	mov	si,gs:wScreenWidth	;
	mov	di,gs:wScreenHeight	;
	test	EllipseFlags,EF_CLIP	;is there a cliping rect?
	jz	short @F		;no.
;
	mov	si,wptr lpClipRect[0]	;
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
;Tell Windows we are busy on the device by setting BUSY bit in deFlags
;field.
;
PLABEL ESetBusyBit
	mov	ax,gs:[wMMIOSelector]	;
	mov	fs,ax			;set FS to point to MMIO seg base

	les	di,lpDstDev		; ES:DI = dest pdevice struct ptr
	xor	eax,eax 		; default base

	mov	bx,gs:[wScreenWidthBytes];bytes per scanlines
	mov	dx,bx			;save dest stride
	shl	ebx,16			;destination stride high word
	mov	bx,dx			;source stride = dest

	CheckFIFOSpace THREE_SLOTS
	EngineWrite B_SRC_BASE
	EngineWrite B_DEST_BASE
	EngineWrite B_DEST_SRC_STR ,, ebx
;
	xor	eax,eax
	les	di,lpDrawMode		;ES:DI-->lpDrawMode
	mov	ax,es:[di].Rop2 	;get the desired Rop2
	dec	ax			;make it offset from 0
	mov	bx,CodeOFFSET S3Rop2TranslateTable
	xlat	cs:S3Rop2TranslateTable ;now AX has Mix Modes
	shl	eax,17			;get b8ROP256 in [24:17]
	mov	Rop2Code,eax		;save it
	or	lCmd,eax
	mov	ax,es:[di].bkMode	;
	mov	BackgroundMode,ax	;
;
;Copy pattern to pattern registers for patterned brush
;
PLABEL EDispatchBrush
	cmp	lpPhysBrush,0		;is there a brush ?
	je	EDrawObjects		;no, don't need to fill interior
;
	or	EllipseFlags,EF_INTERIOR
	lds	si,lpPhysBrush		;DS:SI-->lpPhysBrush
	mov	ax,[si].dp8BrushStyle	;AL contains style
	mov	ah,[si].dp8BrushFlags	;AH contains flags
;
	cmp	al,BS_PATTERN		;patterned brush ?
	jne	short @F		;no, check for solid brush
;
	test	ah,PATTERNMONO		;monochrome pattern ?
	jz	EColorPatternBrush	;no, it is a color pattern brush
	jmp	EMonoPatternBrush	;yes, it is a mono pattern brush
;
@@:	cmp	al,BS_SOLID		;solid brush ?
	jne	short @F		;no, check for hatched brush
	test	ah,COLORSOLID		;solid color ?
	jnz	ESolidBrush		;yes
	jmp	EColorPatternBrush	;no, we have dithered color pattern
;
@@:	cmp	al,BS_HATCHED		;hatched brush ?
	je	EHatchPatternBrush	;yes, treat it as mono pattern
;
;The brush is hollow. We perform the drawing if the binary raster op does
;not include source operands.
;
	and	EllipseFlags,NOT EF_INTERIOR
	jmp	EDrawObjects		;
;
;For color patterned brush, we load the pattern into the pattern regs
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
PLABEL EColorPatternBrush
	or	lCmd,CLRPATBLT
;
;load 8x8 color pattern into the color pattern registers
;
	cld
	push	es
	mov	ax,wMMIOSelector
	mov	es,ax
	mov	ax,gs:wBpp_Byte 	;get bytes_per_pixel
	lea	si,[si].dp8BrushBits	;DS:SI-->color pattern bitmap
	mov	di,COLOR_PATTERN	;es:di -> color pattern regs
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
	jmp	EDrawObjects

;
;Enter here for hatched patterned brush
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
PLABEL EHatchPatternBrush
	cmp	BackgroundMode,2	;is background opaque ?
	jne	EReturnToGDI	       ;no, its transparent
	lea	si,[si].dp8BrushMask	;DS:SI-->hatch masks
;
;For Opaque case, load the hatched patterned into the MONO pat regs
;
EHatchOpaque:
	or	lCmd,MONOPATBLT
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

	jmp	EDrawObjects

;
;For monochrome patterned brush, we load the pattern into off-screen memory.
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
PLABEL EMonoPatternBrush
	cmp	BackgroundMode,2	;is background opaque ?
	jne	EReturnToGDI	       ;no, its transparent
	or	lCmd,MONOPATBLT
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

	les	di,lpDrawMode		;ES:DI-->lpDrawMode
	mov	eax,es:[di].TextColor	;
	EngineWrite B_PAT_FG_CLR

	mov	eax,es:[di].bkColor	;
	EngineWrite B_PAT_BG_CLR

	jmp	EDrawObjects
;
;
;  DS:SI-->lpPhysBrush
;
PLABEL	ESolidBrush
if patch16
	test	BWFlag,08		;ViRGE/VX running fast page mode.
	jz	@F
	or	lCmd,MONOPATBLT
	CheckFIFOSpace FOUR_SLOTS

	mov	eax,-1
	EngineWrite B_MONO_PAT_0
	EngineWrite B_MONO_PAT_1

	mov	eax,dword ptr [si].dp8BrushBits
	EngineWrite B_PAT_FG_CLR
	EngineWrite B_PAT_BG_CLR
	jmp	short EDrawObjects
@@:
endif
	or	lCmd,RECT_FILL + bPAT_Mono

	CheckFIFOSpace ONE_SLOT

	mov	eax,dword ptr [si].dp8BrushBits
	EngineWrite B_PAT_FG_CLR	;get brush solid colour
	jmp	short EDrawObjects


PLABEL EHollowBrush
	or	lCmd,RECT_FILL + bPAT_Mono

PLABEL EDrawObjects
	test	EllipseFlags,EF_CLIP	;do we have a clip rectangle ?
	jz	short EBuildVertices	;no, skip setting of clip

	CheckFIFOSpace TWO_SLOTS

	lds	si,lpClipRect		;DS:SI-->lpClipRect
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
;
PLABEL EBuildVertices
	cCall	LookUpTableEllipse,<X1,Y1,X2,Y2>
;
	test	EllipseFlags,EF_INTERIOR
	jz	short ECheckPen 	;
;
	cCall	CircleFill
;
PLABEL ECheckPen
	test	EllipseFlags,EF_OUTLINE ;
	je	EClearBusyBit
	cCall	EllipseBorder		;

EClearBusyBit:

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

	cmp	gs:wCursorType,SOFTWARECURSOR
	jne	short EExitSuccess	;
	push	lpDstDev		;
	push	CURSOREXCLUDE		;
	call	EndAccess		;
;
EExitSuccess:
	mov	es,gs:[wMMIOSelector]	; ES: = engine MMIO selector
	EnablePD
	mov	ax,1			;indicate success
	jmp	short EExit		;
;
PLABEL EReturnToGDI
	mov	ax,-1			;
;
EExit:
	lfs	si,lpDstDev		;
	and	fs:[si].deFlags,not BUSY
cEnd
;
;-----------------------------------------------------------------------------
; EllipseBorder
;
; Desc: Called from Ellipse.  Draws outline of an ellipse.
;-----------------------------------------------------------------------------
cProc	EllipseBorder,<NEAR,PUBLIC>,<di>
cBegin
;
	mov	ax,gs			;
	mov	ds,ax			;
	assumes ds,Data
;
if patch16
	test	BWFlag,08		;ViRGE/VX running fast page mode.
	jz	@F
	or	lCmd,MONOPATBLT
	CheckFIFOSpace FIVE_SLOTS

	mov	eax,-1
	EngineWrite B_MONO_PAT_0
	EngineWrite B_MONO_PAT_1

	les	di,lpPhysPen		;ES:DI -> lpPhysPen
	mov	eax,es:[di].dpPenColor	;
	EngineWrite B_PAT_FG_CLR
	EngineWrite B_PAT_BG_CLR
	jmp	short prg_wh_ellp
@@:
endif
	or	lCmd,RECT_FILL + bPAT_Mono

;	 mov	 eax,lCmd
;	 WriteCMDAutoON B_CMD_SET

	CheckFIFOSpace TWO_SLOTS

	les	di,lpPhysPen		;ES:DI -> lpPhysPen
	mov	eax,es:[di].dpPenColor	;
	EngineWrite B_PAT_FG_CLR
if patch16
prg_wh_ellp:
endif
;
;Set one pixel at a time, using rect fill
;
	mov	eax,1			 ;width = 1, height = 1
	EngineWrite B_RWIDTH_HEIGHT

;
; See if this is a circle
;
	mov	ax,radiusX		;
	cmp	ax,radiusY		;
	jnz	short @F		;
;
	cCall	CircleBorder		;
	jmp	EBExit			;
;
; Start pixels
;
@@:
	lea	si,PGScratchBuffer	;
;
	cmp	evenHeight,1		;
	jz	EBEven1 		;
;
; For odd height
;
PLABEL EBOddLoop
	mov	bx,word ptr [si]	;x1
	mov	cx,word ptr [si+2]	;y1
	cmp	bx,-1			;
	jz	EBExit			;
;
	add	si,4			;
	or	bx,bx			;
	jz	EBOL0Y			;
;
	CheckFIFOSpace	EIGHT_SLOTS
;
	mov	ax,centerX		;do (-x,y)
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;Y=centerY+y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;do (x,y)
	add	ax,bx			;X=centerX+x (+1 for even)
	add	ax,evenWidth		;
	rol	eax,16			;get x in high dword
	pop	ax			;restore y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	neg	cx			;-(y)
	jz	EBOddLoop		;skip (x,-y) if y=0
;
	mov	ax,centerX		;do (-x,-y)
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;Y=centerY+y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;do (x,-y)
	add	ax,bx			;X=centerX+x (+1 for even)
	add	ax,evenWidth		;
	rol	eax,16			;get x in high dword
	pop	ax			;get y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
	jmp	EBOddLoop
;
EBOL0Y:
	CheckFIFOSpace FOUR_SLOTS
	mov	ax,centerX		;X=centerX
	rol	eax,16			;get x in high dword
	mov	ax,centerY
	sub	ax,cx			;Y=centerY-y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;X=centerX
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;
	add	ax,evenHeight		;Y=centerY+y (+1 for even height)
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	cmp	evenWidth,1		;
	jnz	short @F		;
;
	CheckFIFOSpace FOUR_SLOTS
;
	mov	ax,centerX		;X=centerX+1 (for even width)
	inc	ax			;
	rol	eax,16			;get x in high dword
	push	eax			;save x
	mov	ax,centerY		;
	sub	ax,cx			;Y=centerY-y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	pop	eax			;get x
	mov	ax,centerY		;
	add	ax,cx			;
	add	ax,evenHeight		;Y=centerY+y (+1 for even height)
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
@@:	jmp	EBOddLoop
;
; For even height
;
EBEven1:
	cmp	evenWidth,1		;
	jz	EBEvenEvenLoop		;

PLABEL EBEvenLoop
	mov	bx,word ptr [si]	;x1
	mov	cx,word ptr [si+2]	;y1
	cmp	bx,-1			;
	jz	EBExit			;
;
	add	si,4			;
	or	bx,bx
	jz	EBEL0Y
;
	inc	cx
;
	CheckFIFOSpace EIGHT_SLOTS
	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;Y=centerY+y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	add	ax,bx			;X=centerX+x (+1 for even)
	add	ax,evenWidth		;
	rol	eax,16			;get x in high dword
	pop	ax			;get y in low dword
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	dec	cx
	neg	cx			;do (x,-y)
;
	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;Y=centerY+y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	add	ax,bx			;X=centerX+x (+1 for even)
	add	ax,evenWidth		;
	rol	eax,16			;get x in high dword
	pop	ax			;get y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	jmp	EBEvenLoop
;
EBEL0Y:
	CheckFIFOSpace FOUR_SLOTS
	mov	ax,centerX		;X=centerX
	rol	eax,16			;get x in high dword
	mov	ax,centerY
	sub	ax,cx			;Y=centerY-y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;X=centerX
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;
	add	ax,evenHeight		;Y=centerY+y (+1 for even height)
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	cmp	evenWidth,1		;
	jnz	short @F		;
;
	CheckFIFOSpace FOUR_SLOTS
	mov	ax,centerX		;X=centerX+1 (for even width)
	inc	ax			;
	rol	eax,16			;get x in high dword
	push	eax			;save x
	mov	ax,centerY		;
	sub	ax,cx			;Y=centerY-y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	pop	eax			;get x
	mov	ax,centerY		;
	add	ax,cx			;
	add	ax,evenHeight		;Y=centerY+y (+1 for even height)
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
@@:	jmp	EBEvenLoop
;
; For even height AND even width
;
PLABEL EBEvenEvenLoop
;	EBSkipSpecialCond
	mov	bx,word ptr [si]	;x1
	mov	cx,word ptr [si+2]	;y1
	cmp	bx,-1			;
	jz	EBExit			;
;
	add	si,4			;
	CheckFIFOSpace EIGHT_SLOTS
;
	mov	ax,centerX		;
	add	ax,bx			;
	inc	ax			;X=centerX+x+1
	rol	eax,16			;get x in high dword
	push	eax			;save x
	mov	ax,centerY		;
	add	ax,cx			;
	inc	ax			;Y=centerY+y+1
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	pop	eax			;get x
	mov	ax,centerY		;
	sub	ax,cx			;Y=centerY-y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	pop	ax			;get y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;
	inc	ax			;Y=centerY+y+1
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	jmp	EBEvenEvenLoop		;
;
EBExit:
cEnd
;
;
;-----------------------------------------------------------------------------
; CircleBorder
;
; Desc: Called from EllipseBorder.  Draws outline of a circle.
;-----------------------------------------------------------------------------
cProc	CircleBorder,<NEAR,PUBLIC>,<di>
cBegin
	assumes ds,Data
;
	lea	si,PGScratchBuffer
;
	cmp	evenHeight,1		;
	jz	CBEven1 		;
;
; First do two pixels @y=0
;
	mov	bx,word ptr [si]
	mov	cx,word ptr [si+2]
	add	si,4
;
	CheckFIFOSpace FOUR_SLOTS
	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;Y=centerY+y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	add	ax,bx			;X=centerX+x (+1 for even)
	add	ax,evenWidth		;
	rol	eax,16			;get x in high dword
	pop	ax			;get y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
; Now the rest.
;
CBOddLoop:
	mov	bx,word ptr [si]
	mov	cx,word ptr [si+2]
	cmp	bx,-1			;end of array?
	jz	CBExit
	or	bx,bx
	jz	CBTopBottom
	add	si,4
;
	CheckFIFOSpace FOUR_SLOTS
	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;Y=centerY+y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	add	ax,bx			;X=centerX+x (+1 for even)
	add	ax,evenWidth		;
	rol	eax,16			;get x in high dword
	pop	ax			;get y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	neg	cx			;do -y
;
	CheckFIFOSpace FOUR_SLOTS
	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;Y=centerY+y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	add	ax,bx			;X=centerX+x (+1 for even)
	add	ax,evenWidth		;
	rol	eax,16			;get x in high dword
	pop	ax			;get y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	jmp	CBOddLoop
;
; For even height
;
CBEven1:
	cmp	evenWidth,1
	jz	CBEvenEvenLoop
;
CBEvenLoop:
	mov	bx,word ptr [si]
	mov	cx,word ptr [si+2]
	cmp	bx,-1			;end of array?
	jz	CBExit
	or	bx,bx
	jz	CBTopBottom
	add	si,4
;
	inc	cx			;for even height only!
	CheckFIFOSpace FOUR_SLOTS
;
	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;Y=centerY+y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	add	ax,bx			;X=centerX+x (+1 for even)
	add	ax,evenWidth		;
	rol	eax,16			;get x in high dword
	pop	ax			;get y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	dec	cx			;restore y
	neg	cx			;do -y
	CheckFIFOSpace FOUR_SLOTS
;
	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;Y=centerY+y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	add	ax,bx			;X=centerX+x (+1 for even)
	add	ax,evenWidth		;
	rol	eax,16			;get x in high dword
	pop	ax
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	jmp	CBEvenLoop
;
; For even height AND even width
;
CBEvenEvenLoop:
	mov	bx,word ptr [si]	;
	cmp	bx,-1			;end of array?
	jz	CBExit			;
;
	mov	cx,word ptr [si+2]
	CheckFIFOSpace EIGHT_SLOTS
	mov	ax,centerX		;
	add	ax,bx			;
	inc	ax			;X=centerX+x+1
	rol	eax,16			;get x in high dword
	push	eax			;save x
	mov	ax,centerY		;
	add	ax,cx			;
	inc	ax			;Y=centerY+y+1
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	pop	eax			;get x
	mov	ax,centerY		;
	sub	ax,cx			;Y=centerY-y
	push	ax			;save y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	pop	ax			;get y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;
	sub	ax,bx			;X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;
	inc	ax			;Y=centerY+y+1
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	add	si,4			;
	jmp	CBEvenEvenLoop		;
;
; Lastly do two(or four) pixels @x=0
;
CBTopBottom:
	CheckFIFOSpace FOUR_SLOTS
;
	mov	ax,centerX		;X=centerX
	rol	eax,16			;get x in high dword
	mov	ax,centerY
	sub	ax,cx			;Y=centerY-y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	mov	ax,centerX		;X=centerX
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;
	add	ax,evenHeight		;Y=centerY+y (+1 for even height)
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
	cmp	evenWidth,1		;
	jnz	short CBExit		;
;
	CheckFIFOSpace FOUR_SLOTS
;
	mov	ax,centerX		;X=centerX+1 (for even width)
	inc	ax			;
	rol	eax,16			;get x in high dword
	push	eax			;save x
	mov	ax,centerY		;
	sub	ax,cx			;Y=centerY-y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	pop	eax			;get x
	mov	ax,centerY		;
	add	ax,cx			;
	add	ax,evenHeight		;Y=centerY+y (+1 for even height)
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
CBExit:
cEnd
;
;
;-----------------------------------------------------------------------------
; SavePixel
;
; Desc: Used by LookUpTableEllipse to save pixel coordinates.
;-----------------------------------------------------------------------------
ESavePixel	macro	x,y,reg
local	SPMaybe, SPNew, SPOld
		cmp	x,[reg-4]	;last x
		jnz	short SPNew
		cmp	y,[reg-2]	;last y
		jnz	short SPMaybe
		jmp	short SPOld
SPMaybe:
		mov	ax,[reg-8]	;last x2
		dec	ax
		cmp	ax,x
		jnz	short SPNew
		mov	ax,[reg-6]	;last y2
		inc	ax
		cmp	ax,y
		jnz	short SPNew
		sub	reg,4		;erase last (x,y) pair

SPNew:
		mov	[reg],x
		mov	[reg+2],y
		add	reg,4
SPOld:
		endm
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;LookUpTableEllipse
;
; Desc:
;	Draws an ellipse using fixed entry sine & cosine look up table.
;	In this implementation all the pixel coordinates are saved in an array,
;	and processed later.
; Arg:
;	parmW	Left	;Start horizontal coordinate
;	parmW	Top	;Start vertical coordinate
;	parmW	Right	;End horizontal coordinate
;	parmW	Bottom	;End vertical coordinate
; In:
;	array  <- PGScratchBuffer
; Out:
;	array of (X,Y) pairs for one quadrant of an ellipse
; Note:
;	1) All X>=0 and Y>=0.
;	2) The array starts with (radiusX,0) as the 1st element and
;	    ends with (0,radiusY).
;	3) Both ends of the array are marked by -1.
;	4) No special handling (or optimization) for circles!
;	    Circles are handled by MidpointCircle routine.
;
;-----------------------------------------------------------------------------
cProc	LookUpTableEllipse,<NEAR,PUBLIC>,<di>
	parmW	Left
	parmW	Top
	parmW	Right
	parmW	Bottom
	localW	index
	localW	indexInc
;	localW	X
;	localW	Y
cBegin
	mov	ax,gs			;
	mov	ds,ax			;
	assumes ds,Data
;
	xor	ax,ax			;
	mov	evenWidth,ax		;
	mov	evenHeight,ax		;
;
	mov	ax,Right		;
	sub	ax,Left 		;
	shr	ax,1			;
	adc	evenWidth,0		;even width (non-integer center condition)
	or	ax,ax			;
	jz	LUTEExit		;
;
	mov	radiusX,ax		;
	add	ax,Left 		;
	mov	centerX,ax		;
;
	mov	ax,Bottom		;
	sub	ax,Top			;
	shr	ax,1			;
	adc	evenHeight,0		;even height (non-integer center condition)
	or	ax,ax			;
	jz	LUTEExit		;
;
	mov	radiusY,ax		;
	add	ax,Top			;
	mov	centerY,ax		;
;
; See if same circle
;
	mov	ax,radiusX		;
	mov	bx,radiusY		;
	cmp	ax,lastRadiusX		;
	jnz	short LUTENew		;
;
	cmp	bx,lastRadiusY		;
	jnz	short LUTENew		;
	jmp	LUTEExit		;
;
public	LUTENew
LUTENew:
	mov	lastRadiusX,ax		;
	mov	lastRadiusY,bx		;
;
; See if this is a circle
;
	mov	ax,radiusX		;
	cmp	ax,radiusY		;
	jnz	short @F		;
;
	call	MidpointCircle		;
	jmp	LUTEExit		;
;
; See if we can increment table index by more than 1
;
@@:	mov	ax,radiusX		;
	mov	bx,radiusY		;
	cmp	ax,bx			;
	jl	short @F		;
	mov	bx,ax			;
;
;	cmp	bx,0100h		;?max(a,b)<256
@@:	cmp	bx,0200h		;?max(a,b)<512
	ja	short LUTE1		;
;
	xor	dx,dx			;yes
	mov	ax,NumEntries		;
	div	bx			;
	mov	cx,04000h		;kind of BSR (bit scan reverse)
;
@@:	shl	ax,1			;
	jc	short @F		;
	shr	cx,1			;
	jmp	short @B		;
;
@@:	mov	indexInc,cx		;skip some indices
	jmp	short LUTE2		;

LUTE1:
	mov	indexInc,1		;NOT!

LUTE2:
	mov	index,NumEntries	;
	lea	si,SinCos		;
	lea	di,PGScratchBuffer	;
;
; Get X = a * cos(index)
;
LUTELoop:
	mov	ax,radiusX		;
	mov	dx,ax			;
	mov	bx,index		;
	cmp	bx,NumEntries-IEntries	;case: cos(index) = 1
	ja	short @F		;
;
	shl	bx,1			;
	add	si,bx			;
	mov	dx,cs:[si]		;
	sub	si,bx			;restore si
	mul	dx			;
	shl	ax,1			;
	adc	dx,0			;roundup from fraction
;
@@:	mov	cx,dx			;save integer portion of x
;
; Get Y = b * sin(index)
;
	mov	ax,radiusY		;
	mov	dx,ax			;
	mov	bx,index		;
	cmp	bx,IEntries		;case: sin(index) = 1
	jl	short @F		;
;
	sub	bx,NumEntries		;
	neg	bx			;
	shl	bx,1			;
	add	si,bx			;
	mov	dx,cs:[si]		;
	sub	si,bx			;restore si
	mul	dx			;
	shl	ax,1			;
	adc	dx,0			;roundup from fraction
;
@@:
;
; Process (X,Y)
;
	ESavePixel cx,dx,di		;keep (x,y) in an array
;
	mov	cx,index		;
	sub	cx,indexInc		;
	jnc	short @F		;
	jmp	short LUTELoopEnd	;
;
@@:	mov	index,cx		;
	jmp	short LUTELoop		;
;
LUTELoopEnd:
	mov	ax,-1			;
	ESavePixel ax,ax,di		;
;
LUTEExit:
cEnd
;
;
;-----------------------------------------------------------------------------
; SavePixel
;
; Desc: Used by MidpointCircle to save pixel coordinates.
;-----------------------------------------------------------------------------
CSavePixel	macro	y,x,reg ;reverse to make decreasing-x order
	mov	[reg],x
	mov	[reg+2],y
	add	reg,4
	endm
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; MidpointCircle
;
; Desc:
;	Draw a circle using integer midpoint circle scan-conversion algorithm
;	(variation of Bresenham's) described in Computer Graphics, 2nd ed.
;	In this implementation, all the pixel coordinates are saved in an array,
;	and processed later for both fill and outline operation.
; In:
;	radius <- radiusX
;	array  <- PGScratchBuffer
; Out:
;	array of (X,Y) pairs for one quadrant of a circle
; Note:
;	1) All X>=0 and Y>=0.
;	2) The array starts with (radiusX,0) as the 1st element and
;	    ends with (0,radiusX).
;	3) Only half the elements are actually calculated and the rest is
;	    reflected off across X=Y axis, i.e. (X,Y) -> (Y,X)
;	4) Both ends of the array are marked by -1.
;
;-----------------------------------------------------------------------------
cProc	MidpointCircle,<NEAR,PUBLIC>
cBegin
	assumes ds,Data
;
	lea	si,PGScratchBuffer	;
MCInit:
	xor	ax,ax			;x=0
	mov	bx,radiusX		;y=radius
	mov	di,1			;
	sub	di,bx			;d=1-radius
	mov	cx,3			;deltaE=3
	mov	dx,radiusX		;
	shl	dx,1			;
	sub	dx,5			;
	neg	dx			;deltaSE=-2*radius+5
;
	CSavePixel ax,bx,si		;
;
MCLoop:
	cmp	ax,bx			;while( y>x )
	jl	short @f		;
	jmp	short MCLoopEnd 	;
;
@@:	cmp	di,0			;if( d<0 )
	jge	short MCSelectSE	;
;
MCSelectE:
	add	di,cx			;d+=deltaE;
	add	cx,2			;deltaE+=2;
	add	dx,2			;deltaSE+=2;
	inc	ax			;x++;
	jmp	short @f		;
;
MCSelectSE:
	add	di,dx			;d+=deltaSE;
	add	cx,2			;deltaE+=+2;
	add	dx,4			;deltaSE+=4;
	inc	ax			;x++;
	dec	bx			;y--;
;
@@:
	CSavePixel ax,bx,si		;
	jmp	short MCLoop		;
;
; duplicate (x,y) -> (y,x)
;
MCLoopEnd:
	mov	cx,4			;
	mov	di,si			;
	sub	si,8			;point at the previous pixel
	cmp	ax,bx			;
	jz	MCDup			;
	sub	si,cx			;one before!
;
MCDup:
	mov	ax,[si] 		;
	cmp	ax,-1			;
	jz	short MCDupEnd		;
	mov	dx,[si+2]		;
	mov	[di],dx 		;
	mov	[di+2],ax		;
	sub	si,cx			;
	add	di,cx			;
	jmp	short MCDup		;
;
MCDupEnd:
	mov	ax,-1			;
	CSavePixel ax,ax,di		;indicate end of array
;
MCExit:
cEnd
;
;
;-----------------------------------------------------------------------------
; CircleFill
;
; Desc: Called from MidpointCircle.  Fills the circle.
;-----------------------------------------------------------------------------
cProc	CircleFill,<NEAR,PUBLIC>,<di>
cBegin
	mov	ax,gs			;
	mov	ds,ax			;
	assumes ds,Data
;
	lea	si,PGScratchBuffer	;
	mov	dx,-1			;
;
	cmp	evenHeight,1		;
	jz	CFEvenLoop		;
;
; First do a single line @y=0
;
	mov	bx,word ptr [si]
	mov	cx,word ptr [si+2]
	add	si,4

	CheckFIFOSpace THREE_SLOTS

	mov	ax,bx
	add	ax,ax
	add	ax,evenWidth		;set width W=2*x, 2*x+1 iff even width
	mov	wWidth,ax		;save it
	rol	eax,16			;get width in high dword
	mov	ax,1			;set height = 1
	EngineWrite B_RWIDTH_HEIGHT

	call	EScanline1		;

; Now the rest.
;
CFOddLoop:
	mov	bx,word ptr [si]	;
	cmp	bx,-1			;end of array?
	jz	CFExit			;
;
	mov	cx,word ptr [si+2]	;
	add	si,4			;
	cmp	cx,dx			;same scanline?
	jz	short CFOddLoop 	;
;
	CheckFIFOSpace FIVE_SLOTS

	push	cx			;save y for last y
	mov	ax,bx
	add	ax,ax
	add	ax,evenWidth		;set width W=2*x, 2*x+1 iff even width
	mov	wWidth,ax		;save it
	rol	eax,16			;get width in high dword
	mov	ax,1			;set height = 1
	EngineWrite B_RWIDTH_HEIGHT

	call	EScanline1

	call	EScanline2

	pop	dx			;recover last y
	jmp	CFOddLoop		;
;
; For even height
;
CFEvenLoop:
	mov	bx,word ptr [si]	;
	cmp	bx,-1			;end of array?
	jz	CFExit			;
;
	mov	cx,word ptr [si+2]	;
	add	si,4			;
	cmp	cx,dx			;same scanline?
	jz	short CFEvenLoop	;
;
	push	cx			;save y for last y
	inc	cx			;for even height only!

	CheckFIFOSpace FIVE_SLOTS

	mov	ax,bx
	add	ax,ax
	add	ax,evenWidth		;set width W=2*x, 2*x+1 iff even width
	mov	wWidth,ax		;save it
	rol	eax,16			;get width in high dword
	mov	ax,1			;set height = 1
	EngineWrite B_RWIDTH_HEIGHT

	call	EScanline1
;
	dec	cx			;restore y
	call	EScanline2
	pop	dx			;recover last y
;
	jmp	CFEvenLoop
	mov	InSrcBlt,0		;we're done with imagexfers
;
CFExit:
cEnd
;
;
cProc	EScanline1,<NEAR,PUBLIC>
cBegin
	assumes ds,Data
;
	mov	ax,centerX		;
	sub	ax,bx			;set current X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	add	ax,cx			;;set current Y=centerY+y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
cEnd
;
;
cProc	EScanline2,<NEAR,PUBLIC>
cBegin
	assumes ds,Data
;
	mov	ax,centerX		;
	sub	ax,bx			;set current X=centerX-x
	rol	eax,16			;get x in high dword
	mov	ax,centerY		;
	sub	ax,cx			;;set current Y=centerY+y
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
cEnd
;
;
sEnd	OutputSeg
end
.286c
