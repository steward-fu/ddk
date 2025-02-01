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

	INCLUDE VMDANEC.INC		; Grabber interface definitions


;**************************************
; Specific video adaptor conditional assembly map to generic conditionals
;
ISVGA	MACRO
IFNDEF VGA
VGA equ -1
ENDIF
ENDM

ISVGAMONO	MACRO
IFNDEF VGAMONO
VGAMONO equ -1
ENDIF
ENDM

ISEGA	MACRO
IFNDEF EGA
EGA equ -1
ENDIF
ENDM

; Default EGA
IFDEF		EGA
PEGA		equ	-1		; Turn on Paradise EGA code
VEGA		equ	-1		; Turn on Video 7 VEGA code
ATIEGA		equ	-1		; Turn on ATI EGA codee
ENDIF

VDD_DisINT_Init	equ	10

DefFlag MACRO name, bit
f&name		EQU	1 SHL bit
b&name		EQU	bit
ENDM

;***************
;  Video display equates
;
; BIOS AREA define

CRT_FF_COLOR	EQU	02H
CRT_FF_MONOCHRO	EQU	03H
CRT_FF_80	EQU	08H
CRT_FF_88	EQU	09H
CRT_FF		EQU	68H
CRT_FF2		EQU	6AH
CRT_BORD	EQU	6CH
CRT_MS		EQU	70H
CRT_COLR_67	EQU	0A8H
CRT_COLR_45	EQU	0AAH
CRT_COLR_23	EQU	0ACH
CRT_COLR_01	EQU	0AEH
CRT_MS_PL	EQU	70H
CRT_MS_BL	EQU	72H
CRT_MS_CL	EQU	74H
CRT_MS_SSL	EQU	76H
CRT_ATR_V	EQU	00H
CRT_ATR_G	EQU	01H
CRT_LETTER_80	EQU	04H
CRT_LETTER_40	EQU	05H
CRT_FONT_80	EQU	06H
CRT_FONT_88	EQU	07H
CRT_RAC_CODE	EQU	0AH
CRT_RAC_DOT	EQU	0BH
CRT_DISP_NE	EQU	0EH
CRT_DISP_E	EQU	0FH
CRT_LINE_SENSE	EQU	08H
CRT_TYPE_SENSE	EQU	08H
CRT_WARM_SENSE	EQU	80H
CRT_LETT_SENSE	EQU	04H
CRT_PDISP_REG	EQU	0A4H
CRT_WDISP_REG	EQU	0A6H

;	Character Generator Accsee port

CRT_CHAR_2nd	equ	0A1H
CRT_CHAR_1st	equ	0A3H
CRT_CHAR_LC	equ	0A5H
CRT_CHAR_RW	equ	0A9H

;	GRCG port

CRT_CG_MODE_N		equ	7CH
CRT_CG_PALETTE_N	equ	7EH

CRT_CG_MODE_H		equ	0A4H
CRT_CG_PALETTE_H	equ	0A6H
CRT_CG_PLANE0		equ	0EH
CRT_CG_PLANE1		equ	1DH
CRT_CG_PLANE2		equ	2BH
CRT_CG_PLANE3		equ	37H

;	EGC port
CRT_EGC_ACT_PLANE	equ	04A0H
	CRT_EGC_ACT_PLANE0	equ	0FFFEH
	CRT_EGC_ACT_PLANE1	equ	0FFFDH
	CRT_EGC_ACT_PLANE2	equ	0FFFBH
	CRT_EGC_ACT_PLANE3	equ	0FFF7H
	CRT_EGC_ACT_PLANE4	equ	0FFEFH
	CRT_EGC_ACT_PLANE5	equ	0FFDFH
	CRT_EGC_ACT_PLANE6	equ	0FFBFH
	CRT_EGC_ACT_PLANE7	equ	0FF7FH
CRT_EGC_READ_PLANE	equ	04A2H
	CRT_EGC_READ_PLANE0	equ	00FFH
	CRT_EGC_READ_PLANE1	equ	01FFH
	CRT_EGC_READ_PLANE2	equ	02FFH
	CRT_EGC_READ_PLANE3	equ	03FFH
	CRT_EGC_READ_PLANE4	equ	04FFH
	CRT_EGC_READ_PLANE5	equ	05FFH
	CRT_EGC_READ_PLANE6	equ	06FFH
	CRT_EGC_READ_PLANE7	equ	07FFH
CRT_EGC_MODE		equ	04A4H
CRT_EGC_F_COLOR		equ	04A6H
CRT_EGC_MASK		equ	04A8H
CRT_EGC_B_COLOR		equ	04AAH
CRT_EGC_SD_ADDR		equ	04ACH
CRT_EGC_BIT_LENG	equ	04AEH
CRT_EGC_FBGC		equ	6000H

CRTC_GRCG_ADD_H	equ	0A4h+1
CRTC_GRCG_ADD_N	equ	34h+1

;	VRAM Offset
IF 0
IFDEF HMODE
CRT_G_ENT		equ	0C0000H
CRT_T_ENT		equ	0E0000H
CRT_CGW_ENT		equ	0E4000H
CRT_CMOS_Start_Addr	equ	0E3FE2H
CRT_CMOS_Offset		equ	000FE2H
CRT_TEXTtoGRAPH		equ	-24000H
ELSE
CRT_G1_ENT		equ	0A8000H
CRT_G2_ENT		equ	0E0000H
CRT_T_ENT		equ	0A0000H
CRT_CGW_ENT		equ	0A4000H
CRT_CMOS_Start_Addr	equ	0A3FE2H
CRT_CMOS_Offset		equ	000FE2H
CRT_TEXTtoGRAPH		equ	004000H
CRT_PlaneGtoI		equ	020000H
ENDIF
ENDIF
;	Font sizw

