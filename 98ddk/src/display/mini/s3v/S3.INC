;------------------------------------------------------------------------------
; S3.INC
; Copyright (c) 1996 S3 Inc.
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
; E Q U A T E S
;------------------------------------------------------------------------------
;
;###

M5  equ 1
M5DEBUG equ 1


TRUE		equ	01h	; BOOLEAN TRUE	flag
FALSE		equ	00h	; BOOLEAN FALSE flag
NULL		equ   0000h	; NULL linked list ptr/handle equate

;
;Equates for cursor types cursor format
;
SOFTWARECURSOR	    equ     0
HARDWARECURSOR	    equ     1
MONO_CURSOR	    equ     0101h

s3kernel=0
;
;------------------------------------------------------------------------------
; Virge/VirgeVX Software patch information:
;------------------------------------------------------------------------------
; Virge Patch Define	Hardware Errata Problem Description
; --------------------	-------------------------------------------------------
; VIRGE_REVAB_PATCH Command following rectangle fills or line draws.
; VIRGE_PATCH12     8 QWORD image xfer or screen to screen BitBlt.
; VIRGE_PATCH13     Consecutive rectangle or line commands.
; VIRGE_PATCH14     Consecutive lines in 24Bpp color depth.
; VIRGE_PATCH15     Wrong data in last subspan of BitBlt.
; VIRGE_PATCH16     Dots in rectangles and lines.
; VIRGE_PATCH17     Transparent text corruption at 8/16/24bpp color depths
;------------------------------------------------------------------------------
;
VIRGE_REVAB_PATCH   equ     1	    ; Set engine state after rect/line draw
VIRGE_PATCH12	    equ     1	    ; Patch 12 enabled
VIRGE_PATCH14	    equ     1	    ; Patch 14 enabled
VIRGE_PATCH15	    equ     1	    ; Patch 15 enabled
VIRGE_PATCH16	    equ     1	    ; Patch 16 enabled
NEW_VDD_LOCATION    equ     1
FIX_VX_16x12x75HZ   equ     1

pci_fix=1
X_CNT	equ 12

;
; Chip Id and Family equates
;
S3764	equ 0e0h	    ; Major chip id 764 (Trio)
D_S3764 equ 1188h
D_S3VIRGEFamily equ 008ah	; Virge Family Id.
D_S3VIRGE   equ 3156h	    ; Virge
D_S3VIRGEVX equ 3d88h	    ; Virge VX
D_S3VIRGEDXGX	equ 018ah	; Virge DX/GX
D_S3VIRGEGX2	equ	108ah		; Virge GX2
D_S3M3		equ	008ch		; M3
D_S3M5		equ	018ch		; M5
D_S3M5PM	equ	038ch		; M5+ w/ Macrovision
D_S3M5P		equ	028ch		; M5+ w/o Macrovision
D_S3VIRGEGX2PM	equ	118ah		; ViRGE GX2+ w/ Macrovision
D_S3VIRGEGX2P	equ	128ah		; ViRGE GX2+ w/o Macrovision

;
;MMIO address for crtc and sequencer registers
;
MMIO_CRTC_INDEX 	equ	83d4h
MMIO_CRTC_DATA		equ	83d5h
MMIO_SEQ_INDEX		equ	83c4h
MMIO_SEQ_DATA		equ	83c5h

;
;Compatibility equates
;
VIRGE_COMPATIBLE    equ 00000001b
VIRGEDXGX_COMPATIBLE	equ 00000010b

;Patch flag
SPEEDY_24BPP_PATCH  equ     00000001b	;M5/GX2 24Bpp mode Speedy patch

;
;Peripheral Feature equates
;
UMA_SUPPORTED	equ 00000001b	; UMA is a feature.

;
;Equates for bFamilyFlags
;
MOBILE_FAMILY	  equ  00000001b ; 01h
UMA_FAMILY	  equ  00000010b ; 02h
;TRIO64V_FAMILY   equ  00000100b ; 04h
;TRIOV2_FAMILY	  equ  00001000b ; 08h
VIRGE_FAMILY	  equ  00010000b ; 10h
VIRGEDXGX_FAMILY  equ  00100000b ; 20h

;
;DAC Types:
;
DAC_ATT     equ 1	; ATT or clone
DAC_BT485   equ 2	; Brooktree 485
DAC_RGB525  equ 3	; IBM RGB525/514
DAC_TI025   equ 4	; Texas Instruments 3020/3025

;
;Bus Types:
;
BUS_ISA     equ 1	; ISA bus
BUS_LOCAL   equ 2	; local bus
BUS_PCI     equ 3	; PCI local bus
;
;Following are equates for the various VGA ports:
;
INPUT_STATUS_1_REG  equ 3dah
ATTRIBUTE_REG	    equ 3c0h
ATTRIBUTE_READ_REG  equ 3c1h
MISC_OUT_REG	    equ 3c2h
MISC_OUT_READ_REG   equ 3cch
FEATURE_REG	equ 3dah
FEATURE_READ_REG    equ 3cah
CRTC_REG	equ 3d4h
SEQ_REG 	equ 3c4h
GCR_REG 	equ 3ceh
PEL_READ_REG	    equ 3c7h
PEL_WRITE_REG	    equ 3c8h
PEL_DATA_REG	    equ 3c9h

;
CRT_VERT_RETRACE_REG	equ 011h
CRTC_LOCK_BIT	    equ 080h
;
HORZ_TOTAL_1	    equ 001h
HORZ_TOTAL_2	    equ 05dh
HORZ_2_BIT	equ 02	    ;bit 1 of CR5d
;
VERT_TOTAL_1	    equ 012h
VERT_TOTAL_2	    equ 007h
VERT_2_BITS	equ 042h	;bit [6,1] of CR7
VERT_TOTAL_3	    equ 05eh
VERT_3_BIT	equ 002h	;bit 1 of CR5e

;
;Equates for bStreamProcessorFlags
;
SP_FAMILY	equ 00000001b
SP_ENABLED	equ 00000010b
SP_IN_VGA_MODE	    equ 00000100b
SP_IN_ENHANCE_MODE  equ 00001100b
SP_DISABLE_OVERLAY  equ 00010000b
SP_HIDE             equ 00100000b

;
;Equates for video type passed to display driver from 32-bit direct draw layer
;
DD_HWCODEC	equ 100000h
DD_LIVEVIDEO	    equ 080000h
I_HWCODEC	equ 0h
I_SWCODEC	equ 1h
I_LIVEVIDEO	equ 2h

;
;Equates for bFamilyFlags
;
MOBILE_FAMILY	    equ  00000001b

;
;Stream Processor Registers
;
SP_BLENDCTRL	equ	081a0h
PS_ADDR     equ     081c0h
SS_ADDR     equ     081d0h
SS_STRIDE   equ     081d8h
SS_CONTROL  equ     08190h
PS_XY	    equ     081f0h
PS_WH	    equ     081f4h
SS_XY	    equ     081f8h
SS_WH	    equ     081fch
SP_FIFO     equ     081ech

;
;Equates for SS_CONTROL register
;
SSC_INPUT_DATA_POS	equ	24
SSC_INPUT_DATA_MASK	equ	7
SSC_TWO_BYTE_FORMAT	equ	5
SSC_THREE_BYTE_FORMAT	equ	6
SSC_FOUR_BYTE_FORMAT	equ	7


;
;Equates for Blend control register
;
COMPOSE_MASK	equ	0f0ffffffh
COMPOSE_SHIFT	equ	24
P_ON_S	    equ     1

;
;Equates for TV output register
;
NTSC_PAL_REG	equ	03dh
TV_OUTPUT   equ     020h
TV_ENABLED  equ     001h

;
;Address of extended sequencer registers
;
ARCH_CONFIG_REG 	equ	30h
SPSRC_IGA2		equ	010b		;SP source is on IGA2
TV_IGA2 		equ	100b		;TV DAC source is IGA2

