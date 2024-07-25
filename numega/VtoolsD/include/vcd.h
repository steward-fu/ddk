/*
 *
 *   (C) Copyright MICROSOFT Corp., 1989-1992
 *   (C) Copyright Compuware Corporation, 1994
 */

#ifndef __vcd_h_
#define __vcd_h_

#define	VCD_Service	Declare_Service

Begin_Service_Table( VCD )
	VCD_Service(VCD_Get_Version)
	VCD_Service(VCD_Set_Port_Global)
	VCD_Service(VCD_Get_Focus)
	VCD_Service(VCD_Virtualize_Port)
	VCD_Service(VCD_Acquire_Port)
	VCD_Service(VCD_Free_Port)
// New for WFW 3.11
	VCD_Service(VCD_Acquire_Port_Windows_Style)
	VCD_Service(VCD_Free_Port_Windows_Style)
	VCD_Service(VCD_Steal_Port_Windows_Style)
#ifdef WIN40
	VCD_Service(VCD_Find_COM_Index)
	VCD_Service(VCD_Set_Port_Global_Special)
	VCD_Service(VCD_Virtualize_Port_Dynamic)
	VCD_Service(VCD_Unvirtualize_Port_Dynamic)
#endif
End_Service_Table(VCD)
//
// PM API Services
//
#define	VCD_PM_API_Get_Version		0
#define	VCD_PM_API_Get_Port_Array	1
#define	VCD_PM_API_Get_Port_Behavior	2
#define	VCD_PM_API_Set_Port_Behavior	3
#define	VCD_PM_API_Acquire_Port		4
#define	VCD_PM_API_Free_Port		5
#define	VCD_PM_API_Steal_Port		6


//
//  UART ports and offsets
//
#define	UART_PORTS	7		// # of ports for UART
#define	UART_REG_MASK	7

#define	UART_BAUD_LSB	0		// Least Signif. Byte of BAUD divisor
#define	UART_BAUD_MSB	1		// Most	Signif. Byte of BAUD divisor
#define	UART_RBR	0		// Receive buffer register
#define	UART_THR	0		// Transmit holding register
#define	UART_IER	1		// Interrupt enble register
#define	UART_IIR	2		// Interrupt identity register
#define	UART_LCR	3		// Line control register
#define	UART_MCR	4		// Modem control register
#define	UART_LSR	5		// Line status register
#define	UART_MSR	6		// Modem status register

//***********
//  Interrupt Enable Register Equates
//
#define	IER_MS		0x08		// Modem Status
#define	IER_LS		0x04		// Line Status
#define	IER_THRE	0x02		// Transmit Holding Register Empty
#define	IER_DR		0x01		// Data Ready

//***********
//  Interrupt Identity Register Equates
//
#define	IIR_NONE	0x01	// No interrupt pending
#define	IIR_LS		0x06	// Line Status Interrupt
#define	IIR_DR		0x04	// Data Ready Interrupt
#define	IIR_THRE	0x02	// Transmit Holding Register Empty
#define	IIR_MS		0x00	// Modem Status Interrupt

//***********
//  Line Control Register Equates
//
#define	LCR_DLAB	0x80	// Divisor Latch Access

//***********
//  Modem Control Register Equates
//
#define	MCR_INTEN	0x08	// Interrupt Enable
#define	MCR_RTS		0x02	// Request to Send
#define	MCR_DTR		0x01	// Data Terminal Ready
#define	MCR_Loopback	0x10	// Enable Internal Loopback

//***********
//  Line Status Register Equates
//
#define	LSR_THRE	0x20	// Transmit Holding Register Empty
#define	LSR_DR		0x01	// Data Ready
#define	LSR_OE		0x02	// Overrun Error
#define	LSR_PE		0x04	// Parity Error
#define	LSR_FE		0x08	// Frame Error
#define	LSR_BI		0x10	// Break Detected
#define	LSR_ERRBITS	0x1E	// All the error bits
#define	LSR_TXBITS	0x60	// THRE and TEMT

