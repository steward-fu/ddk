;------------------------------------------------------------------------------
; S3BLT.ASM
; Copyright (c) 1994 Microsoft Corporation
; Copyright (c) 1996 S3 Incorporated
;------------------------------------------------------------------------------
;
.xlist
DOS5 = 1
include cmacros.inc
include macros.inc
incDevice   = 1 			; GDIDEFS - Include device control defs
incDrawMode = 1 			; GDIDEFS - Include DRAWMODE structure
incLogical  = 1 			; GDIDEFS - Include brush styles (BS_x)
include gdidefs.inc
include dibeng.inc
include S3.inc				; S3 specific defines

VIRGE_ID_HIGH	equ	08ah

;------------------------------------------------------------------------------
; LOCAL EQUATES:
;------------------------------------------------------------------------------
;
DstPr			equ	000000000B ; Dest included into Blt
SrcPr			equ	000000100B ; Source included into Blt
PatPr			equ	000000010B ; Pattern included into Blt

.list

;------------------------------------------------------------------------------
; E X T E R N S
;------------------------------------------------------------------------------
;
externFP    DIB_BitBlt			; DIB_ENGINE: Performs src -> dest blt
externFP    BeginAccess 		; ACCESS.ASM: Begin frame buff access
externFP    EndAccess			; ACCESS.ASM: End frame buffer access

;------------------------------------------------------------------------------
; D A T A
; NOTE: Data is aligned on DWORD boundaries at the beginning of the segment
;	to maintain the overall data type alignment.  Within the data segment,
;	data should be organized from largest (DWORD) to smallest (BYTE) order
;	to minimize CPU data misalignment access penalties.
;------------------------------------------------------------------------------
;
sBegin	Data
;
	ALIGN	4

externD     lpDriverPDevice		; ENABLE.ASM: Screen PDEVICE struct ptr
externD     dDestFmt			; VGA.ASM:  Bpp in CMD_SET format
externW     wScreenWidthBytes		; VGA.ASM:  Bytes per scan line
externW     wMMIOSelector		; VGA.ASM:  Engine MMIO selector
externW     wBWwidth			; INIT.ASM: BlockWrite width in bytes
externW     wBpp			; INIT.ASM: Bits per pixel (8/16/24/32)
externB     BWFlag			; INIT.ASM: Engine BlockWrite status
externB     wCursorType 		; VGA.ASM:  Hardware/Software cursor
externB     bInDDraw
externB     DeviceId_High		; VGA.ASM
IFDEF	ASPEN ;-------------------------
externB     bRop			; VGA.ASM:  Raster Operation byte
ENDIF ; ASPEN ;-------------------------
;
IFDEF	VIRGE_PATCH12 ;-----------------
externW     patch12_x1			; VGA.ASM
externW     patch12_x2			; VGA.ASM
ENDIF ; VIRGE_PATCH12 ;-----------------

IFDEF	VIRGE_PATCH15 ;-----------------
externW     patch15_x1			; VGA.ASM
ENDIF ; VIRGE_PATCH15 ;-----------------

globalD     dCmd,0			; current value in CMD_SET
IFDEF	VIRGE_PATCH12 ;-----------------
globalW     delta_x,0			;
globalW     org_xExt,0			;
ENDIF ; VIRGE_PATCH12 ;-----------------
globalB     InSrcBlt,0			; flag for image xfer in operation
;
externB S3MobileData			;in MOBILE.ASM
sEnd	Data

;------------------------------------------------------------------------------
; C O D E
;------------------------------------------------------------------------------
;
sBegin	Code
	assumes cs,Code
.386


;------------------------------------------------------------------------------
; PROCEDURE:	BitBLT_S3:
;
; DESCRIPTION:	Performs bit block transfer to the screen depending on the ROP
;		(raster operation) specified.
;
; ASSUMPTIONS:	None (all 2D graphics engine registers may have been modified
;		    by other drivers).
;
; CALLS:	[S3BeginAccessAddr] - Global engine access serialization code
;		[S3EndAccessAddr]   - Global engine access serialization code
;
; ENTRY  REGS:	SS:SP = Stack frame (return address and parameters)
;		  GS: = Driver data selector
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	EAX, EBX, ECX, EDX
;
; NOTES:	ES: is used for the MMIO selector to facilitate block transfers
;		without the overhead of pushing/loading/popping selectors.
;------------------------------------------------------------------------------
;
	ALIGN	16

cProc	BitBLT_S3,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	include bbparms.inc
	include bblocals.inc
cBegin
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	lfs	bx,ss:[lpDestDev]	; DS:SI = dest pdevice struct ptr
	mov	dx,fs:[bx.deFlags]	; AX = DIBENGINE struct status flags
	test	dx,VRAM 		; on-screen/offscreen mem destination ?
	jz	B_JmpToDIBEngine	; no, let DIB engine handle it
	test	dx,PALETTE_XLAT 	; background palette xlation needed ?
	jnz	B_JmpToDIBEngine	; yes, let DIB engine handle it
	bts	fs:[bx.deFlags],BUSY_BIT ; set dest device busy, CF=1 if already
	jc	B_JmpToDIBEngine	; already busy, let DIB engine handle

	mov	es,gs:[wMMIOSelector]	; ES: = engine MMIO selector

;### do a dummy blt to clear up state machine after 3D command

	cmp	gs:[bInDDraw],0 	; need to patch?
	je	short @F		; N, skip

	WaitDMA es
	CheckFIFOSpace SIX_SLOTS, es	; wait for FIFO space available

	xor	eax,eax
	EngineWrite B_SRC_BASE	    es	; source base coords
	EngineWrite B_DEST_BASE     es	; dest base coords
	inc	eax			; EAX = width/height, source/dest X,Y
	EngineWrite B_RWIDTH_HEIGHT, es ; write EAX = bitblt width/height
        xor     eax,eax
        EngineWrite B_RSRC_XY, es	; write EAX = bitblt source X,Y
	EngineWrite B_RDEST_XY, es	; write EAX = bitblt dest   X,Y
	mov	eax,[dDestFmt]		;
	or	eax,(07980000h + bDraw + CPUAlign_dword + bFirst_dword) ;
	EngineWrite B_CMD_SET, es	; write EAX=bitblt restores engine state
@@:
;
; Perform engine command/BitBlt procedure setup
;
.ERRE	((DstPr OR PatPr OR SrcPr) EQ 06h) ; ERROR: Code assumes max index=06h

	mov	ax,gs:[wScreenWidthBytes] ; AX = DES stride in bytes per scanline
	mov	di,wptr ss:[Rop + 2]	; BX = raster operation (ROP) code
	mov	cx,ax			; CX = SRC stride in bytes per scanline

	xor	esi,esi 		; ESI = default dest base address
	mov	bx,di			; DI = ROP256 code

IFDEF	ASPEN ;-------------------------
	mov	gs:[bRop],bl		; save BL = ROP256 raster operation code
ENDIF ; ASPEN ;-------------------------

	shl	edi,17			; shift into b8ROP_256, CMD_SET [24:17]

	or	edi,(Y_Positive + X_Positive + CPUAlign_dword + bDraw) ; default
	shl	eax,16			; EAX.H = destination stride
	or	edi,gs:[dDestFmt]	; combine with Bpp destination format
	mov	ax,cx			; EAX.H=dest stride, EAX.L=source stride
	mov	bl,byte ptr cs:[BltTypeTable + bx] ; BX=blt dispatch table index
	mov	ss:[lCmd],edi		; save local var lCmd = ROP256+destfmt
	mov	bx,cs:[BltDispatchTable + bx] ; BX = BitBlt routine offset
	mov	ecx,esi 		; default for source base is dest base

	CheckFIFOSpace THREE_SLOTS  es	; wait for FIFO slots available

	EngineWrite B_SRC_BASE	    es, ecx ; write ECX = source base coords
	EngineWrite B_DEST_BASE     es, esi ; write ESI = dest	 base coords
	EngineWrite B_DEST_SRC_STR  es, eax ; write EAX = source/dest stride

	cld				; DF=0, forward string movement
	call	bx			; perform specific BitBlt operation

PLABEL B_Done
	mov	ax,1			; return success
	lds	si,ss:[lpDestDev]	; DS:SI = dest pdevice struct ptr
	and	ds:[si.deFlags],(NOT BUSY) ; free destination device surface
PLABEL B_Exit

cEnd ;	BitBLT_S3

;------------------------------------------------------------------------------
; BitBlt function dispatch table
;
	ALIGN	2

BltDispatchTable    label   word

	dw	DoBltNoDSP		; D/NoDSP
	dw	DoBltDP 		; P/DP
	dw	DoBltDS 		; S/DS
	dw	DoBltDSP		; SP/DSP


;------------------------------------------------------------------------------
; PROCEDURE:	DoBltD:
;		DoBltNoDSP:
;
; DESCRIPTION:	DoBltD	   - This function is called when the passed Rop = D
;			     (DESTINATION) or Dn (DESTINATION NOT) and the
;			     destination bitmap is the screen.
;		DoBltNoDSP - This function is called when the passed Rop = 00h
;			     (BLACKNESS) or FFh (WHITENESS) and the destination
;			     bitmap is the screen.
;
; ASSUMPTIONS:	None
;
; CALLS:	None
;
; ENTRY  REGS:	SS:BP = BitBlt stack parameters/return address
;		ES: = Engine MMIO selector
;		GS: = Driver data selector (DGROUP)
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	EAX, EBX, ECX, EDX, ESI, EDI
;
; NOTES:	None
;------------------------------------------------------------------------------
;
	ALIGN	4

PLABEL	DoBltD
cProc	DoBltNoDSP,<NEAR,PUBLIC>
cBegin
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	mov	bx,word ptr ss:[Rop + 2] ; BX = raster operation (ROP) code
	mov	ax,offset @F		; AX = return addr from SW curs exclude
	cmp	bx,0AAh 		; ROP == D (destination, do nothing) ?
	je	NoDSP_Exit		; yes, exit (no action on dest surface)
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using a software cursor ?
	je	B_SWCursorExcludeRect	; yes, go exclude the S/W cursor
@@:
	mov	ebx,ss:[lCmd]		; EBX = BitBlt operation cmd

IFDEF	VIRGE_PATCH15 ;-----------------
;###patch15
	cmp	gs:[DeviceId_High],VIRGE_ID_HIGH
	je	DBD_Patch15Done 	; yes, skip Patch15 as it doesn't apply
;	test	gs:[BWFlag],01h 	; Virge/VX hardware present ?
;	jnz	DBD_Patch15Done 	; yes, skip Patch15 as it doesn't apply

;
; Apply hardware clipping to the destination BitBlt rectangle as necessary
; Regs: EBX = Engine BitBlt operation cmd
;
	mov	ax,ss:[xExt]		; AX = BitBlt rectangle X extent
	mov	cx,gs:[patch15_x1]	; CX = errata workaround width boundary
	cmp	ax,cx			; rect width requires HW clipping ?
	jae	DBD_Patch15Done 	; no, skip HW clipping enable
	mov	ss:[xExt],cx		; set xExt=65, 33, or 23 rect width

	mov	si,ss:[DestxOrg]	; SI = dest rect Xorg
	mov	di,ss:[DestyOrg]	; DI = clip rect.top
	mov	cx,si			; CX = dest rect Xorg
	mov	dx,di			; DX = clip rect.top
	shl	esi,16			; ESI.H = dest rect Xorg
	shl	edi,16			; EDI.H = clip rect.top
	mov	si,cx			; ESI.H=dest rect Xorg, ESI.L=rect Xorg
	mov	di,dx			; EDI.H=clip.top, EDI.L=clip.top (also)
	add	si,ax			; ESI.H=clip.left, ESI.L=clip.right
	add	di,ss:[yExt]		; EDI.H=clip.top, EDI.L=clip.bottom
	dec	si			; ESI.L = clip.right (0-based)
	dec	di			; EDI.L = clip.bottom (0-based)

	CheckFIFOSpace	TWO_SLOTS, es	; wait for FIFO slots available

	EngineWrite B_CLIP_L_R, es, esi ; write ESI = clip rect left/right
	EngineWrite B_CLIP_T_B, es, edi ; write EDI = clip rect top/bottom
	or	ebx,bClip_Enable	; enable HW clipping in cmd reg
PLABEL DBD_Patch15Done
ENDIF ; VIRGE_PATCH15 ;-----------------

	mov	edi,dword ptr ss:[DestyOrg] ; EDI.H=DestX coord, EDI.L=DestY

IFNDEF	VIRGE_REVAB_PATCH ;-------------
IFDEF	VIRGE_PATCH16 ;-----------------
	test	gs:[BWFlag],08h 	; ViRGE/VX running in fast page mode ?
	jz	DBD_Patch16Done 	; no, skip Patch16 as it doesn't apply
	mov	ax,ss:[xExt]		; AX = bitblt rect X extent
	cmp	ax,20			; blt rectangle width < 20 ?
	jl	@F			; yes, use X extent as is
	mov	ax,20			; no, set new maximum X extent
@@:
	dec	ax			; AX = blt rectangle width (0-based)
	shl	eax,16			; EAX.H = blt rectangle width (0-based)
	mov	ax,1			; EAX.H=width (0-based), EAX.L=height

	CheckFIFOSpace	THREE_SLOTS, es ; wait for FIFO slots available

	EngineWrite B_RWIDTH_HEIGHT es, eax ; write EAX = blt rect width/height
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = rect dest X,Y coords
	WriteCMDAutoOFF B_CMD_SET   es, ebx ; write EBX = bitblt draw cmd
PLABEL DBD_Patch16Done
ENDIF ; VIRGE_PATCH16 ;-----------------
ENDIF ; VIRGE_REVAB_PATCH ;-------------

;
; Setup registers for the destination BitBlt operation
; NOTE: The engine provides the WHITENESS/BLACKNESS/Destination(NOT) patterns
;	so no additional register loading is necessary.
; Regs: EBX = Engine BitBlt operation cmd
;
	mov	eax,dword ptr ss:[yExt]     ; EAX.H = xExt extent, EAX.L = yExt
