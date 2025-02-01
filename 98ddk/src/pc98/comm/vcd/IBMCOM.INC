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

; Include file originally taken from Windows 3.00 COMM driver
; Equates and structures must match

IFDEF NEC_98				;NEC NEC_98******************************
wo       equ   word ptr
by       equ   byte ptr
MAXCOM   equ   2        ;Three COMs supported (COM1,2,3)
ELSE					;NEC NEC_98++++++++++++++++++++++++++++++
MAXCOM   equ   3        ;Two   COMs supported (COM1,2,3,4) [rkh]
ENDIF					;NEC NEC_98==============================

RS232B   equ   0h       ;RS232 Card(s) I/O addr 40:Save area.

IRQ3	 equ   0bh	; Int vector for Com card @ 2xxh
IRQ4	 equ   0ch	; Int vector for Com card @ 3xxh

INTA0    equ   20h      ;X'20' 8259 Interrupt Control Port
INTA1    equ   21h      ;X'21' 8259 Interrupt Mask Port
EOI      equ   20h      ;X'20' 8259 End-of-Interrupt ack

Open     equ   0201h    ;Int 2F open request
Close    equ   0202h    ;Int 2F close request
Lock2F   equ   0203h    ;Int 2F lock request
Unlock2F equ   0204h    ;Int 2F unlock request

; COMDEB - Communications Device Equipment Block.
;
; This is essentially a superset of the DCB used outside of this
; module. The DCB is contained within the DEB as the first fields.
; The fields which follow are data and status fields which
; are unique to this implementation.

ComDEB struc                           ;RS232 Data Equip Block

ifdef	NEC_98
  ComDCB        db      ((NEC_DCBSize+1) and 0FFFEh) DUP (0)
else	;NEC_98
  ComDCB        db      ((DCBSize+1) and 0FFFEh) DUP (0)
endif	;NEC_98

  ComErr        dw      0               ;Non-zero if I/O error
  Port          dw      0               ;Base I/O Address
  BIOSPortLoc   dw      0               ;Offset to port location (i.e. 40:0)

; The following fields are specific to com ports only

IFDEF NEC_98				;NEC NEC_98******************************
  TimerProcAdr	dw	0	; System Timer Procesor
  TimerHandle	dw	0	; System Timer Handle
  MaskFFPort	dw	0	; Mask F/F Set/Reset Port
  ReadSigPort	dw	0	; Read Signal Port
  DataPort	dw	0	; Data Port
  CommandPort	dw	0	; Command Port
  StatusPort	dw	0	; Status Port
  IMRPort	dw	0	; Interrupt Mask Register Port
  EOIPort	dw	0	; End Of Interrupt Port
  CommandShadow	db	0	; Copy of 8251 Command byte
  MaskFFShadow	db	0	; Copy of TxR.RxR.TxE MASK byte
ENDIF					;NEC NEC_98==============================
  XOffPoint     dw      0               ;Q count where XOff is sent
  EvtWord       dw      0               ;Event flags
  EvtMask       dw      0               ;Mask of events to check for
  QInAddr       dd      0               ;Address of the queue
  QInSize       dw      0               ;Length of queue in bytes
  QOutAddr      dd      0               ;Address of the queue
  QOutSize      dw      0               ;Length of queue in bytes
  QInCount      dw      0               ;Number of bytes currently in queue
  QInGet        dw      0               ;Offset into queue to get bytes from
  QInPut        dw      0               ;Offset into queue to put bytes in
  QOutCount     dw      0               ;Number of bytes currently in queue
  QOutGet       dw      0               ;Offset into queue to get bytes from
  QOutPut       dw      0               ;Offset into queue to put bytes in
  EFlags        db      0               ;Extended flags
  ErrorMask     db      0               ;Default error-checking mask
  RxMask        db      0               ;Character mask
  ImmedChar     db      0               ;Char to be transmitted immediately
  HSFlag        db      0               ;Handshake flag
  HHSLines      db      0               ;8250 DTR/RTS bits for handshaking
  OutHHSLines   db      0               ;Lines that must be high to output
  MSRShadow     db      0               ;Modem Status Register Shadow
  MSRMask       db      0               ;Mask of Modem Lines to check
  MSRInfinite   db      0               ;Mask of MSR lines that must be high
  Mask8259      db      0               ;8259 Interrupt mask bit
  IntVecNum     db      0               ;Interrupt vector number
