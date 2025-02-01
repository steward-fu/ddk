;----------------------------------------------------------------------------
; HWCURSOR.ASM
; Copyright (c) 1994 Microsoft Corporation
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
	.xlist
DOS5 = 1
	include macros.inc
	include cmacros.inc
	include dibeng.inc
	include s3.inc
	.list
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
IS_NULL         equ     000000001b      ;Cursor is null.
ENGINE          equ     1               ; chip in Engine mode
MEMORY          equ     0               ; chip in Memory mode

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP	DIB_MoveCursorExt
	externFP 	DIB_SetCursorExt
	externFP	MoveCursor		;in cursor.asm
	externNP        SetModeMemory
	externNP        SetModeEngine

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
externW	CursorFmt
externW MonoCursorType
externW CurrentChipMode
externD SetCursorFn
externD MonoCursorFn
externD lpDriverPDevice
externW wCursorXStretch
externW ScreenSelector
externW wScreenWidth
externW wScreenHeight
externW wScreenWidthBytes
externW wChipId
externW wDeviceId
externW wBpp
externB bShiftPelsToDWords
externB bShiftPelsToBytes
externD CursorCacheAddr
externW wDACType
externW wCursorType
externW wAbsY
externW wMMIOSelector			;in VGA.ASM
externB S3MobileData			;wMobileCaps
externW wPhysScreenWidth
externB bBlockedMode
externB b16BitDAC

align 4
globalW XHot,0
globalW YHot,0
globalW CursorXSize,0
globalW CursorYSize,0
globalW CursorWidthBytes,0
globalW CursorColor,0
public  CursorSaveArea, XparentANDMask
CursorSaveArea  db      4*32 dup(0ffh)   ;4 bytes/row, 32 rows -- AND Mask
                db      4*32 dup(0)      ;4 bytes/row, 32 rows -- XOR Mask

XparentANDMask  db      4*32 dup(0ffh)   ;transparent AND Mask
;
globalW CursorX,0
globalW CursorY,0
globalW UndoneX,0
globalW UndoneY,0
globalW wCursorUpdateCount,0
globalW wCursorDeltaX,0
globalW wCursorDeltaY,0
globalB CursorStatus,IS_NULL

public bShowSSWindow
bShowSSWindow		db	0	;-1 : restore blend control value
;
public wVWLeft, wVWRight, wVWTop, wVWBottom, dwSrcWH, dwSSXY, dwSSWH, dwSSAddr
public wVWTop1, wVWLeft1, wVWBottom1, wVWRight1
public wVWTop2, wVWLeft2, wVWBottom2, wVWRight2
public wLeftBound, wRightBound
wVWTop			dw	0	;Virtual window top
wVWLeft 		dw	0	;Virtual window left
wVWBottom		dw	0	;Virtual window bottom
wVWRight		dw	0	;Virtual window right
wVWTop1			dw	0	;First virtual window top
wVWLeft1 		dw	0	;First virtual window left
wVWBottom1		dw	0	;First virtual window bottom
wVWRight1		dw	0	;First virtual window right
wVWTop2			dw	0	;Second virtual window top
wVWLeft2 		dw	0	;Second virtual window left
wVWBottom2		dw	0	;Second virtual window bottom
wVWRight2		dw	0	;Second virtual window right
wLeftBound              dw      0       ;Virtual window left bound
wRightBound             dw      0       ;Virtual window right bound
dwSrcWH 		dd	0	;Width:height of source rect
dwSSXY			dd	0	;Original Secondary Stream XY
dwSSWH			dd	0	;Original Secondary Stream size
dwSSAddr		dd	0	;Original Secondary Stream addr
public	dwPSAddr, dwPSAddr2
dwPSAddr		dd	0	;Primary Stream addr
dwPSAddr2		dd	0	;Primary Stream addr
;
public dwFlipOffset, dwClipOffset, dwBlendCtrl
dwFlipOffset		dd	0
dwClipOffset		dd	0
dwBlendCtrl		dd	0	;current blend control value
;
public SavedDispStart, SavedDispStart2
SavedDispStart		dd	0
SavedDispStart2		dd	0
sEnd    Data

;
;Macro to select external DAC control register.
;
SetDACRegSelect macro   Select
	mov     dx,CRTC_REG             ;
	mov     al,055h                 ;Point to extented video DAC control
	out     dx,al                   ;
	inc     dx                      ;
	in      al,dx                   ;
	and     al,11111100b            ;
	or      al,Select               ;
	out     dx,al                   ;
	dec     dx                      ;set CRTC index back to 30H
	mov     al,30h                  ;
	out     dx,al                   ;
endm

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
assumes cs,Code
	.386

        externNP        VirgeMXSaveRegIndices
        externNP        VirgeMXRestRegIndices

;--------------------------------------------------------------------------
; SetHWCursor
;--------------------------------------------------------------------------
cProc   SetHWCursor, <FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD   lpCursorShape
	parmD   lpPDevice
	localB  DeviceFlags
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	push	SHC_DrawACursor
	cmp     seg_lpCursorShape,0     ;is this a null pointer?
	je	LoadCursorImage_Null	;yes. Load a null cursor.
	test	es:S3MobileData.wSPStyle,SP_M5
	jnz	short SHC_DrawACursor
	lds	si,lpCursorShape
	jmp	LoadCursorImage         ;no.  Load actual cursor.

PLABEL SHC_DrawACursor
	push    SHC_End                 ;Will return here.
	cmp     wDACType,DAC_ATT
	je      SetHardwareCursor
	cmp     wDACType,DAC_RGB525
	je      Set525HardwareCursor
	cmp     wDACType,DAC_TI025
	je      SetTi025HardwareCursor
	jmp     SetBtHardwareCursor

PLABEL SHC_End

PLABEL SHC_Ret
	mov     ax,1                    ;return success code
cEnd

;--------------------------------------------------------------------------
; MoveHWCursor
;--------------------------------------------------------------------------
cProc   MoveHWCursor,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmW   absX
	parmW   absY
	localB  DeviceFlags
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	mov     ax,absX                 ;get X-coordinate
	mov     bx,absY                 ;get Y-coordinate
	or      ax,ax                   ;exceeding low X-limit?
	jg      short @f                ;nope, go on
	xor     ax,ax                   ;fix cursor position to 0
@@:     mov     dx,wScreenWidth
	cmp     ax,dx                   ;exceeding high X-limit? (SCREEN_WIDTH)
	jb      short @f                ;nope, go on
	dec     dx
	mov     ax,dx                   ;fix cursor position (SCREEN_WIDTH)
@@:     or      bx,bx                   ;exceeding low Y-limit?
	jg      short @f                ;nope, go on
	mov     bx,1                    ;fix cursor position
@@:     mov     dx,wScreenHeight
	cmp     bx,dx                   ;exceeding high Y-limit? (SCREEN_HEIGHT)
	jb      short @f                ;nope, go do the cursor
	dec     dx
	mov     bx,dx                   ;fix cursor position (SCREEN_HEIGHT-1)
@@:     shl     ebx,16
	mov     bx,ax
	mov     dword ptr UndoneX,ebx   ;save both x and y at once.

PLABEL MHC_UndoneLoop
	mov     dword ptr CursorX,ebx   ;save both x and y at once.
	push    MHC_Exit
	cmp     wDACType,DAC_ATT
	je      MoveHardwareCursor
	cmp     wDACType,DAC_RGB525
	je      Move525HardwareCursor
	cmp     wDACType,DAC_TI025
	je      MoveTi025HardwareCursor
	jmp     MoveBtHardwareCursor

PLABEL MHC_Exit
	mov     ebx,dword ptr UndoneX
	cmp     ebx,dword ptr CursorX
	jne     MHC_UndoneLoop

PLABEL MHC_Ret
cEnd

