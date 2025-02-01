PAGE 58,132
;******************************************************************************
TITLE vkdphys.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988
;
;   Title:	vkdphys.asm - physical keyboard services
;
;   Version:	1.00
;
;   Date:	05-Aug-1988
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   05-Aug-1988 RAP
;
;==============================================================================

	.386p



;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

.xlist
	INCLUDE VMM.Inc
	INCLUDE VPICD.Inc
	INCLUDE Debug.Inc
	INCLUDE VTD.Inc
	INCLUDE VKD.Inc
	INCLUDE VKDSYS.Inc
	INCLUDE	SHELL.INC
	INCLUDE OPTTEST.Inc
.list


;******************************************************************************
;			    L O C A L	D A T A
;******************************************************************************

VxD_LOCKED_DATA_SEG

EXTRN	VKD_irq_Handle:DWORD
EXTRN	VKD_CB_Offset:DWORD
EXTRN	VKD_Kbd_Owner:DWORD
EXTRN	VKD_flags:DWORD
ifndef	NEC_98
EXTRN	VKD_8042_owner:DWORD
endif	;NEC_98
EXTRN	NewShell:BYTE
PUBLIC  VKD_Last_LED
PUBLIC  VKD_Last_Key_Rate

VKD_Last_LED        db  0
VKD_Last_Key_Rate   db  0
ifdef	NEC_98
EXTRN	gbl_intdata:BYTE
EXTRN	VKD_MachineID:BYTE
EXTRN	VKD_RepRate:WORD
EXTRN	VKD_RepDelay:WORD
EXTRN	VKD_RepMode:WORD
EXTRN	VKD_Trap_Code:BYTE
EXTRN	physaddr_cmos:DWORD
EXTRN	VKD_gbl_shift_state:DWORD
EXTRN	Note_Cmd_17:BYTE

ifdef	NEC_SHIFT
EXTRN	VKD_Shift_Separate:BYTE
endif	;NEC_SHIFT
endif	;NEC_98

ifndef	NEC_98
PS2_KB_attached		db  0  ; have we ever gotten an IRQ 9 from a PS/2 kb?
endif	;NEC_98

ack_timeoutH	dd  0
ack_response	label dword
ack_res_state	db  0
ack_res_data	db  0
		dw  0

;******************************************************************************
;
; ack handling bits  (used in ack_res_state)
;
ACK_ring0_seq	    equ 80h
ACK_opt_rdy	    equ 40h
ACK_opt_rdy_bit     equ 6


KeyBuf_Size equ 128

	align 4
keybuf_tail dd	0		    ; position of last scan code entered
keybuf_head dd	0		    ; position of next scan code to remove
keybuf	    db	KeyBuf_Size DUP(?)

ifdef	NEC_98

PhysBuf_Size	equ	32			;
PhysBuf_Head	dd	0			;
PhysBuf_Tail	dd	0			;
PhysBuf		db	PhysBuf_Size dup (?)	;

VKD_Rep_Table	label	dword
		;	FEDCBA9876543210	;
		dw	1111111111111111b	;00
		dw	1111111111111111b	;10
		dw	1111111111111111b	;20
		dw	1111111011111111b	;30
		dw	1111111111111111b	;40
		dw	0100000000000011b	;50
		dw	0000000000000011b	;60
		dw	0000000000000000b	;70
ifdef	NEC_SHIFT
PUBLIC	VKD_Phys_KeyState
endif	;NEC_SHIFT
VKD_Phys_KeyState	label	dword
		dw	8 dup (0)		;

VKD_Key_Event	dd	0			;

VKD_RepHand	dd	0			;
VKD_RepCode	db	0			; Scan code saved for S/W repeat
VKD_Kbd2ndID	db	-1			;
VKD_CmdRetry	db	4			;

VKD_LastKeyData	equ	gbl_intdata

endif	;NEC_98

VxD_LOCKED_DATA_ENDS


VxD_CODE_SEG

EXTRN	VKD_VM_Service_Phys:NEAR
ifndef	NEC_98
EXTRN	VKD_8042_Response:NEAR
endif	;NEC_98

VxD_CODE_ENDS


VxD_LOCKED_CODE_SEG

ifndef	NEC_98
EXTRN	Put_8042_Byte:NEAR
endif	;NEC_98

BeginDoc
;******************************************************************************
;
; VKD_Filter_Keyboard_Input
;
; DESCRIPTION:
;		This service can be hooked by a VxD which wishes to
;		modify or reject keyboard input.
; ENTRY:
;		CL = scan code.
; EXIT:
;		Carry Clear => continue processing, CL = new scan code
;		Carry set => Reject keyboard input.
; USES:
;		FLAGS, CL
;==============================================================================
EndDoc
BeginProc VKD_Filter_Keyboard_Input,Async_Service,High_Freq
	clc
	ret
EndProc VKD_Filter_Keyboard_Input

;******************************************************************************
;			I N T E R R U P T   H A N D L E R
;******************************************************************************
ifdef	NEC_98

VKDW	macro	cnt
	local	lab
	mov	ecx, cnt
lab:	out	5Fh, al
	loop	short lab
	endm

;******************************************************************************
;
;   VKD_Int_Handler
;
;   DESCRIPTION:
;
;   ENTRY:	    EAX = IRQ Handle
;		    EBX = Handle of current VM
;
;   EXIT:	    none
;
;   USES:
;
;==============================================================================
BeginProc VKD_Int_Handler, PUBLIC, High_Freq

	add	ebx, [vkd_cb_offset]		;
	in	al, pstat_Kybd			; get status
	test	al, fKBS_ERR			; Q: any error ?
	jz	short VKD_IH_GetData		;    N:
	cmp	[ebx.vkd_retry], 3		;    Y: Q: 4th retry ?
	je	short VKD_IH_GetData		;          Y: 
	inc	[ebx.vkd_retry]			;          N: Inc the counter
	in	al, pdata_Kybd			; Error read data throw away
	mov	al, 14h				; Out DATA's RequestCommand
	out	pcmd_Kybd, al			;
	jmp	short VKD_IH_Ignore		;

VKD_IH_GetData:
	mov	[ebx.vkd_retry], 0		; Clear retry counter
	mov	al, 16h				;

%out	H98!
;;;;	bt	dword ptr [VKD_MachineID], 7	; Q: PC-H98 Series ?
;;;;	adc	al, 0				;    Y: AL=17h

	out	pcmd_Kybd, al			; ERROR RESET, GET ENABLE,
	VKDW	10h				;(37us from interrept)
	in	al, pdata_Kybd			; get data byte from 8042(PC-9800 is 8251A)
	mov	[gbl_intdata], al		; Save IntDATA(NEED)

