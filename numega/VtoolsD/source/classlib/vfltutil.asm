;; vfltutil.asm - utility routines for fault thunks
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

;; Fault Thunk
;;
;; This thunk is used by classes VV86ModeFault, VProtModeFault, VVMMFault,
;; and VNMIEvent.  It calls a handler defined as 
;;
;;	BOOL handler(VMHANDLE, CLIENT_STRUCT*, DWORD iFaultNumber);
;;
;; This fault loads ECX with the "this" pointer provided when the thunk
;; is created just prior to invoking the handler.

	align 4
faultThunk:

prevVec	dd	DUMMY			;; store previous vector (if any) here

	jmp	short FT
	jmp	dword ptr [prevVec]
FT:
	push	eax			;; save regs
	push	ecx
	push	edx
	push	ebp				
	push	ebx

	push	DUMMY			;; iFault parameter
FAULT_OFF = offset32 $ - 4 - offset32 faultThunk

	push	ebp			;; CLIENT_STRUCT pointer parameter
	push	ebx			;; VM Handle parameter

	PassThis			;; Pass -this- : compiler dependent
THIS_OFF = offset32 $ - 4 - offset32 faultThunk
	cld	
	db	0E8h, 0,0,0,0		;; call relative near handler
HANDLER_OFF = offset32 $ - 4 - offset32 faultThunk

	or	eax, eax		;; set carry on BOOLEAN eax
	jnz	short @@skip
	stc
@@skip:
	pop	ebx			;; restore regs
	pop	ebp
	pop	edx
	pop	ecx
	jnc	short thunkReturn
	mov	eax, DUMMY
PREV_OFF = offset32 $ - 4 - offset32 faultThunk
	or	eax, eax
	jz	short thunkReturn
	xchg	eax, [esp]
	ret
thunkReturn:
	pop	eax
	ret

	align 4

FAULTTHUNKSIZE  = offset $ - offset faultThunk
THUNK_OFF 	= offset32 FT - offset32 faultThunk

.errnz FAULTTHUNKSIZE-40h ;; Thunk size has changed. Update here and VFAULT.H

;; Function
;;	createFaultThunk - create a thunk for invocation of a fault handler
;;
;; Syntax
;;	PVOID createFaultThunk(VFault* pFault,BYTE* pThunk, DWORD iFault,
;;				(VOID* handler)());
;;
;; Parameters
;;	pFault		pointer to fault object; becomes "this" for handler
;;	pThunk		pointer to buffer to receive thunk code
;;	iFault		ordinal of fault
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
DEFCPROC createFaultThunk <USES esi edi, pFaultObj:DWORD, pThunk:DWORD, iFault:DWORD, pHandler:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 faultThunk
	mov	ecx, FAULTTHUNKSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	edi, [pThunk]

	lea	eax, [edi + THUNK_OFF -4]
	mov	[eax], edi

	mov	eax, [iFault]
	mov	[edi+FAULT_OFF], eax

	mov	eax, [pFaultObj]
	mov	[edi+THIS_OFF], eax

	mov	edi, [pHandler]
	mov	esi, [pFaultObj]
	call	resolveIndirect

	mov	eax, [pThunk]
	sub	edi, eax
	sub	edi, HANDLER_OFF+4
	mov	[eax+HANDLER_OFF], edi

	mov	eax, [pThunk]
	add	eax, THUNK_OFF
	ret
ENDCPROC createFaultThunk 

;; Function
;;	initFaultThunk - set up parameters in existing fault thunk
;;
;; Syntax
;;	VOID initFaultThunk(BYTE* pThunk, (VOID* prev)());
;;
;; Parameters
;;	pThunk		pointer to thunk buffer (already created)
;;	prev		pointer to previous handler
;;
;; Remarks
;;	The previous handler is saved for the benefit of the
;;	BeginProc macro. When the HOOK_PROC keyword is used, hooks
;;	can be inserted and removed from the chain.  Note:
;;	HOOK_PROC was not available in version 3.1.
;;
DEFCPROC initFaultThunk <pThunk:DWORD, pPrev:DWORD>
	mov	eax, [pPrev]
	mov	ecx, [pThunk]
	mov	[ecx], eax	;; store prev vector in 1st dword of thunk
	mov	[ecx+PREV_OFF], eax
;; 
;; The HOOK_PROC keyword on the Begin_Proc macro causes two jump
;; instructions to be inserted at the head of the routine.  The dword
;; stored below is inserted into the instruction stream as the
;; flat address of the location holding the address of the previous
;; handler.

	mov	[ecx+8], ecx    ;; See BeginProc macro in vmm.h,
	ret
ENDCPROC initFaultThunk 

;; Function
;;	getFaultUnhookAddress - get address of handler entry in thunk
;;
;; Syntax
;;	PVOID getFaultUnhookAddress(BYTE* pThunk)
;;
;; Parameters
;;	pThunk		pointer to the thunk
;;
;; Remarks
;;	The address passed to the VMM to hook and unhook the client
;;	handler is at a non-zero offset from the start of the thunk.
;;
;; Return Value
;;	Returns the flat address within the thunk that should be passed
;;	to the VMM for purposes of hooking or unhooking.
;;
DEFCPROC getFaultUnhookAddress <pThunk:DWORD>
	mov	eax, [pThunk]
	add	eax, THUNK_OFF
	ret
ENDCPROC getFaultUnhookAddress 

VxD_LOCKED_CODE_ENDS

	end

