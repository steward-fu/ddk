/******************************************************************************
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *
 *   Title:     VPICD.H - Include file for Virtual PIC Device
 *
 *   Version:   3.10
 *
 *   Date:      13-Apr-1988
 *
 *   Author:    RAL
 *
 *-----------------------------------------------------------------------------
 *
 *   Change log:
 *
 *      DATE    REV                 DESCRIPTION
 *   ----------- --- ----------------------------------------------------------
 *   13-Apr-1988 RAL Rewrite
 *   20-Oct-1988 RAL Added VPICD_Test_Phys_Request service
 *   20-Jan-1992 RAL Added new services for 3.10
 *
 ******************************************************************************/

#ifndef _VPICD_H
#define _VPICD_H

/*INT32*/

// Equates for result from VPICD_Get_Status

#define VPICD_STAT_IRET_PENDING         0x01
#define VPICD_STAT_IRET_PENDING_BIT     0
#define VPICD_STAT_IN_SERVICE           0x02
#define VPICD_STAT_IN_SERVICE_BIT       1
#define VPICD_STAT_PHYS_MASK            0x04
#define VPICD_STAT_PHYS_MASK_BIT        2
#define VPICD_STAT_PHYS_IN_SERV         0x08
#define VPICD_STAT_PHYS_IN_SERV_BIT     3
#define VPICD_STAT_VIRT_MASK            0x10
#define VPICD_STAT_VIRT_MASK_BIT        4
#define VPICD_STAT_VIRT_REQ             0x20
#define VPICD_STAT_VIRT_REQ_BIT         5
#define VPICD_STAT_PHYS_REQ             0x40
#define VPICD_STAT_PHYS_REQ_BIT         6
#define VPICD_STAT_VIRT_DEV_REQ         0x80
#define VPICD_STAT_VIRT_DEV_REQ_BIT     7
#define VPICD_STAT_GLOBAL               0x100
#define VPICD_STAT_GLOBAL_BIT           8
#define VPICD_STAT_OWNED_BY_VM          0x200
#define VPICD_STAT_OWNED_BY_VM_BIT      9

// Equates for options in IRQ Descriptor

#define VPICD_OPT_READ_HW_IRR           0x01
#define VPICD_OPT_READ_HW_IRR_BIT       0
#define VPICD_OPT_CAN_SHARE             0x02
#define VPICD_OPT_CAN_SHARE_BIT         1
#define VPICD_OPT_REF_DATA              0x04            // new for 4.0
#define VPICD_OPT_REF_DATA_BIT          2
#define VPICD_OPT_VIRT_INT_REJECT       0x10            // new for 4.0
#define VPICD_OPT_VIRT_INT_REJECT_BIT   4
#define VPICD_OPT_SHARE_PMODE_ONLY      0x20            // new for 4.0
#define VPICD_OPT_SHARE_PMODE_ONLY_BIT  5

#define VPICD_OPT_SUPPORT_EDGE_SHARE_BIT 8		// New for 4.1 -- ISA sharing
#define VPICD_OPT_SUPPORT_EDGE_SHARE 	(1 << VPICD_OPT_SUPPORT_EDGE_SHARE_BIT)

//
// SORTABLE should always be set when CAN_SHARE is set. It means that VPICD
// can bump the SIRQ handler that claimed the interrupt to the head of the
// list. The only case this wouldn't be true is if you registered two handlers
// for the same device, and the order you process them mathers.
//
#define VPICD_OPT_SORTABLE_BIT		9		// New for 4.1 -- 
#define VPICD_OPT_SORTABLE	 	(1 << VPICD_OPT_SORTABLE_BIT)

#define VPICD_OPT_ALL                   0x33F            // Internal use

// Equates for VPICD_Force_Default_Owner service.  Flags passed in high word
// of IRQ number

#define VPICD_FDO_NO_CONTENTION         0x10000         // new for 4.0
#define VPICD_FDO_NO_CONTENTION_BIT     16
#define VPICD_FDO_FAVOR_FOCUS           0x20000         // new for 4.0
#define VPICD_FDO_FAVOR_FOCUS_BIT       17

//
// Maximum IRQL level
//

#define NUM_IRQL_LEVELS 32

typedef struct VPICD_IRQ_Descriptor {
    USHORT VID_IRQ_Number;
    USHORT VID_Options;                 // INIT<0>
    ULONG VID_Hw_Int_Proc;
    ULONG VID_Virt_Int_Proc;            // INIT<0>
    ULONG VID_EOI_Proc;                 // INIT<0>
    ULONG VID_Mask_Change_Proc;         // INIT<0>
    ULONG VID_IRET_Proc;                // INIT<0>
    ULONG VID_IRET_Time_Out;            // INIT<500>
    ULONG VID_Hw_Int_Ref;               // new for 4.0
} VID;

