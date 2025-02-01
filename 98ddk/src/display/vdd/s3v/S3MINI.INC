;---------------------------------------------------------------------------
;Include file for MiniVDD definitions:
;

;
;Chip ID equates:
;
S3911	equ	080h			; Major chip id 911/924
S3924	equ	080h			; Major chip id 911/924
S3801	equ	0a0h			; Major chip id 801/805
S3805	equ	0a0h			; Major chip id 801/805
S3928	equ	090h			; Major chip id 928
S3928PCI equ	0b0h			; Major chip id 928 PCI
S3864	equ	0c0h			; Major chip id 864
S3964	equ	0d0h			; Major chip id 964
S3764	equ	0881100h		; 764 id = CR2d | CR2e | CR2f
S3732	equ	0881000h		; 732
S3765	equ	0881140h		; 765 id = CR2d | CR2e | CR2f
S3M65	equ	0881200h		; M65 id = CR2d | CR2e | CR2f
S3M66	equ	0881500h		; M66 id = CR2d | CR2e | CR2f
S3767	equ	0881400h		; 767 id = CR2d | CR2e | CR2f
S3866	equ	0888000h		; 866
S3868	equ	0889000h		; 868
S3968	equ	088b000h		; 968
S3968_B equ	088f000h		; 968 rev. B
VIRGE	equ	0563100h		; ViRGE ;BC1
ASPEN	equ	0883d00h		; ASPEN ;BC1
S3775	equ	0890100h		;

; Patch for first silicon

S3M3	equ	08C0000h		; M3
S3M5	equ	08C0100h		; M5
S3M5P	equ	08C0200h                ; M5+
S3M5PM	equ	08C0300h                ; M5+ Macrovision
VIRGEGX2 equ	08A1000h		; Virge GX2
VIRGEGX2P equ	08A1200h		; Virge GX2+
VIRGEGX2PM equ	08A1100h		; Virge GX2+ Macrovision

VIRGEGX3 equ    08A2000h                ; Virge GX3

TR64V_FamilyId	equ	0890000h	; Trio64V+ Compatible Id
VIRGE_FamilyId	equ	08A0000h	; ViRGE Compatible Id
Mobil_FamilyId	equ	08C0000h	; Mobile Compatible Id

FOURTH_GEN	equ	01h		; OR'd into chip ID if 4th generation

REV764GSpnp	equ	020h
REV764NECpnp	equ	030h
REV764UMCpnp	equ	003h
REV764TSMCpnp	equ	012h
REV732GSpnp	equ	022h
REV765		equ	040h	       ;Rev Id for 765.

;Board/BIOS Type return from minivdd
BIOS_GENERAL	equ	1	;driver bypass minivdd call if it's not Diamond bios
BIOS_DELL	equ	2
BIOS_DIAMOND	equ	4
BIOS_STB	equ	8

;
;Revision ID for M65
;
M65_REV_E	equ	043h

;
;Equate for bFamilyFlag
;
FIRST_GEN_GE	equ	001h
TR64V_FAMILY	equ	00000010b
VIRGE_FAMILY	equ	00000100b
MOBILE_FAMILY	equ	00000001b
UMA_FAMILY	equ	00001000b

;
;Equates for MMIO flag
;
MMIO_32 	equ	001b		;32-bit or packed MMIO
MMIO_READ	equ	010b		;MMIO read supported
;
STATUS_OFFSET_16	   equ	   9ae8h ; I/O offset
STATUS_OFFSET_MMIO_32	   equ	   8118h ; Packed offset
STATUS_OFFSET_MMIO_32V	   equ	   8504h ; Virge/Aspen offset

;
;Equates for bus type
;
BUS_VL		equ	001b

;
;Chip running mode equates (from the S3 Mini-display-driver code):
;
ENGINE	equ	1			; chip in Engine mode
MEMORY	equ	0			; chip in MEMORY mode
LA_GE	equ	2			; chip in bimodal mode

;
;Equates for Advance Power Management
;
WM_POWERBROADCAST		equ	0218h
PBT_APMQUERYSUSPEND		equ	0000h
PBT_APMSUSPEND			equ	0004h
PBT_APMSTANDBY			equ	0005h
PBT_APMRESUMESUSPEND		equ	0007h
PBT_APMRESUMESTANDBY		equ	0008h
PBT_APMOEMEVENT 		equ	000Bh
OEM_HOTKEY_MODESWITCH		equ	0201h

