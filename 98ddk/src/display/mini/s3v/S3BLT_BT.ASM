;----------------------------------------------------------------------------
; S3BLT.ASM
; Copyright (c) 1994 Microsoft Corporation
; Copyright (c) 1995 S3 Incorporated
;----------------------------------------------------------------------------
patch16 	equ	1
	.xlist
DOS5 = 1
	include cmacros.inc
	include macros.inc
	incDevice = 1			;Include control for gdidefs.inc
	incDrawMode = 1 		;Include DRAWMODE structure
	include gdidefs.inc
	include dibeng.inc
	include S3.inc			; S3 specific defines
	.list
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
BS_HOLLOW	equ	1

;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP DIB_BitBlt
	externFP BeginAccess		;in ACCESS.ASM
	externFP EndAccess		;in ACCESS.ASM

	externNP ClipSrc

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externD dCmd		      ;current value in CMD_SET
	externB bInDDraw
	externB InSrcBlt	      ;flag for image xfer in operation
	externW wBpp_Byte
	externW wBpp
	externW wScreenWidthBytes	     ;bytes per scanlines
	externW wMMIOSelector
	externB bAUTOEnable		;autoexec flag
	externB wCursorType
	externD dDestFmt
;IFDEF ASPEN
externB 	bRop			;in VGA.ASM
externB 	BWFlag			;in VGA.ASM
externW 	wBWwidth		;in INIT.ASM
;ENDIF

;###patch12
if	1
externW 	patch12_x1			;in INIT.ASM
externW 	patch12_x2			;in INIT.ASM
externW 	delta_x
externW 	org_xExt
endif

;###patch15
if	1
externW 	patch15_x1			;in INIT.ASM
endif
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
assumes cs,Code
	.386

;
DstPr	equ	000001000B
SrcPr	equ	000000100B
PatPr	equ	000000010B

externB BltTypeTable
;
BltDispatchtable	dw	DoBltNoDSP	; no DSP
			dw	DoBltDP 	; P(same as DP)
			dw	DoBltDS 	; S(same as DS)
			dw	DoBltDSP	; SP(same as DSP)
			dw	DoBltD		; D
			dw	DoBltDP 	; DP
			dw	DoBltDS 	; DS
			dw	DoBltDSP	; DSP

B_ReleaseAndCallDIBEngine:
	lfs	si,lpDestDev		;fs:si-->dest pdevice
	and	fs:[si].deFlags,not BUSY;release surface
	add	sp,2			;discard return address to B_Done

B_JmpToDIBEngine:
	pop	edi
	pop	esi
	lea	sp,[bp-2]
	pop	ds
	pop	bp
	jmp	DIB_BitBlt

;----------------------------------------------------------------------------
; BitBlt_S3
;----------------------------------------------------------------------------
cProc	BitBLT_S3_BT,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	include bbparms.inc
	include bblocals.inc
cBegin

	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	mov	ax,DGROUP		; AX = driver data selector
	lfs	si,lpDestDev		;fs:si-->dest pdevice
	mov	es,ax			; ES: = driver data selector
	mov	di,fs:[si].deFlags
	mov	bx,wptr Rop+2		;Get the raster op code.
	and	di,VRAM+PALETTE_XLAT
	cmp	di,VRAM
	jne	B_JmpToDIBEngine

	bts	fs:[si].deFlags,BUSY_BIT
	jc	B_JmpToDIBEngine	;already busy, let DIB engine handle

	cmp	bx,0aah 		;ROP == D ?
	je	B_Done			;yes

	mov	ax,wMMIOSelector
	mov	fs,ax

	cmp	bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	WaitDMA				; wait for DMA to stop before MMIO access
@@:

if	1;pci_fix
	WaitEngineIdle fs
;	CheckFIFOSpace SIXTEEN_SLOTS
endif

;if	1
;### do a dummy blt to clear up state machine after 3D command

;	cmp	bInDDraw,0		;need to patch?
;	je	short @F		;N, skip
;	CheckFIFOSpace FOUR_SLOTS, fs	; wait for FIFO space available
;###
;IFDEF ASPEN
        mov     es:bRop,bl
;ENDIF ;aspen
	xor	eax,eax
        EngineWrite B_SRC_BASE
        EngineWrite B_DEST_BASE

        mov     ax,wScreenWidthBytes            ;bytes per scanlines
        mov     dx,ax
        shl     eax,16                          ;destination stride
        or      ax,dx                           ;source stride

        EngineWrite B_DEST_SRC_STR

	mov	eax,00000001h		; EAX = width/height, source/dest X,Y
	EngineWrite B_RWIDTH_HEIGHT, fs ; write EAX = bitblt width/height
	EngineWrite B_RSRC_XY, fs	; write EAX = bitblt source X,Y
	EngineWrite B_RDEST_XY, fs	; write EAX = bitblt dest   X,Y
	mov	eax,dDestFmt
	or	eax,(07980000h + bDraw + CPUAlign_dword + bFirst_dword) ;
	EngineWrite B_CMD_SET, fs	; write EAX=bitblt restores engine state
;@@:
;endif

	xor	eax,eax
	mov	ax,bx			;eax = ROP256
	shl	eax,17			;b8ROP_256, CMD_SET[24:17]
	or	eax,Y_Positive+X_Positive+CPUAlign_dword+bDraw
	or	eax,dDestFmt		;BPP
	mov	lCmd,eax		;lCmd = ROP256+destfmt
;IFDEF ASPEN
	mov	dCmd,eax
;ENDIF ;ASPEN

	mov	bl,byte ptr cs:BltTypeTable[bx] ;bx has index into dispatch table
	cCall	cs:BltDispatchtable[bx]

B_Done:
	lfs	si,lpDestDev		;fs:si-->dest pdevice
	and	fs:[si].deFlags,not BUSY
cEnd


B_ExcludeUnion:
	mov	cx,DestxOrg		;cx = DestxOrg
	mov	bx,SrcxOrg		;bx = SrcxOrg
	cmp	cx,bx			;order them
	jl	short @f		;cx is the minimum
	xchg	bx,cx			;cx has smaller and bx has bigger of the two
@@:
	mov	dx,DestyOrg		;dx = DestyOrg
	mov	di,SrcyOrg		;di = DestyOrg
	cmp	dx,di			;order them
	jl	short @f		;dx has smaller and di has bigger of the two
	xchg	dx,di			;dx has smaller and di has bigger of the two
@@:
	add	bx,xExt 		;add extent to bigger x coordinate
	add	di,yExt 		;add extent to bigger y coordinate
	jmp	B_ExcludeCommon      ;merge with common code

B_Exclude:
	mov	bx,xExt 		;bx = xExt
	mov	di,yExt 		;di = yExt
	mov	cx,DestxOrg		;cx = DestxOrg
	mov	dx,DestyOrg		;dx = DestxOrg
	add	bx,cx			;Set right
	add	di,dx			;Set bottom

