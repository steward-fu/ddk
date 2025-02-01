;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; POLYGON.ASM
; Copyright (c) 1993 S3 Incorporated
;----------------------------------------------------------------------------
;
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
;
PF_CLIP         equ     1h
PF_USEHARDWARE  equ     2h
PF_OUTLINE      equ     4h
PF_INTERIOR     equ     8h
;
;----------------------------------------------------------------------------
; E X T E R N
;----------------------------------------------------------------------------
externFP        BeginAccess
externFP        EndAccess
externNP        O_DIBEngineToScreen

externNP        GetHWPolygonParams
externNP        HardwarePolygonFill
externNP        DrawBorder
externNP        ClipLinePoints_PG
;
;
;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin          Data
;
externB         bAUTOEnable             ;autoexec flag
externB         bPolygonFlag            ;in INIT.ASM
externB 	bInDDraw
externW         wScreenWidth            ;in VGA.ASM
externW         wScreenHeight           ;in VGA.ASM
externW         wBpp
externW         wBpp_Byte
externW         wCursorType             ;in VGA.ASM
externW         FullScreenClipRect      ;
externW         wMMIOSelector
externW		wScreenWidthBytes            ;bytes per scanlines
externD         dDestFmt                ;bpp
externD         dCmd                    ;CMD_SET current value
externB         BWFlag                  ;in VGA.ASM
;
clip_min_y      dw      0
clip_max_y      dw      0
;
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

;------------------------------------------------------------------------
; Polygon
; Draw an alternate filled polygon given the list of vertices.
; Entry:
;       es:di   --> lpPBrush
;       ds      --> Data
; Exit:
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
cProc   Polygon_S3_BT, <FAR,PUBLIC,PASCAL,NODATA>, <esi, edi>
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
	localW  PolygonFlags
	localW  PointCount
	localW  MinYIndex
	localW  MaxYIndex
	localW  MaxY
        localW  MinY                    ;ViRGE draws from bottom up
	localW  YReflectCount
	localW  X1
	localW  Y1
	localW  X2
	localW  Y2
	localW  org_X1
	localW  org_X2
        localW	cur_left
        localW	cur_right
        localW	left_height
        localW	right_height
        localD	startX_left
        localD	startX_right
        localD	CurY
	localD  Rop2Code
        localD  lCmd                    ;local cmd_set

	localB	swapped
	localB	last_pixel
	localW  adj
	localD  coef
	localD  xstart
	localD  ystart
	localD  delta_x
	localD  delta_y

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
;	lfs     si,lpDstDev             ;FS:SI-->lpDstDev
;	test    fs:[si].deFlags,VRAM    ;is destination screen ?

        lfs     si,lpDstDev             ;FS:SI-->lpDstDev
        bts     fs:[si].deFlags,BUSY_BIT;
        jc      O_DIBEngineToScreen     ;

PGCheckBrush:
	mov     PolygonFlags,0          ;initialize local flags
	cmp     lpPhysPen,0             ;is there a pen ?
	je      short PGGetParams       ;no,
	les     di,lpPhysPen            ;
	cmp     es:[di].dpPenStyle,LS_SOLID
	jne	PGReturnToGDI
	cmp     es:[di].dpPenStyle,LS_NOLINE
	je      short PGGetParams       ;

	or      PolygonFlags,PF_OUTLINE ;
;
PGGetParams:
	mov     dx,gs:wScreenHeight     ;
	mov     gs:clip_min_y, 0        ;default min/max clipping
	mov     gs:clip_max_y, dx       ;for software polygon
;
	cCall   GetHWPolygonParams      ;
	jc      PGReturnToGDI           ;we can't handle it if carry
;
PGCheckClipRect:
        mov     eax,dDestFmt            ;
        mov     lCmd,eax
	cmp     lpClipRect,0            ;any clipping rectangle passed?
	je      short PGCheckCursor     ;
	or      PolygonFlags,PF_CLIP    ;indicate we have clip rect present
