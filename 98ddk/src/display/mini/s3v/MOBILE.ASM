
;----------------------------------------------------------------------------
; Mobile.asm
;----------------------------------------------------------------------------
	.xlist
	include cmacros.inc
        include macros.inc
	include valmode.inc
	include dibeng.inc
	include s3.inc
	.list

	externNP	PhysicalEnable	;in vga.asm
	externNP	FindMode	;in vga.asm
	externNP	Is_S3		;in vga.asm
	externNP	XYtoRes 	;in init.asm
	externNP	GetMemSize	;in init.asm
	externNP	AdjustLCDParms	;in vga.asm
	externFP	ReloadCursor_far
	externFP	SetRAMDAC_far	;in palette.asm
	externFP	SetPSAddress	;in hwcursor.asm
	externFP	MovePanningCursor
	externFP	MoveHWCursor
	externFP	SetHWCursor
	externFP	DIB_SetCursorExt
	externFP	DIB_MoveCursorExt
	externFP	MoveCursor	;in cursor.asm
	externFP	EnableCenteringInSPMode
	externFP	EnableExpansionInSPMode
        externFP        EnableExpansionCenteringInSPMode
	externFP	DisableCenteringInSPMode
	externFP	DisableExpansionInSPMode
	externFP	GetK2vFifo
	externFP	GetK2vRatio
	externFP	InitK2vTable			;in k2v.c
	externFP	DDCreateDriverObject
	externFP	TurnOffStreamsProcessorMode	; in dddrv.c
	externNP        VirgeMXSyncVCount
	externNP        VirgeMXCfgSPSrc
        externFP        TVInitPosition
        externFP MoveDualImgPanningCursor  ;in HWCURSOR.ASM

sBegin	Data
;
externB 	bPanelType		;in vga.asm
externB 	bStreamProcessorFlags	;in vga.asm
externB		bShowSSWindow		;in hwcursor.asm
externB 	InSrcBlt
externB 	SWCursor
externB 	CursorSaveArea		;in hwcursor.asm
externB		bShiftPelsToDWords	;in init.asm
externB		bShiftPelsToBytes	;in init.asm
externW		wScreenWidthBytes	;in vga.asm
externW 	pAdapterEntry		;in vga.asm
externW 	wRefreshRate		;in vga.asm
externW 	wDeviceId		;in vga.asm
externW 	wMMIOSelector		;in vga.asm
externW 	wLogResolution		;in init.asm
externW 	wBpp			;in init.asm
externW 	wCursorType		;in cursor.asm
externW 	MonoCursorType		;in cursor.asm
externW 	wAbsX			;in cursor.asm
externW 	wAbsY			;in cursor.asm
externW 	XHot
externW 	wCurrentMode
externW 	wScreenWidth		;in VGA.asm
externW 	wScreenHeight		;in VGA.asm
externW 	wVWTop			;in hwcursor.asm
externW 	wVWLeft 		;in hwcursor.asm
externW 	wVWRight		;in hwcursor.asm
externW 	wVWBottom		;in hwcursor.asm
externW 	wVWTop1			;in hwcursor.asm
externW 	wVWLeft1 		;in hwcursor.asm
externW 	wVWRight1		;in hwcursor.asm
externW 	wVWBottom1		;in hwcursor.asm
externW 	wVWTop2			;in hwcursor.asm
externW 	wVWLeft2 		;in hwcursor.asm
externW 	wVWRight2		;in hwcursor.asm
externW 	wVWBottom2		;in hwcursor.asm
externW 	wLogScreenWidth		;in cursor.asm
externW 	wPhysScreenWidth	;in cursor.asm
externW 	wPhysScreenHeight	;in cursor.asm
externW 	MonoCursorType		;in cursor.asm
externW		wCenterY		;in vga.asm
externW		wCenterX		;in vga.asm
externW 	MoveHWCursorFn		;in vga.asm
externW 	ResolutionTable 	;in vga.asm
externW 	wCRTResolution	 	;in vga.asm
externW 	wMaxResolution	 	;in vga.asm
externW		CursorFmt		;in cursor.asm
externD	        SavedDispStart		;in vga.asm
externD 	dwSrcWH			;in hwcursor.asm
externD 	dwSSXY			;in hwcursor.asm
externD 	dwSSWH			;in hwcursor.asm
externD 	dwPSAddr		;in hwcursor.asm
externD 	dwSSAddr		;in hwcursor.asm
externD 	dwFlipOffset		;in hwcursor.asm
externD 	dwBlendCtrl		;in hwcursor.asm
externD 	dwClipOffset		;in hwcursor.asm
externD 	MonoCursorFn		;in cursor.asm
externD 	SetCursorFn		;in cursor.asm
externD 	dwVideoMemorySize	;in vga.asm
externD 	CursorCacheAddr 	;in vga.asm
externD 	lpDriverPDevice 	;in init.asm
externD 	RepaintAddr		;in sswich.asm
externD 	lpColorTable		;in palette.asm
externD 	MPEGInt2fHandler
externD 	SavedSPFIFOReg		;in sswitch.asm
externD 	CurrentLogWidRegs
;
align	4
public wPhysResolution, dwPanelSize, wPanelResolution, dwVideoType
public wDualImageTableOffset, dwTVSize, wTVResolution, dwTVCenteringAdj
dwPanelSize		dd	0	;width:height
dwTVSize		dd	0	;width:height
dwTVCenteringAdj	dd	0	;width:height
dwVideoType		dd	0	;software codec, hwcodec, or live video
wPhysResolution 	dw	0	;resolution of physical display
wPanelResolution	dw	0	;panel resolution
wTVResolution		dw	0	;MODE_RES_640x400	;TV resolution
wDualImageTableOffset	dw	0	;0:un-initialized, -1:DM not supported
;
public wPrevTop, wPrevLeft
wPrevTop		dw	0
wPrevLeft		dw	0
;
public bOutputType, bSavedDisplayMode
bOutputType		db	0	;RGB, TV : NTSC or PAL
bSavedDisplayMode	db	0
;
align	4
public S3MobileData
S3MobileData		db	(size S3_MOBILE_DATA)  dup (0)
;
;

