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

ifdef ENHANCE
    TYPEERRORFLAG equ 1
endif; ENHANCE

ifdef ENVOY
    TYPEERRORFLAG equ 1
    ifdef ICO
        TYPEERRORFLAG equ 3
    endif
    ; also handle enhanced keyboard
    ENHANCE = 1
endif ; ENVOY

; ========================================================================
; display which option(s) we've chosen.

if1

%out
%out    .... TRAP.ASM -- Windows keyboard INT 09H ....
%out    .... Keyboards handled besides XT ....

    ifdef ENVOY
        %out .  Hewlett-Packard Vectra & Envoy version
    endif

    ifdef ENHANCE
        %out .  Handles Enhanced, XT, AT keyboards
    endif

endif   ; if1

; ========================================================================

ifdef	NEC_98
include	syssrc.inc
endif	;NEC_98
include keyboard.inc
include vkwin.inc
include vkoem.inc

;*;
;*; include file for LCID offsets
;*;
include lcid.inc

ifndef DosXMacro
    DosXMacro macro x, y

    endm
endif

; Allocate big stack when calling
; BIGSTK equ 0

ifdef	NEC_98
SYSREQ  =       060h    ; PC-AT SYS REQ key .. not on M24 or ICO keyboard
                        ; (on Enhanced keyboard, is PrintScn/SysReq)

else	;NEC_98
SYSREQ  =       054h    ; PC-AT SYS REQ key .. not on M24 or ICO keyboard
                        ; (on Enhanced keyboard, is PrintScn/SysReq)
endif	;NEC_98
if1
    ifdef SYSREQ
        %out .. Has SysReq handling
    else
        %out .. Does NOT have SysReq handling
    endif
endif


; scan codes for special keys
ifdef	NEC_98
cXfer           equ     035h
cHomeclr        equ     03eh
cHelp           equ     03fh
cNumeq          equ     04dh
cNfer           equ     051h
cStop           equ     060h
cCopy           equ     061h
cShift          equ     070h
cCaps           equ     071h
cGrph           equ     073h
cCtrl           equ     074h
cKana		equ	072h
else	;NEC_98
cEsc            EQU      1      ;
cReturn         EQU     28

cCtrl           EQU     29
cLShift         EQU     42
cSlash          EQU     53      ; 35h
cRShift         EQU     54      ; 36h
cPrint          EQU     55      ; 37h IBM keyboard printscreen
cAlt            EQU     56      ; 38h
ifdef KOREA
cSpace          EQU     57      ; 39h BeomOh's toggle key
endif
cCapsLock       EQU     58      ; 3ah
cF1             equ     59      ; 3bh F1 key on anything
cF7             equ     65      ; 41h F7 key on anything
cNumLock        EQU     69      ; 45h
cBreak          EQU     70      ; 46h
cUp             equ     72      ; 48h up key
cLeft           equ     75      ; 4bh left key
cRight          equ     77      ; 4dh right key
cDown           equ     80      ; 50h down key
cDelete         EQU     83

cExtended       EQU     96      ; E0h-80h - for RT extended codes - 13feb87
cExtended1      equ     97      ; E1h-80h - prefix for Pause key

cCommand        EQU     0F0H    
endif ; NEC_98

ifdef JAPAN
ifndef	NEC_98
; Japanese keyboard command codes.
cAck            EQU     0FAH    ;1 Acknowledge
cSelfError      EQU     0FCH    ;1 self test error
cResend         EQU     0FEH    ;1 resend command

;1 cNonConvert  equ     90      ; AX muhenkan key
cAxNonConvert   equ     5Ah     ;2 AX muhenkan key
cNo1            equ     29h     ;2 [101] Kanji, Hankaku/Zenkaku key
cAlphanumeric   equ     3Ah     ;1 IBM alphanumeric/CapsLock key
cKatakana       equ     70h     ;1 IBM katakana key (5576-1,2,3 type)
cConvert        equ     79h     ;1 IBM henkan key
cNonConvert     equ     7Bh     ;1 IBM muhenkan key
cNewHalfFull    equ     29h     ;2 IBM hankaku/zenkaku key (5576-A01)
cNewHiragana    equ     70h     ;2 IBM hiragana key (5576-A01)
cComma          equ     33h     ;1 IBM comma key on NumPad
cHalfFull       equ     77h     ;1 IBM hankaku/zenkaku key
cHiragana       equ     38h     ;1 IBM hiragana key
cCancel         equ     55h     ;1 IBM torikeshi key (5576-1)
cKanji          equ     68h     ;1 IBM kanji key (5576-1)
cTango          equ     67h     ;1 IBM tango-touroku key (5576-1)
ifdef TOSHIBA
cGrave		equ	29h	; GraveAccent key '`'
cBSlash		equ	56h	; BackSlash key '\'
cYen		equ	2Bh	; YenMark key '\'
c_M		equ	32h	; M key
cRShiftB        equ     0B6h    ; RShift break code
cBSlashB        equ     0D6h    ; RShift break code
endif
endif	;NEC_98
endif ; JAPAN


;*****************************************************************************
;*****************************************************************************
;************************ data segment begins here ***************************
;*****************************************************************************
;*****************************************************************************

sBegin DATA

ifndef	NEC_98

    externB fKeyType            ; flags mainly for RT keyboard.

;
; These things are updated at INIT or ENABLE time from the keyboard
; tables or from WIN.INI.
;
externB     PCType              ; identifies system type
externB     PCTypeHigh
externB     KeyType             ; keyboard type (usually == TableType)
externB     IsEri
externB     IsOli
externB     OliType
externB     NumLockFlag         ; Flag for change in BIOS NumLock flag.
endif	;NEC_98
externW ScanCodes
externW VirtKeys
externA KBD_SCANS
externA	SCAN_TO_VKEY_OFFS
externW	pCurrentLocale
ifdef TOSHIBA
externB	ScrlKana_flg
GlobalB kanamlock 0   
endif
ifdef	NEC_98
externB VirtKeysSTOP
externB VirtKeysCLR
externB VirtKeysHELP
endif	;NEC_98

; This is the translation for the numeric pad when NUMLOCK is set.
; This is the same for all keyboards, and is fixed in length.

        public  KeyNumTrans
KeyNumTrans     LABEL   BYTE
ifdef	NEC_98
        DB      VK_SUBTRACT     ; numpad-minus
        DB      VK_DIVIDE       ; numpad-devide
        DB      VK_NUMPAD7      ; numpad-7
        DB      VK_NUMPAD8      ; numpad-8
        DB      VK_NUMPAD9      ; numpad-9
        DB      VK_MULTIPLY     ; numpad-ast.
        DB      VK_NUMPAD4      ; numpad-4
        DB      VK_NUMPAD5      ; numpad-5
        DB      VK_NUMPAD6      ; numpad-6
        DB      VK_ADD          ; numpad-plus
        DB      VK_NUMPAD1      ; numpad-1
        DB      VK_NUMPAD2      ; numpad-2
        DB      VK_NUMPAD3      ; numpad-3
        DB      VK_OEM_EQU      ; numpad-equ.
        DB      VK_NUMPAD0      ; numpad-0
        DB      VK_SEPARATER    ; numpad-comma
        DB      VK_DECIMAL      ; numpad-period
else	;NEC_98
        DB      VK_NUMPAD7      ; numpad-7
        DB      VK_NUMPAD8      ; numpad-8
        DB      VK_NUMPAD9      ; numpad-9
        DB      VK_SUBTRACT     ; numpad-minus
        DB      VK_NUMPAD4      ; numpad-4
        DB      VK_NUMPAD5      ; numpad-5
        DB      VK_NUMPAD6      ; numpad-6
        DB      VK_ADD          ; numpad-plus
        DB      VK_NUMPAD1      ; numpad-1
        DB      VK_NUMPAD2      ; numpad-2
        DB      VK_NUMPAD3      ; numpad-3
        DB      VK_NUMPAD0      ; numpad-0
        DB      VK_DECIMAL      ; numpad-period
endif	;NEC_98

globalW keyNumBase <dataOffset KeyNumTrans>

ifndef	NEC_98
extrn   AckByte:byte                    ; Value of acknowledge byte
endif	;NEC_98
EXTRN   CVWBreak:FAR                    ; Kernel routine for CVW handling
externB fSysReq                         ; Flag for enabling SysReq key
ifdef   KOREA ; for TG Hangeul keyboard
fTGkeybd    db	0	; initial value = 0 = not TG keyboard
extrn   KeyboardSubType:word  
endif

ifdef JAPAN
ifndef	NEC_98
extern  gNew106:byte
endif	;NEC_98
extern  bKana:byte
        public bRoman, bKatakana, bHiragana, bZenHan, bCode, bKanjiDown
bKanaDown   DB  0       ; 0 if KANA up, 80 if Kana down
bKanjiDown  DB  0       ; for 002/003 keyboard
bRoman      DB  0       ; 0 if Roman, 80 if NoRoman
bKatakana   DB  0       ; 0 if Alpha, 80 if Katakana
bHiragana   DB  0       ; 0 if Alpha, 80 if Hiragana
bZenHan     DB  0       ; 0 if Zenkaku, 80 if Hankaku
bCode       DB  0       ; 0 if no Code input, 80 if Code input
ifdef	NEC_98
bXFER       DB  0       ;NEC 0 if make VK_DBE_xx, 1 if make VK_DBE_xx(not Toggle)
bNFER       DB  0       ;NEC                      other if make VK_DBE_xx
endif	;NEC_98
extrn   DBEEnable:byte          ; DBE Vkey generation switch
extrn   KeyboardSubType:word    ;2 1=101, 2=AX, 3=IBM 5576-A01
                                ;2 4=IBM 5576-2/3, 5=IBM 5576-1
bS3         DB  0
endif
;*****************************************************************************
;******************************** Local data *********************************
;*****************************************************************************
;
; Address of keyboard event-proc
;
; the keyboard event_proc is called with the following parameters:
;
;       ah == 80h for uptrans, 00h for downtrans
;       al == virtual key
;       bh == 0 if no prefix byte, 1 if E0 prefix byte preceded this scancode.
;       bl == scan code

        even

globalD event_proc, 0                   ; Addr of enabled kbd event procedure

ifdef BIGSTK
    if1
        %out HAS BIG LOCAL STACK
    endif
;
; Private keyboard stack (HP c-ralphp 6/9/88) see comments in keybd_int
;
staticB , ?, 384
globalW Stack, ?
staticW SaveSS, 0
staticW SaveSP, 0
staticW NextSS, 0

; end (HP c-ralphp modifications)
else
    if1
        %out .. DOES NOT HAVE BIG LOCAL STACK
    endif
endif ; BIGSTK

; (MS code has ShiftLockException table here)

fBreak      DB  0
fReEnter    DB  0       ; reentrancy flag

fAltGrDn    db  0       ; set if AltGr is down -- for preventing repeats.

LastCursor  db  0        ; last NUMERIC PAD cursor scancode read
                         ; with NumLock and SHIFT on.
NestedCount db  0        ; count nested interrupt occurances

Partial_faile0 db 0      ;count e0 failures.

Partial_faile1 db 0      ;count e1 failures.
                         
GlobalB ShiftCollision 0 ; count these if they occur.

; Address of routine called to handle a scan code.
; For non-RT keyboards, this never changes -- it's always 'standard_proc'.
; For RT keyboards, this changes when a prefix code is seen

ifdef	NEC_98
globalW kbd_proc,standard_proc
else	;NEC_98
staticW kbd_proc,standard_proc
endif	;NEC_98


fExtend db      0               ; flag for above keys.

globalD bios_proc, 0
globalD nmi_vector, 0

ifdef	NEC_98
        public  RepMode, RepCode, RepRate, RepDelay, RepCount, RepSpeed, RepTable
RepMode         dw      3       ; 1:H/W,  2:Break,  3:Timer
RepCode         dw      0ffffh  ; 0ffffh or scan code without make/break flag
RepRate         dw      2       ;  Change Default Repeat Rate 11/05/94 Takaaki
RepDelay        dw      10     ; Change Default Repeat Delay 11/05/94 Takaaki
RepCount        dw      0       ; Repeat Timer Counter
RepSpeed        dw      0ffffh  ; Value of SetSpeed
RepTable        db      11111111b, 11111111b, 11111111b, 11111111b
                db      11111111b, 11111111b, 11111111b, 11111111b
                db      11111111b, 11111111b, 00000011b, 01000000b
                db      00000011b, 00000000b, 00000000b, 00000000b

scr_flag        db      0       ; VK_OEM_SCROLL TAIOU
                                ; 00h = Make code ptr,0ffh = break code ptr
end_flag        db      0
home_flag       db      0
stop_flag       db      0
copy_flag       db      0ffh    ; when make VK_SNAPSHOT
                                ;       01h : bx = 1 ( current window )
                                ;       00h : bx = 0 ( whole screen )
                                ;       ffh : bx = cCopy ( other )

brkflag         db      0       ; Break flag

EMM_FLAG        DB      0
CHKEMM_FST      DB      0
EMM_DEVICE_NAME DB      "EMMXXXX0"

PUBLIC  khelpf
khelpf          db      0       ;KANA-HELP 00H = Un-Reversible
PUBLIC  helpf
helpf           db      0       ;HELP 00H = VK_HELP,0FFH = VK_END
PUBLIC  autof
autof           db      0       ;AUTOREPEET FLAG
PUBLIC  homef
homef           db      0       ;clr 00H = VK_CLR,0FFH = VK_HOME
PUBLIC  stopf
stopf           db      0       ;STOP 00H = VK_CANCEL,0FFH = VK_PAUSE

norepf          db      0

OldKey          db      0
NewKey          db      0
AutoRepKey      db      0
KBtime          dd      0
KBtime2         dd      0
oldkeybuf       dw      0
BreakKeyStatus  db      0       ; waiting flags
DoSysReq        db      0       ; 0:not done 1:do NMI 2:goto CVWBREAK
externFP        GetSystemMsecCount

ShiftSts3       db      01h,02h,04h,08h,10h,20h,40h,80h

        EXTRN   NPC_FLAG:BYTE
        EXTRN   XL_FLAG:BYTE

kb_retry        dw      0

ifdef	NEC_SHIFT
PUBLIC	KBD_LR_Shift
KBD_LR_Shift	db	0	;000000RL  R,L = On:1 Off:0
endif	;NEC_SHIFT

endif	;NEC_98

ifdef   ENVOY

;*****************************************************************************
;****************************** Envoy data area ******************************
;*****************************************************************************
;
include equate.inc      ; HP-System equates

; Envoy keyboard equates
;
cCCP_UP     EQU 60h
cf8         EQU 77h
cPlus       EQU 4Eh
cMinus      EQU 4Ah
cCCPDel     EQU 69h
cBackSlash  EQU 2Bh

; Flags for Hewlett-Packard
;
staticB  fModifier,0    ; -1 if modifier. Set by SetShiftState
FV_A            = 001b
FV_ENVOY        = 100b
externB  fVectra, 0     ; 1 if Vectra. Set by Enable


; This table is used to translate the function keys and cursor pad
; keys. This is now neccesary because they are now placed in a raw
; mode.
;
HPTransTable    label   byte
        DB      VK_UP                   ; CCP up arrow
        DB      VK_LEFT                 ; CCP left arrow
        DB      VK_DOWN                 ; CCP down arrow
        DB      VK_RIGHT                ; CCP right arrow
        DB      VK_HOME                 ; CCP home
        DB      VK_PRIOR                ; CCP PgUp
        DB      VK_END                  ; CCP end
        DB      VK_NEXT                 ; CCP PgDn
        DB      VK_INSERT               ; CCP Ins
        DB      VK_DELETE               ; CCP Del
        DB      VK_CLEAR                ; CCP Cntr
        DB      5 dup (-1)
        DB      VK_F1                   ; f1
        DB      VK_F2                   ; f2
        DB      VK_F3                   ; f3
        DB      VK_F4                   ; f4
        DB      VK_F5                   ; f5
        DB      VK_F6                   ; f6
        DB      VK_F7                   ; f7
        DB      VK_F8                   ; f8


; Reset Vector used for soft resets
;
ResetVector     dd      0ffff0000h

endif   ;ENVOY

sEnd DATA

ifdef	NEC_98
        externA WinFlags
        externA __A000
        externA __E000
        externNP   setCMOS
endif	;NEC_98


;*****************************************************************************
;*****************************************************************************
;******************************* CODE SEGMENT ********************************
;*****************************************************************************
;*****************************************************************************
;
;       Some pointer variables in the CODE segment.
;       These are initialized in ENABLE, using
;       a DATA alias to the CODE segment, so that
;       they can be written in protect mode.
;

sBegin CODE                             ; CODE segment now.

    assumes     CS,CODE
    assumes     DS,DATA

;*****************************************************************************
;******************************* CVWHackCall *********************************
;*****************************************************************************

    hack_for_old_cvw        dw  offset CVWHackCall
                            dw  seg CVWHackCall

cProc   CVWHackCall, <FAR,PUBLIC>
cBegin <nogen>

    push    ds                          ; save DS
    push    ax                          ; ... AX
    mov     ax, _DATA                   ; reload DS
    mov     ds, ax
    assumes ds, data
    pop     ax                          ; restore AX
    push    word ptr [bios_proc][2]
    push    word ptr [bios_proc][0]     ; put address of bios proc on stack
    push    bp
    mov     bp, sp
    mov     ds, [bp+6]                  ; restore ds
    pop     bp
    retf    2                           ; "return" popping ds
                                        ; affecting a jump to bios
cEnd   <nogen>


;*****************************************************************************
;****************************** GetBIOSKeyProc *******************************
;*****************************************************************************

cProc   GetBIOSKeyProc, <FAR, PUBLIC, PUBLIC, LOADDS>
cBegin
    mov     ax, word ptr [bios_proc][0]
    mov     dx, word ptr [bios_proc][2]
cEnd

    assumes     DS,nothing


