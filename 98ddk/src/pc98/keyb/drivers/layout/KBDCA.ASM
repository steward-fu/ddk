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

	page	,132
;*****************************************************************************
;
;	kbdca.asm : French Canadian 
;
;*****************************************************************************

VKFUN = 0
.xlist
include cmacros.inc
include ..\vkwin.inc
include ..\vkoem.inc
include ..\lcid.inc
.list
sBegin	DATA

l_chevron	equ	0abh
r_chevron	equ	0bbh

;---------------------------------------------------------------------------

kbdCaLayout	label byte
public kbdCaLayout

	db	'DS'
	dd	00000c0cH		; Default LCID
	dw	0				; Version
	dd	dataOFFSET pkbdCaStart	; Start of Data
	dW	DATASIZE				; Size of Data
	dd	0				; pDibData
	dw	0				; DIBSIZE
	dd	0				; pUnicode
	dd	0				; nUnicode
LayoutData	label byte

; pkbdCaDibData label word

; DIBSIZE EQU $ - pkbdCaDibData

pkbdCaStart label word
	dw		ALTGRUSED or CAPSNORMAL
	dw		NSTATEKEYS		; NSTATE_KEYS
	dw		kbdCa_NSTATES		; NSTATES
	dw		NUMDEAD		        ; NUM_DEAD
	dw		0			; NUM_LIG
	KBDOFFSET       pkbdCaStates		; STATE_LIST
	KBDOFFSET       pkbdCaToAscStates	; TOASC_STATES
	KBDOFFSET       pkbdCaToAscStateTables	; STATETABLES
	KBDOFFSET       pkbdCaToAscVkeyList	; VKEY_LISTS
	KBDOFFSET       pkbdCaToAscVKeyListLens	; VKEY_LIST_LENS
	KBDOFFSET       pkbdCaVKShiftStates	; VK_STATES
	KBDOFFSET       pkbdCaScanToIdx		; SCANTOIDX
	KBDOFFSET       pkbdCaVKeyToIdx		; VKEYTOIDX
	dw		SCANSIZE		; SCAN_SIZE
	KBDOFFSET       kbdCaVKeyToAnsi		; VKEYTOANSI
	KBDOFFSET       pkbdCaDeadKeyTable	; DEAD_KEYS
	KBDOFFSET       pkbdCaDeadTrans		; DEAD_KEYTRANS
	dw		0			; LIG_KEYS
	KBDOFFSET		pEngCapsBits		; CAPS

;---------------------------------------------------------------------------
; CapsLock Vkey Bit Enable Table - CAPSNORMAL flag is required
;
pEngCapsBits label byte				;used with CAPSNORMAL

	db	0,0,0,0,0,0,0,0				; Controls & Numerals
	db	0FEh,0FFh,0FFh,07h,0,0,0,0 	; VK_A -> VK_Z & Numpad & Function Keys
	db	0,0,0,0,0,0,0,80h			; Function Keys & OEM Keys
	db	0,0,0,0,0,0,0,0				; OEM Keys, SVK_A -> SVK_Z, OEM Controls


;---------------------------------------------------------------------------

pkbdCaStates	label byte
	db	VK_MENU,    080H
	db	VK_SHIFT,   080H
	db	VK_CONTROL, 080H

NSTATEKEYS equ ($ - pkbdCaStates) shr 1

	db	0
pkbdCaToAscStates   label byte
	db	1		; Alt
	db	2		; Shift
	db	3		; Alt+Shift
	db	5		; AltGr
	db	7		; AltGr+Shift
	db	4		; Control
	db 	6		; Control+shift

kbdCa_NSTATES equ $-pkbdCaToAscStates


pkbdCaVKShiftStates label byte
	db	0		; unshifted
	db	0		; alt
	db	1		; shifted
	db	1		; alt+shift
	db	6		; AltGr
	db	7		; AltGr+Shift
	db	2		; Control
	db	3		; Control+Shift

pkbdCaToAscStateTables label word
        KBDOFFSET       kbdCaToAscNormal
        KBDOFFSET       kbdCaToAscAlt
        KBDOFFSET       kbdCaToAscShift
        KBDOFFSET       kbdCaToAscAltShift
	KBDOFFSET	kbdCaToAscAltGr
	KBDOFFSET	kbdCaToAscAltGrShift
        KBDOFFSET       kbdCaToAsCtrl
	KBDOFFSET	kbdCaToAscCtrlShift