;
MIN_OFFSCREEN		equ	10000h
MIN_VALID_MCLK		equ	38
MAX_VALID_MCLK		equ	50
TABLE_SIZE		equ	6
NUM_TFT_TABLES		equ	4

;
;The following are dual image tables for each panel type. Each table is
;a two dimensional array with each column represents a resolution and each
;row represents a color depth or bit per pixel. The following is a
;generic format of each table
;
; PanelTypeTable	640x480 	800x600 	1024x768
;	8bpp		  S		  S		   NS
;	16bpp		  S		  NS		   NS
;
;	S	: supported
;	NS	: not supported

TftVgaTable	label	byte
	db	1, 0, 0
	db	1, 0, 0

TftSVgaTable	label	byte
	db	1, 1, 0
	db	1, 1, 0

TftXgaTable	label	byte
	db	1, 1, 1
	db	1, 1, 1
;	db	0, 0, 0

TftSXgaTable	label	byte
	db	1, 1, 1
	db	1, 1, 1
;	db	0, 0, 0
;	db	0, 0, 0

DDStnVgaTable	label	byte
	db	1, 0, 0
	db	1, 0, 0

DDStnSVgaTable	label	byte
	db	1, 1, 0
	db	1, 1, 0
;	db	0, 0, 0

DDStnXgaTable	label	byte
	db	1, 1, 1
	db	1, 1, 1
;	db	0, 0, 0
;	db	0, 0, 0
;
;
sEnd	Data


createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg
	assumes cs,InitSeg
	.386

;----------------------------------------------------------------------------
; ValidateMobileMode
; Validate the passed mode based on current setting for mobile chips.
;
; Entry
;	ax	new resolution
;	bx	new color depth
;	esi	XY res (x in msw, y in lsw)
; Exit
;	ax	VALMODE_YES or VALMODE_NO_NOMEM
;----------------------------------------------------------------------------
cProc	ValidateMobileMode,<NEAR,PUBLIC>
cBegin
	assumes ds,Data
        mov     wBpp,bx                 ; Update wBpp
	test	S3MobileData.dwMobileCaps,DUOVIEW_IN_8BPP
	jz	short VMM_FindMode      ; No- don't perform the rejection
	test	S3MobileData.bCurrentDisplay,DUOVIEW+HK_DUAL_IGA
	jz	short VMM_FindMode      ; No- don't perform the rejection
	cmp	wBpp,8                  ; Is it 8bpp mode?
	je	short VMM_FindMode      ; Yes- exit
	xor	bx,bx                   ; No- reject the mode
	jmp	short VMM_Exit		; Return Failure
;
VMM_FindMode:
	cCall	ValidateDualImageSupport
	or	bx,bx
	jz	short VMM_Exit		;not enough stride, done
@@:
	push	esi
	push	wPhysResolution
	mov	wPhysResolution,ax
	mov	pAdapterEntry,0
	call	FindMode		;
	pop	wPhysResolution
	pop	esi
	or	bx,bx
	jz	short VMM_Exit		;not enough memory, done
;
VMM_Done:
	mov	bx,1			;return success

VMM_Exit:
cEnd