;
;Equates for S3 extended VGA registers
;
CRTC_REG			equ	3d4h
SEQ_REG 			equ	3c4h
DISPLAY_MODE_REG		equ	6bh
CRTC_MMIO_REG			equ	53h
MMIO_ENABLED			equ	18h

;
;Equates for S3 time out and fifo registers
;
PS_1_TIMEOUT_REG		equ	7bh
PS_2_TIMEOUT_REG		equ	7ch
SS_TIMEOUT_REG			equ	7dh
PS_1_FIFO_TH_REG		equ	85h
PS_2_FIFO_TH_REG		equ	86h
SS_FIFO_TH_REG			equ	87h


;
;Equates for display configuration interface register. 
;
DCI_REG			equ	50h
DCI_ENABLED		equ	01000000b	;bit 6 = 1:DCS interface enable

;
;Bits definition of scratch register
;
CRT_ONLY			equ	001b
FP_ONLY 			equ	010b
FP_CRT_SIM			equ	011b
FP_TV_SIM			equ	110b
DUOVIEW 			equ	1000b
MODE_MASK			equ	111b

;
;Address of extended sequencer registers
;
FP_DISPLAY_REG			equ	31h
FP_IGA_MASK			equ	0f8h

FP_ENABLE			equ	10h	;1 : enable ; 0 : disable
;
FP_CONFIG_REG			equ	3dh
FP_TYPE 			equ	08h
FP_SIZE 			equ	0c0h
;
FP_HORZ_SIZE			equ	61h
FP_HORZ_OVERFLOW		equ	66h
FP_HORZ_EXT_BIT 		equ	02h
;
FP_VERT_SIZE			equ	69h
FP_VERT_OVERFLOW		equ	6eh
FP_VERT_EXT_MASK		equ	070h
FP_VERT_EXT_BIT_POS		equ	04h
;

; Vertical display registers

VERT_1_REG		equ	012h
VERT_2_REG		equ	07h
VERT_3_REG		equ	05eh
VERT_MASK		equ	0fdh

; Horizontal display registers

HORZ_1_REG		equ	01h
HORZ_2_REG		equ	05dh
HORZ_MASK		equ	0feh

;
;Equates for FIFO control registers in M65 rev. E
;
FIFO_IO_REG_LO		equ	3eh
FIFO_IO_REG_HI		equ	3fh
FIFO_CONTROL_BIT	equ	80h
FIFO_CONTROL_MASK	equ	7fh

;
;Equates for mobile caps
;
;MOBILE_FAMILY			equ	00000001b
PANNING_ON			equ	00000010b
RESTORE_DISPLAY_REGS		equ	00000100b
IN_DOS_FULLSCREEN		equ	00001000b
M65_NEW_FIFO_CONTROL		equ	00010000b
;PANNING_ON_CRT 		equ	00100000000000b

;
;Equates for bAPMHotkeyEvent
;
HK_DISPLAY_SWITCH	equ	00000001b	;display switch
HK_CENTER_EXPAND	equ	00000010b
STD_IN_GE_FUNCTION	equ	00000100b
APM_RESUME_IN_DOS	equ	00001000b
STD_RESTORE_GE_REGS	equ	00010000b
STD_RESTORE_SP_REGS	equ	00100000b
APM_RESUME_SUSPEND	equ	01000000b
HK_DSWITCH_IN_DOS	equ	10000000b

GE_BASE 		equ	8100h
GE_SIZE 		equ	026h
SP_BASE 		equ	8180h
SP_SIZE 		equ	020h
GE_BASEV1		equ	0A4D4h
GE_BASEV2		equ	0A504h
GE_BASEV3		equ	0A568h
GE_SIZEV1		equ	11		;Src_Base ~ Src_FG_Clr
GE_SIZEV2		equ	3		;RWidth_Height ~ RDest_XY
GE_SIZEV3		equ	6		;PRdx ~ PyCnt

; Equates for wSPStyle
SP_Trio 	equ	0001h
SP_Virge	equ	0002h
SP_VX		equ	0004h
SP_VirgeGX	equ	0008h
SP_TrioV2	equ	0010h
SP_M5		equ	0020h
SP_Yosemite	equ	0040h

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