; [rkh]  OldIntVec     dd      0               ;Old interrupt vector
  ForcedInt	db	0		; flag to indicate Tx int being forced
  LSRShadow	db	0		;Line Status Register shadow
  QOutMod       dw      0               ; characters sent mod xOnLim ENQ/ETX [rkh]

    ; [gps] real mode queue addresses
  QInSegment	dd	0		;Segment:offset of real mode input queue
  QOutSegment	dd	0		;Segment:offset of real mode output q
IFDEF NEC_98				;NEC NEC_98******************************
  OldMask8259IRQ	db		0	;save old IRQ mask
  IntVecIntCount	db		0	;
  OldIntVecInt		dd		0	;save old vector
  OurIntVecInt		dd		0	;
  RealModeIntVect	dd		0	;real mode int vector
  RMOurIntVect		dd		0	;
ENDIF					;NEC NEC_98==============================

ComDEB ends

      .errnz   (SIZE ComDEB) and 1


; The LptDEB is identical to the ComDEB structure, except
; all the COM port specific stuff has been removed (which
; convientiently was stored at the end so offsets would
; be correct).  This allows the code to act indifferently
; when accessing the strucutres for things like the port.

LptDEB struc
ifdef	NEC_98
  xComDCB       db      ((NEC_DCBSize+1) AND 0FFFEh) dup (0)
else	;NEC_98
  xComDCB       db      ((DCBSize+1) AND 0FFFEh) dup (0)
endif	;NEC_98
  xComErr       dw      0               ;Non-zero if I/O error
  xPort         dw      0               ;Base I/O Address
  xBIOSPortLoc  dw      0               ;Offset to port location (i.e. 40:0)
LptDEB ends

		.errnz	 xComDCB-ComDCB
		.errnz	 xComErr-ComErr
		.errnz	 xPort-Port
		.errnz	 xBIOSPortLoc-BIOSPortLoc


; Values for the handshake flag
;
; BreakSet        - True if break was set - stops transmission
; XOffSent        - True if we have sent the XOff character
; XOffPending     - True if XOff character needs to be sent
; XOnPending      - True if XOn  character needs to be sent
; HHSDown         - True if host dropped required hardware lines
; HHSDropped      - True if we dropped our hardware handshake lines
; XOffReceived    - True if XOff received from host
; HSPending       - Mask to return non-zero if XOn or Xoff must be sent
; HSReceived      - Mask to return non-zero if handshake has been
;                   received from host stopping transmission
; CannotXmit      - Mask to return non-zero if any condition
;                   exists which prevents us from tranmitting.
; HSSent          - Mask to return non-zero if we sent a handshake
; Discard         - True if to discard data (set by $TRMCOM)


XOffPending     equ     00000001b       ;XOff needs to be sent
EnqPending      equ     00000001b       ;Enq needs to be sent [rkh]
EtxPending      equ     00000001b       ;Etx needs to be sent

HHSDropped      equ     00000010b       ;Our hardware handshake lines are down

XOnPending      equ     00000100b       ;XOn needs to be sent
AckPending      equ     00000100b       ;Ack needs to be sent (ENQ/ACK & ETX/ACK)

XOffReceived    equ     00001000b       ;XOff character received
EnqSent         equ     00001000b       ;Enq has been sent
EtxSent         equ     00001000b       ;Etx has been sent

XOffSent        equ     00010000b       ;XOff has been sent
EnqReceived     equ     00010000b       ;Enq character received (ENQ/ACK)
EtxReceived     equ     00010000b       ;Etx character received (ETX/ACK)

HHSDown         equ     00100000b       ;Host hardware handshake lines are down

BreakSet        equ     01000000b       ;Break has been set

Discard         equ     10000000b       ;Discard recevied data


HSPending       equ     XOffPending+XOnPending
HSReceived      equ     XOffReceived+HHSDown
HSSent          equ     XOffSent+HHSDropped
CannotXmit      equ     HSSent+HSPending+HSReceived+BreakSet

