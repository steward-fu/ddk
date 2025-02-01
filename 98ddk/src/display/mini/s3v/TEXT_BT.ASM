;----------------------------------------------------------------------------
; TEXTMM2.ASM
; Copyright (c) 1992 Microsoft Corporation
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
patch16		equ	1
	.xlist
DOS5=1
	include cmacros.inc
	include dibeng.inc
	include macros.inc
	include S3.inc			;S3 specific defines
	incDrawMode = 1
	incFont = 1
	include gdidefs.inc
	.list
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externD dCmd			;in s3blt.asm
	externW wCursorType
	externW FullScreenClipRect	;rectangle that defines entire Bitmap
	externW wScreenWidthBytes	     ;bytes per scanlines
	externW wMMIOSelector
	externD dDestFmt
	externB InSrcBlt		;source blts in progress
	externB bAUTOEnable		;autoexec flag
	externB bInDDraw
	externB	BWFlag                  ;in INIT.ASM
sEnd	Data


;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
	.386

;---------------------------------------------------------------------------
; DrawTextBitmapMM2 - draw the mono bitmap rectangle to given coordinates
; and color. This mono Bitmap is the realized text string. Clips to given
; clip rectangle.
;---------------------------------------------------------------------------
cProc	DrawTextBitmapMM2_BT,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpDevice		;Destination device
	parmD	lpMonoBuffer
	parmW	Flags
	parmD	BgColor
	parmD	FgColor
	parmW	x			;Left origin of string bitmap
	parmW	y			;Top  origin of string bitmap
	parmW	WidthBytes		;width of string bitmap (bytes)
	parmW	Height			;height of string bitmap
	parmD	lpClipRect		;Clipping rectangle

	localW	DeviceFlags
	localW	wAdjust 		;#bytes to adjust for dword-aligned
cBegin
	mov	ax,DGROUP
	mov	gs,ax
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	les	di,lpDevice
	bts	es:[di].deFlags,BUSY_BIT
	jc	DTBMM2_Ret

	mov	ax,wMMIOSelector
	mov	es,ax			;es --> MMIO registers

	cmp	bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	WaitDMA	es			; wait for DMA to stop before MMIO access
@@:

        WaitEngineIdle es               ;better wait for engine idle

	CheckFIFOSpace SEVEN_SLOTS es
	xor	eax,eax
	EngineWrite B_SRC_BASE es
	EngineWrite B_DEST_BASE es

	mov	ax,wScreenWidthBytes		;bytes per scanlines
	mov	dx,ax
	shl	eax,16				;destination stride
	or	ax,dx				;source stride

	EngineWrite B_DEST_SRC_STR es

	mov	eax,00000001h		; EAX = width/height, source/dest X,Y
	EngineWrite B_RWIDTH_HEIGHT, es ; write EAX = bitblt width/height
	EngineWrite B_RSRC_XY, es	; write EAX = bitblt source X,Y
	EngineWrite B_RDEST_XY, es	; write EAX = bitblt dest   X,Y
	mov	eax,dDestFmt
	or	eax,(07980000h + bDraw + CPUAlign_dword + bFirst_dword) ;
	EngineWrite B_CMD_SET, es	; write EAX=bitblt restores engine state
;@@:
	mov	ebx,MONOSRCBLT+SRCCOPY+bClip_Enable+CPUAlign_dword
	or	ebx,dDestFmt

	test	Flags,1 		;transparent
	jz	short @F		;no
	or	ebx,bTransparent	;enable bit
@@:
	CheckFIFOSpace SIX_SLOTS, es

	lds	di,lpClipRect
	mov	ax,[di] 		;set left
	rol	eax,16			;in high dword
	mov	ax,[di+4]		;set right
	dec	ax			;
	EngineWrite B_CLIP_L_R, es	;set X-axis clipping

	mov	ax,[di+2]		;set top
	mov	cx,ax
	rol	eax,16			;in high dword
	mov	ax,[di+6]		;set bottom
	dec	ax			;
	EngineWrite B_CLIP_T_B, es	;set Y-axis clipping

	mov	dx,y
	add	dx,Height
	dec	dx
	cmp	dx,ax		;exceed bottom?
	jle	short @F	;N, continue
	sub	dx,ax
	sub	Height,dx	;Y, clip excess scan(s)
@@:
	mov	dx,y
	mov	wAdjust,0
	cmp	dx,cx		;exceed top?
	jge	short @F	;N, continue
	sub	cx,dx
	mov	ax,cx
	mul	WidthBytes
	sub	Height,cx	;Y, clip excess scan(s)
	add	y,cx
	mov	wAdjust,ax
@@:
	mov	eax,FgColor
	EngineWrite B_SRC_FG_CLR, es

	mov	eax,BgColor
	EngineWrite B_SRC_BG_CLR, es

	lds	si,lpMonoBuffer 	;ds:si points to mono bitmap
	add	si,wAdjust

DTB_SetAutoCmd:
;
;ebx = cmd
;ds:si -> pts to mono bitmap
;

	mov	ax,WidthBytes		;
	mov	cx,ax
	shl	ax,3			;
	dec	ax			;width--
;### patch
if	0
or	ax,31
endif
	rol	eax,16			;in high dword
	mov	ax,Height		;
	EngineWrite B_RWIDTH_HEIGHT, es

	mov	ax,x			;get destx
	rol	eax,16			;in high dword
	mov	ax,y			;get desty in low dword
	EngineWrite B_RDEST_XY, es

	mov	InSrcBlt,0FFh		;we're going to do image transfers
	WaitEngineIdle es		;better wait for engine idle

	WriteCMDAutoOFF B_CMD_SET, es, ebx

	mov	ax,WidthBytes		;
	mov	bx,Height
;
;ax = line to line increment
;bx = yExt (line loop counter)
;ds:si -> 1st src byte to xfer
;cx = #bytes to transfer per scanline
;
	mov	dx,cx			;get # of bytes to do.
	shr	dx,2			;dx = # of dword to xfer
	and	cx,3			;cx = 0,1,2,3 odd bytes
	jz	short @f		;no extra bytes
	inc	dx			;one more dword to xfer
@@:

@@:
	xor	di,di			;IMAGE_XFER
	push	si
	mov	cx,dx			;dx = #dwords to xfer/scanline
;###
if	1;pci_fix
push	dx
MonoTextLoop_1:
mov	cx,dx
cmp	dx,X_CNT
jle	m2
mov	cx,X_CNT
m2:
sub	dx,cx
CheckFIFOSpace SIXTEEN_SLOTS es
	rep	movsd			;move it
