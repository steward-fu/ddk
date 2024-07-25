/*
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *   (C) Copyright Compuware Corporation, 1994
 *
 */

#ifndef __VPICD_H_
#define __VPICD_H_

// Equates for result from VPICD_Get_Status

#define	VPICD_STAT_IRET_PENDING     	0x01
#define	VPICD_STAT_IRET_PENDING_BIT 	0
#define	VPICD_STAT_IN_SERVICE	    	0x02
#define	VPICD_STAT_IN_SERVICE_BIT   	1
#define	VPICD_STAT_PHYS_MASK	    	0x04
#define	VPICD_STAT_PHYS_MASK_BIT    	2
#define	VPICD_STAT_PHYS_IN_SERV     	0x08
#define	VPICD_STAT_PHYS_IN_SERV_BIT 	3
#define	VPICD_STAT_VIRT_MASK	    	0x10
#define	VPICD_STAT_VIRT_MASK_BIT    	4
#define	VPICD_STAT_VIRT_REQ	    	0x20
#define	VPICD_STAT_VIRT_REQ_BIT     	5
#define	VPICD_STAT_PHYS_REQ	    	0x40
#define	VPICD_STAT_PHYS_REQ_BIT     	6
#define	VPICD_STAT_VIRT_DEV_REQ     	0x80
#define	VPICD_STAT_VIRT_DEV_REQ_BIT 	7
#ifdef WIN40
#define VPICD_STAT_GLOBAL		0x100
#define VPICD_STAT_GLOBAL_BIT		8
#define VPICD_STAT_OWNED_BY_VM		0x200
#define VPICD_STAT_OWNED_BY_VM_BIT	9
#endif

// Equates for options in IRQ Descriptor

#define	VPICD_OPT_READ_HW_IRR	    	0x01
#define	VPICD_OPT_READ_HW_IRR_BIT   	0
#define	VPICD_OPT_CAN_SHARE	    	0x02
#define	VPICD_OPT_CAN_SHARE_BIT		1
#ifdef WIN40
#define	VPICD_OPT_REF_DATA	    	0x04		// new for 4.0
#define	VPICD_OPT_REF_DATA_BIT		2
#define	VPICD_OPT_VIRT_INT_REJECT    	0x10		// new for 4.0
#define	VPICD_OPT_VIRT_INT_REJECT_BIT	4
#define VPICD_OPT_SHARE_PMODE_ONLY	0x20		// new for 4.0
#define VPICD_OPT_SHARE_PMODE_ONLY_BIT	5
#define VPICD_OPT_ALL			0x3F		// Internal use

// Equates for VPICD_Force_Default_Owner service.  Flags passed in high word
// of IRQ number

#define VPICD_FDO_NO_CONTENTION 	0x10000 	// new for 4.0
#define VPICD_FDO_NO_CONTENTION_BIT	16
#define VPICD_FDO_FAVOR_FOCUS		0x20000 	// new for 4.0
#define VPICD_FDO_FAVOR_FOCUS_BIT	17
#endif

typedef struct VPICD_IRQ_Descriptor {
    WORD VID_IRQ_Number;
    WORD VID_Options;			// INIT<0>
    DWORD VID_Hw_Int_Proc;
    DWORD VID_Virt_Int_Proc;		// INIT<0>
    DWORD VID_EOI_Proc;			// INIT<0>
    DWORD VID_Mask_Change_Proc;		// INIT<0>
    DWORD VID_IRET_Proc;		// INIT<0>
    DWORD VID_IRET_Time_Out;		// INIT<500>
#ifdef WIN40
    PVOID VID_Hw_Int_Ref;		// new for 4.0
#endif
} VID, *PVID;

typedef ULONG HIRQ;			// IRQ Handle

#define	VPICD_Service	Declare_Service

Begin_Service_Table(VPICD)

VPICD_Service	(VPICD_Get_Version)
VPICD_Service	(VPICD_Virtualize_IRQ)
VPICD_Service	(VPICD_Set_Int_Request)
VPICD_Service	(VPICD_Clear_Int_Request)
VPICD_Service	(VPICD_Phys_EOI)
VPICD_Service	(VPICD_Get_Complete_Status)
VPICD_Service	(VPICD_Get_Status)
VPICD_Service	(VPICD_Test_Phys_Request)
VPICD_Service	(VPICD_Physically_Mask)
VPICD_Service	(VPICD_Physically_Unmask)
VPICD_Service	(VPICD_Set_Auto_Masking)
VPICD_Service	(VPICD_Get_IRQ_Complete_Status)
VPICD_Service	(VPICD_Convert_Handle_To_IRQ)
VPICD_Service	(VPICD_Convert_IRQ_To_Int)
VPICD_Service	(VPICD_Convert_Int_To_IRQ)
VPICD_Service	(VPICD_Call_When_Hw_Int)
VPICD_Service	(VPICD_Force_Default_Owner)
VPICD_Service	(VPICD_Force_Default_Behavior)

VPICD_Service	(VPICD_Auto_Mask_At_Inst_Swap)
VPICD_Service	(VPICD_Begin_Inst_Page_Swap)
VPICD_Service	(VPICD_End_Inst_Page_Swap)
#ifdef WIN40
VPICD_Service	(VPICD_Virtual_EOI)
VPICD_Service	(VPICD_Get_Virtualization_Count)
VPICD_Service	(VPICD_Post_Sys_Critical_Init)
VPICD_Service	(VPICD_VM_SlavePIC_Mask_Change)
#endif
End_Service_Table(VPICD)


#endif		// __VPICD_H_
