;; icrtms.asm - initialize C/C++ run time system for Microsoft C/C++ 8.x
;;
;; Copyright (c) 1994 Compuware Corporation  All Rights Reserved

	.386p
	NO_SEGMENTS=1
	include VMM.INC
	include VSEGMENT.INC

ICRTTEXT	SEGMENT DWORD PUBLIC FLAT 'ICODE'
	dd	0
XCUSTART:
ICRTTEXT	ENDS

ICRTXXXX	SEGMENT DWORD PUBLIC FLAT 'ICODE'
	dd	0
ICRTXXXX	ENDS

	PUBLIC INITCRT

_ITEXT	SEGMENT DWORD PUBLIC FLAT 'ICODE'

INITCRT:
	xor	esi, esi

nextInitializer:
	cmp	dword ptr [XCUSTART+esi], 0
	je	@F
	
	call	DWORD PTR [XCUSTART+esi]

	add	esi, 4
	jmp	nextInitializer
@@:

	ret

_ITEXT	ENDS

	END
