;----------------------------------------------------------------------------
; RECT.ASM
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
RF_CLIP         equ     1h
RF_INTERIOR     equ     2h
RF_OUTLINE      equ     4h
;
;
;----------------------------------------------------------------------------
; E X T E R N
;----------------------------------------------------------------------------
externFP        BeginAccess
externFP        EndAccess
externNP        O_DIBEngineToScreen
;
;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin          Data
;
externB         bRectangleFlag          ;in INIT.ASM
externB         InSrcBlt                ;in S3BLT.ASM
externB         bAUTOEnable             ;autoexec flag
externB 	bInDDraw
externW         wBpp_Byte
externW         wMMIOSelector           ;in VGA.ASM
externW         wScreenWidth            ;in VGA.ASM
externW         wScreenHeight           ;in VGA.ASM
externW         wCursorType             ;in VGA.ASM
externW         FullScreenClipRect      ;
externW		wScreenWidthBytes            ;bytes per scanlines
externD         dDestFmt                ;bpp
externD         dCmd                    ;CMD_SET current value
externB 	BWFlag                  ;in INIT.ASM
;
sEnd            Data
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
;------------------------------------------------------------------------
; Rectangle
; Draw a filled rectangle.
; Entry:
;       es      --> Data
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
cProc   Rectangle_S3_BT, <FAR,PUBLIC,PASCAL,NODATA>, <esi, edi>
	parmD   lpDstDev                ;--> to the destination
	parmW   style                   ;Output operation
	parmW   count                   ;# of points
	parmD   lpPoints                ;--> to a set of points
	parmD   lpPhysPen               ;--> to physical pen
	parmD   lpPhysBrush             ;--> to physical brush
	parmD   lpDrawMode              ;--> to a Drawing mode
	parmD   lpClipRect              ;--> to a clipping rectange if <> 0

        localD  lCmd                    ;cmd_set value
        localD  Rop2Code
	localW  DeviceFlags
	localW  BackgroundMode
	localW  RectFlags
	localV  InteriorRect,8

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
;
	mov     bx,es:wScreenWidth      ;BX contains display width
	mov     dx,es:wScreenHeight     ;DX contains display height
	lds     si,lpPoints             ;DS:SI-->lpPoints
;
; This code can only handle coordinates which are within (0,Xmax) and
; (0,Ymax). Anything that is outside of this range, return to GDI
;
	mov     ax,ds:[si]              ;get 1st X coordinate into AX
	or      ax,ax                   ;is X >= 0 ?
	jl      RReturnToGDI            ;no, don't do neg coord
	cmp     ax,bx                   ;is X < width of screen ?
	jge     RReturnToGDI            ;no, out of bound
;
	mov     cx,ds:[si+4]            ;get 2nd X coordinate into CX
	or      cx,cx                   ;is X >= 0 ?
	jl      RReturnToGDI            ;no, don't do neg coord
	cmp     cx,bx                   ;is X <= width of screen ?
	jg      RReturnToGDI            ;no, out of bound
	cmp     cx,ax                   ;is 2nd X > 1st X ?
	jle     RReturnToGDI            ;no, don't do it
;
	mov     ax,ds:[si+2]            ;get 1st Y coordinate into AX
	or      ax,ax                   ;is Y >= 0 ?
	jl      RReturnToGDI            ;no, don't do neg coord
	cmp     ax,dx                   ;is Y < height of screen ?
	jge     RReturnToGDI            ;no, out of bound
;
	mov     cx,ds:[si+6]            ;get 2nd Y coordinate into CX
	or      cx,cx                   ;is Y >= 0 ?
	jl      RReturnToGDI            ;no, don't do neg coord
	cmp     cx,dx                   ;is Y <= height of screen ?
	jg      RReturnToGDI            ;no, out of bound
	cmp     cx,ax                   ;is 2nd Y > 1st Y ?
	jle     RReturnToGDI            ;no, don't do it
;
	mov     RectFlags,0             ;initialize local flags
        mov     eax,dDestFmt
        mov     lCmd,eax                ;lCmd = destfmt

	cmp     lpPhysPen,0             ;is there a pen ?
	je      short @F                ;no, dont have to draw borders

	lds     si,lpPhysPen            ;DS:SI-->lpPhysPen
	cmp     [si].dpPenStyle,LS_SOLID
	jne	RReturnToGDI
	cmp     [si].dpPenStyle,LS_NOLINE ;null pen ?
	je      short @F                ;yes, go check brush

	or      RectFlags,RF_OUTLINE    ;indicate outline is needed

