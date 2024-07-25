;; vshwiutl.asm - utility routines for shared hardware interrrupt handler
;;                thunks
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
	include VPICD.INC
	include CDEFNS.INC

VxD_LOCKED_CODE_SEG

	EXTRN C getVTabOffset:NEAR

	DUMMY equ 80000000h		;; an arbitrary 32-bit value

;; Shared Hardware Interrupt Thunk
;;
;; This thunk is used by class VSharedHardwareInt.

	align 4
HWIThunk	Label DWORD
OnHardIntThunk:				;; Thunk for OnSharedHardwareInt
	cld
	push	ebx
	PASSTHIS				;; Pass -this- : compiler dependent
THIS_OFF1 = offset32 $ -  4 - offset32 HWIThunk
	db		0E8h, 0,0,0,0	;; call relative near handler
HANDLER_OFF1 = offset32 $ - 4 - offset32 HWIThunk
	bt	eax, 0			;; set carry on BOOLEAN eax
	cmc
	ret

	align 4

OnVirtIntThunk:				;; Thunk for OnVirtualInt
	xchg	[esp], ebx		;; swap VM Handle, return address
	PASSTHIS
THIS_OFF2 = offset32 $ -  4 - offset32 HWIThunk
	cld
	push	ebx			;; return address
	db	0E9h, 0,0,0,0		;; jump to handler
HANDLER_OFF2 = offset32 $ - 4 - offset32 HWIThunk

	align 4

OnVirtEOIThunk:				;; Thunk for OnVirtualEOI
	xchg	[esp], ebx		;; swap VM Handle, return address
	PASSTHIS
THIS_OFF3 = offset32 $ -  4 - offset32 HWIThunk
	cld
	push	ebx			;; return address
	db	0E9h, 0,0,0,0		;; jump to handler
HANDLER_OFF3 = offset32 $ - 4 - offset32 HWIThunk

	align 4

OnVirtMaskChgThunk:			;; Thunk for OnVirtualMask	
	xchg	[esp], ecx		;; swap mask boolean, return address
	mov	eax, ecx		;; copy return address to eax
	push	ebx			;; push VM handler parameter
	PASSTHIS
THIS_OFF4 = offset32 $ -  4 - offset32 HWIThunk
	cld
	push	eax			;; return address
	db	0E9h, 0,0,0,0		;; jump to handler
HANDLER_OFF4 = offset32 $ - 4 - offset32 HWIThunk

	align 4

OnVirtIRETThunk:			;; Thunk for OnVirtualIRET
	xchg	[esp], ebx		;; swap VM Handle, return address
	PASSTHIS
THIS_OFF5 = offset32 $ -  4 - offset32 HWIThunk
	cld
	push	ebx			;; return address
	db	0E9h, 0,0,0,0		;; jump to handler
HANDLER_OFF5 = offset32 $ - 4 - offset32 HWIThunk

ONHARDINTOFF  = offset32 OnHardIntThunk - offset32 HWIThunk
ONVIRTINTOFF  = offset32 OnVirtIntThunk - offset32 HWIThunk
ONVIRTEOIOFF  = offset32 OnVirtEOIThunk - offset32 HWIThunk
ONVIRTMASKCHGOFF = offset32 OnVirtMaskChgThunk - offset32 HWIThunk
ONVIRTIRETOFF = offset32 OnVirtIRETThunk - offset32 HWIThunk

	align 4
HWITHUNKSIZE  = offset32 $ - offset32 HWIThunk
.errnz HWITHUNKSIZE-58h ;; Thunk size has changed. Fix here and VSHWINT.H.


;; Function
;;	createSharedHWIThunk - create a multi-thunk for invocation of a hardware 
;;			 interrrupt handlers
;;
;; Syntax
;;	VOID createHWIThunk(VHardwareInt* phwi,BYTE* pThunk,
;;				VPICD_IRQ_Descriptor* pIRQDesc)
;;
;; Parameters
;;	phwi		pointer to hardware intr object; becomes "this" for handler
;;	pThunk		pointer to buffer to receive thunk code
;;	pIRQDesc	pointer to IRQ descriptor struct (see vpicd.h)
;;
;; Remarks
;;	This function loads thunk code into the supplied buffer.  There
;;	are five thunks in one -- one for each of the VPICD notifications.
;;	This function updates the IRQ descriptor pointed by parameter pIRQDesc
;;	to point to the thunks.  Any functions passed in as zero are unchanged.
;;
DEFCPROC createSharedHWIThunk <USES esi edi, pHWIObj:DWORD,pThunk:DWORD, pIRQDesc:DWORD>

	;; copy the thunk template to the caller's buffer

	mov	edi, [pThunk]
	mov	esi, offset32 HWIThunk
	mov	ecx, HWITHUNKSIZE/4
	cld
	rep	movsd

	;; set up values in the thunk

	mov	eax, [pThunk]

	mov	edi, [pHWIObj]
	mov	[eax+THIS_OFF1], edi
	mov	[eax+THIS_OFF2], edi
	mov	[eax+THIS_OFF3], edi
	mov	[eax+THIS_OFF4], edi
	mov	[eax+THIS_OFF5], edi

	mov	edi, [pIRQDesc]

	mov	edx, [edi].VID_Hw_Int_Proc
	or	edx, edx
	jz	short @1
	sub	edx, eax
	sub	edx, HANDLER_OFF1+4
	mov	[eax+HANDLER_OFF1], edx
	lea	edx, [eax + ONHARDINTOFF]
	mov	[edi].VID_Hw_Int_Proc, edx
@1:
	mov	edx, [edi].VID_Virt_Int_Proc
	or	edx, edx
	jz	short @2
	sub	edx, eax
	sub	edx, HANDLER_OFF2+4
	mov	[eax+HANDLER_OFF2], edx
	lea	edx, [eax + ONVIRTINTOFF]
	mov	[edi].VID_Virt_Int_Proc, edx
@2:
	mov	edx, [edi].VID_EOI_Proc
	or	edx, edx
	jz	short @3
	sub	edx, eax
	sub	edx, HANDLER_OFF3+4
	mov	[eax+HANDLER_OFF3], edx
	lea	edx, [eax + ONVIRTEOIOFF]
	mov	[edi].VID_EOI_Proc, edx
@3:
	mov	edx, [edi].VID_Mask_Change_Proc
	or	edx, edx
	jz	short @4
	sub	edx, eax
	sub	edx, HANDLER_OFF4+4
	mov	[eax+HANDLER_OFF4], edx
	lea	edx, [eax + ONVIRTMASKCHGOFF]
	mov	[edi].VID_Mask_Change_Proc, edx
@4:
	mov	edx, [edi].VID_IRET_Proc
	or	edx, edx
	jz	short @5
	sub	edx, eax
	sub	edx, HANDLER_OFF5+4
	mov	[eax+HANDLER_OFF5], edx
	lea	edx, [eax + ONVIRTIRETOFF]
	mov	[edi].VID_IRET_Proc, edx
@5:

	ret
ENDCPROC createSharedHWIThunk 


VxD_LOCKED_CODE_ENDS

	end