DISABLE_SP		equ  00010000h	  ;disable SP in exp & duoView
LCD_IGA1_CRT_IGA2	equ  00020000h
SINGLE_DISPLAY		equ  00040000h
REJECT_MODESWITCH_IN_SP equ  00080000h
;M65_NEW_FIFO_CONTROL	 equ  00100000h    ;M1, rev. E : sr3f[7] = 1 :
					  ;fifo controlled via I/O
					  ;sr3f[7] = 0 : fifo controlled
					  ;via 81ech
PANNING_ON_CRT		equ  00200000h
DUOVIEW_DIFF_REFRESH	equ  00400000h
FORCED_VIDEO_CENTERING	equ  00800000h	  ;User wants video in centering
DEFAULT_TO_SIM		equ  01000000h	  ;set sim when panel size =
					  ;mode instead of expansion mode

;
;Mobile data area structure
;
S3_MOBILE_DATA		struct
	wSize		dw	?		;size of this structure
	bCurrentDisplay db	?		;CRT_Only, LCD_Only, Sim., ...
	bAPMEvents	db	?		;Display switch, Center/Expand
	dwMobileCaps	dd	?		;Caps.
	GERegs		dw	GE_SIZE dup (0) ;for trio core
	SPRegs		dd	SP_SIZE dup (0)
	wSPStyle	dw	?		;What kind of SP is built in
	b3DCaps 	db	?		;Got 3D core ? what kind ?
	Reserved	db	?
S3_MOBILE_DATA		ends
;S3_MOBILE_DATA 	struct
;	wSize		dw	?		;size of this structure
;	bCurrentDisplay db	?		;CRT_Only, LCD_Only, Sim., ...
;	bAPMEvents	db	?		;Display switch, Center/Expand
;	GERegs		dw	GE_SIZE dup (0)
;	SPRegs		dd	SP_SIZE dup (0)
;S3_MOBILE_DATA 	ends

;
;Stream processor mode
;
SP_IN_VGA_MODE		equ	0100b
SP_IN_ENHANCE_MODE	equ	1100b

;
;Equates for stream processor flags
;
SP_SUPPORT		equ	0001b
SP_WAS_ENABLED		equ	0010b
SP_SUPPORT_V		equ	0101b
SP_VIRGEFamily		equ	0100b
;SP_VIRGE		equ	0100b
SP_ENABLE_MASK		equ	1100b

;
;ViRGE Core GE Regs (all double words)
;
;Begining of Bitblt regs

Src_Base		equ	0a4d4h
Dest_Base		equ	0a4d8h
Clip_L_R		equ	0a4dch
Clip_T_B		equ	0a4e0h
Dest_Src_Str		equ	0a4e4h
Mono_Pat_0		equ	0a4e8h
Mono_Pat_1		equ	0a4ech
Pat_BG_Clr		equ	0a4f0h
Pat_FG_Clr		equ	0a4f4h
Src_BG_Clr		equ	0a4f8h
Src_FG_Clr		equ	0a4fch
CMD_SET 		equ	0a500h
RWidth_Height		equ	0a504h
RSrc_XY 		equ	0a508h
RDest_XY		equ	0a50ch

;end of Bitblt regs

;Extra regs for Polygon and 2D line

PRdx			equ	0a568h
PRxStart		equ	0a56ch
PLdx			equ	0a570h
PLxStart		equ	0a574h
PyStart 		equ	0a578h
PyCnt			equ	0a57ch

;
;GE Regs
;
Srcy_REG		equ	8100h
Srcx_REG		equ	8102h
Dstx_REG		equ	8108h
Dsty_REG		equ	810ah
ERROR_TERM_REG		equ	8110h
STATUS_REG		equ	8118h

COLOUR_0_REG		equ	8120h
COLOUR_1_REG		equ	8124h

WRITE_MASK		equ	8128h
READ_ENABLE_REG 	equ	812ch

COLOUR_COMPARE_REG	equ	8130h
FUNCTION_0_REG		equ	8134h
FUNCTION_1_REG		equ	8136h
CLIPT_REG		equ	8138h
CLIPL_REG		equ	813ah
CLIPB_REG		equ	813ch
CLIPR_REG		equ	813eh
MODE_REG		equ	8140h
MULT_MISC_REG		equ	8144h

RECT_HEIGHT_REG 	equ	8148h
RECT_WIDTH_REG		equ	814ah

