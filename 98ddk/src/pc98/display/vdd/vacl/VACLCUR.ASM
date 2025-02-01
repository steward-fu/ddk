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

	EXTRN	Sequencer:DWORD
	EXTRN	SequencerData:DWORD
	EXTRN	DAC_Write_Index:DWORD
	EXTRN	DAC_Write_Data:DWORD
	EXTRN	Graphics_Controller:DWORD
bSaveBankNo	db	?
dwTempReg	dd	?
VxD_DATA_ENDS



;******************************************************************************
;				 E Q U A T E S
;******************************************************************************

CL_CURSOR_ATT	equ	0012h
CL_CURSOR_ENB	equ	00000001b
CL_EXT_DAC_ENB	equ	00000010b
CL_CURSOR_SIZE	equ	00000100b



;******************************************************************************
;			 D A T A   S T R U C T U R E S
;******************************************************************************



;******************************************************************************
;				  M A C R O S
;******************************************************************************



;******************************************************************************
;			      L O C A L	  D A T A
;******************************************************************************

;VxD_DATA_SEG
;Hot_X		dw	0
;Hot_Y		dw	0
;VxD_DATA_ENDS



;******************************************************************************
;			       M A I N	 C O D E
;******************************************************************************

VxD_CODE_SEG



;******************************************************************************
; ABE_SetCursor
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	ESI = address to cursor data
;
;EXIT:	CF  = 0:Successful, 1:Error
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc ABE_SetCursor, PUBLIC

	mov	edx, ebx
	add	edx, [VACL_CB_Offset]

	test	[edx.ABE_Flags], fABE_Enable
	jz	ABE_SetCursor_Error

	cmp	word ptr [edx.ABE_Video_Mode], vVM_480_16M	; Q: 16M ?
	je	ABE_SetCursor_Error			;   Y: Error

; Copy cursor shape to VACL_CB
	mov	dwTempReg, edi
	push	esi
	mov	edi, edx
	add	edi, ABE_Cursor_Shape
	cld
	mov	ecx, ((ABE_CursorSizeX/8)*ABE_CursorSizeY)*2
	rep movsb

; H/W Setup for copy cursor shape to VRAM (H/W cursor pattern area)

	pop	esi
public	set_hw_cursor
set_hw_cursor:
	push	esi
	push	edx

	mov	edx, Sequencer
	mov	ax, 01206h
	out	dx, ax			;unlock extended register

	mov	edx, Graphics_Controller
	mov	al, 09h
	out	dx, al
	inc	dx
	in	al, dx
	mov	bSaveBankNo, al
	mov	al, 0f0h
	out	dx, al			;Set bank for H/W cursor pattern

	mov	edx, Sequencer
	mov	ax, 3d13h
	out	dx, ax			;Select H/W Cursor No.62

	pop	edx

; Copy cursor shape to VRAM (H/W cursor pattern area)
	mov	ecx, (((ABE_CursorSizeX/8)*ABE_CursorSizeY)*2)/4
	mov	edi, [ebx.CB_High_Linear]
	add	edi, 0a8000h+0fd00h	;Point to No.62 cursor pattern area
	rep	movsd

	mov	edx, Graphics_Controller
	mov	al, 09h
	mov	ah, bSaveBankNo
	out	dx, ax


	pop	esi
	mov	edi, dwTempReg


; Enable Ext DAC
	mov	edx, Sequencer
	mov	al, CL_CURSOR_ATT
	out	dx, al
	mov	edx, SequencerData
	in	al, dx
	or	al, CL_EXT_DAC_ENB
	out	dx, al
; Cursor off
	mov	edx, Sequencer
	mov	al, CL_CURSOR_ATT
	out	dx, al
	mov	edx, SequencerData
	in	al, dx
	and	al, NOT CL_CURSOR_ENB
	out	dx, al
;
	mov	al, 0
	mov	edx, DAC_Write_Index			;RAMDAC INDEX
	out	dx, al					;load it
	mov	edx, DAC_Write_Data			;RAMDAC DATA
	mov	al, 0
	out	dx, al					;write red
	out	dx, al					;write green
	out	dx, al					;write blue
	mov	al, 0ffh
	mov	edx, DAC_Write_Index			;RAMDAC INDEX
	out	dx, al					;load it
	mov	edx, DAC_Write_Data			;RAMDAC DATA
	mov	al, 03Fh
	out	dx, al					;write red
	out	dx, al					;write green
	out	dx, al					;write blue
; Disable Ext DAC
	mov	edx, Sequencer
	mov	al, CL_CURSOR_ATT
	out	dx, al
	mov	edx, SequencerData
	in	al, dx
	and	al, NOT CL_EXT_DAC_ENB
	out	dx, al

ABE_SetCursor_Success:
	clc
	ret

ABE_SetCursor_Error:
	stc
	ret

EndProc ABE_SetCursor


