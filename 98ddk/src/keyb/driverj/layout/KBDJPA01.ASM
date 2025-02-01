        page    ,132
;*****************************************************************************
;
;       kbdjpa01.asm JAPAN 106 keyboard layout
;
;       Copyright (c) Microsoft Corp. 1988-1994
;
;*****************************************************************************

VKFUN = 0

.xlist
JAPAN equ 1 ; #define JAPAN for the following header files.

include cmacros.inc
include ..\vkwin.inc
include ..\vkoem.inc
include ..\lcid.inc
.list

if1
%out
%out   kbdjpa01.asm  (Microsoft)
endif


;*****************************************************************************

sBegin  DATA

Header  label byte
        public Header
        db      'DS'
        dd      000000411H
        dw      1
        dw      offset LayoutData, 0            ;; dd address
        dw      offset LayoutEnd - offset LayoutData
        dd      0
        dw      0
        dd      0                               ; pUnicode
        dd      0                               ; nUnicode
LayoutData      label byte

EngLayout   label byte
        dw              CAPSNORMAL              ; FLAGS
        dw              NSTATEKEYS              ; NSTATE_KEYS
        dw              ENG_NSTATES             ; NSTATES
        dw              0                       ; NUM_DEAD
        dw              0                       ; NUM_LIG
        KBDOFFSET       pEngStates              ; STATE_LIST
        KBDOFFSET       pEngToAscStates         ; TOASC_STATES
        KBDOFFSET       pEngToAscStateTables    ; STATETABLES
        KBDOFFSET       pEngToAscVKeyList       ; VKEY_LISTS
        KBDOFFSET       pEngToAscVKeyListLens   ; VKEY_LIST_LENS
        KBDOFFSET       pEngVKShiftStates       ; VK_STATES
        KBDOFFSET       pEngScanToIdx           ; SCANTOIDX
        KBDOFFSET       pEngVKeyToIdx           ; VKEYTOIDX
        dw              SCANSIZE                ; SCAN_SIZE
        KBDOFFSET       EngVKeyToAnsi           ; VKEYTOANSI
        dw              0                       ; DEAD_KEYS
        dw              0                       ; DEAD_KEYTRANS
        dw              0                       ; LIG_KEYS
        KBDOFFSET       pEngCapsBits            ; CAPS BIT MAP
        KBDOFFSET       KanaNormal              ; KANA_NORMAL

;---------------------------------------------------------------------------
; CapsLock Vkey Bit Enable Table - CAPSNORMAL flag is required
;
pEngCapsBits label byte				;used with CAPSNORMAL

	db	0,0,0,0,0,0,0,0				; Controls & Numerals
	db	0FEh,0FFh,0FFh,07h,0,0,0,0 	; VK_A -> VK_Z & Numpad & Function Keys
	db	0,0,0,0,0,0,0,0				; Function Keys & OEM Keys
	db	0,0,0,0,0,0,0,0				; OEM Keys, SVK_A -> SVK_Z, OEM Controls

;---------------------------------------------------------------------------

pEngStates  label byte
   ;
   ; this table gives the states used. The values are in order, so that
   ; everywhere else bits can be used instead.
   ;
   ; DO NOT CHANGE THE ORDER
   ;
      db    VK_MENU,    080H  ; 0x12, down,    gives state = 1
      db    VK_SHIFT,   080H  ; 0x10, down,    gives state = 2
      db    VK_CONTROL, 080H  ;       down,    gives state = 8, will fail

NSTATEKEYS equ ($ - pEngStates) shr 1

        db      0               ; nothing - dont declare!
pEngToAscStates   label byte
   ;
   ; note: this table MUST be preceded by a zero
   ; note: the first three MUST come first in this order
   ;
        db      1               ; Alt
        db      2               ; Shift
        db      3               ; Alt+Shift
        db      4               ; control
	db	6		; contrl+shift (NOT AltGr)

ENG_NSTATES equ $-pEngToAscStates

pEngVKShiftStates label byte
        db      0               ; unshifted
        db      3               ; alt (normal - wont get here!)
        db      1               ; shifted
        db      3               ; alt+shift (wont get here1)
        db      2               ; control
	db	4		; control+shift (?)

pEngToAscStateTables label word
   ;
   ; the FIRST table here is the no-shift table. repnz scasb leaves di
   ; pointing to the address after the one found.
   ;
        KBDOFFSET       EngToAscNormal
        KBDOFFSET       EngToAscAlt
        KBDOFFSET       EngToAscShift
        KBDOFFSET       EngToAscAltShift
        KBDOFFSET       EngToAscControl
        KBDOFFSET       EngToAscControlShift


pEngToAscVKeyList label word
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngCtrlVkKeyToIdx
        KBDOFFSET       pEngCtrlShiftVkKeyToIdx


pEngToAscVKeyListLens   label byte
   db VKEY_LIST_SIZE
   db VKEY_LIST_SIZE
   db VKEY_LIST_SIZE
   db VKEY_LIST_SIZE
   db VKEY_CTRL_LIST_SIZE
   db VKEY_CTRLSHIFT_LIST_SIZE

pEngVKeyToIdx label byte
        ;
        ; vkey index into the state tables
        ;
        db '1234567890', 0bdh, 0deh, 0dch
        db 'QWERTYUIOP', 0c0h, 0dbh
        db 'ASDFGHJKL',  0bbh, 0bah, 0ddh
        db 'ZXCVBNM', 0bch, 0beh, 0bfh, 0e2h
        db VK_CONVERT, VK_NONCONVERT       ; billlwo 10/2/94