typedef VID *PVID;
typedef ULONG HIRQ;                     // IRQ Handle

/*XLATOFF*/
#define VPICD_Service   Declare_Service
#pragma warning (disable:4003)          // turn off not enough params warning
/*XLATON*/

/*MACROS*/
Begin_Service_Table(VPICD)

VPICD_Service   (VPICD_Get_Version, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Virtualize_IRQ, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Set_Int_Request, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Clear_Int_Request, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Phys_EOI, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Get_Complete_Status, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Get_Status, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Test_Phys_Request, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Physically_Mask, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Physically_Unmask, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Set_Auto_Masking, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Get_IRQ_Complete_Status, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Convert_Handle_To_IRQ, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Convert_IRQ_To_Int, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Convert_Int_To_IRQ, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Call_When_Hw_Int, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Force_Default_Owner, VxD_LOCKED_CODE)
VPICD_Service   (VPICD_Force_Default_Behavior, VxD_LOCKED_CODE)

VPICD_Service   (VPICD_Auto_Mask_At_Inst_Swap)
VPICD_Service   (VPICD_Begin_Inst_Page_Swap)
VPICD_Service   (VPICD_End_Inst_Page_Swap)
VPICD_Service   (VPICD_Virtual_EOI, VxD_LOCKED_CODE)    // 4.0 service
VPICD_Service   (VPICD_Get_Virtualization_Count)
VPICD_Service   (VPICD_Post_Sys_Critical_Init, LOCAL)
VPICD_Service   (VPICD_VM_SlavePIC_Mask_Change, VxD_LOCKED_CODE)
VPICD_Service   (_VPICD_Clear_IR_Bits, VxD_LOCKED_CODE)
VPICD_Service	(RESERVED_VPICD_Get_Level_Mask, RESERVED) // Obsolete
VPICD_Service	(RESERVED_VPICD_Set_Level_Mask, RESERVED) // Obsolete
VPICD_Service   (_VPICD_Set_Irql_Mask, VxD_LOCKED_CODE) //OPK3 starts here!
VPICD_Service   (_VPICD_Set_Channel_Irql, VxD_LOCKED_CODE)
VPICD_Service   (_VPICD_Prepare_For_Shutdown, VxD_LOCKED_CODE) // 4.1 service
VPICD_Service	(_VPICD_Register_Trigger_Handler, VxD_LOCKED_CODE)	// Only ACPI & PCI can use
End_Service_Table(VPICD)

/*ENDMACROS*/

#define	VPICD_TRIGGER_HANDLER_GET	0x00000000
#define	VPICD_TRIGGER_HANDLER_SET	0x00000001

/*XLATOFF*/
#pragma warning (default:4003)          // turn on not enough params warning
#pragma warning (disable:4035)          // turn off no return code warning

typedef	ULONG	(_cdecl *VPICDTRIGGERHANDLER)(ULONG, ULONG);

/*
 *  This function is a macro for efficiency.  The parameters passed are
 *  the variables the version (USHORT), flags (ULONG), and maximum IRQ
 *  number (ULONG) are put.
 *
 */

#define VPICD_Get_Version(ver, fl, cMaxIRQ) \
{ \
    VxDCall(VPICD_Get_Version) \
    __asm xchg [ver],ax \
    __asm xchg [fl],ebx \
    __asm xchg [cMaxIRQ],ecx \
}

void static __inline
VPICD_Set_Int_Request(HIRQ hirq, HVM hvm)
{
    __asm mov eax,[hirq]
    __asm mov ebx,[hvm]
    VxDCall(VPICD_Set_Int_Request)
}

void static __inline
VPICD_Clear_Int_Request(HIRQ hirq, HVM hvm)
{
    __asm mov eax,[hirq]
    __asm mov ebx,[hvm]
    VxDCall(VPICD_Clear_Int_Request)
}

ULONG static __inline
VPICD_Get_Complete_Status(HIRQ hirq, HVM hvm)
{
    __asm mov eax,[hirq]
    __asm mov ebx,[hvm]
    VxDCall(VPICD_Get_Complete_Status)
    __asm xchg eax,ecx                  // trashed ECX for compiler
}

ULONG static __inline
VPICD_Get_Status(HIRQ hirq, HVM hvm)
{
    __asm mov eax,[hirq]
    __asm mov ebx,[hvm]
    VxDCall(VPICD_Get_Status)
    __asm xchg eax,ecx                  // trashed ECX for compiler
}

