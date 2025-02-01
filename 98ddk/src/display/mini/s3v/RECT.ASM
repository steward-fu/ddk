;------------------------------------------------------------------------------
; RECT.ASM
; Copyright (c) 1996 S3 Incorporated
;------------------------------------------------------------------------------
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
; [RectFlags] equates
;
RF_OUTLINE	    equ     01h 	; bit 0 = Rectangle outline present
RF_RESERVED_1	    equ     02h 	; bit 1 = (reserved)
RF_RESERVED_2	    equ     04h 	; bit 2 = (reserved)
RF_RESERVED_3	    equ     08h 	; bit 3 = (reserved)
RF_CLIPLEFT	    equ     10h 	; bit 4 = Rectangle left edge clipped
RF_CLIPTOP	    equ     20h 	; bit 5 = Rectangle top edge clipped
RF_CLIPRIGHT	    equ     40h 	; bit 6 = Rectangle right edge clipped
RF_CLIPBOTTOM	    equ     80h 	; bit 7 = Rectangle bottom edge clipped
RF_CLIPMASK	    equ    0F0h 	; bits 7:4 = Rectangle clipping flags

;------------------------------------------------------------------------------
; E X T E R N
;------------------------------------------------------------------------------
;
externFP	BeginAccess		; ACCESS.ASM: Begin frame buff access
externFP	EndAccess		; ACCESS.ASM: End frame buffer access
externNP	O_DIBEngineToScreen	; OUTPUT.ASM: Return to DIB engine

;------------------------------------------------------------------------------
; D A T A
;------------------------------------------------------------------------------
;
sBegin		Data
;
externD 	dDestFmt		; VGA.ASM:  Bpp in CMD_SET format
externW 	wBpp			; INIT.ASM: Bits per pixel
externW 	wBpp_Byte		; INIT.ASM: Bytes per pixel (wBpp/8)
externW 	wMMIOSelector		; VGA.ASM:  Engine MMIO selector
externW 	wScreenWidth		; VGA.ASM:  Screen width resolution
externW 	wScreenHeight		; VGA.ASM:  Screen height resolution
externW 	wCursorType		; VGA.ASM:  Hardware/Software cursor
externW 	wScreenWidthBytes	; VGA.ASM:  Bytes per scan line
externB 	bInDDraw		; ENABLE:   In DirectDraw application
externB 	BWFlag			; INIT.ASM: Block write/Virge status
externB 	S3MobileData			;in MOBILE.ASM
;
sEnd		Data

;------------------------------------------------------------------------------
; C O D E
;------------------------------------------------------------------------------
;
createSeg _OUTPUT,OutputSeg,para,public,CODE
sBegin	OutputSeg
assumes cs,OutputSeg
.386
;
externB     S3Rop2TranslateTable	; OUTPUT.ASM: ROP -> Engine CMD xlation


;------------------------------------------------------------------------------
; PROCEDURE:	Rectangle_S3:
;
; DESCRIPTION:	Draw a filled rectangle with an optional border.
;
; ASSUMPTIONS:	None (all 2D graphics engine registers may have been modified
;		    by the 3D driver).
;
; CALLS:	BeginAccess
;		EndAccess
;
; ENTRY  REGS:	SS:SP = Stack frame (return address and parameters)
;		  ES: = Driver data selector
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	EAX, EBX, ECX, EDX
;
; NOTES:	1)  The Foreground Color is set as follows:
;		    a)	From the Brush struct for Solid and Color-patterned
;			style brushes.	The colors for these two cases are
;			located at different locations in the Physical Brush
;			Structure. (refer to the code and see documentation
;			in the RealizeObject procedure)
;		    b)	From the DrawMode struct for Mono-patterned style
;			brushes.
;		2)  For far pointer input parameters, only the selector
;		    (segment) is checked.  This is because protected mode
;		    selectors must be non-NULL to be valid.  It should also
;		    reduce the code size by 1 byte per compare and possibly
;		    reduce DWORD data alignment problems.
;		3)  Support for offscreen coordinates (negative, above maximum)
;		    has been added to the software clipping code.
;------------------------------------------------------------------------------
; Outlined Rectangle Boundaries (X1,Y1 & X2,Y2 are specified in lpPoints):
; Key: (* = Rectangle interior pixels drawn, o = Rectangle outline drawn)
; X1,Y1
;   oooooooooooooooooooooooooooo+   NOTE: The interior rectangle size drawn
;   o**************************o|	  includes the area specified by the
;   o**************************o|	  following points:
;   o**************************o|
;   o**************************o|	   X1+1 = Left edge   (1st pixel drawn)
;   o**************************o|	   Y1+1 = Top edge    (1st line  drawn)
;   o**************************o|	   X2-2 = Right edge  (last pixl drawn)
;   o**************************o|	   Y2-2 = Bottom edge (last line drawn)
;   o**************************o|
;   o**************************o|	  This is because Output (OS_RECTANGLE)
;   oooooooooooooooooooooooooooo|	  specifies that the rectangle drawn is
;   +---------------------------+	  up to, but not including the ending
;			      X2,Y2	  points (X2,Y2).
;------------------------------------------------------------------------------
; Non-Outlined Rectangle Boundaries (X1,Y1 & X2,Y2 are specified in lpPoints):
; Key: (* = Rectangle interior pixels drawn)
; X1,Y1
;   ***************************-+   NOTE: The interior rectangle size drawn
;   *************************** |	  includes the area specified by the
;   *************************** |	  following points:
;   *************************** |
;   *************************** |	   X1	= Left edge   (1st pixel drawn)
;   *************************** |	   Y1	= Top edge    (1st line  drawn)
;   *************************** |	   X2-2 = Right edge  (last pixl drawn)
;   *************************** |	   Y2-2 = Bottom edge (last line drawn)
;   *************************** |
;   *************************** |	  Note that the non-outlined rectangle
;   |				|	  exterior dimensions are smaller than
;   +---------------------------+	  the outlined rectangle's dimensions.
;			      X2,Y2	  (Ext Rect right/bottom = X2-2,Y2-2).
;------------------------------------------------------------------------------
;
cProc	Rectangle_S3, <FAR,PUBLIC,PASCAL,NODATA>, <esi, edi>

	parmD	lpDstDev		; Dest device (PDEVICE) struct far ptr
	parmW	style			; Output operation type (OS_RECTANGLE)
	parmW	count			; Rectangle point count
	parmD	lpPoints		; Points array (PTTYPE) far ptr
	parmD	lpPhysPen		; Physical pen (PPEN) far ptr
	parmD	lpPhysBrush		; Physical brush (PBRUSH) far ptr
	parmD	lpDrawMode		; Drawing mode (DRAWMODE) struc far ptr
	parmD	lpClipRect		; Clip rectangle (RECT) far ptr if != 0

	localV	ExteriorRect,%(SIZE RECT) ; Exterior rectangle dimension struct
	localV	InteriorRect,%(SIZE RECT) ; Interior rectangle dimension struct
	localD	EngineCmdSet		; Graphics engine CMD_SET command
	localW	BorderWidth		; Rectangle border width (if bordered)
	localW	BorderHeight		; Rectangle border height (if bordered)
	localW	BackgroundMode		; Background type (opaque, transparent)
	localW	LocalDeFlags		; local copy of lpDstDev->deFlags
	localB	RectFlags		; Rectangle outline/interior/clip flags
	localB	RectFlagsWordAlignment	; (WORD alignment for [RectFlags])
	localB	rect_rop		; rop function for rect. cmd