;----------------------------------------------------------------------------
; ValidateDualImageSupport
; Check to see if dual image mode can be supported
; Entry:
;	esi	width:height
; Exit:
;	bx	0: not support
;		1: support
;----------------------------------------------------------------------------
cProc	ValidateDualImageSupport,<NEAR,PUBLIC>
cBegin
	assumes ds,Data
	push	ax
	test	S3MobileData.dwMobileCaps,MOBILE_FAMILY	;GX2?
	jnz	short VDIS_OK 				;jmp if not
	test	S3MobileData.dwMobileCaps,DUAL_IMAGE
	jz	short VDIS_OK
	mov	ax, wPhysResolution
	cmp	ax,MODE_RES_1024x768
	ja	short VDIS_Fail
	cmp	wBpp, 16
	jne	short @f
	cmp	ax,MODE_RES_800x600
	ja	short VDIS_Fail
@@:
	cmp	wBpp, 24
	jne	short VDIS_OK
	cmp	ax,MODE_RES_640x480
	ja	short VDIS_Fail
VDIS_OK:
	mov	bx, 1
VDIS_Done:
	pop	ax
cEnd

VDIS_Fail:
	xor	bx, bx
	jmp	short VDIS_Done

;----------------------------------------------------------------------------
; ValidateDualImageMode
; Check to see if dual image mode can be supported in the current
; configuration including current display mode, panel type, MCLK value,
; requested resolution and color depth, and memory size.
;
; Entry:
;	edi	width:height
; Exit:
;	ax	new logical resolution
;	bx	new physical resolution
;	NC	support dual image
;	CY	can't support dual image
;----------------------------------------------------------------------------
cProc	ValidateDualImageMode,<NEAR,PUBLIC>
cBegin
	assumes ds,Data

;### work around for GX2
	test	S3MobileData.dwMobileCaps,MOBILE_FAMILY	;GX2?
	jnz	short @F				;N, continue
	mov	esi,edi
	xor	di,di
	add	esi,edi
	call	XYtoRes
	mov	bx,wPhysResolution
	clc
	jmp	VDM_Done
@@:
;
;CRT and panel must be active in order to switch to dual image mode
;
	cmp	S3MobileData.bCurrentDisplay,CRT_ONLY
	je	VDM_Fail
	cmp	S3MobileData.bCurrentDisplay,TV_ONLY
	je	VDM_Fail
;
;Next, ensure that requested resolution is smaller or equal to 1024 and
;smaller or equal to panel size.
;
	mov	ax,wPanelResolution
;
VDM_CheckResolution:
	mov	bx,wPhysResolution
	cmp	bx,ax			;can't set dual image if
	ja	VDM_Fail		;physResolution > panel size
	cmp	ax,MODE_RES_1024x768
	ja	VDM_Fail		;can't support >1024x768 res
	cmp	bx,MODE_RES_640x480
	jb	VDM_Fail
;
;Check color depth and dual image table
;
VDM_CheckTable:
	cmp	wBpp,16
	ja	VDM_Fail		;can't support 24 and 32bpp
	mov	ax,wDualImageTableOffset
	cmp	ax,0
	jne	short VDM_DualImageTable
	cCall	InitDualImageTable	;initialize dual image table
;
;bx	physical resolution
;ax	offset of dual image table
;
VDM_DualImageTable:
	cmp	ax,-1
	je	short VDM_Fail
	mov	si,bx
;	cmp	si,MODE_RES_1024x768
;	jb	short VDM_GetResBase
	cmp	si,MODE_RES_640x480
	je	short VDM_GetResBase
	dec	si			;account for 7x4 mode
	cmp	si,MODE_RES_800x600
	je	short VDM_GetResBase
	dec	si			;account for 10x6 mode
;
VDM_GetResBase:
	sub	si,MODE_RES_640x480	;calculate index into table
	mov	dx,wBpp
	shr	dx,3
	dec	dx
	mov	cx,dx
	shl	dx,1
	add	dx,cx			;each row has three bytes
	add	si,dx
	add	si,ax
	mov	ax,[si]
	or	ax,ax
	jz	VDM_Fail
;
public VDM_GetMemSize
VDM_GetMemSize:
	push	bx			;save new physical resolution
	mov	eax,[dwVideoMemorySize]
	or	eax,eax
	jnz	short VDM_CheckMemSize
	call	GetMemSize		;eax = available video memory
;
VDM_CheckMemSize:
	mov	ecx,eax
	ror	edi,16
	shl	di,1
	ror	edi,16
	mov	esi,edi 		;save for later use
	mov	eax,edi
	shr	eax,16			;width
	and	edi,0fffh		;height
	imul	edi
	xor	ebx,ebx
	mov	bx,wBpp
	shr	bx,3
	imul	ebx
	add	eax,MIN_OFFSCREEN
	pop	bx			;new physical resolution
	cmp	ecx,eax
	jb	short VDM_Fail
;
VDM_GetDualImageRes:
	call	XYtoRes
	clc

