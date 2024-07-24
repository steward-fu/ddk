;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989-1992
;
;   Title:	VCD.inc -
;
;   Version:	2.00
;
;   Date:	3-May-1989
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   3-May-1989 RAP
;   28-Apr-1992 RAP added VCD_Acquire_Port and VCD_Free_Port services
;
;==============================================================================

Begin_Service_Table VCD

VCD_Service	VCD_Get_Version, LOCAL
VCD_Service	VCD_Set_Port_Global, LOCAL
VCD_Service	VCD_Get_Focus, LOCAL
VCD_Service	VCD_Virtualize_Port, LOCAL

; New for WFW 3.10

VCD_Service	VCD_Acquire_Port, LOCAL
VCD_Service	VCD_Free_Port, LOCAL

; New for WFW 3.11

VCD_Service	VCD_Acquire_Port_Windows_Style,LOCAL
VCD_Service	VCD_Free_Port_Windows_Style,LOCAL
VCD_Service	VCD_Steal_Port_Windows_Style,LOCAL

; New For Chicago

VCD_Service	VCD_Find_COM_Index,LOCAL
VCD_Service	VCD_Set_Port_Global_Special,LOCAL
VCD_Service	VCD_Virtualize_Port_Dynamic, LOCAL
VCD_Service	VCD_Unvirtualize_Port_Dynamic, LOCAL

End_Service_Table VCD

;***********
; PM API Services
;
VCD_PM_API_Get_Version		equ 0
VCD_PM_API_Get_Port_Array	equ 1
VCD_PM_API_Get_Port_Behavior	equ 2
VCD_PM_API_Set_Port_Behavior	equ 3
VCD_PM_API_Acquire_Port 	equ 4
VCD_PM_API_Free_Port		equ 5
VCD_PM_API_Steal_Port		equ 6

ifdef   NEC_98
;***********
;  Command Equates
;
CMD_TXE         equ     01h             ; Send Enable
CMD_DTR         equ     02h             ; Data Terminal Ready
CMD_RXE         equ     04h             ; Receive Enable
CMD_RTS         equ     20h             ; Request to Send

;***********
;  Status  Equates
;
STAT_TXR        equ     01h             ; Send Enable
STAT_RXR        equ     02h             ; Receive Enable
STAT_TXE        equ     04h             ; Send buffer empty
STAT_INT        equ     07h             ; Interrupt Enable
STAT_OE         equ     10h             ; Overrun Error


;***********
;  Interrupt Request  Equates
;
IR_TXR          equ     01h             ; Send Enable
IR_RXR          equ     04h             ; Receive Enable
IR_TXE          equ     01h             ; Send buffer empty


;***********
;  Mask Equates
;
MSK_RXR_E       equ     00000001b       ; 01h
MSK_TXE_E       equ     00000010b       ; 02h
MSK_TXR_E       equ     00000100b       ; 04h
MSK_INT_E       equ     00000111b       ; 07h

; Counter Control Word
MSK_CONT_RL     equ     00110000b       ; Read/Write Mask for Timer counter
COUNT_LSB_CHECK equ     00010000b
COUNT_MSB_CHECK equ     00100000b
;                         II
;                         I+------- RL0 
;                         +-------- RL1
;


;***********
; CCU Type
;
; If you want to add CCU type, Shift CCU_RESERVE's number
; and name CCU_RESERVE's number as CCU that you want
;
CCU_16550	equ	0
CCU_8251	equ	1
CCU_8251F	equ	2
CCU_8251F_IDX	equ	3
CCU_RESERVE	equ	4

endif

;***********
;  UART ports and offsets
;
UART_PORTS	equ	7		; # of ports for UART
UART_REG_MASK	equ	7

UART_BAUD_LSB	equ	0		; Least Signif. Byte of BAUD divisor
UART_BAUD_MSB	equ	1		; Most	Signif. Byte of BAUD divisor
UART_RBR	equ	0		; Receive buffer register
UART_THR	equ	0		; Transmit holding register
UART_IER	equ	1		; Interrupt enble register
UART_IIR	equ	2		; Interrupt identity register
UART_LCR	equ	3		; Line control register
UART_MCR	equ	4		; Modem control register
UART_LSR	equ	5		; Line status register
UART_MSR	equ	6		; Modem status register

