;***********************************************************************
;
;	MTDCOMON.INC
;
;***********************************************************************

	.386

;***********************************************************************
;	GLOBAL EQUATES
;***********************************************************************

CPY_BUF_LEN	EQU	128		; MUST be a multiple of 4

MIN_WIN_LEN	EQU	(4 * 1024) 	  ; 4Kb for 32-bit version
MAX_WIN_LEN	EQU	(2 * 1024 * 1024) ; 2Mb for 32-bit version

ErQLen		EQU	4		; # of simultaneous background erases

;***********************************************************************
;       VERSION CONTROL EQUATES
;***********************************************************************

VERSION_NUMBER_HI       EQU     0       ; The version number of the OEM driver
VERSION_NUMBER_LO_hb    EQU     0       ; This should be single digit equates
VERSION_NUMBER_LO       EQU     1
VERSION_NUM_SUB_HB      equ     0
VERSION_NUM_SUB_LB      equ     0

MTD_DESC	EQU	"Intel Series 2"
TECH_TYPE	EQU	"FLASH"

; The system name I chose as ISer2MTD to represent "Intel Series 2 MTD"

VCB_Name	equ	"ISer2MTD"	; Must be 8 characters EXACTLY
VCB_Date	equ	"10/28/93"	; Must be 8 characters EXACTLY
VCB_Date_text	equ	"28 Oct 1993"	; Should match VCB_date

;***********************************************************************
;	CONDITIONAL ASSEMBLY CONTROLS
;***********************************************************************

;  To make internal symbols visible in .MAP and .SYM, set this non-zero:

WANT_INTERNAL_SYMS EQU  1

;  To use "VMMcall _HeapAllocate" to get slot table, set this non-zero:

GET_TABLE_FROM_HEAP EQU 0

if GET_TABLE_FROM_HEAP
else  ;GET_TABLE_FROM_HEAP
    MAX_SOCKETS EQU	16		; maximum number of sockets to allow
endif ;GET_TABLE_FROM_HEAP

;  To get call counts and last call info in MTDIO.ASM, set this non-zero:

WANT_CALL_COUNTS EQU	1

;***********************************************************************
;	DOS CONSTANTS
;***********************************************************************

DOS_GEN_FAIL	EQU	0810CH
DOS_DEV_SUCCESS EQU	00100H
INIT_FUNCTION	EQU	0

;***********************************************************************
;	CARD SERVICES REQUESTS
;***********************************************************************

MTDModifyWindow		EQU	00H
MTDReleaseWindow	EQU	01H
MTDRequestWindow	EQU	02H
MTDSetVpp		EQU	03H
MTDRDYMask		EQU	04H

;***********************************************************************
;	RETURN CODES
;***********************************************************************

STATUS_WRITE_PROTECTED	EQU	1AH

MTD_WaitReq		EQU	00h	; retry when background done
MTD_WaitTimer		EQU	01h	; retry after specified time
MTD_WaitRDY		EQU	02h	; retry when card is ready
MTD_WaitPower		EQU	03h	; retry after power change

READY_CONTINUED		EQU	(1 SHL 4)
TIMEOUT_CONTINUED	EQU	(1 SHL 5)
REQ_CONTINUED		EQU	READY_CONTINUED or TIMEOUT_CONTINUED
CONTINUED_BITS		EQU	READY_CONTINUED or TIMEOUT_CONTINUED

;***********************************************************************
;	OTHER CARD SERVICES EQUATES
;***********************************************************************
NULL		EQU	0		; null pointer

PCMCIA_201	EQU	201h		; PCMCIA Release 2.01
PCMCIA_210	EQU	210h		; PCMCIA Release 2.10
JEDEC_ID_i8Mb	EQU	0A289h		; Intel 8Mb flash
Vcc_0V		EQU	000		;  0 Volts Vcc
Vcc_5V		EQU	050		;  5 Volts Vcc
Vpp_0V		EQU	000		;  0 Volts Vpp
Vpp_5V		EQU	050		;  5 Volts Vpp
Vpp_12V 	EQU	120		; 12 Volts Vpp

;***********************************************************************
;	SCREEN DISPLAY EQUATES
;***********************************************************************

BEL		EQU	07h		; bell
LF		EQU	0Ah		; line feed
CR		EQU	0Dh		; carriage return

;***********************************************************************
;	VOLTAGE CONTROL EQUATES
;***********************************************************************

SET_VPP_LOW	EQU	0
SET_VPP_PROG	EQU	1
SET_VPP_NOW	EQU	(0 SHL 8)
SET_VPP_LATER	EQU	(1 SHL 8)

;***********************************************************************
;	28F008SA COMMAND CODES
;***********************************************************************

ERASE_CMD		EQU	020H
WRITE_CMD		EQU	040H
CLEAR_STATUS_CMD	EQU	050H
READ_STATUS_CMD		EQU	070H
ERASE_SUSPEND_CMD	EQU	0B0H
ERASE_CONFIRM_CMD	EQU	0D0H
ERASE_RESUME_CMD	EQU	0D0H
READ_ARRAY_CMD		EQU	0FFH