VDM_Done:
cEnd

VDM_Fail:
	stc
	jmp	short VDM_Done

;----------------------------------------------------------------------------
; GetPanelResolution
; Set wPhysResolution and dwPanelSize to the correct values
;
; Exit:
;	ax	wPanelResolution,ax
;----------------------------------------------------------------------------
cProc	GetPanelResolution,<NEAR,PUBLIC>
cBegin
        mov     ax,VESA_EXT_SERVICE     ; BIOS extended function
        mov     bx,06h                  ; Get panel information
        int     10h                     ; BIOS INT 10h call
        mov     dwPanelSize,edi         ; EDI = horz./vert. panel size
	mov	esi,edi                 ; ESI = horz./vert. panel size
	call	XYtoRes 	        ; Retrieve mode # for this resolution
	mov	wPanelResolution,ax     ; Initialize wPanelResolution
cEnd
;
;----------------------------------------------------------------------------
; ValidatePanningMode
; This function either sets physical resolution to the current desktop
; size or panel size depending upon status of current display mode. If
; current display mode is CRT_ONLY or TV_ONLY and panning is currently
; enabled, this funtions will disable panning and set physical resolution
; back to desktop resolution. If current display mode is LCD or simultaneous
; mode and logical resolution > physical resolution, this function will
; enable panning and set physical = panel size.
;
;----------------------------------------------------------------------------
cProc	ValidatePanningMode,<FAR,PUBLIC>,<di>
	localB	bInitMPEG
cBegin
	assumes ds,Data
	mov	bInitMPEG,0
	test	S3MobileData.dwMobileCaps,SP_ON
	jz	short VPM_SetMode
	cmp	word ptr MPEGInt2fHandler+2,0
	je	short VPM_SetMode
	push	MPEG_EXIT		;disable MPEG before mode set
	call	MPEGInt2fHandler
	dec	bInitMPEG

	mov	dx,CRTC_REG
	mov	al,67h
	out	dx,al
	inc	dx
	in	al,dx
	and	al,not 0ch
	out	dx,al			;disable SP

VPM_SetMode:
;do limited PhysicalEnable
	mov	ax,wPhysResolution
	push    pAdapterEntry
	mov     pAdapterEntry,0
	call	FindMode
	pop     pAdapterEntry
        or      bx,bx
        jz      VPM_Exit
	mov     wCurrentMode,bx

	push	bx
	mov	cx,bx		;VESA mode
	mov	bx,1		;set refresh rate
	mov	di,wRefreshRate
	mov	ax,VESA_EXT_SERVICE
	int     10h
	pop	bx

	or      bx,8000h                ;Don't erase the screen
	mov     ax,4f02h                ;set mode
	int     10h
;
;Enable MMIO Only
;
        mov     dx,CRTC_REG             ;
	mov	ax,4838h
	out	dx,ax
	mov	ax,0a039h
	out	dx,ax

        mov     dx,SEQ_REG              ;so I must do this stupid check.
        mov     al,09h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,080h                 ;Enable MMIO Only because the PNP BIOS
;###
        out     dx,al                   ;no longer does this.
        mov     dx,CRTC_REG             ;
        mov     al,40h
        out     dx,al
        inc     dx
        in      al,dx
        or      al,01h                  ;Enable Enhanced Register Access because
        out     dx,al                   ;the PNP BIOS no longer enables this.
;
;Enable access to the frame buffer and S3 Engine.
;
        mov     dx,CRTC_REG             ;
        mov     al,58h                  ;
        out     dx,al                   ;
        inc     dx                      ;
        in      al,dx                   ;
        or      al,13h                  ;Linear addressing
        out     dx,al                   ;
        dec     dx                      ;
	mov	al,53h
	out	dx,al
	inc	dx
	in	al,dx
	or	al,8h
	out	dx,al			;Memory map I/O

        dec     dx                      ;
	mov	al,45h
	out	dx,al
	inc	dx
	in	al,dx
	or	al,1
	out	dx,al			;enable HW cursor

	cmp	wCursorType,SOFTWARECURSOR
	je	short VPM_CheckPalette
	call	ReloadCursor_far	;Reload hardware cursor pattern
;
VPM_CheckPalette:
	cmp	wBpp,8
	ja	short VPM_RepaintWindow
	push	ds
	lds	si,lpColorTable 	;ds:[si]-->color table
	mov	cx,256
	je	short @f		;8bpp, load 256 indices
	mov	cx,16			; else, load 16 indices
@@:	xor	ax,ax
	call	SetRAMDAC_far		;set up initial palette
	pop	ds
;
VPM_RepaintWindow:
	call	RepaintAddr		;Force repaint of all windows

