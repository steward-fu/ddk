	PAGE	58,132
;******************************************************************************
; SERFUNC.ASM	- Port driver for 8250/16550AFN.
;******************************************************************************
;
; (C) Copyright Microsoft Corp. 1992-1995
;
; TITLE:	SERFUNC.ASM
;
; Version:	1.0
;
; Date:		01/02/93
;
; Author:	sandeeps
;
;==============================================================================
;
; Change log:
;
;==============================================================================

	.386p

;******************************************************************************
;			INCLUDE files
;******************************************************************************

	.xlist
        include VMM.INC
        include VCOMM.INC
	include	DEBUG.INC
	include	OPTTEST.INC
	include	VPICD.INC
	include	INTERNAL.INC
	include	INS8250.INC
	include	SHELL.INC
        INCLUDE MSGMACRO.INC
	INCLUDE	VCOMMW32.INC    
        .list

;******************************************************************************
;			LOCKED DATA
;==============================================================================

VxD_Locked_Data_Seg

	EXTRN	Serial_IRQHandler:BYTE
	EXTRN	PortInfoHandle:DWORD
	EXTRN	SysVMHandle:DWORD    

Serial_IRQ_Desc	VPICD_IRQ_Descriptor <0>

	PUBLIC	TimeOutHandle

TimeOutHandle	dd	0
IFDEF	NEC_98
IFDEF	WATCH_MODEM_LINE_STATUS
; Please set avobe WATCH_MODEM_LINE_STATUS condition flag when you want to
; watch the modem status and line status for 8251 really time.
hTimeOutStatus	dd	0
ENDIF	;WATCH_MODEM_LINE_STATUS
ENDIF	;NEC_98

VxD_Locked_Data_Ends

VxD_My_Pageable_Data_Seg

myCommprop label	DWORD 
	dw	SIZE _COMMPROP			; length of struct (mult of 4)
	dw      2				; version
	dd	SP_SERIALCOMM			; services provided
	dd	0				; dwReserved1
	dd	0				; dwMaxTxQueue
	dd	0				; dwmaxRxQueue
	dd	10000000h			; dwMaxBaud
	dd	PST_RS232			; specific COMM provider type
	dd	0FFh				; flow control caps
	dd	007Fh				; dwSettable params
	dd	1000FFFFh			; dwSettableBaud
	dw	0Fh				; wSettableDataBits
	dw	01F07h				; wSettableParityStop
	dd	0				; dwCurrentTxQueue
	dd	0				; dwCurrentRxQueue
	dd	0				; dwProvSpec1
	dd	0				; dwProvSpec2
	dw	0				; wcProvChar1
	dw	0				; filler
.errnz ($ - myCommProp - SIZE _COMMPROP)

PM_Function_Dispatch label 	DWORD
	dd	OFFSET32	pmD1query
    dd	OFFSET32	pmD2query
    dd	OFFSET32	pmD3query
    dd	OFFSET32	pmD1cancel
    dd	OFFSET32	pmD2cancel
    dd	OFFSET32	pmD3cancel
    dd	OFFSET32	pmSetD0
    dd	OFFSET32	pmSetD1
    dd	OFFSET32	pmSetD2
    dd	OFFSET32	pmSetD3
    dd	OFFSET32	pmArmWakeup
    dd	OFFSET32	pmDisArmWakeup   

VxD_My_Pageable_Data_Ends

;******************************************************************************
;			PAGEABLE CODE
;==============================================================================

VxD_My_Pageable_Code_Seg

	EXTRN	GetCOMPort:NEAR
	EXTRN	ReleaseCOMPort:NEAR
	EXTRN	TXI:NEAR
	EXTRN	SetCom100:NEAR
	EXTRN	SetCom200:NEAR
	EXTRN	SetCom300:NEAR
	EXTRN	SetCom400:NEAR
	EXTRN	TrmCom:NEAR
	EXTRN	StaCom:NEAR
	EXTRN	ExtnFcn:NEAR
	EXTRN	Flush:NEAR
	EXTRN	ReadCommString:NEAR
	EXTRN	WriteCommString:NEAR    
	EXTRN	UnMaskIRQ:NEAR
	EXTRN	TimerProc:NEAR
	EXTRN	StealPort:NEAR
	EXTRN	Notify_Owner:NEAR
	EXTRN	DeAllocData:NEAR
	EXTRN	StrCmp:Near
	EXTRN	KickTx:NEAR
IFDEF	NEC_98
	EXTRN	EmuModemStatus:NEAR
ENDIF	; NEC_98

BeginDoc
;******************************************************************************
;
; PPortInformation
; PortOpen (char *PortName, ulong Mode, ulong VMId, ulong *lpError);
;
; Open a port. This assumes that VCOMM called here only
; when it knows that we can support the port. It does not validate 
; parameters because of this assumption.
;
; Parameters:
;		PortName	-> Non-null if open port using name
;		VMId		-> Id of the caller
;		lpError		-> location to update error in
;
; Returns:
;		PortHandle if successful, else 0
;		Updates to *lpError are
;		IE_OPEN		port already open
;		IE_HARDWARE	hardware error
;		IE_DEFAULT	generic error
;==============================================================================
EndDoc
BeginProc PortOpen, CCALL, PUBLIC

ArgVar	PortName,DWORD
ArgVar	VMId,DWORD
ArgVar	lpError,DWORD

	EnterProc
	push	esi
	push	edi
	push	ebx

	mov	edi,PortName		; we are looking for this name.
	mov	esi,[PortInfoHandle]
	VMMCall	List_Get_First

PO_SearchNameLoop:
	jz	InitCom27		; end of list...
	call	StrCmp
	jz	PO_FoundList
	VMMCall	List_Get_Next
	jmp	PO_SearchNameLoop

PO_FoundList:
	mov	esi,eax
	cmp	[esi.pData.ValidPortData],'SMTF' ; Q: Is port already open ?
	je	InitCom17		;  Y: say so

	mov	eax,VMId		; get port for this VM
	call	GetCOMPort		; ask Mr. VCOMM to asign the port
	jnc	InitCom35		; continue
	jc	InitCom17		; did not give us the port.

InitCom26:
	call	ReleaseCOMPort		; give it back to VCD

InitCom27:
	mov	eax,IE_HARDWARE		; report lack of hardware
	jmp	PO_Done

InitCom17:
	mov	eax,IE_OPEN
	cmp	[esi.Port],-1		; Q: Does port exist ?
	jne	PO_Done			;   Y: return IE_OPEN
	jmp	InitCom27		;   N: return IE_HARDWARE

InitCom35:
IFNDEF	NEC_98
	mov	edx, [esi.Port]
	add	dl, 2			; ACE_FCR
	in	al, dx
	test	al, 30h			; Q: Is the port there ?
	jnz	InitCom26		;    N: give it back to VCD.
ELSE	; NEC_98 - fixed #13298
	mov	edx, [esi.Port]
	cmp	dx, 30h			; Q: 1stCCU ?
	jne	InitComNot1stCCU	;   N: do nothing

	Begin_Touch_1st_Meg
	cmp	WORD PTR DS:[0f8e80h], 2198h	; Q: 9821 ?
	jne	InitCom1stCCUEnable		;   N: do nothing

	test	BYTE PTR DS:[0f8e91h], 01h	; Q: [f8e8:11] bit0 = 1 ?
	jnz	InitCom1stCCUDisable		;   Y: 1stCCU is Non-support

	test	BYTE PTR DS:[0f8e93h], 02h	; Q: [f8e8:13] bit1 = 1 ?
	jz	InitCom1stCCUEnable		;   N: 1stCCU isn't I/O locked

InitCom1stCCUDisable:
	End_Touch_1st_Meg
	jmp	InitCom26

InitCom1stCCUEnable:
	End_Touch_1st_Meg

InitComNot1stCCU:

	mov	dx,[esi.StatusPort]	; dx = status port
	in	al,dx			; read status
	cmp	al,0FFh			; Q: is the port there ?
	jz	InitCom26		;    N: give it back to VCD.
ENDIF	; NEC_98

	mov	edx, [esi.MyIRQStruc]	; IRQ Struc for this port
	cmp	[edx.VirtCnt], 0	; Q: Virtualized already
	jz	InitCom_Go_Virtualize	;    N: go virtualize it
	cli
	mov	eax, [edx.MyIRQHandle]
	mov	[esi.IRQHandle], eax	; remember irq handle
	inc	[edx.VirtCnt]
	xor	ecx, ecx
	mov	eax, [edx.PIStruc]

InitComWalkLink:
	cmp	[eax.NextPIStruc], ecx	; Q: last in list ?
	je	InitComLinkItHere	;   Y:
	mov	eax, [eax.NextPIStruc]
	jmp	InitComWalkLink

InitComLinkItHere:
	mov	[eax.NextPIStruc], esi
	sti
	jmp	InitCom_IRQ_Virtualized

