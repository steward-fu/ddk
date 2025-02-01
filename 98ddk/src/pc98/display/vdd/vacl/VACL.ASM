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
;******************************************************************************
; VACL.ASM - Virtual Disply Mini Driver Sample
;******************************************************************************

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	MINIVDD equ True

	.XLIST
	INCLUDE VMM.INC
	INCLUDE MINIVDD.INC
	INCLUDE VDDACC.INC
	INCLUDE VDD98.INC
	INCLUDE VACL.INC
	INCLUDE Debug.Inc
	.LIST


;******************************************************************************
;		V I R T U A L	D E V I C E   D E C L A R A T I O N
;******************************************************************************

ifdef VACLB
	Declare_Virtual_Device VACLB, 3, 1, VACLB_Control, UNDEFINED_DEVICE_ID, VDD_Init_Order
else ;VACLB
	Declare_Virtual_Device VACL, 3, 1, VACL_Control, UNDEFINED_DEVICE_ID, VDD_Init_Order
endif ;VACLB


;******************************************************************************
;			      E X T E R N A L S
;******************************************************************************

VxD_CODE_SEG
	EXTRN	ABE_Int10:NEAR
	EXTRN	ABE_GetCapabilities:NEAR
	EXTRN	ABE_GetExtModeInfo:NEAR
	EXTRN	ABE_EnableBIOS:NEAR
	EXTRN	ABE_DisableBIOS:NEAR
	EXTRN	ABE_GetFlatSelector:NEAR
	EXTRN	ABE_SetPalette:NEAR
	EXTRN	ABE_GetPalette:NEAR
	EXTRN	ABE_SetCursor:NEAR
	EXTRN	ABE_ShowCursor:NEAR
	EXTRN	ABE_HideCursor:NEAR
	EXTRN	ABE_SetCursorPos:NEAR
	EXTRN	ABE_GetCurSorPos:NEAR
	EXTRN	ABE_PowerMgr:NEAR

	EXTRN	ACC_WinFunc:NEAR
	EXTRN	ABE_CheckCirrus:NEAR

	EXTRN	Clear_Screen:NEAR
	EXTRN	PC98toCIRRUS:NEAR
	EXTRN	SetLCD_OFF:NEAR
	EXTRN	SetLCD_ON:NEAR
	EXTRN	CIRRUStoPC98:NEAR
	EXTRN	set_hw_cursor:NEAR
VxD_CODE_ENDS

VxD_DATA_SEG
	EXTRN	ABE_PhysWindow:DWORD
	EXTRN	ABE_PageWindow:DWORD
	EXTRN	ABE_Internal_ID:BYTE

	EXTRN	PRODUCT_ID:BYTE
	EXTRN	ACCInfoBlock:BYTE
	EXTRN	ModeBlock256:BYTE
	EXTRN	ModeBlock16M:BYTE
	EXTRN	BiosInfo_ACC:BYTE

	EXTRN	Graphics_Controller:DWORD
	EXTRN	SessionMode:WORD
	EXTRN	DAC_Write_Index:DWORD
	EXTRN	DAC_Write_Data:DWORD

VxD_DATA_ENDS

;******************************************************************************
;				E Q U A T E S
;******************************************************************************



;******************************************************************************
;			D A T A	  S T R U C T U R E S
;******************************************************************************



;******************************************************************************
;		     I N I T I A L I Z A T I O N   D A T A
;******************************************************************************

VxD_IDATA_SEG

BeginMiniFunc VACL_Func_Table
	MiniFunc REGISTER_DISPLAY_DRIVER,  VACL_Register_Display_Driver
	MiniFunc PRE_HIRES_TO_VGA,	   VACL_Pre_Hires_to_VGA
	MiniFunc SAVE_REGISTERS,	   VACL_Save
	MiniFunc RESTORE_REGISTERS,	   VACL_Restore
	MiniFunc ENABLE_TRAPS,		   VACL_Enable_Trap
	MiniFunc DISABLE_TRAPS,		   VACL_Disable_Trap
	MiniFunc DISPLAY_DRIVER_DISABLING, VACL_Display_Driver_Disabling

	MiniFunc ENABLE_ACCELERATER,	   VACL_Enable_Accelerator
	MiniFunc DISABLE_ACCELERATER,	   VACL_Disable_Accelerator
;	MiniFunc CHECK_UPDATE,		   VACL_Check_Update
;	MiniFunc CHECK_WINDOWED,	   VACL_Check_Windowed
	MiniFunc ACC_VBE_PM,		   ABE_PowerMgr