;reset BOTH IGAs' starting address to be 0
	mov	dx,SEQ_REG
        RD_IGA1_WR_BOTH
	xor	ebx,ebx
	mov	dx,CRTC_REG
	mov	al,0dh
	mov	ah,bl
	out	dx,ax			;set display start address low
	dec	al
	mov	ah,bh
	out	dx,ax			;display start high
	mov	eax,ebx
	shr	eax,8
	mov	al,69h
	out	dx,ax			;upper four bits of display start addr
	mov	dx,SEQ_REG
        RD_IGA1_WR_IGA1

	test	S3MobileData.dwMobileCaps,PANNING_ENABLED
	jz	short @F
	call	SetStride
@@:
;
;Enable stream processor and reset panning parameters
;
;	pop	ax
;	mov	bStreamProcessorFlags,al
	test	S3MobileData.dwMobileCaps,SP_ON
	jz	VPM_Exit
;
	mov	ax,wMMIOSelector
	mov	fs,ax
	assumes fs,nothing		;fs -> MMIOSelector
	WaitEngineIdle fs
	WaitVSYNC
	mov	dx,CRTC_REG
	mov	al,67h
	out	dx,al
	inc	dx
	in	al,dx
	or	al,0ch
	out	dx,al			;enable SP
	WaitEngineIdle fs
;
;If the new mode requires panning to be enabled, set PS 's address to
;(wVWTop, wVWLeft)
;
	test	S3MobileData.dwMobileCaps,PANNING_ENABLED
	jnz	short VPM_EnablePanning
	xor	eax,eax
	mov	fs:[PS_ADDR],eax	;Set PS 's addr to windows origin
	mov	eax,dwSSXY		;restore original SS's XY
	mov	fs:[SS_XY],eax
	mov	eax,dwSSWH		;restore original SS's size
	mov	fs:[SS_WH],eax
	mov	eax,dwSSAddr		;restore original SS's address
	mov	fs:[SS_ADDR],eax
	jmp	short VPM_Done

VPM_EnablePanning:
	cCall	SetPSAddress
	push	wAbsX
	push	wAbsY
	cCall	MovePanningCursor
;
VPM_Done:
	cmp	bInitMPEG,0ffh
	jne	short VPM_Exit
	push	MPEG_INIT
	call	MPEGInt2fHandler
;
VPM_Exit:
cEnd
;
;
cProc	AdjustVideoWindow,<NEAR,PUBLIC>
cBegin
	assumes	ds,Data
	mov	ax,wMMIOSelector
	mov	fs,ax
	assumes fs,nothing
;
	mov	eax,S3MobileData.dwMobileCaps
	test	eax,EXPANSION_ENABLED
	jnz	AVW_Done
	test	eax,CENTERING_ENABLED
	jnz	AVW_Centering
	test	eax,PANNING_IGA1_IGA2
	jnz	AVW_Panning
	and	eax,(SP_IGA1 or PANNING_IGA1)
	jz	AVW_Panning
	cmp	eax,(SP_IGA1 or PANNING_IGA1)
	je	AVW_Panning
;
public AVW_FullXY
AVW_FullXY:
	mov	eax,dwSSWH
	mov	fs:[SS_WH],eax
	mov	eax,dwSSAddr		;original SS 's address
	mov	fs:[SS_ADDR],eax
	mov	edx,dwSSXY
	cmp	bShowSSWindow,0 	;do we have to set blend control reg ?
	je	short AVW_SetWindowXY
	mov	eax,dwBlendCtrl
	mov	fs:[SP_BLENDCTRL],eax
	inc	bShowSSWindow		;clear flag
	jmp	short AVW_SetWindowXY
;
public AVW_Centering
AVW_Centering:
	mov	edx,dwSSXY
	add	edx,dword ptr wCenterY
;
AVW_SetWindowXY:
	mov	fs:[SS_XY],edx
	jmp	AVW_Done
;
public AVW_Panning
AVW_Panning:
	mov	ebx,dwSSXY
	mov	edi,ebx
	mov	esi,dwSSWH
	mov	ebx,dwSSXY
	mov	edi,ebx
	mov	esi,dwSSWH
	xor	eax,eax
	mov	edx,eax
	mov	ecx,eax
	ror	ebx,16			;SS_X
	sub	bx,word ptr wVWLeft
	js	short AVWP_SSClippedX
	ror	ebx,16			;newX:orgY
	mov	edi,ebx
	sub	bx,word ptr wVWTop
	js	AVWP_CalcNewYParms
;
	mov	fs:[SS_XY],ebx
	mov	eax,dwSSAddr		;original SS 's address
	cmp	eax,fs:[SS_ADDR]
	je	AVW_Done
	mov	fs:[SS_ADDR],eax
	mov	eax,dwSSWH
	mov	fs:[SS_WH],eax
	mov	dwClipOffset,0