B_ExcludeCommon:
	lfs	si,lpDestDev		;fs:si-->dest pdevice
	dec	bx			;Make the extents inclusive of the
	dec	di			;last point
	push	lpDestDev		;PDevice
	push	cx			;Left
	push	dx			;Top
	push	bx			;Right
	push	di			;Bottom
	push	CURSOREXCLUDE		;Flags
	call	fs:[si].deBeginAccess
	lfs	si,lpDestDev		;restore fs:si-->dest pdevice
	mov	ax,DGROUP
	mov	es,ax			;restore es = Data
	pop	bx			;restore return address
	add	sp,2			;discard return address to B_Done
	push	lpDestDev		;PDevice
	push	CURSOREXCLUDE		;Flags
	push	cs
	push	offset B_Done		;return from unexclude
	push	fs:[si].deEndAccess	;address for end access.
	push	offset B_UnExclude
	jmp	bx			;return to caller

B_UnExclude:
	retf

page +
;------------------------------------------------------------------------------
; DoBltNoDSP
; This function is called when the passed Rop = 00 or 0FFh
; destination bitmap is screen. (Black/White)
; DoBltD
; This function is called when the passed Rop = D or Dn
; destination bitmap is screen.
;
; Entry:
;
;------------------------------------------------------------------------------
cProc		DoBltNoDSP,<NEAR>
;
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing


DoBltD:
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f		;H/W cursor, continue
	push	offset @f		;save address to come back to
	jmp	B_Exclude		;go exclude for S/W cursor
@@:
	mov	ax,wMMIOSelector
	mov	fs,ax			;gs <- MMIOSelector

	CheckFIFOSpace	THREE_SLOTS

	mov	ebx,lCmd		;
	or	ebx,RECT_FILL+bPAT_Mono ;ebx = bROP+destFmt+Rect

;###patch15
if	1
;test	BWFlag,1		;ViRGE/VX ?
;jnz	@F			;yes. skip it
mov	ax,word ptr xExt
cmp	ax,es:patch15_x1
jae	@F
CheckFIFOSpace FIVE_SLOTS, fs
mov	cx,es:patch15_x1
mov	word ptr xExt,cx	;let xExt=65, 33, or 23
mov	cx,ax			;cx=org xExt
push	cx
mov	ax,DestxOrg		 ;
mov	cx,ax
shl	eax,16
mov	ax,cx
pop	cx			;use cx=org xExt to clip away extra pixels
add	ax,cx			;
dec	ax
EngineWrite B_CLIP_L_R, fs
mov	ax,DestyOrg		 ;
mov	cx,ax
shl	eax,16
mov	ax,cx
add	ax,yExt 		;
dec	ax
EngineWrite B_CLIP_T_B, fs
or	ebx,bClip_Enable
@@:
endif
;	 WriteCMDAutoON B_CMD_SET, , ebx

if 0 ;patch16
	test	BWFlag,08h		;ViRGE/VX running fast page mode
	jz	no_pat16
	push	eax
	mov	ax,word ptr xExt
	cmp	ax,20
	jle	@F
	mov	ax,20
@@:
	dec	ax
	shl	eax,16
	mov	ax,1
	EngineWrite B_RWIDTH_HEIGHT

	mov	eax,dword ptr DestyOrg
	EngineWrite B_RDEST_XY		;set destx, desty

	WriteCMDAutoOFF B_CMD_SET, , ebx
	pop	eax

	CheckFIFOSpace	THREE_SLOTS
no_pat16:
endif
	mov	eax,dword ptr yExt	;load xExt and yExt into EAX
	sub	eax,00010000h		;xExt - 1 and yExt
	EngineWrite B_RWIDTH_HEIGHT

	mov	eax,dword ptr DestyOrg
	EngineWrite B_RDEST_XY		;set destx, desty

	WriteCMDAutoOFF B_CMD_SET, , ebx
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

DBDone:
cEnd
;
subttl	Pattern and Destination blt.
page +
;------------------------------------------------------------------------------
; DoBltDP
; This function is called when the passed Rop involves a Pattern
; (Solid, Color pattern, or Mono pattern)
; Entry:
;
;------------------------------------------------------------------------------
cProc	DoBltDP,<NEAR>
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f			;H/W cursor, continue
	push	offset @f			;save address to come back to
	jmp	B_Exclude			;go exclude for S/W cursor
@@:
	mov	ax,wMMIOSelector
	mov	fs,ax				;fs -> MMIO selector
;
;CMD_SET will be set inside PatternBlt for autoexecution
;
	call	PatternBlt			;returns edx = updated dCmd
	jc	DP_Exit 			;nothing to draw, exit

	CheckFIFOSpace	THREE_SLOTS

	mov	eax,dword ptr yExt		;load xExt and yExt into EAX
	sub	eax,00010000h			;xExt - 1 and yExt
	EngineWrite B_RWIDTH_HEIGHT

	mov	eax,dword ptr DestyOrg
	EngineWrite B_RDEST_XY			;set destx, desty

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

DP_Exit:
cEnd
;
;
subttl	Perform Bitblt involving Source and Destination.
page +
;------------------------------------------------------------------------------
; DoBltDS
; This function is called when the passed Rop involves a Source
; (ScreentoScreen, Color source, or Mono source)
; Entry:
;
;------------------------------------------------------------------------------
cProc	DoBltDS,<NEAR>
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	lds	di,lpSrcDev		;DS:DI -> src pdevice
	mov	ax,ds:[di].deFlags	;ax = src pdevice flags
	test	ax,ax			;Is the src a mono bitmap?
	jz	DS_MonoSrcBlt		;Yes, try mono to screen bitblt.
	test	ax,VRAM 		;is the src the display?
	jnz	DS_ScreenBlt		;screen to screen blt
	jmp	short DS_ColorSrcBlt	;either color or mono source

;--------------------------------------------------------------
;Mono Source Blt
;	ds:di -> psrcDev
;	fs -> MMIO selector
;	es -> data
;--------------------------------------------------------------
DS_MonoSrcBlt:
	cmp	ds:[di].deBitsPixel,1	;is it truly mono ?
	jne	B_ReleaseAndCallDIBEngine
	mov	ax,ds:[di].deFlags	;ax = src pdevice flags
	cmp	ds:[di].bmSegmentIndex,ax
	jne	B_ReleaseAndCallDIBEngine

	call	ClipSrc
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f			;H/W cursor, continue
	push	offset @f			;save address to come back to
	jmp	B_Exclude			;go exclude for S/W cursor
@@:
	or	lCmd,MONOSRCBLT+bClip_Enable
	mov	ax,wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector

	call	PrepareMonoSourceBlt
	call	MonoSourceBlt
	jmp	DS_Exit

;--------------------------------------------------------------
;Color Source Blt
;	ds:di -> psrcDev
;	fs -> MMIO selector
;--------------------------------------------------------------
DS_ColorSrcBlt:
	call	ClipSrc
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f			;H/W cursor, continue
	push	offset @f			;save address to come back to
	jmp	B_Exclude			;go exclude for S/W cursor