CLUT_CONTROL_REG	equ	47h
CLUT1_ENABLED		equ	1
CLUT2_ENABLED		equ	2

;
;Address of extended sequencer registers
;
FP_DISPLAY_REG	    equ 31h
FP_ENABLE	equ 10h     ;1 : enable ; 0 : disable
FP_CURSOR_IGA2	    equ 80h
;
FP_FRC_CONTROL_REG  equ 39h	;Panel type & FRC_GRAY
FP_TYPE 	equ 011b
FP_STN		equ 10b     ;stn panel
;
ICON_ADDR_REG	    equ 4eh
TWO_MBYTES	equ 200000h
EIGHT_KBYTES	    equ 2000h
;
STN_DATA_ADDR_REG   equ 4fh
STN_SIZE_LO_REG     equ 50h
STN_SIZE_HI_REG     equ 51h
;
FP_HORZ_SIZE	    equ 61h
FP_HORZ_OVERFLOW    equ 66h
FP_HORZ_EXT_BIT     equ 02h
;
FP_VERT_SIZE	    equ 69h
FP_VERT_OVERFLOW    equ 6eh
FP_VERT_EXT_MASK    equ 070h
FP_VERT_EXT_BIT_POS equ 04h


;
;Equate for extended Seq reg
;
EXT_SEQ_A_REG	equ	0ah
ESA_DS3D	equ	40h
EXT_SEQ_9_REG	equ	09h
ES9_EPD 	equ	02h

;
;Equates for display configuration interface register.
;
DCI_REG 	equ 50h
DCI_ENABLED	equ 01000000b	;bit 6 = 1:DCS interface enable

;
;Equates for display configurtion and DuoView registers
;
DISPLAY_REG	equ 6bh
CRT_ONLY	equ 0001b
FP_ONLY 	equ 0010b
TV_ONLY 	equ 0100b
HK_ENABLE_CB	equ 1000b		;08h, enable system call back function
HK_DUAL_IGA	equ 00010000b		;10h
NEW_DISPLAY	equ 01000000b		;40h
DUOVIEW 	equ 10000000b
;
FP_CRT_SIM	equ (FP_ONLY or CRT_ONLY)
FP_TV_SIM	equ (FP_ONLY or TV_ONLY)
CRT_TV_SIM	equ (CRT_ONLY or TV_ONLY)
MODE_MASK	equ 1111b
DISPLAY_MASK	equ (DUOVIEW or TV_ONLY or FP_ONLY or CRT_ONLY)


;  Expansion and centering registers

FP_HORZ_EXP_1	    equ 054h
H_EXP		equ 0ch
H_EXP_CENTER_MASK   equ 0e3h
H_CENTERING	equ 010h

FP_HORZ_EXP_2	    equ 055h
ALL_H_EXP	equ 01ch
ALL_H_EXP_MASK	    equ 0e3h

FP_VERT_EXP_1	    equ 056h
V_EXP		equ 0ch
V_EXP_CENTER_MASK   equ 0e3h
V_CENTERING	equ 010h

FP_VERT_EXP_2	    equ 057h
ALL_V_EXP	equ 07ch
ALL_V_EXP_MASK	    equ 083h

;
;Equates for escape functions
;
S3_MOBILE_ESCAPE_FUNCTION   equ 03000h
S3_MOBILE_GETPANELINFO	    equ 0
S3_MOBILE_GETDISPLAYCONTROL equ 1
S3_MOBILE_SETDISPLAYCONTROL equ 2
S3_MOBILE_GETHORIZONTALSTATE	equ 3
S3_MOBILE_SETHORIZONTALSTATE	equ 4
S3_MOBILE_GETVERTICALSTATE  equ 5
S3_MOBILE_SETVERTICALSTATE  equ 6
S3_MOBILE_GETCONNECTIONSTATUS	equ 7
S3_MOBILE_GETCRTPANNINGRES  equ 9
S3_MOBILE_SETCRTPANNINGRES  equ 10
S3_MOBILE_GETTIMINGSTATE    equ 11
S3_MOBILE_SETTIMINGSTATE    equ 12
S3_MOBILE_GETIMAGESTATE     equ 13
S3_MOBILE_SETIMAGESTATE     equ 14
S3_MOBILE_GETPRIMARYDEVICE  equ 15
S3_MOBILE_SETPRIMARYDEVICE  equ 16
S3_MOBILE_GETTVFFILTERSTATUS	equ 17
S3_MOBILE_SETTVFFILTERSTATUS	equ 18
S3_MOBILE_GETTVUNDERSCANSTATUS	equ 19
S3_MOBILE_SETTVUNDERSCANSTATUS	equ 20
S3_MOBILE_GETTVSIGNALTYPE   equ 21
S3_MOBILE_SETTVSIGNALTYPE   equ 22
S3_MOBILE_GETTVPOSITION     equ 23
S3_MOBILE_SETTVPOSITION     equ 24
S3_MOBILE_GETTVCENTERINGOPTION	equ 25
S3_MOBILE_SETTVCENTERINGOPTION	equ 26
S3_MOBILE_GETRESOLUTIONTABLE	equ 27
S3_MOBILE_GETCHIPID	equ 28
S3_MOBILE_GETBIOSVERSION    equ 29
S3_MOBILE_GETVIDEOOUTDEVICES	equ 30
S3_MOBILE_SETVIDEOOUTDEVICES	equ 31

;
;Equates for Get/Set Display control functions
;
S3_MOBILE_NO_CONNECTION     equ 0
S3_MOBILE_CRT		equ 1
S3_MOBILE_LCD		equ 2
S3_MOBILE_TV		equ 4
S3_MOBILE_DUOVIEW	equ 8
S3_MOBILE_LCD_CRT	equ (S3_MOBILE_CRT or S3_MOBILE_LCD)
S3_MOBILE_LCD_TV	equ (S3_MOBILE_TV or S3_MOBILE_LCD)

;
;Equates for Get/Set centering or expansion
;
S3_MOBILE_NO_CENTER_EXPAND  equ 0
S3_MOBILE_CENTERING	equ 1
S3_MOBILE_EXPANSION	equ 2

;
;TV output
;
S3_MOBILE_TV_UNKNOWN	    equ 0
S3_MOBILE_TV_NTSC	equ 1
S3_MOBILE_TV_PAL	equ 2
S3_MOBILE_TV_NTSC_JPN	    equ 3

S3_MOBILE_FFILTER_OFF	    equ 0
S3_MOBILE_FFILTER_ON	    equ 1

S3_MOBILE_FFILTER_STATE1    equ 1
S3_MOBILE_FFILTER_STATE2    equ 2
S3_MOBILE_FFILTER_STATE3    equ 3
S3_MOBILE_FFILTER_STATE4    equ 4

S3_MOBILE_UNDERSCAN_OFF     equ 0
S3_MOBILE_UNDERSCAN_ON	    equ 1

S3_MOBILE_TV_COMPOSITE	    equ 0
S3_MOBILE_TV_SVIDEO	equ 1
;
;Connection status
;
S3_MOBILE_PANEL_CONNECTED   equ 1
S3_MOBILE_CRT_CONNECTED     equ 2
S3_MOBILE_TV_CONNECTED	    equ 4

S3_MOBILE_PANNING_CENTERING_OFF equ 0
S3_MOBILE_PANNING_CENTERING_ON	equ 1

S3_MOBILE_VIDEO_OUT_IGA1    equ 0
S3_MOBILE_VIDEO_OUT_IGA2    equ 1

