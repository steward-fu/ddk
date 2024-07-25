;; VCTLUTIL.ASM - primary control dispatching and application interface
;=============================================================================
;
; Compuware Corporation
; NuMega Lab
; 9 Townsend West
; Nashua, NH 03060  USA
;
; Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
; Unpublished - rights reserved under the Copyright laws of the
; United States.
;
; U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
; U.S. Government is subject to restrictions as set forth in 
; Compuware Corporation license agreement and as provided in 
; DFARS 227.7202-1(a) and 227.7202-3(a) (1995), 
; DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995), 
; FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.  
; Compuware Corporation.
; 
; This product contains confidential information and trade secrets 
; of Compuware Corporation. Use, disclosure, or reproduction is 
; prohibited without the prior express written permission of Compuware 
; Corporation.
;
;=============================================================================

	.386P
	.xlist
	NO_SEGMENTS=1
	include VMM.INC
	include VSEGMENT.INC
	include CDEFNS.INC

;; This is the main module for dispatching control messages and handling
;; application entry points for VxDs that use the C++ framework. Entries
;; in the DDB point to routines in this module that are responsible for
;; passing register values as arguments to C functions, which in turn
;; pass them to member functions of the device class, vm class, and thread
;; class.

IFDEF WIN40
	WIN40SERVICES equ 1
ENDIF
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
VxD_IDATA_SEG
	include  VSIG.INC
_bStaticLoad	dd	0	
VxD_IDATA_ENDS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
VxD_ICODE_SEG
	extern INITCRT			: NEAR		;; C start-up code
VxD_ICODE_ENDS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
VxD_LOCKED_DATA_SEG

	EXTERN C TheDevice		: DWORD	;; device class instance
	EXTERN C VXD_SERVICE_TABLE	: NEAR	;; Exported services table
	EXTERN C VXD_SERVICE_TABLE_SIZE	: DWORD	;; Number of exp services
	EXTERN C The_DDB		: DWORD	;; DDB
	EXTERN C _ControlDispatchThunk	: DWORD

	PUBLIC	_VMCBdata
ifdef WIN40
	PUBLIC  _ThreadCBdata
endif

;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;
;; Static data

bInitInProgress		dd	0	;; LSB == 1 during initialization
_VMCBdata    		dd	0	;; offset in VMCB of pointer to
	     				;;    VM class instance
IFDEF WIN40
_ThreadCBdata		dd	0	;; offset in thread control block
ENDIF	     				;;    of pointer to thread class
     					;;    instance

InitSemaphore		dd  0	;; used to lock out during initialization
WaitCount			dd	0	;; used to count waiters at init semaphore

VxD_LOCKED_DATA_ENDS

PushadFrame	STRUC
    pf_EDI DD ?
    pf_ESI DD ?
    pf_RBP DD ?
    pf_ESP DD ?
    pf_EBX DD ?
    pf_EDX DD ?
    pf_ECX DD ?
    pf_EAX DD ?
PushadFrame	ENDS

PF_SIZE	= size PushadFrame

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
VxD_LOCKED_CODE_SEG
	EXTCFUNC _CreateDevice		
	EXTSFUNC _V86_API,8
	EXTSFUNC _PM_API,8

_DEV_	equ	0
_VM_	equ	1
_THD_	equ	2

DispatchTable LABEL BYTE
	db _DEV_ 	;; 00 SYS_CRITICAL_INIT
	db _DEV_ 	;; 01 DEVICE_INIT
	db _DEV_ 	;; 02 INIT_COMPLETE
	db _VM_		;; 03 SYS_VM_INIT
	db _VM_		;; 04 SYS_VM_TERMINATE
	db _DEV_	;; 05 SYSTEM_EXIT
	db _DEV_	;; 06 SYS_CRITICAL_EXIT
	db _DEV_	;; 07 CREATE_VM
	db _VM_		;; 08 VM_CRITICAL_INIT
	db _VM_		;; 09 VM_INIT
	db _VM_		;; 0A VM_TERMINATE
	db _VM_		;; 0B VM_NOT_EXECUTEABLE
	db _VM_		;; 0C DESTROY_VM
	db _VM_		;; 0D VM_SUSPEND
	db _VM_		;; 0E VM_RESUME
	db _VM_		;; 0F SET_DEVICE_FOCUS
	db _VM_		;; 10 BEGIN_MESSAGE_MODE
	db _VM_		;; 11 END_MESSAGE_MODE
	db _DEV_	;; 12 REBOOT_PROCESSOR
	db _VM_		;; 13 QUERY_DESTROY
	db _DEV_ 	;; 14 DEBUG_QUERY
	db _VM_		;; 15 BEGIN_PM_APP
	db _VM_		;; 16 END_PM_APP
	db _DEV_	;; 17 DEVICE_REBOOT_NOTIFY
	db _DEV_	;; 18 CRIT_REBOOT_NOTIFY
	db _VM_		;; 19 CLOSE_VM_NOTIFY
	db _DEV_	;; 1A POWER_EVENT
	db _DEV_	;; 1B SYS_DYNAMIC_DEVICE_INIT
	db _DEV_	;; 1C SYS_DYNAMIC_DEVICE_EXIT