IFDEF	VIRGE_REVAB_PATCH ;-------------
	or	ebx,MONOPATBLT		    ; EBX = mono blt (use intern colors)
ELSE ; !VIRGE_REVAB_PATCH ;-------------
	or	ebx,(RECT_FILL + bPAT_Mono) ; EBX = bROP+destFmt+Rect
ENDIF ; VIRGE_REVAB_PATCH ;-------------
	sub	eax,00010000h		    ; EAX.H=xExt (0-based), EAX.L=yExt

	CheckFIFOSpace	THREE_SLOTS es	; wait for FIFO slots available

	EngineWrite B_RWIDTH_HEIGHT es, eax ; write EAX=BitBlt rect X/Y extent
	EngineWrite B_RDEST_XY	    es, edi ; write EDI=BitBlt rect dest X,Y
	WriteCMDAutoOFF B_CMD_SET,  es, ebx ; write EBX=BitBlt rect draw cmd
PLABEL NoDSP_Exit

cEnd ;	DoBltNoDSP/DoBltD


;------------------------------------------------------------------------------
; PROCEDURE:	DoBltDP:
;
; DESCRIPTION:	This function is called when the passed ROP involves the screen
;		destination bitmap and a Pattern (Solid, Color pattern, or Mono
;		pattern).
;
; ASSUMPTIONS:	None
;
; CALLS:	PatternBlt
;
; ENTRY  REGS:	SS:BP = BitBlt stack parameters/return address
;		ES: = Engine MMIO selector
;		GS: = Driver data selector (DGROUP)
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	EAX, EBX, ECX, EDX
;
; NOTES:	None
;------------------------------------------------------------------------------
;
	ALIGN	4

cProc	DoBltDP,<NEAR,PUBLIC>
cBegin
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	mov	ax,offset @F		; AX = return addr from SW curs exclude
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using a software cursor ?
	je	B_SWCursorExcludeRect	; yes, go exclude the S/W cursor
@@:
	call	PatternBlt		; xfer mono/color/hatch pattern image
	jc	DP_Exit 		; nothing to draw, goto exit
	mov	esi,dword ptr ss:[yExt]     ; ESI.H = xExt extent, ESI.L = yExt
	mov	edi,dword ptr ss:[DestyOrg] ; EDI.H=DestX coord, EDI.L=DestY
	sub	esi,00010000h		    ; ESI.H=xExt (0-based), ESI.L=yExt
	mov	ebx,ss:[lCmd]		    ; EBX = BitBlt draw cmd DWORD

	CheckFIFOSpace	THREE_SLOTS es	; wait for FIFO slots available

	EngineWrite B_RWIDTH_HEIGHT es, esi ; write ESI=BitBlt rect X/Y extent
	EngineWrite B_RDEST_XY	    es, edi ; write EDI=BitBlt rect dest X,Y
	WriteCMDAutoOFF B_CMD_SET,  es, ebx ; write EBX=BitBlt rect draw cmd
PLABEL DP_Exit

cEnd ;	DoBltDP


;------------------------------------------------------------------------------
; PROCEDURE:	DoBltDS:
;
; DESCRIPTION:	This function is called when the passed Rop involves a Source
;		(ScreentoScreen, Color source, or Mono source) and destination.
;
; ASSUMPTIONS:	None
;
; CALLS:	ClipSrcBitmap
;		ColorSourceBlt
;		MonoSourceBlt
;		ScreenToScreenBlt
;
; ENTRY  REGS:	SS:BP = BitBlt stack parameters/return address
;		ES: = Engine MMIO selector
;		GS: = Driver data selector (DGROUP)
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	EAX, EBX, ECX, EDX
;
; NOTES:	None
;------------------------------------------------------------------------------
;
	ALIGN	4

cProc	DoBltDS,<NEAR,PUBLIC>
cBegin
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	mov	ax,ss:[lpSrcDev.sel]	; get src pdevice selector #50819
	test	ax,ax			; is there SRC pdevice
	jz	B_ReleaseAndCallDIBEngine ; no, let DIB engine handle it

	lds	si,ss:[lpSrcDev]	; DS:SI = source pdevice struct ptr
	mov	ax,ds:[si.deFlags]	; AX = source pdevice flags
	test	ax,VRAM 		; on-screen/offscreen memory source ?
	jnz	DS_ScreenBlt		; yes, go do screen to screen blt
	test	ax,ax			; source is a mono bitmap ?
	jnz	DS_ColorSrcBlt		; yes, either color or mono source

;------------------------------------------------------------------------------
; Mono Source BitBlt
; Regs: DS:SI = lpSrcDev source device structure ptr
;	   AX = lpSrcDev.deFlags source device flags (0000h)
;
PLABEL DS_MonoSrcBlt
	cmp	ds:[si.deBitsPixel],1	; is source bitmap truly mono ?
	jne	B_ReleaseAndCallDIBEngine ; no, let DIB engine handle it
	cmp	ds:[si.bmSegmentIndex],ax ; single plane segment (index = 0) ?
	jne	B_ReleaseAndCallDIBEngine ; no, let DIB engine handle it
@@:
	call	ClipSrcBitmap		; clip source bitmap to clip rectangle
	mov	ax,offset @F		; AX = return addr from SW curs exclude
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using a software cursor ?
	je	B_SWCursorExcludeRect	; yes, go exclude the S/W cursor
@@:
	or	ss:[lCmd],MONOSRCBLT+bClip_Enable ; enab HW clip, mono src xfer
	jmp	MonoSourceBlt		; setup/do mono bitmap image BitBlt

;------------------------------------------------------------------------------
; Color Source Blt
; Regs: DS:SI = lpSrcDev source device structure ptr
;
PLABEL DS_ColorSrcBlt
	call	ClipSrcBitmap		; clip source bitmap to clip rectangle
	mov	ax,offset @F		; AX = return addr from SW curs exclude
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using a software cursor ?
	je	B_SWCursorExcludeRect	; yes, go exclude the S/W cursor
@@:
	or	ss:[lCmd],CLRSRCBLT	; color source BitBlt cmd
	jmp	ColorSourceBlt		; setup/do color source image BitBlt

;------------------------------------------------------------------------------
; Screen to Screen Blt
; Regs: DS:SI = lpSrcDev source device structure ptr
;
PLABEL DS_ScreenBlt
	call	ClipSrcBitmap		; clip source bitmap to clip rectangle
	mov	ax,offset @F		; AX = return addr from SW curs exclude
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using a software cursor ?
	je	B_SWCursorExcludeUnion	; yes, go exclude the S/W cursor
@@:
	or	ss:[lCmd],BITBLT	; screen to screen BitBlt cmd
	jmp	ScreenToScreenBlt	; setup/do screen to screen BitBlt

cEnd ;	DoBltDS


;------------------------------------------------------------------------------
; PROCEDURE:	DoBltDSP:
;
; DESCRIPTION:	This function is called when the passed Rop involves Source,
;		Pattern, and Destination.
;
; ASSUMPTIONS:	None
;
; CALLS:	ClipSrcBitmap
;		ColorSourceBlt
;		MonoSourceBlt
;		PatternBlt
;		ScreenToScreenBlt
;
; ENTRY  REGS:	SS:BP = BitBlt stack parameters/return address
;		ES: = Engine MMIO selector
;		GS: = Driver data selector (DGROUP)
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	EAX, EBX, ECX, EDX
;
; NOTES:	None
;------------------------------------------------------------------------------
;
	ALIGN	4

cProc	DoBltDSP,<NEAR,PUBLIC>
cBegin
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	mov	ax,ss:[lpSrcDev.sel]	; get src pdevice selector #50819
	test	ax,ax			; is there SRC pdevice
	jz	B_ReleaseAndCallDIBEngine ; no, let DIB engine handle it

	lds	si,ss:[lpSrcDev]	; DS:SI = source pdevice struct ptr
	mov	ax,ds:[si.deFlags]	; AX = source pdevice flags
	test	ax,VRAM 		; on-screen/offscreen memory source ?
	jnz	short DSP_ScreenBlt	; yes, go do screen to screen blt
	test	ax,ax			; Is the src a mono bitmap ?
	jnz	short DSP_ColorSrcBlt	; no, go do color to screen blt

;------------------------------------------------------------------------------
; Mono Source BitBlt with/without Pattern
; Regs: DS:SI = lpSrcDev source device structure ptr
;	   AX = lpSrcDev.deFlags source device flags (0000h)
;
PLABEL DSP_MonoSrcBlt
	cmp	ds:[si.deBitsPixel],1	  ; is source pattern truly mono ?
	jne	B_ReleaseAndCallDIBEngine ; no, let DIB engine handle it
	cmp	ds:[si.bmSegmentIndex],ax ; single plane segment (index = 0) ?
	jne	B_ReleaseAndCallDIBEngine ; no, let DIB engine handle it
@@:
	call	ClipSrcBitmap		; clip source bitmap to clip rectangle
	mov	ax,offset @F		; AX = return addr from SW curs exclude
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using a software cursor ?
	je	B_SWCursorExcludeRect	; yes, go exclude the S/W cursor
@@:
	or	ss:[lCmd],bSRC_Mono + bSRC_Sys + bClip_Enable + Y_Positive + \
							bDraw + X_Positive
	call	PatternBlt		; xfer mono/color/hatch pattern image
	jmp	MonoSourceBlt		; setup/do mono bitmap image BitBlt

;------------------------------------------------------------------------------
; Color Source BitBlt with/without Pattern
; Regs: DS:SI = lpSrcDev source device structure ptr
;
PLABEL DSP_ColorSrcBlt
	call	ClipSrcBitmap		; clip source bitmap to clip rectangle
	mov	ax,offset @F		; AX = return addr from SW curs exclude
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using a software cursor ?
	je	B_SWCursorExcludeRect	; yes, go exclude the S/W cursor
@@:
	or	ss:[lCmd],bSRC_Sys	; source BitBlt, CPU provides bmp data
	call	PatternBlt		; xfer mono/color/hatch pattern image
	jmp	ColorSourceBlt		; setup/do color source image BitBlt

;------------------------------------------------------------------------------
; Screen to Screen with/without Pattern
; Regs: DS:SI = lpSrcDev source device structure ptr
;
PLABEL DSP_ScreenBlt
	call	ClipSrcBitmap		; clip source bitmap to clip rectangle
	mov	ax,offset @F		; AX = return addr from SW curs exclude
	cmp	gs:[wCursorType],SOFTWARECURSOR ; using a software cursor ?
	je	B_SWCursorExcludeUnion	; yes, go exclude the S/W cursor
@@:
	call	PatternBlt		; xfer mono/color/hatch pattern image
	jmp	ScreenToScreenBlt	; setup/do screen to screen BitBlt

cEnd ;	DoBltDSP


;------------------------------------------------------------------------------
; PROCEDURE:	PatternBlt:
;
; DESCRIPTION:	Handles the pattern blt (Solid/Color/Mono bit block transfer).
;
; ASSUMPTIONS:	None
;
; CALLS:	None
;
; ENTRY  REGS:	SS:BP = BitBlt stack parameters/return address
;		ES: = Engine MMIO selector
;		GS: = Driver data selector (DGROUP)
;		SS:[lCmd] = dDestFmt + Rop
;
; EXIT	 REGS:	CF = PatternBlt status (0 = Do BitBlt, 1 = Nothing to draw)
;
; MODIFY REGS:	EAX, EBX, ECX, EDX, SI, EDI, DS:
;
; NOTES:	The code path is optimized in the following priority order:
;		1)  BS_SOLID   solid color BitBlt		(high priority)
;		2)  BS_PATTERN color pattern BitBlt		(med. priority)
;		3)  BS_PATTERN mono pattern BitBlt		(low  priority)
;		4)  BS_HATCH   hatched pattern BitBlt		(low  priority)
;		5)  BS_HOLLOW  hollow pattern BitBlt		(not  priority)
;------------------------------------------------------------------------------
;
	ALIGN	4

cProc	PatternBlt,<NEAR,PUBLIC>

cBegin
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data
;
; Branch to the appropriate pattern blt routine based on physical brush style
;
.ERRE	(BS_SOLID   EQ	0)		; ERROR: Assume solid	= 0
.ERRE	(BS_HOLLOW  EQ	1)		; ERROR: Assume hollow	> solid style
.ERRE	(BS_HATCHED EQ	2)		; ERROR: Assume hatched > hollow style
.ERRE	(BS_PATTERN EQ	3)		; ERROR: Assume pattern > hatched style
.ERRE	(BS_PATTERN EQ	MaxBrushStyle)	; ERROR: Assume pattern is last style

	lds	si,ss:[lpPBrush]	; DS:SI pBrush physical brush struc ptr
	mov	al,byte ptr ds:[si.dp8BrushStyle] ; AL = physical brush style
	mov	ah,ds:[si.dp8BrushFlags] ; AH = brush flags, AL = brush style
	cmp	al,BS_HOLLOW		; hollow brush ?
	ja	PB_CheckBrushStylesOther ; no, check BS_HATCH/BS_PATTERN styles
	je	PB_DoNothing		; yes, BS_HOLLOW, exit as nothing to do

;------------------------------------------------------------------------------
; BS_SOLID: Check for mono or color pattern brush flags
; Regs: DS:SI = lpPBrush DIB_Brush attribute/mask/bits structure ptr
;	   AL = lpPBrush.dp8BrushStyle brush style (BS_SOLID)
;	   AH = lpPBrush.dp8BrushFlags brush flags
;
PLABEL PB_SolidBrushTypeChk
	test	ah,COLORSOLID		; solid color brush type ?
	jz	PB_ColorPatBlt		; no, do color pattern blt

