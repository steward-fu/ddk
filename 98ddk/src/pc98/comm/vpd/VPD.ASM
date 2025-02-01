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

PAGE 58,132
;******************************************************************************
TITLE VPD.ASM - Virtual Printer Device
;******************************************************************************
;
;   Title:	VPD.ASM - Virtual Printer Device
;
;==============================================================================
;
;   DESCRIPTION:
;
;	This device virtualizes the parallel printer ports (up to LPT3).  It's
;	only function is to prevent two virtual machines from accessing the
;	same printer at the same time.	If more that one VM attempts to use
;	the same printer then a dialog box will be presented to the user
;	allowing him/her to select which virtual machine should be allowed to
;	use the printer.  Network printer contention is only detectable when
;	an application prints using the BIOS Int 17h service.
;
;	If a special switch is set in the system initialization file then
;	VPD provides special support for a Microsoft debugging program called
;	"SGRAB".  Sgrab is a utility used to grab screen shots from a
;	computer through a parallel printer port.
;
;	VPD virtualizes the printer at three levels:
;	    I/O ports
;	    BIOS service interrupt 17h
;	    Hardware interrupt requset 7 (IRQ 7 is ususally interrupt 0Fh)
;
;
;   PRINTER I/O PORTS:
;
;	PC parallel ports each have three byte-wide I/O ports:	Data, Status,
;	and Control.  An interesting note is that the physical address of the
;	I/O ports for a given LPT is not fixed.  If there is a monochrome
;	display adaptor type LPT port then LPT1's I/O ports will start with
;	port 3BCh. Compaq 386 machines have LPT port at this I/O address on
;	the mutipurpose fixed disk controller board.  On other machines LPT1
;	may be located at port 278h-27Ah.
;
;	There are four word values in the BIOS data area from 0040:0008 to
;	0040:000D which specify the base I/O port of the LPT hardware.
;	The word at 0040:0008 specifies the base I/O port for LPT1, the
;	word at 0040:000A specifies the base I/O port for LPT2, etc.
;
;	To print a byte of data through a physical LPT port a virtual machine
;	must do the following:
;
;	    1.	The VM checks the printer status port.	If the busy, paper out
;		or printer fault signals are active, the VM either waits
;		until the status changes or it shows an error message.
;	    2.	The VM sends a byte of data to the printer data port, then
;		pulses the printer strobe signal (through the printer control
;		port) for 500 ns (or more).
;	    3.	The VM then monitors the printer status port for acknowledge-
;		ment of the data byte before sending the next byte.
;
;	DATA PORT (offset 0 from base):
;	    This is a read-write port that is used to latch a byte of data
;	    that is about to be sent to the printer.  Note that this port
;	    CAN be used to receive data on most machines so it is valid for
;	    a VM to read this port.
;
;	STATUS PORT (offset 1 from base):
;	    This is a read-only port that returns the current printer status.
;
;		BIT MASK
;		-----XXX  000 (Reserved)
;		----X---  0 = Printer error
;		---X----  1 = Printer selected (on line)
;		--X-----  1 = Out of paper
;		-X------  0 = Printer acknowledges correct receipt of data
;		X-------  0 = Printer busy
;
;	CONTROL PORT (offset 2 from base):
;	    This port selects the printer for output, strobes the data into
;	    the printer, and performs other printer control functions.
;
;		BIT MASK
;		-------X  1 = Printer strobe
;		------X-  1 = Printer auto line feed
;		-----X--  0 = Initialize printer
;		----X---  1 = Printer select
;		---X----  1 = Enable hardware interrupt
;		XXX-----  000 (Reserved)
;
;
;   BIOS SERVICE INTERRUPT 17h:
;
;	The IBM system BIOS provides a limited set of services for sending
;	data to a printer and reading its status.  The main purpose of hooking
;	Int 17h is to detect network printer contention.  Since printing
;	to a network printer will not access a hardware printer port, we
;	hook the BIOS service so we can raise contention on applications that
;	use Int 17h.  The three services provided by the system BIOS are:
;
;	    AH = 00h - Print Character
;		 AL = Character to print
;		 DX = Printer to use (0=LPT1, 1=LPT2, etc.)
;		 Returns:
;		    AH = Status
;		       Bit 7 = 1 - Not busy
;		       Bit 6 = 1 - Acknowledge
;		       Bit 5 = 1 - Out of paper
;		       Bit 4 = 1 - Selected
;		       Bit 3 = 1 - I/O error
;		       Bits 2, 1 - Reserved
;		       Bit 0 = 1 - Time-out
;
;	    AH = 01h - Initialize the Printer Port
;		 DX = Printer to use (0=LPT1, 1=LPT2, etc.)
;		 Returns:
;		    AH = Status
;		       Bit 7 = 1 - Not busy
;		       Bit 6 = 1 - Acknowledge
;		       Bit 5 = 1 - Out of paper
;		       Bit 4 = 1 - Selected
;		       Bit 3 = 1 - I/O error
;		       Bits 2, 1 - Reserved
;		       Bit 0 = 1 - Time-out
;
;	    AH = 02h - Read Status
;		 DX = Printer to use (0=LPT1, 1=LPT2, etc.)
;		 Returns:
;		    AH = Status
;		       Bit 7 = 1 - Not busy
;		       Bit 6 = 1 - Acknowledge
;		       Bit 5 = 1 - Out of paper
;		       Bit 4 = 1 - Selected
;		       Bit 3 = 1 - I/O error
;		       Bits 2, 1 - Reserved
;		       Bit 0 = 1 - Time-out
;
;	    AH = 03h to FFh - Reserved
;
;	For each of these services, if the current VM can not access the
;	printer, we will return a status with the time-out and out of paper
;	bits set.  If a VM can access the printer then the Int 17h is
;	reflected to the virtual machine and is handled normally by the
;	BIOS or the network software.
;
;
;   HARDWARE IRQ 7:
;
;	Most programs never use the printer hardware interrupt because it
;	is unreliable on many computers.  Therefore, VPD only virtualizes
;	the IRQ if Sgrab support is enabled.  If Sgrab is NOT enabled then
;	VPD will not virtualize the interrupt, thus leaving the default
;	interrupt handling to VPICD.  If Sgrab is enabled then all printer
;	interrupts are reflected to the virtual machine that owns the
;	display.
;
;	The VPD hardware interrupt handlers are simple.  VPD only pays
;	attention to hardware interrupts (VPD_Hw_Int) and the virtual
;	End Of Interrupt (VPD_Virt_EOI).
;
;	The hardware interrupt procedure requests an interrupt for the
;	current display owner.	When the display owner VM services the
;	interrupt and executes an EOI sequence, VPD_Virt_EOI clears the
;	virtual interrupt request and calls VPICD_Phys_EOI to end the actual
;	hardware interrupt.
;
;
;   SGRAB SUPPORT:
;
;	This VxD also contains support for a Microsoft program called
;	"Sgrab" that is used to grab the current display contents for
;	testing purposes.  Sgrab uses an LPT port and the printer hardware
;	interrupt to perform the screen grab.  The interrupt is used to
;	indicate that a screen grab should be performed and the data is
;	transmitted to a second computer through the LPT I/O ports.
;
;	The user can indicate that he is using Sgrab by including the line:
;	    SgrabLPT=x (where x is a number from 1 to 3)
;	in the system initialization file under the [Win386] section.  This
;	disables trapping of the physical I/O ports and causes VPD to send
;	all printer hardware interrupts to the current display owner.  VPD
;	keeps track of which VM owns the display by watching all "Device_-
;	Set_Focus" control calls that change the ownership of the Virtual
;	Display Device (VDD).
;
;
;   PROTECTED MODE API:
;
;	VPD supports several API calls that can only be made by programs
;	running in protected mode.  These calls are primarily designed for
;	use by the Windows control pannel and print spooler.  The services
;	provided allow Windows to:
;
;	    Get the VPD version
;	    Get information about which printer ports are valid
;	    Get the contention behavior for a specified port
;	    Set the contention behavior for a specified port
;	    Aquire ownership of a specified port
;	    Release ownership of a specified port
;
;******************************************************************************

	.386p

;******************************************************************************
;***	All WPS changes are enclosed in "if WPS"
WPS	EQU	1
;******************************************************************************


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************
VPD_Dynamic     EQU     1	; need-work, test only code

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE VPICD.inc
	INCLUDE SHELL.Inc
	INCLUDE DOSMGR.Inc
	INCLUDE	VCOMM.INC
	INCLUDE	OPTTEST.INC
ifdef NEC_98
	INCLUDE VTD.INC
	INCLUDE VPPID.INC
	INCLUDE TOKI.INC
endif ;NEC_98
	.LIST

;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VPD, 3, 0, VPD_Control, VPD_Device_ID, \
		       VPD_Init_Order+20, , VPD_PM_API_Handler


;******************************************************************************
;				 E Q U A T E S
;******************************************************************************

IFDEF NEC_98
VPD_Max_Virt_LPTs	EQU	1		; Max of 3 LPT ports
VPD_BIOS_Timeout_Addr	EQU	458h		; BIOS_FLAG5 PRT I/F mode bit
VPD_BIOS_Timeout_Size	EQU	1		; add H98
VPD_Num_Ports_Per_LPT	EQU	4		;
ELSE ;NEC_98
VPD_Max_Virt_LPTs	EQU	3		; Max of 3 LPT ports
VPD_BIOS_Base_Addr	EQU	408h		; BIOS data for I/O base addr
VPD_BIOS_Timeout_Addr	EQU	478h		; BIOS data for LPT timeouts
VPD_BIOS_Timeout_Size	EQU	VPD_Max_Virt_LPTs
VPD_IRQ 		EQU	7		; IRQ 7 is the LPT interrupt
VPD_Num_Ports_Per_LPT	EQU	3		; Number of I/O ports per LPT
ENDIF ;NEC_98
VPD_Default_Timeout	EQU	60000		; 60 seconds

IFDEF NEC_98
VPD_IRQ			EQU	14		; 
VPD_LPT1_IF_Mode_Bit	EQU	7		; CB_BLOCK Bit6 = LPT1 I/F mode info.
VPD_LPT2_IF_Mode_Bit	EQU	8		; CB_BLOCK Bit7 = LPT2 I/F mode info.
VPD_LPT3_IF_Mode_Bit	EQU	9		; CB_BLOCK Bit8 = LPT3 I/F mode info.
VPD_PORT_REG_Bit	EQU	6		; CB_BLOCK Bit5 = Port Reg mode info.
VPD_PORT_INFO_Bit3	EQU	4		; CB_BLOCK Bit3 = Default Port info.
VPD_PORT_INFO_Bit4	EQU	5		; CB_BLOCK Bit4 = Default Port info.
VPD_INT5_Processing_Bit	EQU	31		; Int 5 processing bit
VPD_INT5_Processing	EQU	(1 SHL VPD_INT5_Processing_Bit)

VPD_BIOS_BI_Addr	EQU	5B3h		; PRT_IFMODE
						;    =5B3h
VPD_BIOS_BI_Size	EQU	1		; 

VPD_IF_SEMI_BIT		EQU	4		; PC-98 SEMI Cen. Bit
VPD_IF_PS2_BIT		EQU	5		; PS2 MODE Bit

VPD_ready_Status_AT	EQU	0DAh 		; AT and PS2 Mode Ready Status
VPD_BIOS_NFull_Status	EQU	006Dh		; Ready Status Full Cen
VPD_BIOS_NSemi_Status	EQU	01h		; Ready Status Semi Cen
VPD_TOKI_Kill_Status	EQU	0FFh		; TOKI Kill Status
VPD_BI_Control_Status	EQU	04h		; Printer Control
VPD_BI_TOKI_Status_N	EQU	20h		; TOKI Status (Default)
VPD_BI_MASK_AT		EQU	0EFh		; MASK AT Bit
VPD_BI_SET_AT		EQU	10h		; Set AT Bit
VPD_BI_EX_ST_DEFAULT	EQU	01h		; EX. TOKI Status (Default)
VPD_BI_PS2_BIT		EQU	20h		; EX. TOKI's PS2 Mode Bit
VPD_BI_EX_PS2_BIT	EQU	60h		; EX. TOKI's Hipp Mode Bit

TOKI_PS2DT		EQU	0140h		; TOKI Data Port
TOKI_PRSTS		EQU	0141h		; TOKI Printer Status Port
TOKI_PRCTR		EQU	0142h		; TOKI Control Port
	TOKI_INIon	EQU	  00h		; Initialize Setting On
	TOKI_INIoff	EQU	  04h		; Initialize Setting Off
	TOKI_STRBon	EQU	  05h		; STROBE On
	TOKI_STRBoff	EQU	  04h		; STROBE Off
	TOKI_CTRMSK	EQU	  04h		; Contorl MASK For Mode Change
TOKI_TKSTS		EQU	0149h		; TOKI Status Port
	TOKI_ON		EQU	  10h		; TOKI Mode
	TOKI_OFF	EQU	  00h		; 98 Mode
TOKI_TKCTR		EQU	014Bh		; TOKI Status
TOKI_HIFO		EQU	014Ch		; HIFO Data
TOKI_EXTRA		EQU	014Eh		; TOKI Printer Extended Contorl
	TOKI_AT		EQU	  00h		; TOKI AT Mode
	TOKI_PS2	EQU	  20h		; TOKI PS2 Mode

ENDIF ;NEC_98

if	WPS
VJPD_Hide_Time          EQU     4000            ; Time to not check port
VJPD_Switch_Wait_Time   EQU     56000           ; Max mode switch takes 26 sec
endif ; WPS

;
;   Status byte to return if printer is being used by another VM.
;
IFNDEF NEC_98
VPD_Busy_Status 	EQU	10111000b
ELSE ;NEC_98
VPD_Busy_Status		EQU	10111010b ; 0xBA = From busy
VPD_ready_Status_Semi	EQU	00000100b ; H98
VPD_ready_Status_Full	EQU	01100101b ; H98
ENDIF ;NEC_98

if	WPS
;
;   Status register values		  | "ready" value
;					  v
;				BIT MASK
;				X-------  1 = Not Busy
;				-X------  1 = No Ack data
;				--X-----  0 = No Paper Error
;				---X----  1 = Selected (on line)
;				----X---  1 = No Fault
;				-----X--  0 (Reserved)
;				------X-  0 (Reserved)
;				-------X  0 (Reserved)
VJPD_Not_Busy		EQU	10000000b
VJPD_Not_Ack		EQU	01000000b
VJPD_Paper_Err		EQU	00100000b
VJPD_Selected		EQU	00010000b
VJPD_Not_Fault		EQU	00001000b
VJPD_Reserved		EQU	00000111b
;
; to see if printer is in PCL ready mode we check that the bits specified
; by VJPD_Status_Ready_Mask are set to the values in VJPD_Status_Ready
VJPD_Status_Ready_Mask  EQU     10111000b
VJPD_Status_Ready       EQU     10011000b
ifdef NEC_98
VJPD_Status_Ready_Mask_Semi	EQU	00000100b
VJPD_Status_Ready_Semi		EQU	00000100b
VJPD_Status_Ready_Mask_Full	EQU	11100100b
VJPD_Status_Ready_Full		EQU	01100100b
endif ;NEC_98
;
;   Control byte to set during Mode Switch message out
;
VJPD_Data_Strobe	EQU	000000001b
;
;   Mask reserved bits in control register
;
VJPD_Control_Mask       EQU     00011111b
endif ; WPS

if	WPS
;
;******************************************************************************
;                                M A C R O S
;******************************************************************************

;
;   Output a byte with 2 pre-fetch flushes (JMP $+2) to delay for I/O Bus
;
OutP MACRO Port, Value
        out     Port, Value
        jmp     $+2
        jmp     $+2
ENDM OutP
endif ; WPS

;******************************************************************************
;			     S T R U C T U R E S
;******************************************************************************

VPD_Struc STRUC
VPD_LPT_Number		dd	?		; Printer port number (1-3)
VPD_Timeout_Limit	dd	?		; Ownership time-out limit
VPD_Proc_Addr		dd	?		; Address of I/O trap routine
VPD_Name		db	"LPTx    "	; Printer port name "LPTx"
VPD_IO_Base		dd	0		; Base I/O port (0=not phys)
VPD_Last_Touched	dd	?		; Time LPT was last accessed
VPD_Owner		dd	0		; VM handle of printer owner
VPD_Flags		dd	0		; Flags
VPD_NotifyProc		dd	0		; procedure to notify for
VPD_NotifyRefData	dd	0		; loss of port and its refdata
VPD_Alt_NotifyProc	dd	0		; alternate owner procedure
VPD_Alt_NotifyRefData	dd	0		; alternate reference data

if	WPS
VJPD_Non_WPS_Mode       	dd      0      	; Address of non-WPS mode switch
VJPD_Non_WPS_Mode_Size  	dd      0      	; Size of non-WPS mode switch
VJPD_Send_Switch_Timeout	dd	0	; Mode Switch Timeout time
endif ; WPS

VPD_Struc ENDS


;******************************************************************************
;			   F L A G   E Q U A T E S
;******************************************************************************

VPDF_Special_Owner	EQU	00000001b
VPDF_Special_Owner_Bit	EQU	00h
VPDF_Is_Connected	EQU	00000010b
VPDF_Is_Connected_Bit	EQU	01h
VPDF_AltOwnerActive	EQU	00000100b
VPDF_AltOwnerActiveBit	EQU	02h

if	WPS
VJPDF_Jumbo             EQU     00001000b	; owned (or last owned) by WPS
VJPDF_Jumbo_Bit         EQU     03h
VJPDF_NONJumbo          EQU     00010000b	; Non Jumbo WPS
VJPDF_NONJumbo_Bit      EQU     04h
VJPDF_PCLSwitch		EQU	00100000b	; mode switch in progress
VJPDF_PCLSwitch_Bit	EQU	05h
VJPDF_JumboAcq		EQU     01000000b	; Acquire request by Jumbo
VJPDF_JumboAcq_Bit	EQU     06h
endif ; WPS

VPDF_Enabled		EQU	10000000b
VPDF_EnabledBit		EQU	07h

;******************************************************************************
;		   I N I T I A L I Z A T I O N	 D A T A
;******************************************************************************

VxD_IDATA_SEG

;
;   IRQ descriptor used to virtualize the LPT IRQ (7)
;
VPD_IRQ_Descriptor VPICD_IRQ_Descriptor <VPD_IRQ,,OFFSET32 VPD_Hw_Int,,\
					 OFFSET32 VPD_Virt_EOI>

;
;   Instance data structures 40:78-40:7B
;
VPD_Inst_Timeout   InstDataStruc <,,VPD_BIOS_Timeout_Addr,VPD_BIOS_Timeout_Size,ALWAYS_Field>

IFDEF NEC_98
VPD_BI_IF_Mode	InstDataStruc <,,VPD_BIOS_BI_Addr,VPD_BIOS_BI_Size,ALWAYS_Field>
; PRTDRV.SYS I/F mode data area
VPD_DRV_IF_Mode	InstDataStruc <,,,1,ALWAYS_Field>
ENDIF ;NEC_98

BeginMsg
;
;   String used to look for "SgrabLPT=x" entry in SYSTEM.INI file
;
Sgrab_Ini_String db "SGRABLPT", 0

;
;   String used to get "LPTxAUTOASSIGN" SYSTEM.INI entry values.
;
VPD_Auto_Assign_Str db "LPT"
VPD_Auto_Assign_Num db "x"
		    db "AUTOASSIGN", 0

;
;   String used to get "LPTxDisable" SYSTEM.INI entry values.
;
VPD_Disable_Str	db	"LPT"
VPD_Disable_Num	db	"x"
		db	"DISABLE", 0

if	WPS
;
;   String used to get PCL Mode switch timeout SYSTEM.INI [386enh] entry value.
;
VJPD_Switch_Timeout_Key db "LPTSwitchTimeout", 0
;
;   String used to get Jumbo LPT Flags from SYSTEM.INI [386enh] entries.
;
VJPD_Jumbo_LPT          db "WPSLPT"
VJPD_Jumbo_LPT_Num      db "x", 0
VJPD_NONJumbo_LPT       db "NONJUMBOLPT"
VJPD_NONJumbo_LPT_Num   db "x", 0
endif ; WPS

IFDEF NEC_98
VPD_INT1A_String    db "INT1ACRITICAL", 0		;
ENDIF ;NEC_98

EndMsg

BeginDoc
;
;   String used to instance the DOS PRN: device.
;
VPD_PRN_Device_Name db "PRN     "
;
;   String used to instance IBM EPT device driver
;
;;;VPD_EPT_Device_Name db "EPT     "
EndDoc


VxD_IDATA_ENDS


;******************************************************************************
;	      R E A L	M O D E   I N I T I A L I Z A T I O N
;******************************************************************************

VxD_REAL_INIT_SEG

;
;   Data for real mode code is in the same segment.  This is the device
;   driver's name for the EPT port.
;
;;;EPT_Device_Name db "EPT", 0


;******************************************************************************
;
;   VPD_Real_Mode_Init
;
;   DESCRIPTION:
;	An IBM postscript printer uses a device driver named "EPT" that
;	must be instanced.
;
;   ENTRY:
;	CS, DS = Real mode segment
;	BX = Flags
;
;   EXIT:
;	EDX = Reference data to pass to protected mode portion of VxD
;
;   USES:
;	AX, BX, DX, SI, Flags
;
;==============================================================================

BeginProc VPD_Real_Mode_Init

;
;   If another printer VxD is loaded then don't load -- Just abort our load
;
	test	bx, Duplicate_From_INT2F OR Duplicate_Device_ID
ifdef NEC_98
	jnz	VPD_RI_Abort_Load
else ;NEC_98
	jnz	SHORT VPD_RI_Abort_Load
endif ;NEC_98


;
;   Try to open a file named "EPT" (hopefully this will be the EPT device)
;
;;;	mov	dx, OFFSET EPT_Device_Name	; DS:DX -> "EPT", 0
;;;	mov	ax, 3D00h			; Open file
;;;	int	21h				; Q: Can Mr. DOS find the file?
;;;	jc	SHORT VPD_RMI_No_EPT		;    N: Done!
						;    Y: Make sure it's device
;
;   The file open worked.  Now make sure it's a device.
;
;;;	xor	dx, dx				; 0 in case it fails
;;;	mov	bx, ax				; BX = File handle
;;;	mov	ax, 4400h			; Get device info IOCTL
;;;	int	21h				; Call Mr. Operating System
	; Ignore any error!			; (returns result in DX)

;;;	mov	ah, 3Eh
;;;	int	21h				; Close the file

;;;	test	dx, 10000000b			; Q: Is this a device
;;;	jnz	SHORT VPD_RMI_Exit		;    Y: EDX != 0 on init call

VPD_RMI_No_EPT:
	xor	edx, edx			; EDX = 0 to indicate no EPT
IFDEF NEC_98
	push	es
	push	ax
	xor	ax,ax
	mov	es,ax

	NOP
	NOP
	mov	dh, byte ptr es:[5B3h]	; PRT_IFMODE

	test	dh,80h			; Check Bi-Direction Support
	jz	SHORT CABLE_NOT14	; Not Bi-Direction

					;
	push	dx			; Save DX Reg.
	mov	ax,1e00h		; Check PRTDRV.SYS
	int	1ah			;
	pop	dx			; Restore DX Reg.

	test	ah,80h			; Q: Exist PRTDRV.SYS?
	jz	SHORT CABLE_CHECK	;  No. go 14Pin cable check

	push	ax			; Save AX.Reg
	test	ah,01h			; Q: Enable DX Reg.?
	jnz	SHORT CABLE_CHECK_DX_ON	;  Yes.

	push	dx			; Save DX
	mov	ax,1e01h		; Enable DX Reg.
	mov	dh,01h			;
	int	1ah			;
	pop	dx			; Restore DX

CABLE_CHECK_DX_ON:
	push	dx			; Save DX
	xor	dx,dx			; Set LPT1
	mov	ax,1800h		; Get Full Status
	int	1ah
	pop	dx			; Restore DX

	cmp	ah,06h			; 14Pin?
	jne	SHORT Ret_Default	; No

	or	dh,01h			; Bit8 On
Ret_Default:
	pop	ax			; Restore AX(L551)
	push	dx			; Save DX

	mov	dh,ah			; DL<-81h or 80h
	and	dh,7fh			; DL<-01h or 00h

	mov	ax,1e01h		; Set boot mode
	int	1ah

	pop	dx			; Restore Reg.
	jmp	SHORT CABLE_NOT14	; End Check

CABLE_CHECK:
	mov	ah,18h			; Get Full Cen. Status
	int	1ah
	cmp	ah,06h			; 14Pin Cable ?
	jne	SHORT CABLE_NOT14	; NO ,36Pin Cable then Jump
	or	dh,01h			; Bit08 ON
CABLE_NOT14:
	mov	dl, byte ptr es:[458h]

	push	dx			; Save DX
					;
	mov	ax,1e00h		; Check PRTDRV.SYS
	int	1ah			;
	pop	dx			; Restore DX

	test	ah,80h			; Q: Exist PRTDRV.SYS?
	jz	SHORT check_prtdrv_end	;  No, jump check_prtdrv_end

;;;If we use Int 1ah functions from protect mode, we don't need the following code
	push	dx			; Save DX
					;
	mov	ax,1effh		; PRTDRV.SYS
	int	1ah			;

	mov	dx,ax			; DX <- AX=PRTDRV.SYS data seg.
	shl	edx,16			; shift 16Bit
					;
	pop	dx			;

	or	dx,0000010000000000b	; Bit10 ON
check_prtdrv_end:
	push	dx			; Save DX
	mov	dx,0549h		;
	in	al,dx			; Get TOKI Status
	cmp	al,0ffh			; Q: Exist Printer Board?
	pop	dx			; Restore DX
	je	SHORT VPD_R_not_board	;  No.JUMP

	or	dx,0000100000000000b	; Bit11 ON

	push	dx			; Save DX
	mov	dx,054fh		; 'Where is the strange printer cable?'
	in	al,dx			;
	test	al,04h			; Q: Max port = 3?
	pop	dx			; Restore DX
	jnz	SHORT VPD_R_not_board	;  Yes.

	or	dx,0000001000000000b	;  No. Bit09 ON

	push	dx
	mov	dx,0439h
	in	al,dx
	test	al,80h
	jnz	SHORT VPD_R_439h_END

	or	al,80h			;
	out	dx,al			; Disable Printer KILL

VPD_R_439h_END:
	pop	dx

VPD_R_not_board:

	pop	ax
	pop	es
ENDIF ;NEC_98

VPD_RMI_Exit:
	xor	bx, bx
	xor	si, si
	mov	ax, Device_Load_Ok
	ret

;
;   Another VPD device has already been loaded so we won't.
;
VPD_RI_Abort_Load:
	xor	bx, bx
	xor	si, si
	mov	ax, Abort_Device_Load + No_Fail_Message
	ret

EndProc VPD_Real_Mode_Init

VxD_REAL_INIT_ENDS


;******************************************************************************
;			    L O C A L	D A T A
;******************************************************************************

VxD_DATA_SEG

VPD_CB_Offset	dd	?

IFDEF NEC_98
VPD_CB_EBX	dd	?	;
VPD_458_BYTE	db	?	; add H98
VPD_DRV_Seg	dw	?
VPD_DRV_Off	dw	?
VPD_BD_DRV_BYTE	db	?

VPD_5B3_BYTE	db	?	; add 98 Full Cen.
VPD_KILL_VM	db	0	;

VPD_BIBD_BYTE	db	0	;
VPD_BI_PORT	db	1	;
VPD_SYS_CB_DATA	dd	0	;
VPD_ACCESS_PORT dd	?	; Current Access Port

VPD_INT1A_Critical      db  0	; Critical section flag
SYSINI_Set_Critical	dd  0	; "INT1ACritical=?"

VJPD_Max_Virt_LPTs      dd     1         ; Max of 1 LPT ports
VJPD_BIOS_Base_Addr	dd	AT_DATA2
			dd	AT_DATA3