ifdef WIN40		   
	db _DEV_	;; 1D CREATE_THREAD
	db _THD_	;; 1E THREAD_INIT
	db _THD_	;; 1F TERMINATE_THREAD
	db _THD_	;; 20 THREAD_Not_Executeable
	db _THD_	;; 21 DESTROY_THREAD
	db _DEV_	;; 22 PNP_NEW_DEVNODE
	db _DEV_	;; 23 W32_DEVICEIOCONTROL
	db _VM_		;; 24 SYS_VM_TERMINATE2
	db _DEV_	;; 25 SYSTEM_EXIT2
	db _DEV_	;; 26 SYS_CRITICAL_EXIT2
	db _VM_		;; 27 VM_TERMINATE2
	db _VM_		;; 28 VM_NOT_EXECUTEABLE2
	db _VM_		;; 29 DESTROY_VM2
	db _VM_		;; 2A VM_SUSPEND2
	db _VM_		;; 2B END_MESSAGE_MODE2
	db _VM_		;; 2C END_PM_APP2
	db _DEV_	;; 2D DEVICE_REBOOT_NOTIFY2
	db _DEV_	;; 2E CRIT_REBOOT_NOTIFY2
	db _VM_		;; 2F CLOSE_VM_NOTIFY2
	db _DEV_	;; 30 GET_CONTENTION_HANDLER
	db _DEV_	;; 31 KERNEL32_INITIALIZED
	db _DEV_	;; 32 KERNEL32_SHUTDOWN
	db _DEV_	;; 33 CREATE_PROCESS
	db _DEV_	;; 34 DESTROY_PROCESS
	db _DEV_	;; 35 SYS_DYNAMIC_DEVICE_REINIT
endif

ifdef WIN40
MAXMSG	equ	35h
else
MAXMSG	equ	1Ch
endif

ifdef __BORLANDC__
PassThisParam	MACRO thisptr
	push	thisptr
endm
else
PassThisParam	MACRO thisptr
	mov		ecx, thisptr
endm
endif

DispatchSelectTable	label DWORD	;; Order is important
	dd	DeviceDispatch
	dd	VMDispatch
ifdef WIN40
	dd	ThreadDispatch
endif

;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;
;; Control Message Dispatcher
;;
;; The DDB references this entry point as the control message handler for
;; the VxD. It passes register values to the C dispatcher on the stack.
;; This routine is also responsible for calling the C start-up on the
;; first time through.
;;

DEFSPROC localControlDispatcher <>
	push	eax
	pushad
	cld

	pushfd					;; are we initialized
	cli
	cmp	[TheDevice], 0
	je	doInit
	popfd

dispatch:
	cmp	eax, MAXMSG			;; Dispatch VxD specific messages
	ja	DeviceDispatch		;;  through the device class

	movzx	eax, byte ptr DispatchTable[eax]
	jmp	dword ptr DispatchSelectTable[eax*4]

ifdef WIN40
ThreadDispatch:
	mov	eax, [_ThreadCBdata]   	;; get offset in thread control block 
			   					;; of object pointer
	or	eax, eax				;; make sure it's valid
	jz	short noDispatch		;; bail out if not
	mov	eax, [eax+edi]			;; get address of VThread object
	or	eax, eax   				;; if zero, just leave 
	jz	short noDispatch		;; (OR clears carry)
	PassThisParam	eax	   		;; this
	jmp	short CallDispatcher
endif

VMDispatch:
	mov	eax, [_VMCBdata]   	;; get offset in control block of VM
					;; object pointer
	mov	eax, [eax+ebx]		;; get address of VM obj
	or	eax, eax		;; if zero, just leave 
	jz	short noDispatch	;; (OR clears carry)
	PassThisParam	eax			;; this
	jmp	short CallDispatcher

DeviceDispatch:
	PassThisParam	TheDevice		;; this

CallDispatcher:
	call	[_ControlDispatchThunk]

ifdef WIN40
	cmp	dword ptr [esp], PNP_NEW_DEVNODE 
	je	short ZeroSuccessReturn
	cmp	dword ptr [esp], W32_DEVICEIOCONTROL 
	je	short ZeroSuccessReturn
endif
	bt	eax, 0			;; test BOOL return from dispatcher
	cmc

dispatchExit:
	lea	esp, [esp+4]
	ret
noDispatch:
	popad
	pop	eax
	ret

ifdef WIN40
ZeroSuccessReturn:
	or	eax, eax		;; test for CR_SUCCESS (==0)
	setne	bl			;; if not 0, bl <- 1
	sar	bl, 1			;; set carry if not zero
	jmp	dispatchExit
endif


;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;
;; Perform once-only initialization tasks
;;
doInit:
	popfd
	pushad