@@:     cmp     lpClipRect,0            ;any clipping rectangle passed?
	je      short RCheckCursor      ;no
	or      RectFlags,RF_CLIP       ;indicate we have clip rect present
;
; Check the passed clipping rectangle. There is nothing to draw, if
; either width or height <= 0.
;
	lds     si,lpClipRect           ;
	mov     ax,[si+4]               ;clip rectangle right
	sub     ax,[si]                 ;make sure width > 0
	jle     RReturnToGDI            ;
;
	mov     ax,[si+6]               ;clip rectangle bottom
	sub     ax,[si+2]               ;make sure height > 0
	jle     RReturnToGDI            ;
        or      lCmd,bClip_Enable       ;destfmt + clip
;
;If using the software cursor, exclude the cursor from clip rectangle
;
;  DS:SI --> clipping rectangle if the clipping rectangle present
;
RCheckCursor:
	cmp	gs:bInDDraw,0		;need to patch?
	je	short @F		;N, skip
        mov     ax,gs:wMMIOSelector
	mov     fs,ax                   ;set FS to point to MMIO seg base
	WaitDMA				; wait for DMA to stop before MMIO access
@@:
	cmp     gs:wCursorType,SOFTWARECURSOR ;using software cursor ?
	jne     short RSetBusyBit       ;no
;
	xor     cx,cx                   ;assume full screen rect.
	xor     dx,dx                   ;
	mov     si,gs:wScreenWidth      ;
	mov     di,gs:wScreenHeight     ;
	test    RectFlags,RF_CLIP       ;is there a cliping rect?
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
RSetBusyBit:
	lds     si,lpDstDev             ;DS:SI-->lpDstDev
        mov     ax,wMMIOSelector
	mov     fs,ax                   ;set FS to point to MMIO seg base
;###
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
	mov     ax,es:[di].Rop2         ;get the desired Rop2 (1-16)
	dec     ax                      ;make it offset from 0
	mov     bx,CodeOFFSET S3Rop2TranslateTable
	xlat    cs:S3Rop2TranslateTable ;now AX has Mix Modes
        shl     eax,17                  ;get b8ROP256 in [24:17]
        mov     Rop2Code,eax            ;save it
        or      lCmd,eax                ;lCmd = destFmt+clip+ROP
	mov     ax,es:[di].bkMode       ;
	mov     BackgroundMode,ax       ;
;
RCalcInteriorRect:
;        or      RectFlags,RF_INTERIOR   ;assumes we need to draw interior
	lds     si,lpPoints             ;DS:SI--> point array
	mov     ax,[si]                 ;
	mov     bx,[si+2]               ;
	mov     cx,[si+4]               ;
	mov     dx,[si+6]               ;
	sub     cx,ax                   ;CX contains width of rectangle
	dec	cx
	jle     short @F                ;internal width <= 0 ?
	sub     dx,bx                   ;DX contains height of rectangle
	dec	dx
	jle     short @F                ;internal height <= 0 ?
	test    RectFlags,RF_OUTLINE    ;
	jz      short RSaveInteriorRect ;
;
	inc     ax                      ;
	inc     bx                      ;
        dec     cx                      ;
        jle	short @F                ;
        dec     dx                      ;
        jle	short @F                ;
	jmp     short RSaveInteriorRect ;
;
@@:     and     RectFlags,NOT RF_INTERIOR ;indicate no interior to fill
	jmp     RDrawObjects            ;go draw objects
;
RSaveInteriorRect:
	mov     InteriorRect,ax         ;
	mov     InteriorRect+2,bx       ;
	mov     InteriorRect+4,cx       ;
	mov     InteriorRect+6,dx       ;
;
RCheckBrush:
	cmp     lpPhysBrush,0           ;is there a brush ?
	je      RDrawObjects            ;no, don't need to fill interior
;
	or      RectFlags,RF_INTERIOR   ;
	lds     si,lpPhysBrush          ;DS:SI-->lpPhysBrush
	mov     ax,[si].dp8BrushStyle   ;AL contains style
	mov     ah,[si].dp8BrushFlags   ;AH contains flags