;--------------------------------------------------------------------------
; CheckHWCursor
;--------------------------------------------------------------------------
cProc   CheckHWCursor,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	localW  DeviceFlags
cBegin
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	mov     ebx,dword ptr UndoneX
	cmp     ebx,dword ptr CursorX
	je      CHC_Exit_10
	mov     dword ptr CursorX,ebx
	push    CHC_Exit
	cmp     wDACType,DAC_ATT
	je      MoveHardwareCursor
	cmp     wDACType,DAC_RGB525
	je      Move525HardwareCursor
	cmp     wDACType,DAC_TI025
	je      MoveTi025HardwareCursor
	jmp     MoveBtHardwareCursor

PLABEL CHC_Exit
;	lds     si,lpDriverPDevice
;
;We check the cursor counter here. The counter delays the time to show
;up the cursor. The display of cursor is not delayed if the counter is 0.
;We will decrease the counter if the value is not 0 and display the cursor
;if the counter is down to 0 after decrement. CR48 (cursor Y high) is always
;reprogrammed to ensure hardware update the position.
;
PLABEL CHC_Exit_10
	cmp     wDACType,DAC_ATT        ;is AT&T comptable DAC ?
	jnz     short CHC_Exit_20       ;no, don't bother to do the following

        mov     ax,wMMIOSelector
	mov	fs,ax

        mov     ax,0a039h               ;unlock registers
        mov     fs:[83d4h],ax           ;use MMIO

	cmp     wCursorUpdateCount,0    ;is update counter = 0 ?
	jz      short @F                ;yes, do nothing

	dec     wCursorUpdateCount      ;decrement the counter
	jnz     short @F                ;is the counter down to 0 ? no,

	mov     ecx,dword ptr wCursorDeltaX
	mov     ah,cl                   ;
	mov     al,4eh                  ;setup X and Y offset to what they
        mov     fs:[83d4h],ax           ;supposed to be
	shr     ecx,16                  ;
	mov     ah,cl                   ;
	mov     al,4fh                  ;
        mov     fs:[83d4h],ax           ;

@@:     mov     al,48h                  ;
        mov     fs:[83d4h],al           ;
        mov     al,fs:[83d5h]           ;
        mov     fs:[83d5h],al           ;

PLABEL CHC_Exit_20
;	and     [si].deFlags,not BUSY

PLABEL CHC_Ret
cEnd

;--------------------------------------------------------------------------
; LoadCursorImage
; Entry:
;   ds:si-->Cursor Shape
;--------------------------------------------------------------------------
PPROC LoadCursorImage   near
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	lodsw                           ;Copy cursor hotspot
	mov     XHot,ax
	lodsw
	mov     YHot,ax
	lodsw                           ;Copy cursor dimensions
	mov     CursorXSize,ax
	lodsw
	mov     CursorYSize,ax
	lodsd
	mov	dword ptr CursorWidthBytes,eax
	lea     di,CursorSaveArea       ;ES:DI --> CursorSaveArea
	mov     cx,32*2                 ;1 dword/row, 32 rows, two masks
	rep     movsd
	ret

PLABEL LoadCursorImage_Null
	lea     di,CursorSaveArea       ;ES:DI --> CursorSaveArea
	xor	eax,eax
	dec	eax
	mov     cx,32	                ;1 dword/row, 32 rows -- AND Mask
	rep     stosd
	xor	eax,eax
	mov     cx,32	                ;1 dword/row, 32 rows -- XOR Mask
	rep     stosd
	ret

LoadCursorImage endp

;--------------------------------------------------------------------------
; SetHardwareCursor
;--------------------------------------------------------------------------
PPROC   SetHardwareCursor       near
	mov     ax,DGROUP
	mov     ds,ax
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

;
;We are not explicitly turning off cursor here. We simply hide the cursor
;by offseting the cursor to lower right corner.
;

        mov     ax,wMMIOSelector
	mov	fs,ax

        mov     ax,0a039h               ;unlock registers
        mov     fs:[83d4h],ax           ;use MMIO
	mov     ax,03f4eh               ;
        mov     fs:[83d4h],ax           ;use MMIO
	mov     ax,03f4fh               ;
        mov     fs:[83d4h],ax           ;use MMIO
	mov     al,30h                  ;reset CRTC index back to 30H
        mov     fs:[83d4h],al           ;use MMIO


;
;MoveHardwareCursor sets cursor offset X and Y which will display the
;cursor we hide in TurnOffHardwareCursor routine. We setup a counter
;here to prevent the cursor to be displayed at this time. The counter
;will be decremented in CheckHWCursor routine and setup the X and Y
;offset to show up the cursor if the counter downs to 0.
;
	call    LoadHardwareCursor      ;
	mov     wCursorUpdateCount,1    ;we delay 1 timer click
	ret                             ;

SetHardwareCursor       endp

;--------------------------------------------------------------------------
; LoadHardwareCursor
;--------------------------------------------------------------------------
PPROC   LoadHardwareCursor      near
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	push    es
	mov     ax,ScreenSelector
	mov     es,ax
	assumes es,nothing
	mov     edi,CursorCacheAddr     ;es:[edi]-->off-screen cache area
	lea     si,CursorSaveArea       ;ds:[si]-->cursor image in data segment

PLABEL LHC_CopyPattern
	mov     bx,128                  ;Offset to XOR mask
	mov     edx,0000FFFFh

PLABEL LHC_CopyMask
	mov     ch,32
PLABEL LHC_InnerLoop
	mov     cl,2
@@:     mov     eax,[si+bx-2]
	mov     ax,[si]
	mov     es:[edi],eax
	add     si,2
	add     edi,4
	dec     cl
	jnz     @b
	mov     es:[edi],edx
	mov     es:[edi+4],edx
	add     edi,8
	dec     ch
	jnz     LHC_InnerLoop
	mov     eax,edx
	mov     ecx,4*32
	rep     stos dword ptr es:[edi]

PLABEL LHC_SetCursorStorageAddr
	pop     es

	errnz $-MoveHardwareCursor
LoadHardwareCursor      endp

;--------------------------------------------------------------------------
; MoveHardwareCursor
;--------------------------------------------------------------------------
PPROC   MoveHardwareCursor      near
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

        call    VirgeMXSaveRegIndices

        mov     ax,wMMIOSelector
	mov	fs,ax

	mov     ecx,dword ptr CursorX   ;cx = CursorX (uninterruptable)
	shld    ebx,ecx,16              ;bx = CursorY
;###
;ifdef	lores_cursor_fix
	cmp	wScreenWidth,640	;resolution below 640 ?
	jae	short @F		;no. Skip the patch
	shl	bx,1
@@:
;endif
	sub     cx,XHot
	sub     bx,YHot
	mov     dx,CRTC_REG             ;DX --> CRTC index register
	mov     ax,0a039h               ;unlock S3 extension registers
        mov     fs:[83d4h],ax           ;
	xor     si,si
	or      cx,cx                   ;Is cursor negative in X?
	jns     short @F                ;No, the offset is 0
	neg     cx
	and     cx,003fh                ;AND to max 64 pixel offset
	mov     si,cx                   ;Save offset in SI
	xor     cx,cx                   ;Set X coordinate to 0
@@:     xor     di,di
	or      bx,bx                   ;Is cursor negative in Y?
	jns     short @F                ;No, the offset is 0
	neg     bx
	and     bx,003fh                ;AND to max 64 pixel offset
	mov     di,bx                   ;save offset in DI
	xor     bx,bx                   ;Set Y coordinate to 0
@@:

MHC_CalcXStretch:
	mov	ax,cx
	mov	cx,wCursorXStretch
	shl	ax,cl
	mov	cx,ax

PLABEL MHC_MaybeWaitForVSync
;At this point:
;       CX contains X coordinate
;       BX contains Y coordinate
;       SI contains horizontal offset
;       DI contains vertical offset
;
PLABEL MHC_SetPos
	mov     ax,di                   ;
	shl     eax,16                  ;
	mov     ax,si                   ;
	mov     dword ptr wCursorDeltaX,eax
	cmp     wCursorUpdateCount,0    ;
	jnz     short MHC_SetXY         ;

	mov     dx,CRTC_REG
	mov     ax,si
	mov     ah,al
	mov     al,04eh
        mov     fs:[83d4h],ax           ;Horizontal offset

	mov     ax,di
	mov     ah,al
	mov     al,04fh
        mov     fs:[83d4h],ax           ;Vertical offset