Status_Op_Board		dd	0	;Option Board Status[54E,549,D4E,D49]
TOKI_Low_Addr_Table	db	Low_AT_DATA		;Dummy(Not Use)
			db	Low_AT_DATA
			db	Low_AT_STATUS
			db	Low_AT_CONTROL
			db	Low_TOKI_CONTROL
			db	Low_TOKI_STATUS
			db	Low_FIFO_DATA
			db	Low_EX_CONTROL
Num_TOKI_Ports_Per_LPT1	equ	7
Num_TOKI_Ports_Per_LPTx	equ	3
VPPIDReg		dd	0	; VPPID Ret Val
ATRes			dd	109703		;Dummy(2.0MHz)
AdjustATTick		dd	0
ifdef MAXDEBUG
public VJPD_Max_Virt_LPTs
public VJPD_BIOS_Base_Addr
endif ;MAXDEBUG
ENDIF ;NEC_98

;
;   Printer data structures.
;   THESE MUST REMAIN IN ORDER FROM LPT1 --> LPTn
;
IFDEF NEC_98
VPD_LPT1 VPD_Struc <1, VPD_Default_Timeout, OFFSET32 VPD_LPT1_Trap,"LPT1",AT_DATA1>
VPD_LPT2 VPD_Struc <2, VPD_Default_Timeout, OFFSET32 VPD_LPT2_Trap,"LPT2",AT_DATA2>
VPD_LPT3 VPD_Struc <3, VPD_Default_Timeout, OFFSET32 VPD_LPT3_Trap,"LPT3",AT_DATA3>
ELSE ;NEC_98
VPD_LPT1 VPD_Struc <1, VPD_Default_Timeout, OFFSET32 VPD_LPT1_Trap,"LPT1">
VPD_LPT2 VPD_Struc <2, VPD_Default_Timeout, OFFSET32 VPD_LPT2_Trap,"LPT2">
VPD_LPT3 VPD_Struc <3, VPD_Default_Timeout, OFFSET32 VPD_LPT3_Trap,"LPT3">
ENDIF ;NEC_98

if	WPS
;
;   Mode Command char defintions
;
ESCAPE	EQU	01Bh
FM	EQU	01Bh
QM	EQU	082h
;
;   Jumbo Printer PCL Mode Switch Command String
;
VJPD_Jumbo_PCL_Mode	db	(8) dup (0)
			db	ESCAPE, "*c16384D"
			db	ESCAPE, ")s39W"
			db	FM, FM, FM, FM, QM
			dw	-1
			dw	01800h
			dd	-1
			dw	00600h, 00800h
			dd	0, 0, 0, 0
			db	033h, 003h, 01Dh, 0E5h, FM, FM
VJPD_Jumbo_PCL_Mode_Size EQU $ - VJPD_Jumbo_PCL_Mode

VJPD_Send_Switch_Limit	dd	4000		; Timeout on sending mode switch
VJPD_Null_Size		EQU	128	 	; nulls to send after a mode switch

endif ; WPS

VxD_DATA_ENDS


VxD_LOCKED_DATA_SEG
;
;   Data for SGRAB support.  The VPD_Sgrab_LPT variable will contain a pointer
;   to the appropriate LPT data structure if SGRAB is enabled.	VPD_Hw_Int_
;   Owner will contain the handle of the virtual machine that currently owns
;   the display.
;
VPD_Sgrab_LPT		dd	0	    ; Turn off contention for this LPT
VPD_Hw_Int_Owner	dd	0	    ; Owner of next hardware interrupt

VxD_LOCKED_DATA_ENDS



;******************************************************************************
;	       D E V I C E   C O N T R O L   P R O C E D U R E
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   VPD_Control
;
;   DESCRIPTION:
;	This is the Virtual Printer Device's control procedure.
;
;   ENTRY:
;	EAX = Control call ID
;
;   EXIT:
;	If carry clear then
;	    Successful
;	else
;	    Control call failed
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc VPD_Control

	Control_Dispatch Device_Init, VPD_Device_Init
	Control_Dispatch Init_Complete, VPD_Init_Complete
	Control_Dispatch Set_Device_Focus, <VPD_Set_Focus>
	Control_Dispatch VM_Not_Executeable, <VPD_VM_Not_Executeable>
	Control_Dispatch GET_CONTENTION_HANDLER, VPD_Get_Contention_Handler
IFDEF NEC_98
	Control_Dispatch VM_Critical_Init, <VPD_VM_Critical_Init>
ENDIF ;NEC_98

if	WPS
        Control_Dispatch System_Exit, <VJPD_System_Exit>
endif ; WPS

IFDEF DEBUG
	Control_Dispatch Debug_Query, VPD_Debug_Query
ENDIF

	clc					; Ignore other control calls
	ret

EndProc VPD_Control

VxD_CODE_ENDS


;******************************************************************************
;		    I N I T I A L I Z A T I O N   C O D E
;******************************************************************************

; VxD_ICODE_SEG   need-work, tmp workaround
VxD_CODE_SEG

;******************************************************************************
;
;   VPD_Device_Init
;
;   DESCRIPTION:
;	This procedure searches the system BIOS data area for any printers.
;	If there are no printers then this procedure will return with carry
;	set to indicate that the VPD should never be called again.  If it
;	does locate one or more printers it will hook the appropriate I/O
;	ports and software interrupt 17h.  It will also instance the
;	printer time-out values.  This allows every virtual machine to set
;	a different length time-out without effecting other VM's states.
;
;	If a "SGRABLPT=x" entry exists in the system initialization file
;	then this procedure will also virtualize the LPT interrupt (IRQ 7).
;
;   ENTRY:
;	EBX = System VM handle
;
;   EXIT:
;	If carry flag set then
;	    VPD should never be called again (no printers)
;	else
;	    Printers present in system
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc VPD_Device_Init

;
;   Try to find the "SGRABLPT=x" entry in the System.Ini file.	If it is not
;   found then leave VPD_Sgrab_LPT to 0 (default value), otherwise set it
;   to the value returned from Get_Profile_Decimal_Int.
;
ifdef NEC_98
	push	edx				; Save EDX Reg.
	push	eax

	xor	eax,eax				;

	test	dx,0000100000000000b		; Q: Exist Printer board?
	jz	SHORT VPD_PINI_BIBD_NO_USE	;  No.
	or	ah,00000001b			;  Yes. Set Bit(00)

	test	dx,0000010000000000b		; Q: Exist PRTDRV.SYS?
	jz	SHORT VPD_PINI_DRV_NO_USE	;  No.
	or	ah,00000010b			;  Yes. Set Bit(01)

	push	edx
	shr	edx,16				; Shift 16Bit
	mov	[VPD_DRV_Seg],dx		; Save PRTDRV.SYS data seg.
	pop	edx

VPD_PINI_DRV_NO_USE:
	mov	al,3				; Max port num = 3
	test	dx,0000001000000000b		; Q: Max port = 3?
	jz	SHORT VPD_BIBD_CHECK_END	;  Yes.
	mov	al,2				;  No. Max port num = 2
	jmp	SHORT VPD_BIBD_CHECK_END	;

VPD_PINI_BIBD_NO_USE:
	mov	al,1				; Max port num = 1
	mov	ah,0				;

VPD_BIBD_CHECK_END:
	mov	[VPD_BI_PORT],al		; Save max port num
	mov	[VPD_BIBD_BYTE],ah		; Save printer info
	pop	eax
	pop	edx

	mov	edx, LPT2_Base+Low_TOKI_CONTROL
	in	al, dx
	cmp	al, 0FFh				;Option Board Support?
	je	SHORT No_Board				;No.
	mov	ecx, 8
	mov	edx, LPT2_Base+Low_EX_CONTROL
	in	al, dx					;Read [54Eh]
	shl	eax, cl					;eax=[0,0,54E,0]
	mov	edx, LPT2_Base+Low_TOKI_CONTROL
	in	al, dx					;Read [549h]
	shl	eax, cl					;eax=[0,54E,549,0]
	mov	edx, LPT3_Base+Low_EX_CONTROL
	in	al, dx					;Read [D4Eh]
	shl	eax, cl					;eax=[54E,549,D4E,0]
	mov	edx, LPT3_Base+Low_TOKI_CONTROL
	in	al, dx					;Read [D49h]
	mov	[Status_Op_Board], eax			;eax=[54E,549,D4E,D49]

	mov	edx, TOKI_Int_Ch_Read
	in	al, dx
	mov	[VJPD_Max_Virt_LPTs], 2
	and	eax, CAON_MASK				;Connect Cable?
	jz	SHORT Cable_On				;Yes. 2 Port
	mov	[VJPD_Max_Virt_LPTs], 3			;No. 3 Port
	mov	al, AT_MODE
	mov	edx, LPT2_Base+Low_TOKI_CONTROL
	out	dx, al					;Active AT Mode
Cable_On:
	mov	al, AT_MODE
	mov	edx, LPT3_Base+Low_TOKI_CONTROL
	out	dx, al					;Active AT Mode
No_Board:

	xor	esi, esi		;
	mov	edi, OFFSET32 VPD_INT1A_String ;
	mov	eax, false		; Default value.
	VMMcall Get_Profile_Boolean	; Get entry from SYSTEM.INI
	mov	[SYSINI_Set_Critical], eax ; Set value
endif ;NEC_98
	xor	esi, esi			; Look in [WIN386] section

if	WPS

ifdef MAXDEBUG
	Trace_Out "VPD_Device_Init, ver 3/30 11:30"
endif

	mov	edi, OFFSET32 VJPD_Switch_Timeout_Key	; SYSTEM.INI Keyword=
	mov	eax, [VJPD_Send_Switch_Limit]		; Default Value
	VMMcall Get_Profile_Decimal_Int
	mov	[VJPD_Send_Switch_Limit], eax		; Returned Value
endif ; WPS

	mov	edi, OFFSET32 Sgrab_Ini_String	; Look for this entry
	VMMcall Get_Profile_Decimal_Int 	; EAX = Value of "SGRABLPT"
	jc	SHORT VPD_Init_Ports		; If not found then leave 0
	test	eax, eax			; Q: Is it 0?
	jz	SHORT VPD_Init_Ports		;    Y: Forget it
ifdef NEC_98
	cmp	eax, [VJPD_Max_Virt_LPTs]	; Q: Is this a valid value?
else ;NEC_98
	cmp	eax, VPD_Max_Virt_LPTs		; Q: Is this a valid value?
endif ;NEC_98
	ja	SHORT VPD_Init_Ports		;    N: Leave it 0
						;    Y: Point at structure
	dec	eax				; EAX = 0-3
	imul	eax, SIZE VPD_Struc		; EAX = Offset in LPT table
	add	eax, OFFSET32 VPD_LPT1		; EAX -> Appropriate structure
	mov	[VPD_Sgrab_LPT], eax		; Save it for later
	mov	[VPD_Hw_Int_Owner], ebx 	; Set interrupt owner to Sys_VM

;
;   Loop to initialize all existing LPTs.
;
VPD_Init_Ports:
	xor	edi, edi			; EDI = Count of LPTs so far
	xor	ecx, ecx			; ECX = LPT number
	mov	esi, OFFSET32 VPD_LPT1		; ESI -> First LPT structure

VPD_Init_Loop:
;
;   Check for an entry "LPTnDisable=true/false" in the SYSTEM.INI file.
;   If true, then don't hook ports here.
;   Check for an entry "LPTnAUTOASSIGN=x" in the SYSTEM.INI file.  If one
;   is not found the use the default time-out value (1 minute).
;
	push	ecx				; Save these for later
	push	esi
	push	edi
	add	cl, "1" 			; CL = ASCII LPT number (1-3)

if	WPS
	mov	[VJPD_Jumbo_LPT_Num], cl	; Set WPSLPTn string
	mov	[VJPD_NONJumbo_LPT_Num], cl	; Set NONJUMBOLPTn string
endif ; WPS

	mov	[VPD_Disable_Num], cl		; set string to this LPT
	mov	[VPD_Auto_Assign_Num], cl	; Set string to this LPT

	xor	esi, esi
	mov	edi, OFFSET32 VPD_Disable_Str	; look for this string
	xor	eax, eax			; assume not disable...
	VMMCall	Get_Profile_Boolean
	test	eax, eax
	pop	edi
	pop	esi
	pop	ecx
	TRAPNZ
	jnz	VPD_Init_Next_LPT

	push	ecx
	push	esi
	push	edi

	xor	esi, esi			; Use [WIN386] section
	mov	edi, OFFSET32 VPD_Auto_Assign_Str;Look for this string
	mov	eax, VPD_Default_Timeout	; Default time-out value
	mov	ecx, 3				; 3 decimal points
	VMMcall Get_Profile_Fixed_Point 	; Get entry from SYSTEM.INI
	cmp	eax, -1000			; Q: Is entry -1.000?
	jne	SHORT VPD_Valid_Timeout 	;    N: Use value in EAX
	or	eax, -1 			;    Y: Set to -1
VPD_Valid_Timeout:
	pop	edi				; Restore registers
	pop	esi
	pop	ecx
	mov	[esi.VPD_Timeout_Limit], eax	; Save time-out value

;
;   To determine if a printer port exists we look at the BIOS data area for
;   non-zero entries in the printer base address fields.  If the entry contains
;   a value > 0FFh then we assume it is the base hardware port for the LPT,
;   otherwise we assume it is a network printer connection (networks tend to
;   put a 1 in the this field).  If the LPT is a hardware parallel printer port
;   then VPD hooks three I/O ports.
;
IFDEF NEC_98
	cmp	[VJPD_Max_Virt_LPTs],2		;Use Default Port Address?
	jne	SHORT @f			;Yes.
        mov     edx, VJPD_BIOS_Base_Addr[ecx*4]	;No.
	mov	[esi.VPD_IO_Base], edx		;    Y: Save port base addr
@@:
	mov	edx, [esi.VPD_IO_Base]
        inc     edi                             ; One more LPT virtualized
ELSE ;NEC_98
	movzx	edx, WORD PTR [VPD_BIOS_Base_Addr][ecx*2]
	test	edx, edx			; Q: Is there a printer here?
	jz	VPD_Init_Next_LPT 		;    N: Test next one
	mov	[esi.VPD_IO_Base], edx		;    Y: Save port base addr
	inc	edi				; One more LPT virtualized
	test	dh, dh				; Q: Hardware port? (> 0FFh)
	jz	VPD_Init_Next_LPT		;    N: Network printer!
ENDIF ;NEC_98
	cmp	esi, [VPD_Sgrab_LPT]		;    Y: Q: SGRAB using it?
	je	VPD_Init_Next_LPT		;	   Y: Don't trap I/O
						;	   N: Hook I/O ports
IFNDEF NEC_98
;
;   Some networks place the I/O address for LPT1 into the LPT2 and LPT3
;   base address fields.  Make sure that hasn't happened.
;
	cmp	esi, OFFSET32 VPD_LPT1
	je	SHORT VPD_Hardware_Port
	cmp	[VPD_LPT1.VPD_IO_Base], edx
	je	SHORT VPD_Duplicate_IO_Port
	cmp	esi, OFFSET32 VPD_LPT2
	je	SHORT VPD_Hardware_Port
	cmp	[VPD_LPT2.VPD_IO_Base], edx
	jne	SHORT VPD_Hardware_Port
	.ERRNZ VPD_Max_Virt_LPTs - 3

VPD_Duplicate_IO_Port:				; I/O base address duplicated
	mov	[esi.VPD_IO_Base], 1		; Set IO base to 1 to indicate
	jmp	SHORT VPD_Init_Next_LPT 	; a network printer port
ENDIF ;NEC_98

;
;   This is a physical printer port.  Hook the I/O ports starting with
;   port # in EDX.
;
VPD_Hardware_Port:

if	WPS
;
; Does this port have a Jumbo 2Way Printer? If so, set flags
;
	pushad
	xor	esi, esi			; Use [386Enh] section
	mov	edi, OFFSET32 VJPD_Jumbo_LPT	; SYSTEM.INI Keyword=
	xor	eax, eax			; Default Value = FALSE
	VMMcall Get_Profile_Boolean
	cmp	eax, 0
	popad
ifdef	NEC_98
	je	SHORT VJPD_Not_Jumbo_Device_98
else
	je	SHORT VJPD_Not_Jumbo_Device
endif	;NEC_98

;
; check for WPS but not JUMBO
;
	pushad
	xor	esi, esi			; Use [386Enh] section
	mov	edi, OFFSET32 VJPD_NONJumbo_LPT ; SYSTEM.INI Keyword=
	xor	eax, eax			; Default Value = FALSE
	VMMcall Get_Profile_Boolean
	cmp	eax, 0
	popad
	je	SHORT @F
ifdef	NEC_98
VJPD_Not_Jumbo_Device_98:
endif	;NEC_98
	or	[esi.VPD_Flags], VJPDF_NONJumbo
@@:

VJPD_Not_Jumbo_Device:

endif ; WPS

	push	ecx				; Save loop counter
	push	esi				; and data structure pointer

ifdef NEC_98
        mov     ecx, Num_TOKI_Ports_Per_LPT1    ; ECX = # of ports to hook
	mov	esi, [esi.VPD_Proc_Addr]	; ESI -> Procedure to call
VPD_ITrap_Loop:
	mov	dl, TOKI_Low_Addr_Table[ecx]	; EDX = LPT port to hook
else ;NEC_98
	mov	esi, [esi.VPD_Proc_Addr]	; ESI -> Procedure to call
	mov	ecx, VPD_Num_Ports_Per_LPT	; ECX = # of ports to hook
VPD_ITrap_Loop:
endif ;NEC_98
	VMMcall Install_IO_Handler		; Install port hook
IFNDEF NEC_98
	jc	VPD_ITrapped
	VMMCall	Disable_Global_Trapping
	inc	edx				; EDX = Next LPT port to hook
ENDIF ;NEC_98
	loopd	VPD_ITrap_Loop			; Loop until all hooked
	clc

VPD_ITrapped:
	pop	esi				; Restore
	pop	ecx
	jnc	VPD_Init_Next_LPT
	mov	[esi.VPD_IO_Base], 0

VPD_Init_Next_LPT:
	add	esi, SIZE VPD_Struc		; ESI -> Next data structure
	inc	ecx				; Bump counter
ifdef NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]	; Q: More LPTs to init?
	jb	VPD_Init_Loop			;    Y: Loop
else ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs		; Q: More LPTs to init?
	jb	VPD_Init_Loop			;    Y: Loop

;
;   All printer ports hooked.  At this point we check to see if there are
;   any printers (network or hardware) connected.  If there are none then
;   VPD_Device_Init will return with carry SET to indicate that it should
;   never be called again.  If any printers do exist then VPD hooks IRQ 7
;   hardware interrupt and software interrpt 17h.
;
	test	edi, edi			; Q: ANY printers virtualized?
	jnz	SHORT VPD_LPTs_Found		;    Y: VPD is needed
						;    N: Nothing to do
	stc					; Tell VMM to never call us
	ret
endif ;NEC_98

;
;   Set up for simulated hardware interrupts.  If VPD can not hook IRQ 7
;   then it will disable SGRAB support since SGRAB uses the printer
;   interrupt.
;
VPD_LPTs_Found:

IFDEF NEC_98
	mov	edx, 040h
	mov	esi, OFFSET32 VPD_LPT1_Trap	; ESI -> Procedure to call
	mov	ecx, VPD_Num_Ports_Per_LPT	; ECX = # of ports to hook
VPD_ITrap_Loop40:
	VMMcall Install_IO_Handler		; Install port hook
	inc	edx				; EDX = Next LPT port to hook
	inc	edx
	loopd	VPD_ITrap_Loop40		; Loop until all hooked
	mov	edx,448h
	VMMcall	Install_IO_Handler		; Install port hook
ENDIF ;NEC_98

	cmp	[VPD_Sgrab_LPT], 0		; Q: Is SGRAB enabled?
	je	SHORT VPD_Alloc_CB_Area 	;    N: Let VPICD handle IRQ
						;    Y: Want to virtualize it
	mov	edi,OFFSET32 VPD_IRQ_Descriptor ; EDI -> VPD IRQ descriptor
	VxDCall VPICD_Virtualize_IRQ		; Q: Can we virtualize IRQ?
	jnc	SHORT VPD_IRQ_Virt_OK		;    Y: OK -- Save IRQ handle
						;    N: ERROR!
	Debug_Out "WARNING:  VPD could not virtualize IRQ"
	mov	[VPD_Sgrab_LPT], 0		; Can't use SGRAB!
	jmp	SHORT VPD_Alloc_CB_Area 	; Go to next step of init

VPD_IRQ_Virt_OK:
;
;   NOTE:  At this point EAX contains the IRQ handle returned from
;   VPICD_Virtualize_IRQ.  However, since it is always passed back to
;   the VPD interrupt procedures, there is no need to save it.	Normally,
;   a VxD would save the handle in a global variable at this point:
;	mov	[VPD_IRQ_Handle], eax
;


VPD_Alloc_CB_Area:
;
;   Allocate per-VM data area in the control block.
;
	VMMcall _Allocate_Device_CB_Area, <4,0> ; One DWORD for contention info
	test	eax, eax
	jnz	SHORT VPD_CB_Ok
	Debug_Out "VPD ERROR:  Could not alloc control block data area space"
	VMMcall Fatal_Memory_Error

VPD_CB_Ok:
	mov	[VPD_CB_Offset], eax		; Save offset

;
;   Instance the printer BIOS areas.  This will make each virtual machine
;   have its own copy of the printer timeout values.
;
	VMMCall _AddInstanceItem, <<OFFSET32 VPD_Inst_Timeout>, 0>
	test	eax, eax
	jnz	SHORT VPD_Inst_Ok
	Debug_Out "VPD ERROR:  Could not instance BIOS printer area"
	VMMcall Fatal_Memory_Error

VPD_Inst_Ok:

IFDEF NEC_98
	NOP
	NOP
	VMMCall _AddInstanceItem, <<OFFSET32 VPD_BI_IF_Mode>, 0>
	test	eax, eax
	jnz	SHORT VPD_BI_Inst_Ok
	Debug_Out "VPD ERROR:  Could not instance BIOS printer area"
	VMMcall Fatal_Memory_Error

VPD_BI_Inst_Ok:

	test	[VPD_BIBD_BYTE],02h		; Q: Exist PRTDRV.SYS?
	jz	VPD_DRV_Inst_Ok			;  No.

        push    eax
	push	edx

        Push_Client_State                       ; Get ready for software ints
        VMMCALL Begin_Nest_Exec

        mov     [ebp.Client_AX],1effh		; Get PRTDRV.SYS Data Area
        mov     eax,1Ah
        VMMCALL Exec_Int                        ; Get
        mov     ax,[ebp.Client_AX]		;   PRTDRV.SYS Segment
        mov     dx,[ebp.Client_DX]		;              Offset

	add	dx,11h

        VMMCALL End_Nest_Exec                   ; All done with software ints
        Pop_Client_State


	mov	[VPD_DRV_Seg],ax		; Save Segment
	mov	[VPD_DRV_Off],dx		; Save Offset

	pop	edx
	pop	eax

	push	edx
	push	eax

	xor	edx,edx
	mov	dx,[VPD_DRV_Seg]
	shl	edx,4				; Shift 4Bit

	xor	eax,eax
	mov	ax,[VPD_DRV_Off]
	add	edx,eax

	mov	dword ptr [VPD_DRV_IF_Mode+8],edx

	pop	eax
	pop	edx

	VMMCall _AddInstanceItem, <<OFFSET32 VPD_DRV_IF_Mode>, 0>
	test	eax, eax
	jnz	SHORT VPD_DRV_Inst_Ok
	Debug_Out "VPD ERROR:  Could not instance BIOS printer area"
	VMMcall Fatal_Memory_Error

VPD_DRV_Inst_Ok:

	push	es
	mov	ax, 40h
	mov	es, ax
	mov	eax, 135027		; counter for 2.5MHz (54.93ms)
	test	byte ptr es:[101h], 80h	; Q : clock is 2.5MHz
	pop	es
	jz	SHORT @f
	mov	eax, 109703		; counter for 2.0MHz (54.93ms)
@@:
	mov	dword ptr [ATRes], eax

ENDIF ;NEC_98
;
;   Hook Int 17h BIOS software interrupt to detect contention.	This allows
;   detection of some network printer contention also.
;
IFDEF NEC_98
;   Int 17h -> Int 1Ah  by NEC ///Printer BIOS is INT 1Ah (PC-9800)

	mov	eax, 1Ah			; # of interrupt to hook
	mov	esi, OFFSET32 VPD_Int_1Ah	; Where to call
ELSE ;NEC_98
	mov	eax, 17h			; # of interrupt to hook
	mov	esi, OFFSET32 VPD_Int_17h	; Where to call
ENDIF ;NEC_98
	VMMcall Hook_V86_Int_Chain		; Hook the interrupt

ifdef NEC_98
;   Hook int 5h (copy key interrupt) to process hardcopy service properly
	mov	eax, 5h
	mov	esi, OFFSET32 VPD_Int_5h
	VMMcall	Hook_V86_Int_Chain
endif ;NEC_98

;
;   Done!  Return with carry flag clear to indicate success.
;
	clc					; Successful initialization
	ret

EndProc VPD_Device_Init



;******************************************************************************
;
;   VPD_Init_Complete
;
;   DESCRIPTION:
;	Instance DOS devices for LPT1-LPT3 and PRN.  This must be delayed
;	until the Init_Complete control call since the DOSMGR service is
;	not valid to call until this time.  We do not check the carry flag
;	after calling DOSMGR_Instance_Device because the DOS device may
;	may not exist or may reside in the DOS BIOS (devices in the DOS
;	BIOS can not be instanced by DOSMGR_Instance_Device).  In any case,
;	no error is flaged if one or more of the devices can not be instanced.
;
;   ENTRY:
;	EBX = System VM Handle
;
;   EXIT:
;	Carry clear (Always works)
;
;   USES:
;
;==============================================================================

BeginProc VPD_Init_Complete

IFDEF NEC_98
	push	edx				; Save EDX
	push	esi				; Save ESI
	mov	edx, 035h			; Set Trap Port Addr
	mov	esi, OFFSET32 VPD_Sys_Port
	VxDCall	VPPID_Install_IO_Handle		; Set Trap routine Addr
	mov	[VPPIDReg], edx			; Set VPPID Set Reg #
	pop	esi				; Restore ESI
	pop	edx				; Restore EDX

	test	[VPD_BIBD_BYTE],02h		; Q: Exist PRTDRV.SYS?
	jz	VPD_SET_CB_FIRST		;  No.

	push	esi				;
	push	edx				;
	push	eax

	xor	edx,edx				; Clear EDX
	mov	dx,[VPD_DRV_Seg]		; Set PRTDRV.SYS Segment
	mov	esi,edx
	shl	esi,4

	push	eax
	xor	eax,eax
	mov	ax,[VPD_DRV_Off]
	add	esi,eax
	pop	eax

	add	esi, [ebx.CB_High_Linear]	; esi = point to 0:458h

	mov	al,[esi]			;
	mov	[VPD_BD_DRV_BYTE],al		;

	pop	eax
	pop	edx
	pop	esi

;=== Check Number of Ports ===

	cmp	[VPD_BI_PORT],1			; Check Number of Port
	je	VPD_SET_CB_FIRST		; 1 port then VPD_SET_CB_FIRST

	cmp	[VPD_BI_PORT],3			; Check Number of Port
	je	SHORT VPD_SET_CB_THIRD		; 3 ports then VPD_SET_CB_THIRD
	jmp	VPD_SET_CB_SECOND		; 2 ports then VPD_SET_CB_SECOND
						;

;=== 3 Ports ===

VPD_SET_CB_THIRD:
	push	edx				;
	push	eax
	push	ecx

VPD_SET_CB_THIRD_3C:
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	[VPD_CB_EBX], ebx		; Save System VM Handle

	test	[VPD_BD_DRV_BYTE],80h		; LPT3 is Full ?
	mov	ecx, VPD_LPT3_IF_Mode_Bit	; ECX = I/F mode info.
	jnz	SHORT VPD_SET_CB_THIRD_3S	;   Semi !

VPD_SET_CB_THIRD_3F:
	bts	DWORD PTR [eax], ecx		; Set I/F mode bit

	jmp	SHORT VPD_SET_CB_THIRD_2C

VPD_SET_CB_THIRD_3S:
	btr	DWORD PTR [eax], ecx		; Reset I/F mode bit