;
; This code is for DEBUG.
;
	mov	cl, [VKD_Trap_Code]		; Scancode for Debug
	cmp	cl, 0ffh			; Q: 
	jz	short VKD_IH_CheckKey		;    
	xor	cl, al				;
	shl	cl, 1				; Q: Debug Key ?
	jne	short VKD_IH_CheckKey		;    N: 
	jc	short VKD_IH_Ignore		;
	xor	eax, eax			; EAX = 0
	movzx	ebx, [ebp.Client_CS]		; EBX = Client_CS
	mov	ecx, [ebp.Client_EIP]		; ECX = Client_EIP
	bt	[ebp.Client_EFlags], 17		; Q: V86 Mode ?
	jnc	short @f			;    N: 
	dec	eax				;    Y: EAX = -1
	movzx	ecx, cx				;       ECX = Client_IP
@@:	int	1				; into debugger
	jmp	short VKD_IH_Ignore		;

VKD_IH_CheckKey:
	call	VKD_Phys_Put_Byte		; Put a key-code
VKD_IH_Ignore:
	mov	eax, [VKD_irq_Handle]		;
	VxDCall VPICD_Phys_EOI			; EOI the physical interrupt
	clc
	ret

EndProc VKD_Int_Handler


;******************************************************************************
;
;   VKD_Key_Handler
;
;   DESCRIPTION:
;
;   ENTRY:	    none
;
;   EXIT:	    none
;
;   USES:
;
;==============================================================================
BeginProc VKD_Key_Handler, PUBLIC, High_Freq

VKD_KH_Next:
	pushfd
	cli
	mov	ecx, PhysBuf_Head		;
	cmp	ecx, PhysBuf_Tail		; Q: buffer empty?
	jne	short @f			;    N:
	mov	[VKD_Key_Event], 0		;
	popfd
	ret

@@:	inc	ecx				;
	and	ecx, PhysBuf_Size - 1		; wrap around for circular Q
	mov	PhysBuf_Head, ecx		;
	mov	cl, PhysBuf[ecx]		; get byte from buffer
	popfd
;
; Check special key-code
;
	cmp	cl, 0FBh			; Q: Hot connect ?
	je	VKD_KH_Init			;    Y: Initialize and Exit
ifdef	NEC_SHIFT
	cmp	cl, SC_RShf + SC_Break
	je	@f
endif	;NEC_SHIFT
	cmp	cl, KBD_Rsp_ACK			; Q: Is IntDATA responce ?
	jae	VKD_KH_Next			;    Y: Not save into keybuf, NEXT
ifdef	NEC_SHIFT
@@:
endif	;NEC_SHIFT
	call	VKD_Buf_Handler			;

	movzx	eax, cl				;
	and	al, 7Fh				;
	cmp	VKD_RepMode, 3			; Q: Repeat mode is VKD ?
	jnz	short VKD_KH_Next		;    N: Abort
	bt	[VKD_Rep_Table], eax		; Q: Repeat key ?
	jnc	short VKD_KH_Next		;    N: Abort
	or	cl, cl				; Q: Break code ?
	jns	short @f			;    N: Do cancel
	cmp	al, VKD_RepCode			;    Y: Q: Repeating key ?
	jnz	short VKD_KH_Next		;          N: Abort
@@:	xor	esi, esi			;
	xchg	VKD_RepHand, esi		;
	VMMCall	Cancel_Time_Out			; Cancel time-out
	mov	VKD_RepCode, -1			; Clear scancode

	mov	ebx, [VKD_Kbd_Owner]		;
	VMMCall	Test_Sys_VM_Handle		; Q: Owner is Sys-VM ?
	jnz	VKD_KH_Next			;    N: Exit
	or	cl, cl				; Q: Break code ?
	js	VKD_KH_Next			;    Y: Exit

	movzx	eax, VKD_RepDelay		;
	mov	edx, ecx			;
	mov	esi, OFFSET32 VKD_Rep_Handler	; Repeat call-back procedure
	VMMCall	Set_Global_Time_Out		;
	mov	VKD_RepHand, esi		; Save event handle
	mov	VKD_RepCode, dl			; Save Scancode of repeat
	jmp	VKD_KH_Next			;

VKD_KH_Init:
	mov	esi, [VKD_Kbd_Owner]		;
	add	esi, [VKD_CB_Offset]		;
	call	VKD_Phys_Initialize		;
	mov	bl, byte ptr [VKD_gbl_shift_state]
	call	VKD_Phys_SetLEDs		;
	jmp	VKD_KH_Next			;

EndProc VKD_Key_Handler


;******************************************************************************
;
;   VKD_Rep_Handler
;
;   DESCRIPTION:
;
;   ENTRY:	    DL = Scancode
;
;   EXIT:	    none
;
;   USES:
;
;==============================================================================
BeginProc VKD_Rep_Handler, PUBLIC, High_Freq

	mov	VKD_RepHand, 0			;
	mov	ebx, [VKD_Kbd_Owner]		;
	VMMCall	Test_Sys_VM_Handle		; Q: Keyboard owner is Sys-VM ?
	jnz	short VKD_RH_Exit		;    N: Exit

	movzx	eax, VKD_RepRate		;
	mov	esi, OFFSET32 VKD_Rep_Handler	; Repeat call-back procedure
	VMMCall	Set_Global_Time_Out		;
	mov	VKD_RepHand, esi		; Save event handle

VKD_RH_Exit:
	mov	ecx, edx			; CL = Scancode
	Assumes_Fall_Through	VKD_Buf_Handler

EndProc VKD_Rep_Handler


;******************************************************************************
;
;   VKD_Buf_Handler
;
;   DESCRIPTION:
;
;   ENTRY:	    CL = Scancode
;
;   EXIT:	    none
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Buf_Handler, PUBLIC, High_Freq

	pushad
	cmp	[NewShell],1
	jne	VKD_BH_OldShell

	VxDCall	SHELL_Update_User_Activity	; tell shell about it

VKD_BH_OldShell:

	VK_HK_Queue_Out "VKD_Buf_Handler #cl"

	mov	esi, [VKD_CB_Offset]

	VxDCall	VKD_Filter_Keyboard_Input ; give a chance to other VxDs to
					; modify/reject the input.
	jc	short VKD_BH_Exit	; no input to be queued.

	mov	eax, [keybuf_tail]
	inc	eax
	and	eax, KeyBuf_Size - 1	; wrap around for circular Q
	cmp	eax, [keybuf_head]	;Q: buffer full?
	je	short VKD_BH_BufFull	;   Y: ignore scan code

	mov	[keybuf_tail], eax	;   N: update tail index & store code
	mov	[eax+keybuf], cl