test	dx,dx
jnz	MonoTextLoop_1
pop	dx
endif
	pop	si
	add	si,ax			;ptr to next scanline
	dec	bx			;height--
	jnz	short @b		;more lines to xfer

DTBSetDestxy:
	mov	InSrcBlt,0		;we're done with image transfers
	les	di,lpDevice
	and	es:[di].deFlags,not BUSY

DTBMM2_Ret:
	mov	ax,1
cEnd

;---------------------------------------------------------------------------
; DrawORectMM2 - draw a rectangle to given coordinates and color.
; Clips to given clip rectangle.
;
;---------------------------------------------------------------------------
cProc	DrawOrectMM2_BT,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpDevice		;Destination device
	parmD	BgColor
	parmW	x			;Left origin of orect
	parmW	y			;Top  origin of orect
	parmW	xExt			;width of orect
	parmW	yExt			;height of orect
	localW	DeviceFlags
cBegin

	les	di,lpDevice
	mov	ax,DGROUP
	mov	ds,ax
	mov	gs,ax
	assumes ds,Data
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

;###
	bts	es:[di].deFlags,BUSY_BIT
	jc	DOMM2_Ret

	mov	ax,wMMIOSelector	;
	mov	fs,ax			;fs:[0] --> MMIO region

	cmp	bInDDraw,0		;is DDraw active?
	je	short @F		;N, skip
        WaitDMA                         ; wait for DMA to stop before MMIO access
@@:

        WaitEngineIdle fs               ;better wait for engine idle

	CheckFIFOSpace SEVEN_SLOTS, fs
	xor	eax,eax
	EngineWrite B_SRC_BASE
	EngineWrite B_DEST_BASE

	mov	ax,wScreenWidthBytes		;bytes per scanlines
	mov	dx,ax
	shl	eax,16				;destination stride
	or	ax,dx				;source stride

	EngineWrite B_DEST_SRC_STR


	mov	eax,00000001h		; EAX = width/height, source/dest X,Y
	EngineWrite B_RWIDTH_HEIGHT, fs ; write EAX = bitblt width/height
	EngineWrite B_RSRC_XY, fs	; write EAX = bitblt source X,Y
	EngineWrite B_RDEST_XY, fs	; write EAX = bitblt dest   X,Y
	mov	eax,dDestFmt
	or	eax,(07980000h + bDraw + CPUAlign_dword + bFirst_dword) ;
	EngineWrite B_CMD_SET, fs	; write EAX=bitblt restores engine state
;@@:
	mov	edi,BgColor
	mov	ax,x
	mov	bx,y
	mov	cx,xExt

	CheckFIFOSpace FOUR_SLOTS
	EngineWrite B_PAT_FG_CLR, fs, edi

	dec	cx			;width--
if patch16
	test	BWFlag,08h
	jz	no_patch_16
	push	eax
	push	ebx
	push	ecx
	cmp	cx,19
	jle	@F
	mov	cx,19
@@:	
	rol	ecx,16			;width in high dword
	mov	cx,1	 		;get height in high dword
	EngineWrite B_RWIDTH_HEIGHT, fs, ecx

	rol	eax,16			;destx in high dword
	mov	ax,bx			;desty in low dword
	EngineWrite B_RDEST_XY

	mov	edx,RECT_SOLID
	or	edx,dDestFmt
	WriteCMDAutoOFF B_CMD_SET, fs, edx
	pop	ecx
	pop	ebx
	pop	eax
	CheckFIFOSpace THREE_SLOTS
no_patch_16:
endif
	rol	ecx,16			;width in high dword
	mov	cx,yExt 		;get height in high dword
	EngineWrite B_RWIDTH_HEIGHT, fs, ecx

	rol	eax,16			;destx in high dword
	mov	ax,bx			;desty in low dword
	EngineWrite B_RDEST_XY

mov	edx,RECT_SOLID
or	edx,dDestFmt
	WriteCMDAutoOFF B_CMD_SET, fs, edx

;###patch
CheckFIFOSpace FOUR_SLOTS, fs
mov	eax,00000001h
EngineWrite B_RWIDTH_HEIGHT, fs
EngineWrite B_RSRC_XY, fs
EngineWrite B_RDEST_XY, fs
mov	eax,07980000h+bDraw+CPUAlign_dword+bFirst_dword
or	eax,dDestFmt
EngineWrite B_CMD_SET, fs

	les	di,lpDevice		;ES:DI -> lpDevice
	and	es:[di].deFlags,not BUSY

DOMM2_Ret:
	mov	ax,1
cEnd

;---------------------------------------------------------------------------
; RowTextMM2 - Handles new 4x style fonts directly.
; WARNING:  This code DOES NOT WORK with SOFTWARE CURSORS. Strblt.asm
; does not call this code if the driver is using a software cursor.
;
; To add software cursor capability:
;
;   In opaque mode -- union all of the rectangle's in the lpOpaqueRect list,
;		      clip to the clip rect, and call exclude, unexclude
;		      (one of these rectangles is the text bounding box).
;
;   In transparent mode -- Same as above with the additon of the
;			   rectangle pointed to by lpXform (this is the
;			   text bounding box).
;---------------------------------------------------------------------------
cProc	RowTextMM2_BT,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpDevice		;Destination device
	parmW	_x			;Left origin of string
	parmW	y			;Top  origin of string
	parmD	lpClipRect		;Clipping rectangle
	parmD	lpString		;The string itself
	parmW	_nCount 		;Number of characters in the string
	parmD	lpFont			;Font to use
	parmD	lpDrawMode		;Drawmode structure to use
	parmD	lpXform 		;Current text transform
	parmD	lpDx			;Widths for the characters
	parmD	lpOpaqueRect		;Opaquing rectangle
	parmW	etoOptions		;ExtTextOut options

	localD	dBgColor
	localD	dFgColor
	localD	CharIndex
	localV	rcClip, %(size RECT)		; Clipping rect
	localW	x
	localW	nCount
	localW	wCursorStatus
	localW	wBkMode
	localW	wMaxRHS
	localW	DeviceFlags
	localW	wFlag				;flag for looping
	localW	wWidth
	localW	wHeight
	localW	wDestx
	localW	wDesty
	localB	bSwitchBack
	localD	lCmd
	localD	dLRClip
	localD	dTBClip
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	mov	ax,DGROUP
	mov	es,ax
	mov	ax,_x
	mov	bx,_nCount
	mov	x,ax				;insure that textout
	mov	nCount,bx			;is restartable

	lgs	si,lpDevice			;GS:SI -> lpDevice
	mov	wFlag,0 			;initialize flag
	mov	InSrcBlt,0FFh			;prepare to do image xfers

	bts	gs:[si].deFlags,BUSY_BIT
	jc	RTMM2_Exit

	mov	ax,es:wMMIOSelector
	mov	fs,ax				;fs:-->MMIO regs
	assumes fs,nothing

	cmp	es:bInDDraw,0		;is DDraw active?
	je	short @F		;N, skip
        WaitDMA                         ; wait for DMA to stop before MMIO access
