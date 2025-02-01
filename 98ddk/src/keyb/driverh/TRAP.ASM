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


SYSREQ  =       054h    ; PC-AT SYS REQ key .. not on M24 or ICO keyboard
                        ; (on Enhanced keyboard, is PrintScn/SysReq)
if1
    ifdef SYSREQ
        %out .. Has SysReq handling
    else
        %out .. Does NOT have SysReq handling
    endif
endif


; scan codes for special keys

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
ifdef JAPAN
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
endif ; JAPAN


;*****************************************************************************
;*****************************************************************************
;************************ data segment begins here ***************************
;*****************************************************************************
;*****************************************************************************

sBegin DATA

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
externW ScanCodes
externW VirtKeys
externA KBD_SCANS
externA SCAN_TO_VKEY_OFFS
externW pCurrentLocale

; This is the translation for the numeric pad when NUMLOCK is set.
; This is the same for all keyboards, and is fixed in length.

        public  KeyNumTrans
KeyNumTrans     LABEL   BYTE
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

globalW keyNumBase <dataOffset KeyNumTrans>

extrn   AckByte:byte                    ; Value of acknowledge byte
EXTRN   CVWBreak:FAR                    ; Kernel routine for CVW handling
externB fSysReq                         ; Flag for enabling SysReq key
ifdef   KOREA ; for TG Hangeul keyboard
fTGkeybd    db  0       ; initial value = 0 = not TG keyboard
extrn   KeyboardSubType:word
endif

ifdef JAPAN
extern  gNew106:byte
extern  bKana:byte
        public bRoman, bKatakana, bHiragana, bZenHan, bCode, bKanjiDown
bKanaDown   DB  0       ; 0 if KANA up, 80 if Kana down
bKanjiDown  DB  0       ; for 002/003 keyboard
bRoman      DB  0       ; 0 if Roman, 80 if NoRoman
bKatakana   DB  0       ; 0 if Alpha, 80 if Katakana
bHiragana   DB  0       ; 0 if Alpha, 80 if Hiragana
bZenHan     DB  0       ; 0 if Zenkaku, 80 if Hankaku
bCode       DB  0       ; 0 if no Code input, 80 if Code input
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

staticW kbd_proc,standard_proc

fExtend db      0               ; flag for above keys.

globalD bios_proc, 0
globalD nmi_vector, 0


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
ifdef   ENHANCE
        cmp     al,0F2h                 ; TG AT Hangeul Key scan code?
else
        cmp     al,0F0h                 ; TG AT Hangeul Key scan code?
endif
        je      short yesTG
        cmp     al,0F1h
        jne     short noTG
yesTG:
        mov     fTGkeybd, 1
        jmp     kbi1
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
        cmp     LastCursor, 0   ; is flag (scancode) set for numpad
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
        xor     ah,ah
ifdef   ENHANCE
        cmp     al,0F2h
else
        cmp     al,0F0h
endif
        je      short kbi13_tg
        cmp     al,0F1h
        je      short kbi13_tg
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
        ;       generate Soft key
        ;        - VK_HANGEUL (Shift+Alt)
        ;        - VK_HANJA (Shift+Ctrl)
        ;
        ;       BH reg is destoried.
        ;

;;;;;;;;;;;;;;;;;;;; for TG Hangeul Keyboard

;       or      ah, ah
;       jns     HG25                    ; eat the down-key
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


ifdef   ENHANCE
        cmp     al, 0F2h
else
        cmp     al, 0F0h
endif
        jnz     short HG15TG
        mov     bl, al
        mov     al, VK_HANGEUL
        jmp     short HG20TG
HG15TG:
        cmp     al, 0F1h
        jnz     short HG30TG
        mov     bl, al
        mov     al, VK_HANJA
