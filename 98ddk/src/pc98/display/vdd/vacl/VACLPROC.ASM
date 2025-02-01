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
PAGE	80,132

	.386p

;******************************************************************************
;				I N C L U D E S
;******************************************************************************

	MINIVDD EQU True

	.XLIST
	INCLUDE VMM.INC
	INCLUDE OPTTEST.INC
	INCLUDE MINIVDD.INC
	INCLUDE VDD98.INC
	INCLUDE VDDACC.INC
	INCLUDE VACL.INC
	INCLUDE DEBUG.INC
	.LIST


;******************************************************************************
;			       E X T E R N A L S
;******************************************************************************

VxD_DATA_SEG

	EXTRN	ABE_PhysWindow:DWORD
	EXTRN	VACL_Flags:DWORD
	EXTRN	BiosInfo_ACC:BYTE

ABE_External_ID	db	DUMMY_ID
ABE_Internal_ID	db	DUMMY_ID
PUBLIC	ABE_Internal_ID
VxD_DATA_ENDS


;******************************************************************************
;				 E Q U A T E S
;******************************************************************************



;******************************************************************************
;			 D A T A   S T R U C T U R E S
;******************************************************************************



;******************************************************************************
;				  M A C R O S
;******************************************************************************



;******************************************************************************
;			      L O C A L	  D A T A
;******************************************************************************

VxD_DATA_SEG

; *****************************************************************************	;
; VCLK parameters table ( SR0B,0C,0D,0E,1B,1C,1D,1E )				;
; *****************************************************************************	;

VCLK_STANDBY	dw	0F0Bh
		dw	0F0Ch
		dw	0F0Dh
		dw	0F0Eh
		dw	3F1Bh
		dw	3F1Ch
		dw	3F1Dh
		dw	3F1Eh

VCLK400_256WIDE	dw	550Bh
		dw	550Ch
		dw	550Dh
		dw	550Eh
		dw	351Bh
		dw	351Ch
		dw	351Dh
		dw	351Eh

VCLK480_256WIDE	dw	550Bh
		dw	550Ch
		dw	550Dh
		dw	550Eh
		dw	351Bh
		dw	351Ch
		dw	351Dh
		dw	351Eh

VCLK480_256Cb	dw	6D0Bh			;used
		dw	6D0Ch
		dw	6D0Dh
		dw	6D0Eh
		dw	3F1Bh			;used
		dw	3F1Ch
		dw	3F1Dh
		dw	3F1Eh

VCLK480_256	dw	660Bh			;used
		dw	660Ch
		dw	660Dh
		dw	660Eh
		dw	3B1Bh			;used
		dw	3B1Ch
		dw	3B1Dh
		dw	3B1Eh

VCLK768_256	dw	750Bh
VCLK768N_60	equ	760Ch
VCLK768N	dw	6E0Ch			;used, if (ref==60Hz) 6Eh->76h
		dw	660Dh
		dw	550Eh
		dw	341Bh
VCLK768D_60	equ	341Ch
VCLK768D	dw	2A1Ch			;used, if (ref==60Hz) 2Ah->34h
		dw	261Dh
		dw	361Eh

VCLK480_16M	dw	3A0Bh			;used
		dw	480Ch
		dw	560Dh
		dw	600Eh
		dw	161Bh			;used
		dw	231Ch
		dw	3D1Dh
		dw	3B1Eh

VCLK480_64K	dw	6D0Bh			;used
		dw	6D0Ch
		dw	6D0Dh
		dw	6D0Eh
		dw	3E1Bh			;used
		dw	3E1Ch
		dw	3E1Dh
		dw	3E1Eh

VCLK600_64K	dw	5F0Bh			;used
		dw	5F0Ch
		dw	5F0Dh
		dw	5F0Eh
		dw	221Bh			;used
		dw	221Ch
		dw	221Dh
		dw	221Eh

VCLK600_256	dw	530Bh			;used
		dw	530Ch
		dw	530Dh
		dw	530Eh
		dw	191Bh			;used
		dw	191Ch
		dw	191Dh
		dw	191Eh

; *****************************************************************************	;
; CRTC parameters table (CR00,01,02,03,04,05,06,07,09,10,11,12,13,15,16,1B)	;
; *****************************************************************************	;
CRTC480_256WIDE	dw	5800h
		dw	4901h
		dw	4A02h
		dw	9D03h
		dw	4E04h
		dw	1805h
		dw	0B06h
		dw	3E07h
		dw	4009h
		dw	0E510h
		dw	8711h		;write protect CR0-CR7
		dw	0DF12h
		dw	5013h
		dw	0E715h
		dw	0416h
		dw	221Bh

CRTC480_256	dw	5F00h
		dw	4F01h
		dw	5002h
		dw	8403h
		dw	5404h
		dw	8005h
		dw	0B06h
		dw	3E07h
		dw	4009h
		dw	0E510h
		dw	8711h		;write protect CR0-CR7
		dw	0DF12h
		dw	5013h
		dw	0E715h
		dw	0416h
		dw	221Bh

HTOTAL_REF60	equ	0A300h	;REF60Hz
CRTC768_256	dw	0A100h	;REF70Hz
		dw	7F01h
		dw	8002h
		dw	8503h
		dw	8504h
		dw	9605h
		dw	2406h
		dw	0FD07h
		dw	6009h
		dw	0210h
		dw	0811h
		dw	0FF12h
		dw	8013h
		dw	0015h
		dw	2616h
		dw	221Bh

CRTC480_16M	dw	5F00h
		dw	4F01h
		dw	5002h
		dw	8403h
		dw	5304h
		dw	9F05h
		dw	0B06h
		dw	3E07h
		dw	4009h
		dw	0E510h
		dw	8711h		;write protect CR0-CR7
		dw	0DF12h
		dw	0013h
		dw	0E715h
		dw	0416h
		dw	321Bh

CRTC480_64K	dw	5F00h
		dw	4F01h
		dw	5002h
		dw	8403h
		dw	5304h
		dw	9F05h
		dw	0B06h
		dw	3E07h
		dw	4009h
		dw	0E510h
		dw	8711h		;write protect CR0-CR7
		dw	0DF12h
		dw	0A013h
		dw	0E715h
		dw	0416h
		dw	221Bh

CRTC600_64K	dw	7F00h
		dw	6301h
		dw	6402h
		dw	8403h
		dw	6A04h
		dw	1A05h
		dw	07206h
		dw	0F007h
		dw	6009h
		dw	5810h
		dw	8C11h		;write protect CR0-CR7
		dw	5712h
		dw	0C813h
		dw	5715h
		dw	7416h
		dw	221Bh

CRTC600_256	dw	7F00h
		dw	6301h
		dw	6402h
		dw	8403h
		dw	6804h
		dw	1205h
		dw	6F06h
		dw	0F007h
		dw	6009h
		dw	5810h
		dw	8B11h		;write protect CR0-CR7
		dw	5712h
		dw	6413h
		dw	5715h
		dw	7116h
		dw	221Bh

; *****************************************************************************	;
; 640x480 (256 colors)								;
;										;
; Now, DRAM60nS is not supported on this res.					;
; *****************************************************************************	;
para_480_256	dd	OFFSET32 Miscellaneous
MISC_V256	db	0E3h
		dd	OFFSET32 Feature_Control_3DA
		db	00h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Pel_Mask
HDR_V256A	db	20h		;Bit7:0 = Palette DAC is VGA Compatible
		dd	OFFSET32 Sequencer
		dw	1206h	;enable All Extension Register (GD5428)
		dd	OFFSET32 CRT_3D4
		dw	0011h	;CR0-CR7 write enableout word data
;Sequencer:
		dw	0300h	;
		dw	0101h	;
		dw	0F02h	;
		dw	0E04h	;
		dw	1206h	;
GD5430_0	dw	0107h	; linear Addr (GD5430:0A107h)
		dw	0008h	;
		dw	300Fh	;
		dw	0012h	;
TUNE_V256_5430	equ	05816h	;
TUNE_V256_50	equ	0D316h	;
TUNE_V256	dw	0DA16h	;
MCLK_V256_5430	equ	201Fh	;
MCLK_V256_50	equ	1C1Fh	;
MCLK_V256	dw	191Fh	;
		dw	0FFFFh	;Sequencer END
;CRTC:
		dw	0008h	;
		dw	000Ah	;
		dw	000Bh	;
		dw	000Ch	;
		dw	000Dh	;
		dw	080Eh	;
		dw	200Fh	;
		dw	0014h	;
		dw	0E317h	;
		dw	0FF18h	;
		dw	3219h	;
CR1A_V256	dw	901Ah	;
		dw	0FFFFh	;CRTC END
		db	10h,41h
		db	11h,00h
		db	12h,0Fh
		db	13h,00h
		db	14h,00h
		db	0FFH	;COLORD END
