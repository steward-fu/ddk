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


;******************************************************************************
;				 E Q U A T E S
;******************************************************************************
VBE_PM_ON	equ	00000000b
VBE_PM_STADBY	equ	00000001b
VBE_PM_SUSPEND	equ	00000010b
VBE_PM_OFF	equ	00000100b

VBE_PM_Caps	equ	VBE_PM_SUSPEND+VBE_PM_OFF
VBE_PM_Version	equ	10h

Report_VBE_PM_Caps equ	00h
Set_Display_Power_State equ 01h
Get_Display_Power_State equ 02h


;******************************************************************************
;			      L O C A L	  D A T A
;******************************************************************************
VxD_DATA_SEG
CurrentPowerState	db	VBE_PM_ON

crt_on_off		db	0
internal_accelerator	db	0	;
internal_acceleratorID	db	0	;
external_accelerator	db	0	;
external_acceleratorID	db	0	; append for B3
PCI_acc_sw		db	0	;

vision_sel		dw	0	; for Vison864
					; necessary to release it at end of PM
bDisplayStatus		db	0	
bOverScanColor		db	0	
bOverScanMode		db	0	
bIsCb2etc			db	0	
bPMFlag			db	0	
EXTRN	PRODUCT_ID:BYTE			

EXTRN	Graphics_Controller:DWORD	
EXTRN	attribute_controller:DWORD	
EXTRN	attribute_controllerR:DWORD	
EXTRN	Miscellaneous:DWORD		
EXTRN	CRT_3D4:DWORD			
EXTRN	GlueIndexReg:DWORD		

VxD_DATA_ENDS


;******************************************************************************
;			 D A T A   S T R U C T U R E S
;******************************************************************************


;******************************************************************************
;				  M A C R O S
;******************************************************************************


;******************************************************************************
;			       M A I N	 C O D E
;******************************************************************************
VxD_CODE_SEG
;******************************************************************************
; ABE_PowerMgr
;
;DESCRIPTION:
;
;ENTRY:
;
;EXIT:
;
;USES:
;
BeginProc ABE_PowerMgr, PUBLIC

	and	al, al
	jz	ABE_PowerMgr_GetCaps
	cmp	al, Get_Display_Power_State
	je	ABE_PowerMgr_GetState
	cmp	al, Set_Display_Power_State
	je	ABE_PowerMgr_SetState

ABE_PowerMgr_Error:
	stc
ABE_PowerMgr_Exit:
	ret

ABE_PowerMgr_GetCaps:
;Report VBE/PM Capabilities
	mov	ax, VBE_PM_Caps SHL 8 + VBE_PM_Version
	clc
	jmp	ABE_PowerMgr_Exit

ABE_PowerMgr_GetState:
;Get Display Power State
	mov	ah, [CurrentPowerState]
	clc
	jmp	ABE_PowerMgr_Exit

ABE_PowerMgr_SetState:
;Set Display Power State
	test	ah, NOT VBE_PM_Caps
	jnz	ABE_PowerMgr_Error

	mov	[CurrentPowerState], ah

	and	ah, ah
	jnz	power_saving

;Power on
	call	HW_PM_OUT
	mov	bPMFlag, 0
	clc
	jmp	ABE_PowerMgr_Exit
;Power save
power_saving:
	cmp	bPMFlag, 0
	jnz	ABE_PowerMgr_Error
	call	HW_PM_IN
	mov	bPMFlag, 1
	clc
	jmp	ABE_PowerMgr_Exit

EndProc ABE_PowerMgr

;******************************************************************************
; HW_PM_IN
;
;******************************************************************************
public	HW_PM_IN
HW_PM_IN	proc	near
	pushad
	push	eax
	push	ebx
	push	dx

	inc	byte ptr ds:[456h]	; block suspend


;set LCD BackLight OFF
	mov	bl,[PRODUCT_ID]
	cmp	bl, vID_Ts
	jz	@f
	cmp	bl, vID_Ne2
	jz	@f
	cmp	bl, vID_Nd
	jz	@f
	cmp	bl, vID_Ns
	jz	@f
	cmp	bl, vID_Np_Es
	jz	LCD_off_Np
	jmp	LCD_off