MHC_SetXY:
	mov     al,047h
	mov     ah,cl
        mov     fs:[83d4h],ax
	mov     al,046h
	mov     ah,ch
        mov     fs:[83d4h],ax           ;Cursor x

	mov     al,049h
	mov     ah,bl
        mov     fs:[83d4h],ax
	mov     al,048h                 ;We program CR48 in full V_Sync interval
	mov     ah,bh
        mov     fs:[83d4h],ax           ;Cursor y
	mov     al,30h                  ;reset CRTC index register to 30H
        mov     fs:[83d4h],al           ;

        call    VirgeMXRestRegIndices
	ret

MoveHardwareCursor      endp

;----------------------------------------------------------------------
; TurnOffBtHardwareCursor
;----------------------------------------------------------------------
PPROC   TurnOffBtHardwareCursor near
	mov     dx,CRTC_REG             ;
	mov     ax,0a039h               ;Unlock S3 extension registers
	out     dx,ax                   ;
	SetDACRegSelect 02h             ;Select BtDAC cmd 2
	mov     dx,03c9h                ;
	in      al,dx                   ;
	and     al,11111100b            ;set cursor mode 0
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Reset DAC Select (also sets CRTC
					;index register back to 30H)
	ret
TurnOffBtHardwareCursor endp

;----------------------------------------------------------------------
; TurnOnBtHardwareCursor
;----------------------------------------------------------------------
PPROC   TurnOnBtHardwareCursor  near
	mov     dx,CRTC_REG             ;
	mov     ax,0a039h               ;Unlock S3 extension registers
	out     dx,ax                   ;
	mov     dx,INPUT_STATUS_1_REG
@@:     in      al,dx                   ;Wait until display interval
	test    al,08h
	jnz     @b
@@:     in      al,dx                   ;Wait for a full V-Sync interval
	test    al,08h
	jz      @b
	SetDACRegSelect 02h             ;Select BtDAC cmd 2
	mov     dx,03c9h                ;
	in      al,dx                   ;
	and     al,11111100b            ;
	or      al,00000010b            ;set cursor mode 2
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Reset DAC Select
					;(also sets CRTC index register to 30H)
	ret
TurnOnBtHardwareCursor  endp

;----------------------------------------------------------------------
; SetBtHardwareCursor
;----------------------------------------------------------------------
PPROC   SetBtHardwareCursor     near
	mov     ax,DGROUP
	mov     ds,ax
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	call    TurnOffBtHardwareCursor
	call    LoadBtHardwareCursor
	call    TurnOnBtHardwareCursor
	ret
SetBtHardwareCursor     endp

;--------------------------------------------------------------------------
; LoadBtHardwareCursor
;--------------------------------------------------------------------------
PPROC   LoadBtHardwareCursor    near
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	lea     si,CursorSaveArea       ;ds:[si]-->cursor image in data segment
	mov     dx,CRTC_REG             ;unlock S3 extension registers
	mov     ax,0a039h
	out     dx,ax
	SetDACRegSelect 01h             ;Select BtDAC Cmd0 reg
	mov     dx,03c6h                ;
	in      al,dx                   ;
	or      al,10000000b            ;Set CR07 to 1
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Select address register
	mov     dx,03c8h                ;
	mov     al,1                    ;
	out     dx,al                   ;
	SetDACRegSelect 02h             ;Select BtDAC Cmd3 reg
	mov     dx,03c6h                ;
	in      al,dx                   ;
	and     al,11111000b            ;Set CR32 to 64x64x2 cursor mode and
	or      al,00000100b            ;reset 2 MSBs of 10 bits addr counter
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Select BtDAC address register
	mov     dx,03c8h                ;
	sub     al,al                   ;Reset address counter
	out     dx,al                   ;
	SetDACRegSelect 02h             ;Select BtDAC cursor data register
	mov     dx,03c7h                ;

	push    si
	add     si,128                  ;ds:si-->xor mask
;
;Copy XOR mask first
;
	mov     bl,32
@@:     mov     cx,4
	rep     outsb
	xor     al,al
	out     dx,al
	out     dx,al
	out     dx,al
	out     dx,al
	dec     bl
	jnz     @b
	mov     cx,256
@@:     out     dx,al
	dec     cx
	jnz     @B
	pop     si                      ;ds:si-->and mask
;
;Copy AND mask
;
	mov     bl,32
@@:     mov     cx,4
	rep     outsb
	mov     al,0FFh
	out     dx,al
	out     dx,al
	out     dx,al
	out     dx,al
	dec     bl
	jnz     @b
	mov     cx,256
@@:     out     dx,al
	dec     cx
	jnz     @B
;
;Set cursor color
;
	SetDACRegSelect 01h             ;Select cursor color addr reg
	mov     dx,03c8h                ;
	sub     al,al                   ;Set index to 0
	out     dx,al                   ;
	inc     dx                      ;DX points to 03c9h(color data port)
	out     dx,al                   ;Color 1 = 0 (this is not used)
	out     dx,al                   ;
	out     dx,al                   ;
	out     dx,al                   ;Color 2 = 0
	out     dx,al                   ;
	out     dx,al                   ;
	dec     al                      ;Color 3 = 0xffffff
	out     dx,al                   ;
	out     dx,al                   ;
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Reset DAC select
					;(also sets CRTC index reg to 30H)
	errnz $-MoveBtHardwareCursor    ;Fall into MoveBtHardwareCursor
LoadBtHardwareCursor    endp

;----------------------------------------------------------------------
; MoveBtHardwareCursor
;----------------------------------------------------------------------
PPROC   MoveBtHardwareCursor    near
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	mov     ecx,dword ptr CursorX   ;cx = CursorX
	shld    ebx,ecx,16              ;bx = CursorY
	add     cx,64
	add     bx,64
	sub     cx,XHot
	sub     bx,YHot
	SetDACRegSelect 03h
	mov     dx,3c8h                 ;Set X low
	mov     al,cl
	out     dx,al
	inc     dx                      ;Set X high
	mov     al,ch
	out     dx,al
	mov     dx,03c6h                ;Set Y low
	mov     al,bl
	out     dx,al
	inc     dx                      ;Set Y high
	mov     al,bh
	out     dx,al
	SetDACRegSelect 00h             ;Reset DAC select
					;(also sets CRTC index reg to 30H)
	ret
MoveBtHardwareCursor    endp

;----------------------------------------------------------------------
; TurnOff525HardwareCursor
;----------------------------------------------------------------------
PPROC   TurnOff525HardwareCursor near
	mov     dx,CRTC_REG             ;
	mov     ax,0a039h               ;Unlock S3 extension registers
	out     dx,ax                   ;
	SetDACRegSelect 01h             ;Select BtDAC cmd 2
	mov     ax,30h                  ;cursor control index
	mov     dx,3c8h                 ;
	out     dx,al                   ;
	inc     dx                      ;
	mov     al,ah                   ;
	out     dx,al                   ;
	mov     dx,3c6h                 ;
	mov     al,24h                  ;
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Reset DAC Select (also sets CRTC
					;index register back to 30H)
	ret
TurnOff525HardwareCursor endp

;----------------------------------------------------------------------
; TurnOn525HardwareCursor
;----------------------------------------------------------------------
PPROC   TurnOn525HardwareCursor  near
	mov     dx,CRTC_REG             ;
	mov     ax,0a039h               ;Unlock S3 extension registers
	out     dx,ax                   ;
	SetDACRegSelect 01h             ;Select BtDAC cmd 2
	mov     ax,30h                  ;cursor control index
	mov     dx,3c8h                 ;
	out     dx,al                   ;
	inc     dx                      ;
	mov     al,ah                   ;
	out     dx,al                   ;
	mov     dx,3c6h                 ;
	mov     al,26h                  ;
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Reset DAC Select
					;(also sets CRTC index register to 30H)
	ret