;***********
;  Interrupt Enable Register Equates
;
IER_MS		equ	08h		; Modem Status
IER_LS		equ	04h		; Line Status
IER_THRE	equ	02h		; Transmit Holding Register Empty
IER_DR		equ	01h		; Data Ready

;***********
;  Interrupt Identity Register Equates
;
IIR_NONE	equ	01h	; No interrupt pending
IIR_LS		equ	06h	; Line Status Interrupt
IIR_DR		equ	04h	; Data Ready Interrupt
IIR_THRE	equ	02h	; Transmit Holding Register Empty
IIR_MS		equ	00h	; Modem Status Interrupt

;***********
;  Line Control Register Equates
;
LCR_DLAB	equ	80h	; Divisor Latch Access

;***********
;  Modem Control Register Equates
;
MCR_INTEN	equ	08h	; Interrupt Enable
MCR_RTS		equ	02h	; Request to Send
MCR_DTR 	equ	01h	; Data Terminal Ready
MCR_Loopback	equ	10h	; Enable Internal Loopback

;***********
;  Line Status Register Equates
;
LSR_THRE	equ	20h	; Transmit Holding Register Empty
LSR_DR		equ	01h	; Data Ready
LSR_OE		equ	02h	; Overrun Error
LSR_PE		equ	04h	; Parity Error
LSR_FE		equ	08h	; Frame Error
LSR_BI		equ	10h	; Break Detected
LSR_ERRBITS	equ	1Eh	; All the error bits
LSR_TXBITS	equ	60h	; THRE and TEMT

;***********
;  Modem Status Register Equates
;
MSR_DELTA	equ	0Fh	; Modem Status DELTA bits
MSR_DCTS	equ	01h	; Delta Clear to Send
MSR_DDSR	equ	02h	; Delta Data Set Ready
MSR_TERI	equ	04h	; Trailing Edge Ring Indicator
MSR_DRLSD	equ	08h	; Delta Receive Line Signal Detect
MSR_CTS 	equ	10h	; Clear to Send
MSR_DSR 	equ	20h	; Data Set Ready
MSR_RING	equ	40h	; Ring Indicator
MSR_RLSD	equ	80h	; Receive Line Signal Detect

;***********
;  Masks for read status flags
;
VCD_Read_LSR_Mask equ   01h     ; Read Line Status Register
VCD_Read_MSR_Mask equ   02h     ; Read Modem Status Register



;***********
;  Flag bits  for VCD_COM_Struc.VCD_Flags
;
VCD_global_port 	equ 00000001b
VCD_global_port_Bit	equ	   0
VCD_not_sharable_IRQ	equ 00000010b	; Set if IRQ is not sharable
VCD_not_sharable_IRQ_Bit equ	  1
VCD_IRQ_Init		equ 00000100b	; Set if COM attempted to virt IRQ
VCD_IRQ_Init_Bit	equ	 2
VCD_Owns_IRQ		equ 00001000b	; Set if COM DID virtualize IRQ
VCD_Owns_IRQ_Bit	equ	3
VCD_Windows_30Drvr	equ 00010000b	; Set if Windows 3.0 com driver
VCD_Windows_30Drvr_Bit	equ    4
VCD_SetOwner_Event	equ 00100000b	; Set if event scheduled to set owner
VCD_SetOwner_Event_Bit	equ   5
VCD_Windows_Port	equ 01000000b	; Set if Windows has control of this port
VCD_Windows_Port_Bit	equ  6
VCD_Virtualized 	equ 10000000b	; Set if VCD_Virtualize_Port was called
VCD_Virtualized_Bit	equ 7
VCD_Mouse_Port		equ 100000000b
VCD_Mouse_Port_Bit	equ 8
VCD_1st_on_IRQ		equ 1000000000b ; Set if first to virtualize IRQ
VCD_1st_on_IRQ_Bit	equ 9
VCD_Global_IRQ		equ 10000000000b
VCD_Global_IRQ_Bit	equ 10		; Set if IRQ was unmasked when VCD
					; tried to virtualize it for the 1st
					; time.
VCD_AltOwnerActive	equ 100000000000b
VCD_AltOwnerActiveBit	equ 11		; set if an alternate owner is waiting
					; to own the port. Basically we have
					; two possible (VxD/COMM.DRV) owners
					; and a DOS VM as the owner.
VCD_Dynamic_Port	equ 1000000000000b
VCD_Dynamic_Port_Bit	equ 12		; set if the port "arrives" after
					; device Init.