Font_Data_size_H	equ	72
Font_Data_size_N	equ	32

;	GDC COMMAND

GDC_RESET	EQU	00H
GDC_MASTER	EQU	6FH
GDC_SLAVE	EQU	6EH
GDC_PITCH	EQU	47H
GDC_ZOOM	EQU	46H
GDC_VSYNC	EQU	20H
GDC_START	EQU	0DH
GDC_START2      EQU     6BH
GDC_STOP	EQU	0CH
GDC_STOP2	EQU	05H
GDC_SYNC_IDCT	EQU	0FH
GDC_SYNC_NIDC	EQU	0EH
GDC_SCROLL	EQU	70H
GDC_TEXTW	EQU	78H
GDC_WRITE	EQU	20H
GDC_CSRW	EQU	49H
GDC_VECTW	EQU	4CH
GDC_VECTE	EQU	6CH
GDC_TEXTE	EQU	68H
GDC_LPEN	EQU	0C0H
GDC_CSRFORM	EQU	4BH
GDC_DMAW	EQU	24H
GDC_DMAR	EQU	0A4H
GDC_READ	EQU	0A0H
GDC_MASK	EQU	4AH
GDC_CSRR	EQU	0E0H

;	GDC STATUS FLAGS

GDC_NOT_FULL	EQU	02H
GDC_EMPTY	EQU	04H
GDC_DREADY	EQU	01H

;	GDC IN/OUT PORT ADDRESS

GDC_S_STA	EQU	0A0H
GDC_S_CMD	EQU	0A2H
GDC_S_PARA	EQU	0A0H
GDC_M_STA	EQU	60H
GDC_M_CMD	EQU	62H
GDC_M_PARA	EQU	60H
GDC_M_DATA	EQU	62H

;	CRTC EQUS
CRTC_860	equ	860h
CRTC_862	equ	862h
CRTC_864	equ	864h
CRTC_866	equ	866h
CRTC_Bit	equ	00000001b

;	GDC_VDD_Flags
nVDD_Tx_off	equ	00000001h
nVDD_Tx_offBit	equ	0
nVDD_Gr_off	equ	00000002h
nVDD_Gr_offBit	equ	1
nVDD_Mas_Sla	equ	00000004h
nVDD_Mas_VSt	equ	00000008h
nVDD_Mas_TSt	equ	00000010h

nVDD_Gvram	equ	00000020h
DefFlag VDD_Gvram	5	;

nVDD_GvramRW	equ	00000040h
nVDD_GvramRWBit	equ	6
DefFlag VDD_GvramRW	6	;

nVDD_G_Use	equ	00000080h
nVDD_FontSel	equ	00000100h
nVDD_GDC_Hz	equ	00000200h
nVDD_W_cmd	equ	00000400h
nVDD_KAC	equ	00000800h
nVDD_ATR	equ	00001000h
nVDD_GRP	equ	00002000h
nVDD_EX		equ	00004000h
nVDD_T_Sc	equ	00008000h
nVDD_INT_IO_SPD	equ	00010000h
nVDD_T_ScBit	equ	15
nVDD_GvramRW_CD equ	00020000h
nVDD_GGDC_Sc_Para_Count	equ	28

;*******
; Special masks for flags
mVDD_GDC_Chk	equ	nVDD_Tx_off+nVDD_Gr_off+nVDD_Gvram+nVDD_ATR
mVDD_Kanji_Right equ	1000000010000000b
; Mask for get PLANE No.
Get_Plane_Msk	equ	00110000b
;
;*	- PORT ADDRESS -
;
CR_VINT_RST	EQU  64H

;	- PROT 64 BIT DEFINE -

CR_FONT_MAP_PFN0	equ	80h
CR_FONT_MAP_PFN0_BIT	equ	7
CR_FONT_MAP_AFN0	equ	40h
CR_FONT_MAP_AFN0_BIT	equ	6
CR_FONT_MAP_E0MAP	equ	20h
CR_FONT_MAP_E0MAP_BIT	equ	5
CR_FONT_MAP_A0MAP	equ	10h
CR_FONT_MAP_A0MAP_BIT	equ	4
CR_FONT_MAP_I		equ	08h
CR_FONT_MAP_G		equ	04h
CR_FONT_MAP_R		equ	02h
CR_FONT_MAP_B		equ	01h
CR_FONT_MAP_COLOR	equ	CR_FONT_MAP_I + CR_FONT_MAP_G + CR_FONT_MAP_R + CR_FONT_MAP_B
;
;*	- GDC STAUTS MODIFIER -
;
;GDC_LPND	EQU  80H
;
;*	- FONT PATTERN -
;
CR_KZ_F 	EQU  8 		; KANJI ZANKAKU FONT FLAG
CR_KH_F		EQU  4		; KANJI HANKAKU FONT FLAG
CR_A7_F		EQU  2		; ASCII 7*11 FONT FLAG
CR_A6_F		EQU  1		; ASCII 6*7  FONT FLAG
;
;*	- CR_STS_FLAG MODIFIER -
;
CR_S_CRT	EQU  80H	; CRT TYPE
CR_S_VS		EQU  40H	; VSYNC INT CALL
CR_S_KCG	EQU  08H	; KCG ACCESS MODE
CR_S_ATRB	EQU  04H	; ATTRIBUTE SELECTION
CR_S_CLM	EQU  02H	; COLUMN / LINE
CR_S_LN		EQU  01H	; LINE / SCREEN