@@:
if	1;pci_fix
        WaitEngineIdle fs
	CheckFIFOSpace SIXTEEN_SLOTS
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

	mov	eax,00000001h		; EAX = width/height, source/dest X,Y
	EngineWrite B_RWIDTH_HEIGHT, fs ; write EAX = bitblt width/height
	EngineWrite B_RSRC_XY, fs	; write EAX = bitblt source X,Y
	EngineWrite B_RDEST_XY, fs	; write EAX = bitblt dest   X,Y
	mov	eax,dDestFmt
	or	eax,(07980000h + bDraw + CPUAlign_dword + bFirst_dword) ;
	EngineWrite B_CMD_SET, fs	; write EAX=bitblt restores engine state
;@@:
;---------------------------------------------------------------------------
; Copy text mode, fg, bg colors, and clipping rect to frame
;---------------------------------------------------------------------------

	lds	si,lpDrawMode			; Note DrawMode info locally
	assumes ds, nothing			;

	mov	ax, [si].bkMode 		;
	mov	wBkMode, ax			;
	mov	eax, [si].bkColor		;
	mov	dBgColor, eax			;
	mov	eax, [si].TextColor		;
	mov	dFgColor, eax			;

	lds	si,lpClipRect			; ds:[si] -> clipping rect
	assumes ds, nothing			;

	mov	eax,[si]			;
	mov	dword ptr rcClip,eax		;
	mov	eax,[si][4]			;
	mov	dword ptr rcClip[4],eax 	;
	.errnz	left
	.errnz	top-left-2
	.errnz	right-top-2
	.errnz	bottom-right-2
	.errnz	(size RECT)-bottom-2

;---------------------------------------------------------------------------
; Set up clipping rect
;---------------------------------------------------------------------------

RTM2SetClipping:
	mov	ax,rcClip.left		;set left edge
	rol	eax,16			;in high dword
	dec	rcClip.right
	mov	ax,rcClip.right 	;and right edge
	mov	dLRClip,eax		;save left/right clipping

	mov	ax, rcClip.top		;Set top edge
	rol	eax,16			;in high dword
	dec	rcClip.bottom
	mov	ax, rcClip.bottom	;Set bottom edge
	mov	dTBClip,eax		;save top/bottom clipping

;---------------------------------------------------------------------------
; Draw any opaquing rectangles.
;---------------------------------------------------------------------------
;
; Since we clip the opaquing rectangles in software, we don't have to
;  set the clip rect into the hardware.

	cmp	lpOpaqueRect.sel, 0	; Is there an opaque rect?
	je	RTM2_SetTextColors	; No, skip over

	lds	si, lpOpaqueRect	; ds:si -> opaquing rectangle
	assumes ds, nothing		;

	mov	eax,RECT_SOLID+bClip_Enable
	or	eax,dDestFmt
	mov	lCmd,eax		;save it

	CheckFIFOSpace THREE_SLOTS

	mov	eax,dTBClip		;get top/bottom clipping
	EngineWrite B_CLIP_T_B

	mov	eax,dLRClip		;get left/right clipping
	EngineWrite B_CLIP_L_R

	mov	eax,dBgColor		;get color
	EngineWrite B_PAT_FG_CLR	;and set it

;---------------------------  Rectangle loop  ---------------------------------

RTM2NextRect:
	mov	bx, [si].left		;bx = left
	mov	cx, [si].right		;cx = right
	cmp	bx, rcClip.left 	;Pin left
	jge	short RTM2LeftPinned
	mov	bx, rcClip.left 	;Clip left side

RTM2LeftPinned:
	cmp	cx, rcClip.right	;Pin right
	jle	short RTM2RightPinned
	mov	cx, rcClip.right	;Clip right side
	inc	cx			;restore original right

RTM2RightPinned:
	mov	di,bx			;save di = x
	sub	cx,bx			;right - left
	jle	RTM2SkipRect		;
	dec	cx

	mov	bx,[si].top		;bx = top
	mov	dx,[si].bottom		;dx = bottom
	cmp	bx,rcClip.top		;Pin top
	jge	short RTM2TopPinned
	mov	bx,rcClip.top		;clip bottom

RTM2TopPinned:
	cmp	dx, rcClip.bottom	;Pin bottom
	jle	short RTM2BottomPinned
	mov	dx,rcClip.bottom
	inc	dx			;restore original bottom
;
;bx = top, dx = bottom, di = x, cx = width--
;
RTM2BottomPinned:
	sub	dx,bx			;dx = height = bottom - top
	jle	RTM2SkipRect		;

if patch16
	test	BWFlag,08h
	jz	nopatch_16
	CheckFIFOSpace THREE_SLOTS
	push	ebx
	push	ecx
	push	edx
	cmp	cx,19
	jle	@F
	mov	cx,19
@@:	
	rol	ecx,16			;width in high dword
	mov	cx,1	 		;get height in high dword
	EngineWrite B_RWIDTH_HEIGHT, fs, ecx

	mov	ax,di			;get x
	rol	eax,16			;destx in high dword
	mov	ax,bx			;desty in low dword
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	pop	edx
	pop	ecx
	pop	ebx
nopatch_16:
endif
	CheckFIFOSpace THREE_SLOTS
	rol	ecx,16			;move width-1 to high dword
	mov	cx,dx			;and height in low dword
	EngineWrite B_RWIDTH_HEIGHT, fs, ecx

	mov	ax,di			;get x
	rol	eax,16			;move it in high dword
	mov	ax,bx			;get y in low dword
	EngineWrite B_RDEST_XY, fs

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

RTM2SkipRect:
	add	si, size RECT			; pRect++

	mov	eax, dword ptr [si]		; NULL terminator?
	or	eax, dword ptr [si][4]		;
	jnz	RTM2NextRect			; No, continue looping