;***********
;  Flag bits  for VCD_CB_Struc.VCD_CB_Flags
;
VCD_Contended		equ 00000001b	; set if VM contended for port and lost
VCD_Contended_Bit	equ	   0
VCD_Assign_Failed	equ 00000010b	; set if VCD_Assign failed because port
VCD_Assign_Failed_Bit	equ	  1	;   was owned by a VxD and a msg has
					;   been displayed
VCD_CB_Windows_30Drvr	equ 00010000b	; Set if Windows 3.0 com driver
VCD_CB_Windows_30Drvr_Bit equ  4
VCD_CB_Windows_Port	equ 01000000b	; Set if Windows has control of this port
VCD_CB_Windows_Port_Bit equ  6
VCD_Touched		equ 10000000b
VCD_Touched_Bit 	equ 7
VCD_CB_Mouse_Port	equ 100000000b
VCD_CB_Mouse_Port_Bit	equ 8
VCD_CB_Initialized	equ 1000000000b
VCD_CB_Initialized_Bit	equ 9
VCD_PowerOn		equ 10000000000b
VCD_PowerOn_Bit	equ 10

.errnz VCD_CB_Windows_Port_Bit - VCD_Windows_Port_Bit
.errnz VCD_CB_Windows_30Drvr_Bit - VCD_Windows_30Drvr_Bit
.errnz VCD_CB_Mouse_Port_Bit - VCD_Mouse_Port_Bit

VCD_CB_Windows_Bits	equ VCD_CB_Windows_30Drvr OR VCD_CB_Windows_Port OR VCD_CB_Mouse_Port

;******************************************************************************
;			  D A T A   S T R U C T U R E S
;******************************************************************************

;***********
;
; Per-VM structure
;
VCD_CB_Struc STRUC
VCD_BAUD_LSB	    db	?	; LSB of virtual BAUD
VCD_BAUD_MSB	    db	?	; MSB of virtual BAUD
VCD_IER 	    db	?	; Virtual Int Enable Reg
VCD_LCR 	    db	?	; Virtual Line Ctrl Reg
VCD_MCR 	    db	?	; Virtual Modem Ctrl Reg
VCD_Read_Stat	    db	?	; Status of LSR & MSR reads
VCD_CB_Flags	    dw	?
		    dd	4 DUP(?); reserved
ifdef   NEC_98
VCD_Set_Mask        db  ?       ; for PC-98
VCD_Set_Mode        db  ?       ; for PC-98
VCD_Set_Command     db  ?       ; for PC-98
VCD_Write_Data      db  ?       ; for PC-98
VCD_Read_Data       db  ?       ; for PC-98
VCD_Read_Status     db  ?       ; for PC-98
VCD_Read_Signal     db  ?       ; for PC-98
Count               db  0       ; for PC-98
VCD_COM_Touched     db  ?       ; True if COM I/O touched
                    db  ?       ; PAD TO DWORD ALIGN
                    dw  ?       ; reserved
		    dd	2 DUP(?); reserved
endif	; Not NEC_98
VCD_CB_Struc ENDS


;***********
;
; Global state data
;
VCD_COM_Struc STRUC

VCD_CB_Offset	dd	?	; Offset in VM control block for per VM data
VCD_Number	db	?	; Either 1 or 2 for COM1 or COM2
VCD_IRQN	db	?	; h/w IRQ number for this device
VCD_Flags	dw	0	; Flags for Mouse, Virtualized, etc.
VCD_IRQ_Desc	dd	?	; Pointer to IRQ descriptor
VCD_Auto_Assign dd	?	; -1 = never auto assign, 0 = always auto assign
				; > 0, auto assign after n msec of no use
VCD_Name	db	8 DUP(?); Pointer to name of COM port
VCD_IObase	dd	?	; Base of IO regs
VCD_Owner	dd	0	; VM handle owning this COM device
VCD_IRQ_Handle	dd	?	; Handle used to talk to VPIC
VCD_Last_Use	dd	?

VCD_COMDEB	dd	?	; linear ptr to Windows COMDEB, if VCD_Windows_Port
VCD_QIn 	dd	?	; linear ptr to input queue, if VCD_Windows_Port
VCD_QOut	dd	?	; linear ptr to output queue, if VCD_Windows_Port