;
; Check the passed clipping rectangle. There is nothing to draw, if
; either width or height <= 0.
;
	lds     si,lpClipRect
	mov     ax,[si+4]               ;clip rectangle right
	sub     ax,[si]                 ;make sure width > 0
	jle     PGReturnToGDI           ;
	mov     ax,[si+6]               ;clip rectangle bottom
	sub     ax,[si+2]               ;make sure height > 0
	jle     PGReturnToGDI           ;

        or      lCmd,bClip_Enable       ;lCmd = destfmt + clip
;
;If using the software cursor, exclude the cursor
;
;  DS:SI --> clipping rectangle if the clipping rectangle present
;
PGCheckCursor:
	cmp	gs:bInDDraw,0		;need to patch?
	je	short @F		;N, skip
	mov     ax,gs:wMMIOSelector
	mov     fs,ax                   ;FS -> MMIO
	WaitDMA				; wait for DMA to stop before MMIO access
@@:
	cmp     gs:wCursorType,SOFTWARECURSOR ;using software cursor ?
	jne     short PGSetBusyBit      ;no
;
	xor     cx,cx                   ;assume full screen rect.
	xor     dx,dx                   ;
	mov     si,gs:wScreenWidth      ;
	mov     di,gs:wScreenHeight     ;
	test    PolygonFlags,PF_CLIP    ;is there a cliping rect?
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
PGSetBusyBit:
	lds     si,lpDstDev             ;DS:SI-->lpDstDev
	mov     ax,wMMIOSelector
	mov     fs,ax                   ;FS -> MMIO
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
	les     di,lpDrawMode           ;ES:DI-->lpDrawMode
	mov     ax,es:[di].Rop2         ;get the desired Rop2
	dec     ax                      ;make it offset from 0
	mov     bx,CodeOFFSET S3Rop2TranslateTable
	xlat    cs:S3Rop2TranslateTable ;now AX has Mix Modes
        shl     eax,17                  ;get Rop into CMD_SET[24:17]
        or      lCmd,eax                ;bROP+destFmt+bClip
        mov     Rop2Code,eax            ;save it for DrawBorder
	mov     ax,es:[di].bkMode       ;
	mov     BackgroundMode,ax       ;

PGDispatchBrush:
	cmp     lpPhysBrush,0           ;is there a brush ?
	jz      PGDrawObjects           ;no, don't need to fill interior
;
	or      PolygonFlags,PF_INTERIOR
	lds     si,lpPhysBrush          ;DS:SI-->lpPhysBrush
	mov     ax,[si].dp8BrushStyle   ;AL contains style
	mov     ah,[si].dp8BrushFlags   ;AH contains flags
;
	cmp     al,BS_PATTERN           ;patterned brush ?
	jne     short @F                ;no, check for solid brush
;
	test    ah,PATTERNMONO          ;monochrome pattern ?
	jz      PGColorPatternBrush     ;no, it is a color pattern brush
	jmp     PGMonoPatternBrush      ;yes, it is a mono pattern brush
;
@@:     cmp     al,BS_SOLID             ;solid brush ?
	jne     short @F                ;no, check for hatched brush
	test    ah,COLORSOLID           ;solid color ?
	jnz     PGSolidBrush            ;yes
	jmp     PGColorPatternBrush     ;no, we have dithered color pattern
;
@@:     cmp     al,BS_HATCHED           ;hatched brush ?
	je      PGHatchPatternBrush     ;yes, treat it as mono pattern
;
;The brush is hollow. We perform the drawing if the binary raster op does
;not include source operands.
;
	and     PolygonFlags,NOT PF_INTERIOR
	jmp     PGDrawObjects           ;

;For color patterned brush, we load the pattern into the pattern regs
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
PGColorPatternBrush:
        or      lCmd,CLRPAT_POLYGON

