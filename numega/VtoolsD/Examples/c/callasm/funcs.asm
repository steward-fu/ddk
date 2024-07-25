;; FUNCS.ASM - demonstration of assembler functions called from C/C++ code
;; Copyright (c) 1995 Compuware Corporation

	.386p
	NO_SEGMENTS=1
	include	vmm.inc
	include	vsegment.inc
	include cdefns.inc
;;
;; Tips for interfacing assembly language to C/C++ code:
;;
;;	1. Begin assembly language modules with these lines:
;;
;;		.386p
;;		NO_SEGMENTS=1
;;		include	vmm.inc
;;		include	vsegment.inc
;;		include cdefns.inc
;;
;;	2. Make sure all assembly language functions preserve ESI, EDI,
;;	   EBX, and EBP.
;;
;;	3. Use the following macros to simplify declaration of functions:
;;	
;;		DEFCFUNC functionname, <arglist>	;; define __cdecl
;;		DEFSFUNC functionname, <arglist>	;; define __stdcall
;;		EXTCFUNC functionname			;; extern __cdecl
;;		EXTSFUNC functionname, nargbytes	;; extern __stdcall
;;
;;	4. Function return values should be put in EAX before the ret
;;	   instruction.
;;

	EXTSFUNC CallThisStdcallFromAssembler,4
	EXTCFUNC CallThisCdeclFromAssembler

VXD_LOCKED_DATA_SEG

stringArg	db	'calling from assembler code', 0

VXD_LOCKED_DATA_ENDS

VXD_LOCKED_CODE_SEG

;;
;; Example of __cdecl function returns the sum of two DWORD args
;;
DEFCPROC Sum_ThisIsACdecl, <Carg1:DWORD, Carg2:DWORD>
	mov	eax, [Carg1]
	add	eax, [Carg2]
	ret
ENDCPROC Sum_ThisIsACdecl

;;
;; Example of __stdcall function returns the difference (a1-a2) of two args
;;
DEFSPROC Diff_ThisIsAStdcall, <Sarg1:DWORD, Sarg2:DWORD>
	mov	eax, [Sarg1]
	sub	eax, [Sarg2]
	ret	;; this instruction pops args - assembler does the right thing
ENDSPROC Diff_ThisIsAStdcall

;;
;; Example of assembler interface function - returns sum of ecx and edx
;;
	PUBLIC _Sum_ThisAddsECXandEDX

_Sum_ThisAddsECXandEDX PROC
	push	ecx		;; save the input args
	push	edx

;; call the __stdcall - called function pops the argument
	push	offset32 stringArg
	call	CallThisStdcallFromAssembler

;; call the __cdecl - caller is responsible for popping arguments
	push	offset32 stringArg
	call	CallThisCdeclFromAssembler
	lea	esp, [esp+4]

;; pop the arguments to this function and compute the return value
	pop	edx
	pop	ecx
	mov	eax, ecx
	add	eax, edx
	ret
_Sum_ThisAddsECXandEDX ENDP


VXD_LOCKED_CODE_ENDS


	END