S3_MOBILE_CHIPID_UNKNOWN    equ 0
S3_MOBILE_CHIPID_M1	    equ 1
S3_MOBILE_CHIPID_M3	    equ 2
S3_MOBILE_CHIPID_M5	    equ 3
S3_MOBILE_CHIPID_GX2	    equ 4
S3_MOBILE_CHIPID_M5P	    equ 5
S3_MOBILE_CHIPID_M5PM	    equ 6
S3_MOBILE_CHIPID_GX2P	    equ 7
S3_MOBILE_CHIPID_GX2PM	    equ 8

;
;Equates for output types
;
RGB_OUTPUT	    equ 0
NTSC_OUTPUT	    equ 080h
PAL_OUTPUT	    equ 0c0h

;
;Equates for video BIOS extension functions
;
DISPLAY_CONTROL_FUNCTION    equ 6f02h
EXTERNAL_MONITOR_DETECTION  equ 6f06h

VESA_EXT_SERVICE	equ 4f14h
SET_ACTIVE_DISPLAY		equ	03

;
;Panel type
;
S3_MOBILE_STN_PANEL	equ 0
S3_MOBILE_TFT_PANEL	equ 1

;
;Vertical and horziontal end registers
;
VERT_1_REG	    equ 12h
VERT_2_REG	    equ 07h
VERT_3_REG	    equ 5eh
VERT_MASK	    equ 0fdh
;
HORZ_1_REG	    equ 01h
HORZ_2_REG	    equ 5dh
HORZ_MASK	    equ 0feh

;
;Mclck control register
;
MCLK_CONTROL_REG    equ 027h
MCLK		equ 0
DCLK		equ 1

;
;Equates for DLCK low register
;
DCLK_LO_REG	equ 012h
D_PLL_N_MASK	    equ 03fh
D_PLL_R_POS	equ 6

;
;Equates for DLCK high register
;
DCLK_HI_REG	equ 013h

;
;Equates for MLCK low register
;
MCLK_LO_REG	equ 010h
M_PLL_N_MASK	    equ 01fh
M_PLL_R_MASK	    equ 60h
M_PLL_R_POS	equ 5

;
;Equates for MLCK high register
;
MCLK_HI_REG	equ 011h
M_PLL_M_MASK	    equ 7fh

;
;Equates for panel type register
;
PANEL_TYPE_REG	    equ 30h
TFT_PANEL	equ 1
DD_STN_PANEL	    equ 0

;
;Equates for FIFO control registers in M65 rev. E
;
FIFO_IO_REG_LO	    equ 3eh
FIFO_IO_REG_HI	    equ 3fh
FIFO_CONTROL_BIT    equ 80h
FIFO_CONTROL_MASK   equ 7fh


;
;Equates for dwMobileCaps
;

MOBILE_FAMILY		equ  00000001h
PANNING_ENABLED 	equ  00000002h
DUAL_IMAGE		equ  00000004h	  ;duoview with diff images
DUOVIEW_SAMEIMAGE	equ  00000008h	  ;same-image, different-timing
EXPANSION_ENABLED	equ  00000010h
CENTERING_ENABLED	equ  00000020h
PANNING_IGA1		equ  00000040h
SP_IGA1 		equ  00000080h	  ;SP on IGA1
DSTN_ON 		equ  00000100h	  ;STN pannel
SP_ON			equ  00000200h	  ;Streams processor is on
UNDERSCAN_ENABLED	equ  00000400h	  ;Underscanning is enabled
TV_PAL			equ  00000800h	  ;TV output is PAL
USER_SP_IGA1		equ  00001000h
PANNING_CENTER		equ  00002000h
VIDEO_ON_TV		equ  00004000h
PANNING_IGA1_IGA2	equ  00008000h
;
FIX_XPARENT_CURSOR	equ  00010000h	  
LCD_IGA1_CRT_IGA2	equ  00020000h
FIX_GE_POWER_SAVING	equ  00040000h
REJECT_MODESWITCH_IN_SP equ  00080000h
M65_NEW_FIFO_CONTROL	equ  00100000h	  ;M1, rev. E : sr3f[7] = 1 :
					  ;fifo controlled via I/O
					  ;sr3f[7] = 0 : fifo controlled
					  ;via 81ech

PANNING_ON_CRT		equ  00200000h
DUOVIEW_DIFF_REFRESH	equ  00400000h
FORCED_VIDEO_CENTERING	equ  00800000h	  ;User wants video in centering
DEFAULT_TO_SIM		equ  01000000h	  ;set sim when panel size =
					  ;mode instead of expansion mode
IN_DISPLAY_SWITCH	equ  02000000h
DUOVIEW_IN_8BPP 	equ  04000000h
IN_FLIPPING             EQU  08000000h
HIDE_OVERLAY            EQU  10000000h

;
;Information about graphics and stream processor registers
;

;
GE_SIZE 	equ 026h
GE_BASE 	equ 8100h
SP_SIZE 	equ 020h
SP_BASE 	equ 8180h
GE_BASEV1	equ 0A4D4h
GE_BASEV2	equ 0A504h
GE_BASEV3	equ 0A568h
GE_SIZEV1	equ 11	    ;Src_Base ~ Src_FG_Clr
GE_SIZEV2	equ 3	    ;RWidth_Height ~ RDest_XY
GE_SIZEV3	equ 6	    ;PRdx ~ PyCnt

;
;Mobile data area structure
;
S3_MOBILE_DATA          STRUCT
    wSize	        DW      ?	; size of this structure
    bCurrentDisplay     DB	?	; CRT_Only, LCD_Only, Sim., ...
    bAPMEvents          DB      ?       ; Display switch, Center/Expand
    dwMobileCaps        DD	?
    GERegs	        DW      GE_SIZE dup (0)
    SPRegs	        DD      SP_SIZE dup (0)
    wSPStyle	        DW      ?
    b3DCaps             DB	?
    Reserved            DB      ?	; make it word aligned
S3_MOBILE_DATA          ENDS

;
;Equates for b3DCaps
;
Virge_3D    equ 00000001b   ;Virge 3D engine
VirgeGX_3D  equ 00000010b   ;Virge/GX 3D engine

;
;Equates for bAPMHotkeyEvent
;
HK_DISPLAY_SWITCH   equ 00000001b   ;display switch
HK_CENTER_EXPAND    equ 00000010b
STD_IN_GE_FUNCTION  equ 00000100b
APM_RESUME_IN_DOS   equ 00001000b
STD_RESTORE_GE_REGS equ 00010000b
STD_RESTORE_SP_REGS equ 00100000b
APM_RESUME_SUSPEND  equ 01000000b
HK_DSWITCH_IN_DOS   equ 10000000b

;
;Equates for wSPStyle
;
SP_Trio     equ 00000001b
SP_Virge    equ 00000010b
SP_VX	    equ 00000100b
SP_VirgeGX  equ 00001000b
SP_TrioV2   equ 00010000b
SP_M5	    equ 00100000b
SP_Yosemite equ 01000000b

;
;Equates for refresh rate on LCD
;
LCD_REFRESH_RATE    equ 60


;
;Equates for escape functions for Color Adjust utility
;
S3_COLOR_ADJUST_FUNCTION    equ 02880h
S3_COLOR_ADJUST_STATUS	    equ 1
S3_COLOR_ADJUST_READ	    equ 2
S3_COLOR_ADJUST_WRITE	    equ 3


;
;Equates for MPEG and live video call back functions
;
MPEG_EXIT	equ 4024h
MPEG_INIT	equ 4025h
NOTIFY_MODE_CHANGE  equ 4027h
VIDEO_FREEZE	    equ 4028h
VIDEO_UNFREEZE	    equ 4029h

;
; Binary raster ops (for Output)
;