pause   macro                ;;macro to insure that an instruction
        jmp     $+2          ;;  fetch occurs between IN and/or OUT
        endm                 ;;  instructions on the PC-AT machine


TimeoutError   equ   -2      ;Timeout error code for $TRMCOM
Timeout        equ   30      ;30 second timeout

;       Status bits returned from the BIOS for LPT ports

PS_NotBusy      equ     10000000b       ;Printer not busy
PS_Ack          equ     01000000b       ;Data acknowledged
PS_PaperOut     equ     00100000b       ;Out of paper
PS_Select       equ     00010000b       ;Device is selected
PS_IOError      equ     00001000b       ;IO error
PS_Timeout      equ     00000001b       ;Timeout occured

;	WinFlags constants...remove when included in windows.inc

WF_PMODE	equ	01h
WF_WIN286	equ	10h

IFDEF NEC_98				;NEC NEC_98******************************
;
;---------------  8251 ¼Ø±Ù ²ÝÀ-Ìª¨½ É ºÏÝÄÞ ¥ ½Ã-À½  ----------------------

;--------------------------------------------------------
;	ÓÃÞÑ ½Ã-À½	( IBM ´Ð­Ú-¼®Ý )	USE MSR_READ
;--------------------------------------------------------
 
DSR	=	10000000B	; Data Set Ready
RI	=	01000000B	; Ring Indicater (CI)
CTS	=	00100000B	; Clear To Send
RLSD	=	00010000B	; Recive Line Signal Detect (CD)

;--------------------------------------------------------
;	Îß-Ä C ( USART Ä×¯Ìß Ï½¸ )
;--------------------------------------------------------

RAM_CHECK	=	00010000b	; RAM É ÊßØÃ¨Á´¯¸ ²È-ÌÞÙ
SPEAKER_OFF	=	00001000b	; ½Ëß-¶- ½Ä¯Ìß
MSK_TXR		=	00000100B	; TX ÚÃÞ¨ ²È-ÌÞÙ
MSK_TXE		=	00000010B	; TX ´ÝÌßÃ¨ ²È-ÌÞÙ
MSK_RXR		=	00000001B	; RX ÚÃÞ¨ ²È-ÌÞÙ

;--------------------------------------------------------
;	8251 ½Ã-À½
;--------------------------------------------------------

BRK_DETECT	=	01000000B	; ÌÞÚ²¸ ÃÞ¨Ã¸À-	  	
F_ERR		= 	00100000B	; ÌÚ-ÐÝ¸Þ ´×-	 
O_ERR		= 	00010000B	; µ-ÊÞ ×Ý ´×-	 
P_ERR		= 	00001000B	; ÊßØÃ¨ ´×-	 
TX_EMP		=	00000100B	; TX ´ÝÌßÃ¨	 
RX_RDY		= 	00000010B	; RX ÚÃÞ²	 
TX_RDY		=	00000001B	; TX ÚÃÞ²	 

;--------------------------------------------------------
;	8251 ºÏÝÄÞ ÊÞ²Ä
;--------------------------------------------------------

INTER_RESET	=	01000000B	; Ó-ÄÞ ²Ý½Ä×¸¼®Ý ¾¯Ä
RTS		=	00100000B	; RTS
ERR_RESET	=	00010000B	; PE & OE & FE Bit É Ø¾¯Ä
SEND_BREAK	=	00001000B	; "0" = ÌÞÚ²¸ º-ÄÞ É ¿³¼Ý
RX_E		=	00000100B	; RX ²È-ÌÞÙ
DTR		=	00000010B	; DTR
TX_E		= 	00000001B	; TX ²È-ÌÞÙ

;--------------------------------------------------------
;	etc.....
;--------------------------------------------------------

