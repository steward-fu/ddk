;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************
PAGE	80,132

	.386p

;******************************************************************************
;				I N C L U D E S
;******************************************************************************

	MINIVDD EQU True

	.XLIST
	INCLUDE VMM.INC
	INCLUDE OPTTEST.INC
	INCLUDE MINIVDD.INC
	INCLUDE VDD98.INC
	INCLUDE VDDACC.INC
	INCLUDE VACL.INC
	INCLUDE DEBUG.INC
	.LIST



;******************************************************************************
;			       E X T E R N A L S
;******************************************************************************

VxD_DATA_SEG
	EXTRN	VACL_CB_Offset:DWORD

	EXTRN	DAC_Write_Index:DWORD
	EXTRN	DAC_Write_Data:DWORD
VxD_DATA_ENDS



;******************************************************************************
;				 E Q U A T E S
;******************************************************************************



;******************************************************************************
;			 D A T A   S T R U C T U R E S
;******************************************************************************



;******************************************************************************
;				  M A C R O S
;******************************************************************************



;******************************************************************************
;			      L O C A L	  D A T A
;******************************************************************************



;******************************************************************************
;			       M A I N	 C O D E
;******************************************************************************

VxD_CODE_SEG



;******************************************************************************
; ABE_SetPalette
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	AL  = Index
;	ECX = Color (x.B.R.G)
;
;EXIT:	CF  = 0:Successful, 1:Error
;
;USES:	Flags, EAX(AL), ECX, EDX
;
BeginProc ABE_SetPalette, PUBLIC

	mov	edx, ebx
	add	edx, [VACL_CB_Offset]
	test	[edx.ABE_Flags], fABE_Enable
	jz	short ABE_SetPalette_Error

	cmp	word ptr [edx.ABE_Video_Mode], vVM_1024_256	; Q: 256 ?
	jg	short ABE_SetPalette_Error			;  N: Error

	cmp	word ptr [edx.ABE_Video_Mode], vVM_600_16
	je	short ABE_SetPalette_Error
	cmp	word ptr [edx.ABE_Video_Mode], vVM_768_16
	je	short ABE_SetPalette_Error
	cmp	word ptr [edx.ABE_Video_Mode], vVM_1024_16
	je	short ABE_SetPalette_Error

	and	ecx, 0FFFFFFh		; mask RGB value

	mov	edx, eax
	and	edx, 0FFh		; mask palette index
	shl	edx, 2			; palette index x4

	add	edx, ebx
	add	edx, [VACL_CB_Offset]

	mov	dword ptr [edx.ABE_Palette], ecx

	mov	edx, DAC_Write_Index
	out	dx, al			; write palette index

	shr	ecx, 2
	and	ecx, 3F3F3Fh		; 8bits -> 6bits

	mov	edx, DAC_Write_Data

	mov	al, ch			; Red value
	out	dx, al

	mov	al, cl			; Green value
	out	dx, al

	shr	ecx, 8
	mov	al, ch			; Bule value
	out	dx, al

ABE_SetPalette_Success:
	clc
	ret

ABE_SetPalette_Error:
	stc
	ret

EndProc ABE_SetPalette


;******************************************************************************
; ABE_GetPalette
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	AL  = Index
;
;EXIT:	CF  = 0:Successful, 1:Error
;	ECX = Color (x.B.R.G)
;
;USES:	Flags, ECX
;
BeginProc ABE_GetPalette, PUBLIC

	mov	edx, ebx
	add	edx, [VACL_CB_Offset]
	test	[edx.ABE_Flags], fABE_Enable
	jz	short ABE_GetPalette_Error

	cmp	word ptr [edx.ABE_Video_Mode], vVM_1024_256	; Q: 256 ?
	jg	short ABE_GetPalette_Error			;  N: Error

	cmp	word ptr [edx.ABE_Video_Mode], vVM_600_16
	je	short ABE_GetPalette_Error
	cmp	word ptr [edx.ABE_Video_Mode], vVM_768_16
	je	short ABE_GetPalette_Error
	cmp	word ptr [edx.ABE_Video_Mode], vVM_1024_16
	je	short ABE_GetPalette_Error

	xor	ecx, ecx
	mov	cl, al
	shl	ecx, 2			; palette index x4

	add	ecx, ebx
	add	ecx, [VACL_CB_Offset]

	mov	ecx, dword ptr [ecx.ABE_Palette]

ABE_GetPalette_Success:
	clc
	ret

ABE_GetPalette_Error:
	stc
	ret

EndProc ABE_GetPalette


VxD_CODE_ENDS


	END