R2_BLACK    =  1
R2_NOTMERGEPEN	=  2
R2_MASKNOTPEN	=  3
R2_NOTCOPYPEN	=  4
R2_MASKPENNOT	=  5
R2_NOT	    =  6
R2_XORPEN   =  7
R2_NOTMASKPEN	=  8
R2_MASKPEN  =  9
R2_NOTXORPEN	= 10
R2_NOP	    = 11
R2_MERGENOTPEN	= 12
R2_COPYPEN  = 13
R2_MERGEPENNOT	= 14
R2_MERGEPEN = 15
R2_WHITE    = 16
;

;
;color pattern registers offset
;
Pat0	    equ 0100h
Pat1	    equ 0104h
Pat2	    equ 0108h
Pat3	    equ 010Ch
Pat4	    equ 0110h
Pat5	    equ 0114h
Pat6	    equ 0118h
Pat7	    equ 011Ch
Pat8	    equ 0120h
Pat9	    equ 0124h
Pat10	    equ 0128h
Pat11	    equ 012Ch
Pat12	    equ 0130h
Pat13	    equ 0134h
Pat14	    equ 0138h
Pat15	    equ 013Ch
Pat16	    equ 0140h
Pat17	    equ 0144h
Pat18	    equ 0148h
Pat19	    equ 014Ch
Pat20	    equ 0150h
Pat21	    equ 0154h
Pat22	    equ 0158h
Pat23	    equ 015Ch
Pat24	    equ 0160h
Pat25	    equ 0164h
Pat26	    equ 0168h
Pat27	    equ 016Ch
Pat28	    equ 0170h
Pat29	    equ 0174h
Pat30	    equ 0178h
Pat31	    equ 017Ch
Pat32	    equ 0180h
Pat33	    equ 0184h
Pat34	    equ 0188h
Pat35	    equ 018Ch
Pat36	    equ 0190h
Pat37	    equ 0194h
Pat38	    equ 0198h
Pat39	    equ 019Ch
Pat40	    equ 01A0h
Pat41	    equ 01A4h
Pat42	    equ 01A8h
Pat43	    equ 01ACh
Pat44	    equ 01B0h
Pat45	    equ 01b4h
Pat46	    equ 01b8h
Pat47	    equ 01bch

ON	equ 1
OFF	equ 0
AUTO_OFF    equ 078000000h  ;CMD_SET[0] = 0,CMD_SET[30:27] = 1111

;
;Image Transfer Data
;
IMAGE_XFER	equ 0000h   ; Image transfer area base offset
IMAGE_XFER_MAXBYTES equ 8000h	; Maximum image transfer size in bytes

;
;S3 MMIO registers
;
SUBSYS_CNTL	  equ	  08504h	;42e8h
ADVFUNC_PORT	      equ     0850Ch	    ;4ae8h
COLOR_ADJUST_REG      equ     0819ch
;
;The S3D ViRGE Engine
;
D2BaseOffset	    equ 0a000h
PatternArea	equ D2BaseOffset + 00000h
BitBLTArea	equ D2BaseOffset + 0400h
D2LineArea	equ D2BaseOffset + 0800h
PolygonArea	equ D2BaseOffset + 0c00h
;
;The configuration parameters offset to the base of each command
;
SRC_BASE	equ 00d4h
DEST_BASE	equ 00d8h
CLIP_L_R	equ 00dch
CLIP_T_B	equ 00e0h
DEST_SRC_STR	    equ 00e4h
MONO_PAT_0	equ 00e8h
MONO_PAT_1	equ 00ech
PAT_BG_CLR	equ 00f0h
PAT_FG_CLR	equ 00f4h
SRC_BG_CLR	equ 00f8h
SRC_FG_CLR	equ 00fch
CMD_SET 	equ 0100h
RWIDTH_HEIGHT	    equ 0104h
RSRC_XY 	equ 0108h
RDEST_XY	equ 010ch

;
;BitBLT MMIO registers
;
B_SRC_BASE	equ (BitBLTArea + SRC_BASE)
B_DEST_BASE	equ (BitBLTArea + DEST_BASE)
B_CLIP_L_R	equ (BitBLTArea + CLIP_L_R)
B_CLIP_T_B	equ (BitBLTArea + CLIP_T_B)
B_DEST_SRC_STR	    equ (BitBLTArea + DEST_SRC_STR)
B_MONO_PAT_0	    equ (BitBLTArea + MONO_PAT_0)
B_MONO_PAT_1	    equ (BitBLTArea + MONO_PAT_1)
B_PAT_FG_CLR	    equ (BitBLTArea + PAT_FG_CLR)
B_PAT_BG_CLR	    equ (BitBLTArea + PAT_BG_CLR)
B_SRC_FG_CLR	    equ (BitBLTArea + SRC_FG_CLR)
B_SRC_BG_CLR	    equ (BitBLTArea + SRC_BG_CLR)
B_RWIDTH_HEIGHT     equ (BitBLTArea + RWIDTH_HEIGHT)
B_RSRC_XY	equ (BitBLTArea + RSRC_XY)
B_RDEST_XY	equ (BitBLTArea + RDEST_XY)
B_CMD_SET	equ (BitBLTArea + CMD_SET)
COLOR_PATTERN	    equ (PatternArea + Pat0)

;
;2D Line MMIO registers
;
L_SRC_BASE	equ (D2LineArea + SRC_BASE)
L_DEST_BASE	equ (D2LineArea + DEST_BASE)
L_CLIP_L_R	equ (D2LineArea + CLIP_L_R)
L_CLIP_T_B	equ (D2LineArea + CLIP_T_B)
L_DEST_SRC_STR	    equ (D2LineArea + DEST_SRC_STR)
L_PAT_FG_CLR	    equ (D2LineArea + PAT_FG_CLR)
L_SRC_FG_CLR	    equ (D2LineArea + SRC_FG_CLR)
L_CMD_SET	equ (D2LineArea + CMD_SET)

LXEnd0_End1	equ (D2LineArea + 016ch)
Ldx	    equ (D2LineArea + 0170h)
LXStart 	equ (D2LineArea + 0174h)
LYStart 	equ (D2LineArea + 0178h)
LYCnt		equ (D2LineArea + 017ch)
;
;2D Polygon MMIO registers
;
P_SRC_BASE	equ (PolygonArea + SRC_BASE)
P_DEST_BASE	equ (PolygonArea + DEST_BASE)
P_CLIP_L_R	equ (PolygonArea + CLIP_L_R)
P_CLIP_T_B	equ (PolygonArea + CLIP_T_B)
P_DEST_SRC_STR	    equ (PolygonArea + DEST_SRC_STR)
P_MONO_PAT_0	    equ (PolygonArea + MONO_PAT_0)
P_MONO_PAT_1	    equ (PolygonArea + MONO_PAT_1)
P_PAT_BG_CLR	    equ (PolygonArea + PAT_BG_CLR)
P_PAT_FG_CLR	    equ (PolygonArea + PAT_FG_CLR)
P_SRC_FG_CLR	    equ (PolygonArea + SRC_FG_CLR)
P_CMD_SET	equ (PolygonArea + CMD_SET)