@@:
	or	lCmd,CLRSRCBLT
	mov	ax,wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector

	call	PrepareColorSourceBlt
	call	ColorSourceBlt
	jmp	short DS_Exit

;--------------------------------------------------------------
;Screen to Screen Blt
;	ds:di -> psrcDev
;	fs -> MMIO selector
;--------------------------------------------------------------
DS_ScreenBlt:
	call	ClipSrc
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f			;H/W cursor, continue
	push	offset @f			;save address to come back to
	jmp	B_ExcludeUnion			;go exclude for S/W cursor
@@:
	or	lCmd,BITBLT
	mov	ax,wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector

	call	ScreenToScreenBlt
DS_Exit:
cEnd


;
subttl	Perform Bitblt involving Pattern, Source, and Destination.
page +
;------------------------------------------------------------------------------
; DoBltDSP
; This function is called when the passed Rop involves Source & Pattern
; Entry:
;
;------------------------------------------------------------------------------
cProc	DoBltDSP,<NEAR>
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	lds	di,lpSrcDev		;DS:DI -> src pdevice
	mov	ax,ds:[di].deFlags	;ax = src pdevice flags
	test	ax,ax			;Is the src a mono bitmap?
	jz	DSP_MonoSrcBlt		;Yes, try mono to screen bitblt.
	test	ax,VRAM 		;is the src the display?
	jnz	short DSP_ScreenBlt	;screen to screen blt
	jmp	short DSP_ColorSrcBlt

;--------------------------------------------------------------
;Mono Source Blt with/without Pattern
;--------------------------------------------------------------
DSP_MonoSrcBlt:
	cmp	ds:[di].deBitsPixel,1	;is it truly mono ?
	jne	B_ReleaseAndCallDIBEngine
	mov	ax,ds:[di].deFlags	;ax = src pdevice flags
	cmp	ds:[di].bmSegmentIndex,ax
	jne	B_ReleaseAndCallDIBEngine

	call	ClipSrc
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f			;H/W cursor, continue
	push	offset @f			;save address to come back to
	jmp	B_Exclude			;go exclude for S/W cursor
@@:
	or	lCmd,bSRC_Mono+bSRC_Sys+bClip_Enable+ Y_Positive + X_Positive + bDraw
	mov	ax,wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector

	call	PrepareMonoSourceBlt	;returns ds:si -> mono bitmap
	call	PatternBlt		;sets cmd if autoexec enabled
	call	MonoSourceBlt		;sets cmd if autoexec disabled
	jmp	short DSP_Exit

;--------------------------------------------------------------
;Color Source Blt with/without Pattern
;--------------------------------------------------------------
DSP_ColorSrcBlt:
	call	ClipSrc
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f			;H/W cursor, continue
	push	offset @f			;save address to come back to
	jmp	B_Exclude			;go exclude for S/W cursor
@@:
	or	lCmd,bSRC_Sys
	mov	ax,wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector

	call	PrepareColorSourceBlt	;returns ds:si -> clr bitmap
	call	PatternBlt		;sets cmd if autoexec enabled
	call	ColorSourceBlt		;sets cmd if autoexec disabled
	jmp	short DSP_Exit

;--------------------------------------------------------------
;Screen to Screen with/without Pattern
;--------------------------------------------------------------
DSP_ScreenBlt:
	call	ClipSrc
	cmp	wCursorType,SOFTWARECURSOR
	jne	short @f			;H/W cursor, continue
	push	offset @f			;save address to come back to
	jmp	B_ExcludeUnion			;go exclude for S/W cursor
@@:
	mov	ax,wMMIOSelector
	mov	fs,ax			;fs -> MMIO selector

	call	PatternBlt		;sets cmd if autoexec enabled
	call	ScreenToScreenBlt	;sets cmd if autoexec disabled

DSP_Exit:
cEnd


subttl	Pattern Blt
page +
;------------------------------------------------------------------------------
; PatternBlt
; Handles the pattern blt (Solid/Color/Mono)
; Entry:
;	fs	--> MMIO Register segment
;	lCmd = dDestFmt+Rop
; Exit:
;	CF if nothing to draw
;------------------------------------------------------------------------------
cProc	PatternBlt,<NEAR>
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	lds	bx,lpPBrush		;ds:bx-->pBrush
	cmp	ds:[bx].dp8BrushStyle,BS_HOLLOW
	jz	PB_Done 		;its hollow, nothing to do
	cmp	ds:[bx].dp8BrushStyle,0 ;BS_SOLID
	je	short @F
	cmp	ds:[bx].dp8BrushStyle,2 ;BS_HATCHED
	jz	B_HatchPatBlt		;do hatch mono pattern blt
	cmp	ds:[bx].dp8BrushStyle,3 ;BS_PATTERN
	jz	B_ColorPatBlt		;do color or mono pattern blt
	jmp	PB_Done
@@:
	test	ds:[bx].dp8BrushFlags,COLORSOLID
	jz	B_ColorPatBlt		;do color or mono pattern blt

;--------------------------------------------------------------
;SOLID PATTERN BLT
;Entry:
;	fs -> MMIO selector
;	ds:bx --> pBrush
;--------------------------------------------------------------
B_SolidPatBlt:
	or	lCmd,MONOPATBLT 	 ;treat solid brush like mono pattern

	CheckFIFOSpace THREE_SLOTS

	mov	eax,dword ptr ds:[bx].dp8BrushBits
	EngineWrite B_PAT_FG_CLR

	mov	eax,0ffffffffh
	EngineWrite B_MONO_PAT_0
	EngineWrite B_MONO_PAT_1

	jmp	PB_Success

;--------------------------------------------------------------
;COLOR PATTERN BLT
;Entry:
;	fs    --> MMIO selector
;	ds:bx --> pBrush
;--------------------------------------------------------------
B_ColorPatBlt:
	test	ds:[bx].dp8BrushFlags,PATTERNMONO
	jnz	B_MonoPatBlt

;IFDEF ASPEN
	cmp	word ptr xExt,64		;is width >= 64 ?
	jge	@F				;yes.
	or	lCmd,CLRPATBLT+BW_disable	;disable BW
	jmp	short LoadCLRPat
@@:
;ENDIF ;ASPEN
	or	lCmd,CLRPATBLT
;IFDEF ASPEN
LoadCLRPat:
;ENDIF

;
;load 8x8 color pattern into PATTERN_REGISTERS
;
	cld
	push	es
	mov	ax,es:wBpp_Byte 	;get bytes_per_pixel
	mov	cx,fs
	mov	es,cx			;es -> MMIO
	lea	si,ds:[bx].dp8BrushBits ;ds:si -> color pattern brush
	mov	di,COLOR_PATTERN	;es:di -> color pattern regs

	WaitEngineIdle es
@@:
	mov	cx,16			;16 dwords
	rep	movsd			;xfer patbrush to ES:[COLOR_PATTERN]
