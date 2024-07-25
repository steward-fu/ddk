;; vipfutil.asm - utility routines for invalid page fault thunks
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

;; Invalid Page Fault Thunk
;;
;; This thunk is used by class VInvalidPageFault.
;; It calls a handler defined as 
;;
;;	BOOL handler(VMHANDLE, IPF_Data*);
;;
;; This thunk loads ECX with the "this" pointer provided when the thunk
;; is created just prior to invoking the handler.

	align 4

IPFThunk:
	cld
	push	ebp
	push	edi			;; Pointer to struct IPF_Data 
	push	ebx			;; VM Handle parameter

	PassThis			;; Pass -this- : compiler dependent
THIS_OFF = offset32 $ -  4 - offset32 IPFThunk
	
	db	0E8h, 0,0,0,0		;; call relative near handler
HANDLER_OFF = offset32 $ - 4 - offset32 IPFThunk

	bt	eax, 0			;; set carry on BOOLEAN eax
	cmc
	ret

	align 4

IPFTHUNKSIZE  = offset32 $ - offset32 IPFThunk

.errnz IPFTHUNKSIZE-14h ;; Thunk size has changed. Update here and VIPF.H

;; Function
;;	createIPFThunk - create a thunk for invocation of a IPF handler
;;
;; Syntax
;;	PVOID createIPFThunk(VInvalidPageFault* pcb,BYTE* pThunk, (VOID* handler)());
;;
;; Parameters
;;	pipfObj		pointer to IPF object; becomes "this" for handler
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
DEFCPROC createIPFThunk <USES esi edi, pipfObj:DWORD, pThunk:DWORD, pHandler:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 IPFThunk
	mov	ecx, IPFTHUNKSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	eax, [pThunk]

	mov	edi, [pipfObj]
	mov	[eax+THIS_OFF], edi

	mov	edi, [pHandler]
	mov	esi, [pipfObj]
	call	resolveIndirect
	sub	edi, eax
	sub	edi, HANDLER_OFF+4
	mov	[eax+HANDLER_OFF], edi

	ret
ENDCPROC createIPFThunk 


VxD_LOCKED_CODE_ENDS

	end

