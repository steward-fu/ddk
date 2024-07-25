;  vsegment.inc -  segment definitions and order for all libraries
;
;  Copyright (c) 1994 Compuware Corporation All Rights Reserved

IFNDEF __BORLANDC__
;* 32-bit Initialization code 
	_ITEXT	SEGMENT DWORD PUBLIC FLAT 'ICODE'
	_ITEXT	ENDS

;* 32-Bit initialization data
	_IDATA	SEGMENT DWORD PUBLIC FLAT 'ICODE'
	_IDATA	ENDS

;* 32 bit locked code
	_LTEXT	SEGMENT DWORD PUBLIC FLAT 'LCODE'
	_LTEXT	ENDS	

;* 32-bit locked data
	_LDATA	SEGMENT DWORD PUBLIC FLAT 'LCODE'
	_LDATA	ENDS

;* 32-bit pageable code
	_PTEXT	SEGMENT DWORD PUBLIC FLAT 'PCODE'
	_PTEXT	ENDS

IFDEF WIN40
;* 32-bit pageable data
	_PDATA	SEGMENT DWORD PUBLIC FLAT 'PDATA'
	_PDATA	ENDS
ELSE
;* 32-bit pageable data
	_PDATA	SEGMENT DWORD PUBLIC FLAT 'PCODE'
	_PDATA	ENDS
ENDIF

;* 32-bit static code
	_STEXT	SEGMENT DWORD PUBLIC FLAT 'SCODE'
	_STEXT	ENDS

;* 32-bit static data
	_SDATA	SEGMENT DWORD PUBLIC FLAT 'SCODE'
	_SDATA	ENDS

IFDEF WIN40
;* 32-bit debug-only code
	_DBOSTART SEGMENT DWORD PUBLIC FLAT 'DBOCODE'
	_DBOSTART ENDS

;* 32-bit debug-only code
	_DBOTEXT SEGMENT DWORD PUBLIC FLAT 'DBOCODE'
	_DBOTEXT ENDS

;* 32-bit debug-only data
	_DBODATA SEGMENT DWORD PUBLIC FLAT 'DBOCODE'
	_DBODATA ENDS

ENDIF
;* Real Mode initialization code/data for devices
	_RCODE	SEGMENT WORD USE16 PUBLIC 'RCODE'
	_RCODE	ENDS

;* Compiler generated segments
	_TEXT	SEGMENT BYTE PUBLIC FLAT 'LCODE'
	_TEXT	ENDS

	_DATA	SEGMENT DWORD PUBLIC FLAT 'LCODE'
	_DATA	ENDS

	_CONST	SEGMENT DWORD PUBLIC FLAT 'LCODE'
	_CONST	ENDS	

	CONST	SEGMENT DWORD PUBLIC FLAT 'LCODE'
	CONST	ENDS	

	_BSS	SEGMENT DWORD PUBLIC FLAT 'LCODE'
	_BSS	ENDS	

	_TLS	SEGMENT DWORD PUBLIC FLAT 'LCODE'
	_TLS	ENDS	

ELSE	;;   __BORLANDC__

;* 32-bit Initialization code 
	_ITEXT	SEGMENT DWORD PUBLIC USE32 'ICODE'
	_ITEXT	ENDS

;* 32-Bit initialization data
	_IDATA	SEGMENT DWORD PUBLIC USE32 'ICODE'
	_IDATA	ENDS

;* Borland Compiler Initialization segment
	_INIT_	SEGMENT WORD PUBLIC USE32 'INITDATA'
	_INIT_  ENDS

;* 32 bit locked code
	_LTEXT	SEGMENT DWORD PUBLIC USE32 'LCODE'
	_LTEXT	ENDS	

;* 32-bit locked data
	_LDATA	SEGMENT DWORD PUBLIC USE32 'LCODE'
	_LDATA	ENDS

;* 32-bit pageable code
	_PTEXT	SEGMENT DWORD PUBLIC USE32 'PCODE'
	_PTEXT	ENDS

IFDEF WIN40
;* 32-bit pageable data
	_PDATA	SEGMENT DWORD PUBLIC USE32 'PDATA'
	_PDATA	ENDS
ELSE
;* 32-bit pageable data
	_PDATA	SEGMENT DWORD PUBLIC USE32 'PCODE'
	_PDATA	ENDS
ENDIF

;* 32-bit static code
	_STEXT	SEGMENT DWORD PUBLIC USE32 'SCODE'
	_STEXT	ENDS

;* 32-bit static data
	_SDATA	SEGMENT DWORD PUBLIC USE32 'SCODE'
	_SDATA	ENDS

IFDEF WIN40
;* 32-bit debug-only code
	_DBOSTART SEGMENT DWORD PUBLIC USE32 'DBOCODE'
	_DBOSTART ENDS

;* 32-bit debug-only code
	_DBOTEXT SEGMENT DWORD PUBLIC USE32 'DBOCODE'
	_DBOTEXT ENDS

;* 32-bit debug-only data
	_DBODATA SEGMENT DWORD PUBLIC USE32 'DBOCODE'
	_DBODATA ENDS
ENDIF

IFDEF REALMODE
;* Real Mode initialization code/data for devices
	_RCODE	SEGMENT WORD USE16 PUBLIC 'RCODE'
	_RCODE	ENDS
ENDIF

;* Compiler generated segments
;	_TEXT	SEGMENT BYTE PUBLIC USE32 'LCODE'
;	_TEXT	ENDS
;
;	_DATA	SEGMENT DWORD PUBLIC USE32 'LDATA'
;	_DATA	ENDS

	_CONST	SEGMENT DWORD PUBLIC USE32 'LCODE'
	_CONST	ENDS	

	CONST	SEGMENT DWORD PUBLIC USE32 'LCODE'
	CONST	ENDS	

	_BSS	SEGMENT DWORD PUBLIC USE32 'LCODE'
	_BSS	ENDS	

	_TLS	SEGMENT DWORD PUBLIC USE32 'LCODE'
	_TLS	ENDS	

ENDIF


ifndef COFF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Groups


ifdef __BORLANDC__
_LGROUP		GROUP	_LDATA, _CONST, CONST, _BSS, _TLS, _LTEXT, _TEXT
else
_LGROUP		GROUP	_LDATA, _DATA, _CONST, CONST, _BSS, _TLS, _LTEXT, _TEXT
endif

_SGROUP		GROUP	_STEXT, _SDATA
_IGROUP		GROUP	_ITEXT, _IDATA

IFDEF WIN40
_DBOGROUP	GROUP	_DBOSTART, _DBOTEXT, _DBODATA
ENDIF

IFNDEF WIN40
_PGROUP		GROUP	_PTEXT, _PDATA
ENDIF

;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

endif


;; "far" flat call macro
;;
;; Use this macro with MASM 6.1 to do intersegment calls. MASM 6.1 does
;; not generate the correct fixup if you code "call func".  MASM 6.0 did
;; not exhibit this problem.

ffcall	MACRO func
	local fptr, skip
ifdef __BORLANDC__
	call	func
else

IF @Version EQ 600

	call	func

ELSE
    %out Warning - not using MASM 600 - inter-segment workaround used
	push	OFFSET FLAT:skip
	jmp	DWORD PTR fptr
fptr	dd	OFFSET FLAT:func
skip:

ENDIF

endif
	ENDM

ifdef __BORLANDC__
	.model FLAT
endif
