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

;PAGE 58,132
;******************************************************************************
;TITLE vkd98.asm -
;******************************************************************************
;
;   Title:	vkd98.asm
;
;   Version:	1.00
;
;==============================================================================

;	.386p


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************
;
;.XLIST
;	INCLUDE VMM.INC
;	INCLUDE Debug.INC
;	INCLUDE VPICD.INC
;	INCLUDE VSD.INC
;	INCLUDE VDD.INC
;	INCLUDE	SHELL.INC
;	INCLUDE	PIF.INC
;	INCLUDE VMPOLL.INC
;	INCLUDE VKD.INC
;	INCLUDE VKDSYS.INC
;	INCLUDE OPTTEST.Inc
;	INCLUDE INT2FAPI.INC
;       INCLUDE MSGMACRO.INC
;       INCLUDE VKDMSG.INC
;       INCLUDE V86MMGR.INC
;
;.LIST

VxD_DATA_SEG

vkd_1st_cmd	db	0
vkd_2nd_cmd	db	0

Hex_SCs		db	0ah,01h,02h,03h,04h,05h,06h,07h,08h,09h
		db	1dh,2dh,2bh,1fh,12h,20h

chk_atok6	db	"ATOK50200",1,"ATOK60"

nul_device_addr	dd	0			; linear offset of NUL device

xlat_atok7	db	00h, 02h, 03h, 04h, 06h, 07h, 08h, 12h, 22h, 03h, 02h

atok7_buffer	db	512 dup (?)

VxD_DATA_ENDS


VxD_CODE_SEG
;******************************************************************************
;
;   check_int6f
;
;   DESCRIPTION:    
;
;
;   ENTRY:	EDI is points to VKD data in VM's CB
;   		EBX is VM handle
;
;   EXIT:	CY = 1		not exist ATOK6/7
;		CY = 0, ZR = 0	exist ATOK7
;		CY = 0, ZR = 1	exist ATOK6
;
;   USES:	EAX, ECX, ESI , FLAGS
;
;==============================================================================
BeginProc check_int6f

	mov	esi, [ebx.CB_High_Linear]	; check INT 6F
	mov	eax, [esi+6fh*4]
	push	eax
	movzx	eax, ax
	add	esi, eax
	pop	eax
	xor	ax, ax
	shr	eax, 12
	add	esi, eax

	mov	[edi.atok_warning], ATOK_WARNING_7TO6
	cmp	[esi+45h], dword ptr "6OT7"	; check "7TO6.COM"
	jz	short thereis_atok		; mean atok6
	mov	[edi.atok_warning], 0

	cmp	[esi+30h], byte ptr "A"		; check ATOK7
	jnz	short check_atok6
	cmp	[esi+33h], word ptr "7K"
	jnz	short check_atok6
	cmp	[esi+3ah], byte ptr "1"
	jz	short check_atok6
thereis_atok7:
	or	ebx, ebx
	jmp	short thereis_atok

check_atok6:
	add	esi, 3				; check ATOK6
	push	edi
	cld
	mov	edi, offset32 chk_atok6
	mov	ecx, 4
	repe cmpsd
	pop	edi
	jnz	short non_atok
thereis_atok:
	clc
	ret

non_atok:
	stc
	ret

EndProc check_int6f

BeginDoc
;******************************************************************************
;
;   set_paste_normal
;
;   DESCRIPTION:    
;
;
;   ENTRY:	EDI is points to VKD data in VM's CB
;		EBX is VM handle
;
;   EXIT:
;
;   USES:	EAX, EDX, ESI, flags
;
;==============================================================================
EndDoc
BeginProc set_paste_normal

;Save VM's shift state, when paste is started.
;
	mov	ax, [edi.loc_shift_state]
	mov	[edi.before_shift_paste], ax

; set timeout to get server started
;
	mov	eax, [VKD_INT9_Paste_Delay]
	mov	edx, edi		    ; ptr to VKD data in CB is reference data
	mov	esi, OFFSET32 VKD_VM_Server
	VMMCall Set_VM_Time_Out
	VMMcall	Wake_Up_VM
	ret

