;----------------------------------------------------------------------------
; ELLIPSE.ASM
; Copyright (c) 1995 S3 Incorporated
;----------------------------------------------------------------------------
;
patch16		equ	1
.xlist
DOS5 = 1
include         cmacros.inc
include         macros.inc
include         dibeng.inc
incOutput       equ     1
incDrawMode     equ     1
incLogical      equ     1
include         gdidefs.inc
include         s3.inc
.list
;
NumEntries      equ     2048    ;Number of entries in the sine & cosine table
IEntries        equ     6       ;Number of 1's in the table
;
EF_OUTLINE      equ     0001h
EF_CLIP         equ     0002h
EF_INTERIOR     equ     0004h
;
;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------
;
;----------------------------------------------------------------------------
; E X T E R N
;----------------------------------------------------------------------------
externFP        BeginAccess
externFP        EndAccess
externNP        O_DIBEngineToScreen

externNP        LookUpTableEllipse
externNP        CircleFill
externNP        EllipseBorder
;
;
;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin          Data
;
externB         InSrcBlt                ;in S3BLT.ASM
externB         bAUTOEnable             ;autoexec flag
externB         bEllipseFlag            ;in INIT.ASM
externB 	bInDDraw
externW         wBpp_Byte
externW         wMMIOSelector
externW         wScreenWidth            ;in VGA.ASM
externW         wScreenHeight           ;in VGA.ASM
externW         wCursorType             ;in VGA.ASM
externW         FullScreenClipRect      ;
externW		wScreenWidthBytes            ;bytes per scanlines
externD         dCmd                    ;CMD_SET current value
externD         dDestFmt                ;bpp
externB         BWFlag		        ;in INIT.ASM
;
externW	centerX       ;x comp. of center of a circle (or ellipse)
externW	centerY       ;y comp. of center of a circle (or ellipse)
;
externW	radiusX       ;circle if radiusX == radiusY
externW	radiusY
;
externW	evenWidth       ;1 if even width (i.e. non-integer center)
externW	evenHeight      ;1 if even height (i.e. non-integer center)

externB	PGScratchBuffer
sEnd            Data
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
externB         S3Rop2TranslateTable    ;in OUTPUT.ASM
;
externB		SinCos
;
;
;------------------------------------------------------------------------
; Ellipse
; Draw an ellipse the list of vertices.
;
; Notes:
;  The Foreground Color is set as follows:
;      (a) from the Brush struct for Solid and Color-patterned style brushes.
;          The colors for these two cases are located at different
;          locations in the Physical Brush Struct. (refer to the code
;          and see documentation in RealizeObject procedure)
;      (b) from the DrawMode struct for Mono-patterned style brushes.
;
;------------------------------------------------------------------------
cProc   Ellipse_S3_BT, <FAR,PUBLIC,PASCAL,NODATA>, <esi, edi>
	parmD   lpDstDev                ;--> to the destination
	parmW   style                   ;Output operation
	parmW   Count                   ;# of points
	parmD   lpPoints                ;--> to a set of points
	parmD   lpPhysPen               ;--> to physical pen
	parmD   lpPhysBrush             ;--> to physical brush
	parmD   lpDrawMode              ;--> to a Drawing mode
	parmD   lpClipRect              ;--> to a clipping rectange if <> 0

	localW  DeviceFlags
	localW  BackgroundMode
	localW  EllipseFlags
	localW  X1
	localW  Y1
	localW  X2
	localW  Y2
        localW  wWidth
	localD  Rop2Code
        localD  lCmd
;
cBegin
	mov     ax,es
	mov     gs,ax
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data
;
; Return to GDI if we are unable to support rectangle. Don't go to the DIB
; engine, DIB engine doesn't support rectangles.
;
	lfs     si,lpDstDev             ;FS:SI-->lpDstDev
        bts     fs:[si].deFlags,BUSY_BIT;
        jc      O_DIBEngineToScreen     ;

ECheckBrush:
	mov     EllipseFlags,0          ;initialize local flags
	cmp     lpPhysPen,0             ;is there a pen ?
	je      short ECheckCoordinates ;no,
	les     di,lpPhysPen            ;
	cmp     es:[di].dpPenStyle,LS_NOLINE
	je      short ECheckCoordinates ;
	or      EllipseFlags,EF_OUTLINE ;