;; Taking some precautions against race conditions. We'll allocate a
;; semaphore, then check to see if there is an initialization already
;; in progress. If not, just store the semaphore and proceed with the
;; initialization. If so, discard the semaphore we allocated, and
;; wait on the semaphore that was allocated by the initializing thread.

	xor		ecx, ecx			;; token count zero
	VMMCall	Create_Semaphore	;; semaphore handle ret'd in eax
	jc		InitDone		

	pushfd
	cli

	bts		bInitInProgress, 0	;; are we currently initializing
	jc		WaitForInit

	mov		[InitSemaphore],eax	;; save the sem handle
	popfd

	jmp		short doInitProceed

WaitForInit:
	inc		[WaitCount]
	popfd
	VMMcall	Destroy_Semaphore	;; discard semaphore just allocated
	mov		eax, [InitSemaphore];; get lock
	xor		ecx, ecx			;; flags=0
	VMMcall	Wait_Semaphore		;; wait for init to complete
	jmp		InitDone			;; and be on our way

doInitProceed:

ifdef WFW311
ifndef WIN40
	xor	eax, SYS_DYNAMIC_DEVICE_INIT
	mov	[_bStaticLoad],eax
endif
endif

ifdef WIN40
	mov	[_ThreadCBdata], 0
endif 

;; Set service count in DDB

	mov	eax, VXD_SERVICE_TABLE_SIZE
	mov	[The_DDB.DDB_Service_Table_Size], eax

ifdef WFW311
ifndef WIN40
	mov	[_VMCBdata], 0
    test [_bStaticLoad], -1
	jz	doCInit
endif
endif

	VMMcall _Allocate_Device_CB_Area, <4, 0>
	mov	[_VMCBdata], eax
	or	eax, eax
	jz	short InitDone
	
;; Initialize C/C++ run-time system
doCInit:
	ffcall	INITCRT	

;; Create instance of device class
	ffcall	_CreateDevice
	mov	[TheDevice], eax

InitDone:
	pushfd
	cli
	btr		[bInitInProgress], 0
	mov		esi, [WaitCount]
	popfd

FreeWaiters:	
	or		esi, esi
	jz		short FreeDone

	mov		eax, [InitSemaphore]
ifdef WIN40
	VMMcall	Signal_Semaphore_No_Switch
else
	VMMcall	Signal_Semaphore
endif
	dec		esi
	jmp		FreeWaiters
FreeDone:
	mov		eax, [InitSemaphore]
	VMMcall	Destroy_Semaphore	

	popad

ifdef WFW311
ifndef WIN40
	cmp	[_bStaticLoad],0	;; VMCBdata will be zero on dyn load
	je	short checkDevObject	;;   (which is OK)
endif
endif
	cmp	[_VMCBdata], 0		;; check on allocation in VM ctrl blk
	je	short ReturnCarry	;; if failed, fail the call
checkDevObject:
	cmp    	[TheDevice], 0		;; check if device object created ok
	jne	dispatch

ReturnCarry:
	stc
	jmp	dispatchExit

ENDSPROC localControlDispatcher 

;; InitThreadDataSlot
;; 
;; This routine is called by the constructor for VThread. It 
;; allocates a thread data slot, and zeros the slot in all existing
;; threads. It is coded to allocate only one TDS per VxD.
;;
;; The function returns TRUE if the thread data slot has been
;; allocated for this VxD. If the allocation fails, it returns
;; FALSE.
;;
DEFCPROC _InitThreadDataSlot_ <>
ifdef WIN40
	push	edi
	push	ebx

	cmp	[_ThreadCBdata], 0
	jne	ITDSok

	VMMcall _AllocateThreadDataSlot
	or	eax, eax
	jz	short ITDSdone
	mov	[_ThreadCBdata], eax

;; Zero thread data slot values for all threads
	VMMcall Get_Sys_VM_Handle
	VMMcall	Get_Initial_Thread_Handle
	mov	ebx, edi
	mov ecx, [_ThreadCBdata]
	xor	eax, eax
NextThread:
	mov	[edi+ecx], eax
	VMMcall	Get_Next_Thread_Handle
	cmp	ebx, edi
	jne	NextThread

ITDSok:
	mov	eax, 1

ITDSdone:
	pop	ebx
	pop	edi
endif
	ret
ENDCPROC _InitThreadDataSlot_

;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;
;; Primary V86 API entry
;;
;; Input:
;;	EBP				;; pointer to client register struct
;;	EBX				;; VMHANDLE of calling VM
;;
DEFSPROC v86_api_entry  <>
	pop	eax
	push	ebp			;; pass CLIENT_STRUCT*
	push	ebx			;; pass VMHANDLE
	push	eax
	jmp	_V86_API		;; call C dispatcher (_stdcall)
ENDSPROC v86_api_entry	

;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;  ;;
;; Primary V86 API entry
;;
;; Input:
;;	EBP				;; pointer to client register struct
;;	EBX				;; VMHANDLE of calling VM
;;
DEFSPROC pm_api_entry  <>
	pop	eax
	push	ebp			;; pass CLIENT_STRUCT*
	push	ebx			;; pass VMHANDLE
	push	eax
	jmp	_PM_API			;; call C dispatcher (_stdcall)
ENDSPROC pm_api_entry	


VxD_LOCKED_CODE_ENDS

        end