EndProc set_paste_normal

BeginDoc
;******************************************************************************
;
;   set_paste_dos
;
;   DESCRIPTION:    
;
;
;   ENTRY:	EDI is points to VKD data in VM's CB
;		EBX is VM handle
;
;   EXIT:	CY = 0 : done
;		   = 1 : not exist 'CON'
;
;   USES:	EAX, EDX, ESI, FLAG
;
;==============================================================================
EndDoc
BeginProc set_paste_dos, PUBLIC

	mov	esi, [nul_device_addr]		; check 'CON' device
	add	esi, [ebx.CB_High_Linear]

loop_search_con:
	mov	eax, [esi]
	cmp	eax, 0ffffffffh
	jz	spd_not_exist
	mov	edx, eax
	movzx	esi, ax
	xor	ax, ax
	shr	eax, 12
	add	esi, eax
	add	esi, [ebx.CB_High_Linear]

	cmp	dword ptr [esi+10], ' NOC'
	jnz	loop_search_con
	cmp	dword ptr [esi+14], '    '
	jnz	loop_search_con
	mov	[edi.vkd_paste_mode], PASTE_MODE_DOS	; mode of paste to dos

						; hiword of EDX is segment
	mov	dx, [esi+6]			; strategy address(offset)
	mov	[edi.dos_strategy], edx
	push	dx
	mov	dx, [esi+8]			; intentry address(offset)
	mov	[edi.dos_intentry], edx
	movzx	eax, dx
	xor	dx, dx
	shr	edx, 12
	add	edx, [ebx.CB_High_Linear]
	add	eax, edx
	mov	[edi.linear_intentry], eax
	xor	eax, eax
	pop	ax
	add	eax, edx
	mov	[edi.linear_strategy], eax

	mov	esi, eax			; Linear address of strategy
	mov	ax, [esi]			; get code of strategy
	mov	[edi.code_strategy], ax		; save code of strategy
	mov	[esi], word ptr 6fcdh		; put code of INT 6Fh

	mov	eax, 1000
	mov	esi, OFFSET32 vkd_try_paste_atok
	VMMCall Set_VM_Time_Out
	mov	[edi.dos_timeout], esi

	VMMcall	Wake_Up_VM
	clc
	ret

spd_not_exist:
	stc
	ret

EndProc set_paste_dos

BeginProc vkd_try_paste_atok, PUBLIC

	mov	edi, ebx
	add	edi, [VKD_CB_Offset]

	mov	esi, [edi.linear_strategy]
	or	esi, esi
	jz	short tpa_1
	cmp	word ptr [esi], 6fcdh
	jnz	short tpa_1
	mov	ax, [edi.code_strategy]
	mov	[esi], ax
tpa_1:
	mov	esi, [edi.linear_intentry]
	or	esi, esi
	jz	short tpa_2
	cmp	word ptr [esi], 6fcdh
	jnz	short tpa_2
	mov	ax, [edi.code_intentry]
	mov	[esi], ax
tpa_2:
	call	check_int6f
	jc	short tpa_paste_int9
	jz	short tpa_paste_atok6
	call	set_paste_atok7			; atok7 paste
	jc	short tpa_paste_int9
	jmp	short tpa_paste_save
tpa_paste_atok6:
	call	set_paste_atok6			; atok6 paste
	jnc	short tpa_paste_save
tpa_paste_int9:
	mov	[edi.vkd_paste_mode], PASTE_MODE_INT9	; int9 paste
tpa_paste_save:
	call	set_paste_normal		; normal paste
	ret

EndProc vkd_try_paste_atok