; NOTE:  The bytes BAUD_LSB - Def_MCR MUST BE LEFT IN THIS ORDER.  They are
;        copied into each VMs control block.
ifdef   NEC_98
VCD_Def_BAUD_LSB        db      16      ; Default BAUD rate LSB for PC-98
VCD_Def_BAUD_MSB        db      0       ; Default BAUD rate MSB for PC-98
else	;NEC_98
VCD_Def_BAUD_LSB    db  ?       ; Default virtual BAUD rate LSB
VCD_Def_BAUD_MSB    db  ?       ; Default virtual BAUD rate MSB
endif	;NEC_98
VCD_Def_IER     db      ?       ; Default vitrual Interrupt Enable register
VCD_Def_LCR     db      ?       ; Default virtual Line Control Register
VCD_Def_MCR     db      ?       ; Default virtual Modem Control Register
VCD_Def_LSR     db      ?       ; Default virtual Line Status Register
VCD_Def_MSR     db      ?       ; Default virtual Modem Status Register

VCD_Virt_IIR	db	?	; IIR value read in ring 0

VCD_virt_procs	dd	?

VCD_Hw_Int_Proc 	dd  ?
VCD_Virt_Int_Proc	dd  ?
VCD_EOI_Proc		dd  ?
VCD_Mask_Change_Proc	dd  ?
VCD_IRET_Proc		dd  ?


VCD_Notify	dd	?
VCD_RefData	dd	?
VCD_Alt_Notify	dd	?
VCD_Alt_RefData	dd	?
VCD_DevNode	dd	?

		dd	3 DUP(?); reserved
ifdef   NEC_98
;                                          CH.1  CH.2    CH.3
VCD_IO_Write_Data       db      ?       ;  30H   B1H     B9H -+
VCD_IO_Read_Data        db      ?       ;  30H   B1H     B9H  |
VCD_IO_Set_Mode         db      ?       ;  32H   B3H     BBH  |Install_IO_Handler
VCD_IO_Set_Command      db      ?       ;  32H   B3H     BBH  |
VCD_IO_Read_Status      db      ?       ;  32H   B3H     BBH  |
VCD_IO_Read_Signal      db      ?       ;  33H   B0H     B2H  |
VCD_IO_BAUD             db      ?       ;  75H   --      --  -+
VCD_IO_Timer            db      ?       ;  77H   --      --  -+Install_Multi_IO_Handler (CH.1 only)
VCD_IO_Set_Mask         db      ?       ;  35H   B0H     B2H  |Install_IO_Handler (CH.2,3)
VCD_IO_Read_Mask        db      ?       ;  35H   --      --  -+
VCD_CCUType             db      0       ; CCU chip type
			db	?	; reserved
VCD_Def_Set_Mask        db      0       ; for PC-98
VCD_Def_Set_Mode        db      4Eh     ; for PC-98  ; <= (9600,B8,PN,S1,X16)
VCD_Def_Set_Command     db      27h     ; for PC-98  ; <= (00100111b)
VCD_Def_Write_Data      db      0       ; for PC-98
VCD_Def_Read_Data       db      0       ; for PC-98
VCD_Def_Read_Status     db      ?       ; for PC-98
VCD_Def_Read_Signal     db      ?       ; for PC-98
			db	?	; reserved
VCD_Delay_Time      DW  ?       ; NEW function (Add Receive delay)
VCD_Vir_BAUD_LSB    db  ?       ; Default virtual BAUD rate LSB 
VCD_Vir_BAUD_MSB    db  ?       ; Default virtual BAUD rate MSB 
VCD_Vir_Set_Mask    db  ?       ; for PC-98
VCD_Vir_Set_Mode    db  ?       ; for PC-98
VCD_Vir_Set_Command db  ?       ; for PC-98
VCD_Vir_Write_Data  db  ?       ; for PC-98
VCD_Vir_Read_Data   db  ?       ; for PC-98
VCD_Vir_Read_Status db  ?       ; for PC-98
VCD_Vir_Read_Signal db  ?       ; for PC-98
VCD_Virr            db  0       ; Virtual Interrupt Request register
		dd	2 DUP(?); reserved
endif	; NEC_98
VCD_COM_Struc ENDS

VCD_COMDEB_Flag equ dword ptr VCD_QIn

VCD_Control_Set_Owner	equ 0


COM_IRQ_Sharable    equ 00000001h