;------------------------------------------------------------------------------
; BS_SOLID: SOLID COLOR BLT
; Regs: DS:SI = lpPBrush DIB_Brush attribute/mask/bits structure ptr
;
PLABEL PB_SolidPatBlt
	or	ss:[lCmd],MONOPATBLT	; treat solid brush like mono pattern
	mov	ecx,dword ptr ds:[si.dp8BrushBits] ; ECX = solid foregnd color
	or	edx,0FFFFFFFFh		; EDX = expand bits to foreground color

	CheckFIFOSpace THREE_SLOTS  es	; wait for FIFO slots available

	EngineWrite B_MONO_PAT_0,  es, edx ; write EDX = mono pattern bytes 3:0
	EngineWrite B_MONO_PAT_1,  es, edx ; write EDX = mono pattern bytes 7:4
	EngineWrite B_PAT_FG_CLR,  es, ecx ; write ECX = foreground color
PB_Success:
	clc				; CF=0, ready to do BitBlt operation
PB_Exit:

cEnd ;	PatternBlt

;------------------------------------------------------------------------------
; BS_??????: Check for other brush styles (not BS_SOLID or BS_HOLLOW)
; Regs: DS:SI = lpPBrush physical brush structure ptr
;	   AL = lpPBrush.dp8BrushStyle brush style (not BS_SOLID or BS_HOLLOW)
;	   AH = lpPBrush.dp8BrushFlags brush flags
;
PLABEL PB_CheckBrushStylesOther
	cmp	al,BS_PATTERN		; pattern brush ?
	jb	PB_HatchPatBlt		; no, BS_HATCHED, xfer hatched pattern
	ja	PB_DoNothing		; invalid brush, exit as nothing to do

;------------------------------------------------------------------------------
; BS_PATTERN: COLOR PATTERN BLT
; Regs: DS:SI = lpPBrush physical brush structure ptr
;	   AL = lpPBrush.dp8BrushStyle brush style (BS_PATTERN)
;	   AH = lpPBrush.dp8BrushFlags brush flags
;
PLABEL PB_ColorPatBlt
	test	ah,PATTERNMONO		; monochrome pattern brush ?
	jnz	PB_MonoPatBlt		; yes, go transfer mono pattern brush
	mov	eax,CLRPATBLT		; EAX = BitBlt operation OR mask
	cmp	ss:[xExt],64		; is BitBlt rect width >= 64 ?
	jge	@F			; yes, leave block writes enabled
	mov	eax,(CLRPATBLT + BW_disable) ; no, disable block writes
@@:
	or	ss:[lCmd],eax		; save updated BitBlt draw cmd
;
; Transfer 8x8 color pattern into PATTERN_REGISTERS (adjusted for color depth)
;
.ERRE	(( 8 SHL 1) EQ ((8 * 8) / 4) * 1) ; ERROR:  8Bpp color xfer size wrong
.ERRE	((16 SHL 1) EQ ((8 * 8) / 4) * 2) ; ERROR: 16Bpp color xfer size wrong
.ERRE	((24 SHL 1) EQ ((8 * 8) / 4) * 3) ; ERROR: 24Bpp color xfer size wrong
.ERRE	((32 SHL 1) EQ ((8 * 8) / 4) * 4) ; ERROR: 32Bpp color xfer size wrong

	mov	cx,gs:[wBpp]		; CX = bits per pixel cnt (8/16/24/32)
	add	si,offset dp8BrushBits	; DS:SI = color pattern brush source
	shl	cx,1			; CX = color pattern DWORDs to xfer
	mov	di,COLOR_PATTERN	; ES:DI = color pattern regs dest ptr

	DisablePD  
	WaitEngineIdle es		; wait for idle, all FIFO slots free
	EnablePD  

	rep	movsd			; xfer patt brush to ES:[COLOR_PATTERN]
	clc				; CF=0, ready to do BitBlt operation
	ret				;

PLABEL PB_DoNothing
	stc				; CF=1, do not perform BitBlt operation
	jmp	PB_Exit 		; goto common exit

;------------------------------------------------------------------------------
; BS_PATTERN: MONO PATTERN BLT
; Regs: DS:SI = lpPBrush DIB_Brush attribute/mask/bits structure ptr
;
PLABEL PB_MonoPatBlt
	or	ss:[lCmd],MONOPATBLT	; set mono pattern transfer cmd bit
	lfs	bx,ss:[lpDrawMode]	; FS:BX = DRAWMODE structure ptr

	assumes fs,nothing

IFDEF	ASPEN ;-------------------------
	cmp	gs:[bRop],0F0h		; ROP = F0 (PATCOPY) ?
	jne	PB_MonoAspenDone	; no, skip BitBlt workaround
	cmp	gs:[BWFlag],7		; Block Writes disabled ?
	jne	PB_MonoAspenDone	; no, skip BitBlt workaround
	mov	ax,ss:[xExt]		; AX = BitBlt rectangle width extent
	cmp	ax,gs:[wBWwidth]	; >=CR53.b5 (16/32) ?
	jl	PB_MonoAspenDone	; no, skip BitBlt workaround
	mov	edi,dword ptr ss:[DestyOrg] ; EDI.H=DestXorg, EDI.L=DestYorg
	mov	ecx,fs:[bx.bkColor]	; ECX = mono pattern background color
	mov	edx,RECT_SOLID		; EDX = rectangle draw cmd

	CheckFIFOSpace FOUR_SLOTS   es	; wait for FIFO slots available

	EngineWrite B_PAT_FG_CLR    es, ecx ; write ECX = mono patt fgnd color

IFDEF	VIRGE_PATCH16 ;-----------------
	test	gs:[BWFlag],08h 	; Virge/VX running in fast page mode ?
	jz	PB_MonoPatch16Done	; no, skip patch16 as it doesn't apply
	mov	ax,ss:[xExt]		; AX = bitblt rectangle X extent
	cmp	ax,20			; bitblt rectangle width < 20 ?
	jl	@F			; yes, use X extent as is
	mov	ax,20			; no, AX = maximum rectangle X extent
@@:
	dec	ax			; AX = bitblt rect width (0-based)
	shl	eax,16			; EAX.H = bitblt rect width (0-based)
	mov	ax,1			; EAX.H=rect width 0-based, EAX.L=height

	EngineWrite B_RWIDTH_HEIGHT es, eax ; write EAX = rect width/height
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = DestX, DestX coords
	WriteCMDAutoOFF B_CMD_SET   es, edx ; write EDX = bitblt rect draw cmd

	CheckFIFOSpace	THREE_SLOTS es	; wait for FIFO slots available
PLABEL PB_MonoPatch16Done
ENDIF ; VIRGE_PATCH16 ;-----------------

	mov	eax,dword ptr ss:[yExt] ; EAX.H=xExt extent, EAX.L=yExt extent
	sub	eax,00010000h		; EAX.H=xExt 0-based, EAX.L=yExt extent

	EngineWrite B_RWIDTH_HEIGHT es, eax ; write EAX = rect width/height
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = DestX, DestX coords
	WriteCMDAutoOFF B_CMD_SET   es, edx ; write EDX = bitblt rect draw cmd

	or	ss:[lCmd],bTransparent	; draw as transparent mono pattern
PLABEL PB_MonoAspenDone
ENDIF ; ASPEN ;-------------------------

;
; Transfer the mono pattern into offscreen memory
; NOTE: The monochrome foreground/background bit designations are reversed
;	from the typical designation (typical: 0 = background, 1 = foreground).
;	The foreground/background color settings are reversed to convert bits
;	back to the expected (typical) designations.
; Regs: DS:SI = lpPBrush DIB_Brush attribute/mask/bits structure ptr
;
	mov	dh,ds:[si.dp8BrushMono + 1Ch] ; DH = mono pattern byte 7
	mov	ch,ds:[si.dp8BrushMono + 0Ch] ; CH = mono pattern byte 3
	mov	dl,ds:[si.dp8BrushMono + 18h] ; DL = mono pattern byte 6
	mov	cl,ds:[si.dp8BrushMono + 08h] ; CL = mono pattern byte 2
	shl	edx,16			      ; EDX.H = mono pattern bytes 7:6
	mov	edi,fs:[bx.bkColor]	      ; EDI = foregrnd color (reversed)
	shl	ecx,16			      ; ECX.H = mono pattern bytes 3:2
	mov	ebx,fs:[bx.TextColor]	      ; EBX = backgrnd color (reversed)
	mov	dh,ds:[si.dp8BrushMono + 14h] ; DH = mono pattern byte 5
	mov	ch,ds:[si.dp8BrushMono + 04h] ; CH = mono pattern byte 1
	mov	dl,ds:[si.dp8BrushMono + 10h] ; EDX = mono pattern bytes 7:4
	mov	cl,ds:[si.dp8BrushMono + 00h] ; ECX = mono pattern bytes 3:0

	CheckFIFOSpace FOUR_SLOTS   es	; wait for FIFO slots available

	EngineWrite B_MONO_PAT_0, es, ecx ; write ECX = mono pattern bytes 3:0
	EngineWrite B_MONO_PAT_1, es, edx ; write EDX = mono pattern bytes 7:4
	EngineWrite B_PAT_BG_CLR, es, ebx ; write EBX = background color
	EngineWrite B_PAT_FG_CLR, es, edi ; write EDI = foreground color
	clc				; CF=0, ready to do BitBlt operation
	ret				;

;------------------------------------------------------------------------------
; BS_HATCHED: HATCH PATTERN BLT
; Regs: DS:SI = lpPBrush DIB_Brush attribute/mask/bits structure ptr
;
PLABEL PB_HatchPatBlt
	or	ss:[lCmd],MONOPATBLT	; set mono pattern transfer cmd bit

IFDEF	ASPEN ;-------------------------
	cmp	gs:[bRop],0F0h		; ROP = F0h (PATCOPY) ?
	jne	PB_HatchAspenDone	; no, skip BitBlt workaround
	cmp	gs:[BWFlag],7		; Block Writes disabled ?
	jne	PB_HatchAspenDone	; no, skip BitBlt workaround
	mov	ax,ss:[xExt]		; AX = BitBlt rect width extent
	cmp	ax,gs:[wBWwidth]	; >= CR53.b5 (16/32) ?
	jl	PB_HatchAspenDone	; no, skip BitBlt workaround
;
; Fill the BitBlt rectangle with the background color
; Regs: DS:SI = lpPBrush DIB_Brush attribute/mask/bits structure ptr
;
	mov	edi,dword ptr ss:[DestyOrg] ; EDI.H=DestXorg, EDI.L=DestYorg
	mov	ecx,ds:[si.dp8BgColor]	; ECX = hatch pattern background color
	mov	ebx,RECT_SOLID		; EBX = rectangle draw cmd

	CheckFIFOSpace FOUR_SLOTS   es	; wait for FIFO slots available

	EngineWrite B_PAT_FG_CLR    es, ecx ; write ECX = hatch patt bgnd color

IFDEF	VIRGE_PATCH16 ;-----------------
	test	gs:[BWFlag],08h 	; Virge/VX running in fast page mode ?
	jz	PB_HatchPatch16Done	; no, skip patch16 as it doesn't apply
	mov	ax,ss:[xExt]		; AX = bitblt rect X extent
	cmp	ax,20			; bitblt rectangle width < 20 ?
	jl	@F			; yes, use rectangle width as is
	mov	ax,20			; no, AX = maximum rectangle width
@@:
	dec	ax			; AX = bitblt rect width (0-based)
	shl	eax,16			; EAX.H = bitblt rect width (0-based)
	mov	ax,1			; EAX.H=rect width 0-based, EAX.L=height

	EngineWrite B_RWIDTH_HEIGHT es, eax ; write EAX = rect width/height
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = DestX, DestX coords
	WriteCMDAutoOFF B_CMD_SET   es, ebx ; write EBX = rectangle draw cmd

	CheckFIFOSpace	THREE_SLOTS es	; wait for FIFO slots available
PLABEL PB_HatchPatch16Done
ENDIF ; VIRGE_PATCH16 ;-----------------

	mov	eax,dword ptr ss:[yExt] ; EAX.H=xExt extent, EAX.L=yExt extent
	sub	eax,00010000h		; EAX.H=xExt 0-based, EAX.L=yExt extent

	EngineWrite B_RWIDTH_HEIGHT es, eax ; write EAX = rect width/height
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = DestX, DestX coords
	WriteCMDAutoOFF B_CMD_SET   es, ebx ; write EBX = rectangle draw cmd

	or	ss:[lCmd],bTransparent	; draw transparent BitBlt hatch pattern
PLABEL PB_HatchAspenDone
ENDIF ; ASPEN ;-------------------------

;
; Transfer the hatched pattern into offscreen memory
; Regs: DS:SI = lpPBrush DIB_Brush attribute/mask/bits structure ptr
;
	mov	dh,ds:[si.dp8BrushMask + 1Ch]	; DH = hatched pattern byte 7
	mov	ch,ds:[si.dp8BrushMask + 0Ch]	; CH = hatched pattern byte 3
	mov	dl,ds:[si.dp8BrushMask + 18h]	; DL = hatched pattern byte 6
	mov	cl,ds:[si.dp8BrushMask + 08h]	; CL = hatched pattern byte 2
	shl	ecx,16				; ECX.H=hatch pattern bytes 3:2
	mov	edi,ds:[si.dp8FgColor]		; EDI = foreground color
	shl	edx,16				; EDX.H=hatch pattern bytes 7:6
	mov	ebx,ds:[si.dp8BgColor]		; EBX = background color
	mov	dh,ds:[si.dp8BrushMask + 14h]	; DH = hatched pattern byte 5
	mov	ch,ds:[si.dp8BrushMask + 04h]	; CH = hatched pattern byte 1
	mov	dl,ds:[si.dp8BrushMask + 10h]	; EDX=hatched pattern bytes 7:4
	mov	cl,ds:[si.dp8BrushMask + 00h]	; ECX=hatched pattern bytes 3:0

	CheckFIFOSpace FOUR_SLOTS   es	; wait for FIFO slots available

	EngineWrite B_MONO_PAT_0, es, ecx ; write ECX = hatch pattern LSD
	EngineWrite B_MONO_PAT_1, es, edx ; write EDX = hatch pattern MSD
	EngineWrite B_PAT_BG_CLR, es, ebx ; write EBX = background color
	EngineWrite B_PAT_FG_CLR, es, edi ; write EDI = foreground color
	clc				; CF=0, ready to do BitBlt operation
	ret				;


