;; vthkutil.asm - general utility routines for thunks
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

;; Function
;;	getVTabOffset - get the offset in the virtual table of a class
;;			instance for a particular function
;;
;; Syntax
;;	LONG getVTabOffset(PFN pFunc)
;;
;; Parameters
;;	pFunc	pointer to the virtual member function
;;
;; Remarks
;;	The compiler generates thunks to dispatch to virtual member
;;	functions.  The thunks jump through a table of function pointers
;;	(the vtab).  By parsing the thunk pointed to by pFunc, we can
;;	determine at what offset in the vtab to find the ultimate function
;;	address.
;;
;;	There are four known thunk types for MS C8-9/32:
;;
;;		1) mov eax,[ecx]	; Zero offset 20FF018Bh
;;		   jmp [eax]
;;		2) mov eax,[ecx]	; Byte offset 60FF018Bh nn
;;		   jmp [eax+nn]
;;		3) mov eax,[esp+4]	; _cdecl zero offset 8b 44 24 04
;;		   mov eax, [eax]	; 8b 00
;;		   jmp [eax]		; ff 20 
;;		4) mov eax,[esp+4]	; _cdecl non-zero offset 8b 44 24 04
;;		   mov eax, [eax]	; 8b 00
;;		   jmp [eax+nn]		; ff 60 nn 
;;
;;	Known Borland thunk types
;;
;;		1) mov ecx,[esp+4]
;;		   mov ecx,[ecx+nn]
;;		   jmp ecx
;;		2) mov ecx,[esp+4]
;;		   mov ecx,[ecx]
;;		   jmp [ecx+nn]
;;
;;	IMPORTANT: this function may only clobber EAX - must preserve
;;	all other registers on exit.
;;
;; Return value
;;	Returns the offset in the vtab of the given function. If the thunk
;;	cannot be parsed, returns -1.
;;
ifndef __BORLANDC__
_getVTabOffset:
endif
DEFCPROC getVTabOffset <pFunc:DWORD>
	mov	eax, [pFunc]
	cmp	dword ptr [eax], 60FF018Bh	; non-zero offset
	jne	short @1
	movzx	eax, byte ptr [eax+4]
	jmp	short gvtoexit
@1:
	cmp	dword ptr [eax], 20FF018Bh	; zero offset
	jne	short @2
	xor	eax, eax
	jmp	short gvtoexit
@2:
	cmp	dword ptr [eax], 04244C8Bh	; _cdecl mov ecx,[esp+4]
	jne	short @4
	cmp	word ptr [eax+4], 498Bh		; mov ecx,[ecx+xx]
	jne	short @3
	movzx	eax, byte ptr [eax+6]
	jmp	short gvtoexit
@3:
	cmp	word ptr [eax+4], 098Bh		; mov ecx,[ecx]
	jne	short unknownThunk
	cmp	word ptr [eax+6], 61FFh		; jmp [ecx+nn]
	je	short @31
	cmp	word ptr [eax+6], 21FFh		; jmp [ecx]
	jne	short unknownThunk
	xor	eax, eax
	jmp	short gvtoexit
@31:
	movzx	eax, byte ptr [eax+8]
	jmp	short gvtoexit
	
@4:
	cmp	dword ptr [eax], 0424448Bh	; _cdecl mov eax,[esp+4]
	jne	short unknownThunk
	cmp	dword ptr [eax+4], 60ff008bh	; _cdecl non-zero offset
	jne	short @5
	movzx	eax, byte ptr [eax+8]
	jmp	short gvtoexit
@5:
	cmp	dword ptr [eax+4], 20ff008bh	; _cdecl zero offset
	jne	unknownThunk
	xor	eax, eax
	jmp	gvtoexit

unknownThunk:
	int 3					; contact Compuware
	mov	eax, -1
	jmp	 gvtoexit

gvtoexit:
	ret
ENDCPROC getVTabOffset


;; Function
;;	resolveIndirect
;;
;; Parameters
;; 	edi = virtual function call thunk address
;; 	esi = object address
;;
;; Remarks
;;	Use this function to resolve the indirection of a virtual
;;	function call thunk.  This allows us to bind a handler directly
;;	at the time of the hook, thereby eliminating the overhead of
;;	the virtual function call thunk at dispatch time.
;;
;; Returns
;;	edi = virtual function address
;;	
;;
DEFCPROC resolveIndirect <>
	push	eax

	push	edi	    	;; first get offset in Vtab of function
	call	_getVTabOffset	;;   address by examining thunk code
	add	esp, 4

	cmp	eax, -1		;; if the thunk is not readable, bail out
	je	short ri_fail

	mov	edi, [esi]	;; edi <- VTab base
	mov	edi, [eax+edi]	;; edi <- virtual function address

ri_exit:
	pop	eax
	ret

ri_fail:

ifdef DEBUG
	int 3
endif
	jmp	ri_exit

ENDCPROC resolveIndirect 

VxD_LOCKED_CODE_ENDS

	end