;****************
;CRT_BIOS_Common_Area
;
CRT_FONT	EQU	450h
CRT_P1		EQU	CRT_FONT + 2
CRT_P2		EQU	CRT_FONT + 3
CRT_P3		EQU	CRT_FONT + 4
BIOS_FLAG5	EQU	CRT_FONT + 8
CR_EXT_STS	EQU	CRT_FONT + 9
CR_RASTER	EQU	CRT_FONT + 0EBh
CR_STS_FLAG	EQU	CRT_FONT + 0ECh
CR_CNT		EQU	CRT_FONT + 0EDh
CR_OFST		EQU	CRT_FONT + 0EEh
CR_SEG_ADR	EQU	CRT_FONT + 0F0h
CR_V_INT_OFST	EQU	CRT_FONT + 0F2h
CR_V_INT_SEG	EQU	CRT_FONT + 0F4h
CR_FONT		EQU	CRT_FONT + 0F6h
CR_WINDW_NO	EQU	CRT_FONT + 0F7h
CR_W_VRAMADR	EQU	CRT_FONT + 0F8h
CR_W_RASTER	EQU	CRT_FONT + 0FAh
PRXCRT		EQU	CRT_FONT + 0FCh
PRXDUPD		EQU	CRT_FONT + 0FDh
PRXGCPTN	EQU	CRT_FONT + 0FEh
PRXCRT2         EQU     CRT_FONT + 147h
;

M_GGDC_Port	equ	09A0h

	ModeFF_68h		equ	68h
	ModeATR4SG		equ	00000001b
	ModeTMON		equ	00000010b
	ModeC40			equ	00000100b
	ModeANKBF		equ	00001000b
	ModeINTER		equ	00010000b
	ModeDISP		equ	00100000b
	ModeBUMWEN		equ	01000000b
	ModeCGACM		equ	10000000b
	ModeFF_6Ah		equ	6Ah
	ModeFF_Color		equ	1
	ModeFF_Lines		equ	2
	ModeFF_Disp_Enable	equ	3
	ModeFF_16		equ	4
	ModeFF_Dot		equ	5
	ModeFF_EX		equ	7
	ModeFF_Enable		equ	8
	ModeFF_ColorEx		equ	10
	ModeFF_480Line		equ	13
	ModeFF_Pal256St		equ	24
	ModeFF_Hz1		equ	30
	ModeFF_Hz2		equ	31

	NewCRT_480Line		equ	10000000h
ModeFF_6Ah_Def_Size	equ	18


Font_Buf_Size	equ	72
VDD_FontPages_H	equ	14
VDD_FontPages_N	equ	8

;*****************************************************************************
;	GDC_Mode
;	†±†£†Ñ†£†Ñ†£†Ñ†£†Õ†£†Ñ†£†Ñ†£†Ñ†£†µ
;	†¥@@  @   GDC_Mode   @@@@†¥
;	†¹†Ñ†Ù†Ñ†Ù†Ñ†Ù†Ñ†Ý†Ñ†Ù†Ñ†Ù†Ñ†Ù†Ñ†½
;	@†¨@†¨@†¨  †¨@†¨@†¨@†¨@†¶†¦ GCo_Bw_BIT	 equ 00000001b
;	@†¨@†¨@†¨  †¨@†¨@†¨@†¶†¦†¦†¦ G8_16_BIT	 equ 00000010b
;	@†¨@†¨@†¨  †¨@†¨@†¶†¦†¦†¦†¦†¦ G400_200_BIT1 equ 00000100b
;	@†¨@†¨@†¨  †¨@†¶†¦†¦†¦†¦†¦†¦†¦ G400_200_BIT2 equ 00001000b
;	@†¨@†¨@†¨  †¶†¦†¦†¦†¦†¦†¦†¦†¦†¦ G200_U_D	 equ 00010000b
;	@†¨@†¨@†¶†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦ T30_480_BIT	 equ 00100000b
;	@†¨@†¶†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦ T25_20_BIT	 equ 01000000b
;	@†¶†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦†¦ T80_40_BIT	 equ 10000000b

GCo_Bw_BIT	equ 00000001b
G8_16_BIT	equ 00000010b
G400_200_BIT1   equ 00000100b
G400_200_BIT2   equ 00001000b
G200_U_D	equ 00010000b
T25_20_BIT2	equ 00100000b
T25_20_BIT	equ 01000000b
T80_40_BIT	equ 10000000b

;****************************************************************************
; Virtual Display Device(VDD) equates
;


;***************
; Timeout period to check for video updates when VM is windowed
;
VDD_Def_Update_Time EQU 50		; Default window update time = 50ms

;***************
; Priority boost for System VM when grabber is rendering VM into a window.
;
VDD_Pri_Device_Boost EQU Low_Pri_Device_Boost

;**************************************
; BIOS instance definitions
;
EGA_I_Addr1 EQU 450h
EGA_I_Len1  EQU 5h
EGA_I_Addr2 EQU 495h
EGA_I_Len2  EQU 5h
EGA_I_Addr3 EQU 53Bh
EGA_I_Len3  EQU 1Bh
EGA_I_Addr4 EQU 597h
EGA_I_Len4  EQU 1h
;**************************************
; Message mode screen dimensions
;
VDD_Msg_ScrnWid EQU 80			; 80 columns
VDD_Msg_ScrnHgt EQU 25			; 25 rows

