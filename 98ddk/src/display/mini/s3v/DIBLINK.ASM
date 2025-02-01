;------------------------------------------------------------------------------
; DIBLINK.ASM
; Copyright (c) 1994 Microsoft Corporation
; Copyright (c) 1996 S3 Incorporated
;------------------------------------------------------------------------------
;
.xlist
;
DOS5 = 1                                ; DOS5 or later, disable odd stack align
include cmacros.inc                     ;
include macros.inc                      ; (For PLABEL macro)
include dibeng.inc
include s3.inc                          ;

;------------------------------------------------------------------------------
; Local Macros
;
;----------------------------------------------------------------------------

DIBLINK     MACRO   name,target,extra
externFP    target
public      name

        ALIGN   16
name:
        assumes	ds,nothing
        assumes	es,nothing
        assumes	fs,nothing
        assumes	gs,nothing

IFNB    <extra> ;-----------------------
        mov     ax,DGROUP               ; AX = driver data selector
        mov     es,ax                   ; ES: = driver data selector
                                        
        assumes	es,Data

        pop     ecx                     ; save ECX = caller far return address
        push    extra                   ; save driver extra input parameter
        push    ecx                     ; save ECX = caller far return address
ENDIF ; IFNB <extra> ;------------------
        jmp     target                  ; jump to target DIBEngine function

ENDM ;  DIBLINK


;------------------------------------------------------------------------------
; DIBLINKIDLE: Wait for engine idle before jumping to the DIBEngine handler
;------------------------------------------------------------------------------

DIBLINKIDLE     MACRO   name,target,extra
externFP    target
public	    name

    	ALIGN	16
name:
    	mov     ax,DGROUP               ; AX = driver data selector
        mov     gs,ax                   ; GS: = driver data selector

IFNB	<extra> ;-----------------------

	assumes gs,Data

        pop     ecx                     ; save ECX = caller far return address
        push    extra                   ; save driver extra input parameter
        push    ecx                     ; save ECX = caller far return address

ENDIF ; IFNB <extra> ;------------------

        push    (SEG    target)         ; save destination code segment
        push    (OFFSET target)         ; save destination code
        jmp     DL_WaitForIdle          ; goto common wait for engine idle code

ENDM ;	DIBLINKIDLE

.list
;------------------------------------------------------------------------------
; DIBLINKDEV: Wait for engine idle before jumping to the DIBEngine handler
;------------------------------------------------------------------------------

DIBLINKDEV      MACRO   name,pd,target,extra
local	NotDev
externFP target
public   name

	ALIGN 16
name:
	assumes	ds,nothing
	assumes	es,nothing
	assumes	fs,nothing
	assumes	gs,nothing

	mov	bx,sp
	lfs	bx,ss:[bx+pd]
	mov	ax,fs:[bx.deFlags]
	test	ax,VRAM+OFFSCREEN
	jnz	NotDev	
	test	fs:[bx.dxBitmapFlags],CACHEABLE
	jz	NotDev
	and	fs:[bx.dxBitmapFlags],NOT CACHEABLE
		
NotDev:
IFNB    <extra> ;-----------------------
	mov	ax,DGROUP
	mov	es,ax
	assumes	es,Data
	pop	ecx
	push	extra
	push	ecx
ENDIF 	; IFNB <extra> ;----------------
	jmp	target

ENDM	; DIBLINKDEV



;------------------------------------------------------------------------------
; DIBLINKIDLEDEV: Wait for engine idle before jumping to the DIBEngine handler
;------------------------------------------------------------------------------

DIBLINKIDLEDEV     MACRO   name,pd,target,extra
local	NotDev
externFP    target
public	    name

    	ALIGN	16
name:
	assumes	ds,nothing
	assumes	es,nothing
	assumes	fs,nothing
	assumes	gs,nothing

	mov	bx,sp
	lfs	bx,ss:[bx+pd]
	mov	ax,fs:[bx.deFlags]
	test	ax,VRAM+OFFSCREEN
	jnz	NotDev	
	test	ax,CACHEABLE
	jz	NotDev
	and	fs:[bx.dxBitmapFlags],NOT CACHEABLE
		
NotDev:
    	mov     ax,DGROUP               ; AX = driver data selector
        mov     gs,ax                   ; GS: = driver data selector