TurnOn525HardwareCursor  endp

;----------------------------------------------------------------------
; Set525HardwareCursor
;----------------------------------------------------------------------
PPROC   Set525HardwareCursor     near
	mov     ax,DGROUP
	mov     ds,ax
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	call    TurnOff525HardwareCursor
	call    Load525HardwareCursor
	call    TurnOn525HardwareCursor
	ret
Set525HardwareCursor     endp

;--------------------------------------------------------------------------
; Load525HardwareCursor
;--------------------------------------------------------------------------
PPROC   Load525HardwareCursor    near
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	lea     si,CursorSaveArea       ;ds:[si]-->cursor image in data segment
	mov     dx,CRTC_REG             ;unlock S3 extension registers
	mov     ax,0a039h
	out     dx,ax
	SetDACRegSelect 01h             ;Select BtDAC Cmd0 reg
;
	mov     dx,03c7h        ;DX point to Index control
	in      al,dx           ;read current status
	push    ax              ;save it in stack
	or      al,1            ;turn on auto-increment
	out     dx,al           ;

	mov     ax,0100h        ;cursor bitmap start index
	mov     dx,03c8h        ;index low port
	out     dx,al           ;
	inc     dx              ;index high port
	mov     al,ah           ;
	out     dx,al           ;

	mov     dx,03c6h        ;DX points to data port
	mov     cx,32           ;

@@:     mov     al,[si]         ;
	mov     ah,[si+128]     ;
	xor     bx,bx           ;reset result
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
;
	mov     al,bh           ;
	out     dx,al           ;
	mov     al,bl           ;
	out     dx,al           ;
;
	mov     al,[si+1]       ;
	mov     ah,[si+129]     ;
	xor     bx,bx           ;reset result
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
;
	mov     al,bh           ;
	out     dx,al           ;
	mov     al,bl           ;
	out     dx,al           ;
;
	mov     al,[si+2]       ;
	mov     ah,[si+130]     ;
	xor     bx,bx           ;reset result
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
;
	mov     al,bh           ;
	out     dx,al           ;
	mov     al,bl           ;
	out     dx,al           ;
;
	mov     al,[si+3]       ;
	mov     ah,[si+131]     ;
	xor     bx,bx           ;reset result
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
;
	mov     al,bh           ;
	out     dx,al           ;
	mov     al,bl           ;
	out     dx,al           ;
;
	mov     al,0aah         ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
;
	add     si,4            ;
	dec     cx              ;
	jnz     @B              ;
;
	mov     cx,32*16        ;
	mov     al,0aah         ;
@@:     out     dx,al           ;
	dec     cx              ;
	jnz     short @B        ;
;
	mov     ax,0040h        ;cursor color 1 index
	mov     dx,03c8h        ;index low port
	out     dx,al           ;
	inc     dx              ;index high port
	mov     al,ah           ;
	out     dx,al           ;

	mov     dx,03c6h        ;DX points to data port
	xor     ax,ax           ;set color to 0
	out     dx,al           ;red element
	out     dx,al           ;green element
	out     dx,al           ;blue element

	dec     ax              ;we are about to set color 2, make it white
	out     dx,al           ;red element
	out     dx,al           ;green element
	out     dx,al           ;blue element

	pop     ax              ;restore index control status
	mov     dx,03c7h        ;
	out     dx,al           ;
;
	SetDACRegSelect 00h             ;Reset DAC select
					;(also sets CRTC index reg to 30H)
	errnz $-Move525HardwareCursor   ;Fall into MoveBtHardwareCursor
Load525HardwareCursor    endp

;----------------------------------------------------------------------
; Move525HardwareCursor
;----------------------------------------------------------------------
PPROC   Move525HardwareCursor    near
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
;
	SetDACRegSelect 01h             ;Select BtDAC cmd 2
;
;Set cursor hot spot
;
	mov     ax,0035h        ;X hot spot register
	mov     dx,03c8h        ;
	out     dx,al           ;
	mov     al,ah           ;
	inc     dx              ;
	out     dx,al           ;
	mov     dx,03c6h        ;DX points data port
	mov     ax,XHot         ;
	out     dx,al           ;
;
	mov     ax,0036h        ;Y hot spot register
	mov     dx,03c8h        ;
	out     dx,al           ;
	mov     al,ah           ;
	inc     dx              ;
	out     dx,al           ;
	mov     dx,03c6h        ;DX points data port
	mov     ax,YHot         ;
	out     dx,al           ;
;
;Set the X and Y-coordinate:
;
	mov     ecx,dword ptr CursorX   ;cx = CursorX
	shld    ebx,ecx,16              ;bx = CursorY
;
	mov     ax,0031h        ;cursor X position register low
	mov     dx,03c8h        ;
	out     dx,al           ;
	mov     al,ah           ;
	inc     dx              ;
	out     dx,al           ;
	mov     dx,03c6h        ;DX points data port
	mov     al,cl           ;
	out     dx,al           ;
;
	mov     ax,0032h        ;cursor X position register high
	mov     dx,03c8h        ;
	out     dx,al           ;
	mov     al,ah           ;
	inc     dx              ;
	out     dx,al           ;
	mov     dx,03c6h        ;DX points data port
	mov     al,ch           ;
	out     dx,al           ;
;
	mov     ax,0033h        ;cursor Y position register low
	mov     dx,03c8h        ;
	out     dx,al           ;
	mov     al,ah           ;
	inc     dx              ;
	out     dx,al           ;
	mov     dx,03c6h        ;DX points data port
	mov     al,bl           ;
	out     dx,al           ;
;
	mov     ax,0034h        ;cursor Y position register high
	mov     dx,03c8h        ;
	out     dx,al           ;
	mov     al,ah           ;
	inc     dx              ;
	out     dx,al           ;
	mov     dx,03c6h        ;DX points data port
	mov     al,bh           ;
	out     dx,al           ;

	SetDACRegSelect 00h             ;Reset DAC select
					;(also sets CRTC index reg to 30H)
	ret
Move525HardwareCursor    endp

;----------------------------------------------------------------------
; TurnOffTi025HardwareCursor
;----------------------------------------------------------------------
PPROC   TurnOffTi025HardwareCursor near
	mov     dx,CRTC_REG             ;
	mov     ax,0a039h               ;Unlock S3 extension registers
	out     dx,ax                   ;
	SetDACRegSelect 01h             ;
	mov     dx,03c6h                ;Ti 3025 index port
	mov     al,6                    ;select index 6
	out     dx,al                   ;
	inc     dx                      ;Ti 3025 data port (3C7)
	in      al,dx                   ;
	and     al,10111111b            ;turn cursor off
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Reset DAC Select (also sets CRTC
					;index register back to 30H)
	ret
TurnOffTi025HardwareCursor endp

;----------------------------------------------------------------------
; TurnOnTi025HardwareCursor
;----------------------------------------------------------------------
PPROC   TurnOnTi025HardwareCursor  near
	mov     dx,CRTC_REG             ;
	mov     ax,0a039h               ;Unlock S3 extension registers
	out     dx,ax                   ;
	SetDACRegSelect 01h             ;
	mov     dx,03c6h                ;Ti 3025 index port
	mov     al,6                    ;select index 6
	out     dx,al                   ;
	inc     dx                      ;Ti 3025 data port (3C7)
	in      al,dx                   ;
	or      al,01000000b            ;turn cursor on
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Reset DAC Select
					;(also sets CRTC index register to 30H)
	ret
TurnOnTi025HardwareCursor  endp