;******************************************************************************
; ABE_ShowCursor
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	CF  = 0:Successful, 1:Error
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc ABE_ShowCursor, PUBLIC

	mov	edx, ebx
	add	edx, [VACL_CB_Offset]
	test	[edx.ABE_Flags], fABE_Enable
	jz	short ABE_ShowCursor_Error

	cmp	word ptr [edx.ABE_Video_Mode], vVM_480_16M	; Q: 16M ?
	je	short ABE_ShowCursor_Error			;   Y: Error

	mov	byte ptr [edx.ABE_Cursor_Enable], HWCursorOn

	mov	edx, Sequencer
	mov	al, CL_CURSOR_ATT
	out	dx, al

	mov	edx, SequencerData
	in	al, dx
	and	al, NOT CL_CURSOR_SIZE	; Graphics Cursor will be 32x32 pixels
	or	al, CL_CURSOR_ENB
	out	dx, al

ABE_ShowCursor_Success:
	clc
	ret

ABE_ShowCursor_Error:
	stc
	ret

EndProc ABE_ShowCursor


;******************************************************************************
; ABE_HideCursor
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	CF  = 0:Successful, 1:Error
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc ABE_HideCursor, PUBLIC

	mov	edx, ebx
	add	edx, [VACL_CB_Offset]
	test	[edx.ABE_Flags], fABE_Enable
	jz	short ABE_HideCursor_Error

	cmp	word ptr [edx.ABE_Video_Mode], vVM_480_16M	; Q: 16M ?
	je	short ABE_HideCursor_Error			;   Y: Error

	mov	byte ptr [edx.ABE_Cursor_Enable], HWCursorOff

	mov	edx, Sequencer
	mov	al, CL_CURSOR_ATT
	out	dx, al

	mov	edx, SequencerData
	in	al, dx
	and	al, NOT CL_CURSOR_ENB
	out	dx, al

ABE_HideCursor_Success:
	clc
	ret

ABE_HideCursor_Error:
	stc
	ret

EndProc ABE_HideCursor


;******************************************************************************
; ABE_SetCursorPos
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	CX  = X position
;	DX  = Y position
;
;EXIT:	CF  = 0:Successful, 1:Error
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc ABE_SetCursorPos, PUBLIC

	push	edi
	mov	edi, ebx
	add	edi, [VACL_CB_Offset]

	test	[edi.ABE_Flags], fABE_Enable
	jz	ABE_SetCursorPos_Error

	mov	eax, ((640 SHL 16)+480)
	cmp	word ptr [edi.ABE_Video_Mode], vVM_480_256	; 640x480x256
	je	@f
	cmp	word ptr [edi.ABE_Video_Mode], vVM_480_64K	; 640x480x64K
	je	@f

	mov	eax, ((800 SHL 16)+600)
	cmp	word ptr [edi.ABE_Video_Mode], vVM_600_256	; 800x600x256
	je	@f
	cmp	word ptr [edi.ABE_Video_Mode], vVM_600_64K	; 800x600x64K
	je	@f

	mov	eax, ((1024 SHL 16)+768)
	cmp	word ptr [edi.ABE_Video_Mode], vVM_768_256	; 1024x768x256
	jne	ABE_SetCurSorPos_Error				; Unknown video mode

@@:	cmp	ax, dx
	jae	@f
	mov	dx, ax
	dec	dx

@@:	shr	eax, 16
	cmp	ax, cx
	jae	@f
	mov	cx, ax
	dec	dx

@@:	cmp	dx, 0
	jae	@f
	xor	dx, dx

@@:	cmp	cx, 0
	jae	@f
	xor	cx, cx

@@:	mov	word ptr [edi.ABE_Cursor_X], cx
	mov	word ptr [edi.ABE_Cursor_Y], dx

	mov	di, cx
	mov	si, dx

; save and restore the sequencer index to avoid problems.
; We were seeing problems on blitter driver where SR8 was being
; destroyed.
	mov	edx, Sequencer
	in	al, dx
	mov	bl, al			; save sequencer index in bl

	mov	ax, di			; set x coordinate
	shl	ax, 5
	or	al, 10h
	out	dx, ax

	mov	edx, Sequencer
	mov	ax, si			; set y coordinate
	shl	ax, 5
	or	al, 11h
	out	dx, ax

	mov	edx, Sequencer
	mov	al, bl			; restore sequencer index
	out	dx, al

ABE_SetCurSorPos_Success:
	pop	edi
	clc
	ret

ABE_SetCurSorPos_Error:
	pop	edi
	stc
	ret

EndProc ABE_SetCursorPos


;******************************************************************************
; ABE_GetCurSorPos
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	CF  = 0:Successful, 1:Error
;	CX  = X position
;	DX  = Y position
;
;USES:	Flags, ECX, EDX
;
BeginProc ABE_GetCurSorPos, PUBLIC

	mov	ecx, ebx
	add	ecx, [VACL_CB_Offset]
	test	[ecx.ABE_Flags], fABE_Enable
	jz	short ABE_GetCursorPos_Error

	cmp	word ptr [ecx.ABE_Video_Mode], vVM_480_16M	; Q: 16M ?
	je	short ABE_GetCursorPos_Error			;   Y: Error

	mov	dx, word ptr [ecx.ABE_Cursor_Y]
	mov	cx, word ptr [ecx.ABE_Cursor_X]

ABE_GetCurSorPos_Success:
	clc
	ret

ABE_GetCurSorPos_Error:
	stc
	ret

EndProc ABE_GetCurSorPos


VxD_CODE_ENDS


	END