cBegin
	lfs	si,ss:[lpDstDev]	; FS:SI = lpDstDev dest device ptr
	mov	ax,DGROUP		; AX = driver data selector
	mov	gs,ax			; GS: = driver data selector

	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	mov	ax,fs:[si.deFlags]	; set dest dev busy, CF=1 if already
	mov	ss:[LocalDeFlags],ax	; save local copy of flags
	bts	fs:[si.deFlags],BUSY_BIT ; set dest dev busy, CF=1 if already
	jc	O_DIBEngineToScreen	; dest device busy, give to DIB engine

	mov	eax,gs:[dDestFmt]	; EAX = engine default command
	mov	ss:[EngineCmdSet],eax	; save in local variable

;
; Validate the exterior rectangle dimensions; if invalid, return error to GDI
;   Save the exterior rectangle dimensions for later software clipping
; NOTE: To avoid calculating the actual rectangle right/bottom boundaries all
;	over the code, the last pixel drawn rectangle edges are adjusted here.
; Parameter Validation: X1,Y1 >= 0,0	   (positive coordinates)
;			X2,Y2 >= X1,Y1	   (and therefore X2,Y2 > 0,0)
;			X2,Y2 <= Xmax,Ymax (and therefore X1,Y1 < Xmax,Ymax)
;
	lds	si,ss:[lpPoints]	; DS:SI = lpPoints struct ptr
	mov	ss:[RectFlags],0	; init local rectangle draw flags
	mov	ax,ds:[si.left] 	; AX = rect.left
	mov	bx,ds:[si.top]		; BX = rect.top
	mov	cx,ds:[si.right]	; CX = rect.right
	mov	dx,ds:[si.bottom]	; DX = rect.bottom
	dec	cx			; CX = rect.right,  last pixel drawn
	dec	dx			; DX = rect.bottom, last pixel drawn
	mov	di,0001h		; DI=ExteriorRect adjust if no outline
	cmp	ss:[lpPhysPen.sel],0	; physical pen (selector != NULL) ?
	je	short RExteriorRectSave ; no, adjust/save exterior rect coords
	lds	si,ss:[lpPhysPen]	; DS:SI = lpPhysPen PPEN struct ptr
	xor	di,di			; DI=ExteriorRect adj if outline (none)
	cmp	ds:[si.dpPenStyle],LS_SOLID ; solid rectangle outline style ?
	jne	RReturnToGDI		; no, let GDI draw rectangle/outline
	or	ss:[RectFlags],RF_OUTLINE ; flag rectangle outline draw needed
PLABEL RExteriorRectSave
	sub	cx,di			; CX = rect.right  last pixel drawn
	sub	dx,di			; DX = rect.bottom last pixel drawn
	cmp	cx,ax			; rect.right > rect.left ?
	jl	RReturnToGDI		; no, return to GDI as invalid
	cmp	dx,bx			; rect.bottom > rect.top ?
	jl	RReturnToGDI		; no, return to GDI as invalid
	mov	ss:[ExteriorRect.left],ax   ; save rect.left
	mov	ss:[ExteriorRect.top],bx    ; save rect.top
	mov	ss:[ExteriorRect.right],cx  ; save adjusted rect.right
	mov	ss:[ExteriorRect.bottom],dx ; save adjusted rect.bottom

;
; Validate the clipping rectangle dimensions (width > 0 and height > 0)
; NOTE: Coordinates below/beyond the screen dimensions are possible for either
;	the rectangle or the clipping rectangle.  To ensure that the software
;	clipping below does not exceed the screen's coordinates, the clipping
;	rectangle coordinates are forced to a range within the visible screen.
;
	xor	ax,ax			; AX = screen clip rect.left
	xor	bx,bx			; BX = screen clip rect.top
	mov	cx,gs:[wScreenWidth]	; CX = screen clip rect.right
	mov	dx,gs:[wScreenHeight]	; DX = screen clip rect.bottom
	cmp	ss:[lpClipRect.sel],0	; any clipping rectangle passed ?
	je	short RClipRectAdjBounds ; no, use full screen clip rectangle
	lds	si,ss:[lpClipRect]	; DS:SI = clip rectangle struct ptr
	cmp	ax,ds:[si.left] 	; screen rect.left > clip rect.left ?
	jg	short @F		; yes, use screen rect.left
	mov	ax,ds:[si.left] 	; no, AX = clip rect.left
