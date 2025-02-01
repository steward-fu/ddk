PAGE 80, 132
;***********************************************************************
TITLE MTDIO.ASM - MTD VxD Driver
;***********************************************************************
;
;   MTDIO.ASM - Handle all MTD service callback events/requests for
;               SRAM cards.
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


;***********************************************************************
;	EXTERNALS
;***********************************************************************

extrn	SetVpp: near
extrn	SetWindow: near

VxD_LOCKED_DATA_SEG

;***********************************************************************
;------------ dword boundary -------------

	.errnz	(CPY_BUF_LEN AND 0003H)
ipub	copy_buf, db, <CPY_BUF_LEN dup(?)>


;***********************************************************************

if WANT_CALL_COUNTS
	align	4
	db      "CALL COUNTERS(RWEC):"
	public read_count, write_count, erase_count, copy_count, last_call
read_count	dd	0
write_count	dd	0
erase_count	dd	0
copy_count	dd	0
last_call	dd	0

endif ; WANT_CALL_COUNTS

VxD_LOCKED_DATA_ENDS

VxD_LOCKED_CODE_SEG

;***********************************************************************
;
;	SetBusy
;
;	Enter a background erase in the queue, or reject it if
;	there are no free table entries.
;
;	ENTRY:	ebp -> MTD request packet
;               edx = starting card offset
;
;	EXIT:	All registers preserved.
;		CY Set => All table entries are already in use.
;		CY Clear => Request has been recorded in the erase table.
;
;   NOTE: ASSUMES
;	(1) THAT CHECKBUSY HAS ALREADY BEEN CALLED, AND RETURNED CY=0
;	(2) THAT TOTAL # OF ERASE BLOCKS WILL NOT EXCEED 32K
;
;***********************************************************************

	INTERNAL SetBusy
SetBusy proc near
	clc
	ret
SetBusy endp


;***********************************************************************
;
;	CheckBusy
;
;	Check a requested card offset against background erases
;	in progress.
;
;	ENTRY:	ebp -> MTD request packet
;               edx = starting card offset
;
;	EXIT:	All registers preserved.
;		CY Set => request conflicts with an erase in progress
;		CY Clear => request can proceed immediately
;
;   NOTE: ASSUMES
;	(1) THAT TRANSFERS WILL NEVER STRADDLE AN ERASE BLOCK BOUNDARY,
;	(2) THAT TOTAL # OF ERASE BLOCKS WILL NOT EXCEED 32K
;
;***********************************************************************

	INTERNAL CheckBusy
CheckBusy proc near
	clc
	ret
CheckBusy endp


;***********************************************************************
;
;	CheckDone
;
;	Check a background erase in progress for completion
;
;	ENTRY:	ebp -> MTD request packet
;               edx = starting card offset
;
;	EXIT:	AX = chip status error bits if CY clear
;		CY Set => still in progress, AX trash
;		CY Clear => finished, AX is final status
;		All other registers preserved.
;
;   NOTE: ASSUMES
;	(1) THAT TRANSFERS WILL NEVER STRADDLE AN ERASE BLOCK BOUNDARY,
;	(2) THAT TOTAL # OF ERASE BLOCKS WILL NOT EXCEED 32K
;
;***********************************************************************

	INTERNAL CheckDone
CheckDone proc near
	sub     ax,ax
	ret
CheckDone endp


;***********************************************************************
;
;	Erase
;
;	Provides erase operation for SRAM Card.
;
;	ENTRY:	ebp -> MTD request packet
;		ebx -> socket data structure
;
;	EXIT:	AX, BX indicate result
;	    Caller will postprocess as follows:
;		AX will be returned to CS
;		BX will be returned to CS in MTD_Status
;		CY will be set if AX != 0
;
;***********************************************************************

	INTERNAL Erase
Erase proc near
	ASSUME EBP:PTR MTDRequest_packet
	ASSUME EBX:PTR SocketData_s

    if WANT_CALL_COUNTS
	inc	erase_count
	mov     last_call, OFFSET32 Erase
    endif ; WANT_CALL_COUNTS

	ASSUME EBX:NOTHING

	mov	ax, R_SUCCESS		; it is done!
	mov	bx, ax
	ret

	ASSUME EBP:NOTHING
Erase endp