;----------------------------------------------------------------------
; SetTi025HardwareCursor
;----------------------------------------------------------------------
PPROC   SetTi025HardwareCursor     near
	mov     ax,DGROUP
	mov     ds,ax
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	call    TurnOffTi025HardwareCursor
	call    LoadTi025HardwareCursor
	call    TurnOnTi025HardwareCursor
	ret
SetTi025HardwareCursor     endp

;--------------------------------------------------------------------------
; LoadTi025HardwareCursor
;--------------------------------------------------------------------------
PPROC   LoadTi025HardwareCursor    near
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	lea     si,CursorSaveArea       ;ds:[si]-->cursor image in data segment
	mov     dx,CRTC_REG             ;unlock S3 extension registers
	mov     ax,0a039h
	out     dx,ax
	SetDACRegSelect 01h             ;
;
	mov     dx,3c6h                 ;
	mov     al,8                    ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	xor     al,al                   ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,9                    ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	xor     al,al                   ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,10                   ;cursor RAM data port
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
;
	mov     cx,32                   ;
;
@@:     mov     al,[si]         ;
	mov     ah,[si+128]     ;
	xor     bx,bx           ;reset result
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
;
	mov     al,bh           ;
	out     dx,al           ;
	mov     al,bl           ;
	out     dx,al           ;
;
	mov     al,[si+1]       ;
	mov     ah,[si+129]     ;
	xor     bx,bx           ;reset result
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
;
	mov     al,bh           ;
	out     dx,al           ;
	mov     al,bl           ;
	out     dx,al           ;
;
	mov     al,[si+2]       ;
	mov     ah,[si+130]     ;
	xor     bx,bx           ;reset result
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
;
	mov     al,bh           ;
	out     dx,al           ;
	mov     al,bl           ;
	out     dx,al           ;
;
	mov     al,[si+3]       ;
	mov     ah,[si+131]     ;
	xor     bx,bx           ;reset result
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
	add     al,al           ;shift MS bit to carry
	adc     bx,bx           ;stuff the bit into bx and shift bx left
	add     ah,ah           ;
	adc     bx,bx           ;shift bx left 1 bit
;
	mov     al,bh           ;
	out     dx,al           ;
	mov     al,bl           ;
	out     dx,al           ;
;
	mov     al,0aah         ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
	out     dx,al           ;
;
	add     si,4            ;
	dec     cx              ;
	jnz     @B              ;
;
	mov     cx,32*16        ;
	mov     al,0aah         ;
@@:     out     dx,al           ;
	dec     cx              ;
	jnz     short @B        ;
;
	dec     dx              ;DX = 3C6
;
;Set cursor foreground and background colors.
;
	mov     al,26h                  ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	mov     al,0ffh                 ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,27h                  ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	mov     al,0ffh                 ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,28h                  ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	mov     al,0ffh                 ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,23h                  ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	xor     al,al                   ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,24h                  ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	xor     al,al                   ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,25h                  ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	xor     al,al                   ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
;
;Set X and Y sprite origin.
;
	mov     al,4                    ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	mov     ax,XHot                 ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,5                    ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	mov     ax,YHot                 ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
;
	SetDACRegSelect 00h             ;Reset DAC select
					;(also sets CRTC index reg to 30H)
	errnz $-MoveTi025HardwareCursor   ;Fall into MoveBtHardwareCursor
LoadTi025HardwareCursor    endp

;----------------------------------------------------------------------
; MoveTi025HardwareCursor
;----------------------------------------------------------------------
PPROC   MoveTi025HardwareCursor    near
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	mov     ecx,dword ptr CursorX   ;cx = CursorX
	shld    ebx,ecx,16              ;bx = CursorY
	SetDACRegSelect 01h             ;
	mov     dx,3c6h                 ;
	xor     al,al                   ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	mov     al,cl                   ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,1                    ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	mov     al,ch                   ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,2                    ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	mov     al,bl                   ;
	out     dx,al                   ;
	dec     dx                      ;DX = 3C6
	mov     al,3                    ;
	out     dx,al                   ;
	inc     dx                      ;DX = 3C7
	mov     al,bh                   ;
	out     dx,al                   ;
	SetDACRegSelect 00h             ;Reset DAC select
					;(also sets CRTC index reg to 30H)
	ret
MoveTi025HardwareCursor    endp

;----------------------------------------------------------------------
; ReloadCursor
;----------------------------------------------------------------------
PPROC   ReloadCursor    near
	mov     ax,DGROUP
	mov     es,ax
	assumes ds,Data
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
	push    RC_Done
	cmp     wDACType,DAC_ATT
	je      SetHardwareCursor
	cmp     wDACType,DAC_RGB525
	je      Set525HardwareCursor
	cmp     wDACType,DAC_TI025
	je      SetTi025HardwareCursor
	jmp     SetBtHardwareCursor
PLABEL RC_Done
	ret
ReloadCursor    endp

PPROC   ReloadCursor_far        far
        call    ReloadCursor
        ret
ReloadCursor_far        endp



;---------------------------------------------------------------------------
;When logical window is larger than physical (virtual) window, everytime
;move cursor is called, we have to
;	1- convert cursor coordinates from logical to physical size.
;	2- Set display start register to start at a new virtual
;	   window 's origin.
;---------------------------------------------------------------------------
cProc	MovePanningCursor,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmW	absX
	parmW	absY
cBegin
	mov	ax,DGROUP
	mov	es,ax
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
;
;Validate absX and absY coordinates
;
	mov	ax,absX 		;get X-coordinate
	mov	bx,absY 		;get Y-coordinate
	or	ax,ax			;exceeding low X-limit?
	jg	short @f		;nope, go on
	xor	ax,ax			;fix cursor position to 0
@@:
	mov	dx,wScreenWidth
	cmp	ax,dx			;exceeding high X-limit? (SCREEN_WIDTH)
	jb	short @f		;nope, go on
	dec	dx
	mov	ax,dx			;fix cursor position (SCREEN_WIDTH)
@@:
	or	bx,bx			;exceeding low Y-limit?
	jg	short @f		;nope, go on
	mov	bx,0			;fix cursor position
@@:
	mov	dx,wScreenHeight
	cmp	bx,dx			;exceeding high Y-limit? (SCREEN_HEIGHT)
	jb	short MPC_SetCurrentWindow
	dec	dx
	mov	bx,dx			;fix cursor position (SCREEN_HEIGHT-1)
;
MPC_SetCurrentWindow:
	test	S3MobileData.dwMobileCaps,PANNING_IGA1_IGA2
	jz	MPC_SaveLastUpperLeft
	test	S3MobileData.dwMobileCaps,VIDEO_ON_TV
	jnz	MPC_SaveLastUpperLeft
	mov	edi,dword ptr wVWTop1
	mov	dword ptr wVWTop,edi
	mov	edi,dword ptr wVWBottom1
	mov	dword ptr wVWBottom,edi
;
;Update coordinates of virtual window and convert absX and absY into virtual
;window 's coordinates.
;ax	absX
;bx	abxY
;
MPC_SaveLastUpperLeft:
	mov	cx,wVWLeft
	mov	dx,wVWTop
	mov	absX,ax
	mov	absY,bx
;
MPC_CheckVWLeft:
	cmp	ax,cx
	ja	short MPC_CheckVWRight	;may have to move VW forward
	sub	ax,cx			;amount to move backward
	add	wVWLeft,ax		;adjust Virtual Window left, right
	add	wVWRight,ax
	jmp	short MPC_CheckVWTop
;
MPC_CheckVWRight:
	cmp	ax,wVWRight
	jb	MPC_CheckVWTop
	sub	ax,wVWRight
	add	wVWRight,ax		;move virtual window forward
	add	wVWLeft,ax
;
MPC_CheckVWTop:
	cmp	bx,dx
	ja	short MPC_CheckVWBottom ;may have to move VW down
	sub	bx,dx
	add	wVWTop,bx		;move virtual window up
	add	wVWBottom,bx
	jmp	short MPC_AdjustCursorCoords
;
MPC_CheckVWBottom:
	cmp	bx,wVWBottom
	jb	short MPC_AdjustCursorCoords
	sub	bx,wVWBottom
	add	wVWBottom,bx		;move virtual window down
	add	wVWTop,bx