InitCom_Go_Virtualize:
	movzx	eax,[esi.IRQn]
	mov	edi, OFFSET32 Serial_IRQ_Desc ; fill out IRQ_Number etc.
	mov	[edi.VID_IRQ_Number],ax
	mov	[edi.VID_Hw_Int_Proc],OFFSET32 Serial_IRQHandler
	mov	[edi.VID_Options],(VPICD_Opt_Can_Share OR VPICD_OPT_REF_DATA)
	mov	[edi.VID_Hw_Int_Ref],edx	; reference data
	VxDCall	VPICD_Virtualize_IRQ
	jc	InitCom26

	mov	[edx.PIStruc], esi	; save PortInfoStruc
	mov	[esi.IRQHandle],eax	; save handle
	mov	[edx.MyIRQHandle], eax	; save for next virtualizer.
	inc	[edx.VirtCnt]		; increment the virt count
	mov	ebx,[SysVMHandle]
	VxDCall	VPICD_Get_Complete_Status
	and	cl,VPICD_STAT_PHYS_MASK	; just remember if physically masked
	mov	[edx.OldMask],cl

InitCom_IRQ_Virtualized:

 ;
 ; Zero Queue counts and indices.
 ;
	lea	edi,[esi.pData.QInCount]
	xor	eax,eax
	mov	ecx,(_PortData.ValidPortData-_PortData.QInCount)/4
	cld
	rep	stosd

	.errnz	_PortData.QInGet-_PortData.QInCount-4
	.errnz	_PortData.QInPut-_PortData.QInGet-4
	.errnz	_PortData.QOutCount-_PortData.QInPut-4
	.errnz	_PortData.QOutGet-_PortData.QOutCount-4
	.errnz	_PortData.QOutPut-_PortData.QOutGet-4
	.errnz	_PortData.ValidPortData-_PortData.QOutPut-4

	mov	[esi.HSFlag],al		; show no handshaking yet
	mov	[esi.MiscFlags],al	; show no discarding
	mov	[esi.pData.LossByte],al	; reset who owns the port
	mov	[esi.SendTrigger],eax
	mov	[esi.NotifyFlags],eax	; reset all flags
	dec	eax
	mov	[esi.RecvTrigger],eax
	mov	[esi.ComDCB.XonLim],eax
	mov	[esi.ComDCB.XoffLim],eax
	mov	[esi.ComDCB.BitMask],fBinary

IFDEF	NEC_98
	mov	dx,[esi.MaskSetPort]
	cli
	mov	al,[esi.MaskSetShadow]
	cmp	dx,35h			; Q: is this mask set register of CH1?
	jnz	short @F		;   N: CH2 or CH3
	in	al,dx			; input mask value from I/O 35h.
	IO_Delay_NEC	5
@@:
	and	al,NOT 00000111B	; disable TxRE, TxEE, RxRE
	out	dx,al
	mov	[esi.MaskSetShadow],al	; save mask value
	sti

	mov	dx,[esi.CountModePort]		; dx = count mode register
	or	dx,dx				; Q: CH2 or CH3?
	jz	short InitCom_Initialize	;   Y: skip to set bps

	push	ecx
	mov	cl,16
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0501h],10000000B	; Q: 8MHz?
	End_Touch_1st_Meg
	jz	short @F			;   N:

	mov	cl,13
@@:
	cli
	;
	; set baudrate 9600
	;
	mov	al,0B6h
	out	dx,al
	IO_Delay_NEC	5
	mov	al,cl
	mov	dx,[esi.CountSetPort]
	out	dx,al
	IO_Delay_NEC	5
	xor	al,al
	out	dx,al
	IO_Delay_NEC	5
	sti
	pop	ecx

InitCom_initialize:
	;
	; initialize 8251
	;
	mov	ax,0300h
	mov	dx,[esi.CommandPort]
@@:
	out	dx,al
	IO_Delay_NEC	5
	push	ecx
	mov	ecx,16
	loop	$
	pop	ecx

	dec	ah
	jnz	short @B

	push	ecx
	mov	ecx,16
	loop	$
	pop	ecx

	mov	al,01000000B
	out	dx,al
	mov	[esi.CommandShadow],al
	IO_Delay_NEC	5

	mov	al,01001110B	; x16 mode, 8 bits, parity none, stop 1
	mov	dx,[esi.ModeAPort]
	out	dx,al
	IO_Delay_NEC	5

	push	ecx
	mov	ecx,16
	loop	$
	pop	ecx

	mov	al,00110111B
	out	dx,al
	mov	[esi.CommandShadow],al
	IO_Delay_NEC	5

	mov	dx,[esi.MaskSetPort]
	mov	al,[esi.MaskSetShadow]
	cmp	dx,35h			; Q: is this mask set register of CH1?
	jnz	short @F		;   N: CH2 or CH3
	in	al,dx			; input mask value from I/O 35h.
	IO_Delay_NEC	5
@@:
	or	al,00000101B		; enable TxRE, RxRE
	out	dx,al
	mov	[esi.MaskSetShadow],al	; save masm value

	mov	[esi.ComDCB.BaudRate],9600	; baudrate 9600
        mov	[esi.ComDCB.ByteSize],8		; 8 bits
	mov	[esi.ComDCB.Parity],0		; parity none
	mov	[esi.ComDCB.StopBits],0		; stop bit 1
ELSE	; NEC_98
	mov	edx,[esi.Port]

	add	dl,ACE_LCR		; EDX -> LCR
	in	al,dx
	mov	ah,al
	or	al,80h
	out	dx,al
	sub	dl,ACE_LCR-ACE_DLL
	in	al,dx
	movzx	ecx,al
	inc	dl
	.errnz	ACE_DLM-ACE_DLL-1
	in	al,dx
	mov	ch,al
	mov	al,ah
	add	dl,ACE_LCR-ACE_DLM
	out	dx,al
	push	edx
	xor	edx,edx
	xor	eax,eax			; assume baud rate of 0
	jecxz	@F
	mov	eax,01C200h
	div	ecx
@@:
	pop	edx			; EDX -> LCR
	mov	[esi.ComDCB.BaudRate],eax
	mov	[esi.ComDCB.ByteSize],8

	dec	dl			; EDX -> FCR
	.errnz	ACE_LCR-ACE_FCR-1
	in	al,dx
	test	al,ACE_FIFO_E2		; Q: FIFO already on ?
	jz	@F			;    N:
	or	[esi.EFlags],fFIFOpre	;    Y: flag it
@@:
ENDIF	; NEC_98
	lea	edi,[esi.pData.dwLastReceiveTime]
	mov	[esi.RxTimeAddr],edi
	lea	edi,[esi.pData.dwDetectedEvents] ; initial AddrEvtDWord
	mov	[esi.AddrEvtDword],edi	; set it
	lea	edi,[esi.pData.bMSRShadow] ; initial AddrMSRShadow
	mov	[esi.AddrMSRShadow],edi	; set it

	and	[esi.EFlags],fEFlagsMask ; clear internal state
	or	[esi.EFlags], fFIFOForcedOff
	xor	eax,eax			; no errors
	mov	[esi.pData.dwCommError],eax ; reset pending bogus error codes
IFDEF	NEC_98
IFDEF	WATCH_MODEM_LINE_STATUS
	cmp	hTimeOutStatus,0	; Q: Have we set a timer already?
	jnz	@F			;  Y: get out
	push	eax
	push	esi
	mov	eax,10			;  N: set a 10 ms timer
	mov	edx,[PortInfohandle]	; List handle is ref data
	mov	esi,OFFSET32 StatusWatchTimer
	VMMCall	Set_Global_Time_Out
	mov	hTimeOutStatus,esi
	pop	esi
	pop	eax
@@:
ENDIF	;WATCH_MODEM_LINE_STATUS
ENDIF	;NEC_98

PO_Done:
	mov	ebx,lpError
	mov	[ebx],eax		; save error for caller
	or	eax,eax			; Q: Is everything ok ?
	jnz	PO_Failure		;    N: get out
	mov	ebx,VMId		;    Y: get owner of VM
	mov	[esi.OwnerVM],ebx	; save it
	mov	[esi.pData.dwLastError],eax ; save error code
IFNDEF	NEC_98
	mov	edx, [esi.Port]
	add	dl, ACE_MCR
	in	al, dx
	or	al, ACE_OUT2
	out	dx, al
ENDIF	; !NEC_98
	call	UnMaskIRQ
	and	[esi.pData.dwCommError], 0
	mov	eax, esi
	jmp	PO_Success

PO_Failure:
	xor	eax,eax			; FALSE

PO_Success:
	pop	ebx
	pop	edi
	pop	esi
	LeaveProc
	return

EndProc PortOpen


BeginDoc
;******************************************************************************
;
; BOOL
; PortGetState(HPORT hPort, DCB *pDcb);
;
; Parameters:
;		hPort	=  port handle
;		pDcb	-> DCB to fill in
; Returns:
;		TRUE if OK, else false.
;==============================================================================
EndDoc
BeginProc PortGetState, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	pDcb,DWORD

	EnterProc

	push	esi
	push	edi
	push	ebx

	xor	eax,eax				; access to 0
	mov	esi,hPort
	mov	[esi.pData.dwLastError],eax	; no error
	lea	esi,[esi.ComDCB]
	mov	edi,pDcb
	mov	ecx,(SIZE _DCB)/4
	rep	movsd

	inc	eax				; success

	pop	ebx
	pop	edi
	pop	esi

	LeaveProc
	return

