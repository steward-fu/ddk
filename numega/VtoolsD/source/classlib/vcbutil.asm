;; vcbutil.asm - utility routines for callback thunks
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

VxD_LOCKED_CODE_SEG

	EXTCFUNC resolveIndirect

	DUMMY equ 80000000h		;; an arbitrary 32-bit value

;; Callback Thunk
;;
;; This thunk is used by classes VV86Callback and VProtModeCallback.
;; It calls a handler defined as 
;;
;;	BOOL handler(VMHANDLE, CLIENT_STRUCT*, PVOID refData);
;;
;; This thunk loads ECX with the "this" pointer provided when the thunk
;; is created just prior to invoking the handler.

	align 4
callbackThunk:
	cld
	push	edx			;; Reference data paramater
	push	ebp			;; CLIENT_STRUCT pointer parameter
	push	ebx			;; VM Handle parameter

	PassThis			;; Pass -this- : compiler dependent
THIS_OFF = offset32 $ -  4 - offset32 callbackThunk
	
	db	0E8h, 0,0,0,0		;; call relative near handler
HANDLER_OFF = offset32 $ - 4 - offset32 callbackThunk

	bt	eax, 0			;; set carry on BOOLEAN eax
	cmc
	ret

	align 4
CALLBACKTHUNKSIZE  = offset32 $ - offset32 callbackThunk
.errnz CALLBACKTHUNKSIZE-14h ;; Thunk size has changed. Update this file and VCALLBAK.H.

;; Function
;;	createCallbackThunk - create a thunk for invocation of a callback handler
;;
;; Syntax
;;	PVOID createCallbackThunk(VCallback* pcb,BYTE* pThunk, (VOID* handler)());
;;
;; Parameters
;;	pcb		pointer to callback object; becomes "this" for handler
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

DEFCPROC createCallbackThunk <USES esi edi, pcbObj:DWORD, pThunk:DWORD, pHandler:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 callbackThunk
	mov	ecx, CALLBACKTHUNKSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	eax, [pThunk]

	mov	edi, [pcbObj]
	mov	[eax+THIS_OFF], edi

	mov	edi, [pHandler]

;; Unlike other thunks, we cannot resolve the virtual function indirection
;; for callbacks because the thunk is created in the constructor, at which
;; time the vtab pointer is pointing to the vtab for the base class, not
;; the derived class.  As a result, attempting to resolve the indirection
;; at this time would give us a pointer to the wrong (base class) handler.
;; If, at some future time, the thunk is not created in the constructor,
;; it will be desirable to resolve the indirection.

;;;;;;;	mov	esi, [pcbObj]
;;;;;;;	call	resolveIndirect

	sub	edi, eax	  		;; make relative call
	sub	edi, HANDLER_OFF+4

	mov	[eax+HANDLER_OFF], edi

	ret
ENDCPROC createCallbackThunk


VxD_LOCKED_CODE_ENDS

	end