VKD_BH_BufFull:
	mov	edx, [VKD_Kbd_Owner]	; pass focus VM handle to server
	add	edx, esi
	cmp	[edx.eventHandle], 0	;Q: already requested?
	jnz	short VKD_BH_Exit	;  Y: don't request again

	mov	esi, OFFSET32 VKD_VM_Service_Phys
	VMMCall Schedule_Global_Event
	mov	[edx.eventHandle], esi
VKD_BH_Exit:
	popad
	ret

EndProc VKD_Buf_Handler


;******************************************************************************
;
;   VKD_Phys_Put_Byte
;
;   DESCRIPTION:
;
;   ENTRY:	    AL = Scancode
;
;   EXIT:	    if ZF=1, buffer is full.
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Phys_Put_Byte, PUBLIC, High_Freq

	pushad
	mov	ecx, VKD_Phys_KeyState[14]	;
	movzx	eax, al				;
	mov	edx, eax			;
	and	al, 7Fh				;
	bts	VKD_Phys_KeyState, eax		;
	or	dl, dl				; Q: Break ?
	jns	short @f			;    N:
	btr	VKD_Phys_KeyState, eax		;    Y:
@@:	cmp	al, 70h				;
	jz	short @f			;
	cmp	al, 7Dh				;
	jnz	short VKD_PPB_Put		;
ifdef	NEC_SHIFT
@@:						;
	cmp	[VKD_Shift_Separate], 1
	jz	shift_sepa_off
	mov	ebx, [VKD_kbd_Owner]
	VMMCall Test_Sys_VM_Handle
	jz	@f
shift_sepa_off:
	and	dl, 0F0h
@@:
else	;NEC_SHIFT
@@:	and	dl, 0F0h			;
endif	;NEC_SHIFT

	mov	eax, VKD_Phys_KeyState[14]	;(xxRxxxxxxxxxxxxL)
	xchg	ah, cl				;(xxxxxxxLxxxxxxxL)
	shr	ecx, 5				;(xxxxxxxRxxxxxxxR)
	or	eax, ecx			;(xxxxxxxSxxxxxxxS)
	xor	al, ah				;(xxxxxxxC)
	shr	al, 1				; Q: Is SHIFT changed
ifdef	NEC_SHIFT
	pushf
	VMMCall Test_Sys_VM_Handle
	jz	WIN_Shift
	popf
	jnc	short @f
	jmp	VKD_PPB_Put
WIN_Shift:
	popf
else	;NEC_SHIFT
	jnc	short @f			;    N: Skip
endif	;NEC_SHIFT
VKD_PPB_Put:
	mov	eax, PhysBuf_Tail		;
	inc	eax				;
	and	eax, PhysBuf_Size - 1		; wrap around for circular Q
	cmp	eax, PhysBuf_Head		; Q: buffer full?
	je	short @f			;    Y:
	mov	PhysBuf_Tail, eax		;    N: update tail index & store code
	mov	PhysBuf[eax], dl		;
	cmp	[VKD_Key_Event], 0		; Q: Event scheduled ?
	jnz	short @f			;    Y: Skip
	mov	esi, OFFSET32 VKD_Key_Handler	;    N: Schedule event
	VMMCall Schedule_Global_Event		;
	mov	[VKD_Key_Event], esi		;
@@:	popad

	ret

EndProc VKD_Phys_Put_Byte

else	;NEC_98

;******************************************************************************
;
;   VKD_Int_09
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = IRQ Handle
;	EBX = Handle of current VM
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VKD_Int_09, PUBLIC, High_Freq

	xor	ecx, ecx
	dec	ch

vi_09_poll_Busy:
	in	al, pstat_Kybd		; get status
	test	al, fKBS_DAV		; Q: Data avail ?
	jnz	@F			;    Y:
	test	al, fKBS_Bsy		;    N: Q: System busy ?
	loopnz	vi_09_poll_Busy		;          Y: Search again
@@:
	mov	cl, al

	mov	eax, [VKD_irq_Handle]
	VxDCall VPICD_Phys_EOI		; EOI the physical interrupt

	cmp	[NewShell],1
	jne	vi_09_oldshell

	VxDCall	SHELL_Update_User_Activity	; tell shell about it

vi_09_oldshell:

	test	cl, fKBS_DAV		;Q: any data available ?
	jz	i9_exit			;   N: ignore int

	in	al, pdata_Kybd		; get data byte from 8042
	mov	cl, al			; save scan code read

	VK_HK_Queue_Out "VKD_Int_09 key #al"

	mov	esi, [VKD_CB_Offset]

	TestMem [VKD_flags], VKDf_wait_cmd_res
	jnz	short Handle_8042_res

	cmp	cl, KBD_Rsp_Ack 	;Q: ACK byte?
	je	short Handle_Ack	;   Y: handle ACK specially

	VxDCall	VKD_Filter_Keyboard_Input ; give a chance to other VxDs to
					; modify/reject the input.
	jc	i9_exit			; no input to be queued.

	mov	eax, [keybuf_tail]
	inc	eax
	and	eax, KeyBuf_Size - 1	; wrap around for circular Q
	cmp	eax, [keybuf_head]	;Q: buffer full?
	je	short buf_full		;   Y: ignore scan code
	mov	[keybuf_tail], eax	;   N: update tail index & store code
	mov	[eax+keybuf], cl

buf_full:
	mov	edx, [VKD_Kbd_Owner]	; pass focus VM handle to server
	add	edx, esi
	cmp	[edx.eventHandle], 0	;Q: already requested?
	jnz	short i9_exit		;  Y: don't request again

	mov	esi, OFFSET32 VKD_VM_Service_Phys
	VMMCall Schedule_Global_Event
	mov	[edx.eventHandle], esi
	jmp	short i9_exit

Handle_Ack:

	;
	;  If a PS/2 keyboard is attached, it sends an ack for each command,
	;  including initialization.  So there is a working PS/2 keyboard
	;  attached IFF we've ever made it here.
	;
	mov [PS2_KB_attached], 1

	mov	edx, OFFSET32 ack_response
.errnz ack_res_state - ack_response
	cmp	byte ptr [edx], 0	;Q: in ring 0 ack sequence?
	jz	short reflect_data	;   N: just pass the ack to VM
	xor	al, al
	xchg	al, [edx+1]		; get response in ax
.errnz ack_res_data - ack_response - 1
	btr	dword ptr [edx], ACK_opt_rdy_bit ;Q: option byte to send?
	jnc	short end_ack_seq	;	    N: clear res state
	VxDcall	VKD_Send_Data
	jmp	short i9_exit

