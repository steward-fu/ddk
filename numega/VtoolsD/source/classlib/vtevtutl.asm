;; vtevtutl.asm - utility routines for Thread Event thunks
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

;; Event Thunk
;;
;; This thunk is used by classes VVMEvent and VGlobalEvent
;; It calls a handler defined as 
;;
;;	VOID handler(THREADHANDLE hThread, VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
;;
;; This thunk loads ECX with the "this" pointer provided when the thunk
;; is created just prior to invoking the handler.
;;
;; The lowest bit of the first byte of the thunk is a flag that indicates
;; if the thunk is allocated.  This is assuming that the thunk is allocated
;; on the event heap (no harm done if it is not).  This flag automatically
;; gets cleared (effectively deleting the the event object) when the
;; handler returns.

	align 4
EventThunk:
	db	1			;; allocation flag in lowest bit
	push	edx			;; Reference data paramater
	push	ebp			;; CLIENT_STRUCT pointer parameter
	push	ebx			;; VM Handle parameter
	push	edi			;; Thread handle parameter

	PassThis			;; Pass -this- : compiler dependent
THIS_OFF = offset32 $ -  4 - offset32 EventThunk
	cld	
	db	0E8h, 0,0,0,0		;; relative near call to handler
HANDLER_OFF = offset32 $ - 4 - offset32 EventThunk
					;; callee pops args

	mov	eax, DUMMY		;; eax <- thunk address
THUNKADDR_OFF = offset32 $ - 4 - offset32 EventThunk

;; now we have to construct a short patch of code on the stack. It is unsafe
;; to execute the thunk after it has been freed. The code written onto the 
;; stack frees the thunk and returns

	mov	ecx, [esp]						;; save return address 
	mov dword ptr [esp], dword ptr 8	;; 2nd dword of code 
	push	0c20000c6h					;; 1st dword of code
	push	ecx							;; return addresss
	lea		ecx, [esp+4]				;; point to code on stack

	;; the code on the stack is 
	;;		mov	byte ptr [eax], 0
	;;		ret 8

	jmp		ecx							;; bye

	align	4
EventTHUNKSIZE  = offset32 $ - offset32 EventThunk

.errnz EventTHUNKSIZE-2Ch ;; Thunk size has changed. Update here and VEVENT.H


;; Function
;;	createThreadEventThunk - create a thunk for invocation of a Event handler
;;
;; Syntax
;;	PVOID createThreadEventThunk(VThreadEvent* pevent,BYTE* pThunk, (VOID* handler)());
;;
;; Parameters
;;	pevent		pointer to Event object; becomes "this" for handler
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
DEFCPROC createThreadEventThunk <USES esi edi, peventObj:DWORD, pThunk:DWORD, pHandler:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 EventThunk
	mov	ecx, EventTHUNKSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	eax, [pThunk]
	mov	[eax+THUNKADDR_OFF], eax

	mov	edi, [peventObj]
	mov	[eax+THIS_OFF], edi

	mov	edi, [pHandler]
	mov	esi, [peventObj]
	call	resolveIndirect
	sub	edi, [pThunk]
	sub	edi, HANDLER_OFF+4
	mov	[eax+HANDLER_OFF], edi

	inc	eax
	ret
ENDCPROC createThreadEventThunk 


VxD_LOCKED_CODE_ENDS

	end