;        mov     eax,lCmd
;        WriteCMDAutoON P_CMD_SET
;
;load 8x8 color pattern into PATTERN_REGISTERS
;
	cld
        push    es
        mov     ax,wMMIOSelector
        mov     es,ax
        mov     ax,gs:wBpp_Byte        ;get bytes_per_pixel
	lea     si,[si].dp8BrushBits    ;DS:SI-->color pattern bitmap
	mov	di,COLOR_PATTERN        ;fs:di -> color pattern regs
	WaitEngineIdle es
@@:
	mov	cx,16                   ;16 dwords
	rep	movsd
;###
if	1;pci_fix
CheckFIFOSpace SIXTEEN_SLOTS es
endif
	dec	ax                      ;for 8bpp, we need to xfer 16dwords
	jnz	@b                      ;24bpp, need to xfer 16*3 dwords
        pop     es
	jmp     PGDrawObjects
;
;For monochrome patterned brush, we load the pattern into off-screen memory.
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
;ViRGE does NOT have transparent PatBLT, return TransHatch to GDI
PGHatchPatternBrush:
	cmp     BackgroundMode,2        ;is background opaque ?
	jne     PGReturnToGDI           ;no, its transparent
	lea     si,[si].dp8BrushMask    ;DS:SI-->hatch masks

;
;Load mono pattern brush into the mono pat regs
;DS:SI-->hatch masks
;
PGHatchOpaque:
        or      lCmd,MONOPAT_POLYGON

        CheckFIFOSpace FOUR_SLOTS

	mov     al,[si]
	ror	eax,8
	mov     al,[si+4]
	ror	eax,8
	mov     al,[si+8]
	ror	eax,8
	mov     al,[si+12]
	ror	eax,8
	EngineWrite P_MONO_PAT_0

	mov     al,[si+16]
	ror	eax,8
	mov     al,[si+20]
	ror	eax,8
	mov     al,[si+24]
	ror	eax,8
	mov     al,[si+28]
	ror	eax,8
	EngineWrite P_MONO_PAT_1

	lds     si,lpPhysBrush          ;DS:SI-->lpPhysBrush
	mov     eax,[si].dp8FgColor     ;
	EngineWrite P_PAT_FG_CLR

	mov     eax,[si].dp8BgColor     ;
	EngineWrite P_PAT_BG_CLR

	jmp     PGDrawObjects
;
;For monochrome patterned brush, we load the pattern into off-screen memory.
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
PGMonoPatternBrush:
	cmp     BackgroundMode,2        ;is background opaque ?
	jne     PGReturnToGDI           ;no, its transparent
        or      lCmd,MONOPAT_POLYGON
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
	EngineWrite P_MONO_PAT_0

	mov     al,[si+16]
	ror	eax,8
	mov     al,[si+20]
	ror	eax,8
	mov     al,[si+24]
	ror	eax,8
	mov     al,[si+28]
	ror	eax,8
	EngineWrite P_MONO_PAT_1

	les     di,lpDrawMode           ;ES:DI-->lpDrawMode
	mov     eax,es:[di].TextColor   ;
	EngineWrite P_PAT_BG_CLR

	mov     eax,es:[di].bkColor     ;
	EngineWrite P_PAT_FG_CLR

	jmp     PGDrawObjects

;
;  DS:SI-->lpPhysBrush
;
PGSolidBrush:
        or      lCmd,POLYGON_FILL + bPAT_Mono
;
        CheckFIFOSpace THREE_SLOTS
	mov	eax,-1
	EngineWrite P_MONO_PAT_0
	EngineWrite P_MONO_PAT_1
	mov     eax,dword ptr [si].dp8BrushBits ;get brush solid colour
	EngineWrite P_PAT_FG_CLR

        jmp     short PGDrawObjects

PGHollowBrush:
        or      lCmd,POLYGON_FILL