;###patch
CheckFIFOSpace FOUR_SLOTS, fs
mov	eax,00000001h
EngineWrite B_RWIDTH_HEIGHT, fs
EngineWrite B_RSRC_XY, fs
EngineWrite B_RDEST_XY, fs
mov	eax,07980000h+bDraw+CPUAlign_dword+bFirst_dword
or	eax,dDestFmt
EngineWrite B_CMD_SET, fs


 RTM2_SetTextColors:
	cmp	nCount, 0			; Any string?
	jz	RTM2_StringFinished		; No, we are done

;---------------------------------------------------------------------------
; Set up cmd_set
;---------------------------------------------------------------------------

	mov	edx,MONOSRCBLT+SRCCOPY+bClip_Enable
	or	edx,dDestFmt

	cmp	wBkMode, TRANSPARENT	;transparent text ?
	jne	short @F		;no
	or	edx,bTransparent	;enable transparent bit
@@:
	mov	lCmd,edx		;save it for non-autoexec case

;------------------------------------------------------------------------------
;------------------------  Prepare to process glyphs  -------------------------
;------------------------------------------------------------------------------

	mov	ds, lpFont.sel			; ds:[] -> PFont
	assumes ds,nothing			;

	les	di, lpString			; es:[di] -> glyph index array
	assumes es,nothing			;

	cmp	lpDx.sel, 0			; Dx array?
	jne	RTM2Generic			; Yes, skip to generic to

	lgs	bx, lpXform			; Make sure text bounding box
	assumes gs, nothing			;  is within range of the chip
	cmp	gs:[bx].left, -512		;
	jle	RTM2Generic			; I'm not sure of the exact
	cmp	gs:[bx].right, 1280		;   range, but these values
	jge	RTM2Generic			;   should be good enough
	cmp	gs:[bx].top, -512		;
	jle	RTM2Generic			;
	cmp	gs:[bx].bottom, 1024		;
	jge	RTM2Generic			;

	mov	cx, nCount			; ecx.h = nCount - 1
	dec	cx				;
	shl	ecx, 16 			;

	test	ds:[nfFormat], NF_FROM_BMP	; Derived from bmp font?
	jz	RTM2TrueType			; No, pass to TrueType code

;******************************************************************************
;************************  Bitmap font textout code  **************************
;******************************************************************************
;
; This code handles simple bitmap font textout with no dx array. Knowing
;   this, the code becomes much simpler because several assumptions can
;   be made:
;
;  1) Glyph index array is always bytes
;  2) Natural spaced bitmap glyphs never overlap
;  3) Font is small format
;
;  ax  = <unused>
;  bx  = glyph index * 2
;  cx  = <unused>
;  dx  = x
;  si -> glyph data
;  di -> lpGlyphs


RTM2BmpNextGlyph:
	movzx	bx, byte ptr es:[di]		; bx = 2 * glyph index
	inc	di				;
	add	bx, bx				;

;---------------------	Set glyph x,y and height,width	-----------------------

	mov	dx, x				; x += aw[gi]
	mov	si, wptr ds:[nfAWTable] 	;
	mov	ax, ds:[si][bx] 		;
	add	ax, dx				;
	mov	x, ax				;

	mov	si, wptr ds:[nfGlyphOffset]	; ds:[si] -> glyph data
	mov	si, ds:[si][bx] 		;

	cmp	[si].srgHeight, 0		; Any bmp?
	je	RTM2BmpGlyphDone		; No, nothing to do

;dx = x

	movsx	ax, [si].srgOrgX		;Set glyph x-origin
	add	dx, ax				;dx = x + srgOrgX
	mov	wDestx,dx			;save it

	movsx	bx, [si].srgOrgY		; Set glyph y-origin
	mov	ax, y				;
	sub	ax, bx				;ax = y - srcOrgY
	mov	wDesty,ax			;save it

	movzx	bx, [si].srgWidth		;Set glyph width
	lea	ax, [bx][-1]			;ax = width to set
	mov	wWidth,ax			;save it

	movzx	cx, [si].srgHeight		;Set glyph height
	mov	ax,wDesty
	mov	dx,ax
	add	ax,cx
	dec	ax
	cmp	ax,rcClip.bottom	;need bottom clip?
	jle	short @F		;N, continue
	mov	cx,rcClip.bottom
	sub	cx,dx
	js     RTM2BmpGlyphDone 	; No, nothing to do
	inc	cx
@@:
	cmp	dx,rcClip.top	;need top clip?
	jge	short @F	;N, continue
	mov	ax,rcClip.top
	sub	ax,dx		;ax = # of scan to clip from top
	sub	cx,ax		;adjust height
	jle	RTM2BmpGlyphDone		; No, nothing to do
	add	wDesty,ax	;adjust starting Y
	mov	dx,bx
	add	dx,7
	shr	dx,3		;# of byte per scan
	mul	dx
	add	si,ax		;adjust pointer to font image
@@:
	mov	wHeight,cx			;save it

	add	si, size SMALLROWGLYPH		;si -> glyph bits

RTMBmpSetAuto:

	bts	wFlag,1 			;enter here once already ?
	jc	short RTMBmpSetWidthHeight	;yes, skip it
;
;only need to set source fgcolor/bkcolor & clipping once in this loop
;
	CheckFIFOSpace FOUR_SLOTS

	mov	eax,dLRClip			;get left/right clipping
	EngineWrite B_CLIP_L_R, fs

	mov	eax,dTBClip			;get top/bottom clipping
	EngineWrite B_CLIP_T_B, fs

	mov	eax,dFgColor			; Plug in foreground mode
	EngineWrite B_SRC_FG_CLR, fs

	mov	eax,dBgColor			; Plus in background mode
	EngineWrite B_SRC_BG_CLR, fs

RTMBmpSetWidthHeight:
	CheckFIFOSpace TWO_SLOTS
	mov	ax,wWidth			;get width
;### patch
if	0
or	al,7
endif
	rol	eax,16				;in high dword
	mov	ax,wHeight			;get height
	EngineWrite B_RWIDTH_HEIGHT, fs 	;set width & height

	mov	ax,wDestx			;get destx
	rol	eax,16				;in high dword
	mov	ax,wDesty			;get desty
	EngineWrite B_RDEST_XY, fs		;and set it
;###wjm
if	0
CheckFIFOSpace TWO_SLOTS
push	bx
mov	bx,wDestx
add	bx,wWidth
mov	eax,dLRClip			;get left/right clipping
cmp	bx,ax
jae	@F
mov	ax,bx
@@:
EngineWrite B_CLIP_L_R, fs
pop	bx
endif

WaitEngineIdle fs		;better wait for engine idle
	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

