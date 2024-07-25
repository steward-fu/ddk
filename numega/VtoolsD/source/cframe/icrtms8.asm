;; icrtms.asm - initialize C/C++ run time system for Microsoft C/C++ 8.x
;;
;; Copyright (c) 1994 Compuware Corporation  All Rights Reserved

	.386p
	.model FLAT

_ITEXT		SEGMENT DWORD PUBLIC FLAT 'ICODE'
_ITEXT		ENDS

_CRT$XCU	SEGMENT DWORD PUBLIC FLAT 'ICODE'
XCUSTART:
_CRT$XCU	ENDS
_CRT$XCL	SEGMENT DWORD PUBLIC FLAT 'ICODE'
XCUEND:
_CRT$XCL	ENDS
_CRT$XCC	SEGMENT DWORD PUBLIC FLAT 'ICODE'
_CRT$XCC	ENDS
	
	PUBLIC INITCRT

_IGROUP	GROUP _ITEXT, _CRT$XCU, _CRT$XCL, _CRT$XCC

_ITEXT	SEGMENT DWORD PUBLIC FLAT 'ICODE'
INITCRT:
	push	esi
	push	edi

	mov	edi, offset FLAT:XCUEND
	mov	esi, offset FLAT:XCUSTART

nextInitializer:
	cmp	esi, edi
	je	@F
	
	call	DWORD PTR [esi]

	add	esi, 4
	jmp	nextInitializer
@@:
	pop	edi
	pop	esi
	ret

_ITEXT	ENDS

	END