EndProc PortGetState

BeginDoc
;******************************************************************************
;
; BOOL
; PortClose(HPORT hPort);
;
; Parameters:
;		hPort	= port handle
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortClose, CCALL, PUBLIC

ArgVar	hPort,DWORD

	EnterProc
	push	esi
	push	edi
	push	ebx

	mov	esi,hPort		; esi -> port information
	call	TrmCom

PClose_Done:
	mov	[esi.pData.dwLastError],eax

	test	[esi.MiscFlags],RxQInternal	; N: free if we alloced them
	jz	PClose_RxFree
	VMMCall	_HeapFree,<[esi.pData.QInAddr],0>

PClose_RxFree:
	test	[esi.MiscFlags],TxQInternal
	jz	PClose_TxFree
	VMMCall	_HeapFree,<[esi.pData.QOutAddr],0>

PClose_TxFree:

	mov	ebx,esi			; save PortInformation struct.
	SetFlag	[esi.MiscFlags], ClrTimer ; remove this port from consideration
	call	ManageTimer		; ManageTimer SAVES EBX.
IFDEF	NEC_98
IFDEF	WATCH_MODEM_LINE_STATUS
	push	esi
	xor	esi,esi
	xchg	esi,hTimeOutStatus
	VMMCall	Cancel_Time_Out
	pop	esi
ENDIF	;WATCH_MODEM_LINE_STATUS
ENDIF	;NEC_98

	call	DeAllocData

	or	al,1			; success

	pop	ebx
	pop	edi
	pop	esi

	LeaveProc
	return

EndProc PortClose

BeginDoc
;******************************************************************************
;
; BOOL
; PortSetModemStatusShadow(HPORT hPort, ulong EventMask, BYTE *MSRShadow);
;
; Parameters:
;		hPort	= port handle
;		EventMask = event mask
;		MSRShadow = Addr to update WIN 3.1 style MSRShadow.
;
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortSetModemStatusShadow, esp, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	MSRShadow,DWORD

	EnterProc
	mov	edx,hPort
	mov	ecx,MSRShadow
	mov	[edx.AddrMSRShadow],ecx
	xor	eax,eax				; access to 0
	mov	[edx.pData.dwLastError],eax
	inc	eax
	LeaveProc
	return

EndProc PortSetModemStatusShadow

BeginDoc
;******************************************************************************
;
; BOOL
; PortGetProperties(HPORT hPort, COMMPROP *pCommprop);
;
; Parameters:
;		hPort	= port handle
;		pCommprop -> property structure to fill in
;
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortGetProperties, esp, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	pCommprop,DWORD

	EnterProc

	SaveReg	<esi,edi>

	mov	esi,OFFSET32 myCommProp
	mov	edi,pCommProp
	mov	eax,edi				; save ptr
	mov	ecx,SIZE _COMMPROP
	rep	movsb
	mov	edx,hPort			; edx -> hPort
	mov	ecx,[edx.pdata.QInSize]		; return the current q sizes.
	mov	[eax._COMMPROP.dwCurrentRxQueue],ecx
	mov	ecx,[edx.pdata.QOutSize]
	mov	[eax._COMMPROP.dwCurrentTxQueue],ecx
	xor	eax,eax
	mov	[edx.pData.dwLastError],eax	; EAX is 0, no error
	inc	eax

	RestoreReg <edi,esi>
	LeaveProc
	return

EndProc PortGetProperties

BeginDoc
;******************************************************************************
;
; BOOL
; PortSetEventMask(HPORT hPort, long EventMask, WORD *EventMaskLocation);
;
; Parameters:
;		hPort			= port handle
;		EventMask		= event mask to set
;		EventMaskLocation	= location of event mask DWORD.
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortSetEventMask, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	EventMask,DWORD
ArgVar	EventMaskLoc,DWORD

	EnterProc

	mov	edx,hPort
	mov	eax,EventMask		; get event mask to set
	mov	[edx.EvtMask],eax	; set it
	mov	ecx,EventMaskLoc	; get new location of event dword
	jecxz	PSEM_NoNewLoc		; set it only if non-zero
	mov	[edx.AddrEvtDWord],ecx
PSEM_NoNewLoc:
	xor	eax,eax			; access to 0
	mov	[edx.pData.dwLastError],eax ; no error
	inc	eax

	LeaveProc
	return

EndProc PortSetEventMask

BeginDoc
;******************************************************************************
;
; BOOL
; PortEnableNotification(HPORT hPort, PFN lpFunc, ulong RefData);
;
; Parameters:
;		hPort	= port handle
;		lpFunc	-> pointer to function to call
;		RefData	= pass in this data while calling
;
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortEnableNotification, CCALL, esp, PUBLIC

ArgVar	hPort,DWORD
ArgVar	lpFunc,DWORD
ArgVar	RefData,DWORD

	EnterProc

	mov	edx,hPort
	mov	ecx,lpFunc
	mov	eax,RefData
	and	[edx.NotifyFlagsHI], NOT CN_Notify ; assume turning off notify
	mov	[edx.ReferenceData],eax	; save reference data here
	mov	[edx.NotifyHandle],ecx	; save proc to call
	jecxz	@F			; Q: Is assumption correct ?
	cli
	or	[edx.NotifyFlagsHI], CN_Notify	;    N: say so
	mov	ecx,[edx.AddrEvtDword]	; get accumulated events address
	mov	eax,[edx.EvtMask]	; detected events client is
	and	eax,[ecx]		; is interested in
	sti
	or	eax,eax			; Q: Any events seen yet ?
	jz	@F			;    N:
	SaveReg	<esi>
	mov	esi,edx
	mov	eax,CN_EVENT
	call	Notify_Owner
	RestoreReg <esi>
@@:
	xor	eax,eax			; access to 0
	mov	[edx.pData.dwLastError],eax ; no error for this function
	inc	eax

	LeaveProc
	return

EndProc PortEnableNotification

;******************************************************************************
;
; BOOL
; PortSetReadCallBack(HPORT hPort, ulong RxTrigger, ulong RxCallBack,
;		      ulong RxRefData);
;
; Parameters:
;		hPort	   = port handle of an open port
;		RxTrigger  = high watermask for receive queue
;		RxCallBack = when receive population becomes greater than
;			     the RxTrigger this procedure is called.
;		RxRefData  = RxCallBack is called with this reference data
;
; Returns:
;		TRUE if OK, else FALSE.
;
;==============================================================================

BeginProc PortSetReadCallBack,CCALL,PUBLIC

ArgVar	hPort,DWORD
ArgVar	RxTrigger,DWORD
ArgVar	RxCallBack,DWORD
ArgVar	RxRefData,DWORD

	EnterProc
	push	esi
	push	ebx

	mov	ecx,RxTrigger
	mov	esi,hPort

	mov	eax,ecx
	inc	eax
	jz	psr_recv_ok
	cmp	ecx,[esi.pData.QInSize]	; Q: receive threshold reasonable ?
	jb	psr_recv_ok		;    Y:
	mov	ecx,[esi.pData.QInSize]	;    N:

psr_recv_ok:
	mov	ebx,RxCallBack
	or	ebx,ebx			; Q: null read callback
	jnz	psr_save_read_threshold
	or	ecx,-1

psr_save_read_threshold:
	cli
	mov	[esi.ReadNotifyHandle],ebx
	mov	edx,RxRefData
	mov	[esi.ReadNotifyRefData],edx
	mov	[esi.RecvTrigger],ecx
	sti
	cmp	[esi.pData.QInCount],0	; Q: Does input queue contain data ?
	jne	@F			;    Y: keep dwLastReceiveTime.
	mov	esi,[esi.RxTimeAddr]
	mov	DWORD PTR [esi],0
@@:
	call	ManageTimer

	or	al,1			; success

	pop	ebx
	pop	esi

	LeaveProc
	return

EndProc PortSetReadCallBack


;******************************************************************************
;
; BOOL
; PortSetWriteCallBack(HPORT hPort, ulong TxTrigger, ulong TxCallBack,
;		      ulong TxRefData);
;
; Parameters:
;		hPort	   = port handle of an open port
;		TxTrigger  = low watermask for xmit queue
;		TxCallBack = when xmit population becomes less than
;			     the TxTrigger this procedure is called.
;		TxRefData  = TxCallBack is called with this reference data
;
; Returns:
;		TRUE if OK, else FALSE.
;
;==============================================================================

BeginProc PortSetWriteCallBack,CCALL,PUBLIC