Handle_8042_res:
	mov	edx, [VKD_8042_owner]
	add	esi, edx

	call	Put_8042_Byte
	mov	ch, [esi._8042_expected]
	cmp	ch, VKD_NulTerminated	    ;Q: waiting for nul?
	jne	short no_nul		    ;	N:
	or	cl, cl			    ;	Y: Q: nul found?
	jz	short sch_event
	inc	ch
no_nul:
	dec	ch
	mov	[esi._8042_expected], ch
	jnz	short i9_exit

sch_event:
	mov	al, KBD_Ctl_Ena 	    ; re-enable keyboard
	call	VKD_Send_Cmd
	ClrFlag [VKD_flags], VKDf_wait_cmd_res
reflect_8042_byte:
	mov	esi, OFFSET32 VKD_8042_Response
	sti
	VMMCall	Call_Global_Event

i9_exit:
	clc
	ret

end_ack_seq:
	mov	[ack_res_state], 0	; indicate last ack received
	jmp	i9_exit

reflect_data:
	mov	edx, [VKD_Kbd_Owner]
	add	esi, edx
	call	Put_8042_Byte
	jmp	reflect_8042_byte

EndProc VKD_Int_09
endif	;NEC_98

VxD_LOCKED_CODE_ENDS


VxD_CODE_SEG
ifdef	NEC_98
;******************************************************************************
;
;   VKD_Phys_SendCmd
;
;   DESCRIPTION:
;
;   ENTRY:	    AL = Command Byte
;
;   EXIT:	    none
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Phys_SendCmd

	push	ecx
	push	eax
	VKDW	0FA0h			;(4ms(3~10ms))
	test	[Note_Cmd_17], 01h	; Q: D78011BGC-635 or 691 ?
	jz	short no_635_691	;   N:
	mov	al, 17h			;
	jmp	@f			;
no_635_691:
	mov	al, 37h			;
@@:
	out	pcmd_Kybd, al		;
	VKDW	0300h			;(650us)
	in	al, pstat_Kybd		;
	test	al, fKBS_RDY		; Q: Receve Buffer is Available ?
	jz	short @f		;    N: 
	VKDW	20h			;    Y:(15us)
	in	al, pdata_Kybd		;       Get Data from KB
	cmp	al, [VKD_LastKeyData]	;
	jz	short @f		;
	mov	[VKD_LastKeyData], al	;
	call	VKD_Phys_Put_Byte	; Put a key-code
@@:	VKDW	2			;
	mov	al, 33h			;
	out	pcmd_Kybd, al		;
	VKDW	2			;
	pop	eax			;
	push	eax			;
	out	pdata_Kybd, al		;
@@:	out	5Fh, al			;
	in	al, pstat_Kybd		;
	test	al, fKBS_EMP		; Q: Send Buffer is Empty ?
	jz	short @b		;    N: Loop
	VKDW	2			;    Y:
	mov	al, 37h			;
	out	pcmd_Kybd, al		;
	VKDW	2			;
	mov	al, 16h			;

%out	H98!
;;;;	bt	dword ptr [VKD_MachineID], 7	; Q: PC-H98 Series ?
;;;;	adc	al, 0				;    Y: AL=17h

	out	pcmd_Kybd, al		;
	pop	eax
	pop	ecx
	ret

EndProc VKD_Phys_SendCmd


;******************************************************************************
;
;   VKD_Phys_GetData
;
;   DESCRIPTION:
;
;   ENTRY:	    none
;
;   EXIT:	    CF = 1 : Time Out
;		    CF = 0 : AL = Data Byte
;
;   USES:	    AL, flags
;
;==============================================================================
BeginProc VKD_Phys_GetData

	push	ecx
	VKDW	8			;(15us)
	mov	ecx, 4000h		;(20ms time-out)
@@:	out	5Fh, al			;
	in	al, pstat_Kybd		;
	test	al, fKBS_RDY		; Q: Receve Buffer is Available ?
	jnz	short @f		;    Y: Successful
	loop	short @b		;    N: Q: Time Out ?
	pop	ecx			;          Y: Error
	stc
	ret

@@:	VKDW	20h			;(15us)
	in	al, pdata_Kybd		; Get Data from KB
	pop	ecx			;
	clc
	ret

EndProc VKD_Phys_GetData


;******************************************************************************
;
;   VKD_Phys_Command
;
;   DESCRIPTION:
;
;   ENTRY:	    AH = Command Byte
;
;   EXIT:	    CY = 1 : Error (No response, or NAK)
;
;   USES:	    AL, flags
;
;==============================================================================
BeginProc VKD_Phys_Command

	mov	VKD_CmdRetry, 8
VPC_C:	dec	VKD_CmdRetry
	jz	short VPC_E
	mov	al, ah
	call	VKD_Phys_SendCmd
VPC_D:	call	VKD_Phys_GetData
	jc	short VPC_X
	cmp	al, KBD_Rsp_ACK
	jz	short VPC_X
	cmp	al, KBD_Rsp_NAK
	jz	short VPC_C
	cmp	al, [VKD_LastKeyData]
	jz	short VPC_R
	mov	[VKD_LastKeyData], al
	call	VKD_Phys_Put_Byte	; Put a key-code
VPC_R:	dec	VKD_CmdRetry
	jnz	short VPC_D
VPC_E:	stc
VPC_X:	ret

EndProc VKD_Phys_Command


;******************************************************************************
;
;   VKD_Phys_Cmd2
;
;   DESCRIPTION:
;
;   ENTRY:	    DH = 1st Command Byte
;		    DL = 2nd Command Byte
;
;   EXIT:	    CY = 1 : Error
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Phys_Cmd2

	push	eax
	mov	ah, dh				;
	call	VKD_Phys_Command		;
	jc	short @f			;
	mov	ah, dl				;
	call	VKD_Phys_Command		;
@@:	pop	eax
	ret

EndProc VKD_Phys_Cmd2


;******************************************************************************
;
;   VKD_Phys_Cmd1
;
;   DESCRIPTION:
;
;   ENTRY:	    DL = Command Byte
;
;   EXIT:	    CY = 1 : Error
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Phys_Cmd1

	push	eax
	mov	ah, dl				;
	call	VKD_Phys_Command		;
	pop	eax
	ret

EndProc VKD_Phys_Cmd1


;******************************************************************************
;
;   VKD_Phys_Mask
;
;   DESCRIPTION:
;
;   ENTRY:	    none
;
;   EXIT:	    none
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Phys_Mask

	push	eax
	mov	eax, [VKD_irq_Handle]		; IRQ Handle
	VxDCall	VPICD_Physically_mask		; Mask the IRQ
	pop	eax
	ret

EndProc VKD_Phys_Mask


