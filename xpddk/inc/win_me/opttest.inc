;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1990
;
;   Title:	OPTTEST.INC - Macros to optimize test instructions
;
;   Version:	1.00
;
;   Date:	28-Jun-1990
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   28-Jun-1990 RAL
;   20-Sep-1993 rjc mov_b
;   07-Oct-1993 rjc SetReg
;   22-Jan-1994 rjc movnz
;   12-Nov-1995 TCS ClrReg
;
;==============================================================================


TestMem MACRO MemPtr, M, N
IFNB <N>
        .err    <TestMem has too many parameters: N>
ENDIF
IFE (M) AND 0FFFFFF00h
	test	BYTE PTR [MemPtr], M
ELSEIFE (M) AND 0FFFF00FFh
	test	BYTE PTR [MemPtr+1], ((M) SHR 8)
ELSEIFE (M) AND 0FF00FFFFh
	test	BYTE PTR [MemPtr+2], ((M) SHR 16)
ELSEIFE (M) AND 000FFFFFFh
	test	BYTE PTR [MemPtr+3], ((M) SHR 24)
ELSEIFE (M) AND 0FFFF0000h
	test	WORD PTR [MemPtr], M
ELSEIFE (M) AND 0FF0000FFh
	test	WORD PTR [MemPtr+1], ((M) SHR 8)
ELSEIFE (M) AND 00000FFFFh
	test	WORD PTR [MemPtr+2], ((M) SHR 16)
ELSE
	test	DWORD PTR [MemPtr], M
ENDIF
	ENDM


;
; NOTE: any future modifications of this macro must also clear carry
;
; BUGBUG -- ClrFlag uses BYTE overrides when a DWORD would do better on
; a 486 in 32-bit mode.
;
ClrFlag MACRO MemPtr, M, N
IFNB <N>
        .err    <ClrFlag has too many parameters: N>
ENDIF
IFE (M) AND 0FFFFFF00h
	and	BYTE PTR [MemPtr], NOT (M)
ELSEIFE (M) AND 0FFFF00FFh
	and	BYTE PTR [MemPtr+1], NOT ((M) SHR 8)
ELSEIFE (M) AND 0FF00FFFFh
	and	BYTE PTR [MemPtr+2], NOT ((M) SHR 16)
ELSEIFE (M) AND 000FFFFFFh
	and	BYTE PTR [MemPtr+3], NOT ((M) SHR 24)
ELSEIFE (M) AND 0FFFF0000h
	and	WORD PTR [MemPtr], NOT (M)
ELSEIFE (M) AND 0FF0000FFh
	and	WORD PTR [MemPtr+1], NOT ((M) SHR 8)
ELSEIFE (M) AND 00000FFFFh
	and	WORD PTR [MemPtr+2], NOT ((M) SHR 16)
ELSE
	and	DWORD PTR [MemPtr], NOT (M)
ENDIF
	ENDM


SetFlag MACRO MemPtr, M, N
IFNB <N>
        .err    <SetFlag has too many parameters: N>
ENDIF
IFE (M) AND 0FFFFFF00h
	or	BYTE PTR [MemPtr], M
ELSEIFE (M) AND 0FFFF00FFh
	or	BYTE PTR [MemPtr+1], ((M) SHR 8)
ELSEIFE (M) AND 0FF00FFFFh
	or	BYTE PTR [MemPtr+2], ((M) SHR 16)
ELSEIFE (M) AND 000FFFFFFh
	or	BYTE PTR [MemPtr+3], ((M) SHR 24)
ELSEIFE (M) AND 0FFFF0000h
	or	WORD PTR [MemPtr], M
ELSEIFE (M) AND 0FF0000FFh
	or	WORD PTR [MemPtr+1], ((M) SHR 8)
ELSEIFE (M) AND 00000FFFFh
	or	WORD PTR [MemPtr+2], ((M) SHR 16)
ELSE
	or	DWORD PTR [MemPtr], M
ENDIF
	ENDM



_@TR MACRO Ltr, Mask
IFE (Mask) AND 0FFFFFF00h
        test    Ltr&l, Mask
ELSEIFE (Mask) AND 0FFFF00FFh
        test    Ltr&h, ((Mask) SHR 8)
ELSEIFE (Mask) AND 0FFFF0000h
        test    Ltr&x, Mask
ELSE
        test    e&Ltr&x, Mask
ENDIF
	ENDM


TestReg MACRO R, Mask

IFIDNI <R>, <EAX>
        _@TR a, <Mask>
ELSEIFIDNI <R>, <EBX>
        _@TR b, <Mask>
ELSEIFIDNI <R>, <ECX>
        _@TR c, <Mask>
ELSEIFIDNI <R>, <EDX>
        _@TR d, <Mask>
ELSEIFIDNI <R>, <ESI>
    IFE (Mask) AND 0FFFF0000h
	test	si, Mask
      ;;%out    WARNING: TestReg ESI isn't really that great...
    ELSE
	test	esi, Mask
    ENDIF
ELSEIFIDNI <R>, <EDI>
    IFE (Mask) AND 0FFFF0000h
	test	di, Mask
      ;;%out    WARNING: TestReg EDI isn't really that great...
    ELSE
	test	edi, Mask
    ENDIF
ELSE
        .err    <Bad param to TestReg macro: R>
ENDIF
	ENDM


;
; SetReg sets a bit in a register, analogous to TestReg.
;

