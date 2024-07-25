;; vioputil.asm - utility routines for class VIOPort thunks
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

;; IOPort Thunk
;;
;; This thunk is used by class VIOPort.
;; It calls a handler defined as 
;;
;;	DWORD handler(VMHANDLE, DWORD port,CLIENT_STRUCT* pRegs,
;;			DWORD iotype, DWORD outdata);
;;
;; This thunk loads ECX with the "this" pointer provided when the thunk
;; is created just prior to invoking the handler.

	align 4

IOPortThunk:
	cld
	xchg	eax, [esp]		;; push out data, eax <- return addr
	push	ecx			;; IO type parameter
	push	ebp			;; client register pointer parameter
	push	edx			;; port number parameter
	push	ebx			;; VM handle parameter

	PassThis			;; Pass -this- : compiler dependent
THIS_OFF = offset32 $ -  4 - offset32 IOPortThunk
	
	push	eax			;; return address
	db	0E9h, 0,0,0,0		;; jmp relative near handler
HANDLER_OFF = offset32 $ - 4 - offset32 IOPortThunk

	align 4

IOPortTHUNKSIZE  = offset32 $ - offset32 IOPortThunk

.errnz IOPortTHUNKSIZE-14h ;; Thunk size has changed. Update here and VPORT.H

;; Function
;;	createIOPortThunk - create a thunk for invocation of a VIOPort handler
;;
;; Syntax
;;	PVOID createPortThunk(VIOPort* pport,BYTE* pThunk, (VOID* handler)());
;;
;; Parameters
;;	pport		pointer to IOPort object; becomes "this" for handler
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
DEFCPROC createPortThunk <USES esi edi, pportObj:DWORD,	pThunk:DWORD, pHandler:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 IOPortThunk
	mov	ecx, IOPortTHUNKSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	eax, [pThunk]

	mov	edi, [pportObj]
	mov	[eax+THIS_OFF], edi

	mov	edi, [pHandler]
	mov	esi, [pportObj]
	call	resolveIndirect
	sub	edi, eax
	sub	edi, HANDLER_OFF+4
	mov	[eax+HANDLER_OFF], edi

	ret
ENDCPROC createPortThunk 


VxD_LOCKED_CODE_ENDS

	end