BeginDoc
;******************************************************************************
;
;   set_paste_atok6
;
;   DESCRIPTION:    
;
;
;   ENTRY:	    EDI is points to VKD data in VM's CB
;		    EBX is VM handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================
EndDoc
BeginProc set_paste_atok6

	mov	[edi.vkd_paste_mode], PASTE_MODE_ATOK6	; ATOK6
	mov	[edi.atok_num_mode], 5
	mov	[edi.atok_num_ank], 3
	mov	[edi.atok_num_code], 4
	inc	[edi.vkd_paste_flag]

	mov	ah, 86h
	call	call_atok6
	test	al, 00110000b
	jnz	can_not_paste_atok6
	test	al, 00000001b
	jz	short @f
	or	[edi.atok_warning], ATOK_WARNING_REST
@@:
	test	al, 00001110b
	jnz	short @f
	or	[edi.atok_warning], ATOK_WARNING_OFF
@@:
	shr	al, 1
	and	al, 7
	mov	[edi.atok_save], al
	mov	[edi.atok_mode], al

	mov	ah, 6bh
	call	call_atok6
	mov	[edi.atok_code], al

	mov	ah, 64h
	call	call_atok6
	mov	[edi.atok_key], dl

	test	[edi.atok_warning], ATOK_WARNING_7TO6
	jnz	short paste_7to6
	mov	ah, 66h
	call	call_atok6
	mov	[edi.atok_num_def], ah
	jmp	short seted_paste_atok6
paste_7to6:
	cmp	[edi.atok_mode], 1
	jbe	short @f
	dec	[edi.atok_mode]
	dec	[edi.atok_save]
@@:
	mov	[edi.atok_num_mode], 4
	mov	[edi.atok_num_ank], 2
	mov	[edi.atok_num_code], 3
	mov	[edi.atok_num_def], 1
seted_paste_atok6:

	test	[edi.atok_warning], ATOK_WARNING_OFF
	jz	short not_off6
	mov	al, [edi.atok_num_def]
	mov	[edi.atok_mode], al
not_off6:
	test	[edi.atok_warning], ATOK_WARNING_REST
	jz	short not_rest6
	mov	al, [edi.atok_num_code]
	cmp	[edi.atok_mode], al
	jnz	short not_rest6
	xor	[edi.atok_warning], ATOK_WARNING_REST or ATOK_WARNING_CODE
not_rest6:

	dec	[edi.vkd_paste_flag]
	VMMcall	Wake_Up_VM
	clc
	ret

can_not_paste_atok6:
	dec	[edi.vkd_paste_flag]
	stc
	ret

EndProc set_paste_atok6

BeginDoc
;******************************************************************************
;
;   set_paste_atok7
;
;   DESCRIPTION:    
;
;
;   ENTRY:	    EDI is points to VKD data in VM's CB
;		    EBX is VM handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================
EndDoc
BeginProc set_paste_atok7

	mov	esi, [ebx.CB_High_Linear]
	mov	eax, [esi+6fh*4]
	add	ax, 49h
	mov	[edi.atok7_entry], eax
	movzx	edx, ax
	xor	ax, ax
	shr	eax, 12
	add	eax, edx
	add	eax, [ebx.CB_High_Linear]
	mov	[edi.linear_atok7], eax
	xchg	eax, ebx
	mov	dx, [ebx]
	mov	word ptr [ebx], 6fcdh			; int 6f
	xchg	eax, ebx
	mov	[edi.code_atok7], dx

	mov	[edi.vkd_paste_mode], PASTE_MODE_ATOK7	; ATOK7
	mov	[edi.atok_num_mode], 4
	mov	[edi.atok_num_ank], 2
	mov	[edi.atok_num_code], 3
	mov	[edi.atok_num_def], 1
	inc	[edi.vkd_paste_flag]

	mov	ah, 86h
	call	call_atok7
	test	al, 00110000b
	jnz	short can_not_paste_atok7
	test	al, 00000001b
	jz	short @f
	or	[edi.atok_warning], ATOK_WARNING_REST
@@:
	test	al, 00001110b
	jnz	short @f
	or	[edi.atok_warning], ATOK_WARNING_OFF