;***************
; Instance Map equates
;   VxD_IM_Len	defines the number of instance map ranges to reserve
;   VxD_IMV_Len defines the number of bytes those ranges represent
;
VDD_IM_Len	equ	2		; 2 ranges of RAM in instance data
VDD_IMV_Len	equ	1*2+1Eh+5+1	; 1 pointer, 1Eh bytes, 5 bytes, 1 byte

;***************
; Init values for V_VertCnt and V_HorizCnt
VDD_VERT_PERIOD equ	12		; Must be at lease 11 for Wordstar 2000
VDD_HORIZ_PERIOD equ	255		; Trial Value must not be too big

;****************
; Delay window update by this time to avoid screen size bounce 
; during a 43 line mode change
; Related to the fModeChange in VDD_Flags.
; At the beg. of a mode change this bit is set,
; at the end of a mode change we set a long time out and,
; at the end of this time out we clear this bit in VDD_VM_Time_Out. 
;
; when this bit is set - vdd_get_mod returns no changes,
;                      - vdd_VMIdle skips setting a timeout 
;                      - vdd_clr_vm_time_out does not clear any time out.
;
;
;
VDD_ModeChangeDelay     equ     0100    ; long time out during a mode change

;****************************************************************************
; Virtual Display Device(VDD) structure definitions
;

VDD_Mod_Scr struc               ;†®†¢†¢†¢†¢†¢†¢†¢†¢†²
Scr_SAD1_L              db  ?   ;†¾†¢†¢ SAD1(L)  †¢†Æ
Scr_SAD1_H              db  ?   ;†¤0 0 0†¾ SAD1(H) †Æ
Scr_SL1_L               db  ?   ;†¾ SL1(L)†Æ0 0 0 0†¤
Scr_SL1_H               db  ?   ;†¤* I†¾ SL1(H)  †¢†Æ
Scr_SAD2_L              db  ?   ;†¾†¢†¢ SAD2(L)  †¢†Æ
Scr_SAD2_H              db  ?   ;†¤0 0 0†¾ SAD2(H) †Æ
Scr_SL2_L               db  ?   ;†¾ SL2(L)†Æ0 0 0 0†¤
Scr_SL2_H               db  ?   ;†¤* I†¾ SL2(H)  †¢†Æ
Scr_SAD3_L              db  ?   ;†¾†¢†¢ SAD3(L)  †¢†Æ
Scr_SAD3_H              db  ?   ;†¤0 0 0†¾ SAD3(H) †Æ
Scr_SL3_L               db  ?   ;†¾ SL3(L)†Æ0 0 0 0†¤
Scr_SL3_H               db  ?   ;†¤* I†¾ SL3(H)  †¢†Æ
Scr_SAD4_L              db  ?   ;†¾†¢†¢ SAD4(L)  †¢†Æ
Scr_SAD4_H              db  ?   ;†¤0 0 0†¾ SAD4(H) †Æ
Scr_SL4_L               db  ?   ;†¾ SL4(L)†Æ0 0 0 0†¤
Scr_S4L_H               db  ?   ;†¤* I†¾ SL4(H)  †¢†Æ
VDD_Mod_Scr ends                ;†¶†¢†¢†¢†¢†¢†¢†¢†¢†º

VDA_Stt_Siz		equ	968
VDA_Mod_Siz		equ	312
VDD_Scr_Siz		equ	16

;****************
; controller state
Reg_State_struc     struc

VDA_Stt		db	VDA_Stt_Siz dup (?)		; VDA_Stt_Struc

GDC_VDD_Flags		dd	?
GDC_Mode		db	?
			db	?,?,?

TGDC_Mode_inf	db	?
TGDC_ChrCnt_1L	db	?
TGDC_def_P3	db	?
TGDC_def_P4	db	?
TGDC_def_P5	db	?
TGDC_def_P6	db	?
TGDC_def_P7	db	?
TGDC_def_P8	db	?
TGDC_Zoom	db	?
TGDC_Scr	db VDD_Scr_Siz dup (?)
TGDC_CsrForm_P1	db	?
TGDC_CsrForm_P2	db	?
TGDC_CsrForm_P3	db	?
TGDC_Pitch	db	?
TGDC_Csrw_EADl	db	?
TGDC_Csrw_EADh	db	?
TGDC_Csrw_dAD	db	?
TGDC_Maskl	db	?
TGDC_Maskh	db	?
GGDC_Mode_inf	db	?
GGDC_ChrCnt_1L	db	?
GGDC_def_P3	db	?
GGDC_def_P4	db	?
GGDC_def_P5	db	?
GGDC_def_P6	db	?
GGDC_def_P7	db	?
GGDC_def_P8	db	?
GGDC_Zoom	db	?
GGDC_Scr	db VDD_Scr_Siz/2 dup (?)
GGDC_Text_Tx8	db	?
GGDC_Text_Tx7	db	?
GGDC_Text_Tx6	db	?
GGDC_Text_Tx5	db	?
GGDC_Text_Tx4	db	?
GGDC_Text_Tx3	db	?
GGDC_Text_Tx2	db	?
GGDC_Text_Tx1	db	?
GGDC_Text_C	db	?
GGDC_CsrForm_P1	db	?
GGDC_CsrForm_P2	db	?
GGDC_CsrForm_P3	db	?
GGDC_Pitch	db	?
GGDC_Vect_stat	db	?
GGDC_Vect_DCl	db	?
GGDC_Vect_DCh	db	?
GGDC_Vect_Dl	db	?
GGDC_Vect_Dh	db	?
GGDC_Vect_D2l	db	?
GGDC_Vect_D2h	db	?
GGDC_Vect_D1l	db	?
GGDC_Vect_D1h	db	?
GGDC_Vect_DMl	db	?
GGDC_Vect_DMh	db	?
GGDC_Csrw_EADl	db	?
GGDC_Csrw_EADh	db	?
GGDC_Csrw_dAD	db	?
GGDC_Maskl	db	?
GGDC_Maskh	db	?
GGDC_Csrr_EADl	db	?
GGDC_Csrr_EADm	db	?
GGDC_Csrr_EADh	db	?
GGDC_Csrr_dADl	db	?
GGDC_Csrr_dADh	db	?
A_Pal		db	4 dup(?)
GDC_EGA_AD_Reg	db	?
GDC_EGA_Par16	db	16*3 dup(?)
GDC_EGA_KAD_Reg	db	?
GDC_EGA_Par256	db	256*3 dup(?)
GGDC_Write	db	?
GGDC_Write_P	db	?

