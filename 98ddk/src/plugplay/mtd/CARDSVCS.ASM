PAGE 80, 132
;***********************************************************************
TITLE CARDSVCS.ASM - MTD VxD Driver
;***********************************************************************
;
;   CARDSVCS.ASM -  low level interfaces to Card Services calls
;
;   (C) Copyright MICROSOFT Corp., 1993-1998
;
;-----------------------------------------------------------------------

	.386

.xlist
include vmm.inc
include debug.inc
include configmg.inc
include pccard.inc
include mtdcomon.inc
.list

VxD_LOCKED_CODE_SEG


;***********************************************************************
;
;	SetWindow
;
;	Set memory-mapped window via the Card Services function
;	"MTDModifyWindow".
;
;	ENTRY:	esi =  32-bit card address
;		ebx -> socket data structure
;		ebp -> MTD request
;	EXIT:	if address was successfully mapped
;		    Carry clear
;		    [ebx].skt_window_base set
;		if error
;		    Carry set
;
;***********************************************************************

	public SetWindow
SetWindow proc near
	ASSUME EBX:PTR SocketData_s
	ASSUME EBP:PTR MTDRequest_packet

	push	esi

; align offset to window sized boundary

	and	esi, [ebx].skt_win_mmask

; Compare against the last offset we actually set the window to, and
; only make the call to Card Services if this time is different

IFNDEF NEC_98
	cmp	esi, [ebx].skt_cur_map
	clc				; indicate success
	je	short SW_exit
ENDIF

	push	eax
	push	ebx
	push	edx
	push	edi

; Update the slot structure with the new card offset

	mov	[ebx].skt_cur_map, esi
	mov	edi, esi		; Who knows which to use???

; Call MTD Helper to remap the window

	mov	ax, MTDModifyWindow
	mov	dx, [ebx].skt_win_handle
	mov	bh,0			; common memory
	mov	bl,[ebp].AccessSpeed
	call	[ebp].HelperPtr

	pop	edi
	pop	edx
	pop	ebx
	pop	eax

	jnc	SW_exit

; Invalidate the current map, and exit with Carry set to indicate failure

	mov	[ebx].skt_cur_map, 0ffffffffH
	stc

SW_exit:
	pop	esi
	ret

	ASSUME EBP:NOTHING
	ASSUME EBX:NOTHING
SetWindow endp


;***********************************************************************
;
;	SetVpp
;
;	Set Vpp voltage via the MTD Helper service "MTDSetVpp"
;	but only if this is a change from the present value.
;
;	ENTRY:	AX = desired Vpp action:
;		  AL is one of SET_VPP_LOW or SET_VPP_PROG
;		  AH is one of SET_VPP_NOW or SET_VPP_LATER
;		ebx -> socket data structure
;		ebp -> MTD request
;
;	EXIT:	CY <==> failure
;		AX trashed
;
;***********************************************************************

	public SetVpp
SetVpp proc near
	ASSUME EBX:PTR SocketData_s
	ASSUME EBP:PTR MTDRequest_packet

	cmp	al,SET_VPP_LOW
	mov	al,Vpp_12V		; Assume turning on
	jne	short @f
	mov	al,[ebx].skt_LowVpp	; turning off
@@:
;***********************************************************************
;  HZM:  For now, anytime this gets called we force the requested input
; value to 12V.  Soon (I hope) we will put in a time delay before turning
; Vpp off, so that we do not have to wait 100ms before each write!
;***********************************************************************

	mov	al,Vpp_12V		; TURN IT ON!

;***********************************************************************

; First, check if Vpp is already at the desired setting:

	cmp	al,[ebx].skt_CurVpp
	jne	short @f
	ret				; CY is cleared if equal
@@:
; Update the slot table entry with this new setting.

	push	edx
	push	ebx
	mov	[ebx].skt_CurVpp, al
	mov	dx,[ebp].Socket
	mov	bl, al
	mov	bh, bl
	mov	ax, MTDSetVpp
	call	[ebp].HelperPtr
	pop	ebx
	pop	edx
	ret

	ASSUME EBP:NOTHING
	ASSUME EBX:NOTHING
SetVpp endp

VxD_LOCKED_CODE_ENDS
	END
