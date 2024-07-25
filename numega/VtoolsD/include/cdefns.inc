;  cdefns.inc -  definitions C/C++ interface to .asm modules
;
;  Copyright (c) 1994 Compuware Corporation All Rights Reserved


;; Macro to define pubilc C entry point (__cdecl)

DEFCPROC MACRO PROCNAME, ARGS
ifdef __BORLANDC__
	PUBLIC _&PROCNAME
_&PROCNAME PROC C ARGS
else
PROCNAME PROC C PUBLIC ARGS
endif
	ENDM

;; Macro to define end of pubilc C entry point

ENDCPROC MACRO PROCNAME
ifdef __BORLANDC__
_&PROCNAME ENDP
else
PROCNAME ENDP
endif
	ENDM

;; Macro to define external C function

EXTCFUNC MACRO FUNCNAME
ifdef __BORLANDC__
	EXTRN C FUNCNAME:NEAR
else
	EXTRN C FUNCNAME:NEAR
endif
	ENDM

;; Macro to define pubilc standard entry point (__stdcall)

DEFSPROC MACRO PROCNAME, ARGS
ifdef __BORLANDC__
	PUBLIC PROCNAME
PROCNAME PROC STDCALL ARGS
else
PROCNAME PROC STDCALL PUBLIC ARGS
endif
	ENDM


;; Macro to define end of pubilc standard entry point

ENDSPROC MACRO PROCNAME
PROCNAME ENDP
	ENDM

;; Macro to define external __stdcall function

EXTSFUNC MACRO FUNCNAME, NARGBYTES
ifdef __BORLANDC__
	EXTRN FUNCNAME:NEAR
else
	EXTERN C FUNCNAME&@&NARGBYTES:NEAR
	FUNCNAME EQU FUNCNAME&@&NARGBYTES
endif
	ENDM


;; Macro to pass -this- pointer to member function (used in thunks)

PassThis MACRO 
ifdef __BORLANDC__
	db	68h, 0,0,0,0		;; push imm32
else
	mov	ecx, 80000000h		;; pass in ecx
endif
	ENDM


PassThisCdecl MACRO
	db	68h, 0,0,0,0		;; push imm32

	ENDM