@@:
	cmp	bx,ds:[si.top]		; screen rect.top > clip rect.top ?
	jg	short @F		; yes, use screen rect.top
	mov	bx,ds:[si.top]		; no, BX = clip rect.top
@@:
	cmp	cx,ds:[si.right]	; screen rect.right < clip rect.right ?
	jl	short @F		; yes, use screen rect.right
	mov	cx,ds:[si.right]	; no, CX = clip rect.right
@@:
	cmp	dx,ds:[si.bottom]	; scrn rect.bottom < clip rect.bottom ?
	jl	short @F		; yes, use screen rect.bottom
	mov	dx,ds:[si.bottom]	; no, DX = clip rect.bottom
@@:
	cmp	ax,cx			; clip rect.left < clip rect.right ?
	jge	RReturnToGDI		; ERROR: invalid clip rect dimensions
	cmp	bx,dx			; clip rect.top < clip rect.bottom ?
	jge	RReturnToGDI		; ERROR: invalid clip rect dimensions
PLABEL RClipRectAdjBounds
	dec	cx			; CX=clip rect.right  last pixel drawn
	dec	dx			; DX=clip rect.bottom last pixel drawn
;
; Perform software clipping on the rectangle to be rendered.  The clipping
;   edges (left/right/top/bottom) are tracked for determining which rectangle
;   outline line segments will be drawn (only those which are not clipped).
; NOTE: The clipping rectangle rect.right & rect.bottom have been decremented
;	to specify the last scanline/pixel drawn to match ExteriorRect points
;	for software clipping.
; Regs: AX = Clip rect.left
;	BX = Clip rect.top
;	CX = Clip rect.right-1	(clip rectangle last pixel drawn)
;	DX = Clip rect.bottom-1 (clip rectangle last pixel drawn)
;	DS:SI = lpClipRect clip rectangle ptr (if clipping rectangle present)
;
	xor	di,di			; DI = temp [RectFlags] clipping status
	cmp	ax,ss:[ExteriorRect.left] ; clip rect.left <= rect.left
	jle	short @F		  ; yes, skip soft clipping
	mov	ss:[ExteriorRect.left],ax ; no, save new rect.left
	or	di,RF_CLIPLEFT		  ; flag rect.left edge clipped
@@:
	cmp	bx,ss:[ExteriorRect.top]  ; clip rect.top <= rect.top
	jle	short @F		  ; yes, skip soft clipping
	mov	ss:[ExteriorRect.top],bx  ; no, save new rect.top
	or	di,RF_CLIPTOP		  ; flag rect.top edge clipped
@@:
	cmp	cx,ss:[ExteriorRect.right] ; clip rect.right >= rect.right
	jge	short @F		   ; yes, skip soft clipping
	mov	ss:[ExteriorRect.right],cx ; no, save new rect.right
	or	di,RF_CLIPRIGHT 	   ; flag rect.right edge clipped
@@:
	cmp	dx,ss:[ExteriorRect.bottom] ; clip rect.bottom >= rect.bottom
	jge	short @F		    ; yes, skip soft clipping
	mov	ss:[ExteriorRect.bottom],dx ; no, save new rect.bottom
	or	di,RF_CLIPBOTTOM	    ; flag rect.bottom edge clipped
@@:
;
; Do final check to ensure rectangle and clipping rectangle intersect (overlap)
;   If the rectangle and clipping rectangle do not intersect, return success
; Regs: DS:SI = lpClipRect clip rectangle ptr (if clipping rectangle present)
;	   DI = Temporary [RectFlags] clip status (saved during SW clipping)
;
	mov	ax,ss:[ExteriorRect.right]  ; AX = clipped rect.right
	mov	bx,ss:[ExteriorRect.bottom] ; BX = clipped rect.bottom
	cmp	ax,ss:[ExteriorRect.left] ; clipped rect.right >= rect.left ?
	jl	RExitSuccess		  ; no, skip draw as no rect intersect
	cmp	bx,ss:[ExteriorRect.top]  ; clipped rect.bottom >= rect.top ?
	jl	RExitSuccess		  ; no, skip draw as no rect intersect
	or	word ptr ss:[RectFlags],di ; update [RectFlags] with clip status

;
; If using the software cursor, exclude the cursor from clip rectangle
; Regs: DS:SI = lpClipRect clip rectangle ptr (if clipping rectangle present)
;
PLABEL RChkBeginSWCursor
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using software cursor ?
	je	RBeginSWCursorExclude	; yes, go exclude cursor from rect
PLABEL RBeginSWCursorExcludeDone	; (label for SW cursor exclude return)

	assumes es,nothing
;
; Convert the ROP2 code into a graphics engine ROP256 type
;
PLABEL RConvertRopCode
	les	di,ss:[lpDrawMode]	; ES:DI = lpDrawMode DRAWMODE struct ptr
	xor	ax,ax			; clear AX
	mov	bx,es:[di.Rop2] 	; BX = ROP2 operation
	dec	bx			; change to zero-based ROP2 operation
	add	bx,CodeOFFSET S3Rop2TranslateTable ; CS:BX = ROP->mix xlate tbl
	mov	al,cs:[bx]		; AX = translated ROP2 engine mix type
	mov	ss:[rect_rop],al	; save this for patch16
	shl	eax,17			; EAX = CMD_SET 256 ROPs field
	or	ss:[EngineCmdSet],eax	; CMD_SET = destFmt | clip | ROP code
	mov	ax,es:[di.bkMode]	; AX=backgnd mode (opaque, transp, etc)
	mov	ss:[BackgroundMode],ax	; save for later use (and used below)