;
AVWP_CheckBlendCtrol:
	cmp	bShowSSWindow,0
	je	AVW_Done
	mov	eax,dwBlendCtrl
	mov	fs:[SP_BLENDCTRL],eax
	inc	bShowSSWindow		;clear flag
	jmp	AVW_Done
;
public AVWP_SSClippedX
AVWP_SSClippedX:
	neg	bx			;clipped_x
	ror	esi,16			;original width
	sub	si,bx			;new width
	jge	short AVWP_NewSSWidth
	xor	esi,esi

AVWP_NewSSWidth:
	ror	esi,16
	shl	edi,16
	mov	di,1			;new starting x
	ror	edi,16
	mov	ax,bx			;x adjustment
	mov	cl,bShiftPelsToBytes
	shl	eax,cl
	cmp	wBpp,24
	jne	short AVWP_CheckSrcWidth
	add	ax,bx

;
;eax	x offset
;
public AVWP_CheckSrcWidth
AVWP_CheckSrcWidth:
	mov	edx,dwSrcWH
	shr	edx,16
	mov	ecx,dwSSWH
	shr	ecx,16
	inc	cx			;Secondary stream width
	cmp	dx,cx
	je	short AVWP_CheckY
	mul	edx
	xor	edx,edx
	idiv	ecx

;eax	x offset
;ebx	clipped_x:ori y
;edi	new x:org.y
;esi	new width: ori. height
;
public AVWP_CheckY
AVWP_CheckY:
	ror	ebx,16			;clipped_x:org y
	sub	bx,word ptr [wVWTop]
	js	short AVWP_CalcNewYParms
	mov	di,bx
	mov	ebx,eax 		;SS offset
	xor	eax,eax
	jmp	short AVWP_LinesToBytes
;
AVWP_CalcNewYParms:
	mov	di,1			;new y
	neg	bx
	sub	si,bx			;new height
	jge	AVWP_CalcYOffset
	xor	esi,esi
;
AVWP_CalcYOffset:
	xchg	eax,ebx 		;eax = y offset, ebx = x offset
	shl	eax,16
	shr	eax,16
	mov	dx,word ptr dwSrcWH
	mov	cx,word ptr dwSSWH
	cmp	dx,cx
	je	short AVWP_LinesToBytes
	mul	edx
	xor	edx,edx
	idiv	ecx			;eax = number of scanlines to skip

;ebx	x offset
;eax	y offset
;
public AVWP_LinesToBytes
AVWP_LinesToBytes:
	mov	edx,fs:[SS_STRIDE]
	and	edx,0fffh
	mul	edx
	mov	edx,fs:[SS_CONTROL]
	shr	edx,SSC_INPUT_DATA_POS
	and	edx,SSC_INPUT_DATA_MASK
	mov	cl,1			;assume 2-byte format
	cmp	dl,SSC_TWO_BYTE_FORMAT
	jbe	short AVWP_ConvertPelToBytes
	inc	cl
	cmp	dl,SSC_FOUR_BYTE_FORMAT
	je	short AVWP_ConvertPelToBytes
	mov	edx,ebx
	shl	ebx,1
	add	ebx,edx
	jmp	short AVWP_AddYOffset
;
AVWP_ConvertPelToBytes:
	shl	ebx,cl
;
AVWP_AddYOffset:
	add	ebx,eax 		;SS offset
	mov	eax,ebx
	and	eax,7
	sub	ebx,eax 		;quadword boundary

;ebx	offset to SS buffer address
;edi	newX : newY
;esi	new width: new height
;
AVWP_SetNewSSParms:
	mov	dwClipOffset,ebx
	add	ebx,dwSSAddr
	mov	fs:[SS_ADDR],ebx
	mov	fs:[SS_XY],edi
	cmp	esi,0
	jg	short AVWP_SetVWindowSize
	mov	esi,010001h
	mov	fs:[SS_WH],esi
	mov	dword ptr fs:[SP_BLENDCTRL],20h
	dec	bShowSSWindow
	jmp	AVW_Done
;
AVWP_SetVWindowSize:
	mov	fs:[SS_WH],esi
	cmp	bShowSSWindow,0 	;do we have to set blend control reg ?
	je	AVW_Done		;nop, almost done
	mov	eax,dwBlendCtrl
	mov	fs:[SP_BLENDCTRL],eax
	inc	bShowSSWindow		;clear flag

AVW_Done:
;
cEnd
;
;

;### skip this for now
if	0
;----------------------------------------------------------------------------
; ValidateDuoViewMode
;----------------------------------------------------------------------------
cProc	ValidateDuoViewMode,<FAR,PUBLIC>
	localB	bInitMPEG