SLAVE_PIC	=	08H		; ½Ú-ÌÞ PIC Îß-Ä ±ÄÞÚ½
ISR_READ	=	0BH		; ISR Ø-ÄÞ ºÏÝÄÞ
MASTER_OCW2	=	00H		; Ï½À PIC OCW2 Îß-Ä ±ÄÞÚ½
SLAVE_ISR	=	08H		; ½Ú-ÌÞ PIC ISR Îß-Ä ±ÄÞÚ½
EOI		=	20H		; EOI ºÏÝÄÞ
ALL_DONE	= 	00000000B	; 8251 ÜØºÐ ÃÞ¨½´²ÌÞÙ
RS_TIME_MOD	= 	0B6H		; Ch.0 ÎÞ-Ú-Ä ¼ÞªÈÚ-À
TIMER2		=	075H		; 
TIME_CTL	=	077H		;

SI_CHR		=	0FH		; SHIFT IN
SO_CHR		=	0EH		; SHIFT OUT

;---------------------------------------------------------
;	Êß×ÚÙ ²ÝÀ-Ìª²½ ºÏÝÄÞ & ½Ã-À½ ¾¯Ä
;---------------------------------------------------------

PRN_WRITE_DATA		equ	040h	;
PRN_READ_SIGNAL1	equ	042h	;
PRN_WRITE_SIGNAL1	equ	046h	;

PRN_PSTB_Active_X2 	equ	004h	;
PRN_PSTB_NonActive_X2	equ	005h	;
CMOS_RAM_SEG_X2		equ	0e300h	; ÒÓØ½²¯Á É ¾¸ÞÒÝÄ( pc-98XA )

PRN_PSTB_Active_X1 	equ	00eh	;
PRN_PSTB_NonActive_X1	equ	00fh	;
CMOS_RAM_SEG_X1		equ	0a300h	; ÒÓØ½²¯Á É ¾¸ÞÒÝÄ( pc-9800 )

PRN_TimeOutValue	equ	005h

;-------------------------------------------
;	¿ÌÄ³ª± ½²¯Á É µÌ¾¯Ä ±ÄÞÚ½
;-------------------------------------------

ssw1			equ	0fe2h	;
ssw2			equ	0fe6h	;
ssw3			equ	0feah	;
ssw4			equ	0feeh	;
ssw5			equ	0ff2h	;
ssw6			equ	0ff6h	;

;	8251	Int Enable F/F
Enable_TxR	equ	00000100b	;Tx Ready F/F Enable
Enable_TxE	equ	00000010b	;Tx Empty F/F Enable
Enable_RxR	equ	00000001b	;Rx Ready F/F Enable

;	8251	Status Format
BRK_Detect	equ	01000000b	;Break Detect
F_ERR		equ	00100000b	;Framing Err
O_ERR		equ	00010000b	;OverRun Err
P_ERR		equ	00001000b	;Parity  Err
Tx_EMP		equ	00000100b	;Tx Empty
Rx_RDY		equ	00000010b	;Rx Ready
Tx_RDY		equ	00000001b	;Tx Ready

;	8251	Command Byte		;
Inter_Reset	equ	01000000b	;
RTS		equ	00100000b	;Request to Send
Err_Reset	equ	00010000b	;
Send_Break	equ	00001000b	;
Rx_Enable	equ	00000100b	;
DTR		equ	00000010b	;Data Terminal Ready
Tx_Enable	equ	00000001b	;

;	etc...									(INS 90.9.3)
Slave_Pic	equ	08h		;					(INS 90.9.3)
ISR_Read	equ	0Bh		;Interrupt Service Register Ready	(INS 90.9.3)
Master_OCW2	equ	00h		;					(INS 90.9.3)
Slave_ISR	equ	08h		;					(INS 90.9.3)
EOI		equ	20h		;End of Interrupt			(INS 90.9.3)
All_Done	equ	00h		;					(INS 90.9.3)
RS_Time_Mod	equ	0B6h		;					(INS 90.9.3)
Timer2		equ	075h		;					(INS 90.9.3)
Timer_CTL	equ	077h		;Timer Control				(INS 90.9.3)

ENDIF					;NEC NEC_98==============================
;	Interrupt 31h service call equates

include int31.inc

Get_RM_IntVector    equ     (Int31_Int_Serv SHL 8 ) OR Int_Get_Real_Vec
Set_RM_IntVector    equ     (Int31_Int_Serv SHL 8 ) OR Int_Set_Real_Vec
ifdef	NEC_98
; INS8250 ACE Register Offsets And Bit Definitions