VPD_SET_CB_THIRD_2C:
	test	[VPD_BD_DRV_BYTE],40h		; LPT2 is Full ?
	mov	ecx, VPD_LPT2_IF_Mode_Bit	; ECX = I/F mode info.
	jnz	SHORT VPD_SET_CB_THIRD_2S	;   Semi !

VPD_SET_CB_THIRD_2F:
	bts	DWORD PTR [eax], ecx		; Set I/F mode bit

	jmp	SHORT VPD_SET_CB_THIRD_1C

VPD_SET_CB_THIRD_2S:
	btr	DWORD PTR [eax], ecx		; Reset I/F mode bit

VPD_SET_CB_THIRD_1C:
	mov	ecx, VPD_PORT_REG_Bit		; ECX = Port Reg Info.
	btr	DWORD PTR [eax], ecx		; Clear Info

	test	[VPD_BD_DRV_BYTE],10h		; Port Reg. ON?
	jz	SHORT VPD_SET_CB_THIRD_PORT

	bts	DWORD PTR [eax], ecx		; Set Info.

VPD_SET_CB_THIRD_PORT:
	mov	ecx, VPD_PORT_INFO_Bit3		; ECX = Default Port Info.
	btr	DWORD PTR [eax], ecx		; Clear Info
	mov	ecx, VPD_PORT_INFO_Bit4		; ECX = Default Port Info.
	btr	DWORD PTR [eax], ecx		; Clear Info

	test	[VPD_BD_DRV_BYTE],01h		; LPT1 ?
	jnz	SHORT VPD_SET_CB_THIRD_END	; End Get Info

VPD_SET_CB_THIRD_PORT_LPT2:
	test	[VPD_BD_DRV_BYTE],02h		; LPT2 ?
	jz	SHORT VPD_SET_CB_THIRD_PORT_LPT3	; LPT3 !

	mov	ecx, VPD_PORT_INFO_Bit3		; ECX = Default Port Info.
	bts	DWORD PTR [eax], ecx		; Set Info
	jmp	SHORT VPD_SET_CB_THIRD_END	; End Get Info

VPD_SET_CB_THIRD_PORT_LPT3:
	mov	ecx, VPD_PORT_INFO_Bit4		; ECX = Default Port Info.
	bts	DWORD PTR [eax], ecx		; Set Info

VPD_SET_CB_THIRD_END:
	pop	ecx
	pop	eax
	pop	edx

	jmp	VPD_SET_CB_FIRST		; Check built in printer port


;=== 2 Ports ===

VPD_SET_CB_SECOND:
;@PRTDRV.SYS control all ports
;  Don't use system data area

	push	edx
	push	eax
	push	ecx

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	[VPD_CB_EBX], ebx		; Save System VM Handle

	mov	ecx, VPD_LPT3_IF_Mode_Bit	; ECX = I/F mode info.
	btr	DWORD PTR [eax], ecx		; Set I/F mode bit

	test	[VPD_BD_DRV_BYTE],80h		; LPT2 is Full ?
	mov	ecx, VPD_LPT2_IF_Mode_Bit	; ECX = I/F mode info.
	jnz	SHORT VPD_SET_CB_SECOND_2S	;   Semi !

VPD_SET_CB_SECOND_2F:
	bts	DWORD PTR [eax], ecx		; Set I/F mode bit

	jmp	SHORT VPD_SET_CB_SECOND_1C

VPD_SET_CB_SECOND_2S:
	btr	DWORD PTR [eax], ecx		; Reset I/F mode bit

VPD_SET_CB_SECOND_1C:
	test	[VPD_BD_DRV_BYTE],40h		; LPT1 is Full ?
	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.
	jnz	SHORT VPD_SET_CB_SECOND_1S	;   Semi !

VPD_SET_CB_SECOND_1F:
	bts	DWORD PTR [eax], ecx		; Set I/F mode bit

	jmp	SHORT VPD_SET_CB_SECOND_PE

VPD_SET_CB_SECOND_1S:
	btr	DWORD PTR [eax], ecx		; Reset I/F mode bit

VPD_SET_CB_SECOND_PE:
	mov	ecx, VPD_PORT_REG_Bit		; ECX = Port Reg Info.
	btr	DWORD PTR [eax], ecx		; Clear Info

	test	[VPD_BD_DRV_BYTE],10h		; Port Reg. ON?
	jz	SHORT VPD_SET_CB_SECOND_PORT

	bts	DWORD PTR [eax], ecx		; Set Info.

VPD_SET_CB_SECOND_PORT:
	mov	ecx, VPD_PORT_INFO_Bit3		; ECX = Default Port Info.
	btr	DWORD PTR [eax], ecx		; Clear Info
	mov	ecx, VPD_PORT_INFO_Bit4		; ECX = Default Port Info.
	btr	DWORD PTR [eax], ecx		; Clear Info

	test	[VPD_BD_DRV_BYTE],02h		; LPT1 ?
	jnz	SHORT VPD_SET_CB_SECOND_END	; End Get Info

VPD_SET_CB_SECOND_PORT_LPT2:
	test	[VPD_BD_DRV_BYTE],04h		; LPT2 ?
	jz	SHORT VPD_SET_CB_SECOND_END	;

	mov	ecx, VPD_PORT_INFO_Bit3		; ECX = Default Port Info.
	bts	DWORD PTR [eax], ecx		; Set Info

VPD_SET_CB_SECOND_END:
	pop	ecx
	pop	eax
	pop	edx

	mov	[VPD_458_BYTE], dl		; Save 0:458h
	mov	[VPD_5B3_BYTE] ,dh		; Save 0:5B3h = PRT_IFMODE

	jmp	VPD_Check_IF_END		; Check done.

;=== 1 Port & 3 Port's LPT1 ===

VPD_SET_CB_FIRST:
	mov	[VPD_458_BYTE], dl		; Save 0:458h

	mov	[VPD_5B3_BYTE] ,dh		; Save 0:5B3h = PRT_IFMODE

	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jz	SHORT 	VPD_IC_Not_H98		;no,then skip

	push	eax	;
	push	ecx	;

	mov	eax, [VPD_CB_Offset]

	add	eax, ebx			; EAX -> VPD control block data
	mov	[VPD_CB_EBX], ebx		; Save System VM Handle

	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.

	test	[VPD_458_BYTE],04h		; Full?
	jz	SHORT VPD_IC_Semi

	bts	DWORD PTR [eax], ecx		; Set I/F mode bit
	jmp	SHORT VPD_IC_H98_End
VPD_IC_semi:
	btr	DWORD PTR [eax], ecx		; Reset I/F mode bit
VPD_IC_H98_End:
	pop	ecx
	pop	eax
	jmp	SHORT VPD_BIP_Disable		;

VPD_IC_Not_H98:
	push	edx
	mov	edx, 448h
	VMMcall Disable_Global_Trapping		; Disable port trapping
	pop	edx

	cmp	[VPD_LPT1.VPD_IO_Base], LPT2_Base+Low_AT_DATA
	je	SHORT @f
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Check Bi-Direction Support
	End_Touch_1st_Meg
	jz	SHORT VPD_BIP_Disable		; Non Bi-Direction PC
@@:

;*** Check I/F mode
	push	edx
	push	eax
	push	ecx

	mov	edx, [VPD_LPT1.VPD_IO_Base]
	mov	[VPD_CB_EBX], ebx		; Save System VM Handle
	mov	eax,[VPD_CB_Offset]		; 
	add	eax,ebx				; EAX -> VPD control block data
	push	eax

	mov	dl,049h				; TOKI Control Port
	in	al,dx				; Get TOKI Status
	test	al,10h				; PC-98 Semi Mode
	pop	eax
	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.
	jnz	SHORT VPD_Check_AT		; No, then jump

;*** PC-98 Semi Mode

	btr	DWORD PTR [eax], ecx		; Reset LPT1 I/F Mode Bit
	jmp	SHORT VPD_Check_AT_END		; END Check

;*** AT or PS ? ***

VPD_Check_AT:
	bts	DWORD PTR [eax], ecx		; Set LPT1 I/F Mode Bit

VPD_Check_AT_END:
	pop	ecx
	pop	eax
	pop	edx
	jmp	SHORT VPD_Check_IF_END		; 

VPD_BIP_Disable:
	push	edx
	push	eax
	push	ecx

	mov	edx, LPT1_Base+Low_AT_DATA	;edx = 140h
        mov     ecx, Num_TOKI_Ports_Per_LPT1    ; ECX = # of ports to hook
VPD_BI_ITrap_Dis:
	mov	dl, TOKI_Low_Addr_Table[ecx]	; EDX = LPT port to hook
	VMMcall Disable_Global_Trapping		; Disable port trapping
	loopd	SHORT VPD_BI_ITrap_Dis

	pop	ecx
	pop	eax
	pop	edx

VPD_Check_IF_END:
	push	eax				;
	push	ecx				;
	push	edx

	mov	eax,[VPD_CB_Offset]		; 
	add	eax,ebx				; EAX -> VPD control block data
	mov	edx,[eax]			; Set I/F mode bit
	mov	[VPD_SYS_CB_DATA],edx		; Save System VM Control Block

	pop	edx				;
	pop	ecx				;
	pop	eax				;
ENDIF ;NEC_98
;
;   Instance devices LPT1, LPT2, and LPT3
;
ifdef MAXDEBUG
	Trace_Out "VPD_Init_Complete"
endif
IFDEF NEC_98
	mov	ecx, [VJPD_Max_Virt_LPTs]	; Number of LPT devices
ELSE ;NEC_98
	mov	ecx, VPD_Max_Virt_LPTs		; Number of LPT devices
ENDIF ;NEC_98
	lea	esi, VPD_LPT1.VPD_Name		; ESI -> "LPT1    "
VPD_IC_Instance_Loop:
	VxDcall DOSMGR_Instance_Device		; Instance this LPT
	add	esi, SIZE VPD_Struc		; ESI -> Next name
	loopd	VPD_IC_Instance_Loop		; Loop for all LPTs

;
;   Instance the PRN device
;
	mov	esi, OFFSET32 VPD_PRN_Device_Name
	VxDcall DOSMGR_Instance_Device

;
;   If EPT device is installed then instance it.
;
;;;	test	edx, edx
;;;	jz	SHORT VPD_IC_Exit
;;;	mov	esi, OFFSET32 VPD_EPT_Device_Name
;;;	VxDcall DOSMGR_Instance_Device

;
;   Return with carry clear to indicate success
;
VPD_IC_Exit:
	clc
	ret

EndProc VPD_Init_Complete

; VxD_ICODE_ENDS   need-work, tmp workaround
VxD_CODE_ENDS

VxD_Pageable_Data_Seg

Contention_Handler	dd	OFFSET32 _VPD_Map_Name_To_Resource
			dd	OFFSET32 _VPD_Acquire_Resource
			dd	OFFSET32 _VPD_Steal_Resource
			dd	OFFSET32 _VPD_Release_Resource
			dd	OFFSET32 _VPD_Add_Resource

VxD_Pageable_Data_Ends

VxD_Pageable_Code_Seg

;******************************************************************************
;
; VPD_Map_Name_To_Resource
;
; Description: maps a name such as LPTx to a resource
;
; Parameters:
;
;	FunctionCode = MAP_NAME_TO_RESOURCE
;	PName -> Name to map
;	hDevNode = devnode corresponding to the device
;	IOBase	= Base allocated to the device
;
; Returns:
;	0 if could not be mapped, else Struct for the port.
;
;=============================================================================
BeginProc VPD_Map_Name_To_Resource,CCALL,PUBLIC

ArgVar	FunctionName,DWORD
ArgVar	PName,DWORD
ArgVar	hDevNode,DWORD
ArgVar	IOBase,DWORD

	EnterProc
ifdef MAXDEBUG
	Trace_Out "VPD_MapName_To_Resource"
endif

ifdef NEC_98
	mov	ecx,[VJPD_Max_Virt_LPTs]
else ;NEC_98
	mov_b	ecx,VPD_Max_Virt_LPTs
endif ;NEC_98
	mov	edx,OFFSET32 VPD_LPT1		; first LPT structure
	mov	eax,IOBase
ifdef NEC_98
	cmp	eax, 040h
	je	MapName_Done
endif ;NEC_98

MapName_Loop:
	cmp	[edx.VPD_IO_Base],eax
	je	MapName_Done
	lea	edx,[edx+ SIZE VPD_Struc]
	loopd	MapName_Loop
	xor	edx,edx				; Couldn't map

MapName_Done:
	mov	eax,edx				; return PTR to VPD_Struc

ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Map_Name_To_Resource

;******************************************************************************
;
; VPD_Steal_Resource
;
; Description:
;	Called to steal a port which was lost due to someone else
;	wanting control.
;
; Parameters:
;	FunctionCode = STEAL_RESOURCE
;	ResourceHnd = resource handle
;	NotifyProc = Address of proc set during Acquire_Resource.
;
; Exit:
;	EAX != 0 if stolen, else = 0.
;
;==============================================================================
BeginProc VPD_Steal_Resource,CCALL,PUBLIC

ArgVar	FunctionCode,DWORD
ArgVar	Resource,DWORD
ArgVar	NotifyProc,DWORD

	EnterProc

	SaveReg	<esi,edi,ebx>
ifdef MAXDEBUG
	Trace_Out "VPD_Steal_Resource"
endif

	mov	esi,Resource
	test	esi,esi
	jz	VSR_Failed
	VMMCall	Get_Sys_VM_Handle
	mov	eax,NotifyProc
	cmp	[esi.VPD_Alt_NotifyProc],eax ; Q: Alternate owner ?
	jne	VSR_Primary_Owner_Steal	;    N: steal it for primary owner
	cmp	ebx,[esi.VPD_Owner]	; Q: Does primary owner own it ?
	jnz	VSR_Switch_Owners	;    N: Switch owners and try stealing.
	cCall	[esi.VPD_NotifyProc],<[esi.VPD_NotifyRefData],0> ; losing port
	test	eax,eax			; Q: Ok to steal from primary owner ?
	jz	VSR_Done		;   N:
	mov	eax,NotifyProc

VSR_Switch_Owners:
 ;
 ; Some other VM owns it, and alternate owner wants to steal. We will make
 ; it primary owner and try to steal.
 ;
	xchg	eax,[esi.VPD_NotifyProc] ; VPD_NotifyProc = alt notifyProc
	xchg	eax,[esi.VPD_Alt_NotifyProc]	; VPD_Alt_NotifyProc = prim own
	mov	eax,[esi.VPD_NotifyRefData]
	xchg	eax,[esi.VPD_Alt_NotifyRefData] ; VPD_Alt_NotifyRef = prim ref
	mov	[esi.VPD_NotifyRefData],eax	; VPD_NotifyRefData = alt ref

VSR_Primary_Owner_Steal:
	cmp	ebx,[esi.VPD_Owner]
	jz	VSR_Success
	xor	al,al			; set Z => Don't force ownership
	call	VPD_Acquire_Port
	jnc	VSR_Success

VSR_Failed:
	xor	eax,eax
	jmp	VSR_Done

VSR_Success:
	or	al,1

VSR_Done:
	RestoreReg <ebx,edi,esi>
ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Steal_Resource

;******************************************************************************
;
; VPD_Acquire_Resource
;
; Description:
;	Tries to acquire the port.
;
; Entry:
;	FunctionCode = ACQUIRE_RESOURCE
;	Resource = VPD_Struc
;	NotifyProc -> Address of procedure to call
;	NotifyRefData = reference data to call with.
;	StealFlag = if 0, don't even try to acquire if owned
;		    if 1, acquire it, try to steal if owned.
;
; Exit:
;	0 if not acquired, else port handle
; Uses:
;	C style.
;==============================================================================
BeginProc VPD_Acquire_Resource,CCALL,PUBLIC,esp

ArgVar	FunctionCode,DWORD
ArgVar	Resource,DWORD
ArgVar	NotifyProc,DWORD
ArgVar	NotifyRefData,DWORD
ArgVar	StealFlag,DWORD

	EnterProc

	SaveReg	<esi,edi,ebx>
ifdef MAXDEBUG
	Trace_Out "VPD_Acquire_Resource"
endif

	mov	esi,Resource		; VPD_STRUC
	test	esi,esi
	jz	VAR_Failed

if	WPS
	TestMem	[esi.VPD_Flags], VJPDF_Jumbo
	jz	@F
	mov	eax,Resource
	VMMCall	Get_Cur_VM_Handle
	cmp	[esi.VPD_Owner], ebx		; Q: Does this VM own it?
	je	VAR_Done			;   Y: no switch needed
@@:
endif

ifndef NEC_98
	TestMem	[esi.VPD_Flags],VPDF_Enabled	; Q: Ports already hooked ?
	jnz	@F				;    Y: no need to rehook
	call	VPD_Enable_Trapping
@@:
endif ;NEC_98
	TestMem	[esi.VPD_Flags],VPDF_AltOwnerActive ; Q: Two owners pending ?
	jnz	VAR_Failed		;  Y: no more owners allowed.

	mov	ecx,[esi.VPD_NotifyProc] ; Q: Owned port ?
	jecxz	VAR_Acquire		;    N: let it be acquired.
	cmp	StealFlag,0		; Y: Should we try to acquire ?
	je	VAR_Failed		;    N:
	cCall	ecx,<[esi.VPD_NotifyRefData],0> ; losing port
	test	eax,eax			; Q: Should we allow it ?
	jz	VAR_Done		;    N:
	mov	ecx,[esi.VPD_NotifyProc] ; Q: Did owner free the port ?
	jecxz	VAR_Acquire		; Y: normal acquire
	mov	edx,[esi.VPD_NotifyRefData]
	mov	[esi.VPD_Alt_NotifyProc],ecx
	mov	[esi.VPD_Alt_NotifyRefData],edx
	SetFlag	[esi.VPD_Flags],VPDF_AltOwnerActive

VAR_Acquire:
	VMMCall	Get_Sys_VM_Handle
	xor	al,al			; set Z => Don't force ownership
	call	VPD_Acquire_Port
	jc	VAR_Failed
	mov	eax,NotifyProc
	mov	[esi.VPD_NotifyProc],eax
	mov	eax,NotifyRefData
	mov	[esi.VPD_NotifyRefData],eax
	mov	eax,Resource		; return this as the handle also.
	jmp	VAR_Done

VAR_Failed:
	xor	eax,eax

VAR_Done:

	RestoreReg <ebx,edi,esi>
ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Acquire_Resource

;******************************************************************************
;
; VPD_Release_Resource
;
; Description:
;	Called to release a resource acquired via Acquire_Resource.
;
; Parameters:
;	FunctionCode = RELEASE_RESOURCE
;	ResourceHnd = handle of the resource
;	NotifyProc = Address of proc set during Acquire_Resource.
; Exit:
;	None
;
; Uses:	C Style
;==============================================================================
BeginProc VPD_Release_Resource,CCALL,PUBLIC,esp

ArgVar	FunctionCode,DWORD
ArgVar	ResourceHnd,DWORD
ArgVar	NotifyProc,DWORD

	EnterProc

	SaveReg	<esi,edi,ebx>
ifdef MAXDEBUG
	Trace_Out "VPD_Release_Resource"
endif

	mov	esi,ResourceHnd
	mov	eax,NotifyProc
	cmp	[esi.VPD_Alt_NotifyProc],eax	; Q: Alternate owner ?
	jz	VRR_Alt_Owner			;    Y: just clear the flag

	VMMCall	Get_Sys_VM_Handle
	cmp	[esi.VPD_Owner],ebx
	jne	VRR_Release_Resource_Done

	call	VPD_Detach

VRR_Release_Resource_Done:
	mov	[esi.VPD_NotifyProc],0
	btr	[esi.VPD_Flags],VPDF_AltOwnerActiveBit ; Q: Pending alt_Owner ?
	jnc	VRR_Done

	cCall	VPD_Acquire_Resource,<ACQUIRE_RESOURCE,esi, \
		[esi.VPD_Alt_NotifyProc], [esi.VPD_Alt_NotifyRefData], \
		0>

	cCall	[esi.VPD_NotifyProc],<[esi.VPD_NotifyRefData],1>

VRR_Alt_Owner:
	ClrFlag	[esi.VPD_Flags],VPDF_AltOwnerActive

VRR_Done:
ifndef NEC_98
	cmp	[esi.VPD_NotifyProc], 0
	jnz	@F
	call	VPD_Disable_Trapping
@@:
endif ;NEC_98
	RestoreReg <ebx,edi,esi>
ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Release_Resource

;******************************************************************************
;
; VPD_Add_Resource
;
;
; Decription: We verify that the port exists. Then we take limited actions
;	to add this port to our list of ports if not already present.
;
; Entry:
;	
; Exit:
;	0 if port not present, else non-zero
;==============================================================================
BeginProc VPD_Add_Resource, CCALL, PUBLIC, esp, RARE

ArgVar	FunctionCode, DWORD
ArgVar	hDevNode, DWORD
ArgVar	AllocBase, DWORD
ArgVar	AllocIRQ, DWORD

	EnterProc

	mov	edx, AllocBase
ifdef NEC_98
	mov	eax, edx
	cmp	edx, 040h			; Q: Standart port?
	je	VARD_Done			;  Yes
	cmp	edx, AT_DATA1
	je	VARD_ReadControl
	cmp	edx, AT_DATA2
	je	VARD_ReadControl
	cmp	edx, AT_DATA3
	je	VARD_ReadControl
	xor	eax, eax
	jmp	VARD_Done
VARD_ReadControl:
	mov	dl, Low_TOKI_CONTROL
	in	al, dx				; read toki control
	cmp	al, 0FFh			; Q: invalid port?
	mov	eax, 0
	je	VARD_Done			;  Yes
	mov	eax, AllocBase			;  No
else ;NEC_98
	inc	edx				; status port
	in	al, dx				; read status
	mov	ch, al
	inc	edx				; control port
	in	al, dx				; read control
	mov	cl, al
	xor	eax, eax
	cmp	cl, 0FFh			; Q: invalid ?
	jz	VARD_Done
	cmp	cl, ch
	jz	VARD_Done

	xor	ecx, ecx
	mov	eax, AllocBase

VARD_Loop:
	movzx	edx, WORD PTR [ecx*2+0408h]
	cmp	eax, edx			; Q: BDA know about it ?
	je	VARD_Done			;    Y: get out
	test	edx, edx			;    N: fill the area.
	jz	VARD_Fill_It
	test	dh, 0FFh
	jz	VARD_Done
	inc	cl
	cmp	cl, 2
	jbe	VARD_Loop
	jmp	VARD_Done

VARD_Fill_It:
	Trace_Out "VPD: Adding port #EAX to BDA"

	mov	WORD PTR [ecx*2+0408h], ax
endif ;NEC_98

VARD_Done:
	LeaveProc
	return

EndProc VPD_Add_Resource

;******************************************************************************
;
; VPD_Get_Contention_Handler
;
; Description:
;	Called to get the contention handler's address by VCOMM
;
; Entry:
;	EAX = GET_CONTENTION_HANDLER
; Exit:
;	VPD's Contention handler address
; Uses:
;	EAX,flags
;==============================================================================
BeginProc VPD_Get_Contention_Handler,PUBLIC

ifdef MAXDEBUG
	Trace_Out "VPD_GetContention_Handler"
endif
	mov	eax,OFFSET32 _VPD_Contention_Handler
	clc
	ret

EndProc VPD_Get_Contention_Handler

;******************************************************************************
;
; VPD_Contention_Handler
;
; Description:
;	Calls appropriate function handler
; Entry:
;	FunctionCode = function to call
; Exit:
;	That of Function
; Uses:
;	C style
;==============================================================================
BeginProc VPD_Contention_Handler,CCALL,esp,PUBLIC

ArgVar	FunctionCode,DWORD

	EnterProc
	mov	eax,FunctionCode
ifdef MAXDEBUG
	Trace_Out "VPD_Contention_Handler (function code #EAX)"
endif
	cmp	eax,ADD_RESOURCE
	ja	VCH_Done
	jmp	[Contention_Handler+eax*4]

VCH_Done:
	xor	eax,eax
ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Contention_Handler

VxD_Pageable_Code_Ends

;******************************************************************************
;	       D E V I C E   C O N T R O L   H A N D L E R S
;******************************************************************************

VxD_CODE_SEG

IFDEF NEC_98
;*****************************************************************************
;   VPD_VM_Critical_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;          ebx = VM handle of new VM.
;
;=============================================================================
BeginProc VPD_VM_Critical_Init

	push	eax				;
	push	ebx				;
	push	edx

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	edx,[VPD_SYS_CB_DATA]		; Save System VM Control Block
	mov	[eax], edx			; VM Control Block

	pop	edx
	pop	ebx
	pop	eax
VPD_VM_CI_EXIT:

	clc
	ret
EndProc VPD_VM_Critical_Init
ENDIF ;NEC_98

;******************************************************************************
;
;   VPD_Set_Focus
;
;   DESCRIPTION:
;	This control call is only important if Sgrab support is enabled.
;	If Sgrab is enabled then this procedure will watch for display set-
;	focus calls to track the display owner.
;
;   ENTRY:
;	EBX = VM handle for new display focus
;
;   EXIT:
;	Carry clear (no error)
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VPD_Set_Focus

	cmp	[VPD_Sgrab_LPT], 0		    ; Q: Is Sgrab enabled?
	je	SHORT VPD_SF_Exit		    ;	 N: Then we don't care!
						    ;	 Y: Display may change
	test	edx, edx			    ; Q: Critical set-focus?
	jz	SHORT VPD_SF_Display_Changed	    ;	 Y: Set new Sgrab focus
	cmp	edx, VDD_Device_ID		    ;	 N: Q: VDD device?
	jne	SHORT VPD_SF_Exit		    ;	       N: Ignore it
VPD_SF_Display_Changed:
	mov	[VPD_Hw_Int_Owner], ebx 	    ; Ints go to display owner

VPD_SF_Exit:
	ret

EndProc VPD_Set_Focus


;******************************************************************************
;
;   VPD_VM_Not_Executeable
;
;   DESCRIPTION:
;	The "VM_Not_Executeable" control call indicates that the specified
;	VM will never run again.  This procedure releases ownership of any
;	LPT that the VM currently owns.
;
;   ENTRY:
;	EBX = Handle of terminated VM
;
;   EXIT:
;	Carry clear (no error)
;
;   USES:
;       Flags
;
;==============================================================================

BeginProc VPD_VM_Not_Executeable
ifdef MAXDEBUG
	Trace_Out "VPD_VM_Not_Executable (vm=#EBX)"
endif

IFDEF NEC_98
	mov	ecx, [VJPD_Max_Virt_LPTs]	; ECX = Max number of LPTs
ELSE ;NEC_98
	mov	ecx, VPD_Max_Virt_LPTs		; ECX = Max number of LPTs
ENDIF ;NEC_98
	mov	esi, OFFSET32 VPD_LPT1		; ESI -> First LPT structure
VPD_Destroy_Loop:
	cmp	ebx, [esi.VPD_Owner]		; Q: Owned by dead VM?
	jne	SHORT VPD_Destroy_Next		;    N: Try the next one
IFDEF NEC_98
	mov	[VPD_KILL_VM],-1		; kill Bit On
ENDIF ;NEC_98
	call	VPD_Detach			;    Y: Not anymore!
IFDEF NEC_98
	mov	[VPD_KILL_VM],0			; kill Bit On
ENDIF ;NEC_98
	cmp	[esi.VPD_NotifyProc],0
	je	VPD_Destroy_Next
	cCall	VPD_Acquire_Resource,<ACQUIRE_RESOURCE,esi, \
		[esi.VPD_Alt_NotifyProc], [esi.VPD_Alt_NotifyRefData], \
		0>

	cCall	[esi.VPD_NotifyProc],<[esi.VPD_NotifyRefData],1>

VPD_Destroy_Next:
	add	esi, SIZE VPD_Struc		; ESI -> Next LPT structure
	loopd	VPD_Destroy_Loop		; Loop for all LPTs

	ret

EndProc VPD_VM_Not_Executeable