@@:
	mov	dx, MOTHER_PORTBASE
	xor	ax, ax			; vGL_PRODUCT_ID
	out	dx, al
	mov	al,0ffh
	mov	dx, MOTHER_PORTBASE+1
	out	dx, al
	in	al, dx
	cmp	al,55h
	je	LCD_off_Np

	mov	dx, 08f0h
	mov	ax, 0052h
	out	dx, ax
	out	05fh, al
	mov	dx, 08f2h
	in	ax,dx			; stay other bits
	or	ax,0080h
	out	dx, ax
	jmp	short LCD_off

LCD_off_Np:
	mov	dx, MOTHER_PORTBASE	;
	mov	al,083h			;
	out	dx,al
	mov	dl,0abh
	mov	dx, MOTHER_PORTBASE+1
	mov	al,01h
	out	dx,al
LCD_off:

; Make sure a Attribute Controller Toggle
	mov	edx, CRT_3D4		; dirty VGA:toggle ATR.cont.
	mov	al, 24h			; for index offer
	out	dx, al
	inc	dx
	in	al, dx
	test	al, 080h
	jz	cirrus_off_command
	mov	edx, attribute_controllerR
	in	al, dx
	mov	edx, attribute_controller
	out	dx, al
cirrus_off_command:

; Save display status
	mov	edx, CRT_3D4
	mov	al, 26h
	out	dx, al
	inc	dx
	in	al, dx
	and	al, 00100000b		; bit5 == Display status
	mov	bDisplayStatus, al

; Save over scan color
	mov	bl, 011h		; if display status == off
	cmp	al, 0
	jz	short @f
	mov	bl, 031h
@@:
	mov	edx, attribute_controller
	mov	al, bl
	out	dx, al
	inc	dx
	in	al, dx
	mov	bOverScanColor, al

; Save over scan mode
	mov	cl, [PRODUCT_ID]
	mov	bl, cl
	cmp	bl, vID_Cb2
	jz	Is_Cb2etc_0
	cmp	bl, vID_Cx2
	jz	Is_Cb2etc_0
	cmp	bl, vID_Xe10
	jz	Is_Cb2etc_0
	mov	bIsCb2etc, 0
	jmp	GD28_skip
Is_Cb2etc_0:
	mov	bIsCb2etc, 1
	mov	dx, 0faah
	mov	al, 20h
	out	dx, al
	inc	dx
	in	al, dx
	and	al, 00010000b		; bit4 == Over scan mode
	mov	bOverScanMode, al

; Let under scan mode (for GD5430)
	mov	edx, MOTHER_PORTBASE
	mov	al, 20h
	out	dx, al
	inc	dx
	mov	al,0f0h
	out	dx, al

GD28_skip:
; Set over scan color to BLACK
	mov	edx, attribute_controller
	xor	al, al
	out	dx, al

; Stop display
	out	dx, al

; Fix VSYNC to high
	mov	bl, cl			;cl== PRODUCT_ID
	and	bl, 00001000b
	jnz	PMI_GD5430
PMI_GD5428:
	mov	edx, GlueindexReg
	mov	al, 3
	out	dx, al
	inc	dx
	in	al, dx
	or	al, 04h
	out	dx, al
	jmp	PMI_done

PMI_GD5430:

Is_Cb2etc_a:
	mov	edx, Graphics_Controller
	mov	al, 0eh
	out	dx, al
	inc	dx
	in	al, dx
	or	al, 3			;2->3
	out	dx, al			;Stop VSYNC, HSYNC

	mov	dx, 0ac6ch		;Set LED to red
	mov	ax, 5412h
	out	dx, ax
	mov	dx, 0ac6eh
	mov	ax, 4120h
	out	dx, ax


PMI_done:

	dec	byte ptr ds:[456h]	; release suspend

	pop	dx
	pop	ebx
	pop	eax
	popad
	ret
HW_PM_IN	endp

;******************************************************************************
; HW_PM_OUT
;
;******************************************************************************
public	HW_PM_OUT
HW_PM_OUT	proc	near
	pushad
	push	ax
	push	dx

	inc	byte ptr ds:[456h]	; block suspend

; Make sure a Attribute Controller Toggle
	mov	edx, CRT_3D4		; dirty VGA:toggle ATR.cont.
	mov	al, 24h			; for index offer
	out	dx, al
	inc	dx
	in	al, dx
	test	al, 080h
	jz	cirrus_off_command2
	mov	edx, attribute_controllerR
	in	al, dx
	mov	edx, attribute_controller
	out	dx, al
cirrus_off_command2:

; Set over scan color
	mov	edx, attribute_controller
	mov	al, 11h
	out	dx, al
	mov	al, bOverScanColor
	out	dx, al

; Move VSYNC/HSYNC

	mov	cl, [PRODUCT_ID]
	mov	bl, cl
	and	bl, 00001000b
	jz	PMO_GD5428

	mov	edx, Graphics_Controller	;------GD5430
	mov	al, 0eh
	out	dx, al
	inc	dx
	in	al, dx
	and	al, 11111100b
	out	dx, al

	mov	dx, 0ac6ch		;Set LED to green
	mov	ax, 5412h
	out	dx, ax
	mov	dx, 0ac6eh
	mov	ax, 4110h
	out	dx, ax
	jmp	@f

PMO_GD5428:				;------GD5428
	mov	edx, GlueIndexReg
	mov	al, 3
	out	dx, al
	inc	dx
	in	al, dx
	and	al, 0fbh
	out	dx, al

@@:
; Set display statuse (for GD5430)
	mov	bl, bDisplayStatus
	or	bl, bl
	jz	@f

; wait 2 seconds for CRT wake-up  
	xor	ecx, ecx
	mov	bl,byte ptr ds:[45Bh]	; get time stamper support
	test	bl,04h
	jz	wait_out5f
					; wait by time stamper
	mov	cx,18			; 2S / 107mS
wait_timestamp_0:
	in	ax,05Eh
	test	ax,40h			; b6 toggle by 107mS
	jnz	wait_timestamp_0
wait_timestamp_1:
	in	ax,05Eh
	test	ax,40h			; b6 toggle by 107mS
	jz	wait_timestamp_1
	loop	wait_timestamp_0
	jmp	short wait_passed

wait_out5f:
; waitting 2 seconds
	push	cx
	mov	cx, 17	; wait time:1.7uS x 65536 x 17 = 1.9S
wait_loop_big:
	push	cx
	xor	cx, cx
wait_loop:
	out	5fh, al
	loop	wait_loop
	pop	cx
	loop	wait_loop_big
	pop	cx
wait_passed:

	mov	edx, attribute_controller
	mov	al, 20h
	out	dx, al

@@:
; Set over scan mode

	mov	cl, [PRODUCT_ID]
	mov	bl, cl
	cmp	bl, vID_Cb2
	jz	Is_Cb2etc_1
	cmp	bl, vID_Cx2
	jz	Is_Cb2etc_1
	jmp	not_Cb2etc
Is_Cb2etc_1:
	mov	bl, bOverScanMode
	mov	edx, GlueIndexReg
	mov	al, 20h
	out	dx, al
	inc	dx
	in	al, dx
	cmp	bl, 0
	jz	OSM_0
	or	al, 00010000b
	jmp	OSM_done
OSM_0:
	and	al, 11101111b
OSM_done:
	out	dx, al

not_Cb2etc:
; Active VSYNC/HSYNC
	cmp	bIsCb2etc, 1
	jz	Is_Cb2etc_2
	and	bl, 00001000b
	jz	Is_GD5428_2

Is_Cb2etc_2:
	mov	edx, Graphics_Controller
	mov	al, 0eh
	out	dx, al
	inc	dx
	in	al, dx
	and	al, 3
	out	dx, al			;Active VSYNC/ HSYNC
	jmp	LCD_on

Is_GD5428_2:

; set LCD BackLight ON
	mov	bl,[PRODUCT_ID]
	cmp	bl, vID_Ts
	jz	@f
	cmp	bl, vID_Ne2
	jz	@f
	cmp	bl, vID_Nd
	jz	@f
	cmp	bl, vID_Ns
	jz	@f
	cmp	bl, vID_Np_Es
	jz	LCD_on_Np
	jmp	LCD_on
@@:
	mov	dx, 08f0h
	mov	ax, 0052h
	out	dx, ax
	out	05fh, al
	mov	dx, 08f2h
	in	ax,dx			; stay other bits
	and	ax,not 0080h
	out	dx, ax
	jmp	short LCD_on

LCD_on_Np:

	mov	edx, MOTHER_PORTBASE
	mov	al,083h
	out	dx,al
	mov	edx, MOTHER_PORTBASE+1
	in	al,dx
	mov	al,03h		;
	out	dx,al

LCD_on:

	dec	byte ptr ds:[456h]	; release suspend

	pop	dx
	pop	ax
	popad
	ret
HW_PM_OUT	endp


VxD_CODE_ENDS


	END