@@:
	shr	al, 1
	and	al, 7
	mov	[edi.atok_save], al
	mov	[edi.atok_mode], al

	mov	ah, 6bh
	call	call_atok7
	mov	[edi.atok_code], al

	mov	ah, 64h
	call	call_atok7
	mov	[edi.atok_key], dl

	test	[edi.atok_warning], ATOK_WARNING_OFF
	jz	short not_off7
	mov	[edi.atok_mode], 1
not_off7:
	test	[edi.atok_warning], ATOK_WARNING_REST
	jz	short not_rest7
	cmp	[edi.atok_mode], 3
	jnz	short not_rest7
	xor	[edi.atok_warning], ATOK_WARNING_REST or ATOK_WARNING_CODE
not_rest7:

	dec	[edi.vkd_paste_flag]
	VMMcall	Wake_Up_VM
	clc
	ret

can_not_paste_atok7:
	dec	[edi.vkd_paste_flag]
	stc
	ret

EndProc set_paste_atok7

BeginDoc
;******************************************************************************
;
;   vkd_int_6f
;
;   DESCRIPTION:    
;
;
;   ENTRY:	    EAX is int number
;		    EBX is VM handle
;		    EBP is points to the client frame on the stack
;
;   EXIT:
;
;   USES:
;
;==============================================================================
EndDoc
BeginProc vkd_int_6f, PUBLIC

	pushad
	mov	edi, ebx
	add	edi, [VKD_CB_Offset]

	mov	ax, [ebp.Client_CS]
	shl	eax, 16
	mov	ax, [ebp.Client_IP]
	dec	eax
	dec	eax

	cmp	eax, [edi.dos_strategy]
	jnz	short not_strategy_int6f
	call	vkd_hook_strategy		; emulate strategy
	jmp	short done_int6f_emulate
not_strategy_int6f:
	cmp	eax, [edi.dos_intentry]
	jnz	short not_intentry_int6f
	call	vkd_hook_intentry		; emulate intentry
	jmp	short done_int6f_emulate
not_intentry_int6f:
	cmp	eax, [edi.atok7_entry]
	jnz	short atok6_api
	call	vkd_hook_atok7			; emulate atok7-api
done_int6f_emulate:
	VMMcall	Simulate_Far_Ret
	popad
	clc
	ret

atok6_api:
	popad
	stc
	ret

EndProc vkd_int_6f

BeginDoc
;******************************************************************************
;
;   vkd_hook_strategy
;
;   DESCRIPTION:    
;
;
;   ENTRY:	    EAX is Client CS:IP
;		    EBX is VM handle
;		    EDX is reference data
;		    ESI is linear address of break point(CS << 4+IP)
;   		    EDI is points to VKD data in VM's CB
;		    EBP is points to the client frame on the stack
;
;   EXIT:
;
;   USES:	EAX, ECX, EDX, ESI, flags
;
;==============================================================================
EndDoc
BeginProc vkd_hook_strategy

	mov	esi, [edi.linear_strategy]
	mov	ax, [edi.code_strategy]
	mov	[esi], ax

	xor	esi, esi
	xchg	esi, [edi.dos_timeout]
	VMMCall Cancel_Time_Out

	movzx	eax, [ebp.Client_ES]
	shl	eax, 4
	mov	[edi.request_header],eax
	movzx	eax, [ebp.Client_BX]
	add	[edi.request_header],eax

	VMMcall	Begin_Nest_V86_Exec
	mov	cx, word ptr [edi.dos_strategy+2]
	movzx	edx, word ptr [edi.dos_strategy]
	VMMcall	Simulate_Far_Call
	VMMcall	Resume_Exec
	VMMcall	End_Nest_Exec

	mov	esi, [edi.linear_strategy]
	mov	[esi], word ptr 6fcdh		; put code of INT 6Fh

	cmp	[edi.vkd_paste_mode], PASTE_MODE_DOS
	jnz	short not_paste_vm

	mov	esi, [edi.linear_intentry]	; Linear address of strategy
	mov	ax, [esi]			; get code of strategy
	cmp	ax, 6fcdh
	jz	short not_paste_vm
	mov	[edi.code_intentry], ax		; save code of strategy
	mov	[esi], word ptr 6fcdh		; put code of INT 6Fh