BOTH_BYTES		EQU	0101H

;***********************************************************************
; 28F008SA Status Register bits
;***********************************************************************

ER_ST_READY	 EQU	(1 SHL 7)
ER_ST_SUSP	 EQU	(1 SHL 6)
ER_ST_ERASE_ERR	 EQU	(1 SHL 5)
ER_ST_WRITE_ERR	 EQU	(1 SHL 4)
ER_ST_BAD_VPP	 EQU	(1 SHL 3)
ER_ST_ERASE_FAIL EQU	ER_ST_SUSP OR ER_ST_ERASE_ERR OR ER_ST_BAD_VPP

;*************************************************************************
;	28F010 and 28F020 COMMAND CODES
;*************************************************************************

OLD_READ_ARRAY_CMD	EQU	000H
OLD_ERASE_CMD		EQU	020H
OLD_WRITE_CMD		EQU	040H
OLD_ERASE_VERIFY_CMD	EQU	0A0H
OLD_PROG_VERIFY_CMD	EQU	0C0H
OLD_CLEAR_STATUS_CMD	EQU	0FFH



;***********************************************************************
;	CARD SERVICES CALL MACRO
;***********************************************************************

CS_Call	macro	Function, ArgLength
    ifnb <csFunction>
	mov	ax, (0AF00h OR Function)
    else
	%out <CS_Call macro requires a function>
	.err
    endif
    ifnb <ArgLength>
	mov	cx, ArgLength
    else
	%out <Missing arg length in CS_Call macro>
	.err
    endif
	VxDcall PCCARD_Card_Services
	endm

;***********************************************************************
;	INTERNAL LABEL MACRO
;
;   This allows us to distinguish between labels that are required to be
;   global for linking purposes from those that are declared global only
;   for inclusion in the .MAP and .SYM files.
;***********************************************************************

INTERNAL macro	name
    if WANT_INTERNAL_SYMS
	public	name
    endif ; WANT_INTERNAL_SYMS
	endm

;***********************************************************************
;	GLOBAL DATA MACRO
;***********************************************************************

xpub	macro name, type, init
	public	name
name	type	init
	endm

;***********************************************************************
;	INTERNAL DATA MACRO
;***********************************************************************

ipub	macro name, type, init
	INTERNAL        name
name	type	init
	endm



;***********************************************************************
;	CARD SERVICES CALL STRUCTURES
;***********************************************************************


;***********************************************************************
;	CARD INFO STRUCTURE
;***********************************************************************

SocketData_s	struc
skt_socket	dw	?
skt_client_h	dw	?
skt_cardoff	dd	?
skt_devnode	dd	?
skt_flags	dd	?
skt_cur_map	dd	?
skt_win_handle	dw	?
skt_status	dw	?
skt_win_base	dd	?
skt_win_end	dd	?
skt_win_mmask	dd	?		; to get window map
skt_win_omask	dd	?		; to get window offset
skt_win_size	dd	?
skt_CurVpp	db	?
skt_LowVpp	db	?
skt_in_progress db	?
		db	?		; spare
skt_busy_blks	dw	?		; blk numbers of in progress erases
		dw      (ErQLen-1) dup(?) ; (more blk numbers)
SocketData_s	ends

; skt_flags definitions

SDF_REG_IN_PROG     EQU     1		; client registration is in progress
SDF_DO_EXIT	    EQU     2		; indicates exit is pending

;***********************************************************************
;	MTD REQUEST PACKET FROM CARD SERVICES
;***********************************************************************

MTDRequest_Packet   struc
PacketLength	    dw	    ?
Socket		    dw	    ?
SrcOffset	    dd	    ?
DestOffset	    dd	    ?
XferLength	    dd	    ?
Function	    db	    ?
AccessSpeed	    db	    ?
MTD_odd_byte	    db	    ?		; "MTDID DW..." in specification
MTD_suspend_flag    db	    ?
MTDStatus	    dw	    ?
TimeoutCount	    dw	    ?
	; MAT [19 DWORDS] goes here, but we're not interested
MTD_buffer	    dd	    ?
		    dd	18 dup(?)	; more MAT...
HelperPtr	    dd	    ?
MTDRequest_packet   ends

;***********************************************************************
;       Initialization Reference Data Packet
;***********************************************************************

MTDInitPacket	struc
Socket		dw	?
CurVpp		db	?
LowVpp		db	?
MTDInitPacket	ends

IFDEF NEC_98
NECSocketData	struc
skt_flag	dw	MAX_SOCKETS dup(0)
win_handle	dw	MAX_SOCKETS dup(0)
win_base	dd	MAX_SOCKETS dup(0)
win_size	dd	MAX_SOCKETS dup(0)
NECSocketData	ends
ENDIF