ArgVar	hPort,DWORD
ArgVar	TxTrigger,DWORD
ArgVar	TxCallBack,DWORD
ArgVar	TxRefData,DWORD

	EnterProc

	push	esi
	push	ebx

	mov	edx,TxTrigger
	mov	esi,hPort

	inc	edx				; TxTrigger = -1 => no notify
	jz	psw_send_ok
	dec	edx
	TestMem	[esi.MiscFlags],TxQSet	; Q: output buffer set ?
	jz	psw_send_ok		;    N:
	cmp	edx,[esi.pData.QOutSize]	; Q: send threshold reasonable?
	jbe	psw_send_ok
	mov	edx,[esi.pData.QOutSize]

psw_send_ok:
	mov	ebx,TxCallBack
	or	ebx,ebx
	jnz	psw_save_write_threshold
	xor	edx,edx

psw_save_write_threshold:
	cli
	mov	[esi.SendTrigger],edx
	mov	[esi.WriteNotifyHandle],ebx
	mov	ebx,TxRefData
	mov	[esi.WriteNotifyRefData],ebx
	cmp	[esi.pData.QOutCount],edx	; Q: Is QOutCount >= trigger ?
	jae	psw_sendXmitWhenWMHit		;   Y: send notification when
						;    we fall below the trigger
	or	[esi.NotifyFlagsHI],CN_TRANSMIT	;   N: we are below the trigger
						;      at this point, don't
						;      generate notification
						;      till we reach above it.
psw_sendXmitWhenWMHit:
	sti
	or	al,1				; success

	pop	ebx
	pop	esi

	LeaveProc
	return

EndProc PortSetWriteCallBack

;******************************************************************************
;
; BOOL PortGetModemStatus(HPORT hPort, DWORD *pModemStatus);
;
; Parameters:
;		hPort -> port handle
;		pModemStatus -> return current modem status
;
; Exit:
;	if True, *pModemStatus contains the status
;
;==============================================================================
BeginProc PortGetModemStatus, CCALL, esp, PUBLIC

ArgVar	hPort,DWORD
ArgVar	pModemStatus,DWORD

	EnterProc
	mov	edx,hPort
	mov	ecx, [edx.AddrMSRShadow]
IFDEF	NEC_98
	push	ecx
	push	esi

	mov	esi,edx
	mov	dx,[esi.StatusPort]	; dx = status register
	in	al,dx
	IO_Delay_NEC	5

	xor	ecx,ecx

	test	al,10000000B		; DR
	jz	short @F
	or	ecx,MS_DSR_ON
@@:
	mov	dx,[esi.ReadSignPort]	; dx = read signal register
	in	al,dx
	IO_Delay_NEC	5

	test	al,01000000B		; CS
	jnz	short @F
	or	ecx,MS_CTS_ON
@@:
	test	al,10000000B		; CI
	jnz	short @F
	or	ecx,MS_RING_ON
@@:
	test	al,00100000B		; CD
	jnz	short @F
	or	ecx,MS_RLSD_ON
@@:
	mov	eax, ecx		; MS modem status set to EAX
	pop	esi
	pop	ecx
ELSE	; NEC_98
	mov	edx,[edx.Port]		; edx = base port
	add	dl,ACE_MSR		; edx = MSR register
	in	al,dx			; get current value
ENDIF	; NEC_98
	mov	BYTE PTR [ecx], al	; save Modem status shadow.
	and	eax,MS_Modem_Status	; isolate bits of interest
	mov	edx,pModemStatus	; edx -> fill data here
	mov	dword ptr [edx],eax	; fill it
	or	al,1			; success
	LeaveProc
	return

EndProc PortGetModemStatus

;******************************************************************************
;
; BOOL
; PortGetCommConfig(HPORT hPort, LPCOMMCONFIG lpCC, LPDWORD lpSize);
;
; Description:
;
;	If (*lpSize >= SIZE _COMM_CONFIG) then return the config
;	 else fail it and return SIZE _COMM_CONFIG in *lpSize.
;
;	NOTE: The _COMM_CONFIG structure includes a Win32 style DCB
;	      and NOT a ring0 DCB. So we need to call VCOMM to convert
;	      our DCB into Win32 style one.
;
; Parameters:
;		hPort = port handle
;		lpCC  -> LPCOMMCONFIG
;		lpSize -> fill it with size of DCB
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
BeginProc PortGetCommConfig,CCALL,esp,PUBLIC

ArgVar	hPort,DWORD
ArgVar	lpCC,DWORD
ArgVar	lpSize,DWORD

	EnterProc

	xor	eax,eax				; assume failure
	mov	edx,lpSize
	push	SIZE _COMM_CONFIG
	pop	ecx
	cmp	DWORD PTR [edx], ecx		; Q: Is buffer large enough?
	mov	DWORD PTR [edx], ecx

%OUT BUGBUG Not certain about error codes here (new API).

	jb	PGCC_Success			;    N: just return size
	mov	edx,lpCC			; EDX -> _COMM_CONFIG
	or	edx,edx				; Q: Is LPCOMMCONFIG 0 ?
	jz	PGCC_Success			;    Y: just return size
	mov	[edx._COMM_CONFIG.cc_dwProviderOffset],eax
	mov	[edx._COMM_CONFIG.cc_dwProviderSize],eax
	mov	[edx._COMM_CONFIG.cc_wcProviderData],al

	mov	al,SIZE _COMM_CONFIG
	.errnz	(SIZE _COMM_CONFIG) AND 0FFFFFF00h
	mov	[edx._COMM_CONFIG.cc_dwSize],eax

	mov	al,PST_RS232
	.errnz	PST_RS232 AND 0FFFFFF00h
	mov	[edx._COMM_CONFIG.cc_dwProviderSubType],eax

	shl	eax,8
	.errnz	PST_RS232 - 1
	mov	[edx._COMM_CONFIG.cc_wVersion],ax ; version = 1.0000

	mov	eax,hPort
	lea	eax,[eax.ComDCB]		; EAX -> Ring0DCB
	lea	edx,[edx._COMM_CONFIG.cc_dcb]	; EDX -> Win32 DCB.

	VxDCall	VCOMM_Map_Ring0DCB_To_Win32	; EAX -> Ring0DCB, EDX -> Win32

PGCC_Success:
	or	al,1				; success

PGCC_Done:
	LeaveProc
	return

EndProc PortGetCommConfig

;******************************************************************************
;
; BOOL
; PortSetCommConfig(HPORT hPort, LPCOMMCONFIG lpCC, DWORD dwSize);
;
;	NOTE: The _COMM_CONFIG structure includes a Win32 style DCB
;	      and NOT a ring0 DCB. So we need to call VCOMM to convert
;	      it to Ring0 style DCB.
;
; Parameters:
;		hPort = port handle
;		lpCC  -> LPCOMMCONFIG
;		dwSize -> size to copy
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
BeginProc PortSetCommConfig,CCALL,esp,PUBLIC

ArgVar	hPort,DWORD
ArgVar	lpCC,DWORD
ArgVar	dwSize,DWORD

LocalVar lDcb, %(size _DCB)

	EnterProc

	xor	eax,eax
	mov	edx,hPort
	mov	[edx._PortData.dwLastError],IE_INVALIDPARAM

	cmp	dwSize,SIZE _COMM_CONFIG	; Q: Valid lpCommConfig ?
	jb	PSCC_Done			;    N:

	lea	edx,lDcb			; EDX -> ring 0 DCB
	mov	eax,lpCC
	lea	eax,[eax._COMM_CONFIG.cc_dcb]	; EAX -> ring 3 DCB
	VxDCall	VCOMM_Map_Win32DCB_To_Ring0

	cCall	PortSetState,<hPort,edx,-1>	; call to set state

PSCC_Done:
	LeaveProc
	return

EndProc PortSetCommConfig

;******************************************************************************
;
; BOOL
; PortGetWin32Error(HPORT hPort, ulong *dwError);
;
; Parameters:
;	hPort -> Port handle
;	dwError -> SERIAL.386 fills this with Win32 style error
;
; Returns: TRUE
; Uses: C style
;==============================================================================
BeginProc PortGetWin32Error,CCALL,esp,PUBLIC

ArgVar	hPort,DWORD
ArgVar	dwError,DWORD

	EnterProc

	LeaveProc
	return

EndProc PortGetWin32Error

VxD_My_Pageable_Code_Ends

;******************************************************************************
;			LOCKED CODE
;==============================================================================

VxD_Locked_Code_Seg

IFDEF	NEC_98
IFDEF	WATCH_MODEM_LINE_STATUS
	EXTRN	LineStat:NEAR
	EXTRN	ModemStatus:NEAR
ENDIF	;WATCH_MODEM_LINE_STATUS
ENDIF	;NEC_98

BeginDoc
;******************************************************************************
;
; BOOL
; PortGetQueueStatus(HPORT hPort, COMSTAT *pComstat);
;
; Parameters:
;		hPort	= port handle
;		pComstat -> status structure of queue to fill in.
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortGetQueueStatus, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	pComstat,DWORD

	EnterProc

	push	esi
	push	ebx
	mov	esi,hPort
	mov	ebx,pComstat
	call	StaCom

	xor	eax,eax
	mov	[esi.pData.dwLastError],eax
	inc	eax

	pop	ebx
	pop	esi
	LeaveProc
	return