if	WPS
;******************************************************************************
;
;   VJPD_System_Exit
;
;   DESCRIPTION:
;	The "System_Exit" control call indicates that Windows is exiting,
;	normally or via a crash.  Interrupts are enabled.
;
;	If a printer on any LPT port was left in Jumbo mode, switch it
;	back to PCL mode now.
;
;   ENTRY:
;	none
;
;   EXIT:
;	none
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VJPD_System_Exit
ifdef MAXDEBUG
	Trace_Out "VJPD_System_Exit"
endif

ifdef NEC_98
	mov	ecx, [VJPD_Max_Virt_LPTs]	; ECX = Max number of LPTs
else ;NEC_98
	mov	ecx, VPD_Max_Virt_LPTs 		; ECX = Max number of LPTs
endif ;NEC_98
	mov	esi, OFFSET32 VPD_LPT1 		; ESI -> First LPT structure

VJPD_System_Exit_Loop:
	bt	[esi.VPD_Flags], VJPDF_Jumbo_Bit
	jnc	SHORT VJPD_System_Exit_Next
	call	VJPD_Jumbo_PCL_Mode_Switch
VJPD_System_Exit_Next:
	add	esi, SIZE VPD_Struc		; ESI -> Next LPT structure
	loopd	VJPD_System_Exit_Loop		; Loop for all LPTs

ifdef NEC_98
	push	eax
	mov	edx, LPT2_Base+Low_TOKI_CONTROL
	in	al, dx
	cmp	al, 0FFh				;Option Board Support?
	je	SHORT No_Op_Board			;No.
	mov	eax, [Status_Op_Board]			;eax=[54E,549,D4E,D49]
	mov	ecx, 8
	mov	edx, LPT3_Base+Low_TOKI_CONTROL
	out	dx, al					;Restore [D49h]
	shr	eax, cl					;eax=[0,54E,549,D4E]
	mov	edx, LPT3_Base+Low_EX_CONTROL
	out	dx, al					;Restore [D4Eh]
	shr	eax, cl					;eax=[0,0,54e,549]
	mov	edx, LPT2_Base+Low_TOKI_CONTROL
	out	dx, al					;Restore [549h]
	shr	eax, cl					;eax=[0,0,0,54E]
	mov	edx, LPT2_Base+Low_EX_CONTROL
	out	dx, al					;Restore [54Eh]
No_Op_Board:
	pop	eax
	push	edx				; Save EDX
	push	esi				; Save ESI
	mov	edx, 035h			; Set Trap Port Addr
	mov	esi, [VPPIDReg]			; Set VPPID Set Reg #
	VxDCall	VPPID_Remove_IO_Handle		; Clear Trap routine Addr
	pop	esi				; Restore ESI
	pop	edx				; Restore EDX
endif ;NEC_98
	ret

EndProc VJPD_System_Exit

endif ; WPS

IFDEF NEC_98
;******************************************************************************
;	add for system port(35h,37h)
;******************************************************************************

;******************************************************************************
;
;   VPD_Sys_Port
;
;   Description: NEC PC-9800 series System port(35h,37h) must be trapped by
;		VPD. Windows 95 VPPID.VxD trap this port, and by VxD_call to
;		this VPD service routin.
;
;		35h -> Call(In/Out)
;		37h -> Call(Out)
;
;   ENTRY:  AL = value for output
;	   EBX = VM handle
;	   EDX = Port addres(35h or 37h)
;	   ECX = 0:byte in
;	         4:byte out
;
;   EXIT:   AL = value for input
;	   EBX = VM andle
;	   EDX = Port addres(=35h)
;	   ECX = 0:byte in
;	         4:byte out
;
;   USES:
;
;******************************************************************************
BeginProc VPD_Sys_Port
	push	esi
	mov	esi, OFFSET32 VPD_LPT1		; ESI -> LPT data structure

	cmp	edx, 37h
	jne	SHORT VPD_Port_35
;*************************************************************
;   VPD_Out_Port_37
;	If data(AL) is PSTB then Test_And_Set_Owner and out,
;	else ignore.
;*************************************************************
VPD_Out_Port_37:
	cmp	al, 0Ch				; Q: PSTB OFF?
	je	SHORT @f			;  Yes.
	cmp	al, 0Dh				; Q: PSTB ON?
	jne	SHORT VPD_37_Trap_End		;  No.
@@:
	push	ecx
	xor	ecx,ecx
	call	VPD_Test_And_Set_Owner		; Q: Does current VM own port?
	pop	ecx
	jc	SHORT VPD_37_Trap_End		;   No, then ignore output

        out     dx,al				; If owner of lpt then output

VPD_37_Trap_End:
	pop	esi
	clc
	ret

VPD_Port_35:
	jecxz	short VPD_In_Port_35		; if (in al,35h) then jmp
	jmp	short VPD_Out_Port_35		; if (out al,35h) then jmp
VPD_In_Port_35:
	mov	ah, al				; Save al
	in	al, dx				; Get Physical PSTB Status
	and	ax, 0BF40h			; Clear PSTB and Calc PSTB bit
	or	al, ah				; Set Physical PSTB bit
	jmp	short VPD_35_Trap_End

;******************************************************************************
;   VPD_Out_Port_35
;	Output to System port 35h : (1)check if VM own LPT  (2)if can own do
;	nothing ; else (cannot own) set(reset) bit6 of AL to curr 35h value.
;******************************************************************************
VPD_Out_Port_35:

	push	eax
	mov	ah, al				; ah = output value
	in	al, dx				; al = current 35h value
	and	ax, 4040h			; mask
	cmp	ah, al
	pop	eax
	je	SHORT VPD_35_Trap_End

	push	ecx
	xor	ecx,ecx
	call	VPD_Test_And_Set_Owner		; Q: Does current VM own port?
	pop	ecx
	jc	SHORT VPD_35_Trap_End		;   No, then ignore output

	push	eax				; Save output value
	bt	eax, 6				; Q: PSTB OFF
	mov	eax, 0Ch			;  Set PSTB OFF value
	jnc	short @f			;  Yes.
	inc	eax				;  No. Set PSTB ON value(0Dh)
@@:
	out	37h, al				; Output PSTB val
	pop	eax				; Restore output value

VPD_35_Trap_End:
	pop	esi
	clc
	ret

EndProc VPD_Sys_Port
ENDIF ;NEC_98

;******************************************************************************
;  A P I   H A N D L E R S   F O R   W I N D O W S   C O N T R O L   P A N E L
;			    A N D   S P O O L E R
;******************************************************************************

VxD_DATA_SEG

VPD_API_Call_Table LABEL DWORD
	dd	OFFSET32 VPD_PM_API_Get_Ver		; 0
	dd	OFFSET32 VPD_PM_API_Get_Port_Array	; 1
	dd	OFFSET32 VPD_PM_Get_Port_Behavior	; 2
	dd	OFFSET32 VPD_PM_Set_Port_Behavior	; 3
	dd	OFFSET32 VPD_PM_Aquire_Port		; 4
	dd	OFFSET32 VPD_PM_Release_Port		; 5

if	WPS
	dd	0					; 6
	dd	0					; 7
	dd	0					; 8
	dd	0					; 9
	dd	0					; 10
	dd	0					; 11
	dd	0					; 12
	dd	0					; 13
	dd	0					; 14
	dd	OFFSET32 VJPD_PM_SetNonWPSModeSwitch	; 15
	dd	OFFSET32 VJPD_PM_Jumbo_Acquire_Port	; 16
	dd	OFFSET32 VJPD_PM_Jumbo_Release_Port	; 17
	dd	OFFSET32 VJPD_PM_Get_True_Status	; 18
endif ; WPS


VPD_Max_API_Function = ($ - VPD_API_Call_Table) / 4 - 1

IFNDEF NEC_98
VPD_Jmp_Table	LABEL	DWORD
	dd	OFFSET32 VPD_Read_Phys_Port	; Read data port
	dd	OFFSET32 VPD_Out_Owner_Only	; Write data port
	dd	OFFSET32 VPD_Read_Status	; Read status port
	dd	OFFSET32 VPD_Out_Owner_Only	; Write status port
	dd	OFFSET32 VPD_Read_Phys_Port	; Read control port
	dd	OFFSET32 VPD_Out_Owner_Only	; Write control port
ELSE ;NEC_98
VPD_Jmp_Table	LABEL	DWORD
	dd	OFFSET32 VPD_Read_Phys_Port	; Port = 40h : In
	dd	OFFSET32 VPD_Out_Owner_Only	; Port = 40h : Out
	dd	OFFSET32 VPD_Read_Status	; Port = 42h : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 42h : Out
	dd	OFFSET32 VPD_Read_Phys_Port	; Port = 44h : In
	dd	OFFSET32 VPD_Out_Owner_Only	; Port = 44h : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 46h : In
	dd	OFFSET32 VPD_Out_Owner_Only	; Port = 46h : Out
	dd	OFFSET32 VPD_Read_Phys_Port	; Port = 448h : In   for H98
	dd	OFFSET32 VPD_Out_EX_Port	; Port = 448h : Out  for H98

VPD_BI_Jmp_Table	LABEL	DWORD
	dd	OFFSET32 VPD_BI_Read_Phys_Port	; Port = 140h : In
	dd	OFFSET32 VPD_BI_Out_Owner_Only	; Port = 140h : Out
	dd	OFFSET32 VPD_BI_Read_Status	; Port = 141h : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 141h : Out
	dd	OFFSET32 VPD_BI_Control_Read	; Port = 142h : In
	dd	OFFSET32 VPD_BI_Out_Owner_Only	; Port = 142h : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 143h : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 143h : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 144h : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 144h : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 145h : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 145h : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 146h : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 146h : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 147h : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 147h : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 148h : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 148h : Out
	dd	OFFSET32 VPD_BI_TOKI_Read	; Port = 149h : In
	dd	OFFSET32 VPD_BI_TOKI_Set	; Port = 149h : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 14Ah : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 14Ah : Out
	dd	OFFSET32 VPD_BI_TOKI_Status	; Port = 14Bh : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 14Bh : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 14Ch : In
	dd	OFFSET32 VPD_BI_Out_Owner_Only	; Port = 14Ch : Out
	dd	OFFSET32 VPD_IO_Ignore		; Port = 14Dh : In
	dd	OFFSET32 VPD_IO_Ignore		; Port = 14Dh : Out
	dd	OFFSET32 VPD_BI_EX_TOKI_Read	; Port = 14Eh : In
	dd	OFFSET32 VPD_BI_EX_TOKI_Set	; Port = 14Eh : Out
ENDIF ;NEC_98

VxD_DATA_ENDS


;******************************************************************************
;
;   VPD_PM_API_Handler
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = Handle of VM that called API
;	EBP -> Client register structure
;
;   EXIT:
;	Client registers and flags may be altered
;
;   USES:
;	All registers and flags
;
;==============================================================================

BeginProc VPD_PM_API_Handler

	VMMcall Test_Sys_VM_Handle		; Q: Called from Windows?
	jne	SHORT VPD_PM_API_Failed 	;    N: Fail it

	movzx	eax, [ebp.Client_DX]
	cmp	eax, VPD_Max_API_Function	; Q: Is this a valid function?
	ja	SHORT VPD_PM_API_Failed

if	WPS
        mov     ecx, VPD_API_Call_Table[eax*4]	; Q: is this an unused table entry
        jecxz   SHORT VPD_PM_API_Failed		;  Y: do not use it
endif ; WPS

	CallRet VPD_API_Call_Table[eax*4]

VPD_PM_API_Failed:
ifdef MAXDEBUG
	Trace_Out "Failed"
endif
	or	[ebp.Client_EFlags], CF_Mask
	ret

VPD_PM_API_Success:
ifdef MAXDEBUG
	Trace_Out "Success"
endif
	and	[ebp.Client_EFlags], NOT CF_Mask
	ret

EndProc VPD_PM_API_Handler


;******************************************************************************
;
;   VPD_PM_API_Get_Ver
;
;   DESCRIPTION:
;	Return the VPD version to Windows control panel.
;
;   ENTRY:
;	Client_DX = 0
;
;   EXIT:
;	Client_AX = 300h
;	Client_DX = 0CB01h
;	Client carry flag clear
;
;   USES:
;	EAX, Flags, Client_AX, Client_Flags
;
;==============================================================================

BeginProc VPD_PM_API_Get_Ver
ifdef MAXDEBUG
	Trace_Out "VPD_PM_API_Get_Ver"
endif

IFNDEF NEC_98
if	WPS
        mov     [ebp.Client_AX], 30Bh
        mov     [ebp.Client_DX], 0CB01h
else
	mov	[ebp.Client_AX], 300h
endif ; WPS
ELSE ;NEC_98
if	WPS
        mov     [ebp.Client_AX], 30Bh
        mov     [ebp.Client_DX], 0CB01h
else  ; WPS
	mov	[ebp.Client_AX], 30Ah
endif ; WPS
ENDIF ;NEC_98

	jmp	VPD_PM_API_Success

EndProc VPD_PM_API_Get_Ver


;******************************************************************************
;
;   VPD_PM_API_Get_Port_Array
;
;   DESCRIPTION:
;	This API returns a bit array in the Client_AX register that indicates
;	which LPT ports exists.
;
;   ENTRY:
;	Client_DX = 1
;
;   EXIT:
;	Client_AX = Bit array of valid ports.
;	    Bit 0 = 1 if LPT1 exists
;	    Bit 1 = 1 if LPT2 exists
;	    Bit 2 = 1 if LPT3 exists
;	    etc.
;	Client carry flag clear (no error possible).
;
;   USES:
;	EAX, ECX, ESI, Flags, Client_AX, Client_Flags
;
;==============================================================================

BeginProc VPD_PM_API_Get_Port_Array

	mov	esi, OFFSET32 VPD_LPT1
	xor	eax, eax
	mov	ecx, eax
VPD_GPA_Loop:
	cmp	[esi.VPD_IO_Base], 0
	je	SHORT VPD_GPA_Next_LPT
	bts	eax, ecx
VPD_GPA_Next_LPT:
	add	esi, SIZE VPD_Struc
	inc	ecx
IFDEF NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]
ELSE ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs
ENDIF ;NEC_98
	jb	VPD_GPA_Loop

	mov	[ebp.Client_AX], ax
ifdef MAXDEBUG
	Trace_Out "VPD_PM_API_Get_Port_Array (#EAX)"
endif
	jmp	VPD_PM_API_Success

EndProc VPD_PM_API_Get_Port_Array


;******************************************************************************
;
;   VPD_PM_Get_Port_Behavior
;
;   DESCRIPTION:
;	This API will return the current contention behavior value for the
;	specified LPT port.
;
;   ENTRY:
;	Client_DX = 2
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid
;	else
;	    Client_BX:Client_AX = Port behavior value
;		-1 = Always warn on contenetion (never auto assign)
;		0 = Never warn on contention (always auto assign)
;		Other = Number of milliseconds to delay before auto assign OK
;
;   USES:
;	EAX, ECX, Client_BX, Client_AX, Client_Flags,
;
;==============================================================================

BeginProc VPD_PM_Get_Port_Behavior
ifdef MAXDEBUG
	Trace_Out "VPD_PM_Get_Port_Behavior"
endif

	movzx	ecx, [ebp.Client_CX]
IFDEF NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]
ELSE ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs
ENDIF ;NEC_98
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	add	ecx, OFFSET32 VPD_LPT1
	mov	eax, [ecx.VPD_Timeout_Limit]
	mov	[ebp.Client_AX], ax
	shr	eax, 16
	mov	[ebp.Client_BX], ax
	jmp	VPD_PM_API_Success

EndProc VPD_PM_Get_Port_Behavior

;******************************************************************************
;
;   VPD_PM_Set_Port_Behavior
;
;   DESCRIPTION:
;	This API will modify the contention behavior value for the sepecified
;	LPT port.
;
;   ENTRY:
;	Client_DX = 3
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;	Client_BX:Client_AX = Port behavior value
;	    -1 = Always warn on contenetion (never auto assign)
;	    0 = Never warn on contention (always auto assign)
;	    Other = Number of milliseconds to delay before auto assign OK
;
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid
;	else
;	    Port behavior modified
;
;   USES:
;	EAX, ECX, Client_BX, Client_AX, Client_Flags,
;
;==============================================================================

BeginProc VPD_PM_Set_Port_Behavior
ifdef MAXDEBUG
	Trace_Out "VPD_PM_Set_Port_Behavior"
endif

	movzx	ecx, [ebp.Client_CX]
IFDEF NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]
ELSE ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs
ENDIF ;NEC_98
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	add	ecx, OFFSET32 VPD_LPT1
	mov	ax, [ebp.Client_BX]
	shl	eax, 16
	mov	ax, [ebp.Client_AX]
	mov	[ecx.VPD_Timeout_Limit], eax
	jmp	VPD_PM_API_Success

EndProc VPD_PM_Set_Port_Behavior


;******************************************************************************
%OUT Document me!

BeginProc VPD_Acquire_Port

	jnz	SHORT VPD_AP_Force_Owner

ifdef MAXDEBUG
	Trace_Out "VPD_Acquire_Port"
endif
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, [esi.VPD_LPT_Number]	; ECX = LPT number
	push	DWORD PTR [eax]
	bts	DWORD PTR [eax], ecx		; Fake contention
	call	VPD_Test_And_Set_Owner
	pop	DWORD PTR [eax]
	jc	VPD_AP_Done

; need-work, if the Test_And_Set_Owner above succeeds, VPD_Attach has already
; been called, this then falls through and calls it again. This path
; does not introduce any bugs, but it can cause redundant changing of the
; state of 'trapping', is that an efficiency problem ?

VPD_AP_Force_Owner:
ifdef MAXDEBUG
	Trace_Out "VPD_Acquire_Port, attaching"
endif
	xor	ecx, ecx
	dec	ecx				; "Special" ownership will turn
	call	VPD_Attach			; off trapping of ALL ports
	clc

VPD_AP_Done:
	ret

EndProc VPD_Acquire_Port

;******************************************************************************
;
;   VPD_Enable_Trapping
;
;   DESCRIPTION:
;
;   ENTRY:	    ESI -> VPD_Struc
;
;   EXIT:	    Carry set, if port isn't physical
;
;   USES:	    ECX, EDX, Flags
;
;==============================================================================
BeginProc VPD_Enable_Trapping

	mov	edx, [esi.VPD_IO_Base]
	cmp	edx, 2
	jb	short vet_exit
	mov	ecx, VPD_Num_Ports_Per_LPT
ifdef NEC_98
	cmp	edx, LPT1_Base + Low_AT_DATA
	jne	SHORT VPD_ET_NOT_140h
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support?
	End_Touch_1st_Meg
	jz	SHORT VPD_ET_Trap98		; No,then skip
VPD_ET_NOT_140h:
endif ;NEC_98
@@:
	VMMCall Enable_Global_Trapping
	inc	edx
	loop	@B

ifdef NEC_98
	;TOKI PORT
	mov	dl,Low_TOKI_STATUS	;edx=0x4bh
        VMMCall Enable_Global_Trapping
	mov	dl,Low_FIFO_DATA	;edx=0x4ch
        VMMCall Enable_Global_Trapping
	mov	dl,Low_EX_CONTROL	;edx=0x4eh
        VMMCall Enable_Global_Trapping

	;98 PORT
        cmp     [esi.VPD_LPT_Number],1	;LPT1?
	jne	SHORT VPD_ET98_end	;No.
VPD_ET_Trap98:
	mov	edx, 40h		;edx=040h
        mov     ecx, 3
@@:
        VMMCall Enable_Global_Trapping	;Trap Port (40h,42h,44h)
        inc     edx
        inc     edx
        loop    @B
VPD_ET98_end:
endif ;NEC_98

	or	[esi.VPD_Flags], VPDF_Enabled
	clc
vet_exit:
	ret

EndProc VPD_Enable_Trapping


;******************************************************************************
;
;   VPD_Disable_Trapping
;
;   DESCRIPTION:
;
;   ENTRY:	    ESI -> VPD_Struc
;
;   EXIT:	    Carry set, if port isn't physical
;
;   USES:	    ECX, EDX, Flags
;
;==============================================================================
BeginProc VPD_Disable_Trapping

	mov	edx, [esi.VPD_IO_Base]
	cmp	edx, 2
	jb	short vdt_exit
	mov	ecx, VPD_Num_Ports_Per_LPT
ifdef NEC_98
	cmp	edx, LPT1_Base + Low_AT_DATA
	jne	SHORT VPD_DT_NOT_140h
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support?
	End_Touch_1st_Meg
	jz	SHORT VPD_DT_Trap98		; No,then skip
VPD_DT_NOT_140h:
endif ;NEC_98
@@:
	VMMCall Disable_Global_Trapping
	inc	edx
	loop	@B

ifdef NEC_98
        cmp     [esi.VPD_LPT_Number],1	;LPT1?
	jne	SHORT VPD_DT98_end	;No.
VPD_DT_Trap98:
	mov	edx, 40h		;edx=040h
        mov     ecx, 3
@@:
        VMMCall Disable_Global_Trapping	;Trap Port (40h,42h,44h)
        inc     edx
        inc     edx
        loop    @B
VPD_DT98_end:
endif ;NEC_98

	and	[esi.VPD_Flags], NOT VPDF_Enabled
	clc
vdt_exit:
	ret

EndProc VPD_Disable_Trapping

;******************************************************************************
;
;   VPD_PM_Aquire_Port
;
;   DESCRIPTION:
;
;   ENTRY:
;	Client_DX = 4
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;	Client_AX = Flags
;		    Bit 0 = 1 if port should be assigned regardless of current
;			    ownership
;		    All other flags must be zero
;
;   EXIT:
;
;
;   USES:
;
;==============================================================================

BeginProc VPD_PM_Aquire_Port
ifdef MAXDEBUG
	Trace_Out "VPD_PM_Acquire_Port (vm=#EBX)"
endif

	movzx	ecx, [ebp.Client_CX]
IFDEF NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]
ELSE ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs
ENDIF ;NEC_98
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

if	WPS
	TestMem	[esi.VPD_Flags], VJPDF_Jumbo
	jz	@F
	VMMCall	Get_Cur_VM_Handle
	cmp	[esi.VPD_Owner], ebx		; Q: Does this VM own it?
	je	VPD_PM_API_Success		;   Y: no switch needed
@@:
endif

	test	[ebp.Client_AL], 1
	call	VPD_Acquire_Port

	jnc	VPD_PM_API_Success
	jc	VPD_PM_API_Failed

EndProc VPD_PM_Aquire_Port


;******************************************************************************
;
;   VPD_PM_Release_Port
;
;   DESCRIPTION:
;
;   ENTRY:
;	Client_DX = 5
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VPD_PM_Release_Port
ifdef MAXDEBUG
	Trace_Out "VPD_PM_Release_Port (vm=#EBX)"
endif

	movzx	ecx, [ebp.Client_CX]
IFDEF NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]
ELSE ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs
ENDIF ;NEC_98
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

	cmp	[esi.VPD_Owner], ebx
	jne	VPD_PM_API_Failed
	call	VPD_Detach
IFDEF NEC_98
	mov	edx, 46h			; I/O Start adress = 40h
	mov	ecx, VPD_Num_Ports_Per_LPT-2	; ECX = 2 Number of ports to hook
VPD_EN_Loop:					;
	VMMcall Enable_Local_Trapping		; Enable Local port trapping
	mov	edx, 448h			; printer expand port
	loopd	VPD_EN_Loop			; Loop until all Disabled

	mov	ecx,3				; 3 Times Loop
	mov	edx,140h			; 
VPD_PM_BI_ITrap_EN:
	VMMcall Enable_Local_Trapping		; Enable Local port trapping
	inc	edx
	loopd	VPD_PM_BI_ITrap_EN

	mov	edx,149h			; 
	VMMcall Enable_Local_Trapping		; Enable Local port trapping

	mov	edx,14Bh			; 
	VMMcall Enable_Local_Trapping		; Enable Local port trapping

	mov	edx,14Ch			; 
	VMMcall Enable_Local_Trapping		; Enable Local port trapping

	mov	edx,14Eh			; 
	VMMcall Enable_Local_Trapping		; Enable Local port trapping

ENDIF ;NEC_98
	jmp	VPD_PM_API_Success

EndProc VPD_PM_Release_Port

if	WPS
;******************************************************************************
;
;   VJPD_PM_SetNonWPSModeSwitch
;
;   DESCRIPTION:
;	Set the non-WPS mode switch string.
;
;   ENTRY:
;	EBX = Windows VM
;	Client_DX = 15
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;	Client_AX = Non-WPS mode switch string length
;	Client_ES:Client_DI = Non-WPS mode switch string
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid, not a real port
;
;   USES:
;
;==============================================================================

BeginProc VJPD_PM_SetNonWPSModeSwitch

	movzx	ecx, [ebp.Client_CX]		    ; ECX = Port index
ifdef MAXDEBUG
	Trace_Out "VJPD_PM_SetNonWPSModeSwitch (VM=#EBX, LPT=#CX)"
endif
ifdef NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]    	    ; Q: Port index valid?
else ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs 	    	    ; Q: Port index valid?
endif ;NEC_98
	jae	VPD_PM_API_Failed			    ;	N: Return failure
						    ;	Y: Continue
	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]		    ; ESI -> LPT structure

	movzx	ecx, [ebp.Client_AX]		    ; ECX = Mode switch size
	cmp	ecx, [esi.VJPD_Non_WPS_Mode_Size]   ; Q: Size changed?
	jne	SHORT @F			    ;	Y: Free the buffer
	or	ecx, ecx			    ; Q: Size zero?
	jz	VPD_PM_API_Success 		    ;	Y: Return success
	jmp	SHORT VJPD_SetNWMS_Copy 	    ;	N: Copy new mode switch
@@:
	mov	eax, [esi.VJPD_Non_WPS_Mode]	    ; EAX = Buffer address
	or	eax, eax			    ; Q: Buffer allocated?
	jz	SHORT @F			    ;	N: Don't free the buffer
	VMMCall _HeapFree, <EAX, 0>		    ;	Y: Free the buffer
	mov	[esi.VJPD_Non_WPS_Mode], 0	    ; Zero the buffer address
@@:
	mov	[esi.VJPD_Non_WPS_Mode_Size], ecx   ; Save new mode size
	or	ecx, ecx			    ; Q: Size zero?
	jz	VPD_PM_API_Success 		    ;	Y: Return success

	push	ecx				    ; Save buffer size
	VMMCall _HeapAllocate, <ECX, HeapZeroInit>  ; Allocate new buffer
	pop	ecx				    ; Restore buffer size
	or	eax, eax			    ; Q: Buffer OK?
	jnz	SHORT @F			    ;	Y: Continue
	mov	[esi.VJPD_Non_WPS_Mode_Size], 0     ;	N: Mark as no buffer
	jmp	VPD_PM_API_Failed			    ;	   and return failure
@@:
	mov	[esi.VJPD_Non_WPS_Mode], eax	    ; Save buffer address

	; Copy the new mode switch command

VJPD_SetNWMS_Copy:

ifdef MAXDEBUG
	Trace_Out "Copying non-WPS mode switch string"
endif
	mov	edi, [esi.VJPD_Non_WPS_Mode]	    ; EDI -> buffer

	mov	ax, (Client_ES SHL 8) + Client_DI   ; Map source address
	VMMCall Map_Flat
	cmp	eax, -1 			    ; Q: Mapping OK?
	jne	SHORT @F			    ;	Y: Continue
						    ;	N: Free the buffer
	mov	eax, [esi.VJPD_Non_WPS_Mode]	    ; EAX = Buffer address
	VMMCall _HeapFree, <EAX, 0>
	mov	[esi.VJPD_Non_WPS_Mode], 0	    ; Zero the buffer address
	mov	[esi.VJPD_Non_WPS_Mode_Size], 0     ; Zero the buffer size
	jmp	VPD_PM_API_Failed		    ; Return failure
@@:
	mov	esi, eax			    ; ESI = Source address
	cld					    ; Clear the direction
	rep	movsb				    ; Copy the string
	jmp	VPD_PM_API_Success 		    ; Return success

EndProc VJPD_PM_SetNonWPSModeSwitch