PRdX		equ (PolygonArea + 168h)
PXRStart	equ (PolygonArea + 16ch)
PLdX		equ (PolygonArea + 170h)
PXLStart	equ (PolygonArea + 174h)
PYStart 	equ (PolygonArea + 178h)
PYCnt		equ (PolygonArea + 17ch)
;
;Equates for CMD_SET
;
b2D	    equ 0
b3D	    equ 80000000h
bBitBLT 	equ 0
Rect		equ 010000000h
Line		equ 018000000h
Polygon 	equ 028000000h
Y_Positive	equ 004000000h
Y_Negative	equ 0
X_Positive	equ 002000000h
X_Negative	equ 0
bFirst_dword	    equ 0
bFirst_3bytes	    equ 000003000h
bFirst_2bytes	    equ 000002000h
bFirst_1byte	    equ 000001000h
BW_disable	equ 000010000h	;ASPEN
CPUAlign_dword	    equ 000000800h
CPUAlign_word	    equ 000000400h
CPUAlign_byte	    equ 0
bTransparent	    equ 000000200h
bPAT_Mono	equ 000000100h
bSRC_Sys	equ 000000080h
bSRC_Mono	equ 000000040h
bDraw		equ 000000020h
bFMT_24BPP	equ 000000008h
bFMT_16BPP	equ 000000004h
bFMT_8BPP	equ 0
bClip_Enable	    equ 000000002h
bAE_On		equ 000000001h
;
;Equates for CMD_SET
;
PATCOPY     equ 01E00000h   ;CMD_SET[24:17] = 0F0h
SRCCOPY     equ 01980000h   ;CMD_SET[24:17] = 0CCh
LINE_DRAW   equ b2D + Line + bDraw + bPAT_Mono
;LINE_DRAW   equ     b2D + Line + bDraw + Y_Positive + X_Positive
LINE_SOLID  equ LINE_DRAW + PATCOPY
;;RECT_FILL equ b2D + Rect + bDraw
RECT_FILL   equ b2D + Rect + Y_Positive + X_Positive + bDraw
;RECT_SOLID  equ    RECT_FILL + PATCOPY + bPAT_Mono
RECT_SOLID  equ RECT_FILL + PATCOPY + bPAT_Mono + bTransparent
BITBLT	    equ b2D + bBitBLT + Y_Positive + X_Positive + bDraw
CLRPATBLT   equ BITBLT
MONOPATBLT  equ BITBLT + bPAT_Mono
CLRSRCBLT   equ BITBLT + bSRC_Sys
MONOSRCBLT  equ BITBLT + bSRC_Sys + bSRC_Mono
;POLYGON_FILL	 equ	 b2D + Polygon + bDraw	;VIRGE
POLYGON_FILL	equ b2D + Polygon + bDraw + bTransparent    ;ASPEN
MONOPAT_POLYGON equ POLYGON_FILL + bPAT_Mono
CLRPAT_POLYGON	equ POLYGON_FILL


;
;Here are equates related to S3D FIFO space:
;

ONE_SLOT	equ 01h
TWO_SLOTS	equ 02h
THREE_SLOTS	equ 03h
FOUR_SLOTS	equ 04h
FIVE_SLOTS	equ 05h
SIX_SLOTS	equ 06h
SEVEN_SLOTS	equ 07h
EIGHT_SLOTS	equ 08h
NINE_SLOTS	equ 09h
TEN_SLOTS	equ 0ah
ELEVEN_SLOTS	    equ 0bh
TWELVE_SLOTS	    equ 0ch
THIRTEEN_SLOTS	    equ 0dh
FOURTEEN_SLOTS	    equ 0eh
FIFTEEN_SLOTS	    equ 0fh
SIXTEEN_SLOTS	    equ 10h



;
;CursorType
;
SOFTWARECURSOR	equ 0
HARDWARECURSOR	equ 1

;
; Equates and Structures for Int 31h Interface
;

Int31_Sel_Mgt	    EQU 00h
    SelMgt_Alloc_Sel	EQU 00h
    SelMgt_Free_Sel EQU 01h
    SelMgt_Seg_To_Sel	EQU 02h
    SelMgt_Get_LDT_Base EQU 03h
    SelMgt_Lock_Sel EQU 04h
    SelMgt_Unlock_Sel	EQU 05h
    SelMgt_Get_Base EQU 06h
    SelMgt_Set_Base EQU 07h
    SelMgt_Set_Limit	EQU 08h
    SelMgt_Set_Acc_Bits EQU 09h
    SelMgt_Alias_Sel	EQU 0Ah
    SelMgt_Get_Desc EQU 0Bh
    SelMgt_Set_Desc EQU 0Ch
    SelMgt_Get_Spec_Sel EQU 0Dh

Int31_DOS_Mem_Mgt   EQU 01h
    DOSMem_Allocate EQU 00h
    DOSMem_Free     EQU 01h
    DOSMem_Resize   EQU 02h

Int31_Int_Serv	    EQU 02h
    Int_Get_Real_Vec	EQU 00h
    Int_Set_Real_Vec	EQU 01h
    Int_Get_Excep_Vec	EQU 02h
    Int_Set_Excep_Vec	EQU 03h
    Int_Get_PMode_Vec	EQU 04h
    Int_Set_PMode_Vec	EQU 05h

Int31_Trans_Serv    EQU 03h
    Trans_Sim_Int   EQU 00h
    Trans_Far_Call  EQU 01h
    Trans_Call_Int_Proc EQU 02h
    Trans_Call_Back EQU 03h
    Trans_Free_CB   EQU 04h
    Trans_Get_Save_Addr EQU 05h
    Trans_Get_Sw_Addr	EQU 06h

Int31_Get_Version   EQU 04h

Int31_Mem_Mgt	    EQU 05h
    MemMgt_Get_Info EQU 00h
    MemMgt_Allocate EQU 01h
    MemMgt_Free     EQU 02h
    MemMgt_Resize   EQU 03h

Int31_Page_Lock     EQU 06h
    Lock_Region     EQU 00h
    Unlock_Region   EQU 01h
    Mark_Pageable   EQU 02h
    Mark_Not_Pageable	EQU 03h
    Get_Page_Size   EQU 04h

Int31_Demand_Page_Tune	EQU 07h
    Page_Candidate  EQU 00h
    Page_Discard    EQU 01h
    DPMI_Candidate  EQU 02h
    DPMI_Discard    EQU 03h

Int31_Map_Phys_Addr EQU 08h

Int31_Virt_Int_State	EQU 09h
    Get_Clear_Int_State EQU 00h
    Get_Set_Int_State	EQU 01h
    Get_Int_State   EQU 02h

Int31_MCP_Control   EQU 0Eh
    Get_Status	    EQU 00h
    Set_Emulation   EQU 01h


;--------------------------------------------------------------
; Mode resolution
;--------------------------------------------------------------

MODE_RES_320x200   equ 0
MODE_RES_320x240   equ 1
MODE_RES_400x300   equ 2
MODE_RES_512x384   equ 3
MODE_RES_640x400   equ 4
MODE_RES_640x432   equ 5	;NTSC TV all modes
MODE_RES_640x480   equ 6
MODE_RES_720x480   equ 7
MODE_RES_800x480   equ 8
MODE_RES_800x514   equ 9	;PAL TV 800x600 mode
MODE_RES_800x600   equ 10
MODE_RES_880x514   equ 11	;PAL TV > 800x600 modes
MODE_RES_1024x600  equ 12
MODE_RES_1024x768  equ 13
MODE_RES_1152x864  equ 14
MODE_RES_1280x1024 equ 15
MODE_RES_1600x1200 equ 16
MODE_RES_1280x480  equ 17
MODE_RES_1600x600  equ 18
MODE_RES_2048x600  equ 19
MODE_RES_2048x768  equ 20
MAX_MODE_RES	equ MODE_RES_2048x768

NTSC_RES	equ MODE_RES_640x432
PAL_RES_8x6	equ MODE_RES_800x514
PAL_RES_non8x6	equ MODE_RES_640x480
NTSC_SIZE	equ 28001b0h
PAL_SIZE_8x6	equ 3200202h		;displayable area size
PAL_SIZE_non8x6	equ 28001e0h		;displayable area size
PAL_PHY_8x6	equ 3200258h
PAL_PHY_non8x6	equ 28001e0h

;----------------------------------------------------------------------------
; S T R U C S
;----------------------------------------------------------------------------
SEGMENTDESCRIPTOR   struct
    sdLimit	dw  ?
    sdBase0_15	dw  ?
    sdBase16_23 db  ?
    sdAccess	db  ?
    sdSegmentType   db	?
    sdBase24_31 db  ?
SEGMENTDESCRIPTOR   ends