MS_Pl		db	?
MS_Bl		db	?
MS_Cl		db	?
		db	?,?,?
GC_Mod		db	?
GC_Tail_Cnt	dd	?
GC_Tail		db	?,?,?,?

EGC_ACT_PLANE	dw	?
EGC_READ_PLANE	dw	?
EGC_MODE	dw	?
EGC_F_COLOR	dw	?
EGC_MASK	dw	?
EGC_B_COLOR	dw	?
EGC_SD_ADDR	dw	?
EGC_BIT_LENG	dw	?
Pattern_Reg_AD	dw	?
Pattern_Reg	dw	8 dup(?)

Text_Line_Offset dw	32 dup (?)

Font_MAP_64_STS			db	?
Font_TallSize			db	?
Text_Lux			db	?
VDD_PMask_Reg			db	?
ModeFF_68h_IN_States		db	?
ModeFF_6Ah_IN_States		dd	?
ModeFF_6Ah_IN_States2		db	?
ModeFF_Read_Port_States		db	?
ATR_6Eh_IN_States		db	?,?

		db	?

Reg_State_struc 	ends

.ERRNZ	size Reg_State_struc MOD 4
.ERRNZ	VDA_Stt

%out	EGA_State_struc	equ	Reg_State_struc


DefFlag CG_CODE		0
DefFlag CG_VCGW		1
DefFlag CG_PHYS		2

DefFlag CG_EUDC		5
DefFlag CG_ANK		6
DefFlag CG_LR		7


VDD_Font_struc	struct
CG_Code		dw	?
CG_RCLR		db	?
CG_Flag		db	?

DATA_Type	dd	?
DATA_Addr	dd	?
DATA_Attr	db	?
		db	?,?,?

EUDC_Hand	dd	?
EUDC_Addr	dd	?

VCGW_Hand	dd	?
VCGW_Addr	dd	?
VDD_Font_struc	ends
.errnz	size VDD_Font_struc mod 4

;****************
; Control Block (CB) structure definition
;
VDD_CB_Struc	 struc
VDD_Flags		dd	?			; VDD Flags
VDD_EFlags		dd	?			; VDD Event Flags
VDD_OFlags		dd	?			; VDD Option Flags
VDD_SFlags		dd	?			; VDD Static Flags
VDD_SCount		dd	?			; VDD Suspend Count

EGC_CNT			dd	?			;
EGC_SRC			dd	4 dup (?)		;
EGC_PAT			dd	4 dup (?)		; EGC State
EGC_MSK			dd	?			;
EGC_110			dw	?			;
EGC_LEN			dw	?			;
EGC_ACT			db	?			;
EGC_001			db	?			;
EGC_ROP			db	?			;
EGC_MOD			db	?			;
EGC_WAD			db	?			;
EGC_FGC			db	?			;
EGC_BGC			db	?			;
GRCG_Mode		db	?			; GRCG State
GRCG_Tile		dd	4 dup (?)		;
GRCG_TileNum		db	?			;
GRCG_Flags		db	?			;
			db	?,?			; ======== for dword align =========

GRCG_LOD_Proc		dd	?			; GRCG Procedure
GRCG_STOB_Proc		dd	?			;
GRCG_STOW_Proc		dd	?			;
GRCG_STOD_Proc		dd	?			;
EGC_ReadPl_Proc		dd	?			; EGC Procedure

VDD_GGDC_EM_Dot_Func	dd	?, ?			; GDC Emulate (1dot draw func. off32)
VDD_GGDC_EM_WR_Func	dd	?			; GDC Emulate (1word draw func. off32)

VDD_Font		db	size VDD_Font_struc dup (?)

VDD_Stt			db	size Reg_State_struc dup (?) ; Controller state structure
VDD_SttCopy		db	size Reg_State_struc dup (?) ; Controller state copy
VDD_ModSave		db	size VDA_Mod_Siz dup (?);

VDD_Dirty_Text		db	?			;   4 page
VDD_Dirty_GRCG		db	?			;   8 page
			db	?,?			;
VDD_Dirty_Bank_0	dd	?			;  32 page
VDD_Dirty_Bank_1	dd	?			;  32 page
VDD_Dirty_Packed	dd	4 dup (?)		; 128 page

