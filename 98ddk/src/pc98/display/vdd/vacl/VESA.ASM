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
	EXTRN	Set_GD5428:NEAR
	EXTRN	Clear_Screen:NEAR
	EXTRN	ABE_PowerMgr:NEAR
	EXTRN	SetLCD_ON:NEAR
	EXTRN	SetLCD_OFF:NEAR
VxD_CODE_ENDS


VxD_DATA_SEG
	EXTRN	VACL_CB_Offset:DWORD
	EXTRN	SessionMode:WORD
	EXTRN	Graphics_Controller:DWORD
	EXTRN	CRT_3D4:DWORD
	EXTRN	CRT_3D5:DWORD
	EXTRN	Feature_Control_3DA:DWORD
	EXTRN	attribute_controller:DWORD
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
PUBLIC ACCInfoBlock
ACCInfoBlock	label byte
	db	'VESA'					; VESASignature
	dw	0102h					; VESAVersion		Ver 1.2
	dd	?	; Device_Init			; OEMStringPtr
	dd	0b					; Capabilities		Unchangable DAC Width
	dd	?	; Device_Init			; VideoModePtr
	dw	16					; TotalMemory		1024K
.errnz	$-ACCInfoBlock-(size VGAInfoBlock)

PUBLIC ModeBlock16M
ModeBlock16M	label byte
	dw	11011b					; ModeAttributes	Color Graphics
	db	111b					; WinAAttributes	Readable/Writable
	db	000b					; WinBAttributes	None
	dw	4					; WinGranularity	4K
	dw	64					; WinSize		64K
	dw	0A800h					; WinASegment		A800
	dw	0A800h					; WinBSegment		None
	dd	?	; Device_Init			; WinFuncPtr		Set at Device_Init
	dw	2048					; BytesPerScanline	640x3 bytes/line

	dw	640					; XResolution		640 pixels
	dw	480					; YResolution		480 lines
	db	8	; ?				; XCharSize
	db	16	; ?				; YCharSize
	db	1					; NumberOfPlanes
	db	24					; BitsPerPixel		16M colors
	db	1					; NumberOfBanks
	db	6					; MemoryModel		Direct color
	db	0					; BankSize
	db	0					; NumberOfImagePages
	db	0					; Reserved

	db	8					; RedMaskSize
	db	16					; RedFieldPositins
	db	8					; GreenMaskSize
	db	8					; GreenFieldPosition
	db	8					; BlueMaskSize
	db	0					; BlueFieldPosition
	db	0					; RsvdMaskSize
	db	0					; RsvdFieldPosition
	db	0					; DirectColorModeInfo
.errnz	$-ModeBlock16M-(size ModeInfoBlock)

PUBLIC ModeBlock64K
ModeBlock64K	label byte
	dw	11011b					; ModeAttributes	Color Graphics
	db	111b					; WinAAttributes	Readable/Writable
	db	000b					; WinBAttributes	None
	dw	4					; WinGranularity	4K
	dw	64					; WinSize		64K
	dw	0A800h					; WinASegment		A800
	dw	0A800h					; WinBSegment		None
	dd	?	; Device_Init			; WinFuncPtr		Set at Device_Init
	dw	2048					; BytesPerScanline	640x3 bytes/line

	dw	640					; XResolution		640 pixels
	dw	480					; YResolution		480 lines
	db	8	; ?				; XCharSize
	db	16	; ?				; YCharSize
	db	1					; NumberOfPlanes
	db	24					; BitsPerPixel		16M colors
	db	1					; NumberOfBanks
	db	6					; MemoryModel		Direct color
	db	0					; BankSize
	db	0					; NumberOfImagePages
	db	0					; Reserved

	db	8					; RedMaskSize
	db	16					; RedFieldPositins
	db	8					; GreenMaskSize
	db	8					; GreenFieldPosition
	db	8					; BlueMaskSize
	db	0					; BlueFieldPosition
	db	0					; RsvdMaskSize
	db	0					; RsvdFieldPosition
	db	0					; DirectColorModeInfo
