;; vcputil.asm - utility routines for VCOMM ports and port driver
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
	.386P

ifdef WIN40
WIN40SERVICES equ 1
endif
	.xlist
	NO_SEGMENTS=1
	include VMM.INC
	include VSEGMENT.INC
	include CDEFNS.INC

VxD_LOCKED_CODE_SEG

	EXTCFUNC resolveIndirect

	align 4
portdrvrThunk:
	cld
	PassThisCdecl			;; Pass -this- 
THIS_OFF = offset32 $ -  4 - offset32 portdrvrThunk
	db	0E8h, 0,0,0,0		;; CALL rel32	
RELHANDLER_OFF = offset32 $ - 4 - offset32 portdrvrThunk
	lea	esp, [esp+4]		;; pop this
	ret
	align 4
portdrvrThunkSIZE  = offset32 $ - offset32 portdrvrThunk
.errnz portdrvrThunkSIZE-10h ;; Thunk size (portdrvrThunkSIZE) has changed. Update here and VCOMPORT.H

	align 4
portFuncThunk:
	cld
	mov	[esp+4], dword ptr 80000000h ;; replace hPort arg with this
THIS_OFF2 = offset32 $ -  4 - offset32 portFuncThunk
	db	0E9h, 0,0,0,0		;; JMP rel32	
RELHANDLER_OFF2 = offset32 $ - 4 - offset32 portFuncThunk
	align 4
portFuncThunkSIZE  = offset32 $ - offset32 portFuncThunk
.errnz portFuncThunkSIZE-10h ;; Thunk size (portFuncThunkSIZE) has changed. Update here and VCOMPORT.H


;; Function
;;	createPortDrvrThunk - create a thunk for invocation of 
;;				a VCommPortDriver member function
;;
;; Syntax
;;	PVOID createPortDrvrThunk(VCommPortDriver* pdr,BYTE* pThunk, (VOID* handler)());
;;
;; Parameters
;;	pdr		pointer to VCOMM Port driver object; becomes "this" for handler
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
DEFCPROC createPortDrvrThunk <USES esi edi, pdrObj:DWORD, pThunk:DWORD,	pHandler:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 portdrvrThunk
	mov	ecx, portdrvrThunkSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	eax, [pThunk]

	mov	edi, [pdrObj]
	mov	[eax+THIS_OFF], edi

	mov	edi, [pHandler]
	sub	edi, eax
	sub	edi, RELHANDLER_OFF+4
	mov	[eax+RELHANDLER_OFF], edi

	ret
ENDCPROC createPortDrvrThunk	

ifdef __BORLANDC__
	createPortDrvrThunk equ _createPortDrvrThunk
endif

_createDCThunk 		= createPortDrvrThunk
_createOpenThunk 	= createPortDrvrThunk
_createCloseThunk	= createPortDrvrThunk

	public _createDCThunk
	public _createOpenThunk
	public _createCloseThunk

DEFCPROC createPortFunctionThunk <USES esi edi, pObject:DWORD, pBaseObj:DWORD, pThunk:DWORD, bUseBase:DWORD, pFunc:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 portFuncThunk
	mov	ecx, portFuncThunkSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	eax, [pThunk]
	mov	edi, [pObject]
	mov	[eax+THIS_OFF2], edi

	;; Resolve the virtual member function thunks to obtain the 
	;; actual member function addresses for the current object
	;; and an instance of the base object. In the case where they are ,
	;; identical,i.e., there is no override, the value returned depends
	;; on the bUseBase parameter. If it is non-zero (TRUE), then the
	;; address of the base class' member function is returned. Otherwise,
	;; NULL is returned. If the member function addresses are not the 
	;; same, then this function returns the address of the member function 
	;; of the current object and inserts that address into the thunk.

	mov	edi, [pFunc]			; virtual member thunk ptr
	mov	esi, [pBaseObj]			; address of base class object
	call	resolveIndirect		; edi <- member address
	mov	ecx, edi				; save member ptr in ecx
	mov	edi, [pFunc]			; virutal member thunk ptr
	mov	esi, [pObject]			; address of current object
	call	resolveIndirect		; edi <- member address

	mov	edx, [pThunk]			; edx <- return value
	cmp	ecx, edi				; compare to that of base class
	jne	short cpft_insert		; if equal, no override

	cmp	[bUseBase], 0			; is base address OK?
	jne	cpft_insert				; jump if so
	xor	edx, edx				; otherwise return NULL

cpft_insert:
	mov	eax, [pThunk]			; eax <- pointer to thunk
	sub	edi, eax				; make relative address
	sub	edi, RELHANDLER_OFF2+4	; ditto
	mov	[eax+RELHANDLER_OFF2], edi	; insert resolved addr
	jmp	short cpft_exit			; success

cpft_exit:
	mov	eax, edx
	ret
ENDCPROC createPortFunctionThunk 

VxD_LOCKED_CODE_ENDS
	end
