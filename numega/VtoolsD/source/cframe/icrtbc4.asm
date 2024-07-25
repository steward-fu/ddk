;; icrtbc4.asm - initialize C/C++ run time system for Borland C/C++ 4.x
;;
;; Copyright (c) 1994 Compuware Corporation  All Rights Reserved

	.386p

PELEDATA SEGMENT DWORD USE32 PUBLIC 'PELEDATA'
PELEDATA ENDS

_INIT0_ SEGMENT DWORD PUBLIC USE32 'INITDAT0'
	db	200h dup (0)	
_I0END_	LABEL	BYTE
_INIT0_ ENDS

_INIT_	SEGMENT WORD PUBLIC USE32 'INITDATA'
	dw	0FFFFh
_INIT_	ENDS

	_TLS	SEGMENT DWORD PUBLIC USE32 'LCODE'
	_TLS	ENDS	

	include vsegment.inc

	.model FLAT

	PUBLIC INITCRT

_ITEXT	SEGMENT DWORD PUBLIC USE32 'ICODE'
	ASSUME CS:_ITEXT

INITCRT:
	push	esi
	mov	esi, offset FLAT:_I0END_

nextInit:
	cmp	WORD PTR [esi], 0FFFFh
	jne	short @1
	inc	esi
	inc	esi
@1:
	cmp	WORD PTR [esi], 0
	je	short init_exit
	call	DWORD PTR [esi+2]
	add	esi, 6
	jmp	nextInit

init_exit:
	pop	esi
	ret

	public	__TLS_index
__TLS_index	dd	0

_ITEXT	ENDS

	END