;	MiniFunc ACC_VBE_DDC,		   ABE_DDC
	MiniFunc ACC_INT_10,		   ABE_Int10
	MiniFunc ACC_GET_CAPABILITIES,	   ABE_GetCapabilities
	MiniFunc ACC_GET_EXT_MODE_INFO,	   ABE_GetExtModeInfo
	MiniFunc ACC_GET_FLAT_SELECTOR,	   ABE_GetFlatSelector
	MiniFunc ACC_ENABLE_BIOS,	   ABE_EnableBIOS
	MiniFunc ACC_DISABLE_BIOS,	   ABE_DisableBIOS
	MiniFunc ACC_SET_PALETTE,	   ABE_SetPalette
	MiniFunc ACC_GET_PALETTE,	   ABE_GetPalette
	MiniFunc ACC_SET_CURSOR,	   ABE_SetCursor
	MiniFunc ACC_SHOW_CURSOR,	   ABE_ShowCursor
	MiniFunc ACC_HIDE_CURSOR,	   ABE_HideCursor
	MiniFunc ACC_SET_CURSOR_POS,	   ABE_SetCursorPos
	MiniFunc ACC_GET_CURSOR_POS,	   ABE_GetCurSorPos
EndMiniFunc VACL_Func_Table

VxD_IDATA_ENDS


;******************************************************************************
;			       L O C A L   D A T A
;******************************************************************************

VxD_IDATA_SEG

OEMStringMax	equ	32
DefID	macro	IDN
	local	IDNL
IDNL	label	byte
	db	IDN
.errnz	($-IDNL) ge OEMStringMax
	db	(OEMStringMax-($-IDNL)) dup (0)
endm

ACC_stub_start	label byte			; These data must be referenced from RealMode Apps.
ACC_OEMString	label byte
	DefID	'NEC Unknown'
ACC_VideoMode	label word
	dw	vVM_480_256, vVM_600_256, vVM_768_256
	dw	vVM_480_64K, vVM_480_16M, vVM_600_64K
	dw	0FFFFh
ACC_stub_size	equ	($-ACC_stub_start)

Cirrus_C_Table	label	byte
	DefID	'NEC PC-9801-96'		; 01100000:	   :B3

Cirrus_ML_Table	label	byte
	DefID	'NEC PC-9821Bp/Bs/Be'		; 01010000: GD5428 
	DefID	'NEC PC-9821Cs2'		; 01010001:   "    
	DefID	'NEC PC-9821Bf/SV98model1/2'	; 01010010:   "    
	DefID	'NEC PC-9821Ns'			; 01010011:   "    
	DefID	'NEC PC-9821Ts'			; 01010100:   "    
	DefID	'NEC PC-9821Np/Es'		; 01010101:   "    
	DefID	'NEC PC-9821Ne2'		; 01010110:   "    
	DefID	'NEC PC-9821Nd'			; 01010111:   "    
	DefID	'NEC PC-9821Xe'			; 01011000: GD5430 
	DefID	'NEC PC-9821Cb'			; 01011001:   "    
	DefID	'NEC PC-9821Cx/Cf'		; 01011010:   "    
Cirrus_Nf_Table	label	byte
	DefID	'NEC PC-9821Nf'			; 01110000:   "    

ChipID_TBL	label	byte
	db	50h dup (0)
	db	02h	; 0x50 PC-9821Be/Bs/Bp (GD5428)
	db	04h	; 0x51 PC-9821Cs2 (GD5428)
	db	09h	; 0x52 PC-9821Bf,SV-98model1/2 (GD5428)
	db	07h	; 0x53 PC-9821Ns (GD5428)
	db	03h	; 0x54 PC-9821Ts (GD5428)
	db	06h	; 0x56 PC-9821Ne2 (GD5428)
	db	08h	; 0x55 PC-9821Np/Es (GD5428)
	db	05h	; 0x57 PC-9821Nd (GD5428)
	db	0Bh	; 0x58 PC-9821Xe (GD5430)
	db	0Ch	; 0x59 PC-9821Cb (GD5430)
	db	0Dh	; 0x5A PC-9821Cx/Cf (GD5430)
	db	5 dup (0)
	db	01h	; 0x60 PC-9801-96
	db	15 dup (0)
	db	0Ah	; 0x70 PC-9821Nf (GD5428)
;	db	8Fh dup (0)

BusID_TBL	label	byte
	db	00000000b				; Dummy
	db	00000010b				; ML-Bus
	db	00000001b				; C-Bus
	db	00000010b				; ML-Bus (for Nf)

VxD_IDATA_ENDS