EndProc PortGetQueueStatus

BeginDoc
;******************************************************************************
;
; BOOL
; PortSetState(HPORT hPort, DCB *pDcb, long ActionMask);
;
; Parameters:
;		hPort		-> Port handle
;		pDcb		-> DCB to set from
;		ActionMask	= dword specifying relevant DCB fields.
; Returns:
;		TRUE if OK, else false.
;==============================================================================
EndDoc
BeginProc PortSetState, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	pDcb,DWORD
ArgVar	ActionMask,DWORD

LocalVar BaudRateChange,DWORD	

	EnterProc
	push	esi
	push	edi
	push	ebx

	cld
	mov	esi,hPort		; ESI -> PortInformation

	test	[esi.pData.LossByte],1	; Q: Do we own the port
	jz	PSS_SerialOwnsPort	;    Y: continue
	call	StealPort		;    N: try to steal it
	mov	eax,IE_Default		; assume failure to steal
	jz	PSS_Done		; jump if failed

PSS_SerialOwnsPort:
	mov	ebx,pDcb		; EBX -> DCB
	TestMem	ActionMask,fBaudRate	; is client interested in setting baud?
	jz	PSS_NoBaudCheck
	call	SetCom300		; Baud rate valid ?
	jecxz	PSS_D0			;  No, return error

PSS_NoBaudCheck:
	TestMem	ActionMask,<fByteSize OR fbParity OR fStopBits>
	jz	PSS_NoLCRCheck
	call	SetCom400		; validate Byte size/parity/stop bits
	jnc	PSS_NoLCRCheck

PSS_D0:
	jmp	PSS_Done

PSS_NoLCRCheck:
 ;
 ; Parameters seem correct. Copy relevant DCB fields into our
 ; space and initialize ACE with relevant parameters.
 ;
IFDEF	NEC_98
	mov	dx,[esi.MaskSetPort]	; dx = mask set register
	cli
	mov	al,[esi.MaskSetShadow]
	cmp	dx,35h			; Q: is this mask set register of CH1?
	jnz	short @F		;   N: CH2 or CH3
	in	al,dx			; input mask value from I/O 35h.
	IO_Delay_NEC	5
@@:
	and	al,NOT 00000111B	; disable TxRE, TxEE, RxRE
	out	dx,al
	mov	[esi.MaskSetShadow],al	; save mask value
	sti
ELSE	; NEC_98
	mov	edx,[esi.Port]		; disable interrupts first
	inc	edx
	.errnz	ACE_IER-1
	xor	eax,eax

	out	dx,al
ENDIF	; NEC_98

	mov	ecx, [ebx._DCB.BaudRate]
	sub	ecx, [esi.ComDCB.BaudRate]
	mov	BaudRateChange, ecx

	mov	ecx,ActionMask
	call	SetCom100		; Copy DCB selectively!
	lea	ebx,[esi.ComDCB]	; set EBX -> DCB

	TestMem	ActionMask,fTimeout	; is caller interested in timeouts ?
	jz	PSS_NoTimeoutSet
	call	SetCom200		; get timeout masks, ah = check, al=inf
	xchg	al,ah			; get them in correct registers
	mov	word ptr [esi.MSRMask],ax
	.errnz	MSRInfinite-MSRMask-1

PSS_NoTimeoutSet:

	add	dl,2			; --> LCR
	TestMem	ActionMask, <fByteSize OR fbParity OR fStopBits>
	jz	PSS_NoLCRSet
	call	SetCom400		; get line control byte
	push	eax			; and save LCR value
IFDEF	NEC_98
	push	eax

	; bits size
	mov	ah,al
	and	ah,ACE_WLS
	shl	ah,2

	; parity
	test	al,ACE_PEN
	jz	short @F
	or	ah,00010000B
	test	al,ACE_EPS
	jz	short @F
	or	ah,00100000B
@@:
	; stop bits
	mov	al,[ebx._DCB.StopBits]
	inc	al			; 0, 1, 2 to 1, 2, 3
	shl	al,6
	or	al,ah
	or	al,00000010B		; x16
	;
	; initialize 8251
	;
	push	eax
	mov	ax,0300h
	mov	dx,[esi.CommandPort]
@@:
	out	dx,al
	IO_Delay_NEC	5
	push	ecx
	mov	ecx,16
	loop	$
	pop	ecx

	dec	ah
	jnz	short @B

	mov	dx,[esi.CommandPort]
	mov	al,[esi.CommandShadow]
	or	al,01000000B
	out	dx,al
	mov	[esi.CommandShadow],al
	IO_Delay_NEC	5

	push	ecx
	mov	ecx,16
	loop	$
	pop	ecx

	pop	eax
	mov	dx,[esi.ModeAPort]
	out	dx,al
	IO_Delay_NEC	5

	mov	dx,[esi.CommandPort]
	mov	al,[esi.CommandShadow]
	and	al,NOT 01000000B
	out	dx,al
	mov	[esi.CommandShadow],al
	IO_Delay_NEC	5

	pop	eax
ELSE	; NEC_98
	.errnz	ACE_LCR-ACE_IER-2
	or	al,ACE_DLAB		; access divisor latch
	out	dx,al
ENDIF	; NEC_98
	mov	[esi.RxMask],ah		; save Receive char mask
	mov	eax,edi			; get flag mask, error mask
	and	byte ptr [esi.ComDCB.BitMask],ah ; disable parity checking
					; if no parity
	mov	[esi.ErrorMask],al	; save line status error mask
	pop	eax

PSS_NoLCRSet:
	TestMem	ActionMask,fBaudRate
	jz	PSS_NoBaudSet

	push	eax
	call	SetCom300		; get baud rate
IFDEF	NEC_98
	mov	dx,[esi.CountModePort]	; dx = count mode register
	or	dx,dx			; Q: CH2 or CH3?
	jz	short @f		;   N: skip to set bps
	;
	; set baudrate
	;
	mov	al,0B6h
	cli
	out	dx,al
	IO_Delay_NEC	5
	mov	dx,[esi.CountSetPort]
	mov	al,cl
	out	dx,al
	IO_Delay_NEC	5
	mov	al,ch
	out	dx,al
	IO_Delay_NEC	5
	sti
@@:
	pop	eax
ELSE	; NEC_98
	sub	dl,ACE_LCR-ACE_DLL	; --> LSB of divisor latch
	mov	al,cl
	out	dx,al
	mov	al,ch
	inc	edx			; --> MSB of divisor latch
	.errnz ACE_DLM-ACE_DLL-1
	IO_Delay
	out	dx,al
	add	dl,2			; --> LCR and clear divisor bits
	.errnz ACE_LCR-ACE_DLM-2
	pop	eax
	out	dx,al
ENDIF	; NEC_98

PSS_NoBaudSet:
	inc	dl			; --> MCR
	.errnz	ACE_MCR-ACE_LCR-1

	TestMem	ActionMask,fBitMask	; is caller interested in this ?
	jz	PSS_NoBitmaskWork

 ;
 ;	Compute initial state of DTR and RTS. If they have been disabled,
 ;	then do not raise them, and disallow being used as a handshaking
 ;	line. Also compute the bits to use as hardware handshake bits
 ;	(DTR and/or RTS as indicated, qualified with the disabled flags).
 ;
	mov	al,byte ptr [esi.ComDCB.Bitmask] ; align DTR/RTS disable 
					; flags for 8250
	and	al,fRTSDisable+fDTRDisable
	rol	al,1			;d0 = DTR, d2 = RTS  (1 = disabled)
	shr	al,1			;'C'= DTR, d1 = RTS
	adc	al,0			;d0 = DTR, d1 = RTS
	.errnz	fRTSDisable-00000010b
	.errnz	fDTRDisable-10000000b
	.errnz	ACE_DTR-00000001b
	.errnz	ACE_RTS-00000010b

	mov	ah,al			;Save disable mask
IFDEF	NEC_98
	mov	dx,[esi.CommandPort]
	mov	cl,[esi.CommandShadow]
	and	cl,NOT 00100010B
	xor	al,ACE_DTR+ACE_RTS
	test	al,ACE_DTR		; Q: DTR on?
	jz	short @F		;   N: skip
	or	cl,00000010B		; DTR
@@:
	test	al,ACE_RTS		; Q: RTS on?
	jz	short @F		;   N: skip
	or	cl,00100000B		; RTS
@@:
	mov	al,cl
	out	dx,al
	mov	[esi.CommandShadow],al
	IO_Delay_NEC	5
ELSE	; NEC_98
	xor	al,ACE_DTR+ACE_RTS+ACE_OUT2
	out	dx,al			;Set Modem Control Register