;***********************************************************************
;
;	Read
;
;	Provides read operation for SRAM Card.
;
;	ENTRY:	ebp -> MTD request packet
;		ebx -> socket data structure
;
;	EXIT:	AX, BX indicate result
;	    Caller will postprocess as follows:
;		AX will be returned to CS
;		BX will be returned to CS in MTD_Status
;		CY will be set if AX != 0
;
;	Note:	Must preserve [ebp].SrcOffset (Copy relies on this).
;
;***********************************************************************

	INTERNAL Read
Read proc near
	ASSUME EBP:PTR MTDRequest_packet
	ASSUME EBX:PTR SocketData_s

    if WANT_CALL_COUNTS
	inc	read_count
	mov     last_call, OFFSET32 Read
    endif ; WANT_CALL_COUNTS

	cld				; just in case...

	push    ecx
	push    edx
	push    esi
	push    edi

; Set the memory window.

	mov     esi,[ebp].SrcOffset
	call    SetWindow
	jnc     short @f
	jmp     read_failed
@@:
	and	esi, [ebx].skt_win_omask
	add	esi, [ebx].skt_win_base

;***********************************************************************
; In the following, EDX will hold the total number of bytes remaining
; to be moved, and ECX will get the number to move this time around.
;***********************************************************************
just_go_read:
	mov	edi, [ebp].MTD_buffer
	mov	ecx, [ebp].XferLength	; bytes to move
	or	ecx, ecx
	jnz     short @f
	jmp     short read_done
@@:
; If source is odd, move a single byte. 
	test	esi, 1
	jz      short @f
	movsb
	dec     ecx
	jz      short read_done
@@:
	mov	[ebp].MTD_odd_byte, cl	; bit 0 is odd byte flag
	and	ecx, 0fffffffeH
	mov	edx, ecx		; EDX tracks bytes remaining

read_main_loop:
	mov	ecx, edx

; did we cross end of window?
	cmp	esi, [ebx].skt_win_end
	jb	read_compute_fit

; Remap window to next WINSIZE offset and move some more data.
	mov	esi, [ebp].SrcOffset
	add	esi, [ebp].XferLength
	sub	esi, edx
	call    SetWindow
	jc      short read_failed
	mov	esi, [ebx].skt_win_base ; resume at base of window

read_compute_fit:
;***********************************************************************
; Compute the remaining space in the current window, from the
; offset in esi (EAX = WINEND - ESI).
; Then, compute the number of bytes to move as the smaller of
;   (1) the number of bytes currently visible, or
;   (2) the remaining transfer length
;***********************************************************************
	mov	eax, [ebx].skt_win_end
	sub	eax, esi		; EAX: size of target in current window
	cmp	eax, ecx		; CX: total number of bytes
	jae	short @f
	mov	ecx, eax
@@:
	sub	edx, ecx		; update remaining bytes to move
	shr	ecx, 1
	jecxz   read_no_more_words
	rep  movsw

	or	edx, edx
	jnz     read_main_loop

;**********************************************************************
; No more full words to do.  But if there is one more byte to copy,
; we go back through one more time to get the window set right.  Since
; DX==0, the top of the loop will exit to the label "read_no_more_words"
; (below) after it has adjusted the window mapping if necessary.
;**********************************************************************

	test	[ebp].MTD_odd_byte, 1
	jnz     read_main_loop
	jmp     short read_done

read_no_more_words:
; One more byte remains, copy it now
	movsb                           ; move 1 byte
	jmp     short read_done

	ASSUME EBX:NOTHING
	ASSUME EBP:NOTHING

read_failed:
	mov	ebx, eax
	jmp     short @f

read_done:
	mov	bx, R_SUCCESS
@@:
	mov	ax, R_SUCCESS		; So to speak...  Really means "Done"

read_busy:
	pop	edi			; Restore regs and exit.
	pop	esi
	pop     edx
	pop     ecx
	ret
Read endp


;***********************************************************************
;
;	Write
;
;	Provides write operation for SRAM Card.
;
;	ENTRY:	ebp -> MTD request packet
;		ebx -> socket data structure
;
;	EXIT:	AX, BX indicate result
;	    Caller will postprocess as follows:
;		AX will be returned to CS
;		BX will be returned to CS in MTD_Status
;		CY will be set if AX != 0
;
;	Note:	Must preserve [ebp].DestOffset (Copy relies on this).
;
;***********************************************************************

	INTERNAL Write
Write proc near
	ASSUME EBP:PTR MTDRequest_packet
	ASSUME EBX:PTR SocketData_s

    if WANT_CALL_COUNTS
	inc	write_count
	mov     last_call, OFFSET32 Write
    endif ; WANT_CALL_COUNTS

	cld				; just in case...

	push	ecx
	push	edx
	push	esi
	push	edi