VxD_DATA_SEG

PUBLIC	Vids
PUBLIC	VDD_Data_Area
PUBLIC	VACL_CB_Offset
PUBLIC	VACL_MiniID
PUBLIC	VACL_Flags

Vids			db	(size Vids_struct) dup (0)

VACL_CB_Offset		dd	?
VDD_Data_Area		dd	?
VACL_MiniID		db	?

VACL_Flags		dd	?

VDD_TR_Indx15_00	dw	0
VDD_TR_Indx15_01	dw	0
VDD_TR_Indx15_02	dw	0
VDD_TR_Indx15_03	dw	0
VDD_TR_Indx15_04	dw	0
VDD_TR_Indx15_05	dw	0

vddsave_sysinf		dd	0

VDD_S3_BaseAddr		dd	0


clstby_table	dw	0CA4h	; set MCLK to 7.1MHz
		db	01Fh
		dw	0CA5h
		db	004h

		dw	0CA4h	; set VCLK to 3.46MHz
		db	00Bh
		dw	0CA5h
		db	00Fh

		dw	0CA4h
		db	00Ch
		dw	0CA5h
		db	00Fh

		dw	0CA4h
		db	00Dh
		dw	0CA5h
		db	00Fh

		dw	0CA4h
		db	00Eh
		dw	0CA5h
		db	00Fh


		dw	0CA4h
		db	01Bh
		dw	0CA5h
		db	03Fh

		dw	0CA4h
		db	01Ch
		dw	0CA5h
		db	03Fh

		dw	0CA4h
		db	01Dh
		dw	0CA5h
		db	03Fh

		dw	0CA4h
		db	01Eh
		dw	0CA5h
		db	03Fh

		dw	0FFFFh	; end of this table


;ÇQÇTÇUêFÉpÉåÉbÉg