;DODES:
		dw	000Bh	;Index 0Bh
		dw	4005h	;Index 05h
		dw	0000h	;Index 00h
		dw	0001h	;Index 01h
		dw	0002h	;Index 02h
		dw	0003h	;Index 03h
		dw	0004h	;Index 04h
		dw	0506h	;Index 06h
		dw	0F07h	;Index 07h
		dw	0FF08h	;Index 08h
		dw	0010h	;Index 10h
		dw	0FF11h	;Index 11h
		dw	0431h	;Index 31h
		dw	0FFFFh	;DODESEND
		dd	OFFSET32 DAC_Pel_Mask
HDR_V256B	db	20h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Write_Index
		db	00h

; *****************************************************************************	;
; 1024x768 (256 colors)								;
; *****************************************************************************	;
para_768_256	dd	OFFSET32 Miscellaneous
		db	0E7h
		dd	OFFSET32 Feature_Control_3DA
		db	00h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Pel_Mask
		db	20h		;Bit7:0 = Palette DAC is VGA Compatible
		dd	OFFSET32 Sequencer
		dw	1206h		;enable All Extension Register (GD5428)
		dd	OFFSET32 CRT_3D4
		dw	0011h		;CR0-CR7 write enable
;Sequencer:
		dw	0200h	;
		dw	0300h	;
		dw	0101h	;
		dw	0F02h	;
		dw	0E04h	;
		dw	1206h	;
GD5430_1	dw	0107h	; linear Addr (GD5430:0A107h)
		dw	0008h	;
		dw	300Fh	;
		dw	0012h	;
TUNE_X256_5430	equ	05816h	;
TUNE_X256_50	equ	0D916h	;
TUNE_X256_45	equ	0DB16h	;
TUNE_X256	dw	0DB16h	;
;		dw	0D916h	;
;		dw	0D716h	;
MCLK_X256_5430	equ	201Fh	;
MCLK_X256_50	equ	1C1Fh	;
MCLK_X256	dw	191Fh	;
		dw	0FFFFh	;Sequencer END
;CRTC:
		dw	0008h	;
		dw	000Ah	;
		dw	000Bh	;
		dw	000Ch	;
		dw	000Dh	;
		dw	080Eh	;
		dw	200Fh	;
		dw	8013h	;
		dw	0014h	;
		dw	0E317h	;
		dw	0FF18h	;
		dw	3219h	;
		dw	0E01Ah	;
		dw	0FFFFh	;CRTC END
;COLORD:
		db	10h,41h
		db	11h,00h
		db	12h,0Fh
		db	13h,00h
		db	14h,00h
		db	0FFH	;COLORD END
;DODES:
		dw	000Bh	;Index 0Bh
		dw	4005h	;Index 05h
		dw	0000h	;Index 00h
		dw	0001h	;Index 01h
		dw	0002h	;Index 02h
		dw	0003h	;Index 03h
		dw	0004h	;Index 04h
		dw	0506h	;Index 06h
		dw	0F07h	;Index 07h
		dw	0FF08h	;Index 08h
		dw	0010h	;Index 10h
		dw	0FF11h	;Index 11h
		dw	0431h	;Index 31h
		dw	0FFFFh	;DODESEND

		dd	OFFSET32 DAC_Pel_Mask
		db	20h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Write_Index
		db	00h

; *****************************************************************************	;
; 640x480 (16M colors)								;
; *****************************************************************************	;
;out/in/out
;out/in/out
para_480_16M	dd	OFFSET32 Miscellaneous
		db	0E3h
		dd	OFFSET32 Feature_Control_3DA
		db	00h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Pel_Mask
		db	0E5h		;Bit7:0 = Palette DAC is VGA Compatible
		dd	OFFSET32 Sequencer
		dw	1206h		;enable All Extension Register (GD5428)
		dd	OFFSET32 CRT_3D4
		dw	0011h		;CR0-CR7 write enable
;Sequencer:
		dw	0300h	;
		dw	0101h	;
		dw	0F02h	;
		dw	0E04h	;
		dw	1206h	;
GD5430_2	dw	0507h	; linear Addr (GD5430:0A507h)
		dw	0008h	;
		dw	300Fh	;
		dw	0012h	;
TUNE_V16M_5430	equ	05816h	;
TUNE_V16M_50	equ	0D816h	;
TUNE_V16M	dw	0DA16h	;
MCLK_V16M_5430	equ	201Fh	;
MCLK_V16M_50	equ	1C1Fh	;
MCLK_V16M	dw	191Fh	;
		dw	0FFFFh	;Sequencer END
;CRTC:
		dw	0008h	;
		dw	000Ah	;
		dw	000Bh	;
		dw	000Ch	;
		dw	000Dh	;
		dw	080Eh	;
		dw	200Fh	;
		dw	0013h	;
		dw	0014h	;
		dw	0E317h	;
		dw	0FF18h	;
		dw	3219h	;
		dw	901Ah	;
		dw	0FFFFh	;CRTC END
;in
;COLORD:
		db	10h,41h
		db	11h,00h
		db	12h,0Fh
		db	13h,00h
		db	14h,00h
		db	0FFH	;COLORD END
;DODES:
		dw	000Bh	;Index 0Bh
		dw	4005h	;Index 05h
		dw	0000h	;Index 00h
		dw	0001h	;Index 01h
		dw	0002h	;Index 02h
		dw	0003h	;Index 03h
		dw	0004h	;Index 04h
		dw	0506h	;Index 06h
		dw	0F07h	;Index 07h
		dw	0FF08h	;Index 08h
		dw	0010h	;Index 10h
		dw	0FF11h	;Index 11h
		dw	0431h	;Index 31h
		dw	0FFFFh	;DODESEND

		dd	OFFSET32 DAC_Pel_Mask
		db	0E5h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Write_Index
		db	00h

; *****************************************************************************	;
; 640x480 (64K colors)								;
; *****************************************************************************	;
;out/in/out
;out/in/out
para_480_64k	dd	OFFSET32 Miscellaneous
		db	0E3h
		dd	OFFSET32 Feature_Control_3DA
		db	00h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Pel_Mask
		db	0E1h		
		dd	OFFSET32 Sequencer
		dw	1206h	;enable All Extension Register (GD5428)
		dd	OFFSET32 CRT_3D4
		dw	0011h	;CR0-CR7 write enable
;Sequencer:
		dw	0300h   ;
		dw	0101h	;
		dw	0F02h	;
		dw	0E04h	;
		dw	1206h	;
GD5430_3	dw	0307h	;
		dw	0008h	;
		dw	300Fh	;
		dw	0012h	;
		dw	5816h	;
		dw	201Fh	;
		dw   	0FFFFh  ;
;CRTC:
		dw	0008h	;
		dw	000Ah	;
		dw	000Ch	;
		dw	000Dh	;
		dw	080Eh	;
		dw	200Fh	;
		dw	0014h	;
		dw	0E317h	;
		dw	0FF18h	;
		dw	3219h	;
		dw	901Ah	;
		dw   	0FFFFh  ;CRTC END
;COLORD:
	    	db	10h,41h
		db	11h,00h
		db	12h,0Fh
		db	13h,00h
		db	14h,00h
		db	0FFH    ;COLORD END
;DODES:
		dw	000Bh	;Index 0Bh
		dw	4005h	;Index 05h
		dw	0000h	;Index 00h
		dw	0001h	;Index 01h
		dw	0002h	;Index 02h
		dw	0003h	;Index 03h
		dw	0004h	;Index 04h
		dw	0506h	;Index 06h
		dw	0F07h	;Index 07h
		dw	0FF08h	;Index 08h
		dw	0010h	;Index 10h
		dw	0FF11h	;Index 11h
		dw	0431h	;Index 31h
                dw   	0FFFFh  ;DODESEND

		dd	OFFSET32 DAC_Pel_Mask
		db	0E1h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Write_Index
		db	00h
; *****************************************************************************	;
; 800x600 (64K colors)								;
; *****************************************************************************	;
;out/in/out
;out/in/out
para_800_64k	dd	OFFSET32 Miscellaneous
		db	0E3h
		dd	OFFSET32 Feature_Control_3DA
		db	00h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Pel_Mask
		db	0E1h
		dd	OFFSET32 Sequencer
		dw	1206h		;enable All Extension Register (GD5428)
		dd	OFFSET32 CRT_3D4
		dw	0011h		;CR0-CR7 write enable
;Sequencer:
		dw	0300h   ;
		dw	0101h	;
		dw	0F02h	;
		dw	0E04h	;
		dw	1206h	;
GD5430_4	dw	0307h	; linear Addr (GD5430:0A307h)
		dw	0008h	;
		dw	300Fh	;
		dw	0012h	;
		dw	05816h	;

