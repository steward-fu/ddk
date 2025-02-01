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
;*****************************************************************************
;
; Kbdnec.asm NEC PC-9800 Series KEYBOARD LAYOUT
;
;*****************************************************************************

VKFUN = 0

.xlist
include cmacros.inc

include ..\vkwin.inc
include ..\vkoem.inc
include ..\lcid.inc
.list

if1
%out
%out   kbdjp.asm  (NEC)
endif


;*****************************************************************************

sBegin   DATA

Header  label byte
        public Header
        db      'DS'
        dd      00000411H
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
	KBDOFFSET	pEngCapsBits		; CAPS BIT MAP
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
ENG_NSTATES equ $-pEngToAscStates

pEngVKShiftStates label byte
        db      0               ; unshifted
        db      3               ; alt (normal - wont get here!)
        db      1               ; shifted
        db      3               ; alt+shift (wont get here1)
        db      2               ; control

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

pEngToAscVKeyList label word
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngCtrlVkKeyToIdx

pEngToAscVKeyListLens   label byte
   db VKEY_LIST_SIZE
   db VKEY_LIST_SIZE
   db VKEY_LIST_SIZE
   db VKEY_LIST_SIZE
   db VKEY_CTRL_LIST_SIZE

pEngVKeyToIdx label byte
        ;
        ; vkey index into the state tables
        ;
        db '1234567890', 0bdh, 0deh, 0dch
        db 'QWERTYUIOP', 0c0h, 0dbh
        db 'ASDFGHJKL', 00bbh, 0bah, 0ddh
        db 'ZXCVBNM', 0bch, 0beh, 0bfh, 0dfh

VKEY_LIST_SIZE equ $-pEngVKeyToIdx

pEngScanToIdx label byte
   ;
   ; scan code to index into the state tables
   ;
   db    001H, 002H, 003H, 004H, 005H, 006H, 007H, 008H, 009H, 00AH  ; number row
   db    00BH, 00Ch, 00Dh
   db    010H, 011h, 012h, 013h, 014h, 015h, 016h  ; qwerty ...
   db    017h, 018h, 019h, 01ah, 01bh
   db    01DH, 01eH, 01fh, 020h, 021h, 022h, 023h, 024h, 025h  ; asdf...
   db    026h, 027h, 028h
   db    029h, 02ah, 02bh, 02ch, 02dh, 02eh, 02fh, 030h, 031h  ; zxc ..
   db	 032H, 033h
SCANSIZE   equ   $ - pEngScanToIdx

;*****************************************************************************

pEngCtrlVkKeyToIdx label byte
        db 0C0H, 0DBH, 0DCH, 0DDH, 0DEH, 0DFH
VKEY_CTRL_LIST_SIZE equ $-pEngCtrlVkKeyToIdx

labelB   EngToAscControl
        ;
        ; control characters
        ;
        db 00H, 01BH, 01CH, 01DH, 01EH, 01FH

;*****************************************************************************

labelB   EngToAscAlt
labelB   EngToAscNormal
        ;
        ; unshifted characters
        ;
        db      '1234567890-^\'
        db      'qwertyuiop@[asdfghjkl;:]'
        db      'zxcvbnm,./',00h

EngToAscAltShift  label byte
EngToAscShift     label byte
        ;
        ; shifted characters
        ;
        db '!"#$%&',27h,'()',00h,'=`|'
        db 'QWERTYUIOP~{ASDFGHJKL+*}'
        db 'ZXCVBNM<>?_'

EngVKeyToAnsi     label byte
        db      '1234567890-^\'
        db      'QWERTYUIOP@[ASDFGHJKL;:]'
        db      'ZXCVBNM,./',00h

KanaNormal        label byte ; Kana_KeyCode
        db      0c7h, 0cch, 0b1h, 0b3h, 0b4h, 0b5h, 0d4h, 0d5h, 0d6h
        db      0dch, 0ceh, 0cdh, 0b0h
        db      0c0h, 0c3h, 0b2h, 0bdh, 0b6h, 0ddh, 0c5h, 0c6h, 0d7h
        db      0beh, 0deh, 0dfh
        db      0c1h, 0c4h, 0bch, 0cah, 0b7h, 0b8h, 0cfh, 0c9h, 0d8h
        db      0dah, 0b9h, 0d1h
        db      0c2h, 0bbh, 0bfh, 0cbh, 0bah, 0d0h, 0d3h, 0c8h, 0d9h
        db      0d2h, 0dbh

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