default_palette	db      000h,000h,000h, 000h,000h,007h, 007h,000h,000h
		db	007h,000h,007h, 000h,007h,000h, 000h,007h,007h
		db	007h,007h,000h, 007h,007h,007h, 004h,004h,004h
		db	000h,000h,00Fh, 00Fh,000h,000h, 00Fh,000h,00Fh
		db	000h,00Fh,000h, 000h,00Fh,00Fh, 00Fh,00Fh,000h
		db	00Fh,00Fh,00Fh, 00Ch,00Ch,00Ch, 000h,000h,017h
		db	017h,000h,000h, 017h,000h,017h, 000h,017h,000h
		db	000h,017h,017h, 017h,017h,000h, 017h,017h,017h
		db	014h,014h,014h, 000h,000h,01Fh, 01Fh,000h,000h
		db	01Fh,000h,01Fh, 000h,01Fh,000h, 000h,01Fh,01Fh
		db	01Fh,01Fh,000h, 01Fh,01Fh,01Fh, 01Ch,01Ch,01Ch
		db	000h,000h,027h, 027h,000h,000h, 027h,000h,027h
		db	000h,027h,000h, 000h,027h,027h, 027h,027h,000h
		db	027h,027h,027h, 024h,024h,024h, 000h,000h,02Fh
		db	02Fh,000h,000h, 02Fh,000h,02Fh, 000h,02Fh,000h
		db	000h,02Fh,02Fh, 02Fh,02Fh,000h, 02Fh,02Fh,02Fh
		db	02Ch,02Ch,02Ch, 000h,000h,037h, 037h,000h,000h
		db	037h,000h,037h, 000h,037h,000h, 000h,037h,037h
		db	037h,037h,000h, 037h,037h,037h, 034h,034h,034h
		db	000h,000h,03Fh, 03Fh,000h,000h, 03Fh,000h,03Fh
		db	000h,03Fh,000h, 000h,03Fh,03Fh, 03Fh,03Fh,000h
		db	03Fh,03Fh,03Fh, 03Ch,03Ch,03Ch, 000h,000h,047h
		db	047h,000h,000h, 047h,000h,047h, 000h,047h,000h
		db	000h,047h,047h, 047h,047h,000h, 047h,047h,047h
		db	044h,044h,044h, 000h,000h,04Fh, 04Fh,000h,000h
		db	04Fh,000h,04Fh, 000h,04Fh,000h, 000h,04Fh,04Fh
		db	04Fh,04Fh,000h, 04Fh,04Fh,04Fh, 04Ch,04Ch,04Ch
		db	000h,000h,057h, 057h,000h,000h, 057h,000h,057h
		db	000h,057h,000h, 000h,057h,057h, 057h,057h,000h
		db	057h,057h,057h, 054h,054h,054h, 000h,000h,05Fh
		db	05Fh,000h,000h, 05Fh,000h,05Fh, 000h,05Fh,000h
		db	000h,05Fh,05Fh, 05Fh,05Fh,000h, 05Fh,05Fh,05Fh
		db	05Ch,05Ch,05Ch, 000h,000h,067h, 067h,000h,000h
		db	067h,000h,067h, 000h,067h,000h, 000h,067h,067h
		db	067h,067h,000h, 067h,067h,067h, 064h,064h,064h
		db	000h,000h,06Fh, 06Fh,000h,000h, 06Fh,000h,06Fh
		db	000h,06Fh,000h, 000h,06Fh,06Fh, 06Fh,06Fh,000h
		db	06Fh,06Fh,06Fh, 06Ch,06Ch,06Ch, 000h,000h,077h
		db	077h,000h,000h, 077h,000h,077h, 000h,077h,000h
		db	000h,077h,077h, 077h,077h,000h, 077h,077h,077h
		db	074h,074h,074h, 000h,000h,07Fh, 07Fh,000h,000h
		db	07Fh,000h,07Fh, 000h,07Fh,000h, 000h,07Fh,07Fh
		db	07Fh,07Fh,000h, 07Fh,07Fh,07Fh, 07Ch,07Ch,07Ch
		db	000h,000h,087h, 087h,000h,000h, 087h,000h,087h
		db	000h,087h,000h, 000h,087h,087h, 087h,087h,000h
		db	087h,087h,087h, 084h,084h,084h, 000h,000h,08Fh
		db	08Fh,000h,000h, 08Fh,000h,08Fh, 000h,08Fh,000h
		db	000h,08Fh,08Fh, 08Fh,08Fh,000h, 08Fh,08Fh,08Fh
		db	08Ch,08Ch,08Ch, 000h,000h,097h, 097h,000h,000h
		db	097h,000h,097h, 000h,097h,000h, 000h,097h,097h
		db	097h,097h,000h, 097h,097h,097h, 094h,094h,094h
		db	000h,000h,09Fh, 09Fh,000h,000h, 09Fh,000h,09Fh
		db	000h,09Fh,000h, 000h,09Fh,09Fh, 09Fh,09Fh,000h
		db	09Fh,09Fh,09Fh, 09Ch,09Ch,09Ch, 000h,000h,0A7h
		db	0A7h,000h,000h, 0A7h,000h,0A7h, 000h,0A7h,000h
		db	000h,0A7h,0A7h, 0A7h,0A7h,000h, 0A7h,0A7h,0A7h
		db	0A4h,0A4h,0A4h, 000h,000h,0AFh, 0AFh,000h,000h
		db	0AFh,000h,0AFh, 000h,0AFh,000h, 000h,0AFh,0AFh
		db	0AFh,0AFh,000h, 0AFh,0AFh,0AFh, 0ACh,0ACh,0ACh
		db	000h,000h,0B7h, 0B7h,000h,000h, 0B7h,000h,0B7h
		db	000h,0B7h,000h, 000h,0B7h,0B7h, 0B7h,0B7h,000h
		db	0B7h,0B7h,0B7h, 0B4h,0B4h,0B4h, 000h,000h,0BFh
		db	0BFh,000h,000h, 0BFh,000h,0BFh, 000h,0BFh,000h
		db	000h,0BFh,0BFh, 0BFh,0BFh,000h, 0BFh,0BFh,0BFh
		db	0BCh,0BCh,0BCh, 000h,000h,0C7h, 0C7h,000h,000h
		db	0C7h,000h,0C7h, 000h,0C7h,000h, 000h,0C7h,0C7h
		db	0C7h,0C7h,000h, 0C7h,0C7h,0C7h, 0C4h,0C4h,0C4h
		db	000h,000h,0CFh, 0CFh,000h,000h, 0CFh,000h,0CFh
		db	000h,0CFh,000h, 000h,0CFh,0CFh, 0CFh,0CFh,000h
		db	0CFh,0CFh,0CFh, 0CCh,0CCh,0CCh, 000h,000h,0D7h
		db	0D7h,000h,000h, 0D7h,000h,0D7h, 000h,0D7h,000h
		db	000h,0D7h,0D7h, 0D7h,0D7h,000h, 0D7h,0D7h,0D7h
		db	0D4h,0D4h,0D4h, 000h,000h,0DFh, 0DFh,000h,000h
		db	0DFh,000h,0DFh, 000h,0DFh,000h, 000h,0DFh,0DFh
		db	0DFh,0DFh,000h, 0DFh,0DFh,0DFh, 0DCh,0DCh,0DCh
		db	000h,000h,0E7h, 0E7h,000h,000h, 0E7h,000h,0E7h
		db	000h,0E7h,000h, 000h,0E7h,0E7h, 0E7h,0E7h,000h
		db	0E7h,0E7h,0E7h, 0E4h,0E4h,0E4h, 000h,000h,0EFh
		db	0EFh,000h,000h, 0EFh,000h,0EFh, 000h,0EFh,000h
		db	000h,0EFh,0EFh, 0EFh,0EFh,000h, 0EFh,0EFh,0EFh
		db	0ECh,0ECh,0ECh, 000h,000h,0F7h, 0F7h,000h,000h
		db	0F7h,000h,0F7h, 000h,0F7h,000h, 000h,0F7h,0F7h
		db	0F7h,0F7h,000h, 0F7h,0F7h,0F7h, 0F4h,0F4h,0F4h
		db	000h,000h,0FFh, 0FFh,000h,000h, 0FFh,000h,0FFh
		db	000h,0FFh,000h, 000h,0FFh,0FFh, 0FFh,0FFh,000h
		db	0FFh,0FFh,0FFh  