pkbdCaToAscVKeyList label word
        KBDOFFSET       pkbdCaVKeyToIdx
        KBDOFFSET       pkbdCaVKeyToIdx
        KBDOFFSET       pkbdCaVKeyToIdx
        KBDOFFSET       pkbdCaVKeyToIdx
        KBDOFFSET       pkbdCaAltGrVKeyToIdx
        KBDOFFSET       pkbdCaAltGrShiftVKeyToIdx
        KBDOFFSET       pkbdCaCtrlVKeyToIdx
        KBDOFFSET       pkbdCaCtrlShiftVKeyToIdx

pkbdCaToAscVKeyListLens	label byte
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_ALTGRLIST_SIZE
	db VKEY_ALTGRSHIFT_SIZE
	db VKEY_CTRLLIST_SIZE
	db VKEY_CTRLSHIFT_SIZE

pkbdCaScanToIdx label byte
	db 029H, 002H, 003H, 004H, 005H, 006H, 007H, 008H, 009H
	db 00aH, 00bH, 00cH, 00dH, 010H, 011H, 012H, 013H
	db 014H, 015H, 016H, 017H, 018H, 019H, 01aH, 01bH
	db 01eH, 01fH, 020H, 021H, 022H, 023H, 024H, 025H
	db 026H, 027H, 028H,             02cH, 02dH, 02eH
	db 02fH, 030H, 031H, 032H, 033H, 034H, 035H
	db 02bH, 056H

SCANSIZE	equ	$ - pkbdCaScanToIdx

;---------------------------------------------------------------------------

pkbdCaVKeyToIdx label byte
;Virtual Keys
	db 0deh, 031H, 032H, 033H, 034H, 035H, 036H, 037H, 038H
	db 039H, 030H, 0bdH, 0bbH, 051H, 057H, 045H, 052H
	db 054H, 059H, 055H, 049H, 04fH, 050H, 0dbH, 0ddH
	db 041H, 053H, 044H, 046H, 047H, 048H, 04aH, 04bH
	db 04cH, 0baH, 0c0H,             05aH, 058H, 043H
	db 056H, 042H, 04eH, 04dH, 0bcH, 0beH, 0bfH
	db 0dch, 0e2H

VKEY_LIST_SIZE	equ	$ - pkbdCaVKeyToIdx

;---------------------------------------------------------------------------

kbdCaToAsCtrl label byte
        db 01FH, 01BH, 01CH, 01DH, 01CH

pkbdCaCtrlVKeyToIdx label byte
	db 0BDH, 0DBH, 0DCH, 0DDH, 0E2H
VKEY_CTRLLIST_SIZE	equ	$ - pkbdCaCtrlVKeyToIdx


kbdCaToAscCtrlShift label byte
	db 01EH

pkbdCaCtrlShiftVKeyToIdx label byte
	db 036H
VKEY_CTRLSHIFT_SIZE	equ	$ - pkbdCaCtrlShiftVKeyToIdx

;---------------------------------------------------------------------------

pkbdCaAltGrVKeyToIdx	label byte
	db 031H, 032H, 033H, 034H, 035H, 036H, 037H, 038H
	db 039H, 030H, 0bdH, 0bbH, 04fH, 050H, 0dbH, 0ddH
	db 0baH, 0c0H, 0deH, 0dcH, 04dH, 0bcH, 0beH, 0bfH
	db 0e2H

VKEY_ALTGRLIST_SIZE	equ $-pkbdCaAltGrVKeyToIdx

kbdCaToAscAltGr	label	byte
	db 0b1H, 040H, 0a3H, 0a2H, 0a4H, 0acH, 0a6H, 0b2H
	db 0b3H, 0bcH, 0bdH, 0beH, 0a7H, 0b6H, 05bH, 05dH
	db 07eH, 07bH, 05cH, 07dH, 0b5H, 0afH, 0adH, acute
	db ring

;---------------------------------------------------------------------------

pkbdCaAltGrShiftVKeyToIdx    label byte
	db 0bfH

VKEY_ALTGRSHIFT_SIZE     equ $-pkbdCaAltGrShiftVKeyToIdx

kbdCaToAscAltGrShift label   byte
	db 0b4H