;
;Stream processor registers
;
ENABLE_SP		equ	067h
NUM_SP_REGS		equ	16h
P_CONTROL		equ	8180h	;Primary Stream Control register
SP_CKEY_LOW		equ	8184h	;Color/Chroma Key Control
S_CONTROL		equ	8190h	;Secondary Stream Control
SP_CKEY_HI		equ	8194h	;Chroma Key Upper Bound
S_HK1K2 		equ	8198h	;Secondary Stream Stretch/Filter const
SP_BLEND_CONTROL	equ	81a0h	;Blend Control
P_0			equ	81c0h	;Primary Stream Frame Buffer Address 0
P_1			equ	81c4h	;Primary Stream Frame Buffer Address 1
P_STRIDE		equ	81c8h	;Primary Stream Stride
SP_LPB_DB		equ	81cch	;Double buffer and LPB Support
S_0			equ	81d0h	;Secondary Stream Frame Buffer Addr 0
S_1			equ	81d4h	;Secondary Stream Frame Buffer Addr 1
S_STRIDE		equ	81d8h	;Secondary Stream Stride
SP_OPAQUE_CONTROL	equ	81dch	;Opaque Overlay Control
S_VK1			equ	81e0h	;K1 Vertical Scale Factor
S_VK2			equ	81e4h	;K2 Vertical Scale Factor
S_VDDA			equ	81e8h	;DDA Vertical Accumulator Init Value
SP_FIFO_CONTROL 	equ	81ech	;Stream FIFO and RAS Controls
P_XY			equ	81f0h	;Primary Stream Window Start Coord
P_WH			equ	81f4h	;Primary Stream Window Size
S_XY			equ	81f8h	;Secondary Stream Window Start Coord
S_WH			equ	81fch	;Secondary Stream Window Size

SP_FIFO_CONTROLV	equ	8200h	;ViRGE
SP_TIMEOUT		equ	8208h
MISC_TIMEOUT		equ	820ch

;
;EDID Data Equates
;
EDID_640_75		equ	000000004h
EDID_640_72		equ	000000008h
EDID_640_67		equ	000000010h
EDID_640_60		equ	000000020h
EDID_800_75		equ	000004000h
EDID_800_72		equ	000008000h
EDID_800_60		equ	000000001h
EDID_800_56		equ	000000002h
EDID_1024_75		equ	000000200h
EDID_1024_70		equ	000000400h
EDID_1024_60		equ	000000800h
EDID_1024_43		equ	000001000h
EDID_1280_75		equ	000000100h

EDIDBlock	struc
	EDID_Header		db	8 dup(?)
	EDID_ProductInfo	db	10 dup(?)
	EDID_EDIDVersion	dw	?
	EDID_BasicParams	db	5 dup(?)
	EDID_ColorChars 	db	10 dup(?)
	EDID_EstablishedRates	db	3 dup(?)
	EDID_StandardRates	db	16 dup(?)
	EDID_DetailedRates	db	72 dup(?)
	EDID_ExtensionFlag	db	?
	EDID_Checksum		db	?
EDIDBlock	ends

;-----------------------------------------
;  A.L.
;-----------------------------------------
; WIN32 DIOC interface
; #define S3KERNEL_PHYSTOLIN			10
; #define S3KERNEL_LINTOPHYS			11
; #define S3KERNEL_DeviceID		8888

S3MINI_DeviceID    equ 8889

S3KERNEL_PHYSTOLIN equ 10
S3KERNEL_LINTOPHYS equ 11
S3KERNEL_GETDMABUF equ 12

;
;The PerVMData structure.  This structure represents the our reserved space
;in the so-called CB data structure.  The CB data structure is created for
;each VM started in the system.  We can therefore, save states that we need
;to remember on a per-VM basis.  This data will vary for every mini-VDD.
;The S3 chipset requires a few registers states for its extension registers
;to be saved.
;
PerVMData	struc
	Reg31HState	db	?
	Reg35HState	db	?
	Reg40HState	db	?
	Reg41HState	db	?
	Reg42HState	db	?
	Reg51HState	db	?
	Reg53HState	db	?
	Reg58HState	db	?
	Reg59HState	db	?
	Reg5AHState	db	?
	Reg67HState	db	?
	Reg6AHState	db	?
	Reg6DHState	db	?
	Reg6EHState	db	?
	BIOSVideoMode	dw	?
	Reg3C2State	db	?
	pad1		db	?
	pad2		dw	?
PerVMData	ends
.errnz	size PerVMData MOD 4