VDD_Mem_Flags		dd	?			;
VDD_Mem_Bank_0_Hand	dd	?			;
VDD_Mem_Bank_0_Addr	dd	?			;
VDD_Mem_Bank_1_Hand	dd	?			;
VDD_Mem_Bank_1_Addr	dd	?			;
VDD_Mem_Extend_Hand	dd	?			;
VDD_Mem_Extend_Addr	dd	?			;
VDD_Mem_GrCopy_Hand	dd	?			;
VDD_Mem_GrCopy_Addr	dd	?			;
VDD_Mem_Common_Hand	dd	?			;
VDD_Mem_TxMast_Addr	dd	?			;
VDD_Mem_TxCopy_Addr	dd	?			;
VDD_Mem_Mem_IO_Addr	dd	?			;


VDD_AV_CRT_Scan db  ?
VDD_AV_Regs_A660 db  ?
VDD_ModeEGA	db  ?
VDD_LastMode	db  ?
VDD_MMode	db  ?
VDD_VertCnt	db  ?
VDD_HorizCnt	db  ?
VDD_PIF 	dd  ?
VDD_ModTime	dd  ?
VDD_Time_Out_Handle dd	?
VDD_Routine	dd  ?
VDD_Upd_Start	dd  ?
VDD_RoutineM		dd ?
VDD_RoutineS		dd ?
VDD_R_CntM		db ?
VDD_R_CntS		db ?
VDD_IO_SvCmdM		db ?
VDD_IO_SvCmdS		db ?
VDD_IO_SvCntM		dw ?
VDD_IO_SvCntS		dw ?
VDD_Save_ParaM		db 16 dup (?)
VDD_Save_ParaS		db 16 dup (?)
VDD_NOT_OUTM		db ?
VDD_NOT_OUTS		db ?
VDD_Save_offsetM	dd ?
VDD_Save_offsetS	dd ?
VDD_Vsync		db ?
;VDD_CGW_Code		dw ?
;VDD_CGW_LR		db ?
;VDD_A9_Offset		db ?
;VDD_PortA9		db 32 dup(0)
;VDD_Font_Addr		dd ?
;VDD_Font_Handle		dd ?
VDD_CRTC_Flags		db	0
VDD_Monitor_ID		db	0
VDD_Dot_ClockH		db	0
VDD_Dot_ClockM		db	0
VDD_Dot_ClockC		db	0
VDD_VRAM_Bank8		db	0
VDD_VRAM_BankE		db	0
VDD_DisINT_Counter	db	0
VDD_Get_Mem_Count dd    ?               ; Match calls to VDD_Get_Mem/VDD_Free_Mem
                                        ; VDD_UnLock_APP unfreezes the App only
                                        ; if this count is Zero.

VDD_Update_Hit_Cnt  DB	?		;3.1

VDD_Recent_User_Action 	DW 	0	;3.1 recently event occured
VDD_Check_Upd_Soon 	DW 	0	;3.1 check update
VDD_GDCEmulate		db	0	;3.1 GDC Emulate Flag (Door/Window)
VDD_Int18_OVector 	DD 	0	;3.1 Int18 Old Vector for VM
VDD_Save_0EAAh_State    db      0
VDD_Save_Dot_Clock      db      0
VDD_vertical_line_J	db	0	; For J Only vertical line connect/disconnect

MiniVDD_LTrap_Table	db ((size MiniVDD_LTrap_Struct)*MaxMiniTrap) dup (?)

VAB_Handle		dd	?			; 
VAB_Entry		dd	?			; 
PVD_Window_Pos		dd	?			; Low word is for WinA, High word is for WinB.
PVD_Scanline		dw	?			; Pixels per Scanline
PVD_Video_Mode		dw	?			; 
PVD_Text_Mode		db	?			; 
VAB_Flags		db	?			; 

			db	?,?,?

VDD_CB_Struc	 ends
.errnz	(size VDD_CB_Struc) mod 4


;******* Special video mode values
; When mem mapped virtually, this holds video mode: 0-6
; Otherwise it holds one of the following values.
VDD_MMPhys	EQU 0FFh		;   Physical or undefined (virtual)
VDD_MM2nd	EQU 0FEh		;   Remapped to 2nd half of physical


;***************
; VDD Flag bit definitions	(.VDD_Flags: fVDD_ & bVDD_)
;
DefFlag VDD_Win		0	; VM is displayed in window
DefFlag VDD_HCurTrk	1	; Track cursor horizontally
DefFlag VDD_Grab	2	; Grab event pending
DefFlag VDD_ModeChange 	3	; indicate mode change in progress
DefFlag VDD_ModeSet	4	; Video in INT 10 mode set
DefFlag VDD_Save	5	; We have EGA state to restore
DefFlag VDD_MEna	6	; Video memory enabled
DefFlag VDD_Font	7	; Memory mapped for font load
DefFlag VDD_Upd		11	; Update event pending
DefFlag VDD_IdleUpdate	12	; VM went idle during window update
DefFlag VDD_WaitAtt	13	; VDD suspended VM until attached
DefFlag VDD_ForcedUpd 	15	; Force window update after VMInit
DefFlag VDD_SuspEvent	16	; Suspend VM Event has been queued
DefFlag VDD_CanRestore	17	; VM knows how to restore its screen
DefFlag VDD_UpdAll1	18	; see notes in VDD_Clear_Mod
DefFlag VDD_UpdAll2 	19	; see notes in VDD_Clear_Mod
DefFlag VDD_AccEnable	20	; Accelerator BIOS enabled
;;DefFlag VDD_NoTrap	21	; Don't trap I/O ports(use INT 2F)
DefFlag VDD_NoSaveRes	21	; set if VM must restore screen
DefFlag VDD_IOT		22	; I/O ports presently trapped
DefFlag VDD_NTSave	23	; NotTrap save during mode changes
DefFlag VDD_Boost	24	; SYS VM boosted for memory read
DefFlag VDD_Hide	27	; Hide cursor in window
DefFlag VDD_ScrollModf 	30	; Previous get_mod  returned scroll event
DefFlag VDD_MInit	31	; Display mem initialized