; Bail out with error if write protected
	test	[ebx].skt_status, CRDS_WRITE_PROTECTED
	jz	short @f
	mov	ax, STATUS_WRITE_PROTECTED
	jmp	write_error
@@:
; Set the memory window
	mov	esi, [ebp].DestOffset
	call	SetWindow
	jnc	short @f
	jmp	write_error
@@:
; edi -> destination in window
	mov	edi, esi
	and	edi, [ebx].skt_win_omask
	add	edi, [ebx].skt_win_base

; esi -> source buffer address
	mov	esi, [ebp].MTD_buffer

; ecx: #bytes to move this loop
	mov	ecx, [ebp].XferLength	; bytes to write
	or	ecx, ecx
	jnz	short @f
	jmp	write_done
@@:
; If starting destination is odd, move a single byte.
	test	edi, 1
	jz	short wr_check_last_odd

	movsb				   ; write the odd first byte
	dec	ecx
	jnz	short @f
	jmp	short write_done
@@:
wr_check_last_odd:
	mov	[ebp].MTD_odd_byte, cl	; bit 0 is odd byte flag
	and	ecx, 0fffffffeH
	mov	edx, ecx		; track bytes remaining

write_main_loop:
	mov	ecx, edx		; cx gets total remaining

; did we cross end of window?
	cmp	edi, [ebx].skt_win_end
	jb	short write_compute_fit

; Remap window to next WINSIZE offset and move some more data.
	mov	edi, esi		; save ESI
	mov	esi, [ebp].DestOffset
	add	esi, [ebp].XferLength
	sub	esi, edx
	call	SetWindow
	jc	short write_error
	mov	esi, edi		; restore ESI
	mov	edi, [ebx].skt_win_base ; resume at base of window

write_compute_fit:
;***********************************************************************
; Compute the remaining space in the current window, from the
; offset in di (EAX = WINEND - EDI).
; Then, compute the number of bytes to move as the smaller of
;   (1) the number of bytes currently visible, or
;   (2) the remaining transfer length
;***********************************************************************
	mov	eax, [ebx].skt_win_end
	sub	eax, edi		; AX: size of target in current window
	cmp	eax, ecx		; CX: total number of bytes
	jae	short @f
	mov	ecx, eax
@@:
	sub	edx, ecx		; update remaining bytes to move

; Compute the number of words to write (could be zero).
	shr	ecx, 1			; # words = 1/2 (# bytes)
	jecxz	write_no_more_words

; Write number of words indicated by CX.
write_words:
	rep movsw

	or	edx, edx		; finished with this chunk
	jnz	write_main_loop 	; go do more words

;**********************************************************************
; No more full words to do.  But if there is one more byte to copy,
; we go back through one more time to get the window set right.  Since
; DX==0, the top of the loop will exit to the label "write_no_more_words"
; (below) after it has adjusted the window mapping if necessary.
;**********************************************************************

	test	[ebp].MTD_odd_byte, 1	; words done, check odd byte
	jnz	write_main_loop 	; go make sure window is right
	jmp	short write_done

write_no_more_words:
; One odd byte remains at the end of the transfer -- write it.
	movsb

write_done:
	mov	ax, R_SUCCESS
write_error:

	ASSUME EBX:NOTHING

	mov	bx, ax
	mov	ax, R_SUCCESS
write_busy:
	pop	edi			; Restore regs and exit.
	pop	esi
	pop	edx
	pop	ecx
	ret

	ASSUME EBP:NOTHING
Write endp


;***********************************************************************
;
;	Copy
;
;	Provides copy operation for SRAM Card.
;
;	ENTRY:	ebp -> MTD request packet
;		ebx -> socket data structure
;
;	EXIT:	AX, BX indicate result
;	    Caller will postprocess as follows:
;		AX will be returned to CS
;		BX will be returned to CS in MTD_Status
;		CY will be set if AX != 0
;
;	NOTE:  Since the MTD Spec does not say otherwise, we must assume that
;	we are not allowed to change any of the fields in the MTD request
;	packet other than the status return fields.  Therefore, we save and
;	restore the ones that we use, even though it seems that their final
;	values can be of no interest to Card Services after our return...
;
;***********************************************************************

	INTERNAL Copy
Copy proc near
	ASSUME EBP:PTR MTDRequest_packet
	ASSUME EBX:PTR SocketData_s

    if WANT_CALL_COUNTS
	inc	copy_count
	mov     last_call, OFFSET32 Copy
    endif ; WANT_CALL_COUNTS

	push	edx