;******************************************************************************
;
;   VJPD_PM_Jumbo_Acquire_Port
;
;   DESCRIPTION:
;	Acquires the port for the Jumbo software ONLY if the port is
;	unowned or the owner has timed out.
;
;	DON'T succeed if Jumbo already owns the port, otherwise, two
;	Jumbo tasks (INSTALL and QP) may think they own the port, use
;	it, and call Jumbo_Release_Port so that neither owns it.
;
;   ENTRY:
;	Client_DX = 16
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;	Client_AX = 0
;
;   EXIT:
;	If client carry set then
;	    ERROR:	Port index is invalid, or
;			Port in use by someone else
;	else
;	    Port acquired
;
;
;   USES:
;
;==============================================================================

BeginProc VJPD_PM_Jumbo_Acquire_Port

	movzx	ecx, [ebp.Client_CX]
ifdef NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]
else ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs
endif ;NEC_98
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

ifdef MAXDEBUG
	mov	eax, [esi.VPD_Flags]
	Trace_Out "VJPD_PM_Jumbo_Acquire_Port (VM=#EBX, LPT=#ECX, Flags=#EAX)"
endif

	; If mode switch to PCL in progress, disallow this acquire
	TestMem	[esi.VPD_Flags], VJPDF_PCLSwitch
	jnz	VPD_PM_API_Failed

	; If COMM OR JUMBO(!!) already own it, fail
	TestMem	[esi.VPD_Flags], VPDF_Special_Owner
	jnz	VPD_PM_API_Failed

	; ensure that TSO will not go to user for contention
	cmp	[esi.VPD_Owner], 0		; Q: Does any VM own printer?
	je	SHORT VPD_Jumbo_OK_To_Assign	;    N: Then ok to acquire

	cmp	[esi.VPD_Timeout_Limit], -1	; Q: Can we time-out?
	je	SHORT VPD_Jumbo_OK_To_Assign	;    N: Then MUST ask user
						;    Y: See if it happened
	VMMCall Get_System_Time 		; EAX = Current system time
	sub	eax, [esi.VPD_Last_Touched]	; EAX = Elapsed time
	cmp	eax, [esi.VPD_Timeout_Limit]	; Q: Has owner timed-out?
	jb	VPD_PM_API_Failed		;    N: keep waiting
						;    Y: OK to change owners
VPD_Jumbo_OK_To_Assign:
	xor	al,al			; set Z => Don't force ownership
	or	[esi.VPD_Flags], VJPDF_JumboAcq
	call	VPD_Acquire_Port
	jnc	@F
	and	[esi.VPD_Flags], NOT VJPDF_JumboAcq
	jmp	VPD_PM_API_Failed
@@:
	and	[esi.VPD_Flags], NOT VJPDF_JumboAcq
	or	[esi.VPD_Flags], VJPDF_Jumbo
	jmp	VPD_PM_API_Success

EndProc VJPD_PM_Jumbo_Acquire_Port


;******************************************************************************
;
;   VJPD_PM_Jumbo_Release_Port
;
;   DESCRIPTION:
;	Give up Jumbo's ownership of a port, leaving the Jumbo_Owned
;	flag on to warn the next non-Jumbo owner that it has to switch
;	the printer back to PCL mode.
;
;   ENTRY:
;	Client_DX = 17
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid / port unacquired
;	else
;	    Port released
;
;   USES:
;
;==============================================================================

BeginProc VJPD_PM_Jumbo_Release_Port

	movzx	ecx, [ebp.Client_CX]
ifdef NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]
else ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs
endif ;NEC_98
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

ifdef MAXDEBUG
	mov	eax, [esi.VPD_Flags]
	Trace_Out "VJPD_PM_Jumbo_Release_Port (VM=#EBX, LPT=#ECX, Flags=#EAX)"
endif

	TestMem	[esi.VPD_Flags], VJPDF_Jumbo
	jz	VPD_PM_API_Failed

	; need-work, while jumbo owns port, VPD_PM_Acquire_Port & Release
	; are executed causing the Special owner bit and actual ownership
	; to be lost
	; test	[esi.VPD_Flags], VPDF_Special_Owner
	; jz	VPD_PM_API_Failed

	call	VPD_Detach
	jmp	VPD_PM_API_Success

EndProc VJPD_PM_Jumbo_Release_Port


;******************************************************************************
;
;   VJPD_PM_Get_True_Status
;
;   DESCRIPTION:
;	Reads the status register without affecting ownership in any way.
;
;   ENTRY:
;	EBX = Windows VM
;	Client_DX = 18
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid, not a real port
;	else
;	    Client_AX = True Status of LPT Port
;
;   USES:
;
;==============================================================================

BeginProc VJPD_PM_Get_True_Status
	movzx	ecx, [ebp.Client_CX]
ifdef MAXDEBUG
	Trace_Out "VJPD_PM_Get_True_Status (VM=#EBX, LPT=#CX)"
endif
ifdef NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]
else ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs
endif ;NEC_98
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

	mov	edx, [esi.VPD_IO_Base]
ifdef NEC_98
	cmp	[esi.VPD_LPT_Number], 1			;LPT1
	jne	SHORT VJPD_GTS_Physical_Port		;No.
	cmp	edx, LPT1_Base+Low_AT_DATA
	jne	SHORT @f
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h
	End_Touch_1st_Meg
	jz	SHORT VJPD_GTS_Physical_Port98
@@:
	mov	dl, Low_TOKI_CONTROL
	in	al, dx
	mov	dl, Low_AT_DATA
	test	al, AT_MODE
	jnz	SHORT VJPD_GTS_Physical_Port

VJPD_GTS_Physical_Port98:
	mov	dx, 42h
	in	al, dx
	mov	ah, al
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h		; H98?
	End_Touch_1st_Meg
	jz	SHORT VJPD_GTS_Semi
	mov	dx, 448h
	in	al, dx
	test	al, 01h
	jnz	SHORT VJPD_GTS_Full
VJPD_GTS_Semi:
	or	ah, 61h
	and	ah, 65h
VJPD_GTS_Full:
	mov	al,ah	     			;	 ____  _ _
	and 	ah,11110101b 			;	 SFP5|0B0A
						;        ____  __
	inc	ah	     			;	 SFP5|0BA?
						;         ___ _ __
	shr	ah,1	     			;	 0SFP|50BA
						;	 _ __ __ _
	ror	ah,1	     			;	 A0SF|P50B
						;	 __ _ ___ 
	ror	ah,1	     			;	 BA0S|FP50
	and 	ah,11011010b
						;        __ _ _ _
						;	 BA0S|F050    _
	and	al,00100000b  			;	 ____ _	_   00P0|0000
	or	al,ah				;	 BAPS|F050
	xor	al,00110010b
						;	 __   _
						;	 BAPS|F050
	jmp	SHORT VJPD_GTS_SET_AX
else ;NEC_98
	test	dh, dh
	jnz	SHORT VJPD_GTS_Physical_Port
	jmp	VPD_PM_API_Failed
endif ;NEC_98

VJPD_GTS_Physical_Port:
	inc	dx
	in	al, dx
ifdef NEC_98
VJPD_GTS_SET_AX:
endif ;NEC_98
	xor	ah, ah
	mov	[ebp.Client_AX], ax
	jmp	VPD_PM_API_Success

EndProc VJPD_PM_Get_True_Status

endif ; WPS

;******************************************************************************
;		    I / O   T R A P   P R O C E D U R E S
;******************************************************************************

;******************************************************************************
;
;   VPD_LPTx_Trap
;
;   DESCRIPTON:
;	All printer port traps use the same code.  These entry points set
;	ESI to point to the appropriate LPT data structure.  They then jump
;	to a common I/O trap handler.
;
;   ENTRY:
;	If output then EAX/AX/AL = Data output
;	EBX = Current VM Handle
;	ECX = Type of I/O
;	EDX = Port number
;
;   EXIT:
;	If input then AL = Virtual port value
;
;   USES:
;	EAX, ESI, EDI, Flags
;
;==============================================================================

BeginProc VPD_LPT1_Trap, High_Freq

	mov	esi, OFFSET32 VPD_LPT1
	jmp	SHORT VPD_Common_IO_Trap

EndProc VPD_LPT1_Trap, High_Freq

;------------------------------------------------------------------------------

BeginProc VPD_LPT2_Trap, High_Freq

	mov	esi, OFFSET32 VPD_LPT2
ifdef NEC_98
	jmp	VPD_Common_IO_Trap
else ;NEC_98
	jmp	SHORT VPD_Common_IO_Trap
endif ;NEC_98

EndProc VPD_LPT2_Trap

;------------------------------------------------------------------------------

BeginProc VPD_LPT3_Trap, High_Freq

	mov	esi, OFFSET32 VPD_LPT3
ifdef NEC_98
	jmp	VPD_Common_IO_Trap
else ;NEC_98
	jmp	SHORT VPD_Common_IO_Trap
endif ;NEC_98

EndProc VPD_LPT3_Trap, High_Freq


;******************************************************************************
;
;   VPD_Common_IO_Trap
;
;   DESCRIPTION:
;	This procedure dispatches I/O traps for every I/O port of every
;	LPT.  It jumps through the VPD_Jmp_Table using the following
;	formula to determine which procedure should handle the trap:
;	    (I/O Port MOD 4) * 8 + I/O type
;
;	All non-byte I/O instructions are emulated by the "Emulate_Non_-
;	Byte_IO" macro which will recursively call the trap procedures
;	with simpler I/O types.  See the documentation on I/O trapping
;	for more details about this macro.
;
;   ENTRY:
;	If output then EAX/AX/AL = Data output
;	EBX = Current VM Handle
;	ECX = Type of I/O
;	EDX = Port number
;	ESI -> LPT data structure
;
;   EXIT:
;	If input then AL = Virtual port value
;
;   USES:
;	EAX, EDI, Flags
;
;==============================================================================

;------------------------------------------------------------------------------

;
;   A few paranoid MASM tests here.  This procedure relys on the fact that
;   byte input is 0 and byte output is 4.  All non-byte I/O requests are
;   handled by the "Emulate_Non_Byte_IO" macro (which will recursively
;   call this procedure).
;
.ERRNZ	Byte_Input
.ERRNZ	Byte_Output - 4

BeginProc VPD_Common_IO_Trap, High_Freq

	Emulate_Non_Byte_IO			; Non-byte I/O emulated

	mov	edi, edx			; EDI = trapped port
IFNDEF NEC_98
	and	edi, 11b			; Only lowest two bits matter
	CallRet VPD_Jmp_Table[edi*8][ecx]	; Call appropriate handler and
						; return
ELSE ;NEC_98

	test	edi, 100h			; Bi-Direction Port ?
	jnz	SHORT VPD_BI_Common_IO_Trap	; Yes, then jump

	and	edi, 0Eh			; Only lowest one byte
	CallRet VPD_Jmp_Table[edi*4][ecx]	; Call appropriate handler and
						; return

VPD_BI_Common_IO_Trap:
	test	edi, 0400h			; edi = 54xh ?
	jnz	SHORT @f			; Yes.
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support ?
	End_Touch_1st_Meg
	jz	SHORT VPD_BI_Common_Exit	; No, then jump
@@:	and	edi, 0Fh			; Only lowest one byte
	CallRet VPD_BI_Jmp_Table[edi*8][ecx]	; 
						; 
VPD_BI_Common_Exit:
	ret
ENDIF ;NEC_98

EndProc VPD_Common_IO_Trap


;******************************************************************************
;
;   VPD_Read_Phys_Port
;
;   DESCRIPTION:
;	The VM is reading either the data or control port.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_Read_Phys_Port

	in	al, dx				; Read port into AL
	ret

EndProc VPD_Read_Phys_Port


;******************************************************************************
;
;   VPD_Out_Owner_Only
;
;   DESCRIPTION:
;	The VM is writing data to the printer data or control port.  If the
;	printer is owned by another virtual machine then throw the I/O away.
;	Otherwise, output the data to the physical port.
;
;   ENTRY:
;	 AL = Byte output
;	EDX = I/O port
;
;   EXIT:
;	None
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VPD_Out_Owner_Only, High_Freq

	call	VPD_Test_And_Set_Owner		; Q: Does current VM own port?
	jc	SHORT VPD_OD_Exit		;    N: Ignore output

	out	dx, al				;    Y: Output byte to port
VPD_OD_Exit:					;	and return
	ret

EndProc VPD_Out_Owner_Only


;******************************************************************************
;
;   VPD_Read_Status
;
;   DESCRIPTION:
;	If the current VM owns the LPT or no VM owns the LPT then the actual
;	status is returned.  Otherwise, a "busy" status will be returned.
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Status
;
;   USES:
;	AL, Flags
;
;==============================================================================

BeginProc VPD_Read_Status

IFDEF NEC_98
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jz	SHORT 	VPD_RS_Not_H98		;no,then skip

	call	VPD_Test_Owner		; check this VM own LPT
	jnc	SHORT VPD_Real_Status		; can own then jmp

	push	eax
	push	ecx
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.
	bt	DWORD PTR [eax], ecx		; Q: VM Semicen?
	pop	ecx
	pop	eax

	jnc	SHORT VPD_RS_Semi_Not_Ready

	mov	al, VPD_Ready_Status_Full	; set busy bit (printer ready)
	jmp	SHORT VPD_RS_Exit

VPD_RS_Not_H98:
	call	VPD_Test_Owner		; check this VM own LPT
	jnc	SHORT VPD_Real_Status		; cannot own then jmp
VPD_RS_Semi_Not_Ready:
	in	al, dx				; read status
	or	al, VPD_Ready_Status_Semi	; set busy bit (printer ready)
	jmp	SHORT VPD_RS_Exit
VPD_Real_Status:
	in	al, dx				; read status port
VPD_RS_Exit:
	ret
ELSE ;NEC_98
	in	al, dx				; Read the status into AL
	cmp	[esi.VPD_Owner], 0		; Q: Does no VM own LPT?
	jz	SHORT VPD_RS_Exit		;    Y: Return status in AL
	call	VPD_Test_And_Set_Owner		; Q: Does current VM own LPT?
	jnc	SHORT VPD_RS_Exit		;    Y: Return status in AL
	mov	al, VPD_Busy_Status		;    N: Status = none connected
VPD_RS_Exit:
	ret
ENDIF ;NEC_98

EndProc VPD_Read_Status

IFDEF NEC_98
;******************************************************************************
;
;   VPD_IO_Ignore
;
;   DESCRIPTION:
;	Invalid IO should Ignore.(Out to read only port, In from write only
;	port.)///PC-9800 series only
;
;	+-----------------------------------------------------------+
;	|        PC-9800 series                                     |
;	+-----------------------------------------------------------+
;	|          |  IN   |  OUT                                   |
;	|    ------+-------+---------                               |
;	|    Port# |  37h  |  42h                                   |
;	|          |  46h  |                                        |
;	+-----------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	none
;
;   USES:
;	none
;
;==============================================================================

BeginProc VPD_IO_Ignore

	ret			; Do nothing !

EndProc VPD_IO_Ignore


;******************************************************************************
;
;   VPD_Out_EX_Port
;
;   DESCRIPTION:
;
;	+---------------------------------------------------------+
;	|                 NEC PC-H98 series   ONLY!               |
;	+---------------------------------------------------------+
;	|    Port = 448h    Out                                   |
;       |                                                         |
;	| If Out Port is 448h 					  |
;	|                                                         |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	 AL = Byte output
;	EDX = I/O port
;
;   EXIT:
;	None
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VPD_Out_EX_Port, High_Freq

	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jz	SHORT 	VPD_OEX_Exit		;no,then skip

	push	eax
	push	ecx
	test	al, 01h
	jnz	SHORT VPD_OEX_Full

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.
	btr	DWORD PTR [eax], ecx		; Reset I/F mode bit
	jmp	SHORT VPD_OEX_OUT
VPD_OEX_Full:
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.
	bts	DWORD PTR [eax], ecx		; Set I/F mode bit
VPD_OEX_OUT:
	pop	ecx
	pop	eax

	push	ecx
	xor	ecx, ecx
	call	VPD_Test_And_Set_Owner		; Q: Does current VM own port?
	pop	ecx
	jc	SHORT VPD_OEX_Exit		;    N: Ignore output

	out	dx, al				;    Y: Output byte to port
VPD_OEX_Exit:					;	and return
	ret

EndProc VPD_Out_EX_Port


;******************************************************************************
;
;   VPD_BI_Read_Phys_Port
;
;   DESCRIPTION:
;	The VM is reading either the data or control port.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;	+---------------------------------------------------------+
;	|   NEC PC-9800 series Bi-Direction Support Machine       |
;	+---------------------------------------------------------+
;	|   Port =  140h,142h,14Ch                                |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_BI_Read_Phys_Port

	cmp	edx, AT_DATA1
	jne	short VPD_BI_RD_Status

	test	[VPD_5B3_BYTE],01h		; 14Pin Cable ?
	jz	SHORT VPD_BI_RD_Status		; No, then jump

	mov	al,VPD_TOKI_Kill_Status		; AL <- FFh
	jmp	SHORT VPD_BI_Read_Exit		; Exit

VPD_BI_RD_Status:				; Yes, then jump
	in	al, dx				; Read port into AL

VPD_BI_Read_Exit:
	ret

EndProc VPD_BI_Read_Phys_Port


;******************************************************************************
;
;   VPD_BI_Out_Owner_Only
;
;   DESCRIPTION:
;	The VM is writing data to the printer data or control port.  If the
;	printer is owned by another virtual machine then throw the I/O away.
;	Otherwise, output the data to the physical port.
;
;	+---------------------------------------------------------+
;	|   NEC PC-9800 series Bi-Direction Support Machine       |
;	+---------------------------------------------------------+
;	|   Port =  140h,142h,14Ch                                |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Output Data
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_BI_Out_Owner_Only, High_Freq

	call	VPD_Test_And_Set_Owner		; Does current VM own port?
	jc	SHORT VPD_BI_Out_Exit		;  No, then jump and exit

	out	dx, al				;  Yes, then output byte to port

VPD_BI_Out_Exit:
	ret

EndProc VPD_BI_Out_Owner_Only


;******************************************************************************
;
;   VPD_BI_Read_Status
;
;   DESCRIPTION:
;	The VM is reading AT or PS2 Mode Status.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;	+---------------------------------------------------------+
;	|   NEC PC-9800 series Bi-Direction Support Machine       |
;	+---------------------------------------------------------+
;	|   Port =  141h                                          |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_BI_Read_Status

	call	VPD_Test_Owner			; This VM is own LPT ?
	jnc	SHORT VPD_BI_Real_Status	; Yes, then jump

	cmp	[esi.VPD_LPT_Number], 1		; Q: LPT1?
	jne	SHORT VPD_BI_RS_Full		;  No.

;*** VM is not owner ***

	push	eax
	push	ecx
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.

	bt	DWORD PTR [eax], ecx		; Q: VM Semicen?
	pop	ecx
	pop	eax

VPD_BI_RS_Semi:
	mov	al,VPD_TOKI_Kill_Status		; AL <- FFh

	jnc	SHORT VPD_BI_RS_Exit		; Semi Cen !

VPD_BI_RS_Full:
;	mov	al,VPD_ready_Status_AT		; Set Ready Status
	mov	al,VPD_Busy_Status		; Set Busy Status
	jmp	SHORT VPD_BI_RS_Exit

VPD_BI_Real_Status:
	in	al, dx				; read status port

VPD_BI_RS_Exit:
	ret

EndProc VPD_BI_Read_Status


;******************************************************************************
;
;   VPD_BI_Control_Read
;
;   DESCRIPTION:
;	The VM is reading either the data or control port.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;	+---------------------------------------------------------+
;	|   NEC PC-9800 series Bi-Direction Support Machine       |
;	+---------------------------------------------------------+
;	|   Port =  142h                                          |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_BI_Control_Read

	call	VPD_Test_Owner			; This VM is own LPT ?

	jnc	SHORT VPD_BI_CR_Real_Status	; Yes, then jump

	cmp	[esi.VPD_LPT_Number], 1		; Q: LPT1?
	jne	SHORT VPD_BI_CR_Not_Semi	;  No.

;*** VM is not owner ***

	push	eax
	push	ecx
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.

	bt	DWORD PTR [eax], ecx		; Q: VM Semicen?
	pop	ecx
	pop	eax

VPD_BI_CR_Semi:
	mov	al,VPD_TOKI_Kill_Status		; AL <- FFh

	jnc	SHORT VPD_BI_CR_Exit		; Semi Cen !

VPD_BI_CR_Not_Semi:
	mov	al,VPD_BI_Control_Status	; AL <- 04h
	jmp	SHORT VPD_BI_CR_Exit

VPD_BI_CR_Real_Status:
	in	al, dx				; read status port

VPD_BI_CR_Exit:
	ret

EndProc VPD_BI_Control_Read