ENDIF	; NEC_98

	mov	al,byte ptr [esi.ComDCB.BitMask+1] ;Get hardware 
					;handshake flags
	rol	al,1			;Align flags as needed
	rol	al,1
	rol	al,1
	and	al,ACE_DTR+ACE_RTS	;Mask bits of interest
	not	ah			;Want inverse of disable mask
	and	al,ah			;al = bits to handshake with
	mov	[esi.HHSLines],al 	;Save for interrupt code


	.errnz	fDTRFlow-2000h
	.errnz	fRTSFlow-4000h
	.errnz	ACE_DTR-00000001b
	.errnz	ACE_RTS-00000010b

	mov	al,byte ptr [esi.ComDCB.BitMask] ;Compute the mask 
					;  for the output
	shl	al,1			;  hardware handshake lines
	and	al,ACE_DSR+ACE_CTS
	mov	[esi.OutHHSLines],al

	.errnz	fOutXCTSFlow-00001000b
	.errnz	fOutXDSRFlow-00010000b
	.errnz	ACE_CTS-00010000b
	.errnz	ACE_DSR-00100000b

PSS_NoBitmaskWork:

; Compute the queue count where XOff should be issued (or hardware
; lines dropped).  This will prevent having to do it at interrupt
; time.

	mov	eax,[esi.pData.QInSize]	; get where they want it
	sub	eax,[esi.ComDCB.XoffLim];  and compute queue count
	mov	[esi.XoffPoint],eax

	TestMem	[esi.MiscFlags], SetComStateDone ; Q: 1st SetComState called ?
	jz	@F				 ;    Y: 
;
; SetCommState has already been called once. Now if the baud rate is
; is same, then Eflags contain the correct flags from the point of
; view of FIFO enabling etc. So we can skip setting the fifo etc
; and the consequent delay loop.
;
	cmp	BaudRateChange, 0
	jne	@F
	sub	dl,ACE_MCR-ACE_RBR
	jmp	sc_skip_fifo_and_delay
@@:
;
; Try to drain data from the UART.
;
IFDEF	NEC_98
	mov	ah,16			; a max of 16 times

PSS_Try_Drain_Data:
	mov	dx,[esi.StatusPort]
	in	al,dx
	IO_Delay_NEC	5
	test	al,00000010B
	jz	short PSS_Data_Drained
	mov	dx,[esi.DataReadPort]
	in	al,dx
	IO_Delay_NEC	5
ELSE	; NEC_98
	sub	dl,ACE_MCR-ACE_FCR	; EDX = FCR
	xor	al,al
	out	dx,al			; disable FIFO if present.
	mov	ah,16			; a max of 16 times
	add	dl,ACE_LSR-ACE_FCR	; EDX = LSR

PSS_Try_Drain_Data:
	in	al,dx
	test	al,ACE_DR		; Q: Is data in the buffer
	jz	PSS_Data_Drained	;    N:
	sub	dl,ACE_LSR-ACE_RBR	; EDX = data register
	in	al,dx			; read in one byte
	add	dl,ACE_LSR-ACE_RBR	; EDX = LSR
ENDIF	; NEC_98
	dec	ah
	jnz	PSS_Try_Drain_Data

PSS_Data_Drained:

;
; Enable FIFO if possible
;
IFNDEF	NEC_98
	and	[esi.EFlags], NOT fFifoForcedOff
	sub	dl,ACE_LSR - ACE_FCR	; EDX = FCR
	test	[esi.EFlags], fNoFIFO	; Q: Can FIFO be enabled ?
	jnz	sc_nofifo		;   N:
	mov	eax, [esi.ComDCB.BaudRate]
	cmp	eax, 4800
	jb	sc_nofifo
	cmp	ah, -1			; Q: Baudrate index ?
	jne	sc_fifo			;    N: baudrate >= 4800, enable it
	cmp	eax, CBR_4800
	jb	sc_nofifo

sc_fifo:
	mov	al,ACE_EFIFO OR ACE_CRFIFO OR ACE_CTFIFO
	or	al,[esi.RxFifoTrigger]
	out	dx,al			; attempt to enable it
	test	[esi.EFlags], fFIFOchkd	; Q: FIFO detect been done ?
	jnz	sc_fifodone		;  Y: enabled FIFO
	IO_Delay
	.errnz	ACE_IIDR-ACE_FCR
	in	al,dx
	or	[esi.EFlags],fFIFOchkd
	test	al,ACE_FIFO_E2		; Q: FIFO enabled ?
	jz	@F
	test	al,ACE_FIFO_E1		; Q: 16550A detected?
	jnz	sc_fifodone		;  Y: enabled FIFO
@@:
	IO_Delay
	or	[esi.Eflags], fNoFIFO

sc_nofifo:
	or	[esi.Eflags], fFifoForcedOff
	xor	al,al
	out	dx,al
sc_fifodone:
	sub	dl,ACE_FCR-ACE_RBR	; dx -> RBR
ELSE	; !NEC_98
	mov	dx,[esi.DataReadPort]
ENDIF	; !NEC_98

;
; Delay for things to settle
;
	VMMCall	Get_System_Time
	mov	ecx,eax
delay_loop:
	in	al,dx			; read it once
	mov	eax,DELAY_TIME/2
	VMMCall	Time_Slice_Sleep
	VMMCall	Get_System_Time
	sub	eax,ecx
	cmp	eax,DELAY_TIME		; Q: timeout reached ?
	jb	delay_loop		;  N:

sc_skip_fifo_and_delay:
IFDEF	NEC_98
	call	EmuModemStatus
ELSE	; NEC_98
	add	dl,ACE_MSR		;--> modem status reg
	in	al,dx			; Throw away 1st status read
	IO_Delay
	in	al,dx			; save 2nd for MSRWait (clear MSR int)
ENDIF	; NEC_98
	push	esi
	mov	esi,[esi.AddrMSRShadow]
	mov	byte ptr [esi],al	; save it
	pop	esi

	mov	[esi.HSFlag],0
;
; Win 3.0 didn't check hardware handshaking until the line status changed.
; Allow some apps to keep that behavior.
;
	TestMem	[esi.ComDCB.BitMask], fWin30Compat
	jnz	sc_HHSup

;
; HACK FOR SOME MODEMS:  apparently some modems set CTS, but don't set DSR
; which means that COMM.DRV won't send if the app specifies that hardware
; handshaking is based on CTS & DSR being set.
;
	mov	eax,[esi.AddrMSRShadow]
	mov	al,[eax]		; get the shadow
	mov	ah,[esi.OutHHSLines]
	and	al,ah			;Only leave bits of interest
	cmp	al, ah			;Q: handshaking lines ok?
	je	sc_HHSup		;   Y:
	cmp	ah, ACE_CTS OR ACE_DSR	;Q: app looking for both high?
	jne	sc_HHSdown		;   N: skip hack
	test	[esi.EFlags], fUseDSR	;Q: DSR is always significant?
	jnz	sc_HHSdown		;   Y: skip hack
	cmp	al, ACE_CTS		;Q: DSR low & CTS high
	jne	sc_HHSdown		;   N: skip hack
	and	ah, NOT ACE_DSR 	;   Y: ignore DSR line
	mov	[esi.OutHHSLines], ah
	jmp	sc_HHSup

sc_HHSdown:
	or	[esi.HSFlag], HHSDown OR HHSAlwaysDown ; flag handshaking down
sc_HHSup:

 ;
 ; Now, at last, interrupts can be enabled.  Don't enable the
 ; transmitter empty interrupt.  It will be enabled by the first
 ; call to KickTx.
 ;

IFDEF	NEC_98
	mov	dx,[esi.MaskSetPort]	; dx = mask set register
	cli
	mov	al,[esi.MaskSetShadow]
	cmp	dx,35h			; Q: is this mask set register of CH1?
	jnz	short @F		;   N: CH2 or CH3
	in	al,dx
	IO_Delay_NEC	5
@@:
	or	al,00000001B		; enable RxRE
	and	al,NOT 00000110B	; disable TxRE, TxEE
	out	dx,al
	mov	[esi.MaskSetShadow],al
	IO_Delay_NEC	5
	mov	dx,[esi.StatusPort]
	in	al,dx
	IO_Delay_NEC	5
	mov	dx,[esi.DataReadPort]
	in	al,dx
	sti
ELSE	; NEC_98
	sub	dl,ACE_MSR-ACE_IER	;--> Interrupt Enable Register

	mov	al,ACE_ERBFI+ACE_ELSI+ACE_EDSSI
	cli
	out	dx,al			;Enable interrupts.
	add	dl,ACE_LSR-ACE_IER	;--> Line Status Register
	IO_Delay
	in	al,dx			;Clear any Line Status interrupt
	sub	dl,ACE_LSR		;--> Receiver Buffer Register
	IO_Delay
	in	al,dx			;Clear any Received Data interrupt
	sti
ENDIF	; NEC_98
	xor	eax,eax 		;All done

PSS_Done:
	mov	[esi.pData.dwLastError],eax ; save error
	or	eax,eax			; any errors ?
	jnz	PSS_Failed

PSS_IRQUnmasked:
	SetFlag	[esi.MiscFlags], SetComStateDone
	or	al,1
	jmp	PSS_Success