;------------------------------------------------------------------------------
; PROCEDURE:	MonoSourceBlt:
;
; DESCRIPTION:	Setup and perform BitBLT involving Mono source pattern.
;
; ASSUMPTIONS:	Mono/Color/Solid pattern/color has alread been loaded.
;
; CALLS:	None
;
; ENTRY  REGS:	SS:BP = BitBlt stack parameters/return address
;		ES: = Engine MMIO selector
;		GS: = Driver data selector (DGROUP)
;		SS:[lCmd] = dDestFmt + Rop
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	EAX, EBX, ECX, EDX, ESI, EDI, DS:
;
; NOTES:	None
;------------------------------------------------------------------------------
;
	ALIGN	4

cProc	MonoSourceBlt,<NEAR,PUBLIC>
cBegin
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data
;
; Get pointer to mono bitmap and save line to line offset, odd source data
;   alignment, scanline transfer size, etc.
;
	xor	esi,esi
	lds	si,ss:[lpSrcDev]	; DS:SI = source device struct ptr
	mov	ax,ss:[SrcxOrg] 	; AX = BitBlt rect source X origin
	mov	cx,ss:[xExt]		; CX = BitBlt rect width extent
	and	ax,07h			; AX = source bit offset within byte
	mov	bx,ss:[DestxOrg]	; BX = BitBlt rect dest X origin
	add	cx,ax			; CX=xExt corrected for start bit offset
	or	ss:[lCmd],CPUAlign_dword ; set image xfer to DWORD-aligned
	add	cx,07h			; round up to nearest BYTE boundary
	and	cx,0FF8h		; mask out odd bits, limit xExt range
	sub	bx,ax			; BX=DestxOrg corrected for bit offset
	dec	cx			; CX = BitBlt rect width (0-based)
	mov	ss:[wAdjWidth],cx	; save CX = BitBlt rect width (0-based)
	inc	cx			; CX = BitBlt rect width (1-based)
	mov	ss:[wAdjDestx],bx	; save destx to set
	mov	dx,ds:[si.bmWidthBytes] ; DX = mono bmp scanline width in bytes
	shr	cx,3			; CX=BitBlt rect width in bytes, 0-based
	mov	ss:[wLineToLineInc],dx	; save DX=mono src scan width in bytes
	mov	ax,ss:[SrcyOrg] 	; AX = source bitmap rect Y origin
	mul	dx			; DX:AX=linear offset of source bitmap
;
; Calculate/save the mono source bitmap starting address
;
	mov	dx,ds:[si.bmBits.sel]	; DX = Source bitmap image base selector
	mov	si,ds:[si.bmBits.off]	; SI = Source bitmap image base offset
	mov	ss:[wAdjXExt],cx	; save CX = bytes to transfer per line
	add	si,ax			; DS:SI = Source bmp image Y adj'd ptr
	mov	ax,ss:[SrcxOrg] 	; AX = source bitmap rect X origin
	mov	ss:[wBitmapSeg],dx	; save DX = source mono bitmap selector
	shr	ax,3			; AX = byte X offset into source bitmap
	add	si,ax			; DS:SI = source bitmap xfer start ptr
	mov	ss:[wMonoSrcBitmapAddr],si ; save SI = source mono bitmap offset
;
; Load registers with fgnd/bgnd color, destination, clipping, and width/height
;
	lds	si,ss:[lpDrawMode]	; DS:SI = DRAWMODE structure ptr
	mov	bx,ss:[DestyOrg]	; BX = source rect top clip Y coord
	mov	ax,ss:[yExt]		; AX = source rect Y height extent
	mov	dx,ss:[DestxOrg]	; DX = source rect left clip X coord
	add	ax,bx			; AX = source rect bottom clip Y coord
	shl	ebx,16			; EBX.H = source rect top clip Y coord
	mov	ecx,ds:[si.TextColor]	; ECX = mono background color (reversed)
	mov	bx,ax			; EBX.H=clip top Y, EBX.L=clip bottom Y
	mov	ax,ss:[xExt]		; AX = source rect X width extent
	dec	bx			; EBX.H=top Y, EBX.L=bottom Y last pixel
	add	ax,dx			; AX = source rect right clip X coord
	shl	edx,16			; EDX.H = source rect left clip X coord
	mov	dx,ax			; EDX.H=clip left X, EDX.L=clip right X
	mov	eax,ds:[si.bkColor]	; EAX = mono foreground color (reversed)
	dec	dx			; EDX.H=left X, EDX.L=right X last pixel

	mov	si,ss:[wAdjWidth]	; SI = bitmap rect adj'd width 0-based
	mov	di,ss:[wAdjDestx]	; DI = bitmap rect adjusted dest X coord
	shl	esi,16			; ESI.H = bitmap rect adjusted width
	shl	edi,16			; EDI.H = rect adjusted dest X coord
	mov	si,ss:[yExt]		; ESI.H=rect width, ESI.L=rect height
	mov	di,ss:[DestyOrg]	; EDI.H=rect X coord, EDI.L=rect Y coord

IFDEF	ASPEN ;-------------------------
;
; Check for ASPEN SRCCOPY BitBlt workaround necessity
; Regs: EAX = Bitmap rectangle foreground color (from lpDrawMode.bkColor)
;	EBX = Bitmap rectangle top/bottom clipping coords
;	ECX = Bitmap rectangle background color (from lpDrawMode.TextColor)
;	EDX = Bitmap rectangle left/right clipping coords
;	ESI = Bitmap rectangle width/height
;	EDI = Bitmap rectangle destination X,Y coords
;
	cmp	gs:[bRop],0CCh		; ROP = CCh (SRCCOPY) ?
	jne	MSB_AspenDone		; no, skip BitBlt workaround
	cmp	gs:[BWFlag],7		; Block Writes disabled ?
	jne	MSB_AspenDone		; no, skip BitBlt workaround
	push	ax			; save AX = foreground color LSW
	mov	ax,ss:[wAdjWidth]	; AX = BitBlt rect width extent
	cmp	ax,gs:[wBWwidth]	; >= CR53.b5 (16/32) ?
	pop	ax			; restore AX = foreground color LSW
	jl	MSB_AspenDone		; no, skip BitBlt workaround

	CheckFIFOSpace SIX_SLOTS    es	; wait for FIFO slots available

	EngineWrite B_SRC_FG_CLR    es, ecx ; write ECX = rect foreground color

IFDEF	VIRGE_PATCH16 ;-----------------
	test	gs:[BWFlag],08h 	; Virge/VX running in fast page mode ?
	jz	MSB_Patch16Done 	; no, skip Patch16 as it doesn't apply
	push	eax			; save EAX = foreground color
	mov	ax,ss:[wAdjWidth]	; AX = BitBlt rect width extent
	cmp	ax,(20 - 1)		; bitblt rect width > 20 (0-based) ?
	jl	@F			; no, use rectangle width as is
	mov	ax,(20 - 1)		; yes, AX = maximum rectangle width
@@:
	shl	eax,16			; EAX.H = bitblt rectangle width
	mov	ax,1			; EAX.H=rect width 0-based, EAX.L=height

	EngineWrite B_RWIDTH_HEIGHT es, eax ; write EAX=bitblt rect width/height
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = rect dest X,Y coords
	WriteCMDAutoOFF B_CMD_SET   es, RECT_SOLID+bClip_Enable ; draw rect
	pop	eax			; restore EAX = foreground color

	CheckFIFOSpace	FIVE_SLOTS  es	; wait for FIFO slots available
PLABEL MSB_Patch16Done
ENDIF ; VIRGE_PATCH16 ;-----------------

;
; Fill the destination rectangle with the background color and setup for
;   transparent BitBlt operation below
; Regs: EAX = Bitmap rectangle foreground color (from lpDrawMode.bkColor)
;	EBX = Bitmap rectangle top/bottom clipping coords
;	ECX = Bitmap rectangle background color (from lpDrawMode.TextColor)
;	EDX = Bitmap rectangle left/right clipping coords
;	ESI = Bitmap rectangle width/height
;	EDI = Bitmap rectangle destination X,Y coords
;
	EngineWrite B_CLIP_L_R	    es, edx ; write EDX = rect left/right clip
	EngineWrite B_CLIP_T_B	    es, ebx ; write EBX = rect top/bottom clip
	EngineWrite B_RWIDTH_HEIGHT es, esi ; write ESI = rect width/height
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = rect dest X,Y coords
	WriteCMDAutoOFF B_CMD_SET   es, RECT_SOLID+bClip_Enable ; draw rect

	or	ss:[lCmd],(bTransparent + X_Positive) ; turn on transparent bit

	CheckFIFOSpace	FOUR_SLOTS  es	; wait for FIFO slots available

	jmp	MSB_BypassSetClipRect	; skip clip rect set as done already
PLABEL MSB_AspenDone
ENDIF ; ASPEN ;-------------------------

;
; Perform the mono source BitBlt operation
; NOTE: The monochrome foreground/background bit designations are reversed
;	from the typical designation (typical: 0 = background, 1 = foreground).
;	The foreground/background color settings are reversed to convert bits
;	back to the expected (typical) designations.
; Regs: EAX = Bitmap rectangle foreground color (from lpDrawMode.bkColor)
;	EBX = Bitmap rectangle top/bottom clipping coords
;	ECX = Bitmap rectangle background color (from lpDrawMode.TextColor)
;	EDX = Bitmap rectangle left/right clipping coords
;	ESI = Bitmap rectangle width/height
;	EDI = Bitmap rectangle destination X,Y coords
;
	CheckFIFOSpace SIX_SLOTS    es	; wait for FIFO slots available

	EngineWrite B_CLIP_L_R	    es, edx ; write EDX = rect left/right clip
	EngineWrite B_CLIP_T_B	    es, ebx ; write EBX = rect top/bottom clip

IFDEF	ASPEN ;-------------------------
PLABEL MSB_BypassSetClipRect
ENDIF ; ASPEN ;-------------------------

	EngineWrite B_SRC_BG_CLR    es, ecx ; write ECX = rect background color
	EngineWrite B_SRC_FG_CLR    es, eax ; write EAX = rect foreground color
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = rect dest X,Y coords

IFDEF	VIRGE_PATCH12 ;-----------------
;###patch12
;
; Apply BitBlt rectangle width patch12 workaround (if necessary)
; Regs: ESI = Bitmap rectangle width/height
;
	xor	bx,bx			; clear BX
	mov	ax,ss:[wAdjWidth]	; AX = bitmap rect adj'd width 0-based
	mov	gs:[delta_x],bx 	; set delta X = 0
;	cmp	gs:[DeviceId_High],VIRGE_ID_HIGH
;	je	MSB_NoPatch12m		; no, skip patch12 as it doesn't apply
;	test	gs:[BWFlag],01h 	; Virge/VX hardware present ?
;	jnz	MSB_NoPatch12m		; no, skip patch12 as it doesn't apply
	cmp	ax,gs:[patch12_x1]	; width >= 57, 29, or 17 ?
	jb	MSB_NoPatch12m		; no, skip patch12
	cmp	ax,gs:[patch12_x2]	; width >= 65, 33, or 23 ?
	jae	MSB_NoPatch12m		; yes, skip patch12
	cmp	gs:[patch12_x2],23	; 24 bpp ?
	je	MSB_NoPatch12m		; yes, skip patch12
	ror	esi,16			; ESI.L = rect width, 0-based
	mov	gs:[delta_x],1		; set delta X = 1
	mov	si,gs:[patch12_x2]	; SI = rect width as 65, 33, or 23
	rol	esi,16			; ESI.H=rect width, ESI.L=rect height
PLABEL MSB_NoPatch12m
ENDIF ; VIRGE_PATCH12 ;-----------------

IFDEF	VIRGE_PATCH15 ;-----------------
;### patch15
	or	esi,(31 SHL 16) 	; round up width to odd 31 byte bound
ENDIF ; VIRGE_PATCH15 ;-----------------

	EngineWrite B_RWIDTH_HEIGHT es, esi ; write ESI = rect width/height

	mov	dx,ss:[wAdjXExt]	; DX = number of bytes to xfer per line
	mov	ax,ss:[wLineToLineInc]	; AX = line to line src bitmap offset
	add	dx,3			; round up to nearest DWORD boundary
	mov	bx,ss:[yExt]		; BX = scanline loop counter
	shr	dx,2			; DX = number of DWORDs to xfer per line
	mov	edi,ss:[lCmd]		; EDI = BitBlt cmd DWORD
	mov	cx,dx			; CX = DWORD xfer count per scanline
	mov	ds,ss:[wBitmapSeg]	; DS: = source bitmap selector
	shl	cx,2			; CX = BYTE xfer count per scanline
	mov	si,ss:[wMonoSrcBitmapAddr] ; DS:SI = source bitmap ptr
	sub	ax,cx			; AX = scanline end/next begin offset

	DisablePD  
	WaitEngineIdle	es		; wait for idle, all FIFO slots free
	EnablePD  

	mov	gs:[InSrcBlt],0FFh	; flag image transfer in progress
	WriteCMDAutoOFF B_CMD_SET   es, edi ; write EDI = BitBlt xfer start cmd
;
; Transfer the mono source data to the engine image transfer registers
; Regs: DS:SI = Source bitmap image ptr
;	   AX = Source bitmap scanline end to next scanline begin offset
;	   BX = Source bitmap height ([yExt])
;	   DX = Number of DWORDs to transfer per line
;
.ERRE	(IMAGE_XFER EQ 0000h)		; ERROR: Code assumes image xfer at 0h

PLABEL MSB_MonoDataXfer
	xor	di,di			; ES:DI = IMAGE_XFER base address
	mov	cx,dx			; CX = DWORD count to transfer per line
	rep	movsd			; transfer mono bitmap image to engine

