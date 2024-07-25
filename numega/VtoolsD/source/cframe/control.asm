;;		Control.asm
;;
;;	Control dispatcher for C Framework
;;
;;	Copyright (c) 1994 by Compuware Corporation
;;
;;
	.386p

	.XLIST
	NO_SEGMENTS=1
	INCLUDE	VMM.INC
	INCLUDE VSEGMENT.INC
	INCLUDE CDEFNS.INC
	.LIST

	PUBLIC _LocalControlDispatcher
	PUBLIC _LocalPMhandler
	PUBLIC _LocalV86handler

;----------------------------------------------------------------
VxD_IDATA_SEG

INCLUDE	vsig.inc

VxD_IDATA_ENDS

;----------------------------------------------------------------
VxD_LOCKED_DATA_SEG


	EXTERN C The_DDB		: NEAR
	EXTERN C VXD_SERVICE_TABLE_SIZE	: DWORD

bInitialized	dd	0		; bit 0 set when control
					; dispatcher initializes
VxD_LOCKED_DATA_ENDS

;----------------------------------------------------------------
VxD_LOCKED_CODE_SEG

	EXTCFUNC ControlDispatcher	: NEAR
	EXTCFUNC PM_Api_Handler		: NEAR
	EXTCFUNC V86_Api_Handler	: NEAR

	ASSUME cs:FLAT, ds:FLAT, es:nothing, fs:nothing, gs:nothing

;;	LocalInit
;;
;;	Initialization done at SYS_CRITICAL_INIT time
;;	for VtoolsD 'C' framework
;;
;; Entry:
;;	ebx	VM handle
;; Exit:
;;	ebx	VM handle
;; Uses:
;;	eax, edx, esi, edi
;;

LocalInit:

;; Initialize the size of static tables that C preprocessor
;; won't fill in for us...

;; VXD Service Table size in the DDB

	mov	edx, OFFSET32 The_DDB
	mov	eax, VXD_SERVICE_TABLE_SIZE
	mov	[edx].DDB_Service_Table_Size, eax

	jmp LocalInitReturn		;; finished

;;		LocalPMhandler
;;
;; This is the procedure which is called when a PM application
;; requests VxD services.  We call the user's API handler.  If
;; the user did not designate one, then the default handler from
;; the library will be called (which does nothing).
;;
BeginProc	_LocalPMhandler

	push	ebp
	push	ebx

	call	PM_API_Handler

	pop	ebx
	pop	ebp
	ret

EndProc		_LocalPMhandler



;;		LocalV86handler
;;
;; This is the procedure which is called when a V86 application
;; requests VxD services.  We call the user's API handler.  If
;; the user did not designate one, then the default handler from
;; the library will be called (which does nothing).
;;
BeginProc	_LocalV86handler

	push	ebp
	push	ebx

	call	V86_API_Handler

	pop	ebx
	pop	ebp
	ret

EndProc		_LocalV86handler


;;		LocalControlDispatcher
;;
;; The VMM calls this procedure:
;;
;; Args:
;;	eax	System Control Message
;;	regs	Register arguments depend on control message (eax)
;; Ret:
;;	Carry flag may be set to indicate failure for some messages
;; Uses:
;;	all registers
;; Remarks:
;;	The control dispatcher calls the user-supplied 'C' routine
;;	ControlDispatcher(RegStruct)
;;
;;	The ControlDispatcher(RegStruct) routine returns TRUE or FALSE.
;;	If TRUE, then _LocalControlDispatcher returns carry clear to VMM.
;;	If FALSE, then _LocalControlDispatcher returns carry set to VMM.
;;
;;
BeginProc	_LocalControlDispatcher

	push	ecx
	push	edi
	push	esi
	push	edx
	push	ebx
	push	eax

	bts	bInitialized,0		;; test initialization state
	jnc	LocalInit		;; jump if not initialized

LocalInitReturn:

;; check for possible failure conditions from local initialization
;;
	cld				;; Ensure direction clear for C code
	call	ControlDispatcher	;; call C dispatcher

	pop	ebx			;; Calling EAX holds message number
IFDEF WIN40
	cmp	ebx, PNP_NEW_DEVNODE	;; new devnode needs value AND carry bit
	  je	Value_newdevnode
	cmp	ebx, W32_DEVICEIOCONTROL;; same treatment for W32_DEVICEIOCONTROL
	  je	Value_newdevnode
ENDIF	

	bt	eax, 0		;; store low bit into carry
	cmc				;; bool (1) means true, no carry

IFDEF WIN40
Return_newdevnode:
ENDIF
	pop	ebx			;;    (for W32_DEVICEIOCONTROL, PNP_NEWDEVNODE)
	pop	edx
	pop	esi
	pop	edi
	pop	ecx

	ret

IFDEF WIN40
Value_newdevnode:
	or	eax, eax		;; test for CR_SUCCESS (== 0)
	setne	bl			;; if not 0, bl := -1
	sar	bl, 1			;; set carry bit if not 0
	jmp	Return_newdevnode	;; return eax and carry bit
ENDIF

EndProc		_LocalControlDispatcher


VxD_LOCKED_CODE_ENDS


END