;******************************************************************************
;
;   VKD_Phys_Unmask
;
;   DESCRIPTION:
;
;   ENTRY:	    none
;
;   EXIT:	    none
;
;   USES:	    none
;
;==============================================================================
BeginProc VKD_Phys_Unmask

	pushfd
	push	eax
	mov	eax, [VKD_irq_Handle]		; IRQ Handle
	VxDCall	VPICD_Physically_Unmask		; Unmask the IRQ
	VxDCall	VPICD_Set_Auto_Masking		;
	pop	eax
	popfd
	ret

EndProc VKD_Phys_Unmask


;******************************************************************************
;
;   VKD_Phys_Initialize
;
;   DESCRIPTION:    Check 2nd ID and set Extend Keyboard Mode
;
;   ENTRY:	    ESI -> VKD CB Ptr
;
;   EXIT:	    none
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Phys_Initialize, PUBLIC

	pushad
	call	VKD_Phys_Mask		;
	mov	[VKD_Kbd2ndID], -1	;
	mov	dl, 96h			;
	call	VKD_Phys_Cmd1		;
	jc	short @f		;
	call	VKD_Phys_GetData	;
	jc	short @f		;
	cmp	al, 0A0h		;
	jnz	short @f		;
	call	VKD_Phys_GetData	;
	jc	short @f		;
	mov	[VKD_Kbd2ndID], al	;
@@:
	cmp	al, 83h			; Q: Win95 Keyboard ?
	jnz	short @f		;    N: Skip
	mov	dh, 95h			;    Y: Set Win95 Mode
	mov	dl, 03h			;
	call	VKD_Phys_Cmd2		;
@@:
	mov	dh, 9Ch			;
	mov	dl, [esi.vkd_repcmd]	;
	call	VKD_Phys_Cmd2		;
	call	VKD_Phys_Unmask		;
	popad
	ret

EndProc VKD_Phys_Initialize


;******************************************************************************
;
;   VKD_Phys_SetKbdMode
;
;   DESCRIPTION:    Set Keyboard Mode Command (95h XXh)
;
;   ENTRY:	    AL = 2nd Command Byte of 
;			01h: Compatible
;			02h: Separate SHIFT
;			03h: Windows95
;
;   EXIT:	    CY = 1 : Error
;
;   USES:	    flags
;
;==============================================================================
;BeginProc VKD_Phys_SetKbdMode, PUBLIC
;
;	push	edx
;	call	VKD_Phys_Mask		;
;	mov	dh, 95h			;
;	mov	dl, al			;
;	call	VKD_Phys_Cmd2		;
;	call	VKD_Phys_Unmask		;
;	pop	edx
;	ret
;
;EndProc VKD_Phys_SetKbdMode


;******************************************************************************
;
;   VKD_Phys_Get2ndID
;
;   DESCRIPTION:    Get Keyboard 2nd ID Command (96h -> A0h, 83h(Win95))
;
;   ENTRY:	    none
;
;   EXIT:	    CY = 1 : AL = -1 (Error)
;			 0 : AL = 2nd ID
;
;   USES:	    EAX, flags
;
;==============================================================================
;BeginProc VKD_Phys_Get2ndID, PUBLIC
;
;	push	edx
;	call	VKD_Phys_Mask		;
;	mov	al, -1			;
;	push	eax			;
;	mov	dl, 96h			;
;	call	VKD_Phys_Cmd1		;
;	jc	short @f		;
;	call	VKD_Phys_GetData	;
;	jc	short @f		;
;	cmp	al, 0A0h		;
;	jnz	short @f		;
;	call	VKD_Phys_GetData	;
;	jc	short @f		;
;	mov	byte ptr [esp], al	;
;@@:	pop	eax			;
;	call	VKD_Phys_Unmask		;
;	push	edx
;	ret
;
;EndProc VKD_Phys_Get2ndID


;******************************************************************************
;
;   VKD_Phys_SetKanaMode
;
;   DESCRIPTION:    Set KANA Synchronization Command (97h XXh)
;
;   ENTRY:	    AL = 2nd Command Byte
;			71h: 
;			72h: 
;
;   EXIT:	    CY = 1 : Error
;
;   USES:	    flags
;
;==============================================================================
;BeginProc VKD_Phys_SetKanaMode, PUBLIC
;
;	push	edx
;	call	VKD_Phys_Mask		;
;	mov	dh, 97h			;
;	mov	dl, al			;
;	call	VKD_Phys_Cmd2		;
;	call	VKD_Phys_Unmask		;
;	pop	edx
;	ret
;
;EndProc VKD_Phys_SetKanaMode


;******************************************************************************
;
;   VKD_Phys_SelfTest
;
;   DESCRIPTION:    Self Test Command (99h -> FBh/FEh,FFh)
;
;   ENTRY:	    none
;
;   EXIT:	    CY = 1 : Error/Alarm
;
;   USES:	    flags
;
;==============================================================================
;BeginProc VKD_Phys_SelfTest, PUBLIC
;
;	mov	al, 99h
;	call	VKD_Phys_SendCmd
;
;
;
;	ret
;
;EndProc VKD_Phys_SelfTest


;******************************************************************************
;
;   VKD_Phys_SetRepMode
;
;   DESCRIPTION:    Set Repeat Mode Command (98h XXh)
;
;   ENTRY:	    AL = 2nd Command Byte
;			71h: 98 Compatible (MAKE/BREAK)
;			72h: AT Compatible (Only MAKE)
;
;   EXIT:	    CY = 1 : Error
;
;   USES:	    flags
;
;==============================================================================
;BeginProc VKD_Phys_SetRepMode, PUBLIC
;
;	push	edx
;	call	VKD_Phys_Mask		;
;	mov	dh, 98h			;
;	mov	dl, al			;
;	call	VKD_Phys_Cmd2		;
;	call	VKD_Phys_Unmask		;
;	pop	edx
;	ret
;
;EndProc VKD_Phys_SetRepMode


;******************************************************************************
;
;   VKD_Phys_SetRepRate
;
;   DESCRIPTION:    Set Repeat Interval Command (9Ch XXh)
;
;   ENTRY:	    AL = 2nd Command Byte
;			70h: Stop repeat
;			XXh: Set Delay/Rate
;
;   EXIT:	    CY = 1 : Error
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Phys_SetRepRate, PUBLIC

	push	edx
	call	VKD_Phys_Mask		;
	mov	dh, 9Ch			;
	mov	[ack_res_data], al	; Sus/Reg
	mov	dl, al			;
	call	VKD_Phys_Cmd2		;
	call	VKD_Phys_Unmask		;
	pop	edx
	ret

EndProc VKD_Phys_SetRepRate