cBegin
	assumes ds,Data
	mov	bInitMPEG,0
	test	S3MobileData.dwMobileCaps,SP_ON
	jz	short VDVM_SetMode
	cmp	word ptr MPEGInt2fHandler+2,0
	je	short VDVM_SetMode
	push	MPEG_EXIT		;disable MPEG before mode set
	call	MPEGInt2fHandler
	dec	bInitMPEG
;
VDVM_SetMode:
	push	word ptr bStreamProcessorFlags
	push	wCursorType
	call	PhysicalEnable
	pop	wCursorType
	mov	wCursorType,SOFTWARECURSOR
;
VDVM_CheckPalette:
	cmp	wBpp,8
	ja	short VDVM_RepaintWindow
	push	ds
	lds	si,lpColorTable 	;ds:[si]-->color table
	mov	cx,256
	je	short @f		;8bpp, load 256 indices
	mov	cx,16			; else, load 16 indices
@@:	xor	ax,ax
	call	SetRAMDAC_far		;set up initial palette
	pop	ds
;
VDVM_RepaintWindow:
	call	RepaintAddr		;Force repaint of all windows
;
	pop	ax
	mov	bStreamProcessorFlags,al
	test	S3MobileData.dwMobileCaps,SP_ON
	jz	VDVM_Exit

	cCall	TurnOffStreamsProcessorMode
	push	1
	cCall	DDCreateDriverObject
	and	bStreamProcessorFlags,not SP_ENABLED
;
VDVM_Done:
;
VDVM_Exit:
cEnd
endif

;----------------------------------------------------------------------------
; InitDualImageTable
; Initializes dual image tables for all panel types. Each table is
; a two dimensional matrix consisted of color depths and resolutions. Each
; entry in the table indicates whether a mode of interested is supported
; or not supported on a current panel.
;
; Entry
;	none
; Exit
;	ax = wDualImageTableOffset =
;		-1  : Dual image is not supported on this panel
;		offset of one of the above tables.
;
;Notes
;-----
; *** Need to ensure the algorithm used to calculate MCLK in this routine is
; still valid for M3/M5/GX2
;----------------------------------------------------------------------------
cProc	InitDualImageTable,<NEAR,PUBLIC>,<esi,edi,ebx>
cBegin
	assumes ds,Data
	mov	wDualImageTableOffset,-1

;At this point, dual image modes can only be supported on board running
;at 40Mhz mclk. Validate this condition and return immediately if mclk
;is not set at 40Mhz. To get MCLK value, check mclk control register. If
;bit 0 set, get value from dclk. Otherwise, get value from mclk

	xor	eax,eax
	mov	dx,SEQ_REG
	mov	al,MCLK_CONTROL_REG
	out	dx,al
	inc	dx
	in	al,dx
	mov	esi,eax
	shr	si,2
	and	si,11b			;si = IMCLK MUX2
	test	al,DCLK
	jz	short IDMT_GetFromMclk
;
;Get mclk value from dclk (sr12 and sr13)
;
	dec	dx
	mov	al,DCLK_LO_REG
	out	dx,al
	inc	dx
	in	al,dx
	mov	ebx,eax
	and	bx,D_PLL_N_MASK 	;bx = PLL N
	mov	ecx,eax
	shr	cl,D_PLL_R_POS
;
	dec	dx
	mov	al,DCLK_LO_REG
	out	dx,al
	inc	dx
	in	al,dx			;ax = PLL M
	jmp	short IDMT_CalcIMclkValue
;
;Get from Mclk (SR10, SR11)
;
IDMT_GetFromMclk:
	dec	dx
	mov	al,MCLK_LO_REG
	out	dx,al
	inc	dx
	in	al,dx
	mov	ebx,eax
	and	bx,M_PLL_N_MASK
	mov	ecx,eax
	and	cx,M_PLL_R_MASK
	shr	cx,M_PLL_R_POS
;
	dec	dx
	mov	al,MCLK_HI_REG
	out	dx,al
	inc	dx
	in	al,dx
	and	al,M_PLL_M_MASK
;
;Calculate the value of imclk
;if R == 0
;	R = 1
;else
;	R = 2<<(R-1)
;imclktmp = ((M+2)*14318)/((N+2)*R*100)
;imclk = imclktmp/10
;if ((imclktmp % 10 ) > 7)
;	imclk++
;
;ax	PLL M
;bx	PLL N
;cx	PLL R
;si	IMCLK MUX2
;
public IDMT_CalcIMclkValue
IDMT_CalcIMclkValue:
	add	bx,2
	shl	bx,cl			;bx = (N+2)*new R
	mov	cx,ax			;cx = PLL M
	mov	ax,100
	mul	ebx
	mov	edi,eax 		;edi = (N+2)*R*100