MCLK_bb6M_5430	equ	201Fh	;
MCLK_bb6M_50	equ	1C1Fh	;
		dw	201Fh	;
		dw   	0FFFFh  ;Sequencer END
;CRTC:
		dw	0008h	;
		dw	000Ah	;
		dw	000Bh	;
		dw	000Ch	;
		dw	000Dh	;
		dw	080Eh	;
		dw	200Fh	;
		dw	0014h	;
		dw	0E317h	;
		dw	0FF18h	;
		dw	3219h	;
		dw	0A01Ah	;
		dw   	0FFFFh  ;CRTC END
;in
;COLORD:
	    	db	10h,41h
		db	11h,00h
		db	12h,0Fh
		db	13h,00h
		db	14h,00h
		db	0FFH    ;COLORD END
;DODES:
		dw	000Bh	;Index 0Bh
		dw	4005h	;Index 05h
		dw	0000h	;Index 00h
		dw	0001h	;Index 01h
		dw	0002h	;Index 02h
		dw	0003h	;Index 03h
		dw	0004h	;Index 04h
		dw	0506h	;Index 06h
		dw	0F07h	;Index 07h
		dw	0FF08h	;Index 08h
		dw	0010h	;Index 10h
		dw	0FF11h	;Index 11h
		dw	0431h	;Index 31h
                dw   	0FFFFh  ;DODESEND

		dd	OFFSET32 DAC_Pel_Mask
		db	0E1h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Write_Index
		db	00h

; *****************************************************************************	;
; 800x600 (256 colors)								;
; *****************************************************************************	;
;out/in/out
;out/in/out
para_800_256	dd	OFFSET32 Miscellaneous
		db	0E3h
		dd	OFFSET32 Feature_Control_3DA
		db	00h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Pel_Mask
		db	020h	;Bit7:0 = Palette DAC is VGA Compatible
		dd	OFFSET32 Sequencer
		dw	1206h	;enable All Extension Register (GD5428)
		dd	OFFSET32 CRT_3D4
		dw	0B11h	;CR0-CR7 write enable
;Sequencer:
		dw	0300h   ;
		dw	0101h	;
		dw	0F02h	;
		dw	0E04h	;
		dw	1206h	;
GD5430_5	dw	0107h	;
		dw	0008h	;
		dw	300Fh	;
		dw	0012h	;
TUNE_cc6M_5430	equ	05816h	;
TUNE_cc6M_50	equ	0D816h	;
TUNE_cc6M	dw	05816h	;
MCLK_cc6M_5430	equ	201Fh	;
MCLK_cc6M_50	equ	1C1Fh	;
MCLK_cc6M	dw	201Fh	;
		dw   	0FFFFh  ;Sequencer END
;CRTC:
		dw	0008h	;
		dw	000Ah	;
		dw	000Bh	;
		dw	000Ch	;
		dw	000Dh	;
		dw	080Eh	;
		dw	200Fh	;
		dw	0014h	;
		dw	0E317h	;
		dw	0FF18h	;
		dw	3219h	;
		dw	0A01Ah	;
		dw   	0FFFFh  ;CRTC END
;in
;COLORD:
	    	db	10h,41h
		db	11h,00h
		db	12h,0Fh
		db	13h,00h
		db	14h,00h
		db	0FFH    ;COLORD END
;DODES:
		dw	000Bh	;Index 0Bh
		dw	4005h	;Index 05h
		dw	0000h	;Index 00h
		dw	0001h	;Index 01h
		dw	0002h	;Index 02h
		dw	0003h	;Index 03h
		dw	0004h	;Index 04h
		dw	0506h	;Index 06h
		dw	0F07h	;Index 07h
		dw	0FF08h	;Index 08h
		dw	0010h	;Index 10h
		dw	0FF11h	;Index 11h
		dw	0431h	;Index 31h
                dw   	0FFFFh  ;DODESEND

		dd	OFFSET32 DAC_Pel_Mask
		db	020h
		dd	OFFSET32 DAC_Pel_Mask
		db	0FFh
		dd	OFFSET32 DAC_Write_Index
		db	00h


; *****************************************************************************	;
; Graphics Controller Registers							;
; *****************************************************************************	;
grfx_regs	dw 0000h
		dw 0001h
		dw 0002h
		dw 0003h
		dw 0004h
		dw 0005h
		dw 0506h
		dw 0F07h
		dw 0FF08h

; *****************************************************************************	;
; Pallete Data									;
; *****************************************************************************	;
pallete_data	db	00,00,00	;palette No.1
		DB	00,00,14H	;palette No.2
		DB	00,00,18H	;palette No.3
		DB	00,00,1CH	;palette No.4
		DB	00,00,23H	;palette No.5
		DB	00,00,2AH	;palette No.6
		DB	00,00,31H	;palette No.7
		DB	00,00,38H	;palette No.8
		DB	00,00,3FH	;palette No.9
		DB	07,07,3FH	;palette No.10
		DB	0FH,0FH,3FH	;palette No.11
		DB	17H,17H,3FH	;palette No.12
		DB	1FH,1FH,3FH	;palette No.13
		DB	27H,27H,3FH	;palette No.14
		DB	2DH,2DH,3FH	;palette No.15
		DB	2FH,2FH,3FH	;palette No.16

PARA_MODE	db	0		; for SetVCLK()/SetCRTC(), Standby/Overlay
BOOT_CHK	db	0		; 0:windows boot 1:re-initialize

public	PRODUCT_ID, SessionMode, builtin_chip
PRODUCT_ID	db	0
Ts_LCD	db	0
_VRAMSel	dw	0		;
refreshRate	dw	0
SessionMode	dw	0
builtin_chip	db	0

; timing trim value for each LCD

			; index 80h,81h,82h	; analog I/F TFT LCD(Np)
INIT_Np_LCD_TBL	db	02h,01h,01h	; 256 colors value
;			db	01h,01h,01h	; 16M colors value
			db	01h,01h,00h	; 16M colors value

						; timing trim value for LCD
TERM_Np_LCD_TBL	db	00h,00h,00h	;

VxD_DATA_ENDS


;******************************************************************************
;			       M A I N	 C O D E
;******************************************************************************
VxD_CODE_SEG

PPROC	GD5428_INIT near

	pushad

	cld
	mov	bl, byte ptr [SessionMode]

	mov	esi, offset32 para_480_256
	cmp	bl, vSM_480_256			; 640x480 256 colors
	je	short Set256para
	mov	esi, offset32 para_768_256
	cmp	bl, vSM_768_256			; 1024x768 256 colors
	je	short Set256para
	mov	esi, offset32 para_480_16M
	cmp	bl, vSM_480_16M			; 640x480 16M colors
	je	short Set16Mpara

	mov     esi, offset32 para_800_256
	cmp	bl, vSM_600_256			; 800x600 256 colors
	je	short Set256para

	mov     esi, offset32 para_480_64k
	cmp	bl, vSM_480_64K			; 640x480 64k colors
	je	short Set16Mpara

	mov     esi, offset32 para_800_64k
	cmp	bl, vSM_600_64K			; 800x600 64k colors
	je	short Set16Mpara

	int	1				; invalid SessionMode !!!
	jmp	short EndSetpara

Set256para:
	call	SetRegistersSTD
	call	Set_Grfx_regs
	jmp	short EndSetpara
Set16Mpara:
	call	SetRegistersSTD

EndSetpara:
	popad
	ret
GD5428_INIT	endp

; *****************************************************************************	;
; Set_Grfx_regs									;
;										;
; Input:	nothing								;
;										;
; Output:	nothing								;
;										;
; *****************************************************************************	;
Set_Grfx_regs	proc	near
	push	ecx
	push	edx
	push	esi

	mov	esi, offset32 grfx_regs
	mov	edx, [Graphics_Controller]
	mov	ecx, 9
@@:
	outsw
	IOdelay
	loop	@b

	pop	esi
	pop	edx
	pop	ecx
	ret
Set_Grfx_regs	endp

; *****************************************************************************	;
; SetRegistersSTD								;
;										;
; Input:	SI = DataAddress						;
;										;
; Output:	nothing								;
;										;
; *****************************************************************************	;
SetRegistersSTD	proc	near

	lodsd			;Miscellanneous Output Register
	mov	dx, [eax]		;Port 3C2h(CA2h)
	outsb
	lodsd			;Feature Control Register
	mov	dx, [eax]		;Port 3DAh(DAAh)
	outsb

	lodsd			;Pixel Mask Register
	mov	dx, [eax]		;Port 3C6h(CA6h)
	outsb

	mov	edx,[DAC_Pel_Mask]	;Pixel Mask Register
	in	al,dx		;Port 3C6h(CA6h)
	in	al,dx
	in	al,dx
	in	al,dx

	lodsd			;Pixel Mask Register
	mov	dx, [eax]		;Port 3C6h(CA6h)
	outsb

	lodsd			;Unlock ALL Extensions Register(SR6)
	mov	dx, [eax]		;Index 3C4h(CA4h) Data 3C5h(CA5h)
	outsw

	lodsd			;Vertical Sync End Register(CR11)
	mov	dx, [eax]		;Index 3D4h(DA4h) Data 3D5h(DA5h)
	outsw

