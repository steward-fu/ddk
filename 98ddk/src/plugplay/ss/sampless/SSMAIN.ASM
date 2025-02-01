PAGE 58,132
;******************************************************************************
TITLE SSMAIN.ASM - Sample Socket Services Wrapper
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1996
;
;   Title:	SSMAIN.ASM - Sample Socket Services Wrapper
;
;==============================================================================
;
;   DESCRIPTION:
;
;******************************************************************************

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE vmm.inc
	INCLUDE pccard.inc
	INCLUDE ssdefs.inc
	INCLUDE pcicss.inc
	.LIST


FnTableEntry macro name
VXD_PAGEABLE_CODE_SEG
        extrn &name:near
VXD_PAGEABLE_CODE_ENDS
        dd      OFFSET32 &name
endm


VXD_LOCKED_CODE_SEG
        extrn _SSAckInt:near
VXD_LOCKED_CODE_ENDS

VXD_PAGEABLE_CODE_SEG
        extrn _SSGetVendorInfo:near
VXD_PAGEABLE_CODE_ENDS


VXD_LOCKED_DATA_SEG

SSFN_BASE       equ     SSFN_GET_SS_INFO
SSFnTable label DWORD
        FnTableEntry _SSGetSSInfo               ;83h
        FnTableEntry _SSInqAdapter              ;84h
        FnTableEntry _SSGetAdapter              ;85h
        FnTableEntry _SSSetAdapter              ;86h
        FnTableEntry _SSInqWindow               ;87h
        FnTableEntry _SSGetWindow               ;88h
        FnTableEntry _SSSetWindow               ;89h
        FnTableEntry _SSGetPage                 ;8Ah
        FnTableEntry _SSSetPage                 ;8Bh
        FnTableEntry _SSInqSocket               ;8Ch
        FnTableEntry _SSGetSocket               ;8Dh
        FnTableEntry _SSSetSocket               ;8Eh
        FnTableEntry _SSGetStatus               ;8Fh
        FnTableEntry _SSResetSocket             ;90h

SSFN_TABLE_ENTRIES      equ     (($ - SSFnTable)/4)
SSFN_END                equ     (SSFN_BASE + SSFN_TABLE_ENTRIES)

VXD_LOCKED_DATA_ENDS


;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device SAMPLESS, 1, 0, \
		       SSControl, UNDEFINED_DEVICE_ID, UNDEFINED_INIT_ORDER


VXD_LOCKED_CODE_SEG

;******************************************************************************
;
;   SSControl
;
;   DESCRIPTION:
;	Control procedure for socket services driver.
;
;   ENTRY:
;	EAX = Control call ID
;
;   EXIT:
;	If carry clear then
;	    Successful
;	else
;	    Control call failed
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc SSControl

	Control_Dispatch PNP_NEW_DEVNODE, SSNewDevnode, sCall, <ebx, edx>
        Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, SSInit, sCall
	clc					; Ignore other control calls
	ret

EndProc SSControl


;******************************************************************************
;
;   SSEntry - Socket Services Entry Point
;
;   ENTRY:
;	AH  =  Socket service number
;	ESI -> Adapter handle
;	Other registers unique to service being called
;
;   EXIT:
;	AH  =  Status
;	Carry flag set if error, else clear and AH = 0
;
;   USES:
;	Can modify EAX, EBX, ECX, EDX, ESI, EDI, and flags as specified for
;	the given function.
;
;==============================================================================

BeginProc SSEntry

        pushfd
	pushad
	mov	ebp, esp			; Set up param frame

	cmp	ah, SSFN_ACK_INTERRUPT
        jne     SSEChkHandle
;
;   Special code for Acknowledge_Interrupt handling.  We allow this function
;   to be called even when the busy status is set.
;
        cCall   _SSAckInt, <esi, ebp>
        jmp     short SSENoResetBusy

SSEChkHandle:
        cmp     [esi.dwSig], SIG_MYSS           ;Valid signature?
        je      SSEChkReentry

        mov     [ebp.r_ah], SSERR_BAD_ADAPTER
SSEEarlyError:
        or      [ebp.r_eflags], EFLAGS_CARRY
        jmp     short SSENoResetBusy

SSEChkReentry:
        bts     [esi.dwfAdapter], AF_BUSY_BITPOS;Have we been reentered?
        jnc     SSEChkFnLo                      ;  N: continue

        mov     [ebp.r_ah], SSERR_BUSY
        jmp     short SSEEarlyError

SSEChkFnLo:
        sub     ah, SSFN_BASE                   ;ah < our first function code ?
        jae     SSEChkFnHi

;   Bad function number passed.
;
SSEBadFn:
        mov     [ebp.r_ah], SSERR_BAD_SERVICE
        or      [ebp.r_eflags], EFLAGS_CARRY
        jmp     short SSEDone

SSEChkFnHi:
        cmp     ah, (SSFN_END - SSFN_BASE)      ;ah > our last function code ?
        jbe     SSECallFn

        cmp     ah, (SSFN_GET_VENDOR_INFO - SSFN_BASE)
        jne     SSEBadFn

        cCall   _SSGetVendorInfo, <esi, ebp>
        jmp     short SSEDone

SSECallFn:
        movzx   eax, ah
        mov     eax, SSFnTable[eax*4]           ;EAX -> Function to call
        cCall   eax, <esi, ebp>                 ;Call the handler

SSEDone:
        and     DWORD PTR [esi.dwfAdapter], NOT AF_BUSY
SSENoResetBusy:
        popad
        popfd
        ret

EndProc SSEntry

VXD_LOCKED_CODE_ENDS

VXD_PNP_CODE_SEG

;***LP  RegWithCS - Register with card services
;
;   This is the first start call for this adapter.  Regsiter with card
;   services.
;
;   ENTRY
;       pAdapter -> ADAPTER structure
;
;   EXIT-SUCCESS
;       returns 0
;   EXIT-FAILURE
;       returns Card Services Error code

BeginProc RegWithCS,CCALL
        ArgVar pAdapter,DWORD

        EnterProc

        push    esi
        push    edi
        push    ebx

        .ERRNZ  SIZE AddSocketServices_s - 6    ; Reserve 2 dwords of stack
        sub     esp, 8
        mov     ebx, esp

        mov     esi, pAdapter
        mov     [ebx.AddSocketServices_s.Attributes], SSA_FLAT_PROTECT_MODE
        mov     [ebx.AddSocketServices_s.DataPointer], esi

        lea     esi, SSEntry
        xor     edx, edx
        mov     ecx, SIZE AddSocketServices_s
        mov     eax, F_ADD_SOCKET_SERVICES
        VxDcall PCCARD_Card_Services

        add     esp, 8
        pop     ebx
        pop     edi
        pop     esi
        LeaveProc
        return

EndProc RegWithCS

VXD_PNP_CODE_ENDS

END