;*****************************************************************************
;************************ Kbd Hardware service routine ***********************
;*****************************************************************************
;
;       Usage of segment registers:
;
;       Currently:              In 2.10 and earlier drivers:
;
;       CS = CODE               (was DATA)
;       DS = DATA               (was BIOS data area at 40h)
;       ES = RAMBIOS (40h)      (wasn't used)
;
ifdef	NEC_98
public keybd_int
keybd_int   PROC    FAR
        push    ds
        push    ax                      ; save registers we use
        push    bx
        push    cx
        push    dx
        push    es

        mov     ax, _DATA               ; What Cmacros does to 'DATA'
        mov     ds,ax                   ;  .. load DS
        mov     ax, RAMBIOS             ; BIOS RAM segment at 40h
        mov     es,ax                   ;  .. point ES to that!
    assumes DS, DATA
    assumes ES, RAMBIOS

        in      al,kb_ctl

        delaynp 8251,I-O
        test    al,38h
        jz      kb_int

        cmp     kb_retry,3
        je      kb_int
kb_int_again:
        inc     kb_retry
        mov     al,kb_retry_cmd
        out     kb_ctl,al
        delaynp 8251,I-O
        in      al,kb_data
        jmp     key_again

        ; Recover scan code and translate to virtual key code
kb_int:
        mov     kb_retry,0
        cmp     ds:[fReEnter],0         ; are we alone?
        jnz     kb_int_again

        mov     al,kb_nretry_cmd
        out     kb_ctl,al

        mov     cx,46
        test    byte ptr es:[0080h],00000010b   ; 02h = 386 mode ?
        jz      delay1                          ; no
        mov     cx,100
delay1:
        loop    $                               ; wait 37 usec from int

        in      al, kb_data             ; get key code

        push    ax                      ; this is for PC-9800 systems
        mov     al,eoi                  ; acknowledge interrupt
        out     ack_port,al
        pop     ax

        mov     ah,80h                  ; move the high order bit to the high
        and     ah,al                   ; order byte.
        xor     al,ah                   ; turn off bit if set
        inc     ds:[fReEnter]
        call    ds:[kbd_proc]   ; standard_proc or repeat_proc or break_proc

key_again:

        push    ax                      ; this is for PC-9800 systems
        mov     al,eoi                  ; acknowledge interrupt
        out     ack_port,al
        pop     ax

        cmp     DoSysReq, 0
        je      short notsys            ;

        ; now determine if we are to call off to int 2 or kernel!

        mov     freenter,0
        cmp     DoSysReq, 1
        mov     DoSysReq, 0

        ; now we simulate the NMI
        ; The code pointer nmi_vector must be in the CODE segment, since
        ; we want to restore ALL the other registers to what they were
        ; when the INT 09 interrupt happened, when we emulate the NMI.
        ; The Enable() function sets up nmi_vector.

        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
	
        jnz     @F

        push   word ptr nmi_vector[2]
        push   word ptr nmi_vector[0]  ; address of nmi_vector on stack
        push   bp
        mov    bp, sp                   ;NEC
        mov    ds, [bp+6]              ; restore ds
        pop    bp
        retf   2                       ; "return" to nmi_vector & pop ds

@@:
     pop     ds
        jmp     CVWBreak                ; go to KERNEL!!
notsys:

        cli
        mov     ds:[fReEnter],0         ; unlock keyboard

        Public ignorekey
ignorekey:
        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        pop     ds
        iret
keybd_int       ENDP


; This routine call from kbd_cmd in order to inform data.
;	AL = DATA
;
public keybd_int2
keybd_int2   PROC    FAR
        push    ds
        push    ax                      ; save registers we use
        push    bx
        push    cx
        push    dx
        push    es

        mov     cx, _DATA               ; What Cmacros does to 'DATA'
        mov     ds,cx                   ;  .. load DS
        mov     cx, RAMBIOS             ; BIOS RAM segment at 40h
        mov     es,cx                   ;  .. point ES to that!
    assumes DS, DATA
    assumes ES, RAMBIOS

        mov     ah,80h                  ; move the high order bit to the high
        and     ah,al                   ; order byte.
        xor     al,ah                   ; turn off bit if set

        call    ds:[kbd_proc]   ; standard_proc or repeat_proc or break_proc

        ; now determine if we are to call off to int 2 or kernel!
        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
	pop	ds
	ret
keybd_int2       ENDP


;***********************************************************
;
;
;
;       Input   AL= Scancode (without make/break bit)
;               AH= 00 if make, 80h if break
;
;***********************************************************

        Public break_proc

break_proc proc near

        or      ah, ah                  ; Q : Make Code ?
        jns     short kb_at_make
        push    ax
        cmp     ax, OldKeyBuf
        je      @f
        cmp     OldKeyBuf, 0
        je      @f
        mov     ax, OldKeyBuf           ; get break key code
        call    standard_proc           ; send break code
@@:
        pop     ax

        call    norepeetchk
        cmp     byte ptr[norepf],00h
        jne     norep

        mov     OldKeyBuf, ax
        mov     BreakKeyStatus, 1       ; set waiting flags
        jmp     break_ignor             ; ignor key interupt

norep:
        mov     OldKeyBuf, 0
        mov     BreakKeyStatus, 0       ; set waiting flags
        call    standard_proc
        jmp     break_ignor             ; ignor key interupt

kb_at_make:
        push    ax
        call    GetSystemMsecCount
        mov     word ptr KBtime2, ax
        mov     word ptr KBtime2 + 2 , dx

        mov     AutoRepKey, 0           ; flag reset
        pop     ax
        push    ax
        cmp     al, OldKey              ; Q : Same old key
        jne     kb_at2                  ;  N : 

        mov     ax, word ptr [Kbtime2]
        cmp     dx, word ptr KBtime     ; Q : timer is lap around ?
        jae     KB_at2                  ; Y : time
        
        not     word ptr [KBtime]       ; old time = old
        not     word ptr [KBtime + 2]   ;
        add     ax, word ptr [KBtime]
        jae     @f
        inc     dx
@@:
        add     dx, word ptr [KBtime + 2]
        jmp     KB_at1
                                                ; no lap around
KB_at0:
        sub     ax, word ptr KBtime
        jae     @f
        dec     dx
@@:
        sub     dx, word ptr KBtime+2
KB_at1:
        or      dx, dx
        jnz     KB_at2
        cmp     ax, 500                 ; Q : auto repeat ?
        jae     KB_at2                  ;  N : go exit
        mov     AutoRepKey, 0ffh        ; set Auto repeat flag
KB_at2:
        mov     ax, word ptr KBtime2
        mov     word ptr KBtime, ax
        mov     ax, word ptr KBtime2+2
        mov     word ptr KBtime+2 ,ax
        pop     ax
        mov     OldKey, al              ; set Old Key code
kb_at_send_make_key:
        cmp     AutoRepKey, 0
        jne     @f
        cmp     OldKeyBuf, 0
        je      @f
        push    ax
        mov     ax, OldKeyBuf           ; get break key code
        call    standard_proc           ; send break code
        pop     ax
        mov     BreakKeyStatus, 0       ; set waiting flags
        mov     OldKeyBuf, 0
@@:
        call    standard_proc

break_ignor:
        ret

break_proc endp

        Public break_int
break_int proc far

        push    ds
        push    ax                      ; save registers we use
        push    bx
        push    cx
        push    dx
        push    es

        mov     ax, _DATA               ; What Cmacros does to 'DATA'
        mov     ds,ax                   ;  .. load DS
        mov     ax, RAMBIOS             ; BIOS RAM segment at 40h
        mov     es,ax                   ;  .. point ES to that!
    assumes DS, DATA
    assumes ES, RAMBIOS

        mov     ax, OldKeyBuf
        cmp     ax, 0                   ; Q : no wait key ?
        jz      break_exit              ;   Y : ignor
        cmp     BreakKeyStatus, 0       ; Q : idle ?
        je      break_exit              ;   Y : ignor
        cmp     BreakKeyStatus, 1       ; Q : break wating ?
        jne     break_send              ;   N : ignor
        inc     BreakKeyStatus          ; now Counting !
        jmp     short break_exit
break_send:
        mov     ax, OldKeyBuf
        call    standard_proc
        mov     OldKeyBuf, 0            ; claer break key code
        mov     BreakKeyStatus, 0       ; clear status
break_exit:
        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        pop     ds
        ret

break_int endp


;***********************************************************
;
;
;
;       Input   AL= Scancode (without make/break bit)
;               AH= 00 if make, 80h if break
;
;***********************************************************

        Public repeat_proc

repeat_proc proc near

	or	ah, ah	; Break code?
	js	Rep_BreakCheck		;  Y: no influence to auto repeat
        xor     bx, bx
        mov     bl, al
        shr     bx, 3
        mov     cl, al
        and     cl, 7
        mov     ch, 1
        shl     ch, cl
        test    RepTable[bx], ch
        jz      Rep_Ignore_Repeat
Rep_Continue:
        mov     RepCode, ax
        mov     cx, RepDelay
        mov     RepCount, cx
Rep_Ignore_Repeat:
	call    standard_proc
        ret
Rep_BreakCheck:
	cmp	al, byte ptr RepCode
	jnz	Rep_Ignore_Repeat
	jmp	Rep_Continue

repeat_proc endp


    assumes DS, nothing
    assumes ES, nothing

        Public repeat_int

repeat_int proc far

        push    ds
        push    ax                      ; save registers we use
        mov     ax, _DATA               ; What Cmacros does to 'DATA'
        mov     ds, ax                  ;  .. load DS
    assumes DS, DATA

;       Reentry Check ??
;		Yes, Please
;
        cmp     ds:[fReEnter],0         ; are we alone?
        jz      RPI_R
        jmp     RPI_ignore              ; is he typing REALLY fast?

RPI_R:  inc     ds:[fReEnter]
        mov     ax, RepCode
        or      ax, ax
        js      no_repeat
        dec     RepCount
        jnz     no_repeat

        pusha
        push    es
        call    standard_proc
        pop     es
        popa

        mov     ax, RepRate
        mov     RepCount, ax
no_repeat:
        mov     ds:[fReEnter],0
RPI_ignore:
        pop     ax
        pop     ds
        ret

repeat_int endp

;***********************************************************
; Standard routine for translating scan code to virtual keycode
; This is the default and the only one if the keyboard is PC-9800 like.
;
;       Input   AL= Scancode (without make/break bit)
;               AH= 00 if make, 80h if break
;
;***********************************************************

        Public standard_proc

standard_proc proc near
;kb_SendKey proc near

;
; Set Shift Status in RAMBIOS
;
	push	ax
        mov     ax, RAMBIOS             ; BIOS RAM segment at 40h
        mov     es,ax                   ;  .. point ES to that!
    assumes ES, RAMBIOS
	pop	ax
        call    SetShiftState           ;                       86/01/29

;
; Check Kana
;
        cmp     al, cKana                       ; Kana key ?
        jne     kb_no_kana                      ; no
        mov     bx, ax
        mov     ax, VK_KANA
        push    bx
        call    [event_proc]
        pop     bx
        mov     ax, VK_KANA+8000h		; set break
        or      bx, 0c000h
	xor	[bKana], 80h			; Toggle the status
        jmp     kb_send
kb_no_kana:

;
; check STOP
;
        cmp     al, cStop                       ; STOP key ?
        jne     kb_no_stop                      ; no
        or      ah, ah                          ; make ?
        jnz     kb_skip_stop                    ; no
        call    stopcase
kb_skip_stop:

        cmp     stopf, 0			; need to sence CTRL state?
        jz      kb_stop
        or      ah, ah                          ; make ?
        jz      kb_mk_stop                      ;  yes
        cmp     stop_flag, 0
        jz      kb_stop
        mov     stop_flag, 0
        mov     bx, cStop
        mov     al, VK_CANCEL
        jmp     kb_send

kb_mk_stop:
        test    byte ptr es:[kb_flag], 10h      ; CTRL ON ?
        jz      kb_stop
        mov     stop_flag, 0ffh
        mov     bx, ax                          ; bx = cStop
        mov     ax, VK_CANCEL
        jmp     kb_send

kb_stop:
	mov	al, VirtKeysSTOP		; get VK_CANCEL or VK_PAUSE
	mov	bx, cStop
	jmp	kb_send

kb_no_stop:

;
; CAPS
;
        cmp     al, cCaps                       ; CAPS key ?
        jne     kb_no_caps                      ; no
        mov     bx, ax
        mov     ax, VK_CAPITAL
        push    bx
        call    [event_proc]
        pop     bx
        mov     ax, VK_CAPITAL+8000h            ; set break
        or      bx, 0c000h
        jmp     kb_send
kb_no_caps:

;
; check COPY
;
;; Check for print-screen.
;;
;; For 3.0 Windows:
;;      ALT-printscreen is grab of current window.
;;      unshifted printscreen is grab of whole screen.
;;      (shift-printscreen on XT, AT keyboards, however).
;;
;; If this is detected, event_proc is called with VK = VK_SNAPSHOT
;; and BX = 0 for current window, and 1 for full screen grab.
;
        cmp     al, cCopy                       ; COPY Key ?
        jne     kb_no_copy                      ;  no
        or      ah, ah                          ; make ?
        jz      kb_mk_copy                      ;  yes
        mov     bl, copy_flag
        cmp     bl, 0ffh                        ; if GRPH+COPY Break case ?
        je      kb_no_copy                      ;  no,
        mov     copy_flag, 0ffh                 ; clear flag
kb_copy:
        xor     bh, bh
        mov     al, VK_SNAPSHOT
        jmp     kb_send
kb_mk_copy:
        test    byte ptr es:[kb_flag],19h       ; GRPH ON?
        jnz     @f
        mov     bl, 1
        mov     copy_flag, bl
        jmp     short kb_copy
@@:
        test    byte ptr es:[kb_flag],08h       ; GRPH ON?
        jz      kb_no_copy                      ; No,
        mov     bl, 0
        mov     copy_flag, bl                   ;
        jmp     short kb_copy
kb_no_copy:

;
; check CLR/HOME
;
        cmp     al, cHomeclr			; HOME/CLR key?
        jnz     kb_no_clr
        cmp     XL_FLAG, 0			; have HOME and CLR keys?
        jnz     kb_no_clr
        cmp     homef, 0			; need to sence SHIFT state?
        jnz     kb_clr
        or      ah, ah                          ; make ?
        jz      kb_mk_clr                       ;  yes
        cmp     home_flag, 0
        jz      kb_clr
        mov     home_flag, 0
        mov     bx, cHomeclr
        mov     al, VK_HOME
        jmp     kb_send

kb_mk_clr:
        test    byte ptr es:[kb_flag], 01h      ; SHIFT ON ?
        jz      kb_clr
        mov     home_flag, 0ffh
        push    ax
        mov     ax, VK_SHIFT + 8000h            ; break SHIFT
        mov     bx, cShift
        call    [event_proc]
        pop     bx                              ; bx = cHomeclr
        mov     ax, VK_HOME
        call    [event_proc]
        mov     ax, VK_SHIFT                    ; make SHIFT
        mov     bx, cShift
        jmp     kb_send

kb_clr:
	mov	al, VirtKeysCLR			; get VK_HOME or VK_CLEAR
	mov	bx, cHomeclr
	jmp	kb_send

kb_no_clr:

;
; check HELP/END
;
        cmp     al, cHelp                       ; HELP key ?
        jnz     kb_no_help                      ;  no
        mov     bx, cHelp                       ; bx = cHelp
        mov     al, VirtKeysHELP                ; get VK_HELP or VK_END
        or      ah, ah                          ; make ?
        jz      kb_mk_help                      ;  yes
        cmp     end_flag, 0                     ; with kana when make ?
        jz      @f                              ;  no
        xor     al, VK_HELP xor VK_END          ; xchange VK_HELP <-> VK_END
        mov     end_flag, 00h
@@:
        jmp     kb_send
kb_mk_help:
        cmp     byte ptr khelpf, 0              ; Reversible On ?
        jz      @f                              ;  no
        test    byte ptr es:[kb_flag], 04h      ; KANA ON ?
        jz      @f                              ;  no
        xor     al, VK_HELP xor VK_END          ; xchange VK_HELP <-> VK_END
        mov     end_flag, 0ffh
@@:
        jmp     kb_send
kb_no_help:

;
; check Numpad'='
;
        cmp     al, cNumeq                      ; Numpad '=' Key ?
        jne     kb_no_numeq                     ; no
        or      ah, ah
        jz      kb_mk_numeq
        cmp     scr_flag, 00h                   ; if VK_OEM_SCROLL Break case ?
        je      kb_no_numeq                     ; Yes,
        mov     scr_flag,00h                    ; clear flag
        jmp     short kb_numeq
kb_mk_numeq:
        test    byte ptr es:[kb_flag],10h       ; CTRL ON?
        jz      kb_no_numeq                     ; No,
        mov     scr_flag,0ffh                   ;
kb_numeq:
        xor     bh, bh
        mov     bl, al
        mov     al, VK_OEM_SCROLL               ;
        jmp     kb_send
kb_no_numeq:

;ifdef JAPAN
;
; check VK_DBE_xxxx
;
        cmp     DBEEnable,0                     ; DBE enable?
        jnz     @f
        jmp     kb_no_dbe                       ; jump if not
@@:
        cmp     al, cXfer                       ; XFER ?
        jnz     kb_chk_nfer                     ; Jump if not

        or      ah, ah                          ; make?
        jz      kb_make_xfer                    ; jump if not

        mov     bl, bXFER
        mov     bXFER, 0
        jmp     short kb_brk_dbe

kb_chk_nfer:
        cmp     al, cNfer                               ; NFER ?
        jz      @f
        jmp     kb_no_dbe                       ; Jump if not
@@:
        or      ah, ah                          ; make?
        jnz     @f
        jmp     kb_make_nfer                    ; jump if not
@@:
        mov     bl, bNFER
        mov     bNFER, 0
kb_brk_dbe:
        cmp     bl, 1                           ; what send at make
        jnz     @f
        jmp     kb_SendKey_Exit                 ; jump if 1 : Toggle key
@@:
        jnc     @f
        jmp     kb_no_dbe                       ; jump if 0 : XFER/NFER
@@:
        xchg    al, bl
        mov     cx, ax
        call    send_vk_dbe1
        jmp     kb_send

;
; << XFER >>
;
kb_make_xfer:
        mov     bx, ax
        mov     al, byte ptr es:[kb_flag]       ; Shift State
        and     al, fShift + fCtrl + fAlt
        mov     ah, al

;
; VK_DBE_ROMAN / VK_DBE_NOROMAN
;
        mov     al, ah                          ; Shift State
        xor     al, fAlt                        ; Shift is GRPH ?
        jnz     kb_no_roman                     ; Skip if not
        mov     cl, VK_DBE_NOROMAN
        mov     ch, VK_DBE_ROMAN
        mov     bXFER, 1
        xor     bRoman, 80h
        call    send_vk_dbe
        jmp     kb_send
kb_no_roman:

;
; VK_DBE_IME_DIALOG
;
        mov     al, ah                          ; Shift State
        xor     al, fShift + fAlt               ; Shift: SHIFT+GRPH ?
        jnz     kb_no_dlg                       ; Skip if not
        mov     cx, VK_DBE_IME_DIALOG
        mov     bXFER, cl
        call    send_vk_dbe1
        jmp     kb_send
kb_no_dlg:

;
; VK_DBE_CODEINPUT
;
        mov     al, ah                          ; Shift State
        xor     al, fCTRL+fAlt                  ; Shift is CTRL+GRPH ?
        jnz     kb_no_code                      ; Skip if not
        mov     ch, VK_DBE_CODEINPUT
        mov     bXFER, 1
        cmp     bCode, 80h
        jz      kb_non_up
        xor     bCode, 80h
        xor     cl, cl
        mov     cl, VK_DBE_NOCODEINPUT
        call    send_vk_dbe
        jmp     kb_send
kb_no_code:

;
; VK_DBE_HIRAGANA
;
        mov     al, ah                          ; Shift State
        xor     al, fShift + fCtrl + fAlt       ; Shift: SHIFT + CTRL + GRPH ?
        jnz     kb_no_hira                      ; Skip if not
        mov     bXFER, 1
        mov     ch, VK_DBE_HIRAGANA
        cmp     bHiragana, 80h
        jz      kb_non_up
        xor     bHiragana, 80h
        cmp     bKatakana, 80h
        jz      kb_up_kata
        jmp     short kb_up_alph
kb_no_hira:

        jmp     kb_no_dbe1

;
; << NFER >>
;
kb_make_nfer:
        mov     bx, ax
        mov     al, byte ptr es:[kb_flag]       ; Shift State
        and     al, fShift + fCtrl + fAlt
        mov     ah, al
;
; VK_DBE_ALPHANUMRIC
;
        mov     al, ah                          ; Shift State
        xor     al, fCtrl                       ; Shift: CTRL ?
        jnz     kb_no_alph                      ; Skip if not
        mov     bNFER, 1
        mov     ch, VK_DBE_ALPHANUMERIC
        cmp     bKatakana, 80h
        jz      kb_up_kata
        cmp     bHiragana, 80h
        jz      kb_up_hira
kb_non_up:
        mov     al, ch                          ; downtrans key to post
        xor     ah, ah                          ; downtrans
        mov     cx, ax
        call    send_vk_dbe1
        jmp     kb_send
kb_up_kata:
        mov     cl, VK_DBE_KATAKANA
        xor     bKatakana, 80h
        jmp     short kb_up_zero
kb_up_Hira:
        mov     cl, VK_DBE_HIRAGANA
        xor     bHiragana, 80h
kb_up_zero:
        call    send_vk_dbe
        jmp     kb_send
kb_no_alph:

;
; VK_DBE_KATAKANA
;
        mov     al, ah                          ; Shift State
        xor     al, fAlt                        ; Shift: GRPH ?
        jnz     kb_no_kata                      ; Skip if not
        mov     bNFER, 1
        mov     ch, VK_DBE_KATAKANA
        cmp     bKatakana, 80h
        jz      kb_non_up
        xor     bKatakana, 80h
        cmp     bHiragana, 80h
        jz      kb_up_hira
kb_up_alph:
        xor     cl, cl
        mov     cl, VK_DBE_ALPHANUMERIC
        jmp     short kb_up_zero
kb_no_kata:

;
; VK_DBE_NOCODEINPUT
;
        mov     al, ah                          ; Shift State
        xor     al, fCTRL+fAlt                  ; Shift is CTRL+GRPH ?
        jnz     kb_no_nocode                    ; Skip if not
        mov     ch, VK_DBE_NOCODEINPUT
        mov     bNFER, 1
        cmp     bCode, 80h
        jnz     kb_non_up
        mov     cl, VK_DBE_CODEINPUT
        xor     bCode, 80h
        call    send_vk_dbe
        jmp     kb_send
kb_no_nocode:

;
; VK_DBE_SBCSCHAR / VK_DBE_DBCSCHAR
;
        mov     al, ah                          ; Shift State
        xor     al, fShift+fCtrl                ; Shift: SHIFT+CTRL ?
        jnz     kb_no_char                      ; Skip if not
        mov     cl, VK_DBE_SBCSCHAR
        mov     ch, VK_DBE_DBCSCHAR
        mov     bNFER, 1
        xor     bZenHan, 80h
        call    send_vk_dbe
        jmp     kb_send
kb_no_char:

;
; VK_DBE_IME_WORDREGISTER
;
        mov     al, ah                          ; Shift State
        xor     al, fShift + fAlt               ; Shift: SHIFT+GRPH ?
        jnz     kb_no_word                      ; Skip if not
        mov     cx, VK_DBE_IME_WORDREGISTER
        mov     bNFER, cl
        call    send_vk_dbe1
        jmp     kb_send
kb_no_word:

;
; VK_DBE_FLUSH
;
        mov     al, ah                          ; Shift State
        xor     al, fShift + fCtrl + fAlt       ; Shift: SHIFT+CTRL+GRPH ?
        jnz     kb_no_flush                     ; Skip if not
        mov     cx, VK_DBE_FLUSH
        mov     bNFER, cl
        call    send_vk_dbe1
        jmp     kb_send
kb_no_flush:

kb_no_dbe1:
        mov     ax, bx

kb_no_dbe:

;endif   ; JAPAN

ifdef 	NEC_SHIFT
	or	ah, ah				;Q: Break?
	jz	kb_no_shift			;  N
	cmp	al, 70h				;Q: L-Shift?
	jz	kb_fake_shift			;  Y
	cmp	al, 7Dh				;Q: R-Shift?
	jz	kb_fake_shift			;  Y
	jmp	short kb_no_shift		;  N
kb_fake_shift:
	mov	dl, byte ptr [KBD_LR_Shift]
	and	dl, 03h				;Q: L or R Shift On?
	jnz	kb_SendKey_Exit			;  Y
kb_no_shift:

endif	;NEC_SHIFT
	
CheckScanForVKey:
        public CheckScanForVKey
        push    di				; set registers and flags
        push    cx
        push    es
	pushf
	cld					; !!!!!!!
        push    ds
        pop     es

        mov     bx,     pCurrentLocale		; check base lcid list
        mov     di,     [bx+SCANTOIDX]
        mov     cx,     [bx+SCAN_SIZE]
        repnz   scasb
        jz      kbi2b

        mov     di,     dataOFFSET ScanCodes	; didnt find in pCurrent list, 
        mov     cx,     KBD_SCANS		; try base list.
        repnz   scasb
        jnz     kbi2a

	add	di, SCAN_TO_VKEY_OFFS		; found vkey in base list
        jmp     kbi2c
kbi2a:
        mov     bl,     al			; didnt find vkey anywhere!
        mov     al,     -1
        jmp     kbi2d
kbi2b:
        sub     di,     [bx+SCANTOIDX]		; move from table to table
        dec     di
        add     di,     [bx+VKEYTOIDX]
kbi2c:
        mov     bl,     al			; put back scan code
        mov     al,     [di]			; get vkey
kbi2d:
        xor     bh,     bh			; clear extended bit

	popf					; restore flags
        pop     es
        pop     cx
        pop     di
        cmp     al,     -1
        je      kbi4

Translated:

; ====== Handle keys on numeric pad ============
CheckNumPad:


        xor     bh,bh                   ; is it on numeric keypad?

        cmp     bl,3eh			; Numpad 7 key ScanCode
        jb      kbi4
        cmp     bl,50h			; Numpad . key ScanCode
        ja      kbi4


isNumPad:
        ; yes...  This is the numeric pad.
        ; Here, if NumLock is set, we change the virtual keycodes to
        ; numeric VK_NUMPAD codes, so the keys will be translated
        ; as numbers etc.  But if a shift key is down, we handle
        ; these as cursor keys, but we need to make sure that these
        ; are seen as UNSHIFTED
 
 	; But NEC's NumPad(10 Key) is ALWAYS NumPad!
  
if 0
        test    byte ptr es:[kb_flag],fNum      ; is num lock down?
        jz      kbi4                    ; no, do normal processing
        test    byte ptr es:[kb_flag],fShift ; either shift key down?
        jnz     kbi3                    ; yes, keep as cursor keys
                                        ; no, treat as numeric keys.
endif

        ; Now we prepare to translate as a NUMERIC key

        mov     al,bh
        or      al,al
        jnz     kbi4


        push    si
        mov     si,KeyNumBase

;;        mov     al,ds:[bx+si-71]        ; new: movable table
        mov     al,ds:[bx+si-64]        ; new: movable table
        pop     si

        jmp     short kbi4

kbi3:   ; The key is on the numeric pad, NumLock is set, but a shift
        ; key is down (bit(s) set in BIOS key state), so we are going
        ; to keep this as a cursor key.  To do this, we need to
        ; make sure that Windows' state vector entry for VK_SHIFT is OFF
        ; even though a shift key is actually down.

        cmp     bl, LastCursor          ; are we repeating?
        je      kbi3a                   ; if so, we just send the key
        mov     LastCursor, bl          ; otherwise, we save the scan code
                                        ; as a flag and turn off VK_SHIFT.
        push    ax
        push    bx


        mov     ax,VK_SHIFT+8000H
        mov     bl,54
        call    ds:[event_proc]
        pop     bx
        pop     ax
        jmp     short kbi4

        ; This is the same cursor key --  turn VK_SHIFT back on and
        ; clear the flag if the cursor key is being released (break)

kbi3a:
        test    ah,80h                  ; break?
        jz      kbi4                    ; if not, keep shift off, and
;;        call    ds:[event_proc]         ; send actual key now...
        call    [event_proc]         ; send actual key now...
        mov     ax,VK_SHIFT             ; pretend shift key went down
        mov     bl,54
        mov     LastCursor,ah           ; clear flag

kbi4:
        public kbi4

        ;
        ; Call windows with ah == 80h for uptrans, 00h for downtrans
        ; al = virtual key, bl = scan code
        ; bh = 0 (except for ICO extended keys, and NumLock)
        ;
        ; Windows preserves all registers
        ;
if 0
        xor     bh,bh                   ; clear and
        xchg    bh,fExtend              ; get extended key flag
endif

kb_send:
        call    [event_proc]            ; post a key
kb_SendKey_Exit:
        ret
standard_proc endp


send_vk_dbe     proc    near

        jz      @f                      ; jump if zero
        xchg    ch, cl                  ; flip vk posting order
@@:
        test    byte ptr es:[kb_flag], fAlt
        jz      send_vk_dbe2

        push    bx
        test    byte ptr es:[kb_flag], fCtrl
        jnz     @f
        mov     bx, cCtrl               ; 
        mov     ax, VK_CONTROL          ; make CTRL
        call    ds:[event_proc]         ; post a key
        mov     ax, VK_CONTROL or 8000h ; make CTRL
        call    ds:[event_proc]         ; post a key
@@:
        mov     bx, cGrph               ; 
        mov     ax, VK_MENU or 8000h    ; break GRPH
        call    ds:[event_proc]         ; post a key
        pop     bx

send_vk_dbe2:
        mov     ax, cx                  ; uptrans key to post
        mov     ah, 80h                 ; uptrans
        xor     bh,bh                   ; no extended key
        call    ds:[event_proc]         ; post a key
        mov     al, ch                  ; downtrans key to post
        xor     ah, ah                  ; downtrans

        test    byte ptr es:[kb_flag], fAlt
        jz      send_vk_dbe3

        call    ds:[event_proc]         ; post a key
        mov     bx, cGrph               ; 
        mov     ax, VK_MENU             ; make GRPH
        test    byte ptr es:[kb_flag], fCtrl
        jnz     @f
        call    ds:[event_proc]         ; post a key
        mov     bx, cCtrl               ; 
        mov     ax, VK_CONTROL          ; break CTRL
        call    ds:[event_proc]         ; post a key
        mov     ax, VK_CONTROL or 8000h ; make CTRL
@@:

send_vk_dbe3:
        ret

send_vk_dbe     endp


send_vk_dbe1    proc    near

        test    byte ptr es:[kb_flag], fAlt
        jz      send_vk_dbe4

        push    bx
        test    byte ptr es:[kb_flag], fCtrl
        jnz     @f
        mov     bx, cCtrl               ; 
        mov     ax, VK_CONTROL          ; make CTRL
        call    ds:[event_proc]         ; post a key
        mov     ax, VK_CONTROL or 8000h ; make CTRL
        call    ds:[event_proc]         ; post a key
@@:
        mov     bx, cGrph               ; 
        mov     ax, VK_MENU or 8000h    ; break GRPH
        call    ds:[event_proc]         ; post a key
        pop     bx

send_vk_dbe4:

        mov     ax, cx                  ; downtrans key to post
        xor     bh,bh                   ; no extended key

        test    byte ptr es:[kb_flag], fAlt
        jz      send_vk_dbe5

        call    ds:[event_proc]         ; post a key
        mov     bx, cGrph               ; 
        mov     ax, VK_MENU             ; make GRPH
        test    byte ptr es:[kb_flag], fCtrl
        jnz     @f
        call    ds:[event_proc]         ; post a key
        mov     bx, cCtrl               ; 
        mov     ax, VK_CONTROL          ; break CTRL
        call    ds:[event_proc]         ; post a key
        mov     ax, VK_CONTROL or 8000h ; make CTRL
@@:

send_vk_dbe5:
        ret

send_vk_dbe1    endp


norepeetchk     proc    near
        cmp     al,00h
        je      repchk01
        cmp     al,51h
        jna     repchk
        cmp     al,57h
        jb      repchk01
        cmp     al,5fh
        jna     repchk
        cmp     al,6ch
        jb      repchk01
        cmp     al,6fh
        jna     repchk
        cmp     al,75h
        jb      repchk01
repchk:
        mov     byte ptr[norepf],00h
        ret
repchk01:
        mov     byte ptr[norepf],0ffh
        ret

norepeetchk     endp


        public  stopcase
stopcase:
        cmp     fSysReq, 0              ; EnableKBSysReq() sets/resets this:
        jz      NormalStop              ; can we break to debugger?
        test    byte ptr es:[kb_flag], fAlt     ; GRAPH
        jz      NormalStop
        test    byte ptr es:[kb_flag], fCtrl    ; CTRL
        jz      NormalStop
        test    byte ptr es:[kb_flag], fShift   ; SHIFT
        jnz     NormalStop

        push    ax
        push    bx

        ; we now assume NMI points to SymDeb , so we simulate the NMI.
        ; But first, we turn control [and Alt?] off, in both the BIOS
        ; and Windows!

        mov     ax,VK_MENU+8000h        ; ALT off in Windows
        mov     bx,73h                  ; was bl .. make sure bh 0
        call    ds:[event_proc]
        mov     ax,VK_CONTROL+8000h     ; CONTROL off in Windows
        mov     bx,74h
        call    ds:[event_proc]

        ; now determine if we are to call off to int 2 or kernel!

        pop     bx
        pop     ax
        mov     DoSysReq, 1
        test    fSysReq,02

        ; now we simulate the NMI
        ; The code pointer nmi_vector must be in the CODE segment, since
        ; we want to restore ALL the other registers to what they were
        ; when the INT 09 interrupt happened, when we emulate the NMI.
        ; The Enable() function sets up nmi_vector.

        jz      StopExit                ; go to SYMDEB!!
        inc     DoSysReq                ; go to KERNEL!!
        jmp     StopExit

NormalStop:
        test    fSysReq, 2
        jnz     StopExit
        int     3
StopExit:
        ret

	public	SetShiftState
SetShiftState   proc    near

ifdef	NEC_SHIFT
	cmp	al, 70h			;Q: L-Shift?
	jnz	short NonLshift		;  N
	test	ah, 80h			;Q: L-Shift Break?
	jnz	short L_Shift_Break	;  Y
	or	[KBD_LR_Shift], 01h	;Set L-Shift On:1
	jmp	short LR_Shift
L_Shift_Break:
	and	byte ptr [KBD_LR_Shift], 0FEh	; Set L-Shift Off:0
	jmp	short LR_Shift
NonLshift:
	cmp	al, 7Dh			;Q: R-Shift ?
	jnz	short @f		;  N
	test	ah, 80h			;Q: R-Shift Break?
	jnz	short R_Shift_Break	;  Y
	or	byte ptr [KBD_LR_Shift], 02h	;Set R-Shift On:1
	jmp	short LR_Shift
R_Shift_Break:
	and	byte ptr [KBD_LR_Shift], 0FDh	;Set R-Shift Off:0
	jmp	short LR_Shift
@@:
endif	;NEC_SHIFT
        cmp     al,70h                  ;Shift ?
        jb      NoSetSts                ;no
        push    ax
ifdef	NEC_SHIFT
	jmp	@f
LR_Shift:				;Shift Only
	push	ax			;save ax
	cmp	al, 7Dh			;Q: R-Shift?
	jnz	@f			;  N
	mov	al, 70h			;SC change R-> L-Shift
@@:
endif	;NEC_SHIFT
        mov     bx, offset kb_flag
        sub     al,70h
        mov     cl,1
        xchg    al,cl
        shl     al,cl
ifdef	NEC_SHIFT
	cmp	cl, 00h			;Q: Shift?
	jnz	@f			;  N
	push	ax			;save ax
	mov	al, [KBD_LR_Shift]
	and	al, 03h			;Q: Shift On?
	pop	ax			;restore
	jnz	short MakeSts		;Shift Make
	jmp	BreakSts		;Shift Break
@@:
endif	;NEC_SHIFT
        test    ah,80h
        jnz     BreakSts
ifdef	NEC_SHIFT
MakeSts:
endif	;NEC_SHIFT
        or      es:[bx],al
        jmp     short ShiftStsRet
BreakSts:
        not     al
        and     es:[bx],al
ShiftStsRet:
        call    setCMOS
        pop     ax
        push    ax
        push    cx
        mov     ah,al
        and     al,07fh
        mov     cl,al
        and     al,07h
        mov     bx,OFFSET ShiftSts3
        xlat    byte ptr [bx]
        xor     bx,bx
        mov     bl,cl
        shr     bx,1
        shr     bx,1
        shr     bx,1
        xor     es:kb_all_flag[bx],al
        pop     cx
        pop     ax
NoSetSts:
        ret

SetShiftState   endp

else	;NEC_98
public keybd_int
keybd_int   PROC    FAR


        push    ds
        push    ax                      ; save registers we use
        push    es

        mov     ax, _DATA               ; What Cmacros does to 'DATA'
        mov     ds,ax                   ;  .. load DS
        mov     ax, RAMBIOS             ; BIOS RAM segment at 40h
        mov     es,ax                   ;  .. point ES to that!
    assumes DS, DATA
    assumes ES, RAMBIOS

        in      al,kb_data              ; get the scan code
        mov     ah,es:[kb_flag]         ; get current shift state..

ifdef   KOREA   ; for TG Hangeul keyboard
ifdef	ENHANCE
	cmp	al,0F2h 		; TG AT Hangeul Key scan code?
else
	cmp	al,0F0h 		; TG AT Hangeul Key scan code?
endif
	je	short yesTG
	cmp	al,0F1h
	jne	short noTG
yesTG:
	mov	fTGkeybd, 1
	jmp	kbi1
noTG:
endif   ;KOREA

ifdef JAPAN  ;  a01/002/003/001 Kana break key is in US cCommand range.
        cmp     byte ptr [KeyboardSubType],Kbda01 ; 106/001/002/003 keyboard ?
        jb      @F                                ; No.        
        cmp     al, cAck                ; a01 keyboard command code == 0xfa,....
        jz      kbToBios
        cmp     al, cSelfError
        jz      kbToBios
        cmp     al, cResend
        jz      kbToBios
        jmp     kbiTestCursor
@@:
endif
        cmp     al,cCommand             ; is it a keyboard command?
        jb      kbiTestCursor           ; (note: cCommand is different
                                        ;  for Ericsson!)
        jmp     kbToBios                ; if so, let BIOS do it

kbiTestCursor:
ifdef TOSHIBA
        test    es:[kb_flag], fRShift   ; RShift pressed?
        jz      kbi_noRshifted          ; no
        cmp     al,cRShiftB             ; RShift break?
        je      @F
        cmp     al,cBSlashB             ; BSlash break?
        jne     kbi_noRshifted
@@:
        mov     al,cRShiftB             ; reset Rshift.
        jmp     kbi_NotCnv
kbi_noRshifted:
	test	ScrlKana_flg,fKanaCnv	; need Kana conversion?
	jz	kbi_Chk_Scrl		; no
        test	byte ptr ds:[bKana],080h; Kana Shifted?
	jz	kbi_Chk_Scrl		; no
	test	ScrlKana_flg,fMCnv	; need M conversion?
	jz	kbi_isNotM		; no
	test	ah,fShift		; L|R Shifted?
	jz	kbi_isNotM
	cmp	al,c_M			; 'M' make?
	je	kbi_isM			; yes
	cmp	al,c_M + 80h		; 'M' break?
	jne	kbi_isNotM
kbi_isM:
	push	bx
	mov	bl,c_M
	sub	bl,cGrave
	sub	al,bl			; 32->29/B2->A9
	pop	bx
	jmp	kbi_NotCnv
kbi_isNotM:
	test	ah,fScroll		; ScrollLock?
	jz	kbi_doConv		; no
	test	ScrlKana_flg,fScrlCnv	; need Scroll conversion?
	jnz	kbi_NotCnv		; yes Don't convert.
	jmp	kbi_doConv		; no
kbi_Chk_Scrl:
	test	ScrlKana_flg,fScrlCnv	; need Scroll conversion?
	jz	kbi_NotCnv		; no
	test	ah,fScroll		; Scroll Locked?
	jz	kbi_NotCnv		; no
;;; Scan code conversion for 101keymode on DOS/V.
kbi_doConv:
	push	bx			; use for make/break bit
	xor	bx,bx
	test	al,80h			; break code?
	jz	short kbi_DC_is_make	; no
	mov	bl,80h
	and	al,7fh
kbi_DC_is_make:
	cmp	al,cGrave		; GraveAccent?
	jne	kbi_isNotGrave		; no
	mov	al,cYen			; 29->2B
	jmp	kbi_CnvEnd
kbi_isNotGrave:
	cmp	al,cYen			; YenMark?
	jne	kbi_isNotYen		; no
	mov	al,cGrave		; 2B->29
	jmp	kbi_CnvEnd
kbi_isNotYen:
	cmp	al,cRShift		; RShift?
	jne	kbi_isNotRShift		; no
	mov	al,cBSlash		; 36->56
	jmp	kbi_CnvEnd
kbi_isNotRShift:
	cmp	al,cBSlash		; BSlash?
	jne	kbi_CnvEnd		; no
	test	ah,fScroll		; ScrollLock?
	jnz	kbi_isBSlash		; yes
	;;; trash this code.
	assumes	DS,DATA
	push	ax
	mov	al,ds:AckByte
	out	ack_port,al		; EOI
	pop	ax
	;;; we'll pop bx at ignorekey.
	jmp	ignorekey		; trash it.
kbi_isBSlash:
	mov	al,cRShift		; 56->36
	jmp	kbi_CnvEnd
kbi_CnvEnd:
	or	al,bl
	pop	bx
kbi_NotCnv:
;;; fixed by Chym to prevent RShift locking. 1995-09-21
        test    al, 80h
        jz      @F
; for safety reason, we unlock the flags no matter it's RO or not
        mov     kanamlock, 0

@@:
        and     es:[kb_flag], not fRShift ; should be changing the BIOS.

;;; fixed end.
endif ;TOSHIBA

        cmp     LastCursor, 0	; is flag (scancode) set for numpad
        jz      kbiTestUp       ; cursor?
        cmp     al, LastCursor  ; yes, is this the same scancode?
        jne     kbiTestCursorUp ; no, might be same key, different state
        jmp     kbi1            ; yes, so it's a MAKE of the same one
;
; Determine if the same scancode but different state - it might be a 
; breaking key of the same scancode. 
;        
kbiTestCursorUp:                ; no... (was not same scancode)
        push    ax              ; it might be BREAK for same code
        and     al,7fh          ; remove key state (80H == break)
        cmp     LastCursor,al   ;
        pop     ax              ;
        je      kbiTestUp       ; if ==, it is a break of same scancode
;
;  What we have here is a reentrancy rarity: the interrupt belongs to a key  
;  that is not the same as the last NUMPAD cursor key. By definition NUMLOCK
;  is down (ON) here, and so is shift, but we are faking right-shift-UP to
;  USER if numpad cursor keys are being used. There is logic in standard_proc
;  for numeric keypad cursor keys which sets LastCursor.
;
;  We simulate SHIFT-down to USER and let standard_proc decide on the key's 
;  coming impact (including call to SetShiftState). If shift is breaking it will
;  generate a SHIFT-up message to USER to counter this message, because we will
;  clear LastCursor here and now. If a different key then we let standard_proc
;  potentially reconstitute the numpad processing logic. It appears safer to
;  assume the shift-down condition and let further processing counter this
;  assumption.
;
public KbiCollision
KbiCollision:
;
        push    ax               ; but it's not break, so we
        push    bx               ; save original key info and
        mov     ax, VK_SHIFT     ; prepare to fake SHIFT-down 
        mov     bx,54
        mov     LastCursor, 0    ; clear flag (protected by CLI)
                                 ; do now in case USER does STI
        call    ds:[event_proc]  ; fake SHIFT DOWN message
                                 ; USER had better not enable interrupts. 
; 
;  ** Problem ** if the reentrancy flag (fReEnter) is set then standard_proc 
;  is not called and the shift may get stuck down, so we conditionalize the
;  calling of USER. Note: we believe the first SHIFT-down message must be sent 
;  regardless because numpad cursor keys use a faked shift-up condition. that 
;  must be neutralized. Later, in standard_proc when the interrupt system in ON 
;  there is more carefull attention paid to reentrancy.
;
        test    ds:[fReEnter], 0FFH  ; is the field crowded?
        je      kbiTestUp1           ; no, try to reach standard_proc
;
;  Windows preserves all registers. Assume 
;
        inc     ShiftCollision         ; record this rare event        
        mov     ax, VK_SHIFT+8000H     ; fake SHIFT UP
        mov     bx,54                  ;
        call    ds:[event_proc]
        call    SetShiftState          ; correct the key state
        
kbiTestUp1:        
        pop     bx          ; restore original key info
        pop     ax          ; and atempt to send it

;-------------------------------------------------------------------        
     
kbiTestUp:
        test    al,80h                  ; is it an up transition?
        jz      notkbi1                 ; .. if not, continue
        jmp     kbi1                    ; it's an UP transition, jump..
notkbi1:                                ; it's a DOWN transition..

        ; this will make BIOS clear fHold if it's set:
        
        test    byte ptr es:[kb_flag_1],fHold   ; in hold state?
        jnz     jkbToBios               ; if so, jump to ROM

        cmp     al,cCapsLock            ; is it CapsLock ?
        jnz     kbib                    ; no...

kbic:                               ; it's CapsLock --

ifdef JAPAN     
        cmp     byte ptr [KeyboardSubType],Kbd101 ;2 101 keyboard ?
;        jnz     no101Kana                         ;2 no
;        test    ah,fShift               ;2; Shift+Ctrl+CapsLock ?
;        jz      no101Kana               ;2; no .. check Ctrl
;        test    ah,fCtrl                ;2;
;        jz      no101Kana
;        jmp     kbi1                    ; no ctrl-caps stuff.
;        xor     byte ptr bKana, 80h     ;set Kana state to lock position.
;no101Kana:
         jz      kbi1
endif
ifdef JAPAN
;       BUG BUG BUG!
;       CTRL+CAPSLOCK is Hiragana key for DOS IME.
;       pass to int 9 handler will generate B6h scan code and sound beep eventually.
        test    gNew106, 1
        jnz     kbi1                    ; don't want to jump to ROM if new 106 case.
endif
        test    ah,fCtrl                ; is it Ctrl-NumLock or -CapsLock?
        jnz     jkbToBios               ; yes, jump to ROM
        jmp     kbi1                    ; no, go check hotkey
jkbToBios:
        jmp     kbToBios


; It's not capslock 
; [AH] = BIOS shift state, [AL] = scan code, 
; If it's Delete or Break, go check for Ctl-Alt


kbib:   

        cmp     al,cDelete
        je      kbia
        cmp     al,cBreak
        je      kbia


ifdef   ENVOY
; Look for CTRL-Alt + or - which on Vectra A, and A+ is handled in the
; int9 BIOS.
;
        test    ds:fVectra, FV_A ; If Vectra A,A+, Pass thru Cntl-Alt + and -
        jz      kbihp
        cmp     al,cPlus
        je      kbia
        cmp     al,cMinus
        je      kbia

kbihp:
        cmp     al,cBackSlash           ; If Carrera, pass thru Cntl-Alt-\
        je      kbia                    ; for speed change


; The following code was added to test for the possiblity of a CCP CTRL-ALT-DEL
;
        test    ds:[fVectra], FV_ENVOY
        jz      no_reset
        cmp     al, cCCPDel
        je      kbia
no_reset:
endif ; ENVOY

; OLD Screen Print code here deleted 14 dec 88 .. see screen-capture code
; at KBI1:

; kbia: check for Control ALT something here
; Scan code is one of
;
;       cDelete         (Reset)
;       cBreak          (interrupt)
;       cPrint          (Screen Grab)
;       
;       cCCPDel         - Vectra
;       cBackSlash      - Vectra
;       cMinus          - Vectra
;       cPlus           - Vectra
;
; 
kbia:
        mov     ah,es:[kb_flag]         ; Get BIOS shift state (again)
        not     ah
        test    ah,fAlt+fCtrl           ; test for CTRL-ALT something...
        jnz     kbi1                    ; nope, go to hotkey check


; Control and Alt are BOTH down now.  We're going to BIOS
; if its cDelete..

        cmp     al,cDelete
        jne     kbi1

; It's control-alt-Delete ..
; Inform the kernel that Ctl+Alt+Del is happening so that he can inform
; any expanded memory card to reset. (Wed 21-Oct-1987 : bobgu)
; Not needed as of 5/21/93 DavidDS.  Handled by Win386 etc.
;       call    KbdRst

kbToBios:

        pop     es
        pop     ax

        push    word ptr [bios_proc][2]
        push    word ptr [bios_proc][0]     ; put address of bios proc on stack
        push    bp
        mov     bp, sp
        mov     ds, [bp+6]                  ; restore ds
        assumes DS,NOTHING
        pop     bp
        retf    2                           ; "return" popping ds

kbi1:

;
; Reset the keyboard controller and acknowledge the interrupt.
;

        assumes DS,DATA

kbi13:

        push    ax                      ; this little bit is for XT-like systems
        in      al,kb_ctl               ; reset interface chip (8255)
        mov     ah,al
        or      al,80h
        out     kb_ctl,al
        xchg    ah,al
        out     kb_ctl,al
        pop     ax                      ; just for delay..

        ;; ??? ;;                       ; for ATs and PS2s, may need 
                                        ;  to enable keyboard here!

        push    ax                      ; this is for XT or AT-like systems
        mov     al,ds:AckByte           ; acknowledge interrupt
        out     ack_port,al
        pop     ax

ifdef   KOREA   ; for TG Hangeul Keyboard
	xor	ah,ah
ifdef	ENHANCE
	cmp	al,0F2h
else
	cmp	al,0F0h
endif
	je	short kbi13_tg
	cmp	al,0F1h
	je	short kbi13_tg
endif   ;KOREA

        mov     ah,80h                  ; move the high order bit to the high
        and     ah,al                   ; order byte.
        xor     al,ah                   ; turn off bit if set

ifdef KOREA
kbi13_tg:
endif

        push    bx                      ; stack = [AX,DS,ES,BX]

; Check for print-screen.
;
; For 3.0 Windows:
;       ALT-printscreen is grab of current window.
;       unshifted printscreen is grab of whole screen.
;       (shift-printscreen on XT, AT keyboards, however).
;
; If this is detected, event_proc is called with VK = VK_SNAPSHOT
; and BX = 0 for current window, and 1 for full screen grab.
;
; AL contains scan code (hi bit is 0), AH contains up/down flag.
;
        mov     bl,es:[kb_flag]         ; get current shift state..
        test    bl,fCtrl                ; CTRL down? if so,
        jnz     kbiNotPrint             ; This is no printscreen...!

ifdef ENHANCE ; 101/102 enhanced?
        ; this little bit is a check for ALT-PRINT (= ALT-SysReq) on
        ; enhanced keyboard.  What a pain!  the PRINT key gives unprefixed
        ; SYSRQ scancode if ALT is down, and we want ALT-PRINT for current
        ; window grab!
        cmp     ds:[KeyType],4          ; Enhanced?
        jne     kbiNoAltPrint
        cmp     al,SYSREQ               ; SYSREQ scan code?
        jne     kbiNoAltPrint           ; .. nope.
        test    bl,fAlt                 ; ALT down? (must be, but be patient)
        jz      kbiNoAltPrint
        mov     al,VK_SNAPSHOT          ; this is really a screen grab
        mov     bx,0                    ;  0: current window
        jmp     short kbiPrintSend      ; 
    kbiNoAltPrint:
endif
        cmp     al, cPrint              ; PrtScn scancode (multiply key)?
        jne     kbiNotPrint             ; (XT, AT, or Enhanced)

ifdef ENHANCE ; 'RT' 101/102 keyboard supported?
        cmp     ds:[KeyType],4          ; Enhanced keyboard?
        jne     kbiIsPrintKey           ; if not, skip the following;
                                        ; if so, don't need shift..
                                        ; and check/restore kbd_proc..
        cmp     ds:[kbd_proc], codeOFFSET prev_was_ext_proc
        jne     kbiNotPrint             ; MUST have E0 prefix for CTRL PRINT
        mov     ds:[kbd_proc], codeOFFSET standard_proc
        
        ; it is an enhanced-keyboard printscreen key, so..
        test    bl,fShift               ; is SHIFT down?
        jnz     kbiGoIgnore             ; if so, don't do it.
        mov     al,VK_SNAPSHOT
        mov     bx,1                    ; full-screen grab: bx == 1
        jmp     short kbiPrintSend

    kbiIsPrintKey:

endif ; ENHANCE
        ; We have Printscreen scancode on non-enhanced keyboard.
        ; We look for SHIFT or ALT, and snap all or part of the screen.
        ; (unshifted PrintScreen is '*' character in this case)
kbiCheckPrint:
        test    bl,fShift+fAlt          ; is SHIFT or ALT down?
        jz      kbiNotPrint
kbiCheckIsAlt:
        mov     al,VK_SNAPSHOT          ; send special snapshot VK code.
        test    bl,fAlt                 ; is ALT down?
        mov     bx,1                    ; set BX, but don't change Z flag!!
        jz      kbiPrintSend            ; ALT-PRINT has SysRq scancode, so skip
        dec     bx                      ; if ALT, clear BX

kbiPrintSend:
        call    ds:[event_proc]         ; send VK_SNAPSHOT !!
kbiGoIgnore:
        jmp     ignorekey               ; .. no more processing

kbiNotPrint:

ifdef   SYSREQ
;
;  The (PC-AT) SYS REQ key is used to simulate an NMI.
;
;  This is done by clearing up the stack and doing a far jump to the NMI
;  interrupt vector location.  If the NMI interrupt points into the ROM,
;  we don't jump, since the ROM treats NMIs as parity errors.
;
;  When SymDeb returns from the NMI, it returns to wherever CS:IP was
;  when the keyboard interrupt happened!
;
; Depending on the system and keyboard, SysReq requires that ALT or
; both Control and ALT be down, when the scan code 54h is input.
;
; On the Olivetti M24, this is the scan code for the 00 key.
; 
; On Enhanced (RT) and ICO (102-key Olivetti M24) keyboards, interpret
; Control-Alt-PrintScreen or Control-Alt-00 as Sys Req.
; We do a little arithmetic on the ifdef's here...
;
; Additional test, required if this is an Enhanced or ICO (2) keyboard:
; Must use CTRL ALT SCR-PRT on these keyboards, since Windows uses
; ALT SCR-PRT for screen grab.


        cmp     al,SYSREQ               ; SYSREQ key?
        jne     notsys

ifdef ENHANCE

        ; This handles Enhanced but NOT ICO keyboards:
        ; For Enhanced keyboards, this is the printscreen key, so
        ; we change the scan code if Ctrl is not down.

        cmp     ds:[KeyType], 4         ; skip the following test,
        jne     sysNotEnh               ;  if it's XT or AT keyboard.
                                        ; it IS an Enhanced keyboard
        test    byte ptr es:[kb_flag], fCtrl    ; is Ctrl down?
        mov     al, cPrint              ; change scan code
        jz      notsys
sysNotEnh:

endif ;  CtrlAltSysRq -- ENHANCE or ICO

sysreqwait:
        or      ah,ah                   ; Only on key-down
        js      sysreqdown              ; sign set if key-up
igkey:  
        jmp     ignorekey

sysreqdown:

        cmp     fSysReq, 0              ; EnableKBSysReq() sets/resets this:
        jz      igkey                   ; can we break to debugger?

        ; we now assume NMI points to SymDeb , so we simulate the NMI.
        ; But first, we turn control [and Alt?] off, in both the BIOS
        ; and Windows!

        and     byte ptr es:[kb_flag], not (fCtrl+fAlt) ; do BIOS

        mov     ax,VK_MENU+8000h        ; ALT off in Windows
        mov     bx,38h                  ; was bl .. make sure bh 0
        call    ds:[event_proc]
        mov     ax,VK_CONTROL+8000h     ; CONTROL off in Windows
        mov     bx,1dh
        call    ds:[event_proc]

        ; now determine if we are to call off to int 2 or kernel!

        test    fSysReq,02

        ; now we simulate the NMI
        ; The code pointer nmi_vector must be in the CODE segment, since
        ; we want to restore ALL the other registers to what they were
        ; when the INT 09 interrupt happened, when we emulate the NMI.
        ; The Enable() function sets up nmi_vector.

        pop     bx
        pop     es
        pop     ax
        jnz     @F

        push   word ptr nmi_vector[2]
        push   word ptr nmi_vector[0]  ; address of nmi_vector on stack
        push   bp
        mov    ds, [bp+6]              ; restore ds
        pop    bp
        retf   2                       ; "return" to nmi_vector & pop ds

@@:     pop     ds
        jmp     CVWBreak                ; go to KERNEL!!
notsys:

ENDIF                                   ; End of SYSREQ stuff

; We are about to enable interrupt, before doing that we have to protect
; against another interrupt happening before we are done with this one.

        cmp     ds:[fReEnter],0         ; are we alone?
        jz      kbiR                    ; yes, continue
        
        inc     NestedCount             ; no, count the event 
        jmp     ignorekey               ; is he typing REALLY fast?

kbiR:   inc     ds:[fReEnter]
    ;
    ;   In order for this to work in various 386 Virtual environments
    ;   just setting this "fReEnter" exclusion flag is not the right thing
    ;   to do. A 386 virtual machine monitor may be Simulating keyboard
    ;   activity to us and just doing this exclude will cause us to miss
    ;   lots of keys that the monitor is trying to send us because it will
    ;   send us keys as soon as we EOI the keyboard and enable interrupts.
    ;
    ;   We fix this problem by masking off the keyboard interrupt at the
    ;   interrupt controller. This prevents the 386 monitor from sending us
    ;   more keys till we're ready to get them. This works fine in the non-386
    ;   environments as well. This method is prefered over disabling the
    ;   keyboard at the Keyboard Controller because it is more portable.
    ;   There seems to be a fair variation amoung clones in the keyboard
    ;   controller used, but the keyboard is always IRQ 1, and the interrupt
    ;   controller is always at the IBM port addresses, and is an 8259.
    ;
    ;
        push    ax
        in      al,21h                  ; Get IRQ mask register
        or      al,02h                  ; mask off kybd ints (IRQ 1)
        jmp     $+2                     ; I/O delay for AT type machines
        jmp     $+2
        out     21h,al                  ; set new mask
        pop     ax

ifdef BIGSTK    ; hp addition..

; Modification to allow apps with small stack sizes not to blow up
; (i.e. diskcopy) (HP c-ralphp 6/9/88)
;
;
; Switch to a private stack
;
        mov     ds:SaveSS, ss
        mov     ds:SaveSP, sp
        mov     ds:NextSS, cs
        mov     ss, ds:NextSS
        mov     sp, dataOFFSET Stack

; end (HP c-ralphp modifications)
endif ; BIGSTK

public kbiCall
kbiCall:
        xor     bh,bh
;;;     sti       ; interrupts on for others... (do this later!!)
;
; it is important to do the STI after the call and not rely on the 
; hardware to ensure that the interrupt system remains off for the 
; next instruction as well (namely the call). Nested interrupts can
; conceivably destroy the kbd_proc link before the previous call 
; gets to execute.
;
        call    ds:[kbd_proc]   ; standard_proc or prev_was_ext_proc

ifdef BIGSTK
; Modification to restore the original stack (HP c-ralphp 6/9/88)
;
; Restore the old stack
;
        cli
        mov     ss, ds:SaveSS
        mov     sp, ds:SaveSP

; end (HP c-ralphp modifications)
endif ; BIGSTK

        mov     ds:[fReEnter],0         ; unlock keyboard
;
; Re-enable keyboard INTs at the interrupt controller
;
        cli                       ; Ints off again.
        in      al,21h            ; get IRQ mask register
        and     al,NOT 02h        ; turn on kybd ints again (IRQ 1)
        jmp     $+2               ; I/O delay for AT type machines
        jmp     $+2
        out     21h,al            ; restore correct mask
        jmp     $+2               ; I/O delay for AT type machines
        
int_exit:
        pop     bx
        pop     es
        pop     ax
        pop     ds
        sti                       ; add for '286
        iret
        
;-----------------------------------------------------------------        
Public ignorekey
ignorekey:
        cmp     ds:[fReEnter],0  ; are we alone?
        je      int_exit         ; yes, then just leave
        
        cmp     al, cExtended    ; scancode = e0?
        jne     int_exit1
        inc     partial_faile0   ; yes, count it
        jmp     int_exit         ;
                
int_exit1:                ; 
        cmp     al, cExtended1   ; scancode = e1?
        jne     int_exit2        ; 
        inc     partial_faile1   ; yes, count it
        
int_exit2:
        jmp    int_exit ; easy to differentiate from normal exits

keybd_int   ENDP

;*****************************************************************************
;******************************* standard_proc *******************************
;*****************************************************************************
;
; Standard routine for translating scan code to virtual keycode
; This is the default and the only one if the keyboard is NOT RT-like.
;
;       Input   AL= Scancode (without make/break bit)
;               AH= 00 if make, 80h if break
;               BH=0
;
        Public standard_proc

standard_proc proc near

        sti
        xor     bh,bh                   ; Clear Extended key flag
        cmp     al,cNumLock             ; (See ICO code below)
        jne     notNumLock              ; but if it's NumLock, we
        inc     bh                      ; set it.
notNumLock:
        mov     fExtend, bh

        call    SetShiftState

ifdef ENHANCE                           ; driver for enhanced keyboard

; The following block of code is for drivers handling Enhanced keyboard
; only!

        cmp     ds:[KeyType],4          ; Enhanced?
        jne     noPrevExt
                                        ; this IS an Enhanced keyboard.
        cmp     al,cExtended1           ; E1 prefix for Pause key?
        jne     CheckForE0
        mov     ds:[kbd_proc],codeOFFSET pause_proc     ; yes, handle specially
        jmp     short j_stdproc_end

CheckForE0:
        cmp     al,cExtended            ; is this Extended scan code (E0)?
        jne     noPrevExt               ; if so, next keyboard int handled
        mov     ds:[kbd_proc],codeOFFSET prev_was_ext_proc      ; by this..
j_stdproc_end:
        jmp     stdproc_end

        Public noPrevExt

noPrevExt:

; code to handle ShiftLock.
;
; This code simulates pressing CapsLock when either the ShiftLock
; or either Shift key is pressed, depending on the state of the fCaps
; flag in kb_flag.
;
; To the Windows high-level code, and to the ToAscii() routine, this
; looks just like CapsLock.

        test    ds:[fKeyType],kbShiftLock       ; ShiftLock for this KB?
        jz      noSpecial               ; no, normal Caps Lock instead.

        cmp     al,cCapsLock            ; yes -- is this Capslock key?
        jnz     noCaps
        or      ah,ah                   ; up or down?
        js      shiftLockUp

        test    byte ptr es:[kb_flag], fCaps    ; shiftlock set?
        jnz     shiftLockUp             ; if not, 
        call    FakeCapsLock            ; fake CAPSLOCK key going down/up

shiftLockUp:                            ; CapsLock up .. do nothing
        jmp     stdproc_end

noCaps:                                 ; not Caps Lock key
        cmp     al,cLShift              ; is (non-extended) Left Shift?
        je      isShift
        cmp     al,cRShift
        jnz     noSpecial
isShift:                                ; it's shift key, clear Caps Lock
        or      ah,ah                   ; is it make?
        js      shiftUp                 ; skip if not

        test    byte ptr es:[kb_flag], fCaps    ; is shiftlock flag set?
        jz      shiftUp                 ; if so, 
        call    FakeCapsLock            ; fake CAPSLOCK key going down/up

shiftUp:

;; End of special code for RT keyboard ...

noSpecial:

endif   ; ENHANCE

ifdef   KOREA
	;
	;	generate Soft key 
	;	 - VK_HANGEUL (Shift+Alt) 
	;	 - VK_HANJA (Shift+Ctrl)
	; 
	; 	BH reg is destoried.
	;

;;;;;;;;;;;;;;;;;;;; for TG Hangeul Keyboard

;	or	ah, ah
;	jns	HG25			; eat the down-key
        cmp     byte ptr [KeyboardSubType],Kbd84
        je      KBD84S
        cmp     byte ptr [KeyboardSubType],Kbd101s
        je      KBD101S1
        jmp     hannotoggle1

KBD84S:
        cmp     al,cSpace               ; BeomOh's toggle key coding start...
        jnz     notoggle                ; No toggle key, jump nomal operation
        test    byte ptr es:[kb_flag],fLShift
        jz      notoggle
        mov     bl, al
        mov     al,VK_HANGEUL           ;
        jmp     short HG20TG            ; BeomOh's toggle key coding end...
notoggle:

        cmp     al,cSpace
        jnz     hannotoggle             ; No toggle key, jump nomal operation
        test    byte ptr es:[kb_flag_1],fLCtrl
        jz      hannotoggle
        mov     bl, al
        mov     al,VK_HANJA
        jmp     short HG20TG
hannotoggle:


KBD101S1:
        cmp     ah, 80h                 ; prevent too fast switching.
        je      hannotoggle1            ;
        cmp     al,cSpace               ; BeomOh's toggle key coding start...
        jnz     notoggle1               ; No toggle key, jump nomal operation
        test    byte ptr es:[kb_flag],fLShift
        jz      notoggle1
        mov     bl, al
        mov     al,VK_HANGEUL           ;
        jmp     short HG20TG            ;
notoggle1:                              ; BeomOh's toggle key coding end...

        cmp     al,cSpace
        jnz     hannotoggle1            ; No toggle key, jump nomal operation
        test    byte ptr es:[kb_flag_1],fLCtrl
        jz      hannotoggle1
        mov     bl, al
        mov     al,VK_HANJA
        jmp     short HG20TG
hannotoggle1:


ifdef	ENHANCE
	cmp	al, 0F2h
else
	cmp	al, 0F0h
endif
	jnz	short HG15TG
	mov	bl, al
	mov	al, VK_HANGEUL
	jmp	short HG20TG
HG15TG:
	cmp	al, 0F1h
	jnz	short HG30TG
	mov	bl, al
	mov	al, VK_HANJA
HG20TG:
        call    ds:[event_proc]
	jmp	stdproc_end
 
HG30TG:

;;;;;;;;;;;;;;;;;;;;;; end of TG

endif   ;KOREA

; If the 2 shift keys are down, when the first is released we need
; to keep Shift State down, so we'll ignore the first Shift break.
; Remember: we already called SetShiftState that may have reset
;           the Shift state we are interested in, Yeerk ! 
;
        test    byte ptr es:[kb_flag],fShift
        jz      cbd0                    ; no Shift down
        or      ah,ah                   ; is it break ?
        jns     cbd0                    ; no
        cmp     al,cLShift              ; LeftShift ?
        jz      nosp1                   ; yes, ignore
        cmp     al,cRShift              ; RightShift ?
        jnz     cbd0                    ; no, skip
nosp1:  jmp     stdproc_end             ; yes, ignore
cbd0:

;
; Translate scan code to virtual key
; First, is it control-numlock or control-break?
;
        test    byte ptr es:[kb_flag],fCtrl     ; is it Ctrl?
        jz      kbi2

;; test for control-numlock:
        cmp     al,cNumLock
        jnz     kbi1a

        ; Control + Numlock translated to VK_PAUSE ..
        mov     bx,ax
        mov     al,VK_PAUSE
        jmp     kbi4                    ; (no longer short)

kbi1a:                                  ; it's NOT NumLock

        cmp     al,cBreak               ; is it Break?
        jnz     kbi2

        mov     ds:[fBreak],0
        mov     bx,ax
        test    ah,80h                  ; only signal for up transitions!
        jz      @F
@@:     
        mov     al,VK_CANCEL
tokbi4:
        jmp     kbi4

kbi2:   

ifdef   JAPAN
        public debug_japan      ; for debug purpose
debug_japan:
endif

ifdef JAPAN
        mov bl,al                                       ; Assume later process will not depends on bl value.
;-------------------------------------------------------+
;                                                       |
;       generate DBE virtual keys                       |
;                                                       |
;-------------------------------------------------------+
        cmp     byte ptr [KeyboardSubType],Kbd101       ; 101 keyboard ?
        jnz     @F                                      ; no
        jmp     kbi_101                                 ;
@@:                                                     ;
        cmp     byte ptr [KeyboardSubType],KbdAX        ; AX keyboard ?
        jnz     @F                                      ; no
        jmp     kbi_AX                                  ;
@@:                                                     ;
        cmp     byte ptr [KeyboardSubType],KbdA01       ; A01 keyboard ?
        jnz     @F                                      ; no
        jmp     kbi_A01                                 ;
@@:                                                     ;
        cmp     byte ptr [KeyboardSubType],KbdDBCS_OLD1 ; DBCS old keyboard ?
        ja      @F                                      ; no
        jmp     kbi_DBCS_OLD                            ;
@@:                                                     ;
        jmp     noDBE_Vkey                              ;

;-----------------------------------------------+
;                                               |
; for 101 keyboard (OADG assign)                |
;                                               |
; `(#1)                 VK_KANJI                |
;                       VK_DBE_DBCSCHAR/SBCSCHAR|
;                                               |
; CapsLock              VK_KANA                 |
;                       VK_DBE_ALPHANUMERIC     |
;                       VK_DBE_HIRAGANA         |
;                       VK_DBE_KATAKANA         |
;                       VK_DBE_ROMAN/NOROMAN    |
;                                               |
;-----------------------------------------------+
public kbi_101
kbi_101:
        ; Alt + `(#1) translated to VK_KANJI
        cmp     bl,cNo1
        jnz     noKanji_101
;;; stick kanji hotkey bug.
        cmp     bKanjiDown, 080h
        jne     @F
; It seems we are slow enough to prevent this????
;        or      ah, ah
;        jz      noKanji_101             ; Repeat Kanji down will be remove here.    
        mov     bKanjiDown, 00h ; Reset flag
        mov     al, VK_KANJI
        jmp     kbi4
@@:
;;;
        test    byte ptr es:[kb_flag],fAlt
        jz      noKanji_101
        mov     al, VK_KANJI
        mov     bKanjiDown, 080h
        jmp     kbi4
;         jmp noDBE_VKey
noKanji_101:

        ; Shift + Ctrl + CapsLock translated to VK_KANA
        cmp     bl,cCapsLock
        jnz     noKana_101
;;; modify to stablize kana hotkey.
        or      ah, ah                  ; make or break?
        jz      @F                      ; make                     
        cmp     bKanaDown, 080h         ; break case, see if CTRL+SHIFT+KANA keydown before?
        jz      KanaUp_101              ; hot key succeed.        
        jmp     noKana_101              ; only single kana key pressed.
@@:
        test    byte ptr es:[kb_flag],fCtrl
        jz      noKana_101
        test    byte ptr es:[kb_flag],fShift
        jz      noKana_101
public debug_101
debug_101:
;        or      ah,ah                   ; Is it make?
;        jnz     @F                      ; toggle if key up      92/08/18 yasuho
        mov     al,VK_KANA              ; generate VK_KANA keyup message.
        mov     bKanaDown, 080h         ; Indicate Kana key pressed.
        jmp     kbi4

;        jmp     stdproc_end             ; No, ignore key.
;        mov     ah,ds:[bKana]           ; Get software kana state.
KanaUp_101:
        xor     ds:[bKana],080h         ; Switch internal kana state.
        mov     bKanaDown, 000h         ; Indicate Kana key pressed.
        mov     al,VK_KANA
        jmp     kbi4
noKana_101:

; The following code should be run only when DBE key generation is enable
        cmp     [DBEEnable],0
        jnz     @F
        jmp     noDBE_Vkey              ; ignore
@@:
; test for Ctrl+`(#1)
        cmp     bl,cNo1
        jne     noZenHan_101
        test    byte ptr es:[kb_flag],fCtrl
        jz      noZenHan_101
        ; Ctrl+`(#1) translated to VK_DBE_DBCSCHAR/VK_DBE_SBCSCHAR
        jmp     make_DBCS_SBCS          ;

noZenHan_101:
        cmp     bl,cCapsLock
        jz      DBE_Caps
noDBE_Caps:
        jmp     noDBE_Vkey              ;  No, ignore key.
DBE_Caps:
        test    es:[kb_flag],fAlt+fCtrl+fShift
        jz      noDBE_Caps
; ----- make VK_DBE_ROMAN/VK_DBE_NOROMAN ----
Roman_101:
        test    es:[kb_flag],fAlt
        jz      noRoman_101
        test    es:[kb_flag],fCtrl
        jz      noRoman_101
; Ctrl + Alt + CapsLock translated to VK_DBE_ROMAN/VK_DBE_NOROMAN
        jmp     make_ROMAN

noRoman_101:
        test    es:[kb_flag],fAlt
        jnz     Katakana_101
        test    es:[kb_flag],fCtrl
        jnz     Hiragana_101
        test    es:[kb_flag],fShift
        jnz     Alphanumeric_101
        jmp     noDBE_Vkey

; ----- make VK_DBE_KATAKANA ------------
Katakana_101:
        jmp     make_Katakana

; ----- make VK_DBE_HIRAGANA ------------
Hiragana_101:
        jmp     make_Hiragana

; ----- make VK_DBE_ALPHANUMERIC --------
; Shift + CapsLock translated to VK_DBE_ALPHANUMERIC
Alphanumeric_101:
        jmp     make_Alphanumeric

;-----------------------------------------------+
;                                               |
; for AX keyboard                               |
;                                               |
; Muhenkan              VK_DBE_DBCSCHAR/SBCSCHAR|
;                       VK_DBE_ROMAN/NOROMAN    |
;                       VK_DBE_CODEINPUT        |
;                       VK_DBE_NOCODEINPUT      |
;                                               |
;-----------------------------------------------+
kbi_AX:
        jmp noDBE_VKey
; it uses extended key sequences.
; The following code should be run only when DBE key generation is enable
        cmp     [DBEEnable],0
        jnz     @F
        jmp     noDBE_Vkey              ; ignore
@@:
        cmp     bl,cAxNonConvert                ;
        jz      @F                              ;
        jmp     noDBE_Vkey                      ;
@@:                                             ;
        test    byte ptr es:[kb_flag],fShift    ;
        jz      @F                              ;
;3      test    byte ptr es:[kb_flag],fCtrl     ;
;3      ; Shift + muhenkan translated to VK_DBE_DBCSCHAR/SBCSCHAR
;3      jz      ZenHan_AX                       ;
        ; Shift + Ctrl + muhenkan translated to VK_DBE_IME_WORDREGISTER
        jmp     make_WordReg                    ;
@@:
        ; Ctrl + muhenkan translated to VK_DBE_CODEINPUT/NOCODEINOUT
        test    byte ptr es:[kb_flag],fCtrl     ;
        jnz     CodeInput_AX                    ;
        jmp     noDBE_Vkey
;3ZenHan_AX:
;3      jmp     make_DBCS_SBCS
CodeInput_AX:
        jmp     make_CodeInput

;-----------------------------------------------+
;                                               |
; for A01 keyboard (OADG 106)                   |
;                                               |
; ZenHan                VK_DBE_DBCSCHAR/SBCSCHAR|
;                       VK_DBE_IME_DIALOG       |
;                                               |
; Alphanumeric          VK_DBE_ALPHANUMERIC     |
;                       VK_DBE_CODEINPUT        |
;                       VK_DBE_NOCODEINPUT      |
;                                               |
; Hiragana              VK_DBE_HIRAGANA         |
;                       VK_DBE_KATAKANA         |
;                       VK_DBE_ROMAN/NOROMAN    |
;                                               |
; Muhenkan              VK_DBE_IME_WORDREGISTER |
;                                               |
;-----------------------------------------------+
kbi_A01:
        ; old 106 driver Shift + Ctrl + Kana translated to VK_KANA

        test    gNew106, 01
        jz      @F
        cmp     bl, cCapsLock
        jnz     noKana_A01
        jmp     New106

@@:
        cmp     bl,cKatakana
        jnz     noKana_A01
New106:
        or      ah, ah                  ; make or break?
        jz      @F                      ; make                     
        cmp     bKanaDown, 080h         ; break case, see if CTRL+SHIFT+KANA keydown before?
        jz      KanaUp_a01              ; hot key succeed.        
        jmp     noKana_A01              ; only single kana key pressed.
@@:
        test    gNew106, 01
        jz      @F
        test    byte ptr es:[kb_flag],fCtrl ; New 106 uses Ctrl+Eisu.
        jz      noKana_A01
        test    byte ptr es:[kb_flag],fShift ; Exclude Ctrl+Caps as kana hotkey.
        jnz     noKana_A01

        jmp     debug_a01
@@:
        test    byte ptr es:[kb_flag],fCtrl
        jz      noKana_A01
        test    byte ptr es:[kb_flag],fShift
        jz      noKana_A01
public debug_a01
debug_a01:
;        or      ah,ah                   ; Is it make?
;        jnz     @F                      ; toggle if key up      92/08/18 yasuho
        mov     bKanaDown, 080h         ; Indicate Kana key pressed.
        mov     al,VK_KANA              ; generate VK_KANA WM_KEYDOWN message
        jmp     kbi4
;        jmp     stdproc_end             ; No, ignore key.
KanaUp_a01:
;        mov     ah,ds:[bKana]           ; Get software kana state.
        xor     ds:[bKana],080h         ; Switch internal kana state.
        mov     bKanaDown, 00h          ; Indicate Kana release state.
        mov     al,VK_KANA
        jmp     kbi4
noKana_A01:
; pass to make_Kanji        jmp noDBE_Vkey 
;        jmp noDBE_Vkey 
; The following code should be run only when DBE key generation is enable
        cmp     [DBEEnable],0
        jnz     @F
        jmp     noKana                ; ignore
@@:
;
; make VK_DBE_SBCSCHAR/VK_DBE_DBCSCHAR -----------
;
HalfFull:
        cmp     bl,cNewHalfFull                 ;2 hankaku/zenkaku key (A01) ?
        jnz     CodeInput                       ;2 no .. see next DBE key
        test    byte ptr es:[kb_flag],fAlt      ;1 Alt shift ?
        jnz     Dialog                          ;1 yes .. next DBE key
        ; HalfFull translated to VK_DBE_DBCSCHAR/VK_DBE_SBCSCHAR
        jmp     make_DBCS_SBCS
;
; make VK_DBE_IME_DIALOG ----------------------
;
Dialog:
        test    byte ptr es:[kb_flag],fCtrl
        jz      noDialog
; Ctrl+Alt+HalfFull key translated to VK_DBE_IME_DIALOG
        jmp     make_Dialog
noDialog:
        jmp     make_VK_KANJI

;
; make VK_DBE_CODEINPUT/VK_DBE_NOCODEINPUT ------
;
CodeInput:                                      ;1
        cmp     bl,cAlphanumeric                ;1 eisu/CapsLock key ?
        jnz     Hiragana                        ;1 no .. next DBE key

        test    byte ptr es:[kb_flag],fAlt      ;1 Alt shift ?
        jz      noCodeInput                     ;1 no .. next DBE key
        test    byte ptr es:[kb_flag],fCtrl     ;1 Alt shift ?
        jz      noCodeInput                     ;1 no .. next DBE key
CodeInput2:
        ; Ctrl+Alt+CapsLock is generating VK_DBE_CODEINPUT/VK_DBE_NOCODEINPUT
        jmp     make_CodeInput
noCodeInput:
;                                               ; MSKK support FakeCapsLock
        test    es:[kb_flag],fShift             ; MSKK for quick operation
        jz      CapsEnd1                        ; MSKK SR #5238 ,03/20/1993
        or      ah,ah                           ; MSKK
        js      CapsUP1                         ; MSKK
        call    FakeCapsLock                    ; MSKK
CapsUP1:                                        ; MSKK
        jmp     stdproc_end                     ; MSKK
CapsEnd1:                                       ; MSKK
;                                               ; MSKK

;
; make VK_DBE_ALPHANUMERIC ----------------------
;
Alphanumeric:
        test    es:[kb_flag],fCtrl+fShift
        jnz     noAlpha
; Alpha(CapsLock) key translated to VK_DBE_ALPHANUMERIC
        jmp     make_Alphanumeric
noAlpha:

;
; make VK_DBE_HIRAGANA --------------------------
;
Hiragana:
        cmp     bl,cNewHiragana
        jz      @F
        jmp     WordReg
@@:
        test    byte ptr es:[kb_flag],fShift
        jnz     noHiragana
        test    byte ptr es:[kb_flag],fAlt
        jz      @F
        jmp     Roman
@@:
; Hiragana key translated to VK_DBE_HIRAGANA
        jmp     make_Hiragana
noHiragana:
;
; make VK_DBE_KATAKANA --------------------------
; Shift+Hiragana key translated to VK_DBE_KATAKANA
;
Katakana:
        test    byte ptr es:[kb_flag],fCtrl
        jz      @F                              ;2
        jmp     noKana                          ;2
@@:
        test    byte ptr es:[kb_flag],fAlt
        jnz     noKatakana
        jmp     make_Katakana
noKatakana:

;
; make VK_DBE_ROMAN/VK_DBE_NOROMAN ---------------------
;
Roman:
; Ctrl+Alt+Hiragana key translated to VK_DBE_ROMAN/VK_DBE_NOROMAN
        jmp     make_Roman
noRoman:

;
; make VK_DBE_WORDREGISTER ---------------------
;
WordReg:
; Ctrl+Alt+muhenkan key translated to VK_DBE_IME_WORDREGISTER
        cmp     bl,cNonConvert
        jnz     noWordReg
        test    byte ptr es:[kb_flag],fAlt
        jz      noWordReg
        test    byte ptr es:[kb_flag],fCtrl
        jz      noWordReg
        jmp     make_WordReg
noWordReg:

;
; make VK_KANA ----------------------------------
;
;Kana_A01:
;        cmp     bl,cKatakana
;        jnz     noKana
;        or      ah,ah                   ; Is it make?
;        jnz     @F                      ; if break, make VK_KANA
;        jmp     stdproc_end             ; if make, ignore key.
;@@:
;        test    byte ptr es:[kb_flag],fAlt
;        jnz     noKana
;        test    byte ptr es:[kb_flag],fShift
;        jz      noKana
;        test    byte ptr es:[kb_flag],fCtrl
;        jz      noKana
;        mov     ah,ds:[bKana]           ; Get software kana state.
;        xor     ds:[bKana],080h         ; Switch internal kana state.
;        mov     al,VK_KANA              ; say we get VK_KANA.
;        jmp     kbi4
noKana:
        
;
; make VK_KANJI ---------------------------------
;
make_VK_KANJI:
        cmp     bl,cNewHalfFull         ; Is it Kanji key (Alt-HalfFull) ?
        jnz     noKanji                 ; No, ignore key.
;;; Sticky KANJI Hotkey problem
        cmp     bKanjiDown, 080h
        jne     @F
; It seems we are slow enough to prevent this????
;        or      ah, ah
;        jz      noKanji
        mov     bKanjiDown, 00h         ; Reset Kanji hotkey down flag.
        mov     al, VK_KANJI
        jmp     kbi4
@@:
;;;
        test    byte ptr es:[kb_flag],fShift+fCtrl
        jnz     noKanji
        test    byte ptr es:[kb_flag],fAlt
        jz      noKanji

        mov     al,VK_KANJI             ; assume kanji key.
        mov     bKanjiDown, 080h
        jmp     kbi4
public noKanji
noKanji:
;;;; 106 key has no Cap function.
; S0: original state
; S1: Cap lock press/release state
; S3: Cap lock + shift press/release state   
        cmp     bl, cCapsLock
        jne     S0
        test    es:[kb_flag], fShift
        jnz     S3
S1:
        cmp     bS3, 080h
        jne     @F
        cmp     ah, 000h
        je      S0  ; keydown repeat won't trigger state change
        mov     bS3, 00h
        jmp     S0
@@:
        mov     al, 0f0h  ; 106 key has no CapsLock behaviour.
        jmp     S0
S3:
        mov     bS3, 080h ; previous State was S3 indicator.
S0:
;;;; 106 key has no Cap function.
        jmp     noDBE_Vkey

;-----------------------------------------------+
;                                               |
; for 5576-1,2,3 keyboard                       |
;                                               |
; ZenHan                VK_DBE_DBCSCHAR/SBCSCHAR|
;                       VK_DBE_IME_DIALOG       |
;                                               |
; Alphanumeric          VK_DBE_ALPHANUMERIC     |
;                                               |
; Katakana              VK_DBE_KATAKANA         |
;                       VK_DBE_CODEINPUT        |
;                       VK_DBE_NOCODEINPUT      |
;                                               |
; Muhenkan              VK_DBE_IME_WORDREGISTER |
;                                               |
;       ----------------------------------      |
; 5576-1 only                                   |
;                                               |
; Kanji                 VK_DBE_CODEINPUT        |
;                       VK_DBE_NOCODEINPUT      |
;                                               |
; TangoToroku           VK_DBE_IME_WORDREGISTER |
;                                               |
;-----------------------------------------------+
kbi_DBCS_OLD:
        ; Ctrl + Kana translated to VK_KANA
        cmp     bl,cKatakana
        jnz     noKana_001

        or      ah, ah                  ; make or break?
        jz      @F                      ; make                     
        cmp     bKanaDown, 080h         ; break case, see if CTRL+SHIFT+KANA keydown before?
        jz      KanaUp_001              ; hot key succeed.        
        jmp     noKana_001              ; only single kana key pressed.
@@:
        test    byte ptr es:[kb_flag],fCtrl
        jz      noKana_001
;        or      ah,ah                   ; Is it make?
;        jnz     @F                      ; toggle if key up      92/08/18 yasuho
        mov     al,VK_KANA              ; Generate WM_KEYDOWN for VK_KANA.
        mov     bKanaDown, 080h         ; Indicate Kana key pressed.
        jmp     kbi4
;        jmp     stdproc_end             ; No, ignore key.
;@@:
;        mov     ah,ds:[bKana]           ; Get software kana state.
KanaUp_001:
        xor     ds:[bKana],080h         ; Switch internal kana state.
        mov     bKanaDown, 000h         ; Indicate Kana key cleared.
        mov     al,VK_KANA
        jmp     kbi4
noKana_001:

; The following code should be run only when DBE key generation is enable
        cmp     [DBEEnable],0
        jnz     @F
        jmp     Kana_OLD                ; ignore
@@:
;
; make VK_DBE_SBCSCHAR/VK_DBE_DBCSCHAR -----------
;
HalfFull_OLD:                                   ;
        cmp     bl,cHalfFull                    ;1 hankaku/zenkaku key ?
        jnz     CodeInput_OLD                   ;1 no .. see next DBE key
        test    byte ptr es:[kb_flag],fAlt      ;1 Alt shift ?
        jnz     Dialog_OLD                      ;1 yes .. next DBE key
        ; HalfFull translated to VK_DBE_DBCSCHAR/VK_DBE_SBCSCHAR
        jmp     make_DBCS_SBCS

;
; make VK_DBE_IME_DIALOG ----------------------
;
Dialog_OLD:
        test    byte ptr es:[kb_flag],fCtrl
        jz      @F
; Ctrl+Alt+HalfFull key translated to VK_DBE_IME_DIALOG
        jmp     make_Dialog
@@:
        jmp     Kanji_OLD

;
; make VK_DBE_CODEINPUT/VK_DBE_NOCODEINPUT ------
;
CodeInput_OLD:
        cmp     bl,cKanji                       ; Kanji of 5576-1 ?
        jz      CodeInput_OLD1                  ;
        cmp     bl,cKatakana                    ;1 katakana key ?
        jnz     noCode_OLD                      ;1 no .. next DBE key
        cmp     byte ptr [KeyboardSubType],KbdDBCS_OLD  ; 5576-2 keyboard ?
        jnz     noCode_OLD                              ; no  --> Katakana
CodeInput_OLD1:
        test    byte ptr es:[kb_flag],fAlt      ;1 Alt shift ?
        jz      noCode_OLD                      ;1 no
        test    byte ptr es:[kb_flag],fCtrl     ;1 Ctrl ? ?
        jz      noCode_OLD                      ;1 no
; Ctrl+Alt+Katakana translated to VK_DBE_CODEINPUT/NOCODEINPUT (5576-2/3)
; Ctrl+Alt+Kanji    translated to VK_DBE_CODEINPUT/NOCODEINPUT (5576-1)
        jmp     make_CodeInput
noCode_OLD:

;
; make VK_DBE_ALPHANUMERIC ----------------------
;
Alphanumeric_OLD:
        cmp     bl,cAlphanumeric
        jnz     @F
;MSKK   test    es:[kb_flag],fShift+fCtrl+fAlt  ; Shift+Caps --> Caps ON/OFF
        test    es:[kb_flag],fCtrl+fAlt         ; MSKK
        jnz     @F
;                                               ; MSKK support FakeCapsLock
        test    es:[kb_flag],fShift             ; MSKK for quick operation
        jz      CapsEnd2                        ; MSKK SR #5238, 03/20/1993
        or      ah,ah                           ; MSKK
        js      CapsUP2                         ; MSKK
        call    FakeCapsLock                    ; MSKK
CapsUP2:                                        ; MSKK
        jmp     stdproc_end                     ; MSKK
CapsEnd2:                                       ; MSKK
;                                               ; MSKK
; Alpha(CapsLock) key translated to VK_DBE_ALPHANUMERIC
        jmp     make_Alphanumeric
@@:

;
; make VK_DBE_KATAKANA --------------------------
; Shift+Hiragana key translated to VK_DBE_KATAKANA
;
Katakana_OLD:
        cmp     bl,cKatakana
        jnz     noKatakana_OLD
        cmp     byte ptr [KeyboardSubType],KbdDBCS_OLD  ; 5576-2 keyboard ?
        jnz     @F                                      ; no  --> Katakana
        test    byte ptr es:[kb_flag],fAlt+fCtrl+fShift
        jnz     noKatakana_OLD
        jmp     make_Katakana
@@:
        test    byte ptr es:[kb_flag],fCtrl     ; Ctrl+Katakana --> VK_KANA
        jnz     noKatakana_OLD
        jmp     make_Katakana
noKatakana_OLD:

;
; make VK_DBE_WORDREGISTER ---------------------
;
; Tango-touroku key is translated to VK_DBE_IME_WORDREGISTER
; 5576-1 keyboard only
        cmp     bl,cTango               ;
        jnz     @F                      ;
        jmp     make_WordReg            ;
@@:                                     ;
; Ctrl+Alt+muhenkan key translated to VK_DBE_IME_WORDREGISTER
        cmp     bl,cNonConvert
        jnz     @F
        test    byte ptr es:[kb_flag],fAlt
        jz      @F
        test    byte ptr es:[kb_flag],fCtrl
        jz      @F
        jmp     make_WordReg
@@:

;
; make VK_KANA ----------------------------------
;
Kana_OLD:
; MSKK  to make [VK_KANJI down] event. 01/20/1993 MasahT
        cmp     byte ptr [KeyboardSubType],KbdDBCS_OLD  ; 5576-2/3 keyboard ?
        jz      Kanji_OLD1                              ; yes .. make VK_KANJI
;; MSKK
;        jmp     stdproc_end             ; if make, ignore key.
;@@:
;        test    byte ptr es:[kb_flag],fAlt
;        jnz     noKana_OLD
;        test    byte ptr es:[kb_flag],fCtrl
;        jz      noKana_OLD
;        mov     ah,ds:[bKana]           ; Get software kana state.
;        xor     ds:[bKana],080h         ; Switch internal kana state.
;        mov     al,VK_KANA              ; say we get VK_KANA.
;        jmp     kbi4
;noKana_OLD:
;


;
; make VK_KANJI ---------------------------------
;
Kanji_OLD:
        cmp     byte ptr [KeyboardSubType],KbdDBCS_OLD  ; 5576-2/3 keyboard ?
        jz      @F                      ; 002/003 skip
        cmp     bl, cKanji              ; 001 VK_KANJI generation.
        jnz     @F
        jmp     old_kanji_up
@@:
Kanji_OLD1:                             ; 01/20/1993 MasahT  MSKK, Japan.
        cmp     bl,cKatakana            ; Is it Kanji key (Shift+Katakana) ?
        jnz     @F                      ; No, ignore key.
        cmp     bKanjiDown, 080h        ; Kanji break detected?
        jnz     old_Kanji_up_no         ; No
; It seems we are slow enough to prevent this????
;        or      ah, ah                  ; Break case?
;        jz      @F                      ; no. (No multiple VK_Kanji in WM_KEYDOWM msg.
        mov     bKanjiDown, 00          ; reset Kanji flag
        jmp     old_kanji_up

old_Kanji_up_no:            
        test    byte ptr es:[kb_flag],fAlt+fCtrl
        jnz     @F
        test    byte ptr es:[kb_flag],fShift
        jz      @F
        mov     bKanjiDown, 080h        ; Eliminate the         sticky hot key problem.
old_Kanji_up:
        mov     al,VK_KANJI             ; assume kanji key.
        jmp     kbi4                    ; ???????????
@@:
        jmp     noDBE_Vkey

;-----------------------------------------------+
;                                               |
; make DBE VKs                                  |
;                                               |
;-----------------------------------------------+

; VK_DBE_ALPHANUMERIC -------------------

make_Alphanumeric:
        or      ah,ah                   ; Is it make?
        jz      @F
        jmp     stdproc_end             ; No, ignore key
@@:
        mov     ah,ds:[bKatakana]       ; Get software Katakana state
        or      ah,ah
        jz      alpha2
; VK_DBE_KATAKATA/UP & VK_DBE_ALPHANUMERIC/DOWN
        mov     al,VK_DBE_KATAKANA
        or      bh,1
        call    ds:[event_proc]
        xor     ah,080h
        mov     ds:[bKatakana],ah
        mov     al,VK_DBE_ALPHANUMERIC
        jmp     kbi4

alpha2:
        mov     ah,ds:[bHiragana]               ; Get software Hiragana state
        or      ah,ah
        jz      alpha3
; VK_DBE_HIRAGANA/UP & VK_DBE_ALPHANUMERIC/DOWN
        mov     al,VK_DBE_HIRAGANA
        or      bh,1
        call    ds:[event_proc]
        xor     ah,080h
        mov     ds:[bHiragana],ah
alpha3:
        mov     al,VK_DBE_ALPHANUMERIC
        jmp     kbi4

; VK_DBE_HIRAGANA -----------------------

make_Hiragana:
        or      ah,ah                   ; Is it make?
        jz      @F
        jmp     stdproc_end             ; No, ignore key
@@:
        mov     ah,ds:[bKatakana]
        or      ah,ah
        jz      Hira2
; VK_DBE_KATAKANA/UP & VK_DBE_HIRAGANA/DOWN
        mov     al,VK_DBE_KATAKANA
        or      bh,1
        call    ds:[event_proc]
        mov     ds:[bHiragana],ah
        xor     ah,080h
        mov     ds:[bKatakana],ah
        mov     al,VK_DBE_HIRAGANA
        jmp     kbi4
Hira2:
; VK_DBE_ALPHANUMERIC/UP & VK_DBE_HIRAGANA/DOWN
        xor     ah,080h
        mov     al,VK_DBE_ALPHANUMERIC
        or      bh,1
        call    ds:[event_proc]
        mov     ds:[bHiragana],ah
        xor     ah,080h
        mov     al,VK_DBE_HIRAGANA
        jmp     kbi4

; VK_DBE_KATAKANA -----------------------

make_Katakana:
        or      ah,ah                   ; Is it make?
        jz      @F
        jmp     stdproc_end             ; No, ignore key
@@:
        mov     ah,ds:[bHiragana]
        or      ah,ah
        jz      Kata2
; VK_DBE_HIRAGANA/UP & VK_DBE_KATAKANA/DOWN
        mov     al,VK_DBE_HIRAGANA
        or      bh,1
        call    ds:[event_proc]
        mov     ds:[bKatakana],ah
        xor     ah,080h
        mov     ds:[bHiragana],ah
        mov     al,VK_DBE_KATAKANA
        jmp     kbi4
Kata2:
; VK_DBE_ALPHANUMERIC/UP & VK_DBE_KATAKANA/DOWN
        xor     ah,080h
        mov     al,VK_DBE_ALPHANUMERIC
        or      bh,1
        call    ds:[event_proc]
        mov     ds:[bKatakana],ah
        xor     ah,080h
        mov     al,VK_DBE_KATAKANA
        jmp     kbi4

; VK_DBE_DBCSCHAR/SBCSCHAR --------------

make_DBCS_SBCS:
        or      ah,ah                   ; Is it make?
        jz      @F
        jmp     stdproc_end             ; No, ignore key
@@:
        mov     ah,ds:[bZenHan]         ; Get software Zen/Han state
        xor     ds:[bZenHan],080h       ; Switch internal Zen/Han state
        or      ah,ah
        jnz     ZenHan1
; VK_DBE_DBCSCHAR/UP & VK_DBE_SBCSCHAR/DOWN
        xor     ah,080h
        mov     al,VK_DBE_DBCSCHAR
        xor     bh,bh
        call    ds:[event_proc]
        xor     ah,080h
        mov     al,VK_DBE_SBCSCHAR
        jmp     kbi4
ZenHan1:
; VK_DBE_SBCSCHAR/UP & VK_DBE_DBCSCHAR/DOWN
        mov     al,VK_DBE_SBCSCHAR
        xor     bh,bh
        call    ds:[event_proc]
        xor     ah,080h
        mov     al,VK_DBE_DBCSCHAR
        jmp     kbi4

; VK_DBE_ROMAN/NOROMAN ------------------

make_Roman:
        or      ah,ah                   ; Is it make?
        jz      @F
        jmp     stdproc_end             ; No, ignore key
@@:
        mov     ah,ds:[bRoman]          ; Get software Romaji state
        xor     ds:[bRoman],080h
        or      ah,ah
        jnz     Roman1

        xor     ah,080h
        mov     al,VK_DBE_ROMAN
        or      bh,1
        call    ds:[event_proc]
        xor     ah,080h
        mov     al,VK_DBE_NOROMAN
        jmp     kbi4
Roman1:
        mov     al,VK_DBE_NOROMAN
        or      bh,1
        call    ds:[event_proc]
        xor     ah,080h
        mov     al,VK_DBE_ROMAN
        jmp     kbi4

; VK_DBE_IME_WORDREGISTER ---------------

make_WordReg:
        mov     bl,al
        mov     al,VK_DBE_IME_WORDREGISTER
        jmp     kbi4

; VK_DBE_IME_DIALOG ---------------------

make_Dialog:
        mov     al,VK_DBE_IME_DIALOG
        jmp     kbi4

; VK_DBE_CODEINPUT/NOCODEINPUT ----------

make_CodeInput:
        or      ah,ah                   ; Is it make?
        jz      @F
        jmp     stdproc_end             ; No, ignore key
@@:
        push    cx                      ;
        mov     ch, VK_DBE_CODEINPUT    ; prepare virtual keys to post
        mov     cl, VK_DBE_NOCODEINPUT  ;
        xor     bCode, 80h              ; switch current state
        cmp     bCode, 0                ; which state we are in?
        jz      NowInNocode             ; jump if NOCODEINPUT mode
        xchg    ch, cl                  ; flip vk posting order
NowInNocode:
        mov     ah, 80h                 ; uptrans
        mov     al, ch                  ; uptrans key to post
        xor     bh,bh                   ; no extended key
        call    ds:[event_proc]         ; post a key
        xor     ah, ah                  ; downtrans
        mov     al, cl                  ;
        pop     cx                      ; backup CX
        jmp     kbi4                    ; post a key and bye
noDBE_Vkey:
endif ; JAPAN

CheckScanForVKey:
        public CheckScanForVKey
        push    di				; set registers and flags
        push    cx
        push    es
	pushf
	cld					; !!!!!!!
        push    ds
        pop     es

        mov     bx,     pCurrentLocale		; check base lcid list
        mov     di,     [bx+SCANTOIDX]
        mov     cx,     [bx+SCAN_SIZE]
        repnz   scasb
        jz      kbi2b

        mov     di,     dataOFFSET ScanCodes	; didnt find in pCurrent list, 
        mov     cx,     KBD_SCANS		; try base list.
        repnz   scasb
        jnz     kbi2a

	add	di, SCAN_TO_VKEY_OFFS		; found vkey in base list
        jmp     kbi2c
kbi2a:
        mov     bl,     al			; didnt find vkey anywhere!
        mov     al,     -1
        jmp     kbi2d
kbi2b:
        sub     di,     [bx+SCANTOIDX]		; move from table to table
        dec     di
        add     di,     [bx+VKEYTOIDX]
kbi2c:
        mov     bl,     al			; put back scan code
        mov     al,     [di]			; get vkey
kbi2d:
        xor     bh,     bh			; clear extended bit

	popf					; restore flags
        pop     es
        pop     cx
        pop     di
        cmp     al,     -1
        je      kbi4

Translated:

ifdef   KOREA
	cmp	al, VK_OEM_PLUS
	jne	HG10
        test    byte ptr es:[kb_flag], fAlt
	jz	HG10
        or      ah, ah
        js      HG05                    ; eat Up-key

        mov     ax, VK_JUNJA
        call    ds:[event_proc]
	jmp	stdproc_end
;;;        call    send_softkey
HG05:
        jmp     stdproc_end
HG10:

endif   ; KOREA
;
;================================================
; ====== Handle keys on numeric pad ============
;================================================
;
CheckNumPad:

        xor     bh,bh         ; is it on numeric keypad?
        cmp     bl,71 
        jb      kbi4
        cmp     bl,83
        ja      kbi4          ; nope! 

public isNumPad
isNumPad:
        ; yes...  This is the numeric pad.
        ; Here, if NumLock is set, we change the virtual keycodes to
        ; numeric VK_NUMPAD codes, so the keys will be translated
        ; as numbers etc.  But if a shift key is down, we handle
        ; these as cursor keys, but we need to make sure that these
        ; are seen as UNSHIFTED
        
        test    byte ptr es:[kb_flag],fNum      ; is num lock down?
        jz      kbi4                    ; no, do normal processing
        
        test    byte ptr es:[kb_flag],fShift ; either shift key down?
        jnz     kbi3                    ; yes, keep as cursor keys
                                        ; no, treat as numeric keys.

        ; Now we prepare to translate as a NUMERIC key
        
        mov     al,bh
        or      al,al
        jnz     kbi4

        push    si
        mov     si,KeyNumBase

        mov     al,ds:[bx+si-71]        ; new: movable table
        pop     si

        jmp     short kbi4

; The key is on the numeric pad, NumLock is set, but a shift key is down 
; (bit(s) set in BIOS key state), so we are going to keep this as a cursor 
; key.  To do this, we need to make sure that Windows' state vector entry 
; for VK_SHIFT is OFF even though a shift key is actually down. To protect
; against nested interrupt reentrancy problems we hit the interrupt flag,
; because LastCursor is used as a binary semaphore in keybd_int().

kbi3:
        cli                             ; protect numpad semaphore 
        cmp     bl, LastCursor          ; are we repeating?
        mov     LastCursor, bl          ; save the scan code anyway
        sti
        je      kbi3a                   ; if so, we just send the key
                                        ; as a flag and turn off VK_SHIFT.
;
;  A SHIFT-up message is sent only once for every numpad cursor key that is 
;  different from the previous one (repeats of the same key do not result in
;  repeated SHIFT-up messages).
;
;  ** Problem ** If the shift key is released (by means of a nested interrupt) 
;  then SHIFT up/down messages would be out of sequence - the UP message here 
;  is benign, but the premature DOWN message that would result is catered for 
;  within keybd_int(). So we ignore the problem here, however, LastCursor was
;  treated as an atomic binary semaphore above.
;                                        
        push    ax
        push    bx
        mov     ax,VK_SHIFT+8000H       
        mov     bl,54
        call    ds:[event_proc]    ; fake shift UP.
        
        pop     bx
        pop     ax                 ; restore original key now and let
        jmp     short kbi4         ; USER see that one also.               

;--------------------------------------------------------------------
;
; This is the same cursor key --  turn VK_SHIFT back on and
; clear the flag if the cursor key is being released (break)

kbi3a:
        test    ah,80h     ; break?  If not, keep shift OFF
        jz      kbi4       ; and send actual cursor key now.
;
; Repeating key is breaking so follow the cursor key message with a make
; believe shift key-down message. 
;
; ** Problem ** A potential reentrancy problem may occur here from a nested 
; interrupt call to standard_proc. Prior to standard_proc being called from 
; keybd_int() the potential loss of SHIFT-up was catered for, however, if 
; that occurred first then we would have an extra SHIFT-down message here 
; without an accompanying UP message. We can tell by checking shift is still 
; down. The best solution entails a short hit on the interrupt system.
;
        cli
        mov     LastCursor, 0                ; end of cursor logic
        test    byte ptr es:[kb_flag],fShift ; either shift key down?
        sti
        je     kbi4    ; no, we got hit - throw away actual cursor key.
;
; We send this SHIFT-down message knowing that keybd_int() could never send
; another SHIFT-down message inadvertently (because LastCursor = 0).
;                
        call    ds:[event_proc]     ; send actual cursor key now...
        mov     ax,VK_SHIFT         ; pretend shift key went down
        mov     bl,54
;
kbi4:
        public kbi4
        
ifdef   ENVOY

;!!! if Vectra, down transition and not a modifier or a lock, key click

        test    ds:fVectra, FV_A        ; 23 jun 88 peterbe, was CMP
        jz      no_click
        cmp     ds:[fModifier], 0
        je      no_click
        cmp     bl, cCapsLock
        je      no_click
        cmp     bl, cNumLock
        je      no_click
        cmp     bl, cBreak
        je      no_click
        cmp     ah, 0
        jne     no_click
        push    ax
        call    ds:[event_proc]
        pop     ax
;       push    bp
;       mov     ah, F_SND_CLICK
;       mov     bp, V_SYSTEM
;       int     6fh
;       pop     bp
        jmp     stdproc_end
no_click:

endif   ;ENVOY
;
;-----------------------------------------------------------
; Call windows with ah == 80h for uptrans, 00h for downtrans
; al = virtual key, bl = scan code
; bh = 0 (except for ICO extended keys, and NumLock)
;
; Windows preserves all registers
;


        xor     bh,bh                   ; clear and
        xchg    bh,fExtend              ; get extended key flag
        call    ds:[event_proc]

stdproc_end:

        ret

standard_proc endp


;*****************************************************************************
;******************************* FakeCapsLock ********************************
;*****************************************************************************
ifdef ENHANCE

; For Shift Lock, fake Caps Lock key being depressed.
; This is called when the caps lock or shift key is pressed,
; and the value of fCaps flag at 40:17h is to be changed.

        Public FakeCapsLock

FakeCapsLock proc near

ifdef JAPAN
        test    gNew106, 1  ; New 106 hot key CTRL+EISU will exclude CTRL+CAPS case.       
        jz      @F
        test    es:[kb_flag], fCtrl
        jz      @F
        ret
@@:
endif
        push    ax                      ; fake CAPSLOCK key going ..
        push    bx
        mov     bx,cCapsLock            ; load capslock scan code.
        mov     ax,VK_CAPITAL           ; .. down and ..
        call    ds:[event_proc]
        mov     ax,VK_CAPITAL+8000H     ; .. up.
        call    ds:[event_proc]
        pop     bx
        pop     ax                      ; continue processing SHIFT key..

        ret

FakeCapsLock endp

endif ; ENHANCE

ifdef ENHANCE
;*****************************************************************************
;***************************** prev_was_ext_proc *****************************
;*****************************************************************************
;
; prev_was_ext_proc -- used when previous was extended prefix (E0)
;
; used only by RT keyboard
;
;       Input   AL= Scancode (without make/break bit)
;               AH= 00 if make, 80h if break
;               BH=0
;
;       Mainly undoes Shifting and Unshifting generated internally
;       by this (@#$) keyboard and uses the extended prefixes to
;       distingush between normal keys and new (RT) keys.
;

        Public prev_was_ext_proc

prev_was_ext_proc proc near

        mov     ds:[kbd_proc], codeOFFSET standard_proc
        sti     ; enable interrupts for others
        
        cmp     al,cLShift          ; is it extended Left Shift ?
        jz      LeavePrevProc        ; if yes, eat it
        cmp     al,cRShift          ; else is it extended Right Shift ?
        jz      LeavePrevProc        ; if yes, eat it

; Test if we receive PrintScreen. Remember the make code has been eaten
; before we get a chance to be called. The break code will release the
; temporarily LeftShift that was then set.
        cmp     al,cPrint
        jnz     prev10

LeavePrevProc:                          ; Add label 04dec87
        jmp     prevproc_end

prev10:
ifdef   KOREA   ; for TG 101/102 Hangeul keyboard
        cmp     byte ptr [KeyboardSubType],Kbd101s
        je      @F
        cmp     byte ptr [KeyboardSubType],Kbd103
        je      @F
        
	cmp	ds:[fTGkeybd], 0
        jne     okTGkeybd
        cmp     al,cAlt
	jz	noSetShiftState
okTGkeybd:
@@:

endif   ;KOREA

ifdef JAPAN
        mov     bl,al                   ; bl= scancode (all along proc)
ifdef TOSHIBA
	cmp	byte ptr [KeyboardSubType],KbdJ31DT	;J-3100 keyboard?
	jb	noPrevDBE				; no
	cmp	byte ptr [KeyboardSubType],KbdJ31SP	;J-3100 keyboard?
	ja	noPrevDBE				; no
	jmp	prev_J31				; yes
endif
        cmp     byte ptr [KeyboardSubType],KbdAX        ;2 AX keyboard ?
        jz      prev_AX                                 ;2 yes
        cmp     byte ptr [KeyboardSubType],KbdDBCS_OLD1
        ja      @F
        cmp     byte ptr [KeyboardSubType],KbdA01       ; Type 1,2,3 keyboard ?
        ja      prev_OLD                                ; yes
@@:
        jmp     noPrevDBE
prev_OLD:
        jmp     prev_DBCS_OLD
ifdef TOSHIBA
;---------------------------------------------------------------+
;                                                               |
; J-3100 keyboard has two extended keys.			|
; We should treat them here.					|
;       kana key (E0-1D) should be a virtual key VK_KANA.       |
;       kanji key (E0-38) should be a virtual key VK_KANJI.     |
;                                                               |
;---------------------------------------------------------------+
prev_J31:
	cmp	al,cCtrl			; Is it kana key (E0-1D) ?
	jz	prJ31_isKana			; yes
	jmp	prJ31_noKana			; no
prJ31_isKana:
;;; Head append for Make VK_KANA even if [DBEEnable]=0 by Chym.
	test	es:[kb_flag],fAlt+fCtrl+fShift	; Any shifted?
	jnz	@F				; yes
	jmp	prJ31_noHiragana		; no, make VK_KANA
@@:
;;; Tail append for Make VK_KANA even if [DBEEnable]=0 by Chym.
	cmp	[DBEEnable],0			; DBEEnable=0?
	jnz	@F				; yes
	jmp	prJ31_Continue			; no
@@:
	test	es:[kb_flag],fAlt
	jz	prJ31_noAlpha
; Alt+Kana key translated to VK_DBE_ALPHANUMERIC
	or	ah,ah				; Is it make?
	jz	@F
	jmp	prJ31_Ignore			; no, ignore key.
@@:
	mov	ah,ds:[bKatakana]		; Get software Katakana state
	or	ah,ah
	jz	prJ31_Alpha2
; VK_DBE_KATAKATA/UP & VK_DBE_ALPHANUMERIC/DOWN
	mov	al,VK_DBE_KATAKANA
	or	bh,1
	call	ds:[event_proc]
	xor	ah,080h
	mov	ds:[bKatakana],ah
	mov	al,VK_DBE_ALPHANUMERIC
	jmp	prev40
prJ31_Alpha2:
	mov	ah,ds:[bHiragana]		; Get software Hiragana state
	or	ah,ah
	jz	prJ31_Alpha3
; VK_DBE_HIRAGANA/UP & VK_DBE_ALPHANUMERIC/DOWN
	mov	al,VK_DBE_HIRAGANA
	or	bh,1
	call	ds:[event_proc]
	xor	ah,080h
	mov	ds:[bHiragana],ah
prJ31_Alpha3:
	mov	al,VK_DBE_ALPHANUMERIC
	jmp	prev40
prJ31_noAlpha:

	test	byte ptr es:[kb_flag],fCtrl
	jz	prJ31_noRoman
	test	byte ptr es:[kb_flag],fShift
	jz	@F
	jmp	prJ31_Kana
@@:
; Ctrl+Kana key translated to VK_DBE_ROMAN/VK_DBE_NOROMAN
	jmp	prJ31_Roman
prJ31_noRoman:

	test	byte ptr es:[kb_flag],fShift
	jz	prJ31_noHiragana
	or	ah,ah				; Is it make?
	jz	@F
	jmp	prJ31_Ignore			; no, ignore key.
@@:
	mov	ah,ds:[bKatakana]
	or	ah,ah
	jz	prJ31_Hira2
; VK_DBE_KATAKANA/UP & VK_DBE_HIRAGANA/DOWN
	mov	al,VK_DBE_KATAKANA
	or	bh,1
	call	ds:[event_proc]
	mov	ds:[bHiragana],ah
	xor	ah,080h
	mov	ds:[bKatakana],ah
	mov	al,VK_DBE_HIRAGANA
	jmp	prev40
prJ31_Hira2:
; VK_DBE_ALPHANUMERIC/UP & VK_DBE_HIRAGANA/DOWN
	xor	ah,080h
	mov	al,VK_DBE_ALPHANUMERIC
	or	bh,1
	call	ds:[event_proc]
	mov	ds:[bHiragana],ah
	xor	ah,080h
	mov	al,VK_DBE_HIRAGANA
	jmp	prev40
prJ31_noHiragana:
; KANA key translated to VK_KANA
;;; fixed to be toggle kana. -by Chym. 1995-04-05
;;;	or	ah,ah				; Is it make or break?
;;;	jnz	@F				; if break, make VK_KANA
;;;	jmp	prJ31_Ignore			; if make, ignore it
;;;@@:
;;;	mov	ah,ds:[bKana]			; Get software kana state.
;;;	xor	ds:[bKana],080h			; Switch internal kana state.

;
; we won't use bKanaDown flag, because our keyboard toggles KANA with just
; one key. Both make and break code make VK_KANA, but bKana flag must be
; switched by break code.
;
        or      ah,ah                           ; Is it make or break?
        jz      @F                              ; make
        mov     kanamlock, 0                    ; unlock toshiba kana repeat lock 
        jmp     cont_nothing                    ; do nothing.

@@:
        test    kanamlock, 1                    ; repeat kana make?
        jnz     cont_nothing                    ; Yes, skip bkana modification
        mov     kanamlock, 1                    ; No, set flags
        xor     ds:[bKana],080h                 ; if make, switch bKana (TOSHIBA special)
cont_nothing:

;;; fixed end
	mov	al,VK_KANA			; say we get VK_KANA.
	jmp	prev40				; and process it.

prJ31_Kana:
; Ctrl+Shift+KANA key translated to VK_DBE_KATAKANA
	or	ah,ah				; Is it make?
	jz	@F
	jmp	prJ31_Ignore			; no, ignore key.
@@:
	mov	ah,ds:[bHiragana]
	or	ah,ah
	jz	prJ31_Kata2
; VK_DBE_HIRAGANA/UP & VK_DBE_KATAKANA/DOWN
	mov	al,VK_DBE_HIRAGANA
	or	bh,1
	call	ds:[event_proc]
	mov	ds:[bKatakana],ah
	xor	ah,080h
	mov	ds:[bHiragana],ah
	mov	al,VK_DBE_KATAKANA
	jmp	prev40
prJ31_Kata2:
; VK_DBE_ALPHANUMERIC/UP & VK_DBE_KATAKANA/DOWN
	xor	ah,080h
	mov	al,VK_DBE_ALPHANUMERIC
	or	bh,1
	call	ds:[event_proc]
	mov	ds:[bKatakana],ah
	xor	ah,080h
	mov	al,VK_DBE_KATAKANA
	jmp	prev40

prJ31_noKana:
	cmp	al,cAlt				; Is it Kanji key (E0-38) ?
	jz	prJ31_isKanji			; yes
	jmp	prJ31_Continue			; no, ignore key.
prJ31_isKanji:
;;; Head append for Make VK_KANJI even if [DBEEnable]=0 by Chym.
	test	es:[kb_flag],fAlt+fCtrl+fShift	; Any shifted?
	jnz	@F				; yes
	jmp	prJ31_noDialog			; no, make VK_KANJI
@@:
;;; Tail append for Make VK_KANJI even if [DBEEnable]=0 by Chym.
	cmp	[DBEEnable],0
	jnz	@F
	jmp	prJ31_noDialog
@@:
	test	byte ptr es:[kb_flag],fShift
	jz	prJ31_noDialog
	test	byte ptr es:[kb_flag],fCtrl
	jz	prJ31_HalfFull
; Ctrl+Shift+KANJI key translated to VK_DBE_IME_DIALOG
	mov	al,VK_DBE_IME_DIALOG
	jmp	prev40

prJ31_HalfFull:
; Shift+KANJI key translated to VK_DBE_DBCSCHAR/SBCSCHAR
	or	ah,ah				; Is it make?
	jz	@F
	jmp	prJ31_Ignore			; if make, ignore key.
@@:
	mov	ah,ds:[bZenHan]			; Get software Zen/Han state
	xor	ds:[bZenHan],080h		; Switch internal Zen/Han state
	or	ah,ah
	jnz	prJ31_ZenHan
; VK_DBE_DBCSCHAR/UP & VK_DBE_SBCSCHAR/DOWN
	xor	ah,080h
	mov	al,VK_DBE_DBCSCHAR
	xor	bh,bh
	call	ds:[event_proc]
	xor	ah,080h
	mov	al,VK_DBE_SBCSCHAR
	jmp	prev40
prJ31_ZenHan:
; VK_DBE_SBCSCHAR/UP & VK_DBE_DBCSCHAR/DOWN
	mov	al,VK_DBE_SBCSCHAR
	xor	bh,bh
	call	ds:[event_proc]
	xor	ah,080h
	mov	al,VK_DBE_DBCSCHAR
	jmp	prev40
prJ31_Roman:
	or	ah,ah				; Is it make?
	jz	@F
	jmp	prJ31_Ignore			; no, ignore key.
@@:
	mov	ah,ds:[bRoman]			; Get software Romanji state
	xor	ds:[bRoman],080h
	or	ah,ah
	jnz	J31Roman1
	xor	ah,080h
	mov	al,VK_DBE_ROMAN
	or	bh,1
	call	ds:[event_proc]
	xor	ah,080h
	mov	al,VK_DBE_NOROMAN
	jmp	prev40
J31Roman1:
	mov	al,VK_DBE_NOROMAN
	or	bh,1
	call	ds:[event_proc]
	xor	ah,080h
	mov	al,VK_DBE_ROMAN
	jmp	prev40

prJ31_noDialog:
	mov	al,VK_KANJI			; assume kanji key.
	jmp	prev40

; Ignore this key
prJ31_Ignore:
	jmp	prevproc_end

prJ31_End:
endif ;TOSHIBA

;---------------------------------------------------------------+
;                                                               |
; AX keyboard has two extended keys. We should treat them here. |
;       kana key (E0-1D) should be a virtual key VK_KANA.       |
;       kanji key (E0-38) should be a virtual key VK_KANJI.     |
;                                                               |
;---------------------------------------------------------------+
prev_AX:
        cmp     al,cCtrl                ; Is it kana key (E0-1D) ?
        jz      @F                      ;
        jmp     noAxKana                ;
@@:                                     ;
        cmp     [DBEEnable],0           ;
        jnz     @F                      ;
;3      jmp     noAxHiragana            ;
        jmp     AxKana                  ;3
@@:
        test    es:[kb_flag],fAlt       ;3
        jz      noAXalpha               ;3
; Alt+Kana key translated to VK_DBE_ALPHANUMERIC
        or      ah,ah                   ;3 Is it make?
        jz      @F                      ;3
        jmp     LeavePrevProc           ;3 No, ignore key.
@@:                                     ;3
        mov     ah,ds:[bKatakana]       ;3 Get software Katakana state
        or      ah,ah                   ;3
        jz      AXalpha2                ;3
; VK_DBE_KATAKATA/UP & VK_DBE_ALPHANUMERIC/DOWN
        mov     al,VK_DBE_KATAKANA      ;3
        or      bh,1                    ;3
        call    ds:[event_proc]         ;3
        xor     ah,080h                 ;3
        mov     ds:[bKatakana],ah       ;3
        mov     al,VK_DBE_ALPHANUMERIC  ;3
        jmp     prev40                  ;3
AXalpha2:                               ;3
        mov     ah,ds:[bHiragana]       ;3 Get software Hiragana state
        or      ah,ah                   ;3
        jz      AXalpha3                ;3
; VK_DBE_HIRAGANA/UP & VK_DBE_ALPHANUMERIC/DOWN
        mov     al,VK_DBE_HIRAGANA      ;3
        or      bh,1                    ;3
        call    ds:[event_proc]         ;3
        xor     ah,080h                 ;3
        mov     ds:[bHiragana],ah       ;3
AXalpha3:                               ;3
        mov     al,VK_DBE_ALPHANUMERIC  ;3
        jmp     prev40                  ;3
noAXalpha:                              ;3

        test    byte ptr es:[kb_flag],fCtrl     ;3
;        jz      noAxRoman                       ;3
        jz      noPrevDBE
        test    byte ptr es:[kb_flag],fShift    ;3
        jz      @F                              ;3
        jmp     AxKana                          ;3
@@:      
;        jz      noAxRoman                       
        jmp     noPrevDBE                        
; Ctrl+Kana key translated to VK_DBE_ROMAN/VK_DBE_NOROMAN       ;3
        jmp     prev_Roman
noAxRoman:

        test    byte ptr es:[kb_flag],fShift    ;3
        jz      noAxHiragana                    ;3
        or      ah,ah                   ;3 Is it make?
        jz      @F                      ;3
        jmp     LeavePrevProc           ;3 No, ignore key.
@@:                                     ;3
        mov     ah,ds:[bKatakana]       ;3
        or      ah,ah                   ;3
        jz      AxHira2                 ;3
; VK_DBE_KATAKANA/UP & VK_DBE_HIRAGANA/DOWN
        mov     al,VK_DBE_KATAKANA      ;3
        or      bh,1                    ;3
        call    ds:[event_proc]         ;3
        mov     ds:[bHiragana],ah       ;3
        xor     ah,080h                 ;3
        mov     ds:[bKatakana],ah       ;3
        mov     al,VK_DBE_HIRAGANA      ;3
        jmp     prev40                  ;3
AxHira2:                                ;3
; VK_DBE_ALPHANUMERIC/UP & VK_DBE_HIRAGANA/DOWN
        xor     ah,080h                 ;3
        mov     al,VK_DBE_ALPHANUMERIC  ;3
        or      bh,1                    ;3
        call    ds:[event_proc]         ;3
        mov     ds:[bHiragana],ah       ;3
        xor     ah,080h                 ;3
        mov     al,VK_DBE_HIRAGANA      ;3
        jmp     prev40                  ;3
NoAxHiragana:                           ;3

        or      ah,ah                   ;3 Is it make?
        jz      @F                      ;3
        jmp     LeavePrevProc           ;3 No, ignore key.
@@:                                     ;3
        mov     ah,ds:[bHiragana]       ;3
        or      ah,ah                   ;3
        jz      AxKata2                 ;3
; VK_DBE_HIRAGANA/UP & VK_DBE_KATAKANA/DOWN
        mov     al,VK_DBE_HIRAGANA      ;3
        or      bh,1                    ;3
        call    ds:[event_proc]         ;3
        mov     ds:[bKatakana],ah       ;3
        xor     ah,080h                 ;3
        mov     ds:[bHiragana],ah       ;3
        mov     al,VK_DBE_KATAKANA      ;3
        jmp     prev40                  ;3
AxKata2:                                ;3
; VK_DBE_ALPHANUMERIC/UP & VK_DBE_KATAKANA/DOWN
        xor     ah,080h                 ;3
        mov     al,VK_DBE_ALPHANUMERIC  ;3
        or      bh,1                    ;3
        call    ds:[event_proc]         ;3
        mov     ds:[bKatakana],ah       ;3
        xor     ah,080h                 ;3
        mov     al,VK_DBE_KATAKANA      ;3
        jmp     prev40                  ;3
NoAxKatakana:                           ;3

AxKana:                                         ;3
; Ctrl+Shift+KANA key translated to VK_KANA     ;3
        or      ah, ah                  ; make or break?
        jz      @F                      ; make                     
        cmp     bKanaDown, 080h         ; break case, see if CTRL+SHIFT+KANA keydown before?
        jz      KanaUp_ax               ; hot key succeed.        
        jmp     prev60                  ; only single kana key pressed.
@@:
        test    byte ptr es:[kb_flag],fShift    ;3
        jz      prev60                          ;3
        test    byte ptr es:[kb_flag],fCtrl     ;3
        jz      prev60                          ;3
public debug_ax
debug_ax:
;        or      ah,ah                   ; Is it make?
;        jnz     @F                      ; if break, make VK_KANA
        mov     bKanaDown, 080h         ; Indicate Kana key cleared.
        mov     al,VK_KANA              ; say we get VK_KANA.
        jmp     prev60                  ; and process it.
;         jmp     LeavePrevProc           ; if make, ignore key. Because Scan Code Kana=fCtrl!!!
;        jmp     noPrevDBE               ; Continue our journey.
;@@:                                     ;
;        mov     ah,ds:[bKana]           ; Get software kana state.
KanaUp_ax:
        xor     ds:[bKana],080h         ; Switch internal kana state.
        mov     bKanaDown, 000h         ; Indicate Kana key cleared.
        call    SetLEDsAX
;;;;        jmp     noPrevDBE
        mov     al,VK_KANA              ; say we get VK_KANA.
        jmp     prev60                  ; and process it.
noAxKana:                               ;
        cmp     bl,cAlt                 ; Is it Kanji key (E0-38) ?
        jz      @F                      ; YES
         jmp     noPrevDBE               ; No, ignore key.
;         jmp     LeavePrevProc           ; if make, ignore key. Because Scan Code Kana=fCtrl!!!

@@:     
;        mov al, VK_KANJI
;        jmp prev60

        cmp     [DBEEnable],0           ;
        jz      noAxDialog              ;
        test    byte ptr es:[kb_flag],fShift    ;
        jz      noAxDialog                      ;
        test    byte ptr es:[kb_flag],fCtrl     ;
;3      jz      noAxDialog                      ;
        jz      AxHalfFull                      ;3
; Ctrl+Shift+KANJI key translated to VK_DBE_IME_DIALOG
        mov     al,VK_DBE_IME_DIALOG    ;
        jmp     prev40                  ;

AxHalfFull:                             ;3
; Shift+KANJI key translated to VK_DBE_DBCSCHAR/SBCSCHAR
        or      ah,ah                   ;3 Is it make?
        jz      @F                      ;3
        jmp     LeavePrevProc           ;3 if make, ignore key.
@@:                                     ;3
        mov     ah,ds:[bZenHan]         ;3 Get software Zen/Han state
        xor     ds:[bZenHan],080h       ;3 Switch internal Zen/Han state
        or      ah,ah                   ;3
        jnz     AxZenHan1               ;3
; VK_DBE_DBCSCHAR/UP & VK_DBE_SBCSCHAR/DOWN
        xor     ah,080h                 ;3
        mov     al,VK_DBE_DBCSCHAR      ;3
        xor     bh,bh                   ;3
        call    ds:[event_proc]         ;3
        xor     ah,080h                 ;3
        mov     al,VK_DBE_SBCSCHAR      ;3
        jmp     prev40                  ;3
AxZenHan1:                              ;3
; VK_DBE_SBCSCHAR/UP & VK_DBE_DBCSCHAR/DOWN
        mov     al,VK_DBE_SBCSCHAR      ;3
        xor     bh,bh                   ;3
        call    ds:[event_proc]         ;3
        xor     ah,080h                 ;3
        mov     al,VK_DBE_DBCSCHAR      ;3
        jmp     prev40                  ;3

noAxDialog:                             ;
        mov     al,VK_KANJI             ; assume kanji key.
        jmp     prev60                  ; and process it.

noAxKanji:                              ;

;-----------------------------------------------+
;                                               |
; for 5576-1,2,3 keyboard                       |
;                                               |
; Hiragana              VK_DBE_HIRAGANA         |
;                       VK_DBE_ROMAN/NOROMAN    |
;                                               |
;-----------------------------------------------+
prev_DBCS_OLD:
        cmp     [DBEEnable],0
        jnz     OldHiragana
        jmp     noPrevDBE
;
; make VK_DBE_HIRAGANA --------------------------
;
OldHiragana:
        cmp     bl,cHiragana
        jz      @F
        jmp     noPrevDBE
@@:
        or      ah,ah                   ; Is it make?
        jz      @F
        jmp     LeavePrevProc           ; No, ignore key.
@@:
;2      test    byte ptr es:[kb_flag],fShift
;2      jz      @F                      ;2
;2      jmp     LeavePrevProc           ;2; No, ignore key.
;@@:                                    ;2
        test    byte ptr es:[kb_flag],fAlt
        jz      @F
        jmp     OldRoman
@@:
; Hiragana key translated to VK_DBE_HIRAGANA
        mov     ah,ds:[bKatakana]
        or      ah,ah
        jz      OldHira2
; VK_DBE_KATAKANA/UP & VK_DBE_HIRAGANA/DOWN
        mov     al,VK_DBE_KATAKANA
        or      bh,1
        call    ds:[event_proc]
        mov     ds:[bHiragana],ah
        xor     ah,080h
        mov     ds:[bKatakana],ah
        mov     al,VK_DBE_HIRAGANA
        jmp     prev40
OldHira2:
; VK_DBE_ALPHANUMERIC/UP & VK_DBE_HIRAGANA/DOWN
        xor     ah,080h
        mov     al,VK_DBE_ALPHANUMERIC
        or      bh,1
        call    ds:[event_proc]
        mov     ds:[bHiragana],ah
        xor     ah,080h
        mov     al,VK_DBE_HIRAGANA
        jmp     prev40
noOldHiragana:

;
; make VK_DBE_ROMAN/VK_DBE_NOROMAN ---------------------
;
OldRoman:
;;      test    byte ptr es:[kb_flag],fShift
;;      jz      noRoman
; Ctrl+Alt+Hiragana key translated to VK_DBE_ROMAN/VK_DBE_NOROMAN
prev_Roman:
        or      ah,ah                   ; Is it make?
        jz      @F                      ;
        jmp     LeavePrevProc           ;  No, ignore key.
@@:                                     ;
        mov     ah,ds:[bRoman]          ; Get software Romaji state
        xor     ds:[bRoman],080h
        or      ah,ah
        jnz     OldRoman1

        xor     ah,080h
        mov     al,VK_DBE_ROMAN
        or      bh,1
        call    ds:[event_proc]
        xor     ah,080h
        mov     al,VK_DBE_NOROMAN
        jmp     prev40
OldRoman1:
        mov     al,VK_DBE_NOROMAN
        or      bh,1
        call    ds:[event_proc]
        xor     ah,080h
        mov     al,VK_DBE_ROMAN
        jmp     prev40
noOldRoman:

noPrevDBE:

ifdef TOSHIBA
prJ31_Continue:
endif

endif ; JAPAN

        push    bx
        call    SetShiftStateNoNumlock
        pop     bx

ifndef JAPAN                            ; it's been done before.
        mov     bl,al                   ; bl= scancode (all along proc)
endif
ifdef KOREA
        cmp     byte ptr [KeyboardSubType],Kbd101S
        jne     @F

	cmp	al, cCtrl
	jnz @F
    and byte ptr es:[kb_flag_1], 0feh ;clear bit 0 of 40:18
@@:
noSetShiftState:
        cmp     byte ptr [KeyboardSubType],Kbd101a
        jne     HG00

;
;	In 101/102 key keyboard, right hand ALT (E0-38) and CTRL (E0-1D)
;	should be VK_HANGEUL and VK_HANJA. 
;
;;;     or      ah, ah                  ; Is it make?
;;;     jnz     HG00                    ;  No, continue
	cmp	al, cAlt		; Is it HANGEUL key (E0-38) ?
	jnz	noHangeul
	mov	al, VK_HANGEUL		; say we get VK_HANGEUL
	jmp	short prev40		; and process it
noHangeul:
	mov	al, VK_HANJA		; assume kanji key
	cmp	bl, cCtrl		; Is it Hanja key (E0-1D)?
	jz	prev40			;  Yes, process it.
	mov	al, bl			; restore scan code.
HG00:
        cmp     byte ptr [KeyboardSubType],Kbd101b
        jne     HG000

;
;	In 101/102 key keyboard, right hand ALT (E0-38) and CTRL (E0-1D)
;	should be VK_HANJA and VK_HANGEUL. 
;
;;;     or      ah, ah                  ; Is it make?
;;;     jnz     HG000                   ;  No, continue
        cmp     al, cAlt                ; Is it HANGEUL key (E0-1D) ?
        jnz     noHangeul1
        mov     al, VK_HANJA            ; say we get VK_HANGEUL
	jmp	short prev40		; and process it
noHangeul1:
        mov     al, VK_HANGEUL          ; assume kanji key
        cmp     bl, cCtrl               ; Is it Hanja key (E0-38)?
	jz	prev40			;  Yes, process it.
	mov	al, bl			; restore scan code.
HG000:
endif   ; KOREA

;
; Divide key special case: if Slash is found a VK_DIVIDE is sent
;
        cmp     al,cSlash               ; is it extended Slash ?
        jnz     prev20                  ; if not, skip
        mov     al,VK_DIVIDE            ; else send VK_DIVIDE
        jmp     short prev40

; When ShiftLock is on, we don't want to be get our VK_code shifted (none
; of our code correspond to alphanumeric), so we have nothing to do.
; We would have one exception (VK_DIVIDE), but we already took care of it.

; Break case
;
prev20:
        cmp     al,cBreak               ; is it extended Break ?
        jnz     prev30                  ; if not, skip
        mov     ds:[fBreak],0
        test    ah,80h                  ; only signal for up transitions!
        jz      @F
@@:     
        mov     al,VK_CANCEL            ; always do VK_CANCEL
        JMP     SHORT PREV40            ; 07 sep 89.. don't stuff below..

; Standard case, convert to our virtual key
;
prev30:
        push    di
        push    es
        push    cx
	pushf
	cld
        push    ds
        pop     es

        lea     di, ScanCodes
        mov     cx, KBD_SCANS
        mov     al, bl              ; get the scan code
        repnz   scasb
        jz      @F
        mov     al, -1
        jmp     prev35
@@:
	add	di, SCAN_TO_VKEY_OFFS
        mov     al, [di]
prev35:
	popf
        pop     cx
        pop     es
        pop     di

        errn$   prev40  ; causes phase error if MASM size error
;
prev40:
;
; If flag is set for AltGr (right hand ALT) handled as control-ALT,
; then insert a CONTROL key call to event_proc before the ALT key call,
; but only if the BIOS CONTROL key flag is false (got that?).
; (15 oct 87: add code to prevent repeats on AltGr key, which were putting
; strange things in Windows' input buffer.)
;
        cmp     bl,cAlt                 ; was this an ALT key?
        jne     prev60                  ; skip on if not..


        test    ds:[fKeyType],kbAltGr   ; does this KB do this?
        jz      prev60                  ; skip on if not..
        test    byte ptr es:[kb_flag],fCtrl     ; is the real control key down?
        jnz     prev60                  ; if so, don't bother..
                                        ; the following code prevents repeats
        or      ah,ah                   ; on AltGr key depressions.
        jns     FakeAltGrDown           ; up or down?
        mov     fAltGrDn,0              ; up, clear flag
        jmp     short DoFakeAltGr       ; go fake control-Alt release

FakeAltGrDown:
        cmp     fAltGrDn,0              ; down, is this a repeat?
        jnz     prevproc_end            ; if so, ignore it.
        mov     fAltGrDn,1              ; otherwise fake control-Alt depression

DoFakeAltGr:
        push    ax                      ; OK, let's fake a Control key
        push    bx                      ; (but not set kb_flag bit!!)
        mov     al,VK_CONTROL           ; [AH] tells whether fake ctrl key
        mov     bx,cCtrl                ;  is going up or down.. (bh = 0)
        call    ds:[event_proc]         ; we have control (key)
        pop     bx                      ; get ALT key parameters back,
        pop     ax                      ; and now tell Windows about that...
;
; Windows preserves all registers
prev60:

; Call windows with ah == 80h for uptrans, 00h for downtrans
; al = virtual key, bh = extended key flag, bl = scan code
;
        or      bh,1                    ; this flag indicates extended key.
        call    ds:[event_proc]

prevproc_end: 

        ret

prev_was_ext_proc endp

ifdef JAPAN
;1------------------------------------------------------
;1
;1      SetLEDsAX
;1
;1      update the LEDs on/off status for AX keyboard (Subtype=2).
;1      NumLock, CapsLock, ScrollLock, Kana
;1
;1------------------------------------------------------
        public SetLEDsAX
SetLEDsAX       proc    far

        push    ax
        push    bx
        push    es
        mov     ax,RAMBIOS
        mov     es,ax

        mov     ah,es:[kb_flag]                 ; get shift status
        mov     al,ds:[bKana]                   ; get Kana status
        and     al,80h                          ;
        shr     ax,4                            ;
        and     ah,fLEDs                        ; get Num, Caps, Scroll status
        or      al,ah                           ; make LED bits
UpdateLED:
        push    ax                              ;
        mov     al,0EDh                         ; LED command
        call    SendToKBD                       ;
        pop     ax                              ;
        and     al, 00fh                        ; rib off unwanted bit setting.
        call    SendToKBD                       ; SEND SECOND BYTE OF COMMAND

        mov     ah,al                           ;
        and     al,fLEDs                        ;
        and     byte ptr es:[kb_flag_2],not fLEDs
        or      es:[kb_flag_2],al               ; store current LED status

EndUpdate:
        pop     es
        pop     bx
        pop     ax
        ret

SetLEDsAX       endp
;1------------------------------------------------------
;1
;1      SendToKBD
;1
;1      Send command directly to keyboard
;1
;1------------------------------------------------------

SendToKBD       proc    near

;;      pushf                           ;
;;      cli                             ;
        push    cx                      ;
        mov     ah,al                   ; save parameter
        xor     cx,cx                   ; loop count
WAIT_LOOP:
        in      al,kb_status            ; KBC status
        test    al,fInputBufFull        ; see input buffer
        loopnz  WAIT_LOOP               ; exit if timeout

        mov     al,ah                   ;
        out     kb_data,al              ; send data to KBD
        sti                             ;
;;      xor     cx,cx                   ;
;;      loop    $                       ;

        pop     cx                      ;
;;      popf                            ;
        ret                             ;

SendToKBD       endp
endif   ; JAPAN

;*****************************************************************************
;******************************** pause_proc *********************************
;*****************************************************************************
; Handle Extended keyboard Pause key.
; This is called for the 2 bytes following an E1 prefix.
; the whole scan code sequence is
;
;       E1 1D 45  E1 9D C5
;
; The shift state bits are not changed.
;
; Input         AL = Scancode
;               AH = 00 if make, 80h if break
;               bh = 0
;***********************************************************

pause_proc proc near

        sti
        cmp     al, cCtrl               ; first byte = 1D (control)?
        je      pause_proc_end          ; yes, just pass this on

        mov     ds:[kbd_proc], codeOFFSET standard_proc
        cmp     al, 45h                 ; second byte?
        jne     pause_proc_end

        xor     bh, bh                  ; clear enhanced flag
        mov     bl, al                  ; scan code in BL
        mov     al, VK_PAUSE            ; it's VK_PAUSE
        call    ds:[event_proc]

pause_proc_end:
        ret

pause_proc endp


endif ; ENHANCE

;*****************************************************************************
;******************************* SetShiftState *******************************
;*****************************************************************************
;
;  Keep accurate track of shift state byte at 40:17H
;  For Alt and Control, we set or reset bits in 40:18 also.
;  For Numlock, the state bit is in 40:18, and the toggle bit is
;  in 40:17h.
;
;       AL:     scan code
;       AH:     sign bit indicates up/down
;
;       Uses BL or BX as mask.
;
        Public SetShiftState            ; public only for debug

SetShiftState proc near

ifdef   ENVOY
        mov     ds:[fModifier],0
endif

        cmp     al,cNumLock             ; check for numlock
        jz      ccNum1

SetShiftStateNoNumlock:
        mov     bx,fLshift              ; check shift, ctrl, alt bits
        cmp     al,cLshift
        jz      ccv4
        mov     bx,fRshift
        cmp     al,cRshift
        jz      ccv4
        mov     bx,fCtrlW
        cmp     al,cCtrl
        jz      ccv4
        mov     bx,fAltW
        cmp     al,cAlt

ifdef   ENVOY
if1
%out .. ENVOY code in SetShiftState
endif

        ; Envoy version
        jz      ccv4
        mov     ds:[fModifier], -1
        ret
else
if1
%out .. STANDARD code in SetShiftState
endif

        ; standard version
        jnz     ccv6
endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ccv4:
        DosXMacro       ds, Mod0
        or      ah,ah
        jns     ccv5
        not     bx
        and     word ptr es:[kb_flag],bx
        ret
ccv5:   or      word ptr es:[kb_flag],bx
ccv6:
        ret

; It's NumLock up/down.  This is similar to above, but keeps state in
; 40:18H, and toggles bit in 40:18H, so it's a little more complex.
; [BL] =  fNum is the appropriate bit in BOTH bytes.
; If CTRL is down, we do nothing.

ccNum1:
        test    byte ptr es:[kb_flag], fCtrl    ; (was fShift+fAlt+fCtrl)
         jnz    ccv6                            ; must have Ctrl off!
        mov     bl,fNum                         ; bit for numlock
        or      ah,ah
        jns     ccNum2
        not     bl                              ; upstroke, so just 
        and     byte ptr es:[kb_flag_1],bl      ; clear state
        ret
ccNum2:                                         ; downstroke: already down?
        test    byte ptr es:[kb_flag_1],bl
        jnz     ccNum3
        xor     byte ptr es:[kb_flag],bl        ; no, so toggle the toggle bit,
        inc     ds:[NumLockFlag]                ; and set flag for ToAscii()

ccNum3:
        or      byte ptr es:[kb_flag_1],bl      ; set state bit.
        ret

SetShiftState endp
endif	;NEC_98

        public endTRAP
endTRAP:
ifdef	NEC_98 ;Origin code is "IFDEF JAPAN"
;
;     SetInternalKeyState( WORD state )
;
;       bit 0 - VK_DBE_ALPHANUMERIC
;           1 - VK_DBE_KATAKANA
;           2 - VK_DBE_HIRAGANA
;           3 - Sync bit 0 through 2
;           4 - VK_DBE_HANKAKU
;           5 - VK_DBE_ZENKAKU
;           6 - Sync bit 4 and 5
;           7 - VK_DBE_ROMAN
;           8 - VK_DBE_NOROMAN
;           9 - Sync bit 7 and 8
;          10 - VK_DBE_CODEINPUT
;          11 - VK_DBE_NOCODEINPUT
;          12 - Sync bit 10 and 11
;
cProc   SetInternalKeyState,<PUBLIC,FAR>,<si,di>

        parmW   wState

cBegin  SetInternalKeyState

        cmp     DBEEnable,0             ; DBE enable?
        jz      noSyncRoma              ; jump if not

        cli                             ; We don't want key input during this

        mov     ax, wState              ; load sync request word
        test    al, 1000B               ; Request to sync mode keys?
        jz      noSyncModeKey           ; jump if not
        sub     bx,bx                   ; init Katakana state
                                        ; init Hiragana state
        test    al, 1                   ; ALPHANUMERIC?
        jnz     SetModeKey              ;  jump if so
        test    al, 10B                 ; KATAKANA?
        jz      @F                      ;  jump if not
        mov     bh,80h                  ; set KATAKANA
        jmp     short SetModeKey        ;
@@:
        mov     bl,80h                  ; set HIRAGANA
SetModeKey:
        mov     bKatakana, bh
        mov     bHiragana, bl
noSyncModeKey:
        test    al,1000000B             ; Request to sync mode Zen/Han?
        jz      noSyncZenHan            ; jump if not
        sub     bl,bl                   ; init Zen/Han state
        test    al,10000B               ; Hankaku?
        jz      @F                      ; jump if not
        mov     bl,80h                  ; set state to Hankaku
@@:
        mov     bZenHan, bl             ;
noSyncZenHan:
        test    ax, 1000000000B         ; Request to sync mode RomaJi?
        jz      noSyncRoma              ; jump if not
        xor     bl,bl
        test    ax, 100000000B          ; No RomaJi?
        jz      @F                      ; jump if not
        mov     bl,80h                  ; set state to RomaJi
@@:
        mov     bRoman, bl              ;
noSyncRoma:
        test    ax, 1000000000000B      ; Request to sync mode Code?
        jz      noSyncCode              ; jump if not
        xor     bl,bl
        test    ax, 0100000000000B      ; No code input mode?
        jnz     @F                      ; jump if so
        mov     bl, 80h                 ; set state to Code input
@@:
        mov     bCode, bl               ;
noSyncCode:
        sti                             ; Ok folks.
cEnd SetInternalKeyState

;
;       VOID FAR PASCAL NotifyMenuMode(UINT Mode)
;               Mode:   0:  Menu changed to Alphabetical accelerator key
;                       !0: Menu changed to Katakana accelerator key
;
;       NOTE: This is for some OEMs specific function
;
cProc   NotifyMenuMode,<PUBLIC,FAR>,<si,di>
        ParmW   wMode
cBegin
cEnd    NotifyMenuMode

endif	;NEC_98

if2
%out    .... END TRAP.ASM ....
%out
endif
sEnd CODE

END