VxD_DATA_ENDS


;******************************************************************************
;			      M A I N	C O D E
;******************************************************************************

VxD_LOCKED_CODE_SEG

;******************************************************************************
;
;   NAME:
;	VACL_Control
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = control code
;	EBX = VM handle
;	EBP = Client ptr
;
;   EXIT:
;	control code specific return of carry flag indicate aborted operation
;
;   USES:
;	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================

ifdef VACLB
Begin_Control_Dispatch VACLB
else ;VACLB
Begin_Control_Dispatch VACL
endif ;VACLB
	Control_Dispatch Sys_Critical_Init,		VACL_Dynamic_Init
	Control_Dispatch Sys_Dynamic_Device_Init,	VACL_Dynamic_Init
	Control_Dispatch Init_Complete,			VACL_Init_Complete
	Control_Dispatch Create_VM,			VACL_Create_VM
IFDEF	DEBUG
	Control_Dispatch Debug_Query,			VACL_DebugQuery
ENDIF
ifdef VACLB
End_Control_Dispatch VACLB
else ;VACLB
End_Control_Dispatch VACL
endif ;VACLB

VxD_LOCKED_CODE_ENDS


VxD_ICODE_SEG

;******************************************************************************
;
;   NAME:
;	VACL_Dynamic_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX contains the VM handle of the Windows VM.
;
;   EXIT:
;	CY set if initialization failed.  MiniVDD won't be loaded.
;
;   USES:
;
;==============================================================================
BeginProc VACL_Dynamic_Init, PUBLIC

	VMMCall _Allocate_Device_CB_Area, <<SIZE VACL_CB_Struct>, 0>
	or	eax, eax
	jz	VACL_DI_Error
	mov	[VACL_CB_Offset], eax

	call	ABE_CheckCirrus
	jc	VACL_DI_Abort				; No accelerator
	mov	[ABE_PageWindow], ecx			; Page window address
	shl	ecx, 12
	VMMCall _MapPhysToLinear, <ecx,0100000h,0>	; 1M
	inc	eax
	jz	VACL_DI_Error
	dec	eax
	mov	[ABE_PhysWindow], eax			; linear address

; Set WinFuncPtr address
	mov	esi, OFFSET32 ACC_WinFunc		; ESI = address of handler
	xor	edx, edx
	VMMCall Allocate_V86_Call_Back			; Hook WinFunc
	jc	VACL_DI_Error
	mov	[ModeBlock256.WinFuncPtr], eax
	mov	[ModeBlock16M.WinFuncPtr], eax

; Set OEM string
	movzx	eax, [PRODUCT_ID]

	mov	esi, OFFSET32 Cirrus_C_Table
	cmp	al, vID_B3
	jz	short VACL_DI_SetID

	mov	esi, OFFSET32 Cirrus_Nf_Table
	cmp	al, vID_Nf
	jz	short VACL_DI_SetID

	mov	esi, OFFSET32 Cirrus_ML_Table
	xor	al, 01010000b
	cmp	al, 7
	ja	short VACL_DI_UnknownID
.errnz	OEMStringMax-(1 shl 5)
	shl	eax, 5
	add	esi, eax

VACL_DI_SetID:
	mov	edi, OFFSET32 ACC_OEMString
	mov	ecx, OEMStringMax
	rep movsb
