;----------------------------------------------------------------------------
; DIAG.ASM
; Copyright (c) 1994 Microsoft Corporation
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
	.xlist
	include cmacros.inc
	include macros.inc
	.list

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP        DiagOutput

;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------
DIAGSTRING      macro   s
dw @f-$-2
db s,13,10,0
@@:
endm

GLOBALSTRING    macro   s,v,t
dw @f-$-2
db s
@@:
dw v
dw t
endm

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
globalW Diag0,0

;;externB bMMIO
externB bPolygonFlag
;;externB bPolylineFlag
externB bEllipseFlag
externB bRectangleFlag
externB bTextOutFlag
externB bBlockedMode
;mv externB b16BitDAC
externW wBpp
externW wChipId
externW wChipRev
externW wDeviceId
externW wCursorType
externW wBusType
externW SWCursor
externW wCurrentMode
externD dwVideoMemorySize
externD CursorCacheAddr
externW wScreenWidth
externW wScreenHeight
externW wMaxWidth
externW wMaxHeight
externW wScreenWidthBytes


sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
	.386
PLABEL DiagStrings
DIAGSTRING "s3.drv: S3 hardware not found"
DIAGSTRING "s3.drv: Disabling MMIO due to chip id/rev"
DIAGSTRING "s3.drv: Nibble readback test failed"
DIAGSTRING "s3.drv: Bad Screen Selector"
DIAGSTRING "s3.drv: VDD call ok"
DIAGSTRING "s3.drv: CheckCursorTimer call ok"
DIAGSTRING "s3.drv: No Modes found"
DIAGSTRING "s3.drv: Trying 24bpp modes"
DIAGSTRING "s3.drv: Screen width is negative"
DIAGSTRING "s3.drv: Screen height is negative"
DIAGSTRING "s3.drv: Int 10 mode init failed"
DIAGSTRING "s3.drv: GSV zero path taken"
dw      0                               ;sentinal


PLABEL GlobalStrings
;mvGLOBALSTRING "s3.drv: bMMIO=",                  bMMIO,                  0
GLOBALSTRING "s3.drv: bPolygonFlag=",           bPolygonFlag,           0
;mvGLOBALSTRING "s3.drv: bPolylineFlag=",          bPolylineFlag,          0
GLOBALSTRING "s3.drv: bEllipseFlag=",           bEllipseFlag,           0
GLOBALSTRING "s3.drv: bRectangleFlag=",         bRectangleFlag,         0
GLOBALSTRING "s3.drv: bTextOutFlag=",           bTextOutFlag,           0
GLOBALSTRING "s3.drv: bBlockedMode=",           bBlockedMode,           0
GLOBALSTRING "s3.drv: wBpp=",                   wBpp,                   1
GLOBALSTRING "s3.drv: wChipId=",                wChipId,                1
GLOBALSTRING "s3.drv: wChipRev=",               wChipRev,               1
GLOBALSTRING "s3.drv: wDeviceId=",              wDeviceId,              1
GLOBALSTRING "s3.drv: wCursorType=",            wCursorType,            1
;mv GLOBALSTRING "s3.drv: wDACType=",               wDACType,               1
GLOBALSTRING "s3.drv: wBusType=",               wBusType,               1
GLOBALSTRING "s3.drv: SWCursor=",               SWCursor,               1
GLOBALSTRING "s3.drv: wCurrentMode=",           wCurrentMode,           1

GLOBALSTRING "s3.drv: wScreenWidth=",           wScreenWidth,           1
GLOBALSTRING "s3.drv: wScreenHeight=",          wScreenHeight,          1
GLOBALSTRING "s3.drv: wMaxWidth=",              wMaxWidth,              1
GLOBALSTRING "s3.drv: wMaxHeight=",             wMaxHeight,             1
GLOBALSTRING "s3.drv: wScreenWidthBytes=",      wScreenWidthBytes,      1

GLOBALSTRING "s3.drv: dwVideoMemorySize=",      dwVideoMemorySize,      3
dw      0


;----------------------------------------------------------------------------
; WriteDiagInfo
; Write out trace flags to bootlog.txt.
;----------------------------------------------------------------------------
PPROC   WriteDiagInfo   near
	assumes ds,Data
	assumes es,nothing
	assumes fs,nothing
	assumes gs,nothing
	mov     ax,Diag0
	lea     si,DiagStrings
	xor     di,di
@@:     add     si,di
	mov     di,word ptr cs:[si]
	test    di,di
	jz      WDI_Exit
	add     si,2
	shr     ax,1
	jnc     @b
	push    ax
	cCall   DiagOutput,<cs,si>
	pop     ax
	jmp     @b


PLABEL  WDI_Exit
	ret
WriteDiagInfo   endp

;----------------------------------------------------------------------------
; WriteGlobalInfo
; Write out globals to bootlog.txt.
;----------------------------------------------------------------------------
PPROC   WriteGlobalInfo near
	assumes ds,Data
	assumes es,nothing
	assumes fs,nothing
	assumes gs,nothing
	sub     sp,50
	mov     di,sp
	mov     ax,ss
	mov     es,ax                   ;es:di = temp buffer
	lea     si,GlobalStrings

PLABEL WGI_Loop
	mov     cx,word ptr cs:[si]
	test    cx,cx
	jz      WGI_Exit
	push    di
	add     si,2
	rep     movs byte ptr es:[di], cs:[si]
	mov     bx,cs:[si]              ;bx = address of variable
	add     si,2                    ;advance to variable type
	push    si
	mov     si,cs:[si]              ;get offset to 1st byte of variable
@@:     mov     al,[bx][si]             ;get value of 1st variable byte
	call    itoa                    ;convert to ascii and store into buffer
	dec     si                      ;do for each byte in variable
	jns     @b
	pop     si
	add     si,2                    ;advance to next table entry.
	mov     es:[di],0a0dh
	mov     byte ptr es:[di+2],0
	pop     di
	push    es
	cCall   DiagOutput,<ss,di>
	pop     es
	jmp     WGI_Loop

PLABEL WGI_Exit
	add     sp,50
	ret

WriteGlobalInfo endp



;----------------------------------------------------------------------------
; itoa
; Entry:
;   al = byte to convert
;   es:di-->destination string buffer
; Exit:
;   es:di-->advanced by 2 bytes
;----------------------------------------------------------------------------
PPROC   itoa    near
	mov     ch,2
	mov     ah,al
	shr     al,4

PLABEL ITA_Loop
	mov     cl,'0'
	cmp     al,9
	jle     short @f
	mov     cl,'A'-0Ah
@@:     add     al,cl
	stosb
	and     ah,0fh
	mov     al,ah
	dec     ch
	jnz     ITA_Loop
	ret
itoa    endp

sEnd    InitSeg
end