;***********
;
; List of virtualization procs
;
VCD_ProcList_Struc STRUC
; Control proc for handling status change notifications
; proc will be entered with:
;     EAX = message #
;     EBX = VM Handle
;     ESI -> VCD_COM_Struc
VPS_Control_Proc	dd  ?

; Procs for virtualizing the IRQ, 0 = none - use default
; procs will be entered with:
;     EAX = IRQ handle
;     EBX = VM handle
;     ESI -> VCD_COM_Struc
;
VPS_Hw_Int_Proc 	dd  0	; hardware int for IRQ
VPS_Virt_Int_Proc	dd  0	; virtual int for IRQ
VPS_EOI_Proc		dd  0	; EOI for IRQ
VPS_Mask_Change_Proc	dd  0	; mask change for IRQ
VPS_IRET_Proc		dd  0	; virtual iret for IRQ

; Procs for virtualizing I/O while port owned
; procs will be entered with:
;     EBX = VM handle
;     ESI -> VCD_COM_Struc
;     EDX = port #
;     ECX = 0 - Input, 4 - Output
;     AL = data, if output
; procs should return AL = data, if input
VPS_In_RxTxB		dd  ?	; in from receive buffer
VPS_Out_RxTxB		dd  ?	; out to transmit buffer
VPS_In_IER		dd  ?	; in from Interrupt Enable Register
VPS_Out_IER		dd  ?	; out to Interrupt Enable Register
VPS_In_IIR		dd  ?	; in from Interrupt Identification Register
VPS_Out_IIR		dd  ?	; out to Interrupt Identification Register
VPS_In_LCR		dd  ?	; in from Line Control Register
VPS_Out_LCR		dd  ?	; out to Line Control Register
VPS_In_MCR		dd  ?	; in from Modem Control Register
VPS_Out_MCR		dd  ?	; out to Modem Control Register
VPS_In_LSR		dd  ?	; in from Line Status Register
VPS_Out_LSR		dd  ?	; out to Line Status Register
VPS_In_MSR		dd  ?	; in from Modem Status Register
VPS_Out_MSR		dd  ?	; out to Modem Status Register
VCD_ProcList_Struc ENDS

ifdef   NEC_98
VCD_PL_Struc8251 STRUC
; Control proc for handling status change notifications
; proc will be entered with:
;     EAX = message #
;     EBX = VM Handle
;     ESI -> VCD_COM_Struc
VPS_Control_Proc_8251	dd  ?

; Procs for virtualizing the IRQ, 0 = none - use default
; procs will be entered with:
;     EAX = IRQ handle
;     EBX = VM handle
;     ESI -> VCD_COM_Struc
;
VPS_Hw_Int_Proc_8251 		dd  0	; hardware int for IRQ
VPS_Virt_Int_Proc_8251		dd  0	; virtual int for IRQ
VPS_EOI_Proc_8251		dd  0	; EOI for IRQ
VPS_Mask_Change_Proc_8251	dd  0	; mask change for IRQ
VPS_IRET_Proc_8251		dd  0	; virtual iret for IRQ

; Procs for virtualizing I/O while port owned
; procs will be entered with:
;     EBX = VM handle
;     ESI -> VCD_COM_Struc
;     EDX = port #
;     ECX = 0 - Input, 4 - Output
;     AL = data, if output
; procs should return AL = data, if input
VPS_In_RxTxB_8251       dd  ?   ; in from receive buffer               0:IN  30h,B1h,B9h 
VPS_Out_RxTxB_8251      dd  ?   ; out to transmit buffer                 OUT 30h,B1h,B9h 
VPS_In_STATUS           dd  ?   ; read STATUS                          1:INp 32h,B3h,BBh 
VPS_Out_MODE            dd  ?   ; out to MODE/COMMAND                    OUT 32h,B3h,BBh 
VPS_Read_Signal1        dd  ?   ; in from Line Status Register 2,3ch   2:IN  -- ,B0h,B2h 
VPS_Set_MASK            dd  ?   ; out to Interrupt Enable Register       OUT -- ,B0h,B2h 
VPS_Read_Signal         dd  ?   ; in from Line Status Register 1ch     3:IN  33h -- ,--
                        dd  ?   ; not WRITE Signal                       OUT -- ,-- ,--
                        dd  ?   ; not READ Counter                     4:IN  -- ,-- ,--
VPS_Set_Counter         dd  ?   ; out to COM counter                     OUT 75h -- ,--
                        dd  ?   ; not READ Sysport                     5:IN  -- ,-- ,--