;
; The graphics engine only supports OPAQUE background hatched or mono patterns
;   All other hatched or mono pattern background types are returned to GDI
; Regs: AX = Background mode (OPAQUE, TRANSPARENT, etc)
;
.ERRE	(BS_SOLID   EQ	0)		; ERROR: Assume solid	= 0
.ERRE	(BS_HOLLOW  EQ	1)		; ERROR: Assume hollow	> solid style
.ERRE	(BS_HATCHED EQ	2)		; ERROR: Assume hatched > hollow style
.ERRE	(BS_PATTERN EQ	3)		; ERROR: Assume pattern > hatched style
.ERRE	(BS_PATTERN EQ	MaxBrushStyle)	; ERROR: Assume pattern is last style

	assumes ds,nothing

	cmp	ax,OPAQUE		; opaque rectangle background ?
	je	short @F		; yes, skip brush style checks
	cmp	ss:[lpPhysBrush.sel],0	; is there a physical brush ?
	je	short @F		; no, skip brush style checks
	lds	si,ss:[lpPhysBrush]	; DS:SI = lpPhysBrush PBRUSH struct ptr
	mov	cx,ds:[si.dp8BrushStyle] ; CX = physical brush style
	cmp	cx,BS_HATCHED		; hatched brush style ?
	je	RReturnToGDI		; yes, can't handle, return to GDI
	cmp	cx,BS_PATTERN		; pattern brush style ?
	jne	short @F		; no, continue
	test	ds:[si.dp8BrushFlags],PATTERNMONO ; mono pattern brush ?
	jnz	RReturnToGDI		; yes, can't handle, return to GDI
@@:

;
; NOTE: The code is entering a critical section (exclusive access to the engine
;	is required by the display driver).  From this point in the code on,
;	the rectangle and/or border outline will be drawn by this code (no exit
;	or return to GDI/DIB engine is allowed until drawing has been completed
;	and the critical section has been ended).
;
; Set the engine source/destination base address and stride
;
	xor	eax,eax 			;default base
	mov	bx,gs:[wScreenWidthBytes]	;bytes per scanlines
	mov	dx,bx				;save dest stride
	shl	ebx,16				;destination stride high word
	mov	bx,dx				;source stride = dest stride

	assumes fs,nothing
	mov	fs,gs:[wMMIOSelector]	; FS: = MMIO selector for engine

	cmp	gs:bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	WaitDMA 			; wait for DMA to stop before MMIO access
@@:
	DisablePD gs,fs
	CheckFIFOSpace THREE_SLOTS
	EngineWrite B_DEST_BASE
	EngineWrite B_SRC_BASE
	EngineWrite B_DEST_SRC_STR ,, ebx
;
; Calculate the interior rectangle dimensions (allowing for outline, if any)
;   Clipped rectangle (RectFlags.RF_CLIPxxxx=1) outline edges are not drawn
; NOTE: If no outline, interior bounds are rect.right-1 & rect.bottom-1
;	If outline,    interior bounds are rect.right-2 & rect.bottom-2
;
.ERRE	(RF_OUTLINE EQ 01h)		; ERROR: Code assumes bit 0=RF_OUTLINE

PLABEL RCalcInteriorRect
	mov	al,ss:[RectFlags]	; AL = [RectFlags] clipping status
	mov	di,ax			; DI.0 = RF_OUTLINE adjustment bit
	mov	ah,al			; save in AH for test below
	and	di,RF_OUTLINE		; DI.0 = rect outline adjust count
	test	ah,RF_CLIPLEFT		; rect.left edge clip	(ZF=1 if not) ?
	setz	al			; AL = 01h if no clip (outline adjust)
	test	ah,RF_CLIPTOP		; rect.top edge clip	(ZF=1 if not) ?
	setz	bl			; BL = 01h if no clip (outline adjust)
	test	ah,RF_CLIPRIGHT 	; rect.right edge clip	(ZF=1 if not) ?
	setz	cl			; CL = 01h if no clip (outline adjust)
	test	ah,RF_CLIPBOTTOM	; rect.bottom edge clip (ZF=1 if not) ?
	setz	dl			; DL = 01h if no clip (outline adjust)
	and	ax,di			; AX = interior rect.left outline adjust
	and	bx,di			; BX = interior rect.top outline adjust
	and	cx,di			; CX = interior rect.right outline adj
	and	dx,di			; DX = interior rect.bottom outline adj
	neg	cx			; invert sign to subtract from right
	neg	dx			; invert sign to subtract from bottom
	add	ax,ss:[ExteriorRect.left]   ; AX = outline adjusted rect.left
	add	bx,ss:[ExteriorRect.top]    ; BX = outline adjusted rect.top
	add	cx,ss:[ExteriorRect.right]  ; CX = outline adjusted rect.right
	add	dx,ss:[ExteriorRect.bottom] ; DX = outline adjusted rect.bottom
	sub	cx,ax			; CX = InteriorRect width (0-based)
	jl	RDrawBorder		; no interior rectangle, go draw border
	sub	dx,bx			; DX = InteriorRect height (0-based)
	jl	RDrawBorder		; no interior rectangle, go draw border
	mov	ss:[InteriorRect.left],ax   ; save interior rect.left
	mov	ss:[InteriorRect.top],bx    ; save interior rect.top
	mov	ss:[InteriorRect.right],cx  ; save int. rect.width  (0-based)
	mov	ss:[InteriorRect.bottom],dx ; save int. rect.height (0-based)

	assumes ds,nothing
;
; Based on the brush style, jump to the apprpriate style handler
;
.ERRE	(BS_SOLID   EQ	0)		; ERROR: Assume solid	= 0
.ERRE	(BS_HOLLOW  EQ	1)		; ERROR: Assume hollow	> solid style
.ERRE	(BS_HATCHED EQ	2)		; ERROR: Assume hatched > hollow style
.ERRE	(BS_PATTERN EQ	3)		; ERROR: Assume pattern > hatched style
.ERRE	(BS_PATTERN EQ	MaxBrushStyle)	; ERROR: Assume pattern is last style

