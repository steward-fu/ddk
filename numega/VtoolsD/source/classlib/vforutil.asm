;; vforutil.asm - utility routines for foreign API thunks
;;
;=============================================================================
;
; Compuware Corporation
; NuMega Lab
; 9 Townsend West
; Nashua, NH 03060  USA
;
; Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
; Unpublished - rights reserved under the Copyright laws of the
; United States.
;
; U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
; U.S. Government is subject to restrictions as set forth in 
; Compuware Corporation license agreement and as provided in 
; DFARS 227.7202-1(a) and 227.7202-3(a) (1995), 
; DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995), 
; FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.  
; Compuware Corporation.
; 
; This product contains confidential information and trade secrets 
; of Compuware Corporation. Use, disclosure, or reproduction is 
; prohibited without the prior express written permission of Compuware 
; Corporation.
;
;=============================================================================

	.386p
	NO_SEGMENTS=1
	include VMM.INC
	include VSEGMENT.INC
	include CDEFNS.INC

	EXTCFUNC resolveIndirect

VxD_LOCKED_CODE_SEG

	DUMMY equ 80000000h		;; an arbitrary 32-bit value

;; Foreign API Thunk
;;
;; This thunk is used by classes VV86API and VPMAPI.
;; It calls a handler defined as 
;;
;;	VOID handler(VMHANDLE, CLIENT_STRUCT*);
;;
;; This thunk loads ECX with the "this" pointer provided when the thunk
;; is created just prior to invoking the handler.

	align 4

foreignAPIThunk:
	cld
	mov	eax, ebp
	xchg	eax, [esp]		;; CLIENT_STRUCT pointer parameter
	push	ebx			

	PassThis			;; Pass -this- : compiler dependent
THIS_OFF = offset32 $ - 4 - offset32 foreignAPIThunk
	
	push	eax			;; return address
	db	0E9h, 0,0,0,0		;; jmp relative near handler
HANDLER_OFF = offset32 $ - 4 - offset32 foreignAPIThunk

	align 4

foreignAPIThunkSIZE  = offset32 $ - offset32 foreignAPIThunk

.errnz foreignAPIThunkSIZE-14h ;; Thunk size has changed. Update here and VDEVAPI.H

;; Function
;;	createForeignAPIThunk - create a thunk for invocation of a handler
;;
;; Syntax
;;	PVOID createforeignAPIThunk(VAPI* pObj, BYTE* pThunk, PVOID (handler*)())
;;
;; Parameters
;;	pObj		pointer to object; becomes "this" for handler
;;	pThunk		pointer to buffer to receive thunk code
;;	handler		address of handler to invoke
;;
;; Remarks
;;	This function loads thunk code into the supplied buffer.  The
;;	thunk pushes parameters and calls the handler.
;;
;; Return Value
;;	Returns the address that should be passed to the VM as the
;;	handler address.
;;
DEFCPROC createForeignAPIThunk <USES esi edi, pObj:DWORD, pThunk:DWORD,	pHandler:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 foreignAPIThunk
	mov	ecx, foreignAPIThunkSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	edi, [pThunk]

	mov	eax, [pObj]
	mov	[edi+THIS_OFF], eax

	mov	edi, [pHandler]
	mov	esi, [pObj]
	call	resolveIndirect

	mov	eax, [pThunk]
	sub	edi, eax
	sub	edi, HANDLER_OFF+4
	mov	[eax+HANDLER_OFF], edi

	mov	eax, [pThunk]
	ret
ENDCPROC createForeignAPIThunk 

ifdef __BORLANDC__

DEFCPROC _call_previous <USES ebx, hVM:DWORD, pRegs:DWORD, pPrev:DWORD>

	push ebp
	mov edx, pPrev
	mov ebx, hVM
	mov eax, pRegs
	mov ebp, eax
	call edx
	pop ebp

	ret
ENDCPROC _call_previous 

endif

VxD_LOCKED_CODE_ENDS

	end

