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

VxD_CODE_SEG
	EXTRN	PC98toCIRRUS:NEAR
	EXTRN	CIRRUStoPC98:NEAR
	EXTRN	SetLCD_OFF:NEAR
	EXTRN	SetLCD_ON:NEAR
	EXTRN	Clear_Screen:NEAR
VxD_CODE_ENDS


VxD_DATA_SEG
	EXTRN	VACL_CB_Offset:DWORD
	EXTRN	SessionMode:WORD
	EXTRN	PRODUCT_ID:BYTE
	EXTRN	Graphics_Controller:DWORD
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

VxD_DATA_SEG

PUBLIC	ABE_PhysWindow, ABE_PageWindow
PUBLIC	BiosInfo_ACC, ModeInfo_ACC

ABE_PhysWindow	dd	?
ABE_PageWindow	dd	?

BiosInfo_ACC	label	byte
	db	0010b					; BiosCaps	Support VESA BIOS
	db	0100b					; SvgaCaps	Only Window Access
	dw	0					; VenderID	NEC
	dw	0100b					; ChipID	Cirrus
	db	?	; Device_Init			; ChipNum
	db	?	; Device_Init			; BusID
.errnz	$-BiosInfo_ACC-(size BiosInfoBlock)

ModeInfo_ACC	label	byte
	db	00000000b				; TextCaps	Not support text
	db	1					; CursorCaps
	db	ABE_CursorSizeX				; CursorSizeX
	db	ABE_CursorSizeY				; CursorSizeY
.errnz	$-ModeInfo_ACC-(size ExtInfoBlock)

VxD_DATA_ENDS



;******************************************************************************
;			       M A I N	 C O D E
;******************************************************************************

VxD_CODE_SEG



;******************************************************************************
; ABE_GetCapabilities
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	ESI = address to BiosInfoBlock
;	ECX = size of BiosInfoBlock structure
;
;USES:	Flags, ECX, ESI, EAX
;
BeginProc ABE_GetCapabilities, PUBLIC

	mov	esi, OFFSET32 BiosInfo_ACC
	mov	ecx, size BiosInfoBlock

	xor	eax, eax
	cmp	[PRODUCT_ID], vID_B3	; B3 ?
	jne	@f
	or	eax, 1
@@:
	ret

EndProc ABE_GetCapabilities


;******************************************************************************
; ABE_GetExtModeInfo
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	CX  = number of video mode
;	EDI = address to buffer (ExtInfoBlock)
;
;EXIT:	CF  = 0:Successful, 1:Invalid video mode number
;
;USES:	Flags
;
BeginProc ABE_GetExtModeInfo, PUBLIC

	pushad

	cmp	cx, vVM_480_16M			; Q: 16M ?
	jz	ABE_GEI_Error

	mov	esi, OFFSET32 ModeInfo_ACC

	mov	ecx, size ExtInfoBlock
	cld
	rep movsb
	xor	al, al
	mov	ecx, 256-(size ExtInfoBlock)
	rep stosb
	popad
	clc
	ret

ABE_GEI_Error:
	popad
	stc
	ret

EndProc ABE_GetExtModeInfo


;******************************************************************************
; ABE_EnableBIOS
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	CF  = 0:Successful, 1:Error
;
;USES:	Flags
;
BeginProc ABE_EnableBIOS, PUBLIC

	pushad
	mov	edi, ebx
	add	edi, [VACL_CB_Offset]
	SetFlag	[edi.ABE_Flags], fABE_Enable

	mov	[SessionMode], vSM_480_256
	mov	[edi.ABE_Video_Mode], vVM_480_256	; Default 640x480x256
	mov	[edi.ABE_SessionMode], vSM_480_256


	mov	[edi.ABE_BytesPerScanline], 640
	xor	edx, edx
	mov	[edi.ABE_DisplayStartX], dx
	mov	[edi.ABE_DisplayStartY], dx
	mov	[edi.ABE_Window_Pos], dx

	call	SetLCD_OFF				; Set LCD BackLight OFF
	call	PC98toCIRRUS
	call	Clear_Screen
	call	SetLCD_ON				; Set LCD BackLight ON

	mov	edx, Graphics_Controller		; Index
	mov	al, 09h					; Offset Register 0 (GR9)
	out	dx, al					; Data
	inc	dx
	xor	al, al
	out	dx, al

	mov	eax, [ABE_PageWindow]		; Physical page number
	xor	ecx, ecx			; A800-
	mov	edx, 10h			; 16pages(64K)
	VxDCall	VDD_Map_VRAM
	popad
	clc
	ret

EndProc ABE_EnableBIOS


;******************************************************************************
; ABE_DisableBIOS
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	CF  = 0:Successful, 1:Error
;
;USES:	Flags
;
BeginProc ABE_DisableBIOS, PUBLIC

	pushad
	mov	edi, ebx
	add	edi, [VACL_CB_Offset]
	ClrFlag	[edi.ABE_Flags], fABE_Enable

	call	SetLCD_OFF			; Set LCD BackLight OFF
	call	CIRRUStoPC98
	call	SetLCD_ON			; Set LCD BackLight ON

	xor	eax, eax			; Unmap
	VxDCall	VDD_Map_VRAM
	popad
	clc
	ret

EndProc ABE_DisableBIOS


;******************************************************************************
; ABE_GetFlatSelector
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	CF  = 0:Successful, 1:Error
;	CX  = Size
;	DX  = Selector
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc ABE_GetFlatSelector, PUBLIC

	cmp	[ebp.Client_CL], 1
	jnz	short @f

	mov	eax, [ABE_PhysWindow]
	mov	ecx, 10000h			; 64K
	VMMcall	Map_Lin_To_VM_Addr		; cx=sel, edx=off
	jc	short @f
	mov	dx, cx
	mov	ecx, 64
	ret

@@:	stc
	ret

EndProc ABE_GetFlatSelector


;******************************************************************************
; ACC_WinFunc
;
;DESCRIPTION:
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	None
;
;USES:	Flags
;
BeginProc ACC_WinFunc, PUBLIC

	xor	ecx, ecx
	mov	eax, [ebp.Client_EBX]
	shr	al, 1					; Q: Window A/B ?
	jnz	short ACC_WinFunc_Exit			;    N: Error
	rcl	ecx, 1					;    Y: ECX = A:0 / B:2
	shr	ah, 1					; Q: Window Set/Get ?
	jnz	short ACC_WinFunc_Exit			;    N: Error
	jnc	short ACC_WinFunc_0500			;    Set:
							;    Get:
ACC_WinFunc_0501:
	mov	ax, [edi.ABE_Window_Pos]		; Get Window position from CB
	mov	[ebp.Client_DX], ax			; Set Client Reg.
	jmp	short ACC_WinFunc_Exit			; Exit

ACC_WinFunc_0500:
	movzx	eax, [ebp.Client_DX]
	or	ah, ah
	jnz	short ACC_WinFunc_Exit			; Error
	mov	[edi.ABE_Window_Pos], ax		;
	mov	ah, al

	mov	edx, Graphics_Controller		; Index
	mov	al, 09h					; Offset Register 0 (GR9)
	out	dx, al					; Data
	inc	dx
	mov	al, ah
	out	dx, al

ACC_WinFunc_Exit:
	VMMCall	Simulate_Far_Ret
	ret

EndProc ACC_WinFunc


VxD_CODE_ENDS


	END