VACL_DI_UnknownID:

	VMMCall _Allocate_Global_V86_Data_Area, <ACC_stub_size, 0>
	or	eax, eax
	jc	VACL_DI_Error
	push	edi
	mov	edi, eax
	mov	esi, OFFSET32 ACC_stub_start
	mov	ecx, ACC_stub_size
	cld
	Begin_Touch_1st_Meg
	rep	movsb
	End_Touch_1st_Meg
	pop	edi

	shl	eax, 12
	shr	ax, 12
	push	eax
	add	eax, ACC_OEMString - ACC_stub_start
	mov	ACCInfoBlock.OEMStringPtr, eax
	pop	eax
	add	eax, ACC_VideoMode - ACC_stub_start
	mov	ACCInfoBlock.VideoModePtr, eax


	xor	eax, eax
	mov	al, [PRODUCT_ID]
	mov	al, [ChipID_TBL+eax]
	mov	[BiosInfo_ACC.ChipNum], al

	xor	eax, eax
	mov	al, [PRODUCT_ID]
	shr	eax, 4
	and	eax, 0011b
	mov	al, [BusID_TBL+eax]
	cmp	[PRODUCT_ID], vID_B3	; External
	jne	@f
	or	al, 80h
@@:
	mov	[BiosInfo_ACC.BusID], al


	mov	ecx, NBR_MINI_VDD_FUNCTIONS
	mov	esi, OFFSET32 VACL_Func_Table
	xor	edi, edi
	VxDCall VDD_Register_Mini_VDD
	jc	SHORT VACL_DI_Error
	mov	[VACL_MiniID], ah
	mov	[VDD_Data_Area], edi
	mov	[VDD_CB_Offset], esi
	mov	eax, [VACL_Flags]
	or	[edi.Vids_SFlags], eax
	clc
	ret

VACL_DI_Abort:
	stc
	ret

VACL_DI_Error:
	VMMJmp	Fatal_Memory_Error

EndProc VACL_Dynamic_Init


;******************************************************************************
;
;   NAME:
;	VACL_Init_Complete
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;	set CY if error
;
;   USES:
;
;==============================================================================
BeginProc VACL_Init_Complete, PUBLIC

	pushad
	mov	edi, [VDD_Data_Area]
	mov	eax, [edi.Vids_SFlags]
	mov	[Vid_SFlags], eax
	popad
	clc
	ret

EndProc VACL_Init_Complete

VxD_ICODE_ENDS


VxD_CODE_SEG

;******************************************************************************
;
;   NAME:
;	VACL_Create_VM
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;	set CY if error
;
;   USES:
;
;==============================================================================
BeginProc VACL_Create_VM, PUBLIC
	pushad
	push	ebx

; Set default palette to CB
	mov	esi, offset32 default_palette	;Set default palette to CB
	mov	edi, ebx
	add	edi, [VACL_CB_Offset]
	add	edi, ABE_Palette
	mov	ecx, 0100h
	xor	al, al
set_def_palette_loop:
	movsb
	movsb
	movsb
	stosb					;reserved data
	loop	set_def_palette_loop
	pop	ebx

; Set H/W palette
	cmp	word ptr [edx.ABE_Video_Mode], vVM_1024_256	; Q: 256 ?
	jg	dont_set_hw_palette				;  N: Bye

	mov	edx, DAC_Write_Index
	xor	al, al
	out	dx, al			; write palette index

	push	ebx

	mov	ecx, 0100h
	mov	esi, edi
	add	esi, ABE_Palette

	mov	edx, DAC_Write_Data
set_hw_palette_loop:
	lodsd
	mov	ebx, eax

	mov	al, bh			; Red value
	out	dx, al
	mov	al, bl			; Green value
	out	dx, al
	shr	ebx, 8
	mov	al, bh			; Blue value
	out	dx, al
	loop	set_hw_palette_loop

	pop	ebx

dont_set_hw_palette:

	popad

	clc
	ret

EndProc VACL_Create_VM

VxD_CODE_ENDS


;******************************************************************************
;			 M i n i V D D	 S E R V I C E
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   NAME:
;	VACL_Register_Display_Driver
;
;   DESCRIPTION:
;
;   ENTRY:
;	none
;
;   EXIT:
;	none
;
;   USES:
;
;==============================================================================
BeginProc VACL_Register_Display_Driver, PUBLIC

	push	eax

	mov	eax, [ebp.Client_ECX]
	or	eax, eax
	jnz	@f

	mov	eax, [ABE_PageWindow]
	shl	eax, 12

	mov	[ebp.Client_CX], ax
	shr	eax, 16
	mov	[ebp.Client_BX], ax