;###
if	1;pci_fix
CheckFIFOSpace SIXTEEN_SLOTS es
endif
	dec	ax			;for 8bpp, we need to xfer 16dwords
	jnz	short @b		;24bpp, need to xfer 16*3 dwords
	pop	es
	jmp	PB_Success

;--------------------------------------------------------------
;HATCH PATTERN BLT
;Entry:
;	fs    --> MMIO selector
;	ds:bx --> pBrush
;--------------------------------------------------------------
B_HatchPatBlt:
IFDEF ASPEN
	cmp	es:bRop,0f0h		;ROP = F0 ?
	je	chk_BW_hatchblt 	;yes.
	jmp	use_org_hatchblt	;use original code for ViRGE
chk_BW_hatchblt:
	cmp	es:BWFlag,7		;BW disable ?
	je	chk_BWw_hatchblt	;no.
	jmp	use_org_hatchblt	;use original code for ViRGE
chk_BWw_hatchblt:
	mov	ax,word ptr xExt	;get width
	cmp	ax,es:wBWwidth		;>=CR53.b5 (16/32)
	jge	BW_for_hatchblt 	;yes
	jmp	use_org_hatchblt	;use original code for smaller monopat
BW_for_hatchblt:
;	 cmp	 BackgroundMode,2	;is background opaque ?
;	 jne	 Do_pass2_hatchblt	;no, its transparent
	call	HatchPat_step1		;fill BitBlt with background color
;Do_pass2_hatchblt:			;first
	or	lCmd,MONOPATBLT+bTransparent
	CheckFIFOSpace FOUR_SLOTS

	mov	al,byte ptr ds:[bx].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+4].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+8].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+12].dp8BrushMask
	ror	eax,8
	EngineWrite B_MONO_PAT_0

	mov	al,byte ptr ds:[bx+16].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+20].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+24].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+28].dp8BrushMask
	ror	eax,8
	EngineWrite B_MONO_PAT_1

	lds	si,lpPBrush		;DS:SI-->lpPhysBrush
	mov	eax,[si].dp8FgColor	;
	EngineWrite B_PAT_FG_CLR

	mov	eax,[si].dp8BgColor	;let's keep this before HW is back
	EngineWrite B_PAT_BG_CLR

	jmp	PB_Success
use_org_hatchblt:
ENDIF
	or	lCmd,MONOPATBLT 	 ;

	CheckFIFOSpace FOUR_SLOTS

	mov	al,byte ptr ds:[bx].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+4].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+8].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+12].dp8BrushMask
	ror	eax,8
	EngineWrite B_MONO_PAT_0

	mov	al,byte ptr ds:[bx+16].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+20].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+24].dp8BrushMask
	ror	eax,8
	mov	al,byte ptr ds:[bx+28].dp8BrushMask
	ror	eax,8
	EngineWrite B_MONO_PAT_1

	mov	eax,ds:[bx].dp8BgColor	   ;
	EngineWrite B_PAT_BG_CLR

	mov	eax,ds:[bx].dp8FgColor	   ;
	EngineWrite B_PAT_FG_CLR
	jmp	PB_Success

;--------------------------------------------------------------
;MONO PATTERN BLT
;Entry:
;	fs    --> MMIO selector
;	ds:bx --> pBrush
;--------------------------------------------------------------
B_MonoPatBlt:
IFDEF ASPEN
	cmp	es:bRop,0f0h		;ROP =F0 ?
	je	chk_BW_Monoblt		;yes.
	jmp	use_org_Monoblt 	;use original code for ViRGE
chk_BW_Monoblt:
	cmp	es:BWFlag,7		;BW disable ?
	je	chk_BWw_Monoblt 	;no.
	jmp	use_org_Monoblt 	;use original code for ViRGE
chk_BWw_Monoblt:
	mov	ax,word ptr xExt	;get width
	cmp	ax,es:wBWwidth		;>=CR53.b5 (16/32)
	jge	BW_for_Monoblt		;yes
	jmp	use_org_Monoblt 	;use original code for smaller monopat
BW_for_Monoblt:
;int 1
;	 cmp	 BackgroundMode,2	 ;is background opaque ?
;	 jne	 Do_pass2_Monoblt	 ;no, its transparent
	call	MonoPat_step1		;fill BitBlt with background color
;Do_pass2_Monoblt:			 ;first
	or	lCmd,MONOPATBLT+bTransparent

	CheckFIFOSpace FOUR_SLOTS

	mov	al,byte ptr ds:[bx].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+4].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+8].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+12].dp8BrushMono
	ror	eax,8
	EngineWrite B_MONO_PAT_0

	mov	al,byte ptr ds:[bx+16].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+20].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+24].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+28].dp8BrushMono
	ror	eax,8
	EngineWrite B_MONO_PAT_1

	lds	bx,lpDrawMode		;ds:bx --> draw mode

	mov	eax,dword ptr ds:[bx].TextColor
	EngineWrite B_PAT_BG_CLR

	mov	eax,dword ptr ds:[bx].bkColor
	EngineWrite B_PAT_FG_CLR			;before HW is back

	jmp	PB_Success
use_org_Monoblt:
ENDIF
	or	lCmd,MONOPATBLT 	 ;

	CheckFIFOSpace FOUR_SLOTS

	mov	al,byte ptr ds:[bx].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+4].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+8].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+12].dp8BrushMono
	ror	eax,8
	EngineWrite B_MONO_PAT_0

	mov	al,byte ptr ds:[bx+16].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+20].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+24].dp8BrushMono
	ror	eax,8
	mov	al,byte ptr ds:[bx+28].dp8BrushMono
	ror	eax,8
	EngineWrite B_MONO_PAT_1

	lds	bx,lpDrawMode		;ds:bx --> draw mode
	mov	eax,dword ptr ds:[bx].TextColor 	;let keep this
	EngineWrite B_PAT_BG_CLR

	mov	eax,dword ptr ds:[bx].bkColor
	EngineWrite B_PAT_FG_CLR

PB_Success:
	clc
	jmp	short PB_Exit

PB_Done:
	stc
PB_Exit:
cEnd

subttl	Prepare Mono Source Blt
page +
;------------------------------------------------------------------------------
; Gets pointer to Mono bitmap and update bFirst_Offset and bCPU_Align bits
; in the CMD_SET
; Exit:
;------------------------------------------------------------------------------
cProc	PrepareMonoSourceBlt,<NEAR>
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
cBegin

	or	lCmd,CPUAlign_dword	;set to dword-aligned
	mov	bx,DestxOrg		;
	mov	cx,xExt 		;
	mov	ax,SrcxOrg		;get starting source X-origin
	and	ax,07h			;AX contains bit offset from byte start
	add	cx,ax			;correct xExt for starting bit offset
	add	cx,07h			;make ending-X on a byte boundry too!
	and	cx,0ff8h		;(also make sure xExt is in range)
	sub	bx,ax			;correct DstxOrg for bit offset
	dec	cx			;width--
	mov	wAdjWidth,cx		;save width to set
	inc	cx			;
	mov	wAdjDestx,bx		;save destx to set
	shr	cx,3			;divide by 8 bits per byte

	lds	bx,lpSrcDev		;ds:bx-->lpSrcDev