PLABEL RCheckBrushStyle
	cmp	ss:[lpPhysBrush.sel],0	; is there a brush ?
	je	RDrawBorder		; no, don't need to fill interior
	lds	si,ss:[lpPhysBrush]	; DS:SI = lpPhysBrush PBRUSH struct ptr
	mov	ax,ds:[si.dp8BrushStyle] ; AL = physical brush style
	mov	ah,ds:[si.dp8BrushFlags] ; AH = physical brush flags
	cmp	al,BS_HOLLOW		; hollow brush (nothing to draw) ?
	jb	RSolidBrushStyle	; no, BS_SOLID style, go check flags
	je	RDrawBorder		; yes, go draw border outline
	cmp	al,BS_PATTERN		; pattern brush ?
	jb	RHatchPatternBrush	; no, BS_HATCHED, go load brush pattern
	ja	RDrawBorder		; invalid brush, go draw border outline
;
; BS_PATTERN: Check whether drawing a monochrome or color pattern
; Regs: AL = BS_PATTERN
;	AH = lpPhysBrush.dp8BrushFlags (physical brush flags)
;	DS:SI = lpPhysBrush PBRUSH structure ptr
;
	test	ah,PATTERNMONO		; monochrome brush pattern flag ?
	jnz	RMonoPatternBrush	; yes, go process mono pattern brush

;------------------------------------------------------------------------------
; COLOR PATTERN BRUSH: The pattern is 8x8 elements that depend on the screen
;   color format (if 8 bits/pixel, the pattern is 8x8 bytes, etc.).
; Copy the color brush pattern into offscreen memory for engine draw on screen
; Regs: DS:SI = lpPhysBrush PBRUSH structure ptr
;	FS:   = Engine MMIO selector
;
PLABEL RColorPatternBrush
	add	si,offset dp8BrushBits	; DS:SI=lpPhysBrush.dp8BrushBits bitmap
	or	ss:[EngineCmdSet],CLRPATBLT ; set cmd for color pattern blit

	assumes es,nothing

	mov	es,gs:[wMMIOSelector]	; ES: = MMIO selector
	cld				; DF = 0, forward string movement
	mov	cx,gs:[wBpp_Byte]	; AX = bytes per pixel count
	mov	ax,((8 * 8) / 4)	; AX = 8bpp pattern xfer size in DWORDs
	shr	cx,1			; CX = power of 2 for >= 8bpp resolution
	shl	ax,cl			; AX = number of DWORDs to transfer
	mov	cx,ax			; CX = number of DWORDs to transfer
	mov	di,COLOR_PATTERN	; ES:DI = dest color pattern regs

	WaitEngineIdle es		; wait for idle, all FIFO slots empty

	rep	movsd			; transfer color pattern to engine mem
	jmp	RDrawInteriorRect	; go draw/fill interior rectangle


;------------------------------------------------------------------------------
; HATCHED PATTERN BRUSH: The pattern is 8x8 bits where each bit is expanded
;   into a color (for each bit, 0 = Background color, 1 = Foreground color).
; Copy the hatched brush pattern into offscreen memory for engine draw on scrn
; NOTE: Only the first byte of each DWORD is used for the hatched pattern.
; Regs: DS:SI = lpPhysBrush PBRUSH structure ptr
;	FS:   = Engine MMIO selector
;
PLABEL RHatchPatternBrush
	or	ss:[EngineCmdSet],MONOPATBLT ; set source = mono pattern in mem
	mov	ch,ds:[si.dp8BrushMask + 1Ch] ; CH = hatch pattern byte 7
	mov	cl,ds:[si.dp8BrushMask + 18h] ; CL = hatch pattern byte 6
	shl	ecx,16			; save ECX.H = hatch pattern bytes 7,6
	mov	ch,ds:[si.dp8BrushMask + 14h] ; CH = hatch pattern byte 5
	mov	cl,ds:[si.dp8BrushMask + 10h] ; ECX = hatch pattern bytes 7-4
	mov	ah,ds:[si.dp8BrushMask + 0Ch] ; AH = hatch pattern byte 3
	mov	al,ds:[si.dp8BrushMask + 08h] ; AL = hatch pattern byte 2
	shl	eax,16			; save EAX.H = hatch pattern bytes 3,2
	mov	ah,ds:[si.dp8BrushMask + 04h] ; AH = hatch pattern byte 1
	mov	al,ds:[si.dp8BrushMask + 00h] ; EAX = hatch pattern bytes 3-0
	mov	ebx,eax 		; EBX = hatch pattern bytes 3-0
	mov	edi,ds:[si.dp8BgColor]	; EDI = background color
	mov	esi,ds:[si.dp8FgColor]	; ESI = foreground color

	CheckFIFOSpace FOUR_SLOTS	; wait for FIFO slots available

	EngineWrite B_MONO_PAT_0 ,,ebx	; write EBX = hatch pattern bytes 3-0
	EngineWrite B_MONO_PAT_1 ,,ecx	; write ECX = hatch pattern bytes 7-4
	EngineWrite B_PAT_FG_CLR ,,esi	; write ESI = foreground color
	EngineWrite B_PAT_BG_CLR ,,edi	; write EDI = background color
	jmp	RDrawInteriorRect	; go draw/fill interior rectangle