VKEY_LIST_SIZE equ $-pEngVKeyToIdx

pEngScanToIdx label byte
   ;
   ; scan code to index into the state tables
   ;
   db    002H, 003H, 004H, 005H, 006H, 007H, 008H, 009H  ; row 1, number row
   db    00aH, 00BH, 00Ch, 00Dh, 07Dh

   db    010H, 011h, 012h, 013h, 014h, 015h, 016h  ; row 2, qwerty ...
   db    017h, 018h, 019h, 01ah, 01bh

   db    01eh, 01fh, 020h, 021h, 022h, 023h, 024h  ; row 3, asdf...
   db    025h, 026h, 027h, 028h, 02bh

   db    02ch, 02dh, 02eh, 02fh, 030h, 031h, 032H, 033h  ; row 4, zxc ...
   db    034h, 035h, 073h
   db    079h, 07bh     ; billlwo 10/2/94

SCANSIZE   equ   $ - pEngScanToIdx

;********
pEngCtrlVkKeyToIdx label byte
	;
	; companion vkeys for the control characters
	;
	db 0DBH, 0DCH, 0DDH, 0E2H

VKEY_CTRL_LIST_SIZE equ $-pEngCtrlVkKeyToIdx	;# of control characters

labelB   EngToAscControl
	;
	; control characters
	;
	db 01BH, 01CH, 01DH, 01CH

;****************************************************************************

pEngCtrlShiftVkKeyToIdx label byte
	;
	; companion vkeys for the control+shift characters
	;
        db 036H, 0E2H

VKEY_CTRLSHIFT_LIST_SIZE equ $-pEngCtrlShiftVkKeyToIdx	;# of CTRL+SHIFT chars

labelB   EngToAscControlShift
	;
	; control shifted characters
	;
	db 01Eh, 01FH

;*******

labelB   EngToAscAlt
labelB   EngToAscNormal
        ;
        ; unshifted characters
        ;
        db      '1234567890-^\'
        db      'qwertyuiop@['
        db      'asdfghjkl;:]'
        db      'zxcvbnm,./\'
        db      00h, 00h ; billlwo 10/2/94

EngToAscAltShift  label byte
EngToAscShift     label byte
        ;
        ; shifted characters
        ;
        db '!"#$%&', 027h  ,'()',00h,'=~|'
        db 'QWERTYUIOP`{'
        db 'ASDFGHJKL+*}'
        db 'ZXCVBNM<>?_'
        db 00h,00h      ; billlwo 10/2/94

EngVKeyToAnsi     label byte
   db    '1234567890-^\'
   db    'QWERTYUIOP@[ASDFGHJKL;:]'
   db    'ZXCVBNM,./\'
   db    00h, 00h       ; billlwo 10/2/94

KanaNormal        label byte ; Kana_KeyCode
        db      0c7h, 0cch, 0b1h, 0b3h, 0b4h, 0b5h, 0d4h, 0d5h, 0d6h
        db      0dch, 0ceh, 0cdh, 0b0h
        db      0c0h, 0c3h, 0b2h, 0bdh, 0b6h, 0ddh, 0c5h, 0c6h, 0d7h
        db      0beh, 0deh, 0dfh
        db      0c1h, 0c4h, 0bch, 0cah, 0b7h, 0b8h, 0cfh, 0c9h, 0d8h
        db      0dah, 0b9h, 0d1h
        db      0c2h, 0bbh, 0bfh, 0cbh, 0bah, 0d0h, 0d3h, 0c8h, 0d9h
        db      0d2h, 0dbh
        db      00h,  00h  ; billlwo 10/2/94

; This table must follow KanaNormal table and adjacent to each other.
KanaShift       label byte ; Kana_KeyCode
        db      0c7h, 0cch, 0a7h, 0a9h, 0aah, 0abh, 0ach, 0adh, 0aeh
        db      0a6h, 0ceh, 0cdh, 0b0h
        db      0c0h, 0c3h, 0a8h, 0bdh, 0b6h, 0ddh, 0c5h, 0c6h, 0d7h
        db      0beh, 0deh, 0a2h
        db      0c1h, 0c4h, 0bch, 0cah, 0b7h, 0b8h, 0cfh, 0c9h, 0d8h
        db      0dah, 0b9h, 0a3h
        db      0afh, 0bbh, 0bfh, 0cbh, 0bah, 0d0h, 0d3h, 0a4h, 0a1h
        db      0a5h, 0dbh
        db      000h, 000h      ; billlwo 10/2/94
;
;        db      0ffh, 0ffh, 0a7h, 0a9h, 0aah, 0abh, 0ach, 0adh, 0aeh
;        db      0a6h, 0ffh, 0ffh, 0ffh
;        db      0ffh, 0ffh, 0a8h, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
;        db      0ffh, 0ffh, 0a2h
;        db      0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
;        db      0ffh, 0ffh, 0a3h
;        db      0afh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0a4h, 0a1h
;        db      0a5h, 0ffh
LayoutEnd       label byte

sEnd     DATA

;*****************************************************************************

sBegin   CODE        ; Beginning of code segment

assumes CS,CODE
assumes DS,DATA

sEnd     CODE

end

