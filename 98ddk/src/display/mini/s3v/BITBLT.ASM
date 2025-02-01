;------------------------------------------------------------------------------
; BITBLT.ASM
; Copyright (c) 1992 Microsoft Corporation
; Copyright (c) 1996 S3 Incorporated
;------------------------------------------------------------------------------
;
.xlist
DOS5 = 1
include cmacros.inc
include macros.inc
include dibeng.inc
.list

;------------------------------------------------------------------------------
; D A T A
;------------------------------------------------------------------------------
;
sBegin	Data
;
externW     pBltFn			; VGA.ASM - Near ptr to BitBLT function
;
sEnd	Data

;------------------------------------------------------------------------------
; C O D E
;------------------------------------------------------------------------------
;
sBegin	Code
	assumes cs,Code
.386
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,Data

PLABEL BitBlt
	mov	ax,DGROUP		; AX = driver data selector
	mov	gs,ax			; GS: = driver data selector
	jmp	gs:[pBltFn]		; jump to BitBLT function

sEnd	Code
end