//***********
//  Modem Status Register Equates
//
#define	MSR_DELTA	0x0F	// Modem Status DELTA bits
#define	MSR_DCTS	0x01	// Delta Clear to Send
#define	MSR_DDSR	0x02	// Delta Data Set Ready
#define	MSR_TERI	0x04	// Trailing Edge Ring Indicator
#define	MSR_DRLSD	0x08	// Delta Receive Line Signal Detect
#define	MSR_CTS		0x10	// Clear to Send
#define	MSR_DSR		0x20	// Data Set Ready
#define	MSR_RING	0x40	// Ring Indicator
#define	MSR_RLSD	0x80	// Receive Line Signal Detect

//***********
//  Masks for read status flags
//
#define	VCD_Read_LSR_Mask   0x01     // Read Line Status Register
#define	VCD_Read_MSR_Mask   0x02     // Read Modem Status Register



//***********
//  Flag bits  for VCD_COM_Struc.VCD_Flags
//
#define	VCD_global_port		0x0001
#define	VCD_global_port_Bit	0
#define	VCD_not_sharable_IRQ	0x0002	// Set if IRQ is not sharable
#define	VCD_not_sharable_IRQ_Bit 1
#define	VCD_IRQ_Init		0x0004	// Set if COM attempted to virt IRQ
#define	VCD_IRQ_Init_Bit	2
#define	VCD_Owns_IRQ		0x0008	// Set if COM DID virtualize IRQ
#define	VCD_Owns_IRQ_Bit	3
#define	VCD_Windows_30Drvr	0x0010	// Set if Windows 3.0 com driver
#define	VCD_Windows_30Drvr_Bit	4
#define	VCD_SetOwner_Event	0x0020	// Set if event scheduled to set owner
#define	VCD_SetOwner_Event_Bit	5
#define	VCD_Windows_Port	0x0040	// Set if Windows has control of this port
#define	VCD_Windows_Port_Bit	6
#define	VCD_Virtualized		0x0080	// Set if VCD_Virtualize_Port was called
#define	VCD_Virtualized_Bit	7
#define	VCD_Mouse_Port		0x0100
#define	VCD_Mouse_Port_Bit	8
#define	VCD_1st_on_IRQ		0x0200	// Set if first to virtualize IRQ
#define	VCD_1st_on_IRQ_Bit	9
	
#ifdef WIN40

#define	VCD_Global_IRQ		0x0400
#define	VCD_Global_IRQ_Bit	10		// Set if IRQ was unmasked when VCD
						// tried to virtualize it for the 1st
						// time.
#define	VCD_AltOwnerActive	0x0800
#define	VCD_AltOwnerActiveBit	11	// set if an alternate owner is waiting
					// to own the port. Basically we have
					// two possible (VxD/COMM.DRV) owners
					// and a DOS VM as the owner.

#define VCD_Dynamic_Port	0x1000
#define VCD_Dynamic_Port_Bit	12	// set if the port "arrives" after
					// device Init.
#endif
//***********
//  Flag bits  for VCD_CB_Struc.VCD_CB_Flags
//
#define	VCD_Contended		0x01	// set if VM contended for port and lost
#define	VCD_Contended_Bit	0
#ifdef WIN40
#define	VCD_Assign_Failed	0x02	// set if VCD_Assign failed because port
#define	VCD_Assign_Failed_Bit	1	//   was owned by a VxD and a msg has
					//   been displayed
#endif
#define	VCD_CB_Windows_30Drvr	0x0010	// Set if Windows 3.0 com driver
#define	VCD_CB_Windows_30Drvr_Bit 4
#define	VCD_CB_Windows_Port	0x0040	// Set if Windows has control of this port
#define	VCD_CB_Windows_Port_Bit	6
#define	VCD_Touched		0x0080
#define	VCD_Touched_Bit		7
#define	VCD_CB_Mouse_Port	0x0100
#define	VCD_CB_Mouse_Port_Bit	8
#define VCD_CB_Initialized	0x0200
#define VCD_CB_Initialized_Bit	9

#if (VCD_CB_Windows_Port_Bit != VCD_Windows_Port_Bit)
#error Bad definition
#endif