;--------- Port 3C4H/3C5H Sequencer Reg ----------------

	mov	edx,[Sequencer]	;Sequencer Registers
sloop:					;Index 3C4h(CA4h) Data 3C5h(CA5h)
	lodsw
	cmp	ax,0FFFFh	;Table End?
	je	short sexit
	out	dx,ax
	jmp	sloop
sexit:
	test	[PRODUCT_ID],fID_GD5430
	jz	short @f
	mov	al, 05h		;SR index = 5
	out	dx, al		;avoid GD5430 BUG
@@:

;--------- Port 3CEH/3CFH Graphics Reg ----------------

	mov	edx,[CRT_3D4]	;CRT Controller Registers
gloop:					;Index 3D4h(DA4h) Data 3D5h(DA5h)
	lodsw
	cmp	ax,0FFFFh	;Table End?
	je	short gexit
	out	dx,ax
	jmp	gloop
gexit:
	call	SetCRTC		;Set Sync parameters

	call	SetARtoggle	; set AR toggle to 'index'
	mov	edx,[attribute_controller]
hloop:					;Attribbute Controller Registers
	lodsb			;Write 3C0h(CA0h) Read 3C1h(CA1h)
	cmp	al,0FFh		;Table End?
	je	short hexit
	out	dx,al
	jmp	hloop
hexit:

;------------------------------------------------------

	mov	edx,[Graphics_Controller]
Iloop:					;Graphics Controller Registers
	lodsw			;Index 3CEh(CAEh) Data 3CFh(CAFh)
	cmp	ax,0FFFFh	;Table End?
	je	short iexit
	out	dx,ax
	jmp	Iloop
iexit:

	mov	edx,[DAC_Pel_Mask]	;Pixel Mask Register
	in	al,dx		;Port 3C6h(CA6h)
	in	al,dx
	in	al,dx
	in	al,dx

	lodsd			;Pixel Mask Register
	mov	dx, [eax]	;Port 3C6h(CA6h)
	outsb

	lodsd			;Pixel Mask Register
	mov	dx, [eax]	;Port 3C6h(CA6h)
	outsb

	lodsd			;Pixel AddressRegister(Write Mode)
	mov	dx, [eax]	;Port 3C8h(CA8h)
	outsb

	ret
SetRegistersSTD	endp

FullBandwidthOFF	proc	near
		push	eax
		push	edx

		mov	edx, [Sequencer]	;Sequencer Registers Index

		in	al, dx		;Save Index No.
		push	eax		;

		mov	al, 1		;Clocking Mode Register(SR01)
		out	dx, al

		inc	dx		;Sequencer Registers Data

		in	al, dx
		and	al, not 20h	;Full Bandwidth Bit OFF
		out	dx, al

		dec	dx		;Sequencer Registers Index

		pop	eax		;Restore Index No.
		out	dx, al		;

		pop	edx
		pop	eax
		ret
FullBandwidthOFF	endp

; *****************************************************************************	;
; FullBandwidthON								;
;										;
; Input:	nothing								;
;										;
; Output:	nothing								;
;										;
; *****************************************************************************	;
FullBandwidthON	proc	near
		push	eax
		push	edx

		mov	edx, [Sequencer]	;Sequencer Registers Index

		in	al, dx		;Save Index No.
		push	ax		;

		mov	al, 1		;Clocking Mode Register(SR01)
		out	dx, al

		inc	dx		;Sequencer Registers Data

		in	al, dx
		or	al, 20h		;Full Bandwidth Bit ON
		out	dx, al

		dec	dx		;Sequencer Registers Index

		pop	ax		;Restore Index No.
		out	dx, al		;

		pop	edx
		pop	eax
		ret
FullBandwidthON	endp

; *****************************************************************************	;
; Clear_Screen									;
;										;
; Input:	nothing								;
;										;
; Output:	nothing								;
;										;
; *****************************************************************************	;
PPROC	Clear_Screen near


		cld
		pushad
		call	FullBandwidthOFF	;Start VRAM refresh

		mov	ebx, [ABE_PhysWindow]

ifndef FORCE_BANK
		test	byte ptr [Product_ID], fID_GD5430
		jnz	Clear_Screen_30
endif ; FORCE_BANK

		xor	eax, eax
		mov	ecx, 256
loopt:
		mov	edx, [Graphics_Controller]	;Index
		mov	al, 09h			;Offset Register 0 (GR9)
		out	dx, al			;Data
		inc	dx
		mov	al, ah
		out	dx, al

		push	ecx
		mov	ecx, 1000h		;4k Bytes
		mov	edi, ebx		;Offset Reset
		xor	al, al
		rep stosb			;Clear the video
		pop	ecx

		inc	ah			;Next Bank
		loop	loopt

		popad
		ret

ifndef FORCE_BANK
Clear_Screen_30:
		mov	al, byte ptr [SessionMode]
		mov	ecx, 640*480
		cmp	al, vSM_480_256
		je	@f
		mov	ecx, 1024*600
		cmp	al, vSM_600_256
		je	@f
		mov	ecx, 1024*768
		cmp	al, vSM_768_256
		je	@f
		mov	ecx, 1600*600
		cmp	al, vSM_600_64K
		je	@f
		mov	ecx, 2048*480
		cmp	al, vSM_480_16M
		je	@f
		mov	ecx, 1280*480
;		cmp	al, vSM_480_64K
;		Trace_Out "Invalid session mode!"


@@:		mov	edi, ebx		;Offset Reset
		xor	al, al
		rep stosb
endif ;FORCE_BANK

		popad
		ret

Clear_Screen	endp

PPROC	Set_GD5428 near


	pushad
	cld

	jmp	toCIRRUS

Set_GD5428	endp

PPROC	PC98toCIRRUS near

	pushad
	cld

	mov	bl,[PRODUCT_ID]
	cmp	bl,vID_Cb
	je	short @f
	cmp	bl,vID_Cf_Cx
	je	short @f
	jmp	short NotInitVO
@@:	or	[PARA_MODE],01h	; select Mx4 VCLK
	mov	bl,byte ptr [SessionMode]
	mov	ch,11110111b		; SCM0=0
	mov	cl,00110000b		; SCM1=1 No Expand, DOFF=1 No Ext-Video
	cmp	bl,vSM_480_256		; ? 640x480 256
	je	short @f

;Over scan is 640x480 only.
;
	mov	edx,[GlueIndexReg]
	mov	al,20h
	out	dx,al
	inc	dx
	mov	al,11110000b		; No P-bus drive, No out DCLK, No Ext-Video
	out	dx,al
	jmp	NotInitVO
@@:	mov	edx,[GlueIndexReg]
	mov	al,20h
	out	dx,al
	inc	dx
	in	al,dx
	and	al,ch
	or	al,cl
	out	dx,al
NotInitVO:

;--------------------------------------------------------------------------
; Modify parameters in table for each Subsystem H/W variations
;
; DRAM speed lank 70 nS : PC-9821Bp/Bs/Be/Ts only
;		  60 nS : the other
; Refresh rate	  70 Hz : standard
;		  60 Hz : hidden (system.ini: [display] refreshrate=60)
;			  60Hz is available on 1024x768 256 mode only.
; Parameters depending on GD5428/GD5430
;--------------------------------------------------------------------------
	mov	word ptr [TUNE_X256],TUNE_X256_45	; SR16 (default)

	mov	bl,[PRODUCT_ID]
	cmp	bl,vID_Bp_Bs_Be
	je	short DRAM70
	cmp	bl,vID_Ts
	je	short DRAM70
DRAM60:
	mov	word ptr [TUNE_X256],TUNE_X256_50	; SR16 (-=2)
	mov	word ptr [MCLK_X256],MCLK_X256_50	; SR1F
	mov	word ptr [TUNE_V16M],TUNE_V16M_50
	mov	word ptr [MCLK_V16M],MCLK_V16M_50
DRAM70:
	cmp	[refreshRate],0			; ? refresh = 70Hz
	je	short @f
REF60:
	mov	word ptr [VCLK768N],VCLK768N_60	; SR0C
	mov	word ptr [VCLK768D],VCLK768D_60	; SR1C
	sub	byte ptr [TUNE_X256+1],2		; SR16 (-=2)
	mov	word ptr [CRTC768_256],HTOTAL_REF60