;------------------------------------------------------------------------------
; MONOCHROME PATTERN BRUSH: The pattern is 8x8 bits where each bit is expanded
;   into a color (for each bit, 1 = Background color, 0 = Foreground color).
; NOTE: The monochrome foreground/background bit designations are reversed
;	from the typical designation (typical: 0 = background, 1 = foreground).
;	The foreground/background color settings are reversed to convert bits
;	back to the expected (typical) designations.
;
; Load mono pattern brush data into regs for transfer into offscreen memory
; NOTE: Only the first byte of each DWORD is used for the mono pattern.
; Regs: DS:SI = lpPhysBrush PBRUSH structure ptr
;	FS:   = Engine MMIO selector
;
PLABEL RMonoPatternBrush
	or	ss:[EngineCmdSet],MONOPATBLT  ; set mono pattern blit in cmd
	mov	ch,ds:[si.dp8BrushMono + 1Ch] ; CH = mono pattern byte 7
	mov	cl,ds:[si.dp8BrushMono + 18h] ; CL = mono pattern byte 6
	shl	ecx,16			      ; save ECX.H=mono patt bytes 7,6
	mov	ch,ds:[si.dp8BrushMono + 14h] ; CH = mono pattern byte 5
	mov	cl,ds:[si.dp8BrushMono + 10h] ; ECX = mono pattern bytes 7-4
	mov	ah,ds:[si.dp8BrushMono + 0Ch] ; AH = mono pattern byte 3
	mov	al,ds:[si.dp8BrushMono + 08h] ; AL = mono pattern byte 2
	shl	eax,16			      ; save EAX.H=mono patt bytes 3,2
	mov	ah,ds:[si.dp8BrushMono + 04h] ; AH = mono pattern byte 1
	mov	al,ds:[si.dp8BrushMono + 00h] ; EAX = mono pattern bytes 3-0
	mov	ebx,eax 		      ; EBX = mono pattern bytes 3-0
	lds	si,ss:[lpDrawMode]	; DS:SI = lpDrawMode DRAWMODE struct ptr
	mov	edi,ds:[si.bkColor]	; EDI = background color
	mov	esi,ds:[si.TextColor]	; ESI = foreground color

	CheckFIFOSpace FOUR_SLOTS	; wait for FIFO space available

	EngineWrite B_MONO_PAT_0 ,,ebx	; write mono pattern brush DWORD 0
	EngineWrite B_MONO_PAT_1 ,,ecx	; write mono pattern brush DWORD 1
	EngineWrite B_PAT_BG_CLR ,,esi	; write backgnd color (see notes above)
	EngineWrite B_PAT_FG_CLR ,,edi	; write foregnd color (see notes above)
	jmp	RDrawInteriorRect	; go draw/fill interior rectangle

;
; BS_SOLID: Check if the solid brush is a solid color or a dithered color brush
; Regs: AL = BS_SOLID
;	AH = lpPhysBrush.dp8BrushFlags (physical brush flags)
;	DS:SI = lpPhysBrush PBRUSH structure ptr
;
PLABEL	RSolidBrushStyle
	test	ah,COLORSOLID		; solid color brush ?
	jz	RColorPatternBrush	; no, go render dithered color pattern

;------------------------------------------------------------------------------
; SOLID COLOR BRUSH: Fill the rectangle with the specified foreground color
;   using the background mode ROP2 operation code.
; Regs: DS:SI = lpPhysBrush PBRUSH structure ptr
;
PLABEL	RSolidBrush
	or	ss:[EngineCmdSet],(RECT_FILL + bPAT_Mono) ; rect fill, mono patt
	mov	ebx,dword ptr [si.dp8BrushBits] ; EBX = brush solid color

	CheckFIFOSpace ONE_SLOT 	; wait for FIFO space available

	EngineWrite B_PAT_FG_CLR ,,ebx	; write EBX = solid foreground color

.ERRE	(CodeOFFSET $ EQ (CodeOFFSET RDrawInteriorRect)) ; ERR: Assume drop in

;
; Draw the interior rectangle
; NOTE: The following conditions must be setup before executing this code
;	1)  3 DWORD FIFO slots must have been made available
;	2)  All engine regs but the rect top/left, rect height/width, and rect
;	    draw cmd have been set
; NOTE: The graphics engine's rectangle height register value is 1-based
;	(not 0-based) so the interior rectangle height is adjusted here.
; Regs: SS:[EngineCmdSet] = Engine draw command to send
;	FS: = Engine MMIO selector
;
PLABEL RDrawInteriorRect
	mov	esi,dword ptr ss:[InteriorRect.left]  ; ESI=inter. top/left pos
	mov	edi,dword ptr ss:[InteriorRect.right] ; EDI=inter. width/height
	ror	esi,16			; ESI.H=int rect left pos, ESI.L=top pos
	ror	edi,16			; EDI.H=int rect width, EDI.L=height
	mov	ebx,ss:[EngineCmdSet]	; EBX = engine rect draw cmd
	inc	di			; DI = interior rect height (1-based)

if patch16
	CheckFIFOSpace FIVE_SLOTS	; wait for FIFO slots available
else
	CheckFIFOSpace THREE_SLOTS	; wait for FIFO slots available
endif

	EngineWrite B_RDEST_XY	    ,,esi ; write ESI=rect top/left position
if patch16
	test	BWFlag,08h		;ViRGE/VX running fast page mode ?
	jz	no_pat_16		;no
	cmp	ss:[rect_rop],0f0h	; rop = f0 ?
	jne	no_pat_16
	push	edi
	and	edi,0ffff0000h
	cmp	edi,000190000h
	jle	@F
	mov	edi,000190000h
@@:
	mov	di,1
	EngineWrite B_RWIDTH_HEIGHT ,,edi ; write EDI=width/height 0/1-based
	WriteCMDAutoOFF B_CMD_SET   ,,ebx ; write EBX = rect draw command
	pop	edi
no_pat_16:
endif
	EngineWrite B_RWIDTH_HEIGHT ,,edi ; write EDI=width/height 0/1-based
	WriteCMDAutoOFF B_CMD_SET   ,,ebx ; write EBX = rect draw command