.errnz	$-ModeBlock64K-(size ModeInfoBlock)

PUBLIC ModeBlock256
ModeBlock256	label byte
	dw	11011b					; ModeAttributes	Color Graphics
	db	111b					; WinAAttributes	Readable/Writable
	db	000b					; WinBAttributes	None
	dw	4					; WinGranularity	4K
	dw	64					; WinSize		64K
	dw	0A800h					; WinASegment		A800
	dw	0A800h					; WinBSegment		None
	dd	?	; Device_Init			; WinFuncPtr
	dw	?	; 640/1024			; BytesPerScanline	640/1024 bytes/line

	dw	?	; 640/1024			; XResolution		640/1024 pixels
	dw	?	; 400/480/768			; YResolution		400/480 lines
	db	8	; ?				; XCharSize
	db	16	; ?				; YCharSize
	db	1					; NumberOfPlanes	Packed Pixel
	db	8					; BitsPerPixel		256 colors
	db	1					; NumberOfBanks
	db	4					; MemoryModel		Packed Pixel
	db	0					; BankSize
	db	0					; NumberOfImagePages
	db	0					; Reserved

	db	0					; RedMaskSize
	db	0					; RedFieldPositins
	db	0					; GreenMaskSize
	db	0					; GreenFieldPosition
	db	0					; BlueMaskSize
	db	0					; BlueFieldPosition
	db	0					; RsvdMaskSize
	db	0					; RsvdFieldPosition
	db	0					; DirectColorModeInfo
.errnz	$-ModeBlock256-(size ModeInfoBlock)

VxD_DATA_ENDS



;******************************************************************************
;			       M A I N	 C O D E
;******************************************************************************

VxD_CODE_SEG



;******************************************************************************
; ABE_Int10
;
;DESCRIPTION:
;
;ENTRY:	EAX = 10h
;	EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	CF = 1 indicates interrupt is to be reflected, else it has been
;	     emulated
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_Int10, PUBLIC

	mov	edi, ebx
	add	edi, [VACL_CB_Offset]
	test	[edi.ABE_Flags], fABE_Enable
	jz	short ABE_I10_Error

	cmp	[ebp.Client_AH], 04Fh
	je	ABE_VESA

ABE_I10_Error:
	stc
	ret

EndProc ABE_Int10


;******************************************************************************
; ABE_VESA
;
;DESCRIPTION:
;
;ENTRY:	EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	CF = 1 indicates interrupt is to be reflected, else it has been emulated
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
ABE_VESA_Table	label	dword
	dd	OFFSET32 ABE_VESA_00		; 00: Retuen Super VGA Information
	dd	OFFSET32 ABE_VESA_01		; 01: Return Super VGA Mode Information
	dd	OFFSET32 ABE_VESA_02		; 02: Set Super VGA Video Mode
	dd	OFFSET32 ABE_VESA_03		; 03: Get Super VGA Video Mode
	dd	OFFSET32 ABE_VESA_04		; 04: Save/Restore Super VGA Video State
	dd	OFFSET32 ABE_VESA_05		; 05: CPU Video Memory Window Control
	dd	OFFSET32 ABE_VESA_06		; 06: Set/Get Logical Scanline Length
	dd	OFFSET32 ABE_VESA_07		; 07: Set/Get Display Start
	dd	OFFSET32 ABE_VESA_08		; 08: Set/Get DAC Palette Control
						; 10: Display Power Management Extensions
						; 15: Display Identification Extensions
ABE_VESA_Max	equ	($-ABE_VESA_Table)/4

BeginProc ABE_VESA, PUBLIC

	movzx	eax, [ebp.Client_AL]
	cmp	al, 10h				; Q: Display Power Management Extensions
	je	ABE_VESA_10			;    Y: Go
	cmp	al, ABE_VESA_Max		; Q: Valid/support function
	jae	short ABE_VESA_IRET		;    Y: Go
	jmp	ABE_VESA_Table[eax*4]

ABE_VESA_Success:
	mov	[ebp.Client_AH], 00h		; Successful