;
; _@SRb is the SetReg helper macro for registers which are byte-addressible.
;
_@SRb macro reg:req, mask:req
    ife (mask) and 0ffffff00h
	    or	reg&l, mask
    elseife (mask) and 0ffff00ffh
	    or	reg&h, ((mask) shr 8)
    elseife (mask) and 0ffff0000h
	    or	reg&x, mask
    else
	    or	e&reg&x, mask
    endif

endm

;
; _@SRw is the SetReg helper macro for registers which are word-addressible.
;
; Note that the case where the bitmask is less than 128 is special, because
; we are better off using the short form, to take advantage of the CPU's
; special `byte operand' mode.  No such luck with Test, which has no `byte'
; mode.
;
_@SRw macro reg:req, mask:req
    ife (mask) and 0ffffff80h
	if  @WordSize EQ 2
	    or	reg, mask
	else
	    or	e&reg, mask
	endif
    elseife (mask) and 0ffff0000h
	    or	reg, mask
    else
	    or	e&reg&x, mask
    endif
endm


SetReg macro r:req, mask:req

    ifidni <r>, <eax>
	    _@SRb a, <mask>
    elseifidni <r>, <ebx>
	    _@SRb b, <mask>
    elseifidni <r>, <ecx>
	    _@SRb c, <mask>
    elseifidni <r>, <edx>
	    _@SRb d, <mask>
    elseifidni <r>, <esi>
	    _@SRw si, <mask>
    elseifidni <r>, <edi>
	    _@SRw di, <mask>
    else
        .err <Bad param to SetReg macro: r>
    endif
endm


;
; ClrReg clears a bit in a register, analogous to TestReg.
;

;
; _@CRb is the ClrReg helper macro for registers which are byte-addressible.
;
_@CRb macro reg:req, mask:req
    ife (mask) and 0ffffff00h
            and reg&l, NOT (mask)
    elseife (mask) and 0ffff00ffh
            and reg&h, NOT ((mask) shr 8)
    elseife (mask) and 0ffff0000h
            and reg&x, NOT (mask)
    else
            and e&reg&x, NOT (mask)
    endif

endm

;
; _@CRw is the ClrReg helper macro for registers which are word-addressible.
;
; Note that the case where the bitmask is less than 128 is special, because
; we are better off using the short form, to take advantage of the CPU's
; special `byte operand' mode.  No such luck with Test, which has no `byte'
; mode.
;
_@CRw macro reg:req, mask:req
    ife (mask) and 0ffffff80h
	if  @WordSize EQ 2
            and reg, NOT (mask)
	else
            and e&reg, NOT (mask)
	endif
    elseife (mask) and 0ffff0000h
            and reg, NOT (mask)
    else
            and e&reg&x, NOT (mask)
    endif
endm


ClrReg macro r:req, mask:req

    ifidni <r>, <eax>
            _@CRb a, <mask>
    elseifidni <r>, <ebx>
            _@CRb b, <mask>
    elseifidni <r>, <ecx>
            _@CRb c, <mask>
    elseifidni <r>, <edx>
            _@CRb d, <mask>
    elseifidni <r>, <esi>
            _@CRw si, <mask>
    elseifidni <r>, <edi>
            _@CRw di, <mask>
    else
        .err <Bad param to ClrReg macro: r>
    endif
endm


;
; mov_b moves a small byte value into a 32-bit register or 32-bit memory
; location, sacrificing speed for size.  It may destroy flags.
;
mov_b	macro	regmem:req, val:req
    ??mov_b_done = 0
    ife ((val) LT 080h) OR ((val) GE 0ffffff80h)
        .err <Second argument to mov_b out of range: val>
    endif
    if	(OPATTR regmem) and 10h		; If is a register
					; Then additional opt'z'ns possible
	if (val) EQ 0
	    xor	regmem, regmem		; Zero
	    ??mov_b_done = 1
	elseif (val) EQ 1
	    xor	regmem, regmem
	    inc	regmem			; One
	    ??mov_b_done = 1
	else
	    ifidni <regmem>, <eax>	; EAX has really cool stuff available
		if ((val) GE 0) and ((val) LE 0FFh)
	    xor	    eax, eax
	    mov	    al, val
		    ??mov_b_done = 1
		endif
	    endif
	endif
    endif

    ife ??mov_b_done
	if (VAL) eq 0
	    and	regmem, 0		; Zero
	elseif (VAL) eq 0FFFFFFFFh
	    or	regmem, -1		; Negative one
	else
	    push val			; Anything else
	    pop	regmem
	endif
    endif
endm

;
; movnz moves an arbitrary nonzero value into the specified register.
; It may destroy flags.  If fParanoid is non-empty, then we will also
; ensure that the low word of the register is nonzero.  (This is to
; cover for some people who forgot that they are in 32-bit land and
; just do a "test ax, ax" to check the return value.)  I.e., if you
; suspect your caller of being a bozo, you say
;
;	movnz r, BOZO
;
; instead of just
;
;	movnz r
;
movnz	macro	R:req, fParanoid
    IFIDNI <R>, <EAX>
	mov	al, 1		; 2 bytes
    ELSEIFIDNI <R>, <EBX>
	mov	bl, 1		; 2 bytes
    ELSEIFIDNI <R>, <ECX>
	mov	cl, 1		; 2 bytes
    ELSEIFIDNI <R>, <EDX>
	mov	dl, 1		; 2 bytes
    ELSEIFNB <fParanoid>
	or	R, 1		; 3 bytes
    ELSE
	mov	R, esp	    	; 2 bytes (at ring 0, esp is never zero)
    ENDIF
endm