ACE_RBR         equ     0               ;Receiver Buffer
ACE_THR         equ     0               ;Transmit Holding Register

ACE_IER         equ     1               ;Interrupt Enable
  ACE_ERBFI     equ     00000001b       ;  Received Data Available
  ACE_ETBEI     equ     00000010b       ;  Transmitter Holding Register Empty
  ACE_ELSI      equ     00000100b       ;  Receiver Line Status
  ACE_EDSSI     equ     00001000b       ;  Modem Status

ACE_IIDR        equ     2               ;Interrupt Identification
  ACE_IIP       equ     00000001b       ;  Inverted Interrupt Pending (0=int)
  ACE_IID       equ     00000110b       ;  Interrupt ID
  ACE_MSI       equ     00000000b       ;  Modem Status
  ACE_THREI     equ     00000010b       ;  Transmitter Holding Register Empty
  ACE_RDAI      equ     00000100b       ;  Received Data Available
  ACE_RLSI      equ     00000110b       ;  Receiver Line Status

ACE_LCR         equ     3               ;Line Control
  ACE_WLS       equ     00000011b       ;  Word Length Select Bits
  ACE_WLS0      equ     00000001b       ;  Word Length Select Bit 0
  ACE_WLS1      equ     00000010b       ;  Word Length Select Bit 1
  ACE_5BW       equ     00000000b       ;  5 Bit Words
  ACE_6BW       equ     00000001b       ;  6 Bit Words
  ACE_7BW       equ     00000010b       ;  7 Bit Words
  ACE_8BW       equ     00000011b       ;  8 Bit Words
  ACE_STB       equ     00000100b       ;  Stop Bits
  ACE_1SB       equ     00000000b       ;  1 Stop Bits (1.5 for 5 bit words)
  ACE_2SB       equ     00000100b       ;  2 Stop Bits
  ACE_PEN       equ     00001000b       ;  Parity Enable
  ACE_PSB       equ     00110000b       ;  Parity select bits
  ACE_EPS       equ     00010000b       ;  Even Parity Select
  ACE_SP        equ     00100000b       ;  Stick Parity
  ACE_SB        equ     01000000b       ;  Set Break
  ACE_DLAB      equ     10000000b       ;  Divisor Latch Access Bit

ACE_MCR         equ     4               ;Modem Control
  ACE_DTR       equ     00000001b       ;  Data Terminal ready
  ACE_RTS       equ     00000010b       ;  Request To Send
  ACE_OUT1      equ     00000100b       ;  Output Line 1
  ACE_OUT2      equ     00001000b       ;  Output Line 2
  ACE_LOOP      equ     00010000b       ;  Loopback

ACE_LSR         equ     5               ;Line Status
  ACE_DR        equ     00000001b       ;  Data Ready
  ACE_OR        equ     00000010b       ;  Overrun Error
  ACE_PE        equ     00000100b       ;  Parity Error
  ACE_FE        equ     00001000b       ;  Framing Error
  ACE_BI        equ     00010000b       ;  Break Interrupt
  ACE_THRE      equ     00100000b       ;  Transmitter Holding Register Empty
  ACE_TSRE      equ     01000000b       ;  Transmitter Shift Register Empty

ACE_MSR         equ     6               ;Modem Status
  ACE_DCTS      equ     00000001b       ;  Delta Clear to Send
  ACE_DDSR      equ     00000010b       ;  Delta Data Set Ready
  ACE_TERI      equ     00000100b       ;  Trailing Edge Ring Indicator
  ACE_DRLSD     equ     00001000b       ;  Delta Receive Line Signal Detect
  ACE_CTS       equ     00010000b       ;  Clear To Send
  ACE_DSR       equ     00100000b       ;  Data Set ready
  ACE_RI        equ     01000000b       ;  Ring Indicator
  ACE_RLSD      equ     10000000b       ;  Receive Line Signal Detect

ACE_DLL         equ     0               ;LSB Baud Rate Divisor

ACE_DLM         equ     1               ;MSB Baud Rate Divisor
endif	;NEC_98