IFDEF	VIRGE_REVAB_PATCH ;-------------
;
; Virge Rev A/B: Rectangle/line operations leave the engine in an indeterminate
;   state.  Perform a BitBlt operation to leave the engine in a known state.
;
	CheckFIFOSpace FOUR_SLOTS, fs	; wait for FIFO space available

	xor	eax,eax 		; zero X,Y
	EngineWrite B_RSRC_XY, fs	; write EAX = bitblt source X,Y
	EngineWrite B_RDEST_XY, fs	; write EAX = bitblt dest   X,Y
	inc	ax			; EAX = width/height
	EngineWrite B_RWIDTH_HEIGHT, fs ; write EAX = bitblt width/height
	mov	eax,gs:[dDestFmt]	; EAX = engine bpp default value
	or	eax,(07980000h + bDraw + CPUAlign_dword + bFirst_dword) ;
	EngineWrite B_CMD_SET, fs	; write EAX=bitblt restores engine state
ENDIF ; VIRGE_REVAB_PATCH ;-------------

;
; Draw the rectangle border outline (vertical/horizontal lines) if required
; NOTE: Some rectangle edges may be clipped; if so, no outline is drawn for
;	those edges.  If all edges are clipped, the outline draw is bypassed.
;
.ERRE RF_CLIPMASK EQ RF_CLIPLEFT OR RF_CLIPTOP OR RF_CLIPRIGHT OR RF_CLIPBOTTOM

PLABEL RDrawBorder
	mov	al,ss:[RectFlags]	; AL = rectangle flags
	test	al,RF_OUTLINE		; rectangle outline required ?
	jz	REndCriticalSection	; no, skip rectangle outline draw
	and	al,RF_CLIPMASK		; AL = rectangle clipping status
	cmp	al,RF_CLIPMASK		; all rectangle edges clipped ?
	je	REndCriticalSection	; yes, skip rectangle outline draw

;
; Calculate the rectangle outline dimensions
; NOTE: The rectangle exterior dimensions have been validated above after the
;	soft clipping was performed so no extra checking is needed here.
; NOTE: The graphics engine's rectangle height register value is 1-based
;	(not 0-based) so the border height is adjusted here.
;
	mov	ax,ss:[ExteriorRect.right]  ; AX = rect.right
	mov	bx,ss:[ExteriorRect.bottom] ; BX = rect.bottom
	sub	ax,ss:[ExteriorRect.left]   ; AX = rect width to draw
	sub	bx,ss:[ExteriorRect.top]    ; BX = rect height to draw
	inc	bx			; BX = 1-based rect height to draw
	mov	ss:[BorderWidth],ax	; save border width,  0-based
	mov	ss:[BorderHeight],bx	; save border height, 1-based

;
; Set the rectangle border/outline foreground pen color
;
	les	di,ss:[lpPhysPen]	; ES:DI = lpPhysPen struct ptr

	CheckFIFOSpace ONE_SLOT, fs	; wait for FIFO space available

	mov	eax,es:[di.dpPenColor]	; EAX = rectangle outline pen color
	EngineWrite B_PAT_FG_CLR	; write EAX = rect outline pen color

;
; Draw the rectangle outline lines using shared X/Y coords (if present)
; NOTE: This code sequence is used for all rectangle outline edges.  As each
;	line is drawn, its RF_CLIPxxxx status bit is set to exclude it.
; NOTE: The graphics engine's rectangle height register value is 1-based
;	(not 0-based) so the border rectangle height is adjusted here.
;
	mov	ebx,ss:[EngineCmdSet]	; EBX = engine rect draw cmd
	or	ebx,(RECT_FILL + bPAT_Mono) ; EBX = draw rect/line, mono color

PLABEL RDrawBorderLines
	mov	al,ss:[RectFlags]	; AL = [RectFlags] clipping status
	mov	edi,dword ptr ss:[ExteriorRect.left] ; EDI.L=X.left, EDI.H=Y.top
	mov	ecx,00010000h		; ECX.H=top/bottom line height 1-based

PLABEL RSetTopBorderParms
	mov	ah,RF_CLIPTOP		; AH = top edge clip status bit
	mov	cx,ss:[BorderWidth]	; ECX.L = border width, ECX.H = 0001h
	test	al,ah			; rect.top edge clipped ?
	jz	short @F		; no, go draw rect outline top edge
PLABEL RSetBottomBorderParms
	mov	ah,RF_CLIPBOTTOM	; AH = bottom edge clip status bit
	ror	edi,16			; EDI.H = outline rect.left
	mov	di,ss:[ExteriorRect.bottom] ; EDI.L = outline rect.bottom coord
	ror	edi,16			; EDI.H=line.bottom, EDI.L=line.left
	test	al,ah			; rect.bottom edge clipped ?
	jz	short @F		; no, go draw rect outline bottom edge
PLABEL RSetLeftBorderParms
	mov	ah,RF_CLIPLEFT		; AH = left edge clip status bit
	mov	cx,ss:[BorderHeight]	; ECX.L = border line height
	mov	edi,dword ptr ss:[ExteriorRect.left] ; EDI.L=X.left, EDI.H=Y.top
	shl	ecx,16			; ECX.H=line height, ECX.L=width 0-based
	test	al,ah			; rect.left edge clipped ?
	jz	short @F		; no, go draw rect outline left edge
PLABEL RSetRightBorderParms
	mov	ah,RF_CLIPRIGHT 	; AH = right edge clip status bit
	mov	di,ss:[ExteriorRect.right] ; DI = rectangle right edge coord

;
; Draw the rectangle outline edge
; Regs:  AH = RF_CLIPxxxx outline edge being drawn
;	EBX = Graphics engine line/rectangle draw command
;	ECX = Rect/line width/height (ECX.L = width, ECX.H = height)
;	EDI = Rect/line source X/Y coordinates (EDI.L = Xcoord, EDI.H = Ycoord)
;
@@:
	or	ss:[RectFlags],ah	; flag rectangle outline edge as drawn
	ror	edi,16			; EDI.L=dest line.X, EDI.H=dest line.Y