ABE_VESA_Error:
	mov	[ebp.Client_AL], 4Fh		; Support
ABE_VESA_IRET:
	clc
	ret

EndProc ABE_VESA


;******************************************************************************
; ABE_VESA_00
;
;DESCRIPTION:
;	Return Super VGA Information
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_00, PUBLIC

	Client_Ptr_Flat edi, es, di		; edi = linear address of Client_ES:DI

	cmp	edi, -1				; -1 if error
	je	ABE_VESA_Error

	mov	esi, OFFSET32 ACCInfoBlock
	mov	ecx, size VGAInfoBlock
	cld
	rep movsb
	xor	al, al
	mov	ecx, 256-(size VGAInfoBlock)
	rep stosb
	jmp	ABE_VESA_Success

EndProc ABE_VESA_00


;******************************************************************************
; ABE_VESA_01
;
;DESCRIPTION:
;	Return Super VGA Mode Information
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_01, PUBLIC

	movzx	eax, [ebp.Client_CX]

	mov	esi, OFFSET32 ModeBlock16M
	cmp	ax, vVM_480_16M			; 640x480x16M
	je	short ABE_VESA_01_Movs

	mov	esi, OFFSET32 ModeBlock64K
	mov	edx, (800 shl 16) + 800
	cmp	ax, vVM_600_64K			; 800x600x64K
	je	short @f
	mov	edx, (480 shl 16) + 640
	cmp	ax, vVM_480_64K			; 640x480x64K
	je	short @f

	mov	esi, OFFSET32 ModeBlock256
	mov	edx, (800 shl 16) + 800
	cmp	ax, vVM_600_256			; 800x600x256
	je	short @f
	mov	edx, (480 shl 16) + 640
	cmp	ax, vVM_480_256			; 640x480x256
	je	short @f
	cmp	ax, vVM_768_256			; 1024x768x256
	jne	ABE_VESA_Error
	mov	edx, (768 shl 16) + 1024
@@:
.errnz	YResolution-XResolution-2
	mov	dword ptr [esi.XResolution], edx
	mov	[esi.BytesPerScanline], dx

ABE_VESA_01_Movs:
	Client_Ptr_Flat edi, es, di		; edi = linear address of Client_ES:DI

	cmp	edi, -1				; -1 if error
	je	ABE_VESA_Error

	mov	ecx, size ModeInfoBlock
	cld
	rep movsb
	xor	al, al
	mov	ecx, 256-(size ModeInfoBlock)
	rep stosb
	jmp	ABE_VESA_Success

EndProc ABE_VESA_01


;******************************************************************************
; ABE_VESA_02
;
;DESCRIPTION:
;	Set Super VGA Video Mode
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_02, PUBLIC

	mov	eax, [ebp.Client_EBX]
	and	ah, 7Fh
	cmp	ah, 01h				; Q: 01xxh
	jnz	ABE_VESA_Error			;   Y:

	mov	edx, 640
	mov	cl, vSM_480_256
	cmp	al, 01h				; 0101h
	jz	short @f

	mov	edx, 800
	mov	cl, vSM_600_256
	cmp	al, 03h				; 0103h
	jz	short @f

	mov	edx, 1280
	mov	cl, vSM_480_64K
	cmp	al, 11h				; 0111h
	jz	short @f

	mov	edx, 1600
	mov	cl, vSM_600_64K
	cmp	al, 14h				; 0114h
	jz	short @f

	mov	edx, 1024
	mov	cl, vSM_768_256
	cmp	al, 05h				; 0105h
	jz	short @f

	mov	edx, 2048
	mov	cl, vSM_480_16M
	cmp	al, 12h				; 0112h
	jnz	ABE_VESA_Error

@@:	xor	ch, ch
	mov	[edi.ABE_SessionMode], cx
	mov	[SessionMode], cx
	mov	[edi.ABE_Video_Mode], ax
	mov	[edi.ABE_BytesPerScanline], edx
	xor	edx, edx
	mov	[edi.ABE_DisplayStartX], dx
	mov	[edi.ABE_DisplayStartY], dx
	mov	[edi.ABE_Window_Pos], dx

	call	SetLCD_OFF

	call	Set_GD5428
	mov	ah, [ebp.Client_BH]
	test	ah, 80h
	jnz	short @f
	call	Clear_Screen