;
; Definitions for the access byte in a descriptor
;

; Following fields are common to segment and control descriptors

D_PRES	    equ 080h	    ; present in memory
D_NOTPRES   equ 0	; not present in memory

D_DPL0	    equ 0	; Ring 0
D_DPL1	    equ 020h	    ; Ring 1
D_DPL2	    equ 040h	    ; Ring 2
D_DPL3	    equ 060h	    ; Ring 3

D_SEG	    equ 010h	    ; Segment descriptor
D_CTRL	    equ 0	; Control descriptor

D_GRAN_BYTE equ 000h	    ; Segment length is byte granular
D_GRAN_PAGE equ 080h	    ; Segment length is page granular
D_DEF16     equ 000h	    ; Default operation size is 16 bits
D_DEF32     equ 040h	    ; Default operation size is 32 bits


; Following fields are specific to segment descriptors

D_CODE	    equ 08h	; code
D_DATA	    equ 0	; data

D_RX	    equ 02h	; if code, readable
D_X	equ 0	    ; if code, exec only
D_W	equ 02h     ; if data, writable
D_R	equ 0	    ; if data, read only

D_ACCESSED  equ 1	; segment accessed bit


; Useful combination access rights bytes

ST_RW_DATA  equ (D_PRES+D_SEG+D_DATA+D_W)
ST_R_DATA   equ (D_PRES+D_SEG+D_DATA+D_R)
ST_CODE     equ (D_PRES+D_SEG+D_CODE+D_RX)

D_PAGE32    equ (D_GRAN_PAGE+D_DEF32) ; 32 bit Page granular


; Masks for selector fields

SFM_SELECTOR	equ 0fff8h	; selector index
SFM_SEL_LOW equ 0f8h	    ; mask for low byte of sel indx
SFM_TABLE   equ 04h	; table bit
SFM_RPL     equ 03h	; privilige bits
SFM_RPL_CLR equ not 03h     ; clear ring bits


;
;Access modes
;

MEMORY	equ 0		; chip in Memory mode
ENGINE	equ 1		; chip in Engine mode
LA_GE	equ 2		; chip in bimodal mode


;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------

EngineWrite MACRO   Port:REQ, Segment:=<fs>, Value:=<eax>
    mov dword ptr Segment&:[Port&],Value
    endm

DisableAutoexec MACRO	Port:REQ, Segment:=<fs>
    mov dword ptr Segment&:[Port&],AUTO_OFF
    endm

;###
SWDEBUG   equ	  1	    ;take this out when we have real HW

WaitEngineIdle	macro	Segment:=<fs>
local wait1
if SWDEBUG
	push	eax
;
wait1:
	mov	eax,dword ptr Segment&:[8504h]
	test	ah,20h		;bit 13 (0/1 -> busy/idle)
	jz	short wait1	;engine busy, keep waiting
	pop	eax
endif	 ;*************** For DEBUGGING, take out later!!! ***************
	endm

DisablePD  macro   DataSegment:=<gs>, MMIOSegment:=<es>
local Cancel_Disable
	test	DataSegment&:S3MobileData.dwMobileCaps,FIX_GE_POWER_SAVING
	jz	short Cancel_Disable
	mov	byte ptr MMIOSegment&:[MMIO_SEQ_INDEX],EXT_SEQ_A_REG
	and	byte ptr MMIOSegment&:[MMIO_SEQ_DATA],not ESA_DS3D
	mov	byte ptr MMIOSegment&:[MMIO_SEQ_INDEX],EXT_SEQ_9_REG
	and	byte ptr MMIOSegment&:[MMIO_SEQ_DATA],not ES9_EPD
Cancel_Disable:
	endm

EnablePD  macro   DataSegment:=<gs>, MMIOSegment:=<es>
local Cancel_Enable
	test	DataSegment&:S3MobileData.dwMobileCaps,FIX_GE_POWER_SAVING
	jz	short Cancel_Enable
	mov	byte ptr MMIOSegment&:[MMIO_SEQ_INDEX],EXT_SEQ_9_REG
	or	byte ptr MMIOSegment&:[MMIO_SEQ_DATA],ES9_EPD
	mov	byte ptr MMIOSegment&:[MMIO_SEQ_INDEX],EXT_SEQ_A_REG
	or	byte ptr MMIOSegment&:[MMIO_SEQ_DATA],ESA_DS3D
Cancel_Enable:
	endm
;
; Added check for InSrcBlt active. If it is active, then we are in
; the middle of doing an image transfer, and the engine will stay busy
; until the transfer is finished. For this reason, we can't wait for the
; engine to be idle - it never will be. The engine will stay busy until
; the image transfer has completed. This fixes intermittent hanging problems.
;
WaitEngineIdle_Semaphore macro	MMIOSegment:=<fs>, DataSegment:=<ds>
local Wait1, Skip_Wait
if SWDEBUG
	cmp	DataSegment&:InSrcBlt,0ffh  ;if doing image transfer
	je	Skip_Wait		    ;then don't wait
	push	eax
;
Wait1:
	mov eax,dword ptr MMIOSegment&:[8504h]
	test	ah,20h			;bit 13 (0/1 -> busy/idle)
	jz	short Wait1		;engine busy, keep waiting
	pop	eax
Skip_Wait:
endif	 ;*************** For DEBUGGING, take out later!!! ***************
    endm


CheckFIFOSpace macro SpaceNeeded:REQ,	Segment:=<fs>
local check1
if SWDEBUG
    push    eax
check1:
    mov eax,dword ptr Segment&:[8504h]
    and ah,01fh 	;
    cmp ah,SpaceNeeded
    jb	short check1
    pop eax
endif	 ;*************** For DEBUGGING, take out later!!! ***************
    endm

;----------------------------------------------------------------------------
;Sets CMD_SET if autoexec flag is disabled
;Exits: destroys EAX
;----------------------------------------------------------------------------
WriteCMDAutoOFF MACRO	Port:REQ, Segment:=<fs>, Value:=<eax>
    mov dword ptr Segment&:[Port&],Value
endm

;----------------------------------------------------------------------------
;Sets CMD_SET if autoexec flag is enabled
;Exits: destroys EAX
;----------------------------------------------------------------------------
WriteCMDAutoON MACRO   Port:REQ, Segment:=<fs>, Value:=<eax>
if 0	;Enable this macro later
    mov dword ptr Segment&:[Port&],Value
endif;
endm

EX_MCTL_1   equ 53h
ENB_MMIO    equ 18h

EnableMMIO  macro
    mov dx,CRTC_REG	; 3d4h
    mov al,EX_MCTL_1	    ;
    out dx,al		;
    inc dx	    ;
    in	al,dx		;
    and al,0e7h 	; cr53[4:3] = MMIO Select
    or	al,ENB_MMIO	; New MMIO
    dec dx	    ;
    endm

DisableMMIO macro
    mov dx,CRTC_REG	;
    mov al,EX_MCTL_1	    ;
    out dx,al		;
    inc dx	    ;
    in	al,dx		;
    and al,0e7h
    out dx, al		;
    dec dx	    ;
    endm

;
; Off-screen region structure
;

SCREENRGN   struc
    sr_rect	db  8 dup (?)
    sr_width	dw	   ?
    sr_height	dw	   ?
SCREENRGN ends

;
;Wait util the beginning of next VSYNC
;
WaitVSYNC   macro
local VertLoop0, DisplayLoop0

    mov edx,03dah
VertLoop0:
    in	al,dx
    test    al,8
    jnz VertLoop0
;
DisplayLoop0:
    in	al,dx
    test    al,8
    jz	DisplayLoop0
;
    endm

GMEM_SHARE    =   2000h   ; (WINDOWS.INC): GlobalAlloc mem flags
GMEM_MOVEABLE	=   0002h		; (WINDOWS.INC): GlobalAlloc mem flags
GMEM_ZEROINIT	=   0040h		; (WINDOWS.INC):

