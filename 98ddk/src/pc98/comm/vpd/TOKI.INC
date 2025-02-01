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

; Constant definitions
LPT1_Base		equ	0100h	;LPT1 Base Address
LPT2_Base		equ	0500h	;LPT2 Base Address
LPT3_Base		equ	0D00h	;LPT3 Base Address

Low_AT_DATA		equ	0040h	;DATA PORT
Low_AT_STATUS		equ	0041h	;STATUS PORT
Low_AT_CONTROL		equ	0042h	;CONTROL PORT
Low_TOKI_CONTROL	equ	0049h	;TOKI CONTROL PORT
Low_FIFO_DATA		equ	004Ch	;FIFO DATA PORT
Low_TOKI_STATUS		equ	004Bh	;TOKI STATUS PORT
Low_EX_CONTROL		equ	004Eh	;TOKI EXTENDED CONTROL PORT

AT_DATA1		equ	0140h	;DATA PORT
AT_DATA2		equ	0540h	;DATA PORT
AT_DATA3		equ	0D40h	;DATA PORT

TOKI_Int_Ch_Read	equ	054Fh	;BiDi Board Int Ch Read Port


MODE_MASK		equ	0010h	;
AT_MODE			equ	0010h	;
PC98_MODE		equ	0000h	;

EX_MODE_MASK		equ	00E0h	;
EX_AT_MODE		equ	0000h	;
EX_PS2_MODE		equ	0020h	;
EX_HPP_MODE		equ	0040h	;
EX_ECP_MODE		equ	0060h	;
EX_TEST_MODE		equ	00C0h	;
EX_ECPCONFIG_MODE	equ	00E0h	;


Int_Ch_MASK		equ	0003h	;
CAON_MASK		equ	0004h	;