IFDEF	VIRGE_PATCH12 ;-----------------
;###patch12
	mov	cx,gs:[delta_x] 	; CX = workaround DWORD fill cnt (0/1)
	rep	stosd			; do dummy image xfer data fill
ENDIF ; VIRGE_PATCH12 ;-----------------

	add	si,ax			; DS:SI = next bitmap scanline ptr
	dec	bx			; decrement source bitmap height
	jnz	MSB_MonoDataXfer	; continue mono bitmap image xfer

PLABEL	MonoCleanup
	mov	gs:[InSrcBlt],00h	; flag image transfer completed

cEnd ;	MonoSourceBlt


;------------------------------------------------------------------------------
; PROCEDURE:	ColorSourceBlt:
;
; DESCRIPTION:	Setup and perform BitBlt involving Color source pattern.
;
; ASSUMPTIONS:	SS:BP = BitBlt stack parameters/return address
;		ES:   = Engine MMIO selector
;		GS:   = Driver data selector (DGROUP)
;
; CALLS:	None
;
; ENTRY  REGS:	None
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	EAX, EBX, ECX, EDX, ESI, EDI
;
; NOTES:	None
;------------------------------------------------------------------------------
;
	ALIGN	4

cProc	ColorSourceBlt,<NEAR,PUBLIC>
cBegin
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	lds	si,ss:[lpSrcDev]	; DS:SI = source device struct ptr
IFDEF	VIRGE_PATCH12 ;-----------------
;###patch12
;
; Apply BitBlt rectangle width patch12 workaround (if necessary)
;
	xor	bx,bx			; clear BX
	mov	ax,ss:[xExt]		; AX = original rect X width extent
	mov	gs:[delta_x],bx 	; set delta X = 0
	mov	gs:[org_xExt],ax	; save original rect X width extent

IFDEF	VIRGE_PATCH15 ;-----------------
;###patch15
;	cmp	gs:[DeviceId_High],VIRGE_ID_HIGH
;	je	PCSB_nopatch12c 	; yes, skip patch12 as it doesn't apply
;	test	gs:[BWFlag],01h 	; Virge/VX hardware present ?
;	jnz	PCSB_nopatch12c 	; no, skip patch12 as it doesn't apply
	mov	cx,gs:[patch15_x1]	; CX = workaround rect width
	cmp	ax,cx			; rect width requires workaround ?
	jb	PCSB_init_clip		; yes, go apply workaround rect width
ENDIF ; VIRGE_PATCH15 ;-----------------

	cmp	ax,gs:[patch12_x1]	; rect width >= 57, 29, or 16 ?
	jb	PCSB_nopatch12c 	; no, skip patch12
	mov	cx,gs:[patch12_x2]	; CX = workaround rect width
	cmp	ax,cx			; rect width < 65, 33, or 23 ?
	jae	PCSB_nopatch12c 	; no, skip patch12

IFDEF	VIRGE_PATCH15 ;-----------------
;###patch15
PLABEL PCSB_init_clip
ENDIF ; VIRGE_PATCH15 ;-----------------
;
; Setup the clipping rectangle for the color source BitBlt
; Regs: AX = Original	xExt rectangle width
;	CX = Workaround xExt rectangle width
;
	mov	ss:[xExt],cx		; save xExt = rect width 65, 33, or 23
	sub	cx,ax			; CX = (65,33,23) - original xExt
	mov	bx,ss:[DestxOrg]	; BX = clip rect X left coord
	mov	gs:[delta_x],cx 	; save delta X loop count
	add	ax,bx			; AX = clip rect X right coord, 1-based
	shl	ebx,16			; EBX.H = clip rect X left coord
	mov	dx,ss:[DestyOrg]	; DX = clip rect top Y coord
	mov	bx,ax			; EBX.H=clip rect.left, EBX.L=clip.right
	mov	ax,dx			; AX = clip rect top Y coord
	shl	edx,16			; EDX.H = clip rect top Y coord
	add	ax,ss:[yExt]		; AX = clip rect bottom Y coord
	dec	bx			; EBX.H=clip.left, EBX.L=right last pixl
	mov	dx,ax			; EDX.H=clip rect.top, EDX.L=clip.bottom
	dec	dx			; EDX.H=clip.top, EDX.L=bottom last pixl

	CheckFIFOSpace	TWO_SLOTS, es	; wait for FIFO slots available

	EngineWrite B_CLIP_L_R, es, ebx ; write EBX=clip rect left/right coords
	EngineWrite B_CLIP_T_B, es, edx ; write EDX=clip rect top/bottom coords
	or	ss:[lCmd],bClip_Enable	; enable HW clipping in cmd reg

	mov	cl,ds:[si.deBitsPixel]	; CL = Bpp value (8,16,or 32)
	mov	dx,gs:[delta_x] 	; DX = delta X loop count
	mov	ch,cl			; CH = Bpp value (8,16,or 32)
	mov	ax,dx			; AX = delta X loop count
	shr	cl,4			; CL = Words/pixel count (0,1,2)
	shl	dx,cl			; DX = srcx in bytes
	cmp	ch,24			; 24Bpp mode ?
	jne	short @F		; no, continue
	add	dx,ax			; yes, DX = 24Bpp delta X xfer loop cnt

IFDEF	VIRGE_PATCH15 ;-----------------
;###patch15
	cmp	gs:[org_xExt],3 	; original X extent is 3 ?
	jne	short @F		; no, continue
	sub	dx,4			; because one dummy pixel already xfered
ENDIF ; VIRGE_PATCH15 ;-----------------
@@:
	add	dx,3			; round up to nearest DWORD boundary
	shr	dx,2			; DX = dummy DWORD count to transfer
	mov	gs:[delta_x],dx 	; save dummy DWORD count to transfer
PLABEL PCSB_nopatch12c
ENDIF ; VIRGE_PATCH12 ;-----------------

	xor	eax,eax 		; clear EAX
	xor	ebx,ebx 		; clear EBX
	mov	cl,ds:[si.deBitsPixel]	; CL = bits per pixel (8/16/24/32)
	mov	bx,ss:[SrcxOrg] 	; EBX = source X origin coord

IFDEF	VIRGE_PATCH12 ;-----------------
;###patch12
	mov	ax,gs:[org_xExt]	; AX = original xExt rect width extent
ELSE ; !VIRGE_PATCH12 ;-----------------
	mov	ax,ss:[xExt]		; AX = xExt rect width extent
ENDIF ; VIRGE_PATCH12 ;-----------------

	mov	edx,ebx 		; EDX = [SrcxOrg] source X origin coord
	mov	edi,eax 		; EDI = [xExt] source rect width extent
	cmp	cl,24			; 24Bpp pixel (odd byte) color depth ?
	je	@F			; yes, use EDI/EDX = odd 8 bit adjust
	xor	edi,edi 		; EDI = 0, 8/16/32Bpp, no adjust needed
	xor	edx,edx 		; EDX = 0, 8/16/32Bpp, no adjust needed
@@:
	shr	cl,4			; CL = Words/pixel count (0,1,2)
	shl	eax,cl			; EAX = xExt rect width in bytes
	shl	ebx,cl			; EBX = SrcxOrg offset in bytes
	add	eax,edi 		; EAX = xExt	offset in bytes
	add	ebx,edx 		; EBX = SrcxOrg offset in bytes
	mov	ss:[dAdjXExt],eax	; save adjusted xExt offset into bitmap

;
; Setup for either line by line or full bitmap transfer
; Regs: DS:SI = lpSrcDev source device ptr
;	  EAX = xExt	adjusted offset in bytes
;	  EBX = SrcxOrg adjusted offset in bytes
;
	xor	edi,edi 		; clear EDI (no image adjust needed)
	mov	edx,CPUAlign_dword	; EDX = DWORD-aligned image xfer flag
	mov	ss:[wTransferAll],di	; FALSE = presume transfer line by line

IFDEF	VIRGE_PATCH12 ;-----------------
;###patch12
;
; NOTE: Transfer scanline by scanline as full scan requires alignment setup
;
ELSE ; !VIRGE_PATCH12 ;-----------------
	cmp	eax,ds:[si.deDeltaScan] ; xExt same as bitmap width ?
	jne	PCSB_SaveEngineDrawParms ; no, do transfer line by line
;
; Setup for a full bitmap image transfer
; Regs: DS:SI = lpSrcDev source device ptr
;	  EAX = xExt	adjusted offset in bytes
;	  EBX = SrcxOrg adjusted offset in bytes
;	  EDX = B_CMD_SET image transfer cmd OR mask
;	  EDI = 00000000h
;
PLABEL PCSBXferAll
	mov	di,bx			; EBX = SrcxOrg adj'd offset in bytes
	mov	ss:[wTransferAll],1	; TRUE = transfer the whole bitmap
	xor	edx,edx 		; EDX = B_CMD_SET flags OR mask (none)
	and	di,3			; DI = byte offset within DWORD
	jz	PCSBValid		; no odd BYTEs, no adjustment needed
	mov	dx,bFirst_1byte 	; EDX = presume 3 odd BYTEs in xfer
	cmp	di,2			; 2 odd BYTEs in image xfer ?
	jb	PCSBValid		; no, 1 odd BYTE, EDX = CMD_SET flags
	mov	dx,bFirst_2bytes	; EDX = presume 2 odd BYTEs in xfer
	je	PCSBValid		; yes, 2 odd BYTEs, EDX = CMD_SET flags
	mov	dx,bFirst_3bytes	; EDX = presume 1 odd BYTE in xfer
PCSBValid:
	and	bl,0FCh 		; EDX = DWORD bound adjusted srcx_bytes
	mov	ecx,CPUAlign_word	; ECX = presume xExt WORD aligned image
	test	ax,2			; bitmap image width on WORD boundary ?
	jnz	@F			; yes, continue
	mov	ecx,CPUAlign_dword	; ECX = xExt DWORD aligned image extent
@@:
	or	edx,ecx 		; EDX=B_CMD_SET image xfer cmd OR mask
ENDIF ; VIRGE_PATCH12 ;-----------------

;
; Adjust the bitmap source offset/parameters for later rendering
; Regs: DS:SI = lpSrcDev source device ptr
;	  EBX = SrcxOrg adjusted offset in bytes (DWORD aligned)
;	  EDX = B_CMD_SET image transfer cmd OR mask
;	  EDI = SrcxOrg odd byte offset
;
PLABEL PCSB_SaveEngineDrawParms
	xor	eax,eax 		; clear EAX.H
	mov	ecx,ds:[si.deDeltaScan] ; ECX = delta scan
	mov	ax,ss:[SrcyOrg] 	; EAX = Source bitmap Y origin
	or	ss:[lCmd],edx		; save B_CMD_SET image xfer cmd flags
	mul	ecx			; EDX:EAX = y byte offset
	mov	edx,dword ptr ds:[si.deBits] ; EDX = source bitmap offset
	mov	ss:[dLineToLineInc],ecx ; save ECX = delta scan
	add	edx,eax 		; add adjusted xExt extent in bytes
	mov	cx,word ptr ds:[si.deBits + 4] ; CX = source bitmap selector
	add	edx,ebx 		; EDX = 1st source bitmap byte to xfer
	mov	ss:[wBitmapSeg],cx	; save source bitmap selector
	sub	edx,edi 		; EDX=final adjusted source bmp offset
	mov	ss:[wAdjust],di 	; save odd byte adjustment
	mov	ss:[dSrcBitmapAddr],edx ; save final adjusted source bmp offset

;
; Setup registers for the color source bitmap image transfer
;
	mov	esi,dword ptr ss:[yExt]     ; ESI.H=src width, ESI.L=src height
	mov	edi,dword ptr ss:[DestyOrg] ; EDI.H=dest X coord, EDI.L=dest Y
	sub	esi,00010000h		; ESI.H=src width 0-based, ESI.L=height
	mov	ecx,ss:[lCmd]		; ECX = bitmap image xfer cmd
	mov	ds,ss:[wBitmapSeg]	; DS: = source bitmap image selector
	xor	eax,eax 		; clear EAX.H
	mov	ebx,ss:[dAdjXExt]	; EBX = number of bytes to transfer
	mov	ax,si			; EAX = src image height scanline count
	mov	edx,ss:[dLineToLineInc] ; EDX = line to line xfer offset

	DisablePD  
	WaitEngineIdle	es		; wait for idle, all FIFO slots free
	EnablePD

	mov	gs:[InSrcBlt],0FFh	; flag image transfer in progress
	EngineWrite B_RWIDTH_HEIGHT es, esi ; write ESI = bitmap width/height
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = bitmap dest X,Y coords
	WriteCMDAutoOFF B_CMD_SET   es, ecx ; write ECX = bitmap xfer cmd

	mov	esi,ss:[dSrcBitmapAddr] ; DS:ESI = source bitmap image ptr
	test	ss:[wTransferAll],1	; transfer whole bitmap at once ?
	jnz	CSB_ColorAll		; yes, go do it
	add	bx,3			; round up to DWORD boundary
	shr	bx,2			; EBX = number of DWORDs to transfer
	mov	ecx,ebx 		; EBX = DWORD xfer count per scanline
	shl	ecx,2			; ECX = BYTE xfer count per scanline
	sub	edx,ecx 		; EDX = scanline end/next begin offset
;
; Transfer color source data to the image xfer registers scanline by scanline
; Regs: DS:ESI = Source bitmap image ptr
;	   EAX = Source bitmap height
;	   EBX = Source bitmap width in DWORDs (scanline width)
;	   ECX = 0000xxxxh (high word clear)
;	   EDX = Source bitmap scanline to next scanline begin offset
;
.ERRE	(IMAGE_XFER EQ 0000h)		; ERROR: Code assumes IMAGE_XFER = 0h

	xor	edi,edi 		; ES:EDI = engine IMAGE_XFER area ptr
	dec	ax			; AX = (scanline count - last scanline)
	jz	CSB_LastScanline	; on last scanline, handle special case