;###
	mov	dx,[bx].bmWidthBytes	;dx = width bytes
	mov	wLineToLineInc,dx	;
	mov	ax,dx			;
	sub	ax,cx			;ax = line to line increment
	mov	di,ax			;save in di.
	mov	ax,SrcyOrg		;ax = vertical offset of src bitmap
	mul	dx			;ax = linear offset of src bitmap

	lds	si,[bx].bmBits		;ds:si-->src bitmap
	add	si,ax			;
	mov	ax,SrcxOrg		;
	shr	ax,3			;
	add	si,ax			;ds:si-->1st src byte to xfer
	mov	wAdjXExt,cx		;#bytes to transfer per line
	mov	wMonoSrcBitmapAddr,si	;ptr to monosrc bitmap
	mov	ax,ds			;
	mov	wBitmapSeg,ax		;save it

cEnd

;
subttl	Perform BitBLT involving Mono source pattern
page +
;------------------------------------------------------------------------------
; MONO SOURCE BLT
; Entry:
;	wMonoSrcBitmapAddr -> pts to monosrc bitmap (done in PrepareMonoSourceBlt)
;	fs --> MMIO selector
;------------------------------------------------------------------------------
cProc	MonoSourceBlt,<NEAR>
cBegin

	mov	ax,wMMIOSelector
	mov	fs,ax			;gs <- MMIOSelector

IFDEF ASPEN
	cmp	es:bRop,0cch		;ROP=CC ?
	je	chk_BW_srcmono		;yes.
	jmp	use_org_srcmono 	;use original code for ViRGE
chk_BW_srcmono:
	cmp	es:BWFlag,7		;BW disable ?
	je	chk_BWw_srcmono 	;no.
	jmp	use_org_srcmono 	;use original code for ViRGE
chk_BWw_srcmono:
	mov	ax,wAdjWidth		;get width
;int 1
	cmp	ax,es:wBWwidth		;>=CR53.b5 (16/32)
	jge	BW_for_srcmono		;yes
	jmp	use_org_srcmono 	;use original code for smaller monopat
BW_for_srcmono:
;
;now it's time to do block writes step1 : fill rect with BG color
;
	CheckFIFOSpace SIX_SLOTS

	lgs	bx,lpDrawMode		;gs:bx --> draw mode
	mov	eax,dword ptr gs:[bx].TextColor
	EngineWrite B_SRC_FG_CLR

;
;Set y-axis clipping for image transfers
;
	mov	ax,DestyOrg		;top clipping of source rect
	mov	cx,yExt 		;
	mov	bx,ax			;
	rol	eax,16			;in high dword of CLIP_T_B
	mov	ax,bx			;
	add	ax,cx			;desty + yExt
	dec	ax			;bottom clip of source rect
	EngineWrite B_CLIP_T_B		;set Y-axis clipping

;
;Set x-axis clipping for dest rectangle
;
	mov	ax,DestxOrg		;left clipping of dest rect
	mov	cx,xExt 		;
	mov	bx,ax			;bx = DestxOrg
	rol	eax,16			;in high dword of CLIP_L_R
	mov	ax,bx			;ax = destxOrg
	add	ax,cx			;DestxOrg + xExt
	dec	ax			;right clipping of dest rectangle
	EngineWrite B_CLIP_L_R		;set X-axis clipping

	mov	ax,wAdjWidth		;get width
	rol	eax,16			;in high dword
	mov	ax,yExt 		;and get height in low dword
	EngineWrite B_RWIDTH_HEIGHT

	mov	ax,wAdjDestx		;get destx
if patch16
	test	BWFlag,08h
	jz	skip_p16
	push	eax
	cmp	ax,19
	jle	@F
	mov	ax,19
@@:
	rol	eax,16
	mov	ax,1

	CheckFIFOSpace SIX_SLOTS

	EngineWrite B_RWIDTH_HEIGHT

	mov	ax,wAdjDestx		;get destx
	rol	eax,16			;in high word
	mov	ax,DestyOrg		;DstyOrg
	EngineWrite B_RDEST_XY		;and set it

	mov	eax,RECT_SOLID+bClip_Enable
	WriteCMDAutoOFF B_CMD_SET
	pop	eax

skip_p16:
endif
	rol	eax,16			;in high word
	mov	ax,DestyOrg		;DstyOrg
	EngineWrite B_RDEST_XY		;and set it

	mov	eax,RECT_SOLID+bClip_Enable
	WriteCMDAutoOFF B_CMD_SET

Do_pass2_srcmono:		       ;first
	or	lCmd,bTransparent+X_Positive	   ;turn on transparent bit

	CheckFIFOSpace SIX_SLOTS

	lgs	bx,lpDrawMode		;gs:bx --> draw mode
	mov	eax,dword ptr gs:[bx].bkColor
	EngineWrite B_SRC_FG_CLR

	mov	eax,dword ptr gs:[bx].TextColor
	EngineWrite B_SRC_BG_CLR
	jmp	short MonoSrc_Pass2_Shortcut
use_org_srcmono:
;
;original code is also the second step for block writes
;
ENDIF ;ASPEN
	CheckFIFOSpace SIX_SLOTS

	lds	bx,lpDrawMode		;gs:bx --> draw mode
	mov	eax,dword ptr ds:[bx].bkColor
	EngineWrite B_SRC_FG_CLR

	mov	eax,dword ptr ds:[bx].TextColor
	EngineWrite B_SRC_BG_CLR

;
;Set y-axis clipping for image transfers
;
	mov	ax,DestyOrg		;top clipping of source rect
	mov	cx,yExt 		;
	mov	bx,ax			;
	rol	eax,16			;in high dword of CLIP_T_B
	mov	ax,bx			;
	add	ax,cx			;desty + yExt
	dec	ax			;bottom clip of source rect
	EngineWrite B_CLIP_T_B		;set Y-axis clipping

;
;Set x-axis clipping for dest rectangle
;
	mov	ax,DestxOrg		;left clipping of dest rect
	mov	cx,xExt 		;
	mov	bx,ax			;bx = DestxOrg
	rol	eax,16			;in high dword of CLIP_L_R
	mov	ax,bx			;ax = destxOrg
	add	ax,cx			;DestxOrg + xExt
	dec	ax			;right clipping of dest rectangle
	EngineWrite B_CLIP_L_R		;set X-axis clipping

IFDEF ASPEN
MonoSrc_Pass2_Shortcut:
ENDIF ;ASPEN
	mov	ax,wAdjWidth		;get width

;###patch12
if	1
mov	bx,DGROUP
mov	gs,bx
assumes gs,Data