;
public MPC_AdjustCursorCoords
MPC_AdjustCursorCoords:
	mov	ax,absY
	sub	ax,wVWTop		;convert to virtual window 's coordinate
	jnz	short @f
	inc	ax
@@:	shl	eax,16
	mov	ax,absX
	sub	ax,wVWLeft
	mov	dword ptr UndoneX,eax
	cmp	cx,wVWLeft
	jne	short MPC_CheckPanningOn2Windows
	cmp	dx,wVWTop
	jne	short MPC_CheckPanningOn2Windows
;
	test	S3MobileData.dwMobileCaps,PANNING_IGA1_IGA2
	jz	MPC_CheckCursorType
	test	S3MobileData.dwMobileCaps,VIDEO_ON_TV
	jnz	MPC_CheckCursorType
	mov	edi,dword ptr wVWTop2
	mov	dword ptr wVWTop,edi
	mov	edi,dword ptr wVWBottom2
	mov	dword ptr wVWBottom,edi
	jmp	MPC_CheckCursorType
;
MPC_CheckPanningOn2Windows:
	test	S3MobileData.dwMobileCaps,PANNING_IGA1_IGA2
	jz	MPC_CalcWindowOffset
	mov	edi,dword ptr wVWTop
	mov	dword ptr wVWTop1,edi
	mov	edi,dword ptr wVWBottom
	mov	dword ptr wVWBottom1,edi
;
MPC_CalcWindow2Offset:
	mov	cx,wVWLeft2
	mov	dx,wVWTop2
	mov	ax,absX
	mov	bx,absY
;
MPC_CheckVWLeft2:
	cmp	ax,cx
	ja	short MPC_CheckVWRight2	;may have to move VW forward
	sub	ax,cx			;amount to move backward
	add	wVWLeft2,ax		;adjust Virtual Window left, right
	add	wVWRight2,ax
	jmp	short MPC_CheckVWTop2
;
MPC_CheckVWRight2:
	cmp	ax,wVWRight2
	jb	MPC_CheckVWTop2
	sub	ax,wVWRight2
	add	wVWRight2,ax		;move virtual window forward
	add	wVWLeft2,ax
;
MPC_CheckVWTop2:
	cmp	bx,dx
	ja	short MPC_CheckVWBottom2
	sub	bx,dx
	add	wVWTop2,bx		;move virtual window up
	add	wVWBottom2,bx
	jmp	MPC_NewVWXY2
;
MPC_CheckVWBottom2:
	cmp	bx,wVWBottom2
	jb	short MPC_NewVWXY2
	sub	bx,wVWBottom2
	add	wVWBottom2,bx		;move virtual window down
	add	wVWTop2,bx
;
MPC_NewVWXY2:
	cmp	cx,wVWLeft2
	jne	short MPC_CalcOffset2
	cmp	dx,wVWTop2
	je	MPC_CalcWindowOffset
;
public MPC_CalcOffset2
MPC_CalcOffset2:
	xor	eax,eax
	mov	ebx,eax
	mov	edx,eax
	mov	bx,wVWLeft2
	cmp	wBpp,24
	jne	short @f
	mov	ax,bx			;eax = left offset in pels
	shl	ebx,1
	add	ebx,eax 		;offset in bytes
	and	ax,7			;pixel boundary = qword boundary
	mov	cx,ax
	shl	ax,1
	add	ax,cx
	sub	ebx,eax 		;offset after allignment
	mov	cx,3
	idiv	cx			;convert back to number of pixels
	sub	wVWLeft2,ax		;adjust left offset
	sub	wVWRight2,ax
	jmp	short MPC_SetOffset2
;
@@:	mov	cl,bShiftPelsToBytes
	shl	ebx,cl
	mov	edx,ebx 		;edx = left offset in bytes
	and	edx,7
	sub	ebx,edx 		;qword alligned
	mov	cl,bShiftPelsToBytes
	shr	edx,cl
	sub	wVWLeft2,dx		;adjust left offset
	sub	wVWRight2,dx
;
MPC_SetOffset2:
	mov	dx,wVWTop2
	mov	ax,wScreenWidthBytes
	mul	edx
	add	eax,ebx
        mov     dwPSAddr,eax
	add	eax,dwFlipOffset
	shr	eax,2
	mov	ebx,eax
	mov	SavedDispStart2,eax
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
;
public MPC_CalcWindowOffset
MPC_CalcWindowOffset:
	xor	eax,eax
	mov	ebx,eax
	mov	edx,eax
	mov	bx,wVWLeft
	cmp	wBpp,24
	jne	short @f
	mov	ax,bx			;eax = left offset in pels
	shl	ebx,1
	add	ebx,eax 		;offset in bytes
	and	ax,7			;pixel boundary = qword boundary
	mov	cx,ax
	shl	ax,1
	add	ax,cx
	sub	ebx,eax 		;offset after allignment
	mov	cx,3
	idiv	cx			;convert back to number of pixels
	sub	wVWLeft,ax		;adjust left offset
	sub	wVWRight,ax
	jmp	short MPC_SetDispStartAddr
;
@@:	mov	cl,bShiftPelsToBytes
	shl	ebx,cl
	mov	edx,ebx 		;edx = left offset in bytes
	and	edx,7
	sub	ebx,edx 		;qword alligned
	mov	cl,bShiftPelsToBytes
	shr	edx,cl
	sub	wVWLeft,dx		;adjust left offset
	sub	wVWRight,dx
;
MPC_SetDispStartAddr:
	mov	dx,wVWTop
	mov	ax,wScreenWidthBytes
	mul	edx
	add	eax,ebx
	test	S3MobileData.dwMobileCaps,PANNING_IGA1_IGA2
	jz	short @F
        mov     dwPSAddr2,eax
	jmp	short MPC_SDS
@@:
        mov     dwPSAddr,eax
MPC_SDS:
	add	eax,dwFlipOffset
	shr	eax,2
	mov	ebx,eax
	mov	SavedDispStart,eax
        test    S3MobileData.dwMobileCaps,PANNING_IGA1
	jnz	short @F
	mov	dx,SEQ_REG
        RD_IGA2_WR_IGA2
@@:
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
;
	test	S3MobileData.dwMobileCaps,PANNING_IGA1_IGA2
	jz	MPC_HardwareOverlay
	test	S3MobileData.dwMobileCaps,VIDEO_ON_TV
	jnz	MPC_HardwareOverlay
	mov	edi,dword ptr wVWTop2
	mov	dword ptr wVWTop,edi
	mov	edi,dword ptr wVWBottom2
	mov	dword ptr wVWBottom,edi
;
MPC_HardwareOverlay:
	test	S3MobileData.dwMobileCaps,SP_ON
	jz	MPC_CheckCursorType
	mov	eax,S3MobileData.dwMobileCaps
	test	eax,PANNING_IGA1_IGA2
	jnz	MPC_CheckSSWindow
        and     eax,(PANNING_IGA1 or SP_IGA1)
	jz	MPC_CheckSSWindow
        cmp     eax,(PANNING_IGA1 or SP_IGA1)
	je	MPC_CheckSSWindow
	mov	dwFlipOffset,0
	jmp	MPC_CheckCursorType
;
MPC_CheckSSWindow:
	mov	ax,wMMIOSelector
	mov	fs,ax
	assumes fs,nothing

        test    S3MobileData.dwMobileCaps,HIDE_OVERLAY
        jnz     MPC_CheckCursorType
	mov	ebx,dwSSXY		;original x:y
	mov	edi,ebx
	mov	esi,dwSSWH		;original width:height
	xor	eax,eax
	mov	edx,eax
	mov	ecx,eax
	ror	ebx,16			;SS_X
	sub	bx,word ptr es:[wVWLeft]
	js	short MPC_SSClippedX
	ror	ebx,16			;newX:orgY
	mov	edi,ebx
	sub	bx,word ptr es:[wVWTop]
	js	MPC_CalcNewYParms
