PAGE 80, 132
;***********************************************************************
TITLE MTDHDR.ASM - MTD VxD Driver
;***********************************************************************
;
;   MTDHDR.ASM - MTD VxD Driver
;
;   (C) Copyright MICROSOFT Corp., 1993-1998
;
;-----------------------------------------------------------------------

	.386

;***********************************************************************
;				I N C L U D E S
;***********************************************************************

.xlist
NEWSTRUCTS equ TRUE

include vmm.inc
include debug.inc
include configmg.inc
include pccard.inc
include mtdcomon.inc
include mtdlocal.inc
.list


VxD_LOCKED_DATA_SEG

GCSI	GetCardServicesInfo_s	<>
RC	RegisterClient_s	<>
RMTD	RegisterMTD_s		<>
GtS	GetStatus_s		<>
RWDW	RequestWindow_s 	<>
IFDEF NEC_98
NECSD	NECSocketData		<>
ENDIF
VxD_LOCKED_DATA_ENDS

VxD_LOCKED_CODE_SEG

;***********************************************************************
;	EXTERNALS
;***********************************************************************

extrn	MTD_RequestHandler: near



;***********************************************************************
;
;	MTD_NewDevNode
;
;	ENTRY:	EBX = DevNode
;		EDX = Load Type
;
;	EXIT:	Carry clear to indicate success
;
;	USES:	All registers and flags
;
;=======================================================================

BeginProc MTD_NewDevNode

LocalVar dnDevNode, dword
LocalVar pSocketData, dword

	EnterProc

	mov	dnDevNode, ebx

	cmp	edx, DLVXD_LOAD_DRIVER	; Is this the driver?
	jne	ndn_error		;   No, return an error

	; Get MTD Init Data from private dword in devnode.

	lea	eax, pSocketData
	VxDCall _CONFIGMG_Get_Private_DWord, <eax, ebx, ebx, 0>
	mov	edi, pSocketData	; edi -> MTDInit_s
	or	edi, edi
	jz	ndn_error

	; Allocate socket data structure for this node.

	VMMcall _HeapAllocate, <SIZE SocketData_s, HeapZeroInit>
	or	eax, eax
	jz	ndn_error

	mov	pSocketData, eax

	; Save pointer to socket data structure in private dword for devnode.

	VxDCall _CONFIGMG_Set_Private_DWord, <ebx, ebx, eax, 0>

	mov	ebx, pSocketData	; ebx -> socket data structure

	ASSUME EBX:PTR SocketData_s
	ASSUME EDI:PTR MTDInit_s

	mov	[ebx].skt_cur_map, 0ffffffffH
	mov	ax, [edi].Socket
	mov	[ebx].skt_socket, ax
	mov	eax, [edi].CardOffset
	mov	[ebx].skt_cardoff, eax
	mov	eax, dnDevNode
	mov	[ebx].skt_devnode, eax
;;	  mov	  al, [edi].CurVpp
;;	  mov	  [ebx].skt_CurVpp, al
;;	  mov	  al, [edi].LowVpp
;;	  mov	  [ebx].skt_LowVpp, al

	ASSUME EDI:NOTHING

	; Request Card Services Info and verify Card Services is present.

	mov     GCSI.Signature,0
	mov     ebx, OFFSET32 GCSI
	CS_Call F_GET_CARD_SERVICES_INFO, <size GetCardServicesInfo_s>
	cmp     word ptr GCSI.Signature, 'SC'
	jne	ndn_cleanup		; jump if Card Services not loaded

	; Check release level of this Card Services.

	cmp     word ptr GCSI.CSLevel,PCMCIA_201
	je      short @f        ; jump if Card Services is 2.01 and continue
	cmp     word ptr GCSI.CSLevel,PCMCIA_210
	jne	ndn_cleanup