;
;bx = width of glyph in pixels
;cx = height of glyph in scans
;ds:si -> glyph pattern
;fs -> MMIO
;
	call	TransferSmallGlyph		;Dump glyph to chip

RTM2BmpGlyphDone:
	sub	ecx, 00010000h			; Any more glyphs to process?
	jge	RTM2BmpNextGlyph		; Yep, loop back

;------------------------------------------------------------------------------
;-------------------  Done with string, cleanup and exit  ---------------------
;------------------------------------------------------------------------------

RTM2_StringFinished:
	mov	ax, DGROUP			; ds -> data
	mov	ds, ax				;
	assumes ds,Data 			;

	lgs	si,lpDevice			;
	and	gs:[si].deFlags,not BUSY

RTMM2_Exit:
	xor	ax,ax				;
	xor	dx,dx				;
	mov	InSrcBlt,al			;

cEnd



;******************************************************************************
;*****************   T R U E T Y P E   T E X T	 O U T	 **********************
;******************************************************************************
;
; Handles simple TrueType textout. Code makes the following assumptions:
;
;  1) Small format
;  2) Word glyph index array
;  3) Possible glyph overlap
;
;  ax = <unused>
;  bx = glyph index * 2
;  cx = <unused>
;  dx =
;  si =
;  di = glyph index array offset
;
;  ds:[]   -> new font
;  es:[di] -> glyph index array
;  fs:[]   -> mem-mapped transfer register

RTM2TrueType:
	mov	wMaxRHS, -32000 		; Init to way left
	mov	bSwitchBack, 0			; No switchback

	test	ds:[nfFormat], NF_LARGE 	; Small format?
	jnz	RTM2Generic			; No, use generic code

RTM2T2NextGlyph:
	mov	bx, es:[di]			; bx = 2 * glyph index
	add	di, 2				;
	add	bx, bx				;

;---------------------	Set glyph x,y and height,width	-----------------------

	mov	dx, x				; x += aw[gi]
	mov	si, wptr ds:[nfAWTable] 	;
	mov	ax, ds:[si][bx] 		;
	add	ax, dx				;
	mov	x, ax				;

	mov	si, wptr ds:[nfGlyphOffset]	; ds:[si] -> glyph data
	mov	si, ds:[si][bx] 		;

	cmp	[si].srgHeight, 0		; Any bmp?
	je	RTM2T2GlyphDone 		; No, nothing to do

; dx = xOrg of glyph

	movsx	ax, [si].srgOrgX		;Set glyph x-origin
	add	ax, dx				;ax = x - srgOrgX
	mov	wDestx,ax			;save it

;mv-dont think we need this here again ?
	cmp	ax, wMaxRHS			;Overlaps previous glyph?
	jge	RTM2T2OverlapDone
;	cmp	wBkMode, TRANSPARENT		;transparent?
;	jne	RTM2T2OverlapDone		;no
	or	lCmd,bTransparent

RTM2T2OverlapDone:
	movsx	bx, [si].srgOrgY		;Set glyph y-origin
	mov	cx, y				;
	sub	cx, bx				;cx = y - srgOrgY
	mov	wDesty,cx			;save it

	movzx	bx, [si].srgWidth		;Set glyph width
	lea	cx, [bx][-1]			;width
	mov	wWidth,cx			;save width

	add	ax, bx				;ax = RHS for this glyph
	cmp	ax, wMaxRHS			;> than old rhs?
	jle	short @F			;No, skip over
	mov	wMaxRHS, ax			;Yes, set new rhs
@@:
	movzx	cx, [si].srgHeight		;Set glyph height
	mov	ax,wDesty
	mov	dx,ax
	add	ax,cx
	dec	ax
	cmp	ax,word ptr dTBClip	;need bottom clip?
	jle	short @F		;N, continue
	mov	cx,word ptr dTBClip
	sub	cx,dx
	js	RTM2T2GlyphDone 		; No, nothing to do
	inc	cx
@@:
	cmp	dx,rcClip.top	;need top clip?
	jge	short @F	;N, continue
	mov	ax,rcClip.top
	sub	ax,dx		;ax = # of scan to clip from top
	sub	cx,ax		;adjust height
	jle	RTM2T2GlyphDone 		; No, nothing to do
	add	wDesty,ax	;adjust starting Y
	mov	dx,bx
	add	dx,7
	shr	dx,3		;# of byte per scan
	mul	dx
	add	si,ax		;adjust pointer to font image
@@:
	mov	wHeight,cx			;save it

	add	si, size SMALLROWGLYPH		;si -> glyph bits

RTMTrueSetAuto:

	bts	wFlag,1 			;enter here once already ?
	jc	short RTMTTSetWidthHeight
;
;only need to set source fgcolor/bkcolor & clipping once in this loop
;
	CheckFIFOSpace FOUR_SLOTS

	mov	edx,dLRClip			;get left/right clipping
	EngineWrite B_CLIP_L_R, fs, edx

	mov	edx,dTBClip			;get top/bottom clipping
	EngineWrite B_CLIP_T_B, fs, edx

	mov	edx,dFgColor			; Plug in foreground mode
	EngineWrite B_SRC_FG_CLR, fs, edx

	mov	edx,dBgColor			; Plus in background mode
	EngineWrite B_SRC_BG_CLR, fs, edx

RTMTTSetWidthHeight:
	CheckFIFOSpace TWO_SLOTS

	mov	dx,wWidth			;get width
;### patch
if	0
or	dl,7
endif
	rol	edx,16				;in high dword
	mov	dx,wHeight			;get height
	EngineWrite B_RWIDTH_HEIGHT, fs, edx	;and set it

	mov	dx,wDestx			;get x
	rol	edx,16				;in high dword
	mov	dx,wDesty			;get y
	EngineWrite B_RDEST_XY, fs, edx 	;and set it

;###wjm
if	0
CheckFIFOSpace TWO_SLOTS
push	bx
mov	bx,wDestx
add	bx,wWidth
mov	eax,dLRClip			;get left/right clipping
cmp	bx,ax
jae	@F
mov	ax,bx
@@:
EngineWrite B_CLIP_L_R, fs
pop	bx
endif
	WaitEngineIdle fs		;better wait for engine idle

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

;
;bx = width of glyph in pixels
;cx = height of glyph in scans
;ds:si -> glyph pattern
;fs -> MMIO
;
	call	TransferSmallGlyph		;Dump glyph to chip
						;returns ax = adjustment
RTM2T2GlyphDone:
;**	cmp	bSwitchBack, 1			; Switch back to opaque mode?
;**	je	RTM2T2SwitchBack		; Yes, jump out to do it