PSS_Failed:
	xor	eax,eax

PSS_Success:
	pop	ebx
	pop	edi
	pop	esi
	LeaveProc
	return

EndProc PortSetState

BeginDoc
;******************************************************************************
;
; BOOL
; PortSetup(HPORT hPort, BYTE *RxBase, ulong RxLength, BYTE *TxBase,
;	    ulong TxLength);
;
; Parameters:
;		hPort	= port handle
;		RxBase	= receive queue base
;		RxLength = receive queue's length
;		TxBase	= base of transmit queue
;		TxLength = transmit queue length
;
; Returns:
;		TRUE if OK, else false.
;==============================================================================
EndDoc
BeginProc PortSetup, CCALL, esp, PUBLIC

ArgVar	hPort,DWORD
ArgVar	RxBase,DWORD
ArgVar	RxLength,DWORD
ArgVar	TxBase,DWORD
ArgVar	TxLength,DWORD

	EnterProc
	SaveReg	<edi>

	mov	edx,hPort		; esi -> PortInformation struct
	lea	edi,[edx.pData.QInCount]
	mov	ecx, (_PortData.ValidPortData - _PortData.QInCount)/4
	xor	eax,eax
	mov	[edx.pData.dwLastError],eax	; no error

	rep	stosd			; reset all counts

	.errnz	_PortData.QInGet-_PortData.QInCount-4
	.errnz	_PortData.QInPut-_PortData.QInGet-4
	.errnz	_PortData.QOutCount-_PortData.QInPut-4
	.errnz	_PortData.QOutGet - _PortData.QOutCount -4
	.errnz	_PortData.QOutPut - _PortData.QOutGet - 4
	.errnz	_PortData.ValidPortData - _PortData.QOutPut - 4

	mov	edi,edx			; EDI -> port information structure

	mov	eax,RxBase		; get base of receive queue
	or	eax,eax			; Q: external buffer ?
	jnz	PS_NoAllocRxBuffer	;  Y:
	test	[edi.MiscFlags],RxQInternal ; Q: already alloced internal buf ?
	jz	PS_AllocRxBuffer
	VMMCall	_HeapReallocate,<[edi.pData.QInAddr],RxLength,0>
	or	eax,eax
	jz	PS_Done
	jnz	PS_NoAllocRxBuffer

PS_AllocRxBuffer:
	VMMCall	_HeapAllocate,<RxLength,0>
	or	eax,eax
	jz	PS_Fail
	or	[edi.MiscFlags],RxQInternal

PS_NoAllocRxBuffer:
	mov	[edi.pData.QInAddr],eax
	mov	eax,RxLength		; get length
	mov	[edi.pData.QInSize],eax

	mov	eax,TxBase		; get base of transmit q
	or	eax,eax			; Q: external buffer ?
	jnz	PS_NoAllocTxBuffer	;  Y:
	cmp	TxLength,0		;  N: Q: Is desired length 0 ?
	jz	PS_XmitBufferEmpty	;     Y: No buffer.....

	test	[edi.MiscFlags],TxQInternal
	jz	PS_AllocTxBuffer
	VMMCall	_HeapReallocate,<[edi.pData.QOutAddr],TxLength,0>
	or	eax,eax
	jz	PS_Done
	jnz	PS_NoAllocTxBuffer

PS_AllocTxBuffer:
	VMMCall	_HeapAllocate,<TxLength,0>	; try allocation
	or	eax,eax				; Q: success ?
	jnz	PS_SetTxInternalFlag		;    Y: set flag and continue

PS_Fail:
	test	[edi.MiscFlags],RxQInternal
	jz	PS_Fail1
	and	[edi.MiscFlags],NOT RxQInternal
	VMMCall	_HeapFree,<[edi.pData.QInAddr],0>

PS_Fail1:
	xor	eax,eax
	jmp	PS_Done

PS_SetTxInternalFlag:
	or	[edi.MiscFlags],TxQInternal

PS_NoAllocTxBuffer:
	SetFlag	[edi.MiscFlags],TxQSet

PS_XmitBufferEmpty:
	mov	[edi.pData.QOutAddr],eax
	mov	eax,TxLength		; get length
	mov	[edi.pData.QOutSize],eax

	or	al,1

PS_Done:
	RestoreReg <edi>
	LeaveProc
	return

EndProc PortSetup

BeginDoc
;******************************************************************************
;
; BOOL
; PortTransmitChar(HPORT hPort, char cbyte);
;
; Parameters:
;		hPort	= port handle
;		cbyte	= char to transmit
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortTransmitChar, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	cbyte,BYTE

	EnterProc
	push	esi
	push	ebx

	mov	esi,hPort		; ESI -> Portinformation
	mov	eax,4000h		; in case we cannot send
	TestMem	[esi.EFlags],fTxImmed	; is another "Immediate" char waiting?
	jnz	PTC_Done		; yes, return error
	mov	ah,cbyte		; set char for TXI
	cli
	call	TXI			; set char to tx immediately
	sti
	xor	eax,eax			; ALL OK
PTC_Done:
	mov	[esi.pData.dwLastError],eax
	or	eax,eax			; all OK ?
	mov	eax,VMM_TRUE		; assume OK
	jz	PTC_Success
	xor	eax,eax			; NO

PTC_Success:
	pop	ebx
	pop	esi
	LeaveProc
	return

EndProc	PortTransmitChar

BeginDoc
;******************************************************************************
;
; BOOL
; PortClearError(HPORT hPort, COMSTAT *pComstat, ulong *pError);
;
; Parameters:
;		hPort	= port handle
;		pComstat -> status structure to fill in
;		pError	-> location to fill in error condition (reset)
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortClearError, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	pComstat,DWORD
ArgVar	pError,DWORD

	EnterProc
	push	esi
	push	ebx

	mov	esi,hPort
	mov	ebx,pComstat
	call	StaCom			; call worker

	xor	eax,eax
	mov	[esi.pData.dwLastError],eax
	xchg	eax,[esi.pData.dwCommError]	; return old error and clear
	mov	edx,pError
	mov	DWORD PTR [edx],eax		; return old error here
	or	al,1				; success

	pop	ebx
	pop	esi
	LeaveProc
	return

EndProc PortClearError

BeginProc	RestorePortState

	EnterProc
    ArgVar	hPort, DWORD
ifndef NEC_98
    
    push	esi
    mov		esi, hPort
    lea		esi, [esi.ComDcb]
    
	cCall	PortSetState,<hPort,esi,-1>	; call to set state   
    
    pop		esi    
endif ;NEC_98
    LeaveProc
    Return
    
EndProc		RestorePortState

BeginDoc
;******************************************************************************
;
; BOOL
; PortEscapeFunction(HPORT hPort, long function, long Indata, long Outdata);
;
; Parameters:
;		hPort	= port handle
;		function = escape code
;		Indata	= optional data
;		Outdata	= optional data
;
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortEscapeFunction, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	function,DWORD
ArgVar	Indata,DWORD
ArgVar	Outdata,DWORD

	EnterProc

	push	esi
	push	ebx

	mov	ebx,function
	mov	esi,hPort		; esi -> portinformation
	xor	eax,eax			; assume error
	mov	[esi.pData.dwLastError],IE_EXTINVALID
	cmp	bl, PEEKCHAR
	jne	@F
	mov	[esi.pData.dwLastError],eax
	cmp	[esi.pData.QInCount], eax
	je	PEF_Done
	mov	eax, [esi.pData.QInAddr]
	add	eax, [esi.pData.QInGet]
	mov	cl, [eax]
	mov	eax, OutData
	mov	BYTE PTR [eax], cl
	jmp	PEF_NoOutData
@@:
	cmp	bl,ENABLETIMERLOGIC
	jnz	@F
	ClrFlag	[esi.MiscFlags],ClrTimer
	jmp	PEF_NoOutData
@@:
	cmp	bl,IGNOREERRORONREADS
	jnz	@F
	SetFlag	[esi.MiscFlags],IgnoreCommError
	jmp	PEF_NoOutData
@@:
	cmp	bl,CLRTIMERLOGIC
	jnz	@F
	SetFlag	[esi.MiscFlags],ClrTimer
	jmp	PEF_NoOutData
@@:
	cmp	bl,SETUPDATETIMEADDR
	jne	@F
	mov	eax,InData
	mov	[esi.RxTimeAddr],eax
	jmp	PEF_NoOutData
@@:
	cmp	bl,CLEARBREAK
	ja	@f

	mov	[esi.pData.dwLastError],eax	; reset error
	call	ExtnFcn
	mov	ecx,OutData
	jecxz	PEF_NoOutData
	mov	[ecx],eax			; return this information
    jmp	PEF_NoOutData


@@:		
	cmp	bl, PM_QUERY_D1
    jb	PEF_Done
    
    cmp	bl, PM_DISARM_WAKEUP
    ja	PEF_Done
    
    sub	bl, PM_QUERY_D1
    mov	eax, OutData
    mov	DWORD PTR [eax], PM_SUCCESS
    xor	eax,eax    
    lea	ebx, [PM_Function_Dispatch+4*ebx]
    jmp	DWORD PTR [ebx]
    