@@:	mov	edx, Graphics_Controller		; Index
	mov	al, 09h					; Offset Register 0 (GR9)
	out	dx, al					; Data
	inc	dx
	xor	al, al
	out	dx, al
	call	SetLCD_ON
	jmp	ABE_VESA_Success

EndProc ABE_VESA_02


;******************************************************************************
; ABE_VESA_03
;
;DESCRIPTION:
;	Get Current Video Mode
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_03, PUBLIC

	mov	ax, [edi.ABE_Video_Mode]
	mov	[ebp.Client_BX], ax
	jmp	ABE_VESA_Success

EndProc ABE_VESA_03


;******************************************************************************
; ABE_VESA_04
;
;DESCRIPTION:
;	Save/Restore Super VGA Video State
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_04, PUBLIC

	jmp	ABE_VESA_IRET

EndProc ABE_VESA_04


;******************************************************************************
; ABE_VESA_05
;
;DESCRIPTION:
;	CPU Video Memory Window Control
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_05, PUBLIC

	mov	eax, [ebp.Client_EBX]
	or	al, al					; Q: Window A ?
	jnz	ABE_VESA_Error				;    N: Error
	shr	ah, 1					; Q: Window Set/Get ?
	jnz	ABE_VESA_Error				;    N: Error
	jnc	short ABE_VESA_0500			;    Set:
							;    Get:
ABE_VESA_0501:
	mov	ax, [edi.ABE_Window_Pos]		; Get Window position from CB
	mov	[ebp.Client_DX], ax			; Set Client Reg.
	jmp	ABE_VESA_Success			; Exit

ABE_VESA_0500:
	movzx	eax, [ebp.Client_DX]
	or	ah, ah
	jnz	ABE_VESA_Error				; Error
	mov	[edi.ABE_Window_Pos], ax		;
	mov	ah, al

	mov	edx, [Graphics_Controller]		; Index
	mov	al, 09h					; Offset Register 0 (GR9)
	out	dx, al					; Data
	inc	dx
	mov	al, ah
	out	dx, al
	jmp	ABE_VESA_Success			;

EndProc ABE_VESA_05


;******************************************************************************
; ABE_VESA_06
;
;DESCRIPTION:
;	Set/Get Logical Scanline Length
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_06, PUBLIC

	cmp	[edi.ABE_Video_Mode], vVM_480_16M	; Q: 16M ?
	je	ABE_VESA_Error				;   Y: Error
	mov	eax, [ebp.Client_EBX]
	shr	al, 1					; Q: Window Set/Get ?
	jnz	ABE_VESA_Error				;    N: Error
	jc	short ABE_VESA_0601			;    Get:
							;    Set:
ABE_VESA_0600:
	mov	ecx, [ebp.Client_ECX]			; cx = pixels/scanline
	and	ecx, 0FF8h				; 8 bytes align 9 bits
	mov	[edi.ABE_BytesPerScanline], ecx
	shr	ecx, 3
	mov	edx, [CRT_3D4]
	mov	al, 13h
	out	dx, al
	mov	edx, [CRT_3D5]
	mov	al, cl
	out	dx, al
	mov	edx, [CRT_3D4]
	mov	al, 1Bh
	out	dx, al
	shr	ecx, 12					; move bit16 to bit4
	and	cl, 00010000b
	in	al, dx
	and	al, 11101111b
	or	al, cl
	out	dx, al

ABE_VESA_0601:
	mov	ecx, [edi.ABE_BytesPerScanline]
	mov	[ebp.Client_CX], cx
	xor	eax, eax
	mov	edx, 16					; dx:ax = 1M bytes
	div	cx					; ax = maximum scanlines
	mov	[ebp.Client_DX], ax
	jmp	ABE_VESA_Success			; Exit