@@:
	; Check the status of the write protect switch on our card.
	; In this version, we do not support a write protect change while the
	; card is inserted.

	mov	ebx,pSocketData
	mov	ax, [ebx].skt_socket
	mov     word ptr GtS.Socket, ax
	mov     ebx, OFFSET32 GtS
	CS_Call F_CS_GET_STATUS, <size GetStatus_s>
	jc	ndn_cleanup
	mov     ax, word ptr GtS.CardState
	mov	ebx,pSocketData
	mov	[ebx].skt_status, ax

	; Register as a Card Services client.

	mov     word ptr  RC.Attributes, CLTA_MTD_DRIVER OR CLTA_INSERT_SHARED
	mov     word ptr  RC.EventMask, EVTM_CARD_DETECT_CHG
	mov     word ptr  RC.Version, PCMCIA_201
	lea	esi, RC.ClientData

	ASSUME ESI:PTR ClientData_s

	mov	[esi].ReferenceData, 0
	mov	[esi].ReservedWord, 0
	mov	[esi].DataOffset, ebx

	ASSUME ESI:NOTHING

	mov     esi, OFFSET32 CBHandler
	mov     ebx, OFFSET32 RC
	CS_Call F_REGISTER_CLIENT, <size RegisterClient_s>
	jc	ndn_cleanup

	mov	ebx,pSocketData
	mov	[ebx].skt_client_h, dx
	or	[ebx].skt_flags, SDF_REG_IN_PROG

	; Request a window from card services.	Ask for a base and size of 0,
	; they will be filled in by card services.

	mov	ax, [ebx].skt_socket
IFDEF NEC_98
	movzx	edi,ax
	add	edi,edi
	cmp	word ptr NECSD.skt_flag[edi], 0
	je	go_req_window
	mov	ebx,pSocketData
	mov	ax, word ptr NECSD.win_handle[edi]
	mov	[ebx].skt_win_handle, ax
	add	edi,edi
	mov	eax, dword ptr NECSD.win_base[edi]
	mov	[ebx].skt_win_base, eax
	mov	edx, dword ptr NECSD.win_size[edi]
	mov	[ebx].skt_win_size, edx
	jmp	setup_refdata
go_req_window:
ENDIF
	mov     word ptr RWDW.Socket, ax
	mov     dword ptr RWDW.MemSize, 0
	mov     dword ptr RWDW.Base, 0
	mov     word ptr RWDW.Attributes, WDWA_16BIT or WDWA_ENABLED
	mov     RWDW.AccessSpeed, 80h
	mov	dx, [ebx].skt_client_h
	mov     ebx, OFFSET32 RWDW
	CS_Call F_REQUEST_WINDOW, <size RequestWindow_s>
	jc	ndn_cleanup

	mov	ebx,pSocketData
	mov	[ebx].skt_win_handle, dx
IFDEF NEC_98
	mov	word ptr NECSD.skt_flag[edi], 1
	mov	word ptr NECSD.win_handle[edi], dx
ENDIF

	VmmCall _MapPhysToLinear, <dword ptr RWDW.Base, dword ptr RWDW.MemSize, 0>
	cmp     eax, 0FFFFFFFFh
	je	ndn_cleanup

	mov	[ebx].skt_win_base, eax
	mov	edx, dword ptr RWDW.MemSize
	mov	[ebx].skt_win_size, edx
IFDEF NEC_98
	add	edi,edi
	mov	dword ptr NECSD.win_base[edi], eax
	mov	dword ptr NECSD.win_size[edi], edx
setup_refdata:
ENDIF
	add	eax, edx
	mov	[ebx].skt_win_end, eax
	dec	edx
	mov	[ebx].skt_win_omask, edx
	not	edx
	mov	[ebx].skt_win_mmask, edx

	; Register with Card Services as an MTD for this card

	mov	ax, [ebx].skt_socket
	mov     word ptr  RMTD.Socket, ax
	mov     word ptr  RMTD.Attributes, 1000h
	mov	eax, [ebx].skt_cardoff
	mov	dword ptr RMTD.CardOffset, eax
	mov	word ptr  RMTD.MTDMediaID, 0
	mov	dx, [ebx].skt_client_h
	mov     ebx, OFFSET32 RMTD
	CS_Call F_REGISTER_MTD, <size RegisterMTD_s>
	jnc	ndn_exit

ndn_cleanup:
	mov	esi,pSocketData
	call	MTD_Exit

ndn_error:
	mov	eax, CR_FAILURE
	stc

ndn_exit:
	LeaveProc
	Return

	ASSUME EBX:NOTHING

EndProc MTD_NewDevNode


;***********************************************************************
;
;	MTD_Get_Version
;
;	ENTRY:
;
;	EXIT:	AH = Major version number
;		AL = Minor version number
;
;	USES:	EAX, Flags
;
;=======================================================================

BeginProc MTD_Get_Version, Service

	mov	eax, 100h
	clc
	ret