;;---------------------------------------------------------------------------
;;
;; RD_IGA1_WR_IGA1 - Configure SR26 to read/write IGA1 only
;;
RD_IGA1_WR_IGA1 MACRO
        mov     dx,SEQ_REG              ; DX=3C4h
        mov     ax,4026h                ; Configure SR26
        out     dx,ax
ENDM

;;---------------------------------------------------------------------------
;;
;; RD_IGA2_WR_IGA2 - Configure SR26 to read/write IGA2 only
;;
RD_IGA2_WR_IGA2 MACRO
        mov     dx,SEQ_REG              ; DX=3C4h
        mov     ax,4F26h                ; Configure SR26
        out     dx,ax
ENDM

;;---------------------------------------------------------------------------
;;
;; RD_IGA1_WR_BOTH - Configure SR26 to read IGA1 but write IGA1/IGA2
;;
RD_IGA1_WR_BOTH MACRO
        mov     dx,SEQ_REG              ; DX=3C4h
        mov     ax,4426h                ; Configure SR26
        out     dx,ax
ENDM

;
;This macro initialize the panning window to the size specified by the
;passed resolution index.
;
InitPanningWindow macro
local IPW_Done, IPW_Center, IPW_AdjustToNewWindow, IPWA_CheckWidth
local IPWA_SaveLRCoords
	test	S3MobileData.dwMobileCaps,PANNING_ENABLED
	jnz	short IPW_AdjustToNewWindow
	shl	bx,2
	mov	ax,ResolutionTable[bx]
	mov	wLogScreenWidth,ax
	mov	bx,ResolutionTable[bx+2]
	test	S3MobileData.dwMobileCaps,PANNING_CENTER
	jnz	IPW_Center
	mov	dword ptr wVWTop,0
	dec	ax
	dec	bx
	mov	word ptr wVWBottom,bx
	mov	word ptr wVWRight,ax
	jmp	IPW_Done
;
;Set panning window start location to be the center of Desktop
;
IPW_Center:
	mov	cx,wScreenWidth
	mov	dx,wScreenHeight
	sub	cx,ax
	sub	dx,bx
	shr	cx,1
	shr	dx,1
	mov	word ptr wVWTop,dx
	mov	word ptr wVWLeft,cx
	dec	bx
	dec	ax
	add	bx,dx
	add	ax,cx
	mov	word ptr wVWRight,ax
	mov	word ptr wVWBottom,bx
	jmp	IPW_Done
;
;Adjust to the new panning window
;
IPW_AdjustToNewWindow:
	shl	bx,2
	mov	ax,ResolutionTable[bx]
	mov	wLogScreenWidth,ax
	mov	bx,ResolutionTable[bx+2]
	dec	bx
	dec	ax
	add	bx,wVWTop
;deal with the coming back from smaller panning window to larger panning
;window case
	cmp	bx,wScreenHeight
	jl	short IPWA_CheckWidth
	sub	bx,wScreenHeight
	inc	bx
	sub	wVWTop,bx
	mov	bx,wScreenHeight
	dec	bx
;
IPWA_CheckWidth:
	add	ax,wVWLeft
;deal with the coming back from smaller panning window to larger panning
;window case
	cmp	ax,wScreenWidth
	jl	short IPWA_SaveLRCoords
	sub	ax,wScreenWidth
	inc	ax
	sub	wVWLeft,ax
	mov	ax,wScreenWidth
	dec	ax
;
IPWA_SaveLRCoords:
	mov	word ptr wVWBottom,bx
	mov	word ptr wVWRight,ax

IPW_Done:
	endm


;
;This macro updates the stream processor assignment
;Input:
;	ds	-> data segment
;
AssignSPIGA1	macro
	or	S3MobileData.dwMobileCaps,SP_IGA1
	mov	dx,SEQ_REG
	mov	al,ARCH_CONFIG_REG		;SR30
	out	dx,al
	inc	dx
	in	al,dx
	and	al,not SPSRC_IGA2
	out	dx,al
	endm
;
;This macros sets display start address to the correct value for each
;IGA.
; Input :
;	ds	      : data segment
;
; Registers destroy:
;	all
;
SetDisplayAddr	macro
local SDA_CalcCurrentDisplayAddr, SDA_ConvertToCharClock, SDA_SetIGAs
local SDA_IGA1_0_IGA2Current, SDA_IGA1CurrentIGA2_0
local SDA_SetDSRegs, SDA_Done

	test	S3MobileData.dwMobileCaps,PANNING_ENABLED
	jnz	short SDA_CalcCurrentDisplayAddr
        RD_IGA1_WR_BOTH
	ResetDisplayAddr
        RD_IGA1_WR_IGA1
	jmp	SDA_Done
;
SDA_CalcCurrentDisplayAddr:
	xor	eax,eax
	mov	ebx,eax
	mov	bx,wVWTop
	mov	ax,wScreenWidthBytes
	mul	ebx
        mov     dwPSAddr,eax
        add     eax,dwFlipOffset
	shr	eax,2			;convert to number of double words
	mov	bx,wVWLeft
	cmp	wBpp,24
	jne	short SDA_ConvertToCharClock
	mov	cx,bx
	shl	bx,1
	add	bx,cx			;pitch in bytes
	and	cx,3			;ensure pixel boundary = dword boundary
	mov	dx,cx
	shl	cx,1
	add	cx,dx
	sub	bx,cx
	shr	bx,2			;pitch in double words
	jmp	short SDA_SetIGAs
;
SDA_ConvertToCharClock:
	mov	cl,bShiftPelsToDWords
	shr	bx,cl
;
; IGA1_CURRENT_IGA2_0 : IGA1 to current address, IGA2 to 0
; IGA1_0_IGA2_CURRENT : IGA1 to 0, IGA2 to current address
;
SDA_SetIGAs:
	add	ebx,eax
	test	S3MobileData.dwMobileCaps,PANNING_IGA1
	jnz	short SDA_IGA1CurrentIGA2_0

SDA_IGA1_0_IGA2Current:
	ResetDisplayAddr		;Set DSAddr of IGA1 to 0
        RD_IGA2_WR_IGA2
	jmp	short SDA_SetDSRegs
;
SDA_IGA1CurrentIGA2_0:
        RD_IGA2_WR_IGA2
	ResetDisplayAddr		;Set DSAddr of IGA2 to 0
        RD_IGA1_WR_IGA1
;
SDA_SetDSRegs:
	mov	SavedDispStart,ebx
	mov	dx,CRTC_REG
	mov	al,0dh
	mov	ah,bl
	out	dx,ax			;set display start address low
	dec	al
	mov	ah,bh
	out	dx,ax			;display start high
	mov	eax,ebx
	shr	eax,8
	mov	al,69h
	out	dx,ax			;upper four bits of display start addr
        RD_IGA1_WR_IGA1
	test	S3MobileData.dwMobileCaps,PANNING_IGA1
	jnz	short SDA_Done
        RD_IGA1_WR_IGA1
;
SDA_Done:
	endm

DISPLAYADDR		equ	1
STRIDE_DISPLAYADDR	equ	2

;
;This macros is called to initialize the last two compoments for the panning
;process : logical screen width and display start address.
;
;Input:
;	ds : data segment
;
;Registers destroy:
;	all
;
InitPanningVars macro	Vars
local IPV_SetDisplayStart, IPV_ConvertToCharClock, IPV_SetIGAs
local IPV_SetDSRegs, IPV_Done

ifidn	<Vars>,<STRIDE_DISPLAYADDR>
        RD_IGA1_WR_BOTH
	mov	eax,CurrentLogWidRegs
	mov	dx,CRTC_REG
	out	dx,ax
	shr	eax,16
	out	dx,ax
        RD_IGA1_WR_IGA1