@@:	pop	eax

	clc
	ret

EndProc VACL_Register_Display_Driver


;******************************************************************************
;
;   NAME:
;	VACL_Pre_Hires_to_VGA
;
;   DESCRIPTION:
;
;   ENTRY:
;	none
;
;   EXIT:
;	none
;
;   USES:
;
;==============================================================================
BeginProc VACL_Pre_Hires_to_VGA, PUBLIC

	call	VACL_Display_Driver_Disabling
	ret

EndProc VACL_Pre_Hires_to_VGA


;******************************************************************************
;
;   NAME:
;	VACL_Save
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = VM Handle
;	EDI = VDD CB ptr
;
;   EXIT:
;	set CY if error
;
;   USES:
;
;==============================================================================
BeginProc VACL_Save, PUBLIC

	clc
	ret

EndProc VACL_Save


;******************************************************************************
;
;   NAME:
;	VACL_Restore
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = VM Handle
;	EDI = VDD CB ptr
;
;   EXIT:
;	set CY if error
;
;   USES: EDI
;
;==============================================================================
BeginProc VACL_Restore, PUBLIC

	mov	edi, ebx
	add	edi, [VACL_CB_Offset]
	test	[edi.ABE_Flags], fABE_Enable
	jz	vacl_restore_done

	pushad
	mov	dx, [edi.ABE_SessionMode]
	mov	[SessionMode], dx
	call	SetLCD_OFF			; Set LCD BackLight OFF
	call	PC98toCIRRUS
	call	Clear_Screen
	call	SetLCD_ON			; Set LCD BackLight ON

	mov	edx, Graphics_Controller	; Index
	mov	al, 09h				; Offset Register 0 (GR9)
	out	dx, al				; Data
	inc	dx
	xor	al, al
	out	dx, al
	popad

ifdef	VACLB
	mov	edx, OPTION_PORTBASE	; GlueIndexReg
	mov	al, vGL_STATUS_CMD
	out	dx, al			; set glue logic index(4):PKG STATUS
	mov	edx, OPTION_PORTBASE+1	; GlueDataReg
	in	al, dx			; get PKG STATUS
					;  mapping index (DIP-SW 1-3)

	mov	ah, 0a0h		; F00000h
	mov	ecx, 0F00h
	test	al, 011b		; 000b
	jz	short @f
	mov	ah, 080h		; F20000h
	mov	ecx, 0F20h
	test	al, 010b		; 001b
	jz	short @f
	mov	ah, 0E0h		; F40000h
	mov	ecx, 0F40h
	test	al, 001b		; 010b
	jz	short @f
	mov	ah, 0C0h		; F60000h
	mov	ecx, 0F60h
@@:
	mov	edx, OPTION_PORTBASE	; GlueIndexReg
	mov	al, vGL_VGA_REMAP
	out	dx, al			; set glue logic index(1):VGA Remapping
	mov	edx, OPTION_PORTBASE+1	; GlueDataReg
	mov	al, ah
	out	dx, al			; set VGA Remapping
else   ;VACLB
	mov	edx, MOTHER_PORTBASE	; GlueIndexReg
	mov	al, vGL_BASE_ADDR	; Index 02h
	out	dx, al
	mov	edx, MOTHER_PORTBASE+1	; Data Reg

	xor	al, al
	test	[ABE_Internal_ID], fID_GD5430
	jz	@f
	mov	al, 0Fh
@@:
	out	dx, al
endif  ;VACLB

	VMMcall	Test_Sys_VM_Handle		; Q: VM1?
	jz	vacl_restore_done		;  Y: jump

	mov	eax, [ABE_PageWindow]		; Physical page number
	xor	ecx, ecx			; A800-
	mov	edx, 10h			; 16pages(64K)
	VxDCall	VDD_Map_VRAM

	cmp	byte ptr [edi.ABE_Cursor_Enable], HWCursorOn
	jnz	no_restore_hwc
	push	ebx
	push	edi
	mov	esi, edi
	add	esi, ABE_Cursor_Shape
	call	set_hw_cursor			;Subset ABE_Cursor_Shape
	call	ABE_ShowCursor
	pop	edi
	push	edi
	mov	cx, word ptr [edi.ABE_Cursor_X]
	mov	dx, word ptr [edi.ABE_Cursor_Y]
	call	ABE_SetCursorPos
	pop	edi
	pop	ebx

no_restore_hwc:

; palette restore from CB

	cmp	word ptr [edi.ABE_Video_Mode], vVM_1024_256	; Q: 256 ?
	jg	short restore_palette_done			;  N: Bye

	mov	edx, DAC_Write_Index
	xor	al, al
	out	dx, al			; write palette index

	push	ebx
	push	edi

	mov	ecx, 0100h
	mov	esi, edi
	add	esi, ABE_Palette

	mov	edx, DAC_Write_Data
restore_palette_loop:
	lodsd
	mov	ebx, eax

	mov	al, bh			; Red value
	out	dx, al
	mov	al, bl			; Green value
	out	dx, al
	shr	ebx, 8
	mov	al, bh			; Blue value
	out	dx, al
	loop	restore_palette_loop

	pop	edi
	pop	ebx

restore_palette_done:

	mov	edx, Graphics_Controller	; Index
	mov	al, 09h				; Offset Register 0 (GR9)
	out	dx, al				; Data
	inc	dx
	mov	ax, [edi.ABE_Window_Pos]
	out	dx, al

vacl_restore_done:
	clc
	ret

EndProc VACL_Restore


;******************************************************************************
;
;   NAME:
;	VACL_Enable_Trap
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = VM Handle
;	EDI = VDD CB ptr
;
;   EXIT:
;	set CY if error
;
;   USES:
;
;==============================================================================
BeginProc VACL_Enable_Trap, PUBLIC

	clc
	ret

EndProc VACL_Enable_Trap


;******************************************************************************
;
;   NAME:
;	VACL_Disable_Trap
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = VM Handle
;	EDI = VDD CB ptr
;
;   EXIT:
;	set CY if error
;
;   USES:
;
;==============================================================================
BeginProc VACL_Disable_Trap, PUBLIC

	clc
	ret

EndProc VACL_Disable_Trap


;******************************************************************************
;
;   NAME:
;	VACL_Enable_Accelerator
;
;   DESCRIPTION:
;
;   ENTRY:
;	none
;
;   EXIT:
;	none
;
;   USES:
;
;==============================================================================
BeginProc VACL_Enable_Accelerator, PUBLIC

	ret

EndProc VACL_Enable_Accelerator

;******************************************************************************
;
;   NAME:
;	VACL_Disable_Accelerator
;
;   DESCRIPTION:
;
;   ENTRY:
;	none
;
;   EXIT:
;	none
;
;   USES:
;
;==============================================================================
BeginProc VACL_Disable_Accelerator, PUBLIC
	call	VACL_Display_Driver_Disabling
	ret
EndProc VACL_Disable_Accelerator


;******************************************************************************
;
;   NAME:
;	VACL_Display_Driver_Disabling
;
;   DESCRIPTION:
;
;
;   ENTRY:
;	none
;
;   EXIT:
;	none
;
;   USES:
;
;==============================================================================
BeginProc VACL_Display_Driver_Disabling, PUBLIC

	pushad

	call	SetLCD_OFF			; Set LCD BackLight OFF
	call	CIRRUStoPC98
	call	SetLCD_ON			; Set LCD BackLight ON

	popad
	clc
	ret

EndProc VACL_Display_Driver_Disabling

VxD_CODE_ENDS


;******************************************************************************
;			    I / O   T R A P P I N G
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   NAME:
;	VACL_Phys_IO
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VACL_Phys_IO, High_Freq

	test	ecx, OUTPUT
	jz	short VACL_Trap_Input

	test	ecx, Dword_IO
	jz	short @f
	out	dx, eax
	ret

@@:	test	ecx, Word_IO
	jz	short @f
	out	dx, ax
	ret

@@:	out	dx, al
	ret

VACL_Trap_Input:
	test	ecx, Dword_IO
	jz	short @f
	in	eax, dx
	ret

@@:	test	ecx, Word_IO
	jz	short @f
	in	ax, dx
	ret

@@:	in	al, dx
	ret

EndProc VACL_Phys_IO

VxD_CODE_ENDS


IFDEF	DEBUG
;******************************************************************************
;				D E B U G   C O D E
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   NAME:
;	VACL_DebugQuery
;
;   DESCRIPTION:
;	Dump appropriate VACL state information for each VM.
;
;   ENTRY:
;	None
;
;   EXIT:
;	None
;
;   USES:
;	Flags
;
;==============================================================================
BeginProc VACL_DebugQuery

	pushad
Trace_Out   "VACL video state dump for all Virtual Machines:"

	mov	eax, [VACL_CB_Offset]
	Trace_Out "VACL_CB_Offset = #eax"

	popad
	ret

EndProc VACL_DebugQuery

VxD_CODE_ENDS

ENDIF	; DEBUG

	END