@@:
	test	bl,fID_GD5430
	jz	short @f
	mov	word ptr [TUNE_V256],TUNE_V256_5430	; SR16
	mov	word ptr [MCLK_V256],MCLK_V256_5430     ; SR1F
	mov	word ptr [TUNE_X256],TUNE_X256_5430
	mov	word ptr [MCLK_X256],MCLK_X256_5430
	mov	word ptr [TUNE_V16M],TUNE_V16M_5430
	mov	word ptr [MCLK_V16M],MCLK_V16M_5430
@@:
RE_INIT:

;--------------------------------------------------------------------------
; Wait 0.16 sec
;--------------------------------------------------------------------------
	call	Wait01sec
	call	Wait01sec

;--------------------------------------------------------------------------
; Blank 9800 core display
;--------------------------------------------------------------------------
	mov	al,00Eh			; break 9801 core display
	out	068h,al

;--------------------------------------------------------------------------
; Disable 9800 core display
;
; This is necessary on internal mother board CIRRUS system only !
; On B3BOARD, it is not necessary.
;--------------------------------------------------------------------------
	cmp	[PRODUCT_ID], vID_B3
	je	short @f

	mov	al,007h			; enable Ex-bits
	out	06Ah,al
	mov	al,08Fh			; disable 9801 core control VRAM
	out	06Ah,al
	mov	al,006h			; disable Ex-bits
	out	06Ah,al

	cmp	[PRODUCT_ID], vID_Cx2				
	jz	PTC_Cb2_1                                       
	cmp	[PRODUCT_ID], vID_Cb2                           
	jz	PTC_Cb2_1                                       
	cmp	[PRODUCT_ID], vID_Xe10                        
	jz	PTC_Cb2_1                                       
	jmp	@f                                              
PTC_Cb2_1:                                                      
	mov	dx, 0fach					
	mov	al, 02h						
	out	dx, al			; disable core graph    

@@:

;--------------------------------------------------------------------------
; Switch 9800 Coresystem to CIRRUS Subsystem
;
; Switch VRAM share, I/O, CRT outlet
; Never access core VRAM from here !!!
;--------------------------------------------------------------------------
	mov	al,003h			; activate cirrus
	mov	edx,[GlueIndexReg]	; switch to cirrus to display
	out	dx,al
	mov	edx,[GlueDataReg]
	out	dx,al

;--------------------------------------------------------------------------
; Wait 600 nS
;--------------------------------------------------------------------------
	IOdelay				; wait over 600nS

toCIRRUS:
;--------------------------------------------------------------------------
; Timing trim for LCD (on CIRRUS Subsystem)
;--------------------------------------------------------------------------
; a) for PC-9821Ts
;    set display mode for LCD through DOT clock register.
	mov	bl,[PRODUCT_ID]
	cmp	bl,vID_Ts
	jne	short NOTs

	mov	ah,01b
	cmp	[SessionMode], vSM_480_256	; 640x480 256 colors
	je	short @f
	mov	ah,11b			;(this value will break display on LCD)
@@:
	mov	dx,009A8h
	in	al,dx
	mov	byte ptr [Ts_LCD],al	; back up original value
	mov	al,ah
	out	dx,al				; set dot clock for LCDC
	jmp	short NOLCD1
NOTs:

; b) for PC-9821Np/Es (analog I/F LCD)
;    modify timing trimmer
	cmp	bl,vID_Np_Es
	jne	short NOLCD1

	mov	esi,offset32 INIT_Np_LCD_TBL	; 256 colors trim table
	cmp	[SessionMode], vSM_480_256	; 640x480 256 colors
	je	short @f
	cmp	[SessionMode], vSM_768_256	; 1024x768 256 colors
	je	short @f
	add	esi,3				; 16M colors trim table
@@:
	mov	edx,[GlueIndexReg]
	mov	al,80h			; DE timing 1
	out	dx,al
	mov	dl,0ABh
	lodsb
	out	dx,al
	mov	dl,0AAh

	mov	al,81h			; DE timing 2
	out	dx,al
	mov	dl,0ABh
	lodsb
	out	dx,al

	mov	dl,0AAh
	mov	al,82h			; DCLK timing
	out	dx,al
	mov	dl,0ABh
	lodsb
	out	dx,al
	mov	dl,0AAh
NOLCD1:

;--------------------------------------------------------------------------
; Wake up CIRRUS GD5428 Subsystem.
; GD5430 SubSystem doesn't necessary this.
;--------------------------------------------------------------------------
	test	[PRODUCT_ID],fID_GD5430
	jnz	short @f

	xor	al,al
	mov	edx,[GD5428_0094]
	out	dx,al
	mov	al,001h			; 0:sleep / 1:wake
	mov	edx,[GD5428_0102]
	out	dx,al
	mov	al,020h
	mov	edx,[GD5428_0094]
	out	dx,al

	mov	edx, [Sequencer]
	mov	ax, 1206h
	out	dx, ax			; enable All Extensions Register
@@:
;--------------------------------------------------------------------------
; Set CRTC mode to 'color mode'
;
; If we do not set this mode, it may be mono mode.
; CR I/O port addrres is decided by the mode.
; So, we must set this mode.
;--------------------------------------------------------------------------
	mov	edx,[Graphics1_Pos]
	in	al,dx
	or	al,01h			; set color mode
	mov	edx,[Miscellaneous]
	out	dx,al

;--------------------------------------------------------------------------
; Blank CIRRUS display
;--------------------------------------------------------------------------
	call	SetARtoggle		; set AR toggle to 'index'
	mov	edx,[attribute_controller]
	mov	al,011h			; 
	out	dx,al			; Video disable & AR index=11h
	xor	al,al			; 
	out	dx,al			; border color = black
					; next out to AR is 'index'

;--------------------------------------------------------------------------
; Initialize CIRRUS part.1 ( for Video Overlay )
;--------------------------------------------------------------------------
	call	SetVCLK		; set VCLK ( SR0B,0C,0D,0E,1B,1C,1D,1E )

	cmp	[PRODUCT_ID], vID_Cx2
	jz	Cx2Overlay
	cmp	[PRODUCT_ID], vID_Cb2
	jz	Cx2Overlay
	jmp	next_Ov_check
Cx2Overlay:
	mov	edx,[Sequencer]
	mov	ax,0018h
	out	dx,ax			; SR18 = 00h
	jmp	EndInitPart1
next_Ov_check:
	call	ChkCfVO
	jc	NotOverlay		; if CY, VideoOverlay isn't available

SetOverlay:
	mov	edx,[Sequencer]
	mov	ax,0018h
	out	dx,ax			; SR18 = 00h

	mov	edx,[GlueIndexReg]
	mov	al,20h			; Overlay control (Index 20h)
	out	dx,al
	mov	edx,[GlueDataReg]
	in	al,dx
	and	al,00111110b		; P-bus drive, out DCLK
	or	al,00000001b		; HDR=565(64K)
	out	dx,al			; This b0 must be equal b0 of HDR !!!

; MISC & HDR is set on 'Initialize CIRRUS Part.2'
; change value in table for Part.2

	or	byte ptr [MISC_V256],00001100b
	or	byte ptr [CR1A_V256+1],00001000b
	mov	byte ptr [HDR_V256A],0C1h
	mov	byte ptr [HDR_V256B],0C1h
	jmp	short EndInitPart1

NotOverlay:
	mov	edx,[Sequencer]
	mov	ax,0018h		; enable DCLK for LCD
	mov	bl,[PRODUCT_ID]
	cmp	bl,vID_Ts
	je	short @f
	cmp	bl,vID_Ns
	je	short @f
	cmp	bl,vID_Np_Es
	je	short @f
	cmp	bl,vID_Ne2
	je	short @f
	cmp	bl,vID_Nd
	je	short @f
	or	ah,01000000b		; disable DCLK
@@:	out	dx,ax			; SR18 = 40h or 00h (00 for LCD)

EndInitPart1:

;--------------------------------------------------------------------------
; Initialize CIRRUS part.2
;--------------------------------------------------------------------------
	call	GD5428_INIT

;--------------------------------------------------------------------------
; Cursor's palette re-initialize
;--------------------------------------------------------------------------

	mov	edx,[Sequencer]
	mov	al,12h			; CL_CURSOR_ATT
	out	dx,al
	inc	dx
	in	al,dx
	or	al,010b			; CL_EXT_DAC_ENB
	out	dx,al
	mov	al,0
	mov	edx,[DAC_Write_Index]	;RAMDAC INDEX
	out	dx,al			;load it
	mov	edx,[DAC_Write_Data]	;RAMDAC DATA
	mov	al,0h
	out	dx,al			;write red
	out	dx,al			;write green
	out	dx,al			;write blue
	mov	al,0ffh
	mov	edx,[DAC_Write_Index]	;RAMDAC INDEX
	out	dx,al			;load it
	mov	edx,[DAC_Write_Data]	;RAMDAC DATA
	mov	al,03Fh
	out	dx,al			;write red
	out	dx,al			;write green
	out	dx,al			;write blue
	mov	edx,[Sequencer]
	mov	al,12h			; CL_CURSOR_ATT
	out	dx,al
	inc	dx
	in	al,dx
	and	al,NOT 010b		; CL_EXT_DAC_ENB
	out	dx,al