;---------------------------------------------------------------------------

kbdCaToAscAlt		label byte
kbdCaToAscNormal	label byte
;Unshifted Keys'
	db '#1234567890-='
	db 'qwertyuiop', circumflex, cedilla
	db 'asdfghjkl;', grave
	db 'zxcvbnm,.', 0e9h
	db '<', l_chevron

;---------------------------------------------------------------------------

kbdCaToAscAltShift	label byte
kbdCaToAscShift		label byte
;Shift State'
	db '|!"/$%?&*()_+'
	db 'QWERTYUIOP', circumflex, umlaut
	db 'ASDFGHJKL:', grave
	db 'ZXCVBNM''"', 0c9h
	db '>', r_chevron

;---------------------------------------------------------------------------

kbdCaVKeyToAnsi	label byte
	db '#1234567890-='
	db 'QWERTYUIOP', circumflex, cedilla
	db 'ASDFGHJKL;', grave
	db 'ZXCVBNM,.', 0c9h
	db '<', l_chevron

;---------------------------------------------------------------------------
; Each entry is as follows:   char | 0 | vkey | shiftstate
;
pkbdCaDeadKeyTable	label byte
	db	circumflex
	db	cedilla
	db	umlaut
	db	grave
	db	acute
NUMDEAD equ ($-pkbdCaDeadKeyTable)

pkbdCaDeadTrans	label byte
	dw	NDEADLIST
	db	grave,		'a'
	db	grave,		'e'
	db	grave,		'i'
	db	grave,		'o'
	db	grave,		'u'
	db	grave,		'A'
	db	grave,		'E'
	db	grave,		'I'
	db	grave,		'O'
	db	grave,		'U'
	db	grave,		' '

	db	acute,		'a'
	db	acute,		'e'
	db	acute,		'i'
	db	acute,		'o'
	db	acute,		'u'
	db	acute,		'y'
	db	acute,		'A'
	db	acute,		'E'
	db	acute,		'I'
	db	acute,		'O'
	db	acute,		'U'
	db	acute,		'Y'
	db	acute,		' '

	db	circumflex,	'a'
	db	circumflex,	'e'
	db	circumflex,	'i'
	db	circumflex,	'o'
	db	circumflex,	'u'
	db	circumflex,	'A'
	db	circumflex,	'E'
	db	circumflex,	'I'
	db	circumflex,	'O'
	db	circumflex,	'U'
	db	circumflex,	' '

	db	umlaut,		'a'
	db	umlaut,		'e'
	db	umlaut,		'i'
	db	umlaut,		'o'
	db	umlaut,		'u'
	db	umlaut,		'y'
	db	umlaut,		'A'
	db	umlaut,		'E'
	db	umlaut,		'I'
	db	umlaut,		'O'
	db	umlaut,		'U'
	db	umlaut,		' '

	db	cedilla,	'c'
	db	cedilla,	'C'
	db	cedilla,	' '

NDEADLIST	equ	(($-pkbdCaDeadTrans) shr 1)-1

pkbdCaDeadList	label byte
	db	0E0h
	db	0E8h
	db	0ECh
	db	0F2h
	db	0F9h
	db	0C0h
	db	0C8h
	db	0CCh
	db	0D2h
	db	0D9h
	db	grave

	db	0E1h
	db	0E9h
	db	0EDh
	db	0F3h
	db	0FAh
	db	0FDh
	db	0C1h
	db	0C9h
	db	0CDh
	db	0D3h
	db	0DAh
	db	0DDh
	db	acute

	db	0E2h
	db	0EAh
	db	0EEh
	db	0F4h
	db	0FBh
	db	0C2h
	db	0CAh
	db	0CEh
	db	0D4h
	db	0DBh
	db	circumflex

	db	0E4h
	db	0EBh
	db	0EFh
	db	0F6h
	db	0FCh
	db	0FFh
	db	0C4h
	db	0CBh
	db	0CFh
	db	0D6h
	db	0DCh
	db	umlaut

	db	231
	db	199
	db	cedilla

DATASIZE equ $-pkbdCaStart

;---------------------------------------------------------------------------

sEnd	DATA

;---------------------------------------------------------------------------


sBegin	CODE	; Beginning of code segment
assumes CS,CODE
assumes DS,DATA
sEnd	CODE

end