mov	delta_x,0		;init this first
;test	BWFlag,1		;ViRGE/VX ?
;jnz	nopatch12m		;yes. skip it
cmp	ax,es:patch12_x1	;>57,29,or 17 ?
jb	nopatch12m		;no, skip patch12
cmp	ax,es:patch12_x2
jae	nopatch12m
cmp	patch12_x2,23		;24 bpp ?
je	nopatch12m
mov	delta_x,1		;init this first
mov	ax,es:patch12_x2			;let ax=65, 33, or 23
nopatch12m:
endif

;### patch15
or	al,31
	rol	eax,16			;in high dword
	mov	ax,yExt 		;and get height in low dword
	EngineWrite B_RWIDTH_HEIGHT

	mov	ax,wAdjDestx		;get destx
	rol	eax,16			;in high word
	mov	ax,DestyOrg		;DstyOrg
	EngineWrite B_RDEST_XY		;and set it

	WaitEngineIdle			;for doing image xfers
	mov	InSrcBlt,0FFh		;flag for VDD to know we're

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
;Transfer mono source data to the image xfer registers
;ES:[IMAGE_XFER] <= Mono source data
;
					;starting source blt operation
	push	es
	mov	ax,fs			;
	mov	es,ax			;ES -> MMIO

	mov	ax,wBitmapSeg		;
	mov	ds,ax			;
	mov	si,wMonoSrcBitmapAddr	;ds:si -> src bitmap
	mov	bx,yExt 		;get line loop counter in BX
	mov	cx,wAdjXExt		;#bytes to transfer per line
	mov	ax,wLineToLineInc	;
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

MonoLoop0:
@@:
	xor	di,di			;IMAGE_XFER
	push	si
	mov	cx,dx			;dx = #dwords to xfer/scanline
;###
if	1;pci_fix
push	dx
MonoLoop_1:
mov	cx,dx
cmp	dx,X_CNT
jle	m2
mov	cx,X_CNT
m2:
sub	dx,cx
CheckFIFOSpace SIXTEEN_SLOTS
	rep	movsd			;move it
test	dx,dx
jnz	MonoLoop_1
pop	dx
endif

;###patch12
if	1
mov	cx,gs:delta_x
rep	stosd
endif
	pop	si
	add	si,ax			;ptr to next scanline
	dec	bx			;height--
	jnz	short @b		;more lines to xfer

MonoCleanup:
	pop	es			;restore es
	mov	ax,DGROUP
	mov	ds,ax
	assumes ds,Data
	mov	InSrcBlt,0		;done with source blt

cEnd

subttl	Prepare Color Source Blt
page +
;------------------------------------------------------------------------------
; Gets pointer to Color bitmap and update bFirst_Offset and bCPU_Align bits
; in the CMD_SET
; Entry:
;	ds:si -> lpSrcDev
;
;------------------------------------------------------------------------------
cProc	PrepareColorSourceBlt,<NEAR>
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
cBegin
;
;Convert width and srcx to bytes unit
;
;###patch12
if	1
mov	delta_x,0		;init this first
mov	ax,word ptr xExt
mov	org_xExt,ax

;###patch15
if	1
;test	BWFlag,1		;ViRGE/VX ?
;jnz	nopatch12c		;yes. skip it
cmp	ax,es:patch15_x1
jae	@F
mov	cx,es:patch15_x1
jmp	short init_clip
@@:
endif

cmp	ax,patch12_x1		;>57,29,or 16 ?
jb	nopatch12c		;no, skip patch12
mov	cx,patch12_x2
cmp	ax,cx			;<65,33,or 23 ?
jae	nopatch12c

;###patch15
if	1
init_clip:
endif

CheckFIFOSpace TWO_SLOTS, fs
mov	word ptr xExt,cx	;let xExt=65, 33, or 23
sub	cx,ax			;cx=(65,33,23) - org xExt
mov	delta_x,cx		;
mov	cx,ax			;cx=org xExt
push	cx
mov	ax,DestxOrg		 ;
mov	cx,ax
shl	eax,16
mov	ax,cx
pop	cx			;use cx=org xExt to clip away extra pixels
add	ax,cx			;
dec	ax
EngineWrite B_CLIP_L_R, fs
mov	ax,DestyOrg		 ;
mov	cx,ax
shl	eax,16
mov	ax,cx
add	ax,yExt 		;
dec	ax
EngineWrite B_CLIP_T_B, fs
or	lCmd,bClip_Enable

mov	dx,delta_x
mov	ax,dx
mov	cl,[si].deBitsPixel	;get the Bpp value (8,16,or 32)
shr	cl,4			;cl = 0,1,2
shl	dx,cl			;dx = srcx in bytes
cmp	[si].deBitsPixel,24
jne	short @F
add	dx,ax

;###patch15
if	1
cmp	org_xExt,3
jne	short @F
sub	dx,4		;because one dummy pixel already transfered
endif

@@:
add	dx,3
shr	dx,2
mov	delta_x,dx		;dummy dword to transfer
nopatch12c:
endif
	xor	ebx,ebx
	xor	eax,eax
	xor	edx,edx
	mov	cl,[si].deBitsPixel	;get the Bpp value (8,16,or 32)
	mov	bx,SrcxOrg
;###patch12
if	1
	mov	ax,org_xExt
else
	mov	ax,xExt
endif
	shr	cl,4			;cl = 0,1,2
	shl	ebx,cl			;ebx = srcx in bytes
	shl	eax,cl			;eax = xExt in bytes
	cmp	[si].deBitsPixel,24
	jne	short @F
	mov	dx,SrcxOrg
	add	ebx,edx
;###patch12
if	1
	mov	dx,org_xExt
else
	mov	dx,xExt
endif
	add	eax,edx
@@:
	mov	dAdjXExt,eax		;save it
;###patch12
if	1
;always transfer scan by scan
else
	cmp	eax,[si].deDeltaScan	;xExt same as bitmap width ?
	je	short PCSBAll		;yes, prepare to xfer the whole bitmap
endif

;
;DWord-aligned starting and ending X-axis of source rectangle
;xExt = (((srcx & 3) + xExt) + 3) & FCh
;ebx = srcx_bytes
;eax = xExt_bytes
;
PCSBPerLine:
	or	lCmd,CPUAlign_dword	;set to dword-aligned
	mov	wTransferAll,0		;xfer line by line
	xor	edi,edi
	jmp	short PCSB_Calc 	;eax = adjusted xExt in bytes

;ebx = srcx_bytes
;eax = xExt_bytes
PCSBAll:
	mov	wTransferAll,1		;xfer the whole bitmap
	mov	di,bx			;srcx_bytes
	and	edi,3			;di = #bytes to adjust
	jz	short PCSBValid
	cmp	di,1			;one byte invalid ?
	jne	short @f		;no, keep checking
	or	lCmd,bFirst_1byte	;
	jmp	short PCSBValid 	;
@@:
	cmp	di,2			;2 bytes invalid ?
	jne	short @f
	or	lCmd,bFirst_2bytes
	jmp	short PCSBValid