not_paste_vm:
	ret

EndProc vkd_hook_strategy

BeginDoc
;******************************************************************************
;
;   vkd_hook_intentry
;
;   DESCRIPTION:    
;
;
;   ENTRY:	    EAX is Client CS:IP
;		    EBX is VM handle
;		    EDX is reference data
;		    ESI is linear address of break point(CS << 4+IP)
;   		    EDI is points to VKD data in VM's CB
;		    EBP is points to the client frame on the stack
;
;   EXIT:
;
;   USES:	EAX, ECX, EDX, ESI, flags
;
;==============================================================================
EndDoc
BeginProc vkd_hook_intentry

	mov	esi, [edi.linear_intentry]
	mov	ax, [edi.code_intentry]
	mov	[esi], ax

	cmp	[edi.vkd_paste_mode], PASTE_MODE_DOS
	jz	short paste_intentry
not_paste_intentry:
	VMMcall	Begin_Nest_V86_Exec
	mov	cx, word ptr [edi.dos_intentry+2]
	movzx	edx, word ptr [edi.dos_intentry]
	VMMcall	Simulate_Far_Call
	VMMcall	Resume_Exec
	VMMcall	End_Nest_Exec
	jmp	simulate_intentry_ret

paste_intentry:
	mov	eax, 1000
	mov	esi, OFFSET32 vkd_try_paste_atok
	VMMCall Set_VM_Time_Out
	mov	[edi.dos_timeout], esi

	mov	ecx, [edi.paste_ptr]
	mov	esi, [ebx.CB_High_Linear]
	mov	edx, esi
	add	esi, [edi.request_header]

	mov	al, [esi._cmd]
	cmp	al, 4
	jnz	short not_cmd_4

	xchg	ecx, esi
	mov	al, [esi.Paste_Char]
	xchg	ecx, esi
	add	ecx,SIZE Paste_Rec
	mov	[edi.paste_ptr], ecx
	push	ecx
	movzx	ecx, word ptr [esi._address]
	add	edx, ecx
	movzx	ecx, word ptr [esi._address+2]
	shl	ecx, 4
	add	edx, ecx
	xchg	edi, edx
	mov	[edi], al
	xchg	edi, edx

	pop	ecx
	cmp	ecx, [edi.paste_end]		; Q: paste buffer empty?
	jb	short intentry_done_ret
	and	[esi._status], 7C00h		; BUSY = 0,  ERROR = 0
	or	[esi._status], 0100h		; DONE = 1
	mov	eax, Paste_Complete
	call	VKD_End_Paste
	ret

not_cmd_4:
	cmp	al, 5
	jnz	short not_cmd_5

	xchg	ecx, edi
	mov	al, [edi.Paste_Char]
	mov	[esi._media], al
	xchg	ecx, edi
	jmp	short intentry_done_ret

not_cmd_5:
	cmp	al, 6
	jz	short intentry_done_ret

	cmp	al, 7
	jnz	not_paste_intentry
intentry_done_ret:
	and	[esi._status], 7C00h		; BUSY = 0,  ERROR = 0
	or	[esi._status], 0100h		; DONE = 1

simulate_intentry_ret:
	mov	esi, [edi.linear_intentry]	; Linear address of strategy
	mov	[esi], word ptr 6fcdh		; put code of INT 6Fh
	ret

EndProc vkd_hook_intentry