if patch16
	test	BWFlag,08h		;ViRGE/VX running fast page mode ?
	jz	nopatch16		;no
	cmp	ss:[rect_rop],0f0h	; rop = f0 ?
	jne	nopatch16
	push	ecx
	or	cx,cx			;drawing vertical lines ?
	jnz	@F			;no. it's horz. lines.
	ror	ecx,16
	mov	cx,1			;draw 1 pixel only for vertical line.

	CheckFIFOSpace THREE_SLOTS	; wait for FIFO slots available

	EngineWrite B_RDEST_XY	    ,,edi ; write EDI = rect origin X/Y coords
	EngineWrite B_RWIDTH_HEIGHT ,,ecx ; write ECX = rect height/width
	WriteCMDAutoOFF B_CMD_SET   ,,ebx ; write EBX=draw rect outline segment
	jmp	short patch16done
@@:
	cmp	cx,19			;width <= 20
	jle	@F
	mov	cx,19
@@:
	ror	ecx,16			;draw min(width, 20)x1 rect
	CheckFIFOSpace THREE_SLOTS	;

	EngineWrite B_RDEST_XY	    ,,edi ; write EDI = rect origin X/Y coords
	EngineWrite B_RWIDTH_HEIGHT ,,ecx ; write ECX = rect height/width
	WriteCMDAutoOFF B_CMD_SET   ,,ebx ; write EBX=draw rect outline segment
patch16done:
	pop	ecx
nopatch16:
endif
	ror	ecx,16			; ECX.L=rect height, ECX.H =rect width

	CheckFIFOSpace THREE_SLOTS	; wait for FIFO slots available

	EngineWrite B_RDEST_XY	    ,,edi ; write EDI = rect origin X/Y coords
	EngineWrite B_RWIDTH_HEIGHT ,,ecx ; write ECX = rect height/width
	WriteCMDAutoOFF B_CMD_SET   ,,ebx ; write EBX=draw rect outline segment

	mov	al,ss:[RectFlags]	; AL = [RectFlags] clipping status
	and	al,RF_CLIPMASK		; AL = rectangle clipping status
	cmp	al,RF_CLIPMASK		; all rect edges clipped/lines drawn ?
if patch16
	jne	RDrawBorderLines	; no, go draw the next border line
else
	jne	short RDrawBorderLines	; no, go draw the next border line
endif

IFDEF	VIRGE_REVAB_PATCH ;-------------
;
; Virge Rev A/B: Rectangle/line operations leave the engine in an indeterminate
;   state.  Perform a BitBlt operation to leave the engine in a known state.
;
       CheckFIFOSpace FOUR_SLOTS, fs	; wait for FIFO space available

	xor	eax,eax 		; zero X,Y
	EngineWrite B_RSRC_XY, fs	; write EAX = bitblt source X,Y
	EngineWrite B_RDEST_XY, fs	; write EAX = bitblt dest   X,Y
	inc	ax			; EAX = width/height
	EngineWrite B_RWIDTH_HEIGHT, fs ; write EAX = bitblt width/height
	mov	eax,gs:[dDestFmt]	; EAX = engine bpp default value
	or	eax,(07980000h + bDraw + CPUAlign_dword + bFirst_dword) ;
	EngineWrite B_CMD_SET, fs	; write EAX=bitblt restores engine state
ENDIF ; VIRGE_REVAB_PATCH ;-------------

;
; End of code critical section (release exclusive engine access)
;
PLABEL REndCriticalSection
;
; If using a software cursor, end cursor exclusion
;
PLABEL RChkEndSWCursor
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using a software cursor ?
	je	REndSWCursorExclude ; yes, end software cursor exclusion
PLABEL REndSWCursorExcludeDone
	mov	es,gs:[wMMIOSelector]	; ES: = engine MMIO selector
	EnablePD
;
RExitSuccess:
	mov	ax,1			; AX = RECT success
					; (drop into exit code)
	assumes ds,nothing
RExit:
	lds	si,ss:[lpDstDev]	; DS:SI=lpDstDev dest device struct ptr
	and	ds:[si.deFlags],NOT BUSY ; flag destination device as free

cEnd ;	Rectangle_S3

PLABEL RReturnToGDI
	mov	ax,-1			; AX = not handled, return error to GDI
	jmp	short RExit		;


;------------------------------------------------------------------------------
; Exclude software cursor from rectangle to be drawn
; Regs: DS:SI = lpClipRect clip rectangle ptr (if clipping rectangle present)
;
PLABEL RBeginSWCursorExclude
	mov	eax,ss:[lpDstDev]	; EAX = destination device far ptr
	xor	cx,cx			; CX = full screen rect.left
	xor	dx,dx			; DX = full screen rect.top
	mov	bx,gs:[wScreenWidth]	; BX = full screen rect.right
	mov	di,gs:[wScreenHeight]	; DI = full screen rect.bottom
	cmp	ss:[lpClipRect.sel],0	; any clipping rectangle passed ?
	je	short @F		; no, use full screen dimensions
	mov	cx,ds:[si.left] 	; CX = clip rect.left
	mov	dx,ds:[si.top]		; DX = clip rect.top
	mov	bx,ds:[si.right]	; BX = clip rect.right
	mov	di,ds:[si.bottom]	; DI = clip rect.bottom
@@:
	push	eax			; PDevice
	push	cx			; rect.left
	push	dx			; rect.top
	push	bx			; rect.right
	push	di			; rect.bottom
	push	CURSOREXCLUDE		; Flags
	call	BeginAccess		; exclude SW cursor from rectangle draw
	jmp	RBeginSWCursorExcludeDone ; continue rectangle output code

;------------------------------------------------------------------------------
; End software cursor exclusion from rectangle that has been drawn
;
PLABEL REndSWCursorExclude
	mov	eax,ss:[lpDstDev]	; EAX = destination device far ptr
	push	eax			; PDevice
	push	CURSOREXCLUDE		; flags
	call	EndAccess		; end SW cursor exclusion
	jmp	REndSWCursorExcludeDone ; continue rectangle output code

sEnd	OutputSeg
end