RTM2T2SwitchDone:
	sub	ecx, 00010000h			; Any more glyphs to process?
	jge	RTM2T2NextGlyph 		; Yep, loop back
	jmp	RTM2_StringFinished		; Outta here...

;******************************************************************************
;***************   R T M 2   G E N E R I C   T E X T O U T   ******************
;******************************************************************************

RTM2Generic:
	or	lCmd,bTransparent
	mov	gs,seg_lpDx			; gs:[]-->lpDx
	assumes gs,nothing			;

	mov	ds,seg_lpFont			; ds:[]-->PFont
	assumes ds,nothing			;

	les	di,lpString			; es:[di]-->glyph index array
	assumes es,nothing			;

;------------------------------------------------------------------------------
; Pickup next glyph index
;------------------------------------------------------------------------------

RTM2_NextGlyph:
	test	etoOptions, ETO_GLYPH_INDEX	; Byte or word string?
	jnz	short @f			;
	movzx	esi, byte ptr es:[di]		; Pickup 8 bit index
	inc	di				;
	jmp	short RTM2_LargeOrSmall 	;
@@:	movzx	esi, wptr es:[di]		; Pickup 16 bit index
	add	di, 2				;

RTM2_LargeOrSmall:
	mov	CharIndex, esi			;

	mov	edx, ds:[nfGlyphOffset] 	; edx --> glyph offset table
	test	ds:[nfFormat], NF_LARGE 	; Large font?
	jnz	RTM2_PositionLargeGlyph 	; Yes, special handling

;------------------------------------------------------------------------------
; Position Small Glyph
;------------------------------------------------------------------------------

	movzx	esi, word ptr [edx][esi*2]	; Fetch small glyph offset
	cmp	[si].srgHeight, 0		; Any bits?
	je	RTM2_GlyphDone			; No, blank bitmap

	mov	ax, x				; Set glyph x-origin
	movsx	cx, [si].srgOrgX		;
	add	ax, cx				;
	cmp	ax, rcClip.right		; Off to right?
	jg	RTM2_GlyphDone			; Yes, skip it
	mov	wDestx,ax			;save it

	movzx	bx, [si].srgWidth		; bx = glyph width
	add	ax, bx				; ax = glyph rhs+1
	cmp	ax, rcClip.left 		; Clipped?
	jle	RTM2_GlyphDone			; yes.
	dec	bx				; no.
	mov	wWidth,bx			;save it

	movsx	ax, [si].srgOrgY		; Set glyph y-origin
	neg	ax				;
	add	ax, y				;
	cmp	ax, rcClip.bottom		;
	jg	RTM2_GlyphDone			;
	mov	wDesty,ax			;save it

	movzx	bx, [si].srgHeight		;
	add	ax, bx				;ax = bottom of glyph
	cmp	ax, rcClip.top			;clipped?
	jle	RTM2_GlyphDone			;yes.
	mov	wHeight,bx			;save it

	movzx	bx, [si].srgWidth		;
	movzx	ecx, [si].srgHeight		; cx = height in scans.
	mov	ax,wDesty
	mov	dx,ax
	add	ax,cx
	dec	ax
	cmp	ax,rcClip.bottom	;need bottom clip?
	jle	short @F		;N, continue
	mov	cx,rcClip.bottom
	sub	cx,dx
	js     RTM2_GlyphDone
	inc	cx
@@:
	cmp	dx,rcClip.top	;need top clip?
	jge	short @F	;N, continue
	mov	ax,rcClip.top
	sub	ax,dx		;ax = # of scan to clip from top
	sub	cx,ax		;adjust height
	jle	RTM2_GlyphDone
	add	wDesty,ax	;adjust starting Y
	mov	dx,bx
	add	dx,7
	shr	dx,3		;# of byte per scan
	mul	dx
	add	si,ax		;adjust pointer to font image
@@:
	mov	wHeight,cx			;save it

	add	si, size SMALLROWGLYPH		; esi-->glyph bits

;
;bx = width of glyph in pixels
;ecx = height of glyph in scans
;ds:esi -> glyph pattern
;edi = width height to set
;fs -> MMIO
;
RTM2_TransferGlyph:

	bts	wFlag,1 			;enter here once already ?
	jc	short RTMGSetWidthHeight	;yes, skip it
;
;only need to set source fgcolor/bkcolor & clipping once in this loop
;
	CheckFIFOSpace FOUR_SLOTS

	mov	edx,dLRClip			;get left/right clipping
	EngineWrite B_CLIP_L_R, fs, edx

	mov	edx,dTBClip			;get top/bottom clipping
	EngineWrite B_CLIP_T_B, fs, edx

	mov	edx,dFgColor			; Plug in foreground mode
	EngineWrite B_SRC_FG_CLR, fs, edx

	mov	edx,dBgColor			; Plus in background mode
	EngineWrite B_SRC_BG_CLR, fs, edx

RTMGSetWidthHeight:
	CheckFIFOSpace TWO_SLOTS

	mov	dx,wWidth			;get width
;### patch
if	0
or	dl,7
endif
	rol	edx,16				;in high dword
	mov	dx,wHeight			;get height
	EngineWrite B_RWIDTH_HEIGHT, fs, edx	;and set it

	mov	dx,wDestx			;get x
	rol	edx,16				;in high dword
	mov	dx,wDesty			;get y
	EngineWrite B_RDEST_XY, fs, edx 	;and set it

;###wjm
if	0
CheckFIFOSpace TWO_SLOTS
push	bx
mov	bx,wDestx
add	bx,wWidth
mov	eax,dLRClip			;get left/right clipping
cmp	bx,ax
jae	@F
mov	ax,bx
@@:
EngineWrite B_CLIP_L_R, fs
pop	bx
endif
	WaitEngineIdle fs		;better wait for engine idle

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

;
;bx = width of glyph in pixels
;ecx = height in scans
;ds:esi -> glyph pattern
;fs -> MMIO
;
	call	TransferLargeGlyph		; Dump glyph bmp to chip

RTM2_GlyphDone:
	cmp	word ptr lpDx+2,0		; lpDx given?
	jne	short @f			; Yes, process it
	mov	ebx, CharIndex			; No, use advance width array
	mov	esi, ds:[nfAWTable]		;
	mov	ax, word ptr ds:[esi][ebx*2]	;
	jmp	RTM2_AdvanceX			;
@@:
	mov	bx, lpDx.off			; x += *lpDx++
	mov	ax, gs:[bx]			;
	add	lpDx.off, 2			;