;******************************************************************************
;
;   vkd_hook_atok7
;
;   DESCRIPTION:    
;
;
;   ENTRY:	    EAX is Client CS:IP
;		    EBX is VM handle
;		    EDX is reference data
;		    ESI is linear address of break point(CS << 4+IP)
;   		    EDI is points to VKD data in VM's CB
;		    EBP is points to the client frame on the stack
;
;   EXIT:
;
;   USES:	EAX, ECX, EDX, ESI, flags
;
;==============================================================================
EndDoc
BeginProc vkd_hook_atok7

	mov	esi, [edi.linear_atok7]
	mov	ax, [edi.code_atok7]
	mov	[esi], ax
	inc	[edi.in_atok7]

	VMMcall	Begin_Nest_V86_Exec
	mov	cx, word ptr [edi.atok7_entry+2]
	movzx	edx, word ptr [edi.atok7_entry]
	VMMcall	Simulate_Far_Call
	VMMcall	Resume_Exec
	VMMcall	End_Nest_Exec

	dec	[edi.in_atok7]
	mov	[esi], word ptr 6fcdh		; put code of INT 6Fh
	ret

EndProc vkd_hook_atok7

;******************************************************************************
;
;   call_atok6
;
;   DESCRIPTION:
;
;   ENTRY:	    AH is command
;		    EBX is VM handle
;
;   EXIT:	    AL is return AL
;		    AH is return DL
;
;   USES:	    EAX, flags
;
;==============================================================================
BeginProc call_atok6

	pushad
	mov	edi, ebx
	add	edi, [VKD_CB_Offset]
	push	eax
	VMMcall Begin_Nest_V86_Exec
call6_loop1:					; wait to done control ATOK6
	movzx	eax, [edi._8042_out_head]
	cmp	al, [edi._8042_out_tail]    ;Q: buffer empty?
	jne	short call6_false
	test	[edi.kbdState], KBS_sim_hold or KBS_sim_alt_hld
	jz	short call6_done1
call6_false:
	VMMcall	Resume_Exec
	jmp	short call6_loop1
call6_done1:
	VMMcall End_Nest_Exec

	mov	[edi.atok_status], -1
	pop	edx
	mov	esi, offset32 vkd_call_atok6
	VMMcall	Call_VM_Event

	VMMcall Begin_Nest_V86_Exec
call6_loop:					; wait to done control ATOK6
	cmp	[edi.atok_status], -1
	jne	short call6_done
	VMMcall	Resume_Exec
	jmp	short call6_loop
call6_done:
	VMMcall End_Nest_Exec

	xor	eax, eax
	mov	ax, [edi.atok_status_ax]
	mov	[esp.Pushad_EAX], eax
	mov	ax, [edi.atok_status_dx]
	mov	[esp.Pushad_EDX], eax
	popad
	ret

EndProc call_atok6

BeginProc vkd_call_atok6

	mov	edi, ebx
	add	edi, [VKD_CB_Offset]
	Push_Client_State
	VMMcall Begin_Nest_V86_Exec			; Get ready for software ints
	mov	[ebp.Client_AX], dx		; Command
	mov	eax, 6fh			; ATOK6 Control
	push	edx
	VMMcall Exec_Int
	mov	ax, [ebp.Client_AX]		; Return AX
	pop	edx
	cmp	dh, 86h
	jnz	short @f
	test	al, 8
	jz	short @f
	xor	al, 2
@@:
	mov	dx, [ebp.Client_DX]		; Return DX
	mov	[edi.atok_status], 0
	mov	[edi.atok_status_ax], ax
	mov	[edi.atok_status_dx], dx
	VMMcall End_Nest_Exec
	Pop_Client_State
	ret

EndProc vkd_call_atok6

;******************************************************************************
;
;   call_atok7
;
;   DESCRIPTION:
;
;   ENTRY:	    AH is command
;		    EBX is VM handle
;
;   EXIT:	    AL is return AL
;		    AH is return DL
;
;   USES:	    EAX, flags
;
;==============================================================================
BeginProc call_atok7

	pushad
	mov	edi, ebx
	add	edi, [VKD_CB_Offset]
	push	eax
	VMMcall Begin_Nest_V86_Exec
call7_loop1:					; wait to done control ATOK7
	cmp	[edi.in_atok7], 0
	jnz	short call7_false
	movzx	eax, [edi._8042_out_head]
	cmp	al, [edi._8042_out_tail]    ;Q: buffer empty?
	jne	short call7_false
	test	[edi.kbdState], KBS_sim_hold or KBS_sim_alt_hld
	jz	short call7_done1