;
ECheckCoordinates:
	lds     si,lpPoints             ;
	mov     ax,word ptr ds:[si]     ;
	mov     X1,ax                   ;
	mov     bx,word ptr ds:[si+2]   ;
	mov     Y1,bx                   ;
	mov     cx,word ptr ds:[si+4]   ;
	mov     X2,cx                   ;
	mov     dx,word ptr ds:[si+6]   ;
	mov     Y2,dx                   ;
;
	sub     cx,ax                   ;don't handle size smaller than 2x2
	cmp     cx,2                    ;i.e. radius=1
	jl      EReturnToGDI            ;
	sub     dx,bx                   ;
	cmp     dx,2                    ;
	jl      EReturnToGDI            ;
	cmp     cx,2048                 ;or bigger than 2048x2048
	jg      EReturnToGDI            ;
	cmp     dx,2048                 ;
	jg      EReturnToGDI            ;
;
ECheckClipRect:
        mov     eax,dDestFmt
        mov     lCmd,eax
	cmp     lpClipRect,0            ;any clipping rectangle passed?
	je      short ECheckCursor      ;
        or      lCmd,bClip_Enable
	or      EllipseFlags,EF_CLIP    ;indicate we have clip rect present
;
; Check the passed clipping rectangle. There is nothing to draw, if
; either width or height <= 0.
;
	lds     si,lpClipRect
	mov     ax,[si+4]               ;clip rectangle right
	sub     ax,[si]                 ;make sure width > 0
	jle     EReturnToGDI            ;
	mov     ax,[si+6]               ;clip rectangle bottom
	sub     ax,[si+2]               ;make sure height > 0
	jle     EReturnToGDI            ;
;
;If using the software cursor, exclude the cursor
;
;  DS:SI --> clipping rectangle if the clipping rectangle present
;
ECheckCursor:
	cmp	gs:bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	mov     ax,gs:wMMIOSelector        ;
	mov     fs,ax                   ;set FS to point to MMIO seg base
	WaitDMA				; wait for DMA to stop before MMIO access
@@:
	cmp     gs:wCursorType,SOFTWARECURSOR ;using software cursor ?
	jne     short ESetBusyBit       ;no
;
	xor     cx,cx                   ;assume full screen rect.
	xor     dx,dx                   ;
	mov     si,gs:wScreenWidth      ;
	mov     di,gs:wScreenHeight     ;
	test    EllipseFlags,EF_CLIP    ;is there a cliping rect?
	jz      short @F                ;no.
;
        mov     si,wptr lpClipRect[0]   ;
	mov     cx,[si].left            ;starting x
	mov     dx,[si].top             ;starting y
	mov     di,[si].bottom          ;ending y+1
	mov     si,[si].right           ;ending x+1
;
@@:     push    es
	push    lpDstDev                ;PDevice
	push    cx                      ;Left
	push    dx                      ;Top
	push    si                      ;Right
	push    di                      ;Bottom
	push    CURSOREXCLUDE           ;Flags
	call    BeginAccess
	pop     es
;
;Tell Windows we are busy on the device by setting BUSY bit in deFlags
;field.
;
ESetBusyBit:
	mov     ax,wMMIOSelector        ;
	mov     fs,ax                   ;set FS to point to MMIO seg base
if	1;pci_fix
	WaitEngineIdle fs
;	CheckFIFOSpace SIXTEEN_SLOTS
endif
;	cmp	bInDDraw,0		;need to patch?
;	je	short @F		;N, skip
	xor	eax,eax
        EngineWrite B_SRC_BASE
        EngineWrite B_DEST_BASE

        mov     ax,wScreenWidthBytes            ;bytes per scanlines
        mov     dx,ax
        rol     eax,16                          ;destination stride
        or      ax,dx                           ;source stride

        EngineWrite B_DEST_SRC_STR
;@@:
;
        xor     eax,eax
	les     di,lpDrawMode           ;ES:DI-->lpDrawMode
	mov     ax,es:[di].Rop2         ;get the desired Rop2
	dec     ax                      ;make it offset from 0
	mov     bx,CodeOFFSET S3Rop2TranslateTable
	xlat    cs:S3Rop2TranslateTable ;now AX has Mix Modes
        shl     eax,17                  ;get b8ROP256 in [24:17]
        mov     Rop2Code,eax            ;save it
        or      lCmd,eax
	mov     ax,es:[di].bkMode       ;
	mov     BackgroundMode,ax       ;