ULONG static __inline
VPICD_Test_Phys_Request(HIRQ hirq)
{
    __asm mov eax,[hirq]
    VxDCall(VPICD_Test_Phys_Request)
    __asm sbb eax,eax                   // EAX == 0, interrupt request clear
}

void static __inline
VPICD_Set_Auto_Masking(HIRQ hirq)
{
    __asm mov eax,[hirq]
    VxDCall(VPICD_Set_Auto_Masking)
}

ULONG static __inline
VPICD_Get_IRQ_Complete_Status(ULONG irqn)
{
    __asm xor ecx,ecx
    __asm mov eax,[irqn]
    VxDCall(VPICD_Get_IRQ_Complete_Status)
    __asm xchg eax,ecx                  // trashed ECX for compiler
}

ULONG static __inline
VPICD_Convert_Handle_To_IRQ(HIRQ hirq)
{
    __asm mov eax,[hirq]
    VxDCall(VPICD_Convert_Handle_To_IRQ)
    __asm xchg eax,esi                  // trashed ESI for compiler
}

ULONG static __inline
VPICD_Convert_IRQ_To_Int(ULONG irqn, HVM hvm)
{
    __asm {
        mov eax,[irqn]
        mov ebx,[hvm]
        VxDCall(VPICD_Convert_IRQ_To_Int)
        jnc short cii10
        xor eax,eax             // returns 0 if invalid IRQ number
    cii10:
    }
}

ULONG static __inline
VPICD_Convert_Int_To_IRQ(ULONG intn)
{
    __asm {
        mov eax,[intn]
        VxDCall(VPICD_Convert_Int_To_IRQ)
        jnc short cii10
        xor eax,eax
        dec eax                 // returns -1 if vector not mapped to any IRQ
    cii10:
    }
}

PFN static __inline
VPICD_Call_When_Hw_Int(PFN pfn)
{
    __asm mov esi,[pfn]
    VxDCall(VPICD_Call_When_Hw_Int)
    __asm mov eax,esi
}

ULONG static __inline
VPICD_Force_Default_Owner(HIRQ hirq, HVM hvm)
{
    __asm mov eax,[hirq]
    __asm mov ebx,[hvm]
    VxDCall(VPICD_Force_Default_Owner)
    __asm cmc
    __asm sbb eax,eax           // EAX != 0 success, EAX == 0 failure
}

void static __inline
VPICD_Force_Default_Behavior(HIRQ hirq)
{
    __asm mov eax,[hirq]
    VxDCall(VPICD_Force_Default_Behavior)
}

ULONG static __inline
VPICD_Auto_Mask_At_Inst_Swap(HIRQ hirq)
{
    __asm mov eax,[hirq]
    VxDCall(VPICD_Auto_Mask_At_Inst_Swap)
    __asm cmc
    __asm sbb eax,eax           // EAX != 0 success, EAX == 0 failure
}

void static __inline
VPICD_Begin_Inst_Page_Swap(void)
{
    VxDCall(VPICD_Begin_Inst_Page_Swap)
}

void static __inline
VPICD_End_Inst_Page_Swap(void)
{
    VxDCall(VPICD_End_Inst_Page_Swap)
}

ULONG static __inline
VPICD_Get_Virtualization_Count(ULONG intn)
{
    _asm mov    eax,[intn]
    VxDCall(VPICD_Get_Virtualization_Count)
}

#ifndef WANTVXDWRAPS

//HIRQ static __inline
HIRQ static
VPICD_Virtualize_IRQ(PVID pvid)
{
    __asm {
        mov edi,[pvid]
        VxDCall(VPICD_Virtualize_IRQ)
        jnc short vvi10
        xor eax,eax
    vvi10:
    }
}

void static __inline
VPICD_Phys_EOI(HIRQ hirq)
{
    __asm mov eax,[hirq]
    VxDCall(VPICD_Phys_EOI)
}

void static __inline
VPICD_Physically_Mask(HIRQ hirq)
{
    __asm mov eax,[hirq]
    VxDCall(VPICD_Physically_Mask)
}

void static __inline
VPICD_Physically_Unmask(HIRQ hirq)
{
    __asm mov eax,[hirq]
    VxDCall(VPICD_Physically_Unmask)
}

VOID static __inline
_VPICD_Clear_IR_Bits(DWORD dwIRQMask)
{
    _asm push   dwIRQMask
    VxDCall(_VPICD_Clear_IR_Bits)
    _asm add    esp, 4
}

#endif  // WANTVXDWRAPS

#pragma warning (default:4035)          // turn on no return code warning

/*XLATON*/

#endif  // ifndef _VPICD_H