;
public MPC_SSWindowNotClipped
MPC_SSWindowNotClipped:
	mov	fs:[SS_XY],ebx
	mov	eax,dwSSAddr		;original SS 's address
	cmp	eax,fs:[SS_ADDR]
	je	MPC_CheckCursorType
	mov	fs:[SS_ADDR],eax
	mov	eax,dwSSWH
	mov	fs:[SS_WH],eax
	mov	dwClipOffset,0
;
MPC_CheckBlendCtrol:
	cmp	bShowSSWindow,0 	;do we have to set blend control reg ?
	je	MPC_CheckCursorType	;nop, almost done
	mov	eax,dwBlendCtrl
	mov	fs:[SP_BLENDCTRL],eax
	inc	bShowSSWindow		;clear flag
	jmp	MPC_CheckCursorType
;
public MPC_SSClippedX
MPC_SSClippedX:
	neg	bx			;clipped_x
	ror	esi,16			;original width
	sub	si,bx			;new width
	jge	short MPC_NewSSWidth
	xor	esi,esi

MPC_NewSSWidth:
	ror	esi,16
	shl	edi,16
	mov	di,1			;new starting x
	ror	edi,16
	mov	ax,bx			;x adjustment
	mov	cl,bShiftPelsToBytes
	shl	eax,cl
	cmp	wBpp,24
	jne	short MPC_CheckSrcWidth
	add	ax,bx

;
;eax	x offset
;
public MPC_CheckSrcWidth
MPC_CheckSrcWidth:
	mov	edx,dwSrcWH
	shr	edx,16
	mov	ecx,dwSSWH
	shr	ecx,16
	inc	cx			;Secondary stream width
	cmp	dx,cx
	je	short MPC_CheckY
	mul	edx
	xor	edx,edx
	idiv	ecx

;eax	x offset
;ebx	clipped_x:ori y
;edi	new x:org.y
;esi	new width: ori. height
;
public MPC_CheckY
MPC_CheckY:
	ror	ebx,16			;clipped_x:org y
	sub	bx,word ptr es:[wVWTop]
	js	short MPC_CalcNewYParms
	mov	di,bx
	mov	ebx,eax 		;SS offset
	xor	eax,eax
	jmp	short MPC_LinesToBytes
;
MPC_CalcNewYParms:
	mov	di,1			;new y
	neg	bx
	sub	si,bx			;new height
	jge	MPC_CalcYOffset
	xor	esi,esi
;
MPC_CalcYOffset:
	xchg	eax,ebx 		;eax = y offset, ebx = x offset
	shl	eax,16
	shr	eax,16
	mov	dx,word ptr dwSrcWH
	mov	cx,word ptr dwSSWH
	cmp	dx,cx
	je	short MPC_LinesToBytes
	mul	edx
	xor	edx,edx
	idiv	ecx			;eax = number of scanlines to skip

;ebx	x offset
;eax	y offset
;
public MPC_LinesToBytes
MPC_LinesToBytes:
	mov	edx,fs:[SS_STRIDE]
	and	edx,0fffh
	mul	edx
	mov	edx,fs:[SS_CONTROL]
	shr	edx,SSC_INPUT_DATA_POS
	and	edx,SSC_INPUT_DATA_MASK
	mov	cl,1			;assume 2-byte format
	cmp	dl,SSC_TWO_BYTE_FORMAT
	jbe	short MPC_CalcPSColorDepth
	inc	cl
	cmp	dl,SSC_FOUR_BYTE_FORMAT
	je	short MPC_CalcPSColorDepth
	mov	edx,ebx
	shl	ebx,1
	add	ebx,edx
	jmp	short MPC_AddYOffset
;
MPC_CalcPSColorDepth:
	cmp	wBpp,8
	je	short MPC_XOffsetInBytes
	shr	cl,1
	cmp	wBpp,16
	je	short MPC_XOffsetInBytes
	shl	ebx,1
	cmp	cl,0
	je	short MPC_ConvertToPS24bpp
	shl	ebx,1
;
public MPC_ConvertToPS24bpp
MPC_ConvertToPS24bpp:
	mov	ecx,eax
	mov	eax,ebx
	xor	edx,edx
	mov	ebx,3
	idiv	ebx
	add	eax,ecx
	jmp	short MPC_CheckAllignment
;
MPC_XOffsetInBytes:
	shl	ebx,cl
;
MPC_AddYOffset:
	add	eax,ebx 		;SS offset

MPC_CheckAllignment:
	cmp	eax,7
	jg	short MPC_QuadWordAllignment
	xor	ebx,ebx
	jmp	short MPC_SetNewSSParms

MPC_QuadWordAllignment:
	mov	ebx,eax
	and	eax,7
	sub	ebx,eax 		;quadword boundary

;
;ebx	offset to SS buffer address
;edi	newX : newY
;esi	new width: new height
;
MPC_SetNewSSParms:
	mov	dwClipOffset,ebx
	add	ebx,dwSSAddr
	mov	fs:[SS_ADDR],ebx
	mov	fs:[SS_XY],edi
	test	dword ptr fs:[8184h],60000000h  ; Check if window keying
        jnz     MPC_SetVWindowSize      ; No, skip the following
        or      si,si                   ; If window keying and v.size = 0?
        jnz     SHORT MPC_SetVWindowSize; No, no need to hide stream
	mov	dword ptr fs:[SP_BLENDCTRL],2000h ; Hide the s.stream
	dec	bShowSSWindow           ; Set flag to indicate stream no shown
	jmp	MPC_CheckCursorType     ; We're done
;
MPC_SetVWindowSize:
	mov	fs:[SS_WH],esi
	cmp	bShowSSWindow,0 	;do we have to set blend control reg ?
	je	MPC_CheckCursorType	;nop, almost done
	mov	eax,dwBlendCtrl
	mov	fs:[SP_BLENDCTRL],eax
	inc	bShowSSWindow		;clear flag

;
; Let DIB engine setting x,y coordinates if wCursorType = software
;
MPC_CheckCursorType:
	cmp	wCursorType,SOFTWARECURSOR
	jne	short MPC_UndoneLoop
	pop	edi
	pop	esi
	mov	bx,bp
	mov	sp,bx			;restore stack frame
	pop	bp
	pop	ecx
	push	lpDriverPDevice
	push	ecx
	jmp	DIB_MoveCursorExt

MPC_UndoneLoop:
	mov	eax,dword ptr UndoneX
	mov	dword ptr CursorX,eax
	push	MPC_Exit
	jmp	MoveHardwareCursor

PLABEL MPC_Exit
	mov	eax,dword ptr UndoneX
	cmp	eax,dword ptr CursorX
	jne	MPC_UndoneLoop
PLABEL MPC_Ret
cEnd


;
;---------------------------------------------------------------------------
;MoveDualImgPanningCursor
;---------------------------------------------------------------------------
cProc	MoveDualImgPanningCursor,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmW	absX
	parmW	absY
cBegin
	mov	ax,DGROUP
	mov	es,ax
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing
cEnd
;
;
;---------------------------------------------------------------------------
;MoveCenteringCursor
;---------------------------------------------------------------------------
cProc	MoveCenteringCursor,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmW	absX
	parmW	absY
cBegin
	mov	ax,DGROUP
	mov	es,ax
	assumes es,Data
;
;Validate absX and absY coordinates
;
	mov	ax,absX
	mov	bx,absY 		;bx = y coordinate
	or	ax,ax			;exceeding low X-limit?
	jg	short @f		;nope, go on
	xor	ax,ax			;fix cursor position to 0
@@:	mov	dx,wScreenWidth
	cmp	ax,dx			;exceeding high X-limit? (SCREEN_WIDTH)
	jb	short @f		;nope, go on
	dec	dx
	mov	ax,dx			;fix cursor position (SCREEN_WIDTH)