IFNB	<extra> ;-----------------------

	assumes gs,Data

        pop     ecx                     ; save ECX = caller far return address
        push    extra                   ; save driver extra input parameter
        push    ecx                     ; save ECX = caller far return address

ENDIF ; IFNB <extra> ;------------------

        push    (SEG    target)         ; save destination code segment
        push    (OFFSET target)         ; save destination code
        jmp     DL_WaitForIdle          ; goto common wait for engine idle code

ENDM ;	DIBLINKIDLEDEV


;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
;
sBegin	Data
;
externD	    lpDriverPDevice             ; ENABLE: Screen surface PDEVICE struct
externW     wPalettized                 ; INIT:   Palettized pixels flag
externW     wMMIOSelector               ; VGA:    Engine regs MMIO selector
externB     InSrcBlt                    ; S3BLT:  Engine image xfer in progress
;
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
;
sBegin	Code
	assumes cs,Code
	.386
;
;------------------------------------------------------------------------------
; SelectBitmap: Return success here instead of jumping to DIBEngine and having
;               the DIBEngine immediately return success.
;------------------------------------------------------------------------------
;
        ALIGN   16

PUBLIC SelectBitmap
SelectBitmap	proc	far

        mov     ax,1                    ; AX = successful return status
        retf    16                      ;

SelectBitmap	endp


;------------------------------------------------------------------------------
; If forwarding a VRAM/offscreen surface to the DIBEngine, wait for engine
;   idle before forwarding the call.  This ensures any surface drawing
;   operations are completed before copying data to or from it.
; Regs: GS: = Driver data selector (DGROUP)
;
        assumes es,nothing
        assumes gs,Data

        ALIGN	16

PLABEL DL_WaitForIdle
	les	bx,gs:[lpDriverPDevice]	;#19189
	test	es:[bx].deFlags,BUSY
	jnz	short	DL_JumpToDIBEngine; Don't touch HW if BUSY	
        mov     es,gs:[wMMIOSelector]   ; ES: = engine MMIO selector

        WaitEngineIdle_Semaphore es, gs ; wait for idle/surface drawing done

PLABEL DL_JumpToDIBEngine
        retf                            ; jmp to DIBEngine using stack far ptr


;------------------------------------------------------------------------------
; Forward unhandled calls to the DIBEngine
; NOTE: DIBLINKIDLE functions could reference a screen/offscreen bitmap PDEVICE
;       surface that requires the engine to be idle (engine drawing to/from
;       the surface must be completed before DIBEngine access is allowed).
;
DIBLINKIDLE BitmapBits,         DIB_BitmapBits

DIBLINK     ColorInfo,          DIB_ColorInfo

DIBLINK     GetCharWidth,       DIB_GetCharWidth

DIBLINKIDLE DibBlt,             DIB_DibBltExt,              wPalettized

DIBLINKIDLE DibToDevice,        DIB_DibToDevice

DIBLINK     CreateDIBitmap,     DIB_CreateDIBitmap

DIBLINK     EnumDFonts,         DIB_EnumDFonts

DIBLINK     EnumObj,            DIB_EnumObjExt,             lpDriverPDevice

DIBLINK     SetPaletteTranslate,DIB_SetPaletteTranslateExt, lpDriverPDevice

DIBLINK     GetPaletteTranslate,DIB_GetPaletteTranslateExt, lpDriverPDevice

DIBLINK     GetPalette,         DIB_GetPaletteExt,          lpDriverPDevice

DIBLINK     UpdateColors,       DIB_UpdateColorsExt,        lpDriverPDevice

DIBLINKIDLE Pixel,              DIB_Pixel

DIBLINK	RealizeObject           DIB_RealizeObjectExt        lpDriverPDevice

DIBLINK ScanLR,                 DIB_ScanLR


DIBLINKIDLE StretchBlt,         DIB_StretchBlt

DIBLINKIDLE StretchDIBits,      DIB_StretchDIBits


DIBLINK     DeviceMode,         DIB_DeviceMode

DIBLINK     DeviceBitmap,       DIB_DeviceBitmap

DIBLINK     SetAttribute,       DIB_SetAttribute

DIBLINK     FastBorder,         DIB_FastBorder

sEnd	Code
end