;***************
; VDD_EFlags definitions, error and error msg status
;	Note that Msg bits are error bits shifted left 8
;
fVDE_Crit	equ	00000001h	    ; Save during VDD critical section
fVDE_CritBit	equ	0
fVDE_NoMain	equ	00000100h	    ; Out of main, display not accurate
fVDE_NoMainBit	equ	8
fVDE_NoCopy	equ	00000200h	    ; Out of copy, window not accurate
fVDE_NoCopyBit	equ	9
fVDE_NoFGrnd	equ	00000400h	    ; Cannot run bkgrnd with fgrnd VM
fVDE_NoFGrndBit equ	10
fVDE_NoWind	equ	00000800h	    ; Cannot window this VM
fVDE_NoWindBit	equ	11
fVDE_Excl	equ	00001000h	    ; Background high Res excluded
fVDE_ExclBit	equ	12
fVDE_NoSupMode	equ	00002000h	    ; Mode not supported
fVDE_NoSupBit	equ	13
fVDE_NoMMsg	equ	00010000h	    ; User told out of main
fVDE_NoMMsgBit	equ	16
fVDE_NoCMsg	equ	00020000h	    ; User told out of copy
fVDE_NoCMsgBit	equ	17
fVDE_NoFGMsg	equ	00040000h	    ; User told cannot run with fgrnd
fVDE_NoFGMsgBit equ	18
fVDE_NoWMsg	equ	00080000h	    ; User told cannot window
fVDE_NoWMsgBit	equ	19
fVDE_ExclMsg	equ	00100000h	    ; User told VM exclusive
fVDE_ExclMsgBit equ	20
fVDE_NoSMsg	equ	00200000h	    ; User told mode not supported
fVDE_NoSMsgBit	equ	21
;;;;IFDEF	NHMODE
fVDE_NoWinVMExeMsg	equ	00400000h	    ; User told Nmode_VM not Window_VM
fVDE_NoWinVMExeMsgBit	equ	22
;;;;ENDIF

;***************
; VDD_OFlags definitions
;
fVDO_IOTrap	equ	00000001h
fVDO_IOTrap_Bit	equ	0
fVDO_IOTS	equ	00000002h
fVDO_IOTS_Bit	equ	1
fVDO_GraphD	equ	00000004h
fVDO_GraphD_Bit	equ	2
fVDO_Crit	equ	00000008h
fVDO_Crit_Bit	equ	3
fVDO_Scrn	equ	00000010h
fVDO_Scrn_Bit	equ	4
fVDO_IOTrap2	equ	00000020h
fVDO_IOTrap2_Bit	equ	5

;***************
; VDD_VAB_Flags/.VAB_Flags definitions
;
DefFlag VAB_PVD		0		; PV-Disp
DefFlag VAB_ACC		1		; Accelerator

;***************
; VDD_Mem_Flags definitions
;
DefFlag VMF_Acc_Bank_0	0		;
DefFlag VMF_Acc_Bank_1	1		;
DefFlag VMF_Acc_Extend	2		;
DefFlag	VMF_Map_Grph	3		;
DefFlag VMF_Repaint	4		;
DefFlag VMF_Got_Stt	5		;
DefFlag VMF_Got_Mod	6		;

;***************
; VDD global variables
;
Extern_vgVDD MACRO
EXTRN	Vids:BYTE
EXTRN	Vid_Flags:DWORD
EXTRN	Vid_CB_Off:DWORD
EXTRN	Vid_Focus_VM:DWORD
EXTRN	Vid_VM_Handle:DWORD
EXTRN	Vid_VM_HandleRun:DWORD
EXTRN	Vid_Msg_Pseudo_CB:BYTE
EXTRN	Vid_Msg_Pseudo_VM:DWORD
EXTRN	Dsp_Focus_VM:DWORD
EXTRN	Dsp_VM_Handle:DWORD
ENDM


;
; VDD global flag definitions	(Vid_Flags: fVid_ & bVid_)
;
DefFlag Vid_SysVMI		0		; VDD_VMInit called for SYS VM
DefFlag Vid_25Line		2		; 25 line / 20 line (31 line) Display
;DefFlag Vid_DOff		4		; Display is off(during save/restore)
DefFlag Vid_SFPending		7		; Set Focus is pending
DefFlag Vid_Crit		10		; In video critical section
DefFlag Vid_PageSwapInst	11		; Set if PageSwap device installed
DefFlag Vid_MsgR		12		; System message mode in Run state
DefFlag Vid_MsgA		13		; System message mode active

DefFlag Vid_CoreFree		16		; Use CoreGrph in Windowed VM
DefFlag	Vid_DispEnable		17		; DisplayDriver Enable
DefFlag	Vid_DisableText		18		; Don't access to text H/W
DefFlag	Vid_DisableGrph		19		; Don't access to grph H/W
DefFlag	Vid_DisableFont		20		; Don't access to gont H/W
mVid_Disable	equ	(Vid_DisableText + Vid_DisableGrph + Vid_DisableFont)