;--------------------------------------------------------------------------
; Wait 0.1 sec
;--------------------------------------------------------------------------
	call	Wait01sec

;--------------------------------------------------------------------------
; Start CIRRUS display
;--------------------------------------------------------------------------
	call	SetARtoggle		; set AR toggle to 'index'
	mov	edx,[attribute_controller]
	mov	al,20h			; video enable
	out	dx,al

	popad
	ret
PC98toCIRRUS	endp


PPROC	CIRRUStoPC98 near

	pushad
	cld

; Check system, CIRRUS Subsystem available ?

	mov	edx,[GlueIndexReg]
	mov	al,3
	out	dx,al
	mov	edx,[GlueDataReg]
	in	al,dx
	test	al,2
	jz	CIRRUStoPC98_ret

;--------------------------------------------------------------------------
; Wait 0.16 sec
;--------------------------------------------------------------------------
	call	Wait01sec
	call	Wait01sec

;--------------------------------------------------------------------------
; Blank CIRRUS display
;--------------------------------------------------------------------------
	call	SetARtoggle
	mov	al,011h			;
	mov	edx,[attribute_controller]
	out	dx,al
	xor	al,al			; 4)) turn black overscan color
	out	dx,al

;--------------------------------------------------------------------------
; Enable DCLK
;--------------------------------------------------------------------------
	mov	edx,[Sequencer]
	mov	ax,1206h
	out	dx,ax			; enable All Extensions Register
	mov	edx,[Sequencer]
	mov	al,18h
	out	dx,al			; SR18
	inc	dx
	in	al,dx
	and	al,not 01000000b
	out	dx,al			; enable DCLK drive of GD5428

;--------------------------------------------------------------------------
; Set Stand_by state parameters and DAC reset (DAC share)
;--------------------------------------------------------------------------
	mov	bl,[PRODUCT_ID]
; DAC terminate(its not necessary on TR25W,but useful on power saving)
	test	bl,fID_GD5430
	jnz	short NotDACreset	;it isn't necessary on GD5430 system
	cmp	bl,vID_B3
	je	short NotDACreset	;it isn't necessary on B3BOARD

	cmp	bl,vID_Ns
	je	short StandbyCLK
	cmp	bl,vID_Np_Es
	je	short StandbyCLK
	cmp	bl,vID_Ne2
	je	short StandbyCLK
	cmp	bl,vID_Nd
	je	short StandbyCLK
	jmp	short DACreset
StandbyCLK:
	mov	edx,[Sequencer]
	mov	ax,041Fh
	out	dx,ax				;MCLK to 7.10MHz
	or	byte ptr [PARA_MODE],080h	;set Standby mode
	call	SetVCLK				;VCLK to 3.46MHz
	and	byte ptr [PARA_MODE],07Fh
DACreset:
	mov	ah,0FFh			;DAC Standby
	cmp	bl,vID_Np_Es
	je	short @f
	mov	ah,080h			;DAC 5-5-5 sierra except Np
@@:
	mov	edx,[DAC_Pel_Mask]
	in	al,dx
	in	al,dx
	in	al,dx
	in	al,dx
	mov	al,ah			;set DAC 5-5-5 / Stand_by
	out	dx,al
NotDACreset:

;--------------------------------------------------------------------------
; Sleep CIRRUS GD5428 Subsystem
;--------------------------------------------------------------------------
	mov	bl,[PRODUCT_ID]
	test	bl,fID_GD5430
	jnz	short SleepGD5430
					; Its not necessary on GD5430 subsystem
	xor	al,al			;
	mov	edx,[GD5428_0094]	; initialize cirrus system
	out	dx,al
	mov	al,000h			; 0:sleep / 1:wake
	mov	edx,[GD5428_0102]
	out	dx,al
	mov	al,020h
	mov	edx,[GD5428_0094]
	out	dx,al
	jmp	short @f
SleepGD5430:
	mov	edx,[Vssm_SleepAdress]	;
	mov	al,0			; sleep GD5430
	out	dx,al			; its NOT necessary to re-active this bit
@@:

;--------------------------------------------------------------------------
; Switch CIRRUS Subsystem to 9800 Coresystem
;--------------------------------------------------------------------------
	mov	edx,[GlueIndexReg]		;4) disable CIRRUS system
	mov	al,003h
	out	dx,al
	inc	dx
	xor	al,al
	out	dx,al

;--------------------------------------------------------------------------
; Wait 340 nS
;--------------------------------------------------------------------------
	IOdelay				;5) wait over 340nS

;--------------------------------------------------------------------------
; Enable 9800 core display
;
; This is necessary on internal mother board CIRRUS system only !
; On B3BOARD, it is not necessary.
;--------------------------------------------------------------------------
	cmp	[PRODUCT_ID],vID_B3
	je	short @f

	mov	al,007h			; enable Ex-bits
	out	06Ah,al
	mov	al,08Eh			; enable 9801 core control VRAM
	out	06Ah,al
	mov	al,006h			; disable Ex-bits
	out	06Ah,al

	cmp	[PRODUCT_ID], vID_Cx2				
	jz	CTP_Cx2_1					
	cmp	[PRODUCT_ID], vID_Cb2				
	jz	CTP_Cx2_1					
	cmp	[PRODUCT_ID], vID_Xe10				
	jz	CTP_Cx2_1					
	jmp	@f						
CTP_Cx2_1:							
	mov	dx, 0fach					
	mov	al, 0						
	out	dx, al						

@@:

;--------------------------------------------------------------------------
; Timing trim for LCD (on 9800 core)
;--------------------------------------------------------------------------
	mov	bl,[PRODUCT_ID]
	cmp	bl,vID_Ts
	jne	short @f			; no,others
	xor	al, al
	mov	dx,09A8h
	out	dx,al
@@:
	cmp	bl,vID_Np_Es
	jne	short @f
	mov	esi,offset32 TERM_Np_LCD_TBL
	mov	edx,[GlueIndexReg]
	mov	al,80h			; DE timing 1
	out	dx,al
	mov	dl,0ABh
	lodsb
	out	dx,al
	mov	dl,0AAh
	mov	al,81h			; DE timing 2
	out	dx,al
	mov	dl,0ABh
	lodsb
	out	dx,al
	mov	dl,0AAh
	mov	al,82h			; DCLK timing
	out	dx,al
	mov	dl,0ABh
	lodsb
	out	dx,al
	mov	dl,0AAh
@@:

;--------------------------------------------------------------------------
; Wait 0.1 sec
;--------------------------------------------------------------------------
	call	Wait01sec

;--------------------------------------------------------------------------
; Start 9800 core display
;--------------------------------------------------------------------------
	mov	al,00Fh			; 98core display start
	out	068h,al

CIRRUStoPC98_ret:

	popad
	ret
CIRRUStoPC98	endp


PPROC	Wait01sec near

	push	eax
	push	ecx

	cmp	byte ptr [PRODUCT_ID],vID_B3
	jne	short Wait_timestamp	; built-in -> using time stamper

	mov	ecx,6
Wait_vsync:				; option -> count core VSYNC 6 times
	in	al,060h
	test	al,020h
	jnz	short Wait_vsync

	pushad
	VMMCall	Get_Cur_VM_Handle			; Get current vm handle
	TestMem [ebx.CB_VM_Status], VMStat_Suspended	; Q: Suspended ?
	jnz	short @f				;    Y: Exit
	VMMCall Begin_Nest_Exec			; Get ready for software ints
	VMMCall	Resume_Exec			;
	VMMCall End_Nest_Exec			; All done with software ints
@@:
	popad
@@:
	in	al,060h
	test	al,020h
	jz	@b


	loop	Wait_vsync
	jmp	short EndWait

Wait_timestamp:
	mov	ecx,8		; wait 0.1 sec(8x13.3mS)
wait_timestamp_A:
	in	ax,05Eh
	test	ax,8h		; b3 toggle by 13.3mS */
	jnz	short wait_timestamp_A

	pushad
	VMMCall	Get_Cur_VM_Handle			; Get current vm handle
	TestMem [ebx.CB_VM_Status], VMStat_Suspended	; Q: Suspended ?
	jnz	short @f				;    Y: Exit
	VMMCall Begin_Nest_Exec			; Get ready for software ints
	VMMCall	Resume_Exec			;
	VMMCall End_Nest_Exec			; All done with software ints