;
	mov	eax,ecx
	add	ax,2			; M + 2
	mov	ebx,14318
	mul	ebx			;eax=(M+2)*14318
	mov	ebx,edi
	div	ebx			;eax = imclktmp
;
	xor	edx,edx
	mov	bx,10
	div	bx
	cmp	edx,7
	jl	IDMT_CalcMclkValue
	inc	ax

;MCLK value is calculated as follow :
;MUX2 = 0  -> MCLK = IMCLK
;MUX2 = 1  -> MCLK = IMCLK/2
;MUX2 = 2  -> MCLK = IMCLK/3
;MUX2 = 3  -> MCLK = IMCLK/4
;ax	IMCLK
;si	MUX2
;
IDMT_CalcMclkValue:
	inc	si
	cmp	si,3
	jne	short IDMT_CanUseShift
	xor	dx,dx
	div	si
	jmp	short IDMT_CheckMCLK

IDMT_CanUseShift:
	shr	si,1
	mov	cx,si
	shr	ax,cl
;
;If MCLK value is not in the valid range, set wDualImageOffset to -1
;
public IDMT_CheckMCLK
IDMT_CheckMCLK:
	cmp	ax,MIN_VALID_MCLK
;;        jb      short IDMT_Done         ;not a valid MCLK, return
	cmp	ax,MAX_VALID_MCLK
;;        ja      short IDMT_Done         ;not a valid MCLK, return
;
;Get panel type and set table pointer to point to the correct table offset
;
	lea	bx,TftVgaTable		;offset of TftVgaTable
	mov	dx,SEQ_REG
	mov	al,PANEL_TYPE_REG
	out	dx,al
	inc	dx
	in	al,dx
	test	al,TFT_PANEL
	jnz	short IDMT_CheckPanelWidth
	add	bx,(TABLE_SIZE * NUM_TFT_TABLES)
;
;VGA, SVGA, XGA, or SXVGA is labled based on width and height of panel.
;
IDMT_CheckPanelWidth:
	dec	dx
	mov	al,FP_HORZ_SIZE
	out	dx,al
	inc	dx
	in	al,dx
	mov	ah,al
	dec	dx
	mov	al,FP_HORZ_OVERFLOW
	out	dx,al
	inc	dx
	in	al,dx
	test	al,FP_HORZ_EXT_BIT
	jnz	IDMT_Done		;width > 1024, return fail
	inc	ah
	shr	ah,5
	sub	ah,2			;compute index into table
	mov	al,ah
	shl	al,2
	shl	ah,1
	add	al,ah
	xor	ah,ah
	add	ax,bx
	mov	wDualImageTableOffset,ax
;
IDMT_Done:
	mov	ax,wDualImageTableOffset
cEnd
;

cProc	SetStride,<NEAR,PUBLIC>
cBegin
	assumes ds,Data
	mov	dx,SEQ_REG
        RD_IGA1_WR_BOTH
	mov	eax,CurrentLogWidRegs
	mov	dx,CRTC_REG
	out	dx,ax
	shr	eax,16
	out	dx,ax
	mov	dx,SEQ_REG
        RD_IGA1_WR_IGA1
cEnd


;----------------------------------------------------------------------------
;In a laptop environment, active displays can be switched from one state to
;another via a hot-key or a utility. A transition from current state to new
;state is first handled by the video BIOS (function 4f14, subfunction 03).
;After that the control is transfered to this function for finishing
;the display switch including set/reset panning, switch to the correct
;cursor type, and fifo values for hardware overlay if necessary.
;
;Input
;-----
; S3MobileData.bCurrentDisplay : New display type
;
;----------------------------------------------------------------------------
;
cProc	DisplaySwitchHandler,<FAR,PUBLIC>
cBegin
	mov	ax,DGROUP
	mov	es,ax
	mov	ds,ax
	assumes	ds,Data
	assumes es,Data
cEnd

public Calc_TV_SP_Centering
Calc_TV_SP_Centering:
	test	bOutputType,8	;PAL?
	jnz	short @F	;Y, different process
	mov	eax,28001e0h	;physical TV size
	sub	eax,dwTVSize	;displayable TV size
	shr	ax,1
	rol	eax,16
	shr	ax,1
	rol	eax,16
	mov	dwTVCenteringAdj,eax
	ret
@@:
	mov	eax,PAL_PHY_8x6		;physical TV size
	cmp	wLogResolution,MODE_RES_800x600
	jne	short CT_end
	sub	eax,dwTVSize	;displayable TV size
	shr	ax,1
	rol	eax,16
	shr	ax,1
	rol	eax,16
	mov	dwTVCenteringAdj,eax
CT_end:
	ret

PPROC   Calc_TV_SP_Centering_far	far
	call	Calc_TV_SP_Centering
	ret
Calc_TV_SP_Centering_far	endp
;
sEnd	InitSeg
end