@@:	or	bx,bx			;exceeding low Y-limit?
	jg	short @f		;nope, go on
	mov	bx,1			;fix cursor position
@@:	mov	dx,wScreenHeight
	cmp	bx,dx			;exceeding high Y-limit? (SCREEN_HEIGHT)
	jb	short MCC_AddCenteringOffset
	dec	dx
	mov	bx,dx			;fix cursor position (SCREEN_HEIGHT-1)
;
;ax	absX
;bx	abxY
;
public MCC_AddCenteringOffset
MCC_AddCenteringOffset:
	add	bx,wVWTop
	shl	ebx,16
	mov	bx,ax
	add	bx,wVWLeft
	mov	dword ptr UndoneX,ebx
;
; Let DIB engine setting x,y coordinates if wCursorType = software
;
MCC_CheckCursorType:
	cmp	wCursorType,SOFTWARECURSOR
	jne	short MCC_UndoneLoop
	pop	edi
	pop	esi
	mov	bx,bp
	mov	sp,bx			;restore stack frame
	pop	bp
	pop	ecx
	push	lpDriverPDevice
	push	ecx
	jmp	DIB_MoveCursorExt
;
MCC_UndoneLoop:
	mov	eax, dword ptr UndoneX
	mov	dword ptr CursorX,eax
	push	MCC_Exit
	jmp	MoveHardwareCursor

PLABEL MCC_Exit
	mov	eax,dword ptr UndoneX
	cmp	eax,dword ptr CursorX
	jne	MCC_UndoneLoop
cEnd
;
;
;---------------------------------------------------------------------------
;MoveDualImgCursor
;---------------------------------------------------------------------------
cProc	MoveDualImgCursor,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmW	absX
	parmW	absY
cBegin
	assumes ds,nothing
	assumes es,Data
cEnd
;
;
;-------------------------------------------------------------------
;Set Primary Stream address register to the origin of virtual window,
;(wVWTop, wVWLeft)
;
;-------------------------------------------------------------------
cProc	SetPSAddress,<PUBLIC,FAR,PASCAL,NODATA>
cBegin
.386
	mov	ax,DGROUP
	mov	es,ax
	assumes ds,nothing
	assumes es,Data
	mov	ax,wMMIOSelector
	mov	fs,ax
	assumes fs,nothing
	xor	eax,eax
	mov	ebx,eax
	mov	edx,eax
	mov	bx,wVWLeft
	cmp	wBpp,24
	jne	short @f
	mov	ax,bx			;edx = left offset in pels
	shl	ebx,1
	add	ebx,eax 		;offset in bytes
	and	ax,7			;pixel boundary = qword boundary
	mov	cx,ax
	shl	ax,1
	add	ax,cx
	sub	ebx,eax 		;offset after allignment
	mov	cx,3
	idiv	cx			;convert back to number of pixels
	sub	wVWLeft,ax		;adjust left offset
	sub	UndoneX,ax
	jmp	short SPA_SetPSWindow
;
@@:	mov	cl,bShiftPelsToBytes
	shl	ebx,cl
	mov	edx,ebx 		;edx = left offset in bytes
	and	edx,7
	sub	ebx,edx 		;qword alligned
	mov	cl,bShiftPelsToBytes
	shr	edx,cl
	sub	wVWLeft,dx		;adjust left offset
	sub	UndoneX,dx
;
SPA_SetPSWindow:
	mov	dx,wVWTop
	mov	ax,wScreenWidthBytes
	mul	edx
	add	eax,ebx
	mov	fs:[PS_ADDR],eax	;program new primary stream address
        mov     dwPSAddr,eax
cEnd


;-------------------------------------------------------------------
;Set display start address register to the origin of virtual window,
;(wVWTop, wVWLeft)
;
;-------------------------------------------------------------------
cProc	SetDispStart,<PUBLIC,FAR,PASCAL,NODATA>
cBegin
.386
	mov	ax,DGROUP
	mov	es,ax
	assumes ds,nothing
	assumes es,Data
	assumes fs,nothing
	assumes gs,nothing

	xor	eax,eax
	mov	ebx,eax
	mov	bx,wVWTop
	mov	ax,wScreenWidthBytes
	mul	ebx
	shr	eax,2			;convert to number of double words
	mov	bx,wVWLeft
	cmp	wBpp,24
	jne	short @f
	mov	cx,bx
	shl	bx,1
	add	bx,cx			;pitch in bytes
	and	cx,3			;ensure pixel boundary = dword boundary
	mov	dx,cx
	shl	cx,1
	add	cx,dx
	sub	bx,cx
	shr	bx,2			;pitch in double words
	jmp	short SDS_SetDisplayStart
;
@@:	mov	cl,bShiftPelsToDWords
	shr	bx,cl
;
SDS_SetDisplayStart:
	add	ebx,eax
;pass info to DDRAW
	mov	eax,ebx
	shl	eax,2
	mov     dwPSAddr,eax
        test    S3MobileData.dwMobileCaps,PANNING_IGA1
	jnz	short SDS_SetDSRegs
	mov	dx,SEQ_REG
        RD_IGA2_WR_IGA2
;
SDS_SetDSRegs:
        mov     SavedDispStart,ebx
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
        test    S3MobileData.dwMobileCaps,PANNING_IGA1
	jnz	short SDS_Done
	mov	dx,SEQ_REG
        RD_IGA1_WR_IGA1
;
SDS_Done:
cEnd

cProc	SaveCursorBitmap,<NEAR,PUBLIC>
cBegin
	assumes es,Data
	cmp     [bp+8],0    		;seg_lpCursorShape
	jne	SCB_SaveCursorInfo
	lea     di,CursorSaveArea       ;ES:DI --> CursorSaveArea
	xor	eax,eax
	dec	eax
	mov     cx,32	                ;1 dword/row, 32 rows -- AND Mask
	rep     stosd
	xor	eax,eax
	mov     cx,32	                ;1 dword/row, 32 rows -- XOR Mask
	rep     stosd
	jmp	SCB_Done
;
SCB_SaveCursorInfo:
	lds	si,[bp+6]
	lea	di,XHot			;es:di -> XHot
	mov	ecx,67			;cursor info + 2 masks
	rep     movsd
;
	test	S3MobileData.dwMobileCaps,SP_ON
	jz	SCB_Done
	cmp	CursorFmt,MONO_CURSOR
	jne	SCB_Done		;color cursor done
	cmp	wCursorType,HARDWARECURSOR
	je	SBC_CheckXparentBitmap
	test	S3MobileData.dwMobileCaps,FIX_XPARENT_CURSOR
	jz	SCB_Done		;REAL software cursor, done
;
SBC_CheckXparentBitmap:
	lea	si,CursorSaveArea	;-> AND mask
	lea	di,XparentANDMask 	;-> transparent and mask
	mov	ax,es
	mov	ds,ax
	assumes	ds,Data
	mov	ecx,32
	repe	cmpsd
	cmp	wCursorType,HARDWARECURSOR
	je	short SCB_Switch2SWCursor
	cmp	ecx,0
	je	SCB_Done
	and	S3MobileData.dwMobileCaps,not FIX_XPARENT_CURSOR
	mov	wCursorType,HARDWARECURSOR
 	SwitchToHWCursor
	jmp	short SCB_Done
;
SCB_Switch2SWCursor:
	cmp	ecx,0
	jne	short SCB_Done
	or	S3MobileData.dwMobileCaps,FIX_XPARENT_CURSOR
	mov	wCursorType,SOFTWARECURSOR
	SwitchToSWCursor
;
SCB_Done:
cEnd

cProc	Switch2HWCursor,<PUBLIC,FAR,PASCAL,NODATA>,<edi,esi>
cBegin
	push	es
	mov	ax,DGROUP
	mov	es,ax
	assumes es,Data
	and	es:S3MobileData.dwMobileCaps,not FIX_XPARENT_CURSOR
	SwitchToHWCursor
	pop	es
cEnd

sEnd            Code
end