;******************************************************************************
;
;   VKD_Phys_SetLEDs
;
;   DESCRIPTION:    Set LED Command (9Dh 7Xh)
;
;   ENTRY:	    BL = 
;
;   EXIT:	    CY = 1 : Error
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Phys_SetLEDs, PUBLIC

	pushad
	and	bl, 10000110b
	rol	bl, 1
	or	bl, 70h

	mov	[ack_res_data], bl		; Sus/Reg
	call	VKD_Phys_GetLEDs		; Get NUM LEDs
	jc	short @f			;
	and	al, 01h				; if NUM ON, al is 01h
	or	bl, al				; Set LEDs
@@:
	call	VKD_Phys_Mask			;
	mov	dh, 9Dh				;
	mov	dl, bl				;
	call	VKD_Phys_Cmd2			;

	pushf
	pushad
	mov	al, bl				; LED status
	and	al, 01100b			; Mask KANA,CAPS bit
	VxDcall	VKD_Set_LEDs			;
	popad
	popf

	jc	short @f			;
	mov     al, 0Dh				;
	out     68h, al				; write_enable cmos
	mov	al, bl				;
	shl     al, 4				;
        mov     ebx, [physaddr_cmos]		; = physical A000:3000H
        add     ebx, 0FF6h			; = physical A000:3FF6H
	and	al, 0C0h			;
        and     byte ptr [ebx], not 0C0h	;
	or	[ebx], al			;
	mov     al, 0Ch				;
	out     68h, al				; write_disable coms
@@:	call	VKD_Phys_Unmask			;
	popad
	ret

EndProc VKD_Phys_SetLEDs


;******************************************************************************
;
;   VKD_Phys_GetLEDs
;
;   DESCRIPTION:    Get LED Command (9Dh 60h)
;
;   ENTRY:	    none
;
;   EXIT:	    CY = 1 : Error
;			 0 : AL = Response Byte
;
;   USES:	    EAX, flags
;
;==============================================================================
BeginProc VKD_Phys_GetLEDs, PUBLIC

	mov	al, [VKD_MachineID]
	and	al, 01100000b
	cmp	al, 01100000b
	je	short @f
	stc
	ret

@@:	call	VKD_Phys_Mask			;
	push	edx				;
	mov	dh, 9Dh				;
	mov	dl, 60h				;
	call	VKD_Phys_Cmd2			;
	pop	edx				;
	jc	short @f			;
	call	VKD_Phys_GetData		;
@@:	call	VKD_Phys_Unmask			;
	ret					;

EndProc VKD_Phys_GetLEDs

;******************************************************************************
;
;   VKD_Send_Data
;
;   DESCRIPTION:    send a byte of data to the keyboard data port
;
;   ENTRY:	    AL byte to send
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_Send_Data,Async_Service,High_Freq

    cmp     al,KBD_Cmd_LED  ; is this a change in LED status?
    jnz     @F              ; N
    push    eax             ; Y:  save new LED state
    mov     al,[ack_res_data]
    mov     [VKD_Last_LED],al
    pop     eax
    jmp     send_data
@@:
    cmp     al, KBD_Cmd_Rpt ; is this a change in start/repeat interval?
    jnz     send_data       ; N
    push    eax             ; Y:  save new interval state
    mov     al,[ack_res_data]
    mov     [VKD_Last_Key_Rate],al
    pop     eax

send_data:
	ret

EndProc VKD_Send_Data

else	;NEC_98

;******************************************************************************
;
;   Kybd_Wait_Out   Wait till the keyboard controller receives data/cmd
;
;   DESCRIPTION:    Waits for input buffer full status port to clear or timeout
;
;   ENTRY:	    none
;
;   EXIT:	    none
;
;   USES:	    Flags
;
;   ASSUMES:
;	The time spent waiting for output buffer to clear is indeterminate.
;	The assumption is made that it will clear before a tight loop executed
;	64k times can finish.
;
;==============================================================================
BeginProc Kybd_Wait_Out

	push	eax
	push	ecx
	mov	eax, [VKD_irq_Handle]
	or	eax, eax
	jz	short not_hooked1
	VxDCall VPICD_Physically_Mask

not_hooked1:
	mov	ecx, 10000h
Kybd_WO_Lp:
	in	al, pstat_Kybd
	test	al, fKBS_Bsy		; Is buffer still full?
	loopdnz Kybd_WO_Lp		;   Yes, wait till empty or timeout
IFDEF DEBUG
	jz	short kwo_no_timeout
	Trace_Out 'VKD:  wait for busy bit clear timed out'
kwo_no_timeout:
ENDIF
	mov	eax, [VKD_irq_Handle]
	or	eax, eax
	jz	short not_hooked2
	VxDCall VPICD_Physically_Unmask

not_hooked2:
	pop	ecx
	pop	eax
	ret

EndProc Kybd_Wait_Out


;******************************************************************************
;
;   VKD_Send_Data
;
;   DESCRIPTION:    send a byte of data to the keyboard data port
;
;   ENTRY:	    AL byte to send
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_Send_Data,Async_Service,High_Freq

    cmp     al,KBD_Cmd_LED  ; is this a change in LED status?
    jnz     @F              ; N
    push    eax             ; Y:  save new LED state
    mov     al,[ack_res_data]
    mov     [VKD_Last_LED],al
    pop     eax
    jmp     send_data
@@:
    cmp     al, KBD_Cmd_Rpt ; is this a change in start/repeat interval?
    jnz     send_data       ; N
    push    eax             ; Y:  save new interval state
    mov     al,[ack_res_data]
    mov     [VKD_Last_Key_Rate],al
    pop     eax

send_data:
	call	Kybd_Wait_Out		; Wait for previous command to clear
	out	pdata_Kybd, al		; send keyboard command byte
	ret

EndProc VKD_Send_Data



;******************************************************************************
;
;   VKD_Send_Cmd
;
;   DESCRIPTION:    send a byte of data to the keyboard command port
;
;   ENTRY:	    AL byte to send
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_Send_Cmd

	call	Kybd_Wait_Out		    ; Wait for previous command to clear
	out	pcmd_Kybd, al
	ret

EndProc VKD_Send_Cmd
endif	;NEC_98

;******************************************************************************
;   P U B L I C   S E R V I C E S
;******************************************************************************

;******************************************************************************
;
;   VKD_Buf_Empty
;
;   DESCRIPTION:    determine if the input buffer is empty
;
;   ENTRY:
;
;   EXIT:	    Z flag set, if buffer is empty
;
;   USES:	    EAX
;
;==============================================================================

BeginProc VKD_Buf_Empty, PUBLIC

	mov	eax, [keybuf_head]
	cmp	eax, [keybuf_tail]	;Q: buffer empty?
	ret

EndProc VKD_Buf_Empty