VPS_Set_Sysport         dd  ?   ; out to Interrupt Enable Register       OUT 37h -- ,--
VPS_Read_MASK1          dd  ?   ; in from Interrupt Enable Register    6:IN  35h -- ,--
VPS_Set_MASK1           dd  ?   ; out to Interrupt Enable Register       OUT 35h -- ,--
                        dd  ?   ; not READ Counter MODE                7:IN  -- ,-- ,--
VPS_Set_Cnt_MODE        dd  ?   ; out to COM counter MODE                OUT 77h -- ,--
VCD_PL_Struc8251 ENDS

.errnz VPS_Virt_Int_Proc_8251 - VPS_Hw_Int_Proc_8251 - 4
.errnz VPS_EOI_Proc_8251 - VPS_Virt_Int_Proc_8251 - 4
.errnz VPS_Mask_Change_Proc_8251 - VPS_EOI_Proc_8251 - 4
.errnz VPS_IRET_Proc_8251 - VPS_Mask_Change_Proc_8251 - 4

.errnz VPS_Control_Proc_8251 - VPS_Control_Proc
.errnz VPS_Hw_Int_Proc_8251 - VPS_Hw_Int_Proc
.errnz VPS_Virt_Int_Proc_8251 - VPS_Virt_Int_Proc
.errnz VPS_EOI_Proc_8251 - VPS_EOI_Proc
.errnz VPS_Mask_Change_Proc_8251 - VPS_Mask_Change_Proc
.errnz VPS_IRET_Proc_8251 - VPS_IRET_Proc
.errnz VPS_In_RxTxB_8251 - VPS_In_RxTxB
.errnz VPS_Out_RxTxB_8251 - VPS_Out_RxTxB

endif	;NEC_98

.errnz VCD_Virt_Int_Proc - VCD_Hw_Int_Proc - 4
.errnz VCD_EOI_Proc - VCD_Virt_Int_Proc - 4
.errnz VCD_Mask_Change_Proc - VCD_EOI_Proc - 4
.errnz VCD_IRET_Proc - VCD_Mask_Change_Proc - 4

.errnz VPS_Virt_Int_Proc - VPS_Hw_Int_Proc - 4
.errnz VPS_EOI_Proc - VPS_Virt_Int_Proc - 4
.errnz VPS_Mask_Change_Proc - VPS_EOI_Proc - 4
.errnz VPS_IRET_Proc - VPS_Mask_Change_Proc - 4

ifdef NEC_98
;***********
; Start(8251)
;
CCU_8251_Start	macro	Jump_Label,COMStruc_Ptr_Reg, Only
.ERRB	<Jump_Label>
.ERRB	<COMStruc_Ptr_Reg>
	cmp	byte ptr [COMStruc_Ptr_Reg.VCD_CCUType], CCU_RESERVE
	jae	Jump_Label&_End
	cmp	byte ptr [COMStruc_Ptr_Reg.VCD_CCUType], CCU_16550
IFB	<Only>
	je	Jump_Label&_Center
ELSE
	je	Jump_Label&_End
ENDIF
endm


;***********
; Center(16550)
;
CCU_16550_Center	macro	Jump_Label, COMStruc_Ptr_Reg, Only
.ERRB	<Jump_Label>
IFB	<Only>
	jmp	Jump_Label&_End
Jump_Label&_Center:
ELSE
.ERRB	<COMStruc_Ptr_Reg>
	cmp	byte ptr [COMStruc_Ptr_Reg.VCD_CCUType], CCU_RESERVE
	jae	Jump_Label&_End
	cmp	byte ptr [COMStruc_Ptr_Reg.VCD_CCUType], CCU_16550
	jne	Jump_Label&_End
ENDIF
endm


;***********
; End(Reserve)
;
Reserved_End	macro	Jump_Label
.ERRB	<Jump_Label>
Jump_Label&_End:
endm


;***********
; Sample code
;

;***********
;CCU_8251_Start	16550_8251, esi,
;	8251 source code
;CCU_16550_Center	16550_8251,
;	16550 source code
;Reserved_End		16550_8251

;***********
;CCU_8251_Start	16550_8251, esi, Only
;	8251 source code
;Reserved_End		16550_8251

;***********
;CCU_16550_Center	16550_8251, esi, Only
;	16550 source code
;Reserved_End		16550_8251

endif ;NEC_98