RTM2_AdvanceX:
	add	x, ax				; x += dx
	dec	nCount				; Any more?
	jnz	RTM2_NextGlyph			; Yep, loop back

	jmp	RTM2_StringFinished		; Stick a fork in it, we're done


;------------------------------------------------------------------------------
; Position Large Glyph
;------------------------------------------------------------------------------

RTM2_PositionLargeGlyph:
	mov	esi,[edx][esi*4]		;Fetch large glyph offset
	cmp	[esi].lrgHeight,0		;Any bits?
	je	RTM2_GlyphDone			;No, blank bitmap

	mov	ax,x				;Set glyph x-origin
	add	ax,[esi].lrgOrgX		;
	cmp	ax,rcClip.right 		;Off to right?
	jg	RTM2_GlyphDone			;Yes, skip it
	mov	wDestx,ax			;save it

	mov	bx,[esi].lrgWidth		;bx = glyph width
	add	ax,bx				;ax = glyph rhs+1
	cmp	ax,rcClip.left			;Clipped?
	jle	RTM2_GlyphDone			;yes.
	dec	bx				;width--
	mov	wWidth,bx			;save it

	mov	ax,[esi].lrgOrgY		;Set glyph y-origin
	neg	ax
	add	ax,y
	cmp	ax,rcClip.bottom
	jg	RTM2_GlyphDone
	mov	wDesty,ax			;save it

	movzx	ecx,[esi].lrgHeight		;cx = height in scans.
	add	ax,cx				;ax = bottom of glyph
	cmp	ax,rcClip.top			;clipped?
	jle	RTM2_GlyphDone			;yes.

	mov	bx,[esi].lrgWidth		;bx = width
	mov	ax,wDesty
	mov	dx,ax
	add	ax,cx
	dec	ax
	cmp	ax,rcClip.bottom	;need bottom clip?
	jle	short @F		;N, continue
	mov	cx,rcClip.bottom
	sub	cx,dx
	js     RTM2_GlyphDone
	inc	cx
@@:
	cmp	dx,rcClip.top	;need top clip?
	jge	short @F	;N, continue
	mov	ax,rcClip.top
	sub	ax,dx		;ax = # of scan to clip from top
	sub	cx,ax		;adjust height
	jle	RTM2_GlyphDone
	add	wDesty,ax	;adjust starting Y
	mov	dx,bx
	add	dx,7
	shr	dx,3		;# of byte per scan
	mul	dx
	movzx	eax,ax
	add	esi,eax 	;adjust pointer to font image
@@:
	mov	wHeight,cx			;save it

	add	esi,size LARGEROWGLYPH		;esi-->glyph bits
	jmp	RTM2_TransferGlyph		;Pump to hardware


;******************************************************************************
;****************   T R A N S F E R   S M A L L   G L Y P H   *****************
; Entry:
;	bx     = width of glyph in pixels
;	cx     = height of glyph in scans
;	ds:si -> glyph pattern
;	fs:[] -> memory transfer buffer
; Exit:
;	save ax, es, di
;------------------------------------------------------------------------------
cProc	TransferSmallGlyph, <NEAR>
cBegin	<nogen>

	push	es
	push	di
	mov	dx,fs
	mov	es,dx			;ES -> MMIO selector

	xor	di,di			;IMAGE_XFER
	cmp	bx, 8			;> 8 pixels wide?
	jg	TSGTryTwo		;Try next check

;------------------------------------------------------------------------------
					;ax = bytes adjustment
TSGOne: 				;cx = #bytes to transfer
	add	cx,3
	shr	cx,2			;cx = #dwords to transfer
	cmp	cx,2000h		;is glyph > 32K bytes (FIFO size) ?
	ja	short TSGVeryLarge	;yes, go handle it

;###
if	1;pci_fix
mov	bx,cx
tsg1:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short @F
mov	cx,X_CNT
@@:
sub	bx,cx
	rep	movsd
mov	cx,bx
test	bx,bx
jnz	short tsg1
endif
	jmp	TSGTransferDone 	;all done

TSGVeryLarge:
	mov	dx,cx			;dx = dwords to transfer
@@:	sub	dx,2000h		;update counter
	mov	cx,2000h		;32K bytes max to transfer
;###
if	1;pci_fix
mov	bx,cx
tsg3:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short tsg2
mov	cx,X_CNT
tsg2:
sub	bx,cx
	rep	movsd			;transfer the glyph
mov	cx,bx
test	bx,bx
jnz	short tsg3
endif
	xor	di,di			;reset ptr to beginning of IMAGE_XFER
	mov	cx,dx			;left over to transfer
	cmp	dx,2000h		;stil larger than the FIFO buffer
	ja	short @b		;yes, break it up
	jz	TSGTransferDone 	;we're done
;###
if	1;pci_fix
mov	bx,cx
tsg4:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short tsg5
mov	cx,X_CNT
tsg5:
sub	bx,cx
	rep	movsd			;transfer the glyph
mov	cx,bx
test	bx,bx
jnz	short tsg4
endif
	jmp	TSGTransferDone 	;returns ax = adjustment for dstx

;------------------------------------------------------------------------------
TSGTryTwo:
	cmp	bx, 16			;> 16 pixels wide?
	jg	TSGTryThree		;Try next check

					;cx = #words to transfer
	add	cx,cx			;cx * 2
	jmp	TSGOne		  ;cx = #bytes to transfer

;------------------------------------------------------------------------------
TSGTryThree:
	cmp	bx, 24			;> 24 pixels wide?
	jg	TSGTryFour		;Try next check

	mov	dx,cx			;width = 3 bytes
	add	cx,cx			;cx*2
	add	cx,dx			;cx = cx * 3
	jmp	TSGOne		  ;cx = #bytes to transfer

;------------------------------------------------------------------------------
;	Width is 4 bytes wide
;------------------------------------------------------------------------------
TSGTryFour:
	cmp	bx, 32			;> 32 pixels wide?
	jg	TSGGeneralCase		;Try next check

	cmp	cx,2000h		;is glyph > 32K bytes (FIFO size) ?
	jg	TSGVeryLarge		;yes, go handle it

;###
if	1;pci_fix
mov	bx,cx
tsg6:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short tsg7
mov	cx,X_CNT
tsg7:
sub	bx,cx
	rep	movsd			;transfer the glyph
mov	cx,bx
test	bx,bx
jnz	short tsg6
endif
	jmp	TSGTransferDone