;******************************************************************************
;
;   VPD_BI_TOKI_Read
;
;   DESCRIPTION:
;	The VM is reading either the data or control port.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;	+---------------------------------------------------------+
;	|   NEC PC-9800 series Bi-Direction Support Machine       |
;	+---------------------------------------------------------+
;	|   Port =  149h                                          |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_BI_TOKI_Read

	cmp	[ebp.Client_CS], 0D800h		; Q:Suspend resume BIOS Code?
	je	SHORT VPD_BI_TKR_Real_Status	;  Yes.(Don't Contention Check)

        VMMCall Test_Sys_VM_Handle              ; Q:System VM?
	jne	SHORT VPD_BTR_Not_SYS_VM	;  No.
	call	VPD_Test_Owner			; This VM is own LPT ?
	jnc	SHORT VPD_BI_TKR_Real_Status	; Yes, then jump
	jmp	SHORT VPD_BTS_Not_OWNER
VPD_BTR_Not_SYS_VM:
	push	ecx
	xor	ecx,ecx
	call	VPD_Test_And_Set_Owner		; Does current VM own port?
	pop	ecx

	jnc	SHORT VPD_BI_TKR_Real_Status	; Yes, then jump

VPD_BTS_Not_OWNER:
;*** VM is not owner ***

	push	eax
	push	ecx
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.

	bt	DWORD PTR [eax], ecx		; Q: VM Semicen?
	pop	ecx
	pop	eax

	in	al,dx				; Get TOKI Control Port

	jc	SHORT VPD_BI_TKR_Not_Semi	; Not Semi Cen !

VPD_BI_TKR_Semi:
	and	al,VPD_BI_MASK_AT		; Set 98 Bit
	jmp	SHORT VPD_BI_TKR_Exit

VPD_BI_TKR_Not_Semi:
	or	al,VPD_BI_SET_AT		; Set AT
	jmp	SHORT VPD_BI_TKR_Exit		; 

VPD_BI_TKR_Real_Status:
	in	al, dx				; read status port

VPD_BI_TKR_Exit:
	ret

EndProc VPD_BI_TOKI_Read


;******************************************************************************
;
;   VPD_BI_TOKI_Set
;
;   DESCRIPTION:
;	The VM is reading either the data or control port.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;	+---------------------------------------------------------+
;	|   NEC PC-9800 series Bi-Direction Support Machine       |
;	+---------------------------------------------------------+
;	|   Port =  149h                                          |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_BI_TOKI_Set, High_Freq

	cmp	edx, AT_DATA1
	jne	short @f

	test	[VPD_5B3_BYTE],01h		; 14Pin Cable ?
	jnz	SHORT VPD_BI_TK_Set_OUT_TRUE	; Yes, then jump
@@:
	push	eax
	push	ecx

	test	al,VPD_BI_SET_AT		; VM want to set AT ?
	jnz	SHORT VPD_BI_TK_98toAT_Set	; Yes, then jump

VPD_BI_TK_ATto98_Set:
	mov	eax, [VPD_CB_Offset]		; Set Control-Block
	add	eax, ebx			; Cal. VM Control-Block

	mov	ecx, [esi.VPD_LPT_Number]
	add	ecx, 6				; ECX = I/F mode info.
	btr	DWORD PTR [eax], ecx		; Set SEMI Cen. Bit

	jmp	SHORT VPD_BI_TK_Set_OUT

VPD_BI_TK_98toAT_Set:
	test	[VPD_5B3_BYTE],01h		; 14Pin Cable ?
	jnz	SHORT VPD_BI_TK_Set_Out		; Yes, then jump

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx, [esi.VPD_LPT_Number]
	add	ecx, 6				; ECX = I/F mode info.
	bts	DWORD PTR [eax], ecx		; Reset SEMI Cen. Bit

VPD_BI_TK_Set_OUT:
	pop	ecx
	pop	eax

VPD_BI_TK_Set_OUT_TRUE:

	call	VPD_Test_And_Set_Owner		; Does current VM own port?
	jc	SHORT VPD_BI_TK_Set_Exit	;  No, then Cable Check

	out	dx, al				;  Yes, then output byte to port

VPD_BI_TK_Set_Exit:
	ret

EndProc VPD_BI_TOKI_Set


;******************************************************************************
;
;   VPD_BI_TOKI_Status
;
;   DESCRIPTION:
;	The VM is reading either the data or control port.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;	+---------------------------------------------------------+
;	|   NEC PC-9800 series Bi-Direction Support Machine       |
;	+---------------------------------------------------------+
;	|   Port =  14Bh                                          |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_BI_TOKI_Status

	call	VPD_Test_Owner			; This VM is own LPT ?

	jnc	SHORT VPD_BI_TKS_Real_Status	; Yes, then jump

	cmp	[esi.VPD_LPT_Number], 1		; Q: LPT1?
	jne	SHORT VPD_BI_TKS_Not_Semi	;  No.

;*** VM is not owner ***

	push	eax
	push	ecx
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.

	bt	DWORD PTR [eax], ecx		; Q: VM Semicen?
	pop	ecx
	pop	eax

VPD_BI_TKS_Semi:
	mov	al,VPD_TOKI_Kill_Status		; AL <- FFh

	jnc	SHORT VPD_BI_TKS_Exit		; Semi Cen !

VPD_BI_TKS_Not_Semi:
	mov	al,VPD_BI_TOKI_Status_N		; AL <- 20h
	jmp	SHORT VPD_BI_TKS_Exit

VPD_BI_TKS_Real_Status:
	in	al, dx				; read status port

VPD_BI_TKS_Exit:
	ret

EndProc VPD_BI_TOKI_Status


;******************************************************************************
;
;   VPD_BI_EX_TOKI_Read
;
;   DESCRIPTION:
;	The VM is reading either the data or control port.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;	+---------------------------------------------------------+
;	|   NEC PC-9800 series Bi-Direction Support Machine       |
;	+---------------------------------------------------------+
;	|   Port =  14Eh                                          |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_BI_EX_TOKI_Read

	call	VPD_Test_And_Set_Owner		; Does current VM own port?

	jnc	SHORT VPD_BI_EX_TKR_Real_Status	; Yes, then jump

	cmp	[esi.VPD_LPT_Number], 1		; Q: LPT1?
	jne	SHORT VPD_BI_EX_TKR_Not_Semi	;  No.

;*** VM is not owner ***

	push	eax
	push	ecx
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = I/F mode info.

	bt	DWORD PTR [eax], ecx		; Q: VM Semicen?
	pop	ecx
	pop	eax

	jc	SHORT VPD_BI_EX_TKR_Not_Semi	; Not Semi Cen !

VPD_BI_EX_TKR_Semi:
	mov	al,VPD_TOKI_Kill_Status		; AL <- FFh
	jmp	SHORT VPD_BI_EX_TKR_Exit	; Semi Cen !

VPD_BI_EX_TKR_Not_Semi:

	mov	al,VPD_BI_EX_ST_DEFAULT		; Set Default
	jmp	SHORT VPD_BI_EX_TKR_Exit	; AT Mode !

VPD_BI_EX_TKR_Real_Status:
	in	al, dx				; read status port

VPD_BI_EX_TKR_Exit:
	ret

EndProc VPD_BI_EX_TOKI_Read


;******************************************************************************
;
;   VPD_BI_EX_TOKI_Set
;
;   DESCRIPTION:
;	The VM is reading either the data or control port.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;	+---------------------------------------------------------+
;	|   NEC PC-9800 series Bi-Direction Support Machine       |
;	+---------------------------------------------------------+
;	|   Port =  14Eh                                          |
;	+---------------------------------------------------------+
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_BI_EX_TOKI_Set, High_Freq

	test	[VPD_5B3_BYTE],01h		; 14Pin Cable ?
	jnz	SHORT VPD_BI_EX_TK_Set_OUT_TRUE	; Yes, then jump

VPD_BI_EX_TK_Set_OUT_TRUE:

	call	VPD_Test_And_Set_Owner		; Does current VM own port?
	jc	SHORT VPD_BI_EX_TK_Set_Exit	;  No, then Cable Check

	out	dx, al				;  Yes, then output byte to port

VPD_BI_EX_TK_Set_Exit:
	ret

EndProc VPD_BI_EX_TOKI_Set

ENDIF ;NEC_98


;******************************************************************************
;      S O F T W A R E	 I N T E R R U P T   H O O K   P R O C E D U R E S
;******************************************************************************
IFDEF NEC_98
;******************************************************************************
;
;   VPD_Int_1Ah		(NEC CHANGE 17h->1Ah)
;
;   DESCRIPTION:
;	This procedure monitors the BIOS printer interrupt service and will
;	return a busy error status if the printer is not owned by the current
;	VM.
;
;   ENTRY:
;	EAX = 1Ah (interrupt #)
;	EBX = Current VM handle
;
;   EXIT:
;	If carry clear then interrupt eaten (error status returned in AH)
;	else interrupt is reflected to current VM
;
;   USES:
;	ECX, ESI, Client_AH, Flags
;
;==============================================================================

%OUT Equates for this procedure

BeginProc VPD_Int_1Ah, High_Freq

	Assert_Cur_VM_Handle ebx

	push	eax
	push	ecx

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, VPD_PORT_REG_Bit		; Check Enable DX Reg.
	bt	DWORD PTR [eax], ecx		; Q: Enable DX Reg.

	jnc	SHORT VPD_INT_1AH_DEF_P		;  No.
						;
        movzx   esi, [ebp.Client_DX]            ; esi = LPT port number

	mov	[VPD_ACCESS_PORT],esi		; Save Current Access Port

	jmp	SHORT VPD_INT_1AH_PORT_AREA

VPD_INT_1AH_DEF_P:
	mov	esi,0				;
	mov	ecx, VPD_PORT_INFO_Bit4		;
	bt	DWORD PTR [eax], ecx		; Q: Default port = 3?
	jc	SHORT VPD_INT_1AH_DEF_P3	;  Yes.

	mov	ecx, VPD_PORT_INFO_Bit3		;
	bt	DWORD PTR [eax], ecx		; Q: Default port = 2?
	jc	SHORT VPD_INT_1AH_DEF_P2	;  Yes.

	jmp	SHORT VPD_INT_1AH_PORT_AREA	;  No.
						;

VPD_INT_1AH_DEF_P2:
	mov	esi,1				; Calc. port index
	jmp	SHORT VPD_INT_1AH_PORT_AREA	;

VPD_INT_1AH_DEF_P3:
	mov	esi,2				; Calc. port index

VPD_INT_1AH_PORT_AREA:				; ESI = port index
	xor	ecx,ecx
	mov	cl,[VPD_BI_PORT]		; Get Max port number
	dec	ecx				; Calc. port index

	mov	[VPD_ACCESS_PORT],esi		; Save Current Access Port
        cmp	esi, ecx			; Q: Invalid port index
	jna	SHORT VPD_INT_1AH_P_AREA_SET	;  No.

	pop	ecx
	pop	eax

	jmp	VPD_I1A_Reflect			; invalid (reflect)

VPD_INT_1AH_P_AREA_SET:
	pop	ecx
	pop	eax
						;
        imul    esi, SIZE VPD_Struc             ; ESI = Offset in LPT list
        add     esi, OFFSET32 VPD_LPT1          ; ESI -> LPT data structure

VPD_INT_1AH_COMMAND_CHECK:
;---------------------------------------------------------------------
;  Check command (all machine)                                        ;
;---------------------------------------------------------------------
	cmp	[ebp.Client_AH], 00h		; AT Timer BIOS CALL AH=00h ?
	je	VPD_I1Ah_00H			; Yes, then jump

	cmp	[ebp.Client_AH], 10h		; Printer Initialize Command ?
	je	VPD_I1A_OK			; Yes, then jump
	cmp	[ebp.Client_AH], 11h		; Output 1 Byte Data Command ?
	je	VPD_I1A_OK			; Yes, then jump
	cmp	[ebp.Client_AH], 12h		; Printer Status Get Command ?
	je	VPD_I1A_OK			; Yes, then jump
	cmp	[ebp.Client_AH], 30h		; Output many Bytes Command ?
	je	VPD_I1A_OK			; Yes, then jump

;---------------------------------------------------------------------
;  Check commnad (H98,TOKI,Printer Board)
;---------------------------------------------------------------------
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support?
	End_Touch_1st_Meg
	jnz	SHORT VPD_INT_1AH_CHECK_FULL	; Yes, then skip
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jnz	SHORT VPD_INT_1AH_CHECK_FULL	; Yes, then skip
	cmp	[VPD_ACCESS_PORT],1		; Current Access Port LPT2 or LPT3 ?
	jae	SHORT VPD_INT_1AH_CHECK_FULL	; Yes, then skip
	test	[VPD_BD_DRV_BYTE],08h		; Q: Max port num = 3?
	jnz	SHORT VPD_INT_1AH_CHECK_FULL	;  Yes.

	jmp	SHORT VPD_INT_1AH_CHECK_BD	; Check PRTDRV.SYS command

VPD_INT_1AH_CHECK_FULL:
	cmp	[ebp.Client_AH], 17h		; Change Full Cen.(AT) Command ?
	je	SHORT VPD_I1A_OK		; Yes, then jump
	cmp	[ebp.Client_AH], 18h		; Printer Full Status Command ?
	je	SHORT VPD_I1A_OK		; Yes, then jump
	cmp	[ebp.Client_AH], 19h		; Printer I/F Mode sence Command ?
	je	SHORT VPD_I1A_OK		; Yes, then jump
	cmp	[ebp.Client_AH], 1Ah		; Change Semi Cen. Command ?
	je	SHORT VPD_I1A_OK		; Yes, then jump

;---------------------------------------------------------------------
;  Check PRTDRV.SYS command                                          ;
;---------------------------------------------------------------------
VPD_INT_1AH_CHECK_BD:

	test	[VPD_BIBD_BYTE],02h		; Q: Exist PRTDRV.SYS?
	jz	SHORT VPD_INT_1AH_COMMAND_ERROR	;  No. Invalid command

	cmp	[ebp.Client_AX], 1E00h		; PRTDRV.SYS
	je	SHORT VPD_I1A_OK		; Yes, then jump
	cmp	[ebp.Client_AX], 1E01h		; PRTDRV.SYS
	je	SHORT VPD_I1A_OK		; Yes, then jump
	cmp	[ebp.Client_AX], 1E02h		; PRTDRV.SYS
	je	SHORT VPD_I1A_OK		; Yes, then jump

VPD_INT_1AH_COMMAND_ERROR:
	jmp	VPD_I1A_Reflect			; Invalid Command

VPD_I1A_OK:
	cmp	[ebp.Client_AH], 1Eh		; PRTDRV.SYS COMMAND
	jne	VPD_I1A_Not_1EH			; Yes, then jump

	cmp	[ebp.Client_AL], 00h		; Sence Command?
	jne	SHORT VPD_I1A_Not_1E00H		; Yes, then jump

;===== Check PRTDRV.SYS infomation =====
VPD_I1A_1E00H:
; This function don't use I/O (in/out).

	jmp	VPD_I1A_Reflect			; Exec PRTDRV.SYS

VPD_I1A_Not_1E00H:
	cmp	[ebp.Client_AL], 01h		; DX Reg ON/OFF Command ?
	jne	SHORT VPD_I1A_1E02H		; Yes, then jump

VPD_I1A_1E01H:
	push	eax
	push	ecx
	push	esi

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	cmp	[ebp.Client_DH], 01h		; Change to DX Reg ON
	jne	SHORT VPD_I1A_1E01H_OFF		; No, then jump

	mov	ecx, VPD_PORT_REG_Bit		;
	bts	DWORD PTR [eax], ecx		; Enable DX Reg.(Control-Block)

	jmp	SHORT VPD_I1A_1E01H_END		;

VPD_I1A_1E01H_OFF:
	mov	ecx, VPD_PORT_REG_Bit		;
	btr	DWORD PTR [eax], ecx		; Disable DX Reg.(Control-Block)

VPD_I1A_1E01H_END:
	pop	esi
	pop	ecx
	pop	eax

	jmp	VPD_I1A_Reflect			; Exec PRTDRV.SYS

VPD_I1A_1E02H:					; Change Current Port Command
	cmp	[ebp.Client_AL], 02h		; DX Reg ON/OFF Command ?
	je	SHORT VPD_I1A_1E02H_CHECK	; Yes, then jump
	jmp	VPD_I1A_Reflect			; Exec PBIOS or RTDRV.SYS

VPD_I1A_1E02H_CHECK:
	push	eax
	push	ecx
	push	esi

	cmp	[ebp.Client_DX], 0002h		; LPT3?
	jne	SHORT VPD_I1A_1E02H_CHECK_LPT2	; No, then jump

	cmp	[VPD_BI_PORT],3			; Q: Max port num = 3?
	jne	SHORT VPD_I1A_1E02H_END		;  No.
						;

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx,VPD_PORT_INFO_Bit4		; Set (10)
	bts	DWORD PTR [eax], ecx		; Set default port is LPT3

	mov	ecx,VPD_PORT_INFO_Bit3		;
	btr	DWORD PTR [eax], ecx		; Set default port is LPT3

	jmp	SHORT VPD_I1A_1E02H_END		;

VPD_I1A_1E02H_CHECK_LPT2:
	cmp	[ebp.Client_DX], 0001h		; LPT2?
	jne	SHORT VPD_I1A_1E02H_CHECK_LPT1	; No, then jump

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx,VPD_PORT_INFO_Bit4		; Set (01)
	btr	DWORD PTR [eax], ecx		; Set default port is LPT2

	mov	ecx,VPD_PORT_INFO_Bit3		; 
	bts	DWORD PTR [eax], ecx		; Set default port is LPT2

	jmp	SHORT VPD_I1A_1E02H_END		;

VPD_I1A_1E02H_CHECK_LPT1:
	cmp	[ebp.Client_DX], 0000h		; LPT1?
	jne	SHORT VPD_I1A_1E02H_END		; No, then jump

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx,VPD_PORT_INFO_Bit4		; Set (00)
	btr	DWORD PTR [eax], ecx		; Set default port is LPT1

	mov	ecx,VPD_PORT_INFO_Bit3		; 
	btr	DWORD PTR [eax], ecx		; Set default port is LPT1

VPD_I1A_1E02H_END:
	pop	esi
	pop	ecx
	pop	eax

	jmp	VPD_I1A_Reflect			; Exec PRTDRV.SYS

VPD_I1A_Not_1EH:
						;
	cmp	[ebp.Client_AH], 10h		; Printer Initialize Command ?
	je	SHORT VPD_I1A_NO_CHECK		; Yes, then jump
	cmp	[ebp.Client_AH], 11h		; Output 1 Byte Data Command ?
	je	SHORT VPD_I1A_NO_CHECK		; Yes, then jump
	cmp	[ebp.Client_AH], 12h		; Printer Status Get Command ?
	je	SHORT VPD_I1A_NO_CHECK		; Yes, then jump

	cmp	[ebp.Client_AH], 30h		; Output many Bytes Command ?
	je	SHORT VPD_I1A_30H		; Yes, then jump

	cmp	[ebp.Client_AH], 17h		; Change Full Cen.(AT) Command ?
	je	VPD_I1A_17H			; Yes, then jump

	cmp	[ebp.Client_AH], 18h		; Printer Full Status Command ?
	je	VPD_I1A_18H			; Yes, then jump

	cmp	[ebp.Client_AH], 19h		; Printer I/F Mode sence Command ?
	je	VPD_I1A_19H			; Yes, then jump

	cmp	[ebp.Client_AH], 1Ah		; Change Semi Cen. Command ?
	je	VPD_I1A_1AH			; Yes, then jump

	jmp	VPD_I1A_Reflect			; Return BIOS & PRTDRV.SYS


VPD_I1A_NO_CHECK:
	call	VPD_Test_And_Set_Owner		; Q: Does this VM own it?
	jnc	VPD_I1A_Reflect			; Exec PBIOS or RTDRV.SYS

	push	eax
	push	ecx
	push	esi

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx, [VPD_ACCESS_PORT]		; ECX <- Port Number
	add	ecx, 7				; Port n I/F Mode Bit

	bt	DWORD PTR [eax], ecx		; Full Centro ?
	jc	SHORT VPD_I1A_NO_CHECK_Full	; No, then JUMP

	mov	[ebp.Client_AH], VPD_BIOS_NSemi_Status	; Semi. Ready Status

	pop	esi
	pop	ecx
	pop	eax

	jmp	VPD_I1A_Exit			; Return next int 1a code

VPD_I1A_NO_CHECK_Full:
	mov	[ebp.Client_AX], VPD_BIOS_NFull_Status	; Full. Ready Status

	pop	esi
	pop	ecx
	pop	eax

	jmp	VPD_I1A_Exit			; Return net int 1a code

VPD_I1A_30H:
	call	VPD_Test_And_Set_Owner		; Q: Does this VM own it?
	jnc	VPD_I1A_Reflect			; Exec PBIOS or RTDRV.SYS

	mov	[ebp.Client_AH], 00h		; Set No Error

	push	eax
	push	ebx

	xor	eax,eax
	xor	ebx,ebx
	mov	ax, [ebp.Client_CX]		; Data Length
	mov	bx, [ebp.Client_BX]		; Start Addr
	add	eax,ebx				; Cal. End Addr
	mov	[ebp.Client_BX], ax		; Set Data Last Addr
	mov	[ebp.Client_CX], 00h		; Set No Remain Data

	pop	ebx
	pop	eax

	jmp	VPD_I1A_Exit			; Return next int 1a code

VPD_I1A_17H:
	push	eax
	push	ecx
	push	esi

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	cmp	[VPD_ACCESS_PORT],0
	je	SHORT VPD_I1A_17H_CHECK_LPT1	; LPT1 then jump

VPD_I1A_17H_GO_FULL:
	mov	ecx, [VPD_ACCESS_PORT]		; ECX <- Port Number
	add	ecx, 7				; Port n I/F Mode Bit

	bts	DWORD PTR [eax], ecx		; C.B. Set Full Centro

	call	VPD_Test_And_Set_Owner		; Q: Does this VM own it?
	jnc	SHORT VPD_I1A_17H_Reflect	; Exec PBIOS or RTDRV.SYS

	mov	[ebp.Client_AX], VPD_BIOS_NFull_Status	; Full. Ready Status

	jmp	SHORT VPD_I1A_17H_NO_FULL	; Return next int 1a code

VPD_I1A_17H_CHECK_LPT1:
	cmp	[VPD_BI_PORT],2			; Q: Max port num = 2?
	je	SHORT VPD_I1A_17H_GO_FULL	;  Yes.

	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support?
	End_Touch_1st_Meg
	jz	SHORT VPD_I1A_17H_CHECK_LPT1_H98; Yes.

	test	[VPD_5B3_BYTE],01h		; 14Pin Cable?
	jz	SHORT VPD_I1A_17H_GO_FULL	;  No.

	mov	[ebp.Client_AH], 06h		; Set Cable Error
	jmp	SHORT VPD_I1A_17H_NO_FULL	; Return next int 1a code

VPD_I1A_17H_CHECK_LPT1_H98:
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jnz	SHORT VPD_I1A_17H_GO_FULL	; Yes.

	mov	[ebp.Client_AH], 00h		; Set return val AH=00h

VPD_I1A_17H_NO_FULL:
	pop	esi
	pop	ecx
	pop	eax

	jmp	VPD_I1A_Exit			; Return next int 1a code

VPD_I1A_17H_Reflect:
	pop	esi
	pop	ecx
	pop	eax

	jmp	VPD_I1A_Reflect			; Exec PBIOS or RTDRV.SYS

VPD_I1A_18H:
	call	VPD_Test_And_Set_Owner		; Q: Does this VM own it?
	jnc	VPD_I1A_Reflect			;  Yes. Exec PBIOS or RTDRV.SYS

	cmp	[VPD_BI_PORT],2			; Q: Max port num = 2?
	je	SHORT VPD_I1A_18H_FULL		; Yes, then Full Status !

	cmp	[VPD_ACCESS_PORT], 0		; Port is LPT1 ?
	jne	SHORT VPD_I1A_18H_FULL		; No, then FUll Status !

	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jnz	SHORT VPD_I1A_18H_FULL		; Yes, then Full Status !

	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support?
	End_Touch_1st_Meg
	jz	SHORT VPD_I1A_18H_SEMI		; No, then SEMI Status

	test	[VPD_5B3_BYTE],01h		; 14Pin?
	jz	SHORT VPD_I1A_18H_FULL		; No, then Full Status

	mov	[ebp.Client_AH], 06h		; Set Cable Error
	jmp	SHORT VPD_I1A_18H_END		; Return next int 1a code

VPD_I1A_18H_SEMI:
	mov	[ebp.Client_AH], VPD_BIOS_NSemi_Status	; Semi. Ready Status
	jmp	SHORT VPD_I1A_18H_END		; Return next int 1a code

VPD_I1A_18H_FULL:
	mov	[ebp.Client_AX], VPD_BIOS_NFull_Status	; Full. Ready Status

VPD_I1A_18H_END:
	jmp	VPD_I1A_Exit			; Return next int 1a code

VPD_I1A_19H:
	call	VPD_Test_And_Set_Owner		; Q: Does this VM own it?
	jnc	VPD_I1A_Reflect			;  Yes. PBIOS or RTDRV.SYS

	push	eax
	push	ecx
	push	esi

	cmp	[VPD_BI_PORT],2			; Q: Max port num = 2?
	je	SHORT VPD_I1A_19H_RET_8x	;  Yes.

	cmp	[VPD_ACCESS_PORT], 0		; Port is LPT1 ?
	jne	SHORT VPD_I1A_19H_RET_8x	; No, then Jump

	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jnz	SHORT VPD_I1A_19H_RET_0x	; Yes.

	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support?
	End_Touch_1st_Meg
	jz	SHORT VPD_I1A_19H_RET_00	; No.

	test	[VPD_5B3_BYTE],01h		; 14Pin Cable?
	jz	SHORT VPD_I1A_19H_RET_8x	; No.

	mov	[ebp.Client_AH], 81h		; Yes. Set return val AH=81h
	jmp	SHORT VPD_I1A_19H_END		; Return next int 1a code

VPD_I1A_19H_RET_00:
	mov	[ebp.Client_AH], 00h		; Set return val AH=00h
	jmp	SHORT VPD_I1A_19H_END		; Return next int 1a code

VPD_I1A_19H_RET_8x:
	mov	[ebp.Client_AH], 81h		; AH=81h
	jmp	SHORT VPD_I1A_19H_CHECK_CB	; Check C.B.

VPD_I1A_19H_RET_0x:
	mov	[ebp.Client_AH], 01h		; AH=01h

VPD_I1A_19H_CHECK_CB:
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx, [VPD_ACCESS_PORT]		; ECX <- Port Number
	add	ecx, 7				; Port n I/F Mode Bit

	bt	DWORD PTR [eax], ecx		; Full Centro ?
	jnc	SHORT VPD_I1A_19H_END		; No, then JUMP

	or	[ebp.Client_AH], 02h		; Set Full.

VPD_I1A_19H_END:
	pop	esi
	pop	ecx
	pop	eax

	jmp	SHORT VPD_I1A_Exit		; Return next int 1a code

VPD_I1A_1AH:
	push	eax
	push	ecx
	push	esi

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	mov	ecx, [VPD_ACCESS_PORT]		; ECX <- Port Number
	add	ecx, 7				; Port n I/F Mode Bit

	btr	DWORD PTR [eax], ecx		; C.B. Reset Full Centro

	call	VPD_Test_And_Set_Owner		; Q: Does this VM own it?
	jnc	SHORT VPD_I1A_1AH_Reflect	; Exec PBIOS or RTDRV.SYS

	mov	[ebp.Client_AH], 00h		; Set return val AH=00h

	pop	esi
	pop	ecx
	pop	eax

	jmp	SHORT VPD_I1A_Exit		; Return next int 1a code

VPD_I1A_1AH_Reflect:
	pop	esi
	pop	ecx
	pop	eax

	jmp	SHORT VPD_I1A_Reflect		; Exec PBIOS or RTDRV.SYS

VPD_I1A_Exit:
	clc					; Eat the interrupt
	ret					; (The VM will never see it)

;
;   If this is function call 0 then make sure the printer is connected
;   before reflecting it to the BIOS to prevent time-out hang of doom.
;
VPD_I1A_Test_Connected:
;------------------------------------------------------------------------------
;
;   Reflect this int 1Ah to the virtual machine.
;

VPD_I1A_Reflect:

;=============================================================================
; Exec INT 1Ah Critical Section
; -------------------------------------------------------------------
;	1."INT1ACritical=true" ?
;	    true  -> do noting (refrect VM)
;	    false -> next
;	2.Check critical section
;	    Yes -> do notiong (refrect VM)
;	    No  -> next
;	3.Begin Critical Section
;	    < VMMCall Begin_Critical_Section >
;	4.Set return address
;	    < VMMCall Call_When_VM_Return : return addr=VPD_Critical_Reset >
;	5.Set carry and Refrect VM
;-----------------------------------------------------------------------------

	cmp	[SYSINI_Set_Critical], false	;
	je	short VPD_Critical_Set_End	;
	cmp	[VPD_INT1A_Critical], 0		;
	jne	SHORT VPD_Critical_Set_End	;
	inc	[VPD_INT1A_Critical]		;
	VMMcall Begin_Critical_Section		;
	xor	eax, eax			;
	mov	esi, OFFSET32 VPD_Critical_Reset;
	VMMcall Call_When_VM_Returns		;
						;
VPD_Critical_Set_End:
	stc
	ret

; AT Timer BIOS CALL Emulate(AH=0h)
VPD_I1Ah_00H:
        VMMCall Test_Sys_VM_Handle              ; Q: Called from System VM?
	jne	VPD_I1A_Exit			;  N: Do nothing!

	push	eax
	push	ebx
	push	edx
	VxDCall	VTD_Get_Real_Time
	mov	ebx, [ATRes]
	div	ebx
	sub	eax, [AdjustATTick]

	mov	ebx, 0
	cmp	eax, 1800b0h
	jb	SHORT @f
	sub	eax, 1800b0h
	inc	ebx
@@:
	mov	[ebp.Client_DX], ax
	shr	eax, 0Fh
	mov	[ebp.Client_CX], ax
	mov	[ebp.Client_AL], bl
	pop	edx
	pop	ebx
	pop	eax
	clc					; Eat the interrupt
	ret					; (The VM will never see it)
EndProc VPD_Int_1Ah

;============================================================================
; END Critical Section
; ----------------------------
;	1.Check Critical Section
;		No  ->  do notiong (refrect VM)
;		Yes ->  next
;	2.End Critical Section
;		< VMMCall End_Critical_Section >
;	3.Clear carry and Refrect VM
;============================================================================

BeginProc VPD_Critical_Reset,PUBLIC		;
	cmp	[VPD_INT1A_Critical], 1		;
	jne	SHORT VPD_Critical_Reset_Exit	;
	mov	[VPD_INT1A_Critical], false	;
	VMMjmp	End_Critical_Section		;
						;
VPD_Critical_Reset_Exit:			;
	clc					;
	ret					;
EndProc VPD_Critical_Reset			;

;******************************************************************************
;
;   VPD_Int_5h
;
;   DESCRIPTION:
;	This is copy (PrintScreen) key handler to let realmode hardcopy
;	procedure work properly. Mainly, this process is required to avoid
;	dead locking of critical section.
;
;   ENTRY:
;	EAX = 5h (interrupt #)
;	EBX = Current VM handle
;
;   EXIT:
;	none
;
;   USES:
;	ECX, ESI, Client_AH, Flags
;
;==============================================================================

BeginProc VPD_Int_5h, High_Freq
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	test	dword ptr [eax], VPD_INT5_Processing	; test current
							; processing bit
	jnz	vpd_int_5h_reflect

	VMMcall	Test_Sys_VM_Handle		; Who calls this entry in sys
	je	vpd_int_5h_ret			; VM? There is something wrong.
	xor	eax, eax
	mov	ecx, PEF_Wait_Not_Crit		; not be called in critical
	mov	edx, ebx
	mov	esi, OFFSET32 VPD_Scheduled_Int_5h
	xor	edi, edi
	VMMcall	Call_Priority_VM_Event
						; esi has event handle
vpd_int_5h_ret:
	clc
	ret
vpd_int_5h_reflect:
	and	dword ptr [eax], not VPD_INT5_Processing	; clear int5 bit
	stc
	ret
EndProc VPD_Int_5h

BeginProc VPD_Scheduled_Int_5h
	cmp	ebx, edx			; check VM handle
	jne	vpd_scheduled_int_5h_ret

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	or	dword ptr [eax], VPD_INT5_Processing 	; set current processing
							; bit
	VMMcall	Begin_Nest_Exec
	mov	eax, 5
	VMMcall	Exec_Int
	VMMcall	End_Nest_Exec

vpd_scheduled_int_5h_ret:
	ret
EndProc VPD_Scheduled_Int_5h

ELSE ;NEC_98
;******************************************************************************
;
;   VPD_Int_17h
;
;   DESCRIPTION:
;	This procedure monitors the BIOS printer interrupt service and will
;	return a busy error status if the printer is not owned by the current
;	VM.
;
;   ENTRY:
;	EAX = 17h (interrupt #)
;	EBX = Current VM handle
;
;   EXIT:
;	If carry clear then interrupt eaten (error status returned in AH)
;	else interrupt is reflected to current VM
;
;   USES:
;	ECX, ESI, Client_AH, Flags
;
;==============================================================================

%OUT Equates for this procedure

BeginProc VPD_Int_17h, High_Freq

	Assert_Cur_VM_Handle ebx

	cmp	[ebp.Client_AH], 2		; Q: Get status or unknown?
	jae	SHORT VPD_I17_Reflect		;    Y: Just reflect it
						;    N: Check for contention
	movzx	esi, [ebp.Client_DX]		; ESI = Port # (0-3)
	cmp	esi, VPD_Max_Virt_LPTs-1	; Q: Valid port?
	ja	SHORT VPD_I17_Reflect		;    N: Just let it go
	imul	esi, SIZE VPD_Struc		; ESI = Offset in LPT list
	add	esi, OFFSET32 VPD_LPT1		; ESI -> LPT data structure

	TestMem	[esi.VPD_Flags], VPDF_Enabled	; Q: Trapping on this port ?
	jz	SHORT VPD_I17_Reflect		;    N: just reflect it.

	call	VPD_Test_And_Set_Owner		; Q: Does this VM own it?
	jnc	SHORT VPD_I17_Test_Connected	;    Y: Make sure OK to print

VPD_I17_Fail:
	mov	[ebp.Client_AH], 00100001b	; Time-out, Out of paper
	clc					; Eat the interrupt
	ret					; (The VM will never see it)

;
;   If this is function call 0 then make sure the printer is connected
;   before reflecting it to the BIOS to prevent time-out hang of doom.
;
VPD_I17_Test_Connected:
	cmp	[ebp.Client_AH], 0		; Q: Is this a print char call?
	jne	SHORT VPD_I17_Reflect		;    N: Reflect init calls

	TestMem	[esi.VPD_Flags], VPDF_Is_Connected ; Q: Detected printer yet?
	jnz	SHORT VPD_I17_Reflect		   ;	Y: Just reflect it

	VMMcall Begin_Nest_V86_Exec		; Prepare to reflect int
	mov	ecx, [ebp.Client_EAX]		; ECX = Caller's EAX
	mov	[ebp.Client_AH], 02h		; Read status command
	VMMcall Exec_Int			; Relfect Int 17h (in EAX)
	xchg	ecx, [ebp.Client_EAX]		; Get client EAX/Restore old
	VMMcall End_Nest_Exec			; End of nested exectuion
	test	ch, 10000000b			; Q: Is printer NOT busy
	jz	SHORT VPD_I17_Fail		;    N: Time-out RIGHT NOW!
	test	ch, 00100000b			;    Y: Q: Any paper in it?
	jnz	SHORT VPD_I17_Fail		;	   N: Error
	or	[esi.VPD_Flags], VPDF_Is_Connected ;	   Y: Set flag

;
;   Reflect this int 17h to the virtual machine.
;
VPD_I17_Reflect:
	stc
	ret

EndProc VPD_Int_17h
ENDIF ;NEC_98


;******************************************************************************
;	  H A R D W A R E   I N T E R R U P T	P R O C E D U R E S
;******************************************************************************

;******************************************************************************
;
;   VPD_Hw_Int
;
;   DESCRIPTION:
;	This procedure is called whenever a hardware interrupt occurs on
;	IRQ 7.	Note that this procedure will not be called unless Sgrab
;	support is enabled since VPD will only virtualize the printer
;	interrupt if Sgrab is enabled.
;
;	VPD_Hw_Int sets the interrupt request for the virtual printer IRQ
;	line for the VM that is the current display owner.  Note that
;	this procedure does NOT service the interrupting hardware.  It is
;	assumed that the Sgrab code running in the virtual machine will
;	service the interrupt and issue an End Of Interrupt (EOI).
;
;	Note that this procedure is delared in a LOCKED code segment.  All
;	hardware interrupt handlers must be in locked memory.  VPD_Virt_EOI,
;	however, does not need to be in locked memory since it is called
;	when the VM issues an EOI.  Therefore, the virtual EOI is synchronous
;	with Windows/386.  In other words, a hardware interrupt may happen
;	in the middle of a VxD operation but a virtual EOI can not.
;
;   ENTRY:
;	EAX = IRQ handle
;	EBX = Current VM handle
;
;   EXIT:
;	None
;
;   USES:
;	EBX, Flags
;
;==============================================================================

BeginProc VPD_Hw_Int

	mov	ebx, [VPD_Hw_Int_Owner] 	; Reflect interrupt to this VM
	VxDjmp	VPICD_Set_Int_Request		; Set int request and return

EndProc VPD_Hw_Int



;------------------------------------------------------------------------------

;******************************************************************************
;
;   VPD_Virt_EOI
;
;   DESCRIPTION:
;	This procedure is called whenever a VM issues an End Of Interrupt for
;	IRQ 7.	Note that this procedure will not be called unless Sgrab
;	support is enabled since VPD will only virtualize the printer
;	interrupt if Sgrab is enabled.
;
;	At this point the Virtual Machine has serviced the simulated hardware
;	interrupt and is signaling the Virtual PIC Device that it is ready
;	to receive another interrupt.  This procedure lowers the virtual
;	interrupt request to prevent simulating another unwanted interrupt
;	and then issues an End Of Interrupt call for the PHYSICAL hardware
;	interrupt.
;
;   ENTRY:
;	EAX = IRQ handle
;	EBX = VM handle
;
;   EXIT:
;	None
;
;   USES:
;	EBX, Flags
;
;==============================================================================

BeginProc VPD_Virt_EOI

	VxDCall VPICD_Clear_Int_Request 	; Clear virtual IRQ request
	VxDjmp	VPICD_Phys_EOI			; Physical end of interrupt

EndProc VPD_Virt_EOI


;******************************************************************************
;			L O C A L   P R O C E D U R E S
;******************************************************************************

;******************************************************************************
;
;   VPD_Detach
;
;   DESCRIPTION:
;	Detaches the specified LPT from its current owner.  If the LPT is
;	currently owned and is a hardware (non-network) printer port then
;	this procedure will re-enable trapping of the I/O ports
;
;   ENTRY:
;	ESI -> VPD data structure
;
;   EXIT:
;	None
;
;   USES:
;       Flags
;
;==============================================================================

BeginProc VPD_Detach

	pushad
ifdef MAXDEBUG
	Trace_Out "VPD_Detach (VM=#EBX)"
endif

	and	[esi.VPD_Flags], NOT VPDF_Special_Owner

	mov	ebx, [esi.VPD_Owner]		; EBX = Handle of owner
	test	ebx, ebx			; Q: Does any VM own printer?
ifdef NEC_98
	jz	VPD_D_Exit			;    N: Do nothing!
else ;NEC_98
	jz	SHORT VPD_D_Exit		;    N: Do nothing!
endif ;NEC_98
						;    Y: Detach it
	mov	edx, [esi.VPD_IO_Base]		; EDX = Base I/O port
IFDEF NEC_98
	cmp	edx, LPT1_Base + Low_AT_DATA
	jne	SHORT VPD_D_NOT_140h
	test	[VPD_5B3_BYTE], 80h		; Bi-Direction Support?
	jz	SHORT VPD_D_Trap98		; No,then skip
VPD_D_NOT_140h:
	mov	ecx, VPD_Num_Ports_Per_LPT	; ECX = # I/O ports per LPT
VPD_D_Loop:
	VMMCall Enable_Local_Trapping		; Enable trapping for port EDX
	inc	edx				; Bump to next port
	loopd	VPD_D_Loop			; Loop for all LPT I/O ports

	;TOKI PORT
	mov	dl,Low_TOKI_STATUS	;edx=0x4bh
        VMMCall Enable_Local_Trapping
	mov	dl,Low_FIFO_DATA	;edx=0x4ch
        VMMCall Enable_Local_Trapping
	mov	dl,Low_EX_CONTROL	;edx=0x4eh
        VMMCall Enable_Local_Trapping

	;98 PORT
        cmp     [esi.VPD_LPT_Number],1	;LPT1?
	jne	SHORT VPD_D98_end	;No.
VPD_D_Trap98:
	mov	edx, 40h		;edx=040h
        mov     ecx, 3
@@:
        VMMCall Enable_Local_Trapping	;Trap Port (40h,42h,44h)
        inc     edx
        inc     edx
        loop    @B
VPD_D98_end:
        VMMcall Test_Sys_VM_Handle                  ; Q: System VM handle?
        je      SHORT VPD_D_Pre_Exit                ;   Y: Continue

	push	ecx
	mov	ecx, [esi.VPD_LPT_Number]	; ECX = LPT number
	cmp	ecx,1				; LPT1
	pop	ecx
	jne	SHORT VPD_Detach_Set_IF_BD

	cmp	[VPD_BI_PORT],2			; Q: Max port num = 2?
	je	SHORT VPD_Detach_Set_IF_BD	;  Yes.

	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jz	SHORT 	VPD_DIF_Not_H98	;no,then skip
	push	ebx
	mov	ebx, [VPD_CB_EBX]		; System VM Handle
	call	VPD_Set_Interface		; Set boot printer mode
	pop	ebx

VPD_DIF_Not_H98:
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support?
	End_Touch_1st_Meg
	jz	SHORT VPD_DIF_Not_BI_Sup	; No,then skip
	test	[VPD_5B3_BYTE],01h		; 14Pin Cable ?
	jnz	SHORT VPD_DIF_Not_BI_Sup	; Yes, then jump

	push	ebx
	mov	ebx, [VPD_CB_EBX]		; System VM Handle

	call	VPD_BI_Set_Interface		; Set boot printer mode

	pop	ebx

VPD_DIF_Not_BI_Sup:
	jmp	SHORT VPD_D_Pre_Exit

VPD_Detach_Set_IF_BD:

	push	ebx
	mov	ebx, [VPD_CB_EBX]		; System VM Handle

	call	VPD_BD_Set_Interface		; Set boot printer mode

	pop	ebx

VPD_D_Pre_Exit:
ELSE ;NEC_98
	test	dh, dh				; Q: Physical LPT?
	jz	SHORT VPD_D_Exit		;    N: Done!
						;    Y: Enable port trapping
	mov	ecx, VPD_Num_Ports_Per_LPT	; ECX = # I/O ports per LPT
VPD_D_Loop:
	VMMCall Enable_Local_Trapping		; Enable trapping for port EDX
	inc	edx				; Bump to next port
	loopd	VPD_D_Loop			; Loop for all LPT I/O ports
ENDIF ;NEC_98

	mov	[esi.VPD_Owner], 0		; LPT is not owned

VPD_D_Exit:
	popad
	ret

EndProc VPD_Detach


;******************************************************************************
;
;   VPD_Attach
;
;   DESCRIPTION:
;	Assigns ownership of a LPT port to a VM.  When a VM owns a physical
;	(non-network) LPT we disable trapping of the status and control ports
;	to avoid the overhead of port traps.  We continue to trap the data
;	port so that we can time-out printer ownership.
;
;   ENTRY:
;	EBX = Handle of VM to own LPT
;	ESI -> VPD data structure
;	ECX = 0 if normal attach (LPT should time out)
;	    != 0 if special owner (tells us when port free)
;
;   EXIT:
;	None
;
;   USES:
;       Flags
;
;==============================================================================

BeginProc VPD_Attach

	Assert_VM_Handle ebx			; Paranoia...
ifdef MAXDEBUG
	Trace_Out "VPD_Attach (VM=#EBX)"
endif

	pushad

	mov	edx, [esi.VPD_IO_Base]		; EDX = Base I/O port
ifdef NEC_98
	cmp	dl, 040h
	jne	VPD_Attach_Exit			;    N: Done
else ;NEC_98
	test	dh, dh				; Q: Phys LPT (has I/O ports)?
	jz	SHORT VPD_Attach_Exit		;    N: Done
endif ;NEC_98
						;    Y: Disable port traps

	call	VPD_Detach			; Detach from current owner
	mov	[esi.VPD_Owner], ebx		; Set new owner

if	WPS
;
; If previous owner was jumbo and new owner is not jumbo, perform mode switch
	TestMem	[esi.VPD_Flags], VJPDF_Jumbo
	jz	@F
	TestMem	[esi.VPD_Flags], VJPDF_JumboAcq
	jnz	@F

	; clear flag before switch to avoid multiple entries
	and	[esi.VPD_Flags], NOT VJPDF_Jumbo
	call	VJPD_Jumbo_PCL_Mode_Switch
ifdef NEC_98
        bt      [esi.VPD_Flags], VJPDF_NONJumbo_Bit
        jc      SHORT @F
endif ;NEC_98
	call    VJPD_Nulls_After_Mode_Switch
@@:
endif ; WPS

	jecxz	VPD_A_Reset_Contention
	or	[esi.VPD_Flags], VPDF_Special_Owner
VPD_A_Reset_Contention:

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	edi, [esi.VPD_LPT_Number]	; EDI = LPT number
	btr	DWORD PTR [eax], edi		; Reset contention flag in VM

IFDEF NEC_98
	cmp	edx, LPT1_Base + Low_AT_DATA
	jne	SHORT VPD_A_NOT_140h
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support?
	End_Touch_1st_Meg
	jz	SHORT VPD_A_Trap98		; No,then skip
VPD_A_NOT_140h:
	jecxz	VPD_A_Disable_Last_2_Ports	; If special owner then
	VMMcall Disable_Local_Trapping		; Disable trapping for port EDX
	push	edx
	mov	dl,Low_FIFO_DATA		; edx=0x4ch
	VMMcall Disable_Local_Trapping		; Turn off trapping
	pop	edx
VPD_A_Disable_Last_2_Ports:
	inc	edx				; EDX = Status port
	VMMcall Disable_Local_Trapping		; Turn off trapping
	inc	edx				; EDX = Control port
	VMMcall Disable_Local_Trapping		; Turn off trapping

	;TOKI PORT
	mov	dl,Low_TOKI_STATUS		; edx=0x4bh
	VMMcall Disable_Local_Trapping		; Turn off trapping
	mov	dl,Low_EX_CONTROL		; edx=0x4eh
	VMMcall Disable_Local_Trapping		; Turn off trapping

	;98 PORT
        cmp     [esi.VPD_LPT_Number],1		;LPT1?
	jne	SHORT VPD_A98_end		;No.
VPD_A_Trap98:
	mov	edx, 40h			;edx=040h
	jecxz	VPD_A_Disable_Last_2_Ports98	; If special owner then
	VMMcall Disable_Local_Trapping		; Turn off trapping
VPD_A_Disable_Last_2_Ports98:
        mov     edx, 42h
	VMMcall Disable_Local_Trapping		; Turn off trapping
        mov     edx, 46h
	VMMcall Disable_Local_Trapping		; Turn off trapping
VPD_A98_end:
	cmp	[esi.VPD_LPT_Number], 1		; Q: LPT1?
	jne	SHORT VPD_Attach_Set_IF_BD	;  No.

	cmp	[VPD_BI_PORT],2			; Q: Max port num = 2?
	je	SHORT VPD_Attach_Set_IF_BD	;  Yes.
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jz	SHORT 	VPD_AIF_Not_H98		;no,then skip

	call	VPD_Set_Interface
	jmp	SHORT VPD_AIF_Not_BI_Sup_END

VPD_AIF_Not_H98:
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	; Bi-Direction Support?
	End_Touch_1st_Meg
	jz	SHORT VPD_AIF_Not_BI_Sup_END	; No,then skip
	test	[VPD_5B3_BYTE],01h		; 14Pin Cable ?
	jnz	SHORT VPD_AIF_Not_BI_Sup_END	; Yes, then jump

	call	VPD_BI_Set_Interface
	jmp	SHORT VPD_AIF_Not_BI_Sup_END

VPD_Attach_Set_IF_BD:
	call	VPD_BD_Set_Interface

VPD_AIF_Not_BI_Sup_END:
ELSE ;NEC_98
	jecxz	VPD_A_Disable_Last_2_Ports	; If special owner then
	VMMcall Disable_Local_Trapping		; Turn off data too
VPD_A_Disable_Last_2_Ports:
	inc	edx				; EDX = Status port
	VMMcall Disable_Local_Trapping		; Turn off trapping
	inc	edx				; EDX = Control port
	VMMcall Disable_Local_Trapping		; Turn off trapping
ENDIF ;NEC_98

VPD_Attach_Exit:
	popad
	ret

EndProc VPD_Attach

IFDEF NEC_98
;******************************************************************************
;   VPD_Set_Interface
;
;   DESCRIPTION:
;		THIS ROUTIN IS ADDED FOR H98
;
;   ENTRY:
;		EBX = VM HANDLE OF NEW OWNER
;
;==============================================================================
BeginProc VPD_Set_Interface

SYS_PORT	EQU	37H
PR_MODE		EQU	46H
PR_WSIG		EQU	46H
PR_DATA		EQU	40H
PRN_EXMOD	EQU	448H

PR_MSK_ON	EQU	0DH
PR_MSK_OFF	EQU	0CH
PRCEN_STBON	EQU	0FH
PRCEN_MOD	EQU	82H
PRCEN_HMOD	EQU	0A2H

PRN_CompatibleMode	EQU	00H
PRN_INT_MOD		EQU	01H

IPrimeResetMask		EQU	00H
IPrimeSetMask		EQU	01H
FullCentro	equ	04h


	push	ecx
	push	eax
	push	edx
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jz	VPD_END_Set_IF				;no,then skip

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = LPT number
	bt	DWORD PTR [eax], ecx		; Q: Has VM contended already?

	jnc	SHORT VPD_Fullcen_ModeSet 	; set full -> semi

VPD_Semicen_Mode_Set:				;semi-centro

;
;Change interface mode (semicentro -> fullcentro)
;
;	This command executing I.PRIME is active
;	and Reset Printer.


	push	esi

	mov	esi, VPD_BIOS_Timeout_Addr	;
	add	esi, [ebx.CB_High_Linear]	; esi = point to 0:458h
	bts	WORD PTR [esi],02h		; 0:458=02h

	pop	esi

	mov	dx,448h
	in	al,dx				;Curr IF Mode?
	test	al,01h
	jnz	SHORT VPD_End_Set_IF				;fullcen,then skip

	pushfd				;
	cli				;disable interrupt

	push	edx
	push	ecx

;------ PSTB ON(Port C)
	MOV	AL,PR_MSK_ON		; PORT_C6 MASK ON !
	OUT	SYS_PORT,AL		;
				;
;------	Semi -> Full
 	mov	dx,PRN_EXMOD		;expanded mode register
	mov	al,PRN_INT_MOD		;01h -> port 448h
					;INT disable
	out	dx,al			;Now it's INT mode!
;------	8255 Mode Set
	MOV	AL,PRCEN_HMOD		; SET 8255A INITIALIZE SIGNAL
	OUT	PR_MODE,AL		; 8255A	PORTC MODE SET

	MOV	AL,IPrimeResetMask	; Input prime MASK OFF
	OUT	PR_MODE,AL		;

	sti
	mov	ecx,0ffffh
VPD_IPrimeLoop2:
	out	5fh,al
	loop	VPD_IPrimeLoop2		; Wait till initialization complete
	cli

	MOV	AL,IPrimeSetMask	; Input prime MASK ON !
	OUT	PR_MODE,AL		;

;------	initial Port1
	mov	al,00h			;initial value
	OUT	PR_DATA,AL		; PRINT	THE CHARACTER

;------	PSTB HIGH 
	MOV	AL,05h			;
	OUT	PR_WSIG,AL
					;
;------	PSTB OFF(Port C)
	MOV	AL,PR_MSK_OFF		; PORT-C6 MASK OFF !
	OUT	SYS_PORT,AL

	pop	ecx
	pop	edx
	popfd
	jmp	SHORT VPD_End_Set_IF
	
VPD_Fullcen_ModeSet:				;full-centro

;
;Change interface mode (fullcentro -> semicentro)
;


	push	esi

	mov	esi, VPD_BIOS_Timeout_Addr	;
	add	esi, [ebx.CB_High_Linear]	; esi = point to 0:458h
	btr	WORD PTR [esi],02h		; 0:458=02h

	pop	esi

	mov	dx,448h
	in	al,dx				;Curr IF Mode?
	test	al,01h

	jz	SHORT VPD_END_Set_IF				;semicen,then skip

	pushfd
	cli				;disable interrupt

	push	edx

;------	PSTB ON(Port C)
	MOV	AL,PR_MSK_ON		; PORT_C6 MASK ON !
	OUT	SYS_PORT,AL		;
				;
;------	Full -> Semi
	mov	dx,PRN_EXMOD		;expanded mode register
	mov	al,PRN_CompatibleMode	;00h -> port 448h
	out	dx,al			;Now it's compat-mode!

;------	8255 Mode Set
	MOV	AL,PRCEN_MOD		; SET 8255A INITIALIZE SIGNAL
	OUT	PR_MODE,AL		; 8255A	PORTC MODE SET
	
	OUT	5FH,AL			;delay
	OUT	5FH,AL
	OUT	5FH,AL
	OUT	5FH,AL

;------	PSTB HIGH
	MOV	AL,PRCEN_STBON			;
	OUT	PR_WSIG,AL
			;

;------	PSTB OFF(Port C)
	MOV	AL,PR_MSK_OFF		; PORT-C6 MASK OFF !
	OUT	SYS_PORT,AL
	pop	edx
	popfd			;

VPD_END_Set_IF:
	pop	edx
	pop	eax
	pop	ecx
	ret
EndProc VPD_Set_Interface


;******************************************************************************
;   VPD_BD_Set_Interface
;
;   DESCRIPTION:
;		THIS ROUTIN IS ADDED FOR Bi-Direction Support PC
;
;   ENTRY:
;		EBX = VM HANDLE OF NEW OWNER
;
;==============================================================================
BeginProc VPD_BD_Set_Interface

	push	ecx
	push	eax
	push	edx
	push	esi

	test	[VPD_BIBD_BYTE],02h		; Q: Exist PRTDRV.SYS?
	jz	VPD_BD_END_Set_IF		;  No.

	xor	edx,edx				; Clear EDX
	mov	dx,[VPD_DRV_Seg]		; Set PRTDRV.SYS Segment
	mov	esi,edx
	shl	esi,4
	push	eax
	xor	eax,eax
	mov	ax,[VPD_DRV_Off]
	add	esi,eax
	pop	eax
	add	esi, [ebx.CB_High_Linear]	; ESI = Point to PRTDRV.SYS

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data

	cmp	[VPD_BI_PORT],3			; Q: Max port num =3?
	je	SHORT VPD_BD_Set_IF_Port3	;  Yes.
						;  No.

	mov	ecx,VPD_LPT2_IF_Mode_Bit	; Board Port 2 is Full ?
	bt	DWORD PTR [eax], ecx		;
	jnc	SHORT VPD_BD_Set_IF_Port22_NFull ;

	and	byte ptr [esi],01111111b	; Bit07 OFF
	jmp	SHORT VPD_BD_Set_IF_Port1

VPD_BD_Set_IF_Port22_NFull:
	or	byte ptr [esi],10000000b	; Bit07 ON

VPD_BD_Set_IF_Port1:
	mov	ecx,VPD_LPT1_IF_Mode_Bit	; Board Port 1 is Full ?
	bt	DWORD PTR [eax], ecx		;
	jnc	SHORT VPD_BD_Set_IF_Port1_NFull ;

	and	byte ptr [esi],10111111b	; Bit06 OFF

	pop	esi
	mov	ecx, [esi.VPD_LPT_Number]	; ECX = LPT number
	cmp	ecx,1				; Owner Set is LPT1?
	push	esi				; Stack is Safety !
	jne	SHORT VPD_BD_END_Set_IF		; Go END

	VMMcall	Test_Sys_VM_Handle
	je	VPD_BD_END_Set_IF

	push	eax
	push	edx

	mov	dx,0549h			; Board TOKI Control Port
	mov	al,10h				; TOKI ON
	out	dx,al

	pop	edx
	pop	eax

	jmp	SHORT VPD_BD_END_Set_IF		; Go END

VPD_BD_Set_IF_Port1_NFull:
	or	byte ptr [esi],01000000b	; Bit06 ON

	pop	esi
	mov	ecx, [esi.VPD_LPT_Number]	; ECX = LPT number
	cmp	ecx,1				; Owner Set is LPT1?
	push	esi				; Stack is Safety !
	jne	SHORT VPD_BD_END_Set_IF		; Go END

	VMMcall	Test_Sys_VM_Handle
	je	VPD_BD_END_Set_IF

	push	eax
	push	edx

	mov	dx,0549h			; Board TOKI Control Port
	mov	al,00h				; TOKI OFF
	out	dx,al

	pop	edx
	pop	eax

	jmp	SHORT VPD_BD_END_Set_IF		; Go END

VPD_BD_Set_IF_Port3:
	mov	ecx,VPD_LPT3_IF_Mode_Bit	; Board Port 2 is Full ?
	bt	DWORD PTR [eax], ecx		;
	jnc	SHORT VPD_BD_Set_IF_Port3_NFull	;

	and	byte ptr [esi],01111111b	; Bit07 OFF
	jmp	SHORT VPD_BD_Set_IF_Port2	; Go LPT2 Check

VPD_BD_Set_IF_Port3_NFull:
	or	byte ptr [esi],10000000b	; Bit07 ON

VPD_BD_Set_IF_Port2:
	mov	ecx,VPD_LPT2_IF_Mode_Bit	; Board Port 1 is Full ?
	bt	DWORD PTR [eax], ecx		;
	jnc	SHORT VPD_BD_Set_IF_Port2_NFull	;

	and	byte ptr [esi],10111111b	; Bit06 OFF
	jmp	SHORT VPD_BD_END_Set_IF		; Go END

VPD_BD_Set_IF_Port2_NFull:
	or	byte ptr [esi],01000000b	; Bit06 ON

VPD_BD_END_Set_IF:
	pop	esi
	pop	edx
	pop	eax
	pop	ecx
	ret

EndProc VPD_BD_Set_Interface


;******************************************************************************
;   VPD_BI_Set_Interface
;
;   DESCRIPTION:
;		THIS ROUTIN IS ADDED FOR Bi-Direction Support PC
;
;   ENTRY:
;		EBX = VM HANDLE OF NEW OWNER
;
;==============================================================================
BeginProc VPD_BI_Set_Interface

	push	ecx
	push	eax
	push	edx

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, VPD_LPT1_IF_Mode_Bit	; ECX = LPT number
	bt	DWORD PTR [eax], ecx		; Full Cen. ?

	jc	SHORT VPD_BI_TOKI_ModeSet 	; Yes, then jump

	call	VPD_BI_TOKI_to_98		; Call 98 Mode Set Routine

	jmp	SHORT VPD_BI_END_Set_IF		; Exit

VPD_BI_TOKI_ModeSet:
VPD_BI_AT_ModeSet:
	call	VPD_BI_Set_AT_Mode		; Call AT Mode Set Routine

	jmp	SHORT VPD_BI_END_Set_IF		; Exit

VPD_BI_END_Set_IF:
	pop	edx
	pop	eax
	pop	ecx
	ret

EndProc VPD_BI_Set_Interface


;******************************************************************************
;   VPD_BI_TOKI_to_98
;
;   DESCRIPTION:
;		THIS ROUTIN IS ADDED FOR Bi-Direction Support PC
;
;   ENTRY:
;
;==============================================================================
BeginProc VPD_BI_TOKI_to_98

	push	esi				;

	mov	esi, VPD_BIOS_Timeout_Addr	;
	add	esi, [ebx.CB_High_Linear]	; esi = point to 0:458h
	btr	WORD PTR [esi],02h		; 0:458=02h

	mov	esi, VPD_BIOS_BI_Addr		; 
	add	esi, [ebx.CB_High_Linear]	; esi = point to 0:5B3h
	btr	WORD PTR [esi],05h		; 0:5B3=80h

	pop	esi

	VMMcall	Test_Sys_VM_Handle
	jne	VPD_BI_TOKI_to_98_KILL
	ret

VPD_BI_TOKI_to_98_KILL:

	cli				; Disable Interrupt

	push	edx
	push	eax
	push	ecx

	mov	al,TOKI_CTRMSK		; al <- 04h
	mov	dx,TOKI_PRCTR		; dx <- 142h
	out	dx,al			;

	mov	al,TOKI_OFF		; al <- 00h
	mov	dx,TOKI_TKSTS		; dx <- 149h
	out	dx,al			;

VPD_BI_SEMI_INIT:
	MOV	AL,PR_MSK_ON		; PORT_C6 MASK ON !
	OUT	SYS_PORT,AL		;

	OUT	5FH,AL
	OUT	5FH,AL

	MOV	AL,PRCEN_MOD		; SET 8255A INITIALIZE SIGNAL
	OUT	PR_MODE,AL		; 8255A	PORTC MODE SET

	OUT	5FH,AL			;delay
	OUT	5FH,AL

	MOV	AL,IPrimeResetMask	; Input prime MASK OFF
	OUT	PR_MODE,AL		;

	sti
	mov	ecx,0ffffh
VPD_BI_IPrimeLoop1:
	out	5fh,al
	loop	VPD_BI_IPrimeLoop1	; Wait till initialization complete
	cli

	MOV	AL,IPrimeSetMask	; Input prime MASK ON !
	OUT	PR_MODE,AL		;

	OUT	5FH,AL			;delay
	OUT	5FH,AL

	MOV	AL,PRCEN_STBON		;
	OUT	PR_WSIG,AL

	OUT	5FH,AL			;delay
	OUT	5FH,AL

	MOV	AL,PR_MSK_OFF		; PORT-C6 MASK OFF !
	OUT	SYS_PORT,AL

	pop	ecx
	pop	eax
	pop	edx

	sti				; Enable Interrupt

	ret

EndProc VPD_BI_TOKI_to_98


;******************************************************************************
;   VPD_BI_Set_AT_Mode
;
;   DESCRIPTION:
;		THIS ROUTIN IS ADDED FOR Bi-Direction Support PC
;
;   ENTRY:
;
;==============================================================================
BeginProc VPD_BI_Set_AT_Mode

	push	esi				;

	mov	esi, VPD_BIOS_Timeout_Addr	;
	add	esi, [ebx.CB_High_Linear]	; esi = point to 0:458h
	bts	WORD PTR [esi],02h		; 0:458=06h

	mov	esi, VPD_BIOS_BI_Addr		; 
	add	esi, [ebx.CB_High_Linear]	; esi = point to 0:5B3h
	btr	WORD PTR [esi],05h		; 0:5B3=80h

	pop	esi

	VMMcall	Test_Sys_VM_Handle
	jne	VPD_BI_Set_AT_Mode_KILL
	ret

VPD_BI_Set_AT_Mode_KILL:

	cli				; Disable Interrupt

	push	edx
	push	eax

	MOV	AL,TOKI_ON
	MOV	DX,TOKI_TKSTS
	OUT	DX,AL

VPD_BI_AT_NotSEMI:
	MOV	AL,TOKI_AT
	MOV	DX,TOKI_EXTRA
	OUT	DX,AL

	MOV	AL,TOKI_CTRMSK
	MOV	DX,TOKI_PRCTR
	OUT	DX,AL

	jmp	SHORT VPD_BI_AT_INI_END		;

	mov	al,TOKI_INIon		; al <- 00h
	mov	dx,TOKI_PRCTR		; dx <- 142h
	out	dx,al			;

	mov	al,TOKI_INIoff		; al <- 04h
	mov	dx,TOKI_PRCTR		; dx <- 142h
	out	dx,al			;

VPD_BI_AT_INI_END:

	pop	eax
	pop	edx

	sti				; Enable Interrupt

	ret

EndProc VPD_BI_Set_AT_Mode
ENDIF ;NEC_98


;******************************************************************************
;
;   VPD_Test_And_Set_Owner
;
;   DESCRIPTION:
;       This procedure will test to see if the current VM owns the printer
;	port.  This procedure has several possible results:
;
;	If no VM owns the printer then the printer will be assigned to
;	    the current VM.
;	If another VM owns the printer but has timed-out the current VM
;	    will get ownersimp of the printer.
;	If another VM owns the printer and has not timed-out AND the current
;	    VM has never contended for the LPT port then the user will be
;	    presented with a contention dialogue box had he/she will be
;	    given the opportunity to select the VM that is allowed to use
;	    the printer.
;	If another VM owns the printer and has not timed-out and the current
;	    VM has previously contended for the printer then this procedure
;	    will return with carry set to indicate that an error status should
;	    be returned for the current VM.
;
;   ENTRY:
;	EBX = VM to assign printer to
;	ESI -> LPT data structure
;
;   EXIT:
;	Returns with carry clear if VM owns the printer port.  Returns with
;	carry flag set if error / dummy data should be returned.  If contention
;	event is generated then this procedure will not return.
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VPD_Test_And_Set_Owner, High_Freq
	push	eax				; We'll use EAX

	cmp	[esi.VPD_Owner], ebx		; Q: Does this VM own it?
	je	SHORT VPD_TASO_Assigned 	;    Y: Done!
ifdef MAXDEBUG
	Trace_Out "VPD_Test_And_Set_Owner (VM=#EBX)"
endif
	cmp	[esi.VPD_Owner], 0		; Q: Does any VM own printer?
	je	SHORT VPD_TASO_OK_To_Assign	;    N: Assign it to this VM
						;    Y: Test for time-out

	TestMem	[esi.VPD_Flags], VPDF_Special_Owner
	jnz	SHORT VPD_TASO_Contention

	cmp	[esi.VPD_Timeout_Limit], -1	; Q: Can we time-out?
	je	SHORT VPD_TASO_Contention	;    N: Skip this part
						;    Y: See if it happened
ifdef NEC_98
        VMMcall Test_Sys_VM_Handle              ; Q: Called from Windows?
	je	VPD_TASO_OK_To_Assign		;    Yes.
endif ;NEC_98

	VMMCall Get_System_Time 		; EAX = Current system time
	sub	eax, [esi.VPD_Last_Touched]	; EAX = Elapsed time
	cmp	eax, [esi.VPD_Timeout_Limit]	; Q: Has owner timed-out?
	jb	SHORT VPD_TASO_Contention	;    N: Resolve this contention
						;    Y: OK to change owners
VPD_TASO_OK_To_Assign:
	xor	ecx, ecx			; NOT a special owner
	call	VPD_Attach			; New LPT owner = VM in EBX

VPD_TASO_Assigned:
	VMMCall Get_System_Time 		; EAX = Current system time
	mov	[esi.VPD_Last_Touched], eax	; Save this for time-out
	pop	eax				; Restore EAX
	clc					; Return with carry clear
	ret					; (VM owns printer)

;
;   The printer is currently begin used by another VM.
;
VPD_TASO_Contention:
	pushad

ifdef MAXDEBUG
	Trace_Out "VPD_Test_And_Set_Owner, shell resolved contension"
endif
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, [esi.VPD_LPT_Number]	; ECX = LPT number
	bts	DWORD PTR [eax], ecx		; Q: Has VM contended already?
	jc	SHORT VPD_TASO_Assign_Failed	;    Y: Don't ask user again
						;    N: Display a dialogue box
	mov	eax, [esi.VPD_Owner]		; EAX = Current owner
	push	esi				; EBX = Contender
	lea	esi, [esi.VPD_Name]		; ESI -> Device name ("LPTx")
	VxDcall SHELL_Resolve_Contention	; EBX = Contention winner
	pop	esi
	mov	eax, [esi.VPD_Owner]
IFDEF NEC_98
	cmp	eax, 0				; Q. No owner? |  owner VM has
	je	SHORT VPD_TASO_Assign_Failed	;  Y: VM lost  +  Exit.
ENDIF ;NEC_98
	cmp	ebx, eax			; Q: Same owner?
	je	SHORT VPD_TASO_Assign_Failed	;    Y: VM lost contention

	mov	ecx, [esi.VPD_LPT_Number]	; ECX = LPT number
	add	eax, [VPD_CB_Offset]		; EAX -> Contention flags
	bts	DWORD PTR [eax], ecx		; Set contention bit

	xor	ecx, ecx			; NOT a special owner
	call	VPD_Attach
	popad
	jmp	VPD_TASO_Assigned

VPD_TASO_Assign_Failed:
	popad
	pop	eax
	stc
	ret

EndProc VPD_Test_And_Set_Owner

IFDEF NEC_98
;******************************************************************************
;   VPD_Test_Owner
;
;   DESCRIPTION:
;	This procedure check if VMx can own LPT, but this VMx never own LPT.
;
;   ENTRY:
;	EBX = VM to assign printer to
;	ESI -> LPT data structure
;
;   EXIT:
;	Returns with carry clear if VM can own the printer port.  Returns with
;	carry flag set if error / dummy data should be returned.
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VPD_Test_Owner
	push	eax				; We'll use EAX

	cmp	[esi.VPD_Owner], ebx		; Q: Does this VM own it?
	je	SHORT VPD_LPT_Owner	 	;    Y: return CF clear
	cmp	[esi.VPD_Owner], 0		; Q: Does any VM own printer?
	je	SHORT VPD_LPT_Owner		;    N: return CF clear
						;    Y: special owner check
	test	[esi.VPD_Flags], VPDF_Special_Owner; Q: Is curr owner special?
	jnz	SHORT VPD_Not_Owner		;	Y: return CF set

	cmp	[esi.VPD_Timeout_Limit], -1	; Q: Can we time-out?
	je	SHORT VPD_Not_Owner		;    N: return CF set
						;    Y: check timeout
	VMMCall Get_System_Time 		; EAX = Current system time
	sub	eax, [esi.VPD_Last_Touched]	; EAX = Elapsed time
	cmp	eax, [esi.VPD_Timeout_Limit]	; Q: Has owner timed-out?
	jb	SHORT VPD_Not_Owner		;    N: ret CF set
						;    Y: ret CF clear
VPD_LPT_Owner:
	clc
	jmp	SHORT VPD_TO_Exit
VPD_Not_Owner:
	stc
VPD_TO_Exit:
	pop	eax
	ret

EndProc VPD_Test_Owner
ENDIF ;NEC_98

if	WPS
;******************************************************************************
;
;   VJPD_Jumbo_PCL_Mode_Switch
;
;   DESCRIPTION:
;	Sends the Jumbo GoToPCL command out the port.
;
;   ENTRY:
;	EBX = Handle of VM to own LPT
;	ESI -> LPT_Struct (Moved to EDI)
;
;   EXIT:
;	None
;
;   USES:
;	None(All Pushed).
;
;==============================================================================

BeginProc VJPD_Jumbo_PCL_Mode_Switch
	pushad

ifdef MAXDEBUG
	mov	eax, esi
	Trace_Out "VJPD_Jumbo_PCL_Mode_Switch (VM=#EBX, LPT Struc=#EAX)"
endif
;
; Save the LPT struct in EDI
; Get the string base address and the string size depending on WPS type
;
	mov	edi, esi

        bt      [edi.VPD_Flags], VJPDF_NONJumbo_Bit ; Q: Jumbo printer?
        jnc     SHORT VJPD_JPMS_Jumbo               ;   Y: Use Jumbo string
                                                    ;   N: Use non-WPS string

; non Jumbo
        mov     esi, [edi.VJPD_Non_WPS_Mode]        ; ESI -> mode switch
        mov     ecx, [edi.VJPD_Non_WPS_Mode_Size]   ; ECX = mode switch size
        jcxz    VJPD_JPMS_Exit                      ; Exit if zero length
        jmp     SHORT VJPD_JPMS_Send

VJPD_JPMS_Jumbo:
	lea	esi, VJPD_Jumbo_PCL_Mode	    ; ESI -> mode switch
	mov	ecx, [VJPD_Jumbo_PCL_Mode_Size]	    ; ECX = mode switch size
;
; Output the string
;
VJPD_JPMS_Send:
	call	VJPD_Send_LPT_String

VJPD_JPMS_Exit:
ifdef NEC_98
        mov     edx, [edi.VPD_IO_Base]
	or	dh, dh				;TOKI Port?
	jz	SHORT VJPD_SEN_Not_TOKI		;No.
	mov	dl, Low_TOKI_CONTROL
	in	al, dx
	mov	ah, al				;Store al
	cmp	al, 0FFh			;Support TOKI?
	je	SHORT VJPD_SEN_Not_TOKI		;No.
	test	eax, MODE_MASK			;Yes. Active AT_Mode?
	jnz	SHORT VJPD_SEN_AT		;Yes.
	mov	al, AT_MODE
	out	dx, al				;No. Set AT_Mode
VJPD_SEN_AT:
	mov	dl, Low_AT_CONTROL
	in	al, dx				;in AT_CONTROL
	and	al, 0F7h			;Clear Sel_In
	out	dx, al
	mov	dl, Low_TOKI_CONTROL
	mov	al, ah				;Restore al
	out	dx, al				;Set Defaule Mode
VJPD_SEN_Not_TOKI:
endif ;NEC_98
	popad
	ret
EndProc VJPD_Jumbo_PCL_Mode_Switch

;******************************************************************************
;
;   VJPD_Nulls_After_Mode_Switch
;
;   DESCRIPTION:
;	Sends 128 nulls out the port.	This covers up a
;	printer bug that eats data when printer first comes online
;
;   ENTRY:
;	EBX = Handle of VM to own LPT
;	ESI -> LPT_Struct (Moved to EDI)
;
;   EXIT:
;	None
;
;   USES:
;	None(All Pushed).
;
;==============================================================================

BeginProc VJPD_Nulls_After_Mode_Switch
	pushad
	mov	edi, esi
ifdef MAXDEBUG
	Trace_Out "VJPD_Nulls_After_Mode_Switch"
endif
	cld

;
; Wait for port to stabalize from mode switch before sending nulls
;
	or	[edi.VPD_Flags], VJPDF_PCLSwitch
	VMMcall	Begin_Nest_Exec
ifdef MAXDEBUG
	Trace_Out "In Nest Exec"
endif

        VMMcall Get_System_Time
	add	eax, VJPD_Hide_Time
	mov	esi, eax
@@:
	VMMcall	Resume_Exec
        VMMcall Get_System_Time
	cmp	eax, esi
	jc	@B
ifdef MAXDEBUG
	Trace_Out "Done hiding, start checking port"
endif

;
; Wait for mode switch
;
        VMMcall Get_System_Time
	add	eax, VJPD_Switch_Wait_Time
	mov	esi, eax

	mov	edx, [edi.VPD_IO_Base]		    ; EDX = Base I/O port
	inc	dx

;
; Loop till printer ready or timer expires
ifdef NEC_98
	cmp	[edi.VPD_LPT_Number], 1
	jne	VJPD_JPMS_Wait
	cmp	edx, LPT1_Base+Low_AT_DATA
	jne	SHORT @f
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h
	End_Touch_1st_Meg
	jz	SHORT VJPD_JPMS_98
@@:
	mov	dl, Low_TOKI_CONTROL
	in	al, dx
	mov	dl, Low_AT_STATUS
	test	al, AT_MODE
	jnz	SHORT VJPD_JPMS_Wait

VJPD_JPMS_98:
	mov	edx, 042h
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h		; H98?
	End_Touch_1st_Meg
	jz	SHORT VJPD_JPMS_Semi
	mov	dx, 448h
	in	al, dx
	test	al, 01h
	jnz	SHORT VJPD_JPMS_Full

VJPD_JPMS_Semi:
	VMMcall	Resume_Exec
        in      al, dx				; Read the status into AL
        and     al, 04h
        cmp     al, 04h				; Q: Printer ready?
        je      SHORT VJPD_JPMS_Wait_Done

        VMMcall Get_System_Time
	cmp	eax, esi
	jc	VJPD_JPMS_Semi

VJPD_JPMS_Full:
	VMMcall	Resume_Exec
        in      al, dx				; Read the status into AL
        and     al, 04h
        cmp     al, 04h				; Q: Printer ready?
        je      SHORT VJPD_JPMS_Wait_Done

        VMMcall Get_System_Time
	cmp	eax, esi
	jc	VJPD_JPMS_Full

VJPD_JPMS_Wait:
endif ;NEC_98
@@:
	VMMcall	Resume_Exec
        in      al, dx                              ; Read the status into AL
        and     al, VJPD_Status_Ready_Mask
        cmp     al, VJPD_Status_Ready               ; Q: Printer ready?
        je      SHORT VJPD_JPMS_Wait_Done

        VMMcall Get_System_Time
	cmp	eax, esi
	jc	@B

VJPD_JPMS_Wait_Done:
        VMMcall End_Nest_Exec
	and	[edi.VPD_Flags], NOT VJPDF_PCLSwitch
ifdef MAXDEBUG
	Trace_Out "Mode switch complete, now send nulls"
endif

;
; Acorn does not need the Nulls, just the wait
;
	bt      [edi.VPD_Flags], VJPDF_NONJumbo_Bit
	jc      SHORT VJPD_SNL_Exit
;
; Set the LPT mode switch timeout value (Timeout + Current_System_Time)
;
ifdef MAXDEBUG
	Trace_Out "Actually sending the nulls now"
endif
	VMMcall Get_System_Time
	add	eax, [VJPD_Send_Switch_Limit]
	mov	[edi.VJPD_Send_Switch_Timeout], eax
;
; Get the LPT Base I/O Port Address
;
	mov	edx, [edi.VPD_IO_Base]
	in	al, dx
	mov	ah, al
ifdef NEC_98
	cmp	[edi.VPD_LPT_Number], 1
	jne	@f
	mov	edx, 040h
	in	al, dx
	mov	edx, [edi.VPD_IO_Base]
@@:
	push	eax
endif ;NEC_98

	mov     ecx, [VJPD_Null_Size]

VJPD_SNL_Start:
;
; Get the next character to send
;
	xor	eax, eax
	call	VJPD_Send_LPT_Char
	jc	SHORT VJPD_SNL_Done		; If timeout
	loop	VJPD_SNL_Start
	clc
;
; Done with the string
;
VJPD_SNL_Done:
ifdef NEC_98
	pop	eax
	cmp	[edi.VPD_LPT_Number], 1
	jne	@f
	mov	edx, 040h
	out	dx, al
	mov	edx, [edi.VPD_IO_Base]
@@:
endif ;NEC_98
	mov	al, ah
	out	dx, al
VJPD_SNL_Exit:
ifdef MAXDEBUG
	Trace_Out "VJPD_Nulls_After_Mode_Switch exit"
endif
	popad
	ret

EndProc VJPD_Nulls_After_Mode_Switch

;******************************************************************************
;
;   VJPD_Send_LPT_String
;
;   DESCRIPTION:
;	Sends a string to an LPT port
;
;   ENTRY:
;	EBX = Handle of VM to own LPT
;	EDI -> LPT_Struct
;	ESI -> String to Send
;	ECX = Size of String
;
;   EXIT:
;	Carry if timed-out
;
;   USES:
;	None(All Pushed).
;
;==============================================================================


BeginProc VJPD_Send_LPT_String
	pushad
ifdef MAXDEBUG
	Trace_Out "VJPD_Send_LPT_String"
endif
	cld

ifdef NEC_98
        cmp     [edi.VPD_LPT_Number], 1		;LPT No = 1 ?
	jne	SHORT VJPD_SLS_AT		;No.
        mov     edx, [edi.VPD_IO_Base]
	cmp	edx, LPT2_Base+Low_AT_DATA	;Base Address = 540h ?
	je	SHORT VJPD_SLS_LPT2		;Yes.
VJPD_SLS_LPT1:
	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[05B3h], 80h	;AT Mode?
	End_Touch_1st_Meg
	jz	SHORT VJPD_SLS_98		;No.
VJPD_SLS_LPT2:
	push	edx
	mov	dl, Low_TOKI_CONTROL		;edx=0x49h
	in	al, dx
	pop	edx
	cmp	al, 0FFh			;Active AT Mode ?
	je	SHORT VJPD_SLS_98		;No.
	test	al, 10h				;TOKI AT Mode?
	jnz	SHORT VJPD_SLS_AT		;Yes.
	jmp	SHORT VJPD_SLS_98		;No.

VJPD_SLS_98:
;
; Set the LPT mode switch timeout value (Timeout + Current_System_Time)
;
        VMMcall Get_System_Time
        add     eax, [VJPD_Send_Switch_Limit]
        mov     [edi.VJPD_Send_Switch_Timeout], eax
;
; Get the LPT Base I/O Port Address
;
        mov     edx, 40h
        in      al, dx
	push	eax

	Begin_Touch_1st_Meg
	test	BYTE PTR DS:[0458h], 80h	; H98?
	End_Touch_1st_Meg
	jz	SHORT VJPD_SLS_Semi		;No.
	push	edx				;Save EDX
	mov	edx, 448h
	in	al, dx
	pop	edx				;Restore EDX
	test	al, 01h				;FUll Sentro?
	jz	SHORT VJPD_SLS_Semi		;No.
VJPD_SLS_Full:
	mov	ah, 04h				;Yes.
	jmp	SHORT VJPD_SLS_Start98
VJPD_SLS_Semi:
	mov	ah, 0Eh

VJPD_SLS_Start98:
;
; Get the next character to send
;
        lodsb
        call    VJPD_Send_LPT_Char98
        jc      SHORT VJPD_SLS_Exit             ; If timeout
        loop    VJPD_SLS_Start98
        clc
	jmp	SHORT VJPD_SLS_Exit

VJPD_SLS_AT:
endif ;NEC_98
;
; Set the LPT mode switch timeout value (Timeout + Current_System_Time)
;
	VMMcall Get_System_Time
	add	eax, [VJPD_Send_Switch_Limit]
	mov	[edi.VJPD_Send_Switch_Timeout], eax
;
; Get the LPT Base I/O Port Address
;
	mov	edx, [edi.VPD_IO_Base]
	in	al, dx
ifdef NEC_98
	push	eax
else ;NEC_98
	mov	ah, al
endif ;NEC_98

VJPD_SLS_Start:
;
; Get the next character to send
;
	lodsb
	call	VJPD_Send_LPT_Char
	jc	SHORT VJPD_SLS_Exit		; If timeout
	loop	VJPD_SLS_Start
	clc
;
; Done with the string
;
VJPD_SLS_Exit:
ifdef NEC_98
	pop	eax
else ;NEC_98
	mov	al, ah
endif ;NEC_98
	out	dx, al
	popad
	ret
EndProc VJPD_Send_LPT_String

;******************************************************************************
;
;   VJPD_Send_LPT_Char
;
;   DESCRIPTION:
;	Sends a single char to an LPT port
;
;   ENTRY:
;	EAX = Value to Output
;	EBX = Handle of VM to own LPT
;	EDI -> LPT_Struct
;   EXIT:
;	Carry = Timeout
;   USES:
;	None(All Pushed).
;
;==============================================================================


BeginProc VJPD_Send_LPT_Char
	pushad

;
; Save the char to be output
;
	push	eax
;
; Get the base LPT I/O port
;
	mov	edx, [edi.VPD_IO_Base]
;
; Select the status line port
;
	inc	dx
;
; Check for a timeout
;
VJPD_SLC_Check_Timeout:
	VMMcall Get_System_Time
	cmp	[edi.VJPD_Send_Switch_Timeout], eax
	jbe	SHORT VJPD_SLC_Timeout
;
; Read the status port for "Not Busy"
;
VJPD_SLC_Read_Status:
	in	al, dx
	test	al, VJPD_Not_Busy
	jnz	SHORT VJPD_SLC_Send_Char
;
; Let any outstanding events be processed...
;
	VMMcall Begin_Nest_Exec
	VMMcall Resume_Exec
	VMMcall End_Nest_Exec
	jmp	SHORT VJPD_SLC_Check_Timeout
;
; Output the character to the LPT data port
;
VJPD_SLC_Send_Char:
	dec	dx
	pop	eax
	OutP	dx, al
ifdef NEC_98
	out	5fh, al			;I/O Delay (1 micro sec)
endif ;NEC_98
;
; Select the control line port
;
	inc	dx
	inc	dx
;
; Set the data strobe control bit (Repeat for I/O bus delay)
;
	in	al, dx
        and     al, VJPD_Control_Mask
	or	al, VJPD_Data_Strobe
	OutP	dx, al
ifndef NEC_98
;
; Delay several cycles to allow for printer timing
;
	push	ecx
	mov	ecx, 100h
delay_loop:
	loop	SHORT delay_loop
	pop	ecx
endif ;NEC_98
;
; Clear the data strobe control line
;
	and	al, NOT VJPD_Data_Strobe
	OutP	dx, al
ifdef NEC_98
	out	5fh, al			;I/O Delay (1 micro sec)
endif ;NEC_98
;
; Done with this character.
;
VJPD_SLC_Done:
	popad
	clc
	ret
;
; Timeout occurred!
;
VJPD_SLC_Timeout:
	pop	eax

ifdef MAXDEBUG
	Trace_Out "Send Char Timeout!"
endif
	popad
	stc
	ret
EndProc VJPD_Send_LPT_Char

ifdef NEC_98
;******************************************************************************
;
;   VJPD_Send_LPT_Char98
;
;   DESCRIPTION:
;       Sends a single char to an LPT port
;
;   ENTRY:
;       EAX = Value to Output
;       EBX = Handle of VM to own LPT
;       EDI -> LPT_Struct
;   EXIT:
;       Carry = Timeout
;   USES:
;       None(All Pushed).
;
;==============================================================================


BeginProc VJPD_Send_LPT_Char98
        pushad

;
; Save the char to be output
;
        push    eax
;
; Check for a timeout
;
VJPD_SLC_Check_Timeout98:
        VMMcall Get_System_Time
        cmp     [edi.VJPD_Send_Switch_Timeout], eax
        jbe     SHORT VJPD_SLC_Timeout98
;
; Read the status port for "Not Busy"
;
VJPD_SLC_Read_Status98:
        in      al, 42h
        test    al, 00000100b		;Busy?
        jnz     SHORT VJPD_SLC_Send_Char98
;
; Let any outstanding events be processed...
;
        VMMcall Begin_Nest_Exec
        VMMcall Resume_Exec
        VMMcall End_Nest_Exec
        jmp     SHORT VJPD_SLC_Check_Timeout98
;
; Output the character to the LPT data port
;
VJPD_SLC_Send_Char98:
        pop     eax
        OutP    40h, al
	out	5fh, al			;I/O Delay (1 micro sec)
ifdef MAXDEBUG
        Trace_Out "#AL ",0
endif
;
; Set the data strobe control bit (Repeat for I/O bus delay)
;
	mov	al, ah			;Strobe On
        OutP    46h, al
	out	5fh, al			;I/O Delay (1 micro sec)
;
; Clear the data strobe control line
;
	inc	al			;Strobe Off
        OutP    46h, al
	out	5fh, al			;I/O Delay (1 micro sec)
;
; Done with this character.
;
VJPD_SLC_Done98:
        popad
        clc
        ret
;
; Timeout occurred!
;
VJPD_SLC_Timeout98:
        pop     eax

ifdef MAXDEBUG
	Trace_Out "Send Char Timeout!"
endif
        popad
        stc
        ret
EndProc VJPD_Send_LPT_Char98
endif ;NEC_98
endif ; WPS

;******************************************************************************
;		       D E B U G G I N G   C O D E
;******************************************************************************

;******************************************************************************
;
;   VPD_Debug_Query
;
;   DESCRIPTION:
;	This procedure is only assembled in the debug version of VPD.  It
;	will dump the status of each LPT port to the debugging terminal and
;	display the state of SGRAB ownership if SGRAB is enabled.
;
;	Note that since this procedure can be called at any time by the
;	debugger it is not allowed to call any non-asynchronous services and
;	it must be in a LOCKED code segment.
;
;   ENTRY:
;	EAX = Debug_Query (equate)
;
;   EXIT:
;	None
;
;   USES:
;	EBX, ECX, EDX, ESI, Flags
;
;==============================================================================

IFDEF DEBUG

BeginProc VPD_Debug_Query

	Trace_Out "LPT#   Type         Owner     Base Port"

	mov	ecx, 1				; ECX = LPT #
	mov	esi, OFFSET32 VPD_LPT1		; ESI -> First LPT structure
VPD_DQ_LPT_Loop:
	mov	ebx, [esi.VPD_Owner]		; EBX = VM handle of owner
	mov	edx, [esi.VPD_IO_Base]		; ESI = Base I/O port

ifdef NEC_98
	mov	eax, [esi.VPD_Flags]
	cmp	dl, 040h			; Q: Port # == 0x40h?
	jne	SHORT VPD_DQ_Not_Hardware	;    N: Not a hardware port
else ;NEC_98
	test	dh, dh				; Q: Port # > 0FFh?
	jz	SHORT VPD_DQ_Not_Hardware	;    N: Not a hardware port
endif ;NEC_98
						;    Y: Print hardware info
	Trace_Out " #CL   Hardware    #EBX      #DX"
	jmp	SHORT VPD_DQ_Next_LPT

VPD_DQ_Not_Hardware:				; Not a hardware LPT
	test	edx, edx			; Q: Does it even exist?
	jz	SHORT VPD_DQ_None		;    N: No LPT here
	Trace_Out " #CL   Network     #EBX"	;    Y: Network LPT
	jmp	SHORT VPD_DQ_Next_LPT
VPD_DQ_None:
	Trace_Out " #CL   (None)"
	jmp	SHORT VPD_DQ_Next_LPT

VPD_DQ_Next_LPT:
	add	esi, SIZE VPD_Struc		; ESI -> Next LPT data structure
	inc	ecx				; Inc counter
ifdef NEC_98
	cmp	ecx, [VJPD_Max_Virt_LPTs]	; Q: End of list?
else ;NEC_98
	cmp	ecx, VPD_Max_Virt_LPTs		; Q: End of list?
endif ;NEC_98
	jbe	VPD_DQ_LPT_Loop 		;    N: Loop

	Trace_Out " "

	mov	ecx, [VPD_Sgrab_LPT]		; Q: Is SGRAB enabled?
	jecxz	VPD_DQ_Exit			;    N: Skip this part
						;    Y: Report sgrab LPT number
	mov	ecx, [ecx.VPD_LPT_Number]
	Trace_Out "SGRAB LPT = #CL"
	Trace_Out " "

VPD_DQ_Exit:
ifdef NEC_98
	Trace_Out "Owner        CB"
	VMMCall	Get_Cur_VM_Handle
VPD_Scan_Loop:
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx
	mov	eax, [eax]
	Trace_Out "#EBX     #EAX"
	VMMCall	Get_Next_VM_Handle
	VMMCall	Test_Cur_VM_Handle
	jne	VPD_Scan_Loop
	Trace_Out " "
endif ;NEC_98
	ret

EndProc VPD_Debug_Query

ENDIF

VxD_CODE_ENDS


	END VPD_Real_Mode_Init