call7_false:
	VMMcall	Resume_Exec
	jmp	short call7_loop1
call7_done1:
	VMMcall End_Nest_Exec

	mov	[edi.atok_status], -1
	pop	edx
	mov	esi, offset32 vkd_call_atok7
	VMMcall	Call_VM_Event

	VMMcall Begin_Nest_V86_Exec
call7_loop:					; wait to done control ATOK7
	cmp	[edi.atok_status], -1
	jne	short call7_done
	VMMcall	Resume_Exec
	jmp	short call7_loop
call7_done:
	VMMcall End_Nest_Exec

	xor	eax, eax
	mov	ax, [edi.atok_status_ax]
	mov	[esp.Pushad_EAX], eax
	mov	ax, [edi.atok_status_dx]
	mov	[esp.Pushad_EDX], eax
	popad
	ret

EndProc call_atok7

BeginProc vkd_call_atok7

	push	fs
	push	edx

	mov	esi, offset32 atok7_buffer
	mov	ax, ds
	mov	fs, ax

	cmp	dh, 64h
	jnz	short vca7_not_64
	mov	word ptr [esi+140h], 8000h
	mov	word ptr [esi+142h], 0024h
	mov	word ptr [esi+144h], 0000h
	mov	dx, 0011h
	jmp	short vca7_not_86

vca7_not_64:
	cmp	dh, 6bh
	jnz	short vca7_not_6b
	mov	word ptr [esi+140h], 0ffffh
	mov	dx, 0005h
	jmp	short vca7_not_86

vca7_not_6b:
	cmp	dh, 86h
	jnz	short vca7_not_86
	mov	dx, 0841h

vca7_not_86:
	Push_Client_State
	mov	ecx, 512
	stc
	VxDcall V86MMGR_Allocate_Buffer

	mov	[ebp.Client_AX], dx		; Command
	mov	eax, edi
	add	ax, 102h
	mov	[ebp.Client_SP], ax
	add	ax, 140h-102h
	mov	[ebp.Client_BX], ax
	shr	eax, 16
	mov	[ebp.Client_SS], ax
	mov	[ebp.Client_ES], ax

	VMMcall Begin_Nest_V86_Exec
	push	ecx
	push	edi
	mov	edi, ebx
	add	edi, [VKD_CB_Offset]
	mov	cx, word ptr [edi.atok7_entry+2]
	movzx	edx, word ptr [edi.atok7_entry]
	VMMcall	Simulate_Far_Call
	VMMcall	Resume_Exec
	pop	edi
	pop	ecx
	VMMcall End_Nest_Exec
	stc
	VxDcall V86MMGR_Free_Buffer
	Pop_Client_State
	pop	edx

	cmp	dh, 64h
	jnz	short vca7_not_e64
	mov	dl, 36h			; = ROLL UP
	cmp	word ptr [esi+140h], 0161h
	jz	short @f
	mov	dl, 6bh			; = f.10
@@:
	jmp	short vca7_not_e86

vca7_not_e64:
	cmp	dh, 6bh
	jnz	short vca7_not_e6b
	mov	al, [esi+140h]
	jmp	short vca7_not_e86

vca7_not_e6b:
	cmp	dh, 86h
	jnz	short vca7_not_e86
	mov	ax, [esi+142h]
	push	ebx
	mov	ebx, offset32 xlat_atok7
	xlat
	pop	ebx

vca7_not_e86:
	mov	edi, ebx
	add	edi, [VKD_CB_Offset]
	mov	[edi.atok_status], 0
	mov	[edi.atok_status_ax], ax
	mov	[edi.atok_status_dx], dx
	pop	fs
	ret

EndProc vkd_call_atok7

