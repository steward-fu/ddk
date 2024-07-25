;; vpipeutl.asm - utility routines for VPipe
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
;;	_pipe_lock - lock the pipe
;;
;; Syntax
;;	BOOL _pipe_lock(BOOL* pFlag)
;;
;; Parameters
;;	pFlag	pointer to dword containing guard bit (LSB)
;;
;; Returns
;;	Returns TRUE if the pipe is successfully locked.
;;	Returns FALSE if the pipe was already locked.
;;
DEFCPROC _pipe_lock <pFlag:DWORD>
	xor	eax, eax		;; clear result
	mov	ecx, [pFlag]		;; get flag pointer
	bts	dword ptr [ecx], 0	;; set carry if already allocated
	cmc				;; set carry if not already allocated
	rcl	eax, 1			;; carry flag to eax lsb
	ret	
ENDCPROC _pipe_lock

VxD_LOCKED_CODE_ENDS

	end