;*******
; Macros for dispatching to display dependent routines(VGA, EGA, CGA)
;	Address of vectors for the routines is set at initialization
;
;CallVDD macro	 RtnName
;    extrn   VDD_ProcTab:dword
;    push    eax
;    mov     eax,[VDD_ProcTab]
;    call    dword ptr [RtnName.eax]
;    pop     eax
;    endm
;
;JumpVDD macro	 RtnName
;;    extrn   VDD_ProcTab:dword
;    push    eax
;    mov     eax,[VDD_ProcTab]
;    mov     eax,[RtnName.eax]
;    xchg    eax,[esp]
;    ret
;    endm
;
;******
; Macro for defining the vector tables
;
;DefProc macro	 TableName,RtnName,EntryPoint
;.errnz      ($-TableName)-RtnName
;    dd      OFFSET32 EntryPoint
;    endm
;
;******
; Definitions for routine vector offsets
;VP_RState   equ     0				 ; Restore complete ctrlr state
;VP_PreSRAM  equ     VP_RState+4		 ; Set up for memory save/restore
;VP_RestIndx equ     VP_PreSRAM+4		 ; Restore index registers
;VP_RestCtlr equ     VP_RestIndx+4		 ; Restore the regs
;VP_RestCtl2 equ     VP_RestCtlr+4		 ; Restore 2nd EGA regs

;***************
; Special definitions of the memory used by WINDOWS DISPLAY.DRV
;	see EGAMEM.INC
;
;***************
; VM1's VRAM save area, last 16 bytes of the last displayed 4k page
;
SYS_VRAM_Addr	equ	0A7FF0h
SYS_VRAM_Len	equ	10h

SYS_VRAM_struc	    struc
current_brush	    db	  8 dup (?)	    ;8 bytes for a brush
tonys_bar_n_grill   db	  ?		    ;1 byte for a solid color
shadow_mem_status   db	  ?		    ;Status flags for upper 16/32K of mem
known_word	    dw	  ?		    ;Required locations for state
shadowed_mode	    db	  ?		    ;  detection code
plane_index	    db	  ?
enable_test	    db	  ?
saved_latches	    db	  ?
SYS_VRAM_struc	    ends

; When using the second half of the EGA, we clear the Shadow memory exists
;   bit and set the shadow_trashed bit if the shadow_in_use bit is set

shadow_exists	equ	00000001b	;Shadow memory exists
shadow_exists_Bit equ	0
shadow_in_use	equ	00000010b	;Shadow contains valid image
shadow_trashed	equ	00000100b	;Shadow contents have been destroyed

;*****************************************************************************
;*****************************************************************************
; MACROS

AssertVDDPtrs	macro	pCB, pST
ifdef	AssertVDD
	local	AVP_Fail, AVP_Good

	pushfd
	push	ebx
	add	ebx, [Vid_CB_Off]
ifnb	<pCB>
	cmp	ebx, pCB
	jnz	short AVP_Fail
endif
ifnb	<pST>
	lea	ebx, [ebx.VDD_Stt]
	cmp	ebx, pST
	jnz	short AVP_Fail
endif
	jmp	short AVP_Good
AVP_Fail:
	int	1
AVP_Good:
	pop	ebx
	popfd
endif
	endm


Assert_VDD_ptrs MACRO R_Handle,R_VDDptr,Pseudo_OK
LOCAL	AVP_Ok1, AVP_Ok2
IFNB <Pseudo_OK>
	cmp	ebx, VDD_Msg_Pseudo_VM
	je	SHORT AVP_Ok1
ENDIF
Assert_VM_Handle    R_Handle
AVP_Ok1:
	pushfd
	push	R_VDDptr
	sub	R_VDDptr,[VDD_CB_Offset]
	cmp	R_Handle,R_VDDptr
	pop	R_VDDptr
	je	short AVP_Ok2
Debug_Out   "VDD ptr not correct, #&R_VDDptr"
AVP_Ok2:
	popfd
	ENDM

False   EQU     0
True    EQU     NOT False

;****************
; Macro for loading register with VDD CB ptr
SetVDDPtr MACRO VPTR_Reg, VMHand
 mov VPTR_Reg,[VDD_CB_Offset]
 add VPTR_Reg,ebx
ENDM


GROUP_VID	EQU	3

VID_DEFAULT		equ	200h
VIDNEC_NTMODEFF		equ	1h
VIDNEC_NTMODEFFC16	equ	2h
VIDNEC_NTDISPRW		equ	4h
VIDNEC_NTPAL		equ	8h
VIDNEC_NTGDC		equ	20h
VIDNEC_NTGDCTON		equ	40h
VIDNEC_NTGDCGON		equ	80h
VIDNEC_NTFONT		equ	100h
VIDNEC_CRTC		equ	200h
VIDNEC_XFERDATA		equ	400h
VIDNEC_XFERP0		equ	800h
VIDNEC_XFERP1		equ	1000h
VIDNEC_XFERP2		equ	2000h
VIDNEC_XFERP3		equ	4000h

PROPVID	STRUC
flVid	DW	?
flVidInit	DW	?
ifdef NEC_98
flVidNEC	DW	?
else
wReserved1	DW	?
endif
wReserved2	DW	?
wReserved3	DW	?
cScreenLines	DW	?
PROPVID	ENDS

    .LIST