;******************************************************************************
;
;   convert_code
;
;   DESCRIPTION:
;
;
;   ENTRY:	AX is SHIFT JIS code
;		ESI is points to VKD data in VM's CB
;
;   EXIT:	AX is SHIFT JIS, JIS or KUTEN code
;
;   USES:	EAX, EBX, flags
;
;==============================================================================
BeginProc convert_code, PUBLIC

	cmp	[esi.atok_code], 0
	jz	short cc_exit
;
; Shift JIS => JIS
;
	cmp	al,80h			;if (ah >= 0x80)
	cmc				;	then ah=ah-1 ;
	sbb	al,0			;
	sub	al,(40h-21h)		;ah=ah-(0x40-0x21) ;
	and	ah,10111111b		;al-=(al<0xe0) ? 0 : 0xe0-0xa0 ;
	shl	ah,1			;al=(al-0x81)*2 + 0x21 ;
	add	ah,21h-2h		;
	cmp	al,7eh			;if (ah <= 0x7e)
	jbe	short @f		;	then ;
	sub	al,(7fh-21h)		;	else { ah=ah-(0x7f-0x21) ;
	inc	ah			;	       al++ ; }
@@:
	cmp	[esi.atok_code], 1
	jz	short cc_exit
;
; JIS => Kuten
;
	sub	ax, 2020h		;if ah = 12d, al = 34d
	mov	bl, ah			;  bl = 12d
	aam				;	ax = 0304h
	xchg	ax, bx			;	bx = 0304h
					;  al = 12d
	aam				;	ax = 0102h
	xchg	al, bh			;	ax = 0103h, bx = 0204h
	shl	ax, 4			;		    ax = 1030h
	or	ax, bx			;	ax = 1234h
cc_exit:
	ret

EndProc convert_code

;******************************************************************************
;
;   change_atok_mode
;
;   DESCRIPTION:
;
;
;   ENTRY:	AL is number of mode
;		ESI is points to VKD data in VM's CB
;
;   EXIT:
;
;   USES:	EAX, ECX, flags
;
;==============================================================================
BeginProc change_mode_atok, PUBLIC

	test	[esi.vkd_paste_mode], PASTE_MODE_ATOK6 or PASTE_MODE_ATOK7
	jz	short cma_exit

	movzx	ecx, al
	sub	cl, [esi.atok_mode]
	jz	short cma_exit
	mov	[esi.atok_mode], al
	jnc	short cma_loop
	add	cl, [esi.atok_num_mode]		;
cma_loop:
	Queue_Stroke [esi.atok_key]	; f10 or ROLL UP
	loop	short cma_loop
cma_exit:
	ret

EndProc change_mode_atok


;******************************************************************************
;
;   VKD_Get_BIOS_Key_State
;
;   DESCRIPTION:    Look at BIOS to determine current key state
;
;   ENTRY:	    EBX = VM Handle
;
;   EXIT:	    None
;
;   USES:	    Flags
;
;==============================================================================

BeginProc VKD_Get_BIOS_Key_State, PUBLIC

	push	esi
	push	edi
	mov	esi, BIOS_Key_State_Loc
	add	esi, [ebx.CB_High_Linear]
	lea	edi, [ebx.loc_bios_key_state]
	add	edi, [VKD_CB_Offset]
	cld
	movsd
	movsd
	movsd
	movsd
	pop	edi
	pop	esi
	ret

EndProc VKD_Get_BIOS_Key_State


;******************************************************************************
;
;   VKD_Clear_BIOS_Key_State
;
;   DESCRIPTION:    Clear BIOS to determine current key state
;
;   ENTRY:	    ESI = VKD data in CB
;
;   EXIT:	    None
;
;   USES:	    Flags
;
;==============================================================================

BeginProc VKD_Clear_BIOS_Key_State, PUBLIC

	push	eax
	push	edi
	lea	edi, [esi.loc_bios_key_state]
	xor	eax, eax
	cld
	stosd
	stosd
	stosd
	stosd
	pop	edi
	pop	eax
	ret

EndProc VKD_Clear_BIOS_Key_State

VxD_CODE_ENDS