PGDrawObjects:
	test    PolygonFlags,PF_CLIP    ;do we have a clip rectangle ?
	jz      short PGFillInterior    ;no, skip setting of clip
;
	CheckFIFOSpace TWO_SLOTS
	lds     si,lpClipRect           ;DS:SI-->lpClipRect
        mov     ax,[si].left            ;left clipping
        rol     eax,16                  ;in high dword
        mov     ax,[si].right           ;get right clipping
	dec	ax
        EngineWrite P_CLIP_L_R

        mov     ax,[si].top             ;get top clipping
        rol     eax,16                  ;in high dword
        mov     ax,[si].bottom          ;get bottom clipping
	dec	ax
        EngineWrite P_CLIP_T_B          ;set Y-axis clipping

	mov     ax,[si+2]               ;
	mov     gs:clip_min_y,ax        ;
	mov     ax,[si+6]               ;
	mov     gs:clip_max_y,ax        ;
;
PGFillInterior:
	test    PolygonFlags,PF_INTERIOR
	jz      short PGCheckPen
;
	call	HardwarePolygonFill     ;call hardware support
;
PGCheckPen:
	test    PolygonFlags,PF_OUTLINE ;
	je      short PGClearBusyBit

        call	DrawBorder_BT              ;
;
PGClearBusyBit:
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
	jne     short PGExitSuccess     ;
	push    lpDstDev                ;
	push    CURSOREXCLUDE           ;
	call    EndAccess               ;
;
PGExitSuccess:
	mov     ax,1                    ;indicate success
	jmp     short PGExit            ;
;
PGReturnToGDI:
	mov     ax,-1                   ;
;
PGExit:
	lfs     si,lpDstDev             ;
	and	fs:[si].deFlags,not BUSY
cEnd
;
;
;-------------------------------------------------------------------------------
;DrawBorder
; This function is called to draw border of a given polygon.
; For horziontal and vertical lines, use Rect fill to draw border.
; Otherwise, use Line to draw the polygon border.
;
; Entry: es:di point to lpPPen
;        fs -> MMIO selector
; Exit:
;-------------------------------------------------------------------------------
PPROC	DrawBorder_BT	near
        and     lCmd,not Polygon
        or	lCmd,LINE_DRAW

	les     di,lpPhysPen            ;ES:DI -> lpPhysPen
	mov     eax,es:[di].dpPenColor  ;get pen color
	CheckFIFOSpace ONE_SLOT, fs
        EngineWrite L_PAT_FG_CLR, fs, eax
;
DBSetupDrawLoop:
	mov	last_pixel,1		;last pixel off
	mov     cx,Count                ;
	dec     cx                      ;
	lds     si,lpPoints             ;-> point array
;
;Next polyline draw enters here
;
DBLoop:
	push    cx                      ;save our loop counter
	lodsw
        mov     X1,ax
	lodsw
        mov     Y1,ax
	mov     cx,[si]                 ;cx = X2
	mov     dx,[si+2]               ;dx = Y2
        mov     X2,cx
        mov     Y2,dx

	cCall   ClipLinePoints_PG       ;clip (x1,y1)(x2,y2)
	jc      DBEndLoop

	mov	ax,Y1
	cmp	ax,Y2
	jge	short PL_1
;swap them
	mov     ax,X1
	xchg    ax,X2
	mov     X1,ax
	mov     ax,Y1
	xchg    ax,Y2
	mov     Y1,ax
	mov	last_pixel,1		;adjust first scan
	jmp	short PL_CalcDelta
PL_1:
	mov	last_pixel,2		;adjust last scan

PL_CalcDelta:
        mov     ax,X1                   ;ax = X1
        mov     cx,X2                   ;cx = X2
        mov     org_X1,ax
        mov     org_X2,cx