;------------------------------------------------------------------------------
;	Enter here if width is larger than 4 bytes
;------------------------------------------------------------------------------
;si -> srcbitmap ptr
;bx = width (in bytes)
;cx = height
;ax = #bytes to adjust
TSGGeneralCase:
	push	ax
	add	bx,7
	shr	bx,3
	mov	ax,bx			;ax = width (bytes)
	mul	cx			;dx:ax = width*height (bytes)
	mov	cx,ax			;cx = #bytes
	add	cx,3
	shr	cx,2			;cx = #dwords to transfer
	pop	ax
	xor	di,di			;IMAGE_XFER
	cmp	cx,2000h		;glyph larger than 32K bytes ?
	ja	TSGVeryLarge		;yes, go handle it

;###
if	1;pci_fix
mov	bx,cx
tsg8:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short tsg9
mov	cx,X_CNT
tsg9:
sub	bx,cx
	rep	movsd			;transfer the glyph
mov	cx,bx
test	bx,bx
jnz	short tsg8
endif
	jmp	short TSGTransferDone	;returns ax = adjustment for dstx


TSGTransferDone:
	pop	di
	pop	es
	ret
cEnd	<nogen>

;******************************************************************************
;***************   T R A N S F E R   L A R G E	 G L Y P H   ******************
;******************************************************************************
;
; On entry:
;   bx	    = width of glyph in pixels
;   cx	    = height of glyph in scans
;   ds:esi -> glyph pattern
;   fs:[]  -> pixel transfer buffer
;	save ax, es, di
;
cProc	TransferLargeGlyph,<NEAR>
cBegin	<nogen>

	push	es
	push	di
	mov	dx,fs
	mov	es,dx			;ES -> MMIO selector

	xor	edi,edi 		;IMAGE_XFER
	cmp	bx,8			;> 8 pixels wide?
	jg	TLGTryTwo		;Try next check

;------------------------------------------------------------------------------
;	Enter here if width is one byte
;------------------------------------------------------------------------------
					;ax = bytes adjustment
TLGOne: 				;cx = #bytes to transfer
	add	cx,3
	shr	cx,2			;cx = #dwords to transfer
	cmp	cx,2000h		;is glyph > 32K bytes (FIFO size) ?
	ja	TLGVeryLarge		;yes, go handle it

;###
if	1;pci_fix
mov	bx,cx
tlg1:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short tlg2
mov	cx,X_CNT
tlg2:
sub	bx,cx
	rep	movs dword ptr es:[edi],ds:[esi]
mov	cx,bx
test	bx,bx
jnz	short tlg1
endif
	jmp	TSGTransferDone 	;and exit

TLGVeryLarge:				;cx = #dwords to transfer
	mov	dx,cx			;dx = dwords to transfer
@@:	sub	dx,2000h		;update counter
	mov	cx,2000h		;32K bytes max to transfer
;###
if	1;pci_fix
mov	bx,cx
tlg0:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short tlg3
mov	cx,X_CNT
tlg3:
sub	bx,cx
	rep	movs dword ptr es:[edi],ds:[esi]
mov	cx,bx
test	bx,bx
jnz	short tlg0
endif
	xor	edi,edi 		;reset ptr to beginning of IMAGE_XFER
	mov	cx,dx			;left over to transfer
	cmp	dx,2000h		;stil larger than the FIFO buffer
	ja	short @b		;yes, break it up
	jz	TLGTransferDone 	;none left
;###
if	1;pci_fix
mov	bx,cx
tlg4:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short tlg5
mov	cx,X_CNT
tlg5:
sub	bx,cx
	rep	movs dword ptr es:[edi],ds:[esi]
mov	cx,bx
test	bx,bx
jnz	short tlg4
endif
	jmp	TLGTransferDone 	;returns ax = adjustment for dstx

;------------------------------------------------------------------------------
;	Enter here if width is two bytes
;------------------------------------------------------------------------------
TLGTryTwo:
	cmp	bx,16			;> 16 pixels wide?
	jg	TLGTryThree		;Try next check

					;cx = #words to transfer
	add	cx,cx			;cx * 2
	jmp	TLGOne		  ;cx = #bytes to transfer

;------------------------------------------------------------------------------
;	Enter here if width is three bytes
;------------------------------------------------------------------------------
TLGTryThree:
	cmp	bx,24			;> 24 pixels wide?
	jg	TLGTryFour		;Try next check

	mov	dx,cx			;width = 3 bytes
	add	cx,cx			;cx*2
	add	cx,dx			;cx = cx * 3
	jmp	TLGOne		  ;cx = #bytes to transfer

;------------------------------------------------------------------------------
;	Enter here if width is four bytes
;------------------------------------------------------------------------------
TLGTryFour:
	cmp	bx,32			;> 32 pixels wide?
	jg	TLGGeneralCase		;Try next check

	cmp	cx,2000h		;is glyph > 32K bytes (FIFO size) ?
	ja	TSGVeryLarge		;yes, go handle it

;###
if	1;pci_fix
mov	bx,cx
tlg6:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short tlg7
mov	cx,X_CNT
tlg7:
sub	bx,cx
	rep	movs dword ptr es:[edi],ds:[esi]
mov	cx,bx
test	bx,bx
jnz	short tlg6
endif
	jmp	TSGTransferDone

;------------------------------------------------------------------------------
;	Enter here if width is larger than 4 bytes
;------------------------------------------------------------------------------
;esi -> srcbitmap ptr
;bx = width (in bytes)
;cx = height
;ax = #bytes to adjust
TLGGeneralCase:
	push	ax			;save adjusted bytes
	add	bx,7
	shr	bx,3
	mov	ax,bx			;ax = width (bytes)
	mul	cx			;dx:ax = width*height (bytes)
	mov	cx,ax			;cx = #bytes
	add	cx,3
	shr	cx,2			;cx = #dwords to transfer
	pop	ax			;restore adjusted bytes
	xor	edi,edi 		;IMAGE_XFER
	cmp	cx,2000h		;is glyph > 32K bytes (FIFO size) ?
	ja	TSGVeryLarge		;yes, go handle it

;###
if	1;pci_fix
mov	bx,cx
tlg8:
CheckFIFOSpace SIXTEEN_SLOTS es
cmp	bx,X_CNT
jle	short tlg9
mov	cx,X_CNT
tlg9:
sub	bx,cx
	rep	movs dword ptr es:[edi],ds:[esi]
mov	cx,bx
test	bx,bx
jnz	short tlg8
endif

TLGTransferDone:
	pop	di
	pop	es
	ret
cEnd


sEnd	Code
	end