@@:
	or	lCmd,bFirst_3bytes
PCSBValid:
	sub	bx,di			;ebx = adjusted srcx_bytes

	test	ax,2
	jnz	short @F
	or	lCmd,CPUAlign_dword	;set to dword-aligned
	jmp	short PCSB_Calc 	;no invalid bytes in first dword
@@:
	or	lCmd,CPUAlign_word	;set to word-aligned

;ebx = dword-aligned srcx_bytes
;ax = xExt_bytes
;edi = #bytes to adjust left

PCSB_Calc:
	mov	wAdjust,di
	mov	ax,SrcyOrg
	mul	dword ptr [si].deDeltaScan ;eax = y byte offset

	mov	edx,[si].deDeltaScan	;edx = delta scan
	lds	esi,[si].deBits 	;points to source bitmap

	add	esi,eax 		;
	add	esi,ebx 		;ds:[esi]-->1st src byte to xfer

	mov	dLineToLineInc,edx	;
	sub	esi,edi 		;do possible adjustment
	mov	dSrcBitmapAddr,esi	;save it
	mov	ax,ds			;
	mov	wBitmapSeg,ax		;save it
cEnd

;
subttl	Perform BitBLT involving Color source pattern
page +
;------------------------------------------------------------------------------
; COLOR SOURCE BLT
; Entry:
;	fs --> MMIO Selector
;------------------------------------------------------------------------------
cProc	ColorSourceBlt,<NEAR>
cBegin

;###patch12
if	1
mov	ax,es
mov	gs,ax
assumes gs,Data
endif
	lds	di,lpSrcDev		;DS:DI -> src pdevice
	mov	InSrcBlt,0FFh		;let VDD know we're starting srcblt
	WaitEngineIdle fs		;

	mov	ax,xExt
	dec	ax
	rol	eax,16			;in high dword
	mov	ax,yExt 		;get height
	EngineWrite B_RWIDTH_HEIGHT	;

	mov	ax,DestxOrg		;
	rol	eax,16			;in high dword of B_RDEST_XY
	mov	ax,DestyOrg		;
	EngineWrite B_RDEST_XY		;


	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET

	cld
	push	es			;save es
	mov	ax,fs			;
	mov	es,ax			;ES -> MMIOSelector
	mov	ax,wBitmapSeg		;
	mov	ds,ax			;
	xor	eax,eax 		;
	mov	ax,yExt 		;ax = height
	mov	ebx,dAdjXExt		;ebx = #bytes to transfer
	mov	esi,dSrcBitmapAddr	;ds:esi -> src bitmap
	test	wTransferAll,1		;transfer whole bitmap at once ?
;###	    jnz     short B_ColorAll	    ;yes
	jnz	B_ColorAll	  ;yes
	mov	edx,dLineToLineInc	;
	add	bx,3
	shr	bx,2		       ;ebx = #dwords to transfer
;
;Transfer color source data to the image xfer registers
;ES:[IMAGE_XFER] <= color source data
;
	dec	ax
	jz	short last_scan
B_ColorLoop:
@@:
	xor	edi,edi 		;IMAGE_XFER
	mov	ecx,ebx 		;#dwords to xfer per scanline
	push	esi
;###
if	1;pci_fix
push	ebx
B_ColorInnerLoop:
cmp	bx,X_CNT
jle	b16
mov	cx,X_CNT
b16:
sub	bx,cx
CheckFIFOSpace SIXTEEN_SLOTS
	rep	movs dword ptr es:[edi], ds:[esi]
mov	cx,bx
test	bx,bx
jnz	B_ColorInnerLoop
pop	ebx
endif
;###patch12
if	1
mov	cx,gs:delta_x
rep	stosd
endif
	pop	esi
	add	esi,edx 		;move source bitmap ptr to next scanline
	dec	ax			;height--
	jnz	short @b		;keep looping

;pay special attention to last few byte(s) of last scan to avoid GP Fault
last_scan:
	mov	ecx,ebx
	dec	ecx
	jz	short last_dword
;###
if	1;pci_fix
mov	ebx,ecx
push	ebx
B_InnerLoop:
cmp	bx,X_CNT
jle	@F
mov	cx,X_CNT
@@:
sub	bx,cx
CheckFIFOSpace SIXTEEN_SLOTS
	rep	movs dword ptr es:[edi], ds:[esi]
mov	cx,bx
test	bx,bx
jnz	B_InnerLoop
pop	ebx
endif

last_dword:
	mov	ebx,dAdjXExt		;ebx = #bytes to transfer
All_last_dword:
	and	bl,3
	jz	short last_4
	cmp	bl,1
	jne	short @F
	mov	al,ds:[esi]
	jmp	short last_out
@@:
	cmp	bl,2
	jne	short @F
	mov	ax,ds:[esi]
	jmp	short last_out
@@:
	mov	ax,ds:[esi]
	shl	eax,16
	mov	al,ds:[esi+2]
	rol	eax,16
	jmp	short last_out
last_4:
	mov	eax,ds:[esi]
last_out:
	mov	es:[edi],eax
;###patch12
if	1
mov	cx,gs:delta_x
rep	stosd
endif
;###	    jmp     short B_ColorCleanup    ;we're done
	jmp	B_ColorCleanup	  ;we're done

B_ColorAll:
	mul	ebx			;xExt_bytes * yExt
	movzx	ecx,wAdjust
	add	ecx,eax 		;ecx = total bytes to transfer
	mov	ebx,ecx
	add	ecx,3
	shr	ecx,2
	xor	edi,edi 		;IMAGE_XFER
	cmp	ecx,2000h		 ;is bitmap > 32K bytes (FIFO size) ?
	ja	short CSBVeryLarge	;yes, go handle it
	dec	cx
	jz	short All_last_dword
;###
if	1;pci_fix
push	ax
mov	ax,cx
B_ChkFIFOLoop:
cmp	ax,X_CNT
jle	@F
mov	cx,X_CNT
@@:
sub	ax,cx
CheckFIFOSpace SIXTEEN_SLOTS
	rep	movs dword ptr es:[edi], ds:[esi]
mov	cx,ax
test	ax,ax
jnz	B_ChkFIFOLoop
pop	ax
endif
;###	jmp	short All_last_dword
	jmp	All_last_dword
;	 jmp	 short B_ColorCleanup	 ;we're done

CSBVeryLarge:
	dec	ecx			;special process for last dword
	mov	edx,ecx 		;edx = dwords to transfer
@@:	sub	edx,2000h		;update counter
	mov	ecx,2000h		;32K bytes max to transfer
;###
if	1;pci_fix
push	ax
mov	ax,cx
B_InnerLoop1:
cmp	ax,X_CNT
jle	short b1
mov	cx,X_CNT
b1:
sub	ax,cx
CheckFIFOSpace SIXTEEN_SLOTS
	rep	movsd es:[edi],ds:[esi] ;