; First, make sure this request does not conflict with a background erase

	mov	edx, [ebp].SrcOffset
	call	CheckBusy
	jc	short @f		; jump if source conflicts
	mov	edx, [ebp].DestOffset
	call	CheckBusy
	jnc	short copy_no_conflict	; fall thru if dest conflicts
@@:
	mov	ax, R_BUSY
	mov	bx, MTD_WaitReq
	pop	edx
	ret

copy_no_conflict:
	pop	edx
	push	[ebp].SrcOffset 	; save source and dest
	push	[ebp].DestOffset	;  offsets, restored on exit
	mov	edx, [ebp].XferLength
	mov	esi, OFFSET32 copy_buf

copy_again:
	mov	ecx, edx
	jecxz	copy_done

;  First, make our intermediate buffer have the same alignment as the
;  source card address.

	mov	edi, [ebp].SrcOffset
	and	edi, 1
	cmp	ecx, CPY_BUF_LEN
	jb	short @f

	mov	ecx, CPY_BUF_LEN
	sub	ecx, edi		; only need to adjust CX if full size
@@:
	add	edi, esi		; adjust buffer base
	mov	[ebp].XferLength, ecx
	mov	[ebp].MTD_buffer, edi

	push	ebx
	call	read			; read preserves CX, DX, SI, DI
	or	ax, ax
	jnz	short copy_failed
	pop	ebx

	add	[ebp].SrcOffset, ecx
	mov	[ebp].XferLength, ecx
	mov	[ebp].MTD_buffer, edi	; use adjusted base

	push	ebx
	call	write			; write preserves CX, DX, SI, DI
	or	ax, ax
	jnz	short copy_failed
	pop	ebx

	add	[ebp].DestOffset, ecx
	sub	edx, ecx
	jmp	copy_again

copy_failed:
	add	esp, 4			; discard saved ebx
	jmp	short @f

	ASSUME EBX:NOTHING

copy_done:
	mov	ax, R_SUCCESS
	mov	bx, ax
@@:
	pop	[ebp].DestOffset
	pop	[ebp].SrcOffset
	ret

	ASSUME EBP:NOTHING
Copy endp


;***********************************************************************
;
;	MTD_RequestHandler
;
;	Handle MTD I/O requests from Card Services.
;	Jumped to from CBHandler.
;
;	ENTRY:	AL  = function (MTD_Request_Event)
;		CX  = socket (same as [ebp+2])
;		DI  = client data[0..1]
;		ESI = client data[4..7]
;		EBP = MTD request packet
;		EBX = buffer, (read, write)
;		EBX = undefined (erase, copy)
;
;	EXIT:	AX    = status
;		[EBP].MTD_status has additional return info
;               carry set if AX != R_SUCCESS
;
;		===> RETURNS DIRECTLY TO CARD SERVICES <===		
;
;***********************************************************************

ActionTable	label	DWORD
		dd      OFFSET32 Erase
		dd      OFFSET32 Read
		dd      OFFSET32 Write
		dd      OFFSET32 Copy

	public MTD_RequestHandler
MTD_RequestHandler proc
	ASSUME EBP:PTR MTDRequest_packet

; Save borrowed fields of request packet

	push	[ebp].XferLength

	push	dword ptr [ebp].Function
	push	[ebp].MTD_buffer

; Save BX, and initialize pointer to buffer
	push	ebx
	mov	[ebp].MTD_buffer, ebx

	mov	ebx, esi		; ebx -> socket data structure

	ASSUME EBX:PTR SocketData_s

	mov	[ebp].Socket, cx

; Call appropriate action routine, indexed by function code.
	xchg	eax, esi
	mov	al, [ebp].Function
	xchg	eax, esi
	and	esi, 3			; isolate function
	shl	esi, 2			; convert to DWORD offset
	call	ActionTable[esi]

	ASSUME EBX:NOTHING

; Return both return codes where they belong...

	mov	[ebp].MTDStatus, bx
	pop	ebx

; Restore borrowed fields of request packet

	pop	[ebp].MTD_buffer
	pop	dword ptr [ebp].Function
	pop	[ebp].XferLength

	or	ax, ax
	jnz	short @f		; (no jump in the frequent case)
	ret
@@:
	stc
	ret

	ASSUME EBP:NOTHING
MTD_RequestHandler endp

VxD_LOCKED_CODE_ENDS
	END