EndProc ABE_VESA_06


;******************************************************************************
; ABE_VESA_07
;
;DESCRIPTION:
;	Set/Get Display Start
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_07, PUBLIC

	cmp	[edi.ABE_Video_Mode], vVM_480_16M	; Q: 16M ?
	je	ABE_VESA_Error				;   Y: Error
	mov	eax, [ebp.Client_EBX]
	shr	al, 1					; Q: Window Set/Get ?
	jnz	ABE_VESA_Error				;    N: Error
	jnc	short ABE_VESA_0700			;    Set:
							;    Get:
ABE_VESA_0701:
	mov	ax, [edi.ABE_DisplayStartX]
	mov	[ebp.Client_CX], ax
	mov	ax, [edi.ABE_DisplayStartY]
	mov	[ebp.Client_DX], ax
	jmp	ABE_VESA_Success			; Exit

ABE_VESA_0700:
	movzx	ecx, [ebp.Client_CX]
	movzx	eax, [ebp.Client_DX]
	mov	[edi.ABE_DisplayStartX], cx
	mov	[edi.ABE_DisplayStartY], ax
	mov	edx, [edi.ABE_BytesPerScanline]
	mul	edx
	add	ecx, eax
	mov	ah, cl
	and	ah, 3					;
	shr	ecx, 2					; 4byte align

	mov	edx, [CRT_3D4]
	mov	al, 0Ch
	out	dx, al
	mov	edx, [CRT_3D5]
	mov	al, ch
	out	dx, al

	mov	edx, [CRT_3D4]
	mov	al, 0Dh
	out	dx, al
	mov	edx, [CRT_3D5]
	mov	al, cl
	out	dx, al

	mov	edx, [CRT_3D4]
	mov	al, 1Bh
	out	dx, al
	shr	ecx, 16
	ror	cl, 1
	clc
	rcl	cl, 2
	mov	edx, [CRT_3D5]
	in	al, dx
	and	al, 11111010b
	and	cl, 00000101b
	or	al, cl
	out	dx, al

	mov	edx, [Feature_Control_3DA]
	in	al, dx
	mov	edx, [attribute_controller]
	mov	al, 33h
	out	dx, al
	out	5fh, al
	mov	al, ah
	out	dx, al
	jmp	ABE_VESA_Success			; Exit

EndProc ABE_VESA_07


;******************************************************************************
; ABE_VESA_08
;
;DESCRIPTION:
;	Set/Get DAC Palette Control
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_08, PUBLIC

	mov	eax, [ebp.Client_EBX]
	shr	al, 1					; Q: Window Set/Get ?
	jnz	ABE_VESA_Error				;    N: Error
	jc	short ABE_VESA_0801			;    Get:
							;    Set:
ABE_VESA_0800:
	cmp	ah, 6
	jnz	ABE_VESA_Error

ABE_VESA_0801:
	mov	[ebp.Client_BH], 6
	jmp	ABE_VESA_Success			; Exit

EndProc ABE_VESA_08


;******************************************************************************
; ABE_VESA_10
;
;DESCRIPTION:
;	Display Power Management Extensions
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDI = VACL control block data ptr
;
;EXIT:	None
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc ABE_VESA_10, PUBLIC
	mov	al, [ebp.Client_BL]
	cmp	al, 01h
	jne	@f
	mov	ah, [ebp.Client_BH]
@@:
	call	ABE_PowerMgr
	jc	ABE_VESA_Error

	cmp	byte ptr [ebp.Client_BL], 00h
	jne	@f
	mov	word ptr [ebp.Client_BX], ax
	jmp	ABE_VESA_Success

@@:
	cmp	byte ptr [ebp.Client_BL], 01h
	jne	@f
	jmp	ABE_VESA_Success

@@:
	cmp	byte ptr [ebp.Client_BL], 02h
	jne	ABE_VESA_Error
	mov	[ebp.Client_BH], ah
	jmp	ABE_VESA_Success

EndProc ABE_VESA_10


VxD_CODE_ENDS


	END