@@:
	popad

wait_timestamp_B:
	in	ax,05Eh
	test	ax,8h		; b3 toggle by 13.3mS */
	jz	short wait_timestamp_B
	loop	wait_timestamp_A

EndWait:

	pop	ecx
	pop	eax
	ret
Wait01sec	endp

SetARtoggle	proc	near
	push	eax
	push	edx

	mov	edx,[CRT_3D4]
	mov	al,024h		; CR24
	out	dx,al
	inc	dx
	in	al,dx
	test	al,080h		; check b7 on
	jz	short @f	; confirm Attr Controller Toggle
	mov	edx,[attribute_controllerR]
	in	al,dx
	dec	dx
	out	dx,al
@@:
	pop	edx
	pop	eax
	ret
SetARtoggle	endp

PPROC	SetLCD_ON near


		push	eax
		push	edx

		mov	al,byte ptr [PRODUCT_ID]

		cmp	al,vID_Np_Es
		je	short LON_B
		cmp	al,vID_Ts
		je	short LON_A
		cmp	al,vID_Ns
		je	short LON_A
		cmp	al,vID_Ne2
		je	short LON_A
		cmp	al,vID_Nd
		je	short LON_A
		jmp	short LON_END
LON_A:
; Ts (Ne2/Ns..)
		mov	dx, 08f0h
		mov	ax, 0052h
		out	dx, ax
		out	05fh, al
		out	05fh, al
		mov	dx, 08f2h
		in	ax,dx		; stay other bits
		and	ax,not 0080h	; in this port, bit conflict with PCMCIA !!!
		out	dx, ax
		jmp	short LON_END
LON_B:
; Np
		mov	dx, MOTHER_PORTBASE	; GlueIndexReg
		mov	al,083h			; mistake index
		out	dx,al
		mov	dx, MOTHER_PORTBASE+1	; GlueDataReg
		in	al,dx
		mov	al,03h			; as same as 8f0/2h(52h)<-00h
		out	dx,al
LON_END:
		pop	edx
		pop	eax
		ret
SetLCD_ON	endp

PPROC	SetLCD_OFF near

		push	eax
		push	edx

		mov	al,byte ptr [PRODUCT_ID]

		cmp	al,vID_Np_Es
		je	short LOFF_B
		cmp	al,vID_Ts
		je	short LOFF_A
		cmp	al,vID_Ns
		je	short LOFF_A
		cmp	al,vID_Ne2
		je	short LOFF_A
		cmp	al,vID_Nd
		je	short LOFF_A
		jmp	short LOFF_END
LOFF_A:
; PC-9821Ts (Ns/Ne2/Nd)
; This routine works on Ns/Ne2/Nd, but same function is defind glue(85h)
		mov	dx, 08f0h
		mov	ax, 0052h
		out	dx, ax
		out	05fh, al
		out	05fh, al
		mov	dx, 08f2h
		in	ax,dx		; stay other bits
		or	ax, 0080h	; in this port, bit conflict with PCMCIA !!!
		out	dx, ax
		jmp	short LOFF_END
LOFF_B:
; PC-9821Np/Es
; This routine works on Np/Es, but ew don't cofirm it
		mov	dx, MOTHER_PORTBASE	; GlueIndexReg
		mov	al,083h		; mistake index
		out	dx,al
		mov	dx, MOTHER_PORTBASE+1	; GlueDataReg
		in	al,dx
		mov	al,01h		; as same as 8f0/2h(52h)<-80h
		out	dx,al
LOFF_END:
		pop	edx
		pop	eax
		ret
SetLCD_OFF	endp

PPROC	SetVCLK near

		pushad
		cld

		mov	al, byte ptr [SessionMode]
		mov	ah, byte ptr [PARA_MODE]

		mov	esi, offset32 VCLK_STANDBY
		test	ah, 080h		; ? standby mode
		jnz	short @f
		and	ah, 7Fh

		mov	esi, offset32 VCLK480_256Cb
		cmp	ax, 015fh		; ? 640x480 256 / Cbetc
		je	short @f
		je	short @f
		mov	esi, offset32 VCLK480_256WIDE
		cmp	ax, 035fh		; ? 640x480 256 / Cbetc overlay wide
		je	short @f
		mov	esi, offset32 VCLK600_256
		cmp	al, vSM_600_256		; ? 800x600 256
		je	short @f
		mov	esi, offset32 VCLK480_64K
		cmp	al, vSM_480_64K		; ? 640x480 64K
		je	short @f
		mov	esi, offset32 VCLK600_64K
		cmp	al, vSM_600_64K		; ? 800x600 64K
		je	short @f

		mov	esi, offset32 VCLK480_256
		cmp	al, vSM_480_256		; ? 640x480 256
		je	short @f
		mov	esi, offset32 VCLK768_256
		cmp	al, vSM_768_256		; ? 1024x768 256
		je	short @f
		mov	esi, offset32 VCLK480_16M
@@:
		mov	edx, [Sequencer]
		mov	ecx, 8
		rep	outsw
ifndef FORCE_BANK
		test	[PRODUCT_ID],fID_GD5430
		jz	short @f
		mov	al, 05h			;SR index = 5
		out	dx, al			;avoid GD5430 BUG
@@:
endif ; FORCE_BANK
		popad
		ret
SetVCLK		endp

SetCRTC		proc	near
	pushad
	cld

	mov	al, byte ptr [SessionMode]
	mov	ah, byte ptr [PARA_MODE]

	and	ah, 7Fh			; clear standby bit

	mov	esi, offset32 CRTC480_256WIDE
	cmp	ax, 035fh		; ? 640x480 256 / Cbetc overlay wide
	je	short @f
	mov	esi, offset32 CRTC480_256
	cmp	al, vSM_480_256		; ? 640x480 256
	je	short @f
	mov	esi, offset32 CRTC768_256
	cmp	al, vSM_768_256		; ? 1024x768 256
	je	short @f
	mov	esi, offset32 CRTC600_256
	cmp	al, vSM_600_256		; ? 800x600 256
	je	short @f
	mov	esi, offset32 CRTC480_64K
	cmp	al, vSM_480_64K		; ? 640x480 64K
	je	short @f
	mov	esi, offset32 CRTC600_64K
	cmp	al, vSM_600_64K		; ? 800x600 64K
	je	short @f
	mov	esi, offset32 CRTC480_16M
@@:
	mov	edx, [CRT_3D4]
	mov	al, 11h
	out	dx, al
	inc	dx
	in	al, dx
	and	al, 01111111b	;CR11 bit7 clear
	out	dx, al		;write enable CR0-CR7
	dec	dx

	mov	ecx, 16
	rep	outsw

	popad
	ret
SetCRTC		endp

; *****************************************************************************	;
; ChkCfVO									;
; input:	[PRODUCT_ID]							;
;		[SessionMode]							;
; output:	CF=0	Cf Video Overlay is available				;
;		CF=1	                  is not available			;
; *****************************************************************************	;
ChkCfVO	proc	near
	push	ax

	mov	al,ds:[PRODUCT_ID]
	cmp	al,vID_Cb
	je	short ChkRES
	cmp	al,vID_Cf_Cx
	je	short ChkRES
	jmp	short VO_CANT
ChkRES:
	mov	edx,[GlueIndexReg]
	mov	al, 20h
	out	dx, al
	inc	dx
	mov	al, 0f0h		;reset defalut
	out	dx, al

	mov	bl,byte ptr ds:[SessionMode]
	cmp	bl,5fh			;? 640x480 256
	je	short VO_OK
VO_CANT:
	stc				;Overlay is not available
	jmp	short VO_END
VO_OK:
	clc				;Overlay is available
VO_END:
	pop	ax
	ret
ChkCfVO	endp

VxD_CODE_ENDS

VxD_DATA_SEG

public	GlueIndexReg
public	GlueDataReg

public	GD5428_0094
public	GD5428_0102
public	GD5428_46E8

public	attribute_controller
public	attribute_controllerR
public	Miscellaneous
public	Vssm_SleepAdress
public	Sequencer
public	SequencerData
public	DAC_Pel_Mask
public	DAC_Read_Index
public	DAC_Write_Index
public	DAC_Write_Data
public	Graphics2_Pos
public	Graphics1_Pos
public	Graphics_Controller
public	CRT_3D4
public	CRT_3D5
public	Feature_Control_3DA

TabPorts_Int	label dword

GlueIndexReg		dd	MOTHER_PORTBASE
GlueDataReg		dd	MOTHER_PORTBASE+1

GD5428_0094		dd	0904h
GD5428_0102		dd	0FF82h
GD5428_46E8		dd	04E68h		;not use