PLABEL CSB_ColorDataXfer
	xor	di,di			; ES:EDI = engine IMAGE_XFER area ptr
	mov	cx,bx			; ECX = DWORDs to xfer per scanline
	rep	movs dword ptr es:[edi],ds:[esi] ; xfer color bitmap to engine

IFDEF	VIRGE_PATCH12 ;-----------------
;###patch12
	mov	cx,gs:[delta_x] 	; CX = workaround DWORD fill count
	rep	stosd			; do dummy image xfer data fill
ENDIF ; VIRGE_PATCH12 ;-----------------

	add	esi,edx 		; DS:ESI = source bitmap next scanline
	dec	ax			; decrement bitmap image scanline count
	jnz	CSB_ColorDataXfer	; continue xfering color bitmap data

;
; Pay special attention to last few byte(s) of last scan to avoid GP Fault
; Regs: DS:ESI = Source bitmap image ptr
;	   ECX = 0000xxxxh (high word clear)
;	   EDI = 0000xxxxh (high word clear)
;
PLABEL CSB_LastScanline
	mov	cx,bx			; ECX = DWORDs to xfer per scanline
	xor	di,di			; ES:EDI = engine IMAGE_XFER area ptr
	dec	cx			; ECX = (DWORDs to xfer - last DWORD)
	rep	movs dword ptr es:[edi],ds:[esi] ; xfer color bitmap to engine
	mov	bl,byte ptr ss:[dAdjXExt] ; BL = scanline DWORD xfer odd bits

;
; Read the relevant bytes in the last DWORD (using a negative offset if needed)
; NOTE: This code assumes that the CPU odd data alignment penalty is smaller
;	than a mispredicted branch penalty (much branching done in older code).
; Regs: DS:ESI = Source bitmap image ptr
;	BL.1:0 = Odd source bitmap scanline bytes to xfer
;		    01h = Transfer last 1 BYTE
;		    02h = Transfer last 2 BYTEs
;		    03h = Transfer last 3 BYTEs
;		    00h = Transfer last 4 BYTEs (1 DWORD)
;
PLABEL CSB_AllLastDword
	neg	bl			; BL.1:0 = last BYTE offset in DWORD
	xor	di,di			; ES:DI = engine IMAGE_XFER area ptr
	and	ebx,3			; EBX = last BYTE offset in DWORD
	mov	cl,bl			; CL = last BYTE offset in DWORD
	sub	esi,ebx 		; ESI = last DWORD address to read
	shl	cl,3			; CL = shift so AL=Byte0, AH=Byte1, etc
	mov	eax,ds:[esi]		; EAX = odd bytes (before truncation)
	shr	eax,cl			; EAX = odd bitmap bytes to transfer
	mov	es:[di],eax		; write odd bitmap bytes to dest device

IFDEF	VIRGE_PATCH12 ;-----------------
;###patch12
	mov	cx,gs:[delta_x] 	; CX = workaround DWORD fill count
	rep	stosd			; do dummy image xfer data fill
ENDIF ; VIRGE_PATCH12 ;-----------------

	mov	gs:[InSrcBlt],00h	; flag image transfer completed

cEnd ;	ColorSourceBlt

;------------------------------------------------------------------------------
; Source bitmap image is the same width as the delta scan value
;   Transfer the complete source bitmap as a block (up to image xfer size)
; Regs: DS:ESI = Source bitmap image ptr
;	   EAX = Source bitmap height
;	   EBX = Source bitmap width in DWORDs (scanline width)
;
.ERRE	(IMAGE_XFER EQ 0000h)		; ERROR: Code assumes IMAGE_XFER = 0h

PLABEL CSB_ColorAll
	mul	ebx			; EDX = 0h, EAX = bitmap area in BYTEs
	mov	dx,ss:[wAdjust] 	; EDX = odd byte adjust, assumes EDX.H=0
	add	edx,eax 		; EDX = total bitmap image bytes to xfer
	mov	ebx,edx 		; EBX = total bitmap image bytes to xfer
	add	edx,3			; round up to nearest DWORD boundary
	shr	edx,2			; EDX = bitmap image xfer size in DWORDS
	dec	edx			; EDX = image xfer size less last DWORD
	jz	CSB_AllLastDword	; on last DWORD, handle as special case

PLABEL CSB_VeryLargeXferData
	mov	ecx,(IMAGE_XFER_MAXBYTES / 4) ; ECX=max image xfer DWORDs size
	xor	edi,edi 		; ES:EDI = engine IMAGE_XFER area ptr
	cmp	edx,ecx 		; remaining xfer > image xfer size ?
	ja	@F			; yes, use ECX = max image xfer size
	mov	ecx,edx 		; no, ECX=remaining image size to xfer
@@:
	sub	edx,ecx 		; EDX=remain image xfer size, CF/ZF flag
	rep	movs dword ptr es:[edi],ds:[esi] ; xfer bitmap image data
	jbe	CSB_AllLastDword	; image xfer completed, do last DWORD
	jmp	CSB_VeryLargeXferData	; more data, continue bitmap image xfer



;------------------------------------------------------------------------------
; PROCEDURE:	ScreenToScreenBlt:
;
; DESCRIPTION:	Performs screen to screen BitBlt operation.
;
; ASSUMPTIONS:	SS:BP = BitBlt stack parameters/return address
;		ES:   = Engine MMIO selector
;		GS:   = Driver data selector (DGROUP)
;
; CALLS:	None
;
; ENTRY  REGS:	None
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	EAX, EBX, ECX, EDX, ESI, EDI
;
; NOTES:	None
;------------------------------------------------------------------------------
;
	ALIGN	4

cProc	ScreenToScreenBlt,<NEAR,PUBLIC>
cBegin
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

IFDEF	VIRGE_PATCH12 ;-----------------
;###patch12
;	cmp	gs:[DeviceId_High],VIRGE_ID_HIGH
;	je	no_patch12		; yes, skip patch12 as it doesn't apply
;	test	gs:[BWFlag],01h 	; ViRGE/VX hardware present ?
;	jnz	no_patch12		; yes, skip patch12 as it doesn't apply
	mov	ax,ss:[xExt]		; AX = bitmap rect width X extent
	mov	cx,gs:[patch12_x2]	; CX = errata width range high end
	cmp	ax,gs:[patch12_x1]	; bitmap width >= 57, 29, or 16 ?
	jb	no_patch12		; no, skip patch12
	cmp	ax,cx			; bitmap width < 65, 33, or 23 ?
	jae	no_patch12		; no, skip patch12
	mov	ss:[xExt],cx		; yes, let xExt width = 65, 33, or 23

	mov	dx,ss:[DestxOrg]	; DX = bitmap image left X coord
	dec	ax			; AX = xExt bitmap rect width, 0-based
	mov	cx,dx			; CX = bitmap image left X coord
	mov	bx,ss:[DestyOrg]	; BX = bitmap dest Y coord
	shl	edx,16			; EDX.H = clip rect.left coord
	mov	dx,cx			; EDX.H=clip.left coord, EDX.L=clip.left
	mov	cx,bx			; CX = bitmap dest Y coord
	shl	ebx,16			; EBX.H = bitmap dest Y coord
	add	dx,ax			; EDX.H=clip.left, EDX.L=clip.right
	mov	bx,ss:[yExt]		; BX = bitmap rect height Y extent
	add	bx,cx			; EBX.H=clip.top, EBX.L=clip.bottom
	dec	bx			; EBX.H=clip.top, EBX.L=bottom last pixl

	CheckFIFOSpace	TWO_SLOTS, es	; wait for FIFO slots available

	EngineWrite B_CLIP_L_R, es, edx ; write EDX = clip rect left/right
	EngineWrite B_CLIP_T_B, es, ebx ; write EBX = clip rect top/bottom
	or	ss:[lCmd],bClip_Enable	; enable HW clipping
no_patch12:
ENDIF ; VIRGE_PATCH12 ;-----------------

;
; If the source bitmap Y origin is negative with respect to the destination
;   bitmap Y origin, use the bottom edge coordinates and a negative direction.
;
	mov	esi,dword ptr ss:[SrcyOrg]  ; ESI.H=SrcXorg coord, ESI.L=SrcYorg
	mov	edi,dword ptr ss:[DestyOrg] ; EDI.H=DestXorg,	  EDI.L=DestYorg
	mov	eax,ss:[lCmd]		; EAX = engine scrn to scrn BitBlt cmd
	mov	ecx,dword ptr ss:[yExt] ; ECX.H=xExt width, ECX.L=yExt height
	cmp	di,si			; dest Y1 origin <= source Y1 origin ?
	jle	@F			; yes, use parameters as is
	add	si,cx			; SrcyOrg += yExt
	add	di,cx			; DestyOrg += yExt
	dec	si			; ESI.H=SrcXorg coord, ESI.L=SrcYorg
	dec	di			; EDI.H=DestXorg,      EDI.L=DestYorg
	and	eax,(NOT Y_Positive)	; set image xfer negative Y direction
@@:
;
; If the source bitmap X origin is negative with respect to the destination
;   bitmap X origin, use the right edge coordinates and a negative direction.
; Regs: EAX = B_CMD_SET BitBlt operation cmd DWORD
;	ECX = Bitmap rectangle width/height
;	ESI = Source bitmap rectangle X,Y coordinates
;	EDI = Dest   bitmap rectangle X,Y coordinates
;
	cmp	edi,esi 		; dest X1 origin <= source X1 origin ?
	jle	@F			; yes, use parameters as is
	ror	ecx,16			; ECX.H=yExt height,   ECX.L=xExt width
	ror	esi,16			; ESI.H=SrcYorg coord, ESI.L=SrcXorg
	ror	edi,16			; EDI.H=DestYorg,      EDI.L=DestXorg
	add	si,cx			; SrcxOrg  += xExt
	add	di,cx			; DestxOrg += xExt
	dec	si			; ESI.H=SrcYorg  coord, ESI.L=SrcXorg
	dec	di			; EDI.H=DestYorg coord, EDI.L=DestXorg
	and	eax,(NOT X_Positive)	; set image xfer negative X direction
	ror	ecx,16			; ECX.H=xExt width,    ECX.L=yExt height
	ror	esi,16			; ESI.H=SrcXorg coord, ESI.L=SrcYorg
	ror	edi,16			; EDI.H=DestXorg,      EDI.L=DestYorg
@@:
	sub	ecx,00010000h		; ECX.H=xExt width 0-based, ECX.L=yExt

;
; Perform the screen to screen BitBlt operation
; Regs: EAX = B_CMD_SET BitBlt operation cmd DWORD
;	ECX = Bitmap rectangle width/height
;	ESI = Source bitmap rectangle X,Y coordinates
;	EDI = Dest   bitmap rectangle X,Y coordinates
;
;;	CheckFIFOSpace	FOUR_SLOTS, es	; wait for FIFO space available
	CheckFIFOSpace	EIGHT_SLOTS, es ; wait for FIFO space available
					; this was changed to fix a HP Pavilion
					; sound problem

	EngineWrite B_RWIDTH_HEIGHT es, ecx ; write ECX = rect width/height
	EngineWrite B_RSRC_XY	    es, esi ; write ESI = rect src  X,Y coords
	EngineWrite B_RDEST_XY	    es, edi ; write EDI = rect dest X,Y coords
	WriteCMDAutoOFF B_CMD_SET   es, eax ; write EAX = scrn to scrn blt cmd

cEnd ;	ScreenToScreenBlt


;------------------------------------------------------------------------------
; PROCEDURE:	ClipSrcBitmap:
;
; DESCRIPTION:	Clip the source bitmap image to valid points/dimensions.
;
; ASSUMPTIONS:	None
;
; CALLS:	None
;
; ENTRY  REGS:	DS:SI = lpSrcDev Source bitmap structure ptr (do not change as
;			the ptr is preserved for next routine being called).
;
; EXIT	 REGS:	None
;
; MODIFY REGS:	AX, BX, CX, DX
;
; NOTES:	The code optimization is prioritized in the following order:
;		1)  No source bitmap clipping occurs	   (high priority)
;		2)  Partial source bitmap clipping occurs  (mod. priority)
;		3)  Complete source bitmap clipping occurs (low  priority)
;------------------------------------------------------------------------------
;
.ERRE	((CodeOFFSET CSB_CheckClipTop)	  GT (CodeOFFSET CSB_CheckClipLeft))
.ERRE	((CodeOFFSET CSB_CheckClipRight)  GT (CodeOFFSET CSB_CheckClipTop))
.ERRE	((CodeOFFSET CSB_CheckClipBottom) GT (CodeOFFSET CSB_CheckClipRight))
					; ERR: Clip order=left/top/right/bottom
	ALIGN	4

PPROC	ClipSrcBitmap	near
PLABEL	ClipSrc 			; for S3BLT_A, S3BLT_BT compatibility

	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

	mov	ax,ss:[SrcxOrg] 	; AX = source bitmap X origin coord
	mov	bx,ss:[SrcyOrg] 	; BX = source bitmap Y origin coord
	mov	cx,ss:[xExt]		; CX = source bitmap X width  extent
	mov	dx,ss:[yExt]		; DX = source bitmap Y height extent
	add	cx,ax			; CX = source bitmap X ending coord
	jle	CSB_NullBlt		; fully clipped on left, NULL BitBlt
	add	dx,bx			; DX = source bitmap Y ending coord
	jle	CSB_NullBlt		; fully clipped on top,  NULL BitBlt

PLABEL CSB_CheckClipLeft
	test	ax,ax			; negative source bitmap X origin ?
	jl	CSB_ClipLeft		; yes, go clip the bitmap left edge
PLABEL CSB_CheckClipTop
	test	bx,bx			; negative source bitmap Y origin ?
	jl	CSB_ClipTop		; yes, go clip the bitmap top edge
PLABEL CSB_CheckClipRight
	cmp	cx,ds:[si.deWidth]	; source bitmap right edge > width ?
	jg	CSB_ClipRight		; yes, go clip the bitmap right edge