mov	cx,ax
test	ax,ax
jnz	B_InnerLoop1
pop	ax
endif
	xor	edi,edi 		;reset ptr to beginning of IMAGE_XFER
	cmp	edx,2000h		;stil larger than the FIFO buffer
	ja	short @b		;yes, break it up
	or	edx,edx
	jz	All_last_dword
	mov	ecx,edx 		;xfer the rest
;###
if	1;pci_fix
push	ax
mov	ax,cx
B_InnerLoop2:
cmp	ax,X_CNT
jle	short b2
mov	cx,X_CNT
b2:
sub	ax,cx
CheckFIFOSpace SIXTEEN_SLOTS
	rep	movsd es:[edi],ds:[esi] ;
mov	cx,ax
test	ax,ax
jnz	B_InnerLoop2
pop	ax
endif
	jmp	All_last_dword

B_ColorCleanup:
	pop	es
	mov	ax,DGROUP
	mov	ds,ax
	assumes ds,Data
	mov	InSrcBlt,0		;done with srcblt

cEnd
;
subttl	Blt screen bitmap.
page +
;------------------------------------------------------------------------------
; ScreenToScreenBlt
; Handles Screen to screen blt
; Entry:
;	fs	--> MMIO Register segment
;------------------------------------------------------------------------------
cProc	ScreenToScreenBlt,<NEAR>
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

;###patch12
if	1
	CheckFIFOSpace SIX_SLOTS, fs
;test	BWFlag,1		;ViRGE/VX ?
;jnz	no_patch12			;yes. skip it
mov	ax,word ptr xExt
cmp	ax,es:patch12_x1	;>57,29,or 16 ?
jb	no_patch12		;no, skip patch12
mov	cx,es:patch12_x2
cmp	ax,cx			;<65,33,or 23 ?
jae	no_patch12
mov	word ptr xExt,cx	;let xExt=65, 33, or 23
mov	cx,ax			;cx=org xExt
push	cx
mov	ax,DestxOrg		 ;
mov	cx,ax
shl	eax,16
mov	ax,cx
pop	cx			;use cx=org xExt to clip away extra pixels
add	ax,cx			;
dec	ax
EngineWrite B_CLIP_L_R, fs
mov	ax,DestyOrg		 ;
mov	cx,ax
shl	eax,16
mov	ax,cx
add	ax,yExt 		;
dec	ax
EngineWrite B_CLIP_T_B, fs
or	lCmd,bClip_Enable
no_patch12:
else
	CheckFIFOSpace FOUR_SLOTS, fs
endif

	mov	ax,DestxOrg
	cmp	ax,SrcxOrg
	jle	@F		  ;x1 >= x2
	add	ax,xExt 		;x1 < x2
	dec	ax
	mov	DestxOrg,ax		;Dstx = x2 + WIDTH-1
	mov	ax,SrcxOrg		;
	add	ax,xExt 		;
	dec	ax			;
    	mov	SrcxOrg,ax		;Srcx = x1 + WIDTH-1
	and	lCmd,not X_Positive	;set negative x direction
@@:
	mov	ax,DestyOrg
	cmp	ax,SrcyOrg		;
	jle	short @F		;y1 >= y2
	add	ax,yExt 		;y1 < y2
	dec	ax			;
	mov	DestyOrg,ax		;Dsty = y2 + HEIGHT-1
	mov	ax,SrcyOrg		;
	add	ax,yExt 		;
	dec	ax			;
	mov	SrcyOrg,ax		;Srcy = y1 + HEIGHT-1
	and	lCmd,not Y_Positive	;set negative y direction
@@:
	mov	eax,dword ptr yExt	;load xExt and yExt into EAX
	sub	eax,00010000h		;xExt - 1 and yExt
	EngineWrite B_RWIDTH_HEIGHT

	mov	eax,dword ptr SrcyOrg
	EngineWrite B_RSRC_XY

	mov	eax,dword ptr DestyOrg
	EngineWrite B_RDEST_XY

	mov	eax,lCmd
	WriteCMDAutoOFF B_CMD_SET
;
STSBExit:
cEnd

IFDEF ASPEN
PPROC	MonoPat_step1	near
;	or	dCmd,RECT_FILL + bPAT_Mono

	CheckFIFOSpace FOUR_SLOTS	;now we're going to fill the rect
					;with background colour first.
	push	es
	push	di
	les	di,lpDrawMode		;ES:DI-->lpDrawMode
	mov	eax,es:[di].bkColor	;
	EngineWrite B_PAT_FG_CLR
	pop	di
	pop	es
;
if patch16
	test	BWFlag,08
	jz	skip_p_16
	mov	ax,word ptr xExt
	cmp	ax,20
	jle	@F
	mov	ax,20
@@:
	dec	ax
	shl	eax,16
	mov	ax,1

	EngineWrite B_RWIDTH_HEIGHT

	mov	eax,dword ptr DestyOrg
	EngineWrite B_RDEST_XY			;set destx, desty

	mov	eax,RECT_SOLID
	WriteCMDAutoOFF B_CMD_SET
skip_p_16:
endif
	mov	eax,dword ptr yExt		;load xExt and yExt into EAX
	sub	eax,00010000h			;xExt - 1 and yExt
	EngineWrite B_RWIDTH_HEIGHT

	mov	eax,dword ptr DestyOrg
	EngineWrite B_RDEST_XY			;set destx, desty

	mov	eax,RECT_SOLID
	WriteCMDAutoOFF B_CMD_SET

	ret

MonoPat_step1	endp

PPROC	HatchPat_step1	near
;	or	dCmd,RECT_FILL + bPAT_Mono

	CheckFIFOSpace FOUR_SLOTS	;now we're going to fill the rect
					;with background colour first.
	lds	si,lpPBrush		;DS:SI-->lpPhysBrush
	mov	eax,[si].dp8BgColor	;
	EngineWrite B_PAT_FG_CLR
;
if patch16
	test	BWFlag,08
	jz	skp_p_16
	mov	ax,word ptr xExt
	cmp	ax,20
	jle	@F
	mov	ax,20
@@:
	dec	ax
	shl	eax,16
	mov	ax,1

	EngineWrite B_RWIDTH_HEIGHT

	mov	eax,dword ptr DestyOrg
	EngineWrite B_RDEST_XY			;set destx, desty

	mov	eax,RECT_SOLID
	WriteCMDAutoOFF B_CMD_SET
skp_p_16:
endif
	mov	eax,dword ptr yExt		;load xExt and yExt into EAX
	sub	eax,00010000h			;xExt - 1 and yExt
	EngineWrite B_RWIDTH_HEIGHT

	mov	eax,dword ptr DestyOrg
	EngineWrite B_RDEST_XY			;set destx, desty

;	mov	eax,dCmd
	mov	eax,RECT_SOLID
	WriteCMDAutoOFF B_CMD_SET
	ret

HatchPat_step1	endp
ENDIF ;ASPEN
;

sEnd	Code
end