attribute_controller	dd	0CA0h		;Attribute controller index (write)
attribute_controllerR	dd	0CA1h		;Attribute controller index (read)
Miscellaneous		dd	0CA2h		;Miscellaneous output register
Vssm_SleepAdress	dd	0CA3h		;3C3 Sleep Adress Register
Sequencer		dd	0CA4h		;Sequencer index register
SequencerData		dd	0CA5h		;Sequencer data register
DAC_Pel_Mask		dd	0CA6h		;DAC Pel Mask Register
DAC_Read_Index		dd	0CA7h		;DAC Read Index Register
DAC_Write_Index		dd	0CA8h		;DAC Index Register
DAC_Write_Data		dd	0CA9h		;DAC Data Register
Graphics2_Pos		dd	0CAAH		;Graphics 2 position register
Graphics1_Pos		dd	0CACh		;Graphics 1 position register
Graphics_Controller	dd	0CAEh		;Graphics controller index
CRT_3D4			dd	0DA4h		;Monochrome CRT index addr
CRT_3D5			dd	0DA5h		;Monochrome CRT index addr
Feature_Control_3DA	dd	0DAAh		;Feature control for 3Dx

NumPorts_Int	equ	($-TabPorts_Int)/4

VxD_DATA_ENDS

VxD_IDATA_SEG

TabPorts_Ext	label dword

_GlueIndexReg		dd	OPTION_PORTBASE
_GlueDataReg		dd	OPTION_PORTBASE+1

_GD5428_0094		dd	0904h
_GD5428_0102		dd	0902h
_GD5428_46E8		dd	04E68h		;not use

_attribute_controller	dd	0C50h		;Attribute controller index (write)
_attribute_controllerR	dd	0C51h		;Attribute controller index (read)
_Miscellaneous		dd	0C52h		;Miscellaneous output register
_Vssm_SleepAdress	dd	0C53h		;3C3 Sleep Adress Register
_Sequencer		dd	0C54h		;Sequencer index register
_SequencerData		dd	0C55h		;Sequencer data register
_DAC_Pel_Mask		dd	0C56h		;DAC Pel Mask Register
_DAC_Read_Index		dd	0C57h		;DAC Read Index Register
_DAC_Write_Index	dd	0C58h		;DAC Index Register
_DAC_Write_Data		dd	0C59h		;DAC Data Register
_Graphics2_Pos		dd	0C5AH		;Graphics 2 position register
_Graphics1_Pos		dd	0C5Ch		;Graphics 1 position register
_Graphics_Controller	dd	0C5Eh		;Graphics controller index
_CRT_3D4		dd	0D54h		;Monochrome CRT index addr
_CRT_3D5		dd	0D55h		;Monochrome CRT index addr
_Feature_Control_3DA	dd	0D5Ah		;Feature control for 3Dx

NumPorts_Ext	equ	($-TabPorts_Ext)/4
.errnz	NumPorts_Int - NumPorts_Ext

VxD_IDATA_ENDS

VxD_ICODE_SEG

;******************************************************************************
;ABE_CheckCirrus
;
;DESCRIPTION:
;
;ENTRY: None
;
;EXIT:	ECX = Physical page number of video window
;	[PRODUCT_ID]
;	[VACL_Flags]
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc ABE_CheckCirrus, PUBLIC

	xor	ecx, ecx

	mov	dx, OPTION_PORTBASE		; Glue logic Index
	mov	al, vGL_PRODUCT_ID
	out	dx, al
	mov	dx, OPTION_PORTBASE+1		; Glue logic Data
	mov	al, DUMMY_ID			; This is neccessary on B3BOARD
	out	dx, al				; dummy write
	in	al, dx
	cmp	al, vID_B3			; Q: Is External AC board?
	jne	@f				;   N:
	or	ecx, vFlg_Acc_External		;   Y:
	mov	[ABE_External_ID], al
@@:
	mov	dx, MOTHER_PORTBASE		; Glue logic Index
	mov	al, vGL_BOARD_CMD
	out	dx, al
	IO_Delay
	mov	dx, MOTHER_PORTBASE+1		; Glue logic Data
	in	al, dx
	cmp	al, DUMMY_ID			; Q: Is Internal AC board?
	je	@f				;   N:
	or	ecx, vFlg_Acc_Internal		;   Y:
	mov	dx, MOTHER_PORTBASE		; Glue logic Index
	mov	al, vGL_PRODUCT_ID
	out	dx, al
	IO_Delay
	mov	dx, MOTHER_PORTBASE+1		; Glue logic Data
	in	al, dx
	mov	[ABE_Internal_ID], al
	cmp	al, 70h				;Nf?
	jnz	@f
	mov	[ABE_Internal_ID], 55h		;Nf is as same as Np
@@:
	mov	[VACL_Flags], ecx

ifdef VACLB
	test	ecx, vFlg_Acc_External
	jnz	short ABE_CC_ChkExt
else ;VACLB
	test	ecx, vFlg_Acc_Internal
	jnz	ABE_CC_ChkInt
endif ;VACLB

	Trace_Out "Not found any CIRRUS chip!"

	stc
	ret

ABE_CC_ChkExt:
	mov	esi, OFFSET32 TabPorts_Ext
	mov	edi, OFFSET32 TabPorts_Int
	mov	ecx, NumPorts_Int
	cld
	rep movsd

	mov	edx, OPTION_PORTBASE	; GlueIndexReg
	mov	al, vGL_STATUS_CMD
	out	dx, al			; set glue logic index(4):PKG STATUS
	mov	edx, OPTION_PORTBASE+1	; GlueDataReg
	in	al, dx			; get PKG STATUS
					;  mapping index (DIP-SW 1-3)

	mov	ah, 10h			; 0B0000h
	mov	ecx, 00B0h
	test	al, 100b		; 1xxb
	jnz	short @f
	mov	ah, 0a0h		; F00000h
	mov	ecx, 0F00h
	test	al, 011b		; 000b
	jz	short @f
	mov	ah, 080h		; F20000h
	mov	ecx, 0F20h
	test	al, 010b		; 001b
	jz	short @f
	mov	ah, 0E0h		; F40000h
	mov	ecx, 0F40h
	test	al, 001b		; 010b
	jz	short @f
	mov	ah, 0C0h		; F60000h
	mov	ecx, 0F60h
@@:
	mov	edx, OPTION_PORTBASE	; GlueIndexReg
	mov	al, vGL_VGA_REMAP
	out	dx, al			; set glue logic index(1):VGA Remapping
	mov	edx, OPTION_PORTBASE+1	; GlueDataReg
	mov	al, ah
	out	dx, al			; set VGA Remapping

	mov	al, [ABE_External_ID]
	jmp	ABE_CC_Chk30

ABE_CC_ChkInt:
;
; Set VRAM-address at least 16MB(00000h-F00000h) area.
; This is not necessary on B3BOARD.
;
	mov	edx, MOTHER_PORTBASE	; GlueIndexReg
	mov	al, vGL_BASE_ADDR	; Index 02h
	out	dx, al
	mov	edx, MOTHER_PORTBASE+1	; Data Reg

	xor	al, al
ifndef FORCE_BANK
	test	[ABE_Internal_ID], fID_GD5430
	jz	@f
	mov	al, 0Fh
endif ; FORCE_BANK
@@:	out	dx, al
;
; Get VRAM-address from Glue(1):VGA remap addr register.
;
	mov	edx, MOTHER_PORTBASE	; GlueIndexReg
	mov	al, vGL_VGA_REMAP	; get glue logic index(1):VGA remap address
	out	dx, al
	mov	edx, MOTHER_PORTBASE+1	; Data Reg
	in	al, dx

	mov	ecx, 00B0h
	cmp	al, 010h		; VGA remap value -> VRAM address
	je	short @f
	mov	ecx, 0F00h		; F0
	cmp	al, 0A0h
	je	short @f
	mov	ecx, 0F20h		; F2
	cmp	al, 080h
	je	short @f
	mov	ecx, 0F40h		; F4
	cmp	al, 0E0h
	je	short @f
	mov	ecx, 0F60h		; F6
@@:
	mov	al, [ABE_Internal_ID]

ABE_CC_Chk30:
	mov	[PRODUCT_ID], al
ifndef FORCE_BANK
	test	al, fID_GD5430
	jz	short @f
	mov	[GD5430_0], 0A107h	; linear Addr
	mov	[GD5430_1], 0A107h	; linear Addr
	mov	[GD5430_2], 0A507h	; linear Addr
	mov	[GD5430_3], 0A307h	; linear Addr
	mov	[GD5430_4], 0A307h	; linear Addr
	mov	[GD5430_5], 0A107h	; linear Addr
	mov	ecx, 0F000h
@@:
endif ; FORCE_BANK
	clc
	ret

EndProc ABE_CheckCirrus


VxD_ICODE_ENDS


	END