EndProc MTD_Get_Version


;***********************************************************************
;
;	MTD_Control
;
;	This is the main control procedure for the MTD device.
;
;	ENTRY:	EAX = System control message
;		Other registers may contain parameters.  See DDK for details of
;		each system control call.
;
;	EXIT:	Standard system control exit (usually, carry set indicates
;		error)
;
;	USES:	See individual procedures for details.
;
;=======================================================================

BeginProc MTD_Control

	Control_Dispatch PNP_NEW_DEVNODE, MTD_NewDevNode
	clc
	ret

EndProc MTD_Control


;***********************************************************************
;
;	MTD_RegComplete
;
;	ENTRY:	ESI = pointer to socket data structure
;
;	EXIT:	Carry clear to indicate success
;
;	USES:	All registers and flags
;
;=======================================================================

BeginProc MTD_RegComplete

	mov	ebx, esi		; ebx -> socket data structure

	ASSUME EBX:PTR SocketData_s

	and	[ebx].skt_flags, NOT SDF_REG_IN_PROG

	test	[ebx].skt_flags, SDF_DO_EXIT
	jnz	MTD_Exit		; go handle exit now

	ASSUME EBX:NOTHING

	clc				; No error, successful exit
	ret

EndProc MTD_RegComplete


;***********************************************************************
;
;	MTD_Exit
;
;	ENTRY:	ESI = pointer to socket data structure
;
;	EXIT:	Carry clear to indicate success
;
;	USES:	All registers and flags
;
;=======================================================================

BeginProc MTD_Exit

	mov	ebx, esi		; ebx -> socket data structure

	ASSUME EBX:PTR SocketData_s

	or	[ebx].skt_flags, SDF_DO_EXIT

	test	[ebx].skt_flags, SDF_REG_IN_PROG
	stc
	jnz	me_error

IFDEF NEC_98
	mov	ax, [ebx].skt_socket
	movzx	edi,ax
	add	edi,edi
	cmp	word ptr NECSD.skt_flag[edi], 0
	je	me_nowindow
	mov	word ptr NECSD.skt_flag[edi], 0
ENDIF
	movzx	edx, [ebx].skt_win_handle
	or	edx, edx
	jz	me_nowindow
	push	ebx
	CS_Call F_RELEASE_WINDOW, 0
	pop	ebx
	jc	me_error

me_nowindow:
	movzx	edx, [ebx].skt_client_h
	or	edx, edx
	jz	me_noclient
	push	ebx
	CS_Call F_DEREGISTER_CLIENT, 0
	pop	ebx
	jc	me_error

me_noclient:
	; Free socket data structure for this node.

	VMMcall _HeapFree, <ebx, 0>

	ASSUME EBX:NOTHING

	mov	ax, R_SUCCESS		; return SUCCESS
	clc				; No error, successful exit
me_error:
	ret

EndProc MTD_Exit


;***********************************************************************
;
;	CBHandler
;
;	Handler for all Card Services events, including MTD requests.
;
;	ENTRY:	AL  = function
;		CX  = socket
;		DX  = info
;		DI  = client data[0..1]
;		ESI = client data[4..7]
;		EBP = MTD request
;		EBX = buffer
;		BX  = misc (if no buffer)
;
;	EXIT:	AX  = status
;		carry set if error
;
;=======================================================================

BeginProc CBHandler

	ASSUME ESI:PTR SocketData_s

	cmp	al, EV_REGISTRATION_COMPLETE
	je	MTD_RegComplete 	; RETURNS DIRECTLY TO CARD SVCS

	cmp	cx, [esi].skt_socket	; Is the request for this socket?
	jne	CBH_exit		;   No, just exit

	; Handle an MTD I/O request.

	cmp	al, EV_MTD_REQUEST
	je	MTD_RequestHandler	; RETURNS DIRECTLY TO CARD SVCS

	; Check for card removal and free up socket data structure.

	cmp	al, EV_CARD_REMOVAL
	je	MTD_Exit		; RETURNS DIRECTLY TO CARD SVCS

	; Unknown or uninteresting CS Event.  The Spec requires that we not care.

CBH_exit:
	ASSUME ESI:NOTHING

	mov	ax, R_SUCCESS		; return SUCCESS
	clc
	ret

EndProc CBHandler


VxD_LOCKED_CODE_ENDS


	END