;
;The MonitorTimimg structure stores monitor timing information required
;to calculate CRTC parameters and the result CRTC values to be programmed
;in to registers to achieve desired monitor refresh rate. The timing
;values in the structure can be either derived from VDIF (by VESA DDC2 call)
;info or calculated based on information from registry. The registry can
;only provide limited information for calculation. There are some assumptions
;have to be made to obtain all the parameters. These assumptions might not
;center the screen accurately. User has to adjust the screen maunally by
;pushing buttons on monitor or rely on the future Galileo like utilities.
;
MONITORTIMING	struct
	mtHorPixels	dw	?	;pixels
	mtVerPixels	dw	?	;pixels(lines)
	mtHorTotalPixels dw	?	;pixels
	mtVerTotalPixels dw	?	;pixels(lines)
	mtBitsPixel	db	?	;bits
	mtPASize	db	?	;bits
	mtScaleFactor	dw	?	;%
	mtHorFrequency	dw	?	;kHz
	mtVerFrequency	dw	?	;Hz
	mtPixelClock	dw	?	;MHz
	mtHorTotalTime	dw	?	;microseconds
	mtHorAddrTime	dw	?	;microseconds
	mtHorBlankStart dw	?	;microseconds
	mtHorBlankTime	dw	?	;microseconds
	mtHorSyncStart	dw	?	;microseconds
	mtHorSyncTime	dw	?	;microseconds
	mtVerTotalTime	dw	?	;milliseconds
	mtVerAddrTime	dw	?	;milliseconds
	mtVerBlankStart dw	?	;milliseconds
	mtVerBlankTime	dw	?	;milliseconds
	mtVerSyncStart	dw	?	;milliseconds
	mtVerSyncTime	dw	?	;milliseconds
	mtHorTotalChars dw	?	;chars
	mtHorAddrChars	dw	?	;chars
	mtHorBlankStartChars dw ?	;chars
	mtHorBlankChars dw	?	;chars
	mtHorSyncStartChars dw	?	;chars
	mtHorSyncChars	dw	?	;chars
	mtVerTotalLines dw	?	;lines
	mtVerAddrLines	dw	?	;lines
	mtVerBlankStartLines dw ?	;lines
	mtVerBlankLines dw	?	;lines
	mtVerSyncStartLines dw	?	;lines
	mtVerSyncLines	dw	?	;lines
	mtCR0		dw	0000h	;
	mtCR1		dw	0001h	;
	mtCR2		dw	0002h	;
	mtCR3		dw	0003h	;
	mtCR4		dw	0004h	;
	mtCR5		dw	0005h	;
	mtCR6		dw	0006h	;
	mtCR7		dw	0007h	;
	mtCR9		dw	0009h	;
	mtCR10		dw	0010h	;
	mtCR11		dw	0011h	;
	mtCR12		dw	0012h	;
	mtCR15		dw	0015h	;
	mtCR16		dw	0016h	;
	mtCR18		dw	0018h	;
	mtCR3b		dw	003bh	;
	mtCR3c		dw	003ch	;
	mtCR42		dw	0042h	;
	mtCR43		dw	0043h	;
	mtCR5d		dw	005dh	;
	mtCR5e		dw	005eh	;
	mtCR67		dw	0067h	;
MONITORTIMING	ends

;
;Wait util the beginning of next VSYNC
;
WaitVSYNC	macro
local VertLoop0, DisplayLoop0

	mov	edx,03dah
VertLoop0:
	in	al,dx
	test	al,8
	jnz	VertLoop0
;
DisplayLoop0:
	in	al,dx
	test	al,8
	jz	DisplayLoop0
;
	endm
;
;Wait until the engine is idle.
;
MakeEngineIdle	macro
local	MEI_TestBltStatus_Virge, MEI_TestBltStatus_IO, MEI_Exit

	mov	ecx,1000
	test	MMIOFlag,MMIO_READ
	jz	short MEI_TestBltStatus_IO
;
	mov	edx,MMIOLinearAddrBase	;
	add	edx,StatusReg		;

;	cmp	ChipID,ASPEN		;is it ASPEN?
;	je	short MEI_TestBltStatus_Virge
;	cmp	ChipID,VIRGE		;is it ViRGE?
;	je	short MEI_TestBltStatus_Virge
	test	bFamilyFlags,VIRGE_FAMILY	;ViRGE 2D core ?
	jnz	short MEI_TestBltStatus_Virge	;yes.

