;; vprcutil.asm - utility routines for class VPreChainV86Int thunks
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
;;
	.386p

	NO_SEGMENTS=1
	include VMM.INC
	include VSEGMENT.INC
	include CDEFNS.INC

VxD_LOCKED_CODE_SEG

	EXTCFUNC resolveIndirect

	DUMMY equ 80000000h		;; an arbitrary 32-bit value

;; Pre-chain V86 Interrupt Handler Thunk
;;
;; This thunk is used by class VPreChainV86Int. It calls a handler defined as 
;;
;;	BOOL handler(VMHANDLE,CLIENT_STRUCT*,DWORD);
;;
	align 4
PIHThunk:	
PrevHandler	dd ?		;; Hook Proc preamble
	jmp		short PIHThunkStart
	jmp		dword ptr [PrevHandler]
PIHThunkStart:
	cld
	push	eax			;; interrupt number parameter
	push	ebp			;; Pointer to CLIENT_STRUCT parameter
	push	ebx			;; VM Handle parameter

	PassThis			;; Pass -this- : compiler dependent
THIS_OFF = offset32 $ -  4 - offset32 PIHThunk
	
	db	0E8h, 0,0,0,0		;; call relative near handler
HANDLER_OFF = offset32 $ - 4 - offset32 PIHThunk
	bt	eax, 0			;; set carry on BOOLEAN eax
	cmc

	ret

	align 4

PIHTHUNKSIZE  = offset32 $ - offset32 PIHThunk
THUNK_OFF = offset32 PIHThunkStart - offset32 PIHThunk

.errnz PIHTHUNKSIZE-20h ;; Thunk size has changed. Update here and VINTRS.H

;; Function
;;	createPreChainV86IntThunk - create a thunk for invocation of a post chain handler
;;
;; Syntax
;;	PVOID createPreChainV86IntThunk(VBaseInt* pbi,BYTE* pThunk, (VOID* handler)());
;;
;; Parameters
;;	pbi		pointer to VBaseInt object; becomes "this" for handler
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

DEFCPROC createPreChainV86IntThunk <USES esi edi, pbiObj:DWORD, pThunk:DWORD, pHandler:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 PIHThunk
	mov	ecx, PIHTHUNKSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	eax, [pThunk]

	lea	ecx, [eax + THUNK_OFF - 4]
	mov	[ecx], eax

	mov	edi, [pbiObj]
	mov	[eax+THIS_OFF], edi

	mov	edi, [pHandler]
	mov	esi, [pbiObj]
	call	resolveIndirect
	sub	edi, [pThunk]
	sub	edi, HANDLER_OFF+4
	mov	[eax+HANDLER_OFF], edi
	add	eax, THUNK_OFF

	ret
ENDCPROC createPreChainV86IntThunk 

DEFCPROC getPreChainV86IntHookAddress <pThunk:DWORD>
	mov	eax, [pThunk]
	add	eax, THUNK_OFF

	ret
ENDCPROC getPreChainV86IntHookAddress 

;; Post Interrupt Handler Thunk
;;
;; This thunk is used by class VBaseInt. It calls a handler defined as 
;;
;;	VOID postChainHandler(VMHANDLE,CLIENT_STRUCT*,DWORD);
;;
;; This thunk loads ECX with the "this" pointer provided when the thunk
;; is created just prior to invoking the handler.

	align 4

PostIHThunk:
	cld
	xchg	edx, [esp]		;; Reference data parameter pushed
					;; edx <- return address
	push	ebp			;; Pointer to CLIENT_STRUCT parameter
	push	ebx			;; VM Handle parameter

	PassThis
THIS_OFF = offset32 $ -  4 - offset32 PostIHThunk
	
	push	edx			;; push return address
	db	0E9h, 0,0,0,0		;; jump to handler
HANDLER_OFF = offset32 $ - 4 - offset32 PostIHThunk

	ret

	align 4

PostIHTHUNKSIZE  = offset32 $ - offset32 PostIHThunk
.errnz PostIHTHUNKSIZE-14h ;; Thunk size has changed. Update here and VINTRS.H


;; Function
;;	createPostChainThunk - create a thunk for invocation of a post chain handler
;;
;; Syntax
;;	PVOID createPostChainThunk(VBaseInt* pbi,BYTE* pThunk, (VOID* handler)());
;;
;; Parameters
;;	pbi		pointer to VBaseInt object; becomes "this" for handler
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
DEFCPROC createPostChainThunk <USES esi edi, pbiObj:DWORD, pThunk:DWORD, pHandler:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 PostIHThunk
	mov	ecx, PostIHTHUNKSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	eax, [pThunk]

	mov	edi, [pbiObj]
	mov	[eax+THIS_OFF], edi

	mov	edi, [pHandler]
	mov	esi, [pbiObj]
	call	resolveIndirect
	sub	edi, eax
	sub	edi, HANDLER_OFF+4
	mov	[eax+HANDLER_OFF], edi

	ret
ENDCPROC createPostChainThunk 

DEFCPROC getPostChainHookAddress <pThunk:DWORD>
	mov	eax, [pThunk]
	ret
ENDCPROC getPostChainHookAddress 


VxD_LOCKED_CODE_ENDS

	end