;
;Copy pattern to pattern registers for patterned brush
;
EDispatchBrush:
	cmp     lpPhysBrush,0           ;is there a brush ?
	je      EDrawObjects            ;no, don't need to fill interior
;
	or      EllipseFlags,EF_INTERIOR
	lds     si,lpPhysBrush          ;DS:SI-->lpPhysBrush
	mov     ax,[si].dp8BrushStyle   ;AL contains style
	mov     ah,[si].dp8BrushFlags   ;AH contains flags
;
	cmp     al,BS_PATTERN           ;patterned brush ?
	jne     short @F                ;no, check for solid brush
;
	test    ah,PATTERNMONO          ;monochrome pattern ?
	jz      EColorPatternBrush      ;no, it is a color pattern brush
	jmp     EMonoPatternBrush       ;yes, it is a mono pattern brush
;
@@:     cmp     al,BS_SOLID             ;solid brush ?
	jne     short @F                ;no, check for hatched brush
	test    ah,COLORSOLID           ;solid color ?
	jnz     ESolidBrush             ;yes
	jmp     EColorPatternBrush      ;no, we have dithered color pattern
;
@@:     cmp     al,BS_HATCHED           ;hatched brush ?
	je      EHatchPatternBrush      ;yes, treat it as mono pattern
;
;The brush is hollow. We perform the drawing if the binary raster op does
;not include source operands.
;
	and     EllipseFlags,NOT EF_INTERIOR
	jmp     EDrawObjects            ;
;
;For color patterned brush, we load the pattern into the pattern regs
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
EColorPatternBrush:
        or      lCmd,CLRPATBLT
;
;load 8x8 color pattern into the color pattern registers
;
        cld
        push    es
        mov     ax,wMMIOSelector
        mov     es,ax
        mov     ax,gs:wBpp_Byte         ;get bytes_per_pixel
	lea     si,[si].dp8BrushBits    ;DS:SI-->color pattern bitmap
	mov	di,COLOR_PATTERN        ;es:di -> color pattern regs
;
;ds:si -> dp8BrushBits
;es:di -> color pattern registers
;
	WaitEngineIdle es
@@:
	mov	cx,16                   ;16 dwords
	rep	movsd
;###
if	1;pci_fix
CheckFIFOSpace SIXTEEN_SLOTS es
endif
	dec	ax                      ;16bpp, xfer 16*2 dwords
	jnz	short @b                ;24bpp, xfer 16*3 dwords
        pop     es
	jmp     EDrawObjects

;
;Enter here for hatched patterned brush
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
EHatchPatternBrush:
	cmp     BackgroundMode,2        ;is background opaque ?
	jne     EReturnToGDI           ;no, its transparent
	lea     si,[si].dp8BrushMask    ;DS:SI-->hatch masks
;
;For Opaque case, load the hatched patterned into the MONO pat regs
;
EHatchOpaque:
        or      lCmd,MONOPATBLT
;
;Load hatched pattern brush into the mono pat regs
;
        CheckFIFOSpace FOUR_SLOTS

	mov     al,[si]
	ror	eax,8
	mov     al,[si+4]
	ror	eax,8
	mov     al,[si+8]
	ror	eax,8
	mov     al,[si+12]
	ror	eax,8
	EngineWrite B_MONO_PAT_0

	mov     al,[si+16]
	ror	eax,8
	mov     al,[si+20]
	ror	eax,8
	mov     al,[si+24]
	ror	eax,8
	mov     al,[si+28]
	ror	eax,8
	EngineWrite B_MONO_PAT_1

	lds     si,lpPhysBrush          ;DS:SI-->lpPhysBrush
	mov     eax,[si].dp8FgColor     ;
	EngineWrite B_PAT_FG_CLR

	mov     eax,[si].dp8BgColor
	EngineWrite B_PAT_BG_CLR

	jmp     EDrawObjects