;
	cmp     al,BS_PATTERN           ;patterned brush ?
	jne     short @F                ;no, check for solid brush
;
	test    ah,PATTERNMONO          ;monochrome pattern ?
	jz      RColorPatternBrush      ;no, it is a color pattern brush
	jmp     RMonoPatternBrush       ;yes, it is a mono pattern brush
;
@@:     cmp     al,BS_SOLID             ;solid brush ?
	jne     short @F                ;no, check for hatched brush
	test    ah,COLORSOLID           ;solid color ?
	jnz     RSolidBrush             ;yes
	jmp     RColorPatternBrush      ;no, we have dithered color pattern
;
@@:     cmp     al,BS_HATCHED           ;hatched brush ?
	je      RHatchPatternBrush      ;yes, treat it as mono pattern
;
;The brush is hollow. We perform the drawing if the binary raster op does
;not include source operands.
;
	and     RectFlags,NOT RF_INTERIOR
	jmp     RDrawObjects            ;

;
;For color patterned brush, we load the pattern into the pat registers
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO selector
;  lCmd = dDestFmt + Rop2Code + clip
;
RColorPatternBrush:
        or      lCmd,CLRPATBLT
;        mov     eax,lCmd
;        WriteCMDAutoON B_CMD_SET

;
;load 8x8 color pattern into PATTERN_REGISTERS
;
	cld
        push    es                      ;save es
        mov     ax,gs:wMMIOSelector     ;
        mov     es,ax                   ;es -> MMIO selector
        mov     ax,gs:wBpp_Byte         ;get bytes_per_pixel
	lea     si,[si].dp8BrushBits    ;DS:SI-->color pattern bitmap
	mov	di,COLOR_PATTERN        ;es:di -> color pattern regs
	WaitEngineIdle es
@@:
	mov	cx,16                   ;16 dwords
	rep	movsd                   ;move it
;###
if	1;pci_fix
CheckFIFOSpace SIXTEEN_SLOTS es
endif
	dec	ax                      ;for 8bpp, we need to xfer 16dwords
	jnz	short @b                ;24bpp, need to xfer 16*3 dwords
        pop     es                      ;restore es
	jmp     RDrawObjects            ;

;
;Hatch patterned brush
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
RHatchPatternBrush:
	cmp     BackgroundMode,2        ;is background opaque ?
	jne     RReturnToGDI           ;no, its transparent
	lea     si,[si].dp8BrushMask    ;DS:SI-->hatch masks

;
;Load mono pattern brush into the mono pat regs
;DS:SI-->hatch masks
;
RHatchOpaque:
        or      lCmd,MONOPATBLT
;        mov     eax,lCmd
;        WriteCMDAutoON B_CMD_SET

        CheckFIFOSpace FOUR_SLOTS

	mov     al,[si]
	ror	eax,8
	mov     al,[si+4]
	ror	eax,8
	mov     al,[si+8]
	ror	eax,8
	mov     al,[si+12]
	ror	eax,8
	EngineWrite B_MONO_PAT_0        ;

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

	mov     eax,[si].dp8BgColor     ;
	EngineWrite B_PAT_BG_CLR

	jmp     RDrawObjects            ;

;
;For monochrome patterned brush, we load the pattern to the mono pat regs
;
;  DS:SI-->lpPhysBrush
;  FS--> MMIO segment
;
RMonoPatternBrush:
	cmp     BackgroundMode,2        ;is background opaque ?
	jne     RReturnToGDI           ;no, its transparent
        or      lCmd,MONOPATBLT
	lea     si,[si].dp8BrushMono    ;DS:SI-->mono pattern bitmap

;        mov     eax,lCmd
;        WriteCMDAutoON B_CMD_SET

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
	EngineWrite B_PAT_BG_CLR

	mov     eax,es:[di].bkColor     ;
	EngineWrite B_PAT_FG_CLR

	jmp     RDrawObjects            ;
;
;  DS:SI-->lpPhysBrush
;  lCmd = dDestFmt + Rop2Code
;
RSolidBrush:
        or      lCmd,RECT_FILL + bPAT_Mono

        CheckFIFOSpace ONE_SLOT
	mov     eax,dword ptr [si].dp8BrushBits ;get brush solid colour
	EngineWrite B_PAT_FG_CLR

        jmp     short RDrawObjects