;******************************************************************************
;
;   VKD_Get_Byte
;
;   DESCRIPTION:    get next available byte from input queue, if possible
;
;   ENTRY:	    nothing
;
;   EXIT:	    al byte from queue
;		    Carry flag set, if queue empty
;
;   USES:	    EAX, Carry flag
;
;==============================================================================

BeginProc VKD_Get_Byte, PUBLIC, High_Freq

	stc
	pushfd
	cli
	mov	eax, [keybuf_head]
	cmp	eax, [keybuf_tail]	;Q: buffer empty?
	je	short gb_exit		;   Y: ret
	inc	eax
	and	eax, KeyBuf_Size - 1	; wrap around for circular Q
	mov	[keybuf_head], eax
	mov	al, [eax+keybuf]	; get byte from buffer
	and	dword ptr [esp], NOT CF_Mask	; clear carry
gb_exit:
	popfd
	ret

EndProc VKD_Get_Byte


;******************************************************************************
;
;   VKD_Put_Byte
;
;   DESCRIPTION:    put a byte into the input queue, if possible
;
;   ENTRY:	    AL is byte to input
;
;   EXIT:	    Carry flag set, if queue full
;
;   USES:	    nothing
;
;==============================================================================

BeginProc VKD_Put_Byte, PUBLIC, Async_Service

	pushfd
	push	ebx
	cli
	mov	ebx, [keybuf_tail]
	inc	ebx
	and	ebx, KeyBuf_Size - 1	; wrap around for circular Q
	cmp	ebx, [keybuf_head]	;Q: buffer full?
	je	short pb_buf_full	;   Y:
	mov	[keybuf_tail], ebx	;   N: update tail index & store code
ifdef	NEC_98
ifdef	NEC_SHIFT
	cmp	al, 7Dh			;Q: R-Shift ?
	jz	short shift_chk		;   Y:
	cmp	al, 0FDh		;Q: R-Shift+Break?
	jnz	short no_shift		;   N:
shift_chk:
	cmp	[VKD_Shift_Separate], 1	;Q: ShiftSeparate=Off?
	jz	short shift_sepa_off2	;   Y:
	push	ebx
	mov	ebx, [VKD_kbd_Owner]
	VMMCall	Test_Sys_VM_Handle	;Q: DOS prt?
	pop	ebx
	jz	short no_shift		;   N:
shift_sepa_off2:
	and	al, 0F0h		;   Y:Shift = 70h
no_shift:
endif	;NEC_SHIFT
endif	;NEC_98
	mov	[ebx+keybuf], al
	pop	ebx
	popfd
	clc
	jmp	short pb_exit
pb_buf_full:
	pop	ebx
	popfd
	stc
pb_exit:
	ret

EndProc VKD_Put_Byte


VxD_Code_Ends

VxD_Pageable_Code_Seg

ifndef	NEC_98
;******************************************************************************
;
;   VKD_Enable_Kbd
;
;   DESCRIPTION:    send command to 8042 to enable the keyboard
;
;   ENTRY:	    nothing
;
;   EXIT:	    nothing
;
;   USES:	    nothing
;
;==============================================================================

BeginProc VKD_Enable_Kbd, PUBLIC

	push	eax
	mov	al, KBD_Ctl_Ena
	call	VKD_Send_Cmd		; send keyboard command byte
	pop	eax
	ret

EndProc VKD_Enable_Kbd

;******************************************************************************
;
;   VKD_Disable_Kbd
;
;   DESCRIPTION:    send command to 8042 to disable the keyboard
;
;   ENTRY:	    nothing
;
;   EXIT:	    nothing
;
;   USES:	    nothing
;
;==============================================================================

BeginProc VKD_Disable_Kbd, PUBLIC

	push	eax
	mov	al, KBD_Ctl_Dis
	call	VKD_Send_Cmd		; send keyboard command byte
	pop	eax
	ret

EndProc VKD_Disable_Kbd
endif	;NEC_98

VxD_Pageable_Code_Ends

VxD_Code_Seg

;******************************************************************************
;
;   VKD_Clear_Data_Buf
;
;   DESCRIPTION:    clear the keyboard data buffer port
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VKD_Clear_Data_Buf, PUBLIC

	push	eax
clr_lp:
	in	al, pstat_Kybd		; get status
ifdef	NEC_98
	test	al, fKBS_RDY		;Q: any data available?
else	;NEC_98
	test	al, fKBS_DAV		;Q: any data available?
endif	;NEC_98
	jz	short clr_exit		;  N: all done

	in	al, pdata_Kybd		; get data byte for 8042
	jmp	clr_lp

clr_exit:
	pop	eax
	ret

EndProc VKD_Clear_Data_Buf


ifndef	NEC_98
;******************************************************************************
;
;   VKD_Poll_ACK
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
BeginProc VKD_Poll_ACK

	push	eax

	;
	;  If there is no PS/2 keyboard attached, we don't want to poll
	;  on set-LEDs and set-key-rate because they will cause delayed
	;  reaction in the UI.  It's especially bad for set-key-rate becaause
	;  the keyboard control panel applet's slider will get very jumpy.
	;
    cmp		[PS2_KB_attached], 0
	je		poll_ack_done
	VxdCall VTD_Get_Real_Time
	mov	esi, eax
vsl_8042_busy:
	cmp	[ack_res_state], 0	;Q: waiting for ack response?
	je	short poll_ack_done		;   N: it happened
	VxdCall VTD_Get_Real_Time
	sub	eax, esi
	cmp	eax, 1000000		;Q: ACK taken longer than .8 sec?
	jb	vsl_8042_busy		;   N: loop
poll_ack_done:
	pop	eax
	ret

EndProc VKD_Poll_ACK
endif	;NEC_98

;******************************************************************************
;
;   VKD_Set_LEDs
;
;   DESCRIPTION:
;
;   ENTRY:	    AL = LED status to send to the keyboard
;
;   EXIT:
;
;   USES:	    flags
;
;==============================================================================

BeginProc VKD_Set_LEDs, PUBLIC, Async_Service
ifdef	NEC_98

	pushad
	
	mov	[ack_res_data], al
	mov	al, KBD_Cmd_LED
	VxDcall	VKD_Send_Data

	popad
	ret

else	;NEC_98
	pushad

	call	VKD_Poll_ACK
	mov	[ack_res_data], al
	mov	[ack_res_state], ACK_ring0_seq + ACK_opt_rdy ; set flag bits
					; to indicate option byte to be sent
	mov	al, KBD_Cmd_LED
	VxDcall	VKD_Send_Data

	xor	esi, esi
	xchg	esi, [ack_timeoutH]
	VMMCall Cancel_Time_Out
	mov	eax, 1000		; 1 sec timeout
	xor	edx, edx
	mov	esi, OFFSET32 VKD_ACK_Timeout
	VMMCall Set_Global_Time_Out
	mov	[ack_timeoutH], esi
	popad
	ret
