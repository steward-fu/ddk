;----------------------------------------------------------------------------
; Copyright (c) 1995 S3 Inc.
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; ACCESS.ASM
; Begin and End framebuffer access functions.
;
; Copyright (c) 1994 Microsoft Corporation
;----------------------------------------------------------------------------
	.xlist
DOS5=1
	include cmacros.inc
	include dibeng.inc
	include	s3.inc
	include	macros.inc
	.list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
LA_GE	equ	2
ENGINE  equ     1               ; chip in Engine mode
MEMORY  equ     0               ; chip in Memory mode

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP	DIB_BeginAccess
	externFP	DIB_EndAccess

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
Align 2
	globalW	CurrentChipMode,ENGINE
	globalW	NestLevel,0
	globalW wMMIOActive,0	; low byte-> bMMIOActive
				; High byte->bMMIOActiveValue

	externB InSrcBlt		;Image Transfer in progress
	externW	wCursorType
        externB bLAGEOn
        externW wMMIOSelector
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
	.386

;----------------------------------------------------------------------------
; BeginAccess(PDevice,Left,Top,Right,Bottom,Flags)
;
; GRAB surface semaphore
;
; IF Flags == FB_ACCESS
;   INCREMENT NestLevel
;   IF CurrentMode <> memory mode
;     call SetModeMemory
;     CurrentMode = memory mode
; ENDIF
;
; IF Flags == CURSOREXCLUDE
;     PrevMode = CurrentMode
;     IF CurrentMode <> memory mode
;       call SetModeMemory
;     ENDIF
;     call DIB_ExcludeCursor(Left,Top,Right,Bottom)
;     IF PrevMode == engine mode
;       call SetEngineMode
;     ENDIF
;   ENDIF
; ENDIF
;
; RELEASE surface semaphore
; RETURN(Flags)
;
;----------------------------------------------------------------------------
Align 16
cProc	BeginAccess,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpDevice
	parmW	Left
	parmW	Top
	parmW	Right
	parmW	Bottom
	parmW	Flags
	localB	DeviceFlags
cBegin
	mov	ax,DGROUP
	mov	ds,ax
	push	fs
	assumes	ds,Data
	assumes	es,nothing
	assumes	fs,nothing
	assumes	gs,nothing
	les	si,lpDevice
	bts	es:[si].deFlags,BUSY_BIT	;Lock screen
	setc	DeviceFlags
;
BA_BiModalAccess:
	mov     ax,wMMIOSelector
        mov     es,ax
	WaitEngineIdle_Semaphore es ds
;
BBA_CheckCursor:
	test	Flags,CURSOREXCLUDE		;Want CursorExclusion?
	jz	BA_Done				;no.
	cmp	wCursorType,SOFTWARECURSOR	;Is the cursor software?
	jne	BA_Done				;no, exclusion not needed.
	push	lpDevice
	push	Left
	push	Top
	push	Right
	push	Bottom
	push	CURSOREXCLUDE			;Let the DIB Engine
	call	DIB_BeginAccess			; exclude the cursor.

PLABEL BA_Done
	test	DeviceFlags,1			;was the pdevice busy?
	jnz	short @f			;yes, leave it busy.
	les	si,lpDevice
	and	es:[si].deFlags,not BUSY
@@:	mov	ax,Flags
	pop	fs
cEnd

;----------------------------------------------------------------------------
; EndAccess(PDevice,Flags)
;
; GRAB surface semaphore
;
; IF Flags == FB_ACCESS
;   call SetModeEngine
; ENDIF
;
; IF Flags == CURSOREXCLUDE
;     call DIB_UnexcludeCursor
; ENDIF
;
; RELEASE surface semaphore
;
; RETURN(Flags)
;----------------------------------------------------------------------------
Align 16
cProc	EndAccess,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpDevice
	parmW	Flags
	localB	DeviceFlags
cBegin
	mov	ax,DGROUP
	mov	ds,ax
	push	fs
	assumes	ds,Data
	assumes	es,nothing
	assumes	fs,nothing
	assumes	gs,nothing
	les	si,lpDevice
	bts	es:[si].deFlags,BUSY_BIT
	setc	DeviceFlags

PLABEL EA_CheckCursorBit
	test	Flags,CURSOREXCLUDE		;Unexclude cursor?
	jz	short @f			;no.
	cmp	wCursorType,SOFTWARECURSOR	;Is the cursor software?
	jnz	short @f			;no, unexclusion not needed.
	push	lpDevice
	push	CURSOREXCLUDE
	call	DIB_EndAccess			;Let DIB Engine unexclude cursor.
@@:	test	DeviceFlags,1			;was the device busy?
	jnz	short @f			;yes, leave it busy.
	les	si,lpDevice
	and	es:[si].deFlags,not BUSY
@@:	mov	ax,Flags

PLABEL EA_Exit
	pop	fs
cEnd

ifdef DEBUG
PLABEL EA_BigTimeProblem
	int	3
	jmp	EA_Exit
endif


;----------------------------------------------------------------------------
; SetModeMemory
;   Waits for the S3 accelerator to become idle, then enables direct
;   access to the framebuffer.
;   NOTE:  This routine is not re-entrant and should be protected by
;   a semaphore.
;----------------------------------------------------------------------------
PPROC	SetModeMemory	near
	assumes	ds,Data
	assumes	es,nothing
	assumes	fs,nothing
	assumes	gs,nothing
mov	CurrentChipMode,LA_GE
        mov     ax,wMMIOSelector
        mov     fs,ax
        WaitEngineIdle fs
if	0
;
; Wait for the h/w to become idle.
;
        mov     ax,wMMIOSelector
        mov     fs,ax
        WaitEngineIdle fs
;
PLABEL SMM_RegMode
	mov     CurrentChipMode,MEMORY
endif
;
PLABEL SMM_Exit
	ret
SetModeMemory	endp

PPROC	SetModeMemory_far	far
	call	SetModeMemory
	ret
SetModeMemory_far	endp

;----------------------------------------------------------------------------
; SetModeEngine
;   NOTE:  This routine is not re-entrant and should be protected by
;   a semaphore.
;----------------------------------------------------------------------------
PPROC	SetModeEngine	near
	assumes	ds,Data
	assumes	es,nothing
	assumes	fs,nothing
	assumes	gs,nothing

	mov	byte ptr wMMIOActive,10h
	mov	byte ptr wMMIOActive+1,10h
	EnableMMIO
;
PLABEL SME_RegMode
	mov	CurrentChipMode,LA_GE
;	mov	CurrentChipMode,ENGINE
;
PLABEL SME_Exit
	ret
SetModeEngine	endp


PPROC	SetModeEngine_far	far
	call	SetModeEngine
	ret
SetModeEngine_far	endp


sEnd	Code
end