#if (VCD_CB_Windows_30Drvr_Bit != VCD_Windows_30Drvr_Bit)
#error Bad defs
#endif

#if (VCD_CB_Mouse_Port_Bit != VCD_Mouse_Port_Bit)
#error Bad defs
#endif

#define	VCD_CB_Windows_Bits	(VCD_CB_Windows_30Drvr | VCD_CB_Windows_Port | VCD_CB_Mouse_Port)

//******************************************************************************
//			  D A T A   S T R U C T U R E S
//******************************************************************************

//***********
//
// Per-VM structure
//


typedef struct VCD_CB_Struc {
	BYTE	VCD_BAUD_LSB;	// LSB of virtual BAUD
	BYTE	VCD_BAUD_MSB;	// MSB of virtual BAUD
	BYTE	VCD_IER; 	// Virtual Int Enable Reg
	BYTE	VCD_LCR; 	// Virtual Line Ctrl Reg
	BYTE	VCD_MCR; 	// Virtual Modem Ctrl Reg
	BYTE	VCD_Read_Stat;	// Status of LSR & MSR reads
	WORD	VCD_CB_Flags;	// Flags
	DWORD	RESERVED[4];
} VCD_CB, *PVCD_CB;


//***********
//
// Global state data
//

typedef struct VCD_COM_Struc {

	DWORD	VCD_CB_Offset;	// Offset in VM control block for per VM data
	BYTE	VCD_Number;	// Either 1 or 2 for COM1 or COM2
	BYTE	VCD_IRQN;	// h/w IRQ number for this device
	WORD	VCD_Flags;	// Flags for Mouse, Virtualized, etc.
	DWORD	VCD_IRQ_Desc;	// Pointer to IRQ descriptor
	DWORD	VCD_Auto_Assign;	// -1 = never auto assign, 0 = always auto assign
					// > 0, auto assign after n msec of no use
	BYTE	VCD_Name[8];	// Pointer to name of COM port
	DWORD	VCD_IObase;	// Base of IO regs
	DWORD	VCD_Owner;	// VM handle owning this COM device
	DWORD	VCD_IRQ_Handle;	// Handle used to talk to VPIC
	DWORD	VCD_Last_Use;

	DWORD	VCD_COMDEB;	// linear ptr to Windows COMDEB, if VCD_Windows_Port
	DWORD	VCD_QIn;	// linear ptr to input queue, if VCD_Windows_Port
	DWORD	VCD_QOut;	// linear ptr to output queue, if VCD_Windows_Port

// NOTE:  The bytes BAUD_LSB - Def_MCR MUST BE LEFT IN THIS ORDER.  They are
//        copied into each VMs control block.
	BYTE	VCD_Def_BAUD_LSB;	// Default virtual BAUD rate LSB
	BYTE	VCD_Def_BAUD_MSB;	// Default virtual BAUD rate MSB
	BYTE	VCD_Def_IER;		// Default vitrual Interrupt Enable register
	BYTE	VCD_Def_LCR;		// Default virtual Line Control Register
	BYTE	VCD_Def_MCR;		// Default virtual Modem Control Register
	BYTE	VCD_Def_LSR;		// Default virtual Line Status Register
	BYTE	VCD_Def_MSR;		// Default virtual Modem Status Register

	BYTE	VCD_Virt_IIR;		// IIR value read in ring 0

	DWORD	VCD_virt_procs;

	DWORD	VCD_Hw_Int_Proc;
	DWORD	VCD_Virt_Int_Proc;
	DWORD	VCD_EOI_Proc;
	DWORD	VCD_Mask_Change_Proc;
	DWORD	VCD_IRET_Proc;

#ifdef WIN40
	DWORD	VCD_Alt_Notify;
	DWORD	VCD_Alt_RefData;
#endif

	DWORD	RESERVED[4];
} VCD_COM, *PVCD_COM;

//VCD_COMDEB_Flag equ dword ptr VCD_QIn
#ifdef WIN40
//VCD_COMDEB_Notify equ dword ptr VCD_QOut
#endif

#define	VCD_Control_Set_Owner 0


#define COM_IRQ_Sharable 0x01



#endif		// __vcd_h_