endif	;NEC_98
EndProc VKD_Set_LEDs


;******************************************************************************
;
;   VKD_Set_Key_Rate
;
;   DESCRIPTION:    set the keyboard repeat & delay rates
;
;   ENTRY:	    al is repeat & delay values to set
;
;			0ddrrrrr	dd    = 2 bits of delay rate
;					rrrrr = 5 bits of repeat rate
;
;   EXIT:
;
;   USES:	    flags
;
;==============================================================================

BeginProc VKD_Set_Key_Rate, PUBLIC, Async_Service
ifdef	NEC_98

	pushad

	mov	[ack_res_data], al
	mov	al, KBD_Cmd_Rpt
	VxDcall	VKD_Send_Data
	popad
	ret

else	;NEC_98

	pushad

	call	VKD_Poll_ACK
	mov	[ack_res_data], al
	mov	[ack_res_state], ACK_ring0_seq + ACK_opt_rdy ; set flag bits
					; to indicate option byte to be sent
	mov	al, KBD_Cmd_Rpt
	VxDcall	VKD_Send_Data

	xor	esi, esi
	xchg	esi, [ack_timeoutH]
	VMMCall Cancel_Time_Out
	mov	eax, 1000		; 1 sec timeout
	xor	edx, edx
	mov	esi, OFFSET32 VKD_ACK_Timeout
	VMMCall Set_Global_Time_Out
	mov	[ack_timeoutH], esi
	popad
	ret
endif	;NEC_98
EndProc VKD_Set_Key_Rate


;******************************************************************************
;
;   VKD_Get_Key_Rate
;
;   DESCRIPTION:    get the keyboard repeat & delay rates
;
;   ENTRY:
;
;   EXIT:           AL is repeat & delay values last set by VKD_Set_Key_Rate
;                   (regardless of whether or not VKD_Set_Key_Rate succeeded)
;
;			0ddrrrrr	dd    = 2 bits of delay rate
;					rrrrr = 5 bits of repeat rate
;
;      
;   USES:	    flags
;
;==============================================================================

BeginProc VKD_Get_Key_Rate, PUBLIC, Async_Service

        mov     al, [VKD_Last_Key_Rate]
        ret

EndProc VKD_Get_Key_Rate


ifndef	NEC_98
;******************************************************************************
;
;   VKD_ACK_Timeout
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
BeginProc VKD_ACK_Timeout

	mov	[ack_timeoutH], 0
	cmp	[ack_res_state], 0
	je	short skip_enable
	Trace_Out 'VKD: wait for ACK timed out'
	xor	eax, eax
	mov	[ack_response], eax
	mov	al, KBD_Cmd_Enb 	    ; try sending an enable to the
	VxDcall	VKD_Send_Data		    ; keyboard
skip_enable:
	ret

EndProc VKD_ACK_Timeout
endif	;NEC_98


IFDEF Support_Reboot

VxD_DATA_SEG
EXTRN VKD_attempt_reboot:BYTE
VxD_DATA_ENDS

;******************************************************************************
;
;   VKD_Reboot
;
;   DESCRIPTION:    try to reboot the system using the system dependent way
;		    by trying to toggle the reset line through the 8042
;
;   ENTRY:
;
;   EXIT:	    System will reset, or return to caller
;
;   USES:
;
;==============================================================================
BeginProc VKD_Reboot, PUBLIC

ifdef	NEC_98
	mov	al,92h
	out	37h,al
	mov	al,07h
	out	37h,al
	mov	al,0bh
	out	37h,al
	mov	al,0fh
	out	37h,al
	mov	al,08h
	out	37h,al
	out	50h,al

;Å@Graph&Text display off for RESET infomation 
	mov	al,05h
	out	0a2h,al
	out	62h,al

	mov	ecx, 40000h
	loop	$
	mov	al, 00h
	out	0f0h, al			; CPU Reset
	jmp	$				; For write cache machine
else	;NEC_98
if 0
	;
	; The call to MapPhysToLinear, below, causes all sorts of problems
	; during shutdown, especially if the swapfile is closed.  We'll just
	; write directly to the physical address instead.
	;
	cmp	[VKD_attempt_reboot], 0
	je	short reboot_exit

	VMMCall _MapPhysToLinear,<472h,2,0>
	inc	eax
IFDEF	DEBUG
	jnz	SHORT VR_GotPhysAddr
Debug_Out "VKD: Cannot address reboot parameter at 472h"
VR_GotPhysAddr:
ENDIF
	jz	SHORT VR_NoWarm
	dec	eax
endif
	mov	eax, 472h
	mov	word ptr [eax], 1234h	    ; specify warm boot
VR_NoWarm:
%OUT	THIS SHOULD BE PARAMETERIZED in WIN.INI!!! 2.1 had at least one test!
	mov	al, KBD_Ctl_Pulse OR 1110b  ; toggle the reset line
	call	VKD_Send_Cmd
endif	;NEC_98

reboot_exit:
	clc
	ret				    ; failed, if we got this far

EndProc VKD_Reboot

ENDIF


IFDEF DEBUG

;******************************************************************************
;
;   Dump_KeyBuf
;
;   DESCRIPTION:    Debug code to dump the global keyboard buffer
;
;   From keybuf_head to keybuf_tail are global keys not yet handled.
;   From keybuf_tail to keybuf_head are global keys already handled.
;
;   ENTRY:	    nothing
;
;   EXIT:	    nothing
;
;   USES:	    EAX, EBX, Flags
;
;==============================================================================

BeginProc Dump_KeyBuf

	Trace_Out "global keybuf =" /noeol
	mov	ebx, [keybuf_head]
dkb_lp1:
	cmp	ebx, [keybuf_tail]	;Q: buffer empty?
	je	short dkb_exit1		;   Y: ret
	inc	ebx
	and	ebx, KeyBuf_Size - 1	; wrap around for circular Q
	mov	al, [ebx+keybuf]	; get byte from buffer
	Trace_Out ' #al' /noeol
	jmp	dkb_lp1
dkb_exit1:
	Trace_Out ' '

	Trace_Out "recent global keybuf =" /noeol
	mov	ebx, [keybuf_tail]
dkb_lp2:
	mov	al, [ebx+keybuf]	; get byte from buffer
	Trace_Out ' #al' /noeol
	inc	ebx
	and	ebx, KeyBuf_Size - 1	; wrap around for circular Q
	cmp	ebx, [keybuf_head]	;Q: buffer empty?
	jne	short dkb_lp2		;   N: keep going
	Trace_Out ' '

	ret

EndProc Dump_KeyBuf

ENDIF

VxD_CODE_ENDS

	END
