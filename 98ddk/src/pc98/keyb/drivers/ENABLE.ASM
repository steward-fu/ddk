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

        page    ,132

include keyboard.inc
include vkoem.inc
include vkwin.inc
include lcid.inc

include int31.inc

if1
%out
%out ...........
%out ENABLE.ASM - (Multilingual Chicago)
%out ...........
    ifdef ENVOY
        %out .. With HP Vectra support
    endif
endif

if1
ifdef NEWNMI
%out .. NMI save in EnableKBSysReq()
else
%out .. NMI save only in Enable()
endif
endif

;***************************************************************************
; Some Olivetti M24x /AT&T 6300x keyboard definitions.
;***************************************************************************

; Double byte range values for the Far East.
; The values defined here are for the Rest Of the World.
; These values are for the inquireData (KBINFO) structure defined below.
; ('KeyInfo' in the Kernel, pState in USER)
;
ifdef	NEC_98
BeginRange1     equ     129	; 81h
EndRange1       equ     159	; 9Fh
BeginRange2     equ     224	; E0h
EndRange2       equ     252	; FCh
else	;NEC_98
BeginRange1     equ     255
EndRange1       equ     254
BeginRange2     equ     255
EndRange2       equ     254
endif	;NEC_98

OFFSEL          struc
off     dw      ?
sel     dw      ?
OFFSEL          ends

externFP	InquireEx
ExternFP        LoadTheWorld            ; for initialisation


;***************************************************************************
; DATA segment -- data declarations and local data
;***************************************************************************

sBegin  DATA

assumes DS,DATA

; DATA segment variables accessed in KbInit() below


; Data to specify keyboard type and system type, in the keyboard table
; module.  Some of these values are loaded with the table, others are
; computed in the INIT code in this module.

ifndef	NEC_98
        extrn   TableType:byte          ; Table type loaded (1,2,3,4)

        extrn   KeyType:byte            ; Computed keyboard type (1,2,3,4,5,6)

        extrn   OliType:byte            ; NZ if Olivetti-protocol keyboard,
                                        ; identifies keyboard type. Also
                                        ; for AT&T.
                                        ; For PCType values, see OLIKBD.INC
        extrn   fKeyType:byte           ; if RT, copy of KeyType

        extrn   PCType:byte             ; needed for Nokia