PLABEL CSB_CheckClipBottom
	cmp	dx,ds:[si.deHeight]	; source bitmap bottom edge > height ?
	jg	CSB_ClipBottom		; yes, go clip the bitmap bottom edge
	ret				; clipping completed

;------------------------------------------------------------------------------
; Source bitmap is clipped on the left edge (negative X coordinate)
; Regs: AX = [SrcxOrg]	      source bitmap X origin coord (negative)
;	BX = [SrcyOrg]	      source bitmap Y origin coord
;	CX = [SrcxOrg + xExt] source bitmap X ending coord
;	DX = [SrcyOrg + yExt] source bitmap Y ending coord
;
PLABEL CSB_ClipLeft
	sub	ss:[DestxOrg],ax	; adjust dest X origin for source clip
	xor	ax,ax			; AX=clipped src bitmap X origin coord
	mov	ss:[xExt],cx		; save visible src bmp X width extent
	mov	ss:[SrcxOrg],ax 	; save clipped source bitmap X origin
	jmp	CSB_CheckClipTop	; check source bitmap top edge clip

;------------------------------------------------------------------------------
; Source bitmap is clipped on the top edge (negative Y coordinate)
; Regs: AX = [SrcxOrg]	      source bitmap X origin coord
;	BX = [SrcyOrg]	      source bitmap Y origin coord (negative)
;	CX = [SrcxOrg + xExt] source bitmap X ending coord
;	DX = [SrcyOrg + yExt] source bitmap Y ending coord
;
PLABEL CSB_ClipTop
	sub	ss:[DestyOrg],bx	; adjust dest Y origin for source clip
	xor	bx,bx			; BX=clipped src bitmap Y origin coord
	mov	ss:[yExt],dx		; save visible src bmp Y height extent
	mov	ss:[SrcyOrg],bx 	; save clipped source bitmap Y origin
	jmp	CSB_CheckClipRight	; check source bitmap right edge clip

;------------------------------------------------------------------------------
; Source bitmap is clipped on the bottom edge
; Regs: AX = [SrcxOrg]	      source bitmap X origin coord
;	BX = [SrcyOrg]	      source bitmap Y origin coord
;	CX = [SrcxOrg + xExt] source bitmap X ending coord
;	DX = [SrcyOrg + yExt] source bitmap Y ending coord
;
PLABEL CSB_ClipBottom
	sub	dx,ds:[si.deHeight]	; DX=clipped bottom edge extent adjust
	sub	ss:[yExt],dx		; adjust visible src bmp Y height extent
	jle	CSB_NullBlt		; fully clipped on bottom, NULL BitBlt
	ret				; optimized: skip jmp back to main exit

;------------------------------------------------------------------------------
; Source bitmap is clipped on the right edge
; Regs: AX = [SrcxOrg]	      source bitmap X origin coord
;	BX = [SrcyOrg]	      source bitmap Y origin coord
;	CX = [SrcxOrg + xExt] source bitmap X ending coord
;	DX = [SrcyOrg + yExt] source bitmap Y ending coord
;
PLABEL CSB_ClipRight
	sub	cx,ds:[si.deWidth]	; CX = clipped right edge extent adjust
	sub	ss:[xExt],cx		; adjust visible src bmp X width extent
	jg	CSB_CheckClipBottom	; source bitmap visible, go chk bottom
PLABEL CSB_NullBlt
	pop	ax			; AX = caller's return addr (discarded)
	ret				; goto BitBlt exit (B_Done)

ClipSrcBitmap	endp


;------------------------------------------------------------------------------
; The BitBlt operation includes the source bitmap
;   Exclude the union of the source/destination rectangles
; Regs: DS:SI = lpSrcDev source device struct ptr (must be preserved/restored)
;	   AX = BitBlt calling operation return address
;
	ALIGN	4

PLABEL B_SWCursorExcludeUnion
	push	ax			; save AX = BitBlt caller return offset
	lds	si,gs:[lpDriverPDevice] ; DS:SI = screen PDEVICE struct far ptr
	cmp	ds:[si.deCursorExclude],0	;NULL ?
	jne	short	@f
	ret
@@:
	mov	ax,ss:[SrcxOrg] 	; AX = source bitmap X origin
	mov	bx,ss:[SrcyOrg] 	; BX = source bitmap Y origin
	mov	cx,ss:[DestxOrg]	; CX = dest   bitmap X origin
	mov	dx,ss:[DestyOrg]	; DX = dest   bitmap Y origin
	cmp	ax,cx			; source X origin < dest X origin ?
	jl	@F			; yes, AX = smaller X, CX = bigger X
	xchg	ax,cx			; no, now AX = smaller X, CX = bigger X
@@:
	cmp	bx,dx			; source Y origin < dest Y origin ?
	jl	@F			; yes, BX = smaller Y, DX = bigger Y
	xchg	bx,dx			; no, now BX = smaller Y, DX = bigger Y
@@:
	jmp	B_SWCursorExcludeCommon ; go exclude source/dest union rect

;------------------------------------------------------------------------------
; The BitBlt operation includes a destination bitmap rect; calculate its size
; Regs: DS:SI = lpSrcDev source device struct ptr (must be preserved/restored)
;	   AX = BitBlt calling operation return address
;
	ALIGN	4

PLABEL B_SWCursorExcludeRect
	push	ax			; save AX = BitBlt caller return offset
	lds	si,gs:[lpDriverPDevice] ; DS:SI = screen PDEVICE struct far ptr
	cmp	ds:[si.deCursorExclude],0	;NULL ?
	jne	short	@f
	ret
@@:
	mov	ax,ss:[DestxOrg]	; AX = dest bitmap X origin
	mov	bx,ss:[DestyOrg]	; BX = dest bitmap Y origin
	mov	cx,ax			; CX = dest bitmap X origin
	mov	dx,bx			; DX = dest bitmap Y origin

;
; Calculate the rectangle right/bottom coordinates for SW cursor exclusion
; NOTE: DS:SI = lpSrcDev on entry (must be restored before return to code path)
; Regs: AX = Source bitmap X origin (smaller X)
;	BX = Source bitmap Y origin (smaller Y)
;	CX = Dest   bitmap X origin (larger  X)
;	DX = Dest   bitmap Y origin (larger  Y)
;
	assumes gs,Data

PLABEL B_SWCursorExcludeCommon

	assumes ds,nothing

	add	cx,ss:[xExt]		; CX = rect.right  X coord
	add	dx,ss:[yExt]		; DX = rect.bottom Y coord
	dec	cx			; CX = rect.right  X, last pixel drawn
	dec	dx			; DX = rect.bottom Y, last pixel drawn
	push	ds			; save DS: = screen PDEVICE.sel selector
	push	si			; save SI  = screen PDEVICE.off offset
	push	ax			; save AX = rect.left
	push	bx			; save BX = rect.top
	push	cx			; save CX = rect.right
	push	dx			; save DX = rect.bottom
	push	CURSOREXCLUDE		; Flags
	call	ds:[si.deCursorExclude] ; begin software cursor exclusion

	assumes ds,nothing
	assumes es,nothing
	assumes gs,Data

	mov	ax,DGROUP		; AX = driver data selector
	mov	gs,ax			; GS: = driver data selector
	lds	si,gs:[lpDriverPDevice] ; DS:SI = screen PDEVICE struct far ptr
	mov	edi,ds:[si.deCursorUnexclude] ; EDI = cursor unexclude far ptr
	mov	es,gs:[wMMIOSelector]	; ES: = engine MMIO selector
	pop	bx			; BX = BitBlt caller return offset
	pop	ax			; AX = B_Done return address
	push	ds			; save DS: = lpDestDev.sel selector
	push	si			; save SI  = lpDestDev.off offset
	push	CURSOREXCLUDE		; Flags
	push	cs			; save CS: = B_Done ret addr selector
	push	ax			; save AX  = B_Done ret addr offset
	push	edi			; save EDI = deCursorUnexclude far ptr
	push	offset B_SWCursorUnExclude ; save RETF far return offset
	lds	si,ss:[lpSrcDev]	; restore DS:SI = src dev struct far ptr
	jmp	bx			; return to SW cursor exclude caller

PLABEL B_SWCursorUnExclude
	retf


;------------------------------------------------------------------------------
; Driver is unwilling or unable to perform BitBlt operation
;   Free the destination surface (if BUSY) and forward call to the DIBEngine
;
	ALIGN	4

PLABEL B_ReleaseAndCallDIBEngine
	lds	si,ss:[lpDestDev]	; DS:SI = dest pdevice struct ptr
	pop	ax			; AX = discarded B_Done return address
	and	ds:[si.deFlags],(NOT BUSY) ; release destination surface

PLABEL B_JmpToDIBEngine
	pop	edi			; restore regs
	pop	esi			;
	lea	sp,[bp-2]		; SS:SP -> saved BP, DS: regs on stack
	pop	ds			;
	pop	bp			;
	jmp	DIB_BitBlt		;


;------------------------------------------------------------------------------
; ROP256 BitBlt type table - Converts ROP256 code into BitBlt jump table index
;------------------------------------------------------------------------------
;
.ERRE	((DstPr OR PatPr OR SrcPr) EQ 06h) ; ERROR: Code assumes max index=06h