RHollowBrush:
        or      lCmd,RECT_FILL + bPAT_Mono
;
RDrawObjects:
	test    RectFlags,RF_CLIP       ;do we have a clip rectangle ?
	jz      short RFillInterior     ;no, skip setting of clip
;
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

RFillInterior:
	test    RectFlags,RF_INTERIOR   ;do we need to fill interior ?
	jz      RDrawBorder             ;no, go draw the border
;
	CheckFIFOSpace THREE_SLOTS
	mov     ax,InteriorRect+4       ;get width
	dec     ax                      ;width--
if patch16
	test	BWFlag,08h		;ViRGE/VX running fast page mode ?
	jz	skip_pat_16		;no
	push	eax
	cmp	ax,19
	jle	@F
	mov	ax,19
@@:
        rol     eax,16                  ;in high dword
	mov     ax,1		        ;get height in low dword
	EngineWrite B_RWIDTH_HEIGHT

        mov     ax,InteriorRect         ;get x
        rol     eax,16                  ;in high dword
        mov     ax,InteriorRect+2       ;get y in low dword
	EngineWrite B_RDEST_XY

        mov     eax,lCmd
        WriteCMDAutoOFF B_CMD_SET
	pop	eax
skip_pat_16:
endif
        rol     eax,16                  ;in high dword
	mov     ax,InteriorRect+6       ;get height in low dword
	EngineWrite B_RWIDTH_HEIGHT

        mov     ax,InteriorRect         ;get x
        rol     eax,16                  ;in high dword
        mov     ax,InteriorRect+2       ;get y in low dword
	EngineWrite B_RDEST_XY

        mov     eax,lCmd
        WriteCMDAutoOFF B_CMD_SET

;###patch
CheckFIFOSpace FOUR_SLOTS, fs
mov     eax,00000001h
EngineWrite B_RWIDTH_HEIGHT, fs
EngineWrite B_RSRC_XY, fs
EngineWrite B_RDEST_XY, fs
mov     eax,07980000h+bDraw+CPUAlign_dword+bFirst_dword
or      eax,dDestFmt
EngineWrite B_CMD_SET, fs

;
RDrawBorder:
	test    RectFlags,RF_OUTLINE    ;do we need to draw outlines ?
	jz      RRestoreClip            ;no, we are done

;
;Use rect fill to draw the rectangle borders
;
        mov     edx,lCmd
        or      edx,RECT_FILL + bPAT_Mono

if patch16
        CheckFIFOSpace ONE_SLOT, fs
else
        CheckFIFOSpace FOUR_SLOTS, fs
endif

	lds     si,lpPhysPen            ;DS:SI-->lpPhysPen
	mov     eax,[si].dpPenColor     ;get pen color
	EngineWrite B_PAT_FG_CLR

        lds     si,lpPoints             ;DS:SI-->lpPoints
	mov     ax,[si+4]               ;
	sub     ax,[si]                 ;DI contains width to draw
	dec     ax                      ;width--
	jl      RRestoreClip            ;
;
        xor     ecx,ecx                 ;
	mov     cx,[si+6]               ;
	sub     cx,[si+2]               ;
        dec     cx                      ;CX contains height to draw
	jl      RRestoreClip            ;
if patch16
	test	BWFlag,08h
	jz	skip_pat16
;
;Draws the top border, (with height = 1)
;
;do lines for patch16
	push	eax
	cmp	ax,19
	jle	@F
	mov	ax,19
@@:
        CheckFIFOSpace EIGHT_SLOTS, fs
        rol     eax,16                  ;get width in   
        mov     ax,1                    ;set height = 1
	EngineWrite B_RWIDTH_HEIGHT

	mov     ax,[si]                 ;get x
        rol     eax,16                  ;in high dword
	mov     ax,[si+2]               ;get y in low dword
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx

	mov     ax,[si+6]               ;get y
	dec     ax                      ;in low dword
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx

	pop	eax

        rol     eax,16                  ;get width in   
        mov     ax,1                    ;set height = 1
	EngineWrite B_RWIDTH_HEIGHT

	mov     ax,[si]                 ;get x
        rol     eax,16                  ;in high dword
	mov     ax,[si+2]               ;get y in low dword
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx

;
;Draws the bottom border, (with height = 1)
;
	CheckFIFOSpace TWO_SLOTS
	mov     ax,[si+6]               ;get y
	dec     ax                      ;in low dword
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx

	or	cx,cx
	jz      RRestoreClip            ;
;       
;Draws the left border (with width = 1)
;
;do lines for patch16
	push	cx
	mov	cx,1
        CheckFIFOSpace FIVE_SLOTS

	EngineWrite B_RWIDTH_HEIGHT, , ecx

	mov     ax,[si]                 ;get x
        rol     eax,16                  ;in high dword
	mov     ax,[si+2]               ;
	inc     ax                      ;
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx
	
	mov     ax,[si+4]               ;
	dec     ax                      ;get x
        rol     eax,16                  ;in high dword
	mov     ax,[si+2]               ;get y
	inc     ax                      ;in low dword
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx
	pop	cx

;now do the real Verticle lines

        CheckFIFOSpace THREE_SLOTS

	EngineWrite B_RWIDTH_HEIGHT, , ecx

	mov     ax,[si]                 ;get x
        rol     eax,16                  ;in high dword
	mov     ax,[si+2]               ;
	inc     ax                      ;
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx
;
;Draws the right border (with width = 1)
;
	CheckFIFOSpace TWO_SLOTS
	mov     ax,[si+4]               ;
	dec     ax                      ;get x
        rol     eax,16                  ;in high dword
	mov     ax,[si+2]               ;get y
	inc     ax                      ;in low dword
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx
	jmp     RRestoreClip            ;
;

skip_pat16:

        CheckFIFOSpace THREE_SLOTS, fs

endif	;patch16


;
;Draws the top border, (with height = 1)
;
        rol     eax,16                  ;get width in   
        mov     ax,1                    ;set height = 1
	EngineWrite B_RWIDTH_HEIGHT

	mov     ax,[si]                 ;get x
        rol     eax,16                  ;in high dword
	mov     ax,[si+2]               ;get y in low dword
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx

;
;Draws the bottom border, (with height = 1)
;
	CheckFIFOSpace TWO_SLOTS
	mov     ax,[si+6]               ;get y
	dec     ax                      ;in low dword
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx

	or	cx,cx
	jz      RRestoreClip            ;
;       
;Draws the left border (with width = 1)
;
        CheckFIFOSpace THREE_SLOTS

	EngineWrite B_RWIDTH_HEIGHT, , ecx

	mov     ax,[si]                 ;get x
        rol     eax,16                  ;in high dword
	mov     ax,[si+2]               ;
	inc     ax                      ;
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx
;
;Draws the right border (with width = 1)
;
	CheckFIFOSpace TWO_SLOTS
	mov     ax,[si+4]               ;
	dec     ax                      ;get x
        rol     eax,16                  ;in high dword
	mov     ax,[si+2]               ;get y
	inc     ax                      ;in low dword
	EngineWrite B_RDEST_XY

        WriteCMDAutoOFF B_CMD_SET, ,edx
;
RRestoreClip:
;
RClearBusyBit:
	cmp     gs:wCursorType,SOFTWARECURSOR
	jne     short RExitSuccess      ;
	push    lpDstDev                ;
	push    CURSOREXCLUDE           ;
	call    EndAccess               ;
;
RExitSuccess:

;###patch
CheckFIFOSpace FOUR_SLOTS, fs
mov     eax,00000001h
EngineWrite B_RWIDTH_HEIGHT, fs
EngineWrite B_RSRC_XY, fs
EngineWrite B_RDEST_XY, fs
mov     eax,07980000h+bDraw+CPUAlign_dword+bFirst_dword
or      eax,dDestFmt
EngineWrite B_CMD_SET, fs

if	1;pci_fix
WaitEngineIdle fs
endif

	mov     ax,1                    ;indicate success
	jmp     short RExit             ;
;
RReturnToGDI:
	mov     ax,-1                   ;
;
RExit:
	lfs     si,lpDstDev             ;FS:SI-->lpDstDev
        and     fs:[si].deFlags,not BUSY;clear busy bit
cEnd
;
;
sEnd    OutputSeg
end