;### patch for bug14
if	1
test	BWFlag,1			;ViRGE/VX rev C or up ?
jz	@F				;no. go check if it's 24 bits mode
test	BWFlag,8			;fast page mode too
jz	PL_8_16				;no. skip it
cmp	wBpp,16				;16 bits mode ?
jnz	PL_8_16
jmp	short dopatch14			;no. skip it
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
mov     ebx,05980020h
or      ebx,dDestFmt            ;initialize dCmd
cmp	ax,cx
jge	short @F
or	ebx,02000000h
@@:
WriteCMDAutoOFF B_CMD_SET, fs, ebx

PL_8_16:
endif

        mov     di,Y1                   ;di = Y1
        sub     ax,X2                   ;deltaX = X1 - X2
        movsx   eax,ax
        mov     delta_x,eax
	sub     di,Y2                   ;deltaY = Y1 - Y2
        mov     cx,ax                   ;save delat x in cx
        movsx   edi,di
        mov     delta_y,edi
;
;eax =  delta x
;edi =  delta y
        or      ax,ax
        jnz     short not_vertical
        mov     coef,eax
        cmp     last_pixel,1
        jne     short @F
        dec     Y1
        jmp     short ver1
@@:
        cmp     last_pixel,2
        jne     short ver1
        inc     Y2
ver1:
        mov     ax,Y1
        cmp     ax,Y2
	jl      DBEndLoop
	mov	ax,X1
        shl     eax,20
        mov     xstart,eax
        jmp     prog_eng

not_vertical:
        mov     adj,1
        cmp     ax,0            ;positive delta x?
        jg      short @F        ;Y, continue
        mov     adj,-1
@@:
        or      di,di           ;horizontal?
        jnz     short @F        ;N, continue
        mov     coef,eax
        jmp     adj_endpoint

@@:     ;calculate coeficient
;        cdq
	xor	edx,edx
	test	eax,eax
	jns	short @F
	dec	edx
@@:
        sal     eax,20
        idiv    edi
        mov     coef,eax

        mov     bx,X1
        shl     ebx,20
        mov     xstart,ebx

        mov     ax,cx
        or      ax,ax
        jns     short @F
        neg     ax              ;abs(delta x)
@@:
        cmp     ax,di
        jle     short y_major
;x major
        mov     eax,coef
        sar     eax,1
        sub     ebx,eax
        cmp     cx,0                    ;delat x > 0
        jle     short @F                ;N, continue
        add     ebx,(1 shl 20) - 1      ;Y, adjust
@@:
        mov     xstart,ebx
        jmp     short adj_endpoint
y_major:
        add     ebx,1 shl 19
        mov     xstart,ebx
adj_endpoint:
        mov     ax,adj
        cmp     last_pixel,1
        jne     short @F
        sub     X1,ax
        jmp     short prog_eng
@@:
        cmp     last_pixel,2
        jne     short prog_eng
        add     X2,ax

prog_eng:
if	1;pci_fix
        CheckFIFOSpace TWELVE_SLOTS, fs
endif
        mov     edx,coef
	neg	edx
        EngineWrite Ldx, fs, edx

        mov     ax,X1
        rol     eax,16                  ;End0 in high dword
        mov     ax,X2                   ;End1 in low dword
        EngineWrite LXEnd0_End1, fs, eax

        xor     eax,eax
        mov     edx,xstart
        EngineWrite LXStart, fs, edx

        mov     ax,Y1
        EngineWrite LYStart, fs, eax

        xor     edx,edx
        mov     dx,Y1
        sub     dx,Y2
        inc     dx
        mov     ax,org_X1
	cmp	ax,org_X2
	jge	short @F
	or	edx,80000000h
@@:
        EngineWrite LYCnt, fs, edx      ;start draw

        mov     eax,lCmd
        EngineWrite L_CMD_SET, fs, eax

DBEndLoop:
	pop     cx                      ;line count
	dec     cx                      ;account for continous points
	jnz     DBLoop                  ;Next polyline
;
DBDone:
	ret
DrawBorder_BT	endp

sEnd    OutputSeg
end
.286c