pmD1query:  
pmD2query:	
pmD3query:
pmD1cancel:
pmD2cancel:
pmD3cancel:
    jmp	PEF_NoOutData
        
pmSetD0:	
	pushfd
    push	esi        
    cli
    call	RestorePortState            
    add		esp,4
    popfd
    
    xor		eax,eax
    jmp		PEF_NoOutData
        
pmSetD1:       
pmSetD2:       
pmSetD3:	   
    xor		eax,eax
    jmp	PEF_NoOutData
        
pmArmWakeup:       
pmDisArmWakeup:
    jmp	PEF_NoOutData
    
    
PEF_NoOutData:
	or	al,1				; success

PEF_Done:
	pop	ebx
	pop	esi
	LeaveProc
	return

EndProc PortEscapeFunction

BeginDoc
;******************************************************************************
;
; BOOL
; PortGetEventMask(HPORT hPort, long EventMask, long *OldEventMask);
;
; Parameters:
;		hPort	= port handle
;		EventMask = mask of events to clear
;		OldEventMask -> location where old mask should be returned.
;
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortGetEventMask, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	EventMask,DWORD
ArgVar	OldEventMask,DWORD

	EnterProc
	push	ebx

	mov	edx,hPort		; EDX -> PortInformation
	mov	[edx.pData.dwLastError],0 ; no errors here.
	mov	edx,[edx.AddrEvtDWord]	; edx -> event dword's location
	mov	ebx,EventMask		; get mask to clear
	not	ebx

	cli
	mov	eax,[edx]		; get current events
	and	ebx,eax			; clear them
	mov	dword ptr [edx],ebx	; save results
	sti
	mov	edx,OldEventMask
	mov	dword ptr [edx],eax	; return old accumulated events.

PGEM_Done:
	or	al,1			; success
	pop	ebx
	LeaveProc
	return

EndProc PortGetEventMask

BeginDoc
;******************************************************************************
;
; BOOL
; PortWrite(HPORT hPort, char *lpBuf, ulong NumBWrite, ulong *lpNumBWritten);
;
; Parameters:
;		hPort	= port handle
;		lpBuf	-> buffer to write from
;		NumBWrite = number of bytes to write
;		lpNumBWritten -> update location for number of bytes written
;
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortWrite, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	lpBuf,DWORD
ArgVar	NumBWrite,DWORD
ArgVar	lpNumBWritten,DWORD

	EnterProc
	push	esi
	push	edi
	push	ebx

	mov	esi,hPort
	mov	ecx,NumBWrite
	mov	edi,lpBuf
	TestMem	[esi.MiscFlags],TxQSet
	jnz	PW_QueueData
	cli
	mov	[esi.pData.QOutAddr],edi
	mov	[esi.pData.QOutSize],ecx
	mov	[esi.pData.QOutCount],ecx
	xor	eax,eax
	mov	[esi.pData.QOutPut],eax
	mov	[esi.pData.QOutGet],eax
	cmp	ecx,[esi.SendTrigger]	; Q: have we overshot the trigger ?
	jb	PW_StillBelowTrigger	;    N: don't start looking
	and	[esi.NotifyFlagsHI], NOT CN_TRANSMIT ; start looking now

PW_StillBelowTrigger:
	sti
	SaveReg	<ecx>			; save the #bytes written
	cli
	call	KickTx
	sti
	RestoreReg <eax>		; get back #bytes written
	jmp	PW_BytesQueued

PW_QueueData:
	push	esi
	call	WriteCommString
	pop	esi

PW_BytesQueued:
	mov	edx,lpNumBWritten
	mov	dword ptr [edx],eax	; # bytes written
	xor	eax,eax			; access to 0
	mov	[esi.pData.dwLastError],eax
	inc	eax			; success

	pop	ebx
	pop	edi
	pop	esi
	LeaveProc
	return

EndProc PortWrite

BeginDoc
;******************************************************************************
;
; BOOL
; PortRead(HPORT hPort, char *lpBuf, ulong NumBToRead, ulong *lpNumBRead);
;
; Parameters:
;		hPort	= port handle
;		lpBuf	-> buffer to read into
;		NumBToRead = number of bytes to read
;		lpNumBRead -> update location for number of bytes read.
;
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortRead, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	lpBuf,DWORD
ArgVar	NumBToRead,DWORD
ArgVar	lpNumBRead,DWORD

	EnterProc
	push	esi
	push	edi
	push	ebx

	mov	esi,hPort		; esi -> portinformation
	mov	edi,lpBuf		; edi -> buffer
	mov	ecx,NumBToRead		; ecx = max bytes to read
	call	ReadCommString
	mov	ecx,0			; assume no error
	jnz	PR_DataRead
	mov	ecx,eax			; save error/no data here
	xor	eax,eax
PR_DataRead:
	mov	edx,lpNumBRead
	mov	dword ptr [edx],eax
	mov	[esi.pData.dwLastError],ecx ; set error
	mov	eax,VMM_TRUE		; assume success
	jecxz	PR_Done
	xor	eax,eax
PR_Done:
	pop	ebx
	pop	edi
	pop	esi
	LeaveProc
	return

EndProc PortRead

;****
;
; ManageTimer
;
; If any COMM port is open and a receive trigger is set, then starts the
; timer is not already active, else stops it
;
; Entry:
;	NONE
; Exit:
;	NONE
; Uses:
;	Everything except EDI,EBX
;
BeginProc ManageTimer,PUBLIC

	mov	esi,[PortInfoHandle]
	VMMCall	List_Get_First

MT_Loop:
	jz	MT_Cancel_Timer		; exhausted the list...
	TestMem	[eax.MiscFlags],ClrTimer ; Q: Timer logic to be skipped ?
	jnz	MT_Next			;  Y:
	cmp	[eax.RecvTrigger],-1	; Q: Owner wants notification?
	jne	MT_SetTimer		;  Y: Check if
MT_Next:
	VMMCall	List_Get_Next
	jnz	MT_Loop

MT_Cancel_Timer:
	xor	esi,esi
	xchg	esi,TimeOutHandle
	VMMCall	Cancel_Time_Out
	jmp	MT_Done

MT_SetTimer:
	cmp	TimeOutHandle,0		; Q: Have we set a timer already?
	jnz	MT_Done			;  Y: get out
	mov	eax,100			;  N: set a 100 ms timer
	mov	edx,esi			; List handle is ref data
	mov	esi,OFFSET32 TimerProc
	VMMCall	Set_Global_Time_Out
	mov	TimeOutHandle,esi
	jmp	MT_Done

MT_Done:
	ret

EndProc ManageTimer

BeginDoc
;******************************************************************************
;
; BOOL
; PortPurge(HPORT hPort, DWORD dwQueue);
;
; Parameters:
;		hPort	= port handle
;		dwQueue = queue type
;
; Returns:
;		TRUE if OK, else FALSE.
;==============================================================================
EndDoc
BeginProc PortPurge, CCALL, PUBLIC

ArgVar	hPort,DWORD
ArgVar	dwQueue,DWORD

	EnterProc
	push	esi
	push	ebx

	mov	esi,hPort
	mov	ebx,dwQueue
	call	Flush
PP_Done:
	xor	eax,eax				; access to 0
	mov	[esi.pData.dwLastError],eax
	inc	eax				; success

	pop	ebx
	pop	esi
	LeaveProc
	return

EndProc PortPurge

IFDEF	NEC_98
IFDEF	WATCH_MODEM_LINE_STATUS
;******************************************************************************
;
; StatusWatchTimer
;
; Description:
;	Checks modem status and line status.
;	8251 has no modem/line status interrupt. So, Timer routine will check
;	them.
;
; Entry:
;	EDX -> PortInfoHandle
; Exit:
;	None
; Uses:
;	ALL
;==============================================================================

BeginProc StatusWatchTimer, PUBLIC

	mov	[hTimeOutStatus],0	; reset timer handle

	mov	esi,edx			; ESI -> PortinfoHandle
	VMMCall	List_Get_First

TP_Lp:
	jz	TP_Done			; end of list

	push	eax
	push	esi
	mov	esi,eax			; ESI -> PortInformation struct
	call	LineStat		; Update Line status
	call	ModemStatus		; Update Modem status
	pop	esi
	pop	eax

	VMMCall	List_Get_Next
	jmp	TP_Lp

TP_Done:
	mov	eax,10
	mov	edx,esi			; RefData = PortInfoHandle
	mov	esi,OFFSET32 StatusWatchTimer
	VMMCall	Set_Global_Time_Out
	mov	[hTimeOutStatus],esi	; save timer handle

	ret

EndProc StatusWatchTimer

ENDIF	;WATCH_MODEM_LINE_STATUS
ENDIF	;NEC_98

VxD_Locked_Code_Ends

	end