public	BltTypeTable
BltTypeTable	label	byte

	db	0					    ; 00h Blackness
	db	DstPr OR PatPr OR SrcPr 		    ; 01h DPSoon
	db	DstPr OR PatPr OR SrcPr 		    ; 02h DPSona
	db	PatPr OR SrcPr				    ; 03h PSon
	db	SrcPr OR DstPr OR PatPr 		    ; 04h SDPona
	db	DstPr OR PatPr				    ; 05h DPon
	db	PatPr OR DstPr OR SrcPr 		    ; 06h PDSxnon
	db	PatPr OR DstPr OR SrcPr 		    ; 07h PDSaon
	db	SrcPr OR DstPr OR PatPr 		    ; 08h SDPnaa
	db	PatPr OR DstPr OR SrcPr 		    ; 09h PDSxon
	db	DstPr OR PatPr				    ; 0Ah DPna
	db	PatPr OR SrcPr OR DstPr 		    ; 0Bh PSDnaon
	db	SrcPr OR PatPr				    ; 0Ch SPna
	db	PatPr OR DstPr OR SrcPr 		    ; 0Dh PDSnaon
	db	PatPr OR DstPr OR SrcPr 		    ; 0Eh PDSonon
	db	PatPr					    ; 0Fh Pn
	db	PatPr OR DstPr OR SrcPr 		    ; 10h PDSona
	db	DstPr OR SrcPr				    ; 11h DSon
	db	SrcPr OR DstPr OR PatPr 		    ; 12h SDPxnon
	db	SrcPr OR DstPr OR PatPr 		    ; 13h SDPaon
	db	DstPr OR PatPr OR SrcPr 		    ; 14h DPSxnon
	db	DstPr OR PatPr OR SrcPr 		    ; 15h DPSaon
	db	PatPr OR SrcPr OR DstPr OR PatPr OR SrcPr   ; 16h PSDPSanaxx
	db	SrcPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; 17h SSPxDSxaxn
	db	SrcPr OR PatPr OR PatPr OR DstPr	    ; 18h SPxPDxa
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 19h SDPSanaxn
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; 1Ah PDSPaox
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 1Bh SDPSxaxn
	db	PatPr OR SrcPr OR DstPr OR PatPr	    ; 1Ch PSDPaox
	db	DstPr OR SrcPr OR PatPr OR DstPr	    ; 1Dh DSPDxaxn
	db	PatPr OR DstPr OR SrcPr 		    ; 1Eh PDSox
	db	PatPr OR DstPr OR SrcPr 		    ; 1Fh PDSoan
	db	DstPr OR PatPr OR SrcPr 		    ; 20h DPSnaa
	db	SrcPr OR DstPr OR PatPr 		    ; 21h SDPxon
	db	DstPr OR SrcPr				    ; 22h DSna
	db	SrcPr OR PatPr OR DstPr 		    ; 23h SPDnaon
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 24h SPxDSxa
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; 25h PDSPanaxn
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 26h SDPSaox
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 27h SDPSxnox
	db	DstPr OR PatPr OR SrcPr 		    ; 28h DPSxa
	db	PatPr OR SrcPr OR DstPr OR PatPr OR SrcPr   ; 29h PSDPSaoxxn
	db	DstPr OR PatPr OR SrcPr 		    ; 2Ah DPSana
	db	SrcPr OR SrcPr OR PatPr OR PatPr OR DstPr   ; 2Bh SSPxPDxaxn
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 2Ch SPDSoax
	db	PatPr OR SrcPr OR DstPr 		    ; 2Dh PSDnox
	db	PatPr OR SrcPr OR DstPr OR PatPr	    ; 2Eh PSDPxox
	db	PatPr OR SrcPr OR DstPr 		    ; 2Fh PSDnoan
	db	PatPr OR SrcPr				    ; 30h PSna
	db	SrcPr OR DstPr OR PatPr 		    ; 31h SDPnaon
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 32h SDPSoox
	db	SrcPr					    ; 33h Sn
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 34h SPDSaox
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 35h SPDSxnox
	db	SrcPr OR DstPr OR PatPr 		    ; 36h SDPox
	db	SrcPr OR DstPr OR PatPr 		    ; 37h SDPoan
	db	PatPr OR SrcPr OR DstPr OR PatPr	    ; 38h PSDPoax
	db	SrcPr OR PatPr OR DstPr 		    ; 39h SPDnox
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 3Ah SPDSxox
	db	SrcPr OR PatPr OR DstPr 		    ; 3Bh SPDnoan
	db	PatPr OR SrcPr				    ; 3Ch PSx
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 3Dh SPDSonox
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 3Eh SPDSnaox
	db	PatPr OR SrcPr				    ; 3Fh PSan
	db	PatPr OR SrcPr OR DstPr 		    ; 40h PSDnaa
	db	DstPr OR PatPr OR SrcPr 		    ; 41h DPSxon
	db	SrcPr OR DstPr OR PatPr OR DstPr	    ; 42h SDxPDxa
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 43h SPDSanaxn
	db	SrcPr OR DstPr				    ; 44h SDna
	db	DstPr OR PatPr OR SrcPr 		    ; 45h DPSnaon
	db	DstPr OR SrcPr OR PatPr OR DstPr	    ; 46h DSPDaox
	db	PatPr OR SrcPr OR DstPr OR PatPr	    ; 47h PSDPxaxn
	db	SrcPr OR DstPr OR PatPr 		    ; 48h SDPxa
	db	PatPr OR DstPr OR SrcPr OR PatPr OR DstPr   ; 49h PDSPDaoxxn
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; 4Ah DPSDoax
	db	PatPr OR DstPr OR SrcPr 		    ; 4Bh PDSnox
	db	SrcPr OR DstPr OR PatPr 		    ; 4Ch SDPana
	db	SrcPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; 4Dh SSPxDSxoxn
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; 4Eh PDSPxox
	db	PatPr OR DstPr OR SrcPr 		    ; 4Fh PDSnoan
	db	PatPr OR DstPr				    ; 50h PDna
	db	DstPr OR SrcPr OR PatPr 		    ; 51h DSPnaon
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; 52h DPSDaox
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 53h SPDSxaxn
	db	DstPr OR PatPr OR SrcPr 		    ; 54h DPSonon
	db	DstPr					    ; 55h Dn
	db	DstPr OR PatPr OR SrcPr 		    ; 56h DPSox
	db	DstPr OR PatPr OR SrcPr 		    ; 57h DPSoan
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; 58h PDSPoax
	db	DstPr OR PatPr OR SrcPr 		    ; 59h DPSnox
	db	DstPr OR PatPr				    ; 5Ah DPx
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; 5Bh DPSDonox
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; 5Ch DPSDxox
	db	DstPr OR PatPr OR SrcPr 		    ; 5Dh DPSnoan
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; 5Eh DPSDnaox
	db	DstPr OR PatPr				    ; 5Fh DPan
	db	PatPr OR DstPr OR SrcPr 		    ; 60h PDSxa
	db	DstPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; 61h DSPDSaoxxn
	db	DstPr OR SrcPr OR PatPr OR DstPr	    ; 62h DSPDoax
	db	SrcPr OR DstPr OR PatPr 		    ; 63h SDPnox
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 64h SDPSoax
	db	DstPr OR SrcPr OR PatPr 		    ; 65h DSPnox
	db	DstPr OR SrcPr				    ; 66h DSx
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 67h SDPSonox
	db	DstPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; 68h DSPDSonoxxn
	db	PatPr OR DstPr OR SrcPr 		    ; 69h PDSxxn
	db	DstPr OR PatPr OR SrcPr 		    ; 6Ah DPSax
	db	PatPr OR SrcPr OR DstPr OR PatPr OR SrcPr   ; 6Bh PSDPSoaxxn
	db	SrcPr OR DstPr OR PatPr 		    ; 6Ch SDPax
	db	PatPr OR DstPr OR SrcPr OR PatPr OR DstPr   ; 6Dh PDSPDoaxxn
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 6Eh SDPSnoax
	db	PatPr OR DstPr OR SrcPr 		    ; 6Fh PDSxnan
	db	PatPr OR DstPr OR SrcPr 		    ; 70h PDSana
	db	SrcPr OR SrcPr OR DstPr OR PatPr OR DstPr   ; 71h SSDxPDxaxn
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 72h SDPSxox
	db	SrcPr OR DstPr OR PatPr 		    ; 73h SDPnoan
	db	DstPr OR SrcPr OR PatPr OR DstPr	    ; 74h DSPDxox
	db	DstPr OR SrcPr OR PatPr 		    ; 75h DSPnoan
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 76h SDPSnaox
	db	DstPr OR SrcPr				    ; 77h DSan
	db	PatPr OR DstPr OR SrcPr 		    ; 78h PDSax
	db	DstPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; 79h DSPDSoaxxn
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; 7Ah DPSDnoax
	db	SrcPr OR DstPr OR PatPr 		    ; 7Bh SDPxnan
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 7Ch SPDSnoax
	db	DstPr OR PatPr OR SrcPr 		    ; 7Dh DPSxnan
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 7Eh SPxDSxo
	db	DstPr OR PatPr OR SrcPr 		    ; 7Fh DPSaan
	db	DstPr OR PatPr OR SrcPr 		    ; 80h DPSaa
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 81h SPxDSxon
	db	DstPr OR PatPr OR SrcPr 		    ; 82h DPSxna
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; 83h SPDSnoaxn
	db	SrcPr OR DstPr OR PatPr 		    ; 84h SDPxna
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; 85h PDSPnoaxn
	db	DstPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; 86h DSPDSoaxx
	db	PatPr OR DstPr OR SrcPr 		    ; 87h PDSaxn
	db	DstPr OR SrcPr				    ; 88h DSa
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 89h SDPSnaoxn
	db	DstPr OR SrcPr OR PatPr 		    ; 8Ah DSPnoa
	db	DstPr OR SrcPr OR PatPr OR DstPr	    ; 8Bh DSPDxoxn
	db	SrcPr OR DstPr OR PatPr 		    ; 8Ch SDPnoa
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 8Dh SDPSxoxn
	db	SrcPr OR SrcPr OR DstPr OR PatPr OR DstPr   ; 8Eh SSDxPDxax
	db	PatPr OR DstPr OR SrcPr 		    ; 8Fh PDSanan
	db	PatPr OR DstPr OR SrcPr 		    ; 90h PDSxna
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 91h SDPSnoaxn
	db	DstPr OR PatPr OR SrcPr OR DstPr OR PatPr   ; 92h DPSDPoaxx
	db	SrcPr OR PatPr OR DstPr 		    ; 93h SPDaxn
	db	PatPr OR SrcPr OR DstPr OR PatPr OR SrcPr   ; 94h PSDPSoaxx
	db	DstPr OR PatPr OR SrcPr 		    ; 95h DPSaxn
	db	DstPr OR PatPr OR SrcPr 		    ; 96h DPSxx
	db	PatPr OR SrcPr OR DstPr OR PatPr OR SrcPr   ; 97h PSDPSonoxx
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 98h SDPSonoxn
	db	DstPr OR SrcPr				    ; 99h DSxn
	db	DstPr OR PatPr OR SrcPr 		    ; 9Ah DPSnax
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; 9Bh SDPSoaxn
	db	SrcPr OR PatPr OR DstPr 		    ; 9Ch SPDnax
	db	DstPr OR SrcPr OR PatPr OR DstPr	    ; 9Dh DSPDoaxn
	db	DstPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; 9Eh DSPDSaoxx
	db	PatPr OR DstPr OR SrcPr 		    ; 9Fh PDSxan
	db	DstPr OR PatPr				    ; A0h DPa
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; A1h PDSPnaoxn
	db	DstPr OR PatPr OR SrcPr 		    ; A2h DPSnoa
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; A3h DPSDxoxn
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; A4h PDSPonoxn
	db	PatPr OR DstPr				    ; A5h PDxn
	db	DstPr OR SrcPr OR PatPr 		    ; A6h DSPnax
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; A7h PDSPoaxn
	db	DstPr OR PatPr OR SrcPr 		    ; A8h DPSoa
	db	DstPr OR PatPr OR SrcPr 		    ; A9h DPSoxn
	db	DstPr					    ; AAh D
	db	DstPr OR PatPr OR SrcPr 		    ; ABh DPSono
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; ACh SPDSxax
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; ADh DPSDaoxn
	db	DstPr OR SrcPr OR PatPr 		    ; AEh DSPnao
	db	DstPr OR PatPr				    ; AFh DPno
	db	PatPr OR DstPr OR SrcPr 		    ; B0h PDSnoa
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; B1h PDSPxoxn
	db	SrcPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; B2h SSPxDSxox
	db	SrcPr OR DstPr OR PatPr 		    ; B3h SDPanan
	db	PatPr OR SrcPr OR DstPr 		    ; B4h PSDnax
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; B5h DPSDoaxn
	db	DstPr OR PatPr OR SrcPr OR DstPr OR PatPr   ; B6h DPSDPaoxx
	db	SrcPr OR DstPr OR PatPr 		    ; B7h SDPxan
	db	PatPr OR SrcPr OR DstPr OR PatPr	    ; B8h PSDPxax
	db	DstPr OR SrcPr OR PatPr OR DstPr	    ; B9h DSPDaoxn
	db	DstPr OR PatPr OR SrcPr 		    ; BAh DPSnao
	db	DstPr OR SrcPr				    ; BBh DSno
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; BCh SPDSanax
	db	SrcPr OR DstPr OR PatPr OR DstPr	    ; BDh SDxPDxan
	db	DstPr OR PatPr OR SrcPr 		    ; BEh DPSxo
	db	DstPr OR PatPr OR SrcPr 		    ; BFh DPSano
	db	PatPr OR SrcPr				    ; C0h PSa
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; C1h SPDSnaoxn
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; C2h SPDSonoxn
	db	PatPr OR SrcPr				    ; C3h PSxn
	db	SrcPr OR PatPr OR DstPr 		    ; C4h SPDnoa
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; C5h SPDSxoxn
	db	SrcPr OR DstPr OR PatPr 		    ; C6h SDPnax
	db	PatPr OR SrcPr OR DstPr OR PatPr	    ; C7h PSDPoaxn
	db	SrcPr OR DstPr OR PatPr 		    ; C8h SDPoa
	db	SrcPr OR PatPr OR DstPr 		    ; C9h SPDoxn
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; CAh DPSDxax
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; CBh SPDSaoxn
	db	SrcPr					    ; CCh S
	db	SrcPr OR DstPr OR PatPr 		    ; CDh SDPono
	db	SrcPr OR DstPr OR PatPr 		    ; CEh SDPnao
	db	SrcPr OR PatPr				    ; CFh SPno
	db	PatPr OR SrcPr OR DstPr 		    ; D0h PSDnoa
	db	PatPr OR SrcPr OR DstPr OR PatPr	    ; D1h PSDPxoxn
	db	PatPr OR DstPr OR SrcPr 		    ; D2h PDSnax
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; D3h SPDSoaxn
	db	SrcPr OR SrcPr OR PatPr OR PatPr OR DstPr   ; D4h SSPxPDxax
	db	DstPr OR PatPr OR SrcPr 		    ; D5h DPSanan
	db	PatPr OR SrcPr OR DstPr OR PatPr OR SrcPr   ; D6h PSDPSaoxx
	db	DstPr OR PatPr OR SrcPr 		    ; D7h DPSxan
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; D8h PDSPxax
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; D9h SDPSaoxn
	db	DstPr OR PatPr OR SrcPr OR DstPr	    ; DAh DPSDanax
	db	SrcPr OR PatPr OR DstPr OR SrcPr	    ; DBh SPxDSxan
	db	SrcPr OR PatPr OR DstPr 		    ; DCh SPDnao
	db	SrcPr OR DstPr				    ; DDh SDno
	db	SrcPr OR DstPr OR PatPr 		    ; DEh SDPxo
	db	SrcPr OR DstPr OR PatPr 		    ; DFh SDPano
	db	PatPr OR DstPr OR SrcPr 		    ; E0h PDSoa
	db	PatPr OR DstPr OR SrcPr 		    ; E1h PDSoxn
	db	DstPr OR SrcPr OR PatPr OR DstPr	    ; E2h DSPDxax
	db	PatPr OR SrcPr OR DstPr OR PatPr	    ; E3h PSDPaoxn
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; E4h SDPSxax
	db	PatPr OR DstPr OR SrcPr OR PatPr	    ; E5h PDSPaoxn
	db	SrcPr OR DstPr OR PatPr OR SrcPr	    ; E6h SDPSanax
	db	SrcPr OR PatPr OR PatPr OR DstPr	    ; E7h SPxPDxan
	db	SrcPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; E8h SSPxDSxax
	db	DstPr OR SrcPr OR PatPr OR DstPr OR SrcPr   ; E9h DSPDSanaxxn
	db	DstPr OR PatPr OR SrcPr 		    ; EAh DPSao
	db	DstPr OR PatPr OR SrcPr 		    ; EBh DPSxno
	db	SrcPr OR DstPr OR PatPr 		    ; ECh SDPao
	db	SrcPr OR DstPr OR PatPr 		    ; EDh SDPxno
	db	DstPr OR SrcPr				    ; EEh DSo
	db	SrcPr OR DstPr OR PatPr 		    ; EFh SDPnoo
	db	PatPr					    ; F0h P
	db	PatPr OR DstPr OR SrcPr 		    ; F1h PDSono
	db	PatPr OR DstPr OR SrcPr 		    ; F2h PDSnao
	db	PatPr OR SrcPr				    ; F3h PSno
	db	PatPr OR SrcPr OR DstPr 		    ; F4h PSDnao
	db	PatPr OR DstPr				    ; F5h PDno
	db	PatPr OR DstPr OR SrcPr 		    ; F6h PDSxo
	db	PatPr OR DstPr OR SrcPr 		    ; F7h PDSano
	db	PatPr OR DstPr OR SrcPr 		    ; F8h PDSao
	db	PatPr OR DstPr OR SrcPr 		    ; F9h PDSxno
	db	DstPr OR PatPr				    ; FAh DPo
	db	DstPr OR PatPr OR SrcPr 		    ; FBh DPSnoo
	db	PatPr OR SrcPr				    ; FCh PSo
	db	PatPr OR SrcPr OR DstPr 		    ; FDh PSDnoo
	db	DstPr OR PatPr OR SrcPr 		    ; FEh DPSoo
	db	0					    ; FFh 1 Whiteness

.ERRE	((CodeOFFSET $ - CodeOFFSET BltTypeTable) EQ 256)
;					; ERROR: ROP256 table length incorrect
;------------------------------------------------------------------------------

sEnd	Code
end