;
;For monochrome patterned brush, we load the pattern into off-screen memory.
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
EMonoPatternBrush:
	cmp     BackgroundMode,2        ;is background opaque ?
	jne     EReturnToGDI           ;no, its transparent
        or      lCmd,MONOPATBLT
	lea     si,[si].dp8BrushMono    ;DS:SI-->mono pattern bitmap

        CheckFIFOSpace FOUR_SLOTS

	mov     al,[si]
	ror	eax,8
	mov     al,[si+4]
	ror	eax,8
	mov     al,[si+8]
	ror	eax,8
	mov     al,[si+12]
	ror	eax,8
	EngineWrite B_MONO_PAT_0

	mov     al,[si+16]
	ror	eax,8
	mov     al,[si+20]
	ror	eax,8
	mov     al,[si+24]
	ror	eax,8
	mov     al,[si+28]
	ror	eax,8
	EngineWrite B_MONO_PAT_1

	les     di,lpDrawMode           ;ES:DI-->lpDrawMode
	mov     eax,es:[di].TextColor   ;
	EngineWrite B_PAT_FG_CLR

	mov     eax,es:[di].bkColor     ;
	EngineWrite B_PAT_BG_CLR

        jmp     EDrawObjects
;
;
;  DS:SI-->lpPhysBrush
;
ESolidBrush:
if patch16
	test	BWFlag,08		;ViRGE/VX running fast page mode.
	jz	@F
        or      lCmd,MONOPATBLT
        CheckFIFOSpace FOUR_SLOTS

	mov	eax,-1
	EngineWrite B_MONO_PAT_0
	EngineWrite B_MONO_PAT_1

	mov     eax,dword ptr [si].dp8BrushBits
	EngineWrite B_PAT_FG_CLR
	EngineWrite B_PAT_BG_CLR
        jmp     short EDrawObjects
@@:
endif
        or      lCmd,RECT_FILL + bPAT_Mono

        CheckFIFOSpace ONE_SLOT

	mov     eax,dword ptr [si].dp8BrushBits
	EngineWrite B_PAT_FG_CLR        ;get brush solid colour
        jmp     short EDrawObjects


EHollowBrush:
        or      lCmd,RECT_FILL + bPAT_Mono

EDrawObjects:
	test    EllipseFlags,EF_CLIP    ;do we have a clip rectangle ?
	jz      short EBuildVertices    ;no, skip setting of clip

	CheckFIFOSpace TWO_SLOTS

	lds     si,lpClipRect           ;DS:SI-->lpClipRect
        mov     ax,[si].left            ;left clipping
        rol     eax,16                  ;in high dword
        mov     ax,[si].right           ;get right clipping
	dec	ax
        EngineWrite B_CLIP_L_R

        mov     ax,[si].top             ;get top clipping
        rol     eax,16                  ;in high dword
        mov     ax,[si].bottom          ;get bottom clipping
	dec	ax
        EngineWrite B_CLIP_T_B          ;set Y-axis clipping
;
EBuildVertices:
	cCall   LookUpTableEllipse,<X1,Y1,X2,Y2>
;
	test    EllipseFlags,EF_INTERIOR
	jz      short ECheckPen         ;
;
	cCall   CircleFill
;
ECheckPen:
	test    EllipseFlags,EF_OUTLINE ;
	je      EClearBusyBit
	cCall   EllipseBorder           ;

EClearBusyBit:

;###patch
if	1
CheckFIFOSpace FOUR_SLOTS, fs
mov     eax,00000001h
EngineWrite B_RWIDTH_HEIGHT, fs
EngineWrite B_RSRC_XY, fs
EngineWrite B_RDEST_XY, fs
mov     eax,07980000h+bDraw+CPUAlign_dword+bFirst_dword
or      eax,dDestFmt
EngineWrite B_CMD_SET, fs
endif

	cmp     gs:wCursorType,SOFTWARECURSOR
	jne     short EExitSuccess      ;
	push    lpDstDev                ;
	push    CURSOREXCLUDE           ;
	call    EndAccess               ;
;
EExitSuccess:
	mov     ax,1                    ;indicate success
	jmp     short EExit             ;
;
EReturnToGDI:
	mov     ax,-1                   ;
;
EExit:
	lfs     si,lpDstDev             ;
	and	fs:[si].deFlags,not BUSY
cEnd
;
;
sEnd    OutputSeg
end
.286c