HG20TG:
        call    ds:[event_proc]
        jmp     stdproc_end

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
        push    di                              ; set registers and flags
        push    cx
        push    es
        pushf
        cld                                     ; !!!!!!!
        push    ds
        pop     es

        mov     bx,     pCurrentLocale          ; check base lcid list
        mov     di,     [bx+SCANTOIDX]
        mov     cx,     [bx+SCAN_SIZE]
        repnz   scasb
        jz      kbi2b

        mov     di,     dataOFFSET ScanCodes    ; didnt find in pCurrent list,
        mov     cx,     KBD_SCANS               ; try base list.
        repnz   scasb
        jnz     kbi2a

        add     di, SCAN_TO_VKEY_OFFS           ; found vkey in base list
        jmp     kbi2c
kbi2a:
        mov     bl,     al                      ; didnt find vkey anywhere!
        mov     al,     -1
        jmp     kbi2d
kbi2b:
        sub     di,     [bx+SCANTOIDX]          ; move from table to table
        dec     di
        add     di,     [bx+VKEYTOIDX]
kbi2c:
        mov     bl,     al                      ; put back scan code
        mov     al,     [di]                    ; get vkey
kbi2d:
        xor     bh,     bh                      ; clear extended bit

        popf                                    ; restore flags
        pop     es
        pop     cx
        pop     di
        cmp     al,     -1
        je      kbi4

Translated:

ifdef   KOREA
        cmp     al, VK_OEM_PLUS
        jne     HG10
        test    byte ptr es:[kb_flag], fAlt
        jz      HG10
        or      ah, ah
        js      HG05                    ; eat Up-key

        mov     ax, VK_JUNJA
        call    ds:[event_proc]
        jmp     stdproc_end
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

        cmp     ds:[fTGkeybd], 0
        jne     okTGkeybd
        cmp     al,cAlt
        jz      noSetShiftState
okTGkeybd:
@@:

endif   ;KOREA

ifdef JAPAN
        mov     bl,al                   ; bl= scancode (all along proc)
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

        cmp     al, cCtrl
        jnz @F
    and byte ptr es:[kb_flag_1], 0feh ;clear bit 0 of 40:18
@@:
noSetShiftState:
        cmp     byte ptr [KeyboardSubType],Kbd101a
        jne     HG00

;
;       In 101/102 key keyboard, right hand ALT (E0-38) and CTRL (E0-1D)
;       should be VK_HANGEUL and VK_HANJA.
;
;;;     or      ah, ah                  ; Is it make?
;;;     jnz     HG00                    ;  No, continue
        cmp     al, cAlt                ; Is it HANGEUL key (E0-38) ?
        jnz     noHangeul
        mov     al, VK_HANGEUL          ; say we get VK_HANGEUL
        jmp     short prev40            ; and process it
noHangeul:
        mov     al, VK_HANJA            ; assume kanji key
        cmp     bl, cCtrl               ; Is it Hanja key (E0-1D)?
        jz      prev40                  ;  Yes, process it.
        mov     al, bl                  ; restore scan code.
HG00:
        cmp     byte ptr [KeyboardSubType],Kbd101b
        jne     HG000

;
;       In 101/102 key keyboard, right hand ALT (E0-38) and CTRL (E0-1D)
;       should be VK_HANJA and VK_HANGEUL.
;
;;;     or      ah, ah                  ; Is it make?
;;;     jnz     HG000                   ;  No, continue
        cmp     al, cAlt                ; Is it HANGEUL key (E0-1D) ?
        jnz     noHangeul1
        mov     al, VK_HANJA            ; say we get VK_HANGEUL
        jmp     short prev40            ; and process it
noHangeul1:
        mov     al, VK_HANGEUL          ; assume kanji key
        cmp     bl, cCtrl               ; Is it Hanja key (E0-38)?
        jz      prev40                  ;  Yes, process it.
        mov     al, bl                  ; restore scan code.
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
        add     di, SCAN_TO_VKEY_OFFS
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

        public endTRAP
endTRAP:

if2
%out    .... END TRAP.ASM ....
%out
endif
sEnd CODE

END