@@:
	mov	ax,word ptr [edx]	;
	test	ax,0200h		;
	jz	MEI_Exit		;nope, continue
	call	Delay			;give it a rest to let it finish
	dec	ecx			;bump down our loop counter
	jnz	short @B		;wait for it to finish
	jmp	short MEI_Exit
;
MEI_TestBltStatus_Virge:
	mov	ax,word ptr [edx]	;
	test	ax,2000h		;
	jnz	short MEI_Exit		;nope, continue
	call	Delay			;give it a rest to let it finish
	dec	ecx			;bump down our loop counter
	jnz	short MEI_TestBltStatus_Virge ;wait for it to finish
	jmp	short MEI_Exit
;
MEI_TestBltStatus_IO:
	mov	edx,9ae9h		;EDX --> BLT status register
	in	al,dx			;get BLT status
	test	al,02h			;is BLTer busy?
	jz	short MEI_Exit		;nope, continue
	call	Delay			;give it a rest to let it finish
	dec	ecx			;bump down our loop counter
	jnz	short MEI_TestBltStatus_IO ;wait for it to finish
;
MEI_Exit:
	endm



;
; S3DriverData structure
;

S3DRIVERDATA	struc
	InSrcBltAddress dd	       ?
	MCLKData	db	       ?
	VideoData	db	       ?
	VGAFlag 	db	       ?
	reserved	db	       ?;
	DACType 	dw	       ?
	dwRasPreCharge	dd	       ?
	PanelResolution dw	       ?;
	LogResolution	dw	       ?;
  	PhysResolution  dw             ?
  	LAGEOnAddress   dd             ?
        ScreenWidth     dw             ?
        ScreenHeight    dw             ?
S3DRIVERDATA ends

PS3DRIVERDATA TYPEDEF	PTR S3DRIVERDATA
;
; Write Combining Equates
;
MTRR_MASK0		equ	201h
MTRR_USER_NR		equ	6
MTRR_MASK_VALID		equ	800h
WC_TYPE			equ	01h
MASK_1MB		equ	0fff00000h
MASK_2MB		equ	0ffe00000h
MASK_4MB		equ	0ffc00000h
BASE_RESERVED		equ	0f00h
MASK_RESERVED		equ	07ffh
HI_RESERVED		equ	0fffffff0h
BANKING_ON		equ	0ff000000h
EFLAGS_ID		equ	200000h
RD_WRMSR_ON		equ	20h
MTRR_ON			equ	1000h
LAST_MASK		equ	20bh
HI_MASK			equ	0fh
ALIGN_1MB		equ	0fffffh
ALIGN_2MB		equ	1fffffh
ALIGN_4MB		equ	3fffffh
CLEAR_LOW3B		equ	0fffff000h

_cpuid	 macro
	dw	0a20fh
	endm


CheckMTRR	macro
local	NoMTRR, ExitMTRR

	push	eax
	push	ebx
	push	ecx
	push	edx
;;Check if CPUID is available
	mov	edx,esp		; save the stack pointer
	and	esp,not 3	; allign it to avoid AC fault
	pushfd
	pop	eax
	mov	ecx,eax		; ECX = original EFLAGS
	xor	eax,EFLAGS_ID	; flip ID bit in EFLAGS
	push	eax
	popfd			; copy to EFLAGS
	pushfd
	pop	eax		; EAX = new EFLAGS value
	xor	eax,ecx		; EAX = 200000h if CPUID supported
				; EAX = 0 if CPUID not supported
	push	ecx
	popfd			; restore original EFLAGS
	mov	esp,edx		; restore original stack pointer
	test 	eax,EFLAGS_ID
	jz	NoMTRR		; ID bit = 0 => CPUID not supported

	xor	eax,eax		; 
	_cpuid
	cmp	eax,1		; Pentium ?
	je	NoMTRR		; Yes => jump
	mov	eax,1
	_cpuid
	test	edx,RD_WRMSR_ON	; Processor supports the RDMSR and
	jz	NoMTRR		; WRMSR instructions?; jump if not
	test	edx,MTRR_ON	; Processor supports MTRRs?
	jz	NoMTRR		; jump if not
	stc			; CY = 1 if we can set MTRRs
	jmp	ExitMTRR
NoMTRR:
	clc			; CY = 0 if we cannot use MTRRs
ExitMTRR:
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax

	endm

_rdmsr	 macro
	dw	320fh
	endm


_wrmsr	 macro
	dw	300fh
	endm

;------------------------------------------------------------------------------
; End of S3MINI.INC file
;------------------------------------------------------------------------------