endif
;
IPV_SetDisplayStart:
	xor	eax,eax
	mov	ebx,eax
	mov	bx,wVWTop
	mov	ax,wScreenWidthBytes
	mul	ebx
	shr	eax,2			;convert to number of double words
	mov	bx,wVWLeft
	cmp	wBpp,24
	jne	short IPV_ConvertToCharClock
	mov	cx,bx
	shl	bx,1
	add	bx,cx			;pitch in bytes
	and	cx,3			;ensure pixel boundary = dword boundary
	mov	dx,cx
	shl	cx,1
	add	cx,dx
	sub	bx,cx
	shr	bx,2			;pitch in double words
	jmp	short IPV_SetIGAs
;
IPV_ConvertToCharClock:
	mov	cl,bShiftPelsToDWords
	shr	bx,cl
;
IPV_SetIGAs:
	add	ebx,eax
	test	S3MobileData.dwMobileCaps,PANNING_IGA1
	jnz	short IPV_SetDSRegs
        RD_IGA2_WR_IGA2
;
IPV_SetDSRegs:
	mov	SavedDispStart,ebx
	mov	dx,CRTC_REG
	mov	al,0dh
	mov	ah,bl
	out	dx,ax			;set display start address low
	dec	al
	mov	ah,bh
	out	dx,ax			;display start high
	mov	eax,ebx
	shr	eax,8
	mov	al,69h
	out	dx,ax			;upper four bits of display start addr
	test	S3MobileData.dwMobileCaps,PANNING_IGA1
	jnz	short IPV_Done
;
;Panning on IGA2, display start address on IGA2 has been set correctly. Since,
;there is no panning on IGA1, set IGA1's display start address to 0.
;
        RD_IGA1_WR_IGA1
	ResetDisplayAddr
;
IPV_Done:
	endm

;
;Set display start address of IGA1 to 0
;
ResetDisplayAddr macro
	mov	dx,CRTC_REG
	mov	ax,0ch
	out	dx,ax
	inc	al
	out	dx,ax
	mov	ax,069h
	out	dx,ax
	endm

;
;
;This macro sets cursor type and bitmap to either software or hardware
;cursor based on the passed CursorType
;
;Input:
;	ds,es	-> Data segment
;
;Register destroys:
;	ax,bx,cx,dx
;
;
SetCursorType	macro	CursorType
local SCT_Done
ifidn	<CursorType>,<HARDWARECURSOR>
	cmp	CursorFmt,MONO_CURSOR
	jmp	short SCT_Done			;color cursor, done
	mov	wCursorType,HARDWARECURSOR
	mov	MonoCursorType,HARDWARECURSOR
;	SwitchToHWCursor
else
	cmp	MonoCursorType,SOFTWARECURSOR
	je	short SCT_Done
	SwitchToSWCursor
endif

SCT_Done:
	endm

;
SetActiveDisplay macro DisplayType
local	Mobile_parts
	mov	cl,DisplayType
	test	S3MobileData.dwMobileCaps,MOBILE_FAMILY ;GX2?
	jnz	short Mobile_parts			;N, continue
	and	cl,0C7h
Mobile_parts:
	mov	ax,VESA_EXT_SERVICE
	mov	bx,SET_ACTIVE_DISPLAY
	int	10h
	endm



SwitchToSWCursor	macro
local SSW_SetCursor, SSW_Done
	mov	dx,CRTC_REG
	mov	ax,03f4eh
	out	dx,ax
	mov	ax,03f4fh
	out	dx,ax
	mov	ax,seg DIB_SetCursorExt ;
	shl	eax,16			;
	mov	ax,DIB_SetCursorExt	;
	mov	SetCursorFn,eax
	mov	MonoCursorFn,eax
	mov	ax,wCursorType
	mov	wCursorType,SOFTWARECURSOR
	mov	MonoCursorType,SOFTWARECURSOR
	cmp	ax,SOFTWARECURSOR
	je	short SSW_Done
;
SSW_SetCursor:
	push	ds
	lea	ax,XHot 		;-> starting address of cursor struct
	push	ax
	push	lpDriverPDevice
	cCall	SetCursorFn
	mov	eax,dword ptr wAbsY
	push	eax
	call	MoveCursor

SSW_Done:
	endm

SwitchToHWCursor	macro
local	STHC_UpdateCursor, STHW_Done
	push	es
	pushd	0			
	push	lpDriverPDevice
	call	[SetCursorFn]
	pop	es
	mov	ax,seg SetHWCursor
	shl	eax,16
	mov	ax,SetHWCursor
	mov	MonoCursorFn,eax
	mov	SetCursorFn,eax
	mov	CursorFmt,0101h
	mov	ax,wCursorType
	mov	wCursorType,HARDWARECURSOR
	mov	MonoCursorType,HARDWARECURSOR
	cmp	ax,HARDWARECURSOR
	je	short STHW_Done
;
STHC_UpdateCursor:
	push	ds
	lea	ax,XHot
	push	ax
	push	lpDriverPDevice
	cCall	SetCursorFn
	mov	eax,dword ptr wAbsY
	push	eax
	call	MoveCursor
STHW_Done:
	endm



;------------------------------------------------------------------------------
; [OMMInitStatus] equates
;
OMIS_INITPASS		equ	00h	; bits 7:0 = 00h, RealizeObject enabled
;
OMIS_INITFAIL		equ	01h	; bit 0: Offscrn memory mgr init failed
OMIS_DISABLED		equ	02h	; bit 1: RealizeObject disabled by user
OMIS_BUSTHROTTLE	equ	04h	; bit 2: Bus throttle code in use
OMIS_RESERVED		equ    0F8h	; bits 7:3 = Reserved

;
; S3DriverData structure
;

S3DRIVERDATA	struc
	InSrcBltAddress dd	       ?
	MCLKData	db	       0
	VideoData	db	       0
	VGAFlag 	db	       0
	reserved	db	       ?;
	DACType 	dw	       ?;
	dwRasPreCharge	dd	       0
	PanelResolution dw	       ?;
	LogResolution	dw	       ?;
	PhysResolution	dw	       ?
	LAGEOnAddress	dd	       ?
	ScreenWidth	dw	       ?
	ScreenHeight	dw	       ?
S3DRIVERDATA ends

;
; S3DriverData.VGAFLag equates
;
IN_VGA_MEMORY_MODE  equ 1
IN_DOS_FULL_SCREEN  equ 2


WaitDMA 	macro	Segment:=<fs>
local DMA_wait, DMA_exit, Wait_Idle, Wait_Read

	push	ax
	mov	ax,word ptr Segment&:[859Ch]	; DMA Enable Reg
	test	ax,01h				; bit0 = 1 => DMA enable
	jz	short DMA_exit

	push	bx
	mov	ax,word ptr Segment&:[8594h]	; DMA Write Ptr
	and	ax,0ffch
DMA_wait:
	mov	bx,word ptr Segment&:[8598h]	; DMA Read Ptr
	and	bx,0ffch
	cmp	ax,bx
	jne	DMA_wait

	mov	dword ptr Segment&:[859Ch],0	; disable DMA

Wait_Idle:
	mov	ax,word ptr Segment&:[8504h]	; wait for engine to be Idle
	test	ax,2000h			; bit 13 (0/1 -> busy/idle)
	jz	short Wait_Idle 		; engine busy, keep waiting

	pop	 bx
DMA_exit:
	pop	 ax
	endm

;Following are the definition of TV

TV_TYPE_NTSC		equ	0
TV_TYPE_PAL		equ	1
TV_TYPE_UNKNOWN 	equ	2
TV_TYPE_NO_CONNECT	equ	3



;------------------------------------------------------------------------------
; End of S3.INC file
;------------------------------------------------------------------------------
        