endif	;NEC_98
        EXTRN   event_proc:DWORD        ; in TRAP.ASM (user's keyboard handler)

ifdef JAPAN
        extrn   DBEEnable:byte          ; VK_DBE Key generation switch
        extrn   IniConv:byte, IniSize:byte, IniRoma:byte
        extrn   IniCode:byte
        extrn   KeyboardType:byte, KeyboardSubType:word
endif
ifdef KOREA
        extrn   KeyboardType:byte, KeyboardSubType:word
endif

public fTables                          ; public for debug
fTables         db      0               ; Call has been made to NewTable

public fSysReq
fSysReq         db      0               ; Enables CTRL-ALT-SysReq if NZ

; Keyboard information block (copied to 'KeyInfo' in the kernel)
; this is a KBINFO data structure.. defined in KERNEL.INC, USER.INC, USER.H
; and WINDEFS.INC.
;
; As of 3.0, build 1.30, KBINFO includes the number of function keys
; As of 3.0, build 1.59, the number of bytes in the state block is
; fixed at 4 MAX, for compatibility with Excel 2.1c!!!
;
                PUBLIC  inquireData
                PUBLIC  iqdNumFunc
inquireData     LABEL   BYTE
                DB      BeginRange1
                DB      EndRange1
                DB      BeginRange2
                DB      EndRange2
                DW      4               ; #bytes of state info for ToAscii()
iqdNumFunc      label   word
                dw      10              ; number of function keys

; flag for OS/2 to allow/prevent screen switches
; The ScreenSwitchEnable function, called from the display driver,
; sets/resets this flag. A nonzero value indicates that switching
; is enabled.
;
;       extrn   fSwitchEnable:byte
;


kflags          db      0ffh    ; prev. value in [kb_flag]
                                ; For handling of Oli M24 keyboard lights
globalB old_enhanced, 0         ; save value at 40h:96h (enhanced kbd) here..
globalB enabled, 0              ; layouts/driver enabled yet?


ifdef   ENVOY
    if1
    %out ... Has VECTRA support
    endif
;-------------------------------------------------------------
;
; Envoy data area
;
include equate.inc      ; HP-System equates

FV_NOEXBIOS     = 0
FV_A            = 001b
FV_NOA          = 010b
FV_ENVOY        = 100b

WF_PMODE        equ     01


        PUBLIC fVectra

staticB  fVectra, 0     ; 1 if Vectra. Set by Enable



; This is used to store the old HP-HIL keyboard translators
;
OldSoftkeyTrans dw      ?                                               ;9/23/86
OldCCPTrans     dw      ?                                               ;9/23/86

;SCANDOOR State 0 and State 2 saved here                ;11/02/87 VVR
;
ScanState0      db      ?                               ;11/02/87
ScanState2      db      ?                               ;11/02/87 VVR

RealMode_Word_Struc     Real_Mode_Call_Struc    <>

else
    if1
    %out ... Does NOT have VECTRA support
    endif

endif   ;ENVOY


ExternD bios_proc           ; these are in DS for ROM
ExternD nmi_vector
ifdef	NEC_98

;; Define NEC Original Data
;;  Data and Macro

        EXTRN   khelpf:BYTE
        EXTRN   helpf:BYTE
        EXTRN   VirtKeysHELP:BYTE
        EXTRN   homef:BYTE
        EXTRN   VirtKeysCLR:BYTE
        EXTRN   stopf:BYTE
        EXTRN   VirtKeysSTOP:BYTE
        EXTRN   autof:BYTE

externA WinFlags

WF_ENHANCED	equ	20h

extrn	__A000:ABS
extrn   __E000:ABS
extrn   __F000:ABS

externW RepMode
externW kbd_proc

szSystemIni     db      "system.ini",0
szKeyboard      db      "keyboard",0
szHelpKey       db      "helpkey",0
szKanaHelp      db      "kanahelpkey",0
szClearKey      db      "clrkey",0
szStopKey       db      "stopkey",0
szAutoRepeet    db      "autorepeet",0
szHome          db      "Home",0
szEnd           db      "End",0
szPause         db      "Pause",0
szYes           db      "Yes",0
szNo            db      "No",0
szOn            db      "On",0
szInterval      db      "Interval",0
szNull          db      0
szBuffer        db      16 dup (0)

ChkKeyboardIni  macro   KeyName, DefVal, ChkVal
        push    ds
        push    offset szKeyboard
        push    ds
        push    offset KeyName
        push    ds
        push    offset DefVal
        push    ds
        push    offset szBuffer
        push    15
        push    ds
        push    offset szSystemIni
        cCall   GetPrivateProfileString

        xor     ah, ah
        xor     bx, bx
@@:
        mov     al, ChkVal[bx]
        or      al, al
        jz      @f
        xor     al, szBuffer[bx]
        or      ah, al
	inc	bx
        jmp     short @b
@@:
        and     ah, 0ffh-20h
                endm

        extrn   PC98Type:byte
        extrn   NPC_FLAG:byte
        extrn   XL_FLAG:byte

public funckey
funckey         dw      0

public hTimer
hTimer          dw      0               ; Timer handle

selText         dw      0

KbdCmd          dw      0

endif	;NEC_98

sEnd


;***************************************************************************
;***************************************************************************
;***************************************************************************
;       Resident code for this module
;***************************************************************************
;***************************************************************************
;***************************************************************************

externFP        NewTable                                ;in TABS
externFP        LoadOemAnsiXlat                         ;Tabs.asm [damianf]
externFP        GetKeyboardInfo                         ;Tabs.asm [damianf]

ifdef	NEC_98
externFP        CreateSystemTimer
externFP        KillSystemTimer
externFP        GetPrivateProfileString

externFP        keybd_int2

externFP        break_int
externNP        break_proc
externFP        repeat_int
externNP        repeat_proc
endif	;NEC_98


sBegin  CODE        ; Beginning of code segment
assumes CS,CODE
assumes DS,DATA

.386

; Light handler in ToAscii.asm

    extrn SetLightHardware: near

; keyboard interrupt trap in TRAP.ASM

;*****************************************************************************
;**************************** ScreenSwitchEnable *****************************
;*****************************************************************************
;
;   This function is called by the display driver to inform the keyboard
;   driver that the display driver is in a critical section and to ignore
;   all OS/2 screen switches until the display driver leaves its
;   critical section.  The fEnable parameter is set to 0 to disable
;   screen switches, and a NONZERO value to re-enable them.  At startup,
;   screen switches are enabled.

cProc	ScreenSwitchEnable,<PUBLIC,FAR>
;ParmW	fEnable

cBegin nogen

retf 2

cEnd nogen


;*****************************************************************************
;****************************** EnableKBSysReq *******************************
;*****************************************************************************
;
;   This function enables and shuttles off NMI interrupt simulation
;   (trap to debugger) when CTRL-ALT-SysReq is pressed.
;   CVWBreak overides int 2.
;   fSysParm    = 01    enable  int 2
;               = 02    disable int 2
;               = 04    enable  CVWBreak
;               = 08    disable CVWBreak
;
cProc   EnableKBSysReq,<PUBLIC,FAR, LOADDS>
ParmW   fSysParm

cBegin  EnableKBSysReq

        mov     ax, fSysParm            ; get WORD parameter

        test    al,01                   ; turn on int 2?
        jz      @F
        or      fSysReq,01              ; yes, turn it on!
@@:     test    al,02                   ; turn off int 2?
        jz      @F
        and     fSysReq,NOT 01          ; yes, turn it off!

@@:     test    al,04                   ; turn on CVWBreak?
        jz      @F
        or      fSysReq,02              ; yes, turn it on!
@@:     test    al,08                   ; turn off CVWBreak?
        jz      @F
        and     fSysReq,NOT 02          ; yes, turn it off!
@@:
        xor     ah,ah
        mov     al,fSysReq
ifdef NEWNMI
        push    ax                      ; save return value
        call    short GetNmi            ; save NMI
        pop     ax                      ; restore ..
endif

cEnd    EnableKBSysReq

; save NMI vector.  Called above in EnableKBSysReq() and in Enable().

ifdef NEWNMI
GetNmi proc near
        mov     ax,3502h
        int     21h
        mov     word ptr ds:[nmi_vector][0],bx
        mov     word ptr ds:[nmi_vector][2],es
        ret
GetNmi endp
endif

;*****************************************************************************
;******************************* Envoy Macros ********************************
;*****************************************************************************
ifdef   ENVOY

;------------------------------------------------------------------
;
; RSysCal       Driver, AXReg
;
; Purpose       General purpose HP system calling routine
;
; Parameters    Driver  which will be stored in BP
;               AX value
;
; Results       returns AH which is 0 for success
;                                   2 for unsupported
;
;-------------------------------------------------------------------
RSysCal macro   device, AXReg
        mov     ax, device
        push    ax
        mov     ax, AXReg
        call    HPSysCall
        endm

cProc   HPSysCall,<NEAR>, <ds,bp>
        parmW   Device

cBegin  HPSysCall
        mov     bp, Device
        int     HPENTRY                 ; Status in AH

cEnd    HPSysCall

;------------------------------------------------------------------
;
; RSysCalPM     Driver, AXReg
;
; Purpose       General purpose HP system calling routine 
;               protected mode
;
; Parameters    Driver  which will be stored in BP
;               AX value
;
; Results       returns AH which is 0 for success
;                                   2 for unsupported
;
;-------------------------------------------------------------------
RSysCalPM       macro   device, AXReg

        push    es
        push    di
        mov     di, offset RealMode_Word_Struc
        mov     RealMode_BP[di], Device
        mov     RealMode_AX[di], AXReg
        mov     RealMode_BX[di], bx
        mov     ax, ds
        mov     es, ax                          ; make es = ds
        mov     bl, 6fh
        xor     bh, bh
        xor     cx, cx
        mov     ax, 0300h
        int     31h
        mov     ax, es:RealMode_AX[di]
        mov     bx, es:RealMode_BX[di]
        pop     di
        pop     es
        endm

endif   ;ENVOY


;*****************************************************************************
;********************************** Enable ***********************************
;*****************************************************************************
;
; Enable( eventProc ) - enable hardware keyboard interrupts, with the passed
; procedure address being the target of all keyboard events.
;
; lpKeyState is a long pointer to the Windows 256 byte keystate table
;

cProc   Enable,<LOADDS, PUBLIC,FAR>,<si,di>
        ParmD   eventProc
        ParmD   lpKeyState

cBegin  
ifdef	NEC_98
	EnterCrit
;;---------------------------------------------------
;;	Calling Machine Check Routine was deleted.
;;---------------------------------------------------

endif	;NEC_98
        les     bx,eventProc
        mov     ax,es
        mov     WORD PTR [event_proc],bx
        mov     WORD PTR [event_proc+2],es

; Initialize shift-key bytes in keyboard state vector to correspond to
; BIOS's flags in 40:17h.
; ES:DI points to movable memory, so no allocs allowed up to the point
; InitKeyState() is called.

; We do this EVERY time Enable is called, since we may be coming back from
; a full-screen old app.

ifndef	NEC_98
;;	Since PC-9800 driver resets its keyboard when Enabling,
;;	processes as follows are needless. 
;;
        les     di,lpKeyState
        call    InitKeyState
endif	;NEC_98

;
; All done if just reenabling a different event proc
;
        cmp     [enabled],0
        jnz     EnableExit

	call	LoadTheWorld			; load em up!

ifdef JAPAN
;  Initialize VK_DBE_ state if MakeIMEVKey=yes
        cmp     [DBEEnable],0                   ; DBE enable?
        jz      @F                              ; jump if not
        les     di,lpKeyState
        call    InitDBEKeyState                 ; go get DBE initializer
@@:
endif
        
ifndef	NEC_98
;
; Set lights properly
;
        call    SetLightHardware
endif	;NEC_98

    public EnableExit   ; for debug
EnableExit:

ifdef	NEC_98
	cmp	NPC_FLAG, 1
	jne	no_npc03
	LeaveCrit_np
	jmp	npc30
no_npc03:
	LeaveCrit
npc30:
endif	;NEC_98

cEnd    

;*****************************************************************************
;******************************* InitKeyState ********************************
;*****************************************************************************
;
; The point of this routine is to make the shift, control, alt,
; numlock, capslock or shiftlock, and scrollock bits in KeyState
; correspond to the ROM BIOS's key state bits when Windows starts.
;
; We assume that Windows has first cleared its key state vector to
; all 0's.
;
; NOTE:  the sign bit of each byte in KeyState indicates up/down,
;  and bit 0 is toggled on each depression.
; The entries in KeyState have already been initialized to 0's by
; Windows.
;
; NOTE: When this routine is called, the keyboard type may NOT have been
; determined yet.
;
; This is called with ES:DI => key state vector

ifndef	NEC_98
;;	Since PC-9800 driver resets its keyboard when Enabling,
;;	processes as follows are needless. 
;;
public InitKeyState 
InitKeyState proc near

        ; Get BIOS shift state byte directly from RAM (40H:17H) instead
        ; of via int 16H call.

        push    es                      ; remember, we need ES:DI
        ; access RAM directly
        mov     si,RAMBIOS              ; segment 40H or selector for it.
        mov     es,si
        mov     al,byte ptr es:[kb_flag] ; [AL] = BIOS keystate (40H:17H)
        pop     es

        ; the binary masks
        ; The shift (first 3) flags need their STATE (80H) bit initialized:
        ; so we put 80h into DL to load into the state vector.

ifdef MIKESCH
        mov     dl,02h                  ; load STATE bit
else
	mov	dl,80h
endif
        mov     ah,fShift               ; init. VK_SHIFT byte in KeyState
        mov     bx,VK_SHIFT             ; note -- THIS CLEARS BH!
        call    SetKeyState
        mov     ah,fCtrl                ; init. VK_CONTROL byte in KeyState
        mov     bl,VK_CONTROL
        call    SetKeyState
        mov     ah,fALt                 ; init. VK_MENU byte in KeyState
        mov     bl,VK_MENU              ; 
        call    SetKeyState             ;

        ; The CAPSLOCK, SCROLL and NUMLOCK flags need their TOGGLE
        ; bits initialized, so put 1 in DL to load into the state vector.
        mov     dl,1                    ; load TOGGLE bit
        mov     ah,fScroll
        mov     bl,VK_OEM_SCROLL
        call    SetKeyState
        mov     ah,fNum
        mov     bl,VK_NUMLOCK
        call    SetKeyState
        mov     ah,fCaps                ; BIOS CAPS LOCK/SHIFT LOCK bit?
        mov     bl,VK_CAPITAL
ifdef JAPAN
        call    SetKeyState
        mov     dl,80h                  ; load STATE bit
        ; this is a AX specific BIOS call.
        xor     ax, ax
        cmp byte ptr [KeyboardSubType], KbdAX
        jnz     @F
        mov     ah,51h                  ; get shift state into AL
        int     16h
@@:
        mov     al,fKana                ; kana shift returns in AH
        mov     bl,VK_KANA
comment !
        call    SetKeyState

        mov     dl,80h                  ; load STATE bit
        mov     ax,-1                   ; force to set state bit
        mov     bl,VK_DBE_ALPHANUMERIC  ; init. VK_DBE_ALPHANUMERIC in KeyState
        call    SetKeyState
        mov     bl,VK_DBE_ROMAN         ; init. VK_DBE_ROMAN in KeyState
        call    SetKeyState
        mov     bl,VK_DBE_DBCSCHAR      ; init. VK_DBE_DBCSCHAR in KeyState
!
endif ; JAPAN

endif	;not NEC_98

public SetKeyState, sks1

SetKeyState:
ifdef MIKESCH
        ; BX determines which byte in the key state vector to modify.
        ; DL is the new value for this byte
        ; (AL & AH) determines whether to copy the byte: AL contains
        ;    the BIOS shift state.
	;
	; BX is destroyed here!
	; CL is destroyed here!
	;
	mov	cl, bl
	shr	bx, 3			; go down to the bit range
	and	cl, 3
	add	cl, cl			; cl x2, shifting two bits
endif
        test    al,ah
        jz      sks1
ifdef MIKESCH
	shl	dl, cl			; move bits up
	or	es:[di+bx], dl		; SET rgbKeyState[bx]
else
	mov	es:[di+bx],dl
endif
        ret
sks1:
ifdef MIKESCH
	mov	dl, 0FCH
	shl	dl, cl
	and	byte ptr es:[di+bx],dl ; CLEAR rgbKeyState[bx]
else
	mov	byte ptr es:[di+bx],0
endif
        ret

ifndef	NEC_98
InitKeyState endp
endif	;NEC_98

ifdef JAPAN
public InitDBEKeyState
InitDBEKeyState proc near
        mov     dl,80h                  ; load STATE bit
        mov     ax,-1                   ; force to set state bit
;
        xor     bx,bx                   ; clear hibyte - Fix 29, jan - yutakan
;
        mov     bl, [IniConv]           ; initial conv. mode (Hira/Kata/Alpha)
        call    SetKeyState
        mov     bl, [IniSize]           ; initial input mode (Zenkaku/Hankaku)
        call    SetKeyState
        mov     bl, [IniRoma]           ; initial mode (Roma-ji/Hyoujyunn)
        call    SetKeyState
        mov     bl, [IniCode]           ; initial codeinput mode
        call    SetKeyState
        ret
InitDBEKeyState endp
endif


;*****************************************************************************
;******************************** ExchParent *********************************
;*****************************************************************************

ifdef   ENVOY
;
; ExchParent( Device, NewParent )
;
; Purpose       Replace the parent of the specified device.
;
; Parameter     Device - the specifed device vector
;               NewParent - the new parent vector
;
; Result        The previous parent
;

cProc   ExchParent, <NEAR, PUBLIC,LOADDS>, <BP,SI,DI,DS,ES,CX>
        ParmW   Device
        ParmW   NewParent
cBegin  ExchParent

	mov	ax, ds
	mov	es, ax				; make es = ds
	mov	bx, offset RealMode_Word_Struc
	mov	RealMode_BP[bx], V_SYSTEM
	mov	RealMode_AH[bx], F_INS_BASEHPVT
	mov	di, bx
	mov	bl, 6fh			; calling int 6f through int 31h
	xor	bh, bh
	xor	cx, cx
	mov	ax, 0300h
	int	31h			;es:di points to RealMode_Word_struc
	mov	ax, es
	mov	ds, ax			; make it point to RealMode_Word_Str
	mov	bx, di
	mov	bx, RealMode_ES[bx]	; now get real mode ES
	mov	ax, 0002h		; convert Segment to Selector
	int	31h			; do the conversion

	mov     si, Device
	mov	es, ax			; ax (es) has the selector
	mov	bx, es:[si+4]		; get segment into bx
	mov	ax, 0002h		; convert it again to access segment
	int	31h			; do the conversion
	mov 	es, ax			; use the di that is passed 
	mov 	ax, NewParent		; now prepare to exchange parents
	xchg	ax, es:[DH_V_PARENT]

cEnd    ExchParent

endif   ;ENVOY


;*****************************************************************************
;********************************* Disable ***********************************
;*****************************************************************************
;
; Disable( eventProc ) - disable hardware keyboard interrupts, restoring
; the previous IBM BIOS keyboard interrupt handler.
;

cProc	Disable,<PUBLIC,FAR>,<si,di>

cBegin  Disable
        ;
        ; Do nothing if not enabled
        ;
        cmp     [enabled],0
        jz      Dis_Done

;
; Wild stuff !! If RT keyboard has been detected, we put back
; the Enhanced keyboard flag at 40:96h, whatever it was.
;

ifndef	NEC_98
        cmp     [KeyType], 4                    ; is it RT keyboard ?
        jne     dis10                           ; ... if not, skip
                                                ; ... else is RT
        mov     bl, old_enhanced                ; get saved value for 40:96h
        push    ds
        mov     ax, RAMBIOS
        mov     ds, ax
        assumes ds,RAMBIOS
        mov     byte ptr[KB_type], bl           ; restore Enhanced flag
        pop     ds
        assumes ds,DATA
dis10:
endif	;NEC_98

ifdef	NEC_98
        EnterCrit
        mov     ax, hTimer
        or      ax,ax                   ;Was the timer created?
        je      @f                      ;  No, cannot kill it
        cCall   KillSystemTimer,<ax>
@@:
        cmp     RepMode, 3                      ; no autorepeet ?
        jne     @f                              ; Yes.
        call    far ptr ena_rep
@@:
endif	;NEC_98
        ;
        ; Restore the keyboard interrupt vector to point to previous value
        ;
        mov     ax, 2500h or vector
        push    ds
        lds     dx,[bios_proc]
        int     21h
ifdef	NEC_98
	pop	ds
else	;NEC_98
        mov     ax,RAMBIOS
        mov     ds,ax
        assumes ds,RAMBIOS
        and     WORD PTR [kb_flag],0FCFFh       ; clear left ctrl+left alt
        pop     ds
        assumes ds,DATA
endif	;NEC_98

ifdef   ENVOY
;!!!-------------------------------------------------------------
;
; if Envoy keyboard, restore the old keyboard translators which are
;  saved in OldKeyTrans. This is first done using the EX-BIOS and
;  then done by adjusting the translators device headers directly.
;

        test    [fVectra], FV_ENVOY
        jnz     kd_envoy
        jmp     kd_not_envoy
kd_envoy:

; Using EX-BIOS set the keyboard the translators to their default
;
        mov     ax, F16_SET_TRANSLATORS         ; Map V_CCP to V_CCPCUR
        mov     bl, 0
        int     16h

        mov     ax, F16_SET_TRANSLATORS         ; Map V_SOFTKEY to V_SKEY2FKEY
        mov     bl, 5
        int     16h

; Now restore the saved translators
;

        mov     si, V_SPCCP                     ; Restore cursor pad    ;9/23/86
        mov     ax, OldCCPTrans
        cCall   ExchParent, <si, ax>

        mov     si, V_PSOFTKEY                  ; Restore softkeys
        mov     ax, OldSoftkeyTrans
        cCall   ExchParent, <si, ax>                                    ;9/23/86
        
        jmp     kbd_en_end_hp

kd_not_envoy:
; Restore the state of Scandor. If V_SCANDOOR is not present
; these calls will have no effect.
;

        mov     bl, 0                   ; State 0
        mov     bh, ScanState0
        RSysCalPM V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>
        mov     bl, 2                   ; State 2
        mov     bh, ScanState2
        RSysCalPM V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>

kbd_en_end_hp:

endif   ;ENVOY

ifdef	NEC_98
        mov     ah, 3
        int     18h
        mov     [enabled],0
        LeaveCrit
else	;NEC_98
        mov     [enabled],0
endif	;NEC_98

Dis_done:
cEnd    Disable

ifdef	NEC_98
;***************************************************************************
;
;
;***************************************************************************
        public  helpchk
helpchk proc far
        push    ax
        push    bx
        push    cx
        push    dx
        push    es
        ChkKeyboardIni  szHelpKey, szNull, szEnd
        mov     byte ptr helpf, 00h             ; HELP = vk_help
        mov     byte ptr VirtKeysHELP, VK_HELP  ; 
        jnz     @f
        mov     byte ptr helpf, 0ffh            ; HELP = vk_end
        mov     byte ptr VirtKeysHELP, VK_END   ; 
@@:
        ChkKeyboardIni  szKanaHelp, szYes, szNo
        mov     byte ptr khelpf, 00h            ; Kana+HELP is Un-Reversible
        jz      @f
        mov     byte ptr khelpf, 0ffh           ; Kana+HELP is Reversible
@@:
        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret
helpchk endp

;***************************************************************************
;
;
;***************************************************************************
        public  homechk
homechk proc far
        push    ax
        push    bx
        push    cx
        push    dx
        push    es
        cmp     XL_FLAG, 0                      ; XL2 or XL(H Mode) ?
        jnz     short @f                        ;   yes

        ChkKeyboardIni  szClearKey, szNull, szHome
        mov     byte ptr homef, 00h             ; HOME/CLR = VK_CLEAR
        mov     byte ptr VirtKeysCLR, VK_CLEAR  ; 
        jnz     @f
        mov     byte ptr homef, 0ffh            ; HOME/CLR = VK_HOME
        mov     byte ptr VirtKeysCLR, VK_HOME   ; 
@@:
        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret
homechk endp


;***************************************************************************
;
;
;***************************************************************************
        public  stopchk
stopchk proc far
        push    ax
        push    bx
        push    cx
        push    dx
        push    es

        ChkKeyboardIni  szStopKey, szNull, szPause
        mov     byte ptr stopf, 00h             ; STOP = VK_CANCEL
        mov     byte ptr VirtKeysSTOP, VK_CANCEL; 
        jnz     @f
        mov     byte ptr stopf, 0ffh            ; STOP = VK_PAUSE
        mov     byte ptr VirtKeysSTOP, VK_PAUSE ; 

        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret
stopchk endp


;***************************************************************************
;
;
;***************************************************************************
        public  autochk
autochk proc far
        push    ax
        push    bx
        push    cx
        push    dx
        push    es
        ChkKeyboardIni  szAutoRepeet, szNull, szInterval	;^ Use New autorepeat
	mov	ax, 3						;| check "INTERVAL"
	jz	autochk_set					;v
        ChkKeyboardIni  szAutoRepeet, szNull, szOn		;^ Use Old autorepeat( win3.0 3.1 compatible)
	mov	ax, 2						;| check "ON"
	jz	autochk_set					;v
								;^ OTHER
								;| Use PC-9800 Series H/W autorepeat
	mov	ax, 1						;v
autochk_set:
        mov     RepMode, ax
        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret
autochk endp

;***********************************************************
;
;--- keyboard hardware control -----------------------
;
;***********************************************************
wait5f  macro   delay_o, delay_j
	mov	cx, delay_o
@@:	out	5fh, al
	loop	@b
	jmp	short $+2
        endm

;************************************************
; input
;	ah = command
;
; output
;	none
;
; break
;	
;************************************************
        public cmd_out
cmd_out proc near

; Is available any data?
	push	ax
        wait5f  13h, 80h
CO_Try_again:
	in	al, 43h
	test	al, 02h			; Does KB have a Data ?
	jz	CO_Empty		;  N: continue
					;  Y: try to process for KB inpput
        mov     al,kb_nretry_cmd
        out     kb_ctl,al		; I'm ready for NEXT DATA
	in	aL, 41H			;  Get KB Data
	call	keybd_int2
        wait5f  13h, 80h
	jmp	CO_Try_Again		; Try Again

CO_Empty:
	pop	ax

        mov     dl, 4
cmd_out_retry:
        wait5f  24a0h, 0ffffh

	mov	al, 37h
	out	43h, al
	rept 8
	jmp	short $+2
	endm
	mov	al, ah
	out	41h, al

        wait5f  0ah, 40h

@@:	in	al, 43h
	test	al, 01h
	jz	@b
	mov	al, 16h
	out	43h, al

        wait5f  140h, 800h

cmd_out_wait:
        wait5f  13h, 80h

	mov	cx, 8000h

@@:	jmp	short $+2
	in	al, 43h
	test	al, 02h
	jnz	@f
	loop	@b
	jmp     cmd_out_err
@@:
        wait5f  13h, 80h

	in	al, 41h

	cmp	al, 0fah
	je	cmd_out_ack
	cmp	al, 0fch
	je	cmd_out_retry
	call	keybd_int2
	dec	dl
	jnz	cmd_out_wait

cmd_out_err:
        stc                     ; Time out
        ret

cmd_out_ack:
        clc                     ; OK !
        ret

cmd_out	endp


;************************************************
; input
;	KbdCmd[0][1] = Command1,2
;
; output
;	none
;
; break
;	flags, ax, cx, dl
;************************************************
        public kbd_cmd
kbd_cmd proc near
	mov	ax, WinFlags
	test	ax, WF_ENHANCED
	jz	kbd_cmd_std

	mov	al, byte ptr KbdCmd[0]
	out	41h, al
	mov	al, byte ptr KbdCmd[1]
	out	41h, al
	ret

kbd_cmd_std:
	in	al, 02h
	push	ax
	or	al, 02h
	out	02h, al

	mov	ah, byte ptr KbdCmd[0]
        call    cmd_out
        jc      @f
	mov	ah, byte ptr KbdCmd[1]
        call    cmd_out
@@:
	in	al, 02h
	pop	ax
	out	02h, al
	ret

kbd_cmd	endp


;************************************************
; input
;	bl = LED Status
;	es = RAMBIOS
;
; output
;	none
;
; break
;	flags, ax, cx, dl
;************************************************
        public set_led
set_led proc far

        mov     ax, 709dh
        or      ah, bl
        mov     KbdCmd, ax
	call	setCMOS
        call    kbd_cmd
	ret

set_led endp


;************************************************
; input
;	none
;
; output
;	none
;
; break
;	flags, ax, cx, dl
;************************************************
        public dis_rep
dis_rep proc far

        mov     KbdCmd, 709ch
        call    kbd_cmd
	ret

dis_rep endp

        public ena_rep
ena_rep proc far

        mov     KbdCmd, 519ch
        call    kbd_cmd
	ret

ena_rep endp

        public chg_rep
chg_rep proc far

        mov     al, 9ch         ; ah is rate of repeat
        mov     KbdCmd, ax
        call    kbd_cmd
	ret

chg_rep endp


ifdef	NEC_WINKEY
;************************************************
; input
;	none
;
; output
;	none
;
; break
;	flags, ax, cx, dl
;************************************************
        public ena_winkey
ena_winkey proc far

        mov     KbdCmd, 0395h
        call    kbd_cmd
	ret

ena_winkey endp
endif	;;NEC_WINKEY



;**************************************************************
;       CMOS set routine
;
;Break  ax, bx, cl, flag
;**************************************************************
	public setCMOS
setCMOS proc near

        push    es
        test    byte ptr es:[081h], 48h         ; check keyboard type
        jz      RetCMOS                         ;    Old Keyboard : Skip

        mov     ax, selText                     ; 
        or      ax, ax                          ; check selector of text VRAM
        jnz     IsSelText                       ;    text VRAM selector
        test    byte ptr es:[101h], 08h         ; check display mode
        jz      @f                              ;    N Mode
        mov     ax, __E000                      ; H mode T-VRAM
        jmp     short SetSelText
@@:
        mov     ax, __A000                      ; N mdoe T-VRAM
SetSelText:                                     ; 
        mov     selText, ax                     ; 
IsSelText:                                      ; 

        mov     bx, ax                          ; selector of Text VRAM
        mov     ah, es:[kb_flag]                ; 
        mov     al, 0dh                         ; 
        out     68h, al                         ; 
        mov     al, es:[101h]                   ; check display mode (N/H)
        mov     es, bx                          ; ds:TextVRAM
        test    al, 08h                         ; 
        jnz     setHmode                        ; 
        mov     cl, 5                           ; 
        shl     ax, cl                          ; 
        and     ah, 0c0h                        ; 
        mov     bx, 3ff6h
        and     byte ptr es:[bx], not 0c0h      ; set CMOS for CAPS/KANA lock
        or      byte ptr es:[bx], ah            ; 
        jmp     short setExit                   ; 
setHmode:                                       ; 
        mov     bx, 3feah
        test    ah, 02h                         ; check KANA lock
        jz      KANAoff                         ;    off
        or      byte ptr es:[bx], 20h           ; KANA lock
        jmp     short setCAPS                   ; 
KANAoff:                                        ; 
        and     byte ptr es:[bx], not 20h       ; KANA unlock
setCAPS:                                        ; 
        mov     bx, 3ffah
        test    ah, 04h                         ; check CAPS lock
        jz      CAPSoff                         ;    off
        or      byte ptr es:[bx], 80h           ; CAPS lock
        jmp     short setExit                   ; 
CAPSoff:                                        ; 
        and     byte ptr es:[bx], not 80h       ; CAPS unlock
setExit:                                        ; 
        mov     al, 0ch                         ; 
        out     68h, al                         ; 
RetCMOS:                                        ; 
        pop     es                              ; 
        ret                                     ; 

setCMOS endp

endif	;NEC_98

.286p
sEnd CODE

;***************************************************************************
;       int GetKbdType(Which)
;
;       if Which == 0, returns keyboard type (1..6)
;
;       if Which == 1, returns keyboard subtype ("OliType")
;
;       if Which == 2, returns number of function keys
;
;       otherwise returns 0.
;
;***************************************************************************

createSeg _GETTYPE, GETTYPE, BYTE, PUBLIC, CODE
sBegin  GETTYPE
assumes CS,GETTYPE
assumes DS,DATA

.386
cProc GetKeyboardType,<LOADDS, PUBLIC,FAR>,<si,di>

    ParmB Which

cBegin GetKeyboardType

        xor     ax,ax                           ; clear return value
        cmp     Which,0
        jne     GetKbd1
ifdef JAPAN
        mov     al,KeyboardType
else
   ifdef KOREA
        mov     al,KeyboardType
   else
        mov     al,KeyType                      ; type= from SYSTEM.INI
   endif
endif
        jmp     short GetKbdExit
GetKbd1:
        cmp     Which,1
        jne     GetKbd2
ifdef JAPAN
        mov     ax,KeyboardSubType
elseifdef KOREA
        mov     ax,KeyboardSubType
else
        mov     al,OliType                      ; subtype= from SYSTEM.INI
endif
        jmp     short GetKbdExit
GetKbd2:
        cmp     Which,2
        jne     GetKbdExit
        mov     ax,iqdNumFunc                   ; number of function keys

GetKbdExit:

cEnd GetKeyboardType

.286p
sEnd GETTYPE

if2
%out .. end ENABLE.ASM
%out
endif

END
